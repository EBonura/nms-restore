//! Rewrite NMS's assembler source (SN asmpsx syntax) as GNU as source.
//!
//!     asmpsx2gas IN.s OUT.s
//!
//! Syntax only: every instruction and data item stays as NMS wrote it, in the same order, assembled with
//! `.set noreorder` (NMS fills delay slots by hand). What changes:
//!
//!   ; comments           -> #
//!   label (column 0)     -> label:
//!   @local labels        -> .L<scope>_<name>, scoped by the last plain label
//!   $hex                 -> 0xhex
//!   registers            -> $name
//!   section .x           -> .section (.sdata/.sbss go to .data/.bss: no $gp)
//!   global x             -> .globl x
//!   rsreset / rb / rh / rw   structure offsets, as assignments (rh and rw align the offset first)
//!   name = expr          -> unchanged
//!   db / dh / dw         -> .byte (strings as .ascii) / .half / .word
//!   dcb / dch / dcw n,v  -> .fill n,1|2|4,v
//!   incbin "f"           -> .incbin "f"
//!   cnop 0,n             -> .balign n
//!   PUBLIC ON / OFF      -> every label defined while on is also .globl
//!   section name         -> a named section NMS made up (360X's `incbins`) is data: .section .data.name
//!   ori/andi/xori with a 32-bit immediate -> or/and/xor (GNU as then builds the constant in $at)
//!   sectend(.bss)        -> __bss_end
//!   div rd,rs,rt         -> the checked sequence asmpsx emitted (div, the divide-by-zero and overflow
//!                           breaks, then mflo)
//!   move                 -> addu rd, rs, $zero; li of a 16-bit value -> ori/addiu
//!   blt/bge/bgt/ble(u)   -> slt(u) $at + beqz/bnez
//!
//! GNU as expands the remaining macros (li, la, lw from a label, bgt, mul, rol). It replaces
//! `tools/asmpsx2gas.py` (same output; the first comment line names this tool).

use std::collections::HashMap;
use std::process::ExitCode;

const REGS: &[&str] = &["zero", "at", "v0", "v1", "a0", "a1", "a2", "a3", "k0", "k1", "gp", "sp", "fp", "ra"];

fn is_reg(name: &str) -> bool {
    if REGS.contains(&name) {
        return true;
    }
    let numbered = |prefix: char, max: u32| {
        name.strip_prefix(prefix)
            .filter(|n| !n.is_empty() && n.bytes().all(|b| b.is_ascii_digit()) && !(n.len() > 1 && n.starts_with('0')))
            .and_then(|n| n.parse::<u32>().ok())
            .is_some_and(|n| n < max)
    };
    numbered('t', 10) || numbered('s', 9) || numbered('r', 32)
}

fn is_word(c: char) -> bool {
    c.is_alphanumeric() || c == '_'
}

/// The line up to a `;` outside quotes.
fn split_comment(line: &str) -> String {
    let (mut out, mut quote) = (String::new(), None);
    for ch in line.chars() {
        if let Some(q) = quote {
            out.push(ch);
            if ch == q {
                quote = None;
            }
        } else if ch == '\'' || ch == '"' {
            quote = Some(ch);
            out.push(ch);
        } else if ch == ';' {
            break;
        } else {
            out.push(ch);
        }
    }
    out
}

/// Split an operand list on commas outside quotes and parentheses.
fn operands(text: &str) -> Vec<String> {
    let (mut parts, mut cur, mut depth, mut quote) = (Vec::new(), String::new(), 0i32, None);
    for ch in text.chars() {
        if let Some(q) = quote {
            cur.push(ch);
            if ch == q {
                quote = None;
            }
            continue;
        }
        match ch {
            '\'' | '"' => quote = Some(ch),
            '(' => depth += 1,
            ')' => depth -= 1,
            ',' if depth == 0 => {
                parts.push(cur.trim().to_string());
                cur.clear();
                continue;
            }
            _ => {}
        }
        cur.push(ch);
    }
    if !cur.trim().is_empty() {
        parts.push(cur.trim().to_string());
    }
    parts
}

/// `$hex` -> `0xhex`.
fn dollar_hex(text: &str) -> String {
    let chars: Vec<char> = text.chars().collect();
    let mut out = String::new();
    let mut i = 0;
    while i < chars.len() {
        if chars[i] == '$' && i + 1 < chars.len() && chars[i + 1].is_ascii_hexdigit() {
            out.push_str("0x");
            i += 1;
            while i < chars.len() && chars[i].is_ascii_hexdigit() {
                out.push(chars[i]);
                i += 1;
            }
        } else {
            out.push(chars[i]);
            i += 1;
        }
    }
    out
}

/// A constant expression as the original script's `eval` read it: integers (decimal or 0x), known symbols,
/// `+ - * // % << >> & | ^ ~`, parentheses. `None` for anything else (a register, a label).
struct Eval<'a> {
    chars: Vec<char>,
    at: usize,
    syms: &'a HashMap<String, i64>,
}

impl Eval<'_> {
    fn ws(&mut self) {
        while self.at < self.chars.len() && self.chars[self.at].is_whitespace() {
            self.at += 1;
        }
    }
    fn peek(&mut self) -> Option<char> {
        self.ws();
        self.chars.get(self.at).copied()
    }
    fn eat(&mut self, s: &str) -> bool {
        self.ws();
        let want: Vec<char> = s.chars().collect();
        if self.chars[self.at..].starts_with(&want) {
            self.at += want.len();
            true
        } else {
            false
        }
    }
    fn or(&mut self) -> Option<i64> {
        let mut v = self.xor()?;
        while self.peek() == Some('|') {
            self.at += 1;
            v |= self.xor()?;
        }
        Some(v)
    }
    fn xor(&mut self) -> Option<i64> {
        let mut v = self.and()?;
        while self.peek() == Some('^') {
            self.at += 1;
            v ^= self.and()?;
        }
        Some(v)
    }
    fn and(&mut self) -> Option<i64> {
        let mut v = self.shift()?;
        while self.peek() == Some('&') {
            self.at += 1;
            v &= self.shift()?;
        }
        Some(v)
    }
    fn shift(&mut self) -> Option<i64> {
        let mut v = self.sum()?;
        loop {
            if self.eat("<<") {
                v = v.checked_shl(u32::try_from(self.sum()?).ok()?)?;
            } else if self.eat(">>") {
                v >>= self.sum()?.clamp(0, 63);
            } else {
                return Some(v);
            }
        }
    }
    fn sum(&mut self) -> Option<i64> {
        let mut v = self.term()?;
        loop {
            match self.peek() {
                Some('+') => {
                    self.at += 1;
                    v = v.checked_add(self.term()?)?;
                }
                Some('-') => {
                    self.at += 1;
                    v = v.checked_sub(self.term()?)?;
                }
                _ => return Some(v),
            }
        }
    }
    fn term(&mut self) -> Option<i64> {
        let mut v = self.unary()?;
        loop {
            if self.eat("//") {
                let d = self.unary()?;
                if d == 0 {
                    return None;
                }
                v = v.div_euclid(d) - i64::from(d < 0 && v.rem_euclid(d) != 0);
            } else if self.peek() == Some('*') {
                self.at += 1;
                v = v.checked_mul(self.unary()?)?;
            } else if self.peek() == Some('%') {
                self.at += 1;
                let d = self.unary()?;
                if d == 0 {
                    return None;
                }
                v = ((v % d) + d) % d;
            } else {
                return Some(v);
            }
        }
    }
    fn unary(&mut self) -> Option<i64> {
        match self.peek()? {
            '-' => {
                self.at += 1;
                self.unary()?.checked_neg()
            }
            '+' => {
                self.at += 1;
                self.unary()
            }
            '~' => {
                self.at += 1;
                Some(!self.unary()?)
            }
            '(' => {
                self.at += 1;
                let v = self.or()?;
                (self.peek()? == ')').then(|| self.at += 1)?;
                Some(v)
            }
            c if c.is_ascii_digit() => {
                let start = self.at;
                if self.chars[self.at..].starts_with(&['0', 'x']) || self.chars[self.at..].starts_with(&['0', 'X']) {
                    self.at += 2;
                    let s = self.at;
                    while self.at < self.chars.len() && self.chars[self.at].is_ascii_hexdigit() {
                        self.at += 1;
                    }
                    let text: String = self.chars[s..self.at].iter().collect();
                    return i64::from_str_radix(&text, 16).ok();
                }
                while self.at < self.chars.len() && self.chars[self.at].is_ascii_digit() {
                    self.at += 1;
                }
                let text: String = self.chars[start..self.at].iter().collect();
                text.parse().ok()
            }
            c if c.is_alphabetic() || c == '_' => {
                let start = self.at;
                while self.at < self.chars.len() && is_word(self.chars[self.at]) {
                    self.at += 1;
                }
                let name: String = self.chars[start..self.at].iter().collect();
                self.syms.get(&name).copied()
            }
            _ => None,
        }
    }
}

fn eval(text: &str, syms: &HashMap<String, i64>) -> Option<i64> {
    let mut e = Eval { chars: text.replace('$', "").chars().collect(), at: 0, syms };
    let v = e.or()?;
    (e.peek().is_none()).then_some(v)
}

fn int_literal(text: &str) -> Option<i64> {
    let (neg, body) = text.strip_prefix('-').map_or((false, text), |b| (true, b));
    let v = if let Some(hex) = body.strip_prefix("0x") {
        if hex.is_empty() || !hex.bytes().all(|b| b.is_ascii_hexdigit()) {
            return None;
        }
        i64::from_str_radix(hex, 16).ok()?
    } else if !body.is_empty() && body.bytes().all(|b| b.is_ascii_digit()) {
        body.parse().ok()?
    } else {
        return None;
    };
    Some(if neg { -v } else { v })
}

struct Translator {
    scope: String,
    rs: i64,
    syms: HashMap<String, i64>,
    public: bool,
    out: Vec<String>,
}

impl Translator {
    fn new() -> Self {
        Self {
            scope: "top".into(),
            rs: 0,
            syms: HashMap::new(),
            public: false,
            out: vec![
                "# Generated by tools/asmpsx2gas from NMS asmpsx source. Do not edit.".into(),
                "\t.set noreorder".into(),
                "\t.set at".into(),
            ],
        }
    }

    fn expr(&self, text: &str) -> String {
        let text = text.replace("sectend(.bss)", "__bss_end");
        let text = dollar_hex(&text);
        // @name -> .L<scope>_<name>
        let chars: Vec<char> = text.chars().collect();
        let mut at_done = String::new();
        let mut i = 0;
        while i < chars.len() {
            if chars[i] == '@' && i + 1 < chars.len() && is_word(chars[i + 1]) {
                let mut j = i + 1;
                while j < chars.len() && is_word(chars[j]) {
                    j += 1;
                }
                at_done.push_str(&format!(".L{}_", self.scope));
                at_done.extend(&chars[i + 1..j]);
                i = j;
            } else {
                at_done.push(chars[i]);
                i += 1;
            }
        }
        // registers, outside quoted strings
        let chars: Vec<char> = at_done.chars().collect();
        let mut out = String::new();
        let mut i = 0;
        while i < chars.len() {
            let c = chars[i];
            if c == '"' || c == '\'' {
                if let Some(rel) = chars[i + 1..].iter().position(|&x| x == c) {
                    let end = i + 1 + rel;
                    out.extend(&chars[i..=end]);
                    i = end + 1;
                    continue;
                }
            }
            let prev_ok = i == 0 || !(is_word(chars[i - 1]) || chars[i - 1] == '.' || chars[i - 1] == '$');
            if c.is_ascii_lowercase() && prev_ok {
                let mut j = i + 1;
                while j < chars.len() && (chars[j].is_ascii_lowercase() || chars[j].is_ascii_digit()) {
                    j += 1;
                }
                if j == chars.len() || !is_word(chars[j]) {
                    let word: String = chars[i..j].iter().collect();
                    if is_reg(&word) {
                        out.push('$');
                    }
                    out.push_str(&word);
                    i = j;
                    continue;
                }
            }
            out.push(c);
            i += 1;
        }
        out
    }

    fn value(&self, text: &str) -> Option<i64> {
        eval(text, &self.syms)
    }

    fn data(&mut self, op: &str, args: &str) -> Result<(), String> {
        match op {
            "db" => {
                for a in operands(args) {
                    let first = a.chars().next().unwrap_or(' ');
                    if first == '\'' || first == '"' {
                        let inner: String = a.chars().skip(1).take(a.chars().count().saturating_sub(2)).collect();
                        self.out.push(format!("\t.ascii \"{inner}\""));
                    } else {
                        let e = self.expr(&a);
                        self.out.push(format!("\t.byte {e}"));
                    }
                }
            }
            "dh" | "dw" => {
                let items: Vec<String> = operands(args).iter().map(|a| self.expr(a)).collect();
                self.out.push(format!("\t{} {}", if op == "dh" { ".half" } else { ".word" }, items.join(", ")));
            }
            _ => {
                let ops = operands(args);
                let [n, v] = ops.as_slice() else { return Err(format!("{op} takes two operands: {args}")) };
                let size = match op {
                    "dcb" => 1,
                    "dch" => 2,
                    _ => 4,
                };
                self.out.push(format!("\t.fill {}, {size}, {}", self.expr(n), self.expr(v)));
            }
        }
        Ok(())
    }

    fn line(&mut self, raw: &str) -> Result<(), String> {
        let line = split_comment(raw.trim_end_matches(['\r', '\n']));
        let line = line.trim_end().to_string();
        if line.trim().is_empty() {
            return Ok(());
        }
        let mut label: Option<String> = None;
        let mut rest = line.clone();
        if !line.starts_with(char::is_whitespace) {
            let (first, tail) = split_once_ws(&line);
            label = Some(first.trim_end_matches(':').to_string());
            rest = tail.to_string();
        }
        let (op_raw, args_raw) = split_once_ws(&rest);
        let op = op_raw.to_lowercase();
        let args = args_raw.trim().to_string();
        let has_label_eq = label.as_deref().is_some_and(|l| l.ends_with('='));
        if matches!(op.as_str(), "rb" | "rh" | "rw") {
            let size = match op.as_str() {
                "rb" => 1,
                "rh" => 2,
                _ => 4,
            };
            let label = label.ok_or("rs item without a label")?;
            self.rs = (self.rs + size - 1) / size * size;
            self.out.push(format!("{label} = {}", self.rs));
            self.syms.insert(label, self.rs);
            let count = eval(&dollar_hex(&args), &self.syms).ok_or_else(|| format!("cannot evaluate {args}"))?;
            self.rs += size * count;
            return Ok(());
        }
        if op == "rsreset" {
            self.rs = 0;
            return Ok(());
        }
        if op == "=" || has_label_eq {
            let label = label.unwrap_or_default();
            let e = self.expr(&args);
            self.out.push(format!("{label} = {e}"));
            if let Some(v) = self.value(&e) {
                self.syms.insert(label, v);
            }
            return Ok(());
        }
        if let Some(label) = &label {
            if let Some(local) = label.strip_prefix('@') {
                self.out.push(format!(".L{}_{local}:", self.scope));
            } else {
                self.scope = label.clone();
                if self.public {
                    self.out.push(format!("\t.globl {label}"));
                }
                self.out.push(format!("{label}:"));
            }
        }
        if op.is_empty() {
            return Ok(());
        }
        match op.as_str() {
            "section" => {
                let name = args.split_whitespace().next().unwrap_or("").split(',').next().unwrap_or("").to_string();
                let known = match name.as_str() {
                    ".text" => Some(".text"),
                    ".data" | ".sdata" => Some(".data"),
                    ".rdata" => Some(".rodata"),
                    ".bss" | ".sbss" => Some(".bss"),
                    _ => None,
                };
                match known {
                    Some(k) => self.out.push(format!("\t.section {k}")),
                    None => self.out.push(format!("\t.section .data.{}, \"aw\"", name.trim_start_matches('.'))),
                }
            }
            "public" => self.public = args.split_whitespace().next().is_some_and(|w| w.eq_ignore_ascii_case("on")),
            "cnop" => {
                let ops = operands(&args);
                let [offset, align] = ops.as_slice() else { return Err(format!("cnop takes two operands: {args}")) };
                let (offset, align) = (self.expr(offset), self.expr(align));
                self.out.push(format!("\t.balign {align}{}", if offset == "0" { String::new() } else { format!("\n\t.space {offset}") }));
            }
            "global" => self.out.push(format!("\t.globl {args}")),
            "db" | "dh" | "dw" | "dcb" | "dch" | "dcw" => self.data(&op, &args)?,
            "incbin" => self.out.push(format!("\t.incbin {args}")),
            "div" if operands(&args).len() == 3 => {
                let ops: Vec<String> = operands(&args).iter().map(|a| self.expr(a)).collect();
                let (rd, rs, rt) = (&ops[0], &ops[1], &ops[2]);
                let n = self.out.len();
                self.out.extend(
                    [
                        "\t.set noat".to_string(),
                        format!("\tdiv $zero, {rs}, {rt}"),
                        format!("\tbnez {rt}, .Ldiv{n}_a"),
                        "\tnop".into(),
                        "\tbreak 7".into(),
                        format!(".Ldiv{n}_a:"),
                        "\tli $at, -1".into(),
                        format!("\tbne {rt}, $at, .Ldiv{n}_b"),
                        "\tlui $at, 0x8000".into(),
                        format!("\tbne {rs}, $at, .Ldiv{n}_b"),
                        "\tnop".into(),
                        "\tbreak 6".into(),
                        format!(".Ldiv{n}_b:"),
                        format!("\tmflo {rd}"),
                        "\t.set at".into(),
                    ],
                );
            }
            _ => {
                let mut op = op.clone();
                let mut ops: Vec<String> = operands(&args).iter().map(|a| self.expr(a)).collect();
                if op == "move" {
                    op = "addu".into();
                    ops.push("$zero".into());
                } else if op == "li" {
                    let value = ops.get(1).and_then(|o| self.value(o));
                    match value {
                        Some(v) if (0..=0xffff).contains(&v) => {
                            op = "ori".into();
                            ops = vec![ops[0].clone(), "$zero".into(), ops[1].clone()];
                        }
                        Some(v) if (-0x8000..0).contains(&v) => {
                            op = "addiu".into();
                            ops = vec![ops[0].clone(), "$zero".into(), ops[1].clone()];
                        }
                        _ => {}
                    }
                } else if let Some((slt, swap, test)) = branch(&op) {
                    if ops.len() < 3 {
                        return Err(format!("{op} takes three operands: {args}"));
                    }
                    let (a, b) = if swap { (&ops[1], &ops[0]) } else { (&ops[0], &ops[1]) };
                    self.out.push("\t.set noat".into());
                    self.out.push(format!("\t{slt} $at, {a}, {b}"));
                    self.out.push(format!("\t{test} $at, {}", ops[2]));
                    self.out.push("\t.set at".into());
                    return Ok(());
                }
                if matches!(op.as_str(), "ori" | "andi" | "xori") && ops.len() == 3 {
                    if let Some(v) = int_literal(&ops[2]) {
                        if !(-0x8000..=0xffff).contains(&v) {
                            op.pop(); // asmpsx took a 32-bit immediate here (lui $at + the register form)
                        }
                    }
                }
                self.out.push(format!("\t{op} {}", ops.join(", ")));
            }
        }
        Ok(())
    }
}

/// `blt`..`bleu`: the compare instruction, whether the operands swap, and the branch on `$at`.
fn branch(op: &str) -> Option<(&'static str, bool, &'static str)> {
    Some(match op {
        "blt" => ("slt", false, "bnez"),
        "bge" => ("slt", false, "beqz"),
        "bgt" => ("slt", true, "bnez"),
        "ble" => ("slt", true, "beqz"),
        "bltu" => ("sltu", false, "bnez"),
        "bgeu" => ("sltu", false, "beqz"),
        "bgtu" => ("sltu", true, "bnez"),
        "bleu" => ("sltu", true, "beqz"),
        _ => return None,
    })
}

/// `s.split(None, 1)`: the first whitespace-separated word and the rest without its leading whitespace.
fn split_once_ws(s: &str) -> (&str, &str) {
    let s = s.trim_start();
    match s.find(char::is_whitespace) {
        Some(i) => (&s[..i], s[i..].trim_start()),
        None => (s, ""),
    }
}

fn main() -> ExitCode {
    let args: Vec<String> = std::env::args().collect();
    if args.len() != 3 {
        eprintln!("usage: asmpsx2gas IN.s OUT.s");
        return ExitCode::from(2);
    }
    let bytes = match std::fs::read(&args[1]) {
        Ok(b) => b,
        Err(e) => {
            eprintln!("asmpsx2gas: {}: {e}", args[1]);
            return ExitCode::from(1);
        }
    };
    // The sources are latin-1.
    let text: String = bytes.iter().map(|&b| char::from(b)).collect();
    let mut t = Translator::new();
    for (n, raw) in text.split_inclusive('\n').enumerate() {
        if let Err(e) = t.line(raw) {
            eprintln!("asmpsx2gas: {}:{}: {e}", args[1], n + 1);
            return ExitCode::from(1);
        }
    }
    let mut out = t.out.join("\n");
    out.push('\n');
    if let Err(e) = std::fs::write(&args[2], out) {
        eprintln!("asmpsx2gas: {}: {e}", args[2]);
        return ExitCode::from(1);
    }
    ExitCode::SUCCESS
}

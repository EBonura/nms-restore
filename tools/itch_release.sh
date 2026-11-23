#!/usr/bin/env bash
# Stage an installed disc of 360X or QD Up for itch.io, and push it with butler.
#
#   tools/itch_release.sh 360x 1.0.0           stage and check it, print the push it would make
#   tools/itch_release.sh 360x 1.0.0 --push    the same, then push it to the game's itch page
#
# Nothing is built here. The disc can't be built on a CI runner either: the game data stays in
# the original archives, outside this repository. What ships is the disc already installed in
# the PS1 library (PS1_LIBRARY, default "~/Downloads/ps1 games"), the one that passed the full
# test run. Its provenance file names the commit it came from and the sha256 of every file, and
# this script refuses to stage it unless:
#   - the provenance names this repository and a clean tree,
#   - that commit is on origin/main (or ITCH_SOURCE_COMMIT names the one it was squashed into),
#   - the .bin on disk has the sha256 the provenance names.
#
# The staged folder (build/itch/<game>-<version>/) holds the .bin, a .cue pointing at it, the
# player's README.txt (tools/itch/<game>.txt) and SHA256SUMS.txt. butler pushes it to
# bonnie-studios/<page>:psx with the version as the user version. butler must be logged in
# (`butler login`) or have BUTLER_API_KEY set.
set -euo pipefail

usage() { echo "usage: $0 <360x|qdup> <version> [--push]" >&2; exit 2; }
[[ $# -ge 2 && $# -le 3 ]] || usage
game=$1 version=${2#v} push=no
if [[ $# -eq 3 ]]; then [[ $3 == --push ]] || usage; push=yes; fi

case $game in
  360x) title="360X"  page="360x"  short="360x" ;;
  qdup) title="QD Up" page="qd-up" short="qdup" ;;
  *) usage ;;
esac
[[ $version =~ ^[0-9]+\.[0-9]+\.[0-9]+(-[0-9A-Za-z.-]+)?$ ]] ||
  { echo "version must look like 1.0.0 or 1.0.0-rc.1, got '$version'" >&2; exit 2; }

repo=$(cd "$(dirname "$0")/.." && pwd)
library=${PS1_LIBRARY:-$HOME/Downloads/ps1 games}
name="$title (nms-restore)"
bin="$library/$name.bin"
provenance="$library/$name.provenance.json"
target="bonnie-studios/$page:psx"
out="$repo/build/itch/$game-$version"

for tool in jq butler shasum; do
  command -v $tool >/dev/null || { echo "needs $tool" >&2; exit 1; }
done
[[ -f $bin && -f $provenance ]] || { echo "no installed disc at $library/$name.*" >&2; exit 1; }

repository=$(jq -r .source.repository "$provenance")
dirty=$(jq -r .source.dirty "$provenance")
commit=$(jq -r .source.commit "$provenance")
want=$(jq -r --arg f "$name.bin" '.files[$f]' "$provenance")
[[ $repository == EBonura/nms-restore ]] || { echo "provenance names $repository" >&2; exit 1; }
[[ $dirty == false ]] || { echo "the installed disc was built from a dirty tree" >&2; exit 1; }

# A disc installed before the history was squashed (2026-11-23) names a commit that is no longer
# on main. ITCH_SOURCE_COMMIT names the main commit to credit instead; it must be on main, and
# the provenance's sha256 check below still ties the release to the tested bytes.
git -C "$repo" fetch -q origin main
if ! git -C "$repo" merge-base --is-ancestor "$commit" FETCH_HEAD 2>/dev/null; then
  [[ -n ${ITCH_SOURCE_COMMIT:-} ]] ||
    { echo "commit $commit is not on origin/main (set ITCH_SOURCE_COMMIT to the main commit it was squashed into)" >&2; exit 1; }
  git -C "$repo" merge-base --is-ancestor "$ITCH_SOURCE_COMMIT" FETCH_HEAD ||
    { echo "ITCH_SOURCE_COMMIT $ITCH_SOURCE_COMMIT is not on origin/main" >&2; exit 1; }
  echo "note: installed from ${commit:0:12}, which is not on main; crediting $ITCH_SOURCE_COMMIT" >&2
  commit=$(git -C "$repo" rev-parse "$ITCH_SOURCE_COMMIT")
fi

have=$(shasum -a 256 "$bin" | cut -d' ' -f1)
[[ $have == "$want" ]] || { echo "$bin: sha256 $have, provenance says $want" >&2; exit 1; }

rm -rf "$out"
mkdir -p "$out"
cp "$bin" "$out/$short.bin"
printf 'FILE "%s.bin" BINARY\n  TRACK 01 MODE2/2352\n    INDEX 01 00:00:00\n' "$short" > "$out/$short.cue"
sed -e "s/@VERSION@/$version/g" -e "s/@COMMIT@/${commit:0:12}/g" \
  "$repo/tools/itch/$game.txt" > "$out/README.txt"
(cd "$out" && shasum -a 256 "$short.bin" "$short.cue" README.txt > SHA256SUMS.txt)

echo "staged $title $version from ${commit:0:12} in $out"
cat "$out/SHA256SUMS.txt"

if [[ $push == no ]]; then
  echo "dry run; to publish: butler push \"$out\" $target --userversion $version"
  exit 0
fi
butler status "${target%:*}" >/dev/null ||
  { echo "butler can't see ${target%:*}: create the page on itch.io first" >&2; exit 1; }
butler push "$out" "$target" --userversion "$version"

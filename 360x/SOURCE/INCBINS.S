

;***************************************
;*      INCLUDE ALL INCBINS HERE       *
;**************************************/

	 	PUBLIC ON					;MAKE THESE INCBINS GLOBAL TO 'C'
		section incbins

;/**************************************
;*         INCLUDE MATHS TABLES        *
;**************************************/

		cnop 0,4
sqrtable	incbin	"SQRTABLE.BIN"

		cnop 0,4
sintable	incbin	"SINTABLE.BIN"

;/**************************************
;*          RESIDENT SPRITES           *
;**************************************/

		cnop 0,4
fonts	incbin	"..\GRAPHICS\MISC\FONTS\FONTS.TIM"



include \masm32\include\masm32rt.inc	;assembler file
include game.inc
.code

main proc
	invoke load		;move the pictures we need to the ram from the hard disk
	;start infinite loop
	loopi:
		invoke draw
		invoke update
	jmp loopi
ret
main endp
end main
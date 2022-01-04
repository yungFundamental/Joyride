include drd.inc
includelib drd.lib

drawDigit proc x:dword, y:dword, value:dword		;(0<=value<=9)
;draw value in the x and y coordinates

	mov eax, 8
	mul value
	;eax = value*8 (index of values digit picture in dgtArray)
	invoke drd_imageDrawCrop, offset res.dgtStrip, x, y, dgtArr[eax],0,dgtArr[eax+4],59		;draw digit

ret
drawDigit endp

drawScore proc x:dword, y:dword, adrScore:dword
;draws score with first digit at given coordinates and the rest to the right of it
	mov ebx, adrScore
	ASSUME ebx: ptr Score

	invoke drawDigit, x, y, [ebx].thou
	add x, 20
	invoke drawDigit, x, y, [ebx].hndrds
	add x, 20
	invoke drawDigit, x, y, [ebx].tens
	add x, 20
	invoke drawDigit, x, y, [ebx].ones
	
	ASSUME ebx:nothing
ret
drawScore endp
	
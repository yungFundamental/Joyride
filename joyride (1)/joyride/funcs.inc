.code
.686
.MODEL flat, STDCALL


accelerate proc  key:dword			
;checks if key is pressed, changes speed and animates in accordance 

	invoke GetAsyncKeyState, key	;windows procedure: if key is pressed eax!=0 else eax==0
	cmp eax,0
	jne PRES
	;if key is not pressed (gravity takes place)
		add spd, acr	;speed towards the ground increases
		mov recXFly,8	;cat is in mode 2 of fly animation
	ret

PRES:	
;if key is spaced (jetpack is used)
	sub spd,acr		;speed towards the roof increases
	mov recXFly,0	;cat is in mode 1 of fly animation (legs straight
ret
accelerate endp

moveCat proc
; moves cats y coordinate between floorY and roofY in accordance to speed
	mov ebx, spd
	add ebx, charPos.y
	;ebx=speed+posY=where the character is supposed to be at the next frame
	cmp ebx,roofY	
	jle HITHEAD		;if character will be above the roof next frame, jump to HITHEAD
		cmp ebx, floorY
		jge RUNNING	;if character will be below the floor next frame, jump to RUNNING
			;if cat wont hit head or hit the floor next frame, set his y as predicted
			mov charPos.y, ebx
		ret
HITHEAD:	;if cat reached max y
mov charPos.y, roofY	;set y to max
mov spd, 0				;restart speed to 0
ret
RUNNING:	;if cat reached min y
mov charPos.y, floorY	;set y to min
mov spd, 0				;restart speed to 0
ret
moveCat endp


animRun proc	;animates the cat running

	;slow anim will be increased untill he equals runAnimPace(defined value) (anim takes place once every runSlowAnim frames)
	inc slowAnim
	cmp slowAnim, runAnimPace
	jl DONT		;if slowAnim is less than runAnimPace
	mov slowAnim,0		;return slowAnim back to 0
		
	cmp recXRun, 32		;check if cat was on his last animation picture
	je NULLIFY			;if he was
	;if not:
	add recXRun,8	;add 8 to run animation strip array (in other words, set animation as next animation picture)
ret

NULLIFY:	;if cat animation was last animation picture
mov recXRun,0	;set cat animation as first animation picture
ret

DONT:	;if slowAnim is less than runAnimPace(defined value) dont change cat animation
ret		;don't change cat animation picture

animRun endp



collided proc  adrObj1Pos:dword, adrObj2Pos:dword, adrFlag:dword		;checks if obj1 and obj2 have collided
;(if yes then flag is set to 1)
	
	mov ebx, adrObj1Pos		;ebx contains adress of object number 1
	mov ecx, adrObj2Pos		;ecx contains adress of object number 2

	;next, we allow the compiler to assume that these registers contain objPos struct pointers
	ASSUME ebx: ptr objPos
	ASSUME ecx: ptr objPos

	;next, we start checking if the objects collided by comparing the different edges of both pictures
	;(since the stack/data variables can't be compared to each other we use registers)

	;x axle:
	mov edi, [ecx].x
	add edi, [ecx].wdth
	cmp [ebx].x, edi ;comparing left side of object1 to right side of object2(edi)
	jg DIDNTCOLLIDE  ;if left side of object 1 is more right then right side of object2, then they definetly did not collide

	;vice versa:
	mov edi, [ebx].x
	add edi, [ebx].wdth
	cmp edi, [ecx].x		;comparing right side of object 1(edi) to left side of object 2
	jl DIDNTCOLLIDE  ;if right side of object 1 is more left then left side of object2, then they definetly did not collide

	;y axle:
	mov edi,[ecx].y  
	add edi, [ecx].height
	cmp [ebx].y, edi  ;comparing top of object1 to bottom of object2(edi)
	jg DIDNTCOLLIDE  ;if top of object 1 is beneath the bottom of object 2, then they definetly did not collide

	;vise versa
	mov edi, [ebx].y
	add edi, [ebx].height
	cmp edi,[ecx].y		;comparing bottom of object 1 to top of object 2
	jl DIDNTCOLLIDE  ;if bottom of object 1 is above the top of object 2, then they definetly did not collide

;if we reach these lines while running, the objects collided
;document the collision by using the given flag:
mov edi,adrFlag ;edi contains address of flag
mov eax, 1		;eax contains 1 (represents boolean collided)
mov [edi], eax	;insert 1 to flag

DIDNTCOLLIDE:	;jump to here if the objects didnt collide (these lines will be reached if they did collide as well)
;return ebx and ecx to their previous state and return
ASSUME ebx: nothing	
ASSUME ecx: nothing
ret
collided endp



revive proc	key:dword			;checks if key is pressed, if so then it restarts the data to its starting state
;(intended for reviving cat)

	invoke GetAsyncKeyState, key	;windows procedure: if key is pressed eax!=0 else eax==0
	cmp eax, 0
	je STILLDEAD	;if key is not pressed jump to STILLDEAD and dont do anything

	mov catDead, 0	;document the revivaval of the cat

	mov charPos.y, floorY	;return the cat to the floor
	mov spd, 0				;speed restarts to 0
	
	;the obstacles are restarted to edge of the screen and farther in order to give the user time to react
	mov obst1.x, widthbg		
	mov obst2.x, widthbg+500

	;restart the current score to 0
	mov current.valu, 0
	mov current.ones,0
	mov current.tens,0
	mov current.hndrds,0
	mov current.thou,0

STILLDEAD:		;jump to here if r is not pressed
ret
revive endp


getRandNum proc min:dword, max:dword	;sets edx as a random number between min and max (including) 
	;;0<=rndNum<=(range-1)

	;puts in eax a random num between 0,2^32
	rdrand eax 

	;ecx= max-min+1
	mov ecx, max
	sub ecx,min
	inc ecx
	;;edx = 0
	mov edx,0

	div ecx ;(edx:eax)%ecx -> edx

	;;; 0<=edx<=max-min
	add edx, min
	;;; min<=edx<=max
	
ret
getRandNum endp

movObst proc adrObst:dword, edge:dword		;moves the obstacle x position one left unless obst reaches the edge

	mov ebx, adrObst		;ebx contains the address of the obstacle
	ASSUME ebx: PTR objPos
	
	mov ecx, edge			
	cmp [ebx].x, ecx		;check if obstacle reached its edge coordinate
	jle BACK				;if so, jump to back line
	dec [ebx].x				;else, move zapper one pixel left
	jmp ENDING				;and jump to ending


BACK:				;if edge has been reached
mov [ebx].x, widthbg	;return the obstacle to the edge of the screen
;and randomize its y coordinate
invoke getRandNum, roofY, floorY
mov [ebx].y, edx
ENDING:
ASSUME ebx: nothing
ret
movObst endp

scoreAdd proc adrDig1:dword, adrDig2:dword, adrDig3:dword, adrDig4:dword, total:dword
;score proc changes digits to make the number that is built by all of them greater by 1
;score proc IS NOT RESPONSIBLE FOR INCREASING TOTAL POINTS BY ONE OR CHECKING IF SCORE HAS REACHED MAXSCORE
	

	mov ebx, adrDig1	;ebx contains the address of first digit 
	ASSUME ebx: PTR dword
	
	;check if the first digit has reached the last digit (9)
	cmp [ebx],9		
	je NEXTDIG	;if so jump to NEXTDIG
	inc [ebx]	;else increase dig1 value by 1
	jmp RETURNEBX	;jump to the end of the procedure

NEXTDIG:	;if dig1 has reached its maximum value
mov [ebx],0	;restart the digit
invoke scoreAdd, adrDig2, adrDig3, adrDig4, 0, total	;and add one to the other digits (digit4 will not be reached since total<max)
 RETURNEBX:
	ASSUME ebx:nothing	;return ebx to its previous state (must be at the end of the procedure to avoid compilation errors)
ret
scoreAdd endp


setDigit proc dgtNew:dword, adrDgtOld:dword	;sets the value of old digit as value of new digit
	mov ebx, adrDgtOld
	mov ecx, dgtNew
	mov [ebx], ecx
ret
setDigit endp

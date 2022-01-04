.const
acr equ 1	 ;acceleration irregardles of direction
floorY equ 400	;y position of the floor (as in max value of the y character)
roofY equ 100	;y position of the roof (as in min value of the y character)
widthbg equ 1076	;width of the background picture
heightbg equ 579	;height of the background picture

edgeObst equ -35	 ;should be a bit less than 0 (reason in movObst function)

maxScore equ 9999	;max possible score

difficulty equ 2	;higher the value - easier the game (more explaination in game.inc)
slowPoints equ 5	;higher the value - slower the score goes up
updatePace equ 35	;higher the value - slower update procedure (score, movement and animation pace)
runAnimPace equ 100	;higher the value - slower run animation

;Resources struct contains the paths to pictures on the hard disk and Img structs to store the pictures in the ram
Resources struct
	bgpath byte "pics\backg.png",0				;background picture
	dngrPath byte "pics\Zapper.png",0			;zapper picture
	deadmsgPath byte "pics\deadmsg.png",0		;message that appears after user dies
	dgtStripPath byte "pics\digitStrip.png",0	;contains a strip of all decimel digits
	highScorePath byte "pics\High Score.png",0	;contains the text "HIGH SCORE:"
	bg Img<>
	dngr Img<>
	deathMsg Img<>
	dgtStrip Img<>
	highScore Img<>
Resources ends


Score struct
	valu dword ?		;value of the score
	ones dword ?		;ones digit
	tens dword ?		;tens digit
	hndrds dword ?		;hundreds digit
	thou dword ?		;thousands digit
Score ends 

;path to cat character photos on hard disk
HDcat struct
	fly byte "pics/CATFLY.png",0		;strip of fly animation
	run byte "pics/CATRUN.png",0		;strip of run animation
HDcat ends

;will contain the pictures in the ram of the cat character
RAM struct
	fly Img<>
	run Img<>
RAM ends

;object size and placement
objPos struct
	x dword ?		;x position
	y dword ?		;y position
	wdth dword ?	;width
	height dword ?	;height
objPos ends



.data

obst1 objPos<1076,150,30,119>		;zapper 1
obst2 objPos<1500,340,30,119>		;zapper 2
charPos objPos<100,floorY,32,53>	;cat character


slowUpdate dword 0		;used to slow update procedure
slowBg dword 0			;used to slow the pace of background
slowAnim dword 0		;used to slow running animation
slowScore dword 0		;used to slow score

xbg dword 0				;used for background animation (describes how much to crop from background pic)

spd dword 0				;speed of character(y axle)

shmulik HDcat<>			;paths to character pictures in hard disk
shmulRM RAM<>			;character pictures in ram
res Resources<>

recXFly dword 0			;describes which animation state cat is in (if in air)
recXRun dword 0			;describes which animation state cat is in (if on floor)


;the following arrays include instructions of cropping specific pictures out of strips:
;picture n: x coordinate of start of picture in strip is in idx(n*2), width of picture in idx(n*2+1)
;(since the arrays contain: dwords x coordinate=idx(n*8), width=idx(n*8+1))
catFlies dword 4 dup(0,60,60,55)								;cat fly animation strip
catRuns dword  8 dup(0,47,52,41,109,48,161,40,207,54)			;cat run animation strip
dgtArr dword 20 dup(0,20,26,11,44,20,70,17,88,20,112,18,132,20,156,18,177,18,199,19)	;digit strip (digit = idx(digit*8+1))

catDead byte 0			 ;boolean describes if cat is dead (0=ALIVE,1=DEAD)

current Score<0,0,0,0,0>	;score of current run
HISC Score<0,0,0,0,0>		;high score




;include appropriate files
include drd.inc
includelib drd.lib
include data.inc
include funcs.inc
include drdFuncs.inc
.code



load proc		;load game
	invoke drd_init,widthbg,heightbg,0		;create screen
		;load the images from the hard disk into the ram:
	invoke drd_imageLoadFile,offset res.bgpath,offset res.bg
	invoke drd_imageLoadFile,offset shmulik.fly,offset shmulRM.fly
	invoke drd_imageLoadFile, offset shmulik.run,offset shmulRM.run
	invoke drd_imageLoadFile, offset res.dngrPath, offset res.dngr
	invoke drd_imageLoadFile, offset res.deadmsgPath, offset res.deathMsg
	invoke drd_imageLoadFile, offset res.dgtStripPath, offset res.dgtStrip
	invoke drd_imageLoadFile, offset res.highScorePath, offset res.highScore
		;set needed images as transparent(take out the color white from photos)
	invoke drd_imageSetTransparent,offset shmulRM.fly,0ffffffh
	invoke drd_imageSetTransparent,offset shmulRM.run,0ffffffh
	invoke drd_imageSetTransparent,offset res.dngr,0ffffffh
	invoke drd_imageSetTransparent,offset res.dgtStrip,0ffffffh
	invoke drd_imageSetTransparent,offset res.highScore,0ffffffh
ret
load endp


draw proc

		cmp catDead,1	
		je BLACK		;if cat is dead, jump to BLACK

		;move background and obstacles: 0=every run, 1, every other run, 2 = every two runs:
		inc slowBg		
		cmp slowBg, difficulty ;if slowbg is smaller than difficulty (defiened value) dont move background and obstacles
		jl STOPBG		;(in other words skip after the movement process)		
		mov slowBg, 0	;else restart slowBg

		inc xbg		;increase crop size (more in the future of the procedure)
		invoke movObst, offset obst1, edgeObst 
		invoke movObst, offset obst2, edgeObst

		cmp xbg,widthbg	;if crop size equals entire background width
		je RETBG	;jump to retbg
		RETRETBG:	;return point of retbg
		
		STOPBG:	;end of background and obstacle movement
		invoke drd_pixelsClear,0	; fill the back buffer in black
		
		;background animation:
		mov ebx, widthbg
		sub ebx, xbg	;ebx has the x of the other cropped image of background
		;the background image is always cropped into two:
		;one slowly contains less and less of the right side of of the background and is drawn on the left side of the screen
		invoke drd_imageDrawCrop, offset res.bg,0, 0, xbg, 0, ebx, heightbg	
		;the other contains the rest of the background image and is placed at the end of the first cropped image
		invoke drd_imageDrawCrop,offset res.bg,ebx,0, 0,0,xbg, heightbg
		;the combination of the first crops constant shrinking and the seconds constant growth create the effect of a moving background

		;next we draw the obstacles
		invoke drd_imageDraw, offset res.dngr, obst1.x ,obst1.y
		invoke drd_imageDraw,offset res.dngr, obst2.x,obst2.y

		;check if cat is on the floor
		cmp charPos.y, floorY
		je RUNNING	;if so jump to running code
		;else the cat is flying
		mov ebx, recXFly
		;draw cat flying in accordance to charPos, recXFly and the catFlies array
		invoke drd_imageDrawCrop,offset shmulRM.fly,charPos.x,charPos.y, catFlies[ebx],0,catFlies[ebx+4],68	 
		RETRUNNING:	;end of animation process

		RETBLACK:	;return end of alive process

		;next we draw the scores
		invoke drawScore, 10, 20, offset current

		invoke drd_imageDraw, offset res.highScore, 780, 20 ;draw at xcoordinate of HISC - widthHighScoreImg
		invoke drawScore, 980, 20, offset HISC

		
		invoke drd_flip		;flip display and back buffers
ret

RETBG:	;if crop size equals entire background width
mov xbg,0	;restart xbg to 0
jmp RETRETBG	;return to code

BLACK:	;reached if cat is dead:
invoke drd_imageDraw, offset res.deathMsg,0 ,0
jmp RETBLACK

RUNNING:	;if cat is on the floor
invoke animRun	;start run animation process
mov ebx, recXRun
;draw cat running in accordance to charPos, recXRun and the catRuns array
invoke drd_imageDrawCrop, offset shmulRM.run,charPos.x,charPos.y, catRuns[ebx],0,catRuns[ebx+4],68	
jmp RETRUNNING	;return to the code after the animation process

draw endp


update proc		;updates the data
	
	invoke drd_processMessages	;allows us to close game screen while running 

	cmp catDead, 1	;check cat state
	je DEATH		;if dead jump to death

	;check if character collided with the obstacles (flag in catDead)
	invoke collided, offset obst1, offset charPos, offset catDead
	invoke collided, offset obst2, offset charPos, offset catDead

	;update once every updatePace (defined value)
	;increase slow untill it equals update pace
	inc slowUpdate
	cmp slowUpdate,updatePace
	jl SLOWDOWN		;if not slowUpdate didnt reach yet jump to after update process
	;if reached:
	mov slowUpdate,0	;restart slowUpdate

	;do the exact same technique with the score:
	inc slowScore
	cmp slowScore,slowPoints
	jl DONEADDING
	mov slowScore,0

	cmp current.valu, maxScore	;compare current score and max score
	jge DONEADDING				;if they equal one another jump to after scoring process (current score remains maximum)

	;add to score digits and value
	invoke scoreAdd, offset current.ones, offset current.tens, offset current.hndrds, offset current.thou, current.valu
	inc current.valu

	
	mov ebx, current.valu
	cmp ebx, HISC.valu	;compare current score to high score
	jg SETHIGH			;if current  score exceeds high score jump to set high
	RETHIGH:			;return point of sethigh

	DONEADDING:			;end of increasing score by 1

	invoke accelerate, VK_SPACE		;add/decrease speed
	invoke moveCat		;move character in accordance to speed
SLOWDOWN:
ret


DEATH:	;if dead:
invoke revive, VK_R	;wait for revive key (r)
ret

SETHIGH:	;set high score as current score:
mov HISC.valu, ebx
invoke setDigit, current.ones, offset HISC.ones
invoke setDigit, current.tens, offset HISC.tens
invoke setDigit, current.hndrds, offset HISC.hndrds
invoke setDigit, current.thou, offset HISC.thou
jmp RETHIGH	;return to code
update endp

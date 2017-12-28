.THUMB				@ turn on thumb
.ALIGN  2			@ align code correctly for GBA
.GLOBL  backB		@ name of function goes here
.GLOBAL check
.GLOBAL upPos
.GLOBAL downPos


.THUMB_FUNC			@ we are about to declare a thumb function
downPos:				@ function start

push { r4-r7, lr }	@ push r4-r7 and link register onto stack. Your function might use these
					@ registers, so we need to preserve the values just in case!
					@ we don't need to worry about r0-r3 as it is assumed they will be regularly messed up anyway
	

@ the magic happens here!
@ r0-r3 will automatically contain the parameters sent when calling the function.
	
	ldr r4, [r0]
	sub r4,#1
	str r4,[r0]
	
	
pop { r4-r7 }		@ pop first 4 values from stack back into r4-r7, and also
pop { r3 }			@ pop the next value from stack (stored value for lr) into some unused register, e.g. r3 -
					@ we cannot overwrite lr so we have to do it via a normal register
bx r3				@ "branch and exchange" (return) back to C, using the previous value for lr stored in r3
@ ==================================





.THUMB_FUNC			@ we are about to declare a thumb function
upPos:				@ function start

push { r4-r7, lr }	@ push r4-r7 and link register onto stack. Your function might use these
					@ registers, so we need to preserve the values just in case!
					@ we don't need to worry about r0-r3 as it is assumed they will be regularly messed up anyway
	
@ the magic happens here!
@ r0-r3 will automatically contain the parameters sent when calling the function.
	
	ldr r4, [r0]
	add r4,#1
	str r4,[r0]
	
	
pop { r4-r7 }		@ pop first 4 values from stack back into r4-r7, and also
pop { r3 }			@ pop the next value from stack (stored value for lr) into some unused register, e.g. r3 -
					@ we cannot overwrite lr so we have to do it via a normal register
bx r3				@ "branch and exchange" (return) back to C, using the previous value for lr stored in r3
@ ==================================







.THUMB_FUNC			@ we are about to declare a thumb function
backB:				@ function start

push { r4-r7, lr }	@ push r4-r7 and link register onto stack. Your function might use these
					@ registers, so we need to preserve the values just in case!
					@ we don't need to worry about r0-r3 as it is assumed they will be regularly messed up anyway
	

@ the magic happens here!
@ r0-r3 will automatically contain the parameters sent when calling the function.
	
	ldr r4, [r0]
	ldr r5,[r1]
	ldr r6,[r2]
	
	add r5,#2
	add r6,#1
	
	
	str r5, [r1]
	str r6, [r2]
	
	bl check
	
pop { r4-r7 }		@ pop first 4 values from stack back into r4-r7, and also
pop { r3 }			@ pop the next value from stack (stored value for lr) into some unused register, e.g. r3 -
					@ we cannot overwrite lr so we have to do it via a normal register
bx r3				@ "branch and exchange" (return) back to C, using the previous value for lr stored in r3
@ ==================================

.THUMB_FUNC
check:

push { r4-r7, lr }	@ preserve r4-r7 and lr as before...

	ldr r4,[r0]
	cmp r4,#32
	bne test
	mov r4,#0
	sub r4,#1
	
	test:
	add r4,#1
	str r4, [r0]
	
pop { r4-r7 }		@ pop first 4 values from stack into r4-r7, and also
pop { r3 }			@ pop the next value from stack (old value for lr) into r3 - we cannot overwrite lr so we have to do it via a normal register
bx r3 				@ "branch and exchange" (return) back to calling function, using the previous value for lr stored in r3




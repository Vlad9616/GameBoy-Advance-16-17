.THUMB				@ turn on thumb
.ALIGN  2			@ align code correctly for GBA
.GLOBL  collisionx		@ name of function goes here
.GLOBL  collisiony
.GLOBAL secondTest



.THUMB_FUNC			@ we are about to declare a thumb function
collisionx:				@ function start

push { r4-r7, lr }	@ push r4-r7 and link register onto stack. Your function might use these
					@ registers, so we need to preserve the values just in case!
					@ we don't need to worry about r0-r3 as it is assumed they will be regularly messed up anyway
	

@ the magic happens here!
@ r0-r3 will automatically contain the parameters sent when calling the function.
	
	ldr r4,[r0]
	ldr r5,[r1]
	ldr r6,[r2]
	
	mov r6,#0
	cmp r4,r5
	bne set
	mov r6,#1
	str r6,[r2]
	str r5,[r1]
	str r4,[r0]
	set:
	
	
	
	
	
	
	
pop { r4-r7 }		@ pop first 4 values from stack back into r4-r7, and also
pop { r3 }			@ pop the next value from stack (stored value for lr) into some unused register, e.g. r3 -
					@ we cannot overwrite lr so we have to do it via a normal register
bx r3				@ "branch and exchange" (return) back to C, using the previous value for lr stored in r3
@ ==================================







.THUMB_FUNC			@ we are about to declare a thumb function
collisiony:				@ function start

push { r4-r7, lr }	@ push r4-r7 and link register onto stack. Your function might use these
					@ registers, so we need to preserve the values just in case!
					@ we don't need to worry about r0-r3 as it is assumed they will be regularly messed up anyway
	

@ the magic happens here!
@ r0-r3 will automatically contain the parameters sent when calling the function.
	
	ldr r4,[r0]
	ldr r5,[r1]
	ldr r6,[r2]
	
	mov r6,#0
	mov r7,#0
	sub r7,r4,#5
	add r4,#5
	
	cmp r5,r7
	BLE one
	add r6,#1
	cmp r5,r4
	BGE one
	add r6,#1
	
	str r6,[r2]
	one:
	

	
	
	
	
pop { r4-r7 }		@ pop first 4 values from stack back into r4-r7, and also
pop { r3 }			@ pop the next value from stack (stored value for lr) into some unused register, e.g. r3 -
					@ we cannot overwrite lr so we have to do it via a normal register
bx r3				@ "branch and exchange" (return) back to C, using the previous value for lr stored in r3
@ ==================================









	.file	"petDiskPiInterface.cpp"
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__SREG__ = 0x3f
__tmp_reg__ = 0
__zero_reg__ = 1
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"starting\r\n"
.LC1:
	.string	"\r\n"
	.section	.text.startup,"ax",@progbits
.global	main
	.type	main, @function
main:
	push r28
	push r29
	in r28,__SP_L__
	in r29,__SP_H__
	subi r28,1
	sbci r29,2
	in __tmp_reg__,__SREG__
	cli
	out __SP_H__,r29
	out __SREG__,__tmp_reg__
	out __SP_L__,r28
/* prologue: function */
/* frame size = 513 */
/* stack size = 515 */
.L__stack_usage = 515
	subi r28,lo8(-513)
	sbci r29,hi8(-513)
	st Y,__zero_reg__
	subi r28,lo8(513)
	sbci r29,hi8(513)
	movw r24,r28
	subi r24,-1
	sbci r25,-3
	call _ZN7softSpi4initEv
	ldi r24,lo8(12)
	ldi r25,0
	call uart0_init
	ldi r24,lo8(.LC0)
	ldi r25,hi8(.LC0)
	call transmitString
	ldi r24,0
	ldi r25,0
	movw r16,r28
	subi r16,-1
	sbci r17,-1
.L2:
	cp r24,__zero_reg__
	ldi r18,2
	cpc r25,r18
	brge .L8
	movw r30,r16
	add r30,r24
	adc r31,r25
	st Z,r24
	adiw r24,1
	rjmp .L2
.L8:
	ldi r20,0
	ldi r21,lo8(2)
	movw r22,r16
	movw r24,r28
	subi r24,-1
	sbci r25,-3
	call _ZN7softSpi8transferEPhi
	movw r12,r24
	movw r20,r24
	clr r22
	sbrc r21,7
	com r22
	mov r23,r22
	ldi r24,lo8(1)
	call transmitHex
	ldi r24,lo8(.LC1)
	ldi r25,hi8(.LC1)
	call transmitString
	movw r14,r16
.L4:
	movw r24,r14
	sub r24,r16
	sbc r25,r17
	cp r24,r12
	cpc r25,r13
	brge .L9
	movw r26,r14
	ld r24,X+
	movw r14,r26
	call transmitByte
	rjmp .L4
.L9:
	ldi r24,lo8(.LC1)
	ldi r25,hi8(.LC1)
	call transmitString
	movw r26,r16
/* #APP */
 ;  71 "petDiskPiInterface.cpp" 1
	ld r20, X
 ;  0 "" 2
/* #NOAPP */
	ldi r21,0
	ldi r22,0
	ldi r23,0
	ldi r24,0
	call transmitHex
	ldi r24,0
	ldi r25,0
	rjmp .L2
	.size	main, .-main
	.ident	"GCC: (GNU) 4.8.1"
.global __do_copy_data

	.file	"softSpi.cpp"
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__SREG__ = 0x3f
__tmp_reg__ = 0
__zero_reg__ = 1
	.text
.global	_ZN7softSpi4initEv
	.type	_ZN7softSpi4initEv, @function
_ZN7softSpi4initEv:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	movw r30,r24
	ld r24,Z
	tst r24
	breq .L2
	ldi r24,lo8(12)
	out 0x4,r24
	ldi r24,lo8(4)
	out 0x5,r24
	ret
.L2:
	ldi r24,lo8(8)
	out 0x4,r24
	out 0x5,__zero_reg__
	ret
	.size	_ZN7softSpi4initEv, .-_ZN7softSpi4initEv
.global	_ZN7softSpi7setMisoEh
	.type	_ZN7softSpi7setMisoEh, @function
_ZN7softSpi7setMisoEh:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	in r24,0x5
	cpse r22,__zero_reg__
	rjmp .L6
	andi r24,lo8(-9)
	rjmp .L7
.L6:
	ori r24,lo8(8)
.L7:
	out 0x5,r24
	ret
	.size	_ZN7softSpi7setMisoEh, .-_ZN7softSpi7setMisoEh
.global	_ZN7softSpi8transferEPhi
	.type	_ZN7softSpi8transferEPhi, @function
_ZN7softSpi8transferEPhi:
	push r6
	push r7
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push r16
	push r17
	push r28
	push r29
	rcall .
	in r28,__SP_L__
	in r29,__SP_H__
/* prologue: function */
/* frame size = 2 */
/* stack size = 16 */
.L__stack_usage = 16
	movw r16,r24
	movw r14,r22
	movw r10,r20
	movw r26,r24
	ld r24,X
	tst r24
	breq .L11
	out 0x5,__zero_reg__
	ldi r19,lo8(1)
	rjmp .L10
.L11:
	in r24,0x3
	mov r19,r24
	andi r19,lo8(4)
	sbrc r24,2
	rjmp .L11
.L10:
	movw r26,r14
/* #APP */
 ;  156 "../shared/avr/softSpi.cpp" 1
	transferbyte:
	ldi r20, 0x00
	ld r17, X
	ldi r21, 8
	transferbit:
	lsl r17
	brcs sendmisohigh
	sendmisolow:
	in r18, 5
	andi r18, 247
	sendmisohigh:
	in r18, 5
	ori r18, 8
	presentmiso:
	out r18, 5
	waitclock:
	in r18, 3
	mov r19, r18
	andi r18, 32
	brne readinputbit
	andi r19, 4
	brne done
	jmp waitclock
	readinputbit:
	lsl r20
	in r18, 3
	andi r18, 16
	breq waitclocklow
	readbithigh:
	ori r20, 0x01
	waitclocklow:
	in r18, 3
	andi r18, 32
	brne waitclocklow
	dec r21
	brne transferbit
	st X+, r20
	jmp transferbyte
	done:
	
 ;  0 "" 2
/* #NOAPP */
	in r18,0x3
	andi r18,lo8(32)
	movw r6,r14
	mov r12,__zero_reg__
	mov r13,__zero_reg__
.L12:
	movw r24,r6
	sub r24,r14
	sbc r25,r15
	cp r24,r10
	cpc r25,r11
	brlt .L23
.L27:
	movw r26,r16
	ld r24,X
	tst r24
	breq .L25
	ldi r24,lo8(4)
	out 0x5,r24
	rjmp .L25
.L23:
	cpse r19,__zero_reg__
	rjmp .L27
	movw r30,r6
	ld r8,Z
	ldi r24,lo8(8)
	mov r9,r24
	ldi r19,0
.L22:
	mov r22,r8
	andi r22,lo8(-128)
	movw r24,r16
	std Y+2,r18
	std Y+1,r19
	call _ZN7softSpi7setMisoEh
	lsl r8
	ldi r24,0
	ldd r18,Y+2
	ldd r19,Y+1
.L13:
	cpse r18,__zero_reg__
	rjmp .L15
	cpse r24,__zero_reg__
	rjmp .L28
	in r18,0x3
	andi r18,lo8(32)
	movw r26,r16
	ld r25,X
	cpse r25,__zero_reg__
	rjmp .L13
	in r24,0x3
	andi r24,lo8(4)
	rjmp .L13
.L15:
	cpse r24,__zero_reg__
	rjmp .L28
	lsl r19
	sbic 0x3,4
	ori r19,lo8(1)
.L20:
	in r24,0x3
	mov r18,r24
	andi r18,lo8(32)
	sbrc r24,5
	rjmp .L20
	dec r9
	brne .L22
	movw r30,r6
	st Z,r19
	ldi r31,-1
	sub r12,r31
	sbc r13,r31
	ldi r24,0
.L28:
	ldi r25,-1
	sub r6,r25
	sbc r7,r25
	mov r19,r24
	rjmp .L12
.L25:
	movw r24,r12
/* epilogue start */
	pop __tmp_reg__
	pop __tmp_reg__
	pop r29
	pop r28
	pop r17
	pop r16
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop r7
	pop r6
	ret
	.size	_ZN7softSpi8transferEPhi, .-_ZN7softSpi8transferEPhi
	.ident	"GCC: (GNU) 4.8.1"

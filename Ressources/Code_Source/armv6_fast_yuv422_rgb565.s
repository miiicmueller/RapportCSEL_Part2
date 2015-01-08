@ Stack layout on entry:
@ r2      void *rgb
@ r1      int size
@ r0  	  void *yuv
@ (%sp)   return address

.section .text
.globl yuv422_to_rgb565
.size	yuv422_to_rgb565, .-yuv422_to_rgb565
.align	2
.type	yuv420_to_rgb565, %function

yuv422_to_rgb565:
	@ args = 0, pretend = 0, frame = 88
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!	
	add	fp, sp, #0	
	sub	sp, sp, #92
	asr	r8, r1, #2	@ i = size >> 2
	mov	r1, r0	@ src = yuv
	mov 	r0, r2	@ r0 = dest	
	b	.Lforloop_test		
.Lforloop:
	ldmia 	r1!,{r3} @ => ldr r3,[r1] and add r3,r3,#4
	pld  	[r1,+#4] 	 @ Memory access optimisation
	and	r6, r3, #255	@ Y = (yuv32 & 0x000000ff);
	and	r4, r3, #65280	
	lsr	r4, r4, #8	@ U = (yuv32 & 0x0000ff00) >> 8;
	and	r7, r3, #16711680
	lsr	r7, r7, #16	@ Y2 = (yuv32 & 0x00ff0000) >> 16;				
	lsr	r2, r3, #24	@ V = (yuv32 & 0xff000000) >> 24;	
	sub	r2, r2, #128	@ V = V -128
	sub	r3, r4, #128	@ U = U -128
	mov	r4, #179	
	mul	r4, r2	
	asr	r4, r4, #7	
	str	r4, [fp, #-48]	@ Temp1
	add	r9, r6, r4	@ R = Y + Temp1
	bic	r9, r9, r9, asr #31
	mov	r4,#0xffffff00
	qadd 	r9,r9,r4
	and 	r9,r9,#0xff
	mov 	r4, #11
	mul	r4, r3
	mov 	r5, #22
	mul	r5, r2
	rsb	r4,r5,r4
	asr	r12,r4,#5 	@ Temp2
	rsb	r10, r12, r6	@ G = Y - Temp2
	bic	r10, r10, r10, asr #31
	mov	r4,#0xffffff00
	qadd 	r10,r10,r4
	and 	r10,r10,#0xff
	mov	r4, #226	 
	mul	r4, r3	
	asr	r2, r4, #7	@ Temp3
	add	r4, r2, r6	@ B = Y + Temp3
	bic	r4, r4, r4, asr #31
	mov	r3,#0xffffff00
	qadd 	r4,r4,r3
	and 	r4,r4,#0xff
	asr	r9, r9, #3	@ R >> 3
	mov	r3, r9, asl #11	
	asr	r10, r10, #2	@ G>>2
	add	r3,r3,r10,asl#5 @ r3 = ((R>>3) << 11) | ((G>>2) << 5)
	add	r6,r3,r4,asr#3 @ r6 = (R>>3) << 11) | ((G>>2) << 5) | (B>>3)
	ldr	r3, [fp, #-48]	@ Temp1
	add	r9, r7, r3	@ R = Y2 + Temp1
	bic	r9, r9, r9, asr #31
	mov	r4,#0xffffff00
	qadd 	r9,r9,r4
	and 	r9,r9,#0xff
	rsb	r10, r12, r7	@ G = Y2 - Temp2
	bic	r10, r10, r10, asr #31
	mov	r4,#0xffffff00
	qadd 	r10,r10,r4
	and 	r10,r10,#0xff
	add	r3, r7, r2	@ B = Y2 - Temp3
	bic	r3, r3, r3, asr #31
	mov	r4,#0xffffff00
	qadd 	r3,r3,r4
	and 	r3,r3,#0xff
	asr	r4, r9, #3	@ (R>>3)
	mov	r2, r4, asl #11	@ (R>>3) << 11
	asr	r4, r10, #2	@ (G>>2)
	add	r2, r2, r4, asl #5 @ (((R>>3) << 11) | ((G>>2) << 5) 
	add	r2, r2, r3, asr #3 @ (((R>>3) << 11) | ((G>>2) << 5) | (B>>3))	
	add	r6, r6, r2, asl #16 @rgb |= ()
	stmia	r0!,{r6}
	pld  	[r0,+#4] 	 @ Memory access optimisation
	sub	r8, r8, #1	@ i--
.Lforloop_test:
	cmp	r8, #0	   	@ compare i == 0
	bne	.Lforloop	
	sub	sp, fp, #0	@ else = exit
	@ sp needed	@
	ldr	fp, [sp], #4	@,
	bx	lr	@

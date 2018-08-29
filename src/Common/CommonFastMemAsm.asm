;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  CG Assembly code for X86_64 arch
;  Dayang Tech. Develop. Co.Ltd	2014
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
EXTERN lpdwColorMixCalTable : QWORD

PUBLIC asmX64_MMX_FastColorMix
PUBLIC asmX64_dmemset
PUBLIC asmX64_dmemcpy
PUBLIC asmX64_dmemcpy64
PUBLIC asmX64_SSE2_FastMemset	
PUBLIC asmX64_GetTrans
PUBLIC asmX64_MMX_LightDeal
_TEXT SEGMENT

;DWORD
asmX64_MMX_FastColorMix PROC
;(rcx)clB :DWORD,
;(rdx)clW :DWORD
		mov			eax,	edx			;clW
		cmp			eax,	0ff000000H
		jae			__OUT				; if(AW==255) return clW
		cmp			eax,	00ffffffH	; if(AW==0) return clB
		jbe			__OUT2				
		mov	        edx,	ecx			;clB		; È¡clB
		cmp			edx,	00ffffffH	;if(AB==0) return clW
		jbe			__OUT
		movd		mm1,	eax		;mm1 clw
		movd		mm0,	edx		;mm0 clb	
		shr			edx,	16		; DH = AB
		and			edx,	0ff00H	; EDX = 0x0000AA00
		shr			eax,	24		; EAX = 0x000000AA
		or			eax,	edx		; EAX = (AB*256+AW) ah = Alpha,al = KValue
		cmp			eax,	0ff00H
		jbe			__GetTable		; if(AB == 255) goto __Comput (K=AW,A=AB)
		mov			edx,	eax
		shl			edx,	16
		or			eax,	edx		; eax = AKAK
		jmp			__Comput
__GetTable:
		shl			eax,	2				; EAX = (AW*256+AB)<<2
		mov			r8,	lpdwColorMixCalTable
		add			r8,		rax				;prh	
		mov			edx,	dword ptr[r8]	; ah = KValue,al=Alpha
		mov			eax,	edx
__Comput:
		and			edx,	0ff000000H
		pxor		mm4,	mm4
		and			eax,	00ff00ffH
		punpcklbw	mm0,	mm4		; ²ð·âmm0	mm0 = xxCB(000A000R000G000B)
		punpcklbw	mm1,	mm4		; ²ð·âmm1	mm1 = xxCW(000A000R000G000B)
		movd		mm2,	eax		; mm2 = K,K
		punpckldq	mm2,	mm2		; mm2 = K,K,K,K
		psubsw		mm1,	mm0		; mm1 = CW-CB
		pmullw		mm1,	mm2		; mm1 = (CW-CB)*K
		psllw		mm0,	8		; mm0 = CB<<8
		paddw		mm0,	mm1		; mm0 = CB<<8+(CW-CB)*k
		psrlw		mm0,	8		; mm0 = C
		packuswb	mm0,	mm2		; ½ôËõ
		movd        eax,	mm0
		and			eax,	00ffffffH
		or			eax,	edx
__OUT:
		emms
		ret			0
__OUT2:
		mov			eax,	ecx		;clB
									;mov			clB,	eax
		emms 
		ret			0										
asmX64_MMX_FastColorMix ENDP

asmX64_dmemset PROC
;(rcx)*buffer, 
;(edx)value,
;(r8d)num
	push		rdi
	mov			rdi,rcx	;buffer
	mov			eax,edx	;value
	mov			ecx,r8d	;num
	rep			stosd
	pop			rdi
	ret			0
asmX64_dmemset ENDP	


asmX64_dmemcpy PROC 
;lpdwDes:QWORD, lpdwSrc:QWORD,nSize:SDWORD
		sub		rsp,24
		mov		QWORD PTR [rsp+8],rsi
		mov		QWORD PTR [rsp],rdi
		
		cld
		mov		rdi,rcx
		mov		rsi,rdx
		movsxd	rcx,r8d
		rep		movsd
		
		mov		rsi,QWORD PTR [rsp+8]	
		mov		rdi,QWORD PTR [rsp]
		add		rsp,24
		ret		0
asmX64_dmemcpy ENDP

;mov qword once a time
asmX64_dmemcpy64 PROC 
;lpdwDes:QWORD, lpdwSrc:QWORD,nSize:SDWORD
		sub		rsp,24
		mov		qword ptr [rsp+8],rsi
		mov		qword ptr [rsp],rdi
		
		cld
		mov		rdi,rcx
		mov		rsi,rdx
		movsxd	rcx,r8d
		rep		movsq
		
		mov		rsi,qword ptr [rsp+8]	
		mov		rdi,qword ptr [rsp]
		add		rsp,24
		ret		0
asmX64_dmemcpy64 ENDP


asmX64_SSE2_FastMemset PROC 
;lpdwDes:QWORD,dwColor:DWORD,nSize:SDWORD
		push		rdi
		mov			rdi,rcx ;lpdwDes
		mov			rax,rdx ;dwColor
		movsxd		rcx,r8d ;nSize
		cmp			ecx,16
		ja			__SSESET
		rep			stosd	
		jmp 		__RETURN
__SSESET:
		test		rdi,0fh
		jz			__PARSSET
		mov			rdx,rdi
		not			rdx
		inc			rdx
		and			rdx,0fh
		shr			rdx,2
		sub			rcx,rdx
__HEADERLOOP:
		mov			dword ptr [rdi],eax
		add			rdi,4
		dec			rdx
		ja			__HEADERLOOP
__PARSSET:
		mov			rdx,rcx
		and			rdx,3h
		shr			rcx,2
		jz			__REMAINDER
		movd		xmm3,eax
		pshufd		xmm3,xmm3,0
__LOOP:
		movntdq		oword ptr [rdi],xmm3
		add			rdi,16
		dec			rcx
		ja			__LOOP
__REMAINDER:
		cmp			rdx,0
		jz			__RETURN
		mov			rcx,rdx
		rep			stosd
		
__RETURN:
		pop rdi
		emms		
		ret	0	
asmX64_SSE2_FastMemset ENDP

asmX64_MMX_LightDeal PROC
; (rcx)lpdwDes :QWORD,
; (rdx)lpdwSrc:QWORD,
; (r8)lpLightMask:QWORD,
; (r9)lpLightTrans:QWORD,	// ¶¨³¤ 256
; (40)nWidth :SDWORD

			nWidth$ = 40									  
			mov		r10, rcx		;lpdwDes
			mov		r11, rdx		;lpdwSrc
			
			mov			ecx, DWORD PTR nWidth$[rsp]
			pxor		mm0, mm0
__LOOP:
			xor			rax, rax
			mov			al,	 BYTE PTR [r8]
			lea			rax, [rax*4]
			inc			r8

			movd		mm3, DWORD PTR [r11]
			add			rax, r9

			punpcklbw	mm3, mm0				; ²ð·â mm3 = Cs
			mov			edx, DWORD PTR [rax]			; eax - Mi

			test		edx, 01000000H		; 0x01000000	¸ßÁ¿±êÖ¾
			jz			__LOWLIGHT

			and			edx, 00ffffffH
			movq		mm5, mm3				; mm5 = Cs
			movd		mm4, edx

			punpcklbw	mm4, mm0				; ²ð·â mm4 = Mi
			pmullw		mm5, mm4				; mm5 = Cs*Mi
			paddw		mm3, mm4				; mm3 = Cs+Mi

			psrlw		mm5, 8
			psubw		mm3, mm5				; mm3 = Cs+Mi-Cs*Mi
			packuswb	mm3, mm0				; ½ôËõ

			movd		DWORD PTR [r10], mm3
			add			r11,4
			add			r10,4

			dec			ecx
			jnz			__LOOP
			jmp			__OVER

__LOWLIGHT:
			movd		mm4, edx
			punpcklbw	mm4, mm0				; ²ð·â mm4 = Mi
			pmullw		mm4, mm3				; mm4 = Cs*Mi
			psrlw		mm4, 8
			psubw		mm3, mm4
			packuswb	mm3, mm0				; ½ôËõ
			movd		DWORD PTR [r10], mm3
			add			r11,4
			add			r10,4
			dec			ecx
			jnz			__LOOP
__OVER:
			emms
			ret			0
asmX64_MMX_LightDeal ENDP

asmX64_GetTrans PROC
;ret value DWORD(eax)
;(ecx)dwFactor:DWORD,(edx)dwTrans:DWORD
			pxor		mm0, mm0
			mov			r9d, edx		;dwTrans
			and			r9d, 0ffh
			mov			r8d, r9d
			shl			r9d, 8
			or			r9d, r8d
			shl			r9d, 8
			or			r9d, r8d
			movd		mm1, ecx ;dwFactor
			movd		mm2, r9d
			punpcklbw	mm1, mm0
			punpcklbw	mm2, mm0
			pmullw		mm1, mm2
			psrlw		mm1, 8
			packuswb	mm1, mm0				; ½ôËõ
			movd		eax, mm1
			emms
			ret			0
asmX64_GetTrans ENDP

	
_TEXT ENDS

END

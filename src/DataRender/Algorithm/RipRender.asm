	public _memset_dword_line_x64
	public _memset_dword_line_align_x64
	public memset_alpha_line_all_x64
	public _memcpy_line_align_x64
	public _memcpy_line_x64
	public memcpy_line_with_filter_x64
	public memset_alpha_line_mask_x64
	public memset_alpha_line_notmask_x64
	public bgra32_scale_alpha_line_x64
	public bgra32_line_mix_x64
	public bgra32_bl_copy_line_x64
	
	public _memcpy_rev_char_x64
	public _memcpy_rev_dword_x64
	public _memcpy_dword_col_x64
	public _memset_char_line_x64
	public copy_dword_line_rgb_mula_x64
	public copy_dword_line_mula_x64
	public copy_dword_line_mula_invert_x64
	public memset_dword_line_rgb_mula_x64
	public memset_dword_line_rgb_x64
	public memrev_char_line_x64
	public memrev_dword_line_x64
	public bgra32_line_mix_notsrc_x64
	public bgra32_line_alpha_blend_x64
	public bgra32_line_alpha_blend_not_x64
	public bgra32_line_linear_dodge_x64
	public bgra32_line_linear_burn_x64
	public bgra32_powerfill_x64
	public bgra32_powerfill_q_x64
	public bgra32_linear_recursive_x64
	public bgra32_mean_copy_add_y_x64
	public bgra32_mean_copy_add_y_quick_x64
	public bgra32_mean_copy_add_x_x64
	public bgra32_replace_alpha_line_x64
	public bgra32_key_mul_line_x64
	public bgra32_key_mul_notsrc_line_x64
	public bgra32_key_mul_notdes_line_x64
	public bgra32_key_mix_line_x64
	public bgra32_key_mix_notsrc_line_x64
	public bgra32_key_sat_sub_line_x64
	public keybuf_mul_char_line_x64
	public keybuf_mul_char_line_notdes_x64
	public keybuf_mix_char_line_x64
	
	public keybuf_mul_buf_line_x64
	public keybuf_or_buf_line_x64
	public keybuf_mul_buf_line_notsrc_x64
	public keybuf_mul_buf_line_notdes_x64
	public keybuf_mix_buf_line_x64
	public keybuf_mix_buf_line_notsrc_x64
	public keybuf_sat_sub_buf_line_x64
	public keybuf_bl_copy_line_x64
	public math_serial_perfect_square_x64
	public math_serial_perfect_square_ex_x64
	public math_offset_sqrt_index_lookup_x64
	public math_tyler_arc_index_lookup_x_x64
	public math_tyler_arc_index_lookup_y_x64
	public math_negtive_float_x64
	public math_serial_offset_2_x64
	public trans_power_index_bl_x64
	public trans_interpo_bl_x64
	
	public color24_to_color32_x64
	public yuyv_to_color24_inner_x64
	public color24_to_yuyv_inner_x64
	public yuyv_to_bgra_inner_x64
	public yuyv_to_rgba_inner_x64
	public yuyva_to_bgra_inner_x64
	public yuyva_to_rgba_inner_x64
	public color32_to_yuyv_inner_x64
	public color32_to_yuyva_inner_x64
	public yuv422_to_color32_inner_x64	
	public uv20_to_uv22_line_1_L_x64	
	public uv20_to_uv22_line_2_L_x64
	public uv20_to_uv22_line_2_x64
	public uv20_to_uv22_line_2_F_x64
	public yuv422_to_yuyv_line_x64
	
	public _yuv420_to_yuyv_line_2_F_x64
	public _yuv420_to_yuyv_line_2_x64
	public _yuv420_to_yuyv_line_2_L_x64
	public _yuv420_to_yuyv_line_1_L_x64
	public _Fill_inner_pixel

	public _rip_x_filllinebuf_x64
	public _rip_update_buf_byte_x64		

	public _yuv_yyuuvv2yuv_x64
	public _yuv_yuv2yyuuvv_x64
	public _yuv_uyvy2yuv8_x64
	public _yuv_yuyv2yuv8_x64
	public _bgra32_mean_copy_add_y_quick_field_x64
	public _asmX64_SSE2_FastMemset

.data
ALIGN 16
gc_dwMaskDLW	dd	65535
				dd	65535
				dd	65535
				dd	65535

gc_dwMaskDHW	dd	4294901760
				dd	4294901760
				dd	4294901760
				dd	4294901760

gc_dwMaskDLB	dd	255
				dd	255
				dd	255
				dd	255
				
gc_dwMaskWHB	dd	4278255360
				dd	4278255360
				dd	4278255360
				dd	4278255360

gc_dwMaskWLB	dd	16711935
				dd	16711935
				dd	16711935
				dd	16711935
				
gc_dwMaskRGB	dd	16777215
				dd	16777215
				dd	16777215
				dd	16777215
				
gc_dwMaskG		dd	65280
				dd	65280
				dd	65280
				dd	65280
				
gc_dwMaskSpecUV	dd	4294967295
				dd	4294967295
				dd	4294967295
				dd	4294967295
								
gc_dwMaskAlpha	dd	4278190080
				dd	4278190080
				dd	4278190080
				dd	4278190080 

gc_nComp_N1		dd	4294967295
				dd	4294967295
				dd	4294967295
				dd	4294967295 
				
gc_nOff128_5	dd	268439552
				dd	268439552
				dd	268439552
				dd	268439552
				
gc_nOff128_6	dd	536879104
				dd	536879104
				dd	536879104
				dd	536879104
				
gc_nOff128_7	dd	1073758208
				dd	1073758208
				dd	1073758208
				dd	1073758208
				
gc_fRoundComp	dd	0.5f
				dd	0.5f
				dd	0.5f
				dd	0.5f

gc_nRoundComp_1	dd	65537
				dd	65537
				dd	65537
				dd	65537 
						
gc_nRoundComp_5	dd	1048592
				dd	1048592
				dd	1048592
				dd	1048592
				
gc_nRoundComp_2	dd	131074
				dd	131074
				dd	131074
				dd	131074
				
gc_fCoef_6		dd	6.f
				dd	6.f
				dd	6.f
				dd	6.f
				
gc_fCoef_13_6	dd	2.1666667f
				dd	2.1666667f
				dd	2.1666667f
				dd	2.1666667f
				
gc_fCoef_149_120	dd	1.2416667f
					dd	1.2416667f
					dd	1.2416667f
					dd	1.2416667f
							
.code
ALIGN 16
   
;-----------------------------------------------------------------------------
; _memset_dword_line_x64(OUT void *des,	// 目标
;						IN DWORD value,				// 设置值
;						IN int na16,				// 宽度除以 16 的商
;						IN int nm16);				// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

_memset_dword_line_x64:

	;r9 - nm16
	;r8 - na16
	;rdx - value
    ;rcx - des 
           
    push		rdi
    
    mov			rdi, rcx;
	mov			rcx, r8;
	mov			eax, edx;				; v
	mov			rdx, r9;
	movd		xmm0, eax;				; 0 0 0 v
	shufps		xmm0, xmm0, 00h;		; v v v v
	test		ecx, ecx;
	jnz			loop_memset_a16;
	jmp			loop_memset_a8;
loop_memset_a16:
	movdqu		[rdi], xmm0;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_memset_a16;
loop_memset_a8:
	test		edx, 00000008h;
	jz			loop_memset_a4;
	movlps		QWORD PTR[rdi], xmm0;
	add			rdi, 08h;
loop_memset_a4:
	test		edx, 00000004h;
	jz			loop_memset_end;
	mov			[rdi], eax;
loop_memset_end:

	pop			rdi
    ret
    
   
   
;-----------------------------------------------------------------------------
; _memset_dword_line_align_x64(OUT void *des,	// 目标
;						IN DWORD value,				// 设置值
;						IN int na16,				// 宽度除以 16 的商
;						IN int nm16);				// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

_memset_dword_line_align_x64:

	;r9 - nm16
	;r8 - na16
	;rdx - value
    ;rcx - des 
    
    push		rdi
        
	mov			rdi, rcx;
	mov			rcx, r8;
	mov			eax, edx;			; v
	mov			rdx, r9;
	movd		xmm0, eax;			; 0 0 0 v
	shufps		xmm0, xmm0, 00h;	; v v v v
	test		ecx, ecx;
	jnz			loop_memset_align_a16;
	jmp			loop_memset_align_a8;
loop_memset_align_a16:
	movdqa		[rdi], xmm0;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_memset_align_a16;
loop_memset_align_a8:
	test		edx, 00000008h;
	jz			loop_memset_align_a4;
	movlps		QWORD PTR[rdi], xmm0;
	add			rdi, 08h;
loop_memset_align_a4:
	test		edx, 00000004h;
	jz			loop_memset_align_end;
	mov			[rdi], eax;
loop_memset_align_end:

	pop			rdi
    ret
    
    

;-----------------------------------------------------------------------------
; memset_alpha_line_all_x64( OUT void *des,	// 目标
;						   IN char alpha,		// 填充值
;						   IN int na16,			// 宽度除以 16 的商
;						   IN int nm16)			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

memset_alpha_line_all_x64:

	;r9 - nm16
	;r8 - na16
	;rdx - alpha
    ;rcx - des 
    
    push		rdi
   
    mov			rdi, rcx;
	mov			rcx, r8;
	mov			eax, edx;				; 000a
	mov			edx, 00ffffffh;			; 掩码
	movd		xmm0, eax;				; 0  0  0  000a
	movd		xmm1, edx;
	shufps		xmm0, xmm0, 00h;		; 000a  000a  000a  000a
	shufps		xmm1, xmm1, 00h;		; 掩码
	pslld		xmm0, 18h;				; a000  a000  a000  a000
	test		ecx, ecx;
	jnz			loop_memset_alpha_a16;
	jmp			loop_memset_alpha_a8;
loop_memset_alpha_a16:
	movdqu		xmm2, [rdi];
	pand		xmm2, xmm1;
	por			xmm2, xmm0;
	movdqu		[rdi], xmm2;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_memset_alpha_a16;
loop_memset_alpha_a8:
	mov			rcx, r9;
	test		ecx, 00000008h;
	jz			loop_memset_alpha_a4;
	movlps		xmm2, QWORD PTR[rdi];
	pand		xmm2, xmm1;
	por			xmm2, xmm0;
	movlps		QWORD PTR[rdi], xmm2;
	add			rdi, 08h;
loop_memset_alpha_a4:
	test		ecx, 00000004h;
	jz			loop_memset_alpha_end;
	mov			ecx, [rdi];
	shl			eax, 18h;
	and			ecx, edx;
	or			ecx, eax;
	mov			[rdi], ecx;
loop_memset_alpha_end:

	pop			rdi
    ret
    
    

;-----------------------------------------------------------------------------
; _memcpy_line_align_x64(OUT void *des,		// 目标
;							IN const void *src,		// 源
;							IN int na16,			// 宽度除以 16 的商
;							IN int nm16);			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

_memcpy_line_align_x64:

	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    
    push		rdi
    push		rsi
   
    mov			rdi, rcx;
	mov			rsi, rdx;
	mov			rcx, r8;
	mov			rdx, r9;
	test		ecx, ecx;
	jnz			loop_memcpy_line_align_a16;
	jmp			loop_memcpy_line_align_a8;
	
loop_memcpy_line_align_a16:
	movdqa		xmm0, [rsi];
	movdqa		[rdi], xmm0;
	add			rsi, 10h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_memcpy_line_align_a16;
	
loop_memcpy_line_align_a8:
	test		edx, 00000008h;
	jz			loop_memcpy_line_align_a4;
	movlps		xmm0, QWORD PTR[rsi];
	movlps		QWORD PTR[rdi], xmm0;
	add			rsi, 08h;
	add			rdi, 08h;
	
loop_memcpy_line_align_a4:
	test		edx, 00000004h;
	jz			loop_memcpy_line_align_a2;
	mov			eax, [rsi];
	add			rsi, 04h;
	mov			[rdi], eax;
	add			rdi, 04h;
	
loop_memcpy_line_align_a2:
	test		edx, 00000002h;
	jz			loop_memcpy_line_align_a1;
	mov			ax, [rsi];
	add			rsi, 02h;
	mov			[rdi], ax;
	add			rdi, 02h;
	
loop_memcpy_line_align_a1:
	test		edx, 00000001h;
	jz			loop_memcpy_line_align_end;
	mov			al, [rsi];
	mov			[rdi], al;
	
loop_memcpy_line_align_end:

	pop			rsi
	pop			rdi
    ret
    
    

;-----------------------------------------------------------------------------
; void _memcpy_line_x64(OUT void *des,			// 目标
;						IN const void *src,		// 源
;						IN int na16,			// 宽度除以 16 的商
;						IN int nm16);			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

_memcpy_line_x64:

	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    
    push		rdi
    push		rsi
   
	mov			rdi, rcx;
	mov			rsi, rdx;
	mov			rcx, r8;
	mov			rdx, r9;
	test		ecx, ecx;
	jnz			loop_memcpy_line_a16;
	jmp			loop_memcpy_line_a8;
	
loop_memcpy_line_a16:
	movdqu		xmm0, [rsi];
	movdqu		[rdi], xmm0;
	add			rsi, 10h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_memcpy_line_a16;
	
loop_memcpy_line_a8:
	test		edx, 00000008h;
	jz			loop_memcpy_line_a4;
	movlps		xmm0, QWORD PTR [rsi];
	movlps		QWORD PTR [rdi], xmm0;
	add			rsi, 08h;
	add			rdi, 08h;
	
loop_memcpy_line_a4:
	test		edx, 00000004h;
	jz			loop_memcpy_line_a2;
	mov			eax, [rsi];
	add			rsi, 04h;
	mov			[rdi], eax;
	add			rdi, 04h;
	
loop_memcpy_line_a2:
	test		edx, 00000002h;
	jz			loop_memcpy_line_a1;
	mov			ax, [rsi];
	add			rsi, 02h;
	mov			[rdi], ax;
	add			rdi, 02h;
	
loop_memcpy_line_a1:
	test		edx, 00000001h;
	jz			loop_memcpy_line_end;
	mov			al, [rsi];
	mov			[rdi], al;
	
loop_memcpy_line_end:

	pop			rsi
	pop			rdi
    ret
    
 
;-----------------------------------------------------------------------------
; void memcpy_line_with_filter_x64(
;							OUT void *des,			// 目标
;							IN void *src,			// 源
;							IN int na16,			// 宽度除以 16 的商
;							IN int nm16,
;							ALIGN16_DECLARE(DWORD) mask[4]);		// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

memcpy_line_with_filter_x64:

	;[rsp+28h] - mask
	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    
    push		rdi
    push		rsi
   
    mov			rdi, rcx;
	mov			rsi, rdx;
	mov			rcx, r8;
	mov			rax,[rsp+28h+10h];
	mov			edx, [rax];
	movdqu		xmm2,[rax];
	test		ecx, ecx;
	jnz			loop_memcpy_line_with_filter_a16;
	jmp			loop_memcpy_line_with_filter_a8;
	
loop_memcpy_line_with_filter_a16:
	movdqu		xmm0, [rsi];
	pand		xmm0,xmm2;
	movdqu		[rdi], xmm0;
	add			rsi, 10h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_memcpy_line_with_filter_a16;
	
loop_memcpy_line_with_filter_a8:
	mov			rcx, r9
	test		ecx, 00000008h;
	jz			loop_memcpy_line_with_filter_a4;
	movlps		xmm0, QWORD PTR[rsi];
	pand		xmm0,xmm2;
	movlps		QWORD PTR[rdi], xmm0;
	add			rsi, 08h;
	add			rdi, 08h;
	
loop_memcpy_line_with_filter_a4:
	test		ecx, 00000004h;
	jz			loop_memcpy_line_with_filter_a2;
	mov			eax, [rsi];
	and			eax,edx;
	add			rsi, 04h;
	mov			[rdi], eax;
	add			rdi, 04h;
	
loop_memcpy_line_with_filter_a2:
	test		ecx, 00000002h;
	jz			loop_memcpy_line_with_filter_a1;
	mov			ax, [rsi];
	add			rsi, 02h;
	mov			[rdi], ax;
	add			rdi, 02h;
	
loop_memcpy_line_with_filter_a1:
	test		ecx, 00000001h;
	jz			loop_memcpy_line_with_filter_end;
	mov			al, [rsi];
	mov			[rdi], al;
	
loop_memcpy_line_with_filter_end:

	pop			rsi
	pop			rdi
    ret
    
 
;-----------------------------------------------------------------------------
; void memset_alpha_line_mask_x64(IN OUT void *des,	// 目标
;							IN char alpha,			// 填充值
;							IN DWORD color,			// 只填充颜色值
;							IN int na16,			// 宽度除以 16 的商
;							IN int nm16);			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

memset_alpha_line_mask_x64:

	;[rsp+28h] - nm16
	;r9 - na16
	;r8 - color
	;rdx - alpha
    ;rcx - des 
    
    push		rdi
       
	mov			rdi, rcx;
	mov			rcx, r9;
	mov			eax, edx;			; 000a
	mov			edx, 00ffffffh;		; 掩码
	movd		xmm0, eax;			; 0  0  0  000a
	movd		xmm1, edx;
	movd		xmm5, r8;
	shufps		xmm0, xmm0, 00h;	; 000a  000a  000a  000a
	shufps		xmm1, xmm1, 00h;	; 掩码
	shufps		xmm5, xmm5, 00h;	; 只填充颜色
	pslld		xmm0, 18h;			; a000  a000  a000  a000
	test		ecx, ecx;
	jnz			loop_memset_alpha_line_mask_a16;
	jmp			loop_memset_alpha_line_mask_a8;
loop_memset_alpha_line_mask_a16:
	movdqu		xmm2, [rdi];
	movdqa		xmm3, xmm2;
	pcmpeqd		xmm2, xmm5;		; 做出只填充颜色的掩码
	movdqa		xmm4, xmm2;
	pandn		xmm2, xmm3;		; 不填充颜色
	pand		xmm3, xmm4;		; 只填充颜色
	pand		xmm4, xmm0;		; 填充alpha
	pand		xmm3, xmm1;		; 去除旧alpha值
	por			xmm2, xmm4;
	por			xmm2, xmm3;
	movdqu		[rdi], xmm2;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_memset_alpha_line_mask_a16;
loop_memset_alpha_line_mask_a8:
	mov			ecx, [rsp+28h+8h];
	test		ecx, 08h;
	jz			loop_memset_alpha_line_mask_a4;
	movlps		xmm2, QWORD PTR[rdi];
	movdqa		xmm3, xmm2;
	pcmpeqd		xmm2, xmm5;		; 做出只填充颜色的掩码
	movdqa		xmm4, xmm2;
	pandn		xmm2, xmm3;		; 不填充颜色
	pand		xmm3, xmm4;		; 只填充颜色
	pand		xmm4, xmm0;		; 填充alpha
	pand		xmm3, xmm1;		; 去除旧alpha值
	por			xmm2, xmm4;
	por			xmm2, xmm3;
	movlps		QWORD PTR[rdi], xmm2;
	add			rdi, 08h;
loop_memset_alpha_line_mask_a4:
	test		ecx, 04h;
	jz			loop_memset_alpha_line_mask_end;
	mov			rcx, [rdi];
	cmp			rcx, r8;
	jne			loop_memset_alpha_line_mask_end;
	shl			eax, 18h;
	and			ecx, edx;
	or			ecx, eax;
	mov			[rdi], ecx;
loop_memset_alpha_line_mask_end:

	pop			rdi
    ret
    
 
;-----------------------------------------------------------------------------
; void memset_alpha_line_notmask_x64(IN OUT void *des,	// 目标
;							IN char alpha,			// 填充值
;							IN DWORD notcolor,		// 不填充颜色值
;							IN int na16,			// 宽度除以 16 的商
;							IN int nm16);			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

memset_alpha_line_notmask_x64:

	;[rsp+28h] - nm16
	;r9 - na16
	;r8 - notcolor
	;rdx - alpha
    ;rcx - des 
    
    push		rdi
	sub			rsp,10h
	movups		[rsp],xmm7
   
	mov			rdi, rcx;
	mov			rcx, r9;
	mov			eax, edx;			; 000a
	mov			edx, 00ffffffh;	; 掩码
	movd		xmm0, eax;			; 0  0  0  000a
	movd		xmm1, edx;
	movd		xmm7, r8;
	shufps		xmm0, xmm0, 00h;	; 000a  000a  000a  000a
	shufps		xmm1, xmm1, 00h;	; 掩码
	shufps		xmm7, xmm7, 00h;	; 不填充颜色
	pslld		xmm0, 18h;			; a000  a000  a000  a000
	test		ecx, ecx;
	jnz			loop_memset_alpha_line_notmask_a16;
	jmp			loop_memset_alpha_line_notmask_a8;
	
loop_memset_alpha_line_notmask_a16:
	movdqu		xmm2, [rdi];
	movdqa		xmm3, xmm2;
	pcmpeqd		xmm2, xmm7;		; 做出不填充颜色的掩码
	movdqa		xmm4, xmm2;
	movdqa		xmm5, xmm2;
	pandn		xmm2, xmm3;		; 填充颜色
	pandn		xmm4, xmm0;		; 填充alpha
	pand		xmm3, xmm5;		; 不填充颜色
	pand		xmm2, xmm1;		; 去除旧alpha值
	por			xmm4, xmm3;
	por			xmm2, xmm4;
	movdqu		[rdi], xmm2;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_memset_alpha_line_notmask_a16;
	
loop_memset_alpha_line_notmask_a8:
	mov			ecx, [rsp+28h+18h];
	test		ecx, 00000008h;
	jz			loop_memset_alpha_line_notmask_a4;
	movlps		xmm2, QWORD PTR[rdi];
	movdqa		xmm3, xmm2;
	pcmpeqd		xmm2, xmm7;		; 做出不填充颜色的掩码
	movdqa		xmm4, xmm2;
	movdqa		xmm5, xmm2;
	pandn		xmm2, xmm3;		; 填充颜色
	pandn		xmm4, xmm0;		; 填充alpha
	pand		xmm3, xmm5;		; 不填充颜色
	pand		xmm2, xmm1;		; 去除旧alpha值
	por			xmm4, xmm3;
	por			xmm2, xmm4;
	movlps		QWORD PTR[rdi], xmm2;
	add			rdi, 08h;
	
loop_memset_alpha_line_notmask_a4:
	test		ecx, 00000004h;
	jz			loop_memset_alpha_line_notmask_end;
	mov			ecx, [rdi];
	cmp			ecx, r8d;
	je			loop_memset_alpha_line_notmask_end;
	shl			eax, 18h;
	and			ecx, edx;
	or			ecx, eax;
	mov			[rdi], ecx;
	
loop_memset_alpha_line_notmask_end:

	movups		xmm7,[rsp]
	add			rsp,10h
	pop			rdi
    ret
    
  
;-----------------------------------------------------------------------------
; void bgra32_scale_alpha_line_x64(IN OUT void *des,	// 目标
;							IN char alpha,			// 缩放值
;							IN int na16,			// 宽度除以 16 的商
;							IN int nm16);			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

bgra32_scale_alpha_line_x64:

	;r9 - nm16
	;r8 - na16
	;rdx - alpha
    ;rcx - des 
    
    push		rdi
   
	mov			rdi, rcx;
	mov			eax, edx;		
	mov			rcx, r8;
	movd		xmm0, eax;		
	movdqa		xmm1, oword ptr[gc_dwMaskDLB];	
	movdqa		xmm4, oword ptr[gc_dwMaskRGB];
	movdqa		xmm5, oword ptr[gc_dwMaskAlpha];
	shufps		xmm0, xmm0, 00h;	
	test		ecx, ecx;
	jnz			loop_bgra32_scale_alpha_a16;
	jmp			loop_bgra32_scale_alpha_a8;
loop_bgra32_scale_alpha_a16:
	movdqu		xmm2, [rdi];
	movdqa		xmm3, xmm2;
	psrld		xmm2, 18h;		
	pmullw		xmm2, xmm0;
	paddd		xmm2, xmm1;
	pslld		xmm2, 10h;
	pand		xmm3, xmm4;
	pand		xmm2, xmm5;
	por			xmm2, xmm3;
	movdqu		[rdi], xmm2;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_bgra32_scale_alpha_a16;
loop_bgra32_scale_alpha_a8:
	mov			rcx, r9;
	test		ecx, 00000008h;
	jz			loop_bgra32_scale_alpha_a4;
	movlps		xmm2, QWORD PTR[rdi];
	movdqa		xmm3, xmm2;
	psrld		xmm2, 18h;		
	pmullw		xmm2, xmm0;
	paddd		xmm2, xmm1;
	pslld		xmm2, 10h;
	pand		xmm3, xmm4;
	pand		xmm2, xmm5;
	por			xmm2, xmm3;
	movlps		QWORD PTR[rdi], xmm2;
	add			rdi, 08h;
loop_bgra32_scale_alpha_a4:
	test		ecx, 00000004h;
	jz			loop_bgra32_scale_alpha_end;
	movd		xmm2, DWORD PTR[rdi];
	movdqa		xmm3, xmm2;
	psrld		xmm2, 18h;		
	pmullw		xmm2, xmm0;
	paddd		xmm2, xmm1;
	pslld		xmm2, 10h;
	pand		xmm3, xmm4;
	pand		xmm2, xmm5;
	por			xmm2, xmm3;
	movd		DWORD PTR[rdi], xmm2;
loop_bgra32_scale_alpha_end:

	pop			rdi
    ret
    
    
 
;-----------------------------------------------------------------------------
; void bgra32_line_mix_x64(IN OUT void *des,	// 目标
;							IN void *src,			// 源
;							IN int na16,			// 宽度除以 16 的商
;							IN int nm16,			// 宽度除以 16 的余数
;							IN void *ka_table);		// 256 * 256 * sizeof(DWORD) 的 KA 表
;-----------------------------------------------------------------------------

bgra32_line_mix_x64:

	;[rsp+28h] - ka_table
	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    
    push		rdi
    push		rsi
      
	mov			rdi, rcx;
	mov			rsi, rdx;
    mov			ecx, r8d;
	mov			rdx, [rsp+28h + 10h];
	
	sub			rsp,32
	movups		[rsp],xmm6
	movups		[rsp + 16],xmm7
	
	test		ecx, ecx;
	jnz			loop_bgra32_line_mix_a16;
	jmp			loop_bgra32_line_mix_a8;
	
		
loop_bgra32_line_mix_a16:
	; 根据 (sa * 256 + da) * 4 查增益表，由于表为 DWORD 型，转成字节时需要乘以长度
	movdqu		xmm6, [rsi];	; src(A3R3G3B3 A2R2G2B2 A1R1G1B1 A0R0G0B0)
	movdqu		xmm7, [rdi];	; des(A3R3G3B3 A2R2G2B2 A1R1G1B1 A0R0G0B0)
	movdqa		xmm4, xmm6;
	movdqa		xmm5, xmm7;
	psrld		xmm4, 18h;		; sa(src alpha A3 A2 A1 A0)
	psrld		xmm5, 18h;		;; da(des alpha A3 A2 A1 A0)
	movd		xmm1, rdx;		; 表头 0 0 0 table
	pshufd		xmm1, xmm1, 00h;	; table table table table
	movdqa		xmm2, xmm4;		; sa
	movdqa		xmm3, xmm4;		; sa
	pmullw		xmm4, xmm5;		; sa * da
	paddd		xmm3, xmm5;		; sa + da
	pslld		xmm2, 0ah;		; sa * 256 * 4
	pslld		xmm5, 02h;		; da * 4
	paddd		xmm2, xmm5;		; (sa * 256 + da) * 4 存放 Offset3 Offset2 Offset1 Offset0
	paddd		xmm2, xmm1;		; 增益地址 ptr3 ptr2 ptr1 ptr0
	paddd		xmm4, oword ptr[gc_dwMaskDLB];	; sa * da + 255
	psrld		xmm4, 08h;		; (sa * da + 255) >> 8
	psubd		xmm3, xmm4;		; 计算 MA = SA + DA - (SA * DA + 255) >> 8
	pslld		xmm3, 18h;		; 存放结果 A000 A000 A000 A000
	
	; 挨个像素取出增益值
	movd		eax, xmm2;		; 第 0 个像素的增益 ptr0
	pshufd		xmm2, xmm2, 39h;	; ptr0 ptr3 ptr2 ptr1
	movd		xmm4, dWORD PTR[eax];	; 存放 0 0 0 K0
	movd		eax, xmm2;		; 第 1 个像素的增益 ptr1
	pshufd		xmm2, xmm2, 39h;	; ptr1 ptr0 ptr3 ptr2
	movd		xmm5, dWORD PTR[eax];	; 存放 0 0 0 K1
	movd		eax, xmm2;		; 第 2 个像素的增益 ptr2
	pshufd		xmm2, xmm2, 39h;	; ptr2 ptr1 ptr0 ptr3
	movd		xmm1, dWORD PTR[eax];	; 存放 0 0 0 K2
	movd		eax, xmm2;		; 第 3 个像素的增益 ptr3
	shufps		xmm4, xmm5, 00h;	; K1 K1 K0 K0
	movd		xmm2, dWORD PTR[eax];	; 存放 0 0 0 K3
	shufps		xmm1, xmm2, 00h;	; K3 K3 K2 K2
	packssdw	xmm4, xmm1;		; K3K3 K2K2 K1K1 K0K0
	
	; 先计算 R B 再单独计算 G
	movdqa		xmm2, xmm6;
	movdqa		xmm5, xmm7;
	psrld		xmm6, 08h;
	psrld		xmm7, 08h;
	pand		xmm2, oword ptr[gc_dwMaskWLB];	; src RB(0R0B 0R0B 0R0B 0R0B)
	pand		xmm5, oword ptr[gc_dwMaskWLB];	; des RB(0R0B 0R0B 0R0B 0R0B)
	pand		xmm6, oword ptr[gc_dwMaskDLB];	; src G(000G 000G 000G 000G)
	pand		xmm7, oword ptr[gc_dwMaskDLB];	; des G(000G 000G 000G 000G)
	movdqa		xmm0, xmm2;		; srb
	movdqa		xmm1, xmm6;		; sg
	pcmpgtw		xmm2, xmm5;		; (srb > drb) ? -1 : 0
	pcmpgtw		xmm6, xmm7;		; (sg > dg) ? -1 : 0
	psubw		xmm0, xmm5;		; srb - drb
	psubw		xmm1, xmm7;		; sg - dg
	psubw		xmm0, xmm2;		; srb - drb + compRB
	psubw		xmm1, xmm6;		; sg - dg + compG
	paddw		xmm0, xmm0;		; (srb - drb + compRB) * 2 浮点乘法到整型乘法的转变
	paddw		xmm1, xmm1;		; (sg - dg + compG) * 2 浮点乘法到整型乘法的转变
	pmulhw		xmm0, xmm4;		; (srb - drb) * KA
	pmulhw		xmm1, xmm4;		; (sg - dg) * KA
	paddw		xmm0, xmm5;		; (srb - drb) * KA + drb
	paddw		xmm1, xmm7;		; (sg - dg) * KA + dg
	pslld		xmm1, 08h;
	por			xmm3, xmm0;		; mix |= (mrb) 存放 AR0B AR0B AR0B AR0B
	por			xmm3, xmm1;		; mix |= (mg << 8) 存放 ARGB ARGB ARGB ARGB
	
	; 写入 Mix 结果并开始下一个连续的四个像素计算
	movdqu		[rdi], xmm3;
	add			rsi, 10h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_bgra32_line_mix_a16;
	
loop_bgra32_line_mix_a8:
	mov			rcx, r9;
	pxor		xmm0, xmm0;		; 0 0 0 0
	test		ecx, 00000008h;
	jz			loop_bgra32_line_mix_a4;
	
	movlps		xmm6, QWORD PTR[rsi];	; src(@ @ A1R1G1B1 A0R0G0B0)
	movlps		xmm7, QWORD PTR[rdi];	; des(@ @ A1R1G1B1 A0R0G0B0)
	movdqa		xmm4, xmm6;
	movdqa		xmm5, xmm7;
	psrld		xmm4, 18h;		; sa(src alpha @ @ A1 A0)
	psrld		xmm5, 18h;		; da(des alpha @ @ A1 A0)
	movd		xmm1, rdx;		; 表头 0 0 0 table
	pshufd		xmm1, xmm1, 00h;	; table table table table
	movdqa		xmm2, xmm4;		; sa
	movdqa		xmm3, xmm4;		; sa
	pmullw		xmm4, xmm5;		; sa * da
	paddd		xmm3, xmm5;		; sa + da
	pslld		xmm2, 0ah;		; sa * 256 * 4
	pslld		xmm5, 02h;		; da * 4
	paddd		xmm2, xmm5;		; (sa * 256 + da) * 4 存放 @ @ Offset1 Offset0
	paddd		xmm2, xmm1;		; 增益地址 @ @ ptr1 ptr0
	paddd		xmm4, oword ptr[gc_dwMaskDLB];	; sa * da + 255
	psrld		xmm4, 08h;		; (sa * da + 255) >> 8
	psubd		xmm3, xmm4;		; 计算 MA = SA + DA - (SA * DA + 255) >> 8
	pslld		xmm3, 18h;		; 存放结果 0000 0000 A000 A000
	
	; 挨个像素取出增益值
	movd		eax, xmm2;		; 第 0 个像素的增益 ptr0
	pshufd		xmm2, xmm2, 39h;	; ptr0 0 0 ptr1
	movd		xmm4, DWORD PTR[eax];	; 存放 0 0 0 K0
	movd		eax, xmm2;		; 第 1 个像素的增益 ptr1
	movd		xmm5, DWORD PTR[eax];	; 存放 0 0 0 K1
	shufps		xmm4, xmm4, 00h;	; K0 K0 K0 K0
	shufps		xmm5, xmm5, 00h;	; K1 K1 K1 K1
	packssdw	xmm4, xmm5;		; K1K1 K1K1 K0K0 K0K0
	
	; 同时计算 R G B
	pand		xmm6, oword ptr[gc_dwMaskRGB];; src RGB(@ @ 0RGB 0RGB)
	pand		xmm7, oword ptr[gc_dwMaskRGB];; des RGB(@ @ 0RGB 0RGB)
	punpcklbw	xmm6, xmm0;		; 000R 0G0B 000R 0G0B
	punpcklbw	xmm7, xmm0;		; 000R 0G0B 000R 0G0B
	movdqa		xmm2, xmm6;		; src RGB
	pcmpgtw		xmm2, xmm7;		; (src > des) ? -1 : 0
	psubw		xmm6, xmm7;		; src - des
	psubw		xmm6, xmm2;		; src - des + compRGB
	paddw		xmm6, xmm6;		; (src - des + compRGB) * 2 浮点乘法到整型乘法的转变
	pmulhw		xmm6, xmm4;		; (src - des) * KA
	paddw		xmm6, xmm7;		; (src - des) * KA + des
	packuswb	xmm6, xmm0;		; 0000 0000 0RGB 0RGB
	por			xmm3, xmm6;		; mix |= (RGB) 存放 0000 0000 ARGB ARGB
	
	; 写入 Mix 结果
	movlps		QWORD PTR[rdi], xmm3;
	add			rsi, 08h;
	add			rdi, 08h;
	
loop_bgra32_line_mix_a4:
	test		ecx, 00000004h;
	jz			loop_bgra32_line_mix_end;

	movd		xmm1, rdx;		; 表头 0 0 0 table
	movd		xmm6, DWORD PTR[rsi];	; src(0000 0000 0000 ARGB)
	movd		xmm7, DWORD PTR[rdi];	; des(0000 0000 0000 ARGB)
	movdqa		xmm4, xmm6;
	movdqa		xmm5, xmm7;
	psrld		xmm4, 18h;		; sa(src alpha 0 0 0 A0)
	psrld		xmm5, 18h;		; da(des alpha 0 0 0 A0)
	movdqa		xmm2, xmm4;		; sa
	movdqa		xmm3, xmm4;		; sa
	pmullw		xmm4, xmm5;		; sa * da
	paddd		xmm3, xmm5;		; sa + da
	pslld		xmm2, 0ah;		; sa * 256 * 4
	pslld		xmm5, 02h;		; da * 4
	paddd		xmm2, xmm5;		; (sa * 256 + da) * 4 存放 0 0 0 Offset0
	paddd		xmm2, xmm1;		; 增益地址 0 0 0 ptr0
	paddd		xmm4, oword ptr[gc_dwMaskDLB];	; sa * da + 255
	psrld		xmm4, 08h;		; (sa * da + 255) >> 8
	psubd		xmm3, xmm4;		; 计算 MA = SA + DA - (SA * DA + 255) >> 8
	pslld		xmm3, 18h;		; 存放结果 0000 0000 0000 A000
	
	; 挨个像素取出增益值
	movd		eax, xmm2;		; 第 0 个像素的增益 ptr0
	movd		xmm4, dWORD PTR[eax];	; 存放 0 0 0 K0
	shufps		xmm4, xmm4, 00h;	; K0 K0 K0 K0
	packssdw	xmm4, xmm0;		; 0 0 K0K0 K0K0
	pand		xmm6, oword ptr[gc_dwMaskRGB];; src RGB(0000 0000 0000 0RGB)
	pand		xmm7, oword ptr[gc_dwMaskRGB];; des RGB(0000 0000 0000 0RGB)
	punpcklbw	xmm6, xmm0;		; 0000 0000 000R 0G0B
	punpcklbw	xmm7, xmm0;		; 0000 0000 000R 0G0B
	movdqa		xmm2, xmm6;		; src RGB
	pcmpgtw		xmm2, xmm7;		; (src > des) ? -1 : 0
	psubw		xmm6, xmm7;		; src - des
	psubw		xmm6, xmm2;		; src - des + compRGB
	paddw		xmm6, xmm6;		; (src - des + compRGB) * 2 浮点乘法到整型乘法的转变
	pmulhw		xmm6, xmm4;		; (src - des) * KA
	paddw		xmm6, xmm7;		; (src - des) * KA + des
	packuswb	xmm6, xmm0;		; 0000 0000 0000 0RGB
	por			xmm3, xmm6;		; mix |= (RGB) 存放 0000 0000 0000 ARGB
	
	; 写入 Mix 结果
	movss		DWORD PTR[rdi], xmm3;
	
loop_bgra32_line_mix_end:

	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 16]
	add			rsp,32
		
	pop			rsi
	pop			rdi
    ret
    
 
;-----------------------------------------------------------------------------
; void bgra32_bl_copy_line_x64(OUT void* des,		// 目标拷贝行
;							IN int dcw,				// 目标拷贝行的宽度
;							IN void* src,			// 源拷贝行
;							IN int src_pitch,		// 源图一行的字节数
;							IN void* pPowerX,		// 目标映射到源的 X 方向上的列权重
;							IN void* pMapX,			// 目标映射到源的 X 方向上的列索引
;							IN DWORD powerY);		// 目标行映射到源行的行权重
;-----------------------------------------------------------------------------

bgra32_bl_copy_line_x64:
	;[rsp+38h] - powerY
	;[rsp+30h] - pMapX
	;[rsp+28h] - pPowerX
	;r9 - src_pitch
	;r8 - src
	;rdx - dcw
    ;rcx - des 
    
    push		rdi
    push		rsi

      
	pxor		xmm0, xmm0;			; 0 0 0 0
	movd		xmm3, DWORD PTR[rsp+38h+10h];	; 行权重 0000 0000 0000 0y0y
	movdqa		xmm5, oword ptr[gc_nComp_N1];	; ffffffff ffffffff ffffffff ffffffff
	shufps		xmm3, xmm3, 00h;	; 0y0y 0y0y 0y0y 0y0y
	psubusw		xmm5, xmm3;			; 1 - y
	mov 		rdi, rcx;			; 目标行
	mov 		rsi, [rsp+28h+10h];	; X 方向上的权重表
	mov 		ecx, edx;			; 行宽
	mov 		rdx, [rsp+30h+10h]	; X 方向上的索引表
	test		ecx, ecx;
	jnz			loop_bgra32_bl_copy_line_pixel;
	jmp			loop_bgra32_bl_copy_line_end;
	
loop_bgra32_bl_copy_line_pixel:
	mov			eax, DWORD PTR[rdx] ;; 目标行上的映射索引 map
	add			rax, r8;			; 第一像素的地址 src + map
	movlps		xmm1, QWORD PTR[rax];		; @ @ S10 S00
	punpcklbw	xmm1, xmm0;			; 0a0r 0g0b 0a0r 0g0b
	paddw		xmm1, oword ptr[gc_nRoundComp_1];	;; 误差补偿
	pmulhuw		xmm1, xmm3;			; ((S@0 + comp) * y) >> 16
	add			rax, r9;			; 下一行的地址
	add			rdx, 04h;
	movlps		xmm2, QWORD PTR[rax];		; @ @ S11 S01
	punpcklbw	xmm2, xmm0;			; 0a0r 0g0b 0a0r 0g0b
	paddw		xmm2, oword ptr[gc_nRoundComp_1];	; 误差补偿
	pmulhuw		xmm2, xmm5;			; ((S@1 + comp) * (65535 - y)) >> 16
	movd		xmm4, DWORD PTR[rsi];		; 目标行上的映射权重 0 0 0 (1-x)x
	shufps		xmm4, xmm4, 00h;	; (1-x)x (1-x)x (1-x)x (1-x)x
	pshuflw		xmm4, xmm4, 00h;	; (1-x)x (1-x)x xx xx
	pshufhw		xmm4, xmm4, 55h;	; (1-x)(1-x) (1-x)(1-x) xx xx
	paddusw		xmm1, oword ptr[gc_nRoundComp_1];
	paddusw		xmm1, xmm2;			; S@0 * y + S@1 * (1 - y)
	pmulhuw		xmm1, xmm4;			; S1@ * (1 - x)	S0@ * x
	pshufd		xmm2, xmm1, 0eh;	; S0@ * x			S1@ * (1 - x)
	paddusw		xmm1, xmm2;			; 0a0r 0g0b 0a0r 0g0b
	packuswb	xmm1, xmm1;			; D0 D0 D0 D0
	movd		DWORD PTR[rdi], xmm1;
	add			rsi, 04h;
	add			rdi, 04h;
	sub			ecx, 01h;
	jnz			loop_bgra32_bl_copy_line_pixel;
	
loop_bgra32_bl_copy_line_end:

	pop			rsi
	pop			rdi
    ret
    
    

;-----------------------------------------------------------------------------
; void _memcpy_rev_char_x64(OUT void* des,		// 目标
;							 IN void* src,		// 源
;							 IN int nlen,		// 宽度（用以 seek 到最后一段起始点）
;							 IN int na16,		// 宽度除以 16 的商
;							 IN int nm16);		// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

_memcpy_rev_char_x64:
	;[rsp+28h] - nm16
	;r9 - na16
	;r8 - nlen
	;rdx - src
    ;rcx - des 
    
    push		rdi
    push		rsi    
    
    mov			rdi, rcx;
	mov			rcx, r9;
	add			rdi, r8;			// seek 到目标最后点
	mov			rsi, rdx;
	sub			rdi, 10h;			// 先前移 16 个字节到达第一个写入段
	movdqa		xmm5, oword ptr[gc_dwMaskG];	// 0000ff00h
	test		ecx, ecx;
	jnz			loop_memcpy_rev_char_a16;
	jmp			loop_memcpy_rev_char_m8;
loop_memcpy_rev_char_a16:
	movdqu		xmm0, [rsi];		// Pf Pd Pe Pc  Pb Pa P9 P8  P7 P6 P5 P4  P3 P2 P1 P0
	shufps		xmm0, xmm0, 1bh;	// P3 P2 P1 P0  P7 P6 P5 P4  Pb Pa P9 P8  Pf Pd Pe Pc
	movdqa		xmm1, xmm0;
	movdqa		xmm2, xmm0;
	movdqa		xmm3, xmm0;
	pand		xmm1, xmm5;			// 0 0 P1 0  0 0 P5 0  0 0 P9 0  0 0 Pe 0
	pslld		xmm0, 18h;			// P0 0 0 0  P4 0 0 0  P8 0 0 0  Pc 0 0 0
	pslld		xmm1, 08h;			// 0 P1 0 0  0 P5 0 0  0 P9 0 0  0 Pe 0 0
	psrld		xmm2, 08h;			// 0 P3 P2 P1  0 P7 P6 P5  0 Pb Pa P9  0 Pf Pd Pe
	psrld		xmm3, 18h;			// 0 0 0 P3  0 0 0 P7  0 0 0 Pb  0 0 0 Pf
	pand		xmm2, xmm5;			// 0 0 P2 0  0 0 P6 0  0 0 Pa 0  0 0 Pd 0
	por			xmm0, xmm1;			// P0 P1 0 0  P4 P5 0 0  P8 P9 0 0  Pc Pe 0 0
	por			xmm2, xmm3;			// 0 0 P2 P3  0 0 P6 P7  0 0 Pa Pb  0 0 Pd Pf
	por			xmm0, xmm2;			// P0 P1 P2 P3  P4 P5 P6 P7  P8 P9 Pa Pb  Pc Pe Pd Pf
	movdqu		[rdi], xmm0;
	sub			rdi, 10h;
	add			rsi, 10h;
	sub			ecx, 01h;
	jnz			loop_memcpy_rev_char_a16;
loop_memcpy_rev_char_m8:
	mov			ecx, [rsp+28h+10h];
	add			rdi, 08h;			// 使之只前移 8 个字节
	test		ecx, 08h;
	jz			loop_memcpy_rev_char_m4;
	movlps		xmm0, QWORD PTR[rsi];		// @  @  P7 P6 P5 P4  P3 P2 P1 P0
	shufps		xmm0, xmm0, 11h;	// P0  P1  P0  P1
	movdqa		xmm1, xmm0;
	movdqa		xmm2, xmm0;
	movdqa		xmm3, xmm0;
	pand		xmm1, xmm5;			// 0 0 P1 0  0 0 P5 0  0 0 P1 0  0 0 P5 0
	pslld		xmm0, 18h;			// P0 0 0 0  P4 0 0 0  P0 0 0 0  P4 0 0 0
	pslld		xmm1, 08h;			// 0 P1 0 0  0 P5 0 0  0 P1 0 0  0 P5 0 0
	psrld		xmm2, 08h;			// 0 P3 P2 P1  0 P7 P6 P5  0 P3 P2 P1  0 P7 P6 P5
	psrld		xmm3, 18h;			// 0 0 0 P3  0 0 0 P7  0 0 0 P3  0 0 0 P7
	pand		xmm2, xmm5;			// 0 0 P2 0  0 0 P6 0  0 0 P2 0  0 0 P6 0
	por			xmm0, xmm1;			// P0 P1 0 0  P4 P5 0 0  P0 P1 0 0  P4 P5 0 0
	por			xmm2, xmm3;			// 0 0 P2 P3  0 0 P6 P7  0 0 P2 P3  0 0 P6 P7
	por			xmm0, xmm2;			// P0 P1 P2 P3  P4 P5 P6 P7  P0 P1 P2 P3  P4 P5 P6 P7
	movlps		QWORD PTR[rdi], xmm0;
	sub			rdi, 08h;
	add			rsi, 08h;
loop_memcpy_rev_char_m4:
	add			rdi, 04h;			// 使之只前移 4 个字节
	test		ecx, 04h;
	jz			loop_memcpy_rev_char_m2;
	movss		xmm0, DWORD PTR[rsi];		// 0 0 0  P3 P2 P1 P0
	movdqa		xmm1, xmm0;
	movdqa		xmm2, xmm0;
	movdqa		xmm3, xmm0;
	pand		xmm1, xmm5;			// 0  0  0  0 0 P1 0
	pslld		xmm0, 18h;			// 0  0  0  P0 0 0 0
	pslld		xmm1, 08h;			// 0  0  0  0 P1 0 0
	psrld		xmm2, 08h;			// 0  0  0  0 P3 P2 P1
	psrld		xmm3, 18h;			// 0  0  0  0 0 0 P3
	pand		xmm2, xmm5;			// 0  0  0  0 0 P2 0
	por			xmm0, xmm1;			// 0  0  0  P0 P1 0 0
	por			xmm2, xmm3;			// 0  0  0  0 0 P2 P3
	por			xmm0, xmm2;			// 0  0  0  P0 P1 P2 P3
	movss		DWORD PTR[rdi], xmm0;
	sub			rdi, 04h;
	add			rsi, 04h;
loop_memcpy_rev_char_m2:
	add			rdi, 02h;			// 使之只前移 2 个字节
	test		ecx, 02h;
	jz			loop_memcpy_rev_char_m1;
	mov			ax, [rsi];			// P1 P0
	mov			edx, eax;
	shl			eax, 08h;
	shr			edx, 08h;
	and			eax, 0000ff00h;		// 0 0 P0 0
	and			edx, 000000ffh;		// 0 0 0 P1
	or			eax, edx;			// 0 0 P0 P1
	mov			[rdi], ax;
	sub			rdi, 02h;
	add			rsi, 02h;
loop_memcpy_rev_char_m1:
	test		ecx, 01h;
	jz			loop_memcpy_rev_char_end;
	add			rdi, 01h;
	mov			al, [rsi];			// P0
	mov			[rdi], al;
loop_memcpy_rev_char_end:
    
    pop			rsi
	pop			rdi
    ret
    
    

;-----------------------------------------------------------------------------
; void _memcpy_rev_dword_x64(OUT void* des,	// 目标
;						   IN void* src,		// 源
;						   IN int nlen,			// 宽度（用以 seek 到最后一段起始点）
;						   IN int na16,			// 宽度除以 16 的商
;						   IN int nm16);		// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

_memcpy_rev_dword_x64:
	;[rsp+28h] - nm16
	;r9 - na16
	;r8 - nlen
	;rdx - src
    ;rcx - des 
    
    push		rdi
    push		rsi    
    
    mov			rdi, rcx;
	mov			rcx, r9;
	add			rdi, r8;			// seek 到目标最后点
	mov			rsi, rdx;
	sub			rdi, 10h;			// 先前移 16 个字节到达第一个写入段
	test		ecx, ecx;
	jnz			loop_memcpy_rev_dword_a16;
	jmp			loop_memcpy_rev_dword_m8;
loop_memcpy_rev_dword_a16:
	movdqu		xmm0, [rsi];		// P3 P2 P1 P0
	shufps		xmm0, xmm0, 1bh;	// P0 P1 P2 P3
	movdqu		[rdi], xmm0;
	sub			rdi, 10h;
	add			rsi, 10h;
	sub			ecx, 01h;
	jnz			loop_memcpy_rev_dword_a16;
loop_memcpy_rev_dword_m8:
	mov			ecx, [rsp+38h];
	add			rdi, 08h;			// 使之只前移 8 个字节
	test		ecx, 08h;
	jz			loop_memcpy_rev_dword_m4;
	movlps		xmm0, QWORD PTR[rsi];		// @  @  P1  P0
	shufps		xmm0, xmm0, 11h;	// P1  P0  P0  P1
	movlps		QWORD PTR[rdi], xmm0;
	sub			rdi, 08h;
	add			rsi, 08h;
loop_memcpy_rev_dword_m4:
	test		ecx, 04h;
	jz			loop_memcpy_rev_dword_end;
	add			rdi, 04h;			// 使之只前移 4 个字节
	movss		xmm0, DWORD PTR[rsi];		// 0 0 0 P0
	movss		DWORD PTR[rdi], xmm0;
loop_memcpy_rev_dword_end:
    
    pop			rsi
	pop			rdi
    ret
    
;-----------------------------------------------------------------------------
;void _yuv420_to_yuyv_line_2_F_x64(OUT BYTE* yuyv,        // yuyv 输出，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
;							 IN int yuyv_p,
;							 IN BYTE* y,            // y 输入，存放方式 Y0 Y1 Y2 Y3 ……
;							 IN int y_p_2,
;							 IN BYTE* u,            // u 输入，存放方式 U0 U2 U4 U6 ……
;							 IN int u_p_2,
;							 IN BYTE* v,            // v 输入，存放方式 V0 V2 V4 V6 ……
;							 IN int v_p_2,
;							 IN int na16,           // 一行像素宽度除以 16 的商与余数（注意目标为 16 位的缓存）
;							 IN int nm16,
;							int* gc_nRoundComp_2)
;-----------------------------------------------------------------------------
_yuv420_to_yuyv_line_2_F_x64:
	;rcx/xmm0	:	yuyv
	;rdx/xmm1	:	yuyv_p
	;r8 /xmm2	:	y
	;r9 /xmm3	:	y_p_2
	;[rsp+28h]	:	u
	;[rsp+30h]	:	u_p_2
	;[rsp+38h]	:	v
	;[rsp+40h]	:	v_p_2
	;[rsp+48h]	:	na16
	;[rsp+50h]	:	nm16
	;[rsp+58h]	:	gc_nRoundComp_2
	
		push		rsi
		push		rdi
		push		rbx;
		sub			rsp,20h
		movups		[rsp],xmm6
		movups		[rsp + 10h],xmm7

		mov         rdi, rcx;
		add         rdx, rcx;
		mov         ecx, [rsp+48h + 8h +30h];
		mov         rax, r8;
		mov         rbx, [rsp+28h + 8h +30h];
		mov         rsi, [rsp+38h + 8h +30h];
		test        ecx, ecx;
		jnz         yuv420_to_yuyv_line_2_F_loop_a16;
		jmp         yuv420_to_yuyv_line_2_F_loop_m8;
yuv420_to_yuyv_line_2_F_loop_a16:
		movdqu      xmm1, [rax];        // Y15 Y14 Y13 Y12  Y11 Y10 Y9 Y8  Y7 Y6 Y5 Y4  Y3 Y2 Y1 Y0
		movsd       xmm2, qword ptr[rbx];        // 0000 0000  UU14 UU12 UU10 UU8  UU6 UU4 UU2 UU0
		movsd       xmm3, qword ptr[rsi];        // 0000 0000  VV14 VV12 VV10 VV8  VV6 VV4 VV2 VV0
		movdqa      xmm4, xmm2;
		movdqa      xmm0, xmm1;
		punpcklbw   xmm2, xmm3;         // VV14 UU14 ……
		add			 rax, r9;
		add			 rbx, [rsp+30h + 8h+30h];
		add			rsi, [rsp+40h + 8h+30h];
		movdqu      xmm5, [rax];
		movsd	     xmm6, qword ptr[rbx];
		movsd       xmm7, qword ptr[rsi];
		sub			 rax, r9;
		sub			 rbx, [rsp+30h + 8h+30h];
		sub			rsi, [rsp+40h + 8h+30h];
		;// 结果为 UU_0
		punpcklbw   xmm0, xmm2;
		punpckhbw   xmm1, xmm2;
		pxor        xmm2, xmm2;         // 0
		punpcklbw   xmm4, xmm2;         // UU_0
		punpcklbw   xmm3, xmm2;         // VV_0
		punpcklbw   xmm6, xmm2;         // UU_2
		punpcklbw   xmm7, xmm2;         // VV_2
		;// 结果为 UU_0 * 0.75 + UU_2 * 0.25
		paddw       xmm6, xmm4;
		paddw       xmm7, xmm3;
		paddw       xmm4, xmm4;
		paddw       xmm3, xmm3;
		paddw       xmm6, [rsp+58h + 8h+30h];gc_nRoundComp_2
		paddw       xmm7, [rsp+58h + 8h+30h];gc_nRoundComp_2
		paddw       xmm6, xmm4;
		paddw       xmm7, xmm3;
		psrlw       xmm6, 002h;
		psllw       xmm7, 006h;
		movdqa      xmm4, xmm5;
		pand        xmm7, oword ptr[gc_dwMaskWHB];
		por         xmm6, xmm7;
		punpcklbw   xmm4, xmm6;
		punpckhbw   xmm5, xmm6;
		movdqu      [rdi], xmm0;
		movdqu      [rdx], xmm4;
		add         rdi, 010h;
		add         rdx, 010h;
		movdqu      [rdi], xmm1;
		movdqu      [rdx], xmm5;
		add         rax, 010h;
		add         rbx, 008h;
		add         rsi, 008h;
		add         rdi, 010h;
		add         rdx, 010h;
		sub         ecx, 001h;
		jnz         yuv420_to_yuyv_line_2_F_loop_a16;
yuv420_to_yuyv_line_2_F_loop_m8:
		mov         ecx, [rsp+50h + 8h+30h];
		test        ecx, 008h;
		jz          yuv420_to_yuyv_line_2_F_loop_m4;
		movsd       xmm1, qword ptr[rax];        // 0000 0000  Y7 Y6 Y5 Y4  Y3 Y2 Y1 Y0
		movss       xmm2, dword ptr[rbx];        // 0000 0000 0000 UU6 UU4 UU2 UU0
		movss       xmm3, dword ptr[rsi];        // 0000 0000 0000 VV6 VV4 VV2 VV0
		movdqa      xmm4, xmm2;
		pxor        xmm0, xmm0;         // 0
		punpcklbw    xmm2, xmm3;         // 0000 0000  VV6 UU6 ……
		add			 rax, r9;
		add			 rbx, [rsp+30h + 8h+30h];
		add			 rsi, [rsp+40h + 8h+30h];
		movsd       xmm5, qword ptr[rax];
		movss	    xmm6, dword ptr[rbx];
		movss       xmm7, dword ptr[rsi];
		sub			 rax, r9;
		sub			 rbx, [rsp+30h + 8h+30h];
		sub			 rsi, [rsp+40h + 10h+30h];//????????????????????????????????????
		;// 结果为 UU_0
		punpcklbw   xmm1, xmm2;
		punpcklbw   xmm4, xmm0;         // UU_0
		punpcklbw   xmm3, xmm0;         // VV_0
		punpcklbw   xmm6, xmm0;         // UU_2
		punpcklbw   xmm7, xmm0;         // VV_2
		;// 结果为 UU_0 * 0.75 + UU_2 * 0.25
		paddw       xmm6, xmm4;
		paddw       xmm7, xmm3;
		paddw       xmm4, xmm4;
		paddw       xmm3, xmm3;
		paddw       xmm6, [rsp+58h + 8h+30h];
		paddw       xmm7, [rsp+58h + 8h+30h];
		paddw       xmm6, xmm4;
		paddw       xmm7, xmm3;
		psrlw       xmm6, 002h;
		psllw       xmm7, 006h;
		pand        xmm7, oword ptr[gc_dwMaskWHB];
		por         xmm6, xmm7;
		punpcklbw   xmm5, xmm6;
		movdqu      [rdi], xmm1;
		movdqu      [rdx], xmm5;
		add         rax, 008h;
		add         rbx, 004h;
		add         rsi, 004h;
		add         rdi, 010h;
		add         rdx, 010h;
yuv420_to_yuyv_line_2_F_loop_m4:
		test        ecx, 004h;
		jz          yuv420_to_yuyv_line_2_F_loop_m2;
		pxor        xmm0, xmm0;         // 0
		pxor		xmm2, xmm2;
		pxor		xmm3, xmm3;
		movss       xmm1, dword ptr[rax];        // 0000 0000 0000  Y3 Y2 Y1 Y0
		pinsrw      xmm2, word ptr[rbx], 000h;  // UU2 UU0
		pinsrw      xmm3, word ptr[rsi], 000h;  // VV2 VV0
		movdqa      xmm4, xmm2;
		punpcklbw    xmm2, xmm3;         // 0000 0000 0000 VV2 UU2 VV0 UU0
		add			 rax, r9;
		add			 rbx, [rsp+30h + 8h+30h];
		add			 rsi, [rsp+40h + 8h+30h];
		movss       xmm5, dword ptr[rax];
		pinsrw      xmm6, word ptr[rbx], 000h;
		pinsrw      xmm7, word ptr[rsi], 000h;
		sub			 rax, r9;
		sub			 rbx, [rsp+30h + 8h+30h];
		sub			 rsi, [rsp+40h + 10h+30h];//????????????????????????????????????
		;// 结果为 UU_0
		punpcklbw   xmm1, xmm2;
		punpcklbw   xmm4, xmm0;         // UU_0
		punpcklbw   xmm3, xmm0;         // VV_0
		punpcklbw   xmm6, xmm0;         // UU_2
		punpcklbw   xmm7, xmm0;         // VV_2
		;// 结果为 UU_0 * 0.75 + UU_2 * 0.25
		paddw       xmm6, xmm4;
		paddw       xmm7, xmm3;
		paddw       xmm4, xmm4;
		paddw       xmm3, xmm3;
		paddw       xmm6, [rsp+58h + 8h+30h];
		paddw       xmm7, [rsp+58h + 8h+30h];
		paddw       xmm6, xmm4;
		paddw       xmm7, xmm3;
		psrlw       xmm6, 002h;
		psllw       xmm7, 006h;
		pand        xmm7, oword ptr[gc_dwMaskWHB];
		por         xmm6, xmm7;
		punpcklbw   xmm5, xmm6;
		movsd      qword ptr[rdi], xmm1;
		movsd      qword ptr[rdx], xmm5;
		add         rax, 004h;
		add         rbx, 002h;
		add         rsi, 002h;
		add         rdi, 008h;
		add         rdx, 008h;
yuv420_to_yuyv_line_2_F_loop_m2:
		test        ecx, 002h;
		jz          loop_end;
		push		rdx;
		push		rax;
		xor			ecx, ecx;
		xor			rdx, rdx;
		xor			eax, eax;
		mov			ch, [rbx];		// UU_C
		mov			dl, [rsi];		// VV_C
		add			rbx, [rsp+30h+10h + 8h+30h];
		add			rsi, [rsp+40h+10h + 8h+30h];
		mov			al, [rbx];		// UU_N
		xor			rbx, rbx;
		mov			bl, [rsi];		// VV_N
		xor			rsi, rsi;
		mov			rsi, rdx;
		shl			rsi, 018h;
		or			esi, ecx;		// VV_C 0 UU_C 0
		shr			ecx, 008h;
		add			ax, cx;
		add			bx, dx;
		add			cx, cx;
		add			dx, dx;
		add			ax, 002h;
		add			bx, 002h;
		add			ax, cx;
		add			bx, dx;
		and			eax, 0fffch;
		and			ebx, 0fffch;
		shl			eax, 006h;
		shl			ebx, 016h;
		or			ebx, eax;		// VV_N 0 UU_N 0
		pop			rax;
		mov			cx, [rax];
		add			rax, r9;
		mov			edx, ecx;
		and			ecx, 000ffh;
		and			edx, 0ff00h;
		or			ecx, esi;
		shl			edx, 008h;
		or			ecx, edx;
		mov			[rdi], ecx;
		pop			rdx;
		mov			cx, [rax];
		mov			edi, ecx;
		and			ecx, 000ffh;
		and			edi, 0ff00h;
		or			ecx, ebx;
		shl			edi, 008h;
		or			ecx, edi;
		mov			[rdx], ecx;
loop_end:
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 10h]
		add		rsp,20h
		pop		rbx;
		pop		rdi
		pop		rsi
	ret

;-----------------------------------------------------------------------------
;void _yuv420_to_yuyv_line_2_x64(OUT BYTE* yuyv,        // yuyv 输出，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
	;					   IN int yuyv_p,
	;					   IN BYTE* y,            // y 输入，存放方式 Y0 Y1 Y2 Y3 ……
	;					   IN int y_p_2,
	;					   IN BYTE* u,            // u 输入，存放方式 UU0 UU2 UU4 UU6 ……
	;					   IN int u_p_2,
	;					   IN BYTE* v,            // v 输入，存放方式 VV0 VV2 VV4 VV6 ……
	;					   IN int v_p_2,
	;;					   IN int na16,           // 一行像素宽度除以 16 的商与余数（注意目标为 16 位的缓存）
	;					   IN int nm16);
;-----------------------------------------------------------------------------
_yuv420_to_yuyv_line_2_x64:
	;rcx/xmm0	:	yuyv
	;rdx/xmm1	:	yuyv_p
	;r8 /xmm2	:	y
	;r9 /xmm3	:	y_p_2
	;[rsp+28h]	:	u
	;[rsp+30h]	:	u_p_2
	;[rsp+38h]	:	v
	;[rsp+40h]	:	v_p_2
	;[rsp+48h]	:	na16
	;[rsp+50h]	:	nm16

		push		rsi
		push		rdi
		push		rbx;
		sub			rsp,20h
		movups		[rsp],xmm6
		movups		[rsp + 10h],xmm7
		
		mov         rdi, rcx;
		add         rdx, rdx;
		mov         ecx, [rsp+48h + 38h];
		mov         rax, r8;
		mov         rbx, [rsp+28h + 38h];
		mov         rsi, [rsp+38h + 38h];
		test        ecx, ecx;
		jnz         yuv420_to_yuyv_line_2_loop_a16;
		jmp         yuv420_to_yuyv_line_2_loop_m8;
yuv420_to_yuyv_line_2_loop_a16:
		movsd       xmm4, qword ptr[rbx];        // 0000 0000  UU14 UU12 UU10 UU8  UU6 UU4 UU2 UU0
		movsd       xmm5, qword ptr[rsi];        // 0000 0000  VV14 VV12 VV10 VV8  VV6 VV4 VV2 VV0
		add         rbx, [rsp+30h + 38h];
		add         rsi, [rsp+40h + 38h];
		movsd       xmm6, qword ptr[rbx];    // UU_N
		movsd       xmm7, qword ptr[rsi];    // VV_N
		sub         rbx, [rsp+30h + 38h];
		sub         rsi, [rsp+40h + 38h];
		sub         rbx, [rsp+30h + 38h];
		sub         rsi, [rsp+40h + 38h];
		movsd       xmm2, qword ptr[rbx];        // UU_P
		movsd       xmm3, qword ptr[rsi];        // VV_P
		add         rbx, [rsp+30h + 38h];
		add         rsi, [rsp+40h + 38h];
		pxor        xmm0, xmm0;
		punpcklbw   xmm2, xmm0;     // UU_P
		punpcklbw   xmm3, xmm0;     // VV_P
		punpcklbw   xmm4, xmm0;     // UU_C
		punpcklbw   xmm5, xmm0;     // VV_C
		punpcklbw   xmm6, xmm0;     // UU_N
		punpcklbw   xmm7, xmm0;     // VV_N
		;// 第一个结果：UU_P * 0.25 + UU_C * 0.75、第二个结果：UU_N * 0.25 + UU_C * 0.75
		paddw       xmm2, xmm4;
		paddw       xmm3, xmm5;
		paddw       xmm6, xmm4;
		paddw       xmm7, xmm5;
		paddw       xmm4, xmm4;
		paddw       xmm5, xmm5;
		paddw       xmm2, xmm4;
		paddw       xmm3, xmm5;
		paddw       xmm6, xmm4;
		paddw       xmm7, xmm5;
		paddw       xmm2, oword ptr[gc_nRoundComp_2];
		paddw       xmm3, oword ptr[gc_nRoundComp_2];
		paddw       xmm6, oword ptr[gc_nRoundComp_2];
		paddw       xmm7, oword ptr[gc_nRoundComp_2];
		psrlw		 xmm2, 002h;
		psrlw		 xmm6, 002h;
		psllw       xmm3, 006h;
		psllw       xmm7, 006h;
		pand        xmm3, oword ptr[gc_dwMaskWHB];
		pand        xmm7, oword ptr[gc_dwMaskWHB];
		por         xmm2, xmm3;
		por         xmm6, xmm7;
		movdqu      xmm0, [rax];        // Y15 Y14 Y13 Y12  Y11 Y10 Y9 Y8  Y7 Y6 Y5 Y4  Y3 Y2 Y1 Y0
		add			rax, r9;
		movdqu      xmm4, [rax];
		sub			rax, r9;
		movdqa      xmm1, xmm0;
		movdqa      xmm5, xmm4;
		punpcklbw   xmm0, xmm2;
		punpcklbw   xmm4, xmm6;
		punpckhbw   xmm1, xmm2;
		punpckhbw   xmm5, xmm6;
		movdqu      [rdi], xmm0;
		movdqu      [rdx], xmm4;
		add         rdi, 010h;
		add         rdx, 010h;
		movdqu      [rdi], xmm1;
		movdqu      [rdx], xmm5;
		add         rax, 010h;
		add         rbx, 008h;
		add         rsi, 008h;
		add         rdi, 010h;
		add         rdx, 010h;
		sub         ecx, 001h;
		jnz         yuv420_to_yuyv_line_2_loop_a16;
yuv420_to_yuyv_line_2_loop_m8:
		mov         ecx, [rsp+50h + 38h];
		test        ecx, 008h;
		jz          yuv420_to_yuyv_line_2_loop_m4;
		movss       xmm4, dword ptr[rbx];        // 0000 0000 0000  UU6 UU4 UU2 UU0
		movss       xmm5, dword ptr[rsi];        // 0000 0000 0000  VV6 VV4 VV2 VV0
		add         rbx, [rsp+30h + 38h];
		add         rsi, [rsp+40h + 38h];
		movss       xmm6, dword ptr[rbx];    // UU_N
		movss       xmm7, dword ptr[rsi];    // VV_N
		sub         rbx, [rsp+30h + 38h];
		sub         rsi, [rsp+40h + 38h];
		sub         rbx, [rsp+30h + 38h];
		sub         rsi, [rsp+40h + 38h];
		movss       xmm2, dword ptr[rbx];        // UU_P
		movss       xmm3, dword ptr[rsi];        // VV_P
		add         rbx, [rsp+30h + 38h];
		add         rsi, [rsp+40h + 38h];
		pxor        xmm0, xmm0;
		punpcklbw   xmm2, xmm0;     // UU_P
		punpcklbw   xmm3, xmm0;     // VV_P
		punpcklbw   xmm4, xmm0;     // UU_C
		punpcklbw   xmm5, xmm0;     // VV_C
		punpcklbw   xmm6, xmm0;     // UU_N
		punpcklbw   xmm7, xmm0;     // VV_N
		;// 第一个结果：UU_P * 0.25 + UU_C * 0.75、第二个结果：UU_N * 0.25 + UU_C * 0.75
		paddw       xmm2, xmm4;
		paddw       xmm3, xmm5;
		paddw       xmm6, xmm4;
		paddw       xmm7, xmm5;
		paddw       xmm4, xmm4;
		paddw       xmm5, xmm5;
		paddw       xmm2, xmm4;
		paddw       xmm3, xmm5;
		paddw       xmm6, xmm4;
		paddw       xmm7, xmm5;
		paddw       xmm2, oword ptr[gc_nRoundComp_2];
		paddw       xmm3, oword ptr[gc_nRoundComp_2];
		paddw       xmm6, oword ptr[gc_nRoundComp_2];
		paddw       xmm7, oword ptr[gc_nRoundComp_2];
		psrlw		 xmm2, 002h;
		psrlw		 xmm6, 002h;
		psllw       xmm3, 006h;
		psllw       xmm7, 006h;
		pand        xmm3, oword ptr[gc_dwMaskWHB];
		pand        xmm7, oword ptr[gc_dwMaskWHB];
		por         xmm2, xmm3;
		por         xmm6, xmm7;
		movsd      xmm0, qword ptr[rax];        // 0000 0000  Y7 Y6 Y5 Y4  Y3 Y2 Y1 Y0
		add			rax, r9;
		movsd      xmm4, qword ptr[rax];
		sub			rax, r9;
		punpcklbw   xmm0, xmm2;
		punpcklbw   xmm4, xmm6;
		movdqu      [rdi], xmm0;
		movdqu      [rdx], xmm4;
		add         rax, 008h;
		add         rbx, 004h;
		add         rsi, 004h;
		add         rdi, 010h;
		add         rdx, 010h;
yuv420_to_yuyv_line_2_loop_m4:
		test        ecx, 004h;
		jz          yuv420_to_yuyv_line_2_loop_m2;
		pxor        xmm0, xmm0;
		pxor		xmm2, xmm2;
		pxor		xmm3, xmm3;
		pxor		xmm4, xmm4;
		pxor		xmm5, xmm5;
		pxor		xmm6, xmm6;
		pxor		xmm7, xmm7;
		pinsrw      xmm4, word ptr[rbx], 000h;        // 0000 0000 0000  0 0 UU2 UU0
		pinsrw      xmm5, word ptr[rsi], 000h;        // 0000 0000 0000  0 0 VV2 VV0
		add         rbx, [rsp+30h + 38h];
		add         rsi, [rsp+40h + 38h];
		pinsrw      xmm6, word ptr[rbx], 000h;    // UU_N
		pinsrw      xmm7, word ptr[rsi], 000h;    // VV_N
		sub         rbx, [rsp+30h + 38h];
		sub         rsi, [rsp+40h + 38h];
		sub         rbx, [rsp+30h + 38h];
		sub         rsi, [rsp+40h + 38h];
		pinsrw      xmm2, word ptr[rbx], 000h;        // UU_P
		pinsrw      xmm3, word ptr[rsi], 000h;        // VV_P
		add         rbx, [rsp+30h + 38h];
		add         rsi, [rsp+40h + 38h];
		punpcklbw   xmm2, xmm0;     // UU_P
		punpcklbw   xmm3, xmm0;     // VV_P
		punpcklbw   xmm4, xmm0;     // UU_C
		punpcklbw   xmm5, xmm0;     // VV_C
		punpcklbw   xmm6, xmm0;     // UU_N
		punpcklbw   xmm7, xmm0;     // VV_N
		;// 第一个结果：UU_P * 0.25 + UU_C * 0.75、第二个结果：UU_N * 0.25 + UU_C * 0.75
		paddw       xmm2, xmm4;
		paddw       xmm3, xmm5;
		paddw       xmm6, xmm4;
		paddw       xmm7, xmm5;
		paddw       xmm4, xmm4;
		paddw       xmm5, xmm5;
		paddw       xmm2, xmm4;
		paddw       xmm3, xmm5;
		paddw       xmm6, xmm4;
		paddw       xmm7, xmm5;
		paddw       xmm2, oword ptr[gc_nRoundComp_2];
		paddw       xmm3, oword ptr[gc_nRoundComp_2];
		paddw       xmm6, oword ptr[gc_nRoundComp_2];
		paddw       xmm7, oword ptr[gc_nRoundComp_2];
		psrlw		 xmm2, 002h;
		psrlw		 xmm6, 002h;
		psllw       xmm3, 006h;
		psllw       xmm7, 006h;
		pand        xmm3, oword ptr[gc_dwMaskWHB];
		pand        xmm7, oword ptr[gc_dwMaskWHB];
		por         xmm2, xmm3;
		por         xmm6, xmm7;
		movss      xmm0, dword ptr[rax];        // 0000 0000 0000  Y3 Y2 Y1 Y0
		add			rax, r9;
		movss      xmm4, dword ptr[rax];
		sub			rax, r9;
		punpcklbw   xmm0, xmm2;
		punpcklbw   xmm4, xmm6;
		movsd      qword ptr[rdi], xmm0;
		movsd      qword ptr[rdx], xmm4;
		add         rax, 004h;
		add         rbx, 002h;
		add         rsi, 002h;
		add         rdi, 008h;
		add         rdx, 008h;
yuv420_to_yuyv_line_2_loop_m2:
		test        ecx, 002h;
		jz          loop_end;
		push		rdx;
		push		rdi;
		push		rax;
		xor			edx, edx;
		xor			eax, eax;
		mov			dl, [rbx];		// UU0
		mov			al, [rsi];		// VV0
		mov			edi, edx;
		shl			eax, 010h;
		or			edi, eax;		// 0 VV_C 0 UU_C
		add			rbx, [rsp+30h+18h + 38h];
		add			rsi, [rsp+40h+18h + 38h];
		xor			edx, edx;
		xor			eax, eax;
		mov			dl, [rbx];		// UU0
		mov			al, [rsi];		// VV0
		mov			ecx, edx;
		shl			eax, 010h;
		or			ecx, eax;		// 0 VV_N 0 UU_N
		sub         rbx, [rsp+30h + 38h];
		sub         rsi, [rsp+40h + 38h];
		sub         rbx, [rsp+30h + 38h];
		sub         rsi, [rsp+40h + 38h];
		xor			edx, edx;
		xor			eax, eax;
		mov			dl, [rbx];		// UU_P
		mov			al, [rsi];		// VV_P
		mov			ebx, edi;		// UU_C
		mov			esi, ecx;		// UU_N
		shr			edi, 010h;		// VV_C
		shr			ecx, 010h;		// VV_N
		add			dx, bx;
		add			si, bx;
		add			ax, di;
		add			cx, di;
		add			bx, bx;
		add			di, di;
		add			dx, 002h;
		add			si, 002h;
		add			ax, 002h;
		add			cx, 002h;
		add			dx, bx;		// UU_1
		add			si, bx;		// UU_2
		add			ax, di;		// VV_1
		add			cx, di;		// VV_2
		and			edx, 0fffch;
		and			esi, 0fffch;
		and			eax, 0fffch;
		and			ecx, 0fffch;
		shl			edx, 006h;
		shl			esi, 006h;
		shl			eax, 016h;
		shl			ecx, 016h;
		or			edx, eax;	// VV_1 0 UU_1 0
		or			esi, ecx;	// VV_2 0 UU_2 0
		pop			rax;
		xor			ebx, ebx;
		xor			ecx, ecx;
		mov			bx, [rax];	// Y_1
		add			rax, r9;
		mov			cx, [rax];	// Y_2
		mov			edi, ebx;
		mov			eax, ecx;
		and			ebx, 000ffh;
		and			ecx, 000ffh;
		and			edi, 0ff00h;
		and			eax, 0ff00h;
		shl			edi, 008h;
		shl			eax, 008h;
		or			ebx, edi;
		or			ecx, eax;
		or			ebx, edx;
		or			ecx, esi;
		pop			rdi;
		pop			rdx;
		mov			[rdi], ebx;
		mov			[rdx], ecx;
yuv420_to_yuyv_line_2_loop_end:
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 10h]
		add			rsp,20h
		pop			rbx;
		pop			rdi
		pop			rsi
	ret
;-----------------------------------------------------------------------------
;void _yuv420_to_yuyv_line_2_L_x64(OUT BYTE* yuyv,        // yuyv 输出，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
;							 IN int yuyv_p,
;							 IN BYTE* y,            // y 输入，存放方式 Y0 Y1 Y2 Y3 ……
;							 IN int y_p_2,
;							 IN BYTE* u,            // u 输入，存放方式 UU0 UU2 UU4 UU6 ……
;							 IN int u_p_2,
;							 IN BYTE* v,            // v 输入，存放方式 VV0 VV2 VV4 VV6 ……
;							 IN int v_p_2,
;							 IN int na16,           // 一行像素宽度除以 16 的商与余数（注意目标为 16 位的缓存）
;							 IN int nm16)
;-----------------------------------------------------------------------------
_yuv420_to_yuyv_line_2_L_x64:
	;rcx/xmm0	:	yuyv
	;rdx/xmm1	:	yuyv_p
	;r8 /xmm2	:	y
	;r9 /xmm3	:	y_p_2
	;[rsp+28h]	:	u
	;[rsp+30h]	:	u_p_2
	;[rsp+38h]	:	v
	;[rsp+40h]	:	v_p_2
	;[rsp+48h]	:	na16
	;[rsp+50h]	:	nm16

		push		rsi
		push		rdi
		push		rbx;
		sub			rsp,20h
		movups		[rsp],xmm6
		movups		[rsp + 10h],xmm7
		
		mov         rdi, rcx;
		add         rdx, rcx;
		mov         ecx, [rsp+48h + 38h];
		mov         rax, r8;
		mov         rbx, [rsp+28h + 38h];
		mov         rsi, [rsp+38h + 38h];
		sub         rbx, [rsp+30h + 38h];
		sub         rsi, [rsp+40h + 38h];
		test        ecx, ecx;
		jnz         yuv420_to_yuyv_line_2_L_loop_a16;
		jmp         yuv420_to_yuyv_line_2_L_loop_m8;
yuv420_to_yuyv_line_2_L_loop_a16:
		movdqu      xmm1, [rax];        // Y15 Y14 Y13 Y12  Y11 Y10 Y9 Y8  Y7 Y6 Y5 Y4  Y3 Y2 Y1 Y0
		movsd       xmm2, qword ptr[rbx];        // 0000 0000  UU14 UU12 UU10 UU8  UU6 UU4 UU2 UU0
		movsd       xmm3, qword ptr[rsi];        // 0000 0000  VV14 VV12 VV10 VV8  VV6 VV4 VV2 VV0
		add			 rax, r9;
		add			 rbx, [rsp+30h + 38h];
		add			rsi, [rsp+40h + 38h];
		movdqu      xmm5, [rax];
		movsd       xmm6, qword ptr[rbx];
		movsd       xmm7, qword ptr[rsi];
		sub			 rax, r9;
		sub			 rbx, [rsp+30h + 38h];
		sub 		 rsi, [rsp+40h + 38h];
		movdqa      xmm0, xmm6;
		movdqa      xmm4, xmm5;
		punpcklbw   xmm0, xmm7;         // VV14 UU14 ……
		;// 结果为 UU_2
		punpcklbw   xmm4, xmm0;
		punpckhbw   xmm5, xmm0;
		pxor        xmm0, xmm0;         // 0
		punpcklbw   xmm2, xmm0;         // UU_0
		punpcklbw   xmm3, xmm0;         // VV_0
		punpcklbw   xmm6, xmm0;         // UU_2
		punpcklbw   xmm7, xmm0;         // VV_2
		;// 结果为 UU_0 * 0.25 + UU_2 * 0.75
		paddw       xmm2, xmm6;
		paddw       xmm3, xmm7;
		paddw       xmm6, xmm6;
		paddw       xmm7, xmm7;
		paddw       xmm2, oword ptr[gc_nRoundComp_2];
		paddw       xmm3, oword ptr[gc_nRoundComp_2];
		paddw       xmm2, xmm6;
		paddw       xmm3, xmm7;
		psrlw       xmm2, 002h;
		psllw       xmm3, 006h;
		movdqa      xmm0, xmm1;
		pand        xmm3, oword ptr[gc_dwMaskWHB];
		por         xmm2, xmm3;
		punpcklbw   xmm0, xmm2;
		punpckhbw   xmm1, xmm2;
		movdqu      [rdi], xmm0;
		movdqu      [rdx], xmm4;
		add         rdi, 010h;
		add         rdx, 010h;
		movdqu      [rdi], xmm1;
		movdqu      [rdx], xmm5;
		add         rax, 010h;
		add         rbx, 008h;
		add         rsi, 008h;
		add         rdi, 010h;
		add         rdx, 010h;
		sub         ecx, 001h;
		jnz         yuv420_to_yuyv_line_2_L_loop_a16;
yuv420_to_yuyv_line_2_L_loop_m8:
		mov         ecx, [rsp+50h + 38h];
		pxor		 xmm0, xmm0;
		test        ecx, 008h;
		jz          yuv420_to_yuyv_line_2_L_loop_m4;
		movsd       xmm1, qword ptr[rax];        // 0000 0000  Y7 Y6 Y5 Y4  Y3 Y2 Y1 Y0
		movss       xmm2, dword ptr[rbx];        // 0000 0000 0000  UU6 UU4 UU2 UU0
		movss       xmm3, dword ptr[rsi];        // 0000 0000 0000  VV6 VV4 VV2 VV0
		add			 rax, r9;
		add			 rbx, [rsp+30h + 38h];
		add			 rsi, [rsp+40h + 38h];
		movsd       xmm5, qword ptr[rax];
		movss       xmm6, dword ptr[rbx];
		movss       xmm7, dword ptr[rsi];
		sub			 rax, r9;
		sub			 rbx, [rsp+30h + 38h];
		sub 		 rsi, [rsp+40h + 38h];
		movdqa      xmm4, xmm6;
		punpcklbw   xmm4, xmm7;         // 0000 0000  VV6 UU6 ……
		;// 结果为 UU_2
		punpcklbw   xmm5, xmm4;
		punpcklbw   xmm2, xmm0;         // UU_0
		punpcklbw   xmm3, xmm0;         // VV_0
		punpcklbw   xmm6, xmm0;         // UU_2
		punpcklbw   xmm7, xmm0;         // VV_2
		;// 结果为 UU_0 * 0.25 + UU_2 * 0.75
		paddw       xmm2, xmm6;
		paddw       xmm3, xmm7;
		paddw       xmm6, xmm6;
		paddw       xmm7, xmm7;
		paddw       xmm2, oword ptr[gc_nRoundComp_2];
		paddw       xmm3, oword ptr[gc_nRoundComp_2];
		paddw       xmm2, xmm6;
		paddw       xmm3, xmm7;
		psrlw       xmm2, 002h;
		psllw       xmm3, 006h;
		pand        xmm3, oword ptr[gc_dwMaskWHB];
		por         xmm2, xmm3;
		punpcklbw   xmm1, xmm2;
		movdqu      [rdi], xmm1;
		movdqu      [rdx], xmm5;
		add         rax, 008h;
		add         rbx, 004h;
		add         rsi, 004h;
		add         rdi, 010h;
		add         rdx, 010h;
yuv420_to_yuyv_line_2_L_loop_m4:
		test        ecx, 004h;
		jz          yuv420_to_yuyv_line_2_L_loop_m2;
		movss       xmm1, dword ptr[rax];        // 0000 0000 0000  Y3 Y2 Y1 Y0
		pinsrw      xmm2, word ptr[rbx], 000h;   // 0000 0000 0000  0 0 UU2 UU0
		pinsrw      xmm3, word ptr[rsi], 000h;   // 0000 0000 0000  0 0 VV2 VV0
		add			 rax, r9;
		add			 rbx, [rsp+30h + 38h];
		add			 rsi, [rsp+40h + 38h];
		movsd       xmm5, qword ptr[rax];
		pinsrw      xmm6, word ptr[rbx], 000h;
		pinsrw      xmm7, word ptr[rsi], 000h;
		sub			 rax, r9;
		sub			 rbx, [rsp+30h + 38h];
		sub 		 rsi, [rsp+40h + 38h];
		movdqa      xmm4, xmm6;
		punpcklbw   xmm4, xmm7;         // 0000 0000 0000  VV2 UU2 VV0 UU0
		;// 结果为 UU_2
		punpcklbw   xmm5, xmm4;
		punpcklbw   xmm2, xmm0;         // UU_0
		punpcklbw   xmm3, xmm0;         // VV_0
		punpcklbw   xmm6, xmm0;         // UU_2
		punpcklbw   xmm7, xmm0;         // VV_2
		;// 结果为 UU_0 * 0.25 + UU_2 * 0.75
		paddw       xmm2, xmm6;
		paddw       xmm3, xmm7;
		paddw       xmm6, xmm6;
		paddw       xmm7, xmm7;
		paddw       xmm2, oword ptr[gc_nRoundComp_2];
		paddw       xmm3, oword ptr[gc_nRoundComp_2];
		paddw       xmm2, xmm6;
		paddw       xmm3, xmm7;
		psrlw       xmm2, 002h;
		psllw       xmm3, 006h;
		pand        xmm3, oword ptr[gc_dwMaskWHB];
		por         xmm2, xmm3;
		punpcklbw   xmm1, xmm2;
		movsd       qword ptr[rdi], xmm1;
		movsd       qword ptr[rdx], xmm5;
		add         rax, 004h;
		add         rbx, 002h;
		add         rsi, 002h;
		add         rdi, 008h;
		add         rdx, 008h;
yuv420_to_yuyv_line_2_L_loop_m2:
		test        ecx, 002h;
		jz          loop_end;
		push		rdx;
		push		rax;
		xor			ecx, ecx;
		xor			edx, edx;
		xor			eax, eax;
		mov			cl, [rbx];		// UU_P
		mov			dl, [rsi];		// VV_P
		add			ebx, [rsp+30h+10h + 38h];
		add			esi, [rsp+40h+10h + 38h];
		mov			ah, [rbx];		// UU_C
		xor			ebx, ebx;
		mov			bl, [rsi];		// VV_C
		xor			rsi, rsi;
		mov			esi, ebx;
		shl			esi, 018h;
		or			esi, eax;		// VV_C 0 UU_C 0
		shr			eax, 008h;
		add			cx, ax;
		add			dx, bx;
		add			ax, ax;
		add			bx, bx;
		add			cx, 002h;
		add			dx, 002h;
		add			ax, cx;
		add			bx, dx;
		and			eax, 0fffch;
		and			ebx, 0fffch;
		shl			eax, 006h;
		shl			ebx, 016h;
		or			ebx, eax;		// VV_N 0 UU_N 0
		pop			rax;
		mov			cx, [rax];
		add			rax, r9;
		mov			edx, ecx;
		and			ecx, 000ffh;
		and			edx, 0ff00h;
		or			ecx, esi;
		shl			edx, 008h;
		or			ecx, edx;
		mov			[rdi], ecx;
		pop			rdx;
		mov			cx, [rax];
		mov			edi, ecx;
		and			ecx, 000ffh;
		and			edi, 0ff00h;
		or			ecx, ebx;
		shl			edi, 008h;
		or			ecx, edi;
		mov			[rdx], ecx;
yuv420_to_yuyv_line_2_L_loop_end:
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 10h]
		add			rsp,20h
		pop			rbx;
		pop			rdi
		pop			rsi
	ret
;-----------------------------------------------------------------------------
;void _yuv420_to_yuyv_line_1_L_x64(OUT BYTE* yuyv,        // yuyv 输出，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
	;						 IN BYTE* y,            // y 输入，存放方式 Y0 Y1 Y2 Y3 ……
	;						 IN BYTE* u,            // u 输入，存放方式 UU0 UU2 UU4 UU6 ……
	;						 IN int u_p_2,
	;						 IN BYTE* v,            // v 输入，存放方式 VV0 VV2 VV4 VV6 ……
	;						 IN int v_p_2,
	;						 IN int na16,           // 一行像素宽度除以 16 的商与余数（注意目标为 16 位的缓存）
	;						 IN int nm16)
;-----------------------------------------------------------------------------
_yuv420_to_yuyv_line_1_L_x64:
	;rcx/xmm0	:	yuyv
	;rdx/xmm1	:	yuyv_p
	;r8 /xmm2	:	y
	;r9 /xmm3	:	u
	;[rsp+28h]	:	u_p_2
	;[rsp+30h]	:	v
	;[rsp+38h]	:	v_p_2
	;[rsp+40h]	:	na16
	;[rsp+48h]	:	nm16

		push		rsi
		push		rdi
		push		rbx;
		sub			rsp,20h
		movups		[rsp],xmm6
		movups		[rsp + 10h],xmm7
		
		mov         rdi, rcx;
		mov         ecx, [rsp+40h + 38h];
		mov         rax, r8;
		mov         rbx, r9;
		mov         rsi, [rsp+30h + 38h];
		sub         rbx, [rsp+28h + 38h];
		sub         rsi, [rsp+38h + 38h];
		pxor        xmm0, xmm0;
		movdqa      xmm4, oword ptr[gc_nRoundComp_2];
		movdqa      xmm5, oword ptr[gc_dwMaskWHB];
		test        ecx, ecx;
		jnz         yuv420_to_yuyv_line_1_L_loop_a16;
		jmp         yuv420_to_yuyv_line_1_L_loop_m8;
yuv420_to_yuyv_line_1_L_loop_a16:
		movdqu      xmm1, [rax];        // Y15 Y14 Y13 Y12  Y11 Y10 Y9 Y8  Y7 Y6 Y5 Y4  Y3 Y2 Y1 Y0
		movsd       xmm2, qword ptr[rbx];        // 0000 0000  UU14 UU12 UU10 UU8  UU6 UU4 UU2 UU0
		movsd       xmm3, qword ptr[rsi];        // 0000 0000  VV14 VV12 VV10 VV8  VV6 VV4 VV2 VV0
		add			 rbx, [rsp+28h + 38h];
		add			 rsi, [rsp+38h + 38h];
		movsd       xmm6, qword ptr[rbx];
		movsd       xmm7, qword ptr[rsi];
		sub			 rbx, [rsp+28h + 38h];
		sub			 rsi, [rsp+38h + 38h];
		punpcklbw   xmm2, xmm0;         // UU_0
		punpcklbw   xmm3, xmm0;         // VV_0
		punpcklbw   xmm6, xmm0;         // UU_2
		punpcklbw   xmm7, xmm0;         // VV_2
		;// 结果为 UU_0 * 0.25 + UU_2 * 0.75
		paddw       xmm2, xmm6;
		paddw       xmm3, xmm7;
		paddw       xmm6, xmm6;
		paddw       xmm7, xmm7;
		paddw       xmm2, xmm4;
		paddw       xmm3, xmm4;
		paddw       xmm2, xmm6;
		paddw       xmm3, xmm7;
		psrlw       xmm2, 002h;
		psllw       xmm3, 006h;
		movdqa      xmm7, xmm1;
		pand        xmm3, xmm5;
		por         xmm2, xmm3;
		punpcklbw   xmm1, xmm2;
		punpckhbw   xmm7, xmm2;
		movdqu      [rdi], xmm1;
		add         rax, 010h;
		add         rdi, 010h;
		movdqu      [rdi], xmm7;
		add         rbx, 008h;
		add         rsi, 008h;
		add         rdi, 010h;
		sub         ecx, 001h;
		jnz         yuv420_to_yuyv_line_1_L_loop_a16;
yuv420_to_yuyv_line_1_L_loop_m8:
		mov         ecx, [rsp+48h + 38h];
		test        ecx, 008h;
		jz          yuv420_to_yuyv_line_1_L_loop_m4;
		movsd       xmm1, qword ptr[rax];        // 0000 0000  Y7 Y6 Y5 Y4  Y3 Y2 Y1 Y0
		movss       xmm2, dword ptr[rbx];        // 0000 0000 0000  UU6 UU4 UU2 UU0
		movss       xmm3, dword ptr[rsi];        // 0000 0000 0000  VV6 VV4 VV2 VV0
		add			 rbx, [rsp+28h + 38h];
		add			 rsi, [rsp+38h + 38h];
		movss       xmm6, dword ptr[rbx];
		movss       xmm7, dword ptr[rsi];
		sub			 rbx, [rsp+28h + 38h];
		sub			 rsi, [rsp+38h + 38h];
		punpcklbw   xmm2, xmm0;         // UU_0
		punpcklbw   xmm3, xmm0;         // VV_0
		punpcklbw   xmm6, xmm0;         // UU_2
		punpcklbw   xmm7, xmm0;         // VV_2
		;// 结果为 UU_0 * 0.25 + UU_2 * 0.75
		paddw       xmm2, xmm6;
		paddw       xmm3, xmm7;
		paddw       xmm6, xmm6;
		paddw       xmm7, xmm7;
		paddw       xmm2, xmm4;
		paddw       xmm3, xmm4;
		paddw       xmm2, xmm6;
		paddw       xmm3, xmm7;
		psrlw       xmm2, 002h;
		psllw       xmm3, 006h;
		pand        xmm3, xmm5;
		por         xmm2, xmm3;
		punpcklbw   xmm1, xmm2;
		movdqu      [rdi], xmm1;
		add         rax, 008h;
		add         rbx, 004h;
		add         rsi, 004h;
		add         rdi, 010h;
yuv420_to_yuyv_line_1_L_loop_m4:
		test        ecx, 004h;
		jz          yuv420_to_yuyv_line_1_L_loop_m2;
		pxor		xmm2, xmm2;
		pxor		xmm3, xmm3;
		pxor		xmm6, xmm6;
		pxor		xmm7, xmm7;
		movss       xmm1, dword ptr[rax];        // 0000 0000 0000  Y3 Y2 Y1 Y0
		pinsrw		xmm2, word ptr[rbx], 000h;   // 0000 0000 0000  0 0 UU2 UU0
		pinsrw		xmm3, word ptr[rsi], 000h;   // 0000 0000 0000  0 0 VV2 VV0
		add			 rbx, [rsp+28h + 38h];
		add			 rsi, [rsp+38h + 38h];
		pinsrw		xmm6, word ptr[rbx], 000h;
		pinsrw		xmm7, word ptr[rsi], 000h;
		sub			 rbx, [rsp+28h + 38h];
		sub			 rsi, [rsp+38h + 38h];
		punpcklbw   xmm2, xmm0;         // UU_0
		punpcklbw   xmm3, xmm0;         // VV_0
		punpcklbw   xmm6, xmm0;         // UU_2
		punpcklbw   xmm7, xmm0;         // VV_2
		;// 结果为 UU_0 * 0.25 + UU_2 * 0.75
		paddw       xmm2, xmm6;
		paddw       xmm3, xmm7;
		paddw       xmm6, xmm6;
		paddw       xmm7, xmm7;
		paddw       xmm2, xmm4;
		paddw       xmm3, xmm4;
		paddw       xmm2, xmm6;
		paddw       xmm3, xmm7;
		psrlw       xmm2, 002h;
		psllw       xmm3, 006h;
		pand        xmm3, xmm5;
		por         xmm2, xmm3;
		punpcklbw    xmm1, xmm2;
		movsd       qword ptr[rdi], xmm1;
		add         rax, 004h;
		add         rbx, 002h;
		add         rsi, 002h;
		add         rdi, 008h;
yuv420_to_yuyv_line_1_L_loop_m2:
		test        ecx, 002h;
		jz          yuv420_to_yuyv_line_1_L_loop_end;
		push		rax;
		xor			ecx, ecx;
		xor			edx, edx;
		xor			eax, eax;
		mov			cl, [rbx];
		mov			dl, [rsi];
		add			rbx, [rsp+28h+8h + 38h];
		add			rsi, [rsp+38h+8h + 38h];
		mov			al, [rbx];
		xor			rbx, rbx;
		mov			bl, [rsi];
		add			cx, ax;
		add			dx, bx;
		add			ax, ax;
		add			bx, bx;
		add			cx, 002h;
		add			dx, 002h;
		add			cx, ax;
		add			dx, bx;
		and			ecx, 0fffch;
		and			edx, 0fffch;
		shl			ecx, 006h;
		shl			edx, 016h;
		pop			rax;
		xor			rbx, rbx;
		mov			bx, [rax];
		mov			esi, ebx;
		and			ebx, 000ffh;
		and			esi, 0ff00h;
		shl			esi, 008h;
		or			ecx, edx;
		or			ebx, esi;
		or			ecx, ebx;
		mov			[rdi], ecx;
yuv420_to_yuyv_line_1_L_loop_end:
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 10h]
		add			rsp,20h
		pop			rbx;
		pop			rdi
		pop			rsi
	ret
;end add
     

;-----------------------------------------------------------------------------
; void _memcpy_dword_col_x64(OUT void* des,	// 目标起点
;						   IN void* src,		// 源
;						   IN int npitch,	// 目标一行的宽度（符号决定目标指针的前进方向）
;						   IN int na16,		// 拷贝列高度乘于 4 后除以 16 的商
;						   IN int nm16);		// 拷贝列高度乘于 4 后除以 16 的余数
;-----------------------------------------------------------------------------

_memcpy_dword_col_x64:
	;[rsp+28h] - nm16
	;r9 - na16
	;r8 - npitch
	;rdx - src
    ;rcx - des 
    
    push		rdi
    push		rsi        
	push		rbx;
    
    movsxd      rax,r8d 
    movsxd      rbx,r8d 
    ;mov			rax, r8;
	;mov			rbx, r8;
	mov			rdi, rcx;			// p0
	mov			rsi, rdx;
	mov			rdx, rcx;
	mov			rcx, r9;
	
	add			rbx, rbx;
	add			rdx, rbx;			// p2
	add			rbx, rbx;			// pitch * 4
	test		ecx, ecx;
	jnz			loop_memcpy_dword_col_a16;
	jmp			loop_memcpy_dword_col_m8;
loop_memcpy_dword_col_a16:
	push		rcx;
	push		rbx;
	mov			rcx, rdx;
	mov			rbx, rdi;
	add			rcx, rax;			// p3
	add			rbx, rax;			// p1
	movdqu		xmm1, [rsi];
	movdqa		xmm2, xmm1;
	movhlps		xmm3, xmm1;
	movhlps		xmm4, xmm1;
	shufps		xmm2, xmm2, 01h;
	shufps		xmm4, xmm4, 01h;
	movss		DWORD PTR[rdi], xmm1;		// p0
	movss		DWORD PTR[rbx], xmm2;		// p1
	movss		DWORD PTR[rdx], xmm3;		// p2
	movss		DWORD PTR[rcx], xmm4;		// p3
	pop			rbx;
	pop			rcx;
	add			rdi, rbx;
	add			rdx, rbx;
	add			rsi, 10h;
	sub			ecx, 01h;
	jnz			loop_memcpy_dword_col_a16;
loop_memcpy_dword_col_m8:
	mov			ecx, [rsp+28h + 18h];
	test		ecx, 08h;
	jz			loop_memcpy_dword_col_m4;
	mov			rbx, rdi;
	add			rbx, rax;
	movlps		xmm1, QWORD PTR[rsi];
	movdqa		xmm2, xmm1;
	shufps		xmm2, xmm2, 01h;
	movss		DWORD PTR[rdi], xmm1;		// p0
	movss		DWORD PTR[rbx], xmm2;		// p1
	add			rsi, 08h;
	mov			rdi, rdx;
loop_memcpy_dword_col_m4:
	test		ecx, 04h;
	jz			loop_memcpy_dword_col_end;
	mov			eax, [rsi];
	mov			[rdi], eax;
loop_memcpy_dword_col_end:
     
	pop			rbx;
    pop			rsi
	pop			rdi
    ret
    
        
;-----------------------------------------------------------------------------
; void _memset_char_line_x64(OUT void *des,	// 目标
;						   IN char value,	// 设置值
;						   IN int na16,		// 宽度除以 16 的商
;						   IN int nm16);	// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

_memset_char_line_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - value
    ;rcx - des 
    
    push		rdi
    push		rsi    
    
    mov			rdi, rcx;
	mov			rcx, r8;
	movzx		rax, dx;			// 000c
	mov			rdx, r9;
	movd		xmm0, eax;			// 0000 0000 0000 000c
	pshuflw		xmm0, xmm0, 00h;	// 0000 0000 0c0c 0c0c
	packuswb	xmm0, xmm0;			// 0000 cccc 0000 cccc
	pshufd		xmm0, xmm0, 00h;	// cccc cccc cccc cccc
	movd		eax, xmm0;			// cccc
	test		ecx, ecx;
	jnz			loop_memset_char_line_a16;
	jmp			loop_memset_char_line_a8;
loop_memset_char_line_a16:
	movdqu		[rdi], xmm0;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_memset_char_line_a16;
loop_memset_char_line_a8:
	test		edx, 00000008h;
	jz			loop_memset_char_line_a4;
	movlps		QWORD PTR[rdi], xmm0;
	add			rdi, 08h;
loop_memset_char_line_a4:
	test		edx, 00000004h;
	jz			loop_memset_char_line_a2;
	mov			[rdi], eax;
	add			rdi, 04h;
loop_memset_char_line_a2:
	test		edx, 00000002h;
	jz			loop_memset_char_line_a1;
	mov			[rdi], ax;
	add			rdi, 02h;
loop_memset_char_line_a1:
	test		edx,00000001h;
	jz			loop_memset_char_line_end;
	mov			[rdi], al; 
loop_memset_char_line_end:
     
    pop			rsi
	pop			rdi
    ret

;------------------------------------------------------------------------------
;void copy_dword_line_mula_invert_x64(OUT void *des,	// 目标
;							  IN void *src,	// 设置值
;							  IN int na16,		// 宽度除以 16 的商
;							  IN int nm16)		// 宽度除以 16 的余数
;------------------------------------------------------------------------------							      
 copy_dword_line_mula_invert_x64:
 	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    push		rdi
    push		rsi  
    
	mov			rdi, rcx;
	mov			rsi, rdx;
	mov			rcx, r8;
	mov			rdx, r9;
	movdqa		xmm1,oword ptr[gc_dwMaskAlpha];

	test		ecx, ecx;
	jnz			loop_copy_dword_line_mula_a16;
	jmp			loop_copy_dword_line_mula_a8;
	
	;test		ecx, ecx;
	;jnz			loop_a16;
	;jmp			loop_a8;
loop_copy_dword_line_mula_a16:
	movdqu		xmm3,[rdi];//目的
	movdqu		xmm0,[rsi];//源
	movdqa		xmm5,xmm3;
	pxor		xmm0,xmm1;
	pand		xmm0,xmm1;//源取alpha
	pand		xmm5,oword ptr[gc_dwMaskRGB];//取RGB
	pand		xmm3,xmm1;//目的alpha
	psrld		xmm0,8;//左移8位
	psrld		xmm3,8;//左移8位
	pmullw		xmm0,xmm3;//alpha相乘
	pand		xmm0,oword ptr[gc_dwMaskAlpha];//取alpha
	por			xmm5,xmm0;

	movdqu		[rdi], xmm5;

	add			rdi, 10h;
	add			rsi, 10h;
	sub			ecx, 01h;
	jnz			loop_copy_dword_line_mula_a16;
loop_copy_dword_line_mula_a8:
	test		edx, 00000008h;
	jz			loop_copy_dword_line_mula_a4;
	movsd		xmm3,QWORD PTR[rdi];
	movsd		xmm0,QWORD PTR[rsi];
	movdqa		xmm5,xmm3;
	pxor		xmm0,xmm1;
	pand		xmm0,xmm1;//源取alpha
	pand		xmm5,oword ptr[gc_dwMaskRGB];//取RGB
	pand		xmm3,xmm1;//目的alpha
	psrld		xmm0,8;//左移8位
	psrld		xmm3,8;//左移8位
	pmullw		xmm0,xmm3;//alpha相乘
	pand		xmm0,oword ptr[gc_dwMaskAlpha];//取alpha
	por			xmm5,xmm0;
	movlps		QWORD PTR[rdi], xmm5;
	add			rdi, 08h;
	add			rsi, 08h;
loop_copy_dword_line_mula_a4:
	test		edx, 00000004h;
	jz			loop_copy_dword_line_mula_end;
	movd		xmm3,DWORD PTR[rdi];
	movd		xmm0,DWORD PTR[rsi];
	movdqa		xmm5,xmm3;
	pxor		xmm0,xmm1;
	pand		xmm0,xmm1;//源取alpha
	pand		xmm5,oword ptr[gc_dwMaskRGB];//取RGB
	pand		xmm3,xmm1;//目的alpha
	psrld		xmm0,8;//左移8位
	psrld		xmm3,8;//左移8位
	pmullw		xmm0,xmm3;//alpha相乘
	pand		xmm0,oword ptr[gc_dwMaskAlpha];//取alpha
	por			xmm5,xmm0;
	movd		DWORD PTR[rdi], xmm5;
loop_copy_dword_line_mula_end:
	pop			rsi
	pop			rdi
	ret
	
	    
;------------------------------------------------------------------------------
;void copy_dword_line_mula_x64(OUT void *des,	// 目标
;							  IN void *src,	// 设置值
;							  IN int na16,		// 宽度除以 16 的商
;							  IN int nm16)		// 宽度除以 16 的余数
;------------------------------------------------------------------------------							      
 copy_dword_line_mula_x64:
 	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    push		rdi
    push		rsi  
    
	mov			rdi, rcx;
	mov			rsi, rdx;
	mov			rcx, r8;
	mov			rdx, r9;
	movdqa		xmm1,oword ptr[gc_dwMaskAlpha];

	test		ecx, ecx;
	jnz			loop_copy_dword_line_mula_invert_a16;
	jmp			loop_copy_dword_line_mula_invert_a8;
	
	;test		ecx, ecx;
	;jnz			loop_a16;
	;jmp			loop_a8;
loop_copy_dword_line_mula_invert_a16:
	movdqu		xmm3,[rdi];//目的
	movdqu		xmm0,[rsi];//源
	movdqa		xmm5,xmm3;
	pand		xmm0,xmm1;//源取alpha
	pand		xmm5,oword ptr[gc_dwMaskRGB];//取RGB
	pand		xmm3,xmm1;//目的alpha
	psrld		xmm0,8;//左移8位
	psrld		xmm3,8;//左移8位
	pmullw		xmm0,xmm3;//alpha相乘
	pand		xmm0,oword ptr[gc_dwMaskAlpha];//取alpha
	por			xmm5,xmm0;

	movdqu		[rdi], xmm5;

	add			rdi, 10h;
	add			rsi, 10h;
	sub			ecx, 01h;
	jnz			loop_copy_dword_line_mula_invert_a16;
loop_copy_dword_line_mula_invert_a8:
	test		edx, 00000008h;
	jz			loop_copy_dword_line_mula_invert_a4;
	movsd		xmm3,QWORD PTR[rdi];
	movsd		xmm0,QWORD PTR[rsi];
	movdqa		xmm5,xmm3;
	pand		xmm0,xmm1;//源取alpha
	pand		xmm5,oword ptr[gc_dwMaskRGB];//取RGB
	pand		xmm3,xmm1;//目的alpha
	psrld		xmm0,8;//左移8位
	psrld		xmm3,8;//左移8位
	pmullw		xmm0,xmm3;//alpha相乘
	pand		xmm0,oword ptr[gc_dwMaskAlpha];//取alpha
	por			xmm5,xmm0;
	movlps		QWORD PTR[rdi], xmm5;
	add			rdi, 08h;
	add			rsi, 08h;
loop_copy_dword_line_mula_invert_a4:
	test		edx, 00000004h;
	jz			loop_copy_dword_line_mula_invert_end;
	movd		xmm3,DWORD PTR[rdi];//rdi是64位寄存器；原先的edi是32位寄存器，容易造成寻址越界,HTH
	movd		xmm0,DWORD PTR[rsi];//rsi是64位寄存器
	movdqa		xmm5,xmm3;
	pand		xmm0,xmm1;//源取alpha
	pand		xmm5,oword ptr[gc_dwMaskRGB];//取RGB
	pand		xmm3,xmm1;//目的alpha
	psrld		xmm0,8;//左移8位
	psrld		xmm3,8;//左移8位
	pmullw		xmm0,xmm3;//alpha相乘
	pand		xmm0,oword ptr[gc_dwMaskAlpha];//取alpha
	por			xmm5,xmm0;
	movd		DWORD PTR[rdi], xmm5;
loop_copy_dword_line_mula_invert_end:
	pop			rsi
	pop			rdi
	ret
	
;-----------------------------------------------------------------------------
; void copy_dword_line_rgb_mula_x64(OUT void *des,	// 目标
;							  IN void *src,	// 设置值
;							  IN int na16,		// 宽度除以 16 的商
;							  IN int nm16);		// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

copy_dword_line_rgb_mula_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    
    push		rdi
    push		rsi    
    
    mov			rdi, rcx;
	mov			rsi, rdx;
	mov			rcx, r8;
	mov			rdx, r9;
	movdqa		xmm1,oword ptr[gc_dwMaskAlpha];
	
	test		ecx, ecx;
	jnz			loop_copy_dword_line_rgb_mula_a16;
	jmp			loop_copy_dword_line_rgb_mula_a8;
loop_copy_dword_line_rgb_mula_a16:
	movdqu		xmm3,[rdi];
	movdqu		xmm0,[rsi];
	pand		xmm3,xmm1;
	movdqa		xmm4,xmm0;
	psrld		xmm3,8h;
	pand		xmm4,oword ptr[gc_dwMaskAlpha];
	pand		xmm0,oword ptr[gc_dwMaskRGB];
	psrld		xmm4,8h;
	pmullw		xmm3,xmm4;
	pand		xmm3,oword ptr[gc_dwMaskAlpha];
	por			xmm3,xmm0;
	movdqu		[rdi], xmm3;

	add			rdi, 10h;
	add			rsi, 10h;
	sub			ecx, 01h;
	jnz			loop_copy_dword_line_rgb_mula_a16;
loop_copy_dword_line_rgb_mula_a8:
	test		edx, 00000008h;
	jz			loop_copy_dword_line_rgb_mula_a4;
	movsd		xmm3,QWORD PTR[rdi];
	movsd		xmm0,QWORD PTR[rsi];
	pand		xmm3,xmm1;
	movdqa		xmm4,xmm0;
	psrld		xmm3,8h;
	pand		xmm4,oword ptr[gc_dwMaskAlpha];
	pand		xmm0,oword ptr[gc_dwMaskRGB];
	psrld		xmm4,8h;
	pmullw		xmm3,xmm4;
	pand		xmm3,oword ptr[gc_dwMaskAlpha];
	por			xmm3,xmm0;
	movlps		QWORD PTR[rdi], xmm3;
	add			rdi, 08h;
	add			rsi, 08h;
loop_copy_dword_line_rgb_mula_a4:
	test		edx, 00000004h;
	jz			loop_copy_dword_line_rgb_mula_end;
	movd		xmm3,DWORD PTR[rdi];
	movd		xmm0,DWORD PTR[rsi];
	pand		xmm3,xmm1;
	movdqa		xmm4,xmm0;
	psrld		xmm3,8h;
	pand		xmm4,oword ptr[gc_dwMaskAlpha];
	pand		xmm0,oword ptr[gc_dwMaskRGB];
	psrld		xmm4,8h;
	pmullw		xmm3,xmm4;
	pand		xmm3,oword ptr[gc_dwMaskAlpha];
	por			xmm3,xmm0;
	movd		DWORD PTR[rdi], xmm3;
loop_copy_dword_line_rgb_mula_end:
     
    pop			rsi
	pop			rdi
    ret
    
 
 
 
;-----------------------------------------------------------------------------
; void memset_dword_line_rgb_mula_x64(OUT void *des,	// 目标
;						   IN DWORD value,	// 设置值
;						   IN int na16,		// 宽度除以 16 的商
;						   IN int nm16);		// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

memset_dword_line_rgb_mula_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - value
    ;rcx - des 
    
    push		rdi
   
    mov			rdi, rcx;
	mov			rcx, r8;
	mov			rax, rdx;		// v
	mov			rdx, r9;
	movd		xmm0, eax;		// 0 0 0 v
	shufps		xmm0, xmm0, 00h;	// v v v v
	movdqa		xmm4,xmm0;
	pand		xmm4,oword ptr[gc_dwMaskAlpha];
	pand		xmm0,oword ptr[gc_dwMaskRGB];
	movdqa		xmm1,oword ptr[gc_dwMaskAlpha];
	psrld		xmm4,8;
	test		ecx, ecx;
	jnz			loop_memset_dword_line_rgb_mula_a16;
	jmp			loop_memset_dword_line_rgb_mula_a8;
loop_memset_dword_line_rgb_mula_a16:
	movdqu		xmm3,[rdi];
	pand		xmm3,xmm1;
	psrld		xmm3,8;
	pmullw		xmm3,xmm4;
	pand		xmm3,oword ptr[gc_dwMaskAlpha];
	por			xmm3,xmm0;
	movdqu		[rdi], xmm3;

	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_memset_dword_line_rgb_mula_a16;
loop_memset_dword_line_rgb_mula_a8:
	test		edx, 00000008h;
	jz			loop_memset_dword_line_rgb_mula_a4;
	movsd		xmm3,QWORD PTR[rdi];
	pand		xmm3,xmm1;
	psrld		xmm3,8;
	pmullw		xmm3,xmm4;
	pand		xmm3,oword ptr[gc_dwMaskAlpha];
	por			xmm3,xmm0;
	movlps		QWORD PTR[rdi], xmm3;
	add			rdi, 08h;
loop_memset_dword_line_rgb_mula_a4:
	test		edx, 00000004h;
	jz			loop_memset_dword_line_rgb_mula_end;
	movd		xmm3,DWORD PTR[rdi];
	pand		xmm3,xmm1;
	psrld		xmm3,8;
	pmullw		xmm3,xmm4;
	pand		xmm3,oword ptr[gc_dwMaskAlpha];
	por			xmm3,xmm0;
	movd		DWORD PTR[rdi], xmm3;
loop_memset_dword_line_rgb_mula_end:
     
	pop			rdi
    ret
    
    
    
;-----------------------------------------------------------------------------
; void memset_dword_line_rgb_x64(OUT void *des,	// 目标
;						   IN DWORD value,	// 设置值
;						   IN int na16,		// 宽度除以 16 的商
;						   IN int nm16);		// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

memset_dword_line_rgb_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - value
    ;rcx - des 
    
    push		rdi
   
    mov			rdi, rcx;
	mov			rcx, r8;
	mov			rax, rdx;			// v
	mov			rdx, r9;
	movd		xmm0, eax;			// 0 0 0 v
	shufps		xmm0, xmm0, 00h;	// v v v v
	movdqa		xmm1, oword ptr[gc_dwMaskAlpha];
	test		ecx, ecx;
	jnz			loop_memset_dword_line_rgb_a16;
	jmp			loop_memset_dword_line_rgb_a8;
loop_memset_dword_line_rgb_a16:
	movdqu		xmm3,[rdi];
	pand		xmm3,xmm1;
	por			xmm3,xmm0;
	movdqu		[rdi], xmm3;
	
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_memset_dword_line_rgb_a16;
loop_memset_dword_line_rgb_a8:
	test		edx, 00000008h;
	jz			loop_memset_dword_line_rgb_a4;
	movsd		xmm3,QWORD PTR[rdi];
	pand		xmm3,xmm1;
	por			xmm3,xmm0;
	movlps		QWORD PTR[rdi], xmm3;
	add			rdi, 08h;
loop_memset_dword_line_rgb_a4:
	test		edx, 00000004h;
	jz			loop_memset_dword_line_rgb_end;
	movd		xmm3,DWORD PTR[rdi];
	pand		xmm3,xmm1;
	por			xmm3,xmm0;
	movd		DWORD PTR[rdi], xmm3;
loop_memset_dword_line_rgb_end:
     
	pop			rdi
    ret
    
    
;-----------------------------------------------------------------------------
; void memrev_char_line_x64(IN OUT void *head,	// 缓存起始位置
;					  IN OUT void *end,		// 缓存结束位置（end = head + nlen）
;					  IN int na16,			// 一半宽度除以 16 的商
;					  IN int nm16);			// 一半宽度除以 16 的余数
;-----------------------------------------------------------------------------

memrev_char_line_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - end
    ;rcx - head 
    
    push		rdi
    push		rsi    
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
    
    mov			rdi, rdx;
	mov			rsi, rcx;
	mov			rcx, r8;
	
	sub			rdi, 10h;		// 先前移 16 个字节
	test		ecx, ecx;
	jnz			loop_memrev_char_line_a16;
	jmp			loop_memrev_char_line_m8;
loop_memrev_char_line_a16:
	movdqu		xmm0, [rsi];	// Pf Pd Pe Pc  Pb Pa P9 P8  P7 P6 P5 P4  P3 P2 P1 P0
	movdqu		xmm7, [rdi];	// Pn-1 Pn-2 Pn-3 Pn-4 Pn-5 …… Pn-13 Pn-14 Pn-15 Pn-16
	shufps		xmm0, xmm0, 1bh;	// P3 P2 P1 P0  P7 P6 P5 P4  Pb Pa P9 P8  Pf Pd Pe Pc
	shufps		xmm7, xmm7, 1bh;	// PPn-4  PPn-3  PPn-2  PPn-1
	movdqa		xmm1, xmm0;
	movdqa		xmm2, xmm0;
	movdqa		xmm3, xmm0;
	movdqa		xmm4, xmm7;
	movdqa		xmm5, xmm7;
	movdqa		xmm6, xmm7;
	pand		xmm1, oword ptr[gc_dwMaskG];	// 0 0 P1 0  0 0 P5 0  0 0 P9 0  0 0 Pe 0
	pand		xmm5, oword ptr[gc_dwMaskG];
	pslld		xmm0, 18h;		// P0 0 0 0  P4 0 0 0  P8 0 0 0  Pc 0 0 0
	pslld		xmm4, 18h;
	pslld		xmm1, 08h;		// 0 P1 0 0  0 P5 0 0  0 P9 0 0  0 Pe 0 0
	pslld		xmm5, 08h;
	psrld		xmm2, 08h;		// 0 P3 P2 P1  0 P7 P6 P5  0 Pb Pa P9  0 Pf Pd Pe
	psrld		xmm6, 08h;
	psrld		xmm3, 18h;		// 0 0 0 P3  0 0 0 P7  0 0 0 Pb  0 0 0 Pf
	psrld		xmm7, 18h;
	pand		xmm2, oword ptr[gc_dwMaskG];	// 0 0 P2 0  0 0 P6 0  0 0 Pa 0  0 0 Pd 0
	pand		xmm6, oword ptr[gc_dwMaskG];
	por			xmm0, xmm1;		// P0 P1 0 0  P4 P5 0 0  P8 P9 0 0  Pc Pe 0 0
	por			xmm4, xmm5;
	por			xmm2, xmm3;		// 0 0 P2 P3  0 0 P6 P7  0 0 Pa Pb  0 0 Pd Pf
	por			xmm6, xmm7;
	por			xmm0, xmm2;		// P0 P1 P2 P3  P4 P5 P6 P7  P8 P9 Pa Pb  Pc Pe Pd Pf
	por			xmm4, xmm6;
	movdqu		[rdi], xmm0;
	movdqu		[rsi], xmm4;
	sub			rdi, 10h;
	add			rsi, 10h;
	sub			ecx, 01h;
	jnz			loop_memrev_char_line_a16;
loop_memrev_char_line_m8:
	mov			rcx, r9;
	add			rdi, 08h;		// 使之只前移 8 个字节
	test		ecx, 08h;
	jz			loop_memrev_char_line_m4;
	movlps		xmm0, QWORD PTR[rsi];	// @  @  P7 P6 P5 P4  P3 P2 P1 P0
	movhps		xmm0, QWORD PTR[rdi];	// Pn-1 Pn-2 Pn-3 Pn-4  Pn-5 Pn-6 Pn-7 Pn-8  P7 P6 P5 P4  P3 P2 P1 P0
	shufps		xmm0, xmm0, 0b1h;	// Pn-2  Pn-1  P0  P1
	movdqa		xmm1, xmm0;
	movdqa		xmm2, xmm0;
	movdqa		xmm3, xmm0;
	pand		xmm1, oword ptr[gc_dwMaskG];	// 0 0 Pn-7 0  0 0 Pn-3 0  0 0 P1 0  0 0 P5 0
	pslld		xmm0, 18h;		// Pn-8 0 0 0  Pn-4 0 0 0  P0 0 0 0  P4 0 0 0
	pslld		xmm1, 08h;		// 0 Pn-7 0 0  0 Pn-3 0 0  0 P1 0 0  0 P5 0 0
	psrld		xmm2, 08h;		// 0 Pn-5 Pn-6 Pn-7  0 Pn-1 Pn-2 Pn-3  0 P3 P2 P1  0 P7 P6 P5
	psrld		xmm3, 18h;		// 0 0 0 Pn-5  0 0 0 Pn-1  0 0 0 P3  0 0 0 P7
	pand		xmm2, oword ptr[gc_dwMaskG];	// 0 0 Pn-6 0  0 0 Pn-2 0  0 0 P2 0  0 0 P6 0
	por			xmm0, xmm1;		// Pn-8 Pn-7 0 0  Pn-4 Pn-3 0 0  P0 P1 0 0  P4 P5 0 0
	por			xmm2, xmm3;		// 0 0 Pn-6 Pn-5  0 0 Pn-2 Pn-1  0 0 P2 P3  0 0 P6 P7
	por			xmm0, xmm2;		// Pn-8 Pn-7 Pn-6 Pn-5  Pn-4 Pn-3 Pn-2 Pn-1  P0 P1 P2 P3  P4 P5 P6 P7
	movlps		QWORD PTR[rdi], xmm0;
	movhps		QWORD PTR[rsi], xmm0;
	sub			rdi, 08h;
	add			rsi, 08h;
loop_memrev_char_line_m4:
	add			rdi, 04h;		// 使之只前移 4 个字节
	test		ecx, 04h;
	jz			loop_memrev_char_line_m2;
	movss		xmm0, DWORD PTR[rsi];	// 0 0 0  P3 P2 P1 P0
	movss		xmm7, DWORD PTR[rdi];	// 0 0 0  Pn-1 Pn-2 Pn-3 Pn-4
	movlhps		xmm0, xmm7;		// 0  Pn-1 Pn-2 Pn-3 Pn-4  0  P3 P2 P1 P0
	movdqa		xmm1, xmm0;
	movdqa		xmm2, xmm0;
	movdqa		xmm3, xmm0;
	pand		xmm1, oword ptr[gc_dwMaskG];	// 0  0 0 Pn-3 0  0  0 0 P1 0
	pslld		xmm0, 18h;		// 0  Pn-4 0 0 0  0  P0 0 0 0
	pslld		xmm1, 08h;		// 0  0 Pn-3 0 0  0  0 P1 0 0
	psrld		xmm2, 08h;		// 0  0 Pn-1 Pn-2 Pn-3  0  0 P3 P2 P1
	psrld		xmm3, 18h;		// 0  0 0 0 Pn-1  0  0 0 0 P3
	pand		xmm2, oword ptr[gc_dwMaskG];	// 0  0 0 Pn-2 0  0  0 0 P2 0
	por			xmm0, xmm1;		// 0  Pn-4 Pn-3 0 0  0  P0 P1 0 0
	por			xmm2, xmm3;		// 0  0 0 Pn-2 Pn-1  0  0 0 P2 P3
	por			xmm0, xmm2;		// 0  Pn-4 Pn-3 Pn-2 Pn-1  0  P0 P1 P2 P3
	movhlps		xmm7, xmm0;		// 0 0 0  Pn-4 Pn-3 Pn-2 Pn-1
	movss		DWORD PTR[rdi], xmm0;
	movss		DWORD PTR[rsi], xmm7;
	sub			rdi, 04h;
	add			rsi, 04h;
loop_memrev_char_line_m2:
	add			rdi, 02h;		// 使之只前移 2 个字节
	test		ecx, 02h;
	jz			loop_memrev_char_line_m1;
	mov			ax, [rsi];		// P1 P0
	mov			dx, [rdi];		// Pn-1 Pn-2
	mov			ebx, eax;
	shl			eax, 08h;
	shr			ebx, 08h;
	and			eax, 0000ff00h;	// 0 0 P0 0
	and			ebx, 000000ffh;	// 0 0 0 P1
	or			eax, ebx;		// 0 0 P0 P1
	mov			ebx, edx;
	shl			edx, 08h;
	shr			ebx, 08h;
	and			edx, 0000ff00h;	// 0 0 Pn-2 0
	and			ebx, 000000ffh;	// 0 0 0 Pn-1
	or			edx, ebx;		// 0 0 Pn-2 Pn-1
	mov			[rdi], ax;
	mov			[rsi], dx;
	sub			rdi, 02h;
	add			rsi, 02h;
loop_memrev_char_line_m1:
	test		ecx, 01h;
	jz			loop_memrev_char_line_end;
	add			rdi, 01h;
	mov			al, [rsi];		// P0
	mov			dl, [rdi];		// Pn-1
	mov			[rsi], dl;
	mov			[rdi], al;
loop_memrev_char_line_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
    pop			rsi
	pop			rdi
    ret
    
    
;-----------------------------------------------------------------------------
; void memrev_dword_line_x64(IN OUT void *head,	// 缓存起始位置
;								  IN OUT void *end,	// 缓存结束位置（end = head + nlen）
;								  IN int na16,			// 一半像素宽度的字节宽度除以 16 的商
;								  IN int nm16);			// 一半像素宽度的字节宽度除以 16 的余数
;-----------------------------------------------------------------------------

memrev_dword_line_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - end
    ;rcx - head 
    
    push		rdi
    push		rsi    
    sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7

    mov			rdi, rdx;
	mov			rsi, rcx;
	mov			rcx, r8;
	sub			rdi, 10h;		// 先前移 16 个字节
	test		ecx, ecx;
	jnz			loop_memrev_dword_line_a16;
	jmp			loop_memrev_dword_line_m8;
loop_memrev_dword_line_a16:
	movdqu		xmm0, [rsi];	// P3 P2 P1 P0
	movdqu		xmm7, [rdi];	// Pn-1 Pn-2 Pn-3 Pn-4
	shufps		xmm0, xmm0, 1bh;	// P0 P1 P2 P3
	shufps		xmm7, xmm7, 1bh;	// Pn-4 Pn-3 Pn-2 Pn-1
	movdqu		[rdi], xmm0;
	movdqu		[rsi], xmm7;
	sub			rdi, 10h;
	add			rsi, 10h;
	sub			ecx, 01h;
	jnz			loop_memrev_dword_line_a16;
loop_memrev_dword_line_m8:
	mov			rcx, r9;
	add			rdi, 08h;		// 使之只前移 8 个字节
	test		ecx, 08h;
	jz			loop_memrev_dword_line_m4;
	movlps		xmm0, QWORD PTR[rsi];	// @  @  P1  P0
	movhps		xmm0, QWORD PTR[rdi];	// Pn-1  Pn-2  P1  P0
	shufps		xmm0, xmm0, 0b1h;	// Pn-2  Pn-1  P0  P1
	movlps		QWORD PTR[rdi], xmm0;
	movhps		QWORD PTR[rsi], xmm0;
	sub			rdi, 08h;
	add			rsi, 08h;
loop_memrev_dword_line_m4:
	test		ecx, 04;
	jz			loop_memrev_dword_line_end;
	add			rdi, 04;		// 使之只前移 4 个字节
	movss		xmm0, DWORD PTR[rsi];	// 0 0 0 P0
	movss		xmm7, DWORD PTR[rdi];	// 0 0 0 Pn-1
	movss		DWORD PTR[rsi], xmm7;
	movss		DWORD PTR[rdi], xmm0;
	
	movd		xmm0, eax
loop_memrev_dword_line_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
    pop			rsi
	pop			rdi
    ret
    
    
;-----------------------------------------------------------------------------
; void bgra32_line_mix_notsrc_x64(IN OUT void *des,		// 目标
;							IN void *src,			// 源
;							IN int na16,			// 宽度除以 16 的商
;							IN int nm16,			// 宽度除以 16 的余数
;							IN void *ka_table)		// 256 * 256 * sizeof(DWORD) 的 KA 表
;-----------------------------------------------------------------------------

bgra32_line_mix_notsrc_x64:
	;[rsp+28h] - ka_table
	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    
    push		rdi
    push		rsi    
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
    
    mov			rdi, rcx;
	mov			rsi, rdx;
    mov			rcx, r8;
	mov			rdx, [rsp+28h+30h];
	
	test		ecx, ecx;
	jnz			loop_bgra32_line_mix_notsrc_a16;
	jmp			loop_bgra32_line_mix_notsrc_a8;
	
loop_bgra32_line_mix_notsrc_a16:
	;// 根据 ((255 - sa) * 256 + da) * 4 查增益表，由于表为 DWORD 型，转成字节时需要乘以长度
	movdqu		xmm6, [rsi];	// src(A3R3G3B3 A2R2G2B2 A1R1G1B1 A0R0G0B0)
	movdqu		xmm7, [rdi];	// des(A3R3G3B3 A2R2G2B2 A1R1G1B1 A0R0G0B0)
	movdqa		xmm4, xmm6;
	movdqa		xmm5, xmm7;
	movdqa		xmm2, oword ptr[gc_dwMaskDLB];	// 255
	psrld		xmm4, 18h;		// sa(src alpha A3 A2 A1 A0)
	psrld		xmm5, 18h;		// da(des alpha A3 A2 A1 A0)
	movd		xmm1, rdx;		// 表头 0 0 0 table
	pshufd		xmm1, xmm1, 00h;	// table table table table
	psubd		xmm2, xmm4;		// 255 - sa
	pmullw		xmm4, xmm5;		// sa * da
	movdqa		xmm3, xmm2;		// 255 - sa
	pslld		xmm2, 0ah;		// (255 - sa) * 256 * 4
	pslld		xmm5, 02h;		// da * 4
	paddd		xmm2, xmm5;		// ((255 - sa) * 256 + da) * 4 存放 Offset3 Offset2 Offset1 Offset0
	paddd		xmm2, xmm1;		// 增益地址 ptr3 ptr2 ptr1 ptr0
	paddd		xmm4, oword ptr[gc_dwMaskDLB];	// sa * da + 255
	psrld		xmm4, 08h;		// (sa * da + 255) >> 8
	paddd		xmm3, xmm4;		// 计算 MA = 255 - SA + (SA * DA + 255) >> 8
	pslld		xmm3, 18h;		// 存放结果 A000 A000 A000 A000
	;// 挨个像素取出增益值
	movd		eax, xmm2;		// 第 0 个像素的增益 ptr0
	pshufd		xmm2, xmm2, 39h;	// ptr0 ptr3 ptr2 ptr1
	movd		xmm4, DWORD PTR[rax];	// 存放 0 0 0 K0
	movd		eax, xmm2;		// 第 1 个像素的增益 ptr1
	pshufd		xmm2, xmm2, 39h;	// ptr1 ptr0 ptr3 ptr2
	movd		xmm5, DWORD PTR[rax];	// 存放 0 0 0 K1
	movd		eax, xmm2;		// 第 2 个像素的增益 ptr2
	pshufd		xmm2, xmm2, 39h;	// ptr2 ptr1 ptr0 ptr3
	movd		xmm1, DWORD PTR[rax];	// 存放 0 0 0 K2
	movd		eax, xmm2;		// 第 3 个像素的增益 ptr3
	shufps		xmm4, xmm5, 00h;	// K1 K1 K0 K0
	movd		xmm2, DWORD PTR[rax];	// 存放 0 0 0 K3
	shufps		xmm1, xmm2, 00h;	// K3 K3 K2 K2
	packssdw	xmm4, xmm1;		// K3K3 K2K2 K1K1 K0K0
	;// 先计算 R B 再单独计算 G
	movdqa		xmm2, xmm6;
	movdqa		xmm5, xmm7;
	psrld		xmm6, 08h;
	psrld		xmm7, 08h;
	pand		xmm2, oword ptr[gc_dwMaskWLB];	// src RB(0R0B 0R0B 0R0B 0R0B)
	pand		xmm5, oword ptr[gc_dwMaskWLB];	// des RB(0R0B 0R0B 0R0B 0R0B)
	pand		xmm6, oword ptr[gc_dwMaskDLB];	// src G(000G 000G 000G 000G)
	pand		xmm7, oword ptr[gc_dwMaskDLB];	// des G(000G 000G 000G 000G)
	movdqa		xmm0, xmm2;		// srb
	movdqa		xmm1, xmm6;		// sg
	pcmpgtw		xmm2, xmm5;		// (srb > drb) ? -1 : 0
	pcmpgtw		xmm6, xmm7;		// (sg > dg) ? -1 : 0
	psubw		xmm0, xmm5;		// srb - drb
	psubw		xmm1, xmm7;		// sg - dg
	psubw		xmm0, xmm2;		// srb - drb + compRB
	psubw		xmm1, xmm6;		// sg - dg + compG
	paddw		xmm0, xmm0;		// (srb - drb + compRB) * 2 浮点乘法到整型乘法的转变
	paddw		xmm1, xmm1;		// (sg - dg + compG) * 2 浮点乘法到整型乘法的转变
	pmulhw		xmm0, xmm4;		// (srb - drb) * KA
	pmulhw		xmm1, xmm4;		// (sg - dg) * KA
	paddw		xmm0, xmm5;		// (srb - drb) * KA + drb
	paddw		xmm1, xmm7;		// (sg - dg) * KA + dg
	pslld		xmm1, 08h;
	por			xmm3, xmm0;		// mix |= (mrb) 存放 AR0B AR0B AR0B AR0B
	por			xmm3, xmm1;		// mix |= (mg << 8) 存放 ARGB ARGB ARGB ARGB
	;// 写入 Mix 结果并开始下一个连续的四个像素计算
	movdqu		[rdi], xmm3;
	add			rsi, 10h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_bgra32_line_mix_notsrc_a16;
loop_bgra32_line_mix_notsrc_a8:
	mov			rcx, r9;
	pxor		xmm0, xmm0;		// 0 0 0 0
	test		ecx, 00000008h;
	jz			loop_bgra32_line_mix_notsrc_a4;
	;// 根据 ((255 - sa) * 256 + da) * 4 查增益表，由于表为 DWORD 型，转成字节时需要乘以长度
	movlps		xmm6, QWORD PTR[rsi];	// src(@ @ A1R1G1B1 A0R0G0B0)
	movlps		xmm7, QWORD PTR[rdi];	// des(@ @ A1R1G1B1 A0R0G0B0)
	movdqa		xmm4, xmm6;
	movdqa		xmm5, xmm7;
	movdqa		xmm2, oword ptr[gc_dwMaskDLB];	// 255
	psrld		xmm4, 18h;		// sa(src alpha @ @ A1 A0)
	psrld		xmm5, 18h;		// da(des alpha @ @ A1 A0)
	movd		xmm1, edx;		// 表头 0 0 0 table
	pshufd		xmm1, xmm1, 00h;	// table table table table
	psubd		xmm2, xmm4;		// 255 - sa
	pmullw		xmm4, xmm5;		// sa * da
	movdqa		xmm3, xmm2;		// 255 - sa
	pslld		xmm2, 0ah;		// (255 - sa) * 256 * 4
	pslld		xmm5, 02h;		// da * 4
	paddd		xmm2, xmm5;		// ((255 - sa) * 256 + da) * 4 存放 @ @ Offset1 Offset0
	paddd		xmm2, xmm1;		// 增益地址 @ @ ptr1 ptr0
	paddd		xmm4, oword ptr[gc_dwMaskDLB];	// sa * da + 255
	psrld		xmm4, 08h;		// (sa * da + 255) >> 8
	paddd		xmm3, xmm4;		// 计算 MA = 255 - SA + (SA * DA + 255) >> 8
	pslld		xmm3, 18h;		// 存放结果 0000 0000 A000 A000
	;// 挨个像素取出增益值
	movd		eax, xmm2;		// 第 0 个像素的增益 ptr0
	pshufd		xmm2, xmm2, 39h;	// ptr0 0 0 ptr1
	movd		xmm4, DWORD PTR[rax];	// 存放 0 0 0 K0
	movd		eax, xmm2;		// 第 1 个像素的增益 ptr1
	movd		xmm5, DWORD PTR[rax];	// 存放 0 0 0 K1
	shufps		xmm4, xmm4, 00h;	// K0 K0 K0 K0
	shufps		xmm5, xmm5, 00h;	// K1 K1 K1 K1
	packssdw	xmm4, xmm5;		// K1K1 K1K1 K0K0 K0K0
	;// 同时计算 R G B
	pand		xmm6, oword ptr[gc_dwMaskRGB];// src RGB(@ @ 0RGB 0RGB)
	pand		xmm7, oword ptr[gc_dwMaskRGB];// des RGB(@ @ 0RGB 0RGB)
	punpcklbw	xmm6, xmm0;		// 000R 0G0B 000R 0G0B
	punpcklbw	xmm7, xmm0;		// 000R 0G0B 000R 0G0B
	movdqa		xmm2, xmm6;		// src RGB
	pcmpgtw		xmm2, xmm7;		// (src > des) ? -1 : 0
	psubw		xmm6, xmm7;		// src - des
	psubw		xmm6, xmm2;		// src - des + compRGB
	paddw		xmm6, xmm6;		// (src - des + compRGB) * 2 浮点乘法到整型乘法的转变
	pmulhw		xmm6, xmm4;		// (src - des) * KA
	paddw		xmm6, xmm7;		// (src - des) * KA + des
	packuswb	xmm6, xmm0;		// 0000 0000 0RGB 0RGB
	por			xmm3, xmm6;		// mix |= (RGB) 存放 0000 0000 ARGB ARGB
	;// 写入 Mix 结果
	movlps		QWORD PTR[rdi], xmm3;
	add			rsi, 08h;
	add			rdi, 08h;
loop_bgra32_line_mix_notsrc_a4:
	test		ecx, 00000004h;
	jz			loop_bgra32_line_mix_notsrc_end;
	;// 根据 ((255 - sa) * 256 + da) * 4 查增益表，由于表为 DWORD 型，转成字节时需要乘以长度
	movd		xmm1, edx;		// 表头 0 0 0 table
	movd		xmm6, DWORD PTR[rsi];	// src(0000 0000 0000 ARGB)
	movd		xmm7, DWORD PTR[rdi];	// des(0000 0000 0000 ARGB)
	movdqa		xmm4, xmm6;
	movdqa		xmm5, xmm7;
	movdqa		xmm2, oword ptr[gc_dwMaskDLB];	// 255
	psrld		xmm4, 18h;		// sa(src alpha 0 0 0 A0)
	psrld		xmm5, 18h;		// da(des alpha 0 0 0 A0)
	psubd		xmm2, xmm4;		// 255 - sa
	pmullw		xmm4, xmm5;		// sa * da
	movdqa		xmm3, xmm2;		// 255 - sa
	pslld		xmm2, 0ah;		// (255 - sa) * 256 * 4
	pslld		xmm5, 02h;		// da * 4
	paddd		xmm2, xmm5;		// ((255 - sa) * 256 + da) * 4 存放 0 0 0 Offset0
	paddd		xmm2, xmm1;		// 增益地址 0 0 0 ptr0
	paddd		xmm4, oword ptr[gc_dwMaskDLB];	// sa * da + 255
	psrld		xmm4, 08h;		// (sa * da + 255) >> 8
	paddd		xmm3, xmm4;		// 计算 MA = 255 - SA + (SA * DA + 255) >> 8
	pslld		xmm3, 18h;		// 存放结果 0000 0000 0000 A000
	;// 挨个像素取出增益值
	movd		eax, xmm2;		// 第 0 个像素的增益 ptr0
	movd		xmm4, DWORD PTR[rax];	// 存放 0 0 0 K0
	shufps		xmm4, xmm4, 00h;	// K0 K0 K0 K0
	packssdw	xmm4, xmm0;		// 0 0 K0K0 K0K0
	pand		xmm6, oword ptr[gc_dwMaskRGB];// src RGB(0000 0000 0000 0RGB)
	pand		xmm7, oword ptr[gc_dwMaskRGB];// des RGB(0000 0000 0000 0RGB)
	punpcklbw	xmm6, xmm0;		// 0000 0000 000R 0G0B
	punpcklbw	xmm7, xmm0;		// 0000 0000 000R 0G0B
	movdqa		xmm2, xmm6;		// src RGB
	pcmpgtw		xmm2, xmm7;		// (src > des) ? -1 : 0
	psubw		xmm6, xmm7;		// src - des
	psubw		xmm6, xmm2;		// src - des + compRGB
	paddw		xmm6, xmm6;		// (src - des + compRGB) * 2 浮点乘法到整型乘法的转变
	pmulhw		xmm6, xmm4;		// (src - des) * KA
	paddw		xmm6, xmm7;		// (src - des) * KA + des
	packuswb	xmm6, xmm0;		// 0000 0000 0000 0RGB
	por			xmm3, xmm6;		// mix |= (RGB) 存放 0000 0000 0000 ARGB
	;// 写入 Mix 结果
	movss		DWORD PTR[rdi], xmm3;
loop_bgra32_line_mix_notsrc_end:
    movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
    pop			rsi
	pop			rdi
    ret
    
    
;-----------------------------------------------------------------------------
; void bgra32_line_alpha_blend_x64(IN OUT void *des,		// 目标
;										IN void *src,			// 源
;										IN int na16,			// 宽度除以 16 的商
;										IN int nm16);			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

bgra32_line_alpha_blend_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    
    push		rdi
    push		rsi    
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
    
    mov			rdi, rcx;
	mov			rsi, rdx;
    mov			rcx, r8;
	movdqa		xmm0, oword ptr[gc_dwMaskWLB];
	test		ecx, ecx;
	jnz			loop_bgra32_line_alpha_blend_a16;
	jmp			loop_bgra32_line_alpha_blend_a8;
loop_bgra32_line_alpha_blend_a16:
	movdqu		xmm6, [rsi];	// src(A3R3G3B3 A2R2G2B2 A1R1G1B1 A0R0G0B0)
	movdqu		xmm7, [rdi];	// des(A3R3G3B3 A2R2G2B2 A1R1G1B1 A0R0G0B0)
	movdqa		xmm2, xmm6;
	movdqa		xmm4, xmm6;
	movdqa		xmm5, xmm7;
	psrld		xmm2, 18h;		// sa(000A3 000A2 000A1 000A0)
	psrlw		xmm6, 08h;		// src AG(0A0G 0A0G 0A0G 0A0G)
	psrlw		xmm7, 08h;		// des AG(0A0G 0A0G 0A0G 0A0G)
	movdqa		xmm3, xmm2;
	pslld		xmm2, 10h;		// sa(0A00 0A00 0A00 0A00)
	pand		xmm4, xmm0;		// src RB(0R0B 0R0B 0R0B 0R0B)
	pand		xmm5, xmm0;		// des RB(0R0B 0R0B 0R0B 0R0B)
	por			xmm2, xmm3;		// sa(A3A3 A2A2 A1A1 A0A0)
	movdqa		xmm3, xmm6;		// sag
	movdqa		xmm1, xmm4;		// srb
	pcmpgtw		xmm6, xmm7;		// (sag > dag) ? -1 : 0
	pcmpgtw		xmm4, xmm5;		// (srb > drb) ? -1 : 0
	psubw		xmm3, xmm7;		// sag - dag
	psubw		xmm1, xmm5;		// srb - drb
	psubw		xmm3, xmm6;		// sag - dag + compAG
	psubw		xmm1, xmm4;		// srb - drb + compRB
	pmullw		xmm3, xmm2;		// (sag - dag) * sa
	pmullw		xmm1, xmm2;		// (srb - drb) * sa
	psraw		xmm3, 08h;
	psraw		xmm1, 08h;
	paddw		xmm3, xmm7;		// (sag - dag) * sa + dag
	paddw		xmm1, xmm5;		// (srb - drb) * sa + drb
	pslld		xmm3, 08h;
	por			xmm3, xmm1;
	movdqu		[rdi], xmm3;
	add			rsi, 10h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_bgra32_line_alpha_blend_a16;
loop_bgra32_line_alpha_blend_a8:
	mov			rcx, r9;
	pxor		xmm0, xmm0;		// 0 0 0 0
	test		ecx, 00000008h;
	jz			loop_bgra32_line_alpha_blend_a4;
	movlps		xmm6, QWORD PTR[rsi];	// src(@  @  A1R1G1B1 A0R0G0B0)
	movlps		xmm7, QWORD PTR[rdi];	// des(@  @  A1R1G1B1 A0R0G0B0)
	movdqa		xmm2, xmm6;
	psrld		xmm2, 18h;		// sa(@  @  000A1 000A0)
	punpcklbw	xmm6, xmm0;
	punpcklbw	xmm7, xmm0;
	pshuflw		xmm2, xmm2, 0a0h;	// sa(@  @  A1A1  A0A0)
	movdqa		xmm3, xmm6;		// src
	pcmpgtw		xmm6, xmm7;		// (src > des) ? -1 : 0
	shufps		xmm2, xmm2, 50h;	// sa(A1A1  A1A1  A0A0  A0A0)
	psubw		xmm3, xmm7;		// src - des
	psubw		xmm3, xmm6;		// src - des + comp
	pmullw		xmm3, xmm2;		// (src - des) * sa
	psraw		xmm3, 08h;
	paddw		xmm3, xmm7;		// (src - des) * sa + des
	packuswb	xmm3, xmm0;
	movlps		QWORD PTR[rdi], xmm3;
	add			rsi, 08h;
	add			rdi, 08h;
loop_bgra32_line_alpha_blend_a4:
	test		ecx, 00000004h;
	jz			loop_bgra32_line_alpha_blend_end;
	movss		xmm6, DWORD PTR[rsi];	// src(0  0  0  A0R0G0B0)
	movss		xmm7, DWORD PTR[rdi];	// des(0  0  0  A0R0G0B0)
	movdqa		xmm2, xmm6;
	psrld		xmm2, 18h;		// sa(0  0  0  000A0)
	punpcklbw	xmm6, xmm0;
	punpcklbw	xmm7, xmm0;
	pshuflw		xmm2, xmm2,00h;	// sa(0  0  A0A0  A0A0)
	movdqa		xmm3, xmm6;		// src
	pcmpgtw		xmm6, xmm7;		// (src > des) ? -1 : 0
	psubw		xmm3, xmm7;		// src - des
	psubw		xmm3, xmm6;		// src - des + comp
	pmullw		xmm3, xmm2;		// (src - des) * sa
	psraw		xmm3, 08h;
	paddw		xmm3, xmm7;		// (src - des) * sa + des
	packuswb	xmm3, xmm0;
	movss		DWORD PTR[rdi], xmm3;
loop_bgra32_line_alpha_blend_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add		rsp,20h
    pop			rsi
	pop			rdi
    ret
    
    
;-----------------------------------------------------------------------------
; void bgra32_line_alpha_blend_not_x64(IN OUT void *des,		// 目标
;											IN void *src,			// 源
;											IN int na16,			// 宽度除以 16 的商
;											IN int nm16);			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

bgra32_line_alpha_blend_not_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    
    push		rdi
    push		rsi    
    sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
    
    mov			rdi, rcx;
	mov			rsi, rdx;
    mov			rcx, r8;
	movdqa		xmm0, oword ptr[gc_dwMaskWLB];
	test		ecx, ecx;
	jnz			loop_bgra32_line_alpha_blend_not_a16;
	jmp			loop_bgra32_line_alpha_blend_not_a8;
loop_bgra32_line_alpha_blend_not_a16:
	movdqu		xmm6, [rsi];	// src(A3R3G3B3 A2R2G2B2 A1R1G1B1 A0R0G0B0)
	movdqu		xmm7, [rdi];	// des(A3R3G3B3 A2R2G2B2 A1R1G1B1 A0R0G0B0)
	movdqa		xmm2, xmm6;
	movdqa		xmm4, xmm6;
	movdqa		xmm5, xmm7;
	psrld		xmm2, 18h;		// sa(000A3 000A2 000A1 000A0)
	psrlw		xmm6, 08h;		// src AG(0A0G 0A0G 0A0G 0A0G)
	psrlw		xmm7, 08h;		// des AG(0A0G 0A0G 0A0G 0A0G)
	movdqa		xmm3, xmm2;
	pslld		xmm2, 10h;		// sa(0A00 0A00 0A00 0A00)
	pand		xmm4, xmm0;		// src RB(0R0B 0R0B 0R0B 0R0B)
	pand		xmm5, xmm0;		// des RB(0R0B 0R0B 0R0B 0R0B)
	por			xmm2, xmm3;		// sa(A3A3 A2A2 A1A1 A0A0)
	movdqa		xmm3, xmm7;		// dag
	movdqa		xmm1, xmm5;		// drb
	pcmpgtw		xmm7, xmm6;		// (dag > sag) ? -1 : 0
	pcmpgtw		xmm5, xmm4;		// (drb > srb) ? -1 : 0
	psubw		xmm3, xmm6;		// dag - sag
	psubw		xmm1, xmm4;		// drb - srb
	psubw		xmm3, xmm7;		// dag - sag + compAG
	psubw		xmm1, xmm5;		// drb - srb + compRB
	pmullw		xmm3, xmm2;		// (dag - sag) * sa
	pmullw		xmm1, xmm2;		// (drb - srb) * sa
	psraw		xmm3, 08h;
	psraw		xmm1, 08h;
	paddw		xmm3, xmm6;		// (dag - sag) * sa + sag
	paddw		xmm1, xmm4;		// (drb - srb) * sa + srb
	pslld		xmm3, 08h;
	por			xmm3, xmm1;
	movdqu		[rdi], xmm3;
	add			rsi, 10h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_bgra32_line_alpha_blend_not_a16;
loop_bgra32_line_alpha_blend_not_a8:
	mov			rcx, r9;
	pxor		xmm0, xmm0;		// 0 0 0 0
	test		ecx, 00000008h;
	jz			loop_bgra32_line_alpha_blend_not_a4;
	movlps		xmm6, QWORD PTR[rsi];	// src(@  @  A1R1G1B1 A0R0G0B0)
	movlps		xmm7, QWORD PTR[rdi];	// des(@  @  A1R1G1B1 A0R0G0B0)
	movdqa		xmm2, xmm6;
	psrld		xmm2, 18h;		// sa(@  @  000A1 000A0)
	punpcklbw	xmm6, xmm0;
	punpcklbw	xmm7, xmm0;
	pshuflw		xmm2, xmm2, 0a0h;	// sa(@  @  A1A1  A0A0)
	movdqa		xmm3, xmm7;		// des
	pcmpgtw		xmm7, xmm6;		// (des > src) ? -1 : 0
	shufps		xmm2, xmm2, 50h;	// sa(A1A1  A1A1  A0A0  A0A0)
	psubw		xmm3, xmm6;		// des - src
	psubw		xmm3, xmm7;		// des - src + comp
	pmullw		xmm3, xmm2;		// (des - src) * sa
	psraw		xmm3, 08h;
	paddw		xmm3, xmm6;		// (des - src) * sa + src
	packuswb	xmm3, xmm0;
	movlps		QWORD PTR[rdi], xmm3;
	add			rsi, 08h;
	add			rdi, 08h;
loop_bgra32_line_alpha_blend_not_a4:
	test		ecx, 00000004h;
	jz			loop_bgra32_line_alpha_blend_not_end;
	movss		xmm6, DWORD PTR[rsi];	// src(0  0  0  A0R0G0B0)
	movss		xmm7, DWORD PTR[rdi];	// des(0  0  0  A0R0G0B0)
	movdqa		xmm2, xmm6;
	psrld		xmm2, 18h;		// sa(0  0  0  000A0)
	punpcklbw	xmm6, xmm0;
	punpcklbw	xmm7, xmm0;
	pshuflw		xmm2, xmm2, 00h;	// sa(0  0  A0A0  A0A0)
	movdqa		xmm3, xmm7;		// des
	pcmpgtw		xmm7, xmm6;		// (des > src) ? -1 : 0
	psubw		xmm3, xmm6;		// des - src
	psubw		xmm3, xmm7;		// des - src + comp
	pmullw		xmm3, xmm2;		// (des - src) * sa
	psraw		xmm3, 08h;
	paddw		xmm3, xmm6;		// (des - src) * sa + src
	packuswb	xmm3, xmm0;
	movss		DWORD PTR[rdi], xmm3;
loop_bgra32_line_alpha_blend_not_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
    pop			rsi
	pop			rdi
    ret
    
    
;-----------------------------------------------------------------------------
; void bgra32_line_linear_dodge_x64(IN OUT void *des,		// 目标
;							  IN void *src,			// 源
;							  IN int na16,			// 宽度除以 16 的商
;							  IN int nm16,			// 宽度除以 16 的余数
;							  IN void *ka_table)	// 256 * 256 * sizeof(DWORD) 的 KA 表
;-----------------------------------------------------------------------------

bgra32_line_linear_dodge_x64:
	;[rsp+28h] - ka_table
	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    
    push		rdi
    push		rsi    
    push		rbx
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
    
    mov			rdi, rcx;
	mov			rsi, rdx;
    mov			rcx, r8;
	mov			rdx, [rsp+28h+38h];
	
	test		ecx, ecx;
	jnz			loop_bgra32_line_linear_dodge_a16;
	jmp			loop_bgra32_line_linear_dodge_a8;
loop_bgra32_line_linear_dodge_a16:
	;// 根据 Offset_S 与 Offset_D 查增益表，由于表为 DWORD 型，转成字节时需要乘以长度
	movdqu		xmm6, [rsi];	// src(A3R3G3B3 A2R2G2B2 A1R1G1B1 A0R0G0B0)
	movdqu		xmm7, [rdi];	// des(A3R3G3B3 A2R2G2B2 A1R1G1B1 A0R0G0B0)
	movdqa		xmm4, xmm6;
	movdqa		xmm5, xmm7;
	psrld		xmm4, 18h;		// sa(src alpha A3 A2 A1 A0)
	psrld		xmm5, 18h;		// da(des alpha A3 A2 A1 A0)
	movd		xmm1, rdx;		// 表头 0 0 0 table
	movdqa		xmm0, xmm4;
	movdqa		xmm2, xmm4;
	movdqa		xmm3, xmm5;
	pmullw		xmm4, xmm5;		// sa * da
	pslld		xmm2, 08h;		// sa * 256
	pslld		xmm3, 08h;		// da * 256
	shufps		xmm1, xmm1, 00h;	// table table table table
	paddd		xmm4, oword ptr[gc_dwMaskDLB];	// sa * da + 255
	paddd		xmm2, xmm5;		// sa * 256 + da
	paddd		xmm3, xmm0;		// da * 256 + sa
	paddd		xmm5, xmm0;		// sa + da
	psrld		xmm4, 08h;		// (sa * da + 255) >> 8
	pslld		xmm2, 02h;		// (sa * 256 + da) * 4
	pslld		xmm3, 02h;		// (da * 256 + sa) * 4
	psubd		xmm5, xmm4;		// 计算 MA = SA + DA - (SA * DA + 255) >> 8
	paddd		xmm2, xmm1;		// Src 增益地址 ptr3 ptr2 ptr1 ptr0
	paddd		xmm3, xmm1;		// Des 增益地址 ptr3 ptr2 ptr1 ptr0
	pslld		xmm5, 18h;		// 结果 A000 A000 A000 A000
	;// 挨个像素取出增益值
	movdqa		xmm1, xmm2;		// SKA
	shufps		xmm2, xmm2, 39h;	// ptr0 ptr3 ptr2 ptr1
	movd		eax, xmm1;		// 第 0 个像素的增益 ptr0
	movd		ebx, xmm2;		// 第 1 个像素的增益 ptr1
	movd		xmm0, DWORD PTR[rax];	// 存放 0 0 0 K0
	movd		xmm4, DWORD PTR[rbx];	// 存放 0 0 0 K1
	movhlps		xmm1, xmm1;		// ptr3 ptr2 ptr3 ptr2
	movhlps		xmm2, xmm2;		// ptr0 ptr3 ptr0 ptr3
	movd		eax, xmm1;		// 第 2 个像素的增益 ptr2
	movd		ebx, xmm2;		// 第 3 个像素的增益 ptr3
	movd		xmm1, DWORD PTR[rax];	// 存放 0 0 0 K2
	movd		xmm2, DWORD PTR[rbx];	// 存放 0 0 0 K3
	shufps		xmm0, xmm4, 00h;	// K1 K1 K0 K0
	shufps		xmm1, xmm2, 00h;	// K3 K3 K2 K2
	movdqa		xmm4, xmm3;		// DKA
	shufps		xmm3, xmm3, 39h;	// ptr0 ptr3 ptr2 ptr1
	packssdw	xmm0, xmm1;		// K3K3 K2K2 K1K1 K0K0
	movd		eax, xmm4;		// 第 0 个像素的增益 ptr0
	movd		ebx, xmm3;		// 第 1 个像素的增益 ptr1
	movd		xmm1, DWORD PTR[rax];	// 存放 0 0 0 K0
	movd		xmm2, DWORD PTR[rbx];	// 存放 0 0 0 K1
	movhlps		xmm4, xmm4;		// ptr3 ptr2 ptr3 ptr2
	movhlps		xmm3, xmm3;		// ptr0 ptr3 ptr0 ptr3
	movd		eax, xmm4;		// 第 2 个像素的增益 ptr2
	movd		ebx, xmm3;		// 第 3 个像素的增益 ptr3
	movd		xmm4, DWORD PTR[rax];	// 存放 0 0 0 K2
	movd		xmm3, DWORD PTR[rbx];	// 存放 0 0 0 K3
	shufps		xmm1, xmm2, 00h;	// K1 K1 K0 K0
	shufps		xmm4, xmm3, 00h;	// K3 K3 K2 K2
	packssdw	xmm1, xmm4;		// K3K3 K2K2 K1K1 K0K0
	;// 先计算 R B 再单独计算 G
	movdqa		xmm2, xmm6;
	movdqa		xmm3, xmm7;
	movdqa		xmm4, oword ptr[gc_nRoundComp_1];
	psrlw		xmm6, 08h;		// src AG(0A0G 0A0G 0A0G 0A0G)
	psrlw		xmm7, 08h;		// des AG(0A0G 0A0G 0A0G 0A0G)
	pand		xmm2, oword ptr[gc_dwMaskWLB];	// src RB(0R0B 0R0B 0R0B 0R0B)
	pand		xmm3, oword ptr[gc_dwMaskWLB];	// des RB(0R0B 0R0B 0R0B 0R0B)
	paddw		xmm6, xmm4;		// Comp
	paddw		xmm7, xmm4;
	paddw		xmm2, xmm4;
	paddw		xmm3, xmm4;
	paddw		xmm6, xmm6;		// SC * 2
	paddw		xmm7, xmm7;
	paddw		xmm2, xmm2;
	paddw		xmm3, xmm3;
	pmulhw		xmm6, xmm0;		// SC * SKA
	pmulhw		xmm7, xmm1;		// DC * DKA
	pmulhw		xmm2, xmm0;
	pmulhw		xmm3, xmm1;
	paddusb		xmm6, xmm7;		// SC * SKA + DC * DKA
	paddusb		xmm2, xmm3;
	pslld		xmm6, 08h;
	por			xmm5, xmm2;		// mix |= (mrb) 存放 AR0B AR0B AR0B AR0B
	pand		xmm6, oword ptr[gc_dwMaskG];
	por			xmm5, xmm6;		// mix |= (mg << 8) 存放 ARGB ARGB ARGB ARGB
	;// 写入 Mix 结果并开始下一个连续的四个像素计算
	movdqu		[rdi], xmm5;
	add			rsi, 10h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_bgra32_line_linear_dodge_a16;
loop_bgra32_line_linear_dodge_a8:
	mov			rcx, r9;
	test		ecx, 00000008h;
	jz			loop_bgra32_line_linear_dodge_a4;
	;// 根据 Offset_S 与 Offset_D 查增益表，由于表为 DWORD 型，转成字节时需要乘以长度
	movdqu		xmm6, [rsi];	// src(@ @ A1R1G1B1 A0R0G0B0)
	movdqu		xmm7, [rdi];	// des(@ @ A1R1G1B1 A0R0G0B0)
	movdqa		xmm4, xmm6;
	movdqa		xmm5, xmm7;
	psrld		xmm4, 18h;		// sa(src alpha @ @ A1 A0)
	psrld		xmm5, 18h;		// da(des alpha @ @ A1 A0)
	movd		xmm1, edx;		// 表头 0 0 0 table
	movdqa		xmm0, xmm4;
	movdqa		xmm2, xmm4;
	movdqa		xmm3, xmm5;
	pmullw		xmm4, xmm5;		// sa * da
	pslld		xmm2, 08h;		// sa * 256
	pslld		xmm3, 08h;		// da * 256
	shufps		xmm1, xmm1, 00h;	// table table table table
	paddd		xmm4, oword ptr[gc_dwMaskDLB];	// sa * da + 255
	paddd		xmm2, xmm5;		// sa * 256 + da
	paddd		xmm3, xmm0;		// da * 256 + sa
	paddd		xmm5, xmm0;		// sa + da
	psrld		xmm4, 08h;		// (sa * da + 255) >> 8
	pslld		xmm2, 02h;		// (sa * 256 + da) * 4
	pslld		xmm3, 02h;		// (da * 256 + sa) * 4
	psubd		xmm5, xmm4;		// 计算 MA = SA + DA - (SA * DA + 255) >> 8
	paddd		xmm2, xmm1;		// Src 增益地址 @ @ ptr1 ptr0
	paddd		xmm3, xmm1;		// Des 增益地址 @ @ ptr1 ptr0
	pslld		xmm5, 18h;		// 结果 A000 A000 A000 A000
	;// 挨个像素取出增益值
	movd		eax, xmm2;		// 第 0 个像素的增益 ptr0
	movd		ebx, xmm3;
	shufps		xmm2, xmm2, 39h;	// ptr0 0 0 ptr1
	shufps		xmm3, xmm3, 39h;	// ptr0 0 0 ptr1
	movd		xmm0, DWORD PTR[rax];	// 存放 0 0 0 K0
	movd		xmm1, DWORD PTR[rbx];	// 存放 0 0 0 K0
	movd		eax, xmm2;		// 第 1 个像素的增益 ptr1
	movd		ebx, xmm3;
	movd		xmm2, DWORD PTR[rax];	// 存放 0 0 0 K1
	movd		xmm3, DWORD PTR[rbx];	// 存放 0 0 0 K1
	shufps		xmm0, xmm0, 00h;	// K0 K0 K0 K0
	shufps		xmm1, xmm1, 00h;	// K0 K0 K0 K0
	shufps		xmm2, xmm2, 00h;	// K1 K1 K1 K1
	shufps		xmm3, xmm3, 00h;	// K1 K1 K1 K1
	packssdw	xmm0, xmm2;		// K1K1 K1K1 K0K0 K0K0
	packssdw	xmm1, xmm3;		// K1K1 K1K1 K0K0 K0K0
	;// 同时计算 R G B
	pxor		xmm4, xmm4;		// 0 0 0 0
	punpcklbw	xmm6, xmm4;		// 0A0R 0G0B 0A0R 0G0B
	punpcklbw	xmm7, xmm4;		// 0A0R 0G0B 0A0R 0G0B
	paddw		xmm6, oword ptr[gc_nRoundComp_1];	// Comp
	paddw		xmm7, oword ptr[gc_nRoundComp_1];
	paddw		xmm6, xmm6;		// SC * 2
	paddw		xmm7, xmm7;
	pmulhw		xmm6, xmm0;		// SC * SKA
	pmulhw		xmm7, xmm1;		// DC * DKA
	paddusb		xmm6, xmm7;		// SC * SKA + DC * DKA
	packuswb	xmm6, xmm4;		// 0000 0000 @RGB @RGB
	pand		xmm6, oword ptr[gc_dwMaskRGB];
	por			xmm5, xmm6;		// mix |= (RGB) 存放 0000 0000 ARGB ARGB
	;// 写入 Mix 结果
	movlps		QWORD PTR[rdi], xmm5;
	add			rsi, 08h;
	add			rdi, 08h;
loop_bgra32_line_linear_dodge_a4:
	test		ecx, 00000004h;
	jz			loop_bgra32_line_linear_dodge_end;
	;// 根据 Offset_S 与 Offset_D 查增益表，由于表为 DWORD 型，转成字节时需要乘以长度
	movdqu		xmm6, [rsi];	// src(@ @ A1R1G1B1 A0R0G0B0)
	movdqu		xmm7, [rdi];	// des(@ @ A1R1G1B1 A0R0G0B0)
	movdqa		xmm4, xmm6;
	movdqa		xmm5, xmm7;
	psrld		xmm4, 18h;		// sa(src alpha @ @ A1 A0)
	psrld		xmm5, 18h;		// da(des alpha @ @ A1 A0)
	movd		xmm1, edx;		// 表头 0 0 0 table
	movdqa		xmm0, xmm4;
	movdqa		xmm2, xmm4;
	movdqa		xmm3, xmm5;
	pmullw		xmm4, xmm5;		// sa * da
	pslld		xmm2, 08h;		// sa * 256
	pslld		xmm3, 08h;		// da * 256
	shufps		xmm1, xmm1, 00h;	// table table table table
	paddd		xmm4, oword ptr[gc_dwMaskDLB];	// sa * da + 255
	paddd		xmm2, xmm5;		// sa * 256 + da
	paddd		xmm3, xmm0;		// da * 256 + sa
	paddd		xmm5, xmm0;		// sa + da
	psrld		xmm4, 08h;		// (sa * da + 255) >> 8
	pslld		xmm2, 02h;		// (sa * 256 + da) * 4
	pslld		xmm3, 02h;		// (da * 256 + sa) * 4
	psubd		xmm5, xmm4;		// 计算 MA = SA + DA - (SA * DA + 255) >> 8
	paddd		xmm2, xmm1;		// Src 增益地址 @ @ ptr1 ptr0
	paddd		xmm3, xmm1;		// Des 增益地址 @ @ ptr1 ptr0
	pslld		xmm5, 18h;		// 结果 A000 A000 A000 A000
	;// 挨个像素取出增益值
	movd		eax, xmm2;		// 第 0 个像素的增益 ptr0
	movd		ebx, xmm3;
	shufps		xmm2, xmm2, 39h;	// ptr0 0 0 ptr1
	shufps		xmm3, xmm3, 39h;	// ptr0 0 0 ptr1
	movd		xmm0, DWORD PTR[rax];	// 存放 0 0 0 K0
	movd		xmm1, DWORD PTR[rbx];	// 存放 0 0 0 K0
	shufps		xmm0, xmm0, 00h;	// SK0
	shufps		xmm1, xmm1, 00h;	// DK0
	packssdw	xmm0, xmm1;		// DK0  SK0
	pxor		xmm4, xmm4;		// 0 0 0 0
	punpcklbw	xmm6, xmm4;		// 0000 0000 0A0R 0G0B
	punpcklbw	xmm7, xmm4;		// 0000 0000 0A0R 0G0B
	movlhps		xmm6, xmm7;		// DC  SC
	paddw		xmm6, oword ptr[gc_nRoundComp_1];
	paddw		xmm6, xmm6;
	pmulhw		xmm6, xmm0;		// DC * DKA    SC * SKA
	movhlps		xmm7, xmm6;
	paddusb		xmm6, xmm7;		// SC * SKA + DC * DKA
	packuswb	xmm6, xmm4;		// 0000 0000 @@@@ @RGB
	pand		xmm6, oword ptr[gc_dwMaskRGB];
	por			xmm5, xmm6;		// mix |= (RGB) 存放 0000 0000 0000 ARGB
	;// 写入 Mix 结果
	movss		DWORD PTR[rdi], xmm5;
loop_bgra32_line_linear_dodge_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
	pop			rbx
    pop			rsi
	pop			rdi
    ret
    
    
 ;-----------------------------------------------------------------------------
; void bgra32_line_linear_burn_x64(IN OUT void *des,		// 目标
;										IN void *src,			// 源
;										IN int na16,			// 宽度除以 16 的商
;										IN int nm16,			// 宽度除以 16 的余数
;										IN void *ka_table);		// 256 * 256 * sizeof(DWORD) 的 KA 表
;-----------------------------------------------------------------------------

bgra32_line_linear_burn_x64:
	;[rsp+28h] - ka_table
	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    
    push		rdi
    push		rsi
    push		rbx
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
    
    mov			rdi, rcx;
	mov			rsi, rdx;
    mov			rcx, r8;
	mov			rdx, [rsp+28h+38h];
	test		ecx, ecx;
	jnz			loop_bgra32_line_linear_burn_a16;
	jmp			loop_bgra32_line_linear_burn_a8;
loop_bgra32_line_linear_burn_a16:
	;// 根据 Offset_S 与 Offset_D 查增益表，由于表为 DWORD 型，转成字节时需要乘以长度
	movdqu		xmm6, [rsi];	// src(A3R3G3B3 A2R2G2B2 A1R1G1B1 A0R0G0B0)
	movdqu		xmm7, [rdi];	// des(A3R3G3B3 A2R2G2B2 A1R1G1B1 A0R0G0B0)
	movdqa		xmm4, xmm6;
	movdqa		xmm5, xmm7;
	psrld		xmm4, 18h;		// sa(src alpha A3 A2 A1 A0)
	psrld		xmm5, 18h;		// da(des alpha A3 A2 A1 A0)
	movd		xmm1, edx;		// 表头 0 0 0 table
	movdqa		xmm0, xmm4;
	movdqa		xmm2, xmm4;
	movdqa		xmm3, xmm5;
	pmullw		xmm4, xmm5;		// sa * da
	pslld		xmm2, 08h;		// sa * 256
	pslld		xmm3, 08h;		// da * 256
	shufps		xmm1, xmm1, 00h;	// table table table table
	paddd		xmm4, oword ptr[gc_dwMaskDLB];	// sa * da + 255
	paddd		xmm2, xmm5;		// sa * 256 + da
	paddd		xmm3, xmm0;		// da * 256 + sa
	paddd		xmm5, xmm0;		// sa + da
	psrld		xmm4, 08h;		// (sa * da + 255) >> 8
	pslld		xmm2, 02h;		// (sa * 256 + da) * 4
	pslld		xmm3, 02h;		// (da * 256 + sa) * 4
	psubd		xmm5, xmm4;		// 计算 MA = SA + DA - (SA * DA + 255) >> 8
	paddd		xmm2, xmm1;		// Src 增益地址 ptr3 ptr2 ptr1 ptr0
	paddd		xmm3, xmm1;		// Des 增益地址 ptr3 ptr2 ptr1 ptr0
	pslld		xmm5, 18h;		// 结果 A000 A000 A000 A000
	;// 挨个像素取出增益值
	movdqa		xmm1, xmm2;		// SKA
	shufps		xmm2, xmm2, 39h;	// ptr0 ptr3 ptr2 ptr1
	movd		eax, xmm1;		// 第 0 个像素的增益 ptr0
	movd		ebx, xmm2;		// 第 1 个像素的增益 ptr1
	movd		xmm0, DWORD PTR[rax];	// 存放 0 0 0 K0
	movd		xmm4, DWORD PTR[rbx];	// 存放 0 0 0 K1
	movhlps		xmm1, xmm1;		// ptr3 ptr2 ptr3 ptr2
	movhlps		xmm2, xmm2;		// ptr0 ptr3 ptr0 ptr3
	movd		eax, xmm1;		// 第 2 个像素的增益 ptr2
	movd		ebx, xmm2;		// 第 3 个像素的增益 ptr3
	movd		xmm1, DWORD PTR[rax];	// 存放 0 0 0 K2
	movd		xmm2, DWORD PTR[rbx];	// 存放 0 0 0 K3
	shufps		xmm0, xmm4, 00h;	// K1 K1 K0 K0
	shufps		xmm1, xmm2, 00h;	// K3 K3 K2 K2
	movdqa		xmm4, xmm3;		// DKA
	shufps		xmm3, xmm3, 39h;	// ptr0 ptr3 ptr2 ptr1
	packssdw	xmm0, xmm1;		// K3K3 K2K2 K1K1 K0K0
	movd		eax, xmm4;		// 第 0 个像素的增益 ptr0
	movd		ebx, xmm3;		// 第 1 个像素的增益 ptr1
	movd		xmm1, DWORD PTR[rax];	// 存放 0 0 0 K0
	movd		xmm2, DWORD PTR[rbx];	// 存放 0 0 0 K1
	movhlps		xmm4, xmm4;		// ptr3 ptr2 ptr3 ptr2
	movhlps		xmm3, xmm3;		// ptr0 ptr3 ptr0 ptr3
	movd		eax, xmm4;		// 第 2 个像素的增益 ptr2
	movd		ebx, xmm3;		// 第 3 个像素的增益 ptr3
	movd		xmm4, DWORD PTR[rax];	// 存放 0 0 0 K2
	movd		xmm3, DWORD PTR[rbx];	// 存放 0 0 0 K3
	shufps		xmm1, xmm2, 00h;	// K1 K1 K0 K0
	shufps		xmm4, xmm3, 00h;	// K3 K3 K2 K2
	packssdw	xmm1, xmm4;		// K3K3 K2K2 K1K1 K0K0
	;// 先计算 R B 再单独计算 G
	movdqa		xmm2, xmm6;
	movdqa		xmm3, xmm7;
	movdqa		xmm4, oword ptr[gc_nRoundComp_1];
	psrlw		xmm6, 08h;		// src AG(0A0G 0A0G 0A0G 0A0G)
	psrlw		xmm7, 08h;		// des AG(0A0G 0A0G 0A0G 0A0G)
	pand		xmm2, oword ptr[gc_dwMaskWLB];	// src RB(0R0B 0R0B 0R0B 0R0B)
	pand		xmm3, oword ptr[gc_dwMaskWLB];	// des RB(0R0B 0R0B 0R0B 0R0B)
	paddw		xmm6, xmm4;		// Comp
	paddw		xmm7, xmm4;
	paddw		xmm2, xmm4;
	paddw		xmm3, xmm4;
	paddw		xmm6, xmm6;		// SC * 2
	paddw		xmm7, xmm7;
	paddw		xmm2, xmm2;
	paddw		xmm3, xmm3;
	pmulhw		xmm6, xmm0;		// SC * SKA
	pmulhw		xmm7, xmm1;		// DC * DKA
	pmulhw		xmm2, xmm0;
	pmulhw		xmm3, xmm1;
	paddsw		xmm6, xmm7;		// SC * SKA + DC * DKA
	paddsw		xmm2, xmm3;
	psubusw		xmm6, oword ptr[gc_dwMaskWLB];
	psubusw		xmm2, oword ptr[gc_dwMaskWLB];
	pslld		xmm6, 08h;
	por			xmm5, xmm2;		// mix |= (mrb) 存放 AR0B AR0B AR0B AR0B
	pand		xmm6, oword ptr[gc_dwMaskG];
	por			xmm5, xmm6;		// mix |= (mg << 8) 存放 ARGB ARGB ARGB ARGB
	;// 写入 Mix 结果并开始下一个连续的四个像素计算
	movdqu		[rdi], xmm5;
	add			rsi, 10h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_bgra32_line_linear_burn_a16;
loop_bgra32_line_linear_burn_a8:
	mov			rcx, r9;
	test		ecx, 00000008h;
	jz			loop_bgra32_line_linear_burn_a4;
	;// 根据 Offset_S 与 Offset_D 查增益表，由于表为 DWORD 型，转成字节时需要乘以长度
	movdqu		xmm6, [rsi];	// src(@ @ A1R1G1B1 A0R0G0B0)
	movdqu		xmm7, [rdi];	// des(@ @ A1R1G1B1 A0R0G0B0)
	movdqa		xmm4, xmm6;
	movdqa		xmm5, xmm7;
	psrld		xmm4, 18h;		// sa(src alpha @ @ A1 A0)
	psrld		xmm5, 18h;		// da(des alpha @ @ A1 A0)
	movd		xmm1, edx;		// 表头 0 0 0 table
	movdqa		xmm0, xmm4;
	movdqa		xmm2, xmm4;
	movdqa		xmm3, xmm5;
	pmullw		xmm4, xmm5;		// sa * da
	pslld		xmm2, 08h;		// sa * 256
	pslld		xmm3, 08h;		// da * 256
	shufps		xmm1, xmm1, 00h;	// table table table table
	paddd		xmm4, oword ptr[gc_dwMaskDLB];	// sa * da + 255
	paddd		xmm2, xmm5;		// sa * 256 + da
	paddd		xmm3, xmm0;		// da * 256 + sa
	paddd		xmm5, xmm0;		// sa + da
	psrld		xmm4, 08h;		// (sa * da + 255) >> 8
	pslld		xmm2, 02h;		// (sa * 256 + da) * 4
	pslld		xmm3, 02h;		// (da * 256 + sa) * 4
	psubd		xmm5, xmm4;		// 计算 MA = SA + DA - (SA * DA + 255) >> 8
	paddd		xmm2, xmm1;		// Src 增益地址 @ @ ptr1 ptr0
	paddd		xmm3, xmm1;		// Des 增益地址 @ @ ptr1 ptr0
	pslld		xmm5, 18h;		// 结果 A000 A000 A000 A000
	;// 挨个像素取出增益值
	movd		eax, xmm2;		// 第 0 个像素的增益 ptr0
	movd		ebx, xmm3;
	shufps		xmm2, xmm2, 39h;	// ptr0 0 0 ptr1
	shufps		xmm3, xmm3, 39h;	// ptr0 0 0 ptr1
	movd		xmm0, DWORD PTR[rax];	// 存放 0 0 0 K0
	movd		xmm1, DWORD PTR[rbx];	// 存放 0 0 0 K0
	movd		eax, xmm2;		// 第 1 个像素的增益 ptr1
	movd		ebx, xmm3;
	movd		xmm2, DWORD PTR[rax];	// 存放 0 0 0 K1
	movd		xmm3, DWORD PTR[rbx];	// 存放 0 0 0 K1
	shufps		xmm0, xmm0, 00h;	// K0 K0 K0 K0
	shufps		xmm1, xmm1, 00h;	// K0 K0 K0 K0
	shufps		xmm2, xmm2, 00h;	// K1 K1 K1 K1
	shufps		xmm3, xmm3, 00h;	// K1 K1 K1 K1
	packssdw	xmm0, xmm2;		// K1K1 K1K1 K0K0 K0K0
	packssdw	xmm1, xmm3;		// K1K1 K1K1 K0K0 K0K0
	;// 同时计算 R G B
	pxor		xmm4, xmm4;		// 0 0 0 0
	punpcklbw	xmm6, xmm4;		// 0A0R 0G0B 0A0R 0G0B
	punpcklbw	xmm7, xmm4;		// 0A0R 0G0B 0A0R 0G0B
	paddw		xmm6, oword ptr[gc_nRoundComp_1];	// Comp
	paddw		xmm7, oword ptr[gc_nRoundComp_1];
	paddw		xmm6, xmm6;		// SC * 2
	paddw		xmm7, xmm7;
	pmulhw		xmm6, xmm0;		// SC * SKA
	pmulhw		xmm7, xmm1;		// DC * DKA
	paddsw		xmm6, xmm7;		// SC * SKA + DC * DKA
	psubusw		xmm6, oword ptr[gc_dwMaskWLB];
	packuswb	xmm6, xmm4;		// 0000 0000 @RGB @RGB
	pand		xmm6, oword ptr[gc_dwMaskRGB];
	por			xmm5, xmm6;		// mix |= (RGB) 存放 0000 0000 ARGB ARGB
	;// 写入 Mix 结果
	movlps		QWORD PTR[rdi], xmm5;
	add			rsi, 08h;
	add			rdi, 08h;
loop_bgra32_line_linear_burn_a4:
	test		ecx, 00000004h;
	jz			loop_bgra32_line_linear_burn_end;
	;// 根据 Offset_S 与 Offset_D 查增益表，由于表为 DWORD 型，转成字节时需要乘以长度
	movdqu		xmm6, [rsi];	// src(@ @ A1R1G1B1 A0R0G0B0)
	movdqu		xmm7, [rdi];	// des(@ @ A1R1G1B1 A0R0G0B0)
	movdqa		xmm4, xmm6;
	movdqa		xmm5, xmm7;
	psrld		xmm4, 18h;		// sa(src alpha @ @ A1 A0)
	psrld		xmm5, 18h;		// da(des alpha @ @ A1 A0)
	movd		xmm1, edx;		// 表头 0 0 0 table
	movdqa		xmm0, xmm4;
	movdqa		xmm2, xmm4;
	movdqa		xmm3, xmm5;
	pmullw		xmm4, xmm5;		// sa * da
	pslld		xmm2, 08h;		// sa * 256
	pslld		xmm3, 08h;		// da * 256
	shufps		xmm1, xmm1, 00h;	// table table table table
	paddd		xmm4, oword ptr[gc_dwMaskDLB];	// sa * da + 255
	paddd		xmm2, xmm5;		// sa * 256 + da
	paddd		xmm3, xmm0;		// da * 256 + sa
	paddd		xmm5, xmm0;		// sa + da
	psrld		xmm4, 08h;		// (sa * da + 255) >> 8
	pslld		xmm2, 02h;		// (sa * 256 + da) * 4
	pslld		xmm3, 02h;		// (da * 256 + sa) * 4
	psubd		xmm5, xmm4;		// 计算 MA = SA + DA - (SA * DA + 255) >> 8
	paddd		xmm2, xmm1;		// Src 增益地址 @ @ ptr1 ptr0
	paddd		xmm3, xmm1;		// Des 增益地址 @ @ ptr1 ptr0
	pslld		xmm5, 18h;		// 结果 A000 A000 A000 A000
	;// 挨个像素取出增益值
	movd		eax, xmm2;		// 第 0 个像素的增益 ptr0
	movd		ebx, xmm3;
	shufps		xmm2, xmm2, 39h;	// ptr0 0 0 ptr1
	shufps		xmm3, xmm3, 39h;	// ptr0 0 0 ptr1
	movd		xmm0, DWORD PTR[rax];	// 存放 0 0 0 K0
	movd		xmm1, DWORD PTR[rbx];	// 存放 0 0 0 K0
	shufps		xmm0, xmm0, 00h;	// SK0
	shufps		xmm1, xmm1, 00h;	// DK0
	packssdw	xmm0, xmm1;		// DK0  SK0
	pxor		xmm4, xmm4;		// 0 0 0 0
	punpcklbw	xmm6, xmm4;		// 0000 0000 0A0R 0G0B
	punpcklbw	xmm7, xmm4;		// 0000 0000 0A0R 0G0B
	movlhps		xmm6, xmm7;		// DC  SC
	paddw		xmm6, oword ptr[gc_nRoundComp_1];
	paddw		xmm6, xmm6;
	pmulhw		xmm6, xmm0;		// DC * DKA    SC * SKA
	movhlps		xmm7, xmm6;
	paddsw		xmm6, xmm7;		// SC * SKA + DC * DKA
	psubusw		xmm6, oword ptr[gc_dwMaskWLB];
	packuswb	xmm6, xmm4;		// 0000 0000 @@@@ @RGB
	pand		xmm6, oword ptr[gc_dwMaskRGB];
	por			xmm5, xmm6;		// mix |= (RGB) 存放 0000 0000 0000 ARGB
	;// 写入 Mix 结果
	movss		DWORD PTR[rdi], xmm5;
loop_bgra32_line_linear_burn_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
    pop			rbx
    pop			rsi
	pop			rdi
    ret
    
        
;-----------------------------------------------------------------------------
; void bgra32_powerfill_x64(OUT void* des,		// 目标
;								 IN DWORD c0,			// 起点颜色
;								 IN DWORD c1,			// 终点颜色
;								 IN short* power,		// 权重
;								 IN int na32,			// 宽度除以 32 的商
;								 IN int nm32);			// 宽度除以 32 的余数
;-----------------------------------------------------------------------------

bgra32_powerfill_x64:
	;[rsp+30h] - nm32
	;[rsp+28h] - na32
	;r9 - power
	;r8 - c1
	;rdx - c0
    ;rcx - des 
    
    push		rdi
    push		rsi    
    sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
	
    mov			rdi, rcx;
    mov			rax, rdx;
	mov			rdx, rcx;
	mov			rsi, r9;
	mov			ecx, [rsp+28h+30h];
	add			rdx, 10h;
	pxor		xmm7, xmm7;
	movd		xmm0, eax;			// 0000 0000 0000 ARGB
	movd		xmm1, r8;
	punpcklbw	xmm0, xmm7;			// 0 0 0 0 A R G B
	punpcklbw	xmm1, xmm7;
	movlhps		xmm0, xmm0;			// A R G B A R G B
	movlhps		xmm1, xmm1;
	psubw		xmm0, xmm1;			// c0 - c1
	psllw		xmm1, 05h;			// 放大 5 位
	psllw		xmm0, 07h;			// 放大 7 位
	test		ecx, ecx;
	jnz			loop_bgra32_powerfill_a32;
	jmp			loop_bgra32_powerfill_m16;
loop_bgra32_powerfill_a32:
	movdqu		xmm2, [rsi];		// p7 p6 p5 p4 p3 p2 p1 p0
	movdqa		xmm6, oword ptr[gc_nRoundComp_5];
	movdqa		xmm4, xmm2;
	punpcklwd	xmm2, xmm2;			// p3 p3 p2 p2 p1 p1 p0 p0
	punpckhwd	xmm4, xmm4;			// p7 p7 p6 p6 p5 p5 p4 p4
	movdqa		xmm3, xmm2;
	movdqa		xmm5, xmm4;
	shufps		xmm2, xmm2, 050h;	// p1 p1 p1 p1 p0 p0 p0 p0
	shufps		xmm3, xmm3, 0fah;	// p3 p3 p3 p3 p2 p2 p2 p2
	shufps		xmm4, xmm4, 050h;	// p5 p5 p5 p5 p4 p4 p4 p4
	shufps		xmm5, xmm5, 0fah;	// p7 p7 p7 p7 p6 p6 p6 p6
	pmulhw		xmm2, xmm0;			// (c0 - c1) * f
	pmulhw		xmm3, xmm0;
	pmulhw		xmm4, xmm0;
	pmulhw		xmm5, xmm0;
	paddw		xmm2, xmm1;			// (c0 - c1) * f + c1
	paddw		xmm3, xmm1;
	paddw		xmm4, xmm1;
	paddw		xmm5, xmm1;
	paddw		xmm2, xmm6;			// 四舍五入
	paddw		xmm3, xmm6;
	paddw		xmm4, xmm6;
	paddw		xmm5, xmm6;
	pmaxsw		xmm2, xmm7;
	pmaxsw		xmm3, xmm7;
	pmaxsw		xmm4, xmm7;
	pmaxsw		xmm5, xmm7;
	movdqa		xmm6, oword ptr[gc_dwMaskWLB];
	psrlw		xmm2, 05h;
	psrlw		xmm3, 05h;
	psrlw		xmm4, 05h;
	psrlw		xmm5, 05h;
	pminsw		xmm2, xmm6;			// A R G B A R G B
	pminsw		xmm3, xmm6;
	pminsw		xmm4, xmm6;
	pminsw		xmm5, xmm6;
	packuswb	xmm2, xmm3;
	packuswb	xmm4, xmm5;
	movdqu		[rdi], xmm2;
	movdqu		[rdx], xmm4;
	add			rsi, 10h;
	add			rdi, 20h;
	add			rdx, 20h;
	sub			ecx, 01h;
	jnz			loop_bgra32_powerfill_a32;
loop_bgra32_powerfill_m16:
	mov			ecx, [rsp+30h+30h];
	movdqa		xmm6, oword ptr[gc_nRoundComp_5];
	movdqa		xmm5, oword ptr[gc_dwMaskWLB];
	test		ecx, 10h;
	jz			loop_bgra32_powerfill_m8;
	movlps		xmm2, QWORD PTR[rsi];		// @ @ @ @ p3 p2 p1 p0
	punpcklwd	xmm2, xmm2;			// p3 p3 p2 p2 p1 p1 p0 p0
	movdqa		xmm3, xmm2;
	shufps		xmm2, xmm2, 50h;	// p1 p1 p1 p1 p0 p0 p0 p0
	shufps		xmm3, xmm3, 0fah;	// p3 p3 p3 p3 p2 p2 p2 p2
	pmulhw		xmm2, xmm0;			// (c0 - c1) * f
	pmulhw		xmm3, xmm0;
	paddw		xmm2, xmm1;			// (c0 - c1) * f + c1
	paddw		xmm3, xmm1;
	paddw		xmm2, xmm6;			// 四舍五入
	paddw		xmm3, xmm6;
	pmaxsw		xmm2, xmm7;
	pmaxsw		xmm3, xmm7;
	psrlw		xmm2, 05h;
	psrlw		xmm3, 05h;
	pminsw		xmm2, xmm5;			// A R G B A R G B
	pminsw		xmm3, xmm5;
	packuswb	xmm2, xmm3;
	movdqu		[rdi], xmm2;
	add			rsi, 08h;
	add			rdi, 10h;
loop_bgra32_powerfill_m8:
	test		ecx, 08h;
	jz			loop_bgra32_powerfill_m4;
	movss		xmm2, DWORD PTR[rsi];		// 0 0 0 0 0 0 p1 p0
	punpcklwd	xmm2, xmm2;			// 0 0 0 0 p1 p1 p0 p0
	shufps		xmm2, xmm2, 50h;	// p1 p1 p1 p1 p0 p0 p0 p0
	pmulhw		xmm2, xmm0;			// (c0 - c1) * f
	paddw		xmm2, xmm1;			// (c0 - c1) * f + c1
	paddw		xmm2, xmm6;			// 四舍五入
	pmaxsw		xmm2, xmm7;
	psrlw		xmm2, 05h;
	pminsw		xmm2, xmm5;			// A R G B A R G B
	packuswb	xmm2, xmm2;
	movlps		QWORD PTR[rdi], xmm2;
	add			rsi, 04h;
	add			rdi, 08h;
loop_bgra32_powerfill_m4:
	test		ecx, 04h;
	jz			loop_bgra32_powerfill_end;
	pinsrw		xmm2, word ptr[rsi], 00h;	// 0 0 0 0 0 0 0 p0
	pshuflw		xmm2, xmm2, 00h;	// 0 0 0 0 p0 p0 p0 p0
	pmulhw		xmm2, xmm0;			// (c0 - c1) * f
	paddw		xmm2, xmm1;			// (c0 - c1) * f + c1
	paddw		xmm2, xmm6;			// 四舍五入
	pmaxsw		xmm2, xmm7;
	psrlw		xmm2, 05h;
	pminsw		xmm2, xmm5;			// @ @ @ @ A R G B
	packuswb	xmm2, xmm2;
	movss		DWORD PTR[rdi], xmm2;
loop_bgra32_powerfill_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
    pop			rsi
	pop			rdi
    ret
    
    
;-----------------------------------------------------------------------------
; void bgra32_powerfill_q_x64(OUT void* des,		// 目标
;								   IN DWORD c0,		// 起点颜色
;								   IN DWORD c1,		// 终点颜色
;								   IN float fPrev,		// 起点前一个权重
;								   IN float fDelta,	// 像素间的权重增量
;								   IN int na16,		// 宽度除以 16 的商（一次做 4 个像素）
;								   IN int nm16);		// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

bgra32_powerfill_q_x64:
	;[rsp+38h] - nm16
	;[rsp+30h] - na16
	;[rsp+28h] - fDelta
	;xmm3 - fPrev
	;r8 - c1
	;rdx - c0
    ;rcx - des
    
    push		rdi
    push		rsi    
   	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
    
    mov			rdi, rcx;
    mov			ecx, [rsp+30h+30h];
	
	movd		xmm0, rdx;			// 0000 0000 0000 ARGB
	movd		xmm1, r8;
	movss		xmm5, xmm3;		// 前一次的权重
	movss		xmm6, DWORD PTR[rsp+28h+30h];		// 权重增量
	pxor		xmm7, xmm7;
	pxor		xmm4, xmm4;
	punpcklbw	xmm0, xmm7;			// 0 0 0 0 A R G B
	punpcklbw	xmm1, xmm7;
	shufps		xmm4, xmm6, 00h;	// d  d  0  0
	shufps		xmm5, xmm5, 00h;	// p  p  p  p
	addps		xmm4, xmm4;			// 2d  2d  0  0
	movlhps		xmm0, xmm0;			// A R G B A R G B
	movlhps		xmm1, xmm1;
	shufps		xmm6, xmm4, 0a0h;	// 2d  2d  d  d
	shufps		xmm6, xmm6, 88h;	// 2d  d  2d  d
	psubw		xmm0, xmm1;			// c0 - c1
	addps		xmm6, xmm4;			// 4d  3d  2d  d
	addps		xmm5, xmm6;			// p3  p2  p1  p0
	psllw		xmm0, 07h;			// 放大 7 位
	psllw		xmm1, 05h;			// 放大 5 位
	shufps		xmm6, xmm6, 0ffh;	// 4d  4d  4d  4d
	test		ecx, ecx;
	jnz			loop_bgra32_powerfill_q_a16;
	jmp			loop_bgra32_powerfill_q_m8;
loop_bgra32_powerfill_q_a16:
	movaps		xmm2, xmm5;			// p3  p2  p1  p0
	movdqa		xmm4, oword ptr[gc_nRoundComp_5];
	cvtps2dq	xmm2, xmm2;			// 取整
	movdqa		xmm3, xmm2;
	punpcklwd	xmm2, xmm2;			// @ @ p1 p1 @ @ p0 p0
	punpckhwd	xmm3, xmm3;			// @ @ p3 p3 @ @ p2 p2
	shufps		xmm2, xmm2, 0a0h;	// p1 p1 p1 p1 p0 p0 p0 p0
	shufps		xmm3, xmm3, 0a0h;	// p3 p3 p3 p3 p2 p2 p2 p2
	pmulhw		xmm2, xmm0;			// (c0 - c1) * f
	pmulhw		xmm3, xmm0;
	paddw		xmm2, xmm1;			// (c0 - c1) * f + c1
	paddw		xmm3, xmm1;
	paddw		xmm2, xmm4;			// 四舍五入
	paddw		xmm3, xmm4;
	pmaxsw		xmm2, xmm7;
	pmaxsw		xmm3, xmm7;
	movdqa		xmm4, oword ptr[gc_dwMaskWLB];
	psrlw		xmm2, 05h;
	psrlw		xmm3, 05h;
	pminsw		xmm2, xmm4;			// A R G B A R G B
	pminsw		xmm3, xmm4;
	packuswb	xmm2, xmm3;
	addps		xmm5, xmm6;			// 新权重 p3  p2  p1  p0
	movdqu		[rdi], xmm2;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_bgra32_powerfill_q_a16;
loop_bgra32_powerfill_q_m8:
	mov			ecx, [rsp+38h+30h];
	cvtps2dq	xmm5, xmm5;			// 取整
	movdqa		xmm3, oword ptr[gc_dwMaskWLB];
	movdqa		xmm4, oword ptr[gc_nRoundComp_5];
	pand		xmm5, xmm3;			// p3  p2  p1  p0
	test		ecx, 08h;
	jz			loop_bgra32_powerfill_q_m4;
	movdqa		xmm2, xmm5;
	punpcklwd	xmm2, xmm2;			// @ @ p1 p1 @ @ p0 p0
	shufps		xmm2, xmm2, 0a0h;	// p1 p1 p1 p1 p0 p0 p0 p0
	pmulhw		xmm2, xmm0;			// (c0 - c1) * f
	paddw		xmm2, xmm1;			// (c0 - c1) * f + c1
	paddw		xmm2, xmm4;			// 四舍五入
	pmaxsw		xmm2, xmm7;
	psrlw		xmm2, 05h;
	pminsw		xmm2, xmm3;			// A R G B A R G B
	packuswb	xmm2, xmm2;
	movlps		QWORD PTR[rdi], xmm2;
	add			rdi, 08h;
loop_bgra32_powerfill_q_m4:
	test		ecx, 04h;
	jz			loop_bgra32_powerfill_q_end;
	punpckhwd	xmm5, xmm5;			// @ @ p3 p3 @ @ p2 p2
	shufps		xmm5, xmm5, 0a0h;	// p3 p3 p3 p3 p2 p2 p2 p2
	pmulhw		xmm5, xmm0;			// (c0 - c1) * f
	paddw		xmm5, xmm1;			// (c0 - c1) * f + c1
	paddw		xmm5, xmm4;			// 四舍五入
	pmaxsw		xmm5, xmm7;
	psrlw		xmm5, 05h;
	pminsw		xmm5, xmm3;			// @ @ @ @ A R G B
	packuswb	xmm5, xmm5;
	movss		DWORD PTR[rdi], xmm5;
loop_bgra32_powerfill_q_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
    pop			rsi
	pop			rdi
    ret
    
    
  
;-----------------------------------------------------------------------------
; void bgra32_linear_recursive_x64(OUT void* des,			// 目标
;										IN float* pfCr0,		// 起点颜色
;										IN float* pfCr1,		// 终点颜色
;										IN float fX,			// X 的起始点（像素索引）
;										IN float fCoefA,		// 系数 A
;										IN float fCoefB,		// 系数 B
;										IN float* pfCoefC,		// 系数 C
;										IN int na16,			// 宽度除以 16 的商（一次做 4 个像素）
;										IN int nm16);			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

bgra32_linear_recursive_x64:
	;[rsp+48h] - nm16
	;[rsp+40h] - na16
	;[rsp+38h] - pfCoefC
	;[rsp+30h] - fCoefB
	;[rsp+28h] - fCoefA
	;xmm3 - fX
	;r8 - pfCr1
	;rdx - pfCr0
    ;rcx - des 
    
    push		rdi
    push		rsi    
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
    
	mov			rdi, rcx;
	mov			rax, rdx;
	mov			rdx, r8;
	mov			rcx, [rsp+40h+30h];
	mov			rsi, [rsp+38h+30h];
	;// 先计算前四个像素的颜色值
	movss		xmm0, xmm3;			// x
	movss		xmm1, DWORD PTR[rsp+28h+30h];		// A
	movss		xmm2, DWORD PTR[rsp+30h+30h];		// B
	movups		xmm3, [rsi];		// C
	movups		xmm6, [rax];		// c0
	movups		xmm7, [rdx];		// c1
	movdqa		xmm5, oword ptr[gc_fRoundComp];	// 四舍五入
	subps		xmm0, xmm2;			// x - B
	subps		xmm7, xmm6;			// c1 - c0
	addps		xmm3, xmm5;
	shufps		xmm0, xmm0, 00h;
	shufps		xmm1, xmm1, 00h;
	mulps		xmm7, xmm1;			// A * (c1 - c0) = Delta
	mulps		xmm0, xmm7;
	addps		xmm0, xmm3;			// D0
	movdqa		xmm1, xmm0;
	movdqa		xmm2, xmm0;
	addps		xmm1, xmm7;			// D1
	addps		xmm7, xmm7;			// 2 * Delta
	movdqa		xmm3, xmm1;
	addps		xmm2, xmm7;			// D2
	addps		xmm3, xmm7;			// D3
	addps		xmm7, xmm7;			// 4 * Delta
	test		ecx, ecx;
	jnz			loop_bgra32_linear_recursive_a16;
	jmp			loop_bgra32_linear_recursive_m8;
loop_bgra32_linear_recursive_a16:
	cvtps2dq	xmm4, xmm0;			// D0（不做安全检测）
	cvtps2dq	xmm5, xmm1;			// D1
	addps		xmm0, xmm7;
	addps		xmm1, xmm7;
	packssdw	xmm4, xmm5;			// D1 D0
	cvtps2dq	xmm5, xmm2;			// D2
	cvtps2dq	xmm6, xmm3;			// D3
	addps		xmm2, xmm7;
	addps		xmm3, xmm7;
	packssdw	xmm5, xmm6;			// D3 D2
	packuswb	xmm4, xmm5;			// D3 D2 D1 D0
	movdqu		[rdi], xmm4;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_bgra32_linear_recursive_a16;
loop_bgra32_linear_recursive_m8:
	mov			ecx, [rsp+48h+30h];
	test		ecx, 08h;
	jz			loop_bgra32_linear_recursive_m4;
	cvtps2dq	xmm4, xmm0;			// D0（不做安全检测）
	cvtps2dq	xmm5, xmm1;			// D1
	movdqa		xmm0, xmm2;			// 最后一个像素
	packssdw	xmm4, xmm5;			// D1 D0
	packuswb	xmm4, xmm4;			// D1 D0 D1 D0
	movlps		QWORD PTR[rdi], xmm4;
	add			rdi, 08h;
loop_bgra32_linear_recursive_m4:
	test		ecx, 04h;
	jz			loop_bgra32_linear_recursive_end;
	cvtps2dq	xmm4, xmm0;			// D0（不做安全检测）
	packssdw	xmm4, xmm4;			// D0 D0
	packuswb	xmm4, xmm4;			// D0 D0 D0 D0
	movss		DWORD PTR[rdi], xmm4;
loop_bgra32_linear_recursive_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
    pop			rsi
	pop			rdi
    ret
    
      
      
;-----------------------------------------------------------------------------
; void bgra32_mean_copy_add_y_x64(OUT void* des,			// 临时缓存行（是源行拷贝宽度的四倍）
;									   IN void* src,			// 源拷贝起始行
;									   IN int scw,				// 源拷贝宽度
;									   IN int src_pitch,		// 源行字节数
;									   IN void* pPowerY,		// 目标某行映射的垂直方向上的权重表
;									   IN void* pCoefH,		// 垂直方向上的缩放系数
;									   IN int nMapY);			// 目标一行映射到源的行数
;-----------------------------------------------------------------------------

bgra32_mean_copy_add_y_x64:
	;[rsp+38h] - nMapY
	;[rsp+30h] - pCoefH
	;[rsp+28h] - pPowerY
	;r9 - src_pitch
	;r8 - scw
	;rdx - src
    ;rcx - des 
    
    push		rsi
    push		rdi
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
	
    mov			rdi, rcx;
	mov			rsi, rdx;
	mov			rcx, r8;
	mov			rdx, [rsp+28h+30h];
	movss		xmm2, DWORD PTR[rdx];		// 垂直方向上的首权重 0 0 0 powStart
	movss		xmm3, DWORD PTR[rdx + 04h];	// 垂直方向上的尾权重 0 0 0 powEnd
	shufps		xmm2, xmm2, 00h;	// power power power power
	shufps		xmm3, xmm3, 00h;	// power power power power
	mov			rax, [rsp+30h+30h];
	pxor		xmm0, xmm0;			// 0 0 0 0
	movaps		xmm7, [rax];		// 缩放系数 coef coef coef coef
loop_bgra32_mean_copy_add_y_pixel:
	mov			rax, [rsp+38h+30h];			// 目标行映射到源的行数
	mov			rdx, rsi;
	pxor		xmm6, xmm6;			// 存放源的一个像素加权结果
	pxor		xmm5, xmm5;			// 存放中间列的整型和
bgra32_mean_copy_add_y_seg_first_row:
	;// 取得第一行的像素
	movss		xmm1, DWORD PTR[rdx];		// 源像素 0 0 0 argb
	punpcklbw	xmm1, xmm0;			// 0000 0000 0a0r 0g0b
	punpcklwd	xmm1, xmm0;			// 000a 000r 000g 000b
	cvtdq2ps	xmm1, xmm1;
	mulps		xmm1, xmm2;
	add			rdx, r9;		// 下一行的同一列像素
	addps		xmm6, xmm1;			// 加权求和
	;// 判断占用的像素情况
	sub			rax, 02h;
	jb			bgra32_mean_copy_add_y_map_end;			// 只占一个像素
	jnz			loop_bgra32_mean_copy_add_y_mid_row;		// 占多于两个的像素
	jmp			bgra32_mean_copy_add_y_seg_last_row;		// 只占两个像素
loop_bgra32_mean_copy_add_y_mid_row:
	;// 取得中间行的像素
	movd		xmm1, DWORD PTR[rdx];		// 源像素 0 0 0 argb
	punpcklbw	xmm1, xmm0;			// 0000 0000 0a0r 0g0b
	punpcklwd	xmm1, xmm0;			// 000a 000r 000g 000b
	add			rdx, r9;		// 下一行的同一列像素
	paddd		xmm5, xmm1;
	sub			rax, 01h;
	jnz			loop_bgra32_mean_copy_add_y_mid_row;
bgra32_mean_copy_add_y_seg_last_row:
	;// 取得最后一行的像素
	movss		xmm1, DWORD PTR[rdx];		// 源像素 0 0 0 argb
	punpcklbw	xmm1, xmm0;			// 0000 0000 0a0r 0g0b
	punpcklwd	xmm1, xmm0;			// 000a 000r 000g 000b
	cvtdq2ps	xmm1, xmm1;
	mulps		xmm1, xmm3;
	cvtdq2ps	xmm5, xmm5;
	addps		xmm6, xmm5;			// 加权求和
	addps		xmm6, xmm1;			// 加权求和
bgra32_mean_copy_add_y_map_end:
	;// 加权结束
	mulps		xmm6, xmm7;			// 平均
	movaps		[rdi], xmm6;
	add			rsi, 04h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_bgra32_mean_copy_add_y_pixel;
    
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
    pop			rdi
    pop			rsi
    ret
     
     
;-----------------------------------------------------------------------------
; void bgra32_mean_copy_add_y_quick_x64(OUT void* des,			// 临时缓存行（是源行拷贝宽度的四倍）
;									   IN void* src,			// 源拷贝起始行
;									   IN int scw,				// 源拷贝宽度
;									   IN int src_pitch,		// 源行字节数
;									   IN void* pPowerY,		// 目标某行映射的垂直方向上的权重表
;									   IN void* pCoefH,		// 垂直方向上的缩放系数
;									   IN int nMapY);			// 目标一行映射到源的行数
;-----------------------------------------------------------------------------

bgra32_mean_copy_add_y_quick_x64:
	;[rsp+38h] - nMapY
	;[rsp+30h] - pCoefH
	;[rsp+28h] - pPowerY
	;r9 - src_pitch
	;r8 - scw
	;rdx - src
    ;rcx - des 
    
    push		rsi
    push		rdi
    
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 16],xmm7

    mov			rdi, rcx;
	mov			rsi, rdx;
	mov			rcx, r8;
	mov			rdx, [rsp+28h + 30h];
	
	movss		xmm2, DWORD PTR[rdx];		// 垂直方向上的首权重 0 0 0 powStart
	movss		xmm3, DWORD PTR[rdx + 04h];	// 垂直方向上的尾权重 0 0 0 powEnd
	shufps		xmm2, xmm2, 00h;	// power power power power
	shufps		xmm3, xmm3, 00h;	// power power power power
	mov			rax, [rsp+30h + 30h];
	pxor		xmm0, xmm0;			// 0 0 0 0
	movaps		xmm7, [rax];		// 缩放系数 coef coef coef coef
loop_bgra32_mean_copy_add_y_quick_pixel:
	mov			eax, [rsp+38h + 30h];			// 目标行映射到源的行数
	mov			rdx, rsi;
	pxor		xmm6, xmm6;			// 存放源的一个像素加权结果
	pxor		xmm5, xmm5;			// 存放中间列的整型和
bgra32_mean_copy_add_y_quick_seg_first_row:
	;// 取得第一行的像素
	movss		xmm1, DWORD PTR[rdx];		// 源像素 0 0 0 argb
	punpcklbw	xmm1, xmm0;			// 0000 0000 0a0r 0g0b
	punpcklwd	xmm1, xmm0;			// 000a 000r 000g 000b
	cvtdq2ps	xmm1, xmm1;
	mulps		xmm1, xmm2;
	add			rdx, r9;		// 下一行的同一列像素
	addps		xmm6, xmm1;			// 加权求和
	;// 判断占用的像素情况
	sub			eax, 02h;
	jb			bgra32_mean_copy_add_y_quick_map_end;			// 只占一个像素
	jnz			loop_bgra32_mean_copy_add_y_quick_mid_row;		// 占多于两个的像素
	jmp			bgra32_mean_copy_add_y_quick_seg_last_row;		// 只占两个像素
loop_bgra32_mean_copy_add_y_quick_mid_row:
	;// 取得中间行的像素
	movd		xmm1, DWORD PTR[rdx];		// 源像素 0 0 0 argb
	punpcklbw	xmm1, xmm0;			// 0000 0000 0a0r 0g0b
	add			rdx, r9;		// 下一行的同一列像素
	paddusw		xmm5, xmm1;
	sub			eax, 01h;
	jnz			loop_bgra32_mean_copy_add_y_quick_mid_row;
bgra32_mean_copy_add_y_quick_seg_last_row:
	;// 取得最后一行的像素
	movss		xmm1, DWORD PTR[rdx];		// 源像素 0 0 0 argb
	punpcklbw	xmm1, xmm0;			// 0000 0000 0a0r 0g0b
	punpcklwd	xmm1, xmm0;			// 000a 000r 000g 000b
	cvtdq2ps	xmm1, xmm1;
	punpcklwd	xmm5, xmm0;			// 000a 000r 000g 000b
	mulps		xmm1, xmm3;
	cvtdq2ps	xmm5, xmm5;
	addps		xmm6, xmm5;			// 加权求和
	addps		xmm6, xmm1;			// 加权求和
bgra32_mean_copy_add_y_quick_map_end:
	;// 加权结束
	mulps		xmm6, xmm7;			// 平均
	movaps		[rdi], xmm6;
	add			rsi, 04h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_bgra32_mean_copy_add_y_quick_pixel;
     
	movups		xmm6,[rsp];//重置寄存器
	movups		xmm7,[rsp + 16];//重置寄存器
	add			rsp,32

     pop		rdi
     pop		rsi
    ret
     
     
;-----------------------------------------------------------------------------
; void bgra32_mean_copy_add_x_x64(OUT void* des,
;									   IN int dcw,
;									   IN void* src,
;									   IN void* pPowerX,
;									   IN void* pMapX,
;									   IN void* pCoefW);
;-----------------------------------------------------------------------------

bgra32_mean_copy_add_x_x64:
	;[rsp+30h] - pCoefW
	;[rsp+28h] - pMapX
	;r9 - pPowerX
	;r8 - src
	;rdx - dcw
    ;rcx - des 
    
    push		rsi
    push		rdi

	sub			rsp,32
	movups		[rsp],xmm6
	movups		[rsp + 16],xmm7
	
    mov			rdi, rcx;
	mov			rsi, r8;
    mov			rax, [rsp+30h + 30h];
	mov			rcx, rdx;
	movaps		xmm7, [rax];		// 缩放系数 coef coef coef coef
	pxor		xmm0, xmm0;
	
	mov			rdx, [rsp+28h + 30h];
	mov			rax, r9;
loop_bgra32_mean_copy_add_x_pixel:
	push		rcx;				// 保存一行剩余宽度
	pxor		xmm6, xmm6;			// 存放结果
	mov			rcx, [rdx];			// 权重个数
	add			rdx, 04h;
	movss		xmm2, DWORD PTR[rax];		// 水平方向上的列首权重 0 0 0 powStart
	movss		xmm3, DWORD PTR[rax + 04h];	// 水平方向上的列尾权重 0 0 0 powEnd
	add			rax, 08h;
	shufps		xmm2, xmm2, 00h;	// power power power power
	shufps		xmm3, xmm3, 00h;	// power power power power
bgra32_mean_copy_add_x_seg_first_col:
	;// 取得第一列的像素
	movaps		xmm1, [rsi];		// 源的一个像素
	mulps		xmm1, xmm2;
	add			rsi, 10h;
	addps		xmm6, xmm1;			// 加权求和
	;// 判断占用的像素情况
	sub			ecx, 02h;
	jb			bgra32_mean_copy_add_x_map_end;			// 只占一个像素
	jnz			loop_bgra32_mean_copy_add_x_mid_col;		// 占多于两个的像素
	jmp			bgra32_mean_copy_add_x_seg_last_col;		// 只占两个像素
loop_bgra32_mean_copy_add_x_mid_col:
	;// 取得中间列的像素
	movaps		xmm1, [rsi];		// 源的一个像素
	add			rsi, 10h;
	addps		xmm6, xmm1;			// 加权求和
	sub			ecx, 01h;
	jnz			loop_bgra32_mean_copy_add_x_mid_col;
bgra32_mean_copy_add_x_seg_last_col:
	;// 取得最后一列的像素
	movaps		xmm1, [rsi];		// 源的一个像素
	mulps		xmm1, xmm3;
	add			rsi, 10h;
	addps		xmm6, xmm1;			// 加权求和
bgra32_mean_copy_add_x_map_end:
	;// 加权结束
	mulps		xmm6, xmm7;			// 平均 000a 000r 000g 000b
	pop			rcx;				// 取回一行剩余宽度
	cvtps2dq	xmm6, xmm6;
	packssdw	xmm6, xmm6;			// 压缩结果 0a0r 0g0b 0a0r 0g0b
	packuswb	xmm6, xmm6;			// argb argb argb argb
	movd		DWORD PTR[rdi], xmm6;
	sub			rsi, 10h;
	add			rdi, 04h;
	sub			ecx, 01h;
	jnz			loop_bgra32_mean_copy_add_x_pixel;
		
	movups		xmm6,[rsp];
	movups		xmm7,[rsp + 16];
	add			rsp,32
     pop		rdi
     pop		rsi
    ret
     
     

;-----------------------------------------------------------------------------
; void bgra32_replace_alpha_line_x64(IN OUT void *des,			// 目标
;										  IN void* src,			// 一行 8 位的键值
;										  IN int na16,			// 宽度除以 16 的商
;										  IN int nm16);			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

bgra32_replace_alpha_line_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    
    push		rdi
    push		rsi
		
    mov			rdi, rcx;
	mov			rsi, rdx;
	mov			rcx, r8;
	pxor		xmm0, xmm0;
	movdqa		xmm3, oword ptr[gc_dwMaskRGB];
	test		ecx, ecx;
	jnz			loop_bgra32_replace_alpha_line_a16;
	jmp			loop_bgra32_replace_alpha_line_a8;
loop_bgra32_replace_alpha_line_a16:
	movd		xmm1, dword ptr [rsi];	// 键值 0000  0000  0000  k3k2k1k0
	movdqu		xmm2, [rdi];
	punpcklbw	xmm1, xmm0;		// 0 0  0 0  k3 k2  k1 k0
	pand		xmm2, xmm3;
	punpcklwd	xmm1, xmm0;		// k3  k2  k1  k0
	pslld		xmm1, 18h;
	por			xmm1, xmm2;
	movdqu		[rdi], xmm1;
	add			rsi, 04h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_bgra32_replace_alpha_line_a16;
loop_bgra32_replace_alpha_line_a8:
	mov			rcx, r9;
	test		ecx, 00000008h;
	jz			loop_bgra32_replace_alpha_line_a4;
	pinsrw		xmm1, word ptr [rsi], 0;	// 键值 @  @  @  @@ k1k0
	movlps		xmm2, qword ptr [rdi];
	punpcklbw	xmm1, xmm0;		// @  @  @  k1 k0
	pand		xmm2, xmm3;
	punpcklwd	xmm1, xmm0;		// @  @  k1  k0
	pslld		xmm1, 18h;
	por			xmm1, xmm2;
	movlps		qword ptr [rdi], xmm1;
	add			rsi, 02h;
	add			rdi, 08h;
loop_bgra32_replace_alpha_line_a4:
	test		ecx, 00000004h;
	jz			loop_bgra32_replace_alpha_line_end;
	movzx		eax, byte ptr [rsi];
	movd		xmm2, dword ptr [rdi];
	movd		xmm1, eax;
	pand		xmm2, xmm3;
	pslld		xmm1, 18h;
	por			xmm1, xmm2;
	movd		dword ptr [rdi], xmm1;
loop_bgra32_replace_alpha_line_end:
	pop			rsi
	pop			rdi
    ret
    
          

;-----------------------------------------------------------------------------
; void bgra32_key_mul_line_x64(IN OUT void *des,			// 目标
;										  IN void* src,			// 一行 8 位的键值
;										  IN int na16,			// 宽度除以 16 的商
;										  IN int nm16);			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

bgra32_key_mul_line_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    
    push		rsi
    push		rdi
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
	
    mov			rdi, rcx;
	mov			rsi, rdx;
	mov			rcx, r8;
	pxor		xmm0, xmm0;
	movdqa		xmm5, oword ptr[gc_dwMaskDLB];	// 255 Comp
	movdqa		xmm6, oword ptr[gc_dwMaskRGB];
	movdqa		xmm7, oword ptr[gc_dwMaskAlpha];
	test		ecx, ecx;
	jnz			loop_bgra32_key_mul_line_a16;
	jmp			loop_bgra32_key_mul_line_a8;
loop_bgra32_key_mul_line_a16:
	movd		xmm1, dword ptr [rsi];	// 键值 0000  0000  0000  k3k2k1k0
	movdqu		xmm2, [rdi];
	punpcklbw	xmm1, xmm0;		// 0 0  0 0  k3 k2  k1 k0
	movdqa		xmm3, xmm2;
	psrld		xmm2, 18h;		// 000a
	punpcklwd	xmm1, xmm0;		// k3  k2  k1  k0
	pmullw		xmm1, xmm2;		// k * a
	paddd		xmm1, xmm5;		// Comp
	pslld		xmm1, 10h;
	pand		xmm3, xmm6;
	pand		xmm1, xmm7;
	por			xmm1, xmm3;
	movdqu		[rdi], xmm1;
	add			rsi, 04h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_bgra32_key_mul_line_a16;
loop_bgra32_key_mul_line_a8:
	mov			rcx, r9;
	test		ecx, 00000008h;
	jz			loop_bgra32_key_mul_line_a4;
	pinsrw		xmm1, word ptr [rsi], 0;	// 键值 @  @  @  @@ k1k0
	movlps		xmm2, qword ptr [rdi];
	punpcklbw	xmm1, xmm0;		// @  @  @  k1 k0
	movdqa		xmm3, xmm2;
	psrld		xmm2, 18h;		// @  @  000a  000a
	punpcklwd	xmm1, xmm0;		// @  @  k1  k0
	pmullw		xmm1, xmm2;		// k * a
	paddd		xmm1, xmm5;		// Comp
	pslld		xmm1, 10h;
	pand		xmm3, xmm6;
	pand		xmm1, xmm7;
	por			xmm1, xmm3;
	movlps		qword ptr [rdi], xmm1;
	add			rsi, 02h;
	add			rdi, 08h;
loop_bgra32_key_mul_line_a4:
	test		ecx, 00000004h;
	jz			loop_bgra32_key_mul_line_end;
	movzx		eax, byte ptr [rsi];
	movd		xmm2, dword ptr [rdi];
	movd		xmm1, eax;
	movdqa		xmm3, xmm2;
	psrld		xmm2, 18h;		// 000a
	pmullw		xmm2, xmm1;
	paddd		xmm2, xmm5;
	pslld		xmm2, 10h;
	pand		xmm3, xmm6;
	pand		xmm2, xmm7;
	por			xmm2, xmm3;
	movd		dword ptr [rdi], xmm2;
loop_bgra32_key_mul_line_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
    pop			rdi
    pop			rsi
    ret
    
         

;-----------------------------------------------------------------------------
; void bgra32_key_mul_notsrc_line_x64(IN OUT void *des,			// 目标
;										  IN void* src,			// 一行 8 位的键值
;										  IN int na16,			// 宽度除以 16 的商
;										  IN int nm16);			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

bgra32_key_mul_notsrc_line_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    
    push		rsi
    push		rdi
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
	
    mov			rdi, rcx;
	mov			rsi, rdx;
	mov			rcx, r8;
	pxor		xmm0, xmm0;
	movdqa		xmm5, oword ptr[gc_dwMaskDLB];	// 255 Comp
	movdqa		xmm6, oword ptr[gc_dwMaskRGB];
	movdqa		xmm7, oword ptr[gc_dwMaskAlpha];
	test		ecx, ecx;
	jnz			loop_bgra32_key_mul_notsrc_line_a16;
	jmp			loop_bgra32_key_mul_notsrc_line_a8;
loop_bgra32_key_mul_notsrc_line_a16:
	movd		xmm1, dword ptr [rsi];	// 键值 0000  0000  0000  k3k2k1k0
	movdqu		xmm2, [rdi];
	punpcklbw	xmm1, xmm0;		// 0 0  0 0  k3 k2  k1 k0
	movdqa		xmm3, xmm2;
	movdqa		xmm4, xmm5;		// 255
	punpcklwd	xmm1, xmm0;		// k3  k2  k1  k0
	psrld		xmm2, 18h;		// 000a
	psubusw		xmm4, xmm1;		// 1 - k
	pmullw		xmm4, xmm2;		// (1 - k) * a
	paddd		xmm4, xmm5;		// Comp
	pslld		xmm4, 10h;
	pand		xmm3, xmm6;
	pand		xmm4, xmm7;
	por			xmm4, xmm3;
	movdqu		[rdi], xmm4;
	add			rsi, 04h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_bgra32_key_mul_notsrc_line_a16;
loop_bgra32_key_mul_notsrc_line_a8:
	mov			rcx, r9;
	test		ecx, 00000008h;
	jz			loop_bgra32_key_mul_notsrc_line_a4;
	pinsrw		xmm1, word ptr [rsi], 0;	// 键值 @  @  @  @@ k1k0
	movlps		xmm2, qword ptr [rdi];
	punpcklbw	xmm1, xmm0;		// @  @  @  k1 k0
	movdqa		xmm3, xmm2;
	movdqa		xmm4, xmm5;
	punpcklwd	xmm1, xmm0;		// @  @  k1  k0
	psrld		xmm2, 18h;		// @  @  000a  000a
	psubusw		xmm4, xmm1;		// 1 - k
	pmullw		xmm4, xmm2;		// (1 - k) * a
	paddd		xmm4, xmm5;		// Comp
	pslld		xmm4, 10h;
	pand		xmm3, xmm6;
	pand		xmm4, xmm7;
	por			xmm4, xmm3;
	movlps		qword ptr [rdi], xmm4;
	add			rsi, 02h;
	add			rdi, 08h;
loop_bgra32_key_mul_notsrc_line_a4:
	test		ecx, 00000004h;
	jz			loop_bgra32_key_mul_notsrc_line_end;
	movzx		eax, byte ptr [rsi];
	movd		xmm2, dword ptr [rdi];
	movd		xmm1, eax;
	movdqa		xmm3, xmm2;
	movdqa		xmm4, xmm5;
	psrld		xmm2, 18h;		// 000a
	psubusw		xmm4, xmm1;
	pmullw		xmm4, xmm2;
	paddd		xmm4, xmm5;
	pslld		xmm4, 10h;
	pand		xmm3, xmm6;
	pand		xmm4, xmm7;
	por			xmm4, xmm3;
	movd		dword ptr [rdi], xmm4;
loop_bgra32_key_mul_notsrc_line_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
	pop			rdi
	pop			rsi
    ret
    
         

;-----------------------------------------------------------------------------
; void bgra32_key_mul_notdes_line_x64(IN OUT void *des,			// 目标
;										  IN void* src,			// 一行 8 位的键值
;										  IN int na16,			// 宽度除以 16 的商
;										  IN int nm16);			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

bgra32_key_mul_notdes_line_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    
    push		rsi
    push		rdi
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
	
    mov			rdi, rcx;
	mov			rsi, rdx;
	mov			rcx, r8;
	pxor		xmm0, xmm0;
	movdqa		xmm5, oword ptr[gc_dwMaskDLB];	// 255 Comp
	movdqa		xmm6, oword ptr[gc_dwMaskRGB];
	movdqa		xmm7, oword ptr[gc_dwMaskAlpha];
	test		ecx, ecx;
	jnz			loop_bgra32_key_mul_notdes_line_a16;
	jmp			loop_bgra32_key_mul_notdes_line_a8;
loop_bgra32_key_mul_notdes_line_a16:
	movd		xmm1, dword ptr [rsi];	// 键值 0000  0000  0000  k3k2k1k0
	movdqu		xmm2, [rdi];
	punpcklbw	xmm1, xmm0;		// 0 0  0 0  k3 k2  k1 k0
	movdqa		xmm4, xmm5;		// 255
	movdqa		xmm3, xmm2;
	psrld		xmm2, 18h;		// 000a
	punpcklwd	xmm1, xmm0;		// k3  k2  k1  k0
	psubusw		xmm4, xmm2;		// 1 - a
	pmullw		xmm4, xmm1;		// (1 - a) * k
	paddd		xmm4, xmm5;		// Comp
	pslld		xmm4, 10h;
	pand		xmm3, xmm6;
	pand		xmm4, xmm7;
	por			xmm4, xmm3;
	movdqu		[rdi], xmm4;
	add			rsi, 04h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_bgra32_key_mul_notdes_line_a16;
loop_bgra32_key_mul_notdes_line_a8:
	mov			rcx, r9;
	test		ecx, 00000008h;
	jz			loop_bgra32_key_mul_notdes_line_a4;
	pinsrw		xmm1, word ptr [rsi], 0;	// 键值 @  @  @  @@ k1k0
	movlps		xmm2, qword ptr [rdi];
	punpcklbw	xmm1, xmm0;		// @  @  @  k1 k0
	movdqa		xmm4, xmm5;
	movdqa		xmm3, xmm2;
	psrld		xmm2, 18h;		// @  @  000a  000a
	punpcklwd	xmm1, xmm0;		// @  @  k1  k0
	psubusw		xmm4, xmm2;		// 1 - a
	pmullw		xmm4, xmm1;		// (1 - a) * k
	paddd		xmm4, xmm5;		// Comp
	pslld		xmm4, 10h;
	pand		xmm3, xmm6;
	pand		xmm4, xmm7;
	por			xmm4, xmm3;
	movlps		qword ptr [rdi], xmm4;
	add			rsi, 02h;
	add			rdi, 08h;
loop_bgra32_key_mul_notdes_line_a4:
	test		ecx, 00000004h;
	jz			loop_bgra32_key_mul_notdes_line_end;
	movzx		eax, byte ptr [rsi];
	movd		xmm2, dword ptr [rdi];
	movd		xmm1, eax;
	movdqa		xmm4, xmm5;
	movdqa		xmm3, xmm2;
	psrld		xmm2, 18h;		// 000a
	psubusw		xmm4, xmm2;
	pmullw		xmm4, xmm1;
	paddd		xmm4, xmm5;
	pslld		xmm4, 10h;
	pand		xmm3, xmm6;
	pand		xmm4, xmm7;
	por			xmm4, xmm3;
	movd		dword ptr [rdi], xmm4;
loop_bgra32_key_mul_notdes_line_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
	pop			rdi
	pop			rsi
    ret
         

;-----------------------------------------------------------------------------
; void bgra32_key_mix_line_x64(IN OUT void *des,			// 目标
;										  IN void* src,			// 一行 8 位的键值
;										  IN int na16,			// 宽度除以 16 的商
;										  IN int nm16);			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

bgra32_key_mix_line_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    
    push		rsi
    push		rdi
	sub			rsp,10h
	movups		[rsp],xmm7
	
    mov			rdi, rcx;
	mov			rsi, rdx;
	mov			rcx, r8;
	pxor		xmm0, xmm0;
	movdqa		xmm5, oword ptr[gc_dwMaskDLB];	// 255 Comp
	movdqa		xmm7, oword ptr[gc_dwMaskAlpha];
	test		ecx, ecx;
	jnz			loop_bgra32_key_mix_line_a16;
	jmp			loop_bgra32_key_mix_line_a8;
loop_bgra32_key_mix_line_a16:
	movd		xmm1, dword ptr [rsi];	// 键值 0000  0000  0000  k3k2k1k0
	movdqu		xmm2, [rdi];
	punpcklbw	xmm1, xmm0;		// 0 0  0 0  k3 k2  k1 k0
	movdqa		xmm3, xmm2;
	movdqa		xmm4, xmm5;		// 255
	psrld		xmm2, 18h;		// 000a
	punpcklwd	xmm1, xmm0;		// k3  k2  k1  k0
	psubd		xmm4, xmm2;		// 255 - a
	pmullw		xmm4, xmm1;		// k * (1 - a)
	paddd		xmm4, xmm5;		// Comp
	pslld		xmm4, 10h;
	pand		xmm4, xmm7;
	paddusb		xmm4, xmm3;
	movdqu		[rdi], xmm4;
	add			rsi, 04h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_bgra32_key_mix_line_a16;
loop_bgra32_key_mix_line_a8:
	mov			rcx, r9;
	test		ecx, 08h;
	jz			loop_bgra32_key_mix_line_a4;
	pinsrw		xmm1, word ptr [rsi], 0;	// 键值 @  @  @  @@ k1k0
	movlps		xmm2, qword ptr [rdi];
	punpcklbw	xmm1, xmm0;		// @  @  @  k1 k0
	movdqa		xmm3, xmm2;
	movdqa		xmm4, xmm5;		// 255
	psrld		xmm2, 18h;		// @  @  000a  000a
	punpcklwd	xmm1, xmm0;		// @  @  k1  k0
	psubd		xmm4, xmm2;		// 255 - a
	pmullw		xmm4, xmm1;		// k * (1 - a)
	paddd		xmm4, xmm5;		// Comp
	pslld		xmm4, 10h;
	pand		xmm4, xmm7;
	paddusb		xmm4, xmm3;
	movlps		qword ptr [rdi], xmm4;
	add			rsi, 02h;
	add			rdi, 08h;
loop_bgra32_key_mix_line_a4:
	test		ecx, 00000004h;
	jz			loop_bgra32_key_mix_line_end;
	movzx		eax, byte ptr [rsi];
	movd		xmm2, dword ptr [rdi];
	movd		xmm1, eax;
	movdqa		xmm4, xmm5;
	movdqa		xmm3, xmm2;
	psrld		xmm2, 18h;		// 000a
	psubd		xmm4, xmm2;
	pmullw		xmm4, xmm1;
	paddd		xmm4, xmm5;
	pslld		xmm4, 10h;
	pand		xmm4, xmm7;
	paddusb		xmm4, xmm3;
	movd		dword ptr [rdi], xmm4;
loop_bgra32_key_mix_line_end:
	movups		xmm7,[rsp]
	add			rsp,10h
	pop			rdi
	pop			rsi
    ret
         

;-----------------------------------------------------------------------------
; void bgra32_key_mix_notsrc_line_x64(IN OUT void *des,			// 目标
;										  IN void* src,			// 一行 8 位的键值
;										  IN int na16,			// 宽度除以 16 的商
;										  IN int nm16);			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

bgra32_key_mix_notsrc_line_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    
    push		rsi
    push		rdi
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
	
    mov			rdi, rcx;
	mov			rsi, rdx;
	mov			rcx, r8;
	pxor		xmm0, xmm0;
	movdqa		xmm5, oword ptr[gc_dwMaskDLB];	// 255 Comp
	movdqa		xmm6, oword ptr[gc_dwMaskRGB];
	movdqa		xmm7, oword ptr[gc_dwMaskAlpha];
	test		ecx, ecx;
	jnz			loop_bgra32_key_mix_notsrc_line_a16;
	jmp			loop_bgra32_key_mix_notsrc_line_a8;
loop_bgra32_key_mix_notsrc_line_a16:
	movd		xmm1, dword ptr [rsi];	// 键值 0000  0000  0000  k3k2k1k0
	movdqu		xmm2, [rdi];
	punpcklbw	xmm1, xmm0;		// 0 0  0 0  k3 k2  k1 k0
	movdqa		xmm3, xmm2;
	movdqa		xmm4, xmm5;		// 255
	psrld		xmm2, 18h;		// 000a
	punpcklwd	xmm1, xmm0;		// k3  k2  k1  k0
	psubd		xmm4, xmm2;		// 255 - a
	movdqa		xmm2, xmm5;
	pmullw		xmm4, xmm1;		// k * (1 - a)
	paddd		xmm4, xmm5;		// Comp
	psrld		xmm4, 08h;
	psubd		xmm2, xmm4;		// 1 - k * (1 - a) = (1 - k) + a - (1 - k) * a
	pand		xmm3, xmm6;
	pslld		xmm2, 18h;
	por			xmm2, xmm3;
	movdqu		[rdi], xmm2;
	add			rsi, 04h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_bgra32_key_mix_notsrc_line_a16;
loop_bgra32_key_mix_notsrc_line_a8:
	mov			rcx, r9;
	test		ecx, 08h;
	jz			loop_bgra32_key_mix_notsrc_line_a4;
	pinsrw		xmm1, word ptr [rsi], 0;	// 键值 @  @  @  @@ k1k0
	movlps		xmm2, qword ptr [rdi];
	punpcklbw	xmm1, xmm0;		// @  @  @  k1 k0
	movdqa		xmm3, xmm2;
	movdqa		xmm4, xmm5;		// 255
	psrld		xmm2, 18h;		// @  @  000a  000a
	punpcklwd	xmm1, xmm0;		// @  @  k1  k0
	psubd		xmm4, xmm2;		// 255 - a
	movdqa		xmm2, xmm5;
	pmullw		xmm4, xmm1;		// k * (1 - a)
	paddd		xmm4, xmm5;		// Comp
	psrld		xmm4, 08h;
	psubd		xmm2, xmm4;
	pand		xmm3, xmm6;
	pslld		xmm2, 18h;
	por			xmm2, xmm3;
	movlps		qword ptr [rdi], xmm2;
	add			rsi, 02h;
	add			rdi, 08h;
loop_bgra32_key_mix_notsrc_line_a4:
	test		ecx, 00000004h;
	jz			loop_bgra32_key_mix_notsrc_line_end;
	movzx		eax, byte ptr [rsi];
	movd		xmm2, dword ptr [rdi];
	movd		xmm1, eax;
	movdqa		xmm4, xmm5;
	movdqa		xmm3, xmm2;
	psrld		xmm2, 18h;		// 000a
	psubd		xmm4, xmm2;
	pmullw		xmm4, xmm1;
	paddd		xmm4, xmm5;
	psrld		xmm4, 08h;
	psubd		xmm5, xmm4;
	pslld		xmm5, 18h;
	pand		xmm3, xmm6;
	por			xmm3, xmm5;
	movd		dword ptr [rdi], xmm3;
loop_bgra32_key_mix_notsrc_line_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
	pop			rdi
	pop			rsi
    ret
         

;-----------------------------------------------------------------------------
; void bgra32_key_sat_sub_line_x64(IN OUT void *des,			// 目标
;										  IN void* src,			// 一行 8 位的键值
;										  IN int na16,			// 宽度除以 16 的商
;										  IN int nm16);			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

bgra32_key_sat_sub_line_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    
    push		rsi
    push		rdi
    mov			rdi, rcx;
	mov			rsi, rdx;
	mov			rcx, r8;
	test		ecx, ecx;
	jnz			loop_bgra32_key_sat_sub_line_a16;
	jmp			loop_bgra32_key_sat_sub_line_a8;
loop_bgra32_key_sat_sub_line_a16:
	movd		xmm1, dword ptr [rsi];	// 键值 0000  0000  0000  k3k2k1k0
	movdqu		xmm2, [rdi];
	pxor		xmm0, xmm0;
	pxor		xmm3, xmm3;
	punpcklbw	xmm0, xmm1;
	punpcklbw	xmm3, xmm0;		// k3 000  k2 000  k1 000  k0 000
	psubusb		xmm2, xmm3;
	movdqu		[rdi], xmm2;
	add			rsi, 04h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_bgra32_key_sat_sub_line_a16;
loop_bgra32_key_sat_sub_line_a8:
	mov			rcx, r9;
	test		ecx, 08h;
	jz			loop_bgra32_key_sat_sub_line_a4;
	pinsrw		xmm1, word ptr [rsi], 0;	// 键值 @  @  @  @@ k1k0
	movlps		xmm2, qword ptr [rdi];
	pxor		xmm0, xmm0;
	pxor		xmm3, xmm3;
	punpcklbw	xmm0, xmm1;		// @  @  @  k1 k0
	punpcklbw	xmm3, xmm0;		// @  @  k1 000  k0 000
	psubusb		xmm2, xmm3;
	movlps		qword ptr [rdi], xmm2;
	add			rsi, 02h;
	add			rdi, 08h;
loop_bgra32_key_sat_sub_line_a4:
	test		ecx, 00000004h;
	jz			loop_bgra32_key_sat_sub_line_end;
	movzx		eax, byte ptr [rsi];
	movd		xmm2, dword ptr [rdi];
	movd		xmm1, eax;
	pslld		xmm1, 18h;
	psubusb		xmm2, xmm1;
	movd		dword ptr [rdi], xmm2;
loop_bgra32_key_sat_sub_line_end:
	pop			rdi
	pop			rsi
    ret
         

;-----------------------------------------------------------------------------
; void keybuf_mul_char_line_x64(IN OUT void* des,		// 一行 8 位键目标
;									 IN char alpha,		// 标量值
;									 IN int na16,			// 宽度除以 16 的商
;									 IN int nm16);			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

keybuf_mul_char_line_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - alpha
    ;rcx - des 
    
    push		rdi
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
	
    movzx		rax, dx;		// 000a
	mov			rdi, rcx;
	mov			rcx, r8;
	movd		xmm6, eax;		// 0  0  0  000a
	pxor		xmm0, xmm0;
	pshuflw		xmm6, xmm6, 00h;	// 0  0  a a  a a
	movdqa		xmm7, oword ptr[gc_dwMaskWLB];	// 255 Comp
	movlhps		xmm6, xmm6;		// a a  a a  a a  a a
	test		ecx, ecx;
	jnz			loop_keybuf_mul_char_line_a16;
	jmp			loop_keybuf_mul_char_line_a8;
loop_keybuf_mul_char_line_a16:
	movdqu		xmm1, [rdi];	// k15 ... k0
	movhlps		xmm2, xmm1;		// @  k15 ... k8
	punpcklbw	xmm1, xmm0;		// k7 k6 k5 k4 k3 k2 k1 k0
	punpcklbw	xmm2, xmm0;		// k15 k14 k13 k12 k11 k10 k9 k8
	pmullw		xmm1, xmm6;		// k * a
	pmullw		xmm2, xmm6;
	paddusw		xmm1, xmm7;		// Comp
	paddusw		xmm2, xmm7;
	psrlw		xmm1, 08h;
	psrlw		xmm2, 08h;
	packuswb	xmm1, xmm2;
	movdqu		[rdi], xmm1;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_keybuf_mul_char_line_a16;
loop_keybuf_mul_char_line_a8:
	mov			rcx, r9;
	test		ecx, 08h;
	jz			loop_keybuf_mul_char_line_a4;
	movlps		xmm1, qword ptr [rdi];	// @  k7 ... k0
	punpcklbw	xmm1, xmm0;		// k7 k6 k5 k4 k3 k2 k1 k0
	pmullw		xmm1, xmm6;		// k * a
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	packuswb	xmm1, xmm0;
	movlps		qword ptr [rdi], xmm1;
	add			rdi, 08h;
loop_keybuf_mul_char_line_a4:
	test		ecx, 04h;
	jz			loop_keybuf_mul_char_line_a2;
	movd		xmm1, dword ptr [rdi];	// 0  0  0  k3 k2 k1 k0
	punpcklbw	xmm1, xmm0;		// 0  k3 k2 k1 k0
	pmullw		xmm1, xmm6;		// k * a
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	packuswb	xmm1, xmm0;
	movd		dword ptr [rdi], xmm1;
	add			rdi, 04h;
loop_keybuf_mul_char_line_a2:
	test		ecx, 02h;
	jz			loop_keybuf_mul_char_line_a1;
	pinsrw		xmm1, word ptr [rdi], 0;	// @  @  @  @ k1k0
	punpcklbw	xmm1, xmm0;		// @  @  @  k1 k0
	pmullw		xmm1, xmm6;		// k * a
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	packuswb	xmm1, xmm0;
	movd		eax, xmm1;
	mov			word ptr [rdi], ax;
	add			rdi, 02h;
loop_keybuf_mul_char_line_a1:
	test		ecx, 01h;
	jz			loop_keybuf_mul_char_line_end;
	movzx		eax, byte ptr [rdi];	// 000k
	movd		xmm1, eax;
	pmullw		xmm1, xmm6;
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	movd		eax, xmm1;
	mov			byte ptr [rdi], al;
loop_keybuf_mul_char_line_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
	pop			rdi
    ret
         

;-----------------------------------------------------------------------------
; void keybuf_mul_char_line_notdes_x64(IN OUT void* des,		// 一行 8 位键目标
;									 IN char alpha,		// 标量值
;									 IN int na16,			// 宽度除以 16 的商
;									 IN int nm16);			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

keybuf_mul_char_line_notdes_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - alpha
    ;rcx - des 
    
    push		rdi
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
	
    movzx		rax, dx;		// 000a
	mov			rdi, rcx;
	mov			rcx, r8;
	movd		xmm6, eax;		// 0  0  0  000a
	pxor		xmm0, xmm0;
	pshuflw		xmm6, xmm6, 00h;	// 0  0  a a  a a
	movdqa		xmm7, oword ptr[gc_dwMaskWLB];	// 255 Comp
	movdqa		xmm5, oword ptr[gc_nComp_N1];
	movlhps		xmm6, xmm6;		// a a  a a  a a  a a
	test		ecx, ecx;
	jnz			loop_keybuf_mul_char_line_notdes_a16;
	jmp			loop_keybuf_mul_char_line_notdes_a8;
loop_keybuf_mul_char_line_notdes_a16:
	movdqu		xmm3, [rdi];	// k15 ... k0
	movdqa		xmm1, xmm5;		// 255
	psubb		xmm1, xmm3;		// 1 - k
	movhlps		xmm2, xmm1;		// @  k15 ... k8
	punpcklbw	xmm1, xmm0;		// k7 k6 k5 k4 k3 k2 k1 k0
	punpcklbw	xmm2, xmm0;		// k15 k14 k13 k12 k11 k10 k9 k8
	pmullw		xmm1, xmm6;		// (1 - k) * a
	pmullw		xmm2, xmm6;
	paddusw		xmm1, xmm7;		// Comp
	paddusw		xmm2, xmm7;
	psrlw		xmm1, 08h;
	psrlw		xmm2, 08h;
	packuswb	xmm1, xmm2;
	movdqu		[rdi], xmm1;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_keybuf_mul_char_line_notdes_a16;
loop_keybuf_mul_char_line_notdes_a8:
	mov			rcx, r9;
	test		ecx, 08h;
	jz			loop_keybuf_mul_char_line_notdes_a4;
	movlps		xmm3, qword ptr [rdi];	// @  k7 ... k0
	movdqa		xmm1, xmm5;		// 255
	psubb		xmm1, xmm3;		// 1 - k
	punpcklbw	xmm1, xmm0;		// k7 k6 k5 k4 k3 k2 k1 k0
	pmullw		xmm1, xmm6;		// (1 - k) * a
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	packuswb	xmm1, xmm0;
	movlps		qword ptr [rdi], xmm1;
	add			rdi, 08h;
loop_keybuf_mul_char_line_notdes_a4:
	test		ecx, 04h;
	jz			loop_keybuf_mul_char_line_notdes_a2;
	movd		xmm3, dword ptr [rdi];	// 0  0  0  k3 k2 k1 k0
	movdqa		xmm1, xmm5;		// 255
	psubb		xmm1, xmm3;		// 1 - k
	punpcklbw	xmm1, xmm0;		// 0  k3 k2 k1 k0
	pmullw		xmm1, xmm6;		// (1 - k) * a
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	packuswb	xmm1, xmm0;
	movd		dword ptr [rdi], xmm1;
	add			rdi, 04h;
loop_keybuf_mul_char_line_notdes_a2:
	test		ecx, 02h;
	jz			loop_keybuf_mul_char_line_notdes_a1;
	pinsrw		xmm3, word ptr [rdi], 0;	// @  @  @  @ k1k0
	movdqa		xmm1, xmm5;		// 255
	psubb		xmm1, xmm3;		// 1 - k
	punpcklbw	xmm1, xmm0;		// @  @  @  k1 k0
	pmullw		xmm1, xmm6;		// (1 - k) * a
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	packuswb	xmm1, xmm0;
	movd		eax, xmm1;
	mov			word ptr [rdi], ax;
	add			rdi, 02h;
loop_keybuf_mul_char_line_notdes_a1:
	test		ecx, 01h;
	jz			loop_keybuf_mul_char_line_notdes_end;
	mov			ecx, 000000ffh;
	movzx		eax, byte ptr [rdi];	// 000k
	sub			ecx, eax;
	movd		xmm1, ecx;
	pmullw		xmm1, xmm6;
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	movd		eax, xmm1;
	mov			byte ptr [rdi], al;
loop_keybuf_mul_char_line_notdes_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
	pop			rdi
    ret
         

;-----------------------------------------------------------------------------
; void keybuf_mix_char_line_x64(IN OUT void* des,		// 一行 8 位键目标
;									 IN char alpha,		// 标量值
;									 IN int na16,			// 宽度除以 16 的商
;									 IN int nm16);			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

keybuf_mix_char_line_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - alpha
    ;rcx - des 
    
    push		rdi
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
	
    movzx		rax, dx;		// 000a
	mov			rdi, rcx;
	mov			rcx, r8;
	movd		xmm5, eax;		// 0  0  0  000a
	pxor		xmm0, xmm0;
	pshuflw		xmm5, xmm5, 00h;	// 0  0  a a  a a
	movdqa		xmm7, oword ptr[gc_dwMaskWLB];	// 255 Comp
	movdqa		xmm6, xmm7;
	movlhps		xmm5, xmm5;		// a a  a a  a a  a a
	psubw		xmm6, xmm5;		// 1 - a
	test		ecx, ecx;
	jnz			loop_keybuf_mix_char_line_a16;
	jmp			loop_keybuf_mix_char_line_a8;
loop_keybuf_mix_char_line_a16:
	movdqu		xmm1, [rdi];	// k15 ... k0
	movhlps		xmm2, xmm1;		// @  k15 ... k8
	punpcklbw	xmm1, xmm0;		// k7 k6 k5 k4 k3 k2 k1 k0
	punpcklbw	xmm2, xmm0;		// k15 k14 k13 k12 k11 k10 k9 k8
	pmullw		xmm1, xmm6;		// k * (1 - a)
	pmullw		xmm2, xmm6;
	paddusw		xmm1, xmm7;		// Comp
	paddusw		xmm2, xmm7;
	psrlw		xmm1, 08h;
	psrlw		xmm2, 08h;
	paddusb		xmm1, xmm5;		// k * (1 - a) + a
	paddusb		xmm2, xmm5;
	packuswb	xmm1, xmm2;
	movdqu		[rdi], xmm1;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_keybuf_mix_char_line_a16;
loop_keybuf_mix_char_line_a8:
	mov			rcx, r9;
	test		ecx, 08h;
	jz			loop_keybuf_mix_char_line_a4;
	movlps		xmm1, qword ptr [rdi];	// @  k7 ... k0
	punpcklbw	xmm1, xmm0;		// k7 k6 k5 k4 k3 k2 k1 k0
	pmullw		xmm1, xmm6;		// k * (1 - a)
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	paddusb		xmm1, xmm5;		// k * (1 - a) + a
	packuswb	xmm1, xmm0;
	movlps		qword ptr [rdi], xmm1;
	add			rdi, 08h;
loop_keybuf_mix_char_line_a4:
	test		ecx, 04h;
	jz			loop_keybuf_mix_char_line_a2;
	movd		xmm1, dword ptr [rdi];	// 0  0  0  k3 k2 k1 k0
	punpcklbw	xmm1, xmm0;		// 0  k3 k2 k1 k0
	pmullw		xmm1, xmm6;		// k * (1 - a)
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	paddusb		xmm1, xmm5;		// k * (1 - a) + a
	packuswb	xmm1, xmm0;
	movd		dword ptr [rdi], xmm1;
	add			rdi, 04h;
loop_keybuf_mix_char_line_a2:
	test		ecx, 02h;
	jz			loop_keybuf_mix_char_line_a1;
	pinsrw		xmm1, word ptr [rdi], 0;	// @  @  @  @ k1k0
	punpcklbw	xmm1, xmm0;		// @  @  @  k1 k0
	pmullw		xmm1, xmm6;		// k * (1 - a)
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	paddusb		xmm1, xmm5;		// k * (1 - a) + a
	packuswb	xmm1, xmm0;
	movd		eax, xmm1;
	mov			word ptr [rdi], ax;
	add			rdi, 02h;
loop_keybuf_mix_char_line_a1:
	test		ecx, 01h;
	jz			loop_keybuf_mix_char_line_end;
	movzx		eax, byte ptr [rdi];	// 000k
	movd		xmm1, eax;
	pmullw		xmm1, xmm6;
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	paddusb		xmm1, xmm5;		// k * (1 - a) + a
	movd		eax, xmm1;
	mov			byte ptr [rdi], al;
loop_keybuf_mix_char_line_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
	pop			rdi
    ret
         

;-----------------------------------------------------------------------------
; void keybuf_mul_buf_line_x64(IN OUT void *des,			// 目标
;										  IN void* src,			// 一行 8 位的键值
;										  IN int na16,			// 宽度除以 16 的商
;										  IN int nm16);			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

keybuf_mul_buf_line_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    
    push		rsi
    push		rdi
	sub			rsp,10h
	movups		[rsp],xmm7
	
    mov			rdi, rcx;
	mov			rsi, rdx;
	mov			rcx, r8;
	pxor		xmm0, xmm0;
	movdqa		xmm7, oword ptr[gc_dwMaskWLB];	// 255 Comp
	test		ecx, ecx;
	jnz			loop_keybuf_mul_buf_line_a16;
	jmp			loop_keybuf_mul_buf_line_a8;
loop_keybuf_mul_buf_line_a16:
	movdqu		xmm1, [rdi];	// dk15 ... dk0
	movdqu		xmm2, [rsi];	// sk15 ... sk0
	movhlps		xmm3, xmm1;		// @  dk15 ... dk8
	movhlps		xmm4, xmm2;		// @  sk15 ... sk8
	punpcklbw	xmm1, xmm0;		// des  k7 k6 k5 k4 k3 k2 k1 k0
	punpcklbw	xmm2, xmm0;		// src  k7 k6 k5 k4 k3 k2 k1 k0
	punpcklbw	xmm3, xmm0;		// des  k15 k14 k13 k12 k11 k10 k9 k8
	punpcklbw	xmm4, xmm0;		// src  k15 k14 k13 k12 k11 k10 k9 k8
	pmullw		xmm1, xmm2;		// src * des
	pmullw		xmm3, xmm4;
	paddusw		xmm1, xmm7;		// Comp
	paddusw		xmm3, xmm7;
	psrlw		xmm1, 08h;
	psrlw		xmm3, 08h;
	packuswb	xmm1, xmm3;
	movdqu		[rdi], xmm1;
	add			rsi, 10h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_keybuf_mul_buf_line_a16;
loop_keybuf_mul_buf_line_a8:
	mov			rcx, r9;
	test		ecx, 08h;
	jz			loop_keybuf_mul_buf_line_a4;
	movlps		xmm1, qword ptr [rdi];	// des  @  k7 ... k0
	movlps		xmm2, qword ptr [rsi];	// src  @  k7 ... k0
	punpcklbw	xmm1, xmm0;		// des  k7 k6 k5 k4 k3 k2 k1 k0
	punpcklbw	xmm2, xmm0;		// src  k7 k6 k5 k4 k3 k2 k1 k0
	pmullw		xmm1, xmm2;		// src * des
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	packuswb	xmm1, xmm0;
	movlps		qword ptr [rdi], xmm1;
	add			rsi, 08h;
	add			rdi, 08h;
loop_keybuf_mul_buf_line_a4:
	test		ecx, 04h;
	jz			loop_keybuf_mul_buf_line_a2;
	movd		xmm1, dword ptr [rdi];	// des  0  0  0  k3 k2 k1 k0
	movd		xmm2, dword ptr [rsi];	// src  0  0  0  k3 k2 k1 k0
	punpcklbw	xmm1, xmm0;		// des  0  k3 k2 k1 k0
	punpcklbw	xmm2, xmm0;		// src  0  k3 k2 k1 k0
	pmullw		xmm1, xmm2;		// src * des
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	packuswb	xmm1, xmm0;
	movd		dword ptr [rdi], xmm1;
	add			rsi, 04h;
	add			rdi, 04h;
loop_keybuf_mul_buf_line_a2:
	test		ecx, 02h;
	jz			loop_keybuf_mul_buf_line_a1;
	pinsrw		xmm1, word ptr [rdi], 0;	// des  @  @  @  @ k1k0
	pinsrw		xmm2, word ptr [rsi], 0;	// src  @  @  @  @ k1k0
	punpcklbw	xmm1, xmm0;		// des  @  @  @  k1 k0
	punpcklbw	xmm2, xmm0;		// src  @  @  @  k1 k0
	pmullw		xmm1, xmm2;		// src * des
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	packuswb	xmm1, xmm0;
	movd		eax, xmm1;
	mov			word ptr [rdi], ax;
	add			rsi, 02h;
	add			rdi, 02h;
loop_keybuf_mul_buf_line_a1:
	test		ecx, 01h;
	jz			loop_keybuf_mul_buf_line_end;
	movzx		eax, byte ptr [rdi];	// des  000k
	movzx		ecx, byte ptr [rsi];	// src  000k
	movd		xmm1, eax;
	movd		xmm2, ecx;
	pmullw		xmm1, xmm2;
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	movd		eax, xmm1;
	mov			byte ptr [rdi], al;
loop_keybuf_mul_buf_line_end:
	movups		xmm7,[rsp]
	add			rsp,10h
	pop			rdi
	pop			rsi
    ret
         

;-----------------------------------------------------------------------------
; void keybuf_or_buf_line_x64(IN OUT void *des,			// 目标
;										  IN void* src,			// 一行 8 位的键值
;										  IN int na16,			// 宽度除以 16 的商
;										  IN int nm16);			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

keybuf_or_buf_line_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    
    push		rsi
    push		rdi
	sub			rsp,10h
	movups		[rsp],xmm7

    mov			rdi, rcx;
	mov			rsi, rdx;
	mov			rcx, r8;
	pxor		xmm0, xmm0;
	movdqa		xmm7, oword ptr[gc_dwMaskWLB];	// 255 Comp
	test		ecx, ecx;
	jnz			loop_keybuf_or_buf_line_a16;
	jmp			loop_keybuf_or_buf_line_a8;
loop_keybuf_or_buf_line_a16:
	movdqu		xmm1, [rdi];	// dk15 ... dk0
	movdqu		xmm2, [rsi];	// sk15 ... sk0
	orps		xmm1,xmm2;
	movdqu		[rdi], xmm1;
	add			rsi, 10h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_keybuf_or_buf_line_a16;
loop_keybuf_or_buf_line_a8:
	mov			rcx, r9;
	test		ecx, 08h;
	jz			loop_keybuf_or_buf_line_a4;
	movlps		xmm1, qword ptr [rdi];	// des  @  k7 ... k0
	movlps		xmm2, qword ptr [rsi];	// src  @  k7 ... k0
	orps		xmm1,xmm2;
	movlps		qword ptr [rdi], xmm1;
	add			rsi, 08h;
	add			rdi, 08h;
loop_keybuf_or_buf_line_a4:
	test		ecx, 04h;
	jz			loop_keybuf_or_buf_line_a2;
	movd		xmm1, dword ptr [rdi];	// des  0  0  0  k3 k2 k1 k0
	movd		xmm2, dword ptr [rsi];	// src  0  0  0  k3 k2 k1 k0
	movd		dword ptr [rdi], xmm1;
	orps		xmm1,xmm2;
	add			rsi, 04h;
	add			rdi, 04h;
loop_keybuf_or_buf_line_a2:
	test		ecx, 02h;
	jz			loop_keybuf_or_buf_line_a1;
	pinsrw		xmm1, word ptr [rdi], 0;	// des  @  @  @  @ k1k0
	pinsrw		xmm2, word ptr [rsi], 0;	// src  @  @  @  @ k1k0
	orps		xmm1,xmm2;
	movd		eax, xmm1;
	mov			word ptr [rdi], ax;
	add			rsi, 02h;
	add			rdi, 02h;
loop_keybuf_or_buf_line_a1:
	test		ecx, 01h;
	jz			loop_keybuf_or_buf_line_end;
	movzx		eax, byte ptr [rdi];	// des  000k
	movzx		ecx, byte ptr [rsi];	// src  000k
	movd		xmm1, eax;
	movd		xmm2, ecx;
	orps		xmm1,xmm2;
	movd		eax, xmm1;
	mov			byte ptr [rdi], al;
loop_keybuf_or_buf_line_end:
	movups		xmm7,[rsp]
	add			rsp,10h
	pop			rdi
	pop			rsi
    ret
         

;-----------------------------------------------------------------------------
; void keybuf_mul_buf_line_notsrc_x64(IN OUT void *des,			// 目标
;										  IN void* src,			// 一行 8 位的键值
;										  IN int na16,			// 宽度除以 16 的商
;										  IN int nm16);			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

keybuf_mul_buf_line_notsrc_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
   
	push		rsi
    push		rdi
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
	
    mov			rdi, rcx;
	mov			rsi, rdx;
	mov			rcx, r8;
	pxor		xmm0, xmm0;
	movdqa		xmm6, oword ptr[gc_nComp_N1];	// 255
	movdqa		xmm7, oword ptr[gc_dwMaskWLB];	// 255 Comp
	test		ecx, ecx;
	jnz			loop_keybuf_mul_buf_line_notsrc_a16;
	jmp			loop_keybuf_mul_buf_line_notsrc_a8;
loop_keybuf_mul_buf_line_notsrc_a16:
	movdqa		xmm2, xmm6;		// 255
	movdqu		xmm5, [rsi];	// sk15 ... sk0
	movdqu		xmm1, [rdi];	// dk15 ... dk0
	psubb		xmm2, xmm5;		// 1 - src
	movhlps		xmm3, xmm1;		// @  dk15 ... dk8
	movhlps		xmm4, xmm2;		// @  sk15 ... sk8
	punpcklbw	xmm1, xmm0;		// des  k7 k6 k5 k4 k3 k2 k1 k0
	punpcklbw	xmm2, xmm0;		// src  k7 k6 k5 k4 k3 k2 k1 k0
	punpcklbw	xmm3, xmm0;		// des  k15 k14 k13 k12 k11 k10 k9 k8
	punpcklbw	xmm4, xmm0;		// src  k15 k14 k13 k12 k11 k10 k9 k8
	pmullw		xmm1, xmm2;		// src * des
	pmullw		xmm3, xmm4;
	paddusw		xmm1, xmm7;		// Comp
	paddusw		xmm3, xmm7;
	psrlw		xmm1, 08h;
	psrlw		xmm3, 08h;
	packuswb	xmm1, xmm3;
	movdqu		[rdi], xmm1;
	add			rsi, 10h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_keybuf_mul_buf_line_notsrc_a16;
loop_keybuf_mul_buf_line_notsrc_a8:
	mov			rcx, r9;
	test		ecx, 08h;
	jz			loop_keybuf_mul_buf_line_notsrc_a4;
	movdqa		xmm2, xmm6;		// 255
	movlps		xmm5, qword ptr [rsi];	// src  @  k7 ... k0
	movlps		xmm1, qword ptr [rdi];	// des  @  k7 ... k0
	psubb		xmm2, xmm5;		// 1 - src
	punpcklbw	xmm1, xmm0;		// des  k7 k6 k5 k4 k3 k2 k1 k0
	punpcklbw	xmm2, xmm0;		// src  k7 k6 k5 k4 k3 k2 k1 k0
	pmullw		xmm1, xmm2;		// src * des
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	packuswb	xmm1, xmm0;
	movlps		qword ptr [rdi], xmm1;
	add			rsi, 08h;
	add			rdi, 08h;
loop_keybuf_mul_buf_line_notsrc_a4:
	test		ecx, 04h;
	jz			loop_keybuf_mul_buf_line_notsrc_a2;
	movdqa		xmm2, xmm6;		// 255
	movd		xmm5, dword ptr [rsi];	// src  0  0  0  k3 k2 k1 k0
	movd		xmm1, dword ptr [rdi];	// des  0  0  0  k3 k2 k1 k0
	psubb		xmm2, xmm5;		// 1 - src
	punpcklbw	xmm1, xmm0;		// des  0  k3 k2 k1 k0
	punpcklbw	xmm2, xmm0;		// src  0  k3 k2 k1 k0
	pmullw		xmm1, xmm2;		// src * des
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	packuswb	xmm1, xmm0;
	movd		dword ptr [rdi], xmm1;
	add			rsi, 04h;
	add			rdi, 04h;
loop_keybuf_mul_buf_line_notsrc_a2:
	test		ecx, 02h;
	jz			loop_keybuf_mul_buf_line_notsrc_a1;
	movdqa		xmm2, xmm6;		// 255
	pinsrw		xmm5, word ptr [rsi], 0;	// src  @  @  @  @ k1k0
	pinsrw		xmm1, word ptr [rdi], 0;	// des  @  @  @  @ k1k0
	psubb		xmm2, xmm5;		// 1 - src
	punpcklbw	xmm1, xmm0;		// des  @  @  @  k1 k0
	punpcklbw	xmm2, xmm0;		// src  @  @  @  k1 k0
	pmullw		xmm1, xmm2;		// src * des
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	packuswb	xmm1, xmm0;
	movd		eax, xmm1;
	mov			word ptr [rdi], ax;
	add			rsi, 02h;
	add			rdi, 02h;
loop_keybuf_mul_buf_line_notsrc_a1:
	test		ecx, 01h;
	jz			loop_keybuf_mul_buf_line_notsrc_end;
	mov			ecx, 000000ffh;	// 255
	movzx		edx, byte ptr [rsi];	// src  000k
	movzx		eax, byte ptr [rdi];	// des  000k
	sub			ecx, edx;			// 1 - src
	movd		xmm1, eax;
	movd		xmm2, ecx;
	pmullw		xmm1, xmm2;
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	movd		eax, xmm1;
	mov			byte ptr [rdi], al;
loop_keybuf_mul_buf_line_notsrc_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
	pop			rdi
	pop			rsi
    ret
         

;-----------------------------------------------------------------------------
; void keybuf_mul_buf_line_notdes_x64(IN OUT void *des,			// 目标
;										  IN void* src,			// 一行 8 位的键值
;										  IN int na16,			// 宽度除以 16 的商
;										  IN int nm16);			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

keybuf_mul_buf_line_notdes_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    
    push		rsi
    push		rdi
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
	
    mov			rdi, rcx;
	mov			rsi, rdx;
	mov			rcx, r8;
	pxor		xmm0, xmm0;
	movdqa		xmm6, oword ptr[gc_nComp_N1];	// 255
	movdqa		xmm7, oword ptr[gc_dwMaskWLB];	// 255 Comp
	test		ecx, ecx;
	jnz			loop_keybuf_mul_buf_line_notdes_a16;
	jmp			loop_keybuf_mul_buf_line_notdes_a8;
loop_keybuf_mul_buf_line_notdes_a16:
	movdqa		xmm1, xmm6;		// 255
	movdqu		xmm5, [rdi];	// dk15 ... dk0
	movdqu		xmm2, [rsi];	// sk15 ... sk0
	psubb		xmm1, xmm5;		// 1 - des
	movhlps		xmm4, xmm2;		// @  sk15 ... sk8
	movhlps		xmm3, xmm1;		// @  dk15 ... dk8
	punpcklbw	xmm1, xmm0;		// des  k7 k6 k5 k4 k3 k2 k1 k0
	punpcklbw	xmm2, xmm0;		// src  k7 k6 k5 k4 k3 k2 k1 k0
	punpcklbw	xmm3, xmm0;		// des  k15 k14 k13 k12 k11 k10 k9 k8
	punpcklbw	xmm4, xmm0;		// src  k15 k14 k13 k12 k11 k10 k9 k8
	pmullw		xmm1, xmm2;		// src * des
	pmullw		xmm3, xmm4;
	paddusw		xmm1, xmm7;		// Comp
	paddusw		xmm3, xmm7;
	psrlw		xmm1, 08h;
	psrlw		xmm3, 08h;
	packuswb	xmm1, xmm3;
	movdqu		[rdi], xmm1;
	add			rsi, 10h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_keybuf_mul_buf_line_notdes_a16;
loop_keybuf_mul_buf_line_notdes_a8:
	mov			rcx, r9;
	test		ecx, 08h;
	jz			loop_keybuf_mul_buf_line_notdes_a4;
	movdqa		xmm1, xmm6;		// 255
	movlps		xmm5, qword ptr [rdi];	// des  @  k7 ... k0
	movlps		xmm2, qword ptr [rsi];	// src  @  k7 ... k0
	psubb		xmm1, xmm5;		// 1 - des
	punpcklbw	xmm2, xmm0;		// src  k7 k6 k5 k4 k3 k2 k1 k0
	punpcklbw	xmm1, xmm0;		// des  k7 k6 k5 k4 k3 k2 k1 k0
	pmullw		xmm1, xmm2;		// src * des
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	packuswb	xmm1, xmm0;
	movlps		qword ptr [rdi], xmm1;
	add			rsi, 08h;
	add			rdi, 08h;
loop_keybuf_mul_buf_line_notdes_a4:
	test		ecx, 04h;
	jz			loop_keybuf_mul_buf_line_notdes_a2;
	movdqa		xmm1, xmm6;		// 255
	movd		xmm5, dword ptr [rdi];	// des  0  0  0  k3 k2 k1 k0
	movd		xmm2, dword ptr [rsi];	// src  0  0  0  k3 k2 k1 k0
	psubb		xmm1, xmm5;		// 1 - des
	punpcklbw	xmm2, xmm0;		// src  0  k3 k2 k1 k0
	punpcklbw	xmm1, xmm0;		// des  0  k3 k2 k1 k0
	pmullw		xmm1, xmm2;		// src * des
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	packuswb	xmm1, xmm0;
	movd		dword ptr [rdi], xmm1;
	add			rsi, 04h;
	add			rdi, 04h;
loop_keybuf_mul_buf_line_notdes_a2:
	test		ecx, 02h;
	jz			loop_keybuf_mul_buf_line_notdes_a1;
	movdqa		xmm1, xmm6;		// 255
	pinsrw		xmm5, word ptr [rdi], 0;	// des  @  @  @  @ k1k0
	pinsrw		xmm2, word ptr [rsi], 0;	// src  @  @  @  @ k1k0
	psubb		xmm1, xmm5;		// 1 - des
	punpcklbw	xmm2, xmm0;		// src  @  @  @  k1 k0
	punpcklbw	xmm1, xmm0;		// des  @  @  @  k1 k0
	pmullw		xmm1, xmm2;		// src * des
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	packuswb	xmm1, xmm0;
	movd		eax, xmm1;
	mov			word ptr [rdi], ax;
	add			rsi, 02h;
	add			rdi, 02h;
loop_keybuf_mul_buf_line_notdes_a1:
	test		ecx, 01h;
	jz			loop_keybuf_mul_buf_line_notdes_end;
	mov			eax, 000000ffh;	// 255
	movzx		edx, byte ptr [rdi];	// des  000k
	movzx		ecx, byte ptr [rsi];	// src  000k
	sub			eax, edx;			// 1 - des
	movd		xmm2, ecx;
	movd		xmm1, eax;
	pmullw		xmm1, xmm2;
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	movd		eax, xmm1;
	mov			byte ptr [rdi], al;
loop_keybuf_mul_buf_line_notdes_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
	pop			rdi
	pop			rsi
    ret
         

;-----------------------------------------------------------------------------
; void keybuf_mix_buf_line_x64(IN OUT void *des,			// 目标
;										  IN void* src,			// 一行 8 位的键值
;										  IN int na16,			// 宽度除以 16 的商
;										  IN int nm16);			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

keybuf_mix_buf_line_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    
    push		rsi
    push		rdi
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
	
    mov			rdi, rcx;
	mov			rsi, rdx;
	mov			rcx, r8;
	pxor		xmm0, xmm0;
	movdqa		xmm6, oword ptr[gc_nComp_N1];	// 255
	movdqa		xmm7, oword ptr[gc_dwMaskWLB];	// 255 Comp
	test		ecx, ecx;
	jnz			loop_keybuf_mix_buf_line_a16;
	jmp			loop_keybuf_mix_buf_line_a8;
loop_keybuf_mix_buf_line_a16:
	movdqa		xmm2, xmm6;		// 255
	movdqu		xmm5, [rsi];	// sk15 ... sk0
	movdqu		xmm1, [rdi];	// dk15 ... dk0
	psubusb		xmm2, xmm5;		// 1 - src
	movhlps		xmm3, xmm1;		// @  dk15 ... dk8
	movhlps		xmm4, xmm2;		// @  sk15 ... sk8
	punpcklbw	xmm1, xmm0;		// des  k7 k6 k5 k4 k3 k2 k1 k0
	punpcklbw	xmm2, xmm0;		// src  k7 k6 k5 k4 k3 k2 k1 k0
	punpcklbw	xmm3, xmm0;		// des  k15 k14 k13 k12 k11 k10 k9 k8
	punpcklbw	xmm4, xmm0;		// src  k15 k14 k13 k12 k11 k10 k9 k8
	pmullw		xmm1, xmm2;		// (1 - src) * des
	pmullw		xmm3, xmm4;
	paddusw		xmm1, xmm7;		// Comp
	paddusw		xmm3, xmm7;
	psrlw		xmm1, 08h;
	psrlw		xmm3, 08h;
	packuswb	xmm1, xmm3;
	paddusb		xmm1, xmm5;		// (1 - src) * des + src
	movdqu		[rdi], xmm1;
	add			rsi, 10h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_keybuf_mix_buf_line_a16;
loop_keybuf_mix_buf_line_a8:
	mov			rcx, r9;
	test		ecx, 08h;
	jz			loop_keybuf_mix_buf_line_a4;
	movdqa		xmm2, xmm6;		// 255
	movlps		xmm5, qword ptr [rsi];	// src  @  k7 ... k0
	movlps		xmm1, qword ptr [rdi];	// des  @  k7 ... k0
	psubusb		xmm2, xmm5;		// 1 - src
	punpcklbw	xmm1, xmm0;		// des  k7 k6 k5 k4 k3 k2 k1 k0
	punpcklbw	xmm2, xmm0;		// src  k7 k6 k5 k4 k3 k2 k1 k0
	pmullw		xmm1, xmm2;		// (1 - src) * des
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	packuswb	xmm1, xmm0;
	paddusb		xmm1, xmm5;		// (1 - src) * des + src
	movlps		qword ptr [rdi], xmm1;
	add			rsi, 08h;
	add			rdi, 08h;
loop_keybuf_mix_buf_line_a4:
	test		ecx, 04h;
	jz			loop_keybuf_mix_buf_line_a2;
	movdqa		xmm2, xmm6;		// 255
	movd		xmm5, dword ptr [rsi];	// src  0  0  0  k3 k2 k1 k0
	movd		xmm1, dword ptr [rdi];	// des  0  0  0  k3 k2 k1 k0
	psubusb		xmm2, xmm5;		// 1 - src
	punpcklbw	xmm1, xmm0;		// des  0  k3 k2 k1 k0
	punpcklbw	xmm2, xmm0;		// src  0  k3 k2 k1 k0
	pmullw		xmm1, xmm2;		// (1 - src) * des
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	packuswb	xmm1, xmm0;
	paddusb		xmm1, xmm5;		// (1 - src) * des + src
	movd		dword ptr [rdi], xmm1;
	add			rsi, 04h;
	add			rdi, 04h;
loop_keybuf_mix_buf_line_a2:
	test		ecx, 02h;
	jz			loop_keybuf_mix_buf_line_a1;
	movdqa		xmm2, xmm6;		// 255
	pinsrw		xmm5, word ptr [rsi], 0;	// src  @  @  @  @ k1k0
	pinsrw		xmm1, word ptr [rdi], 0;	// des  @  @  @  @ k1k0
	psubusb		xmm2, xmm5;		// 1 - src
	punpcklbw	xmm1, xmm0;		// des  @  @  @  k1 k0
	punpcklbw	xmm2, xmm0;		// src  @  @  @  k1 k0
	pmullw		xmm1, xmm2;		// (1 - src) * des
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	packuswb	xmm1, xmm0;
	paddusb		xmm1, xmm5;		// (1 - src) * des + src
	movd		eax, xmm1;
	mov			word ptr [rdi], ax;
	add			rsi, 02h;
	add			rdi, 02h;
loop_keybuf_mix_buf_line_a1:
	test		ecx, 01h;
	jz			loop_keybuf_mix_buf_line_end;
	movzx		ecx, byte ptr [rsi];	// src  000k
	movzx		eax, byte ptr [rdi];	// des  000k
	movdqa		xmm2, xmm7;
	movd		xmm5, ecx;
	movd		xmm1, eax;
	psubusb		xmm2, xmm5;		// 1 - src
	pmullw		xmm1, xmm2;		// (1 - src) * des
	paddusw		xmm1, xmm7;		// Comp
	psrlw		xmm1, 08h;
	paddusb		xmm1, xmm5;		// (1 - src) * des + src
	movd		eax, xmm1;
	mov			byte ptr [rdi], al;
loop_keybuf_mix_buf_line_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
	pop			rdi
	pop			rsi
    ret
         

;-----------------------------------------------------------------------------
; void keybuf_mix_buf_line_notsrc_x64(IN OUT void *des,			// 目标
;										  IN void* src,			// 一行 8 位的键值
;										  IN int na16,			// 宽度除以 16 的商
;										  IN int nm16);			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

keybuf_mix_buf_line_notsrc_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    
    push		rsi
    push		rdi
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
	
    mov			rdi, rcx;
	mov			rsi, rdx;
	mov			rcx, r8;
	pxor		xmm0, xmm0;
	movdqa		xmm6, oword ptr[gc_nComp_N1];	// 255
	movdqa		xmm7, oword ptr[gc_dwMaskWHB];	// ff00h
	test		ecx, ecx;
	jnz			loop_keybuf_mix_buf_line_notsrc_a16;
	jmp			loop_keybuf_mix_buf_line_notsrc_a8;
loop_keybuf_mix_buf_line_notsrc_a16:
	movdqa		xmm1, xmm6;			// 255
	movdqu		xmm5, [rdi];	// dk15 ... dk0
	movdqu		xmm2, [rsi];	// sk15 ... sk0
	psubusb		xmm1, xmm5;		// 1 - des
	movhlps		xmm4, xmm2;		// @  sk15 ... sk8
	movhlps		xmm3, xmm1;		// @  dk15 ... dk8
	punpcklbw	xmm2, xmm0;		// src  k7 k6 k5 k4 k3 k2 k1 k0
	punpcklbw	xmm1, xmm0;		// des  k7 k6 k5 k4 k3 k2 k1 k0
	punpcklbw	xmm4, xmm0;		// src  k15 k14 k13 k12 k11 k10 k9 k8
	punpcklbw	xmm3, xmm0;		// des  k15 k14 k13 k12 k11 k10 k9 k8
	pmullw		xmm2, xmm1;		// src * (1 - des)
	pmullw		xmm4, xmm3;
	movdqa		xmm1, xmm7;		// 已补偿
	movdqa		xmm3, xmm7;
	psubusw		xmm1, xmm2;		// 1 - src * (1 - des) = (1 - src) + des - (1 - src) * des
	psubusw		xmm3, xmm4;
	psrlw		xmm1, 08h;
	psrlw		xmm3, 08h;
	packuswb	xmm1, xmm3;
	movdqu		[rdi], xmm1;
	add			rsi, 10h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_keybuf_mix_buf_line_notsrc_a16;
loop_keybuf_mix_buf_line_notsrc_a8:
	mov			rcx, r9;
	test		ecx, 08h;
	jz			loop_keybuf_mix_buf_line_notsrc_a4;
	movdqa		xmm1, xmm6;		// 255
	movdqa		xmm3, xmm7;
	movlps		xmm5, qword ptr [rdi];	// des  @  k7 ... k0
	movlps		xmm2, qword ptr [rsi];	// src  @  k7 ... k0
	psubusb		xmm1, xmm5;		// 1 - des
	punpcklbw	xmm2, xmm0;		// src  k7 k6 k5 k4 k3 k2 k1 k0
	punpcklbw	xmm1, xmm0;		// des  k7 k6 k5 k4 k3 k2 k1 k0
	pmullw		xmm2, xmm1;		// (1 - des) * src
	psubusw		xmm3, xmm2;		// 1 - src * (1 - des) = (1 - src) + des - (1 - src) * des
	psrlw		xmm3, 08h;
	packuswb	xmm3, xmm0;
	movlps		qword ptr [rdi], xmm3;
	add			rsi, 08h;
	add			rdi, 08h;
loop_keybuf_mix_buf_line_notsrc_a4:
	test		ecx, 04h;
	jz			loop_keybuf_mix_buf_line_notsrc_a2;
	movdqa		xmm1, xmm6;		// 255
	movdqa		xmm3, xmm7;
	movd		xmm5, dword ptr [rdi];	// des  0  0  0  k3 k2 k1 k0
	movd		xmm2, dword ptr [rsi];	// src  0  0  0  k3 k2 k1 k0
	psubusb		xmm1, xmm5;		// 1 - des
	punpcklbw	xmm2, xmm0;		// src  0  k3 k2 k1 k0
	punpcklbw	xmm1, xmm0;		// des  0  k3 k2 k1 k0
	pmullw		xmm2, xmm1;		// (1 - des) * src
	psubusw		xmm3, xmm2;		// 1 - src * (1 - des) = (1 - src) + des - (1 - src) * des
	psrlw		xmm3, 08h;
	packuswb	xmm3, xmm0;
	movd		dword ptr [rdi], xmm3;
	add			rsi, 04h;
	add			rdi, 04h;
loop_keybuf_mix_buf_line_notsrc_a2:
	test		ecx, 02h;
	jz			loop_keybuf_mix_buf_line_notsrc_a1;
	movdqa		xmm1, xmm6;		// 255
	movdqa		xmm3, xmm7;
	pinsrw		xmm5, word ptr [rdi], 0;	// des  @  @  @  @ k1k0
	pinsrw		xmm2, word ptr [rsi], 0;	// src  @  @  @  @ k1k0
	psubusb		xmm1, xmm5;		// 1 - des
	punpcklbw	xmm2, xmm0;		// src  @  @  @  k1 k0
	punpcklbw	xmm1, xmm0;		// des  @  @  @  k1 k0
	pmullw		xmm2, xmm1;		// (1 - des) * src
	psubusw		xmm3, xmm2;		// 1 - src * (1 - des) = (1 - src) + des - (1 - src) * des
	psrlw		xmm3, 08h;
	packuswb	xmm3, xmm0;
	movd		eax, xmm3;
	mov			word ptr [rdi], ax;
	add			rsi, 02h;
	add			rdi, 02h;
loop_keybuf_mix_buf_line_notsrc_a1:
	test		ecx, 01h;
	jz			loop_keybuf_mix_buf_line_notsrc_end;
	mov			eax, 000000ffh;
	movzx		edx, byte ptr [rdi];	// des  000k
	movzx		ecx, byte ptr [rsi];	// src  000k
	sub			eax, edx;		// 1 - des
	movd		xmm2, ecx;
	movd		xmm1, eax;
	pmullw		xmm2, xmm1;		// (1 - des) * src
	psubusw		xmm7, xmm2;		// 1 - src * (1 - des) = (1 - src) + des - (1 - src) * des
	psrlw		xmm7, 08h;
	movd		eax, xmm7;
	mov			byte ptr [rdi], al;
loop_keybuf_mix_buf_line_notsrc_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
	pop			rdi
	pop			rsi
    ret
         

;-----------------------------------------------------------------------------
; void keybuf_sat_sub_buf_line_x64(IN OUT void *des,			// 目标
;										  IN void* src,			// 一行 8 位的键值
;										  IN int na16,			// 宽度除以 16 的商
;										  IN int nm16);			// 宽度除以 16 的余数
;-----------------------------------------------------------------------------

keybuf_sat_sub_buf_line_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    
    push		rsi
    push		rdi
    mov			rdi, rcx;
	mov			rsi, rdx;
	mov			rcx, r8;
	test		ecx, ecx;
	jnz			loop_keybuf_sat_sub_buf_line_a16;
	jmp			loop_keybuf_sat_sub_buf_line_a8;
loop_keybuf_sat_sub_buf_line_a16:
	movdqu		xmm1, [rdi];	// dk15 ... dk0
	movdqu		xmm2, [rsi];	// sk15 ... sk0
	psubusb		xmm1, xmm2;		// sat(dk - sk)
	movdqu		[rdi], xmm1;
	add			rsi, 10h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_keybuf_sat_sub_buf_line_a16;
loop_keybuf_sat_sub_buf_line_a8:
	mov			rcx, r9;
	test		ecx, 08h;
	jz			loop_keybuf_sat_sub_buf_line_a4;
	movlps		xmm1, qword ptr [rdi];	// des  @  k7 ... k0
	movlps		xmm2, qword ptr [rsi];	// src  @  k7 ... k0
	psubusb		xmm1, xmm2;		// sat(dk - sk)
	movlps		qword ptr [rdi], xmm1;
	add			rsi, 08h;
	add			rdi, 08h;
loop_keybuf_sat_sub_buf_line_a4:
	test		ecx, 04h;
	jz			loop_keybuf_sat_sub_buf_line_a2;
	movd		xmm1, dword ptr [rdi];	// des  0  0  0  k3 k2 k1 k0
	movd		xmm2, dword ptr [rsi];	// src  0  0  0  k3 k2 k1 k0
	psubusb		xmm1, xmm2;		// sat(dk - sk)
	movd		dword ptr [rdi], xmm1;
	add			rsi, 04h;
	add			rdi, 04h;
loop_keybuf_sat_sub_buf_line_a2:
	test		ecx, 02h;
	jz			loop_keybuf_sat_sub_buf_line_a1;
	pinsrw		xmm1, word ptr [rdi], 0;	// des  @  @  @  @ k1k0
	pinsrw		xmm2, word ptr [rsi], 0;	// src  @  @  @  @ k1k0
	psubusb		xmm1, xmm2;		// sat(dk - sk)
	movd		eax, xmm1;
	mov			word ptr [rdi], ax;
	add			rsi, 02h;
	add			rdi, 02h;
loop_keybuf_sat_sub_buf_line_a1:
	test		ecx, 01h;
	jz			loop_keybuf_sat_sub_buf_line_end;
	movzx		eax, byte ptr [rdi];	// des  000k
	movzx		ecx, byte ptr [rsi];	// src  000k
	movd		xmm1, eax;
	movd		xmm2, ecx;
	psubusb		xmm1, xmm2;		// sat(dk - sk)
	movd		eax, xmm1;
	mov			byte ptr [rdi], al;
loop_keybuf_sat_sub_buf_line_end:
	pop			rdi
	pop			rsi
    ret
         

;-----------------------------------------------------------------------------
; void keybuf_bl_copy_line_x64(OUT void* des,		// 目标拷贝行
;						 IN int na4,		// 目标拷贝行的宽度除以 4 的商
;						 IN int nm4,		// 目标拷贝行的宽度除以 4 的余数
;						 IN void* src,		// 源拷贝行
;						 IN int src_pitch,	// 源图一行的字节数
;						 IN void* pPowerX,	// 目标映射到源的 X 方向上的列权重
;						 IN void* pMapX,	// 目标映射到源的 X 方向上的列索引
;						 IN DWORD powerY)	// 目标行映射到源行的行权重
;-----------------------------------------------------------------------------

keybuf_bl_copy_line_x64:
	;[rsp+40h] - powerY
	;[rsp+38h] - pMapX
	;[rsp+30h] - pPowerX
	;[rsp+28h] - src_pitch
	;r9 - src
	;r8 - nm4
	;rdx - na4
        ;rcx - des 

	push		rsi
	push		rdi
	push		rbx    
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7

    mov			rax, r9;
	mov			ebx, [rsp+28h+38h];
	mov 		rdi, rcx;			// 目标行
	mov 		rcx, rdx;			// 一次处理四个像素
	mov 		rdx, [rsp+38h+38h];			// X 方向上的索引表
	
	mov 		rsi, [rsp+30h];		// X 方向上的权重表
	movd		xmm7, DWORD PTR[rsp+40h+38h];		// 行权重 0000 0000 0000 0y0y
	movd		xmm5, eax;			// 0  0  0  src
	movd		xmm4, ebx;			// 0  0  0  src_pitch
	movdqa		xmm6, oword ptr[gc_nComp_N1];	// ffffffff ffffffff ffffffff ffffffff
	shufps		xmm7, xmm7, 00h;	// 0y0y 0y0y 0y0y 0y0y
	shufps		xmm5, xmm5, 00h;	// src  src  src  src
	shufps		xmm4, xmm4, 00h;	// src_pitch
	psubusw		xmm6, xmm7;			// 1 - y
	paddd		xmm4, xmm5;			// src_pitch + src
	test		ecx, ecx;
	jnz			loop_keybuf_bl_copy_line_a4;
	jmp			loop_keybuf_bl_copy_line_m2;
loop_keybuf_bl_copy_line_a4:
	movdqu		xmm0, oword ptr [rdx];	// map3 map2 map1 map0
	movdqa		xmm1, xmm4;
	paddd		xmm1, xmm0;				// src + map + src_pitch
	paddd		xmm0, xmm5;				// src + map
	movd		ebx, xmm1;
	movd		eax, xmm0;
	pinsrw		xmm3, word ptr [rbx], 0;	// @ @  @ @  @ @  @ 0*1
	pinsrw		xmm2, word ptr [rax], 0;	// @ @  @ @  @ @  @ 0*0
	shufps		xmm0, xmm0, 39h;		// src0 src3 src2 src1
	shufps		xmm1, xmm1, 39h;
	movd		eax, xmm0;
	movd		ebx, xmm1;
	pinsrw		xmm2, word ptr [rax], 1;	// @ @  @ @  @ @  1*0 0*0
	pinsrw		xmm3, word ptr [rbx], 1;	// @ @  @ @  @ @  1*1 0*1
	shufps		xmm0, xmm0, 39h;		// src1 src0 src3 src2
	shufps		xmm1, xmm1, 39h;
	movd		eax, xmm0;
	movd		ebx, xmm1;
	pinsrw		xmm2, word ptr [rax], 2;	// @ @  @ @  @ 2*0  1*0 0*0
	pinsrw		xmm3, word ptr [rbx], 2;	// @ @  @ @  @ 2*1  1*1 0*1
	shufps		xmm0, xmm0, 39h;		// src2 src1 src0 src3
	shufps		xmm1, xmm1, 39h;
	movd		eax, xmm0;
	movd		ebx, xmm1;
	pinsrw		xmm2, word ptr [rax], 3;	// @ @  @ @  3*0 2*0  1*0 0*0
	pinsrw		xmm3, word ptr [rbx], 3;	// @ @  @ @  3*1 2*1  1*1 0*1
	pxor		xmm0, xmm0;
	movdqu		xmm1, [rsi];		// 目标行上的映射权重 (1-x3)x3 (1-x2)x2 (1-x1)x1 (1-x0)x0
	punpcklbw	xmm2, xmm0;			// 310 300  210 200  110 100  010 000
	punpcklbw	xmm3, xmm0;			// 311 301  211 201  111 101  011 001
	psllw		xmm2, 06h;			// 放大 6 位
	psllw		xmm3, 06h;
	paddw		xmm2, oword ptr[gc_nRoundComp_1];	// 误差补偿
	paddw		xmm3, oword ptr[gc_nRoundComp_1];	// 误差补偿
	pmulhuw		xmm2, xmm7;			// ((S@0 + comp) * y) >> 16
	pmulhuw		xmm3, xmm6;			// ((S@1 + comp) * (65535 - y)) >> 16
	paddusw		xmm2, oword ptr[gc_nRoundComp_1];
	paddusw		xmm2, xmm3;			// S@0 * y + S@1 * (1 - y)
	pmulhuw		xmm2, xmm1;			// S1@ * (1 - x)	S0@ * x
	movdqa		xmm3, xmm2;
	psrld		xmm3, 10h;			// S1@ * (1 - x)
	paddusw		xmm3, xmm2;			// @ D3  @ D2  @ D1  @ D0 （放大 6 位）
	psrlw		xmm3, 06h;			// @ D3  @ D2  @ D1  @ D0
	pand		xmm3, oword ptr[gc_dwMaskDLW];	// D3  D2  D1  D0
	packssdw	xmm3, xmm3;			// D3 D2 D1 D0 D3 D2 D1 D0
	packuswb	xmm3, xmm3;			// D3D2D1D0
	movd		dword ptr [rdi], xmm3;
	add			edx, 10h;
	add			rsi, 10h;
	add			rdi, 04h;
	sub			ecx, 01h;
	jnz			loop_keybuf_bl_copy_line_a4;
loop_keybuf_bl_copy_line_m2:
	mov			rcx, r8;
	test		ecx, 02h;			// 测试是否有后几个像素
	jz			loop_keybuf_bl_copy_line_m1;
	movlps		xmm0, qword ptr [rdx];	// @ @ map1 map0
	movdqa		xmm1, xmm4;
	paddd		xmm1, xmm0;				// src + map + src_pitch
	paddd		xmm0, xmm5;				// src + map
	movd		ebx, xmm1;
	movd		eax, xmm0;
	pinsrw		xmm3, word ptr [rbx], 0;	// @ @  @ @  @ @  @ 0*1
	pinsrw		xmm2, word ptr [rax], 0;	// @ @  @ @  @ @  @ 0*0
	shufps		xmm0, xmm0, 39h;		// src0 @ @ src1
	shufps		xmm1, xmm1, 39h;
	movd		eax, xmm0;
	movd		ebx, xmm1;
	pinsrw		xmm2, word ptr [rax], 1;	// @ @  @ @  @ @  1*0 0*0
	pinsrw		xmm3, word ptr [rbx], 1;	// @ @  @ @  @ @  1*1 0*1
	pxor		xmm0, xmm0;
	movlps		xmm1, QWORD PTR[rsi];		// 目标行上的映射权重 @ @ (1-x1)x1 (1-x0)x0
	punpcklbw	xmm2, xmm0;			// @ @  @ @  110 100  010 000
	punpcklbw	xmm3, xmm0;			// @ @  @ @  111 101  011 001
	psllw		xmm2, 06h;			// 放大 6 位
	psllw		xmm3, 06h;
	paddw		xmm2, oword ptr[gc_nRoundComp_1];	// 误差补偿
	paddw		xmm3, oword ptr[gc_nRoundComp_1];	// 误差补偿
	pmulhuw		xmm2, xmm7;			// ((S@0 + comp) * y) >> 16
	pmulhuw		xmm3, xmm6;			// ((S@1 + comp) * (65535 - y)) >> 16
	paddusw		xmm2, oword ptr[gc_nRoundComp_1];
	paddusw		xmm2, xmm3;			// S@0 * y + S@1 * (1 - y)
	pmulhuw		xmm2, xmm1;			// S1@ * (1 - x)	S0@ * x
	movdqa		xmm3, xmm2;
	psrld		xmm3, 10h;			// S1@ * (1 - x)
	paddusw		xmm3, xmm2;			// @ @  @ @  @ D1  @ D0 （放大 6 位）
	psrlw		xmm3, 06h;			// @ @  @ @  @ D1  @ D0
	pand		xmm3, oword ptr[gc_dwMaskDLW];	// @  @  D1  D0
	packssdw	xmm3, xmm3;			// @ @ D1 D0 @ @ D1 D0
	packuswb	xmm3, xmm3;			// @@D1D0
	movd		eax, xmm3;
	mov			word ptr [rdi], ax;
	add			edx, 08h;
	add			rsi, 08h;
	add			rdi, 02h;
loop_keybuf_bl_copy_line_m1:
	test		ecx, 01h;			// 测试是否有后几个像素
	jz			loop_keybuf_bl_copy_line_end;
	mov			eax, dword ptr [rdx];	// map0
	mov			ebx, [rsp+28h+38h];
	mov			rcx, r9;
	add			ebx, eax;
	add			ebx, ecx;
	add			eax, ecx;				// src0
	pinsrw		xmm3, word ptr [rbx], 0;	// @ @  @ @  @ @  @ 0*1
	pinsrw		xmm2, word ptr [rax], 0;	// @ @  @ @  @ @  @ 0*0
	pxor		xmm0, xmm0;
	movd		xmm1, DWORD PTR[rsi];		// 目标行上的映射权重 @ @ @ (1-x0)x0
	punpcklbw	xmm2, xmm0;			// @ @  @ @  @ @  010 000
	punpcklbw	xmm3, xmm0;			// @ @  @ @  @ @  011 001
	psllw		xmm2, 06h;			// 放大 6 位
	psllw		xmm3, 06h;
	paddw		xmm2, oword ptr[gc_nRoundComp_1];	// 误差补偿
	paddw		xmm3, oword ptr[gc_nRoundComp_1];	// 误差补偿
	pmulhuw		xmm2, xmm7;			// ((S@0 + comp) * y) >> 16
	pmulhuw		xmm3, xmm6;			// ((S@1 + comp) * (65535 - y)) >> 16
	paddusw		xmm2, oword ptr[gc_nRoundComp_1];
	paddusw		xmm2, xmm3;			// S@0 * y + S@1 * (1 - y)
	pmulhuw		xmm2, xmm1;			// S1@ * (1 - x)	S0@ * x
	movdqa		xmm3, xmm2;
	psrld		xmm3, 10h;			// S1@ * (1 - x)
	paddusw		xmm3, xmm2;			// @ @  @ @  @ @  @ D0 （放大 6 位）
	psrlw		xmm3, 06h;			// @ @  @ @  @ @  @ D0
	movd		eax, xmm3;
	mov			byte ptr [rdi], al;
loop_keybuf_bl_copy_line_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
	pop		rbx
	pop		rdi
	pop		rsi
    ret
    
    
    
;-----------------------------------------------------------------------------
; void math_serial_perfect_square_x64(OUT void* des,		// 结果行
;								IN float fStart,	// 序列起始数
;								IN float fStep,		// 等差
;								IN float fGiven,	// 特定数
;								IN int na16,		// 行字节宽度除以 16 的商与余数（一次做四个元素）
;								IN int nm16)
;-----------------------------------------------------------------------------

math_serial_perfect_square_x64:
	;[rsp+30h] - nm16
	;[rsp+28h] - na16
	;xmm3 - fGiven
	;xmm2 - fStep
	;xmm1 - fStart
    ;rcx - des 
    
    push		rdi
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
	
    mov			rdi, rcx;
    mov			rcx, [rsp+28h+28h];
	movss		xmm7, xmm2;
	movss		xmm0, xmm3;
	;movss		xmm1, xmm1;
	movdqa		xmm6, xmm7;
	shufps		xmm0, xmm0, 00h;		// ag ag ag ag
	shufps		xmm1, xmm1, 00h;		// a0 a0 a0 a0
	shufps		xmm6, xmm6, 15h;		// d 0 0 0
	shufps		xmm7, xmm7, 00h;		// d d d d
	movhlps		xmm5, xmm6;				// @ @ d 0
	addps		xmm7, xmm7;				// 2d 2d 2d 2d
	addps		xmm6, xmm7;				// 3d 2d 2d 2d
	addps		xmm7, xmm7;				// 4d 4d 4d 4d
	movsd		xmm6, xmm5;				// 3d 2d d 0
	addps		xmm1, xmm6;				// a3 a2 a1 a0
	test		ecx, ecx;
	jnz			loop_math_serial_perfect_square_a16;
	jmp			loop_math_serial_perfect_square_m8;
loop_math_serial_perfect_square_a16:
	movdqa		xmm2, xmm0;
	addps		xmm2, xmm1;
	addps		xmm1, xmm7;				// 下一组元素
	mulps		xmm2, xmm2;				// 完全平方和
	movups		[rdi], xmm2;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_math_serial_perfect_square_a16;
loop_math_serial_perfect_square_m8:
	mov			ecx, [rsp+30h+28h];
	addps		xmm0, xmm1;				// 直接求出最后可能存在的三个元素
	mulps		xmm0, xmm0;				// 完全平方和
	test		ecx, 08h;
	jz			loop_math_serial_perfect_square_m4;
	movlps		QWORD PTR[rdi], xmm0;
	shufps		xmm0, xmm0, 0eh;		// r0 r0 r3 r2
	add			rdi, 08h;
loop_math_serial_perfect_square_m4:
	test		ecx, 04h;
	jz			loop_math_serial_perfect_square_end;
	movss		DWORD PTR[rdi], xmm0;
loop_math_serial_perfect_square_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
	pop			rdi
    ret
    
    
    
;-----------------------------------------------------------------------------
; void math_serial_perfect_square_ex_x64(OUT void* square,		// 完全平方和结果行
;								   OUT void* quartic,		// 四次方结果行
;								   OUT void* sum,			// 和的线性缩放结果行
;								   IN float fStart,			// 序列起始数
;								   IN float fStep,			// 等差
;								   IN float fGiven,			// 特定数
;								   IN float fScaling,		// 和的线性缩放比
;								   IN int na16,				// 行字节宽度除以 16 的商与余数（一次做四个元素）
;								   IN int nm16)
;-----------------------------------------------------------------------------

math_serial_perfect_square_ex_x64:
	;[rsp+48h] - nm16
	;[rsp+40h] - na16
	;[rsp+38h] - fScaling
	;[rsp+30h] - fGiven
	;[rsp+28h] - fStep
	;xmm3 - fStart
	;r8 - sum
	;rdx - quartic
    ;rcx - square 
    
    push		rdi
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
	
    mov			rdi, rcx;
    mov			rax, rdx;
	mov			rcx, [rsp+40h+28h];
	mov			rdx, r8;
	movss		xmm7, DWORD PTR[rsp+28h+28h];
	movss		xmm0, DWORD PTR[rsp+30h+28h];
	movss		xmm1, xmm3;
	movss		xmm3, DWORD PTR[rsp+38h+28h];
	movdqa		xmm6, xmm7;
	shufps		xmm0, xmm0, 00h;		// ag ag ag ag
	shufps		xmm1, xmm1, 00h;		// a0 a0 a0 a0
	shufps		xmm6, xmm6, 15h;		// d 0 0 0
	shufps		xmm7, xmm7, 00h;		// d d d d
	shufps		xmm3, xmm3, 00h;		// scaling
	movhlps		xmm5, xmm6;				// @ @ d 0
	addps		xmm7, xmm7;				// 2d 2d 2d 2d
	addps		xmm6, xmm7;				// 3d 2d 2d 2d
	addps		xmm7, xmm7;				// 4d 4d 4d 4d
	movsd		xmm6, xmm5;				// 3d 2d d 0
	addps		xmm1, xmm6;				// a3 a2 a1 a0
	test		ecx, ecx;
	jnz			loop_math_serial_perfect_square_ex_a16;
	jmp			loop_math_serial_perfect_square_ex_m8;
loop_math_serial_perfect_square_ex_a16:
	movdqa		xmm2, xmm0;
	addps		xmm2, xmm1;
	addps		xmm1, xmm7;				// 下一组元素
	movdqa		xmm6, xmm3;
	mulps		xmm6, xmm2;				// 和的线性缩放
	mulps		xmm2, xmm2;				// 完全平方和
	movdqa		xmm5, xmm2;
	mulps		xmm5, xmm5;				// 四次方
	movups		[rdi], xmm2;
	movups		[rdx], xmm6;
	movups		[rax], xmm5;
	add			rdi, 10h;
	add			rdx, 10h;
	add			rax, 10h;
	sub			ecx, 01h;
	jnz			loop_math_serial_perfect_square_ex_a16;
loop_math_serial_perfect_square_ex_m8:
	mov			ecx, [rsp+48h+28h];
	addps		xmm0, xmm1;				// 直接求出最后可能存在的三个元素
	mulps		xmm3, xmm0;				// 和的线性缩放
	mulps		xmm0, xmm0;				// 完全平方和
	movdqa		xmm5, xmm0;
	mulps		xmm5, xmm5;				// 四次方
	test		ecx, 08h;
	jz			loop_math_serial_perfect_square_ex_m4;
	movlps		QWORD PTR[rdi], xmm0;
	movlps		QWORD PTR[rdx], xmm3;
	movlps		QWORD PTR[rax], xmm5;
	shufps		xmm0, xmm0, 0eh;		// r0 r0 r3 r2
	shufps		xmm3, xmm3, 0eh;		// r0 r0 s3 s2
	shufps		xmm5, xmm5, 0eh;		// r0 r0 r3 r2
	add			rdi, 08h;
	add			rdx, 08h;
	add			rax, 08h;
loop_math_serial_perfect_square_ex_m4:
	test		ecx, 04h;
	jz			loop_math_serial_perfect_square_ex_end;
	movss		DWORD PTR[rdi], xmm0;
	movss		DWORD PTR[rdx], xmm3;
	movss		DWORD PTR[rax], xmm5;
loop_math_serial_perfect_square_ex_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
	pop			rdi
    ret
    
    
;-----------------------------------------------------------------------------
; void math_offset_sqrt_index_lookup_x64(OUT void* des,		// 查表结果（32 位表，要求与表的类型相同）
;								   IN float* pSerial,	// 序列
;								   IN float fOffset,	// 特定偏移
;								   IN void* pTable,		// 表（32 位表）
;								   IN float fScaling,	// 表的大小相对于序列长度的放大倍数
;								   IN int na16,			// 行字节宽度除以 16 的商与余数（一次做四个元素）
;								   IN int nm16)
;-----------------------------------------------------------------------------

math_offset_sqrt_index_lookup_x64:
	;[rsp+38h] - nm16
	;[rsp+30h] - na16
	;[rsp+28h] - fScaling
	;r9 - pTable
	;xmm2 - fOffset
	;rdx - pSerial
    ;rcx - des 
    
    push		rsi
    push		rdi
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
	
    mov			rdi, rcx;
	mov			rsi, rdx;
    mov			rax, r9;
	mov			rcx, [rsp+30h+30h];
	movss		xmm0, xmm2;
	movss		xmm7, DWORD PTR[rsp+28h+30h];
	movd		xmm6, rax;
	shufps		xmm0, xmm0, 00h;		// offset
	shufps		xmm7, xmm7, 00h;		// scale
	shufps		xmm6, xmm6, 44h;		// table
	test		ecx, ecx;
	jnz			loop_math_offset_sqrt_index_lookup_a16;
	jmp			loop_math_offset_sqrt_index_lookup_m8;
loop_math_offset_sqrt_index_lookup_a16:
	movups		xmm1, [rsi];
	addps		xmm1, xmm0;				// 偏移
	rsqrtps		xmm1, xmm1;				// 平方根倒数
	rcpps		xmm1, xmm1;				// 平方根（会比直接使用 sqrtps 快 5 倍，但会出现奇异值）
	mulps		xmm1, xmm7;				// 放大
	cvtps2dq	xmm2, xmm1;				// 四舍五入
	pslld		xmm2, 02h;				// 32 表占据四个字节
	movhlps		xmm4, xmm2;				// @ @ pt3 pt2

	pxor		xmm3,xmm3;
	punpckldq	xmm4, xmm3;
	punpckldq	xmm2, xmm3;
	paddq       xmm2, xmm6;		// 真实的数据地址
	paddq       xmm4, xmm6;		// 真实的数据地址

	movd		rax, xmm2;				// pt0
	movd		rdx, xmm4;				// pt2
	movss		xmm1, DWORD PTR[rax];			// v0
	movss		xmm3, DWORD PTR[rdx];			// v2

	movhlps		xmm4, xmm4;
	movhlps		xmm2, xmm2;
	movd		rax, xmm2;				// pt1
	movd		rdx, xmm4;				// pt3

	movss		xmm2, DWORD PTR[rax];			// v1
	movss		xmm4,DWORD PTR [rdx];			// v3
	movlhps		xmm1, xmm3;				// 0 v2 0 v0
	movlhps		xmm2, xmm4;				// 0 v3 0 v1
	shufps		xmm2, xmm2, 93h;		// v3 0 v1 0
	por			xmm1, xmm2;				// v3 v2 v1 v0
	movdqu		[rdi], xmm1;
	add			rsi, 10h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_math_offset_sqrt_index_lookup_a16;
loop_math_offset_sqrt_index_lookup_m8:
	mov			ecx, [rsp+38h+30h];
	test		ecx, 08h;
	jz			loop_math_offset_sqrt_index_lookup_m4;
	movlps		xmm1, QWORD PTR[rsi];
	addps		xmm1, xmm0;				// 偏移
	rsqrtps		xmm1, xmm1;				// 平方根倒数
	rcpps		xmm1, xmm1;				// 平方根（会比直接使用 sqrtps 快 5 倍，但会出现奇异值）
	mulps		xmm1, xmm7;				// 放大
	cvtps2dq	xmm2, xmm1;				// 四舍五入
	pslld		xmm2, 02h;				// 32 表占据四个字节

	pxor		xmm3,xmm3;
	punpckldq	xmm2, xmm3;
	paddq       xmm2, xmm6;		// 真实的数据地址

	movhlps		xmm4,xmm2;		// @ @ @ pt1

	movd		rax, xmm2;				// pt0
	movd		rdx, xmm4;				// pt1
	movss		xmm1, DWORD PTR[rax];			// v0
	movss		xmm2, DWORD PTR[rdx];			// v1
	movlhps		xmm1, xmm2;				// 0 v1 0 v0
	shufps		xmm1, xmm1, 88h;		// v1 v0 v1 v0
	movlps		QWORD PTR[rdi], xmm1;
	add			rsi, 08h;
	add			rdi, 08h;
loop_math_offset_sqrt_index_lookup_m4:
	test		ecx, 04h;
	jz			loop_math_offset_sqrt_index_lookup_end;
	movss		xmm1,DWORD PTR [rsi];
	addps		xmm1, xmm0;				// 偏移
	rsqrtss		xmm1, xmm1;
	rcpss		xmm1, xmm1;				// 平方根（会比直接使用 sqrtss 快 5 倍，但会出现奇异值）
	mulps		xmm1, xmm7;				// 放大
	cvtps2dq	xmm2, xmm1;				// 四舍五入
	pslld		xmm2, 02h;				// 32 表占据四个字节

	pxor		xmm3, xmm3;
	punpckldq	xmm2, xmm3;
	paddq		xmm2, xmm6;

	movd		rax, xmm2;				// pt0
	movss		xmm1, DWORD PTR[rax];			// v0
	movss		DWORD PTR[rdi], xmm1;
loop_math_offset_sqrt_index_lookup_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
	pop			rdi
	pop			rsi
    ret
    
    
    
;-----------------------------------------------------------------------------
; void math_tyler_arc_index_lookup_x_x64(OUT void* des,			// 查表结果（32 位表，要求与表的类型相同）
;								   IN float* pQuartic,		// 四次方序列
;								   IN float* pSquare,		// 完全平方和序列
;								   IN float* pSum,			// 和序列
;								   IN float fOffsetQ,		// (y - yc)^4
;								   IN float fOffsetSq,		// (y - yc)^2
;								   IN void* pTable,			// 表（32 位表）
;								   IN float fConst,			// 计算式最后添加的常量（定积分的起点值）
;								   IN int na16,				// 行字节宽度除以 16 的商与余数（一次做四个元素）
;								   IN int nm16)
;-----------------------------------------------------------------------------

math_tyler_arc_index_lookup_x_x64:
	;[rsp+50h] - nm16
	;[rsp+48h] - na16
	;[rsp+40h] - fConst
	;[rsp+38h] - pTable
	;[rsp+30h] - fOffsetSq
	;[rsp+28h] - fOffsetQ
	;r9 - pSum
	;r8 - pSquare
	;rdx - pQuartic
    ;rcx - des 
    
    push		rsi
    push		rdi
    push		rbx
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7

    mov			rdi, rcx;
	mov			rsi, r8;			// (x - xc)^2
    mov			rax, [rsp+38h + 38h];
	mov			rbx, r9;				// R * (x - xc)
	mov			rcx, [rsp+48h + 38h];
	;mov			rdx, rdx;			// (x - xc)^4
	movss		xmm0, DWORD PTR[rsp+30h+38h];		// (y - yc)^2
	movss		xmm1, DWORD PTR[rsp+28h+38h];			// (y - yc)^4
	movss		xmm7, DWORD PTR[rsp+40h+38h];			// Cx
	movd		xmm6, rax;				// 表头
	shufps		xmm0, xmm0, 00h;
	shufps		xmm1, xmm1, 00h;
	shufps		xmm7, xmm7, 00h;
	shufps		xmm6, xmm6, 44h;
	test		ecx, ecx;
	jnz			loop_math_tyler_arc_index_lookup_x_a16;
	jmp			loop_math_tyler_arc_index_lookup_x_m8;
loop_math_tyler_arc_index_lookup_x_a16:
	movups		xmm2, [rsi];			// (x - xc)^2
	movups		xmm3, [rbx];			// R * (x - xc)
	movups		xmm4, [rdx];			// (x - xc)^4
	movdqa		xmm5, xmm2;				// (x - xc)^2
	mulps		xmm2, oword ptr[gc_fCoef_13_6];	// 13/6 * (x - xc)^2
	mulps		xmm4, oword ptr[gc_fCoef_149_120];	// 149/120 * (x - xc)^4
	mulps		xmm2, xmm0;				// 13/6 * (x - xc)^2 * (y - yc)^2
	addps		xmm4, xmm1;				// 149/120 * (x - xc)^4 + (y - yc)^4
	addps		xmm5, xmm0;				// (x - xc)^2 + (y - yc)^2
	addps		xmm2, xmm4;				// 149/120 * (x - xc)^4 + 13/6 * (y - yc)^2 * (x - xc)^2 + (y - yc)^4
	rsqrtps		xmm4, xmm5;				// 平方根倒数
	rcpps		xmm5, xmm5;				// 1.f / [(x - xc)^2 + (y - yc)^2]
	mulps		xmm2, xmm3;				// R * (x - xc)[ 149/120 * (x - xc)^4 + 13/6 * (y - yc)^2 * (x - xc)^2 + (y - yc)^4 ]
	mulps		xmm4, xmm5;				// 1.f / [(x - xc)^2 + (y - yc)^2]^2
	mulps		xmm2, xmm5;				// R * (x - xc)[ 149/120 * (x - xc)^4 + 13/6 * (y - yc)^2 * (x - xc)^2 + (y - yc)^4 ] / [(x - xc)^2 + (y - yc)^2]^1/2
	mulps		xmm2, xmm4;
	addps		xmm2, xmm7;
	cvtps2dq	xmm2, xmm2;				// 表索引
	pslld		xmm2, 02h;				// 32 表占据四个字节
	movhlps		xmm4, xmm2;				// @ @ pt3 pt2
	pxor		xmm3, xmm3;
	punpckldq	xmm4, xmm3;
	punpckldq	xmm2, xmm3;
	paddq		xmm2, xmm6;
	;paddd		xmm2, xmm6;				// 真实的数据地址
	paddq		xmm4, xmm6;
	;movhlps		xmm4, xmm2;				// @ @ pt3 pt2

	push		rdx;
	movd		rax, xmm2;				// pt0
	movd		rdx, xmm4;				// pt2
	movss		xmm3, DWORD PTR[rax];			// v0
	movss		xmm5, DWORD PTR[rdx];			// v2
	movhlps		xmm4, xmm4;
	movhlps		xmm2, xmm2;
	;shufps		xmm2, xmm2, 01h;		// pt0 pt0 pt0 pt1
	;shufps		xmm4, xmm4, 01h;		// pt2 pt2 pt2 pt3
	movd		rax, xmm2;				// pt1
	movd		rdx, xmm4;				// pt3
	movss		xmm2, DWORD PTR[rax];			// v1
	movss		xmm4, DWORD PTR[rdx];			// v3
	movlhps		xmm3, xmm5;				// 0 v2 0 v0
	movlhps		xmm2, xmm4;				// 0 v3 0 v1
	shufps		xmm2, xmm2, 93h;		// v3 0 v1 0
	por			xmm3, xmm2;				// v3 v2 v1 v0
	movdqu		[rdi], xmm3;
	pop			rdx;
	add			edx, 10h;
	add			ebx, 10h;
	add			rsi, 10h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_math_tyler_arc_index_lookup_x_a16;
loop_math_tyler_arc_index_lookup_x_m8:
	mov			ecx, [rsp+50h+38h];
	test		ecx, 08h;
	jz			loop_math_tyler_arc_index_lookup_x_m4;
	movlps		xmm2, QWORD PTR[rsi];			// (x - xc)^2
	movlps		xmm3, QWORD PTR[rbx];			// R * (x - xc)
	movlps		xmm4, QWORD PTR[rdx];			// (x - xc)^4
	movdqa		xmm5, xmm2;				// (x - xc)^2
	mulps		xmm2, oword ptr[gc_fCoef_13_6];	// 13/6 * (x - xc)^2
	mulps		xmm4, oword ptr[gc_fCoef_149_120];	// 149/120 * (x - xc)^4
	mulps		xmm2, xmm0;				// 13/6 * (x - xc)^2 * (y - yc)^2
	addps		xmm4, xmm1;				// 149/120 * (x - xc)^4 + (y - yc)^4
	addps		xmm5, xmm0;				// (x - xc)^2 + (y - yc)^2
	addps		xmm2, xmm4;				// 149/120 * (x - xc)^4 + 13/6 * (y - yc)^2 * (x - xc)^2 + (y - yc)^4
	rsqrtps		xmm4, xmm5;				// 平方根倒数
	rcpps		xmm5, xmm5;				// 1.f / [(x - xc)^2 + (y - yc)^2]
	mulps		xmm2, xmm3;				// R * (x - xc)[ 149/120 * (x - xc)^4 + 13/6 * (y - yc)^2 * (x - xc)^2 + (y - yc)^4 ]
	mulps		xmm4, xmm5;				// 1.f / [(x - xc)^2 + (y - yc)^2]^2
	mulps		xmm2, xmm5;				// R * (x - xc)[ 149/120 * (x - xc)^4 + 13/6 * (y - yc)^2 * (x - xc)^2 + (y - yc)^4 ] / [(x - xc)^2 + (y - yc)^2]^1/2
	mulps		xmm2, xmm4;
	addps		xmm2, xmm7;
	cvtps2dq	xmm2, xmm2;				// 表索引
	pslld		xmm2, 02h;				// 32 表占据四个字节

	pxor		xmm3, xmm3;
	punpckldq	xmm2, xmm3;
	paddq		xmm2, xmm6;

	;paddd		xmm2, xmm6;				// 真实的数据地址

	push		rdx;
	movd		rax, xmm2;				// pt0
	movhlps		xmm2, xmm2;				// pt0 pt0 pt0 pt1
	movd		rdx, xmm2;				// pt1
	
	movss		xmm3, DWORD PTR[rax];			// v0
	movss		xmm5, DWORD PTR[rdx];			// v1
	shufps		xmm5, xmm5, 93h;		// 0 0 v1 0
	por			xmm3, xmm5;				// 0 0 v1 v0
	movlps		QWORD PTR[rdi], xmm3;
	pop			rdx;
	add			edx, 08h;
	add			ebx, 08h;
	add			rsi, 08h;
	add			rdi, 08h;
loop_math_tyler_arc_index_lookup_x_m4:
	test		ecx, 04h;
	jz			loop_math_tyler_arc_index_lookup_x_end;
	pxor		xmm2,xmm2
	movss		xmm2, DWORD PTR[rsi];			// (x - xc)^2
	movss		xmm3, DWORD PTR[rbx];			// R * (x - xc)
	movss		xmm4, DWORD PTR[rdx];			// (x - xc)^4
	movdqa		xmm5, xmm2;				// (x - xc)^2
	mulps		xmm2, oword ptr[gc_fCoef_13_6];	// 13/6 * (x - xc)^2
	mulps		xmm4, oword ptr[gc_fCoef_149_120];	// 149/120 * (x - xc)^4
	mulps		xmm2, xmm0;				// 13/6 * (x - xc)^2 * (y - yc)^2
	addps		xmm4, xmm1;				// 149/120 * (x - xc)^4 + (y - yc)^4
	addps		xmm5, xmm0;				// (x - xc)^2 + (y - yc)^2
	addps		xmm2, xmm4;				// 149/120 * (x - xc)^4 + 13/6 * (y - yc)^2 * (x - xc)^2 + (y - yc)^4
	rsqrtps		xmm4, xmm5;				// 平方根倒数
	rcpps		xmm5, xmm5;				// 1.f / [(x - xc)^2 + (y - yc)^2]
	mulps		xmm2, xmm3;				// R * (x - xc)[ 149/120 * (x - xc)^4 + 13/6 * (y - yc)^2 * (x - xc)^2 + (y - yc)^4 ]
	mulps		xmm4, xmm5;				// 1.f / [(x - xc)^2 + (y - yc)^2]^2
	mulps		xmm2, xmm5;				// R * (x - xc)[ 149/120 * (x - xc)^4 + 13/6 * (y - yc)^2 * (x - xc)^2 + (y - yc)^4 ] / [(x - xc)^2 + (y - yc)^2]^1/2
	mulps		xmm2, xmm4;
	addps		xmm2, xmm7;
	cvtps2dq	xmm2, xmm2;				// 表索引
	pslld		xmm2, 02h;				// 32 表占据四个字节
	pxor		xmm3, xmm3;
	punpckldq	xmm2, xmm3;
	paddq		xmm2, xmm6;
	;paddd		xmm2, xmm6;				// 真实的数据地址
	movd		rax, xmm2;				// pt0
	movss		xmm3, DWORD PTR[rax];			// v0
	movss		DWORD PTR[rdi], xmm3;
loop_math_tyler_arc_index_lookup_x_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
	pop			rbx
	pop			rdi
	pop			rsi

    ret
    
    
    
;-----------------------------------------------------------------------------
; void math_tyler_arc_index_lookup_y_x64(OUT void* des,			// 查表结果（32 位表，要求与表的类型相同）
;								   IN float* pQuartic,		// 四次方序列
;								   IN float* pSquare,		// 完全平方和序列
;								   IN float fOffsetQ,		// (y - yc)^4
;								   IN float fOffsetSq,		// (y - yc)^2
;								   IN float fSum,			// R * (y - yc)
;								   IN void* pTable,			// 表（32 位表）
;								   IN float fConst,			// 计算式最后添加的常量（定积分的起点值）
;								   IN int na16,				// 行字节宽度除以 16 的商与余数（一次做四个元素）
;								   IN int nm16)
;-----------------------------------------------------------------------------

math_tyler_arc_index_lookup_y_x64:
	;[rsp+50h] - nm16
	;[rsp+48h] - na16
	;[rsp+40h] - fConst
	;[rsp+38h] - pTable
	;[rsp+30h] - fSum
	;[rsp+28h] - fOffsetSq
	;xmm3 - fOffsetQ
	;r8 - pSquare
	;rdx - pQuartic
    ;rcx - des 

	push		rsi
	push		rdi
	push		rbx
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
    
   	mov			rdi, rcx;
	mov			rsi, r8;			// (x - xc)^2
    mov			rax, [rsp+38h+38h];
	mov			rcx, [rsp+48h+38h];
	;mov			edx, pQuartic;			// (x - xc)^4
	movss		xmm0, DWORD PTR[rsp+28h+38h];		// (y - yc)^2
	movss		xmm1, xmm3;			// (y - yc)^4
	movss		xmm2, DWORD PTR[rsp+30h+38h];				// R * (y - yc)
	movss		xmm7, DWORD PTR[rsp+40h+38h];			// Cy
	movd		xmm6, eax;				// 表头
	movdqa		xmm4, oword ptr[gc_fCoef_149_120];	// 149/120
	shufps		xmm0, xmm0, 00h;
	shufps		xmm1, xmm1, 00h;
	shufps		xmm2, xmm2, 00h;
	shufps		xmm7, xmm7, 00h;
	shufps		xmm6, xmm6, 00h;
	mulps		xmm1, xmm4;				// 149/120 * (y - yc)^4
	test		ecx, ecx;
	jnz			loop_math_tyler_arc_index_lookup_y_a16;
	jmp			loop_math_tyler_arc_index_lookup_y_m8;
loop_math_tyler_arc_index_lookup_y_a16:
	movups		xmm3, [rsi];			// (x - xc)^2
	movups		xmm4, [rdx];			// (x - xc)^4
	movdqa		xmm5, xmm3;				// (x - xc)^2
	addps		xmm4, xmm1;				// 149/120 * (y - yc)^4 + (x - xc)^4
	addps		xmm5, xmm0;				// (x - xc)^2 + (y - yc)^2
	mulps		xmm3, oword ptr[gc_fCoef_13_6];	// 13/6 * (x - xc)^2
	mulps		xmm3, xmm0;				// 13/6 * (x - xc)^2 * (y - yc)^2
	addps		xmm3, xmm4;				// 149/120 * (y - yc)^4 + 13/6 * (y - yc)^2 * (x - xc)^2 + (x - xc)^4
	rsqrtps		xmm4, xmm5;				// 平方根倒数
	rcpps		xmm5, xmm5;				// 1.f / [(x - xc)^2 + (y - yc)^2]
	mulps		xmm3, xmm2;				// R * (y - yc)[ 149/120 * (y - yc)^4 + 13/6 * (y - yc)^2 * (x - xc)^2 + (x - xc)^4 ]
	mulps		xmm4, xmm5;				// 1.f / [(x - xc)^2 + (y - yc)^2]^2
	mulps		xmm3, xmm5;				// R * (y - yc)[ 149/120 * (y - yc)^4 + 13/6 * (y - yc)^2 * (x - xc)^2 + (x - xc)^4 ] / [(x - xc)^2 + (y - yc)^2]^1/2
	mulps		xmm3, xmm4;
	addps		xmm3, xmm7;
	cvtps2dq	xmm3, xmm3;				// 表索引
	pslld		xmm3, 02h;				// 32 表占据四个字节
	paddd		xmm3, xmm6;				// 真实的数据地址
	movhlps		xmm4, xmm3;				// @ @ pt3 pt2
	push		rcx;
	push		rdx;
	movd		eax, xmm3;				// pt0
	movd		ebx, xmm4;				// pt2
	shufps		xmm3, xmm3, 01h;		// pt0 pt0 pt0 pt1
	shufps		xmm4, xmm4, 01h;		// pt2 pt2 pt2 pt3
	movd		ecx, xmm3;				// pt1
	movd		edx, xmm4;				// pt3
	movss		xmm3, DWORD PTR[rax];			// v0
	movss		xmm5, DWORD PTR[rbx];			// v2
	movlhps		xmm3, xmm5;				// 0 v2 0 v0
	movss		xmm5, DWORD PTR[rcx];			// v1
	movss		xmm4, DWORD PTR[rdx];			// v3
	movlhps		xmm5, xmm4;				// 0 v3 0 v1
	shufps		xmm5, xmm5, 93h;		// v3 0 v1 0
	por			xmm3, xmm5;				// v3 v2 v1 v0
	movdqu		[rdi], xmm3;
	pop			rdx;
	pop			rcx;
	add			edx, 10h;
	add			rsi, 10h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_math_tyler_arc_index_lookup_y_a16;
loop_math_tyler_arc_index_lookup_y_m8:
	mov			ecx, [rsp+50h+38h];
	test		ecx, 08h;
	jz			loop_math_tyler_arc_index_lookup_y_m4;
	movlps		xmm3, QWORD PTR[rsi];			// (x - xc)^2
	movlps		xmm4, QWORD PTR[rdx];			// (x - xc)^4
	movdqa		xmm5, xmm3;				// (x - xc)^2
	addps		xmm4, xmm1;				// 149/120 * (y - yc)^4 + (x - xc)^4
	addps		xmm5, xmm0;				// (x - xc)^2 + (y - yc)^2
	mulps		xmm3, oword ptr[gc_fCoef_13_6];	// 13/6 * (x - xc)^2
	mulps		xmm3, xmm0;				// 13/6 * (x - xc)^2 * (y - yc)^2
	addps		xmm3, xmm4;				// 149/120 * (y - yc)^4 + 13/6 * (y - yc)^2 * (x - xc)^2 + (x - xc)^4
	rsqrtps		xmm4, xmm5;				// 平方根倒数
	rcpps		xmm5, xmm5;				// 1.f / [(x - xc)^2 + (y - yc)^2]
	mulps		xmm3, xmm2;				// R * (y - yc)[ 149/120 * (y - yc)^4 + 13/6 * (y - yc)^2 * (x - xc)^2 + (x - xc)^4 ]
	mulps		xmm4, xmm5;				// 1.f / [(x - xc)^2 + (y - yc)^2]^2
	mulps		xmm3, xmm5;				// R * (y - yc)[ 149/120 * (y - yc)^4 + 13/6 * (y - yc)^2 * (x - xc)^2 + (x - xc)^4 ] / [(x - xc)^2 + (y - yc)^2]^1/2
	mulps		xmm3, xmm4;
	addps		xmm3, xmm7;
	cvtps2dq	xmm3, xmm3;				// 表索引
	pslld		xmm3, 02h;				// 32 表占据四个字节
	paddd		xmm3, xmm6;				// 真实的数据地址
	movd		eax, xmm3;				// pt0
	shufps		xmm3, xmm3, 01h;		// pt0 pt0 pt0 pt1
	movd		ebx, xmm3;				// pt1
	movss		xmm4, DWORD PTR[rax];			// v0
	movss		xmm5, DWORD PTR[rbx];			// v1
	shufps		xmm5, xmm5, 93h;		// 0 0 v1 0
	por			xmm4, xmm5;				// 0 0 v1 v0
	movlps		QWORD PTR[rdi], xmm4;
	add			edx, 08h;
	add			rsi, 08h;
	add			rdi, 08h;
loop_math_tyler_arc_index_lookup_y_m4:
	test		ecx, 04h;
	jz			loop_math_tyler_arc_index_lookup_y_end;
	movss		xmm3, DWORD PTR[rsi];			// (x - xc)^2
	movss		xmm4, DWORD PTR[rdx];			// (x - xc)^4
	movdqa		xmm5, xmm3;				// (x - xc)^2
	addps		xmm4, xmm1;				// 149/120 * (y - yc)^4 + (x - xc)^4
	addps		xmm5, xmm0;				// (x - xc)^2 + (y - yc)^2
	mulps		xmm3, oword ptr[gc_fCoef_13_6];	// 13/6 * (x - xc)^2
	mulps		xmm3, xmm0;				// 13/6 * (x - xc)^2 * (y - yc)^2
	addps		xmm3, xmm4;				// 149/120 * (y - yc)^4 + 13/6 * (y - yc)^2 * (x - xc)^2 + (x - xc)^4
	rsqrtps		xmm4, xmm5;				// 平方根倒数
	rcpps		xmm5, xmm5;				// 1.f / [(x - xc)^2 + (y - yc)^2]
	mulps		xmm3, xmm2;				// R * (y - yc)[ 149/120 * (y - yc)^4 + 13/6 * (y - yc)^2 * (x - xc)^2 + (x - xc)^4 ]
	mulps		xmm4, xmm5;				// 1.f / [(x - xc)^2 + (y - yc)^2]^2
	mulps		xmm3, xmm5;				// R * (y - yc)[ 149/120 * (y - yc)^4 + 13/6 * (y - yc)^2 * (x - xc)^2 + (x - xc)^4 ] / [(x - xc)^2 + (y - yc)^2]^1/2
	mulps		xmm3, xmm4;
	addps		xmm3, xmm7;
	cvtps2dq	xmm3, xmm3;				// 表索引
	pslld		xmm3, 02h;				// 32 表占据四个字节
	paddd		xmm3, xmm6;				// 真实的数据地址
	movd		eax, xmm3;				// pt0
	movss		xmm4, DWORD PTR[rax];			// v0
	movss		DWORD PTR[rdi], xmm4;
loop_math_tyler_arc_index_lookup_y_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
	pop			rbx
	pop			rdi
	pop			rsi
    ret
    
    
     
;-----------------------------------------------------------------------------
; void math_negtive_float_x64(OUT float* des,		// 目标
;						IN float* src,		// 源
;						IN int na16,		// 行字节宽度除以 16 的商与余数（一次做四个元素）
;						IN int nm16)
;-----------------------------------------------------------------------------

math_negtive_float_x64:
	;r9 - nm16
	;r8 - na16
	;rdx - src
    ;rcx - des 
    
    push		rsi
    push		rdi
    mov			rsi, rdx;
    mov			rdi, rcx;
    mov			rcx, r8;
	test		ecx, ecx;
	jnz			loop_math_negtive_float_a16;
	jmp			loop_math_negtive_float_m8;
loop_math_negtive_float_a16:
	movups		xmm1, [rsi];
	xorps		xmm0, xmm0;
	subps		xmm0, xmm1;
	movups		[rdi], xmm0;
	add			rsi, 10h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_math_negtive_float_a16;
loop_math_negtive_float_m8:
	mov			rcx, r9;
	test		ecx, 08h;
	jz			loop_math_negtive_float_m4;
	movlps		xmm1, QWORD PTR[rsi];
	xorps		xmm0, xmm0;
	subps		xmm0, xmm1;
	movlps		QWORD PTR[rdi], xmm0;
	add			rsi, 08h;
	add			rdi, 08h;
loop_math_negtive_float_m4:
	test		ecx, 04h;
	jz			loop_math_negtive_float_end;
	movss		xmm1, DWORD PTR[rsi];
	xorps		xmm0, xmm0;
	subps		xmm0, xmm1;
	movss		DWORD PTR[rdi], xmm0;
loop_math_negtive_float_end:
	pop			rdi
	pop			rsi
    ret
         
         
         
;-----------------------------------------------------------------------------
; void math_serial_offset_2(OUT int* des1,		// offset_1 + a
;						  OUT int* des2,		// offset_2 - a
;						  IN int nmin,			// 最小值
;						  IN int nmax,			// 最大值
;						  IN float fOffset1,	// 对于 y = x 为 xc - yc
;						  IN float fOffset2,	// 对于 y = -x 为 xc + yc
;						  IN int nStart,		// 序列起点
;						  IN int nStep,			// 序列步长
;						  IN int na16,			// 缓存字节宽度除以 16 的商与余数（一次做四个元素）
;						  IN int nm16)
;-----------------------------------------------------------------------------

math_serial_offset_2_x64:
	;[rsp+50h] - nm16
	;[rsp+48h] - na16
	;[rsp+40h] - nStep
	;[rsp+38h] - nStart
	;[rsp+30h] - fOffset2
	;[rsp+28h] - fOffset1
	;r9 - nmax
	;r8 - nmin
	;rdx - des2
    ;rcx - des1 
    
    push		rdi
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
	
    mov			rdi, rdx;
	mov			rdx, rcx;
	mov			rcx, [rsp+48h+28h];
	mov			rax,r8;
	movd		xmm0, eax;
	movss		xmm1, DWORD PTR[rsp+28h+28h];
	movss		xmm2, DWORD PTR[rsp+30h+28h];
	movss		xmm4, DWORD PTR[rsp+38h+28h];
	movss		xmm6, DWORD PTR[rsp+40h+28h];
	mov			rax,r9;
	movd		xmm7, eax;
	movdqa		xmm3, xmm6;
	shufps		xmm0, xmm0, 00h;		// min
	shufps		xmm1, xmm1, 00h;		// offset1
	shufps		xmm2, xmm2, 00h;		// offset2
	shufps		xmm3, xmm3, 15h;		// d 0 0 0
	shufps		xmm4, xmm4, 00h;		// a0 a0 a0 a0
	shufps		xmm6, xmm6, 00h;		// d d d d
	shufps		xmm7, xmm7, 00h;		// max
	movhlps		xmm5, xmm3;				// @ @ d 0
	addps		xmm6, xmm6;				// 2d 2d 2d 2d
	addps		xmm3, xmm6;				// 3d 2d 2d 2d
	addps		xmm6, xmm6;				// 4d 4d 4d 4d
	movsd		xmm3, xmm5;				// 3d 2d d 0
	addps		xmm4, xmm3;				// a3 a2 a1 a0
	cvtdq2ps	xmm0, xmm0;
	cvtdq2ps	xmm7, xmm7;
	cvtdq2ps	xmm6, xmm6;
	cvtdq2ps	xmm4, xmm4;
	test		ecx, ecx;
	jnz			loop_math_serial_offset_2_a16;
	jmp			loop_math_serial_offset_2_m8;
loop_math_serial_offset_2_a16:
	movdqa		xmm3, xmm1;
	movdqa		xmm5, xmm2;
	addps		xmm3, xmm4;				// offset1 + a
	subps		xmm5, xmm4;				// offset2 - a
	addps		xmm4, xmm6;				// 下一组序列
	maxps		xmm3, xmm0;
	maxps		xmm5, xmm0;
	minps		xmm3, xmm7;
	minps		xmm5, xmm7;
	cvtps2dq	xmm3, xmm3;
	cvtps2dq	xmm5, xmm5;
	movdqu		[rdx], xmm3;
	movdqa		[rdi], xmm5;
	add			edx, 10h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_math_serial_offset_2_a16;
loop_math_serial_offset_2_m8:
	mov			ecx, [rsp+50h+28h];
	;// 直接把可能的元素都做完
	addps		xmm1, xmm4;				// offset1 + a
	subps		xmm2, xmm4;				// offset2 - a
	maxps		xmm1, xmm0;
	maxps		xmm2, xmm0;
	minps		xmm1, xmm7;
	minps		xmm2, xmm7;
	cvtps2dq	xmm1, xmm1;
	cvtps2dq	xmm2, xmm2;
	test		ecx, 08h;
	jz			loop_math_serial_offset_2_m4;
	movlps		QWORD PTR[rdx], xmm1;
	movlps		QWORD PTR[rdi], xmm2;
	shufps		xmm1, xmm1, 0eh;		// r0 r0 r3 r2
	shufps		xmm2, xmm2, 0eh;		// r0 r0 s3 s2
	add			edx, 08h;
	add			rdi, 08h;
loop_math_serial_offset_2_m4:
	test		ecx, 04h;
	jz			loop_math_serial_offset_2_end;
	movss		DWORD PTR[rdx], xmm1;
	movss		DWORD PTR[rdi], xmm2;
loop_math_serial_offset_2_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
	pop			rdi
    ret
    
    
;-----------------------------------------------------------------------------
; void trans_power_index_bl_x64(OUT DWORD* pdwSrcIndex,			// 双线性纹理映射时源的四个索引（S11, S10, S01, S00）
;						  OUT float* pfPowerX,				// 水平方向的权重
;						  OUT float* pfPowerY,				// 垂直方向的权重
;						  OUT float* pfPowerXY,				// 权重之积
;						  IN void* pvLeftEdgeUVW,			// 行左边缘的 UVW 值
;						  IN void* pvDeltaUVW,				// 行内的 UVW 增量
;						  IN int nSrcWidth,					// 源的大小
;						  IN int nSrcHeight,
;						  IN int nSrcBufWidth,				// 源缓存的真实宽度
;						  IN int nLinePixels)				// 一行要处理的像素个数
;-----------------------------------------------------------------------------

trans_power_index_bl_x64:
	;[rsp+50h] - nLinePixels
	;[rsp+48h] - nSrcBufWidth
	;[rsp+40h] - nSrcHeight
	;[rsp+38h] - nSrcWidth
	;[rsp+30h] - pvDeltaUVW
	;[rsp+28h] - pvLeftEdgeUVW
	;r9 - pfPowerXY
	;r8 - pfPowerY
	;rdx - pfPowerX
    ;rcx - pdwSrcIndex 
    
    push		rsi
    push		rdi
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
    
    mov			rdi, rcx;	// S11  S10  S01  S00
    mov			rsi, rdx;		// x
    
    mov			rax, [rsp+28h+30h];
	mov			rdx, [rsp+30h+30h];
	mov			ecx, 01h;
	movups		xmm0, [rax];		// @  1/w  v/w  u/w
	movups		xmm1, [rdx];		// @  d(1/w)  d(v/w)  d(u/w)
	movd		xmm4, ecx;			// 0  0  0  1
	movss		xmm5, DWORD PTR[rsp+38h+30h];	// 0  0  0  sw
	movss		xmm6, DWORD PTR[rsp+40h+30h];	// 0  0  0  sh
	movss		xmm7, DWORD PTR[rsp+48h+30h];	// 0  0  0  sbw
	movlhps		xmm5, xmm6;			// 0  sh  0  sw
	shufps		xmm4, xmm4, 44h;	// 0  1  0  1
	shufps		xmm7, xmm7, 00h;	// sbw  sbw  sbw  sbw
	psubd		xmm5, xmm4;			// 0  (sh-1)  0  (sw-1)
	shufps		xmm5, xmm4, 08h;	// 1  1  (sh-1)  (sw-1)
	cvtdq2ps	xmm7, xmm7;
	cvtdq2ps	xmm5, xmm5;
	mulps		xmm0, xmm5;			// @  1/w  v*(sh-1)/w  u*(sw-1)/w
	mulps		xmm1, xmm5;			// @  d(1/w)  d(v/w)*(sh-1)  d(u/w)*(sw-1)
	mov			ecx, [rsp+50h+30h];
	
	mov			rax, rsi;		// x
	mov			rdx, r8;		// y
	mov			rsi, r9;		// xy
	test		ecx, ecx;
	jnz			loop_trans_power_index_bl_pixel;
	jmp			loop_trans_power_index_bl_end;
loop_trans_power_index_bl_pixel:
	movaps		xmm2, xmm0;			// @  w  @  @
	;//shufps		xmm2, xmm2, aah;	// w  w  w  w
	;//mulps		xmm2, xmm0;			// @  @  fSrcY  fSrcX
	addps		xmm0, xmm1;
	cvttps2dq	xmm3, xmm2;			// @  @  nSrcY  nSrcX
	cvtdq2ps	xmm3, xmm3;
	movhlps		xmm4, xmm5;			// @  @  1  1
	addps		xmm4, xmm3;			// @  @  (nSrcY+1)  (nSrcX+1)
	movaps		xmm6, xmm5;			// 1  1  (sh-1)  (sw-1)
	shufps		xmm3, xmm4, 44h;	// (nSrcY+1)  (nSrcX+1)  nSrcY  nSrcX
	shufps		xmm6, xmm6, 44h;	// (sh-1)  (sw-1)  (sh-1)  (sw-1)
	subps		xmm4, xmm2;			// @  @  y  x
	xorps		xmm2, xmm2;
	maxps		xmm3, xmm2;
	minps		xmm3, xmm6;			// avoid overflow
	movaps		xmm2, xmm4;
	movaps		xmm6, xmm3;
	shufps		xmm6, xmm6, 0f5h;	// (nSrcY+1)  (nSrcY+1)  nSrcY  nSrcY
	shufps		xmm3, xmm3, 88h;	// (nSrcX+1)  nSrcX  (nSrcX+1)  nSrcX
	shufps		xmm4, xmm4, 11h;	// x  y  x  y
	mulps		xmm6, xmm7;
	addps		xmm6, xmm3;			// S11  S10  S01  S00
	movaps		xmm3, xmm4;
	mulps		xmm4, xmm2;			// @  @  xy  xy
	cvttps2dq	xmm6, xmm6;
	movss		DWORD PTR[rax], xmm2;
	movss		DWORD PTR[rdx], xmm3;
	movss		DWORD PTR[rsi], xmm4;
	movdqu		[rdi], xmm6;
	add			eax, 04h;
	add			edx, 04h;
	add			rsi, 04h;
	add			rdi, 10h;
	sub			ecx, 01h;
	jnz			loop_trans_power_index_bl_pixel;
loop_trans_power_index_bl_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
	pop			rdi
	pop			rsi
    ret
    
    
;-----------------------------------------------------------------------------
; void trans_interpo_bl_x64(OUT DWORD* pdwDesLeftEdge,	// 目标行的起始位置
;					  IN DWORD* *ppdwSrcStart,		// 源缓存的起始位置
;					  IN DWORD* pdwSrcIndex,		// 双线性纹理映射时源的四个索引
;					  IN float* pfPowerX,			// 水平方向的权重
;					  IN float* pfPowerY,			// 垂直方向的权重
;					  IN float* pfPowerXY,			// 权重之积
;					  IN int nLinePixels)			// 一行要处理的像素个数
;-----------------------------------------------------------------------------

trans_interpo_bl_x64:
	;[rsp+38h] - nLinePixels
	;[rsp+30h] - pfPowerXY
	;[rsp+28h] - pfPowerY
	;r9 - pfPowerX
	;r8 - pdwSrcIndex
	;rdx - ppdwSrcStart
    ;rcx - pdwDesLeftEdge 

	push		rsi
	push		rdi
	push		rbx
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7
    
    mov			rdi, rcx;		// output line
	mov			rsi, r8;			// relative indexes to start position
	mov			rax, rdx;
	mov			ecx, [rsp+38h+38h];
	movdqu		xmm0, [rax];			// start position of source
	mov			rax, r9;
	mov			ebx, [rsp+28h+38h];
	mov			edx, [rsp+30h+38h];
	test		ecx, ecx;
	jnz			loop_trans_interpo_bl_pixel;
	jmp			loop_trans_interpo_bl_end;
loop_trans_interpo_bl_pixel:
	movdqu		xmm2, [rsi];		// S11  S10  S01  S00
	pslld		xmm2, 02h;			// sizeof(DWORD)
	paddd		xmm2, xmm0;
	push		rax;
	push		rdx;
	;// Read source data by pointers in xmm1 and xmm2.
	;// S00 and S10.
	movhlps		xmm4, xmm2;			// @  @  S11  S10
	movd		eax, xmm2;			// S00
	movd		edx, xmm4;			// S10
	movss		xmm1, DWORD PTR[rax];		// S00
	movss		xmm3, DWORD PTR[rdx];		// S10
	;// S01 and S11.
	shufps		xmm2, xmm2, 01h;	// @  @  @  S01
	shufps		xmm4, xmm4, 01h;	// @  @  @  S11
	movd		eax, xmm2;			// S01
	movd		edx, xmm4;			// S11
	movss		xmm2, DWORD PTR[rax];		// S01
	movss		xmm4, DWORD PTR[rdx];		// S11
	pop			rdx;
	pop			rax;
	;// Expand each byte to a dword, and then converts them to float for calculating.
	pxor		xmm7, xmm7;			// 0
	punpcklbw	xmm1, xmm7;			// @ 0 @ 0 A 0 R 0 G 0 B 0
	punpcklbw	xmm2, xmm7;
	punpcklbw	xmm3, xmm7;
	punpcklbw	xmm4, xmm7;
	punpcklwd	xmm1, xmm7;			// A  R  G  B
	punpcklwd	xmm2, xmm7;
	punpcklwd	xmm3, xmm7;
	punpcklwd	xmm4, xmm7;
	;// Edge processing with alpha mask.
	;	// Switch the rows' color with alpha mask.
	movdqa		xmm5, xmm1;			// S00
	movdqa		xmm6, xmm3;			// S10
	por			xmm5, xmm2;			// A00 | A01
	por			xmm6, xmm4;			// A10 | A11
	pcmpeqd		xmm5, xmm7;			// check (A00 | A01) == 0
	pcmpeqd		xmm6, xmm7;			// check (A10 | A11) == 0
	shufps		xmm5, xmm5, 0ffh;	// mask_00&01
	shufps		xmm6, xmm6, 0ffh;	// mask_10&11
	pand		xmm5, oword ptr[gc_dwMaskSpecUV];	// 0  mask_00&01
	pand		xmm6, oword ptr[gc_dwMaskSpecUV];	// 0  mask_10&11
	pandn		xmm5, oword ptr[gc_nComp_N1];		// 1  neg_mask_00&01
	pandn		xmm6, oword ptr[gc_nComp_N1];		// 1  neg_mask_10&11
	movdqa		xmm7, xmm5;			// 1  neg_mask_00&01
	pand		xmm1, xmm5;			// A00  (C00 & neg_mask_00&01)
	pand		xmm2, xmm5;			// A01  (C01 & neg_mask_00&01)
	pandn		xmm5, xmm3;			// 0  (C10 & mask_00&01)
	pandn		xmm7, xmm4;			// 0  (C11 & mask_00&01)
	por			xmm1, xmm5;			// new S00
	por			xmm2, xmm7;			// new S01
	movdqa		xmm7, xmm6;			// 1  neg_mask_10&11
	pand		xmm3, xmm6;			// A10  (C10 & neg_mask_10&11)
	pand		xmm4, xmm6;			// A11  (C11 & neg_mask_10&11)
	pandn		xmm6, xmm1;			// 0  (C00 & mask_10&11)
	pandn		xmm7, xmm2;			// 0  (C01 & mask_10&11)
	por			xmm3, xmm6;			// new S10
	por			xmm4, xmm7;			// new S11
	;	// Switch the first row's columns' color with alpha mask.
	pxor		xmm7, xmm7;			// 0
	movdqa		xmm5, xmm1;			// S00
	movdqa		xmm6, xmm2;			// S01
	pcmpeqd		xmm5, xmm7;			// check A00 == 0
	pcmpeqd		xmm6, xmm7;			// check A01 == 0
	shufps		xmm5, xmm5, 0ffh;	// mask_00
	shufps		xmm6, xmm6, 0ffh;	// mask_01
	pand		xmm5, oword ptr[gc_dwMaskSpecUV];	// 0  mask_00
	pand		xmm6, oword ptr[gc_dwMaskSpecUV];	// 0  mask_01
	pandn		xmm5, oword ptr[gc_nComp_N1];		// 1  neg_mask_00
	pandn		xmm6, oword ptr[gc_nComp_N1];		// 1  neg_mask_01
	movdqa		xmm7, xmm5;
	pand		xmm2, xmm6;			// A01  (C01 & neg_mask_01)
	pandn		xmm5, xmm2;			// 0  (C01 & mask_00)
	pandn		xmm6, xmm1;			// 0  (C00 & mask_01)
	pand		xmm1, xmm7;			// A00  (C00 & neg_mask_00)
	por			xmm2, xmm6;			// new S01
	por			xmm1, xmm5;			// new S00
	;	// Switch the second row's columns' color with alpha mask.
	pxor		xmm7, xmm7;			// 0
	movdqa		xmm5, xmm3;			// S10
	movdqa		xmm6, xmm4;			// S11
	pcmpeqd		xmm5, xmm7;			// check A10 == 0
	pcmpeqd		xmm6, xmm7;			// check A11 == 0
	shufps		xmm5, xmm5, 0ffh;	// mask_10
	shufps		xmm6, xmm6, 0ffh;	// mask_11
	pand		xmm5, oword ptr[gc_dwMaskSpecUV];	// 0  mask_10
	pand		xmm6, oword ptr[gc_dwMaskSpecUV];	// 0  mask_11
	pandn		xmm5, oword ptr[gc_nComp_N1];		// 1  neg_mask_10
	pandn		xmm6, oword ptr[gc_nComp_N1];		// 1  neg_mask_11
	movdqa		xmm7, xmm5;
	pand		xmm4, xmm6;			// A11  (C11 & neg_mask_11)
	pandn		xmm5, xmm4;			// 0  (C11 & mask_10)
	pandn		xmm6, xmm3;			// 0  (C10 & mask_11)
	pand		xmm3, xmm7;			// A00  (C10 & neg_mask_10)
	por			xmm4, xmm6;			// new S11
	por			xmm3, xmm5;			// new S10
	;// Do integer operations.
	psubd		xmm2, xmm4;			// S01 - S11
	psubd		xmm1, xmm3;			// S00 - S10
	psubd		xmm3, xmm4;			// S10 - S11
	psubd		xmm1, xmm2;			// S00 - S01 - S10 + S11
	cvtdq2ps	xmm2, xmm2;
	cvtdq2ps	xmm3, xmm3;
	cvtdq2ps	xmm1, xmm1;
	;// Multiply weight coefficients.
	movss		xmm5,DWORD PTR [rax];		// x
	movss		xmm6, DWORD PTR[rbx];		// y
	movss		xmm7, DWORD PTR[rdx];		// xy
	shufps		xmm5, xmm5, 00h;	// x  x  x  x
	shufps		xmm6, xmm6, 00h;	// y  y  y  y
	shufps		xmm7, xmm7, 00h;	// xy  xy  xy  xy
	mulps		xmm5, xmm3;			// (S10 - S11) * x
	mulps		xmm6, xmm2;			// (S01 - S11) * y
	mulps		xmm7, xmm1;			// (S00 - S01 - S10 + S11) * xy
	addps		xmm5, xmm6;
	addps		xmm5, xmm7;
	cvtps2dq	xmm5, xmm5;
	paddd		xmm5, xmm4;			// D
	packssdw	xmm5, xmm5;
	packuswb	xmm5, xmm5;
	movss		DWORD PTR[rdi], xmm5;
	;// Add pointer for next loop.
	add			rsi, 10h;
	add			eax, 04h;
	add			ebx, 04h;
	add			edx, 04h;
	add			rdi, 04h;
	sub			ecx, 01h;
	jnz			loop_trans_interpo_bl_pixel;
loop_trans_interpo_bl_end:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
	pop			rbx
	pop			rdi
	pop			rsi
    ret                        
    
  
;------------------------------------------------------------------------- 
;// 一行 24 位缓存到 32 位缓存的转换，填充统一的 Alpha 通道
;// 很显然 24 位缓存会四字节对齐，因此在处理最后几个像素时要充分利用这个特点
;void color24_to_color32_x64(OUT void* color32,	// *G@A 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……
;						IN void* color24,	// *G@ 输入，存放方式 *0 G0 @0 *1 G1 @1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
;						IN DWORD dwAlpha,	// Alpha 通道值，要求以这种方式提供：AA000000h
;						IN int nap5,		// 源像素宽度除以 5 的商与余数（正好满足十六字节对齐）
;						IN int nmp5)
;--------------------------------------------------------------------------
color24_to_color32_x64:
	;rcx			- color32
	;rdx			- color24
	;r8				- dwAlpha
	;r9				- nap5
	;[rsp + 28h]	- nmp5
	
	push		rsi
	push		rdi
	sub			rsp,20h
	movups		[rsp],xmm6
	movups		[rsp + 10h],xmm7

	mov			rdi, rcx;		// 前四个像素
	mov			rsi, rdx;
	mov			rdx, rcx;		// 最后一个像素
	mov			rcx, r9;
	
	add			rdx, 10h;
	mov			rax, r8;
	movd		xmm0, eax;
	movdqa		xmm1, OWORD PTR[gc_dwMaskRGB];
	shufps		xmm0, xmm0, 00h;
	test		ecx, ecx;
	jnz			loop_a5_color24_to_color32;
	jmp			loop_m4_color24_to_color32;
loop_a5_color24_to_color32:;		// 一次做五个像素
	movdqu		xmm7, [rsi];		// RBGR BGRB GRBG RBGR
	movdqa		xmm4, xmm7;
	movdqa		xmm5, xmm7;
	movdqa		xmm6, xmm7;
	pshufd		xmm3, xmm7, 00h;	// RBGR RBGR RBGR RBGR
	pshufd		xmm2, xmm7, 0aah;	// BGRB BGRB BGRB BGRB
	pslld		xmm4, 08h;			// * *  *B1G1 0  *
	psrld		xmm5, 10h;			// * * 00G2R2 *
	psrld		xmm6, 08h;			// * 0B3G3R3 * *
	psrld		xmm3, 18h;			// 000R1 000R1 000R1 000R1
	pslld		xmm2, 10h;			// *B2 00  *B2 00  *B2 00  *B2 00
	por			xmm4, xmm3;			// * * *B1G1R1 *
	por			xmm5, xmm2;			// * * *B2G2R2 *
	shufps		xmm4, xmm7, 05h;	// *B0G0R0 *B0G0R0 *B1G1R1 *B1G1R1
	shufps		xmm5, xmm6, 0a5h;	// 0B3G3R3 0B3G3R3 *B2G2R2 *B2G2R2
	shufps		xmm7, xmm7, 0ffh;	// *B4G4R4 *B4G4R4 *B4G4R4 *B4G4R4
	shufps		xmm4, xmm5, 82h;	// 0B3G3R3 *B2G2R2 *B1G1R1 *B0G0R0
	pand		xmm7, xmm1;
	pand		xmm4, xmm1;
	por			xmm7, xmm0;
	por			xmm4, xmm0;
	movss		DWORD PTR[rdx], xmm7;
	movdqu		[rdi], xmm4;
	add			rsi, 0fh;
	add			rdx, 14h;
	add			rdi, 14h;
	sub			rcx, 01h;
	jnz			loop_a5_color24_to_color32;
loop_m4_color24_to_color32:		;// 剩余四个像素
	mov			rcx, [rsp + 28h+30h];
	test		rcx, 04h;
	jz			loop_m3_color24_to_color32;
	mov			rax, rsi;
	add			rsi, 08h;
	movsd		xmm7, QWORD PTR[rax];		// 0000 0000 GRBG RBGR
	movss		xmm6, DWORD PTR[rsi];		// 0000 0000 0000 BGRB
	pshufd		xmm2, xmm7, 92h;	// 0000 GRBG RBGR 0000
	shufps		xmm6, xmm6, 05h;	// BGRB BGRB 0000 0000
	movdqa		xmm4, xmm7;
	movdqa		xmm5, xmm2;
	movdqa		xmm3, xmm6;
	psrld		xmm2, 10h;			// 0  00G2R2  *  0
	pslld		xmm4, 08h;			// 0 0  *B1G1 0  *
	psrld		xmm5, 18h;			// 0 *  000R1  0
	pslld		xmm3, 10h;			// *  *B2 00  0 0
	psrld		xmm6, 08h;			// 0B3G3R3 * 0 0
	por			xmm4, xmm5;			// 0 * *B1G1R1 *
	por			xmm2, xmm3;			// * *B2G2R2 * 0
	shufps		xmm4, xmm7, 05h;	// *B0G0R0 *B0G0R0 *B1G1R1 *B1G1R1
	shufps		xmm2, xmm6, 0fah;	// 0B3G3R3 0B3G3R3 *B2G2R2 *B2G2R2
	shufps		xmm4, xmm2, 82h;	// 0B3G3R3 *B2G2R2 *B1G1R1 *B0G0R0
	pand		xmm4, xmm1;
	por			xmm4, xmm0;
	movdqu		[rdi], xmm4;
	and			rcx, 03h;
loop_m3_color24_to_color32:		;// 剩余三个像素
	cmp			rcx, 03h;
	jnz			loop_m2_color24_to_color32;
	mov			rax, rsi;
	add			rsi, 08h;
	sub			rdx, 08h;
	movsd		xmm7, QWORD PTR[rax];		// 0000 0000 GRBG RBGR
	xor			rax,rax;
	mov			al,[rsi];
	movd		xmm6, eax;		// 0000 0000 0000 000B
	pshufd		xmm2, xmm7, 0a1h;	// 0000 0000 RBGR GRBG
	movdqa		xmm5, xmm2;
	movdqa		xmm4, xmm7;
	psrld		xmm2, 10h;			// 0 0 * 00G2R2
	pslld		xmm6, 10h;			// 0 0 0  0B2 00
	pslld		xmm4, 08h;			// 0 0  *B1G1 0  *
	psrld		xmm5, 18h;			// 0 0  000R1  *
	por			xmm2, xmm6;			// 0 0 * 0B2G2R2
	por			xmm4, xmm5;			// 0 0 *B1G1R1 *
	shufps		xmm4, xmm7, 05h;	// *B0G0R0 *B0G0R0 *B1G1R1 *B1G1R1
	shufps		xmm4, xmm4, 22h;	// *B1G1R1 *B0G0R0 *B1G1R1 *B0G0R0
	pand		xmm2, xmm1;
	pand		xmm4, xmm1;
	por			xmm2, xmm0;
	por			xmm4, xmm0;
	movss		DWORD PTR[rdx], xmm2;
	movsd		QWORD PTR[rdi], xmm4;
	jmp			loop_end_color24_to_color32;
loop_m2_color24_to_color32:		;// 剩余两个像素
	test		rcx, 02h;
	jz			loop_m1_color24_to_color32;
	movss		xmm7, DWORD PTR[rsi];
	pand		xmm7, xmm1;
	por			xmm7, xmm0;
	movss		DWORD PTR[rdi], xmm7;
	add			rsi,3
	add			rdi,4
	mov			ax,[rsi];
	sal			rax,16;
	add			rsi,2;
	mov			ah,[rsi];
	sar			rax,8;
	movd		xmm7, rax;
	pand		xmm7, xmm1;
	por			xmm7, xmm0;
	movss		DWORD PTR[rdi], xmm7;
	jmp			loop_end_color24_to_color32;
loop_m1_color24_to_color32:		;// 剩余一个像素
	test		rcx, 01h;
	jz			loop_end_color24_to_color32;
	mov			ax,[rsi];
	sal			rax,16;
	add			rsi,2;
	mov			ah,[rsi];
	sar			rax,8;
	movd		xmm7, rax;
	pand		xmm7, xmm1;
	por			xmm7, xmm0;
	movss		DWORD PTR[rdi], xmm7;
loop_end_color24_to_color32:
	movups		xmm6,[rsp]
	movups		xmm7,[rsp + 10h]
	add			rsp,20h
	pop			rdi
	pop			rsi
	ret

;--------------------------------------------------------------------------
;void yuyv_to_color24_inner_x64(OUT void* color24,   // *G@ 缓存，存放方式 *0 G0 @0 *1 G1 @1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
;						   IN void* yuyv,		// 源 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
;						   IN int na16,			// 源字节宽度减 4 后除以 16 的商与余数
;						   IN int nm16,
;						   IN int* pnR_V,       // 系数（通过变更 R 分量与 B 分量的系数位置可以做到 BGRA 与 RGBA 的兼容）
;						   IN int* pnG_U,       // （此处只需要将 pnR_V 与 pnB_U 的输入交换即可实现）
;						   IN int* pnG_V,
;						   IN int* pnB_U)
;--------------------------------------------------------------------------						   
yuyv_to_color24_inner_x64:
	;rcx		- color24
	;rdx		- yuyv
	;r8			- na16
	;r9			- nm16
	;[rsp+28h]	- pnR_V
	;[rsp+30h]	- pnG_U
	;[rsp+38h]	- pnG_V
	;[rsp+40h]	- pnB_U

		push		rsi
		push		rdi
		push		rbx
		sub			rsp,20h
		movups		[rsp],xmm6
		movups		[rsp + 10h],xmm7

		mov			rsi, rdx;
		mov			rax, rdx;
		mov			rdi, rcx;
		mov			rdx, rcx;
		mov			rcx, r8;
		mov        rbx, [rsp+38h+38h];
		add			rax, 10h;			// 下一个 VYUV 用于 UV 插值
		add			rdx, 10h;			// 下半组 RGB 存储
		test		ecx, ecx;
		jnz			loop_a16_yuyv_to_color24_inner;
		jmp			loop_m16_yuyv_to_color24_inner;
loop_a16_yuyv_to_color24_inner:
		movdqu		xmm7, [rsi];		// VYUY VYUY VYUY VYUY
		movss		xmm6, DWORD PTR[rax];		// 下一个 VYUV 用于 UV 插值
		movdqa		xmm1, xmm7;
		movdqa		xmm0, OWORD PTR[gc_dwMaskWLB];
		movdqa		xmm3, OWORD PTR[gc_dwMaskWHB];
		movdqa		xmm4, OWORD PTR[gc_dwMaskSpecUV];
		movdqa		xmm5, OWORD PTR[gc_nOff128_6];
		pand		xmm7, xmm0;			// Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
		pand		xmm1, xmm3;			// V6 U6 V4 U4 V2 U2 V0 U0
		pand		xmm6, xmm3;			// 0 0 0 0 0 0 V8 U8
		psrlw		xmm1, 2;			// 放大 6 位
		psrlw		xmm6, 2;			// 放大 6 位
		psllw		xmm7, 5;			// 放大 5 位
		pshufd		xmm2, xmm1, 39h;	// V0 U0 V6 U6 V4 U4 V2 U2
		shufps		xmm6, xmm6, 15h;	// V8 U8 0 0 0 0 0 0
		pand		xmm2, xmm4;			// 0 0 V6 U6 V4 U4 V2 U2
		por			xmm2, xmm6;			// V8 U8 V6 U6 V4 U4 V2 U2
		psubsw		xmm1, xmm5;			// 减去偏移量 128
		psubsw		xmm2, xmm5;			// 减去偏移量 128
		movdqa		xmm3, xmm1;
		movdqa		xmm5, xmm1;
		movdqa		xmm6, xmm1;
		movdqa		xmm4, xmm2;
		pand		xmm1, OWORD PTR[gc_dwMaskDLW];	// 0 U6 0 U4 0 U2 0 U0
		pand		xmm3, OWORD PTR[gc_dwMaskDHW];	// V6 0 V4 0 V2 0 V0 0
		pand		xmm2, OWORD PTR[gc_dwMaskDHW];	// V8 0 V6 0 V4 0 V2 0
		pslld		xmm4, 10h;			// U8 0 U6 0 U4 0 U2 0
		pslld		xmm5, 10h;			// U6 0 U4 0 U2 0 U0 0
		psrld		xmm6, 10h;			// 0 V6 0 V4 0 V2 0 V0
		por			xmm4, xmm1;			// U8 U6 U6 U4 U4 U2 U2 U0
		por			xmm5, xmm1;			// U6 U6 U4 U4 U2 U2 U0 U0
		por			xmm2, xmm6;			// V8 V6 V6 V4 V4 V2 V2 V0
		por			xmm3, xmm6;			// V6 V6 V4 V4 V2 V2 V0 V0
		paddw		xmm4, xmm5;			// U7 U6 U5 U4 U3 U2 U1 U0
		paddw		xmm3, xmm2;			// V7 V6 V5 V4 V3 V2 V1 V0
		push        rsi;
		push        rax;
		push        rcx;
		mov         rax, [rsp+40h+38h];	//pnR_V
		mov         rcx, [rsp+48h+38h];	//pnG_U
		mov         rsi, [rsp+58h+38h];	//pnB_U
		movdqu		xmm1, [rax];
		movdqu		xmm2, [rbx];
		movdqu		xmm5, [rcx];
		movdqu		xmm6, [rsi];
		pop         rcx;
		pop         rax;
		pop         rsi;
		pmulhw		xmm1, xmm3;
		pmulhw		xmm2, xmm3;
		pmulhw		xmm5, xmm4;
		pmulhw		xmm6, xmm4;
		pxor		xmm3, xmm3;			// 0
		movdqa		xmm4, OWORD PTR[gc_nRoundComp_5];
		paddsw		xmm1, xmm7;			// R
		paddsw		xmm2, xmm7;			// G
		paddsw		xmm5, xmm4;
		paddsw		xmm6, xmm7;			// B
		paddsw		xmm1, xmm4;			// R
		paddsw		xmm2, xmm5;			// G
		paddsw		xmm6, xmm4;			// B
		pmaxsw		xmm1, xmm3;			// R
		pmaxsw		xmm2, xmm3;			// G
		pmaxsw		xmm6, xmm3;			// B
		psrlw		xmm1, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm1, xmm0;		// R7 R6 R5 R4 R3 R2 R1 R0
		pminsw		xmm2, xmm0;		// G7 G6 G5 G4 G3 G2 G1 G0
		pminsw		xmm6, xmm0;		// B7 B6 B5 B4 B3 B2 B1 B0
		movdqa		xmm4, xmm1;		// R7 R6 R5 R4 R3 R2 R1 R0
		movdqa		xmm5, xmm2;		// G7 G6 G5 G4 G3 G2 G1 G0
		movdqa		xmm7, xmm6;		// B7 B6 B5 B4 B3 B2 B1 B0
		psrld		xmm1, 08h;		// 00 R7 0  00 R5 0  00 R3 0  00 R1 0
		psllw		xmm2, 08h;		// G7 0 G6 0 G5 0 G4 0 G3 0 G2 0 G1 0 G0 0
		psllw		xmm6, 08h;		// B7 0 B6 0 B5 0 B4 0 B3 0 B2 0 B1 0 B0 0
		por			xmm1, xmm7;		// 0 B7  R7B6  0 B5  R5B4  0 B3  R3B2  0 B1  R1B0
		por			xmm2, xmm4;		// G7R7 G6R6 G5R5 G4R4 G3R3 G2R2 G1R1 G0R0
		por			xmm6, xmm5;		// B7G7 B6G6 B5G5 B4G4 B3G3 B2G2 B1G1 B0G0
		pand		xmm1, OWORD PTR[gc_dwMaskDLW];	// 00 R7B6 00 R5B4 00 R3B2 00 R1B0
		pand		xmm2, OWORD PTR[gc_dwMaskDLW];	// 00 G6R6 00 G4R4 00 G2R2 00 G0R0
		pand		xmm6, OWORD PTR[gc_dwMaskDHW];	// B7G7 00 B5G5 00 B3G3 00 B1G1 00
		movdqa		xmm4, xmm1;
		movdqa		xmm5, xmm2;
		movdqa		xmm7, xmm6;
		pslld		xmm1, 10h;		// R7B6 00 R5B4 00 R3B2 00 R1B0 00
		pslld		xmm2, 10h;		// G6R6 00 G4R4 00 G2R2 00 G0R0 00
		psrld		xmm6, 10h;		// 00 B7G7 00 B5G5 00 B3G3 00 B1G1
		shufps		xmm2, xmm2, 0b1h;	// G4R4 00 G6R6 00 G0R0 00 G2R2 00
		por			xmm1, xmm5;		// **** R5B5G4R4 **** R1B0G0R0
		por			xmm2, xmm6;		// **** G6R6B5G5 **** G2R2B1G1
		por			xmm7, xmm4;		// B7G7R7B6 **** B3G3R3B2 ****
		shufps		xmm1, xmm2, 88h;	// G6R6B5G5 G2R2B1G1 R5B5G4R4 R1B0G0R0
		shufps		xmm2, xmm7, 0fah;	// B7G7R7B6 B7G7R7B6 G6R6B5G5 G6R6B5G5
		shufps		xmm7, xmm1, 55h;	// R5B5G4R4 R5B5G4R4 B3G3R3B2 B3G3R3B2
		shufps		xmm2, xmm2, 88h;	// B7G7R7B6 G6R6B5G5 B7G7R7B6 G6R6B5G5
		shufps		xmm1, xmm7, 88h;	// R5B5G4R4 B3G3R3B2 G2R2B1G1 R1B0G0R0
		movsd		QWORD PTR[rdx], xmm2;
		movdqu		[rdi], xmm1;
		add			rsi, 10h;
		add			rax, 10h;
		add			rdi, 18h;
		add			rdx, 18h;
		sub			rcx, 01h;
		jnz			loop_a16_yuyv_to_color24_inner;
loop_m16_yuyv_to_color24_inner:
		mov			rcx, r9;
		test		rcx, 10h;
		jz			loop_m12_yuyv_to_color24_inner;
		movdqu		xmm7, [rsi];		// VYUY VYUY VYUY VYUY
		movdqa		xmm0, OWORD PTR[gc_dwMaskWLB];
		movdqa		xmm1, xmm7;
		movdqa		xmm5, OWORD PTR[gc_nOff128_6];
		pand		xmm7, xmm0;			// Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
		pand		xmm1, OWORD PTR[gc_dwMaskWHB];	// V6 U6 V4 U4 V2 U2 V0 U0
		psllw		xmm7, 5;			// 放大 5 位
		psrlw		xmm1, 2;			// 放大 6 位
		pshufd		xmm2, xmm1, 0f9h;	// V6 U6 V6 U6 V4 U4 V2 U2
		psubw		xmm1, xmm5;			// 减去偏移量 128
		psubw		xmm2, xmm5;			// 减去偏移量 128
		movdqa		xmm3, xmm1;
		movdqa		xmm5, xmm1;
		movdqa		xmm6, xmm1;
		movdqa		xmm4, xmm2;
		pand		xmm1, OWORD PTR[gc_dwMaskDLW];	// 0 U6 0 U4 0 U2 0 U0
		pand		xmm3, OWORD PTR[gc_dwMaskDHW];	// V6 0 V4 0 V2 0 V0 0
		pand		xmm2, OWORD PTR[gc_dwMaskDHW];	// V6 0 V6 0 V4 0 V2 0
		pslld		xmm4, 10h;			// U6 0 U6 0 U4 0 U2 0
		pslld		xmm5, 10h;			// U6 0 U4 0 U2 0 U0 0
		psrld		xmm6, 10h;			// 0 V6 0 V4 0 V2 0 V0
		por			xmm4, xmm1;			// U6 U6 U6 U4 U4 U2 U2 U0
		por			xmm5, xmm1;			// U6 U6 U4 U4 U2 U2 U0 U0
		por			xmm2, xmm6;			// V6 V6 V6 V4 V4 V2 V2 V0
		por			xmm3, xmm6;			// V6 V6 V4 V4 V2 V2 V0 V0
		paddw		xmm4, xmm5;			// U7 U6 U5 U4 U3 U2 U1 U0
		paddw		xmm3, xmm2;			// V7 V6 V5 V4 V3 V2 V1 V0
		mov         rax, [rsp+28h+38h];
		mov         rcx, [rsp+30h+38h];
		mov         rsi, [rsp+40h+38h];
		movdqu		xmm1, [rax];
		movdqu		xmm2, [rbx];
		movdqu		xmm5, [rcx];
		movdqu		xmm6, [rsi];
		pmulhw		xmm1, xmm3;
		pmulhw		xmm2, xmm3;
		pmulhw		xmm5, xmm4;
		pmulhw		xmm6, xmm4;
		pxor		xmm3, xmm3;			// 0
		movdqa		xmm4, OWORD PTR[gc_nRoundComp_5];
		paddsw		xmm1, xmm7;			// R
		paddsw		xmm2, xmm7;			// G
		paddsw		xmm5, xmm4;
		paddsw		xmm6, xmm7;			// B
		paddsw		xmm1, xmm4;			// R
		paddsw		xmm2, xmm5;			// G
		paddsw		xmm6, xmm4;			// B
		pmaxsw		xmm1, xmm3;			// R
		pmaxsw		xmm2, xmm3;			// G
		pmaxsw		xmm6, xmm3;			// B
		psrlw		xmm1, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm1, xmm0;		// R7 R6 R5 R4 R3 R2 R1 R0
		pminsw		xmm2, xmm0;		// G7 G6 G5 G4 G3 G2 G1 G0
		pminsw		xmm6, xmm0;		// B7 B6 B5 B4 B3 B2 B1 B0
		movdqa		xmm4, xmm1;		// R7 R6 R5 R4 R3 R2 R1 R0
		movdqa		xmm5, xmm2;		// G7 G6 G5 G4 G3 G2 G1 G0
		movdqa		xmm7, xmm6;		// B7 B6 B5 B4 B3 B2 B1 B0
		psrld		xmm1, 08h;		// 00 R7 0  00 R5 0  00 R3 0  00 R1 0
		psllw		xmm2, 08h;		// G7 0 G6 0 G5 0 G4 0 G3 0 G2 0 G1 0 G0 0
		psllw		xmm6, 08h;		// B7 0 B6 0 B5 0 B4 0 B3 0 B2 0 B1 0 B0 0
		por			xmm1, xmm7;		// 0 B7  R7B6  0 B5  R5B4  0 B3  R3B2  0 B1  R1B0
		por			xmm2, xmm4;		// G7R7 G6R6 G5R5 G4R4 G3R3 G2R2 G1R1 G0R0
		por			xmm6, xmm5;		// B7G7 B6G6 B5G5 B4G4 B3G3 B2G2 B1G1 B0G0
		pand		xmm1, OWORD PTR[gc_dwMaskDLW];	// 00 R7B6 00 R5B4 00 R3B2 00 R1B0
		pand		xmm2, OWORD PTR[gc_dwMaskDLW];	// 00 G6R6 00 G4R4 00 G2R2 00 G0R0
		pand		xmm6, OWORD PTR[gc_dwMaskDHW];	// B7G7 00 B5G5 00 B3G3 00 B1G1 00
		movdqa		xmm4, xmm1;
		movdqa		xmm5, xmm2;
		movdqa		xmm7, xmm6;
		pslld		xmm1, 10h;		// R7B6 00 R5B4 00 R3B2 00 R1B0 00
		pslld		xmm2, 10h;		// G6R6 00 G4R4 00 G2R2 00 G0R0 00
		psrld		xmm6, 10h;		// 00 B7G7 00 B5G5 00 B3G3 00 B1G1
		shufps		xmm2, xmm2, 0b1h;	// G4R4 00 G6R6 00 G0R0 00 G2R2 00
		por			xmm1, xmm5;		// **** R5B5G4R4 **** R1B0G0R0
		por			xmm2, xmm6;		// **** G6R6B5G5 **** G2R2B1G1
		por			xmm7, xmm4;		// B7G7R7B6 **** B3G3R3B2 ****
		shufps		xmm1, xmm2, 88h;	// G6R6B5G5 G2R2B1G1 R5B5G4R4 R1B0G0R0
		shufps		xmm2, xmm7, 0fah;	// B7G7R7B6 B7G7R7B6 G6R6B5G5 G6R6B5G5
		shufps		xmm7, xmm1, 55h;	// R5B5G4R4 R5B5G4R4 B3G3R3B2 B3G3R3B2
		shufps		xmm2, xmm2, 88h;	// B7G7R7B6 G6R6B5G5 B7G7R7B6 G6R6B5G5
		shufps		xmm1, xmm7, 88h;	// R5B5G4R4 B3G3R3B2 G2R2B1G1 R1B0G0R0
		movsd		QWORD PTR[rdx], xmm2;
		movdqu		[rdi], xmm1;
		jmp			loop_end_yuyv_to_color24_inner;
loop_m12_yuyv_to_color24_inner:
		cmp			rcx, 0ch;
		jnz			loop_m8_yuyv_to_color24_inner;
		sub			rax, 08h;
		movsd		xmm7, QWORD PTR[rsi];		// 0000 0000 VYUY VYUY
		movss		xmm6, DWORD PTR[rax];		// 最后两个像素 0000 0000 0000 VYUY
		shufps		xmm6, xmm6, 00h;
		movlhps		xmm7, xmm6;			// VYUY VYUY VYUY VYUY
		movdqa		xmm0, OWORD PTR[gc_dwMaskWLB];
		movdqa		xmm1, xmm7;
		movdqa		xmm5, OWORD PTR[gc_nOff128_6];
		pand		xmm7, xmm0;			// Y5 Y4 Y5 Y4 Y3 Y2 Y1 Y0
		pand		xmm1, OWORD PTR[gc_dwMaskWHB];	// V4 U4 V4 U4 V2 U2 V0 U0
		psllw		xmm7, 5;			// 放大 5 位
		psrlw		xmm1, 2;			// 放大 6 位
		pshufd		xmm2, xmm1, 0f9h;	// V4 U4 V4 U4 V4 U4 V2 U2
		psubw		xmm1, xmm5;			// 减去偏移量 128
		psubw		xmm2, xmm5;			// 减去偏移量 128
		movdqa		xmm3, xmm1;
		movdqa		xmm5, xmm1;
		movdqa		xmm6, xmm1;
		movdqa		xmm4, xmm2;
		pand		xmm1, OWORD PTR[gc_dwMaskDLW];	// 0 U4 0 U4 0 U2 0 U0
		pand		xmm3, OWORD PTR[gc_dwMaskDHW];	// V4 0 V4 0 V2 0 V0 0
		pand		xmm2, OWORD PTR[gc_dwMaskDHW];	// V4 0 V4 0 V4 0 V2 0
		pslld		xmm4, 10h;			// U4 0 U4 0 U4 0 U2 0
		pslld		xmm5, 10h;			// U4 0 U4 0 U2 0 U0 0
		psrld		xmm6, 10h;			// 0 V4 0 V4 0 V2 0 V0
		por			xmm4, xmm1;			// U4 U4 U4 U4 U4 U2 U2 U0
		por			xmm5, xmm1;			// U4 U4 U4 U4 U2 U2 U0 U0
		por			xmm2, xmm6;			// V4 V4 V4 V4 V4 V2 V2 V0
		por			xmm3, xmm6;			// V4 V4 V4 V4 V2 V2 V0 V0
		paddw		xmm4, xmm5;			// U5 U4 U5 U4 U3 U2 U1 U0
		paddw		xmm3, xmm2;			// V5 V4 V5 V4 V3 V2 V1 V0
		mov         rax, [rsp+28h+38h];
		mov         rcx, [rsp+30h+38h];
		mov         rsi, [rsp+40h+38h];
		movdqu		xmm1, [rax];
		movdqu		xmm2, [rbx];
		movdqu		xmm5, [rcx];
		movdqu		xmm6, [rsi];
		pmulhw		xmm1, xmm3;
		pmulhw		xmm2, xmm3;
		pmulhw		xmm5, xmm4;
		pmulhw		xmm6, xmm4;
		pxor		xmm3, xmm3;			// 0
		movdqa		xmm4, OWORD PTR[gc_nRoundComp_5];
		paddsw		xmm1, xmm7;			// R
		paddsw		xmm2, xmm7;			// G
		paddsw		xmm5, xmm4;
		paddsw		xmm6, xmm7;			// B
		paddsw		xmm1, xmm4;			// R
		paddsw		xmm2, xmm5;			// G
		paddsw		xmm6, xmm4;			// B
		pmaxsw		xmm1, xmm3;			// R
		pmaxsw		xmm2, xmm3;			// G
		pmaxsw		xmm6, xmm3;			// B
		psrlw		xmm1, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm1, xmm0;			// R5 R4 R5 R4 R3 R2 R1 R0
		pminsw		xmm2, xmm0;			// G5 G4 G5 G4 G3 G2 G1 G0
		pminsw		xmm6, xmm0;			// B5 B4 B5 B4 B3 B2 B1 B0
		movdqa		xmm4, xmm1;		// R5 R4 R5 R4 R3 R2 R1 R0
		movdqa		xmm5, xmm2;		// G5 G4 G5 G4 G3 G2 G1 G0
		movdqa		xmm7, xmm6;		// B5 B4 B5 B4 B3 B2 B1 B0
		psrld		xmm1, 08h;		// 00 R5 0  00 R5 0  00 R3 0  00 R1 0
		psllw		xmm2, 08h;		// G5 0 G4 0 G5 0 G4 0 G3 0 G2 0 G1 0 G0 0
		psllw		xmm6, 08h;		// B5 0 B4 0 B5 0 B4 0 B3 0 B2 0 B1 0 B0 0
		por			xmm1, xmm7;		// 0 B5  R5B4  0 B5  R5B4  0 B3  R3B2  0 B1  R1B0
		por			xmm2, xmm4;		// G5R5 G4R4 G5R5 G4R4 G3R3 G2R2 G1R1 G0R0
		por			xmm6, xmm5;		// B5G5 B4G4 B5G5 B4G4 B3G3 B2G2 B1G1 B0G0
		pand		xmm1, OWORD PTR[gc_dwMaskDLW];	// 00 R5B4 00 R5B4 00 R3B2 00 R1B0
		pand		xmm2, OWORD PTR[gc_dwMaskDLW];	// 00 G4R4 00 G4R4 00 G2R2 00 G0R0
		pand		xmm6, OWORD PTR[gc_dwMaskDHW];	// B5G5 00 B5G5 00 B3G3 00 B1G1 00
		movdqa		xmm4, xmm1;
		movdqa		xmm5, xmm2;
		movdqa		xmm7, xmm6;
		pslld		xmm1, 10h;		// R5B4 00 R5B4 00 R3B2 00 R1B0 00
		pslld		xmm2, 10h;		// G4R4 00 G4R4 00 G2R2 00 G0R0 00
		psrld		xmm6, 10h;		// 00 B5G5 00 B5G5 00 B3G3 00 B1G1
		shufps		xmm2, xmm2, 0b1h;	// G4R4 00 G4R4 00 G0R0 00 G2R2 00
		por			xmm1, xmm5;		// **** R5B5G4R4 **** R1B0G0R0
		por			xmm2, xmm6;		// **** **B5G5 **** G2R2B1G1
		por			xmm7, xmm4;		// **** **** B3G3R3B2 ****
		shufps		xmm2, xmm2, 0ah;	// G2R2B1G1 G2R2B1G1 **B5G5 **B5G5
		shufps		xmm7, xmm1, 5ah;	// R5B5G4R4 R5B5G4R4 B3G3R3B2 B3G3R3B2
		shufps		xmm1, xmm2, 0a0h;	// G2R2B1G1 G2R2B1G1 R1B0G0R0 R1B0G0R0
		shufps		xmm1, xmm7, 88h;	// R5B5G4R4 B3G3R3B2 G2R2B1G1 R1B0G0R0
		movss		DWORD PTR[rdx], xmm2;
		movdqu		[rdi], xmm1;
		jmp			loop_end_yuyv_to_color24_inner;
loop_m8_yuyv_to_color24_inner:
		test		rcx, 08h;
		jz			loop_m4_yuyv_to_color24_inner;
		movsd		xmm7, QWORD PTR[rsi];		// 0000 0000 VYUY VYUY
		movdqa		xmm0, OWORD PTR[gc_dwMaskWLB];
		movdqa		xmm1, xmm7;
		pand		xmm7, xmm0;			// 0 0 0 0 Y3 Y2 Y1 Y0
		pand		xmm1, OWORD PTR[gc_dwMaskWHB];	// 0 0 0 0 V2 U2 V0 U0
		psllw		xmm7, 5;			// 放大 5 位
		psrlw		xmm1, 2;			// 放大 6 位
		psubw		xmm1, OWORD PTR[gc_nOff128_6];	// 减去偏移量 128
		pshufd		xmm2, xmm1, 0f5h;	// 0 0 0 0 V2 U2 V2 U2
		movdqa		xmm3, xmm1;
		movdqa		xmm5, xmm1;
		movdqa		xmm6, xmm1;
		movdqa		xmm4, xmm2;
		pand		xmm1, OWORD PTR[gc_dwMaskDLW];	// 0 0 0 0 0 U2 0 U0
		pand		xmm3, OWORD PTR[gc_dwMaskDHW];	// 0 0 0 0 V2 0 V0 0
		pand		xmm2, OWORD PTR[gc_dwMaskDHW];	// 0 0 0 0 V2 0 V2 0
		pslld		xmm4, 10h;			// 0 0 0 0 U2 0 U2 0
		pslld		xmm5, 10h;			// 0 0 0 0 U2 0 U0 0
		psrld		xmm6, 10h;			// 0 0 0 0 0 V2 0 V0
		por			xmm4, xmm1;			// 0 0 0 0 U2 U2 U2 U0
		por			xmm5, xmm1;			// 0 0 0 0 U2 U2 U0 U0
		por			xmm2, xmm6;			// 0 0 0 0 V2 V2 V2 V0
		por			xmm3, xmm6;			// 0 0 0 0 V2 V2 V0 V0
		paddw		xmm4, xmm5;			// 0 0 0 0 U3 U2 U1 U0
		paddw		xmm3, xmm2;			// 0 0 0 0 V3 V2 V1 V0
		mov         rax, [rsp+28h+38h];
		mov         rcx, [rsp+30h+38h];
		mov         rsi, [rsp+40h+38h];
		movdqu		xmm1, [rax];
		movdqu		xmm2, [rbx];
		movdqu		xmm5, [rcx];
		movdqu		xmm6, [rsi];
		pmulhw		xmm1, xmm3;
		pmulhw		xmm2, xmm3;
		pmulhw		xmm5, xmm4;
		pmulhw		xmm6, xmm4;
		pxor		xmm3, xmm3;			// 0
		movdqa		xmm4, OWORD PTR[gc_nRoundComp_5];
		paddsw		xmm1, xmm7;			// R
		paddsw		xmm2, xmm7;			// G
		paddsw		xmm5, xmm4;
		paddsw		xmm6, xmm7;			// B
		paddsw		xmm1, xmm4;			// R
		paddsw		xmm2, xmm5;			// G
		paddsw		xmm6, xmm4;			// B
		pmaxsw		xmm1, xmm3;			// R
		pmaxsw		xmm2, xmm3;			// G
		pmaxsw		xmm6, xmm3;			// B
		psrlw		xmm1, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm1, xmm0;			// 0 0 0 0 R3 R2 R1 R0
		pminsw		xmm2, xmm0;			// 0 0 0 0 G3 G2 G1 G0
		pminsw		xmm6, xmm0;			// 0 0 0 0 B3 B2 B1 B0
		movdqa		xmm4, xmm1;			// 0 0 0 0 R3 R2 R1 R0
		shufps		xmm1, xmm2, 44h;	// G3 G2 G1 G0 R3 R2 R1 R0
		shufps		xmm2, xmm6, 44h;	// B3 B2 B1 B0 G3 G2 G1 G0
		psrld		xmm4, 08h;			// 0 0 0 0 0 R3 0 R1
		psllw		xmm2, 08h;			// B3 B2 B1 B0 G3 G2 G1 G0
		por			xmm4, xmm6;			// 0 0 0 0 * R3B2 * R1B0
		por			xmm2, xmm1;			// B3G3 * B1G1 * * G2R2 * G0R0
		movdqa		xmm6, xmm4;
		movdqa		xmm1, xmm2;
		shufps		xmm4, xmm2, 50h;	// * G2R2 * G2R2 * R1B0 * R1B0
		pand		xmm1, OWORD PTR[gc_dwMaskDLW];	// 0 * 0 * 0 * 0 G0R0
		pand		xmm2, OWORD PTR[gc_dwMaskDHW];	// B3G3 0 B1G1 0 * 0 * 0
		pand		xmm6, OWORD PTR[gc_dwMaskDLW];	// 0 0 0 0 0 R3B2 0 R1B0
		movdqa		xmm5, xmm2;
		pslld		xmm4, 10h;			// G2R2 0 G2R2 0 R1B0 0 R1B0 0
		psrld		xmm5, 10h;			// 0 0 0 B1G1 0 0 0 0
		shufps		xmm6, xmm6, 55h;	// 0 R3B2 0 R3B2 0 R3B2 0 R3B2
		shufps		xmm1, xmm5, 0a0h;	// 0 B1G1 0 B1G1 0 G0R0 0 G0R0
		shufps		xmm2, xmm2, 0ffh;	// B3G3 0 B3G3 0 B3G3 0 B3G3 0
		por			xmm1, xmm4;			// G2R2B1G1 G2R2B1G1 R1B0G0R0 R1B0G0R0
		por			xmm2, xmm6;			// B3G3R3B2 B3G3R3B2 B3G3R3B2 B3G3R3B2
		shufps		xmm1, xmm1, 88h;	// G2R2B1G1 R1B0G0R0 G2R2B1G1 R1B0G0R0
		sub			rdx, 08h;
		movsd		QWORD PTR[rdi], xmm1;
		movss		DWORD PTR[rdx], xmm2;
		jmp			loop_end_yuyv_to_color24_inner;
loop_m4_yuyv_to_color24_inner:
		test		rcx, 04h;
		jz			loop_end_yuyv_to_color24_inner;
		movss		xmm7, DWORD PTR[rsi];		// 0000 0000 0000 VYUY
		movdqa		xmm0, OWORD PTR[gc_dwMaskWLB];
		movdqa		xmm1, xmm7;
		pand		xmm7, xmm0;			// 0 0 0 0 0 0 Y1 Y0
		pand		xmm1, OWORD PTR[gc_dwMaskWHB];	// 0 0 0 0 0 0 V0 U0
		psllw		xmm7, 5;			// 放大 5 位
		psrlw		xmm1, 2;			// 放大 6 位
		psubw		xmm1, OWORD PTR[gc_nOff128_6];	// 减去偏移量 128
		movdqa		xmm3, xmm1;
		psrld		xmm1, 10h;			// 0 0 0 0 0 0 0 V0
		mov         rax, [rsp+28h+38h];
		mov         rcx, [rsp+30h+38h];
		mov         rsi, [rsp+40h+38h];
		movdqu		xmm1, [rax];
		movdqu		xmm2, [rbx];
		movdqu		xmm5, [rcx];
		movdqu		xmm6, [rsi];
		pmulhw		xmm4, xmm1;
		pmulhw		xmm2, xmm1;
		pmulhw		xmm5, xmm3;
		pmulhw		xmm6, xmm3;
		pxor		xmm1, xmm1;			// 0
		movdqa		xmm3, OWORD PTR[gc_nRoundComp_5];
		paddsw		xmm4, xmm7;			// R
		paddsw		xmm2, xmm7;			// G
		paddsw		xmm5, xmm3;
		paddsw		xmm6, xmm7;			// B
		paddsw		xmm4, xmm3;			// R
		paddsw		xmm2, xmm5;			// G
		paddsw		xmm6, xmm3;			// B
		pmaxsw		xmm4, xmm1;			// R
		pmaxsw		xmm2, xmm1;			// G
		pmaxsw		xmm6, xmm1;			// B
		psrlw		xmm4, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm4, xmm0;			// 0 0 0 0 0 0 R1 R0
		pminsw		xmm2, xmm0;			// 0 0 0 0 0 0 G1 G0
		pminsw		xmm6, xmm0;			// 0 0 0 0 0 0 B1 B0
		movdqa		xmm5, xmm4;			// 0 0 0 0 0 0 R1 R0
		shufps		xmm4, xmm2, 00h;	// G1 G0 G1 G0 R1 R0 R1 R0
		shufps		xmm2, xmm6, 00h;	// B1 B0 B1 B0 G1 G0 G1 G0
		psrld		xmm5, 08h;			// 0 0 0 0 0 0 0 R1
		psllw		xmm2, 08h;			// B1 B0 B1 B0 G1 G0 G1 G0
		por			xmm5, xmm6;			// 0 0 0 0 0 0 * R1B0
		por			xmm2, xmm4;			// B1G1 * B1G1 * * G0R0 * G0R0
		movdqa		xmm1, xmm2;
		pslld		xmm5, 10h;			// 0 0 0 0 0 0 R1B0 0
		psrld		xmm2, 10h;			// 0 B1G1 0 B1G1 0 * 0 *
		pand		xmm1, OWORD PTR[gc_dwMaskDLW];	// 0 * 0 * 0 * 0 G0R0
		por			xmm1, xmm5;			// * * * R1B0G0R0
		shufps		xmm1, xmm2, 0a0h;	// **B1G1 **B1G1 R1B0G0R0 R1B0G0R0
		shufps		xmm1, xmm1, 88h;	// **B1G1 R1B0G0R0 **B1G1 R1B0G0R0
		movsd		QWORD PTR[rdi], xmm1;
loop_end_yuyv_to_color24_inner:
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 10h]
		add			rsp,20h
		pop			rbx
		pop			rdi
		pop			rsi
		ret

;--------------------------------------------------------------------------
;void color24_to_yuyv_inner_x64(OUT void* yuyv,		// 目标 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
;						   IN void* color24,	// *G@ 输入，存放方式 *0 G0 @0 *1 G1 @1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
;						   IN int na12,			// 源字节宽度减 4 后除以 12 的商
;						   IN int nmp,			// 做完 na12 循环后剩余的像素'
;						   IN int* pnY24_RGB,   // 系数（通过变更 R 分量与 B 分量的系数位置可以做到 BGRA 与 RGBA 的兼容）
;						   IN int* pnY32_BGR,
;						   IN int* pnU_BGR,
;						   IN int* pnV_BGR,
;						   IN int* pnUV_BGR)
;--------------------------------------------------------------------------						   
color24_to_yuyv_inner_x64:
	;rcx			- yuyv
	;rdx			- color24
	;r8				- na12
	;r9				- nmp
	;[rsp+28h]		- pnY24_RGB
	;[rsp+30h]		- pnY32_BGR
	;[rsp+38h]		- pnU_BGR
	;[rsp+40h]		- pnV_BGR
	;[rsp+48h]		- pnUV_BGR

		push		rsi
		push		rdi
		push		rbx
		sub			rsp,20h
		movups		[rsp],xmm6
		movups		[rsp + 10h],xmm7

		mov			rdi, rcx;
		mov			rcx, r8;
		mov			rsi, rdx;
		mov         rax, [rsp+38h+38h];
		mov         rbx, [rsp+40h+38h];
		mov         rdx, [rsp+28h+38h];
		test		ecx, ecx;
		jnz			loop_a12_color24_to_yuyv_inner;
		jmp			loop_mp4_color24_to_yuyv_inner;
loop_a12_color24_to_yuyv_inner:		;// 只做前四个像素
		movdqu		xmm7, [rsi];		// RBGR BGRB GRBG RBGR
		movdqu		xmm4, [rax];      	// U
		movdqu		xmm5, [rbx];  	    // V
		movdqu		xmm6, [rdx];  	    // YrYg YbYr YgYb YrYg
		movdqa		xmm1, xmm7;
		movdqa		xmm2, xmm7;
		movdqa		xmm3, xmm7;			// 为了取 U0V0 和 U2V2
		pxor		xmm0, xmm0;
		pand		xmm1, OWORD PTR[gc_dwMaskWLB];	// ** G3B2 R2G1 B0R0
		pand		xmm2, OWORD PTR[gc_dwMaskWHB];	// ** B3R3 G2B1 R1G0
		punpcklbw	xmm0, xmm7;			// G2R2 ** *B0 G0R0
		pslld		xmm3, 08h;			// ** *B2 ** **
		pshufd		xmm7, xmm6, 12h;	// YrYg YgYb YrYg YbYr
		shufps		xmm3, xmm0, 0fah;	// G2R2 G2R2 *B2 *B2
		shufps		xmm0, xmm3, 24h;	// *B2 G2R2 *B0 G0R0
		psllw		xmm1, 07h;			// 放大 7 位
		psrlw		xmm2, 01h;
		psrlw		xmm0, 01h;
		pmulhw		xmm6, xmm2;			// 还放大 5 位
		pmulhw		xmm7, xmm1;			// * *  YG3 YB2  YR2 YG1  YB0 YR0
		pmulhw		xmm4, xmm0;
		pmulhw		xmm5, xmm0;
		movdqa		xmm2, xmm6;			// * *  YB3 YR3  YG2 YB1  YR1 YG0
		paddsw		xmm6, xmm7;			// * *  YG3+YB3 *  YR2+YG2 YG1+YB1  * YR0+YG0
		shufps		xmm2, xmm7, 88h;	// YG3 YB2  YB0 YR0  YB3 YR3  YR1 YG0
		movdqa		xmm3, xmm6;
		movdqa		xmm7, xmm2;
		pand		xmm2, OWORD PTR[gc_dwMaskDLW];	// 0 YB2  0 *  0 YR3  0 *
		psrld		xmm3, 10h;			// 0 *  0 YG3+YB3  0 YR2+YG2  0 *
		psrld		xmm7, 10h;			// 0 *  0 YB0  * 0  0 YR1
		shufps		xmm6, xmm3, 94h;	// 0 YG3+YB3  0 YR2+YG2  * YG1+YB1  * YR0+YG0
		shufps		xmm7, xmm2, 72h;	// 0 YR3  0 YB2  0 YR1  0 YB0
		pshufd		xmm1, xmm4, 0b1h;
		pshufd		xmm2, xmm5, 0b1h;
		paddsw		xmm7, xmm6;			// 0 Y3  0 Y2  * Y1  * Y0
		paddsw		xmm1, xmm4;			// UR + UB
		paddsw		xmm2, xmm5;			// VR + VB
		movdqa		xmm0, OWORD PTR[gc_nRoundComp_5];
		movdqa		xmm3, OWORD PTR[gc_dwMaskDLB];
		movdqa		xmm6, OWORD PTR[gc_nOff128_5];
		psrld		xmm4, 10h;
		psrld		xmm5, 10h;
		paddsw		xmm1, xmm0;
		paddsw		xmm2, xmm0;
		paddsw		xmm7, xmm0;
		paddsw		xmm4, xmm6;
		paddsw		xmm5, xmm6;
		paddsw		xmm1, xmm4;
		paddsw		xmm2, xmm5;
		psrlw		xmm1, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm7, 05h;
		shufps		xmm1, xmm1, 88h;	// U2 U0 U2 U0
		shufps		xmm2, xmm2, 88h;	// V2 V0 V2 V0
		pminsw		xmm7, xmm3;
		pminsw		xmm1, xmm3;
		pminsw		xmm2, xmm3;
		pand		xmm7, xmm3;			// 0 Y3 0 Y2 0 Y1 0 Y0
		pand		xmm1, xmm3;			// 0 U2 0 U0 0 U2 0 U0
		pand		xmm2, xmm3;			// 0 V2 0 V0 0 V2 0 V0
		packssdw	xmm7, xmm7;			// Y3Y2 Y1Y0 Y3Y2 Y1Y0
		pslld		xmm1, 08h;			// 00 U2 0  00 U0 0  00 U2 0  00 U0 0
		pslld		xmm2, 18h;			// V2 000  V0 000  V2 000  V0 000
		por			xmm7, xmm1;			// 0 Y3U2Y2  0 Y1U0Y0  0 Y3U2Y2  0 Y1U0Y0
		por			xmm7, xmm2;			// V2Y3U2Y2 V0Y1U0Y0 V2Y3U2Y2 V0Y1U0Y0
		movsd		QWORD PTR[rdi], xmm7;
		add			rsi, 0ch;			// 12 字节
		add			rdi, 08h;
		sub			rcx, 01h;
		jnz			loop_a12_color24_to_yuyv_inner;
loop_mp4_color24_to_yuyv_inner:		;// 剩余四个像素
		mov			rcx, r9;
		test		rcx, 04h;
		jz			loop_mp3_color24_to_yuyv_inner;
		push		rax;
		mov			rax, rsi;
		add			rax, 08h;
		movsd		xmm7, QWORD PTR[rsi];		// 0000 0000 GRBG RBGR
		movss		xmm6, DWORD PTR[rax];		// 0000 0000 0000 BGRB
		pop			rax;
		pxor		xmm0, xmm0;
		movlhps		xmm7, xmm6;			// 0000 BGRB GRBG RBGR
		movdqu		xmm4, [rax];	    // U
		movdqu		xmm5, [rbx];	    // V
		movdqu		xmm6, [rdx];	    // YrYg YbYr YgYb YrYg
		movdqa		xmm1, xmm7;
		movdqa		xmm2, xmm7;
		movdqa		xmm3, xmm7;			// 为了取 U0V0 和 U2V2
		pand		xmm1, OWORD PTR[gc_dwMaskWLB];		// ** G3B2 R2G1 B0R0
		pand		xmm2, OWORD PTR[gc_dwMaskWHB];	// ** B3R3 G2B1 R1G0
		punpcklbw	xmm0, xmm7;			// G2R2 ** *B0 G0R0
		pslld		xmm3, 08h;			// ** *B2 ** **
		pshufd		xmm7, xmm6, 12h;	// YrYg YgYb YrYg YbYr
		shufps		xmm3, xmm0, 0fah;	// G2R2 G2R2 *B2 *B2
		shufps		xmm0, xmm3, 024h;	// *B2 G2R2 *B0 G0R0
		psllw		xmm1, 07h;			// 放大 7 位
		psrlw		xmm2, 01h;
		psrlw		xmm0, 01h;
		pmulhw		xmm6, xmm2;			// 还放大 5 位
		pmulhw		xmm7, xmm1;			// * *  YG3 YB2  YR2 YG1  YB0 YR0
		pmulhw		xmm4, xmm0;
		pmulhw		xmm5, xmm0;
		movdqa		xmm2, xmm6;			// * *  YB3 YR3  YG2 YB1  YR1 YG0
		paddsw		xmm6, xmm7;			// * *  YG3+YB3 *  YR2+YG2 YG1+YB1  * YR0+YG0
		shufps		xmm2, xmm7, 88h;	// YG3 YB2  YB0 YR0  YB3 YR3  YR1 YG0
		movdqa		xmm3, xmm6;
		movdqa		xmm7, xmm2;
		pand		xmm2, OWORD PTR[gc_dwMaskDLW];	// 0 YB2  0 *  0 YR3  0 *
		psrld		xmm3, 10h;			// 0 *  0 YG3+YB3  0 YR2+YG2  0 *
		psrld		xmm7, 10h;			// 0 *  0 YB0  * 0  0 YR1
		shufps		xmm6, xmm3, 94h;	// 0 YG3+YB3  0 YR2+YG2  * YG1+YB1  * YR0+YG0
		shufps		xmm7, xmm2, 72h;	// 0 YR3  0 YB2  0 YR1  0 YB0
		pshufd		xmm1, xmm4, 0b1h;
		pshufd		xmm2, xmm5, 0b1h;
		paddsw		xmm7, xmm6;			// 0 Y3  0 Y2  * Y1  * Y0
		paddsw		xmm1, xmm4;			// UR + UB
		paddsw		xmm2, xmm5;			// VR + VB
		movdqa		xmm0, OWORD PTR[gc_nRoundComp_5];
		movdqa		xmm3, OWORD PTR[gc_dwMaskDLB];
		movdqa		xmm6, OWORD PTR[gc_nOff128_5];
		psrld		xmm4, 10h;
		psrld		xmm5, 10h;
		paddsw		xmm1, xmm0;
		paddsw		xmm2, xmm0;
		paddsw		xmm7, xmm0;
		paddsw		xmm4, xmm6;
		paddsw		xmm5, xmm6;
		paddsw		xmm1, xmm4;
		paddsw		xmm2, xmm5;
		psrlw		xmm1, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm7, 05h;
		shufps		xmm1, xmm1, 88h;	// U2 U0 U2 U0
		shufps		xmm2, xmm2, 88h;	// V2 V0 V2 V0
		pminsw		xmm7, xmm3;
		pminsw		xmm1, xmm3;
		pminsw		xmm2, xmm3;
		pand		xmm7, xmm3;			// 0 Y3 0 Y2 0 Y1 0 Y0
		pand		xmm1, xmm3;			// 0 U2 0 U0 0 U2 0 U0
		pand		xmm2, xmm3;			// 0 V2 0 V0 0 V2 0 V0
		packssdw	xmm7, xmm7;			// Y3Y2 Y1Y0 Y3Y2 Y1Y0
		pslld		xmm1, 08h;			// 00 U2 0  00 U0 0  00 U2 0  00 U0 0
		pslld		xmm2, 18h;			// V2 000  V0 000  V2 000  V0 000
		por			xmm7, xmm1;			// 0 Y3U2Y2  0 Y1U0Y0  0 Y3U2Y2  0 Y1U0Y0
		por			xmm7, xmm2;			// V2Y3U2Y2 V0Y1U0Y0 V2Y3U2Y2 V0Y1U0Y0
		movsd		QWORD PTR[rdi], xmm7;
		jmp			loop_end_color24_to_yuyv_inner;
loop_mp3_color24_to_yuyv_inner:		;// 剩余三个像素
		cmp			rcx, 03h;
		jnz			loop_mp2_color24_to_yuyv_inner;
		push		rax;
		mov			rax, rsi;
		add			rax, 08h;
		movsd		xmm7, QWORD PTR[rsi];		// 0000 0000 GRBG RBGR
		movss		xmm6, DWORD PTR[rax];		// 0000 0000 0000 000B
		pop			rax;
		pshufd		xmm0, xmm7, 9ah;	// 0000 GRBG 0000 0000
		shufps		xmm6, xmm6, 45h;	// 0000 000B 0000 0000
		pand		xmm0, OWORD PTR[gc_dwMaskDHW];	// 0000 GR00 0000 0000
		pand		xmm6, OWORD PTR[gc_dwMaskDLB];
		movdqa		xmm1, xmm6;
		pslld		xmm0, 08h;			// 0000 0GR0 0000 0000
		pslld		xmm1, 18h;			// 0000 B000 0000 0000
		por			xmm7, xmm6;
		por			xmm0, xmm1;
		por			xmm7, xmm0;			// 0000 BGRB GRBG RBGR
		movdqu		xmm4, [rax];  	    // U
		movdqu		xmm5, [rbx];  	    // V
		movdqu		xmm6, [rdx];  	    // YrYg YbYr YgYb YrYg
		pxor		xmm0, xmm0;
		movdqa		xmm1, xmm7;
		movdqa		xmm2, xmm7;
		movdqa		xmm3, xmm7;			// 为了取 U0V0 和 U2V2
		pand		xmm1, OWORD PTR[gc_dwMaskWLB];		// ** G3B2 R2G1 B0R0
		pand		xmm2, OWORD PTR[gc_dwMaskWHB];	// ** B3R3 G2B1 R1G0
		punpcklbw	xmm0, xmm7;			// G2R2 ** *B0 G0R0
		pslld		xmm3, 08h;			// ** *B2 ** **
		pshufd		xmm7, xmm6, 12h;	// YrYg YgYb YrYg YbYr
		shufps		xmm3, xmm0, 0fah;	// G2R2 G2R2 *B2 *B2
		shufps		xmm0, xmm3, 24h;	// *B2 G2R2 *B0 G0R0
		psllw		xmm1, 07h;			// 放大 7 位
		psrlw		xmm2, 01h;
		psrlw		xmm0, 01h;
		pmulhw		xmm6, xmm2;			// 还放大 5 位
		pmulhw		xmm7, xmm1;			// * *  YG3 YB2  YR2 YG1  YB0 YR0
		pmulhw		xmm4, xmm0;
		pmulhw		xmm5, xmm0;
		movdqa		xmm2, xmm6;			// * *  YB3 YR3  YG2 YB1  YR1 YG0
		paddsw		xmm6, xmm7;			// * *  YG3+YB3 *  YR2+YG2 YG1+YB1  * YR0+YG0
		shufps		xmm2, xmm7, 88h;	// YG3 YB2  YB0 YR0  YB3 YR3  YR1 YG0
		movdqa		xmm3, xmm6;
		movdqa		xmm7, xmm2;
		pand		xmm2, OWORD PTR[gc_dwMaskDLW];	// 0 YB2  0 *  0 YR3  0 *
		psrld		xmm3, 10h;			// 0 *  0 YG3+YB3  0 YR2+YG2  0 *
		psrld		xmm7, 10h;			// 0 *  0 YB0  * 0  0 YR1
		shufps		xmm6, xmm3, 94h;	// 0 YG3+YB3  0 YR2+YG2  * YG1+YB1  * YR0+YG0
		shufps		xmm7, xmm2, 72h;	// 0 YR3  0 YB2  0 YR1  0 YB0
		pshufd		xmm1, xmm4, 0b1h;
		pshufd		xmm2, xmm5, 0b1h;
		paddsw		xmm7, xmm6;			// 0 Y3  0 Y2  * Y1  * Y0
		paddsw		xmm1, xmm4;			// UR + UB
		paddsw		xmm2, xmm5;			// VR + VB
		movdqa		xmm0, OWORD PTR[gc_nRoundComp_5];
		movdqa		xmm3, OWORD PTR[gc_dwMaskDLB];
		movdqa		xmm6, OWORD PTR[gc_nOff128_5];
		psrld		xmm4, 10h;
		psrld		xmm5, 10h;
		paddsw		xmm1, xmm0;
		paddsw		xmm2, xmm0;
		paddsw		xmm7, xmm0;
		paddsw		xmm4, xmm6;
		paddsw		xmm5, xmm6;
		paddsw		xmm1, xmm4;
		paddsw		xmm2, xmm5;
		psrlw		xmm1, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm7, 05h;
		shufps		xmm1, xmm1, 88h;	// U2 U0 U2 U0
		shufps		xmm2, xmm2, 88h;	// V2 V0 V2 V0
		pminsw		xmm7, xmm3;
		pminsw		xmm1, xmm3;
		pminsw		xmm2, xmm3;
		pand		xmm7, xmm3;			// 0 Y3 0 Y2 0 Y1 0 Y0
		pand		xmm1, xmm3;			// 0 U2 0 U0 0 U2 0 U0
		pand		xmm2, xmm3;			// 0 V2 0 V0 0 V2 0 V0
		packssdw	xmm7, xmm7;			// Y3Y2 Y1Y0 Y3Y2 Y1Y0
		pslld		xmm1, 08h;			// 00 U2 0  00 U0 0  00 U2 0  00 U0 0
		pslld		xmm2, 18h;			// V2 000  V0 000  V2 000  V0 000
		por			xmm7, xmm1;			// 0 Y3U2Y2  0 Y1U0Y0  0 Y3U2Y2  0 Y1U0Y0
		por			xmm7, xmm2;			// V2Y3U2Y2 V0Y1U0Y0 V2Y3U2Y2 V0Y1U0Y0
		movsd		QWORD PTR[rdi], xmm7;
		jmp			loop_end_color24_to_yuyv_inner;
loop_mp2_color24_to_yuyv_inner:
		mov         rax, [rsp+30h+38h];//pnY32_BGR;
		mov         rbx, [rsp+48h+38h];//pnUV_BGR;
		test		ecx, 02h;
		jz			loop_mp1_color24_to_yuyv_inner;
		movsd		xmm7, QWORD PTR[rsi];		// 0000 0000 00BG RBGR
		pshufd		xmm1, xmm7, 0a6h;	// 0000 0000 00BG 0000
		pshufd		xmm2, xmm7, 8ah;	// 0000 0000 RBGR 0000
		pshufd		xmm3, xmm7, 0a8h;	// 0000 0000 0000 RBGR
		shufps		xmm7, xmm7, 00h;	// RBGR RBGR RBGR RBGR    UV
		pslld		xmm1, 08h;			// 0000 0000 0BG0 0000
		psrld		xmm2, 18h;			// 0000 0000 000R 0000
		por			xmm3, xmm1;
		por			xmm2, xmm1;			// 0000 0000 0BGR RBGR
		pxor		xmm0, xmm0;			// 0000 0000 0000 0000
		pxor		xmm1, xmm1;
		movdqu		xmm5, [rax];	    // Y
		movdqu		xmm6, [rbx];	    // UV
		movdqa		xmm3, OWORD PTR[gc_dwMaskDLB];
		punpcklbw	xmm0, xmm7;			// 00B0 G0R0 R0B0 G0R0
		punpcklbw	xmm1, xmm7;			// R0B0 G0R0 R0B0 G0R0
		psrlw		xmm0, 01h;			// 放大 7 位
		psrlw		xmm1, 01h;
		pmulhw		xmm5, xmm0;			// 还放大 5 位
		pmulhw		xmm6, xmm1;
		pshufd		xmm0, xmm5, 0b1h;
		pshufd		xmm1, xmm6, 0b1h;
		paddsw		xmm0, xmm5;			// YR + YB
		paddsw		xmm1, xmm6;			// VR + VB  UR + UB
		psrld		xmm5, 10h;
		psrld		xmm6, 10h;
		paddsw		xmm0, xmm2;
		paddsw		xmm1, xmm2;
		paddsw		xmm6, OWORD PTR[gc_nOff128_5];
		paddsw		xmm0, xmm5;			// Y
		paddsw		xmm1, xmm6;			// U V
		psrlw		xmm0, 05h;
		psrlw		xmm1, 05h;
		pminsw		xmm0, xmm3;
		pminsw		xmm1, xmm3;
		shufps		xmm0, xmm0, 88h;	// Y1 Y0 Y1 Y0
		shufps		xmm1, xmm1, 88h;	// V0 U0 V0 U0
		pand		xmm0, xmm3;			// 0 Y1 0 Y0 0 Y1 0 Y0
		pand		xmm1, xmm3;			// 0 V0 0 U0 0 V0 0 U0
		packssdw	xmm0, xmm0;			// Y1Y0 Y1Y0 Y1Y0 Y1Y0
		packssdw	xmm1, xmm1;			// V0U0 V0U0 V0U0 V0U0
		pslld		xmm1, 08h;
		por			xmm0, xmm1;			// V0Y1U0Y0
		movss		DWORD PTR[rdi], xmm0;
		jmp			loop_end_color24_to_yuyv_inner;
loop_mp1_color24_to_yuyv_inner:
		test		rcx, 01h;
		jz			loop_end_color24_to_yuyv_inner;
		movss		xmm7, DWORD PTR[rsi];		// 0000 0000 0000 0BGR
		movdqa		xmm2, OWORD PTR[gc_nRoundComp_5];
		movdqu		xmm5, [rax];	    // Y
		movdqu		xmm6, [rbx]; 	    // UV
		shufps		xmm7, xmm7, 0f0h;
		pxor		xmm0, xmm0;
		punpcklbw	xmm0, xmm7;			// A0B0 G0R0 A0B0 G0R0
		psrlw		xmm0, 01h;			// 放大 7 位
		pmulhw		xmm5, xmm0;			// 还放大 5 位
		pmulhw		xmm6, xmm0;
		pshufd		xmm0, xmm5, 0b1h;
		pshufd		xmm1, xmm6, 0b1h;
		paddsw		xmm0, xmm5;			// YR + YB
		paddsw		xmm1, xmm6;			// VR + VB  UR + UB
		psrld		xmm5, 10h;
		psrld		xmm6, 10h;
		paddsw		xmm0, xmm2;
		paddsw		xmm1, xmm2;
		paddsw		xmm6, OWORD PTR[gc_nOff128_5];
		paddsw		xmm0, xmm5;			// Y
		paddsw		xmm1, xmm6;			// U V
		psrlw		xmm0, 05h;
		psrlw		xmm1, 05h;
		pminsw		xmm0, xmm3;
		pminsw		xmm1, xmm3;
		shufps		xmm0, xmm0, 88h;	// Y1 Y0 Y1 Y0
		shufps		xmm1, xmm1, 88h;	// V0 U0 V0 U0
		pand		xmm0, xmm3;			// 0 Y1 0 Y0 0 Y1 0 Y0
		pand		xmm1, xmm3;			// 0 V0 0 U0 0 V0 0 U0
		packssdw	xmm0, xmm0;			// Y1Y0 Y1Y0 Y1Y0 Y1Y0
		packssdw	xmm1, xmm1;			// V0U0 V0U0 V0U0 V0U0
		pslld		xmm1, 08h;
		por			xmm0, xmm1;			// V0Y1U0Y0
		movss		DWORD PTR[rdi], xmm0;
loop_end_color24_to_yuyv_inner:
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 10h]
		add			rsp,20h
		pop			rbx
		pop			rdi
		pop			rsi
		ret
		
;--------------------------------------------------------------------------
;void yuyv_to_bgra_inner_x64(OUT void* bgra,     // BGRA 缓存，存放方式 B0 G0 R0 A0 B1 G1 R1 A1 ……
;						IN void* yuyv, 		// 源 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
;						IN int na16,		// 源字节宽度减 4 后除以 16 的商与余数
;						IN int nm16,
;						IN int* pnR_V,      // 系数（无法做到 RGBA 与 BGRA 的兼容）
;						IN int* pnG_U,
;						IN int* pnG_V,
;						IN int* pnB_U)
;--------------------------------------------------------------------------						
yuyv_to_bgra_inner_x64:
	;rcx			- bgra
	;rdx			- yuyv
	;r8				- na16
	;r9				- nm16
	;[rsp+28h]		- pnR_V
	;[rsp+30h]		- pnG_U
	;[rsp+38h]		- pnG_V
	;[rsp+40h]		- pnB_U

		push		rsi
		push		rdi
		push		rbx
		sub			rsp,20h
		movups		[rsp],xmm6
		movups		[rsp + 10h],xmm7
		
		mov			rax, rdx;
		mov			rsi, rdx;
		mov			rdi, rcx;
		mov			rcx, r8;
		add			rax, 10h;			// 下一个 VYUV 用于 UV 插值
		
		mov         rbx, [rsp+38h+38h];
		mov         rdx, [rsp+30h+38h];
		test		rcx, rcx;
		jnz			loop_a16_yuyv_to_bgra_inner;
		jmp			loop_m16_yuyv_to_bgra_inner;
loop_a16_yuyv_to_bgra_inner:
		movdqu		xmm7, [rsi];		// VYUY VYUY VYUY VYUY
		movss		xmm6, DWORD PTR[rax];		// 下一个 VYUV 用于 UV 插值
		movdqa		xmm1, xmm7;
		movdqa		xmm0, OWORD PTR[gc_dwMaskWLB];
		movdqa		xmm3, OWORD PTR[gc_dwMaskWHB];
		movdqa		xmm4, OWORD PTR[gc_dwMaskSpecUV];
		movdqa		xmm5, OWORD PTR[gc_nOff128_6];
		pand		xmm7, xmm0;			// Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
		pand		xmm1, xmm3;			// V6 U6 V4 U4 V2 U2 V0 U0
		pand		xmm6, xmm3;			// 0 0 0 0 0 0 V8 U8
		psrlw		xmm1, 2;			// 放大 6 位
		psrlw		xmm6, 2;			// 放大 6 位
		psllw		xmm7, 5;			// 放大 5 位
		pshufd		xmm2, xmm1, 39h;	// V0 U0 V6 U6 V4 U4 V2 U2
		shufps		xmm6, xmm6, 15h;	// V8 U8 0 0 0 0 0 0
		pand		xmm2, xmm4;			// 0 0 V6 U6 V4 U4 V2 U2
		por			xmm2, xmm6;			// V8 U8 V6 U6 V4 U4 V2 U2
		psubsw		xmm1, xmm5;			// 减去偏移量 128
		psubsw		xmm2, xmm5;			// 减去偏移量 128
		movdqa		xmm3, xmm1;
		movdqa		xmm5, xmm1;
		movdqa		xmm6, xmm1;
		movdqa		xmm4, xmm2;
		pand		xmm1, OWORD PTR[gc_dwMaskDLW];	// 0 U6 0 U4 0 U2 0 U0
		pand		xmm3, OWORD PTR[gc_dwMaskDHW];	// V6 0 V4 0 V2 0 V0 0
		pand		xmm2, OWORD PTR[gc_dwMaskDHW];	// V8 0 V6 0 V4 0 V2 0
		pslld		xmm4, 10h;			// U8 0 U6 0 U4 0 U2 0
		pslld		xmm5, 10h;			// U6 0 U4 0 U2 0 U0 0
		psrld		xmm6, 10h;			// 0 V6 0 V4 0 V2 0 V0
		por			xmm4, xmm1;			// U8 U6 U6 U4 U4 U2 U2 U0
		por			xmm5, xmm1;			// U6 U6 U4 U4 U2 U2 U0 U0
		por			xmm2, xmm6;			// V8 V6 V6 V4 V4 V2 V2 V0
		por			xmm3, xmm6;			// V6 V6 V4 V4 V2 V2 V0 V0
		paddw		xmm4, xmm5;			// U7 U6 U5 U4 U3 U2 U1 U0
		paddw		xmm3, xmm2;			// V7 V6 V5 V4 V3 V2 V1 V0
		push        rax;
		push        rsi;
		mov         rax, [rsp+38h+38h];
		mov         rsi, [rsp+50h+38h];
		movdqu		xmm1, [rax];
		movdqu		xmm2, [rbx];
		movdqu		xmm5, [rdx];
		movdqu		xmm6, [rsi];
		pop         rsi;
		pop         rax;
		pmulhw		xmm1, xmm3;
		pmulhw		xmm2, xmm3;
		pmulhw		xmm5, xmm4;
		pmulhw		xmm6, xmm4;
		pxor		xmm3, xmm3;			// 0
		movdqa		xmm4, OWORD PTR[gc_nRoundComp_5];
		paddsw		xmm1, xmm7;			// R
		paddsw		xmm2, xmm7;			// G
		paddsw		xmm5, xmm4;
		paddsw		xmm6, xmm7;			// B
		paddsw		xmm1, xmm4;			// R
		paddsw		xmm2, xmm5;			// G
		paddsw		xmm6, xmm4;			// B
		pmaxsw		xmm1, xmm3;			// R
		pmaxsw		xmm2, xmm3;			// G
		pmaxsw		xmm6, xmm3;			// B
		psrlw		xmm1, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm1, xmm0;		// R7 R6 R5 R4 R3 R2 R1 R0
		pminsw		xmm2, xmm0;		// G7 G6 G5 G4 G3 G2 G1 G0
		pminsw		xmm6, xmm0;		// B7 B6 B5 B4 B3 B2 B1 B0
		psllw		xmm2, 08h;
		por			xmm6, xmm2;		// G7B7 G6B6 G5B5 G4B4 G3B3 G2B2 G1B1 G0B0
		pxor		xmm5, xmm5;
		movdqa		xmm0, xmm6;
		movdqa		xmm7, OWORD PTR[gc_dwMaskAlpha];	// A000 A000 A000 A000
		punpcklwd	xmm0, xmm5;			// 00 G3B3 00 G2B2 00 G1B1 00 G0B0
		punpckhwd	xmm6, xmm5;			// 00 G7B7 00 G6B6 00 G5B5 00 G4B4
		punpcklwd	xmm3, xmm1;			// 0R3 00 0R2 00 0R1 00 0R0 00
		punpckhwd	xmm5, xmm1;			// 0R7 00 0R6 00 0R5 00 0R4 00
		por			xmm0, xmm3;
		por			xmm6, xmm5;
		por			xmm0, xmm7;
		por			xmm6, xmm7;
		movdqu		[rdi], xmm0;
		add			rdi, 10h;
		add			rax, 10h;
		movdqu		[rdi], xmm6;
		add			rsi, 10h;
		add			rdi, 10h;
		sub			rcx, 01h;
		jnz			loop_a16_yuyv_to_bgra_inner;
loop_m16_yuyv_to_bgra_inner:
		mov			rcx, r9;
		test		rcx, 10h;
		jz			loop_m12_yuyv_to_bgra_inner;
		movdqu		xmm7, [rsi];		// VYUY VYUY VYUY VYUY
		movdqa		xmm0, OWORD PTR[gc_dwMaskWLB];
		movdqa		xmm1, xmm7;
		movdqa		xmm5, OWORD PTR[gc_nOff128_6];
		pand		xmm7, xmm0;			// Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
		pand		xmm1, OWORD PTR[gc_dwMaskWHB];	// V6 U6 V4 U4 V2 U2 V0 U0
		psllw		xmm7, 5;			// 放大 5 位
		psrlw		xmm1, 2;			// 放大 6 位
		pshufd		xmm2, xmm1, 0f9h;	// V6 U6 V6 U6 V4 U4 V2 U2
		psubw		xmm1, xmm5;			// 减去偏移量 128
		psubw		xmm2, xmm5;			// 减去偏移量 128
		movdqa		xmm3, xmm1;
		movdqa		xmm5, xmm1;
		movdqa		xmm6, xmm1;
		movdqa		xmm4, xmm2;
		pand		xmm1, OWORD PTR[gc_dwMaskDLW];	// 0 U6 0 U4 0 U2 0 U0
		pand		xmm3, OWORD PTR[gc_dwMaskDHW];	// V6 0 V4 0 V2 0 V0 0
		pand		xmm2, OWORD PTR[gc_dwMaskDHW];	// V6 0 V6 0 V4 0 V2 0
		pslld		xmm4, 10h;			// U6 0 U6 0 U4 0 U2 0
		pslld		xmm5, 10h;			// U6 0 U4 0 U2 0 U0 0
		psrld		xmm6, 10h;			// 0 V6 0 V4 0 V2 0 V0
		por			xmm4, xmm1;			// U6 U6 U6 U4 U4 U2 U2 U0
		por			xmm5, xmm1;			// U6 U6 U4 U4 U2 U2 U0 U0
		por			xmm2, xmm6;			// V6 V6 V6 V4 V4 V2 V2 V0
		por			xmm3, xmm6;			// V6 V6 V4 V4 V2 V2 V0 V0
		paddw		xmm4, xmm5;			// U7 U6 U5 U4 U3 U2 U1 U0
		paddw		xmm3, xmm2;			// V7 V6 V5 V4 V3 V2 V1 V0
		mov         eax, [rsp+28h+38h];
		mov         esi, [rsp+40h+38h];
		movdqu		xmm1, [rax];
		movdqu		xmm2, [rbx];
		movdqu		xmm5, [rdx];
		movdqu		xmm6, [rsi];
		pmulhw		xmm1, xmm3;
		pmulhw		xmm2, xmm3;
		pmulhw		xmm5, xmm4;
		pmulhw		xmm6, xmm4;
		pxor		xmm3, xmm3;			// 0
		movdqa		xmm4, OWORD PTR[gc_nRoundComp_5];
		paddsw		xmm1, xmm7;			// R
		paddsw		xmm2, xmm7;			// G
		paddsw		xmm5, xmm4;
		paddsw		xmm6, xmm7;			// B
		paddsw		xmm1, xmm4;			// R
		paddsw		xmm2, xmm5;			// G
		paddsw		xmm6, xmm4;			// B
		pmaxsw		xmm1, xmm3;			// R
		pmaxsw		xmm2, xmm3;			// G
		pmaxsw		xmm6, xmm3;			// B
		psrlw		xmm1, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm1, xmm0;		// R7 R6 R5 R4 R3 R2 R1 R0
		pminsw		xmm2, xmm0;		// G7 G6 G5 G4 G3 G2 G1 G0
		pminsw		xmm6, xmm0;		// B7 B6 B5 B4 B3 B2 B1 B0
		psllw		xmm2, 08h;
		por			xmm6, xmm2;			// G7B7 G6B6 G5B5 G4B4 G3B3 G2B2 G1B1 G0B0
		pxor		xmm5, xmm5;
		movdqa		xmm0, xmm6;
		movdqa		xmm7, OWORD PTR[gc_dwMaskAlpha];	// A000 A000 A000 A000
		punpcklwd	xmm0, xmm5;			// 00 G3B3 00 G2B2 00 G1B1 00 G0B0
		punpckhwd	xmm6, xmm5;			// 00 G7B7 00 G6B6 00 G5B5 00 G4B4
		punpcklwd	xmm3, xmm1;			// 0R3 00 0R2 00 0R1 00 0R0 00
		punpckhwd	xmm5, xmm1;			// 0R7 00 0R6 00 0R5 00 0R4 00
		por			xmm0, xmm3;
		por			xmm6, xmm5;
		por			xmm0, xmm7;
		por			xmm6, xmm7;
		movdqu		[rdi], xmm0;
		add			rdi, 10h;
		movdqu		[rdi], xmm6;
		jmp			loop_end_yuyv_to_bgra_inner;
loop_m12_yuyv_to_bgra_inner:
		cmp			rcx, 0ch;
		jnz			loop_m8_yuyv_to_bgra_inner;
		sub			rax, 08h;
		movsd		xmm7, QWORD PTR[rsi];		// 0000 0000 VYUY VYUY
		movss		xmm6, DWORD PTR[rax];		// 最后两个像素 0000 0000 0000 VYUY
		shufps		xmm6, xmm6, 00h;
		movlhps		xmm7, xmm6;			// VYUY VYUY VYUY VYUY
		movdqa		xmm0, OWORD PTR[gc_dwMaskWLB];
		movdqa		xmm1, xmm7;
		movdqa		xmm5, OWORD PTR[gc_nOff128_6];
		pand		xmm7, xmm0;			// Y5 Y4 Y5 Y4 Y3 Y2 Y1 Y0
		pand		xmm1, OWORD PTR[gc_dwMaskWHB];	// V4 U4 V4 U4 V2 U2 V0 U0
		psllw		xmm7, 5;			// 放大 5 位
		psrlw		xmm1, 2;			// 放大 6 位
		pshufd		xmm2, xmm1, 0f9h;	// V4 U4 V4 U4 V4 U4 V2 U2
		psubw		xmm1, xmm5;			// 减去偏移量 128
		psubw		xmm2, xmm5;			// 减去偏移量 128
		movdqa		xmm3, xmm1;
		movdqa		xmm5, xmm1;
		movdqa		xmm6, xmm1;
		movdqa		xmm4, xmm2;
		pand		xmm1, OWORD PTR[gc_dwMaskDLW];	// 0 U4 0 U4 0 U2 0 U0
		pand		xmm3, OWORD PTR[gc_dwMaskDHW];	// V4 0 V4 0 V2 0 V0 0
		pand		xmm2, OWORD PTR[gc_dwMaskDHW];	// V4 0 V4 0 V4 0 V2 0
		pslld		xmm4, 10h;			// U4 0 U4 0 U4 0 U2 0
		pslld		xmm5, 10h;			// U4 0 U4 0 U2 0 U0 0
		psrld		xmm6, 10h;			// 0 V4 0 V4 0 V2 0 V0
		por			xmm4, xmm1;			// U4 U4 U4 U4 U4 U2 U2 U0
		por			xmm5, xmm1;			// U4 U4 U4 U4 U2 U2 U0 U0
		por			xmm2, xmm6;			// V4 V4 V4 V4 V4 V2 V2 V0
		por			xmm3, xmm6;			// V4 V4 V4 V4 V2 V2 V0 V0
		paddw		xmm4, xmm5;			// U5 U4 U5 U4 U3 U2 U1 U0
		paddw		xmm3, xmm2;			// V5 V4 V5 V4 V3 V2 V1 V0
		mov         rax, [rsp+28h+38h];
		mov         rsi, [rsp+40h+38h];
		movdqu		xmm1, [rax];
		movdqu		xmm2, [rbx];
		movdqu		xmm5, [rdx];
		movdqu		xmm6, [rsi];
		pmulhw		xmm1, xmm3;
		pmulhw		xmm2, xmm3;
		pmulhw		xmm5, xmm4;
		pmulhw		xmm6, xmm4;
		pxor		xmm3, xmm3;			// 0
		movdqa		xmm4, OWORD PTR[gc_nRoundComp_5];
		paddsw		xmm1, xmm7;			// R
		paddsw		xmm2, xmm7;			// G
		paddsw		xmm5, xmm4;
		paddsw		xmm6, xmm7;			// B
		paddsw		xmm1, xmm4;			// R
		paddsw		xmm2, xmm5;			// G
		paddsw		xmm6, xmm4;			// B
		pmaxsw		xmm1, xmm3;			// R
		pmaxsw		xmm2, xmm3;			// G
		pmaxsw		xmm6, xmm3;			// B
		psrlw		xmm1, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm1, xmm0;			// R5 R4 R5 R4 R3 R2 R1 R0
		pminsw		xmm2, xmm0;			// G5 G4 G5 G4 G3 G2 G1 G0
		pminsw		xmm6, xmm0;			// B5 B4 B5 B4 B3 B2 B1 B0
		psllw		xmm2, 08h;
		por			xmm6, xmm2;			// G5B5 G4B4 G5B5 G4B4 G3B3 G2B2 G1B1 G0B0
		pxor		xmm5, xmm5;
		movdqa		xmm0, xmm6;
		movdqa		xmm7, OWORD PTR[gc_dwMaskAlpha];	// A000 A000 A000 A000
		punpcklwd	xmm0, xmm5;			// 00 G3B3 00 G2B2 00 G1B1 00 G0B0
		punpckhwd	xmm6, xmm5;			// 00 G5B5 00 G4B4 00 G5B5 00 G4B4
		punpcklwd	xmm3, xmm1;			// 0R3 00 0R2 00 0R1 00 0R0 00
		punpckhwd	xmm5, xmm1;			// 0R5 00 0R4 00 0R5 00 0R4 00
		por			xmm0, xmm3;
		por			xmm6, xmm5;
		por			xmm0, xmm7;
		por			xmm6, xmm7;
		movdqu		[rdi], xmm0;
		add			rdi, 10h;
		movsd		QWORD PTR[rdi], xmm6;
		jmp			loop_end_yuyv_to_bgra_inner;
loop_m8_yuyv_to_bgra_inner:
		test		rcx, 08h;
		jz			loop_m4_yuyv_to_bgra_inner;
		movsd		xmm7, QWORD PTR[rsi];		// 0000 0000 VYUY VYUY
		movdqa		xmm0, OWORD PTR[gc_dwMaskWLB];
		movdqa		xmm1, xmm7;
		pand		xmm7, xmm0;			// 0 0 0 0 Y3 Y2 Y1 Y0
		pand		xmm1, OWORD PTR[gc_dwMaskWHB];	// 0 0 0 0 V2 U2 V0 U0
		psllw		xmm7, 5;			// 放大 5 位
		psrlw		xmm1, 2;			// 放大 6 位
		psubw		xmm1, OWORD PTR[gc_nOff128_6];	// 减去偏移量 128
		pshufd		xmm2, xmm1, 0f5h;	// 0 0 0 0 V2 U2 V2 U2
		movdqa		xmm3, xmm1;
		movdqa		xmm5, xmm1;
		movdqa		xmm6, xmm1;
		movdqa		xmm4, xmm2;
		pand		xmm1, OWORD PTR[gc_dwMaskDLW];	// 0 0 0 0 0 U2 0 U0
		pand		xmm3, OWORD PTR[gc_dwMaskDHW];	// 0 0 0 0 V2 0 V0 0
		pand		xmm2, oWORD PTR[gc_dwMaskDHW];	// 0 0 0 0 V2 0 V2 0
		pslld		xmm4, 10h;			// 0 0 0 0 U2 0 U2 0
		pslld		xmm5, 10h;			// 0 0 0 0 U2 0 U0 0
		psrld		xmm6, 10h;			// 0 0 0 0 0 V2 0 V0
		por			xmm4, xmm1;			// 0 0 0 0 U2 U2 U2 U0
		por			xmm5, xmm1;			// 0 0 0 0 U2 U2 U0 U0
		por			xmm2, xmm6;			// 0 0 0 0 V2 V2 V2 V0
		por			xmm3, xmm6;			// 0 0 0 0 V2 V2 V0 V0
		paddw		xmm4, xmm5;			// 0 0 0 0 U3 U2 U1 U0
		paddw		xmm3, xmm2;			// 0 0 0 0 V3 V2 V1 V0
		mov         rax, [rsp+28h+38h];
		mov         rsi, [rsp+40h+38h];
		movdqu		xmm1, [rax];
		movdqu		xmm2, [rbx];
		movdqu		xmm5, [rdx];
		movdqu		xmm6, [rsi];
		pmulhw		xmm1, xmm3;
		pmulhw		xmm2, xmm3;
		pmulhw		xmm5, xmm4;
		pmulhw		xmm6, xmm4;
		pxor		xmm3, xmm3;			// 0
		movdqa		xmm4, OWORD PTR[gc_nRoundComp_5];
		paddsw		xmm1, xmm7;			// R
		paddsw		xmm2, xmm7;			// G
		paddsw		xmm5, xmm4;
		paddsw		xmm6, xmm7;			// B
		paddsw		xmm1, xmm4;			// R
		paddsw		xmm2, xmm5;			// G
		paddsw		xmm6, xmm4;			// B
		pmaxsw		xmm1, xmm3;			// R
		pmaxsw		xmm2, xmm3;			// G
		pmaxsw		xmm6, xmm3;			// B
		psrlw		xmm1, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm1, xmm0;			// 0 0 0 0 R3 R2 R1 R0
		pminsw		xmm2, xmm0;			// 0 0 0 0 G3 G2 G1 G0
		pminsw		xmm6, xmm0;			// 0 0 0 0 B3 B2 B1 B0
		psllw		xmm2, 08h;
		por			xmm6, xmm2;			// 00 00 00 00 G3B3 G2B2 G1B1 G0B0
		punpcklwd	xmm6, xmm3;			// 00 G3B3 00 G2B2 00 G1B1 00 G0B0
		punpcklwd	xmm3, xmm1;			// 0B3 00 0B2 00 0B1 00 0B0 00
		por			xmm6, OWORD PTR[gc_dwMaskAlpha];
		por			xmm6, xmm3;
		movdqu		[rdi], xmm6;
		jmp			loop_end_yuyv_to_bgra_inner;
loop_m4_yuyv_to_bgra_inner:
		test		rcx, 04h;
		jz			loop_end_yuyv_to_bgra_inner;
		movss		xmm7, DWORD PTR[rsi];		// 0000 0000 0000 VYUY
		movdqa		xmm0, OWORD PTR[gc_dwMaskWLB];
		movdqa		xmm1, xmm7;
		pand		xmm7, xmm0;			// 0 0 0 0 0 0 Y1 Y0
		pand		xmm1, OWORD PTR[gc_dwMaskWHB];	// 0 0 0 0 0 0 V0 U0
		psllw		xmm7, 5;			// 放大 5 位
		psrlw		xmm1, 2;			// 放大 6 位
		psubw		xmm1, OWORD PTR[gc_nOff128_6];	// 减去偏移量 128
		movdqa		xmm3, xmm1;
		psrld		xmm1, 10h;			// 0 0 0 0 0 0 0 V0
		mov         rax, [rsp+28h+38h];
		mov         rsi, [rsp+40h+38h];
		movdqu		xmm1, [rax];
		movdqu		xmm2, [rbx];
		movdqu		xmm5, [rdx];
		movdqu		xmm6, [rsi];
		pmulhw		xmm4, xmm1;
		pmulhw		xmm2, xmm1;
		pmulhw		xmm5, xmm3;
		pmulhw		xmm6, xmm3;
		pxor		xmm1, xmm1;			// 0
		movdqa		xmm3, OWORD PTR[gc_nRoundComp_5];
		paddsw		xmm4, xmm7;			// R
		paddsw		xmm2, xmm7;			// G
		paddsw		xmm5, xmm3;
		paddsw		xmm6, xmm7;			// B
		paddsw		xmm4, xmm3;			// R
		paddsw		xmm2, xmm5;			// G
		paddsw		xmm6, xmm3;			// B
		pmaxsw		xmm4, xmm1;			// R
		pmaxsw		xmm2, xmm1;			// G
		pmaxsw		xmm6, xmm1;			// B
		psrlw		xmm4, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm4, xmm0;			// 0 0 0 0 0 0 R1 R0
		pminsw		xmm2, xmm0;			// 0 0 0 0 0 0 G1 G0
		pminsw		xmm6, xmm0;			// 0 0 0 0 0 0 B1 B0
		psllw		xmm2, 08h;
		por			xmm6, xmm2;			// 00 00 00 00 00 00 G1B1 G0B0
		punpcklwd	xmm6, xmm1;			// 00 00 00 00 00 G1B1 00 G0B0
		punpcklwd	xmm1, xmm4;			// 00 00 00 00 0R1 00 0R0 00
		por			xmm6, OWORD PTR[gc_dwMaskAlpha];
		por			xmm6, xmm1;
		movsd		QWORD PTR[rdi], xmm6;
loop_end_yuyv_to_bgra_inner:
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 10h]
		add			rsp,20h
		pop			rbx
		pop			rdi
		pop			rsi
		ret
		
;--------------------------------------------------------------------------
;void yuyv_to_rgba_inner_x64(OUT void* rgba,	        // RGBA 缓存，存放方式 R0 G0 B0 A0 R1 G1 B1 A1 ……
;						IN void* yuyv, 		// 源 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
;						IN int na16,		    // 源字节宽度减 4 后除以 16 的商与余数
;						IN int nm16,
;						IN int* pnR_V,        // 系数（无法做到 RGBA 与 BGRA 的兼容）
;						IN int* pnG_U,
;						IN int* pnG_V,
;						IN int* pnB_U)
;--------------------------------------------------------------------------		
yuyv_to_rgba_inner_x64:
	;rcx			- rgba
	;rdx			- yuyv
	;r8				- na16
	;r9				- nm16
	;[rsp+28h]		- pnR_V
	;[rsp+30h]		- pnG_U
	;[rsp+38h]		- pnG_V
	;[rsp+40h]		- pnB_U

		push		rsi
		push		rdi
		push		rbx
		sub			rsp,20h
		movups		[rsp],xmm6
		movups		[rsp + 10h],xmm7
	
		mov			rdi, rcx;
		mov			rax, rdx;
		mov			rsi, rdx;
		mov			rcx, r8;
		add			rax, 10h;			// 下一个 VYUV 用于 UV 插值
		
		mov         rbx, [rsp+38h+38h];
		mov         rdx, [rsp+30h+38h];
		test		rcx, rcx;
		jnz			loop_a16_yuyv_to_rgba_inner;
		jmp			loop_m16_yuyv_to_rgba_inner;
loop_a16_yuyv_to_rgba_inner:
		movdqu		xmm7, [esi];		// VYUY VYUY VYUY VYUY
		movss		xmm6, DWORD PTR[rax];		// 下一个 VYUV 用于 UV 插值
		movdqa		xmm1, xmm7;
		movdqa		xmm0, OWORD PTR[gc_dwMaskWLB];
		movdqa		xmm3, OWORD PTR[gc_dwMaskWHB];
		movdqa		xmm4, OWORD PTR[gc_dwMaskSpecUV];
		movdqa		xmm5, OWORD PTR[gc_nOff128_6];
		pand		xmm7, xmm0;			// Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
		pand		xmm1, xmm3;			// V6 U6 V4 U4 V2 U2 V0 U0
		pand		xmm6, xmm3;			// 0 0 0 0 0 0 V8 U8
		psrlw		xmm1, 2;			// 放大 6 位
		psrlw		xmm6, 2;			// 放大 6 位
		psllw		xmm7, 5;			// 放大 5 位
		pshufd		xmm2, xmm1, 39h;	// V0 U0 V6 U6 V4 U4 V2 U2
		shufps		xmm6, xmm6, 15h;	// V8 U8 0 0 0 0 0 0
		pand		xmm2, xmm4;			// 0 0 V6 U6 V4 U4 V2 U2
		por			xmm2, xmm6;			// V8 U8 V6 U6 V4 U4 V2 U2
		psubsw		xmm1, xmm5;			// 减去偏移量 128
		psubsw		xmm2, xmm5;			// 减去偏移量 128
		movdqa		xmm3, xmm1;
		movdqa		xmm5, xmm1;
		movdqa		xmm6, xmm1;
		movdqa		xmm4, xmm2;
		pand		xmm1, OWORD PTR[gc_dwMaskDLW];	// 0 U6 0 U4 0 U2 0 U0
		pand		xmm3, OWORD PTR[gc_dwMaskDHW];	// V6 0 V4 0 V2 0 V0 0
		pand		xmm2, OWORD PTR[gc_dwMaskDHW];	// V8 0 V6 0 V4 0 V2 0
		pslld		xmm4, 10h;			// U8 0 U6 0 U4 0 U2 0
		pslld		xmm5, 10h;			// U6 0 U4 0 U2 0 U0 0
		psrld		xmm6, 10h;			// 0 V6 0 V4 0 V2 0 V0
		por			xmm4, xmm1;			// U8 U6 U6 U4 U4 U2 U2 U0
		por			xmm5, xmm1;			// U6 U6 U4 U4 U2 U2 U0 U0
		por			xmm2, xmm6;			// V8 V6 V6 V4 V4 V2 V2 V0
		por			xmm3, xmm6;			// V6 V6 V4 V4 V2 V2 V0 V0
		paddw		xmm4, xmm5;			// U7 U6 U5 U4 U3 U2 U1 U0
		paddw		xmm3, xmm2;			// V7 V6 V5 V4 V3 V2 V1 V0
		push        rax;
		push        rsi;
		mov         rax, [rsp+38h+38h];
		mov         esi, [rsp+30h+38h];
		movdqu		xmm1, [rax];
		movdqu		xmm2, [rbx];
		movdqu		xmm5, [rdx];
		movdqu		xmm6, [rsi];
		pop         rsi;
		pop         rax;
		pmulhw		xmm1, xmm3;
		pmulhw		xmm2, xmm3;
		pmulhw		xmm5, xmm4;
		pmulhw		xmm6, xmm4;
		pxor		xmm3, xmm3;			// 0
		movdqa		xmm4, OWORD PTR[gc_nRoundComp_5];
		paddsw		xmm1, xmm7;			// R
		paddsw		xmm2, xmm7;			// G
		paddsw		xmm5, xmm4;
		paddsw		xmm6, xmm7;			// B
		paddsw		xmm1, xmm4;			// R
		paddsw		xmm2, xmm5;			// G
		paddsw		xmm6, xmm4;			// B
		pmaxsw		xmm1, xmm3;			// R
		pmaxsw		xmm2, xmm3;			// G
		pmaxsw		xmm6, xmm3;			// B
		psrlw		xmm1, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm1, xmm0;		// R7 R6 R5 R4 R3 R2 R1 R0
		pminsw		xmm2, xmm0;		// G7 G6 G5 G4 G3 G2 G1 G0
		pminsw		xmm6, xmm0;		// B7 B6 B5 B4 B3 B2 B1 B0
		psllw		xmm2, 08h;
		por			xmm1, xmm2;			// G7R7 G6R6 G5R5 G4R4 G3R3 G2R2 G1R1 G0R0
		pxor		xmm5, xmm5;
		movdqa		xmm0, xmm1;
		movdqa		xmm7, OWORD PTR[gc_dwMaskAlpha];	// A000 A000 A000 A000
		punpcklwd	xmm0, xmm5;			// 00 G3R3 00 G2R2 00 G1R1 00 G0R0
		punpckhwd	xmm1, xmm5;			// 00 G7R7 00 G6R6 00 G5R5 00 G4R4
		punpcklwd	xmm3, xmm6;			// 0B3 00 0B2 00 0B1 00 0B0 00
		punpckhwd	xmm5, xmm6;			// 0B7 00 0B6 00 0B5 00 0B4 00
		por			xmm0, xmm3;
		por			xmm1, xmm5;
		por			xmm0, xmm7;
		por			xmm1, xmm7;
		movdqu		[rdi], xmm0;
		add			edi, 10h;
		add			eax, 10h;
		movdqu		[rdi], xmm1;
		add			rsi, 10h;
		add			rdi, 10h;
		sub			rcx, 01h;
		jnz			loop_a16_yuyv_to_rgba_inner;
loop_m16_yuyv_to_rgba_inner:
		mov			rcx, r9;
		test		rcx, 10h;
		jz			loop_m12_yuyv_to_rgba_inner;
		movdqu		xmm7, [rsi];		// VYUY VYUY VYUY VYUY
		movdqa		xmm0, OWORD PTR[gc_dwMaskWLB];
		movdqa		xmm1, xmm7;
		movdqa		xmm5, OWORD PTR[gc_nOff128_6];
		pand		xmm7, xmm0;			// Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
		pand		xmm1, OWORD PTR[gc_dwMaskWHB];	// V6 U6 V4 U4 V2 U2 V0 U0
		psllw		xmm7, 5;			// 放大 5 位
		psrlw		xmm1, 2;			// 放大 6 位
		pshufd		xmm2, xmm1, 0f9h;	// V6 U6 V6 U6 V4 U4 V2 U2
		psubw		xmm1, xmm5;			// 减去偏移量 128
		psubw		xmm2, xmm5;			// 减去偏移量 128
		movdqa		xmm3, xmm1;
		movdqa		xmm5, xmm1;
		movdqa		xmm6, xmm1;
		movdqa		xmm4, xmm2;
		pand		xmm1, OWORD PTR[gc_dwMaskDLW];	// 0 U6 0 U4 0 U2 0 U0
		pand		xmm3, OWORD PTR[gc_dwMaskDHW];	// V6 0 V4 0 V2 0 V0 0
		pand		xmm2, OWORD PTR[gc_dwMaskDHW];	// V6 0 V6 0 V4 0 V2 0
		pslld		xmm4, 10h;			// U6 0 U6 0 U4 0 U2 0
		pslld		xmm5, 10h;			// U6 0 U4 0 U2 0 U0 0
		psrld		xmm6, 10h;			// 0 V6 0 V4 0 V2 0 V0
		por			xmm4, xmm1;			// U6 U6 U6 U4 U4 U2 U2 U0
		por			xmm5, xmm1;			// U6 U6 U4 U4 U2 U2 U0 U0
		por			xmm2, xmm6;			// V6 V6 V6 V4 V4 V2 V2 V0
		por			xmm3, xmm6;			// V6 V6 V4 V4 V2 V2 V0 V0
		paddw		xmm4, xmm5;			// U7 U6 U5 U4 U3 U2 U1 U0
		paddw		xmm3, xmm2;			// V7 V6 V5 V4 V3 V2 V1 V0
		mov         rax, [rsp+28h+38h];
		mov         rsi, [rsp+40h+38h];
		movdqu		xmm1, [rax];
		movdqu		xmm2, [rbx];
		movdqu		xmm5, [rdx];
		movdqu		xmm6, [rsi];
		pmulhw		xmm1, xmm3;
		pmulhw		xmm2, xmm3;
		pmulhw		xmm5, xmm4;
		pmulhw		xmm6, xmm4;
		pxor		xmm3, xmm3;			// 0
		movdqa		xmm4, OWORD PTR[gc_nRoundComp_5];
		paddsw		xmm1, xmm7;			// R
		paddsw		xmm2, xmm7;			// G
		paddsw		xmm5, xmm4;
		paddsw		xmm6, xmm7;			// B
		paddsw		xmm1, xmm4;			// R
		paddsw		xmm2, xmm5;			// G
		paddsw		xmm6, xmm4;			// B
		pmaxsw		xmm1, xmm3;			// R
		pmaxsw		xmm2, xmm3;			// G
		pmaxsw		xmm6, xmm3;			// B
		psrlw		xmm1, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm1, xmm0;		// R7 R6 R5 R4 R3 R2 R1 R0
		pminsw		xmm2, xmm0;		// G7 G6 G5 G4 G3 G2 G1 G0
		pminsw		xmm6, xmm0;		// B7 B6 B5 B4 B3 B2 B1 B0
		psllw		xmm2, 08h;
		por			xmm1, xmm2;			// G7R7 G6R6 G5R5 G4R4 G3R3 G2R2 G1R1 G0R0
		pxor		xmm5, xmm5;
		movdqa		xmm0, xmm1;
		movdqa		xmm7, OWORD PTR[gc_dwMaskAlpha];	// A000 A000 A000 A000
		punpcklwd	xmm0, xmm5;			// 00 G3R3 00 G2R2 00 G1R1 00 G0R0
		punpckhwd	xmm1, xmm5;			// 00 G7R7 00 G6R6 00 G5R5 00 G4R4
		punpcklwd	xmm3, xmm6;			// 0B3 00 0B2 00 0B1 00 0B0 00
		punpckhwd	xmm5, xmm6;			// 0B7 00 0B6 00 0B5 00 0B4 00
		por			xmm0, xmm3;
		por			xmm1, xmm5;
		por			xmm0, xmm7;
		por			xmm1, xmm7;
		movdqu		[rdi], xmm0;
		add			edi, 10h;
		movdqu		[rdi], xmm1;
		jmp			loop_end_yuyv_to_rgba_inner;
loop_m12_yuyv_to_rgba_inner:
		cmp			rcx, 0ch;
		jnz			loop_m8_yuyv_to_rgba_inner;
		sub			rax, 08h;
		movsd		xmm7, QWORD PTR[rsi];		// 0000 0000 VYUY VYUY
		movss		xmm6, DWORD PTR[rax];		// 最后两个像素 0000 0000 0000 VYUY
		shufps		xmm6, xmm6, 00h;
		movlhps		xmm7, xmm6;			// VYUY VYUY VYUY VYUY
		movdqa		xmm0, OWORD PTR[gc_dwMaskWLB];
		movdqa		xmm1, xmm7;
		movdqa		xmm5, OWORD PTR[gc_nOff128_6];
		pand		xmm7, xmm0;			// Y5 Y4 Y5 Y4 Y3 Y2 Y1 Y0
		pand		xmm1, OWORD PTR[gc_dwMaskWHB];	// V4 U4 V4 U4 V2 U2 V0 U0
		psllw		xmm7, 5;			// 放大 5 位
		psrlw		xmm1, 2;			// 放大 6 位
		pshufd		xmm2, xmm1, 0f9h;	// V4 U4 V4 U4 V4 U4 V2 U2
		psubw		xmm1, xmm5;			// 减去偏移量 128
		psubw		xmm2, xmm5;			// 减去偏移量 128
		movdqa		xmm3, xmm1;
		movdqa		xmm5, xmm1;
		movdqa		xmm6, xmm1;
		movdqa		xmm4, xmm2;
		pand		xmm1, OWORD PTR[gc_dwMaskDLW];	// 0 U4 0 U4 0 U2 0 U0
		pand		xmm3, OWORD PTR[gc_dwMaskDHW];	// V4 0 V4 0 V2 0 V0 0
		pand		xmm2, OWORD PTR[gc_dwMaskDHW];	// V4 0 V4 0 V4 0 V2 0
		pslld		xmm4, 10h;			// U4 0 U4 0 U4 0 U2 0
		pslld		xmm5, 10h;			// U4 0 U4 0 U2 0 U0 0
		psrld		xmm6, 10h;			// 0 V4 0 V4 0 V2 0 V0
		por			xmm4, xmm1;			// U4 U4 U4 U4 U4 U2 U2 U0
		por			xmm5, xmm1;			// U4 U4 U4 U4 U2 U2 U0 U0
		por			xmm2, xmm6;			// V4 V4 V4 V4 V4 V2 V2 V0
		por			xmm3, xmm6;			// V4 V4 V4 V4 V2 V2 V0 V0
		paddw		xmm4, xmm5;			// U5 U4 U5 U4 U3 U2 U1 U0
		paddw		xmm3, xmm2;			// V5 V4 V5 V4 V3 V2 V1 V0
		mov         rax, [rsp+28h+38h];
		mov         rsi, [rsp+40h+38h];
		movdqu		xmm1, [rax];
		movdqu		xmm2, [rbx];
		movdqu		xmm5, [rdx];
		movdqu		xmm6, [rsi];
		pmulhw		xmm1, xmm3;
		pmulhw		xmm2, xmm3;
		pmulhw		xmm5, xmm4;
		pmulhw		xmm6, xmm4;
		pxor		xmm3, xmm3;			// 0
		movdqa		xmm4, OWORD PTR[gc_nRoundComp_5];
		paddsw		xmm1, xmm7;			// R
		paddsw		xmm2, xmm7;			// G
		paddsw		xmm5, xmm4;
		paddsw		xmm6, xmm7;			// B
		paddsw		xmm1, xmm4;			// R
		paddsw		xmm2, xmm5;			// G
		paddsw		xmm6, xmm4;			// B
		pmaxsw		xmm1, xmm3;			// R
		pmaxsw		xmm2, xmm3;			// G
		pmaxsw		xmm6, xmm3;			// B
		psrlw		xmm1, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm1, xmm0;			// R5 R4 R5 R4 R3 R2 R1 R0
		pminsw		xmm2, xmm0;			// G5 G4 G5 G4 G3 G2 G1 G0
		pminsw		xmm6, xmm0;			// B5 B4 B5 B4 B3 B2 B1 B0
		psllw		xmm2, 08h;
		por			xmm1, xmm2;			// G5R5 G4R4 G5R5 G4R4 G3R3 G2R2 G1R1 G0R0
		pxor		xmm5, xmm5;
		movdqa		xmm0, xmm1;
		movdqa		xmm7, OWORD PTR[gc_dwMaskAlpha];	// A000 A000 A000 A000
		punpcklwd	xmm0, xmm5;			// 00 G3R3 00 G2R2 00 G1R1 00 G0R0
		punpckhwd	xmm1, xmm5;			// 00 G5R5 00 G4R4 00 G5R5 00 G4R4
		punpcklwd	xmm3, xmm6;			// 0B3 00 0B2 00 0B1 00 0B0 00
		punpckhwd	xmm5, xmm6;			// 0B5 00 0B4 00 0B5 00 0B4 00
		por			xmm0, xmm3;
		por			xmm1, xmm5;
		por			xmm0, xmm7;
		por			xmm1, xmm7;
		movdqu		[rdi], xmm0;
		add			rdi, 10h;
		movsd		QWORD PTR[rdi], xmm1;
		jmp			loop_end_yuyv_to_rgba_inner;
loop_m8_yuyv_to_rgba_inner:
		test		rcx, 08h;
		jz			loop_m4_yuyv_to_rgba_inner;
		movsd		xmm7, QWORD PTR[rsi];		// 0000 0000 VYUY VYUY
		movdqa		xmm0, OWORD PTR[gc_dwMaskWLB];
		movdqa		xmm1, xmm7;
		pand		xmm7, xmm0;			// 0 0 0 0 Y3 Y2 Y1 Y0
		pand		xmm1, OWORD PTR[gc_dwMaskWHB];	// 0 0 0 0 V2 U2 V0 U0
		psllw		xmm7, 5;			// 放大 5 位
		psrlw		xmm1, 2;			// 放大 6 位
		psubw		xmm1, OWORD PTR[gc_nOff128_6];	// 减去偏移量 128
		pshufd		xmm2, xmm1, 0f5h;	// 0 0 0 0 V2 U2 V2 U2
		movdqa		xmm3, xmm1;
		movdqa		xmm5, xmm1;
		movdqa		xmm6, xmm1;
		movdqa		xmm4, xmm2;
		pand		xmm1, OWORD PTR[gc_dwMaskDLW];	// 0 0 0 0 0 U2 0 U0
		pand		xmm3, OWORD PTR[gc_dwMaskDHW];	// 0 0 0 0 V2 0 V0 0
		pand		xmm2, OWORD PTR[gc_dwMaskDHW];	// 0 0 0 0 V2 0 V2 0
		pslld		xmm4, 10h;			// 0 0 0 0 U2 0 U2 0
		pslld		xmm5, 10h;			// 0 0 0 0 U2 0 U0 0
		psrld		xmm6, 10h;			// 0 0 0 0 0 V2 0 V0
		por			xmm4, xmm1;			// 0 0 0 0 U2 U2 U2 U0
		por			xmm5, xmm1;			// 0 0 0 0 U2 U2 U0 U0
		por			xmm2, xmm6;			// 0 0 0 0 V2 V2 V2 V0
		por			xmm3, xmm6;			// 0 0 0 0 V2 V2 V0 V0
		paddw		xmm4, xmm5;			// 0 0 0 0 U3 U2 U1 U0
		paddw		xmm3, xmm2;			// 0 0 0 0 V3 V2 V1 V0
		mov         rax, [rsp+28h+38h];
		mov         rsi, [rsp+40h+38h];
		movdqu		xmm1, [rax];
		movdqu		xmm2, [rbx];
		movdqu		xmm5, [rdx];
		movdqu		xmm6, [rsi];
		pmulhw		xmm1, xmm3;
		pmulhw		xmm2, xmm3;
		pmulhw		xmm5, xmm4;
		pmulhw		xmm6, xmm4;
		pxor		xmm3, xmm3;			// 0
		movdqa		xmm4, OWORD PTR[gc_nRoundComp_5];
		paddsw		xmm1, xmm7;			// R
		paddsw		xmm2, xmm7;			// G
		paddsw		xmm5, xmm4;
		paddsw		xmm6, xmm7;			// B
		paddsw		xmm1, xmm4;			// R
		paddsw		xmm2, xmm5;			// G
		paddsw		xmm6, xmm4;			// B
		pmaxsw		xmm1, xmm3;			// R
		pmaxsw		xmm2, xmm3;			// G
		pmaxsw		xmm6, xmm3;			// B
		psrlw		xmm1, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm1, xmm0;			// 0 0 0 0 R3 R2 R1 R0
		pminsw		xmm2, xmm0;			// 0 0 0 0 G3 G2 G1 G0
		pminsw		xmm6, xmm0;			// 0 0 0 0 B3 B2 B1 B0
		psllw		xmm2, 08h;
		por			xmm1, xmm2;			// 00 00 00 00 G3R3 G2R2 G1R1 G0R0
		punpcklwd	xmm1, xmm3;			// 00 G3R3 00 G2R2 00 G1R1 00 G0R0
		punpcklwd	xmm3, xmm6;			// 0B3 00 0B2 00 0B1 00 0B0 00
		por			xmm1, OWORD PTR[gc_dwMaskAlpha];
		por			xmm1, xmm3;
		movdqu		[rdi], xmm1;
		jmp			loop_end_yuyv_to_rgba_inner;
loop_m4_yuyv_to_rgba_inner:
		test		rcx, 04h;
		jz			loop_end_yuyv_to_rgba_inner;
		movss		xmm7, DWORD PTR[rsi];		// 0000 0000 0000 VYUY
		movdqa		xmm0, OWORD PTR[gc_dwMaskWLB];
		movdqa		xmm1, xmm7;
		pand		xmm7, xmm0;			// 0 0 0 0 0 0 Y1 Y0
		pand		xmm1, OWORD PTR[gc_dwMaskWHB];	// 0 0 0 0 0 0 V0 U0
		psllw		xmm7, 5;			// 放大 5 位
		psrlw		xmm1, 2;			// 放大 6 位
		psubw		xmm1, OWORD PTR[gc_nOff128_6];	// 减去偏移量 128
		movdqa		xmm3, xmm1;
		psrld		xmm1, 10h;			// 0 0 0 0 0 0 0 V0
		mov         rax, [rsp+28h+38h];
		mov         rsi, [rsp+40h+38h];
		movdqu		xmm1, [rax];
		movdqu		xmm2, [rbx];
		movdqu		xmm5, [rdx];
		movdqu		xmm6, [rsi];
		pmulhw		xmm4, xmm1;
		pmulhw		xmm2, xmm1;
		pmulhw		xmm5, xmm3;
		pmulhw		xmm6, xmm3;
		pxor		xmm1, xmm1;			// 0
		movdqa		xmm3, OWORD PTR[gc_nRoundComp_5];
		paddsw		xmm4, xmm7;			// R
		paddsw		xmm2, xmm7;			// G
		paddsw		xmm5, xmm3;
		paddsw		xmm6, xmm7;			// B
		paddsw		xmm4, xmm3;			// R
		paddsw		xmm2, xmm5;			// G
		paddsw		xmm6, xmm3;			// B
		pmaxsw		xmm4, xmm1;			// R
		pmaxsw		xmm2, xmm1;			// G
		pmaxsw		xmm6, xmm1;			// B
		psrlw		xmm4, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm4, xmm0;			// 0 0 0 0 0 0 R1 R0
		pminsw		xmm2, xmm0;			// 0 0 0 0 0 0 G1 G0
		pminsw		xmm6, xmm0;			// 0 0 0 0 0 0 B1 B0
		psllw		xmm2, 08h;
		por			xmm4, xmm2;			// 00 00 00 00 00 00 G1R1 G0R0
		punpcklwd	xmm4, xmm1;			// 00 00 00 00 00 G1R1 00 G0R0
		punpcklwd	xmm1, xmm6;			// 00 00 00 00 0B1 00 0B0 00
		por			xmm4, OWORD PTR[gc_dwMaskAlpha];
		por			xmm4, xmm1;
		movsd		QWORD PTR[rdi], xmm4;
loop_end_yuyv_to_rgba_inner:	
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 10h]
		add			rsp,20h
		pop			rbx
		pop			rdi
		pop			rsi
		ret
	
;--------------------------------------------------------------------------
;void yuyva_to_bgra_inner_x64(OUT void* bgra,        // BGRA 缓存，存放方式 B0 G0 R0 A0 B1 G1 R1 A1 ……
;						 IN void* yuyv, 		// 源 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
;						 IN void* alpha,		// 源 alpha 缓存，存放方式 A0 A1 A2 A3 ……
;						 IN int na16,		    // 源字节宽度减 4 后除以 16 的商与余数
;						 IN int nm16,
;						 IN int* pnR_V,         // 系数（无法做到 RGBA 与 BGRA 的兼容）
;						 IN int* pnG_U,       
;						 IN int* pnG_V,
;						 IN int* pnB_U)
;--------------------------------------------------------------------------	
yuyva_to_bgra_inner_x64:
	;rcx			- bgra
	;rdx			- yuyv
	;r8				- alpha
	;r9				- na16
	;[rsp+28h]		- nm16
	;[rsp+30h]		- pnR_V
	;[rsp+38h]		- pnG_U
	;[rsp+40h]		- pnG_V
	;[rsp+48h]		- pnB_U

		push		rsi
		push		rdi
		push		rbx
		sub			rsp,20h
		movups		[rsp],xmm6
		movups		[rsp + 10h],xmm7

		mov			rsi, rdx;
		mov			rax, rdx;
		mov			rdi, rcx;
		mov			rcx, r9;
		mov			rdx, r8;
		mov         rbx, [rsp+40h+38h];
		add			rax, 10h;			// 下一个 VYUV 用于 UV 插值
		test		rcx, rcx;
		jnz			loop_a16_yuyva_to_bgra_inner;
		jmp			loop_m16_yuyva_to_bgra_inner;
loop_a16_yuyva_to_bgra_inner:
		movdqu		xmm7, [rsi];		// VYUY VYUY VYUY VYUY
		movss		xmm6, DWORD PTR[rax];		// 下一个 VYUV 用于 UV 插值
		movdqa		xmm1, xmm7;
		movdqa		xmm0, OWORD PTR[gc_dwMaskWLB];
		movdqa		xmm3, OWORD PTR[gc_dwMaskWHB];
		movdqa		xmm4, OWORD PTR[gc_dwMaskSpecUV];
		movdqa		xmm5, OWORD PTR[gc_nOff128_6];
		pand		xmm7, xmm0;			// Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
		pand		xmm1, xmm3;			// V6 U6 V4 U4 V2 U2 V0 U0
		pand		xmm6, xmm3;			// 0 0 0 0 0 0 V8 U8
		psrlw		xmm1, 2;			// 放大 6 位
		psrlw		xmm6, 2;			// 放大 6 位
		psllw		xmm7, 5;			// 放大 5 位
		pshufd		xmm2, xmm1, 39h;	// V0 U0 V6 U6 V4 U4 V2 U2
		shufps		xmm6, xmm6, 15h;	// V8 U8 0 0 0 0 0 0
		pand		xmm2, xmm4;			// 0 0 V6 U6 V4 U4 V2 U2
		por			xmm2, xmm6;			// V8 U8 V6 U6 V4 U4 V2 U2
		psubsw		xmm1, xmm5;			// 减去偏移量 128
		psubsw		xmm2, xmm5;			// 减去偏移量 128
		movdqa		xmm3, xmm1;
		movdqa		xmm5, xmm1;
		movdqa		xmm6, xmm1;
		movdqa		xmm4, xmm2;
		pand		xmm1, OWORD PTR[gc_dwMaskDLW];	// 0 U6 0 U4 0 U2 0 U0
		pand		xmm3, OWORD PTR[gc_dwMaskDHW];	// V6 0 V4 0 V2 0 V0 0
		pand		xmm2, OWORD PTR[gc_dwMaskDHW];	// V8 0 V6 0 V4 0 V2 0
		pslld		xmm4, 10h;			// U8 0 U6 0 U4 0 U2 0
		pslld		xmm5, 10h;			// U6 0 U4 0 U2 0 U0 0
		psrld		xmm6, 10h;			// 0 V6 0 V4 0 V2 0 V0
		por			xmm4, xmm1;			// U8 U6 U6 U4 U4 U2 U2 U0
		por			xmm5, xmm1;			// U6 U6 U4 U4 U2 U2 U0 U0
		por			xmm2, xmm6;			// V8 V6 V6 V4 V4 V2 V2 V0
		por			xmm3, xmm6;			// V6 V6 V4 V4 V2 V2 V0 V0
		paddw		xmm4, xmm5;			// U7 U6 U5 U4 U3 U2 U1 U0
		paddw		xmm3, xmm2;			// V7 V6 V5 V4 V3 V2 V1 V0
		push        rsi;
		push        rax;
		push        rcx;
		mov         rax, [rsp+48h+38h];pnR_V;
		mov         rcx, [rsp+50h+38h];pnG_U;
		mov         rsi, [rsp+60h+38h];pnB_U;
		movdqu		xmm1, [rax];
		movdqu		xmm2, [rbx];
		movdqu		xmm5, [rcx];
		movdqu		xmm6, [rsi];
		pop         rcx;
		pop         rax;
		pop         rsi;
		pmulhw		xmm1, xmm3;
		pmulhw		xmm2, xmm3;
		pmulhw		xmm5, xmm4;
		pmulhw		xmm6, xmm4;
		pxor		xmm3, xmm3;			// 0
		movdqa		xmm4, OWORD PTR[gc_nRoundComp_5];
		paddsw		xmm1, xmm7;			// R
		paddsw		xmm2, xmm7;			// G
		paddsw		xmm5, xmm4;
		paddsw		xmm6, xmm7;			// B
		paddsw		xmm1, xmm4;			// R
		paddsw		xmm2, xmm5;			// G
		paddsw		xmm6, xmm4;			// B
		pmaxsw		xmm1, xmm3;			// R
		pmaxsw		xmm2, xmm3;			// G
		pmaxsw		xmm6, xmm3;			// B
		psrlw		xmm1, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm1, xmm0;			// R7 R6 R5 R4 R3 R2 R1 R0
		pminsw		xmm2, xmm0;			// G7 G6 G5 G4 G3 G2 G1 G0
		pminsw		xmm6, xmm0;			// B7 B6 B5 B4 B3 B2 B1 B0
		psllw		xmm2, 08h;
		movsd		xmm7, QWORD PTR[rdx];		// 0000 0000 A7A6A5A4 A3A2A1A0
		por			xmm6, xmm2;			// G7B7 G6B6 G5B5 G4B4 G3B3 G2B2 G1B1 G0B0
		pxor		xmm2, xmm2;
		pxor		xmm4, xmm4;
		pxor		xmm5, xmm5;
		movdqa		xmm0, xmm6;
		punpcklbw	xmm2, xmm7;			// A7 A6 A5 A4 A3 A2 A1 A0
		punpcklwd	xmm3, xmm1;			// 0R3 00 0R2 00 0R1 00 0R0 00
		punpcklwd	xmm0, xmm5;			// 00 G3B3 00 G2B2 00 G1B1 00 G0B0
		punpckhwd	xmm6, xmm5;			// 00 G7B7 00 G6B6 00 G5B5 00 G4B4
		pxor		xmm7, xmm7;
		punpcklwd	xmm4, xmm2;			// A3 000 A2 000 A1 000 A0 000
		punpckhwd	xmm5, xmm1;			// 0R7 00 0R6 00 0R5 00 0R4 00
		punpckhwd	xmm7, xmm2;			// A7 000 A6 000 A5 000 A4 000
		por			xmm0, xmm3;
		por			xmm6, xmm5;
		por			xmm0, xmm4;
		por			xmm6, xmm7;
		movdqu		[rdi], xmm0;
		add			rax, 10h;
		add			rdi, 10h;
		add			rsi, 10h;
		movdqu		[rdi], xmm6;
		add			rdx, 08h;
		add			rdi, 10h;
		sub			rcx, 01h;
		jnz			loop_a16_yuyva_to_bgra_inner;
loop_m16_yuyva_to_bgra_inner:
		mov			rcx, [rsp+28h+38h];
		test		rcx, 10h;
		jz			loop_m12_yuyva_to_bgra_inner;
		movdqu		xmm7, [rsi];		// VYUY VYUY VYUY VYUY
		movdqa		xmm0, OWORD PTR[gc_dwMaskWLB];
		movdqa		xmm1, xmm7;
		movdqa		xmm5, OWORD PTR[gc_nOff128_6];
		pand		xmm7, xmm0;			// Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
		pand		xmm1, OWORD PTR[gc_dwMaskWHB];	// V6 U6 V4 U4 V2 U2 V0 U0
		psllw		xmm7, 5;			// 放大 5 位
		psrlw		xmm1, 2;			// 放大 6 位
		pshufd		xmm2, xmm1, 0f9h;	// V6 U6 V6 U6 V4 U4 V2 U2
		psubw		xmm1, xmm5;			// 减去偏移量 128
		psubw		xmm2, xmm5;			// 减去偏移量 128
		movdqa		xmm3, xmm1;
		movdqa		xmm5, xmm1;
		movdqa		xmm6, xmm1;
		movdqa		xmm4, xmm2;
		pand		xmm1, OWORD PTR[gc_dwMaskDLW];	// 0 U6 0 U4 0 U2 0 U0
		pand		xmm3, OWORD PTR[gc_dwMaskDHW];	// V6 0 V4 0 V2 0 V0 0
		pand		xmm2, OWORD PTR[gc_dwMaskDHW];	// V6 0 V6 0 V4 0 V2 0
		pslld		xmm4, 10h;			// U6 0 U6 0 U4 0 U2 0
		pslld		xmm5, 10h;			// U6 0 U4 0 U2 0 U0 0
		psrld		xmm6, 10h;			// 0 V6 0 V4 0 V2 0 V0
		por			xmm4, xmm1;			// U6 U6 U6 U4 U4 U2 U2 U0
		por			xmm5, xmm1;			// U6 U6 U4 U4 U2 U2 U0 U0
		por			xmm2, xmm6;			// V6 V6 V6 V4 V4 V2 V2 V0
		por			xmm3, xmm6;			// V6 V6 V4 V4 V2 V2 V0 V0
		paddw		xmm4, xmm5;			// U7 U6 U5 U4 U3 U2 U1 U0
		paddw		xmm3, xmm2;			// V7 V6 V5 V4 V3 V2 V1 V0
		mov         rax, [rsp+30h+38h];		//pnR_V;
		mov         rcx, [rsp+38h+38h];		//pnG_U;
		mov         rsi, [rsp+48h+38h];		//pnB_U;
		movdqu		xmm1, [rax];
		movdqu		xmm2, [rbx];
		movdqu		xmm5, [rcx];
		movdqu		xmm6, [rsi];
		pmulhw		xmm1, xmm3;
		pmulhw		xmm2, xmm3;
		pmulhw		xmm5, xmm4;
		pmulhw		xmm6, xmm4;
		pxor		xmm3, xmm3;			// 0
		movdqa		xmm4, OWORD PTR[gc_nRoundComp_5];
		paddsw		xmm1, xmm7;			// R
		paddsw		xmm2, xmm7;			// G
		paddsw		xmm5, xmm4;
		paddsw		xmm6, xmm7;			// B
		paddsw		xmm1, xmm4;			// R
		paddsw		xmm2, xmm5;			// G
		paddsw		xmm6, xmm4;			// B
		pmaxsw		xmm1, xmm3;			// R
		pmaxsw		xmm2, xmm3;			// G
		pmaxsw		xmm6, xmm3;			// B
		psrlw		xmm1, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm1, xmm0;		// R7 R6 R5 R4 R3 R2 R1 R0
		pminsw		xmm2, xmm0;		// G7 G6 G5 G4 G3 G2 G1 G0
		pminsw		xmm6, xmm0;		// B7 B6 B5 B4 B3 B2 B1 B0
		psllw		xmm2, 08h;
		movsd		xmm7, QWORD PTR[rdx];		// 0000 0000 A7A6A5A4 A3A2A1A0
		por			xmm6, xmm2;			// G7B7 G6B6 G5B5 G4B4 G3B3 G2B2 G1B1 G0B0
		pxor		xmm2, xmm2;
		pxor		xmm4, xmm4;
		pxor		xmm5, xmm5;
		movdqa		xmm0, xmm6;
		punpcklbw	xmm2, xmm7;			// A7 A6 A5 A4 A3 A2 A1 A0
		punpcklwd	xmm3, xmm1;			// 0R3 00 0R2 00 0R1 00 0R0 00
		punpcklwd	xmm0, xmm5;			// 00 G3B3 00 G2B2 00 G1B1 00 G0B0
		punpckhwd	xmm6, xmm5;			// 00 G7B7 00 G6B6 00 G5B5 00 G4B4
		pxor		xmm7, xmm7;
		punpcklwd	xmm4, xmm2;			// A3 000 A2 000 A1 000 A0 000
		punpckhwd	xmm5, xmm1;			// 0R7 00 0R6 00 0R5 00 0R4 00
		punpckhwd	xmm7, xmm2;			// A7 000 A6 000 A5 000 A4 000
		por			xmm0, xmm3;
		por			xmm6, xmm5;
		por			xmm0, xmm4;
		por			xmm6, xmm7;
		movdqu		[rdi], xmm0;
		add			rdi, 10h;
		movdqu		[rdi], xmm6;
		jmp			loop_end_yuyva_to_bgra_inner;
loop_m12_yuyva_to_bgra_inner:
		cmp			rcx, 0ch;
		jnz			loop_m8_yuyva_to_bgra_inner;
		mov			rcx, rdx;
		sub			rax, 08h;
		add			rdx, 04h;
		movsd		xmm7, QWORD PTR[rsi];		// 0000 0000 VYUY VYUY
		movss		xmm6, DWORD PTR[rax];		// 最后两个像素 0000 0000 0000 VYUY
		shufps		xmm6, xmm6, 00h;
		movlhps		xmm7, xmm6;			// VYUY VYUY VYUY VYUY
		movdqa		xmm0, OWORD PTR[gc_dwMaskWLB];
		movdqa		xmm1, xmm7;
		movdqa		xmm5, OWORD PTR[gc_nOff128_6];
		pand		xmm7, xmm0;			// Y5 Y4 Y5 Y4 Y3 Y2 Y1 Y0
		pand		xmm1, OWORD PTR[gc_dwMaskWHB];	// V4 U4 V4 U4 V2 U2 V0 U0
		psllw		xmm7, 5;			// 放大 5 位
		psrlw		xmm1, 2;			// 放大 6 位
		pshufd		xmm2, xmm1, 0f9h;	// V4 U4 V4 U4 V4 U4 V2 U2
		psubw		xmm1, xmm5;			// 减去偏移量 128
		psubw		xmm2, xmm5;			// 减去偏移量 128
		movdqa		xmm3, xmm1;
		movdqa		xmm5, xmm1;
		movdqa		xmm6, xmm1;
		movdqa		xmm4, xmm2;
		pand		xmm1, OWORD PTR[gc_dwMaskDLW];	// 0 U4 0 U4 0 U2 0 U0
		pand		xmm3, OWORD PTR[gc_dwMaskDHW];	// V4 0 V4 0 V2 0 V0 0
		pand		xmm2, OWORD PTR[gc_dwMaskDHW];	// V4 0 V4 0 V4 0 V2 0
		pslld		xmm4, 10h;			// U4 0 U4 0 U4 0 U2 0
		pslld		xmm5, 10h;			// U4 0 U4 0 U2 0 U0 0
		psrld		xmm6, 10h;			// 0 V4 0 V4 0 V2 0 V0
		por			xmm4, xmm1;			// U4 U4 U4 U4 U4 U2 U2 U0
		por			xmm5, xmm1;			// U4 U4 U4 U4 U2 U2 U0 U0
		por			xmm2, xmm6;			// V4 V4 V4 V4 V4 V2 V2 V0
		por			xmm3, xmm6;			// V4 V4 V4 V4 V2 V2 V0 V0
		paddw		xmm4, xmm5;			// U5 U4 U5 U4 U3 U2 U1 U0
		paddw		xmm3, xmm2;			// V5 V4 V5 V4 V3 V2 V1 V0
		push        rcx;
		mov         rax, [rsp+38h+38h];		//pnR_V;
		mov         rcx, [rsp+40h+38h];		//pnG_U;
		mov         rsi, [rsp+50h+38h];		//pnB_U;
		movdqu		xmm1, [rax];
		movdqu		xmm2, [rbx];
		movdqu		xmm5, [rcx];
		movdqu		xmm6, [rsi];
		pop         rcx;
		pmulhw		xmm1, xmm3;
		pmulhw		xmm2, xmm3;
		pmulhw		xmm5, xmm4;
		pmulhw		xmm6, xmm4;
		pxor		xmm3, xmm3;			// 0
		movdqa		xmm4, OWORD PTR[gc_nRoundComp_5];
		paddsw		xmm1, xmm7;			// R
		paddsw		xmm2, xmm7;			// G
		paddsw		xmm5, xmm4;
		paddsw		xmm6, xmm7;			// B
		paddsw		xmm1, xmm4;			// R
		paddsw		xmm2, xmm5;			// G
		paddsw		xmm6, xmm4;			// B
		pmaxsw		xmm1, xmm3;			// R
		pmaxsw		xmm2, xmm3;			// G
		pmaxsw		xmm6, xmm3;			// B
		psrlw		xmm1, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm1, xmm0;			// R5 R4 R5 R4 R3 R2 R1 R0
		pminsw		xmm2, xmm0;			// G5 G4 G5 G4 G3 G2 G1 G0
		pminsw		xmm6, xmm0;			// B5 B4 B5 B4 B3 B2 B1 B0
		psllw		xmm2, 08h;
		movss		xmm7, DWORD PTR[rcx];		// 0000 0000 0000 A3A2A1A0
		mov			ax, [rdx];			// 00A5A4
		por			xmm6, xmm2;			// G5B5 G4B4 G5B5 G4B4 G3B3 G2B2 G1B1 G0B0
		movd		xmm0, rax;			// 0000 0000 0000 00A5A4
		shufps		xmm0, xmm0, 01h;
		pxor		xmm2, xmm2;
		pxor		xmm4, xmm4;
		pxor		xmm5, xmm5;
		por			xmm7, xmm0;			// **** **** 00A5A4 A3A2A1A0
		movdqa		xmm0, xmm6;
		punpcklbw	xmm2, xmm7;			// 0 0 A5 A4 A3 A2 A1 A0
		punpcklwd	xmm3, xmm1;			// 0R3 00 0R2 00 0R1 00 0R0 00
		punpcklwd	xmm0, xmm5;			// 00 G3B3 00 G2B2 00 G1B1 00 G0B0
		punpckhwd	xmm6, xmm5;			// 00 G5B5 00 G4B4 00 G5B5 00 G4B4
		pxor		xmm7, xmm7;
		punpcklwd	xmm4, xmm2;			// A3 000 A2 000 A1 000 A0 000
		punpckhwd	xmm5, xmm1;			// 0R5 00 0R4 00 0R5 00 0R4 00
		punpckhwd	xmm7, xmm2;			// 0000 0000 A5 000 A4 000
		por			xmm0, xmm3;
		por			xmm6, xmm5;
		por			xmm0, xmm4;
		por			xmm6, xmm7;
		movdqu		[rdi], xmm0;
		add			rdi, 10h;
		movsd		QWORD PTR[rdi], xmm6;
		jmp			loop_end_yuyva_to_bgra_inner;
loop_m8_yuyva_to_bgra_inner:
		test		rcx, 08h;
		jz			loop_m4_yuyva_to_bgra_inner;
		movsd		xmm7, QWORD PTR[rsi];		// 0000 0000 VYUY VYUY
		movdqa		xmm0, OWORD PTR[gc_dwMaskWLB];
		movdqa		xmm1, xmm7;
		pand		xmm7, xmm0;			// 0 0 0 0 Y3 Y2 Y1 Y0
		pand		xmm1, OWORD PTR[gc_dwMaskWHB];	// 0 0 0 0 V2 U2 V0 U0
		psllw		xmm7, 5;			// 放大 5 位
		psrlw		xmm1, 2;			// 放大 6 位
		psubw		xmm1, OWORD PTR[gc_nOff128_6];	// 减去偏移量 128
		pshufd		xmm2, xmm1, 0f5h;	// 0 0 0 0 V2 U2 V2 U2
		movdqa		xmm3, xmm1;
		movdqa		xmm5, xmm1;
		movdqa		xmm6, xmm1;
		movdqa		xmm4, xmm2;
		pand		xmm1, OWORD PTR[gc_dwMaskDLW];	// 0 0 0 0 0 U2 0 U0
		pand		xmm3, OWORD PTR[gc_dwMaskDHW];	// 0 0 0 0 V2 0 V0 0
		pand		xmm2, OWORD PTR[gc_dwMaskDHW];	// 0 0 0 0 V2 0 V2 0
		pslld		xmm4, 10h;			// 0 0 0 0 U2 0 U2 0
		pslld		xmm5, 10h;			// 0 0 0 0 U2 0 U0 0
		psrld		xmm6, 10h;			// 0 0 0 0 0 V2 0 V0
		por			xmm4, xmm1;			// 0 0 0 0 U2 U2 U2 U0
		por			xmm5, xmm1;			// 0 0 0 0 U2 U2 U0 U0
		por			xmm2, xmm6;			// 0 0 0 0 V2 V2 V2 V0
		por			xmm3, xmm6;			// 0 0 0 0 V2 V2 V0 V0
		paddw		xmm4, xmm5;			// 0 0 0 0 U3 U2 U1 U0
		paddw		xmm3, xmm2;			// 0 0 0 0 V3 V2 V1 V0
		mov         rax, [rsp+30h+38h];		//pnR_V;
		mov         rcx, [rsp+38h+38h];		//pnG_U;
		mov         rsi, [rsp+48h+38h];		//pnB_U;
		movdqu		xmm1, [rax];
		movdqu		xmm2, [rbx];
		movdqu		xmm5, [rcx];
		movdqu		xmm6, [rsi];
		pmulhw		xmm1, xmm3;
		pmulhw		xmm2, xmm3;
		pmulhw		xmm5, xmm4;
		pmulhw		xmm6, xmm4;
		pxor		xmm3, xmm3;			// 0
		movdqa		xmm4, OWORD PTR[gc_nRoundComp_5];
		paddsw		xmm1, xmm7;			// R
		paddsw		xmm2, xmm7;			// G
		paddsw		xmm5, xmm4;
		paddsw		xmm6, xmm7;			// B
		paddsw		xmm1, xmm4;			// R
		paddsw		xmm2, xmm5;			// G
		paddsw		xmm6, xmm4;			// B
		pmaxsw		xmm1, xmm3;			// R
		pmaxsw		xmm2, xmm3;			// G
		pmaxsw		xmm6, xmm3;			// B
		psrlw		xmm1, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm1, xmm0;			// 0 0 0 0 R3 R2 R1 R0
		pminsw		xmm2, xmm0;			// 0 0 0 0 G3 G2 G1 G0
		pminsw		xmm6, xmm0;			// 0 0 0 0 B3 B2 B1 B0
		psllw		xmm2, 08h;
		movss		xmm7, DWORD PTR[rdx];		// 0000 0000 0000 A3A2A1A0
		por			xmm6, xmm2;			// 00 00 00 00 G3B3 G2B2 G1B1 G0B0
		pxor		xmm2, xmm2;
		pxor		xmm4, xmm4;
		punpcklbw	xmm2, xmm7;			// 0 0 0 0 A3 A2 A1 A0
		punpcklwd	xmm6, xmm4;			// 00 G3B3 00 G2B2 00 G1B1 00 G0B0
		punpcklwd	xmm3, xmm1;			// 0R3 00 0R2 00 0R1 00 0R0 00
		punpcklwd	xmm4, xmm2;			// A3 000 A2 000 A1 000 A0 000
		por			xmm6, xmm3;
		por			xmm6, xmm4;
		movdqu		[rdi], xmm6;
		jmp			loop_end_yuyva_to_bgra_inner;
loop_m4_yuyva_to_bgra_inner:
		test		rcx, 04h;
		jz			loop_end_yuyva_to_bgra_inner;
		movss		xmm7, DWORD PTR[rsi];		// 0000 0000 0000 VYUY
		movdqa		xmm0, OWORD PTR[gc_dwMaskWLB];
		movdqa		xmm1, xmm7;
		pand		xmm7, xmm0;			// 0 0 0 0 0 0 Y1 Y0
		pand		xmm1, OWORD PTR[gc_dwMaskWHB];	// 0 0 0 0 0 0 V0 U0
		psllw		xmm7, 5;			// 放大 5 位
		psrlw		xmm1, 2;			// 放大 6 位
		psubw		xmm1, OWORD PTR[gc_nOff128_6];	// 减去偏移量 128
		movdqa		xmm3, xmm1;
		psrld		xmm1, 10h;			// 0 0 0 0 0 0 0 V0
		mov         rax, [rsp+30h+38h];	//pnR_V;
		mov         rcx, [rsp+38h+38h];	//pnG_U;
		mov         rsi, [rsp+48h+38h];	//pnB_U;
		movdqu		xmm1, [rax];
		movdqu		xmm2, [rbx];
		movdqu		xmm5, [rcx];
		movdqu		xmm6, [rsi];
		pmulhw		xmm4, xmm1;
		pmulhw		xmm2, xmm1;
		pmulhw		xmm5, xmm3;
		pmulhw		xmm6, xmm3;
		pxor		xmm1, xmm1;			// 0
		movdqa		xmm3, OWORD PTR[gc_nRoundComp_5];
		paddsw		xmm4, xmm7;			// R
		paddsw		xmm2, xmm7;			// G
		paddsw		xmm5, xmm3;
		paddsw		xmm6, xmm7;			// B
		paddsw		xmm4, xmm3;			// R
		paddsw		xmm2, xmm5;			// G
		paddsw		xmm6, xmm3;			// B
		pmaxsw		xmm4, xmm1;			// R
		pmaxsw		xmm2, xmm1;			// G
		pmaxsw		xmm6, xmm1;			// B
		psrlw		xmm4, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm4, xmm0;			// 0 0 0 0 0 0 R1 R0
		pminsw		xmm2, xmm0;			// 0 0 0 0 0 0 G1 G0
		pminsw		xmm6, xmm0;			// 0 0 0 0 0 0 B1 B0
		psllw		xmm2, 08h;
		mov			ax, [rdx];			// 00A1A0
		por			xmm6, xmm2;			// 00 00 00 00 00 00 G1B1 G0B0
		movd		xmm7, rax;			// 0000 0000 0000 00A1A0
		pxor		xmm2, xmm2;
		pxor		xmm3, xmm3;
		punpcklbw	xmm2, xmm7;			// 0 0 0 0 0 0 A1 A0
		punpcklwd	xmm6, xmm1;			// 00 00 00 00 00 G1B1 00 G0B0
		punpcklwd	xmm3, xmm4;			// 00 00 00 00 0R1 00 0R0 00
		punpcklwd	xmm1, xmm2;			// 0000 0000  A1 000  A0 000
		por			xmm6, xmm3;
		por			xmm6, xmm1;
		movsd		QWORD PTR[rdi], xmm6;
loop_end_yuyva_to_bgra_inner:
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 10h]
		add			rsp,20h
		pop			rbx
		pop			rdi
		pop			rsi
		ret

;--------------------------------------------------------------------------
;void yuyva_to_rgba_inner_x64(OUT void* rgba,        // RGBA 缓存，存放方式 R0 G0 B0 A0 R1 G1 B1 A1 ……
;						 IN void* yuyv, 		// 源 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
;						 IN void* alpha,		// 源 alpha 缓存，存放方式 A0 A1 A2 A3 ……
;						 IN int na16,		    // 源字节宽度减 4 后除以 16 的商与余数
;						 IN int nm16,
;						 IN int* pnR_V,         // 系数（无法做到 RGBA 与 BGRA 的兼容）
;						 IN int* pnG_U,       
;						 IN int* pnG_V,
;						 IN int* pnB_U)
;--------------------------------------------------------------------------
yuyva_to_rgba_inner_x64:
	;rcx			- rgba
	;rdx			- yuyv
	;r8				- alpha
	;r9				- na16
	;[rsp+28h]		- nm16
	;[rsp+30h]		- pnR_V
	;[rsp+38h]		- pnG_U
	;[rsp+40h]		- pnG_V
	;[rsp+48h]		- pnB_U

		push		rsi
		push		rdi
		push		rbx
		sub			rsp,20h
		movups		[rsp],xmm6
		movups		[rsp + 10h],xmm7
	
		mov			rsi, rdx;
		mov			rax, rdx;
		mov			rdi, rcx;
		mov			rcx, r9;
		mov			rdx, r8;
		mov			rbx, [rsp+40h+38h];
		add			rax, 10h;			// 下一个 VYUV 用于 UV 插值
		test		rcx, rcx;
		jnz			loop_a16_yuyva_to_rgba_inner;
		jmp			loop_m16_yuyva_to_rgba_inner;
loop_a16_yuyva_to_rgba_inner:
		movdqu		xmm7, [rsi];		// VYUY VYUY VYUY VYUY
		movss		xmm6, DWORD PTR[rax];		// 下一个 VYUV 用于 UV 插值
		movdqa		xmm1, xmm7;
		movdqa		xmm0, OWORD PTR[gc_dwMaskWLB];
		movdqa		xmm3, OWORD PTR[gc_dwMaskWHB];
		movdqa		xmm4, OWORD PTR[gc_dwMaskSpecUV];
		movdqa		xmm5, OWORD PTR[gc_nOff128_6];
		pand		xmm7, xmm0;			// Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
		pand		xmm1, xmm3;			// V6 U6 V4 U4 V2 U2 V0 U0
		pand		xmm6, xmm3;			// 0 0 0 0 0 0 V8 U8
		psrlw		xmm1, 2;			// 放大 6 位
		psrlw		xmm6, 2;			// 放大 6 位
		psllw		xmm7, 5;			// 放大 5 位
		pshufd		xmm2, xmm1, 39h;	// V0 U0 V6 U6 V4 U4 V2 U2
		shufps		xmm6, xmm6, 15h;	// V8 U8 0 0 0 0 0 0
		pand		xmm2, xmm4;			// 0 0 V6 U6 V4 U4 V2 U2
		por			xmm2, xmm6;			// V8 U8 V6 U6 V4 U4 V2 U2
		psubsw		xmm1, xmm5;			// 减去偏移量 128
		psubsw		xmm2, xmm5;			// 减去偏移量 128
		movdqa		xmm3, xmm1;
		movdqa		xmm5, xmm1;
		movdqa		xmm6, xmm1;
		movdqa		xmm4, xmm2;
		pand		xmm1, OWORD PTR[gc_dwMaskDLW];	// 0 U6 0 U4 0 U2 0 U0
		pand		xmm3, OWORD PTR[gc_dwMaskDHW];	// V6 0 V4 0 V2 0 V0 0
		pand		xmm2, OWORD PTR[gc_dwMaskDHW];	// V8 0 V6 0 V4 0 V2 0
		pslld		xmm4, 10h;			// U8 0 U6 0 U4 0 U2 0
		pslld		xmm5, 10h;			// U6 0 U4 0 U2 0 U0 0
		psrld		xmm6, 10h;			// 0 V6 0 V4 0 V2 0 V0
		por			xmm4, xmm1;			// U8 U6 U6 U4 U4 U2 U2 U0
		por			xmm5, xmm1;			// U6 U6 U4 U4 U2 U2 U0 U0
		por			xmm2, xmm6;			// V8 V6 V6 V4 V4 V2 V2 V0
		por			xmm3, xmm6;			// V6 V6 V4 V4 V2 V2 V0 V0
		paddw		xmm4, xmm5;			// U7 U6 U5 U4 U3 U2 U1 U0
		paddw		xmm3, xmm2;			// V7 V6 V5 V4 V3 V2 V1 V0
		push        rsi;
		push        rax;
		push        rcx;
		mov         rax, [rsp+48h+38h];		//pnR_V;
		mov         rcx, [rsp+50h+38h];		//pnG_U;
		mov         rsi, [rsp+60h+38h];		//pnB_U;
		movdqu		xmm1, [rax];
		movdqu		xmm2, [rbx];
		movdqu		xmm5, [rcx];
		movdqu		xmm6, [rsi];
		pop         rcx;
		pop         rax;
		pop         rsi;
		pmulhw		xmm1, xmm3;
		pmulhw		xmm2, xmm3;
		pmulhw		xmm5, xmm4;
		pmulhw		xmm6, xmm4;
		pxor		xmm3, xmm3;			// 0
		movdqa		xmm4, OWORD PTR[gc_nRoundComp_5];
		paddsw		xmm1, xmm7;			// R
		paddsw		xmm2, xmm7;			// G
		paddsw		xmm5, xmm4;
		paddsw		xmm6, xmm7;			// B
		paddsw		xmm1, xmm4;			// R
		paddsw		xmm2, xmm5;			// G
		paddsw		xmm6, xmm4;			// B
		pmaxsw		xmm1, xmm3;			// R
		pmaxsw		xmm2, xmm3;			// G
		pmaxsw		xmm6, xmm3;			// B
		psrlw		xmm1, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm1, xmm0;			// R7 R6 R5 R4 R3 R2 R1 R0
		pminsw		xmm2, xmm0;			// G7 G6 G5 G4 G3 G2 G1 G0
		pminsw		xmm6, xmm0;			// B7 B6 B5 B4 B3 B2 B1 B0
		psllw		xmm2, 08h;
		movsd		xmm7, QWORD PTR[rdx];		// 0000 0000 A7A6A5A4 A3A2A1A0
		por			xmm1, xmm2;			// G7R7 G6R6 G5R5 G4R4 G3R3 G2R2 G1R1 G0R0
		pxor		xmm2, xmm2;
		pxor		xmm4, xmm4;
		pxor		xmm5, xmm5;
		movdqa		xmm0, xmm1;
		punpcklbw	xmm2, xmm7;			// A7 A6 A5 A4 A3 A2 A1 A0
		punpcklwd	xmm3, xmm6;			// 0B3 00 0B2 00 0B1 00 0B0 00
		punpcklwd	xmm0, xmm5;			// 00 G3R3 00 G2R2 00 G1R1 00 G0R0
		punpckhwd	xmm1, xmm5;			// 00 G7R7 00 G6R6 00 G5R5 00 G4R4
		pxor		xmm7, xmm7;
		punpcklwd	xmm4, xmm2;			// A3 000 A2 000 A1 000 A0 000
		punpckhwd	xmm5, xmm6;			// 0B7 00 0B6 00 0B5 00 0B4 00
		punpckhwd	xmm7, xmm2;			// A7 000 A6 000 A5 000 A4 000
		por			xmm0, xmm3;
		por			xmm1, xmm5;
		por			xmm0, xmm4;
		por			xmm1, xmm7;
		movdqu		[rdi], xmm0;
		add			rax, 10h;
		add			rdi, 10h;
		add			rsi, 10h;
		movdqu		[rdi], xmm1;
		add			rdx, 08h;
		add			rdi, 10h;
		sub			rcx, 01h;
		jnz			loop_a16_yuyva_to_rgba_inner;
loop_m16_yuyva_to_rgba_inner:
		mov			rcx, [rsp+28h+38h];
		test		rcx, 10h;
		jz			loop_m12_yuyva_to_rgba_inner;
		movdqu		xmm7, [rsi];		// VYUY VYUY VYUY VYUY
		movdqa		xmm0, OWORD PTR[gc_dwMaskWLB];
		movdqa		xmm1, xmm7;
		movdqa		xmm5, OWORD PTR[gc_nOff128_6];
		pand		xmm7, xmm0;			// Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
		pand		xmm1, OWORD PTR[gc_dwMaskWHB];	// V6 U6 V4 U4 V2 U2 V0 U0
		psllw		xmm7, 5;			// 放大 5 位
		psrlw		xmm1, 2;			// 放大 6 位
		pshufd		xmm2, xmm1, 0f9h;	// V6 U6 V6 U6 V4 U4 V2 U2
		psubw		xmm1, xmm5;			// 减去偏移量 128
		psubw		xmm2, xmm5;			// 减去偏移量 128
		movdqa		xmm3, xmm1;
		movdqa		xmm5, xmm1;
		movdqa		xmm6, xmm1;
		movdqa		xmm4, xmm2;
		pand		xmm1, OWORD PTR[gc_dwMaskDLW];	// 0 U6 0 U4 0 U2 0 U0
		pand		xmm3, OWORD PTR[gc_dwMaskDHW];	// V6 0 V4 0 V2 0 V0 0
		pand		xmm2, OWORD PTR[gc_dwMaskDHW];	// V6 0 V6 0 V4 0 V2 0
		pslld		xmm4, 10h;			// U6 0 U6 0 U4 0 U2 0
		pslld		xmm5, 10h;			// U6 0 U4 0 U2 0 U0 0
		psrld		xmm6, 10h;			// 0 V6 0 V4 0 V2 0 V0
		por			xmm4, xmm1;			// U6 U6 U6 U4 U4 U2 U2 U0
		por			xmm5, xmm1;			// U6 U6 U4 U4 U2 U2 U0 U0
		por			xmm2, xmm6;			// V6 V6 V6 V4 V4 V2 V2 V0
		por			xmm3, xmm6;			// V6 V6 V4 V4 V2 V2 V0 V0
		paddw		xmm4, xmm5;			// U7 U6 U5 U4 U3 U2 U1 U0
		paddw		xmm3, xmm2;			// V7 V6 V5 V4 V3 V2 V1 V0
		mov         rax, [rsp+30h+38h];		//pnR_V;
		mov         rcx, [rsp+38h+38h];		//pnG_U;
		mov         rsi, [rsp+48h+38h];		//pnB_U;
		movdqu		xmm1, [rax];
		movdqu		xmm2, [rbx];
		movdqu		xmm5, [rcx];
		movdqu		xmm6, [rsi];
		pmulhw		xmm1, xmm3;
		pmulhw		xmm2, xmm3;
		pmulhw		xmm5, xmm4;
		pmulhw		xmm6, xmm4;
		pxor		xmm3, xmm3;			// 0
		movdqa		xmm4, OWORD PTR[gc_nRoundComp_5];
		paddsw		xmm1, xmm7;			// R
		paddsw		xmm2, xmm7;			// G
		paddsw		xmm5, xmm4;
		paddsw		xmm6, xmm7;			// B
		paddsw		xmm1, xmm4;			// R
		paddsw		xmm2, xmm5;			// G
		paddsw		xmm6, xmm4;			// B
		pmaxsw		xmm1, xmm3;			// R
		pmaxsw		xmm2, xmm3;			// G
		pmaxsw		xmm6, xmm3;			// B
		psrlw		xmm1, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm1, xmm0;		// R7 R6 R5 R4 R3 R2 R1 R0
		pminsw		xmm2, xmm0;		// G7 G6 G5 G4 G3 G2 G1 G0
		pminsw		xmm6, xmm0;		// B7 B6 B5 B4 B3 B2 B1 B0
		psllw		xmm2, 08h;
		movsd		xmm7, QWORD PTR[rdx];		// 0000 0000 A7A6A5A4 A3A2A1A0
		por			xmm1, xmm2;			// G7R7 G6R6 G5R5 G4R4 G3R3 G2R2 G1R1 G0R0
		pxor		xmm2, xmm2;
		pxor		xmm4, xmm4;
		pxor		xmm5, xmm5;
		movdqa		xmm0, xmm1;
		punpcklbw	xmm2, xmm7;			// A7 A6 A5 A4 A3 A2 A1 A0
		punpcklwd	xmm3, xmm6;			// 0B3 00 0B2 00 0B1 00 0B0 00
		punpcklwd	xmm0, xmm5;			// 00 G3R3 00 G2R2 00 G1R1 00 G0R0
		punpckhwd	xmm1, xmm5;			// 00 G7R7 00 G6R6 00 G5R5 00 G4R4
		pxor		xmm7, xmm7;
		punpcklwd	xmm4, xmm2;			// A3 000 A2 000 A1 000 A0 000
		punpckhwd	xmm5, xmm6;			// 0B7 00 0B6 00 0B5 00 0B4 00
		punpckhwd	xmm7, xmm2;			// A7 000 A6 000 A5 000 A4 000
		por			xmm0, xmm3;
		por			xmm1, xmm5;
		por			xmm0, xmm4;
		por			xmm1, xmm7;
		movdqu		[rdi], xmm0;
		add			rdi, 10h;
		movdqu		[rdi], xmm1;
		jmp			loop_end_yuyva_to_rgba_inner;
loop_m12_yuyva_to_rgba_inner:
		cmp			rcx, 0ch;
		jnz			loop_m8_yuyva_to_rgba_inner;
		mov			rcx, rdx;
		sub			rax, 08h;
		add			rdx, 04h;
		movsd		xmm7, QWORD PTR[rsi];		// 0000 0000 VYUY VYUY
		movss		xmm6, DWORD PTR[rax];		// 最后两个像素 0000 0000 0000 VYUY
		shufps		xmm6, xmm6, 00h;
		movlhps		xmm7, xmm6;			// VYUY VYUY VYUY VYUY
		movdqa		xmm0, OWORD PTR[gc_dwMaskWLB];
		movdqa		xmm1, xmm7;
		movdqa		xmm5, OWORD PTR[gc_nOff128_6];
		pand		xmm7, xmm0;			// Y5 Y4 Y5 Y4 Y3 Y2 Y1 Y0
		pand		xmm1, OWORD PTR[gc_dwMaskWHB];	// V4 U4 V4 U4 V2 U2 V0 U0
		psllw		xmm7, 5;			// 放大 5 位
		psrlw		xmm1, 2;			// 放大 6 位
		pshufd		xmm2, xmm1, 0f9h;	// V4 U4 V4 U4 V4 U4 V2 U2
		psubw		xmm1, xmm5;			// 减去偏移量 128
		psubw		xmm2, xmm5;			// 减去偏移量 128
		movdqa		xmm3, xmm1;
		movdqa		xmm5, xmm1;
		movdqa		xmm6, xmm1;
		movdqa		xmm4, xmm2;
		pand		xmm1, OWORD PTR[gc_dwMaskDLW];	// 0 U4 0 U4 0 U2 0 U0
		pand		xmm3, OWORD PTR[gc_dwMaskDHW];	// V4 0 V4 0 V2 0 V0 0
		pand		xmm2, OWORD PTR[gc_dwMaskDHW];	// V4 0 V4 0 V4 0 V2 0
		pslld		xmm4, 10h;			// U4 0 U4 0 U4 0 U2 0
		pslld		xmm5, 10h;			// U4 0 U4 0 U2 0 U0 0
		psrld		xmm6, 10h;			// 0 V4 0 V4 0 V2 0 V0
		por			xmm4, xmm1;			// U4 U4 U4 U4 U4 U2 U2 U0
		por			xmm5, xmm1;			// U4 U4 U4 U4 U2 U2 U0 U0
		por			xmm2, xmm6;			// V4 V4 V4 V4 V4 V2 V2 V0
		por			xmm3, xmm6;			// V4 V4 V4 V4 V2 V2 V0 V0
		paddw		xmm4, xmm5;			// U5 U4 U5 U4 U3 U2 U1 U0
		paddw		xmm3, xmm2;			// V5 V4 V5 V4 V3 V2 V1 V0
		push        rcx;
		mov         rax, [rsp+38h+38h];		//pnR_V;
		mov         rcx, [rsp+40h+38h];		//pnG_U;
		mov         rsi, [rsp+50h+38h];		//pnB_U;
		movdqu		xmm1, [rax];
		movdqu		xmm2, [rbx];
		movdqu		xmm5, [rcx];
		movdqu		xmm6, [rsi];
		pop         rcx;
		pmulhw		xmm1, xmm3;
		pmulhw		xmm2, xmm3;
		pmulhw		xmm5, xmm4;
		pmulhw		xmm6, xmm4;
		pxor		xmm3, xmm3;			// 0
		movdqa		xmm4, OWORD PTR[gc_nRoundComp_5];
		paddsw		xmm1, xmm7;			// R
		paddsw		xmm2, xmm7;			// G
		paddsw		xmm5, xmm4;
		paddsw		xmm6, xmm7;			// B
		paddsw		xmm1, xmm4;			// R
		paddsw		xmm2, xmm5;			// G
		paddsw		xmm6, xmm4;			// B
		pmaxsw		xmm1, xmm3;			// R
		pmaxsw		xmm2, xmm3;			// G
		pmaxsw		xmm6, xmm3;			// B
		psrlw		xmm1, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm1, xmm0;			// R5 R4 R5 R4 R3 R2 R1 R0
		pminsw		xmm2, xmm0;			// G5 G4 G5 G4 G3 G2 G1 G0
		pminsw		xmm6, xmm0;			// B5 B4 B5 B4 B3 B2 B1 B0
		psllw		xmm2, 08h;
		movss		xmm7, DWORD PTR[rcx];		// 0000 0000 0000 A3A2A1A0
		mov			ax, [rdx];			// 00A5A4
		por			xmm1, xmm2;			// G5R5 G4R4 G5R5 G4R4 G3R3 G2R2 G1R1 G0R0
		movd		xmm0, rax;			// 0000 0000 0000 00A5A4
		shufps		xmm0, xmm0, 01h;
		pxor		xmm2, xmm2;
		pxor		xmm4, xmm4;
		pxor		xmm5, xmm5;
		por			xmm7, xmm0;			// **** **** 00A5A4 A3A2A1A0
		movdqa		xmm0, xmm1;
		punpcklbw	xmm2, xmm7;			// 0 0 A5 A4 A3 A2 A1 A0
		punpcklwd	xmm3, xmm6;			// 0B3 00 0B2 00 0B1 00 0B0 00
		punpcklwd	xmm0, xmm5;			// 00 G3R3 00 G2R2 00 G1R1 00 G0R0
		punpckhwd	xmm1, xmm5;			// 00 G5R5 00 G4R4 00 G5R5 00 G4R4
		pxor		xmm7, xmm7;
		punpcklwd	xmm4, xmm2;			// A3 000 A2 000 A1 000 A0 000
		punpckhwd	xmm5, xmm6;			// 0B5 00 0B4 00 0B5 00 0B4 00
		punpckhwd	xmm7, xmm2;			// 0000 0000 A5 000 A4 000
		por			xmm0, xmm3;
		por			xmm1, xmm5;
		por			xmm0, xmm4;
		por			xmm1, xmm7;
		movdqu		[rdi], xmm0;
		add			rdi, 10h;
		movsd		QWORD PTR[rdi], xmm1;
		jmp			loop_end_yuyva_to_rgba_inner;
loop_m8_yuyva_to_rgba_inner:
		test		rcx, 08h;
		jz			loop_m4_yuyva_to_rgba_inner;
		movsd		xmm7, QWORD PTR[rsi];		// 0000 0000 VYUY VYUY
		movdqa		xmm0, OWORD PTR[gc_dwMaskWLB];
		movdqa		xmm1, xmm7;
		pand		xmm7, xmm0;			// 0 0 0 0 Y3 Y2 Y1 Y0
		pand		xmm1, OWORD PTR[gc_dwMaskWHB];	// 0 0 0 0 V2 U2 V0 U0
		psllw		xmm7, 5;			// 放大 5 位
		psrlw		xmm1, 2;			// 放大 6 位
		psubw		xmm1, OWORD PTR[gc_nOff128_6];	// 减去偏移量 128
		pshufd		xmm2, xmm1, 0f5h;	// 0 0 0 0 V2 U2 V2 U2
		movdqa		xmm3, xmm1;
		movdqa		xmm5, xmm1;
		movdqa		xmm6, xmm1;
		movdqa		xmm4, xmm2;
		pand		xmm1, OWORD PTR[gc_dwMaskDLW];	// 0 0 0 0 0 U2 0 U0
		pand		xmm3, OWORD PTR[gc_dwMaskDHW];	// 0 0 0 0 V2 0 V0 0
		pand		xmm2, OWORD PTR[gc_dwMaskDHW];	// 0 0 0 0 V2 0 V2 0
		pslld		xmm4, 10h;			// 0 0 0 0 U2 0 U2 0
		pslld		xmm5, 10h;			// 0 0 0 0 U2 0 U0 0
		psrld		xmm6, 10h;			// 0 0 0 0 0 V2 0 V0
		por			xmm4, xmm1;			// 0 0 0 0 U2 U2 U2 U0
		por			xmm5, xmm1;			// 0 0 0 0 U2 U2 U0 U0
		por			xmm2, xmm6;			// 0 0 0 0 V2 V2 V2 V0
		por			xmm3, xmm6;			// 0 0 0 0 V2 V2 V0 V0
		paddw		xmm4, xmm5;			// 0 0 0 0 U3 U2 U1 U0
		paddw		xmm3, xmm2;			// 0 0 0 0 V3 V2 V1 V0
		mov         rax, [rsp+30h+38h];		//pnR_V;
		mov         rcx, [rsp+38h+38h];		//pnG_U;
		mov         rsi, [rsp+48h+38h];		//pnB_U;
		movdqu		xmm1, [rax];
		movdqu		xmm2, [rbx];
		movdqu		xmm5, [rcx];
		movdqu		xmm6, [rsi];
		pmulhw		xmm1, xmm3;
		pmulhw		xmm2, xmm3;
		pmulhw		xmm5, xmm4;
		pmulhw		xmm6, xmm4;
		pxor		xmm3, xmm3;			// 0
		movdqa		xmm4, OWORD PTR[gc_nRoundComp_5];
		paddsw		xmm1, xmm7;			// R
		paddsw		xmm2, xmm7;			// G
		paddsw		xmm5, xmm4;
		paddsw		xmm6, xmm7;			// B
		paddsw		xmm1, xmm4;			// R
		paddsw		xmm2, xmm5;			// G
		paddsw		xmm6, xmm4;			// B
		pmaxsw		xmm1, xmm3;			// R
		pmaxsw		xmm2, xmm3;			// G
		pmaxsw		xmm6, xmm3;			// B
		psrlw		xmm1, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm1, xmm0;			// 0 0 0 0 R3 R2 R1 R0
		pminsw		xmm2, xmm0;			// 0 0 0 0 G3 G2 G1 G0
		pminsw		xmm6, xmm0;			// 0 0 0 0 B3 B2 B1 B0
		psllw		xmm2, 08h;
		movss		xmm7, DWORD PTR[rdx];		// 0000 0000 0000 A3A2A1A0
		por			xmm1, xmm2;			// 00 00 00 00 G3R3 G2R2 G1R1 G0R0
		pxor		xmm2, xmm2;
		pxor		xmm4, xmm4;
		punpcklbw	xmm2, xmm7;			// 0 0 0 0 A3 A2 A1 A0
		punpcklwd	xmm1, xmm4;			// 00 G3R3 00 G2R2 00 G1R1 00 G0R0
		punpcklwd	xmm3, xmm6;			// 0B3 00 0B2 00 0B1 00 0B0 00
		punpcklwd	xmm4, xmm2;			// A3 000 A2 000 A1 000 A0 000
		por			xmm1, xmm3;
		por			xmm1, xmm4;
		movdqu		[rdi], xmm1;
		jmp			loop_end_yuyva_to_rgba_inner;
loop_m4_yuyva_to_rgba_inner:
		test		rcx, 04h;
		jz			loop_end_yuyva_to_rgba_inner;
		movss		xmm7, DWORD PTR[rsi];		// 0000 0000 0000 VYUY
		movdqa		xmm0, OWORD PTR[gc_dwMaskWLB];
		movdqa		xmm1, xmm7;
		pand		xmm7, xmm0;			// 0 0 0 0 0 0 Y1 Y0
		pand		xmm1, OWORD PTR[gc_dwMaskWHB];	// 0 0 0 0 0 0 V0 U0
		psllw		xmm7, 5;			// 放大 5 位
		psrlw		xmm1, 2;			// 放大 6 位
		psubw		xmm1, OWORD PTR[gc_nOff128_6];	// 减去偏移量 128
		movdqa		xmm3, xmm1;
		psrld		xmm1, 10h;			// 0 0 0 0 0 0 0 V0
		mov         rax, [rsp+30h+38h];	//pnR_V;
		mov         rcx, [rsp+38h+38h];	//pnG_U;
		mov         rsi, [rsp+48h+38h];	//pnB_U;
		movdqu		xmm1, [rax];
		movdqu		xmm2, [rbx];
		movdqu		xmm5, [rcx];
		movdqu		xmm6, [rsi];
		pmulhw		xmm4, xmm1;
		pmulhw		xmm2, xmm1;
		pmulhw		xmm5, xmm3;
		pmulhw		xmm6, xmm3;
		pxor		xmm1, xmm1;			// 0
		movdqa		xmm3, OWORD PTR[gc_nRoundComp_5];
		paddsw		xmm4, xmm7;			// R
		paddsw		xmm2, xmm7;			// G
		paddsw		xmm5, xmm3;
		paddsw		xmm6, xmm7;			// B
		paddsw		xmm4, xmm3;			// R
		paddsw		xmm2, xmm5;			// G
		paddsw		xmm6, xmm3;			// B
		pmaxsw		xmm4, xmm1;			// R
		pmaxsw		xmm2, xmm1;			// G
		pmaxsw		xmm6, xmm1;			// B
		psrlw		xmm4, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm4, xmm0;			// 0 0 0 0 0 0 R1 R0
		pminsw		xmm2, xmm0;			// 0 0 0 0 0 0 G1 G0
		pminsw		xmm6, xmm0;			// 0 0 0 0 0 0 B1 B0
		psllw		xmm2, 08h;
		mov			ax, [rdx];			// 00A1A0
		por			xmm4, xmm2;			// 00 00 00 00 00 00 G1R1 G0R0
		movd		xmm7, rax;			// 0000 0000 0000 00A1A0
		pxor		xmm2, xmm2;
		pxor		xmm3, xmm3;
		punpcklbw	xmm2, xmm7;			// 0 0 0 0 0 0 A1 A0
		punpcklwd	xmm4, xmm1;			// 00 00 00 00 00 G1R1 00 G0R0
		punpcklwd	xmm3, xmm6;			// 00 00 00 00 0B1 00 0B0 00
		punpcklwd	xmm1, xmm2;			// 0000 0000  A1 000  A0 000
		por			xmm4, xmm3;
		por			xmm4, xmm1;
		movsd		QWORD PTR[rdi], xmm4;
loop_end_yuyva_to_rgba_inner:	
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 10h]
		add			rsp,20h
		pop			rbx
		pop			rdi
		pop			rsi
		ret

;--------------------------------------------------------------------------
;void color32_to_yuyv_inner_x64(OUT void* yuyv, 	    // 目标 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
;						   IN void* color32,	// 源 *g@a 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
;						   IN int na16,		    // 源字节宽度除以 16 的商与余数
;						   IN int nm16,
;						   IN int* pnY32_BR,    // 系数（通过变更 R 分量与 B 分量的系数位置可以做到 BGRA 与 RGBA 的兼容）
;						   IN int* pnY32_G,     // BR 表示 R 在低位
;						   IN int* pnY32_BGR,
;						   IN int* pnUV_BGR,
;						   IN int* pnU_BGR,
;						   IN int* pnV_BGR)
;--------------------------------------------------------------------------
color32_to_yuyv_inner_x64:
	;rcx			- yuyv
	;rdx			- color32
	;r8				- na16
	;r9				- nm16
	;[rsp+28h]		- pnY32_BR
	;[rsp+30h]		- pnY32_G
	;[rsp+38h]		- pnY32_BGR
	;[rsp+40h]		- pnUV_BGR
	;[rsp+48h]		- pnU_BGR
	;[rsp+50h]		- pnV_BGR
	
		push		rsi
		push		rdi
		push		rbx
		sub			rsp,20h
		movups		[rsp],xmm6
		movups		[rsp + 10h],xmm7
	
		mov			rdi, rcx;
		mov			rcx, r8;
		
		mov			rsi, rdx;
		mov			rax, [rsp+28h+38h];
		mov			rbx, [rsp+30h+38h];
		mov			rdx, [rsp+50h+38h];
		movdqa		xmm3, OWORD PTR[gc_dwMaskDLB];
		test		rcx, rcx;
		jnz			loop_a16_color32_to_yuyv_inner;
		jmp			loop_m8_color32_to_yuyv_inner;
loop_a16_color32_to_yuyv_inner:
		movdqu		xmm7, [rsi];		// ABGR ABGR ABGR ABGR
		movdqu		xmm6, [rax];   	    // Y
		movdqa		xmm1, xmm7;
		movdqa		xmm2, xmm7;
		pxor		xmm0, xmm0;
		shufps		xmm7, xmm7, 08h;	// 取 U0V0 与 U2V2
		pand		xmm1, OWORD PTR[gc_dwMaskWLB];	// 0B0R 0B0R 0B0R 0B0R
		pand		xmm2, OWORD PTR[gc_dwMaskWHB];	// A0G0 A0G0 A0G0 A0G0
		punpcklbw	xmm0, xmm7;			// A0B0 G0R0 A0B0 G0R0
		psllw		xmm1, 07h;			// 放大 7 位
		psrlw		xmm2, 01h;
		psrlw		xmm0, 01h;
		push        rcx;
		mov         rcx, [rsp+50h+38h];	//pnU_BGR;
		movdqu		xmm7, [rbx];
		movdqu		xmm4, [rcx];  	    // U
		movdqu		xmm5, [rdx];  	    // V
		pop         rcx;
		pmulhw		xmm6, xmm1;			// 还放大 5 位
		pmulhw		xmm7, xmm2;
		pmulhw		xmm4, xmm0;
		pmulhw		xmm5, xmm0;
		paddsw		xmm7, xmm6;			// YR + YG
		pshufd		xmm1, xmm4, 0b1h;
		pshufd		xmm2, xmm5, 0b1h;
		psrld		xmm6, 10h;			// YB
		paddsw		xmm1, xmm4;			// UR + UB
		paddsw		xmm2, xmm5;			// VR + VB
		paddsw		xmm7, xmm6;			// Y
		movdqa		xmm0, OWORD PTR[gc_nRoundComp_5];
		movdqa		xmm6, OWORD PTR[gc_nOff128_5];
		psrld		xmm4, 10h;
		psrld		xmm5, 10h;
		paddsw		xmm1, xmm0;
		paddsw		xmm2, xmm0;
		paddsw		xmm7, xmm0;
		paddsw		xmm4, xmm6;
		paddsw		xmm5, xmm6;
		paddsw		xmm1, xmm4;
		paddsw		xmm2, xmm5;
		psrlw		xmm1, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm7, 05h;
		shufps		xmm1, xmm1, 88h;	// U2 U0 U2 U0
		shufps		xmm2, xmm2, 88h;	// V2 V0 V2 V0
		pminsw		xmm7, xmm3;
		pminsw		xmm1, xmm3;
		pminsw		xmm2, xmm3;
		pand		xmm7, xmm3;			// 0 Y3 0 Y2 0 Y1 0 Y0
		pand		xmm1, xmm3;			// 0 U2 0 U0 0 U2 0 U0
		pand		xmm2, xmm3;			// 0 V2 0 V0 0 V2 0 V0
		packssdw	xmm7, xmm7;			// Y3Y2 Y1Y0 Y3Y2 Y1Y0
		pslld		xmm1, 08h;			// 00 U2 0  00 U0 0  00 U2 0  00 U0 0
		pslld		xmm2, 18h;			// V2 000  V0 000  V2 000  V0 000
		por			xmm7, xmm1;			// 0 Y3U2Y2  0 Y1U0Y0  0 Y3U2Y2  0 Y1U0Y0
		por			xmm7, xmm2;			// V2Y3U2Y2 V0Y1U0Y0 V2Y3U2Y2 V0Y1U0Y0
		movsd		QWORD PTR[rdi], xmm7;
		add			rsi, 10h;
		add			rdi, 08h;
		sub			rcx, 01h;
		jnz			loop_a16_color32_to_yuyv_inner;
loop_m8_color32_to_yuyv_inner:
		mov			rcx, r9;
		mov			rax, [rsp+38h+38h];
		mov			rdx, [rsp+40h+38h];
		test		rcx, 08h;
		jz			loop_m4_color32_to_yuyv_inner;
		movsd		xmm7, QWORD PTR[rsi];		// 0000 0000 ABGR ABGR
		movdqa		xmm2, OWORD PTR[gc_nRoundComp_5];
		movdqu		xmm5, [rax];        // Y
		movdqu		xmm6, [rdx];	    // UV
		pxor		xmm0, xmm0;
		pxor		xmm1, xmm1;
		punpcklbw	xmm0, xmm7;			// A0B0 G0R0 A0B0 G0R0
		shufps		xmm7, xmm7, 00h;
		punpcklbw	xmm1, xmm7;			// A0B0 G0R0 A0B0 G0R0
		psrlw		xmm0, 01h;			// 放大 7 位
		psrlw		xmm1, 01h;			// 放大 7 位
		pmulhw		xmm5, xmm0;			// 还放大 5 位
		pmulhw		xmm6, xmm1;
		pshufd		xmm0, xmm5, 0b1h;
		pshufd		xmm1, xmm6, 0b1h;
		paddsw		xmm0, xmm5;			// YR + YB
		paddsw		xmm1, xmm6;			// VR + VB  UR + UB
		psrld		xmm5, 10h;
		psrld		xmm6, 10h;
		paddsw		xmm0, xmm2;
		paddsw		xmm1, xmm2;
		paddsw		xmm6, OWORD PTR[gc_nOff128_5];
		paddsw		xmm0, xmm5;			// Y
		paddsw		xmm1, xmm6;			// U V
		psrlw		xmm0, 05h;
		psrlw		xmm1, 05h;
		pminsw		xmm0, xmm3;
		pminsw		xmm1, xmm3;
		shufps		xmm0, xmm0, 88h;	// Y1 Y0 Y1 Y0
		shufps		xmm1, xmm1, 88h;	// V0 U0 V0 U0
		pand		xmm0, xmm3;			// 0 Y1 0 Y0 0 Y1 0 Y0
		pand		xmm1, xmm3;			// 0 V0 0 U0 0 V0 0 U0
		packssdw	xmm0, xmm0;			// Y1Y0 Y1Y0 Y1Y0 Y1Y0
		packssdw	xmm1, xmm1;			// V0U0 V0U0 V0U0 V0U0
		pslld		xmm1, 08h;
		por			xmm0, xmm1;			// V0Y1U0Y0
		movss		DWORD PTR[rdi], xmm0;
		add			rsi, 08h;
		add			rdi, 04h;
loop_m4_color32_to_yuyv_inner:
		test		rcx, 04h;
		jz			loop_end_color32_to_yuyv_inner;
		movss		xmm7, DWORD PTR[rsi];		// 0000 0000 0000 ABGR
		movdqa		xmm2, OWORD PTR[gc_nRoundComp_5];
		movdqu		xmm5, [rax];  	    // Y
		movdqu		xmm6, [rdx]; 	    // UV
		shufps		xmm7, xmm7, 0f0h;
		pxor		xmm0, xmm0;
		punpcklbw	xmm0, xmm7;			// A0B0 G0R0 A0B0 G0R0
		psrlw		xmm0, 01h;			// 放大 7 位
		pmulhw		xmm5, xmm0;			// 还放大 5 位
		pmulhw		xmm6, xmm0;
		pshufd		xmm0, xmm5, 0b1h;
		pshufd		xmm1, xmm6, 0b1h;
		paddsw		xmm0, xmm5;			// YR + YB
		paddsw		xmm1, xmm6;			// VR + VB  UR + UB
		psrld		xmm5, 10h;
		psrld		xmm6, 10h;
		paddsw		xmm0, xmm2;
		paddsw		xmm1, xmm2;
		paddsw		xmm6, OWORD PTR[gc_nOff128_5];
		paddsw		xmm0, xmm5;			// Y
		paddsw		xmm1, xmm6;			// U V
		psrlw		xmm0, 05h;
		psrlw		xmm1, 05h;
		pminsw		xmm0, xmm3;
		pminsw		xmm1, xmm3;
		shufps		xmm0, xmm0, 88h;	// Y1 Y0 Y1 Y0
		shufps		xmm1, xmm1, 88h;	// V0 U0 V0 U0
		pand		xmm0, xmm3;			// 0 Y1 0 Y0 0 Y1 0 Y0
		pand		xmm1, xmm3;			// 0 V0 0 U0 0 V0 0 U0
		packssdw	xmm0, xmm0;			// Y1Y0 Y1Y0 Y1Y0 Y1Y0
		packssdw	xmm1, xmm1;			// V0U0 V0U0 V0U0 V0U0
		pslld		xmm1, 08h;
		por			xmm0, xmm1;			// V0Y1U0Y0
		movss		DWORD PTR[rdi], xmm0;
loop_end_color32_to_yuyv_inner:	
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 10h]
		add			rsp,20h
		pop			rbx
		pop			rdi
		pop			rsi
		ret

;--------------------------------------------------------------------------
;void color32_to_yuyva_inner_x64(OUT void* yuyv,	        // 目标 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
;							OUT void* alpha,    	// 目标 alpha 缓存，存放方式 A0 A1 A2 A3 ……
;							IN void* color32,	    // 源 *g@a 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
;							IN int na16,    		// 源字节宽度除以 16 的商与余数
;							IN int nm16,
;							IN int* pnY32_BR,       // 系数（通过变更 R 分量与 B 分量的系数位置可以做到 BGRA 与 RGBA 的兼容）
;							IN int* pnY32_G,        // BR 表示 R 在低位
;							IN int* pnY32_BGR,
;							IN int* pnUV_BGR,
;							IN int* pnU_BGR,
;							IN int* pnV_BGR)
;--------------------------------------------------------------------------
color32_to_yuyva_inner_x64:
	;rcx			- yuyv
	;rdx			- alpha
	;r8				- color32
	;r9				- na16
	;[rsp+28h]		- nm16
	;[rsp+30h]		- pnY32_BR
	;[rsp+38h]		- pnY32_G
	;[rsp+40h]		- pnY32_BGR
	;[rsp+48h]		- pnUV_BGR
	;[rsp+50h]		- pnU_BGR
	;[rsp+58h]		- pnV_BGR

		push		rsi
		push		rdi
		push		rbx
		sub			rsp,20h
		movups		[rsp],xmm6
		movups		[rsp + 10h],xmm7
	
		mov			rdi, rcx;
		mov			rsi, r8;
		mov			rcx, r9;
		mov			rdx, rdx;
		test		rcx, rcx;
		jnz			loop_a16_color32_to_yuyva_inner;
		jmp			loop_m8_color32_to_yuyva_inner;
loop_a16_color32_to_yuyva_inner:
		mov         rax, [rsp+50h+38h];
		mov         rbx, [rsp+58h+38h];
		movdqu		xmm7, [rsi];		// ABGR ABGR ABGR ABGR
		movdqu		xmm4, [rax];	    // U
		movdqu		xmm5, [rbx];  	    // V
		mov       rax, [rsp+30h+38h];
		mov       rbx, [rsp+38h+38h];
		movdqu		xmm6, [rax];   	// Y
		movdqa		xmm1, xmm7;
		movdqa		xmm2, xmm7;
		movdqa		xmm3, xmm7;
		pxor		xmm0, xmm0;
		shufps		xmm7, xmm7, 08h;	// 取 U0V0 与 U2V2
		pand		xmm1, OWORD PTR[gc_dwMaskWLB];	// 0B0R 0B0R 0B0R 0B0R
		pand		xmm2, OWORD PTR[gc_dwMaskWHB];	// A0G0 A0G0 A0G0 A0G0
		punpcklbw	xmm0, xmm7;			// A0B0 G0R0 A0B0 G0R0
		psrld		xmm3, 18h;			// 000A 000A 000A 000A
		psllw		xmm1, 07h;			// 放大 7 位
		psrlw		xmm2, 01h;
		psrlw		xmm0, 01h;
		packssdw	xmm3, xmm3;			// 0A0A 0A0A 0A0A 0A0A
		movdqu		xmm7, [rbx];
		pmulhw		xmm6, xmm1;			// 还放大 5 位
		pmulhw		xmm7, xmm2;
		pmulhw		xmm4, xmm0;
		pmulhw		xmm5, xmm0;
		packuswb	xmm3, xmm3;			// AAAA AAAA AAAA AAAA
		paddsw		xmm7, xmm6;			// YR + YG
		pshufd		xmm1, xmm4, 0b1h;
		pshufd		xmm2, xmm5, 0b1h;
		psrld		xmm6, 10h;			// YB
		movss		DWORD PTR[rdx], xmm3;		// 保存 Alpha
		paddsw		xmm1, xmm4;			// UR + UB
		paddsw		xmm2, xmm5;			// VR + VB
		paddsw		xmm7, xmm6;			// Y
		movdqa		xmm0, OWORD PTR[gc_nRoundComp_5];
		movdqa		xmm6, OWORD PTR[gc_nOff128_5];
		movdqa		xmm3, OWORD PTR[gc_dwMaskDLB];
		psrld		xmm4, 10h;
		psrld		xmm5, 10h;
		paddsw		xmm1, xmm0;
		paddsw		xmm2, xmm0;
		paddsw		xmm7, xmm0;
		paddsw		xmm4, xmm6;
		paddsw		xmm5, xmm6;
		paddsw		xmm1, xmm4;
		paddsw		xmm2, xmm5;
		psrlw		xmm1, 05h;
		psrlw		xmm2, 05h;
		psrlw		xmm7, 05h;
		shufps		xmm1, xmm1, 88h;	// U2 U0 U2 U0
		shufps		xmm2, xmm2, 88h;	// V2 V0 V2 V0
		pminsw		xmm7, xmm3;
		pminsw		xmm1, xmm3;
		pminsw		xmm2, xmm3;
		pand		xmm7, xmm3;			// 0 Y3 0 Y2 0 Y1 0 Y0
		pand		xmm1, xmm3;			// 0 U2 0 U0 0 U2 0 U0
		pand		xmm2, xmm3;			// 0 V2 0 V0 0 V2 0 V0
		packssdw	xmm7, xmm7;			// Y3Y2 Y1Y0 Y3Y2 Y1Y0
		pslld		xmm1, 08h;			// 00 U2 0  00 U0 0  00 U2 0  00 U0 0
		pslld		xmm2, 18h;			// V2 000  V0 000  V2 000  V0 000
		por			xmm7, xmm1;			// 0 Y3U2Y2  0 Y1U0Y0  0 Y3U2Y2  0 Y1U0Y0
		por			xmm7, xmm2;			// V2Y3U2Y2 V0Y1U0Y0 V2Y3U2Y2 V0Y1U0Y0
		movsd		QWORD PTR[rdi], xmm7;
		add			rsi, 10h;
		add			rdx, 04h;
		add			rdi, 08h;
		sub			rcx, 01h;
		jnz			loop_a16_color32_to_yuyva_inner;
loop_m8_color32_to_yuyva_inner:
		mov			rcx, [rsp+28h+38h];
		mov        rax, [rsp+40h+38h];
		mov        rbx, [rsp+48h+38h];
		test		rcx, 08h;
		jz			loop_m4_color32_to_yuyva_inner;
		movsd		xmm7, QWORD PTR[rsi];		// 0000 0000 ABGR ABGR
		movdqa		xmm2, OWORD PTR[gc_nRoundComp_5];
		movdqa		xmm3, OWORD PTR[gc_dwMaskDLB];
		pshufd		xmm4, xmm7, 00h;
		pxor		xmm0, xmm0;
		pxor		xmm1, xmm1;
		movdqu		xmm5, [rax];	    // Y
		movdqu		xmm6, [rbx]; 	    // UV
		punpcklbw	xmm0, xmm4;			// A0B0 G0R0 A0B0 G0R0
		punpcklbw	xmm1, xmm7;			// A0B0 G0R0 A0B0 G0R0
		psrld		xmm7, 18h;			// 0000 0000 000A 000A
		psrlw		xmm0, 01h;			// 放大 7 位
		psrlw		xmm1, 01h;			// 放大 7 位
		packssdw	xmm7, xmm7;			// 0000 0A0A 0000 0A0A
		pmulhw		xmm5, xmm1;			// 还放大 5 位
		pmulhw		xmm6, xmm0;
		packuswb	xmm7, xmm7;			// 0000 00AA 0000 00AA
		pshufd		xmm0, xmm5, 0b1h;
		pshufd		xmm1, xmm6, 0b1h;
		paddsw		xmm0, xmm5;			// YR + YB
		paddsw		xmm1, xmm6;			// VR + VB  UR + UB
		psrld		xmm5, 10h;
		psrld		xmm6, 10h;
		paddsw		xmm0, xmm2;
		paddsw		xmm1, xmm2;
		paddsw		xmm6, OWORD PTR[gc_nOff128_5];
		paddsw		xmm0, xmm5;			// Y
		paddsw		xmm1, xmm6;			// U V
		psrlw		xmm0, 05h;
		psrlw		xmm1, 05h;
		pminsw		xmm0, xmm3;
		pminsw		xmm1, xmm3;
		shufps		xmm0, xmm0, 88h;	// Y1 Y0 Y1 Y0
		shufps		xmm1, xmm1, 88h;	// V0 U0 V0 U0
		pand		xmm0, xmm3;			// 0 Y1 0 Y0 0 Y1 0 Y0
		pand		xmm1, xmm3;			// 0 V0 0 U0 0 V0 0 U0
		packssdw	xmm0, xmm0;			// Y1Y0 Y1Y0 Y1Y0 Y1Y0
		packssdw	xmm1, xmm1;			// V0U0 V0U0 V0U0 V0U0
		pslld		xmm1, 08h;
		movd		rax, xmm7;			// 00AA
		por			xmm0, xmm1;			// V0Y1U0Y0
		mov			[rdx], ax;
		movss		DWORD PTR[rdi], xmm0;
		add			rsi, 08h;
		add			rdi, 04h;
		add			rdx, 02h;
loop_m4_color32_to_yuyva_inner:
		test		rcx, 04h;
		jz			loop_end_color32_to_yuyva_inner;
		movss		xmm7, DWORD PTR[rsi];		// 0000 0000 0000 ABGR
		movdqa		xmm2, OWORD PTR[gc_nRoundComp_5];
		movdqa		xmm3, OWORD PTR[gc_dwMaskDLB];
		pshufd		xmm4, xmm7, 00h;
		pxor		xmm0, xmm0;
		movdqu		xmm5, [rax];  	    // Y
		movdqu		xmm6, [rbx];        // UV
		punpcklbw	xmm0, xmm4;			// A0B0 G0R0 A0B0 G0R0
		psrld		xmm7, 18h;			// 0000 0000 0000 000A
		psrld		xmm4, 10h;
		psrlw		xmm0, 01h;			// 放大 7 位
		pand		xmm4, xmm3;			// 00A0 00A0 00A0 00A0
		pmulhw		xmm5, xmm0;			// 还放大 5 位
		pmulhw		xmm6, xmm0;
		por			xmm7, xmm4;			// 00A0 00A0 00A0 00AA
		pshufd		xmm0, xmm5, 0b1h;
		pshufd		xmm1, xmm6, 0b1h;
		paddsw		xmm0, xmm5;			// YR + YB
		paddsw		xmm1, xmm6;			// VR + VB  UR + UB
		psrld		xmm5, 10h;
		psrld		xmm6, 10h;
		paddsw		xmm0, xmm2;
		paddsw		xmm1, xmm2;
		paddsw		xmm6, OWORD PTR[gc_nOff128_5];
		paddsw		xmm0, xmm5;			// Y
		paddsw		xmm1, xmm6;			// U V
		psrlw		xmm0, 05h;
		psrlw		xmm1, 05h;
		pminsw		xmm0, xmm3;
		pminsw		xmm1, xmm3;
		shufps		xmm0, xmm0, 88h;	// Y1 Y0 Y1 Y0
		shufps		xmm1, xmm1, 88h;	// V0 U0 V0 U0
		pand		xmm0, xmm3;			// 0 Y1 0 Y0 0 Y1 0 Y0
		pand		xmm1, xmm3;			// 0 V0 0 U0 0 V0 0 U0
		packssdw	xmm0, xmm0;			// Y1Y0 Y1Y0 Y1Y0 Y1Y0
		packssdw	xmm1, xmm1;			// V0U0 V0U0 V0U0 V0U0
		pslld		xmm1, 08h;
		movd		rax, xmm7;			// 00AA
		por			xmm0, xmm1;			// V0Y1U0Y0
		mov			[rdx], ax;
		movss		DWORD PTR[rdi], xmm0;
loop_end_color32_to_yuyva_inner:	
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 10h]
		add			rsp,20h
		pop			rbx
		pop			rdi
		pop			rsi
		ret

;--------------------------------------------------------------------------
;void yuv422_to_color32_inner_x64(OUT void* color32,     // *G@A 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
;							 IN void* y,            // y 输入，存放方式 Y0 Y1 Y2 Y3 ……
;							 IN void* u,            // u 输入，存放方式 UU0 UU2 UU4 UU6 ……
;							 IN void* v,            // v 输入，存放方式 VV0 VV2 VV4 VV6 ……
;							 IN int na8,		    // 源像素宽度减 1 后除以 8 的商与余数
;							 IN int nm8,
;							 IN int* pnR_V,         // 系数
;							 IN int* pnG_U,         // （通过交换 U 与 V 可以做到 RGBA 与 BGRA 的兼容）
;							 IN int* pnG_V,         // （输入 u 与 v 交换，pnR_V 与 pnB_U 交换，pnG_V 与 pnG_U 交换即可实现）
;							 IN int* pnB_U)
;--------------------------------------------------------------------------							 		
yuv422_to_color32_inner_x64:
	;rcx			- color32
	;rdx			- y
	;r8				- u
	;r9				- v
	;[rsp+28h]		- na8
	;[rsp+30h]		- nm8
	;[rsp+38h]		- pnR_V
	;[rsp+40h]		- pnG_U
	;[rsp+48h]		- pnG_V
	;[rsp+50h]		- pnB_U
	
		push		rsi
		push		rdi
		push		rbx
		sub			rsp,20h
		movups		[rsp],xmm6
		movups		[rsp + 10h],xmm7

		mov			rbx, rdx;
		mov			rdx, r8;
		mov			rsi, r9;
		mov			rdi, rcx;
		mov			rcx, [rsp+28h+38h];

		pxor        xmm0, xmm0;
		test		rcx, rcx;
		jnz			loop_a8_yuv422_to_color32_inner;
		jmp			loop_m8_yuv422_to_color32_inner;
loop_a8_yuv422_to_color32_inner:
		movsd       xmm1, QWORD PTR[rbx];        // 0000  0000  Y7 Y6 Y5 Y4  Y3 Y2 Y1 Y0
		movss       xmm7, DWORD PTR[rdx];        // 0000  0000  0000  U6 U4 U2 U0
		movss       xmm6, DWORD PTR[rsi];        // 0000  0000  0000  V6 V4 V2 V0
		push        rdx;
		push        rcx;
		push        rbx;
		xor         rax, rax;
		xor         rcx, rcx;
		mov         al, [rdx + 04h];   // 000 U8
		mov         cl, [rsi + 04h];   // 000 V8
		punpcklbw   xmm7, xmm0;         // 0 0 0 0 U6 U4 U2 U0
		punpcklbw   xmm6, xmm0;         // 0 0 0 0 V6 V4 V2 V0
		punpcklbw   xmm1, xmm0;         // Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
		punpcklwd   xmm7, xmm0;         // 0 U6 0 U4 0 U2 0 U0
		punpcklwd   xmm6, xmm0;         // 0 V6 0 V4 0 V2 0 V0
		movdqa      xmm5, xmm7;
		movdqa      xmm4, xmm6;
		pslld       xmm5, 10h;         // U6 0 U4 0 U2 0 U0 0
		pslld       xmm4, 10h;         // V6 0 V4 0 V2 0 V0 0
		movdqa      xmm3, xmm5;
		movdqa      xmm2, xmm4;
		shufps      xmm3, xmm3, 39h;   // U0 0 U6 0 U4 0 U2 0
		shufps      xmm2, xmm2, 39h;   // V0 0 V6 0 V4 0 V2 0
		pinsrw      xmm3, rax, 07h;    // U8 0 U6 0 U4 0 U2 0
		pinsrw      xmm2, rcx, 07h;    // V8 0 V6 0 V4 0 V2 0
		por         xmm5, xmm7;         // U6 U6 U4 U4 U2 U2 U0 U0
		por         xmm3, xmm7;         // U8 U6 U6 U4 U4 U2 U2 U0
		por         xmm4, xmm6;         // V6 V6 V4 V4 V2 V2 V0 V0
		por         xmm2, xmm6;         // V8 V6 V6 V4 V4 V2 V2 V0
		paddw       xmm5, xmm3;         // U7 U6 U5 U4 U3 U2 U1 U0
		paddw       xmm4, xmm2;         // V7 V6 V5 V4 V3 V2 V1 V0
		movdqa      xmm7, OWORD PTR[gc_nOff128_7];
		psllw       xmm5, 06h;         // 总共放大 7 位
		psllw       xmm4, 06h;
		psllw       xmm1, 05h;         // 放大 5 位
		psubw       xmm5, xmm7;         // - 128
		psubw       xmm4, xmm7;
		mov         rax, [rsp+50h+38h];		//pnR_V;
		mov         rbx, [rsp+60h+38h];
		mov         rcx, [rsp+58h+38h];
		mov         rdx, [rsp+68h+38h];
		movdqu		xmm2, [rax];
		movdqu		xmm3, [rbx];
		movdqu		xmm6, [rcx];
		movdqu		xmm7, [rdx];
		pop         rbx;
		pop         rcx;
		pop         rdx;
		pmulhw		xmm2, xmm4;         // 还放大 5 位
		pmulhw		xmm3, xmm4;
		pmulhw		xmm6, xmm5;
		pmulhw		xmm7, xmm5;
		movdqa      xmm4, OWORD PTR[gc_nRoundComp_5];
		movdqa      xmm5, OWORD PTR[gc_dwMaskWLB]; // 255
		paddw       xmm2, xmm1;         // R
		paddw       xmm3, xmm1;         // G
		paddw       xmm6, xmm4;
		paddw       xmm7, xmm1;         // B
		paddw       xmm2, xmm4;         // R
		paddw       xmm3, xmm6;         // G
		paddw       xmm7, xmm4;         // B
		pmaxsw		xmm2, xmm0;			// R
		pmaxsw		xmm3, xmm0;			// G
		pmaxsw		xmm7, xmm0;			// B
		psrlw		xmm2, 05h;
		psrlw		xmm3, 05h;
		psrlw		xmm7, 05h;
		pminsw		xmm2, xmm5;		// R7 R6 R5 R4 R3 R2 R1 R0
		pminsw		xmm3, xmm5;		// G7 G6 G5 G4 G3 G2 G1 G0
		pminsw		xmm7, xmm5;		// B7 B6 B5 B4 B3 B2 B1 B0
		psllw		xmm3, 08h;
		pxor        xmm4, xmm4;
		pxor        xmm5, xmm5;
		por			xmm2, xmm3;		// G7R7 G6R6 G5R5 G4R4 G3R3 G2R2 G1R1 G0R0
		movdqa		xmm1, OWORD PTR[gc_dwMaskAlpha];	// A000 A000 A000 A000
		movdqa		xmm6, xmm2;
		punpcklwd	xmm2, xmm5;			// 00 G3R3 00 G2R2 00 G1R1 00 G0R0
		punpckhwd	xmm6, xmm5;			// 00 G7R7 00 G6R6 00 G5R5 00 G4R4
		punpcklwd	xmm4, xmm7;			// 0B3 00 0B2 00 0B1 00 0B0 00
		punpckhwd	xmm5, xmm7;			// 0B7 00 0B6 00 0B5 00 0B4 00
		por			xmm2, xmm1;
		por			xmm6, xmm1;
		por			xmm2, xmm4;
		por			xmm6, xmm5;
		movdqu		[rdi], xmm2;
		add			rbx, 08h;
		add			rdi, 10h;
		add			rdx, 04h;
		movdqu		[rdi], xmm6;
		add			rsi, 04h;
		add			rdi, 10h;
		sub			rcx, 01h;
		jnz			loop_a8_yuv422_to_color32_inner;
loop_m8_yuv422_to_color32_inner:
		mov			rcx, [rsp+30h+38h];
		test		rcx, 08h;
		jz			loop_m5_yuv422_to_color32_inner;
		movsd       xmm1, QWORD PTR[rbx];        // 0000  0000  Y7 Y6 Y5 Y4  Y3 Y2 Y1 Y0
		movss       xmm7, DWORD PTR[rdx];        // 0000  0000  0000  U6 U4 U2 U0
		movss       xmm6, DWORD PTR[rsi];        // 0000  0000  0000  V6 V4 V2 V0
		punpcklbw   xmm7, xmm0;         // 0 0 0 0 U6 U4 U2 U0
		punpcklbw   xmm6, xmm0;         // 0 0 0 0 V6 V4 V2 V0
		punpcklbw   xmm1, xmm0;         // Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
		punpcklwd   xmm7, xmm0;         // 0 U6 0 U4 0 U2 0 U0
		punpcklwd   xmm6, xmm0;         // 0 V6 0 V4 0 V2 0 V0
		movdqa      xmm5, xmm7;
		movdqa      xmm4, xmm6;
		pslld       xmm5, 10h;         // U6 0 U4 0 U2 0 U0 0
		pslld       xmm4, 10h;         // V6 0 V4 0 V2 0 V0 0
		movdqa      xmm3, xmm5;
		movdqa      xmm2, xmm4;
		shufps      xmm3, xmm3, 0f9h;   // U6 0 U6 0 U4 0 U2 0
		shufps      xmm2, xmm2, 0f9h;   // V6 0 V6 0 V4 0 V2 0
		por         xmm5, xmm7;         // U6 U6 U4 U4 U2 U2 U0 U0
		por         xmm3, xmm7;         // U6 U6 U6 U4 U4 U2 U2 U0
		por         xmm4, xmm6;         // V6 V6 V4 V4 V2 V2 V0 V0
		por         xmm2, xmm6;         // V6 V6 V6 V4 V4 V2 V2 V0
		paddw       xmm5, xmm3;         // U7 U6 U5 U4 U3 U2 U1 U0
		paddw       xmm4, xmm2;         // V7 V6 V5 V4 V3 V2 V1 V0
		movdqa      xmm7, OWORD PTR[gc_nOff128_7];
		psllw       xmm5, 06h;         // 总共放大 7 位
		psllw       xmm4, 06h;
		psllw       xmm1, 05h;         // 放大 5 位
		psubw       xmm5, xmm7;         // - 128
		psubw       xmm4, xmm7;
		mov         rax, [rsp+38h+38h];
		mov         rbx, [rsp+48h+38h]	;
		mov         rcx, [rsp+40h+38h];
		mov         rdx, [rsp+50h+38h];
		movdqu		xmm2, [rax];
		movdqu		xmm3, [rbx];
		movdqu		xmm6, [rcx];
		movdqu		xmm7, [rdx];
		pmulhw		xmm2, xmm4;         // 还放大 5 位
		pmulhw		xmm3, xmm4;
		pmulhw		xmm6, xmm5;
		pmulhw		xmm7, xmm5;
		movdqa      xmm4, OWORD PTR[gc_nRoundComp_5];
		movdqa      xmm5, OWORD PTR[gc_dwMaskWLB]; // 255
		paddw       xmm2, xmm1;         // R
		paddw       xmm3, xmm1;         // G
		paddw       xmm6, xmm4;
		paddw       xmm7, xmm1;         // B
		paddw       xmm2, xmm4;         // R
		paddw       xmm3, xmm6;         // G
		paddw       xmm7, xmm4;         // B
		pmaxsw		xmm2, xmm0;			// R
		pmaxsw		xmm3, xmm0;			// G
		pmaxsw		xmm7, xmm0;			// B
		psrlw		xmm2, 05h;
		psrlw		xmm3, 05h;
		psrlw		xmm7, 05h;
		pminsw		xmm2, xmm5;		// R7 R6 R5 R4 R3 R2 R1 R0
		pminsw		xmm3, xmm5;		// G7 G6 G5 G4 G3 G2 G1 G0
		pminsw		xmm7, xmm5;		// B7 B6 B5 B4 B3 B2 B1 B0
		psllw		xmm3, 08h;
		pxor        xmm4, xmm4;
		pxor        xmm5, xmm5;
		por			xmm2, xmm3;		// G7R7 G6R6 G5R5 G4R4 G3R3 G2R2 G1R1 G0R0
		movdqa		xmm1, OWORD PTR[gc_dwMaskAlpha];	// A000 A000 A000 A000
		movdqa		xmm6, xmm2;
		punpcklwd	xmm2, xmm5;			// 00 G3R3 00 G2R2 00 G1R1 00 G0R0
		punpckhwd	xmm6, xmm5;			// 00 G7R7 00 G6R6 00 G5R5 00 G4R4
		punpcklwd	xmm4, xmm7;			// 0B3 00 0B2 00 0B1 00 0B0 00
		punpckhwd	xmm5, xmm7;			// 0B7 00 0B6 00 0B5 00 0B4 00
		por			xmm2, xmm1;
		por			xmm6, xmm1;
		por			xmm2, xmm4;
		por			xmm6, xmm5;
		movdqu		[rdi], xmm2;
		add			rdi, 10h;
		movdqu		[rdi], xmm6;
		jmp			loop_end_yuv422_to_color32_inner;
loop_m5_yuv422_to_color32_inner:
		cmp			rcx, 04h;
		jle			loop_m4_yuv422_to_color32_inner;
		pxor        xmm7, xmm7;
		movss       xmm1, DWORD PTR[rbx];        // 0000  0000  0000  Y3 Y2 Y1 Y0
		pinsrw      xmm7, WORD PTR[rdx], 00h;  // 0000  0000  0000  00 U2 U0
		pinsrw      xmm7, WORD PTR[rsi], 01h;  // 0000  0000  0000  V2 V0 U2 U0
		push        rdx;
		push        rcx;
		push        rbx;
		xor         rax, rax;
		xor         rcx, rcx;
		mov         al, [rdx + 02h];   // 000 U4
		mov         cl, [rsi + 02h];   // 000 V4
		punpcklbw   xmm7, xmm0;         // 0 0 0 0 V2 V0 U2 U0
		punpcklbw   xmm1, xmm0;         // 0 0 0 0 Y3 Y2 Y1 Y0
		punpcklwd   xmm7, xmm0;         // 0 V2 0 V0 0 U2 0 U0
		movdqa      xmm5, xmm7;
		pslld       xmm5, 10h;         // V2 0 V0 0 U2 0 U0 0
		movdqa      xmm3, xmm5;
		movdqa      xmm4, OWORD PTR[gc_nOff128_7];
		shufps      xmm3, xmm3, 0b1h;   // V0 0 V2 0 U0 0 U2 0
		pinsrw      xmm3, rax, 03h;    // V0 0 V2 0 U4 0 U2 0
		pinsrw      xmm3, rcx, 07h;    // V4 0 V2 0 U4 0 U2 0
		por         xmm5, xmm7;         // V2 V2 V0 V0 U2 U2 U0 U0
		por         xmm3, xmm7;         // V4 V2 V2 V0 U4 U2 U2 U0
		paddw       xmm5, xmm3;         // V3 V2 V1 V0 U3 U2 U1 U0
		psllw       xmm5, 06h;         // 总共放大 7 位
		psllw       xmm1, 05h;         // 放大 5 位
		psubw       xmm5, xmm4;         // - 128
		movhlps     xmm4, xmm5;         // @ @ @ @ V3 V2 V1 V0
		mov         rax, [rsp+50h+38h];		//pnR_V;
		mov         rbx, [rsp+60h+38h];		//pnG_V;
		mov         rcx, [rsp+58h+38h];		//pnG_U;
		mov         rdx, [rsp+68h+38h];		//pnB_U;
		movdqu		xmm2, [rax];
		movdqu		xmm3, [rbx];
		movdqu		xmm6, [rcx];
		movdqu		xmm7, [rdx];
		pop         rbx;
		pop         rcx;
		pop         rdx;
		pmulhw		xmm2, xmm4;         // 还放大 5 位
		pmulhw		xmm3, xmm4;
		pmulhw		xmm6, xmm5;
		pmulhw		xmm7, xmm5;
		movdqa      xmm4, OWORD PTR[gc_nRoundComp_5];
		movdqa      xmm5, OWORD PTR[gc_dwMaskWLB]; // 255
		paddw       xmm2, xmm1;         // R
		paddw       xmm3, xmm1;         // G
		paddw       xmm6, xmm4;
		paddw       xmm7, xmm1;         // B
		paddw       xmm2, xmm4;         // R
		paddw       xmm3, xmm6;         // G
		paddw       xmm7, xmm4;         // B
		pmaxsw		xmm2, xmm0;			// R
		pmaxsw		xmm3, xmm0;			// G
		pmaxsw		xmm7, xmm0;			// B
		psrlw		xmm2, 05h;
		psrlw		xmm3, 05h;
		psrlw		xmm7, 05h;
		pminsw		xmm2, xmm5;		// @ @ @ @ R3 R2 R1 R0
		pminsw		xmm3, xmm5;		// @ @ @ @ G3 G2 G1 G0
		pminsw		xmm7, xmm5;		// @ @ @ @ B3 B2 B1 B0
		psllw		xmm3, 08h;
		pxor        xmm4, xmm4;
		por			xmm2, xmm3;		// @ @ @ @ G3R3 G2R2 G1R1 G0R0
		movdqa		xmm1, OWORD PTR[gc_dwMaskAlpha];	// A000 A000 A000 A000
		punpcklwd	xmm2, xmm0;			// 00 G3R3 00 G2R2 00 G1R1 00 G0R0
		punpcklwd	xmm4, xmm7;			// 0B3 00 0B2 00 0B1 00 0B0 00
		por			xmm2, xmm1;
		por			xmm2, xmm4;
		movdqu		[rdi], xmm2;
		add         rbx, 04h;
		add         rdx, 02h;
		add         rsi, 02h;
		add         rdi, 10h;
		jmp			loop_m3_yuv422_to_color32_inner;
loop_m4_yuv422_to_color32_inner:
		test		rcx, 08h;
		jz			loop_m3_yuv422_to_color32_inner;
		pxor        xmm7, xmm7;
		movss       xmm1, DWORD PTR[rbx];        // 0000  0000  0000  Y3 Y2 Y1 Y0
		pinsrw      xmm7, WORD PTR[rdx], 00h;  // 0000  0000  0000  00 U2 U0
		pinsrw      xmm7, WORD PTR[rsi], 01h;  // 0000  0000  0000  V2 V0 U2 U0
		punpcklbw   xmm7, xmm0;         // 0 0 0 0 V2 V0 U2 U0
		punpcklbw   xmm1, xmm0;         // 0 0 0 0 Y3 Y2 Y1 Y0
		punpcklwd   xmm7, xmm0;         // 0 V2 0 V0 0 U2 0 U0
		movdqa      xmm5, xmm7;
		pslld       xmm5, 10h;         // V2 0 V0 0 U2 0 U0 0
		movdqa      xmm3, xmm5;
		movdqa      xmm4, OWORD PTR[gc_nOff128_7];
		shufps      xmm3, xmm3, 0f5h;   // V2 0 V2 0 U2 0 U2 0
		por         xmm5, xmm7;         // V2 V2 V0 V0 U2 U2 U0 U0
		por         xmm3, xmm7;         // V2 V2 V2 V0 U2 U2 U2 U0
		paddw       xmm5, xmm3;         // V3 V2 V1 V0 U3 U2 U1 U0
		psllw       xmm5, 06h;         // 总共放大 7 位
		psllw       xmm1, 05h;         // 放大 5 位
		psubw       xmm5, xmm4;         // - 128
		movhlps     xmm4, xmm5;         // @ @ @ @ V3 V2 V1 V0
		mov         rax, [rsp+38h+38h];		//pnR_V;
		mov         rbx, [rsp+48h+38h];		//pnG_V;
		mov         rcx, [rsp+40h+38h];		//pnG_U;		
		mov         rdx, [rsp+50h+38h];		//pnB_U;
		movdqu		xmm2, [rax];
		movdqu		xmm3, [rbx];
		movdqu		xmm6, [rcx];
		movdqu		xmm7, [rdx];
		pmulhw		xmm2, xmm4;         // 还放大 5 位
		pmulhw		xmm3, xmm4;
		pmulhw		xmm6, xmm5;
		pmulhw		xmm7, xmm5;
		movdqa      xmm4, OWORD PTR[gc_nRoundComp_5];
		movdqa      xmm5, OWORD PTR[gc_dwMaskWLB]; // 255
		paddw       xmm2, xmm1;         // R
		paddw       xmm3, xmm1;         // G
		paddw       xmm6, xmm4;
		paddw       xmm7, xmm1;         // B
		paddw       xmm2, xmm4;         // R
		paddw       xmm3, xmm6;         // G
		paddw       xmm7, xmm4;         // B
		pmaxsw		xmm2, xmm0;			// R
		pmaxsw		xmm3, xmm0;			// G
		pmaxsw		xmm7, xmm0;			// B
		psrlw		xmm2, 05h;
		psrlw		xmm3, 05h;
		psrlw		xmm7, 05h;
		pminsw		xmm2, xmm5;		// @ @ @ @ R3 R2 R1 R0
		pminsw		xmm3, xmm5;		// @ @ @ @ G3 G2 G1 G0
		pminsw		xmm7, xmm5;		// @ @ @ @ B3 B2 B1 B0
		psllw		xmm3, 08h;
		pxor        xmm4, xmm4;
		por			xmm2, xmm3;		// @ @ @ @ G3R3 G2R2 G1R1 G0R0
		movdqa		xmm1, OWORD PTR[gc_dwMaskAlpha];	// A000 A000 A000 A000
		punpcklwd	xmm2, xmm0;			// 00 G3R3 00 G2R2 00 G1R1 00 G0R0
		punpcklwd	xmm4, xmm7;			// 0B3 00 0B2 00 0B1 00 0B0 00
		por			xmm2, xmm1;
		por			xmm2, xmm4;
		movdqu		[rdi], xmm2;
		jmp			loop_end_yuv422_to_color32_inner;
loop_m3_yuv422_to_color32_inner:
		cmp         rcx, 03h;
		jnz         loop_m2_yuv422_to_color32_inner;
		pxor        xmm1, xmm1;
		pxor        xmm7, xmm7;
		pinsrw      xmm1, WORD PTR[rbx], 00h;  // 0000  0000  0000  00 Y1 Y0
		pinsrw      xmm7, WORD PTR[rdx], 00h;  // 0000  0000  0000  00 U2 U0
		pinsrw      xmm7, WORD PTR[rsi], 01h;  // 0000  0000  0000  V2 V0 U2 U0
		xor         rax, rax;
		mov         al, [rbx + 02h];   // 000 Y2
		pinsrw      xmm1, rax, 01h;    // 0000  0000  0000  0 Y2 Y1 Y0
		punpcklbw   xmm7, xmm0;         // 0 0 0 0 V2 V0 U2 U0
		punpcklbw   xmm1, xmm0;         // 0 0 0 0 0 Y2 Y1 Y0
		punpcklwd   xmm7, xmm0;         // 0 V2 0 V0 0 U2 0 U0
		movdqa      xmm5, xmm7;
		pslld       xmm5, 10h;         // V2 0 V0 0 U2 0 U0 0
		movdqa      xmm3, xmm5;
		movdqa      xmm4, OWORD PTR[gc_nOff128_7];
		shufps      xmm3, xmm3, 0f5h;   // V2 0 V2 0 U2 0 U2 0
		por         xmm5, xmm7;         // V2 V2 V0 V0 U2 U2 U0 U0
		por         xmm3, xmm7;         // V2 V2 V2 V0 U2 U2 U2 U0
		paddw       xmm5, xmm3;         // V2 V2 V1 V0 U2 U2 U1 U0
		psllw       xmm5, 06h;         // 总共放大 7 位
		psllw       xmm1, 05h;         // 放大 5 位
		psubw       xmm5, xmm4;         // - 128
		movhlps     xmm4, xmm5;         // @ @ @ @ V2 V2 V1 V0
		mov         rax, [rsp+38h+38h];		//pnR_V;
		mov         rbx, [rsp+48h+38h];		//pnG_V;
		mov         rcx, [rsp+40h+38h];		//pnG_U;
		mov         rdx, [rsp+50h+38h];		//pnB_U;
		movdqu		xmm2, [rax];
		movdqu		xmm3, [rbx];
		movdqu		xmm6, [rcx];
		movdqu		xmm7, [rdx];
		pmulhw		xmm2, xmm4;         // 还放大 5 位
		pmulhw		xmm3, xmm4;
		pmulhw		xmm6, xmm5;
		pmulhw		xmm7, xmm5;
		movdqa      xmm4, OWORD PTR[gc_nRoundComp_5];
		movdqa      xmm5, OWORD PTR[gc_dwMaskWLB]; // 255
		paddw       xmm2, xmm1;         // R
		paddw       xmm3, xmm1;         // G
		paddw       xmm6, xmm4;
		paddw       xmm7, xmm1;         // B
		paddw       xmm2, xmm4;         // R
		paddw       xmm3, xmm6;         // G
		paddw       xmm7, xmm4;         // B
		pmaxsw		xmm2, xmm0;			// R
		pmaxsw		xmm3, xmm0;			// G
		pmaxsw		xmm7, xmm0;			// B
		psrlw		xmm2, 05h;
		psrlw		xmm3, 05h;
		psrlw		xmm7, 05h;
		pminsw		xmm2, xmm5;		// @ @ @ @ R2 R2 R1 R0
		pminsw		xmm3, xmm5;		// @ @ @ @ G2 G2 G1 G0
		pminsw		xmm7, xmm5;		// @ @ @ @ B2 B2 B1 B0
		psllw		xmm3, 08h;
		pxor        xmm4, xmm4;
		por			xmm2, xmm3;		// @ @ @ @ G2R2 G2R2 G1R1 G0R0
		movdqa		xmm1, OWORD PTR[gc_dwMaskAlpha];	// A000 A000 A000 A000
		punpcklwd	xmm2, xmm0;			// 00 G2R2 00 G2R2 00 G1R1 00 G0R0
		punpcklwd	xmm4, xmm7;			// 0B2 00 0B2 00 0B1 00 0B0 00
		por			xmm2, xmm1;
		por			xmm2, xmm4;     // D2 D2 D1 D1
		movhlps     xmm3, xmm2;     // @ @ D2 D2
		movsd		QWORD PTR[rdi], xmm2;
		add         rdi, 08h;
		movss		DWORD PTR[rdi], xmm3;
		jmp         loop_end_yuv422_to_color32_inner;
loop_m2_yuv422_to_color32_inner:
		test        rcx, 02h;
		jz          loop_m1_yuv422_to_color32_inner;
		xor         rax, rax;
		xor         rcx, rcx;
		mov         al, [rdx];      // 000 U0
		mov         cl, [rsi];      // 000 V0
		mov         ah, al;         // 00 U0 U0
		mov         ch, cl;         // 00 V0 V0
		pxor        xmm1, xmm1;
		pxor        xmm7, xmm7;
		pinsrw      xmm1, WORD PTR[rbx], 00h;  // 0000  0000  0000  00 Y1 Y0
		pinsrw      xmm7, rax, 00h;    // 0000  0000  0000  00 U1 U0
		pinsrw      xmm7, rcx, 02h;     // 0 0 0 0 0 0 Y1 Y0
			punpcklbw   xmm7, xmm0;         // 0 0 V1 V0 0 0 U1 U0
		psllw       xmm1, 05h;         // 放大 5 位
		psllw       xmm7, 07h;         // 总共放大 7 位
		psubw       xmm7, OWORD PTR[gc_nOff128_7]; // - 128
		movhlps     xmm4, xmm7;         // @ @ @ @ @ @ V1 V0
		mov         rax, [rsp+38h+38h];		//pnR_V;
		mov         rbx, [rsp+48h+38h];		//pnG_V;
		mov         rcx, [rsp+40h+38h];		//pnG_U;
		mov         rdx, [rsp+50h+38h];		//pnB_U;
		movdqu		xmm2, [rax];
		movdqu		xmm3, [rbx];
		movdqu		xmm5, [rcx];
		movdqu		xmm6, [rdx];
		pmulhw		xmm2, xmm4;         // 还放大 5 位
		pmulhw		xmm3, xmm4;
		pmulhw		xmm5, xmm7;
		pmulhw		xmm6, xmm7;
		movdqa      xmm4, OWORD PTR[gc_nRoundComp_5];
		movdqa      xmm7, OWORD PTR[gc_dwMaskWLB]; // 255
		paddw       xmm2, xmm1;         // R
		paddw       xmm3, xmm1;         // G
		paddw       xmm5, xmm4;
		paddw       xmm6, xmm1;         // B
		paddw       xmm2, xmm4;         // R
		paddw       xmm3, xmm5;         // G
		paddw       xmm6, xmm4;         // B
		pmaxsw		xmm2, xmm0;			// R
		pmaxsw		xmm3, xmm0;			// G
		pmaxsw		xmm6, xmm0;			// B
		psrlw		xmm2, 05h;
		psrlw		xmm3, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm2, xmm7;		// @ @ @ @ @ @ R1 R0
		pminsw		xmm3, xmm7;		// @ @ @ @ @ @ G1 G0
		pminsw		xmm6, xmm7;		// @ @ @ @ @ @ B1 B0
		psllw		xmm3, 08h;
		pxor        xmm4, xmm4;
		por			xmm2, xmm3;		// @ @ @ @ @ @ G1R1 G0R0
		movdqa		xmm1, OWORD PTR[gc_dwMaskAlpha];	// A000 A000 A000 A000
		punpcklwd	xmm2, xmm0;			// 00 @ 00 @ 00 G1R1 00 G0R0
		punpcklwd	xmm4, xmm6;			// 0@ 00 @ 00 0B1 00 0B0 00
		por			xmm2, xmm1;
		por			xmm2, xmm4;     // @ @ D1 D1
		movsd		QWORD PTR[rdi], xmm2;
		jmp         loop_end_yuv422_to_color32_inner;
loop_m1_yuv422_to_color32_inner:
		test        rcx, 01h;
		jz          loop_end_yuv422_to_color32_inner;
		xor         rax, rax;
		xor         rcx, rcx;
		mov         al, [rbx];      // 000 Y
		mov         cl, [rsi];      // 000 V
		xor         rbx, rbx;
		mov         bl, [rdx];      // 000 U
		pxor        xmm1, xmm1;
		pxor        xmm7, xmm7;
		pinsrw      xmm1, rax, 00h;    // 0000  0000  0000  000 Y
		pinsrw      xmm7, rbx, 00h;    // 0000  0000  0000  000 U
		pinsrw      xmm7, rcx, 04h;    // 0000  000 V  0000  000 U
		psllw       xmm1, 05h;
		psllw       xmm7, 07h;     // 放大 7 位
		psubw       xmm7, OWORD PTR[gc_nOff128_7]; // - 128
		mov         eax, [rsp+38h+38h];		//pnR_V;
		mov         ebx, [rsp+48h+38h];		//pnG_V;
		mov         ecx, [rsp+40h+38h];		//pnG_U;
		mov         edx, [rsp+50h+38h];		//pnB_U;
		movhlps     xmm4, xmm7;     // @ @ @ @ @ @ @ V
		movdqu		xmm2, [rax];
		movdqu		xmm3, [rbx];
		movdqu		xmm5, [rcx];
		movdqu		xmm6, [rdx];
		pmulhw		xmm2, xmm4;         // 还放大 5 位
		pmulhw		xmm3, xmm4;
		pmulhw		xmm5, xmm7;
		pmulhw		xmm6, xmm7;
		movdqa      xmm4, OWORD PTR[gc_nRoundComp_5];
		movdqa      xmm7, OWORD PTR[gc_dwMaskWLB]; // 255
		paddw       xmm2, xmm1;         // R
		paddw       xmm3, xmm1;         // G
		paddw       xmm5, xmm4;
		paddw       xmm6, xmm1;         // B
		paddw       xmm2, xmm4;         // R
		paddw       xmm3, xmm5;         // G
		paddw       xmm6, xmm4;         // B
		pmaxsw		xmm2, xmm0;			// R
		pmaxsw		xmm3, xmm0;			// G
		pmaxsw		xmm6, xmm0;			// B
		psrlw		xmm2, 05h;
		psrlw		xmm3, 05h;
		psrlw		xmm6, 05h;
		pminsw		xmm2, xmm7;		// @ @ @ @ @ @ @ R
		pminsw		xmm3, xmm7;		// @ @ @ @ @ @ @ G
		pminsw		xmm6, xmm7;		// @ @ @ @ @ @ @ B
		psllw		xmm3, 08h;
		pxor        xmm4, xmm4;
		por			xmm2, xmm3;		// @ @ @ @ @ @ @ GR
		movdqa		xmm1, OWORD PTR[gc_dwMaskAlpha];	// A000 A000 A000 A000
		punpcklwd	xmm2, xmm0;			// 00 @ 00 @ 00 @ 00 GR
		punpcklwd	xmm4, xmm6;			// 0@ 00 @ 00 @ 00 0B 00
		por			xmm2, xmm1;
		por			xmm2, xmm4;     // @ @ @ D
		movss		DWORD PTR[rdi], xmm2;
loop_end_yuv422_to_color32_inner:
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 10h]
		add			rsp,20h
		pop			rbx
		pop			rdi
		pop			rsi
		ret

;--------------------------------------------------------------------------
;void uv20_to_uv22_line_1_L_x64(OUT void* u_o,         // 422 u 输出，存储方式 U0 U2 U4 U6 ……（两行）
;						   OUT void* v_o,         // 422 v 输出，存储方式 V0 V2 V4 V6 ……（两行）
;						   IN void* u_i,          // 420 u 输入，存储方式 UU0 UU2 UU4 UU6 ……
;						   IN int u_i_p_2,
;						   IN void* v_i,          // 420 v 输入，存储方式 VV0 VV2 VV4 VV6 ……
;						   IN int v_i_p_2,
;						   IN int na16,           // 一行像素宽度的一半（因为一行内的 UV 与 Y 为 422 的关系）除以 16 的商与余数
;						   IN int nm16)
;--------------------------------------------------------------------------
uv20_to_uv22_line_1_L_x64:
	;rcx			- u_o
	;rdx			- v_o
	;r8				- u_i
	;r9				- u_i_p_2
	;[rsp+28h]		- v_i
	;[rsp+30h]		- v_i_p_2
	;[rsp+38h]		- na16
	;[rsp+40h]		- nm16

		push		rsi
		push		rdi
		push		rbx
		sub			rsp,20h
		movups		[rsp],xmm6
		movups		[rsp + 10h],xmm7
	
		mov         rdi, rcx;
		mov         rcx, [rsp+38h+38h];
		
		mov         rdx, rdx;
		mov         rsi, rdx;
		mov         rbx, rdx;
		mov         rax, [rsp+28h+38h];
		sub         rbx, r9;
		pxor        xmm0, xmm0;
		movdqa      xmm7, OWORD PTR[gc_nRoundComp_2];
		test        rcx, rcx;
		jnz         loop_a16_uv20_to_uv22_line_1_L;
		jmp         loop_m8_uv20_to_uv22_line_1_L;
loop_a16_uv20_to_uv22_line_1_L:
		push        rax;
		push        rcx;
		mov         rcx, rax;
		sub         rcx, [rsp+40h+38h];		//v_i_p_2;
		movdqu      xmm3, [rsi];        // UU30 UU28 UU26 UU24  UU22 UU20 UU18 UU16  UU14 UU12 UU10 UU8  UU6 UU4 UU2 UU0
		movdqu      xmm1, [rbx];        // UU_P
		movdqu      xmm6, [rax];        // VV_C
		movdqu      xmm5, [rcx];        // VV_P
		movdqa      xmm2, xmm1;
		movdqa      xmm4, xmm3;
		;// 结果：UU_P * 0.25 + UU_C * 0.75
		punpcklbw   xmm1, xmm0;         // UU_P  low
		punpcklbw   xmm3, xmm0;         // UU_C  low
		punpckhbw   xmm2, xmm0;         // UU_P  high
		punpckhbw   xmm4, xmm0;         // UU_C  high
		paddw       xmm1, xmm3;     // UU_P + UU_C
		paddw       xmm2, xmm4;
		paddw       xmm3, xmm3;     // UU_C * 2
		paddw       xmm4, xmm4;
		paddw       xmm1, xmm7;
		paddw       xmm2, xmm7;
		paddw       xmm1, xmm3;     // UU_P + UU_C * 3
		paddw       xmm2, xmm4;
		movdqa      xmm3, xmm5;
		movdqa      xmm4, xmm6;
		punpcklbw   xmm3, xmm0;         // VV_P  low
		punpcklbw   xmm4, xmm0;         // VV_C  low
		punpckhbw   xmm5, xmm0;         // VV_P  high
		punpckhbw   xmm6, xmm0;         // VV_C  high
		paddw       xmm3, xmm4;     // VV_P + VV_C
		paddw       xmm5, xmm6;
		paddw       xmm4, xmm4;     // VV_C * 2
		paddw       xmm6, xmm6;
		paddw       xmm3, xmm7;
		paddw       xmm5, xmm7;
		paddw       xmm3, xmm4;     // VV_P + VV_C * 3
		paddw       xmm5, xmm6;
		psrlw       xmm1, 02h;
		psrlw       xmm2, 02h;
		psrlw       xmm3, 02h;
		psrlw       xmm5, 02h;
		packuswb    xmm1, xmm2;     // UU_1
		packuswb    xmm3, xmm5;     // VV_1
		movdqu      [rdi], xmm1;
		movdqu      [rdx], xmm3;
		pop         rcx;
		pop         rax;
		add         rax, 10h;
		add         rbx, 10h;
		add         rsi, 10h;
		add         rdx, 10h;
		add         rdi, 10h;
		sub         rcx, 01h;
		jnz         loop_a16_uv20_to_uv22_line_1_L;
loop_m8_uv20_to_uv22_line_1_L:
		mov         rcx, [rsp+40h+38h];
		test        rcx, 08h;
		jz          loop_m4_uv20_to_uv22_line_1_L;
		push        rax;
		push        rcx;
		mov         rcx, rax;
		sub         rcx, [rsp+40h+38h];		//v_i_p_2;
		movsd       xmm3, QWORD PTR[rsi];        // UU30 UU28 UU26 UU24  UU22 UU20 UU18 UU16  UU14 UU12 UU10 UU8  UU6 UU4 UU2 UU0
		movsd       xmm1, QWORD PTR[rbx];        // UU_P
		movsd       xmm6, QWORD PTR[rax];        // VV_C
		movsd       xmm5, QWORD PTR[rcx];        // VV_P
		;// 结果：UU_P * 0.25 + UU_C * 0.75
		punpcklbw   xmm1, xmm0;         // UU_P
		punpcklbw   xmm3, xmm0;         // UU_C
		punpcklbw   xmm5, xmm0;         // VV_P
		punpcklbw   xmm6, xmm0;         // VV_C
		paddw       xmm1, xmm3;     // UU_P + UU_C
		paddw       xmm5, xmm6;     // VV_P + VV_C
		paddw       xmm3, xmm3;     // UU_C * 2
		paddw       xmm6, xmm6;     // VV_C * 2
		paddw       xmm1, xmm7;
		paddw       xmm5, xmm7;
		paddw       xmm1, xmm3;     // UU_P + UU_C * 3
		paddw       xmm5, xmm6;     // VV_P + VV_C * 3
		psrlw       xmm1, 02h;
		psrlw       xmm5, 02h;
		packuswb    xmm1, xmm5;     // VV_1    UU_1
		movsd       QWORD PTR[rdi], xmm1;
		movhps      QWORD PTR[rdx], xmm1;
		pop         rcx;
		pop         rax;
		add         rax, 08h;
		add         rbx, 08h;
		add         rsi, 08h;
		add         rdx, 08h;
		add         rdi, 08h;
loop_m4_uv20_to_uv22_line_1_L:
		test        rcx, 04h;
		jz          loop_m2_uv20_to_uv22_line_1_L;
		push        rax;
		push        rcx;
		mov         rcx, rax;
		sub         rcx, [rsp+40h+38h];		//v_i_p_2;
		movss       xmm3, DWORD PTR[rsi];        // UU30 UU28 UU26 UU24  UU22 UU20 UU18 UU16  UU14 UU12 UU10 UU8  UU6 UU4 UU2 UU0
		movss       xmm1, DWORD PTR[rbx];        // UU_P
		movss       xmm6, DWORD PTR[rax];        // VV_C
		movss       xmm5, DWORD PTR[rcx];        // VV_P
		;// 结果：UU_P * 0.25 + UU_C * 0.75
		punpcklbw   xmm1, xmm0;         // UU_P
		punpcklbw   xmm3, xmm0;         // UU_C
		punpcklbw   xmm5, xmm0;         // VV_P
		punpcklbw   xmm6, xmm0;         // VV_C
		paddw       xmm1, xmm3;     // UU_P + UU_C
		paddw       xmm5, xmm6;     // VV_P + VV_C
		paddw       xmm3, xmm3;     // UU_C * 2
		paddw       xmm6, xmm6;     // VV_C * 2
		paddw       xmm1, xmm7;
		paddw       xmm5, xmm7;
		paddw       xmm1, xmm3;     // UU_P + UU_C * 3
		paddw       xmm5, xmm6;     // VV_P + VV_C * 3
		psrlw       xmm1, 02h;
		psrlw       xmm5, 02h;
		packuswb    xmm1, xmm5;     // VV_1    UU_1
		movhlps     xmm3, xmm1;
		movss       DWORD PTR[rdi], xmm1;
		movss       DWORD PTR[rdx], xmm3;
		pop         rcx;
		pop         rax;
		add         rax, 04h;
		add         rbx, 04h;
		add         rsi, 04h;
		add         rdx, 04h;
		add         rdi, 04h;
loop_m2_uv20_to_uv22_line_1_L:
		test        rcx, 02h;
		jz          loop_end_uv20_to_uv22_line_1_L;
		mov         rcx, rax;
		sub         rcx, [rsp+30h+38h];		//v_i_p_2;
		pinsrw      xmm1, WORD PTR[rbx], 00h;  // UU_P
		pinsrw      xmm3, WORD PTR[rsi], 00h;  // @@@@  @@@@  @@@@  @ @ UU2 UU0
		pinsrw      xmm1, WORD PTR[rcx], 02h;  // VV_P
		pinsrw      xmm3, WORD PTR[rax], 02h;  // VV_C
		;// 结果：VV_P * 0.25 + VV_C * 0.75
		punpcklbw   xmm1, xmm0;         // VV_P  UU_P
		punpcklbw   xmm3, xmm0;         // VV_C  UU_C
		paddw       xmm1, xmm3;     // VV_P + VV_C    UU_P + UU_C
		paddw       xmm3, xmm3;     // VV_C * 2    UU_C * 2
		paddw       xmm1, xmm7;
		paddw       xmm1, xmm3;     // VV_P + VV_C * 3    UU_P + UU_C * 3
		psrlw       xmm1, 02h;
		packuswb    xmm1, xmm1;     // @ V1 @ U1  @ V1 @ U1
		pextrw      rax, xmm1, 00h;    // U1
		pextrw      rbx, xmm1, 02h;    // V1
		mov         [rdi], ax;
		mov         [rdx], bx;
loop_end_uv20_to_uv22_line_1_L:	
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 10h]
		add			rsp,20h
		pop			rbx
		pop			rdi
		pop			rsi
		ret

;--------------------------------------------------------------------------
;void uv20_to_uv22_line_2_L_x64(OUT void* u_o,         // 422 u 输出，存储方式 U0 U2 U4 U6 ……（两行）
;						   IN int u_o_p,
;						   OUT void* v_o,         // 422 v 输出，存储方式 V0 V2 V4 V6 ……（两行）
;						   IN int v_o_p,
;						   IN void* u_i,          // 420 u 输入，存储方式 UU0 UU2 UU4 UU6 ……
;						   IN int u_i_p_2,
;						   IN void* v_i,          // 420 v 输入，存储方式 VV0 VV2 VV4 VV6 ……
;						   IN int v_i_p_2,
;						   IN int na16,           // 一行像素宽度的一半（因为一行内的 UV 与 Y 为 422 的关系）除以 16 的商与余数
;						   IN int nm16)
;--------------------------------------------------------------------------
uv20_to_uv22_line_2_L_x64:
	;rcx			- u_o
	;rdx			- u_o_p
	;r8				- v_o
	;r9				- v_o_p
	;[rsp+28h]		- u_i
	;[rsp+30h]		- u_i_p_2
	;[rsp+38h]		- v_i
	;[rsp+40h]		- v_i_p_2
	;[rsp+48h]		- na16
	;[rsp+50h]		- nm16

		push		rsi
		push		rdi
		push		rbx
		sub			rsp,20h
		movups		[rsp],xmm6
		movups		[rsp + 10h],xmm7
	
		mov         rdi, rcx;
		mov         rcx, [rsp+48h+38h];
		mov         rdx, r8;
		mov         rsi, [rsp+28h+38h];
		mov         rbx, [rsp+28h+38h];
		mov         rax, [rsp+38h+38h];
		sub         rbx, [rsp+30h+38h];
		pxor        xmm0, xmm0;
		movdqa      xmm7, OWORD PTR[gc_nRoundComp_2];
		test        rcx, rcx;
		jnz         loop_a16_uv20_to_uv22_line_2_L;
		jmp         loop_m8_uv20_to_uv22_line_2_L;
loop_a16_uv20_to_uv22_line_2_L:
		push        rax;
		push        rcx;
		mov         rcx, rax;
		sub         rcx, [rsp+50h+38h];		//v_i_p_2;
		movdqu      xmm3, [rsi];        // UU30 UU28 UU26 UU24  UU22 UU20 UU18 UU16  UU14 UU12 UU10 UU8  UU6 UU4 UU2 UU0
		movdqu      xmm1, [rbx];        // UU_P
		movdqu      xmm6, [rax];        // VV_C
		movdqu      xmm5, [rcx];        // VV_P
		movdqa      xmm2, xmm1;
		movdqa      xmm4, xmm3;
		;// 第二个结果：UU_C
		mov         rax, rdi;
		mov         rcx, rdx;
		add         rax, rdx;
		add         rcx, r9;
		movdqu      [rax], xmm3;
		movdqu      [rcx], xmm6;
		;// 第一个结果：UU_P * 0.25 + UU_C * 0.75
		punpcklbw   xmm1, xmm0;         // UU_P  low
		punpcklbw   xmm3, xmm0;         // UU_C  low
		punpckhbw   xmm2, xmm0;         // UU_P  high
		punpckhbw   xmm4, xmm0;         // UU_C  high
		paddw       xmm1, xmm3;     // UU_P + UU_C
		paddw       xmm2, xmm4;
		paddw       xmm3, xmm3;     // UU_C * 2
		paddw       xmm4, xmm4;
		paddw       xmm1, xmm7;
		paddw       xmm2, xmm7;
		paddw       xmm1, xmm3;     // UU_P + UU_C * 3
		paddw       xmm2, xmm4;
		movdqa      xmm3, xmm5;
		movdqa      xmm4, xmm6;
		punpcklbw   xmm3, xmm0;         // VV_P  low
		punpcklbw   xmm4, xmm0;         // VV_C  low
		punpckhbw   xmm5, xmm0;         // VV_P  high
		punpckhbw   xmm6, xmm0;         // VV_C  high
		paddw       xmm3, xmm4;     // VV_P + VV_C
		paddw       xmm5, xmm6;
		paddw       xmm4, xmm4;     // VV_C * 2
		paddw       xmm6, xmm6;
		paddw       xmm3, xmm7;
		paddw       xmm5, xmm7;
		paddw       xmm3, xmm4;     // VV_P + VV_C * 3
		paddw       xmm5, xmm6;
		psrlw       xmm1, 02h;
		psrlw       xmm2, 02h;
		psrlw       xmm3, 02h;
		psrlw       xmm5, 02h;
		packuswb    xmm1, xmm2;     // UU_1
		packuswb    xmm3, xmm5;     // VV_1
		movdqu      [rdi], xmm1;
		movdqu      [rdx], xmm3;
		pop         rcx;
		pop         rax;
		add         rax, 10h;
		add         rbx, 10h;
		add         rsi, 10h;
		add         rdx, 10h;
		add         rdi, 10h;
		sub         rcx, 01h;
		jnz         loop_a16_uv20_to_uv22_line_2_L;
loop_m8_uv20_to_uv22_line_2_L:
		mov         rcx, [rsp+50h+38h];
		test        rcx, 08h;
		jz          loop_m4_uv20_to_uv22_line_2_L;
		push        rax;
		push        rcx;
		mov         rcx, rax;
		sub         rcx, [rsp+50h+38h];		//v_i_p_2;
		movsd       xmm3, QWORD PTR[rsi];        // UU30 UU28 UU26 UU24  UU22 UU20 UU18 UU16  UU14 UU12 UU10 UU8  UU6 UU4 UU2 UU0
		movsd       xmm1, QWORD PTR[rbx];        // UU_P
		movsd       xmm6, QWORD PTR[rax];        // VV_C
		movsd       xmm5, QWORD PTR[rcx];        // VV_P
		;// 第二个结果：UU_C
		mov         rax, rdi;
		mov         rcx, rdx;
		add         rax, rdx;
		add         rcx, r9;
		movsd       QWORD PTR[rax], xmm3;
		movsd       QWORD PTR[rcx], xmm6;
		;// 第一个结果：UU_P * 0.25 + UU_C * 0.75
		punpcklbw   xmm1, xmm0;         // UU_P
		punpcklbw   xmm3, xmm0;         // UU_C
		punpcklbw   xmm5, xmm0;         // VV_P
		punpcklbw   xmm6, xmm0;         // VV_C
		paddw       xmm1, xmm3;     // UU_P + UU_C
		paddw       xmm5, xmm6;     // VV_P + VV_C
		paddw       xmm3, xmm3;     // UU_C * 2
		paddw       xmm6, xmm6;     // VV_C * 2
		paddw       xmm1, xmm7;
		paddw       xmm5, xmm7;
		paddw       xmm1, xmm3;     // UU_P + UU_C * 3
		paddw       xmm5, xmm6;     // VV_P + VV_C * 3
		psrlw       xmm1, 02h;
		psrlw       xmm5, 02h;
		packuswb    xmm1, xmm5;     // VV_1    UU_1
		movsd       QWORD PTR[rdi], xmm1;
		movhps      QWORD PTR[rdx], xmm1;
		pop         rcx;
		pop         rax;
		add         rax, 08h;
		add         rbx, 08h;
		add         rsi, 08h;
		add         rdx, 08h;
		add         rdi, 08h;
loop_m4_uv20_to_uv22_line_2_L:
		test        rcx, 04h;
		jz          loop_m2_uv20_to_uv22_line_2_L;
		push        rax;
		push        rcx;
		mov         rcx, rax;
		sub         rcx, [rsp+50h+38h];		//v_i_p_2;
		movss       xmm3, DWORD PTR[rsi];        // UU30 UU28 UU26 UU24  UU22 UU20 UU18 UU16  UU14 UU12 UU10 UU8  UU6 UU4 UU2 UU0
		movss       xmm1, DWORD PTR[rbx];        // UU_P
		movss       xmm6, DWORD PTR[rax];        // VV_C
		movss       xmm5, DWORD PTR[rcx];        // VV_P
		;// 第二个结果：UU_C
		mov         rax, rdi;
		mov         rcx, rdx;
		add         rax, rdx;
		add         rcx, r9;
		movss       DWORD PTR[rax], xmm3;
		movss       DWORD PTR[rcx], xmm6;
		;// 第一个结果：UU_P * 0.25 + UU_C * 0.75
		punpcklbw   xmm1, xmm0;         // UU_P
		punpcklbw   xmm3, xmm0;         // UU_C
		punpcklbw   xmm5, xmm0;         // VV_P
		punpcklbw   xmm6, xmm0;         // VV_C
		paddw       xmm1, xmm3;     // UU_P + UU_C
		paddw       xmm5, xmm6;     // VV_P + VV_C
		paddw       xmm3, xmm3;     // UU_C * 2
		paddw       xmm6, xmm6;     // VV_C * 2
		paddw       xmm1, xmm7;
		paddw       xmm5, xmm7;
		paddw       xmm1, xmm3;     // UU_P + UU_C * 3
		paddw       xmm5, xmm6;     // VV_P + VV_C * 3
		psrlw       xmm1, 02h;
		psrlw       xmm5, 02h;
		packuswb    xmm1, xmm5;     // VV_1    UU_1
		movhlps     xmm3, xmm1;
		movss       DWORD PTR[rdi], xmm1;
		movss       DWORD PTR[rdx], xmm3;
		pop         rcx;
		pop         rax;
		add         rax, 04h;
		add         rbx, 04h;
		add         rsi, 04h;
		add         rdx, 04h;
		add         rdi, 04h;
loop_m2_uv20_to_uv22_line_2_L:
		test        rcx, 02h;
		jz          loop_end_uv20_to_uv22_line_2_L;
		mov         rcx, rax;
		sub         rcx, [rsp+40h+38h];		//v_i_p_2;
		pinsrw      xmm1, WORD PTR[rbx], 00h;  // UU_P
		pinsrw      xmm3, WORD PTR[rsi], 00h;  // @@@@  @@@@  @@@@  @ @ UU2 UU0
		pinsrw      xmm1, WORD PTR[rcx], 02h;  // VV_P
		pinsrw      xmm3, WORD PTR[rax], 02h;  // VV_C
		;// 第二个结果：VV_C
		pextrw      rbx, xmm3, 00h;
		pextrw      rsi, xmm3, 02h;
		mov         rax, rdi;
		mov         rcx, rdx;
		add         rax, rdx;
		add         rcx, r9;
		mov         [rax], bx;
		mov         [rcx], si;
		;// 第一个结果：VV_P * 0.25 + VV_C * 0.75
		punpcklbw   xmm1, xmm0;         // VV_P  UU_P
		punpcklbw   xmm3, xmm0;         // VV_C  UU_C
		paddw       xmm1, xmm3;     // VV_P + VV_C    UU_P + UU_C
		paddw       xmm3, xmm3;     // VV_C * 2    UU_C * 2
		paddw       xmm1, xmm7;
		paddw       xmm1, xmm3;     // VV_P + VV_C * 3    UU_P + UU_C * 3
		psrlw       xmm1, 02h;
		packuswb    xmm1, xmm1;     // @ V1 @ U1  @ V1 @ U1
		pextrw      eax, xmm1, 00h;    // U1
		pextrw      ebx, xmm1, 02h;    // V1
		mov         [edi], ax;
		mov         [edx], bx;
loop_end_uv20_to_uv22_line_2_L:	
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 10h]
		add			rsp,20h
		pop			rbx
		pop			rdi
		pop			rsi
		ret
		
;--------------------------------------------------------------------------
;void uv20_to_uv22_line_2_x64(OUT void* u_o,         // 422 u 输出，存储方式 U0 U2 U4 U6 ……（两行）
;						 IN int u_o_p,
;						 OUT void* v_o,         // 422 v 输出，存储方式 V0 V2 V4 V6 ……（两行）
;						 IN int v_o_p,
;						 IN void* u_i,          // 420 u 输入，存储方式 UU0 UU2 UU4 UU6 ……
;						 IN int u_i_p_2,
;						 IN void* v_i,          // 420 v 输入，存储方式 VV0 VV2 VV4 VV6 ……
;						 IN int v_i_p_2,
;						 IN int na16,           // 一行像素宽度的一半（因为一行内的 UV 与 Y 为 422 的关系）除以 16 的商与余数
;						 IN int nm16)
;--------------------------------------------------------------------------
uv20_to_uv22_line_2_x64:
	;rcx			- u_o
	;rdx			- u_o_p
	;r8				- v_o
	;r9				- v_o_p
	;[rsp+28h]		- u_i
	;[rsp+30h]		- u_i_p_2
	;[rsp+38h]		- v_i
	;[rsp+40h]		- v_i_p_2
	;[rsp+48h]		- na16
	;[rsp+50h]		- nm16

		push		rsi
		push		rdi
		push		rbx
		sub			rsp,20h
		movups		[rsp],xmm6
		movups		[rsp + 10h],xmm7

		mov         rdi, rcx;
		mov         rcx, [rsp+48h+38h];
		mov         rdx, r8;
		mov         rsi, [rsp+28h+38h];
		mov         rbx, [rsp+28h+38h];
		mov         rax, [rsp+38h+38h];
		sub         rbx, [rsp+30h+38h];
		pxor        xmm0, xmm0;
		movdqa      xmm7, OWORD PTR[gc_nRoundComp_2];
		test        rcx, rcx;
		jnz         loop_a16_uv20_to_uv22_line_2;
		jmp         loop_m8_uv20_to_uv22_line_2;
loop_a16_uv20_to_uv22_line_2:
		push        rcx;
		push        rsi;
		push        rax;
		movdqu      xmm3, [rsi];        // UU30 UU28 UU26 UU24  UU22 UU20 UU18 UU16  UU14 UU12 UU10 UU8  UU6 UU4 UU2 UU0
		movdqu      xmm1, [rbx];        // UU_P
		mov         rcx, rdi;
		add         rsi, [rsp+48h+38h];		//u_i_p_2;       // 下一行
		add         rcx, rdx;
		movdqu      xmm5, [rsi];        // UU_N
		movdqa      xmm2, xmm1;         // UU_P
		movdqa      xmm4, xmm3;         // UU_C
		movdqa      xmm6, xmm5;         // UU_N
		punpcklbw   xmm1, xmm0;         // UU_P  low
		punpcklbw   xmm3, xmm0;         // UU_C  low
		punpcklbw   xmm5, xmm0;         // UU_N  low
		punpckhbw   xmm2, xmm0;         // UU_P  high
		punpckhbw   xmm4, xmm0;         // UU_C  high
		punpckhbw   xmm6, xmm0;         // UU_N  high
		;// 第一个结果：UU_P * 0.25 + UU_C * 0.75、第二个结果：UU_N * 0.25 + UU_C * 0.75
		paddw       xmm1, xmm3;     // UU_P + UU_C
		paddw       xmm5, xmm3;     // UU_N + UU_C
		paddw       xmm2, xmm4;
		paddw       xmm6, xmm4;
		paddw       xmm3, xmm3;     // UU_C * 2
		paddw       xmm4, xmm4;
		paddw       xmm1, xmm7;
		paddw       xmm5, xmm7;
		paddw       xmm2, xmm7;
		paddw       xmm6, xmm7;
		paddw       xmm1, xmm3;     // UU_P + UU_C * 3
		paddw       xmm5, xmm3;     // UU_N + UU_C * 3
		paddw       xmm2, xmm4;
		paddw       xmm6, xmm4;
		psrlw       xmm1, 02h;
		psrlw       xmm5, 02h;
		psrlw       xmm2, 02h;
		psrlw       xmm6, 02h;
		packuswb    xmm1, xmm2;     // 第一个结果
		packuswb    xmm5, xmm6;     // 第二个结果
		movdqu      [rdi], xmm1;
		movdqu      [rcx], xmm5;
		mov         rsi, rax;
		mov         rcx, rdx;
		sub         rsi, [rsp+58h+38h];	//v_i_p_2;   // 上一行
		movdqu      xmm3, [rax];    // VV_C
		movdqu      xmm1, [rsi];    // VV_P
		add         rax, [rsp+58h+38h];	v_i_p_2;   // 下一行
		add         rcx, r9;
		movdqu      xmm5, [rax];    // VV_N
		movdqa      xmm2, xmm1;         // VV_P
		movdqa      xmm4, xmm3;         // VV_C
		movdqa      xmm6, xmm5;         // VV_N
		punpcklbw   xmm1, xmm0;         // VV_P  low
		punpcklbw   xmm3, xmm0;         // VV_C  low
		punpcklbw   xmm5, xmm0;         // VV_N  low
		punpckhbw   xmm2, xmm0;         // VV_P  high
		punpckhbw   xmm4, xmm0;         // VV_C  high
		punpckhbw   xmm6, xmm0;         // VV_N  high
		;// 第一个结果：VV_P * 0.25 + VV_C * 0.75、第二个结果：VV_N * 0.25 + VV_C * 0.75
		paddw       xmm1, xmm3;     // VV_P + VV_C
		paddw       xmm5, xmm3;     // VV_N + VV_C
		paddw       xmm2, xmm4;
		paddw       xmm6, xmm4;
		paddw       xmm3, xmm3;     // VV_C * 2
		paddw       xmm4, xmm4;
		paddw       xmm1, xmm7;
		paddw       xmm5, xmm7;
		paddw       xmm2, xmm7;
		paddw       xmm6, xmm7;
		paddw       xmm1, xmm3;     // VV_P + VV_C * 3
		paddw       xmm5, xmm3;     // VV_N + VV_C * 3
		paddw       xmm2, xmm4;
		paddw       xmm6, xmm4;
		psrlw       xmm1, 02h;
		psrlw       xmm5, 02h;
		psrlw       xmm2, 02h;
		psrlw       xmm6, 02h;
		packuswb    xmm1, xmm2;     // 第一个结果
		packuswb    xmm5, xmm6;     // 第二个结果
		movdqu      [rdx], xmm1;
		movdqu      [rcx], xmm5;
		pop         rax;
		pop         rsi;
		pop         rcx;
		add         rax, 10h;
		add         rbx, 10h;
		add         rsi, 10h;
		add         rdx, 10h;
		add         rdi, 10h;
		sub         rcx, 01h;
		jnz         loop_a16_uv20_to_uv22_line_2;
loop_m8_uv20_to_uv22_line_2:
		mov         rcx, [rsp+50h+38h];		//nm16;
		test        rcx, 08h;
		jz          loop_m4_uv20_to_uv22_line_2;
		push        rcx;
		push        rax;
		push        rsi;
		mov         rcx, rax;
		sub         rcx, [rsp+58h+38h];		v_i_p_2;
		movsd       xmm1, QWORD PTR[rbx];        // UU_P
		movsd       xmm3, QWORD PTR[rsi];        // 0000  0000  UU14 UU12 UU10 UU8  UU6 UU4 UU2 UU0
		movsd       xmm2, QWORD PTR[rcx];        // VV_P
		movsd       xmm4, QWORD PTR[rax];        // VV_C
		add         rsi, [rsp+58h+38h];		//u_i_p_2;       // 下一行
		add         rax, [rsp+58h+38h];		//v_i_p_2;
		movsd       xmm5, QWORD PTR[rsi];        // UU_N
		movsd       xmm6, QWORD PTR[rax];        // VV_N
		punpcklbw   xmm1, xmm0;         // UU_P
		punpcklbw   xmm2, xmm0;         // VV_P
		punpcklbw   xmm3, xmm0;         // UU_C
		punpcklbw   xmm4, xmm0;         // VV_C
		punpcklbw   xmm5, xmm0;         // UU_N
		punpcklbw   xmm6, xmm0;         // VV_N
		;// 第一个结果：UU_P * 0.25 + UU_C * 0.75、第二个结果：UU_N * 0.25 + UU_C * 0.75
		;// 第一个结果：VV_P * 0.25 + VV_C * 0.75、第二个结果：VV_N * 0.25 + VV_C * 0.75
		paddw       xmm1, xmm3;     // UU_P + UU_C
		paddw       xmm2, xmm4;     // VV_P + VV_C
		paddw       xmm5, xmm3;     // UU_N + UU_C
		paddw       xmm6, xmm4;     // VV_N + VV_C
		paddw       xmm3, xmm3;     // UU_C * 2
		paddw       xmm4, xmm4;     // VV_C * 2
		paddw       xmm1, xmm7;
		paddw       xmm2, xmm7;
		paddw       xmm5, xmm7;
		paddw       xmm6, xmm7;
		paddw       xmm1, xmm3;     // UU_P + UU_C * 3
		paddw       xmm2, xmm4;     // VV_P + VV_C * 3
		paddw       xmm5, xmm3;     // UU_N + UU_C * 3
		paddw       xmm6, xmm4;     // VV_N + VV_C * 3
		psrlw       xmm1, 02h;
		psrlw       xmm2, 02h;
		psrlw       xmm5, 02h;
		psrlw       xmm6, 02h;
		packuswb    xmm1, xmm5;     // U2  U1
		packuswb    xmm2, xmm6;     // V2  V1
		mov         rax, rdx;
		mov         rcx, rdi;
		add         rax, rdx;
		add         rcx, r9;
		movsd       QWORD PTR[rdi], xmm1;
		movsd       QWORD PTR[rdx], xmm2;
		movhps      QWORD PTR[rax], xmm1;
		movhps      QWORD PTR[rcx], xmm2;
		pop         rsi;
		pop         rax;
		pop         rcx;
		add         rax, 08h;
		add         rbx, 08h;
		add         rsi, 08h;
		add         rdx, 08h;
		add         rdi, 08h;
loop_m4_uv20_to_uv22_line_2:
		test        rcx, 04h;
		jz          loop_m2_uv20_to_uv22_line_2;
		push        rcx;
		push        rax;
		push        rsi;
		mov         rcx, rax;
		sub         rcx, [rsp+58h+38h];		//v_i_p_2;
		movss       xmm1, DWORD PTR[rbx];        // UU_P
		movss       xmm3, DWORD PTR[rsi];        // 0000  0000  0000  UU6 UU4 UU2 UU0
		movss       xmm2, DWORD PTR[rcx];        // VV_P
		movss       xmm4, DWORD PTR[rax];        // VV_C
		add         rsi, [rsp+48h+38h];		//u_i_p_2;       // 下一行
		add         rax, [rsp+50h+38h];		//v_i_p_2;
		movss       xmm5, DWORD PTR[rsi];        // UU_N
		movss       xmm6, DWORD PTR[rax];        // VV_N
		punpcklbw   xmm1, xmm0;         // UU_P
		punpcklbw   xmm2, xmm0;         // VV_P
		punpcklbw   xmm3, xmm0;         // UU_C
		punpcklbw   xmm4, xmm0;         // VV_C
		punpcklbw   xmm5, xmm0;         // UU_N
		punpcklbw   xmm6, xmm0;         // VV_N
		;// 第一个结果：UU_P * 0.25 + UU_C * 0.75、第二个结果：UU_N * 0.25 + UU_C * 0.75
		;// 第一个结果：VV_P * 0.25 + VV_C * 0.75、第二个结果：VV_N * 0.25 + VV_C * 0.75
		paddw       xmm1, xmm3;     // UU_P + UU_C
		paddw       xmm2, xmm4;     // VV_P + VV_C
		paddw       xmm5, xmm3;     // UU_N + UU_C
		paddw       xmm6, xmm4;     // VV_N + VV_C
		paddw       xmm3, xmm3;     // UU_C * 2
		paddw       xmm4, xmm4;     // VV_C * 2
		paddw       xmm1, xmm7;
		paddw       xmm2, xmm7;
		paddw       xmm5, xmm7;
		paddw       xmm6, xmm7;
		paddw       xmm1, xmm3;     // UU_P + UU_C * 3
		paddw       xmm2, xmm4;     // VV_P + VV_C * 3
		paddw       xmm5, xmm3;     // UU_N + UU_C * 3
		paddw       xmm6, xmm4;     // VV_N + VV_C * 3
		psrlw       xmm1, 02h;
		psrlw       xmm2, 02h;
		psrlw       xmm5, 02h;
		psrlw       xmm6, 02h;
		packuswb    xmm1, xmm5;     // U2  U1
		packuswb    xmm2, xmm6;     // V2  V1
		movhlps     xmm3, xmm1;     // U2
		movhlps     xmm4, xmm2;     // V2
		mov         rax, rdx;
		mov         rcx, rdi;
		add         rax, rdx;
		add         rcx, r9;
		movss       DWORD PTR[rdi], xmm1;
		movss       DWORD PTR[rdx], xmm2;
		movss       DWORD PTR[rax], xmm3;
		movss       DWORD PTR[rcx], xmm4;
		pop         rsi;
		pop         rax;
		pop         rcx;
		add         rax, 04h;
		add         rbx, 04h;
		add         rsi, 04h;
		add         rdx, 04h;
		add         rdi, 04h;
loop_m2_uv20_to_uv22_line_2:
		test        rcx, 02h;
		jz          loop_end_uv20_to_uv22_line_2;
		mov         rcx, rax;
		sub         rcx, [rsp+40h+38h];		//v_i_p_2;
		pinsrw      xmm1, WORD PTR[rbx], 00h;  // UU_P
		pinsrw      xmm3, WORD PTR[rsi], 00h;  // @@@@  @@@@  @@@@  @ @ UU2 UU0
		pinsrw      xmm1, WORD PTR[rcx], 02h;  // VV_P
		pinsrw      xmm3, WORD PTR[rax], 02h;  // VV_C
		add         esi, [rsp+30h+38h];       // 下一行
		add         eax, [rsp+40h+38h];
		pinsrw      xmm5, WORD PTR[rsi], 00h;  // UU_N
		pinsrw      xmm5, WORD PTR[rax], 02h;  // VV_N
		punpcklbw   xmm1, xmm0;         // VV_P  UU_P
		punpcklbw   xmm3, xmm0;         // VV_C  UU_C
		punpcklbw   xmm5, xmm0;         // VV_N  UU_N
		;// 第一个结果：UU_P * 0.25 + UU_C * 0.75、第二个结果：UU_N * 0.25 + UU_C * 0.75
		;// 第一个结果：VV_P * 0.25 + VV_C * 0.75、第二个结果：VV_N * 0.25 + VV_C * 0.75
		paddw       xmm1, xmm3;     // VV_P + VV_C    UU_P + UU_C
		paddw       xmm5, xmm3;     // VV_N + VV_C    UU_N + UU_C
		paddw       xmm3, xmm3;     // VV_C * 2    UU_C * 2
		paddw       xmm1, xmm7;
		paddw       xmm5, xmm7;
		paddw       xmm1, xmm3;     // VV_P + VV_C * 3    UU_P + UU_C * 3
		paddw       xmm5, xmm3;     // VV_N + VV_C * 3    UU_N + UU_C * 3
		psrlw       xmm1, 02;
		psrlw       xmm5, 02;
		packuswb    xmm1, xmm5;     // @ V2 @ U2  @ V1 @ U1
		pextrw      rax, xmm1, 00h;    // U1
		pextrw      rbx, xmm1, 02h;    // V1
		pextrw      rcx, xmm1, 04h;    // U2
		pextrw      rsi, xmm1, 06h;    // V2
		mov         [rdi], ax;
		mov         [rdx], bx;
		add         rdi, rdx;
		add         rdx, r9;
		mov         [rdi], cx;
		mov         [rdx], si;
loop_end_uv20_to_uv22_line_2:
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 10h]
		add			rsp,20h
		pop			rbx
		pop			rdi
		pop			rsi
		ret
		
;--------------------------------------------------------------------------
;void uv20_to_uv22_line_2_F_x64(OUT void* u_o,         // 422 u 输出，存储方式 U0 U2 U4 U6 ……（两行）
;						   IN int u_o_p,
;						   OUT void* v_o,         // 422 v 输出，存储方式 V0 V2 V4 V6 ……（两行）
;						   IN int v_o_p,
;						   IN void* u_i,          // 420 u 输入，存储方式 UU0 UU2 UU4 UU6 ……
;						   IN int u_i_p_2,
;						   IN void* v_i,          // 420 v 输入，存储方式 VV0 VV2 VV4 VV6 ……
;						   IN int v_i_p_2,
;						   IN int na16,           // 一行像素宽度的一半（因为一行内的 UV 与 Y 为 422 的关系）除以 16 的商与余数
;						   IN int nm16)
;--------------------------------------------------------------------------
uv20_to_uv22_line_2_F_x64:
	;rcx			- u_o
	;rdx			- u_o_p
	;r8				- v_o
	;r9				- v_o_p
	;[rsp+28h]		- u_i
	;[rsp+30h]		- u_i_p_2
	;[rsp+38h]		- v_i
	;[rsp+40h]		- v_i_p_2
	;[rsp+48h]		- na16
	;[rsp+50h]		- nm16

		push		rsi
		push		rdi
		push		rbx
		sub			rsp,20h
		movups		[rsp],xmm6
		movups		[rsp + 10h],xmm7
	
		mov         rdi, rcx;
		mov         rcx, [rsp+48h+38h];
		mov         rdx, r8;
		mov         rsi, [rsp+28h+38h];
		mov         rax, [rsp+38h+38h];
		pxor        xmm0, xmm0;
		test        rcx, rcx;
		jnz         loop_a16_uv20_to_uv22_line_2_F;
		jmp         loop_m8_uv20_to_uv22_line_2_F;
loop_a16_uv20_to_uv22_line_2_F:
		movdqu      xmm1, [rsi];        // UU30 UU28 UU26 UU24  UU22 UU20 UU18 UU16  UU14 UU12 UU10 UU8  UU6 UU4 UU2 UU0
		movdqu      xmm2, [rax];        // VV30 VV28 VV26 VV24  VV22 VV20 VV18 VV16  VV14 VV12 VV10 VV8  VV6 VV4 VV2 VV0
		movdqa      xmm3, xmm1;
		movdqa      xmm4, xmm2;
		;// 结果为 UU_0
		movdqu      [rdi], xmm1;
		movdqu      [rdx], xmm2;
		add			rsi, [rsp+30h+38h];	//u_i_p_2;
		add			rax, [rsp+40h+38h];	//v_i_p_2;
		add         rdi, rdx;			//u_o_p;
		add         rdx, r9;			//v_o_p;
		;// 结果为 UU_0 * 0.75 + UU_2 * 0.25
		movdqu      xmm5, [rsi];        // UU_2
		movdqu      xmm6, [rax];        // VV_2
		movdqa      xmm7, xmm5;
		punpcklbw   xmm1, xmm0;         // UU14 UU12 UU10 UU8 UU6 UU4 UU2 UU0
		punpcklbw   xmm2, xmm0;         // VV14 VV12 VV10 VV8 VV6 VV4 VV2 VV0
		punpcklbw   xmm5, xmm0;         // UU_2  low
		punpckhbw   xmm7, xmm0;         // UU_2  high
		punpckhbw   xmm3, xmm0;         // UU30 UU28 UU26 UU24 UU22 UU20 UU18 UU16
		punpckhbw   xmm4, xmm0;         // VV30 VV28 VV26 VV24 VV22 VV20 VV18 VV16
		paddw       xmm5, xmm1;         // UU_2 + UU_0
		paddw       xmm7, xmm3;
		paddw       xmm1, xmm1;         // UU_0 * 2
		paddw       xmm3, xmm3;
		paddw       xmm1, xmm5;         // UU_0 * 3 + UU_2
		paddw       xmm3, xmm7;
		movdqa      xmm5, xmm6;
		movdqa      xmm7, OWORD PTR[gc_nRoundComp_2];
		punpcklbw   xmm5, xmm0;         // VV_2  low
		punpckhbw   xmm6, xmm0;         // VV_2  high
		paddw       xmm5, xmm2;         // VV_2 + VV_0
		paddw       xmm6, xmm4;
		paddw       xmm2, xmm2;         // VV_0 * 2
		paddw       xmm4, xmm4;
		paddw       xmm2, xmm5;         // VV_0 * 3 + VV_2
		paddw       xmm4, xmm6;
		paddw       xmm1, xmm7;         // UU_1  low
		paddw       xmm3, xmm7;         // UU_1  high
		paddw       xmm2, xmm7;         // VV_1  low
		paddw       xmm4, xmm7;         // VV_1  high
		psrlw       xmm1, 02h;
		psrlw       xmm3, 02h;
		psrlw       xmm2, 02h;
		psrlw       xmm4, 02h;
		packuswb    xmm1, xmm3;         // UU_1
		packuswb    xmm2, xmm4;         // VV_1
		movdqu      [rdi], xmm1;
		movdqu      [rdx], xmm2;
		sub         rsi, [rsp+30h+38h];		//u_i_p_2;
		sub         rax, [rsp+40h+38h];		//v_i_p_2;
		sub         rdi, rdx;				//u_o_p;
		sub         rdx, r9;				//v_o_p;
		add         rsi, 10h;
		add         rax, 10h;
		add         rdi, 10h;
		add         rdx, 10h;
		sub         rcx, 01h;
		jnz         loop_a16_uv20_to_uv22_line_2_F;
loop_m8_uv20_to_uv22_line_2_F:
		mov         rcx, [rsp+50h+38h];		//nm16;
		test        rcx, 08h;
		jz          loop_m4_uv20_to_uv22_line_2_F;
		movsd       xmm1, QWORD PTR[rsi];        // 0000  0000  UU14 UU12 UU10 UU8  UU6 UU4 UU2 UU0
		movsd       xmm2, QWORD PTR[rax];        // 0000  0000  VV14 VV12 VV10 VV8  VV6 VV4 VV2 VV0
		movdqa      xmm7, OWORD PTR[gc_nRoundComp_2];
		;// 结果为 UU_0
		movsd       QWORD PTR[rdi], xmm1;
		movsd       QWORD PTR[rdx], xmm2;
		add			rsi, [rsp+30h+38h];		//u_i_p_2;
		add			rax, [rsp+40h+38h];		//v_i_p_2;
		add         rdi, rdx;				//u_o_p;
		add         rdx, r9;				//v_o_p;
		;// 结果为 UU_0 * 0.75 + UU_2 * 0.25
		movsd       xmm5, QWORD PTR[rsi];        // UU_2
		movsd       xmm6, QWORD PTR[rax];        // VV_2
		punpcklbw   xmm1, xmm0;         // UU14 UU12 UU10 UU8 UU6 UU4 UU2 UU0
		punpcklbw   xmm2, xmm0;         // VV14 VV12 VV10 VV8 VV6 VV4 VV2 VV0
		punpcklbw   xmm5, xmm0;         // UU_2
		punpckhbw   xmm6, xmm0;         // VV_2
		paddw       xmm5, xmm1;         // UU_2 + UU_0
		paddw       xmm6, xmm2;         // VV_2 + VV_0
		paddw       xmm1, xmm1;         // UU_0 * 2
		paddw       xmm2, xmm2;         // VV_0 * 2
		paddw       xmm1, xmm5;         // UU_0 * 3 + UU_2
		paddw       xmm2, xmm6;         // VV_0 * 3 + VV_2
		psrlw       xmm1, 02h;
		psrlw       xmm2, 02h;
		packuswb    xmm1, xmm2;         // VV_1  UU_1
		movsd       QWORD PTR[rdi], xmm1;
		movhps      QWORD PTR[rdx], xmm1;
		sub         rsi, [rsp+30h+38h];		//u_i_p_2;
		sub         rax, [rsp+40h+38h];		//v_i_p_2;
		sub         rdi, rdx;				//u_o_p;
		sub         rdx, r9;				//v_o_p;
		add         rsi, 08h;
		add         rax, 08h;
		add         rdi, 08h;
		add         rdx, 08h;
loop_m4_uv20_to_uv22_line_2_F:
		test        rcx, 04h;
		jz          loop_m2_uv20_to_uv22_line_2_F;
		movss       xmm1, DWORD PTR[rsi];        // 0000  0000  0000  UU6 UU4 UU2 UU0
		movss       xmm2, DWORD PTR[rax];        // 0000  0000  0000  VV6 VV4 VV2 VV0
		movdqa      xmm7, OWORD PTR[gc_nRoundComp_2];
		;// 结果为 UU_0
		movss       DWORD PTR[rdi], xmm1;
		movss       DWORD PTR[rdx], xmm2;
		add			rsi, [rsp+30h+38h]	;		//u_i_p_2;
		add			rax, [rsp+40h+38h];		//v_i_p_2;
		add         rdi, rdx;				//u_o_p;
		add         rdx, r9;				//v_o_p;
		;// 结果为 UU_0 * 0.75 + UU_2 * 0.25
		movss       xmm5, DWORD PTR[rsi];        // UU_2
		movss       xmm6, DWORD PTR[rax];        // VV_2
		punpcklbw   xmm1, xmm0;         // 0 0 0 0 UU6 UU4 UU2 UU0
		punpcklbw   xmm2, xmm0;         // 0 0 0 0 VV6 VV4 VV2 VV0
		punpcklbw   xmm5, xmm0;         // UU_2
		punpckhbw   xmm6, xmm0;         // VV_2
		paddw       xmm5, xmm1;         // UU_2 + UU_0
		paddw       xmm6, xmm2;         // VV_2 + VV_0
		paddw       xmm1, xmm1;         // UU_0 * 2
		paddw       xmm2, xmm2;         // VV_0 * 2
		paddw       xmm1, xmm5;         // UU_0 * 3 + UU_2
		paddw       xmm2, xmm6;         // VV_0 * 3 + VV_2
		psrlw       xmm1, 02h;
		psrlw       xmm2, 02h;
		packuswb    xmm1, xmm2;         // VV_1  UU_1
		movhlps     xmm2, xmm1;         // VV_1
		movss       DWORD PTR[rdi], xmm1;
		movss       DWORD PTR[rdx], xmm2;
		sub         rsi, [rsp+30h+38h];		//u_i_p_2;
		sub         rax, [rsp+40h+38h];		//v_i_p_2;
		sub         rdi, rdx;				//u_o_p;
		sub         rdx, r9;				//v_o_p;
		add         rsi, 04h;
		add         rax, 04h;
		add         rdi, 04h;
		add         rdx, 04h;
loop_m2_uv20_to_uv22_line_2_F:
		test        rcx, 02h;
		jz          loop_end_uv20_to_uv22_line_2_F;
		xor         rbx, rbx;
		xor         rcx, rcx;
		mov         bx, [rsi];      // UU_0
		mov         cx, [rax];      // VV_0
		;// 结果为 UU_0
		mov         [rdi], bx;
		mov         [rdx], cx;
		add			rsi, [rsp+30h+38h];	//u_i_p_2;
		add			rax, [rsp+40h+38h];	//v_i_p_2;
		add         rdi, rdx;			//u_o_p;
		add         rdx, r9;			//v_o_p;
		push        rdi;
		push        rdx;
		xor         rdi, rdi;
		xor         rdx, rdx;
		mov         di, [rsi];      // UU_2
		mov         dx, [rax];      // VV_2
		shl         rcx, 10h;      // VV_0 0
		shl         rdx, 10h;      // VV_2 0
		or          rbx, rcx;       // VV_0 UU_0
		or          rdx, rdi;       // VV_2 UU_2
		xor         rax, rax;
		xor         rcx, rcx;
		mov         rsi, rbx;
		mov         rdi, rdx;
		shr         rbx, 08h;
		shr         rdx, 08h;
		and         rsi, 00ff00ffh;    // VV0_0  UU0_0
		and         rdi, 00ff00ffh;    // VV0_2  UU0_2
		and         rbx, 00ff00ffh;    // VV2_0  UU2_0
		and         rdx, 00ff00ffh;    // VV2_2  UU2_2
		add         rdi, rsi;
		add         rdx, rbx;
		add         rsi, rsi;
		add         rbx, rbx;
		add         rdi, 00020002h;
		add         rdx, 00020002h;
		add         rsi, rdi;
		add         rbx, rdx;
		shr         rsi, 02h;
		shr         rbx, 02h;
		and         rsi, 00ff00ffh;    // VV0_1  UU0_1
		and         rbx, 00ff00ffh;    // VV2_1  UU2_1
		shl         rbx, 08h;
		or          rbx, rsi;           // VV2 VV0 UU2 UU0
		mov         rax, rbx;
		shr         rbx, 10h;          // 0 0 VV2 VV0
		pop         rdx;
		pop         rdi;
		mov			[rdx], bx;
		mov         [rdi], ax;
loop_end_uv20_to_uv22_line_2_F:
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 10h]
		add			rsp,20h
		pop			rbx
		pop			rdi
		pop			rsi
		ret

;--------------------------------------------------------------------------		
;void yuv422_to_yuyv_line_x64(OUT BYTE* yuyv,        // yuyv 输出，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
;						 IN BYTE* y,            // y 输入，存放方式 Y0 Y1 Y2 Y3 ……
;						 IN BYTE* u,            // u 输入，存放方式 U0 U2 U4 U6 ……
;						 IN BYTE* v,            // v 输入，存放方式 V0 V2 V4 V6 ……
;						 IN int na16,           // 一行像素宽度除以 16 的商与余数（注意目标为 16 位的缓存）
;						 IN int nm16)
;--------------------------------------------------------------------------						 
yuv422_to_yuyv_line_x64:
	;rcx		- yuyv
	;rdx		- y
	;r8			- u
	;r9			- v
	;[rsp+28h]	- na16
	;[rsp+30h]	- nm16

		push		rsi
		push		rdi
		push		rbx
		sub			rsp,20h
		movups		[rsp],xmm6
		movups		[rsp + 10h],xmm7

		mov         rdi, rcx;
		mov         rdx, rcx;	
		mov         rcx, [rsp+28h+38h];
		mov         rsi, rdx;
		mov         rax, r8;
		mov         rbx, r9;
		add         rdx, 10h;
		test        rcx, rcx;
		jnz         loop_a16;
		jmp         loop_m8;
loop_a16:
		movdqu      xmm0, [rsi];    // Y15 Y14 Y13 Y12  Y11 Y10 Y9 Y8  Y7 Y6 Y5 Y4  Y3 Y2 Y1 Y0
		movsd       xmm1, QWORD PTR[rax];    // 0000 0000  U14 U12 U10 U8  U6 U4 U2 U0
		movsd       xmm2, QWORD PTR[rbx];    // 0000 0000  V14 V12 V10 V8  V6 V4 V2 V0
		punpcklbw   xmm1, xmm2;     // V14 U14 V12 U12  V10 U10 V8 U8  V6 U6 V4 U4  V2 U2 V0 U0
		movdqa      xmm3, xmm0;
		punpcklbw   xmm0, xmm1;     // V6 Y7 U6 Y6  V4 Y5 U4 Y4  V2 Y3 U2 Y2  V0 Y1 U0 Y0
		punpckhbw   xmm3, xmm1;     // V14 Y15 U14 Y14  V12 Y13 U12 Y12  V10 Y11 U10 Y10  V8 Y9 U8 Y8
		movdqu      [rdi], xmm0;
		movdqu      [rdx], xmm3;
		add         rsi, 10h;
		add         rax, 08h;
		add         rbx, 08h;
		add         rdi, 10h;
		add         rdx, 10h;
		sub         rcx, 01h;
		jnz         loop_a16;
loop_m8:
		mov         rcx, [rsp+30h+38h];
		test        rcx, 08h;
		jz          loop_m4;
		movsd       xmm0, QWORD PTR[rsi];    // 0000 0000  Y7 Y6 Y5 Y4  Y3 Y2 Y1 Y0
		movss       xmm1, DWORD PTR[rax];    // 0000 0000 0000  U6 U4 U2 U0
		movss       xmm2, DWORD PTR[rbx];    // 0000 0000 0000  V6 V4 V2 V0
		punpcklbw   xmm1, xmm2;     // 0000 0000  V6 U6 V4 U4  V2 U2 V0 U0
		punpcklbw   xmm0, xmm1;     // V6 Y7 U6 Y6  V4 Y5 U4 Y4  V2 Y3 U2 Y2  V0 Y1 U0 Y0
		movdqu      [rdi], xmm0;
		add         rsi, 08h;
		add         rax, 04h;
		add         rbx, 04h;
		add         rdi, 10h;
loop_m4:
		test        rcx, 04h;
		jz          loop_m2;
		movss       xmm0, DWORD PTR[rsi];    // 0000 0000 0000  Y3 Y2 Y1 Y0
		pxor        xmm1, xmm1;
		pxor        xmm2, xmm2;
		pinsrw      xmm1, WORD PTR[rax], 00h;      // U2 U0
		pinsrw      xmm2, WORD PTR[rbx], 00h;      // V2 V0
		punpcklbw   xmm1, xmm2;     // 0000 0000 0000  V2 U2 V0 U0
		punpcklbw   xmm0, xmm1;     // 0000 0000  V2 Y3 U2 Y2  V0 Y1 U0 Y0
		movsd       QWORD PTR[rdi], xmm0;
		add         rsi, 04h;
		add         rax, 02h;
		add         rbx, 02h;
		add         rdi, 08h;
loop_m2:
		test        rcx, 02h;
		jz          loop_end1;
		xor         rdx, rdx;
		xor         rcx, rcx;
		mov         dh, [rbx];      // V0
		mov         cx, [rsi];
		shl         rdx, 10;      // V0 0 0 0
		mov         rbx, rcx;       // 0 0 Y1 Y0
		mov         dh, [rax];      // V0 0 U0 0
		and         rcx, 0ff00h;    // 0 0 Y1 0
		and         rbx, 00ffh;    // 0 0 0 Y0
		shl         rcx, 08h;      // 0 Y1 0 0
		or          rdx, rbx;
		or          rdx, rcx;
		mov         [rdi], rdx;
loop_end1:	
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 10h]
		add			rsp,20h
		pop			rbx
		pop			rdi
		pop			rsi
		ret
		
;--------------------------------------------------------------------------		
;void _Fill_inner_pixel(DWORD* t_pSrc,int src_buff_width,DWORD* cur_dst,int num_pix,        
;					 	float cur_v_u[4],float dxv_dxu[4],short src_offest[8]        ,
;						int	alpha_mask[4],int	alpha_one_mask[4],int alpha_ps_mask[4],int one_ps_mask2[4],int one_ps_mask[4]);
;--------------------------------------------------------------------------						 
_Fill_inner_pixel:
	;rcx		- t_pSrc
	;rdx		- src_buff_width
	;r8			- cur_dst
	;r9			- num_pix
	;[rsp+28h]	- cur_v_u
	;[rsp+30h]	- dxv_dxu
	;[rsp+38h]	- src_offset
	;[rsp+40h]	- alpha_mask
	;[rsp+48h]  - alpha_one_mask 
	;[rsp+50h]  - alpha_ps_mask 
	;[rsp+58h]  - one_ps_mask2 
	;[rsp+60h]  - one_ps_mask 
			sub			rsp,30h
			movups		[rsp],xmm6
			movups		[rsp + 16],xmm7
			movups		[rsp + 32],xmm8

			push 	rsi;
			push 	rdi;
			mov		rsi, rcx; rcx - >t_pSrc
			mov		rdi, r8;r8 -> cur_dst
			mov		rcx,r9; r9 -> num_pix
			test	ecx, ecx
			jle		tag_row_loop_end
		;; 计算公式		S00 * x * y				S10 * (1 - x) * y
		;;				S01 * x * (1 - y)		S11 * (1 - x) * (1 - y)
		;; D = S11 + (S00 - S10 - S01 + S11) * x * y + (S01 - S11) * x + (S10 - S11) * y
tag_row_loop:	
			mov		rax,[rsp+28h + 40h];
			movaps	xmm1, [rax];		//v2 u2 v1 u1
			movaps	xmm0, xmm1;			//v2 u2 v1 u1

			cvttps2dq	xmm2, xmm1;		//p5v p5u p1v p1u
			cvtdq2ps	xmm3, xmm2;		//p5v p5u p1v p1u
			mov		r9,[rsp + 30h + 40h];//cur_v_u
			addps	xmm0, [r9];		//next_v2 next_u2 next_v1 next_u1
			movaps	[rax], xmm0;		//next_v2 next_u2 next_v1 next_u1
			
			mov		eax, 3f800000h;float 1
			movd   xmm7,eax;
			pshufd	xmm7,xmm7,0; 
			subps	xmm3, xmm1;
			addps	xmm3, xmm7;			//p5yw p5xw p1yw p1xw //1-v2.dec 1-u2.dec 1-v1.dec 1-u1.dec

			subps	xmm7, xmm3;			// [p7yw, p6xw, p3yw, p2xw] //v2.dec u2.dec v1.dec u1.dec

			movaps	xmm5, xmm3;			// [p5yw, p5xw, p1yw, p1xw]//1-v2.dec 1-u2.dec 1-v1.dec 1-u1.dec
			pshufd	xmm4, xmm3,	177;	//177 = 1011 0001(2)   把高双字和低双字中的字对换 ab cd -> ba dc: [p5xw, p5yw, p1xw, p1yw]
			;;1-u2.dec 1-v2.dec  1-u1.dec 1-v1.dec

			mulps	xmm4, xmm3;			// [p5xw*p5yw, p5xw*p5yw, p1xw*p1yw, p1xw*p1yw]
			;;[(1-u2.dec) * (1-v2.dec),
			;;(1-u2.dec) * (1-v2.dec),
			;;(1-u1.dec) * (1-v1.dec),
			;;(1-u1.dec) * (1-v1.dec)]

			subps	xmm5, xmm4;			//[p6xw*p6yw, p7xw*p7yw, p2xw*p2yw, p3xw*p3yw]
			;;[(1-v2.dec) - (1-u2.dec) * (1-v2.dec),  ==>(1-v2.dec) * u2.dec
			;;(1-u2.dec) - (1-u2.dec) * (1-v2.dec), ==> (1-u2.dec) * v2.dec
			;;(1-v1.dec) - (1-u1.dec) * (1-v1.dec),==>(1-v1.dec) * u1.dec
			;;(1-u1.dec) - (1-u1.dec) * (1-v1.dec)]==> (1-u1.dec) * v1.dec

			psrldq	xmm7, 4;			//[0, p7yw, p6xw, p3yw]
			addps	xmm7, xmm4;			//[p5xw*p5yw, p7yw+p5xw*p5yw, p6xw+p1xw*p1yw, p3yw+p1xw*p1yw]
			subps	xmm7, xmm3;			//[-p6xw*p6yw, p8xw*p8yw, p6xw-p2xw*p1yw, p4xw*p4yw]

			packssdw	xmm2, xmm2;		//[p5v p5u, p1v p1u, p5v p5u, p1v p1u] 【注：此处假设源图像的大小不超出两个字节65535】
			mov 	rax,[rsp+38h+40h];
			pmaddwd xmm2, [rax];	//[0, 0, p5idx, p1idx]
			xor		rax,rax;
			movd	eax, xmm2;			// p1idx

			psrldq		xmm2, 4;		//[0, 0, 0, p5idx]
			xor		r10,r10;
			movd	r10d, xmm2;			//p5idx

			movq	xmm0, qword ptr [rsi+4*rax]; //读取当前行的1、2两个像素颜色 [0, 0, p2RGBA, p1RGBA]
			add		rax, rdx;
			movhpd  xmm0, qword ptr [rsi+4*rax];
			
;///////////////////////////第一轮
			movdqa  xmm1,xmm0;
			pand	xmm1,oword ptr[gc_dwMaskAlpha];
			xorps	xmm2,xmm2;
			pcmpeqd xmm1,xmm2;
			movdqa  xmm2,xmm1;
			pandn   xmm1,xmm0;
			movdqa	xmm0,xmm1;
			movdqa	xmm8,xmm0;
;///////////////////////////第二轮
			pshufd	xmm1,xmm8, 177;
			pand	xmm1,xmm2;
			movdqa  xmm6,xmm1;
			pand	xmm6,oword ptr[gc_dwMaskRGB];
			paddb	xmm0,xmm6;
			pand	xmm1,oword ptr[gc_dwMaskAlpha];
			xorps	xmm6,xmm6;
			pcmpeqd xmm1,xmm6;
			pand	xmm2,xmm1;
;///////////////////////////第三轮
			pshufd	xmm1,xmm8,4bh;
			pand	xmm1,xmm2;
			movdqa  xmm6,xmm1;
			pand	xmm6,oword ptr[gc_dwMaskRGB];
			paddb	xmm0,xmm6;
			pand	xmm1,oword ptr[gc_dwMaskAlpha];

			xorps	xmm6,xmm6;
			pcmpeqd xmm1,xmm6;
			pand	xmm2,xmm1;
;///////////////////////////第四轮
			pshufd	xmm1,xmm8, 1eh;
			pand	xmm1,xmm2;
			movdqa  xmm6,xmm1;
			pand	xmm6,oword ptr[gc_dwMaskRGB];
			paddb	xmm0,xmm6;
;///////////////////////////////////////////////////


			movaps  xmm8,xmm0;
			xorps	xmm6,xmm6;
			punpcklbw xmm0, xmm6;		//[p2R p2G, p2B p2A, p1R p1G, p1B p1A]
			;//------------------------------------------------------------------------
			;//预乘alpha
			movdqa	xmm6, xmm0;			//[p2A p2B, p2G p2R, p1A p1B, p1G p1R]
			mov		r11,[rsp+40h + 40h];
			pand	xmm6, [r11];	//[p2A 0, 0 0, p1A 0, 0 0]
			mov		r11,[rsp+48h + 40h];
			por		xmm6, [r11];//[p2A 0, 0 1, p1A 0, 0 1]
			pshufhw	xmm6, xmm6, 3fh;	//[1 p2A, p2A p2A, p1A 0, 0 1]
			pshuflw	xmm6, xmm6, 3fh;	//[1 p2A, p2A p2A, 1 p1A, p1A p1A]
			pmullw	xmm0, xmm6;			//[p2Apshufd p2B*p2A, p2G*p2A p2R*p2A, p1A p1B*p1A, p1G*p1A p1R*p1A]
			;//------------------------------------------------------------------------
			movdqa	xmm1, xmm0;			//[p2R p2G, p2B p2A, p1R p1G, p1B p1A]

			xorps xmm2,xmm2;
			punpcklwd xmm0, xmm2;		//p1r   p1g   p1b   p1a
			pshufd	xmm6, xmm4, 0 ;		//p1yw*p1xw   p1yw*p1xw   p1yw*p1xw   p1yw*p1xw
			;;[(1-u1.dec) * (1-v1.dec),
			;;(1-u1.dec) * (1-v1.dec),
			;;(1-u1.dec) * (1-v1.dec),
			;;(1-u1.dec) * (1-v1.dec)]

			cvtdq2ps  xmm0, xmm0;		//p1r   p1g   p1b   p1a
			mulps	xmm0, xmm6;			//p1r*p1xw*p1yw   p1g*p1xw*p1yw   p1b*p1xw*p1yw   p1a*p1xw*p1yw
			;;[(1-u1.dec) * (1-v1.dec) * p1r,
			;;(1-u1.dec) * (1-v1.dec) * p1g,
			;;(1-u1.dec) * (1-v1.dec) * p1b,
			;;(1-u1.dec) * (1-v1.dec) * p1a]

			punpckhwd xmm1, xmm2;		//p2r   p2g   p2b   p2a
			pshufd	xmm6, xmm5, 85;		//85 = 1010101  p2yw*p2xw   p2yw*p2xw   p2yw*p2xw   p2yw*p2xw
			cvtdq2ps  xmm1, xmm1;		//p2r   p2g   p2b   p2a
			mulps	xmm1, xmm6;			//p2r*p2yw*p2xw   p2g*p2yw*p2xw   p2b*p2yw*p2xw   p2a*p2yw*p2xw

			;;[u1.dec * (1-v1.dec) * p2r,
			;;u1.dec * (1-v1.dec) * p2g,
			;;u1.dec * (1-v1.dec) * p2b,
			;;u1.dec * (1-v1.dec) * p2a]
			addps	xmm0, xmm1;			//p1r*p1xw*p1yw+p2r*p2yw*p2xw   p1g*p1xw*p1yw+p2g*p2yw*p2xw   p1b*p1xw*p1yw+p2b*p2yw*p2xw   p1a*p1xw*p1yw+p2a*p2yw*p2xw

			;;[u1.dec * (1-v1.dec) * p2r + u1.dec * (1-v1.dec) * p2r, 
			;;u1.dec * (1-v1.dec) * p2g + u1.dec * (1-v1.dec) * p2g,
			;;u1.dec * (1-v1.dec) * p2b + u1.dec * (1-v1.dec) * p2b,
			;;u1.dec * (1-v1.dec) * p2a + u1.dec * (1-v1.dec) * p2a]

			movaps  xmm2,xmm8;
			movhlps xmm2,xmm2;
			xorps 	xmm6,xmm6;
			punpcklbw xmm2, xmm6;		
			;//------------------------------------------------------------------------
			;//预乘alpha
			movdqa	xmm6, xmm2;			//[p2A p2B, p2G p2R, p1A p1B, p1G p1R]
			mov		r11,[rsp+40h + 40h];
			pand	xmm6, [r11];	//[p2A 0, 0 0, p1A 0, 0 0]
			mov		r11,[rsp+48h + 40h];
			por		xmm6, [r11];//[p2A 0, 0 1, p1A 0, 0 1]
			pshufhw	xmm6, xmm6, 3fh;	//[1 p2A, p2A p2A, p1A 0, 0 1]
			pshuflw	xmm6, xmm6, 3fh;	//[1 p2A, p2A p2A, 1 p1A, p1A p1A]
			pmullw	xmm2, xmm6;			//[p2A p2B*p2A, p2G*p2A p2R*p2A, p1A p1B*p1A, p1G*p1A p1R*p1A]
			;//------------------------------------------------------------------------
			movdqa	xmm1, xmm2		

			xorps	xmm6,xmm6;
			punpcklwd xmm2, xmm6		
			pshufd	xmm6, xmm5, 0		
			cvtdq2ps  xmm2, xmm2		
			mulps	xmm2, xmm6		
			addps	xmm0, xmm2

			xorps	xmm6,xmm6;
			punpckhwd xmm1, xmm6;		
			pshufd	xmm6, xmm7, 0		
			cvtdq2ps  xmm1, xmm1		
			mulps	xmm1, xmm6		
			addps	xmm0, xmm1;			//

			;//------------------------------------------------------------------------
			;//从rgb中剔除alpha
			movdqa	xmm1, xmm0;			//[dp1A, dp1B, dp1G, dp1R]
			mov		r11,[rsp+50h + 40h];
			pand	xmm1, [r11];//[dp1A, 0, 0, 0]
			movdqa	xmm2, xmm1;			//[dp1A, 0, 0, 0]
			xorps 	xmm3,xmm3;
			pcmpeqd	xmm2, xmm3;			//[0../f.., f.., f.., f..]
			mov		r11,[rsp+58h + 40h];
			pand	xmm2, [r11];	//[0/1.f, 0, 0, 0]
			addps	xmm1, xmm2;			//[dp1A/1.f, 0, 0, 0]
			mov		r11,[rsp+60h + 40h];
			addps	xmm1, [r11];	//[dp1A/1.f, 0, 0, 1.f]
			pshufd	xmm1, xmm1, 3fh;	//[1.f, dp1A, dp1A, dp1A]
			divps	xmm0, xmm1;			//[dp1A, dp1B/dp1A, dp1G/dp1A, dp1R/dp1A]
			;//------------------------------------------------------------------------

			cvtps2dq	xmm0,xmm0	

			sub	ecx, 1
			jne	second_pix

			packssdw	xmm0, xmm0	
			packuswb	xmm0, xmm0	
			movd		DWORD PTR[rdi],	xmm0	

			jmp tag_row_loop_end

second_pix:	

			movq	xmm3, qword ptr [rsi+4h*r10]; //读取当前行的1、2两个像素颜色 [0, 0, p2RGBA, p1RGBA]
			add		r10, rdx
			movhpd  xmm3, qword ptr [rsi+4*r10];

			;///////////////////////////第一轮
			movdqa  xmm1,xmm3;
			pand	xmm1,oword ptr[gc_dwMaskAlpha];
			xorps	xmm2,xmm2;
			pcmpeqd xmm1,xmm2;
			movdqa  xmm2,xmm1;
			pandn   xmm1,xmm3;
			movdqa	xmm3,xmm1;
			movdqa	xmm8,xmm3;
			;///////////////////////////第二轮
			pshufd	xmm1,xmm8, 177;
			pand	xmm1,xmm2;
			movdqa  xmm6,xmm1;
			pand	xmm6,oword ptr[gc_dwMaskRGB];
			paddb	xmm3,xmm6;
			pand	xmm1,oword ptr[gc_dwMaskAlpha];
			xorps	xmm6,xmm6;
			pcmpeqd xmm1,xmm6;
			pand	xmm2,xmm1;
			;///////////////////////////第三轮
			pshufd	xmm1,xmm8,4bh;
			pand	xmm1,xmm2;
			movdqa  xmm6,xmm1;
			pand	xmm6,oword ptr[gc_dwMaskRGB];
			paddb	xmm3,xmm6;
			pand	xmm1,oword ptr[gc_dwMaskAlpha];
			xorps	xmm6,xmm6;
			pcmpeqd xmm1,xmm6;zero
			pand	xmm2,xmm1;
			;///////////////////////////第四轮
			pshufd	xmm1,xmm8, 1eh;
			pand	xmm1,xmm2;
			movdqa  xmm6,xmm1;
			pand	xmm6,oword ptr[gc_dwMaskRGB];
			paddb	xmm3,xmm6;
			;///////////////////////////////////////////////////


			movaps  xmm8,xmm3;
;;			movq	xmm3, qword ptr [esi+4*r10d];
			xorps 	xmm6,xmm6;
			punpcklbw xmm3, xmm6		
			;//------------------------------------------------------------------------
			;//预乘alpha
			movdqa	xmm6, xmm3;			//[p2A p2B, p2G p2R, p1A p1B, p1G p1R]
			mov		r11, [rsp+40h + 40h];
			pand	xmm6, OWORD PTR[r11];	//[p2A 0, 0 0, p1A 0, 0 0]
			mov		r11,[rsp+48h + 40h];
			por		xmm6, [r11];//[p2A 0, 0 1, p1A 0, 0 1]
			pshufhw	xmm6, xmm6, 3fh;	//[1 p2A, p2A p2A, p1A 0, 0 1]
			pshuflw	xmm6, xmm6, 3fh;	//[1 p2A, p2A p2A, 1 p1A, p1A p1A]
			pmullw	xmm3, xmm6;			//[p2A p2B*p2A, p2G*p2A p2R*p2A, p1A p1B*p1A, p1G*p1A p1R*p1A]
			;//------------------------------------------------------------------------

			movdqa	xmm1, xmm3		
			xorps	xmm6,xmm6;
			punpcklwd xmm3, xmm6		
			pshufd	xmm6, xmm4, 170		
			cvtdq2ps  xmm3, xmm3		
			mulps	xmm3, xmm6		

			xorps	xmm6,xmm6;
			punpckhwd xmm1, xmm6;		
			pshufd	xmm6, xmm5, 255		
			cvtdq2ps  xmm1, xmm1		
			mulps	xmm1, xmm6		
			addps	xmm3, xmm1

			movdqa  xmm2,xmm8;
			movhlps xmm2,xmm2;
			;;add		edx, src_buff_width
			;;movq	xmm2, qword ptr [esi+4*edx];	
			xorps	xmm1,xmm1;
			punpcklbw xmm2, xmm1		
			;//------------------------------------------------------------------------
			;//预乘alpha
			movdqa	xmm1, xmm2;			//[p2A p2B, p2G p2R, p1A p1B, p1G p1R]
			mov		r11, [rsp+40h + 40h];
			pand	xmm1, OWORD PTR[r11];	//[p2A 0, 0 0, p1A 0, 0 0]
			mov		r11,[rsp+48h + 40h];
			por		xmm1, [r11];//[p2A 0, 0 1, p1A 0, 0 1]
			pshufhw	xmm1, xmm1, 3fh;	//[1 p2A, p2A p2A, p1A 0, 0 1]
			pshuflw	xmm1, xmm1, 3fh;	//[1 p2A, p2A p2A, 1 p1A, p1A p1A]
			pmullw	xmm2, xmm1;			//[p2A p2B*p2A, p2G*p2A p2R*p2A, p1A p1B*p1A, p1G*p1A p1R*p1A]
			;//------------------------------------------------------------------------
			movdqa	xmm1, xmm2		

			xorps 	xmm6,xmm6;
			punpcklwd xmm2, xmm6		
			pshufd	xmm6, xmm5, 170		
			cvtdq2ps  xmm2, xmm2		
			mulps	xmm2, xmm6		
			addps	xmm3, xmm2

			xorps 	xmm6,xmm6;
			punpckhwd xmm1, xmm6;		
			pshufd	xmm6, xmm7, 170		
			cvtdq2ps  xmm1, xmm1		
			mulps	xmm1, xmm6		

			addps	xmm3, xmm1		

			;//------------------------------------------------------------------------
			;//从rgb中剔除alpha
			movdqa	xmm1, xmm3;			//[dp1A, dp1B, dp1G, dp1R]
			mov		r11,[rsp+50h + 40h];
			pand	xmm1, OWORD PTR[r11];//[dp1A, 0, 0, 0]
			movdqa	xmm2, xmm1;			//[dp1A, 0, 0, 0]
			xorps	xmm8,xmm8;
			pcmpeqd	xmm2, xmm8;			//[0../f.., f.., f.., f..]
			mov		r11,[rsp+58h + 40h];
			pand	xmm2, OWORD PTR[r11];	//[0/1.f, 0, 0, 0]
			addps	xmm1, xmm2;			//[dp1A/1.f, 0, 0, 0]
			mov		r11,[rsp+60h + 40h];
			addps	xmm1, [r11];	//[dp1A/1.f, 0, 0, 1.f]
			pshufd	xmm1, xmm1, 3fh;	//[1.f, dp1A, dp1A, dp1A]
			divps	xmm3, xmm1;			//[dp1A, dp1B/dp1A, dp1G/dp1A, dp1R/dp1A]
			;//------------------------------------------------------------------------

			cvtps2dq	xmm3,xmm3	

			packssdw	xmm0, xmm3	
			packuswb	xmm0, xmm0	
			movq	qword ptr [rdi],xmm0	

			add	rdi, 8	
			sub	ecx, 1
			jne	tag_row_loop

tag_row_loop_end:
			pop 	rdi;
			pop 	rsi;
			movups		xmm8,[rsp + 32]
			movups		xmm7,[rsp + 16]
			movups		xmm6,[rsp]
			add			rsp,30h

	ret
;-----------------------------------------------------------------------------
; void _rip_x_filllinebuf_x64(int* ptempsubtable, 
;												 DWORD* plinebuf,			
;												 int acount,
;												 int mcount);		
;-----------------------------------------------------------------------------
_rip_x_filllinebuf_x64:
	;rcx/xmm0	:	ptempsubtable
	;rdx/xmm1	:	plinebuf
	;r8 /xmm2	:	acount
	;r9 /xmm3	:	mcount
				push rsi;
				push rdi;
				mov rsi,rcx;

				mov eax,[rsi];
				movd xmm0,dword ptr eax;
				shufps xmm0,xmm0,0;
				mov rdi,rdx;
				mov rcx,r8;
rip_x_filllinebuf_align4:		
				test ecx,ecx;
				jz rip_x_filllinebuf_align2;
				movdqu xmm1,[rdi];
				paddd xmm1,xmm0;
				movdqu [rdi],xmm1;
				sub ecx,1;
				add rdi,16;
				jmp rip_x_filllinebuf_align4;
rip_x_filllinebuf_align2:
				mov rcx,r9;
				test ecx,02h;
				jz rip_x_filllinebuf_align1;
				movsd xmm1,qword ptr[rdi];
				paddd xmm1,xmm0;
				movsd qword ptr[rdi],xmm1;
				add rdi,8;
rip_x_filllinebuf_align1:				
				test rcx,01h;
				jz rip_x_filllinebuf_loopend;
				movd xmm1,dword ptr[rdi];
				paddd xmm1,xmm0;
				movd dword ptr[rdi],xmm1;
rip_x_filllinebuf_loopend:
				pop rdi;
				pop rsi;
				
				ret

;-----------------------------------------------------------------------------
;void _rip_update_buf_byte_x64(int  g_nTotalErea,DWORD* psrcbuf,BYTE*  pdesbuf,DWORD* gc_dwScale_temp,int acount,int mcount);
;-----------------------------------------------------------------------------
_rip_update_buf_byte_x64:
	;rcx/xmm0	:	g_nTotalErea
	;rdx/xmm1	:	psrcbuf
	;r8 /xmm2	:	pdesbuf
	;r9 /xmm3	:	gc_dwScale_temp
	;[rsp + 28h]:	acount
	;[rsp + 30h]:	mcount
					push rsi;
					push rdi;
					movd xmm0,ecx;
					mov rsi,rdx;
					mov rdi,r8;
					mov ecx,[rsp + 28h + 10h];
rip_update_buf_byte_align4:		
					test ecx,ecx;
					jz rip_update_buf_byte_align2;
					movdqu xmm1,[rsi];
					packssdw	xmm1, xmm1
					pmulhuw xmm1,[r9];
					packuswb	xmm1, xmm1
					movd dword ptr[rdi],xmm1;
					sub ecx,1;
					add rsi,16;
					add rdi,4;
					jmp rip_update_buf_byte_align4;
rip_update_buf_byte_align2:
					mov ecx,[rsp + 30h + 10h];
					test ecx,02h;
					jz rip_update_buf_byte_align1;
					movsd xmm1,qword ptr[rsi];
					packssdw	xmm1, xmm1
					pmulhuw xmm1,[r9];
					packuswb	xmm1, xmm1
					movd eax,xmm1;
					mov [rdi],ax;
					add rdi,2;
					add rsi,8;
rip_update_buf_byte_align1:				
					test ecx,01h;
					jz rip_update_buf_byte_loopend;
					movd	xmm1,dword ptr[rsi];
					packssdw	xmm1, xmm1
					pmulhuw xmm1,[r9];
					packuswb	xmm1, xmm1
					movd eax,xmm1;
					mov [rdi],al;
rip_update_buf_byte_loopend:
					pop rdi;
					pop rsi;
					ret
;-----------------------------------------------------------------------------
; BOOL _yuv_yyuuvv2yuv_x64(BYTE* pDesBuffer,BYTE* pSrcField,int nA16,int nMod16,DWORD* pMask);
;-----------------------------------------------------------------------------
_yuv_yyuuvv2yuv_x64:
	;rcx/xmm0	:	pDesBuffer	
	;rdx/xmm1	:	pSrcField
	;r8 /xmm2	:	nA16
	;r9 /xmm3	:	nMod16
	;[rsp+28h]	:	pMask
	
		push	rdi
		push	rsi
	
		pxor	xmm0,xmm0
		movdqa	xmm1,oword ptr[rsp+28h+10h]
		mov		rdi,rcx
		mov		rsi,rdx
		mov		rcx,r8
yuv_yyuuvv2yuv_loop_16:
		test	 rcx,rcx
		jz		 yuv_yyuuvv2yuv_loop_8
		movdqu	 xmm2,[rsi]
		pand	 xmm2,xmm1
	    add		 rsi,16
		sub		 rcx,1
		jnz		 loop_16_1
		packuswb xmm2,xmm2
		movsd	 qword ptr[rdi],xmm2 ;pDesBuffer
		add		 rdi,8
		jmp		 yuv_yyuuvv2yuv_loop_8
loop_16_1:
		movdqu   xmm3,[rsi]
		pand	 xmm3,xmm1
		packuswb xmm2,xmm3
		movdqu	 [rdi],xmm2
		add		 rdi,16
		add		 rsi,16
		sub		 rcx,1
		jmp		 yuv_yyuuvv2yuv_loop_16
yuv_yyuuvv2yuv_loop_8:
		mov		 rcx,r9
		test	 rcx,000000008h
		jz		 yuv_yyuuvv2yuv_loop_4
		movsd	 xmm2,qword ptr[rsi]
		pand	 xmm2,xmm1
		packuswb xmm2,xmm2
		movd	 dword ptr[rdi],xmm2
		add		 rdi,4
		add		 rsi,8
yuv_yyuuvv2yuv_loop_4: 
		test	 rcx,000000004h
		jz		 yuv_yyuuvv2yuv_loop_2
		movd	 xmm2,dword ptr[rsi]
		pand	 xmm2,xmm1
		packuswb xmm2,xmm2
		movd	 rax,xmm2
		mov		 [rdi],ax
		add		 rdi,2
		add		 rsi,4
yuv_yyuuvv2yuv_loop_2:
		test	 rcx,000000002h
		jz		 yuv_yyuuvv2yuv_loop_end
		mov		 ax,[esi]
		movd	 xmm2,eax
		pand	 xmm2,xmm1
		packuswb xmm2,xmm2
		movd	 eax,xmm2
		mov		 [edi],al
		add		 rdi,1
		add		 rsi,2
yuv_yyuuvv2yuv_loop_end:
		pop		rsi
		pop		rdi
	ret
;-----------------------------------------------------------------------------
;BOOL _yuv_yuv2yyuuvv_x64(BYTE* pDesBuffer,BYTE* pSrcField,int nA8,int nMod8);
;-----------------------------------------------------------------------------
_yuv_yuv2yyuuvv_x64:
	;rcx/xmm0	:	pDesBuffer
	;rdx/xmm1	:	pSrcField
	;r8 /xmm2	:	nA8
	;r9 /xmm3	:	nMod8
	
		push	rdi
		push	rsi
	
		pxor	xmm0,xmm0
		mov		rdi,rcx
		mov		rsi,rdx
		mov		rcx,r8
yuv_yuv2yyuuvv_loop_8:
		test	 rcx,rcx
		jz		 yuv_yuv2yyuuvv_loop_4
		movsd	 xmm1,qword ptr[rsi]
		punpcklbw xmm1,xmm0
		movdqa	 xmm2,xmm1
		psllw	 xmm2,8
		por		 xmm1,xmm2
		movdqu	 [rdi],xmm1
		sub		 rcx,1
		add		 rsi,8
		add	     rdi,16
		jmp		 yuv_yuv2yyuuvv_loop_8
yuv_yuv2yyuuvv_loop_4: 
		mov		 rcx,r9
		test	 rcx,000000004h
		jz		 yuv_yuv2yyuuvv_loop_2
		movd	 xmm1,dword ptr[rsi]
		punpcklbw xmm1,xmm0
		movdqa	 xmm2,xmm1
		psllw	 xmm2,8
		por		 xmm1,xmm2
		movsd	 qword ptr[rdi],xmm1
		add		 rsi,4
		add	     rdi,8
yuv_yuv2yyuuvv_loop_2:
		test	 rcx,000000002h
		jz		 loop_1
		mov		 ax,[rsi]
		movd	 xmm1,eax
		punpcklbw xmm1,xmm0
		movdqa	 xmm2,xmm1
		psllw	 xmm2,8
		por		 xmm1,xmm2
		movd	 dword ptr[rdi],xmm1
		add		 rdi,4
		add		 rsi,2
loop_1:
		test	 rcx,000000001h
		jz		 yuv_yuv2yyuuvv_loop_end
		mov		 al,[rsi]
		movd	 xmm1,eax
		punpcklbw xmm1,xmm0
		movdqa	 xmm2,xmm1
		psllw	 xmm2,8
		por		 xmm1,xmm2
		movd	 eax,xmm1
		mov		 [rdi],ax
		add		 rdi,2
		add		 rsi,1
yuv_yuv2yyuuvv_loop_end:
		pop		rsi
		pop		rdi
	ret
;-----------------------------------------------------------------------------
; void _yuv_uyvy2yuv8_x64(BYTE* puyvy,BYTE* py,BYTE* pu,BYTE* pv,int nA16,int nM16);
;-----------------------------------------------------------------------------
_yuv_uyvy2yuv8_x64:
	;rcx/xmm0	:	puyvy
	;rdx/xmm1	:	py
	;r8 /xmm2	:	pu
	;r9 /xmm3	:	pv
	;[rsp+28h]	:	nA16
	;[rsp+30h]	:	nM16
	
		push		rdi
		push		rsi
	
		mov			rsi,rcx
		mov			rax,rdx
		mov			rdi,r8
		mov			rdx,r9
		mov			ecx,[rsp+28h+10h]
yuv_uyvy2yuv8_loop_16:		
		test		ecx,ecx
		jz			yuv_uyvy2yuv8_loop_8
		movdqu		xmm1,[rsi]
		movdqa		xmm2,xmm1
		psrlw		xmm1,8
		packuswb	xmm1,xmm1
		psllw		xmm2,8
		psrlw		xmm2,8
		packuswb	xmm2,xmm2

		movdqa		xmm3,xmm2
		psllw		xmm2,8
		psrlw		xmm2,8
		packuswb	xmm2,xmm2
		psrlw		xmm3,8
		packuswb	xmm3,xmm3

		movsd		qword ptr[rax],xmm1
		movd		dword ptr[rdi],xmm2
		movd		dword ptr[rdx],xmm3

		add			rsi,16
		add			rax,8
		add			rdx,4
		add			rdi,4
		sub			ecx,1
		jmp			yuv_uyvy2yuv8_loop_16
yuv_uyvy2yuv8_loop_8:
		mov			ecx,[rsp+30h+10h]
		test		ecx,000000008h
		jz			yuv_uyvy2yuv8_loop_4
		movsd		xmm1,qword ptr[esi]
		movsd		xmm2,xmm1
		psrlw		xmm1,8
		psllw		xmm2,8
		packuswb	xmm1,xmm1
		psrlw		xmm2,8
		packuswb	xmm2,xmm2

		movdqa		xmm3,xmm2
		psllw		xmm2,8
		psrlw		xmm2,8
		packuswb	xmm2,xmm2
		psrlw		xmm3,8
		packuswb	xmm3,xmm3

		movd		dword ptr[rax],xmm1
		push		rax
		movd		eax,xmm2
		mov			[rdi],ax
		movd		eax,xmm3
		mov			[rdx],ax
		pop			rax
		add			rsi,8
		add			rax,4
		add			rdx,2
		add			rdi,2
yuv_uyvy2yuv8_loop_4:
		test		ecx,000000004h
		jz			yuv_uyvy2yuv8_loop_end
		movd		xmm1,qword ptr[rsi]
		movdqa		xmm2,xmm1
		psrlw		xmm1,8
		psllw		xmm2,8
		packuswb	xmm1,xmm1
		psrlw		xmm2,8
		packuswb	xmm2,xmm2

		movdqa		xmm3,xmm2
		psllw		xmm2,8
		psrlw		xmm2,8
		packuswb	xmm2,xmm2
		psrlw		xmm3,8
		packuswb	xmm3,xmm3

		push		rdx
		movd		edx,xmm1
		mov			[rax],dx
		pop			rdx

		push		rax
		movd		eax,xmm2
		mov			[rdi],al
		movd		eax,xmm3
		mov			[rdx],al
		pop			rax

		add			rsi,8
		add			rax,4
		add			rdx,2
		add			rdi,2
yuv_uyvy2yuv8_loop_end:
		pop			rsi
		pop			rdi
	ret
;-----------------------------------------------------------------------------
;void _yuv_yuyv2yuv8_x64(BYTE* puyvy,BYTE* py,BYTE* pu,BYTE* pv,int nA16,int nM16);
;-----------------------------------------------------------------------------
_yuv_yuyv2yuv8_x64:
	;rcx/xmm0	:	puyvy
	;rdx/xmm1	:	py
	;r8 /xmm2	:	pu
	;r9 /xmm3	:	pv
	;[rsp+28h]	:	nA16
	;[rsp+30h]	:	nM16
	
		push		rdi
		push		rsi

		mov			rsi,rcx
		mov			rax,rdx
		mov			rdi,r8
		mov			rdx,r9
		mov			ecx,[rsp+28h+10h]
yuv_yuyv2yuv8_loop_16:		
		test		ecx,ecx
		jz			yuv_yuyv2yuv8_loop_8
		movdqu		xmm1,[rsi]
		movdqa		xmm2,xmm1
		psllw		xmm1,8
		psrlw		xmm1,8
		packuswb	xmm1,xmm1
		psrlw		xmm2,8
		packuswb	xmm2,xmm2

		movdqa		xmm3,xmm2
		psllw		xmm2,8
		psrlw		xmm2,8
		packuswb	xmm2,xmm2
		psrlw		xmm3,8
		packuswb	xmm3,xmm3

		
		

		movsd		qword ptr[rax],xmm1
		movd		dword ptr[rdi],xmm2
		movd		dword ptr[rdx],xmm3

		add			rsi,16
		add			rax,8
		add			rdx,4
		add			rdi,4
		sub			ecx,1
		jmp			yuv_yuyv2yuv8_loop_16
yuv_yuyv2yuv8_loop_8:
		mov			ecx,[rsp+30h+10h]
		test		ecx,000000008h
		jz			yuv_yuyv2yuv8_loop_4
		movsd		xmm1,qword ptr[rsi]
		movsd		xmm2,xmm1
		psllw		xmm1,8
		psrlw		xmm1,8
		packuswb	xmm1,xmm1
		psrlw		xmm2,8
		packuswb	xmm2,xmm2

		movdqa		xmm3,xmm2
		psllw		xmm2,8
		psrlw		xmm2,8
		packuswb	xmm2,xmm2
		psrlw		xmm3,8
		packuswb	xmm3,xmm3

		movd		dword ptr[rax],xmm1
		push		rax
		movd		eax,xmm2
		mov			[rdi],ax
		movd		eax,xmm3
		mov			[rdx],ax
		pop			rax
		add			rsi,8
		add			rax,4
		add			rdx,2
		add			rdi,2
yuv_yuyv2yuv8_loop_4:
		test		ecx,000000004h
		jz			yuv_yuyv2yuv8_loop_end
		movd		xmm1,dword ptr[rsi]
		movdqa		xmm2,xmm1
		psllw		xmm1,8
		psrlw		xmm1,8
		packuswb	xmm1,xmm1
		psrlw		xmm2,8
		packuswb	xmm2,xmm2

		movdqa		xmm3,xmm2
		psllw		xmm2,8
		psrlw		xmm2,8
		packuswb	xmm2,xmm2
		psrlw		xmm3,8
		packuswb	xmm3,xmm3

		push		rdx
		movd		edx,xmm1
		mov			[rax],dx
		pop			rdx

		push		rax
		movd		eax,xmm2
		mov			[rdi],al
		movd		eax,xmm3
		mov			[rdx],al
		pop			rax

		add			rsi,8
		add			rax,4
		add			rdx,2
		add			rdi,2
yuv_yuyv2yuv8_loop_end:
		pop			rsi
		pop			rdi
	ret
;-----------------------------------------------------------------------------
;void _bgra32_mean_copy_add_y_quick_field_x64(OUT void* des,	
;								  IN void* src,		
;								  IN int scw,	
;								  IN int src_pitch,
;								  IN void* pPowerY,	
;								  IN void* pCoefH,
;								  IN int nMapY,
;								  IN int nFirst);
;-----------------------------------------------------------------------------
_bgra32_mean_copy_add_y_quick_field_x64:
	;rcx/xmm0	:	des
	;rdx/xmm1	:	src
	;r8 /xmm2	:	scw
	;r9 /xmm3	:	src_pitch
	;[rsp+28h]	:	pPowerY
	;[rsp+30h]	:	pCoefH
	;[rsp+38h]	:	nMapY
	;[rsp+40h]	:	nFirst
	
		push		rdi
		push		rsi
		sub			rsp,20h
		movups		[rsp],xmm6
		movups		[rsp + 10h],xmm7
	
		mov			rdi, rcx;
		mov			rsi, rdx;
		mov			rcx, r8;
		mov			rdx, [rsp+28h+30h];
		movss		xmm2, dword ptr[rdx];		
		movss		xmm3, dword ptr[rdx + 004h];	
		shufps		xmm2, xmm2, 000h;	
		shufps		xmm3, xmm3, 000h;
		mov			rax, [rsp+30h+30h];
		pxor		xmm0, xmm0;			
		movaps		xmm7, [rax];	
		
loop_pixel:
		mov			rdx, rsi;
		pxor		xmm6, xmm6;
		pxor		xmm5, xmm5;
		mov			eax,[rsp+40h+30h];
		test		eax,eax
		mov			eax, [rsp+38h+30h];
		jz			loop_skip;
seg_first_row:
		movss		xmm1, dword ptr[rdx];		
		punpcklbw	xmm1, xmm0;			
		punpcklwd	xmm1, xmm0;			
		cvtdq2ps	xmm1, xmm1;
		mulps		xmm1, xmm2;
		add			rdx, r9;
		addps		xmm6, xmm1;
		add			rdx, r9;
		sub			eax, 003h;
		jb			map_end;			
		jnz			loop_mid_row;		
		jmp			seg_last_row;		
loop_skip:
		add			rdx, r9;
		sub			eax, 002h;
		jb			map_end;			
		jnz			loop_mid_row;		
		jmp			seg_last_row;
loop_mid_row:
		movd		xmm1, dword ptr[rdx];		
		punpcklbw	xmm1, xmm0;			
		add			rdx, r9;
		paddusw		xmm5, xmm1;
		add			rdx, r9;
		sub			eax, 002h;
		jb			loop_mid_toend;		
		jnz			loop_mid_row;
		jz			seg_last_row;
loop_mid_toend:
		punpcklwd	xmm5, xmm0;			// 000a 000r 000g 000b
		cvtdq2ps	xmm5, xmm5;
		addps		xmm6, xmm5;			
		jmp         map_end;
seg_last_row:
		movss		xmm1, dword ptr[rdx];		// 0 0 0 argb
		punpcklbw	xmm1, xmm0;			// 0000 0000 0a0r 0g0b
		punpcklwd	xmm1, xmm0;			// 000a 000r 000g 000b
		cvtdq2ps	xmm1, xmm1;
		punpcklwd	xmm5, xmm0;			// 000a 000r 000g 000b
		mulps		xmm1, xmm3;
		cvtdq2ps	xmm5, xmm5;
		addps		xmm6, xmm5;
		addps		xmm6, xmm1;			
		
map_end:
		mulps		xmm6, xmm7;		
		movaps		[rdi], xmm6;
		add			esi, 004h;
		add			edi, 010h;
		sub			ecx, 001h;
		jnz			loop_pixel;
		
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 10h]
		add			rsp,20h
		pop			rsi
		pop			rdi
	ret
;-----------------------------------------------------------------------------
;void _asmX64_SSE2_FastMemset(LPDWORD lpdwDes,DWORD	dwColor,int nSize);
;-----------------------------------------------------------------------------
_asmX64_SSE2_FastMemset: 
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

end

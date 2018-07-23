;--------------------------------------------------------------------------------
;for 64 bit asm
;--------------------------------------------------------------------------------
	public rectf_scale_x64
;--------------------------------------------------------------------------------
	public matrix4_negative_x64
	public matrix4_addnum_x64
	public matrix4_mulnum_x64
	public matrix4_add_x64
	public matrix4_minus_x64
	public matrix4_mul_x64
	public matrix4_fromquatn_x64
	public matrix4_fromrotvector_x64
	public matrix4_fromeuler_lyxz_x64
	public matrix4_transpose_x64
;--------------------------------------------------------------------------------
	public _quatnNegtive_x64
	public _quatnAdd_x64
	public _quatnMinus_x64
	public _quatnStandardMul_x64
	public _quatnCustomMul_x64
	public _quatnNormalize_x64
	public _quatnScale_x64
	public _quatnMulNum_x64
	public _quatnMode_x64	
	public _quatnDotProduct_x64
	public _quatnSlerp_x64
	public _quatnFromAngleAxis_x64
	public _quatnFromEuler_LYXZ_x64
	public _quatnMulMatQ_x64
;--------------------------------------------------------------------------------
	public float2MulMatQ_x64
	public float3Add_x64
	public float3Sub_x64
	public float3AddNum_x64
	public float3Offset_x64
	public float3Scale_x64
	public float3MulNum_x64
	public float3Normalize_x64
	public float3Mode_x64
	public float3DotProduct_x64
	public float3CrossProduct_x64
	public float3NormalMulMat_x64
	public float3MulMatQ_x64
	public float4Add_x64
	public float4Sub_x64
	public float4AddNum_x64
	public float4Offset_x64
	public float4Scale_x64
	public float4MulNum_x64
	public float4Normalize_x64
	public float4Mode_x64
	public float4DotProduct_x64
	public float4MulMatQ_x64
	public float4MatQMul_x64
;--------------------------------------------------------------------------------
.data
.code
ALIGN 16
;-----------------------------------------------------------------------------
;  void	rectf_scale_x64(rectf* rt,float fscale);
;-----------------------------------------------------------------------------

rectf_scale_x64:

	;xmm1 - fscale
    ;rcx - rt 
    
	xorps		xmm0, xmm0;
	movss		xmm0, xmm1;
	shufps		xmm0, xmm0, 00h;
	movups		xmm1,[rcx];
	mulps		xmm1, xmm0;
	movups		[rcx],xmm1;
	
    ret
;--------------------------------------------------------------------------------

;--------------------------------------------------------------------------------
;Matrix
;--------------------------------------------------------------------------------
;void matrix4_negative_x64(pmatrix4 pDesM,		// 目标矩阵指针
;						pmatrix4 pSrcM)			// 源矩阵指针
;--------------------------------------------------------------------------
matrix4_negative_x64:
	;rcx	- pDesM
	;rdx	- pSrcM
	
	xorps		xmm1, xmm1;
	xorps		xmm2, xmm2;
	xorps		xmm3, xmm3;
	xorps		xmm4, xmm4;
	
	movups		xmm0, [rdx];			// 第一行
	subps		xmm1, xmm0;
	
	movups		xmm0, [rdx + 10h];		// 第二行
	subps		xmm2, xmm0;
	
	movups		xmm0, [rdx + 20h];		// 第三行
	subps		xmm3, xmm0;
	
	movups		xmm0, [rdx + 30h];		// 第四行
	subps		xmm4, xmm0;
	
	movups		[rcx],		 xmm1;
	movups		[rcx + 10h], xmm2;
	movups		[rcx + 20h], xmm3;
	movups		[rcx + 30h], xmm4;
	ret

;--------------------------------------------------------------------------
;void matrix4_addnum_x64(pmatrix4 pDesM,			// 目标矩阵指针
;							const matrix4* pSrcM,		// 源矩阵指针
;							float k)					// 数
;--------------------------------------------------------------------------
matrix4_addnum_x64:
	;rcx	- pDesM
	;rdx	- pSrcM
	;xmm2	- k
	
	MOV		eax,0ffffffffh;
	xorps		xmm0, xmm0;						// 0 0 0 0
	movd		xmm0, eax;
	pand		xmm2,xmm0;
	
	movdqa		xmm4, xmm2;
	
	movups		xmm0, [rdx];		// 第一行
	movups		xmm1, [rdx + 10h];		// 第二行
	movups		xmm2, [rdx + 20h];		// 第三行
	movups		xmm3, [rdx + 30h];		// 第四行
	
	
	addps		xmm0, xmm4;
	
	movdqa		xmm5, xmm4;
	shufps		xmm5, xmm5, 51h;	// 0 0 k 0
	addps		xmm1, xmm5;
	
	movdqa		xmm5, xmm4;
	shufps		xmm5, xmm5, 45h;	// 0 k 0 0
	addps		xmm2, xmm5;
	
	movdqa		xmm5, xmm4;
	shufps		xmm5, xmm5, 15h;	// k 0 0 0
	addps		xmm3, xmm5;
	
	movups		[rcx], xmm0;
	movups		[rcx + 10h], xmm1;
	movups		[rcx + 20h], xmm2;
	movups		[rcx + 30h], xmm3;

	ret

;--------------------------------------------------------------------------
;	void matrix4_mulnum_x64(pmatrix4 pDesM,			//目标矩阵指针
;								pmatrix4 pSrcM,		//源矩阵指针
;								float k)				//数
;--------------------------------------------------------------------------
matrix4_mulnum_x64:
	;rcx	- pDesM
	;rdx	- pSrcM
	;xmm2	- k
	
	movss		xmm5, xmm2;			// 0 0 0 k
	shufps		xmm5, xmm5, 00h;	// k k k k
	movups		xmm0, [rdx];		// 第一行
	movups		xmm1, [rdx + 10h];		// 第二行
	movups		xmm2, [rdx + 20h];		// 第三行
	movups		xmm3, [rdx + 30h];		// 第四行
	mulps		xmm0, xmm5;
	mulps		xmm1, xmm5;
	mulps		xmm2, xmm5;
	mulps		xmm3, xmm5;
	movups		[rcx], xmm0;
	movups		[rcx + 10h], xmm1;
	movups		[rcx + 20h], xmm2;
	movups		[rcx + 30h], xmm3;
	
	ret
	
;--------------------------------------------------------------------------
;	void matrix4_add_x64(pmatrix4 pDesM,		// 目标矩阵指针
;							pmatrix4 pSrcML,		// 左矩阵指针
;							pmatrix4 pSrcMR)		// 右矩阵指针
;--------------------------------------------------------------------------
matrix4_add_x64:
	;rcx	- pDesM
	;rdx	- pSrcML
	;r8		- pSrcMR
	
		movups		xmm0, [rdx ];		// 第一行
		movups		xmm1, [rdx + 10h];		// 第二行
		movups		xmm2, [rdx + 20h];		// 第三行
		movups		xmm3, [rdx + 30h];		// 第四行
		
		movups		xmm4, [r8];		// 第一行
		addps		xmm0, xmm4;			// 第一行
		
		movups		xmm5, [r8 + 10h];		// 第二行
		addps		xmm1, xmm5;			// 第二行
		
		movups		xmm4, [r8 + 20h];		// 第三行
		addps		xmm2, xmm4;			// 第三行
		
		movups		xmm5, [r8 + 30h];		// 第四行
		addps		xmm3, xmm5;			// 第四行
		;// 求和
		
		movups		[rcx], xmm0;		// 第一行
		movups		[rcx + 10h], xmm1;		// 第二行
		movups		[rcx + 20h], xmm2;		// 第三行
		movups		[rcx + 30h], xmm3;		// 第四行
		
		ret
		
;--------------------------------------------------------------------------
;		void matrix4_minus_x64(pmatrix4 pDesM,			// 目标矩阵指针
;								pmatrix4 pSrcML,		// 左矩阵指针
;								pmatrix4 pSrcMR)		// 右矩阵指针
;--------------------------------------------------------------------------
matrix4_minus_x64:
	;rcx	- pDesM
	;rdx	- pSrcML
	;r8		- pSrcMR

	
	movups		xmm0, [rdx];		// 第一行
	movups		xmm1, [rdx + 10h];		// 第二行
	movups		xmm2, [rdx + 20h];		// 第三行
	movups		xmm3, [rdx + 30h];		// 第四行
	
	movups		xmm4, [r8];			// 第一行
	subps		xmm0, xmm4;			// 第一行
	
	movups		xmm5, [r8 + 10h];		// 第二行
	subps		xmm1, xmm5;			// 第二行
	
	movups		xmm4, [r8 + 20h];		// 第三行
	subps		xmm2, xmm4;			// 第三行
	
	movups		xmm4, [r8 + 30h];		// 第四行
	subps		xmm3, xmm4;			// 第四行
	;// 求差
	movups		[rcx]	, xmm0;		// 第一行
	movups		[rcx + 10h], xmm1;		// 第二行
	movups		[rcx + 20h], xmm2;		// 第三行
	movups		[rcx + 30h], xmm3;		// 第四行
	
	ret

;--------------------------------------------------------------------------
;		void __cdecl matrix4_mul_x64(pmatrix4 pDesM,		// 目标矩阵指针
;							 const matrix4* pSrcML,		// 左矩阵指针
;							 const matrix4*  pSrcMR)		// 右矩阵指针
;--------------------------------------------------------------------------
matrix4_mul_x64:
	;rcx	- pDesM
	;rdx	- pSrcML
	;r8		- pSrcMR

		sub			rsp,32
		movups		[rsp],xmm6
		movups		[rsp + 16],xmm7
		
		;// 先将右矩阵完全读出
		movups		xmm4, [r8];		// 第一行
		movups		xmm5, [r8 + 10h];		// 第二行
		movups		xmm6, [r8 + 20h];		// 第三行
		movups		xmm7, [r8 + 30h];		// 第四行
		mov			eax, 04h;
loop_line:
		movups		xmm0, [rdx];		// 第一行
		movdqa		xmm1, xmm0;
		movdqa		xmm2, xmm0;
		movdqa		xmm3, xmm0;
		
		shufps		xmm0, xmm0, 00h;	// 第一列
		shufps		xmm1, xmm1, 55h;	// 第二列
		shufps		xmm2, xmm2, 0aah;	// 第三列
		shufps		xmm3, xmm3, 0ffh;	// 第四列
		mulps		xmm0, xmm4;			// 第一列 * 第一行
		mulps		xmm1, xmm5;			// 第二列 * 第二行
		mulps		xmm2, xmm6;			// 第三列 * 第三行
		mulps		xmm3, xmm7;			// 第四列 * 第四行
		addps		xmm0, xmm1;
		addps		xmm2, xmm3;
		addps		xmm0, xmm2;			// 求和得到第一行
		movups		[rcx], xmm0;
		add			rdx,10h;
		add			rcx,10h;
		sub			eax, 01h;
		jnz			loop_line;
		
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 16]
		add			rsp,32
		ret

;--------------------------------------------------------------------------
;			void __cdecl matrix4_fromquatn(pmatrix4 pDesM,		// 目标矩阵指针
;								   const quatn* pSrcQ,			// 源四元数指针
;									float fOne)					
;--------------------------------------------------------------------------
matrix4_fromquatn_x64:
	;rcx	- pDesM
	;rdx	- pSrcQ
	;xmm2	- fOne
	
		sub			rsp,32
		movups		[rsp],xmm6
		movups		[rsp + 16],xmm7
		
	xorps		xmm1,xmm1;
	movups		xmm7, [rdx];		// r z y x
	movss		xmm1, xmm2;		// 0 0 0 1
	
	movdqa		xmm0, xmm7;
	movdqa		xmm2, xmm7;
	movdqa		xmm5, xmm7;
	movdqa		xmm6, xmm7;
	shufps		xmm2, xmm2, 0ffh;	// r r r r
	shufps		xmm5, xmm5, 0d2h;	// r y x z
	shufps		xmm6, xmm6, 0c9h;	// r x z y
	shufps		xmm1, xmm1, 15h;	// 1 0 0 0
	mulps		xmm0, xmm0;			// r^2 z^2 y^2 x^2
	mulps		xmm7, xmm2;			// r^2 rz ry rx
	mulps		xmm5, xmm6;			// r^2 xy xz yz
	movdqa		xmm2, xmm0;
	movdqa		xmm3, xmm0;
	movdqa		xmm4, xmm0;
	shufps		xmm2, xmm2, 0ffh;	// r^2 r^2 r^2 r^2
	shufps		xmm3, xmm3, 0c9h;	// r^2 x^2 z^2 y^2
	shufps		xmm4, xmm4, 0d2h;	// r^2 y^2 x^2 z^2
	addps		xmm5, xmm5;			// 2r^2  2xy  2xz  2yz
	addps		xmm7, xmm7;			// 2r^2  2rz  2ry  2rx
	addps		xmm0, xmm2;			// 2r^2  r^2+z^2  r^2+y^2  r^2+x^2
	addps		xmm3, xmm4;			// 2r^2  x^2+y^2  x^2+z^2  y^2+z^2
	movdqa		xmm6, xmm5;
	subps		xmm5, xmm7;			// 0  _21  _13  _32
	subps		xmm0, xmm3;			// 0  _33  _22  _11
	addps		xmm6, xmm7;			// *  _12  _31  _23
	movdqa		xmm2, xmm5;
	movdqa		xmm4, xmm0;
	shufps		xmm5, xmm6, 50h;	// _31  _31  _32  _32
	shufps		xmm0, xmm6, 0a0h;	// _12  _12  _11  _11
	shufps		xmm6, xmm2, 0d0h;	// 0  _13  _23  _23
	shufps		xmm2, xmm4, 5ah;	// _22  _22  _21  _21
	shufps		xmm5, xmm4, 0e2h;	// 0  _33  _32  _31
	shufps		xmm0, xmm6, 0e8h;	// 0  _13  _12  _11
	shufps		xmm2, xmm6, 0c8h;	// 0  _23  _22  _21
	movups		[rcx], xmm0;		// 第一行
	movups		[rcx + 10h], xmm2;		// 第二行
	movups		[rcx + 20h], xmm5;		// 第三行	
	movups		[rcx + 30h], xmm1;		// 第四行

		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 16]
		add			rsp,32
	ret		
		
		
;--------------------------------------------------------------------------
;void matrix4_fromrotvector_x64(pmatrix4 pDesM, 
;								const float3* pSrcV, 
;								float fAngle,
;								float fCoefCos, 
;								float fCoefSin,
;								float fOne);
;--------------------------------------------------------------------------
matrix4_fromrotvector_x64:
	;rcx			- pDesM
	;rdx			- pSrcV
	;xmm2			- fAngle //无用
	;xmm3			- fCoefCos
	;[rsp + 28h]	- fCoefSin 
	;[rsp + 30h]	- fOne
	
		sub			rsp,32
		movups		[rsp],xmm6
		movups		[rsp + 16],xmm7
		
		xorps	xmm2,xmm2;
	movlps		xmm7, QWORD PTR[rdx];				// * * y x
	movss		xmm6, DWORD PTR[rdx + 08h];				// 0 0 0 z
	movss		xmm1, DWORD PTR[rsp + 30h + 32];		// 0 0 0 1
	movss		xmm2, xmm3;						// 0 0 0 cosA
	movss		xmm3, DWORD PTR[rsp + 28h + 32];		// 0 0 0 sinA
	movlhps		xmm7, xmm6;						// 0 z y x
	movdqa		xmm0, xmm1;
	movdqa		xmm5, xmm7;
	movdqa		xmm6, xmm7;
	subps		xmm1, xmm2;			// 0 0 0 1-cosA
	shufps		xmm3, xmm3, 40h;		// 0 sinA sinA sinA
	shufps		xmm5, xmm5, 0c9h;		// 0 x z y
	shufps		xmm1, xmm1, 40h;		// 0 1-cosA 1-cosA 1-cosA
	shufps		xmm2, xmm2, 40h;		// 0 cosA cosA cosA
	shufps		xmm6, xmm6, 0d2h;		// 0 y x z
	mulps		xmm1, xmm7;			// 0 z(1-cosA) y(1-cosA) x(1-cosA)
	mulps		xmm3, xmm5;			// 0 xsinA zsinA ysinA
	mulps		xmm6, xmm1;			// 0 yz(1-cosA) xy(1-cosA) zx(1-cosA)
	mulps		xmm7, xmm1;			// 0 z^2(1-cosA) y^2(1-cosA) x^2(1-cosA)
	movdqa		xmm4, xmm6;
	subps		xmm6, xmm3;			// 0  _32  _21  _13
	addps		xmm7, xmm2;			// 0  _33  _22  _11
	addps		xmm4, xmm3;			// 0  _23  _12  _31
	movdqa		xmm5, xmm6;
	movdqa		xmm1, xmm7;
	shufps		xmm5, xmm7, 55h;	// _22  _22  _21  _21
	shufps		xmm1, xmm4, 50h;	// _12  _12  _11  _11
	shufps		xmm5, xmm4, 0e8h;	// 0  _23  _22  _21
	shufps		xmm4, xmm6, 0a0h;	// _32  _32  _31  _31
	shufps		xmm1, xmm6, 0c8h;	// 0  _13  _12  _11
	shufps		xmm4, xmm7, 0e8h;	// 0  _33  _32  _31
	shufps		xmm0, xmm0, 15h;	// _44 0 0 0
	movups		[rcx ], xmm1;		// 第一行
	movups		[rcx+ 10h], xmm5;		// 第二行
	movups		[rcx + 20h], xmm4;		// 第三行		
	movups		[rcx + 30h], xmm0;		// 第四行
		
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 16]
		add			rsp,32
	ret
	
;--------------------------------------------------------------------------
;void matrix4_fromeuler_lyxz_x64(pmatrix4 pDesM, 
;									float fCosX, 
;									float fSinX, 
;									float fCosY, 
;									float fSinY, 
;									float fCosZ, 
;									float fSinZ, 
;									float fOne)
;--------------------------------------------------------------------------
matrix4_fromeuler_lyxz_x64:
	;rcx			- pDesM
	;xmm1			- fCosX 
	;xmm2			- fSinX
	;xmm3			- fCosY
	;[rsp + 28h]	- fSinY
	;[rsp + 30h]	- fCosZ
	;[rsp + 38h]	- fSinZ
	;[rsp + 40h]	- fOne 
	;[rsp + 48h]	- 
	;[rsp + 50h]	- 
	;[rsp + 58h]	- 

		sub			rsp,32
		movups		[rsp],xmm6
		movups		[rsp + 16],xmm7
		MOV		edx,0ffffffffh;
	xorps		xmm0, xmm0;						// 0 0 0 0
	movd		xmm0, edx;
	pand		xmm2,xmm0;
	pand		xmm1,xmm0;
	pand		xmm3,xmm0;
	movss		xmm4, DWORD PTR[rsp + 28h + 32];		// 0 0 0 sinY
	movss		xmm5, DWORD PTR[rsp + 30h + 32];		// 0 0 0 cosZ
	movss		xmm6, DWORD PTR[rsp + 38h + 32];		// 0 0 0 sinZ
	movss		xmm7, DWORD PTR[rsp + 40h + 32];		// 0 0 0 1
	xorps		xmm0, xmm0;						// 0 0 0 0
	subps		xmm0, xmm2;		// 0 0 0 _23
	shufps		xmm4, xmm3, 00h;	// cosY cosY sinY sinY
	shufps		xmm6, xmm5, 00h;	// cosZ cosZ sinZ sinZ
	shufps		xmm1, xmm1, 00h;	// cosX cosX cosX cosX
	shufps		xmm2, xmm2, 00h;	// sinX sinX sinX sinX
	shufps		xmm7, xmm7, 15h;	// _44 0 0 0
	movdqa		xmm3, xmm4;
	movdqa		xmm5, xmm6;
	shufps		xmm6, xmm6, 88h;	// cosZ sinZ cosZ sinZ
	shufps		xmm4, xmm5, 88h;	// cosZ sinZ cosY sinY
	mulps		xmm3, xmm6;			// cosYcosZ  cosYsinZ  sinYcosZ  sinYsinZ
	mulps		xmm1, xmm4;			// cosXcosZ  cosXsinZ  cosYcosX  sinYcosX ( _22  _21  _33  _13 )
	xorps		xmm5, xmm5;
	subps		xmm5, xmm3;			// -cosYcosZ  -cosYsinZ  -sinYcosZ  -sinYsinZ
	mulps		xmm2, xmm3;			// cosYsinXcosZ  cosYsinXsinZ  sinYsinXcosZ  sinYsinXsinZ
	shufps		xmm5, xmm3, 0c9h;	// cosYcosZ  sinYsinZ  -cosYsinZ  -sinYcosZ
	shufps		xmm0, xmm1, 44h;	// _33  _13  0  _23
	shufps		xmm2, xmm2, 36h;	// sinYsinXsinZ  cosYsinXcosZ  sinYsinXcosZ  cosYsinXsinZ
	addps		xmm2, xmm5;			// _11  _32  _12  _31
	movdqa		xmm4, xmm2;
	shufps		xmm1, xmm0, 4eh;	// 0  _23  _22  _21
	shufps		xmm2, xmm0, 67h;	// 0  _13  _12  _11
	shufps		xmm4, xmm0, 78h;	// 0  _33  _32  _31
	movups		[rcx], xmm2;		// 第一行
	movups		[rcx + 10h], xmm1;		// 第二行
	movups		[rcx + 20h], xmm4;		// 第三行
	movups		[rcx + 30h], xmm7;		// 第四行
	
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 16]
		add			rsp,32
	ret
		
;--------------------------------------------------------------------------
;	void  matrix4_transpose_x64(pmatrix4 pDesM, 
;									pmatrix4 pSrcM)
;--------------------------------------------------------------------------
matrix4_transpose_x64:
	;rcx	- pDesM
	;rdx	- pSrcM
	
	movups		xmm0, [rdx];		// 第一行
	movups		xmm1, [rdx + 10h];		// 第二行
	movups		xmm2, [rdx + 20h];		// 第三行
	movups		xmm3, [rdx + 30h];		// 第四行
	movdqa		xmm4, xmm0;
	movdqa		xmm5, xmm2;
	shufps		xmm0, xmm1, 44h;	// m22 m21 m12 m11
	shufps		xmm2, xmm3, 44h;	// m42 m41 m32 m31
	shufps		xmm4, xmm1, 0eeh;	// m24 m23 m14 m13
	shufps		xmm5, xmm3, 0eeh;	// m44 m43 m34 m33
	movdqa		xmm1, xmm0;		// m22 m21 m12 m11
	movdqa		xmm3, xmm4;		// m24 m23 m14 m13
	shufps		xmm0, xmm2, 88h;	// m41 m31 m21 m11
	shufps		xmm1, xmm2, 0ddh;	// m42 m32 m22 m12
	shufps		xmm3, xmm5, 88h;	// m43 m33 m23 m13
	shufps		xmm4, xmm5, 0ddh;	// m44 m34 m24 m14
	movups		[rcx], xmm0;
	movups		[rcx + 10h], xmm1;
	movups		[rcx + 20h], xmm3;
	movups		[rcx + 30h], xmm4;
	
	ret
;--------------------------------------------------------------------------------	
;end matrix
;--------------------------------------------------------------------------------

;--------------------------------------------------------------------------------
;quatn	
;--------------------------------------------------------------------------------
; void			_quatnNegtive_x64(quatn* pQ);
;--------------------------------------------------------------------------------
_quatnNegtive_x64:
	;rcx : pQ
	xorps		xmm0, xmm0;
	movups		xmm1, [rcx];
	subps		xmm0, xmm1;
	movups		[rcx], xmm0;

	ret

;--------------------------------------------------------------------------------
; void			_quatnAdd_x64(quatn* pDesQ,const quatn* pSrcQL,const quatn* pSrcQR);
;--------------------------------------------------------------------------------
_quatnAdd_x64:
	;rcx : pDesQ
	;rdx : pSrcQL
	;r8  : pSrcQR
	movups		xmm0, [rdx];		// ql
	movups		xmm1, [r8];		// qr
	addps		xmm0, xmm1;			// ql + qr
	movups		[rcx], xmm0;
	ret
;--------------------------------------------------------------------------------
; void _quatnMinus_x64(quatn* pDesQ,const quatn* pSrcQL,const quatn* pSrcQR)
;--------------------------------------------------------------------------------
_quatnMinus_x64:
	;rcx	: 	pDesQ
	;rdx	:	pSrcQL
	;r8		: 	pSrcQR
	movups		xmm0, [rdx];		// ql
	movups		xmm1, [r8];		// qr
	subps		xmm0, xmm1;			// ql - qr
	movups		[rcx], xmm0;
	ret
;--------------------------------------------------------------------------------
; void _quatnStandardMul_x64(quatn* pDesQ, const quatn* pSrcQL, const quatn* pSrcQR)
;--------------------------------------------------------------------------------
_quatnStandardMul_x64:
	;rcx	: 	pDesQ
	;rdx	:	pSrcQL
	;r8		: 	pSrcQR
	
		sub			rsp,32
		movups		[rsp],xmm6
		movups		[rsp + 16],xmm7
		
	movups		xmm1, [rdx];		// r1 z1 y1 x1
	movups		xmm2, [r8];		// r2 z2 y2 x2
	movdqa		xmm3, xmm1;
	movdqa		xmm5, xmm1;
	movdqa		xmm4, xmm2;
	movdqa		xmm6, xmm2;
	shufps		xmm3, xmm3, 0ffh;	// r1 r1 r1 r1
	shufps		xmm5, xmm5, 092h;	// z1 y1 x1 z1
	shufps		xmm4, xmm4, 0ffh;	// r2 r2 r2 r2
	shufps		xmm6, xmm6, 0cdh;	// z2 x2 z2 y2
	xorps		xmm0, xmm0;
	mulps		xmm3, xmm2;			// r1*r2  r1*zz  r1*y2  r1*x2
	mulps		xmm4, xmm1;			// r2*r1  r2*z1  r2*y1  r2*x1
	mulps		xmm2, xmm1;			// r1*r2  z1*z2  y1*y2  z1*z2
	mulps		xmm5, xmm6;			// z1*z2  y1*x2  x1*z2  z1*y2
	mulps		xmm1, xmm6;			// r1*z2  z1*x2  y1*z2  x1*y2
	subps		xmm0, xmm2;			// -r1*r2  -z1*z2  -y1*y2  -x1*x2
	subps		xmm3, xmm5;			// r1*r2-z1*z2  r1*z2-y1*x2  r1*y2-x1*z2  r1*x1-z1*y2
	shufps		xmm1, xmm1, 0c9h;	// r1*z2  x1*y2  z1*x2  y1*z2
	movhlps		xmm6, xmm4;			// @  @  r2*r1  r2*z1
	movhlps		xmm7, xmm1;			// @  @  r1*z2  x1*y2
	movlhps		xmm6, xmm0;			// -y1*y2  -x1*x2  r2*r1  r2*z1
	movlhps		xmm7, xmm0;			// -y1*y2  -x1*x2  r1*z2  x1*y2
	shufps		xmm6, xmm6, 088h;	// -x1*x2  r2*z1  -x1*x2  r2*z1
	shufps		xmm7, xmm7, 0cch;	// -y1*y2  x1*y2  -y1*y2  x1*y2
	movlhps		xmm4, xmm6;			// -x1*x2  r2*z1  r2*y1  r2*x1
	movlhps		xmm1, xmm7;			// -y1*y2  x1*y2  z2*x1  y1*z2
	addps		xmm3, xmm4;			// r1*r2-x1*x2-z1*z2  r1*z2+r2*z1-y1*x2  r1*y2+r2*y1-x1*z2  r1*x2+r2*x1-z1*y2
	addps		xmm3, xmm1;			// r z y x
	movups		[rcx], xmm3;
	
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 16]
		add			rsp,32
	ret
;--------------------------------------------------------------------------------
;void _quatnCustomMul_x64(quatn* pDesQ, const quatn* pSrcQL, const quatn* pSrcQR)
;--------------------------------------------------------------------------------
_quatnCustomMul_x64:
	;rcx	: 	pDesQ
	;rdx	:	pSrcQL
	;r8		: 	pSrcQR
	
		sub			rsp,32
		movups		[rsp],xmm6
		movups		[rsp + 16],xmm7
	movups		xmm1, [rdx];		// r1 z1 y1 x1
	movups		xmm2, [r8];		// r2 z2 y2 x2
	movdqa		xmm3, xmm1;
	movdqa		xmm5, xmm1;
	movdqa		xmm4, xmm2;
	movdqa		xmm6, xmm2;
	shufps		xmm3, xmm3, 0ffh;	// r1 r1 r1 r1
	shufps		xmm5, xmm5, 089h;	// z1 x1 z1 y1
	shufps		xmm4, xmm4, 0ffh;	// r2 r2 r2 r2
	shufps		xmm6, xmm6, 092h;	// z2 y2 x2 z2
	xorps		xmm0, xmm0;
	mulps		xmm3, xmm2;			// r1*r2  r1*zz  r1*y2  r1*x2
	mulps		xmm4, xmm1;			// r2*r1  r2*z1  r2*y1  r2*x1
	mulps		xmm2, xmm1;			// r1*r2  z1*z2  y1*y2  z1*z2
	mulps		xmm5, xmm6;			// z1*z2  x1*y2  z1*x2  y1*z2
	mulps		xmm1, xmm6;			// r1*z2  z1*y2  y1*x2  x1*z2
	subps		xmm0, xmm2;			// -r1*r2  -z1*z2  -y1*y2  -x1*x2
	subps		xmm3, xmm5;			// r1*r2-z1*z2  r1*z2-x1*y2  r1*y2-z1*x2  r1*x1-y1*z2
	shufps		xmm1, xmm1, 0d2h;	// r1*z2  y1*x2  x1*z2  z1*y2
	movhlps		xmm6, xmm4;			// @  @  r2*r1  r2*z1
	movhlps		xmm7, xmm1;			// @  @  r1*z2  y1*x2
	movlhps		xmm6, xmm0;			// -y1*y2  -x1*x2  r2*r1  r2*z1
	movlhps		xmm7, xmm0;			// -y1*y2  -x1*x2  r1*z2  y1*x2
	shufps		xmm6, xmm6, 088h;	// -x1*x2  r2*z1  -x1*x2  r2*z1
	shufps		xmm7, xmm7, 0cch;	// -y1*y2  y1*x2  -y1*y2  y1*x2
	movlhps		xmm4, xmm6;			// -x1*x2  r2*z1  r2*y1  r2*x1
	movlhps		xmm1, xmm7;			// -y1*y2  y1*x2  x2*z1  z1*y2
	addps		xmm3, xmm4;			// r1*r2-x1*x2-z1*z2  r1*z2+r2*z1-x1*y2  r1*y2+r2*y1-z1*x2  r1*x2+r2*x1-y1*z2
	addps		xmm3, xmm1;			// r z y x
	movups		[rcx], xmm3;
	
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 16]
		add			rsp,32
	ret
;--------------------------------------------------------------------------------
;void _quatnNormalize_x64(quatn* pDesQ, const quatn* pSrcQ)
;--------------------------------------------------------------------------------
_quatnNormalize_x64:
	;rcx	:  pDestQ	
    ;rdx	:  pSrcQ
	movups		xmm0, [rdx];
	movdqa		xmm7, xmm0;
	mulps		xmm0, xmm0;		// 骞虫柟
	movhlps		xmm1, xmm0;
	addps		xmm0, xmm1;
	pshufd		xmm2, xmm0, 001h;	// 姹傛ā
	addps		xmm0, xmm2;
	rsqrtss		xmm0, xmm0;
	shufps		xmm0, xmm0, 000h;
	mulps		xmm0, xmm7;		// 褰掍竴鍖?
	movups		[rcx], xmm0;
	ret
;--------------------------------------------------------------------------------
;void _quatnScale_x64(quatn* pQ, float k)
;--------------------------------------------------------------------------------
_quatnScale_x64:
	;rcx/xmm0	: 	pQ
    ;rdx/xmm1	:	k
    ;r8	/xmm2	: 	
	;r9 /xmm3	:
		;movss		xmm1, k;
		movups		xmm0, [rcx];		// r z y x
		shufps		xmm1, xmm1, 000h;	// k k k k
		mulps		xmm0, xmm1;
		movups		[rcx], xmm0;
		ret

;--------------------------------------------------------------------------------
;void _quatnMulNum_x64(quatn* pDesQ, const quatn* pSrcQ, float k)
;--------------------------------------------------------------------------------
_quatnMulNum_x64:
	;rcx/xmm0	: pDesQ
    ;rdx/xmm1	: pSrcQ
    ;r8	/xmm2	: k
    ;r9 /xmm3	:

		;movss		xmm1, xmm2;
		movups		xmm0, [rdx];
		shufps		xmm2, xmm2, 000h;
		mulps		xmm0, xmm2;
		movups		[rcx], xmm0;
	ret

;--------------------------------------------------------------------------------
;void _quatnMode_x64(float* pfRes,const quatn* pQ)			// 鍥涘厓鏁版寚閽?
;--------------------------------------------------------------------------------
	;rcx/xmm0	: pfRes
    ;rdx/xmm1	: pQ
    ;r8	/xmm2	:
    ;r9 /xmm3	:
_quatnMode_x64:	
		movups		xmm0, [rdx];
		mulps		xmm0, xmm0;
		movhlps		xmm1, xmm0;
		addps		xmm0, xmm1;
		pshufd		xmm2, xmm0, 001h;
		addps		xmm0, xmm2;
		rsqrtss		xmm0, xmm0;
		rcpss		xmm0, xmm0;				// 骞虫柟鏍癸紙浼氭瘮鐩存帴浣跨敤 sqrtss 蹇?5 鍊嶏紝浣嗕細鍑虹幇濂囧紓鍊硷級
		movss		dword ptr[rcx], xmm0;
	ret

;--------------------------------------------------------------------------------
; void _quatnDotProduct_x64(float* pfRes, const quatn* pQ1, const quatn* pQ2)
;--------------------------------------------------------------------------------
_quatnDotProduct_x64:
		;rcx/xmm0	: pfRes
        ;rdx/xmm1	: pQ1
        ;r8	/xmm2	: pQ2
        ;r9 /xmm3	:
		movups		xmm0, [rdx];
		movups		xmm1, [r8];
		mulps		xmm0, xmm1;
		movhlps		xmm2, xmm0;
		addps		xmm0, xmm2;
		pshufd		xmm3, xmm0, 001h;
		addps		xmm0, xmm3;
		movss		dword ptr[rcx], xmm0;
	ret
;--------------------------------------------------------------------------------
;void _quatnSlerp_x64(quatn* pDesQ,const quatn* pSrcQ0,const quatn* pSrcQ1,float k0,float k1)
;--------------------------------------------------------------------------------
_quatnSlerp_x64:
	;rcx/xmm0	: pDesQ
    ;rdx/xmm1	: pSrcQ0
    ;r8	/xmm2	: pSrcQ1
    ;r9 /xmm3	: k0
	;[rsp + 28h]; k1
		movups		xmm0, [rdx];
		movups		xmm1, [r8];
		movss		xmm2, xmm3;
		movss		xmm3, dword ptr[rsp + 28h];
		shufps		xmm2, xmm2, 000h;
		shufps		xmm3, xmm3, 000h;
		mulps		xmm0, xmm2;
		mulps		xmm1, xmm3;
		addps		xmm0, xmm1;
		movups		[rcx], xmm0;
	ret

;--------------------------------------------------------------------------------
;void _quatnFromAngleAxis_x64(quatn* pDesQ, const float3* pSrcV, float fCosCoef,float fSinCoef)
;--------------------------------------------------------------------------------
_quatnFromAngleAxis_x64:
	;rcx/xmm0	: pDesQ
    ;rdx/xmm1	: pSrcV
    ;r8	/xmm2	: fCosCoef
    ;r9 /xmm3	: fSinCoef
    
	MOV		eax,0ffffffffh;
	xorps		xmm4, xmm4;						// 0 0 0 0
	movd		xmm4, eax;
	pand		xmm2,xmm4;
	pand		xmm3,xmm4;
	
		movlps		xmm0, qword ptr[rdx];
		movss		xmm1, dword ptr[rdx + 08h];
		movlhps		xmm0, xmm1;			// 0 z y x
		shufps		xmm3, xmm3, 40h;	// 0 fSin fSin fSin
		shufps		xmm2, xmm2, 15h;	// fCos 0 0 0
		mulps		xmm0, xmm3;			// 蹇呴』 0 * 0 鍚﹀垯浣滄垨鐨勬椂鍊欎細鍑虹幇璐熸暟
		orps		xmm0, xmm2;
		movups		[rcx], xmm0;
	ret

;--------------------------------------------------------------------------------
;void _quatnFromEuler_LYXZ_x64(quatn* pDesQ, float fCosX, float fSinX,
;									float fCosY, float fSinY,
;									float fCosZ, float fSinZ)		// 缁?X 杞存棆杞搴?
;--------------------------------------------------------------------------------
_quatnFromEuler_LYXZ_x64:
	;rcx/xmm0	: pDesQ
    ;rdx/xmm1	: fCosX
    ;r8	/xmm2	: fSinX
    ;r9 /xmm3	: fCosY
	;[rsp + 28h]: fSinY
	;[rsp + 30h]: fCosZ
	;[rsp + 38h]: fSinZ
		sub			rsp,10h
		movups		[rsp],xmm6
		
		movss		xmm5, xmm2;		// 0 0 0 sinX
		movss		xmm6, xmm1;		// 0 0 0 cosX
		movss		xmm1, dword ptr[rsp + 28h + 10h];		// 0 0 0 sinY
		movss		xmm2, xmm3;		// 0 0 0 cosY
		movss		xmm3, dword ptr[rsp + 38h + 10h];		// 0 0 0 sinZ
		movss		xmm4, dword ptr[rsp + 30h + 10h];		// 0 0 0 cosZ
		xorps		xmm0, xmm0;
		shufps		xmm3, xmm4, 000h;	// cosZ cosZ sinZ sinZ
		shufps		xmm1, xmm2, 000h;	// cosY cosY sinY sinY
		shufps		xmm3, xmm3, 088h;	// cosZ sinZ cosZ sinZ
		shufps		xmm5, xmm5, 000h;	// sinX sinX sinX sinX
		shufps		xmm6, xmm6, 000h;	// cosX cosX cosX cosX
		mulps		xmm1, xmm3;			// cosYcosZ  cosYsinZ  sinYcosZ  sinYsinZ
		subps		xmm0, xmm1;			// -cosYcosZ  -cosYsinZ  -sinYcosZ  -sinYsinZ
		movdqa		xmm2, xmm1;			// cosYcosZ  cosYsinZ  sinYcosZ  sinYsinZ
		shufps		xmm1, xmm0, 0eeh;	// -cosYcosZ  -cosYsinZ  cosYcosZ  cosYsinZ
		shufps		xmm2, xmm0, 044h;	// -sinYcosZ  -sinYsinZ  sinYcosZ  sinYsinZ
		movdqa		xmm3, xmm1;			// -cosYcosZ  -cosYsinZ  cosYcosZ  cosYsinZ
		shufps		xmm1, xmm2, 013h;	// sinYsinZ  sinYcosZ  cosYsinZ  -cosYcosZ
		shufps		xmm2, xmm3, 06eh;	// cosYcosZ  -cosYsinZ  -sinYcosZ  -sinYsinZ
		mulps		xmm1, xmm5;			// sinYsinXsinZ  sinYsinXcosZ  cosYsinXsinZ  -cosYsinXcosZ
		mulps		xmm2, xmm6;			// cosYcosXcosZ  -cosYcosXsinZ  -sinYcosXcosZ  -sinYcosXsinZ
		addps		xmm1, xmm2;			// r z y x
		movups		[rcx], xmm1;
		
		movups		xmm6,[rsp]
		add			rsp,10h
	ret

;--------------------------------------------------------------------------------
; void _quatnMulMatQ_x64(quatn* pDesQ, const quatn* pSrcQ, const matrix4* pSrcM)
;--------------------------------------------------------------------------------
_quatnMulMatQ_x64:
	;rcx/xmm0	: pDesQ
    ;rdx/xmm1	: pSrcQ
    ;r8	/xmm2	: pSrcM
    ;r9 /xmm3	:
    
		sub			rsp,32
		movups		[rsp],xmm6
		movups		[rsp + 16],xmm7
		
		movups		xmm0, [rdx];		// r z y x
		movups		xmm4, [r8];		// 绗竴琛?
		movups		xmm5, [r8 + 10h];		// 绗簩琛?
		movups		xmm6, [r8 + 20h];		// 绗笁琛?
		movups		xmm7, [r8 + 30h];		// 绗洓琛?
		movdqa		xmm1, xmm0;
		movdqa		xmm2, xmm0;
		movdqa		xmm3, xmm0;
		shufps		xmm0, xmm0, 000h;	// x x x x
		shufps		xmm1, xmm1, 055h;	// y y y y
		shufps		xmm2, xmm2, 0aah;	// z z z z
		shufps		xmm3, xmm3, 0ffh;	// r r r r
		mulps		xmm4, xmm0;			// x * m1
		mulps		xmm5, xmm1;			// y * m2
		mulps		xmm6, xmm2;			// z * m3
		mulps		xmm7, xmm3;			// r * m4
		addps		xmm4, xmm5;
		addps		xmm6, xmm7;
		addps		xmm4, xmm6;
		movups		[rcx], xmm4;
		
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 16]
		add			rsp,32
	ret
;--------------------------------------------------------------------------------	
;end quatn
;--------------------------------------------------------------------------------

;--------------------------------------------------------------------------------	
;Vector
;--------------------------------------------------------------------------------
;rcx,rdx,r8,r9
;xmm0,xmm1,xmm2,xmm3
float2MulMatQ_x64:

	
		movlps		xmm1, QWORD PTR [rdx];	// @ @ y x
		movups		xmm4, [r8];	// 第一行
		movups		xmm5, [r8 + 10h];	// 第二行
		movups		xmm3, [r8 + 30h];	// 第四行
		movdqa		xmm0, xmm1;
		shufps		xmm0, xmm0, 00h;	// x
		shufps		xmm1, xmm1, 55h;	// y
		mulps		xmm4, xmm0;
		mulps		xmm5, xmm1;
		addps		xmm4, xmm5;
		addps		xmm4, xmm3;		// * z y x
		movlps		QWORD PTR [rcx], xmm4;
		ret;
		

;rcx,rdx,r8,r9
;xmm0,xmm1,xmm2,xmm3
float3Add_x64:
		movlps		xmm1, QWORD PTR [rdx];		// @ @ y1 x1
		movlps		xmm3, QWORD PTR [r8];		// @ @ y2 x2
		add			rdx,08h;
		add			r8,08h;
		movss		xmm2, DWORD PTR [rdx];		// 0 0 0 z1
		movss		xmm4, DWORD PTR [r8];		// 0 0 0 z2
		movlhps		xmm1, xmm2;			// 0 z1 y1 x1
		movlhps		xmm3, xmm4;			// 0 z2 y2 x2
		addps		xmm1, xmm3;			// 0 z y x
		movhlps		xmm2, xmm1;			// 0 0 0 z
		movlps		QWORD PTR [rcx], xmm1;
		add			rcx,08h;
		movss		DWORD PTR [rcx], xmm2;
		ret
		


float3Sub_x64:							;(float3* pDesV,		// 目标向量
									;const float3* pSrcVL,		// 左向量
									;const float3* pSrcVR)		// 右向量
		movlps		xmm1, QWORD PTR [rdx];		// @ @ y1 x1
		movlps		xmm3, QWORD PTR [r8];		// @ @ y2 x2
		add			rdx,08h;
		add			r8,08h;
		movss		xmm2, DWORD PTR [rdx];		// 0 0 0 z1
		movss		xmm4, DWORD PTR [r8];		// 0 0 0 z2
		movlhps		xmm1, xmm2;			// 0 z1 y1 x1
		movlhps		xmm3, xmm4;			// 0 z2 y2 x2
		subps		xmm1, xmm3;			// 0 z y x
		movhlps		xmm2, xmm1;			// 0 0 0 z
		movlps		QWORD PTR [rcx], xmm1;
		add			rcx,08h;
		movss		DWORD PTR [rcx], xmm2;
		ret
		
		
float3AddNum_x64:				;float3* pDesV,	// 目标向量
								;const float3* pSrcV,	// 向量
								;float k)				// 数
								
		movlps		xmm1, QWORD PTR [rdx];		// @ @ y x
		shufps		xmm2, xmm2, 00h;	// k k k k
		add			rdx,08h;
		movss		xmm5, DWORD PTR [rdx];		// 0 0 0 z
		movlhps		xmm1, xmm5;			// 0 z y x
		addps		xmm1, xmm2;			// @ k+z k+y k+x
		movhlps		xmm3, xmm1;			// 0 0 @ k+z
		movlps		QWORD PTR [rcx], xmm1;
		add			rcx,08h;
		movss		DWORD PTR [rcx], xmm3;
		ret


float3Offset_x64:					;(float3* pV,	// 向量
								;float k)			// 数
	mov			rdx,  rcx;								
	movlps		xmm0, QWORD PTR [rcx];		// @ @ y x
	add			rcx,08h
	movss		xmm5, DWORD PTR [rcx];		// 0 0 0 z
	shufps		xmm1, xmm1, 00h;	// k k k k
	movlhps		xmm0, xmm5;			// 0 z y x
	addps		xmm0, xmm1;			// @ k+z k+y k+x
	movhlps		xmm1, xmm0;			// 0 0 @ k+z
	movlps		QWORD PTR [rdx], xmm0;
	add			rdx,08h
	movss		DWORD PTR [rdx], xmm1;
	ret;

float3Scale_x64:					;(float3* pV,			// 向量指针
								;float k)						// 缩放倍数
		mov			rdx,rcx;
		movlps		xmm0, QWORD PTR [rcx];		// @ @ y x
		add			rcx, 08h;
		movss		xmm2, DWORD PTR [rcx];		// 0 0 0 z
		shufps		xmm1, xmm1, 00h;	// k k k k
		movlhps		xmm0, xmm2;			// 0 z y x
		mulps		xmm0, xmm1;			// 0 k*z k*y k*x
		movhlps		xmm1, xmm0;			// 0 0 0 k*z
		movlps		QWORD PTR [rdx], xmm0;
		add			rdx,08h;
		movss		DWORD PTR [rdx], xmm1;
		ret;



float3MulNum_x64:				;(float3* pDesV,		// 目标向量指针
								;const float3* pSrcV,		// 源向量指针
								;float k)					// 缩放倍数
		movlps		xmm0, QWORD PTR [rdx];		// @ @ y x
		add			rdx,08h;
		movss		xmm1, DWORD PTR [rdx];		// 0 0 0 z
		shufps		xmm2, xmm2, 00h;	// k k k k
		movlhps		xmm0, xmm1;			// 0 z y x
		mulps		xmm0, xmm2 ;			// 0 k*z k*y k*x
		movhlps		xmm1, xmm0;			// 0 0 0 k*z
		movlps		QWORD PTR [rcx], xmm0;
		add			rcx,08h;
		movss		DWORD PTR [rcx], xmm1;
		ret;


float3Normalize_x64:					;(float3* pDesV,			// 目标向量指针
									;const float3* pSrcV)			// 源向量指针
	movlps		xmm0, QWORD PTR [rdx];		// @ @ y x
	add			rdx,08h
	movss		xmm1, DWORD PTR [rdx];		// 0 0 0 z
	
	movlhps		xmm0, xmm1;			// 0 z y x
	movdqa		xmm2, xmm0;
	mulps		xmm0, xmm0;			// 0 z^2 y^2 x^2
	movhlps		xmm1, xmm0;			// 0 0 0 z^2
	addps		xmm1, xmm0;			// 0 z^2 y^2 x^2+z^2
	shufps		xmm0, xmm0, 01h;	// * * * y^2
	addps		xmm0, xmm1;			// * * * x^2+y^2+z^2
	sqrtss		xmm0, xmm0;			// 求模的倒数 a
	shufps		xmm0, xmm0, 00h;	// a a a a
	divps 		xmm2, xmm0;			// 0 z*a y*a x*a
	movhlps		xmm1, xmm2;			// 0 0 0 z*a
	movlps		QWORD PTR [rcx], xmm2;
	add			rcx,08h;
	movss		DWORD PTR [rcx], xmm1;
	ret;


float3Mode_x64:				;(const float3* pV)			// 向量指针
	movlps		xmm0, QWORD PTR [rcx];		// @ @ y x
	movss		xmm1, DWORD PTR [rcx + 08h];		// 0 0 0 z

	movlhps		xmm0, xmm1;			// 0 z y x
	movdqa		xmm2, xmm0;
	mulps		xmm0, xmm0;			// 0 z^2 y^2 x^2
	movhlps		xmm1, xmm0;			// 0 0 0 z^2
	addps		xmm1, xmm0;			// 0 z^2 y^2 x^2+z^2
	shufps		xmm0, xmm0, 01h;	// * * * y^2
	addps		xmm0, xmm1;			// * * * x^2+y^2+z^2
	sqrtss		xmm0, xmm0;			// 求模的倒数 a
		ret
	;return (float)sqrt(pV->x * (double)pV->x + pV->y * (double)pV->y + pV->z * (double)pV->z);


float3DotProduct_x64:			;(const float3* pV1,		// 向量指针
								;const float3* pV2)		// 向量指针
		movlps		xmm0, QWORD PTR [rcx];		// @ @ y1 x1
		movlps		xmm2, QWORD PTR [rdx];		// 0 0 0 z1
		add			rcx,08h;
		add			rdx,08h;
		movss		xmm1, DWORD PTR [rcx];		// @ @ y2 x2
		movss		xmm3, DWORD PTR [rdx];		// 0 0 0 z2
		movlhps		xmm0, xmm1;			// 0 z1 y1 x1
		movlhps		xmm2, xmm3;			// 0 z2 y2 x2
		mulps		xmm0, xmm2;			// 0 z1*z2 y1*y2 x1*x2
		movhlps		xmm1, xmm0;			// 0 0 0 z1*z2
		addps		xmm1, xmm0;			// * * * x1*x2+z1*z2
		shufps		xmm0, xmm0, 01h;	// * * * y1*y2
		addps		xmm0, xmm1;			// * * * x1*x2+y1*y2+z1*z2
		ret;


;///////////////////////////////////////////////////////////////////////////////
;// 描述：	向量叉乘,右手法则
;//
;// 返回值:	无
;///////////////////////////////////////////////////////////////////////////////
float3CrossProduct_x64:				;(float3* pDesV,		// 目标向量指针
									;const float3* pSrcV1,		// 源向量指针
									;const float3* pSrcV2)		// 源向量指针
	movlps		xmm0, QWORD PTR [rdx];		// @ @ y1 x1
	movlps		xmm2, QWORD PTR [r8];		// @ @ y2 x2
	add			rdx,08h;
	add			r8,08h;
	movss		xmm1, DWORD PTR [rdx];		// 0 0 0 z1
	movss		xmm3, DWORD PTR [r8];		// 0 0 0 z2
	movlhps		xmm0, xmm1;		// 0 z1 y1 x1
	movlhps		xmm2, xmm3;		// 0 z2 y2 x2
	pshufd		xmm1, xmm0, 12h;	// x1 y1 x1 z1
	shufps		xmm0, xmm0, 09h;	// x1 x1 z1 y1
	mulps		xmm1, xmm2;		// 0 y1*z2 x1*y2 z1*x2
	mulps		xmm0, xmm2;		// 0 x1*z2 z1*y2 y1*x2
	shufps		xmm1, xmm1, 12h;	// z1*x2 x1*y2 z1*x2 y1*z2
	shufps		xmm0, xmm0, 09h;	// y1*x2 y1*x2 x1*z2 z1*y2
	subps		xmm1, xmm0;		// @ x1*y2-y1*x2 z1*x2-x1*z2 y1*z2-z1*y2
	movhlps		xmm0, xmm1;
	movlps		QWORD PTR [rcx], xmm1;
	add			rcx,08h;
	movss		DWORD PTR [rcx], xmm0;
	ret;



;///////////////////////////////////////////////////////////////////////////////
;// 描述：	矩阵与向量的乘积
;//
;// 返回值:	无
;///////////////////////////////////////////////////////////////////////////////
float3NormalMulMat_x64:			;(float3* pDesV,			// 目标向量指针
								;const float3* pSrcV,			// 源向量指针
								;const matrix4* pSrcM)				// 矩阵指针
								
		movlps		xmm1, QWORD PTR [rdx];	// @ @ y x
		add			rdx,08h;
		movss		xmm2, DWORD PTR [rdx];	// 0 0 0 z
		
		movups		xmm4, [r8];	// 第一行
		movups		xmm5, [r8 + 10h];	// 第二行
		movups		xmm3, [r8 + 20h];	// 第三行
		
		pshufd		xmm0, xmm1, 00h;	// x
		shufps		xmm1, xmm1, 55h;	// y
		shufps		xmm2, xmm2, 00h;	// z
		mulps		xmm4, xmm0;
		mulps		xmm5, xmm1;
		mulps		xmm3, xmm2;
		addps		xmm4, xmm5;
		addps		xmm4, xmm3;		// * z y x
		movhlps		xmm1, xmm4;		// * * * z
		movlps		QWORD PTR [rcx], xmm4;
		movss		DWORD PTR [rcx + 08h], xmm1;
		ret;

	;float3 temp;
	;temp.x = pSrcM->m[0][0]*pSrcV->x+pSrcM->m[1][0]*pSrcV->y+pSrcM->m[2][0]*pSrcV->z;
	;temp.y = pSrcM->m[0][1]*pSrcV->x+pSrcM->m[1][1]*pSrcV->y+pSrcM->m[2][1]*pSrcV->z;
	;temp.z = pSrcM->m[0][2]*pSrcV->x+pSrcM->m[1][2]*pSrcV->y+pSrcM->m[2][2]*pSrcV->z;
	;*pDesV = temp;


;//////////////////////////////////////////////////////////////////////////
;// 描述：	四维向量与矩阵的乘积（补最后一维为单位值）
;//			本系统采用行向量与矩阵的乘法定义
;//
;// 返回值：	无
;//////////////////////////////////////////////////////////////////////////
float3MulMatQ_x64:				;(float3* pDesV,			// 目标向量指针
								;const float3* pSrcV,			// 源向量指针
								;const matrix4* pSrcM)			// 矩阵指针
	movlps		xmm1, QWORD PTR [rdx];	// @ @ y x
	add			rdx,08h
	movss		xmm2, DWORD PTR [rdx];	// 0 0 0 z
	movups		xmm4, [r8];	// 第一行
	movups		xmm5, [r8 + 10h];	// 第二行
	movups		xmm3, [r8 + 20h];	// 第三行
	movdqa		xmm0, xmm1;
	shufps		xmm0, xmm0, 00h;	// x
	shufps		xmm1, xmm1, 055h;	// y
	shufps		xmm2, xmm2, 00h;	// z
	mulps		xmm4, xmm0;
	mulps		xmm5, xmm1;
	mulps		xmm3, xmm2;
	addps		xmm4, xmm5;
	addps		xmm4, xmm3;		// * z y x
	movups		xmm3, [r8 + 30h];	// 第四行
	addps		xmm4, xmm3;
	movhlps		xmm1, xmm4;		// * * * z
	movlps		QWORD PTR [rcx], xmm4;
	movss		DWORD PTR [rcx + 08h], xmm1;
	ret;

	;assert(pDesV != pSrcV);
	;pDesV->x = pSrcV->x * pSrcM->_11 + pSrcV->y * pSrcM->_21 + pSrcV->z * pSrcM->_31 + 1.f * pSrcM->_41;
	;pDesV->y = pSrcV->x * pSrcM->_12 + pSrcV->y * pSrcM->_22 + pSrcV->z * pSrcM->_32 + 1.f * pSrcM->_42;
	;pDesV->z = pSrcV->x * pSrcM->_13 + pSrcV->y * pSrcM->_23 + pSrcV->z * pSrcM->_33 + 1.f * pSrcM->_43;


;//////////////////////////////////////////////////////////////////////////
;// 描述：	加减法
;//
;// 返回值:	无
;///////////////////////////////////////////////////////////////////////////////
float4Add_x64:				;(float4* pDesV,		// 目标向量
							;const float4* pSrcV1,		// 向量 1
							;const float4* pSrcV2)		// 向量 2
	movups		xmm1, [rdx];		// w1 z1 y1 x1
	movups		xmm2, [r8];		// w2 z2 y2 x2
	addps		xmm1, xmm2;			// w z y x
	movups		[rcx], xmm1;
	ret;

	;pDesV->x = pSrcV1->x + pSrcV2->x;
	;pDesV->y = pSrcV1->y + pSrcV2->y;
	;pDesV->z = pSrcV1->z + pSrcV2->z;
	;pDesV->z = pSrcV1->w + pSrcV2->w;


float4Sub_x64:				;(float4* pDesV,		// 目标向量
							;const float4* pSrcVL,		// 左向量
							;const float4* pSrcVR)		// 右向量
	movups		xmm1, [rdx];		// wL zL yL xL
	movups		xmm2, [r8];		// wR zR yR xR
	subps		xmm1, xmm2;			// w z y x
	movups		[rcx], xmm1;
	ret;
		
	;pDesV->x = pSrcVL->x - pSrcVR->x;
	;pDesV->y = pSrcVL->y - pSrcVR->y;
	;pDesV->z = pSrcVL->z - pSrcVR->z;
	;pDesV->z = pSrcVL->w - pSrcVR->w;


float4AddNum_x64:			;(float4* pDesV,	// 目标向量
							;const float4* pSrcV,	// 向量
							;float k)				// 数
	movups		xmm1, [rdx];		// w z y x
	shufps		xmm2, xmm2, 00h;	// k k k k
	addps		xmm1, xmm2;			// w+k k+z k+y k+x
	movups		[rcx], xmm1;
	ret;

	;pDesV->x = pSrcV->x + k;
	;pDesV->y = pSrcV->y + k;
	;pDesV->z = pSrcV->z + k;
	;pDesV->z = pSrcV->w + k;


float4Offset_x64:		;(float4* pV,	// 向量
						;float k)			// 数
	movups		xmm0, [rcx];		// w z y x
	shufps		xmm1, xmm1, 00h;	// k k k k
	addps		xmm0, xmm1;			// w+k k+z k+y k+x
	movups		[rcx], xmm0;
	ret;

	;pV->x += k;
	;pV->y += k;
	;pV->z += k;
	;pV->w += k;


;///////////////////////////////////////////////////////////////////////////////
;// 描述：	缩放向量
;//
;// 返回值:	无
;///////////////////////////////////////////////////////////////////////////////
float4Scale_x64:			;(float4* pV,			// 向量指针
						;float k)						// 缩放倍数
	movups		xmm0, [rcx];		// w z y x
	shufps		xmm1, xmm1, 00h;	// k k k k
	mulps		xmm0, xmm1;			// w+k k+z k+y k+x
	movups		[rcx], xmm0;
	ret;
	
	;pV->x *= k;
	;pV->y *= k;
	;pV->z *= k;
	;pV->w *= k;


float4MulNum_x64:		;(float4* pDesV,		// 目标向量指针
						;const float4* pSrcV,		// 源向量指针
						;float k)					// 缩放倍数
						
		movups		xmm1, [rdx];		// w z y x
		shufps		xmm2, xmm2, 00h;	// k k k k
		mulps		xmm1, xmm2;			// w*k k*z k*y k*x
		movups		[rcx], xmm1;
		ret;

	;pDesV->x = pSrcV->x * k;
	;pDesV->y = pSrcV->y * k;
	;pDesV->z = pSrcV->z * k;
	;pDesV->w = pSrcV->w * k;


;///////////////////////////////////////////////////////////////////////////////
;// 描述：	归一化向量
;//
;// 返回值:	无
;///////////////////////////////////////////////////////////////////////////////
float4Normalize_x64:		;(float4* pDesV,			// 目标向量指针
						;const float4* pSrcV)			// 源向量指针
		movups		xmm0, [rdx];		// w z y x
		movdqa		xmm3, xmm0;
		mulps		xmm0, xmm0;			// w^2 z^2 y^2 x^2
		movhlps		xmm1, xmm0;			// 0 0 w^2 z^2
		addps		xmm0, xmm1;
		pshufd		xmm2, xmm0, 01h;	// 求模
		addps		xmm0, xmm2;
		rsqrtss		xmm0, xmm0;
		shufps		xmm0, xmm0, 00h;	// a a a a
		mulps		xmm0, xmm3;			// w*a z*a y*a x*a
		movups		[rcx], xmm0;
		ret;

	;float k = 1.f / (float4Mode(pSrcV) + 0.0000011f);

	;pDesV->x = pSrcV->x * k;
	;pDesV->y = pSrcV->y * k;
	;pDesV->z = pSrcV->z * k;
	;pDesV->w = pSrcV->w * k;



;///////////////////////////////////////////////////////////////////////////////
;// 描述：	取向量的模
;//
;// 返回值:	float
;///////////////////////////////////////////////////////////////////////////////
float4Mode_x64:			;(const float4* pV)			// 向量指针
		movups		xmm1, [rcx];		// xmm1 [w, z, y, x]
		movhlps		xmm2, xmm1;			// xmm2 [?, ?, w, z]
		cvtps2pd	xmm3, xmm1;			// xmm3 [yd, xd]
		cvtps2pd	xmm4, xmm2;			// xmm4 [wd, zd]
		mulpd		xmm3, xmm3;			// xmm3 [yd*yd, xd*xd]
		mulpd		xmm4, xmm4;			// xmm4 [wd*wd, zd*zd]
		addpd		xmm4, xmm3;			// xmm4 [wd*wd+yd*yd, zd*zd + xd*xd]
		movhlps		xmm5, xmm4;			// xmm5 [??, wd*wd+yd*yd]
		addsd		xmm5, xmm4;			// xmm5 [??, wd*wd + zd*zd + xd*xd + yd*yd]
		sqrtsd		xmm5, xmm5;			// xmm5 [??, sqrt(wd*wd + zd*zd + xd*xd + yd*yd)]
		cvtsd2ss	xmm5, xmm5;			// xmm3 [?, ?, ?, (float)sqrt(wd*wd + zd*zd + xd*xd + yd*yd)]
		movss		xmm0, xmm5;
		ret;
		
;	fRes = (float)sqrt(pV->x * (double)pV->x + (double)pV->y * pV->y + (double)pV->z * pV->z + (double)pV->w * pV->w);



;//////////////////////////////////////////////////////////////////////////
;// 描述：	向量与矩阵的乘积，本系统采用行向量与矩阵的乘法定义
;//
;// 返回值：	无
;//////////////////////////////////////////////////////////////////////////
float4MulMatQ_x64:			;(float4* pDesV,			// 目标向量指针
							;const float4* pSrcV,			// 源向量指针
							;const matrix4* pSrcM)			// 矩阵指针
	movups		xmm0, [rdx];		// w z y x
	movdqa		xmm1, xmm0;
	movdqa		xmm2, xmm0;
	movdqa		xmm3, xmm0;
	shufps		xmm0, xmm0, 00h;	// x x x x
	shufps		xmm1, xmm1, 055h;	// y y y y
	shufps		xmm2, xmm2, 0aah;	// z z z z
	shufps		xmm3, xmm3, 0ffh;	// w w w w
	
	movups		xmm4, [r8 + 30h];		// 第四行
	mulps		xmm4, xmm3;			// w * m4
	
	movups		xmm3, [r8 + 20h];		// 第三行
	mulps		xmm3, xmm2;			// z * m3
	
	movups		xmm2, [r8 + 10h];		// 第二行
	mulps		xmm2, xmm1;			// y * m2
	
	movups		xmm1, [r8];		// 第一行
	mulps		xmm1, xmm0;			// x * m1
	
	addps		xmm4, xmm3;
	addps		xmm1, xmm2;
	addps		xmm4, xmm1;
	movups		[rcx], xmm4;
	ret;

	;pDesV->x = pSrcV->x * pSrcM->_11 + pSrcV->y * pSrcM->_21 + pSrcV->z * pSrcM->_31 + pSrcV->w * pSrcM->_41;
	;pDesV->y = pSrcV->x * pSrcM->_12 + pSrcV->y * pSrcM->_22 + pSrcV->z * pSrcM->_32 + pSrcV->w * pSrcM->_42;
	;pDesV->z = pSrcV->x * pSrcM->_13 + pSrcV->y * pSrcM->_23 + pSrcV->z * pSrcM->_33 + pSrcV->w * pSrcM->_43;
	;pDesV->w = pSrcV->x * pSrcM->_14 + pSrcV->y * pSrcM->_24 + pSrcV->z * pSrcM->_34 + pSrcV->w * pSrcM->_44;


float4MatQMul_x64:			;(float4* pDesV,			// 目标向量指针
							;const matrix4* pSrcM,			// 矩阵指针
							;const float4* pSrcV)			// 源向量指针
		sub			rsp,32
		movups		[rsp],xmm6
		movups		[rsp + 16],xmm7
							
	movups		xmm0, [r8];				// w z y x
	movups		xmm4, [rdx];			// 第一行
	movups		xmm5, [rdx + 10h];		// 第二行
	movups		xmm6, [rdx + 20h];		// 第三行
	movups		xmm7, [rdx + 30h];		// 第四行
	mulps		xmm4, xmm0;
	mulps		xmm5, xmm0;
	mulps		xmm6, xmm0;
	mulps		xmm7, xmm0;
	movhlps		xmm0, xmm4;
	movhlps		xmm1, xmm5;
	movhlps		xmm2, xmm6;
	movhlps		xmm3, xmm7;
	addps		xmm0, xmm4;			// w + y  z + x
	addps		xmm1, xmm5;
	addps		xmm2, xmm6;
	addps		xmm3, xmm7;
	movlhps		xmm0, xmm1;			// yR  xR
	movlhps		xmm2, xmm3;			// wR  zR
	movdqa		xmm4, xmm0;
	movdqa		xmm6, xmm2;
	shufps		xmm4, xmm4, 0b1h;
	shufps		xmm6, xmm6, 0b1h;
	addps		xmm0, xmm4;
	addps		xmm2, xmm6;
	shufps		xmm0, xmm2, 88h;
	movups		[rcx], xmm0;
	
		movups		xmm6,[rsp]
		movups		xmm7,[rsp + 16]
		add			rsp,32
	ret;

	;pDesV->x = pSrcV->x * pSrcM->_11 + pSrcV->y * pSrcM->_12 + pSrcV->z * pSrcM->_13 + pSrcV->w * pSrcM->_14;
	;pDesV->y = pSrcV->x * pSrcM->_21 + pSrcV->y * pSrcM->_22 + pSrcV->z * pSrcM->_23 + pSrcV->w * pSrcM->_24;
	;pDesV->z = pSrcV->x * pSrcM->_31 + pSrcV->y * pSrcM->_32 + pSrcV->z * pSrcM->_33 + pSrcV->w * pSrcM->_34;
	;pDesV->w = pSrcV->x * pSrcM->_41 + pSrcV->y * pSrcM->_42 + pSrcV->z * pSrcM->_43 + pSrcV->w * pSrcM->_44;

;///////////////////////////////////////////////////////////////////////////////
;// 描述：	向量点乘
;//
;// 返回值:	float
;///////////////////////////////////////////////////////////////////////////////
float4DotProduct_x64:			;(const float4* pV1,		// 向量指针
								;const float4* pV2)		// 向量指针
	movups		xmm0, [rcx];
	movups		xmm1, [rdx];
	mulps		xmm0, xmm1;
	movhlps		xmm2, xmm0;
	addps		xmm0, xmm2;
	pshufd		xmm3, xmm0, 01h;
	addps		xmm0, xmm3;
	ret;
	;fRes = pV1->x * pV2->x + pV1->y * pV2->y + pV1->z * pV2->z + pV1->w * pV2->w;	
;--------------------------------------------------------------------------------	
;end Vector
;--------------------------------------------------------------------------------
end

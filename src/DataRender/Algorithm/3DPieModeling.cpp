#include "StdAfx.h"
#include "DataRender\3DPieModeling.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
#define _USE_MATH_DEFINES
#include "math.h"

CBasePieModeling::CBasePieModeling(void)
{
}

CBasePieModeling::~CBasePieModeling(void)
{
}

CBasePieModeling::CBasePieModeling( UINT uShape,
								UINT uFrag,
								float fAngle0,
								float fAngle1,
								float fRadius,
								float fInnerRadius,
								float fThick,
								UINT  uChamferType,
								float fChamferLen, 
								float fChamferHeight,
								UINT uChamferPrecission,
								UINT uChamferLayout)
: m_shape(uShape)
, m_frag(uFrag)
, m_angle_start(fAngle0)
, m_angle_end(fAngle1)
, m_outer_radius(fRadius)
, m_inner_radius(fInnerRadius)
, m_thick(fThick)
, m_chamfer_type(uChamferType)
, m_chamfer_lenth(fChamferLen)
, m_chamfer_height(fChamferHeight)
, m_chamfer_precission(uChamferPrecission) 
, m_chamfer_layout(uChamferLayout)
, m_vertrics_count(0)
, m_indics_count(0)
, m_triangle_count(0)
, m_inner_arc_rad(fInnerRadius)
, m_inner_top_plane_rad(fInnerRadius)
, m_inner_bottom_plane_rad(fInnerRadius)
, m_outer_arc_rad(fRadius)
, m_outer_top_plane_rad(fRadius)
, m_outer_bottom_plane_rad(fRadius)
, m_has_outer_arc(TRUE)
, m_has_inner_arc(TRUE)
, m_has_top_plane(TRUE)
, m_has_bottom_plane(TRUE)
, m_has_top_holl(FALSE)
, m_has_bottom_holl(FALSE)
, m_has_top_in_chamfer(FALSE)
, m_has_bottom_in_chamfer(FALSE)
, m_has_top_out_chamfer(FALSE)
, m_has_bottom_out_chamfer(FALSE)
{
	m_angle_end += 0.0015f;
	m_frag = min(10,m_frag);

	if(m_inner_radius < ABS_FLOAT_0)
		m_inner_radius = 0;

//	if(SHAPE_PIE == m_shape)
//	{
// 		m_has_inner_arc = FALSE;
// 		m_has_top_holl = FALSE;
// 		m_has_bottom_holl = FALSE;
// 		m_inner_arc_rad = 0;
// 		m_inner_bottom_plane_rad = 0;
// 		m_inner_top_plane_rad = 0;
// 		m_inner_radius = 0;
//	}
//	
//	if(SHAPE_PIE_RING == m_shape)
	{
		m_chamfer_layout = m_chamfer_layout|CHAMFER_LAYOUT_TOP|CHAMFER_LAYOUT_BOTTOM;

		if(m_inner_radius < ABS_FLOAT_0)
		{
			m_has_inner_arc = FALSE;
			m_has_top_holl = FALSE;
			m_has_bottom_holl = FALSE;

//			 if(m_chamfer_layout&CHAMFER_LAYOUT_IN)
// 			{
// 				if(m_chamfer_layout|CHAMFER_LAYOUT_TOP)
// 					m_has_top_holl = TRUE;
// 
// 				if(m_chamfer_layout|CHAMFER_LAYOUT_BOTTOM)
// 					m_has_bottom_holl = TRUE;
// 			}
		}
		else 
		{
			m_has_inner_arc = TRUE;
			m_has_top_holl = TRUE;
			m_has_bottom_holl = TRUE;
		}
	}

	if(abs(m_angle_start - m_angle_end)<ABS_FLOAT_0)
	{
		m_has_bottom_plane = FALSE; 
		m_has_top_plane = FALSE; 
		m_has_inner_arc  = FALSE; 
		m_has_outer_arc  = FALSE; 
	}

	if((m_chamfer_layout & CHAMFER_LAYOUT_TOP) 
		&& (m_chamfer_layout & CHAMFER_LAYOUT_BOTTOM))
	{
		float max_l = (m_thick)/2.0f;

		if(abs(m_chamfer_height - max_l)<ABS_FLOAT_0)
		{
			if(m_chamfer_layout|CHAMFER_LAYOUT_IN)
			 m_has_inner_arc  = FALSE; 

			if(m_chamfer_layout|CHAMFER_LAYOUT_BOTTOM)
			 m_has_outer_arc  = FALSE; 
		}
	}
	else if((m_chamfer_layout & CHAMFER_LAYOUT_TOP) 
		|| (m_chamfer_layout & CHAMFER_LAYOUT_BOTTOM))
	{
		if(abs(m_chamfer_height - m_thick)<ABS_FLOAT_0)
		{
			if(m_chamfer_layout|CHAMFER_LAYOUT_IN)
				m_has_inner_arc  = FALSE; 

			if(m_chamfer_layout|CHAMFER_LAYOUT_BOTTOM)
				m_has_outer_arc  = FALSE; 
		}
	}

	if(abs(m_inner_top_plane_rad - m_outer_top_plane_rad) < ABS_FLOAT_0)
		m_has_top_plane = FALSE; 

	if(abs(m_inner_bottom_plane_rad - m_outer_bottom_plane_rad) 
		< ABS_FLOAT_0)
		m_has_bottom_plane = FALSE; 

	calc_chamfer_size();
	calc_radius();
}

void CBasePieModeling::calc_plane_face(int* v_count, int* i_count, int* t_count)
{
	int v0 = 0;
	int i0 = 0;
	int t0 = 0;

	int v1 = 0;
	int i1 = 0;
	int t1 = 0;

	if(m_has_top_plane)
	{
		plane_veterics_count(&v0,&i0,&t0,m_has_top_holl);
	}

	if(m_has_bottom_plane)
	{
		plane_veterics_count(&v1,&i1,&t1,m_has_bottom_holl);
	}

	if(v_count)
		*v_count = v0 + v1;

	if(i_count)
		*i_count = i0 + i1;

	if(t_count)
		*t_count = t0 + t1;
}

void CBasePieModeling::calc_arc_face(int* v_count, int* i_count, int* t_count)
{
	int num_v = 0;
	int num_i = 0;
	int num_t = 0;

	int arc_count = 0;

	if(m_has_inner_arc) 
		++arc_count;

	if(m_has_outer_arc) 
		++arc_count;

	if(arc_count>0)
	{	
		UINT arc_point_count = calc_arc_point_count(m_angle_start,m_angle_end);

		num_v = arc_point_count*2*arc_count;
		num_t = (arc_point_count - 1)*2*arc_count; 
		num_i =  num_t*3;
	}

	if(v_count)
		*v_count = num_v;

	if(i_count)
		*i_count = num_i;

	if(t_count)
		*t_count = num_t;
}

void CBasePieModeling::calc_chamfer_face(int* v_count, int* i_count, int* t_count)
{
	int num_v = 0;
	int num_i = 0;
	int num_t = 0;

	int chamfer_count = 0;

	if(m_has_top_in_chamfer)
		++chamfer_count;

	if(m_has_bottom_in_chamfer)
		++chamfer_count;

	if(m_has_top_out_chamfer)
		++chamfer_count;

	if(m_has_bottom_out_chamfer)
		++chamfer_count;
		
	if(0 == chamfer_count)
	{
		if(v_count)
			*v_count = num_v;

		if(i_count)
			*i_count = num_i;

		if(t_count)
			*t_count = num_t;

		return;
	}

	UINT arc_point_count = calc_arc_point_count(m_angle_start,m_angle_end);

	if(m_chamfer_type == CHAMFER_PLANE)
	{
		num_v = (arc_point_count)*2*chamfer_count;
		num_t = (arc_point_count - 1)*2*chamfer_count; 
	}
	else
	{
		num_v = (arc_point_count)*(m_chamfer_precission + 2)*chamfer_count;
		num_t = (arc_point_count-1)*(m_chamfer_precission*2 + 2)*chamfer_count;
	}

	num_i = num_t*3;

	if(v_count)
		*v_count = num_v;

	if(i_count)
		*i_count = num_i;

	if(t_count)
		*t_count = num_t;
}

void CBasePieModeling::calc_start_end_face(int* v_count, int* i_count, int* t_count)
{
	int num_v = 0;
	int num_i = 0;
	int num_t = 0;

	int chamfer_count = 0;

	if(m_has_top_in_chamfer)
		++chamfer_count;

	if(m_has_bottom_in_chamfer)
		++chamfer_count;

	if(m_has_top_out_chamfer)
		++chamfer_count;

	if(m_has_bottom_out_chamfer)
		++chamfer_count;

	int face_count = 2;

	if(m_chamfer_type == CHAMFER_PLANE)
	{
		num_v = (4 + chamfer_count)*face_count;
		num_t = (2 + chamfer_count)*face_count;
	}
	else if(m_chamfer_type == CHAMFER_ARC)
	{
		num_v = (4 + chamfer_count*(m_chamfer_precission+1))*face_count;
		num_t = (2 + chamfer_count*(m_chamfer_precission+1))*face_count;
	}
	else
	{
		num_v = 4*face_count;
		num_t = 2*face_count;
	}

	num_i =  num_t*3;

	if(v_count)
		*v_count = num_v;

	if(i_count)
		*i_count = num_i;

	if(t_count)
		*t_count = num_t;
}

UINT  CBasePieModeling::calc_arc_point_count(float angle_start,float m_angle_end)
{
	UINT c = UINT(abs(m_angle_end - angle_start)/PAI*180.0f*m_frag/10 + 1);

	return max(2,c);
}

void CBasePieModeling::GetModelParam(int* v_count, int* i_count, int* t_count)
{
	int num_v = 0;
	int num_i = 0;
	int num_t = 0;

	m_vertrics_count = 0;
	m_indics_count = 0;
	m_triangle_count = 0;

	calc_plane_face(&num_v, &num_i, &num_t);

	m_vertrics_count += num_v;
	m_indics_count += num_i;
	m_triangle_count += num_t;

	calc_start_end_face(&num_v, &num_i, &num_t);

	m_vertrics_count += num_v;
	m_indics_count += num_i;
	m_triangle_count += num_t;
	
	calc_arc_face(&num_v, &num_i, &num_t);

	m_vertrics_count += num_v;
	m_indics_count += num_i;
	m_triangle_count += num_t;

	calc_chamfer_face(&num_v, &num_i, &num_t);

	m_vertrics_count += num_v;
	m_indics_count += num_i;
	m_triangle_count += num_t;

	if(v_count)
		*v_count = m_vertrics_count;

	if(i_count)
		*i_count = m_indics_count;

	if(t_count)
		*t_count = m_triangle_count;
}

int CBasePieModeling::modeling_top_plane( BaseGraphVertex3D*& v_buf, DWORD*& i_buf, int v_start)
{
	int v_count = 0;

	if(!m_has_top_plane)
		return v_count;

	UINT arc_point_count = calc_arc_point_count(m_angle_start,m_angle_end);
	float da = (m_angle_end - m_angle_start)/(arc_point_count -1);

	BaseGraphVertex3D v;
	v.nx = 0;
	v.ny = 0;
	v.nz = -1;

	v.tu = 0.5f;
	v.tv = 0.5f;

	v.binormal.y = 0; //法线不变
	v.tangent.x = 1;  //厚度会变化
	v.tangent.y = 0;  //起始厚度
	
	if(!m_has_top_holl)  
	{
		//圆心
		v.x = 0;
		v.y = 0;
		v.z = -m_thick; 
		v.binormal.x = 0;

		*v_buf++ = v;

		++v_count;

		for(UINT i = 0 ; i< arc_point_count; ++i)
		{
			float a0 = m_angle_start + i*da;

			v.x = m_angle_start;//起始角度
			v.y = a0;//终止角度

			v.binormal.x = m_outer_top_plane_rad; 

			v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
			v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f ;

			*v_buf++ = v;
			++v_count;
		}

		for(UINT i = 1 ; i< arc_point_count; ++i)
		{
			*i_buf++ = 0;

			if(da > 0)
			{
				*i_buf++ = (i + v_start + 1);
				*i_buf++ = (i + v_start);
			}
			else
			{
				*i_buf++ = (i + v_start);
				*i_buf++ = (i + v_start + 1);
			}
		}

		v_start += v_count;
	}
	else
	{
		for(UINT i = 0 ; i< arc_point_count; ++i)
		{
			float a0 = m_angle_start + i*da;

			v.x = m_angle_start;//起始角度
			v.y = a0;//终止角度
			v.z = -m_thick;

			v.binormal.x = m_inner_top_plane_rad; 

			v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
			v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f ;

			*v_buf++ = v;
			++v_count;

			v.binormal.x = m_outer_top_plane_rad; 

			v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
			v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f ;

			*v_buf++ = v;
			++v_count;
		}

		for(UINT j = 0 ; j< arc_point_count-1; ++j)
		{
			if(m_angle_end > m_angle_start)
			{
				*i_buf++ = v_start + j*2;
				*i_buf++ = v_start + j*2 + 2;
				*i_buf++ = v_start + j*2 + 1;

				*i_buf++ = v_start + j*2 + 1;
				*i_buf++ = v_start + j*2 + 2;
				*i_buf++ = v_start + j*2 + 3;
			}
			else
			{
				*i_buf++ = v_start + j*2;
				*i_buf++ = v_start + j*2 + 1;
				*i_buf++ = v_start + j*2 + 2;

				*i_buf++ = v_start + j*2 + 2;
				*i_buf++ = v_start + j*2 + 1;
				*i_buf++ = v_start + j*2 + 3;
			}
		}

		v_start += v_count;
	}

	return v_count;
}

int CBasePieModeling::modeling_bottom_plane(BaseGraphVertex3D*& v_buf, DWORD*& i_buf, int v_start)
{
	int v_count = 0;

	if(!m_has_bottom_plane)
		return v_count;

	UINT arc_point_count = calc_arc_point_count(m_angle_start,m_angle_end);

	float da = (m_angle_end - m_angle_start)/(arc_point_count -1);

	BaseGraphVertex3D v;

	v.nx = 0;
	v.ny = 0;
	v.nz = 1;

	v.tu = 0.5f;
	v.tv = 0.5f;

	v.binormal.x = 0;
	v.binormal.y = 0;

	v.tangent.x = 0; //高度不变

	if(!m_has_top_holl)  
	{
		v.x = 0;
		v.y = 0;
		v.z = 0;
		
		v.tu = 0.5f;
		v.tv = 0.5f;

		v.binormal.x = 0;
		v.binormal.y = 0;

		*v_buf++ = v;
		++v_count;

		for(UINT i = 0 ; i< arc_point_count; ++i)
		{
			float a0 = m_angle_start + i*da;

			v.x = m_angle_start;//起始角度
			v.y = a0;//终止角度

			v.binormal.x = m_outer_bottom_plane_rad; 

			v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
			v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f ;

			*v_buf++ = v;
			++v_count;
		}

		for(UINT i = 1 ; i< arc_point_count; ++i)
		{
			*i_buf++ = v_start;

			if(da < 0)
			{
				*i_buf++ = (i + v_start + 1);
				*i_buf++ = (i + v_start);
			}
			else
			{
				*i_buf++ = (i + v_start);
				*i_buf++ = (i + v_start + 1);
			}
		}
	}
	else
	{
		for(UINT i = 0 ; i< arc_point_count; ++i)
		{
			float a0 = m_angle_start + i*da;

			v.x = m_angle_start;
			v.y = a0;
			v.z = 0;

			v.binormal.x = m_inner_bottom_plane_rad; 
			v.binormal.y = 0; 

			v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
			v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f ;

			*v_buf++ = v;
			++v_count;

			v.binormal.x = m_outer_bottom_plane_rad; 
			v.binormal.y = 0;

			v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
			v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f ;

			*v_buf++ = v;
			++v_count;
		}

		for(UINT j = 0 ; j< arc_point_count-1; ++j)
		{
			if(m_angle_end < m_angle_start)
			{
				*i_buf++ = v_start + j*2;
				*i_buf++ = v_start + j*2 + 2;
				*i_buf++ = v_start + j*2 + 1;

				*i_buf++ = v_start + j*2 + 1;
				*i_buf++ = v_start + j*2 + 2;
				*i_buf++ = v_start + j*2 + 3;
			}
			else
			{
				*i_buf++ = v_start + j*2;
				*i_buf++ = v_start + j*2 + 1;
				*i_buf++ = v_start + j*2 + 2;

				*i_buf++ = v_start + j*2 + 2;
				*i_buf++ = v_start + j*2 + 1;
				*i_buf++ = v_start + j*2 + 3;
			}
		}
	}

	return v_count;
}

// int CBasePieModeling::modeling_plane_face(BaseGraphVertex3D*& v_buf, DWORD*& i_buf, int v_start)
// {
//	 int v_count = 0;
// 
//	 if(m_has_plane_face)
//	 {
//		 UINT arc_point_count = calc_arc_point_count(m_angle_start,m_angle_end);
//			
//		 float da = (m_angle_end - m_angle_start)/(arc_point_count -1);
// 
//		 BaseGraphVertex3D v;
// 
//		 v.tu = 0.5f;
//		 v.tv = 0.5f;
// 
//		 v.binormal.y = 0;
// 
//		 if(!m_has_holl)
//		 {
//			 //上面
//			 {
//				 //圆心 
// 
//				 v.x = 0;
//				 v.y = 0;
//				 v.z = -m_thick; 
// 
// 				v.tu = 0.5f;
// 				v.tv = 0.5f;
// 
//				 v.nx = 0;
//				 v.ny = 0;
//				 v.nz = -1;
// 
//				 v.binormal.x = 0;
// 
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 for(UINT i = 0 ; i< arc_point_count; ++i)
//				 {
//					 float a0 = m_angle_start + i*da;
// 
//					 v.x = m_angle_start;//起始角度
//					 v.y = a0;//终止角度
// 
// 					v.binormal.x = m_outer_plane_rad; 
// 
// 					v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
// 					v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f ;
//					 
//					 *v_buf++ = v;
//					 ++v_count;
//				 }
// 
//				 for(UINT i = 1 ; i< arc_point_count; ++i)
//				 {
//					 *i_buf++ = 0;
// 
//					 if(da > 0)
//					 {
//						 *i_buf++ = (i + v_start + 1);
//						 *i_buf++ = (i + v_start);
//					 }
//					 else
//					 {
//						 *i_buf++ = (i + v_start);
//						 *i_buf++ = (i + v_start + 1);
//					 }
//				 }
// 
//				 v_start += v_count;
//			 }
// 
//			 //下面
//			 {
//				 //圆心
//				 v.x = 0;
//				 v.y = 0;
//				 v.z = 0;
// 
//				 v.nx = 0;
//				 v.ny = 0;
//				 v.nz = 1;
// 
// 				v.tu = 0.5f;
// 				v.tv = 0.5f;
// 
//				 v.binormal.x = 0;
//				 v.binormal.y = 0;
// 
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 for(UINT i = 0 ; i< arc_point_count; ++i)
//				 {
//					 float a0 = m_angle_start + i*da;
// 
//					 v.x = m_angle_start;//起始角度
//					 v.y = a0;//终止角度
// 
// 					v.binormal.x = m_outer_plane_rad; 
// 
// 					v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
// 					v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f ;
//					 
//					 *v_buf++ = v;
//					 ++v_count;
//				 }
// 
//				 for(UINT i = 1 ; i< arc_point_count; ++i)
//				 {
//					 *i_buf++ = v_start;
// 
//					 if(da < 0)
//					 {
//						 *i_buf++ = (i + v_start + 1);
//						 *i_buf++ = (i + v_start);
//					 }
//					 else
//					 {
//						 *i_buf++ = (i + v_start);
//						 *i_buf++ = (i + v_start + 1);
//					 }
//				 }
//			 }
//		 }
//		 else
//		 {
//			 //上面
//			 {
//				 BaseGraphVertex3D v;
// 
//				 v.nx = 0;
//				 v.ny = 0;
//				 v.nz = -1;
// 
//				 v.tu = 0.5f;
//				 v.tv = 0.5f;
// 
//				 for(UINT i = 0 ; i< arc_point_count; ++i)
//				 {
//					 float a0 = m_angle_start + i*da;
// 
//					 v.x = m_angle_start;//起始角度
//					 v.y = a0;//终止角度
//					 v.z = -m_thick;
// 
// 					v.binormal.x = m_inner_plane_rad; 
// 
// 					v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
// 					v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f ;
//					 v.binormal.y = 0; 
// 
//					 *v_buf++ = v;
//					 ++v_count;
// 
//					 v.binormal.x = m_outer_plane_rad; 
// 
// 					v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
// 					v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f ;
// 
//					 v.binormal.y = 0; 
// 
//					 *v_buf++ = v;
//					 ++v_count;
//				 }
// 
//				 for(UINT j = 0 ; j< arc_point_count-1; ++j)
//				 {
//					 if(m_angle_end > m_angle_start)
//					 {
//						 *i_buf++ = v_start + j*2;
//						 *i_buf++ = v_start + j*2 + 2;
//						 *i_buf++ = v_start + j*2 + 1;
// 
//						 *i_buf++ = v_start + j*2 + 1;
//						 *i_buf++ = v_start + j*2 + 2;
//						 *i_buf++ = v_start + j*2 + 3;
//					 }
//					 else
//					 {
//						 *i_buf++ = v_start + j*2;
//						 *i_buf++ = v_start + j*2 + 1;
//						 *i_buf++ = v_start + j*2 + 2;
// 
//						 *i_buf++ = v_start + j*2 + 2;
//						 *i_buf++ = v_start + j*2 + 1;
//						 *i_buf++ = v_start + j*2 + 3;
//					 }
//				 }
// 
//				 v_start += v_count;
//			 }
// 
//			 //下面
//			 {
//				 //圆心
//				 BaseGraphVertex3D v;
// 
//				 v.nx = 0;
//				 v.ny = 0;
//				 v.nz = 1;
// 
//				 v.tu = 0.5f;
//				 v.tv = 0.5f;
//			
//				 for(UINT i = 0 ; i< arc_point_count; ++i)
//				 {
//					 float a0 = m_angle_start + i*da;
// 
//					 v.x = m_angle_start;
//					 v.y = a0;
//					 v.z = 0;
// 
//					 v.binormal.x = m_inner_plane_rad; 
//					 v.binormal.y = 0; 
// 
// 					v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
// 					v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f ;
// 
//					 *v_buf++ = v;
//					 ++v_count;
// 
// 				
//					 v.binormal.x = m_outer_plane_rad; 
//					 v.binormal.y = 0;
// 
// 					v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
// 					v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f ;
// 
//					 *v_buf++ = v;
//					 ++v_count;
//				 }
// 
//				 for(UINT j = 0 ; j< arc_point_count-1; ++j)
//				 {
//					 if(m_angle_end < m_angle_start)
//					 {
//						 *i_buf++ = v_start + j*2;
//						 *i_buf++ = v_start + j*2 + 2;
//						 *i_buf++ = v_start + j*2 + 1;
// 
//						 *i_buf++ = v_start + j*2 + 1;
//						 *i_buf++ = v_start + j*2 + 2;
//						 *i_buf++ = v_start + j*2 + 3;
//					 }
//					 else
//					 {
//						 *i_buf++ = v_start + j*2;
//						 *i_buf++ = v_start + j*2 + 1;
//						 *i_buf++ = v_start + j*2 + 2;
// 
//						 *i_buf++ = v_start + j*2 + 2;
//						 *i_buf++ = v_start + j*2 + 1;
//						 *i_buf++ = v_start + j*2 + 3;
//					 }
//				 }
//			 }
//		 }
//	 }
//	 return v_count;
// }
// 

int CBasePieModeling::modeling_start_face(BaseGraphVertex3D*& v_buf, DWORD*& i_buf, int v_start)
{
	int v_count = 0;

	float c = cosf(m_angle_start);
	float s = sinf(m_angle_start);

	BaseGraphVertex3D v;

	v.x = m_angle_start;
	v.y = m_angle_start;

	v.nx = s;
	v.ny = -c;
	v.nz = 0;

	if(m_angle_end < m_angle_start)
	{
		v.nx = -v.nx;
		v.ny = -v.ny;
		v.nz = -v.nz;
	}

	v.tu = 0.5f;
	v.tv = 0.5f;  

	v.binormal.y = 0;//0:使用固定法线	

	//float dx = m_chamfer_lenth	/ m_chamfer_precission;
	//float dx = m_chamfer_lenth	/ (m_chamfer_precission + 1);

	float dt = M_PI_2/ (m_chamfer_precission + 1);

	if(m_has_top_in_chamfer && m_has_top_holl)
	{
		v.binormal.x = m_inner_arc_rad; //半径
		v.z = -m_thick + m_chamfer_height;

		v.tangent.x = 1;
		v.tangent.y = m_chamfer_height;

		v.tv = (v.z + m_thick)/m_thick;
		v.tu = (v.binormal.x)/m_outer_radius;
		
		*v_buf++ = v;
		++v_count;
		
		if(m_chamfer_type == CHAMFER_ARC)
		{
			for(UINT i = 0 ; i<m_chamfer_precission;i++)
			{
				float x = m_chamfer_lenth*cos(dt*(i+1));

				float y = sqrt(max(0,(1 - x*x/(m_chamfer_lenth*m_chamfer_lenth)))) * m_chamfer_height;

				 float h = (m_chamfer_height - y);

				 v.z = -m_thick + h;
				 v.tangent.y = h;

				 v.binormal.x = m_inner_arc_rad + m_chamfer_lenth - x; 

				 v.tv = (v.z + m_thick)/m_thick;
				 v.tu = (v.binormal.x)/m_outer_radius;

				 *v_buf++ = v;
				 ++v_count;
			 }
		}

		v.binormal.x = m_inner_top_plane_rad; //半径
		v.z = - m_thick ;
		v.tangent.y = 0;

		v.tv = (v.z + m_thick)/m_thick;
		v.tu = (v.binormal.x)/m_outer_radius;

		*v_buf++ = v;
		++v_count;
	}
	else
	{
		v.binormal.x = m_inner_arc_rad; //半径
		v.z = -m_thick;

		v.tangent.x = 1;
		v.tangent.y = 0;

		v.tv = (v.z + m_thick)/m_thick;
		v.tu = (v.binormal.x)/m_outer_radius;

		*v_buf++ = v;
		++v_count;
	}

	if(m_has_top_out_chamfer)
	{
		v.binormal.x = m_outer_top_plane_rad; //半径

		v.z = - m_thick ;

		v.tangent.x = 1;
		v.tangent.y = 0;

		v.tv = (v.z + m_thick)/m_thick;
		v.tu = (v.binormal.x)/m_outer_radius;

		*v_buf++ = v;
		++v_count;
			
		if(m_chamfer_type == CHAMFER_ARC)
		{
			for(UINT i = 0 ; i< m_chamfer_precission;i++)
			{
				float x = m_chamfer_lenth*cos(M_PI_2 - dt*(i+1));
				float y = sqrt(max(0,(1 - x*x/(m_chamfer_lenth*m_chamfer_lenth)))) * m_chamfer_height;
				float h = (m_chamfer_height - y);

				v.z = -m_thick + h;
				v.tangent.x = 1;
				v.tangent.y = h;

				v.tv = (v.z + m_thick)/m_thick;
				v.tu = (v.binormal.x)/m_outer_radius;

				v.binormal.x = m_outer_top_plane_rad + x;
				*v_buf++ = v;
				++v_count;
			}
		}

		v.binormal.x = m_outer_arc_rad; //半径

		v.z = - m_thick  + m_chamfer_height ;
		v.tangent.x = 1;
		v.tangent.y = m_chamfer_height;

		v.tv = (v.z + m_thick)/m_thick;
		v.tu = (v.binormal.x)/m_outer_radius;

		*v_buf++ = v;
		++v_count;
	}
	else
	{
		v.binormal.x = m_outer_arc_rad; //半径
		v.z = -m_thick;

		v.tangent.x = 1;
		v.tangent.y = 0;

		v.tv = (v.z + m_thick)/m_thick;
		v.tu = (v.binormal.x)/m_outer_radius;

		*v_buf++ = v;
		++v_count;
	}

	if(m_has_bottom_out_chamfer )
	{
		v.binormal.x = m_outer_arc_rad; //半径
		v.z = -m_chamfer_height ;

		v.tangent.x = 0;
		v.tangent.y = 0;

		v.tv = (v.z + m_thick)/m_thick;
		v.tu = (v.binormal.x)/m_outer_radius;

		*v_buf++ = v;
		++v_count;

		if(m_chamfer_type == CHAMFER_ARC)
		{
			for(UINT i = 0 ; i< m_chamfer_precission;i++)
			{
				float x = m_chamfer_lenth*cos(dt*(i+1));

				float y = sqrt(max(0,(1 - x*x/(m_chamfer_lenth*m_chamfer_lenth)))) * m_chamfer_height;

				float h = (m_chamfer_height - y);

				v.z = -h;
				v.tangent.x = 0;

				v.binormal.x = m_outer_bottom_plane_rad + x;

				v.tv = (v.z + m_thick)/m_thick;
				v.tu = (v.binormal.x)/m_outer_radius;

				*v_buf++ = v;
				++v_count;
			} 
		}

		v.binormal.x = m_outer_bottom_plane_rad; //半径
		v.z = 0 ;
		v.tangent.x = 0;

		v.tv = (v.z + m_thick)/m_thick;
		v.tu = (v.binormal.x)/m_outer_radius;

		*v_buf++ = v;
		++v_count;
	}
	else
	{
		v.binormal.x = m_outer_arc_rad; //半径
		v.z = 0;
		v.tangent.x = 0;  

		v.tv = (v.z + m_thick)/m_thick;
		v.tu = (v.binormal.x)/m_outer_radius;

		*v_buf++ = v;
		++v_count;
	}

	if(m_has_bottom_in_chamfer && m_has_bottom_holl)
	{
		v.binormal.x = m_inner_bottom_plane_rad; //半径
		v.z = 0 ;
		v.tangent.x = 0;  

		*v_buf++ = v;
		++v_count;

		if(m_chamfer_type == CHAMFER_ARC)
		{
			for(UINT i = 0 ; i< m_chamfer_precission;i++)
			{
				float x = m_chamfer_lenth*cos(M_PI_2 - dt*(i+1));
				float y = sqrt(max(0,(1 - x*x/(m_chamfer_lenth*m_chamfer_lenth)))) * m_chamfer_height;
				float h = (m_chamfer_height - y);

				v.z = -h;
				v.tangent.x = 0;

				v.binormal.x = m_inner_bottom_plane_rad - x; 

				v.tv = (v.z + m_thick)/m_thick;
				v.tu = (v.binormal.x)/m_outer_radius;

				*v_buf++ = v;
				++v_count;

			} 
		}

		v.binormal.x = m_inner_arc_rad; //半径
		v.z = -m_chamfer_height ;
		v.tangent.x = 0;  

		v.tv = (v.z + m_thick)/m_thick;
		v.tu = (v.binormal.x)/m_outer_radius;

		*v_buf++ = v;
		++v_count;
	}
	else
	{
		v.binormal.x = m_inner_arc_rad; //半径
		v.z = 0;
		v.tangent.x = 0;  

		v.tv = (v.z + m_thick)/m_thick;
		v.tu = (v.binormal.x)/m_outer_radius;

		*v_buf++ = v;
		++v_count;
	}

	if(m_angle_end >= m_angle_start)
	{
		for(int i = 1;i <v_count-1 ;++i)
		{
			*i_buf++ = v_start;
			*i_buf++ = v_start + i ;
			*i_buf++ = v_start + i+ 1;
		}
	}
	else
	{
		for(int i = 1;i <v_count-1 ;++i)
		{
			*i_buf++ = v_start;
			*i_buf++ = v_start + i + 1;
			*i_buf++ = v_start + i;
		}
	}

	return v_count;
}

// int CBasePieModeling::modeling_start_face(BaseGraphVertex3D*& v_buf, DWORD*& i_buf, int v_start)
// {
//	 int v_count = 0;
// 
//	 float c = cosf(m_angle_start);
//	 float s = sinf(m_angle_start);
// 
//	 BaseGraphVertex3D v;
// 
//	 v.x = m_angle_start;
//	 v.y = m_angle_start;
// 
//	 v.nx = s;
//	 v.ny = -c;
//	 v.nz = 0;
// 
//	 if(m_angle_end < m_angle_start)
//	 {
//		 v.nx = -v.nx;
//		 v.ny = -v.ny;
//		 v.nz = -v.nz;
//	 }
// 
//	 v.tu = 0.5f;
//	 v.tv = 0.5f;  
// 
//	 v.binormal.y = 0;//0:使用固定法线
// 
// 
//	 if(!m_has_chamfer)
//	 {
//		 v.binormal.x = m_inner_arc_rad; //半径
//		 v.z = -m_thick;
//		 *v_buf++ = v;
//		 ++v_count;
// 
//		 v.binormal.x = m_inner_arc_rad; //半径
//		 v.z = 0;
//		 *v_buf++ = v;
//		 ++v_count;
// 
//		 v.binormal.x = m_outer_arc_rad; //半径
//		 v.z = -m_thick;
//		 *v_buf++ = v;
//		 ++v_count;
// 
//		 v.binormal.x = m_outer_arc_rad; //半径
//		 v.z = 0;
//		 *v_buf++ = v;
//		 ++v_count;
//		
// 
//		 if(m_angle_end > m_angle_start)
//		 {
//			 *i_buf++ = v_start;
//			 *i_buf++ = v_start + 2;
//			 *i_buf++ = v_start + 1;
// 
//			 *i_buf++ = v_start + 1;
//			 *i_buf++ = v_start + 2;
//			 *i_buf++ = v_start + 3;
//		 }
//		 else
//		 {
//			 *i_buf++ = v_start;
//			 *i_buf++ = v_start + 1;
//			 *i_buf++ = v_start + 2;
// 
//			 *i_buf++ = v_start + 2;
//			 *i_buf++ = v_start + 1;
//			 *i_buf++ = v_start + 3;
//		 }
//	 }
//	 else if(m_chamfer_type == CHAMFER_PLANE)
//	 {
//		 
//		 switch(m_shape)
//		 {
//		 case SHAPE_PIE:
//			 {
//				 v.z = -m_thick;
//				 v.binormal.x = 0; 
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = 0;
//				 v.binormal.x = 0; 
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = -m_thick;
//				 v.binormal.x = m_outer_plane_rad; 
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = 0;
//				 v.binormal.x = m_outer_plane_rad; 
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = -m_thick + m_chamfer_height;
//				 v.binormal.x = m_outer_arc_rad; 
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = -m_chamfer_height;
//				 v.binormal.x = m_outer_arc_rad; 
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 if(m_angle_end > m_angle_start)
//				 {
//					 *i_buf++ = v_start;
//					 *i_buf++ = v_start + 2;
//					 *i_buf++ = v_start + 1;
// 
//					 *i_buf++ = v_start + 1;
//					 *i_buf++ = v_start + 2;
//					 *i_buf++ = v_start + 3;
// 
//					 *i_buf++ = v_start + 2;
//					 *i_buf++ = v_start + 4;
//					 *i_buf++ = v_start + 3;
// 
//					 *i_buf++ = v_start + 3;
//					 *i_buf++ = v_start + 4;
//					 *i_buf++ = v_start + 5;
//				 }
//				 else
//				 {
//					 *i_buf++ = v_start;
//					 *i_buf++ = v_start + 1;
//					 *i_buf++ = v_start + 2;
// 
//					 *i_buf++ = v_start + 2;
//					 *i_buf++ = v_start + 1;
//					 *i_buf++ = v_start + 3;
// 
//					 *i_buf++ = v_start + 2;
//					 *i_buf++ = v_start + 3;
//					 *i_buf++ = v_start + 4;
// 
//					 *i_buf++ = v_start + 4;
//					 *i_buf++ = v_start + 3;
//					 *i_buf++ = v_start + 5;
//				 }
//				 
//				 break;
//			 }
//		 case SHAPE_PIE_RING:
//			 {
//				 v.z = -m_thick + m_chamfer_height;
//				 v.binormal.x = m_inner_arc_rad; 
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = -m_chamfer_height;
//				 v.binormal.x = m_inner_arc_rad; 
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = -m_thick ;
//				 v.binormal.x = m_inner_plane_rad; 
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = 0 ;
//				 v.binormal.x = m_inner_plane_rad; 
//				 *v_buf++ = v;
//				 ++v_count;
// 
// 
//				 v.z = -m_thick ;
//				 v.binormal.x = m_outer_plane_rad; 
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = 0 ;
//				 v.binormal.x = m_outer_plane_rad; 
//				 *v_buf++ = v;
//				 ++v_count;
// 
// 
//				 v.z = -m_thick + m_chamfer_height;
//				 v.binormal.x = m_outer_arc_rad; 
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = -m_chamfer_height;
//				 v.binormal.x = m_outer_arc_rad; 
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 if(m_angle_end > m_angle_start)
//				 {
//					 *i_buf++ = v_start;
//					 *i_buf++ = v_start + 2;
//					 *i_buf++ = v_start + 1;
// 
//					 *i_buf++ = v_start + 1;
//					 *i_buf++ = v_start + 2;
//					 *i_buf++ = v_start + 3;
// 
//					 *i_buf++ = v_start + 2;
//					 *i_buf++ = v_start + 4;
//					 *i_buf++ = v_start + 3;
// 
//					 *i_buf++ = v_start + 3;
//					 *i_buf++ = v_start + 4;
//					 *i_buf++ = v_start + 5;
// 
//					 *i_buf++ = v_start + 4;
//					 *i_buf++ = v_start + 6;
//					 *i_buf++ = v_start + 5;
// 
//					 *i_buf++ = v_start + 5;
//					 *i_buf++ = v_start + 6;
//					 *i_buf++ = v_start + 7;
//				 }
//				 else
//				 {
//					 *i_buf++ = v_start;
//					 *i_buf++ = v_start + 1;
//					 *i_buf++ = v_start + 2;
// 
//					 *i_buf++ = v_start + 2;
//					 *i_buf++ = v_start + 1;
//					 *i_buf++ = v_start + 3;
// 
//					 *i_buf++ = v_start + 2;
//					 *i_buf++ = v_start + 3;
//					 *i_buf++ = v_start + 4;
// 
//					 *i_buf++ = v_start + 4;
//					 *i_buf++ = v_start + 3;
//					 *i_buf++ = v_start + 5;
// 
//					 *i_buf++ = v_start + 4;
//					 *i_buf++ = v_start + 5;
//					 *i_buf++ = v_start + 6;
// 
//					 *i_buf++ = v_start + 6;
//					 *i_buf++ = v_start + 5;
//					 *i_buf++ = v_start + 7;
//				 }
// 
//				 break;
//			 }
//		 }
//	 }
//	 else
//	 {
//		 switch(m_shape)
//		 {
//		 case SHAPE_PIE:
//			 {
//				 v.z = -m_thick;
//				 v.binormal.x = 0; 
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = 0;
//				 v.binormal.x = 0; 
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = -m_thick;
//				 v.binormal.x = m_outer_plane_rad; 
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = 0;
//				 v.binormal.x = m_outer_plane_rad; 
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 float dx = (m_outer_arc_rad - m_outer_plane_rad)/m_chamfer_precission;
// 
//				 for(int i = 0 ; i< m_chamfer_precission;i++)
//				 {
//					 float x = dx*(i+1);
//					 float y = sqrt(max(0,m_chamfer_lenth*m_chamfer_lenth - x*x)); 
//					 float h = (m_chamfer_lenth - y);
//					 
//					 v.z = -m_thick + h;
//					 v.binormal.x = m_outer_plane_rad + x; 
//					 *v_buf++ = v;
//					 ++v_count;
// 
//					 v.z = - h;
//					 *v_buf++ = v;
//					 ++v_count;
//				 }
// 
//				 for(int j = 0 ; j< m_chamfer_precission + 1 ;j++)
//				 {
// 
//					 if(m_angle_end > m_angle_start)
//					 {
//						 *i_buf++ = v_start + j*2;
//						 *i_buf++ = v_start + j*2 + 2;
//						 *i_buf++ = v_start + j*2 + 1;
// 
//						 *i_buf++ = v_start + j*2 + 1;
//						 *i_buf++ = v_start + j*2 + 2;
//						 *i_buf++ = v_start + j*2 + 3;
//					 }
//					 else
//					 {
//						 *i_buf++ = v_start + j*2;
//						 *i_buf++ = v_start + j*2 + 1;
//						 *i_buf++ = v_start + j*2 + 2;
// 
//						 *i_buf++ = v_start + j*2 + 2;
//						 *i_buf++ = v_start + j*2 + 1;
//						 *i_buf++ = v_start + j*2 + 3;
//					 }
//				 }
//			  
//				 break;
//			 }
//		 case SHAPE_PIE_RING:
//			 {
//				 float dx = (m_inner_plane_rad - m_inner_arc_rad)/m_chamfer_precission;
// 
//				 for(int i = 0 ; i< m_chamfer_precission;i++)
//				 {
//					 float x = dx*(m_chamfer_precission - i);
//					 float y = sqrt(max(0,m_chamfer_lenth*m_chamfer_lenth - x*x));
//					 float h = (m_chamfer_lenth - y);
// 
//					 v.z = -m_thick + h;
//					 v.binormal.x = m_inner_plane_rad - x; 
//					 *v_buf++ = v;
//					 ++v_count;
// 
//					 v.z = - h;
//					 *v_buf++ = v;
//					 ++v_count;
//				 }
// 
//				 v.z = -m_thick;
//				 v.binormal.x = m_inner_plane_rad; 
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = 0;
//				 v.binormal.x = m_inner_plane_rad; 
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = -m_thick;
//				 v.binormal.x = m_outer_plane_rad; 
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = 0;
//				 v.binormal.x = m_outer_plane_rad; 
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 dx = (m_outer_arc_rad - m_outer_plane_rad)/m_chamfer_precission;
// 
//				 for(int i = 0 ; i< m_chamfer_precission;i++)
//				 {
//					 float x = dx*(i+1);
//					 float y = sqrt(max(0,m_chamfer_lenth*m_chamfer_lenth - x*x));
//					 float h = (m_chamfer_lenth - y);
// 
//					 v.z = -m_thick + h;
//					 v.binormal.x = m_outer_plane_rad + x; 
//					 *v_buf++ = v;
//					 ++v_count;
// 
//					 v.z = - h;
//					 *v_buf++ = v;
//					 ++v_count;
//				 }
// 
//				 for(int j = 0 ; j< m_chamfer_precission*2 + 1 ;j++)
//				 {
// 
//					 if(m_angle_end > m_angle_start)
//					 {
//						 *i_buf++ = v_start + j*2;
//						 *i_buf++ = v_start + j*2 + 2;
//						 *i_buf++ = v_start + j*2 + 1;
// 
//						 *i_buf++ = v_start + j*2 + 1;
//						 *i_buf++ = v_start + j*2 + 2;
//						 *i_buf++ = v_start + j*2 + 3;
//					 }
//					 else
//					 {
//						 *i_buf++ = v_start + j*2;
//						 *i_buf++ = v_start + j*2 + 1;
//						 *i_buf++ = v_start + j*2 + 2;
// 
//						 *i_buf++ = v_start + j*2 + 2;
//						 *i_buf++ = v_start + j*2 + 1;
//						 *i_buf++ = v_start + j*2 + 3;
//					 }
//				 }
//				 break;
//			 }
//		 }
//	 }
// 
//	 return v_count;
// }

int CBasePieModeling::modeling_end_face(BaseGraphVertex3D*& v_buf, DWORD*& i_buf, int v_start)
{
	int v_count = 0;

	float c = cosf(m_angle_start);
	float s = sinf(m_angle_start);

	BaseGraphVertex3D v;

	v.x = m_angle_start;
	v.y = m_angle_end;

	v.nx = 0;
	v.ny = 1;
	v.nz = 0;

	if(m_angle_end < m_angle_start)
	{
		v.nx = -v.nx;
		v.ny = -v.ny;
		v.nz = -v.nz;
	}

	v.tu = 0.5f;
	v.tv = 0.5f;  

	v.binormal.y = 6;//6:变换法线

	//float dx = m_chamfer_lenth	/ m_chamfer_precission;
	//float dx = m_chamfer_lenth	/ (m_chamfer_precission + 1);

	float dt = M_PI_2/ (m_chamfer_precission + 1);

	if(m_has_top_in_chamfer && m_has_top_holl)
	{
		v.binormal.x = m_inner_arc_rad; //半径
		v.z = -m_thick + m_chamfer_height;

		v.tangent.x = 1;
		v.tangent.y = m_chamfer_height;

		v.tv = (v.z + m_thick)/m_thick;
		v.tu = (v.binormal.x)/m_outer_radius;

		*v_buf++ = v;
		++v_count;

		if(m_chamfer_type == CHAMFER_ARC)
		{
			for(UINT i = 0 ; i<m_chamfer_precission;i++)
			{
				float x = m_chamfer_lenth*cos(dt*(i+1));

				float y = sqrt(max(0,(1 - x*x
					/(m_chamfer_lenth*m_chamfer_lenth))))
					*m_chamfer_height;

				float h = (m_chamfer_height - y);

				v.z = -m_thick + h;
				v.tangent.x = 1;
				v.tangent.y = h;
				

				v.binormal.x = m_inner_arc_rad  + m_chamfer_lenth - x; 

				v.tv = (v.z + m_thick)/m_thick;
				v.tu = (v.binormal.x)/m_outer_radius;
				v.tv = (v.z + m_thick)/m_thick;
				v.tu = (v.binormal.x)/m_outer_radius;

				*v_buf++ = v;
				++v_count;
			}
		}

		v.binormal.x = m_inner_top_plane_rad; //半径
		v.z = - m_thick ;
		v.tangent.x = 1;
		v.tangent.y = 0;

		v.tv = (v.z + m_thick)/m_thick;
		v.tu = (v.binormal.x)/m_outer_radius;

		*v_buf++ = v;
		++v_count;
	}
	else
	{
		v.binormal.x = m_inner_arc_rad; //半径
		v.z = -m_thick;
		v.tangent.x = 1;
		v.tangent.y = 0;

		v.tv = (v.z + m_thick)/m_thick;
		v.tu = (v.binormal.x)/m_outer_radius;

		*v_buf++ = v;
		++v_count;
	}

	if(m_has_top_out_chamfer)
	{
		v.binormal.x = m_outer_top_plane_rad; //半径
		v.z = - m_thick ;
		v.tangent.x = 1;
		v.tangent.y = 0;

		v.tv = (v.z + m_thick)/m_thick;
		v.tu = (v.binormal.x)/m_outer_radius;

		*v_buf++ = v;
		++v_count;

		if(m_chamfer_type == CHAMFER_ARC)
		{
			for(UINT i = 0 ; i< m_chamfer_precission;i++)
			{
				float x = m_chamfer_lenth*cos(M_PI_2 - dt*(i+1));

				float y = sqrt(max(0,(1 - x*x
					/(m_chamfer_lenth*m_chamfer_lenth))))
					*m_chamfer_height;

				float h = (m_chamfer_height - y);

				v.z = -m_thick + h;
				v.tangent.x = 1;
				v.tangent.y = h;

				v.binormal.x = m_outer_top_plane_rad + x; 

				v.tv = (v.z + m_thick)/m_thick;
				v.tu = (v.binormal.x)/m_outer_radius;
				v.tv = (v.z + m_thick)/m_thick;
				v.tu = (v.binormal.x)/m_outer_radius;

				*v_buf++ = v;
				++v_count;
			}
		}

		v.binormal.x = m_outer_arc_rad; //半径
		v.z = - m_thick  + m_chamfer_height ;
		v.tangent.x = 1;
		v.tangent.y = m_chamfer_height;

		v.tv = (v.z + m_thick)/m_thick;
		v.tu = (v.binormal.x)/m_outer_radius;

		*v_buf++ = v;
		++v_count;
	}
	else
	{
		v.binormal.x = m_outer_arc_rad; //半径
		v.z = -m_thick;

		v.tv = (v.z + m_thick)/m_thick;
		v.tu = (v.binormal.x)/m_outer_radius;

		*v_buf++ = v;
		++v_count;
	}

	if(m_has_bottom_out_chamfer )
	{
		v.binormal.x = m_outer_arc_rad; //半径
		v.z = -m_chamfer_height ;
		v.tangent.x = 0;

		v.tv = (v.z + m_thick)/m_thick;
		v.tu = (v.binormal.x)/m_outer_radius;

		*v_buf++ = v;
		++v_count;

		if(m_chamfer_type == CHAMFER_ARC)
		{
			for(UINT i = 0 ; i< m_chamfer_precission;i++)
			{
				float x = m_chamfer_lenth*cos(dt*(i+1));

				float y = sqrt(max(0,(1 - x*x
					/(m_chamfer_lenth*m_chamfer_lenth))))
					*m_chamfer_height;

				float h = (m_chamfer_height - y);
				v.z = -h;
				v.tangent.x = 0;  
				v.binormal.x =  m_outer_bottom_plane_rad + x; 

				v.tv = (v.z + m_thick)/m_thick;
				v.tu = (v.binormal.x)/m_outer_radius;
				v.tv = (v.z + m_thick)/m_thick;
				v.tu = (v.binormal.x)/m_outer_radius;

				*v_buf++ = v;
				++v_count;
			} 
		}

		v.binormal.x = m_outer_bottom_plane_rad; //半径
		v.z = 0 ;
		v.tangent.x = 0; 

		v.tv = (v.z + m_thick)/m_thick;
		v.tu = (v.binormal.x)/m_outer_radius;

		*v_buf++ = v;
		++v_count;
	}
	else
	{
		v.binormal.x = m_outer_arc_rad; //半径
		v.z = 0;
		v.tangent.x = 0;

		v.tv = (v.z + m_thick)/m_thick;
		v.tu = (v.binormal.x)/m_outer_radius;

		*v_buf++ = v;
		++v_count;
	}

	if(m_has_bottom_in_chamfer && m_has_bottom_holl)
	{
		v.binormal.x = m_inner_bottom_plane_rad; //半径
		v.z = 0 ;
		v.tangent.x = 0; 

		v.tv = (v.z + m_thick)/m_thick;
		v.tu = (v.binormal.x)/m_outer_radius;

		*v_buf++ = v;
		++v_count;

		if(m_chamfer_type == CHAMFER_ARC)
		{
			for(UINT i = 0 ; i< m_chamfer_precission;i++)
			{
				float x = m_chamfer_lenth*cos(M_PI_2 - dt*(i+1));
				float y = sqrt(max(0,(1 - x*x/(m_chamfer_lenth*m_chamfer_lenth)))) * m_chamfer_height;
				float h = (m_chamfer_height - y);

				v.z = -h;
				v.tangent.x = 0; 

				v.binormal.x = m_inner_bottom_plane_rad - x;

				v.tv = (v.z + m_thick)/m_thick;
				v.tu = (v.binormal.x)/m_outer_radius;

				*v_buf++ = v;
				++v_count;
			} 
		}

		v.binormal.x = m_inner_arc_rad; //半径
		v.z = -m_chamfer_height ;
		v.tangent.x = 0; 

		v.tv = (v.z + m_thick)/m_thick;
		v.tu = (v.binormal.x)/m_outer_radius;

		*v_buf++ = v;
		++v_count;
	}
	else
	{
		v.binormal.x = m_inner_arc_rad; //半径
		v.z = 0;
		v.tangent.x = 0; 

		v.tv = (v.z + m_thick)/m_thick;
		v.tu = (v.binormal.x)/m_outer_radius;

		*v_buf++ = v;
		++v_count;
	}

	if(m_angle_end >= m_angle_start)
	{
		for(int i = 1;i <v_count-1 ;++i)
		{
			*i_buf++ = v_start;
			*i_buf++ = v_start + i + 1;
			*i_buf++ = v_start + i;
		}
	}
	else
	{
		for(int i = 1;i <v_count-1 ;++i)
		{
			*i_buf++ = v_start;
			*i_buf++ = v_start + i ;
			*i_buf++ = v_start + i+ 1;
		}
	}

	return v_count;
}

// int CBasePieModeling::modeling_end_face(BaseGraphVertex3D*& v_buf, DWORD*& i_buf, int v_start)
// {
//	 int v_count = 0;
// 
//	 float c = cosf(m_angle_start);
//	 float s = sinf(m_angle_start);
// 
//	 BaseGraphVertex3D v;
// 
//	 v.x = m_angle_start;
//	 v.y = m_angle_end;
// 
//	 v.nx = 0;
//	 v.ny = 1;
//	 v.nz = 0;
// 
//	 if(m_angle_end < m_angle_start)
//	 {
//		 v.nx = -v.nx;
//		 v.ny = -v.ny;
//		 v.nz = -v.nz;
//	 }
// 
//	 v.tu = 0.5f;
//	 v.tv = 0.5f;  
// 
//	 v.binormal.y = 6;//6:变换法线
// 
//	 if(!m_has_chamfer)
//	 {
//		 v.binormal.x = m_inner_arc_rad; //半径
//		 v.z = -m_thick;
// 
// 		v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 		v.tv = 0.0f;
// 
//		 *v_buf++ = v;
//		 ++v_count;
// 
//		 v.binormal.x = m_inner_arc_rad; //半径
//		 v.z = 0;
// 
// 		v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 		v.tv = 1.0f;
// 
//		 *v_buf++ = v;
//		 ++v_count;
// 
//		 v.binormal.x = m_outer_arc_rad; //半径
//		 v.z = -m_thick;
// 
// 		v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 		v.tv = 0.0f;
// 
//		 *v_buf++ = v;
//		 ++v_count;
// 
//		 v.binormal.x = m_outer_arc_rad; //半径
//		 v.z = 0;
// 
// 		v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 		v.tv = 1.0f;
// 
//		 *v_buf++ = v;
//		 ++v_count;
// 
//		 if(m_angle_end < m_angle_start)
//		 {
//			 *i_buf++ = v_start;
//			 *i_buf++ = v_start + 2;
//			 *i_buf++ = v_start + 1;
// 
//			 *i_buf++ = v_start + 1;
//			 *i_buf++ = v_start + 2;
//			 *i_buf++ = v_start + 3;
//		 }
//		 else
//		 {
//			 *i_buf++ = v_start;
//			 *i_buf++ = v_start + 1;
//			 *i_buf++ = v_start + 2;
// 
//			 *i_buf++ = v_start + 2;
//			 *i_buf++ = v_start + 1;
//			 *i_buf++ = v_start + 3;
//		 }
//	 }
//	 else if(m_chamfer_type == CHAMFER_PLANE)
//	 {
// 
//		 switch(m_shape)
//		 {
//		 case SHAPE_PIE:
//			 {
//				 v.z = -m_thick;
//				 v.binormal.x = 0; 
// 				v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 				v.tv = 0.0f;
// 
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = 0;
//				 v.binormal.x = 0; 
// 				v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 				v.tv = 1.0f;
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = -m_thick;
//				 v.binormal.x = m_outer_plane_rad; 
// 				v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 				v.tv = 0.0f;
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = 0;
//				 v.binormal.x = m_outer_plane_rad; 
// 				v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 				v.tv = 1.0f;
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = -m_thick + m_chamfer_height;
//				 v.binormal.x = m_outer_arc_rad; 
// 				v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 				v.tv = 0.0f;
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = -m_chamfer_height;
// 				v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 				v.tv = 1.0f;
//				 v.binormal.x = m_outer_arc_rad; 
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 if(m_angle_end < m_angle_start)
//				 {
//					 *i_buf++ = v_start;
//					 *i_buf++ = v_start + 2;
//					 *i_buf++ = v_start + 1;
// 
//					 *i_buf++ = v_start + 1;
//					 *i_buf++ = v_start + 2;
//					 *i_buf++ = v_start + 3;
// 
//					 *i_buf++ = v_start + 2;
//					 *i_buf++ = v_start + 4;
//					 *i_buf++ = v_start + 3;
// 
//					 *i_buf++ = v_start + 3;
//					 *i_buf++ = v_start + 4;
//					 *i_buf++ = v_start + 5;
//				 }
//				 else
//				 {
//					 *i_buf++ = v_start;
//					 *i_buf++ = v_start + 1;
//					 *i_buf++ = v_start + 2;
// 
//					 *i_buf++ = v_start + 2;
//					 *i_buf++ = v_start + 1;
//					 *i_buf++ = v_start + 3;
// 
//					 *i_buf++ = v_start + 2;
//					 *i_buf++ = v_start + 3;
//					 *i_buf++ = v_start + 4;
// 
//					 *i_buf++ = v_start + 4;
//					 *i_buf++ = v_start + 3;
//					 *i_buf++ = v_start + 5;
//				 }
// 
//				 break;
//			 }
//		 case SHAPE_PIE_RING:
//			 {
//				 v.z = -m_thick + m_chamfer_height;
//				 v.binormal.x = m_inner_arc_rad; 
// 				v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 				v.tv = 0.0f;
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = -m_chamfer_height;
//				 v.binormal.x = m_inner_arc_rad; 
// 				v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 				v.tv = 1.0f;
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = -m_thick ;
//				 v.binormal.x = m_inner_plane_rad; 
// 				v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 				v.tv = 0.0f;
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = 0 ;
//				 v.binormal.x = m_inner_plane_rad; 
// 				v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 				v.tv = 1.0f;
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = -m_thick ;
//				 v.binormal.x = m_outer_plane_rad; 
// 				v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 				v.tv = 0.0f;
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = 0 ;
//				 v.binormal.x = m_outer_plane_rad; 
// 				v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 				v.tv = 1.0f;
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = -m_thick + m_chamfer_height;
//				 v.binormal.x = m_outer_arc_rad; 
// 				v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 				v.tv = 0.0f;
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = -m_chamfer_height;
//				 v.binormal.x = m_outer_arc_rad; 
// 				v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 				v.tv =1.0f;
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 if(m_angle_end < m_angle_start)
//				 {
//					 *i_buf++ = v_start;
//					 *i_buf++ = v_start + 2;
//					 *i_buf++ = v_start + 1;
// 
//					 *i_buf++ = v_start + 1;
//					 *i_buf++ = v_start + 2;
//					 *i_buf++ = v_start + 3;
// 
//					 *i_buf++ = v_start + 2;
//					 *i_buf++ = v_start + 4;
//					 *i_buf++ = v_start + 3;
// 
//					 *i_buf++ = v_start + 3;
//					 *i_buf++ = v_start + 4;
//					 *i_buf++ = v_start + 5;
// 
//					 *i_buf++ = v_start + 4;
//					 *i_buf++ = v_start + 6;
//					 *i_buf++ = v_start + 5;
// 
//					 *i_buf++ = v_start + 5;
//					 *i_buf++ = v_start + 6;
//					 *i_buf++ = v_start + 7;
//				 }
//				 else
//				 {
//					 *i_buf++ = v_start;
//					 *i_buf++ = v_start + 1;
//					 *i_buf++ = v_start + 2;
// 
//					 *i_buf++ = v_start + 2;
//					 *i_buf++ = v_start + 1;
//					 *i_buf++ = v_start + 3;
// 
//					 *i_buf++ = v_start + 2;
//					 *i_buf++ = v_start + 3;
//					 *i_buf++ = v_start + 4;
// 
//					 *i_buf++ = v_start + 4;
//					 *i_buf++ = v_start + 3;
//					 *i_buf++ = v_start + 5;
// 
//					 *i_buf++ = v_start + 4;
//					 *i_buf++ = v_start + 5;
//					 *i_buf++ = v_start + 6;
// 
//					 *i_buf++ = v_start + 6;
//					 *i_buf++ = v_start + 5;
//					 *i_buf++ = v_start + 7;
//				 }
// 
//				 break;
//			 }
//		 }
//	 }
//	 else
//	 {
//		 switch(m_shape)
//		 {
//		 case SHAPE_PIE:
//			 {
//				 v.z = -m_thick;
//				 v.binormal.x = 0; 
// 
// 				v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 				v.tv = 0.0f;
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = 0;
//				 v.binormal.x = 0; 
// 				v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 				v.tv = 1.0f;
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = -m_thick;
// 				v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 				v.tv = 0.0f;
//				 v.binormal.x = m_outer_plane_rad; 
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = 0;
//				 v.binormal.x = m_outer_plane_rad; 
// 				v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 				v.tv = 1.0f;
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 float dx = (m_outer_arc_rad - m_outer_plane_rad)/m_chamfer_precission;
// 
//				 for(int i = 0 ; i< m_chamfer_precission;i++)
//				 {
//					 float x = dx*(i+1);
//					 float y = sqrt(max(0,m_chamfer_lenth*m_chamfer_lenth - x*x));float h = (m_chamfer_lenth - y);
// 
//					 v.z = -m_thick + h;
//					 v.binormal.x = m_outer_plane_rad + x; 
// 					v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 					v.tv = 0.0f;
//					 *v_buf++ = v;
//					 ++v_count;
// 
//					 v.z = - h;
// 					v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 					v.tv = 1.0f;
//					 *v_buf++ = v;
//					 ++v_count;
//				 }
// 
//				 for(int j = 0 ; j< m_chamfer_precission + 1 ;j++)
//				 {
//					 if(m_angle_end < m_angle_start)
//					 {
//						 *i_buf++ = v_start + j*2;
//						 *i_buf++ = v_start + j*2 + 2;
//						 *i_buf++ = v_start + j*2 + 1;
// 
//						 *i_buf++ = v_start + j*2 + 1;
//						 *i_buf++ = v_start + j*2 + 2;
//						 *i_buf++ = v_start + j*2 + 3;
//					 }
//					 else
//					 {
//						 *i_buf++ = v_start + j*2;
//						 *i_buf++ = v_start + j*2 + 1;
//						 *i_buf++ = v_start + j*2 + 2;
// 
//						 *i_buf++ = v_start + j*2 + 2;
//						 *i_buf++ = v_start + j*2 + 1;
//						 *i_buf++ = v_start + j*2 + 3;
//					 }
//				 }
// 
//				 break;
//			 }
//		 case SHAPE_PIE_RING:
//			 {
//				 float dx = (m_inner_plane_rad - m_inner_arc_rad)/m_chamfer_precission;
// 
//				 for(int i = 0 ; i< m_chamfer_precission;i++)
//				 {
//					 float x = dx*(m_chamfer_precission - i);
//					 float y = sqrt(max(0,m_chamfer_lenth*m_chamfer_lenth - x*x));
//					 float h = (m_chamfer_lenth - y);
// 
//					 v.z = -m_thick + h;
//					 v.binormal.x = m_inner_plane_rad - x; 
// 					v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 					v.tv = 0.0f;
//					 *v_buf++ = v;
//					 ++v_count;
// 
//					 v.z = - h;
// 					v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 					v.tv = 1.0f;
//					 *v_buf++ = v;
//					 ++v_count;
//				 }
// 
//				 v.z = -m_thick;
//				 v.binormal.x = m_inner_plane_rad; 
// 				v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 				v.tv = 0.0f;
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = 0;
//				 v.binormal.x = m_inner_plane_rad; 
// 				v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 				v.tv = 1.0f;
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = -m_thick;
//				 v.binormal.x = m_outer_plane_rad; 
// 				v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 				v.tv = 0.0f;
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 v.z = 0;
//				 v.binormal.x = m_outer_plane_rad; 
// 				v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 				v.tv = 1.0f;
//				 *v_buf++ = v;
//				 ++v_count;
// 
//				 dx = (m_outer_arc_rad - m_outer_plane_rad)/m_chamfer_precission;
// 
//				 for(int i = 0 ; i< m_chamfer_precission;i++)
//				 {
//					 float x = dx*(i+1);
//					 float y = sqrt(max(0,m_chamfer_lenth*m_chamfer_lenth - x*x));
//					 float h = (m_chamfer_lenth - y);
// 
//					 v.z = -m_thick + h;
//					 v.binormal.x = m_outer_plane_rad + x; 
// 					v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 					v.tv = 0.0f;
//					 *v_buf++ = v;
//					 ++v_count;
// 
//					 v.z = - h;
// 					v.tu = (v.binormal.x/m_outer_radius + 1)*0.5f;
// 					v.tv = 1.0f;
//					 *v_buf++ = v;
//					 ++v_count;
//				 }
// 
//				 for(int j = 0 ; j< m_chamfer_precission*2 + 1 ;j++)
//				 {
//					 if(m_angle_end < m_angle_start)
//					 {
//						 *i_buf++ = v_start + j*2;
//						 *i_buf++ = v_start + j*2 + 2;
//						 *i_buf++ = v_start + j*2 + 1;
// 
//						 *i_buf++ = v_start + j*2 + 1;
//						 *i_buf++ = v_start + j*2 + 2;
//						 *i_buf++ = v_start + j*2 + 3;
//					 }
//					 else
//					 {
//						 *i_buf++ = v_start + j*2;
//						 *i_buf++ = v_start + j*2 + 1;
//						 *i_buf++ = v_start + j*2 + 2;
// 
//						 *i_buf++ = v_start + j*2 + 2;
//						 *i_buf++ = v_start + j*2 + 1;
//						 *i_buf++ = v_start + j*2 + 3;
//					 }
//				 }
//				 break;
//			 }
//		 }
//	 }
// 
//	 return v_count;
// 
// }

int CBasePieModeling::modeling_outer_arc(BaseGraphVertex3D*& v_buf, DWORD*& i_buf, int v_start)
{
	if(!m_has_outer_arc)
		return 0;

	int v_count = 0;
	UINT arc_point_count = calc_arc_point_count(m_angle_start,m_angle_end);
	float da = (m_angle_end - m_angle_start)/(arc_point_count -1);
		
	BaseGraphVertex3D v;

	v.x = m_angle_start;//起始角度

	v.nx = 1;
	v.ny = 0;
	v.nz = 0;

	v.tu = 0.5f;
	v.tv = 0.5f;

	v.binormal.x = m_outer_arc_rad;
	v.binormal.y = 6; //变换法线

	//arc_point_count*2个点
	for(UINT i = 0 ; i<arc_point_count; ++i)
	{
		float a0 = m_angle_start + i*da;
		
		v.y = a0;//终止角度

		//top
		v.tangent.x = 1;
		if(m_has_top_out_chamfer)
		{
			v.z = -m_thick + m_chamfer_height;
			v.tangent.y = m_chamfer_height;  
		}
		else
		{
			v.z = -m_thick;
			v.tangent.y = 0;  
		}

		v.tu = 1.0f - a0/(PAI * 2.0f);
		v.tv = 0.0f;

		*v_buf++ = v;
		++v_count;

		v.tu = 1.0f - a0/(PAI * 2.0f);
		v.tv = 1.0f;

		//bottom
		v.tangent.x = 0;
		if(m_has_bottom_out_chamfer)
			v.z = -m_chamfer_height;
		else
			v.z = 0;
		
		*v_buf++ = v;
		++v_count;
	}

	//（arc_point_count-1）*2个三角形
	for(UINT j = 0 ; j<arc_point_count-1; ++j)
	{
		if(m_angle_end > m_angle_start)
		{
			*i_buf++ = v_start + j*2;
			*i_buf++ = v_start + j*2 + 2;
			*i_buf++ = v_start + j*2 + 1;

			*i_buf++ = v_start + j*2 + 1;
			*i_buf++ = v_start + j*2 + 2;
			*i_buf++ = v_start + j*2 + 3;
		}
		else
		{
			*i_buf++ = v_start + j*2;
			*i_buf++ = v_start + j*2 + 1;
			*i_buf++ = v_start + j*2 + 2;

			*i_buf++ = v_start + j*2 + 2;
			*i_buf++ = v_start + j*2 + 1;
			*i_buf++ = v_start + j*2 + 3;
		}
	}

	return v_count;
}

int CBasePieModeling::modeling_inner_arc(BaseGraphVertex3D*& v_buf, DWORD*& i_buf, int v_start)
{
	if(!m_has_inner_arc)
		return 0;

	int v_count = 0;
	UINT arc_point_count = calc_arc_point_count(m_angle_start,m_angle_end);
	float da = (m_angle_end - m_angle_start)/(arc_point_count -1);

	BaseGraphVertex3D v;

	v.x = m_angle_start;//起始角度

	v.nx = -1;
	v.ny = 0;
	v.nz = 0;

	v.tu = 0.5f;
	v.tv = 0.5f;

	v.binormal.x = m_inner_arc_rad;
	v.binormal.y = 6; //变换法线

	//arc_point_count*2个点
	for(UINT i = 0 ; i<arc_point_count; ++i)
	{
		float a0 = m_angle_start + i*da;

		v.y = a0;//终止角度
  
		v.tangent.x = 1;
		if(m_has_top_in_chamfer)
		{
			v.z = -m_thick + m_chamfer_height;
			v.tangent.y = m_chamfer_height;
		}
		else
		{
			v.z = -m_thick;
			v.tangent.y = 0;
		}

		v.tu = 1.f - a0/(PAI * 2.0f);
		v.tv = 1.0f;

		*v_buf++ = v;
		++v_count;

		v.tangent.x = 0;  
		if(m_has_bottom_in_chamfer)
			v.z = -m_chamfer_height;
		else
			v.z = 0;
	
		v.tu = 1.f - a0/(PAI * 2.0f);
		v.tv = 0.0f;

		*v_buf++ = v;
		++v_count;
	}

	//（arc_point_count-1）*2个三角形
	for(UINT j = 0 ; j<arc_point_count-1; ++j)
	{
		if(m_angle_end < m_angle_start)
		{
			*i_buf++ = v_start + j*2;
			*i_buf++ = v_start + j*2 + 2;
			*i_buf++ = v_start + j*2 + 1;

			*i_buf++ = v_start + j*2 + 1;
			*i_buf++ = v_start + j*2 + 2;
			*i_buf++ = v_start + j*2 + 3;
		}
		else
		{
			*i_buf++ = v_start + j*2;
			*i_buf++ = v_start + j*2 + 1;
			*i_buf++ = v_start + j*2 + 2;

			*i_buf++ = v_start + j*2 + 2;
			*i_buf++ = v_start + j*2 + 1;
			*i_buf++ = v_start + j*2 + 3;
		}
	}

	return v_count;
}

int CBasePieModeling::modeling_outer_top_chamfer_plane(BaseGraphVertex3D*& v_buf, DWORD*& i_buf, int v_start)
{
	if(!m_has_top_out_chamfer)
		return 0;

	int v_count = 0;
	UINT arc_point_count = calc_arc_point_count(m_angle_start,m_angle_end);
	float da = (m_angle_end - m_angle_start)/(arc_point_count -1);

	BaseGraphVertex3D v;

	v.x = m_angle_start;//起始角度

	v.tu = 0.5f;
	v.tv = 0.5f;

	v.binormal.y = 6; //变换法线

	//上倒角

	float r = sqrt(m_chamfer_lenth*m_chamfer_lenth  +  m_chamfer_height*m_chamfer_height);

	v.nx = m_chamfer_height/r;
	v.ny = 0;
	v.nz = -m_chamfer_lenth/r; 

	v.tangent.x = 1;
	//arc_point_count*2个点
	for(UINT i = 0 ; i<arc_point_count; ++i)
	{
		float a0 = m_angle_start + i*da; 

		v.y = a0;//终止角度
		v.z = -m_thick;
		v.tangent.y = 0;

		v.binormal.x = m_outer_top_plane_rad;

		v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
		v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f;

		*v_buf++ = v;
		++v_count;

		v.z = -m_thick + m_chamfer_height;
		v.tangent.y = m_chamfer_height;

		v.binormal.x = m_outer_arc_rad;

		v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
		v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f;

		*v_buf++ = v;
		++v_count;
	}

	//（arc_point_count-1）*2个三角形
	for(UINT j = 0 ; j<arc_point_count-1; ++j)
	{
		if(m_angle_end > m_angle_start)
		{
			*i_buf++ = v_start + j*2;
			*i_buf++ = v_start + j*2 + 2;
			*i_buf++ = v_start + j*2 + 1;

			*i_buf++ = v_start + j*2 + 1;
			*i_buf++ = v_start + j*2 + 2;
			*i_buf++ = v_start + j*2 + 3;
		}
		else
		{
			*i_buf++ = v_start + j*2;
			*i_buf++ = v_start + j*2 + 1;
			*i_buf++ = v_start + j*2 + 2;

			*i_buf++ = v_start + j*2 + 2;
			*i_buf++ = v_start + j*2 + 1;
			*i_buf++ = v_start + j*2 + 3;
		}
	}

	return v_count;
}

int CBasePieModeling::modeling_outer_bottom_chamfer_plane(BaseGraphVertex3D*& v_buf, DWORD*& i_buf, int v_start)
{
	if(!m_has_bottom_out_chamfer)
		return 0;

	int v_count = 0;
	UINT arc_point_count = calc_arc_point_count(m_angle_start,m_angle_end);
	float da = (m_angle_end - m_angle_start)/(arc_point_count -1);

	BaseGraphVertex3D v;

	v.x = m_angle_start;//起始角度

	v.tu = 0.5f;
	v.tv = 0.5f;

	v.binormal.y = 6; //变换法线
	v.tangent.x = 0;

	//下倒角

	float r = sqrt(m_chamfer_lenth*m_chamfer_lenth + m_chamfer_height*m_chamfer_height);

	v.nx = m_chamfer_height/r;
	v.ny = 0;
	v.nz = m_chamfer_lenth/r; 

	for(UINT i = 0 ; i<arc_point_count; ++i)
	{
		float a0 = m_angle_start + i*da;

		v.y = a0;//终止角度
		v.z = -m_chamfer_height;
		v.binormal.x = m_outer_arc_rad;

		v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
		v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f;

		*v_buf++ = v;
		++v_count;

		v.binormal.x = m_outer_bottom_plane_rad;
		v.z = 0;

		v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
		v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f;

		*v_buf++ = v;
		++v_count;
	}

	for(UINT j = 0 ; j<arc_point_count-1; ++j)
	{
		if(m_angle_end > m_angle_start)
		{
			*i_buf++ = v_start + j*2;
			*i_buf++ = v_start + j*2 + 2;
			*i_buf++ = v_start + j*2 + 1;

			*i_buf++ = v_start + j*2 + 1;
			*i_buf++ = v_start + j*2 + 2;
			*i_buf++ = v_start + j*2 + 3;
		}
		else
		{
			*i_buf++ = v_start + j*2;
			*i_buf++ = v_start + j*2 + 1;
			*i_buf++ = v_start + j*2 + 2;

			*i_buf++ = v_start + j*2 + 2;
			*i_buf++ = v_start + j*2 + 1;
			*i_buf++ = v_start + j*2 + 3;
		}
	}

	return v_count;
}

// int CBasePieModeling::modeling_outer_chamfer_plane(BaseGraphVertex3D*& v_buf, DWORD*& i_buf, int v_start)
// {
//	 if(!m_has_chamfer)
//		 return 0;
// 
//	 int v_count = 0;
// 
//	 UINT arc_point_count = calc_arc_point_count(m_angle_start,m_angle_end);
// 
//	 float da = (m_angle_end - m_angle_start)/(arc_point_count -1);
// 
//	 BaseGraphVertex3D v;
// 
//	 v.x = m_angle_start;//起始角度
// 
//	 v.tu = 0.5f;
//	 v.tv = 0.5f;
// 
//	 v.binormal.y = 6; //变换法线
// 
//	 //上倒角
// 	v.nx = 1/sqrt(2.0f);
// 	v.ny = 0;
// 	v.nz = 1/sqrt(2.0f);
// 
//	 //arc_point_count*2个点
//	 for(UINT i = 0 ; i<arc_point_count; ++i)
//	 {
//		 float a0 = m_angle_start + i*da;
// 
//		 v.y = a0;//终止角度
// 		v.z = -m_thick;
// 		v.binormal.x = m_outer_plane_rad;
// 
// 		v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
// 		v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f;
//
//		 *v_buf++ = v;
//		 ++v_count;
// 
// 		v.z = -m_thick + m_chamfer_height;
// 		v.binormal.x = m_outer_arc_rad;
// 
// 		v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
// 		v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f;
//		 
//		 *v_buf++ = v;
//		 ++v_count;
//	 }
// 
//	 //（arc_point_count-1）*2个三角形
//	 for(UINT j = 0 ; j<arc_point_count-1; ++j)
//	 {
//		 if(m_angle_end > m_angle_start)
//		 {
//			 *i_buf++ = v_start + j*2;
//			 *i_buf++ = v_start + j*2 + 2;
//			 *i_buf++ = v_start + j*2 + 1;
// 
//			 *i_buf++ = v_start + j*2 + 1;
//			 *i_buf++ = v_start + j*2 + 2;
//			 *i_buf++ = v_start + j*2 + 3;
//		 }
//		 else
//		 {
//			 *i_buf++ = v_start + j*2;
//			 *i_buf++ = v_start + j*2 + 1;
//			 *i_buf++ = v_start + j*2 + 2;
// 
//			 *i_buf++ = v_start + j*2 + 2;
//			 *i_buf++ = v_start + j*2 + 1;
//			 *i_buf++ = v_start + j*2 + 3;
//		 }
//	 }
// 
//	 v_start += v_count;
// 
//	 //下倒角
// 
//	 v.nx = 1/sqrt(2.0f);
//	 v.ny = 0;
//	 v.nz = 1/sqrt(2.0f);
// 
//	 for(UINT i = 0 ; i<arc_point_count; ++i)
//	 {
//		 float a0 = m_angle_start + i*da;
// 
//		 v.y = a0;//终止角度
// 		v.z = -m_chamfer_height;
// 		v.binormal.x = m_outer_arc_rad;
// 
// 		v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
// 		v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f;
//		 
//		 *v_buf++ = v;
//		 ++v_count;
// 
// 		v.binormal.x = m_outer_plane_rad;
// 		v.z = 0;
// 
// 		v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
// 		v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f;
// 
//		 *v_buf++ = v;
//		 ++v_count;
//	 }
// 
//	 for(UINT j = 0 ; j<arc_point_count-1; ++j)
//	 {
// 
//		 if(m_angle_end > m_angle_start)
//		 {
//			 *i_buf++ = v_start + j*2;
//			 *i_buf++ = v_start + j*2 + 2;
//			 *i_buf++ = v_start + j*2 + 1;
// 
//			 *i_buf++ = v_start + j*2 + 1;
//			 *i_buf++ = v_start + j*2 + 2;
//			 *i_buf++ = v_start + j*2 + 3;
//		 }
//		 else
//		 {
//			 *i_buf++ = v_start + j*2;
//			 *i_buf++ = v_start + j*2 + 1;
//			 *i_buf++ = v_start + j*2 + 2;
// 
//			 *i_buf++ = v_start + j*2 + 2;
//			 *i_buf++ = v_start + j*2 + 1;
//			 *i_buf++ = v_start + j*2 + 3;
//		 }
//	 }
// 
//	 return v_count;
// }

int CBasePieModeling::modeling_inner_top_chamfer_plane(BaseGraphVertex3D*& v_buf, DWORD*& i_buf, int v_start)
{
	if(!m_has_top_in_chamfer)
		return 0;

	int v_count = 0;
	UINT arc_point_count = calc_arc_point_count(m_angle_start,m_angle_end);
	float da = (m_angle_end - m_angle_start)/(arc_point_count -1);

	BaseGraphVertex3D v;

	v.x = m_angle_start;//起始角度

	v.tu = 0.5f;
	v.tv = 0.5f;

	v.binormal.y = 6; //变换法线

	v.tangent.x = 1;

	//上倒角
	
	float r = sqrt(m_chamfer_lenth*m_chamfer_lenth + m_chamfer_height*m_chamfer_height);

	v.nx = -m_chamfer_height/r;
	v.ny = 0;
	v.nz = -m_chamfer_lenth/r; 

	//arc_point_count*2个点
	for(UINT i = 0 ; i<arc_point_count; ++i)
	{
		float a0 = m_angle_start + i*da;

		v.y = a0;//终止角度
		v.z = -m_thick;
		v.tangent.y = 0;

		v.binormal.x = m_inner_top_plane_rad;

		v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
		v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f;

		*v_buf++ = v;
		++v_count;
		////////////////////////////////////////////////

		v.z = -m_thick + m_chamfer_height;
		v.tangent.y = m_chamfer_height;

		v.binormal.x = m_inner_arc_rad;

		v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
		v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f;

		*v_buf++ = v;
		++v_count;
	}

	//（arc_point_count-1）*2个三角形
	for(UINT j = 0 ; j<arc_point_count-1; ++j)
	{
		if(m_angle_end < m_angle_start)
		{
			*i_buf++ = v_start + j*2;
			*i_buf++ = v_start + j*2 + 2;
			*i_buf++ = v_start + j*2 + 1;

			*i_buf++ = v_start + j*2 + 1;
			*i_buf++ = v_start + j*2 + 2;
			*i_buf++ = v_start + j*2 + 3;
		}
		else
		{
			*i_buf++ = v_start + j*2;
			*i_buf++ = v_start + j*2 + 1;
			*i_buf++ = v_start + j*2 + 2;

			*i_buf++ = v_start + j*2 + 2;
			*i_buf++ = v_start + j*2 + 1;
			*i_buf++ = v_start + j*2 + 3;
		}
	}

	return v_count;
}

int CBasePieModeling::modeling_inner_bottom_chamfer_plane(BaseGraphVertex3D*& v_buf, DWORD*& i_buf, int v_start)
{
	if(!m_has_bottom_in_chamfer)
		return 0;

	int v_count = 0;
	UINT arc_point_count = calc_arc_point_count(m_angle_start,m_angle_end);
	float da = (m_angle_end - m_angle_start)/(arc_point_count -1);

	BaseGraphVertex3D v;

	v.x = m_angle_start;//起始角度

	v.tu = 0.5f;
	v.tv = 0.5f;

	v.binormal.y = 6; //变换法线

	v.tangent.x = 0;
	
	//下倒角

	float r = sqrt(m_chamfer_lenth*m_chamfer_lenth + m_chamfer_height*m_chamfer_height);

	v.nx = -m_chamfer_height/r;
	v.ny = 0;
	v.nz = m_chamfer_lenth/r; 

	for(UINT i = 0 ; i<arc_point_count; ++i)
	{
		float a0 = m_angle_start + i*da;

		v.y = a0;//终止角度
		v.z = -m_chamfer_height;
		v.binormal.x = m_inner_arc_rad;

		v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
		v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f;

		*v_buf++ = v;
		++v_count;

		v.binormal.x = m_inner_bottom_plane_rad;

		v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
		v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f;

		v.z = 0;
		*v_buf++ = v;
		++v_count;
	}

	for(UINT j = 0 ; j<arc_point_count-1; ++j)
	{
		if(m_angle_end < m_angle_start)
		{
			*i_buf++ = v_start + j*2;
			*i_buf++ = v_start + j*2 + 2;
			*i_buf++ = v_start + j*2 + 1;

			*i_buf++ = v_start + j*2 + 1;
			*i_buf++ = v_start + j*2 + 2;
			*i_buf++ = v_start + j*2 + 3;
		}
		else
		{
			*i_buf++ = v_start + j*2;
			*i_buf++ = v_start + j*2 + 1;
			*i_buf++ = v_start + j*2 + 2;

			*i_buf++ = v_start + j*2 + 2;
			*i_buf++ = v_start + j*2 + 1;
			*i_buf++ = v_start + j*2 + 3;
		}
	}

	return v_count;
}

// int CBasePieModeling::modeling_inner_chamfer_plane(BaseGraphVertex3D*& v_buf, DWORD*& i_buf, int v_start)
// {
//	 if(!m_has_chamfer)
//		 return 0;
// 
//	 int v_count = 0;
// 
//	 UINT arc_point_count = calc_arc_point_count(m_angle_start,m_angle_end);
// 
//	 float da = (m_angle_end - m_angle_start)/(arc_point_count -1);
// 
//	 BaseGraphVertex3D v;
// 
//	 v.x = m_angle_start;//起始角度
// 
//	 v.tu = 0.5f;
//	 v.tv = 0.5f;
// 
//	 v.binormal.y = 6; //变换法线
// 
//	 //上倒角
//	 v.nx = -1/sqrt(2.0f);
//	 v.ny = 0;
//	 v.nz = -1/sqrt(2.0f);
// 
//	 //arc_point_count*2个点
//	 for(UINT i = 0 ; i<arc_point_count; ++i)
//	 {
//		 float a0 = m_angle_start + i*da;
// 
//		 v.y = a0;//终止角度
//		 v.z = -m_thick;
// 
//		 v.binormal.x = m_inner_plane_rad;
// 
// 		v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
// 		v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f;
// 
//		 *v_buf++ = v;
//		 ++v_count;
// 		////////////////////////////////////////////////
// 
//		 v.z = -m_thick + m_chamfer_height;
// 		
// 		v.binormal.x = m_inner_arc_rad;
// 
// 		v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
// 		v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f;
// 
//		 *v_buf++ = v;
//		 ++v_count;
//	 }
// 
//	 //（arc_point_count-1）*2个三角形
//	 for(UINT j = 0 ; j<arc_point_count-1; ++j)
//	 {
// 
//		 if(m_angle_end < m_angle_start)
//		 {
//			 *i_buf++ = v_start + j*2;
//			 *i_buf++ = v_start + j*2 + 2;
//			 *i_buf++ = v_start + j*2 + 1;
// 
//			 *i_buf++ = v_start + j*2 + 1;
//			 *i_buf++ = v_start + j*2 + 2;
//			 *i_buf++ = v_start + j*2 + 3;
//		 }
//		 else
//		 {
//			 *i_buf++ = v_start + j*2;
//			 *i_buf++ = v_start + j*2 + 1;
//			 *i_buf++ = v_start + j*2 + 2;
// 
//			 *i_buf++ = v_start + j*2 + 2;
//			 *i_buf++ = v_start + j*2 + 1;
//			 *i_buf++ = v_start + j*2 + 3;
//		 }
//	 }
// 
//	 v_start += v_count;
// 
//	 //下倒角
// 
//	 v.nx = -1/sqrt(2.0f);
//	 v.ny = 0;
//	 v.nz = 1/sqrt(2.0f);
// 
//	 for(UINT i = 0 ; i<arc_point_count; ++i)
//	 {
//		 float a0 = m_angle_start + i*da;
// 
//		 v.y = a0;//终止角度
//		 v.z = -m_chamfer_height;
//		 v.binormal.x = m_inner_arc_rad;
// 
// 		v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
// 		v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f;
// 
//		 *v_buf++ = v;
//		 ++v_count;
// 
//		 v.binormal.x = m_inner_plane_rad;
// 
// 		v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
// 		v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f;
// 
//		 v.z = 0;
//		 *v_buf++ = v;
//		 ++v_count;
//	 }
// 
//	 for(UINT j = 0 ; j<arc_point_count-1; ++j)
//	 {
//		 if(m_angle_end < m_angle_start)
//		 {
//			 *i_buf++ = v_start + j*2;
//			 *i_buf++ = v_start + j*2 + 2;
//			 *i_buf++ = v_start + j*2 + 1;
// 
//			 *i_buf++ = v_start + j*2 + 1;
//			 *i_buf++ = v_start + j*2 + 2;
//			 *i_buf++ = v_start + j*2 + 3;
//		 }
//		 else
//		 {
//			 *i_buf++ = v_start + j*2;
//			 *i_buf++ = v_start + j*2 + 1;
//			 *i_buf++ = v_start + j*2 + 2;
// 
//			 *i_buf++ = v_start + j*2 + 2;
//			 *i_buf++ = v_start + j*2 + 1;
//			 *i_buf++ = v_start + j*2 + 3;
//		 }
//	 }
// 
//	 return v_count;
// }

int CBasePieModeling::modeling_outer_top_chamfer_arc(BaseGraphVertex3D*& v_buf, DWORD*& i_buf, int v_start)
{
	if(!m_has_top_out_chamfer)
		return 0;

	int v_count = 0;
	UINT arc_point_count = calc_arc_point_count(m_angle_start,m_angle_end);
	float da = (m_angle_end - m_angle_start)/(arc_point_count -1);

	BaseGraphVertex3D v;

	v.x = m_angle_start;//起始角度

	v.tu = 0.5f;
	v.tv = 0.5f;

	v.binormal.y = 6; //变换法线

	v.tangent.x = 1;

	UINT t_point_count = m_chamfer_precission + 2;
	float dt = M_PI_2/(m_chamfer_precission + 1);

	//上倒角
	for(UINT i = 0 ; i<arc_point_count; ++i)
	{
		float a0 = m_angle_start + i*da;
		v.y = a0;//终止角度

		float dx = (m_outer_arc_rad - m_outer_top_plane_rad);

		for(UINT j = 0 ; j< t_point_count - 1; ++j)
		{
			float x = dx*sin(dt*j);
			float y = sqrt(max(0,(1 - x*x/(m_chamfer_lenth*m_chamfer_lenth)))) * m_chamfer_height;
			float h = (m_chamfer_height - y);

			v.z = -m_thick + h;
			v.tangent.y = h;

			v.binormal.x = m_outer_top_plane_rad + x; 

			v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
			v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f;

			float r = sqrt(x*x +y*y);

			v.nx = x/r;
			v.ny = 0;
			v.nz = -y/r;

			*v_buf++ = v;
			++v_count;
		}

		{
			v.z = - m_thick + m_chamfer_height;
			v.tangent.y = m_chamfer_height;

			v.binormal.x = m_outer_arc_rad; 

			v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
			v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f;

			v.nx = 1;
			v.ny = 0;
			v.nz = 0;

			*v_buf++ = v;
			++v_count;
		}
	}

	for(UINT j = 0 ; j<arc_point_count-1; ++j)
	{
		for(UINT k = 0 ; k<m_chamfer_precission + 1; ++k)
		{
			if(m_angle_end < m_angle_start)
			{
				*i_buf++ = v_start + j*t_point_count + k;
				*i_buf++ = v_start + j*t_point_count + k + 1;
				*i_buf++ = v_start + (j+1)*t_point_count + k + 1;

				*i_buf++ = v_start + j*t_point_count + k;
				*i_buf++ = v_start + (j+1)*t_point_count + k + 1;
				*i_buf++ = v_start + (j+1)*t_point_count + k ;
			}
			else
			{
				*i_buf++ = v_start + j*t_point_count + k;
				*i_buf++ = v_start + (j+1)*t_point_count + k + 1;
				*i_buf++ = v_start + j*t_point_count + k + 1;

				*i_buf++ = v_start + j*t_point_count + k;
				*i_buf++ = v_start + (j+1)*t_point_count + k ;
				*i_buf++ = v_start + (j+1)*t_point_count + k + 1;
			}
		}
	}

	return v_count;
}

int CBasePieModeling::modeling_outer_bottom_chamfer_arc(BaseGraphVertex3D*& v_buf, DWORD*& i_buf, int v_start)
{
	if(!m_has_bottom_out_chamfer)
		return 0;

	int v_count = 0;
	UINT arc_point_count = calc_arc_point_count(m_angle_start,m_angle_end);
	float da = (m_angle_end - m_angle_start)/(arc_point_count -1);

	BaseGraphVertex3D v;

	v.x = m_angle_start;//起始角度

	v.tu = 0.5f;
	v.tv = 0.5f;

	v.binormal.y = 6; //变换法线
	v.tangent.x = 0;

	UINT t_point_count = m_chamfer_precission + 2;

	//下倒角
	float dt = M_PI_2/ (m_chamfer_precission + 1);

	for(UINT i = 0 ; i<arc_point_count; ++i)
	{
		float a0 = m_angle_start + i*da;
		v.y = a0;//终止角度

		//float dx = (m_outer_arc_rad - m_outer_bottom_plane_rad)/m_chamfer_precission;

		for(UINT j = 0 ; j<t_point_count; ++j)
		{
			//float x = dx*j;
			float x = m_chamfer_lenth*cos(M_PI_2 - dt*j);
			float y = sqrt(max(0,(1 - x*x/(m_chamfer_lenth*m_chamfer_lenth)))) * m_chamfer_height;
			float h = (m_chamfer_height - y);

			v.z = -h;
			v.binormal.x = m_outer_bottom_plane_rad + x;

			v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
			v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f;

			float r = sqrt(x*x +y*y);

			v.nx = x/r;
			v.ny = 0;
			v.nz = y/r;

			*v_buf++ = v;
			++v_count;
		}
	}

	for(UINT j = 0 ; j<arc_point_count-1; ++j)
	{
		for(UINT k = 0 ; k<m_chamfer_precission + 1; ++k)
		{
			if(m_angle_end > m_angle_start)
			{
				*i_buf++ = v_start + j*t_point_count + k;
				*i_buf++ = v_start + j*t_point_count + k + 1;
				*i_buf++ = v_start + (j+1)*t_point_count + k + 1;

				*i_buf++ = v_start + j*t_point_count + k;
				*i_buf++ = v_start + (j+1)*t_point_count + k + 1;
				*i_buf++ = v_start + (j+1)*t_point_count + k ;
			}
			else
			{
				*i_buf++ = v_start + j*t_point_count + k;
				*i_buf++ = v_start + (j+1)*t_point_count + k + 1;
				*i_buf++ = v_start + j*t_point_count + k + 1;

				*i_buf++ = v_start + j*t_point_count + k;
				*i_buf++ = v_start + (j+1)*t_point_count + k ;
				*i_buf++ = v_start + (j+1)*t_point_count + k + 1;
			}
		}
	}

	return v_count;
}

// int CBasePieModeling::modeling_outer_chamfer_arc(BaseGraphVertex3D*& v_buf, DWORD*& i_buf, int v_start)
// {
//	 if(!m_has_chamfer)
//		 return 0;
// 
//	 int v_count = 0; 
//	 UINT arc_point_count = calc_arc_point_count(m_angle_start,m_angle_end); 
//	 float da = (m_angle_end - m_angle_start)/(arc_point_count -1); 
//	 BaseGraphVertex3D v;
// 
//	 v.x = m_angle_start;//起始角度
// 
//	 v.tu = 0.5f;
//	 v.tv = 0.5f;
// 
//	 v.binormal.y = 6; //变换法线
// 
//	 int t_point_count = m_chamfer_precission + 1;
// 
//	 //上倒角
//	 for(UINT i = 0 ; i<arc_point_count; ++i)
//	 {
//		 float a0 = m_angle_start + i*da;
//		 v.y = a0;//终止角度
// 
//		 float dx = (m_outer_arc_rad - m_outer_plane_rad)/m_chamfer_precission;
// 
//		 for(UINT j = 0 ; j<t_point_count; ++j)
//		 {
//			 float x = dx*j;
//			 float y = sqrt(max(0,m_chamfer_lenth*m_chamfer_lenth - x*x));
//			 float h = (m_chamfer_lenth - y);
// 
//			 v.z = -m_thick + h;
//			 v.binormal.x = m_outer_plane_rad + x; 
// 			
// 			v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
// 			v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f;
// 
//			 v.nx = x/m_chamfer_lenth;
//			 v.ny = 0;
//			 v.nz = -y/m_chamfer_lenth;
// 
//			 *v_buf++ = v;
//			 ++v_count;
//		 }
//	 }
// 
//	 for(UINT j = 0 ; j<arc_point_count-1; ++j)
//	 {
//		 for(UINT k = 0 ; k<m_chamfer_precission; ++k)
//		 {
//			 if(m_angle_end < m_angle_start)
//			 {
//				 *i_buf++ = v_start + j*t_point_count + k;
//				 *i_buf++ = v_start + j*t_point_count + k + 1;
//				 *i_buf++ = v_start + (j+1)*t_point_count + k + 1;
// 
//				 *i_buf++ = v_start + j*t_point_count + k;
//				 *i_buf++ = v_start + (j+1)*t_point_count + k + 1;
//				 *i_buf++ = v_start + (j+1)*t_point_count + k ;
//			 }
//			 else
//			 {
//				 *i_buf++ = v_start + j*t_point_count + k;
//				 *i_buf++ = v_start + (j+1)*t_point_count + k + 1;
//				 *i_buf++ = v_start + j*t_point_count + k + 1;
// 
//				 *i_buf++ = v_start + j*t_point_count + k;
//				 *i_buf++ = v_start + (j+1)*t_point_count + k ;
//				 *i_buf++ = v_start + (j+1)*t_point_count + k + 1;
//			 }
//		 }
//	 }
// 
//	 v_start += v_count;
// 
//	 //下倒角
// 
//	 for(UINT i = 0 ; i<arc_point_count; ++i)
//	 {
//		 float a0 = m_angle_start + i*da;
//		 v.y = a0;//终止角度
// 
//		 float dx = (m_outer_arc_rad - m_outer_plane_rad)/m_chamfer_precission;
// 
//		 for(UINT j = 0 ; j<t_point_count; ++j)
//		 {
//			 float x = dx*j;
//			 float y = sqrt(max(0,m_chamfer_lenth*m_chamfer_lenth - x*x));
//			 float h = (m_chamfer_lenth - y);
// 
//			 v.z = -h;
//			 v.binormal.x = m_outer_plane_rad + x; 
// 
// 			v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
// 			v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f;
// 
//			 v.nx = x/m_chamfer_lenth;
//			 v.ny = 0;
//			 v.nz = y/m_chamfer_lenth;
// 
//			 *v_buf++ = v;
//			 ++v_count;
//		 }
//	 }
// 
//	 for(UINT j = 0 ; j<arc_point_count-1; ++j)
//	 {
//		 for(UINT k = 0 ; k<m_chamfer_precission; ++k)
//		 {
//			 if(m_angle_end > m_angle_start)
//			 {
//				 *i_buf++ = v_start + j*t_point_count + k;
//				 *i_buf++ = v_start + j*t_point_count + k + 1;
//				 *i_buf++ = v_start + (j+1)*t_point_count + k + 1;
// 
//				 *i_buf++ = v_start + j*t_point_count + k;
//				 *i_buf++ = v_start + (j+1)*t_point_count + k + 1;
//				 *i_buf++ = v_start + (j+1)*t_point_count + k ;
//			 }
//			 else
//			 {
//				 *i_buf++ = v_start + j*t_point_count + k;
//				 *i_buf++ = v_start + (j+1)*t_point_count + k + 1;
//				 *i_buf++ = v_start + j*t_point_count + k + 1;
// 
//				 *i_buf++ = v_start + j*t_point_count + k;
//				 *i_buf++ = v_start + (j+1)*t_point_count + k ;
//				 *i_buf++ = v_start + (j+1)*t_point_count + k + 1;
//			 }
//		 }
//	 }
// 
//	 return v_count;
// }

int CBasePieModeling::modeling_inner_top_chamfer_arc(BaseGraphVertex3D*& v_buf, DWORD*& i_buf, int v_start)
{
	if(!m_has_top_in_chamfer)
		return 0;

	int v_count = 0;
	UINT arc_point_count = calc_arc_point_count(m_angle_start,m_angle_end);
	float da = (m_angle_end - m_angle_start)/(arc_point_count -1);

	BaseGraphVertex3D v;

	v.x = m_angle_start;//起始角度

	v.tu = 0.5f;
	v.tv = 0.5f;

	v.binormal.y = 6; //变换法线
	v.tangent.x = 1;

	UINT t_point_count = m_chamfer_precission + 2;

	//float dx = (m_inner_top_plane_rad - m_inner_arc_rad)/m_chamfer_precission;
	float dt = M_PI_2/ (m_chamfer_precission + 1);

	//上倒角
	for(UINT i = 0 ; i<arc_point_count; ++i)
	{
		float a0 = m_angle_start + i*da;
		v.y = a0;//终止角度

		for(UINT j = 0 ; j<t_point_count; ++j)
		{
			//float x = dx*j;
			float x = m_chamfer_lenth*cos(M_PI_2 - dt*j);
			float y = sqrt(max(0,(1 - x*x/(m_chamfer_lenth*m_chamfer_lenth)))) * m_chamfer_height;
			float h = (m_chamfer_height - y);
			
			v.z = -m_thick + h;
			v.tangent.y = h;

			v.binormal.x = m_inner_top_plane_rad - x; 

			float r = sqrt(x*x +y*y);

			v.nx = -x/r;
			v.ny = 0;
			v.nz = -y/r;

			v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
			v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f;

			*v_buf++ = v;
			++v_count;
		}
	}

	for(UINT j = 0 ; j<arc_point_count-1; ++j)
	{
		for(UINT k = 0 ; k<m_chamfer_precission + 1; ++k)
		{
			if(m_angle_end > m_angle_start)
			{
				*i_buf++ = v_start + j*t_point_count + k;
				*i_buf++ = v_start + j*t_point_count + k + 1;
				*i_buf++ = v_start + (j+1)*t_point_count + k + 1;

				*i_buf++ = v_start + j*t_point_count + k;
				*i_buf++ = v_start + (j+1)*t_point_count + k + 1;
				*i_buf++ = v_start + (j+1)*t_point_count + k ;
			}
			else
			{
				*i_buf++ = v_start + j*t_point_count + k;
				*i_buf++ = v_start + (j+1)*t_point_count + k + 1;
				*i_buf++ = v_start + j*t_point_count + k + 1;

				*i_buf++ = v_start + j*t_point_count + k;
				*i_buf++ = v_start + (j+1)*t_point_count + k ;
				*i_buf++ = v_start + (j+1)*t_point_count + k + 1;
			}
		}
	}
	return v_count;
}

int CBasePieModeling::modeling_inner_bottom_chamfer_arc(BaseGraphVertex3D*& v_buf, DWORD*& i_buf, int v_start)
{
	if(!m_has_bottom_in_chamfer)
		return 0;

	int v_count = 0;
	UINT arc_point_count = calc_arc_point_count(m_angle_start,m_angle_end);
	float da = (m_angle_end - m_angle_start)/(arc_point_count -1);

	BaseGraphVertex3D v;

	v.x = m_angle_start;//起始角度

	v.tu = 0.5f;
	v.tv = 0.5f;

	v.binormal.y = 6; //变换法线
	v.tangent.x  = 0;

	UINT t_point_count = m_chamfer_precission + 2;

// 	float dx = (m_inner_bottom_plane_rad - m_inner_arc_rad)
// 					/m_chamfer_precission;
	float dt = M_PI_2/ (m_chamfer_precission + 1);

	//下倒角
	for(UINT i = 0 ; i<arc_point_count; ++i)
	{
		float a0 = m_angle_start + i*da;
		v.y = a0;//终止角度

		for(UINT j = 0 ; j<t_point_count; ++j)
		{
			//float x = dx*j;
			float x = m_chamfer_lenth*cos(M_PI_2 - dt*j);
			float y = sqrt(max(0,(1 - x*x/(m_chamfer_lenth*m_chamfer_lenth)))) * m_chamfer_height;
			float h = (m_chamfer_height - y);

			v.z = -h;
			v.binormal.x = m_inner_bottom_plane_rad - x;

			float r = sqrt(x*x +y*y);

			v.nx = -x/r;
			v.ny = 0;
			v.nz = y/r;

			v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
			v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f;

			*v_buf++ = v;
			++v_count;
		}
	}

	for(UINT j = 0 ; j<arc_point_count-1; ++j)
	{
		for(UINT k = 0 ; k<m_chamfer_precission + 1; ++k)
		{
			if(m_angle_end < m_angle_start)
			{
				*i_buf++ = v_start + j*t_point_count + k;
				*i_buf++ = v_start + j*t_point_count + k + 1;
				*i_buf++ = v_start + (j+1)*t_point_count + k + 1;

				*i_buf++ = v_start + j*t_point_count + k;
				*i_buf++ = v_start + (j+1)*t_point_count + k + 1;
				*i_buf++ = v_start + (j+1)*t_point_count + k ;
			}
			else
			{
				*i_buf++ = v_start + j*t_point_count + k;
				*i_buf++ = v_start + (j+1)*t_point_count + k + 1;
				*i_buf++ = v_start + j*t_point_count + k + 1;

				*i_buf++ = v_start + j*t_point_count + k;
				*i_buf++ = v_start + (j+1)*t_point_count + k ;
				*i_buf++ = v_start + (j+1)*t_point_count + k + 1;
			}
		}
	}

	return v_count;
}

// int CBasePieModeling::modeling_inner_chamfer_arc(BaseGraphVertex3D*& v_buf, DWORD*& i_buf, int v_start)
// {
//	 if(!m_has_chamfer)
//		 return 0;
// 
//	 int v_count = 0; 
//	 UINT arc_point_count = calc_arc_point_count(m_angle_start,m_angle_end);
//	 float da = (m_angle_end - m_angle_start)/(arc_point_count -1);
//	 BaseGraphVertex3D v;
// 
//	 v.x = m_angle_start;//起始角度
// 
//	 v.tu = 0.5f;
//	 v.tv = 0.5f;
// 
//	 v.binormal.y = 6; //变换法线
// 
//	 int t_point_count = m_chamfer_precission + 1;
// 
//	 float dx = (m_inner_plane_rad - m_inner_arc_rad)/m_chamfer_precission;
// 
//	 //上倒角
//	 for(UINT i = 0 ; i<arc_point_count; ++i)
//	 {
//		 float a0 = m_angle_start + i*da;
//		 v.y = a0;//终止角度
//	  
//		 for(UINT j = 0 ; j<t_point_count; ++j)
//		 {
//			 float x = dx*j;
//			 float y = sqrt(max(0,m_chamfer_lenth*m_chamfer_lenth - x*x));
//			 float h = (m_chamfer_lenth - y);
// 
// 
//			 v.z = -m_thick + h;
//			 v.binormal.x = m_inner_plane_rad - x; 
// 
//			 v.nx = -x/m_chamfer_lenth;
//			 v.ny = 0;
//			 v.nz = -y/m_chamfer_lenth;
// 
// 			v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
// 			v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f;
// 
//			 *v_buf++ = v;
//			 ++v_count;
//		 }
//	 }
// 
//	 for(UINT j = 0 ; j<arc_point_count-1; ++j)
//	 {
//		 for(UINT k = 0 ; k<m_chamfer_precission; ++k)
//		 {
//			 if(m_angle_end > m_angle_start)
//			 {
//				 *i_buf++ = v_start + j*t_point_count + k;
//				 *i_buf++ = v_start + j*t_point_count + k + 1;
//				 *i_buf++ = v_start + (j+1)*t_point_count + k + 1;
// 
//				 *i_buf++ = v_start + j*t_point_count + k;
//				 *i_buf++ = v_start + (j+1)*t_point_count + k + 1;
//				 *i_buf++ = v_start + (j+1)*t_point_count + k ;
//			 }
//			 else
//			 {
//				 *i_buf++ = v_start + j*t_point_count + k;
//				 *i_buf++ = v_start + (j+1)*t_point_count + k + 1;
//				 *i_buf++ = v_start + j*t_point_count + k + 1;
// 
//				 *i_buf++ = v_start + j*t_point_count + k;
//				 *i_buf++ = v_start + (j+1)*t_point_count + k ;
//				 *i_buf++ = v_start + (j+1)*t_point_count + k + 1;
//			 }
//		 }
//	 }
// 
//	 v_start += v_count;
// 
//	 //下倒角
// 
//	 for(UINT i = 0 ; i<arc_point_count; ++i)
//	 {
//		 float a0 = m_angle_start + i*da;
//		 v.y = a0;//终止角度
// 
//		 for(UINT j = 0 ; j<t_point_count; ++j)
//		 {
//			 float x = dx*j;
//			 float y = sqrt(max(0,m_chamfer_lenth*m_chamfer_lenth - x*x));
//			 float h = (m_chamfer_lenth - y);
// 
//			 v.z = -h;
//			 v.binormal.x = m_inner_plane_rad - x; 
// 
//			 v.nx = -x/m_chamfer_lenth;
//			 v.ny = 0;
//			 v.nz = y/m_chamfer_lenth;
// 
// 			v.tu = (cosf(a0)*v.binormal.x/m_outer_radius + 1)*0.5f;
// 			v.tv = (1 - sinf(a0)*v.binormal.x/m_outer_radius)*0.5f;
// 
//			 *v_buf++ = v;
//			 ++v_count;
//		 }
//	 }
// 
//	 for(UINT j = 0 ; j<arc_point_count-1; ++j)
//	 {
//		 for(UINT k = 0 ; k<m_chamfer_precission; ++k)
//		 {
//			 if(m_angle_end < m_angle_start)
//			 {
//				 *i_buf++ = v_start + j*t_point_count + k;
//				 *i_buf++ = v_start + j*t_point_count + k + 1;
//				 *i_buf++ = v_start + (j+1)*t_point_count + k + 1;
// 
//				 *i_buf++ = v_start + j*t_point_count + k;
//				 *i_buf++ = v_start + (j+1)*t_point_count + k + 1;
//				 *i_buf++ = v_start + (j+1)*t_point_count + k ;
//			 }
//			 else
//			 {
//				 *i_buf++ = v_start + j*t_point_count + k;
//				 *i_buf++ = v_start + (j+1)*t_point_count + k + 1;
//				 *i_buf++ = v_start + j*t_point_count + k + 1;
// 
//				 *i_buf++ = v_start + j*t_point_count + k;
//				 *i_buf++ = v_start + (j+1)*t_point_count + k ;
//				 *i_buf++ = v_start + (j+1)*t_point_count + k + 1;
//			 }
//		 }
//	 }
// 
//	 return v_count;
// }

BOOL Test(BaseGraphModel* pMod)
{
	int vcount = 4;
	int icount = 6;
	int tcount = 4;

	pMod->vcount = vcount;
	pMod->icount = icount;
	pMod->nviewsub = 1;
	pMod->viewsub = new BaseGraphSubModel[pMod->nviewsub];

	pMod->v = new BaseGraphVertex3D[vcount];
	pMod->indecs = new DWORD[icount];

	BaseGraphVertex3D*  pv =  pMod->v;
	DWORD*		 pi =  pMod->indecs;

	pMod->viewsub[0].vstart = 0;
	pMod->viewsub[0].vcount = vcount;

	pMod->viewsub[0].istart = 0;
	pMod->viewsub[0].icount = icount;

	pMod->viewsub[0].nTri_Num = tcount;
	pMod->viewsub[0].nTri_Type = Common_TRIANGLE_LIST;
	pMod->viewsub[0].pMaterial = NULL;

	return TRUE;
}

BOOL CBasePieModeling::CreateModel(BaseGraphModel* pMod)
{
	int vcount = 0;
	int icount = 0;
	int tcount = 0;

	GetModelParam(&vcount,&icount,&tcount);

	pMod->vcount = vcount;
	pMod->icount = icount;
	pMod->nviewsub = 1;
	pMod->viewsub = new BaseGraphSubModel[pMod->nviewsub];

	pMod->v = new BaseGraphVertex3D[vcount];
	pMod->indecs = new DWORD[icount];

	BaseGraphVertex3D*  pv =  pMod->v;
	DWORD*		 pi =  pMod->indecs;

	int		 vbase = 0;

	vbase += modeling_top_plane(pv,pi,vbase);
	vbase += modeling_bottom_plane(pv,pi,vbase);
	vbase += modeling_start_face(pv,pi,vbase);
	vbase += modeling_end_face(pv,pi,vbase);
	
	if(m_has_top_in_chamfer)
	{
		if(CHAMFER_PLANE == m_chamfer_type)
			vbase += modeling_inner_top_chamfer_plane(pv,pi,vbase);
		else
			vbase += modeling_inner_top_chamfer_arc(pv,pi,vbase); 
	}

	if(m_has_top_out_chamfer)
	{
		if(CHAMFER_PLANE == m_chamfer_type)
			vbase += modeling_outer_top_chamfer_plane(pv,pi,vbase);
		else
			vbase += modeling_outer_top_chamfer_arc(pv,pi,vbase);
	}

	if(m_has_bottom_in_chamfer)
	{
		if(CHAMFER_PLANE == m_chamfer_type)
			vbase += modeling_inner_bottom_chamfer_plane(pv,pi,vbase);
		else
			vbase += modeling_inner_bottom_chamfer_arc(pv,pi,vbase); 
	}

	if(m_has_bottom_out_chamfer)
	{
		if(CHAMFER_PLANE == m_chamfer_type)
			vbase += modeling_outer_bottom_chamfer_plane(pv,pi,vbase);
		else
			vbase += modeling_outer_bottom_chamfer_arc(pv,pi,vbase); 
	}

	vbase += modeling_outer_arc(pv,pi,vbase);
	vbase += modeling_inner_arc(pv,pi,vbase);

	pMod->viewsub[0].vstart = 0;
	pMod->viewsub[0].vcount = vcount;

	pMod->viewsub[0].istart = 0;
	pMod->viewsub[0].icount = icount;

	pMod->viewsub[0].nTri_Num = tcount;
	pMod->viewsub[0].nTri_Type = Common_TRIANGLE_LIST;
	pMod->viewsub[0].pMaterial = NULL;

	return TRUE;
}

void CBasePieModeling::calc_top_plane_inner_rad()
{
	if(m_has_top_in_chamfer)
	{
		m_inner_top_plane_rad = m_inner_radius + m_chamfer_lenth;
	}
	else
	{
		m_inner_top_plane_rad = m_inner_radius;
	}
}

void CBasePieModeling::calc_bottom_plane_inner_rad()
{
	if(m_has_bottom_in_chamfer)
	{
		m_inner_bottom_plane_rad = m_inner_radius + m_chamfer_lenth;
	}
	else
	{
		m_inner_bottom_plane_rad = m_inner_radius;
	}
}

void CBasePieModeling::calc_top_plane_outer_rad()
{
	if(m_has_top_out_chamfer)
	{
		m_outer_top_plane_rad = m_outer_radius - m_chamfer_lenth;
	}
	else
	{
		m_outer_top_plane_rad = m_outer_radius;
	}
}

void CBasePieModeling::calc_bottom_plane_outer_rad()
{
	if(m_has_bottom_out_chamfer)
	{
		m_outer_bottom_plane_rad = m_outer_radius - m_chamfer_lenth;
	}
	else
	{
		m_outer_bottom_plane_rad = m_outer_radius;
	}
}

void CBasePieModeling::calc_radius()
{
	calc_top_plane_inner_rad();
	calc_bottom_plane_inner_rad();
	calc_top_plane_outer_rad();
	calc_bottom_plane_outer_rad();

	m_outer_arc_rad = m_outer_radius;
	m_inner_arc_rad = m_inner_radius;
}

void CBasePieModeling::plane_veterics_count(	int* v_count, int* i_count, int* t_count,BOOL has_holl)
{
	int num_v = 0;
	int num_i = 0;
	int num_t = 0;
	UINT arc_point_count = calc_arc_point_count(m_angle_start,m_angle_end);

	switch(m_shape)
	{
	case SHAPE_PIE:
		{
			num_v =  (arc_point_count + 1);
			num_t =  (arc_point_count - 1); 
			break;
		}
	case SHAPE_PIE_RING:
		{
			if(!has_holl)
			{
				num_v =  (arc_point_count + 1);
				num_t =  (arc_point_count - 1); 
			}
			else
			{
				num_v =  (arc_point_count)*2;
				num_t =  (arc_point_count - 1)*2; 
			}
			break;
		}
	}

	num_i =  num_t*3;

	if(v_count)
		*v_count = num_v;

	if(i_count)
		*i_count = num_i;

	if(t_count)
		*t_count = num_t;
}

void CBasePieModeling::calc_chamfer_size()
{
	if((m_chamfer_type == 0) 
		|| abs(m_chamfer_lenth)< 0.0f 
		|| abs(m_chamfer_height)< 0.0f 
		|| m_chamfer_layout == CHAMFER_LAYOUT_NONE)
	{
		m_chamfer_layout = CHAMFER_LAYOUT_NONE;
		m_chamfer_height = 0;
		m_chamfer_lenth = 0;
	}

	if(SHAPE_PIE == m_shape)
	{
		m_chamfer_layout &= ~CHAMFER_LAYOUT_IN;
	}

	if((m_chamfer_layout & CHAMFER_LAYOUT_TOP) 
		&& (m_chamfer_layout & CHAMFER_LAYOUT_IN))
		m_has_top_in_chamfer = TRUE;

	if((m_chamfer_layout & CHAMFER_LAYOUT_TOP) 
		&& (m_chamfer_layout & CHAMFER_LAYOUT_OUT))
		m_has_top_out_chamfer = TRUE;

	if((m_chamfer_layout & CHAMFER_LAYOUT_BOTTOM) 
		&& (m_chamfer_layout &  CHAMFER_LAYOUT_IN))
		m_has_bottom_in_chamfer = TRUE;

	if((m_chamfer_layout & CHAMFER_LAYOUT_BOTTOM) 
		&& (m_chamfer_layout &  CHAMFER_LAYOUT_OUT))
		m_has_bottom_out_chamfer = TRUE;
	
	if((m_chamfer_layout & CHAMFER_LAYOUT_OUT)
		&& (m_chamfer_layout & CHAMFER_LAYOUT_IN))
	{
		float max_l = (m_outer_radius - m_inner_radius)/2.0f;
		m_chamfer_lenth = min(max_l,m_chamfer_lenth);  
	}
	else if((m_chamfer_layout & CHAMFER_LAYOUT_OUT) 
		|| (m_chamfer_layout & CHAMFER_LAYOUT_IN))
	{
		float max_l = (m_outer_radius - m_inner_radius);
		m_chamfer_lenth = min(max_l,m_chamfer_lenth);  
	}

	if((m_chamfer_layout & CHAMFER_LAYOUT_TOP) 
		&& (m_chamfer_layout & CHAMFER_LAYOUT_BOTTOM))
	{
		float max_l = (m_thick)/2.0f;
		m_chamfer_height = min(max_l,m_chamfer_height);  
	}
	else if((m_chamfer_layout & CHAMFER_LAYOUT_TOP) 
		|| (m_chamfer_layout & CHAMFER_LAYOUT_BOTTOM))
	{
		m_chamfer_height = min(m_thick,m_chamfer_height);
	}
}

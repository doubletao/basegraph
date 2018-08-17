#pragma once
#include "DataRender\DataRenderDataDef.h"

class BASEGRAPH_DATARENDER_API CBasePieModeling
{
private:
	CBasePieModeling(void);

	void calc_plane_face(int* v_count, int* i_count, int* t_count);
	void calc_arc_face(int* v_count, int* i_count, int* t_count);
	void calc_chamfer_face(int* v_count, int* i_count, int* t_count);
	void calc_start_end_face(int* v_count, int* i_count, int* t_count);

	UINT calc_arc_point_count(float angle_start,float m_angle_end);

	int modeling_top_plane(BaseGraphVertex3D*& vertex_buffer, DWORD*& index_buffer, int vertex_start);
	int modeling_bottom_plane(BaseGraphVertex3D*& vertex_buffer, DWORD*& index_buffer, int vertex_start);
	int modeling_start_face(BaseGraphVertex3D*& vertex_buffer, DWORD*& index_buffer, int vertex_start);
	int modeling_end_face(BaseGraphVertex3D*& vertex_buffer, DWORD*& index_buffer, int vertex_start);

	int modeling_outer_arc(BaseGraphVertex3D*& vertex_buffer, DWORD*& index_buffer, int vertex_start);
	int modeling_inner_arc(BaseGraphVertex3D*& vertex_buffer, DWORD*& index_buffer, int vertex_start);
	
	int modeling_outer_top_chamfer_plane(BaseGraphVertex3D*& vertex_buffer, DWORD*& index_buffer, int vertex_start);
	int modeling_inner_top_chamfer_plane(BaseGraphVertex3D*& vertex_buffer, DWORD*& index_buffer, int vertex_start);
	int modeling_outer_bottom_chamfer_plane(BaseGraphVertex3D*& vertex_buffer, DWORD*& index_buffer, int vertex_start);
	int modeling_inner_bottom_chamfer_plane(BaseGraphVertex3D*& vertex_buffer, DWORD*& index_buffer, int vertex_start);

	int modeling_outer_top_chamfer_arc(BaseGraphVertex3D*& vertex_buffer, DWORD*& index_buffer, int vertex_start);
	int modeling_inner_top_chamfer_arc(BaseGraphVertex3D*& vertex_buffer, DWORD*& index_buffer, int vertex_start);
	int modeling_outer_bottom_chamfer_arc(BaseGraphVertex3D*& vertex_buffer, DWORD*& index_buffer, int vertex_start);
	int modeling_inner_bottom_chamfer_arc(BaseGraphVertex3D*& vertex_buffer, DWORD*& index_buffer, int vertex_start);


	UINT m_shape;   //形状

	UINT m_frag;   //精度(1-10);

	float m_angle_start;
	float m_angle_end;

	float m_outer_radius;   //外半径
	float m_inner_radius;   //内半径

	float m_thick;

	UINT  m_chamfer_type;
	float m_chamfer_lenth;
	float m_chamfer_height;
	UINT  m_chamfer_precission;
	UINT  m_chamfer_layout;

	BOOL  m_has_outer_arc; //有无弧面
	BOOL  m_has_inner_arc; //有无内弧面

	BOOL  m_has_top_plane; //有无上平面
	BOOL  m_has_bottom_plane;  //有无下平面

	BOOL  m_has_top_in_chamfer;
	BOOL  m_has_bottom_in_chamfer;

	BOOL  m_has_top_out_chamfer;
	BOOL  m_has_bottom_out_chamfer;

	BOOL  m_has_top_holl;
	BOOL  m_has_bottom_holl;

	float m_outer_top_plane_rad ;//上平面外半径
	float m_outer_bottom_plane_rad ;//下平面外半径
	
	float m_outer_arc_rad ;//外弧面半径
	float m_inner_arc_rad ;//内弧面半径

	float m_inner_top_plane_rad ;//上平面内半径
	float m_inner_bottom_plane_rad ;//下平面内半径

	int   m_vertrics_count;
	int   m_indics_count;
	int   m_triangle_count;

	void  calc_radius();

	void  calc_top_plane_inner_rad();
	void  calc_bottom_plane_inner_rad();
	void  calc_top_plane_outer_rad();
	void  calc_bottom_plane_outer_rad();
	
	void  calc_chamfer_size();

	void  plane_veterics_count(int* v_count, 
								int* i_count, 
								int* t_count,
								BOOL has_holl);

	enum
	{
		CHAMFER_NONE = 0,
		CHAMFER_PLANE = 1,
		CHAMFER_ARC = 2,
	};

	enum
	{
		SHAPE_PIE = 0,
		SHAPE_PIE_RING = 1,
	};

	enum
	{
		CHAMFER_LAYOUT_NONE = 0,
		CHAMFER_LAYOUT_TOP = 1,
		CHAMFER_LAYOUT_BOTTOM = 2,
		CHAMFER_LAYOUT_IN = 4,
		CHAMFER_LAYOUT_OUT = 8,
	};

public:
	CBasePieModeling( UINT uShape,
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
					UINT uChamferLayout);

	~CBasePieModeling(void);

	void GetModelParam(int* vertex_count, int* index_count, int* triangle_count);
	BOOL CreateModel(BaseGraphModel* pMod);
};

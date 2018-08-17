#pragma once
/********************************************************************
	purpose:	二维空间与三维空间转换的API
*********************************************************************/
//数据结构
struct BaseGraphViewPort
{
	int			dwX;
	int			dwY;
	DWORD       dwWidth;
	DWORD       dwHeight;
	float       fMinZ;
	float       fMaxZ;
};

struct BaseGraphSpaceConvertParam
{
	const matrix4f*		pWorldMat;		//世界矩阵
	const matrix4f*		pViewMat;		//摄像机矩阵
	const matrix4f*		pProjMat;		//投影矩阵
	
	const BaseGraphViewPort*	pViewPort;		//视口矩阵
	const float3*		pEyePos;		//摄像机位置
	int					nProjType;		//投影类型
	float				fZNear;
	float				fZFar;
	float				fScale;
};

enum BaseGraphPROJTYPE
{
	Common_PERSPECTIVE_PROJ,
	Common_ORTHO_PROJ,
};

enum Common_SPACE_CONVERT
{
	BaseGraph2DVEC_TO_3D_VIEWPLANE,//将屏幕座标转换为与近平面平行的平面上在VIEW空间下座标
	BaseGraph2DVEC_TO_3D_WORLD,	//将屏幕座标转换为近平面的物理座标（若给定的矩阵为自身矩阵，则转换到自身座标系下，若为单位阵，则转换到世界座标系下）
	BaseGraph2DVEC_TO_3D_PLANE,	//将屏幕座标转换为近平面的物理座标（若给定的矩阵为自身矩阵，则转换到自身座标系下，若为单位阵，则转换到世界座标系下）
	BaseGraph3DVEC_TO_2D,			//将3D投影到2D平面上
	BaseGraph3DVEC_TO_VIEWPLANE,	//将3D坐标转换到指定z值的同一条射线上，输出顶点为物件空间，输出顶点与输入顶点在二维的投影面相同
};

void BASEGRAPH_SCENE_RENDER_API ChangeZPos(float3* pPos,const matrix4f* pWorldPos,const matrix4f* pViewMat,float fZValue);
void BASEGRAPH_SCENE_RENDER_API KeepModelSize(matrix4f* pMat,const matrix4f* pWorldPos,const matrix4f* pViewMat,float fZValue);
void BASEGRAPH_SCENE_RENDER_API KeepModelSize(matrix4f* pMat,int nCount,size_t offset,const matrix4f* pWorldPos,const matrix4f* pViewMat,float fZValue);

BOOL BASEGRAPH_SCENE_RENDER_API SpaceConvert(float3* pOutVec,			//目的向量
								  const float3* pInVec,			//源向量
								  const BaseGraphSpaceConvertParam* pParam,//空间状态
								  int nType,					//转换类型
								  const float*	fZValue = NULL,//转换类型为WD2DVEC_TO_3D_VIEWPLANE时，表示view plane到眼睛的距离
								  const float3* pPlaneNormal = NULL,//投影到平面上时，表示平面的法向量
								  const float3* pPlaneCenter = NULL);//投影到平面上时，表示平面的中心位置			
//场景坐标到buffer坐标的转换
//fCenterX,fCenterY 是场景中心点在Buffer中的位置
void BASEGRAPH_SCENE_RENDER_API SceneToBuffer(float3* pBufferPt,const float3* pScenePt,float fCenterX,float fCenterY,float fScale);
void BASEGRAPH_SCENE_RENDER_API BufferToScene(float3* pScenePt,const float3* pBufferPt,float fCenterX,float fCenterY,float fScale);
void BASEGRAPH_SCENE_RENDER_API SceneToBuffer(float2* pBufferPt, const float2* pScenePt, float fCenterX, float fCenterY, float fScale);
void BASEGRAPH_SCENE_RENDER_API BufferToScene(float2* pScenePt, const float2* pBufferPt, float fCenterX, float fCenterY, float fScale);
void BASEGRAPH_SCENE_RENDER_API SceneToBufferMat( matrix4f* pMat,float fCenterX,float fCenterY); 

void BASEGRAPH_SCENE_RENDER_API CalProjectMat( matrix4f* pOutProjMat,
								   float left,float right,float bottom,float top,	//平面的有效宽高
								   float zNear,float zFar ,BaseGraphPROJTYPE nType);

void BASEGRAPH_SCENE_RENDER_API CalViewMat(matrix4f* pOutViewMat,const float3* pUp,const float3* pEyePos,const float3* pLookAt);
void BASEGRAPH_SCENE_RENDER_API Cal2DViewProjMat(matrix4f* pOutViewMat,int nSceneWidth,int nSceneHeight);

void BASEGRAPH_SCENE_RENDER_API CalBillboardMatrix(matrix4f* pSrcMatrix, const matrix4f* pViewMat,const matrix4f* AdditionRotate = NULL);
void BASEGRAPH_SCENE_RENDER_API CalBillboardMatrix(matrix4f* pSrcMatrix, int nCount,size_t nOffset,const matrix4f* pWorldMat, const matrix4f* pViewMat);

void BASEGRAPH_SCENE_RENDER_API BaseGraphConvertBoxTo2DRect(rectf* pRect,const BaseGraphSpaceConvertParam* param,const float3* vMin,const float3* vMax);
void BASEGRAPH_SCENE_RENDER_API BaseGraphCalBoxNearFar(float* pNear,float* pFar,const matrix4f& matWorldView, const float3& vMin,const float3& vMax);


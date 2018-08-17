#pragma once
namespace BaseGraph_DataBase
{
	enum ITEM_FORMTYPE
	{
		ITEM_FORMTYPE_2D = 0,
		ITEM_FORMTYPE_2DIN3D,
		ITEM_FORMTYPE_3D,
		ITEM_FORMTYPE_3DIN2D,
		ITEM_FORMTYPE_LIGHT,
		ITEM_FORMTYPE_CAMERA,
		ITEM_FORMTYPE_2DRENDERBY3D,//以三维的方式渲染2D保持纵深效果出来
		ITEM_FORMTYPE_UNKNOW,
	};

	enum GRAPH_ART_PROC_TYPE
	{
		ARTNULL,             //无
		CONCAVO,			//凹
		CONVEX,				//凸
		BRIGHT_EDGE,		//亮边
		BEVAL_3D 			//立体边
	};

	struct metallight : public BaseGraph_Common::ValueAbleInterface
	{
		BOOL  enable;
		point3f pos;
		float strength[3];
		BASEGRAPH_DATABASE_API metallight();
		BASEGRAPH_DATABASE_API void Reset();
		BASEGRAPH_DATABASE_API virtual int GetSerializeSize() const;
		BASEGRAPH_DATABASE_API virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		BASEGRAPH_DATABASE_API virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct metalinfo : public BaseGraph_Common::ValueAbleInterface
	{
		DWORD		 highlight;
		BOOL         barc;
		float		 specular;
		DWORD		 width;//0-100
		DWORD		 height;//0-100
		metallight	 light[3];
		vector3f	 eyepos;
		BASEGRAPH_DATABASE_API metalinfo();
		BASEGRAPH_DATABASE_API void Reset();
		BASEGRAPH_DATABASE_API virtual int GetSerializeSize() const;
		BASEGRAPH_DATABASE_API virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		BASEGRAPH_DATABASE_API virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct artinfo : public BaseGraph_Common::ValueAbleInterface
	{
		GRAPH_ART_PROC_TYPE		type;		//效果类型
		DWORD					cv_height;   //凹凸高度       
		float 					cu_angle;	//凹凸角度
		int 					soft;		//虚边宽
		DWORD					color;		//凹凸单色,不透明
		metalinfo				metal;		//立体边
		BASEGRAPH_DATABASE_API artinfo();
		BASEGRAPH_DATABASE_API void Reset();
		BASEGRAPH_DATABASE_API virtual int GetSerializeSize() const;
		BASEGRAPH_DATABASE_API virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		BASEGRAPH_DATABASE_API virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct faceinfo : public BaseGraph_Common::ValueAbleInterface
	{
		int		disable;
		int		soft;
		int		effect_type;
		int		effect_width;
		DWORD	avcolor;
		artinfo art;
		common_color	color;
		BASEGRAPH_DATABASE_API faceinfo();
		BASEGRAPH_DATABASE_API void Reset();
		BASEGRAPH_DATABASE_API virtual int GetSerializeSize() const;
		BASEGRAPH_DATABASE_API virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		BASEGRAPH_DATABASE_API virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct borderinfo : public BaseGraph_Common::ValueAbleInterface
	{
		int		disable;
		int		soft;
		int		linestyle;
		float	width;
		float   interval;//点线的间距
		DWORD	effect_type;
		int		effect_width;
		DWORD	avcolor;
		artinfo	art;
		common_color	color;
		BASEGRAPH_DATABASE_API borderinfo();
		BASEGRAPH_DATABASE_API void Reset();
		BASEGRAPH_DATABASE_API virtual int GetSerializeSize() const;
		BASEGRAPH_DATABASE_API virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		BASEGRAPH_DATABASE_API virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct shadowinfo : public BaseGraph_Common::ValueAbleInterface
	{
		///注意溢出
		int		disable;
		int 	soft;
		int		offsetx;
		int		offsety;
		int	    effect_type;

		float	effect_fxtitle;
		float	effect_fyscale;
		float	effect_fadeout;
		DWORD	effect_style;
		DWORD	avcolor;
		artinfo  art;
		common_color	color;
		BASEGRAPH_DATABASE_API shadowinfo();
		BASEGRAPH_DATABASE_API void Reset();
		BASEGRAPH_DATABASE_API virtual int GetSerializeSize() const;
		BASEGRAPH_DATABASE_API virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		BASEGRAPH_DATABASE_API virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct cubeinfo : public BaseGraph_Common::ValueAbleInterface
	{
		int		disable;
		float	width;
		float	angle;
		int		effect_type;
		int		  effect_width;
		unsigned long	avcolor;
		artinfo	art;
		common_color	color;
		BASEGRAPH_DATABASE_API cubeinfo();
		BASEGRAPH_DATABASE_API void Reset();
		BASEGRAPH_DATABASE_API virtual int GetSerializeSize() const;
		BASEGRAPH_DATABASE_API virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		BASEGRAPH_DATABASE_API virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct lightinfo : public BaseGraph_Common::ValueAbleInterface
	{
		DWORD ambience;	//环镜单色,不透明
		DWORD ambience_degree;  //明亮程度1-100
		DWORD reflectcolor;		//反射单色,不透明
		DWORD dwReverse1;
		DWORD dwReverse2;
		TCHAR szPath[512];
		BASEGRAPH_DATABASE_API lightinfo();
		BASEGRAPH_DATABASE_API void Reset();
		BASEGRAPH_DATABASE_API virtual int GetSerializeSize() const;
		BASEGRAPH_DATABASE_API virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		BASEGRAPH_DATABASE_API virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct channelinfo : public BaseGraph_Common::ValueAbleInterface
	{
		TCHAR szPath[512];
		int   weight;
		int	  channelid;
		DWORD dwReverse1;
		DWORD dwReverse2;
		BASEGRAPH_DATABASE_API channelinfo();
		BASEGRAPH_DATABASE_API void Reset();
		BASEGRAPH_DATABASE_API virtual int GetSerializeSize() const;
		BASEGRAPH_DATABASE_API virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		BASEGRAPH_DATABASE_API virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct materialinfo : public BaseGraph_Common::ValueAbleInterface
	{
		TCHAR szPath[512];
		DWORD dwReverse1;
		DWORD dwReverse2;
		BASEGRAPH_DATABASE_API materialinfo();
		BASEGRAPH_DATABASE_API void Reset();
		BASEGRAPH_DATABASE_API virtual int GetSerializeSize() const;
		BASEGRAPH_DATABASE_API virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		BASEGRAPH_DATABASE_API virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct dynamictexture : public BaseGraph_Common::ValueAbleInterface
	{
		TCHAR szPath[512];
		BOOL  disable;
		float speed;
		float offsetx;
		float offsety;
		float vRunDirx;		// 以此表示方向角度，Y暂不用
		float vRunDiry;
		float scalex;
		float scaley;
		float centerx;
		float centery;
		float rotate;
		float alpha;
		BOOL  bRepeat;
		BOOL  bUseAlpha;
		BASEGRAPH_DATABASE_API dynamictexture();
		BASEGRAPH_DATABASE_API void Reset();
		BASEGRAPH_DATABASE_API virtual int GetSerializeSize() const;
		BASEGRAPH_DATABASE_API virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		BASEGRAPH_DATABASE_API virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};
}

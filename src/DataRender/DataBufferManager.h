#pragma once
#include <map>
#include "ObjectBuffer.h"

class DataBufferManager
{
private:
	int m_nWidthSeed;
	int m_nHeightSeed;
	std::map<int, ObjectBuffer *> m_map2DBuffer;
private:
	DataBufferManager();
	~DataBufferManager();
protected:
	int GetNextID();
	int CreateBuffer(int nWidth, int nHeight);
public:
	static DataBufferManager & Instance();
	//2D Buffer操作
	void InitialSeed(int nWidthSeed, int nHeightSeed);//种子是创建内存块时的基础大小，新创建的内存块的宽高一定是种子宽高的整数倍
	int Query2DBuffer(int nWidth, int nHeight);//取一个2Dbuf
	bool UnQuery2DBuffer(int nBufferID);//释放一个2Dbuf的使用权（不释放）
	bool Compress2DBuffer();//压缩2Dbuf（释放掉所有未使用的bufer）
	//3D Buffer操作
	//...
};


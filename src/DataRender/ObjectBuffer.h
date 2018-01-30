#pragma once
/*
一块buffer，buffer需要保证自己的reserve size大于等于使用size
当前fit的要求是宽高大于等于实际要求的宽高，但面积小于等于实际要求面积的2倍
*/

class ObjectBuffer
{
private:
	//实际使用的宽高（buffer被使用的）
	int m_nWidth;
	int m_nHeight;
	//保留的宽高（buffer实际大小）
	int m_nReserveWidth;
	int m_nReserveHeight;
	//buffer是否被占用
	bool m_bUsed;
	//buffer指针
	byte * m_pBuffer;
public:
	ObjectBuffer();
	~ObjectBuffer();
public:
	bool InitializeBuffer(int nWidth, int nHeight);
	void ReleaseBuffer();

	bool SetUsed(int nWidth, int nHeight);
	void ReleaseUse();
	bool IsUsed();

	bool FitIn(int nReqWidth, int nReqHeight);

	byte * GetBuffer();
	int GetWidth();
	int GetHeight();
};


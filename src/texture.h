
#pragma once
#include<string>
#include "matrix.h"

#include "win.h"

class texture
{
public:
	texture();
	~texture();

	void Bind(unsigned int slot = 0) const; // optional par. spcify slot
	void Unbind() const;


	void data_to_gpu(Matrix3D* myVec3D2);

	inline int GetWidth() const {return m_Width;}
	inline int GetHeight() const {return m_Height;}



//private:
	unsigned int m_RendererID;

	unsigned char* m_LocalBuffer;

	int m_Width, m_Height, m_BPP;
};
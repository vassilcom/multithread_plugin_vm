
#include "texture.h"

texture::texture()
	: m_RendererID(0), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
{

	Matrix3D* myVec3D2 = matrix3D_create(6, 6, 4);
	m_Width = myVec3D2->cols ;
	m_Height = myVec3D2->rows;
	m_BPP = myVec3D2->chan ;

	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

	// give opengl data
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, myVec3D2->flttend3D);
	data_to_gpu(myVec3D2);
	glBindTexture(GL_TEXTURE_2D, 0);

	matrix3D_free(myVec3D2);
}

void texture::data_to_gpu(Matrix3D* myVec3D2)
{
	m_Width = myVec3D2->cols ;
	m_Height = myVec3D2->rows;
	m_BPP = myVec3D2->chan ;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, myVec3D2->flttend3D);
}
texture::~texture()
{

	glDeleteTextures(1, &m_RendererID);
}

void texture::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

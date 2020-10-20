#ifndef GFLIB2_GRX_WINOpenGL_WINOpenGLTEXTURE_H_INCLUDED
#define GFLIB2_GRX_WINOpenGL_WINOpenGLTEXTURE_H_INCLUDED

#include <gfx/include/gfl2_texture.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTypes.h>

namespace gfl2 { namespace gfx { namespace winOpenGL {

class WinOpenGLGL;

class WinOpenGLTexture : public Texture
{
public:

	friend class WinOpenGLGL;

	WinOpenGLTexture( const TextureDesc &rDesc );
	virtual ~WinOpenGLTexture();

	//! @brief テクスチャを更新する
	virtual void Update( const void* pData,	u32 depth = 0 );

	const GLuint GetHandle() const { return m_hTextureObject; }

	virtual void* Lock();
	virtual void UnLock();

private:

	GLuint					m_hTextureObject;
  void*           m_pLockedData;

};

}}}

#endif
#ifndef GFLIB2_GRX_WINOpenGL_WINOpenGLSURFACE_H_INCLUDED
#define GFLIB2_GRX_WINOpenGL_WINOpenGLSURFACE_H_INCLUDED

#include <gfx/include/gfl2_Surface.h>
#include <util/include/gfl2_AutoComPtr.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTypes.h>

namespace gfl2 { namespace gfx { namespace winOpenGL {

class WinOpenGLSurface : public Surface
{
public:

	WinOpenGLSurface( const SurfaceDesc &rDesc );
	WinOpenGLSurface( const SurfaceDesc &rDesc, GLuint handle, b32 isTexture );
	virtual ~WinOpenGLSurface();

	const GLuint GetHandle() const { return m_Handle; }

	const b32 IsTexture() const { return m_IsTexture; }

	void SetSetTargetSlotNo( u32 no ) { m_SetTargetSlotNo = no; }
	u32 GetSetTargetSlotNo() const { return m_SetTargetSlotNo; }


private:

	GLuint						m_Handle;
	b32								m_fOwnership;
	b32								m_IsTexture;
	u32								m_SetTargetSlotNo;

};

}}}

#endif
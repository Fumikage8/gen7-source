#ifndef GFLIB2_GRX_CTR_CTRSURFACE_H_INCLUDED
#define GFLIB2_GRX_CTR_CTRSURFACE_H_INCLUDED

#include <gfx/include/gfl2_Surface.h>
#include <util/include/gfl2_AutoComPtr.h>
#include <gfx/include/CTR/gfl2_CTRTypes.h>

namespace gfl2 { namespace gfx { namespace ctr {

class CTRSurface : public Surface
{
public:

	CTRSurface( const SurfaceDesc &rDesc, void* pRefAddr = NULL );
	//CTRSurface( const SurfaceDesc &rDesc, GLuint handle, b32 isTexture );
	virtual ~CTRSurface();

	const nn::gr::FrameBuffer& GetHandle() const { return m_Handle; }

	const b32 IsTexture() const { return m_IsTexture; }


private:

  nn::gr::FrameBuffer m_Handle;
	b32								m_fOwnership;
	b32								m_IsTexture;

};

}}}

#endif

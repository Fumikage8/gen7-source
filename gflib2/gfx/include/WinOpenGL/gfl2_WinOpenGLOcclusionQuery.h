#ifndef GFLIB2_GRX_WINOPENGL_OCCLUSIONQUERY_H_INCLUDED
#define GFLIB2_GRX_WINOPENGL_OCCLUSIONQUERY_H_INCLUDED

#include <gfx/include/gfl2_OcclusionQuery.h>
#include <util/include/gfl2_AutoComPtr.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTypes.h>

namespace gfl2 { namespace gfx { namespace winOpenGL {

class WinOpenGLOcclusionQuery : public OcclusionQuery
{
public:

	WinOpenGLOcclusionQuery();
	virtual ~WinOpenGLOcclusionQuery();

private:
	
};

}}}

#endif

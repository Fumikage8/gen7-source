#ifndef GFLIB2_GRX_WINOPENGL_OCCLUSIONQUERY_H_INCLUDED
#define GFLIB2_GRX_WINOPENGL_OCCLUSIONQUERY_H_INCLUDED

#include <gfx/include/gfl2_OcclusionQuery.h>
#include <util/include/gfl2_AutoComPtr.h>
#include <gfx/include/WinDX11/gfl2_WinDX11Types.h>

namespace gfl2 { namespace gfx { namespace windx11 {

class WinDX11OcclusionQuery : public OcclusionQuery
{
public:

	WinDX11OcclusionQuery();
	virtual ~WinDX11OcclusionQuery();

private:
	
};

}}}

#endif

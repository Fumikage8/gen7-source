#ifndef GFLIB2_GRX_CLR_SURFACE_CLR_H_INCLUDED
#define GFLIB2_GRX_CLR_SURFACE_CLR_H_INCLUDED

#using <System.Drawing.dll>

#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace clr { namespace gfx {

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■		
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
public ref class Surface{
public:

	Surface( gfl2::gfx::Surface *native, bool f ){ m_pNative = native; m_fReference = f; }
	~Surface(){this->!Surface();}
	!Surface();

	gfl2::gfx::Surface* GetNative(){ return m_pNative; }

private:

	gfl2::gfx::Surface			*m_pNative;
	bool										m_fReference;
};


}}}

#endif
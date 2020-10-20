#ifndef GFLIB2_GRX_CLR_COLOR_CLR_H_INCLUDED
#define GFLIB2_GRX_CLR_COLOR_CLR_H_INCLUDED

#using <System.Drawing.dll>

#include <gfx/include/gfl2_Color.h>

namespace gfl2 { namespace clr { namespace gfx {

public ref class Color{
public:

	Color();
	Color( const float r, const float g, const float b, const float a );
	~Color(){this->!Color();}
	!Color();

	gfl2::gfx::Color* GetNative(){ return m_pNative; }

private:

	gfl2::gfx::Color			*m_pNative;
};


}}}

#endif
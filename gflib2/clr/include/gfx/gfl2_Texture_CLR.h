#ifndef GFLIB2_GRX_CLR_TEXTURE_CLR_H_INCLUDED
#define GFLIB2_GRX_CLR_TEXTURE_CLR_H_INCLUDED

#using <System.Drawing.dll>

#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace clr { namespace gfx {

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■		
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
public ref class Texture{
public:

	Texture( gfl2::gfx::Texture *native ){ m_pNative = native; }
	~Texture(){this->!Texture();}
	!Texture();

	gfl2::gfx::Texture* GetNative(){ return m_pNative; }

private:

	gfl2::gfx::Texture			*m_pNative;
};


}}}

#endif
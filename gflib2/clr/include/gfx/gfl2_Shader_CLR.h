#ifndef GFLIB2_GRX_CLR_SHADER_CLR_H_INCLUDED
#define GFLIB2_GRX_CLR_SHADER_CLR_H_INCLUDED

#using <System.Drawing.dll>

#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace clr { namespace gfx {

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■		
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
public ref class Shader{
public:

	Shader( gfl2::gfx::Shader *pNative ){ m_pNative = pNative; }
	~Shader(){this->!Shader();}
	!Shader();

	gfl2::gfx::Shader* GetNative(){ return m_pNative; }

private:

	gfl2::gfx::Shader			*m_pNative;
};


}}}

#endif
#ifndef GFLIB2_GRX_SHADER_H_INCLUDED
#define GFLIB2_GRX_SHADER_H_INCLUDED

#include <stdio.h>
#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_types.h>
#include <gfx/include/gfl2_GLMemory.h>

namespace gfl2 { namespace gfx {

template <class Implement_>
class GFGLBase;

class Shader : public GLMemory
{
public:
	template <class Implement_>
	friend class GFGLBase;

	virtual ~Shader(){}

protected:
	Shader(){}
	virtual void Bind() const {}
	
};

}}

#endif

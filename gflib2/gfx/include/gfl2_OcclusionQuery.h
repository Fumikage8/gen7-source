#ifndef GFLIB2_GRX_OCCLUSIONQUERY_H_INCLUDED
#define GFLIB2_GRX_OCCLUSIONQUERY_H_INCLUDED

#include <stdio.h>
#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_types.h>
#include <gfx/include/gfl2_GLMemory.h>
#include <gfx/include/gfl2_QueryObject.h>

namespace gfl2 { namespace gfx {

template <class Implement_>
class GFGLBase;

class OcclusionQuery : public QueryObject
{
public:

	template <class Implement_>
	friend class GFGLBase;

	virtual ~OcclusionQuery(){}

protected:
	OcclusionQuery(){}
};

}}

#endif

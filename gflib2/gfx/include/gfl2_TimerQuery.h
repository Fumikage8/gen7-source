#ifndef GFLIB2_GRX_TIMERQUERY_H_INCLUDED
#define GFLIB2_GRX_TIMERQUERY_H_INCLUDED

#include <stdio.h>
#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_types.h>
#include <gfx/include/gfl2_GLMemory.h>
#include <gfx/include/gfl2_QueryObject.h>

namespace gfl2 { namespace gfx {

template <class Implement_>
class GFGLBase;

class TimerQuery : public QueryObject
{
public:

	template <class Implement_>
	friend class GFGLBase;

	virtual ~TimerQuery(){}

  virtual u64 GetGPUElapsedTimeMicroSeconds() = 0;
  virtual u64 GetCPUElapsedTimeMicroSeconds() = 0;

protected:
	TimerQuery(){}
};

}}

#endif

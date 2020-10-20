#ifndef GFLIB2_GRX_WINOPENGL_QTIMERQUERY_H_INCLUDED
#define GFLIB2_GRX_WINOPENGL_QTIMERQUERY_H_INCLUDED

#include <gfx/include/gfl2_TimerQuery.h>
#include <util/include/gfl2_AutoComPtr.h>
#include <gfx/include/WinDx11/gfl2_WinDX11Types.h>

namespace gfl2 { namespace gfx { namespace windx11 {

class WinDX11TimerQuery : public TimerQuery
{
public:

	WinDX11TimerQuery();
	virtual ~WinDX11TimerQuery();

  void Begin();
  void End();

  virtual u64 GetGPUElapsedTimeMicroSeconds();
  virtual u64 GetCPUElapsedTimeMicroSeconds();

private:

};

}}}

#endif

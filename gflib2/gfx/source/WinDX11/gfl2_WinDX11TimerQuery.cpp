#include <gfx/include/WinDx11/gfl2_WinDX11Types.h>
#include <gfx/include/WinDx11//gfl2_WinDX11TimerQuery.h>
#include <gfx/include/gfl2_GFGL.h>

#include <system/include/Timer/gfl2_PerformanceCounter.h>

namespace gfl2 { namespace gfx { namespace windx11 {

WinDX11TimerQuery::WinDX11TimerQuery() :
  TimerQuery()
{
}

WinDX11TimerQuery::~WinDX11TimerQuery()
{
}

void WinDX11TimerQuery::Begin()
{
}

void WinDX11TimerQuery::End()
{
}

u64 WinDX11TimerQuery::GetGPUElapsedTimeMicroSeconds()
{
  return 0;
}

u64 WinDX11TimerQuery::GetCPUElapsedTimeMicroSeconds()
{
  return 0;
}

}}}

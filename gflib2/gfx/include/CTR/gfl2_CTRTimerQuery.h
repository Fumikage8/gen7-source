#ifndef GFLIB2_GRX_CTR_CTRQTIMERQUERY_H_INCLUDED
#define GFLIB2_GRX_CTR_CTRQTIMERQUERY_H_INCLUDED

#include <gfx/include/gfl2_TimerQuery.h>
#include <util/include/gfl2_AutoComPtr.h>
#include <gfx/include/CTR/gfl2_CTRTypes.h>

namespace gfl2 { namespace gfx { namespace ctr {

class CTRCommandList;
class CTRGPUProfiler;

class CTRTimerQuery : public TimerQuery
{
public:

	CTRTimerQuery();
	virtual ~CTRTimerQuery();

  void Begin(CTRCommandList* pCommandList, CTRGPUProfiler* pGPUProfiler);
  void End(CTRCommandList* pCommandList, CTRGPUProfiler* pGPUProfiler);
  void GPUBeginCallback(s32 microSeconds);
  void GPUEndCallback(s32 microSeconds);

  virtual u64 GetGPUElapsedTimeMicroSeconds();
  virtual u64 GetCPUElapsedTimeMicroSeconds();

private:

  CTRCommandList* m_pCommandList;

  s32 m_CpuElapsedTime;
  s32 m_GpuElapsedTime;

  u64 m_CPUBeginTimeMicroSeconds;
  u64 m_GPUBeginTimeMicroSeconds;
};

}}}

#endif

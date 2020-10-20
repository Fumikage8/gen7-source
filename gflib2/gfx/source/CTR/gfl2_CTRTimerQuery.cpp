#include <gfx/include/CTR/gfl2_CTRTypes.h>
#include <gfx/include/CTR/gfl2_CTRTimerQuery.h>
#include <gfx/include/gfl2_GFGL.h>

#include <gfx/include/CTR/gfl2_CTRCommandList.h>
#include <gfx/include/CTR/gfl2_CTRGPUProfiler.h>

namespace gfl2 { namespace gfx { namespace ctr {

CTRTimerQuery::CTRTimerQuery() :
  TimerQuery(),
  m_pCommandList(NULL),
  m_CpuElapsedTime(0),
  m_GpuElapsedTime(0),
  m_CPUBeginTimeMicroSeconds(0),
  m_GPUBeginTimeMicroSeconds(0)
{
	
}

CTRTimerQuery::~CTRTimerQuery()
{
	
}

void CTRTimerQuery::Begin(CTRCommandList* pCommandList, CTRGPUProfiler* pGPUProfiler)
{
  m_CPUBeginTimeMicroSeconds = CTRGPUProfiler::GetCurrentMicroSeconds();

  pGPUProfiler->SetTimerQueryBeginCallback(this, pCommandList);
}

void CTRTimerQuery::End(CTRCommandList* pCommandList, CTRGPUProfiler* pGPUProfiler)
{
  m_CpuElapsedTime = CTRGPUProfiler::GetCurrentMicroSeconds() - m_CPUBeginTimeMicroSeconds;

  pGPUProfiler->SetTimerQueryEndCallback(this, pCommandList);
}

void CTRTimerQuery::GPUBeginCallback(s32 microSeconds)
{
  m_GPUBeginTimeMicroSeconds = microSeconds;
}

void CTRTimerQuery::GPUEndCallback(s32 microSeconds)
{
  m_GpuElapsedTime = microSeconds - m_GPUBeginTimeMicroSeconds;
}

u64 CTRTimerQuery::GetGPUElapsedTimeMicroSeconds()
{
  return m_GpuElapsedTime;
}

u64 CTRTimerQuery::GetCPUElapsedTimeMicroSeconds()
{
  return m_CpuElapsedTime;
}

}}}

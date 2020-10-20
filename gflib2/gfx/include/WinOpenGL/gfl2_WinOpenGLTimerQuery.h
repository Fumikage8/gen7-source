#ifndef GFLIB2_GRX_WINOPENGL_QTIMERQUERY_H_INCLUDED
#define GFLIB2_GRX_WINOPENGL_QTIMERQUERY_H_INCLUDED

#include <gfx/include/gfl2_TimerQuery.h>
#include <util/include/gfl2_AutoComPtr.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTypes.h>

namespace gfl2 { namespace gfx { namespace winOpenGL {

class WinOpenGLTimerQuery : public TimerQuery
{
public:

	WinOpenGLTimerQuery();
	virtual ~WinOpenGLTimerQuery();

  void Begin();
  void End();

  virtual u64 GetGPUElapsedTimeMicroSeconds();
  virtual u64 GetCPUElapsedTimeMicroSeconds();

private:

  struct QueryList
  {
    GLuint queryNames[2];
  };

  u64 m_CPUStartTimeMicroSeconds;
  u64 m_GPUStartTimeMicroSeconds;

  u64 m_CPUElapsedTimeMicroSeconds;
  u64 m_GPUElapsedTimeMicroSeconds;

  bool m_IsFirstQueryDone;
  bool m_IsBeginQuery;

  QueryList m_QueryLists[2];
  u32 m_CurrentQueryNo;
};

}}}

#endif

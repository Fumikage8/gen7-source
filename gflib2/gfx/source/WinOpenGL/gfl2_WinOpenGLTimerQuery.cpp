#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTypes.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTimerQuery.h>
#include <gfx/include/gfl2_GFGL.h>

#include <system/include/Timer/gfl2_PerformanceCounter.h>

namespace gfl2 { namespace gfx { namespace winOpenGL {

WinOpenGLTimerQuery::WinOpenGLTimerQuery() :
  TimerQuery(),
  m_CPUStartTimeMicroSeconds(0),
  m_GPUStartTimeMicroSeconds(0),
  m_IsFirstQueryDone(false),
  m_IsBeginQuery(false),
  m_CurrentQueryNo(0)
{
  // クエリオブジェクト作成
  glGenQueries(2, m_QueryLists[0].queryNames);
  glGenQueries(2, m_QueryLists[1].queryNames);
}

WinOpenGLTimerQuery::~WinOpenGLTimerQuery()
{
  // クエリオブジェクト削除
  glDeleteQueries(2, m_QueryLists[0].queryNames);
  glDeleteQueries(2, m_QueryLists[1].queryNames);
}

void WinOpenGLTimerQuery::Begin()
{
  m_IsBeginQuery = true;

  // CPU計測開始
  m_CPUStartTimeMicroSeconds = system::Timer::PerformanceCounter::GetMicroSecond();

  // GPU計測開始
  glQueryCounter(m_QueryLists[m_CurrentQueryNo].queryNames[0], GL_TIMESTAMP);
}

void WinOpenGLTimerQuery::End()
{
  // 計測開始していなければ無視
  if (!m_IsBeginQuery)
  {
    return;
  }
  m_IsBeginQuery = false;
  m_IsFirstQueryDone = true;

  // GPU計測終了
  glQueryCounter(m_QueryLists[m_CurrentQueryNo].queryNames[1], GL_TIMESTAMP);

  // CPU計測終了
  m_CPUElapsedTimeMicroSeconds = system::Timer::PerformanceCounter::GetMicroSecond() - m_CPUStartTimeMicroSeconds;

  // 切り替え
  m_CurrentQueryNo ^= 1;
}

u64 WinOpenGLTimerQuery::GetGPUElapsedTimeMicroSeconds()
{
  if (!m_IsFirstQueryDone)
  {
    return 0;
  }

  QueryList* pOld = &m_QueryLists[m_CurrentQueryNo ^ 1];

  // クエリ取得できるか？
  GLint done = 0;
  glGetQueryObjectiv(pOld->queryNames[1], GL_QUERY_RESULT_AVAILABLE, &done);
  while (!done)
  {
    glGetQueryObjectiv(pOld->queryNames[1], GL_QUERY_RESULT_AVAILABLE, &done);
    Sleep(0);
  } 

  // クエリ取得
  GLuint64 begin = 0;
  GLuint64 end = 0;
  
  glGetQueryObjectui64v(pOld->queryNames[0], GL_QUERY_RESULT, &begin);
  glGetQueryObjectui64v(pOld->queryNames[1], GL_QUERY_RESULT, &end);

  m_GPUElapsedTimeMicroSeconds = (end - begin) / 1000;
  return m_GPUElapsedTimeMicroSeconds;
}

u64 WinOpenGLTimerQuery::GetCPUElapsedTimeMicroSeconds()
{
  return m_CPUElapsedTimeMicroSeconds;
}

}}}

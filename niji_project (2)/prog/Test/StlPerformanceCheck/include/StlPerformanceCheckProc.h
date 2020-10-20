// ============================================================================
/*
 * @file StlPerformanceCheckProc.h
 * @brief stl と gfl の テンプレートライブラリのパフォーマンスを比較します
 * @date 2015.03.03
 */
// ============================================================================
#if PM_DEBUG

#if !defined( STLPERFORMANCECHECKPROC_H_INCLUDED )
#define STLPERFORMANCECHECKPROC_H_INCLUDED
#pragma once

#include <macro/include/gfl2_macros.h>
#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_Font.h>
#include <util/include/gfl2_UtilTextRenderingPipeLine.h>
#include <GameSys/include/GameProc.h>
#include <util/include/gfl2_Map.h>


GFL_NAMESPACE_BEGIN(System)
class nijiAllocator;
GFL_NAMESPACE_END(System)

  
  GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(StlPerformanceCheck)


class StlPerformanceCheckProc : public GameSys::GameProc
{
	GFL_FORBID_COPY_AND_ASSIGN( StlPerformanceCheckProc );

public:
  StlPerformanceCheckProc();
  virtual~StlPerformanceCheckProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pManager );

private:
  void DisplayPrint();

  void StlMapInsertPerformance();
  void GflMapInsertPerformance();
  void StlMapFindPerformance();
  void GflMapFindPerformance();
  void StlMapIteratorPerformance();
  void GflMapIteratorPerformance();
  void StlMapErasePerformance();
  void GflMapErasePerformance();
  void StlListPushBackPerformance();
  void GflListPushBackPerformance();
  void StlListPopBackPerformance();
  void GflListPopBackPerformance();

  void gflMapDebugPrint( gfl2::util::Map<u32, u32>& gfl_map );

private:
  gfl2::heap::NwAllocator*		                      m_pMemAllocator;
	gfl2::heap::NwAllocator*		                      m_pDevAllocator;
  System::nijiAllocator*                            m_pNijiAllocator;
	gfl2::util::UtilTextRenderingPipeLine*		        m_pRenderingPipeLine;
  gfl2::str::ResFont*				                        m_pResFont;
	gfl2::util::DrawUtilText*                         m_pDrawUtilText;

  u32                                               m_DrawFlag;

  s64                                               m_StlMapInsertTime;
  s64                                               m_GflMapInsertTime;
  s64                                               m_StlMapFindTime;
  s64                                               m_GflMapFindTime;
  s64                                               m_StlMapIteratorTime;
  s64                                               m_GflMapIteratorTime;
  s64                                               m_StlMapEraseTime;
  s64                                               m_GflMapEraseTime;

  s64                                               m_StlListPushBackTime;
  s64                                               m_GflListPushBackTime;
  s64                                               m_StlListPopBackTime;
  s64                                               m_GflListPopBackTime;

};


GFL_NAMESPACE_END(StlPerformanceCheck)
GFL_NAMESPACE_END(Test)

#endif // STLPERFORMANCECHECKPROC_H_INCLUDED

#endif // PM_DEBUG

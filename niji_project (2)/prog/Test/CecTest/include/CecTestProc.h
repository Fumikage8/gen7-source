// ============================================================================
/*
 * @file CecTestProc.h
 * @brief すれ違い通信のテストです。
 * @date 2015.03.03
 */
// ============================================================================
#if PM_DEBUG

#if !defined( CECTESTPROC_H_INCLUDED )
#define CECTESTPROC_H_INCLUDED
#pragma once


#include <macro/include/gfl2_macros.h>
#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_Font.h>
#include <util/include/gfl2_UtilTextRenderingPipeLine.h>
#include <GameSys/include/GameProc.h>
#include <gflnet2/include/cec/gflnet2_ICecSystem.h>

GFL_NAMESPACE_BEGIN(System)
class nijiAllocator;
GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(CecTest)


class CecTestStateAccessor;
class CecTestMessageBoxEventListener;

typedef struct
{
  int testData1;
  int testData2;
  int testData3;
} CecSendData;

class CecTestProc : public GameSys::GameProc
{
	GFL_FORBID_COPY_AND_ASSIGN( CecTestProc );

public:
  CecTestProc();
  virtual~CecTestProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pManager );

private:
  bool UpdateCec();
  void DisplayPrint();

private:
  Test::CecTest::CecTestStateAccessor*              m_pStateAccessor;
  Test::CecTest::CecTestMessageBoxEventListener*    m_pMessageBoxEventListener;
  gflnet2::cec::ICecSystem*                         m_pCecSystem;

  gfl2::heap::NwAllocator*		                      m_pMemAllocator;
	gfl2::heap::NwAllocator*		                      m_pDevAllocator;
  System::nijiAllocator*                            m_pNijiAllocator;
	gfl2::util::UtilTextRenderingPipeLine*		        m_pRenderingPipeLine;
  gfl2::str::ResFont*				                        m_pResFont;
	gfl2::util::DrawUtilText*                         m_pDrawUtilText;

};


GFL_NAMESPACE_END(CecTest)
GFL_NAMESPACE_END(Test)

#endif // CECTESTPROC_H_INCLUDED

#endif // PM_DEBUG

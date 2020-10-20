// ============================================================================
/*
 * @file EndRollFrameFactory.cpp
 * @brief エンドロールアプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2016.03.16
 * @author uchida_yuto
 */
// ============================================================================
#include "App/EndRoll/source/System/EndRollFrameFactory.h"
#include "App/EndRoll/source/EndRollFrameID.h"
#include "App/EndRoll/source/Frame/EndRollFrame.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(EndRoll)
GFL_NAMESPACE_BEGIN(System)


EndRollFrameFactory::EndRollFrameFactory( App::EndRoll::System::EndRollWork* pEndRollWork ) :
  m_pEndRollWork( pEndRollWork )
{
}


NetAppLib::System::NetAppFrameBase* EndRollFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
{
  app::util::Heap* pAppHeap = m_pEndRollWork->GetAppHeap();

  NetAppLib::System::NetAppFrameBase* pFrame = NULL;

  switch( frameID )
  {
    case App::EndRoll::APPENDROLL_FRAME_ID_NORMAL:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::EndRoll::Frame::EndRollFrame( m_pEndRollWork );
    break;
  }

  return pFrame;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(EndRoll)
GFL_NAMESPACE_END(App)

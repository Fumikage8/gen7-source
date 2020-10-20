// ============================================================================
/*
 * @file SuperTrainingFrameFactory.cpp
 * @brief すごい特訓アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.12.04
 * @author uchida_yuto
 */
// ============================================================================
#include "App/SuperTraining/source/System/SuperTrainingFrameFactory.h"
#include "App/SuperTraining/source/SuperTrainingFrameID.h"
#include "App/SuperTraining/source/Frame/SuperTrainingFrame.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(SuperTraining)
GFL_NAMESPACE_BEGIN(System)


  SuperTrainingFrameFactory::SuperTrainingFrameFactory( App::SuperTraining::System::SuperTrainingWork* pSuperTrainingWork ) :
    m_pSuperTrainingWork( pSuperTrainingWork )
  {
  }



  NetAppLib::System::NetAppFrameBase* SuperTrainingFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
  {
    app::util::Heap* pAppHeap = m_pSuperTrainingWork->GetAppHeap();

    NetAppLib::System::NetAppFrameBase* pFrame = NULL;

    switch( frameID )
    {
      case App::SuperTraining::SUPER_TRAINIG_FRAME_ID_MAIN:
        pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::SuperTraining::Frame::SuperTrainingFrame( m_pSuperTrainingWork );
      break;
    }

    return pFrame;
  }


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(SuperTraining)
GFL_NAMESPACE_END(App)

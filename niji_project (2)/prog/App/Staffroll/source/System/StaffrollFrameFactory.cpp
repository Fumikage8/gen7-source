// ============================================================================
/*
 * @file StaffrollFrameFactory.cpp
 * @brief スタッフロールアプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2016.01.06
 * @author uchida_yuto
 */
// ============================================================================
#include "App/Staffroll/source/System/StaffrollFrameFactory.h"
#include "App/Staffroll/source/StaffrollFrameID.h"
#include "App/Staffroll/source/Frame/StaffrollEventFrame.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Staffroll)
GFL_NAMESPACE_BEGIN(System)


StaffrollFrameFactory::StaffrollFrameFactory( App::Staffroll::System::StaffrollWork* pStaffrollWork ) :
  m_pStaffrollWork( pStaffrollWork )
{
}

NetAppLib::System::NetAppFrameBase* StaffrollFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
{
  app::util::Heap* pAppHeap = m_pStaffrollWork->GetAppHeap();

  NetAppLib::System::NetAppFrameBase* pFrame = NULL;

  switch( frameID )
  {
    case App::Staffroll::STAFFROLL_FRAME_ID_EVENT:
    {
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::Staffroll::Frame::StaffrollEventFrame( m_pStaffrollWork );
      break;
    }
  }

  return pFrame;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Staffroll)
GFL_NAMESPACE_END(App)

// ============================================================================
/*
 * @file JoinFestaSelectWordFrameFactory.cpp
 * @brief 挨拶選択アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordFrameFactory.h"
#include "NetApp/JoinFestaSelectWord/source/JoinFestaSelectWordFrameID.h"
#include "NetApp/JoinFestaSelectWord/source/Frame/JoinFestaSelectWordMainFrame.h"
#include "NetApp/JoinFestaSelectWord/source/Frame/JoinFestaSelectWordDescriptionFrame.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(System)


JoinFestaSelectWordFrameFactory::JoinFestaSelectWordFrameFactory( NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork* pJoinFestaSelectWordWork, NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewSystemAccessor* pJoinFestaSelectWordViewSystemAccessor ) :
  m_pJoinFestaSelectWordWork( pJoinFestaSelectWordWork ),
  m_pJoinFestaSelectWordViewSystemAccessor( pJoinFestaSelectWordViewSystemAccessor )
{
}


JoinFestaSelectWordFrameFactory::~JoinFestaSelectWordFrameFactory()
{
}


NetAppLib::System::NetAppFrameBase* JoinFestaSelectWordFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
{
  gfl2::heap::HeapBase* pDeviceHeap = m_pJoinFestaSelectWordWork->GetDeviceHeap();

  NetAppLib::System::NetAppFrameBase* pFrame = NULL;

  switch( frameID )
  {
    case NetApp::JoinFestaSelectWord::JOINFESTASELECTWORD_FRAME_ID_MAIN:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::JoinFestaSelectWord::Frame::JoinFestaSelectWordMainFrame( m_pJoinFestaSelectWordWork, m_pJoinFestaSelectWordViewSystemAccessor );
    break;

    case NetApp::JoinFestaSelectWord::JOINFESTASELECTWORD_FRAME_ID_DESCRIPTION:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::JoinFestaSelectWord::Frame::JoinFestaSelectWordDescriptionFrame( m_pJoinFestaSelectWordWork, m_pJoinFestaSelectWordViewSystemAccessor );
    break;
  }

  return pFrame;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)

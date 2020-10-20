// ============================================================================
/*
 * @file JoinFestaAttractionResultFrameFactory.cpp
 * @brief アトラクション結果アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.11.02
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaAttractionResult/source/System/JoinFestaAttractionResultFrameFactory.h"
#include "NetApp/JoinFestaAttractionResult/source/JoinFestaAttractionResultFrameID.h"
#include "NetApp/JoinFestaAttractionResult/source/Frame/JoinFestaAttractionResultFrame.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)
GFL_NAMESPACE_BEGIN(System)


JoinFestaAttractionResultFrameFactory::JoinFestaAttractionResultFrameFactory( NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultWork* pJoinFestaAttractionResultWork, NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultViewSystemAccessor* pJoinFestaAttractionResultViewSystemAccessor ) :
  m_pJoinFestaAttractionResultWork( pJoinFestaAttractionResultWork ),
  m_pJoinFestaAttractionResultViewSystemAccessor( pJoinFestaAttractionResultViewSystemAccessor )
{
}


JoinFestaAttractionResultFrameFactory::~JoinFestaAttractionResultFrameFactory()
{
}


NetAppLib::System::NetAppFrameBase* JoinFestaAttractionResultFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
{
  gfl2::heap::HeapBase* pDeviceHeap = m_pJoinFestaAttractionResultWork->GetDeviceHeap();

  NetAppLib::System::NetAppFrameBase* pFrame = NULL;

  switch( frameID )
  {
    case NetApp::JoinFestaAttractionResult::JOINFESTAATTRACTIONRESULT_FRAME_ID_RESULT:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::JoinFestaAttractionResult::Frame::JoinFestaAttractionResultFrame( m_pJoinFestaAttractionResultWork, m_pJoinFestaAttractionResultViewSystemAccessor );
    break;
  }

  return pFrame;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetApp)

// ============================================================================
/*
 * @file JoinFestaGlobalAttractionFrameFactory.cpp
 * @brief グローバルアトラクションアプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionFrameFactory.h"
#include "NetApp/JoinFestaGlobalAttraction/source/JoinFestaGlobalAttractionFrameID.h"
#include "NetApp/JoinFestaGlobalAttraction/source/Frame/JoinFestaGlobalAttractionMainFrame.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(System)


JoinFestaGlobalAttractionFrameFactory::JoinFestaGlobalAttractionFrameFactory( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork* pJoinFestaGlobalAttractionWork, NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionViewSystemAccessor* pJoinFestaGlobalAttractionViewSystemAccessor ) :
  m_pJoinFestaGlobalAttractionWork( pJoinFestaGlobalAttractionWork ),
  m_pJoinFestaGlobalAttractionViewSystemAccessor( pJoinFestaGlobalAttractionViewSystemAccessor )
{
}


JoinFestaGlobalAttractionFrameFactory::~JoinFestaGlobalAttractionFrameFactory()
{
}


NetAppLib::System::NetAppFrameBase* JoinFestaGlobalAttractionFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
{
  gfl2::heap::HeapBase* pDeviceHeap = m_pJoinFestaGlobalAttractionWork->GetDeviceHeap();

  NetAppLib::System::NetAppFrameBase* pFrame = NULL;

  switch( frameID )
  {
    case NetApp::JoinFestaGlobalAttraction::JOINFESTAGLOBALATTRACTION_FRAME_ID_MAIN:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::JoinFestaGlobalAttraction::Frame::JoinFestaGlobalAttractionMainFrame( m_pJoinFestaGlobalAttractionWork, m_pJoinFestaGlobalAttractionViewSystemAccessor );
    break;
  }

  return pFrame;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)

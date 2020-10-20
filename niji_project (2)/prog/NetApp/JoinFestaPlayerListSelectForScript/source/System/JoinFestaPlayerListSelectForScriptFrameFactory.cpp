// ============================================================================
/*
 * @file JoinFestaPlayerListSelectForScriptFrameFactory.cpp
 * @brief プレイヤー単体選択アプリ(スクリプトから起動)のフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelectForScript/source/System/JoinFestaPlayerListSelectForScriptFrameFactory.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/JoinFestaPlayerListSelectForScriptFrameID.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/Frame/JoinFestaPlayerListSelectForScriptSelectFrame.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/Frame/JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/Frame/JoinFestaPlayerListSelectForScriptDetailFrame.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerListSelectForScriptFrameFactory::JoinFestaPlayerListSelectForScriptFrameFactory( NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptWork* pJoinFestaPlayerListSelectForScriptWork, NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptViewSystemAccessor* pJoinFestaPlayerListSelectForScriptViewSystemAccessor ) :
  m_pJoinFestaPlayerListSelectForScriptWork( pJoinFestaPlayerListSelectForScriptWork ),
  m_pJoinFestaPlayerListSelectForScriptViewSystemAccessor( pJoinFestaPlayerListSelectForScriptViewSystemAccessor )
{
}


JoinFestaPlayerListSelectForScriptFrameFactory::~JoinFestaPlayerListSelectForScriptFrameFactory()
{
}


NetAppLib::System::NetAppFrameBase* JoinFestaPlayerListSelectForScriptFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
{
  gfl2::heap::HeapBase* pDeviceHeap = m_pJoinFestaPlayerListSelectForScriptWork->GetDeviceHeap();

  NetAppLib::System::NetAppFrameBase* pFrame = NULL;

  switch( frameID )
  {
    case NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_FRAME_ID_SELECT:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::JoinFestaPlayerListSelectForScript::Frame::JoinFestaPlayerListSelectForScriptSelectFrame( m_pJoinFestaPlayerListSelectForScriptWork, m_pJoinFestaPlayerListSelectForScriptViewSystemAccessor );
    break;

    case NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_FRAME_ID_DETAIL:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::JoinFestaPlayerListSelectForScript::Frame::JoinFestaPlayerListSelectForScriptDetailFrame( m_pJoinFestaPlayerListSelectForScriptWork, m_pJoinFestaPlayerListSelectForScriptViewSystemAccessor );
    break;

    case  JOINFESTAPLAYERLISTSELECTFORSCRIPT_FRAME_ID_ForBattleFesCircle:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::JoinFestaPlayerListSelectForScript::Frame::JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes( m_pJoinFestaPlayerListSelectForScriptWork, m_pJoinFestaPlayerListSelectForScriptViewSystemAccessor );
    break;
  }

  return pFrame;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetApp)

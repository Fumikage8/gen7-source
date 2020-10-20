// ============================================================================
/*
 * @file GameSyncMainFrame.h
 * @brief ゲームシンクアプリの○○プロセスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( GAMESYNCMAIN_FRAME_H_INCLUDE )
#define GAMESYNCMAIN_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GameSync/source/Frame/GameSyncFrameBase.h"
#include "NetApp/GameSync/source/System/GameSyncWork.h"
#include "NetApp/GameSync/source/View/GameSyncMessageMenuLowerViewListener.h"
#include "NetApp/GameSync/source/View/GameSyncTwoSelectMenuLowerViewListener.h"
#include "NetApp/GameSync/source/View/GameSyncThreeSelectMenuLowerViewListener.h"
#include "NetStatic/NetAppLib/include/GameSync/GameSyncCreateSaveDataResponseListener.h"
#include "NetStatic/NetAppLib/include/GameSync/GameSyncConfirmPlayStatusResponseListener.h"
#include "NetStatic/NetAppLib/include/GameSync/GameSyncSaveDataSyncResponseListener.h"
#include "NetStatic/NetAppLib/include/Util/NetAppCommonSaveUtility.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(Frame)


class GameSyncMainFrame :
  public NetApp::GameSync::Frame::GameSyncFrameBase,
  public NetAppLib::GameSync::GameSyncCreateSaveDataResponseListener,
  public NetAppLib::GameSync::GameSyncConfirmPlayStatusResponseListener,
  public NetAppLib::GameSync::GameSyncSaveDataSyncResponseListener
{
  GFL_FORBID_COPY_AND_ASSIGN( GameSyncMainFrame );
public:
  GameSyncMainFrame(
    NetApp::GameSync::System::GameSyncWork* pGameSyncWork,
    NetApp::GameSync::System::GameSyncViewSystemAccessor* pGameSyncViewSystemAccessor );
  virtual~GameSyncMainFrame();

  // GameSyncCreateSaveDataResponseListener
  virtual void OnCreateSaveDataSuccess();
  virtual void OnCreateSaveDataError();

  // GameSyncConfirmPlayStatusResponseListener
  virtual void OnConfirmPlayStatusSuccess();
  virtual void OnConfirmPlayStatusError( u32 errorCode );

  // GameSyncSaveDataSyncResponseListener
  virtual void OnSaveDataSyncSuccess();
  virtual void OnSaveDataSyncError( u32 errorCode );

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

  void setupMainView();

  void sequenceDefault();

private:

  NetApp::GameSync::View::GameSyncMessageMenuLowerViewListener        m_GameSyncMessageMenuLowerViewListener;
  NetApp::GameSync::View::GameSyncTwoSelectMenuLowerViewListener      m_GameSyncTwoSelectMenuLowerViewListener;
  NetApp::GameSync::View::GameSyncThreeSelectMenuLowerViewListener    m_GameSyncThreeSelectMenuLowerViewListener;
  NetAppLib::Util::NetAppCommonSaveUtility                            m_NetAppCommonSaveUtility;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)

#endif // GAMESYNCMAIN_FRAME_H_INCLUDE

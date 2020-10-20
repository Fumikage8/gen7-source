// ============================================================================
/*
 * @file GameSyncDescriptionFrame.h
 * @brief ゲームシンクアプリの○○プロセスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( GAMESYNCDESCRIPTION_FRAME_H_INCLUDE )
#define GAMESYNCDESCRIPTION_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GameSync/source/Frame/GameSyncFrameBase.h"
#include "NetApp/GameSync/source/System/GameSyncWork.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class InfoWindowBookType;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(Frame)


class GameSyncDescriptionFrame :
  public NetApp::GameSync::Frame::GameSyncFrameBase,
  public app::tool::InfoWindowBookTypeListener
{
  GFL_FORBID_COPY_AND_ASSIGN( GameSyncDescriptionFrame );
public:
  GameSyncDescriptionFrame(
    NetApp::GameSync::System::GameSyncWork* pGameSyncWork,
    NetApp::GameSync::System::GameSyncViewSystemAccessor* pGameSyncViewSystemAccessor );
  virtual~GameSyncDescriptionFrame();

  // InfoWindowBookTypeListener
  virtual void OnClose();
  virtual void OnChangePage( u32 pageNo );

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual bool cleanup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();


private:

  app::tool::InfoWindowBookType*      m_pInfoWindowBookTypeLowerView;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)

#endif // GAMESYNCDESCRIPTION_FRAME_H_INCLUDE

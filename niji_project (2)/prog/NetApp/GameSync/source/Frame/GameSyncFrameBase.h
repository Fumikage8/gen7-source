// ============================================================================
/*
 * @file GameSyncFrameBase.h
 * @brief ゲームシンクアプリのフレームの基底クラスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( GAMESYNC_FRAMEBASE_H_INCLUDE )
#define GAMESYNC_FRAMEBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetApp/GameSync/source/System/GameSyncWork.h"
#include "NetApp/GameSync/source/System/GameSyncViewList.h"
#include "NetApp/GameSync/source/System/GameSyncViewObjects.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)

GFL_NAMESPACE_BEGIN(System)
class GameSyncViewSystemAccessor;
GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(Frame)


class GameSyncFrameBase : public NetAppLib::System::NetAppFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GameSyncFrameBase );
public:
  GameSyncFrameBase(
    NetApp::GameSync::System::GameSyncWork* pGameSyncWork,
    NetApp::GameSync::System::GameSyncViewSystemAccessor* pGameSyncViewSystemAccessor );
  virtual~GameSyncFrameBase();

protected:

  virtual void exitFrame( NetAppLib::System::FrameResult result );

  NetApp::GameSync::System::GameSyncWork*            m_pGameSyncWork;
  NetApp::GameSync::System::GameSyncViewList*        m_pGameSyncViewList;
  NetApp::GameSync::System::GameSyncViewObjects*     m_pGameSyncViewObjects;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)

#endif // GAMESYNC_FRAMEBASE_H_INCLUDE

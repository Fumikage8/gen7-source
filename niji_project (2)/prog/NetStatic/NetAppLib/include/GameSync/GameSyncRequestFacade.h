// ============================================================================
/*
 * @file GameSyncRequestFacade.h
 * @brief 
 * @date 2015.12.03
 */
// ============================================================================
#if !defined( GAMESYNCREQUESTFACADE_H_INCLUDE )
#define GAMESYNCREQUESTFACADE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"

#include "NetStatic/NetAppLib/include/Util/NetAppTrainerIconUtility.h"
#include <layout/include/gfl2_LytSys.h>

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(GameSync)

struct GAMESYNC_PARAM 
{
  NetAppLib::System::ApplicationWorkBase* pWork;
  gfl2::lyt::LytPicture* pReplacePane;
  NetAppLib::Util::NetAppTrainerIconUtility* pTrainerIconUtility;
  bool isErrorSkip;
};


  /**
   * @brief プレイステータス確認の目的
   */
/*
  typedef enum
  {
    E_PURPOSE_GAMESYNC,            //!< ゲームシンク
    E_PURPOSE_RATING_BATTLE,       //!< レーティングバトル
    E_PURPOSE_INTERNET_CUP,        //!< インターネット大会
  }PGL_PURPOSE_TYPE;
  */
enum
{
  GETPLAYSTATUS_MODE_GAMESYNC = 0,
  GETPLAYSTATUS_MODE_RATING,
  GETPLAYSTATUS_MODE_CUP,
};

struct GETPLAYSTATUS_PARAM 
{
  NetAppLib::System::ApplicationWorkBase* pWork;
  u32 mode;
  bool isErrorSkip;
};


class GameSyncCreateSaveDataResponseListener;
class GameSyncConfirmPlayStatusResponseListener;
class GameSyncSaveDataSyncResponseListener;


class GameSyncRequestFacade
{
public:
  static gflnet2::ServerClient::RequestHandle AddCreateSaveDataRequest( NetAppLib::GameSync::GameSyncCreateSaveDataResponseListener* pGameSyncCreateSaveDataResponseListener, NetAppLib::GameSync::GAMESYNC_PARAM* pWork );
  static gflnet2::ServerClient::RequestHandle AddConfirmPlayStatusRequest( NetAppLib::GameSync::GameSyncConfirmPlayStatusResponseListener* pGameSyncConfirmPlayStatusResponseListener, NetAppLib::GameSync::GETPLAYSTATUS_PARAM* pWork );
  static gflnet2::ServerClient::RequestHandle AddSaveDataSyncRequest( NetAppLib::GameSync::GameSyncSaveDataSyncResponseListener* pGameSyncSaveDataSyncResponseListener, NetAppLib::GameSync::GAMESYNC_PARAM* pWork );

};


GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetAppLib)

#endif // GAMESYNCREQUESTFACADE_H_INCLUDE

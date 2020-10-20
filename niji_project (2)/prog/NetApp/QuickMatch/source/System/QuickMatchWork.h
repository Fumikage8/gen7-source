// ============================================================================
/*
 * @file QuickMatchWork.h
 * @brief クイックマッチアプリのワーククラスです。
 * @date 2015.08.28
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( QUICKMATCH_WORK_H_INCLUDE )
#define QUICKMATCH_WORK_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTrainerIconUtility.h"

#include <gflnet2/include/p2p/gflnet2_NetGame.h>
#include "NetApp/QuickMatch/include/QuickMatchAppParam.h"
#include "Battle/Regulation/include/Regulation.h"

namespace NetApp
{
  namespace QuickMatch
  {
    namespace View
    {
      class QuickMatchUpperView;
      class QuickMatchLowerView;
    }
  }
}

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)
GFL_NAMESPACE_BEGIN(System)



class QuickMatchWork : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( QuickMatchWork );
public:

  enum
  {
    MAX_PLAYER_NUM = 2,
  };

  enum NETGAME_TYPE
  {
    NETGAME_CLIENT_EXIT = 0,
    NETGAME_HOST_EXIT,
    NETGAME_ON_TRADE,
    NETGAME_ON_SINGLE,
    NETGAME_ON_DOUBLE,
    NETGAME_ON_CHANGE,

    NETGAME_LAST_UPDATE,

    NETGAME_NUM,
  };

  enum TIMING_TYPE
  {
    TIMING_CLIENT_EXIT = NETGAME_NUM,//適当にずらす
    TIMING_HOST_EXIT,
    TIMING_ON_TRADE,
    TIMING_ON_SINGLE,
    TIMING_ON_DOUBLE,
    TIMING_ON_CHANGE,

    TIMING_LAST_UPDATE,
  };

private:

  static const int  APP_HEAP_SIZE             = 1024 * 1024 * 4;    //  4MB
  static const int  APP_DEVICE_HEAP_SIZE      = 1024 * 1024 * 10;   // 20MB



public:
  QuickMatchWork( NetApp::QuickMatch::APP_PARAM* pAppParam );
  virtual~QuickMatchWork();

  void CreateNetGame( gfl2::heap::HeapBase* pHeap );
  void DeleteNetGame( void );

  gflnet2::p2p::NetGame* GetNetGame( u32 index );

  Savedata::MyStatus* GetMyStatus( u32 netId );

  void SetAppResult( NetApp::QuickMatch::APP_RESULT result , Regulation::BATTLE_RULE rule , bool isHost );

  void SetView( NetApp::QuickMatch::View::QuickMatchUpperView* pUpperView , NetApp::QuickMatch::View::QuickMatchLowerView* pLowerView );

  NetApp::QuickMatch::View::QuickMatchUpperView* GetUpperView( void );
  NetApp::QuickMatch::View::QuickMatchLowerView* GetLowerView( void );

  void SetRequestChange( bool isHost ){ m_isRequestChange = true; m_isRequestChangeHost = isHost; };
  bool IsRequestChange( void ) const { return m_isRequestChange; };
  bool IsRequestChangeHost( void ) const { return m_isRequestChangeHost; };

public:

  NetAppLib::Util::NetAppTrainerIconUtility* GetTrainerIconUtility() { return m_pTrainerIconUtility; }
  void SetTrainerIconUtility( NetAppLib::Util::NetAppTrainerIconUtility* pTrainerIconUtility ){ m_pTrainerIconUtility = pTrainerIconUtility; }


public:

  NetApp::QuickMatch::APP_PARAM*                                   m_pAppParam;
private:

  gflnet2::p2p::NetGame* m_pNetGameList[NETGAME_NUM];
  Savedata::MyStatus* m_pMyStatusList[MAX_PLAYER_NUM];

  NetApp::QuickMatch::View::QuickMatchUpperView* m_pUpperView;
  NetApp::QuickMatch::View::QuickMatchLowerView* m_pLowerView;

  NetAppLib::Util::NetAppTrainerIconUtility*                    m_pTrainerIconUtility;
  bool m_isRequestChange;
  bool m_isRequestChangeHost;
};



GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)

#endif // QUICKMATCHWORK_H_INCLUDE

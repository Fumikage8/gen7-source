// ============================================================================
/*
 * @file JoinFestaPlayerListSelectLowerViewListener.h
 * @brief プレイヤーリストアプリの下画面の入力イベントを受け取るリスナーです。
 * @date 2015.10.05
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTLOWERVIEWLISTENER_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTLOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListLowerViewBaseListener.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListManager.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


class JoinFestaPlayerListSelectLowerViewListener : public NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener
{
public:

  JoinFestaPlayerListSelectLowerViewListener() :
    m_pSelectPlayer( NULL ),
    m_pActivePlayer( NULL ),
    m_UpdateUpperView( false )
  {
  }
  ~JoinFestaPlayerListSelectLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_SELECT_PLAYER = JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_START,
    INPUT_EVENT_ID_SELECT_PASSIVE_PLAYER
  };

  void SetSelectPlayer( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pData )        { SetInputEventID( INPUT_EVENT_ID_SELECT_PLAYER );          m_pSelectPlayer = pData; }
  void SetSelectPassivePlayer( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pData ) { SetInputEventID( INPUT_EVENT_ID_SELECT_PASSIVE_PLAYER );  m_pSelectPlayer = pData; }
  void SetActivePlayer( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pData)         { m_pActivePlayer = pData; m_UpdateUpperView = true; }

  NetAppLib::JoinFestaUI::JoinFestaPlayerData* GetSelectPlayer() { return m_pSelectPlayer; }
  NetAppLib::JoinFestaUI::JoinFestaPlayerData* GetActivePlayer() { return m_pActivePlayer; }

  bool IsUpdateUpperView() const { return m_UpdateUpperView; }
  
  void ResetInputEvent()
  {
    NetAppLib::System::NetAppViewListenerBase::ResetInputEvent();
    m_UpdateUpperView = false;
  }

private:

  NetAppLib::JoinFestaUI::JoinFestaPlayerData*            m_pSelectPlayer;
  NetAppLib::JoinFestaUI::JoinFestaPlayerData*            m_pActivePlayer;
  bool                                                    m_UpdateUpperView;

};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTAPLAYERLISTSELECTLOWERVIEWLISTENER_H_INCLUDE

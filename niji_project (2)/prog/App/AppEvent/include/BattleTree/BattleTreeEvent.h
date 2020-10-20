// ============================================================================
/*
 * @file BattleTreeEvent.h
 * @brief バトルツリーアプリのイベント
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#if !defined( BATTLE_TREEEVENT_H_INCLUDE )
#define BATTLE_TREEEVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"
#include "App/BattleTree/include/BattleTreeAppParam.h"
#include "GameSys/include/GameEvent.h"

#include "Field/FieldStatic/include/BattleInst/BattleInst.h"
#include "Field/FieldStatic/include/BattleInst/BattleInstManager.h"
#include "ExtSavedata/include/BattleRecorderSaveData.h"
#include "Battle/include/battle_SetupParam.h" //for BATTLE_SETUP_PARAM
#include "NetApp/BattleVideoRecording/include/BattleVideoRecordingEventAppParam.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)

  class BattleTreeProc;

GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)


class BattleTreeEvent : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( BattleTreeEvent );
public:

  static void StartEvent( GameSys::GameManager* pGameManager, App::BattleTree::APP_PARAM* pAppParam );

  BattleTreeEvent( gfl2::heap::HeapBase* pHeap );
  virtual~BattleTreeEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

private:

  App::BattleTree::APP_PARAM*         m_pAppParam;

  App::BattleTree::BattleTreeProc*    m_pProc;
};



/**
* @class EventBattleInstBattleVideoRecord
* @brief バトルビデオ呼び出し
*/
class EventBattleInstBattleVideoRecord : 
  public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventBattleInstBattleVideoRecord);

public:
  static void StartEvent( GameSys::GameManager* gmgr , int battleCount );
  // コンストラクタ
  EventBattleInstBattleVideoRecord( gfl2::heap::HeapBase* pHeap );
  //デストラクタ
  virtual ~EventBattleInstBattleVideoRecord();

  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* gmgr);
  virtual bool BootChk( GameSys::GameManager* gmgr);
  virtual void InitFunc( GameSys::GameManager* gmgr);
  virtual void EndFunc( GameSys::GameManager* gmgr);
private:
  enum Sequence
  {
    CALL_EVENT,
    END,
  };
protected:
  BattleInst::BattleInst* mpInst;
  Sequence seq;
  NetEvent::BattleVideoRecording::EVENT_APP_PARAM         m_battleVideoRecordingParam;  //! バトルビデオ録画アプリのパラメータ 
  ExtSavedata::BattleRecorderSaveData           m_battleRecorderSaveData; //! バトルビデオ拡張セーブクラス
  int m_BattleCount;
};

GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)

#endif // BATTLE_TREEEVENT_H_INCLUDE

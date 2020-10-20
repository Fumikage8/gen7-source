// ============================================================================
/*
 * @file NBREvent.h
 * @brief 通信対戦受付アプリ起動イベント
 * @date 2015.06.23
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBREVENT_H_INCLUDE )
#define NBREVENT_H_INCLUDE
#pragma once

#include <pml/include/pmlib.h>
#include <macro/include/gfl2_Macros.h>
#include "GameSys/include/GameManager.h"
#include "NetApp/NBR/include/NBRAppParam.h"
#include "NetStatic/NetEvent/include/NetEventBase.h" // for NetEventBase
// レギュレーション選択アプリ
#include "Battle/Regulation/include/Regulation.h"
#include "NetStatic/NetAppLib/include/UI/NetAppRegulationDrawInfo.h"
#include "NetStatic/NetEvent/include/RegulationSelectEvent.h"
// バトルチーム選択アプリ
#include "NetApp/TeamSelect/include/TeamSelectEventAppParam.h"
#include "NetStatic/NetEvent/include/TeamSelectEvent.h"
// 通信バトルイベント
#include "NetStatic/NetEvent/include/NetworkBattleEvent.h"
// BTL_RUEL
#include "Battle/Regulation/include/Regulation.h"

namespace NetApp{
  namespace NBR{
    class NBRNet;
  }
}

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(NBR)

//-------------------------------------
/// イベント呼び出しパラメータ構造体
//=====================================
struct EVENT_PARAM{
  // NBR起動パラメータ
  int                     hostNetID;          //! [in]主催者のNetID
  Regulation::BATTLE_RULE battleRule;         //! [in]バトルルール

  // コンストラクタ
  EVENT_PARAM(): 
    hostNetID(0),battleRule(Regulation::BATTLE_RULE_SINGLE) {}
};

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------
/**
 *	@brief  通信対戦受付アプリ起動クラス
 */
//----------------------------------------------------------------------
class NBREvent : public NetEvent::NetEventBase
{
  GFL_FORBID_COPY_AND_ASSIGN( NBREvent );

public:
  //--------------------------------------------------------------
  /**
   * @brief  通信対戦受付アプリ呼び出し
   * 
   * @param   pGameManager  ゲームマネージャ
   * @param   pEventParam   イベント呼び出しパラメータ構造体
   */
  //--------------------------------------------------------------
  static void StartEvent( GameSys::GameManager* pGameManager, EVENT_PARAM* pEventParam );

  // コンストラクタ
  NBREvent( gfl2::heap::HeapBase* pHeap );
  // デストラクタ
  virtual~NBREvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

private:

  void SetEventParam( EVENT_PARAM* pEventParam ){ m_pEventParam = pEventParam; };

private:

  /**
   * @enum Sequence
   *       シーケンス定義
   */
  enum Sequence{
    SEQ_CALL_APP,   //! アプリ呼び出し
    SEQ_APP_END_WAIT,//! アプリ終了待ち
    SEQ_RETURN_APP, //! アプリ戻り
    SEQ_END,        //! 終了
  };

  /**
   * @enum CallAppType
   *       呼び出しアプリ種類
   */
  enum CallAppType{
    CALL_NBR,               //! NBRアプリ呼び出し
    CALL_REGULATION_SELECT, //! レギュレーション選択アプリ呼び出し
    CALL_BATTLE_TEAM_SELECT,//! バトルチーム選択アプリ呼び出し
    CALL_BATTLE,            //! バトル呼び出し
  };

  //! 呼び出すアプリ設定
  void SetCallApp( CallAppType callAppType );
  //! アプリ呼び出し
  void CallApp( GameSys::GameManager* pGameManager );
  //! アプリ戻り
  void ReturnApp();


  //! NBRアプリ呼び出しパラメータの初期化
  void InitializeNbrAppParam();
  //! レギュレーション選択アプリ呼び出しパラメータの初期化
  void InitializeRegulationSelectAppParam();
  //! バトルチーム選択アプリ呼び出しパラメータの初期化
  void InitializeTeamSelectAppParam();
  //! 通信対戦イベント呼び出しパラメータの設定
  void SetupNetworkBattleParam();


private:

  EVENT_PARAM*                                  m_pEventParam;            //! Event呼び出しパラメータ
  Sequence                                      m_sequence;               //! シーケンス

  // 呼び出しアプリ
  CallAppType                                   m_callAppType;            //! 呼び出しアプリ種類

  // アプリ呼び出しパラメータ
  NetApp::NBR::APP_PARAM                        m_nbrAppParam;            //! NBR
  NetApp::RegulationSelect::APP_RESULT          m_regulationSelectResult; //! レギュレーション選択
  NetApp::TeamSelect::EVENT_APP_PARAM           m_teamSelectEventParam;   //! バトルチーム選択
  NetEvent::NetworkBattle::EVENT_PARAM          m_networkBattleParam;     //! 通信対戦イベント

  // 実体
  NetEvent::NetworkBattle::PersonalData         m_personalData[ BTL_CLIENT_NUM ];  //! 参加者データ 
  NetAppLib::UI::RegulationDrawInfo             m_regulationDrawInfo;     //! レギュレーション描画情報

  // NBRの通信クラス（常に生きている必要がある）
  NetApp::NBR::NBRNet*                          m_pNBRNet;                //! NBR通信クラス

};


GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetEvent)

#endif // NBREVENT_H_INCLUDE

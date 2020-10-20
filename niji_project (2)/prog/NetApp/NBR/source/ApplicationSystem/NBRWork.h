// ============================================================================
/*
 * @file NBRWork.h
 * @brief 通信対戦受付アプリのワーククラスです。
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_WORK_H_INCLUDE )
#define NBR_WORK_H_INCLUDE
#pragma once

#include <pml/include/pmlib.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "Savedata/include/MyStatus.h"
#include "NetApp/NBR/include/NBRAppParam.h"
#include "NetStatic/NetAppLib/include/NBR/NBRNetDefine.h"
#include "Battle/include/battle_SetupParam.h"
#include "NetStatic/NetAppLib/include/UI/NetAppRegulationDrawInfo.h"
#include "Battle/Regulation/include/Regulation.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


class NBRWork : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRWork );
public:

  /**
   * @enum EndType
   * 終了が誰からなのか
   */
  enum EndType{
    END_TYPE_ONESELF,   //! 自ら
    END_TYPE_OPPONENT,  //! 相手から
  };

private:

  static const u32  APP_HEAP_SIZE             = 1024 * 1024 * 4;    //  4MB
  static const u32  APP_DEVICE_HEAP_SIZE      = 1024 * 1024 * 10;   // 10MB

public:
  NBRWork();
  virtual~NBRWork();

public:
  int                  *m_pRuleSelectorNetId;                  //! 現在のルール選択者のNetID
  u8                   *m_priority;                            //! NetIdが要素で優先度が格納されている
  BTL_CLIENT_ID        m_place[ NBRNetDefine::NET_NUM_MAX ];   //! NetIdが要素で戦う位置が入っている
  u32                  m_bgm;                                  //! 選択したBGM
  s8                   *m_pSelectBgmIndex;                     //! 選択しているBGMインデックス
  int                  m_hostNetID;                            //! 主催者
  Regulation::BATTLE_RULE m_battleRule;                        //! バトルルール

  // ↓戻り値用 (実体はEventが持っている)
  NetAppLib::UI::RegulationDrawInfo*  m_pSelectRegulationDrawInfo;  //! レギュレーション選択アプリで選択したレギュレーション（NULLで選択していない）


  // フレーム専用
  EndType              m_appEndFrameEndType;                   //! 終了が誰からか
  bool                 m_isEndRuleSelectorTutorial;            //! ルール選択者決めのチュートリアル終了フラグ
  bool                 m_isSelectedRegulation;                 //! レギュレーション選択アプリでレギュレーション選んでいるか(trueで選んでいる）
  bool                 m_isTeamSelectReturn;                   //! バトルチーム選択アプリからの戻りか
  TeamSelectReturnMode m_teamSelectReturnMode;                 //! バトルチーム選択アプリの結果
  u32                  m_teamSelectIndex;                      //! バトルチーム選択アプリで選択したチームインデックス

  bool                 m_isQrTeam[ NBRNetDefine::NET_NUM_MAX ];

private:
  Savedata::MyStatus*  m_pMyStatus[ NBRNetDefine::NET_NUM_MAX ];
  int                  m_myNetId;

  /**
   *  memo:受信データとNetIDの関係
   *
   *  NetIDは不変。親は0だし、子は1,2,3のいずれか。 
   *
   *  MyStatus ・・・ 概要：プレイヤーデータ
   *                  取得方法：添字がNetID。接続している人全てで共通。
   *                            ない場合はNULL。
   *
   *  priority ・・・ 概要：ルール選択する人の優先順位
   *                        優先順位  高い 0 → 3 低い
   *                  取得方法：添字がNetID。 接続している人全てで共通。
   *                  
   *                  
   *  place    ・・・ 概要：マルチの場合の戦う場所（立ち位置）
   *                        立ち位置メモ
   *                         ---------------
   *                              敵1  敵1
   *                  
   *                           自分　味方
   *                         ---------------
   *
   *                        数字だとは
   *                         ---------------
   *                              1  3
   *                  
   *                           0  2
   *                         ---------------
   *                  取得方法：添字がNetID。 接続している人全てで共通。
   *                  親が敵2だとすると
   *                      place[0]→3(BTL_CLIENT_ENEMY2)になる。
   *
   */

public:
  /**
   * @brief  自分の立ち位置取得
   * @return BTL_CLIENT_ID
   */
  BTL_CLIENT_ID GetMinePlace() const;

  /**
   * @brief  自分のMyStatus取得
   * @return MyStatus
   */
  Savedata::MyStatus* GetMineMyStatus() const;

  /**
   * @brief  味方のMyStatus取得
   * @return MyStatus
   */
  Savedata::MyStatus* GetFriendMyStatus() const;

  /**
   * @brief  敵のMyStatus取得
   * @param   no    相手番号   0,1のどっちか
   * @return MyStatus
   */
  Savedata::MyStatus* GetEnemyMyStatus( u8 enemyNo = 0) const;

  /**
   * @brief  NetIDでMyStatus取得
   * @param  netId    接続番号
   * @return MyStatus
   */
  Savedata::MyStatus* GetMyStatusFromNetId( const int netId ) const;

  /**
   * @brief  バトル立ち位置指定でMyStatusを取得
   * @param  btlClientId  立ち位置
   * @return MyStatus
   */
  Savedata::MyStatus* GetMyStatusFromBtlClientId( const BTL_CLIENT_ID btlClientId ) const;

  /**
   * @brief  NetID指定でポケパーティを取得
   * @param  netId        接続番号
   * @return PokeParty    ポケパーティ(無い場合はNULL)
   */
  pml::PokeParty* GetPokePartyFromNetId( const int netId ) const;

  /**
   * @brief  バトル立ち位置指定でポケパーティを取得
   * @param  btlClientId  立ち位置
   * @return PokeParty    ポケパーティ(無い場合はNULL)
   */
  pml::PokeParty* GetPokePartyFromBtlClientId( const BTL_CLIENT_ID btlClientId ) const;
  
  /**
   * @brief  バトル立ち位置指定でパーティがQRバトルチームかどうかを取得
   * @param  btlClientId  立ち位置
   * @return QRバトルチームかどうか
   */
  bool IsQrTeamFromBtlClientId( const BTL_CLIENT_ID btlClientId ) const;

  /**
   * @brief  ルール選択者の優先順位計算
   * @param  connectionNum  接続人数
   */
  void RuleSelectorPriorityCalc( const int connectionNum );

  /**
   * @brief  ルール選択者の優先順位を次の人の優先順位に変更
   * @param  connectionNum  接続人数
   */
  void RuleSelectorChangeNextPriority( const int connectionNum );

  /**
   * @brief  ルール選択者かどうか
   * @param  netId        接続番号
   * @return trueでルール選択者
   */
  bool IsRuleSelector( const int netId ) const;

  /**
   * @brief  自分がルール選択者かどうか
   * @return trueでルール選択者
   */
  bool IsOneselfRuleSelector() const;

  /**
   * @brief  マルチかどうか
   * @return trueでマルチ
   */
  bool IsMutli() const;

  /**
   * @brief  接続相手にnijiが混ざっているかどうか
   * @return trueでnijiがいる
   */
  bool IsNijiConnect();

private:

  //----------------------------------------------------------------------------
  /**
   * @brief   戦闘の立ち位置からNetIDを検索して返す
   * @param   place  立ち位置
   * @return  NetID
   */
  //----------------------------------------------------------------------------
  int SearchNetIdForStandingpos( BTL_CLIENT_ID place ) const;

};



GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBRWORK_H_INCLUDE

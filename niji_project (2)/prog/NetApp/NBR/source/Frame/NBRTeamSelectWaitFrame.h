// ============================================================================
/*
 * @file NBRTeamSelectWaitFrame.h
 * @brief 通信対戦受付アプリのチーム選択待ちプロセスです。
 *        フレームID:NBR_FRAME_ID_TEAM_SELECT_WAIT
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_TEAMSELECTWAIT_FRAME_H_INCLUDE )
#define NBR_TEAMSELECTWAIT_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/NBR/source/NBRFrameBase.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRWork.h"

// Net
#include "NetStatic/NetAppLib/include/NBR/NBRNet.h"
#include "NetStatic/NetAppLib/include/NBR/NBRNetRecvListener.h"
// View
#include "NetApp/NBR/source/View/SubView/NBRUpperViewPlayerInfoDefault.h"
#include "NetApp/NBR/source/View/SubView/NBRLowerViewReturnButton.h"
#include "NetApp/NBR/source/View/SubView/NBRLowerViewReturnButtonListener.h"

// バトルチーム
#include "PokeTool/include/PokeToolBattleParty.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

class NBRTeamSelectWaitFrame :
  public NBRFrameBase,
  public NBRNetRecvListener,
  public View::NBRLowerViewReturnButtonListener
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRTeamSelectWaitFrame );
public:
  // コンストラクタ
  NBRTeamSelectWaitFrame( NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
                          View::NBRParentView* pParentView,
                          NBRNet* pNet );
  // デストラクタ
  virtual ~NBRTeamSelectWaitFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();
  virtual bool cleanup();

public:
  //------------------------------------------------------------------
  //------------------------------------------------------------------
  // NetRecvListener
  //------------------------------------------------------------------
  /**
   * @brief  通信終了通知
   */
  virtual void OnNBRNetRecvEnd();

  //------------------------------------------------------------------
  /**
   * @brief  勝負開始受信通知
   * @param[in] netId           通信ID
   * @param[in] bgm             選択されたBGM（ローカルで子のみ格納されている）
   * @param[in] isQrTeam        QRバトルチームかどうか
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvBattleStart( const int netId, const u32 bgm, const bool isQrTeam );

  //------------------------------------------------------------------
  /**
   * @brief  参加出来ない受信通知
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvCantParticipate();

  //------------------------------------------------------------------
  // NBRLowerViewReturnButtonListener
  //------------------------------------------------------------------
  /**
   * @brief   戻るボタン通知
   */
  virtual void OnBackButtonAction();

  //------------------------------------------------------------------
  // NBRLowerViewMessageMenuListener
  //------------------------------------------------------------------
  /**
   * @brief   はいボタン通知
   */
  virtual void OnYesButtonAction();

  /**
   * @brief   いいえボタン通知
   */
  virtual void OnNoButtonAction();

private:

  /**
   * @enum   Sequence
   * シーケンス定義
   */
  enum Sequence{
    SEQ_BATTLESTART_SEND,         //! 勝負開始送信
    SEQ_BATTLESTART_RECV_WAIT,    //! 勝負開始受信待ち

    SEQ_POKEPARTY_SEND,           //! 選んだパーティ送信
    SEQ_POKEPARTY_RECV_WAIT,      //! ポケパーティ受信待ち
    SEQ_TO_NEXT_TIMING_LOOP,      //! 次へ行くための同期待ち

    SEQ_CANNOT_JOIN_MESSAGE_WAIT, //! ストリーミングメッセージ終了待ち
    SEQ_RETURN_RULE_SELECT_TIMING,//! ルール選択へ戻る前同期
  };

private:

  //! 待機中メッセージ表示
  void ShowWaitMessage();

  //! 選択状態を選択完了に変える
  void PlayerStatusFinish( int targetNetId );

  //! 勝負開始通知受信完了か
  bool IsBattleStartRecvFinish() const;

private:
  // SubView
  View::NBRLowerViewReturnButton*       m_pLowerView;   //! 下画面
  View::NBRUpperViewPlayerInfoDefault*  m_pUpperView;   //! 上画面

  PokeTool::BattleParty                 m_battleParty;  //! バトルパーティ           
  u8                                    m_recvCount;    //! 勝負開始通知受信カウント
};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_TEAMSELECTWAIT_FRAME_H_INCLUDE

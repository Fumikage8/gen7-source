// ============================================================================
/*
 * @file NBRTeamSelectFrame.h
 * @brief 通信対戦受付アプリのチーム選択プロセスです。
 *        フレームID:NBR_FRAME_ID_TEAM_SELECT
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_TEAMSELECT_FRAME_H_INCLUDE )
#define NBR_TEAMSELECT_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/NBR/source/NBRFrameBase.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRWork.h"

// Net
#include "NetStatic/NetAppLib/include/NBR/NBRNet.h"
#include "NetStatic/NetAppLib/include/NBR/NBRNetRecvListener.h"
// View
#include "NetApp/NBR/source/View/SubView/NBRUpperViewRegulation.h"
#include "NetApp/NBR/source/View/SubView/NBRLowerViewTeamSelect.h"
#include "NetApp/NBR/source/View/SubView/NBRLowerViewTeamSelectListener.h"

// バトルチーム
#include "PokeTool/include/PokeToolBattleParty.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

class NBRTeamSelectFrame :
  public NBRFrameBase,
  public NBRNetRecvListener,
  public View::NBRLowerViewTeamSelectListener
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRTeamSelectFrame );
public:
  // コンストラクタ
  NBRTeamSelectFrame( NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
                      View::NBRParentView* pParentView,
                      NBRNet* pNet );
  // デストラクタ
  virtual ~NBRTeamSelectFrame();

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
   * @brief  参加出来ない受信通知
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvCantParticipate();

  //------------------------------------------------------------------
  // NBRLowerViewTeamSelectListener
  //------------------------------------------------------------------
  //--------------------------------------------------------------------------
  /**
   * @brief   対戦チームボタン押下通知
   */
  //--------------------------------------------------------------------------
  virtual void OnTeamButtonAction();

  //--------------------------------------------------------------------------
  /**
   * @brief   参加できないボタン押下通知
   */
  //--------------------------------------------------------------------------
  virtual void OnCannnotJoinButtonAction();

  //--------------------------------------------------------------------------
  /**
   * @brief   しょうぶかいしボタン押下通知
   */
  //--------------------------------------------------------------------------
  virtual void OnBattleStartButtonAction();

  //--------------------------------------------------------------------------
  /**
   * @brief   BGM選択ボタン押下通知（右左）
   */
  //--------------------------------------------------------------------------
  virtual void OnBgmLButtonAction();
  virtual void OnBgmRButtonAction();

  //--------------------------------------------------------------------------
  /**
   * @brief   戻るボタン通知
   */
  //--------------------------------------------------------------------------
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

  //! トップメニューのボタン有効・無効設定
  void SetupTopMenuButton();
  
private:

  /*
   * @enum Sequence
   * シーケンス定義
   */
  enum Sequence{
    SEQ_CANCEL_MESSAGE_WAIT,    //! バトルチーム選択アプリキャンセルした時メッセージ
    SEQ_TEAM_BUTTON_WAIT,       //! チームボタン押下待ち
    SEQ_CANNOT_JOIN_LIST_SEND,  //! 参加できない送信
    SEQ_CANNOT_JOIN_MESSAGE_WAIT, //! ストリーミングメッセージ終了待ち
    SEQ_RETURN_RULE_SELECT_TIMING,  //! ルール選択へ戻る前同期
  };

private:

  // SubView
  View::NBRLowerViewTeamSelect*     m_pLowerView;   //! 下画面
  View::NBRUpperViewRegulation*     m_pUpperView;   //! 上画面

  Sequence                          m_seq;          //! シーケンス
  bool                              m_isAppEndList; //! 表示している選択肢がアプリ終了のか

  PokeTool::BattleParty             m_battleParty;  //! バトルパーティ           

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_TEAMSELECT_FRAME_H_INCLUDE

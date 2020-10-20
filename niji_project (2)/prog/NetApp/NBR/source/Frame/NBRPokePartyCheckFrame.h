// ============================================================================
/*
 * @file NBRPokePartyCheckFrame.h
 * @brief 通信対戦受付アプリのポケパーティ検査フレーム
          フレームID：NBR_FRAME_ID_POKEPARTY_CHECK
 * @date 2015.08.07
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_POKEPARTYCHECK_FRAME_H_INCLUDE )
#define NBR_POKEPARTYCHECK_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/NBR/source/NBRFrameBase.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRWork.h"

// Net
#include "NetStatic/NetAppLib/include/NBR/NBRNet.h"
#include "NetStatic/NetAppLib/include/NBR/NBRNetRecvListener.h"
// View
#include "NetApp/NBR/source/View/SubView/NBRLowerViewReturnButton.h"
#include "NetApp/NBR/source/View/SubView/NBRLowerViewReturnButtonListener.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

class NBRPokePartyCheckFrame :
  public NBRFrameBase,
  public NBRNetRecvListener,
  public View::NBRLowerViewReturnButtonListener
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRPokePartyCheckFrame );

public:
  // コンストラクタ
  NBRPokePartyCheckFrame( NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
                                View::NBRParentView* pParentView,
                                NBRNet* pNet );
  // デストラクタ
  virtual ~NBRPokePartyCheckFrame();

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
   * @brief     ポケパーティ検査結果通知
   * @param[in] レポケパーティ検査結果コマンド
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvPokePartyCheckRet( const u8 command[NBRNetDefine::NET_NUM_MAX] );

  //------------------------------------------------------------------
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
  virtual void OnYesButtonAction(){} // 未使用

  /**
   * @brief   いいえボタン通知
   */
  virtual void OnNoButtonAction(){} // 未使用

private:

  static const u32 PAGE_MAX = 2;

  //=================================
  //! シーケンス定義
  //=================================
  enum{
    SEQ_POKEPARTY_CHECK_AND_SEND,      //! ポケパーティ検査して結果送信
    SEQ_RECV_WAIT,            //! コマンド受信待ち
    SEQ_MESSAGE_WAIT_TO_TEAM_SELECT, //! メッセージ終了待ち(終了後チーム選択へ)
    SEQ_MESSAGE_WAIT_TO_TEAM_SELECT_WAIT, //! メッセージ終了待ち(終了後チーム選択待ちへ)
    SEQ_TIMING_LOOP,          //! 同期待ち
  };

  //! ポケパーティ検査して結果送信
  bool CheckAndSendPokePartyCore();

  //! 全員のコマンド受信完了したか？
  bool IsRecvCommandFinish() const;

  //! 送受信コマンド見て次の処理確定
  void SetNextSeqByNetCommand();

  //! 終了同期へ
  void ToTimingLoop( NetAppLib::System::FrameResult result );

private:
  // SubView
  View::NBRLowerViewReturnButton*       m_pLowerView;   //! 下画面
  NetAppLib::System::FrameResult        m_result;       //! フレーム結果

  // 通信用
  u8                                    m_recvCount;
  bool                                  m_isSendOtherPokePartyRetCommandNg; //! 相手のポケパーティが検査NGだったか
  bool                                  m_isRecvMyPokePartyRetCommandNg;    //! 自分のポケパーティが相手検査でNGだったか
};

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_POKEPARTYCHECK_FRAME_H_INCLUDE

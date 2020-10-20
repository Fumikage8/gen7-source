// ============================================================================
/*
 * @file NBRContinueFrame.h
 * @brief 通信対戦受付アプリの続けて遊ぶプロセスです。
 *        フレームID:NBR_FRAME_ID_CONTINUE
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_CONTINUE_FRAME_H_INCLUDE )
#define NBR_CONTINUE_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/NBR/source/NBRFrameBase.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRWork.h"

// Net
#include "NetStatic/NetAppLib/include/NBR/NBRNet.h"
#include "NetStatic/NetAppLib/include/NBR/NBRNetRecvListener.h"
// View
#include "NetApp/NBR/source/View/SubView/NBRUpperViewPlayerInfoSimple.h"
#include "NetApp/NBR/source/View/SubView/NBRLowerView2Button.h"
#include "NetApp/NBR/source/View/SubView/NBRLowerView2ButtonListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

class NBRContinueFrame : 
  public NBRFrameBase,
  public NBRNetRecvListener,
  public View::NBRLowerView2ButtonListener
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRContinueFrame );
public:
  // コンストラクタ
  NBRContinueFrame( NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
                    View::NBRParentView* pParentView,
                    NBRNet* pNet );
  // デストラクタ
  virtual ~NBRContinueFrame();

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
   * @brief     続けて遊ぶ受信結果通知
   * @param[in] 続けて遊ぶ受信結果通知コマンド
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvContinue( const NBRNetDefine::ContinueCommand command );

  //------------------------------------------------------------------
  //------------------------------------------------------------------
  // NBRLowerViewReturnButtonListener
  //------------------------------------------------------------------
  /**
   * @brief   ボタン押下通知
   * @param[out]  buttonId    選んだボタンのボタンID
   */
  virtual void On2ButtonAction( int buttonId );

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

  //=================================
  //! シーケンス定義
  //=================================
  enum{
    SEQ_BUTTON_WAIT,          //! ボタン押下待ち
    SEQ_RULE_CHANGE_SEND,     //! ルールを変えて遊ぶ送信
    SEQ_RULE_NOT_CHANGE_SEND, //! 同じルールで遊ぶ送信
    SEQ_RECV_WAIT,            //! コマンド受信待ち
    SEQ_TIMING_LOOP,          //! 同期待ち
    SEQ_GOTO_WAIT,            //! 次フレーム行きへのメッセージ表示待ち
  };

  //! 全員のコマンド受信完了したか？
  bool IsRecvCommandFinish() const;

  //! 次のフレームを取得
  NetAppLib::System::FrameResult GetNextFrame() const;
  //! 次のフレームへ行くときのメッセージ表示
  void ShowGotoNextMessage() const;

private:
  // SubView
  View::NBRLowerView2Button*            m_pLowerView;   //! 下画面
  View::NBRUpperViewPlayerInfoSimple*   m_pUpperView;   //! 上画面

  // 受信コマンド
  NBRNetDefine::ContinueCommand         m_recvContinueCommand[ NBRNetDefine::NET_NUM_MAX ];
  u8                                    m_recvCount;
};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_CONTINUE_FRAME_H_INCLUDE

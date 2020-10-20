// ============================================================================
/*
 * @file NBRMultiParentWaitFrame.h
 * @brief 通信対戦受付アプリのマルチ親待ちプロセスです。
 *        フレームID:NBR_FRAME_ID_MULTI_PARENT_WAIT
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_MULTIPARENTWAIT_FRAME_H_INCLUDE )
#define NBR_MULTIPARENTWAIT_FRAME_H_INCLUDE
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

class NBRMultiParentWaitFrame : 
  public NBRFrameBase,
  public NBRNetRecvListener,
  public View::NBRLowerViewReturnButtonListener
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRMultiParentWaitFrame );
public:
  // コンストラクタ
  NBRMultiParentWaitFrame( NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
                           View::NBRParentView* pParentView,
                           NBRNet* pNet );
  // デストラクタ
  virtual ~NBRMultiParentWaitFrame();

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
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvEnd();

  //------------------------------------------------------------------
  /**
   * @brief  マルチ振り分け決定通知
   * @param[in] NetIdが要素で戦う位置が格納されている
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvMultiDecide( const u8 place[4] );

  //------------------------------------------------------------------
  /**
   * @brief  マルチ振り分け通知
   * @param[in] netId   決めた人のNetId
   * @param[in] place   立ち位置（BTL_CLIETN_ID）
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvMultiSelect( const int netId, const u8 place );

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
    SEQ_RECV_WAIT,                //! 受信待ち
    SEQ_TIMING_LOOP,              //! 同期待ち
  };

private:

  // SubView
  View::NBRLowerViewReturnButton*       m_pLowerView;   //! 下画面

  int                                   m_decideNetId[ BTL_CLIENT_NUM ]; //! 立ち位置が要素。立ち位置決定した場合NetIDが入っている。初期値-1
};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_MULTIPARENTWAIT_FRAME_H_INCLUDE

// ============================================================================
/*
 * @file NBRRuleSelectWaitFrame.h
 * @brief 通信対戦受付アプリのルール選択待ちプロセスです。
 *        フレームID:NBR_FRAME_ID_RULE_SELECT_WAIT
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_RULESELECTWAITFRAME_FRAME_H_INCLUDE )
#define NBR_RULESELECTWAITFRAME_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/NBR/source/NBRFrameBase.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRWork.h"

// Net
#include "NetStatic/NetAppLib/include/NBR/NBRNet.h"
#include "NetStatic/NetAppLib/include/NBR/NBRNetRecvListener.h"
// View
#include "NetApp/NBR/source/View/SubView/NBRUpperViewPlayerInfoSimple.h"
#include "NetApp/NBR/source/View/SubView/NBRLowerViewReturnButton.h"
#include "NetApp/NBR/source/View/SubView/NBRLowerViewReturnButtonListener.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

class NBRRuleSelectWaitFrame :
  public NBRFrameBase,
  public NBRNetRecvListener,
  public View::NBRLowerViewReturnButtonListener
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRRuleSelectWaitFrame );
public:
  // コンストラクタ
  NBRRuleSelectWaitFrame( NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
                          View::NBRParentView* pParentView,
                          NBRNet* pNet );
  // デストラクタ
  virtual ~NBRRuleSelectWaitFrame();

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
   * @brief  ルール選択を他の人に任せる通知
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvRuleOther();

  //------------------------------------------------------------------
  /**
   * @brief  レギュレーション準備受信通知
   * @param[in]   isPreset    送信しようとしているレギュレーションがプリセットならtrue
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvRegulationReady( bool isPreset );

  //------------------------------------------------------------------
  /**
   * @brief  レギュレーション受信通知（プリセット）
   * @param[in]   presetIndex        プリセットレギュレーションの番号
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvRegulation( u8 presetIndex );

  //------------------------------------------------------------------
  /**
   * @brief  レギュレーション受信通知（配信）
   * @param[in]   netId               送信者のNetId
   * @param[in]   pRegulationBuffer   レギュレーションデータ
   * @param[in]   regulationSize      レギュレーションデータのサイズ
   * @param[in]   pCerdData           署名データ
   * @param[in]   cerdDataSize        署名データサイズ
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvRegulation( const int netId,
                                       const void* pRegulationBuffer, const u32 regulationSize,
                                       const void* pCerdData, const u32 cerdDataSize );

  /**
   * @brief     レギュレーション受信結果通知
   * @param[in] レギュレーション受信結果通知コマンド
   */
  virtual void OnNBRNetRecvRegulationRet( const NBRNetDefine::RegulationRetCommand command );

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
    SEQ_REGULATION_READY_RECV_WAIT,//! レギュレーション準備受信待ち
    SEQ_REGULATION_SEND_TIMING,   //! レギュレーション送信前同期
    SEQ_REGULATION_RECV_WAIT,     //! レギュレーション受信待ち
    SEQ_DOWNLOAD_REGULATION_SEND, //! 配信レギュレーション送信
    SEQ_REGULATION_RET_SEND,      //! レギュレーション受信結果送信
    SEQ_TO_NEXT_TIMING,           //! 次へ進む前の同期
    SEQ_RULE_OTHER_MESSAGE_WAIT,  //! ルール決める人やめたメッセージ終了待ち
    SEQ_REG_ERROR_MESSAGE_WAIT,   //! レギュレーションエラーメッセージ終了待ち
    SEQ_TO_BATTLE_TEAM_SELECT, //! バトルチーム選択へ
  };


private:

  //! 待機中メッセージ表示
  void ShowWaitMessage();
  //! レギュレーションアラートメッセージ開始
  void StartRegulationErrorMessage( const u32 msgId );

  // SubView
  View::NBRLowerViewReturnButton*       m_pLowerView;   //! 下画面
  View::NBRUpperViewPlayerInfoSimple*   m_pUpperView;   //! 上画面

  bool    m_isPreset;               //! 受信予定のレギュレーションがプリセットかどうか

  //bool  m_isRecvRegulationNow;    //! レギュレーション受信中フラグ

  // @fix GFNMCat[2144]：受信リスナーでシーケンス上書きされないように修正
  bool                                  m_isRecvRegulationReady;      //! レギュレーション準備受信フラグ
  bool                                  m_isRecvRegulation;           //! レギュレーション受信フラグ
  NBRNetDefine::RegulationRetCommand    m_sendRegulationRetCommand;   //! 送信するレギュレーション受信結果コマンド
  u8                                    m_recvRegulationRetCount;     //! レギュレーション受信結果通知受信カウント
};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_RULESELECTWAITFRAME_FRAME_H_INCLUDE

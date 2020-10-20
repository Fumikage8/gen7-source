// ============================================================================
/*
 * @file NBRRuleSelectAppEndFrame.h
 * @brief 通信対戦受付アプリのルール選択アプリ戻りプロセスです。
 *        フレームID:NBR_FRAME_ID_RULE_SELECT_APP_END
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_RULESELECTAPPEND_FRAME_H_INCLUDE )
#define NBR_RULESELECTAPPEND_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/NBR/source/NBRFrameBase.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRWork.h"

// Net
#include "NetStatic/NetAppLib/include/NBR/NBRNet.h"
#include "NetStatic/NetAppLib/include/NBR/NBRNetRecvListener.h"
// View
#include "NetApp/NBR/source/View/SubView/NBRUpperViewRegulation.h"
#include "NetApp/NBR/source/View/SubView/NBRLowerViewReturnButton.h"
#include "NetApp/NBR/source/View/SubView/NBRLowerViewReturnButtonListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

class NBRRuleSelectAppEndFrame :
  public NBRFrameBase,
  public NBRNetRecvListener,
  public View::NBRLowerViewReturnButtonListener
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRRuleSelectAppEndFrame );
public:
  // コンストラクタ
  NBRRuleSelectAppEndFrame( NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
                            View::NBRParentView* pParentView,
                            NBRNet* pNet );
  // デストラクタ
  virtual ~NBRRuleSelectAppEndFrame();

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
   * @param[in] command         レギュレーション受信結果通知コマンド
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

  /*
   * @enum Sequence
   *       シーケンス定義
   */
  enum Sequence{
    SEQ_REGULATION_READY_SEND,  //! レギュレーション準備送信
    SEQ_REGULATION_SEND_TIMING, //! レギュレーション送信前同期
    SEQ_REGULATION_SEND,  //! レギュレーション送信
    SEQ_RET_RECV_WAIT,    //! レギュレーション受信結果通知待ち
    SEQ_TO_NEXT_TIMING,   //! 次へ進む前の同期
    SEQ_MESSAGE_WAIT,     //! ストリーミングメッセージ終了待ち
    SEQ_TO_BATTLE_TEAM_SELECT, //! バトルチーム選択へ
  };


  // SubView
  View::NBRLowerViewReturnButton*       m_pLowerView;   //! 下画面
  View::NBRUpperViewRegulation*         m_pUpperView;   //! 上画面

  // @fix GFNMCat[2144]：受信リスナーでシーケンス上書きされないように修正
  u8                                    m_recvRegulationRetCount; //! レギュレーション受信結果通知受信カウント
  NBRNetDefine::RegulationRetCommand    m_regulationRetCommand;   //! レギュレーション受信結果通知

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_RULESELECTAPPEND_FRAME_H_INCLUDE

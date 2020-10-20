//======================================================================
/**
 * @file NBRNetRecvListener.h
 * @date 2015/07/08 15:02:39
 * @author ichiraku_katsuhiko
 * @brief 通信対戦受付アプリの通信受信通知リスナー
 */
//======================================================================

#if !defined __NBR_NET_RECV_LISTENER_NET_H_INCLUDED__
#define __NBR_NET_RECV_LISTENER_NET_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <pml/include/pmlib.h>
#include <gflnet2/include/p2p/gflnet2_P2pDefine.h>

// NBRNetDefine
#include "NetStatic/NetAppLib/include/NBR/NBRNetDefine.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)

/**
 * @class NBRNetRecvListener
 * @brief NBR通信受信通知リスナー
 */
class NBRNetRecvListener
{
public:
  //------------------------------------------------------------------
  /**
   * @brief  通信終了通知
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvEnd(){}

  //------------------------------------------------------------------
  /**
   * @brief     ルール選ぶ人の候補者優先順位通知（子のみ）
   * @param[in] ルール選ぶ人の候補者優先順位（NetIdが要素で優先度が格納されている）
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvPriority( const u8 priority[NBRNetDefine::NET_NUM_MAX] ){}

  //------------------------------------------------------------------
  /**
   * @brief     ルール選択者通知
   * @param[in] ルール選択者通知コマンド
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvRuleSelector( const NBRNetDefine::RuleSelectorCommand command ){}

  //------------------------------------------------------------------
  /**
   * @brief  ルール選択を他の人に任せる通知
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvRuleOther(){}

  //------------------------------------------------------------------
  /**
   * @brief  ルール選択者が送信者になった通知
   * @param[in]   netId        送信者のNetId
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvRuleSelectorIsMe( int netId ){}

  //------------------------------------------------------------------
  /**
   * @brief  レギュレーション準備受信通知
   * @param[in]   isPreset    送信しようとしているレギュレーションがプリセットならtrue
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvRegulationReady( bool isPreset ){}

  //------------------------------------------------------------------
  /**
   * @brief  レギュレーション受信通知（プリセット）
   * @param[in]   presetIndex        プリセットレギュレーションの番号
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvRegulation( u8 presetIndex ){}
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
                                       const void* pCerdData, const u32 cerdDataSize ){}

  //------------------------------------------------------------------
  /**
   * @brief     レギュレーション受信結果通知
   * @param[in] レギュレーション受信結果通知コマンド
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvRegulationRet( const NBRNetDefine::RegulationRetCommand command ){}

  //------------------------------------------------------------------
  /**
   * @brief  勝負開始受信通知
   * @param[in] netId           通信ID
   * @param[in] bgm             選択されたBGM（ローカルで子のみ格納されている）
   * @param[in] isQrTeam        QRバトルチームかどうか
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvBattleStart( const int netId, const u32 bgm, const bool isQrTeam ){}

  //------------------------------------------------------------------
  /**
   * @brief  参加出来ない受信通知
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvCantParticipate(){}

  //------------------------------------------------------------------
  /**
   * @brief     ポケパーティ検査結果通知
   * @param[in] レポケパーティ検査結果コマンド
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvPokePartyCheckRet( const u8 command[NBRNetDefine::NET_NUM_MAX] ){}

  //------------------------------------------------------------------
  /**
   * @brief  マルチ振り分け決定通知
   * @param[in] NetIdが要素で戦う位置が格納されている
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvMultiDecide( const u8 place[4] ){}

  //------------------------------------------------------------------
  /**
   * @brief  マルチ振り分け通知
   * @param[in] netId   決めた人のNetId
   * @param[in] place   立ち位置（BTL_CLIETN_ID）
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvMultiSelect( const int netId, const u8 place ){}

  //------------------------------------------------------------------
  /**
   * @brief     続けて遊ぶ受信結果通知
   * @param[in] 続けて遊ぶ受信結果通知コマンド
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvContinue( const NBRNetDefine::ContinueCommand command ){}

};

GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // __NBR_NET_RECV_LISTENER_NET_H_INCLUDED__

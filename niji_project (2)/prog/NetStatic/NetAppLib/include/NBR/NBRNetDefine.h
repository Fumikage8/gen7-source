//======================================================================
/**
 * @file NBRNetDefine.h
 * @date 2015/07/08 15:02:39
 * @author ichiraku_katsuhiko
 * @brief 通信対戦受付アプリの通信定義系
 */
//======================================================================

#if !defined __NBR_NET_DEFINE_H_INCLUDED__
#define __NBR_NET_DEFINE_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gflnet2/include/p2p/gflnet2_P2pDefine.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)

/**
 * @class NBRNetDefine
 * @brief NBR通信定義
 */
class NBRNetDefine
{
public:

  static const u32 NET_NUM_MAX = gflnet2::p2p::CONNECT_MAX_FOUR; //! 最大接続人数

  /**
   * @enum TimingCommand
   * 同期コマンド
   */
  typedef enum
  {
    TIMING_COMMAND_END              = 30,   //! 通信終了同期
    TIMING_COMMAND_PRIORITY         = 31,   //! ルール候補者の優先順位通知 完了同期
    TIMING_COMMAND_RULE_SELECTOR    = 32,   //! ルール選択者通知 完了同期
    TIMING_COMMAND_RULE_SELECTOR_END= 33,   //! ルール選択者通知 完了同期
    // いらない TIMING_COMMAND_RULE_OTHER       = 33,   //! ルール選択を他の人に任せる通知 完了同期
    TIMING_COMMAND_REGULATION_SEND  = 34,   //! レギュレーション送信前の同期
    TIMING_COMMAND_REGULATION_RET   = 35,   //! レギュレーション受信結果送信  完了同期
    // いらない TIMING_COMMAND_TEAM_SELECT_WAIT1= 35,   //! チーム選択待ち同期　パーティ送信前
    TIMING_COMMAND_TEAM_SELECT_WAIT2= 36,   //! チーム選択待ち同期　パーティ送信後
    TIMING_COMMAND_CANT_PARTICIPATE = 37,   //! 参加出来ない通知 完了同期
    TIMING_COMMAND_POKEPARTY_CHECK  = 38,   //! ポケパーティ検査 完了同期
    TIMING_COMMAND_MULTI_DECIDE     = 39,   //! マルチ振り分け決定通知 完了同期
    TIMING_COMMAND_TO_BATTLE        = 40,   //! バトルへ行く最終同期
    TIMING_COMMAND_CONTINUE         = 41,   //! 続けて遊ぶ同期
  } TimingCommand;

  /**
   * @enum RuleSelectorCommand
   * ルール選択者通知コマンド
   */
  typedef enum
  {
    RULE_SELECTOR_COMMAND_MINE,     //! 自分が決める
    RULE_SELECTOR_COMMAND_OTHER,    //! 他の人にお任せする
  } RuleSelectorCommand;

  /**
   * @enum RegulationRetCommand
   * レギュレーション受信結果通知コマンド
   */
  typedef enum
  {
    REGULATION_RET_COMMAND_OK,        //! 問題なし
    REGULATION_RET_COMMAND_INVALID,   //! 不正なデータ
    REGULATION_RET_COMMAND_BROKEN,    //! 壊れていた
  } RegulationRetCommand;

  /**
   * @enum RegulationRetCommand
   * ポケパーティ検査結果通知コマンド
   */
  typedef enum
  {
    POKEPARTY_RET_OK,       //! 問題なし
    POKEPARTY_RET_REG_NG,   //! レギュレーション違反
  } PokePartyRetCommand;

  /**
   * @enum ContinueCommand
   * 続けて遊ぶ通知コマンド
   */
  typedef enum
  {
    CONTINUE_COMMAND_CHANGE,      //! ルール変えて遊ぶ
    CONTINUE_COMMAND_NOT_CHANGE,  //! ルール変えずに遊ぶ
  } ContinueCommand;

};

GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // __NBR_NET_DEFINE_H_INCLUDED__

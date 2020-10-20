//============================================================================================
/**
 * @file    P2pSyncSave.h
 * @brief   通信同期セーブ処理
 * @author  Akito Mori / 移植 endo_akira
 * @date    12.10.25 / 15.09.03
 *
 * @note 通信同期を行いながらセーブを行います。
 * @note 途中で片方の電源を切られた際などに反対側の人もセーブが終了しないようする機能になります。
 * @note 通信同期処理を必要とするため外からNetGameと同期に使用する番号を渡す必要があります
 *
 */
//============================================================================================
#if !defined( P2P_SYNC_SAVE_H_INCLUDE )
#define P2P_SYNC_SAVE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <gflnet2/include/p2p/gflnet2_NetGame.h>
#include <fs/include/CTR/gfl2_FsCtrBackupFile.h>
#include "GameSys/include/GameData.h"

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(P2P)



/// 通信同期セーブクラス
class P2pSyncSave {
public:
  /// セーブ中の通信同期に使用するパラメータ
  struct PARAM{
    GameSys::GameData*     gameData;  ///< ゲームデータポインタ
    gflnet2::p2p::NetGame* netGame;   ///< 現在使用中のネットゲームポインタ
    u32                    SyncNo1;   ///< １回目の通信同期に使用する番号
    u32                    SyncNo2;   ///< ２回目の通信同期に使用する番号
    u32                    SyncNo3;   ///< ３回目の通信同期に使用する番号
    gfl2::heap::HeapBase*  pHeap;     ///< システムヒープ
    u32                    StartWait; ///< セーブ開始までのウェイト
    u16                    sendMonsNo;///< 送信するMonsNo
    u8                     sendFormNo;///< 送信するフォルムNo
    u16                    recvMonsNo;///< 受信するMonsNo
    u8                     recvFormNo;///< 受信するフォルムNo
  };

  /// シーケンス
  enum{
    SEQ_INIT=0,           ///< 初期化
    SEQ_SYNCINIT1,        ///< 通信同期１
    SEQ_SYNCMAIN1,        ///< 通信同期１待ち
    SEQ_FIRST_SAVE_INIT,  ///< ９９％セーブ開始
    SEQ_FIRST_SAVE_WAIT,  ///< ９９％セーブ終了待ち
    SEQ_FLUCTUATION,      ///< 乱数による待ち
    SEQ_SYNCINIT2,        ///< 通信同期２
    SEQ_SYNCMAIN2,        ///< 通信同期２待ち　１％セーブ開始
    SEQ_LAST_SAVE_WAIT,   ///< １％セーブ終了待ち
    SEQ_SYNCINIT3,        ///< 通信同期３
    SEQ_SYNCMAIN3,        ///< 通信同期３待ち
  };


  static const u32 RANDOMFLUCTUATION = (10*30);   ///< 二回目のセーブには８かかる

  //--------------------------------------------------------------------------------------------
  /**
    * @brief   コンストラクタ（PARAMは内部にコピーされます）
    * @param   param        初期化パラメータ
    * @brief   purpose_fail 意図的にセーブを失敗するフラグ（テスト用）
    */
  //--------------------------------------------------------------------------------------------
  P2pSyncSave( PARAM *param, bool purpose_fail=false );

  //--------------------------------------------------------------------------------------------
  /// デストラクタ
  //--------------------------------------------------------------------------------------------
  ~P2pSyncSave(void);

  //=============================================================================================
  /**
   * @brief 伝説判定
   */
  //=============================================================================================
  bool IsPokeLegend( u16 monsNo , u8 formNo );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   通信同期セーブメイン
   *
   * @param   none
   *
   * @retval  true  = 終了
   * @retval  false = 処理中
   *
   */
  //--------------------------------------------------------------------------------------------
  int UpdateSyncSave( void );

private:
  GameSys::GameData*                m_gameData; ///< ゲームデータポインタ
  gflnet2::p2p::NetGame*            m_netGame;  ///< 現在使用中のネットゲームポインタ
  u32                               m_syncNo1;  ///< １回目の通信同期に使用する番号
  u32                               m_syncNo2;  ///< ２回目の通信同期に使用する番号
  u32                               m_syncNo3;  ///< ３回目の通信同期に使用する番号
  gfl2::heap::HeapBase*             m_heap;     ///< システムヒープ


  u32    mRandFluctuation;           ///< 二回目のセーブのあとのゆらぎ
  u32    m_seq;           ///< シーケンス管理用ワーク
  u32    m_wait;          ///< 処理スタートまでのウェイト値

  u16                    m_sendMonsNo;///< 送信するMonsNo
  u8                     m_sendFormNo;///< 送信するフォルムNo
  u16                    m_recvMonsNo;///< 受信するMonsNo
  u8                     m_recvFormNo;///< 受信するフォルムNo

#if PM_DEBUG
  bool   m_purpose_fail;  ///< わざとセーブを失敗させるフラグ
#endif

};


GFL_NAMESPACE_END(P2P)
GFL_NAMESPACE_END(NetLib)

#endif // P2P_SYNC_SAVE_H_INCLUDE

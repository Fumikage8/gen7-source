﻿// ============================================================================
/*
 * @file gflnet2_ICecSystem.h
 * @brief すれ違い通信のシステムクラスです。
 *        すれ違い通信の初期化処理や、終了処理などを行います。
 *        3DS本体のすれ違い通信ボックスを操作するインターフェースクラスを取得してから各種操作を行います。
 * @date 2015.02.13
 */
// ============================================================================
#if !defined( GFLNET2_ICECSYSTEM_H_INCLUDE )
#define GFLNET2_ICECSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <gflnet2/include/cec/gflnet2_CecMessageBoxCreateParam.h>


GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)

class ICecCoreMessageBox;
class ICecSendMessageBox;
class ICecRecvMessageBox;

class ICecSystem
{
public:

  virtual~ICecSystem(){}

  // --------------------------------------------------------------------------
  /**
   * @brief  すれ違い通信ライブラリの初期化を行います。
   * @param  pHeap すれ違い通信ライブラリ内で使用するヒープです。
   * @param  pInitParam すれ違い通信ライブラリの初期化パラメータです。
   */
  // --------------------------------------------------------------------------
  virtual void Initialize( gfl2::heap::HeapBase* pHeap, const gflnet2::cec::CecSystemInitParam* pInitParam ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  すれ違い通信ライブラリの終了処理を行います。
   */
  // --------------------------------------------------------------------------
  virtual void Terminate() = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  すれ違い通信ライブラリの更新処理を行います。
   *         主に非同期で実行された処理のスレッドを監視してたりします。
   */
  // --------------------------------------------------------------------------
  virtual void Update() = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  3DS本体のメッセージボックスを作成したり削除したりするクラスを取得します。
   * @return 3DS本体のメッセージボックスを作成したり削除したりするクラス
   */
  // --------------------------------------------------------------------------
  virtual ICecCoreMessageBox* GetCecCoreMessageBox() = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  送信ボックスに対して操作を行うクラスを取得します。
   * @return 送信ボックスに対して操作を行うクラス
   */
  // --------------------------------------------------------------------------
  virtual ICecSendMessageBox* GetCecSendMessageBox() = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  受信ボックスに対して操作を行うクラスを取得します。
   * @return 受信ボックスに対して操作を行うクラス
   */
  // --------------------------------------------------------------------------
  virtual ICecRecvMessageBox* GetCecRecvMessageBox() = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  3DS版かWin32版のすれ違い通信のシステムクラスを作成します。
   * @note   作成する関数はありますが、削除する関数はありません。
   *         削除する場合はアプリ側で delete する事になります。
   * @param  pHeap すれ違い通信のシステムクラスを作成する為のヒープです。
   * @return すれ違い通信のシステムクラス
   */
  // --------------------------------------------------------------------------
  static ICecSystem* CreateCecSystem( gfl2::heap::HeapBase* pHeap );

#if GFL_DEBUG
  // --------------------------------------------------------------------------
  /**
   * @brief  すれ違い通信のすれ違い状態をリセットします。
   *         リセットする事によって８時間待たなくてもすれ違い通信行われるようになります。
   *         この機能は3DS本体のデバッグモードを有効にしないと機能しません。
   */
  // --------------------------------------------------------------------------
  static void ResetCecInfo();
#endif // GFL_DEBUG

};


GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif // GFLNET2_ICECSYSTEM_H_INCLUDE

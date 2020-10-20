
//=============================================================================
/**
 * @file   gflnet2_RequestSequenceBase.h
 * @brief  リクエストシーケンス基底クラス
 * @author shin kosaka
 */
//=============================================================================
#ifndef __GFLNET2_REQUEST_SEQUENCE_BASE_H__
#define __GFLNET2_REQUEST_SEQUENCE_BASE_H__
#pragma once

#include <gflnet2/include/ServerClient/gflnet2_RequestHandle.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include  <debug/include/gfl2_DebugLogWindow.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(ServerClient)

//リクエストの状態
enum REQUEST_SEQUENCE_STATE
{
  REQUEST_STATE_CONTINUE = 0, //処理継続
  REQUEST_STATE_FINISH,       //処理終了
};

/***************************************************************************!
 * @brief  リクエストシーケンス基底クラス
 ***************************************************************************/
class RequestSequenceBase
{
  GFL_FORBID_COPY_AND_ASSIGN(RequestSequenceBase); //コピーコンストラクタ＋代入禁止

public:

  //------------------------------------------------------------------
  /**
  * @brief   コンストラクタ
  */
  //------------------------------------------------------------------
  RequestSequenceBase()
    : m_handle(INVALID_HANDLE)
    , m_isCancelRequested(false)
    , m_seq(0)
  {};

  //------------------------------------------------------------------
  /**
  * @brief   デストラクタ
  */
  //------------------------------------------------------------------
  virtual ~RequestSequenceBase(){};


  //------------------------------------------------------------------
  /**
  * @brief  更新処理。
  *
  * @note   	メインループ毎にポーリングされる
  *			      継承して実装する通信シーケンス本体
  *           クラスに用意されているセッター/ゲッターを使用して個別の
  *           通信シーケンスを実装してください
  *           キャンセル操作が行われた後はこのアップデートは呼ばれなくなり、
  *           UpdateCancelが呼ばれるようになります。
  */
  //------------------------------------------------------------------
  virtual REQUEST_SEQUENCE_STATE Update( void ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief  キャンセル処理の更新
  *
  * @note   	
  *			      リクエストをキャンセルするとUpdateではなくこちらが呼ばれるようになります。
  *           キャンセル処理が完了したらREQUEST_STATE_FINISHを返すとリクエストは終了します。
  *           キャンセルが行われなかった場合、この関数は呼ばれません。
  */
  //------------------------------------------------------------------
  virtual REQUEST_SEQUENCE_STATE UpdateCancel( void ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief  リスナー設定(テンプレート内の実装の為、継承先クラスで実装して下さい。)
  */
  //------------------------------------------------------------------
  //void SetListener( TListener* pListener ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief  リクエストパラメータ設定(テンプレート内の実装の為、必要に応じて継承先クラスで実装して下さい。)
  */
  //------------------------------------------------------------------
  //void SetRequestParam( TParam& pParam ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief  リスナー登録解除
  */
  //------------------------------------------------------------------
  virtual void RemoveListener( void ) = 0;

public:

  //外部に公開しているセッター/ゲッター
  void Cancel( void ){ m_isCancelRequested = true; };
  void SetRequestHandle( RequestHandle handle ){ m_handle = handle; };
  RequestHandle GetRequestHandle( void ) const { return m_handle; };
  bool IsCancelRequested( void ) const { return m_isCancelRequested; };

#if GFL_DEBUG
  //デバッグ用シーケンス番号取得
  u32 GetSequenceNoDebug( void ){ return m_seq; };
#endif

protected:

  //継承先クラスから操作可能なセッター/ゲッター
  void SetSequenceNo( u32 no )
  { 
    m_seq = no;
#if GFL_DEBUG
    gfl2::debug::LogWindow_AddMsg(gfl2::debug::LOGWIN_ID_NEX, L"RequestSequence[%d]\n",m_seq);
#endif
  };
  u32 GetSequenceNo( void ) const { return m_seq; };

private:

  RequestHandle m_handle;       //!<リクエスト識別用ハンドル
  bool m_isCancelRequested;     //!<キャンセル操作されたか
  u32 m_seq;                    //!<現在のシーケンス
};



GFL_NAMESPACE_END(ServerClient)
GFL_NAMESPACE_END(gflnet2)

#endif // __GFLNET2_REQUEST_SEQUENCE_BASE_H__



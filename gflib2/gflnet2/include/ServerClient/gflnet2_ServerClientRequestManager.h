//=============================================================================
/**
 * @file   gflnet2_ServerClientRequestManager.h
 * @brief  サーバ通信リクエスト管理
 * @author shin kosaka
 */
//=============================================================================
#ifndef __GFLNET2_SERVER_CLIENT_REQUEST_MANAGER_H__
#define __GFLNET2_SERVER_CLIENT_REQUEST_MANAGER_H__
#pragma once



#include <gflnet2/include/ServerClient/gflnet2_RequestHandle.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>


#include <base/include/gfl2_Singleton.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <util/include/gfl2_FixedSizeContainer.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(ServerClient)


/***************************************************************************!
 * @brief  サーバ通信リクエスト管理クラス
 ***************************************************************************/
class ServerClientRequestManager: public gfl2::base::Singleton<ServerClientRequestManager>
{
  GFL_FORBID_COPY_AND_ASSIGN(ServerClientRequestManager); //コピーコンストラクタ＋代入禁止

public:

  enum
  {
    MAX_REQUEST_NUM = 5,//リクエスト登録数最大5件まで
    WAIT_SLEEP_MSEC = 100,//通信終了待ちのスリープ時間
    FORCED_TIMEOUT = 10 * 60 * 5 * MAX_REQUEST_NUM,//強制タイムアウト（リクエスト x 5分）
  };

public:


  //------------------------------------------------------------------
  /**
  * @brief   コンストラクタ
  */
  //------------------------------------------------------------------
  ServerClientRequestManager();

  //------------------------------------------------------------------
  /**
  * @brief   デストラクタ
  */
  //------------------------------------------------------------------
  virtual ~ServerClientRequestManager();

  //------------------------------------------------------------------
  /**
  * @brief   初期化処理
  * @param[in] pHeap ヒープ
  */
  //------------------------------------------------------------------
  void Initialize( gfl2::heap::HeapBase* pHeap );

  //------------------------------------------------------------------
  /**
  * @brief   終了処理
  * @note    リクエストが残っているとリクエストの終了を待つので、
  　　　　　IsRequestEmptyがtrueになってから実行すること
  */
  //------------------------------------------------------------------
  void Finalize( void );

  //------------------------------------------------------------------
  /**
  * @brief  更新処理。
  *
  * @note   	メインループ毎にポーリングすること
  *			      通信処理やリスナーに通知されるイベントはすべてこの関数内で実行される
  */
  //------------------------------------------------------------------
  void Update( void );









public://スタティック関数群

  //------------------------------------------------------------------
  /**
  * @brief  実行中のリクエストが無いか確認
  *
  * @retval true： 実行中のリクエストが１つも無い
  * @retval false: 実行中のリクエストがある
  */
  //------------------------------------------------------------------
  static bool IsRequestEmpty( void );

  //------------------------------------------------------------------
  /**
  * @brief  通信リクエストを発行（パラメータあり）
  * @param[in] pListener 結果を受け取るリスナークラス
  *
  * @note
  * <TRequestSequence> 個別の通信シーケンス
  * <TListener>        通信シーケンスに対応したリスナー
  * <TParam>           通信シーケンスに対応したリクエストパラメータ
  *
  * @return リクエスト識別用ハンドル
  */
  //------------------------------------------------------------------
  template <typename TRequestSequence , typename TListener , typename TParam >
  static RequestHandle AddRequest( TListener* pListener , TParam& param );

  //------------------------------------------------------------------
  /**
  * @brief  通信リクエストを発行（パラメータ無し）
  * @param[in] pListener 結果を受け取るリスナークラス
  *
  * @note
  * <TRequestSequence> 個別の通信シーケンス
  * <TListener>        通信シーケンスに対応したリスナー
  *
  * @return リクエスト識別用ハンドル
  */
  //------------------------------------------------------------------
  template <typename TRequestSequence , typename TListener >
  static RequestHandle AddRequest( TListener* pListener );

  //------------------------------------------------------------------
  /**
  * @brief  指定したハンドルのリクエストが実行中かどうか確認
  * @param[in] handle リクエストハンドル
  * @retval true： 指定したリクエストが実行中
  * @retval false: 指定したリクエストは終了している
  */
  //------------------------------------------------------------------
  static bool IsRequestRunning( RequestHandle handle );

  //------------------------------------------------------------------
  /**
  * @brief  リクエストのキャンセル
  * @param[in] handle リクエストハンドル
  */
  //------------------------------------------------------------------
  static void CancelRequest( RequestHandle handle );

  //------------------------------------------------------------------
  /**
  * @brief  全リクエストのキャンセル
  */
  //------------------------------------------------------------------
  static void CancelAllRequest( void );


  //------------------------------------------------------------------
  /**
  * @brief 　 リスナー登録解除
  * @param[in] pListener 登録解除するリスナークラス
  */
  //------------------------------------------------------------------
  static void RemoveListener( RequestHandle handle );

  //------------------------------------------------------------------
  /**
  * @brief 　 全リスナーの登録解除
  */
  //------------------------------------------------------------------
  static void RemoveAllListener( void );

private:

  //------------------------------------------------------------------
  /**
  * @brief  実行中のリクエストが無いか確認
  *
  * @retval true： 実行中のリクエストが無い
  * @retval false: 実行中のリクエストがある
  */
  //------------------------------------------------------------------
  bool IsRequestEmptyInternal( void ) const;

  //------------------------------------------------------------------
  /**
  * @brief  通信シーケンス作成用のヒープを取得
  *
  * @return ヒープ
  */
  //------------------------------------------------------------------
  gfl2::heap::HeapBase* GetHeapInternal( void );

  //------------------------------------------------------------------
  /**
  * @brief 　ハンドルからリクエストシーケンスを取得
  * @param[in] handle リクエストハンドル
  * @return リクエストシーケンス
  */
  //------------------------------------------------------------------
  RequestSequenceBase* GetRequestSequenceInternal( RequestHandle handle );

  //------------------------------------------------------------------
  /**
  * @brief  リクエストシーケンスの追加
  * @param[in] pRequest リクエストシーケンス
  *
  * @return リクエストハンドル
  */
  //------------------------------------------------------------------
  RequestHandle AddRequestSequenceInternal( RequestSequenceBase* pRequest );

  
  //------------------------------------------------------------------
  /**
  * @brief  リクエストのキャンセル
  * @param[in] handle リクエストハンドル
  */
  //------------------------------------------------------------------
  void CancelRequestInternal( RequestHandle handle );

  //------------------------------------------------------------------
  /**
  * @brief  全リクエストのキャンセル
  */
  //------------------------------------------------------------------
  void CancelAllRequestInternal( void );

  //------------------------------------------------------------------
  /**
  * @brief 　 リスナー登録解除
  * @param[in] handle 登録解除するハンドル
  */
  //------------------------------------------------------------------
  void RemoveListenerInternal( RequestHandle handle );

  //------------------------------------------------------------------
  /**
  * @brief 　 全リスナーの登録解除
  */
  //------------------------------------------------------------------
  void RemoveAllListenerInternal( void );

  //------------------------------------------------------------------
  /**
  * @brief 　 リクエスト終了待ち
  * @retval true： 正常終了
  * @retval false:　強制タイムアウト
  */
  //------------------------------------------------------------------
  bool WaitRequestSequenceRunningInternal( void );

private:

  RequestHandle m_lastRequest;    //!<最後に発行したリクエストID
  gfl2::heap::HeapBase* m_pHeap;  //!<リクエスト実行用ヒープ
  u32 m_forcedTimeout;            //!<通信処理待ちの強制タイムアウト
  gfl2::util::FixedSizeContainer<RequestSequenceBase*,MAX_REQUEST_NUM>			m_requestSequenceArray; //!<リクエストキュー
};


GFL_NAMESPACE_END(ServerClient)
GFL_NAMESPACE_END(gflnet2)

// テンプレート実装
#include <gflnet2/source/ServerClient/gflnet2_ServerClientRequestManagerPrivate.h>

#endif // __GFLNET2_SERVER_CLIENT_REQUEST_MANAGER_H__



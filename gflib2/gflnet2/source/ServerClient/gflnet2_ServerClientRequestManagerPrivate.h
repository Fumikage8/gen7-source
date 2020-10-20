//=============================================================================
/**
 * @file   gflnet2_ServerClientRequestManagerPrivate.h
 * @brief  サーバ通信リクエストマネージャのテンプレート実装
 *         (実装を隠蔽する為のprivateヘッダ)
 * @author shin kosaka
 * @date   2014.9.10
 */
//=============================================================================
#ifndef __GFLNET2_SERVER_CLIENT_REQUEST_MANAGER_PRIVATE_H__
#define __GFLNET2_SERVER_CLIENT_REQUEST_MANAGER_PRIVATE_H__
#pragma once
#include <gflnet2/include/ServerClient/gflnet2_ServerClientRequestManager.h>

#include <base/include/gfl2_Singleton.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>


GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(ServerClient)



//------------------------------------------------------------------
/**
* @brief  通信リクエストを発行
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
RequestHandle ServerClientRequestManager::AddRequest( TListener* pListener , TParam& param )
{
  ServerClientRequestManager* pManager = GFL_SINGLETON_INSTANCE(ServerClientRequestManager);
  if( pManager == NULL )
  {
    GFL_ASSERT_MSG( 0 ,"ServerClientRequestManager is NULL" );
    return INVALID_HANDLE;
  }

  //シーケンスクラス生成
  gfl2::heap::HeapBase* pHeap = pManager->GetHeapInternal();
  if( pHeap == NULL )
  {
    GFL_ASSERT_MSG( 0 ,"heap is NULL" );
    return INVALID_HANDLE;
  }

	TRequestSequence* pSeq = GFL_NEW(pHeap) TRequestSequence;
  //リスナー設定
	pSeq->SetListener( pListener );
  //リクエストパラメータ設定
  pSeq->SetRequestParam( param );
	
	//シーケンスクラス登録
  return pManager->AddRequestSequenceInternal( pSeq );
}

//------------------------------------------------------------------
/**
* @brief  通信リクエストを発行
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
RequestHandle ServerClientRequestManager::AddRequest( TListener* pListener )
{
  ServerClientRequestManager* pManager = GFL_SINGLETON_INSTANCE(ServerClientRequestManager);
  if( pManager == NULL )
  {
    GFL_ASSERT_MSG( 0 ,"ServerClientRequestManager is NULL" );
    return INVALID_HANDLE;
  }

  //シーケンスクラス生成
  gfl2::heap::HeapBase* pHeap = pManager->GetHeapInternal();
  if( pHeap == NULL )
  {
    GFL_ASSERT_MSG( 0 ,"heap is NULL" );
    return INVALID_HANDLE;
  }

	TRequestSequence* pSeq = GFL_NEW(pHeap) TRequestSequence;
  //リスナー設定
	pSeq->SetListener( pListener );
	
	//シーケンスクラス登録
  return pManager->AddRequestSequenceInternal( pSeq );
}

GFL_NAMESPACE_END(ServerClient)
GFL_NAMESPACE_END(gflnet2)

#endif // __GFLNET2_SERVER_CLIENT_REQUEST_MANAGER_PRIVATE_H__
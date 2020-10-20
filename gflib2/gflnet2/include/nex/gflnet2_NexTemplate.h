﻿#ifdef  GF_PLATFORM_CTR

//=============================================================================
/**
 * @file   gflnet_NexTemplate.h
 *
 * @brief  nn::nexで汎用的に使用するテンプレート定義用ヘッダ
 *
 * @author h.suzuki
 */
//=============================================================================

#ifndef __GFLNET_NEX_TEMPLATE_H__
#define __GFLNET_NEX_TEMPLATE_H__
#pragma once


#include <nex.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2) 
GFL_NAMESPACE_BEGIN(nex)


//------------------------------------------------------------------
/**
 * @brief  nn::nex::CallContext::RegisterCompletionCallback(CallbackRoot*)の完了コールバック登録用テンプレートクラス
 */
//------------------------------------------------------------------
template<class T> class CallContextCallback : public nn::nex::Callback<T, nn::nex::CallContext*>
{
public:
    /**
     * @brief CallContextのコールバック関数を登録する
     *
     * @param pContext[in]     通知対象インスタンス
     * @param pFunc[in]        通知先関数
     * @param pCallContext[in] 通知対象CallContext
     */
    CallContextCallback(T* pContext, void (T::*pFunc)(nn::nex::CallContext*), nn::nex::CallContext *pCallContext)
        : nn::nex::Callback<T, nn::nex::CallContext*>(pContext, pFunc, pCallContext)
    {
        // コールバック後にpCallContextを削除する
        pCallContext->SetFlag(nn::nex::CallContext::DeleteOnCompletion);
        // 成功しても失敗してもコールバックする
        pCallContext->RegisterCompletionCallback(this, false);
    }
};



//------------------------------------------------------------------
/**
 * @brief Nex用メモリ解放テンプレート関数
 *
 * @param[in,out] obj 解放対象となるハンドル
 */
//------------------------------------------------------------------
template <class Object> void NexFreeMemory( Object** obj )
{
  if( *obj != NULL )
  {
    qDelete *obj;
    *obj = NULL;
  }
}


//------------------------------------------------------------------
/**
 * @brief  Nex用コレクションクラス解放テンプレート関数
 *
 * @note  解放前にコレクションのclear()が実行される。
 *
 * @param[in,out] col 解放対象となるハンドル
 */
//------------------------------------------------------------------
template <class Collection> void NexFreeCollection( Collection** col )
{
  if( *col != NULL )
  {
    (*col)->clear();
    qDelete *col;
    *col = NULL;
  }
}


//------------------------------------------------------------------
/**
 * @brief  Nex用コレクションクラスのワークメモリ解放テンプレート関数
 *
 * @note  コレクションのワークメモリがクリアされる。
 * @note  クリア前にコレクションのclear()が実行される。
 *
 * @param[in,out] col クリア対象となるコレクション
 */
//------------------------------------------------------------------
template <class Collection> void NexCleanCollectionMemory( Collection* col )
{
  if( col != NULL )
  {
    col->clear();
    Collection().swap( *col );
  }
}


GFL_NAMESPACE_END(nex)
GFL_NAMESPACE_END(gflnet2)

#endif  // __GFLNET_NEX_TEMPLATE_H__
#endif

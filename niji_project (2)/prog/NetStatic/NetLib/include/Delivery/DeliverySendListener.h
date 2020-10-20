#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_DeliverySendListener.h
 * @brief  配信システム送信リスナー
 * @author Shin Kosaka
 */
//=============================================================================

#ifndef __GFLNET_DELIVERY_SEND_LISTENER_H__
#define __GFLNET_DELIVERY_SEND_LISTENER_H__
#pragma once

#include <nn/Result.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Delivery)

//------------------------------------------------------------------
/**
 * @brief 配信システム送信リスナークラス
 *
 * @note 配信システムを利用して、送信処理関連の通知を受け取る場合は、
 *       本クラスを継承し仮想関数を実装してください。
 */
//------------------------------------------------------------------
class DeliverySendListener
{
public:
	//------------------------------------------------------------------
	/**
	* @brief ネットワークエラーイベント
	*
	* @param result 結果
	*/
	//------------------------------------------------------------------
	virtual void OnNetworkError( const nn::Result &result ) = 0;
};


GFL_NAMESPACE_END(Delivery)
GFL_NAMESPACE_END(NetLib)



#endif // __GFLNET_DELIVERY_SEND_LISTENER_H__
#endif

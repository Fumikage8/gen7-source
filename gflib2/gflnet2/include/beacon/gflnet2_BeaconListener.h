#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_BEACONLISTENER_H__
#define __GFLNET_BEACONLISTENER_H__

//=============================================================================
/**
 * @file   gflnet_BeaconListener.h
 * @brief  GFタイプ通信イベントハンドラ用クラス
 * @author h.suzuki
 *
 * @note 通信の完了通知を受け取る場合は、本クラスを継承し、
 *       仮想関数を実装してください。
 */
//=============================================================================
#pragma once
#include <nn.h>



#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(base)

class BeaconListener
{
  //GFL_FORBID_COPY_AND_ASSIGN(BeaconListener); //コピーコンストラクタ＋代入禁止
public:
  //------------------------------------------------------------------
  /**
  * @brief  ビーコン更新イベント
  *
  * @param  beacon 更新されたビーコン情報
  *                ※引き渡されたデータアドレス領域は永続保障されない。
  *                  ポインタを保持せず、自前のバッファにコピーすること。
  *
  * @param  localFriendCode ローカルフレンドコード。
  *                         フレンドユーザーのみ有効な値がセットされ、
  *                         その他のユーザーからのアクセス時には
  *                         NN_FRIENDS_INVALID_LOCAL_FRIEND_CODEがセットされる。
  *
  * @param  transferdId     送信者のtransferdId
  */
  //------------------------------------------------------------------
  virtual void OnUpdateBeacon( const void* beacon, u64 localFriendCode, u64 transferdId ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief  ビーコン削除前イベント
  *
  * @param  beacon 更新されるビーコン情報
  *                ※引き渡されたデータアドレス領域は永続保障されない。
  *                  ポインタを保持せず、自前のバッファにコピーすること。
  *
  *
  * @param  localFriendCode ローカルフレンドコード。
  *                         フレンドユーザーのみ有効な値がセットされ、
  *                         その他のユーザーからのアクセス時には
  *                         NN_FRIENDS_INVALID_LOCAL_FRIEND_CODEがセットされる。
  *
  * @param  transferdId     送信者のtransferdId
  */
  //------------------------------------------------------------------
  virtual void OnDeleteBeacon( const void* beacon, u64 localFriendCode, u64 transferdId ) = 0;
};


GFL_NAMESPACE_END(base)
GFL_NAMESPACE_END(gflnet2)



#endif // __GFLNET_BEACONLISTENER_H__
#endif

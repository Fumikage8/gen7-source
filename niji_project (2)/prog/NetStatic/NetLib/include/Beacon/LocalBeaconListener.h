//============================================================================================
/**
 * @file LocalBeaconListener.h
 *
 * @author kousaka_shin
 */
//============================================================================================
#ifndef __LOCAL_BEACON_LISTENER_
#define __LOCAL_BEACON_LISTENER_
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>



GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Beacon)

//--------------------------------------------------------------------------------------------
/**
* @brief ローカル通信ビーコンリスナー
*/
//--------------------------------------------------------------------------------------------
class LocalBeaconListener
{
public:
public:
  LocalBeaconListener(){};
  virtual ~LocalBeaconListener(){};
public:

  //------------------------------------------------------------------
  /**
  * @brief  ビーコンシステム再開時のコールバック（ビーコンデータの再設定が必要）
  */
  //------------------------------------------------------------------
  virtual void OnRestartBeacon( void ){};

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
  virtual void OnUpdateBeacon( const void* beacon, u64 localFriendCode, u64 transferdId ){};

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
  virtual void OnDeleteBeacon( const void* beacon, u64 localFriendCode, u64 transferdId ){};
};

GFL_NAMESPACE_END(Beacon)
GFL_NAMESPACE_END(NetLib)

#endif // __LOCAL_BEACON_LISTENER_
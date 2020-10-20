//============================================================================================
/**
 * @file		JoinFestaGuestSave.h
 * @brief		JoinFestaゲスト登録データ管理情報
 * @date    2015/10/13 15:02:39
 * @author  ichiraku_katsuhiko
 */
//============================================================================================
#ifndef __JOIN_FESTA_GUEST_SAVE_DATA_H__
#define __JOIN_FESTA_GUEST_SAVE_DATA_H__

#include <macro/include/gfl2_Macros.h>

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h"
#include "JoinFestaPersonalSave.h"

namespace Savedata {
namespace JoinFestaGuest{

//------------------------------------------------------------------------------------------
/**
* @brief  セーブデータ領域へのゲスト登録(グローバル関数)
*
* @param[in] friendKey 登録対象となるJoinFestaフレンドキー
*
* @retval true  追加された
* @retval false 空きがない・データがすでに存在する
*/
//------------------------------------------------------------------------------------------
extern bool  AddPersonalToJoinFestaGuestSaveData( const JoinFestaFriendKey& friendKey );

//------------------------------------------------------------------------------------------
/**
* @brief  セーブデータ領域へのゲスト情報を登録(グローバル関数)
*
* @param[in] targetPerson 登録対象となるJoinFestaパーソナルデータ
*
* @retval true  追加された
* @retval false 空きがない・データがすでに存在する
*/
//------------------------------------------------------------------------------------------
extern bool  AddPersonalToJoinFestaGuestSaveData( const NetAppLib::JoinFesta::JoinFestaPersonalData& targetPerson );

//------------------------------------------------------------------------------------------
/**
* @brief  セーブデータ領域のゲスト情報を削除(グローバル関数)
*
* @param[in] friendKey  削除対象となるJoinFestaフレンドキー
*
* @retval true  削除された
* @retval false 削除失敗
*/
//------------------------------------------------------------------------------------------
extern bool RemovePersonalFromJoinFestaGuestSaveData( const JoinFestaFriendKey& friendKey );

//------------------------------------------------------------------------------------------
/**
* @brief  セーブデータ領域のゲスト情報を更新(グローバル関数)
*
* @param[in] friendKey 更新対象となるフレンドキー
*
* @retval true  更新された
* @retval false 更新失敗
*/
//------------------------------------------------------------------------------------------
extern bool  UpdatePersonalToJoinFestaGuestSaveData( const JoinFestaFriendKey& friendKey );

//------------------------------------------------------------------------------------------
/**
* @brief  セーブデータ領域のゲスト情報が未登録か判定(グローバル関数)
*
* @retval true  未登録
* @retval false 1件以上登録あり
*/
//------------------------------------------------------------------------------------------
extern bool IsEmptyJoinFestaGuestSaveData( void );

//------------------------------------------------------------------------------------------
/**
* @brief  セーブデータ領域のゲスト情報が制限件数に達しているか判定(グローバル関数)
*
* @retval true  制限件数に達している
* @retval false 空きがある
*/
//------------------------------------------------------------------------------------------
extern bool IsFullJoinFestaGuestSaveData( void );

//------------------------------------------------------------------------------------------
/**
* @brief  指定JoinFestaフレンドキーのユーザーがセーブデータ領域のゲスト情報に存在するか判定（グローバル関数）
*
* @param[in] friendKey 判定対象JoinFestaフレンドキー
*
* @retval true  制限件数に達している
* @retval false 空きがある
*/
//------------------------------------------------------------------------------------------
extern bool IsExistJoinFestaGuestSaveData( const JoinFestaFriendKey& friendKey );

//------------------------------------------------------------------------------------------
/**
 * @brief  ジョインフェスタゲスト用セーブデータの内容をJoinFestaPersonalDataManagerに反映する
 */
//------------------------------------------------------------------------------------------
extern void ApplyJoinFestaGuestSaveData( void );

} // JoinFestaGuest
}	// Savedata



#endif // __JOIN_FESTA_GUEST_SAVE_DATA_H__


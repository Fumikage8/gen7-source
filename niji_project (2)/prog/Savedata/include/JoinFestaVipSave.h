//============================================================================================
/**
 * @file		JoinFestaVipSave.h
 * @brief		JoinFestaVIP登録データ管理情報
 * @date    2015/10/13 15:02:39
 * @author  ichiraku_katsuhiko
 */
//============================================================================================
#ifndef __JOIN_FESTA_VIP_SAVE_DATA_H__
#define __JOIN_FESTA_VIP_SAVE_DATA_H__

#include <macro/include/gfl2_Macros.h>

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h"
#include "JoinFestaPersonalSave.h"

namespace Savedata {
namespace JoinFestaVip{

//------------------------------------------------------------------------------------------
/**
* @brief  セーブデータ領域へのVIP登録(グローバル関数)
*
* @param[in] friendKey 登録対象となるJoinFestaフレンドキー
*
* @retval true  追加された
* @retval false 空きがない・データがすでに存在する
*/
//------------------------------------------------------------------------------------------
extern bool  AddPersonalToJoinFestaVipSaveData( const JoinFestaFriendKey& friendKey );

//------------------------------------------------------------------------------------------
/**
* @brief  セーブデータ領域へのVIP情報を登録(グローバル関数)
*
* @param[in] targetPerson 登録対象となるJoinFestaパーソナルデータ
*
* @retval true  追加された
* @retval false 空きがない・データがすでに存在する
*/
//------------------------------------------------------------------------------------------
extern bool  AddPersonalToJoinFestaVipSaveData( const NetAppLib::JoinFesta::JoinFestaPersonalData& targetPerson );

//------------------------------------------------------------------------------------------
/**
* @brief  セーブデータ領域のVIP情報を削除(グローバル関数)
*
* @param[in] friendKey  削除対象となるJoinFestaフレンドキー
*
* @retval true  削除された
* @retval false 削除失敗
*/
//------------------------------------------------------------------------------------------
extern bool RemovePersonalFromJoinFestaVipSaveData( const JoinFestaFriendKey& friendKey );

//------------------------------------------------------------------------------------------
/**
* @brief  セーブデータ領域のVIP情報を更新(グローバル関数)
*
* @param[in] friendKey 更新対象となるフレンドキー
*
* @retval true  更新された
* @retval false 更新失敗
*/
//------------------------------------------------------------------------------------------
extern bool  UpdatePersonalToJoinFestaVipSaveData( const JoinFestaFriendKey& friendKey );

//------------------------------------------------------------------------------------------
/**
* @brief  セーブデータ領域のVIP情報が未登録か判定(グローバル関数)
*
* @retval true  未登録
* @retval false 1件以上登録あり
*/
//------------------------------------------------------------------------------------------
extern bool IsEmptyJoinFestaVipSaveData( void );

//------------------------------------------------------------------------------------------
/**
* @brief  セーブデータ領域のVIP情報が制限件数に達しているか判定(グローバル関数)
*
* @retval true  制限件数に達している
* @retval false 空きがある
*/
//------------------------------------------------------------------------------------------
extern bool IsFullJoinFestaVipSaveData( void );

//------------------------------------------------------------------------------------------
/**
* @brief  指定JoinFestaフレンドキーのユーザーがセーブデータ領域のVIP情報に存在するか判定（グローバル関数）
*
* @param[in] friendKey 判定対象JoinFestaフレンドキー
*
* @retval true  制限件数に達している
* @retval false 空きがある
*/
//------------------------------------------------------------------------------------------
extern bool IsExistJoinFestaVipSaveData( const JoinFestaFriendKey& friendKey );

//------------------------------------------------------------------------------------------
/**
 * @brief  ジョインフェスタVIP用セーブデータの内容をJoinFestaPersonalDataManagerに反映する
 */
//------------------------------------------------------------------------------------------
extern void ApplyJoinFestaVipSaveData( void );

} // JoinFestaVip
}	// Savedata



#endif // __JOIN_FESTA_VIP_SAVE_DATA_H__


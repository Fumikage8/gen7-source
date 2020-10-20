//======================================================================
/**
 * @file BattleFesTool.h
 * @date 2016/11/25
 * @author munakata_kai
 * @brief バトルフェスの生成ツール郡
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BATTLE_FES_TOOL_H_INCLUDED__
#define __BATTLE_FES_TOOL_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <pml/include/pmlib.h>
#include <Savedata/include/MyStatus.h>
#include <Savedata/include/BattleFesSave.h>
#include <NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h>
#include "Field/FieldStatic/include/BattleFes/BattleFesDefine.h"
#include "Battle/include/battle_SetupParam.h"

GFL_NAMESPACE_BEGIN( BattleFesTool )

//------------------------------------------------------------------
/**
*  @brief    CheckPokeparaItemAvailability の結果
*/
//------------------------------------------------------------------
enum  PokeparaItemAvailabilityCheckResult
{
  CHECK_RESULT_NoItem,        /**<  アイテムを持っていない          */
  CHECK_RESULT_Available,     /**<  アイテムを持っており、使用可能  */
  CHECK_RESULT_NotAvailable,  /**<  アイテムを持っており、使用不可  */

  CHECK_RESULT_
};


//------------------------------------------------------------------
/**
*  @brief    pokeparaが所持しているアイテムを、Trainerは利用可能であるかを判定する
*            対象はZワザ、メガシンカ
*  @return   pokeparaがZワザ/メガシンカ用アイテムを所持しており、トレーナーが発動条件を満たしていればtrue
*/
//------------------------------------------------------------------
PokeparaItemAvailabilityCheckResult CheckPokeparaItemAvailability(const pml::pokepara::CoreParam* const pPokeCoreParam, const bool has_mega_ring, const bool has_z_ring);

//-----------------------------------------------------------------------------
/**
* @brief バトルツリーポケモン生成 返値のPokemonParam*は開放してください
* param[in] gfl2::heap::HeapBase*
* param[in] u16 バトルツリーＩＤ
* param[in] u8  性別
* param[in] u8　特性
* param[in] u8　ランク
* param[out] pml::pokepara::PokemonParam*
*/
//-----------------------------------------------------------------------------
pml::pokepara::PokemonParam* CreateBattleTreePokemon(gfl2::heap::HeapBase* pHeap, const u16 battle_tree_id, const int sex, const int tokusei, const u8 rank, const u8 has_mega_ring, const u8 has_z_ring);

//------------------------------------------------------------------
/**
  *  @brief    JoinFestaPersonalData から BatleFesCircle用のPokeparaを生成する
  */
//------------------------------------------------------------------
pml::pokepara::PokemonParam* CreateBattleTreePokemon(gfl2::heap::HeapBase* pHeap, const NetAppLib::JoinFesta::JoinFestaPersonalData& rJFPersonalData);


//-----------------------------------------------------------------------------
/**
* @brief ランダムでバトルツリーポケモン生成 返値のPokemonParam*は開放してください※フェスファンポケモン作成限定
* param[in] gfl2::heap::HeapBase*
* param[out] BATTLE_TREE_DATA*
* return pml::pokepara::PokemonParam*
*/
//-----------------------------------------------------------------------------
pml::pokepara::PokemonParam* CreateRandomBattleTreePokemon(gfl2::heap::HeapBase* pHeap, BattleFes::BATTLE_TREE_DATA* pBattleTreeData);

/* ----------------------------------------------------------------------------------------- */
/**
* @brief   ランダムで着せ替えパラメータ取得
* @param[in]  性別
* @param[in]  ヒープ
* @param[in]  セットする着せ替えのパラメータ
*/
/* ----------------------------------------------------------------------------------------- */
void GetDressUpParamRandom(const u32 sex, gfl2::heap::HeapBase* pHeap, poke_3d::model::DressUpParam* out_pDressUpParam);

/* ----------------------------------------------------------------------------------------- */
/**
* @brief   サングラス着用着せ替えパラメータ取得
* @param[in]  着せ替えのパラメータ
*/
/* ----------------------------------------------------------------------------------------- */
void GetSunglassDressUpParam(poke_3d::model::DressUpParam* out_pDressUpParam);

//------------------------------------------------------------------
/**
  *  @brief    JF内のPersonalDataが、受信してから長時間経過していないかを確認する
  *  @retval   true: 受信してからの経過時間が指定時間未満である
  *  @retval   false: 受信してからの経過時間が指定時間以上である
  */
//------------------------------------------------------------------
bool IsRecentPartner(const u32 elapsedHour);
bool IsRecentPartner(const NetAppLib::JoinFesta::JoinFestaPersonalData& rJFPersonal);

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief       バトルフェス用のパケットデータを受信しているかチェックする
 * @param[in]   rJFPersonal      チェックしたいパーソナルデータ
 * @return      trueで受信している
 */
/* ----------------------------------------------------------------------------------------- */
bool IsReceiveBattleFesPacket( const NetAppLib::JoinFesta::JoinFestaPersonalData& rJFPersonalData );

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief       通信人物の中から、バトルフェスに誘える人数を取得する
 * @return      誘う事が出来る人数
 */
/* ----------------------------------------------------------------------------------------- */
u32 GetBattleFesValidPatherCount();


//------------------------------------------------------------------
/**
  *  @brief    JoinFestaPersonalDataの内容をBattleFesSaveにセットする
  */
//------------------------------------------------------------------
void SetJFPersonalToBattleFesSave(Savedata::BattleFesSave& rDst, const u32 partnerIndex, const NetAppLib::JoinFesta::JoinFestaPersonalData& rSrc, gfl2::heap::HeapBase* pTempHeapBase);


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
struct  BFCTrainerInfo
{
  gfl2::str::STRCODE              name[System::STRLEN_PLAYER_NAME + System::EOM_LEN]; ///< 自身の名前   2Byte*STRLEN_PLAYER_NAME(12+1)文字*1 // 26
  Savedata::MyStatus::ICON_DATA   iconData;
  u32                             grade;

  /* ctor */
  BFCTrainerInfo(void)
    : name()
    , iconData()
    , grade(0)
  {
    gfl2::std::MemClear(&iconData, sizeof(iconData));
  }

  bool  Setup(const NetAppLib::JoinFesta::JoinFestaPersonalData& rJFPersonal);
  void  Setup(const Savedata::BattleFesSave& rBFSave, const u32 partnerIndex);
};

//--------------------------------------------------------------
/**
* @brief 今日のBtlFesField取得
* @return BtlFesField
*/
//-------------------------------------------------------------- 
BtlFesField GetBtlFieldType(const u8 rank);





GFL_NAMESPACE_END( BattleFesTool )

#endif // __BATTLE_FES_TOOL_H_INCLUDED__


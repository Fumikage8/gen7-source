//============================================================================================
/**
 * @file		ItemRecover.h
 * @brief		アイテム使用時のポケモン回復処理
 * @author	Hiroyuki Nakamura
 * @date		11.11.19
 */
//============================================================================================
#if !defined( ITEM_RECOVER_H_INCLUDED )
#define ITEM_RECOVER_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// pokelib
#include <pml/include/pmlib.h>


GFL_NAMESPACE_BEGIN(PokeTool)

//--------------------------------------------------------------------------------------------
/**
 * @brief		道具使用チェック処理 ( PokemonParam )
 *
 * @param		pp		回復するポケモンのデータ
 * @param		item	使用するアイテム
 * @param		pos		回復位置（技位置など）
 * @param		heap	ヒープ
 *
 * @retval	"true = 使用可能"
 * @retval	"false = 使用不可"
 */
//--------------------------------------------------------------------------------------------
extern bool ITEM_RCV_RecoverCheck( const pml::pokepara::PokemonParam * pp, u16 item, u16 pos, gfl2::heap::HeapBase * heap );

//--------------------------------------------------------------------------------------------
/**
 * @brief		道具使用チェック処理 ( PokeParty )
 *
 * @param		party		回復するポケモンのデータ
 * @param		item		使用するアイテム
 * @param		pos1		ポケモンデータの中の位置
 * @param		pos2		回復位置（技位置など）
 * @param		heap		ヒープ
 *
 * @retval	"true = 使用可能"
 * @retval	"false = 使用不可"
 */
//--------------------------------------------------------------------------------------------
extern bool ITEM_RCV_PokeParty_RecoverCheck( pml::PokeParty * party, u16 item, u8 pos1, u8 pos2, gfl2::heap::HeapBase * heap );

//--------------------------------------------------------------------------------------------
/**
 * @brief   道具使用チェック処理 ( PokemonParam/ITEM_MANAGER )
 *
 * @param   pp        回復するポケモンのデータ
 * @param   item      使用するアイテム
 * @param   pos       回復位置（技位置など）
 * @param   manager   アイテムマネージャー
 *
 * @retval  "true = 使用可能"
 * @retval  "false = 使用不可"
 */
//--------------------------------------------------------------------------------------------
extern bool ITEM_RCV_RecoverCheck( const pml::pokepara::PokemonParam * pp, u16 item, u16 pos, itemman::ITEM_MANAGER * manager );

//--------------------------------------------------------------------------------------------
/**
 * @brief   道具使用チェック処理 ( PokeParty/ITEM_MANAGER )
 *
 * @param   party     回復するポケモンのデータ
 * @param   item      使用するアイテム
 * @param   pos1      ポケモンデータの中の位置
 * @param   pos2      回復位置（技位置など）
 * @param   manager   アイテムマネージャー
 *
 * @retval  "true = 使用可能"
 * @retval  "false = 使用不可"
 */
//--------------------------------------------------------------------------------------------
extern bool ITEM_RCV_PokeParty_RecoverCheck( pml::PokeParty * party, u16 item, u8 pos1, u8 pos2, itemman::ITEM_MANAGER * manager );

//--------------------------------------------------------------------------------------------
/**
 * @brief		回復処理 ( PokemonParam )
 *
 * @param		pp		回復するポケモンのデータ
 * @param		item	使用するアイテム
 * @param		pos		回復位置（技位置など）
 * @param		place	場所
 * @param		heap	ヒープ
 *
 * @retval	"true = 使用可能"
 * @retval	"false = 使用不可"
 */
//--------------------------------------------------------------------------------------------
extern bool ITEM_RCV_Recover(
              pml::pokepara::PokemonParam * pp, u16 item, u16 pos, u16 place, gfl2::heap::HeapBase * heap );

//--------------------------------------------------------------------------------------------
/**
 * @brief		回復処理 ( PokeParty )
 *
 * @param		party		回復するポケモンのデータ
 * @param		item		使用するアイテム
 * @param		pos1		ポケモンデータの中の位置
 * @param		pos2		回復位置（技位置など）
 * @param		place		場所
 * @param		heap		ヒープ
 *
 * @retval	"true = 使用可能"
 * @retval	"false = 使用不可"
 */
//--------------------------------------------------------------------------------------------
extern bool ITEM_RCV_PokeParty_Recover(
              pml::PokeParty * party, u16 item, u8 pos1, u8 pos2, u16 place, gfl2::heap::HeapBase * heap );

//--------------------------------------------------------------------------------------------
/**
 * @brief   回復処理 ( PokemonParam/ITEM_MANAGER )
 *
 * @param   pp        回復するポケモンのデータ
 * @param   item      使用するアイテム
 * @param   pos       回復位置（技位置など）
 * @param   place     場所
 * @param   manager   アイテムマネージャー
 *
 * @retval  "true = 使用可能"
 * @retval  "false = 使用不可"
 */
//--------------------------------------------------------------------------------------------
extern bool ITEM_RCV_Recover(
              pml::pokepara::PokemonParam * pp, u16 item, u16 pos, u16 place, itemman::ITEM_MANAGER * manager );

//--------------------------------------------------------------------------------------------
/**
 * @brief   回復処理 ( PokeParty/ITEM_MANAGER )
 *
 * @param   party     回復するポケモンのデータ
 * @param   item      使用するアイテム
 * @param   pos1      ポケモンデータの中の位置
 * @param   pos2      回復位置（技位置など）
 * @param   place     場所
 * @param   manager   アイテムマネージャー
 *
 * @retval  "true = 使用可能"
 * @retval  "false = 使用不可"
 */
//--------------------------------------------------------------------------------------------
extern bool ITEM_RCV_PokeParty_Recover(
              pml::PokeParty * party, u16 item, u8 pos1, u8 pos2, u16 place, itemman::ITEM_MANAGER * manager );


GFL_NAMESPACE_END(PokeTool)

#endif // ITEM_RECOVER_H_INCLUDED

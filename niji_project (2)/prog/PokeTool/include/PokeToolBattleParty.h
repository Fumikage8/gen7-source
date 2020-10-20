//=============================================================================
/**
 * @file    PokeToolBattleParty.h
 * @author  k.ohno
 * @brief   ポケモンバトルパーティーにかかわるクラス
 * @date    2015.08.07
 */
//=============================================================================
#if !defined( __POKE_TOOL_BATTLE_PARTY_H__ )
#define __POKE_TOOL_BATTLE_PARTY_H__
#pragma once

#include <pml/include/pmlib.h>
#include "Savedata/include/BoxPokemonSave.h"
#include <str/include/gfl2_StrBuf.h>

GFL_NAMESPACE_BEGIN(PokeTool)

class BattleParty{

public:
	//------------------------------------------------------------------------------------------
	/**
	 * @brief	コンストラクタ＆デストラクタ  PokePartyとStrBufを必要分確保します
	 */
	//------------------------------------------------------------------------------------------
  BattleParty(gfl2::heap::HeapBase* pHeap);
  ~BattleParty();
	//------------------------------------------------------------------------------------------
	/**
	 * @brief	    引数に従ったチームのバトルパーティーを生成する
   * @param     BOX     ボックスのポインタ
   * @param     BoxPokemon     ボックスポケモンのポインタ
   * @param     teamNo  バトルチーム番号 0 - 5
   * @param     pHeap   tempヒープ
   * @return    ポケモン数
   * @attention 開放はデストラクタで行われる
	 */
	//------------------------------------------------------------------------------------------
  int CreateBattleParty( const Savedata::BOX* pBox, Savedata::BoxPokemon* pBoxPoke, int teamNo, gfl2::heap::HeapBase* pHeap);
	//------------------------------------------------------------------------------------------
	/**
	 * @brief	    ポケパーティーを得る  
   * @return    PokePartyポインタ
   * @caution CreateBattlePartyの後にできる
	 */
	//------------------------------------------------------------------------------------------
  pml::PokeParty* GetPokeParty(void){ return  mpPokeParty; };
	//------------------------------------------------------------------------------------------
	/**
	 * @brief	    チーム名を得る
   * @return    チーム名
	 */
	//------------------------------------------------------------------------------------------
  gfl2::str::StrBuf* GetTeamName(void){ return  mpName; };
	//------------------------------------------------------------------------------------------
	/**
	 * @brief	    ロックされたチーム名をえる
   * @return    ロックしてたらtrue
	 */
	//------------------------------------------------------------------------------------------
  bool IsLock(void){ return mIsLocked; };

	//------------------------------------------------------------------------------------------
	/**
	 * @brief	    QRバトルチームかどうか
   * @return    QRバトルチームだった場合true
	 */
	//------------------------------------------------------------------------------------------
  bool IsQrTeam(void){ return mIsQrTeam; };


private:

  //  名前
  gfl2::str::StrBuf*  mpName;
  //  ポケモンパーティー
  pml::PokeParty*     mpPokeParty;
  // ロックしてるかどうか
  bool mIsLocked;
  // QRバトルチームかどうか
  bool mIsQrTeam;
};





GFL_NAMESPACE_END(PokeTool);

#endif  // __POKE_TOOL_BATTLE_PARTY_H__

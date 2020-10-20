//=============================================================================
/**
 * @file    BagPokemonListener.h
 * @brief   バッグ画面ポケモンプレートリスナー
 * @author  Hiroyuki Nakamura
 * @date    2015.04.01
 */
//=============================================================================
#if !defined( BAG_POKEMON_LISTENER_H_INCLUDED )
#define BAG_POKEMON_LISTENER_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(bag)

//=============================================================================
/**
 * @class BagPokemonListener
 * @brief バッグ画面ポケモンプレートリスナークラス
 * @date  2015.04.01
 */
//=============================================================================
class BagPokemonListener
{
public:
  //--------------------------------------------------------------------------
  /**
   * @func    SelectPlate
   * @brief   プレート選択時に呼ばれるリスナー
   * @date    2015.04.10
   *
   * @param   id  動作ID
   */
  //--------------------------------------------------------------------------
  virtual void SelectPlate( u32 id ) = 0;

  //--------------------------------------------------------------------------
  /**
   * @func    PokeItemCatch
   * @brief   ポケモンの持ち物取得動作時に呼ばれるリスナー
   * @date    2015.05.01
   *
   * @param   x         タッチX座標
   * @param   y         タッチY座標
   * @param   poke_pos  タッチ中のポケモン位置
   */
  //--------------------------------------------------------------------------
  virtual void PokeItemCatch( u16 x, u16 y, u32 poke_pos ) = 0;
};

GFL_NAMESPACE_END(bag)
GFL_NAMESPACE_END(app)

#endif  // BAG_POKEMON_LISTENER_H_INCLUDED

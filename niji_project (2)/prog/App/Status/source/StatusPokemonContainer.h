#if !defined( __APP_STATUS_POKEMON_CONTAINER_H_INCLUDED__ )
#define __APP_STATUS_POKEMON_CONTAINER_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    StatusPokemonContainer.h
 * @date    2015/12/01 19:05:24
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：ポケモンコンテナ
 * note     ポケパーティとボックスの違いに関係なくポケモンパラメータを取得するためのクラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <heap/include/gfl2_Heap.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(Savedata)
class BoxPokemon;
GFL_NAMESPACE_END(Savedata)

GFL_NAMESPACE_BEGIN(pml)
class PokeParty;
GFL_NAMESPACE_BEGIN(pokepara)
class PokemonParam;
GFL_NAMESPACE_END(pokepara)
GFL_NAMESPACE_END(pml)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   ポケモンパラメータコンテナ
//------------------------------------------------------------------------------
class PokemonContainer
{
  GFL_FORBID_COPY_AND_ASSIGN( PokemonContainer );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   workHeap   ヒープ
   */
  //------------------------------------------------------------------
  PokemonContainer( gfl2::heap::HeapBase* workHeap );
  
  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~PokemonContainer( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   パーティデータのセット
   * @param   party   ポケパーティ
   */
  //------------------------------------------------------------------
  void SetPokeParty( pml::PokeParty* party );

  //------------------------------------------------------------------
  /**
   * @brief   ボックスデータのセット
   * @param   boxData     ボックスデータ
   * @param   trayIndex   トレイインデックス
   */
  //------------------------------------------------------------------
  void SetBoxData( Savedata::BoxPokemon* boxData, u32 trayIndex );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンパラメータの取得
   * @param   index   インデックス
   * @param   buffer  ポケモンパラメータの格納先
   */
  //------------------------------------------------------------------
  void GetPokemon( u32 index, pml::pokepara::PokemonParam* buffer );

  //------------------------------------------------------------------
  /**
   * @brief   次のインデックスの取得
   * @param   currentIndex    現在のインデックス
   * @return  次のインデックス
   */
  //------------------------------------------------------------------
  u32 GetNextPokeIndex( u32 currentIndex );

  //------------------------------------------------------------------
  /**
   * @brief   前のインデックスの取得
   * @param   currentIndex    現在のインデックス
   * @return  前のインデックス
   */
  //------------------------------------------------------------------
  u32 GetPrevPokeIndex( u32 currentIndex );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンパラメータの更新
   * @param   index   更新対象インデックス
   * @param   pp      更新するパラメータ
   */
  //------------------------------------------------------------------
  void UpdatePokemon( u32 index, const pml::pokepara::PokemonParam& pp );

private:  //!< ポケパーティ
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンパラメータの取得
   * @param   index   インデックス
   * @param   buffer  ポケモンパラメータの格納先
   */
  //------------------------------------------------------------------
  void GetPokemonFromPokeParty( u32 index, pml::pokepara::PokemonParam* buffer );

  //------------------------------------------------------------------
  /**
   * @brief   次のインデックスの取得
   * @param   currentIndex    現在のインデックス
   * @return  次のインデックス
   */
  //------------------------------------------------------------------
  u32 GetNextPokeIndexFromPokeParty( u32 currentIndex );

  //------------------------------------------------------------------
  /**
   * @brief   前のインデックスの取得
   * @param   currentIndex    現在のインデックス
   * @return  前のインデックス
   */
  //------------------------------------------------------------------
  u32 GetPrevPokeIndexFromPokeParty( u32 currentIndex );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンパラメータの更新
   * @param   index   更新対象インデックス
   * @param   pp      更新するパラメータ
   */
  //------------------------------------------------------------------
  void UpdatePokemonOfPokeParty( u32 index, const pml::pokepara::PokemonParam& pp );

private:  //!< ボックスデータ
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンパラメータの取得
   * @param   index   インデックス
   * @param   buffer  ポケモンパラメータの格納先
   */
  //------------------------------------------------------------------
  void GetPokemonFromBoxData( u32 index, pml::pokepara::PokemonParam* buffer );

  //------------------------------------------------------------------
  /**
   * @brief   次のインデックスの取得
   * @param   currentIndex    現在のインデックス
   * @return  次のインデックス
   */
  //------------------------------------------------------------------
  u32 GetNextPokeIndexFromBoxData( u32 currentIndex );

  //------------------------------------------------------------------
  /**
   * @brief   前のインデックスの取得
   * @param   currentIndex    現在のインデックス
   * @return  前のインデックス
   */
  //------------------------------------------------------------------
  u32 GetPrevPokeIndexFromBoxData( u32 currentIndex );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンパラメータの更新
   * @param   index   更新対象インデックス
   * @param   pp      更新するパラメータ
   */
  //------------------------------------------------------------------
  void UpdatePokemonOfBoxData( u32 index, const pml::pokepara::PokemonParam& pp );

public:
  //------------------------------------------------------------------
  /**
   * @brief   パーティデータのセット
   * @param   party   ポケパーティ
   */
  //------------------------------------------------------------------
  pml::PokeParty* GetPokeParty( void ) const;

private:  //!< メソッドの呼びわけ
  typedef void ( PokemonContainer::*MethodPointer_GetPokemon )( u32, pml::pokepara::PokemonParam* );
  typedef u32 ( PokemonContainer::*MethodPointer_GetNextPokeIndex )( u32 );
  typedef u32 ( PokemonContainer::*MethodPointer_GetPrevPokeIndex )( u32 );
  typedef void ( PokemonContainer::*MethodPointer_UpdatePokemon )( u32, const pml::pokepara::PokemonParam& );

  MethodPointer_GetPokemon        m_pMethod_GetPokemon;
  MethodPointer_GetNextPokeIndex  m_pMethod_NextPokeIndex;
  MethodPointer_GetPrevPokeIndex  m_pMethod_PrevPokeIndex;
  MethodPointer_UpdatePokemon     m_pMethod_UpdatePokemon;

private:
  gfl2::heap::HeapBase*   m_pHeap;

  pml::PokeParty*         m_pParty;

  Savedata::BoxPokemon*   m_pBoxData;

  u32                     m_BoxTrayIndex;
};

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

#endif // __APP_STATUS_POKEMON_CONTAINER_H_INCLUDED__

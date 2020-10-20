//=============================================================================
/**
 * @file    UnionPokemonSave.h
 * @brief   セーブデータ：合体ポケモンの退避領域
 * @author  Hiroyuki Nakamura
 * @date    2012.06.13
 */
//=============================================================================
#if !defined( UNION_POKEMON_H_INCLUDED )
#define UNION_POKEMON_H_INCLUDED

#pragma once

// gflib2
#include <util/include/gfl2_std_string.h>
// pokelib
#include <pml/include/pmlib.h>
// niji
#include "SaveDataInterface.h"


GFL_NAMESPACE_BEGIN(Savedata)

//=============================================================================
/**
 * @class UnionPokemon
 * @brief 合体ポケモン退避領域
 * @date  2012.06.13
 */
//=============================================================================
class UnionPokemon
  : public SaveDataInterface
{
  GFL_FORBID_COPY_AND_ASSIGN(UnionPokemon);   // コピーコンストラクタ＋代入禁止

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //-----------------------------------------------------------------------------
  UnionPokemon(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~UnionPokemon();


public:
  //! データID
  enum PokemonID
  {
    POKEID_KYUREMU = 0,   //!< キュレム
    POKEID_NEKUROZUMA_S,  //!< ネクロズマ・ソル
    POKEID_NEKUROZUMA_L,  //!< ネクロズマ・ルナ
    POKEID_MAX,           //!< 最大数
  };

private:
  //! ポケモンデータ
	struct Data {
    u8	pokemonParam[ POKEID_MAX ][ pml::pokepara::PokemonParam::DATASIZE ];  //!< PokemonParam
  };

  Data poke;  //!< ポケモンデータ


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   読み込んだデータをセットする関数
   * @param   pData   先頭のポインタ
   */
  //-----------------------------------------------------------------------------
  virtual void SetData(void * pData){ gfl2::std::MemCopy( pData, reinterpret_cast<void*>(&poke), sizeof(Data) ); };

  //-----------------------------------------------------------------------------
  /**
   * @brief   データの先頭ポインタを得る関数
   * @return  先頭のポインタ
   */
  //-----------------------------------------------------------------------------
  virtual void * GetData(void){ return reinterpret_cast<void*>(&poke); };

  //-----------------------------------------------------------------------------
  /**
   *  @brief  データサイズ
   *  @return バイト
   */
  //-----------------------------------------------------------------------------
	virtual size_t GetDataSize(void){ return sizeof(Data); };

  //-----------------------------------------------------------------------------
  /**
   * @brief セーブデータのクリア
   * @param heap  クリアに使用するテンポラリヒープ
   */
  //-----------------------------------------------------------------------------
  virtual void Clear( gfl2::heap::HeapBase * heap );


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   初期化
   *
   * @param   id    データID
   * @param   heap  ヒープ
   */
  //-----------------------------------------------------------------------------
  void InitPokemonData( PokemonID id, gfl2::heap::HeapBase * heap );

  //-----------------------------------------------------------------------------
  /**
   * @brief   PokemonParamをセット
   *
   * @param   id  データID
   * @param   pp  セットするPokemonParam
   */
  //-----------------------------------------------------------------------------
  void SetPokemon( PokemonID id, pml::pokepara::PokemonParam * pp );

  //-----------------------------------------------------------------------------
  /**
   * @brief   PokemonParamを取得
   *
   * @param   id  データID
   * @param   pp  取得場所
   */
  //-----------------------------------------------------------------------------
  void GetPokemon( PokemonID id, pml::pokepara::PokemonParam * pp );
};

GFL_NAMESPACE_END(Savedata)

#endif  // UNION_POKEMON_H_INCLUDED

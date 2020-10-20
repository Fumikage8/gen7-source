#ifndef NIJI_PROJECT_APP_POKELIST_INTERMEDIATE_DATA_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_INTERMEDIATE_DATA_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    PokeListIntermediateData.h
 * @author  fukushima_yuya
 * @date    2015.03.31
 * @brief   PokeParamを取得するための中間データクラス
 */
//==============================================================================

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_heap_base.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(pml)
class PokeParty;
GFL_NAMESPACE_BEGIN(pokepara)
class PokemonParam;
GFL_NAMESPACE_END(pokepara)
GFL_NAMESPACE_END(pml)

GFL_NAMESPACE_BEGIN(btl)
class BTL_PARTY;
GFL_NAMESPACE_END(btl)


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
// @brief   中間データクラス
//------------------------------------------------------------------------------
class IntermediateData
{
  //GFL_FORBID_COPY_AND_ASSIGN( IntermediateData );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  IntermediateData( void );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~IntermediateData();


public:
  //------------------------------------------------------------------
  /**
   * @brief   パラメータのセット
   * @param   pPokeParty    ポケモンパーティ
   */
  //------------------------------------------------------------------
  void SetParameter( pml::PokeParty* pPokeParty );

  //------------------------------------------------------------------
  /**
   * @brief   パラメータのセット
   * @param   pPokeParty    ポケモンパーティ
   */
  //------------------------------------------------------------------
  void SetParameter( const pml::PokeParty* pPokeParty );

  //------------------------------------------------------------------
  /**
   * @brief   パラメータのセット
   * @param   pBtlParty     バトルシステム　ポケモンパーティ
   */
  //------------------------------------------------------------------
  void SetParameter( const btl::BTL_PARTY* pBtlParty );
  void SetParameter( const btl::BTL_PARTY* pBtlParty, gfl2::heap::HeapBase* heap );

  //------------------------------------------------------------------
  /**
   * @brief   パラメータが無効かどうか
   *
   * @return  "true  == 無効"
   * @return  "false == 有効"
   */
  //------------------------------------------------------------------
  bool IsInvalid( void ) const;


public:
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンパーティの取得
   * @return  pml::PokePartyクラスのポインタ
   */
  //------------------------------------------------------------------
  pml::PokeParty* GetPokeParty( void );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンパーティの取得
   * @return  pml::PokePartyクラスのポインタ
   */
  //------------------------------------------------------------------
  const pml::PokeParty* GetPokePartyConst( void );

  //------------------------------------------------------------------
  /**
   * @brief   バトル用ポケモンパーティの取得
   * @return  btl::BTL_PARTYクラスのポインタ
   */
  //------------------------------------------------------------------
  const btl::BTL_PARTY* GetBtlParty( void );


public:
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンの取得
   * @return  pml::pokepara::PokemonParamクラスのポインタ
   */
  //------------------------------------------------------------------
  pml::pokepara::PokemonParam* GetPoke( u8 poke_index );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンの取得
   * @return  const pml::pokepara::PokemonParamクラスのポインタ
   */
  //------------------------------------------------------------------
  const pml::pokepara::PokemonParam* GetPokeConst( u8 poke_index );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンの取得(バトル専用)
   * @return  const pml::pokepara::PokemonParamクラスのポインタ
   */
  //------------------------------------------------------------------
  const pml::pokepara::PokemonParam* GetPoke_Battle( u8 poke_index );


public:
  //------------------------------------------------------------------
  /**
   * @brief   パーティのメンバー数の取得
   * @return  パーティのメンバー数
   */
  //------------------------------------------------------------------
  u32 GetMemberCount( void );

  //------------------------------------------------------------------
  /**
   * @brief   状態異常・猛毒か
   * @retval  true  = 猛毒
   * @retval  false = それ以外
   */
  //------------------------------------------------------------------
  bool IsSickMoudoku( u8 index );

  //------------------------------------------------------------------
  /**
   * @brief   バトル用パラメータの破棄
   */
  //------------------------------------------------------------------
  void DeleteBattleData( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   バトル用パラメータの生成
   */
  //------------------------------------------------------------------
  void CreateBattleData( gfl2::heap::HeapBase * heap, u8 memberCount );

  //------------------------------------------------------------------
  /**
   * @brief   戦闘中のデータの反映
   */
  //------------------------------------------------------------------
  void SetBattleData( u8 index );


private:
  pml::PokeParty*         m_pPokeParty;
  const pml::PokeParty*   m_cpPokeParty;
  const btl::BTL_PARTY*   m_pBtlParty;

  static const u8 MEMBER_MAX = 6;
  pml::pokepara::PokemonParam * m_pBattleParam[MEMBER_MAX];
  bool m_CreateBattleData;
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )


#endif  // #ifndef NIJI_PROJECT_APP_POKELIST_INTERMEDIATE_DATA_H_INCLUDED

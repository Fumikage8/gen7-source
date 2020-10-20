//=============================================================================
/**
 * @brief 全力技システム
 * @file pml_ZenryokuWazaSystem.h
 * @author obata_toshihiro
 * @date 2015.05.27
 */
//=============================================================================
#ifndef PML_ZENRYOKUWAZASYSTEM_H_INCLUDED
#define PML_ZENRYOKUWAZASYSTEM_H_INCLUDED
#pragma once

#include <pml/include/pml_Type.h>

namespace pml {
namespace pokepara {
  class PokemonParam;
} // pokepara
} // pml

namespace pml {
  namespace waza { 


    /**
     * @brief 全力技の種類
     */
    enum ZenryokuWazaKind
    {
      ZENRYOKUWAZA_KIND_NULL,     // 全力技でない
      ZENRYOKUWAZA_KIND_BOOST,    // ○○ブースト
      ZENRYOKUWAZA_KIND_GENERAL,  // 汎用全力技( 各タイプ、物理・特殊の２つずつ )
      ZENRYOKUWAZA_KIND_SPECIAL,  // 専用全力技
      ZENRYOKUWAZA_KIND_NUM,
    };


    /**
     * @class ZenryokuWazaSystem
     * @brief 全力技に関するメソッド群
     */
    class ZenryokuWazaSystem
    {
    public:

      /**
       * @brief 全力技の元となる情報
       */
      struct ZenryokuWazaSource
      {
        MonsNo monsNo;  // 技を使用するポケモンのモンスターNo.
        FormNo formNo;  // 技を使用するポケモンのフォルムNo.
        ItemNo itemNo;  // 技を使用するポケモンが持っている道具
        WazaNo wazaNo;  // 元技の技No.
      };


      //---------------------------------------------------------------------
      /**
       * @brief 汎用全力技を使用するための道具を取得する
       *
       * @param wazaType  技のタイプ
       *
       * @retval ITEM_DUMMY_DATA  対応する道具が存在しない場合( 引数が不正値の場合 )
       */
      //---------------------------------------------------------------------
      static ItemNo GetItemToUseGeneralZenryokuWaza( PokeType wazaType );

      //---------------------------------------------------------------------
      /**
       * @brief 全力技を使用する条件を満たしているか？
       *
       * @param[in]  source  全力技の元
       *
       * @retval true   全力技を使用する条件を満たしている
       * @retval false  全力技を使用する条件を満たしていない
       *
       * @note 専用全力技の条件：　　元技、道具、種族　の組み合わせが一致している
       * @note 汎用全力技の条件：　　元技、道具　　　　の組み合わせが一致している( 元技がダメージ技の場合 )
       * @note 全力ブーストの条件：　元技、道具　　　　の組み合わせが一致している( 元技が非ダメージ技の場合 )
       */
      //---------------------------------------------------------------------
      static bool IsZenryokuWazaEnable( const ZenryokuWazaSource& source );

      //---------------------------------------------------------------------
      /**
       * @brief 全力技を使用する条件を満たしているか？
       *
       * @param[in]  pp     ポケモンパラメータ
       * @param[in]  item   アイテム番号
       *
       * @retval true   全力技を使用する条件を満たしている
       * @retval false  全力技を使用する条件を満たしていない
       *
       * @note 専用全力技の条件：　　元技、道具、種族　の組み合わせが一致している
       * @note 汎用全力技の条件：　　元技、道具　　　　の組み合わせが一致している( 元技がダメージ技の場合 )
       * @note 全力ブーストの条件：　元技、道具　　　　の組み合わせが一致している( 元技が非ダメージ技の場合 )
       */
      //---------------------------------------------------------------------
      static bool IsZenryokuWazaEnable( const pml::pokepara::PokemonParam * pp, ItemNo item );

      //---------------------------------------------------------------------
      /**
       * @brief 全力技の種類を取得する
       *
       * @param[in]  source  全力技の元
       *
       * @return 全力技の種類
       * @retval ZENRYOKUWAZA_KIND_NULL  全力技に変換できない場合
       */
      //---------------------------------------------------------------------
      static ZenryokuWazaKind GetZenryokuWazaKind( const ZenryokuWazaSource& source );

      //---------------------------------------------------------------------
      /**
       * @brief 全力技の技No.を取得する
       *
       * @param[in]  source  全力技の元
       * 
       * @return 全力技の技No.
       * @retval WAZANO_NULL  全力技に変換できない場合
       */
      //---------------------------------------------------------------------
      static WazaNo GetZenryokuWazaNo( const ZenryokuWazaSource& source );

    };


  } // namespace waza
} // namespace pml

#endif // PML_ZENRYOKUWAZASYSTEM_H_INCLUDED

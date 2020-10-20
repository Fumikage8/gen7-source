//=============================================================================
/**
 * @file    btl_ZenryokuWaza.h
 * @brief   全力技
 * @author  obata_toshihiro
 * @date    2015.05.28
 */
//=============================================================================
#if !defined( BTL_ZENRYOKUWAZA_H_INCLUDED )
#define BTL_ZENRYOKUWAZA_H_INCLUDED
#pragma once

#include "./btl_common.h"


namespace btl {

  class BTL_POKEPARAM;


  /**
   * @class ZenryokuWaza
   * @brief 全力技に関する関数群
   */
  class ZenryokuWaza
  {
  public:
  

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
     * @brief 全力技の種類を取得する
     *
     * @param[in]  attackPoke  全力技を使用するポケモンのパラメータ
     * @param[in]  wazaNo      元技の技No.
     *
     * @return 全力技の種類
     * @retval ZENRYOKUWAZA_KIND_NULL  全力技に変換できない場合
     */
    //---------------------------------------------------------------------
    static ZenryokuWazaKind GetZenryokuWazaKind( const BTL_POKEPARAM& attackPoke, WazaNo wazaNo );

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

    //---------------------------------------------------------------------
    /**
     * @brief 全力技の技No.を取得する
     *
     * @param[in]  attackPoke  全力技を使用するポケモンのパラメータ
     * @param[in]  wazaNo      元技の技No.
     * 
     * @return 全力技の技No.
     * @retval WAZANO_NULL  全力技に変換できない場合
     */
    //---------------------------------------------------------------------
    static WazaNo GetZenryokuWazaNo( const BTL_POKEPARAM& attackPoke, WazaNo wazaNo );

    //---------------------------------------------------------------------
    /**
     * @brief 全力技の技名を取得する
     *
     * @param[out] strbuf      技名の格納先
     * @param[in]  attackPoke  全力技を使用するポケモンのパラメータ
     * @param[in]  wazaNo      元技の技No.
     */
    //---------------------------------------------------------------------
    static void GetZenryokuWazaName( STRBUF* strbuf, const BTL_POKEPARAM& attackPoke, WazaNo wazaNo );

    //---------------------------------------------------------------------
    /**
     * @brief 全力技を使用するための道具( Zクリスタル )を持っているか？
     * @param poke  チェック対象のポケモン
     * @retval true   Zクリスタルを持っている
     * @retval false  Zクリスタルを持っていない
     */
    //---------------------------------------------------------------------
    static bool HaveZenryokuWazaItem( const BTL_POKEPARAM& poke );
  
  
  };


} // namespace btl

#endif // BTL_ZENRYOKUWAZA_H_INCLUDED
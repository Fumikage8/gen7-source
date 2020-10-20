//=============================================================================
/**
 * @file    btl_ZenryokuWaza.cpp
 * @brief   全力技
 * @author  obata_toshihiro
 * @date    2015.05.28
 */
//=============================================================================
#include <pml/include/waza/pml_ZenryokuWazaSystem.h>
#include <pml/include/item/item.h>

#include "./btl_str.h"
#include "./btl_pokeparam.h"
#include "./btl_ZenryokuWaza.h"


namespace btl {


  namespace {


    /**
     * @brief 全力技の元データをセットする
     *
     * @param[out] source      データの格納先
     * @param[in]  attackPoke  全力技を使用するポケモンのパラメータ
     * @param[in]  wazaNo      元技の技No.
     */
    void SetZenryokuWazaSource( ZenryokuWazaSource* source, const BTL_POKEPARAM& attackPoke, WazaNo wazaNo )
    {
      source->monsNo = static_cast<MonsNo>( attackPoke.GetHenshinMonsNo() ); //「へんしん」後なら、変身中のポケモンの全力技を使用出来る
      source->formNo = attackPoke.GetFormNo();
      source->itemNo = attackPoke.GetItem();
      source->wazaNo = wazaNo;
    }


  } // namespace



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
  ZenryokuWazaKind ZenryokuWaza::GetZenryokuWazaKind( const ZenryokuWazaSource& source )
  {
    return pml::waza::ZenryokuWazaSystem::GetZenryokuWazaKind( source );
  }

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
  ZenryokuWazaKind ZenryokuWaza::GetZenryokuWazaKind( const BTL_POKEPARAM& attackPoke, WazaNo wazaNo )
  {
    ZenryokuWazaSource source;
    SetZenryokuWazaSource( &source, attackPoke, wazaNo );
    return ZenryokuWaza::GetZenryokuWazaKind( source );
  }

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
  WazaNo ZenryokuWaza::GetZenryokuWazaNo( const ZenryokuWazaSource& source )
  {
    return pml::waza::ZenryokuWazaSystem::GetZenryokuWazaNo( source );
  }

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
  WazaNo ZenryokuWaza::GetZenryokuWazaNo( const BTL_POKEPARAM& attackPoke, WazaNo wazaNo )
  {
    ZenryokuWazaSource source;
    SetZenryokuWazaSource( &source, attackPoke, wazaNo );
    return ZenryokuWaza::GetZenryokuWazaNo( source );
  }

  //---------------------------------------------------------------------
  /**
   * @brief 全力技の技名を取得する
   *
   * @param[out] strbuf      技名の格納先
   * @param[in]  attackPoke  全力技を使用するポケモンのパラメータ
   * @param[in]  wazaNo      元技の技No.
   */
  //---------------------------------------------------------------------
  void ZenryokuWaza::GetZenryokuWazaName( STRBUF* strbuf, const BTL_POKEPARAM& attackPoke, WazaNo wazaNo )
  {
    const WazaNo zenryokuWazaNo = ZenryokuWaza::GetZenryokuWazaNo( attackPoke, wazaNo );
    if( zenryokuWazaNo != WAZANO_NULL )
    {
      str::GetZenryokuWazaName( strbuf, zenryokuWazaNo );
      return;
    }

    str::GetZenryokuWazaName( strbuf, wazaNo );
  }

  //---------------------------------------------------------------------
  /**
   * @brief 全力技を使用するための道具( Zクリスタル )を持っているか？
   * @param poke  チェック対象のポケモン
   * @retval true   Zクリスタルを持っている
   * @retval false  Zクリスタルを持っていない
   */
  //---------------------------------------------------------------------
  bool ZenryokuWaza::HaveZenryokuWazaItem( const BTL_POKEPARAM& poke )
  {
    const ItemNo itemNo = poke.GetItem();
    return item::ITEM_CheckPiece( itemNo );
  }

} // namespace btl
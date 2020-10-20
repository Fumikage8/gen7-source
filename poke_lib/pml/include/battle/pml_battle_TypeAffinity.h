//=============================================================================
/**
 * @file   pml_battle_TypeAffinity.h
 * @date   2016/02/02 14:54:04
 * @author obata_toshihiro
 * @brief  タイプ相性
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined PML_BATTLE_TYPEAFFINITY_H_INCLUDED
#define PML_BATTLE_TYPEAFFINITY_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <pml/include/pml_Type.h>

GFL_NAMESPACE_BEGIN( pml )
GFL_NAMESPACE_BEGIN( pokepara )
class PokemonParam;
GFL_NAMESPACE_END( pokepara )
GFL_NAMESPACE_END( pml )


GFL_NAMESPACE_BEGIN( pml )
GFL_NAMESPACE_BEGIN( battle )


/**
 * @class TypeAffinity
 * @brief タイプ相性を定義するクラス
 */
class TypeAffinity
{
  GFL_FORBID_COPY_AND_ASSIGN( TypeAffinity );

public:

  //---------------------------------------------------------------------------
  /**
   * @brief タイプ相性( 0, 1/8, 1/4, 1/2, x1, x2, x4, x8 の14段階 )
   */
  //---------------------------------------------------------------------------
  enum AffinityID
  {
    TYPEAFF_0 = 0,  ///< 無効
    TYPEAFF_1_64,
    TYPEAFF_1_32,
    TYPEAFF_1_16,
    TYPEAFF_1_8,
    TYPEAFF_1_4,
    TYPEAFF_1_2,
    TYPEAFF_1,
    TYPEAFF_2,
    TYPEAFF_4,
    TYPEAFF_8,
    TYPEAFF_16,
    TYPEAFF_32,
    TYPEAFF_64,
    TYPEAFF_MAX,
    TYPEAFF_NULL = TYPEAFF_MAX,
  };

  //---------------------------------------------------------------------------
  /**
   * @brief 簡易タイプ相性( こうかなし・ふつう・ばつぐん・もうひとつの４段階 )
   */
  //---------------------------------------------------------------------------
  enum AboutAffinityID
  {
    TYPEAFF_ABOUT_NONE = 0,
    TYPEAFF_ABOUT_NORMAL,
    TYPEAFF_ABOUT_ADVANTAGE,
    TYPEAFF_ABOUT_DISADVANTAGE,
  };

  //---------------------------------------------------------------------------
  /**
   * @brief タイプ相性を計算する
   * @param wazaType        技のタイプ
   * @param pokeType        技を受けるポケモンのタイプ
   * @param isSakasaBattle  さかさバトルか？
   * @return タイプ相性
   */
  //---------------------------------------------------------------------------
  static AffinityID CalcAffinity( pml::PokeType wazaType, pml::PokeType pokeType, bool isSakasaBattle=false );
  static AboutAffinityID CalcAffinityAbout( pml::PokeType wazaType, pml::PokeType pokeType, bool isSakasaBattle=false );

  //---------------------------------------------------------------------------
  /**
   * @brief タイプ相性を計算する
   * @param wazaType        技のタイプ
   * @param pokeType1       技を受けるポケモンのタイプ１
   * @param pokeType2       技を受けるポケモンのタイプ２
   * @param isSakasaBattle  さかさバトルか？
   * @return タイプ相性
   */
  //---------------------------------------------------------------------------
  static AffinityID CalcAffinity( pml::PokeType wazaType, pml::PokeType pokeType1, pml::PokeType pokeType2, bool isSakasaBattle=false );
  static AboutAffinityID CalcAffinityAbout( pml::PokeType wazaType, pml::PokeType pokeType1, pml::PokeType pokeType2, bool isSakasaBattle=false );

  //---------------------------------------------------------------------------
  /**
   * @brief タイプ相性を計算する
   * @param wazaType        技のタイプ
   * @param pokeParam       技を受けるポケモンのパラメータ
   * @param isSakasaBattle  さかさバトルか？
   * @return タイプ相性
   */
  //---------------------------------------------------------------------------
  static AffinityID CalcAffinity( pml::PokeType wazaType, const pml::pokepara::PokemonParam& pokeParam, bool isSakasaBattle=false );
  static AboutAffinityID CalcAffinityAbout( pml::PokeType wazaType, const pml::pokepara::PokemonParam& pokeParam, bool isSakasaBattle=false );

  //---------------------------------------------------------------------------
  /**
   * @brief タイプ相性を掛け合わせる
   * @param aff1  掛け合わせる相性１
   * @param aff2  掛け合わせる相性２
   * @return 掛け合わせた相性
   */
  //---------------------------------------------------------------------------
  static AffinityID MulAffinity( AffinityID aff1, AffinityID aff2 );

  //---------------------------------------------------------------------------
  /**
   * @brief タイプ相性を簡易タイプ相性に変換する
   * @param aff  タイプ相性
   * @return     簡易タイプ相性
   */
  //---------------------------------------------------------------------------
  static AboutAffinityID ConvAboutAffinity( AffinityID aff );

};



GFL_NAMESPACE_END( battle )
GFL_NAMESPACE_END( pml )

#endif // PML_BATTLE_TYPEAFFINITY_H_INCLUDED
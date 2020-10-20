//=============================================================================
/**
 * @file   pml_battle_TypeAffinity.cpp
 * @date   2016/02/02 14:54:04
 * @author obata_toshihiro
 * @brief  タイプ相性
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include <debug/include/gfl2_Assert.h>
#include <pml/include/battle/pml_battle_TypeAffinity.h>
#include <pml/include/pml_PokePara.h>


GFL_NAMESPACE_BEGIN( pml )
GFL_NAMESPACE_BEGIN( battle )


namespace
{

  /**
   *  LSB取得（1org）
   */
  inline u32 calcLSB( u32 value )
  {
    u32 maxBit = (sizeof(value) * 8);
    for(u32 i=0; i<maxBit; ++i)
    {
      if( value & (1 << i) ){
        return i + 1;
      }
    }
    return 0;
  }

  //--------------------------------------------------------------
  /**
   *  タイプ相性値
   */
  //--------------------------------------------------------------
  typedef enum {
      x0 = 0,
      xH = 2,
      x1 = 4,
      x2 = 8,
  }TypeAffValue;
  
  //--------------------------------------------------------------
  /**
   *  タイプ相性テーブル
   */
  //--------------------------------------------------------------
  static const u8 TypeAffTbl[ POKETYPE_NUMS ][ POKETYPE_NUMS ] = {
  //          ノ,  格,  飛,  毒,  地,  岩,  虫,  幽,  鋼,  炎,  水,  草,  電,  超,  氷,  竜,  悪, 妖
  /* ノ */ {  x1,  x1,  x1,  x1,  x1,  xH,  x1,  x0,  xH,  x1,  x1,  x1,  x1,  x1,  x1,  x1,  x1, x1, },
  /* 格 */ {  x2,  x1,  xH,  xH,  x1,  x2,  xH,  x0,  x2,  x1,  x1,  x1,  x1,  xH,  x2,  x1,  x2, xH, },
  /* 飛 */ {  x1,  x2,  x1,  x1,  x1,  xH,  x2,  x1,  xH,  x1,  x1,  x2,  xH,  x1,  x1,  x1,  x1, x1, },
  /* 毒 */ {  x1,  x1,  x1,  xH,  xH,  xH,  x1,  xH,  x0,  x1,  x1,  x2,  x1,  x1,  x1,  x1,  x1, x2, },
  /* 地 */ {  x1,  x1,  x0,  x2,  x1,  x2,  xH,  x1,  x2,  x2,  x1,  xH,  x2,  x1,  x1,  x1,  x1, x1, },
  /* 岩 */ {  x1,  xH,  x2,  x1,  xH,  x1,  x2,  x1,  xH,  x2,  x1,  x1,  x1,  x1,  x2,  x1,  x1, x1, },
  /* 虫 */ {  x1,  xH,  xH,  xH,  x1,  x1,  x1,  xH,  xH,  xH,  x1,  x2,  x1,  x2,  x1,  x1,  x2, xH, },
  /* 幽 */ {  x0,  x1,  x1,  x1,  x1,  x1,  x1,  x2,  x1,  x1,  x1,  x1,  x1,  x2,  x1,  x1,  xH, x1, },
  /* 鋼 */ {  x1,  x1,  x1,  x1,  x1,  x2,  x1,  x1,  xH,  xH,  xH,  x1,  xH,  x1,  x2,  x1,  x1, x2, },
  /* 炎 */ {  x1,  x1,  x1,  x1,  x1,  xH,  x2,  x1,  x2,  xH,  xH,  x2,  x1,  x1,  x2,  xH,  x1, x1, },
  /* 水 */ {  x1,  x1,  x1,  x1,  x2,  x2,  x1,  x1,  x1,  x2,  xH,  xH,  x1,  x1,  x1,  xH,  x1, x1, },
  /* 草 */ {  x1,  x1,  xH,  xH,  x2,  x2,  xH,  x1,  xH,  xH,  x2,  xH,  x1,  x1,  x1,  xH,  x1, x1, },
  /* 電 */ {  x1,  x1,  x2,  x1,  x0,  x1,  x1,  x1,  x1,  x1,  x2,  xH,  xH,  x1,  x1,  xH,  x1, x1, },
  /* 超 */ {  x1,  x2,  x1,  x2,  x1,  x1,  x1,  x1,  xH,  x1,  x1,  x1,  x1,  xH,  x1,  x1,  x0, x1, },
  /* 氷 */ {  x1,  x1,  x2,  x1,  x2,  x1,  x1,  x1,  xH,  xH,  xH,  x2,  x1,  x1,  xH,  x2,  x1, x1, },
  /* 竜 */ {  x1,  x1,  x1,  x1,  x1,  x1,  x1,  x1,  xH,  x1,  x1,  x1,  x1,  x1,  x1,  x2,  x1, x0, },
  /* 悪 */ {  x1,  xH,  x1,  x1,  x1,  x1,  x1,  x2,  x1,  x1,  x1,  x1,  x1,  x2,  x1,  x1,  xH, xH, },
  /* 妖 */ {  x1,  x2,  x1,  xH,  x1,  x1,  x1,  x1,  xH,  xH,  x1,  x1,  x1,  x1,  x1,  x2,  x2, x1, },
  };

}

//-----------------------------------------------------------------------------
/**
 * @brief タイプ相性を計算する
 * @param wazaType        技のタイプ
 * @param pokeType        技を受けるポケモンのタイプ
 * @param isSakasaBattle  さかさバトルか？
 * @return タイプ相性
 */
//-----------------------------------------------------------------------------
TypeAffinity::AffinityID TypeAffinity::CalcAffinity( pml::PokeType wazaType, pml::PokeType pokeType, bool isSakasaBattle )
{
  if( (wazaType == POKETYPE_NULL) || (pokeType == POKETYPE_NULL) ){
    return TYPEAFF_1;
  }
  else
  {
    u8 affinity = TypeAffTbl[ wazaType ][ pokeType ];

    switch( affinity ){
    case x0:    return (!isSakasaBattle)? TYPEAFF_0   : TYPEAFF_2;
    case xH:    return (!isSakasaBattle)? TYPEAFF_1_2 : TYPEAFF_2;
    case x1:    return (!isSakasaBattle)? TYPEAFF_1   : TYPEAFF_1;
    case x2:    return (!isSakasaBattle)? TYPEAFF_2   : TYPEAFF_1_2;
    }
  }

  GFL_ASSERT(0);
  return TYPEAFF_0;
}

//-----------------------------------------------------------------------------
/**
 * @brief タイプ相性を計算する
 * @param wazaType        技のタイプ
 * @param pokeType1       技を受けるポケモンのタイプ１
 * @param pokeType2       技を受けるポケモンのタイプ２
 * @param isSakasaBattle  さかさバトルか？
 * @return タイプ相性
 */
//-----------------------------------------------------------------------------
TypeAffinity::AffinityID TypeAffinity::CalcAffinity( pml::PokeType wazaType, pml::PokeType pokeType1, pml::PokeType pokeType2, bool isSakasaBattle )
{
  AffinityID aff1 = CalcAffinity( wazaType, pokeType1, isSakasaBattle );
  AffinityID aff2 = CalcAffinity( wazaType, pokeType2, isSakasaBattle );
  return MulAffinity( aff1, aff2 );
}

//-----------------------------------------------------------------------------
/**
 * @brief タイプ相性を計算する
 * @param wazaType        技のタイプ
 * @param pokeType1       技を受けるポケモンのタイプ１
 * @param pokeType2       技を受けるポケモンのタイプ２
 * @param isSakasaBattle  さかさバトルか？
 * @return タイプ相性
 */
//-----------------------------------------------------------------------------
TypeAffinity::AffinityID TypeAffinity::CalcAffinity( pml::PokeType wazaType, const pml::pokepara::PokemonParam& pokeParam, bool isSakasaBattle )
{
  pml::PokeType pokeType1 = pokeParam.GetType1();
  pml::PokeType pokeType2 = pokeParam.GetType2();
  return CalcAffinity( wazaType, pokeType1, pokeType2, isSakasaBattle );
}

//-----------------------------------------------------------------------------
/**
 * @brief タイプ相性を掛け合わせる
 * @param aff1  掛け合わせる相性１
 * @param aff2  掛け合わせる相性２
 * @return 掛け合わせた相性
 */
//-----------------------------------------------------------------------------
TypeAffinity::AffinityID TypeAffinity::MulAffinity( AffinityID aff1, AffinityID aff2 )
{
  enum {
    VALUE_0    =      0,  ///< 無効
    VALUE_1_64 =  1<< 0,
    VALUE_1_32 =  1<< 1,
    VALUE_1_16 =  1<< 2,
    VALUE_1_8  =  1<< 3,
    VALUE_1_4  =  1<< 4,
    VALUE_1_2  =  1<< 5,
    VALUE_1    =  1<< 6,
    VALUE_2    =  1<< 7,
    VALUE_4    =  1<< 8,
    VALUE_8    =  1<< 9,
    VALUE_16   =  1<<10,
    VALUE_32   =  1<<11,
    VALUE_64   =  1<<12,
  };

  static const u32 valueTable[ TYPEAFF_MAX ] = {
    VALUE_0,    VALUE_1_64,   VALUE_1_32,  VALUE_1_16, VALUE_1_8,  VALUE_1_4,  VALUE_1_2,
    VALUE_1,    VALUE_2,      VALUE_4,     VALUE_8,    VALUE_16,   VALUE_32,   VALUE_64,
  };
  u32 mulValue;

  GFL_ASSERT(aff1<TYPEAFF_MAX); // @check
  GFL_ASSERT(aff2<TYPEAFF_MAX); // @check

  mulValue = (valueTable[ aff1 ] * valueTable[ aff2 ]) / VALUE_1;

  // 想定外の事態ではあるが、何度も掛け合わされて
  // 1/64未満 or 64倍超過 になってしまったケースに対応しておく。
  if( mulValue > VALUE_64 ){
    GFL_ASSERT(0);
    mulValue = VALUE_64;
  }
  if( (mulValue == VALUE_0) && (aff1!=TYPEAFF_0) && (aff2!=TYPEAFF_0) ){
    GFL_ASSERT(0);
    mulValue = VALUE_1_64;
  }

  u32 result = calcLSB( mulValue );
  if( result >= TYPEAFF_MAX ){
    GFL_ASSERT(0);
    result = TYPEAFF_64;
  }

  return static_cast<AffinityID>( result );
}

//-----------------------------------------------------------------------------
/**
 * @brief タイプ相性を簡易タイプ相性に変換する
 * @param aff  タイプ相性
 * @return     簡易タイプ相性
 */
//-----------------------------------------------------------------------------
TypeAffinity::AboutAffinityID TypeAffinity::ConvAboutAffinity( AffinityID aff )
{
  if( aff > TYPEAFF_1 )
  {
    return TYPEAFF_ABOUT_ADVANTAGE;
  }
  if( aff == TYPEAFF_1 )
  {
    return TYPEAFF_ABOUT_NORMAL;
  }
  if( aff != TYPEAFF_0 )
  {
    return TYPEAFF_ABOUT_DISADVANTAGE;
  }
  return TYPEAFF_ABOUT_NONE;
}

//-----------------------------------------------------------------------------
/**
 * @brief 簡易タイプ相性を計算する
 * @param wazaType        技のタイプ
 * @param pokeType        技を受けるポケモンのタイプ
 * @param isSakasaBattle  さかさバトルか？
 * @return 簡易タイプ相性
 */
//-----------------------------------------------------------------------------
TypeAffinity::AboutAffinityID TypeAffinity::CalcAffinityAbout( pml::PokeType wazaType, pml::PokeType pokeType, bool isSakasaBattle )
{
  AffinityID affinity = CalcAffinity( wazaType, pokeType, isSakasaBattle );
  return ConvAboutAffinity( affinity );
}

//-----------------------------------------------------------------------------
/**
 * @brief 簡易タイプ相性を計算する
 * @param wazaType        技のタイプ
 * @param pokeType1       技を受けるポケモンのタイプ１
 * @param pokeType2       技を受けるポケモンのタイプ２
 * @param isSakasaBattle  さかさバトルか？
 * @return 簡易タイプ相性
 */
//-----------------------------------------------------------------------------
TypeAffinity::AboutAffinityID TypeAffinity::CalcAffinityAbout( pml::PokeType wazaType, pml::PokeType pokeType1, pml::PokeType pokeType2, bool isSakasaBattle )
{
  AffinityID affinity = CalcAffinity( wazaType, pokeType1, pokeType2, isSakasaBattle );
  return ConvAboutAffinity( affinity );
}

//-----------------------------------------------------------------------------
/**
 * @brief 簡易タイプ相性を計算する
 * @param wazaType        技のタイプ
 * @param pokeType1       技を受けるポケモンのタイプ１
 * @param pokeType2       技を受けるポケモンのタイプ２
 * @param isSakasaBattle  さかさバトルか？
 * @return 簡易タイプ相性
 */
//-----------------------------------------------------------------------------
TypeAffinity::AboutAffinityID TypeAffinity::CalcAffinityAbout( pml::PokeType wazaType, const pml::pokepara::PokemonParam& pokeParam, bool isSakasaBattle )
{
  AffinityID affinity = CalcAffinity( wazaType, pokeParam, isSakasaBattle );
  return ConvAboutAffinity( affinity );
}



GFL_NAMESPACE_END( battle )
GFL_NAMESPACE_END( pml )
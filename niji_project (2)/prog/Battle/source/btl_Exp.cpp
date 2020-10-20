//=============================================================================
/**
 * @file    btl_Exp.cpp
 * @brief   経験値計算
 * @author  obata_toshihiro
 * @date    2015.03.02
 */
//=============================================================================
#include <Battle/include/battle_SetupParam.h>
#include "./btl_calc.h"
#include "./btl_Exp.h"

// ロトムパワー
#include <GameSys/include/GameManager.h>
#include "Field/FieldStatic/include/RotomPower/RotomPowerManager.h"

GFL_NAMESPACE_BEGIN(btl)


//---------------------------------------------------------------------------
/**
  * @brief ポケモン１体分の取得経験値を計算する
  * @param      calcParam  入力パラメータ
  * @param[out] pResult    計算結果の格納先
  */
//---------------------------------------------------------------------------
void Exp::CalcExp( const CalcParam& calcParam, CalcResult* pResult )
{
  pResult->exp = 0;
  pResult->isContainBonus = false;
  pResult->byGakusyusouti = false;

  // 倒したポケモンと対面していなくて
  //「がくしゅうそうち」が OFF なら、経験値は貰えない
  if( !( calcParam.isMatch ) &&
      !( calcParam.isGakusyusoutiOn ) ) {
    return;
  }

  // 基本は倒されたポケモンの経験値とレベル差により算出
  u32 baseExp = CalcBaseExp( calcParam.deadPokeExp, calcParam.deadPokeLevel, calcParam.getPokeLevel );

  if( calcParam.isMatch ) 
  {
    pResult->exp = baseExp; // 倒したポケモンと対面しているメンバーは、まるまる貰える
  }
  else if( calcParam.isGakusyusoutiOn )
  {
    pResult->exp = CalcGakusyusoutiExp( baseExp );
    pResult->byGakusyusouti = true;
  }

  // レベル差が大きいほど、経験値が多く貰えるように補正
  pResult->exp = getexp_calc_adjust_level( pResult->exp, calcParam.getPokeLevel, calcParam.deadPokeLevel );

  // 他人が親ならボーナス
  if( !( calcParam.isMyPoke ) )
  {
    pResult->exp = ApplyBonus_Received( pResult->exp, calcParam.playerLanguageId, calcParam.getPokeLanguageId );
    pResult->isContainBonus = true;
  }

  // しあわせタマゴ持ってたらボーナス
  if( calcParam.haveSiawasetamago ) {
    pResult->exp = ApplyBonus_Siawasetamago( pResult->exp );
    pResult->isContainBonus = true;
  }

  // 進化レベルに達しているのに進化していないならボーナス
  if( calcParam.isEvoCanceledPoke ) {
    pResult->exp = ApplyBonus_EvoCanceled( pResult->exp );
    pResult->isContainBonus = true;
  }

  // なかよし度が50以上ならボーナス
  if( 50 <= calcParam.getPokeFriendship ) {
    pResult->exp = ApplyBonus_Friendship( pResult->exp );
    pResult->isContainBonus = true;
  }

  u32 pow_exp = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetRotomPowerManager()->CalcExp(pResult->exp);
  if (pow_exp > pResult->exp)
  {
    // ロトムパワー補正
    pResult->exp = pow_exp;
    pResult->isContainBonus = true;
  }
}

/** 
 * @brief 基礎経験値を計算する 
 * @param deadPokeExp    倒されたポケモンの経験値 
 * @param deadPokeLevel  倒されたポケモンのレベル 
 * @param pokeLevel      倒したポケモンのレベル 
 */ 
u32 Exp::CalcBaseExp( u32 deadPokeExp, u32 deadPokeLevel, u32 pokeLevel ) 
{ 
  u32 exp = deadPokeExp * deadPokeLevel / 5; 
  if( exp == 0 ){ 
    exp = 1; 
  } 
  return exp; 
}


/**
 * @brief「がくしゅうそうち」によって取得する基礎経験値を求める
 * @param baseExp  対面した場合の基礎経験値
 *
 * @note 対面してなくても “がくしゅうそうち” が有効なら半分もらえる
 */
u32 Exp::CalcGakusyusoutiExp( u32 baseExp )
{
  u32 exp = baseExp / 2;
  if( exp < 1 ){
    exp = 1;
  }
  return exp;
}

/** 
 * 倒されたポケモンと経験値を取得するポケモンのレベル差に応じて経験値を補正する（ＷＢより） 
 * 
 * @param   base_exp      基本経験値（GSまでの仕様による計算結果） 
 * @param   getpoke_lv    経験値を取得するポケモンのレベル 
 * @param   deadpoke_lv   倒されたポケモンのレベル 
 * 
 * @retval  u32   補正後経験値 
 */ 
u32 Exp::getexp_calc_adjust_level( u32 base_exp, u16 getpoke_lv, u16 deadpoke_lv ) 
{ 
  f32 fBaseExp    = static_cast<f32>( base_exp );
  f32 fGetPokeLv  = static_cast<f32>( getpoke_lv );
  f32 fDeadPokeLv = static_cast<f32>( deadpoke_lv );

  f32 fNumer = fDeadPokeLv * 2.0f + 10.0f; 
  f32 fDenom = fDeadPokeLv + fGetPokeLv + 10.0f; 
 
  fNumer = calc_adjust_level_sub( fNumer ); 
  fDenom = calc_adjust_level_sub( fDenom ); 
  f32 fRatio = fNumer / fDenom; 
  f32 fResult = ( fBaseExp * fRatio ) + 1.0f;
  u32 unResult = static_cast<u32>( fResult );

  return unResult; 
} 

/** 
 *  経験値補正計算サブ(2.5乗） 
 */ 
f32 Exp::calc_adjust_level_sub( f32 fValue ) 
{ 
  f32 fSqrt = gfl2::math::FSqrt( fValue );
  f32 fValueSquare = fValue * fValue;
  f32 fResult = fValueSquare * fSqrt;

  return fResult; 
} 

/**
 * @brief 経験値ボーナスを適用する( 貰ったポケモン )
 * @param exp  ボーナス適用前の経験値
 * @param playerLangId  プレイヤの言語コード
 * @param pokeLangId  ポケモンの言語コード
 * @return  ボーナス適用後の経験値
 *
 * @note 外国の親ならx1.7，そうじゃなければx1.5
 */
u32 Exp::ApplyBonus_Received( u32 exp, u32 playerLangId, u32 pokeLangId )
{
  fx32 ratio = ( playerLangId != pokeLangId ) ? FX32_CONST(1.7f) : FX32_CONST(1.5f);
  u32 bonus = calc::MulRatio( exp, ratio );
  return bonus;
}

/**
 * @brief 経験値ボーナスを適用する( しあわせタマゴ )
 * @param exp  ボーナス適用前の経験値
 * @return  ボーナス適用後の経験値
 *
 * @note しあわせタマゴ持ってたらボーナス x1.5
 */
u32 Exp::ApplyBonus_Siawasetamago( u32 exp )
{
  return calc::MulRatio( exp, FX32_CONST(1.5f) );
}

/**
 * @brief 経験値ボーナスを適用する( 進化キャンセル )
 * @param exp  ボーナス適用前の経験値
 * @return  ボーナス適用後の経験値
 *
 * @note 進化レベルに達しているのに進化していない場合 x1.2
 */
u32 Exp::ApplyBonus_EvoCanceled( u32 exp )
{
  return calc::MulRatio( exp, FX32_CONST(1.2f) );
}

/**
 * @brief 経験値ボーナスを適用する( なかよし度 )
 * @param exp  ボーナス適用前の経験値
 * @return  ボーナス適用後の経験値
 *
 * @note なかよし度が50以上で 1.2倍
 */
u32 Exp::ApplyBonus_Friendship( u32 exp )
{
  return calc::MulRatio( exp, FX32_CONST(1.2f) );
}



GFL_NAMESPACE_END(btl)
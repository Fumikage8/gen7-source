//=============================================================================
/**
 * @file    btl_Exp.h
 * @brief   経験値計算
 * @author  obata_toshihiro
 * @date    2015.03.02
 */
//=============================================================================
#if !defined( BTL_EXP_H_INCLUDED )
#define BTL_EXP_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <Battle/include/battle_SetupParam.h>
#include "./btl_fx32.h"

GFL_NAMESPACE_BEGIN(btl)


/**
 * @class Exp
 * @brief ポケモン１体分の経験値計算
 */
class Exp
{
public:


  /**
   * @brief 経験値計算の入力パラメータ
   */
  struct CalcParam
  {
    BtlCompetitor competitor;          // 対戦相手
    u32           playerLanguageId;    // プレイヤの言語ID
    u32           deadPokeExp;         // 倒されたポケモンの経験値
    u32           deadPokeLevel;       // 倒されたポケモンのレベル
    u32           getPokeLevel;        // 経験値を取得するポケモンのレベル
    u32           getPokeFriendship;   // 経験値を取得するポケモンのなかよし度
    u32           getPokeLanguageId;   // 経験値を取得するポケモンの言語ID
    bool          isMatch;             // 経験値を取得するポケモンが、倒されたポケモンと対面したか？
    bool          isGakusyusoutiOn;    // 経験値を取得するポケモンが、「がくしゅうそうち」が ON か？
    bool          haveSiawasetamago;   // 経験値を取得するポケモンが、「しあわせたまご」を持っているか？
    bool          isEvoCanceledPoke;   // 経験値を取得するポケモンが、進化キャンセルされたポケモンか？
    bool          isMyPoke;            // 経験値を取得するポケモンの親が、プレイヤ自身か？
  };


  /**
   * @brief 経験値の計算結果
   */
  struct CalcResult
  {
    u32  exp;            // 経験値
    bool byGakusyusouti; //「がくしゅうそうち」によって貰えたなら true
    bool isContainBonus; // 多めに貰えたなら true
  };


  //---------------------------------------------------------------------------
  /**
   * @brief ポケモン１体分の取得経験値を計算する
   * @param      calcParam  入力パラメータ
   * @param[out] pResult    計算結果の格納先
   */
  //---------------------------------------------------------------------------
  static void CalcExp( const CalcParam& calcParam, CalcResult* pResult );



private:
  static u32 CalcBaseExp( u32 deadPokeExp, u32 deadPokeLevel, u32 pokeLevel );
  static u32 CalcGakusyusoutiExp( u32 baseExp );
  static u32 ApplyBonus_Received( u32 exp, u32 playerLangId, u32 pokeLangId );
  static u32 ApplyBonus_Siawasetamago( u32 exp );
  static u32 ApplyBonus_EvoCanceled( u32 exp );
  static u32 ApplyBonus_Friendship( u32 exp );
  static f32 calc_adjust_level_sub( f32 fValue );

#if PM_DEBUG
public: // テストのために公開
#else
private:
#endif
  static u32 getexp_calc_adjust_level( u32 base_exp, u16 getpoke_lv, u16 deadpoke_lv );


private:
  Exp();
  virtual ~Exp();
};


GFL_NAMESPACE_END(btl)

#endif  // BTL_EXP_H_INCLUDED
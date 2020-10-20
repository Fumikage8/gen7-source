//=============================================================================
/**
 * @file    btl_ExpCalculator.h
 * @brief   経験値計算
 * @author  obata_toshihiro
 * @date    2015.03.06
 */
//=============================================================================
#if !defined( BTL_EXP_CALCULATOR_H_INCLUDED )
#define BTL_EXP_CALCULATOR_H_INCLUDED
#pragma once

#include "./btl_ServerFlowSub.h"


GFL_NAMESPACE_BEGIN(Savedata)
class MyStatus;
GFL_NAMESPACE_END(Savedata)


GFL_NAMESPACE_BEGIN(btl)


/**
 * @class ExpCalculator
 * @brief パーティ全員分の経験値を計算し ServerFlowSub::CALC_EXP_WORK に格納する
 */
class ExpCalculator
{
public:

  //---------------------------------------------------------------------------
  /**
   * @brief パーティ全体の取得経験値を計算する
   * @param[out] result        計算結果の格納先
   * @param      myStatus      参照するプレイヤ情報
   * @param      mainModule    参照するメインモジュール
   * @param      party         経験値を取得するポケパーティ
   * @param      deadPoke      倒されたポケモン
   */
  //---------------------------------------------------------------------------
  static void CalcExp( 
    ServerFlowSub::CALC_EXP_WORK* result,
    const Savedata::MyStatus&     myStatus,
    const MainModule&             mainModule,
    const BTL_PARTY&              party, 
    const BTL_POKEPARAM&          deadPoke );


private:
  static void CalcExp( ServerFlowSub::CALC_EXP_WORK* result, const Savedata::MyStatus& myStatus, const MainModule& mainModule, const BTL_POKEPARAM& bpp, const BTL_POKEPARAM& deadPoke );
  static bool IsEvoCanceledPoke( const BTL_POKEPARAM& bpp );
  static u32 GetPokeGiveExp( const BTL_POKEPARAM& bpp );


private:
  ExpCalculator();
  virtual ~ExpCalculator();
};


GFL_NAMESPACE_END(btl)

#endif  // BTL_EXP_CALCULATOR_H_INCLUDED
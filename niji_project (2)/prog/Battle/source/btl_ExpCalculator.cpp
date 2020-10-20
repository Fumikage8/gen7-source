//=============================================================================
/**
 * @file    btl_ExpCalculator.cpp
 * @brief   経験値計算
 * @author  obata_toshihiro
 * @date    2015.03.06
 */
//=============================================================================
#include <Savedata/include/MyStatus.h>
#include "./btl_ExpCalculator.h"
#include "./btl_Exp.h"


GFL_NAMESPACE_BEGIN(btl)


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
void ExpCalculator::CalcExp( 
  ServerFlowSub::CALC_EXP_WORK* result,
  const Savedata::MyStatus&     myStatus,
  const MainModule&             mainModule,
  const BTL_PARTY&              party, 
  const BTL_POKEPARAM&          deadPoke )
{
  // 結果ワーククリア
  for( u32 i=0; i<BTL_PARTY_MEMBER_MAX; ++i ) {
    result[i].Clear();
  }

  const u16 memberCount = party.GetMemberCount();
  for( u32 i=0; i<memberCount; ++i )
  {
    const BTL_POKEPARAM* bpp = party.GetMemberDataConst( i );
    CalcExp( &( result[i] ), myStatus, mainModule, *bpp, deadPoke );
  }
}

/**
 * @brief 指定したポケモンが取得する経験値を求める
 * @param[out] result        計算結果の格納先
 * @param      myStatus      参照するプレイヤ情報
 * @param      mainModule    参照するメインモジュール
 * @param      bpp           経験値を取得するポケモン
 * @param      deadPoke      倒されたポケモン
 */
void ExpCalculator::CalcExp( 
    ServerFlowSub::CALC_EXP_WORK* result,
    const Savedata::MyStatus&     myStatus,
    const MainModule&             mainModule,
    const BTL_POKEPARAM&          bpp,
    const BTL_POKEPARAM&          deadPoke )
{
  if( bpp.IsDead() ) {
    return;
  }

  const pml::pokepara::PokemonParam* pp = bpp.GetSrcData();

  Exp::CalcParam param;
  param.competitor        = mainModule.GetCompetitor();
  param.playerLanguageId  = myStatus.GetPokeLanguageId();
  param.deadPokeExp       = GetPokeGiveExp( deadPoke );
  param.deadPokeLevel     = deadPoke.GetValue( BTL_POKEPARAM::BPP_LEVEL );
  param.getPokeLevel      = bpp.GetValue( BTL_POKEPARAM::BPP_LEVEL );
  param.getPokeFriendship = mainModule.GetPokeFriendship( &bpp );
  param.getPokeLanguageId = pp->GetLangId();
  param.isMatch           = deadPoke.CONFRONT_REC_IsMatch( bpp.GetID() );
  param.isGakusyusoutiOn  = mainModule.HasPlayerGakusyuSouti();
  param.haveSiawasetamago = ( bpp.GetItem() == ITEM_SIAWASETAMAGO );
  param.isEvoCanceledPoke = IsEvoCanceledPoke( bpp );
  param.isMyPoke          = myStatus.IsMyPokemon( *pp );

  Exp::CalcResult calcResult;
  Exp::CalcExp( param, &calcResult );
  result->exp = calcResult.exp;
  result->bGakusyuSouti = calcResult.byGakusyusouti;
  result->fBonus = calcResult.isContainBonus;
}

/**
 * @brief 倒されたポケモンから取得する経験値を取得する
 * @param bpp  倒されたポケモン
 */
u32 ExpCalculator::GetPokeGiveExp( const BTL_POKEPARAM& bpp )
{
  u32 monsno = bpp.GetMonsNo();
  u16 formno = bpp.GetFormNo();
  return btl::calc::PERSONAL_GetParam( monsno, formno, pml::personal::PARAM_ID_give_exp );
}

/**
 * @brief レベル進化をキャンセルしているポケモンかどうか判定
 * @param bpp  チェック対象のポケモン
 */
bool ExpCalculator::IsEvoCanceledPoke( const BTL_POKEPARAM& bpp )
{
  u16 mons_no = bpp.GetMonsNo();
  FormNo formno = bpp.GetFormNo();
  u8 level = bpp.GetValue( BTL_POKEPARAM::BPP_LEVEL );
  return calc::PERSONAL_IsEvoCancelPokemon( mons_no, formno, level );
}


GFL_NAMESPACE_END(btl)
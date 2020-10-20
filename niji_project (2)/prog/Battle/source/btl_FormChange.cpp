//=============================================================================
/**
 * @file   btl_FormChange.cpp
 * @date   2015/11/05 15:47:19
 * @author obata_toshihiro
 * @brief  簡単な説明
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include <pml/include/pml_Personal.h>
#include <pml/include/pml_PokePara.h>
#include "./btl_pokeparam.h"
#include "./btl_FormChange.h"
#include "./btl_calc.h"


GFL_NAMESPACE_BEGIN( btl )


static const u32 YOWASI_FORMCHANGE_LEVEL_MIN      = 20;  // ヨワシが「むれたすがた」になることが出来る最小レベル
static const u32 YOWASI_FORMCHANGE_HP_DENOMINATOR =  4;  // ヨワシが「むれたすがた」になるために必要なＨＰの分母( 残りＨＰが、最大ＨＰ / x より大きければ「むれたすがた」になれる )


//-----------------------------------------------------------------------------
/**
 * @brief ヨワシの「むれたすがた」フォルムになる条件を満たしているか？
 * @param yowasi  ヨワシのパラメータ
 * @retval true   条件を満たしている
 * @retval false  条件を満たしていない
 */
//-----------------------------------------------------------------------------
bool FormChange::YOWASI_IsGyogunFormEnable( const pml::pokepara::PokemonParam& yowasi )
{
  u32 level = yowasi.GetLevel();
  u32 maxHp = yowasi.GetMaxHp();
  u32 hp    = yowasi.GetHp();
  return FormChange::YOWASI_IsGyogunFormEnable( level, maxHp, hp );
}

//-----------------------------------------------------------------------------
/**
 * @brief ヨワシの「むれたすがた」フォルムになる条件を満たしているか？
 * @param yowasi  ヨワシのパラメータ
 * @retval true   条件を満たしている
 * @retval false  条件を満たしていない
 */
//-----------------------------------------------------------------------------
bool FormChange::YOWASI_IsGyogunFormEnable( const BTL_POKEPARAM& yowasi )
{
  u32 level = yowasi.GetValue( BTL_POKEPARAM::BPP_LEVEL );
  u32 maxHp = yowasi.GetValue( BTL_POKEPARAM::BPP_MAX_HP );
  u32 hp    = yowasi.GetValue( BTL_POKEPARAM::BPP_HP );
  return FormChange::YOWASI_IsGyogunFormEnable( level, maxHp, hp );
}

/**
 * @brief ヨワシのフォルムチェンジ条件を満たしているか？
 * @param level   レベル
 * @param maxHp   最大ＨＰ
 * @param hp      残りＨＰ
 * @retval true   条件を満たしている
 * @retval false  条件を満たしていない
 */
bool FormChange::YOWASI_IsGyogunFormEnable( u32 level, u32 maxHp, u32 hp )
{
  // 一定のレベルに達していないとダメ
  if( level < YOWASI_FORMCHANGE_LEVEL_MIN )
  {
    return false;
  }

  // HPが一定割合より多く残っていないとダメ
  u32 hp_threshold = maxHp / YOWASI_FORMCHANGE_HP_DENOMINATOR;
  return ( hp_threshold < hp );
}

//-------------------------------------------------------------------------
/**
 * @brief メテノのパラメータに応じたフォルムを取得する
 * @param meteno  メテノのパラメータ
 */
//-------------------------------------------------------------------------
FormNo FormChange::METENO_CalcFormNo( const pml::pokepara::PokemonParam& meteno )
{
  const u32    maxHp  = meteno.GetMaxHp();
  const u32    hp     = meteno.GetHp();
  const FormNo formno = meteno.GetFormNo();
  return FormChange::METENO_CalcFormNo( maxHp, hp, formno );
}

//-------------------------------------------------------------------------
/**
 * @brief メテノのパラメータに応じたフォルムを取得する
 * @param meteno  メテノのパラメータ
 */
//-------------------------------------------------------------------------
FormNo FormChange::METENO_CalcFormNo( const BTL_POKEPARAM& meteno )
{
  const u32    maxHp  = meteno.GetValue( BTL_POKEPARAM::BPP_MAX_HP );
  const u32    hp     = meteno.GetValue( BTL_POKEPARAM::BPP_HP );
  const FormNo formno = meteno.GetFormNo();
  return FormChange::METENO_CalcFormNo( maxHp, hp, formno );
}

/**
 * @brief メテノのパラメータに応じたフォルムを取得する
 * @param maxHp        最大HP
 * @param currentHp    残りHP
 * @param currentForm  現在のフォルム
 */
FormNo FormChange::METENO_CalcFormNo( u32 maxHp, u32 currentHp, FormNo currentForm )
{
  const u32    hp_border  = maxHp / 2;
  const FormNo form_shell = pml::personal::METENO_GetShellFormNo( currentForm );
  const FormNo form_core  = pml::personal::METENO_GetCoreFormNo( currentForm );
  const FormNo form_next  = ( currentHp <= hp_border ) ? ( form_core ) : ( form_shell );
  return form_next;
}




GFL_NAMESPACE_END( btl )
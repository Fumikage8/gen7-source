//=============================================================================
/**
 * @file   battle_SetupParam_Intrude.cpp
 * @date   2015/10/16 17:27:46
 * @author obata_toshihiro
 * @brief  バトルセットアップパラメータ( 乱入 )
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include <Battle/include/battle_SetupParam_Intrude.h>
#include <Field/FieldStatic/include/Encount/FieldEncountPokeSet.h>


//-----------------------------------------------------------------------------
/**
 * @brief 乱入の設定をクリアする( 無効な値にする )
 * @param[out] param  クリア対象のインスタンス
 */
//-----------------------------------------------------------------------------
void INTRUDE_BATTLE_PARAM_Clear( IntrudeBattleParam* param )
{
  param->intrudeEnableFlag = false;
  param->intrudePokeParamGenerator = NULL;

#if PM_DEBUG
  param->intrudePokeParam = NULL;
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief 乱入が有効な設定か？
 * @param param  チェック対象の設定
 * @retval true   有効
 * @retval false  有効でない
 */
//-----------------------------------------------------------------------------
bool INTRUDE_BATTLE_PARAM_IsEnable( const IntrudeBattleParam& param )
{
  if( !param.intrudeEnableFlag )
  {
    return false;
  }

#if PM_DEBUG
  if( ( param.intrudePokeParam != NULL ) &&
      ( param.intrudePokeParam->GetMonsNo() != MONSNO_NULL ) )
  {
    return true;
  }
#endif

  if( ( param.intrudePokeParamGenerator != NULL ) &&
      ( param.intrudePokeParamGenerator->IsEncountDataExist() ) )
  {
    return true;
  }

  return false;
}
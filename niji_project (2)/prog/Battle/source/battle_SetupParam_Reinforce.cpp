//=============================================================================
/**
 * @file   battle_SetupParam_Reinforce.cpp
 * @date   2015/12/07
 * @author obata_toshihiro
 * @brief  バトルセットアップパラメータ( 援軍 )
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include <Battle/include/battle_SetupParam_Reinforce.h>



//-----------------------------------------------------------------------------
/**
 * @brief 援軍の設定バッファを確保する
 * @param[out] param  クリア対象のインスタンス
 * @param      heap   バッファの確保に使用するヒープ
 */
//-----------------------------------------------------------------------------
void REINFORCE_BATTLE_PARAM_Create( ReinforceBattleParam* param, gfl2::heap::HeapBase* heap )
{
  for( u32 i=0; i<MAX_REINFORCE_POKE_NUM; ++i )
  {
    param->reinforcePoke[i].pokeParam = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief 援軍の設定を破棄する( 破棄可能な状態にする )
 * @param[out] param  破棄対象のインスタンス
 */
//-----------------------------------------------------------------------------
void REINFORCE_BATTLE_PARAM_Delete( ReinforceBattleParam* param )
{
  for( u32 i=0; i<MAX_REINFORCE_POKE_NUM; ++i )
  {
    GFL_SAFE_DELETE( param->reinforcePoke[i].pokeParam  );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief 援軍の設定をクリアする( 無効な値にする )
 * @param[out] param  クリア対象のインスタンス
 */
//-----------------------------------------------------------------------------
void REINFORCE_BATTLE_PARAM_Clear( ReinforceBattleParam* param )
{
  param->reinforceEnableFlag = false;

  for( u32 i=0; i<MAX_REINFORCE_POKE_NUM; ++i )
  {
    param->reinforcePoke[i].condition = REINFORCE_CONDITION_NULL;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief 援軍が有効な設定か？
 * @param param  チェック対象の設定
 * @retval true   有効
 * @retval false  有効でない
 */
//-----------------------------------------------------------------------------
bool REINFORCE_BATTLE_PARAM_IsEnable( const ReinforceBattleParam& param )
{
  if( !param.reinforceEnableFlag )
  {
    return false;
  }

  for( u32 i=0; i<MAX_REINFORCE_POKE_NUM; ++i )
  {
    if( ( param.reinforcePoke[i].condition != REINFORCE_CONDITION_NULL ) &&
        ( param.reinforcePoke[i].pokeParam != NULL ) &&
        ( param.reinforcePoke[i].pokeParam->GetMonsNo() != MONSNO_NULL ) )
    {
      return true;
    }
  }

  return false;
}
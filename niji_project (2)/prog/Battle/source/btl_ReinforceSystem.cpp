//=============================================================================
/**
 * @file   btl_ReinforceSystem.h
 * @date   2015/12/07
 * @author obata_toshihiro
 * @brief  援軍
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include <math/include/gfl2_math.h>

#include <niji_conv_header/poke_lib/tokusei_def.h>
#include <AppLib/include/Tool/app_tool_Gauge.h>

#include "./btl_ReinforceSystem.h"
#include "./btl_BattleRule.h"
#include "./btl_PokemonPosition.h"
#include "./btl_calc.h"
#include "./btl_mainmodule.h"
#include "./btl_pokecon.h"
#include "./btl_pokeparam.h"


GFL_NAMESPACE_BEGIN( btl )


//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param mainModule 関連づけるMainModule
 * @param pokecon    関連付けるPOKECON
 */
 //-----------------------------------------------------------------------------
ReinforceSystem::ReinforceSystem( const MainModule* mainModule, const POKECON* pokecon ) : 
  m_mainModule( mainModule ),
  m_pokecon( pokecon )
{
  for( u32 i=0; i<MAX_REINFORCE_POKE_NUM; ++i )
  {
    m_intrudeCount[i] = 0;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
ReinforceSystem::~ReinforceSystem()
{
}

//-----------------------------------------------------------------------------
/**
 * @brief 助けを呼ぶ
 *
 * @param      callClientId         助けを呼ぶクライアントのID
 * @param[out] out_isCalled         助けを呼んだか？
 * @param[out] out_isAppeared       助けが来たか？
 * @param[out] out_callPokeId       助けを呼ぶポケモンのID
 * @param[out] out_callPokePos      助けを呼ぶポケモンの位置
 * @param[out] out_appearPokePos    乱入ポケモンの立ち位置
 * @param[out] out_appearPokeParam  乱入ポケモンのパラメータ
 */
//-----------------------------------------------------------------------------
void ReinforceSystem::CallHelp( 
  BTL_CLIENT_ID                callClientId,
  bool*                        out_isCalled, 
  bool*                        out_isAppeared,
  u8*                          out_callPokeId, 
  BtlPokePos*                  out_callPokePos,
  BtlPokePos*                  out_appearPokePos,
  pml::pokepara::PokemonParam* out_appearPokeParam )
{
  *out_isCalled      = false;
  *out_isAppeared    = false;
  *out_callPokePos   = BTL_POS_NULL;
  *out_callPokeId    = BTL_POKEID_NULL;
  *out_appearPokePos = BTL_POS_NULL;

  // 助けを呼ぶポケモンを決定
  const BtlPokePos     callPokePos = this->DecideCallPokePos( callClientId );
  const BTL_POKEPARAM* callPoke    = this->GetCallPokeParam( callPokePos );
  if( ( callPokePos == BTL_POS_NULL ) ||
      ( callPoke == NULL ) || 
      ( callPoke->PERMFLAG_Get( BTL_POKEPARAM::PERMFLAG_INTRUDE_IN ) ) )    // 乱入してきたポケモンは援軍を呼ばない
  {
    return;
  }

  // 助けを呼ぶか？
  if( !( this->CheckCall( *callPoke ) ) )
  {
    return;
  }

  // 助けに来るポケモンの立ち位置を決定
  const BtlPokePos appearPokePos = this->DecideAppearPokePos( callClientId );
  if( appearPokePos == BTL_POS_NULL )
  {
    return;
  }

  // 助けに来るポケモンのパラメータを決定
  if( !this->SetupAppearPokeParam( out_appearPokeParam, appearPokePos, *callPoke ) )
  {
    return;
  }

  //「助けが来る」確定
  *out_isCalled      = true;
  *out_callPokePos   = callPokePos;
  *out_callPokeId    = callPoke->GetID();
  *out_isAppeared    = true;
  *out_appearPokePos = appearPokePos;
}

/**
 * @brief 助けを呼ぶポケモンの立ち位置を取得する
 * @param callClientId  助けを呼ぶクライアントのID
 * @return 助けを呼ぶポケモンの立ち位置
 * @retval BTL_POS_NULL  助けを呼ばない場合
 */
BtlPokePos ReinforceSystem::DecideCallPokePos( BTL_CLIENT_ID callClientId ) const
{
  BtlPokePos callPokePos = BTL_POS_NULL;

  const BtlSide callSide       = m_mainModule->GetClientSide( callClientId );
  const u32     callSidePosNum = m_mainModule->GetFrontPosNum( callClientId );

  for( u32 posIndex=0; posIndex<callSidePosNum; ++posIndex )
  {
    const BtlPokePos     pos  = m_mainModule->GetSidePos( callSide, posIndex );
    const BTL_POKEPARAM* poke = m_pokecon->GetFrontPokeDataConst( pos );

    if( ( poke == NULL   ) ||
        ( poke->IsDead() ) )
    {
      continue;
    }

    // 助けを呼び得るポケモンが複数いる場合は、助けを呼ばない
    if( callPokePos != BTL_POS_NULL )
    {
      return BTL_POS_NULL;
    }

    callPokePos = pos;
  }

  return callPokePos;
}

/**
 * @brief 助けを呼ぶポケモンのパラメータを取得する
 * @param callPokePos  助けを呼ぶポケモンの立ち位置
 */
const BTL_POKEPARAM* ReinforceSystem::GetCallPokeParam( BtlPokePos callPokePos ) const
{
  if( callPokePos == BTL_POS_NULL )
  {
    return NULL;
  }

  return m_pokecon->GetFrontPokeDataConst( callPokePos );
}

/**
 * @brief 助けを呼ぶか？
 * @param callPoke   助けを呼ぶポケモン
 * @retval true   助けを呼ぶ
 * @retval false  助けを呼ばない
 */
bool ReinforceSystem::CheckCall( const BTL_POKEPARAM& callPoke ) const
{
  // そらをとぶ等で場にいないなら、呼ばない
  if( callPoke.IsWazaHide() )
  {
    return false;
  }

  // フリーフォール中なら、呼ばない
  if( callPoke.IsUsingFreeFall() ||
      callPoke.CheckSick( pml::wazadata::WAZASICK_FREEFALL ) )
  {
    return false;
  }

  return true;
}

/**
 * @brief 助けに来るポケモンの立ち位置を取得する
 * @param callClientId  助けを呼ぶクライアントのID
 * @return 助けに来るポケモンの立ち位置
 * @retval BTL_POS_NULL  助けが来ない場合
 */
BtlPokePos ReinforceSystem::DecideAppearPokePos( BTL_CLIENT_ID callClientId ) const
{
  const BtlSide    callSide       = PokemonPosition::GetClientSide( BTL_RULE_INTRUDE, callClientId );
  const u32        callSidePosNum = PokemonPosition::GetFrontPosNum( BTL_RULE_INTRUDE, callSide );
  const BTL_PARTY* callSideParty  = m_pokecon->GetPartyDataConst( callClientId );

  for( u32 posIndex=0; posIndex<callSidePosNum; ++posIndex )
  {
    const BTL_POKEPARAM* poke = callSideParty->GetMemberDataConst( posIndex );

    if( ( poke == NULL   ) ||
        ( poke->IsDead() ) )
    {
      return PokemonPosition::GetSidePos( BTL_RULE_INTRUDE, callSide, posIndex );
    }
  }

  return BTL_POS_NULL;
}

/**
 * @brief 乱入するポケモンのパラメータをセットアップする
 * @param[out] appearPoke  セットアップ対象のインスタンス
 * @param      appearPos   乱入ポケモンの位置
 * @param      callPoke    助けを呼ぶポケモン
 * @retval true   パラメータのセットアップに成功
 * @retval false  パラメータのセットアップに失敗
 */
bool ReinforceSystem::SetupAppearPokeParam( pml::pokepara::PokemonParam* appearPoke, BtlPokePos appearPos, const BTL_POKEPARAM& callPoke )
{
  const ReinforceBattleParam& reinforceParam = m_mainModule->GetReinforceBattleParam();
  const s32 reinforcePokeIndex = this->DecideReinforcePokeIndex( callPoke, reinforceParam );
  if( reinforcePokeIndex < 0 )
  {
    return false;
  }

  const pml::pokepara::PokemonParam* reinforcePokeParam = reinforceParam.reinforcePoke[ reinforcePokeIndex ].pokeParam;
  if( reinforcePokeParam == NULL )
  {
    GFL_ASSERT(0);
    return false;
  }

  appearPoke->CopyFrom( *reinforcePokeParam );
  m_intrudeCount[ reinforcePokeIndex ]++;
  return true;
}

/**
 * @brief 場に現れる増援ポケモンのインデックスを決定する
 * @param callPoke        助けを呼ぶポケモン
 * @param reinforceParam  援軍パラメータ
 * @retval -1  条件を満たしたポケモンが存在しない場合
 * @retval [ 0, MAX_REINFORCE_POKE_NUM - 1 ]  条件を満たしたポケモンが存在する場合
 */
s32 ReinforceSystem::DecideReinforcePokeIndex( const BTL_POKEPARAM& callPoke, const ReinforceBattleParam& reinforceParam ) const
{
  for( u32 i=0; i<MAX_REINFORCE_POKE_NUM; ++i )
  {
    const ReinforcePokeParam& reinforcePokeParam = reinforceParam.reinforcePoke[i];

    // すでに最大回数出現したか？
    if( ( 0 < reinforcePokeParam.maxAppearCount ) &&
        ( static_cast<u32>( reinforcePokeParam.maxAppearCount ) <= m_intrudeCount[i] ) )
    {
      continue;
    }

    // 出現する条件を満たしているか？
    if( this->IsReinforceConditionSatisfied( callPoke, reinforcePokeParam ) )
    {
      return i;
    }
  }

  return -1;
}

/**
 * @brief 増援ポケモンについて、場に現れる条件を満たしているか？
 * @param call                助けを呼ぶポケモン
 * @param reinforcePokeParam  増援に来るポケモンのパラメータ
 * @retval true   条件を満たしている
 * @retval false  条件を満たしていない
 */
bool ReinforceSystem::IsReinforceConditionSatisfied( const BTL_POKEPARAM& callPoke, const ReinforcePokeParam& reinforcePokeParam ) const
{
  if( reinforcePokeParam.condition == REINFORCE_CONDITION_NULL )
  {
    return false;
  }

  if( reinforcePokeParam.condition == REINFORCE_CONDITION_MUST )
  {
    return true;
  }

  if( reinforcePokeParam.condition == REINFORCE_CONDITION_PINCH )
  {
    u32 hp        = callPoke.GetValue( BTL_POKEPARAM::BPP_HP );
    u32 hp_border = callPoke.GetValue( BTL_POKEPARAM::BPP_MAX_HP ) * 2 / 3;
    return ( hp < hp_border );
  }

  return false;
}




GFL_NAMESPACE_END( btl )
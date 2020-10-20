//=============================================================================
/**
 * @file   btl_IntrudeSystem.cpp
 * @date   2015/10/09 12:18:43
 * @author obata_toshihiro
 * @brief  乱入
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include <math/include/gfl2_math.h>

#include <niji_conv_header/poke_lib/tokusei_def.h>
#include <AppLib/include/Tool/app_tool_Gauge.h>
#include <Field/FieldStatic/include/Encount/FieldEncountPokeSet.h>

#include "./btl_IntrudeSystem.h"
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
 * @param mainModule                関連づけるMainModule
 * @param pokecon                   関連付けるPOKECON
 * @param appearPokeParamGenerator  乱入ポケモンのパラメータ抽選を行うオブジェクト
 */
 //-----------------------------------------------------------------------------
IntrudeSystem::IntrudeSystem( const MainModule* mainModule, const POKECON* pokecon, Field::Encount::PokeSet* appearPokeParamGenerator ) :
  m_mainModule( mainModule ),
  m_pokecon( pokecon ),
  m_appearPokeParamGenerator( appearPokeParamGenerator ),
  m_randSys(),
  m_isBibiridamaUsed( false ),
  m_intrudeCount( 0 ),
  m_prevCallPokeId( BTL_POKEID_NULL ),
  m_prevAppeared( false )
{
  m_randSys.Initialize();
  this->ClearBatugunDamageRecord();
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
IntrudeSystem::~IntrudeSystem()
{
}

//-----------------------------------------------------------------------------
/**
 * @brief 道具「びびりだま」を使用済みか？
 * @retval true   使用済み
 * @retval false  未使用
 */
//-----------------------------------------------------------------------------
bool IntrudeSystem::IsBibiridamaUsed( void ) const
{
  return m_isBibiridamaUsed;
}

//-----------------------------------------------------------------------------
/**
 * @brief 道具「びびりだま」を使用したことを記録する
 */
//-----------------------------------------------------------------------------
void IntrudeSystem::SetBibiridamaUsed( void )
{
  m_isBibiridamaUsed = true;
}

//-----------------------------------------------------------------------------
/**
 * @brief 乱入回数をインクリメントする
 */
//-----------------------------------------------------------------------------
void IntrudeSystem::IncIntruceCount( void )
{
  if(m_intrudeCount < 0xff)
  {
    ++m_intrudeCount;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief 前ターンにおける、助けを呼んだ結果をセットする
 * @param callPokeId  前ターンにおいて、助けを呼んだポケモンのID( 助けを呼んでいないなら BTL_POKEID_NULL )
 * @param isAppeared  前ターンにおいて、助けが来たか？
 */
//-----------------------------------------------------------------------------
void IntrudeSystem::SetPrevCallResult( u8 callPokeId, bool prevAppeared )
{
  m_prevCallPokeId = callPokeId;
  m_prevAppeared = prevAppeared;
}

//-----------------------------------------------------------------------------
/**
 * @brief 効果抜群のダメージを受けたことを記録する
 * @param pokeId  ダメージを受けたポケモンのID
 */
//-----------------------------------------------------------------------------
void IntrudeSystem::RecordBatugunDamage( u8 pokeId )
{
  if( GFL_NELEMS(m_isBatugunDamaged) <= pokeId )
  {
    GFL_ASSERT(0);
    return;
  }

  m_isBatugunDamaged[ pokeId ] = true;
}

//-----------------------------------------------------------------------------
/**
 * @brief 効果抜群のダメージ履歴をクリアする
 */
//-----------------------------------------------------------------------------
void IntrudeSystem::ClearBatugunDamageRecord( void )
{
  for( u32 i=0; i<BTL_POKEID_MAX; ++i )
  {
    m_isBatugunDamaged[i] = false;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief 助けを呼ぶ
 *
 * @param      weather              天候
 * @param      callClientId         助けを呼ぶクライアントのID
 * @param[out] out_isCalled         助けを呼んだか？
 * @param[out] out_isAppeared       助けが来たか？
 * @param[out] out_callPokeId       助けを呼ぶポケモンのID
 * @param[out] out_callPokePos      助けを呼ぶポケモンの位置
 * @param[out] out_appearPokePos    乱入ポケモンの立ち位置
 * @param[out] out_appearPokeParam  乱入ポケモンのパラメータ
 */
//-----------------------------------------------------------------------------
void IntrudeSystem::CallHelp( 
  BtlWeather                   weather,
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
      ( callPoke == NULL ) )
  {
    return;
  }

  // 助けを呼ぶか？
  if( !( this->CheckCall( *callPoke ) ) )
  {
    return;
  }

  //「助けを呼ぶ」確定
  *out_isCalled    = true;
  *out_callPokePos = callPokePos;
  *out_callPokeId  = callPoke->GetID();

  // 助けが来るか？
  if( !( this->CheckAppear( *callPoke, callPokePos ) ) )
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
  if( !this->SetupAppearPokeParam( out_appearPokeParam, appearPokePos, weather ) )
  {
    return;
  }

  //「助けが来る」確定
  *out_isAppeared    = true;
  *out_appearPokePos = appearPokePos;
}

/**
 * @brief 助けを呼ぶポケモンの立ち位置を取得する
 * @param callClientId  助けを呼ぶクライアントのID
 * @return 助けを呼ぶポケモンの立ち位置
 * @retval BTL_POS_NULL  助けを呼ばない場合
 */
BtlPokePos IntrudeSystem::DecideCallPokePos( BTL_CLIENT_ID callClientId ) const
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
const BTL_POKEPARAM* IntrudeSystem::GetCallPokeParam( BtlPokePos callPokePos ) const
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
bool IntrudeSystem::CheckCall( const BTL_POKEPARAM& callPoke )
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

  // すでに１回乱入が行われている、かつ道具「びびりだま」が使われていないなら、呼ばない
  if( m_intrudeCount > 0 && !m_isBibiridamaUsed )
  {
    return false;
  }

  const u32 callPercentage = this->CalcCallPercentage( callPoke, m_isBibiridamaUsed );
  const u32 randValue      = m_randSys.Next(100);
  return ( randValue < callPercentage );
}

/**
 * @brief 助けを呼ぶ確率[%]を算出する
 * @param callPoke          助けを呼ぶポケモン
 * @param isBibiridamaUsed  道具「びびりだま」を使用したか？
 * @retval [0, 100]
 */
u32 IntrudeSystem::CalcCallPercentage( const BTL_POKEPARAM& callPoke, bool isBibiridamaUsed ) const
{
#if PM_DEBUG
  // デバッグ機能：必ず助けを呼ぶ
  if( m_mainModule->GetDebugFlag( BTL_DEBUGFLAG_INTRUDE_FORCE_CALL ) )
  {
    return 100;
  }
#endif

  // 状態異常のポケモンは助けを呼ばない
  if( callPoke.GetPokeSick() != pml::pokepara::SICK_NULL )
  {
    return 0;
  }

  u32 callValue  = callPoke.GetValue( BTL_POKEPARAM::BPP_CALL_VALUE );
  fx32 rate      = this->CalcCallPercentageRate( callPoke, isBibiridamaUsed );
  u32 percentage = calc::MulRatio( callValue, rate );
  percentage = gfl2::math::Min( percentage, static_cast<u32>(100) ); // 値域を[0, 100]にする
  return percentage;
}

/**
 * @brief 助けを呼ぶ確率の倍率を算出する
 * @param callPoke          助けを呼ぶポケモン
 * @param isBibiridamaUsed  道具「びびりだま」を使用したか？
 */
fx32 IntrudeSystem::CalcCallPercentageRate( const BTL_POKEPARAM& callPoke, bool isBibiridamaUsed ) const
{
  fx32 rate = FX32_CONST(1.0f);

  // 残りHPに応じた確率を計算
  const u32 hp_now    = callPoke.GetValue( BTL_POKEPARAM::BPP_HP );
  const u32 hp_max    = callPoke.GetValue( BTL_POKEPARAM::BPP_MAX_HP );
  const app::tool::Gauge::GaugeColor gaugeColor = app::tool::Gauge::GetDottoColor( hp_now, hp_max );

  switch( gaugeColor ) {
  case app::tool::Gauge::GAUGE_COLOR_GREEN:   rate = FX32_CONST(1.0f);                  break;
  case app::tool::Gauge::GAUGE_COLOR_YELLOW:  rate = FX_Mul( rate, FX32_CONST(3.0f) );  break;
  case app::tool::Gauge::GAUGE_COLOR_RED:     rate = FX_Mul( rate, FX32_CONST(5.0f) );  break;
  }

  // 道具「びびりだま」を使用しているなら、確率２倍
  if( isBibiridamaUsed )
  {
    rate = FX_Mul( rate, FX32_CONST(2.0f) );
  }

  return rate;
}

/**
 * @brief 助けが来るか？
 * @param callPoke     助けを呼ぶポケモン
 * @param callPokePos  助けを呼ぶポケモンの立ち位置
 * @retval true   助けが来る
 * @retval false  助けが来ない
 */
bool IntrudeSystem::CheckAppear( const BTL_POKEPARAM& callPoke, BtlPokePos callPokePos )
{
  const u8   callPokeId       = callPoke.GetID();
  const bool isBatugunDamage  = m_isBatugunDamaged[ callPokeId ];
  const bool prevNotAppeared  = ( m_prevCallPokeId != BTL_POKEID_NULL ) && ( !m_prevAppeared );  // 前ターンに呼んだけど来なかったなら true
  const u32  appearPercentage = this->CalcAppearPercentage( callPoke, callPokePos, isBatugunDamage, m_prevCallPokeId, prevNotAppeared );
  const u32  randValue        = m_randSys.Next(100);
  return ( randValue < appearPercentage );
}

/**
 * @brief 助けに来るポケモンの立ち位置を取得する
 * @param callClientId  助けを呼ぶクライアントのID
 * @return 助けに来るポケモンの立ち位置
 * @retval BTL_POS_NULL  助けが来ない場合
 */
BtlPokePos IntrudeSystem::DecideAppearPokePos( BTL_CLIENT_ID callClientId ) const
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
 * @brief 助けが来る確率[%]を算出する
 * @param callPoke         助けを呼ぶポケモン
 * @param callPokePos      助けを呼ぶポケモンの立ち位置
 * @param isBatugunDamage  助けを呼ぶポケモンが効果抜群のダメージを受けたか？
 * @param prevCallPokeId   前回助けを呼んだポケモンのID( 呼んでいないなら BTL_POKEID_NULL )
 * @param prevNotAppeared  前回助けを呼んだ時に、助けが来なかったなら true
 * @retval [0, 100]
 */
u32 IntrudeSystem::CalcAppearPercentage( 
  const BTL_POKEPARAM& callPoke,
  BtlPokePos           callPokePos, 
  bool                 isBatugunDamage,
  u8                   prevCallPokeId, 
  bool                 prevNotAppeared ) const
{
#if PM_DEBUG
  // デバッグ機能：必ず助けに来る
  if( m_mainModule->GetDebugFlag( BTL_DEBUGFLAG_INTRUDE_FORCE_APPEAR ) )
  {
    return 100;
  }
#endif

  u32  callValue  = callPoke.GetValue( BTL_POKEPARAM::BPP_CALL_VALUE );
  fx32 rate       = this->CalcAppearPercentageRate( callPoke, callPokePos, isBatugunDamage, prevCallPokeId, prevNotAppeared );
  u32  percentage = calc::MulRatio( callValue, rate );
  percentage = gfl2::math::Min( percentage, static_cast<u32>(100) ); // 値域を[0, 100]にする
  return percentage;
}

/**
 * @brief 助けが来る確率の倍率を算出する
 * @param callPoke         助けを呼ぶポケモン
 * @param callPokePos      助けを呼ぶポケモンの立ち位置
 * @param isBatugunDamage  助けを呼ぶポケモンが効果抜群のダメージを受けたか？
 * @param prevCallPokeId   前回助けを呼んだポケモンのID( 呼んでいないなら BTL_POKEID_NULL )
 * @param prevNotAppeared  前回助けを呼んだ時に、助けが来なかったなら true
 */
fx32 IntrudeSystem::CalcAppearPercentageRate( 
  const BTL_POKEPARAM& callPoke,
  BtlPokePos           callPokePos, 
  bool                 isBatugunDamage,
  u8                   prevCallPokeId, 
  bool                 prevNotAppeared ) const
{
  fx32 rate = FX32_CONST(4.0f);

  // 相手の場に特性「いかく」等、乱入確率を上昇させるポケモンがいるなら、確率１．２倍
  if( this->IsPressureEnemyExist( callPokePos ) )
  {
    rate = FX_Mul( rate, FX32_CONST(1.2f) );
  }

  // 前のターンと同じポケモンが助けを呼ぶ場合は、確率１．５倍
  if( callPoke.GetID() == prevCallPokeId )
  {
    rate = FX_Mul( rate, FX32_CONST(1.5f) );
  }

  // 効果抜群の攻撃を受けていた場合は、確率２倍
  if( isBatugunDamage )
  {
    rate = FX_Mul( rate, FX32_CONST(2.0f) );
  }

  // 前のターンに呼んだが助けが来なかったなら、確率３倍
  if( prevNotAppeared )
  {
    rate = FX_Mul( rate, FX32_CONST(3.0f) );
  }

  return rate;
}

/**
 * @brief 相手の場に、特性「いかく」など、乱入確率を上昇させるポケモンが存在するか？
 * @param callPoke         助けを呼ぶポケモン
 * @param callPokePos      助けを呼ぶポケモンの立ち位置
 * @retval true   乱入確率を上昇させるポケモンが存在する
 * @retval false  乱入確率を上昇させるポケモンが存在しない
 */
bool IntrudeSystem::IsPressureEnemyExist( BtlPokePos callPokePos ) const
{
  const BTL_CLIENT_ID callClientId    = PokemonPosition::GetPosCoverClientId( BTL_RULE_INTRUDE, BTL_MULTIMODE_NONE, callPokePos );
  const BTL_CLIENT_ID enemyClientId   = BattleRule::GetOpponentClientId( BTL_RULE_INTRUDE, BTL_MULTIMODE_NONE, callClientId, 0 );
  const u32           enemySidePosNum = PokemonPosition::GetClientCoverPosNum( BTL_RULE_INTRUDE, BTL_MULTIMODE_NONE, enemyClientId );
  const BTL_PARTY*    enemySideParty  = m_pokecon->GetPartyDataConst( enemyClientId );

  for( u32 posIndex=0; posIndex<enemySidePosNum; ++posIndex )
  {
    const BTL_POKEPARAM* poke = enemySideParty->GetMemberDataConst( posIndex );

    if( ( poke == NULL   ) ||
        ( poke->IsDead() ) )
    {
      continue;
    }

    if( this->IsPressurePoke( *poke ) )
    {
      return true;
    }
  }

  return false;
}

/**
 * @brief 指定したポケモンが、相手の乱入確率を上昇させるポケモンか？
 * @param poke  チェック対象ポケモン
 * @retval true   相手の乱入確率を上昇させるポケモンである
 * @retval false  相手の乱入確率を上昇させるポケモンでない
 */
bool IntrudeSystem::IsPressurePoke( const BTL_POKEPARAM& poke ) const
{
  static const TokuseiNo PRESSURE_TOKUSEI_LIST[] = 
  {
    TOKUSEI_IKAKU,       // いかく
    TOKUSEI_PURESSYAA,   // プレッシャー
    TOKUSEI_KINTYOUKAN,  // きんちょうかん
  };

  const TokuseiNo tokusei = static_cast<TokuseiNo>( poke.GetValue( BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE ) );

  for( u32 i=0; i<GFL_NELEMS(PRESSURE_TOKUSEI_LIST); ++i )
  {
    if( PRESSURE_TOKUSEI_LIST[i] == tokusei )
    {
      return true;
    }
  }

  return false;
}

/**
 * @brief 乱入するポケモンのパラメータをセットアップする
 * @param[out] appearPoke  セットアップ対象のインスタンス
 * @param      appearPos   乱入ポケモンの位置
 * @param      weather     天候
 * @retval true   パラメータのセットアップに成功
 * @retval false  パラメータのセットアップに失敗
 */
bool IntrudeSystem::SetupAppearPokeParam( pml::pokepara::PokemonParam* appearPoke, BtlPokePos appearPos, BtlWeather weather )
{
#if PM_DEBUG
  // デバッグ機能：乱入ポケモンのパラメータ固定
  {
    const pml::pokepara::PokemonParam* debugParam = m_mainModule->DEBUG_GetIntrudePokeParam();
    if( debugParam != NULL )
    {
      appearPoke->CopyFrom( *debugParam );
      return true;
    }
  }
#endif

  // エンカウントデータから抽選し、乱入回数に応じたボーナスを付与する
  if( this->SetupAppearPokeParam_ByEncountData( appearPoke, appearPos, weather ) )
  {
    this->ApplyIntrudeCountBonus_TalentPower( appearPoke );
    this->ApplyIntrudeCountBonus_Tokusei( appearPoke );
    return true;
  }

  return false;
}

/**
 * @brief 乱入するポケモンのパラメータを、エンカウントデータを元にセットアップする
 * @param[out] appearPoke  セットアップ対象のインスタンス
 * @param      appearPos   乱入ポケモンの位置
 * @param      weather     天候
 * @retval true   パラメータのセットアップに成功
 * @retval false  パラメータのセットアップに失敗
 */
bool IntrudeSystem::SetupAppearPokeParam_ByEncountData( pml::pokepara::PokemonParam* appearPoke, BtlPokePos appearPos, BtlWeather weather )
{
  // 乱入ポケモンのパラメータ抽選に影響を与える相手( プレイヤ )側のポケモンを選択
  const BtlPokePos     opponentPos  = m_mainModule->GetOpponentPokePos( appearPos, 0 );
  const BTL_POKEPARAM* opponentPoke = m_pokecon->GetFrontPokeDataConst( opponentPos );
  if( opponentPoke == NULL )
  {
    return false;
  }

  // エンカウントデータから抽選
  const PokePara*              opponentPokeSrc = opponentPoke->GetSrcData();
  Field::Encount::IntruderType intruderType    = IntrudeSystem::GetIntruderType( weather );
  m_appearPokeParamGenerator->CheckEffectApplyIntruder( *opponentPokeSrc, &m_randSys );                      // 相手( プレイヤ )側のポケモンによる抽選への影響を更新( 特性「じりょく」など )
  u8 rareDrawCount = this->GetRareDrawCount( m_intrudeCount );
  return m_appearPokeParamGenerator->SetupLotteryIntruderPokeParam( appearPoke, &m_randSys, intruderType, rareDrawCount );  // エンカウントデータから抽選
}

/**
 * @brief 乱入するポケモンの種類( 通常・あめ・すなあらし・あられ )を取得する
 * @param weather   天候
 */
Field::Encount::IntruderType IntrudeSystem::GetIntruderType( BtlWeather weather ) const
{
  switch( weather )
  {
  case BTL_WEATHER_RAIN:
  case BTL_WEATHER_STORM:
    return Field::Encount::INTRUDER_TYPE_RAIN;

  case BTL_WEATHER_SAND:
    return Field::Encount::INTRUDER_TYPE_SAND;

  case BTL_WEATHER_SNOW:
    return Field::Encount::INTRUDER_TYPE_SNOW;

  default:
    return Field::Encount::INTRUDER_TYPE_NORMAL;
  }
}

/**
 * @brief 乱入回数に応じた、乱入ポケモンの色違い抽選回数を取得する
 * @param intrudeCount  乱入回数
 */
u8 IntrudeSystem::GetRareDrawCount( u32 intrudeCount ) const
{
  if( intrudeCount <= 10 )
  {
    return 1;
  }

  if( intrudeCount <= 20 )
  {
    return 5;
  }

  if( intrudeCount <= 30 )
  {
    return 9;
  }

  return 13;
}

/**
 * @brief 乱入するポケモンのパラメータに、乱入回数に応じたボーナスを適用する( 個体値 )
 * @param pokeParam  乱入ポケモンのパラメータ
 */
void IntrudeSystem::ApplyIntrudeCountBonus_TalentPower( pml::pokepara::PokemonParam* pokeParam )
{
  const u32 vNum = this->GetBonusVNum( m_intrudeCount );
  while( pokeParam->GetTalentPowerMaxNum() < vNum )
  {
    u32 rand = m_randSys.Next( pml::pokepara::POWER_NUM );
    pml::pokepara::PowerID powerId = static_cast<pml::pokepara::PowerID>( rand );
    pokeParam->ChangeTalentPower( powerId, pml::MAX_TALENT_POWER );
  }
}

/**
 * @brief 乱入回数に応じた、乱入ポケモンの個体値Vの数を取得する
 * @param intrudeCount  乱入回数
 */
u32 IntrudeSystem::GetBonusVNum( u32 intrudeCount ) const
{
  if( intrudeCount < 5 )
  {
    return 0;
  }

  if( intrudeCount < 10 )
  {
    return 1;
  }
  
  if( intrudeCount < 20 )
  {
    return 2;
  }

  if( intrudeCount < 30 )
  {
    return 3;
  }

  return 4;
}

/**
 * @brief 乱入するポケモンのパラメータに、乱入回数に応じたボーナスを適用する( 特性 )
 * @param pokeParam  乱入ポケモンのパラメータ
 */
void IntrudeSystem::ApplyIntrudeCountBonus_Tokusei( pml::pokepara::PokemonParam* pokeParam )
{
  const u32 rand = m_randSys.Next(100);
  const u32 percentage = this->Get3rdTokuseiPercentage( m_intrudeCount );
  if( !( rand < percentage ) )
  {
    return;
  }

  pokeParam->SetTokuseiIndex( 2 ); // 第３特性にする
}

/**
 * @brief 乱入回数に応じた、乱入ポケモンが隠れ特性になる確率[%]を取得する
 * @param intrudeCount  乱入回数
 */
u32 IntrudeSystem::Get3rdTokuseiPercentage( u32 intrudeCount ) const
{
  if( intrudeCount < 5 )
  {
    return 0;
  }

  if( intrudeCount < 10 )
  {
    return 0;
  }
  
  if( intrudeCount < 20 )
  {
    return 5;
  }

  if( intrudeCount < 30 )
  {
    return 10;
  }

  return 15;
}


GFL_NAMESPACE_END( btl )
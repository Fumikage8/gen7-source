//=============================================================================================
/**
 * @file  btl_field.cpp
 * @brief ポケモンXY バトルシステム 天候など「場」に対する効果に関する処理クラス
 * @author  taya
 *
 * @date  2010.12.22  作成
 */
//=============================================================================================

#include "./btl_common.h"
#include "./btl_MainModule.h"
#include "./btl_calc.h"
#include "./btl_sickcont.h"
#include "./btl_ServerFlow.h"
#include "./btl_pokecon.h"

//#include "btl_event_factor.h"
//#include "handler\hand_field.h"

#include "./handler/hand_field.h"
#include "./btl_field.h"

#if defined(GF_PLATFORM_WIN32)
#pragma warning( disable  : 4800 )  //ブール値を 'true' または 'false' に強制的に設定します (警告の処理)
#endif

GFL_NAMESPACE_BEGIN(btl)


/*=============================================================================================*/
/*                                                                                             */
/* BTL_FIELD と同等の機能をクライアント側に持たせる為の仕組み                                  */
/*                                                                                             */
/*=============================================================================================*/


FieldStatus::FieldStatus( void )
{
  m_fForServer  = false;

  this->initWork();
}

FieldStatus::~FieldStatus( )
{

}

void FieldStatus::Init( bool bForServer )
{
  m_fForServer = bForServer;

  this->initWork();
}

/**
 * ワーク領域を初期化
 */
void FieldStatus::initWork( void )
{
  for(u32 i=0; i<EFF_MAX; ++i)
  {
    clearFactorWork( static_cast<EffectType>( i ) );
  }

  m_weather            = BTL_WEATHER_NONE;
  m_weatherTurn        = BTL_WEATHER_TURN_PERMANENT;
  m_weatherTurnCount   = 0;
  m_weatherCausePokeID = BTL_POKEID_NULL;

  m_baseGround = m_currentGround = BTL_GROUND_NONE;
}

//---------------------------------------------------------------------
/**
 * 天候取得
 *
 * @retval  BtlWeather
 */
//---------------------------------------------------------------------
BtlWeather FieldStatus::GetWeather( void ) const
{
  return m_weather;
}

//---------------------------------------------------------------------
/**
 * @brief 天候の経過ターンを取得する
 */
//---------------------------------------------------------------------
u32 FieldStatus::GetWeatherPassedTurn( void ) const
{
  return m_weatherTurnCount;
}

//---------------------------------------------------------------------
/**
 * @brief 天候の経過ターンをインクリメントする
 */
//---------------------------------------------------------------------
void FieldStatus::IncWeatherPassedTurn( void )
{
  m_weatherTurnCount++;
}

//---------------------------------------------------------------------
/**
 * 天候残りターン取得
 *
 * @retval  u32
 */
//---------------------------------------------------------------------
u32 FieldStatus::GetWeatherRemainingTurn( void ) const
{
  if( m_weather == BTL_WEATHER_NONE ) {
    return 0;
  }

  if( m_weatherTurn == BTL_WEATHER_TURN_PERMANENT ) {
    return BTL_WEATHER_TURN_PERMANENT;
  }

  u32 remainingTurn = m_weatherTurn - m_weatherTurnCount;
  return remainingTurn;
}

//---------------------------------------------------------------------
/**
 * @brief 現在の天候にしたポケモンのIDを取得する
 * @retval BTL_POKEID_NULL  ポケモンによって設定された天候でない場合
 */
//---------------------------------------------------------------------
u8 FieldStatus::GetWeatherCausePokeID( void ) const
{
  return m_weatherCausePokeID;
}

//---------------------------------------------------------------------
/**
 * @brief 天候の全ターン数を取得する
 */
//---------------------------------------------------------------------
u32 FieldStatus::GetWeatherWholeTurn( void ) const
{
  u32 passed = this->GetWeatherPassedTurn();
  u32 remaining = this->GetWeatherRemainingTurn();
  return ( passed + remaining );
}

//---------------------------------------------------------------------
/**
 * @brief 天候の全ターン数のうち、道具や特性により増加したターン数を取得する
 */
//---------------------------------------------------------------------
u32 FieldStatus::GetWeatherTurnUpCount( void ) const
{
  return m_weatherTurnUpCount;
}

//---------------------------------------------------------------------
/**
 *  天候セット
 *
 * @param weather      天候
 * @param turn         天候の継続ターン数
 * @param turnUpCount  天候の継続ターン数のうち、道具や特性などで増加したターン数
 * @param causePokeID  天候を変化させたポケモンのID( 存在しないなら BTL_POKEID_NULL )
 *
 */
//---------------------------------------------------------------------
void FieldStatus::SetWeather( BtlWeather weather, u16 turn, u16 turnUpCount, u8 causePokeID )
{
  m_weather            = weather;
  m_weatherTurn        = turn;
  m_weatherTurnUpCount = turnUpCount;
  m_weatherTurnCount   = 0;
  m_weatherCausePokeID = causePokeID;
  
  //ハンドラがあった場合は消しておく
  if( m_factor[ EFF_WEATHER ] )
  {
    //RemoveEffectは使わない。ターン継続とかやってるっぽいから
    handler::field::Remove( m_factor[EFF_WEATHER] );
    m_factor[EFF_WEATHER] = NULL;
  }

  //@バトル変更箇所
  //天候にハンドラ
  if( weather != BTL_WEATHER_NONE )
  {
    //addEffectCoreは使わない。ターン継続とかやってるっぽいから
    //サーバーの時だけ追加する事。
    if( m_fForServer )
    {
      m_factor[ EFF_WEATHER ] = handler::field::Add( EFF_WEATHER, weather );
    }
  }
}
//---------------------------------------------------------------------
/**
 *  天候終了
 */
//---------------------------------------------------------------------
void FieldStatus::EndWeather( void )
{
  m_weather            = BTL_WEATHER_NONE;
  m_weatherTurn        = 0;
  m_weatherTurnCount   = 0;
  m_weatherCausePokeID = BTL_POKEID_NULL;

  //@バトル変更箇所
  //天候にハンドラ
  if( m_factor[ EFF_WEATHER ] )
  {
    //RemoveEffectは使わない。ターン継続とかやってるっぽいから
    handler::field::Remove( m_factor[EFF_WEATHER] );
    m_factor[EFF_WEATHER] = NULL;
  }
}

//---------------------------------------------------------------------
/**
 * ターンチェック
 *
 * @retval  BtlWeather    ターンチェックにより終わった天候
 */
//---------------------------------------------------------------------
BtlWeather FieldStatus::TurnCheckWeather( void )
{
  if( m_weather != BTL_WEATHER_NONE )
  {
    if( m_weatherTurn != BTL_WEATHER_TURN_PERMANENT )
    {
      this->IncWeatherPassedTurn();
      if( m_weatherTurn <= m_weatherTurnCount )
      {
        BtlWeather endWeather = m_weather;
        m_weather = BTL_WEATHER_NONE;
        m_weatherCausePokeID = BTL_POKEID_NULL;
        return endWeather;
      }
    }
  }
  return BTL_WEATHER_NONE;
}
//---------------------------------------------------------------------
/**
 * フィールドエフェクト追加
 *
 * @param   state
 *
 * @retval  bool
 */
//---------------------------------------------------------------------
bool FieldStatus::AddEffect( EffectType effect, const BTL_SICKCONT& cont )
{
  GFL_ASSERT(effect < EFF_MAX);
  GFL_ASSERT(effect != EFF_GROUND); // Ground系は SetBaseGround, ChangeGround を使う

  return addEffectCore( effect, cont, 0 );
}
//---------------------------------------------------------------------
/**
 * フィールドエフェクト追加コア処理
 *
 * @param   effect
 * @param   cont
 * @param   sub_param
 *
 * @retval  bool
 */
//---------------------------------------------------------------------
bool FieldStatus::addEffectCore( EffectType effect, const BTL_SICKCONT& cont, u16 sub_param )
{
  if( !m_enableFlag[effect] )
  {
    if( m_fForServer )
    {
      m_factor[ effect ] = handler::field::Add( effect, static_cast<u8>(sub_param) );
      if( m_factor[ effect ] == NULL )
      {
        return false;
      }
    }

    m_enableFlag[ effect ] = true;

    m_cont[ effect ] = cont;
    m_turnCount[ effect ] = 0;
    m_dependPokeCount[ effect ] = 0;
    m_subParam[ effect ] = sub_param;
    for(u32 i=0; i<DEPEND_POKE_NUM_MAX; ++i){
      m_dependPokeID[ effect ][i] = BTL_POKEID_NULL;
    }

    {
      u8 dependPokeID = SICKCONT_GetPokeID( cont );
      if( dependPokeID != BTL_POKEID_NULL )
      {
        AddDependPoke( effect, dependPokeID );
      }
    }

    return true;
  }

  return false;
}
//---------------------------------------------------------------------
/**
 * フィールドエフェクト除去
 *
 * @param   effect
 */
//---------------------------------------------------------------------
bool FieldStatus::RemoveEffect( EffectType effect )
{
  GFL_ASSERT_MSG(effect < EFF_MAX, "effNo=%d, max=%d\n", effect, EFF_MAX);

  if( CheckEffect(effect) )
  {
    if( m_factor[effect] != NULL )
    {
      handler::field::Remove( m_factor[effect] );
      m_factor[effect] = NULL;
    }
    clearFactorWork( effect );
    return true;
  }
  return false;
}
//---------------------------------------------------------------------
/**
 * 依存対象ポケモンを追加
 *
 * @param   EffectType
 * @param   pokeID
 *
 * @retval  bool  新規追加できたらtrue / 既に登録されてる場合＆登録失敗したらfalse
 */
//---------------------------------------------------------------------
bool FieldStatus::AddDependPoke( EffectType effect, u8 pokeID )
{
  GFL_ASSERT(effect < EFF_MAX);

  if( CheckEffect(effect) )
  {
    u32 count = m_dependPokeCount[ effect ];

    if( count < DEPEND_POKE_NUM_MAX )
    {
      for(u32 i=0; i<count; ++i)
      {
        if( m_dependPokeID[ effect ][i] == pokeID ){
          return false;
        }
      }

      m_dependPokeID[ effect ][ count++ ] = pokeID;
      m_dependPokeCount[ effect ] = count;
      return true;
    }
  }
  return false;
}
//---------------------------------------------------------------------
/**
 * 指定エフェクトの依存対象ポケモンかどうか判定
 *
 * @param   effect
 * @param   pokeID
 *
 * @retval  bool
 */
//---------------------------------------------------------------------
bool FieldStatus::IsDependPoke( EffectType effect, u8 pokeID ) const
{
  GFL_ASSERT(effect < EFF_MAX);

  if( CheckEffect(effect) )
  {
    u32 count = m_dependPokeCount[ effect ];

    for(u32 i=0; i<count; ++i)
    {
      if( m_dependPokeID[ effect ][i] == pokeID ){
        return true;
      }
    }
  }
  return false;
}
//---------------------------------------------------------------------
/**
 * 特定ポケモン依存のエフェクトを全て除去
 *
 * @param   pokeID
 */
//---------------------------------------------------------------------
void FieldStatus::RemoveDependPokeEffect( u8 pokeID )
{
  for(u32 i=0; i<EFF_MAX; ++i)
  {
    if( CheckEffect(static_cast<EffectType>(i)) )
    {
      if( m_dependPokeCount[i] )
      {
        u32 p, fMatch = false;
        for(p=0; p<m_dependPokeCount[i]; ++p)
        {
          if( m_dependPokeID[i][p] == pokeID ){
            fMatch = true;
            break;
          }
        }
        if( fMatch )
        {
          for( ; p<(DEPEND_POKE_NUM_MAX-1); ++p){
            m_dependPokeID[i][p] = m_dependPokeID[i][p+1];
          }
          m_dependPokeID[i][p] = BTL_POKEID_NULL;
          m_dependPokeCount[i]--;

          if( m_dependPokeCount[i] == 0 )
          {
            if( m_factor[i] != NULL ){
              handler::field::Remove( m_factor[i] );
              m_factor[i] = NULL;
            }
            clearFactorWork( static_cast<EffectType>(i) );
          }
          else if( SICKCONT_GetPokeID(m_cont[i]) == pokeID )
          {
            u8 nextPokeID = m_dependPokeID[i][0];
            SICKCONT_SetPokeID( &(m_cont[i]), nextPokeID );
          }
        }
      }
    }
  }
}
//---------------------------------------------------------------------
/**
 * ふういん禁止対象のワザかどうか判定
 *
 * @param   pokeCon
 * @param   attacker
 *
 * @retval  bool
 */
//---------------------------------------------------------------------
bool FieldStatus::CheckFuin( const MainModule& mainModule, const POKECON* pokeCon, const BTL_POKEPARAM* attacker, WazaID waza )
{
  u8  atkPokeID = attacker->GetID();
  u8  fuinPokeID;
  u32 i, fuinPokeCnt = m_dependPokeCount[ EFF_FUIN ];

  for(i=0; i<fuinPokeCnt; ++i)
  {
    fuinPokeID = m_dependPokeID[ EFF_FUIN ][ i ];

    BTL_N_Printf( DBGSTR_FIDLD_FuinCheck, i, fuinPokeID, atkPokeID, waza );
    if( !mainModule.IsFriendPokeID(atkPokeID, fuinPokeID) )
    {
      const BTL_POKEPARAM* bpp = pokeCon->GetPokeParamConst( fuinPokeID );
      if( bpp->WAZA_IsUsable(waza) )
      {
        BTL_N_Printf( DBGSTR_FIELD_FuinHit, fuinPokeID, waza);
        return true;
      }
    }
  }
  return false;
}
//---------------------------------------------------------------------
/**
 * ターンチェック
 */
//---------------------------------------------------------------------
void FieldStatus::TurnCheck( ServerFlow* flowWk )
{
  for(u32 i=1; i<EFF_MAX; ++i)
  {
    if( CheckEffect(static_cast<EffectType>(i)) )
    {
      u8 turnMax = SICCONT_GetTurnMax( m_cont[i] );
      BTL_PRINT("フィールドエフェクト(%d) の最大ターン数 = %d\n", i, turnMax);
      if( turnMax )
      {
        BTL_PRINT("　継続ターン数 = %d\n", m_turnCount[i]);
        if( ++(m_turnCount[i]) >= turnMax )
        {
          BTL_PRINT("　(%d)おわります\n", i);

          if( m_factor[i] ){
            handler::field::Remove( m_factor[i] );
            m_factor[i] = NULL;
          }
          clearFactorWork( static_cast<EffectType>(i) );

          if( flowWk ){
//-            callbackFunc( static_cast<EffectType>(i), callbackArg );
            flowWk->TurnCheckCallback_Field( static_cast<EffectType>(i) );
          }
        }
      }
    }
  }
}
//---------------------------------------------------------------------
/**
 * 特定のフィールドエフェクトが働いているかチェック
 *
 * @param   effect
 *
 * @retval  bool
 */
//---------------------------------------------------------------------
bool FieldStatus::CheckEffect( EffectType effect ) const
{
  if( effect < EFF_MAX )
  {
    return m_enableFlag[ effect ];
  }
  GFL_ASSERT(0);
  return false;
}
//---------------------------------------------------------------------
/**
 * 特定のフィールドエフェクトが働いているかチェック（サブパラメータも同時にチェック）
 *
 * @param   effect
 *
 * @retval  bool
 */
//---------------------------------------------------------------------
bool FieldStatus::CheckEffect( EffectType effect, u16 subParam ) const
{
  if( effect < EFF_MAX )
  {
    if( m_enableFlag[effect] )
    {
      if( m_subParam[effect] == subParam ){
        return true;
      }
    }
    return false;
  }
  GFL_ASSERT(0);
  return false;
}

//---------------------------------------------------------------------
/**
 * 指定フィールドエフェクトの残りターン数を取得
 *
 * @param   effect
 *
 * @retval  uint32_t
 */
//---------------------------------------------------------------------
uint32_t FieldStatus::CheckRemainingTurn( EffectType effect ) const
{
  if( effect < EFF_MAX )
  {
    u32 turnMax = SICCONT_GetTurnMax( m_cont[effect] );
    u32 turnPassed = this->GetPassedTurn( effect );
    if( ( turnMax != 0 )
    &&  ( turnPassed < turnMax )
    ){
      return  turnMax - turnPassed;
    }
    return 0;
  }
  else
  {
    GFL_ASSERT(0);
    return 0;
  }
}

//---------------------------------------------------------------------
/**
 * @brief フィールドエフェクトの経過ターン数を取得する
 * @param effect  フィールドエフェクト
 */
//---------------------------------------------------------------------
u32 FieldStatus::GetPassedTurn( EffectType effect ) const
{
  if( EFF_MAX <= effect ) 
  {
    GFL_ASSERT(0);
    return 0;
  }

  return m_turnCount[ effect ];
}

//---------------------------------------------------------------------
/**
 * @brief フィールドエフェクトの全ターン数を取得する
 * @param effect  フィールドエフェクト
 */
//---------------------------------------------------------------------
u32 FieldStatus::GetWholeTurn( EffectType effect ) const
{
  u32 passed = this->GetPassedTurn( effect );
  u32 remaining = this->CheckRemainingTurn( effect );
  return ( passed + remaining );
}

//---------------------------------------------------------------------
/**
 * 継続依存ポケモンIDを返す
 *
 * @param   effect
 *
 * @retval  u8
 */
//---------------------------------------------------------------------
u8 FieldStatus::GetDependPokeID( EffectType effect ) const
{
  GFL_ASSERT(effect < EFF_MAX);

  if( CheckEffect(effect) )
  {
    return SICKCONT_GetPokeID( m_cont[effect] );
  }

  return BTL_POKEID_NULL;
}


void FieldStatus::clearFactorWork( EffectType effect )
{
  m_factor[ effect ] = NULL;
  m_cont[ effect ] = SICKCONT_MakeNull();
  m_turnCount[ effect ] = 0;
  m_dependPokeCount[ effect ] = 0;
  m_enableFlag[ effect ] = false;

  for(u32 i=0; i<DEPEND_POKE_NUM_MAX; ++i)
  {
    m_dependPokeID[effect][i] = BTL_POKEID_NULL;
  }
}



//---------------------------------------------------------------------
/**
 * 基本グラウンド状態を設定（一時フィールド状態がターン経過などで失効した際、この状態に戻る）
 *
 * @param   ground
 */
//---------------------------------------------------------------------
void FieldStatus::SetBaseGround( BtlGround  ground )
{
  if( m_baseGround == BTL_GROUND_NONE )
  {
    m_baseGround = m_currentGround = ground;

    if( ground != BTL_GROUND_NONE )
    {
      BTL_SICKCONT  cont = SICKCONT_MakePermanent( BTL_POKEID_NULL );
      addEffectCore( EFF_GROUND, cont, ground );
    }
  }
}
//---------------------------------------------------------------------
/**
 * 一時グラウンド状態を設定
 *
 * @param   ground
 * @param   turn
 */
//---------------------------------------------------------------------
bool FieldStatus::ChangeGround( BtlGround  ground, BTL_SICKCONT cont )
{
  if( m_currentGround != ground )
  {
    RemoveEffect( EFF_GROUND );
    if( ground != BTL_GROUND_NONE )
    {
      if( ground == m_baseGround ){
        cont = SICKCONT_MakePermanent( BTL_POKEID_NULL );
      }
      addEffectCore( EFF_GROUND, cont, ground );
    }
    m_currentGround     = ground;
    return true;
  }
  return false;
}
//---------------------------------------------------------------------
/**
 * 現在のグラウンド状態を取得
 *
 * @retval  BtlGround
 */
//---------------------------------------------------------------------
BtlGround  FieldStatus::GetGround( void ) const
{
  return m_currentGround;
}
BtlGround  FieldStatus::GetBaseGround( void ) const
{
  return m_baseGround;
}

//---------------------------------------------------------------------
/**
 * @brief 現在のグラウンドの経過ターン数を取得する
 */
//---------------------------------------------------------------------
u32 FieldStatus::GetGroundPassedTurn( void ) const
{
  return this->GetPassedTurn( EFF_GROUND );
}

//---------------------------------------------------------------------
/**
 * @brief 現在のグラウンドの残りターン数を取得する
 */
//---------------------------------------------------------------------
u32 FieldStatus::GetGroundRemainingTurn( void ) const
{
  u32 wholeTurn = this->GetGroundWholeTurn();
  u32 passedTurn = this->GetGroundPassedTurn();
  u32 remainTurn = wholeTurn - passedTurn;
  return remainTurn;
}

//---------------------------------------------------------------------
/**
 * @brief 現在のグラウンドの全体のターン数を取得する
 */
//---------------------------------------------------------------------
u32 FieldStatus::GetGroundWholeTurn( void ) const
{
  return this->GetWholeTurn( EFF_GROUND );
}

//---------------------------------------------------------------------
/**
 * @brief グラウンド継続ターン数の増加分を取得する
 */
//---------------------------------------------------------------------
u32 FieldStatus::GetGroundTurnUpCount( void ) const
{
  const BtlGround ground = this->GetGround();

  if( ground == BTL_GROUND_NONE )
  {
    return 0;
  }

  const BTL_SICKCONT& sickcont = m_cont[ EFF_GROUND ];
  return SICKCONT_GetParam( sickcont ); // ターン数の増加分は汎用パラメータにセットされている
}

//---------------------------------------------------------------------
/**
 * @brief 現在のグラウンドを引き起こしたポケモンのIDを取得する
 */
//---------------------------------------------------------------------
u8 FieldStatus::GetGroundCausePokeID( void ) const
{
  const BtlGround ground = this->GetGround();

  if( ground == BTL_GROUND_NONE )
  {
    return 0;
  }

  const BTL_SICKCONT& sickcont = m_cont[ EFF_GROUND ];
  return SICKCONT_GetCausePokeID( sickcont );
}



GFL_NAMESPACE_END(btl)

#if defined(GF_PLATFORM_WIN32)
#pragma warning( default  : 4800 )
#endif

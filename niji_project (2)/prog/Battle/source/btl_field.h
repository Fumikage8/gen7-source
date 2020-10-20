#if !defined( __BTL_FIELD_H__ )
#define __BTL_FIELD_H__
//=============================================================================================
/**
 * @file  btl_field.h
 * @brief ポケモンXY バトルシステム 天候など「場」に対する効果に関する処理クラス
 * @author  taya
 *
 * @date  2010.12.22  作成
 */
//=============================================================================================
#pragma once

#include "Battle/include/battle_SetupParam.h"

#include "./btl_common.h"
#include "./btl_sickcont.h"
#include "./btl_EventFactor.h"

GFL_NAMESPACE_BEGIN(btl)


class MainModule;
class ServerFlow;
class POKECON;
class BTL_POKEPARAM;

class FieldStatus {
  GFL_FORBID_COPY_AND_ASSIGN( FieldStatus );

public:
  /**
   *  バトル独自のフィールド状態定義
   *  Pawnスクリプトから参照できるよう、別ファイルに記述します。
   */
   #include "./btl_fieldEff_def.h"


  /**
   *  ターンチェック処理のコールバック関数型
   */
  typedef void (*TurnCheckCallback)( EffectType, void* );

public:
  FieldStatus();
  ~FieldStatus();
  void  Init( bool bForServer );

  BtlWeather    GetWeather( void ) const;
  u32           GetWeatherPassedTurn( void ) const;
  u32           GetWeatherRemainingTurn( void ) const;
  u32           GetWeatherWholeTurn( void ) const;
  u32           GetWeatherTurnUpCount( void ) const;
  u8            GetWeatherCausePokeID( void ) const;
  void          SetWeather( BtlWeather weather, u16 turn, u16 turnUpCount, u8 causePokeID );
  void          EndWeather( void );
  BtlWeather    TurnCheckWeather( void );
  void          IncWeatherPassedTurn( void );

  bool          AddEffect( EffectType effect, const BTL_SICKCONT& cont );
  bool          RemoveEffect( EffectType effect );
  bool          AddDependPoke( EffectType effect, u8 pokeID );
  bool          IsDependPoke( EffectType effect, u8 pokeID ) const;
  void          RemoveDependPokeEffect( u8 pokeID );

  bool          CheckFuin( const MainModule& mainModule, const POKECON* pokeCon, const BTL_POKEPARAM* attacker, WazaID waza );
  void          TurnCheck( ServerFlow* flowWk );
  bool          CheckEffect( EffectType effect ) const;
  bool          CheckEffect( EffectType effect, u16 subParam ) const;
  u32           CheckRemainingTurn( EffectType effect ) const;
  u32           GetPassedTurn( EffectType effect ) const;
  u32           GetWholeTurn( EffectType effect ) const;
  u8            GetDependPokeID( EffectType effect ) const;

  void          SetBaseGround( BtlGround  ground );
  bool          ChangeGround( BtlGround  ground, BTL_SICKCONT cont );
  BtlGround     GetGround( void ) const;
  BtlGround     GetBaseGround( void ) const;
  u32           GetGroundPassedTurn( void ) const;
  u32           GetGroundRemainingTurn( void ) const;
  u32           GetGroundWholeTurn( void ) const;
  u32           GetGroundTurnUpCount( void ) const;
  u8            GetGroundCausePokeID( void ) const;

private:

  //--------------------------------------------------------------
  /**
   *  定数
   */
  //--------------------------------------------------------------
  enum {
    TURN_MAX = 16,
    DEPEND_POKE_NUM_MAX = BTL_POS_NUM,
  };

  bool                m_fForServer;

  BtlWeather          m_weather;
  u32                 m_weatherTurn;
  u32                 m_weatherTurnUpCount;
  u32                 m_weatherTurnCount;
  u8                  m_weatherCausePokeID;
  BtlGround           m_baseGround;
  BtlGround           m_currentGround;


  BTL_EVENT_FACTOR*   m_factor[ EFF_MAX ];
  BTL_SICKCONT        m_cont[ EFF_MAX ];
  u32                 m_turnCount[ EFF_MAX ];
  u32                 m_dependPokeID[ EFF_MAX ][ DEPEND_POKE_NUM_MAX ];
  u32                 m_dependPokeCount[ EFF_MAX ];
  u32                 m_enableFlag[ EFF_MAX ];
  u16                 m_subParam[ EFF_MAX ];

  void initWork( void );
  bool addEffectCore( EffectType effect, const BTL_SICKCONT& cont, u16 sub_param );
  void clearFactorWork( EffectType eff );

};



GFL_NAMESPACE_END(btl)

#endif // __BTL_FIELD_H__

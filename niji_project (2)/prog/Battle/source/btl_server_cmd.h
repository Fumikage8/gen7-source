#if !defined( __BTL_SERVER_CMD_H__ )
#define __BTL_SERVER_CMD_H__
//=============================================================================================
/**
 * @file  btl_server_cmd.h
 * @brief ポケモンXY バトルシステム サーバコマンド生成，解釈処理
 * @author  taya
 *
 * @date  2010.12.22  作成
 */
//=============================================================================================
#pragma once
#include <util/include/gfl2_std_string.h>

#include "./btl_common.h"
#include "./btl_pokeparam.h"
#include "./btl_server_const.h"
#include "./btl_PosEff.h"
#include "./btl_SideEff.h"


GFL_NAMESPACE_BEGIN(btl)
namespace scmd {

extern void SCQUE_PUT_Common( SCQUE* que, ServerCmd cmd, ... );

extern u16  SCQUE_RESERVE_Pos( SCQUE* que, ServerCmd cmd );
extern void SCQUE_PUT_ReservedPos( SCQUE* que, u16 pos, ServerCmd cmd, ... );

extern ServerCmd SCQUE_Read( SCQUE* que, int* args, u32 argElems );

extern void SCQUE_PUT_ExArgStart( SCQUE* que, u8 argCnt );
extern void SCQUE_PUT_ArgOnly( SCQUE* que, u8 arg );
extern u8 SCQUE_READ_ArgOnly( SCQUE* que );

extern void SCQUE_PUT_MsgImpl( SCQUE* que, u8 scType, ... );


//---------------------------------------------------
// que の読み出し状態を保存、復元するための仕組み
//---------------------------------------------------
typedef unsigned int QUE_READING_STATE;
enum {
  QUE_READING_STATE_NULL = 0xffffffff,
};

extern QUE_READING_STATE  SCQUE_ReadStatePush( SCQUE* que );
extern void               SCQUE_ReadStatePop( SCQUE* que, QUE_READING_STATE state );
extern void               SCQUE_PutWazaSyncChapter( SCQUE* que );
extern void               SCQUE_ReplaceSkipCmd( SCQUE* que, QUE_READING_STATE state );


#if PM_DEBUG
extern u32 SCMD_GetMaxLabelLength( void );
extern const char* SCMD_GetLabelString( ServerCmd cmd );
#endif // PM_DEBUG

} // end of namespace 'scmd'


//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
inline void SCQUE_Init( SCQUE* que )
{
  que->writePtr = 0;
  que->readPtr = 0;
}
inline void SCQUE_Setup( SCQUE* que, const void* data, u16 dataLength )
{
  GFL_ASSERT(dataLength<BTL_SERVERCMD_QUE_SIZE);

  gfl2::std::MemCopy( data, reinterpret_cast<void*>(que->buffer), dataLength );
  que->writePtr = dataLength;
  que->readPtr = 0;
}
inline bool SCQUE_IsFinishRead( const SCQUE* que )
{
  return que->readPtr == que->writePtr;
}

//------------------------------------------------------------------------------------

inline void SCQUE_PUT_OP_HpMinus( SCQUE* que, u8 pokeID, u16 value )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_HP_MINUS, pokeID, value );
}
inline void SCQUE_PUT_OP_HpMinusForSyncWazaEffect( SCQUE* que, u8 pokeID, u16 value )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_HP_MINUS_SYNC_WAZAEFF, pokeID, value );
}
inline void SCQUE_PUT_OP_HpPlus( SCQUE* que, u8 pokeID, u16 value )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_HP_PLUS, pokeID, value );
}
inline void SCQUE_PUT_OP_HpZero( SCQUE* que, u8 pokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_HP_ZERO, pokeID );
}

inline void SCQUE_PUT_OP_PPMinus( SCQUE* que, u8 pokeID, u8 wazaIdx, u8 value )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_PP_MINUS, pokeID, wazaIdx, value );
}
inline void SCQUE_PUT_OP_PPMinus_Org( SCQUE* que, u8 pokeID, u8 wazaIdx, u8 value )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_PP_MINUS_ORG, pokeID, wazaIdx, value );
}
inline void SCQUE_PUT_OP_WazaUsed( SCQUE* que, u8 pokeID, u8 wazaIdx )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_WAZA_USED, pokeID, wazaIdx );
}
inline void SCQUE_PUT_OP_PPPlus( SCQUE* que, u8 pokeID, u8 wazaIdx, u8 value )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_PP_PLUS, pokeID, wazaIdx, value );
}
inline void SCQUE_PUT_OP_PPPlus_Org( SCQUE* que, u8 pokeID, u8 wazaIdx, u8 value )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_PP_PLUS_ORG, pokeID, wazaIdx, value );
}

inline void SCQUE_PUT_OP_RankUp( SCQUE* que, u8 pokeID, u8 statusType, u8 volume )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_RANK_UP, pokeID, statusType, volume );
}

inline void SCQUE_PUT_OP_RankDown( SCQUE* que, u8 pokeID, u8 statusType, u8 volume )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_RANK_DOWN, pokeID, statusType, volume );
}

inline void SCQUE_PUT_OP_RankSet8( SCQUE* que, u8 pokeID, u8 atk, u8 def, u8 sp_atk, u8 sp_def, u8 agi, u8 hit, u8 avoid, u8 critical_rank )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_RANK_SET8, pokeID, atk, def, sp_atk, sp_def, agi, hit, avoid, critical_rank );
}
inline void SCQUE_PUT_OP_RankRecover( SCQUE* que, u8 pokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_RANK_RECOVER, pokeID );
}
inline void SCQUE_PUT_OP_RankUpReset( SCQUE* que, u8 pokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_RANK_UP_RESET, pokeID );
}
inline void SCQUE_PUT_OP_RankReset( SCQUE* que, u8 pokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_RANK_RESET, pokeID );
}

inline void SCQUE_PUT_OP_AddCritical( SCQUE* que, u8 pokeID, int value )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_ADD_CRITICAL, pokeID, value );
}


inline void SCQUE_PUT_OP_SetSick( SCQUE* que, u8 pokeID, u8 sick, const BTL_SICKCONT& contParam )
{
  u32 cont_high = 0;
  u32 cont_low  = 0;
  SICKCONT_Split32bit( contParam, &cont_high, &cont_low );
  scmd::SCQUE_PUT_Common( que, SC_OP_SICK_SET, pokeID, sick, cont_high, cont_low );
}

inline void SCQUE_PUT_OP_CurePokeSick( SCQUE* que, u8 pokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_CURE_POKESICK, pokeID );
}
inline void SCQUE_PUT_OP_CureWazaSick( SCQUE* que, u8 pokeID, u16 sickID )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_CURE_WAZASICK, pokeID, sickID );
}
inline void SCQUE_PUT_OP_MemberIn( SCQUE* que, u8 clientID, u8 posIdx, u8 memberIdx, u16 turnCount )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_MEMBER_IN, clientID, posIdx, memberIdx, turnCount );
}
inline void SCQUE_PUT_OP_SetStatus( SCQUE* que, u8 pokeID, u8 vid, u16 value )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_SET_STATUS, pokeID, vid, value );
}
inline void SCQUE_PUT_OP_SetWeight( SCQUE* que, u8 pokeID, u16 weight )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_SET_WEIGHT, pokeID, weight );
}

inline void SCQUE_PUT_OP_WazaSickTurnCheck( SCQUE* que, u8 pokeCnt, u8 maxSickIdx, u32 packID )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_WAZASICK_TURNCHECK, pokeCnt, maxSickIdx, packID );
}

inline void SCQUE_PUT_OP_ChangePokeType( SCQUE* que, u8 pokeID, u16 typePair, BTL_POKEPARAM::ExTypeCause exTypeCause )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_CHANGE_POKETYPE, pokeID, exTypeCause, typePair );
}

inline void SCQUE_PUT_OP_ExPokeType( SCQUE* que, u8 pokeID, u8 typeEx, BTL_POKEPARAM::ExTypeCause exTypeCause )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_EX_POKETYPE, pokeID, typeEx, exTypeCause );
}

inline void SCQUE_PUT_OP_ConsumeItem( SCQUE* que, u8 pokeID, u16 itemID  )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_CONSUME_ITEM, pokeID, itemID );
}

inline void SCQUE_PUT_OP_UpdateWazaProcResult( SCQUE* que,
    u8 pokeID, BtlPokePos actTargetPos, pml::PokeType actWazaType, bool fActEnable, bool fActZenryoku, WazaID actWaza, WazaID orgWaza )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_UPDATE_USE_WAZA, pokeID, actTargetPos, actWazaType, fActEnable, fActZenryoku, actWaza, orgWaza );

}

inline void SCQUE_PUT_OP_SetTurnFlag( SCQUE* que, u8 pokeID, BTL_POKEPARAM::TurnFlag flag )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_SET_TURNFLAG, pokeID, flag );
}
inline void SCQUE_PUT_OP_ResetTurnFlag( SCQUE* que, u8 pokeID, BTL_POKEPARAM::TurnFlag flag )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_RESET_TURNFLAG, pokeID, flag );
}



inline void SCQUE_PUT_OP_SetContFlag( SCQUE* que, u8 pokeID, BTL_POKEPARAM::ContFlag flag )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_SET_CONTFLAG, pokeID, flag );
}
inline void SCQUE_PUT_OP_ResetContFlag( SCQUE* que, u8 pokeID, BTL_POKEPARAM::ContFlag flag )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_RESET_CONTFLAG, pokeID, flag );
}
inline void SCQUE_PUT_OP_SetPermFlag( SCQUE* que, u8 pokeID, BTL_POKEPARAM::PermFlag flag )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_SET_PERMFLAG, pokeID, flag );
}
inline void SCQUE_PUT_OP_IncBattleTurnCount( SCQUE* que )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_INC_BATTLE_TURN_COUNT );
}
inline void SCQUE_PUT_OP_IncPokeTurnCount( SCQUE* que, u8 pokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_INC_POKE_TURN_COUNT, pokeID );
}
inline void SCQUE_PUT_OP_ChangeTokusei( SCQUE* que, u8 pokeID, u16 tokID )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_CHANGE_TOKUSEI, pokeID, tokID );
}
inline void SCQUE_PUT_OP_SetItem( SCQUE* que, u8 pokeID, u16 itemID )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_SET_ITEM, pokeID, itemID );
}
inline void SCQUE_PUT_OP_UpdateWaza( SCQUE* que, u8 pokeID, u8 wazaIdx, WazaID wazaID, u8 ppMax, u8 fPermanent )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_UPDATE_WAZANUMBER, pokeID, wazaIdx, ppMax, fPermanent, wazaID );
}
inline void SCQUE_PUT_OP_OutClear( SCQUE* que, u8 pokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_OUTCLEAR, pokeID );
}
inline void SCQUE_PUT_OP_AddFieldEffect( SCQUE* que, u8 eff, const BTL_SICKCONT& cont )
{
  u32 cont_high = 0;
  u32 cont_low = 0;
  SICKCONT_Split32bit( cont, &cont_high, &cont_low );
  scmd::SCQUE_PUT_Common( que, SC_OP_ADD_FLDEFF, eff, cont_high, cont_low );
}
inline void SCQUE_PUT_OP_AddFieldEffectDepend( SCQUE* que, u8 eff, u8 pokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_ADD_FLDEFF_DEPEND, eff, pokeID );
}
inline void SCQUE_OP_ChangeGround( SCQUE* que, BtlGround grnd, const BTL_SICKCONT& cont )
{
  u32 cont_high = 0;
  u32 cont_low  = 0;
  SICKCONT_Split32bit( cont, &cont_high, &cont_low );
  scmd::SCQUE_PUT_Common( que, SC_OP_CHANGE_GROUND, grnd, cont_high, cont_low );
}
inline void SCQUE_PUT_OP_DeleteDependPokeFieldEffect( SCQUE* que, u8 pokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_DEL_FLDEFF_DEPEND, pokeID );
}
inline void SCQUE_PUT_OP_RemoveFieldEffect( SCQUE* que, u8 eff )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_REMOVE_FLDEFF, eff );
}
inline void SCQUE_PUT_OP_SetPokeCounter( SCQUE* que, u8 pokeID, u8 counterID, u8 value )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_SET_POKE_COUNTER, pokeID, counterID, value );
}
inline void SCQUE_PUT_OP_IncKillCount( SCQUE* que, u8 pokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_INC_KILLCOUNT, pokeID );
}
inline void SCQUE_PUT_OP_BatonTouch( SCQUE* que, u8 userPokeID, u8 targetPokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_BATONTOUCH, userPokeID, targetPokeID );
}
inline void SCQUE_PUT_OP_MigawariCreate( SCQUE* que, u8 pokeID, u16 migawariHP )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_MIGAWARI_CREATE, pokeID, migawariHP );
}
inline void SCQUE_PUT_OP_MigawariDelete( SCQUE* que, u8 pokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_MIGAWARI_DELETE, pokeID );
}
inline void SCQUE_PUT_OP_SetIllusionForParty( SCQUE* que, u8 clientID )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_SET_ILLUSION, clientID );
}
inline void SCQUE_PUT_OP_ClearConsumedItem( SCQUE* que, u8 pokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_CLEAR_CONSUMED_ITEM, pokeID );
}
inline void SCQUE_PUT_OP_CureSickDependPoke( SCQUE* que, u8 pokeID, u8 dependPokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_CURESICK_DEPEND_POKE, pokeID, dependPokeID );
}

inline void SCQUE_OP_AddWazaDmgRec( SCQUE* que, u8 defPokeID, u8 atkPokeID, BtlPokePos pokePos, pml::PokeType wazaType, WazaDamageType damageType, u16 wazaID, u16 damage )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_WAZADMG_REC, defPokeID, atkPokeID, pokePos, wazaType, damageType, wazaID, damage );
}
inline void SCQUE_PUT_OP_TurnCheck( SCQUE* que, u8 pokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_TURN_CHECK, pokeID );
}
inline void SCQUE_PUT_OP_TurnCheckField( SCQUE* que )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_TURN_CHECK_FIELD, 0 );
}
inline void SCQUE_PUT_OP_Doryoku( SCQUE* que, u8 pokeID, u8 hp, u8 pow, u8 def, u8 agi, u8 sp_pow, u8 sp_def )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_SET_DORYOKU, pokeID, hp, pow, def, agi, sp_pow, sp_def );
}
inline void SCQUE_OP_StartPosEffect( SCQUE* que, BtlPosEffect effect, BtlPokePos pos, u32 effectParam )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_START_POSEFF, effect, pos, effectParam );
}
inline void SCQUE_OP_RemovePosEffect( SCQUE* que, BtlPosEffect effect, BtlPokePos pos )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_REMOVE_POSEFF, effect, pos );
}
inline void SCQUE_OP_UpdatePosEffectParam( SCQUE* que, BtlPosEffect effect, BtlPokePos pos, u32 effectParam )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_POSEFFECT_UPDATE_PARAM, effect, pos, effectParam );
}
inline void SCQUE_PUT_OP_PGLRecord( SCQUE* que, u8 defPokeID, u8 atkPokeID, u16 atkWazaNo )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_PGL_RECORD, defPokeID, atkPokeID, atkWazaNo );
}

inline void SCQUE_PUT_OP_SideEffect_Add( SCQUE* que, BtlSide side, BtlSideEffect sideEffect, const BTL_SICKCONT& contParam )
{
  u32 cont_high = 0;
  u32 cont_low  = 0;
  SICKCONT_Split32bit( contParam, &cont_high, &cont_low );
  scmd::SCQUE_PUT_Common( que, SC_OP_SIDEEFFECT_ADD, side, sideEffect, cont_high, cont_low );
}
inline void SCQUE_PUT_OP_SideEffect_Remove( SCQUE* que, BtlSide side, BtlSideEffect sideEffect )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_SIDEEFFECT_REMOVE, side, sideEffect );
}
inline void SCQUE_PUT_OP_SideEffect_IncTurnCount( SCQUE* que, BtlSide side, BtlSideEffect sideEffect )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_SIDEEFFECT_INC_TURNCOUNT, side, sideEffect );
}

inline void SCQUE_PUT_OP_PublishClientInformation_AppearedPokemon( SCQUE* que, u8 appeardPokeId )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_PUBLISH_CLIENT_INFO_APPEARED_POKEMON, appeardPokeId );
}
inline void SCQUE_PUT_OP_PublishClientInformation_HavePokemonItem( SCQUE* que, u8 pokeId, bool haveItem )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_PUBLISH_CLIENT_INFO_HAVE_POKEMON_ITEM, pokeId, static_cast<u8>( haveItem ) );
}

inline void SCQUE_PUT_OP_SetCurrentRoyalRanking( SCQUE* que, u8 rank_client0, u8 rank_client1, u8 rank_client2, u8 rank_client3 )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_SET_CURRENT_ROYAL_RANKING, rank_client0, rank_client1, rank_client2, rank_client3 );
}


//---------------------------------------------
inline u8 WAZAEFF_ARG( u8 arg, u8 bMegaEvo )
{
  return (bMegaEvo<<7) | (arg & 0x7f);
}
inline bool WAZAEFF_ARG_GetMegaEvoFlag( u8 arg )
{
  return (arg & 0x80) != 0;
}
inline u8 WAZAEFF_ARG_GetArg( u8 arg )
{
  return (arg & 0x7f);
}

inline void SCQUE_PUT_ACT_WazaEffect( SCQUE* que, u8 atPokePos, u8 defPokePos, u16 waza, u8 arg, bool bSyncDamageEffect, ZenryokuWazaKind zWazaKind )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_WAZA_EFFECT, atPokePos, defPokePos, waza, arg, bSyncDamageEffect, zWazaKind );
}
// 溜めワザ（そらをとぶ等）の開始時に姿を消す・攻撃後に姿を現す
inline void SCQUE_PUT_ACT_TameWazaHide( SCQUE* que, u8 pokeID, u8 hideFlag )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_TAMEWAZA_HIDE, pokeID, hideFlag );
}



// 【アクション】単体ダメージ処理
inline void SCQUE_PUT_ACT_WazaDamage( SCQUE* que, u8 defPokeID, u8 affAbout, u16 wazaID )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_WAZA_DMG, defPokeID, affAbout, wazaID );
}
// 【アクション】複数体同時ダメージ処理
inline void SCQUE_PUT_ACT_WazaDamagePlural( SCQUE* que, u8 pokeCnt, u8 affAbout, u16 wazaID )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_WAZA_DMG_PLURAL, pokeCnt, affAbout, wazaID );
}
// 【アクション】みがわりダメージ処理
inline void SCQUE_PUT_ACT_MigawariDamage( SCQUE* que, u8 defPokeID, BtlTypeAff affine, WazaID waza )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_MIGAWARI_DAMAGE, defPokeID, affine, waza );
}

// 【アクション】一撃必殺ワザ処理
inline void SCQUE_PUT_ACT_WazaIchigeki( SCQUE* que, u8 pokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_WAZA_ICHIGEKI, pokeID );
}

// 【アクション】ゲージに状態異常アイコンを表示（POKESICK_NULLで表示オフ）
inline void SCQUE_PUT_ACT_SickIcon( SCQUE* que, u8 pokeID, PokeSick sick )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_SICK_ICON, pokeID, sick );
}

// 【アクション】こんらん自爆ダメージ
inline void SCQUE_PUT_ACT_ConfDamage( SCQUE* que, u8 pokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_CONF_DMG, pokeID );
}
// 【アクション】能力ランクアップ
inline void SCQUE_PUT_ACT_RankUp( SCQUE* que, u8 pokeID, u8 statusType, u8 volume )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_RANKUP, pokeID, statusType, volume );
}
// 【アクション】能力ランクダウン
inline void SCQUE_PUT_ACT_RankDown( SCQUE* que, u8 pokeID, u8 statusType, u8 volume )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_RANKDOWN, pokeID, statusType, volume );
}
/**
 * @brief 【アクション】ポケモンひんし
 * @param pokeID              ひん死になったポケモンのID
 * @param isKillCountEffect 「相手を倒した数」加算演出を再生するか？
 */
inline void SCQUE_PUT_ACT_Dead( SCQUE* que, u8 pokeID, bool isKillCountEffect )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_DEAD, pokeID, isKillCountEffect );
}
// 【アクション】ポケモン生き返り
inline void SCQUE_PUT_ACT_RelivePoke( SCQUE* que, u8 pokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_RELIVE, pokeID );
}
// 【アクション】ポケモン退場メッセージ表示
inline void SCQUE_PUT_ACT_MemberOutMsg( SCQUE* que, u8 clientID, u8 pokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_MEMBER_OUT_MSG, clientID, pokeID );
}
// 【アクション】ポケモン退場
inline void SCQUE_PUT_ACT_MemberOut( SCQUE* que, BtlPokePos pos, u16 effectNo )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_MEMBER_OUT, pos, effectNo );
}
// 【アクション】ポケモン入場
inline void SCQUE_PUT_ACT_MemberIn( SCQUE* que, u8 clientID, u8 posIdx, u8 memberIdx, u8 fPutMsg )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_MEMBER_IN, clientID, posIdx, memberIdx, fPutMsg );
}
// メガ進化
inline void SCQUE_PUT_ACTOP_MegaEvo( SCQUE* que, u8 pokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_ACTOP_MEGAEVO, pokeID );
}

/**
 *  天候を開始する
 * @param weather      天候
 * @param turn         天候の継続ターン数
 * @param turnUpCount  天候の継続ターン数のうち、道具や特性などで増加したターン数
 * @param causePokeID  天候を変化させたポケモンのID
 */
inline void SCQUE_PUT_ACTOP_WeatherStart( SCQUE* que, u8 weather, u8 turn, u8 turnUpCount, u8 causePokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_ACTOP_WEATHER_START, weather, turn, turnUpCount, causePokeID );
}

inline void SCQUE_PUT_ACTOP_WeatherEnd( SCQUE* que, u8 weather )
{
  scmd::SCQUE_PUT_Common( que, SC_ACTOP_WEATHER_END, weather );
}

inline void SCQUE_PUT_ACT_SimpleHP( SCQUE* que, u8 pokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_SIMPLE_HP, pokeID );
}

inline void SCQUE_PUT_ACT_USE_ITEM( SCQUE* que, u8 pokeID, u8 bNuts )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_USE_ITEM, pokeID, bNuts );
}
// ワザエフェクト直前に木の実を食べる演出のため利用(pokeIDは BTL_POSIDX_MAX (１陣営あたりの最大ポケ数）分まで必要）
inline void SCQUE_PUT_ACT_KINOMI_PRE_WAZA( SCQUE* que, u8 pokeCnt, u8 pokeID_1, u8 pokeID_2, u8 pokeID_3, u8 pokeID_4, u8 pokeID_5 )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_KINOMI_PRE_WAZA, pokeCnt, pokeID_1, pokeID_2, pokeID_3, pokeID_4, pokeID_5 );
}


inline void SCQUE_PUT_ACT_Kill( SCQUE* que, u8 pokeID, u8 effectType )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_KILL, pokeID, effectType );
}
inline void SCQUE_PUT_ACTOP_MemberMove( SCQUE* que, u8 clientID, BtlPokePos pos1, BtlPokePos pos2 )
{
  scmd::SCQUE_PUT_Common( que, SC_ACTOP_MOVE, clientID, pos1, pos2 );
}
inline void SCQUE_PUT_ACT_AddExp( SCQUE* que, u8 pokeID, u32 exp )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_EXP, pokeID, exp );
}
inline void SCQUE_PUT_ACT_Naderu( SCQUE* que, u8 pokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_NADERU, pokeID );
}

// ボール投げる
inline void SCQUE_PUT_ACT_BallThrow( SCQUE* que, BtlPokePos pos, u8 yureCnt,
    u8 fSuccess, u8 fZukanRegister, u8 fCritical, u16 ballItemID )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_BALL_THROW, pos, yureCnt, fSuccess, fZukanRegister, fCritical, ballItemID );
}
// ボール投げ後、捕獲成功した場合の追加コマンド
inline void SCQUE_PUT_ACT_BallThrow_Captured( SCQUE* que, BtlPokePos pos, bool bZukanRegister )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_BALL_THROW_CAPTURED, pos, bZukanRegister );
}
// ボールを投げるがトレーナー戦なので起こられる
inline void SCQUE_PUT_ACT_BallThrowForbidden( SCQUE* que, BtlPokePos pos, u16 ballItemID )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_BALL_THROW_FORBIDDEN, pos, ballItemID );
}
inline void SCQUE_PUT_ACTOP_ChangeTokusei( SCQUE* que, u8 pokeID, u16 tokuseiID )
{
  scmd::SCQUE_PUT_Common( que, SC_ACTOP_CHANGE_TOKUSEI, pokeID, tokuseiID );
}
inline void SCQUE_PUT_ACTOP_SwapTokusei( SCQUE* que, u8 pokeID_1, u8 pokeID_2, u16 tokID_1, u16 tokID_2 )
{
  scmd::SCQUE_PUT_Common( que, SC_ACTOP_SWAP_TOKUSEI, pokeID_1, pokeID_2, tokID_1, tokID_2 );
}
inline void SCQUE_PUT_ACT_FakeDisable( SCQUE* que, u8 pokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_FAKE_DISABLE, pokeID );
}
inline void SCQUE_PUT_ACT_EffectSimple( SCQUE* que, u16 effectNo )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_EFFECT_SIMPLE, effectNo );
}
inline void SCQUE_PUT_ACT_EffectByPos( SCQUE* que, BtlPokePos pos, u16 effectNo )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_EFFECT_BYPOS, pos, effectNo );
}
inline void SCQUE_PUT_ACT_EffectByVector( SCQUE* que, BtlPokePos pos_from, BtlPokePos pos_to, u16 effectNo )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_EFFECT_BYVECTOR, pos_from, pos_to, effectNo );
}
inline void SCQUE_PUT_ACT_EffectField( SCQUE* que, BtlGround ground )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_EFFECT_FIELD, ground );
}

inline void SCQUE_PUT_ACT_ExPlural2ndHit( SCQUE* que, BtlPokePos pos, u16 effectNo )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_PLURALHIT_EX_2ND, pos, effectNo );
}

/**
 * @param dontResetFormByOut  true なら、退出時にデフォルトフォルムに戻さない
 */
inline void SCQUE_PUT_ACT_ChangeForm( SCQUE* que, u8 pokeID, u8 formNo, bool dontResetFormByOut )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_CHANGE_FORM, pokeID, formNo, dontResetFormByOut );
}
inline void SCQUE_PUT_ACT_MigawariCreate( SCQUE* que, BtlPokePos pos )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_MIGAWARI_CREATE, pos );
}
inline void SCQUE_PUT_ACT_MigawariDelete( SCQUE* que, BtlPokePos pos )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_MIGAWARI_DELETE, pos );
}
inline void SCQUE_PUT_ACT_Hensin( SCQUE* que, u8 atkPokeID, u8 tgtPokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_HENSIN, atkPokeID, tgtPokeID );
}
inline void SCQUE_PUT_ACT_PlayWinBGM( SCQUE* que, u32 bgmNo )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_WIN_BGM, bgmNo );
}
inline void SCQUE_PUT_ACT_MsgWinHide( SCQUE* que, u8 dmy )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_MSGWIN_HIDE, dmy );
}
inline void SCQUE_PUT_ACT_FriendshipEffect( SCQUE* que, u8 pokeID, FriendshipEffect eff )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_FRIENDSHIP_EFFECT, pokeID, eff  );
}
inline void SCQUE_PUT_ACT_FriendshipEffectWithMsg( SCQUE* que, u8 pokeID, FriendshipEffect eff, u8 msgType, u16 strID, u16 arg1, u16 arg2 )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_FRIENDSHIP_EFFECT_MSG, pokeID, eff, msgType, strID, arg1, arg2  );
}

inline void SCQUE_PUT_ACT_DeadPlural( SCQUE* que, u8 deadPokeCnt )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_DEAD_PLURAL, deadPokeCnt );
}

/**
 * @brief サーバーコマンド SC_OP_SET_ZENRYOKU_WAZA_USED
 *        ポケモンに汚れをセットする
 * @param que     コマンド追加先のキュー
 * @param pokeID  汚れをセットするポケモンのID
 * @param dirt    セットする汚れの種類
 */
inline void SCQUE_PUT_OP_SET_DIRT( SCQUE* que, u8 pokeID, DirtType dirt )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_SET_DIRT, pokeID, dirt );
}

/**
 * @brief サーバーコマンド SC_OP_INC_WEATHER_PASSED_TURN
 *        天候の経過ターン数をインクリメントする
 * @param que  コマンド追加先のキュー
 */
inline void SCQUE_PUT_OP_INC_WEATHER_PASSED_TURN( SCQUE* que )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_INC_WEATHER_PASSED_TURN, 0 );
}

/**
 * @brief サーバーコマンド SC_OP_SET_SPACT_PRIORITY
 *        ポケモンの行動の特殊優先度を設定する
 * @param que      コマンド追加先のキュー
 * @param pokeID   設定対象ポケモンのID
 * @param priority 設定する特殊優先度
 */
inline void SCQUE_PUT_OP_SET_SPACT_PRIORITY( SCQUE* que, u8 pokeID, u8 priority )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_SET_SPACT_PRIORITY, pokeID, priority );
}

/**
 * @brief サーバーコマンド SC_OP_SET_ZENRYOKU_WAZA_USED
 *        全力技を使用したことを記録する
 * @param que     コマンド追加先のキュー
 * @param pokeID  全力技を使用したポケモンのID
 */
inline void SCQUE_PUT_OP_SetZenryokuWazaUsed( SCQUE* que, u8 pokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_OP_SET_ZENRYOKU_WAZA_USED, pokeID );
}

// 全力技効果メッセージのパラメータ
struct ZenryokuEffectMessage
{
  u16 strId;    // 文字列ID( BTL_STRID_SET_xxxx )
  u8  param1;   // メッセージ引数1
  u8  param2;   // メッセージ引数2
  u8  param3;   // メッセージ引数3

  ZenryokuEffectMessage()
  {
    strId  = 0;
    param1 = 0;
    param2 = 0;
    param3 = 0;
  }
};

/**
 * @brief サーバーコマンド SC_ACT_ZENRYOKUWAZA_BEGIN
          全力技開始演出を実行する
 * @param que            コマンド追加先のキュー
 * @param pokeID         全力技を使用するポケモンのID
 * @param wazaNo         実行する全力技の技No.( ただし、実行する技が「ゆびをふる」等の派生技の場合は、派生技の技No. )
 * @param targetPos      ターゲットの立ち位置
 * @param effectMessage  全力効果発動時に表示するメッセージ
 */
inline void SCQUE_PUT_ACT_ZenryokuWazaBegin( SCQUE* que, u8 pokeID, WazaNo wazaNo, BtlPokePos targetPos, const ZenryokuEffectMessage& effectMessage )
{
  scmd::SCQUE_PUT_Common( 
    que, 
    SC_ACT_ZENRYOKUWAZA_BEGIN, 
    pokeID, 
    targetPos,
    effectMessage.param1,
    effectMessage.param2,
    effectMessage.param3,
    effectMessage.strId,
    static_cast<u16>( wazaNo ) );
}
inline void SCQUE_PUT_ACT_ZenryokuWazaEnd( SCQUE* que, u8 pokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_ZENRYOKUWAZA_END, pokeID );
}

inline void SCQUE_PUT_ACT_OteireButtonDisplay( SCQUE* que )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_OTEIRE_BUTTON_DISPLAY );
}

/**
 * @brief サーバーコマンド 乱入( 助けを呼ぶ演出 )
 * @param[out] que          コマンド追加先キュー
 * @param      callPokePos  助けを呼ぶポケモンの位置
 */
inline void SCQUE_PUT_ACT_IntrudeCall( SCQUE* que, BtlPokePos callPokePos )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_INTRUDE_CALL, static_cast<u8>( callPokePos ) );
}

/**
 * @brief サーバーコマンド 乱入( 乱入ポケモンの登場演出 )
 * @param[out] que             コマンド追加先キュー
 * @param      callPokePos     助けを呼んだポケモンの位置
 * @param      intrudePokePos  乱入ポケモンの位置
 */
inline void SCQUE_PUT_ACT_IntrudeAppear( SCQUE* que, BtlPokePos callPokePos, BtlPokePos intrudePokePos )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_INTRUDE_APPEAR, static_cast<u8>( callPokePos ), static_cast<u8>( intrudePokePos ) );
}

/**
 * @brief サーバーコマンド 乱入( 乱入ポケモンが来なかった場合の演出 )
 * @param[out] que          コマンド追加先キュー
 * @param      callPokePos  助けを呼んだポケモンの位置
 */
inline void SCQUE_PUT_ACT_IntrudeNotAppeared( SCQUE* que, BtlPokePos callPokePos )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_INTRUDE_NOT_APPEARD, static_cast<u8>( callPokePos ) );
}
/**
 * @brief サーバーコマンド ターン開始時の演出
 * @param[out] que  コマンド追加先キュー
 */
inline void SCQUE_PUT_ACT_TurnStart( SCQUE* que )
{
  scmd::SCQUE_PUT_Common( que, SC_ACT_TURN_START, 0 );
}



inline void SCQUE_PUT_TOKWIN_IN( SCQUE* que, u8 pokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_TOKWIN_IN, pokeID );
}
inline void SCQUE_PUT_TOKWIN_OUT( SCQUE* que, u8 pokeID )
{
  scmd::SCQUE_PUT_Common( que, SC_TOKWIN_OUT, pokeID );
}

/**
 * @brief 技メッセージを表示する
 * @param que             コマンド追加先のキュー
 * @param pokeID          技を使用するポケモンのID
 * @param waza            使用する技の技No.
 * @param isZenryokuWaza  使用する技が全力技か？
 */
inline void SCQUE_PUT_MSG_WAZA( SCQUE* que, u8 pokeID, u16 waza, bool isZenryokuWaza )
{
  scmd::SCQUE_PUT_Common( que, SC_MSG_WAZA, pokeID, isZenryokuWaza, waza );
}

//=====================================================
typedef u32 ScMsgArg;
enum {
  MSGARG_TERMINATOR = 0xffff0000,
};

#include <stdarg.h>

// ↓ 可変部分の最初の引数には必ず文字列IDを渡す。
#define SCQUE_PUT_MSG_STD(que, ...) scmd::SCQUE_PUT_MsgImpl( que, SC_MSG_STD, __VA_ARGS__, MSGARG_TERMINATOR )
#define SCQUE_PUT_MSG_SET(que, ...) scmd::SCQUE_PUT_MsgImpl( que, SC_MSG_SET, __VA_ARGS__, MSGARG_TERMINATOR )
#define SCQUE_PUT_MSG_STD_SE(que, ...) scmd::SCQUE_PUT_MsgImpl( que, SC_MSG_STD_SE, __VA_ARGS__, MSGARG_TERMINATOR )
#define SCQUE_PUT_MSG_SET_SE(que, ...) scmd::SCQUE_PUT_MsgImpl( que, SC_MSG_SET_SE, __VA_ARGS__, MSGARG_TERMINATOR )

//=====================================================

GFL_NAMESPACE_END(btl)
#endif // __BTL_SERVER_CMD_H__

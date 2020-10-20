/**
 * @file BattleAiCommand.h
 * @brief 戦闘AIスクリプトに提供するコマンド
 * @author obata_toshihiro
 * @date 2015.03.30
 */
#ifndef BATTLE_AI_COMMAND_H_INCLUDED
#define BATTLE_AI_COMMAND_H_INCLUDED

#include <pawn/include/gfl2_Pawn.h>
#include "../btl_pokeparam.h"
#include "../btl_common.h"


GFL_NAMESPACE_BEGIN( btl )

class ServerFlow;
class BTL_CLIENT;
class AiScriptCommandHandler;


/**
 * @class BattleAiCommand
 * @brief 戦闘AIスクリプトに提供するコマンドを定義するクラス
 */
class BattleAiCommand
{
  GFL_FORBID_COPY_AND_ASSIGN( BattleAiCommand );

public:
  static cell AMX_NATIVE_CALL AI_CMD( AMX*, const cell* args );

private: 
  static cell CMDFUNC_IF_RND_UNDER( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_RND_OVER( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_RND_EQUAL( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IFN_RND_EQUAL( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_HP_UNDER( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_HP_OVER( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_HP_EQUAL( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IFN_HP_EQUAL( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_POKESICK( AiScriptCommandHandler* handle, const cell* args );    // 何らかの状態異常になっていたら true
  static cell CMDFUNC_IFN_POKESICK( AiScriptCommandHandler* handle, const cell* args );   // 何も状態異常になっていなかったら true
  static cell CMDFUNC_IF_WAZASICK( AiScriptCommandHandler* handle, const cell* args );    // 指定された状態異常になっていたら true
  static cell CMDFUNC_IFN_WAZASICK( AiScriptCommandHandler* handle, const cell* args );   // 指定された状態異常になっていなかったら true
  static cell CMDFUNC_IF_DOKUDOKU( AiScriptCommandHandler* handle, const cell* args );    // どくどくになっていたら true
  static cell CMDFUNC_IFN_DOKUDOKU( AiScriptCommandHandler* handle, const cell* args );    // どくどくになっていなかったら true
  static cell CMDFUNC_IF_CONTFLG( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IFN_CONTFLG( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_SIDEEFF( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IFN_SIDEEFF( AiScriptCommandHandler* handle, const cell* args );
  static bool check_have_damage_waza( AiScriptCommandHandler* handle, const BTL_POKEPARAM* bpp );
  static cell CMDFUNC_IF_HAVE_DAMAGE_WAZA( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IFN_HAVE_DAMAGE_WAZA( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_TURN( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_TYPE( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_WAZA_USABLE( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_DAMAGE_WAZA( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_IRYOKU( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_COMP_POWER( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_LAST_WAZA( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_FIRST( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_BENCH_COUNT( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_WAZASEQNO( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_TOKUSEI( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_WAZA_AISYOU( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_GET_WAZA_AISYOU( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_HAVE_WAZA_AISYOU_OVER( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_HAVE_WAZA_AISYOU_EQUAL( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_WAZA_NO_EFFECT_BY_TOKUSEI( AiScriptCommandHandler* handle, const cell* args );
  static bool check_pokesick_in_bench( AiScriptCommandHandler* handle, int ai_side );
  static cell CMDFUNC_IF_BENCH_COND( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IFN_BENCH_COND( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_WEATHER( AiScriptCommandHandler* handle, const cell* args );
  static int get_poke_param( AiScriptCommandHandler* handle, int ai_side, BTL_POKEPARAM::ValueID  valueID );
  static cell CMDFUNC_IF_PARA_UNDER( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_PARA_OVER( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_PARA_EQUAL( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IFN_PARA_EQUAL( AiScriptCommandHandler* handle, const cell* args );
  static bool check_current_waza_hinsi( AiScriptCommandHandler* handle, int loss_flag );
  static cell CMDFUNC_IF_WAZA_HINSHI( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IFN_WAZA_HINSHI( AiScriptCommandHandler* handle, const cell* args );
  static bool check_have_waza( AiScriptCommandHandler* handle, int ai_side, WazaNo waza_no );
  static cell CMDFUNC_IF_HAVE_WAZA( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IFN_HAVE_WAZA( AiScriptCommandHandler* handle, const cell* args );
  static bool check_have_waza_seqno( AiScriptCommandHandler* handle, int ai_side, int seq_no );
  static cell CMDFUNC_IF_HAVE_WAZA_SEQNO( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IFN_HAVE_WAZA_SEQNO( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_ESCAPE( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_SOUBI_ITEM( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_SOUBI_EQUIP( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_POKESEX( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_NEKODAMASI( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_TAKUWAERU( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_BTL_RULE( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_BTL_COMPETITOR( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_RECYCLE_ITEM( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_WORKWAZA_TYPE( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_WORKWAZA_POW( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_WORKWAZA_SEQNO( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_MAMORU_COUNT( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_LEVEL( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_CHOUHATSU( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IFN_CHOUHATSU( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_MIKATA_ATTACK( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_HAVE_TYPE( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_HAVE_TOKUSEI( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_ALREADY_MORAIBI( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_HAVE_ITEM( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_FLDEFF_CHECK( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_SIDEEFF_COUNT( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_BENCH_HPDEC( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_BENCH_PPDEC( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_NAGETSUKERU_IRYOKU( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_PP_REMAIN( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_TOTTEOKI( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_WAZA_KIND( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_LAST_WAZA_KIND( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_AGI_RANK( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_SLOWSTART_TURN( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_BENCH_DAMAGE_MAX( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_HAVE_BATSUGUN( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_LAST_WAZA_DAMAGE_CHECK( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_STATUS_UP( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_STATUS_DIFF( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_STATUS( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_COMP_POWER_WITH_PARTNER( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_HINSHI( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IFN_HINSHI( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_GET_TOKUSEI( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_MIGAWARI( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_MONSNO( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_CHECK_FORMNO( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_COMMONRND_UNDER( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_COMMONRND_OVER( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_COMMONRND_EQUAL( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IFN_COMMONRND_EQUAL( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_IF_MIRAIYOCHI( AiScriptCommandHandler* handle, const cell* args );
  static  cell  CMDFUNC_IF_DMG_PHYSIC_UNDER( AiScriptCommandHandler* handle, const cell* args );
  static  cell  CMDFUNC_IF_DMG_PHYSIC_OVER( AiScriptCommandHandler* handle, const cell* args );
  static  cell  CMDFUNC_IF_DMG_PHYSIC_EQUAL( AiScriptCommandHandler* handle, const cell* args );
  static  cell  CMDFUNC_IF_ATE_KINOMI( AiScriptCommandHandler* handle, const cell* args );
  static  cell  CMDFUNC_IF_TYPE_EX( AiScriptCommandHandler* handle, const cell* args );
  static  cell  CMDFUNC_IF_EXIST_GROUND( AiScriptCommandHandler* handle, const cell* args );
  static  cell  CMDFUNC_GET_WEIGHT( AiScriptCommandHandler* handle, const cell* args );
  static  cell  CMDFUNC_IF_MULTI( AiScriptCommandHandler* handle, const cell* args );
  static  cell  CMDFUNC_IF_MEGAEVOLVED( AiScriptCommandHandler* handle, const cell* args );
  static  cell  CMDFUNC_IF_WAZAHIDE( AiScriptCommandHandler* handle, const cell* args );
  static  cell  CMDFUNC_GET_MEZAPA_TYPE( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_GET_HOROBINOUTA_TURN_MAX( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_GET_HOROBINOUTA_TURN_NOW( AiScriptCommandHandler* handle, const cell* args );
  static cell CMDFUNC_GET_KODAWARI_WAZA( AiScriptCommandHandler* handle, const cell* args );
  static  cell  CMDFUNC_IF_CAN_MEGAEVOLVE( AiScriptCommandHandler* handle, const cell* args );
  static  cell  CMDFUNC_IF_I_AM_SENARIO_TRAINER( AiScriptCommandHandler* handle, const cell* args );
  static  cell  CMDFUNC_GET_MAX_WAZA_POWER_INCLUDE_AFFINITY( AiScriptCommandHandler* handle, const cell* args );
  static  cell  CMDFUNC_GET_LAST_DAMAGED_WAZA_AT_PREV_TURN( AiScriptCommandHandler* handle, const cell* args );
  static  cell  CMDFUNC_GET_CURRENT_WAZANO( AiScriptCommandHandler* handle, const cell* args );
  static  cell  CMDFUNC_GET_CURRENT_ITEMNO( AiScriptCommandHandler* handle, const cell* args );
  static  cell  CMDFUNC_IF_ZIDANDA_POWERUP( AiScriptCommandHandler* handle, const cell* args );
  static  cell  CMDFUNC_GET_BATTLEROYAL_RANKING( AiScriptCommandHandler* handle, const cell* args );
  static  cell  CMDFUNC_GET_CLIENT_KILL_COUNT( AiScriptCommandHandler* handle, const cell* args );
  static  cell  CMDFUNC_GET_WAZA_TARGET( AiScriptCommandHandler* handle, const cell* args );
  // momiji新規追加
  static cell CMDFUNC_IF_HAVE_BATSUGUN_CAN_BENCH( AiScriptCommandHandler* handle, const cell* args );

  static u32 GetMaxWazaPowerIncludeAffinity( ServerFlow* serverFlow, const BTL_POKEPARAM* attackPoke, const BTL_POKEPARAM* defencePoke );
  static BtlTypeAff CalcTypeAffinity( ServerFlow* serverFlow, const BTL_POKEPARAM* attackPoke, const BTL_POKEPARAM* defensePoke, WazaNo wazano );
  static BtlTypeAff CalcTypeAffinityCanBench( ServerFlow* serverFlow, const BTL_POKEPARAM* attackPoke, const BTL_POKEPARAM* defensePoke, WazaNo wazano );
  static bool IsWazaUsable( const BTL_CLIENT* attackClient, const BTL_POKEPARAM* attackPoke, WazaNo wazano );
  static TokuseiNo CheckPokeTokusei( const MainModule& mainModule, u8 myClientId, const BTL_POKEPARAM* targetPoke );
};


GFL_NAMESPACE_END( btl )


#endif // BATTLE_AI_COMMAND_H_INCLUDED

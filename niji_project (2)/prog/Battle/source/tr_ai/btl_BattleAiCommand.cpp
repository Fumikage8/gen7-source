/**
 * @file BattleAiCommand.cpp
 * @brief 戦闘AIスクリプトに提供するコマンド
 * @author obata_toshihiro
 * @date 2015.03.30
 */
#include "./tr_ai_cmd.h"
#include "../btl_pokeparam.h"
#include "../btl_SideEff.h"
#include "../btl_ServerFlow.h"
#include "../btl_RoyalRankingContainer.h"
#include "./btl_BattleAiSystem.h"
#include "./btl_BattleAiCommand.h"
#include "./btl_AiScriptCommandHandler.h"

GFL_NAMESPACE_BEGIN( btl )

typedef cell (*BTL_AICMD_FUNC)( AiScriptCommandHandler*, const cell* );


//-----------------------------------------------------------------------------
/**
 * @brief 戦闘AIコマンドを実行する
 * @param args[1]      スクリプトにアタッチされている AiScriptCommandHandler
 * @param args[2]      実行するコマンドのID( AICmd )
 * @param args[3]以降  実行するコマンドの引数
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL BattleAiCommand::AI_CMD( AMX*, const cell* args )
{
  static const BTL_AICMD_FUNC CmdTbl[ NUM_AI_CMD ] = 
  {
    /* CMD_IF_RND_UNDER                         */ BattleAiCommand::CMDFUNC_IF_RND_UNDER,
    /* CMD_IF_RND_OVER                          */ BattleAiCommand::CMDFUNC_IF_RND_OVER,
    /* CMD_IF_RND_EQUAL                         */ BattleAiCommand::CMDFUNC_IF_RND_EQUAL,
    /* CMD_IFN_RND_EQUAL                        */ BattleAiCommand::CMDFUNC_IFN_RND_EQUAL,
    /* CMD_IF_HP_UNDER                          */ BattleAiCommand::CMDFUNC_IF_HP_UNDER,
    /* CMD_IF_HP_OVER                           */ BattleAiCommand::CMDFUNC_IF_HP_OVER,
    /* CMD_IF_HP_EQUAL                          */ BattleAiCommand::CMDFUNC_IF_HP_EQUAL,
    /* CMD_IFN_HP_EQUAL                         */ BattleAiCommand::CMDFUNC_IFN_HP_EQUAL,
    /* CMD_IF_POKESICK                          */ BattleAiCommand::CMDFUNC_IF_POKESICK,
    /* CMD_IFN_POKESICK                         */ BattleAiCommand::CMDFUNC_IFN_POKESICK,
    /* CMD_IF_WAZASICK                          */ BattleAiCommand::CMDFUNC_IF_WAZASICK,
    /* CMD_IFN_WAZASICK                         */ BattleAiCommand::CMDFUNC_IFN_WAZASICK,
    /* CMD_IF_DOKUDOKU                          */ BattleAiCommand::CMDFUNC_IF_DOKUDOKU,
    /* CMD_IFN_DOKUDOKU                         */ BattleAiCommand::CMDFUNC_IFN_DOKUDOKU,
    /* CMD_IF_CONTFLG                           */ BattleAiCommand::CMDFUNC_IF_CONTFLG,
    /* CMD_IFN_CONTFLG                          */ BattleAiCommand::CMDFUNC_IFN_CONTFLG,
    /* CMD_IF_SIDEEFF                           */ BattleAiCommand::CMDFUNC_IF_SIDEEFF,
    /* CMD_IFN_SIDEEFF                          */ BattleAiCommand::CMDFUNC_IFN_SIDEEFF,
    /* CMD_GET_HOROBINOUTA_TURN_MAX             */ BattleAiCommand::CMDFUNC_GET_HOROBINOUTA_TURN_MAX,
    /* CMD_GET_HOROBINOUTA_TURN_NOW             */ BattleAiCommand::CMDFUNC_GET_HOROBINOUTA_TURN_NOW,
    /* CMD_GET_KODAWARI_WAZA                    */ BattleAiCommand::CMDFUNC_GET_KODAWARI_WAZA,
    /* CMD_IF_HAVE_DAMAGE_WAZA                  */ BattleAiCommand::CMDFUNC_IF_HAVE_DAMAGE_WAZA,
    /* CMD_IFN_HAVE_DAMAGE_WAZA                 */ BattleAiCommand::CMDFUNC_IFN_HAVE_DAMAGE_WAZA,
    /* CMD_CHECK_TURN                           */ BattleAiCommand::CMDFUNC_CHECK_TURN,
    /* CMD_CHECK_TYPE                           */ BattleAiCommand::CMDFUNC_CHECK_TYPE,
    /* CMD_CHECK_WAZA_USABLE                    */ BattleAiCommand::CMDFUNC_CHECK_WAZA_USABLE,
    /* CMD_CHECK_DAMAGE_WAZA                    */ BattleAiCommand::CMDFUNC_CHECK_DAMAGE_WAZA,
    /* CMD_CHECK_IRYOKU                         */ BattleAiCommand::CMDFUNC_CHECK_IRYOKU,
    /* CMD_COMP_POWER                           */ BattleAiCommand::CMDFUNC_COMP_POWER,
    /* CMD_CHECK_LAST_WAZA                      */ BattleAiCommand::CMDFUNC_CHECK_LAST_WAZA,
    /* CMD_IF_FIRST                             */ BattleAiCommand::CMDFUNC_IF_FIRST,
    /* CMD_CHECK_BENCH_COUNT                    */ BattleAiCommand::CMDFUNC_CHECK_BENCH_COUNT,
    /* CMD_CHECK_WAZASEQNO                      */ BattleAiCommand::CMDFUNC_CHECK_WAZASEQNO,
    /* CMD_CHECK_TOKUSEI                        */ BattleAiCommand::CMDFUNC_CHECK_TOKUSEI,
    /* CMD_CHECK_WAZA_AISYOU                    */ BattleAiCommand::CMDFUNC_CHECK_WAZA_AISYOU,
    /* CMD_GET_WAZA_AISYOU                      */ BattleAiCommand::CMDFUNC_GET_WAZA_AISYOU,
    /* CMD_IF_HAVE_WAZA_AISYOU_OVER             */ BattleAiCommand::CMDFUNC_IF_HAVE_WAZA_AISYOU_OVER,
    /* CMD_IF_HAVE_WAZA_AISYOU_EQUAL            */ BattleAiCommand::CMDFUNC_IF_HAVE_WAZA_AISYOU_EQUAL,
    /* CMD_IF_BENCH_COND                        */ BattleAiCommand::CMDFUNC_IF_BENCH_COND,
    /* CMD_IFN_BENCH_COND                       */ BattleAiCommand::CMDFUNC_IFN_BENCH_COND,
    /* CMD_CHECK_WEATHER                        */ BattleAiCommand::CMDFUNC_CHECK_WEATHER,
    /* CMD_IF_PARA_UNDER                        */ BattleAiCommand::CMDFUNC_IF_PARA_UNDER,
    /* CMD_IF_PARA_OVER                         */ BattleAiCommand::CMDFUNC_IF_PARA_OVER,
    /* CMD_IF_PARA_EQUAL                        */ BattleAiCommand::CMDFUNC_IF_PARA_EQUAL,
    /* CMD_IFN_PARA_EQUAL                       */ BattleAiCommand::CMDFUNC_IFN_PARA_EQUAL,
    /* CMD_IF_WAZA_HINSHI                       */ BattleAiCommand::CMDFUNC_IF_WAZA_HINSHI,
    /* CMD_IFN_WAZA_HINSHI                      */ BattleAiCommand::CMDFUNC_IFN_WAZA_HINSHI,
    /* CMD_IF_HAVE_WAZA                         */ BattleAiCommand::CMDFUNC_IF_HAVE_WAZA,
    /* CMD_IFN_HAVE_WAZA                        */ BattleAiCommand::CMDFUNC_IFN_HAVE_WAZA,
    /* CMD_IF_HAVE_WAZA_SEQNO                   */ BattleAiCommand::CMDFUNC_IF_HAVE_WAZA_SEQNO,
    /* CMD_IFN_HAVE_WAZA_SEQNO                  */ BattleAiCommand::CMDFUNC_IFN_HAVE_WAZA_SEQNO,
    /* CMD_ESCAPE                               */ BattleAiCommand::CMDFUNC_ESCAPE,
    /* CMD_CHECK_SOUBI_ITEM                     */ BattleAiCommand::CMDFUNC_CHECK_SOUBI_ITEM,
    /* CMD_CHECK_SOUBI_EQUIP                    */ BattleAiCommand::CMDFUNC_CHECK_SOUBI_EQUIP,
    /* CMD_CHECK_POKESEX                        */ BattleAiCommand::CMDFUNC_CHECK_POKESEX,
    /* CMD_CHECK_NEKODAMASI                     */ BattleAiCommand::CMDFUNC_CHECK_NEKODAMASI,
    /* CMD_CHECK_TAKUWAERU                      */ BattleAiCommand::CMDFUNC_CHECK_TAKUWAERU,
    /* CMD_CHECK_BTL_RULE                       */ BattleAiCommand::CMDFUNC_CHECK_BTL_RULE,
    /* CMD_CHECK_BTL_COMPETITOR                 */ BattleAiCommand::CMDFUNC_CHECK_BTL_COMPETITOR,
    /* CMD_CHECK_RECYCLE_ITEM                   */ BattleAiCommand::CMDFUNC_CHECK_RECYCLE_ITEM,
    /* CMD_CHECK_WORKWAZA_TYPE                  */ BattleAiCommand::CMDFUNC_CHECK_WORKWAZA_TYPE,
    /* CMD_CHECK_WORKWAZA_POW                   */ BattleAiCommand::CMDFUNC_CHECK_WORKWAZA_POW,
    /* CMD_CHECK_WORKWAZA_SEQNO                 */ BattleAiCommand::CMDFUNC_CHECK_WORKWAZA_SEQNO,
    /* CMD_CHECK_MAMORU_COUNT                   */ BattleAiCommand::CMDFUNC_CHECK_MAMORU_COUNT,
    /* CMD_IF_LEVEL                             */ BattleAiCommand::CMDFUNC_IF_LEVEL,
    /* CMD_IF_CHOUHATSU                         */ BattleAiCommand::CMDFUNC_IF_CHOUHATSU,
    /* CMD_IFN_CHOUHATSU                        */ BattleAiCommand::CMDFUNC_IFN_CHOUHATSU,
    /* CMD_IF_MIKATA_ATTACK                     */ BattleAiCommand::CMDFUNC_IF_MIKATA_ATTACK,
    /* CMD_CHECK_HAVE_TYPE                      */ BattleAiCommand::CMDFUNC_CHECK_HAVE_TYPE,
    /* CMD_CHECK_HAVE_TOKUSEI                   */ BattleAiCommand::CMDFUNC_CHECK_HAVE_TOKUSEI,
    /* CMD_IF_ALREADY_MORAIBI                   */ BattleAiCommand::CMDFUNC_IF_ALREADY_MORAIBI,
    /* CMD_IF_HAVE_ITEM                         */ BattleAiCommand::CMDFUNC_IF_HAVE_ITEM,
    /* CMD_FLDEFF_CHECK                         */ BattleAiCommand::CMDFUNC_FLDEFF_CHECK,
    /* CMD_CHECK_SIDEEFF_COUNT                  */ BattleAiCommand::CMDFUNC_CHECK_SIDEEFF_COUNT,
    /* CMD_IF_BENCH_HPDEC                       */ BattleAiCommand::CMDFUNC_IF_BENCH_HPDEC,
    /* CMD_IF_BENCH_PPDEC                       */ BattleAiCommand::CMDFUNC_IF_BENCH_PPDEC,
    /* CMD_CHECK_NAGETSUKERU_IRYOKU             */ BattleAiCommand::CMDFUNC_CHECK_NAGETSUKERU_IRYOKU,
    /* CMD_CHECK_PP_REMAIN                      */ BattleAiCommand::CMDFUNC_CHECK_PP_REMAIN,
    /* CMD_IF_TOTTEOKI                          */ BattleAiCommand::CMDFUNC_IF_TOTTEOKI,
    /* CMD_CHECK_WAZA_KIND                      */ BattleAiCommand::CMDFUNC_CHECK_WAZA_KIND,
    /* CMD_CHECK_LAST_WAZA_KIND                 */ BattleAiCommand::CMDFUNC_CHECK_LAST_WAZA_KIND,
    /* CMD_CHECK_AGI_RANK                       */ BattleAiCommand::CMDFUNC_CHECK_AGI_RANK,
    /* CMD_CHECK_SLOWSTART_TURN                 */ BattleAiCommand::CMDFUNC_CHECK_SLOWSTART_TURN,
    /* CMD_IF_BENCH_DAMAGE_MAX                  */ BattleAiCommand::CMDFUNC_IF_BENCH_DAMAGE_MAX,
    /* CMD_IF_HAVE_BATSUGUN                     */ BattleAiCommand::CMDFUNC_IF_HAVE_BATSUGUN,
    /* CMD_IF_LAST_WAZA_DAMAGE_CHECK            */ BattleAiCommand::CMDFUNC_IF_LAST_WAZA_DAMAGE_CHECK,
    /* CMD_CHECK_STATUS_UP                      */ BattleAiCommand::CMDFUNC_CHECK_STATUS_UP,
    /* CMD_CHECK_STATUS_DIFF                    */ BattleAiCommand::CMDFUNC_CHECK_STATUS_DIFF,
    /* CMD_CHECK_STATUS                         */ BattleAiCommand::CMDFUNC_CHECK_STATUS,
    /* CMD_COMP_POWER_WITH_PARTNER              */ BattleAiCommand::CMDFUNC_COMP_POWER_WITH_PARTNER,
    /* CMD_IF_HINSHI                            */ BattleAiCommand::CMDFUNC_IF_HINSHI,
    /* CMD_IFN_HINSHI                           */ BattleAiCommand::CMDFUNC_IFN_HINSHI,
    /* CMD_GET_TOKUSEI                          */ BattleAiCommand::CMDFUNC_GET_TOKUSEI,
    /* CMD_IF_MIGAWARI                          */ BattleAiCommand::CMDFUNC_IF_MIGAWARI,
    /* CMD_CHECK_MONSNO                         */ BattleAiCommand::CMDFUNC_CHECK_MONSNO,
    /* CMD_CHECK_FORMNO                         */ BattleAiCommand::CMDFUNC_CHECK_FORMNO,
    /* CMD_IF_COMMONRND_UNDER                   */ BattleAiCommand::CMDFUNC_IF_COMMONRND_UNDER,
    /* CMD_IF_COMMONRND_OVER                    */ BattleAiCommand::CMDFUNC_IF_COMMONRND_OVER,
    /* CMD_IF_COMMONRND_EQUAL                   */ BattleAiCommand::CMDFUNC_IF_COMMONRND_EQUAL,
    /* CMD_IFN_COMMONRND_EQUAL                  */ BattleAiCommand::CMDFUNC_IFN_COMMONRND_EQUAL,
    /* CMD_IF_MIRAIYOCHI                        */ BattleAiCommand::CMDFUNC_IF_MIRAIYOCHI,
    /* CMD_IF_DMG_PHYSIC_UNDER                  */ BattleAiCommand::CMDFUNC_IF_DMG_PHYSIC_UNDER,
    /* CMD_IF_DMG_PHYSIC_OVER                   */ BattleAiCommand::CMDFUNC_IF_DMG_PHYSIC_OVER,
    /* CMD_IF_DMG_PHYSIC_EQUAL                  */ BattleAiCommand::CMDFUNC_IF_DMG_PHYSIC_EQUAL,
    /* CMD_IF_ATE_KINOMI                        */ BattleAiCommand::CMDFUNC_IF_ATE_KINOMI,
    /* CMD_IF_TYPE_EX                           */ BattleAiCommand::CMDFUNC_IF_TYPE_EX,
    /* CMD_IF_EXIST_GROUND                      */ BattleAiCommand::CMDFUNC_IF_EXIST_GROUND,
    /* CMD_GET_WEIGHT                           */ BattleAiCommand::CMDFUNC_GET_WEIGHT,
    /* CMD_IF_MULTI                             */ BattleAiCommand::CMDFUNC_IF_MULTI,
    /* CMD_IF_MEGAEVOLVED                       */ BattleAiCommand::CMDFUNC_IF_MEGAEVOLVED,
    /* CMD_IF_CAN_MEGAEVOLVE                    */ BattleAiCommand::CMDFUNC_IF_CAN_MEGAEVOLVE,
    /* CMD_IF_WAZAHIDE                          */ BattleAiCommand::CMDFUNC_IF_WAZAHIDE,
    /* CMD_GET_MEZAME_TYPE                      */ BattleAiCommand::CMDFUNC_GET_MEZAPA_TYPE,
    /* CMD_IF_I_AM_SENARIO_TRAINER              */ BattleAiCommand::CMDFUNC_IF_I_AM_SENARIO_TRAINER,
    /* CMD_GET_MAX_WAZA_POWER_INCLUDE_AFFINITY  */ BattleAiCommand::CMDFUNC_GET_MAX_WAZA_POWER_INCLUDE_AFFINITY,
    /* CMD_CHECK_WAZA_NO_EFFECT_BY_TOKUSEI      */ BattleAiCommand::CMDFUNC_CHECK_WAZA_NO_EFFECT_BY_TOKUSEI,
    /* CMD_GET_LAST_DAMAGED_WAZA_AT_PREV_TURN   */ BattleAiCommand::CMDFUNC_GET_LAST_DAMAGED_WAZA_AT_PREV_TURN,
    /* CMD_GET_CURRENT_WAZANO                   */ BattleAiCommand::CMDFUNC_GET_CURRENT_WAZANO,
    /* CMD_GET_CURRENT_ITEMNO                   */ BattleAiCommand::CMDFUNC_GET_CURRENT_ITEMNO,
    /* CMD_IF_ZIDANDA_POWERUP                   */ BattleAiCommand::CMDFUNC_IF_ZIDANDA_POWERUP,
    /* CMD_GET_BATTLEROYAL_RANKING              */ BattleAiCommand::CMDFUNC_GET_BATTLEROYAL_RANKING,
    /* CMD_GET_CLIENT_KILL_COUNT                */ BattleAiCommand::CMDFUNC_GET_CLIENT_KILL_COUNT,
    /* CMD_GET_WAZA_TARGET                      */ BattleAiCommand::CMDFUNC_GET_WAZA_TARGET,
    /* CMD_IF_HAVE_BATSUGUN_CAN_BENCH           */ BattleAiCommand::CMDFUNC_IF_HAVE_BATSUGUN_CAN_BENCH,
  };

  AiScriptCommandHandler* handle = (AiScriptCommandHandler*)(args[1]);
  AICmd  cmd    =  (AICmd)(args[2]);
  args += 3;

//  BTL_PRINT("TR_AI.C AI_CMD cmd[%d]よばれた\n", cmd);
  if( cmd < GFL_NELEMS(CmdTbl) )
  {
    if( CmdTbl[ cmd ] != NULL ){
      return CmdTbl[ cmd ]( handle, args );
    }
    else{
      BTL_PRINT("  >> NULL です\n");
    }
  }

  GFL_ASSERT_MSG(0, "CmdNo=%d", cmd);

  return 0;
}





//-----------------------------------------------------------------------------
//  ランダム分岐
//-----------------------------------------------------------------------------

/**
 * @brief ランダム分岐
 * @param handle   スクリプト実行中のAI
 * @param args[0]  分岐の閾値
 * @return  0〜255の乱数を抽選し、args[0] より小さいなら true
 *          そうでなければ false
 */
cell BattleAiCommand::CMDFUNC_IF_RND_UNDER( AiScriptCommandHandler* handle, const cell* args )
{
  AIRandSys* randGenerator = handle->GetRandGenerator();
  if( randGenerator->Next( BattleAiSystem::BASIC_RAND_RANGE ) < static_cast<u32>(args[0]) ){
    return true;
  }
  return false;
}

/**
 * @brief ランダム分岐
 * @param handle   スクリプト実行中のAI
 * @param args[0]  分岐の閾値
 * @return  0〜255の乱数を抽選し、args[0] より大きいなら true
 *          そうでなければ false
 */
cell BattleAiCommand::CMDFUNC_IF_RND_OVER( AiScriptCommandHandler* handle, const cell* args )
{
  AIRandSys* randGenerator = handle->GetRandGenerator();
  if( randGenerator->Next(BattleAiSystem::BASIC_RAND_RANGE) > static_cast<u32>(args[0]) ){
    return true;
  }
  return false;
}

/**
 * @brief ランダム分岐
 * @param handle   スクリプト実行中のAI
 * @param args[0]  分岐の閾値
 * @return  0〜255の乱数を抽選し、args[0] と同じなら true
 *          そうでなければ false
 */
cell BattleAiCommand::CMDFUNC_IF_RND_EQUAL( AiScriptCommandHandler* handle, const cell* args )
{
  AIRandSys* randGenerator = handle->GetRandGenerator();
  u32 rand = randGenerator->Next( BattleAiSystem::BASIC_RAND_RANGE );
  if( static_cast<s32>( rand ) == args[0] ){
    return true;
  }
  return false;
}

/**
 * @brief ランダム分岐
 * @param handle   スクリプト実行中のAI
 * @param args[0]  分岐の閾値
 * @return  0〜255の乱数を抽選し、args[0] と異なるなら true
 *          そうでなければ false
 */
cell BattleAiCommand::CMDFUNC_IFN_RND_EQUAL( AiScriptCommandHandler* handle, const cell* args )
{
  AIRandSys* randGenerator = handle->GetRandGenerator();
  u32 rand = randGenerator->Next( BattleAiSystem::BASIC_RAND_RANGE );
  if( static_cast<s32>( rand ) != args[0] ){
    return true;
  }
  return false;
}



//-----------------------------------------------------------------------------
//  HPでの分岐
//-----------------------------------------------------------------------------

/**
 * @brief HPでの分岐
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[1]  分岐の閾値[%]
 * @return 指定したポケモンのHPが args[1] より小さいなら true
 */
cell BattleAiCommand::CMDFUNC_IF_HP_UNDER( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  int hp_ratio = bpp->GetHPRatio() >> FX32_SHIFT;

  return (hp_ratio < args[1]);
}

/**
 * @brief HPでの分岐
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[1]  分岐の閾値[%]
 * @return 指定したポケモンのHPが args[1] より大きいなら true
 */
cell BattleAiCommand::CMDFUNC_IF_HP_OVER( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  int hp_ratio = bpp->GetHPRatio() >> FX32_SHIFT;

  return (hp_ratio > args[1]);
}

/**
 * @brief HPでの分岐
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[1]  分岐の閾値[%]
 * @return 指定したポケモンのHPが args[1] と等しいなら true
 */
cell BattleAiCommand::CMDFUNC_IF_HP_EQUAL( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
 int ratio_src = bpp->GetHPRatio();
 int ratio = (ratio_src >> FX32_SHIFT) + ((ratio_src & FX32_DEC_MASK) != 0);

  return (ratio == args[1]);
}

/**
 * @brief HPでの分岐
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[1]  分岐の閾値[%]
 * @return 指定したポケモンのHPが args[1] と等しくないなら true
 */
cell BattleAiCommand::CMDFUNC_IFN_HP_EQUAL( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
 int ratio_src = bpp->GetHPRatio();
 int ratio = (ratio_src >> FX32_SHIFT) + ((ratio_src & FX32_DEC_MASK) != 0);

  return (ratio != args[1]);
}



//-----------------------------------------------------------------------------
//  ポケモンの状態異常チェック
//-----------------------------------------------------------------------------

/**
 * @brief ポケモンが何らかの状態異常にかかっているかチェック
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @return 指定したポケモンが、何らかの状態異常になっていたら true
 */
cell BattleAiCommand::CMDFUNC_IF_POKESICK( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  return bpp->GetPokeSick() != pml::pokepara::SICK_NULL;
}

/**
 * @brief ポケモンが何らかの状態異常にかかっているかチェック
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @return 指定したポケモンが、何も状態異常になっていなかったら true
 */
cell BattleAiCommand::CMDFUNC_IFN_POKESICK( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  return bpp->GetPokeSick() == pml::pokepara::SICK_NULL;
}

/**
 * @brief ポケモンが何らかの状態異常にかかっているかチェック
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[1]  チェック対象の状態異常
 * @return 指定したポケモンが、指定された状態異常になっていたら true
 */
cell BattleAiCommand::CMDFUNC_IF_WAZASICK( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  WazaSick sick = (WazaSick)(args[1]);
  return bpp->CheckSick( sick );
}

/**
 * @brief ポケモンが何らかの状態異常にかかっているかチェック
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[1]  チェック対象の状態異常
 * @return 指定したポケモンが、指定された状態異常になっていなかったら true
 */
cell BattleAiCommand::CMDFUNC_IFN_WAZASICK( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  WazaSick sick = (WazaSick)(args[1]);
  return !(bpp->CheckSick( sick ));
}

/**
 * @brief ポケモンが何らかの状態異常にかかっているかチェック
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @return 指定したポケモンが、どくどくになっていたら true
 */
cell BattleAiCommand::CMDFUNC_IF_DOKUDOKU( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  return bpp->CheckMoudoku();
}

/**
 * @brief ポケモンが何らかの状態異常にかかっているかチェック
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @return 指定したポケモンが、どくどくになっていなかったら true
 */
cell BattleAiCommand::CMDFUNC_IFN_DOKUDOKU( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  return !(bpp->CheckMoudoku());
}

/**
 * @brief ポケモンの「ほろびのうた」状態の発動ターン数を取得する
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @retval 0  指定したポケモンが「ほろびのうた」状態でない場合
 */
cell BattleAiCommand::CMDFUNC_GET_HOROBINOUTA_TURN_MAX( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  if( !( bpp->CheckSick( pml::wazadata::WAZASICK_HOROBINOUTA ) ) ) {
    return 0;
  }

  BTL_SICKCONT cont = bpp->GetSickCont( pml::wazadata::WAZASICK_HOROBINOUTA );
  u8 turnMax = SICCONT_GetTurnMax( cont );
  return turnMax;
}

/**
 * @brief ポケモンの「ほろびのうた」状態の現在のターン数を取得する
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @retval 0  指定したポケモンが「ほろびのうた」状態でない場合
 */
cell BattleAiCommand::CMDFUNC_GET_HOROBINOUTA_TURN_NOW( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  if( !( bpp->CheckSick( pml::wazadata::WAZASICK_HOROBINOUTA ) ) ) {
    return 0;
  }

  u8 turnNow = bpp->GetSickTurnCount( pml::wazadata::WAZASICK_HOROBINOUTA );
  return turnNow;
}


/**
 * @brief ポケモンが「こだわっている」技を取得する
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @return こだわっている技の技No.
 * @retval WAZANO_NULL  こだわっている技が存在しない場合
 */
cell BattleAiCommand::CMDFUNC_GET_KODAWARI_WAZA( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );

  if( bpp == NULL ) {
    GFL_ASSERT(0);
    return WAZANO_NULL;
  }

  if( !( bpp->CheckSick( pml::wazadata::WAZASICK_KODAWARI ) ) ) {
    return WAZANO_NULL;
  }

  BTL_SICKCONT sickCont = bpp->GetSickCont( pml::wazadata::WAZASICK_KODAWARI );
  WazaNo  kodawariWaza = static_cast<WazaNo>( SICKCONT_GetParam( sickCont ) );
  return kodawariWaza;
}




//-----------------------------------------------------------------------------
//  ポケモン状態継続フラグ
//-----------------------------------------------------------------------------

/**
 * @brief ポケモン状態継続フラグ
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[1]  チェック対象の状態継続フラグ
 * @return 指定したポケモンに、指定された状態継続フラグがセットされているなら true
 */
cell BattleAiCommand::CMDFUNC_IF_CONTFLG( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  BTL_POKEPARAM::ContFlag flag = (BTL_POKEPARAM::ContFlag)( args[1] );

  return bpp->CONTFLAG_Get( flag );
}

/**
 * @brief ポケモン状態継続フラグ
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[1]  チェック対象の状態継続フラグ
 * @return 指定したポケモンに、指定された状態継続フラグがセットされていないなら true
 */
cell BattleAiCommand::CMDFUNC_IFN_CONTFLG( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  BTL_POKEPARAM::ContFlag flag = (BTL_POKEPARAM::ContFlag)( args[1] );

  return !(bpp->CONTFLAG_Get( flag ));
}



//-----------------------------------------------------------------------------
//  天候
//-----------------------------------------------------------------------------
/**
 * @brief 天候を取得する
 */
cell BattleAiCommand::CMDFUNC_CHECK_WEATHER( AiScriptCommandHandler* handle, const cell* args )
{
  return handle->GetServerFlow()->Hnd_GetWeather();
}



//-----------------------------------------------------------------------------
//  陣営に対する状態フラグ
//-----------------------------------------------------------------------------

/**
 * @brief 陣営に対する状態フラグ
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[1]  チェック対象の陣営状態
 * @return 指定したポケモンの陣営が、指定された状態なら true
 */
cell BattleAiCommand::CMDFUNC_IF_SIDEEFF( AiScriptCommandHandler* handle, const cell* args )
{
  BtlPokePos    pos = handle->AISideToPokePos( args[0] );
  BtlSideEffect eff = (BtlSideEffect)(args[1]);

  return handle->GetServerFlow()->Hnd_GetSideEffectCount(pos, eff) != 0;
}

/**
 * @brief 陣営に対する状態フラグ
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[1]  チェック対象の陣営状態
 * @return 指定したポケモンの陣営が、指定された状態でないなら true
 */
cell BattleAiCommand::CMDFUNC_IFN_SIDEEFF( AiScriptCommandHandler* handle, const cell* args )
{
  BtlPokePos    pos = handle->AISideToPokePos( args[0] );
  BtlSideEffect eff = (BtlSideEffect)(args[1]);

  return handle->GetServerFlow()->Hnd_GetSideEffectCount(pos, eff) == 0;
}



//-----------------------------------------------------------------------------
//  攻撃ポケ ダメージワザ所持チェック
//-----------------------------------------------------------------------------

/**
 * @brief 攻撃ポケ ダメージワザ所持チェック
 * @param handle   スクリプト実行中のAI
 * @param bpp      チェック対象のポケモン
 * @return 指定したポケモンがダメージ技を持っているなら true
 */
bool BattleAiCommand::check_have_damage_waza( AiScriptCommandHandler* handle, const BTL_POKEPARAM* bpp )
{
  if( bpp == NULL ) {
    return false;
  }

  u32 wazaCnt = bpp->WAZA_GetCount();
  for(u32 i=0; i<wazaCnt; ++i)
  {
    if( WAZADATA_GetParam(bpp->WAZA_GetID(i), pml::wazadata::PARAM_ID_POWER) ){
      return true;
    }
  }
  return false;
}

/**
 * @brief 攻撃ポケ ダメージワザ所持チェック
 * @param handle   スクリプト実行中のAI
 * @return 攻撃するポケモンがダメージ技を持っているなら true
 */
cell BattleAiCommand::CMDFUNC_IF_HAVE_DAMAGE_WAZA( AiScriptCommandHandler* handle, const cell* args )
{
  return check_have_damage_waza( handle, handle->GetAttackPokeParam() );
}

/**
 * @brief 攻撃ポケ ダメージワザ所持チェック
 * @param handle   スクリプト実行中のAI
 * @return 攻撃するポケモンがダメージ技を持っていないなら true
 */
cell BattleAiCommand::CMDFUNC_IFN_HAVE_DAMAGE_WAZA( AiScriptCommandHandler* handle, const cell* args )
{
  return !check_have_damage_waza( handle, handle->GetAttackPokeParam() );
}



//-----------------------------------------------------------------------------
//  ターン数取得
//-----------------------------------------------------------------------------

/**
 * @brief 現在の経過ターン数を取得する
 * @param handle   スクリプト実行中のAI
 */
cell BattleAiCommand::CMDFUNC_CHECK_TURN( AiScriptCommandHandler* handle, const cell* args )
{
  return handle->GetServerFlow()->Hnd_GetTurnCount();
}



//-----------------------------------------------------------------------------
//  ポケモンまたはワザのタイプを取得
//-----------------------------------------------------------------------------

/**
 * @brief ポケモンまたはワザのタイプを取得する
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象( CHECK_DEFENCE_TYPE1, CHECK_ATTACK_TYPE1, CHECK_WAZA, ... )
 * @param 指定したチェック対象のタイプ
 */
cell BattleAiCommand::CMDFUNC_CHECK_TYPE( AiScriptCommandHandler* handle, const cell* args )
{
  int check_ptn = args[0];

  const BTL_POKEPARAM* atk_poke = handle->GetAttackPokeParam();
  const BTL_POKEPARAM* def_poke = handle->GetDefensePokeParam();

  PokeTypePair  atk_type = ( atk_poke == NULL ) ? ( 0 ) : ( atk_poke->GetPokeType() );
  PokeTypePair  def_type = ( def_poke == NULL ) ? ( 0 ) : ( def_poke->GetPokeType() );

  switch( check_ptn ) {
  case CHECK_DEFENCE_TYPE1:   return PokeTypePair_GetType1( def_type );
  case CHECK_ATTACK_TYPE1:    return PokeTypePair_GetType1( atk_type );
  case CHECK_DEFENCE_TYPE2:   return PokeTypePair_GetType2( def_type );
  case CHECK_ATTACK_TYPE2:    return PokeTypePair_GetType2( atk_type );
  case CHECK_WAZA:            return WAZADATA_GetParam( handle->GetCurrentWazaNo(), pml::wazadata::PARAM_ID_TYPE );

  case CHECK_DEFENCE_FRIEND_TYPE1:
  case CHECK_DEFENCE_FRIEND_TYPE2:
  {
    const BTL_POKEPARAM* bpp = handle->GetBppByAISide( CHECK_DEFENCE_FRIEND );
    PokeTypePair  type_pair  = bpp->GetPokeType();
    if( check_ptn == CHECK_DEFENCE_FRIEND_TYPE1 ){
      return PokeTypePair_GetType1( type_pair );
    }else{
      return PokeTypePair_GetType2( type_pair );
    }
  }

  case CHECK_ATTACK_FRIEND_TYPE1:
  case CHECK_ATTACK_FRIEND_TYPE2:
  {
    const BTL_POKEPARAM* bpp = handle->GetBppByAISide( CHECK_ATTACK_FRIEND );
    PokeTypePair  type_pair  = bpp->GetPokeType();
    if( check_ptn == CHECK_ATTACK_FRIEND_TYPE1 ){
      return PokeTypePair_GetType1( type_pair );
    }else{
      return PokeTypePair_GetType2( type_pair );
    }
  }


  default:
    GFL_ASSERT(0);
    return 0;
  }
}



//-----------------------------------------------------------------------------
//  採点中の技
//-----------------------------------------------------------------------------

/**
 * @brief 指定した技を使用可能か？
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[1]  チェック対象の技No.
 * @retval true   使用可能
 * @retval false  
 */
cell BattleAiCommand::CMDFUNC_CHECK_WAZA_USABLE( AiScriptCommandHandler* handle, const cell* args )
{
  u8 attackClientId = handle->AISideToClientID( args[0] );
  const BTL_CLIENT* attackClient = handle->GetMainModule()->GetClient( attackClientId );
  const BTL_POKEPARAM* attackPoke = handle->GetBppByAISide( args[0] );
  WazaNo wazano = static_cast<WazaNo>( args[1] );
  return IsWazaUsable( attackClient, attackPoke, wazano );
}

/**
 * @brief 指定した技を使用可能か？
 * @param attackClient   技を使用するポケモンのクライアント
 * @param attackPoke     技を使用するポケモン
 * @param wazano         使用する技
 * @retval true   使用できる
 * @retval false  使用できない
 */
bool BattleAiCommand::IsWazaUsable( 
  const BTL_CLIENT* attackClient, 
  const BTL_POKEPARAM* attackPoke,
  WazaNo wazano )
{
  // 覚えていない
  if( !( attackPoke->WAZA_IsUsable( wazano ) ) ) {
    return false;
  }

  // 残りPPがゼロ
  if( attackPoke->WAZA_GetPP_ByNumber( wazano ) <= 0 ) {
    return false;
  }

  // 使用できない
  if( attackClient->IsUnselectableWaza( attackPoke, wazano ) ) {
    return false;
  }

  return true;
}

/**
 * @brief 採点中の技が、ダメージ技かどうかチェックする
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象の技No.
 * @return 採点中の技が、ダメージ技なら true
 */
cell BattleAiCommand::CMDFUNC_CHECK_DAMAGE_WAZA( AiScriptCommandHandler* handle, const cell* args )
{
  WazaNo wazano = static_cast<WazaNo>( args[0] );
  return WAZADATA_IsDamage( wazano );
}

/**
 * @brief 採点中の技の威力を取得する
 * @param handle   スクリプト実行中のAI
 */
cell BattleAiCommand::CMDFUNC_CHECK_IRYOKU( AiScriptCommandHandler* handle, const cell* args )
{
  return WAZADATA_GetPower( handle->GetCurrentWazaNo() );
}

/**
 * @brief 採点中の技が、最も威力が高い技かどうかチェックする
 * @param handle   スクリプト実行中のAI
 * @param args[0]  ダメージロス計算のブレの有無( LOSS_CALC_OFF, LOSS_CALC_ON )
 * @retval COMP_POWER_NONE  ダメージ計算しない技の場合 
 * @retval COMP_POWER_NOTOP ダメージ量がトップじゃない
 * @retval COMP_POWER_TOP   ダメージ量がトップ
 */
cell BattleAiCommand::CMDFUNC_COMP_POWER( AiScriptCommandHandler* handle, const cell* args )
{
  bool loss_flag = ( args[0] != LOSS_CALC_OFF );

  const BTL_POKEPARAM* atk_poke = handle->GetAttackPokeParam();
  const BTL_POKEPARAM* def_poke = handle->GetDefensePokeParam();

  if( ( atk_poke == NULL ) ||
      ( def_poke == NULL ) ) {
    GFL_ASSERT(0);
    return false;
  }

  u8  atkPokeID = atk_poke->GetID();
  u8  defPokeID = def_poke->GetID();

  BTL_PRINT("[NATIVE] COMP POWER currentWazaNo=%d, loss_flag=%d\n", handle->GetCurrentWazaNo(), loss_flag);

  u32 src_dmg = handle->GetServerFlow()->Hnd_SimulationDamage( atkPokeID, defPokeID, handle->GetCurrentWazaNo(), true, loss_flag );

  if( src_dmg == 0 )
  {
    return COMP_POWER_NONE;
  }
  else
  {
    cell result = COMP_POWER_TOP;
    u32  wazaCnt = atk_poke->WAZA_GetCount();
    u32  dmg;

    for(u32 i=0 ; i<wazaCnt; ++i )
    {
      WazaNo waza_no = atk_poke->WAZA_GetID( i );
      if( i == handle->GetCurrentWazaIndex() ) continue;

      dmg = handle->GetServerFlow()->Hnd_SimulationDamage( atkPokeID, defPokeID, waza_no, true, loss_flag );
      if( dmg > src_dmg )
      {
        result = COMP_POWER_NOTOP;
        break;
      }
    }

    return result;
  }
}

/**
 * @brief 採点中の技の技No.を取得する
 * @param handle   スクリプト実行中のAI
 */
cell BattleAiCommand::CMDFUNC_GET_CURRENT_WAZANO( AiScriptCommandHandler* handle, const cell* args )
{
  return handle->GetCurrentWazaNo();
}

/**
 * @brief 採点中の技のシーケンスナンバーを返す
 * @param handle   スクリプト実行中のAI
 */
cell BattleAiCommand::CMDFUNC_CHECK_WAZASEQNO( AiScriptCommandHandler* handle, const cell* args )
{
  return WAZADATA_GetParam( handle->GetCurrentWazaNo(), pml::wazadata::PARAM_ID_AI_SEQNO );
}

/**
 * @brief 技を撃ったときの相性をチェックする
 * @param handle   スクリプト実行中のAI
 * @param args[0]  攻撃側ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[1]  防御側ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[2]  使用する技No.
 * @param args[3]  チェック対象のタイプ相性( AISYOU_xxxx )
 * @return タイプ相性が、指定した相性と等しいなら true
 *         ただし、TYPEAFF_1_4 を指定した場合、タイプ相性が TYPEAFF_1_4 以下なら true を返す
 *                 TYPEAFF_4   を指定した場合、タイプ相性が TYPEAFF_4   以上なら true を返す
 */
cell BattleAiCommand::CMDFUNC_CHECK_WAZA_AISYOU( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* atk_poke = handle->GetBppByAISide( args[0] );
  const BTL_POKEPARAM* def_poke = handle->GetBppByAISide( args[1] );
  WazaNo wazano = static_cast<WazaNo>( args[2] );
  BtlTypeAff checkAffinity  = static_cast<BtlTypeAff>( args[3] );

  if( ( atk_poke == NULL ) ||
      ( def_poke == NULL ) ) {
    GFL_ASSERT(0);
    return false;
  }

  BtlTypeAff calcAffinity = CalcTypeAffinity( handle->GetServerFlow(), atk_poke, def_poke, wazano );
  if( calcAffinity == pml::battle::TypeAffinity::TYPEAFF_NULL ) {
    return false;
  }

  switch( checkAffinity ){
    case pml::battle::TypeAffinity::TYPEAFF_1_4: return (calcAffinity <= pml::battle::TypeAffinity::TYPEAFF_1_4);
    case pml::battle::TypeAffinity::TYPEAFF_4:   return (calcAffinity >= pml::battle::TypeAffinity::TYPEAFF_4);
    default:
      return ( calcAffinity == checkAffinity );
  }
}

/**
 * @brief 技を撃ったときの相性を取得する（ポケモンチェンジAI専用）
 * @param handle   スクリプト実行中のAI
 * @param args[0]  攻撃側ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[1]  防御側ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[2]  使用する技No.
 * @return 技を撃ったときの相性( AISYOU_xxxx )
 */
cell BattleAiCommand::CMDFUNC_GET_WAZA_AISYOU( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* atk_poke = handle->GetBppByAISide( args[0] );
  const BTL_POKEPARAM* def_poke = handle->GetBppByAISide( args[1] );
  WazaNo wazano = static_cast<WazaNo>( args[2] );

  if( ( atk_poke == NULL ) ||
      ( def_poke == NULL ) ) {
    GFL_ASSERT(0);
    return AISYOU_1BAI;
  }

  BtlTypeAff calcAffinity = CalcTypeAffinityCanBench( handle->GetServerFlow(), atk_poke, def_poke, wazano );
  if( calcAffinity == pml::battle::TypeAffinity::TYPEAFF_NULL ) {
    return AISYOU_1BAI;
  }

  return calcAffinity;
}

/**
 * @brief 指定した相性より良い相性の技を持っているか？（ポケモンチェンジAI専用）
 * @param handle   スクリプト実行中のAI
 * @param args[0]  攻撃側ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[1]  防御側ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[2]  チェック対象の相性( AISYOU_0BAI, AISYOU_1_64BAI, ... )
 * @return 攻撃側ポケモンが、防御側ポケモンに対して、指定した相性よりも良い相性の技を持っているなら true
 */
cell BattleAiCommand::CMDFUNC_IF_HAVE_WAZA_AISYOU_OVER( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* atk_poke = handle->GetBppByAISide( args[0] );
  const BTL_POKEPARAM* def_poke = handle->GetBppByAISide( args[1] );
  const BtlTypeAff affThreshold = static_cast<BtlTypeAff>( args[2] );

  if( ( atk_poke == NULL ) ||
      ( def_poke == NULL ) ) {
    GFL_ASSERT(0);
    return false;
  }

  u8 attackClientId = handle->AISideToClientID( args[0] );
  const BTL_CLIENT* attackClient = handle->GetMainModule()->GetClient( attackClientId );

  u32 wazaCount = atk_poke->WAZA_GetCount();
  for( u32 wazaIndex=0; wazaIndex<wazaCount; ++wazaIndex )
  {
    WazaNo wazano = atk_poke->WAZA_GetID( wazaIndex );
    if( !( IsWazaUsable( attackClient, atk_poke, wazano ) ) ) {
      continue;
    }

    BtlTypeAff aff = CalcTypeAffinityCanBench( handle->GetServerFlow(), atk_poke, def_poke, wazano );

    if( aff == pml::battle::TypeAffinity::TYPEAFF_NULL ) {
      continue;
    }

    if( affThreshold < aff ) {
      return true;
    }
  }
  return false;
}

/**
 * @brief 指定した相性の技を持っているか？（ポケモンチェンジAI専用）
 * @param handle   スクリプト実行中のAI
 * @param args[0]  攻撃側ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[1]  防御側ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[1]  チェック対象の相性( AISYOU_0BAI, AISYOU_1_64BAI, ... )
 * @return 攻撃側ポケモンが、防御側ポケモンに対して、指定した相性の技を持っているなら true
 */
cell BattleAiCommand::CMDFUNC_IF_HAVE_WAZA_AISYOU_EQUAL( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* atk_poke = handle->GetBppByAISide( args[0] );
  const BTL_POKEPARAM* def_poke = handle->GetBppByAISide( args[1] );
  const BtlTypeAff affThreshold = static_cast<BtlTypeAff>( args[2] );

  if( ( atk_poke == NULL ) ||
      ( def_poke == NULL ) ) {
    GFL_ASSERT(0);
    return false;
  }

  u8 attackClientId = handle->AISideToClientID( args[0] );
  const BTL_CLIENT* attackClient = handle->GetMainModule()->GetClient( attackClientId );

  u32 waza_count = atk_poke->WAZA_GetCount();
  for( u32 wazaIndex=0; wazaIndex<waza_count; ++wazaIndex )
  {
    WazaNo wazano = atk_poke->WAZA_GetID( wazaIndex );
    if( !( IsWazaUsable( attackClient, atk_poke, wazano ) ) ) {
      continue;
    }

    BtlTypeAff aff = CalcTypeAffinityCanBench( handle->GetServerFlow(), atk_poke, def_poke, wazano );

    if( aff == pml::battle::TypeAffinity::TYPEAFF_NULL ) {
      continue;
    }

    if( affThreshold == aff ) {
      return true;
    }
  }
  return false;
}

/**
 * @brief 技の相性を計算する
 * @param serverFlow   計算に使用する ServerFlow
 * @param attackPoke   攻撃側ポケモン
 * @param defensePoke  防御側ポケモン
 * @param wazano       攻撃側ポケモンが使用する技
 * @retval pml::battle::TypeAffinity::TYPEAFF_NULL  指定した技が非ダメージ技である場合
 */
BtlTypeAff BattleAiCommand::CalcTypeAffinity( 
  ServerFlow* serverFlow,
  const BTL_POKEPARAM* attackPoke, 
  const BTL_POKEPARAM* defensePoke,
  WazaNo wazano )
{
  if( ( serverFlow == NULL ) ||
      ( attackPoke == NULL ) ||
      ( defensePoke == NULL ) ) {
    GFL_ASSERT(0);
    return pml::battle::TypeAffinity::TYPEAFF_NULL;
  }

  // 非ダメージ技
  if( !WAZADATA_IsDamage( wazano ) ) {
    return pml::battle::TypeAffinity::TYPEAFF_NULL;
  }

  return serverFlow->Hnd_SimulationAffinity( attackPoke->GetID(), defensePoke->GetID(), wazano );
}

/**
 * @brief 技の相性を計算する（控えポケモンが攻撃側でも正確版 ポケモンチェンジAI専用）
 * @param serverFlow   計算に使用する ServerFlow
 * @param attackPoke   攻撃側ポケモン
 * @param defensePoke  防御側ポケモン
 * @param wazano       攻撃側ポケモンが使用する技
 * @retval pml::battle::TypeAffinity::TYPEAFF_NULL  指定した技が非ダメージ技である場合
 */
BtlTypeAff BattleAiCommand::CalcTypeAffinityCanBench( 
  ServerFlow* serverFlow,
  const BTL_POKEPARAM* attackPoke, 
  const BTL_POKEPARAM* defensePoke,
  WazaNo wazano )
{
  if( ( serverFlow == NULL ) ||
      ( attackPoke == NULL ) ||
      ( defensePoke == NULL ) ) {
    GFL_ASSERT(0);
    return pml::battle::TypeAffinity::TYPEAFF_NULL;
  }

  // 非ダメージ技
  if( !WAZADATA_IsDamage( wazano ) ) {
    return pml::battle::TypeAffinity::TYPEAFF_NULL;
  }

  return serverFlow->Hnd_SimulationAffinityCanBench( attackPoke->GetID(), defensePoke->GetID(), wazano );
}

/**
 * @brief 防御側ポケモンの特性により、技を無効化できるかチェックする
 * @param handle   スクリプト実行中のAI
 * @param args[0]  防御側ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[1]  チェックする技番号
 * @return 指定した技が、防御ポケモンの特性により無効化されるなら true
 */
cell BattleAiCommand::CMDFUNC_CHECK_WAZA_NO_EFFECT_BY_TOKUSEI( AiScriptCommandHandler* handle, const cell* args )
{
  enum {
    CHECK_TOK_MAX = 4,  // チェックするとくせい最大数
  };

  static const struct 
  {
    u8   wazaType;
    u16  tokusei[ CHECK_TOK_MAX ];
  }
  CheckTable[] = {
    { POKETYPE_MIZU,  { TOKUSEI_TYOSUI,   TOKUSEI_YOBIMIZU,   TOKUSEI_KANSOUHADA, TOKUSEI_NULL } },
    { POKETYPE_DENKI, { TOKUSEI_TIKUDEN,  TOKUSEI_DENKIENZIN, TOKUSEI_HIRAISIN,   TOKUSEI_NULL } },
    { POKETYPE_KUSA,  { TOKUSEI_SOUSYOKU, TOKUSEI_NULL,       TOKUSEI_NULL,       TOKUSEI_NULL } },
    { POKETYPE_HONOO, { TOKUSEI_MORAIBI,  TOKUSEI_NULL,       TOKUSEI_NULL,       TOKUSEI_NULL } },
    { POKETYPE_JIMEN, { TOKUSEI_HUYUU,    TOKUSEI_NULL,       TOKUSEI_NULL,       TOKUSEI_NULL } },
  };

  const BTL_POKEPARAM* def_poke = handle->GetBppByAISide( args[0] );
  const WazaNo wazaNo = static_cast<WazaNo>( args[1] );
  const pml::PokeType wazaType = WAZADATA_GetType( wazaNo );

  const u8 myClientId = handle->AISideToClientID( CHECK_ATTACK );
  const MainModule* mainModule = handle->GetMainModule();
  const TokuseiNo tokusei = CheckPokeTokusei( *mainModule, myClientId, def_poke );

  for( u32 i=0; i<GFL_NELEMS(CheckTable); ++i )
  {
    if( CheckTable[i].wazaType != wazaType ){
      continue;
    }
    
    for( u32 t=0; t<CHECK_TOK_MAX; ++t ) 
    {
      if( CheckTable[i].tokusei[t] == TOKUSEI_NULL ) {
        break; 
      }
      if( CheckTable[i].tokusei[t] == tokusei ){
        return true;
      }
    }
  }
  return false;
}

/**
 * @brief 指定ポケモンの特性チェック
 *        味方なら、そのままの特性を返す。
 *        敵なら、既に判明している場合にのみ、正しい特性を返す。
 * @param myClientId  自身のクライアントID
 * @param targetPoke  チェック対象のポケモン
 * @return 指定ポケモンの特性
 * @retval TOKUSEI_NULL  指定ポケモンが相手側で、特性が未判明の場合
 */
TokuseiNo BattleAiCommand::CheckPokeTokusei( const MainModule& mainModule, u8 myClientId, const BTL_POKEPARAM* targetPoke )
{
  TokuseiNo tokusei = static_cast<TokuseiNo>( targetPoke->GetValue(BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE) );
  u8 poke_clientID = MainModule::PokeIDtoClientID( targetPoke->GetID() );

  if( mainModule.IsFriendClientID( myClientId, poke_clientID ) ){
    return tokusei;
  }

  if( BattleAiSystem::IsTokuseiOpened( targetPoke->GetID() ) ){
    return tokusei;
  }

  return TOKUSEI_NULL;
}


/**
 * @brief 採点中の技でダメージ計算して、防御側が瀕死になるかチェック
 * @param handle   スクリプト実行中のAI
 * @param args[0]  ダメージロス計算のブレの有無( LOSS_CALC_OFF, LOSS_CALC_ON )
 * @retval true   防御側が瀕死になる
 * @retval false  防御側が瀕死にならない
 */
bool BattleAiCommand::check_current_waza_hinsi( AiScriptCommandHandler* handle, int loss_flag )
{
  const BTL_POKEPARAM* atk_poke = handle->GetAttackPokeParam();
  const BTL_POKEPARAM* def_poke = handle->GetDefensePokeParam();

  if( ( atk_poke == NULL ) ||
      ( def_poke == NULL ) ) {
    GFL_ASSERT(0);
    return false;
  }

  int hp = handle->GetDefensePokeParam()->GetValue( BTL_POKEPARAM::BPP_HP );
  int damage = handle->GetServerFlow()->Hnd_SimulationDamage( atk_poke->GetID(), def_poke->GetID(), handle->GetCurrentWazaNo(), true, false );
  return (damage >= hp );
}

/**
 * @brief 採点中の技でダメージ計算して、防御側が瀕死になるかチェック
 * @param handle   スクリプト実行中のAI
 * @param args[0]  ダメージロス計算のブレの有無( LOSS_CALC_OFF, LOSS_CALC_ON )
 * @retval true   防御側が瀕死になる
 * @retval false  防御側が瀕死にならない
 */
cell BattleAiCommand::CMDFUNC_IF_WAZA_HINSHI( AiScriptCommandHandler* handle, const cell* args )
{
  return check_current_waza_hinsi( handle, args[0] );
}

/**
 * @brief 採点中の技でダメージ計算して、防御側が瀕死になるかチェック
 * @param handle   スクリプト実行中のAI
 * @param args[0]  ダメージロス計算のブレの有無( LOSS_CALC_OFF, LOSS_CALC_ON )
 * @retval true   防御側が瀕死にならない
 * @retval false  防御側が瀕死になる
 */
cell BattleAiCommand::CMDFUNC_IFN_WAZA_HINSHI( AiScriptCommandHandler* handle, const cell* args )
{
  return !check_current_waza_hinsi( handle, args[0] );
}



//-----------------------------------------------------------------------------
//  指定ポケモンについて 〜
//-----------------------------------------------------------------------------

/**
 * @brief 指定ポケモンが最後に出したワザを取得する
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 */
cell BattleAiCommand::CMDFUNC_CHECK_LAST_WAZA( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  return bpp->GetPrevWazaID();
}

/**
 * @brief 指定ポケモンのとくせいを返す
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 */
cell BattleAiCommand::CMDFUNC_CHECK_TOKUSEI( AiScriptCommandHandler* handle, const cell* args )
{
  return handle->CheckTokuseiByAISide( args[0] );
}

/**
 * @brief 特定のワザを持っているかチェック
 * @param handle   スクリプト実行中のAI
 * @param ai_side  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param waza_no  チェック対象の技No.
 * @return 指定したポケモンが味方サイドなら、指定した技を持っているなら true
 *         指定したポケモンが相手サイドなら、指定した技を今までに使用したことがあるなら true
 */
bool BattleAiCommand::check_have_waza( AiScriptCommandHandler* handle, int ai_side, WazaNo waza_no )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( ai_side );

  if( !bpp->IsDead() )
  {
    // 攻撃側=ポケモンパラメータを全探索する
    if( (ai_side == CHECK_ATTACK) || (ai_side == CHECK_ATTACK_FRIEND) )
    {
      u32 waza_cnt = bpp->WAZA_GetCount();
      for(u32 i=0; i<waza_cnt; ++i)
      {
        WazaNo have_waza = bpp->WAZA_GetID( i );
        if( have_waza == waza_no ){
          return true;
        }
      }
    }
    // 防御側->今までに使ったワザから探索する
    else
    {
      return handle->CheckWazaStored( bpp, waza_no );
    }
  }
  return false;
}

/**
 * @brief 特定のワザを持っているかチェック
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[1]  チェック対象の技No.
 * @return 指定したポケモンが味方サイドなら、指定した技を持っているなら true
 *         指定したポケモンが相手サイドなら、指定した技を今までに使用したことがあるなら true
 */
cell BattleAiCommand::CMDFUNC_IF_HAVE_WAZA( AiScriptCommandHandler* handle, const cell* args )
{
  return check_have_waza( handle, args[0], (WazaNo)args[1] );
}

/**
 * @brief 特定のワザを持っているかチェック
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[1]  チェック対象の技No.
 * @return 指定したポケモンが味方サイドなら、指定した技を持っていないなら true
 *         指定したポケモンが相手サイドなら、指定した技を今までに使用したことがないなら true
 */
cell BattleAiCommand::CMDFUNC_IFN_HAVE_WAZA( AiScriptCommandHandler* handle, const cell* args )
{
  return !check_have_waza( handle, args[0], (WazaNo)args[1] );
}

/**
 * @brief 指定ポケモンが、前ターンで最後に受けた技の技No.を取得する
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @retval WAZANO_NULL  前ターンで技を受けていない場合
 */
cell BattleAiCommand::CMDFUNC_GET_LAST_DAMAGED_WAZA_AT_PREV_TURN( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  if( bpp == NULL ) {
    return WAZANO_NULL;
  }

  BTL_POKEPARAM::WAZADMG_REC rec;
  if( bpp->WAZADMGREC_Get( 1, 0, &rec ) ) {
    return rec.wazaID;
  }
  return WAZANO_NULL;
}


//-----------------------------------------------------------------------------
//  ポケモンの比較
//-----------------------------------------------------------------------------

/**
 * @brief 攻撃ポケモンと防御ポケモンについて、どちらが先行かチェックする
 * @param handle   スクリプト実行中のAI
 * @param args[0]  判定条件( IF_FIRST_ATTACK, IF_FIRST_DEFENCE, IF_FIRST_EQUAL )
 * @return 指定した判定条件を満たすなら true
 */
cell BattleAiCommand::CMDFUNC_IF_FIRST( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* atk_poke = handle->GetAttackPokeParam();
  const BTL_POKEPARAM* def_poke = handle->GetDefensePokeParam();

  if( ( atk_poke == NULL ) ||
      ( def_poke == NULL ) ) {
    GFL_ASSERT(0);
    return false;
  }

  int check_type = args[0];

  u16 atk_agility = handle->GetServerFlow()->Hnd_CalcAgility( atk_poke, true );
  u16 def_agility = handle->GetServerFlow()->Hnd_CalcAgility( def_poke, true );

  switch( check_type ){
  case IF_FIRST_ATTACK:  return (atk_agility > def_agility);
  case IF_FIRST_DEFENCE: return (atk_agility < def_agility);
  case IF_FIRST_EQUAL:   return (atk_agility == def_agility);
  }
  // 未知の定義です
  GFL_ASSERT(0);
  return false;
}





//-----------------------------------------------------------------------------
//  控えポケモン
//-----------------------------------------------------------------------------

/**
 * @brief 控えポケモンの数を返す
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @return 指定したポケモンが含まれるパーティの内、戦えるポケモンの数
 */
cell BattleAiCommand::CMDFUNC_CHECK_BENCH_COUNT( AiScriptCommandHandler* handle, const cell* args )
{
  BtlPokePos  pos = handle->AISideToPokePos( args[0] );
  u8 clientID = handle->GetMainModule()->BtlPosToClientID( pos );

  const BTL_PARTY* party = handle->GetPokeCon()->GetPartyDataConst( clientID );
  u32 front_pos_count = handle->GetMainModule()->GetClientFrontPosCount( clientID );
  u32 member_count = party->GetMemberCount();

  u32 result = 0;
  for(u32 i=front_pos_count; i<member_count; ++i)
  {
    const BTL_POKEPARAM* bpp = party->GetMemberDataConst( i );
    if( bpp->IsFightEnable() ){
      ++result;
    }
  }

  return result;
}

/**
 * @brief 状態異常の控えポケモンが存在するかチェックする
 * @param handle   スクリプト実行中のAI
 * @param ai_side  チェック対象パーティの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @return 指定したポケモンが属するパーティの中に、状態異常のポケモンがいるなら true
 */
bool BattleAiCommand::check_pokesick_in_bench( AiScriptCommandHandler* handle, int ai_side )
{
  u8 clientID = handle->AISideToClientID( ai_side );

  const BTL_PARTY* party = handle->GetPokeCon()->GetPartyDataConst( clientID );
  u32 front_pos_count = handle->GetMainModule()->GetClientFrontPosCount( clientID );
  u32 member_count = party->GetMemberCount();

  for(u32 i=front_pos_count; i<member_count; ++i)
  {
    const BTL_POKEPARAM* bpp = party->GetMemberDataConst( i );
    if( bpp->GetPokeSick() != pml::pokepara::SICK_NULL )
    {
      return true;
    }
  }

  return false;
}

/**
 * @brief 状態異常の控えポケモンが存在するかチェックする
 * @param handle   スクリプト実行中のAI
 * @param ai_side  チェック対象パーティの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @return 指定したポケモンが属するパーティの中に、状態異常のポケモンがいるなら true
 */
cell BattleAiCommand::CMDFUNC_IF_BENCH_COND( AiScriptCommandHandler* handle, const cell* args )
{
  return check_pokesick_in_bench( handle, args[0] );
}

/**
 * @brief 状態異常の控えポケモンが存在するかチェックする
 * @param handle   スクリプト実行中のAI
 * @param ai_side  チェック対象パーティの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @return 指定したポケモンが属するパーティの中に、状態異常のポケモンがいないなら true
 */
cell BattleAiCommand::CMDFUNC_IFN_BENCH_COND( AiScriptCommandHandler* handle, const cell* args )
{
  return !check_pokesick_in_bench( handle, args[0] );
}




//-----------------------------------------------------------------------------
//  自分や相手のパラメータ変化値を参照して分岐
//-----------------------------------------------------------------------------

/**
 * @brief 指定したポケモンのパラメータを取得する
 * @param handle   スクリプト実行中のAI
 * @param ai_side  取得対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param valueID  取得するパラメータ
 */
int BattleAiCommand::get_poke_param( AiScriptCommandHandler* handle, int ai_side, BTL_POKEPARAM::ValueID  valueID )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( ai_side );
  return bpp->GetValue( valueID );
}

/**
 * @brief 指定したポケモンのパラメータ変化値を参照して分岐
 * @param handle   スクリプト実行中のAI
 * @param args[0]  参照対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[1]  参照対象パラメータ
 * @param args[2]  分岐の閾値
 * @return 指定したパラメータが args[2] より小さいなら true
 */
cell BattleAiCommand::CMDFUNC_IF_PARA_UNDER( AiScriptCommandHandler* handle, const cell* args )
{
  int value = get_poke_param( handle, args[0], (BTL_POKEPARAM::ValueID)(args[1]) );
  return value < args[2];
}

/**
 * @brief 指定したポケモンのパラメータ変化値を参照して分岐
 * @param handle   スクリプト実行中のAI
 * @param args[0]  参照対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[1]  参照対象パラメータ
 * @param args[2]  分岐の閾値
 * @return 指定したパラメータが args[2] より大きいなら true
 */
cell BattleAiCommand::CMDFUNC_IF_PARA_OVER( AiScriptCommandHandler* handle, const cell* args )
{
  int value = get_poke_param( handle, args[0], (BTL_POKEPARAM::ValueID)(args[1]) );
  return value > args[2];
}

/**
 * @brief 指定したポケモンのパラメータ変化値を参照して分岐
 * @param handle   スクリプト実行中のAI
 * @param args[0]  参照対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[1]  参照対象パラメータ
 * @param args[2]  分岐の閾値
 * @return 指定したパラメータが args[2] と等しいなら true
 */
cell BattleAiCommand::CMDFUNC_IF_PARA_EQUAL( AiScriptCommandHandler* handle, const cell* args )
{
  int value = get_poke_param( handle, args[0], (BTL_POKEPARAM::ValueID)(args[1]) );
  return value == args[2];
}

/**
 * @brief 指定したポケモンのパラメータ変化値を参照して分岐
 * @param handle   スクリプト実行中のAI
 * @param args[0]  参照対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[1]  参照対象パラメータ
 * @param args[2]  分岐の閾値
 * @return 指定したパラメータが args[2] と等しくないなら true
 */
cell BattleAiCommand::CMDFUNC_IFN_PARA_EQUAL( AiScriptCommandHandler* handle, const cell* args )
{
  int value = get_poke_param( handle, args[0], (BTL_POKEPARAM::ValueID)(args[1]) );
  return value != args[2];
}





//------------------------------------------------------------
//  特定のワザシーケンスを持っているかチェック
//------------------------------------------------------------
bool BattleAiCommand::check_have_waza_seqno( AiScriptCommandHandler* handle, int ai_side, int seq_no )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( ai_side );

  if( !bpp->IsDead() )
  {
    u32 waza_cnt = bpp->WAZA_GetCount();

    // 攻撃側=ポケモンパラメータを全探索する
    if( (ai_side == CHECK_ATTACK) || (ai_side == CHECK_ATTACK_FRIEND) )
    {
      for(u32 i=0; i<waza_cnt; ++i)
      {
        WazaNo waza_no = bpp->WAZA_GetID( i );
        if( WAZADATA_GetParam(waza_no, pml::wazadata::PARAM_ID_AI_SEQNO) == seq_no ){
          return true;
        }
      }
    }
    // 防御側->今までに使ったワザから探索する
    else
    {
      for(u32 i=0; i<waza_cnt; ++i)
      {
        WazaNo waza_no = bpp->WAZA_GetID( i );
        if( handle->CheckWazaStored(bpp, waza_no) )
        {
          if( WAZADATA_GetParam(waza_no, pml::wazadata::PARAM_ID_AI_SEQNO) == seq_no ){
            return true;
          }
        }
      }
    }
  }
  return false;
}
cell BattleAiCommand::CMDFUNC_IF_HAVE_WAZA_SEQNO( AiScriptCommandHandler* handle, const cell* args )
{
  return check_have_waza_seqno( handle, args[0], args[1] );
}
cell BattleAiCommand::CMDFUNC_IFN_HAVE_WAZA_SEQNO( AiScriptCommandHandler* handle, const cell* args )
{
  return !check_have_waza_seqno( handle, args[0], args[1] );
}
//------------------------------------------------------------
//  「にげる」を選択する
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_ESCAPE( AiScriptCommandHandler* handle, const cell* args )
{
  handle->NotifyEscapeByAI();
  return 0;
}
//------------------------------------------------------------
//    装備アイテムNoを取得
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_CHECK_SOUBI_ITEM( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  return bpp->GetItem();
}
//------------------------------------------------------------
//    アイテム装備効果の取得
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_CHECK_SOUBI_EQUIP( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  u16 itemNo = bpp->GetItem();

  if( itemNo != ITEM_DUMMY_DATA ){
    return calc::ITEM_GetParam( itemNo, item::ITEM_DATA::PRM_ID_EQUIP );
  }
  return 0;
}
//------------------------------------------------------------
//    ポケモンの性別を取得
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_CHECK_POKESEX( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );

  return bpp->GetValue( BTL_POKEPARAM::BPP_SEX );
}
//------------------------------------------------------------
//    ねこだましできるかチェック
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_CHECK_NEKODAMASI( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );

  return bpp->CONTFLAG_Get( BTL_POKEPARAM::CONTFLG_ACTION_DONE );
}
//------------------------------------------------------------
//    たくわえるカウンタの取得
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_CHECK_TAKUWAERU( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );

  return bpp->COUNTER_Get( BTL_POKEPARAM::COUNTER_TAKUWAERU );
}
//------------------------------------------------------------
//    バトルルール取得
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_CHECK_BTL_RULE( AiScriptCommandHandler* handle, const cell* args )
{
  return handle->GetMainModule()->GetRule();
}
//------------------------------------------------------------
//    対戦者タイプ取得
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_CHECK_BTL_COMPETITOR( AiScriptCommandHandler* handle, const cell* args )
{
  return handle->GetMainModule()->GetCompetitor();
}
//------------------------------------------------------------
//    リサイクル可能アイテム取得
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_CHECK_RECYCLE_ITEM( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );

  return bpp->GetConsumedItem();
}
//------------------------------------------------------------
//    現在選択中のワザ／タイプ、威力、シーケンスナンバー取得
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_CHECK_WORKWAZA_TYPE( AiScriptCommandHandler* handle, const cell* args )
{
  return WAZADATA_GetParam( handle->GetCurrentWazaNo(), pml::wazadata::PARAM_ID_TYPE );
}
cell BattleAiCommand::CMDFUNC_CHECK_WORKWAZA_POW( AiScriptCommandHandler* handle, const cell* args )
{
  return WAZADATA_GetPower( handle->GetCurrentWazaNo() );
}
cell BattleAiCommand::CMDFUNC_CHECK_WORKWAZA_SEQNO( AiScriptCommandHandler* handle, const cell* args )
{
  return WAZADATA_GetParam( handle->GetCurrentWazaNo(), pml::wazadata::PARAM_ID_AI_SEQNO );
}
//------------------------------------------------------------
//    特定ポケモンの「まもる」カウンタを取得
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_CHECK_MAMORU_COUNT( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );

  return bpp->COUNTER_Get( BTL_POKEPARAM::COUNTER_MAMORU );
}
//------------------------------------------------------------
//    攻撃側、防御側のレベル差をチェック
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_IF_LEVEL( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* atk_poke = handle->GetAttackPokeParam();
  const BTL_POKEPARAM* def_poke = handle->GetDefensePokeParam();

  if( ( atk_poke == NULL ) ||
      ( def_poke == NULL ) ) {
    GFL_ASSERT(0);
    return false;
  }

  int atk_level = atk_poke->GetValue( BTL_POKEPARAM::BPP_LEVEL );
  int def_level = def_poke->GetValue( BTL_POKEPARAM::BPP_LEVEL );

  switch( args[0] ){
  case LEVEL_ATTACK:
    return (atk_level > def_level);
  case LEVEL_DEFENCE:
  default:
    return (atk_level < def_level);
  case LEVEL_EQUAL:
    return (atk_level == def_level);
  }
}
//------------------------------------------------------------
//  防御側が挑発状態かチェック
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_IF_CHOUHATSU( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* def_poke = handle->GetDefensePokeParam();

  if( def_poke == NULL ) {
    GFL_ASSERT(0);
    return false;
  }

  return def_poke->CheckSick( pml::wazadata::WAZASICK_TYOUHATSU );
}
cell BattleAiCommand::CMDFUNC_IFN_CHOUHATSU( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* def_poke = handle->GetDefensePokeParam();

  if( def_poke == NULL ) {
    GFL_ASSERT(0);
    return false;
  }

  return def_poke->CheckSick( pml::wazadata::WAZASICK_TYOUHATSU );
}
//------------------------------------------------------------
//  攻撃対象が味方がどうかチェック
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_IF_MIKATA_ATTACK( AiScriptCommandHandler* handle, const cell* args )
{
  if( handle->GetAttackPokePos() == handle->GetDefensePokePos() ){
    return false;
  }

  BtlRule rule   = handle->GetMainModule()->GetRule();
  bool    result = MainModule::IsFriendPokePos( rule, handle->GetAttackPokePos(), handle->GetDefensePokePos() );
  return result;
}

//------------------------------------------------------------
//  指定ポケモンが、指定タイプを持っているか判定
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_CHECK_HAVE_TYPE( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  pml::PokeType  check_type   = args[1];
  PokeTypePair pair_type = bpp->GetPokeType();

  return PokeTypePair_IsMatch( pair_type, check_type );
}
//------------------------------------------------------------
//  指定ポケモンが、指定とくせいを持っているか判定
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_CHECK_HAVE_TOKUSEI( AiScriptCommandHandler* handle, const cell* args )
{
  TokuseiNo  check_tokusei = (TokuseiNo)( args[1] );
  if( check_tokusei == handle->CheckTokuseiByAISide(args[0]) ){
    return true;
  }
  return false;
}
//------------------------------------------------------------
//  すでに「もらいび」でパワーアップ状態にあるかチェック
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_IF_ALREADY_MORAIBI( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  return bpp->CONTFLAG_Get( BTL_POKEPARAM::CONTFLG_MORAIBI );
}
//------------------------------------------------------------
//  アイテムを所持しているかチェック
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_IF_HAVE_ITEM( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  int itemNo = args[1];

  return (bpp->GetItem() == itemNo);
}
//------------------------------------------------------------
//  フィールド効果チェック
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_FLDEFF_CHECK( AiScriptCommandHandler* handle, const cell* args )
{
 FieldStatus::EffectType  eff_type = (FieldStatus::EffectType)(args[0]);
 return handle->GetServerFlow()->Hnd_CheckFieldEffect( eff_type );
}
//------------------------------------------------------------
//  サイド効果のカウンタ取得
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_CHECK_SIDEEFF_COUNT( AiScriptCommandHandler* handle, const cell* args )
{
  BtlPokePos     pos = handle->AISideToPokePos( args[0] );
  BtlSideEffect  eff = (BtlSideEffect)( args[1] );

  return handle->GetServerFlow()->Hnd_GetSideEffectCount( pos, eff );
}
//------------------------------------------------------------
//  ベンチポケモンの HP, PP減少チェック
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_IF_BENCH_HPDEC( AiScriptCommandHandler* handle, const cell* args )
{
  u8 clientID = handle->AISideToClientID( args[0] );
  const BTL_PARTY* party = handle->GetPokeCon()->GetPartyDataConst( clientID );

  u32 member_count = party->GetMemberCount();
  u32 front_count  = handle->GetMainModule()->GetClientFrontPosCount( clientID );
  for(u32 i=front_count; i<member_count; ++i)
  {
    const BTL_POKEPARAM* bpp = party->GetMemberDataConst( i );
    if( !(bpp->IsDead())
    &&  (bpp->GetHPRatio() < FX32_CONST(100))
    ){
      return true;
    }
  }
  return false;
}
cell BattleAiCommand::CMDFUNC_IF_BENCH_PPDEC( AiScriptCommandHandler* handle, const cell* args )
{
  u8 clientID = handle->AISideToClientID( args[0] );
  const BTL_PARTY* party = handle->GetPokeCon()->GetPartyDataConst( clientID );

  u32 member_count = party->GetMemberCount();
  u32 front_count  = handle->GetMainModule()->GetClientFrontPosCount( clientID );
  for(u32 i=front_count; i<member_count; ++i)
  {
    const BTL_POKEPARAM* bpp = party->GetMemberDataConst( i );
    if( !bpp->IsDead() )
    {
      u32 waza_count = bpp->WAZA_GetCount();
      for(u32 w=0; w<waza_count; ++w)
      {
        if( bpp->WAZA_GetPPShort(w) > 0 ){
          return true;
        }
      }
    }
  }
  return false;
}

//------------------------------------------------------------
//  装備アイテムのなげつける威力を取得
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_CHECK_NAGETSUKERU_IRYOKU( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );

  if( bpp->CheckSick(pml::wazadata::WAZASICK_SASIOSAE) ){
    return 0;
  }
  u32 itemNo = bpp->GetItem();
  return calc::ITEM_GetParam( itemNo, item::ITEM_DATA::PRM_ID_NAGE_ATC );
}
//------------------------------------------------------------
//  攻撃ポケ、選択ワザの残りPPを取得
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_CHECK_PP_REMAIN( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* atk_poke = handle->GetAttackPokeParam();

  if( atk_poke == NULL ) {
    GFL_ASSERT(0);
    return 0;
  }

  return atk_poke->WAZA_GetPP( handle->GetCurrentWazaIndex() );
}
//------------------------------------------------------------
//  とっておきチェック
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_IF_TOTTEOKI( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );

  u32 waza_count = bpp->WAZA_GetCount();
  if( (bpp->WAZA_GetUsedCountInAlive() >= ( waza_count - 1 ) )
  &&  (waza_count > 1)
  ){
    return true;
  }
  return false;
}
//------------------------------------------------------------
//  技ダメージタイプチェック
//------------------------------------------------------------
// 自分が選択中のワザ
cell BattleAiCommand::CMDFUNC_CHECK_WAZA_KIND( AiScriptCommandHandler* handle, const cell* args )
{
  return WAZADATA_GetParam( handle->GetCurrentWazaNo(), pml::wazadata::PARAM_ID_DAMAGE_TYPE );
}
// 相手が最後に出したワザのダメージタイプ
cell BattleAiCommand::CMDFUNC_CHECK_LAST_WAZA_KIND( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* def_poke = handle->GetDefensePokeParam();

  if( def_poke == NULL ) {
    GFL_ASSERT(0);
    return POKETYPE_NORMAL;
  }

  WazaNo  waza = def_poke->GetPrevWazaID();
  return WAZADATA_GetParam( waza, pml::wazadata::PARAM_ID_DAMAGE_TYPE );
}

//------------------------------------------------------------
//  指定ポケが、場にいるポケモンの中で何番目に素早いかチェック（0 org）
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_CHECK_AGI_RANK( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );

  return handle->GetServerFlow()->Hnd_CalcAgilityRank( bpp, true );
}
//------------------------------------------------------------
//  スロースタート何ターン目かチェック
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_CHECK_SLOWSTART_TURN( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  return bpp->GetTurnCount();
}
//------------------------------------------------------------
//  自分（現在攻撃ポケ）より控えにいるポケモンの方が大ダメージを与えるかどうかチェック
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_IF_BENCH_DAMAGE_MAX( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* atk_poke = handle->GetAttackPokeParam();
  const BTL_POKEPARAM* def_poke = handle->GetDefensePokeParam();

  if( ( atk_poke == NULL ) ||
      ( def_poke == NULL ) ) {
    GFL_ASSERT(0);
    return false;
  }

  bool loss_flag = ( args[0] != LOSS_CALC_OFF );
  u8  clientID = handle->GetMainModule()->BtlPosToClientID( handle->GetAttackPokePos() );
  const BTL_PARTY* party = handle->GetPokeCon()->GetPartyDataConst( clientID );

  u32 front_count  = handle->GetMainModule()->GetClientFrontPosCount( clientID );
  u32 member_count = party->GetMemberCount();

  u32 max_damage = handle->CalcMaxDamage( atk_poke, def_poke, loss_flag );
  for(u32 i=front_count; i<member_count; ++i)
  {
    const BTL_POKEPARAM* bpp = party->GetMemberDataConst( i );
    if( !(bpp->IsDead()) )
    {
      u32 dmg = handle->CalcMaxDamage( bpp, def_poke, loss_flag );
      if( dmg > max_damage ){
        return true;
      }
    }
  }
  return false;
}
//-----------------------------------------------------------------------------
/**
 * @brief 指定したポケモンが、相手に対して抜群の技を持っているかチェック
 * @param handle   スクリプト実行中のAI
 * @param args[0]  攻撃側ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[1]  防御側ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 */
//-----------------------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_IF_HAVE_BATSUGUN( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* atk_poke = handle->GetBppByAISide( args[0] );
  const BTL_POKEPARAM* def_poke = handle->GetBppByAISide( args[1] );

  if( ( atk_poke == NULL ) ||
      ( def_poke == NULL ) ) {
    GFL_ASSERT(0);
    return false;
  }

  u32 waza_count = atk_poke->WAZA_GetCount();
  for(u32 i=0; i<waza_count; ++i)
  {
    WazaNo      waza = atk_poke->WAZA_GetID( i );
    BtlTypeAff  aff  = handle->GetServerFlow()->Hnd_SimulationAffinity( atk_poke->GetID(), def_poke->GetID(), waza );

    if( aff >= pml::battle::TypeAffinity::TYPEAFF_2 ){
      return true;
    }
  }
  return false;
}
//------------------------------------------------------------
//  指定したポケモンの最後に出した技と自分の技の最大ダメージをチェック（自分の技の方がが小さければtrue）
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_IF_LAST_WAZA_DAMAGE_CHECK( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* atk_poke = handle->GetAttackPokeParam();
  const BTL_POKEPARAM* def_poke = handle->GetDefensePokeParam();

  if( ( atk_poke == NULL ) ||
      ( def_poke == NULL ) ) {
    GFL_ASSERT(0);
    return false;
  }

  const BTL_POKEPARAM* targetPoke = handle->GetBppByAISide( args[0] );
  bool loss_flag = ( args[1] != LOSS_CALC_OFF );

  u32 my_max_damage = handle->CalcMaxDamage( atk_poke, def_poke, loss_flag );
  u32 target_prev_damage = handle->GetServerFlow()->Hnd_SimulationDamage(
                                targetPoke->GetID(), def_poke->GetID(),
                                targetPoke->GetPrevWazaID(),
                                true, loss_flag );

  return (my_max_damage < target_prev_damage);
}
//------------------------------------------------------------
//  指定したポケモンの能力アップ合計値を取得
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_CHECK_STATUS_UP( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );

  BTL_POKEPARAM::ValueID  id_tbl[]={
    BTL_POKEPARAM::BPP_ATTACK_RANK,
    BTL_POKEPARAM::BPP_DEFENCE_RANK,
    BTL_POKEPARAM::BPP_SP_ATTACK_RANK,
    BTL_POKEPARAM::BPP_SP_DEFENCE_RANK,
    BTL_POKEPARAM::BPP_AGILITY_RANK,
    BTL_POKEPARAM::BPP_HIT_RATIO,
    BTL_POKEPARAM::BPP_AVOID_RATIO,
  };

  int total = 0;

  for(u32 i=0; i<GFL_NELEMS(id_tbl); ++i)
  {
    int rank = bpp->GetValue( id_tbl[i] );
    if( rank > BTL_POKEPARAM::RANK_STATUS_DEFAULT )
    {
      total += (rank - BTL_POKEPARAM::RANK_STATUS_DEFAULT);
    }
  }

  return total;
}
//------------------------------------------------------------
//  自分と指定した相手とのステータス差分を取得
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_CHECK_STATUS_DIFF( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* atk_poke = handle->GetAttackPokeParam();

  if( atk_poke == NULL ) {
    GFL_ASSERT(0);
    return 0;
  }

  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );

  BTL_POKEPARAM::ValueID  valueID = (BTL_POKEPARAM::ValueID)( args[1] );

  int diff = bpp->GetValue(valueID) - atk_poke->GetValue(valueID);

  return diff;
}
//------------------------------------------------------------
//  指定ポケモンの、指定能力についてステータス変動値を取得
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_CHECK_STATUS( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );

  BTL_POKEPARAM::ValueID  valueID = (BTL_POKEPARAM::ValueID)( args[1] );

  return bpp->GetValue( valueID );
}


//------------------------------------------------------------
//  選択している技の威力が一番高いかチェック（パートナーも含む）
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_COMP_POWER_WITH_PARTNER( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* atk_poke = handle->GetAttackPokeParam();
  const BTL_POKEPARAM* def_poke = handle->GetDefensePokeParam();

  if( ( atk_poke == NULL ) ||
      ( def_poke == NULL ) ) {
    GFL_ASSERT(0);
    return false;
  }

  bool loss_flag = ( args[0] != LOSS_CALC_OFF );

  u32 src_dmg = handle->GetServerFlow()->Hnd_SimulationDamage( atk_poke->GetID(), def_poke->GetID(), handle->GetCurrentWazaNo(), true, loss_flag );

  u8 clientID = handle->GetMainModule()->BtlPosToClientID( handle->GetAttackPokePos() );
  const BTL_PARTY* party = handle->GetPokeCon()->GetPartyDataConst( clientID );
  int my_idx = party->FindMember( atk_poke );

  // 選択しているのがダメージ技ではない
  if( (my_idx < 0) || (src_dmg ==0) ){
    return COMP_POWER_NONE;
  }

  u32 front_count = handle->GetMainModule()->GetClientFrontPosCount( clientID );
  for(u32 idx=0; idx<front_count; ++idx)
  {
    const BTL_POKEPARAM* bpp = party->GetMemberDataConst( idx );
    if( !(bpp->IsFightEnable() ) ){
      continue;
    }

    u32 waza_count = bpp->WAZA_GetCount();
    for(u32 i=0; i<waza_count; ++i)
    {
      if( (idx == static_cast<u32>( my_idx ) ) && (i == handle->GetCurrentWazaIndex()) ){
        continue;
      }

      WazaNo  waza = bpp->WAZA_GetID( i );
      u32 dmg = handle->GetServerFlow()->Hnd_SimulationDamage( bpp->GetID(), def_poke->GetID(), waza, true, loss_flag );
      if( dmg > src_dmg ){
        return COMP_POWER_NOTOP;
      }
    }
  }
  return COMP_POWER_TOP;
}

//------------------------------------------------------------
//  指定した相手が瀕死かチェック
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_IF_HINSHI( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );

  return bpp->IsDead();
}

cell BattleAiCommand::CMDFUNC_IFN_HINSHI( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );

  return !(bpp->IsDead());
}

//------------------------------------------------------------
//  技効果を考慮して特性を取得（移動ポケモン専用）
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_GET_TOKUSEI( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  return bpp->GetValue( BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE );
}
//------------------------------------------------------------
//  みがわり中かチェック
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_IF_MIGAWARI( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );

  return bpp->MIGAWARI_IsExist();
}
//------------------------------------------------------------
//  ポケモンナンバーの取得
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_CHECK_MONSNO( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  return bpp->GetMonsNo();
}
//------------------------------------------------------------
//  フォルムナンバーの取得
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_CHECK_FORMNO( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  return bpp->GetFormNo();
}
//------------------------------------------------------------
//  共通乱数による条件分岐
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_IF_COMMONRND_UNDER( AiScriptCommandHandler* handle, const cell* args )
{
  u32  value = args[0];
  return BattleAiSystem::GetCommonRand() < value;
}
cell BattleAiCommand::CMDFUNC_IF_COMMONRND_OVER( AiScriptCommandHandler* handle, const cell* args )
{
  u32  value = args[0];
  return BattleAiSystem::GetCommonRand() > value;
}
cell BattleAiCommand::CMDFUNC_IF_COMMONRND_EQUAL( AiScriptCommandHandler* handle, const cell* args )
{
  u32  value = args[0];
  return BattleAiSystem::GetCommonRand() == value;
}
cell BattleAiCommand::CMDFUNC_IFN_COMMONRND_EQUAL( AiScriptCommandHandler* handle, const cell* args )
{
  u32  value = args[0];
  return BattleAiSystem::GetCommonRand() != value;
}



//------------------------------------------------------------
//  みらいよち中かチェック
//------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_IF_MIRAIYOCHI( AiScriptCommandHandler* handle, const cell* args )
{
  BtlPokePos  pos = handle->AISideToPokePos( args[0] );

  return handle->GetServerFlow()->Hnd_IsExistPosEffect( pos, BTL_POSEFF_DELAY_ATTACK );
}
//------------------------------------------------------------
//  攻撃と特攻を比べて分岐
//------------------------------------------------------------
cell  BattleAiCommand::CMDFUNC_IF_DMG_PHYSIC_UNDER( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );

  int pow    = bpp->GetValue( BTL_POKEPARAM::BPP_ATTACK );
  int sp_pow = bpp->GetValue( BTL_POKEPARAM::BPP_SP_ATTACK );

  // WBとは挙動が異なることになるが、WBがたぶん間違っていると思われる…
  return pow < sp_pow;
}
cell  BattleAiCommand::CMDFUNC_IF_DMG_PHYSIC_OVER( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );

  int pow    = bpp->GetValue( BTL_POKEPARAM::BPP_ATTACK );
  int sp_pow = bpp->GetValue( BTL_POKEPARAM::BPP_SP_ATTACK );

  // WBとは挙動が異なることになるが、WBがたぶん間違っていると思われる…
  return pow > sp_pow;
}
cell  BattleAiCommand::CMDFUNC_IF_DMG_PHYSIC_EQUAL( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );

  int pow    = bpp->GetValue( BTL_POKEPARAM::BPP_ATTACK );
  int sp_pow = bpp->GetValue( BTL_POKEPARAM::BPP_SP_ATTACK );

  // WBとは挙動が異なることになるが、WBがたぶん間違っていると思われる…
  return pow != sp_pow;
}
//------------------------------------------------------------
//  木の実をたべたかどうか
//------------------------------------------------------------
cell  BattleAiCommand::CMDFUNC_IF_ATE_KINOMI( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  return bpp->PERMFLAG_Get( BTL_POKEPARAM::PERMFLAG_ATE_KINOMI );
}
//------------------------------------------------------------
//  特定の拡張タイプが設定されているか（ハロウィン、もりののろい）
//------------------------------------------------------------
cell  BattleAiCommand::CMDFUNC_IF_TYPE_EX( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  return (bpp->GetExType() == args[1]);
}
//------------------------------------------------------------
//  特定のxxxグラウンド状態が生じているか
//------------------------------------------------------------
cell  BattleAiCommand::CMDFUNC_IF_EXIST_GROUND( AiScriptCommandHandler* handle, const cell* args )
{
  const FieldStatus*  fldSim = handle->GetPokeCon()->GetFieldStatusConst();
  if( fldSim )
  {
    return fldSim->CheckEffect( FieldStatus::EFF_GROUND, args[0] );
  }
  return false;
}
//------------------------------------------------------------
//  体重を取得（アイテムやとくせいハンドラの効果はチェックしない）
//------------------------------------------------------------
cell  BattleAiCommand::CMDFUNC_GET_WEIGHT( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  return bpp->GetWeight();
}
//------------------------------------------------------------
//  マルチバトルかどうか判定
//------------------------------------------------------------
cell  BattleAiCommand::CMDFUNC_IF_MULTI( AiScriptCommandHandler* handle, const cell* args )
{
  return handle->GetMainModule()->IsMultiMode();
}
//------------------------------------------------------------
//  対象がメガシンカしたポケモンかどうか判定
//------------------------------------------------------------
cell  BattleAiCommand::CMDFUNC_IF_MEGAEVOLVED( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  return bpp->IsMegaEvolved();
}

//-----------------------------------------------------------------------------
/**
 * @brief 対象がメガシンカ可能なポケモンかどうか判定
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @retval true  指定したポケモンがメガシンカできる場合
 * @retval false 指定したポケモンがメガシンカできない場合
 */
//-----------------------------------------------------------------------------
cell  BattleAiCommand::CMDFUNC_IF_CAN_MEGAEVOLVE( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  return bpp->IsAbleToMegaEvo();
}

//------------------------------------------------------------
//  対象がワザ効果により場から消えているかどうか判定（そらをとぶ等）
//------------------------------------------------------------
cell  BattleAiCommand::CMDFUNC_IF_WAZAHIDE( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  BTL_POKEPARAM::ContFlag flag = bpp->CONTFLAG_CheckWazaHide();
  return flag != BTL_POKEPARAM::CONTFLG_NULL;
}
//------------------------------------------------------------
//  対象のめざめるパワータイプを取得する
//------------------------------------------------------------
cell  BattleAiCommand::CMDFUNC_GET_MEZAPA_TYPE( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* bpp = handle->GetBppByAISide( args[0] );
  const pml::pokepara::PokemonParam* pp = bpp->GetSrcData();
  return pp->GetMezapaType();
}

//-----------------------------------------------------------------------------
/**
 * @brief 戦闘AI( コマンドコールした本人 )がシナリオトレーナーかどうか判定
 * @param handle   スクリプト実行中のAI
 * @return  シナリオトレーナーの場合 true
 */
//-----------------------------------------------------------------------------
cell  BattleAiCommand::CMDFUNC_IF_I_AM_SENARIO_TRAINER( AiScriptCommandHandler* handle, const cell* args )
{
  return ( handle->GetMainModule()->GetCompetitor() == BTL_COMPETITOR_TRAINER );
}

//-----------------------------------------------------------------------------
/**
 * @brief 対象が持つダメージ技のうち、最も高い威力で撃つことが出来る技の、威力値を取得する( ターゲットとのタイプ相性のみを考慮 )
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 */
//-----------------------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_GET_MAX_WAZA_POWER_INCLUDE_AFFINITY( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* attackPoke = handle->GetBppByAISide( args[0] );
  const BTL_POKEPARAM* defensePoke_0 = handle->GetBppByAISide( CHECK_DEFENCE );
  const BTL_POKEPARAM* defensePoke_1 = handle->GetBppByAISide( CHECK_DEFENCE_FRIEND );
  u32 maxPower = GetMaxWazaPowerIncludeAffinity( handle->GetServerFlow(), attackPoke, defensePoke_0 );

  // ダブルバトルなら、両方の相手を対象に計算する
  if( defensePoke_0 != defensePoke_1 ) 
  {
    u32 power = GetMaxWazaPowerIncludeAffinity( handle->GetServerFlow(), attackPoke, defensePoke_1 );
    if( maxPower < power ) {
      maxPower = power;
    }
  }

  return static_cast<s32>( maxPower );
}

/**
 * @brief 対象が持つダメージ技のうち、最も高い威力で撃つことが出来る技の、威力値を取得する( ターゲットとのタイプ相性のみを考慮 )
 * @param attackPoke   攻撃側のポケモン
 * @param defensePoke  防御側のポケモン
 */
u32 BattleAiCommand::GetMaxWazaPowerIncludeAffinity( ServerFlow* serverFlow, const BTL_POKEPARAM* attackPoke, const BTL_POKEPARAM* defensePoke )
{
  u32 maxPower = 0;

  u8 wazaNum = attackPoke->WAZA_GetCount();
  for( u32 wazaIndex=0; wazaIndex<wazaNum; ++wazaIndex )
  {
    if( attackPoke->WAZA_GetPP( wazaIndex ) == 0 ) {
      continue;
    }

    WazaID waza = attackPoke->WAZA_GetID( wazaIndex );
    if( !( WAZADATA_IsDamage( waza ) ) ) {
      continue;
    }

    pml::PokeType wazaType = WAZADATA_GetType( waza );
    u32 wazaPower = WAZADATA_GetPower( waza );
    BtlTypeAff affinity = serverFlow->Hnd_SimulationAffinityCanBench( attackPoke->GetID(), defensePoke->GetID(), static_cast<WazaNo>(waza) );
    wazaPower = calc::AffDamage( wazaPower, affinity );
    if( maxPower < wazaPower ) {
      maxPower = wazaPower;
    }
  }

  return maxPower;
}



//-----------------------------------------------------------------------------
/**
 * @brief 採点中の技の道具No.を取得する
 * @param handle   スクリプト実行中のAI
 */
//-----------------------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_GET_CURRENT_ITEMNO( AiScriptCommandHandler* handle, const cell* args )
{
  return handle->GetCurrentItemNo();
}

//-----------------------------------------------------------------------------
/**
 * @brief 技「じだんだ」を高威力で撃てる状態か？
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @retval true   高威力で撃てる
 * @retval false  高威力で撃てない
 */
//-----------------------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_IF_ZIDANDA_POWERUP( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* poke = handle->GetBppByAISide( args[0] );
  const u8 pokeID = poke->GetID();
  const ServerFlow* serverFlow = handle->GetServerFlow();

  if( serverFlow->Hnd_CheckActionRecord( pokeID, 0, ActionRecorder::ACTION_WAZA_FAILED_HIT_PERCENTAGE ) ||
      serverFlow->Hnd_CheckActionRecord( pokeID, 0, ActionRecorder::ACTION_WAZA_FAILED_TOKUSEI ) ||
      serverFlow->Hnd_CheckActionRecord( pokeID, 0, ActionRecorder::ACTION_WAZA_FAILED_TYPE ) )
  {
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @brief バトルロイヤルの現在順位を取得する
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @return 指定クライアントの現在順位
 * @retval [1,4]  
 */
//-----------------------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_GET_BATTLEROYAL_RANKING( AiScriptCommandHandler* handle, const cell* args )
{
  if( handle->GetMainModule()->GetRule() != BTL_RULE_ROYAL )
  {
    GFL_ASSERT(0);
    return 1;
  }

  const u8 clientID = handle->AISideToClientID( args[0] );
  const ServerFlow* serverFlow = handle->GetServerFlow();
  const RoyalRankingContainer& rankingContainer = serverFlow->Hnd_GetRoyalRaningContainer();
  return rankingContainer.GetClientRanking( clientID ) + 1;  // 1オリジンにするために+1
}

//-----------------------------------------------------------------------------
/**
 * @brief 指定クライアントが倒したポケモンの数を取得する
 * @param handle   スクリプト実行中のAI
 * @param args[0]  チェック対象ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @return 指定クライアントが倒したポケモンの数
 */
//-----------------------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_GET_CLIENT_KILL_COUNT( AiScriptCommandHandler* handle, const cell* args )
{
  const u8 clientID = handle->AISideToClientID( args[0] );
  const BTL_PARTY* party = handle->GetPokeCon()->GetPartyDataConst( clientID );
  return party->GetTotalKillCount();
}

//-----------------------------------------------------------------------------
/**
 * @brief 現在採点中の技の対象範囲を取得する
 */
//-----------------------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_GET_WAZA_TARGET( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* attacker = handle->GetAttackPokeParam();
  const WazaID waza = handle->GetCurrentWazaNo(); 
  return calc::GetWazaTarget( waza, attacker );
}

//-----------------------------------------------------------------------------
/**
 * @brief 指定したポケモンが、相手に対して抜群の技を持っているかチェック（ポケモンチェンジ専用）
 * @param handle   スクリプト実行中のAI
 * @param args[0]  攻撃側ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 * @param args[1]  防御側ポケモンの指定( CHECK_DEFENCE, CHECK_ATTACK, CHECK_DEFENCE_FRIEND, CHECK_ATTACK_FRIEND )
 */
//-----------------------------------------------------------------------------
cell BattleAiCommand::CMDFUNC_IF_HAVE_BATSUGUN_CAN_BENCH( AiScriptCommandHandler* handle, const cell* args )
{
  const BTL_POKEPARAM* atk_poke = handle->GetBppByAISide( args[0] );
  const BTL_POKEPARAM* def_poke = handle->GetBppByAISide( args[1] );

  if( ( atk_poke == NULL ) ||
      ( def_poke == NULL ) ) {
    GFL_ASSERT(0);
    return false;
  }

  u32 waza_count = atk_poke->WAZA_GetCount();
  for(u32 i=0; i<waza_count; ++i)
  {
    WazaNo      waza = atk_poke->WAZA_GetID( i );
    BtlTypeAff  aff  = handle->GetServerFlow()->Hnd_SimulationAffinityCanBench( atk_poke->GetID(), def_poke->GetID(), waza );

    if( aff >= pml::battle::TypeAffinity::TYPEAFF_2 ){
      return true;
    }
  }
  return false;
}

GFL_NAMESPACE_END( btl )

//=============================================================================================
/**
 * @file  hand_tokusei.cpp
 * @brief ポケモンXY バトルシステム イベントファクター [とくせい]
 * @author  taya
 *
 * @date  2011.01.18  作成
 */
//=============================================================================================
#include <niji_conv_header/sound/SoundMiddleID.h>
#include <Savedata/include/MyItemSave.h>

#include "..\btl_calc.h"
#include "..\btl_tables.h"
#include "../btl_FormChange.h"
#include "./hand_common.h"
#include "./hand_tokusei.h"

#if defined(GF_PLATFORM_WIN32)
#pragma warning( disable  : 4800 )  //ブール値を 'true' または 'false' に強制的に設定します (警告の処理)
#endif

GFL_NAMESPACE_BEGIN(btl)
namespace handler {
namespace tokusei {

/**
 * [とくせい]ハンドラ共通 work 用途
 */
enum WorkIdx {
  WIDX0 = 0,
  WIDX1,
  WIDX2,
  WIDX3,
  WIDX_REMOVE_GUARD,
  NUM_WIDX,
};


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
u32 numHandlersWithHandlerPri( u16 pri, u16 numHandlers );
u16 calcTokHandlerSubPriority( const BTL_POKEPARAM* bpp );
BtlEventPriority  devideNumHandersAndPri( u32* numHandlers );
bool isExePer( ServerFlow* flowWk, u8 per );

const BtlEventHandlerTable*  HAND_TOK_ADD_Ikaku( u32* numElems );
void handler_Ikaku_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Seisinryoku( u32* numElems );
void handler_Seisinryoku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Fukutsuno( u32* numElems );
void handler_FukutsunoKokoro( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_AtuiSibou( u32* numElems );
void handler_AtuiSibou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Tikaramoti( u32* numElems );
void handler_Tikaramoti( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Suisui( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Suisui( u32* numElems );
void handler_Youryokuso( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Youryokuso( u32* numElems );
void handler_Hayaasi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Hayaasi( u32* numElems );
void handler_Tidoriasi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Tidoriasi( u32* numElems );
void handler_Harikiri_HitRatio( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Harikiri_AtkPower( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Harikiri( u32* numElems );
void handler_Atodasi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Atodasi( u32* numElems );
void handler_SlowStart_Agility( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SlowStart_AtkPower( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SlowStart_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void common_SlowStart_Declare( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SlowStart_TurnCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_SlowStart( u32* numElems );
void handler_Fukugan( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Fukugan( u32* numElems );
void handler_Sunagakure( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Sunagakure_Weather( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Sunagakure( u32* numElems );
void handler_Yukigakure( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Yukigakure_Weather( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Yukigakure( u32* numElems );
void common_weather_guard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work, BtlWeather weather );
void handler_Iromegane( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Iromegane( u32* numElems );
void handler_HardRock( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_HardRock( u32* numElems );
void handler_Sniper( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Sniper( u32* numElems );
void handler_Kasoku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Kasoku( u32* numElems );
void handler_Tekiouryoku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Tekiouryoku( u32* numElems );
void handler_Mouka( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Mouka( u32* numElems );
void handler_Gekiryu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Gekiryu( u32* numElems );
void handler_Sinryoku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Sinryoku( u32* numElems );
void handler_MusinoSirase( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_MusinoSirase( u32* numElems );
void common_hpborder_powerup( ServerFlow* flowWk, u8 pokeID, pml::PokeType wazaType );
void handler_Konjou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Konjou( u32* numElems );
void handler_PlusMinus( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Plus( u32* numElems );
bool checkExistTokuseiFriend( ServerFlow* flowWk, u8 pokeID, int* work, TokuseiNo tokuseiID );
bool checkFlowerGiftEnablePokemon( ServerFlow* flowWk, u8 pokeID );
void handler_FlowerGift_MemberInComp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_FlowerGift_GotTok( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void common_FlowerGift_FormChange( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, u8 nextForm, u8 fTokWin );
void handler_FlowerGift_Weather( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_FlowerGift_TokOff( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_FlowerGift_AirLock( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_FlowerGift_TokChange( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_FlowerGift_Power( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_FlowerGift_Guard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_FlowerGift( u32* numElems );
void handler_Tousousin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Tousousin( u32* numElems );
void handler_Technician( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Technician( u32* numElems );
void handler_TetunoKobusi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_TetunoKobusi( u32* numElems );
void handler_Sutemi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Sutemi( u32* numElems );
void handler_FusiginaUroko( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_FusiginaUroko( u32* numElems );
void handler_SkillLink( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_SkillLink( u32* numElems );
const BtlEventHandlerTable*  HAND_TOK_ADD_KairikiBasami( u32* numElems );
void handler_KairikiBasami_Check( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KairikiBasami_Guard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Surudoime( u32* numElems );
void handler_Surudoime_Check( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Surudoime_Guard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Surudoime_HitRank( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_ClearBody( u32* numElems );
void handler_ClearBody_Check( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_ClearBody_Guard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void common_RankDownGuard_Check( ServerFlow* flowWk, u8 pokeID, int* work, WazaRankEffect rankType );
void common_RankDownGuard_Fixed( ServerFlow* flowWk, u8 pokeID, u8 tokwin_pokeID, int* work, u16 strID );
void handler_Tanjun( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Tanjun( u32* numElems );
void handler_ReafGuard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_ReafGuard_InemuriCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_ReafGuard( u32* numElems );
void handler_Juunan_PokeSick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Juunan_Wake( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Juunan_ActEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Juunan( u32* numElems );
void handler_Fumin_PokeSick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Fumin_Wake( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Fumin_ActEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Fumin_InemuriCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Fumin( u32* numElems );
void handler_MagumaNoYoroi_PokeSick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MagumaNoYoroi_Wake( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MagumaNoYoroi_ActEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_MagumaNoYoroi( u32* numElems );
void handler_Meneki_PokeSick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Meneki_Wake( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Meneki_ActEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Meneki( u32* numElems );
void handler_MizuNoBale_PokeSick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MizuNoBale_Wake( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MizuNoBale_ActEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_MizuNoBale( u32* numElems );
void handler_MyPace_PokeSick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MyPace_AddSickFailed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MyPace_Wake( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MyPace_ActEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_MyPace( u32* numElems );
void handler_Donkan( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Donkan_Wake( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Donkan_ActEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Donkan_NoEffCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Donkan( u32* numElems );
void common_AddSickFailed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work, u16 strID );
void handler_AddSickFailCommon( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void common_TokuseiWake_CureSick( ServerFlow* flowWk, u8 pokeID, WazaSick sick );
void common_TokuseiWake_CureSickCore( ServerFlow* flowWk, u8 pokeID, WazaSick sick );
void handler_Amefurasi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Amefurasi( u32* numElems );
void handler_Hideri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Hideri( u32* numElems );
void handler_Sunaokosi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Sunaokosi( u32* numElems );
void handler_Yukifurasi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Yukifurasi( u32* numElems );
//===========================================================================================
//@バトル変更箇所
//sango追加ここから(天候系なのでここに追加)
void handler_Hajimarinoumi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Hajimarinoumi_stop( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Hajimarinoumi( u32* numElems );
void handler_Owarinodaichi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Owarinodaichi_stop( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Owarinodaiti( u32* numElems );
void handler_DeltaStream( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_DeltaStream_stop( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_DeltaStream( u32* numElems );
//sango追加ここまで
//===========================================================================================
//@バトル変更箇所(新規天候は永続なのでフラグ追加
void common_weather_change( ServerFlow* flowWk, u8 pokeID, BtlWeather weather, u16 boostItem , bool isPermanent );
//@バトル変更箇所(退場時に天候を止める処理
void common_weather_stop( ServerFlow* flowWk, u8 pokeID, BtlWeather weather );
//@バトル変更箇所(場に特定の特性がいるかチェックする(sango天候を戻すときのチェック用
bool common_check_tokusei( ServerFlow* flowWk, u8 selfPokeId, u16 tokusei );

void handler_AirLock_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_AirLock_ChangeWeather( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_AirLock( u32* numElems );
void handler_IceBody( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_IcoBody( u32* numElems );
void handler_AmeukeZara( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_AmeukeZara( u32* numElems );
void common_weather_recover( ServerFlow* flowWk, u8 pokeID, BtlWeather weather );
void handler_SunPower_Weather( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SunPower_AtkPower( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_SunPower( u32* numElems );
void handler_Rinpun_Sick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Rinpun_Rank( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Rinpun_Shrink( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Rinpun_Guard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Rinpun_GuardHitEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Rinpun( u32* numElems );
void handler_TennoMegumi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_TennoMegumi_Shrink( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_TennoMegumi( u32* numElems );
void handler_UruoiBody( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_UruoiBody( u32* numElems );
void handler_Dappi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Dappi( u32* numElems );
void handler_PoisonHeal( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_PoisonHeal( u32* numElems );
void handler_KabutoArmor( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_KabutoArmor( u32* numElems );
void handler_Kyouun( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Kyouun( u32* numElems );
void handler_IkarinoTubo( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_IkarinoTubo( u32* numElems );
void handler_DokunoToge( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_DokunoToge( u32* numElems );
void handler_Seidenki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Seidenki( u32* numElems );
void handler_HonoNoKarada( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_HonoNoKarada( u32* numElems );
void handler_MeromeroBody( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_MeromeroBody( u32* numElems );
void handler_Housi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Housi( u32* numElems );
void common_touchAddSick( ServerFlow* flowWk, u8 pokeID, WazaSick sick, const BTL_SICKCONT& sickCont, u8 per );
void handler_Samehada( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Samehada( u32* numElems );
void handler_Yuubaku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Yuubaku( u32* numElems );
void handler_Hensyoku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Hensyoku( u32* numElems );
void handler_Syncro( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Syncro( u32* numElems );
void handler_Isiatama( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Isiatama( u32* numElems );
void handler_NormalSkin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_NormalSkin_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_NormalSkin( u32* numElems );
const BtlEventHandlerTable*  HAND_TOK_ADD_Trace( u32* numElems );
void handler_Trace( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SizenKaifuku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_SizenKaifuku( u32* numElems );
void handler_Download( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_DownLoad( u32* numElems );
void handler_Yotimu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Yotimu( u32* numElems );
void handler_KikenYoti( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
bool check_kikenyoti_poke( ServerFlow* flowWk, const BTL_POKEPARAM* bppUser, const BTL_POKEPARAM* bppEnemy );
const BtlEventHandlerTable*  HAND_TOK_ADD_KikenYoti( u32* numElems );
void handler_Omitoosi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Omitoosi( u32* numElems );
const BtlEventHandlerTable*  HAND_TOK_ADD_Ganjou( u32* numElems );
void handler_Ganjou_Ichigeki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Ganjou_KoraeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Ganjou_KoraeExe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tennen_hitRank( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tennen_AtkRank( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tennen_DefRank( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Tennen( u32* numElems );
void handler_Tainetsu_WazaPow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tainetsu_SickDmg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Tainetu( u32* numElems );
bool common_DmgToRecover_Check( ServerFlow* flowWk, u8 pokeID, int* work, pml::PokeType matchType );
bool common_TypeNoEffect( ServerFlow* flowWk, u8 pokeID, pml::PokeType wazaType );
void common_TypeRecoverHP( ServerFlow* flowWk, u8 pokeID, u8 denomHP );
void common_TypeNoEffect_Rankup( ServerFlow* flowWk, u8 pokeID, WazaRankEffect rankType, u8 volume );
void handler_Kansouhada_Weather( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Kansouhada_DmgRecover( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Kansouhada_WazaPow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Kansouhada_Check( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Kansouhada( u32* numElems );
void handler_Tyosui_Check( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Tyosui( u32* numElems );
void handler_Tikuden_CheckEx( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Tikuden( u32* numElems );
const BtlEventHandlerTable*  HAND_TOK_ADD_DenkiEngine( u32* numElems );
void handler_DenkiEngine_CheckEx( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_NoGuard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_NoGuard( u32* numElems );
void handler_Kimottama( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Kimottama( u32* numElems );
void handler_Bouon( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Bouon( u32* numElems );
void handler_Fuyuu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Fuyuu_Disp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Fuyuu_TurnCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Fuyuu( u32* numElems );
void handler_FusiginaMamori( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_FusiginaMamori( u32* numElems );
void handler_Namake( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Namake_Get( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Nameke_Failed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Nameke_EndAct( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Namake( u32* numElems );
void handler_Simerike( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Simerike_Effective( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Simerike_StartSeq( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Simerike_EndSeq( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Simerike_Ieki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
bool handler_Simerike_SkipCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, BtlEventFactorType factorType, BtlEventType eventType, u16 subID, u8 pokeID );
const BtlEventHandlerTable*  HAND_TOK_ADD_Simerike( u32* numElems );
void handler_Moraibi_NoEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Moraibi_DmgRecoverFix( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Moraibi_AtkPower( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Moraibi_Remove( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Moraibi_CheckNoEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Moraibi( u32* numElems );
void handler_Nightmare( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Nightmare( u32* numElems );
void handler_Nigeasi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Nigeasi_Msg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Nigeasi( u32* numElems );
void handler_Katayaburi_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Katayaburi_Start( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Katayaburi_End( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Katayaburi_Ieki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Katayaburi( u32* numElems );
void handler_Tenkiya_MemberInComp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tenkiya_GetTok( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tenkiya_Weather( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tenkiya_AirLock( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tenkiya_ChangeTok( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tenkiya_TokOff( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void common_Tenkiya_Off( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID );
void common_TenkiFormChange( ServerFlow* flowWk, u8 pokeID, BtlWeather weather );
const BtlEventHandlerTable*  HAND_TOK_ADD_Tenkiya( u32* numElems );
const BtlEventHandlerTable*  HAND_TOK_ADD_Yobimizu( u32* numElems );
void handler_Yobimizu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Yobimizu_TemptTargetEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Yobimizu_CheckNoEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Hiraisin( u32* numElems );
void handler_Hiraisin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Hiraisin_TemptTargetEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Hiraisin_WazaExeStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Hiraisin_CheckNoEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void common_WazaTargetChangeToMe( ServerFlow* flowWk, u8 pokeID, int* work, pml::PokeType wazaType, TemptTargetPriority temptPriority, TemptTargetCause temptCause );
void handler_Kyuuban( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Kyuuban( u32* numElems );
void handler_HedoroEki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_HedoroEki_Dead( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_HedoroEki( u32* numElems );
bool handler_Bukiyou_SkipCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, BtlEventFactorType factorType, BtlEventType eventType, u16 subID, u8 pokeID );
void handler_Bukiyou_MemberInPrev( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Bukiyou_GetTokusei( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Bukiyou_PreChange( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Bukiyou_IekiFixed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Bukiyou_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Bukiyou_ExeFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Bukiyou( u32* numElems );
void handler_Nenchaku_NoEff( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Nenchaku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Nenchaku_Reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Nenchaku( u32* numElems );
const BtlEventHandlerTable*  HAND_TOK_ADD_Pressure( u32* numElems );
void handler_Pressure_MemberIN( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void common_MemberInTokMessage( ServerFlow* flowWk, u8 pokeID, u16 strID );
void handler_Pressure( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_MagicGuard( u32* numElems );
void handler_MagicGuard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Akusyuu( u32* numElems );
void handler_Akusyuu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Kagefumi( u32* numElems );
void handler_Kagefumi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Arijigoku( u32* numElems );
void handler_Arijigoku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Jiryoku( u32* numElems );
void handler_Jiryoku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Karuwaza( u32* numElems );
void handler_Karuwaza_BeforeItemSet( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Karuwaza_Agility( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Monohiroi( u32* numElems );
void handler_Monohiroi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_WaruiTeguse( u32* numElems );
void handler_WaruiTeguse( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_NorowareBody( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_NorowareBody( u32* numElems );
void handler_KudakeruYoroi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_KudakeruYoroi( u32* numElems );
void handler_Tikarazuku_WazaPow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tikarazuku_CheckFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tikarazuku_ShrinkCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tikarazuku_HitChk( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Tikarazuku( u32* numElems );
bool IsTikarazukuEffecive( WazaID waza );
void handler_Makenki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Makenki( u32* numElems );
void handler_Katiki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Katiki( u32* numElems );
void handler_Yowaki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Yowaki( u32* numElems );
void handler_MultiScale( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_MultiScale( u32* numElems );
void handler_NakamaIsiki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_FriendGuard( u32* numElems );
void handler_IyasiNoKokoro( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_IyasiNoKokoro( u32* numElems );
void handler_Dokubousou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Dokubousou( u32* numElems );
void handler_Netubousou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Netubousou( u32* numElems );
void handler_AunNoIki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Telepassy( u32* numElems );
void handler_Murakke( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* sys_work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Murakke( u32* numElems );
void handler_Boujin_CalcDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Boujin_WazaNoEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Boujin( u32* numElems );
void handler_Dokusyu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Dokusyu( u32* numElems );
void handler_SaiseiRyoku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_SaiseiRyoku( u32* numElems );
const BtlEventHandlerTable*  HAND_TOK_ADD_Hatomune( u32* numElems );
void handler_Hatomune_Check( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Hatomune_Guard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Sunakaki( u32* numElems );
void handler_Sunakaki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_MilacreSkin( u32* numElems );
void handler_MilacreSkin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Analyze( u32* numElems );
void handler_Sinuti( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_SunanoTikara( u32* numElems );
void handler_SunanoTikara( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_DarumaMode( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_DarumaMode_IekiFixed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_DarumaMode( u32* numElems );
bool handler_Surinuke_SkipCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, BtlEventFactorType factorType, BtlEventType eventType, u16 subID, u8 dependID );
void handler_Surinuke_Start( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Surinuke_End( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Surinuke_MigawariThrew( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Surinuke( u32* numElems );
const BtlEventHandlerTable*  HAND_TOK_ADD_JisinKajou( u32* numElems );
void handler_JisinKajou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_SeiginoKokoro( u32* numElems );
void handler_SeiginoKokoro( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Bibiri( u32* numElems );
void handler_Bibiri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Miira( u32* numElems );
void handler_Miira( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Sousyoku_CheckNoEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Sousyoku( u32* numElems );
void handler_ItazuraGokoro( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void  handler_ItazuraGokoro_WazaParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_ItazuraGokoro( u32* numElems );
void handler_MagicMirror_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MagicMirror_Wait( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MagicMirror_Reflect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_MagicMirror( u32* numElems );
void handler_Syuukaku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Syuukaku( u32* numElems );
const BtlEventHandlerTable*  HAND_TOK_ADD_HeavyMetal( u32* numElems );
void handler_HeavyMetal( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_LightMetal( u32* numElems );
void handler_LightMetal( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Amanojaku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Amanojaku( u32* numElems );
void handler_Kinchoukan_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Kinchoukan_RotationIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
bool handler_Kinchoukan_SkipCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, BtlEventFactorType factorType, BtlEventType eventType, u16 subID, u8 pokeID );
const BtlEventHandlerTable*  HAND_TOK_ADD_Kinchoukan( u32* numElems );
void handler_Hensin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Kawarimono( u32* numElems );
const BtlEventHandlerTable*  HAND_TOK_ADD_Illusion( u32* numElems );
void handler_Illusion_Damage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Illusion_Ieki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Illusion_ChangeTok( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void common_IllusionBreak( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID );
const BtlEventHandlerTable*  HAND_TOK_ADD_GoodLuck( u32* numElems );
void handler_GoodLuck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_MentalVeil( u32* numElems );
void handler_MentalVeil_Check( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MentalVeil_Failed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void common_FriendSickFailed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work, u16 strID );
const BtlEventHandlerTable*  HAND_TOK_ADD_FlowerVeil( u32* numElems );
void handler_FlowerVeil_Check( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_FlowerVeil_CheckInemuri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_FlowerVeil_Guard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_FlowerVeil_SickCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_FlowerVeil_SickFailed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
bool common_IsFlowerVeilTarget( ServerFlow* flowWk, u8 pokeID, u8 targetPokeID );
const BtlEventHandlerTable*  HAND_TOK_ADD_HooBukuro( u32* numElems );
void handler_Hoobukuro( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_HengenZizai( u32* numElems );
void handler_HengenZizai( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_BattleSwitch( u32* numElems );
void  handler_BattleSwitch( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_DarkAura( u32* numElems );
void  handler_DarkAura_MemberIN( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void  handler_DarkAura( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_FairyAura( u32* numElems );
void  handler_FairyAura_MemberIN( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void  handler_FairyAura( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_AuraBreak( u32* numElems );
void  handler_AuraBreak_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void  handler_AuraBreak( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_GanjouAgo( u32* numElems );
void  handler_GanjouAgo( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_FurCoat( u32* numElems );
void  handler_FurCoat( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_KusaNoKegawa( u32* numElems );
void  handler_KusaNoKegawa( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_NumeNume( u32* numElems );
void  handler_NumeNume( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_KataiTume( u32* numElems );
void  handler_KataiTume( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void common_Skin_ChangeType( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work,  pml::PokeType type );
void common_Skin_WazaPow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work, pml::PokeType type );
const BtlEventHandlerTable*  HAND_TOK_ADD_SkySkin( u32* numElems );
void  handler_SkySkin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void  handler_SkySkin_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_FairySkin( u32* numElems );
void  handler_FairySkin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void  handler_FairySkin_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_FreezSkin( u32* numElems );
void  handler_FreezSkin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void  handler_FreezSkin_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void  handler_SkinEndCommon( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_MegaLauncher( u32* numElems );
void handler_MegaLauncher_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MegaLauncher_Recover( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_HayateNoTsubasa( u32* numElems );
void handler_HayateNoTsubasa( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_SweetVeil( u32* numElems );
const BtlEventHandlerTable*  HAND_TOK_ADD_Boudan( u32* numElems );
void handler_Boudan( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SweetVeil_PokeSick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SweetVeil_PokeSickFailed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SweetVeil_Inemuri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_OyakoAi( u32* numElems );
void handler_OyakoAi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Magician( u32* numElems );
void handler_magician_Start( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_magician( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Kyousei( u32* numElems );
void handler_kyousei_wazaSeqStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_kyousei_wazaSeqEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_kyousei( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void kyousei_commonProc( ServerFlow* flowWk, const BTL_POKEPARAM* self, const BTL_POKEPARAM* target );
const BtlEventHandlerTable*  HAND_TOK_ADD_Gyogun( u32* numElems );
void handler_Gyogun( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Gyogun_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Gyogun_TurnCheckDone( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Gyogun_IekiFixed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Gyogun_Common_ChangeForm( ServerFlow* flowWk, u8 pokeID );
const BtlEventHandlerTable*  HAND_TOK_ADD_Zikyuuryoku( u32* numElems );
void handler_Zikyuuryoku_WazaDamageReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Mizugatame( u32* numElems );
void handler_Mizugatame_WazaDamageReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Suihou( u32* numElems );
void handler_Suihou_AttackerPower( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Yukikaki( u32* numElems );
void handler_Yukikaki_CalcAgility( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Haganetukai( u32* numElems );
void handler_Haganetukai_AttackerPower( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_UruoiVoice( u32* numElems );
void handler_UruoiVoice_WazaParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_HealingShift( u32* numElems );
void handler_HealingShift_GetWazaPriority( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_ElecSkin( u32* numElems );
void  handler_ElecSkinWazaParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void  handler_ElecSkin_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_SurfTail( u32* numElems );
void  handler_SurfTail_CalcAgility( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Hitodenasi( u32* numElems );
void handler_Hitodenasi_CriticalCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Enkaku( u32* numElems );
void  handler_Enkaku_WazaParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Zyoounoigen( u32* numElems );
void handler_Zyoounoigen_WazaExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_MohuMohu( u32* numElems );
void handler_MohuMohu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_TOK_ADD_Battery( u32* numElems );
void handler_Battery_WazaPower( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_TOK_ADD_Receiver( u32* numElems );
void handler_Receiver_DeadAfter( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_TOK_ADD_TobidasuNakami( u32* numElems );
void tobidasuNakami_RegisterDamage( ServerFlow* flowWk, u8 pokeID, int* work );
void handler_TobidasuNakami_DamageProcStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_TobidasuNakami_IchigekiGuard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_TobidasuNakami_WazaDamageReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_TOK_ADD_LimitShield( u32* numElems );
void handler_LimitShield_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_LimitShield( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_LimitSheild_SickCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void LimitShield_ChangeForm( ServerFlow* flowWk, u8 pokeID );
const BtlEventHandlerTable* HAND_TOK_ADD_SwarmChange( u32* numElems );
void handler_SwarmChange( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Gyakuzyou( u32* numElems );
void handler_Gyakuzyou_DamegeProcStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Gyakuzyou_IchigekiCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
bool gyakuzyou_isEnoughHP( ServerFlow* flowWk, u8 pokeID );
void handler_Gyakuzyou_EndHitReal( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
bool common_CheckTarget( u8 checkPokeID );
const BtlEventHandlerTable* HAND_TOK_ADD_SoulHeart( u32* numElems );
void handler_SoulHeart_DeadAfter( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_TOK_ADD_Odoriko( u32* numElems );
void handler_Odoriko_WazaSeqStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Odoriko_ExecuteEffective( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Odoriko_WazaSeqEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
BtlPokePos odoriko_GetTargetPos( ServerFlow* flowWk, u8 odorikoPokeID, u8 attackPokeID );
const BtlEventHandlerTable* HAND_TOK_ADD_Husyoku( u32* numElems );
void handler_Husyoku_CheckAddSickFailStdSkip( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_ElecMaker( u32* numElems );
void handler_ElecMaker_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_PhychoMaker( u32* numElems );
void handler_PhychoMaker_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_MistMaker( u32* numElems );
void handler_MistMaker_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_GrassMaker( u32* numElems );
void handler_GrassMaker_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_common_GroundMaker( ServerFlow* flowWk, u8 pokeID, BtlGround ground, u16 strID );
const BtlEventHandlerTable*  HAND_TOK_ADD_Nigegosi( u32* numElems );
void handler_Nigegosi_DamegeProcStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Nigegosi_DamegeProcEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Nigegosi_EndHit( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Nigegosi_BeforeIchigeki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Nigegosi_SimpleDamageBefore( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Nigegosi_SimpleDamageAfter( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void nigegosi_CheckBeforeDamage( ServerFlow* flowWk, u8 pokeID, int* work );
void nigegosi_AfterDamage_Full( ServerFlow* flowWk, u8 pokeID, int* work );
bool nigegosi_AfterDamage_shouldEffect( ServerFlow* flowWk, u8 pokeID, int* work );
void nigegosi_AfterDamage_Effect( ServerFlow* flowWk, u8 pokeID );
const BtlEventHandlerTable*  HAND_TOK_ADD_UltraForce( u32* numElems );
void handler_UltraForce( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
WazaRankEffect ultraForce_GetEffectRankType( ServerFlow* flowWk, u8 pokeID );
const BtlEventHandlerTable*  HAND_TOK_ADD_Harikomi( u32* numElems );
void handler_Harikomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_KizunaHenge( u32* numElems );
void handler_KizunaHenge_DamageProcEndHitL2( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KizunaHenge_HitCount( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KizunaHenge_WazaPowerBase( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_Bakenokawa( u32* numElems );
void handler_Bakenokawa_IchigekiGuard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Bakenokawa_WazaDamageProc( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Bakenokawa_WazaDamageReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Bakenokawa_FixConfDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Bakenokawa_ConfDamageReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
bool bakenokawa_CheckDamageGuard( ServerFlow* flowWk, u8 pokeID, int* work );
void bakenokawa_ChangeForm( ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_TOK_ADD_ZettaiNemuri( u32* numElems );
void handler_ZettaiNemuri_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_ZettaiNemuri_AddSickCheckFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
bool bakenokawa_CheckEnable( const BTL_POKEPARAM& poke );
const BtlEventHandlerTable* HAND_TOK_ADD_BrainPrism(u32* numElems);
void handler_BrainPrism(BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work);




namespace 
{
  /**
   * @brief 静的アサートチェック
   * @note 直接 STATIC_ASSERT を記述すると警告が出てしまうため、この関数経由でチェックする。
   */
  void StaticAssert()
  {
    STATIC_ASSERT( NUM_WIDX <= EVENT_HANDLER_WORK_ELEMS );
  }
}


//=============================================================================================
/**
 * とくせいハンドラ１件登録
 *
 * @param   bpp
 *
 * @retval  BTL_EVENT_FACTOR*
 */
//=============================================================================================
BTL_EVENT_FACTOR*  Add( const BTL_POKEPARAM* bpp )
{
  StaticAssert();

  typedef const BtlEventHandlerTable* (*pTokuseiEventAddFunc)( u32* );

  static const struct {
    TokuseiNo             tokusei;
    pTokuseiEventAddFunc  func;
  }funcTbl[] = {
    { TOKUSEI_IKAKU,            HAND_TOK_ADD_Ikaku         },
    { TOKUSEI_KURIABODHI,       HAND_TOK_ADD_ClearBody     },
    { TOKUSEI_SIROIKEMURI,      HAND_TOK_ADD_ClearBody     }, // しろいけむり=クリアボディと等価
    { TOKUSEI_SEISINRYOKU,      HAND_TOK_ADD_Seisinryoku   },
    { TOKUSEI_HUKUTUNOKOKORO,   HAND_TOK_ADD_Fukutsuno     },
    { TOKUSEI_ATUISIBOU,        HAND_TOK_ADD_AtuiSibou     },
    { TOKUSEI_KAIRIKIBASAMI,    HAND_TOK_ADD_KairikiBasami },
    { TOKUSEI_TIKARAMOTI,       HAND_TOK_ADD_Tikaramoti    },
    { TOKUSEI_YOGAPAWAA,        HAND_TOK_ADD_Tikaramoti    }, // ヨガパワー = ちからもちと等価
    { TOKUSEI_IROMEGANE,        HAND_TOK_ADD_Iromegane     },
    { TOKUSEI_KASOKU,           HAND_TOK_ADD_Kasoku        },
    { TOKUSEI_MOUKA,            HAND_TOK_ADD_Mouka         },
    { TOKUSEI_GEKIRYUU,         HAND_TOK_ADD_Gekiryu       },
    { TOKUSEI_SINRYOKU,         HAND_TOK_ADD_Sinryoku      },
    { TOKUSEI_MUSINOSIRASE,     HAND_TOK_ADD_MusinoSirase  },
    { TOKUSEI_KONZYOU,          HAND_TOK_ADD_Konjou        },
    { TOKUSEI_SUKIRURINKU,      HAND_TOK_ADD_SkillLink     },
    { TOKUSEI_SURUDOIME,        HAND_TOK_ADD_Surudoime     },
    { TOKUSEI_TANZYUN,          HAND_TOK_ADD_Tanjun        },
    { TOKUSEI_HAADOROKKU,       HAND_TOK_ADD_HardRock      },
    { TOKUSEI_FIRUTAA,          HAND_TOK_ADD_HardRock      }, // フィルター = ハードロックと等価
    { TOKUSEI_HUSIGINAUROKO,    HAND_TOK_ADD_FusiginaUroko },
    { TOKUSEI_TOUSOUSIN,        HAND_TOK_ADD_Tousousin     },
    { TOKUSEI_RIIHUGAADO,       HAND_TOK_ADD_ReafGuard     },
    { TOKUSEI_AMEHURASI,        HAND_TOK_ADD_Amefurasi     },
    { TOKUSEI_HIDERI,           HAND_TOK_ADD_Hideri        },
    { TOKUSEI_SUNAOKOSI,        HAND_TOK_ADD_Sunaokosi     },
    { TOKUSEI_YUKIHURASI,       HAND_TOK_ADD_Yukifurasi    },
    { TOKUSEI_EAROKKU,          HAND_TOK_ADD_AirLock       },
    { TOKUSEI_NOOTENKI,         HAND_TOK_ADD_AirLock       }, // ノーてんき = エアロックと等価
    { TOKUSEI_TEKUNISYAN,       HAND_TOK_ADD_Technician    },
    { TOKUSEI_DONKAN,           HAND_TOK_ADD_Donkan        },
    { TOKUSEI_URUOIBODHI,       HAND_TOK_ADD_UruoiBody     },
    { TOKUSEI_POIZUNHIIRU,      HAND_TOK_ADD_PoisonHeal    },
    { TOKUSEI_AISUBODHI,        HAND_TOK_ADD_IcoBody       },
    { TOKUSEI_AMEUKEZARA,       HAND_TOK_ADD_AmeukeZara    },
    { TOKUSEI_RINPUN,           HAND_TOK_ADD_Rinpun        },
    { TOKUSEI_TEKIOURYOKU,      HAND_TOK_ADD_Tekiouryoku   },
    { TOKUSEI_TENNOMEGUMI,      HAND_TOK_ADD_TennoMegumi   },
    { TOKUSEI_SANPAWAA,         HAND_TOK_ADD_SunPower      },
    { TOKUSEI_SUISUI,           HAND_TOK_ADD_Suisui        },
    { TOKUSEI_YOURYOKUSO,       HAND_TOK_ADD_Youryokuso    },
    { TOKUSEI_DAPPI,            HAND_TOK_ADD_Dappi         },
    { TOKUSEI_TIDORIASI,        HAND_TOK_ADD_Tidoriasi     },
    { TOKUSEI_HAYAASI,          HAND_TOK_ADD_Hayaasi       },
    { TOKUSEI_HARIKIRI,         HAND_TOK_ADD_Harikiri      },
    { TOKUSEI_KABUTOAAMAA,      HAND_TOK_ADD_KabutoArmor   },
    { TOKUSEI_SHERUAAMAA,       HAND_TOK_ADD_KabutoArmor   }, // シェルアーマー=カブトアーマーと等価
    { TOKUSEI_KYOUUN,           HAND_TOK_ADD_Kyouun        },
    { TOKUSEI_IKARINOTUBO,      HAND_TOK_ADD_IkarinoTubo   },
    { TOKUSEI_SUNAIPAA,         HAND_TOK_ADD_Sniper        },
    { TOKUSEI_TETUNOKOBUSI,     HAND_TOK_ADD_TetunoKobusi  },
    { TOKUSEI_HUKUGAN,          HAND_TOK_ADD_Fukugan       },
    { TOKUSEI_ISIATAMA,         HAND_TOK_ADD_Isiatama      },
    { TOKUSEI_SUTEMI ,          HAND_TOK_ADD_Sutemi        },
    { TOKUSEI_SEIDENKI ,        HAND_TOK_ADD_Seidenki      },
    { TOKUSEI_DOKUNOTOGE,       HAND_TOK_ADD_DokunoToge    },
    { TOKUSEI_HONOONOKARADA,    HAND_TOK_ADD_HonoNoKarada  },
    { TOKUSEI_HOUSI,            HAND_TOK_ADD_Housi         },
    { TOKUSEI_PURASU,           HAND_TOK_ADD_Plus          },
    { TOKUSEI_MAINASU,          HAND_TOK_ADD_Plus          }, // 今作よりプラスとマイナスは等価
    { TOKUSEI_MEROMEROBODHI,    HAND_TOK_ADD_MeromeroBody  },
    { TOKUSEI_SUNAGAKURE ,      HAND_TOK_ADD_Sunagakure    },
    { TOKUSEI_YUKIGAKURE ,      HAND_TOK_ADD_Yukigakure    },
    { TOKUSEI_TOREESU,          HAND_TOK_ADD_Trace         },
    { TOKUSEI_NOOMARUSUKIN,     HAND_TOK_ADD_NormalSkin    },
    { TOKUSEI_SAMEHADA,         HAND_TOK_ADD_Samehada      },
    { TOKUSEI_SIZENKAIHUKU,     HAND_TOK_ADD_SizenKaifuku  },
    { TOKUSEI_SINKURO,          HAND_TOK_ADD_Syncro        },
    { TOKUSEI_DAUNROODO,        HAND_TOK_ADD_DownLoad      },
    { TOKUSEI_GANZYOU,          HAND_TOK_ADD_Ganjou        },
    { TOKUSEI_TAINETU,          HAND_TOK_ADD_Tainetu       },
    { TOKUSEI_TENNEN,           HAND_TOK_ADD_Tennen        },
    { TOKUSEI_KANSOUHADA,       HAND_TOK_ADD_Kansouhada    },
    { TOKUSEI_TIKUDEN,          HAND_TOK_ADD_Tikuden       },
    { TOKUSEI_TYOSUI,           HAND_TOK_ADD_Tyosui        },
    { TOKUSEI_DENKIENZIN,       HAND_TOK_ADD_DenkiEngine   },
    { TOKUSEI_ZYUUNAN,          HAND_TOK_ADD_Juunan        },
    { TOKUSEI_HUMIN,            HAND_TOK_ADD_Fumin         },
    { TOKUSEI_YARUKI,           HAND_TOK_ADD_Fumin         }, // やるき=ふみんと等価
    { TOKUSEI_MAIPEESU,         HAND_TOK_ADD_MyPace        },
    { TOKUSEI_MAGUMANOYOROI,    HAND_TOK_ADD_MagumaNoYoroi },
    { TOKUSEI_MIZUNOBEERU,      HAND_TOK_ADD_MizuNoBale    },
    { TOKUSEI_MENEKI,           HAND_TOK_ADD_Meneki        },
    { TOKUSEI_KIMOTTAMA,        HAND_TOK_ADD_Kimottama     },
    { TOKUSEI_BOUON,            HAND_TOK_ADD_Bouon         },
    { TOKUSEI_HUYUU,            HAND_TOK_ADD_Fuyuu         },
    { TOKUSEI_HURAWAAGIHUTO,    HAND_TOK_ADD_FlowerGift    },
    { TOKUSEI_MORAIBI,          HAND_TOK_ADD_Moraibi       },
    { TOKUSEI_YOTIMU,           HAND_TOK_ADD_Yotimu        },
    { TOKUSEI_KIKENYOTI,        HAND_TOK_ADD_KikenYoti     },
    { TOKUSEI_OMITOOSI,         HAND_TOK_ADD_Omitoosi      },
    { TOKUSEI_YUUBAKU,          HAND_TOK_ADD_Yuubaku       },
    { TOKUSEI_NIGEASI,          HAND_TOK_ADD_Nigeasi       },
    { TOKUSEI_HENSYOKU,         HAND_TOK_ADD_Hensyoku      },
    { TOKUSEI_KATAYABURI,       HAND_TOK_ADD_Katayaburi    },
    { TOKUSEI_NAMAKE,           HAND_TOK_ADD_Namake        },
    { TOKUSEI_HIRAISIN,         HAND_TOK_ADD_Hiraisin      },
    { TOKUSEI_YOBIMIZU,         HAND_TOK_ADD_Yobimizu      },
    { TOKUSEI_SUROOSUTAATO,     HAND_TOK_ADD_SlowStart     },
    { TOKUSEI_SIMERIKE,         HAND_TOK_ADD_Simerike      },
    { TOKUSEI_HUSIGINAMAMORI,   HAND_TOK_ADD_FusiginaMamori},
    { TOKUSEI_ATODASI,          HAND_TOK_ADD_Atodasi       },
    { TOKUSEI_TENKIYA,          HAND_TOK_ADD_Tenkiya       },
    { TOKUSEI_KYUUBAN,          HAND_TOK_ADD_Kyuuban       },
    { TOKUSEI_HEDOROEKI,        HAND_TOK_ADD_HedoroEki     },
    { TOKUSEI_BUKIYOU,          HAND_TOK_ADD_Bukiyou       },
    { TOKUSEI_NENTYAKU,         HAND_TOK_ADD_Nenchaku      },
    { TOKUSEI_PURESSYAA,        HAND_TOK_ADD_Pressure      },
    { TOKUSEI_MAZIKKUGAADO,     HAND_TOK_ADD_MagicGuard    },
    { TOKUSEI_NAITOMEA,         HAND_TOK_ADD_Nightmare     },
    { TOKUSEI_MONOHIROI,        HAND_TOK_ADD_Monohiroi     },
    { TOKUSEI_KARUWAZA,         HAND_TOK_ADD_Karuwaza      },
    { TOKUSEI_AKUSYUU,          HAND_TOK_ADD_Akusyuu       },
    { TOKUSEI_KAGEHUMI,         HAND_TOK_ADD_Kagefumi      },
    { TOKUSEI_ARIZIGOKU,        HAND_TOK_ADD_Arijigoku     },
    { TOKUSEI_ZIRYOKU,          HAND_TOK_ADD_Jiryoku       },
    { TOKUSEI_WARUITEGUSE,      HAND_TOK_ADD_WaruiTeguse   },
    { TOKUSEI_TIKARAZUKU,       HAND_TOK_ADD_Tikarazuku    },
    { TOKUSEI_MAKENKI,          HAND_TOK_ADD_Makenki       },
    { TOKUSEI_YOWAKI,           HAND_TOK_ADD_Yowaki        },
    { TOKUSEI_MARUTISUKEIRU,    HAND_TOK_ADD_MultiScale    },
    { TOKUSEI_HEVHIMETARU,      HAND_TOK_ADD_HeavyMetal    }, // ヘヴィメタル
    { TOKUSEI_RAITOMETARU,      HAND_TOK_ADD_LightMetal    }, // ライトメタル
    { TOKUSEI_AMANOZYAKU,       HAND_TOK_ADD_Amanojaku     }, // あまのじゃく
    { TOKUSEI_KINTYOUKAN,       HAND_TOK_ADD_Kinchoukan    }, // きんちょうかん
    { TOKUSEI_NOROWAREBODHI,    HAND_TOK_ADD_NorowareBody  }, // のろわれボディ
    { TOKUSEI_IYASINOKOKORO,    HAND_TOK_ADD_IyasiNoKokoro }, // いやしのこころ
    { TOKUSEI_HURENDOGAADO,     HAND_TOK_ADD_FriendGuard   }, // フレンドガード
    { TOKUSEI_KUDAKERUYOROI,    HAND_TOK_ADD_KudakeruYoroi }, // くだけるよろい
    { TOKUSEI_DOKUBOUSOU,       HAND_TOK_ADD_Dokubousou    }, // どくぼうそう
    { TOKUSEI_NETUBOUSOU,       HAND_TOK_ADD_Netubousou    }, // ねつぼうそう
    { TOKUSEI_SYUUKAKU,         HAND_TOK_ADD_Syuukaku      }, // しゅうかく
    { TOKUSEI_TEREPASII,        HAND_TOK_ADD_Telepassy     }, // テレパシー（旧あうんのいき）
    { TOKUSEI_MURAKKE,          HAND_TOK_ADD_Murakke       }, // ムラっけ
    { TOKUSEI_BOUZIN,           HAND_TOK_ADD_Boujin        }, // ぼうじん
    { TOKUSEI_DOKUSYU,          HAND_TOK_ADD_Dokusyu       }, // どくしゅ
    { TOKUSEI_SAISEIRYOKU,      HAND_TOK_ADD_SaiseiRyoku   }, // さいせいりょく
    { TOKUSEI_HATOMUNE,         HAND_TOK_ADD_Hatomune      }, // はとむね
    { TOKUSEI_SUNAKAKI,         HAND_TOK_ADD_Sunakaki      }, // すなかき
    { TOKUSEI_MIRAKURUSUKIN,    HAND_TOK_ADD_MilacreSkin   }, // ミラクルスキン
    { TOKUSEI_ANARAIZU,         HAND_TOK_ADD_Analyze       }, // アナライズ
    { TOKUSEI_IRYUUZYON,        HAND_TOK_ADD_Illusion      }, // イリュージョン
    { TOKUSEI_KAWARIMONO,       HAND_TOK_ADD_Kawarimono    }, // かわりもの
    { TOKUSEI_SURINUKE,         HAND_TOK_ADD_Surinuke      }, // すりぬけ
    { TOKUSEI_MIIRA,            HAND_TOK_ADD_Miira         }, // ミイラ
    { TOKUSEI_ZISINKAZYOU,      HAND_TOK_ADD_JisinKajou    }, // じしんかじょう
    { TOKUSEI_SEIGINOKOKORO,    HAND_TOK_ADD_SeiginoKokoro }, // せいぎのこころ
    { TOKUSEI_BIBIRI,           HAND_TOK_ADD_Bibiri        }, // びびり
    { TOKUSEI_MAZIKKUMIRAA,     HAND_TOK_ADD_MagicMirror   }, // マジックミラー
    { TOKUSEI_SOUSYOKU,         HAND_TOK_ADD_Sousyoku      }, // そうしょく
    { TOKUSEI_ITAZURAGOKORO,    HAND_TOK_ADD_ItazuraGokoro }, // いたずらごころ
    { TOKUSEI_SUNANOTIKARA,     HAND_TOK_ADD_SunanoTikara  }, // すなのちから
    { TOKUSEI_TETUNOTOGE,       HAND_TOK_ADD_Samehada      }, // てつのとげ = さめはだ
    { TOKUSEI_DARUMAMOODO,      HAND_TOK_ADD_DarumaMode    }, // ダルマモード
    { TOKUSEI_SYOURINOHOSI,     HAND_TOK_ADD_GoodLuck      }, // しょうりのほし
    { TOKUSEI_TAABOBUREIZU,     HAND_TOK_ADD_Katayaburi    }, // ターボブレイズ
    { TOKUSEI_TERABORUTEEZI,    HAND_TOK_ADD_Katayaburi    }, // テラボルテージ
    { TOKUSEI_AROMABEERU,       HAND_TOK_ADD_MentalVeil    }, // アロマベール
    { TOKUSEI_HURAWAABEERU,     HAND_TOK_ADD_FlowerVeil     }, // フラワーベール
    { TOKUSEI_HOOBUKURO,        HAND_TOK_ADD_HooBukuro      }, // ほおぶくろ
    { TOKUSEI_HENGENZIZAI,      HAND_TOK_ADD_HengenZizai    }, // へんげんじざい
    { TOKUSEI_BATORUSUITTI,     HAND_TOK_ADD_BattleSwitch   }, // バトルスイッチ
    { TOKUSEI_OORABUREIKU,      HAND_TOK_ADD_AuraBreak      }, // オーラブレーク
    { TOKUSEI_DAAKUOORA,        HAND_TOK_ADD_DarkAura       }, // ダークオーラ
    { TOKUSEI_FEARIIOORA,       HAND_TOK_ADD_FairyAura      }, // フェアリーオーラ
    { TOKUSEI_GANZYOUAGO,       HAND_TOK_ADD_GanjouAgo      }, // がんじょうあご
    { TOKUSEI_FAAKOOTO,         HAND_TOK_ADD_FurCoat        }, // ファーコート
    { TOKUSEI_KUSANOKEGAWA,     HAND_TOK_ADD_KusaNoKegawa   }, // くさのけがわ
    { TOKUSEI_NUMENUME,         HAND_TOK_ADD_NumeNume       }, // ぬめぬめ
    { TOKUSEI_KATAITUME,        HAND_TOK_ADD_KataiTume      }, // かたいつめ
    { TOKUSEI_SUKAISUKIN,       HAND_TOK_ADD_SkySkin        }, // スカイスキン
    { TOKUSEI_FEARIISUKIN,      HAND_TOK_ADD_FairySkin      }, // フェアリースキン
    { TOKUSEI_HURIIZUSUKIN,     HAND_TOK_ADD_FreezSkin      }, // フリーズスキン
    { TOKUSEI_MEGARANTYAA,      HAND_TOK_ADD_MegaLauncher   }, // メガランチャー
    { TOKUSEI_HAYATENOTUBASA,   HAND_TOK_ADD_HayateNoTsubasa}, // はやてのつばさ
    { TOKUSEI_SUIITOBEERU,      HAND_TOK_ADD_SweetVeil      }, // スイートベール
    { TOKUSEI_KATIKI,           HAND_TOK_ADD_Katiki         }, // かちき
    { TOKUSEI_BOUDAN,           HAND_TOK_ADD_Boudan         }, // ぼうだん
    { TOKUSEI_OYAKOAI,          HAND_TOK_ADD_OyakoAi        }, // おやこあい
    { TOKUSEI_MAZISYAN,         HAND_TOK_ADD_Magician       }, // マジシャン
    { TOKUSEI_KYOUSEI,          HAND_TOK_ADD_Kyousei        }, // きょうせい
    { TOKUSEI_HAZIMARINOUMI,    HAND_TOK_ADD_Hajimarinoumi  }, // はじまりのうみ
    { TOKUSEI_OWARINODAITI,     HAND_TOK_ADD_Owarinodaiti   }, // おわりのだいち
    { TOKUSEI_DERUTASUTORIIMU,  HAND_TOK_ADD_DeltaStream    }, // デルタストリーム
    { TOKUSEI_GYOGUN,           HAND_TOK_ADD_Gyogun         }, // ぎょぐん
    { TOKUSEI_ZIKYUURYOKU,      HAND_TOK_ADD_Zikyuuryoku    }, // じきゅうりょく
    { TOKUSEI_MIZUGATAME,       HAND_TOK_ADD_Mizugatame     }, // みずがため
    { TOKUSEI_SUIHOU,           HAND_TOK_ADD_Suihou         }, // すいほう
    { TOKUSEI_YUKIKAKI,         HAND_TOK_ADD_Yukikaki       }, // ゆきかき
    { TOKUSEI_HAGANETUKAI,      HAND_TOK_ADD_Haganetukai    }, // はがねつかい
    { TOKUSEI_URUOIBOISU,       HAND_TOK_ADD_UruoiVoice     }, // うるおいボイス
    { TOKUSEI_HIIRINGUSIHUTO,   HAND_TOK_ADD_HealingShift   }, // ヒーリングシフト
    { TOKUSEI_EREKISUKIN,       HAND_TOK_ADD_ElecSkin       }, // エレキスキン
    { TOKUSEI_SAAHUTEERU,       HAND_TOK_ADD_SurfTail       }, // サーフテール
    { TOKUSEI_HITODENASI,       HAND_TOK_ADD_Hitodenasi     }, // ひとでなし
    { TOKUSEI_ENKAKU,           HAND_TOK_ADD_Enkaku         }, // えんかく
    { TOKUSEI_ZYOOUNOIGEN,      HAND_TOK_ADD_Zyoounoigen    }, // じょおうのいげん
    { TOKUSEI_BIBIDDOBODHI,     HAND_TOK_ADD_Zyoounoigen    }, // ビビッドボディ = じょおうのいげん
    { TOKUSEI_MOHUMOHU,         HAND_TOK_ADD_MohuMohu       }, // もふもふ
    { TOKUSEI_KAARIIHEAA,       HAND_TOK_ADD_NumeNume       }, // カーリーヘアー = ぬめぬめ
    { TOKUSEI_BATTERII,         HAND_TOK_ADD_Battery        }, // バッテリー
    { TOKUSEI_RESIIBAA,         HAND_TOK_ADD_Receiver       }, // レシーバー
    { TOKUSEI_KAGAKUNOTIKARA,   HAND_TOK_ADD_Receiver       }, // かがくのちから = レシーバー
    { TOKUSEI_TOBIDASUNAKAMI,   HAND_TOK_ADD_TobidasuNakami }, // とびだすなかみ
    { TOKUSEI_RIMITTOSIIRUDO,   HAND_TOK_ADD_LimitShield    }, // リミットシールド
    { TOKUSEI_SUWAAMUTHENZI,    HAND_TOK_ADD_SwarmChange    }, // スワームチェンジ
    { TOKUSEI_GYAKUZYOU,        HAND_TOK_ADD_Gyakuzyou      }, // ぎゃくじょう
    { TOKUSEI_SOURUHAATO,       HAND_TOK_ADD_SoulHeart      }, // ソウルハート
    { TOKUSEI_ODORIKO,          HAND_TOK_ADD_Odoriko        }, // おどりこ
    { TOKUSEI_HUSYOKU,          HAND_TOK_ADD_Husyoku        }, // ふしょく
    { TOKUSEI_EREKIMEIKAA,      HAND_TOK_ADD_ElecMaker      }, // エレキメイカー
    { TOKUSEI_SAIKOMEIKAA,      HAND_TOK_ADD_PhychoMaker    }, // サイコメイカー
    { TOKUSEI_MISUTOMEIKAA,     HAND_TOK_ADD_MistMaker      }, // ミストメイカー
    { TOKUSEI_GURASUMEIKAA,     HAND_TOK_ADD_GrassMaker     }, // グラスメイカー
    { TOKUSEI_NIGEGOSI,         HAND_TOK_ADD_Nigegosi       }, // にげごし
    { TOKUSEI_KIKIKAIHI,        HAND_TOK_ADD_Nigegosi       }, // ききかいひ = にげごし
    { TOKUSEI_URUTORAFOOSU,     HAND_TOK_ADD_UltraForce     }, // ウルトラフォース
    { TOKUSEI_HARIKOMI,         HAND_TOK_ADD_Harikomi       }, // はりこみ
    { TOKUSEI_KIZUNAHENGE,      HAND_TOK_ADD_KizunaHenge    }, // きずなへんげ
    { TOKUSEI_BAKENOKAWA,       HAND_TOK_ADD_Bakenokawa     }, // ばけのかわ
    { TOKUSEI_ZETTAINEMURI,     HAND_TOK_ADD_ZettaiNemuri   }, // ぜったいねむり
    { TOKUSEI_METARUPUROTEKUTO, HAND_TOK_ADD_ClearBody      }, // メタルプロテクト = クリアボディ
    { TOKUSEI_FANTOMUGAADO,	    HAND_TOK_ADD_MultiScale     }, // ファントムガード = マルチスケイル
    { TOKUSEI_PURIZUMUAAMAA,    HAND_TOK_ADD_HardRock       }, // プリズムアーマー = ハードロック
    { TOKUSEI_BUREINPURIZUMU,   HAND_TOK_ADD_BrainPrism     }, // ブレインプリズム
  };

  {
    u16 tokusei = bpp->GetValue( BTL_POKEPARAM::BPP_TOKUSEI );

    for(u32 i=0; i<GFL_NELEMS(funcTbl); i++)
    {
      if( funcTbl[i].tokusei == tokusei )
      {
        u16 agi = calcTokHandlerSubPriority( bpp );
        u8 pokeID = bpp->GetID();
        u32 numHandlers;
        BtlEventPriority mainPri;
        const BtlEventHandlerTable* handlerTable;

        handlerTable = funcTbl[i].func( &numHandlers );
        mainPri = devideNumHandersAndPri( &numHandlers );

        return EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokusei,
                  mainPri, agi, pokeID, handlerTable, numHandlers );
      }
    }
    BTL_Printf("とくせい(%d)はハンドラが用意されていない\n", tokusei);
  }
  return NULL;
}

void Remove( const BTL_POKEPARAM* bpp )
{
  BTL_EVENT_FACTOR* factor;
  u8 pokeID = bpp->GetID();

  while( (factor = EVENT_SeekFactor(BTL_EVENT_FACTOR_TOKUSEI, pokeID)) != NULL )
  {
    // 削除禁止ワークが0でない場合削除しない（削除タイミングはそのハンドラにまかせる）
    if( EVENT_FACTOR_GetWorkValue(factor, WIDX_REMOVE_GUARD) ){
      break;
    }
    BTL_PRINT("[Hand_Tokusei] pokeID=%d, factor=%p  remove\n", pokeID, factor);
    EVENT_FACTOR_Remove( factor );
  }
}

//=============================================================================================
/**
 * 指定ポケモンの「とくせい」ハンドラを一時的に休止（ローテーション用）
 *
 * @param   bpp
 */
//=============================================================================================
void RotationSleep( const BTL_POKEPARAM* bpp )
{
  u8 pokeID = bpp->GetID();
  EVENT_SleepFactorRotation( pokeID, BTL_EVENT_FACTOR_TOKUSEI );
}

//=============================================================================================
/**
 * 指定ポケモンの「とくせい」ハンドラを復帰（ローテーション用）
 *
 * @param   bpp
 */
//=============================================================================================
void RotationWake( const BTL_POKEPARAM* bpp )
{
  u8 pokeID = bpp->GetID();
  if( EVENT_WakeFactorRotation(pokeID, BTL_EVENT_FACTOR_TOKUSEI) == false )
  {
    Add( bpp );
  }
}

//=============================================================================================
/**
 * 指定ポケモンの「とくせい」ハンドラを復帰（momiji用）
 *
 * @param   bpp
 */
//=============================================================================================
void RotationWakeNoAdd( const BTL_POKEPARAM* bpp )
{
  u8 pokeID = bpp->GetID();
  EVENT_WakeFactorRotation( pokeID, BTL_EVENT_FACTOR_TOKUSEI );
}

//=============================================================================================
/**
 * ポケモン同士の「とくせい」入れ替え
 *
 * @param   pp1
 * @param   pp2
 */
//=============================================================================================
void Swap( const BTL_POKEPARAM* pp1, const BTL_POKEPARAM* pp2 )
{
  BTL_EVENT_FACTOR *factor1, *factor2;
  u8  ID_1, ID_2;

  ID_1 = pp1->GetID();
  ID_2 = pp2->GetID();
  factor1 = EVENT_SeekFactor( BTL_EVENT_FACTOR_TOKUSEI, ID_1 );
  factor2 = EVENT_SeekFactor( BTL_EVENT_FACTOR_TOKUSEI, ID_2 );

  if( factor1 )
  {
    EVENT_FACTOR_Remove( factor1 );
  }
  if( factor2 )
  {
    EVENT_FACTOR_Remove( factor2 );
  }

  Add( pp1 );
  Add( pp2 );
}


//----------------------------------------------------------------------------------
/**
 * ハンドラ数にハンドラプライオリティの値を合わせてu32変換
 */
//----------------------------------------------------------------------------------
u32 numHandlersWithHandlerPri( u16 pri, u16 numHandlers )
{
  u32 result = (pri << 16) | (numHandlers);
  return result;
}

//----------------------------------------------------------------------------------
/**
 * ハンドラプライオリティを計算
 */
//----------------------------------------------------------------------------------
u16 calcTokHandlerSubPriority( const BTL_POKEPARAM* bpp )
{
  return bpp->GetValue_Base( BTL_POKEPARAM::BPP_AGILITY );
}

//----------------------------------------------------------------------------------
/**
 * ハンドラ登録数＆ハンドラプライオリティの合成値を元の値にバラす
 */
//----------------------------------------------------------------------------------
BtlEventPriority    devideNumHandersAndPri( u32* numHandlers )
{
  u16 mainPri = ((*numHandlers) >> 16) & 0xffff;
  if( mainPri == 0 ){
    mainPri = BTL_EVPRI_TOKUSEI_DEFAULT;
  }
  (*numHandlers) &= 0xffff;
  return (BtlEventPriority)(mainPri);
}

//----------------------------------------------------------------------------------
/**
 * ランダム発動とくせいの共通チェックルーチン
 */
//----------------------------------------------------------------------------------
bool isExePer( ServerFlow* flowWk, u8 per )
{
  if( calc::IsOccurPer(per) ){
    return true;
  }
  if( flowWk->Hnd_GetDebugFlag(BTL_DEBUGFLAG_MUST_TOKUSEI) ){
    return true;
  }
  return false;
}


//------------------------------------------------------------------------------
/**
 *  とくせい「いかく」
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Ikaku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,            handler_Ikaku_MemberIn },
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_Ikaku_MemberIn },
  };

  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ポケモンが出場した時のハンドラ
void handler_Ikaku_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( myHandle );
  GFL_UNUSED( work );

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BtlPokePos myPos = flowWk->Hnd_GetExistFrontPokePos( pokeID );
    BtlExPos   expos = EXPOS_MAKE( BTL_EXPOS_AREA_ENEMY, myPos );

    u8*  pokeIDAry = reinterpret_cast<u8*>(flowWk->Hnd_GetTmpWork( BTL_POS_NUM ));
    u8   pokeCnt = flowWk->Hnd_ExpandPokeID( expos, pokeIDAry );

    if( pokeCnt )
    {
      flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );

      {
        BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT*>
                                                (flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
          param->rankType = pml::wazadata::RANKEFF_ATTACK;
          param->rankVolume = -1;
          param->cause = RANKEFFECT_CAUSE_IKAKU;
          param->fAlmost = true;
          param->poke_cnt = pokeCnt;
          for(u32 i=0; i<pokeCnt; ++i){
            param->pokeID[i] = pokeIDAry[i];
          }
        flowWk->HANDEX_Pop( param );
      }
      flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
    }
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「せいしんりょく」
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Seisinryoku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_SHRINK_CHECK, handler_Seisinryoku },    // ひるみ発生チェックハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ひるみ発生チェックハンドラ
void handler_Seisinryoku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 自分がひるまされる対象
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // 強制失敗フラグをＯＮ
    EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
  }
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ふくつのこころ」
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Fukutsuno( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FAIL, handler_FukutsunoKokoro },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ワザ出し失敗確定
void handler_FukutsunoKokoro( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // ワザ失敗理由=ひるみ, 対象ポケ=自分で発動
  if( (EVENTVAR_GetValue(BTL_EVAR_FAIL_CAUSE) == ServerFlow::SV_WAZAFAIL_SHRINK)
  &&  (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  ){
    BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
      param->header.tokwin_flag = true;
      param->rankType = pml::wazadata::RANKEFF_AGILITY;
      param->rankVolume = 1;
      param->fAlmost = false;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
    flowWk->HANDEX_Pop( param );
  }
}
//------------------------------------------------------------------------------
/**
 *  とくせい「あついしぼう」
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_AtuiSibou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_AtuiSibou },
  };

  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// 攻撃側の攻撃値決定
void handler_AtuiSibou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // ダメージ対象が自分
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  {
    pml::PokeType  type = EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
    // ワザタイプが氷か炎
    if( (type == POKETYPE_KOORI) || (type == POKETYPE_HONOO) )
    {
      EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5) );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ちからもち」
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Tikaramoti( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_Tikaramoti },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// 攻撃側の攻撃値決定
void handler_Tikaramoti( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 攻撃するのが自分
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  {
    // ダメージタイプが物理
    if( EVENTVAR_GetValue( BTL_EVAR_DAMAGE_TYPE ) == pml::wazadata::DMG_PHYSIC )
    {
      EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  とくせい「すいすい」
 */
//------------------------------------------------------------------------------
// すばやさ計算ハンドラ
void handler_Suisui( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // 雨のとき、すばやさ２倍
    if( calc::IsRainWeather(flowWk->Hnd_GetWeather()) )//@バトル変更箇所
    {
      EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Suisui( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY, handler_Suisui },   // すばやさ計算ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ようりょくそ」
 */
//------------------------------------------------------------------------------
// すばやさ計算ハンドラ
void handler_Youryokuso( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // はれの時、すばやさ２倍
    if( calc::IsShineWeather(flowWk->Hnd_GetWeather()) )//@バトル変更箇所
    {
      EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Youryokuso( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY, handler_Youryokuso },   // すばやさ計算ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「はやあし」
 */
//------------------------------------------------------------------------------
// すばやさ計算ハンドラ
void handler_Hayaasi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // 状態異常の時、すばやさ1.5倍＆マヒによる減衰を無視する
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->GetPokeSick() != pml::pokepara::SICK_NULL )
    {
      EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_HAYAASI_AGIRATIO );
      EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, false );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Hayaasi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY, handler_Hayaasi },    // すばやさ計算ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ちどりあし」
 */
//------------------------------------------------------------------------------
// 命中率計算ハンドラ
void handler_Tidoriasi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->CheckSick(pml::wazadata::WAZASICK_KONRAN) )
    {
      EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_CHIDORI_HITRATIO );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Tidoriasi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO, handler_Tidoriasi },    // 命中率計算ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「はりきり」
 */
//------------------------------------------------------------------------------
// 命中率計算ハンドラ
void handler_Harikiri_HitRatio( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 物理ワザは命中率減少
    if( EVENTVAR_GetValue( BTL_EVAR_DAMAGE_TYPE ) == pml::wazadata::DMG_PHYSIC )
    {
      EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_HARIKIRI_HITRATIO );
    }
  }
}
// 攻撃力計算ハンドラ
void handler_Harikiri_AtkPower( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 物理ワザは威力増加
    if( EVENTVAR_GetValue( BTL_EVAR_DAMAGE_TYPE ) == pml::wazadata::DMG_PHYSIC )
    {
      u32 power = EVENTVAR_GetValue( BTL_EVAR_POWER );
      power = calc::MulRatio( power, BTL_CALC_TOK_HARIKIRI_POWRATIO );
      EVENTVAR_RewriteValue( BTL_EVAR_POWER, power );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Harikiri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO, handler_Harikiri_HitRatio },  // 命中率計算ハンドラ
    { BTL_EVENT_ATTACKER_POWER, handler_Harikiri_AtkPower },  // 攻撃力計算ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「あとだし」
 */
//------------------------------------------------------------------------------
// すばやさ計算ハンドラ
void handler_Atodasi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);

  // 既に書き換わっているなら、何もしない
  if( EVENTVAR_GetValue( BTL_EVAR_SP_PRIORITY ) != BTL_SPPRI_DEFAULT )
  {
    return;
  }

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // @fix NMCat[189] 行動後に「せんせいのツメ」「イバンのみ」が発動する
    //「おいうち」で相手の入れ替えに割り込んだ場合など、この時すでに全ての行動が完了場合があります。
    // 行動済みなら、発動しないようにします。
    if( flowWk->Hnd_IsMyAllActDone( pokeID ) )
    {
      return;
    }

    EVENTVAR_RewriteValue( BTL_EVAR_SP_PRIORITY, BTL_SPPRI_LOW );
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Atodasi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_SP_PRIORITY,   handler_Atodasi },  // 特殊優先度チェックハンドラ
  };
  *numElems = numHandlersWithHandlerPri( BTL_EVPRI_TOKUSEI_atodasi, GFL_NELEMS(HandlerTable) );
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  とくせい「スロースタート」
 */
//------------------------------------------------------------------------------
// すばやさ計算ハンドラ
void handler_SlowStart_Agility( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( work[WIDX3] )
    {
      EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_SLOWSTART_AGIRATIO );
    }
  }
}
// 攻撃力計算ハンドラ
void handler_SlowStart_AtkPower( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( work[WIDX3] )
    {
      WazaNo waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
      if( WAZADATA_GetDamageType(waza) == pml::wazadata::DMG_PHYSIC )
      {
        EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_SLOWSTART_ATKRATIO );
      }
    }
  }
}
// メンバー入場ハンドラ（＆とくせい書き換え直後ハンドラ）
void handler_SlowStart_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    common_SlowStart_Declare( myHandle, flowWk, pokeID, work );
  }
}
/**
 *  スロースタート：宣言＆効果開始
 */
void common_SlowStart_Declare( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  work[WIDX0] = true;   // work[WIDX0] : スロースタート開始宣言フラグ
  work[WIDX1] = 0;      // work[WIDX1] : ターンカウンタ
  work[WIDX2] = false;  // work[WIDX2] : スロースタート終了宣言フラグ
  work[WIDX3] = true;   // work[3] : 効果継続中フラグ
  {
    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      param->header.tokwin_flag = true;
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_SlowStartON );
      HANDEX_STR_AddArg( &param->str, pokeID );
    flowWk->HANDEX_Pop( param );
  }
}
// ターンチェック終了ハンドラ
void handler_SlowStart_TurnCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( work[WIDX2] == false ) // work[WIDX2] : スロースタート終了宣言フラグ
    {
      if( work[WIDX1] < BTL_CALC_TOK_SLOWSTART_ENABLE_TURN )
      {
        BTL_ACTION_PARAM  action;

        // 現ターンのアクションが取得できなかったらインクリメントしない
        if( !flowWk->Hnd_GetThisTurnAction( pokeID, &action) ){ return; }

        // 現ターンに入れ替えで入場したばかりならインクリメントしない
        if( action.gen.cmd == BTL_ACTION_CHANGE ){ return; }

        ++(work[WIDX1]);
      }
      if( work[WIDX1] >= BTL_CALC_TOK_SLOWSTART_ENABLE_TURN )
      {
        BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
          HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_SlowStartOFF );
          HANDEX_STR_AddArg( &param->str, pokeID );
        flowWk->HANDEX_Pop( param );

        work[WIDX3] = false;
        work[WIDX2] = true;
      }
    }
  }
}

const BtlEventHandlerTable*  HAND_TOK_ADD_SlowStart( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,            handler_SlowStart_MemberIn  },  // メンバー入場ハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_SlowStart_MemberIn  },  // とくせい書き換え直後ハンドラ
    { BTL_EVENT_CALC_AGILITY,         handler_SlowStart_Agility   },  // すばやさ計算ハンドラ
    { BTL_EVENT_ATTACKER_POWER,       handler_SlowStart_AtkPower  },  // 攻撃力計算ハンドラ
    { BTL_EVENT_TURNCHECK_END,        handler_SlowStart_TurnCheck },  // ターンチェック終了ハンドラ

  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ふくがん」
 */
//------------------------------------------------------------------------------
// 命中率計算ハンドラ
void handler_Fukugan( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_FUKUGAN_HITRATIO );
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Fukugan( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO, handler_Fukugan },  // 命中率計算ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「すながくれ」
 */
//------------------------------------------------------------------------------
// 命中率計算ハンドラ
void handler_Sunagakure( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    if( flowWk->Hnd_GetWeather() == BTL_WEATHER_SAND )
    {
      EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_SUNAGAKURE_HITRATIO );
    }
  }
}
// 天候リアクションハンドラ
void handler_Sunagakure_Weather( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_weather_guard( myHandle, flowWk, pokeID, work, BTL_WEATHER_SAND );
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Sunagakure( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO,   handler_Sunagakure         }, // 命中率計算ハンドラ
    { BTL_EVENT_WEATHER_REACTION, handler_Sunagakure_Weather }, // 天候リアクションハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ゆきがくれ」
 */
//------------------------------------------------------------------------------
// 命中率計算ハンドラ
void handler_Yukigakure( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    if( flowWk->Hnd_GetWeather() == BTL_WEATHER_SNOW )
    {
      EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_SUNAGAKURE_HITRATIO );
    }
  }
}
// 天候リアクションハンドラ
void handler_Yukigakure_Weather( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_weather_guard( myHandle, flowWk, pokeID, work, BTL_WEATHER_SNOW );
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Yukigakure( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO,   handler_Yukigakure         }, // 命中率計算ハンドラ
    { BTL_EVENT_WEATHER_REACTION, handler_Yukigakure_Weather }, // 天候リアクションハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
/**
 *  特定の天候ダメージを受けない処理共通
 */
void common_weather_guard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work, BtlWeather weather )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_DAMAGE) > 0)
  &&  (EVENTVAR_GetValue(BTL_EVAR_WEATHER) == weather)
  ){
    EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「いろめがね」
 */
//------------------------------------------------------------------------------
// ダメージ計算最終段階のハンドラ
void handler_Iromegane( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 攻撃側が自分で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 相性イマイチの時
    BtlTypeAffAbout aff = calc::TypeAffAbout( static_cast<BtlTypeAff>(EVENTVAR_GetValue(BTL_EVAR_TYPEAFF)) );
    if( aff == pml::battle::TypeAffinity::TYPEAFF_ABOUT_DISADVANTAGE )
    {
      // ダメージ２倍
      EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Iromegane( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3, handler_Iromegane },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ハードロック」＆「フィルター」
 */
//------------------------------------------------------------------------------
// ダメージ計算最終段階のハンドラ
void handler_HardRock( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 防御側が自分で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // こうかバツグンの時
    BtlTypeAff  aff = static_cast<BtlTypeAff>(EVENTVAR_GetValue( BTL_EVAR_TYPEAFF ));
    BtlTypeAffAbout affAbout = calc::TypeAffAbout( aff );
    if( affAbout == pml::battle::TypeAffinity::TYPEAFF_ABOUT_ADVANTAGE )
    {
      // ダメージ75％
      EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.75) );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_HardRock( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3, handler_HardRock },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「スナイパー」
 */
//------------------------------------------------------------------------------
// ダメージ計算最終段階のハンドラ
void handler_Sniper( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 攻撃側が自分で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // クリティカルの時
    if( EVENTVAR_GetValue(BTL_EVAR_CRITICAL_FLAG) )
    {
      // ダメージ1.5倍（クリティカルなので実質３倍）
      EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.5) );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Sniper( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3, handler_Sniper },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}


//------------------------------------------------------------------------------
/**
 *  とくせい「かそく」
 */
//------------------------------------------------------------------------------
// ターンチェックのハンドラ
void handler_Kasoku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->CONTFLAG_Get( BTL_POKEPARAM::CONTFLG_ACTION_DONE) )
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
        param->header.tokwin_flag = true;
        param->rankType = pml::wazadata::RANKEFF_AGILITY;
        param->poke_cnt = 1;
        param->pokeID[0] = pokeID;
        param->rankVolume = 1;
      flowWk->HANDEX_Pop( param );
    }
//    param->fAlmost = true;
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Kasoku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_END,         handler_Kasoku          },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  とくせい「てきおうりょく」
 */
//------------------------------------------------------------------------------
// タイプ一致補正率計算ハンドラ
void handler_Tekiouryoku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_TYPEMATCH_FLAG) )
    {
      EVENTVAR_RewriteValue(BTL_EVAR_RATIO, FX32_CONST(2));
      BTL_Printf("ポケ[%d]の てきおうりょく でタイプ一致補正率２倍に\n", pokeID);
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Tekiouryoku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TYPEMATCH_RATIO, handler_Tekiouryoku }, // タイプ一致補正率計算ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  とくせい「もうか」
 */
//------------------------------------------------------------------------------
// 攻撃威力決定のハンドラ
void handler_Mouka( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_hpborder_powerup( flowWk, pokeID, POKETYPE_HONOO );
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Mouka( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_Mouka },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「げきりゅう」
 */
//------------------------------------------------------------------------------
// 攻撃威力決定のハンドラ
void handler_Gekiryu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_hpborder_powerup( flowWk, pokeID, POKETYPE_MIZU );
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Gekiryu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_Gekiryu },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「しんりょく」
 */
//------------------------------------------------------------------------------
// 攻撃威力決定のハンドラ
void handler_Sinryoku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_hpborder_powerup( flowWk, pokeID, POKETYPE_KUSA );
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Sinryoku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_Sinryoku },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「むしのしらせ」
 */
//------------------------------------------------------------------------------
// 攻撃威力決定のハンドラ
void handler_MusinoSirase( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_hpborder_powerup( flowWk, pokeID, POKETYPE_MUSHI );
}
const BtlEventHandlerTable*  HAND_TOK_ADD_MusinoSirase( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_MusinoSirase },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//--------------------------------------------------------------------------
/**
 * HPが1/3の時に、特定タイプのワザ威力を上げるとくせいの共通処理
 *
 * @param   flowWk
 * @param   pokeID
 * @param   wazaType
 *
 */
//--------------------------------------------------------------------------
void common_hpborder_powerup( ServerFlow* flowWk, u8 pokeID, pml::PokeType wazaType )
{
  // 攻撃側が自分で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // HP 1/3 以下で
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u32  borderHP = calc::QuotMaxHP_Zero( bpp, 3 );
    u32  hp = bpp->GetValue( BTL_POKEPARAM::BPP_HP );

    if( hp <= borderHP )
    {
      // 使うのが指定タイプワザなら
      if( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == wazaType )
      {
        // 威力1.5倍
        EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.5) );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  とくせい「こんじょう」
 */
//------------------------------------------------------------------------------
// 攻撃威力決定のハンドラ
void handler_Konjou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 攻撃側が自分で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 状態異常で
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->GetPokeSick() != pml::pokepara::SICK_NULL )
    {
      // ダメージタイプが物理の時
      if( EVENTVAR_GetValue( BTL_EVAR_DAMAGE_TYPE ) == pml::wazadata::DMG_PHYSIC )
      {
        // 威力1.5倍
        EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.5) );

        // やけど減衰を無効化
        EVENTVAR_RewriteValue( BTL_EVAR_DISABLE_YAKEDO_FLAG, true );
      }
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Konjou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_Konjou },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「プラス」「マイナス」
 */
//------------------------------------------------------------------------------
// 攻撃威力決定のハンドラ
void handler_PlusMinus( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  // 攻撃側が自分で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 場に「プラス」or「マイナス」持ちの味方がいて
    if( checkExistTokuseiFriend(flowWk, pokeID, work, TOKUSEI_PURASU)
    ||  checkExistTokuseiFriend(flowWk, pokeID, work, TOKUSEI_MAINASU)
    ){
      // ダメージタイプが特殊の時
      if( EVENTVAR_GetValue( BTL_EVAR_DAMAGE_TYPE ) == pml::wazadata::DMG_SPECIAL )
      {
        // とくこう1.5倍
        EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_PLUS_POWRATIO );
      }
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Plus( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_PlusMinus }, // 攻撃力決定のハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
/**
 *  場に、指定された「とくせい」を持つ味方が居るか判定（プラス・マイナス共通）
 */
bool checkExistTokuseiFriend( ServerFlow* flowWk, u8 pokeID, int* work, TokuseiNo tokuseiID )
{
  BtlPokePos pos = flowWk->Hnd_PokeIDtoPokePos( pokeID );
  if( pos != BTL_POS_NULL )
  {
    BtlExPos  exPos = EXPOS_MAKE( BTL_EXPOS_FULL_FRIENDS, pos );
    u8* pokeIDAry = (u8*)work;
    u8  cnt = flowWk->Hnd_ExpandPokeID( exPos, pokeIDAry );
    u8 i;
    for(i=0; i<cnt; ++i)
    {
      if( pokeIDAry[i] != pokeID )
      {
        const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeIDAry[i] );
        if( bpp->GetValue( BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE) == tokuseiID )
        {
          return true;
        }
      }
    }
  }
  return false;
}


//------------------------------------------------------------------------------
/**
 *  とくせい「フラワーギフト」
 */
//------------------------------------------------------------------------------
/**
 *  フラワーギフト有効ポケモンかどうか判定
 */
bool checkFlowerGiftEnablePokemon( ServerFlow* flowWk, u8 pokeID )
{
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  if( bpp->GetMonsNo() == MONSNO_THERIMU ){
    return true;
  }
  return false;
}
// ポケ入場・とくせい取得後ハンドラ
void handler_FlowerGift_MemberInComp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( checkFlowerGiftEnablePokemon(flowWk, pokeID) )
  {
    //@バトル変更箇所
    u8 nextForm = (calc::IsShineWeather(flowWk->Hnd_GetWeather()))?  FORMNO_THERIMU_POSI : FORMNO_THERIMU_NEGA;
    common_FlowerGift_FormChange( myHandle, flowWk, pokeID, nextForm, true );
    work[WIDX0] = 1;
  }
}
// とくせい取得後ハンドラ
void handler_FlowerGift_GotTok( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    handler_FlowerGift_MemberInComp( myHandle, flowWk, pokeID, work );
  }
}
/**
 *  フラワーギフト共通：フォルムチェンジ処理
 */
void common_FlowerGift_FormChange( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, u8 nextForm, u8 fTokWin )
{
  GFL_UNUSED(myHandle);
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

  if( nextForm != bpp->GetFormNo() )
  {
    BTL_HANDEX_PARAM_CHANGE_FORM* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_FORM *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_FORM, pokeID ));
      param->pokeID = pokeID;
      param->formNo = nextForm;
      param->header.tokwin_flag = fTokWin;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_ChangeForm );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
    flowWk->HANDEX_Pop( param );
  }
}
// 天候変化ハンドラ
void handler_FlowerGift_Weather( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WIDX0] )
  {
    if( checkFlowerGiftEnablePokemon(flowWk, pokeID) )
    {
    //@バトル変更箇所
      u8 nextForm = (calc::IsShineWeather(flowWk->Hnd_GetWeather()))?  FORMNO_THERIMU_POSI : FORMNO_THERIMU_NEGA;
      common_FlowerGift_FormChange( myHandle, flowWk, pokeID, nextForm, true );
    }
  }
}
// いえきハンドラ
void handler_FlowerGift_TokOff( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WIDX0] )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
    {
      if( checkFlowerGiftEnablePokemon(flowWk, pokeID) )
      {
        common_FlowerGift_FormChange( myHandle, flowWk, pokeID, FORMNO_THERIMU_NEGA, false );
      }
    }
  }
}
// エアロック通知ハンドラ
void handler_FlowerGift_AirLock( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WIDX0] )
  {
    if( checkFlowerGiftEnablePokemon(flowWk, pokeID) )
    {
      common_FlowerGift_FormChange( myHandle, flowWk, pokeID, FORMNO_THERIMU_NEGA, false );
    }
  }
}

// とくせい書き換え直前ハンドラ
void handler_FlowerGift_TokChange( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WIDX0] )
  {
    if( checkFlowerGiftEnablePokemon(flowWk, pokeID) )
    {
      if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
      &&  (EVENTVAR_GetValue(BTL_EVAR_TOKUSEI_NEXT) != EVENT_FACTOR_GetSubID(myHandle))
      ){
        common_FlowerGift_FormChange( myHandle, flowWk, pokeID, FORMNO_THERIMU_NEGA, false );
      }
    }
  }
}
// 攻撃威力決定のハンドラ
void handler_FlowerGift_Power( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( checkFlowerGiftEnablePokemon(flowWk, pokeID) )
  {
    // 天気「はれ」で
    //@バトル変更箇所
    if( calc::IsShineWeather(flowWk->Hnd_GetWeather()) )
    {
      // 攻撃側が自分か味方のとき
      u8 atkPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
      if( flowWk->Hnd_GetMainModule()->IsFriendPokeID(pokeID, atkPokeID) )
      {
        // こうげき1.5倍
        if( EVENTVAR_GetValue( BTL_EVAR_DAMAGE_TYPE ) == pml::wazadata::DMG_PHYSIC )
        {
          EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_FLOWERGIFT_POWRATIO );
        }
      }
    }
  }
}
// 防御力決定のハンドラ
void handler_FlowerGift_Guard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( checkFlowerGiftEnablePokemon(flowWk, pokeID) )
  {
    // 天気「はれ」で
    //@バトル変更箇所
    if( calc::IsShineWeather(flowWk->Hnd_GetWeather()) )
    {
      // 防御側が自分か味方のとき
      u8 atkPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
      if( flowWk->Hnd_GetMainModule()->IsFriendPokeID(pokeID, atkPokeID) )
      {
        // とくぼう1.5倍
        if( EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == pml::wazadata::DMG_SPECIAL )
        {
          EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_FLOWERGIFT_GUARDRATIO );
        }
      }
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_FlowerGift( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN_COMP,       handler_FlowerGift_MemberInComp }, // ポケ入場ハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_FlowerGift_GotTok   }, // とくせい書き換えハンドラ
    { BTL_EVENT_WEATHER_CHANGE_AFTER, handler_FlowerGift_Weather  }, // 天候変化後ハンドラ
    { BTL_EVENT_IEKI_FIXED,           handler_FlowerGift_TokOff   }, // いえき確定ハンドラ
    { BTL_EVENT_NOTIFY_AIRLOCK,       handler_FlowerGift_AirLock  },  // エアロック通知ハンドラ
    { BTL_EVENT_ACTPROC_END,          handler_FlowerGift_Weather  },
    { BTL_EVENT_TURNCHECK_DONE,       handler_FlowerGift_Weather  },
    { BTL_EVENT_CHANGE_TOKUSEI_BEFORE,handler_FlowerGift_TokChange}, // とくせい書き換え直前ハンドラ
    { BTL_EVENT_ATTACKER_POWER,       handler_FlowerGift_Power    }, // 攻撃力決定のハンドラ
    { BTL_EVENT_DEFENDER_GUARD,       handler_FlowerGift_Guard    }, // 防御力決定のハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}



//------------------------------------------------------------------------------
/**
 *  とくせい「とうそうしん」
 */
//------------------------------------------------------------------------------
// ワザ威力決定のハンドラ
void handler_Tousousin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 攻撃側が自分
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* myParam = flowWk->Hnd_GetPokeParam( pokeID );
    const BTL_POKEPARAM* targetParam = flowWk->Hnd_GetPokeParam(
          EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );

    u8 mySex = myParam->GetValue( BTL_POKEPARAM::BPP_SEX );
    u8 targetSex = targetParam->GetValue( BTL_POKEPARAM::BPP_SEX );

    // 互いに性別不明じゃない場合
    if( (mySex!=pml::SEX_UNKNOWN) && (targetSex!=pml::SEX_UNKNOWN) )
    {
      if( mySex == targetSex ){
        EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.25) );

      }else{
        EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(0.75) );
      }
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Tousousin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Tousousin },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「テクニシャン」
 */
//------------------------------------------------------------------------------
// ワザ威力決定のハンドラ
void handler_Technician( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 攻撃側が自分
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 威力60以下なら1.5倍にする
    u32 pow = EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
    if( pow <= 60 )
    {
//      TAYA_Printf("テクニシャン有効\n");
      EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, BTL_CALC_TOK_TECKNICIAN_POWRATIO );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Technician( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Technician },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「てつのこぶし」
 */
//------------------------------------------------------------------------------
// ワザ威力決定のハンドラ
void handler_TetunoKobusi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 攻撃側が自分
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // パンチ系イメージのワザなら威力増加
    WazaNo  waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    if( WAZADATA_GetFlag(waza, pml::wazadata::WAZAFLAG_Punch) )
    {
      EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, BTL_CALC_TOK_TETUNOKOBUSI_POWRATIO );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_TetunoKobusi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_TetunoKobusi },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「すてみ」
 */
//------------------------------------------------------------------------------
// ワザ威力決定のハンドラ
void handler_Sutemi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 攻撃側が自分
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 反動のある攻撃なら威力増加
    WazaNo  waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    if( (WAZADATA_GetParam(waza, pml::wazadata::PARAM_ID_DAMAGE_REACTION_RATIO) != 0)
    ||  ( (waza==WAZANO_TOBIGERI) || (waza==WAZANO_TOBIHIZAGERI) )
    ){
      EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, BTL_CALC_TOK_SUTEMI_POWRATIO );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Sutemi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Sutemi },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  とくせい「ふしぎなうろこ」
 */
//------------------------------------------------------------------------------
///< 防御能力決定のハンドラ
void handler_FusiginaUroko( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 防御側が自分で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // 状態異常で
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->GetPokeSick() != pml::pokepara::SICK_NULL )
    {
      // ダメージタイプが物理の時、防御上昇
      if( EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == pml::wazadata::DMG_PHYSIC )
      {
        EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_FUSIGINAUROKO_GDRATIO );
      }
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_FusiginaUroko( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DEFENDER_GUARD, handler_FusiginaUroko },  ///< 防御能力決定のハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  とくせい「スキルリンク」
 */
//------------------------------------------------------------------------------
// 攻撃回数決定のハンドラ
void handler_SkillLink( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 攻撃側が自分の時
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ヒット回数を最大に
    EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_SkillLink( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_COUNT, handler_SkillLink },  // 攻撃回数決定のハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  とくせい「かいりきばさみ」
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_KairikiBasami( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RANKEFF_LAST_CHECK, handler_KairikiBasami_Check },
    { BTL_EVENT_RANKEFF_FAILED,     handler_KairikiBasami_Guard },

  };

  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
void handler_KairikiBasami_Check( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  common_RankDownGuard_Check( flowWk, pokeID, work, pml::wazadata::RANKEFF_ATTACK );
}
void handler_KairikiBasami_Guard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  common_RankDownGuard_Fixed( flowWk, pokeID, pokeID, work, BTL_STRID_SET_RankdownFail_ATK );
}
//------------------------------------------------------------------------------
/**
 *  とくせい「するどいめ」
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Surudoime( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RANKEFF_LAST_CHECK, handler_Surudoime_Check }, // ランクダウン成功チェック
    { BTL_EVENT_RANKEFF_FAILED,     handler_Surudoime_Guard },
    { BTL_EVENT_WAZA_HIT_RANK,      handler_Surudoime_HitRank },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// ランクダウン直前処理のハンドラ
void handler_Surudoime_Check( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  common_RankDownGuard_Check( flowWk, pokeID, work, pml::wazadata::RANKEFF_HIT );
}
void handler_Surudoime_Guard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  common_RankDownGuard_Fixed( flowWk, pokeID, pokeID, work, BTL_STRID_SET_RankdownFail_HIT );
}

// 相手の命中率上昇を無視
void handler_Surudoime_HitRank( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_FLAT_FLAG, true );
  }
}


//------------------------------------------------------------------------------
/**
 *  とくせい「クリアボディ」
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_ClearBody( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RANKEFF_LAST_CHECK, handler_ClearBody_Check }, // ランクダウン成功チェック
    { BTL_EVENT_RANKEFF_FAILED,     handler_ClearBody_Guard },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// ランクダウン直前処理のハンドラ
void handler_ClearBody_Check( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  common_RankDownGuard_Check( flowWk, pokeID, work, pml::wazadata::RANKEFF_MAX );
}
void handler_ClearBody_Guard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  common_RankDownGuard_Fixed( flowWk, pokeID, pokeID, work, BTL_STRID_SET_RankdownFail );
}

//----------------------------------------------------------------------------------
/**
 * ランクダウンを防ぐとくせいの共通処理（チェック時）
 *
 * @param   flowWk
 * @param   pokeID
 * @param   work
 * @param   rankType    防ぐランクタイプ（pml::wazadata::RANKEFF_MAXなら全て）
 */
//----------------------------------------------------------------------------------
void common_RankDownGuard_Check( ServerFlow* flowWk, u8 pokeID, int* work, WazaRankEffect rankType )
{
  GFL_UNUSED(flowWk);

  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID)
  ){
    if( (rankType == pml::wazadata::RANKEFF_MAX)
    ||  (EVENTVAR_GetValue(BTL_EVAR_STATUS_TYPE) == rankType)
    ){
      if( EVENTVAR_GetValue(BTL_EVAR_VOLUME) < 0 )
      {
        work[WIDX0] = EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ランクダウンを防ぐとくせいの共通処理（確定時）
 *
 * @param   flowWk
 * @param   pokeID          ランクダウンを防いだポケモンID
 * @param   tokwin_pokeID   とくせいウィンドウの主ポケモンID
 * @param   work
 * @param   strID   表示メッセージID
 */
//----------------------------------------------------------------------------------
void common_RankDownGuard_Fixed( ServerFlow* flowWk, u8 pokeID, u8 tokwin_pokeID, int* work, u16 strID )
{
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (work[WIDX0])
  ){
    int wazaSerial = EVENTVAR_GetValue( BTL_EVAR_WAZA_SERIAL );

    if( (wazaSerial == 0)
    ||  (work[WIDX1] != wazaSerial)
    ){
      BTL_HANDEX_PARAM_MESSAGE* param;

      flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, tokwin_pokeID );

      param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, tokwin_pokeID ));
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, strID );
        HANDEX_STR_AddArg( &param->str, pokeID );
        HANDEX_STR_SetFailMsgFlag( &param->str );
      flowWk->HANDEX_Pop( param );

      flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, tokwin_pokeID );

      work[WIDX1] = wazaSerial;
    }

    work[WIDX0] = 0;
  }
}



//------------------------------------------------------------------------------
/**
 *  とくせい「たんじゅん」
 */
//------------------------------------------------------------------------------
// ランク増減値修正ハンドラ
void handler_Tanjun( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    int value = EVENTVAR_GetValue( BTL_EVAR_VOLUME );
    value *= 2;
    EVENTVAR_RewriteValue( BTL_EVAR_VOLUME, value );
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Tanjun( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RANKVALUE_CHANGE, handler_Tanjun },  // ランク増減値修正ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「リーフガード」
 */
//------------------------------------------------------------------------------
// ポケモン系状態異常処理ハンドラ
void handler_ReafGuard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  // くらう側が自分
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // 天候が晴れ
    //@バトル変更箇所
    if( calc::IsShineWeather(flowWk->Hnd_GetWeather()) )
    {
      // ポケモン系状態異常＆あくびにはならない
      WazaSick sickID = static_cast<WazaSick>(EVENTVAR_GetValue( BTL_EVAR_SICKID ));
      if( (calc::IsBasicSickID(sickID))
      ||  (sickID == pml::wazadata::WAZASICK_AKUBI)
      ){
        work[WIDX0] = EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
      }
    }
  }
}
// いねむりチェックハンドラ
void handler_ReafGuard_InemuriCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // くらう側が自分
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // 天候が晴れ
    //@バトル変更箇所
    if( calc::IsShineWeather(flowWk->Hnd_GetWeather()) )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
    }
  }
}

const BtlEventHandlerTable*  HAND_TOK_ADD_ReafGuard( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_CHECKFAIL, handler_ReafGuard              }, // ポケモン系状態異常処理ハンドラ
    { BTL_EVENT_ADDSICK_FAILED,    handler_AddSickFailCommon      },
    { BTL_EVENT_CHECK_INEMURI,     handler_ReafGuard_InemuriCheck }, // いねむりチェック
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「じゅうなん」
 */
//------------------------------------------------------------------------------
// ポケモン系状態異常処理ハンドラ
void handler_Juunan_PokeSick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  work[WIDX0] = HandCommon_GuardWazaSick( flowWk, pokeID, pml::wazadata::WAZASICK_MAHI );
}
// とくせい書き換えハンドラ＆ポケ入場ハンドラ
void handler_Juunan_Wake( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_TokuseiWake_CureSick( flowWk, pokeID, pml::wazadata::WAZASICK_MAHI );
}
// アクション終了毎ハンドラ
void handler_Juunan_ActEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_TokuseiWake_CureSickCore( flowWk, pokeID, pml::wazadata::WAZASICK_MAHI );
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Juunan( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_CHECKFAIL,    handler_Juunan_PokeSick   }, // ポケモン系状態異常処理ハンドラ
    { BTL_EVENT_ADDSICK_FAILED,       handler_AddSickFailCommon },
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_Juunan_Wake       }, // とくせい書き換えハンドラ
    { BTL_EVENT_MEMBER_IN,            handler_Juunan_Wake       }, // ポケ入場ハンドラ
    { BTL_EVENT_ACTPROC_END,          handler_Juunan_ActEnd     }, // アクション終了毎ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ふみん」
 */
//------------------------------------------------------------------------------
// ポケモン系状態異常処理ハンドラ
void handler_Fumin_PokeSick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  work[WIDX0] = HandCommon_GuardWazaSick( flowWk, pokeID, pml::wazadata::WAZASICK_NEMURI );
  if( work[WIDX0] == 0 ){
    work[WIDX0] = HandCommon_GuardWazaSick( flowWk, pokeID, pml::wazadata::WAZASICK_AKUBI );
  }
}
// とくせい書き換えハンドラ＆ポケ入場ハンドラ
void handler_Fumin_Wake( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_TokuseiWake_CureSick( flowWk, pokeID, pml::wazadata::WAZASICK_NEMURI );
}
// アクション終了毎ハンドラ
void handler_Fumin_ActEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_TokuseiWake_CureSickCore( flowWk, pokeID, pml::wazadata::WAZASICK_NEMURI );
}
// いねむりチェックハンドラ
void handler_Fumin_InemuriCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID ){
    EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
  }
}

const BtlEventHandlerTable*  HAND_TOK_ADD_Fumin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_CHECKFAIL,    handler_Fumin_PokeSick     }, // ポケモン系状態異常処理ハンドラ
    { BTL_EVENT_CHECK_INEMURI,        handler_Fumin_InemuriCheck }, // いねむりチェック
    { BTL_EVENT_ADDSICK_FAILED,       handler_AddSickFailCommon  },
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_Fumin_Wake         }, // とくせい書き換えハンドラ
    { BTL_EVENT_MEMBER_IN,            handler_Fumin_Wake         }, // ポケ入場ハンドラ
    { BTL_EVENT_ACTPROC_END,          handler_Fumin_ActEnd       }, // アクション終了毎ハンドラ

  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「マグマのよろい」
 */
//------------------------------------------------------------------------------
// ポケモン系状態異常処理ハンドラ
void handler_MagumaNoYoroi_PokeSick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  work[WIDX0] = HandCommon_GuardWazaSick( flowWk, pokeID, pml::wazadata::WAZASICK_KOORI );
}
// とくせい書き換えハンドラ＆メンバー入場ハンドラ
void handler_MagumaNoYoroi_Wake( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_TokuseiWake_CureSick( flowWk, pokeID, pml::wazadata::WAZASICK_KOORI );
}
// アクション終了毎ハンドラ
void handler_MagumaNoYoroi_ActEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_TokuseiWake_CureSickCore( flowWk, pokeID, pml::wazadata::WAZASICK_KOORI );
}
const BtlEventHandlerTable*  HAND_TOK_ADD_MagumaNoYoroi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_CHECKFAIL,     handler_MagumaNoYoroi_PokeSick }, // ポケモン系状態異常処理ハンドラ
    { BTL_EVENT_ADDSICK_FAILED,        handler_AddSickFailCommon      },
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER,  handler_MagumaNoYoroi_Wake     }, // とくせい書き換えハンドラ
    { BTL_EVENT_MEMBER_IN,             handler_MagumaNoYoroi_Wake     }, // メンバー入場ハンドラ
    { BTL_EVENT_ACTPROC_END,           handler_MagumaNoYoroi_ActEnd   }, // アクション終了毎ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「めんえき」
 */
//------------------------------------------------------------------------------
// ポケモン系状態異常処理ハンドラ
void handler_Meneki_PokeSick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  work[WIDX0] = HandCommon_GuardWazaSick( flowWk, pokeID, pml::wazadata::WAZASICK_DOKU );
}
// とくせい書き換えハンドラ＆メンバー入場ハンドラ
void handler_Meneki_Wake( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_TokuseiWake_CureSick( flowWk, pokeID, pml::wazadata::WAZASICK_DOKU );
}
// アクション終了毎ハンドラ
void handler_Meneki_ActEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_TokuseiWake_CureSickCore( flowWk, pokeID, pml::wazadata::WAZASICK_DOKU );
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Meneki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_CHECKFAIL,     handler_Meneki_PokeSick   },  // ポケモン系状態異常処理ハンドラ
    { BTL_EVENT_ADDSICK_FAILED,        handler_AddSickFailCommon },
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER,  handler_Meneki_Wake       },  // とくせい書き換えハンドラ
    { BTL_EVENT_MEMBER_IN,             handler_Meneki_Wake       },  // メンバー入場ハンドラ
    { BTL_EVENT_ACTPROC_END,           handler_Meneki_ActEnd     },  // アクション終了毎ハンドラ

  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「みずのベール」
 */
//------------------------------------------------------------------------------
// ポケモン系状態異常処理ハンドラ
void handler_MizuNoBale_PokeSick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  work[WIDX0] = HandCommon_GuardWazaSick( flowWk, pokeID, pml::wazadata::WAZASICK_YAKEDO );
}
// とくせい書き換えハンドラ＆メンバー入場ハンドラ
void handler_MizuNoBale_Wake( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_TokuseiWake_CureSick( flowWk, pokeID, pml::wazadata::WAZASICK_YAKEDO );
}
// アクション終了毎ハンドラ
void handler_MizuNoBale_ActEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_TokuseiWake_CureSickCore( flowWk, pokeID, pml::wazadata::WAZASICK_YAKEDO );
}
const BtlEventHandlerTable*  HAND_TOK_ADD_MizuNoBale( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_CHECKFAIL,    handler_MizuNoBale_PokeSick },  // ポケモン系状態異常処理ハンドラ
    { BTL_EVENT_ADDSICK_FAILED,       handler_AddSickFailCommon   },
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_MizuNoBale_Wake     },  // とくせい書き換えハンドラ
    { BTL_EVENT_MEMBER_IN,            handler_MizuNoBale_Wake     },  // メンバー入場ハンドラ
    { BTL_EVENT_ACTPROC_END,          handler_MizuNoBale_ActEnd   },  // アクション終了毎ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「マイペース」
 */
//------------------------------------------------------------------------------
// ポケモン系状態異常処理ハンドラ
void handler_MyPace_PokeSick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  work[WIDX0] = HandCommon_GuardWazaSick( flowWk, pokeID, pml::wazadata::WAZASICK_KONRAN );
}
//
void handler_MyPace_AddSickFailed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_AddSickFailed( myHandle, flowWk, pokeID, work, BTL_STRID_SET_KonranNoEffect );
}
// とくせい書き換えハンドラ
void handler_MyPace_Wake( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_TokuseiWake_CureSick( flowWk, pokeID, pml::wazadata::WAZASICK_KONRAN );
}
// アクション終了毎ハンドラ
void handler_MyPace_ActEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_TokuseiWake_CureSickCore( flowWk, pokeID, pml::wazadata::WAZASICK_KONRAN );
}
const BtlEventHandlerTable*  HAND_TOK_ADD_MyPace( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_CHECKFAIL,     handler_MyPace_PokeSick      },  // ポケモン系状態異常処理ハンドラ
    { BTL_EVENT_ADDSICK_FAILED,        handler_MyPace_AddSickFailed },
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER,  handler_MyPace_Wake          },  // とくせい書き換えハンドラ
    { BTL_EVENT_ACTPROC_END,           handler_MyPace_ActEnd        },  // アクション終了毎ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「どんかん」
 */
//------------------------------------------------------------------------------
// ワザ系状態異常ハンドラ
void handler_Donkan( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  work[WIDX0] = HandCommon_GuardWazaSick( flowWk, pokeID, pml::wazadata::WAZASICK_MEROMERO );
  if( work[WIDX0] == 0 ){
    work[WIDX0] = HandCommon_GuardWazaSick( flowWk, pokeID, pml::wazadata::WAZASICK_TYOUHATSU );
  }
}
// とくせい書き換えハンドラ
void handler_Donkan_Wake( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_TokuseiWake_CureSick( flowWk, pokeID, pml::wazadata::WAZASICK_MEROMERO );
  common_TokuseiWake_CureSick( flowWk, pokeID, pml::wazadata::WAZASICK_TYOUHATSU );
}
// アクション終了毎ハンドラ
void handler_Donkan_ActEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_TokuseiWake_CureSickCore( flowWk, pokeID, pml::wazadata::WAZASICK_MEROMERO );
  common_TokuseiWake_CureSickCore( flowWk, pokeID, pml::wazadata::WAZASICK_TYOUHATSU );
}
// ワザ無効化チェックハンドラ
void handler_Donkan_NoEffCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 自分が防御側の時
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // ワザ「ゆうわく」が無効
    WazaNo waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    if( (waza == WAZANO_YUUWAKU) || (waza==WAZANO_TYOUHATU) )
    {
      if( EVENTVAR_RewriteValue(BTL_EVAR_NOEFFECT_FLAG, true) )
      {
        flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );
        {
          BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
            HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_NoEffect );
            HANDEX_STR_AddArg( &param->str, pokeID );
          flowWk->HANDEX_Pop( param );
        }
        flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
      }
    }
  }
}

const BtlEventHandlerTable*  HAND_TOK_ADD_Donkan( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_CHECKFAIL,    handler_Donkan            },  // 状態異常失敗チェックハンドラ
    { BTL_EVENT_ADDSICK_FAILED,       handler_AddSickFailCommon },
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_Donkan_Wake       },  // とくせい書き換えハンドラ
    { BTL_EVENT_NOEFFECT_CHECK_TOKUSEI,    handler_Donkan_NoEffCheck },  // ワザ無効化チェックハンドラ
    { BTL_EVENT_ACTPROC_END,          handler_Donkan_ActEnd     },  // アクション終了毎ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
/**
 *  自分の“とくせい”の機能によって状態異常を防いだ場合の共通処理
 *
 *  ※ 自分の機能で防いだ場合は、work[WIDX0] を true にしておく。
 *  ※ その場合、とくせい帯を表示し、指定されたメッセージを表示する。
 */
void common_AddSickFailed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work, u16 strID )
{
  GFL_UNUSED(myHandle);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (static_cast<bool>(work[WIDX0]) == true)
  ){
    BTL_HANDEX_PARAM_MESSAGE* param;

    flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );

    param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, strID );
      HANDEX_STR_AddArg( &param->str, pokeID );
    flowWk->HANDEX_Pop( param );

    flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
    work[WIDX0] = false;
  }
}
void handler_AddSickFailCommon( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  u16 strID = BTL_STRID_SET_NoEffect;

  // カテゴリ「エフェクトシック」技による状態異常を防ぐとき、状態ごとの個別失敗メッセージがあればそれを使う
  if( EVENTVAR_GetValue(BTL_EVAR_SICK_CAUSE) == SICK_CAUSE_WAZA_EFFECT_SICK )
  {
    WazaSick  sickID = static_cast<WazaSick>(EVENTVAR_GetValue( BTL_EVAR_SICKID ));
    s16 specificStrID = flowWk->Hnd_GetSpecificSickFailStrID( sickID );
    if( specificStrID >= 0 ){
      strID = (u16)specificStrID;
    }
  }

  common_AddSickFailed( myHandle, flowWk, pokeID, work, strID );
}


/**
 * 「とくせい」が有効になった時点で特定の状態異常ならそれを治す処理 共通
 *
 *  スキルスワップ・なりきり等で「とくせい」が書き換わった直後
 *  入場した直後
 *  アクション終了毎（かたやぶりなどで一時的に無効化されたものが復帰する）
 */
void common_TokuseiWake_CureSick( ServerFlow* flowWk, u8 pokeID, WazaSick sick )
{
  if( EVENTVAR_GetValue( BTL_EVAR_POKEID ) == pokeID )
  {
    common_TokuseiWake_CureSickCore( flowWk, pokeID, sick );
  }
}
void common_TokuseiWake_CureSickCore( ServerFlow* flowWk, u8 pokeID, WazaSick sick )
{
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  if( bpp->CheckSick(sick) )
  {
    flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );
    {
      BTL_HANDEX_PARAM_CURE_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
        param->sickCode = static_cast<BtlWazaSickEx>(sick);
        param->poke_cnt = 1;
        param->pokeID[0] = pokeID;
      flowWk->HANDEX_Pop( param );
    }
    flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「あめふらし」
 */
//------------------------------------------------------------------------------
void handler_Amefurasi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  //SBTS[2532] 対処詳細はscproc_AfterMemberIn関数のコメントを見てください。
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  if( bpp->GetMonsNo() == MONSNO_KAIOOGA && bpp->GetFormNo() == FORMNO_KAIOOGA_NORMAL )
  {
    if( bpp->GetItem() == ITEM_AIIRONOTAMA )
    {
      return;
    }
  }

  //@バトル変更箇所(フラグ追加対応
  common_weather_change( flowWk, pokeID, BTL_WEATHER_RAIN, ITEM_SIMETTAIWA ,false);
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Amefurasi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,             handler_Amefurasi }, // 自分が出場ハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER,  handler_Amefurasi }, // とくせい書き換えハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ひでり」
 */
//------------------------------------------------------------------------------
void handler_Hideri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  //@バトル変更箇所(フラグ追加対応

  //SBTS[2532] 対処詳細はscproc_AfterMemberIn関数のコメントを見てください。
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  if( bpp->GetMonsNo() == MONSNO_GURAADON && bpp->GetFormNo() == FORMNO_GURAADON_NORMAL )
  {
    if( bpp->GetItem() == ITEM_BENIIRONOTAMA )
    {
      return;
    }
  }
  
  common_weather_change( flowWk, pokeID, BTL_WEATHER_SHINE, ITEM_ATUIIWA ,false);
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Hideri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,            handler_Hideri },  // 自分が出場ハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_Hideri },  // とくせい書き換えハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「すなおこし」
 */
//------------------------------------------------------------------------------
void handler_Sunaokosi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  //@バトル変更箇所(フラグ追加対応
  common_weather_change( flowWk, pokeID, BTL_WEATHER_SAND, ITEM_SARASARAIWA ,false);
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Sunaokosi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,              handler_Sunaokosi }, // 自分が出場ハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER,   handler_Sunaokosi }, // // とくせい書き換えハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ゆきふらし」
 */
//------------------------------------------------------------------------------
void handler_Yukifurasi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  //@バトル変更箇所(フラグ追加対応
  common_weather_change( flowWk, pokeID, BTL_WEATHER_SNOW, ITEM_TUMETAIIWA ,false);
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Yukifurasi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,            handler_Yukifurasi },  // 自分が出場ハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_Yukifurasi },  // とくせい書き換えハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

//@バトル変更箇所
//===========================================================================================
//sango追加ここから(天候系なのでここに追加)
//------------------------------------------------------------------------------
/**
 *  とくせい「はじまりのうみ」
 */
//------------------------------------------------------------------------------
void handler_Hajimarinoumi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_weather_change( flowWk, pokeID, BTL_WEATHER_STORM, ITEM_DUMMY_DATA ,true);
}
void handler_Hajimarinoumi_stop( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  
  //自分以外に同じ特性がいなかった場合のみ天候を落とす。
  //他の天候に切り替える処理は必要ない。
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( common_check_tokusei( flowWk , pokeID , TOKUSEI_HAZIMARINOUMI ) == false )
    {
      common_weather_stop( flowWk, pokeID, BTL_WEATHER_STORM );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Hajimarinoumi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,            handler_Hajimarinoumi },  // 自分が出場ハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_Hajimarinoumi },  // とくせい書き換えハンドラ
    { BTL_EVENT_MEMBER_OUT_FIXED,     handler_Hajimarinoumi_stop },  // 個別ポケ退場確定後
    { BTL_EVENT_CHANGE_TOKUSEI_BEFORE,handler_Hajimarinoumi_stop },  // とくせい書き換え直前（書き換え確定）
    { BTL_EVENT_CHANGE_POKE_BEFORE,   handler_Hajimarinoumi_stop },  // へんしん前(新規天候特性キャンセル処理
    { BTL_EVENT_NOTIFY_DEAD,          handler_Hajimarinoumi_stop },  // 死亡直前
    { BTL_EVENT_IEKI_FIXED,           handler_Hajimarinoumi_stop },  // いえき確定
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  とくせい「おわりのだいち」
 */
//------------------------------------------------------------------------------
void handler_Owarinodaichi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_weather_change( flowWk, pokeID, BTL_WEATHER_DAY, ITEM_DUMMY_DATA ,true);
}
void handler_Owarinodaichi_stop( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  //自分以外に同じ特性がいなかった場合のみ天候を落とす。
  //他の天候に切り替える処理は必要ない。
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( common_check_tokusei( flowWk , pokeID , TOKUSEI_OWARINODAITI ) == false )
    {
      common_weather_stop( flowWk, pokeID, BTL_WEATHER_DAY );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Owarinodaiti( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,            handler_Owarinodaichi },  // 自分が出場ハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_Owarinodaichi },  // とくせい書き換えハンドラ
    { BTL_EVENT_MEMBER_OUT_FIXED,     handler_Owarinodaichi_stop },  // 個別ポケ退場確定後
    { BTL_EVENT_CHANGE_TOKUSEI_BEFORE,handler_Owarinodaichi_stop },  // とくせい書き換え直前（書き換え確定）
    { BTL_EVENT_CHANGE_POKE_BEFORE,   handler_Owarinodaichi_stop },  // へんしん前(新規天候特性キャンセル処理
    { BTL_EVENT_NOTIFY_DEAD,          handler_Owarinodaichi_stop },  // 死亡直前
    { BTL_EVENT_IEKI_FIXED,           handler_Owarinodaichi_stop },  // いえき確定
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  とくせい「デルタストリーム」
 */
//------------------------------------------------------------------------------
void handler_DeltaStream( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_weather_change( flowWk, pokeID, BTL_WEATHER_TURBULENCE, ITEM_DUMMY_DATA ,true);
}
void handler_DeltaStream_stop( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  //自分以外に同じ特性がいなかった場合のみ天候を落とす。
  //他の天候に切り替える処理は必要ない。
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( common_check_tokusei( flowWk , pokeID , TOKUSEI_DERUTASUTORIIMU ) == false )
    {
      common_weather_stop( flowWk, pokeID, BTL_WEATHER_TURBULENCE );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_DeltaStream( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,            handler_DeltaStream },  // 自分が出場ハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_DeltaStream },  // とくせい書き換えハンドラ
    { BTL_EVENT_MEMBER_OUT_FIXED,     handler_DeltaStream_stop },  // 個別ポケ退場確定後
    { BTL_EVENT_CHANGE_TOKUSEI_BEFORE,handler_DeltaStream_stop },  // とくせい書き換え直前（書き換え確定）
    { BTL_EVENT_CHANGE_POKE_BEFORE,   handler_DeltaStream_stop },  // へんしん前(新規天候特性キャンセル処理
    { BTL_EVENT_NOTIFY_DEAD,          handler_DeltaStream_stop },  // 死亡直前
    { BTL_EVENT_IEKI_FIXED,           handler_DeltaStream_stop },  // いえき確定
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

//sango追加ここまで
//===========================================================================================

//-------------------------------------------
/**
 *  入場時に天候変化させるとくせいの共通処理
 */
//-------------------------------------------
//@バトル変更箇所(新規天候は永続なのでフラグ追加
void common_weather_change( ServerFlow* flowWk, u8 pokeID, BtlWeather weather, u16 boostItem , bool isPermanent )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    u8 turn = BTL_WEATHER_TURN_DEFAULT;
    u8 turnUpCount = 0;

    if( boostItem != ITEM_DUMMY_DATA )
    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
      if( (bpp->GetItem() == boostItem)
      &&  (flowWk->Hnd_CheckItemUsable(pokeID))
      ){
        turnUpCount = calc::ITEM_GetParam( boostItem, ::item::ITEM_DATA::PRM_ID_ATTACK );
        turn += turnUpCount;
      }
    }
    
    //@バトル変更箇所 無限対応
    if( isPermanent == true)
    {
      turn = BTL_WEATHER_TURN_PERMANENT;
    }
    BTL_HANDEX_PARAM_CHANGE_WEATHER* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_WEATHER *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_WEATHER, pokeID ));
      param->header.tokwin_flag = true;
      param->weather = weather;
      param->turn = turn;
      param->turnUpCount = turnUpCount;
    flowWk->HANDEX_Pop( param );
  }
}

//-------------------------------------------
/**
 *  退場時に天候を戻すとくせいの共通処理
 */
//-------------------------------------------
//@バトル変更箇所(退場時に天候を止める処理
void common_weather_stop( ServerFlow* flowWk, u8 pokeID, BtlWeather weather )
{
  //自身である場合
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    //対象の天気が一致
    //エアロック中の時に戻らないため専用関数を作って対応
    if( flowWk->Hnd_GetWeather_True() == weather &&
        flowWk->Hnd_GetDefaultWeather() != weather )
    {
      BTL_HANDEX_PARAM_CHANGE_WEATHER* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_WEATHER *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_WEATHER, pokeID ));
        param->header.tokwin_flag = false;
        param->weather = BTL_WEATHER_NONE;
        param->turn = 0;
      flowWk->HANDEX_Pop( param );
    }
  }
}

//@バトル変更箇所
//-------------------------------------------
/**
 *  場に特定の特性がいるかチェックする(sango天候を戻すときのチェック用
 */
//-------------------------------------------
bool common_check_tokusei( ServerFlow* flowWk, u8 selfPokeId, u16 tokusei )
{
  u8 pokeArr[ BTL_EXIST_POS_MAX ];
  BtlPokePos myPos = flowWk->Hnd_GetExistFrontPokePos( selfPokeId );
  //SBTS[2478]取得範囲が攻撃範囲内だったので、全体を取得するように
  BtlExPos   expos = EXPOS_MAKE( BTL_EXPOS_FULL_ALL, myPos );
  u8 cnt = flowWk->Hnd_ExpandPokeID( expos, pokeArr );
  
  for(int i=0; i<cnt; ++i)
  {
    //SBTS[2478]自分が含まれるので自分ははじく
    if( pokeArr[i] != selfPokeId )
    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeArr[i] );
      //いえきの時にNULLが返ってくる版
      const u16 tok = bpp->GetValue( BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE );
      if( tok == tokusei )
      {
        return true;
      }
    }
  }
  return false;
}


//------------------------------------------------------------------------------
/**
 *  とくせい「エアロック」&「ノーてんき」
 */
//------------------------------------------------------------------------------
// 自分が出場ハンドラ
#if 1
void handler_AirLock_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
//    if( flowWk->Hnd_GetWeather() != BTL_WEATHER_NONE )
    TAYA_Printf("エアロック持ち入場\n");
    {
      BTL_HANDEX_PARAM_CHANGE_WEATHER* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_WEATHER *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_WEATHER, pokeID ));

        param->header.tokwin_flag = true;
        param->weather = BTL_WEATHER_NONE;
        param->fAirLock = true;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, BTL_STRID_STD_AirLock );

      flowWk->HANDEX_Pop( param );
    }
  }
}
#endif
// 天候変化ハンドラ
void handler_AirLock_ChangeWeather( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(pokeID);
  GFL_UNUSED(work);
  EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
}

const BtlEventHandlerTable*  HAND_TOK_ADD_AirLock( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,      handler_AirLock_MemberIn       },
    { BTL_EVENT_WEATHER_CHECK,  handler_AirLock_ChangeWeather  },  // 天候チェックハンドラ
//    { BTL_EVENT_WEATHER_CHANGE, handler_AirLock_ChangeWeather },  // 天候変化ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「アイスボディ」
 */
//------------------------------------------------------------------------------
// 天候ダメージ計算ハンドラ
void handler_IceBody( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_weather_recover( flowWk, pokeID, BTL_WEATHER_SNOW );
}
const BtlEventHandlerTable*  HAND_TOK_ADD_IcoBody( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WEATHER_REACTION, handler_IceBody },   // ターンチェック開始ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「あめうけざら」
 */
//------------------------------------------------------------------------------
// 天候ダメージ計算ハンドラ
void handler_AmeukeZara( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_weather_recover( flowWk, pokeID, BTL_WEATHER_RAIN );
}
const BtlEventHandlerTable*  HAND_TOK_ADD_AmeukeZara( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WEATHER_REACTION, handler_AmeukeZara },  // 天候ダメージ計算ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

/**
 *  天候に応じてHP回復するとくせいの共通処理
 */
void common_weather_recover( ServerFlow* flowWk, u8 pokeID, BtlWeather weather )
{
  //@バトル変更箇所
  //おおあめ、おおひでりは あめ、ひでり でチェック。
  //雨しか存在しないが、一応晴れの対応もしておく。
  BtlWeather fldWeather = EVENTVAR_GetValue(BTL_EVAR_WEATHER);
  if( calc::IsShineWeather(flowWk->Hnd_GetWeather()) )
  {
    fldWeather = BTL_WEATHER_SHINE;
  }
  if( calc::IsRainWeather(flowWk->Hnd_GetWeather()) )
  {
    fldWeather = BTL_WEATHER_RAIN;
  }
  
  if( (fldWeather == weather)
  &&  (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  ){
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    BTL_HANDEX_PARAM_RECOVER_HP* param = reinterpret_cast<BTL_HANDEX_PARAM_RECOVER_HP *>( flowWk->HANDEX_Push( BTL_HANDEX_RECOVER_HP, pokeID ));
      param->header.tokwin_flag = true;
      param->pokeID = pokeID;
      param->recoverHP = calc::QuotMaxHP( bpp, 16 );
    flowWk->HANDEX_Pop( param );

    EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「サンパワー」
 */
//------------------------------------------------------------------------------
// 天候ダメージ計算ハンドラ
void handler_SunPower_Weather( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 晴れの時、MAXHPの 1/8 減る
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    //@バトル変更箇所
    if( calc::IsShineWeather(EVENTVAR_GetValue(BTL_EVAR_WEATHER)) )
    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
      int damage = calc::QuotMaxHP( bpp, 8 );
      flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );
      {
        BTL_HANDEX_PARAM_DAMAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_DAMAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_DAMAGE, pokeID ));
          param->pokeID = pokeID;
          param->damage = damage;
          param->damageCause = DAMAGE_CAUSE_SUNPOWER;
          param->damageCausePokeID = flowWk->Hnd_GetWeatherCausePokeID();
      flowWk->HANDEX_Pop( param );
      }
      flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
    }
  }
}
// 攻撃力決定ハンドラ
void handler_SunPower_AtkPower( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 攻撃側が自分で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 天候が晴れで
    //@バトル変更箇所
    if( calc::IsShineWeather(flowWk->Hnd_GetWeather()) )
    {
      // ダメージタイプが特殊の時
      if( EVENTVAR_GetValue( BTL_EVAR_DAMAGE_TYPE ) == pml::wazadata::DMG_SPECIAL )
      {
        // 威力1.5倍
        EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.5f) );
      }
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_SunPower( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WEATHER_REACTION, handler_SunPower_Weather },  // 天候ダメージ計算ハンドラ
    { BTL_EVENT_ATTACKER_POWER,  handler_SunPower_AtkPower },  // 攻撃力決定ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  とくせい「りんぷん」
 */
//------------------------------------------------------------------------------
// 追加効果（状態異常）ハンドラ
void handler_Rinpun_Sick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( (pokeID == EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF))
  &&  (pokeID != EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK))
  &&  (EVENTVAR_GetValue(BTL_EVAR_SICKID) != pml::wazadata::WAZASICK_BIND)
  ){
    EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
  }
}
// 追加効果（ランク効果）ハンドラ
void handler_Rinpun_Rank( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( (pokeID == EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF))
  &&  (pokeID != EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK))
  ){
    EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
  }
}
// ひるみチェックハンドラ
void handler_Rinpun_Shrink( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( pokeID == EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
  }
}
// ダメージ反応直前ハンドラ
void handler_Rinpun_Guard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( pokeID == EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_RINPUNGUARD_FLG, true );
  }
}
void handler_Rinpun_GuardHitEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( (HandCommon_CheckTargetPokeID(pokeID))
  &&  (EVENTVAR_GetValue(BTL_EVAR_TARGET_POKECNT) == 1)
  ){
    EVENTVAR_RewriteValue( BTL_EVAR_RINPUNGUARD_FLG, true );
  }
}


const BtlEventHandlerTable*  HAND_TOK_ADD_Rinpun( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK,               handler_Rinpun_Sick   }, // 追加効果（状態異常）チェックハンドラ
    { BTL_EVENT_ADD_RANK_TARGET,        handler_Rinpun_Rank   }, // 追加効果（ランク効果）チェックハンドラ
    { BTL_EVENT_SHRINK_CHECK,           handler_Rinpun_Shrink }, // ひるみチェックハンドラ
    { BTL_EVENT_WAZA_DMG_REACTION_PREV, handler_Rinpun_Guard  }, // ダメージ反応直前
    { BTL_EVENT_DAMAGEPROC_END_HIT_PREV,handler_Rinpun_GuardHitEnd  }, // ダメージ反応直前
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「てんのめぐみ」
 */
//------------------------------------------------------------------------------
// 追加効果（状態異常，ランク効果共通）ハンドラ
void handler_TennoMegumi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( pokeID == EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) )
  {
    u16 per = EVENTVAR_GetValue( BTL_EVAR_ADD_PER ) * 2;
    EVENTVAR_RewriteValue( BTL_EVAR_ADD_PER, per );
    BTL_Printf("ポケ[%d]の てんのめぐみ で追加発生率２倍=%d%%\n", pokeID, per);
  }
}
// ひるみチェックハンドラ
void handler_TennoMegumi_Shrink( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( pokeID == EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_CRITICAL_FLAG, true );
  }
}

const BtlEventHandlerTable*  HAND_TOK_ADD_TennoMegumi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK,         handler_TennoMegumi },  // 追加効果（状態異常）チェックハンドラ
    { BTL_EVENT_ADD_RANK_TARGET,  handler_TennoMegumi },  // 追加効果（ランク効果）チェックハンドラ
    { BTL_EVENT_SP_ADDITIONAL_PER,handler_TennoMegumi },  // 特殊なワザの追加効果確率チェックハンドラ
    { BTL_EVENT_WAZA_SHRINK_PER,  handler_TennoMegumi_Shrink },  // ひるみチェックハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  とくせい「うるおいボディ」
 */
//------------------------------------------------------------------------------
// 状態異常ダメージハンドラ
void handler_UruoiBody( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( calc::IsRainWeather(flowWk->Hnd_GetWeather()) )//@バトル変更箇所
    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
      if( bpp->GetPokeSick() != pml::pokepara::SICK_NULL )
      {
        BTL_HANDEX_PARAM_CURE_SICK* param;

        flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );

        param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
          param->sickCode = WAZASICK_EX_POKEFULL;
          param->pokeID[0] = pokeID;
          param->poke_cnt = 1;
          param->header.autoRemoveFlag = true;
        flowWk->HANDEX_Pop( param );

        flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
      }
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_UruoiBody( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_BEGIN,      handler_UruoiBody },  // ターンチェック開始
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  とくせい「だっぴ」
 */
//------------------------------------------------------------------------------
// ターンチェック開始ハンドラ
void handler_Dappi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->GetPokeSick() != pml::pokepara::SICK_NULL )
    {
      if( isExePer(flowWk, 33) )
      {
        BTL_HANDEX_PARAM_CURE_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
          param->header.tokwin_flag = true;
          param->header.autoRemoveFlag = true;
          param->sickCode = WAZASICK_EX_POKEFULL;
          param->pokeID[0] = pokeID;
          param->poke_cnt = 1;
        flowWk->HANDEX_Pop( param );
      }
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Dappi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_BEGIN,      handler_Dappi },  // ターンチェック開始
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  とくせい「ポイズンヒール」
 */
//------------------------------------------------------------------------------
// 状態異常ダメージハンドラ
void handler_PoisonHeal( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( pokeID == EVENTVAR_GetValue(BTL_EVAR_POKEID) )
  {
    WazaSick sick = static_cast<WazaSick>(EVENTVAR_GetValue( BTL_EVAR_SICKID ));
    if( sick == pml::wazadata::WAZASICK_DOKU )
    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
      BTL_HANDEX_PARAM_RECOVER_HP* param;

      EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, 0 );

      param = reinterpret_cast<BTL_HANDEX_PARAM_RECOVER_HP *>( flowWk->HANDEX_Push( BTL_HANDEX_RECOVER_HP, pokeID ));
        param->recoverHP = calc::QuotMaxHP( bpp, 8 );
        param->pokeID = pokeID;
        param->header.tokwin_flag = true;
      flowWk->HANDEX_Pop( param );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_PoisonHeal( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_SICK_DAMAGE,      handler_PoisonHeal }, // 状態異常ダメージハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  とくせい「カブトアーマー」＆「シェルアーマー」
 */
//------------------------------------------------------------------------------
// クリティカルチェックダメージハンドラ
void handler_KabutoArmor( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( pokeID == EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_KabutoArmor( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CRITICAL_CHECK,     handler_KabutoArmor },  // クリティカルチェックハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「きょううん」
 */
//------------------------------------------------------------------------------
// クリティカルチェックダメージハンドラ
void handler_Kyouun( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( pokeID == EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) )
  {
    u8 rank = EVENTVAR_GetValue( BTL_EVAR_CRITICAL_RANK ) + 1;
    EVENTVAR_RewriteValue( BTL_EVAR_CRITICAL_RANK, rank );
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Kyouun( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CRITICAL_CHECK,     handler_Kyouun }, // クリティカルチェックハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「いかりのつぼ」
 */
//------------------------------------------------------------------------------
// ダメージ直後ハンドラ
void handler_IkarinoTubo( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  &&  (EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == false)
  ){
    // クリティカルだったら攻撃マックスまでアップ
    if( EVENTVAR_GetValue(BTL_EVAR_CRITICAL_FLAG) )
    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
      int rankupVolume = bpp->RankEffectUpLimit(BTL_POKEPARAM::BPP_ATTACK_RANK );
      if( rankupVolume > 0 )
      {
        BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
          param->rankType = pml::wazadata::RANKEFF_ATTACK;
          param->rankVolume = bpp->RankEffectUpLimit(BTL_POKEPARAM::BPP_ATTACK_RANK );
          param->fAlmost = true;
          param->poke_cnt = 1;
          param->pokeID[0] = pokeID;
          param->fStdMsgDisable = true;
          param->header.tokwin_flag = true;
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_IkarinoTubo );
          HANDEX_STR_AddArg( &param->exStr, pokeID );
        flowWk->HANDEX_Pop( param );
      }
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_IkarinoTubo( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_IkarinoTubo },  // ダメージ直後ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「どくのトゲ」
 */
//------------------------------------------------------------------------------
// ダメージ直後ハンドラ
void handler_DokunoToge( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  PokeSick sick = pml::pokepara::SICK_DOKU;
  BTL_SICKCONT cont = calc::MakeDefaultPokeSickCont( sick, pokeID );
  common_touchAddSick( flowWk, pokeID, static_cast<WazaSick>(sick), cont, BTL_CALC_TOK_DOKUNOTOGE_PER );
}
const BtlEventHandlerTable*  HAND_TOK_ADD_DokunoToge( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_DokunoToge }, // ダメージ直後ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「せいでんき」
 */
//------------------------------------------------------------------------------
// ダメージ直後ハンドラ
void handler_Seidenki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  PokeSick sick = pml::pokepara::SICK_MAHI;
  BTL_SICKCONT cont = calc::MakeDefaultPokeSickCont( sick, pokeID );
  common_touchAddSick( flowWk, pokeID, static_cast<WazaSick>(sick), cont, BTL_CALC_TOK_SEIDENKI_PER );
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Seidenki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_Seidenki }, // ダメージ直後ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ほのおのからだ」
 */
//------------------------------------------------------------------------------
// ダメージ直後ハンドラ
void handler_HonoNoKarada( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  PokeSick sick = pml::pokepara::SICK_YAKEDO;
  BTL_SICKCONT cont = calc::MakeDefaultPokeSickCont( sick, pokeID );
  common_touchAddSick( flowWk, pokeID, static_cast<WazaSick>(sick), cont, BTL_CALC_TOK_DOKUNOTOGE_PER );
}
const BtlEventHandlerTable*  HAND_TOK_ADD_HonoNoKarada( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_HonoNoKarada }, // ダメージ直後ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「メロメロボディ」
 */
//------------------------------------------------------------------------------
// ダメージ直後ハンドラ
void handler_MeromeroBody( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  &&  (EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == false)
  &&  (!flowWk->Hnd_CheckShowDown())
  ){
    const BTL_POKEPARAM* myParam = flowWk->Hnd_GetPokeParam( pokeID );
    const BTL_POKEPARAM* targetParam = flowWk->Hnd_GetPokeParam(
          EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) );

    u8 mySex = myParam->GetValue( BTL_POKEPARAM::BPP_SEX );
    u8 targetSex = targetParam->GetValue( BTL_POKEPARAM::BPP_SEX );

    if( ((mySex!=pml::SEX_UNKNOWN) && (targetSex!=pml::SEX_UNKNOWN))
    &&  (mySex != targetSex)
    ){
      BTL_SICKCONT cont;
      calc::MakeDefaultWazaSickCont( pml::wazadata::WAZASICK_MEROMERO, myParam, &cont );
      common_touchAddSick( flowWk, pokeID, pml::wazadata::WAZASICK_MEROMERO, cont, BTL_CALC_TOK_MEROMEROBODY_PER );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_MeromeroBody( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_MeromeroBody }, // ダメージ直後ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ほうし」
 */
//------------------------------------------------------------------------------
// ダメージ直後ハンドラ
void handler_Housi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  &&  (EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == false)
  ){
    // 相手がくさタイプ・とくせい「ぼうじん」・アイテム「ぼうじんゴーグル」持ちなら無効
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetPokeID );
    if( (target->IsMatchType(POKETYPE_KUSA))
    ||  (target->GetValue(BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE) == TOKUSEI_BOUZIN)
    ||  (flowWk->Hnd_GetUsableItem(targetPokeID) == ITEM_BOUZINGOOGURU)
    ){
      return;
    }

    WazaSick sick;
    BTL_SICKCONT cont;

    {
      u32 rand = calc::GetRand( 30 );

      if( rand > 20 ){
        sick = pml::wazadata::WAZASICK_DOKU;
      }else if( rand > 10 ){
        sick = pml::wazadata::WAZASICK_MAHI;
      }else{
        sick = pml::wazadata::WAZASICK_NEMURI;
      }
      cont = calc::MakeDefaultPokeSickCont( static_cast<PokeSick>(sick), pokeID );
      common_touchAddSick( flowWk, pokeID, sick, cont, BTL_CALC_TOK_HOUSI_PER );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Housi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_Housi },  // ダメージ直後ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//--------------------------------------------------------------------------
/**
 * 接触ワザによるダメージ後、相手に対して状態異常を発症させるとくせいの共通処理
 *
 * @param   flowWk
 * @param   pokeID
 * @param   sick      状態異常ID
 * @param   per       発生確率
 *
 */
//--------------------------------------------------------------------------
void common_touchAddSick( ServerFlow* flowWk, u8 pokeID, WazaSick sick, const BTL_SICKCONT& sickCont, u8 per )
{
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  &&  ((bool)EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == false)
  &&  ((bool)EVENTVAR_GetValue(BTL_EVAR_WAZA_TOUCH_FLAG) == true)
  ){
    if( isExePer(flowWk, per) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
        param->header.tokwin_flag = true;
        param->sickID = sick;
        param->sickCont = sickCont;
        param->sickCause = SICK_CAUSE_TOKUSEI_TOUCH_COUNTER;
        param->fAlmost = false;
        param->pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
      flowWk->HANDEX_Pop( param );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  とくせい「さめはだ」「てつのとげ」
 */
//------------------------------------------------------------------------------
// ダメージ直後ハンドラ
void handler_Samehada( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  &&  ((bool)EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == false)
  &&  ((bool)EVENTVAR_GetValue(BTL_EVAR_WAZA_TOUCH_FLAG) == true)
  ){
    u8 attackerPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( attackerPokeID );

    if( !bpp->IsDead() )
    {
      u16 tokID = EVENT_FACTOR_GetSubID( myHandle );
      DamageCause damageCause = ( tokID == TOKUSEI_SAMEHADA ) ? ( DAMAGE_CAUSE_SAMEHADA ) : 
                                ( tokID == TOKUSEI_TETUNOTOGE ) ? ( DAMAGE_CAUSE_TETUNOTOGE ) : ( DAMAGE_CAUSE_OTHER );

      // ダメージを与える
      // 
      // このダメージでは、死亡した場合でも退場処理を行いません。
      // ここで退場処理を行ってしまうと、両陣営が同時に全滅した場合の勝敗判定が反転してしまうためです。
      //
      // 両陣営が全滅した場合、後に退場したほうの勝ちとなりますが、
      // ここで退場処理を行ってしまうと
      // 防御側が技ダメージによって退場する前に
      // 攻撃側が「さめはだ」のダメージで退場してしまうため、攻撃側が負けになってしまいます。
      BTL_HANDEX_PARAM_DAMAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_DAMAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_DAMAGE, pokeID ));
        param->header.tokwin_flag  = true;
        param->pokeID              = attackerPokeID;
        param->damage              = calc::QuotMaxHP( bpp, 8 );
        param->damageCause         = damageCause;
        param->damageCausePokeID   = pokeID;
        param->fDisableDeadProcess = true;                         // 死亡した場合でも退場処理を行わない
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Samehada );
        HANDEX_STR_AddArg( &param->exStr, attackerPokeID );
      flowWk->HANDEX_Pop( param );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Samehada( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_Samehada }, // ダメージ直後ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ゆうばく」
 */
//------------------------------------------------------------------------------
// ダメージ直後ハンドラ
void handler_Yuubaku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( ( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF)==pokeID ) &&
      ( EVENTVAR_GetValue(BTL_EVAR_WAZA_TOUCH_FLAG ) ) )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->IsDead() )
    {
      u8 attackerPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
      const BTL_POKEPARAM* attacker = flowWk->Hnd_GetPokeParam( attackerPokeID );

      // ダメージを与える
      // 
      // このダメージでは、死亡した場合でも退場処理を行いません。
      // ここで退場処理を行ってしまうと、両陣営が同時に全滅した場合の勝敗判定が反転してしまうためです。
      //
      // 両陣営が全滅した場合、後に退場したほうの勝ちとなりますが、
      // ここで退場処理を行ってしまうと
      // 防御側が技ダメージによって退場する前に
      // 攻撃側が「ゆうばく」のダメージで退場してしまうため、攻撃側が負けになってしまいます。
      {
        BTL_HANDEX_PARAM_DAMAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_DAMAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_DAMAGE, pokeID ));
          param->header.tokwin_flag  = true;
          param->pokeID              = attackerPokeID;
          param->damage              = calc::QuotMaxHP( attacker, 4 );
          param->damageCause         = DAMAGE_CAUSE_YUUBAKU;
          param->damageCausePokeID   = pokeID;
          param->fAvoidHidePoke      = true;
          param->fDisableDeadProcess = true;                         // 死亡した場合でも退場処理を行わない
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_SimpleDamage );
          HANDEX_STR_AddArg( &param->exStr, attackerPokeID );
        flowWk->HANDEX_Pop( param );
      }
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Yuubaku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_Yuubaku },  // ダメージ直後ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「へんしょく」
 */
//------------------------------------------------------------------------------
// ダメージ直後ハンドラ
void handler_Hensyoku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( (HandCommon_CheckTargetPokeID(pokeID) )
  &&  (!flowWk->Hnd_CheckShowDown())
  ){
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( !bpp->IsDead() )
    {
      pml::PokeType wazaType = EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );

      if( (wazaType != POKETYPE_NULL) && (!bpp->IsMatchType(wazaType)) )
      {
        flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );
        {
          BTL_HANDEX_PARAM_CHANGE_TYPE* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_TYPE *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_TYPE, pokeID ));
            param->next_type = PokeTypePair_MakePure( wazaType );
            param->exTypeCause = BTL_POKEPARAM::EXTYPE_CAUSE_NONE;
            param->pokeID = pokeID;
          flowWk->HANDEX_Pop( param );
        }
        flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
      }
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Hensyoku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL,     handler_Hensyoku }, // ダメージ直後ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「シンクロ」
 */
//------------------------------------------------------------------------------
// ポケモン系状態異常ハンドラ
void handler_Syncro( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF)==pokeID )
  {
    PokeSick sick = static_cast<PokeSick>(EVENTVAR_GetValue( BTL_EVAR_SICKID ));
    if( (sick==pml::pokepara::SICK_DOKU)||(sick==pml::pokepara::SICK_MAHI) ||(sick==pml::pokepara::SICK_YAKEDO) )
    {
      u8 attackPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
      if( (attackPokeID != BTL_POKEID_NULL) && (attackPokeID != pokeID) )
      {
        BTL_HANDEX_PARAM_ADD_SICK* param;
        const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
        const BTL_SICKCONT* cont = reinterpret_cast<const BTL_SICKCONT*>( EVENTVAR_GetValue( BTL_EVAR_SICK_CONT_ADRS ) );

        flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );

        param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
          param->pokeID = attackPokeID;
          param->sickID = static_cast<WazaSick>(sick);
          // もうどくは継続パラメータをそのままにする必要がある
          if( (sick == pml::pokepara::SICK_DOKU) && (SICKCONT_IsMoudokuCont(*cont)) ){
            param->sickCont = *cont;
            SICKCONT_SetCausePokeID( &(param->sickCont), pokeID );
          }else{
            calc::MakeDefaultWazaSickCont( static_cast<WazaSick>(sick), bpp, &param->sickCont );
          }
          param->fAlmost = true;
        flowWk->HANDEX_Pop( param );

        flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
      }
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Syncro( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_POKESICK_FIXED,      handler_Syncro }, // ポケモン系状態異常ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}


//------------------------------------------------------------------------------
/**
 *  とくせい「いしあたま」
 */
//------------------------------------------------------------------------------
// 反動計算ハンドラ
void handler_Isiatama( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( pokeID == EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Isiatama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_KICKBACK,      handler_Isiatama }, // 反動計算ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ノーマルスキン」
 */
//------------------------------------------------------------------------------
// ワザタイプ決定ハンドラ
void handler_NormalSkin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);

  work[WIDX0] = 0;

  WazaID wazaID = static_cast<WazaID>( EVENTVAR_GetValue( BTL_EVAR_WAZAID ) );

  if( ( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID) &&
      !( tables::IsZWaza( wazaID ) ) )
  {
    work[WIDX0] = EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, POKETYPE_NORMAL );
  }
}
// 威力計算
void handler_NormalSkin_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Skin_WazaPow( myHandle, flowWk, pokeID, work, POKETYPE_NORMAL );
}
const BtlEventHandlerTable*  HAND_TOK_ADD_NormalSkin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM,   handler_NormalSkin     }, // ワザタイプ決定ハンドラ
    { BTL_EVENT_WAZA_POWER,   handler_NormalSkin_Pow }, // 威力計算
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  とくせい「トレース」
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Trace( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,            handler_Trace },     // 入場ハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_Trace },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// 入場ハンドラ
void handler_Trace( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  u8 inPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );

  u8 targetPokeID = BTL_POKEID_NULL;
  TokuseiNo nextTok = TOKUSEI_NULL;


  // 自分が入場したタイミングは相手を全部見てランダム
  if( inPokeID == pokeID )
  {
    u8  allPokeID[ BTL_POSIDX_MAX ];
    u8  checkPokeID[ BTL_POSIDX_MAX ];
    const BTL_POKEPARAM* bpp;
    u8  pokeCnt, targetCnt, i;
    TokuseiNo tok;

    BtlPokePos myPos = flowWk->Hnd_GetExistFrontPokePos( pokeID );
    BtlExPos   exPos = EXPOS_MAKE( BTL_EXPOS_AREA_ENEMY, myPos );

    pokeCnt = flowWk->Hnd_ExpandPokeID( exPos, allPokeID );
    targetCnt = 0;
    for(i=0; i<pokeCnt; ++i)
    {
      bpp = flowWk->Hnd_GetPokeParam( allPokeID[i] );
      tok = static_cast<TokuseiNo>(bpp->GetValue( BTL_POKEPARAM::BPP_TOKUSEI ));
      if( !tables::CheckTraceFailTokusei(tok) )
      {
        checkPokeID[ targetCnt++ ] = allPokeID[i];
      }
    }

    // トレースできる対象がいればトレースする
    if( targetCnt )
    {
      u8 idx = (targetCnt==1)? 0 : calc::GetRand( targetCnt );
      bpp = flowWk->Hnd_GetPokeParam( checkPokeID[idx] );
      nextTok = static_cast<TokuseiNo>(bpp->GetValue( BTL_POKEPARAM::BPP_TOKUSEI ));
      targetPokeID = bpp->GetID();
    }
    // いなければ待機状態へ
    else{
      work[WIDX0] = 1;
    }
  }
  // 相手が入場したタイミングに待機状態なら反応
  else if( (!flowWk->Hnd_GetMainModule()->IsFriendPokeID(inPokeID, pokeID))
  &&        (work[WIDX0] == 1)
  ){
    BtlPokePos inPokePos = flowWk->Hnd_PokeIDtoPokePos( inPokeID );

    if( inPokePos != BTL_POS_NULL )
    {
      const BTL_POKEPARAM* inPoke = flowWk->Hnd_GetPokeParam( inPokeID );
      TokuseiNo tok = static_cast<TokuseiNo>(inPoke->GetValue( BTL_POKEPARAM::BPP_TOKUSEI ));

      if( !tables::CheckTraceFailTokusei(tok) )
      {
        nextTok = tok;
        targetPokeID = inPokeID;
      }
    }
  }

  if( (nextTok != TOKUSEI_NULL) && (pokeID != BTL_POKEID_NULL) )
  {
    BTL_HANDEX_PARAM_CHANGE_TOKUSEI* param;
    param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_TOKUSEI *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_TOKUSEI, pokeID ));
      param->pokeID = pokeID;
      param->tokuseiID = nextTok;
      param->fSkipMemberInEvent = true;
      param->header.tokwin_flag = true;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Trace );
      HANDEX_STR_AddArg( &param->exStr, targetPokeID );
      HANDEX_STR_AddArg( &param->exStr, param->tokuseiID );
    flowWk->HANDEX_Pop( param );
    BTL_N_PrintfEx( PRINT_CHANNEL_PRESSURE, DBGSTR_HANDTOK_TRACE_Add, pokeID );
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「しぜんかいふく」
 */
//------------------------------------------------------------------------------
// メンバー退出ハンドラ
void handler_SizenKaifuku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_CURE_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
      param->sickCode = WAZASICK_EX_POKEFULL;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->fStdMsgDisable = true;
    flowWk->HANDEX_Pop( param );
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_SizenKaifuku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_OUT_FIXED, handler_SizenKaifuku },   // メンバー退出ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ダウンロード」
 */
//------------------------------------------------------------------------------
// メンバー入場ハンドラ
void handler_Download( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    u8 ePokeID[BTL_POSIDX_MAX];
    u8 pokeCnt = flowWk->Hnd_GetAllOpponentFrontPokeID( pokeID, ePokeID );
    if( pokeCnt )
    {
      const BTL_POKEPARAM* bpp;
      u16 sumDef = 0;
      u16 sumSpDef = 0;
      WazaRankEffect  valueID;
      u8 i;
      for(i=0; i<pokeCnt; ++i){
        bpp = flowWk->Hnd_GetPokeParam( ePokeID[i] );
        sumDef += bpp->GetValue( BTL_POKEPARAM::BPP_DEFENCE );
        sumSpDef += bpp->GetValue( BTL_POKEPARAM::BPP_SP_DEFENCE );
      }

      // 相手のぼうぎょ・とくぼうを比較して、弱い方に対応した自分のこうげき・とくこうをアップ
      //（同じなら とくこう をアップ）
      if( sumDef >= sumSpDef ){
        valueID = pml::wazadata::RANKEFF_SP_ATTACK;
      }else{
        valueID = pml::wazadata::RANKEFF_ATTACK;
      }

      {
        BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
          param->header.tokwin_flag = true;
          param->poke_cnt = 1;
          param->pokeID[0] = pokeID;
          param->rankType = valueID;
          param->rankVolume = 1;
        flowWk->HANDEX_Pop( param );
      }
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_DownLoad( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,            handler_Download },    // メンバー入場ハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_Download },    // とくせい書き換えハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「よちむ」
 */
//------------------------------------------------------------------------------
// 入場ハンドラ
void handler_Yotimu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    typedef struct {
      u8      pokeID;
      WazaID  wazaID;
    }MAX_PRIORITY_PARAM;

    const BTL_POKEPARAM* bpp;
    MAX_PRIORITY_PARAM*  maxPriParam;
    WazaID waza;
    u8 ePokeID[BTL_POSIDX_MAX];
    u8 pokeCnt = flowWk->Hnd_GetAllOpponentFrontPokeID( pokeID, ePokeID );
    u8 wazaCnt, i, w;
    u8 pri, maxPri, maxPriCnt;

    maxPri = 0;
    maxPriCnt = 0;
    maxPriParam = static_cast<MAX_PRIORITY_PARAM*>
                 (flowWk->Hnd_GetTmpWork( sizeof(MAX_PRIORITY_PARAM) * BTL_POSIDX_MAX ));

    for(i=0; i<pokeCnt; ++i)
    {
      bpp = flowWk->Hnd_GetPokeParam( ePokeID[i] );
      wazaCnt = bpp->WAZA_GetCount( );
      for(w=0; w<wazaCnt; ++w)
      {
        waza = bpp->WAZA_GetID( w );
        // ダメージワザ
        if( WAZADATA_GetDamageType(waza) != pml::wazadata::DMG_NONE )
        {
          pri = WAZADATA_GetPower( waza );
          if( pri == 1 )
          {
            if( WAZADATA_GetCategory(waza) == pml::wazadata::CATEGORY_ICHIGEKI ){
              pri = 150;
            }else {

              switch( waza ){
              case WAZANO_KAUNTAA:
              case WAZANO_MIRAAKOOTO:
              case WAZANO_METARUBAASUTO:
                pri = 120;
                break;
              default:
                pri = 80;
              }
            }
          }
        }
        // 非ダメージワザ
        else
        {
          pri = 1;
        }

        if( pri >= maxPri )
        {
          if( pri > maxPri ){
            maxPri = pri;
            maxPriCnt = 0;
          }
          maxPriParam[ maxPriCnt ].pokeID = bpp->GetID( );
          maxPriParam[ maxPriCnt ].wazaID = waza;
          ++maxPriCnt;
        }
      }
    }

    if( maxPriCnt )
    {
      i = calc::GetRand( maxPriCnt );

      flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );
      {
        BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
          HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_YotimuExe );
          HANDEX_STR_AddArg( &param->str, maxPriParam[i].pokeID );
          HANDEX_STR_AddArg( &param->str, maxPriParam[i].wazaID );
        flowWk->HANDEX_Pop( param );
      }
      flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Yotimu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,             handler_Yotimu }, // 入場ハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER,  handler_Yotimu }, // とくせい書き換えハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「きけんよち」
 */
//------------------------------------------------------------------------------
// 入場ハンドラ
void handler_KikenYoti( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bppUser;
    const BTL_POKEPARAM* bppEnemy;
    u8 ePokeID[BTL_POSIDX_MAX];
    u8 pokeCnt = flowWk->Hnd_GetAllOpponentFrontPokeID( pokeID, ePokeID );
    u8 i;
    bppUser = flowWk->Hnd_GetPokeParam( pokeID );
    for(i=0; i<pokeCnt; ++i)
    {
      bppEnemy = flowWk->Hnd_GetPokeParam( ePokeID[i] );
      if( check_kikenyoti_poke(flowWk, bppUser, bppEnemy) )
      {
        break;
      }
    }
    if( i != pokeCnt )
    {
      BTL_HANDEX_PARAM_MESSAGE* param;

      flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );

      param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Kikenyoti );
        HANDEX_STR_AddArg( &param->str, pokeID );
      flowWk->HANDEX_Pop( param );

      flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
    }
  }
}
/**
 *  きけんよち該当ポケモンかチェック
 */
bool check_kikenyoti_poke( ServerFlow* flowWk, const BTL_POKEPARAM* bppUser, const BTL_POKEPARAM* bppEnemy )
{
  WazaID  waza;
  PokeTypePair user_type;
  u8 waza_cnt, i;

  user_type = bppUser->GetPokeType();
  waza_cnt = bppEnemy->WAZA_GetCount();

  for(i=0; i<waza_cnt; ++i)
  {
    // 一撃必殺ワザはキケン
    waza = bppEnemy->WAZA_GetID( i );
    if( WAZADATA_GetCategory(waza) == pml::wazadata::CATEGORY_ICHIGEKI ){ return true; }

    // 自分に相性バツグンな奴もキケン
    if( WAZADATA_IsDamage(waza) )
    {
      pml::PokeType  waza_type;
      if( waza != WAZANO_MEZAMERUPAWAA ){
        waza_type = WAZADATA_GetType( waza );
      }else{
        waza_type = HandCommon_GetMezapaType( flowWk, bppEnemy->GetID() );
      }
      if( calc::TypeAffPair(waza_type, user_type) > pml::battle::TypeAffinity::TYPEAFF_1 ){ return true; }
    }
  }

  return false;
}

const BtlEventHandlerTable*  HAND_TOK_ADD_KikenYoti( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,             handler_KikenYoti },  // 入場ハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER,  handler_KikenYoti },  // とくせい書き換えハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「おみとおし」
 */
//------------------------------------------------------------------------------
// 入場ハンドラ
void handler_Omitoosi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    struct TmpWork{
      u16 itemID[ BTL_POSIDX_MAX ];
      u8  ePokeID[ BTL_POSIDX_MAX ];
      u8  enableIdx[ BTL_POSIDX_MAX ];
    };

    TmpWork* tmp = (TmpWork*)(flowWk->Hnd_GetTmpWork( sizeof(TmpWork) ));

    u8  pokeCnt = flowWk->Hnd_GetAllOpponentFrontPokeID( pokeID, tmp->ePokeID );
    u8  itemCnt = 0;
    for(u32 i=0; i<pokeCnt; ++i)
    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( tmp->ePokeID[i] );
      tmp->itemID[ i ] = bpp->GetItem();
      if( tmp->itemID[i] != ITEM_DUMMY_DATA)
      {
        tmp->enableIdx[itemCnt++] = i;
      }
    }

    if( itemCnt )
    {
      flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );

      for(u32 i=0; i<itemCnt; ++i)
      {
        BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        u32  enableIdx = tmp->enableIdx[i];
        if( i == 0 )
        {
          HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Omitoosi );
          HANDEX_STR_AddArg( &param->str, pokeID );
          HANDEX_STR_AddArg( &param->str, tmp->ePokeID[enableIdx] );
          HANDEX_STR_AddArg( &param->str, tmp->itemID[enableIdx] );
        }
        else
        {
          HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_OmitoosiCont );
          HANDEX_STR_AddArg( &param->str, tmp->ePokeID[enableIdx] );
          HANDEX_STR_AddArg( &param->str, tmp->itemID[enableIdx] );
        }
        flowWk->HANDEX_Pop( param );
      }
      flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Omitoosi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,             handler_Omitoosi }, // 入場ハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER,  handler_Omitoosi }, // とくせい書き換えハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「がんじょう」
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Ganjou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ICHIGEKI_CHECK, handler_Ganjou_Ichigeki   },   // 一撃チェックハンドラ
    { BTL_EVENT_KORAERU_CHECK,  handler_Ganjou_KoraeCheck },   // こらえるチェックハンドラ
    { BTL_EVENT_KORAERU_EXE,    handler_Ganjou_KoraeExe   },   // こらえる発動ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// 一撃チェックハンドラ
void handler_Ganjou_Ichigeki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    if( EVENTVAR_RewriteValue(BTL_EVAR_FAIL_FLAG, true) )
    {
      BTL_HANDEX_PARAM_MESSAGE* param;

      flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );
      param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_NoEffect );
        HANDEX_STR_AddArg( &param->str, pokeID );
      flowWk->HANDEX_Pop( param );
      flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
    }
  }
}
// こらえるチェックハンドラ
void handler_Ganjou_KoraeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->IsHPFull() )
    {
      work[WIDX0] = EVENTVAR_RewriteValue( BTL_EVAR_KORAERU_CAUSE, BTL_POKEPARAM::KORAE_TOKUSEI_DEFENDER );
    }
    else{
      work[WIDX0] = 0;
    }
  }
}
// こらえる発動ハンドラ
void handler_Ganjou_KoraeExe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  ( work[WIDX0] )
  ){
    BTL_HANDEX_PARAM_MESSAGE* param;

    flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );

    param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Koraeru_Exe );
      HANDEX_STR_AddArg( &param->str, pokeID );
    flowWk->HANDEX_Pop( param );

    flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );

    work[WIDX0] = 0;
  }
}


//------------------------------------------------------------------------------
/**
 *  とくせい「てんねん」
 */
//------------------------------------------------------------------------------
// 命中率・回避率チェックハンドラ
void handler_Tennen_hitRank( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 自分が攻撃側の時、防御側の回避率をフラットに
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_AVOID_RANK, BTL_CALC_HITRATIO_MID );
  }
  // 自分が防御側の時、攻撃側の命中率をフラットに
  else if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_HIT_RANK, BTL_CALC_HITRATIO_MID );
  }
}
// 攻撃ランクチェックハンドラ
void handler_Tennen_AtkRank( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 自分が防御側の時、攻撃側のランクをフラットに
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
  }
}
// 防御ランクチェックハンドラ
void handler_Tennen_DefRank( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 自分が攻撃側の時、防御側のランクをフラットに
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Tennen( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RANK,       handler_Tennen_hitRank }, // 命中率・回避率チェックハンドラ
    { BTL_EVENT_ATTACKER_POWER_PREV, handler_Tennen_AtkRank }, // 攻撃ランクチェックハンドラ
    { BTL_EVENT_DEFENDER_GUARD_PREV, handler_Tennen_DefRank }, // 防御ランクチェックハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「たいねつ」
 */
//------------------------------------------------------------------------------
// ワザ威力計算ハンドラ
void handler_Tainetsu_WazaPow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 自分が防御側の時、炎ワザの威力を半分に
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    if( EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE ) == POKETYPE_HONOO )
    {
      EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(0.5) );
    }
  }
}
// 状態異常ダメージ計算ハンドラ
void handler_Tainetsu_SickDmg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 自分が喰らう時、やけどならダメージ半分に
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    WazaSick sick = static_cast<WazaSick>(EVENTVAR_GetValue( BTL_EVAR_SICKID ));
    if( sick == pml::wazadata::WAZASICK_YAKEDO )
    {
      u32 dmg = calc::Roundup( EVENTVAR_GetValue( BTL_EVAR_DAMAGE ) / 2, 1 );
      EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, dmg );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Tainetu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_Tainetsu_WazaPow },   // ワザ威力計算ハンドラ
    { BTL_EVENT_SICK_DAMAGE,  handler_Tainetsu_SickDmg },   // 状態異常ダメージ計算ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}




/**
 *  ダメージ回復化処理共通：回復化チェック
 */
bool common_DmgToRecover_Check( ServerFlow* flowWk, u8 pokeID, int* work, pml::PokeType matchType )
{
  GFL_UNUSED(flowWk);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == matchType)
  ){
    work[WIDX0] = EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
    return work[WIDX0];
  }
  return false;
}
/**
 *  特定タイプワザを無効化するチェック処理共通
 * @return  ワザを無効化できたら true 
 */
bool common_TypeNoEffect( ServerFlow* flowWk, u8 pokeID, pml::PokeType wazaType )
{
  GFL_UNUSED(flowWk);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == wazaType)
  ){
    WazaID waza = (WazaID)(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    if( !HandCommon_IsSimpleEffectOnlyAdvantage(waza) ){
      return EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, true );
    }
  }
  return false;
}
/**
 *  特定タイプワザを無効化->HP回復の共通処理
 */
void common_TypeRecoverHP( ServerFlow* flowWk, u8 pokeID, u8 denomHP )
{
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

  if( !bpp->IsHPFull() )
  {
    BTL_HANDEX_PARAM_RECOVER_HP*  param = reinterpret_cast<BTL_HANDEX_PARAM_RECOVER_HP*>
                                    (flowWk->HANDEX_Push( BTL_HANDEX_RECOVER_HP, pokeID ));
      param->pokeID = pokeID;
      param->recoverHP = calc::QuotMaxHP( bpp, denomHP );
      param->header.tokwin_flag = true;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_HP_Recover );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
    flowWk->HANDEX_Pop( param );
  }
  else
  {
    BTL_HANDEX_PARAM_MESSAGE* msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_NoEffect );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
      msg_param->header.tokwin_flag = true;
    flowWk->HANDEX_Pop( msg_param );
  }

  EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
}
/**
 *  特定タイプワザを無効化->ランクアップ効果の共通処理
 */
void common_TypeNoEffect_Rankup( ServerFlow* flowWk, u8 pokeID, WazaRankEffect rankType, u8 volume )
{
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  if( bpp->IsRankEffectValid( static_cast<BTL_POKEPARAM::ValueID>(rankType), volume) )
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->fAlmost = true;
      param->rankType = rankType;
      param->rankVolume = volume;
      param->header.tokwin_flag = true;
    flowWk->HANDEX_Pop( param );
  }
  else
  {
    BTL_HANDEX_PARAM_MESSAGE* msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_NoEffect );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
      msg_param->header.tokwin_flag = true;
    flowWk->HANDEX_Pop( msg_param );
  }
}


//------------------------------------------------------------------------------
/**
 *  とくせい「かんそうはだ」
 */
//------------------------------------------------------------------------------
// 天候ダメージ計算ハンドラ
void handler_Kansouhada_Weather( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    BtlWeather weather = EVENTVAR_GetValue( BTL_EVAR_WEATHER );

    // 晴れの時、MAXHPの 1/8 減る
    //@バトル変更箇所
    if( calc::IsShineWeather(weather) )
    {
      flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );
      {
        BTL_HANDEX_PARAM_DAMAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_DAMAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_DAMAGE, pokeID ));
          param->pokeID = pokeID;
          param->damage = calc::QuotMaxHP( bpp, 8 );
          param->damageCause = DAMAGE_CAUSE_KANSOUHADA;
          param->damageCausePokeID = flowWk->Hnd_GetWeatherCausePokeID();
        flowWk->HANDEX_Pop( param );
      }
      flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );

    // 雨の時、MAXHPの 1/8 回復
    }
    else if( calc::IsRainWeather(weather) )    //@バトル変更箇所
    {
      BTL_HANDEX_PARAM_RECOVER_HP* param = reinterpret_cast<BTL_HANDEX_PARAM_RECOVER_HP *>( flowWk->HANDEX_Push( BTL_HANDEX_RECOVER_HP, pokeID ));
        param->header.tokwin_flag = true;
        param->pokeID = pokeID;
        param->recoverHP = calc::QuotMaxHP( bpp, 8 );
      flowWk->HANDEX_Pop( param );
    }
  }
}
// ダメージワザ回復化チェックハンドラ
void handler_Kansouhada_DmgRecover( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  common_DmgToRecover_Check( flowWk, pokeID, work, POKETYPE_MIZU );
}
// ワザ威力計算ハンドラ
void handler_Kansouhada_WazaPow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 自分が防御側の時、ほのおワザの威力を1.25倍
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_HONOO )
    {
      EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.25) );
    }
  }
}



// 無効化チェックハンドラ
void handler_Kansouhada_Check( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( common_TypeNoEffect( flowWk, pokeID, POKETYPE_MIZU ) )
  {
    common_TypeRecoverHP( flowWk, pokeID, 4 );
  }
}

const BtlEventHandlerTable*  HAND_TOK_ADD_Kansouhada( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WEATHER_REACTION,      handler_Kansouhada_Weather    },  // 天候ダメージ計算ハンドラ
    { BTL_EVENT_WAZA_POWER,            handler_Kansouhada_WazaPow    },  // ワザ威力計算ハンドラ

//    { BTL_EVENT_DMG_TO_RECOVER_CHECK,  handler_Kansouhada_DmgRecover    },  // ダメージワザ回復チェックハンドラ
    { BTL_EVENT_NOEFFECT_CHECK_TOKUSEI,     handler_Kansouhada_Check  },  // 無効化チェックハンドラ

  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ちょすい」
 */
//------------------------------------------------------------------------------
// ダメージワザ回復化チェックハンドラ
void handler_Tyosui_Check( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( common_TypeNoEffect( flowWk, pokeID, POKETYPE_MIZU ) )
  {
    common_TypeRecoverHP( flowWk, pokeID, 4 );
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Tyosui( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
//    { BTL_EVENT_DMG_TO_RECOVER_CHECK,   handler_Tyosui_Check }, // ダメージワザ回復チェックハンドラ
    { BTL_EVENT_NOEFFECT_CHECK_TOKUSEI,      handler_Tyosui_Check },   // 無効化チェックハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ちくでん」
 */
//------------------------------------------------------------------------------
// ワザ無効チェックレベル３
void handler_Tikuden_CheckEx( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( common_TypeNoEffect( flowWk, pokeID, POKETYPE_DENKI ) )
  {
    common_TypeRecoverHP( flowWk, pokeID, 4 );
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Tikuden( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_TOKUSEI,      handler_Tikuden_CheckEx },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「でんきエンジン」
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_DenkiEngine( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_TOKUSEI,      handler_DenkiEngine_CheckEx },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// ワザ無効チェックレベル３
void handler_DenkiEngine_CheckEx( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( common_TypeNoEffect( flowWk, pokeID, POKETYPE_DENKI ) )
  {
    common_TypeNoEffect_Rankup( flowWk, pokeID, pml::wazadata::RANKEFF_AGILITY, 1 );
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「ノーガード」
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
 *  とくせい「きもったま」
 */
//------------------------------------------------------------------------------
// タイプによる無効化チェックハンドラ
void handler_Kimottama( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 自分が攻撃側の時
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ゴーストタイプへの無効化を解除
    if( EVENTVAR_GetValue(BTL_EVAR_POKE_TYPE) == POKETYPE_GHOST )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_FLAT_FLAG, true );
    }
  }
}
// 強制ひん死ハンドラ
void handler_Kimottama_kill( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  // 「じばく」などで自殺しようとしている時、タイプ無効化チェックまで生きるために特殊ワーク書き換え
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  ){
    work[WIDX_REMOVE_GUARD] = 1;
  }
}
// ワザシーケンス終了ハンドラ
void handler_Kimottama_check( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WIDX_REMOVE_GUARD] ){
    EVENT_FACTOR_Remove( myHandle );
  }
}

const BtlEventHandlerTable*  HAND_TOK_ADD_Kimottama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_AFFINITY,               handler_Kimottama       }, // 相性計算
    { BTL_EVENT_CHECK_AFFINITY_ONLY_ATTACKER, handler_Kimottama       }, // 相性計算( 攻撃側のイベントハンドラのみが反応し、防御側の特性等を考慮しない )
    { BTL_EVENT_KILL_HANDLER,                 handler_Kimottama_kill  }, // 強制ひんしハンドラ
    { BTL_EVENT_WAZASEQ_END,                  handler_Kimottama_check }, // ワザシーケンス終了ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ぼうおん」
 */
//------------------------------------------------------------------------------
// 無効化チェックLv3ハンドラ
void handler_Bouon( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 自分が防御側の時
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // サウンドワザ無効
    WazaNo waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    if( WAZADATA_GetFlag( waza, pml::wazadata::WAZAFLAG_Sound ) )
    {
      if( EVENTVAR_RewriteValue(BTL_EVAR_NOEFFECT_FLAG, true) )
      {
        flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );
        {
          BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
            HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_NoEffect );
            HANDEX_STR_AddArg( &param->str, pokeID );
          flowWk->HANDEX_Pop( param );
        }
        flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
      }
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Bouon( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_TOKUSEI,        handler_Bouon },  // 無効化チェックLv3ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ふゆう」
 */
//------------------------------------------------------------------------------
// 飛行フラグチェックハンドラ
void handler_Fuyuu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_GEN_FLAG) == false )
    {
      work[WIDX0] = EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
    }
  }
}
// 飛行フラグによる地面ワザ無効化ハンドラ
void handler_Fuyuu_Disp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (work[WIDX0])
  ){
    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      param->header.tokwin_flag = true;
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_NoEffect );
      HANDEX_STR_AddArg( &param->str, pokeID );
    flowWk->HANDEX_Pop( param );
    work[WIDX0] = 0;
  }
}
// ターンチェック開始ハンドラ
void handler_Fuyuu_TurnCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    work[WIDX0] = 0;
  }
}

const BtlEventHandlerTable*  HAND_TOK_ADD_Fuyuu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_FLYING,          handler_Fuyuu      },  // 飛行フラグチェックハンドラ
    { BTL_EVENT_WAZA_NOEFF_BY_FLYING,  handler_Fuyuu_Disp },  // 飛行フラグによる地面ワザ無効化ハンドラ
    { BTL_EVENT_TURNCHECK_BEGIN,       handler_Fuyuu_TurnCheck },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ふしぎなまもり」
 */
//------------------------------------------------------------------------------
// 無効化チェックLv2ハンドラ
void handler_FusiginaMamori( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 自分が防御側で
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID)
  ){
    // 「わるあがき」以外のダメージワザの場合
    WazaID waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    if( WAZADATA_IsDamage(waza) && (waza != WAZANO_WARUAGAKI) )
    {
      // 効果バツグン以外は無効
      BtlTypeAff aff = static_cast<BtlTypeAff>(EVENTVAR_GetValue( BTL_EVAR_TYPEAFF ));
      if( aff <= pml::battle::TypeAffinity::TYPEAFF_1 )
      {
        if( EVENTVAR_RewriteValue(BTL_EVAR_NOEFFECT_FLAG, true) )
        {
          flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );
          {
            BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
              HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_NoEffect );
              HANDEX_STR_AddArg( &param->str, pokeID );
            flowWk->HANDEX_Pop( param );
          }
          flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
        }
      }
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_FusiginaMamori( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_TOKUSEI,        handler_FusiginaMamori }, // 無効化チェックLv2ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「なまけ」
 */
//------------------------------------------------------------------------------
// ワザ出し成否チェックハンドラ
void handler_Namake( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( work[WIDX0] )
    {
      work[WIDX1] = EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_TOKUSEI );
      work[WIDX0] = 0;
    }
    else{
      work[WIDX0] = 1;
    }
  }
}
// とくせい書き換え直後ハンドラ
void handler_Namake_Get( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->TURNFLAG_Get( BTL_POKEPARAM::TURNFLG_ACTION_START) ){
      work[WIDX0] = 1;
    }
  }
}
// ワザだし失敗表示ハンドラ
void handler_Nameke_Failed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( work[WIDX1] )
    {
      BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        param->header.tokwin_flag = true;
        HANDEX_STR_Setup( &(param->str), BTL_STRTYPE_SET, BTL_STRID_SET_Namake );
        HANDEX_STR_AddArg( &(param->str), pokeID );
      flowWk->HANDEX_Pop( param );

      work[WIDX1] = 0;
    }
  }
}
// アクション終了ハンドラ
void handler_Nameke_EndAct( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // 反動ターンになまけフラグが残っているのはクリア
    // （アイテムターンも残ってるハズだが、シューターとの組み合わせが強いので何もしない）
    if( EVENTVAR_GetValue(BTL_EVAR_ACTION) == BTL_ACTION_SKIP )
    {
      work[WIDX0] = 0;
    }
  }
}

const BtlEventHandlerTable*  HAND_TOK_ADD_Namake( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_1ST,   handler_Namake        }, // ワザ出し成否チェックハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER,     handler_Namake_Get    }, // とくせい書き換え直後ハンドラ
    { BTL_EVENT_WAZA_EXECUTE_FAIL,        handler_Nameke_Failed }, // ワザ出し失敗表示ハンドラ
    { BTL_EVENT_ACTPROC_END,              handler_Nameke_EndAct }, // アクション終了ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  とくせい「しめりけ」
 */
//------------------------------------------------------------------------------
// ワザ出し成否チェックハンドラ
void handler_Simerike( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(pokeID);
  WazaID waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));

  work[WIDX0] = 0;

  if( (waza == WAZANO_DAIBAKUHATU)
  ||  (waza == WAZANO_ZIBAKU)
  ||  (waza == WAZANO_HANABISENYOU)
  ){
    ServerFlow::SV_WazaFailCause cause = static_cast<ServerFlow::SV_WazaFailCause>
                                            (EVENTVAR_GetValue( BTL_EVAR_FAIL_CAUSE ));
    if( cause == ServerFlow::SV_WAZAFAIL_NULL )
    {
      work[WIDX0] = EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_TOKUSEI );
      work[WIDX1] = waza;
    }
  }
}
void handler_Simerike_Effective( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( work[WIDX0] )
  {
    u8  targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );

    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      param->header.tokwin_flag = true;
      HANDEX_STR_Setup( &(param->str), BTL_STRTYPE_SET, BTL_STRID_SET_WazaCantUse );
      HANDEX_STR_AddArg( &(param->str), targetPokeID );
      HANDEX_STR_AddArg( &(param->str), work[WIDX1] );
    flowWk->HANDEX_Pop( param );

    work[WIDX0] = 0;
  }
}
// ワザ処理開始ハンドラ
void handler_Simerike_StartSeq( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(pokeID);
  GFL_UNUSED(work);
  EVENT_FACTOR_AttachSkipCheckHandler( myHandle, handler_Simerike_SkipCheck );
}
// ワザ処理終了ハンドラ
void handler_Simerike_EndSeq( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(pokeID);
  GFL_UNUSED(work);
  EVENT_FACTOR_DettachSkipCheckHandler( myHandle );
}
void handler_Simerike_Ieki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID ){
    EVENT_FACTOR_DettachSkipCheckHandler( myHandle );
  }
}

// スキップチェックハンドラ
bool handler_Simerike_SkipCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, BtlEventFactorType factorType, BtlEventType eventType, u16 subID, u8 pokeID )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(eventType);
  GFL_UNUSED(pokeID);
  // とくせい「しめりけ」だけ無効
  if( factorType == BTL_EVENT_FACTOR_TOKUSEI )
  {
    if( subID == TOKUSEI_YUUBAKU ){
      return true;
    }
  }
  return false;
}

const BtlEventHandlerTable*  HAND_TOK_ADD_Simerike( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD,   handler_Simerike            },  // ワザ出し成否チェックハンドラ
    { BTL_EVENT_WAZA_EXECUTE_FAIL,        handler_Simerike_Effective  },
    { BTL_EVENT_WAZASEQ_START,            handler_Simerike_StartSeq   },  // ワザ処理開始ハンドラ
    { BTL_EVENT_WAZASEQ_END,              handler_Simerike_EndSeq     },  // ワザ処理終了ハンドラ
    { BTL_EVENT_IEKI_FIXED,               handler_Simerike_Ieki       },  // いえき確定

  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  とくせい「もらいび」
 */
//------------------------------------------------------------------------------
// ダメージワザ回復化チェックハンドラ
void handler_Moraibi_NoEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 自分が防御側の時
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // 炎ワザは無効＆「もらいび」発動
    pml::PokeType waza_type = EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
    if( waza_type == POKETYPE_HONOO )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
    }
  }
}
// ダメージワザ回復化決定ハンドラ
void handler_Moraibi_DmgRecoverFix( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->CONTFLAG_Get( BTL_POKEPARAM::CONTFLG_MORAIBI) == false )
    {
      flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );
      {
        BTL_HANDEX_PARAM_SET_CONTFLAG* flg_param;
        BTL_HANDEX_PARAM_MESSAGE* param;

        param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
          HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_MoraibiExe );
          HANDEX_STR_AddArg( &param->str, pokeID );
        flowWk->HANDEX_Pop( param );

        flg_param = reinterpret_cast<BTL_HANDEX_PARAM_SET_CONTFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_CONTFLAG, pokeID ));
          flg_param->pokeID = pokeID;
          flg_param->flag = BTL_POKEPARAM::CONTFLG_MORAIBI;
        flowWk->HANDEX_Pop( flg_param );
      }
      flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
    }
  }
}
// 攻撃力決定ハンドラ
void handler_Moraibi_AtkPower( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 攻撃側が自分で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 「もらいび」状態の時
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->CONTFLAG_Get( BTL_POKEPARAM::CONTFLG_MORAIBI) )
    {
      // 炎ワザの威力1.5倍
      if( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_HONOO )
      {
        EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_MORAIBI_POWRATIO );
      }
    }
  }
}
// とくせい書き換え直前ハンドラ
void handler_Moraibi_Remove( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // 「もらいび」状態の解除

    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->CONTFLAG_Get( BTL_POKEPARAM::CONTFLG_MORAIBI) )
    {
      BTL_HANDEX_PARAM_RESET_CONTFLAG* param = reinterpret_cast<BTL_HANDEX_PARAM_RESET_CONTFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_RESET_CONTFLAG, pokeID ));
        param->pokeID = pokeID;
        param->flag = BTL_POKEPARAM::CONTFLG_MORAIBI;
      flowWk->HANDEX_Pop( param );
    }
  }
}
// 無効化チェックハンドラ
void handler_Moraibi_CheckNoEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( common_TypeNoEffect( flowWk, pokeID, POKETYPE_HONOO ) )
  {
    flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );
    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
      if( bpp->CONTFLAG_Get( BTL_POKEPARAM::CONTFLG_MORAIBI) == false )
      {
        BTL_HANDEX_PARAM_MESSAGE* param;
        BTL_HANDEX_PARAM_SET_CONTFLAG* flg_param;

        param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
          HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_MoraibiExe );
          HANDEX_STR_AddArg( &param->str, pokeID );
        flowWk->HANDEX_Pop( param );

        flg_param = reinterpret_cast<BTL_HANDEX_PARAM_SET_CONTFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_CONTFLAG, pokeID ));
          flg_param->pokeID = pokeID;
          flg_param->flag = BTL_POKEPARAM::CONTFLG_MORAIBI;
        flowWk->HANDEX_Pop( flg_param );
      }
      else
      {
        BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
          HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_NoEffect );
          HANDEX_STR_AddArg( &param->str, pokeID );
        flowWk->HANDEX_Pop( param );
      }
    }
    flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
  }
}

const BtlEventHandlerTable*  HAND_TOK_ADD_Moraibi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
//    { BTL_EVENT_DMG_TO_RECOVER_CHECK,  handler_Moraibi_NoEffect      }, // ダメージワザ回復化チェックハンドラ
//    { BTL_EVENT_DMG_TO_RECOVER_FIX,    handler_Moraibi_DmgRecoverFix }, // ダメージ回復化決定ハンドラ
    { BTL_EVENT_NOEFFECT_CHECK_TOKUSEI,     handler_Moraibi_CheckNoEffect }, // 無効化チェックハンドラ

    { BTL_EVENT_ATTACKER_POWER,        handler_Moraibi_AtkPower      }, // 攻撃力決定ハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_BEFORE, handler_Moraibi_Remove        }, // とくせい書き換え直前ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ナイトメア」
 */
//------------------------------------------------------------------------------
// ターンチェック最終ハンドラ
void handler_Nightmare( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp;
    u8 ePokeID[ BTL_POSIDX_MAX ];
    u8 pokeCnt, i;

    u8 exe_flag = false;
    BtlPokePos myPos = flowWk->Hnd_GetExistFrontPokePos( pokeID );
    BtlExPos   expos = EXPOS_MAKE( BTL_EXPOS_AREA_ENEMY, myPos );

    pokeCnt = flowWk->Hnd_ExpandPokeID( expos, ePokeID );

    for(i=0; i<pokeCnt; ++i)
    {
      bpp = flowWk->Hnd_GetPokeParam( ePokeID[i] );
      if( bpp->CheckNemuri( BTL_POKEPARAM::NEMURI_CHECK_INCLUDE_ZETTAINEMURI ) )
      {
        BTL_HANDEX_PARAM_DAMAGE* param;

        if( exe_flag == false ){
          exe_flag = true;
          flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );
        }

        param = reinterpret_cast<BTL_HANDEX_PARAM_DAMAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_DAMAGE, pokeID ));
          param->pokeID = ePokeID[i];
          param->damage = calc::QuotMaxHP( bpp, 8 );
          param->damageCause = DAMAGE_CAUSE_NIGHTMARE;
          param->damageCausePokeID = pokeID;
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_NightMare );
          HANDEX_STR_AddArg( &param->exStr, ePokeID[i] );
        flowWk->HANDEX_Pop( param );
      }
    }
    if( exe_flag ){
      flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Nightmare( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_END,        handler_Nightmare },  // ターンチェック最終ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「にげあし」
 */
//------------------------------------------------------------------------------
// にげるチェックハンドラ
void handler_Nigeasi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  HandCommon_NigeruCalcSkip( myHandle, flowWk, pokeID, work );
}
// にげ確定メッセージハンドラ
void handler_Nigeasi_Msg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( HandCommon_CheckNigeruExMsg(myHandle, flowWk, pokeID) )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      param->header.tokwin_flag = true;
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_EscapeSuccess );
      HANDEX_STR_AddSE( &param->str, (u32)SEQ_SE_BT_NIGERU );
      flowWk->HANDEX_Pop( param );
  }
}

const BtlEventHandlerTable*  HAND_TOK_ADD_Nigeasi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_SKIP_NIGERU_CALC,     handler_Nigeasi       }, // にげるチェックハンドラ
    { BTL_EVENT_NIGERU_EXMSG,         handler_Nigeasi_Msg   }, // 逃げ特殊メッセージハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「かたやぶり」
 */
//------------------------------------------------------------------------------
// メンバー入場ハンドラ
void handler_Katayaburi_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param;
    u16 tokID = EVENT_FACTOR_GetSubID( myHandle );
    u16 strID;

    switch( tokID ){
    case TOKUSEI_TAABOBUREIZU:  strID = BTL_STRID_SET_TurboBraze; break;
    case TOKUSEI_TERABORUTEEZI: strID = BTL_STRID_SET_TeraVoltage; break;
    default:  strID = BTL_STRID_SET_Katayaburi; break;
    }

    flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );

    param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, strID );
      HANDEX_STR_AddArg( &param->str, pokeID );
    flowWk->HANDEX_Pop( param );

    flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
  }
}
// ワザ処理開始ハンドラ
void handler_Katayaburi_Start( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  // 自分が攻撃側ならスキップチェックハンドラをアタッチする
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( work[WIDX0] == 0 ){
      EVENT_FACTOR_AttachSkipCheckHandler( myHandle, HandCommon_Katayaburi_SkipCheck );
      work[WIDX0] = 1;
    }
  }
}
// ワザ処理終了ハンドラ
void handler_Katayaburi_End( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  // 自分が攻撃側ならスキップチェックハンドラをデタッチする
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( work[WIDX0] == 1 ){
      EVENT_FACTOR_DettachSkipCheckHandler( myHandle );
      work[WIDX0] = 0;
    }
  }
}
// いえき確定
void handler_Katayaburi_Ieki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  // スキップチェックハンドラをデタッチ
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID ){
    if( work[WIDX0] == 1 ){
      EVENT_FACTOR_DettachSkipCheckHandler( myHandle );
      work[WIDX0] = 0;
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Katayaburi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,            handler_Katayaburi_MemberIn },  // 入場ハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_Katayaburi_MemberIn },  // とくせい書き換えハンドラ
    { BTL_EVENT_WAZASEQ_START,        handler_Katayaburi_Start    },  // ワザ処理開始ハンドラ
    { BTL_EVENT_WAZASEQ_END,          handler_Katayaburi_End      },  // ワザ処理終了ハンドラ
    { BTL_EVENT_IEKI_FIXED,           handler_Katayaburi_Ieki     },  // いえき確定
  };

  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「てんきや」
 */
//------------------------------------------------------------------------------
// ポケ入場完了ハンドラ
void handler_Tenkiya_MemberInComp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  BtlWeather weather = flowWk->Hnd_GetWeather( );
  common_TenkiFormChange( flowWk, pokeID, weather );
  work[WIDX0] = 1;
}
// ポケ入場ハンドラ
void handler_Tenkiya_GetTok( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    handler_Tenkiya_MemberInComp( myHandle, flowWk, pokeID, work );
  }
}
// 天候変化ハンドラ
void handler_Tenkiya_Weather( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( work[WIDX0] )
  {
    BtlWeather weather = flowWk->Hnd_GetWeather( );
    common_TenkiFormChange( flowWk, pokeID, weather );
  }
}
// エアロック通知ハンドラ
void handler_Tenkiya_AirLock( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WIDX0] )
  {
    common_Tenkiya_Off( myHandle, flowWk, pokeID );
  }
}

// とくせい書き換え直前ハンドラ
void handler_Tenkiya_ChangeTok( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  // 「てんきや」以外のとくせいに書き換わる時、フォルムをノーマルに戻す
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_TOKUSEI_NEXT) != EVENT_FACTOR_GetSubID(myHandle) )
    {
      common_Tenkiya_Off( myHandle, flowWk, pokeID );
    }
  }
}
// いえき確定
void handler_Tenkiya_TokOff( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    common_Tenkiya_Off( myHandle, flowWk, pokeID );
  }
}
void common_Tenkiya_Off( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID )
{
  GFL_UNUSED(myHandle);
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( (bpp->GetMonsNo() == MONSNO_POWARUN)
    &&  (bpp->GetFormNo() != FORMNO_POWARUN_NORMAL)
    ){
      BTL_HANDEX_PARAM_CHANGE_FORM* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_FORM *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_FORM, pokeID ));
        param->pokeID = pokeID;
        param->formNo = FORMNO_POWARUN_NORMAL;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_ChangeForm );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
      flowWk->HANDEX_Pop( param );
    }
  }
}

void common_TenkiFormChange( ServerFlow* flowWk, u8 pokeID, BtlWeather weather )
{
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  if( bpp->GetMonsNo() == MONSNO_POWARUN )
  {
    u8 form_now, form_next;

    form_now = bpp->GetFormNo();
    switch( weather ){
    case BTL_WEATHER_NONE:  form_next = FORMNO_POWARUN_NORMAL; break;
    case BTL_WEATHER_SHINE: form_next = FORMNO_POWARUN_SUN; break;
    case BTL_WEATHER_DAY:   form_next = FORMNO_POWARUN_SUN; break; //@バトル変更箇所
    case BTL_WEATHER_RAIN:  form_next = FORMNO_POWARUN_RAIN; break;
    case BTL_WEATHER_STORM: form_next = FORMNO_POWARUN_RAIN; break; //@バトル変更箇所
    case BTL_WEATHER_SNOW:  form_next = FORMNO_POWARUN_SNOW; break;
    default:
      form_next = FORMNO_POWARUN_NORMAL;
    }

    if( form_next != form_now )
    {
      BTL_HANDEX_PARAM_CHANGE_FORM* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_FORM *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_FORM, pokeID ));
        param->header.tokwin_flag = true;
        param->pokeID = pokeID;
        param->formNo = form_next;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_ChangeForm );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
      flowWk->HANDEX_Pop( param );
    }
  }
}

const BtlEventHandlerTable*  HAND_TOK_ADD_Tenkiya( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN_COMP,        handler_Tenkiya_MemberInComp  },  // ポケ入場ハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER,  handler_Tenkiya_GetTok        },  // とくせい書き換え直後ハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_BEFORE, handler_Tenkiya_ChangeTok     },  // とくせい書き換え直前ハンドラ
    { BTL_EVENT_IEKI_FIXED,            handler_Tenkiya_TokOff        },  // いえき確定ハンドラ
    { BTL_EVENT_NOTIFY_AIRLOCK,        handler_Tenkiya_AirLock       },  // エアロック通知ハンドラ
    { BTL_EVENT_ACTPROC_END,           handler_Tenkiya_Weather       },
    { BTL_EVENT_TURNCHECK_DONE,        handler_Tenkiya_Weather       },
    { BTL_EVENT_WEATHER_CHANGE_AFTER,  handler_Tenkiya_Weather       },  // 天候変化後ハンドラ

  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「よびみず」
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Yobimizu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TEMPT_TARGET,         handler_Yobimizu                    }, // ワザターゲット誘導
    { BTL_EVENT_TEMPT_TARGET_END,     handler_Yobimizu_TemptTargetEnd     }, // ワザターゲット誘導終了
    { BTL_EVENT_WAZA_EXE_START,       handler_Hiraisin_WazaExeStart       }, // ワザ実行開始ハンドラ
    { BTL_EVENT_NOEFFECT_CHECK_TOKUSEI,    handler_Yobimizu_CheckNoEffect }, // 無効化チェックハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ワザターゲット引き寄せハンドラ
void handler_Yobimizu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  common_WazaTargetChangeToMe( flowWk, pokeID, work, POKETYPE_MIZU, TEMPT_TARGET_PRIORITY_TOKUSEI, TEMPT_TARGET_CAUSE_YOBIMIZU );
}
// ワザターゲット誘導終了
// 最終的に、自身の「よびみず」によりターゲットを誘導したかどうかをチェックする
void handler_Yobimizu_TemptTargetEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  u8 temptPokeId = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  TemptTargetCause temptCause = static_cast<TemptTargetCause>( EVENTVAR_GetValue( BTL_EVAR_TEMPT_TARGET_CAUSE ) );
  if( ( temptPokeId == pokeID ) &&
      ( temptCause == TEMPT_TARGET_CAUSE_YOBIMIZU ) )
  {
    work[WIDX0] = 1;
  }
}
// 無効化チェックハンドラ
void handler_Yobimizu_CheckNoEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( common_TypeNoEffect(flowWk, pokeID, POKETYPE_MIZU) )
  {
    common_TypeNoEffect_Rankup( flowWk, pokeID, pml::wazadata::RANKEFF_SP_ATTACK, 1 );
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「ひらいしん」
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Hiraisin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TEMPT_TARGET,         handler_Hiraisin                    }, // ワザターゲット誘導
    { BTL_EVENT_TEMPT_TARGET_END,     handler_Hiraisin_TemptTargetEnd     }, // ワザターゲット誘導終了
    { BTL_EVENT_WAZA_EXE_START,       handler_Hiraisin_WazaExeStart       }, // ワザ実行開始ハンドラ
    { BTL_EVENT_NOEFFECT_CHECK_TOKUSEI,    handler_Hiraisin_CheckNoEffect }, // 無効化チェックハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// ワザターゲット誘導
void handler_Hiraisin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  common_WazaTargetChangeToMe( flowWk, pokeID, work, POKETYPE_DENKI, TEMPT_TARGET_PRIORITY_TOKUSEI, TEMPT_TARGET_CAUSE_HIRAISIN );
}
// ワザターゲット誘導終了
// 最終的に、自身の「ひらいしん」によりターゲットを誘導したかどうかをチェックする
void handler_Hiraisin_TemptTargetEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  u8 temptPokeId = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  TemptTargetCause temptCause = static_cast<TemptTargetCause>( EVENTVAR_GetValue( BTL_EVAR_TEMPT_TARGET_CAUSE ) );
  if( ( temptPokeId == pokeID ) &&
      ( temptCause == TEMPT_TARGET_CAUSE_HIRAISIN ) )
  {
    work[WIDX0] = 1;
  }
}
// ワザ実行開始ハンドラ
void handler_Hiraisin_WazaExeStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( work[WIDX0] )
  {
    if( HandCommon_CheckTargetPokeID(pokeID) )
    {
      BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        param->header.tokwin_flag = true;
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_WazaRecv );
        HANDEX_STR_AddArg( &param->str, pokeID );
      flowWk->HANDEX_Pop( param );
    }
    work[WIDX0] = 0;
  }
}
// 無効化チェックハンドラ
void handler_Hiraisin_CheckNoEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( common_TypeNoEffect(flowWk, pokeID, POKETYPE_DENKI) )
  {
    common_TypeNoEffect_Rankup( flowWk, pokeID, pml::wazadata::RANKEFF_SP_ATTACK, 1 );
  }
}

//--------------------------------------------------------------------------
/**
 * とくていタイプのワザの対象を強制的に自分にするとくせいの共通処理
 *
 * @param   flowWk
 * @param   pokeID
 * @param   wazaType
 *
 */
//--------------------------------------------------------------------------
void common_WazaTargetChangeToMe( ServerFlow* flowWk, u8 pokeID, int* work, pml::PokeType wazaType, TemptTargetPriority temptPriority, TemptTargetCause temptCause )
{
  GFL_UNUSED(work);

  // 自分の攻撃には影響しない
  if( EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK ) == pokeID )
  {
    return;
  }

  // 指定したタイプの技にしか影響しない
  if( EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE ) != wazaType )
  {
    return;
  }

  // 合体技・フリーフォールには影響しない
  WazaID waza = static_cast<WazaNo>( EVENTVAR_GetValue( BTL_EVAR_WAZAID ) );
  if( ( tables::IsCombiWaza( waza ) ) ||
        ( waza == WAZANO_HURIIFOORU ) )
  {
    return;
  }

  HandCommon_TemptTarget( pokeID, temptPriority, temptCause );
}

//------------------------------------------------------------------------------
/**
 *  とくせい「きゅうばん」
 */
//------------------------------------------------------------------------------
// ふきとばしチェックハンドラ
void handler_Kyuuban( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    if( EVENTVAR_RewriteValue(BTL_EVAR_FAIL_FLAG, true) )
    {
      BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        param->header.tokwin_flag = true;
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Kyuuban );
        HANDEX_STR_AddArg( &param->str, pokeID );
      flowWk->HANDEX_Pop( param );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Kyuuban( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_PUSHOUT,    handler_Kyuuban   },  // ふきとばしチェックハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ヘドロえき」
 */
//------------------------------------------------------------------------------
// ドレイン量計算ハンドラ
void handler_HedoroEki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    u16 damage = EVENTVAR_GetValue( BTL_EVAR_VOLUME );
    EVENTVAR_RewriteValue( BTL_EVAR_VOLUME, 0 );

    if( damage )
    {
      // ダメージを与える
      // 
      // このダメージでは、死亡した場合でも退場処理を行いません。
      // ここで退場処理を行ってしまうと、両陣営が同時に全滅した場合の勝敗判定が反転してしまうためです。
      //
      // 両陣営が全滅した場合、後に退場したほうの勝ちとなりますが、
      // ここで退場処理を行ってしまうと
      // 防御側が技ダメージによって退場する前に
      // 攻撃側が「ヘドロえき」のダメージで退場してしまうため、攻撃側が負けになってしまいます。
      BTL_HANDEX_PARAM_DAMAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_DAMAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_DAMAGE, pokeID ));
        param->header.tokwin_flag  = true;
        param->pokeID              = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
        param->damage              = damage;
        param->damageCause         = DAMAGE_CAUSE_HEDOROEKI;
        param->damageCausePokeID   = pokeID;
        param->fDisableDeadProcess = true;                         // 死亡した場合でも退場処理を行わない
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_HedoroEki );
        HANDEX_STR_AddArg( &param->exStr, param->pokeID );
      flowWk->HANDEX_Pop( param );
    }
  }
}
// 死亡通知ハンドラ
void handler_HedoroEki_Dead( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    EVENT_FACTOR_ConvertForIsolate( myHandle );
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_HedoroEki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_DRAIN_END,    handler_HedoroEki      },  // ドレイン量計算ハンドラ
    { BTL_EVENT_NOTIFY_DEAD,       handler_HedoroEki_Dead },  // 死亡通知ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ぶきよう」
 */
//------------------------------------------------------------------------------
bool handler_Bukiyou_SkipCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, BtlEventFactorType factorType, BtlEventType eventType, u16 subID, u8 pokeID )
{
  GFL_UNUSED(eventType);

  if( factorType == BTL_EVENT_FACTOR_ITEM )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->CheckSick(pml::wazadata::WAZASICK_IEKI) ){
      return false;
    }

    if( EVENT_FACTOR_GetPokeID(myHandle) == pokeID )
    {
      // これらのアイテムは無効化しない
      static const u16 IgnoreItems[] = {
        ITEM_KYOUSEIGIPUSU, ITEM_GAKUSYUUSOUTI, ITEM_OMAMORIKOBAN,  ITEM_KOUUNNOOKOU,
        ITEM_KIYOMENOOHUDA, ITEM_KAWARAZUNOISI, ITEM_SIAWASETAMAGO,
        ITEM_PAWAARISUTO,   ITEM_PAWAABERUTO,   ITEM_PAWAARENZU,    ITEM_PAWAABANDO,
        ITEM_PAWAAANKURU,   ITEM_PAWAAUEITO,
      };
      u32 i;
      for(i=0; i<GFL_NELEMS(IgnoreItems); ++i)
      {
        if( IgnoreItems[i] == subID ){
          return false;
        }
      }

      // こだわりアイテムが外れる時は無効化しない
      if( eventType == BTL_EVENT_ITEMSET_DECIDE )
      {
        if( (subID == ITEM_KODAWARIHATIMAKI)
        ||  (subID == ITEM_KODAWARIMEGANE)
        ||  (subID == ITEM_KODAWARISUKAAHU)
        ){
          return false;
        }
      }

      return true;
    }
  }

  return false;
}
void handler_Bukiyou_MemberInPrev( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(pokeID);
  if( work[WIDX0] == 0 ){
    EVENT_FACTOR_AttachSkipCheckHandler( myHandle, handler_Bukiyou_SkipCheck );
    work[WIDX0] = 1;
  }
}
// とくせい書き換え直後ハンドラ
void handler_Bukiyou_GetTokusei( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    handler_Bukiyou_MemberInPrev( myHandle, flowWk, pokeID, work );
  }
}
// とくせい書き換え直前ハンドラ
void handler_Bukiyou_PreChange( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    EVENT_FACTOR_DettachSkipCheckHandler( myHandle );
  }
}
// いえき確定ハンドラ
void handler_Bukiyou_IekiFixed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
//  EVENT_FACTOR_DettachSkipCheckHandler( myHandle );
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->GetItem() != ITEM_DUMMY_DATA )
    {
      BTL_HANDEX_PARAM_CHECK_ITEM_EQUIP* param = reinterpret_cast<BTL_HANDEX_PARAM_CHECK_ITEM_EQUIP *>( flowWk->HANDEX_Push( BTL_HANDEX_CHECK_ITEM_EQUIP, pokeID ));
        param->pokeID = pokeID;
      flowWk->HANDEX_Pop( param );
    }
  }
}

// ワザ出し成否チェックハンドラ
void handler_Bukiyou_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    WazaID waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));

    work[WIDX2] = 0;

    if( (waza == WAZANO_SIZENNOMEGUMI)
    &&  (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
    ){
      work[WIDX2] = EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_TOKUSEI );
    }
  }
}
// ワザ出し失敗確定ハンドラ
void handler_Bukiyou_ExeFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( work[WIDX2] )
    {
      BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        param->header.tokwin_flag = true;
        HANDEX_STR_Setup( &(param->str), BTL_STRTYPE_SET, BTL_STRID_SET_WazaCantUse );
        HANDEX_STR_AddArg( &(param->str), pokeID );
        HANDEX_STR_AddArg( &(param->str), EVENTVAR_GetValue(BTL_EVAR_WAZAID) );
      flowWk->HANDEX_Pop( param );

      work[WIDX2] = 0;
    }
  }
}

const BtlEventHandlerTable*  HAND_TOK_ADD_Bukiyou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN_PREV,           handler_Bukiyou_MemberInPrev },
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER,     handler_Bukiyou_GetTokusei}, // とくせい書き換えハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_BEFORE,    handler_Bukiyou_PreChange }, // とくせい書き換え直前ハンドラ
    { BTL_EVENT_IEKI_FIXED,               handler_Bukiyou_IekiFixed }, // いえき確定ハンドラ
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD,   handler_Bukiyou_ExeCheck  }, // ワザ出し成否チェックハンドラ
    { BTL_EVENT_WAZA_EXECUTE_FAIL,        handler_Bukiyou_ExeFail   }, // ワザ出し失敗確定ハンドラ
  };

  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ねんちゃく」
 */
//------------------------------------------------------------------------------
void handler_Nenchaku_NoEff( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 自分が防御側のとき
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // トリック・すりかえは無効
    WazaID waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    if( (waza == WAZANO_TORIKKU)
    ||  (waza == WAZANO_SURIKAE)
    ){
      if( EVENTVAR_RewriteValue(BTL_EVAR_NOEFFECT_FLAG, true) )
      {
        BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
          param->header.tokwin_flag = true;
          HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_NoEffect );
          HANDEX_STR_AddArg( &param->str, pokeID );
        flowWk->HANDEX_Pop( param );
      }
    }
  }
}
// アイテム書き換えチェックハンドラ
void handler_Nenchaku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_ITEM) == ITEM_DUMMY_DATA)
  ){
    work[WIDX0] = EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
  }
}
// アイテム書き換え失敗後反応ハンドラ
void handler_Nenchaku_Reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (work[WIDX0])
  ){
    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      param->header.tokwin_flag = true;
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Nenchaku );
      HANDEX_STR_AddArg( &param->str, pokeID );
    flowWk->HANDEX_Pop( param );
    work[WIDX0] = 0;
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Nenchaku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_TOKUSEI,    handler_Nenchaku_NoEff    },  // 無効化チェックLv2ハンドラ
    { BTL_EVENT_ITEMSET_CHECK,        handler_Nenchaku          },  // アイテム書き換えチェックハンドラ
    { BTL_EVENT_ITEMSET_FAILED,       handler_Nenchaku_Reaction },  // アイテム書き換え失敗後反応ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「プレッシャー」
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Pressure( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,            handler_Pressure_MemberIN },  // メンバー入場ハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_Pressure_MemberIN },  // とくせい書き換えハンドラ
    { BTL_EVENT_DECREMENT_PP,         handler_Pressure          },  // PP消費チェックハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// メンバー入場ハンドラ
void handler_Pressure_MemberIN( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  common_MemberInTokMessage( flowWk, pokeID, BTL_STRID_SET_Pressure );
}
// メンバー入場時ハンドラ等で、とくせい効果を宣言するだけの処理共通
void common_MemberInTokMessage( ServerFlow* flowWk, u8 pokeID, u16 strID )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );
    {
       BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
         HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, strID );
         HANDEX_STR_AddArg( &param->str, pokeID );
       flowWk->HANDEX_Pop( param );
    }
    flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
  }
}
// PP消費チェックハンドラ
void handler_Pressure( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  u8 atkPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );

  if( !flowWk->Hnd_GetMainModule()->IsFriendPokeID(atkPokeID, pokeID) )
  {
    bool fEnable = false;

    if( HandCommon_CheckTargetPokeID(pokeID) ){
      fEnable = true;
    }
    else{

      WazaID waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
      if( WAZADATA_GetCategory(waza) == pml::wazadata::CATEGORY_FIELD_EFFECT ){
        fEnable = true;
      }
      else if( tables::IsPressureEffectiveWaza(waza) ){
        fEnable = true;
      }
    }

    if( fEnable ){
      u8 vol = EVENTVAR_GetValue( BTL_EVAR_VOLUME ) + 1;
      EVENTVAR_RewriteValue( BTL_EVAR_VOLUME, vol );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  とくせい「マジックガード」
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_MagicGuard( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_SIMPLE_DAMAGE_ENABLE,  handler_MagicGuard },  // ワザ以外ダメージチェックハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ワザ以外ダメージチェックハンドラ
void handler_MagicGuard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, false );
  }
}
//------------------------------------------------------------------------------
/**
 * とくせい「あくしゅう」
 *
 * ひるみ確率０の打撃ワザを、確率10％に押し上げる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Akusyuu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_SHRINK_PER,  handler_Akusyuu }, // ひるみ確率チェックハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ひるみ確率チェックハンドラ
void handler_Akusyuu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 自分が攻撃側で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ひるみ確率０なら、確率10％に
    u8 per = EVENTVAR_GetValue( BTL_EVAR_ADD_DEFAULT_PER );
    if( per == 0 )
    {
      #ifdef PM_BTL_DEBUG
      if( flowWk->Hnd_GetDebugFlag( BTL_DEBUGFLAG_MUST_TOKUSEI) ){
        EVENTVAR_RewriteValue( BTL_EVAR_ADD_PER, 100 );
        return;
      }
      #endif

      EVENTVAR_RewriteValue( BTL_EVAR_ADD_PER, 10 );
    }
  }
}
//------------------------------------------------------------------------------
/**
 * とくせい「かげふみ」
 *
 * 相手が逃げるのを封じる
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Kagefumi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NIGERU_FORBID,  handler_Kagefumi }, // 逃げる禁止チェック
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// 逃げる禁止チェックハンドラ
void handler_Kagefumi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  u8 escPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );

  if( !flowWk->Hnd_GetMainModule()->IsFriendPokeID(escPokeID, pokeID) )
  {
    BtlPokePos myPos = flowWk->Hnd_GetExistFrontPokePos( pokeID );
    BtlExPos   expos = EXPOS_MAKE( BTL_EXPOS_AREA_ENEMY, myPos );
    u8 pokeIDAry[ BTL_POSIDX_MAX ];
    u8 cnt, i;

    // 相手に１体でも「かげふみ」がいるなら無効
    cnt = flowWk->Hnd_ExpandPokeID( expos, pokeIDAry );
    for(i=0; i<cnt; ++i)
    {
      const BTL_POKEPARAM* bppEnemy = flowWk->Hnd_GetPokeParam( pokeIDAry[i] );
      if( bppEnemy->GetValue( BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE) == TOKUSEI_KAGEHUMI ){
        return;
      }
    }

    EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
  }
}
//------------------------------------------------------------------------------
/**
 * とくせい「ありじごく」
 *
 * 相手が逃げるのを封じる（ふゆうしているポケモンは除外）
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Arijigoku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NIGERU_FORBID,  handler_Arijigoku }, // 逃げる禁止チェック
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// 逃げる禁止チェックハンドラ
void handler_Arijigoku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  u8 escPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );

  if( !flowWk->Hnd_GetMainModule()->IsFriendPokeID(escPokeID, pokeID) )
  {
    BtlPokePos myPos = flowWk->Hnd_GetExistFrontPokePos( pokeID );
    BtlExPos   expos = EXPOS_MAKE( BTL_EXPOS_AREA_ENEMY, myPos );
    u8 pokeIDAry[ BTL_POSIDX_MAX ];
    u8 cnt, i;

    // 相手に１体でもふゆう状態でないポケがいれば有効
    cnt = flowWk->Hnd_ExpandPokeID( expos, pokeIDAry );
    for(i=0; i<cnt; ++i)
    {
      if( !flowWk->Hnd_IsFloatingPoke( pokeIDAry[i]) )
      {
        EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
        break;;
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 * とくせい「じりょく」
 *
 * 相手が逃げるのを封じる（ふゆうしているポケモンは除外）
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Jiryoku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NIGERU_FORBID,  handler_Jiryoku }, // 逃げる禁止チェック
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// 逃げる禁止チェックハンドラ
void handler_Jiryoku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  u8 escPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );

  if( !flowWk->Hnd_GetMainModule()->IsFriendPokeID(escPokeID, pokeID) )
  {
    BtlPokePos myPos = flowWk->Hnd_GetExistFrontPokePos( pokeID );
    BtlExPos   expos = EXPOS_MAKE( BTL_EXPOS_AREA_ENEMY, myPos );
    u8 pokeIDAry[ BTL_POSIDX_MAX ];
    u8 cnt, i;


    // 相手に１体でも「はがね」タイプがいたら有効
    cnt = flowWk->Hnd_ExpandPokeID( expos, pokeIDAry );
    for(i=0; i<cnt; ++i)
    {
      const BTL_POKEPARAM* bppEnemy = flowWk->Hnd_GetPokeParam( pokeIDAry[i] );
      if( bppEnemy->IsMatchType( POKETYPE_HAGANE) )
      {
        EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
      }
    }
  }
}




//------------------------------------------------------------------------------
/**
 * とくせい「かるわざ」
 *
 * 持っている道具がなくなると、身軽になり素早さの元の値を２倍にする。
 * 道具をまた持つと下がる。
 * 最初から道具がない場合は発動しない。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Karuwaza( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ITEMSET_DECIDE,  handler_Karuwaza_BeforeItemSet  }, // アイテム書き換え直前
    { BTL_EVENT_CALC_AGILITY,    handler_Karuwaza_Agility        }, // すばやさ計算ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// アイテム書き換え直前ハンドラ
void handler_Karuwaza_BeforeItemSet( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_ITEM) == ITEM_DUMMY_DATA)
  ){
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->GetItem() != ITEM_DUMMY_DATA )
    {
      work[WIDX0] = 1;    // アイテム消費したらフラグON
    }
  }
}
// すばやさ計算ハンドラ
void handler_Karuwaza_Agility( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // フラグON でアイテム所持していない状態ならすばやさ２倍
    if( work[WIDX0] == 1 )
    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
      if( bpp->GetItem() == ITEM_DUMMY_DATA )
      {
        EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * とくせい「ものひろい」
 *
 * 戦闘中、相手が使い切ったものをひろってくる。（ターンエンドに）
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Monohiroi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_END,    handler_Monohiroi },  // ターンチェック開始ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ターンチェック終了ハンドラ
void handler_Monohiroi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bppMe = flowWk->Hnd_GetPokeParam( pokeID );
    if( bppMe->GetItem() == ITEM_DUMMY_DATA )
    {
      typedef struct {
        u8 pokeID[ BTL_EXIST_POS_MAX ];
        u8 consumedPokeID[ BTL_EXIST_POS_MAX ];
        u8 pokeCnt;
        u8 consumedPokeCnt;
      }MONOHIROI_WORK;

      BtlPokePos myPos = flowWk->Hnd_GetExistFrontPokePos( pokeID );
      BtlExPos   expos = EXPOS_MAKE( BTL_EXPOS_AREA_OTHERS, myPos );

      MONOHIROI_WORK* mwk = reinterpret_cast<MONOHIROI_WORK*>(flowWk->Hnd_GetTmpWork( sizeof(MONOHIROI_WORK) ));
      const BTL_POKEPARAM* bpp;
      u8 i, cnt;

      mwk->pokeCnt = flowWk->Hnd_ExpandPokeID( expos, mwk->pokeID );
      mwk->consumedPokeCnt = 0;

      // 今ターン、アイテムを消費したポケモンを記録
      cnt = 0;
      for(i=0; i<mwk->pokeCnt; ++i)
      {
        bpp = flowWk->Hnd_GetPokeParam( mwk->pokeID[i] );
        if( (bpp->TURNFLAG_Get( BTL_POKEPARAM::TURNFLG_ITEM_CONSUMED))
        &&  (bpp->GetConsumedItem() != ITEM_DUMMY_DATA)
        ){
          mwk->consumedPokeID[cnt++] = bpp->GetID();
        }
      }

      if( cnt )
      {
        u8 rndIdx = calc::GetRand( cnt );
        u8 targetPokeID = mwk->consumedPokeID[rndIdx];
        u16 itemID;

        bpp = flowWk->Hnd_GetPokeParam( targetPokeID );
        itemID = bpp->GetConsumedItem();

        if( (itemID != ITEM_DUMMY_DATA)
        &&  (!HandCommon_IsUnbreakablePokeItem(flowWk, bppMe->GetMonsNo(), itemID))
        ){
          BTL_HANDEX_PARAM_SET_ITEM* setParam;

          setParam = reinterpret_cast<BTL_HANDEX_PARAM_SET_ITEM *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_ITEM, pokeID ));
            setParam->header.tokwin_flag = true;
            setParam->pokeID = pokeID;
            setParam->itemID = itemID;
            setParam->fClearConsumeOtherPoke = true;
            setParam->clearConsumePokeID = targetPokeID;
            HANDEX_STR_Setup( &setParam->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Monohiroi );
            HANDEX_STR_AddArg( &setParam->exStr, pokeID );
            HANDEX_STR_AddArg( &setParam->exStr, itemID );
          flowWk->HANDEX_Pop( setParam );
        }
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * とくせい「わるいてぐせ」
 *
 * 打撃攻撃（接触フラグONのダメージワザ）を受けた際、
 * 自分の道具に空きがあれば相手の持ち物を奪い取る。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_WaruiTeguse( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_L5,        handler_WaruiTeguse },  // ダメージワザ処理終了処理ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ダメージワザ処理終了処理ハンドラ
void handler_WaruiTeguse( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( HandCommon_CheckTargetPokeID(pokeID) )
  {
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    if( !HandCommon_CheckCantStealPoke(flowWk, pokeID, targetPokeID) )
    {
      if( EVENTVAR_GetValue( BTL_EVAR_WAZA_TOUCH_FLAG ) )
      {
        const BTL_POKEPARAM* me = flowWk->Hnd_GetPokeParam( pokeID );
        const BTL_POKEPARAM* attacker = flowWk->Hnd_GetPokeParam( targetPokeID );
        u16 attackersItem = attacker->GetItem();

        if( (me->GetItem() == ITEM_DUMMY_DATA)
        &&  (attackersItem != ITEM_DUMMY_DATA)
        ){
          BTL_HANDEX_PARAM_SWAP_ITEM* param = reinterpret_cast<BTL_HANDEX_PARAM_SWAP_ITEM *>( flowWk->HANDEX_Push( BTL_HANDEX_SWAP_ITEM, pokeID ));
            param->header.tokwin_flag = true;
            param->pokeID = targetPokeID;
            param->fIncRecordCount_StealItemFromWildPoke = true;
            HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_WaruiTeguse );
            HANDEX_STR_AddArg( &param->exStr, targetPokeID );
            HANDEX_STR_AddArg( &param->exStr, attacker->GetItem() );
          flowWk->HANDEX_Pop( param );
        }
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 * とくせい「のろわれボディ」
 *
 * ３０％で受けた技をかなしばり状態にする。
 */
//------------------------------------------------------------------------------
// ダメージ反応ハンドラ
void handler_NorowareBody( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == false)
  &&  (!flowWk->Hnd_CheckShowDown())
  ){
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetPokeID );

    if( !target->CheckSick(pml::wazadata::WAZASICK_KANASIBARI) )
    {
      WazaID  prevWaza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_ORG_WAZAID ));
      if( (prevWaza != WAZANO_NULL)
      &&  (prevWaza != WAZANO_WARUAGAKI)
      &&  (!tables::IsDelayAttackWaza(prevWaza))
      ){
        if( isExePer(flowWk, 30) )
        {
          BTL_HANDEX_PARAM_ADD_SICK* param;

          // 現ターンに行動済みのポケモンには効果ターン数を+1する
          u8 turns = BTL_KANASIBARI_EFFECTIVE_TURN;
          if( target->TURNFLAG_Get(BTL_POKEPARAM::TURNFLG_ACTION_DONE) ){
            ++turns;
          }

          param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
            param->sickID = pml::wazadata::WAZASICK_KANASIBARI;
            param->sickCont = SICKCONT_MakeTurnParam( pokeID, turns, prevWaza );
            param->pokeID = targetPokeID;
            param->header.tokwin_flag = true;
          flowWk->HANDEX_Pop( param );
        }
      }
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_NorowareBody( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,    handler_NorowareBody },  // ダメージ反応ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「くだけるよろい」
 *
 * 物理攻撃を受けると防御が１段階下がり、素早さが２段階上がる。
 */
//------------------------------------------------------------------------------
void handler_KudakeruYoroi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == pml::wazadata::DMG_PHYSIC)
  &&  (EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == false)
  ){
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    bool fEffective = false;

    if( bpp->IsRankEffectValid( BTL_POKEPARAM::BPP_DEFENCE_RANK, -1)
    ||  bpp->IsRankEffectValid( BTL_POKEPARAM::BPP_AGILITY_RANK,  1)
    ){
      fEffective = true;
    }

    if( fEffective && (!bpp->IsDead()) )
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* param;

      flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );

        param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
          param->poke_cnt = 1;
          param->pokeID[0] = pokeID;
          param->fAlmost = true;
          param->rankType = pml::wazadata::RANKEFF_DEFENCE;
          param->rankVolume = -1;
        flowWk->HANDEX_Pop( param );

        param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
          param->poke_cnt = 1;
          param->pokeID[0] = pokeID;
          param->fAlmost = true;
          param->rankType = pml::wazadata::RANKEFF_AGILITY;
          param->rankVolume = 2;
        flowWk->HANDEX_Pop( param );

      flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_KudakeruYoroi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,    handler_KudakeruYoroi },  // ダメージ反応ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ちからずく」
 *
 * 追加効果のある技の追加効果を出すことができないが、かわりに威力が1.3倍になる。
 * 追加効果が自分自身の能力を下げるワザの場合には、このとくせい自体が無効
 */
//------------------------------------------------------------------------------
// 威力計算ハンドラ
void handler_Tikarazuku_WazaPow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    WazaID  waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    if( IsTikarazukuEffecive(waza) )
    {
      EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.3) );
    }
  }
}
// 追加効果チェックハンドラ
void handler_Tikarazuku_CheckFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( pokeID == EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) )
  {
    WazaID  waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    if( IsTikarazukuEffecive(waza) )
    {
      BTL_Printf("ちからずくなので効果が発生しません\n");
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
    }
  }
}
// ひるみ追加チェックハンドラ
void handler_Tikarazuku_ShrinkCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
  }
}
//
void handler_Tikarazuku_HitChk( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    WazaID waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    if( IsTikarazukuEffecive(waza) ){
      EVENTVAR_RewriteValue( BTL_EVAR_TIKARAZUKU_FLG, true );
    }
  }
}

const BtlEventHandlerTable*  HAND_TOK_ADD_Tikarazuku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,      handler_Tikarazuku_WazaPow   },    // ワザ威力計算ハンドラ
    { BTL_EVENT_ADD_SICK,        handler_Tikarazuku_CheckFail },    // 追加効果（状態異常）チェックハンドラ
    { BTL_EVENT_ADD_RANK_TARGET, handler_Tikarazuku_CheckFail },    // 追加効果（ランク効果）チェックハンドラ
    { BTL_EVENT_WAZA_SHRINK_PER, handler_Tikarazuku_ShrinkCheck },  // 追加効果（ひるみ）チェックハンドラ
    { BTL_EVENT_DAMAGEPROC_END_HIT_PREV,  handler_Tikarazuku_HitChk },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

/**
 *  「ちからずく」が効くワザかどうか判定
 */
bool IsTikarazukuEffecive( WazaID waza )
{
  WazaCategory category = WAZADATA_GetCategory( waza );

  // ひるみ確率のあるワザは
  if( WAZADATA_GetParam(waza, pml::wazadata::PARAM_ID_SHRINK_PER) != 0 ){
    return true;
  }

  switch( category ){
  // 殴った相手に効果が発動するタイプは基本的に有効
  case pml::wazadata::CATEGORY_DAMAGE_SICK:
    {
      // バインド系のみ無効
      WazaSick sickID = static_cast<WazaSick>(WAZADATA_GetParam( waza, pml::wazadata::PARAM_ID_SICK ));
      if( sickID == pml::wazadata::WAZASICK_BIND ){
        return false;
      }
    }
    /* fallthru */
  case pml::wazadata::CATEGORY_DAMAGE_EFFECT:
    return true;

  // 自分に効果が発動するタイプは、ランクダウンの効果があるワザのみ無効
  case pml::wazadata::CATEGORY_DAMAGE_EFFECT_USER:
    {
      u32 numEffects = WAZADATA_GetRankEffectCount( waza );
      int value;
      u32 i;
      for(i=0; i<numEffects; ++i)
      {
        WAZADATA_GetRankEffect( waza, i, &value );
        if( value < 0 ){
          return false;
        }
      }
      return true;
    }
//    break;
  }

  // 上記カテゴリ以外でも特別に対象とするワザのチェック（ひみつのちから等）
  if( tables::IsTikarazukuEffeciveWaza(waza) ){
    return true;
  }

  return false;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「まけんき」
 *
 * 相手から能力を下げられる技や特性を受けた時、攻撃力が２段階上がる。
 */
//------------------------------------------------------------------------------
void handler_Makenki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    u8 atkPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    if( !flowWk->Hnd_GetMainModule()->IsFriendPokeID(pokeID, atkPokeID) )
    {
      int volume = EVENTVAR_GetValue( BTL_EVAR_VOLUME );
      if( volume < 0 )
      {
        BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));

          param->header.tokwin_flag = true;
          param->rankType = pml::wazadata::RANKEFF_ATTACK;
          param->rankVolume = 2;
          param->fAlmost = true;
          param->poke_cnt = 1;
          param->pokeID[0] = pokeID;

        flowWk->HANDEX_Pop( param );
      }
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Makenki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RANKEFF_FIXED,    handler_Makenki },  // ランク効果確定ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「かちき」
 *
 * 相手から能力を下げられる技や特性を受けた時、特攻が２段階上がる。
 */
//------------------------------------------------------------------------------
void handler_Katiki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    u8 atkPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    BTL_PRINT("ATKPOKEID=%d, myPokeID=%d\n", atkPokeID, pokeID );
    if( !flowWk->Hnd_GetMainModule()->IsFriendPokeID(pokeID, atkPokeID) )
    {
      int volume = EVENTVAR_GetValue( BTL_EVAR_VOLUME );
      if( volume < 0 )
      {
        BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));

          param->header.tokwin_flag = true;
          param->rankType = pml::wazadata::RANKEFF_SP_ATTACK;
          param->rankVolume = 2;
          param->fAlmost = true;
          param->poke_cnt = 1;
          param->pokeID[0] = pokeID;

        flowWk->HANDEX_Pop( param );
      }
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Katiki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RANKEFF_FIXED,    handler_Katiki },  // ランク効果確定ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  とくせい「よわき」
 *
 * ＨＰが半分以下になると、攻撃、特攻が半減する。
 */
//------------------------------------------------------------------------------
// 攻撃側能力の計算ハンドラ
void handler_Yowaki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u32 hp = bpp->GetValue( BTL_POKEPARAM::BPP_HP );
    u32 hp_half = calc::QuotMaxHP_Zero( bpp, 2 );
    if( hp <= hp_half )
    {
      EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5f) );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Yowaki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER,    handler_Yowaki },  // 攻撃側能力の計算ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「マルチスケイル」
 *
 * ＨＰが満タンの時に受けるダメージを半減させる。
 */
//------------------------------------------------------------------------------
// ダメージ計算最終チェックハンドラ
void handler_MultiScale( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // HP満タンならダメージ半減
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->IsHPFull() )
    {
      EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5f) );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_MultiScale( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3,    handler_MultiScale },  // ダメージ計算最終チェックハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「フレンドガード」
 *  （旧名 なかまいしき）
 *
 * 自分以外の見方へのダメージを1/2にする。
 */
//------------------------------------------------------------------------------
// ダメージ計算最終チェックハンドラ
void handler_NakamaIsiki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  u8 defPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );

  // 自分以外の味方へのダメージ半減
  if( (defPokeID != pokeID)
  &&  flowWk->Hnd_GetMainModule()->IsFriendPokeID(pokeID, defPokeID)
  ){
    EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.75f) );
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_FriendGuard( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3,    handler_NakamaIsiki },  // ダメージ計算最終チェックハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「いやしのこころ」
 *
 * 自分以外の味方のポケモン系状態異常が毎ターン30％の確率で回復する。
 */
//------------------------------------------------------------------------------
// ターンチェックハンドラ
void handler_IyasiNoKokoro( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BtlPokePos  myPos = flowWk->Hnd_PokeIDtoPokePos( pokeID );
    BtlExPos    exPos = EXPOS_MAKE( BTL_EXPOS_AREA_FRIENDS, myPos );

    const BTL_POKEPARAM* bpp;
    u8 targetID[ BTL_POSIDX_MAX ];
    u8 cnt, i;

    cnt = flowWk->Hnd_ExpandPokeID( exPos, targetID );
    for(i=0; i<cnt; ++i)
    {
      if( targetID[ i ] == pokeID ){ continue; }
      bpp = flowWk->Hnd_GetPokeParam( targetID[i] );
      if( bpp->GetPokeSick() != pml::pokepara::SICK_NULL )
      {
        if( isExePer(flowWk, 30) )
        {
          BTL_HANDEX_PARAM_CURE_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
            param->header.tokwin_flag = true;
            param->header.autoRemoveFlag = true;
            param->poke_cnt = 1;
            param->pokeID[0] = targetID[i];
            param->sickCode = WAZASICK_EX_POKEFULL;
          flowWk->HANDEX_Pop( param );
        }
      }
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_IyasiNoKokoro( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_BEGIN,    handler_IyasiNoKokoro },  // ターンチェックハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「どくぼうそう」
 *
 * 毒状態の時、物理攻撃の威力が倍になる。
 */
//------------------------------------------------------------------------------
// 威力計算ハンドラ
void handler_Dokubousou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 自分が「どく」なら物理攻撃の威力が倍
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->CheckSick( pml::wazadata::WAZASICK_DOKU )
    &&  (EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == pml::wazadata::DMG_PHYSIC)
    ){
      EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.5) );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Dokubousou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,      handler_Dokubousou   }, // ワザ威力計算ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ねつぼうそう」
 *
 * やけど状態の時、特殊攻撃の威力が倍になる。
 */
//------------------------------------------------------------------------------
// 威力計算ハンドラ
void handler_Netubousou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 自分が「やけど」なら特殊攻撃の威力が倍
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->CheckSick( pml::wazadata::WAZASICK_YAKEDO )
    &&  (EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == pml::wazadata::DMG_SPECIAL)
    ){
      EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.5) );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Netubousou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,      handler_Netubousou   }, // ワザ威力計算ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「テレパシー」
 *
 *  味方の攻撃を受けない。
 */
//------------------------------------------------------------------------------
// 無効化チェックLv2ハンドラ
void handler_AunNoIki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 自分が防御側で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // 攻撃側が味方ポケモンで
    u8 atkPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    if( flowWk->Hnd_GetMainModule()->IsFriendPokeID(pokeID, atkPokeID) && (pokeID != atkPokeID ) )
    {
      // ダメージワザなら無効化
      WazaID waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
      if( WAZADATA_IsDamage(waza) )
      {
        if( EVENTVAR_RewriteValue(BTL_EVAR_NOEFFECT_FLAG, true) )
        {
          BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
            param->header.tokwin_flag = true;
            HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_AunNoIki );
            HANDEX_STR_AddArg( &param->str, pokeID );
          flowWk->HANDEX_Pop( param );
        }
      }
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Telepassy( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_TOKUSEI,        handler_AunNoIki }, // 無効化チェックLv2ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ムラっけ」
 *
 * 毎ターンエンドにどれかひとつの能力が一段階上昇し、ひとつの能力が一段階下降する。
 */
//------------------------------------------------------------------------------
// ターンチェック最終ハンドラ
void handler_Murakke( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* sys_work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(sys_work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    enum {
      PATTERN_MAX = ((pml::wazadata::RANKEFF_NUMS * pml::wazadata::RANKEFF_NUMS) - pml::wazadata::RANKEFF_NUMS) * 2,
    };

    // 重複しない「上昇・下降」の組み合わせを列挙するための計算用ワーク
    // ※必ず、異なる２つの能力ランクが「上昇」「下降」するようにする。
    // （全能力ランクが上がりきっている（下がりきっている）場合は除く）
    typedef struct {

      u16 patterns[ PATTERN_MAX ];
      u8  upEffects[ pml::wazadata::RANKEFF_NUMS ];
      u8  downEffects[ pml::wazadata::RANKEFF_NUMS ];

    }CALC_WORK;


    CALC_WORK* work = reinterpret_cast<CALC_WORK*>(flowWk->Hnd_GetTmpWork( sizeof(CALC_WORK) ));
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u8 upEffect, downEffect, upCnt, downCnt, ptnCnt;
    u32 i;

  //  BTL_Printf("pokeID=%d\n, bpp=%p\n", pokeID, bpp);

    // 上がりうる能力、下がりうる能力を列挙、カウントする
    upCnt = downCnt = 0;
    for(i=0; i<pml::wazadata::RANKEFF_NUMS; ++i)
    {
      if( bpp->IsRankEffectValid( static_cast<BTL_POKEPARAM::ValueID>(pml::wazadata::RANKEFF_ORIGIN+i), 1) ){
        work->upEffects[ upCnt++ ] = pml::wazadata::RANKEFF_ORIGIN+i;
      }
      if( bpp->IsRankEffectValid( static_cast<BTL_POKEPARAM::ValueID>(pml::wazadata::RANKEFF_ORIGIN+i), -1) ){
        work->downEffects[ downCnt++ ] = pml::wazadata::RANKEFF_ORIGIN+i;
      }
    }

    // 上がりうる（下がりうる）能力が０の場合、ランダムで１つだけ上がる（下がる）
    upEffect = downEffect = pml::wazadata::RANKEFF_NULL;
    if( (upCnt == 0) && (downCnt != 0) ){
      i = calc::GetRand( downCnt );
      downEffect = work->downEffects[ i ];
    }
    else if( (upCnt != 0) && (downCnt == 0) ){
      i = calc::GetRand( upCnt );
      upEffect = work->upEffects[ i ];
    }
    // 上がりうる能力、下がりうる能力が共に１以上の場合、考えられる組み合わせを列挙、カウントする
    else if( (upCnt != 0) && (downCnt != 0) )
    {
      u32 j;
      ptnCnt = 0;
      for(i=0; i<upCnt; ++i)
      {
        for(j=0; j<downCnt; ++j)
        {
          if( work->upEffects[i] != work->downEffects[j] )
          {
            work->patterns[ ptnCnt++ ] = (work->upEffects[i] << 8) | (work->downEffects[j]);
            if( ptnCnt >= GFL_NELEMS(work->patterns) ){
              j = downCnt;  // for loop out
              break;
            }
          }
        }
      }
      // 組み合わせの中から１つをランダム決定
      i = calc::GetRand( ptnCnt );
      upEffect = (work->patterns[i] >> 8) & 0xff;
      downEffect = work->patterns[i] & 0xff;
    }

    flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* param;

      if( upEffect != BTL_POKEPARAM::BPP_VALUE_NULL )
      {
        param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
          param->poke_cnt = 1;
          param->pokeID[0] = pokeID;
          param->rankType = static_cast<WazaRankEffect>(upEffect);
          param->rankVolume = 2;
        flowWk->HANDEX_Pop( param );
      }
      if( downEffect != BTL_POKEPARAM::BPP_VALUE_NULL )
      {
        param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
          param->poke_cnt = 1;
          param->pokeID[0] = pokeID;
          param->rankType = static_cast<WazaRankEffect>(downEffect);
          param->rankVolume = -1;
        flowWk->HANDEX_Pop( param );
      }
    }
    flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );

  } // if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Murakke( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_END,        handler_Murakke },  // ターンチェック最終ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ぼうじん」
 *
 * 天候によるダメージを受けない。
 */
//------------------------------------------------------------------------------
// 天候ダメージ計算ハンドラ
void handler_Boujin_CalcDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);

  HandCommon_Boujin_WeatherReaction( pokeID );
}
// ワザ無効化チェックハンドラ
void handler_Boujin_WazaNoEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    if( HandCommon_Boujin_WazaNoEffect(pokeID) )
    {
      flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );
      {
        BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
          HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_NoEffect );
          HANDEX_STR_AddArg( &param->str, pokeID );
        flowWk->HANDEX_Pop( param );
      }
      flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
    }
  }
}

const BtlEventHandlerTable*  HAND_TOK_ADD_Boujin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WEATHER_REACTION,    handler_Boujin_CalcDamage   }, // 天候ダメージ計算ハンドラ
    { BTL_EVENT_NOEFFECT_CHECK_TOKUSEI,   handler_Boujin_WazaNoEffect }, // ワザ無効化チェックハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「どくしゅ」
 *
 * 打撃技を相手に当てると３割の確率で相手を毒状態にする。
 */
//------------------------------------------------------------------------------
// ダメージ反応ハンドラ
void handler_Dokusyu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 自分が攻撃側で
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == false)
  &&  (EVENTVAR_GetValue(BTL_EVAR_RINPUNGUARD_FLG) == false)
  ){
    // 接触ワザなら
    if( EVENTVAR_GetValue( BTL_EVAR_WAZA_TOUCH_FLAG ) )
    {
      // ３割の確率で相手をどくにする
      if( isExePer(flowWk, 30) )
      {
        BTL_HANDEX_PARAM_ADD_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
          param->header.tokwin_flag = true;
          param->pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
          param->sickID = pml::wazadata::WAZASICK_DOKU;
          param->sickCont = calc::MakeDefaultPokeSickCont( pml::pokepara::SICK_DOKU, pokeID );
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Dokusyu );
          HANDEX_STR_AddArg( &param->exStr, param->pokeID );
        flowWk->HANDEX_Pop( param );
      }
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Dokusyu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,        handler_Dokusyu }, // ダメージ反応ハンドラ
  };
  *numElems = numHandlersWithHandlerPri( BTL_EVPRI_TOKUSEI_dokusyu, GFL_NELEMS(HandlerTable) );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「さいせいりょく」
 *
 * 控えに下げるとＨＰが1/3回復する。
 */
//------------------------------------------------------------------------------
// メンバー退場確定ハンドラ
void handler_SaiseiRyoku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( !bpp->IsDead() && !bpp->IsHPFull() )
    {
      BTL_HANDEX_PARAM_SHIFT_HP* param;
      u32 recoverHP = calc::QuotMaxHP( bpp, 3 );
      u32 recoverLimit = bpp->GetValue( BTL_POKEPARAM::BPP_MAX_HP) - bpp->GetValue( BTL_POKEPARAM::BPP_HP);
      if( recoverHP > recoverLimit ){
        recoverHP = recoverLimit;
      }

      param = reinterpret_cast<BTL_HANDEX_PARAM_SHIFT_HP *>( flowWk->HANDEX_Push( BTL_HANDEX_SHIFT_HP, pokeID ));
        param->pokeID[0] = pokeID;
        param->volume[0] = recoverHP;
        param->poke_cnt = 1;
        param->fEffectDisable = true;
      flowWk->HANDEX_Pop( param );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_SaiseiRyoku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_OUT_FIXED, handler_SaiseiRyoku },   // メンバー退場確定ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「はとむね」
 *
 * 相手に防御力を下げられない。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Hatomune( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RANKEFF_LAST_CHECK, handler_Hatomune_Check }, // ランクダウン成功チェック
    { BTL_EVENT_RANKEFF_FAILED,     handler_Hatomune_Guard },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// ランクダウン直前処理のハンドラ
void handler_Hatomune_Check( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  common_RankDownGuard_Check( flowWk, pokeID, work, pml::wazadata::RANKEFF_DEFENCE );
}
void handler_Hatomune_Guard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  common_RankDownGuard_Fixed( flowWk, pokeID, pokeID, work, BTL_STRID_SET_RankdownFail_DEF );
}
//------------------------------------------------------------------------------
/**
 *  とくせい「すなかき」
 *
 * 天候がすなあらし状態の時、素早さ２倍になる＆すなあらしの天候ダメージを受けない
 *
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Sunakaki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,     handler_Sunakaki           },    /// すばやさ計算ハンドラ
    { BTL_EVENT_WEATHER_REACTION, handler_Sunagakure_Weather }, // 天候リアクションハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// すばやさ計算ハンドラ
void handler_Sunakaki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // 天候がすなあらしなら素早さ２倍
    if( flowWk->Hnd_GetWeather() == BTL_WEATHER_SAND ){
      EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ミラクルスキン」
 *
 * 非ダメージ技に対する回避率が高い。50％回避。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_MilacreSkin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO, handler_MilacreSkin   },    /// ワザ命中率取得ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
/// ワザ命中率取得ハンドラ
void handler_MilacreSkin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 自分が防御で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // 非ダメージワザなら命中率50%
    if( EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == pml::wazadata::DMG_NONE )
    {
      u32 per = EVENTVAR_GetValue( BTL_EVAR_HIT_PER );
      if( per > 50 ){
        EVENTVAR_RewriteValue( BTL_EVAR_HIT_PER, 50 );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  とくせい「アナライズ」
 *
 * ターン中、一番最後に技を出すと技の威力が1.3倍になる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Analyze( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Sinuti   },    // ワザ威力計算ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ワザ威力計算ハンドラ
void handler_Sinuti( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 自分が攻撃側で
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (!tables::IsDelayAttackWaza(static_cast<WazaNo>(EVENTVAR_GetValue(BTL_EVAR_WAZAID))))
  ){
    // 順番最後なら威力増加
    if( HandCommon_IsPokeOrderLast(flowWk, pokeID) )
    {
        EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.3) );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  とくせい「すなのちから」
 *
 * 天候がすなあらしの時、地面、岩、鋼ワザの威力が1.3倍になる
 * ＆すなあらしの天候ダメージを受けない
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_SunanoTikara( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,       handler_SunanoTikara       }, // ワザ威力計算ハンドラ
    { BTL_EVENT_WEATHER_REACTION, handler_Sunagakure_Weather }, // 天候リアクションハンドラ

  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ワザ威力計算ハンドラ
void handler_SunanoTikara( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 自分が攻撃側で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 天候すなあらしなら威力増加
    if( flowWk->Hnd_GetWeather() == BTL_WEATHER_SAND )
    {
      pml::PokeType type = EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
      if( (type == POKETYPE_JIMEN)
      ||  (type == POKETYPE_IWA)
      ||  (type == POKETYPE_HAGANE)
      ){
        EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.3) );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ダルマモード」
 *
 * 【ヒヒダルマの場合のみ有効】
 *  ターンチェック終了時、HPが半分以下ならフォルム１に、HPが半分超ならデフォルトフォルムに変化する
 */
//------------------------------------------------------------------------------
// ターンチェック終了ハンドラ
void handler_DarumaMode( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

  if( bpp->GetMonsNo() == MONSNO_HIHIDARUMA )
  {
    u32 hp, hp_border;
    u8  form_next;

    hp = bpp->GetValue( BTL_POKEPARAM::BPP_HP );
    hp_border = calc::QuotMaxHP_Zero( bpp, 2 );

    form_next = (hp <= hp_border)? FORMNO_HIHIDARUMA_MEDITATION : FORMNO_HIHIDARUMA_ACTION;
    if( bpp->GetFormNo() != form_next )
    {
      BTL_HANDEX_PARAM_CHANGE_FORM* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_FORM *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_FORM, pokeID ));
        param->header.tokwin_flag = true;
        param->pokeID = pokeID;
        param->formNo = form_next;
        {
          u16 strID = (form_next == FORMNO_HIHIDARUMA_MEDITATION)? BTL_STRID_STD_DarumaOn : BTL_STRID_STD_DarumaOff;
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, strID );
        }
      flowWk->HANDEX_Pop( param );
    }
  }
}
// いえき確定
void handler_DarumaMode_IekiFixed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->GetMonsNo() == MONSNO_HIHIDARUMA )
    {
      BTL_HANDEX_PARAM_CHANGE_FORM* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_FORM *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_FORM, pokeID ));
        param->pokeID = pokeID;
        param->formNo = FORMNO_HIHIDARUMA_ACTION;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, BTL_STRID_STD_DarumaOff );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
      flowWk->HANDEX_Pop( param );
    }
  }
}


const BtlEventHandlerTable*  HAND_TOK_ADD_DarumaMode( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_DONE,           handler_DarumaMode            },    // ターンチェック終了ハンドラ
    { BTL_EVENT_IEKI_FIXED,               handler_DarumaMode_IekiFixed  },
    { BTL_EVENT_CHANGE_TOKUSEI_BEFORE,    handler_DarumaMode_IekiFixed  },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}




//------------------------------------------------------------------------------
/**
 *  とくせい「すりぬけ」
 *
 * リフレクター、ひかりのかべ、しんぴのまもり、しろいきりの効果を受けずに攻撃できる。
 * 相手の「みがわり」を貫通してワザ効果を与えることが出来る
 */
//------------------------------------------------------------------------------
// 「すりぬけ」スキップチェックハンドラ
bool handler_Surinuke_SkipCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, BtlEventFactorType factorType, BtlEventType eventType, u16 subID, u8 dependID )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(eventType);
  if( factorType == BTL_EVENT_FACTOR_SIDE )
  {
    u8 myPokeID = EVENT_FACTOR_GetPokeID( myHandle );
    BtlSide mySide = flowWk->Hnd_GetMainModule()->PokeIDtoSide( myPokeID );

    if( mySide != dependID )
    {
      if( (subID == BTL_SIDEEFF_REFRECTOR)        // リフレクター無効
      ||  (subID == BTL_SIDEEFF_HIKARINOKABE)     // ひかりのかべ無効
      ||  (subID == BTL_SIDEEFF_SINPINOMAMORI)    // しんぴのまもり無効
      ||  (subID == BTL_SIDEEFF_SIROIKIRI)        // しろいきり無効
      ||  (subID == BTL_SIDEEFF_AURORAVEIL)       // オーロラベール無効
      ){
        return true;
      }
    }
  }
  return false;
}
// ワザ処理開始
void handler_Surinuke_Start( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 自分が攻撃側ならスキップチェックハンドラをアタッチする
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    EVENT_FACTOR_AttachSkipCheckHandler( myHandle, handler_Surinuke_SkipCheck );
  }
}
// ワザ処理終了
void handler_Surinuke_End( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 自分が攻撃側ならスキップチェックハンドラをデタッチする
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    EVENT_FACTOR_DettachSkipCheckHandler( myHandle );
  }
}
// みがわり貫通チェック
void handler_Surinuke_MigawariThrew( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);

  // 自分が攻撃側なら貫通フラグを true にする
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
  }
}

const BtlEventHandlerTable*  HAND_TOK_ADD_Surinuke( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASEQ_START,      handler_Surinuke_Start },  // ワザ処理開始
    { BTL_EVENT_WAZASEQ_END,        handler_Surinuke_End   },  // ワザ処理終了
    { BTL_EVENT_MIGAWARI_THREW,     handler_Surinuke_MigawariThrew },  // みがわり貫通チェック
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「じしんかじょう」
 *
 * 相手を倒すと攻撃力が１段階上がる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_JisinKajou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL, handler_JisinKajou   },    // ワザダメージプロセス最終ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ワザダメージプロセス最終ハンドラ
void handler_JisinKajou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  // 決着がついていたら何もしない
  if( flowWk->Hnd_CheckShowDown() ){
    return;
  }

  u32 killCount = HandCommon_GetKillCount( myHandle, flowWk, pokeID, work );
  if( killCount )
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
      param->header.tokwin_flag = true;
      param->poke_cnt   = 1;
      param->pokeID[0]  = pokeID;
      param->rankType   = pml::wazadata::RANKEFF_ATTACK;
      param->rankVolume = killCount;
    flowWk->HANDEX_Pop( param );
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「ウルトラフォース」
 *
 * 相手を倒すと最も強いパラメータが１段階上がる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_UltraForce( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL, handler_UltraForce   },    // ワザダメージプロセス最終ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ワザダメージプロセス最終ハンドラ
void handler_UltraForce( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  // 決着がついていたら何もしない
  if( flowWk->Hnd_CheckShowDown() ){
    return;
  }

  u32 killCount = HandCommon_GetKillCount( myHandle, flowWk, pokeID, work );
  if( killCount )
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
      param->header.tokwin_flag = true;
      param->poke_cnt   = 1;
      param->pokeID[0]  = pokeID;
      param->rankType   = ultraForce_GetEffectRankType( flowWk, pokeID );
      param->rankVolume = killCount;
    flowWk->HANDEX_Pop( param );
  }
}

/**
 * @brief 特性「ウルトラフォース」で上昇するパラメータを取得する
 * @param pokeID  特性が「ウルトラフォース」のポケモンのID
 */
WazaRankEffect ultraForce_GetEffectRankType( ServerFlow* flowWk, u8 pokeID )
{
  static const struct 
  {
    WazaRankEffect         rankType;
    BTL_POKEPARAM::ValueID pokeValueID;
  }
  RANK_VALUE_TABLE[] = 
  {
    { pml::wazadata::RANKEFF_ATTACK,      BTL_POKEPARAM::BPP_ATTACK     },
    { pml::wazadata::RANKEFF_DEFENCE,     BTL_POKEPARAM::BPP_DEFENCE    },
    { pml::wazadata::RANKEFF_SP_ATTACK,   BTL_POKEPARAM::BPP_SP_ATTACK  },
    { pml::wazadata::RANKEFF_SP_DEFENCE,  BTL_POKEPARAM::BPP_SP_DEFENCE },
    { pml::wazadata::RANKEFF_AGILITY,     BTL_POKEPARAM::BPP_AGILITY    },
  };
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );


  WazaRankEffect result = pml::wazadata::RANKEFF_ATTACK;
  u32 maxValue = 0;

  for( u32 i=0; i<GFL_NELEMS(RANK_VALUE_TABLE); ++i )
  {
    const u32 value = bpp->GetValue_Base( RANK_VALUE_TABLE[i].pokeValueID );

    if( maxValue < value )
    {
      maxValue = value;
      result = RANK_VALUE_TABLE[i].rankType;
    }
  }

  return result;
}

//------------------------------------------------------------------------------
/**
 *  とくせい「せいぎのこころ」
 *
 * 悪タイプのダメージ技を受けると攻撃力が１段階上がる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_SeiginoKokoro( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION, handler_SeiginoKokoro   },    // ダメージ反応ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ダメージ反応ハンドラ
void handler_SeiginoKokoro( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 自分が防御側で
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == false)
  ){
    // 受けたワザタイプが悪なら攻撃ランクアップ
    if( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_AKU )
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
        param->header.tokwin_flag = true;
        param->poke_cnt   = 1;
        param->pokeID[0]  = pokeID;
        param->rankType   = pml::wazadata::RANKEFF_ATTACK;
        param->rankVolume = 1;
      flowWk->HANDEX_Pop( param );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  とくせい「びびり」
 *
 * ゴースト、悪、虫タイプの技を受けると素早さが１段階上がる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Bibiri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION, handler_Bibiri   },    // ダメージ反応ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ダメージ反応ハンドラ
void handler_Bibiri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 自分が防御側で
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  &&  (EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == false)
  ){
    // 受けたワザタイプが悪・虫・ゴーストなら素早さランクアップ
    pml::PokeType  type = EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
    if( (type == POKETYPE_AKU)
    ||  (type == POKETYPE_MUSHI)
    ||  (type == POKETYPE_GHOST)
    ){
      BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
        param->header.tokwin_flag = true;
        param->poke_cnt   = 1;
        param->pokeID[0]  = pokeID;
        param->rankType   = pml::wazadata::RANKEFF_AGILITY;
        param->rankVolume = 1;
      flowWk->HANDEX_Pop( param );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ミイラ」
 *
 * 打撃技を受けた相手の特性を「ミイラ」にしてしまう。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Miira( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION, handler_Miira   },    // ダメージ反応ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ダメージ反応ハンドラ
void handler_Miira( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 自分が防御側で
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  &&  (EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == false)
  &&  (!flowWk->Hnd_CheckShowDown())
  ){
    // 食らったのが接触ワザで
    if( EVENTVAR_GetValue(BTL_EVAR_WAZA_TOUCH_FLAG) )
    {
      // 相手のとくせいが「ミイラ」以外なら「ミイラ」にする
      u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
      const BTL_POKEPARAM* bppTarget = flowWk->Hnd_GetPokeParam( targetPokeID );
      if( bppTarget->GetValue(BTL_POKEPARAM::BPP_TOKUSEI) != TOKUSEI_MIIRA )
      {
        BTL_HANDEX_PARAM_CHANGE_TOKUSEI* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_TOKUSEI *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_TOKUSEI, pokeID ));
          param->tokuseiID = TOKUSEI_MIIRA;
          param->pokeID = targetPokeID;
          param->cause = TOKUSEI_CHANGE_CAUSE_MIIRA;
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Miira );
          HANDEX_STR_AddArg( &param->exStr, param->pokeID );
          if( !flowWk->Hnd_GetMainModule()->IsFriendPokeID(pokeID, targetPokeID) ){
            param->header.tokwin_flag = true;
          }
        flowWk->HANDEX_Pop( param );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  とくせい「そうしょく」
 *
 * 草タイプの技を受けるとダメージを受けずに特攻が１段階上がる。
 */
//------------------------------------------------------------------------------
// 無効化チェックハンドラ
void handler_Sousyoku_CheckNoEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( common_TypeNoEffect(flowWk, pokeID, POKETYPE_KUSA) )
  {
    common_TypeNoEffect_Rankup( flowWk, pokeID, pml::wazadata::RANKEFF_ATTACK, 1 );
  }
}

const BtlEventHandlerTable*  HAND_TOK_ADD_Sousyoku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_TOKUSEI,     handler_Sousyoku_CheckNoEffect }, // 無効化チェックハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「いたずらごころ」
 *
 * 自分が非ダメージワザを出す際、ワザ先制プライオリティを+1できる。（同ワザ同士なら先制できる）
 * 「いたずらごころ」で優先度が高くなった技は、あくタイプには効かない。
 */
//------------------------------------------------------------------------------
// ワザプライオリティ取得ハンドラ
void handler_ItazuraGokoro( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  
  // 自分が攻撃側で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    work[0] = 0;

    // ダメージワザ以外ならプライオリティアップ
    WazaID waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    if( WAZADATA_GetDamageType(waza) == pml::wazadata::DMG_NONE )
    {
      int pri = EVENTVAR_GetValue( BTL_EVAR_WAZA_PRI );
      EVENTVAR_RewriteValue( BTL_EVAR_WAZA_PRI, pri+1 );

      work[0] = 1;
    }
  }
}
// ワザパラメータチェック
void  handler_ItazuraGokoro_WazaParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);

  if( ( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID ) &&
      ( work[0] == 1 ) )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_ITAZURAGOKORO_EFFECT, true );
  }
}

const BtlEventHandlerTable*  HAND_TOK_ADD_ItazuraGokoro( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_GET_WAZA_PRI,   handler_ItazuraGokoro           }, // ワザプライオリティ取得ハンドラ
    { BTL_EVENT_WAZA_PARAM,     handler_ItazuraGokoro_WazaParam }, // ワザパラメータチェック
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「マジックミラー」
 *
 * ワザ“マジックコート”で跳ね返せる技を跳ね返す。
 */
//------------------------------------------------------------------------------
// ワザ出し成否チェックハンドラ
void handler_MagicMirror_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  HandCommon_MagicCoat_CheckSideEffWaza( myHandle, flowWk, pokeID, work );
}
// 無効化チェックハンドラ
void handler_MagicMirror_Wait( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  HandCommon_MagicCoat_Wait( myHandle, flowWk, pokeID, work );
}
// わざ跳ね返し確定ハンドラ
void handler_MagicMirror_Reflect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( EVENTVAR_RewriteValue(BTL_EVAR_GEN_FLAG, true) )
    {
      flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );
      HandCommon_MagicCoat_Reaction( myHandle, flowWk, pokeID, work );
      flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
    }
  }
}

const BtlEventHandlerTable*  HAND_TOK_ADD_MagicMirror( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD,  handler_MagicMirror_ExeCheck  }, // ワザ出し成否チェックハンドラ
    { BTL_EVENT_NOEFFECT_CHECK_TOKUSEI,       handler_MagicMirror_Wait      }, // わざ乗っ取り判定ハンドラ
    { BTL_EVENT_WAZASEQ_REFRECT,         handler_MagicMirror_Reflect   }, // わざ跳ね返し確定
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「しゅうかく」
 *
 * きのみを使ってもターンエンドに復活する。
 */
//------------------------------------------------------------------------------
// ターンチェック終了ハンドラ
void handler_Syuukaku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

    u16 usedItem = bpp->GetConsumedItem();
    if( (usedItem != ITEM_DUMMY_DATA)
    &&  (::item::ITEM_CheckNuts(usedItem))
    &&  (bpp->GetItem() == ITEM_DUMMY_DATA)
    ){
      //@バトル変更箇所
      if( (calc::IsShineWeather(flowWk->Hnd_GetWeather()))
      ||  (isExePer(flowWk, 50))
      ){
        BTL_HANDEX_PARAM_SET_ITEM* param;

        param = reinterpret_cast<BTL_HANDEX_PARAM_SET_ITEM *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_ITEM, pokeID ));
          param->header.tokwin_flag = true;
          param->itemID = usedItem;
          param->pokeID = pokeID;
          param->fClearConsume = true;
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Syuukaku );
          HANDEX_STR_AddArg( &param->exStr, pokeID );
          HANDEX_STR_AddArg( &param->exStr, usedItem );
        flowWk->HANDEX_Pop( param );
      }
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Syuukaku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_END,        handler_Syuukaku },  // ターンチェック終了ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  とくせい「ヘヴィメタル」
 *
 *  体重２倍にして計算
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_HeavyMetal( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WEIGHT_RATIO,        handler_HeavyMetal },  // ランク増減値修正ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
void handler_HeavyMetal( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
  }
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ライトメタル」
 *
 *  体重1/2にして計算
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_LightMetal( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WEIGHT_RATIO,        handler_LightMetal },  // ランク増減値修正ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
void handler_LightMetal( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5) );
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「あまのじゃく」
 *
 *  能力ランク効果が逆転して働く。（ex. "いかく"を受けると攻撃力が上がる）
 */
//------------------------------------------------------------------------------
// ランク増減値修正ハンドラ
void handler_Amanojaku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    int volume = EVENTVAR_GetValue( BTL_EVAR_VOLUME );
    {
      volume *= -1;
      work[WIDX0] = EVENTVAR_RewriteValue( BTL_EVAR_VOLUME, volume );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Amanojaku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RANKVALUE_CHANGE,        handler_Amanojaku },  // ランク増減値修正ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  とくせい「きんちょうかん」
 *
 * 相手が緊張してきのみが食べられない。
 */
//------------------------------------------------------------------------------
// メンバー入場
void handler_Kinchoukan_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  // 「きんちょうかん」持ちであることを告知
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    handler_Kinchoukan_RotationIn( myHandle, flowWk, pokeID, work );
  }
}
// ローテーション
void handler_Kinchoukan_RotationIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WIDX0] == 0 )
  {
    BTL_HANDEX_PARAM_MESSAGE* param;
    BtlSide side = flowWk->Hnd_GetMainModule()->GetOpponentSide( flowWk->Hnd_GetMainModule()->PokeIDtoSide(pokeID) );

    flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );
    param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_Kinchoukan );
      HANDEX_STR_AddArg( &param->str, side );
    flowWk->HANDEX_Pop( param );
    flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );

    EVENT_FACTOR_AttachSkipCheckHandler( myHandle, handler_Kinchoukan_SkipCheck );

    work[WIDX0] = 1;
  }
}
// スキップチェックハンドラ
bool handler_Kinchoukan_SkipCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, BtlEventFactorType factorType, BtlEventType eventType, u16 subID, u8 pokeID )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(eventType);
  if( factorType == BTL_EVENT_FACTOR_ITEM )
  {
    u8 my_pokeID = EVENT_FACTOR_GetPokeID( myHandle );

    if( !flowWk->Hnd_GetMainModule()->IsFriendPokeID(my_pokeID, pokeID) )
    {
      if( ::item::ITEM_CheckNuts(subID) )
      {
        return true;
      }
    }
  }
  return false;
}
// トレースやスキルスワップなどで「きんちょうかん」になった
void handler_Kinchoukan_Got( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  handler_Kinchoukan_RotationIn( myHandle, flowWk, pokeID, work );
}

const BtlEventHandlerTable*  HAND_TOK_ADD_Kinchoukan( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,             handler_Kinchoukan_MemberIn       }, // メンバー入場ハンドラ
    { BTL_EVENT_MEMBER_IN_PREV,        handler_Kinchoukan_RotationIn     },
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER,  handler_Kinchoukan_Got            },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「かわりもの」
 *
 * 場に出た時に正面の相手に変身する。目の前に対象がいない場合はへんしんしない。
 */
//------------------------------------------------------------------------------
// 入場ハンドラ
void handler_Hensin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BtlPokePos myPos = flowWk->Hnd_PokeIDtoPokePos( pokeID );
    BtlRule rule = flowWk->Hnd_GetRule( );
    BtlPokePos tgtPos = MainModule::GetFacedPokePos( rule, myPos );

    u8 tgtPokeID = flowWk->Hnd_PokePosToPokeID( tgtPos );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( tgtPokeID );
    const BTL_POKEPARAM* self = flowWk->Hnd_GetPokeParam( pokeID );

    if( (!self->IsDead()) && (!target->IsDead()) )
    {
      BTL_HANDEX_PARAM_HENSIN* param = reinterpret_cast<BTL_HANDEX_PARAM_HENSIN *>( flowWk->HANDEX_Push( BTL_HANDEX_HENSIN, pokeID ));
        param->header.tokwin_flag = true;
        param->pokeID = tgtPokeID;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Hensin );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
        HANDEX_STR_AddArg( &param->exStr, param->pokeID );
      flowWk->HANDEX_Pop( param );
    }
  }
}
const BtlEventHandlerTable*  HAND_TOK_ADD_Kawarimono( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,              handler_Hensin }, // 入場ハンドラ
//    { BTL_EVENT_CHANGE_TOKUSEI_AFTER,   handler_Hensin }, // とくせい書き換えハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  とくせい「イリュージョン」
 *
 * リストの並び順で後ろのポケモンと同じ姿・名前で戦闘に出せる。ダメージを受けると元に戻る。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Illusion( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_Illusion_Damage    },   // ダメージ反応ハンドラ
    { BTL_EVENT_IEKI_FIXED,            handler_Illusion_Ieki      },   // いえき確定ハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_BEFORE, handler_Illusion_ChangeTok },   // とくせい変換直前
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ダメージ反応ハンドラ
void handler_Illusion_Damage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  // 自分が防御側ならイリュージョン解除
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == false)
  ){
    common_IllusionBreak( myHandle, flowWk, pokeID );
  }
}
// いえき確定ハンドラ
void handler_Illusion_Ieki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    common_IllusionBreak( myHandle, flowWk, pokeID );
  }
}
// とくせい変換直前ハンドラ
void handler_Illusion_ChangeTok( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_TOKUSEI_NEXT) != EVENT_FACTOR_GetSubID(myHandle))
  &&  (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  ){
    common_IllusionBreak( myHandle, flowWk, pokeID );
  }
}

void common_IllusionBreak( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID )
{
  GFL_UNUSED(myHandle);
  // イリュージョン継続中なら正体を明かす
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  if( bpp->IsFakeEnable() )
  {
    BTL_HANDEX_PARAM_FAKE_BREAK* param = reinterpret_cast<BTL_HANDEX_PARAM_FAKE_BREAK *>( flowWk->HANDEX_Push( BTL_HANDEX_FAKE_BREAK, pokeID ));
      param->pokeID = pokeID;
    flowWk->HANDEX_Pop( param );
  }
}
//------------------------------------------------------------------------------
/**
 *  とくせい「しょうりのほし」
 *
 * 自分を含め、味方側の命中率が1.1倍になる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_GoodLuck( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO, handler_GoodLuck   },       /// ワザ命中率取得ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
/// ワザ命中率取得ハンドラ
void handler_GoodLuck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 自分の味方が攻撃する場合 命中率 x1.1
  u8 atkPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
  if( flowWk->Hnd_GetMainModule()->IsFriendPokeID(pokeID, atkPokeID) )
  {
    EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.1) );
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「メンタルベール」
 *
 * 自分と味方のポケモンは、ちょうはつ・アンコールなどメンタルハーブで治る状態異常にならない
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_MentalVeil( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_CHECKFAIL, handler_MentalVeil_Check   },    // 状態異常失敗チェックハンドラ
    { BTL_EVENT_ADDSICK_FAILED,    handler_MentalVeil_Failed  },    // 状態異常失敗確定ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// 状態異常失敗チェックハンドラ
void handler_MentalVeil_Check( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  WazaSick  sickID = (WazaSick)( EVENTVAR_GetValue(BTL_EVAR_SICKID) );

  if( (flowWk->Hnd_GetMainModule()->IsFriendPokeID(pokeID, target_pokeID))
  &&  (tables::IsMentalSickID(sickID))
  ){
    work[WIDX0] = EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
    if( work[WIDX0] ){
      EVENTVAR_RewriteValue( BTL_EVAR_ALMOST_FLAG, true );
    }
  }
}
// 状態異常失敗確定ハンドラ
void handler_MentalVeil_Failed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_FriendSickFailed( myHandle, flowWk, pokeID, work, BTL_STRID_SET_MentalVeil );
}
/**
 *  味方側の状態異常を防いだ時 共通処理
 */
void common_FriendSickFailed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work, u16 strID )
{
  GFL_UNUSED(myHandle);

  u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  if( (flowWk->Hnd_GetMainModule()->IsFriendPokeID(pokeID, target_pokeID))
  &&  (work[WIDX0])
  ){
    BTL_HANDEX_PARAM_MESSAGE* param;

    flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );

    param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, strID );
      HANDEX_STR_AddArg( &param->str, target_pokeID );
    flowWk->HANDEX_Pop( param );

    flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
    work[WIDX0] = 0;
  }
}


//------------------------------------------------------------------------------
/**
 *  とくせい「フラワーベール」
 *
 * 自分を含め、味方側のくさポケモンが能力ランクを下げられない・基本状態異常にならない
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_FlowerVeil( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RANKEFF_LAST_CHECK, handler_FlowerVeil_Check },          // ランクダウン成功チェック
    { BTL_EVENT_RANKEFF_FAILED,     handler_FlowerVeil_Guard },
    { BTL_EVENT_ADDSICK_CHECKFAIL,  handler_FlowerVeil_SickCheck   },    // 状態異常失敗チェックハンドラ
    { BTL_EVENT_ADDSICK_FAILED,     handler_FlowerVeil_SickFailed  },    // 状態異常失敗確定ハンドラ
    { BTL_EVENT_CHECK_INEMURI,      handler_FlowerVeil_CheckInemuri },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ランクダウン直前処理のハンドラ
void handler_FlowerVeil_Check( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  u8 target_pokeID = EVENTVAR_GetValue(BTL_EVAR_POKEID);
  if( common_IsFlowerVeilTarget(flowWk, pokeID, target_pokeID) )
  {
    common_RankDownGuard_Check( flowWk, target_pokeID, work, pml::wazadata::RANKEFF_MAX );
  }
}
// いねむり不可チェック
void handler_FlowerVeil_CheckInemuri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  u8 target_pokeID = EVENTVAR_GetValue(BTL_EVAR_POKEID);
  if( common_IsFlowerVeilTarget(flowWk, pokeID, target_pokeID) )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
  }
}

// ランクダウン失敗確定ハンドラ
void handler_FlowerVeil_Guard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  int wazaSerial = EVENTVAR_GetValue( BTL_EVAR_WAZA_SERIAL );
  if( wazaSerial == WAZASERIAL_NULL || work[WIDX1] != wazaSerial )
  {
    u8 target_pokeID = EVENTVAR_GetValue(BTL_EVAR_POKEID);
    common_RankDownGuard_Fixed( flowWk, target_pokeID, pokeID, work, BTL_STRID_SET_FlowerVeil );
    work[WIDX1] = wazaSerial;
  }
}
// 状態異常失敗チェック
void handler_FlowerVeil_SickCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  if( common_IsFlowerVeilTarget(flowWk, pokeID, target_pokeID) )
  {
    u8 atk_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    if( atk_pokeID != target_pokeID )
    {
      WazaSick sickID = (WazaSick)(EVENTVAR_GetValue(BTL_EVAR_SICKID));
      if( IsPokeSick(sickID) || sickID == pml::wazadata::WAZASICK_AKUBI ){
        work[WIDX0] = EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
      }
    }
  }
}
// 状態異常失敗確定ハンドラ
void handler_FlowerVeil_SickFailed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  int wazaSerial = EVENTVAR_GetValue( BTL_EVAR_WAZA_SERIAL );
  if( wazaSerial == WAZASERIAL_NULL || work[WIDX1] != wazaSerial )
  {
    common_FriendSickFailed( myHandle, flowWk, pokeID, work, BTL_STRID_SET_FlowerVeil );
    work[WIDX1] = wazaSerial;
  }
}
/**
 * フラワーベールで保護する対象ポケモンか判定
 * @param flowWk  ServerFlow
 * @param pokeID  フラワーベール持ちのポケモンID
 * @param targetPokeID   チェックする対象ポケモンID
 * @return 保護対象ならtrue
 */
bool common_IsFlowerVeilTarget( ServerFlow* flowWk, u8 pokeID, u8 targetPokeID )
{
  if( flowWk->Hnd_GetMainModule()->IsFriendPokeID(pokeID, targetPokeID) )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( targetPokeID );
    if( bpp->IsMatchType(POKETYPE_KUSA)){
      return true;
    }
  }
  return false;
}

//------------------------------------------------------------------------------
/**
 *  とくせい「スイートベール」
 *
 * 味方ポケモンは眠らなくなる
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_SweetVeil( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_CHECKFAIL,    handler_SweetVeil_PokeSick       }, // ポケモン系状態異常処理ハンドラ
    { BTL_EVENT_ADDSICK_FAILED,       handler_SweetVeil_PokeSickFailed }, // ポケモン系状態異常失敗ハンドラ
    { BTL_EVENT_CHECK_INEMURI,        handler_SweetVeil_Inemuri        }, // いねむりチェック
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ポケモン系状態異常処理ハンドラ
void handler_SweetVeil_PokeSick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  if( flowWk->Hnd_GetMainModule()->IsFriendPokeID(pokeID, target_pokeID) )
  {
    work[WIDX0] = HandCommon_GuardWazaSick( flowWk, target_pokeID, pml::wazadata::WAZASICK_NEMURI );
    if( work[WIDX0] == 0 ){
      work[WIDX0] = HandCommon_GuardWazaSick( flowWk, target_pokeID, pml::wazadata::WAZASICK_AKUBI );
    }
  }
}
// ポケモン系状態異常処理ハンドラ
void handler_SweetVeil_PokeSickFailed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_FriendSickFailed( myHandle, flowWk, pokeID, work, BTL_STRID_SET_SweetVeil );
}
// いねむりチェック
void handler_SweetVeil_Inemuri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);

  u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( flowWk->Hnd_GetMainModule()->IsFriendPokeID(pokeID, target_pokeID) )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
  }
}



//------------------------------------------------------------------------------
/**
 *  とくせい「ほおぶくろ」
 *
 *  きのみを食べると、きのみの効果に加えてＨＰが1/3回復する
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_HooBukuro( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_AFTER_ITEMEQUIP,     handler_Hoobukuro }, // 道具使用後チェック
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// 道具使用後チェック
void handler_Hoobukuro( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  BTL_PRINT("pokeID=%d, EvPokeID=%d, EvKinomiFlg=%d\n",pokeID,EVENTVAR_GetValue(BTL_EVAR_POKEID),EVENTVAR_GetValue(BTL_EVAR_KINOMI_FLAG) );
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_KINOMI_FLAG))
  ){
    u32 itemID = EVENTVAR_GetValue( BTL_EVAR_ITEM );

    if( ::item::ITEM_CheckNuts(itemID) )
    {
      BTL_HANDEX_PARAM_RECOVER_HP* param;
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

      param = reinterpret_cast<BTL_HANDEX_PARAM_RECOVER_HP *>( flowWk->HANDEX_Push( BTL_HANDEX_RECOVER_HP, pokeID ));
        param->header.tokwin_flag = true;
        param->pokeID = pokeID;
        param->recoverHP = calc::QuotMaxHP( bpp, 3 );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_HP_Recover );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
        HANDEX_STR_AddArg( &param->exStr, EVENT_FACTOR_GetSubID(myHandle) );
      flowWk->HANDEX_Pop( param );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  とくせい「へんげんじざい」
 *
 *  自分のタイプを出したワザのタイプに変化させる
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_HengenZizai( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXE_DECIDE,     handler_HengenZizai }, // ワザ発動開始
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ワザ発動開始
void handler_HengenZizai( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    pml::PokeType  wazaType = EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
    if( wazaType < POKETYPE_MAX )
    {
      BTL_HANDEX_PARAM_CHANGE_TYPE* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_TYPE *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_TYPE, pokeID ));
        param->header.tokwin_flag = true;
        param->next_type = PokeTypePair_MakePure( wazaType );
        param->exTypeCause = BTL_POKEPARAM::EXTYPE_CAUSE_NONE;
        param->pokeID = pokeID;
      flowWk->HANDEX_Pop( param );
    }
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「バトルスイッチ」
 *
 *  自分の出した攻撃ワザが成功したら「ソードフォルム」に変化する。
 *  同じくワザ「はがねのまもり」が成功したら「シールドフォルム」に変化する。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_BattleSwitch( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_CALL_DECIDE,  handler_BattleSwitch }, // ワザ処理開始ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ワザ処理開始ハンドラ
void  handler_BattleSwitch( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

    if( bpp->GetMonsNo() != MONSNO_GIRUGARUDO ){
      return;
    }

    WazaID  waza = (WazaID)(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    u32   cur_formNo = bpp->GetFormNo();
    u32   next_formNo = cur_formNo;

    if( waza == WAZANO_KINGUSIIRUDO )
    {
      next_formNo = FORMNO_GIRUGARUDO_TATE;
    }
    else if( WAZADATA_IsDamage(waza) )
    {
      next_formNo = FORMNO_GIRUGARUDO_HOKO;
    }

    if( cur_formNo != next_formNo )
    {
      u32 strID = (next_formNo == FORMNO_GIRUGARUDO_TATE)? BTL_STRID_STD_BattleSwitch_Shield : BTL_STRID_STD_BattleSwitch_Sord;

      BTL_HANDEX_PARAM_CHANGE_FORM* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_FORM *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_FORM, pokeID ));
        param->header.tokwin_flag = true;
        param->pokeID = pokeID;
        param->formNo = next_formNo;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, strID );
      flowWk->HANDEX_Pop( param );
    }
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「ダークオーラ」
 *
 *  場に出ていたら、悪タイプのワザ威力が1.5倍になる
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_DarkAura( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,            handler_DarkAura_MemberIN }, // メンバー入場ハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_DarkAura_MemberIN }, // とくせい書き換えハンドラ
    { BTL_EVENT_WAZA_POWER,           handler_DarkAura          }, // ワザ威力計算
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// メンバー入場時など
void  handler_DarkAura_MemberIN( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_MemberInTokMessage( flowWk, pokeID, BTL_STRID_SET_DarkAura );
}
// ワザ威力計算
void  handler_DarkAura( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_AKU )
  {
    if( EVENTVAR_RewriteValue(BTL_EVAR_AURA_RESERVE_FLAG, true) )
    {
      fx32 ratio = EVENTVAR_GetValue(BTL_EVAR_AURA_BREAK_FLAG)? FX32_CONST(0.75f) : FX32_CONST(1.33f);
      EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, ratio );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  とくせい「フェアリーオーラ」
 *
 *  場に出ていたら、フェアリータイプのワザ威力が1.5倍になる
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_FairyAura( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,            handler_FairyAura_MemberIN }, // メンバー入場ハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_FairyAura_MemberIN }, // とくせい書き換えハンドラ
    { BTL_EVENT_WAZA_POWER,           handler_FairyAura          }, // ワザ威力計算
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// メンバー入場時など
void  handler_FairyAura_MemberIN( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_MemberInTokMessage( flowWk, pokeID, BTL_STRID_SET_FairyAura );
}
// ワザ威力計算
void  handler_FairyAura( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_FAIRY )
  {
    if( EVENTVAR_RewriteValue(BTL_EVAR_AURA_RESERVE_FLAG, true) )
    {
      fx32 ratio = EVENTVAR_GetValue(BTL_EVAR_AURA_BREAK_FLAG)? FX32_CONST(0.75f) : FX32_CONST(1.33f);
      EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, ratio );
    }
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「オーラブレーク」
 *
 *  ダークオーラ、フェアリーオーラの効果を反転させる
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_AuraBreak( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,            handler_AuraBreak_MemberIn }, // メンバー入場ハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_AuraBreak_MemberIn }, // とくせい書き換えハンドラ
    { BTL_EVENT_WAZA_POWER_BASE,      handler_AuraBreak }, // ワザ威力計算前
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// メンバー入場時など
void  handler_AuraBreak_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_MemberInTokMessage( flowWk, pokeID, BTL_STRID_SET_AuraBreak );
}
void  handler_AuraBreak( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  EVENTVAR_RewriteValue( BTL_EVAR_AURA_BREAK_FLAG, true );
}

//------------------------------------------------------------------------------
/**
 *  とくせい「がんじょうあご」
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_GanjouAgo( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_GanjouAgo }, // ワザ威力計算
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
void  handler_GanjouAgo( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  // 自分が攻撃側で対象ワザなら威力増加
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    WazaID  waza = (WazaID)(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    if( tables::IsAgoBoostWaza(waza) )
    {
      EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.5) );
    }
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「ファーコート」
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_FurCoat( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DEFENDER_GUARD, handler_FurCoat }, // 防御能力決定ハンドラ

  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// 防御能力決定ハンドラ
void  handler_FurCoat( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  // 防御側が自分で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // ダメージタイプが物理の時、防御上昇
    if( EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == pml::wazadata::DMG_PHYSIC )
    {
      EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  とくせい「くさのけがわ」
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_KusaNoKegawa( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DEFENDER_GUARD, handler_KusaNoKegawa }, // 防御能力決定ハンドラ

  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// 防御能力決定ハンドラ
void  handler_KusaNoKegawa( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  // 防御側が自分で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // ダメージタイプが物理で
    if( EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == pml::wazadata::DMG_PHYSIC )
    {
      // グラスフィールドの時、防御上昇
      if( flowWk->Hnd_GetGround() == BTL_GROUND_GRASS )
      {
        EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.5) );
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「ぬめぬめ・カーリーヘアー」
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_NumeNume( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,  handler_NumeNume   },  // ワザダメージ処理後
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ワザダメージ処理後
void  handler_NumeNume( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  // 決着がついていたら何もしない
  if( flowWk->Hnd_CheckShowDown() ){
    return;
  }

  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  &&  ((bool)EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == false)
  &&  ((bool)EVENTVAR_GetValue(BTL_EVAR_WAZA_TOUCH_FLAG) == true)
  ){
    u8 attackerPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( attackerPokeID );

    if( !bpp->IsDead() )
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>(flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID));
        param->header.tokwin_flag = true;
        param->cause      = RANKEFFECT_CAUSE_NUMENUME;
        param->poke_cnt   = 1;
        param->pokeID[0]  = attackerPokeID;
        param->rankType   = pml::wazadata::RANKEFF_AGILITY;;
        param->rankVolume = -1;
        param->bMigawariThrew = true;
        param->fAlmost = true;
      flowWk->HANDEX_Pop( param );
    }
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「かたいつめ」
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_KataiTume( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_KataiTume }, // ワザ威力計算
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ワザ威力計算ハンドラ
void  handler_KataiTume( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  // 自分が攻撃側で接触ワザなら威力増加
  if( ( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID ) &&
      ( EVENTVAR_GetValue(BTL_EVAR_WAZA_TOUCH_FLAG) ) )
  {
    EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.3) );
  }
}

/**
 * ○○スキン共通：タイプ書き換え
 */
void common_Skin_ChangeType( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work,  pml::PokeType type )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);

  work[WIDX0] = 0;

  WazaID wazaID = static_cast<WazaID>( EVENTVAR_GetValue( BTL_EVAR_WAZAID ) );

  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_NORMAL)
  && !( tables::IsZWaza( wazaID ) )
  ){
    work[WIDX0] = EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, type );
  }
}
/**
 * ○○スキン共通：威力計算
 */
void common_Skin_WazaPow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work, pml::PokeType type )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);

  WazaID wazaID = static_cast<WazaID>( EVENTVAR_GetValue( BTL_EVAR_WAZAID ) );

  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == type)
  && !( tables::IsZWaza( wazaID ) )
  &&  work[WIDX0]
  ){
    EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.2f) );
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「スカイスキン」
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_SkySkin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM,     handler_SkySkin       }, // ワザタイプ決定ハンドラ
    { BTL_EVENT_WAZA_POWER,     handler_SkySkin_Pow   }, // 威力計算
    { BTL_EVENT_DAMAGEPROC_END, handler_SkinEndCommon }, // ダメージ計算処理終了ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ワザタイプ決定ハンドラ
void  handler_SkySkin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Skin_ChangeType( myHandle, flowWk, pokeID, work, POKETYPE_HIKOU );
}
// 威力計算
void  handler_SkySkin_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Skin_WazaPow( myHandle, flowWk, pokeID, work, POKETYPE_HIKOU );
}

//------------------------------------------------------------------------------
/**
 *  とくせい「フェアリースキン」
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_FairySkin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM,   handler_FairySkin      }, // ワザタイプ決定ハンドラ
    { BTL_EVENT_WAZA_POWER,   handler_FairySkin_Pow  }, // 威力計算
   { BTL_EVENT_DAMAGEPROC_END, handler_SkinEndCommon }, // ダメージ計算処理終了ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ワザタイプ決定ハンドラ
void  handler_FairySkin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Skin_ChangeType( myHandle, flowWk, pokeID, work, POKETYPE_FAIRY );
}
// 威力計算
void  handler_FairySkin_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Skin_WazaPow( myHandle, flowWk, pokeID, work, POKETYPE_FAIRY );
}

//------------------------------------------------------------------------------
/**
 *  とくせい「フリーズスキン」
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_FreezSkin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM,   handler_FreezSkin      }, // ワザタイプ決定ハンドラ
    { BTL_EVENT_WAZA_POWER,   handler_FreezSkin_Pow  }, // 威力計算
    { BTL_EVENT_DAMAGEPROC_END, handler_SkinEndCommon }, // ダメージ計算処理終了ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ワザタイプ決定ハンドラ
void  handler_FreezSkin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Skin_ChangeType( myHandle, flowWk, pokeID, work, POKETYPE_KOORI );
}
// 威力計算
void  handler_FreezSkin_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Skin_WazaPow( myHandle, flowWk, pokeID, work, POKETYPE_KOORI );
}

// ○○スキン共通：ダメージ計算処理の最後に work[WIDX0] を初期化する
void  handler_SkinEndCommon( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  // 威力上昇効果を複数体に適用することがあるため、ここでクリアする
  work[WIDX0] = 0;
}



//----------------------------------------------------------------------------------
/**
 * メガランチャー
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_MegaLauncher( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,          handler_MegaLauncher_Pow      }, // 威力計算
    { BTL_EVENT_RECOVER_HP_RATIO,    handler_MegaLauncher_Recover  }, // 回復量計算
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 威力計算ハンドラ
void handler_MegaLauncher_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( work );
  GFL_UNUSED( myHandle );

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    WazaID  waza_id = (WazaID)(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    if( (waza_id == WAZANO_MIZUNOHADOU)
    ||  (waza_id == WAZANO_AKUNOHADOU)
    ||  (waza_id == WAZANO_RYUUNOHADOU)
    ||  (waza_id == WAZANO_HADOUDAN)
    //@バトル変更箇所(こんげんのはどう追加)
    ||  (waza_id == WAZANO_KONGENNOHADOU)
    ){
      EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.5f) );
    }
  }
}
// 回復量計算ハンドラ
void handler_MegaLauncher_Recover( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( work );
  GFL_UNUSED( myHandle );

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    WazaID  waza_id = (WazaID)(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    if( waza_id == WAZANO_IYASINOHADOU )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_RATIO, FX32_CONST(0.75) );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  とくせい「はやてのつばさ」
 *
 * 自分がひこうワザを出す際、ワザプライオリティを+1できる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_HayateNoTsubasa( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_GET_WAZA_PRI,   handler_HayateNoTsubasa }, // ワザプライオリティ取得ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ワザプライオリティ取得ハンドラ
void handler_HayateNoTsubasa( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 自分が攻撃側で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

    // HPが満タンで、飛行ワザならプライオリティアップ
    if( ( bpp->IsHPFull() ) &&
        ( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_HIKOU ) )
    {
      int pri = EVENTVAR_GetValue( BTL_EVAR_WAZA_PRI );
      EVENTVAR_RewriteValue( BTL_EVAR_WAZA_PRI, pri+1 );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  とくせい「ぼうだん」
 *
 * 相手のとくていのワザを無効化する
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Boudan( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2, handler_Boudan },    // 無効化チェックレベル２ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// 無効化チェックレベル２ハンドラ
void handler_Boudan( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);

  // 自分が防御側で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // 特定のワザなら無効化
    WazaID  waza = (WazaID)(EVENTVAR_GetValue(BTL_EVAR_WAZAID));
    if( tables::IsBoudanWaza(waza) )
    {
      if( EVENTVAR_RewriteValue(BTL_EVAR_NOEFFECT_FLAG, true) )
      {
        flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );
        {
          BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
            HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_NoEffect );
            HANDEX_STR_AddArg( &param->str, pokeID );
          flowWk->HANDEX_Pop( param );
        }
        flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  とくせい「おやこあい」
 *
 * 単体・単発のダメージワザを２回ダメージのワザとして繰り出す
 *（２回目のダメージ量は半分になる）
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_OyakoAi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_COUNT, handler_OyakoAi },    // 複数回ヒットワザチェックハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// 複数回ヒットワザチェックハンドラ
void handler_OyakoAi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);

  // 自分が攻撃側で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // １回ヒットワザで、遅延攻撃・全力技でないなら発動
    if( (EVENTVAR_GetValue(BTL_EVAR_HITCOUNT_MAX) == 1)
    &&  (!EVENTVAR_GetValue(BTL_EVAR_DELAY_ATTACK_FLAG))
    &&  (EVENTVAR_GetValue(BTL_EVAR_ZENRYOKU_WAZA_KIND) == pml::waza::ZENRYOKUWAZA_KIND_NULL)
    ){
      // がむしゃらなどは強すぎるから除外
      WazaNo  waza = (WazaNo)(EVENTVAR_GetValue(BTL_EVAR_WAZAID));
      if(  tables::IsOyakoaiOmitWaza(waza) ){
        return;
      }
      // ためワザは除外（挙動が不自然）
      if( WAZADATA_GetFlag(waza, pml::wazadata::WAZAFLAG_Tame) ){
        return;
      }
      EVENTVAR_RewriteValue( BTL_EVAR_HITCOUNT, 2 );
    }
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「マジシャン」
 *
 * ダメージを与えた相手の道具をうばう（相手優先、素早さ順）
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Magician( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,        handler_magician_Start }, // ダメージプロセス開始ハンドラ
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL, handler_magician       }, // ダメージ処理後（みがわり除く）
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージプロセス開始ハンドラ
void handler_magician_Start( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  HandCommon_DorobouStart( flowWk, pokeID, work );
}
// ダメージ処理後
void handler_magician( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID ){
    return;
  }
  if( !HandCommon_IsDorobouEnable(flowWk, pokeID, work) ){
    return;
  }
  if( EVENTVAR_GetValue(BTL_EVAR_DELAY_ATTACK_FLAG) ){
    return;
  }

  // 対象ポケモンを素早さ順ソート
  u32 targetCnt = EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
  u8* targetID = (u8*)work;

  for(u32 i=0; i<targetCnt; ++i){
    targetID[i] = EVENTVAR_GetValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1 + i) );
  }
  HandCommon_SortTargetByAgility( flowWk, targetID, targetCnt, pokeID, true );

  for(u32 i=0; i<targetCnt; ++i)
  {
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetID[i] );
    if( target->GetItem() != ITEM_DUMMY_DATA )
    {
      if( (!HandCommon_CheckCantStealPoke(flowWk, pokeID, targetID[i])) )
      {
        BTL_HANDEX_PARAM_SWAP_ITEM* param = reinterpret_cast<BTL_HANDEX_PARAM_SWAP_ITEM *>( flowWk->HANDEX_Push( BTL_HANDEX_SWAP_ITEM, pokeID ));
          param->header.tokwin_flag = true;
          param->pokeID = targetID[i];
          param->fIncRecordCount_StealItemFromWildPoke = true;
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Dorobou );
          HANDEX_STR_AddArg( &param->exStr, pokeID );
          HANDEX_STR_AddArg( &param->exStr, targetID[i] );
          HANDEX_STR_AddArg( &param->exStr, target->GetItem() );
        flowWk->HANDEX_Pop( param );
        break;
      }
    }
  }

  EVENT_FACTOR_ClearWork( myHandle );
}
//------------------------------------------------------------------------------
/**
 *  とくせい「きょうせい」
 *
 * 味方が道具を使ったあと、自分の道具をわたす
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Kyousei( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASEQ_START,          handler_kyousei_wazaSeqStart }, // 技処理開始ハンドラ
    { BTL_EVENT_WAZASEQ_END,            handler_kyousei_wazaSeqEnd   }, // 技処理終了ハンドラ
    { BTL_EVENT_AFTER_ITEMEQUIP,        handler_kyousei }, // 道具使用直後ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 技処理開始ハンドラ
void handler_kyousei_wazaSeqStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  work[0] = true;             // [0] : ワザ処理中であることを示すフラグ
  work[1] = BTL_POKEID_NULL;  // [1] : ワザ処理中に道具を渡す条件がそろったポケモンID（初期値はNULL）
}
// 技処理終了ハンドラ
void handler_kyousei_wazaSeqEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  // ワザ処理中に道具を渡す条件のそろったポケモンがいたら、効果発動させる
  if( work[0] )
  {
    if( work[1] != BTL_POKEID_NULL )
    {
      const BTL_POKEPARAM* self   = flowWk->Hnd_GetPokeParam( pokeID );
      const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( (u8)(work[1]) );

      kyousei_commonProc( flowWk, self, target );

      work[1] = BTL_POKEID_NULL;
    }

    work[0] = false;
  }
}


// 道具使用直後ハンドラ
void handler_kyousei( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( !flowWk->Hnd_GetMainModule()->IsFriendPokeID(pokeID, targetPokeID) ){
    return;
  }

  const BTL_POKEPARAM* self   = flowWk->Hnd_GetPokeParam( pokeID );
  const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetPokeID );

  if( (self->GetItem() == ITEM_DUMMY_DATA)
  ||  (target->GetItem() != ITEM_DUMMY_DATA)
  ||  (self->TURNFLAG_Get(BTL_POKEPARAM::TURNFLG_NAGETUKERU_USING))
  ||  (self->TURNFLAG_Get(BTL_POKEPARAM::TURNFLG_RESERVE_ITEM_SPEND))
  ){
    return;
  }

  // ワザ処理中なら対象ポケモンIDをワーク保存してワザ処理後に再チェック
  if( work[0] ){
    work[1] = targetPokeID;
    return;
  }

  // ワザ処理中でなければ、すぐに発動処理を実行
  kyousei_commonProc( flowWk, self, target );
}

/**
 * @brief  「きょうせい」の共通処理部分
 *
 * @param   flowWk   サーバーフローワーク
 * @param   self     「きょうせい」を発動させるポケモンパラメータ
 * @param   target   「きょうせい」で道具を渡される側のポケモンパラメータ
 */
void kyousei_commonProc( ServerFlow* flowWk, const BTL_POKEPARAM* self, const BTL_POKEPARAM* target )
{
  // 味方以外には発動しない
  if( !(flowWk->Hnd_GetMainModule()->IsFriendPokeID(self->GetID(), target->GetID())) ){
    return;
  }

  // 道具を渡す相手が場に居ないなら、発動しない
  // 道具「だっしゅつボタン」を持った味方が技を受けた場合、この時点で、味方は場に居ない
  const BtlPokePos targetPos = flowWk->Hnd_GetExistFrontPokePos( target->GetID() );
  if( targetPos == BTL_POS_NULL ) {
    return;
  }

  // 自分がアイテムを持ち、相手がアイテムを持っていない状態でなければ、発動しない
  u16  myItem = self->GetItem();
  if( (myItem == ITEM_DUMMY_DATA)
  ||  (target->GetItem() != ITEM_DUMMY_DATA)
  ){
    return;
  }

  // 受け渡し可能なアイテムであれば、発動
  if( (!HandCommon_IsUnbreakablePokeItem(flowWk, target->GetMonsNo(), myItem))
  &&  (!HandCommon_IsUnbreakablePokeItem(flowWk, self->GetMonsNo(), myItem))
  ){
    u8  myPokeID     = self->GetID();
    u8  targetPokeID = target->GetID();

    BTL_HANDEX_PARAM_SWAP_ITEM* param = reinterpret_cast<BTL_HANDEX_PARAM_SWAP_ITEM *>( flowWk->HANDEX_Push( BTL_HANDEX_SWAP_ITEM, myPokeID ));
      param->header.tokwin_flag = true;
      param->pokeID = targetPokeID;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Kyousei );
      HANDEX_STR_AddArg( &param->exStr, myPokeID );
      HANDEX_STR_AddArg( &param->exStr, myItem );
      HANDEX_STR_AddArg( &param->exStr, targetPokeID );
    flowWk->HANDEX_Pop( param );
  }
}

//-----------------------------------------------------------------------------
/**
 * ぎょぐん
 *
 * 場に出たとき、条件を満たしているとフォルムチェンジする
 * ターンチェック時に、状況に応じたフォルムにフォルムチェンジする
 *
 * ※ヨワシにのみ有効
 */
//-----------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Gyogun( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN_EVO,            handler_Gyogun_MemberIn      }, // 全ポケ入場イベント処理後
    { BTL_EVENT_TURNCHECK_DONE,           handler_Gyogun_TurnCheckDone }, // ターンチェック終了ハンドラ
    { BTL_EVENT_IEKI_FIXED,               handler_Gyogun_IekiFixed     },
    { BTL_EVENT_CHANGE_TOKUSEI_BEFORE,    handler_Gyogun_IekiFixed     },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// 全ポケ入場イベント処理後
void handler_Gyogun_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    handler_Gyogun_Common_ChangeForm( flowWk, pokeID );
  }
}

// ターンチェック終了ハンドラ
void handler_Gyogun_TurnCheckDone( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  handler_Gyogun_Common_ChangeForm( flowWk, pokeID );
}

/**
 * @brief ぎょぐん共通処理
 *        ヨワシのフォルムを変更する
 */
void handler_Gyogun_Common_ChangeForm( ServerFlow* flowWk, u8 pokeID )
{
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

  // ヨワシのみ有効
  if( bpp->GetMonsNo() != MONSNO_YOWASI )
  {
    return;
  }

  FormNo nextFormNo = FormChange::YOWASI_IsGyogunFormEnable( *bpp ) ? ( FORMNO_YOWASI_GYOGUN_L ) : ( FORMNO_YOWASI_NORMAL );

  // フォルムが変化しないなら、何もしない
  if( bpp->GetFormNo() == nextFormNo )
  {
    return;
  }

  // フォルムチェンジ
  BTL_HANDEX_PARAM_CHANGE_FORM* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_FORM *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_FORM, pokeID ) );
    param->header.tokwin_flag = true;
    param->pokeID             = pokeID;
    param->formNo             = nextFormNo;
    {
      u16 strID = ( nextFormNo == FORMNO_YOWASI_NORMAL ) ? BTL_STRID_SET_Gyogun_Off : BTL_STRID_SET_Gyogun_On;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, strID );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
    }
  flowWk->HANDEX_Pop( param );
}

// いえき確定
void handler_Gyogun_IekiFixed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue( BTL_EVAR_POKEID ) != pokeID )
  {
    return;
  }

  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  if( bpp->GetMonsNo() != MONSNO_YOWASI )
  {
    return;
  }

  // ノーマルフォルムに戻す
  BTL_HANDEX_PARAM_CHANGE_FORM* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_FORM *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_FORM, pokeID ) );
    param->pokeID = pokeID;
    param->formNo = FORMNO_YOWASI_NORMAL;
    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Gyogun_Off );
    HANDEX_STR_AddArg( &param->exStr, pokeID );
  flowWk->HANDEX_Pop( param );
}


//------------------------------------------------------------------------------
/**
 *  とくせい「じきゅうりょく」
 *
 * 技によるダメージを受けると防御力が１段階アップする。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Zikyuuryoku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,  handler_Zikyuuryoku_WazaDamageReaction },  // ダメージ反応ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// ダメージ反応ハンドラ
void handler_Zikyuuryoku_WazaDamageReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  // 決着がついていたら何もしない
  if( flowWk->Hnd_CheckShowDown() ){
    return;
  }

  if( ( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) != pokeID ) ||
      ( (bool)EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == true ) )
  {
    return;
  }

  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
      param->header.tokwin_flag = true;
      param->poke_cnt           = 1;
      param->pokeID[0]          = pokeID;
      param->rankType           = pml::wazadata::RANKEFF_DEFENCE;
      param->rankVolume         = 1;
    flowWk->HANDEX_Pop( param );
  }
}


//------------------------------------------------------------------------------
/**
 *  とくせい「みずがため」
 *
 * みずタイプの技を受けると、防御力が２段階アップする
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Mizugatame( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION, handler_Mizugatame_WazaDamageReaction },  // ダメージ反応ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// ダメージ反応ハンドラ
void handler_Mizugatame_WazaDamageReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( ( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) != pokeID ) ||
      ( (bool)EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == true ) )
  {
    return;
  }

  // 受けたワザタイプが「みず」なら防御ランクアップ
  if( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_MIZU )
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
      param->header.tokwin_flag = true;
      param->poke_cnt           = 1;
      param->pokeID[0]          = pokeID;
      param->rankType           = pml::wazadata::RANKEFF_DEFENCE;
      param->rankVolume         = 2;
    flowWk->HANDEX_Pop( param );
  }
}


//------------------------------------------------------------------------------
/**
 *  とくせい「すいほう」
 *
 * 自分に対する炎の威力を半減する。
 * やけどしない。
 * 「みず」タイプの技の威力が２倍。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Suihou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER,       handler_Suihou_AttackerPower },  // 攻撃側の攻撃値決定
    { BTL_EVENT_ADDSICK_CHECKFAIL,    handler_MizuNoBale_PokeSick  },  // ポケモン系状態異常処理ハンドラ
    { BTL_EVENT_ADDSICK_FAILED,       handler_AddSickFailCommon    },
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_MizuNoBale_Wake      },  // とくせい書き換えハンドラ
    { BTL_EVENT_MEMBER_IN,            handler_MizuNoBale_Wake      },  // メンバー入場ハンドラ
    { BTL_EVENT_ACTPROC_END,          handler_MizuNoBale_ActEnd    },  // アクション終了毎ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable); 
  return HandlerTable;
}

// 攻撃側の攻撃値決定
void handler_Suihou_AttackerPower( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);

  pml::PokeType wazaType = EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );

  // 自分に対する炎の威力を半減する
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    if( wazaType == POKETYPE_HONOO )
    {
      EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5) );
    }
  }

  // 「みず」タイプの技の威力が２倍
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( wazaType == POKETYPE_MIZU )
    {
      EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}


//------------------------------------------------------------------------------
/**
 *  とくせい「ゆきかき」
 *
 * 天候が「あられ」の時、すばやさ2倍
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Yukikaki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY, handler_Yukikaki_CalcAgility },  // すばやさ計算ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// すばやさ計算ハンドラ
void handler_Yukikaki_CalcAgility( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) != pokeID )
  {
    return;
  }

  if( flowWk->Hnd_GetWeather() == BTL_WEATHER_SNOW )
  {
    EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
  }
}


//------------------------------------------------------------------------------
/**
 *  とくせい「はがねつかい」
 *
 * はがねタイプの技の威力が1.5倍になる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Haganetukai( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_Haganetukai_AttackerPower },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// 攻撃威力決定のハンドラ
void handler_Haganetukai_AttackerPower( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID )
  {
    return;
  }

  if( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_HAGANE )
  {
    EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.5) );
  }
}


//------------------------------------------------------------------------------
/**
 *  とくせい「うるおいボイス」
 *
 * 全ての音技がみずタイプになる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_UruoiVoice( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM, handler_UruoiVoice_WazaParam },  // ワザタイプ決定ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// ワザタイプ決定ハンドラ
void handler_UruoiVoice_WazaParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) != pokeID )
  {
    return;
  }

  WazaNo waza = static_cast<WazaNo>( EVENTVAR_GetValue( BTL_EVAR_WAZAID ) );
  if( WAZADATA_GetFlag( waza, pml::wazadata::WAZAFLAG_Sound ) )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, POKETYPE_MIZU );
  }
}


//------------------------------------------------------------------------------
/**
 *  とくせい「ヒーリングシフト」
 *
 * 回復技( 「かいふくふうじ」対象の技 )が先制で出せる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_HealingShift( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_GET_WAZA_PRI,   handler_HealingShift_GetWazaPriority }, // ワザプライオリティ取得ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// ワザプライオリティ取得ハンドラ
void handler_HealingShift_GetWazaPriority( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID )
  {
    return;
  }

  WazaID waza = static_cast<WazaNo>( EVENTVAR_GetValue( BTL_EVAR_WAZAID ) );
  if( WAZADATA_GetFlag( waza, pml::wazadata::WAZAFLAG_KaifukuHuuji ) )
  {
    int pri = EVENTVAR_GetValue( BTL_EVAR_WAZA_PRI );
    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_PRI, pri+3 );
  }
}


//------------------------------------------------------------------------------
/**
 *  とくせい「エレキスキン」
 *
 * ノーマルタイプの技がでんきタイプになる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_ElecSkin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM,     handler_ElecSkinWazaParam },  // ワザタイプ決定ハンドラ
    { BTL_EVENT_WAZA_POWER,     handler_ElecSkin_Pow      },  // 威力計算
    { BTL_EVENT_DAMAGEPROC_END, handler_SkinEndCommon     },  // ダメージ計算処理終了ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// ワザタイプ決定ハンドラ
void  handler_ElecSkinWazaParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Skin_ChangeType( myHandle, flowWk, pokeID, work, POKETYPE_DENKI );
}

// 威力計算
void  handler_ElecSkin_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Skin_WazaPow( myHandle, flowWk, pokeID, work, POKETYPE_DENKI );
}


//------------------------------------------------------------------------------
/**
 *  とくせい「サーフテール」
 *
 * エレキフィールド状態だと素早さが倍になる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_SurfTail( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY, handler_SurfTail_CalcAgility }, // すばやさ計算ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// すばやさ計算ハンドラ
void  handler_SurfTail_CalcAgility( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) != pokeID )
  {
    return;
  }

  if( flowWk->Hnd_GetGround() == BTL_GROUND_ELEKI )
  {
    EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
  }
}


//------------------------------------------------------------------------------
/**
 *  とくせい「ひとでなし」
 *
 * どく状態の相手に対する攻撃が必ず急所に当たる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Hitodenasi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CRITICAL_CHECK, handler_Hitodenasi_CriticalCheck }, // クリティカルチェックハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// クリティカルチェックダメージハンドラ
void handler_Hitodenasi_CriticalCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( pokeID != EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) )
  {
    return;
  }

  const BTL_POKEPARAM* defender = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
  if( defender->CheckSick( pml::wazadata::WAZASICK_DOKU ) )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_CRITICAL_RANK, BTL_CALC_CRITICAL_MAX );
  }
}


//------------------------------------------------------------------------------
/**
 *  とくせい「えんかく」
 *
 * すべての技が非接触になる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Enkaku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM, handler_Enkaku_WazaParam },  // ワザタイプ決定ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// ワザタイプ決定ハンドラ
void  handler_Enkaku_WazaParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  // 非接触に変更
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TOUCH_FLAG, false );
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「じょおうのいげん・ビビッドボディ」
 *
 * 相手は先制攻撃技を出せなくなる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Zyoounoigen( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD, handler_Zyoounoigen_WazaExeCheck },  // ワザ出し成否チェックハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// ワザ出し成否チェックハンドラ
void handler_Zyoounoigen_WazaExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);

  const u8 attackPokeId = EVENTVAR_GetValue( BTL_EVAR_POKEID );
  const s32 wazaPri = EVENTVAR_GetValue( BTL_EVAR_WAZA_PRI );

  // 先制技でない場合、技を撃つのが味方の場合は、反応しない
  if( ( wazaPri < 1 ) ||
      ( flowWk->Hnd_GetMainModule()->IsFriendPokeID( pokeID, attackPokeId ) ) )
  {
    return;
  }

  // 技ターゲットに「じょおうのいげん」陣営のポケモンが含まれていない場合は、反応しない
  // ( 相手に「じょおうのいげん」がいても、まもる・つるぎのまい等は出せる )
  const BtlSide mySide = flowWk->Hnd_GetMainModule()->PokeIDtoSide( pokeID );
  if( !HandCommon_CheckTargetSide( flowWk, mySide ) )
  {
    return;
  }

  // 技を失敗させる
  if( EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_TOKUSEI ) )
  {
    //「○○は　××を　つかえない！」
    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      param->header.tokwin_flag = true;
      HANDEX_STR_Setup( &(param->str), BTL_STRTYPE_SET, BTL_STRID_SET_WazaCantUse );
      HANDEX_STR_AddArg( &(param->str), attackPokeId );
      HANDEX_STR_AddArg( &(param->str), EVENTVAR_GetValue(BTL_EVAR_WAZAID) );
    flowWk->HANDEX_Pop( param );
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「もふもふ」
 *
 * 接触攻撃のダメージを半減する。
 * 炎のダメージが倍になる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_MohuMohu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3, handler_MohuMohu },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// ダメージ計算最終段階のハンドラ
void handler_MohuMohu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);

  const bool wazaTouchFlag = static_cast<bool>( EVENTVAR_GetValue( BTL_EVAR_WAZA_TOUCH_FLAG ) );
  const pml::PokeType wazaType = static_cast<pml::PokeType>( EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE ) );

  if( EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF ) != pokeID )
  {
    return;
  }

  // 接触技のダメージが半減
  if( wazaTouchFlag )
  {
    EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5) );
  }

  // ほのおタイプのダメージが倍
  if( wazaType == POKETYPE_HONOO )
  {
    EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「バッテリー」
 *
 * 味方の特殊技の威力を1.3倍にする。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_TOK_ADD_Battery( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = 
  {
    { BTL_EVENT_WAZA_POWER,  handler_Battery_WazaPower }, // ワザ威力計算ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// 威力計算ハンドラ
void handler_Battery_WazaPower( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  const u8 attackPokeId = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );

  // 自分の技の威力は上がらない
  if( attackPokeId == pokeID )
  {
    return;
  }

  // 味方以外の技の威力は上がらない
  if( !flowWk->Hnd_GetMainModule()->IsFriendPokeID( pokeID, attackPokeId ) )
  {
    return;
  }

  // 使用する技が特殊技なら、威力が上がる
  if( EVENTVAR_GetValue( BTL_EVAR_DAMAGE_TYPE ) == pml::wazadata::DMG_SPECIAL )
  {
    EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.3) );
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「レシーバー・かがくのちから」
 *
 * 倒された味方と同じ特性になる
 *
 * @fix NMCat[7]「かがくのちから」でメガシンカ後の特性がコピー出来ない
 * BTL_EVENT_DEAD_AFTER は、メガシンカ前の特性に戻した後のいべんとなので、
 * 特性コピーのタイミングを BTL_EVENT_DEAD_AFTER から、BTL_EVENT_DEAD_ACTION_AFTER に変更。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_TOK_ADD_Receiver( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = 
  {
    { BTL_EVENT_DEAD_ACTION_AFTER,  handler_Receiver_DeadAfter }, // ポケモン死亡による退場の直後
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// ポケモン死亡による退場の直後
void handler_Receiver_DeadAfter( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  const u8 deadPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );

  if( ( deadPokeID == pokeID ) ||
     !( flowWk->Hnd_GetMainModule()->IsFriendPokeID( pokeID, deadPokeID ) ) )
  {
    return;
  }

  const BTL_POKEPARAM* deadPoke = flowWk->Hnd_GetPokeParam( deadPokeID );
  const TokuseiNo nextTokusei = static_cast<TokuseiNo>( deadPoke->GetValue( BTL_POKEPARAM::BPP_TOKUSEI ) );

  if( ( nextTokusei != TOKUSEI_NULL ) &&
      ( !tables::CheckReceiverFailTokusei( nextTokusei ) ) )
  {
    BTL_HANDEX_PARAM_CHANGE_TOKUSEI* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_TOKUSEI *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_TOKUSEI, pokeID ));
      param->pokeID = pokeID;
      param->tokuseiID = nextTokusei;
      param->fSkipMemberInEvent = true;
      param->header.tokwin_flag = true;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Receiver );
      HANDEX_STR_AddArg( &param->exStr, deadPokeID );
      HANDEX_STR_AddArg( &param->exStr, nextTokusei );
    flowWk->HANDEX_Pop( param );
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「とびだすなかみ」
 *
 * 倒された時のHP残量分、相手にダメージを与える。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_TOK_ADD_TobidasuNakami( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = 
  {
    { BTL_EVENT_DAMAGEPROC_START,    handler_TobidasuNakami_DamageProcStart     }, // ダメージプロセス開始ハンドラ
    { BTL_EVENT_ICHIGEKI_GUARD,      handler_TobidasuNakami_IchigekiGuard       }, // 一撃必殺ワザが当たること確定した（計算前）
    { BTL_EVENT_WAZA_DMG_REACTION,   handler_TobidasuNakami_WazaDamageReaction  }, // ダメージ処理１体ごとの反応
    { BTL_EVENT_ICHIGEKI_SUCCEED,    handler_TobidasuNakami_WazaDamageReaction  }, // 一撃必殺ワザ成功確定ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// ダメージプロセス開始ハンドラ
void handler_TobidasuNakami_DamageProcStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  if( !HandCommon_CheckTargetPokeID( pokeID ) )
  {
    return;
  }

  // 相手に与えるダメージ量を記憶
  tobidasuNakami_RegisterDamage( flowWk, pokeID, work );
}

// 一撃必殺ワザが当たること確定した（計算前）
void handler_TobidasuNakami_IchigekiGuard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  const u8 defPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  if( defPokeID != pokeID )
  {
    return;
  }

  // 相手に与えるダメージ量を記憶
  tobidasuNakami_RegisterDamage( flowWk, pokeID, work );
}

/**
 * @brief 「とびだすなかみ」で相手に与えるダメージ量を記憶
 */
void tobidasuNakami_RegisterDamage( ServerFlow* flowWk, u8 pokeID, int* work )
{
  const BTL_POKEPARAM* poke = flowWk->Hnd_GetPokeParam( pokeID );
  work[0] = poke->GetValue( BTL_POKEPARAM::BPP_HP );
}

// ダメージ処理１体ごとの反応
void handler_TobidasuNakami_WazaDamageReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  const u8 attackPokeID  = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
  const u8 defensePokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );

  if( ( defensePokeID != pokeID ) ||
      ( work[0] <= 0 ) )
  {
    return;
  }

  const BTL_POKEPARAM* atkPoke = flowWk->Hnd_GetPokeParam( attackPokeID );
  const BTL_POKEPARAM* defPoke = flowWk->Hnd_GetPokeParam( defensePokeID );

  // ダメージを与える
  // 
  // このダメージでは、死亡した場合でも退場処理を行いません。
  // ここで退場処理を行ってしまうと、両陣営が同時に全滅した場合の勝敗判定が反転してしまうためです。
  //
  // 両陣営が全滅した場合、後に退場したほうの勝ちとなりますが、
  // ここで退場処理を行ってしまうと
  // 防御側が技ダメージによって退場する前に
  // 攻撃側が「さめはだ」のダメージで退場してしまうため、攻撃側が負けになってしまいます。
  if( !atkPoke->IsDead() &&
       defPoke->IsDead() )
  {
    BTL_HANDEX_PARAM_DAMAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_DAMAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_DAMAGE, pokeID ));
      param->header.tokwin_flag  = true;
      param->pokeID              = attackPokeID;
      param->damage              = work[0];
      param->damageCause         = DAMAGE_CAUSE_TOBIDASUNAKAMI;
      param->damageCausePokeID   = pokeID;
      param->fDisableDeadProcess = true;                         // 死亡した場合でも退場処理を行わない
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_SimpleDamage );
      HANDEX_STR_AddArg( &param->exStr, attackPokeID );
    flowWk->HANDEX_Pop( param );

    work[0] = 0;
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「リミットシールド」
 *
 * メテノにのみ有効
 * ターンチェック終了時、HPが半分以下ならコアフォルム１に、HPが半分超なら殻つきフォルムに変化する
 * 殻つきフォルムなら、基本状態異常＋あくび状態にならない
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_TOK_ADD_LimitShield( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = 
  {
    { BTL_EVENT_MEMBER_IN_EVO,      handler_LimitShield_MemberIn      },  // ポケ入場ハンドラ
    { BTL_EVENT_TURNCHECK_DONE,     handler_LimitShield               },  // ターンチェック終了ハンドラ
    { BTL_EVENT_ADDSICK_CHECKFAIL,  handler_LimitSheild_SickCheck     },  // 状態異常失敗チェックハンドラ
    { BTL_EVENT_ADDSICK_FAILED,     handler_AddSickFailCommon         },  // 状態異常失敗
    { BTL_EVENT_CHECK_INEMURI,      handler_Fumin_InemuriCheck        },  // いねむりチェック
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// ポケ入場完了ハンドラ
void handler_LimitShield_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    LimitShield_ChangeForm( flowWk, pokeID );
  }
}

// ターンチェック終了ハンドラ
void handler_LimitShield( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  LimitShield_ChangeForm( flowWk, pokeID );
}

/**
 * @brief [リミットシールド] フォルムを変更する
 * @param pokeID  リミットシールド対象ポケモンのID
 */
void LimitShield_ChangeForm( ServerFlow* flowWk, u8 pokeID )
{
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  if( bpp->GetMonsNo() != MONSNO_NAGAREBOSI )
  {
    return;
  }

  const FormNo form_old  = bpp->GetFormNo();
  const FormNo form_next = FormChange::METENO_CalcFormNo( *bpp );
  if( form_next == form_old )
  {
    return;
  }

  {
    BTL_HANDEX_PARAM_CHANGE_FORM* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_FORM *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_FORM, pokeID ));
      param->header.tokwin_flag  = true;
      param->pokeID              = pokeID;
      param->formNo              = form_next;
      {
        u16 strID = ( pml::personal::METENO_IsShellForm( form_next ) ) ? ( BTL_STRID_STD_LimitShieldOn ) : ( BTL_STRID_STD_LimitShieldOff );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, strID );
      }
    flowWk->HANDEX_Pop( param );
  }
}

// 状態異常失敗チェック
void handler_LimitSheild_SickCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF ) != pokeID )
  {
    return;
  }

  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  const FormNo formno = bpp->GetFormNo();
  if(  ( bpp->GetMonsNo() != MONSNO_NAGAREBOSI ) ||
      !( pml::personal::METENO_IsShellForm( formno ) ) )
  {
    return;
  }
  
  WazaSick sickID = (WazaSick)(EVENTVAR_GetValue(BTL_EVAR_SICKID));
  if( IsPokeSick(sickID) || sickID == pml::wazadata::WAZASICK_AKUBI )
  {
    work[WIDX0] = EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「スワームチェンジ」
 *
 * ジガルデにのみ有効
 * ターンチェック終了時、HPが半分以下になるとパーフェクトジガルデにフォルムチェンジする。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_TOK_ADD_SwarmChange( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = 
  {
    { BTL_EVENT_TURNCHECK_DONE,  handler_SwarmChange },    // ターンチェック終了ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// ターンチェック終了ハンドラ
void handler_SwarmChange( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  // ジガルデにのみ有効
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  if( bpp->GetMonsNo() != MONSNO_ZIGARUDE )
  {
    return;
  }

  const u32    hp        = bpp->GetValue( BTL_POKEPARAM::BPP_HP );
  const u32    hp_border = calc::QuotMaxHP_Zero( bpp, 2 );
  const FormNo form_old  = bpp->GetFormNo();
  const FormNo form_next = ( hp <= hp_border ) ? ( FORMNO_ZIGARUDE_100PER ) : ( form_old );
  if( form_next == form_old )
  {
    return;
  }

  //「たくさんの　けはいを　かんじる…！」
  {
    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE*>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ) );
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_SwarmChange );
    flowWk->HANDEX_Pop( param );
  }

  {
    BTL_HANDEX_PARAM_CHANGE_FORM* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_FORM *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_FORM, pokeID ));
      param->header.tokwin_flag  = true;
      param->pokeID              = pokeID;
      param->formNo              = form_next;
      param->fDontResetFormByOut = true;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_SwarmChange );  //「○○は　パーフェクトフォルムに　かわった！」
      HANDEX_STR_AddArg( &param->exStr, pokeID );
    flowWk->HANDEX_Pop( param );
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「ぎゃくじょう」
 *
 * 技のダメージによってHPが1/2以下になると、とくこうが1段階アップする
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Gyakuzyou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,        handler_Gyakuzyou_DamegeProcStart }, // ダメージプロセス開始ハンドラ
    { BTL_EVENT_ICHIGEKI_CHECK,          handler_Gyakuzyou_IchigekiCheck   }, // 一撃必殺ワザ成功チェックハンドラ
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL, handler_Gyakuzyou_EndHitReal      }, // ダメージ処理後（みがわり除く）
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// ダメージプロセス開始ハンドラ
void handler_Gyakuzyou_DamegeProcStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  // 自身がターゲットに含まれていなければ、何もしない
  if( !common_CheckTarget( pokeID ) )
  {
    return;
  }

  work[0] = gyakuzyou_isEnoughHP( flowWk, pokeID );
}
// 一撃必殺ワザ成功チェックハンドラ
void handler_Gyakuzyou_IchigekiCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  // 自身がターゲットでなければ何もしない
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) != pokeID ){
    return;
  }

  work[0] = gyakuzyou_isEnoughHP( flowWk, pokeID );
}
// ダメージ前に「ぎゃくじょう」発動可能なHPが残っているかチェック
bool gyakuzyou_isEnoughHP( ServerFlow* flowWk, u8 pokeID )
{
  // ダメージを受ける前に、残りHPが1/2より多ければ、発動可能
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  const u32 borderHp = calc::QuotMaxHP_Zero( bpp, 2 );
  const u32 hp = bpp->GetValue( BTL_POKEPARAM::BPP_HP );
  return ( borderHp < hp );
}


// ダメージ処理後
void handler_Gyakuzyou_EndHitReal( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  // 決着がついていたら何もしない
  if( flowWk->Hnd_CheckShowDown() )
  {
    return;
  }

  // 自身がターゲットに含まれていなければ、何もしない
  if( !common_CheckTarget( pokeID ) )
  {
    return;
  }

  // ダメージを受ける前に、残りHPが1/2以下だったら、何もしない
  if( work[0] != 1 )
  {
    return;
  }

  // ダメージを受けた後、残りHPが1/2以下でないなら、何もしない
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  const u32 borderHp = calc::QuotMaxHP_Zero( bpp, 2 );
  const u32 hp = bpp->GetValue( BTL_POKEPARAM::BPP_HP );
  if( borderHp < hp )
  {
    return;
  }

  // とくこう上昇
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
      param->header.tokwin_flag = true;
      param->poke_cnt           = 1;
      param->pokeID[0]          = pokeID;
      param->rankType           = pml::wazadata::RANKEFF_SP_ATTACK;
      param->rankVolume         = 1;
    flowWk->HANDEX_Pop( param );
  }

  work[0] = 0;
}

/**
 * @brief 共通処理：指定したポケモンが技ターゲットに含まれているかチェックする
 * @param checkPokeID  チェック対象ポケモンのID
 * @retval true   含まれている
 * @retval false  含まれていない
 */
bool common_CheckTarget( u8 checkPokeID )
{
  const u32 targetNum = EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );

  for( u32 i=0; i<targetNum; ++i )
  {
    u32 targetPokeID = EVENTVAR_GetValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1 + i) );
    if( targetPokeID == checkPokeID )
    {
      return true;
    }
  }

  return false;
}


/**
 * にげごし・ききかいひ用ワークインデックス
 */
enum NigegoshiWorkIdx {
  WIDX_NIGEGOSI_FULFILL_ENOUGH_HP = 0,  // ＨＰが一定割合以上のこっている条件を満たしているか？
  WIDX_NIGEGOSI_ATTACKER_DMG_STATUS,    // 自分の攻撃処理時にアイテム処理でダメージを受ける場合の状態判定
};

/**
 * にげごし・ききかいひ用 自分が攻撃時のアイテム処理ダメージ状態
 */
enum NigegoshiAttackerDamageStatus {
  NIGEGOSI_ATTACKER_DMG_STATUS_NONE = 0,  // 初期状態
  NIGEGOSI_ATTACKER_DMG_STATUS_MYATTACK,  // 自分の攻撃処理中
  NIGEGOSI_ATTACKER_DMG_STATUS_FULFILL_ITEM_EFFECT,  // アイテム処理による効果発動条件を満たした
};


//------------------------------------------------------------------------------
/**
 *  とくせい「にげごし・ききかいひ」
 *
 * HPが半分になると逃げる。自動で交代する。野生なら逃げる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Nigegosi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,        handler_Nigegosi_DamegeProcStart    }, // ダメージプロセス開始ハンドラ
    { BTL_EVENT_DAMAGEPROC_END,          handler_Nigegosi_DamegeProcEnd      }, // ダメージプロセス終了ハンドラ
    { BTL_EVENT_DAMAGEPROC_END_HIT_L3,   handler_Nigegosi_EndHit             }, // ダメージ処理後（みがわり除く）
    { BTL_EVENT_ICHIGEKI_GUARD,          handler_Nigegosi_BeforeIchigeki     }, // 一撃必殺ワザダメージ開始前処理

    { BTL_EVENT_SIMPLE_DAMAGE_BEFORE,    handler_Nigegosi_SimpleDamageBefore }, // ワザ以外ダメージを受ける直前
    { BTL_EVENT_SIMPLE_DAMAGE_AFTER,     handler_Nigegosi_SimpleDamageAfter  }, // ワザ以外ダメージを受けた後( アイテム反応後 )
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// ダメージプロセス開始ハンドラ
void handler_Nigegosi_DamegeProcStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  // 自分のワザによるプロセスであることを記憶しておく
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID ){
    work[WIDX_NIGEGOSI_ATTACKER_DMG_STATUS] = NIGEGOSI_ATTACKER_DMG_STATUS_MYATTACK;
  }

  // 自身がターゲットに含まれていなければ、何もしない
  if( !common_CheckTarget( pokeID ) )
  {
    return;
  }

  nigegosi_CheckBeforeDamage( flowWk, pokeID, work );
}
// 一撃必殺ワザダメージ開始前処理
void handler_Nigegosi_BeforeIchigeki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  // 自分のワザによるプロセスであることを記憶しておく
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID ){
    work[WIDX_NIGEGOSI_ATTACKER_DMG_STATUS] = NIGEGOSI_ATTACKER_DMG_STATUS_MYATTACK;
  }

  // 自身がターゲットに含まれていなければ、何もしない
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) != pokeID )
  {
    return;
  }

  nigegosi_CheckBeforeDamage( flowWk, pokeID, work );
}

// ダメージプロセス終了ハンドラ
void handler_Nigegosi_DamegeProcEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  // 自分のワザによるプロセスであることを記憶したワークを、クリアする
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID ){
    work[WIDX_NIGEGOSI_ATTACKER_DMG_STATUS] = NIGEGOSI_ATTACKER_DMG_STATUS_NONE;
  }
}


// ダメージ処理後
void handler_Nigegosi_EndHit( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  // 自分のワザによるプロセス中、アイテムダメージによる反応が確定していた場合はここで処理する
  if( (work[WIDX_NIGEGOSI_ATTACKER_DMG_STATUS] == NIGEGOSI_ATTACKER_DMG_STATUS_FULFILL_ITEM_EFFECT)
  &&  (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)  // 自分攻撃->だっしゅつボタン等->わりこみおいうちで他ポケモンの攻撃に反応することがあり得るので除外
  ){
    nigegosi_AfterDamage_Effect( flowWk, pokeID );
    return;
  }

  // 自身がターゲットに含まれていなければ、何もしない
  if( !common_CheckTarget( pokeID ) )
  {
    return;
  }

  // 「おいうち」割り込みアクションなら、反応しない
  if( EVENTVAR_GetValue( BTL_EVAR_OIUTI_INTERRUPT_FLAG ) )
  {
    return;
  }

  nigegosi_AfterDamage_Full( flowWk, pokeID, work );
}

// ワザ以外ダメージを受ける直前
void handler_Nigegosi_SimpleDamageBefore( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( EVENTVAR_GetValue( BTL_EVAR_POKEID ) != pokeID )
  {
    return;
  }
  nigegosi_CheckBeforeDamage( flowWk, pokeID, work );
}

// ワザ以外ダメージを受けた後( アイテム反応後 )
void handler_Nigegosi_SimpleDamageAfter( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  // 自分がダメージ対象じゃないなら何もしない
  if( EVENTVAR_GetValue( BTL_EVAR_POKEID ) != pokeID )
  {
    return;
  }

  // 発動条件が満たされていないなら何もしない
  if( !nigegosi_AfterDamage_shouldEffect(flowWk, pokeID, work) ){
    return;
  }

  work[0] = 0;

  // 発動条件が満たされていても、自分の攻撃処理中なら何もせず、ダメージ処理後のハンドラに任せる
  if( work[WIDX_NIGEGOSI_ATTACKER_DMG_STATUS] == NIGEGOSI_ATTACKER_DMG_STATUS_MYATTACK ){
    work[WIDX_NIGEGOSI_ATTACKER_DMG_STATUS] = NIGEGOSI_ATTACKER_DMG_STATUS_FULFILL_ITEM_EFFECT;
    return;
  }

  nigegosi_AfterDamage_Effect( flowWk, pokeID );
}

/**
 * @brief 「にげごし」共通処理：ダメージ前の発動チェック
 * @param pokeID  「にげごし」のポケモン
 * @param work    ハンドラの作業領域
 */
void nigegosi_CheckBeforeDamage( ServerFlow* flowWk, u8 pokeID, int* work )
{
  // ぬし戦、試練中は発動しない
  if( flowWk->Hnd_GetStatusFlag( BTL_STATUS_FLAG_VS_NUSI ) ||
      flowWk->Hnd_GetStatusFlag( BTL_STATUS_FLAG_SIREN_BATTLE ) ||
      // @fix GFMMCat[209] VSヒカリドラゴン戦で、にげることができる
      // @fix GFMMCat[547] 合体ネクロズマ戦にて逃げられる
      flowWk->Hnd_GetStatusFlag( BTL_STATUS_FLAG_NO_CAPTURE ))
  {
    return;
  }

  // ダメージを受ける前に、残りHPが1/2より多ければ、発動可能
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  const u32 borderHp = calc::QuotMaxHP_Zero( bpp, 2 );
  const u32 hp = bpp->GetValue( BTL_POKEPARAM::BPP_HP );
  work[0] = ( borderHp < hp );
}

/**
 * @brief 「にげごし」共通処理：ダメージ後の発動処理（除外条件のチェックも含めた全処理）
 * @param pokeID  「にげごし」のポケモン
 * @param work    ハンドラの作業領域
 */
void nigegosi_AfterDamage_Full( ServerFlow* flowWk, u8 pokeID, int* work )
{
  // 発動条件が満たされていないなら何もしない
  if( !nigegosi_AfterDamage_shouldEffect(flowWk, pokeID, work) ){
    return;
  }

  work[0] = 0;

  nigegosi_AfterDamage_Effect( flowWk, pokeID );
}

/**
 * @brief 「にげごし」共通処理：ダメージ後の発動処理（効果発動させるかどうか判定）
 *
 * @retval bool  発動させる場合は true
 */
bool nigegosi_AfterDamage_shouldEffect( ServerFlow* flowWk, u8 pokeID, int* work )
{
  // 決着がついていたら何もしない
  if( flowWk->Hnd_CheckShowDown() )
  {
    return false;
  }

  // @fix NMCat[2701] 手持ちが3匹の状態で「にげごし」「ききかいひ」による交代処理が1回の行動中に連続して発生するととまる
  //「おいうち」による割り込み処理中なら、何もしない
  if( flowWk->Hnd_IsMemberOutIntr() )
  {
    return false;
  }

  // ダメージを受ける前に、残りHPが1/2以下だったら、何もしない
  if( work[0] != 1 )
  {
    return false;
  }

  // ダメージを受けた後、残りHPが1/2以下でないなら、何もしない
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  const u32 borderHp = calc::QuotMaxHP_Zero( bpp, 2 );
  const u32 hp = bpp->GetValue( BTL_POKEPARAM::BPP_HP );
  if( borderHp < hp )
  {
    return false;
  }

  // ひん死になっていたら何もしない
  if( hp == 0 ){
    return false;
  }

  // 乱入戦で、味方がまだ場にいるなら何もしない
  if( HandCommon_IsExistIntrudeFriend(flowWk, pokeID) ){
    return false;
  }

  return true;
}

/**
 * @brief 「にげごし」共通処理：ダメージ後の発動処理（効果発動処理）
 * @param pokeID  「にげごし」のポケモン
 * @param work    ハンドラの作業領域
 */
void nigegosi_AfterDamage_Effect( ServerFlow* flowWk, u8 pokeID )
{
  // 野生戦なら、戦闘を終了させる
  if( flowWk->Hnd_GetCompetitor() == BTL_COMPETITOR_WILD )
  {
    BTL_HANDEX_PARAM_QUIT_BATTLE* param = reinterpret_cast<BTL_HANDEX_PARAM_QUIT_BATTLE *>( flowWk->HANDEX_Push( BTL_HANDEX_QUIT_BATTLE, pokeID ));
      param->header.tokwin_flag = true;
      param->fForceNigeru = true;  // @fix GFNMCat[337] 野生戦で相手が特性「かげふみ」のとき、特性「にげごし・ききかいひ」で逃げられない
    flowWk->HANDEX_Pop( param );
  }
  else
  {
    // 野生戦以外なら、控えと交換
    // @fix NMCat[75] ターンエンド処理で複数の「ききかいひ」「にげごし」の発動条件を満たしても、1匹しか特性効果が発動しない
    // 対処：既に発生している交換リクエストの数を考慮して、交換可能かどうかを判定する
    if( flowWk->Hnd_IsExistChangeEnablePoke( pokeID) )
    {
      BTL_HANDEX_PARAM_CHANGE_MEMBER* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_MEMBER *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_MEMBER, pokeID ));
        param->header.tokwin_flag = true;
        param->pokeID = pokeID;
        param->fIntrDisable = true;
      flowWk->HANDEX_Pop( param );
    }
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「ソウルハート」
 *
 * 場のポケモンが瀕死になるたび特攻が１段階アップする。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_TOK_ADD_SoulHeart( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = 
  {
    { BTL_EVENT_DEAD_AFTER,  handler_SoulHeart_DeadAfter }, // ポケモン死亡による退場の直後
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// ポケモン死亡による退場の直後
void handler_SoulHeart_DeadAfter( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  // とくこう上昇
  if( !flowWk->Hnd_CheckShowDown() )
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
      param->header.tokwin_flag = true;
      param->poke_cnt           = 1;
      param->pokeID[0]          = pokeID;
      param->rankType           = pml::wazadata::RANKEFF_SP_ATTACK;
      param->rankVolume         = 1;
    flowWk->HANDEX_Pop( param );
  }
}

//----------------------------------------------------------------------------------
/**
 * おどりこ
 *
 * 場のポケモンがおどり技を使った場合、自分も直後にその技を出す。
 *
 * @attention 今後「踊り技」かつ「反動技」である技が登場すると、NMCat[4242] と同じ症状が発生します。
 * １．「おどりこ」で出た技で「反動で動けない」状態になる
 * ２．１と同ターンに「フリーフォール」を撃つ
 * ３．「フリーフォール」の解放ターンが「反動で動けない」ために実行されず、画面から復帰しないままバトルが継続してしまう
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_TOK_ADD_Odoriko( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASEQ_START,           handler_Odoriko_WazaSeqStart     }, // ワザシーケンス開始ハンドラ
    { BTL_EVENT_WAZA_EXECUTE_EFFECTIVE,  handler_Odoriko_ExecuteEffective }, // ワザ出し有効ハンドラ
    { BTL_EVENT_WAZASEQ_END,             handler_Odoriko_WazaSeqEnd       }, // ワザシーケンス終了ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// ワザシーケンス開始ハンドラ
void handler_Odoriko_WazaSeqStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  // 自分以外のワザシーケンス開始時、自分が場にいたら work[2] をONにする
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID )
  {
    work[2] = 1;
  }
}

// ワザ出し有効ハンドラ
void handler_Odoriko_ExecuteEffective( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);

  // 既に反応済み
  if( work[0] != 0 )
  {
    return;
  }

  // 処理開始時、自分が場にいなかったワザに対しては反応しない
  if( work[2] == 0 ){
    return;
  }

  // 通常の技出しにしか反応しない( おどりこ・よこどり・マジックコート等によって出た技には反応しない )
  if( EVENTVAR_GetValue( BTL_EVAR_ACTION_DESC_IS_YOKODORI_ROB_ACTION ) ||
      EVENTVAR_GetValue( BTL_EVAR_ACTION_DESC_IS_MAGICCOAT_REACTION ) ||
      EVENTVAR_GetValue( BTL_EVAR_ACTION_DESC_IS_ODORIKO_REACTION ) )
  {
    return;
  }  

  // 自分の技には反応しない
  if( EVENTVAR_GetValue( BTL_EVAR_POKEID ) == pokeID )
  {
    return;
  }

  // 場に居ないなら反応しない
  {
    const BTL_POKEPARAM* poke = flowWk->Hnd_GetPokeParam( pokeID );
    if( poke->IsWazaHide() )
    {
      return;
    }
  }

  // 踊り技にしか反応しない
  const WazaNo wazano = static_cast<WazaNo>( EVENTVAR_GetValue( BTL_EVAR_WAZAID ) );
  const bool isDanceWaza = WAZADATA_GetFlag( wazano, pml::wazadata::WAZAFLAG_Dance );
  if( !isDanceWaza )
  {
    return;
  }

  work[0] = 1;
  work[1] = wazano;
}

// ワザシーケンス終了ハンドラ
void handler_Odoriko_WazaSeqEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  work[2] = 0;

  if( work[0] != 1 )
  {
    return;
  }

  const u8 attackPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );
  const BtlPokePos targetPos = odoriko_GetTargetPos( flowWk, pokeID, attackPokeID );
    
  if( targetPos != BTL_POS_NULL )
  {
    BTL_HANDEX_PARAM_INSERT_WAZA_ACTION* param = reinterpret_cast<BTL_HANDEX_PARAM_INSERT_WAZA_ACTION *>( flowWk->HANDEX_Push( BTL_HANDEX_INSERT_WAZA_ACTION, pokeID ));
      param->header.tokwin_flag = true;
      param->actPokeID          = pokeID;
      param->actWazaNo          = static_cast<WazaNo>( work[1] );
      param->targetPos          = targetPos;
      param->actionDesc.isOdorikoReaction = true;
    flowWk->HANDEX_Pop( param );
  }

  work[0] = 0;
}

/**
 * @brief 特性「おどりこ」によって発動する技のターゲット位置を取得する
 * @param odorikoPokeID  特性「おどりこ」のポケモン
 * @param attackPokeID   踊り技を使用したポケモン
 *
 * 踊り技を使用したポケモンが相手なら、踊り技を使用したポケモンをターゲットにする
 * 踊り技を使用したポケモンが味方で、味方が選んだターゲットが自分なら、味方をターゲットにする
 * 踊り技を使用したポケモンが味方で、味方が選んだターゲットが自分でないなら、同じ対象をターゲットにする
 */
BtlPokePos odoriko_GetTargetPos( ServerFlow* flowWk, u8 odorikoPokeID, u8 attackPokeID )
{
  // 踊り技を使用したポケモンが相手の場合
  if( !flowWk->Hnd_GetMainModule()->IsFriendPokeID( odorikoPokeID, attackPokeID ) )
  {
    return flowWk->Hnd_GetPokeLastPos( attackPokeID );
  }

  // 踊り技を使用したポケモンが味方の場合
  const BTL_POKEPARAM* attackPoke = flowWk->Hnd_GetPokeParam( attackPokeID );
  BtlPokePos odorikoPos = flowWk->Hnd_GetExistFrontPokePos( odorikoPokeID );
  BtlPokePos attackerTargetPos = attackPoke->GetPrevTargetPos();
  BtlPokePos targetPos;
  if( odorikoPos == attackerTargetPos )
  {
    targetPos = flowWk->Hnd_GetPokeLastPos( attackPokeID );
  }
  else
  {
    targetPos = attackerTargetPos;
  }
  return targetPos;
}

//------------------------------------------------------------------------------
/**
 *  とくせい「ふしょく」
 *
 * はがねタイプ、どくタイプのポケモンもどくにすることができる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_TOK_ADD_Husyoku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = 
  {
    { BTL_EVENT_CHECK_ADDSICK_FAIL_STD_SKIP,  handler_Husyoku_CheckAddSickFailStdSkip }, // 基本ルールによる状態異常の失敗チェックをスキップするか？
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// 基本ルールによる状態異常の失敗チェックをスキップするか？
void handler_Husyoku_CheckAddSickFailStdSkip( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  const u8 attackPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
  const WazaSick sick = static_cast<WazaSick>( EVENTVAR_GetValue( BTL_EVAR_SICKID ) );

  if( ( attackPokeID == pokeID ) &&
      ( sick == pml::wazadata::WAZASICK_DOKU ) )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「エレキメイカー」
 *
 * 場に出たときに地形を「エレキフィールド」にする
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_ElecMaker( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,            handler_ElecMaker_MemberIn },
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_ElecMaker_MemberIn },
  };

  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// ポケモンが出場した時のハンドラ
void handler_ElecMaker_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( myHandle );
  GFL_UNUSED( work );
  handler_common_GroundMaker( flowWk, pokeID, BTL_GROUND_ELEKI, BTL_STRID_STD_ElecField_Start );
}

//------------------------------------------------------------------------------
/**
 *  とくせい「サイコメイカー」
 *
 * 場に出たときに地形を「サイコフィールド」にする
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_PhychoMaker( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,            handler_PhychoMaker_MemberIn },
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_PhychoMaker_MemberIn },
  };

  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// ポケモンが出場した時のハンドラ
void handler_PhychoMaker_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( myHandle );
  GFL_UNUSED( work );
  handler_common_GroundMaker( flowWk, pokeID, BTL_GROUND_PHYCHO, BTL_STRID_STD_PhychoField_Start );
}

//------------------------------------------------------------------------------
/**
 *  とくせい「ミストメイカー」
 *
 * 場に出たときに地形を「ミストフィールド」にする
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_MistMaker( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,            handler_MistMaker_MemberIn },
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_MistMaker_MemberIn },
  };

  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// ポケモンが出場した時のハンドラ
void handler_MistMaker_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( myHandle );
  GFL_UNUSED( work );
  handler_common_GroundMaker( flowWk, pokeID, BTL_GROUND_MIST, BTL_STRID_STD_MistGround_Start );
}

//------------------------------------------------------------------------------
/**
 *  とくせい「グラスメイカー」
 *
 * 場に出たときに地形を「グラスフィールド」にする
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_GrassMaker( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,            handler_GrassMaker_MemberIn },
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_GrassMaker_MemberIn },
  };

  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// ポケモンが出場した時のハンドラ
void handler_GrassMaker_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( myHandle );
  GFL_UNUSED( work );
  handler_common_GroundMaker( flowWk, pokeID, BTL_GROUND_GRASS, BTL_STRID_STD_GrassGround_Start );
}

/**
 * @brief 「○○メイカー」共通処理：地形効果を発生させる
 */
void handler_common_GroundMaker( ServerFlow* flowWk, u8 pokeID, BtlGround ground, u16 strID )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) != pokeID )
  {
    return;
  }

  if( flowWk->Hnd_GetGround() != ground )
  {
    BTL_HANDEX_PARAM_ADD_FLDEFF* param;

    param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_FLDEFF*>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_FLDEFF, pokeID ) );
      param->header.tokwin_flag = true;
      param->effect             = FieldStatus::EFF_GROUND;
      param->ground             = ground;
      param->cont               = SICKCONT_MakeTurn( pokeID, 5 );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, strID );
    flowWk->HANDEX_Pop( param );
  }
}

//------------------------------------------------------------------------------
/**
 *  とくせい「はりこみ」
 *
 * 交代で出てきたポケモンに倍のダメージで攻撃できる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Harikomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = 
  {
    { BTL_EVENT_ATTACKER_POWER, handler_Harikomi },  // 攻撃側能力の計算ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// 攻撃側能力の計算ハンドラ
void handler_Harikomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK ) != pokeID )
  {
    return;
  }

  const u8 defensePokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  const BTL_POKEPARAM* defensePoke = flowWk->Hnd_GetPokeParam( defensePokeID );
  if( defensePoke->TURNFLAG_Get( BTL_POKEPARAM::TURNFLG_APPEARED_BY_POKECHANGE ) )
  {
    EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2.0f) );
  }
}

//----------------------------------------------------------------------------------
/**
 * とくせい「きずなへんげ」
 *
 * サトシゲッコウガ専用
 * ポケモンを１匹倒すとフォルムチェンジする。
 * フォルムチェンジ後は、技「みずしゅりけん」の威力が20になる
 * フォルムチェンジ後は、技「みずしゅりけん」のヒット回数が3固定になる
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_KizunaHenge( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_L2,  handler_KizunaHenge_DamageProcEndHitL2 },  // ダメージ反応ハンドラ
    { BTL_EVENT_WAZA_HIT_COUNT,         handler_KizunaHenge_HitCount           },  // 攻撃回数決定のハンドラ
    { BTL_EVENT_WAZA_POWER_BASE,        handler_KizunaHenge_WazaPowerBase      },  // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// ダメージ反応ハンドラ
void handler_KizunaHenge_DamageProcEndHitL2( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK ) != pokeID )
  {
    return;
  }

  // 決着が着いていたらフォルムチェンジしない
  if( flowWk->Hnd_CheckShowDown() )
  {
    return;
  }

  // サトシゲッコウガ( フォルム変化前 )専用
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  if( ( bpp->GetMonsNo() != MONSNO_GEKKOUGA ) ||
      ( bpp->GetFormNo() != FORMNO_GEKKOUGA_GEKKOUGA2 ) )
  {
    return;
  }

  // 既に1度フォルムチェンジしている
  if( bpp->PERMFLAG_Get( BTL_POKEPARAM::PERMFLAG_KIZUNAHENGE_DONE ) )
  {
    return;
  }

  const u32 killCount = HandCommon_GetKillCount( myHandle, flowWk, pokeID, work );
  if( 0 < killCount )
  {
    //「○○に　きずなの　ちからが　みなぎった！」
    {
      BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE*>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ) );
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Kizunahenge );
        HANDEX_STR_AddArg( &param->str, pokeID );
      flowWk->HANDEX_Pop( param );
    }

    // フォルムチェンジ
    {
      BTL_HANDEX_PARAM_CHANGE_FORM* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_FORM *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_FORM, pokeID ) );
        param->header.tokwin_flag = true;
        param->pokeID = pokeID;
        param->formNo = FORMNO_GEKKOUGA_SATOSHI;
        param->fDontResetFormByOut = true;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Kizunahenge2 );  //「○○は　サトシゲッコウガに へんげした！」
        HANDEX_STR_AddArg( &param->exStr, pokeID );
      flowWk->HANDEX_Pop( param );
    }

    // 永続フラグセット
    {
      BTL_HANDEX_PARAM_SET_PERMFLAG* param = reinterpret_cast<BTL_HANDEX_PARAM_SET_PERMFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_PERMFLAG, pokeID ) );
        param->pokeID = pokeID;
        param->flag = BTL_POKEPARAM::PERMFLAG_KIZUNAHENGE_DONE;
      flowWk->HANDEX_Pop( param );
    }
  }
}

// 攻撃回数決定のハンドラ
void handler_KizunaHenge_HitCount( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);

  // サトシゲッコウガ( フォルム変化後 )専用
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  if( ( bpp->GetMonsNo() != MONSNO_GEKKOUGA ) ||
      ( bpp->GetFormNo() != FORMNO_GEKKOUGA_SATOSHI ) )
  {
    return;
  }

  // 攻撃側が自分の時、「みずしゅりけん」のヒット回数を3に固定
  if( ( EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK ) == pokeID ) &&
      ( EVENTVAR_GetValue( BTL_EVAR_WAZAID ) == WAZANO_MIZUSYURIKEN ) )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_HITCOUNT, 3 );
  }
}

// ワザ威力チェックハンドラ
void handler_KizunaHenge_WazaPowerBase( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK ) != pokeID )
  {
    return;
  }

  // サトシゲッコウガ( フォルム変化後 )専用
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  if( ( bpp->GetMonsNo() != MONSNO_GEKKOUGA ) ||
      ( bpp->GetFormNo() != FORMNO_GEKKOUGA_SATOSHI ) )
  {
    return;
  }

  // 「みずしゅりけん」の威力が20になる
  if( EVENTVAR_GetValue( BTL_EVAR_WAZAID ) == WAZANO_MIZUSYURIKEN )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, 20 );
    //みずしゅりけん自体が連続技で、攻撃回数のでEffectIndexを使ってしまっているため、演出自体の分岐はView側で行う
    //flowWk->Hnd_SetWazaEffectIndex( BTLV_WAZAEFF_MIZUSYURIKEN_SATOSHI );
  }
}

//----------------------------------------------------------------------------------
/**
 * ばけのかわ
 *
 * ホラー専用
 * １回だけ敵からのダメージを無傷で受けることができる
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_Bakenokawa( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ICHIGEKI_GUARD,        handler_Bakenokawa_IchigekiGuard      },  // みがわりチェック後の一撃必殺技無効化チェック
    { BTL_EVENT_WAZA_DMG_PROC2,        handler_Bakenokawa_WazaDamageProc     },  // ダメージ計算最終ハンドラ
    { BTL_EVENT_WAZA_DMG_REACTION_L3,  handler_Bakenokawa_WazaDamageReaction },  // ダメージ直後ハンドラ
    { BTL_EVENT_FIX_CONF_DAMAGE,       handler_Bakenokawa_FixConfDamage      },  // こんらんダメージ補正
    { BTL_EVENT_CONF_DAMAGE_REACTION,  handler_Bakenokawa_ConfDamageReaction },  // こんらんダメージ後リアクション
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// みがわりチェック後の一撃必殺技無効化チェック
void handler_Bakenokawa_IchigekiGuard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) != pokeID )
  {
    return;
  }

  if( !bakenokawa_CheckDamageGuard( flowWk, pokeID, work ) )
  {
    return;
  }

  // 一撃必殺技を無効化
  {
    EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
    work[0] = 1;
  }
}

// ダメージ計算最終ハンドラ
void handler_Bakenokawa_WazaDamageProc( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  if( ( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) != pokeID ) ||
      ( (bool)EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == true ) )
  {
    return;
  }

  if( !bakenokawa_CheckDamageGuard( flowWk, pokeID, work ) )
  {
    return;
  }

  // ダメージを無効化
  {
    EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE, 0 );
    EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE_FLAG, true );
    work[0] = 1;
  }
}

// ダメージ直後ハンドラ
void handler_Bakenokawa_WazaDamageReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  if( ( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) != pokeID ) ||
      ( (bool)EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == true ) )
  {
    return;
  }

  bakenokawa_ChangeForm( flowWk, pokeID, work );
}

// こんらんダメージ補正
void handler_Bakenokawa_FixConfDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) != pokeID )
  {
    return;
  }

  if( !bakenokawa_CheckDamageGuard( flowWk, pokeID, work ) )
  {
    return;
  }

  // ダメージを無効化
  EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE, 0 );
  EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE_FLAG, true );
  work[0] = 1;
}

// こんらんダメージ後リアクション
void handler_Bakenokawa_ConfDamageReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) != pokeID )
  {
    return;
  }

  bakenokawa_ChangeForm( flowWk, pokeID, work );
}

/**
 * @brief 「ばけのかわ」によりダメージを無効化するか？
 * @retval true   無効化する
 * @retval false  無効化しない
 */
bool bakenokawa_CheckDamageGuard( ServerFlow* flowWk, u8 pokeID, int* work )
{
  // シミュレーションでは、ダメージを無効化しない
  if( flowWk->Hnd_IsSimulationMode() )
  {
    return false;
  }

  // ホラー( フォルム変化前 )専用
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  if( !bakenokawa_CheckEnable( *bpp ) )
  {
    return false;
  }

  // 既に無効化している
  if( work[0] != 0 )
  {
    return false;
  }

  return true;
}

/**
 * @brief 「ばけのかわ」によるフォルムチェンジ
 */
void bakenokawa_ChangeForm( ServerFlow* flowWk, u8 pokeID, int* work )
{
  // ホラー( フォルム変化前 )専用
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  if( !bakenokawa_CheckEnable( *bpp ) )
  {
    return;
  }

  // フォルムチェンジ
  if( work[0] == 1 )
  {
    work[0] = 0;

    FormNo nextFormNo = ( bpp->GetFormNo() == FORMNO_HORAA_NORMAL ) ? ( FORMNO_HORAA_KUBIORE ) : ( FORMNO_HORAA_KUBIORE_NUSHI );

    flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );

    //「ばけのかわが　みがわりに　なった！」
    {
      BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE*>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ) );
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_Bakenokawa );
      flowWk->HANDEX_Pop( param );
    }

    {
      BTL_HANDEX_PARAM_CHANGE_FORM* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_FORM *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_FORM, pokeID ) );
        param->pokeID              = pokeID;
        param->formNo              = nextFormNo;
        param->fDontResetFormByOut = true;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Bakenokawa );  //「ばけのかわが　はがれた！」
        HANDEX_STR_AddArg( &param->exStr, pokeID );
      flowWk->HANDEX_Pop( param );
    }

    flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
  }
}

/**
 * @brief 「ばけのかわ」が発動可能かどうかチェックする
 * @param poke  チェック対象のポケモン
 * @retval true   発動できる
 * @retval false  発動できない
 */
bool bakenokawa_CheckEnable( const BTL_POKEPARAM& poke )
{
  if( poke.GetMonsNo() != MONSNO_HORAA )
  {
    return false;
  }

  if( ( poke.GetFormNo() != FORMNO_HORAA_NORMAL ) &&
      ( poke.GetFormNo() != FORMNO_HORAA_NORMAL_NUSHI ) )
  {
    return false;
  }

  //「へんしん」したポケモンは、変身後の特性によりフォルムチェンジを行わないため、
  //「ミミッキュ」が「ミミッキュ」に「へんしん」した場合、
  // 常に「ぬのかぶり」フォルムを維持するため、何度でもダメージを無効化か出来てしまいます。
  // そのため、「ミミッキュ」に変身したポケモンには、「ばけのかわ」が発動しないようにします。
  if( poke.HENSIN_Check() )
  {
    return false;
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 *  とくせい「ぜったいねむり」
 *
 * 常にねむり状態で覚めることがない。眠ったまま技が出せる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_TOK_ADD_ZettaiNemuri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = 
  {
    { BTL_EVENT_MEMBER_IN,            handler_ZettaiNemuri_MemberIn          }, // 入場ハンドラ
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_ZettaiNemuri_MemberIn          }, // とくせい書き換えハンドラ
    { BTL_EVENT_ADDSICK_CHECKFAIL,    handler_ZettaiNemuri_AddSickCheckFail  }, // ポケモン系状態異常処理ハンドラ
    { BTL_EVENT_CHECK_INEMURI,        handler_Fumin_InemuriCheck             }, // いねむりチェック
    { BTL_EVENT_ADDSICK_FAILED,       handler_AddSickFailCommon              }, // 状態異常失敗

  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// メンバー入場ハンドラ
void handler_ZettaiNemuri_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) != pokeID )
  {
    return;
  }

  //「ネッコアラは　ゆめうつつの　じょうたい！」
  {
    flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_IN, pokeID );

    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE*>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ) );
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_ZettaiNemuri );
      HANDEX_STR_AddArg( &param->str, pokeID );
    flowWk->HANDEX_Pop( param );

    flowWk->HANDEX_PushRun( BTL_HANDEX_TOKWIN_OUT, pokeID );
  }
}

// ポケモン系状態異常処理ハンドラ
void handler_ZettaiNemuri_AddSickCheckFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  static const WazaSick GUARD_SICK[] = 
  {
    pml::wazadata::WAZASICK_MAHI,
    pml::wazadata::WAZASICK_NEMURI,
    pml::wazadata::WAZASICK_KOORI,
    pml::wazadata::WAZASICK_YAKEDO,
    pml::wazadata::WAZASICK_DOKU,
    pml::wazadata::WAZASICK_AKUBI,
  };

  for( u32 i=0; i<GFL_NELEMS(GUARD_SICK); ++i )
  {
    work[WIDX0] = HandCommon_GuardWazaSick( flowWk, pokeID, GUARD_SICK[i] );
    if( work[WIDX0] )
    {
      return;
    }
  }
}

//------------------------------------------------------------------------------
/**
*  ブレインプリズム
*  効果抜群でダメージを与えた場合、与えるダメージが１．２５倍になります。
*/
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_TOK_ADD_BrainPrism(u32* numElems)
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3, handler_BrainPrism },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
void handler_BrainPrism(BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work)
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  {
    // 効果抜群
    if (EVENTVAR_GetValue(BTL_EVAR_TYPEAFF) > pml::battle::TypeAffinity::TYPEAFF_1)
    {
      EVENTVAR_MulValue(BTL_EVAR_RATIO, FX32_CONST(1.25));
    }
  }
}



} // end of namespace 'tokusei'
} // end of namespace 'handler'
GFL_NAMESPACE_END(btl)

#if defined(GF_PLATFORM_WIN32)
#pragma warning( default  : 4800 )
#endif


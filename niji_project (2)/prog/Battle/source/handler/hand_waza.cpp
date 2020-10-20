//=============================================================================================
/**
 * @file  hand_waza.cpp
 * @brief ポケモンXY バトルシステム イベントファクター [ワザ]
 * @author  taya
 *
 * @date  2011.01.20  作成
 */
//=============================================================================================
#include <util/include/gfl2_std_string.h>

#include "../btl_PokeParam.h"
#include "../btl_Field.h"
#include "../btl_tables.h"
#include "../btl_ActionRecorder.h"
#include "../btlv/btlv_core.h"
#include "./hand_common.h"
#include "./hand_waza.h"
#include "./hand_side.h"


GFL_NAMESPACE_BEGIN(btl)
namespace handler {
namespace waza    {


/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  WORKIDX_STICK = EVENT_HANDLER_WORK_ELEMS - 1,
};

/**
 *  合体ワザ効果
 */
typedef enum {

  COMBI_EFFECT_NULL = 0,
  COMBI_EFFECT_RAINBOW,   ///< 虹      = 炎・水
  COMBI_EFFECT_BURNING,   ///< 火の海  = 草・炎
  COMBI_EFFECT_MOOR,      ///< 沼      = 水・草

}CombiEffectType;

const BtlEventHandlerTable*  ADD_Texture( u32* numElems );
void handler_Texture( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
bool is_registable( u8 pokeID, WazaID waza, u8* fRegistered );
void removeHandlerForce( u8 pokeID, WazaID waza );


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
bool common_checkActStart_isMyWaza( ServerFlow* flwk, u8 pokeID );
const BtlEventHandlerTable*  ADD_Hogosyoku( u32* numElems );
void handler_Hogosyoku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_TrickRoom( u32* numElems );
void handler_TrickRoom( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Juryoku( u32* numElems );
void handler_Juryoku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_MizuAsobi( u32* numElems );
void handler_MizuAsobi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_DoroAsobi( u32* numElems );
void handler_DoroAsobi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Kiribarai( u32* numElems );
void handler_Kiribarai( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Kawarawari( u32* numElems );
void handler_Kawarawari_DmgProc1( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Kawarawari_DmgProcEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Kawarawari_DmgDetermine( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
bool handler_Kawarawari_SkipCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, BtlEventFactorType factorType, BtlEventType eventType, u16 subID, u8 dependID );
const BtlEventHandlerTable*  ADD_Tobigeri( u32* numElems );
void handler_Tobigeri_Avoid( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tobigeri_NoEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void  common_TobigeriReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Monomane( u32* numElems );
void handler_Monomane( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Sketch( u32* numElems );
void handler_Sketch( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_KonoyubiTomare( u32* numElems );
void handler_KonoyubiTomare_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KonoyubiTomare_Exe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KonoyubiTomare_TemptTarget( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KonoyubiTomare_TurnCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Ikarinokona( u32* numElems );
void handler_Ikarinokona_TemptTarget( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Refresh( u32* numElems );
void handler_Refresh( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_KumoNoSu( u32* numElems );
void handler_KumoNoSu_NoEffCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KumoNoSu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_KuroiKiri( u32* numElems );
void handler_KuroiKiri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Haneru( u32* numElems );
void handler_Haneru( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Haneru_CheckFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Noroi( u32* numElems );
void handler_Noroi_GetZenryokuEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Noroi_WazaParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Noroi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Denjiha( u32* numElems );
void handler_Denjiha( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_NayamiNoTane( u32* numElems );
void handler_NayamiNoTane_NoEff( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_NayamiNoTane( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Yumekui( u32* numElems );
void handler_Yumekui( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Yumekui_MigawariCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Yuwaku( u32* numElems );
void handler_Yuwaku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_TriAttack( u32* numElems );
void handler_TriAttack( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_HimituNoTikara( u32* numElems );
WazaNo common_HimituNoTikara_getParams( ServerFlow* flowWk, u8* dstMethod, u8* dstMethodArg );
void handler_HimituNoTikara_Start( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_HimituNoTikara( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Osyaberi( u32* numElems );
void handler_Osyaberi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Makituku( u32* numElems );
void handler_Makituku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Makituku_Str( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Uzusio( u32* numElems );
void handler_Uzusio_CheckHide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Uzusio_Dmg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_IkariNoMaeba( u32* numElems );
void handler_IkariNoMaeba( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_GuardianDeAlola( u32* numElems );
void handler_GuardianDeAlola( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Common_WazaDamageProc1_FixDamageByDefenderHp( ServerFlow* flowWk, u8 numerator, u8 denominator );
const BtlEventHandlerTable*  ADD_RyuuNoIkari( u32* numElems );
void handler_RyuuNoIkari( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_SonicBoom( u32* numElems );
void handler_SonicBoom( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Gamusyara( u32* numElems );
void handler_Gamusyara_CheckNoEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Gamusyara( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_TikyuuNage( u32* numElems );
void handler_TikyuuNage_CalcDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_TikyuuNage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_PsycoWave( u32* numElems );
void handler_PsycoWave( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Takuwaeru( u32* numElems );
void handler_Takuwaeru_CheckExe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Takuwaeru( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Hakidasu( u32* numElems );
void handler_Hakidasu_CheckExe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Hakidasu_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Hakidasu_Done( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Nomikomu( u32* numElems );
void handler_Nomikomu_Ratio( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Counter( u32* numElems );
void handler_Counter_CheckExe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Counter_Target( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Counter_CalcDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_MilerCoat( u32* numElems );
void handler_MilerCoat_CheckExe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MilerCoat_Target( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MilerCoat_CalcDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_MetalBurst( u32* numElems );
void handler_MetalBurst_CheckExe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MetalBurst_Target( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MetalBurst_CalcDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void common_Counter_ExeCheck( ServerFlow* flowWk, u8 pokeID, int* work, WazaDamageType dmgType );
void common_Counter_SetTarget( ServerFlow* flowWk, u8 pokeID, int* work, WazaDamageType dmgType );
void common_Counter_CalcDamage( ServerFlow* flowWk, u8 pokeID, int* work, WazaDamageType dmgType, fx32 ratio );
bool common_Counter_GetRec( ServerFlow* flowWk, const BTL_POKEPARAM* bpp, WazaDamageType dmgType, BTL_POKEPARAM::WAZADMG_REC* rec );
const BtlEventHandlerTable*  ADD_Totteoki( u32* numElems );
void handler_Totteoki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Ibiki( u32* numElems );
void handler_Ibiki_CheckFail_1( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Ibiki_CheckFail_2( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Akumu( u32* numElems );
void handler_Akumu_NoEff( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Fuiuti( u32* numElems );
void handler_Fuiuti_NoEff( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
bool handler_Fuiuti_CheckSuccess( ServerFlow* flowWk, u8 targetPokeID );
const BtlEventHandlerTable*  ADD_Oiuti( u32* numElems );
void handler_Oiuti_Intr( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Oiuti_HitCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Oiuti_Dmg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Daibakuhatsu( u32* numElems );
void handler_Daibakuhatsu_ExeStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Daibakuhatsu_DmgDetermine( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Daibakuhatsu_ExeFix( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Kiaidame( u32* numElems );
void handler_Kiaidame( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Juden( u32* numElems );
void handler_Juden_Exe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Juden_RemoveAllTarget( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Juden_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Juden_WazaStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Juden_WazaEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_HorobiNoUta( u32* numElems );
void handler_HorobiNoUta_Exe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Common_MigawariEffctive( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_YadorigiNoTane( u32* numElems );
void handler_YadorigiNoTane_Param( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_NekoNiKoban( u32* numElems );
void handler_NekoNiKoban( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Ikari( u32* numElems );
void handler_Ikari_Exe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Ikari_React( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Ikari_Release( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_AquaRing( u32* numElems );
void handler_AquaRing( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Abareru( u32* numElems );
void handler_Abareru( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Abareru_SeqEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Abareru_turnCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Sawagu( u32* numElems );
void handler_Sawagu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Sawagu_SeqEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Sawagu_turnCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Sawagu_CheckSickFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Sawagu_CheckInemuri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void Sawagu_CureLock( ServerFlow* flowWk, u8 pokeID );
const BtlEventHandlerTable*  ADD_Korogaru( u32* numElems );
void handler_Korogaru_ExeFix( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Korugaru_Avoid( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Korogaru_NoEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Korogaru_SeqEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void common_Korogaru_Unlock( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Korogaru_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_TripleKick( u32* numElems );
void handler_TripleKick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_TripleKick_HitCount( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_GyroBall( u32* numElems );
void handler_GyroBall( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Revenge( u32* numElems );
void handler_Revenge( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Jitabata( u32* numElems );
void handler_Jitabata( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Karagenki( u32* numElems );
void handler_Karagenki_AtkPow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Karagenki_WazaPow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Sippegaesi( u32* numElems );
void handler_Sippegaesi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Funka( u32* numElems );
void handler_Funka( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Siboritoru( u32* numElems );
void handler_Siboritoru( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Siomizu( u32* numElems );
void handler_Siomizu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Ongaesi( u32* numElems );
void handler_Ongaesi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Yatuatari( u32* numElems );
void handler_Yatuatari( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_MezamasiBinta( u32* numElems );
void handler_MezamasiBinta( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MezamasiBinta_AfterDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Kituke( u32* numElems );
void handler_Kituke( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Kituke_AfterDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Present( u32* numElems );
void handler_Present_Check( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Present_Fix( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Present_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Kirifuda( u32* numElems );
void handler_Kirifuda( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Osioki( u32* numElems );
void handler_Osioki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_RenzokuGiri( u32* numElems );
void handler_RenzokuGiri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Dameosi( u32* numElems );
void handler_Dameosi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Ketaguri( u32* numElems );
void handler_Ketaguri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_WeatherBall( u32* numElems );
void handler_WeatherBall_Type( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_WeatherBall_GetZenryokuWaza( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_WeatherBall_AfterWazaParamFixed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_WeatherBall_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Tatumaki( u32* numElems );
void handler_Tatumaki_checkHide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tatumaki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_SkyUpper( u32* numElems );
void handler_SkyUpper( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Kaminari( u32* numElems );
void handler_Kaminari_checkHide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Kaminari_excuseHitCalc( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Kaminari_hitRatio( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Fubuki( u32* numElems );
void handler_Fubuki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_ZettaiReido( u32* numElems );
void handler_ZettaiReido_hitRatio( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Jisin( u32* numElems );
void handler_Jisin_checkHide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Jisin_damage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_SabakiNoTubute( u32* numElems );
void handler_SabakiNoTubute( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_MultiAttack( u32* numElems );
void handler_MultiAttack( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_TechnoBaster( u32* numElems );
void handler_TechnoBaster( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_MezameruPower( u32* numElems );
void handler_MezameruPower_Type( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MezameruPower_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_SizenNoMegumi( u32* numElems );
void handler_SizenNoMegumi_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SizenNoMegumi_Type( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SizenNoMegumi_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SizenNoMegumi_CheckAffine( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SizenNoMegumi_Done( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
bool SizenNoMegumi_CheckSuccess( ServerFlow* flowWk, u8 pokeID );
const BtlEventHandlerTable*  ADD_Hatakiotosu( u32* numElems );
void handler_Hatakiotosu_WazaPow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Hatakiotosu_EndHit( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_MagicCoat( u32* numElems );
void handler_MagicCoat_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MagicCoat( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MagicCoat_Wait( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MagicCoat_Reflect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MagicCoat_TurnCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Yokodori( u32* numElems );
void handler_Yokodori( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Yokodori_MagicMirror( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Yokodori_CheckRob( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Yokodori_Rob( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Yokodori_TurnCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Dorobou( u32* numElems );
void handler_Dorobou_Start( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Dorobou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Trick( u32* numElems );
void handler_Trick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Magnitude( u32* numElems );
void handler_Magnitude_effect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Magnitude_pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Naminori( u32* numElems );
void handler_Naminori_checkHide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Naminori( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Fumituke( u32* numElems );
void handler_Fumituke_DamegeProc( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Fumituke_HitCheckSkip( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Mineuti( u32* numElems );
void handler_Mineuti( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Koraeru( u32* numElems );
void handler_Koraeru_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Koraeru( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Koraeru_Check( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Koraeru_TurnCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Mamoru( u32* numElems );
void handler_Mamoru_StartSeq( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Mamoru_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Mamoru_ExeFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Mamoru( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Mamoru_Core( ServerFlow* flowWk, u8 pokeID );
void handler_Mamoru_Off( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Mamoru_DmgZ( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Mamoru_MsgAfterCritical( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
bool mamoru_DmgZ( ServerFlow* flowWk, u8 pokeID, int* work );
bool mamoru_MsgAfterCritical( ServerFlow* flowWk, u8 pokeID, int* work );
void IncrementMamoruCounter( ServerFlow* flowWk, u8 pokeID, bool fFailSkip );
const BtlEventHandlerTable*  ADD_Gaman( u32* numElems );
void handler_Gaman( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Gaman_WazaMsg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Gaman_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Gaman_Target( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Gaman_DmgRec( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Gaman_CalcDmg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Gaman_Fail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
u8 gaman_getTargetPokeID( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void gaman_ReleaseStick( ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Recycle( u32* numElems );
void handler_Recycle( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_PsycoShift( u32* numElems );
void handler_PsycoShift( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Itamiwake( u32* numElems );
void handler_Itamiwake( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Haradaiko( u32* numElems );
void handler_Haradaiko( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Feint( u32* numElems );
const BtlEventHandlerTable*  ADD_IjigenHall( u32* numElems );
void handler_Feint_MamoruBreak( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Feint_AfterDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_IjigenHall_AfterDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void common_feint_proc( ServerFlow* flowWk, u8 pokeID, u16 strID );
void common_mamoruBreakAfter( ServerFlow* flowWk, u8 attackPokeID, const BTL_POKEPARAM* target, u16 strID );
void common_mamoruBreak_RemoveSideEff( ServerFlow* flowWk, u8 pokeID, const BTL_POKEPARAM* target );
bool common_IsExistGuardTypeSideEffect( ServerFlow* flowWk, u8 targetPokeID, bool bIncludeNotCountupType );
void handler_Feint_NoEffCheckBegin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Feint_NoEffCheckEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void SleepGuardSideEffect( ServerFlow* flowWk, u8 attackPokeId, u8 targetPokeId, bool wakeFlag );
const BtlEventHandlerTable*  ADD_TuboWoTuku( u32* numElems );
void handler_TuboWoTuku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Nemuru( u32* numElems );
void handler_Nemuru_exeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Nemuru( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Meromero( u32* numElems );
void handler_Meromero_CheckNoEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Texture2( u32* numElems );
void handler_Texture2( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Encore( u32* numElems );
void handler_Encore( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Chouhatu( u32* numElems );
void handler_Chouhatu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Ichamon( u32* numElems );
void handler_Ichamon( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Kanasibari( u32* numElems );
void handler_Kanasibari( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Fuuin( u32* numElems );
void handler_Fuuin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Alomatherapy( u32* numElems );
void handler_Alomatherapy_Exe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Alomatherapy( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_IyasiNoSuzu( u32* numElems );
void handler_IyasiNoSuzu_Exe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_IyasiNoSuzu_RemoveAllTarget( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_IyasiNoSuzu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void common_CureFriendPokeSick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Okimiyage( u32* numElems );
void handler_Okimiyage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Urami( u32* numElems );
void handler_Urami( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_JikoAnji( u32* numElems );
void handler_JikoAnji( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_HeartSwap( u32* numElems );
void handler_HeartSwap( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_PowerSwap( u32* numElems );
void handler_PowerSwap( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_GuardSwap( u32* numElems );
void handler_GuardSwap( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_PowerTrick( u32* numElems );
void handler_PowerTrick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_PowerShare( u32* numElems );
void handler_PowerShare( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_GuardShare( u32* numElems );
void handler_GuardShare( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_LockON( u32* numElems );
void handler_LockON( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Dokudoku( u32* numElems );
void handler_Dokudoku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Refrector( u32* numElems );
void handler_Refrector( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_HikariNoKabe( u32* numElems );
void handler_HikariNoKabe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_SinpiNoMamori( u32* numElems );
void handler_SinpiNoMamori( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_SiroiKiri( u32* numElems );
void handler_SiroiKiri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Oikaze( u32* numElems );
void handler_Oikaze( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Omajinai( u32* numElems );
void handler_Omajinai( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Makibisi( u32* numElems );
void handler_Makibisi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Dokubisi( u32* numElems );
void handler_Dokubisi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_StealthRock( u32* numElems );
void handler_StealthRock( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_WideGuard( u32* numElems );
void handler_WideGuard_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_WideGuard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_TatamiGaeshi( u32* numElems );
void handler_TatamiGaeshi_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_TatamiGaeshi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void common_SideEffectStdMsg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work,
  BtlSide side, BtlSideEffect effect, const BTL_SICKCONT& cont, u16 strID );
void common_SideEffectSetMsg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work,
  BtlSide side, BtlSideEffect effect, const BTL_SICKCONT& cont, u16 strID );
void common_SideEffectCore( ServerFlow* flowWk, u8 pokeID, BtlSide side, BtlSideEffect effect,
  const BTL_SICKCONT& cont, BtlStrType strType, u32 strID, int strArg, bool replaceStrArg0ByExpandSide );
const BtlEventHandlerTable*  ADD_Hensin( u32* numElems );
void handler_Hensin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_MikadukiNoMai( u32* numElems );
void handler_MikadukiNoMai( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_IyasiNoNegai( u32* numElems );
void handler_IyasiNoNegai( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Negaigoto( u32* numElems );
void handler_Negaigoto( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Miraiyoti( u32* numElems );
void handler_Miraiyoti( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Miraiyoti_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void common_delayAttack( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, u8 targetPos );
const BtlEventHandlerTable*  ADD_HametuNoNegai( u32* numElems );
void handler_HametuNoNegai( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_HametuNoNegai_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Ieki( u32* numElems );
void handler_Ieki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Narikiri( u32* numElems );
void handler_Narikiri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_TonboGaeri( u32* numElems );
void handler_TonboGaeri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_KousokuSpin( u32* numElems );
void handler_KousokuSpin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_BatonTouch( u32* numElems );
void handler_BatonTouch( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Teleport( u32* numElems );
void handler_Teleport_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Teleport( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Teleport_ExMsg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Nagetukeru( u32* numElems );
void handler_Nagetukeru_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Nagetukeru_WazaPower( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Nagetukeru_DmgProcStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Nagetukeru_DmgAfter( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Nagetukeru_Done( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_DenjiFuyuu( u32* numElems );
void handler_DenjiFuyuu_CheckFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_DenjiFuyuu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Tedasuke( u32* numElems );
void handler_Tedasuke_SkipAvoid( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tedasuke_CheckHide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tedasuke_Ready( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tedasuke_WazaPow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tedasuke_TurnCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
bool tedasuke_IsSuccess( ServerFlow* flowWk, u8 pokeID );
const BtlEventHandlerTable*  ADD_FukuroDataki( u32* numElems );
void handler_FukuroDataki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_FukuroDataki_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BTL_POKEPARAM* common_FukuroDataki_GetParam( ServerFlow* flowWk, u8 myPokeID, u8 idx );
const BtlEventHandlerTable*  ADD_Nekodamasi( u32* numElems );
void handler_Nekodamasi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Deaigasira( u32* numElems );
void handler_Deaigasira( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_AsaNoHizasi( u32* numElems );
void handler_AsaNoHizasi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Sunaatume( u32* numElems );
void handler_Sunaatume( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_FlowerHeal( u32* numElems );
void handler_FlowerHeal( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_SoraWoTobu( u32* numElems );
void handler_SoraWoTobu_TameStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SoraWoTobu_TameRelease( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_ShadowDive( u32* numElems );
void handler_ShadowDive_TameStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_ShadowDive_TameRelease( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_ShadowDive_DmgStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_ShadowDive_AfterDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Tobihaneru( u32* numElems );
void handler_Tobihaneru_TameStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tobihaneru_TameRelease( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Diving( u32* numElems );
void handler_Diving_TameStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Diving_TameRelease( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_AnaWoHoru( u32* numElems );
void handler_AnaWoHoru_TameStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_AnaWoHoru_TameRelease( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_SolarBeam( u32* numElems );
void handler_SolarBeam_TameSkip( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SolarBeam_TameStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SolarBeam_Power( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Kamaitati( u32* numElems );
void handler_Kamaitati_TameStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_GodBird( u32* numElems );
void handler_GodBird_TameStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_RocketZutuki( u32* numElems );
void handler_RocketZutuki_TameStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* ADD_Tuibamu( u32* numElems );
void handler_Tuibamu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* ADD_Waruagaki( u32* numElems );
void handler_Waruagaki_KickBack( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Waruagaki_SeqStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Waruagaki_WazaParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Waruagaki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Michidure( u32* numElems );
void handler_Michidure_CheckFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Michidure_Ready( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Michidure_ActStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Michidure_WazaDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Onnen( u32* numElems );
void handler_Onnen_Ready( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Onnen_WazaDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Onnen_ActStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Tiisakunaru( u32* numElems );
void handler_Tiisakunaru( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Marukunaru( u32* numElems );
void handler_Marukunaru( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Haneyasume( u32* numElems );
void handler_Haneyasume( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_KiaiPunch( u32* numElems );
void handler_KiaiPunch( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_YubiWoFuru( u32* numElems );
void handler_YubiWoFuru( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_YubiWoFuru_Msg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_YubiwoFuru_ZenryokuReqWazaMsg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_SizenNoTikara( u32* numElems );
void handler_SizenNoTikara( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SizenNoTikara_Msg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Sizennotikara_ZenryokuReqWazaMsg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Nekonote( u32* numElems );
void handler_Nekonote( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Nekonote_CheckParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Nekonote_ZenryokuReqWazaMsg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Negoto( u32* numElems );
void handler_Negoto( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Negoto_CheckParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Negoto_ZenryokuReqWazaMsg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_OumuGaesi( u32* numElems );
void handler_OumuGaesi_CheckParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Oumugaesi_ZenryokuReqWazaMsg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Sakidori( u32* numElems );
void handler_Sakidori_CheckParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
bool handler_Sakidori_CheckSuccess( ServerFlow* flowWk, u8 targetPokeID );
void handler_Sakidori_Power( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Sakidori_ZenryokuReqWazaMsg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Manekko( u32* numElems );
void handler_Manekko_CheckParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Manekko_ZenryokuReqWazaMsg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_GensiNoTikara( u32* numElems );
void handler_GensiNoTikara( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_BenomShock( u32* numElems );
void handler_BenomShock( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Tatarime( u32* numElems );
void handler_Tatarime( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Acrobat( u32* numElems );
void handler_Acrobat( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_AsistPower( u32* numElems );
void handler_AsistPower( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_HeavyBomber( u32* numElems );
void handler_HeavyBomber( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_HeatStamp( u32* numElems );
const BtlEventHandlerTable*  ADD_ElectBall( u32* numElems );
void handler_ElectBall( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_EchoVoice( u32* numElems );
void handler_EchoVoice( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Katakiuti( u32* numElems );
void handler_Katakiuti( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Ikasama( u32* numElems );
void handler_Ikasama( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Mizubitasi( u32* numElems );
void handler_Mizubitasi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_SimpleBeem( u32* numElems );
void handler_SimpleBeem( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_NakamaDukuri( u32* numElems );
void handler_NakamaDukuri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_ClearSmog( u32* numElems );
void handler_ClearSmog( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Yakitukusu( u32* numElems );
void handler_Yakitukusu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_TomoeNage( u32* numElems );
void handler_TomoeNage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Hoeru( u32* numElems );
void handler_Hoeru( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Utiotosu( u32* numElems );
void handler_Utiotosu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
bool common_UtiotosuEffect( ServerFlow* flowWk, u8 atkPokeID, u8 targetPokeID );
const BtlEventHandlerTable*  ADD_KarawoYaburu( u32* numElems );
void handler_KarawoYaburu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_MirrorType( u32* numElems );
void handler_MirrorType( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_BodyPurge( u32* numElems );
void handler_BodyPurge( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_PsycoShock( u32* numElems );
void handler_PsycoShock( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_NasiKuzusi( u32* numElems );
void handler_NasiKuzusi_CalcDmg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_NasiKuzusi_HitCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_WonderRoom( u32* numElems );
void handler_WonderRoom( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_MagicRoom( u32* numElems );
void handler_MagicRoom( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_HajikeruHonoo( u32* numElems );
void handler_HajikeruHonoo( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
BtlExPos hajikeruHonoo_GetAdditionalDamagePos( ServerFlow* flowWk, u8 attackPokeID, u8 damagePokeID );
const BtlEventHandlerTable*  ADD_SyncroNoise( u32* numElems );
void handler_SyncroNoise( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_GiftPass( u32* numElems );
void handler_GiftPass( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Inotigake( u32* numElems );
void handler_Inotigake_CalcDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Inotigake_CheckDead( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_OsakiniDouzo( u32* numElems );
void handler_OsakiniDouzo( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Sakiokuri( u32* numElems );
void handler_Sakiokuri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Rinsyou( u32* numElems );
void handler_Rinsyou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Rinsyou_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_FastGuard( u32* numElems );
void handler_FastGuard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_SideChange( u32* numElems );
void handler_SideChange( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Telekinesis( u32* numElems );
void handler_Telekinesis_CheckFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Telekinesis_NoEffCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Telekinesis( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_FreeFall( u32* numElems );
void handler_FreeFall_WazaParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_FreeFall_TameFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_FreeFall_TameStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_FreeFall_TameRelease( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_FreeFall_DmgProcEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_FreeFall_NotEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void freefall_common_recoverTarget( ServerFlow* flowWk, int* work );
void handler_FreeFall_Target( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_FreeFall_FailCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_FreeFall_TypeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_InisieNoUta( u32* numElems );
void handler_InisieNoUta( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Miyaburu( u32* numElems );
void handler_Miyaburu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_MiracleEye( u32* numElems );
void handler_MiracleEye( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Seityou( u32* numElems );
void handler_Seityou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_FreezeBolt( u32* numElems );
void handler_FreezeBolt_TameStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_FlameSoul( u32* numElems );
void handler_FlameSoul_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
CombiEffectType GetCombiWazaType( WazaID waza1, WazaID waza2 );
const BtlEventHandlerTable*  ADD_CombiWazaCommon( u32* numElems );
void handler_CombiWaza_CheckExe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_CombiWaza_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_CombiWaza_TypeMatch( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_CombiWaza_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_CombiWaza_ChangeEff( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_CombiWaza_AfterDmg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Halloween( u32* numElems );
void handler_Halloween( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Morinonoroi( u32* numElems );
void handler_Morinonoroi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_ObakeGokko( u32* numElems );
void handler_ObakeGokko( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Kyoumei( u32* numElems );
void handler_Kyoumei( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_MentalCare( u32* numElems );
void handler_MentalCare( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Tagayasu( u32* numElems );
void handler_Tagayasu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tagayasu_CheckHide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tagayasu_CheckHit( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
bool common_isTagayasuNoEffect( ServerFlow* flowWk, u8 pokeID );
const BtlEventHandlerTable*  ADD_FlowerGuard( u32* numElems );
void handler_FlowerGuard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void common_GuardUpByPokeType( ServerFlow* flowWk, u8 pokeID, pml::PokeType pokeType );
const BtlEventHandlerTable*  ADD_Izanau( u32* numElems );
void handler_Izanau( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_TodomeBari( u32* numElems );
void handler_TodomeBari( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_KogoeruHadou( u32* numElems );
void handler_KogoeruHadou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_NebaNebaNet( u32* numElems );
void handler_NebaNebaNet( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Hikkurikaesu( u32* numElems );
void handler_Hikkurikaesu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_NeraiPunch( u32* numElems );
void handler_NeraiPunch( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_SuteZerifu( u32* numElems );
const BtlEventHandlerTable*  ADD_FlyingPress( u32* numElems );
void handler_FlyingPress( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_FreezDry( u32* numElems );
void handler_FreezDry( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Souden( u32* numElems );
void handler_Souden( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_GrassField( u32* numElems );
const BtlEventHandlerTable*  ADD_MistField( u32* numElems );
void handler_GrassField( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MistField( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_common_GroundSet( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work, BtlGround ground, u16 strID );
const BtlEventHandlerTable*  ADD_Nettou( u32* numElems );
void handler_Nettou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_UtakatanoAria( u32* numElems );
void handler_UtakatanoAria( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_KingShield( u32* numElems );
void handler_KingShield( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KingShield_Success( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KingShield_DmgZ( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KingShield_MsgAfterCritical( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KingShield_DmgReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void kingShield_Success( ServerFlow* flowWk, u8 pokeID );
const BtlEventHandlerTable*  ADD_ThousanArrow( u32* numElems );
void handler_ThousanArrow_CancelFloat( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_ThousanArrow_AffEnable( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_ThousanArrow_CheckAffine( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_ThousanArrow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_ElecField( u32* numElems );
void handler_ElecField( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_PhychoField( u32* numElems );
void handler_PhychoField( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_HappyTime( u32* numElems );
void handler_HappyTime( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_ZibaSousa( u32* numElems );
void handler_ZibaSousa( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_BenomTrap( u32* numElems );
void handler_BenomTrap( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_PlasmaShower( u32* numElems );
void handler_PlasmaShower( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_FairyLock( u32* numElems );
void handler_FairyLock( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Funjin( u32* numElems );
void handler_Funjin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_GeoControl( u32* numElems );
void handler_GeoControl_TameStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_TrickGuard( u32* numElems );
void handler_TrickGuard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_NeedleGuard( u32* numElems );
void handler_NeedleGuard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_NeedleGuard_Success( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_NeedleGuard_DmgZ( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_NeedleGuard_MsgAfterCritical( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_NeedleGuard_DmgReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void needleGuard_Success( ServerFlow* flowWk, u8 pokeID );
const BtlEventHandlerTable*  ADD_Tootika( u32* numElems );
void handler_Tootika( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tootika_Success( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tootika_DmgZ( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tootika_MsgAfterCritical( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tootika_DmgReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void tootika_Success( ServerFlow* flowWk, u8 pokeID );
const BtlEventHandlerTable*  ADD_MezameruDance( u32* numElems );
void handler_MezameruDance_WazaParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_SouthernWave( u32* numElems );
void handler_southernWave( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_TeWoTunagu( u32* numElems );
void handler_TeWoTunagu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Oiwai( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Oiwai( u32* numElems );
const BtlEventHandlerTable*  ADD_Kahundango( u32* numElems );
void handler_Kahundango_ExecuteCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Kahundango_RecoverCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Kahundango_RecoverFix(BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work);
void handler_Kahundango_Check_Affinity(BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work);
const BtlEventHandlerTable*  ADD_CorePunisher( u32* numElems );
void handler_CorePunisher_HitReal( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Kagenui( u32* numElems );
void handler_Kagenui_HitReal( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_IjigenRush( u32* numElems );
void handler_IjigenRush( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_IjigenRush_AfterDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_DarkHole( u32* numElems );
void handler_DarkHole( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Zyouka( u32* numElems );
void handler_Zyouka( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Tikarawosuitoru( u32* numElems );
void handler_Tikarawosuitoru( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Togisumasu( u32* numElems );
void handler_Togisumasu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_SpeedSwap( u32* numElems );
void handler_SpeedSwap( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Moetukiru( u32* numElems );
void handler_Moetukiru_WazaMeltCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Moetukiru_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Moetukiru_DamageProcEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_SpotLight( u32* numElems );
void handler_SpotLight( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_KutibasiCanon( u32* numElems );
void handler_KutibasiCanon_BeforeFight( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KutibasiCanon_DamageReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KutibasiCanon_WazaSeqEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KutibasiCanon_ReplaceWaza( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KutibasiCanon_Canceled( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KutibasiCanon_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_TrapShell( u32* numElems );
void handler_TrapShell_BeforeFight( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_TrapShell_DamageProcEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_TrapShell_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_TrapShell_WazaEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Zidanda( u32* numElems );
void handler_Zidanda_Dmg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_AuroraVeil( u32* numElems );
void handler_AuroraVeil_CheckExe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_AuroraVeil( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_Saihai( u32* numElems );
void handler_Saihai( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* ADD_OriginsSuperNova( u32* numElems );
void handler_OriginsSuperNova_AfterDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* ADD_MeteorDrive( u32* numElems );
void handler_MeteorDrive_WazaSeqStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MeteorDrive_WazaSeqEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* ADD_ShadowSteal( u32* numElems );
void handler_ShadowSteal_DamageProcStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  ADD_FieldDelete(u32* numElems);
void handler_FieldDelete(BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work);
const BtlEventHandlerTable* ADD_PrismAttack(u32* numElems);
void handler_PrismAttack_WazaParam(BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work);
#if 0
const BtlEventHandlerTable*  ADD_ZenryokuZyaretuku(u32* numElems);
void handler_ZenryokuZyaretuku(BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work);
#endif
const BtlEventHandlerTable* ADD_Hanabisenyou(u32* numElems);
void handler_Hanabisenyou_dmg_determine(BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work);
void handler_Hanabisenyou_no_effect(BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work);
void common_HanabisenyouReaction(BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work);

bool  Add( const BTL_POKEPARAM* bpp, WazaID waza, u32 subPri )
{
  typedef const BtlEventHandlerTable* (*pEventAddFunc)( u32* numElems );

  static const struct {
    WazaID         waza;
    pEventAddFunc  func;
  }funcTbl[] = {
    { WAZANO_TEKUSUTYAA,             ADD_Texture           },
    { WAZANO_HOGOSYOKU,              ADD_Hogosyoku         },
    { WAZANO_KUROIKIRI,              ADD_KuroiKiri         },
    { WAZANO_YUMEKUI,                ADD_Yumekui           },
    { WAZANO_TORAIATAKKU,            ADD_TriAttack         },
    { WAZANO_HIMITUNOTIKARA,         ADD_HimituNoTikara    },
//  { WAZANO_OSYABERI,               ADD_Osyaberi          },
    { WAZANO_IKARINOMAEBA,           ADD_IkariNoMaeba      },
    { WAZANO_RYUUNOIKARI,            ADD_RyuuNoIkari       },
    { WAZANO_TIKYUUNAGE,             ADD_TikyuuNage        },
    { WAZANO_NAITOHEDDO,             ADD_TikyuuNage        },  // ナイトヘッド=ちきゅうなげと等価
    { WAZANO_SAIKOWHEEBU,            ADD_PsycoWave         },
    { WAZANO_IBIKI,                  ADD_Ibiki             },
    { WAZANO_TOTTEOKI,               ADD_Totteoki          },
    { WAZANO_ZITABATA,               ADD_Jitabata          },
    { WAZANO_KISIKAISEI,             ADD_Jitabata          },  // きしかいせい=じたばた と等価
    { WAZANO_MINEUTI,                ADD_Mineuti           },
    { WAZANO_KUMONOSU,               ADD_KumoNoSu          },
    { WAZANO_KUROIMANAZASI,          ADD_KumoNoSu          },  // くろいまなざし = くものす と等価
    { WAZANO_TOOSENBOU,              ADD_KumoNoSu          },  // とおせんぼう  = くものす と等価
    { WAZANO_KORAERU,                ADD_Koraeru           },
    { WAZANO_SONIKKUBUUMU,           ADD_SonicBoom         },
    { WAZANO_NEKODAMASI,             ADD_Nekodamasi        },
    { WAZANO_GAMUSYARA,              ADD_Gamusyara         },
    { WAZANO_HUNKA,                  ADD_Funka             },
    { WAZANO_SIOHUKI,                ADD_Funka             },  // しおふき=ふんか と等価
    { WAZANO_RIHURESSYU,             ADD_Refresh           },
    { WAZANO_ASANOHIZASI,            ADD_AsaNoHizasi       },
    { WAZANO_TUKINOHIKARI,           ADD_AsaNoHizasi       },  // つきのひかり = あさのひざし と等価
    { WAZANO_KOUGOUSEI,              ADD_AsaNoHizasi       },  // こうごうせい = あさのひざし と等価
    { WAZANO_SIBORITORU,             ADD_Siboritoru        },
    { WAZANO_NIGIRITUBUSU,           ADD_Siboritoru        },  // にぎりつぶす = しぼりとる と等価
    { WAZANO_WHEZAABOORU,            ADD_WeatherBall       },
    { WAZANO_MAMORU,                 ADD_Mamoru            },
    { WAZANO_MIKIRI,                 ADD_Mamoru            },  // みきり = まもる と等価
    { WAZANO_HANERU,                 ADD_Haneru            },
    { WAZANO_NOROI,                  ADD_Noroi             },
    { WAZANO_ABARERU,                ADD_Abareru           },
    { WAZANO_SAWAGU,                 ADD_Sawagu            },
    { WAZANO_HANABIRANOMAI,          ADD_Abareru           },  // はなびらのまい = あばれる と等価
    { WAZANO_GEKIRIN,                ADD_Abareru           },  // げきりん = あばれる と等価
    { WAZANO_SIOMIZU,                ADD_Siomizu           },
    { WAZANO_MEZAMASIBINTA,          ADD_MezamasiBinta     },
    { WAZANO_KITUKE,                 ADD_Kituke            },
    { WAZANO_TUBOWOTUKU,             ADD_TuboWoTuku        },
    { WAZANO_KIRIHUDA,               ADD_Kirifuda          },
    { WAZANO_SORAWOTOBU,             ADD_SoraWoTobu        },
    { WAZANO_TOBIHANERU,             ADD_Tobihaneru        },
    { WAZANO_DAIBINGU,               ADD_Diving            },
    { WAZANO_ANAWOHORU,              ADD_AnaWoHoru         },
    { WAZANO_SOORAABIIMU,            ADD_SolarBeam         },
    { WAZANO_SOORAABUREEDO,          ADD_SolarBeam         }, // ソーラーブレード = ソーラービーム　と等価
    { WAZANO_KAMAITATI,              ADD_Kamaitati         },
    { WAZANO_GODDOBAADO,             ADD_GodBird           },
    { WAZANO_ROKETTOZUTUKI,          ADD_RocketZutuki      },
    { WAZANO_ANKOORU,                ADD_Encore            },
    { WAZANO_TATUMAKI,               ADD_Tatumaki          },
    { WAZANO_KAZEOKOSI,              ADD_Tatumaki          },
    { WAZANO_ZISIN,                  ADD_Jisin             },
    { WAZANO_NAMINORI,               ADD_Naminori          },
    { WAZANO_SUKAIAPPAA,             ADD_SkyUpper          },
    { WAZANO_MAGUNITYUUDO,           ADD_Magnitude         },
    { WAZANO_FEINTO,                 ADD_Feint             },
    { WAZANO_SYADOODAIBU,            ADD_ShadowDive        },
    { WAZANO_WARUAGAKI,              ADD_Waruagaki         },
    { WAZANO_OSIOKI,                 ADD_Osioki            },
    { WAZANO_TYOUHATU,               ADD_Chouhatu          },
    { WAZANO_YUUWAKU,                ADD_Yuwaku            },
    { WAZANO_HARADAIKO,              ADD_Haradaiko         },
    { WAZANO_MITIDURE,               ADD_Michidure         },
    { WAZANO_KARAGENKI,              ADD_Karagenki         },
    { WAZANO_SIPPEGAESI,             ADD_Sippegaesi        },
    { WAZANO_MEZAMERUPAWAA,          ADD_MezameruPower     },
    { WAZANO_TIISAKUNARU,            ADD_Tiisakunaru       },
    { WAZANO_MARUKUNARU,             ADD_Marukunaru        },
    { WAZANO_HUMITUKE,               ADD_Fumituke          },
    { WAZANO_AKUMU,                  ADD_Akumu             },
    { WAZANO_HUIUTI,                 ADD_Fuiuti            },
    { WAZANO_KOROGARU,               ADD_Korogaru          },
    { WAZANO_AISUBOORU,              ADD_Korogaru          },  // アイスボール=ころがる と等価
    { WAZANO_AROMASERAPII,           ADD_Alomatherapy      },
    { WAZANO_IYASINOSUZU,            ADD_IyasiNoSuzu       },
    { WAZANO_OKIMIYAGE,              ADD_Okimiyage         },
    { WAZANO_URAMI,                  ADD_Urami             },
    { WAZANO_NEMURU,                 ADD_Nemuru            },
    { WAZANO_ROKKUON,                ADD_LockON            },
    { WAZANO_KOKORONOME,             ADD_LockON            },  // こころのめ=ロックオンと等価
    { WAZANO_RIHUREKUTAA,            ADD_Refrector         },
    { WAZANO_HIKARINOKABE,           ADD_HikariNoKabe      },
    { WAZANO_SINPINOMAMORI,          ADD_SinpiNoMamori     },
    { WAZANO_SIROIKIRI,              ADD_SiroiKiri         },
    { WAZANO_OIKAZE,                 ADD_Oikaze            },
    { WAZANO_ONGAESI,                ADD_Ongaesi           },
    { WAZANO_YATUATARI,              ADD_Yatuatari         },
    { WAZANO_PUREZENTO,              ADD_Present           },
    { WAZANO_ITYAMON,                ADD_Ichamon           },
    { WAZANO_HUUIN,                  ADD_Fuuin             },
    { WAZANO_ZYUURYOKU,              ADD_Juryoku           },
    { WAZANO_ONNEN,                  ADD_Onnen             },
    { WAZANO_TEDASUKE,               ADD_Tedasuke          },
    { WAZANO_IEKI,                   ADD_Ieki              },
    { WAZANO_NARIKIRI,               ADD_Narikiri          },
    { WAZANO_MAKIBISI,               ADD_Makibisi          },
    { WAZANO_DOKUBISI,               ADD_Dokubisi          },
    { WAZANO_SUTERUSUROKKU,          ADD_StealthRock       },
    { WAZANO_HANEYASUME,             ADD_Haneyasume        },
    { WAZANO_DENZIHUYUU,             ADD_DenjiFuyuu        },
    { WAZANO_RENZOKUGIRI,            ADD_RenzokuGiri       },
    { WAZANO_SAIKOSIHUTO,            ADD_PsycoShift        },
    { WAZANO_DAMEOSI,                ADD_Dameosi           },
    { WAZANO_TEKUSUTYAA2,            ADD_Texture2          },
    { WAZANO_KAUNTAA,                ADD_Counter           },
    { WAZANO_MIRAAKOOTO,             ADD_MilerCoat         },
    { WAZANO_METARUBAASUTO,          ADD_MetalBurst        },
    { WAZANO_RIBENZI,                ADD_Revenge           },
    { WAZANO_YUKINADARE,             ADD_Revenge           },  // ゆきなだれ=リベンジと等価
    { WAZANO_TORIPURUKIKKU,          ADD_TripleKick        },
    { WAZANO_ZYAIROBOORU,            ADD_GyroBall          },
    { WAZANO_ITAMIWAKE,              ADD_Itamiwake         },
    { WAZANO_KONOYUBITOMARE,         ADD_KonoyubiTomare    },
    { WAZANO_NAYAMINOTANE,           ADD_NayamiNoTane      },
    { WAZANO_DENZIHA,                ADD_Denjiha           },
    { WAZANO_ZIKOANZI,               ADD_JikoAnji          },
    { WAZANO_HAATOSUWAPPU,           ADD_HeartSwap         },
    { WAZANO_PAWAASUWAPPU,           ADD_PowerSwap         },
    { WAZANO_GAADOSUWAPPU,           ADD_GuardSwap         },
    { WAZANO_MEROMERO,               ADD_Meromero          },
    { WAZANO_SABAKINOTUBUTE,         ADD_SabakiNoTubute    },
    { WAZANO_SIZENNOMEGUMI,          ADD_SizenNoMegumi     },
    { WAZANO_HATAKIOTOSU,            ADD_Hatakiotosu       },
    { WAZANO_KANASIBARI,             ADD_Kanasibari        },
    { WAZANO_DOROBOU,                ADD_Dorobou           },
    { WAZANO_HOSIGARU,               ADD_Dorobou           },  // ほしがる=どろぼう と等価
    { WAZANO_TORIKKU,                ADD_Trick             },
    { WAZANO_SURIKAE,                ADD_Trick             },  // すりかえ=トリックと等価
    { WAZANO_MONOMANE,               ADD_Monomane          },
    { WAZANO_SUKETTI,                ADD_Sketch            },
    { WAZANO_TOBIGERI,               ADD_Tobigeri          },
    { WAZANO_TOBIHIZAGERI,           ADD_Tobigeri          },
    { WAZANO_KIRIBARAI,              ADD_Kiribarai         },
    { WAZANO_KAWARAWARI,             ADD_Kawarawari        },
    { WAZANO_TORIKKURUUMU,           ADD_TrickRoom         },
    { WAZANO_MIZUASOBI,              ADD_MizuAsobi         },
    { WAZANO_DOROASOBI,              ADD_DoroAsobi         },
    { WAZANO_ZYUUDEN,                ADD_Juden             },
    { WAZANO_HOROBINOUTA,            ADD_HorobiNoUta       },
    { WAZANO_YADORIGINOTANE,         ADD_YadorigiNoTane    },
    { WAZANO_HUKURODATAKI,           ADD_FukuroDataki      },
    { WAZANO_AKUARINGU,              ADD_AquaRing          },
    { WAZANO_MIKADUKINOMAI,          ADD_MikadukiNoMai     },
    { WAZANO_IYASINONEGAI,           ADD_IyasiNoNegai      },
    { WAZANO_YUBIWOHURU,             ADD_YubiWoFuru        },
    { WAZANO_SIZENNOTIKARA,          ADD_SizenNoTikara     },
    { WAZANO_NEKONOTE,               ADD_Nekonote          },
    { WAZANO_OUMUGAESI,              ADD_OumuGaesi         },
    { WAZANO_SAKIDORI,               ADD_Sakidori          },
    { WAZANO_MANEKKO,                ADD_Manekko           },
    { WAZANO_NEGOTO,                 ADD_Negoto            },
    { WAZANO_KETAGURI,               ADD_Ketaguri          },
    { WAZANO_KUSAMUSUBI,             ADD_Ketaguri          },  // くさむすび=けたぐりと等価
    { WAZANO_KIAIPANTI,              ADD_KiaiPunch         },
    { WAZANO_TAKUWAERU,              ADD_Takuwaeru         },
    { WAZANO_HAKIDASU,               ADD_Hakidasu          },
    { WAZANO_NOMIKOMU,               ADD_Nomikomu          },
    { WAZANO_MIRAIYOTI,              ADD_Miraiyoti         },
    { WAZANO_HAMETUNONEGAI,          ADD_HametuNoNegai     },
    { WAZANO_RISAIKURU,              ADD_Recycle           },
    { WAZANO_OIUTI,                  ADD_Oiuti             },
    { WAZANO_NEKONIKOBAN,            ADD_NekoNiKoban       },
    { WAZANO_GAMAN,                  ADD_Gaman             },
    { WAZANO_YOKODORI,               ADD_Yokodori          },
    { WAZANO_MAZIKKUKOOTO,           ADD_MagicCoat         },
    { WAZANO_TEREPOOTO,              ADD_Teleport          },
    { WAZANO_TONBOGAERI,             ADD_TonboGaeri        },
    { WAZANO_BATONTATTI,             ADD_BatonTouch        },
    { WAZANO_TUIBAMU,                ADD_Tuibamu           },
    { WAZANO_MUSIKUI,                ADD_Tuibamu           },  // むしくい=ついばむ と等価
    { WAZANO_NAGETUKERU,             ADD_Nagetukeru        },
    { WAZANO_MAKITUKU,               ADD_Makituku          },
    { WAZANO_SIMETUKERU,             ADD_Makituku          },
    { WAZANO_HONOONOUZU,             ADD_Makituku          },
    { WAZANO_KARADEHASAMU,           ADD_Makituku          },
    { WAZANO_SUNAZIGOKU,             ADD_Makituku          },
    { WAZANO_MAGUMASUTOOMU,          ADD_Makituku          },
    { WAZANO_UZUSIO,                 ADD_Uzusio            },
    { WAZANO_KOUSOKUSUPIN,           ADD_KousokuSpin       },
    { WAZANO_PAWAATORIKKU,           ADD_PowerTrick        },
    { WAZANO_HENSIN,                 ADD_Hensin            },
    { WAZANO_DAIBAKUHATU,            ADD_Daibakuhatsu      },
    { WAZANO_ZIBAKU,                 ADD_Daibakuhatsu      },
    { WAZANO_KIAIDAME,               ADD_Kiaidame          },
    { WAZANO_IKARI,                  ADD_Ikari             },
    { WAZANO_GENSINOTIKARA,          ADD_GensiNoTikara     },
    { WAZANO_AYASIIKAZE,             ADD_GensiNoTikara     },  // あやしいかぜ=げんしのちから
    { WAZANO_GINIRONOKAZE,           ADD_GensiNoTikara     },  // ぎんいろのかぜ=げんしのちから
    { WAZANO_KAMINARI,               ADD_Kaminari          },
    { WAZANO_HUBUKI,                 ADD_Fubuki            },
    { WAZANO_ZETTAIREIDO,            ADD_ZettaiReido       },
    { WAZANO_NEGAIGOTO,              ADD_Negaigoto         },
    { WAZANO_OMAZINAI,               ADD_Omajinai          },
    { WAZANO_MIYABURU,               ADD_Miyaburu          },
    { WAZANO_KAGIWAKERU,             ADD_Miyaburu          },  // かぎわける=みやぶる
    { WAZANO_MIRAKURUAI,             ADD_MiracleEye        },
    { WAZANO_SEITYOU,                ADD_Seityou           },
    { WAZANO_DOKUDOKU,               ADD_Dokudoku          },
    { WAZANO_BENOMUSYOKKU,           ADD_BenomShock        },
    { WAZANO_IKARINOKONA,            ADD_Ikarinokona       },
    { WAZANO_MIZUBITASI,             ADD_Mizubitasi        },
    { WAZANO_SINPURUBIIMU,           ADD_SimpleBeem        },
    { WAZANO_NAKAMADUKURI,           ADD_NakamaDukuri      },
    { WAZANO_KURIASUMOGGU,           ADD_ClearSmog         },
    { WAZANO_ASISUTOPAWAA,           ADD_AsistPower        },
    { WAZANO_TUKEAGARU,              ADD_AsistPower        },  // つけあがる=アシストパワー
    { WAZANO_KARAWOYABURU,           ADD_KarawoYaburu      },
    { WAZANO_TATARIME,               ADD_Tatarime          },
    { WAZANO_AKUROBATTO,             ADD_Acrobat           },
    { WAZANO_BORUTOTHENZI,           ADD_TonboGaeri        },  // ボルトチェンジ=とんぼがえり
    { WAZANO_WAIDOGAADO,             ADD_WideGuard         },
    { WAZANO_MIRAATAIPU,             ADD_MirrorType        },
    { WAZANO_PAWAASHEA,              ADD_PowerShare        },
    { WAZANO_GAADOSHEA,              ADD_GuardShare        },
    { WAZANO_BODHIPAAZI,             ADD_BodyPurge         },
    { WAZANO_HEBIIBONBAA,            ADD_HeavyBomber       },
    { WAZANO_HIITOSUTANPU,           ADD_HeatStamp         },
    { WAZANO_WANDAARUUMU,            ADD_WonderRoom        },
    { WAZANO_MAZIKKURUUMU,           ADD_MagicRoom         },
    { WAZANO_SAIKOSYOKKU,            ADD_PsycoShock        },
    { WAZANO_SAIKOBUREIKU,           ADD_PsycoShock        },  // サイコブレイク=サイコショック
    { WAZANO_HAZIKERUHONOO,          ADD_HajikeruHonoo     },
    { WAZANO_EREKIBOORU,             ADD_ElectBall         },
    { WAZANO_SINKURONOIZU,           ADD_SyncroNoise       },
    { WAZANO_NASIKUZUSI,             ADD_NasiKuzusi        },
    { WAZANO_SEINARUTURUGI,          ADD_NasiKuzusi        },  // せいなるつるぎ=なしくずし
    { WAZANO_ddRARIATTO,             ADD_NasiKuzusi        },  // ＤＤラリアット=なしくずし
    { WAZANO_EKOOBOISU,              ADD_EchoVoice         },
    { WAZANO_YAKITUKUSU,             ADD_Yakitukusu        },
    { WAZANO_GIHUTOPASU,             ADD_GiftPass          },
    { WAZANO_TOMOENAGE,              ADD_TomoeNage         },
    { WAZANO_DORAGONTEERU,           ADD_TomoeNage         },  // ドラゴンテール=ともえなげ
    { WAZANO_KATAKIUTI,              ADD_Katakiuti         },
    { WAZANO_IKASAMA,                ADD_Ikasama           },
    { WAZANO_UTIOTOSU,               ADD_Utiotosu          },
    { WAZANO_INOTIGAKE,              ADD_Inotigake         },
    { WAZANO_OSAKINIDOUZO,           ADD_OsakiniDouzo      },
    { WAZANO_SAKIOKURI,              ADD_Sakiokuri         },
    { WAZANO_RINSYOU,                ADD_Rinsyou           },
    { WAZANO_FASUTOGAADO,            ADD_FastGuard         },
    { WAZANO_SAIDOTHENZI,            ADD_SideChange        },
    { WAZANO_TEREKINESISU,           ADD_Telekinesis       },
    { WAZANO_HURIIFOORU,             ADD_FreeFall          },
    { WAZANO_HAADOROORAA,            ADD_Fumituke          },  // ハードローラー=ふみつけ
    { WAZANO_BOUHUU,                 ADD_Kaminari          },  // ぼうふう=かみなり
    { WAZANO_SINPINOTURUGI,          ADD_PsycoShock        },  // しんぴのつるぎ=サイコショック
    { WAZANO_INISIENOUTA,            ADD_InisieNoUta       },
    { WAZANO_TEKUNOBASUTAA,          ADD_TechnoBaster      },  // テクノバスター=さばきのつぶて
    { WAZANO_HURIIZUBORUTO,          ADD_FreezeBolt        },
    { WAZANO_KOORUDOHUREA,           ADD_FreezeBolt        },
    { WAZANO_MIZUNOTIKAI,            ADD_CombiWazaCommon   },  // 合体ワザは共通ハンドラ
    { WAZANO_HONOONOTIKAI,           ADD_CombiWazaCommon   },
    { WAZANO_KUSANOTIKAI,            ADD_CombiWazaCommon   },
    { WAZANO_KUROSUHUREIMU,          ADD_FlameSoul         },  // リンクワザも共通ハンドラ
    { WAZANO_KUROSUSANDAA,           ADD_FlameSoul         },
    { WAZANO_NETTOU,                 ADD_Nettou            },
    { WAZANO_UTAKATANOARIA,          ADD_UtakatanoAria     },
    { WAZANO_DORAGONDAIBU,           ADD_Fumituke          },  // ドラゴンダイブは「ふみつけ」と同じ効果を持つ
    { WAZANO_NOSIKAKARI,             ADD_Fumituke          },  // のしかかりは「ふみつけ」と同じ効果を持つ
    { WAZANO_HAROWHIN,               ADD_Halloween         },
    { WAZANO_MORINONOROI,            ADD_Morinonoroi       },
    { WAZANO_HIKKURIKAESU,           ADD_Hikkurikaesu      },
    { WAZANO_SUTEZERIHU,             ADD_SuteZerifu        },
    { WAZANO_HURAWAAGAADO,           ADD_FlowerGuard       },
    { WAZANO_TAGAYASU,               ADD_Tagayasu          },
    { WAZANO_TODOMEBARI,             ADD_TodomeBari        },
    { WAZANO_NEBANEBANETTO,          ADD_NebaNebaNet       },
    { WAZANO_GURASUFIIRUDO,          ADD_GrassField        },
    { WAZANO_MISUTOFIIRUDO,          ADD_MistField         },
    { WAZANO_HURAINGUPURESU,         ADD_FlyingPress       },
    { WAZANO_GOOSUTODAIBU,           ADD_ShadowDive        },
    { WAZANO_TATAMIGAESI,            ADD_TatamiGaeshi      },
    { WAZANO_HURIIZUDORAI,           ADD_FreezDry          },
    { WAZANO_SOUDEN,                 ADD_Souden            },
    { WAZANO_KINGUSIIRUDO,           ADD_KingShield      },
    { WAZANO_SAUZANWHEEBU,           ADD_SouthernWave      }, // サウザンウェーブ = 「くものす」の対象違い版
    { WAZANO_SAUZANAROO,             ADD_ThousanArrow      },
    { WAZANO_EREKIFIIRUDO,           ADD_ElecField         },
    { WAZANO_HAPPIITAIMU,            ADD_HappyTime         },
    { WAZANO_ZIBASOUSA,              ADD_ZibaSousa         },
    { WAZANO_ASISUTOGIA,             ADD_ZibaSousa         }, // アシストギア = じばそうさ　と等価
    { WAZANO_BENOMUTORAPPU,          ADD_BenomTrap         },
    { WAZANO_PURAZUMASYAWAA,         ADD_PlasmaShower      },
    { WAZANO_FEARIIROKKU,            ADD_FairyLock         },
    { WAZANO_IZIGENHOORU,            ADD_IjigenHall        },  // いじげんホール=フェイントと同じ
    { WAZANO_HUNZIN,                 ADD_Funjin            },
    { WAZANO_ZIOKONTOROORU,          ADD_GeoControl        },
    { WAZANO_TORIKKUGAADO,           ADD_TrickGuard        },
    { WAZANO_NIIDORUGAADO,           ADD_NeedleGuard       },
    { WAZANO_TEKAGEN,                ADD_Mineuti           },  // てかげん = みねうち
    { WAZANO_MATOWARITUKU,           ADD_Makituku          },
    { WAZANO_SUTIIMUBAASUTO,         ADD_Nettou            },
    { WAZANO_OIWAI,                  ADD_Oiwai             },
    { WAZANO_TEWOTUNAGU,             ADD_TeWoTunagu        },
    { WAZANO_HOERU,                  ADD_Hoeru             },
    { WAZANO_HUKITOBASI,             ADD_Hoeru             },
    { WAZANO_IZIGENRASSYU,           ADD_IjigenRush        },
    { WAZANO_SUNAATUME,              ADD_Sunaatume         },
    { WAZANO_HURAWAAHIIRU,           ADD_FlowerHeal        },
    { WAZANO_DEAIGASIRA,             ADD_Deaigasira        },
    { WAZANO_TOOTIKA,                ADD_Tootika           },
    { WAZANO_MEZAMERUDANSU,          ADD_MezameruDance     },
    { WAZANO_KAHUNDANGO,             ADD_Kahundango        },
    { WAZANO_KOAPANISSYAA,           ADD_CorePunisher      },
    { WAZANO_KAGENUI,                ADD_Kagenui           },
    { WAZANO_ANKAASYOTTO,            ADD_Kagenui           },  // アンカーショット = かげぬい
    { WAZANO_ZYOUKA,                 ADD_Zyouka            },
    { WAZANO_TIKARAWOSUITORU,        ADD_Tikarawosuitoru   },
    { WAZANO_TOGISUMASU,             ADD_Togisumasu        },
    { WAZANO_SUPIIDOSUWAPPU,         ADD_SpeedSwap         },
    { WAZANO_MOETUKIRU,              ADD_Moetukiru         },
    { WAZANO_SAIKOFIIRUDO,           ADD_PhychoField       },
    { WAZANO_SUPOTTORAITO,           ADD_SpotLight         },
    { WAZANO_KUTIBASIKYANON,         ADD_KutibasiCanon     },
    { WAZANO_SAIKOFANGU,             ADD_Kawarawari        },  // サイコファング = かわらわり
    { WAZANO_TORAPPUSHERU,           ADD_TrapShell         },
    { WAZANO_ZIDANDA,                ADD_Zidanda           },
    { WAZANO_OORORABEERU,            ADD_AuroraVeil        },
    { WAZANO_SAIHAI,                 ADD_Saihai            },
    { WAZANO_SEBUNEBORUBUUSUTO,      ADD_GensiNoTikara     },  // セブンエボルブースト = げんしのちから
    { WAZANO_ORIZINZUSUUPAANOVHA,    ADD_OriginsSuperNova  },
    { WAZANO_METEODORAIBU,           ADD_MeteorDrive       },
    { WAZANO_SYADOOREI,              ADD_MeteorDrive       },  // シャドーレイ = メテオドライブ
    { WAZANO_SIZENNOIKARI,           ADD_IkariNoMaeba      },  // しぜんのいかり = いかりのまえば
    { WAZANO_GAADHIANDEAROORA,       ADD_GuardianDeAlola   },
    { WAZANO_MARUTIATAKKU,           ADD_MultiAttack       },  // マルチアタック
    { WAZANO_SYADOOSUTIIRU,          ADD_ShadowSteal       },
    { WAZANO_HAIPAADAAKUKURASSYAA,   ADD_Fumituke          },  // ハイパーダーククラッシャー = ふみつけ
    { WAZANO_DAAKUHOORU,             ADD_DarkHole          },
    { WAZANO_ZENRYOKUSUTOONEZZI,     ADD_FieldDelete       },
    { WAZANO_MABOROSIWAZA,           ADD_PlasmaShower      }, // プラズマフィスト＝先制にならないプラズマシャワー
    { WAZANO_HIKARIDORAGON,          ADD_PrismAttack       },
    { WAZANO_SINNEKUROZUMAZETTO,     ADD_PrismAttack       }, // シンネクロズマゼット = ひかりドラゴン
    { WAZANO_ZENRYOKUMETEODORAIBU,   ADD_MeteorDrive       }, // ゼンリョクメテオドライブ = メテオドライブ
    { WAZANO_ZENRYOKUSYADOOREI,      ADD_MeteorDrive       }, // ゼンリョクシャドーレイ = メテオドライブ
#if 0
    { WAZANO_ZENRYOKUZYARETUKU,      ADD_ZenryokuZyaretuku }, // ぜんりょくじゃれつく
#endif
    { WAZANO_ZENRYOKUSUKEIRUNOIZU,   ADD_GensiNoTikara     }, // スケイルノイズ = げんしのちから
    { WAZANO_HANABISENYOU,           ADD_Hanabisenyou      }, // ビックリヘッド


#if 0
    { (WazaID)WAZANO_KARI_OBAKEGOKKO,     ADD_ObakeGokko      },
    { (WazaID)WAZANO_KARI_KYOUMEI,        ADD_Kyoumei         },
    { (WazaID)WAZANO_KARI_MENTARUKEA,     ADD_MentalCare      },
    { (WazaID)WAZANO_KARI_WHOOTAAGAADO,   ADD_WaterGuard      },
    { (WazaID)WAZANO_KARI_IZANAU,         ADD_Izanau          },
    { (WazaID)WAZANO_KARI_KOGOERUHADOU,   ADD_KogoeruHadou    },
    { (WazaID)WAZANO_KARI_NERAIPANTI,     ADD_NeraiPunch      },
#endif
  };

  for(u32 i=0; i<GFL_NELEMS(funcTbl); i++)
  {
    if( funcTbl[i].waza == waza )
    {
      u8 pokeID = bpp->GetID();
      u8 fRegistered = false;

      if( is_registable(pokeID, waza, &fRegistered) )
      {
        u32 numHandlers;
        const BtlEventHandlerTable* handlerTable = funcTbl[i].func( &numHandlers );
        if( handlerTable )
        {
          BTL_PRINT("[hand_waza] 追加します  pokeID=%d, waza=%d\n", pokeID, waza);
          EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza,
                BTL_EVPRI_WAZA_DEFAULT, subPri, pokeID, handlerTable, numHandlers );
          return true;
        }
      }
      else if( fRegistered ){
        BTL_N_Printf( DBGSTR_HANDWAZA_AlreadyRegistered, pokeID, waza);
      }
      return false;
    }
  }
  return false;
}
//----------------------------------------------------------------------------------
/**
 * @brief 登録できる条件を満たしているか判定
 * ※ 同じポケかつ同じワザのハンドラは１つまで
 * ※ ポケモン１体につき EVENT_WAZA_STICK_MAX まで 登録できる
 *
 * @param   pokeID   イベントファクター登録対象のポケモンID
 * @param   waza     イベントファクター登録対象の技
 *
 * @retval  bool    条件を満たしていたらtrue
 */
//----------------------------------------------------------------------------------
bool is_registable( u8 pokeID, WazaID waza, u8* fRegistered )
{
  BTL_EVENT_FACTOR* factor;
  u32 cnt = 0;

  factor = EVENT_SeekFactor( BTL_EVENT_FACTOR_WAZA, pokeID );
  while( factor )
  {
    if( EVENT_FACTOR_GetSubID(factor) == waza ){
      *fRegistered = true;
      return false;
    }
    if( ++cnt > EVENT_WAZA_STICK_MAX ){
      return false;
    }
    factor = EVENT_GetNextFactor( factor );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief イベントファクターが存在するかどうかチェックする
 * @param   pokeID   イベントファクター登録対象のポケモンID
 * @param   waza     イベントファクター登録対象の技
 * @retval true   指定したイベントファクターが存在する
 * @retval false  指定したイベントファクターが存在しない
 */
//-----------------------------------------------------------------------------
bool IsExist( u8 pokeID, WazaID waza )
{
  BTL_EVENT_FACTOR* factor = EVENT_SeekFactor( BTL_EVENT_FACTOR_WAZA, pokeID );
  while( factor )
  {
    if( EVENT_FACTOR_GetSubID( factor ) == waza ){
      return true;
    }
    factor = EVENT_GetNextFactor( factor );
  }
  return false;
}

//=============================================================================================
/**
 * ポケモンの「ワザ」ハンドラをシステムから削除（ワザの挙動により貼り付いているものは削除しない）
 *
 * @param   pp
 * @param   waza
 */
//=============================================================================================
void Remove( const BTL_POKEPARAM* bpp, WazaID waza )
{
  BTL_EVENT_FACTOR *factor, *next;
  u8 pokeID = bpp->GetID();

  factor = EVENT_SeekFactor( BTL_EVENT_FACTOR_WAZA, pokeID );
  while( factor )
  {
    next = EVENT_GetNextFactor( factor );
    if( (EVENT_FACTOR_GetSubID(factor) == waza)
    &&  (EVENT_FACTOR_GetWorkValue(factor, WORKIDX_STICK) == 0)
    ){
      EVENT_FACTOR_Remove( factor );
    }
    factor = next;
  }
}
//=============================================================================================
/**
 * ポケモンの「ワザ」ハンドラをシステムから強制削除（貼り付いているものも強制的に削除）
 *
 * @param   pp
 * @param   waza
 */
//=============================================================================================
void RemoveForce( const BTL_POKEPARAM* bpp, WazaID waza )
{
  u8 pokeID = bpp->GetID();
  removeHandlerForce( pokeID, waza );
}
//----------------------------------------------------------------------------------
/**
 *
 *
 * @param   pokeID
 * @param   waza
 */
//----------------------------------------------------------------------------------
void removeHandlerForce( u8 pokeID, WazaID waza )
{
  BTL_EVENT_FACTOR* factor = EVENT_SeekFactor( BTL_EVENT_FACTOR_WAZA, pokeID );
  while( factor )
  {
    if( EVENT_FACTOR_GetSubID(factor) == waza){
      EVENT_FACTOR_Remove( factor );
      factor = EVENT_SeekFactor( BTL_EVENT_FACTOR_WAZA, pokeID );
    }else{
      factor = EVENT_GetNextFactor( factor );
    }
  }
}

//=============================================================================================
/**
 * 特定ポケモンのワザハンドラをシステムから全て強制削除（貼り付いているものも強制的に削除）
 *
 * @param   pp
 */
//=============================================================================================
void RemoveForceAll( const BTL_POKEPARAM* bpp )
{
  u8 pokeID = bpp->GetID();
  BTL_EVENT_FACTOR* factor;
  BTL_PRINT("[Hand_waza] RemoveForceAll waza Handler for PokeID(%d)\n", pokeID);
  while( (factor = EVENT_SeekFactor(BTL_EVENT_FACTOR_WAZA, pokeID)) != NULL )
  {
    BTL_PRINT("  factor:%p  remove..\n", factor);
    EVENT_FACTOR_Remove( factor );
  }
}
//=============================================================================================
/**
 * 貼り付き状態にあるハンドラかどうかを判定
 *
 * @param   myHandle
 * @param   work
 *
 * @retval  bool
 */
//=============================================================================================
bool IsStick( BTL_EVENT_FACTOR* myHandle, const int* work )
{
  GFL_UNUSED( myHandle );

  return work[ WORKIDX_STICK ] != 0;
}

/**
 * ハンドラ共通：アクション開始ハンドラにて、自分の開始アクションかどうか判定する
 */
bool common_checkActStart_isMyWaza( ServerFlow* flwk, u8 pokeID )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // メガシンカは無視（まだこの後に続くアクションがある）
    if( EVENTVAR_GetValue(BTL_EVAR_ACTION) != BTL_ACTION_MEGAEVO ){
      return true;
    }
  }
  return false;
}


//----------------------------------------------------------------------------------
/**
 * テクスチャー
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Texture( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Texture },    // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Texture( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( work );

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

    WazaID waza = bpp->WAZA_GetID( 0 );
    pml::PokeType  _1st_waza_type = WAZADATA_GetType( waza );

    if( !(bpp->IsMatchType(_1st_waza_type)) )
    {
      BTL_HANDEX_PARAM_CHANGE_TYPE* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_TYPE*>
                                                (flowWk->HANDEX_Push(BTL_HANDEX_CHANGE_TYPE, pokeID));

       param->next_type = PokeTypePair_MakePure( _1st_waza_type );
       param->exTypeCause = BTL_POKEPARAM::EXTYPE_CAUSE_NONE;
       param->pokeID = pokeID;

      flowWk->HANDEX_Pop( param );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * ほごしょく
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Hogosyoku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Hogosyoku },    // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Hogosyoku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( work );
  GFL_UNUSED( myHandle );

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 基本は、BGに設定されているタイプになる
    const BgComponentData& bgData = flowWk->Hnd_GetBgComponentData();
    pml::PokeType type = static_cast<pml::PokeType>( bgData.hogoshokuType );

    // グラウンド状態があればそれで上書き
    BtlGround  grnd = flowWk->Hnd_GetGround();
    switch( grnd ){
    case BTL_GROUND_GRASS:    type = POKETYPE_KUSA;  break;
    case BTL_GROUND_ELEKI:    type = POKETYPE_DENKI; break;
    case BTL_GROUND_MIST:     type = POKETYPE_FAIRY; break;
    case BTL_GROUND_PHYCHO:   type = POKETYPE_ESPER; break;
    case BTL_GROUND_AQUA:     type = POKETYPE_MIZU;  break;
    case BTL_GROUND_ICE:      type = POKETYPE_KOORI; break;

    }

    {
      PokeTypePair  pairType = PokeTypePair_MakePure( type );
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

      if( bpp->GetPokeType() != pairType )
      {
        BTL_HANDEX_PARAM_CHANGE_TYPE* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_TYPE*>
                                                (flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_TYPE, pokeID ));
          param->next_type = pairType;
          param->exTypeCause = BTL_POKEPARAM::EXTYPE_CAUSE_NONE;
          param->pokeID = pokeID;
        flowWk->HANDEX_Pop( param );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * トリックルーム
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_TrickRoom( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL, handler_TrickRoom },    // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_TrickRoom( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( work );
  GFL_UNUSED( myHandle );

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( !flowWk->Hnd_CheckFieldEffect(FieldStatus::EFF_TRICKROOM) )
    {
      BTL_HANDEX_PARAM_ADD_FLDEFF* param;

      param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_FLDEFF *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_FLDEFF, pokeID ));
        param->effect = FieldStatus::EFF_TRICKROOM;
        param->cont = SICKCONT_MakeTurn( pokeID, 5 );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_TrickRoom );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
      flowWk->HANDEX_Pop( param );
    }
    else
    {
      BTL_HANDEX_PARAM_REMOVE_FLDEFF* param;
        param = reinterpret_cast<BTL_HANDEX_PARAM_REMOVE_FLDEFF *>( flowWk->HANDEX_Push( BTL_HANDEX_REMOVE_FLDEFF, pokeID ));
        param->effect = FieldStatus::EFF_TRICKROOM;
      flowWk->HANDEX_Pop( param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * じゅうりょく
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Juryoku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL,     handler_Juryoku },          // 未分類ワザハンドラ
  };

  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Juryoku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( work );
  GFL_UNUSED( myHandle );

  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (flowWk->Hnd_CheckFieldEffect(FieldStatus::EFF_JURYOKU) == false)
  ){
    BTL_HANDEX_PARAM_ADD_FLDEFF* param;

    param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_FLDEFF *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_FLDEFF, pokeID ));
      param->effect = FieldStatus::EFF_JURYOKU;
      param->cont = SICKCONT_MakeTurn( pokeID, 5 );
      param->exStr.type = BTL_STRTYPE_STD;
      param->exStr.ID = BTL_STRID_STD_Jyuryoku;
    flowWk->HANDEX_Pop( param );

    flowWk->HANDEX_PushRun( BTL_HANDEX_JURYOKU_CHECK, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * みずあそび
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_MizuAsobi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL, handler_MizuAsobi },    // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_MizuAsobi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( work );
  GFL_UNUSED( myHandle );

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_FLDEFF* param;

    param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_FLDEFF*>(flowWk->HANDEX_Push( BTL_HANDEX_ADD_FLDEFF, pokeID ));
      param->effect = FieldStatus::EFF_MIZUASOBI;
      param->cont = SICKCONT_MakeTurn( pokeID, 5 );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, BTL_STRID_STD_Mizuasobi );
    flowWk->HANDEX_Pop( param );
  }
}
//----------------------------------------------------------------------------------
/**
 * どろあそび
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_DoroAsobi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL, handler_DoroAsobi },    // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_DoroAsobi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( work );
  GFL_UNUSED( myHandle );

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_FLDEFF* param;
      param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_FLDEFF *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_FLDEFF, pokeID ));
      param->effect = FieldStatus::EFF_DOROASOBI;
      param->cont = SICKCONT_MakeTurn( pokeID, 5 );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, BTL_STRID_STD_Doroasobi );
    flowWk->HANDEX_Pop( param );
  }
}
//----------------------------------------------------------------------------------
/**
 * きりばらい
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Kiribarai( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Kiribarai               },  // 未分類ワザハンドラ
//    { BTL_EVENT_MIGAWARI_EXCLUDE,  handler_Common_MigawariEffctive },  // みがわりチェック

  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Kiribarai( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( work );
  GFL_UNUSED( myHandle );


  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );

    {
      BTL_HANDEX_PARAM_RANK_EFFECT* rank_param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
        rank_param->rankType = pml::wazadata::RANKEFF_AVOID;
        rank_param->rankVolume = -1;
        rank_param->fAlmost = true;
        rank_param->fWazaEffect = true;
        rank_param->bMigawariCheck = true;
        rank_param->poke_cnt = 1;
        rank_param->pokeID[0] = targetPokeID;
      flowWk->HANDEX_Pop( rank_param );
    }

    // 技ターゲット側のサイドエフェクトを解除
    {
      BTL_HANDEX_PARAM_SIDEEFF_REMOVE* remove_param = reinterpret_cast<BTL_HANDEX_PARAM_SIDEEFF_REMOVE*>(flowWk->HANDEX_Push( BTL_HANDEX_SIDEEFF_REMOVE, pokeID ));
        remove_param->side = flowWk->Hnd_GetMainModule()->PokeIDtoSide( targetPokeID );
        calc::BITFLG_Construction( remove_param->flags, sizeof(remove_param->flags) );
        calc::BITFLG_Set( remove_param->flags, BTL_SIDEEFF_REFRECTOR );
        calc::BITFLG_Set( remove_param->flags, BTL_SIDEEFF_HIKARINOKABE );
        calc::BITFLG_Set( remove_param->flags, BTL_SIDEEFF_SIROIKIRI );
        calc::BITFLG_Set( remove_param->flags, BTL_SIDEEFF_SINPINOMAMORI );
        calc::BITFLG_Set( remove_param->flags, BTL_SIDEEFF_MAKIBISI );
        calc::BITFLG_Set( remove_param->flags, BTL_SIDEEFF_DOKUBISI );
        calc::BITFLG_Set( remove_param->flags, BTL_SIDEEFF_STEALTHROCK );
        calc::BITFLG_Set( remove_param->flags, BTL_SIDEEFF_NEBANEBANET );
        calc::BITFLG_Set( remove_param->flags, BTL_SIDEEFF_AURORAVEIL );
      flowWk->HANDEX_Pop( remove_param );
    }

    // 自分側のサイドエフェクトを解除
    {
      BTL_HANDEX_PARAM_SIDEEFF_REMOVE* remove_param = reinterpret_cast<BTL_HANDEX_PARAM_SIDEEFF_REMOVE*>(flowWk->HANDEX_Push( BTL_HANDEX_SIDEEFF_REMOVE, pokeID ));
        remove_param->side = flowWk->Hnd_GetMainModule()->PokeIDtoSide( pokeID );
        calc::BITFLG_Construction( remove_param->flags, sizeof(remove_param->flags) );
        calc::BITFLG_Set( remove_param->flags, BTL_SIDEEFF_MAKIBISI );
        calc::BITFLG_Set( remove_param->flags, BTL_SIDEEFF_DOKUBISI );
        calc::BITFLG_Set( remove_param->flags, BTL_SIDEEFF_STEALTHROCK );
        calc::BITFLG_Set( remove_param->flags, BTL_SIDEEFF_NEBANEBANET );

      flowWk->HANDEX_Pop( remove_param );
    }

//    EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, false );
  }
}
//----------------------------------------------------------------------------------
/**
 * かわらわり・サイコファング
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Kawarawari( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC1,     handler_Kawarawari_DmgProc1     },  // ダメージ計算ハンドラ
    { BTL_EVENT_WAZA_DMG_PROC_END,  handler_Kawarawari_DmgProcEnd   },  // ダメージ計算終了ハンドラ
    { BTL_EVENT_WAZA_DMG_DETERMINE, handler_Kawarawari_DmgDetermine },  // ダメージ確定ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージ計算ハンドラ
void handler_Kawarawari_DmgProc1( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( flowWk );
  GFL_UNUSED( work );

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID ){
    EVENT_FACTOR_AttachSkipCheckHandler( myHandle, handler_Kawarawari_SkipCheck );
  }
}
// ダメージ計算終了ハンドラ
void handler_Kawarawari_DmgProcEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( flowWk );
  GFL_UNUSED( work );

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID ){
    EVENT_FACTOR_DettachSkipCheckHandler( myHandle );
  }
}
// ダメージ確定ハンドラ
void handler_Kawarawari_DmgDetermine( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( myHandle );
  GFL_UNUSED( work );

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    BtlSide side = flowWk->Hnd_GetMainModule()->PokeIDtoSide( targetPokeID );

    if( handler::side::IsExist(side, BTL_SIDEEFF_REFRECTOR)
    ||  handler::side::IsExist(side, BTL_SIDEEFF_HIKARINOKABE)
    ||  handler::side::IsExist(side, BTL_SIDEEFF_AURORAVEIL)
    ){
      BTL_HANDEX_PARAM_SIDEEFF_REMOVE* param;
      BTL_HANDEX_PARAM_SET_EFFECT_IDX* eff_param;

      param = reinterpret_cast<BTL_HANDEX_PARAM_SIDEEFF_REMOVE*>(flowWk->HANDEX_Push( BTL_HANDEX_SIDEEFF_REMOVE, pokeID ));
        param->side = side;
        calc::BITFLG_Construction( param->flags, sizeof(param->flags) );
        calc::BITFLG_Set( param->flags, BTL_SIDEEFF_REFRECTOR );
        calc::BITFLG_Set( param->flags, BTL_SIDEEFF_HIKARINOKABE );
        calc::BITFLG_Set( param->flags, BTL_SIDEEFF_AURORAVEIL );
      flowWk->HANDEX_Pop( param );

      eff_param = reinterpret_cast<BTL_HANDEX_PARAM_SET_EFFECT_IDX*>
                    (flowWk->HANDEX_Push( BTL_HANDEX_SET_EFFECT_IDX, pokeID ));
        eff_param->effIndex = BTLV_WAZAEFF_KAWARAWARI_BREAK;
      flowWk->HANDEX_Pop( eff_param );
    }
  }
}
// スキップチェックハンドラ
bool handler_Kawarawari_SkipCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, BtlEventFactorType factorType, BtlEventType eventType, u16 subID, u8 dependID )
{
  GFL_UNUSED( myHandle );
  GFL_UNUSED( flowWk );
  GFL_UNUSED( eventType );
  GFL_UNUSED( dependID );

  if( factorType == BTL_EVENT_FACTOR_SIDE )
  {
    if( (subID == BTL_SIDEEFF_REFRECTOR)
    ||  (subID == BTL_SIDEEFF_HIKARINOKABE)
    ||  (subID == BTL_SIDEEFF_AURORAVEIL)
    ){
      return true;
    }
  }
  return false;
}



//----------------------------------------------------------------------------------
/**
 * とびげり・とびひざげり
 *
 * ワザが外れた時、または効果がなかった時に、自分のHPの1/3ダメージを受ける
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Tobigeri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_NO_EFFECT, handler_Tobigeri_NoEffect  }, // 効果なかったハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Tobigeri_Avoid( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    common_TobigeriReaction( myHandle, flowWk, pokeID, work );
  }
}
void handler_Tobigeri_NoEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    common_TobigeriReaction( myHandle, flowWk, pokeID, work );
  }
}
/**
 *  とびげり反動ダメージ処理共通
 */
void  common_TobigeriReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( myHandle );
  GFL_UNUSED( work );

  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  u32 damage = calc::QuotMaxHP( bpp, 2 );

  BTL_HANDEX_PARAM_DAMAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_DAMAGE*>
                                    (flowWk->HANDEX_Push( BTL_HANDEX_DAMAGE, pokeID ));
    param->pokeID = pokeID;
    param->damage = damage;
    param->damageCause = DAMAGE_CAUSE_TOBIGERI_FAILED;
    param->damageCausePokeID = pokeID;
    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_HazureJibaku );
    HANDEX_STR_AddArg( &param->exStr, pokeID );
  flowWk->HANDEX_Pop( param );

}
//----------------------------------------------------------------------------------
/**
 * ものまね
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Monomane( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,   handler_Monomane    }, // 未分類ワザハンドラ
//    { BTL_EVENT_DECREMENT_PP_VOLUME, handler_Monomane_PP }, // PP消費チェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 未分類ワザハンドラ
void handler_Monomane( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( work );

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* self = flowWk->Hnd_GetPokeParam( pokeID );

    if( !self->HENSIN_Check() )
    {
      const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_TARGET1) );
      WazaID waza = target->GetPrevWazaID( );

      {
        WazaID orgWaza = target->GetPrevOrgWazaID();
        if( orgWaza != waza ){
          waza = orgWaza;
        }
      }

      if( (waza != WAZANO_NULL)
      &&  (!tables::IsMatchMonomaneFail(waza))
      &&  (!target->IsPrevWazaZenryoku())
      &&  (self->WAZA_SearchIdx(waza) == PTL_WAZA_MAX)
//      &&  (counter > 0)
      ){
        u8 wazaIdx = self->WAZA_SearchIdx( static_cast<WazaNo>(EVENT_FACTOR_GetSubID(myHandle)) );
        if( wazaIdx != PTL_WAZA_MAX )
        {
          BTL_HANDEX_PARAM_UPDATE_WAZA*  param;
          BTL_HANDEX_PARAM_MESSAGE* msg_param;

          param = reinterpret_cast<BTL_HANDEX_PARAM_UPDATE_WAZA*>
                    (flowWk->HANDEX_Push( BTL_HANDEX_UPDATE_WAZA, pokeID ));
            param->pokeID = pokeID;
            param->wazaIdx = wazaIdx;
            param->waza = waza;
            param->ppMax = 0;
            param->fPermanent = false;
          flowWk->HANDEX_Pop( param );

          msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
            HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Monomane );
            HANDEX_STR_AddArg( &msg_param->str, pokeID );
            HANDEX_STR_AddArg( &msg_param->str, waza );
          flowWk->HANDEX_Pop( msg_param );

        }
      }
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * スケッチ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Sketch( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Sketch },    // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Sketch( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( work );

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* self = flowWk->Hnd_GetPokeParam( pokeID );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_TARGET1) );
    WazaID waza = target->GetPrevWazaID( );

    {
      WazaID orgWaza = target->GetPrevOrgWazaID();
      if( orgWaza != waza ){
        waza = orgWaza;
      }
    }

    if( (waza != WAZANO_NULL)
    &&  (waza != WAZANO_SUKETTI)
    &&  (waza != WAZANO_WARUAGAKI)
    &&  (waza != WAZANO_OSYABERI)
    &&  (!self->WAZA_IsUsable(waza))
    &&  (!self->HENSIN_Check())
    &&  (!target->IsPrevWazaZenryoku())
    ){
      u8 wazaIdx = self->WAZA_SearchIdx( static_cast<WazaNo>(EVENT_FACTOR_GetSubID(myHandle)) );
      if( wazaIdx != PTL_WAZA_MAX )
      {
        BTL_HANDEX_PARAM_UPDATE_WAZA*  param;
        BTL_HANDEX_PARAM_MESSAGE* msg_param;

        param = reinterpret_cast<BTL_HANDEX_PARAM_UPDATE_WAZA*>
                  (flowWk->HANDEX_Push( BTL_HANDEX_UPDATE_WAZA, pokeID ));
          param->pokeID = pokeID;
          param->wazaIdx = wazaIdx;
          param->waza = waza;
          param->ppMax = 0;
          param->fPermanent = true;
        flowWk->HANDEX_Pop( param );

        msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
          HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Monomane );
          HANDEX_STR_AddArg( &msg_param->str, pokeID );
          HANDEX_STR_AddArg( &msg_param->str, waza );
        flowWk->HANDEX_Pop( msg_param );

      }
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * このゆびとまれ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_KonoyubiTomare( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD, handler_KonoyubiTomare_ExeCheck  },  // ワザ出し成否チェック
    { BTL_EVENT_UNCATEGORIZE_WAZA,      handler_KonoyubiTomare_Exe       },  // 未分類ワザ実行
    { BTL_EVENT_TEMPT_TARGET,           handler_KonoyubiTomare_TemptTarget    },  // ターゲット決定
    { BTL_EVENT_TURNCHECK_BEGIN,        handler_KonoyubiTomare_TurnCheck },  // ターンチェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザ出し成否チェック：シングルなら失敗する
void handler_KonoyubiTomare_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( myHandle );
  GFL_UNUSED( work );

  if( flowWk->Hnd_GetRule() == BTL_RULE_SINGLE )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
    {
      if( EVENTVAR_GetValue(BTL_EVAR_FAIL_CAUSE) == ServerFlow::SV_WAZAFAIL_NULL ){
        EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
      }
    }
  }
}
// ワザ出し成否確定：メッセージ表示
void handler_KonoyubiTomare_Exe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( myHandle );

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE*>
                                        (flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Konoyubi );
      HANDEX_STR_AddArg( &param->str, pokeID );
    flowWk->HANDEX_Pop( param );

    work[ WORKIDX_STICK ] = 1;
  }
}
void handler_KonoyubiTomare_TemptTarget( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  HandCommon_KonoyubiTomare_TemptTarget( myHandle, flowWk, pokeID, TEMPT_TARGET_PRIORITY_WAZA_1, TEMPT_TARGET_CAUSE_OTHER, work );
}
void handler_KonoyubiTomare_TurnCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  removeHandlerForce( pokeID, static_cast<WazaNo>(EVENT_FACTOR_GetSubID(myHandle)) );
}

//----------------------------------------------------------------------------------
/**
 * いかりのこな
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Ikarinokona( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD, handler_KonoyubiTomare_ExeCheck  },  // ワザ出し成否チェック
    { BTL_EVENT_UNCATEGORIZE_WAZA,      handler_KonoyubiTomare_Exe       },  // 未分類ワザ実行
    { BTL_EVENT_TEMPT_TARGET,           handler_Ikarinokona_TemptTarget  },  // ターゲット決定
    { BTL_EVENT_TURNCHECK_BEGIN,        handler_KonoyubiTomare_TurnCheck },  // ターンチェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

/**
 * @brief 「いかりのこな」ターゲット決定イベント
 *
 *        技のターゲットを自分に向ける。
 *        ただし、「くさ」タイプの相手や、特性「ぼうじん」の相手、道具「ぼうじんゴーグル」を持つ相手には無効。
 *
 * @param myHandle
 * @param flowWk
 * @param pokeID      「いかりのこな」を使用しているポケモンのID
 * @param factorWork   イベントファクターの作業領域
 */
void handler_Ikarinokona_TemptTarget( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  u8 atkPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
  const BTL_POKEPARAM* attacker = flowWk->Hnd_GetPokeParam( atkPokeID );
  if( ( attacker->IsMatchType( POKETYPE_KUSA ) ) ||
      ( attacker->GetValue( BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE ) == TOKUSEI_BOUZIN ) ||
      ( flowWk->Hnd_GetUsableItem( atkPokeID ) == ITEM_BOUZINGOOGURU ) )
  {
    return;
  }
  HandCommon_KonoyubiTomare_TemptTarget( myHandle, flowWk, pokeID, TEMPT_TARGET_PRIORITY_WAZA_1, TEMPT_TARGET_CAUSE_OTHER, work );
}

//----------------------------------------------------------------------------------
/**
 * リフレッシュ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Refresh( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Refresh },    // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

void handler_Refresh( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    PokeSick sick = bpp->GetPokeSick();

    if( (sick == pml::pokepara::SICK_DOKU)
    ||  (sick == pml::pokepara::SICK_MAHI)
    ||  (sick == pml::pokepara::SICK_YAKEDO)
    ){
      BTL_HANDEX_PARAM_CURE_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
        param->sickCode = static_cast<BtlWazaSickEx>(sick);
        param->pokeID[0] = pokeID;
        param->poke_cnt = 1;
      flowWk->HANDEX_Pop( param );

      // @@@ メッセージいるか？
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * クモのす、くろいまなざし、とおせんぼう
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_KumoNoSu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2,   handler_KumoNoSu_NoEffCheck }, // 無効チェックハンドラ
    { BTL_EVENT_UNCATEGORIZE_WAZA,   handler_KumoNoSu            }, // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 無効チェック有無ハンドラ
void handler_KumoNoSu_NoEffCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetID );
    if( target->IsMatchType(POKETYPE_GHOST) ){
      EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, true );
    }
  }
}
// 未分類ワザハンドラ
void handler_KumoNoSu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u32  targetCnt  = EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
    for(u32 i=0; i<targetCnt; ++i )
    {
      u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( targetPokeID );
      if( !(bpp->CheckSick(pml::wazadata::WAZASICK_TOOSENBOU))
      &&  !(bpp->IsMatchType(POKETYPE_GHOST))
      ){
        BTL_HANDEX_PARAM_ADD_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));

          param->pokeID = targetPokeID;
          param->sickID = pml::wazadata::WAZASICK_TOOSENBOU;
          param->sickCont = calc::MakeWazaSickCont_Poke( pokeID, pokeID );
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_CantEscWaza );
          HANDEX_STR_AddArg( &param->exStr,targetPokeID );

        flowWk->HANDEX_Pop( param );
      }
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * くろいきり
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_KuroiKiri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL, handler_KuroiKiri },    // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

void handler_KuroiKiri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_RESET_RANK* reset_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    reset_param = reinterpret_cast<BTL_HANDEX_PARAM_RESET_RANK *>( flowWk->HANDEX_Push( BTL_HANDEX_RESET_RANK, pokeID ));
    {
      BtlPokePos myPos = flowWk->Hnd_GetExistFrontPokePos( pokeID );
      BtlExPos   expos = EXPOS_MAKE( BTL_EXPOS_FULL_ALL, myPos );
      reset_param->poke_cnt = flowWk->Hnd_ExpandPokeID( expos, reset_param->pokeID );
    }
    flowWk->HANDEX_Pop( reset_param );

    msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_STD, BTL_STRID_STD_RankReset );
    flowWk->HANDEX_Pop( msg_param );
  }
}
//----------------------------------------------------------------------------------
/**
 * はねる
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Haneru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD, handler_Haneru_CheckFail  },  // ワザ出し成否チェックハンドラ
    { BTL_EVENT_UNCATEGORIZE_WAZA,      handler_Haneru            },  // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// ワザ出し成否チェックハンドラ
void handler_Haneru_CheckFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // 場にじゅうりょく効果が存在するなら失敗
    if( flowWk->Hnd_CheckFieldEffect( FieldStatus::EFF_JURYOKU ) )
    {
      EVENTVAR_RewriteValue(BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_JURYOKU);
    }
  }
}

 // 未分類ワザハンドラ
void handler_Haneru( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param;

    param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_NotHappen );
    flowWk->HANDEX_Pop( param );
  }
}
//----------------------------------------------------------------------------------
/**
 * おいわい
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Oiwai( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Oiwai },    // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

void handler_Oiwai( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, false );
    flowWk->HANDEX_PushRun( BTL_HANDEX_WAZAEFFECT_ENABLE, pokeID );

    u8 clientID = MainModule::PokeIDtoClientID( pokeID );
    if( !flowWk->IsClientTrainerExist(clientID) ){
      clientID = BTL_CLIENT_PLAYER;
    }

    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_Congratulations );
      HANDEX_STR_AddArg( &param->str, clientID );
    flowWk->HANDEX_Pop( param );
  }
}
//----------------------------------------------------------------------------------
/**
 * てをつなぐ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_TeWoTunagu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_TeWoTunagu },    // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

void handler_TeWoTunagu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BtlRule  rule = flowWk->Hnd_GetRule();
    if( rule == BTL_RULE_DOUBLE )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, false );
      flowWk->HANDEX_PushRun( BTL_HANDEX_WAZAEFFECT_ENABLE, pokeID );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * のろい
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Noroi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_GET_ZENRYOKU_EFFECT,   handler_Noroi_GetZenryokuEffect }, // 全力効果取得
    { BTL_EVENT_WAZA_PARAM,            handler_Noroi_WazaParam         }, // ワザパラメータ決定ハンドラ
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Noroi                   }, // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

/**
 * @brief 全力効果取得
 *        技を使用するポケモンのタイプに応じて、全力効果が変わる
 *
 * @param myHandle
 * @param flowWk
 * @param pokeID   技を使用したポケモンのID
 * @param work     イベントファクタの作業領域
 *
 * BTL_EVAR_POKEID       [in]  処理対象のポケモンのID
 * BTL_EVAR_ZENRYOKU_EFFECT  [out] 全力効果
 */
void handler_Noroi_GetZenryokuEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) != pokeID ) {
    return;
  }
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  ZenryokuWazaEffect zenryokuEffect = ( bpp->IsMatchType( POKETYPE_GHOST ) ) ? ( pml::wazadata::ZENRYOKUWAZA_EFFECT_RECOVER_HP ) : ( pml::wazadata::ZENRYOKUWAZA_EFFECT_ATTACK_UP1 );
  EVENTVAR_RewriteValue( BTL_EVAR_ZENRYOKU_EFFECT, zenryokuEffect );
}

// ワザパラメータ決定ハンドラ
void handler_Noroi_WazaParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    WazaTarget targetType = calc::GetNoroiTargetType( bpp );
    EVENTVAR_RewriteValue( BTL_EVAR_TARGET_TYPE, targetType );
  }
}
// 未分類ワザハンドラ
void handler_Noroi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u8 effIndex = BTLV_WAZAEFF_NOROI_NORMAL;

  // ゴーストタイプ
    if( bpp->IsMatchType(POKETYPE_GHOST) )
    {
      int restHP = bpp->GetValue( BTL_POKEPARAM::BPP_HP );
      if( restHP > 0 )
      {
        u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
        if( targetPokeID != BTL_POKEID_NULL )
        {
          BTL_HANDEX_PARAM_ADD_SICK* param;
          BTL_HANDEX_PARAM_SHIFT_HP* hp_param;

          param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
            param->pokeID = targetPokeID;
            param->sickID = pml::wazadata::WAZASICK_NOROI;
            param->sickCont = SICKCONT_MakePermanent( pokeID );
            HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Noroi );
            HANDEX_STR_AddArg( &param->exStr, pokeID );
            HANDEX_STR_AddArg( &param->exStr, targetPokeID );
          flowWk->HANDEX_Pop( param );

          hp_param = reinterpret_cast<BTL_HANDEX_PARAM_SHIFT_HP *>( flowWk->HANDEX_Push( BTL_HANDEX_SHIFT_HP, pokeID ));
            hp_param->pokeID[0] = pokeID;
            hp_param->volume[0] = (int)(calc::QuotMaxHP( bpp, 2 )) * -1;
            hp_param->poke_cnt = 1;
            hp_param->header.failSkipFlag = true;
            hp_param->damageCause = DAMAGE_CAUSE_NOROI;
          flowWk->HANDEX_Pop( hp_param );
        }
      }
      effIndex = BTLV_WAZAEFF_NOROI_GHOST;
  // ゴースト以外タイプ
    }
    else
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* param;

      param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
        param->poke_cnt = 1;
        param->pokeID[0] = pokeID;
        param->rankType = pml::wazadata::RANKEFF_AGILITY;
        param->rankVolume = -1;
        param->fWazaEffect = true;
      flowWk->HANDEX_Pop( param );

      param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
        param->poke_cnt = 1;
        param->pokeID[0] = pokeID;
        param->rankType  = pml::wazadata::RANKEFF_ATTACK;
        param->rankVolume = 1;
        param->fWazaEffect = true;
      flowWk->HANDEX_Pop( param );

      param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
        param->poke_cnt = 1;
        param->pokeID[0] = pokeID;
        param->rankType  = pml::wazadata::RANKEFF_DEFENCE;
        param->rankVolume = 1;
        param->fWazaEffect = true;
      flowWk->HANDEX_Pop( param );
    }

    flowWk->Hnd_SetWazaEffectIndex( effIndex );
  }
}

//----------------------------------------------------------------------------------
/**
 * でんじは
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Denjiha( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
//    { BTL_EVENT_NOEFFECT_CHECK_L2,   handler_Denjiha_NoEff },
    { BTL_EVENT_CHECK_AFFINITY_ENABLE, handler_Denjiha }, // タイプ相性による無効チェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// タイプ相性による無効チェック
void handler_Denjiha( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // タイプ相性による無効チェックを行う
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
  }
}
//----------------------------------------------------------------------------------
/**
 * なやみのタネ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_NayamiNoTane( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2, handler_NayamiNoTane_NoEff },    // 無効化チェックレベル２ハンドラ
    { BTL_EVENT_UNCATEGORIZE_WAZA,   handler_NayamiNoTane       },    // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_NayamiNoTane_NoEff( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetPokeID );

    // なまけ，ふみん持ちには効かない
    if( (target->GetValue( BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE) == TOKUSEI_NAMAKE)
    ||  (target->GetValue( BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE) == TOKUSEI_HUMIN)
    ){
      EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, true );
    }
  }
}
void handler_NayamiNoTane( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 対象のとくせいを「ふみん」に書き換え
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );

    BTL_HANDEX_PARAM_CHANGE_TOKUSEI* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_TOKUSEI *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_TOKUSEI, pokeID ));
      param->pokeID = targetPokeID;
      param->tokuseiID = TOKUSEI_HUMIN;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_TokuseiChange );
      HANDEX_STR_AddArg( &param->exStr, param->pokeID );
      HANDEX_STR_AddArg( &param->exStr, param->tokuseiID );
    flowWk->HANDEX_Pop( param );
  }
}

//----------------------------------------------------------------------------------
/**
 * ゆめくい
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Yumekui( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2, handler_Yumekui },    // 無効化チェックレベル２ハンドラ
//    { BTL_EVENT_MIGAWARI_EXCLUDE,  handler_Yumekui_MigawariCheck },   // みがわりチェック

  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

void handler_Yumekui( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetPokeID );

    if( !target->CheckNemuri( BTL_POKEPARAM::NEMURI_CHECK_INCLUDE_ZETTAINEMURI ) )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, true );
    }
  }
}
// みがわりチェック
void handler_Yumekui_MigawariCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, true );
  }
}

//----------------------------------------------------------------------------------
/**
 * ゆうわく
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Yuwaku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2, handler_Yuwaku },    // 無効化チェックレベル２ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Yuwaku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetPokeID );
    const BTL_POKEPARAM* me = flowWk->Hnd_GetPokeParam( pokeID );

    u8 my_sex = me->GetValue( BTL_POKEPARAM::BPP_SEX );
    u8 target_sex = target->GetValue( BTL_POKEPARAM::BPP_SEX );

    if( (my_sex == pml::SEX_UNKNOWN)
    ||  (target_sex == pml::SEX_UNKNOWN)
    ||  (my_sex == target_sex)
    ){
      EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, true );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * トライアタック
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_TriAttack( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK, handler_TriAttack },    // 追加効果による状態異常チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_TriAttack( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    static const PokeSick sick_tbl[] = {
      pml::pokepara::SICK_MAHI, pml::pokepara::SICK_YAKEDO, pml::pokepara::SICK_KOORI
    };
    PokeSick sick;
    BTL_SICKCONT cont;
    u8 rand = calc::GetRand( GFL_NELEMS(sick_tbl) );

    #if 0
    {
      u32 cnt[3];
      u32 i = 0;
      u32 max = 50;
      if( GFL_UI_KEY_GetCont() & PAD_BUTTON_X ){
        max = 10000;
      }
      if( GFL_UI_KEY_GetCont() & PAD_BUTTON_Y ){
        max = 30000;
      }
      cnt[0] = cnt[1] = cnt[2] = 0;
      while(1){
        rand = calc::GetRand( GFL_NELEMS(sick_tbl) );
        if( calc::GetRand(100) < 20 ){
          cnt[ rand ]++;
          if( ++i >= max ){
            break;
          }
        }
      }
      for(i=0; i<GFL_NELEMS(cnt); ++i){
        TAYA_Printf(" cnt[%d] = %d\n", i, cnt[i]);
      }
    }
    #endif

    sick = sick_tbl[ rand ];
    cont = calc::MakeDefaultPokeSickCont( sick, pokeID );

    EVENTVAR_RewriteValue( BTL_EVAR_SICKID, sick );
    EVENTVAR_RewriteValue( BTL_EVAR_ADD_PER, 20 );
    {
      BTL_SICKCONT* sickcont = reinterpret_cast<BTL_SICKCONT*>( EVENTVAR_GetValue( BTL_EVAR_SICK_CONT_ADRS ) );
      sickcont->raw = cont.raw;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ねっとう
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Nettou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL, handler_Nettou },    // ダメージ反応ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// ダメージ反応ハンドラ
void handler_Nettou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u32 targetCnt = EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
    for(u32 i=0; i<targetCnt; ++i)
    {
      u8 targetPokeID = EVENTVAR_GetValue( (BtlEvVarLabel)(BTL_EVAR_POKEID_TARGET1 + i) );
      const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetPokeID );
      if( !target->IsDead()
      &&  target->CheckSick(pml::wazadata::WAZASICK_KOORI)
      ){
        BTL_HANDEX_PARAM_CURE_SICK* cure_param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
          cure_param->sickCode = static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_KOORI);
          cure_param->poke_cnt = 1;
          cure_param->pokeID[0] = targetPokeID;
        flowWk->HANDEX_Pop( cure_param );
      }
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * うたかたのアリア
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_UtakatanoAria( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL, handler_UtakatanoAria },    // ダメージ反応ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

/**
 * @brief ダメージ反応ハンドラ
 *
 * 技を受けたポケモンは「やけど」が治る
 */
void handler_UtakatanoAria( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID )
  {
    return;
  }
  // 相手が「りんぷん」または自分が「ちからづく」なら効果なし
  if( EVENTVAR_GetValue(BTL_EVAR_RINPUNGUARD_FLG)
  ||  EVENTVAR_GetValue(BTL_EVAR_TIKARAZUKU_FLG)
  ){
    return;
  }

  const u32 targetCnt = EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
  for( u32 i=0; i<targetCnt; ++i )
  {
    const u8 targetPokeID = EVENTVAR_GetValue( (BtlEvVarLabel)(BTL_EVAR_POKEID_TARGET1 + i) );

    BTL_HANDEX_PARAM_CURE_SICK* cure_param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
      cure_param->sickCode  = static_cast<BtlWazaSickEx>( pml::wazadata::WAZASICK_YAKEDO );
      cure_param->poke_cnt  = 1;
      cure_param->pokeID[0] = targetPokeID;
    flowWk->HANDEX_Pop( cure_param );
  }
}

//----------------------------------------------------------------------------------
/**
 * ひみつのちから
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_HimituNoTikara( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL, handler_HimituNoTikara },    // ダメージ反応ハンドラ
    { BTL_EVENT_WAZA_EXE_START,          handler_HimituNoTikara_Start },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
/**
 *  ひみつのちから効果種類
 */
enum {
  HIMITSU_METHOD_SICK,    ///< 状態異常
  HIMITSU_METHOD_RANK,    ///< 能力ランク効果
  HIMITSU_METHOD_SHRINK,  ///< ひるませる
};

/**
 * 「ひみつのちから」共通処理：BGから追加効果種類、引数、エフェクトIndexナンバー取得
 *
 * @retval  再生する技エフェクトの技Ｎｏ．
 */
WazaNo common_HimituNoTikara_getParams( ServerFlow* flowWk, u8* dstMethod, u8* dstMethodArg )
{
  // BGに設定された技エフェクトを再生する
  const BgComponentData& bgData = flowWk->Hnd_GetBgComponentData();
  WazaNo wazaEff = static_cast<WazaNo>( bgData.himitinotikaraEffect );


  // BGに設定された「ひみつのちから」効果を展開
  u8 method = 0;
  u8 method_arg = 0;

  switch( bgData.himitinotikaraType )
  {
  case HIMITUNOTIKARA_TYPE_ATTACK:
    method     = HIMITSU_METHOD_RANK;
    method_arg = BTL_POKEPARAM::BPP_ATTACK_RANK;
    break;

  case HIMITUNOTIKARA_TYPE_DEFENCE:
    method     = HIMITSU_METHOD_RANK;
    method_arg = BTL_POKEPARAM::BPP_DEFENCE_RANK;
    break;

  case HIMITUNOTIKARA_TYPE_SP_ATK:
    method     = HIMITSU_METHOD_RANK;
    method_arg = BTL_POKEPARAM::BPP_SP_ATTACK_RANK;
    break;

  case HIMITUNOTIKARA_TYPE_SP_DEF:
    method     = HIMITSU_METHOD_RANK;
    method_arg = BTL_POKEPARAM::BPP_SP_DEFENCE_RANK;
    break;

  case HIMITUNOTIKARA_TYPE_AGIRITY:
    method     = HIMITSU_METHOD_RANK;
    method_arg = BTL_POKEPARAM::BPP_AGILITY_RANK;
    break;

  case HIMITUNOTIKARA_TYPE_HIT_RATIO:
    method     = HIMITSU_METHOD_RANK;
    method_arg = BTL_POKEPARAM::BPP_HIT_RATIO;
    break;

  case HIMITUNOTIKARA_TYPE_DODGE:
    method     = HIMITSU_METHOD_RANK;
    method_arg = BTL_POKEPARAM::BPP_AVOID_RATIO;
    break;

  case HIMITUNOTIKARA_TYPE_DOKU:
    method     = HIMITSU_METHOD_SICK;
    method_arg = pml::wazadata::WAZASICK_DOKU;
    break;

  case HIMITUNOTIKARA_TYPE_MAHI:
    method     = HIMITSU_METHOD_SICK;
    method_arg = pml::wazadata::WAZASICK_MAHI;
    break;

  case HIMITUNOTIKARA_TYPE_NEMURI:
    method     = HIMITSU_METHOD_SICK;
    method_arg = pml::wazadata::WAZASICK_NEMURI;
    break;

  case HIMITUNOTIKARA_TYPE_KOORI:
    method     = HIMITSU_METHOD_SICK;
    method_arg = pml::wazadata::WAZASICK_KOORI;
    break;

  case HIMITUNOTIKARA_TYPE_YAKEDO:
    method     = HIMITSU_METHOD_SICK;
    method_arg = pml::wazadata::WAZASICK_YAKEDO;
    break;

  case HIMITUNOTIKARA_TYPE_KONRAN:
    method     = HIMITSU_METHOD_SICK;
    method_arg = pml::wazadata::WAZASICK_KONRAN;
    break;

  case HIMITUNOTIKARA_TYPE_HIRUMI:
    method     = HIMITSU_METHOD_SHRINK;
    method_arg = 0;
    break;

  default:
    GFL_ASSERT(0);
    method     = HIMITSU_METHOD_RANK;
    method_arg = BTL_POKEPARAM::BPP_ATTACK_RANK;
    break;
  }



  // グラウンド状態があれば、それで上書き
  BtlGround  grnd = flowWk->Hnd_GetGround();
  switch( grnd ){
  case BTL_GROUND_GRASS:
    method     = HIMITSU_METHOD_SICK;
    method_arg = pml::wazadata::WAZASICK_NEMURI;
    wazaEff    = WAZANO_NIIDORUAAMU;
    break;

  case BTL_GROUND_MIST:
    method     = HIMITSU_METHOD_RANK;
    method_arg = BTL_POKEPARAM::BPP_SP_ATTACK_RANK;
    wazaEff    = WAZANO_YOUSEINOKAZE;
    break;

  case BTL_GROUND_ELEKI:
    method     = HIMITSU_METHOD_SICK;
    method_arg = pml::wazadata::WAZASICK_MAHI;
    wazaEff    = WAZANO_DENKISYOKKU;
    break;

  case BTL_GROUND_PHYCHO:
    method     = HIMITSU_METHOD_RANK;
    method_arg = BTL_POKEPARAM::BPP_AGILITY_RANK;
    wazaEff    = WAZANO_NENRIKI;
    break;
  }


  if( dstMethod ){
    *dstMethod = method;
  }
  if( dstMethodArg ){
    *dstMethodArg = method_arg;
  }

  return wazaEff;
}
// ワザ出し開始
void handler_HimituNoTikara_Start( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    WazaNo effectWazaNo = common_HimituNoTikara_getParams( flowWk, NULL, NULL );
    EVENTVAR_RewriteValue( BTL_EVAR_EFFECT_WAZAID, effectWazaNo );
  }
}

// ダメージ反応ハンドラ
void handler_HimituNoTikara( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  &&  (EVENTVAR_GetValue(BTL_EVAR_RINPUNGUARD_FLG) == false)
  ){
    u8  targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    if( flowWk->Hnd_CheckOccurAdditionalEffect(pokeID, targetPokeID, 30) )
    {
      const BTL_POKEPARAM* attacker = flowWk->Hnd_GetPokeParam( pokeID );
      u8   method, method_arg;

      common_HimituNoTikara_getParams( flowWk, &method, &method_arg );

      switch( method ){
      case HIMITSU_METHOD_SICK:
        {
          BTL_HANDEX_PARAM_ADD_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
            param->pokeID = targetPokeID;
            param->sickID = (WazaSick)method_arg;
            calc::MakeDefaultWazaSickCont( param->sickID, attacker, &param->sickCont );
          flowWk->HANDEX_Pop( param );
        }
        break;

      case HIMITSU_METHOD_RANK:
        {
          BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
            param->poke_cnt = 1;
            param->pokeID[0] = targetPokeID;
            param->rankType = static_cast<WazaRankEffect>(method_arg);
            param->rankVolume = -1;
            param->fWazaEffect = true;
          flowWk->HANDEX_Pop( param );
        }
        break;

      case HIMITSU_METHOD_SHRINK:
        {
          BTL_HANDEX_PARAM_ADD_SHRINK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SHRINK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SHRINK, pokeID ));
            param->pokeID = targetPokeID;
            param->per = 100;
          flowWk->HANDEX_Pop( param );
        }
        break;
      }
    }/* if( rnd < 30) */
  }
}
//----------------------------------------------------------------------------------
/**
 * おしゃべり
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Osyaberi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK, handler_Osyaberi },    // 追加効果による状態異常チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Osyaberi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->GetMonsNo() != MONSNO_PERAPPU )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * しめつける・まきつく・ほのおのうず等々
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Makituku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK,     handler_Makituku },     // 追加効果による状態異常チェックハンドラ
    { BTL_EVENT_WAZASICK_STR, handler_Makituku_Str }, // 状態異常追加時の文字列セットハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Makituku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_SICKCONT* sickcont = reinterpret_cast<BTL_SICKCONT*>( EVENTVAR_GetValue( BTL_EVAR_SICK_CONT_ADRS ) );
    WazaID waza = static_cast<WazaNo>(EVENT_FACTOR_GetSubID( myHandle ));
    SICKCONT_AddParam( sickcont, waza );
  }
}
// 状態異常追加時の文字列セットハンドラ
void handler_Makituku_Str( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_SICKID) == pml::wazadata::WAZASICK_BIND)
  ){
    BTL_HANDEX_STR_PARAMS* str = (BTL_HANDEX_STR_PARAMS*)EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );
    u8 defPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    u16 strID;

    switch( EVENT_FACTOR_GetSubID(myHandle) ){
    case WAZANO_MAKITUKU:       strID = BTL_STRID_SET_Makituku_Start; break;
    case WAZANO_SIMETUKERU:     strID = BTL_STRID_SET_Simetukeru_Start; break;
    case WAZANO_HONOONOUZU:     strID = BTL_STRID_SET_HonoNoUzu_Start; break;
    case WAZANO_KARADEHASAMU:   strID = BTL_STRID_SET_KaradeHasamu_Start; break;
    case WAZANO_SUNAZIGOKU:     strID = BTL_STRID_SET_Sunajigoku_Start; break;
    case WAZANO_MAGUMASUTOOMU:  strID = BTL_STRID_SET_MagmaStorm_Start; break;
    case WAZANO_UZUSIO:         strID = BTL_STRID_SET_Uzusio_Start; break;
    case WAZANO_MATOWARITUKU:   strID = BTL_STRID_SET_Matowarituku_Start; break;
    default:
      return;
    }

    HANDEX_STR_Setup( str, BTL_STRTYPE_SET, strID );
    HANDEX_STR_AddArg( str, defPokeID );
    HANDEX_STR_AddArg( str, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * うずしお
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Uzusio( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK,        handler_Makituku           }, // 追加効果による状態異常チェックハンドラ = まきつくと一緒
    { BTL_EVENT_WAZASICK_STR,    handler_Makituku_Str       }, // 状態異常追加時の文字列セットハンドラ
    { BTL_EVENT_CHECK_POKE_HIDE, handler_Uzusio_CheckHide   }, // 消えポケヒットチェック
    { BTL_EVENT_WAZA_DMG_PROC3,  handler_Uzusio_Dmg         }, // ダメージ計算ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 消えポケヒットチェックハンドラ
void handler_Uzusio_CheckHide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_POKE_HIDE) == BTL_POKEPARAM::CONTFLG_DIVING ){
      EVENTVAR_RewriteValue( BTL_EVAR_AVOID_FLAG, false );
    }
  }
}
// ダメージ計算ハンドラ
void handler_Uzusio_Dmg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ダイビング中の相手に２倍
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( target->CONTFLAG_Get( BTL_POKEPARAM::CONTFLG_DIVING) )
    {
      EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * いかりのまえば
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_IkariNoMaeba( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC1, handler_IkariNoMaeba },    // ダメージ計算最終ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージ計算最終ハンドラ
void handler_IkariNoMaeba( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    handler_Common_WazaDamageProc1_FixDamageByDefenderHp( flowWk, 1, 2 );
  }
}

//----------------------------------------------------------------------------------
/**
 * ガーディアン・デ・アローラ
 *
 * 相手のHPを 1/4 にする
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_GuardianDeAlola( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC1, handler_GuardianDeAlola },    // ダメージ計算最終ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージ計算最終ハンドラ
void handler_GuardianDeAlola( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    handler_Common_WazaDamageProc1_FixDamageByDefenderHp( flowWk, 3, 4 );
  }
}

/**
 * @brief ダメージ計算最終ハンドラ共通処理
 *        ダメージ量を( 相手の現在HP * numerator / denominator )に変更する
 *
 * @param flowWk
 * @param numerator     ダメージ量を決定する分子
 * @param denominator   ダメージ量を決定する分母
 */
void handler_Common_WazaDamageProc1_FixDamageByDefenderHp( ServerFlow* flowWk, u8 numerator, u8 denominator )
{
  const u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetPokeID );

  u16 damage = target->GetValue( BTL_POKEPARAM::BPP_HP) * numerator / denominator;
  if( damage == 0 )
  {
    damage = 1;
  }

  EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE, damage );
  EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE_FLAG, true );
}

//----------------------------------------------------------------------------------
/**
 * りゅうのいかり
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_RyuuNoIkari( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC1, handler_RyuuNoIkari },    // ダメージ計算最終ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_RyuuNoIkari( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE, 40 );
    EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE_FLAG, true );
  }
}
//----------------------------------------------------------------------------------
/**
 * ソニックブーム
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_SonicBoom( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC1, handler_SonicBoom },    // ダメージ計算最終ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

void handler_SonicBoom( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE, 20 );
    EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE_FLAG, true );
  }
}
//----------------------------------------------------------------------------------
/**
 * がむしゃら
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Gamusyara( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2,  handler_Gamusyara_CheckNoEffect },
    { BTL_EVENT_WAZA_DMG_PROC1, handler_Gamusyara },    // ダメージ計算最終ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 無効チェック
void handler_Gamusyara_CheckNoEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetPokeID );
    const BTL_POKEPARAM* user = flowWk->Hnd_GetPokeParam( pokeID );
    int diff = target->GetValue( BTL_POKEPARAM::BPP_HP) - user->GetValue( BTL_POKEPARAM::BPP_HP);
    if( diff <= 0 ){
      EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, true );
    }
  }
}
void handler_Gamusyara( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM *attacker, *defender;
    int diff;

    attacker = flowWk->Hnd_GetPokeParam( pokeID );
    defender = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    diff = defender->GetValue( BTL_POKEPARAM::BPP_HP) - attacker->GetValue( BTL_POKEPARAM::BPP_HP);
    if( diff < 0 ){
      diff = 1;
    }
    EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE, diff );
    EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE_FLAG, true );
  }
}
//----------------------------------------------------------------------------------
/**
 * ちきゅうなげ、ナイトヘッド
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_TikyuuNage( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC1, handler_TikyuuNage },    // ダメージ計算最終ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 相性計算：強制的に等倍にする（効果はバツグン表示をさせない）
void handler_TikyuuNage_CalcDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u8 level = bpp->GetValue( BTL_POKEPARAM::BPP_LEVEL );
    EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, level );
  }
}
// ダメージ計算：固定値
void handler_TikyuuNage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u8 level = bpp->GetValue( BTL_POKEPARAM::BPP_LEVEL );
    EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE, level );
    EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE_FLAG, true );
  }
}
//----------------------------------------------------------------------------------
/**
 * サイコウェーブ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_PsycoWave( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC1, handler_PsycoWave },    // ダメージ計算最終ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

void handler_PsycoWave( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u8 level = bpp->GetValue( BTL_POKEPARAM::BPP_LEVEL );
    u16 rand = 50 + calc::GetRand( 101 );
    u16 damage = level * rand / 100;
    if( damage == 0 ){
      damage = 1;
    }
    EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE, damage );
    EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE_FLAG, true );
  }
}
//----------------------------------------------------------------------------------
/**
 * たくわえる
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Takuwaeru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD,  handler_Takuwaeru_CheckExe   },    // ワザ出し成功判定
    { BTL_EVENT_UNCATEGORIZE_WAZA,   handler_Takuwaeru            },    // 未分類ワザ処理
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 成否チェック（カウンタが３貯まっていたら失敗する）
void handler_Takuwaeru_CheckExe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->COUNTER_Get( BTL_POKEPARAM::COUNTER_TAKUWAERU) >= 3 ){
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
    }
  }
}
void handler_Takuwaeru( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u8 takuwae_cnt = bpp->COUNTER_Get( BTL_POKEPARAM::COUNTER_TAKUWAERU );
    if( takuwae_cnt < 3 )
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* rank_param;
      BTL_HANDEX_PARAM_COUNTER*     counter_param;
      BTL_HANDEX_PARAM_MESSAGE*     msg_param;

      counter_param = reinterpret_cast<BTL_HANDEX_PARAM_COUNTER *>( flowWk->HANDEX_Push( BTL_HANDEX_COUNTER, pokeID ));
        counter_param->pokeID = pokeID;
        counter_param->counterID = BTL_POKEPARAM::COUNTER_TAKUWAERU;
        counter_param->value = takuwae_cnt + 1;
      flowWk->HANDEX_Pop( counter_param );
      msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Takuwaeru );
        HANDEX_STR_AddArg( &msg_param->str, pokeID );
        HANDEX_STR_AddArg( &msg_param->str, takuwae_cnt+1 );
      flowWk->HANDEX_Pop( msg_param );

      rank_param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
        rank_param->poke_cnt = 1;
        rank_param->pokeID[0] = pokeID;
        rank_param->rankType = pml::wazadata::RANKEFF_DEFENCE;
        rank_param->rankVolume = 1;
        rank_param->fAlmost = true;
        rank_param->fWazaEffect = true;
      flowWk->HANDEX_Pop( rank_param );
      counter_param = reinterpret_cast<BTL_HANDEX_PARAM_COUNTER *>( flowWk->HANDEX_Push( BTL_HANDEX_COUNTER, pokeID ));
        counter_param->header.failSkipFlag = true;
        counter_param->pokeID = pokeID;
        counter_param->counterID = BTL_POKEPARAM::COUNTER_TAKUWAERU_DEF;
        counter_param->value = bpp->COUNTER_Get( BTL_POKEPARAM::COUNTER_TAKUWAERU_DEF) + 1;
      flowWk->HANDEX_Pop( counter_param );


      rank_param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
        rank_param->poke_cnt = 1;
        rank_param->pokeID[0] = pokeID;
        rank_param->rankType = pml::wazadata::RANKEFF_SP_DEFENCE;
        rank_param->rankVolume = 1;
        rank_param->fAlmost = true;
        rank_param->fWazaEffect = true;
      flowWk->HANDEX_Pop( rank_param );
      counter_param = reinterpret_cast<BTL_HANDEX_PARAM_COUNTER *>( flowWk->HANDEX_Push( BTL_HANDEX_COUNTER, pokeID ));
        counter_param->header.failSkipFlag = true;
        counter_param->pokeID = pokeID;
        counter_param->counterID = BTL_POKEPARAM::COUNTER_TAKUWAERU_SPDEF;
        counter_param->value = bpp->COUNTER_Get( BTL_POKEPARAM::COUNTER_TAKUWAERU_SPDEF) + 1;
      flowWk->HANDEX_Pop( counter_param );

    }
  }
}
//----------------------------------------------------------------------------------
/**
 * はきだす
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Hakidasu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD,  handler_Hakidasu_CheckExe   },    // ワザ出し成功判定
    { BTL_EVENT_WAZA_POWER_BASE,         handler_Hakidasu_Pow        },    // ワザ威力決定
    { BTL_EVENT_WAZA_EXECUTE_DONE,       handler_Hakidasu_Done       },    // ワザ出し終了
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 成否チェック（たくわえカウンタが０だったら失敗する）
void handler_Hakidasu_CheckExe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->COUNTER_Get( BTL_POKEPARAM::COUNTER_TAKUWAERU) == 0 ){
      BTL_Printf("たくわえてないから失敗します\n");
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
    }
  }
}
void handler_Hakidasu_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u32 pow = bpp->COUNTER_Get( BTL_POKEPARAM::COUNTER_TAKUWAERU ) * 100;
    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
void handler_Hakidasu_Done( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    BTL_HANDEX_PARAM_RANK_EFFECT* rank_param;
    BTL_HANDEX_PARAM_COUNTER*  counter_param;
    BTL_HANDEX_PARAM_MESSAGE*  msg_param;

    int def_rank;
    int spdef_rank;
    int takuwae_count;

    def_rank = bpp->COUNTER_Get( BTL_POKEPARAM::COUNTER_TAKUWAERU_DEF );
    spdef_rank = bpp->COUNTER_Get( BTL_POKEPARAM::COUNTER_TAKUWAERU_SPDEF );
    takuwae_count = bpp->COUNTER_Get( BTL_POKEPARAM::COUNTER_TAKUWAERU );

    if( def_rank )
    {
      rank_param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
        rank_param->poke_cnt = 1;
        rank_param->pokeID[0] = pokeID;
        rank_param->rankType = pml::wazadata::RANKEFF_DEFENCE;
        rank_param->rankVolume = -def_rank;
        rank_param->fWazaEffect = true;
      flowWk->HANDEX_Pop( rank_param );

      counter_param = reinterpret_cast<BTL_HANDEX_PARAM_COUNTER *>( flowWk->HANDEX_Push( BTL_HANDEX_COUNTER, pokeID ));
        counter_param->pokeID = pokeID;
        counter_param->counterID = BTL_POKEPARAM::COUNTER_TAKUWAERU_DEF;
        counter_param->value = 0;
      flowWk->HANDEX_Pop( counter_param );
    }

    if( spdef_rank )
    {
      rank_param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
        rank_param->poke_cnt = 1;
        rank_param->pokeID[0] = pokeID;
        rank_param->rankType = pml::wazadata::RANKEFF_SP_DEFENCE;
        rank_param->rankVolume = -spdef_rank;
        rank_param->fWazaEffect = true;
      flowWk->HANDEX_Pop( rank_param );

      counter_param = reinterpret_cast<BTL_HANDEX_PARAM_COUNTER *>( flowWk->HANDEX_Push( BTL_HANDEX_COUNTER, pokeID ));
        counter_param->pokeID = pokeID;
        counter_param->counterID = BTL_POKEPARAM::COUNTER_TAKUWAERU_SPDEF;
        counter_param->value = 0;
      flowWk->HANDEX_Pop( counter_param );
    }

    // 「よこどり」を使うと「たくわえる」カウンタが０でも出せる…
    if( takuwae_count )
    {
      counter_param = reinterpret_cast<BTL_HANDEX_PARAM_COUNTER *>( flowWk->HANDEX_Push( BTL_HANDEX_COUNTER, pokeID ));
        counter_param->pokeID = pokeID;
        counter_param->counterID = BTL_POKEPARAM::COUNTER_TAKUWAERU;
        counter_param->value = 0;
      flowWk->HANDEX_Pop( counter_param );

      msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_TakuwaeOff );
        HANDEX_STR_AddArg( &msg_param->str, pokeID );
      flowWk->HANDEX_Pop( msg_param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * のみこむ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Nomikomu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD,  handler_Hakidasu_CheckExe   },    // ワザ出し成功判定
    { BTL_EVENT_RECOVER_HP_RATIO,        handler_Nomikomu_Ratio      },    // HP回復率決定
    { BTL_EVENT_WAZA_EXECUTE_DONE,       handler_Hakidasu_Done       },    // ワザ出し終了
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Nomikomu_Ratio( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u8 cnt = bpp->COUNTER_Get( BTL_POKEPARAM::COUNTER_TAKUWAERU );
    fx32 ratio;

    switch( cnt ){
    case 1:
    default:
      ratio = FX32_CONST( 0.25 );
      break;
    case 2:   ratio = FX32_CONST( 0.5 ); break;
    case 3:   ratio = FX32_CONST( 1 ); break;
    }

//    TAYA_Printf("takuwae cnt=%d, ratio=%08x\n", cnt, ratio);
    EVENTVAR_RewriteValue( BTL_EVAR_RATIO, ratio );
  }
}

//----------------------------------------------------------------------------------
/**
 * カウンター
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Counter( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD,  handler_Counter_CheckExe   },    // ワザ出し成功判定
    { BTL_EVENT_DECIDE_TARGET,           handler_Counter_Target     },    // ターゲット決定
    { BTL_EVENT_WAZA_DMG_PROC1,          handler_Counter_CalcDamage },    // ダメージ計算最終ハンドラ

  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Counter_CheckExe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  common_Counter_ExeCheck( flowWk, pokeID, work, pml::wazadata::DMG_PHYSIC );
}
void handler_Counter_Target( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  common_Counter_SetTarget( flowWk, pokeID, work, pml::wazadata::DMG_PHYSIC );
}
void handler_Counter_CalcDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  common_Counter_CalcDamage( flowWk, pokeID, work, pml::wazadata::DMG_PHYSIC, FX32_CONST(2) );
}
//----------------------------------------------------------------------------------
/**
 * ミラーコート
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_MilerCoat( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD,  handler_MilerCoat_CheckExe   },    // ワザ出し成功判定
    { BTL_EVENT_DECIDE_TARGET,       handler_MilerCoat_Target     },    // ターゲット決定
    { BTL_EVENT_WAZA_DMG_PROC1,      handler_MilerCoat_CalcDamage },    // ダメージ計算最終ハンドラ

  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_MilerCoat_CheckExe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  common_Counter_ExeCheck( flowWk, pokeID, work, pml::wazadata::DMG_SPECIAL );
}
void handler_MilerCoat_Target( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  common_Counter_SetTarget( flowWk, pokeID, work, pml::wazadata::DMG_SPECIAL );
}
void handler_MilerCoat_CalcDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  common_Counter_CalcDamage( flowWk, pokeID, work, pml::wazadata::DMG_SPECIAL, FX32_CONST(2) );
}
//----------------------------------------------------------------------------------
/**
 * メタルバースト
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_MetalBurst( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD, handler_MetalBurst_CheckExe   },    // ワザ出し成功判定
    { BTL_EVENT_DECIDE_TARGET,          handler_MetalBurst_Target     },    // ターゲット決定
    { BTL_EVENT_WAZA_DMG_PROC1,         handler_MetalBurst_CalcDamage },    // ダメージ計算最終ハンドラ

  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_MetalBurst_CheckExe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  common_Counter_ExeCheck( flowWk, pokeID, work, pml::wazadata::DMG_NONE );
}
void handler_MetalBurst_Target( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  common_Counter_SetTarget( flowWk, pokeID, work, pml::wazadata::DMG_NONE );
}
void handler_MetalBurst_CalcDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  common_Counter_CalcDamage( flowWk, pokeID, work, pml::wazadata::DMG_NONE, FX32_CONST(1.5f) );
}

//----------------------------------------------------------------------------------
//
// カウンター系共通：ワザだしチェック
//
//----------------------------------------------------------------------------------
void common_Counter_ExeCheck( ServerFlow* flowWk, u8 pokeID, int* work, WazaDamageType dmgType )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    BTL_POKEPARAM::WAZADMG_REC  rec;

    // 指定タイプダメージを受けていないなら失敗
    if( !common_Counter_GetRec(flowWk, bpp, dmgType, &rec) ){
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
    }
  }
}
//----------------------------------------------------------------------------------
//
// カウンター系共通：ターゲット決定
//
//----------------------------------------------------------------------------------
void common_Counter_SetTarget( ServerFlow* flowWk, u8 pokeID, int* work, WazaDamageType dmgType )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    BTL_POKEPARAM::WAZADMG_REC  rec;

    if( common_Counter_GetRec(flowWk, bpp, dmgType, &rec) )
    {
      u8 targetPokeID = rec.pokeID;

      // 対象が既に場にいない場合（とんぼがえりなど）、その位置に現在いるポケを対象にする
      BTL_PRINT(" POKE_%d の存在チェック\n", rec.pokeID);
      if( flowWk->Hnd_GetExistFrontPokePos(rec.pokeID) == BTL_POS_NULL )
      {
        BTL_PRINT("場に居ないよ\n");
        targetPokeID = BTL_POKEID_NULL;

        if( rec.pokePos != BTL_POS_NULL ){
          targetPokeID = flowWk->Hnd_GetExistPokeID( rec.pokePos );
        }

        if( targetPokeID == BTL_POKEID_NULL )
        {
          // その場にもいなければ「はたく」の範囲でランダム
          BtlPokePos pos = flowWk->Hnd_ReqWazaTargetAuto( pokeID, WAZANO_HATAKU );
          if( pos != BTL_POS_NULL ){
            targetPokeID = flowWk->Hnd_GetExistPokeID( pos );
          }

          // それも居ない場合、外れるけど最初のターゲット
          if( targetPokeID == BTL_POKEID_NULL ){

            targetPokeID = rec.pokeID;
          }
        }
      }

      EVENTVAR_RewriteValue( BTL_EVAR_POKEID_DEF, targetPokeID );
    }
  }
}
//----------------------------------------------------------------------------------
//
// カウンター系共通：ダメージ計算
//
//----------------------------------------------------------------------------------
void common_Counter_CalcDamage( ServerFlow* flowWk, u8 pokeID, int* work, WazaDamageType dmgType, fx32 ratio )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    BTL_POKEPARAM::WAZADMG_REC  rec;

    if( common_Counter_GetRec(flowWk, bpp, dmgType, &rec) )
    {
      u32 damage = calc::MulRatio( rec.damage, ratio );
      if( damage == 0 )
      {
        damage = 1;
      }
      EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE, damage );
      EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE_FLAG, true );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * カウンター系共通下請け：対象となるダメージレコードを取得
 *
 * @param   bpp
 * @param   dmgType   対象ダメージタイプ（物理or特殊：pml::wazadata::DMG_NONEなら両対応）
 * @param   rec       [out]取得先構造体アドレス
 *
 * @retval  bool      取得できたらtrue
 */
//----------------------------------------------------------------------------------
bool common_Counter_GetRec( ServerFlow* flowWk,  const BTL_POKEPARAM* bpp, WazaDamageType dmgType, BTL_POKEPARAM::WAZADMG_REC* rec )
{
  u8 pokeID = bpp->GetID();
  u8 idx = 0;
  while( bpp->WAZADMGREC_Get(0, idx++, rec) )
  {
    BTL_PRINT("ダメージレコード取得 : POKEID=%d, rec_pokeID=%d\n", pokeID, rec->pokeID);
    if( (!flowWk->Hnd_GetMainModule()->IsFriendPokeID(pokeID, rec->pokeID))
    &&  ((dmgType==pml::wazadata::DMG_NONE) || (rec->damageType == dmgType))
    ){
      return true;
    }
  }
  return false;
}
//----------------------------------------------------------------------------------
/**
 * とっておき
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Totteoki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD, handler_Totteoki },    // ワザ出し成否チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

void handler_Totteoki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u8 wazaCnt = bpp->WAZA_GetCount( );
    u8 usedCnt, fMaster,i;
    for(usedCnt=0, i=0, fMaster=false; i<wazaCnt; ++i)
    {
      if( bpp->WAZA_GetID(i) == WAZANO_TOTTEOKI ){
        fMaster = true;
        continue;
      }
      if( bpp->WAZA_CheckUsedInAlive(i) ){
        ++usedCnt;
      }
    }

    if( (!fMaster)
    ||  (wazaCnt < 2)
    ||  (usedCnt < (wazaCnt-1))
    ){
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * いびき
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Ibiki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_FAIL_THREW,        handler_Ibiki_CheckFail_1 },  // ワザ出し成否チェックハンドラ１
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD, handler_Ibiki_CheckFail_2 },  // ワザ出し成否チェックハンドラ２
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザ出し成否チェックハンドラ１
void handler_Ibiki_CheckFail_1( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
   ServerFlow::SV_WazaFailCause cause = static_cast<ServerFlow::SV_WazaFailCause>
                                          (EVENTVAR_GetValue( BTL_EVAR_FAIL_CAUSE ));

    if( cause == ServerFlow::SV_WAZAFAIL_NEMURI ){
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
    }
  }
}
// ワザ出し成否チェックハンドラ２
void handler_Ibiki_CheckFail_2( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
   ServerFlow::SV_WazaFailCause  fail_cause = static_cast<ServerFlow::SV_WazaFailCause>
                                                (EVENTVAR_GetValue( BTL_EVAR_FAIL_CAUSE ));

    if( fail_cause == ServerFlow::SV_WAZAFAIL_NEMURI )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_NULL );
    }
    else
    {
      if( !bpp->CheckNemuri( BTL_POKEPARAM::NEMURI_CHECK_INCLUDE_ZETTAINEMURI ) )
      {
        EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * あくむ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Akumu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2, handler_Akumu_NoEff },    // ワザ無効チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Akumu_NoEff( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF);
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( targetPokeID );

    if( !bpp->CheckNemuri( BTL_POKEPARAM::NEMURI_CHECK_INCLUDE_ZETTAINEMURI ) ){
      EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, true );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ふいうち
 *
 * @fix NMCat[9]「ニードルガード」に対して「ふいうち」を使ったポケモンが技の効果でダメージを受ける
 * 技失敗判定のタイミングを
 * BTL_EVENT_NOEFFECT_CHECK_L2　から
 * BTL_EVENT_WAZA_EXECUTE_CHECK_3RD　に変更
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Fuiuti( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD, handler_Fuiuti_NoEff },    // ワザ無効チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Fuiuti_NoEff( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    bool fSuccess = false;

    const u32 targetCount = EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
    if( 0 < targetCount )
    {
      const u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
      fSuccess = handler_Fuiuti_CheckSuccess( flowWk, targetPokeID );
    }

    if( !fSuccess )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
    }
  }
}

/**
 * @brief 「ふいうち」が成功するかどうかチェックする
 * @retval true  「ふいうち」が成功する
 * @retval false 「ふいうち」が失敗する
 */
bool handler_Fuiuti_CheckSuccess( ServerFlow* flowWk, u8 targetPokeID )
{
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( targetPokeID );
  BTL_ACTION_PARAM  action;

  // 相手が既に行動していたら失敗
  if( bpp->TURNFLAG_Get(BTL_POKEPARAM::TURNFLG_WAZAPROC_DONE) ) { 
    return false; 
  }

  // 現ターンのアクションが取得できなかったら失敗
  if( !flowWk->Hnd_GetThisTurnAction( targetPokeID, &action ) ) {
    return false; 
  }

  // 相手がダメージワザを選択していない場合も失敗
  WazaID waza = BTL_ACTION_GetWazaID( &action, bpp );
  if(  ( waza == WAZANO_NULL ) || 
      !( WAZADATA_IsDamage( waza ) ) )
  {
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------------
/**
 * おいうち
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Oiuti( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MENBERCHANGE_INTR, handler_Oiuti_Intr     },  // 入れ替え割り込みハンドラ
    { BTL_EVENT_CALC_HIT_CANCEL,   handler_Oiuti_HitCheck },  // ヒットチェック計算をスキップ
    { BTL_EVENT_WAZA_POWER_BASE,   handler_Oiuti_Dmg      },  // ダメージ計算最終チェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Oiuti_Intr( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  BTL_ACTION_PARAM  action;

  if( flowWk->Hnd_GetThisTurnAction( pokeID, &action) )
  {
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );

    if( !flowWk->Hnd_GetMainModule()->IsFriendPokeID(targetPokeID, pokeID) )
    {
      flowWk->Hnd_AddMemberOutIntr( pokeID );
    }
  }
}
// ヒットチェック計算をスキップ
void handler_Oiuti_HitCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  &&  (flowWk->Hnd_IsMemberOutIntr())
  ){
    EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
  }
}
// ダメージ計算最終チェックハンドラ
void handler_Oiuti_Dmg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( flowWk->Hnd_IsMemberOutIntr() )
    {
      HandCommon_MulWazaBasePower( 2 );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * だいばくはつ・じばく
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Daibakuhatsu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXE_START,       handler_Daibakuhatsu_ExeStart     },  // ワザ処理開始ハンドラ
    { BTL_EVENT_WAZA_DMG_DETERMINE,   handler_Daibakuhatsu_DmgDetermine },  // ダメージ確定ハンドラ
    { BTL_EVENT_WAZA_EXECUTE_DONE,    handler_Daibakuhatsu_ExeFix       },  // ワザ処理終了ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザ出し開始ハンドラ
void handler_Daibakuhatsu_ExeStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    flowWk->Hnd_SetWazaEffectIndex( BTLV_WAZAEFF_JIBAKU_TARGET_DAMAGE );

    // 自爆エフェクト出す
    WazaID     waza  = (WazaID)(EVENT_FACTOR_GetSubID(myHandle));
    BtlPokePos myPos = flowWk->Hnd_PokeIDtoPokePos( pokeID );
    flowWk->Hnd_PlayWazaEffect( myPos, myPos, waza, BTLV_WAZAEFF_JIBAKU_SELF, false );
  }
}
// ダメージ確定ハンドラ
void handler_Daibakuhatsu_DmgDetermine( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_KILL* param = reinterpret_cast<BTL_HANDEX_PARAM_KILL *>( flowWk->HANDEX_Push( BTL_HANDEX_KILL, pokeID ));
      param->pokeID = pokeID;
      param->deadCause = DAMAGE_CAUSE_JIBAKU;
    flowWk->HANDEX_Pop( param );
  }
}
// ワザ処理終了ハンドラ
void handler_Daibakuhatsu_ExeFix( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_KILL* param = reinterpret_cast<BTL_HANDEX_PARAM_KILL *>( flowWk->HANDEX_Push( BTL_HANDEX_KILL, pokeID ));
      param->pokeID = pokeID;
      param->deadCause = DAMAGE_CAUSE_JIBAKU;
    flowWk->HANDEX_Pop( param );
  }
}
//----------------------------------------------------------------------------------
/**
 * きあいだめ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Kiaidame( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,   handler_Kiaidame },         // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Kiaidame( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( !bpp->CONTFLAG_Get( BTL_POKEPARAM::CONTFLG_KIAIDAME) )
    {
      BTL_HANDEX_PARAM_SET_CONTFLAG* param;
      BTL_HANDEX_PARAM_MESSAGE* msg_param;

      param = (BTL_HANDEX_PARAM_SET_CONTFLAG*)flowWk->HANDEX_Push( BTL_HANDEX_SET_CONTFLAG, pokeID );
        param->pokeID = pokeID;
        param->flag = BTL_POKEPARAM::CONTFLG_KIAIDAME;
      flowWk->HANDEX_Pop( param );

      msg_param = (BTL_HANDEX_PARAM_MESSAGE*)flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID );
        HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_KiaiDame );
        HANDEX_STR_AddArg( &msg_param->str, pokeID );
      flowWk->HANDEX_Pop( msg_param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * じゅうでん
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Juden( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXE_START,         handler_Juden_Exe             }, // ワザ出し確定ハンドラ
    { BTL_EVENT_WAZASEQ_START,          handler_Juden_WazaStart       }, // ワザ処理開始ハンドラ
    { BTL_EVENT_CHECK_REMOVEALL_CANCEL, handler_Juden_RemoveAllTarget }, // ターゲットいなくても実行
    { BTL_EVENT_WAZA_POWER,             handler_Juden_Pow             }, // ワザ威力計算ハンドラ
    { BTL_EVENT_WAZASEQ_END,            handler_Juden_WazaEnd         }, // ワザ処理終了ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザ出し確定ハンドラ
void handler_Juden_Exe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_WAZAID) == EVENT_FACTOR_GetSubID(myHandle) ){
      BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Juuden );
        HANDEX_STR_AddArg( &param->str, pokeID );
      flowWk->HANDEX_Pop( param );

      EVENTVAR_RewriteValue( BTL_EVAR_ENABLE_MODE, (int)(ServerFlow::WAZAENABLE_NORMAL) );

      // @fix NMCat[2922]「じゅうでん」を「よこどり」すると状態情報に「じゅうでん」が表示され続ける
      // 「よこどり」は、技終了後にハンドラを強制的に削除するので、ポケモンの「じゅうでん」継続フラグを落とす処理が実行されない。
      // 「よこどり」で出た「じゅうでん」は、ポケモンを「じゅうでん」状態にしないようにする。
      if( EVENTVAR_GetValue( BTL_EVAR_ACTION_DESC_IS_YOKODORI_ROB_ACTION ) == false )
      {
        BTL_HANDEX_PARAM_SET_CONTFLAG* flagParam = reinterpret_cast<BTL_HANDEX_PARAM_SET_CONTFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_CONTFLAG, pokeID ) );
          flagParam->pokeID = pokeID;
          flagParam->flag   = BTL_POKEPARAM::CONTFLG_JUDEN;
        flowWk->HANDEX_Pop( flagParam );

        work[ WORKIDX_STICK ] = 1;
        work[0] = 1;  // じゅうでんシーケンスを１にする（貼り付き開始）
      }
    }
  }
}
// ターゲットいなくても実行チェック
void handler_Juden_RemoveAllTarget( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( ( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID ) &&
      ( EVENTVAR_GetValue(BTL_EVAR_WAZAID) == EVENT_FACTOR_GetSubID(myHandle) ) )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
  }
}
// ワザ威力計算ハンドラ
void handler_Juden_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  if( work[0] == 2 )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
    {
      if( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_DENKI ){
        EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
      }
//      EVENT_FACTOR_Remove( myHandle );
    }
  }
}
// ワザ処理開始ハンドラ
void handler_Juden_WazaStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  // 「じゅうでん」以外のワザを出すなら、シーケンスを２にする
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_WAZAID) != EVENT_FACTOR_GetSubID(myHandle))
  ){
     work[0] = 2;
  }
}
// ワザ処理終了ハンドラ
void handler_Juden_WazaEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (work[0] == 2)
  ){

    BTL_HANDEX_PARAM_RESET_CONTFLAG* param = reinterpret_cast<BTL_HANDEX_PARAM_RESET_CONTFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_RESET_CONTFLAG, pokeID ));
      param->pokeID = pokeID;
      param->flag   = BTL_POKEPARAM::CONTFLG_JUDEN;
    flowWk->HANDEX_Pop( param );

    // シーケンス=2 でワザ処理終了したら貼り付き解除
    EVENT_FACTOR_Remove( myHandle );
  }
}

//----------------------------------------------------------------------------------
/**
 * ほろびのうた
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_HorobiNoUta( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_EFFECTIVE, handler_HorobiNoUta_Exe          },  // ワザ処理終了（有効）ハンドラ
//    { BTL_EVENT_MIGAWARI_EXCLUDE,       handler_Common_MigawariEffctive  },  // みがわりチェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザ処理終了（有効）ハンドラ
void handler_HorobiNoUta_Exe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_HorobiNoUta );
    flowWk->HANDEX_Pop( param );
  }
}
// みがわりチェックハンドラ（共有：自分が攻撃側なら相手がみがわりでも有効にする）
void handler_Common_MigawariEffctive( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_WAZAID) == EVENT_FACTOR_GetSubID(myHandle))
  ){
    EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, false );
  }
}

//----------------------------------------------------------------------------------
/**
 * やどりぎのたね
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_YadorigiNoTane( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASICK_PARAM, handler_YadorigiNoTane_Param },    // 状態異常パラメータ調整ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 状態異常パラメータ調整ハンドラ
void handler_YadorigiNoTane_Param( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BtlPokePos myPos = flowWk->Hnd_GetExistFrontPokePos( pokeID );
    BTL_SICKCONT* cont = reinterpret_cast<BTL_SICKCONT*>( EVENTVAR_GetValue( BTL_EVAR_SICK_CONT_ADRS ) );
    *cont = SICKCONT_MakePermanentParam( pokeID, myPos );
  }
}

//----------------------------------------------------------------------------------
/**
 * ネコにこばん
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_NekoNiKoban( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_DETERMINE, handler_NekoNiKoban },    // ダメージ反応ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_NekoNiKoban( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u32 volume = bpp->GetValue( BTL_POKEPARAM::BPP_LEVEL ) * 5;
    if( flowWk->Hnd_AddBonusMoney(volume, pokeID) )
    {
      BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_NekoNiKoban );
      flowWk->HANDEX_Pop( param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * いかり
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Ikari( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXE_START,      handler_Ikari_Exe     },  // ワザ出し確定ハンドラ
    { BTL_EVENT_WAZA_DMG_REACTION,   handler_Ikari_React   },  // ダメージ反応ハンドラ
//    { BTL_EVENT_TURNCHECK_BEGIN,     handler_Ikari_Release },
    { BTL_EVENT_ACTPROC_START,       handler_Ikari_Release },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

void handler_Ikari_Exe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_WAZAID) == EVENT_FACTOR_GetSubID(myHandle) ){
      work[ WORKIDX_STICK ] = 1;
    }else{
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
      RemoveForce( bpp, static_cast<WazaNo>(EVENT_FACTOR_GetSubID(myHandle)) );
    }
  }
}
void handler_Ikari_React( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == false)
  ){
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( !bpp->IsDead() )
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));

        param->rankType = pml::wazadata::RANKEFF_ATTACK;
        param->rankVolume = 1;
        param->fAlmost = false;
        param->fWazaEffect = true;
        param->poke_cnt = 1;
        param->pokeID[0] = pokeID;
        param->fStdMsgDisable = true;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Ikari );
        HANDEX_STR_AddArg( &param->exStr, pokeID );

      flowWk->HANDEX_Pop( param );
    }
  }
}
void handler_Ikari_Release( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[ WORKIDX_STICK ] )
  {
    if( common_checkActStart_isMyWaza(flowWk, pokeID))
    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
      RemoveForce( bpp, static_cast<WazaNo>(EVENT_FACTOR_GetSubID(myHandle)) );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * アクアリング
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_AquaRing( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_AquaRing },            // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 未分類ワザハンドラ
void handler_AquaRing( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( !bpp->CheckSick( pml::wazadata::WAZASICK_AQUARING) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
        param->sickID = pml::wazadata::WAZASICK_AQUARING;
        param->sickCont = SICKCONT_MakePermanent( pokeID );
        param->pokeID = pokeID;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_AquaRing );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
      flowWk->HANDEX_Pop( param );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * あばれる・はなびらのまい・げきりん
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Abareru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXE_START,     handler_Abareru },            // ワザ出し確定ハンドラ
    { BTL_EVENT_WAZASEQ_END,        handler_Abareru_SeqEnd },  // ワザ処理最終ハンドラ
    { BTL_EVENT_TURNCHECK_END,      handler_Abareru_turnCheck },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザ出し確定ハンドラ
void handler_Abareru( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // @fix NMCat[4053] 「きあいパンチ」の失敗判定が「おどりこ」の効果で繰り出そうとしている技にも適応されている　の類似
    // 「おどりこ」で出た「はなびらのまい」では、技ロック状態にならないようにする
    if( EVENTVAR_GetValue(BTL_EVAR_ACTION_DESC_IS_ODORIKO_REACTION) )
    {
      return;
    }

    // 自分がワザロック状態になっていないなら、ワザロック状態にする
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( (!bpp->CheckSick( pml::wazadata::WAZASICK_WAZALOCK))
    &&  ( work[ WORKIDX_STICK ] == 0 )
    ){
      BTL_HANDEX_PARAM_ADD_SICK* param;
      u8 turns;

      turns = 2 + calc::GetRand( 2 );

      // 技ロック開始
      {
        param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
          param->sickID   = pml::wazadata::WAZASICK_WAZALOCK;
          param->sickCont = SICKCONT_MakeTurnParam( pokeID, turns, EVENT_FACTOR_GetSubID(myHandle) );
          param->fAlmost  = false;
          param->pokeID   = pokeID;
        flowWk->HANDEX_Pop( param );
      }

      // 状態異常セット
      {
        param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
          param->sickID   = pml::wazadata::WAZASICK_ABARERU;
          param->sickCont = SICKCONT_MakeTurnParam( pokeID, turns, EVENT_FACTOR_GetSubID(myHandle) );
          param->fAlmost  = false;
          param->pokeID   = pokeID;
        flowWk->HANDEX_Pop( param );
      }

      work[ WORKIDX_STICK ] = 1;
    }
  }
}

void abareru_Unlock( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( work );

  // 技ロック解除
  {
    BTL_HANDEX_PARAM_CURE_SICK* cure_param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
      cure_param->sickCode  = static_cast<BtlWazaSickEx>( pml::wazadata::WAZASICK_WAZALOCK );
      cure_param->poke_cnt  = 1;
      cure_param->pokeID[0] = pokeID;
    flowWk->HANDEX_Pop( cure_param );
  }

  // 状態異常を解除
  {
    BTL_HANDEX_PARAM_CURE_SICK* cure_param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
      cure_param->sickCode  = static_cast<BtlWazaSickEx>( pml::wazadata::WAZASICK_ABARERU );
      cure_param->poke_cnt  = 1;
      cure_param->pokeID[0] = pokeID;
    flowWk->HANDEX_Pop( cure_param );
  }

  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  RemoveForce( bpp, static_cast<WazaNo>(EVENT_FACTOR_GetSubID(myHandle)) );
}

// ワザ処理最終ハンドラ
void handler_Abareru_SeqEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( HandCommon_IsMyEvent(myHandle, BTL_EVAR_POKEID, pokeID) )
  {
    // @fix NMCat[4053] 「きあいパンチ」の失敗判定が「おどりこ」の効果で繰り出そうとしている技にも適応されている　の類似
    // 「おどりこ」で出た「はなびらのまい」では、技ロック状態にならないようにする
    if( EVENTVAR_GetValue(BTL_EVAR_ACTION_DESC_IS_ODORIKO_REACTION) )
    {
      return;
    }

    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

    if( work[WORKIDX_STICK] != 0)
    {
      // NMCat[404] 「おどりこ」→「はなびらのまい」→「自前のワザ」で、ワザIDが一致しないケースはカウンタ減らさない。
      if( EVENTVAR_GetValue(BTL_EVAR_WAZAID) != EVENT_FACTOR_GetSubID(myHandle) ){
        TAYA_PRINT("[HA] ワザidが一致しないのでカウンタ減らしません: 本来ワザ=%d, 使ったワザ=%d\n", EVENT_FACTOR_GetSubID(myHandle), EVENTVAR_GetValue(BTL_EVAR_WAZAID));
        return;
      }

      bool fUnlock = false;
      bool isLastTurn = bpp->IsSickLastTurn( pml::wazadata::WAZASICK_ABARERU );

      // 失敗したらすぐにロック解除
      if( !EVENTVAR_GetValue(BTL_EVAR_GEN_FLAG) )
      {
        fUnlock = true;
      }

      // 最終ターンの攻撃おわったら失敗しても成功しても混乱＆ロック解除
      if( isLastTurn )
      {
        BTL_HANDEX_PARAM_ADD_SICK* param;

        param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
          param->sickID = pml::wazadata::WAZASICK_KONRAN;
          calc::MakeDefaultWazaSickCont( param->sickID, bpp, &param->sickCont );
          param->fStdMsgDisable = true;
          param->pokeID = pokeID;
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_KonranAbare );
          HANDEX_STR_AddArg( &param->exStr, pokeID );
        flowWk->HANDEX_Pop( param );

        fUnlock = true;
      }

      if( fUnlock ){
        abareru_Unlock( myHandle, flowWk, pokeID, work );
      }
    }
  }
}
// ターンチェック
void handler_Abareru_turnCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);

  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (work[ WORKIDX_STICK ] != 0)
  ){
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    bool  fUnlock = true;

    do {
      // 眠っていたらすぐロック解除
      if( bpp->CheckNemuri( BTL_POKEPARAM::NEMURI_CHECK_ONLY_SICK ) ){
        break;
      }
      // NMcat[1265] あばれる状態が終わっていたら解除（フリーフォールでアクションスキップされた時対応）
      if( !bpp->CheckSick(pml::wazadata::WAZASICK_ABARERU) ){
        break;
      }

      fUnlock = false;

    }while(0);

    if( fUnlock ){
      abareru_Unlock( myHandle, flowWk, pokeID, work );
    }
  }
}


//----------------------------------------------------------------------------------
/**
 * さわぐ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Sawagu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_DETERMINE,  handler_Sawagu               }, // ワザ出し確定ハンドラ
    { BTL_EVENT_WAZASEQ_END,         handler_Sawagu_SeqEnd        }, // ワザ処理最終ハンドラ
    { BTL_EVENT_TURNCHECK_END,       handler_Sawagu_turnCheck     }, // ターンチェック終了ハンドラ
    { BTL_EVENT_ADDSICK_CHECKFAIL,   handler_Sawagu_CheckSickFail }, // 状態異常失敗チェック
    { BTL_EVENT_CHECK_INEMURI,       handler_Sawagu_CheckInemuri  }, // いねむり成否チェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

void handler_Sawagu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( HandCommon_IsMyEvent(myHandle, BTL_EVAR_POKEID_ATK, pokeID) )
  {
    // 自分がワザロック状態になっていないなら、ワザロック状態にする
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( (!bpp->CheckSick( pml::wazadata::WAZASICK_WAZALOCK))
    &&  ( work[ WORKIDX_STICK ] == 0 )
    ){
      BTL_HANDEX_PARAM_ADD_SICK* param;
      BTL_HANDEX_PARAM_MESSAGE* msg_param;
      u8 turns;

      turns = 3;

      {
        param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
          param->sickID = pml::wazadata::WAZASICK_WAZALOCK;
          param->sickCont = SICKCONT_MakeTurnParam( pokeID, turns, EVENT_FACTOR_GetSubID(myHandle) );
          param->fAlmost = false;
          param->pokeID = pokeID;
        flowWk->HANDEX_Pop( param );
      }

      {
        param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
          param->sickID = pml::wazadata::WAZASICK_SAWAGU;
          param->sickCont = SICKCONT_MakeTurnParam( pokeID, turns, EVENT_FACTOR_GetSubID(myHandle) );
          param->fAlmost = false;
          param->pokeID = pokeID;
        flowWk->HANDEX_Pop( param );
      }

      msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Sawagu );
        HANDEX_STR_AddArg( &msg_param->str, pokeID );
      flowWk->HANDEX_Pop( msg_param );

      {
        BTL_HANDEX_PARAM_CURE_SICK* cure_param;
        BtlPokePos myPos = flowWk->Hnd_GetExistFrontPokePos( pokeID );
        BtlExPos   expos = EXPOS_MAKE( BTL_EXPOS_FULL_ALL, myPos );
        HANDWORK_POKEID* idwk = reinterpret_cast<HANDWORK_POKEID*>(flowWk->Hnd_GetTmpWork( sizeof(HANDWORK_POKEID) ));
        const BTL_POKEPARAM* bpp_target;
        u32 i;

        idwk->pokeCnt = flowWk->Hnd_ExpandPokeID( expos, idwk->pokeID );
        for(i=0; i<idwk->pokeCnt; ++i)
        {
          bpp_target = flowWk->Hnd_GetPokeParam( idwk->pokeID[i] );
          if( bpp_target->CheckNemuri( BTL_POKEPARAM::NEMURI_CHECK_ONLY_SICK ) )
          {
            cure_param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
              cure_param->sickCode = static_cast<BtlWazaSickEx>( pml::wazadata::WAZASICK_NEMURI );
              cure_param->poke_cnt = 1;
              cure_param->pokeID[0] = idwk->pokeID[i];
              HANDEX_STR_Setup( &cure_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_SawaguWake );
              HANDEX_STR_AddArg( &cure_param->exStr, idwk->pokeID[i] );
            flowWk->HANDEX_Pop( cure_param );
          }
        }
      }

      work[ WORKIDX_STICK ] = 1;
    }
  }
}
// ワザ処理最終ハンドラ
void handler_Sawagu_SeqEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( HandCommon_IsMyEvent(myHandle, BTL_EVAR_POKEID, pokeID) )
  {
    // 失敗したらすぐにロック解除
    if( !EVENTVAR_GetValue(BTL_EVAR_GEN_FLAG) )
    {
      Sawagu_CureLock( flowWk, pokeID );
    }
  }
}
void handler_Sawagu_turnCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( work[WORKIDX_STICK] )
    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

      if( !bpp->IsDead() )
      {
        //「じごくづき」を受けたなら、ロック解除
        if( bpp->CheckSick( pml::wazadata::WAZASICK_ZIGOKUDUKI ) )
        {
          Sawagu_CureLock( flowWk, pokeID );
        }

        // ロック切れてたら「おとなしくなった」表示
        if( !bpp->CheckSick( pml::wazadata::WAZASICK_WAZALOCK) )
        {
          BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
            HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_SawaguCure );
            HANDEX_STR_AddArg( &param->str, pokeID );
          flowWk->HANDEX_Pop( param );
          RemoveForce( bpp, static_cast<WazaNo>(EVENT_FACTOR_GetSubID(myHandle)) );
        }
        // ロック中なら「さわいでいる」表示
        else
        {
          BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
            HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_SawaguCont );
            HANDEX_STR_AddArg( &param->str, pokeID );
          flowWk->HANDEX_Pop( param );
        }
      }
    }
  }
}

void Sawagu_CureLock( ServerFlow* flowWk, u8 pokeID )
{
  {
    BTL_HANDEX_PARAM_CURE_SICK* cure_param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
      cure_param->sickCode = static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_WAZALOCK);
      cure_param->poke_cnt = 1;
      cure_param->pokeID[0] = pokeID;
    flowWk->HANDEX_Pop( cure_param );
  }

  {
    BTL_HANDEX_PARAM_CURE_SICK* cure_param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
      cure_param->sickCode = static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_SAWAGU);
      cure_param->poke_cnt = 1;
      cure_param->pokeID[0] = pokeID;
    flowWk->HANDEX_Pop( cure_param );
  }
}

void handler_Sawagu_CheckSickFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( work[WORKIDX_STICK] )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_SICKID) == pml::wazadata::WAZASICK_NEMURI )
    {
      if( EVENTVAR_RewriteValue(BTL_EVAR_FAIL_FLAG, true) )
      {
        BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
          u8 defPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
          u16 strID = (defPokeID == pokeID)? BTL_STRID_SET_SawaguSleeplessSelf : BTL_STRID_SET_SawaguSleepless;
          HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, strID );
          HANDEX_STR_AddArg( &param->str, defPokeID );
        flowWk->HANDEX_Pop( param );
      }
    }
  }
}
void handler_Sawagu_CheckInemuri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(pokeID);
  if( work[WORKIDX_STICK] )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
  }
}


//----------------------------------------------------------------------------------
/**
 * ころがる・アイスボール
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Korogaru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXE_START,          handler_Korogaru_ExeFix    },  // ワザ出し確定ハンドラ
    { BTL_EVENT_WAZA_EXECUTE_FAIL,       handler_Korogaru_NoEffect  },  // ワザ出し失敗確定ハンドラ
    { BTL_EVENT_WAZA_EXECUTE_NO_EFFECT,  handler_Korogaru_NoEffect  },   // ワザ無効ハンドラ
    { BTL_EVENT_WAZASEQ_END,             handler_Korogaru_SeqEnd    },
    { BTL_EVENT_WAZA_POWER_BASE,         handler_Korogaru_Pow       },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Korogaru_ExeFix( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  enum {
    KOROGARU_TURNS = 5,
  };

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // 自分がワザロック状態になっていないなら、ワザロック状態にする
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( (!bpp->CheckSick( pml::wazadata::WAZASICK_WAZALOCK))
    &&  (!bpp->CheckNemuri( BTL_POKEPARAM::NEMURI_CHECK_ONLY_SICK ))
    &&  ( work[ WORKIDX_STICK ] == 0 )
    ){
      BTL_HANDEX_PARAM_ADD_SICK* param;

      param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
        param->sickID = pml::wazadata::WAZASICK_WAZALOCK;
        param->sickCont = SICKCONT_MakeTurnParam( pokeID, KOROGARU_TURNS, EVENT_FACTOR_GetSubID(myHandle) );
        param->fAlmost = false;
        param->pokeID = pokeID;
      flowWk->HANDEX_Pop( param );

      work[0] = 0;
      work[1] = KOROGARU_TURNS;
      work[ WORKIDX_STICK ] = 1;
    }
  }
}
// ワザはずれた
void handler_Korugaru_Avoid( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( HandCommon_IsMyEvent(myHandle, BTL_EVAR_POKEID_ATK, pokeID) )
  {
    common_Korogaru_Unlock( myHandle, flowWk, pokeID, work );
  }
}
// ワザだしたが効果なし
void handler_Korogaru_NoEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( HandCommon_IsMyEvent(myHandle, BTL_EVAR_POKEID, pokeID) )
  {
    common_Korogaru_Unlock( myHandle, flowWk, pokeID, work );
  }
}
void handler_Korogaru_SeqEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( (HandCommon_IsMyEvent(myHandle, BTL_EVAR_POKEID, pokeID))
  &&  (EVENTVAR_GetValue(BTL_EVAR_GEN_FLAG) == false)
  ){
    common_Korogaru_Unlock( myHandle, flowWk, pokeID, work );
  }
}

/**
 *  ころがるロック解除＆貼り付き解除共通
 */
void  common_Korogaru_Unlock( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  BTL_HANDEX_PARAM_CURE_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
    param->pokeID[0] = pokeID;
    param->poke_cnt = 1;
    param->sickCode = static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_WAZALOCK);
  flowWk->HANDEX_Pop( param );

  work[ WORKIDX_STICK ] = 0;
  removeHandlerForce( pokeID, static_cast<WazaNo>(EVENT_FACTOR_GetSubID(myHandle)) );
}

// 威力決定
void handler_Korogaru_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u32 mul = 1;

    for(int i=0; i<work[0]; ++i){
      mul *= 2;
    }
    if( bpp->CONTFLAG_Get( BTL_POKEPARAM::CONTFLG_MARUKUNARU) ){
      mul *= 2;
    }

    HandCommon_MulWazaBasePower( mul );

    work[0]++;
    if( work[0] >= work[1] ){
      removeHandlerForce( pokeID, static_cast<WazaNo>(EVENT_FACTOR_GetSubID(myHandle)) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * トリプルキック
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_TripleKick( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER_BASE,  handler_TripleKick          },    // ワザ威力チェックハンドラ
    { BTL_EVENT_WAZA_HIT_COUNT,   handler_TripleKick_HitCount }
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_TripleKick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    work[0]++;
    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, work[0]*10 );
  }
}
void handler_TripleKick_HitCount( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_AVOID_FLAG, true );
  }
}
//----------------------------------------------------------------------------------
/**
 * ジャイロボール
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_GyroBall( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER_BASE, handler_GyroBall },    // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_GyroBall( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bppSelf = flowWk->Hnd_GetPokeParam( pokeID );
    const BTL_POKEPARAM* bppTarget = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );

    u16 selfAgi = flowWk->Hnd_CalcAgility( bppSelf, false );
    u16 targetAgi = flowWk->Hnd_CalcAgility( bppTarget, false );

    u32 pow = 1 + (25 * targetAgi / selfAgi);
    if( pow > 150 ){
      pow = 150;
    }

    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * リベンジ・ゆきなだれ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Revenge( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER_BASE, handler_Revenge },    // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Revenge( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    BTL_POKEPARAM::WAZADMG_REC  rec;
    u8  idx = 0;

    // 狙う相手から現ターンにダメージ受けてたら威力２倍
    while( bpp->WAZADMGREC_Get(0, idx++, &rec) )
    {
      if( (rec.pokeID == target_pokeID)
      &&  (rec.damage != 0)
      ){
        HandCommon_MulWazaBasePower( 2 );
        break;
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * じたばた、きしかいせい
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Jitabata( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER_BASE, handler_Jitabata },    // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Jitabata( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  static const struct {
    u16   dot_ratio;
    u16   pow;
  }tbl[]={
    {  1,  200 },
    {  4,  150 },
    {  9,  100 },
    { 16,   80 },
    { 32,   40 },
    { 48,   20 },
  };

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u32 r = (bpp->GetValue( BTL_POKEPARAM::BPP_HP) * BTLV_GAUGE_HP_DOTTOMAX) / bpp->GetValue( BTL_POKEPARAM::BPP_MAX_HP);
    if( r == 0 ){
      r = 1;
    }

    {
      u32 i, max;
      max = GFL_NELEMS(tbl) - 1;
      for(i=0; i<max; ++i)
      {
        if( r <= tbl[i].dot_ratio ){ break; }
      }

      EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, tbl[i].pow );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * からげんき
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Karagenki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_Karagenki_AtkPow  },    // 攻撃ポケ能力チェックハンドラ
    { BTL_EVENT_WAZA_POWER,     handler_Karagenki_WazaPow },    // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 攻撃ポケ能力チェックハンドラ
void handler_Karagenki_AtkPow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // やけど減衰を無効化
    EVENTVAR_RewriteValue( BTL_EVAR_DISABLE_YAKEDO_FLAG, true );
  }
}
// ワザ威力チェックハンドラ
void handler_Karagenki_WazaPow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    PokeSick  sick = bpp->GetPokeSick( );
    if( (sick == pml::pokepara::SICK_DOKU)
    ||  (sick == pml::pokepara::SICK_MAHI)
    ||  (sick == pml::pokepara::SICK_YAKEDO)
    ){
      EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * しっぺがえし
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Sippegaesi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER_BASE, handler_Sippegaesi },    // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Sippegaesi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( bpp->TURNFLAG_Get(BTL_POKEPARAM::TURNFLG_ACTION_DONE) )
    {
      HandCommon_MulWazaBasePower( 2 );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ふんか、しおふき
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Funka( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER_BASE, handler_Funka },    // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Funka( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

    u32 hp = bpp->GetValue( BTL_POKEPARAM::BPP_HP );
    u32 maxHP = bpp->GetValue( BTL_POKEPARAM::BPP_MAX_HP );
    u32 pow = WAZADATA_GetPower( (WazaNo)(EVENT_FACTOR_GetSubID(myHandle)) );

    pow = (pow * hp) / maxHP;
    if( pow == 0 ){
      pow = 1;
    }
    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * しぼりとる・にぎりつぶす
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Siboritoru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER_BASE, handler_Siboritoru },    // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Siboritoru( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    fx32 r = bpp->GetHPRatio();
    u32 pow = calc::MulRatio_OverZero( 120, r ) / 100;
    if( pow == 0 ){
      pow = 1;
    }
    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * しおみず
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Siomizu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Siomizu },    // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Siomizu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    fx32 r = bpp->GetHPRatio( );
    if( r <= FX32_CONST(50) )
    {
      EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * おんがえし
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Ongaesi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER_BASE, handler_Ongaesi },    // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Ongaesi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    const pml::pokepara::PokemonParam* pp = bpp->GetSrcData();
    u32 natsuki = pp->GetFamiliarity();
    u32 pow = natsuki * 10 / 25;
    if( pow == 0 ){
      pow = 1;
    }
//    TAYA_Printf("おんがえし：なつき度=%d, 基本威力=%d\n", natsuki, pow );
    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * やつあたり
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Yatuatari( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER_BASE, handler_Yatuatari },    // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Yatuatari( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    const pml::pokepara::PokemonParam* pp = bpp->GetSrcData( );
    u32 natsuki = pp->GetFamiliarity();
    u32 pow = (255 - natsuki) * 10 / 25;
    if( pow == 0 ){
      pow = 1;
    }
    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * めざましビンタ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_MezamasiBinta( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER_BASE,         handler_MezamasiBinta             }, // ワザ威力チェックハンドラ
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL, handler_MezamasiBinta_AfterDamage }, // ダメージプロセス終了ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザ威力チェックハンドラ
void handler_MezamasiBinta( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( bpp->CheckNemuri( BTL_POKEPARAM::NEMURI_CHECK_INCLUDE_ZETTAINEMURI ) )
    {
      HandCommon_MulWazaBasePower( 2 );
    }
  }
}
// ダメージプロセス終了ハンドラ
void handler_MezamasiBinta_AfterDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( target_pokeID );
    if( bpp->CheckNemuri( BTL_POKEPARAM::NEMURI_CHECK_ONLY_SICK ) ){
      BTL_HANDEX_PARAM_CURE_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
        param->pokeID[0] = target_pokeID;
        param->poke_cnt = 1;
        param->sickCode = static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_NEMURI);
      flowWk->HANDEX_Pop( param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * きつけ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Kituke( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER_BASE,         handler_Kituke             },  // ワザ威力チェックハンドラ
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL, handler_Kituke_AfterDamage },  // ダメージプロセス終了ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザ威力チェックハンドラ
void handler_Kituke( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // マヒ状態の相手に威力２倍
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( bpp->CheckSick( pml::wazadata::WAZASICK_MAHI) )
    {
      HandCommon_MulWazaBasePower( 2 );
    }
  }
}
// ダメージプロセス終了ハンドラ
void handler_Kituke_AfterDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    if( target_pokeID != BTL_POKEID_NULL )
    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( target_pokeID );
      if( bpp->CheckSick( pml::wazadata::WAZASICK_MAHI) ){
        BTL_HANDEX_PARAM_CURE_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
          param->pokeID[0] = target_pokeID;
          param->poke_cnt = 1;
          param->sickCode = static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_MAHI);
        flowWk->HANDEX_Pop( param );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * プレゼント
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Present( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DMG_TO_RECOVER_CHECK,  handler_Present_Check }, // ダメージワザ回復化チェックハンドラ
    { BTL_EVENT_DMG_TO_RECOVER_FIX,    handler_Present_Fix },   // ダメージワザ回復化確定ハンドラ
    { BTL_EVENT_WAZA_POWER_BASE,       handler_Present_Pow },   // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージワザ回復化チェックハンドラ
void handler_Present_Check( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 20％の確率で回復化
    if( calc::IsOccurPer(20) )
    {
      work[0] = EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
    }

    {
      u8 effIndex = (work[0])? BTLV_WAZAEFF_PRESENT_RECOVER : BTLV_WAZAEFF_PRESENT_DAMAGE;
      flowWk->Hnd_SetWazaEffectIndex( effIndex );
    }
  }
}
// ダメージワザ回復化確定ハンドラ
void handler_Present_Fix( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  // 自分が回復化した場合、回復量を計算
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (work[0])
  ){
    u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( target_pokeID );

    if( !bpp->IsHPFull() )
    {
      BTL_HANDEX_PARAM_RECOVER_HP*  param = reinterpret_cast<BTL_HANDEX_PARAM_RECOVER_HP*>
                                              (flowWk->HANDEX_Push( BTL_HANDEX_RECOVER_HP, pokeID ));
        param->pokeID = target_pokeID;
        param->recoverHP = calc::QuotMaxHP( bpp, 4 );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_HP_Recover );
        HANDEX_STR_AddArg( &param->exStr, target_pokeID );
      flowWk->HANDEX_Pop( param );
    }
    else
    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param;
        msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_NoEffect );
        HANDEX_STR_AddArg( &msg_param->str, target_pokeID );
      flowWk->HANDEX_Pop( msg_param );
    }
  }
}
// ワザ威力チェックハンドラ
void handler_Present_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u32 rand = calc::GetRand( 80 );
    u32 pow = 0;

    if( rand < 40 ){
      pow = 40;
    }else if( rand < 70 ){
      pow = 80;
    }else{
      pow = 120;
    }

    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * きりふだ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Kirifuda( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER_BASE,   handler_Kirifuda },    // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Kirifuda( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  static const u16 powTbl[] = {
    200, 80, 60, 50, 40,
  };

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u16 pow;
    u8 wazaIdx;

    wazaIdx = bpp->WAZA_SearchIdx( static_cast<WazaNo>(EVENT_FACTOR_GetSubID(myHandle)) );
    if( wazaIdx != PTL_WAZA_MAX )
    {
      u8 pp = static_cast<u8>(bpp->WAZA_GetPP( wazaIdx ));
      if( pp >= GFL_NELEMS(powTbl) ){
        pp = GFL_NELEMS(powTbl) - 1;
      }
      pow = powTbl[ pp ];
    }else{
      pow = powTbl[ (GFL_NELEMS(powTbl)-1) ];
    }
    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * おしおき
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Osioki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER_BASE,   handler_Osioki },    // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Osioki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  static const BTL_POKEPARAM::ValueID  statusTbl[] = {
   BTL_POKEPARAM::BPP_ATTACK_RANK,BTL_POKEPARAM::BPP_DEFENCE_RANK,BTL_POKEPARAM::BPP_SP_ATTACK_RANK,BTL_POKEPARAM::BPP_SP_DEFENCE_RANK,BTL_POKEPARAM::BPP_AGILITY_RANK,
   BTL_POKEPARAM::BPP_HIT_RATIO,BTL_POKEPARAM::BPP_AVOID_RATIO,
  };

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    u32 pow, i;
    int rank, rankSum;

    rankSum = 0;
    for(i=0; i<GFL_NELEMS(statusTbl); ++i)
    {
      rank = bpp->GetValue( statusTbl[i] );
      if( rank > BTL_POKEPARAM::RANK_STATUS_DEFAULT ){
        rankSum += (rank - BTL_POKEPARAM::RANK_STATUS_DEFAULT);
      }
    }
    pow = 60 + rankSum * 20;
    if( pow > 200 ){
      pow = 200;
    }
    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * れんぞくぎり
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_RenzokuGiri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER_BASE,   handler_RenzokuGiri },    // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_RenzokuGiri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  enum {
    POW_MAX = 160,    // このワザの威力最大値
  };

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->GetPrevWazaID() == EVENT_FACTOR_GetSubID(myHandle) )
    {
      u32 pow = EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
      u32 cnt = bpp->GetWazaContCounter( );
      u32 i;

      for(i=0; i<cnt; ++i)
      {
        pow *= 2;
        if( pow > POW_MAX ){
          pow = POW_MAX;
          break;
        }
      }
      EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ダメおし
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Dameosi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER_BASE,   handler_Dameosi },    // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Dameosi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );

    if( bpp->TURNFLAG_Get(BTL_POKEPARAM::TURNFLG_DAMAGED) )
    {
      HandCommon_MulWazaBasePower( 2 );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * けたぐり・くさむすび
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Ketaguri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER_BASE,   handler_Ketaguri },    // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Ketaguri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    u32 heavy = flowWk->Hnd_GetWeight( bpp->GetID() );
    u32 pow;

    if( heavy >= 2000 ){
      pow = 120;
    }else if( heavy >= 1000 ){
      pow = 100;
    }else if( heavy >= 500 ){
      pow = 80;
    }else if( heavy >= 250 ){
      pow = 60;
    }else if( heavy >= 100 ){
      pow = 40;
    }else{
      pow = 20;
    }

    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * ウェザーボール
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_WeatherBall( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_GET_ZENRYOKU_WAZA,      handler_WeatherBall_GetZenryokuWaza     }, // 全力技取得
    { BTL_EVENT_WAZA_PARAM,             handler_WeatherBall_Type                }, // ワザパラメータチェックハンドラ
    { BTL_EVENT_AFTER_WAZA_PARAM_FIXED, handler_WeatherBall_AfterWazaParamFixed }, // ワザパラメータ確定後
    { BTL_EVENT_WAZA_POWER_BASE,        handler_WeatherBall_Pow                 }, // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

/**
 * @brief 天候によって変化する「ウェザーボール」のパラメータ
 */
struct WeatherBallParam
{
  pml::PokeType    type;             // 技のタイプ
  u8               effIndex;         // エフェクト番号
  WazaNo           zenryokuWazaNo;   // 全力技の技No.
  ZenryokuWazaKind zenryokuWazaKind; // 全力技の種類
};

/**
 * @brief 天候によって変化した「ウェザーボール」パラメータを取得する
 *
 * @param[out] param    取得したパラメータの格納先
 * @param      weather  天候
 *
 * @retval true   天候によってパラメータが変化した
 * @retval false  天候によってパラメータが変化しなかった
 */
bool get_WeatherBallParam( WeatherBallParam* param, BtlWeather weather )
{
  switch( weather )
  {
  case BTL_WEATHER_SHINE:
  case BTL_WEATHER_DAY: 
    param->type = POKETYPE_HONOO;
    param->effIndex = BTLV_WAZAEFF_WEATHERBALL_SHINE;
    param->zenryokuWazaNo = WAZANO_HONOOZENRYOKU;
    param->zenryokuWazaKind = pml::waza::ZENRYOKUWAZA_KIND_GENERAL;
    return true;

  case BTL_WEATHER_RAIN:
  case BTL_WEATHER_STORM:
    param->type = POKETYPE_MIZU;
    param->effIndex = BTLV_WAZAEFF_WEATHERBALL_RAIN;
    param->zenryokuWazaNo = WAZANO_MIZUZENRYOKU;
    param->zenryokuWazaKind = pml::waza::ZENRYOKUWAZA_KIND_GENERAL;
    return true;

  case BTL_WEATHER_SAND:
    param->type = POKETYPE_IWA;
    param->effIndex = BTLV_WAZAEFF_WEATHERBALL_SAND;
    param->zenryokuWazaNo = WAZANO_IWAZENRYOKU;
    param->zenryokuWazaKind = pml::waza::ZENRYOKUWAZA_KIND_GENERAL;
    return true;

  case BTL_WEATHER_SNOW:
    param->type = POKETYPE_KOORI;
    param->effIndex = BTLV_WAZAEFF_WEATHERBALL_SNOW;
    param->zenryokuWazaNo = WAZANO_KOORIZENRYOKU;
    param->zenryokuWazaKind = pml::waza::ZENRYOKUWAZA_KIND_GENERAL;
    return true;

  default:
    param->type = POKETYPE_NORMAL;
    param->effIndex = BTLV_WAZAEFF_WEATHERBALL_NORMAL;
    param->zenryokuWazaNo = WAZANO_NOOMARUZENRYOKU;
    param->zenryokuWazaKind = pml::waza::ZENRYOKUWAZA_KIND_GENERAL;
    return false;
  }
}

void handler_WeatherBall_Type( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue( BTL_EVAR_POKEID ) != pokeID ) {
    return;
  }

  BtlWeather weather = flowWk->Hnd_GetWeather();

  WeatherBallParam param;
  param.type = EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
  param.effIndex = BTLV_WAZAEFF_WEATHERBALL_NORMAL;
  get_WeatherBallParam( &param, weather );

  EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, param.type );
  flowWk->Hnd_SetWazaEffectIndex( param.effIndex );  
}

/**
 * @brief 全力技取得イベント
 *        天候により変化する
 *
 * @param myHandle
 * @param flowWk
 * @param pokeID   技を使用したポケモンのID
 * @param work     イベントファクタの作業領域
 *
 * BTL_EVAR_POKEID               [in]  処理対象のポケモンID
 * BTL_EVAR_ZENRYOKU_WAZAID      [out] 全力技の技ID
 * BTL_EVAR_ZENRYOKU_WAZA_KIND   [out] 全力技の種類
 */
void handler_WeatherBall_GetZenryokuWaza( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue( BTL_EVAR_POKEID ) != pokeID ) {
    return;
  }

  BtlWeather weather = flowWk->Hnd_GetWeather();

  WeatherBallParam param;
  get_WeatherBallParam( &param, weather );
  EVENTVAR_RewriteValue( BTL_EVAR_ZENRYOKU_WAZAID, param.zenryokuWazaNo );
  EVENTVAR_RewriteValue( BTL_EVAR_ZENRYOKU_WAZA_KIND, param.zenryokuWazaKind );
}

/**
 * @brief 技パラメータ確定後イベント
 *        「ウェザーボール」を全力技として使用した場合、
 *        天候によって実行する全力技が変化したなら、その旨を表示する
 *
 * @param myHandle
 * @param flowWk
 * @param pokeID   技を使用したポケモンのID
 * @param work     イベントファクタの作業領域
 *
 * BTL_EVAR_POKEID               [in] 処理対象のポケモンID
 * BTL_EVAR_ORG_WAZAID           [in] 選択した技
 * BTL_EVAR_WAZAID               [in] 実行する技
 * BTL_EVAR_ZENRYOKU_WAZA_FLAG   [in] 実行する技が全力技か？
 */
void handler_WeatherBall_AfterWazaParamFixed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( work );

  if( EVENTVAR_GetValue( BTL_EVAR_POKEID ) != pokeID ) 
  {
    return;
  }

  //「ウェザーボール」を全力技として選択した場合にのみ表示する
  if( !( EVENTVAR_GetValue( BTL_EVAR_ORG_WAZAID ) == WAZANO_WHEZAABOORU ) ||
      !( EVENTVAR_GetValue( BTL_EVAR_ZENRYOKU_WAZA_FLAG ) ) ) 
  {
    return;
  }

  //「○○は　てんきの　えいきょうで　××に　なった！」
  BtlWeather weather = flowWk->Hnd_GetWeather();
  WeatherBallParam param;
  if( get_WeatherBallParam( &param, weather ) ) 
  {
    BTL_HANDEX_PARAM_MESSAGE* msgParam = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &msgParam->str, BTL_STRTYPE_STD, BTL_STRID_STD_AuraWeatherBall );
      HANDEX_STR_AddArg( &msgParam->str, WAZANO_NOOMARUZENRYOKU );
      HANDEX_STR_AddArg( &msgParam->str, EVENTVAR_GetValue( BTL_EVAR_WAZAID ) );
    flowWk->HANDEX_Pop( msgParam );
  }
}

void handler_WeatherBall_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 乱気流は何もなし！
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (flowWk->Hnd_GetWeather() != BTL_WEATHER_NONE)
  &&  (flowWk->Hnd_GetWeather() != BTL_WEATHER_TURBULENCE)
  ){
    u32 pow = EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
    pow *= 2;
    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}

//----------------------------------------------------------------------------------
/**
 * たつまき・かぜおこし
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Tatumaki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_POKE_HIDE,   handler_Tatumaki_checkHide },  // 消えポケヒットチェック
    { BTL_EVENT_WAZA_POWER_BASE,   handler_Tatumaki },     // ダメージ最終チェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Tatumaki_checkHide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_POKE_HIDE) == BTL_POKEPARAM::CONTFLG_SORAWOTOBU ){
      EVENTVAR_RewriteValue( BTL_EVAR_AVOID_FLAG, false );
    }
  }
}
void handler_Tatumaki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( target->CONTFLAG_Get( BTL_POKEPARAM::CONTFLG_SORAWOTOBU) )
    {
      HandCommon_MulWazaBasePower( 2 );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * スカイアッパー
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_SkyUpper( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_POKE_HIDE,   handler_SkyUpper },  // 消えポケヒットチェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_SkyUpper( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_POKE_HIDE) == BTL_POKEPARAM::CONTFLG_SORAWOTOBU ){
      EVENTVAR_RewriteValue( BTL_EVAR_AVOID_FLAG, false );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * かみなり・ぼうふう
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Kaminari( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_POKE_HIDE,   handler_Kaminari_checkHide     },  // 消えポケヒットチェック
    { BTL_EVENT_CALC_HIT_CANCEL,   handler_Kaminari_excuseHitCalc },  // ヒット確率計算スキップ
    { BTL_EVENT_WAZA_HIT_RATIO,    handler_Kaminari_hitRatio      },  // 命中率計算
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 消えポケヒットチェックハンドラ
void handler_Kaminari_checkHide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_POKE_HIDE) == BTL_POKEPARAM::CONTFLG_SORAWOTOBU ){
      EVENTVAR_RewriteValue( BTL_EVAR_AVOID_FLAG, false );
    }
  }
}
// ヒット確率計算スキップハンドラ
void handler_Kaminari_excuseHitCalc( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( calc::IsRainWeather(flowWk->Hnd_GetWeather()) )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
    }
  }
}
// 命中率計算ハンドラ
void handler_Kaminari_hitRatio( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( calc::IsShineWeather(flowWk->Hnd_GetWeather()) )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_HIT_PER, 50 );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ふぶき
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Fubuki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_HIT_CANCEL,   handler_Fubuki },      // ヒット確率計算スキップハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ヒット確率計算スキップハンドラ
void handler_Fubuki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( flowWk->Hnd_GetWeather() == BTL_WEATHER_SNOW ){
      EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ぜったいれいど
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_ZettaiReido( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO_ICHIGEKI, handler_ZettaiReido_hitRatio },  // 命中率計算( 一撃必殺技 )
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 命中率計算ハンドラ
void handler_ZettaiReido_hitRatio( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  const u8 attackerID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
  if( attackerID != pokeID )
  {
    return;
  }

  // こおりタイプ以外が使用すると、技の命中率が20%になる
  const BTL_POKEPARAM* attacker = flowWk->Hnd_GetPokeParam( attackerID );
  if( !attacker->IsMatchType( POKETYPE_KOORI ) )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_HIT_PER, 20 );
  }
}
//----------------------------------------------------------------------------------
/**
 * じしん
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Jisin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_POKE_HIDE,   handler_Jisin_checkHide },  // 消えポケヒットチェック
    { BTL_EVENT_WAZA_DMG_PROC3,    handler_Jisin_damage },     // ダメージ最終チェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Jisin_checkHide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_POKE_HIDE) == BTL_POKEPARAM::CONTFLG_ANAWOHORU ){
      EVENTVAR_RewriteValue( BTL_EVAR_AVOID_FLAG, false );
    }
  }
}
void handler_Jisin_damage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // あなをほる状態の相手に２倍
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( target->CONTFLAG_Get( BTL_POKEPARAM::CONTFLG_ANAWOHORU) )
    {
      EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * さばきのつぶて
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_SabakiNoTubute( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM,   handler_SabakiNoTubute },  // わざパラメータチェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_SabakiNoTubute( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (flowWk->Hnd_CheckItemUsable(pokeID) )
  ){
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u16 item = bpp->GetItem( );
    u8 type = POKETYPE_NORMAL;

    switch( item ){
    case ITEM_HINOTAMAPUREETO:  type = POKETYPE_HONOO; break;
    case ITEM_SIZUKUPUREETO:    type = POKETYPE_MIZU; break;
    case ITEM_IKAZUTIPUREETO:   type = POKETYPE_DENKI; break;
    case ITEM_MIDORINOPUREETO:  type = POKETYPE_KUSA; break;
    case ITEM_TURARANOPUREETO:  type = POKETYPE_KOORI; break;
    case ITEM_KOBUSINOPUREETO:  type = POKETYPE_KAKUTOU; break;
    case ITEM_MOUDOKUPUREETO:   type = POKETYPE_DOKU; break;
    case ITEM_DAITINOPUREETO:   type = POKETYPE_JIMEN; break;
    case ITEM_AOZORAPUREETO:    type = POKETYPE_HIKOU; break;
    case ITEM_HUSIGINOPUREETO:  type = POKETYPE_ESPER; break;
    case ITEM_TAMAMUSIPUREETO:  type = POKETYPE_MUSHI; break;
    case ITEM_GANSEKIPUREETO:   type = POKETYPE_IWA; break;
    case ITEM_MONONOKEPUREETO:  type = POKETYPE_GHOST; break;
    case ITEM_RYUUNOPUREETO:    type = POKETYPE_DRAGON; break;
    case ITEM_KOWAMOTEPUREETO:  type = POKETYPE_AKU; break;
    case ITEM_KOUTETUPUREETO:   type = POKETYPE_HAGANE; break;
    case ITEM_SEIREIPUREETO:    type = POKETYPE_FAIRY;  break;
    }

    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, type );
  }
}

//----------------------------------------------------------------------------------
/**
 * マルチアタック
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_MultiAttack( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM,   handler_MultiAttack },  // わざパラメータチェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_MultiAttack( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (flowWk->Hnd_CheckItemUsable(pokeID) )
  ){
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u16 item = bpp->GetItem( );
    u8 type = POKETYPE_NORMAL;

    switch( item ){
    case ITEM_FAIYAAMEMORI:    type = POKETYPE_HONOO; break;
    case ITEM_UOOTAAMEMORI:    type = POKETYPE_MIZU; break;
    case ITEM_EREKUTOROMEMORI: type = POKETYPE_DENKI; break;
    case ITEM_GURASUMEMORI:    type = POKETYPE_KUSA; break;
    case ITEM_AISUMEMORI:      type = POKETYPE_KOORI; break;
    case ITEM_FAITOMEMORI:     type = POKETYPE_KAKUTOU; break;
    case ITEM_POIZUNMEMORI:    type = POKETYPE_DOKU; break;
    case ITEM_GURAUNDOMEMORI:  type = POKETYPE_JIMEN; break;
    case ITEM_HURAINGUMEMORI:  type = POKETYPE_HIKOU; break;
    case ITEM_SAIKIKKUMEMORI:  type = POKETYPE_ESPER; break;
    case ITEM_BAGUMEMORI:      type = POKETYPE_MUSHI; break;
    case ITEM_ROKKUMEMORI:     type = POKETYPE_IWA; break;
    case ITEM_GOOSUTOMEMORI:   type = POKETYPE_GHOST; break;
    case ITEM_DORAGONMEMORI:   type = POKETYPE_DRAGON; break;
    case ITEM_DAAKUMEMORI:     type = POKETYPE_AKU; break;
    case ITEM_SUTIIRUMEMORI:   type = POKETYPE_HAGANE; break;
    case ITEM_FEARIIMEMORI:    type = POKETYPE_FAIRY; break;
    }

    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, type );
  }
}

//----------------------------------------------------------------------------------
/**
 * テクノバスター
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_TechnoBaster( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM,   handler_TechnoBaster },  // わざパラメータチェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_TechnoBaster( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (flowWk->Hnd_CheckItemUsable(pokeID) )
  ){
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u16 item = bpp->GetItem( );
    u8 type = POKETYPE_NORMAL;
    u8 effIndex = BTLV_WAZAEFF_TECKNOBASTER_NORMAL;

    switch( item ){
    case ITEM_AKUAKASETTO:    // アクアカセット
      type = POKETYPE_MIZU;
      effIndex = BTLV_WAZAEFF_TECKNOBASTER_MIZU;
      break;
    case ITEM_INAZUMAKASETTO: // イナズマカセット
      type = POKETYPE_DENKI;
      effIndex = BTLV_WAZAEFF_TECKNOBASTER_DENKI;
      break;
    case ITEM_BUREIZUKASETTO: // ブレイズカセット
      type = POKETYPE_HONOO;
      effIndex = BTLV_WAZAEFF_TECKNOBASTER_HONOO;
      break;
    case ITEM_HURIIZUKASETTO: // フリーズカセット
      type = POKETYPE_KOORI;
      effIndex = BTLV_WAZAEFF_TECKNOBASTER_KOORI;
      break;
    }

    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, type );
    flowWk->Hnd_SetWazaEffectIndex( effIndex );
  }
}

//----------------------------------------------------------------------------------
/**
 * めざめるパワー
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_MezameruPower( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM,        handler_MezameruPower_Type },  // わざパラメータチェック
//    { BTL_EVENT_WAZA_POWER_BASE,   handler_MezameruPower_Pow  },  // わざ威力チェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_MezameruPower_Type( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    pml::PokeType type = HandCommon_GetMezapaType( flowWk, pokeID );
    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, type );
  }
}
/*
void handler_MezameruPower_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    const pml::pokepara::PokemonParam* pp = bpp->GetSrcData( );
    u32 pow = pp->GetMezapaPower();

    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
*/
//----------------------------------------------------------------------------------
/**
 * しぜんのめぐみ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_SizenNoMegumi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD, handler_SizenNoMegumi_ExeCheck    }, // わざ出し成否チェック
    { BTL_EVENT_WAZA_PARAM,             handler_SizenNoMegumi_Type        }, // わざパラメータチェック
    { BTL_EVENT_WAZA_POWER_BASE,        handler_SizenNoMegumi_Pow         }, // わざ威力チェック
    { BTL_EVENT_REWRITE_AFFINITY,       handler_SizenNoMegumi_CheckAffine }, // タイプ相性による無効チェック
    { BTL_EVENT_WAZA_EXECUTE_DONE,      handler_SizenNoMegumi_Done        }, // わざ出し終了
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// わざ出し成否チェック
void handler_SizenNoMegumi_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( !SizenNoMegumi_CheckSuccess( flowWk, pokeID ) )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
    }
  }
}
// わざパラメータチェック
void handler_SizenNoMegumi_Type( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u16 itemID = bpp->GetItem( );
    BTL_PRINT("「しぜんのめぐみ] タイプチェック：itemID=%d\n", itemID);
    s32 type = calc::ITEM_GetParam( itemID, ::item::ITEM_DATA::PRM_ID_SIZEN_TYPE );

    // @fix NMCat[49] 技「しぜんのめぐみ」を覚えていてきのみ以外の道具を持っているポケモンが戦おうとするととまる
    // sango までは、「しぜんのめぐみ」威力がゼロに設定されている場合、技出し自体に失敗するため、type が不正値でも、以降の相性計算が行われないため、停止しませんでした。
    // niji では、技選択画面でタイプ相性を表示するために、「たたかう」を選択したときにタイプ相性を計算します。
    // このとき、type が不正値のまま相性計算が行われ、アサートに引っかかってしまいました。
    // niji では、道具データに「しぜんのめぐみ」のタイプが設定されていないなら、「ノーマル」タイプとして相性計算を行うように変更します。
    if( POKETYPE_MAX <= type )
    {
      type = POKETYPE_NORMAL;
    }

    BTL_PRINT("　　　type=%d\n", type);
    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, type );
  }
}
// わざ威力チェック
void handler_SizenNoMegumi_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u16 itemID = bpp->GetItem( );
    s32 pow = calc::ITEM_GetParam( itemID, ::item::ITEM_DATA::PRM_ID_SIZEN_ATC );

    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
// タイプ相性による無効チェック
void handler_SizenNoMegumi_CheckAffine( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( ( EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK ) == pokeID ) && 
      !SizenNoMegumi_CheckSuccess( flowWk, pokeID ) )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_TYPEAFF, pml::battle::TypeAffinity::TYPEAFF_0 );
  }
}
// ワザ出し終了
void handler_SizenNoMegumi_Done( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_CONSUME_ITEM* param = reinterpret_cast<BTL_HANDEX_PARAM_CONSUME_ITEM *>( flowWk->HANDEX_Push( BTL_HANDEX_CONSUME_ITEM, pokeID ));
      param->fNoAction = true;
      param->fNoAteKinomi = true;
    flowWk->HANDEX_Pop( param );
  }
}

/**
 * @brief 「しぜんのめぐみ」が成功するかどうかチェックする
 * @param pokeID  「しぜんのめぐみ」を使用するポケモンのID
 * @retval true   成功する
 * @retval false  失敗する
 */
bool SizenNoMegumi_CheckSuccess( ServerFlow* flowWk, u8 pokeID )
{
  if( !flowWk->Hnd_CheckItemUsable(pokeID) )
  {
    return false;
  }
  else
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u16 itemID = bpp->GetItem();
    int prm = calc::ITEM_GetParam( itemID, ::item::ITEM_DATA::PRM_ID_SIZEN_ATC );

    // 所持アイテムのしぜんのめぐみパラメータが無効値（0）なら失敗する
    if( (itemID == ITEM_DUMMY_DATA)
    ||  (prm == 0)
    ){
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------
/**
 * はたきおとす
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Hatakiotosu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,                handler_Hatakiotosu_WazaPow },  // ワザ威力チェックハンドラ
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL,   handler_Hatakiotosu_EndHit  },  // ダメージプロセス終了ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザ威力チェックハンドラ
void handler_Hatakiotosu_WazaPow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 相手がアイテムを持っていたら２倍
    u8  target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( target_pokeID );
    if( (target->GetItem() != ITEM_DUMMY_DATA)
    &&  (!HandCommon_CheckCantStealPoke(flowWk, pokeID, target_pokeID))
    ){
      EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.5f) );
    }
  }
}
// ダメージプロセス終了ハンドラ
void handler_Hatakiotosu_EndHit( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    if( !HandCommon_CheckCantStealPoke(flowWk, pokeID, target_pokeID) )
    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( target_pokeID );
      u16 itemID = bpp->GetItem( );
      if( itemID != ITEM_DUMMY_DATA )
      {
        BTL_HANDEX_PARAM_SET_ITEM* param = reinterpret_cast<BTL_HANDEX_PARAM_SET_ITEM *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_ITEM, pokeID ));
          param->pokeID = target_pokeID;
          param->itemID = ITEM_DUMMY_DATA;
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Hatakiotosu );
          HANDEX_STR_AddArg( &param->exStr, pokeID );
          HANDEX_STR_AddArg( &param->exStr, target_pokeID );
          HANDEX_STR_AddArg( &param->exStr, itemID );
        flowWk->HANDEX_Pop( param );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * マジックコート
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_MagicCoat( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD,       handler_MagicCoat_ExeCheck  }, // ワザ出し成否チェックハンドラ
    { BTL_EVENT_UNCATEGORIZE_WAZA,            handler_MagicCoat           }, // 未分類ワザ処理
    { BTL_EVENT_NOEFFECT_CHECK_TOKUSEI,            handler_MagicCoat_Wait      }, // ワザ無効化ハンドラ
    { BTL_EVENT_WAZASEQ_REFRECT,              handler_MagicCoat_Reflect   }, // ワザ乗っ取り確定
    { BTL_EVENT_TURNCHECK_BEGIN,              handler_MagicCoat_TurnCheck }, // ターンチェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザ出し成否チェックハンドラ
void handler_MagicCoat_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  u8 wazaUserPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );

  if( wazaUserPokeID == pokeID )
  {
    // 現ターン最後の「マジックコート」なら失敗
    if( HandCommon_IsPokeOrderLast(flowWk, pokeID) &&
        ( EVENTVAR_GetValue( BTL_EVAR_WAZAID) == EVENT_FACTOR_GetSubID(myHandle) ) )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
    }
  }
  else if( !flowWk->Hnd_GetMainModule()->IsFriendPokeID(pokeID, wazaUserPokeID) )
  {
    HandCommon_MagicCoat_CheckSideEffWaza( myHandle, flowWk, pokeID, work );
  }
}
// 未分類ワザ処理
void handler_MagicCoat( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( work[WORKIDX_STICK] == 0 )
    {
      BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_MagicCoatReady );
        HANDEX_STR_AddArg( &param->str, pokeID );
      flowWk->HANDEX_Pop( param );
      work[ WORKIDX_STICK ] = 1;
    }
  }
}
// ワザ無効化ハンドラ
void handler_MagicCoat_Wait( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  HandCommon_MagicCoat_Wait( myHandle, flowWk, pokeID, work );
}
// マジックコート発動ハンドラ
void handler_MagicCoat_Reflect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( EVENTVAR_RewriteValue(BTL_EVAR_GEN_FLAG, true) )
    {
      HandCommon_MagicCoat_Reaction( myHandle, flowWk, pokeID, work );
    }
  }
}
void handler_MagicCoat_TurnCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // ターンチェックで自殺
    EVENT_FACTOR_Remove( myHandle );
  }
}
//----------------------------------------------------------------------------------
/**
 * よこどり
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Yokodori( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,   handler_Yokodori           }, // 未分類ワザ処理
    { BTL_EVENT_CHECK_WAZA_ROB,      handler_Yokodori_CheckRob  }, // 乗っ取り判定
    { BTL_EVENT_WAZASEQ_ROB,         handler_Yokodori_Rob       }, // 乗っ取り確定
    { BTL_EVENT_TURNCHECK_BEGIN,     handler_Yokodori_TurnCheck }, // ターンチェック

//    { BTL_EVENT_WAZASEQ_REFRECT,     handler_Yokodori_MagicMirror },  // （マジックミラーによる）跳ね返し確定
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 未分類ワザ処理
void handler_Yokodori( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (!HandCommon_IsPokeOrderLast(flowWk, pokeID))
  ){
    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_YokodoriReady );
      HANDEX_STR_AddArg( &param->str, pokeID );
    flowWk->HANDEX_Pop( param );
    work[ WORKIDX_STICK ] = 1;
  }
}
// （マジックミラーによる）跳ね返し確定
void handler_Yokodori_MagicMirror( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // よこどり待機中にマジックミラーで跳ね返した場合、待機を解除
    if( work[WORKIDX_STICK] )
    {
      EVENT_FACTOR_Remove( myHandle );
    }
  }
}
void handler_Yokodori_CheckRob( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  u8 atkPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );

  if( atkPokeID != pokeID )
  {
    WazaNo waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    bool isYokodoriEnableWaza = WAZADATA_GetFlag(waza, pml::wazadata::WAZAFLAG_Yokodori);
    bool isZenryokuWaza = ( EVENTVAR_GetValue( BTL_EVAR_ZENRYOKU_WAZA_FLAG ) != 0 );

    if( isYokodoriEnableWaza && !isZenryokuWaza )
    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
      if( !bpp->CheckSick( pml::wazadata::WAZASICK_FREEFALL) )
      {
        if( EVENTVAR_RewriteValue(BTL_EVAR_POKEID, pokeID) )
        {
          BTL_N_Printf( DBGSTR_HANDWAZA_YokodoriDecide, pokeID );
          EVENTVAR_RewriteValue( BTL_EVAR_POKEID_DEF, pokeID );
          work[ 0 ] = 1;
        }
      }
    }
  }
}
void handler_Yokodori_Rob( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (work[0])
  ){
    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_YokodoriExe );
      HANDEX_STR_AddArg( &param->str, pokeID );
      HANDEX_STR_AddArg( &param->str, EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) );
    flowWk->HANDEX_Pop( param );
    EVENT_FACTOR_Remove( myHandle );
  }
}
void handler_Yokodori_TurnCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID ){
    EVENT_FACTOR_Remove( myHandle );
  }
}
//----------------------------------------------------------------------------------
/**
 * どろぼう・ほしがる
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Dorobou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,          handler_Dorobou_Start }, // ダメージプロセス開始ハンドラ
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL,   handler_Dorobou       }, // ダメージプロセス終了ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージプロセス開始ハンドラ
void handler_Dorobou_Start( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  HandCommon_DorobouStart( flowWk, pokeID, work );
}
// ダメージプロセス終了ハンドラ
void handler_Dorobou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID ){
    return;
  }
  if( !HandCommon_IsDorobouEnable(flowWk, pokeID, work) ){
    return;
  }

  u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
  if( target_pokeID == BTL_POKEID_NULL ){
    return;
  }

  const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( target_pokeID );
  if( target->GetItem() != ITEM_DUMMY_DATA )
  {
    if( (!HandCommon_CheckCantStealPoke(flowWk, pokeID, target_pokeID)) )
    {
      BTL_HANDEX_PARAM_SWAP_ITEM* param = reinterpret_cast<BTL_HANDEX_PARAM_SWAP_ITEM *>( flowWk->HANDEX_Push( BTL_HANDEX_SWAP_ITEM, pokeID ));
      BTL_HANDEX_PARAM_SET_EFFECT_IDX* effParam;

        param->pokeID = target_pokeID;
        param->fIncRecordCount_StealItemFromWildPoke = true;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Dorobou );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
        HANDEX_STR_AddArg( &param->exStr, target_pokeID );
        HANDEX_STR_AddArg( &param->exStr, target->GetItem() );
      flowWk->HANDEX_Pop( param );

      effParam = reinterpret_cast<BTL_HANDEX_PARAM_SET_EFFECT_IDX *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_EFFECT_IDX, pokeID ));
        effParam->header.failSkipFlag = true;
        effParam->effIndex = BTLV_WAZAEFF_DOROBOU_STEAL;
      flowWk->HANDEX_Pop( effParam );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * トリック・すりかえ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Trick( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,      handler_Trick }, // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Trick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );

    if( !HandCommon_CheckCantStealWildPoke(flowWk, pokeID) )
    {
      const BTL_POKEPARAM* self = flowWk->Hnd_GetPokeParam( pokeID );
      const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( target_pokeID );

      u16 selfItemID = self->GetItem( );
      u16 targetItemID = target->GetItem( );

      if( (selfItemID != ITEM_DUMMY_DATA) || (targetItemID != ITEM_DUMMY_DATA) )
      {
        u16 myMonsNo = self->GetMonsNo( );
        u16 targetMonsNo = target->GetMonsNo( );

        if( (!calc::ITEM_IsMail(selfItemID))
        &&  (!calc::ITEM_IsMail(targetItemID))
        &&  (!HandCommon_IsUnbreakablePokeItem(flowWk, myMonsNo, targetItemID))
        &&  (!HandCommon_IsUnbreakablePokeItem(flowWk, targetMonsNo, selfItemID))
        &&  (!HandCommon_IsUnbreakablePokeItem(flowWk, myMonsNo, selfItemID))
        &&  (!HandCommon_IsUnbreakablePokeItem(flowWk, targetMonsNo, targetItemID))
        ){
          BTL_HANDEX_PARAM_SWAP_ITEM* param = reinterpret_cast<BTL_HANDEX_PARAM_SWAP_ITEM *>( flowWk->HANDEX_Push( BTL_HANDEX_SWAP_ITEM, pokeID ));

            param->pokeID = target_pokeID;
            param->fIncRecordCount_StealItemFromWildPoke = true;
            HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Trick );
            HANDEX_STR_AddArg( &param->exStr, pokeID );

            if( targetItemID != ITEM_DUMMY_DATA ){
              HANDEX_STR_Setup( &param->exSubStr2, BTL_STRTYPE_SET, BTL_STRID_SET_TrickGetItem );
              HANDEX_STR_AddArg( &param->exSubStr2, pokeID );
              HANDEX_STR_AddArg( &param->exSubStr2, targetItemID );
            }
            if( selfItemID != ITEM_DUMMY_DATA ){
              HANDEX_STR_Setup( &param->exSubStr1, BTL_STRTYPE_SET, BTL_STRID_SET_TrickGetItem );
              HANDEX_STR_AddArg( &param->exSubStr1, target_pokeID );
              HANDEX_STR_AddArg( &param->exSubStr1, selfItemID );
            }
          flowWk->HANDEX_Pop( param );
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * マグニチュード
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Magnitude( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_POKE_HIDE,     handler_Jisin_checkHide  },  // 消えポケヒットチェック
    { BTL_EVENT_WAZA_DMG_PROC3,      handler_Jisin_damage     },  // ダメージ最終チェック
    { BTL_EVENT_WAZA_EXE_START,      handler_Magnitude_effect },  // ワザ出し確定
    { BTL_EVENT_WAZA_POWER_BASE,     handler_Magnitude_pow    },  // ワザ威力計算
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

void handler_Magnitude_effect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    static const struct {
      u8  per;
      u8  pow;
    }powTbl[] = {
      {   5,  10 },
      {  15,  30 },
      {  35,  50 },
      {  65,  70 },
      {  85,  90 },
      {  95, 110 },
      { 100, 150 },
    };

    u8 i, per = calc::GetRand( 100 );

    for(i=0; i<GFL_NELEMS(powTbl); ++i)
    {
      if( per < powTbl[i].per ){
        break;
      }
    }

    // work[0] に威力値を入れておく
    work[0] = powTbl[i].pow;

    {
      BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_Magnitude1 + i );
      flowWk->HANDEX_Pop( param );
    }
  }
}
void handler_Magnitude_pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 有り得ないハズだがねんのため
    if( work[0] == 0 ){
      work[0] = 10;
    }
    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, work[0] );
  }
}

//----------------------------------------------------------------------------------
/**
 * なみのり
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Naminori( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_POKE_HIDE,   handler_Naminori_checkHide },  // 消えポケヒットチェック
    { BTL_EVENT_WAZA_DMG_PROC3,    handler_Naminori },     // ダメージ最終チェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Naminori_checkHide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_POKE_HIDE) == BTL_POKEPARAM::CONTFLG_DIVING ){
      EVENTVAR_RewriteValue( BTL_EVAR_AVOID_FLAG, false );
    }
  }
}
void handler_Naminori( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ダイビング状態の相手に２倍ダメージ
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( target->CONTFLAG_Get( BTL_POKEPARAM::CONTFLG_DIVING) )
    {
      EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ふみつけ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Fumituke( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_HIT_CANCEL,   handler_Fumituke_HitCheckSkip }, // 命中計算スキップ
    { BTL_EVENT_WAZA_DMG_PROC3,    handler_Fumituke_DamegeProc   }, // ダメージ最終チェック

  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// 命中計算スキップ
void handler_Fumituke_HitCheckSkip( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ちいさくなる状態の相手に命中させる
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( target->CONTFLAG_Get( BTL_POKEPARAM::CONTFLG_TIISAKUNARU) )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
    }
  }
}

// ダメージ最終チェック
void handler_Fumituke_DamegeProc( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ちいさくなる状態の相手に２倍ダメージ
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( target->CONTFLAG_Get( BTL_POKEPARAM::CONTFLG_TIISAKUNARU) )
    {
      EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}


//----------------------------------------------------------------------------------
/**
 * みねうち
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Mineuti( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_KORAERU_CHECK, handler_Mineuti },    // こらえるチェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Mineuti( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_GEN_FLAG) ){
      EVENTVAR_RewriteValue( BTL_EVAR_KORAERU_CAUSE,  BTL_POKEPARAM::KORAE_WAZA_ATTACKER );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * こらえる
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Koraeru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASEQ_START,           handler_Mamoru_StartSeq }, // ワザ処理開始
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD,  handler_Mamoru_ExeCheck }, // ワザ出し成否チェックハンドラ
    { BTL_EVENT_WAZA_EXECUTE_FAIL,       handler_Mamoru_ExeFail  }, // ワザだし失敗確定
    { BTL_EVENT_UNCATEGORIZE_WAZA,       handler_Koraeru },           // 未分類ワザハンドラ
    { BTL_EVENT_KORAERU_CHECK,           handler_Koraeru_Check },     // こらえるチェックハンドラ
    { BTL_EVENT_TURNCHECK_BEGIN,         handler_Koraeru_TurnCheck }, // ターンチェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザ出し成否チェックハンドラ
void handler_Koraeru_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  // 成功率がだんだん下がるテーブル 1/2, 1/4, 1/8, ---
  static const u8 randRange[] = {
    2, 4, 8,
  };

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u8 counter = bpp->GetWazaContCounter( );

    if( counter )
    {
      if( counter >= GFL_NELEMS(randRange) ){
        counter = GFL_NELEMS(randRange) - 1;
      }
      if( calc::GetRand( randRange[counter] ) != 0 )
      {
        // 連続利用による失敗。失敗したらハンドラごと消滅することで、こらえるチェックに反応しない。
        EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
        EVENT_FACTOR_Remove( myHandle );
      }
    }
  }
}
// 未分類ワザハンドラ
void handler_Koraeru( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ワザが出たら貼り付いて「こらえる」体勢になる
    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Koraeru_Ready );
      HANDEX_STR_AddArg( &param->str, pokeID );
    flowWk->HANDEX_Pop( param );

    work[ WORKIDX_STICK ] = 1;
    IncrementMamoruCounter( flowWk, pokeID, false );
  }
}
// こらえるチェックハンドラ
void handler_Koraeru_Check( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // 貼り付き中なら「こらえる」発動
    if( work[ WORKIDX_STICK] )
    {
      if( EVENTVAR_GetValue(BTL_EVAR_GEN_FLAG) ){
        EVENTVAR_RewriteValue( BTL_EVAR_KORAERU_CAUSE, BTL_POKEPARAM::KORAE_WAZA_DEFENDER );
      }
//      EVENT_FACTOR_Remove( myHandle );  // １ターン２回以上こらえるためにコメントアウト
    }
  }
}
// ターンチェックハンドラ
void handler_Koraeru_TurnCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // ターンチェックまで貼り付いていたら自分自身を削除
    EVENT_FACTOR_Remove( myHandle );
  }
}


//----------------------------------------------------------------------------------
/**
 * まもる・みきり
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Mamoru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASEQ_START,           handler_Mamoru_StartSeq         }, // ワザ処理開始
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD,  handler_Mamoru_ExeCheck         }, // ワザ出し成否チェックハンドラ
    { BTL_EVENT_WAZA_EXECUTE_FAIL,       handler_Mamoru_ExeFail          }, // ワザだし失敗確定
    { BTL_EVENT_UNCATEGORIZE_WAZA,       handler_Mamoru                  }, // 未分類ワザハンドラ
    { BTL_EVENT_WAZA_DMG_PROC_Z,         handler_Mamoru_DmgZ             }, // Z技に対するダメージ補正
    { BTL_EVENT_AFTER_CRITICAL,          handler_Mamoru_MsgAfterCritical }, // 急所メッセージ後
    { BTL_EVENT_TURNCHECK_BEGIN,         handler_Mamoru_Off              }, // ターンチェック開始ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザ処理開始ハンドラ
void handler_Mamoru_StartSeq( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_WAZAID) != EVENT_FACTOR_GetSubID(myHandle) )
  {
    return;
  }

  static const WazaID checkWazaTbl[] = {
    WAZANO_MAMORU, WAZANO_MIKIRI, WAZANO_KORAERU,
    WAZANO_WAIDOGAADO, WAZANO_FASUTOGAADO,
    WAZANO_NIIDORUGAADO, WAZANO_KINGUSIIRUDO,
    WAZANO_TATAMIGAESI, WAZANO_TOOTIKA,
  };

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    WazaID  waza = bpp->GetPrevWazaID();
    u32 i;
    for(i=0; i<GFL_NELEMS(checkWazaTbl); ++i){
      if( checkWazaTbl[i] == waza ){
        return;
      }
    }

    // ワザ初出 or カウンタ共有ワザ以外のワザを直前に出していたらリセット
    {

      BTL_HANDEX_PARAM_COUNTER*  counterParam = reinterpret_cast<BTL_HANDEX_PARAM_COUNTER *>( flowWk->HANDEX_Push( BTL_HANDEX_COUNTER, pokeID ));
        counterParam->pokeID = pokeID;
        counterParam->counterID = BTL_POKEPARAM::COUNTER_MAMORU;
        counterParam->value = 0;
      flowWk->HANDEX_Pop( counterParam );
    }
  }
}
// ワザ出し成否チェックハンドラ
void handler_Mamoru_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);

  // @fix NMCat[67]さいはいでキングシールドを出しながらワザを出すと失敗することがある
  // 原因：技ハンドラ自身の技( キングシールド・まもる等 )以外の技に反応してしまう
  // 対処：業ハンドラ自身の技( キングシールド・まもる等 )以外の技に反応しないようにする
  if( EVENTVAR_GetValue(BTL_EVAR_WAZAID) != EVENT_FACTOR_GetSubID(myHandle) )
  {
    return;
  }

  // 成功率がだんだん下がるテーブル 1/1, 1/3, 1/9, 1/27, ---
  static const u16 randRange[] = {
    1, 3, 9, 27, 81, 243, 729,
  };

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u8 counter = bpp->COUNTER_Get( BTL_POKEPARAM::COUNTER_MAMORU );

    // 連続利用による失敗チェック
    if( counter )
    {
      if( counter >= GFL_NELEMS(randRange) ){
        counter = GFL_NELEMS(randRange) - 1;
      }
      if( calc::GetRand( randRange[counter] ) != 0 )
      {
        EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
        return;
      }
    }

    // 現ターン最後の行動なら失敗
    if( HandCommon_IsPokeOrderLast(flowWk, pokeID) ){
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
    }
  }
}
// ワザ出し失敗確定
void handler_Mamoru_ExeFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);

  // @fix NMCat[67]さいはいでキングシールドを出しながらワザを出すと失敗することがある
  // 原因：技ハンドラ自身の技( キングシールド・まもる等 )以外の技に反応してしまう
  // 対処：業ハンドラ自身の技( キングシールド・まもる等 )以外の技に反応しないようにする
  if( EVENTVAR_GetValue(BTL_EVAR_WAZAID) != EVENT_FACTOR_GetSubID(myHandle) )
  {
    return;
  }

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // 失敗したらカウンタをリセット＆ハンドラごと消滅することで、その後のイベントに反応しない。
    BTL_HANDEX_PARAM_COUNTER*  counterParam = reinterpret_cast<BTL_HANDEX_PARAM_COUNTER *>( flowWk->HANDEX_Push( BTL_HANDEX_COUNTER, pokeID ));
      counterParam->pokeID = pokeID;
      counterParam->counterID = BTL_POKEPARAM::COUNTER_MAMORU;
      counterParam->value = 0;
    flowWk->HANDEX_Pop( counterParam );

    EVENT_FACTOR_Remove( myHandle );
  }
}
// 未分類ワザハンドラ
void handler_Mamoru( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  handler_Mamoru_Core( flowWk, pokeID );
  work[ WORKIDX_STICK ] = 1;
}
// 「まもる」系のコア処理
void handler_Mamoru_Core( ServerFlow* flowWk, u8 pokeID )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_TURNFLAG* flagParam;
    BTL_HANDEX_PARAM_MESSAGE*  msgParam;

    flagParam = reinterpret_cast<BTL_HANDEX_PARAM_TURNFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_TURNFLAG, pokeID ));
      flagParam->pokeID = pokeID;
      flagParam->flag = BTL_POKEPARAM::TURNFLG_MAMORU;
    flowWk->HANDEX_Pop( flagParam );

    // 守る効果発動したらカウンタをインクリメント
    IncrementMamoruCounter( flowWk, pokeID, false );

    msgParam = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &msgParam->str, BTL_STRTYPE_SET, BTL_STRID_SET_MamoruReady );
      HANDEX_STR_AddArg( &msgParam->str, pokeID );
    flowWk->HANDEX_Pop( msgParam );
  }
}

/**
 * 「まもる」カウンタをインクリメント
 *
 * @param   flowWk
 * @param   pokeID
 * @param   fFailSkip   直前のHandEx命令が失敗していたらインクリメントしない（ワイドガード等に対応）
 */
void IncrementMamoruCounter( ServerFlow* flowWk, u8 pokeID, bool fFailSkip )
{
  BTL_HANDEX_PARAM_COUNTER*  param;
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

  param = reinterpret_cast<BTL_HANDEX_PARAM_COUNTER *>( flowWk->HANDEX_Push( BTL_HANDEX_COUNTER, pokeID ));
    param->value = bpp->COUNTER_Get( BTL_POKEPARAM::COUNTER_MAMORU ) + 1;
    param->pokeID = pokeID;
    param->counterID = BTL_POKEPARAM::COUNTER_MAMORU;
    param->header.failSkipFlag = fFailSkip;
  flowWk->HANDEX_Pop( param );
}

// Z技に対するダメージ補正
void handler_Mamoru_DmgZ( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  mamoru_DmgZ( flowWk, pokeID, work );
}

/**
 * @brief [まもる] Z技に対するダメージ補正
 * @retval true   ダメージを軽減した
 * @retval false  ダメージを軽減しなかった
 */
bool mamoru_DmgZ( ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) != pokeID )
  {
    return false;
  }

  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  if( !bpp->TURNFLAG_Get( BTL_POKEPARAM::TURNFLG_MAMORU ) )
  {
    return false;
  }

  if( EVENTVAR_RewriteValue( BTL_EVAR_RATIO_EX, FX32_CONST(0.25) ) )
  {
    work[0] = 1;
    return true;
  }

  return false;
}

// 急所メッセージ後
void handler_Mamoru_MsgAfterCritical( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  mamoru_MsgAfterCritical( flowWk, pokeID, work );
}

/**
 * @brief [まもる] 急所メッセージ後の特殊メッセージ
 * @retval true   特殊メッセージを表示した
 * @retval false  特殊メッセージを表示しなかった
 */
bool mamoru_MsgAfterCritical( ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( ( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID ) &&
      ( work[0] == 1 ) )
  {
     BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
       HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_protectpierce );  //「こうげきを　まもりきれずに　ダメージを　うけた！」
       HANDEX_STR_AddArg( &param->str, pokeID );
     flowWk->HANDEX_Pop( param );

     work[0] = 0;

     return true;
  }

  return false;
}

// ターンチェックで効果を落とす
void handler_Mamoru_Off( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == BTL_POKEID_NULL )
  {
    EVENT_FACTOR_Remove( myHandle );
  }
}

//----------------------------------------------------------------------------------
/**
 * がまん
 */
//----------------------------------------------------------------------------------
enum {
  GAMAN_STATE_1ST = 0,
  GAMAN_STATE_2ND,
  GAMAN_STATE_3RD,
  GAMAN_STATE_END,
};
enum {
  GAMAN_WORKIDX_STATE = 0,
  GAMAN_WORKIDX_DAMAGE,
  GAMAN_WORKIDX_TARGET_POKEID,
};
const BtlEventHandlerTable*  ADD_Gaman( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXE_START,            handler_Gaman           }, //
    { BTL_EVENT_REQWAZA_MSG,               handler_Gaman_WazaMsg   },
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD,    handler_Gaman_ExeCheck  },
    { BTL_EVENT_DECIDE_TARGET,             handler_Gaman_Target    },
    { BTL_EVENT_WAZA_DMG_PROC1,            handler_Gaman_CalcDmg   },
    { BTL_EVENT_WAZA_EXECUTE_FAIL,         handler_Gaman_Fail      },
    { BTL_EVENT_WAZA_DMG_REACTION,         handler_Gaman_DmgRec    },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Gaman( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( HandCommon_IsMyEvent(myHandle, BTL_EVAR_POKEID, pokeID))
  {
    switch( work[GAMAN_WORKIDX_STATE] ){
    case GAMAN_STATE_1ST:
      // １ターン目：自分をワザロック状態にして、処理を抜ける。
      {
        BTL_HANDEX_PARAM_ADD_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
          param->sickID = pml::wazadata::WAZASICK_WAZALOCK;
          param->sickCont = SICKCONT_MakePermanentParam( pokeID, EVENT_FACTOR_GetSubID(myHandle) );
          param->pokeID = pokeID;
        flowWk->HANDEX_Pop( param );

        BTL_HANDEX_PARAM_SET_CONTFLAG* flagParam = reinterpret_cast<BTL_HANDEX_PARAM_SET_CONTFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_CONTFLAG, pokeID ) );
          flagParam->pokeID = pokeID;
          flagParam->flag   = BTL_POKEPARAM::CONTFLG_GAMAN;
        flowWk->HANDEX_Pop( flagParam );

        EVENTVAR_RewriteValue( BTL_EVAR_ENABLE_MODE, ServerFlow::WAZAENABLE_QUIT );
        work[WORKIDX_STICK] = 1;
        work[GAMAN_WORKIDX_STATE] = GAMAN_STATE_2ND;
        work[ GAMAN_WORKIDX_DAMAGE ] = 0;
      }
      break;
    case GAMAN_STATE_2ND:
      // ２ターン目：何もせず処理を抜ける
      {
        EVENTVAR_RewriteValue( BTL_EVAR_ENABLE_MODE, ServerFlow::WAZAENABLE_QUIT );
        work[GAMAN_WORKIDX_STATE] = GAMAN_STATE_3RD;
      }
      break;
    case GAMAN_STATE_3RD:
    default:
      // ３ターン目：貼り付き＆ワザロック状態解除
      flowWk->Hnd_SetWazaEffectIndex( BTLV_WAZAEFF_GAMAN_ATTACK );
      gaman_ReleaseStick( flowWk, pokeID, work );
      break;
    }
  }
}
void handler_Gaman_WazaMsg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_STR_PARAMS* param = (BTL_HANDEX_STR_PARAMS*)EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );

    switch( work[GAMAN_WORKIDX_STATE] ){
    case GAMAN_STATE_2ND:
      HANDEX_STR_Setup( param, BTL_STRTYPE_SET, BTL_STRID_SET_Gaman );
      HANDEX_STR_AddArg( param, pokeID );
      EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
      break;
    case GAMAN_STATE_3RD:
      HANDEX_STR_Setup( param, BTL_STRTYPE_SET, BTL_STRID_SET_GamanEnd );
      HANDEX_STR_AddArg( param, pokeID );
      EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
      break;
    }
  }
}
// 実行チェック
void handler_Gaman_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);

  if( !HandCommon_IsMyEvent(myHandle, BTL_EVAR_POKEID, pokeID)){
    return;
  }

  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (work[GAMAN_WORKIDX_STATE] >= GAMAN_STATE_3RD)
  ){

    // ダメージを食らっていない場合は失敗
    if( work[GAMAN_WORKIDX_DAMAGE] == 0 ){
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
    }
    // 既に対象がいない場合も失敗
    else if( work[GAMAN_WORKIDX_TARGET_POKEID] == BTL_POKEID_NULL ){
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
    }
    work[GAMAN_WORKIDX_STATE] = GAMAN_STATE_END;
  }
}
// ターゲット指定
void handler_Gaman_Target( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( !HandCommon_IsMyEvent(myHandle, BTL_EVAR_POKEID_ATK, pokeID)){
    return;
  }

  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (work[0] >= GAMAN_STATE_3RD)
  ){
    work[ GAMAN_WORKIDX_TARGET_POKEID ] = gaman_getTargetPokeID( myHandle, flowWk, pokeID, work );
    EVENTVAR_RewriteValue( BTL_EVAR_POKEID_DEF, work[GAMAN_WORKIDX_TARGET_POKEID] );
  }
}
// ダメージ反応ハンドラ（受けたダメージを記憶しておく）
void handler_Gaman_DmgRec( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == false)
  ){
    if( work[WORKIDX_STICK] )
    {
      work[ GAMAN_WORKIDX_DAMAGE ] += EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
    }
  }
}
void handler_Gaman_CalcDmg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  if( !HandCommon_IsMyEvent(myHandle, BTL_EVAR_POKEID_ATK, pokeID)){
    return;
  }

  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (work[GAMAN_WORKIDX_STATE] >= GAMAN_STATE_3RD)
  ){
    u32 dmg_sum = work[ GAMAN_WORKIDX_DAMAGE ];
    if( dmg_sum ){
      dmg_sum *= 2;
    }
    EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE, dmg_sum );
    EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE_FLAG, true );
    
    //SBTS[2555] AIからのシミュレーションでワークが書き換わるのはまずい。
    if( !flowWk->Hnd_IsSimulationMode() )
    {
      work[0] = GAMAN_STATE_END;
    }
  }
}
void handler_Gaman_Fail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  // 失敗したらすぐに貼り付き＆ワザロック状態解除
  if( HandCommon_IsMyEvent(myHandle, BTL_EVAR_POKEID, pokeID))
  {
    gaman_ReleaseStick( flowWk, pokeID, work );
  }
}
u8 gaman_getTargetPokeID( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  u32 t, cnt;
  for(t=0; t<3; ++t)
  {
    cnt = bpp->WAZADMGREC_GetCount( t );
    if( cnt )
    {
      BTL_POKEPARAM::WAZADMG_REC  rec;
      u8 targetPokeID;

      // 直近に自分を殴ったポケモンが対象
      bpp->WAZADMGREC_Get( t, 0, &rec );
      targetPokeID = rec.pokeID;

      // 対象が既に場にいない場合（とんぼがえりなど）、ランダムで決定
      if( flowWk->Hnd_GetExistFrontPokePos(targetPokeID) == BTL_POS_NULL )
      {
        BtlExPos  exPos;
        u8  pokeIDAry[ BTL_EXIST_POS_MAX ];
        u8  poke_cnt;

        exPos = EXPOS_MAKE( BTL_EXPOS_AREA_ENEMY, flowWk->Hnd_PokeIDtoPokePos( pokeID) );
        poke_cnt = flowWk->Hnd_ExpandPokeID( exPos, pokeIDAry );
        if( poke_cnt ){
          u8 idx = calc::GetRand( poke_cnt );
          targetPokeID = pokeIDAry[ idx ];
        }
        else{
          targetPokeID = BTL_POKEID_NULL;
        }
      }
      return targetPokeID;
    }
  }
  return BTL_POKEID_NULL;
}

void gaman_ReleaseStick( ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_STICK] ){
    BTL_HANDEX_PARAM_CURE_SICK* cureSick = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
      cureSick->sickCode = static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_WAZALOCK);
      cureSick->poke_cnt = 1;
      cureSick->pokeID[0] = pokeID;
      cureSick->fStdMsgDisable = true;
    flowWk->HANDEX_Pop( cureSick );

    BTL_HANDEX_PARAM_RESET_CONTFLAG* resetContFlag = reinterpret_cast<BTL_HANDEX_PARAM_RESET_CONTFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_RESET_CONTFLAG, pokeID ) );
      resetContFlag->pokeID = pokeID;
      resetContFlag->flag   = BTL_POKEPARAM::CONTFLG_GAMAN;
    flowWk->HANDEX_Pop( resetContFlag );

    work[ WORKIDX_STICK ] = 0;
  }
}

//----------------------------------------------------------------------------------
/**
 * リサイクル
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Recycle( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Recycle },          // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Recycle( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->GetItem() == ITEM_DUMMY_DATA )
    {
      u16 itemID = bpp->GetConsumedItem();

      if( itemID != ITEM_DUMMY_DATA )
      {
        BTL_HANDEX_PARAM_SET_ITEM* param = reinterpret_cast<BTL_HANDEX_PARAM_SET_ITEM *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_ITEM, pokeID ));

          param->itemID = itemID;
          param->pokeID = pokeID;
          param->fClearConsume = true;
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Recycle );
          HANDEX_STR_AddArg( &param->exStr, pokeID );
          HANDEX_STR_AddArg( &param->exStr, itemID );

        flowWk->HANDEX_Pop( param );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * サイコシフト
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_PsycoShift( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_PsycoShift },          // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_PsycoShift( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    WazaSick sick = static_cast<WazaSick>(bpp->GetPokeSick());
    // 自分がポケモン系の状態異常で、相手がそうでない場合のみ効果がある
    if( sick != pml::wazadata::WAZASICK_NULL )
    {
      u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
      const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( target_pokeID );
      if( target->GetPokeSick() == pml::wazadata::WAZASICK_NULL )
      {
        BTL_HANDEX_PARAM_ADD_SICK    *sick_param;
        BTL_HANDEX_PARAM_CURE_SICK   *cure_param;
        BTL_SICKCONT defaultCont = bpp->GetSickCont( sick );

        sick_param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
          sick_param->sickID = sick;
          if( (sick == pml::pokepara::SICK_DOKU) && SICKCONT_IsMoudokuCont(defaultCont) ){
            defaultCont = SICKCONT_MakeMoudokuCont( pokeID );
          }else{
            defaultCont = calc::MakeDefaultPokeSickCont( static_cast<PokeSick>(sick), pokeID );
          }
          sick_param->sickCont = defaultCont;
          sick_param->pokeID = target_pokeID;
          sick_param->fAlmost = true; // 何らかの要因で失敗したら原因表示する
        flowWk->HANDEX_Pop( sick_param );

        cure_param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
          cure_param->sickCode = static_cast<BtlWazaSickEx>(sick);
          cure_param->poke_cnt = 1;
          cure_param->pokeID[0] = pokeID;
          cure_param->header.failSkipFlag = true;
        flowWk->HANDEX_Pop( cure_param );

        EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, false );  // 標準失敗メッセージをオフ
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * いたみわけ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Itamiwake( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Itamiwake },          // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Itamiwake( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* bpp_me = flowWk->Hnd_GetPokeParam( pokeID );
    const BTL_POKEPARAM* bpp_target = flowWk->Hnd_GetPokeParam( target_pokeID );

    // 両者の平均値にHPを合わせる
    u32 hp_me     = bpp_me->GetValue( BTL_POKEPARAM::BPP_HP );
    u32 hp_target = bpp_target->GetValue( BTL_POKEPARAM::BPP_HP );
    u32 hp_avrg = (hp_me + hp_target) / 2;


    BTL_HANDEX_PARAM_SHIFT_HP* param;
    param = reinterpret_cast<BTL_HANDEX_PARAM_SHIFT_HP *>( flowWk->HANDEX_Push( BTL_HANDEX_SHIFT_HP, pokeID ));
      param->poke_cnt = 2;
      param->pokeID[0] = pokeID;
      param->volume[0] = hp_avrg - hp_me;
      param->pokeID[1] = target_pokeID;
      param->volume[1] = hp_avrg - hp_target;
      param->fItemReactionDisable = true;
      param->damageCause = DAMAGE_CAUSE_ITAMIWAKE;
    flowWk->HANDEX_Pop( param );


    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param;
      BTL_HANDEX_PARAM_CHECK_ITEM_EQUIP   *item_param;

      msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_STD, BTL_STRID_STD_Itamiwake );
      flowWk->HANDEX_Pop( msg_param );

      item_param = reinterpret_cast<BTL_HANDEX_PARAM_CHECK_ITEM_EQUIP *>( flowWk->HANDEX_Push( BTL_HANDEX_CHECK_ITEM_EQUIP, pokeID ));
        item_param->pokeID = pokeID;
        item_param->reactionType = BTL_ITEMREACTION_HP;
      flowWk->HANDEX_Pop( item_param );

      item_param = reinterpret_cast<BTL_HANDEX_PARAM_CHECK_ITEM_EQUIP *>( flowWk->HANDEX_Push( BTL_HANDEX_CHECK_ITEM_EQUIP, pokeID ));
        item_param->pokeID = target_pokeID;
        item_param->reactionType = BTL_ITEMREACTION_HP;
      flowWk->HANDEX_Pop( item_param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * はらだいこ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Haradaiko( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Haradaiko },          // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Haradaiko( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    int downHP = calc::QuotMaxHP( bpp, 2 );
    u32 upVolume = bpp->RankEffectUpLimit( BTL_POKEPARAM::BPP_ATTACK_RANK );
    if( (bpp->GetValue( BTL_POKEPARAM::BPP_HP) > downHP) && (upVolume != 0) )
    {
      BTL_HANDEX_PARAM_SHIFT_HP           *hp_param;
      BTL_HANDEX_PARAM_RANK_EFFECT        *rank_param;
      BTL_HANDEX_PARAM_MESSAGE            *msg_param;
      BTL_HANDEX_PARAM_CHECK_ITEM_EQUIP   *item_param;

      hp_param = reinterpret_cast<BTL_HANDEX_PARAM_SHIFT_HP *>( flowWk->HANDEX_Push( BTL_HANDEX_SHIFT_HP, pokeID ));
        hp_param->poke_cnt = 1;
        hp_param->fItemReactionDisable = true;
        hp_param->pokeID[0] = pokeID;
        hp_param->volume[0] = -downHP;
        hp_param->damageCause = DAMAGE_CAUSE_HARADAIKO;
      flowWk->HANDEX_Pop( hp_param );

      rank_param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
        rank_param->rankType       = pml::wazadata::RANKEFF_ATTACK;
        rank_param->rankVolume     = upVolume;
        rank_param->poke_cnt       = 1;
        rank_param->pokeID[0]      = pokeID;
        rank_param->fStdMsgDisable = true;
        rank_param->fAlmost        = true;
        rank_param->fWazaEffect    = true;
      flowWk->HANDEX_Pop( rank_param );

      msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Haradaiko );
        HANDEX_STR_AddArg( &msg_param->str, pokeID );
      flowWk->HANDEX_Pop( msg_param );

      item_param = reinterpret_cast<BTL_HANDEX_PARAM_CHECK_ITEM_EQUIP *>( flowWk->HANDEX_Push( BTL_HANDEX_CHECK_ITEM_EQUIP, pokeID ));
        item_param->pokeID = pokeID;
        item_param->reactionType = BTL_ITEMREACTION_HP;
      flowWk->HANDEX_Pop( item_param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * フェイント
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Feint( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_MAMORU_BREAK,                 handler_Feint_MamoruBreak     }, // まもる無効化チェック
    { BTL_EVENT_NOEFFECT_CHECK_SIDEEFF_GUARD_BEGIN, handler_Feint_NoEffCheckBegin }, // 無効化チェック開始
    { BTL_EVENT_NOEFFECT_CHECK_SIDEEFF_GUARD_END,   handler_Feint_NoEffCheckEnd   }, // 無効化チェック終了
    { BTL_EVENT_WAZA_DMG_DETERMINE,                 handler_Feint_AfterDamage     }, // ダメージ処理後
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// まもる無効化チェック
void handler_Feint_MamoruBreak( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID ){
    EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
  }
}

/**
 * @brief ワザ無効化チェック開始
 *
 * 技ターゲット陣営について、防御系サイドエフェクトを一時的に無効化する
 */
void handler_Feint_NoEffCheckBegin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  const u32 targetCount = EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
  for(u32 i=0; i<targetCount; ++i)
  {
    u8 targetPokeId = EVENTVAR_GetValue( static_cast<BtlEvVarLabel>( BTL_EVAR_POKEID_TARGET1 + i ) );
    SleepGuardSideEffect( flowWk, pokeID, targetPokeId, false );
  }
}

/**
 * @brief ワザ無効化チェック終了
 *
 * 無効化した防御系サイドエフェクトを有効化する
 */
void handler_Feint_NoEffCheckEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  const u32 targetCount = EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
  for(u32 i=0; i<targetCount; ++i)
  {
    u8 targetPokeId = EVENTVAR_GetValue( static_cast<BtlEvVarLabel>( BTL_EVAR_POKEID_TARGET1 + i ) );
    SleepGuardSideEffect( flowWk, pokeID, targetPokeId, true );
  }
}

/**
 * @brief 防御系サイドエフェクトを一時停止する
 * @param flowWk        使用するServerFlow
 * @param attackPokeId  防御系サイドエフェクトを一時停止させるポケモンのID
 * @param targetPokeId  防御系サイドエフェクトを一時停止されるポケモンのID
 * @param wakeFlag      true:一時停止を解除する, false:一時停止する
 */
void SleepGuardSideEffect( ServerFlow* flowWk, u8 attackPokeId, u8 targetPokeId, bool wakeFlag )
{
  BTL_HANDEX_PARAM_SIDEEFF_SLEEP* param = reinterpret_cast<BTL_HANDEX_PARAM_SIDEEFF_SLEEP *>( flowWk->HANDEX_Push( BTL_HANDEX_SIDEEFF_SLEEP, attackPokeId ));
    param->side  = flowWk->Hnd_GetMainModule()->PokeIDtoSide( targetPokeId );
    param->bWeak = wakeFlag;
    calc::BITFLG_Construction( param->flags, sizeof(param->flags) );
    for( uint32_t i=BTL_SIDEEFF_START; i<BTL_SIDEEFF_MAX; ++i )
    {
      if( tables::IsGuardTypeSideEffect( (BtlSideEffect)i ) ) 
      {
        calc::BITFLG_Set( param->flags, i );
      }
    }
  flowWk->HANDEX_Pop( param );
}

// ダメージ処理後（フェイント）
void handler_Feint_AfterDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  common_feint_proc( flowWk, pokeID, BTL_STRID_SET_Feint );
}

void common_feint_proc( ServerFlow* flowWk, u8 pokeID, u16 strID )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( target_pokeID );

    bool bExistGuardTypeSideEffect = common_IsExistGuardTypeSideEffect( flowWk, target_pokeID, true );

    if( (bpp->TURNFLAG_Get(BTL_POKEPARAM::TURNFLG_MAMORU))
    ||  (bExistGuardTypeSideEffect)
    ){
      common_mamoruBreakAfter( flowWk, pokeID, bpp, strID );
    }
  }
}
// まもる破壊ワザのダメージ後共通処理
void common_mamoruBreakAfter( ServerFlow* flowWk, u8 attackPokeID, const BTL_POKEPARAM* target, u16 strID )
{
  u8 target_pokeID = target->GetID();

  bool bExistCountupGuardTypeSideEffect = common_IsExistGuardTypeSideEffect( flowWk, target_pokeID, false );

  if( (target->TURNFLAG_Get(BTL_POKEPARAM::TURNFLG_MAMORU))
  ||  (bExistCountupGuardTypeSideEffect)
  ){
    BTL_HANDEX_PARAM_TURNFLAG* flg_param = reinterpret_cast<BTL_HANDEX_PARAM_TURNFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_RESET_TURNFLAG, attackPokeID ));
      flg_param->pokeID = target_pokeID;
      flg_param->flag = BTL_POKEPARAM::TURNFLG_MAMORU;
    flowWk->HANDEX_Pop( flg_param );

    BTL_HANDEX_PARAM_COUNTER*  counterParam = reinterpret_cast<BTL_HANDEX_PARAM_COUNTER *>( flowWk->HANDEX_Push( BTL_HANDEX_COUNTER, attackPokeID ));
      counterParam->pokeID = target_pokeID;
      counterParam->counterID = BTL_POKEPARAM::COUNTER_MAMORU;
      counterParam->value = 0;
    flowWk->HANDEX_Pop( counterParam );
  }

  {
    BTL_HANDEX_PARAM_MESSAGE*  msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, attackPokeID ));
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, strID );
      HANDEX_STR_AddArg( &msg_param->str, target_pokeID );
    flowWk->HANDEX_Pop( msg_param );
  }

  common_mamoruBreak_RemoveSideEff( flowWk, attackPokeID, target );
}
// まもる破壊ワザのダメージ後共通処理（サイドエフェクト除去）
void common_mamoruBreak_RemoveSideEff( ServerFlow* flowWk, u8 pokeID, const BTL_POKEPARAM* target )
{
  BtlSide side = flowWk->Hnd_GetMainModule()->PokeIDtoSide( target->GetID() );

  BTL_HANDEX_PARAM_SIDEEFF_REMOVE* param = reinterpret_cast<BTL_HANDEX_PARAM_SIDEEFF_REMOVE *>( flowWk->HANDEX_Push( BTL_HANDEX_SIDEEFF_REMOVE, pokeID ));
    param->side = side;
    calc::BITFLG_Construction( param->flags, sizeof(param->flags) );
    for (uint32_t i=BTL_SIDEEFF_START; i<BTL_SIDEEFF_MAX; ++i)
    {
      if( tables::IsGuardTypeSideEffect((BtlSideEffect)i) ) {
        calc::BITFLG_Set( param->flags, i );
      }
    }
  flowWk->HANDEX_Pop( param );
}
// 対象ポケモンの陣営にガードタイプのサイドエフェクトがかかっているか判定
bool common_IsExistGuardTypeSideEffect( ServerFlow* flowWk, u8 targetPokeID, bool bIncludeNotCountupType )
{
  BtlSide side = flowWk->Hnd_GetMainModule()->PokeIDtoSide( targetPokeID );

  for(u32 i=BTL_SIDEEFF_START; i<BTL_SIDEEFF_MAX; ++i)
  {
    BtlSideEffect  effect = (BtlSideEffect)i;
    if( flowWk->Hnd_IsExistSideEffect(side, effect) )
    {
      if( tables::IsGuardTypeSideEffect(effect) )
      {
        if( !bIncludeNotCountupType && (effect == BTL_SIDEEFF_TRICKGUARD) ){
          continue;
        }
        return true;
      }
    }
  }
  return false;
}

//----------------------------------------------------------------------------------
/**
 * いじげんホール
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_IjigenHall( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_MAMORU_BREAK,                 handler_Feint_MamoruBreak      }, // まもる無効化チェック
    { BTL_EVENT_NOEFFECT_CHECK_SIDEEFF_GUARD_BEGIN, handler_Feint_NoEffCheckBegin  }, // 無効化チェック開始
    { BTL_EVENT_NOEFFECT_CHECK_SIDEEFF_GUARD_END,   handler_Feint_NoEffCheckEnd    }, // 無効化チェック終了
    { BTL_EVENT_WAZA_DMG_DETERMINE,                 handler_IjigenHall_AfterDamage }, // ダメージ処理後
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージ処理後（いじげんホール）
void handler_IjigenHall_AfterDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  common_feint_proc( flowWk, pokeID, BTL_STRID_SET_MamoruBreak );
}

//----------------------------------------------------------------------------------
/**
 * つぼをつく
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_TuboWoTuku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_TuboWoTuku },          // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_TuboWoTuku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    static const BTL_POKEPARAM::ValueID rankType[] = {
     BTL_POKEPARAM::BPP_ATTACK_RANK,    BTL_POKEPARAM::BPP_DEFENCE_RANK,    BTL_POKEPARAM::BPP_AGILITY_RANK,
     BTL_POKEPARAM::BPP_SP_ATTACK_RANK, BTL_POKEPARAM::BPP_SP_DEFENCE_RANK, BTL_POKEPARAM::BPP_HIT_RATIO,
     BTL_POKEPARAM::BPP_AVOID_RATIO,
    };
    u8  target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );

    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( target_pokeID );
    u8 valid_cnt, i;

    for(i=0, valid_cnt=0; i<GFL_NELEMS(rankType); ++i){
      if( bpp->IsRankEffectValid(rankType[i], 2) ){
        ++valid_cnt;
      }
    }

    if( valid_cnt )
    {
      u8 idx = calc::GetRand( valid_cnt );
      for(i=0; i<GFL_NELEMS(rankType); ++i)
      {
        if( bpp->IsRankEffectValid(rankType[i], 2) )
        {
          if( idx == 0 )
          {
            BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
              param->pokeID[0] = target_pokeID;
              param->poke_cnt = 1;
              param->rankType = static_cast<WazaRankEffect>(rankType[i]);
              param->rankVolume = 2;
              param->fAlmost = false;
              param->fWazaEffect = true;
              param->bMigawariThrew = true;
            flowWk->HANDEX_Pop( param );
            break;
          }
          else
          {
            --idx;
          }
        }
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ねむる
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Nemuru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD,   handler_Nemuru_exeCheck },
    { BTL_EVENT_UNCATEGORIZE_WAZA,        handler_Nemuru },          // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Nemuru_exeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
   ServerFlow::SV_WazaFailCause  cause = static_cast<ServerFlow::SV_WazaFailCause>
                                            (EVENTVAR_GetValue( BTL_EVAR_FAIL_CAUSE ));
    if( cause == ServerFlow::SV_WAZAFAIL_NULL )
    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

      if( bpp->CheckNemuri( BTL_POKEPARAM::NEMURI_CHECK_INCLUDE_ZETTAINEMURI ) )
      {
        EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
      }
      else if( ( bpp->CheckSick( pml::wazadata::WAZASICK_KAIHUKUHUUJI) ) &&
               ( EVENTVAR_GetValue( BTL_EVAR_ZENRYOKU_WAZA_KIND ) != pml::waza::ZENRYOKUWAZA_KIND_BOOST ) )
      {
        EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_KAIHUKUHUUJI );
      }
      else if( bpp->IsHPFull() )
      {
        EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_HPFULL );
      }
      else {
        u16 tok = bpp->GetValue( BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE );
        if( (tok == TOKUSEI_HUMIN) || (tok == TOKUSEI_YARUKI) ){
          EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_FUMIN );
        }
      }
    }
  }
}
void handler_Nemuru( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( !bpp->IsHPFull() )
    {
      BTL_HANDEX_PARAM_RECOVER_HP        *hp_param;
      BTL_HANDEX_PARAM_ADD_SICK          *sick_param;
      BTL_HANDEX_PARAM_CHECK_ITEM_EQUIP  *item_param;

      sick_param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
        sick_param->pokeID = pokeID;
        sick_param->sickID = pml::wazadata::WAZASICK_NEMURI;
        sick_param->sickCont = SICKCONT_MakeTurn( pokeID, 3 );
        sick_param->fAlmost = true;
        sick_param->overWriteMode = BTL_SICK_OW_POKESICK;
        sick_param->fItemReactionDisable = true;
        HANDEX_STR_Setup( &sick_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Nemuru );
        HANDEX_STR_AddArg( &sick_param->exStr, pokeID );
      flowWk->HANDEX_Pop( sick_param );

      hp_param = reinterpret_cast<BTL_HANDEX_PARAM_RECOVER_HP *>( flowWk->HANDEX_Push( BTL_HANDEX_RECOVER_HP, pokeID ));
        hp_param->pokeID = pokeID;
        hp_param->recoverHP = (bpp->GetValue( BTL_POKEPARAM::BPP_MAX_HP) - bpp->GetValue( BTL_POKEPARAM::BPP_HP));
        hp_param->header.failSkipFlag = true;
        hp_param->fFailCheckThru = ( EVENTVAR_GetValue( BTL_EVAR_ZENRYOKU_WAZA_KIND ) == pml::waza::ZENRYOKUWAZA_KIND_BOOST ); //「Zねむる」なら「かいふくふうじ」を突破できる
      flowWk->HANDEX_Pop( hp_param );

      item_param = reinterpret_cast<BTL_HANDEX_PARAM_CHECK_ITEM_EQUIP *>( flowWk->HANDEX_Push( BTL_HANDEX_CHECK_ITEM_EQUIP, pokeID ));
        item_param->pokeID = pokeID;
        item_param->reactionType = BTL_ITEMREACTION_SICK;
        item_param->header.failSkipFlag = true;
      flowWk->HANDEX_Pop( item_param );

      EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, false );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * メロメロ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Meromero( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2,   handler_Meromero_CheckNoEffect },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Meromero_CheckNoEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* self = flowWk->Hnd_GetPokeParam( pokeID );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );

    u8 my_sex = self->GetValue( BTL_POKEPARAM::BPP_SEX );
    u8 target_sex = target->GetValue( BTL_POKEPARAM::BPP_SEX );

    if( (my_sex == pml::SEX_UNKNOWN)
    ||  (target_sex == pml::SEX_UNKNOWN)
    ||  ( my_sex == target_sex )
    ){
      EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, true );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * テクスチャー２
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Texture2( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Texture2 },          // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Texture2( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_TARGET1) );
    pml::PokeType  type = bpp->GetPrevWazaType( );
    if( type != POKETYPE_NULL )
    {
      // 抵抗相性を列挙
      pml::PokeType *typeArray = reinterpret_cast<pml::PokeType*>(flowWk->Hnd_GetTmpWork( sizeof(pml::PokeType)* POKETYPE_NUMS ));
      u32 cnt = calc::GetResistTypes( type, typeArray );
      if( cnt )
      {
        const BTL_POKEPARAM* bppSelf = flowWk->Hnd_GetPokeParam( pokeID );

        // 自分とマッチするタイプは除外
        {
          u32 i = 0;
          while( i<cnt )
          {
            if( bppSelf->IsMatchType(typeArray[i]) )
            {
              int move_len = (cnt - 1) - i;

              if( move_len > 0 ){
                gfl2::std::MemCopy( &typeArray[i+1], &typeArray[i], move_len * sizeof(pml::PokeType) );
              }
              --cnt;
            }
            else{
              ++i;
            }
          }
        }

        if( cnt )
        {
          BTL_HANDEX_PARAM_CHANGE_TYPE* param;
          u32 idx = calc::GetRand( cnt );
          pml::PokeType next_type = typeArray[ idx ];

          param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_TYPE *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_TYPE, pokeID ));
            param->next_type = PokeTypePair_MakePure( next_type );
            param->exTypeCause = BTL_POKEPARAM::EXTYPE_CAUSE_NONE;
            param->pokeID = pokeID;
          flowWk->HANDEX_Pop( param );
        }
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * アンコール
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Encore( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,   handler_Encore            }, // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Encore( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetPokeID );
    WazaID  prevWaza;

    prevWaza = target->GetPrevOrgWazaID();

//    TAYA_Printf("アンコール対象ポケ=%d, prevWaza=%d\n", targetPokeID, prevWaza);

    if( (!target->CheckSick( pml::wazadata::WAZASICK_ENCORE))
    &&  (!tables::IsMatchEncoreFail(prevWaza))
    &&  (target->WAZA_GetPP_ByNumber(prevWaza) != 0)
    &&  (target->IsPrevWazaZenryoku() == false)
    ){
      BTL_HANDEX_PARAM_ADD_SICK* param;
      u8 turns = 3;

      // 現ターンに行動済みのポケモンにはターン数+1する
      if( target->TURNFLAG_Get(BTL_POKEPARAM::TURNFLG_ACTION_DONE) ){
        ++turns;
      }

      param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
        param->sickID = pml::wazadata::WAZASICK_ENCORE;
        param->sickCont = SICKCONT_MakeTurnParam( pokeID, turns, prevWaza );
        param->pokeID = targetPokeID;
        param->fAlmost = false;

        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Encore );
        HANDEX_STR_AddArg( &param->exStr, targetPokeID );
      flowWk->HANDEX_Pop( param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ちょうはつ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Chouhatu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Chouhatu },          // 未分類ワザハンドラ
  };

  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Chouhatu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetPokeID );

    if( !target->CheckSick( pml::wazadata::WAZASICK_TYOUHATSU) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param ;

     // 現ターンに行動済みのポケモンには効果ターン数を+1する
      u8 turns = BTL_TYOUHATSU_EFFECTIVE_TURN;
      if( target->TURNFLAG_Get(BTL_POKEPARAM::TURNFLG_ACTION_DONE) ){
        ++turns;
      }

      param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
        param->sickID = pml::wazadata::WAZASICK_TYOUHATSU;
        param->sickCont = SICKCONT_MakeTurn( pokeID, turns );
        param->pokeID = targetPokeID;
      flowWk->HANDEX_Pop( param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * いちゃもん
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Ichamon( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Ichamon                 },   // 未分類ワザハンドラ
//    { BTL_EVENT_MIGAWARI_EXCLUDE,   handler_Common_MigawariEffctive },   // みがわりチェック

  };

  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Ichamon( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetPokeID );

    if( !target->CheckSick( pml::wazadata::WAZASICK_ICHAMON) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
        param->sickID = pml::wazadata::WAZASICK_ICHAMON;
        param->sickCont = SICKCONT_MakePermanent( pokeID );
        param->pokeID = targetPokeID;
      flowWk->HANDEX_Pop( param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * かなしばり
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Kanasibari( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Kanasibari },          // 未分類ワザハンドラ
  };

  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Kanasibari( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetPokeID );

    if( !target->CheckSick( pml::wazadata::WAZASICK_KANASIBARI) )
    {
      WazaID  prevWaza = target->GetPrevOrgWazaID();

      if( ((prevWaza != WAZANO_NULL) && (prevWaza != WAZANO_WARUAGAKI) )
      &&  (target->WAZA_IsUsable(prevWaza))
      &&  (target->IsPrevWazaZenryoku() == false)
      ){
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
        flowWk->HANDEX_Pop( param );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ふういん
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Fuuin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Fuuin },          // 未分類ワザハンドラ
  };

  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Fuuin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    {
      BTL_HANDEX_PARAM_ADD_FLDEFF* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_FLDEFF *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_FLDEFF, pokeID ));
        param->effect = FieldStatus::EFF_FUIN;
        param->cont = SICKCONT_MakePoke( pokeID, pokeID );
        param->fAddDependPoke = true;
        param->exStr.type = BTL_STRTYPE_SET;
        param->exStr.ID = BTL_STRID_SET_Fuuin;
        param->exStr.argCnt = 1;
        param->exStr.args[0] = pokeID;
      flowWk->HANDEX_Pop( param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * アロマセラピー
 *
 * 控えポケモンも含め、味方すべての基本状態異常を回復する
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Alomatherapy( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXE_DECIDE,     handler_Alomatherapy_Exe   },  // ワザ出し確定
    { BTL_EVENT_CHECK_REMOVEALL_CANCEL,handler_IyasiNoSuzu_RemoveAllTarget },  // ターゲットいなくても実行
    { BTL_EVENT_SKIP_AVOID_CHECK,    handler_Tedasuke_SkipAvoid },  // 命中チェックスキップハンドラ
    { BTL_EVENT_CHECK_POKE_HIDE ,    handler_Tedasuke_CheckHide },  // 消えてるポケ回避チェック
    { BTL_EVENT_UNCATEGORIZE_WAZA,   handler_Alomatherapy       },  // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザ出し確定
void handler_Alomatherapy_Exe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  // メッセージ出力
  BTL_HANDEX_PARAM_MESSAGE* msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_STD, BTL_STRID_STD_AlomaTherapy );
  flowWk->HANDEX_Pop( msg_param );
}
// 効果
void handler_Alomatherapy( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_CureFriendPokeSick( myHandle, flowWk, pokeID, work );
}
//----------------------------------------------------------------------------------
/**
 * いやしのすず
 *
 * 控えポケモンも含め、味方すべての基本状態異常を回復する
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_IyasiNoSuzu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXE_DECIDE,       handler_IyasiNoSuzu_Exe    },  // ワザ出し確定
    { BTL_EVENT_CHECK_REMOVEALL_CANCEL,handler_IyasiNoSuzu_RemoveAllTarget },  // ターゲットいなくても実行
    { BTL_EVENT_SKIP_AVOID_CHECK,      handler_Tedasuke_SkipAvoid },  // 命中チェックスキップハンドラ
    { BTL_EVENT_CHECK_POKE_HIDE ,      handler_Tedasuke_CheckHide },  // 消えてるポケ回避チェック
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_IyasiNoSuzu        },  // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザ出し確定
void handler_IyasiNoSuzu_Exe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  // メッセージ出力
  BTL_HANDEX_PARAM_MESSAGE* msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_STD, BTL_STRID_STD_IyasinoSuzu );
  flowWk->HANDEX_Pop( msg_param );
}
// ターゲットいなくても実行チェック
void handler_IyasiNoSuzu_RemoveAllTarget( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
  }
}
// 効果
void handler_IyasiNoSuzu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_CureFriendPokeSick( myHandle, flowWk, pokeID, work );
}

void common_CureFriendPokeSick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  enum {
    NUM_TARGET_MAX = BTL_PARTY_MEMBER_MAX * 2,
  };

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 自分と味方のパーティ全体が対象
    const BTL_PARTY* party[2];
    party[0] = flowWk->Hnd_GetPartyData( pokeID );
    party[1] = flowWk->Hnd_GetFriendPartyData( pokeID );
    u32 partyCount = (party[1]==NULL)? 1 : 2;

    // 対象ポケの中から状態異常ポケをピックアップするための配列を用意
    u8*  pokeIDAry = reinterpret_cast<u8*>( work );
    u32  sickPokeCnt = 0;

    for(u32 p=0; p<partyCount; ++p)
    {
      u32  memberCount = party[p]->GetMemberCount();
      for(u32 i=0; i<memberCount; ++i)
      {
        const BTL_POKEPARAM* bpp = party[p]->GetMemberDataConst( i );
        u8 targetPokeID = bpp->GetID();
        // 場に出ている場合、ターゲットに含まれていないならここで除外（ぼうおんで防がれるケース）
        if( flowWk->Hnd_GetExistFrontPokePos(targetPokeID) != BTL_POS_NULL)
        {
          if( !HandCommon_CheckTargetPokeID(targetPokeID) ){
            continue;
          }
        }

        if( HandCommon_CheckPokeSick(flowWk, targetPokeID) != pml::pokepara::SICK_NULL ){
          pokeIDAry[sickPokeCnt++] = targetPokeID;
        }
      }
    }

    // 状態異常ポケが0なら何もしない
    if( sickPokeCnt == 0 ){
      EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
      return;
    }

    // 状態異常回復
    BTL_HANDEX_PARAM_CURE_SICK *cure_param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
      cure_param->poke_cnt = sickPokeCnt;
      for(u32 i=0; i<sickPokeCnt; ++i){
        cure_param->pokeID[ i ] = pokeIDAry[ i ];
      }
      cure_param->sickCode = static_cast<BtlWazaSickEx>(WAZASICK_EX_POKEFULL);
    flowWk->HANDEX_Pop( cure_param );
  }
}

//----------------------------------------------------------------------------------
/**
 * おきみやげ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Okimiyage( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Okimiyage   },  // ワザ威力決定
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Okimiyage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_KILL* kill_param;
    BTL_HANDEX_PARAM_RANK_EFFECT* rank_param;
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    u32 serial = flowWk->Hnd_GetRankEffSerial( );

    rank_param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
      rank_param->poke_cnt   = 1;
      rank_param->pokeID[0]  = targetPokeID;
      rank_param->rankType   = pml::wazadata::RANKEFF_ATTACK;
      rank_param->rankVolume = -2;
      rank_param->effSerial  = serial;
      rank_param->fAlmost    = true;
      rank_param->fWazaEffect = true;
      rank_param->bMigawariThrew = true;
    flowWk->HANDEX_Pop( rank_param );


    rank_param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
      rank_param->poke_cnt    = 1;
      rank_param->pokeID[0]   = targetPokeID;
      rank_param->rankType    = pml::wazadata::RANKEFF_SP_ATTACK;
      rank_param->rankVolume  = -2;
      rank_param->effSerial   = serial;
      rank_param->fAlmost     = true;
      rank_param->fWazaEffect = true;
      rank_param->bMigawariThrew = true;
    flowWk->HANDEX_Pop( rank_param );

    kill_param = reinterpret_cast<BTL_HANDEX_PARAM_KILL *>( flowWk->HANDEX_Push( BTL_HANDEX_KILL, pokeID ));
      kill_param->pokeID = pokeID;
      kill_param->deadCause = DAMAGE_CAUSE_OKIMIYAGE;
    flowWk->HANDEX_Pop( kill_param );
  }
}
//----------------------------------------------------------------------------------
/**
 * うらみ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Urami( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Urami   },  // ワザ威力決定
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Urami( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  enum {
    URAMI_DECREMENT_PP_VOLUME = 4,
  };

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( targetPokeID );
    if( !( bpp->IsPrevWazaZenryoku() ) )
    {
      WazaID prev_waza = bpp->GetPrevOrgWazaID();
      u8 wazaIdx = bpp->WAZA_SearchIdx( prev_waza );

      if( wazaIdx != PTL_WAZA_MAX )
      {
        u8 volume = static_cast<u8>(bpp->WAZA_GetPP( wazaIdx ));
        if( volume > URAMI_DECREMENT_PP_VOLUME ){ volume = URAMI_DECREMENT_PP_VOLUME; }
        if( volume )
        {
          BTL_HANDEX_PARAM_PP*      decpp_param;

          decpp_param = reinterpret_cast<BTL_HANDEX_PARAM_PP *>( flowWk->HANDEX_Push( BTL_HANDEX_DECREMENT_PP, pokeID ));
            decpp_param->pokeID = targetPokeID;
            decpp_param->volume = volume;
            decpp_param->wazaIdx = wazaIdx;
            HANDEX_STR_Setup( &decpp_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Urami );
            HANDEX_STR_AddArg( &decpp_param->exStr, targetPokeID );
            HANDEX_STR_AddArg( &decpp_param->exStr, prev_waza );
            HANDEX_STR_AddArg( &decpp_param->exStr, volume );
          flowWk->HANDEX_Pop( decpp_param );
        }
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * じこあんじ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_JikoAnji( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_JikoAnji   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_JikoAnji( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( target_pokeID );

    BTL_HANDEX_PARAM_SET_RANK* rank_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    rank_param = reinterpret_cast<BTL_HANDEX_PARAM_SET_RANK *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_RANK, pokeID ));
      rank_param->pokeID        = pokeID;
      rank_param->attack        = target->GetValue( BTL_POKEPARAM::BPP_ATTACK_RANK );
      rank_param->defence       = target->GetValue( BTL_POKEPARAM::BPP_DEFENCE_RANK );
      rank_param->sp_attack     = target->GetValue( BTL_POKEPARAM::BPP_SP_ATTACK_RANK );
      rank_param->sp_defence    = target->GetValue( BTL_POKEPARAM::BPP_SP_DEFENCE_RANK );
      rank_param->agility       = target->GetValue( BTL_POKEPARAM::BPP_AGILITY_RANK );
      rank_param->hit_ratio     = target->GetValue( BTL_POKEPARAM::BPP_HIT_RATIO );
      rank_param->avoid_ratio   = target->GetValue( BTL_POKEPARAM::BPP_AVOID_RATIO );
      rank_param->critical_rank = target->GetCriticalRankPure();
    flowWk->HANDEX_Pop( rank_param );

    // xy よりクリティカル率もコピーするので、気合いだめフラグも同時にしないと重ねがけできてしまう。
    if( target->CONTFLAG_Get(BTL_POKEPARAM::CONTFLG_KIAIDAME) )
    {
      BTL_HANDEX_PARAM_SET_CONTFLAG* flag_param = reinterpret_cast<BTL_HANDEX_PARAM_SET_CONTFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_CONTFLAG, pokeID ));
      flag_param->pokeID = pokeID;
      flag_param->flag = BTL_POKEPARAM::CONTFLG_KIAIDAME;
      flowWk->HANDEX_Pop( flag_param );
    }
    else
    {
      BTL_HANDEX_PARAM_RESET_CONTFLAG * flag_param = reinterpret_cast<BTL_HANDEX_PARAM_RESET_CONTFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_RESET_CONTFLAG, pokeID ));
      flag_param->pokeID = pokeID;
      flag_param->flag = BTL_POKEPARAM::CONTFLG_KIAIDAME;
      flowWk->HANDEX_Pop( flag_param );
    }

    // とぎすます状態をコピー
    if( target->CheckSick( pml::wazadata::WAZASICK_TOGISUMASU ) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* sick_param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ) );
        sick_param->sickID   = pml::wazadata::WAZASICK_TOGISUMASU;
        sick_param->sickCont = target->GetSickCont( pml::wazadata::WAZASICK_TOGISUMASU );
        sick_param->pokeID   = pokeID;
      flowWk->HANDEX_Pop( sick_param );
    }

    msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_JikoAnji );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
      HANDEX_STR_AddArg( &msg_param->str, target_pokeID );
    flowWk->HANDEX_Pop( msg_param );
  }
}
//----------------------------------------------------------------------------------
/**
 * ハートスワップ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_HeartSwap( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_HeartSwap   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_HeartSwap( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( target_pokeID );
    const BTL_POKEPARAM* self = flowWk->Hnd_GetPokeParam( pokeID );

    BTL_HANDEX_PARAM_SET_RANK* rank_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    int* myRankParam = (int*)flowWk->Hnd_GetTmpWork( sizeof(int) * BTL_POKEPARAM::BPP_RANKVALUE_RANGE );
    int  myCriticalRank = self->GetCriticalRankPure();
    int r;

    // 自分を書き換えちゃう前にランクを保存しておく
    for(r=BTL_POKEPARAM::BPP_RANKVALUE_START; r<=BTL_POKEPARAM::BPP_RANKVALUE_END; ++r)
    {
      myRankParam[ r - BTL_POKEPARAM::BPP_RANKVALUE_START ] = self->GetValue( (BTL_POKEPARAM::ValueID)(r) );
    }

    rank_param = reinterpret_cast<BTL_HANDEX_PARAM_SET_RANK *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_RANK, pokeID ));
      rank_param->pokeID        = pokeID;
      rank_param->attack        = target->GetValue( BTL_POKEPARAM::BPP_ATTACK_RANK );
      rank_param->defence       = target->GetValue( BTL_POKEPARAM::BPP_DEFENCE_RANK );
      rank_param->sp_attack     = target->GetValue( BTL_POKEPARAM::BPP_SP_ATTACK_RANK );
      rank_param->sp_defence    = target->GetValue( BTL_POKEPARAM::BPP_SP_DEFENCE_RANK );
      rank_param->agility       = target->GetValue( BTL_POKEPARAM::BPP_AGILITY_RANK );
      rank_param->hit_ratio     = target->GetValue( BTL_POKEPARAM::BPP_HIT_RATIO );
      rank_param->avoid_ratio   = target->GetValue( BTL_POKEPARAM::BPP_AVOID_RATIO );
      rank_param->critical_rank = myCriticalRank;
    flowWk->HANDEX_Pop( rank_param );

    rank_param = reinterpret_cast<BTL_HANDEX_PARAM_SET_RANK *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_RANK, pokeID ));
      rank_param->pokeID      = target_pokeID;
      rank_param->attack      = myRankParam[BTL_POKEPARAM::BPP_ATTACK_RANK - BTL_POKEPARAM::BPP_RANKVALUE_START ];
      rank_param->defence     = myRankParam[BTL_POKEPARAM::BPP_DEFENCE_RANK - BTL_POKEPARAM::BPP_RANKVALUE_START ];
      rank_param->sp_attack   = myRankParam[BTL_POKEPARAM::BPP_SP_ATTACK_RANK - BTL_POKEPARAM::BPP_RANKVALUE_START ];
      rank_param->sp_defence  = myRankParam[BTL_POKEPARAM::BPP_SP_DEFENCE_RANK - BTL_POKEPARAM::BPP_RANKVALUE_START ];
      rank_param->agility     = myRankParam[BTL_POKEPARAM::BPP_AGILITY_RANK - BTL_POKEPARAM::BPP_RANKVALUE_START ];
      rank_param->hit_ratio   = myRankParam[BTL_POKEPARAM::BPP_HIT_RATIO - BTL_POKEPARAM::BPP_RANKVALUE_START ];
      rank_param->avoid_ratio = myRankParam[BTL_POKEPARAM::BPP_AVOID_RATIO - BTL_POKEPARAM::BPP_RANKVALUE_START ];
      rank_param->critical_rank = target->GetCriticalRankPure();
    flowWk->HANDEX_Pop( rank_param );

    msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_HeartSwap );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    flowWk->HANDEX_Pop( msg_param );
  }
}
//----------------------------------------------------------------------------------
/**
 * パワースワップ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_PowerSwap( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_PowerSwap   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_PowerSwap( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( target_pokeID );
    const BTL_POKEPARAM* self = flowWk->Hnd_GetPokeParam( pokeID );

    BTL_HANDEX_PARAM_SET_RANK* rank_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    // 自分のを書き換える前に保存
    int myAtk = self->GetValue( BTL_POKEPARAM::BPP_ATTACK_RANK );
    int mySPAtk = self->GetValue( BTL_POKEPARAM::BPP_SP_ATTACK_RANK );

    rank_param = reinterpret_cast<BTL_HANDEX_PARAM_SET_RANK *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_RANK, pokeID ));
      rank_param->pokeID        = pokeID;
      rank_param->attack        = target->GetValue( BTL_POKEPARAM::BPP_ATTACK_RANK );
      rank_param->sp_attack     = target->GetValue( BTL_POKEPARAM::BPP_SP_ATTACK_RANK );
      rank_param->defence       = self->GetValue( BTL_POKEPARAM::BPP_DEFENCE_RANK );
      rank_param->sp_defence    = self->GetValue( BTL_POKEPARAM::BPP_SP_DEFENCE_RANK );
      rank_param->agility       = self->GetValue( BTL_POKEPARAM::BPP_AGILITY_RANK );
      rank_param->hit_ratio     = self->GetValue( BTL_POKEPARAM::BPP_HIT_RATIO );
      rank_param->avoid_ratio   = self->GetValue( BTL_POKEPARAM::BPP_AVOID_RATIO );
      rank_param->critical_rank = self->GetCriticalRankPure();
    flowWk->HANDEX_Pop( rank_param );

    rank_param = reinterpret_cast<BTL_HANDEX_PARAM_SET_RANK *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_RANK, pokeID ));
      rank_param->pokeID        = target_pokeID;
      rank_param->attack        = myAtk;
      rank_param->sp_attack     = mySPAtk;
      rank_param->defence       = target->GetValue( BTL_POKEPARAM::BPP_DEFENCE_RANK );
      rank_param->sp_defence    = target->GetValue( BTL_POKEPARAM::BPP_SP_DEFENCE_RANK );
      rank_param->agility       = target->GetValue( BTL_POKEPARAM::BPP_AGILITY_RANK );
      rank_param->hit_ratio     = target->GetValue( BTL_POKEPARAM::BPP_HIT_RATIO );
      rank_param->avoid_ratio   = target->GetValue( BTL_POKEPARAM::BPP_AVOID_RATIO );
      rank_param->critical_rank = target->GetCriticalRankPure();
    flowWk->HANDEX_Pop( rank_param );

    msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_PowerSwap );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    flowWk->HANDEX_Pop( msg_param );
  }
}
//----------------------------------------------------------------------------------
/**
 * ガードスワップ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_GuardSwap( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_GuardSwap   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_GuardSwap( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( target_pokeID );
    const BTL_POKEPARAM* self = flowWk->Hnd_GetPokeParam( pokeID );

    BTL_HANDEX_PARAM_SET_RANK* rank_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    // 自分のを書き換える前に保存
    int myDef = self->GetValue( BTL_POKEPARAM::BPP_DEFENCE_RANK );
    int mySPDef = self->GetValue( BTL_POKEPARAM::BPP_SP_DEFENCE_RANK );

    rank_param = reinterpret_cast<BTL_HANDEX_PARAM_SET_RANK *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_RANK, pokeID ));
      rank_param->pokeID        = pokeID;
      rank_param->attack        = self->GetValue( BTL_POKEPARAM::BPP_ATTACK_RANK );
      rank_param->sp_attack     = self->GetValue( BTL_POKEPARAM::BPP_SP_ATTACK_RANK );
      rank_param->defence       = target->GetValue( BTL_POKEPARAM::BPP_DEFENCE_RANK );
      rank_param->sp_defence    = target->GetValue( BTL_POKEPARAM::BPP_SP_DEFENCE_RANK );
      rank_param->agility       = self->GetValue( BTL_POKEPARAM::BPP_AGILITY_RANK );
      rank_param->hit_ratio     = self->GetValue( BTL_POKEPARAM::BPP_HIT_RATIO );
      rank_param->avoid_ratio   = self->GetValue( BTL_POKEPARAM::BPP_AVOID_RATIO );
      rank_param->critical_rank = self->GetCriticalRankPure();
    flowWk->HANDEX_Pop( rank_param );

    rank_param = reinterpret_cast<BTL_HANDEX_PARAM_SET_RANK *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_RANK, pokeID ));
      rank_param->pokeID        = target_pokeID;
      rank_param->attack        = target->GetValue( BTL_POKEPARAM::BPP_ATTACK_RANK );
      rank_param->sp_attack     = target->GetValue( BTL_POKEPARAM::BPP_SP_ATTACK_RANK );
      rank_param->defence       = myDef;
      rank_param->sp_defence    = mySPDef;
      rank_param->agility       = target->GetValue( BTL_POKEPARAM::BPP_AGILITY_RANK );
      rank_param->hit_ratio     = target->GetValue( BTL_POKEPARAM::BPP_HIT_RATIO );
      rank_param->avoid_ratio   = target->GetValue( BTL_POKEPARAM::BPP_AVOID_RATIO );
      rank_param->critical_rank = target->GetCriticalRankPure();
    flowWk->HANDEX_Pop( rank_param );

    msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_GuardSwap );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    flowWk->HANDEX_Pop( msg_param );
  }
}
//----------------------------------------------------------------------------------
/**
 * パワートリック
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_PowerTrick( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_PowerTrick   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_PowerTrick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_STATUS* param;
    BTL_HANDEX_PARAM_SET_CONTFLAG*  flag_param;
    const BTL_POKEPARAM* bpp;

    bpp = flowWk->Hnd_GetPokeParam( pokeID );
    param = reinterpret_cast<BTL_HANDEX_PARAM_SET_STATUS *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_STATUS, pokeID ));

    param->pokeID  = pokeID;
      param->attack  = bpp->GetValue_Base(BTL_POKEPARAM::BPP_DEFENCE );
      param->defence = bpp->GetValue_Base(BTL_POKEPARAM::BPP_ATTACK );
      param->fAttackEnable  = true;
      param->fDefenceEnable = true;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_PowerTrick );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
    flowWk->HANDEX_Pop( param );

    flag_param = (BTL_HANDEX_PARAM_SET_CONTFLAG*)flowWk->HANDEX_Push( BTL_HANDEX_SET_CONTFLAG, pokeID );
      flag_param->pokeID = pokeID;
      flag_param->flag = BTL_POKEPARAM::CONTFLG_POWERTRICK;
    flowWk->HANDEX_Pop( flag_param );
  }
}
//----------------------------------------------------------------------------------
/**
 * パワーシェア
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_PowerShare( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_PowerShare   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_PowerShare( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_STATUS* param;
    const BTL_POKEPARAM *user, *target;
    u32 avrg, sp_avrg;
    u8 target_pokeID;


    target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    user = flowWk->Hnd_GetPokeParam( pokeID );
    target = flowWk->Hnd_GetPokeParam( target_pokeID );

    avrg = (user->GetValue_Base(BTL_POKEPARAM::BPP_ATTACK) + target->GetValue_Base(BTL_POKEPARAM::BPP_ATTACK)) / 2;
    sp_avrg = (user->GetValue_Base(BTL_POKEPARAM::BPP_SP_ATTACK) + target->GetValue_Base(BTL_POKEPARAM::BPP_SP_ATTACK)) / 2;

    param = reinterpret_cast<BTL_HANDEX_PARAM_SET_STATUS *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_STATUS, pokeID ));
      param->pokeID = target_pokeID;
      param->attack = avrg;
      param->sp_attack = sp_avrg;
      param->fAttackEnable = true;
      param->fSpAttackEnable = true;
    flowWk->HANDEX_Pop( param );

    param = reinterpret_cast<BTL_HANDEX_PARAM_SET_STATUS *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_STATUS, pokeID ));
      param->pokeID = pokeID;
      param->attack = avrg;
      param->sp_attack = sp_avrg;
      param->fAttackEnable = true;
      param->fSpAttackEnable = true;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_PowerShare );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
    flowWk->HANDEX_Pop( param );
  }
}
//----------------------------------------------------------------------------------
/**
 * ガードシェア
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_GuardShare( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_GuardShare   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_GuardShare( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_STATUS* param;
    const BTL_POKEPARAM *user, *target;
    u32 avrg, sp_avrg;
    u8 target_pokeID;

    target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    user = flowWk->Hnd_GetPokeParam( pokeID );
    target = flowWk->Hnd_GetPokeParam( target_pokeID );

    avrg = (user->GetValue_Base(BTL_POKEPARAM::BPP_DEFENCE) + target->GetValue_Base(BTL_POKEPARAM::BPP_DEFENCE)) / 2;
    sp_avrg = (user->GetValue_Base(BTL_POKEPARAM::BPP_SP_DEFENCE) + target->GetValue_Base(BTL_POKEPARAM::BPP_SP_DEFENCE)) / 2;

    param = reinterpret_cast<BTL_HANDEX_PARAM_SET_STATUS *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_STATUS, pokeID ));
      param->pokeID = target_pokeID;
      param->defence = avrg;
      param->sp_defence = sp_avrg;
      param->fDefenceEnable = true;
      param->fSpDefenceEnable = true;
    flowWk->HANDEX_Pop( param );

    param = reinterpret_cast<BTL_HANDEX_PARAM_SET_STATUS *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_STATUS, pokeID ));
      param->pokeID = pokeID;
      param->defence = avrg;
      param->sp_defence = sp_avrg;
      param->fDefenceEnable = true;
      param->fSpDefenceEnable = true;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_GuardShare );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
    flowWk->HANDEX_Pop( param );
  }
}
//----------------------------------------------------------------------------------
/**
 * ロックオン
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_LockON( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_LockON   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_LockON( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    BTL_HANDEX_PARAM_ADD_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
      param->pokeID = pokeID;
      param->sickID = pml::wazadata::WAZASICK_MUSTHIT_TARGET;
      param->sickCont = SICKCONT_MakePokeTurn( pokeID, targetPokeID, 2 );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_LockOn );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, targetPokeID );
    flowWk->HANDEX_Pop( param );
  }
}
//----------------------------------------------------------------------------------
/**
 * どくどく
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Dokudoku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXE_START,  handler_Dokudoku   },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Dokudoku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->IsMatchType(POKETYPE_DOKU) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
        param->pokeID = pokeID;
        param->sickID = pml::wazadata::WAZASICK_MUSTHIT;
        param->sickCont = SICKCONT_MakeTurn( pokeID, 1 );
      flowWk->HANDEX_Pop( param );
    }
  }
}


//----------------------------------------------------------------------------------
/**
 * リフレクター
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Refrector( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_Refrector   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Refrector( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  BTL_SICKCONT  cont = SICKCONT_MakeTurn( pokeID, 5 );
  BtlSide side = flowWk->Hnd_GetMainModule()->PokeIDtoSide( pokeID );
  common_SideEffectStdMsg( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_REFRECTOR, cont, BTL_STRID_STD_Reflector );
}
//----------------------------------------------------------------------------------
/**
 * ひかりのかべ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_HikariNoKabe( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_HikariNoKabe   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_HikariNoKabe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  BTL_SICKCONT  cont = SICKCONT_MakeTurn( pokeID, 5 );
  BtlSide side = flowWk->Hnd_GetMainModule()->PokeIDtoSide( pokeID );
  common_SideEffectStdMsg( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_HIKARINOKABE, cont, BTL_STRID_STD_HikariNoKabe );
}
//----------------------------------------------------------------------------------
/**
 * しんぴのまもり
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_SinpiNoMamori( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_SinpiNoMamori   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_SinpiNoMamori( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  BtlSide side = flowWk->Hnd_GetMainModule()->PokeIDtoSide( pokeID );

  BTL_SICKCONT  cont = SICKCONT_MakeTurn( pokeID, 5 );
  common_SideEffectStdMsg( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_SINPINOMAMORI, cont, BTL_STRID_STD_SinpiNoMamori );

}
//----------------------------------------------------------------------------------
/**
 * しろいきり
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_SiroiKiri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_SiroiKiri   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_SiroiKiri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  BTL_SICKCONT  cont = SICKCONT_MakeTurn( pokeID, 5 );
  BtlSide side = flowWk->Hnd_GetMainModule()->PokeIDtoSide( pokeID );
  common_SideEffectStdMsg( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_SIROIKIRI, cont, BTL_STRID_STD_SiroiKiri );
}
//----------------------------------------------------------------------------------
/**
 * おいかぜ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Oikaze( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_Oikaze   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Oikaze( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  BTL_SICKCONT  cont = SICKCONT_MakeTurn( pokeID, 4 );
  BtlSide side = flowWk->Hnd_GetMainModule()->PokeIDtoSide( pokeID );
  common_SideEffectStdMsg( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_OIKAZE, cont, BTL_STRID_STD_Oikaze );
}
//----------------------------------------------------------------------------------
/**
 * おまじない
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Omajinai( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_Omajinai   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Omajinai( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  BTL_SICKCONT  cont = SICKCONT_MakeTurn( pokeID, 5 );
  BtlSide side = flowWk->Hnd_GetMainModule()->PokeIDtoSide( pokeID );
  common_SideEffectStdMsg( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_OMAJINAI, cont, BTL_STRID_STD_Omajinai );
}
//----------------------------------------------------------------------------------
/**
 * まきびし
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Makibisi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_Makibisi   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Makibisi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  BTL_SICKCONT  cont = SICKCONT_MakePermanent( pokeID );
  BtlSide side = flowWk->Hnd_GetMainModule()->PokeIDtoOpponentSide( pokeID );
  common_SideEffectStdMsg( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_MAKIBISI, cont, BTL_STRID_STD_Makibisi );
}
//----------------------------------------------------------------------------------
/**
 * どくびし
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Dokubisi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_Dokubisi   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Dokubisi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  BTL_SICKCONT  cont = SICKCONT_MakePermanent( pokeID );
  BtlSide side = flowWk->Hnd_GetMainModule()->PokeIDtoOpponentSide( pokeID );
  common_SideEffectStdMsg( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_DOKUBISI, cont, BTL_STRID_STD_Dokubisi );
}
//----------------------------------------------------------------------------------
/**
 * ステルスロック
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_StealthRock( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_StealthRock   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_StealthRock( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  BTL_SICKCONT  cont = SICKCONT_MakePermanent( pokeID );
  BtlSide side = flowWk->Hnd_GetMainModule()->PokeIDtoOpponentSide( pokeID );
  common_SideEffectStdMsg( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_STEALTHROCK, cont, BTL_STRID_STD_StealthRock );
}
//----------------------------------------------------------------------------------
/**
 * ねばねばネット
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_NebaNebaNet( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_NebaNebaNet   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_NebaNebaNet( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  BTL_SICKCONT  cont = SICKCONT_MakePermanent( pokeID );
  BtlSide side = flowWk->Hnd_GetMainModule()->PokeIDtoOpponentSide( pokeID );
  common_SideEffectStdMsg( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_NEBANEBANET, cont, BTL_STRID_STD_NebaNebaNet );
}

//----------------------------------------------------------------------------------
/**
 * ワイドガード
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_WideGuard( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD,       handler_WideGuard_ExeCheck }, // ワザ出し成否チェックハンドラ
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_WideGuard          }, // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザ出し成否チェックハンドラ
void handler_WideGuard_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  // 現ターン最後の行動なら失敗
  if( HandCommon_IsPokeOrderLast(flowWk, pokeID) )
  {
    if( EVENTVAR_RewriteValue(BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER) )
    {
      BTL_HANDEX_PARAM_COUNTER*  counterParam = reinterpret_cast<BTL_HANDEX_PARAM_COUNTER *>( flowWk->HANDEX_Push( BTL_HANDEX_COUNTER, pokeID ));
        counterParam->pokeID = pokeID;
        counterParam->counterID = BTL_POKEPARAM::COUNTER_MAMORU;
        counterParam->value = 0;
      flowWk->HANDEX_Pop( counterParam );
    }
  }
}

void handler_WideGuard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  BTL_SICKCONT  cont = SICKCONT_MakeTurn( pokeID, 1 );
  BtlSide side = flowWk->Hnd_GetMainModule()->PokeIDtoSide( pokeID );
  common_SideEffectStdMsg( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_WIDEGUARD, cont, BTL_STRID_STD_WideGuard );

  IncrementMamoruCounter( flowWk, pokeID, true );
}

//----------------------------------------------------------------------------------
/**
 * たたみがえし
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_TatamiGaeshi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD,      handler_TatamiGaeshi_ExeCheck  }, // ワザ出し成否チェックハンドラ
    { BTL_EVENT_WAZASEQ_START,               handler_Mamoru_StartSeq        }, // ワザ処理開始
    { BTL_EVENT_WAZA_EXECUTE_FAIL,           handler_Mamoru_ExeFail         }, // ワザだし失敗確定
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET, handler_TatamiGaeshi           }, // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザ出し成否チェックハンドラ
void handler_TatamiGaeshi_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // ねこだましと同じ条件で失敗
    if(HandCommon_IsNekodamashiFail(flowWk, pokeID)){
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
    }

    // 現ターン最後の行動なら失敗
    if( HandCommon_IsPokeOrderLast(flowWk, pokeID) ){
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
    }
  }
}
// 未分類ワザ実行ハンドラ
void handler_TatamiGaeshi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  BTL_SICKCONT  cont = SICKCONT_MakeTurn( pokeID, 1 );
  BtlSide side = flowWk->Hnd_GetMainModule()->PokeIDtoSide( pokeID );
  common_SideEffectSetMsg( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_TATAMIGAESHI, cont, BTL_STRID_SET_TatamiGaeshi );
}

//-----------------------------------------------------------
/**
 *  サイドエフェクトワザ共通（標準メッセージフォーマット）
 */
//-----------------------------------------------------------
void common_SideEffectStdMsg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work,
  BtlSide side, BtlSideEffect effect, const BTL_SICKCONT& cont, u16 strID )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  static const bool isReplaceArgs0ByExpandSide = true;  // args[0] の陣営IDを展開後の陣営IDに置き換える
  common_SideEffectCore( flowWk, pokeID, side, effect, cont, BTL_STRTYPE_STD, strID, side, isReplaceArgs0ByExpandSide );
}
//-----------------------------------------------------------
/**
 *  サイドエフェクトワザ共通（セットメッセージフォーマット）
 */
//-----------------------------------------------------------
void common_SideEffectSetMsg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work,
  BtlSide side, BtlSideEffect effect, const BTL_SICKCONT& cont, u16 strID )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  static const bool isReplaceArgs0ByExpandSide = false;  // args[0] は陣営IDでないので置き換えない
  common_SideEffectCore( flowWk, pokeID, side, effect, cont, BTL_STRTYPE_SET, strID, pokeID, isReplaceArgs0ByExpandSide );
}
void common_SideEffectCore( ServerFlow* flowWk, u8 pokeID, BtlSide side, BtlSideEffect effect,
  const BTL_SICKCONT& cont, BtlStrType strType, u32 strID, int strArg, bool replaceStrArg0ByExpandSide )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SIDEEFF_ADD* param = reinterpret_cast<BTL_HANDEX_PARAM_SIDEEFF_ADD *>( flowWk->HANDEX_Push( BTL_HANDEX_SIDEEFF_ADD, pokeID ));
      param->effect = effect;
      param->side = side;
      param->cont = cont;
      param->fReplaceExStrArgs0ByExpandSide = replaceStrArg0ByExpandSide;
      HANDEX_STR_Setup( &param->exStr, strType, strID );
      HANDEX_STR_AddArg( &param->exStr, strArg );
    flowWk->HANDEX_Pop( param );
  }
}


//----------------------------------------------------------------------------------
/**
 * へんしん
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Hensin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,   handler_Hensin    }, // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Hensin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_HENSIN* param = reinterpret_cast<BTL_HANDEX_PARAM_HENSIN *>( flowWk->HANDEX_Push( BTL_HANDEX_HENSIN, pokeID ));
      param->pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Hensin );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, param->pokeID );
    flowWk->HANDEX_Pop( param );
  }
}
//----------------------------------------------------------------------------------
/**
 * みかづきのまい
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_MikadukiNoMai( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_MikadukiNoMai   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_MikadukiNoMai( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( flowWk->Hnd_IsExistBenchPoke( pokeID) )
    {
      BTL_HANDEX_PARAM_KILL* kill_param;
      BTL_HANDEX_PARAM_POSEFF_ADD* eff_param;

      eff_param = reinterpret_cast<BTL_HANDEX_PARAM_POSEFF_ADD *>( flowWk->HANDEX_Push( BTL_HANDEX_POSEFF_ADD, pokeID ));
        eff_param->effect = BTL_POSEFF_MIKADUKINOMAI;
        eff_param->pos = flowWk->Hnd_PokeIDtoPokePos( pokeID );
      flowWk->HANDEX_Pop( eff_param );

      kill_param = reinterpret_cast<BTL_HANDEX_PARAM_KILL *>( flowWk->HANDEX_Push( BTL_HANDEX_KILL, pokeID ));
        kill_param->pokeID = pokeID;
        kill_param->deadCause = DAMAGE_CAUSE_MIKAZUKINOMAI;
      flowWk->HANDEX_Pop( kill_param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * いやしのねがい
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_IyasiNoNegai( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_IyasiNoNegai   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_IyasiNoNegai( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( flowWk->Hnd_IsExistBenchPoke( pokeID) )
    {
      BTL_HANDEX_PARAM_KILL* kill_param;
      BTL_HANDEX_PARAM_POSEFF_ADD* eff_param;

      eff_param = reinterpret_cast<BTL_HANDEX_PARAM_POSEFF_ADD *>( flowWk->HANDEX_Push( BTL_HANDEX_POSEFF_ADD, pokeID ));
        eff_param->effect = BTL_POSEFF_IYASINONEGAI;
        eff_param->pos = flowWk->Hnd_PokeIDtoPokePos( pokeID );
      flowWk->HANDEX_Pop( eff_param );

      kill_param = reinterpret_cast<BTL_HANDEX_PARAM_KILL *>( flowWk->HANDEX_Push( BTL_HANDEX_KILL, pokeID ));
        kill_param->pokeID = pokeID;
        kill_param->deadCause = DAMAGE_CAUSE_IYASINONEGAI;
      flowWk->HANDEX_Pop( kill_param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ねがいごと
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Negaigoto( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Negaigoto   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Negaigoto( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_POSEFF_ADD* eff_param;

    eff_param = reinterpret_cast<BTL_HANDEX_PARAM_POSEFF_ADD *>( flowWk->HANDEX_Push( BTL_HANDEX_POSEFF_ADD, pokeID ));
      eff_param->effect = BTL_POSEFF_NEGAIGOTO;
      eff_param->pos = flowWk->Hnd_PokeIDtoPokePos( pokeID );
      eff_param->fSkipHpRecoverSpFailCheck = ( EVENTVAR_GetValue( BTL_EVAR_ZENRYOKU_WAZA_KIND ) == pml::waza::ZENRYOKUWAZA_KIND_BOOST ); //「Zねがいごと」なら「かいふくふうじ」を突破できる
    flowWk->HANDEX_Pop( eff_param );
  }
}
//----------------------------------------------------------------------------------
/**
 * みらいよち
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Miraiyoti( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_DELAY_WAZA,  handler_Miraiyoti        },  // 遅延ワザセットチェック
    { BTL_EVENT_DECIDE_DELAY_WAZA, handler_Miraiyoti_Decide },  // 遅延ワザセット完了
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 遅延ワザセットチェック
void handler_Miraiyoti( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BtlPokePos  targetPos = EVENTVAR_GetValue( BTL_EVAR_POKEPOS );
    common_delayAttack( myHandle, flowWk, pokeID, targetPos );
  }
}
// 遅延ワザセット完了
void handler_Miraiyoti_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Miraiyoti );
      HANDEX_STR_AddArg( &param->str, pokeID );
    flowWk->HANDEX_Pop( param );
  }
}
/**
 *  遅延ワザセットチェック共通処理
 */
void common_delayAttack( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, u8 targetPos )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( EVENTVAR_RewriteValue(BTL_EVAR_GEN_FLAG, true) )
    {
      BtlPokePos myPos = flowWk->Hnd_PokeIDtoPokePos( pokeID );
      BTL_HANDEX_PARAM_POSEFF_ADD* eff_param;

      if( targetPos == BTL_POS_NULL )
      {
        BtlRule  rule = flowWk->Hnd_GetRule( );
        targetPos = MainModule::GetOpponentPokePosByRule( rule, myPos, 0 );
        EVENTVAR_RewriteValue( BTL_EVAR_POKEPOS, targetPos );
        BTL_PRINT("  ターゲット不明瞭につき書き換え -> %d (myPos=%d, rule=%d)\n", targetPos, myPos, rule);
      }

      eff_param = reinterpret_cast<BTL_HANDEX_PARAM_POSEFF_ADD *>( flowWk->HANDEX_Push( BTL_HANDEX_POSEFF_ADD, pokeID ));
        eff_param->effect = BTL_POSEFF_DELAY_ATTACK;
        eff_param->pos    = targetPos;
        eff_param->effectParam.DelayAttack.execTurnMax   = 2;
        eff_param->effectParam.DelayAttack.execTurnCount = 0;
        eff_param->effectParam.DelayAttack.wazaNo        = EVENT_FACTOR_GetSubID( myHandle );

      flowWk->HANDEX_Pop( eff_param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * はめつのねがい
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_HametuNoNegai( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_DELAY_WAZA,  handler_HametuNoNegai        },  // 遅延ワザセットチェック
    { BTL_EVENT_DECIDE_DELAY_WAZA, handler_HametuNoNegai_Decide },  // 遅延ワザセット完了

  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 遅延ワザセットチェック
void handler_HametuNoNegai( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BtlPokePos  targetPos = EVENTVAR_GetValue( BTL_EVAR_POKEPOS );
    common_delayAttack( myHandle, flowWk, pokeID, targetPos );
  }
}
// 遅延ワザセット完了
void handler_HametuNoNegai_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_HametuNoNegai );
      HANDEX_STR_AddArg( &param->str, pokeID );
    flowWk->HANDEX_Pop( param );
  }
}

//----------------------------------------------------------------------------------
/**
 * いえき
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Ieki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Ieki   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Ieki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_SICK       *sick_param;
    u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );

    sick_param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
      sick_param->pokeID = target_pokeID;
      sick_param->sickID = pml::wazadata::WAZASICK_IEKI;
      sick_param->sickCont = SICKCONT_MakePermanent( pokeID );
      HANDEX_STR_Setup( &sick_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Ieki );
      HANDEX_STR_AddArg( &sick_param->exStr, target_pokeID );
    flowWk->HANDEX_Pop( sick_param );
  }
}
//----------------------------------------------------------------------------------
/**
 * なりきり
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Narikiri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Narikiri   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Narikiri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( target_pokeID );
    TokuseiNo  tok = static_cast<TokuseiNo>(target->GetValue( BTL_POKEPARAM::BPP_TOKUSEI ));

    if( !tables::CheckNarikiriFailTokusei(tok))
    {
      BTL_HANDEX_PARAM_CHANGE_TOKUSEI*  tok_param;

      tok_param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_TOKUSEI *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_TOKUSEI, pokeID ));
        tok_param->pokeID = pokeID;
        tok_param->tokuseiID = tok;
        HANDEX_STR_Setup( &tok_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Narikiri );
        HANDEX_STR_AddArg( &tok_param->exStr, pokeID );
        HANDEX_STR_AddArg( &tok_param->exStr, target_pokeID );
        HANDEX_STR_AddArg( &tok_param->exStr, tok_param->tokuseiID );
      flowWk->HANDEX_Pop( tok_param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * とんぼがえり・ボルトチェンジ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_TonboGaeri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
//    { BTL_EVENT_WAZADMG_SIDE_AFTER,  handler_TonboGaeri   },         // ダメージ直後
    { BTL_EVENT_DAMAGEPROC_END_HIT_L4,  handler_TonboGaeri   },         // ダメージ直後
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_TonboGaeri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( (flowWk->Hnd_IsExistBenchPoke(pokeID))
    &&  (!flowWk->Hnd_CheckReservedMemberChangeAction())
    ){
      u8 clientID = MainModule::PokeIDtoClientID( pokeID );

      BTL_HANDEX_PARAM_CHANGE_MEMBER* param;
      BTL_HANDEX_PARAM_ADD_EFFECT* eff_param;

      param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_MEMBER *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_MEMBER, pokeID ));
        param->pokeID = pokeID;
        HANDEX_STR_Setup( &param->preStr, BTL_STRTYPE_SET, BTL_STRID_SET_Tonbogaeri );
        HANDEX_STR_AddArg( &param->preStr, pokeID );
        HANDEX_STR_AddArg( &param->preStr, clientID );
      flowWk->HANDEX_Pop( param );

      eff_param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_EFFECT, pokeID ));
        eff_param->effectNo = (EVENT_FACTOR_GetSubID(myHandle) == WAZANO_TONBOGAERI)?
                      BTLEFF_TONBOGAERI_RETURN : BTLEFF_VOLTCHANGE_RETURN;
        eff_param->pos_from = flowWk->Hnd_PokeIDtoPokePos( pokeID );
        eff_param->pos_to = BTL_POS_NULL;
        eff_param->header.failSkipFlag = true;
      flowWk->HANDEX_Pop( eff_param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * こうそくスピン
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_KousokuSpin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZADMG_SIDE_AFTER,  handler_KousokuSpin   },         // ダメージ直後
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_KousokuSpin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    BTL_HANDEX_PARAM_CURE_SICK* cure_param;

    if( bpp->CheckSick( pml::wazadata::WAZASICK_YADORIGI) )
    {
      cure_param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
        cure_param->poke_cnt = 1;
        cure_param->pokeID[0] = pokeID;
        cure_param->sickCode = static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_YADORIGI);
      flowWk->HANDEX_Pop( cure_param );
    }
    if( bpp->CheckSick( pml::wazadata::WAZASICK_BIND) )
    {
      cure_param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
        cure_param->poke_cnt = 1;
        cure_param->pokeID[0] = pokeID;
        cure_param->sickCode = static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_BIND);
      flowWk->HANDEX_Pop( cure_param );
    }

    {
      BTL_HANDEX_PARAM_SIDEEFF_REMOVE* side_param = reinterpret_cast<BTL_HANDEX_PARAM_SIDEEFF_REMOVE *>( flowWk->HANDEX_Push( BTL_HANDEX_SIDEEFF_REMOVE, pokeID ));

      side_param->side = flowWk->Hnd_GetMainModule()->PokeIDtoSide( pokeID );
      calc::BITFLG_Construction( side_param->flags, sizeof(side_param->flags) );
      calc::BITFLG_Set( side_param->flags, BTL_SIDEEFF_MAKIBISI );
      calc::BITFLG_Set( side_param->flags, BTL_SIDEEFF_DOKUBISI );
      calc::BITFLG_Set( side_param->flags, BTL_SIDEEFF_STEALTHROCK );
      calc::BITFLG_Set( side_param->flags, BTL_SIDEEFF_NEBANEBANET );
      flowWk->HANDEX_Pop( side_param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * バトンタッチ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_BatonTouch( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_BatonTouch   },         // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 未分類ワザハンドラ
void handler_BatonTouch( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( (flowWk->Hnd_IsExistBenchPoke(pokeID))
    &&  (!flowWk->Hnd_CheckReservedMemberChangeAction())
    ){
      BTL_HANDEX_PARAM_POSEFF_ADD* eff_param;
      BTL_HANDEX_PARAM_CHANGE_MEMBER* change_param;
      BTL_HANDEX_PARAM_SET_CONTFLAG*  flag_param;

      eff_param = reinterpret_cast<BTL_HANDEX_PARAM_POSEFF_ADD *>( flowWk->HANDEX_Push( BTL_HANDEX_POSEFF_ADD, pokeID ));
        eff_param->effect = BTL_POSEFF_BATONTOUCH;
        eff_param->pos = flowWk->Hnd_PokeIDtoPokePos( pokeID );
      flowWk->HANDEX_Pop( eff_param );

      flag_param = reinterpret_cast<BTL_HANDEX_PARAM_SET_CONTFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_CONTFLAG, pokeID ));
        flag_param->pokeID = pokeID;
        flag_param->flag = BTL_POKEPARAM::CONTFLG_BATONTOUCH;
        flag_param->header.failSkipFlag = true;
      flowWk->HANDEX_Pop( flag_param );

      change_param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_MEMBER *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_MEMBER, pokeID ));
        change_param->pokeID = pokeID;
        change_param->fIntrDisable = true;
        change_param->header.failSkipFlag = true;
      flowWk->HANDEX_Pop( change_param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * テレポート
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Teleport( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Teleport   },         // 未分類ワザ
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD, handler_Teleport_ExeCheck },  // 実行チェック
    { BTL_EVENT_NIGERU_EXMSG,       handler_Teleport_ExMsg },     // 逃げるときの特殊メッセージ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Teleport_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  // ぬし戦、試練中は逃げられない
  if( flowWk->Hnd_GetStatusFlag( BTL_STATUS_FLAG_VS_NUSI ) ||
      flowWk->Hnd_GetStatusFlag( BTL_STATUS_FLAG_SIREN_BATTLE ) ||
      // @fix GFMMCat[209] VSヒカリドラゴン戦で、にげることができる
      // @fix GFMMCat[547] 合体ネクロズマ戦にて逃げられる
      flowWk->Hnd_GetStatusFlag( BTL_STATUS_FLAG_NO_CAPTURE ) )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
    return;
  }

  // 野生シングル戦または乱入戦でなければ失敗
  if( ( flowWk->Hnd_GetCompetitor() != BTL_COMPETITOR_WILD ) || 
      ( (flowWk->Hnd_GetRule() != BTL_RULE_SINGLE) && (flowWk->Hnd_GetRule() != BTL_RULE_INTRUDE) )
  ){
    EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
    return;
  }

  // 乱入戦で、味方が存在している状態なら失敗
  if( HandCommon_IsExistIntrudeFriend(flowWk, pokeID) ){
    EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
    return;
  }

  // かならず逃げられる状態なら以下のチェックは行わない
  if( !flowWk->Hnd_CheckForceNigeru(pokeID) )
  {
    // 巻き付かれていたら失敗
    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
      if( (bpp->CheckSick(pml::wazadata::WAZASICK_BIND))
      ||  (bpp->CheckSick(pml::wazadata::WAZASICK_TOOSENBOU))
      ||  (flowWk->Hnd_CheckFieldEffect(FieldStatus::EFF_FAIRY_LOCK))
      ){
        EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
      }
    }
  }
}
void handler_Teleport( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    EVENT_FACTOR_SetRecallEnable( myHandle );

    BTL_HANDEX_PARAM_QUIT_BATTLE* param = reinterpret_cast<BTL_HANDEX_PARAM_QUIT_BATTLE *>( flowWk->HANDEX_Push( BTL_HANDEX_QUIT_BATTLE, pokeID ));
    flowWk->HANDEX_Pop( param );
  }
}
void handler_Teleport_ExMsg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_RewriteValue(BTL_EVAR_GEN_FLAG, true) )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Teleport );
      HANDEX_STR_AddArg( &param->str, pokeID );
    flowWk->HANDEX_Pop( param );
  }
}
//----------------------------------------------------------------------------------
/**
 * なげつける
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Nagetukeru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD, handler_Nagetukeru_ExeCheck     },
    { BTL_EVENT_WAZA_POWER_BASE,        handler_Nagetukeru_WazaPower    },
    { BTL_EVENT_DAMAGEPROC_START,       handler_Nagetukeru_DmgProcStart },
    { BTL_EVENT_WAZA_DMG_REACTION,      handler_Nagetukeru_DmgAfter     },
    { BTL_EVENT_WAZA_EXECUTE_DONE,      handler_Nagetukeru_Done         },

  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

void handler_Nagetukeru_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u16 itemID = bpp->GetItem( );
    if( (itemID == ITEM_DUMMY_DATA)
    ||  (calc::ITEM_GetParam(itemID, ::item::ITEM_DATA::PRM_ID_NAGE_ATC) == 0 )
    ||  (!flowWk->Hnd_CheckItemUsable(pokeID))
    ||  (HandCommon_CheckCantChangeItemPoke(flowWk, pokeID))
    ){
      EVENTVAR_RewriteValue(BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER);
    }
  }
}
void handler_Nagetukeru_WazaPower( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u16 itemID = bpp->GetItem( );
    u32 pow = calc::ITEM_GetParam( itemID, ::item::ITEM_DATA::PRM_ID_NAGE_ATC );
    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
void handler_Nagetukeru_DmgProcStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u16 itemID = bpp->GetItem( );
    if( itemID != ITEM_DUMMY_DATA )
    {
      BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Nagetukeru );
        HANDEX_STR_AddArg( &param->str, pokeID );
        HANDEX_STR_AddArg( &param->str, itemID );
      flowWk->HANDEX_Pop( param );

      BTL_HANDEX_PARAM_TURNFLAG* flagParam = reinterpret_cast<BTL_HANDEX_PARAM_TURNFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_TURNFLAG, pokeID ));
        flagParam->pokeID = pokeID;
        flagParam->flag = BTL_POKEPARAM::TURNFLG_NAGETUKERU_USING;
      flowWk->HANDEX_Pop( flagParam );
    }
  }
}
void handler_Nagetukeru_DmgAfter( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  ){
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u16 itemID = bpp->GetItem( );
    if( itemID != ITEM_DUMMY_DATA )
    {
      BTL_HANDEX_PARAM_CONSUME_ITEM* item_param;
        item_param = reinterpret_cast<BTL_HANDEX_PARAM_CONSUME_ITEM *>( flowWk->HANDEX_Push( BTL_HANDEX_CONSUME_ITEM, pokeID ));
        item_param->fNoAction = true;
        item_param->fNoAteKinomi = true;
      flowWk->HANDEX_Pop( item_param );
      work[0] = 1;    // ダメージ与えて消費したフラグ

      // みがわりじゃなければ追加効果発動
      if( (EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == false)
      &&  (EVENTVAR_GetValue(BTL_EVAR_RINPUNGUARD_FLG) == false)
      ){
        int equip = calc::ITEM_GetParam( itemID, ::item::ITEM_DATA::PRM_ID_NAGE_EFF );
        if( equip )
        {
           BTL_HANDEX_PARAM_ITEM_SP*  param;
             param = reinterpret_cast<BTL_HANDEX_PARAM_ITEM_SP *>( flowWk->HANDEX_Push( BTL_HANDEX_ITEM_SP, pokeID ));
             param->pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
             param->itemID = itemID;
             param->bAteKinomi = true;
           flowWk->HANDEX_Pop( param );
        }
      }
    }
  }
}
void handler_Nagetukeru_Done( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( work[0] == 0 )
    {
      // 自分のアイテムを失わせる
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
      u16 itemID = bpp->GetItem( );
      if( itemID != ITEM_DUMMY_DATA )
      {
        BTL_HANDEX_PARAM_CONSUME_ITEM* item_param;
          item_param = reinterpret_cast<BTL_HANDEX_PARAM_CONSUME_ITEM *>( flowWk->HANDEX_Push( BTL_HANDEX_CONSUME_ITEM, pokeID ));
          item_param->fNoAction = true;
          item_param->fNoAteKinomi = true;
        flowWk->HANDEX_Pop( item_param );
      }
    }
  }
}



//----------------------------------------------------------------------------------
/**
 * でんじふゆう
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_DenjiFuyuu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD, handler_DenjiFuyuu_CheckFail  },  // ワザ出し成否チェックハンドラ
    { BTL_EVENT_UNCATEGORIZE_WAZA,      handler_DenjiFuyuu            },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザ出し成否チェックハンドラ
void handler_DenjiFuyuu_CheckFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

    // ねをはる状態・うちおとす状態・じゅうりょく場は失敗
    if( (bpp->CheckSick( pml::wazadata::WAZASICK_NEWOHARU))
    ||  (bpp->CheckSick( pml::wazadata::WAZASICK_FLYING_CANCEL))
    ){
      EVENTVAR_RewriteValue(BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER);
    }
    else if( flowWk->Hnd_CheckFieldEffect(FieldStatus::EFF_JURYOKU) )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_JURYOKU );
    }
  }
}

// 未分類ワザハンドラ
void handler_DenjiFuyuu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_SICK*  param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
      param->pokeID = pokeID;
      param->sickID = pml::wazadata::WAZASICK_FLYING;
      param->sickCont = SICKCONT_MakeTurn( pokeID, 5 );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_DenjiFuyu );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
    flowWk->HANDEX_Pop( param );
  }
}

//----------------------------------------------------------------------------------
/**
 * てだすけ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Tedasuke( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_SKIP_AVOID_CHECK,   handler_Tedasuke_SkipAvoid },  // 命中チェックスキップハンドラ
    { BTL_EVENT_CHECK_POKE_HIDE,    handler_Tedasuke_CheckHide },  // 消えてるポケ回避チェック
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Tedasuke_Ready     },  // 未分類ワザハンドラ
    { BTL_EVENT_WAZA_POWER,         handler_Tedasuke_WazaPow   },  // ワザ威力チェック
    { BTL_EVENT_TURNCHECK_BEGIN,    handler_Tedasuke_TurnCheck },  // ターンチェック開始ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 命中チェックスキップハンドラ
void handler_Tedasuke_SkipAvoid( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
  }
}
// 消えてるポケ回避チェック
void handler_Tedasuke_CheckHide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_AVOID_FLAG, false );
  }
}
// 未分類ワザハンドラ
void handler_Tedasuke_Ready( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID )
  {
    return;
  }

  if( tedasuke_IsSuccess( flowWk, pokeID ) )
  {
    u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( target_pokeID );

    if( !bpp->IsDead()
    &&  !bpp->TURNFLAG_Get(BTL_POKEPARAM::TURNFLG_ACTION_DONE)
    ){
      BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Tedasuke );
        HANDEX_STR_AddArg( &param->str, pokeID );
        HANDEX_STR_AddArg( &param->str, target_pokeID );
      flowWk->HANDEX_Pop( param );

      work[0] = target_pokeID;

      EVENT_FACTOR_ConvertForIsolate( myHandle );
    }
  }
}
/**
 * @brief 「てだすけ」が成功するか？
 * @param pokeID  てだすけを撃ったポケモンのID
 * @retval true   成功
 * @retval false  失敗
 */
bool tedasuke_IsSuccess( ServerFlow* flowWk, u8 pokeID )
{
  if( flowWk->Hnd_GetRule() == BTL_RULE_ROYAL )
  {
    return true;
  }

  if( flowWk->Hnd_GetFrontPosNum(MainModule::PokeIDtoClientID(pokeID)) > 1 )
  {
    return true;
  }

  return false;
}
// ワザ威力チェック
void handler_Tedasuke_WazaPow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(pokeID);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK)==work[0] )
  {
    EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.5f) );
  }
}
// ターンチェック開始ハンドラ
void handler_Tedasuke_TurnCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID ){
    EVENT_FACTOR_Remove( myHandle );  // ターンチェックで強制自殺
  }
}
//----------------------------------------------------------------------------------
/**
 * ふくろだたき
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_FukuroDataki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_COUNT,  handler_FukuroDataki      },  // ワザヒット回数計算ハンドラ
    { BTL_EVENT_WAZA_POWER_BASE, handler_FukuroDataki_Pow  },  // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザヒット回数計算ハンドラ
void handler_FukuroDataki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u32 cnt, i;

    for(i=cnt=0; i<BTL_PARTY_MEMBER_MAX; ++i)
    {
      if( common_FukuroDataki_GetParam(flowWk, pokeID, i) != NULL ){
        ++cnt;
      }
    }

    // 0ということは無いハズだが念のため
    if( cnt == 0 ){
      cnt = 1;
    }
    EVENTVAR_RewriteValue( BTL_EVAR_HITCOUNT, cnt );

    // 威力計算用Indexとしてクリアしておく
    work[0] = 0;
  }
}
// ワザ威力チェックハンドラ
void handler_FukuroDataki_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = common_FukuroDataki_GetParam( flowWk, pokeID, work[0] );

    // NULLということは無いハズだが念のため
    if( bpp == NULL ){
      bpp = flowWk->Hnd_GetPokeParam( pokeID );
    }
    ++(work[0]);
    // 有り得ないハズだがねんのため
    if( work[0] >= BTL_PARTY_MEMBER_MAX ){
      work[0] = 0;
    }

    {
      u32 pow = bpp->GetValue( BTL_POKEPARAM::BPP_MONS_POW );
      pow = 5 + (pow / 10);
      EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
    }
  }
}
/**
 *  回数増加＆威力計算に使えるメンバーのbppポインタ取得
 */
const BTL_POKEPARAM* common_FukuroDataki_GetParam( ServerFlow* flowWk, u8 myPokeID, u8 idx )
{
  const BTL_PARTY* party = flowWk->Hnd_GetPartyData( myPokeID );
  const BTL_POKEPARAM* bpp;

  u32 memberCnt, i;
  bool fEnable;

  memberCnt = party->GetMemberCount();
  if( idx >= memberCnt ){
    return NULL;
  }

  for(i=0; i<memberCnt; ++i)
  {
    fEnable = false;
    bpp = party->GetMemberDataConst( i );

    // 自分は無条件に有効
    if( bpp->GetID() == myPokeID )
    {
      fEnable = true;
    }
    else if(  (bpp->IsFightEnable())
    &&        (bpp->GetPokeSick() == pml::pokepara::SICK_NULL)
    ){
      fEnable = true;
    }

    if( fEnable )
    {
      if( idx == 0 ){
        return bpp;
      }
      --idx;
    }
  }
  return NULL;
}


//----------------------------------------------------------------------------------
/**
 * ねこだまし
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Nekodamasi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD,       handler_Nekodamasi },     // ワザ出し成否チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Nekodamasi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( HandCommon_IsNekodamashiFail(flowWk, pokeID)){
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * であいがしら
 *
 * 場に出た直後でないと出せない
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Deaigasira( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD,       handler_Deaigasira },     // ワザ出し成否チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Deaigasira( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( HandCommon_IsNekodamashiFail(flowWk, pokeID)){
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * あさのひざし・こうごうせい・つきのひかり
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_AsaNoHizasi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RECOVER_HP_RATIO,       handler_AsaNoHizasi },     // HP回復率計算ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_AsaNoHizasi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BtlWeather w = flowWk->Hnd_GetWeather( );
    fx32 ratio;
    switch( w ){
    case BTL_WEATHER_SHINE:
    case BTL_WEATHER_DAY:
      ratio = FX32_CONST( 0.667 );
      break;
    case BTL_WEATHER_RAIN:
    case BTL_WEATHER_STORM:
    case BTL_WEATHER_SAND:
    case BTL_WEATHER_SNOW:
      ratio = FX32_CONST( 0.25 );
      break;
    default:
      ratio = FX32_CONST( 0.5 );
    }

    EVENTVAR_RewriteValue( BTL_EVAR_RATIO, ratio );
  }
}
//----------------------------------------------------------------------------------
/**
 * すなあつめ
 *
 * HPを1/2回復。
 * 天候が「すなあらし」なら2/3回復。
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Sunaatume( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RECOVER_HP_RATIO,       handler_Sunaatume },     // HP回復率計算ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Sunaatume( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BtlWeather weather = flowWk->Hnd_GetWeather( );
    fx32 ratio = FX32_CONST( 0.5 );
    switch( weather )
    {
    case BTL_WEATHER_SAND:
      ratio = FX32_CONST( 0.667 );
      break;
    }

    EVENTVAR_RewriteValue( BTL_EVAR_RATIO, ratio );
  }
}

//----------------------------------------------------------------------------------
/**
 * フラワーヒール
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_FlowerHeal( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RECOVER_HP_RATIO,       handler_FlowerHeal },     // HP回復率計算ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

/**
 * @brief HP回復率計算ハンドラ
 *
 * 場が「グラスフィールド」なら2/3回復。
 */
void handler_FlowerHeal( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK ) != pokeID )
  {
    return;
  }

  BtlGround ground = flowWk->Hnd_GetGround();
  if( ground == BTL_GROUND_GRASS )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_RATIO, FX32_CONST( 0.667 ) );
  }
}

//----------------------------------------------------------------------------------
/**
 * そらをとぶ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_SoraWoTobu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_SoraWoTobu_TameStart },     // 溜め開始
    { BTL_EVENT_TAME_RELEASE,     handler_SoraWoTobu_TameRelease },   // 溜め解放
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_SoraWoTobu_TameStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = reinterpret_cast<BTL_HANDEX_PARAM_SET_CONTFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_CONTFLAG, pokeID ));
      param->pokeID = pokeID;
      param->flag = BTL_POKEPARAM::CONTFLG_SORAWOTOBU;
    flowWk->HANDEX_Pop( param );

    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_SoraWoTobu );
        HANDEX_STR_AddArg( &msg_param->str, pokeID );
      flowWk->HANDEX_Pop( msg_param );
    }
  }
}
void handler_SoraWoTobu_TameRelease( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(pokeID);
  GFL_UNUSED(work);
  // システム側で対処
}
//----------------------------------------------------------------------------------
/**
 * シャドーダイブ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_ShadowDive( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,            handler_ShadowDive_TameStart   },   // 溜め開始
    { BTL_EVENT_TAME_RELEASE,          handler_ShadowDive_TameRelease },   // 溜め解放
    { BTL_EVENT_CHECK_MAMORU_BREAK,    handler_Feint_MamoruBreak      },   // まもる無効化チェック
    { BTL_EVENT_NOEFFECT_CHECK_SIDEEFF_GUARD_BEGIN, handler_Feint_NoEffCheckBegin },  // 無効化チェック開始
    { BTL_EVENT_NOEFFECT_CHECK_SIDEEFF_GUARD_END,   handler_Feint_NoEffCheckEnd   },  // 無効化チェック終了
    { BTL_EVENT_DAMAGEPROC_END_HIT_L1, handler_ShadowDive_AfterDamage },   // ダメージ処理後
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_ShadowDive_TameStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = reinterpret_cast<BTL_HANDEX_PARAM_SET_CONTFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_CONTFLAG, pokeID ));
      param->pokeID = pokeID;
      param->flag = BTL_POKEPARAM::CONTFLG_SHADOWDIVE;
    flowWk->HANDEX_Pop( param );

    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_ShadowDive );
        HANDEX_STR_AddArg( &msg_param->str, pokeID );
      flowWk->HANDEX_Pop( msg_param );
    }
  }
}
void handler_ShadowDive_TameRelease( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(pokeID);
  GFL_UNUSED(work);
  // システム側で対処
}
//
void handler_ShadowDive_DmgStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(pokeID);
  GFL_UNUSED(work);

}
//
void handler_ShadowDive_AfterDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( target_pokeID );

    bool bExistGuardEffect = common_IsExistGuardTypeSideEffect( flowWk, target_pokeID, true );

    if( bpp->TURNFLAG_Get(BTL_POKEPARAM::TURNFLG_MAMORU)
    || (bExistGuardEffect)
    ){
      common_mamoruBreakAfter( flowWk, pokeID, bpp, BTL_STRID_SET_MamoruBreak );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * とびはねる
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Tobihaneru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_Tobihaneru_TameStart },     // 溜め開始
    { BTL_EVENT_TAME_RELEASE,     handler_Tobihaneru_TameRelease },   // 溜め解放
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Tobihaneru_TameStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = reinterpret_cast<BTL_HANDEX_PARAM_SET_CONTFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_CONTFLAG, pokeID ));
      param->pokeID = pokeID;
      param->flag = BTL_POKEPARAM::CONTFLG_SORAWOTOBU;
    flowWk->HANDEX_Pop( param );

    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Tobihaneru );
        HANDEX_STR_AddArg( &msg_param->str, pokeID );
      flowWk->HANDEX_Pop( msg_param );
    }
  }
}
void handler_Tobihaneru_TameRelease( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(pokeID);
  GFL_UNUSED(work);
  // システム側で対処
}
//----------------------------------------------------------------------------------
/**
 * ダイビング
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Diving( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_Diving_TameStart },     // 溜め開始
    { BTL_EVENT_TAME_RELEASE,     handler_Diving_TameRelease },   // 溜め解放
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Diving_TameStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = reinterpret_cast<BTL_HANDEX_PARAM_SET_CONTFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_CONTFLAG, pokeID ));
      param->pokeID = pokeID;
      param->flag = BTL_POKEPARAM::CONTFLG_DIVING;
    flowWk->HANDEX_Pop( param );

    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Diving );
        HANDEX_STR_AddArg( &msg_param->str, pokeID );
      flowWk->HANDEX_Pop( msg_param );
    }
  }
}
void handler_Diving_TameRelease( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(pokeID);
  GFL_UNUSED(work);
  // システム側で対処
}
//----------------------------------------------------------------------------------
/**
 * あなをほる
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_AnaWoHoru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_AnaWoHoru_TameStart },     // 溜め開始
    { BTL_EVENT_TAME_RELEASE,     handler_AnaWoHoru_TameRelease },   // 溜め解放
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_AnaWoHoru_TameStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = reinterpret_cast<BTL_HANDEX_PARAM_SET_CONTFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_CONTFLAG, pokeID ));
      param->pokeID = pokeID;
      param->flag = BTL_POKEPARAM::CONTFLG_ANAWOHORU;
    flowWk->HANDEX_Pop( param );

    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_AnaWoHoru );
        HANDEX_STR_AddArg( &msg_param->str, pokeID );
      flowWk->HANDEX_Pop( msg_param );
    }
  }
}
void handler_AnaWoHoru_TameRelease( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(pokeID);
  GFL_UNUSED(work);
  // システム側で対処
}
//----------------------------------------------------------------------------------
/**
 * ソーラービーム・ソーラーブレード
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_SolarBeam( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_TAMETURN_SKIP,  handler_SolarBeam_TameSkip },  // 溜めスキップ判定
    { BTL_EVENT_TAME_START,           handler_SolarBeam_TameStart }, // 溜め開始
    { BTL_EVENT_WAZA_POWER,           handler_SolarBeam_Power     }, // ワザ威力決定
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_SolarBeam_TameSkip( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( calc::IsShineWeather(flowWk->Hnd_GetWeather()) )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
    }
  }
}
void handler_SolarBeam_TameStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_SolarBeam );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    flowWk->HANDEX_Pop( msg_param );
  }
}
void handler_SolarBeam_Power( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BtlWeather weather = flowWk->Hnd_GetWeather( );
    if( (calc::IsRainWeather(weather))
    ||  (weather == BTL_WEATHER_SNOW)
    ||  (weather == BTL_WEATHER_SAND)
    ){
      EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(0.5) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * かまいたち
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Kamaitati( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_Kamaitati_TameStart },     // 溜め開始
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Kamaitati_TameStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Kamaitati );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    flowWk->HANDEX_Pop( msg_param );
  }
}
//----------------------------------------------------------------------------------
/**
 * ゴッドバード
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_GodBird( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_GodBird_TameStart },     // 溜め開始
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_GodBird_TameStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_GodBird );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    flowWk->HANDEX_Pop( msg_param );
  }
}
//----------------------------------------------------------------------------------
/**
 * ロケットずつき
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_RocketZutuki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
//    { BTL_EVENT_TAME_START,        handler_RocketZutuki_TameStart      }, // 溜め開始チェック
    { BTL_EVENT_TAME_START_FIXED,  handler_RocketZutuki_TameStart }, // 溜め開始確定
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_RocketZutuki_TameStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* msg_param;
    BTL_HANDEX_PARAM_RANK_EFFECT* rank_param;

    msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_RocketZutuki );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    flowWk->HANDEX_Pop( msg_param );

    rank_param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
      rank_param->poke_cnt = 1;
      rank_param->pokeID[0] = pokeID;
      rank_param->rankType = pml::wazadata::RANKEFF_DEFENCE;
      rank_param->rankVolume = 1;
      rank_param->fAlmost = true;
      rank_param->fWazaEffect = true;
    flowWk->HANDEX_Pop( rank_param );
  }
}

//------------------------------------------------------------------------------
/**
 *  ついばむ・むしくい
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_Tuibamu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL,   handler_Tuibamu              },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Tuibamu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetPokeID );
    u16 itemID = target->GetItem();
    if( ::item::ITEM_CheckNuts(itemID) )
    {
      BTL_HANDEX_PARAM_SET_ITEM* item_param;
      BTL_HANDEX_PARAM_ITEM_SP* eq_param;

      item_param = reinterpret_cast<BTL_HANDEX_PARAM_SET_ITEM *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_ITEM, pokeID ));
        item_param->pokeID = targetPokeID;
        item_param->itemID = ITEM_DUMMY_DATA;
        HANDEX_STR_Setup( &item_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Tuibamu );
        HANDEX_STR_AddArg( &item_param->exStr, pokeID );
        HANDEX_STR_AddArg( &item_param->exStr, itemID );
      flowWk->HANDEX_Pop( item_param );

      eq_param = reinterpret_cast<BTL_HANDEX_PARAM_ITEM_SP *>( flowWk->HANDEX_Push( BTL_HANDEX_ITEM_SP, pokeID ));
        eq_param->header.failSkipFlag = true;
        eq_param->itemID = itemID;
        eq_param->pokeID = pokeID;
        eq_param->bAteKinomi = true;
      flowWk->HANDEX_Pop( eq_param );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  わるあがき
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_Waruagaki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_CALL_DECIDE,  handler_Waruagaki_SeqStart    }, // ワザ処理開始ハンドラ
    { BTL_EVENT_WAZA_PARAM,        handler_Waruagaki_WazaParam   }, // ワザパラメータ取得
    { BTL_EVENT_CALC_KICKBACK,     handler_Waruagaki_KickBack    }, // 反動チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 反動チェックハンドラ
void handler_Waruagaki_KickBack( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 無効化禁止フラグON
    EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
  }
}
// ワザ処理開始ハンドラ
void handler_Waruagaki_SeqStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_WaruagakiExe );
      HANDEX_STR_AddArg( &param->str, pokeID );
    flowWk->HANDEX_Pop( param );
  }
}
// ワザパラメータ取得
void handler_Waruagaki_WazaParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_FLAT_FLAG, true );
  }
}
// ダメージプロセス終了ハンドラ
void handler_Waruagaki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    BTL_HANDEX_PARAM_SHIFT_HP* param = reinterpret_cast<BTL_HANDEX_PARAM_SHIFT_HP *>( flowWk->HANDEX_Push( BTL_HANDEX_SHIFT_HP, pokeID ));
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->volume[0] = (int)calc::QuotMaxHP( bpp, 4 ) * -1;
      param->damageCause = DAMAGE_CAUSE_WARUAGAKI;
    flowWk->HANDEX_Pop( param );
  }
}

//----------------------------------------------------------------------------------
/**
 * みちづれ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Michidure( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD,handler_Michidure_CheckFail    },  // ワザ出し成否チェックハンドラ
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Michidure_Ready        },  // 未分類ワザハンドラ
    { BTL_EVENT_ACTPROC_START,         handler_Michidure_ActStart     },  // アクション処理開始ハンドラ
    { BTL_EVENT_WAZA_DMG_REACTION_L2,  handler_Michidure_WazaDamage   },  // ワザダメージ処理後

// ターンチェックで消えると後攻で無意味なワザになるのでマズい
//    { BTL_EVENT_TURNCHECK_BEGIN,     handler_Michidure_TurnCheck  },  // ターンチェック開始ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

/**
 * @brief 「みちづれ」イベントファクタの張り付きを開始する
 */
void stickMitidureFactor( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  BTL_HANDEX_PARAM_SET_CONTFLAG* flagParam = reinterpret_cast<BTL_HANDEX_PARAM_SET_CONTFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_CONTFLAG, pokeID ) );
    flagParam->pokeID = pokeID;
    flagParam->flag   = BTL_POKEPARAM::CONTFLG_MITIDURE;
  flowWk->HANDEX_Pop( flagParam );

  work[ WORKIDX_STICK ] = 1;
}

/**
 * @brief 「みちづれ」イベントファクタの張り付を解除する
 */
void removeMitidureFactor( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  BTL_HANDEX_PARAM_RESET_CONTFLAG* param = reinterpret_cast<BTL_HANDEX_PARAM_RESET_CONTFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_RESET_CONTFLAG, pokeID ));
    param->pokeID = pokeID;
    param->flag   = BTL_POKEPARAM::CONTFLG_MITIDURE;
  flowWk->HANDEX_Pop( param );

  EVENT_FACTOR_Remove( myHandle );
}

// ワザ出し成否チェックハンドラ
void handler_Michidure_CheckFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( HandCommon_IsMyEvent(myHandle, BTL_EVAR_POKEID, pokeID) )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

    // 連続で「みちづれ」を出していたら失敗
    if( ( bpp->GetPrevWazaID() == WAZANO_MITIDURE ) &&
        ( bpp->GetWazaContCounter() > 0 ) )
    {
      EVENTVAR_RewriteValue(BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER);
    }
  }
}

// 未分類ワザハンドラ
void handler_Michidure_Ready( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( HandCommon_IsMyEvent(myHandle, BTL_EVAR_POKEID_ATK, pokeID) )
  {
    // 道連れ準備メッセージ表示
    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_MichidureReady );
      HANDEX_STR_AddArg( &param->str, pokeID );
    flowWk->HANDEX_Pop( param );

    // ハンドラ貼り付き
    stickMitidureFactor( myHandle, flowWk, pokeID, work );
  }
}
// アクション開始ハンドラ
void handler_Michidure_ActStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( flowWk );
  GFL_UNUSED( work );

  // アクション開始時点でハンドラが存在する（=前ターンに貼り付いた）なら削除
  if( common_checkActStart_isMyWaza(flowWk, pokeID ))
  {
    removeMitidureFactor( myHandle, flowWk, pokeID, work );
  }
}
// ワザダメージ処理後
void handler_Michidure_WazaDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF)==pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->IsDead() )
    {
      WazaNo usedWaza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
      if( !tables::IsDelayAttackWaza(usedWaza) )
      {
        u8 atkPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
        const BTL_POKEPARAM* attacker = flowWk->Hnd_GetPokeParam( atkPokeID );
        if( (!flowWk->Hnd_GetMainModule()->IsFriendPokeID(pokeID, atkPokeID))
        &&  (!attacker->IsDead())
        ){
          BTL_HANDEX_PARAM_KILL* kill_param;
          WazaID  atkWaza = (WazaID)(EVENTVAR_GetValue(BTL_EVAR_ORG_WAZAID));

          EVENT_FACTOR_ConvertForIsolate( myHandle );
          kill_param = reinterpret_cast<BTL_HANDEX_PARAM_KILL *>( flowWk->HANDEX_Push( BTL_HANDEX_KILL, atkPokeID ));
            kill_param->pokeID = pokeID;
            kill_param->fDeadPokeEnable = true;
            kill_param->recordWazaID = atkWaza;
            kill_param->deadCause = DAMAGE_CAUSE_WAZA;
          flowWk->HANDEX_Pop( kill_param );

          // 「みちづれ」の効果で死亡するポケモンは、ここでは退場処理を行いません。
          // ここで退場処理を行ってしまうと、
          // ダブルバトルで相手の2体攻撃によって自軍が全滅し、
          // 「みちづれ」によって相手も全滅した場合の勝敗判定が反転してしまうためです。
          kill_param = reinterpret_cast<BTL_HANDEX_PARAM_KILL *>( flowWk->HANDEX_Push( BTL_HANDEX_KILL, pokeID ));
            kill_param->pokeID              = atkPokeID;
            kill_param->recordWazaID        = (WazaID)(EVENT_FACTOR_GetSubID(myHandle));
            kill_param->fDisableDeadProcess = true;  // 退場処理を行わない
            kill_param->deadCause           = DAMAGE_CAUSE_MITIDURE;
            HANDEX_STR_Setup( &kill_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_MichidureDone );
            HANDEX_STR_AddArg( &kill_param->exStr, pokeID );
          flowWk->HANDEX_Pop( kill_param );
        }
      }

      // 自分が死んでたら削除
      removeMitidureFactor( myHandle, flowWk, pokeID, work );
    }
  }
}


//----------------------------------------------------------------------------------
/**
 * おんねん
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Onnen( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,       handler_Onnen_Ready       }, // 未分類ワザハンドラ
    { BTL_EVENT_WAZA_DMG_REACTION,       handler_Onnen_WazaDamage  }, // ワザダメージ処理後
    { BTL_EVENT_ACTPROC_START,           handler_Onnen_ActStart    }, // アクション処理開始ハンドラ

//    ターンチェックで自殺は後攻で無意味なワザになっちゃうのでダメ
//    { BTL_EVENT_TURNCHECK_BEGIN,             handler_Onnen_TurnCheck  }, // ターンチェック開始ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

/**
 * @brief 「おんねん」イベントファクタの張り付きを開始する
 */
void stickOnnenFactor( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  BTL_HANDEX_PARAM_SET_CONTFLAG* flagParam = reinterpret_cast<BTL_HANDEX_PARAM_SET_CONTFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_CONTFLAG, pokeID ) );
    flagParam->pokeID = pokeID;
    flagParam->flag   = BTL_POKEPARAM::CONTFLG_ONNEN;
  flowWk->HANDEX_Pop( flagParam );

  work[ WORKIDX_STICK ] = 1;
}

/**
 * @brief 「おんねん」イベントファクタの張り付を解除する
 */
void removeOnnenFactor( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  BTL_HANDEX_PARAM_RESET_CONTFLAG* param = reinterpret_cast<BTL_HANDEX_PARAM_RESET_CONTFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_RESET_CONTFLAG, pokeID ));
    param->pokeID = pokeID;
    param->flag   = BTL_POKEPARAM::CONTFLG_ONNEN;
  flowWk->HANDEX_Pop( param );

  EVENT_FACTOR_Remove( myHandle );
}

void handler_Onnen_Ready( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( HandCommon_IsMyEvent(myHandle, BTL_EVAR_POKEID_ATK, pokeID) )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_OnnenReady );
      HANDEX_STR_AddArg( &param->str, pokeID );
    flowWk->HANDEX_Pop( param );

    // ハンドラ貼り付き
    stickOnnenFactor( myHandle, flowWk, pokeID, work );
  }
}
void handler_Onnen_WazaDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF)==pokeID )
  {
    WazaNo waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    if( !tables::IsDelayAttackWaza(waza) )
    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
      if( bpp->IsDead() )
      {
        WazaID  orgWaza;
        u8     wazaIdx;

        u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
        const BTL_POKEPARAM* targetPoke = flowWk->Hnd_GetPokeParam( target_pokeID );

        orgWaza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_ORG_WAZAID ));
        wazaIdx = targetPoke->WAZA_SearchIdx( orgWaza );
        if( wazaIdx != PTL_WAZA_MAX )
        {
          BTL_HANDEX_PARAM_PP* pp_param;

          pp_param = reinterpret_cast<BTL_HANDEX_PARAM_PP *>( flowWk->HANDEX_Push( BTL_HANDEX_DECREMENT_PP, pokeID ));
            pp_param->pokeID = target_pokeID;
            pp_param->wazaIdx = wazaIdx;
            pp_param->volume = targetPoke->WAZA_GetPP( wazaIdx ) * -1;
            pp_param->fDeadPokeEnable = true;

            HANDEX_STR_Setup( &pp_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_OnnenDone );
            HANDEX_STR_AddArg( &pp_param->exStr, target_pokeID );
            HANDEX_STR_AddArg( &pp_param->exStr, orgWaza );

          flowWk->HANDEX_Pop( pp_param );
        }

        // おんねん成功で自殺
        removeOnnenFactor( myHandle, flowWk, pokeID, work );
      }
    }
  }
}
// アクション開始ハンドラ
void handler_Onnen_ActStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( flowWk );
  GFL_UNUSED( work );

  // アクション開始時点でハンドラが存在する（=前ターンに貼り付いた）なら削除
  if( common_checkActStart_isMyWaza(flowWk, pokeID) )
  {
    removeOnnenFactor( myHandle, flowWk, pokeID, work );
  }
}

//----------------------------------------------------------------------------------
/**
 * ちいさくなる
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Tiisakunaru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXE_START,  handler_Tiisakunaru   },  // ワザ出し確定決定
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

void handler_Tiisakunaru( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = reinterpret_cast<BTL_HANDEX_PARAM_SET_CONTFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_CONTFLAG, pokeID ));
      param->pokeID = pokeID;
      param->flag = BTL_POKEPARAM::CONTFLG_TIISAKUNARU;
    flowWk->HANDEX_Pop( param );
  }
}
//----------------------------------------------------------------------------------
/**
 * まるくなる
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Marukunaru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXE_START,  handler_Marukunaru   },  // ワザ威力決定
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Marukunaru( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = reinterpret_cast<BTL_HANDEX_PARAM_SET_CONTFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_CONTFLAG, pokeID ));
      param->pokeID = pokeID;
      param->flag = BTL_POKEPARAM::CONTFLG_MARUKUNARU;
    flowWk->HANDEX_Pop( param );
  }
}
//----------------------------------------------------------------------------------
/**
 * はねやすめ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Haneyasume( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_EFFECTIVE,  handler_Haneyasume   },  // ワザ出し有効ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Haneyasume( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_SICK* sick_param;

    sick_param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
      sick_param->pokeID = pokeID;
      sick_param->fStdMsgDisable = true;
      sick_param->sickID = pml::wazadata::WAZASICK_HANEYASUME;
      sick_param->sickCont = SICKCONT_MakeTurnParam( pokeID, 1, POKETYPE_HIKOU );
      SICKCONT_SetFlag( &sick_param->sickCont, true );
    flowWk->HANDEX_Pop( sick_param );
  }
}
//----------------------------------------------------------------------------------
/**
 * きあいパンチ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_KiaiPunch( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_BEFORE_FIGHT,  handler_KiaiPunch   },  // ターン最初のワザシーケンス直前
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_KiaiPunch( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_TURNFLAG* param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    param = reinterpret_cast<BTL_HANDEX_PARAM_TURNFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_TURNFLAG, pokeID ));
      param->pokeID = pokeID;
      param->flag = BTL_POKEPARAM::TURNFLG_KIAI_READY;
    flowWk->HANDEX_Pop( param );

    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
      if( !bpp->IsWazaHide() )
      {
        BTL_HANDEX_PARAM_ADD_EFFECT* eff_param;
        eff_param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_EFFECT, pokeID ));
          eff_param->effectNo = BTLEFF_KIAIPUNCH_TAME;
          eff_param->pos_from = flowWk->Hnd_PokeIDtoPokePos( pokeID );
          eff_param->pos_to = BTL_POS_NULL;
          eff_param->fMsgWinVanish = true;
        flowWk->HANDEX_Pop( eff_param );
      }
    }


    msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_KiaiPunch );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    flowWk->HANDEX_Pop( msg_param );
  }
}

//----------------------------------------------------------------------------------
/**
 * ゆびをふる
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_YubiWoFuru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
//    { BTL_EVENT_REQWAZA_FOR_ACT_ORDER,  handler_YubiWoFuru     },  // 他ワザ呼び出し（順序計算前から）
    { BTL_EVENT_REQWAZA_PARAM,         handler_YubiWoFuru                    },  // 他ワザパラメータチェック
    { BTL_EVENT_REQWAZA_ZENRYOKU_MSG,  handler_YubiwoFuru_ZenryokuReqWazaMsg },  // 派生全力技の抽選結果メッセージ
    { BTL_EVENT_REQWAZA_MSG,           handler_YubiWoFuru_Msg                },  // 他ワザ実行時メッセージ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_YubiWoFuru( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const WazaID* ommitTable;
    u32 ommitTableElems;
    WazaNo waza;
    BtlPokePos targetPos;

    ommitTable = tables::GetYubiFuruOmmitTable( &ommitTableElems );
    waza = calc::RandWaza(ommitTable, ommitTableElems );

    #if PM_DEBUG
    {
      BtlPokePos myPos = flowWk->Hnd_PokeIDtoPokePos( pokeID );
      WazaNo  debug_wazaNo = (WazaNo)(tables::YubifuruDebug_GetNumber( myPos ));
      if( debug_wazaNo > 0 ){
        waza = debug_wazaNo;
        tables::YubifuruDebugInc( myPos );
      }
    }
    #endif

    targetPos  = flowWk->Hnd_ReqWazaTargetAuto( pokeID, waza );
    BTL_PRINT("ゆびをふる：使ったポケ=%d, 出すワザ=%d, ねらう相手の位置=%d\n", pokeID, waza, targetPos );

    EVENTVAR_RewriteValue( BTL_EVAR_WAZAID,  waza );
    EVENTVAR_RewriteValue( BTL_EVAR_POKEPOS, targetPos );
  }
}

/**
 * @brief 派生した技を全力ブーストしたメッセージ
 * @param myHandle
 * @param flowWk
 * @param pokeID   技を使用したポケモンのID
 * @param work     イベントファクタの作業領域
 *
 * BTL_EVAR_ORG_WAZAID      [in] 派生した技
 * BTL_EVAR_ZENRYOKU_WAZAID [in] 派生した技の全力技
 * BTL_EVAR_WORK_ADRS       [in] カスタムメッセージのパラメータを設定するワークのアドレス
 * BTL_EVAR_GEN_FLAG        [out]カスタムメッセージを表示する場合 true
 */
void handler_YubiwoFuru_ZenryokuReqWazaMsg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( myHandle );
  GFL_UNUSED( flowWk );
  GFL_UNUSED( work );
  if( EVENTVAR_GetValue( BTL_EVAR_POKEID ) != pokeID ) {
    return;
  }

  //「○○は　ゆびをふるで　でた　××を　全力ブーストした！」
  BTL_HANDEX_STR_PARAMS* str = (BTL_HANDEX_STR_PARAMS*)( EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS ) );
  HANDEX_STR_Setup( str, BTL_STRTYPE_SET, BTL_STRID_SET_AuraReqWaza_Yubiwohuru );
  HANDEX_STR_AddArg( str, pokeID );
  HANDEX_STR_AddArg( str, EVENTVAR_GetValue( BTL_EVAR_ORG_WAZAID ) );
  EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
}


void handler_YubiWoFuru_Msg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue( BTL_EVAR_POKEID ) != pokeID ) {
    return;
  }

  if( EVENTVAR_GetValue( BTL_EVAR_ZENRYOKU_WAZA_FLAG ) ) {
    return;
  }

  //「ゆびを　ふったら　○○が　でた！」
  BTL_HANDEX_STR_PARAMS* str = (BTL_HANDEX_STR_PARAMS*)EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );
  WazaNo  waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
  HANDEX_STR_Setup( str, BTL_STRTYPE_STD, BTL_STRID_STD_YubiWoFuru );
  HANDEX_STR_AddArg( str, waza );
  EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
}


//----------------------------------------------------------------------------------
/**
 * しぜんのちから
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_SizenNoTikara( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_REQWAZA_PARAM,         handler_SizenNoTikara                    }, // 派生ワザパラメータチェック
    { BTL_EVENT_REQWAZA_MSG,           handler_SizenNoTikara_Msg                }, // 派生ワザ実行時メッセージ
    { BTL_EVENT_REQWAZA_ZENRYOKU_MSG,  handler_Sizennotikara_ZenryokuReqWazaMsg }, // 派生全力技の抽選結果メッセージ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 他ワザ呼び出しパラメータチェック
void handler_SizenNoTikara( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    WazaID  waza = WAZANO_TORAIATAKKU; // デフォルト「トライアッタック」にしとく

    BtlGround  grnd = flowWk->Hnd_GetGround();
    switch( grnd )
    {
    case BTL_GROUND_GRASS:  waza = WAZANO_ENAZIIBOORU;    break;
    case BTL_GROUND_MIST:   waza = WAZANO_MUUNFOOSU;      break;
    case BTL_GROUND_ELEKI:  waza = WAZANO_10MANBORUTO;    break;
    case BTL_GROUND_PHYCHO: waza = WAZANO_SAIKOKINESISU;  break;
    case BTL_GROUND_AQUA:   waza = WAZANO_HAIDOROPONPU;   break;
    case BTL_GROUND_ICE:    waza = WAZANO_REITOUBIIMU;    break; 
    }

    if( waza == WAZANO_TORAIATAKKU )
    {
      const BgComponentData& bgData = flowWk->Hnd_GetBgComponentData();
      waza = static_cast<WazaID>( bgData.sizennotikaraWazaNo );
    }

    BtlPokePos pos  = (BtlPokePos)(EVENTVAR_GetValue( BTL_EVAR_POKEPOS_ORG ));
    if( pos == BTL_POS_NULL ){
      pos = flowWk->Hnd_ReqWazaTargetAuto( pokeID, waza );
    }

    EVENTVAR_RewriteValue( BTL_EVAR_WAZAID,  waza );
    EVENTVAR_RewriteValue( BTL_EVAR_POKEPOS, pos );
  }
}
// 派生ワザ実行時メッセージ
void handler_SizenNoTikara_Msg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) != pokeID ) {
    return;
  }

  if( EVENTVAR_GetValue( BTL_EVAR_ZENRYOKU_WAZA_FLAG ) ) {
    return;
  }

  BTL_HANDEX_STR_PARAMS* str = (BTL_HANDEX_STR_PARAMS*)EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );
  WazaNo  waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
  HANDEX_STR_Setup( str, BTL_STRTYPE_STD, BTL_STRID_STD_SizenNoTikara );
  HANDEX_STR_AddArg( str, waza );
  EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
}

/**
 * @brief 派生した技を全力ブーストしたメッセージ
 * @param myHandle
 * @param flowWk
 * @param pokeID   技を使用したポケモンのID
 * @param work     イベントファクタの作業領域
 *
 * BTL_EVAR_ORG_WAZAID      [in] 派生した技
 * BTL_EVAR_ZENRYOKU_WAZAID [in] 派生した技の全力技
 * BTL_EVAR_WORK_ADRS       [in] カスタムメッセージのパラメータを設定するワークのアドレス
 * BTL_EVAR_GEN_FLAG        [out]カスタムメッセージを表示する場合 true
 */
void handler_Sizennotikara_ZenryokuReqWazaMsg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( myHandle );
  GFL_UNUSED( flowWk );
  GFL_UNUSED( work );
  if( EVENTVAR_GetValue( BTL_EVAR_POKEID ) != pokeID ) {
    return;
  }
  BTL_HANDEX_STR_PARAMS* str = (BTL_HANDEX_STR_PARAMS*)( EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS ) );
  WazaNo zenryokuWaza = static_cast<WazaNo>( EVENTVAR_GetValue( BTL_EVAR_ZENRYOKU_WAZAID ) );
  HANDEX_STR_Setup( str, BTL_STRTYPE_SET, BTL_STRID_SET_AuraReqWaza_Sizennotikara );
  HANDEX_STR_AddArg( str, pokeID );
  HANDEX_STR_AddArg( str, zenryokuWaza );
  EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
}

//----------------------------------------------------------------------------------
/**
 * ねこのて
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Nekonote( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_REQWAZA_ZENRYOKU_MSG,   handler_Nekonote_ZenryokuReqWazaMsg }, // 派生全力技の抽選結果メッセージ
    { BTL_EVENT_REQWAZA_PARAM,          handler_Nekonote                    }, // 他ワザパラメータチェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Nekonote( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_PARTY* party = flowWk->Hnd_GetPartyData( pokeID );
    u32  arySize = PTL_WAZA_MAX * BTL_PARTY_MEMBER_MAX * sizeof(u16);
    WazaNo* wazaAry = reinterpret_cast<WazaNo*>(flowWk->Hnd_GetTmpWork( arySize ));
    const BTL_POKEPARAM* bpp;

    u8 memberCnt, wazaCnt, i;

    wazaCnt = 0;
    memberCnt = party->GetMemberCount();
    for(i=0; i<memberCnt; ++i)
    {
      bpp = party->GetMemberDataConst( i );
      if( bpp->GetID() == pokeID ){                     // 自分はねこのてメンバーから除外
        continue;
      }
      if( !bpp->IsDead() && !bpp->IsFightEnable() ){    // 死んでない＆戦えない = タマゴはメンバーから除外
        continue;
      }

      {
        u8 pokeWazaMax = bpp->WAZA_GetCount();
        u8 j;
        WazaNo waza;
        for(j=0; j<pokeWazaMax; ++j)
        {
          waza = bpp->WAZA_GetID( j );
          if( !tables::IsNekoNoteOmmit(waza) ){
            wazaAry[wazaCnt++] = waza;
          }
        }
      }
    }

    if( wazaCnt )
    {
      u8 idx         = calc::GetRand( wazaCnt );
      BtlPokePos pos = flowWk->Hnd_ReqWazaTargetAuto( pokeID, wazaAry[idx] );

      EVENTVAR_RewriteValue( BTL_EVAR_WAZAID,  wazaAry[idx] );
      EVENTVAR_RewriteValue( BTL_EVAR_POKEPOS, pos );
    }
  }
}
void handler_Nekonote_CheckParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    WazaNo  waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    if( waza == WAZANO_NULL ){
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
    }
  }
}

/**
 * @brief 派生した技を全力ブーストしたメッセージ
 * @param myHandle
 * @param flowWk
 * @param pokeID   技を使用したポケモンのID
 * @param work     イベントファクタの作業領域
 *
 * BTL_EVAR_ORG_WAZAID      [in] 派生した技
 * BTL_EVAR_ZENRYOKU_WAZAID [in] 派生した技の全力技
 * BTL_EVAR_WORK_ADRS       [in] カスタムメッセージのパラメータを設定するワークのアドレス
 * BTL_EVAR_GEN_FLAG        [out]カスタムメッセージを表示する場合 true
 */
void handler_Nekonote_ZenryokuReqWazaMsg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( myHandle );
  GFL_UNUSED( flowWk );
  GFL_UNUSED( work );
  if( EVENTVAR_GetValue( BTL_EVAR_POKEID ) != pokeID ) {
    return;
  }
  BTL_HANDEX_STR_PARAMS* str = (BTL_HANDEX_STR_PARAMS*)( EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS ) );
  WazaNo actWaza = static_cast<WazaNo>( EVENTVAR_GetValue( BTL_EVAR_ORG_WAZAID ) );
  HANDEX_STR_Setup( str, BTL_STRTYPE_SET, BTL_STRID_SET_AuraReqWaza_Nekonote );
  HANDEX_STR_AddArg( str, pokeID );
  HANDEX_STR_AddArg( str, actWaza );
  EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
}

//----------------------------------------------------------------------------------
/**
 * ねごと
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Negoto( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_FAIL_THREW,        handler_Ibiki_CheckFail_1         }, // ワザ出し成否チェックハンドラ１
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD, handler_Ibiki_CheckFail_2         }, // ワザ出し成否チェックハンドラ２
    { BTL_EVENT_REQWAZA_PARAM,          handler_Negoto                    }, // 派生わざパラメータチェック
    { BTL_EVENT_REQWAZA_ZENRYOKU_MSG,   handler_Negoto_ZenryokuReqWazaMsg }, // 派生全力技の抽選結果メッセージ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Negoto( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->CheckNemuri( BTL_POKEPARAM::NEMURI_CHECK_INCLUDE_ZETTAINEMURI ) )
    {
      WazaNo waza;
      u8 wazaMax, i, cnt;
      wazaMax = bpp->WAZA_GetCount( );
      for(i=0, cnt=0; i<wazaMax; ++i)
      {
        waza = bpp->WAZA_GetID( i );
        TAYA_Printf("waza Idx=%d, NO=%d\n", i, waza);
        if( (!tables::IsNegotoOmmit(waza))
        &&  (WAZADATA_GetFlag(waza, pml::wazadata::WAZAFLAG_Tame) == false)
        ){
          work[cnt++] = waza;
        }
      }
      if( cnt )
      {
        u8 idx = calc::GetRand( cnt );
        waza = static_cast<WazaNo>(work[ idx ]);
        BtlPokePos  pos = flowWk->Hnd_ReqWazaTargetAuto( pokeID, waza );

        EVENTVAR_RewriteValue( BTL_EVAR_WAZAID,  waza );
        EVENTVAR_RewriteValue( BTL_EVAR_POKEPOS, pos );
      }
      else
      {
        EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
      }
    }
  }
}
void handler_Negoto_CheckParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    WazaNo  waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    if( waza == WAZANO_NULL ){
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
    }
  }
}

/**
 * @brief 派生した技を全力ブーストしたメッセージ
 * @param myHandle
 * @param flowWk
 * @param pokeID   技を使用したポケモンのID
 * @param work     イベントファクタの作業領域
 *
 * BTL_EVAR_ORG_WAZAID      [in] 派生した技
 * BTL_EVAR_ZENRYOKU_WAZAID [in] 派生した技の全力技
 * BTL_EVAR_WORK_ADRS       [in] カスタムメッセージのパラメータを設定するワークのアドレス
 * BTL_EVAR_GEN_FLAG        [out]カスタムメッセージを表示する場合 true
 */
void handler_Negoto_ZenryokuReqWazaMsg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( myHandle );
  GFL_UNUSED( flowWk );
  GFL_UNUSED( work );
  if( EVENTVAR_GetValue( BTL_EVAR_POKEID ) != pokeID ) {
    return;
  }
  BTL_HANDEX_STR_PARAMS* str = (BTL_HANDEX_STR_PARAMS*)( EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS ) );
  WazaNo actWaza = static_cast<WazaNo>( EVENTVAR_GetValue( BTL_EVAR_ORG_WAZAID ) );
  HANDEX_STR_Setup( str, BTL_STRTYPE_SET, BTL_STRID_SET_AuraReqWaza_Negoto );
  HANDEX_STR_AddArg( str, pokeID );
  HANDEX_STR_AddArg( str, actWaza );
  EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
}

//----------------------------------------------------------------------------------
/**
 * オウムがえし
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_OumuGaesi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_REQWAZA_ZENRYOKU_MSG,   handler_Oumugaesi_ZenryokuReqWazaMsg }, // 派生全力技の抽選結果メッセージ
    { BTL_EVENT_REQWAZA_PARAM,          handler_OumuGaesi_CheckParam         }, // 他ワザパラメータチェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

/**
 * @brief 派生した技を全力ブーストしたメッセージ
 * @param myHandle
 * @param flowWk
 * @param pokeID   技を使用したポケモンのID
 * @param work     イベントファクタの作業領域
 *
 * BTL_EVAR_ORG_WAZAID      [in] 派生した技
 * BTL_EVAR_ZENRYOKU_WAZAID [in] 派生した技の全力技
 * BTL_EVAR_WORK_ADRS       [in] カスタムメッセージのパラメータを設定するワークのアドレス
 * BTL_EVAR_GEN_FLAG        [out]カスタムメッセージを表示する場合 true
 */
void handler_Oumugaesi_ZenryokuReqWazaMsg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( myHandle );
  GFL_UNUSED( flowWk );
  GFL_UNUSED( work );
  if( EVENTVAR_GetValue( BTL_EVAR_POKEID ) != pokeID ) {
    return;
  }
  BTL_HANDEX_STR_PARAMS* str = (BTL_HANDEX_STR_PARAMS*)( EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS ) );
  WazaNo actWaza = static_cast<WazaNo>( EVENTVAR_GetValue( BTL_EVAR_ORG_WAZAID ) );
  HANDEX_STR_Setup( str, BTL_STRTYPE_SET, BTL_STRID_SET_AuraReqWaza_Oumugaesi );
  HANDEX_STR_AddArg( str, pokeID );
  HANDEX_STR_AddArg( str, actWaza );
  EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
}

void handler_OumuGaesi_CheckParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BtlPokePos targetPos = EVENTVAR_GetValue( BTL_EVAR_POKEPOS_ORG );
    u8* targetPokeIDAry = (u8*)work;

    BTL_PRINT( "オウムデフォ位置=%d\n", targetPos);

    // シングル戦の場合、相手選択の必要がないのでNULLが来る
    if( targetPos == BTL_POS_NULL ){
      BtlPokePos  myPos = flowWk->Hnd_GetExistFrontPokePos( pokeID );
      BtlRule  rule = flowWk->Hnd_GetRule( );
      targetPos = MainModule::GetOpponentPokePosByRule( rule, myPos, 0 );
      BTL_PRINT("オウムがえし狙う位置 = %d\n", targetPos );
    }


    if( flowWk->Hnd_ExpandPokeID(targetPos, targetPokeIDAry) )
    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( targetPokeIDAry[0] );
      WazaID  waza = bpp->GetPrevWazaID();
      bool isPrevWazaZ = bpp->IsPrevWazaZenryoku();   // @fix GFNMcat[37] オウムがえしでＺ技の元技が発動してしまう

      BTL_PRINT( "オウム：狙った位置にポケモンいた。位置=%d, ID=%d, waza=%d, flg=%dです\n", targetPos, targetPokeIDAry[0], waza, WAZADATA_GetFlag(waza, pml::wazadata::WAZAFLAG_Oumu) );

      if( (waza == WAZANO_NULL)
      ||  (WAZADATA_GetFlag(waza, pml::wazadata::WAZAFLAG_Oumu) == false)
      ||  (isPrevWazaZ)
      ){
        EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
      }
      else{
        EVENTVAR_RewriteValue( BTL_EVAR_WAZAID, waza );
        EVENTVAR_RewriteValue( BTL_EVAR_POKEPOS, targetPos );
      }
    }
    else
    {
      BTL_PRINT( "オウム：狙った位置にポケモンいない\n");
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * さきどり
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Sakidori( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_REQWAZA_ZENRYOKU_MSG,  handler_Sakidori_ZenryokuReqWazaMsg }, // 派生全力技の抽選結果メッセージ
    { BTL_EVENT_REQWAZA_PARAM,         handler_Sakidori_CheckParam         }, // 他ワザパラメータチェック
    { BTL_EVENT_WAZA_POWER,            handler_Sakidori_Power              }, // ワザ威力決定
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

/**
 * @brief 派生した技を全力ブーストしたメッセージ
 * @param myHandle
 * @param flowWk
 * @param pokeID   技を使用したポケモンのID
 * @param work     イベントファクタの作業領域
 *
 * BTL_EVAR_ORG_WAZAID      [in] 派生した技
 * BTL_EVAR_ZENRYOKU_WAZAID [in] 派生した技の全力技
 * BTL_EVAR_WORK_ADRS       [in] カスタムメッセージのパラメータを設定するワークのアドレス
 * BTL_EVAR_GEN_FLAG        [out]カスタムメッセージを表示する場合 true
 */
void handler_Sakidori_ZenryokuReqWazaMsg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( myHandle );
  GFL_UNUSED( flowWk );
  GFL_UNUSED( work );
  if( EVENTVAR_GetValue( BTL_EVAR_POKEID ) != pokeID ) {
    return;
  }
  BTL_HANDEX_STR_PARAMS* str = (BTL_HANDEX_STR_PARAMS*)( EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS ) );
  WazaNo actWaza = static_cast<WazaNo>( EVENTVAR_GetValue( BTL_EVAR_ORG_WAZAID ) );
  HANDEX_STR_Setup( str, BTL_STRTYPE_SET, BTL_STRID_SET_AuraReqWaza_Sakidori );
  HANDEX_STR_AddArg( str, pokeID );
  HANDEX_STR_AddArg( str, actWaza );
  EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
}


void handler_Sakidori_CheckParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BtlPokePos targetPos = EVENTVAR_GetValue( BTL_EVAR_POKEPOS_ORG );
    u8* targetPokeIDAry = (u8*)work;

    // シングル戦の場合、相手選択の必要がないのでNULLが来る
    if( targetPos == BTL_POS_NULL )
    {
      BtlPokePos  myPos = flowWk->Hnd_GetExistFrontPokePos( pokeID );
      BtlRule  rule = flowWk->Hnd_GetRule( );
      targetPos = MainModule::GetOpponentPokePosByRule( rule, myPos, 0 );
      TAYA_Printf("myPos=%d, targetPos=%d\n", myPos, targetPos);
    }

    if( flowWk->Hnd_ExpandPokeID( targetPos, targetPokeIDAry) )
    {
      u8 targetPokeID = targetPokeIDAry[0];
      u8 fSucceess = handler_Sakidori_CheckSuccess( flowWk, targetPokeID );

      TAYA_Printf("さきどり対象位置=%d, pokeID=%d\n", targetPos, targetPokeID );

      if( fSucceess )
      {
        BTL_ACTION_PARAM action;
        flowWk->Hnd_GetThisTurnAction( targetPokeID, &action );

        const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( targetPokeID );
        WazaID waza = BTL_ACTION_GetWazaID( &action, bpp );

        EVENTVAR_RewriteValue( BTL_EVAR_WAZAID, waza );
        EVENTVAR_RewriteValue( BTL_EVAR_POKEPOS, targetPos );
        return;
      }
    }

    EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
  }
}

/**
 * @brief 「さきどり」が成功するかどうかチェックする
 * @retval true   「さきどり」が成功
 * @retval false  「さきどり」が失敗
 */
bool handler_Sakidori_CheckSuccess( ServerFlow* flowWk, u8 targetPokeID )
{
  BTL_ACTION_PARAM action;
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( targetPokeID );

  // 相手が既に行動していたら失敗
  if( bpp->TURNFLAG_Get(BTL_POKEPARAM::TURNFLG_WAZAPROC_DONE) ) {
    return false; 
  }

  // 現ターンのアクションが取得できなかったら失敗
  if( !flowWk->Hnd_GetThisTurnAction( targetPokeID, &action ) ) {
    return false; 
  }

  // 相手が全力技を選択していた場合は失敗
  if( BTL_ACTION_IsFightWithZenryokuWaza( &action ) ) {
    return false; 
  }

  // 対象が場にいなければ失敗
  if( !flowWk->Hnd_IsExistPokemon( targetPokeID ) ) {
    return false;
  }

  // フリーフォールしてる側・されてる側に失敗
  if( flowWk->Hnd_IsFreeFallPoke( targetPokeID ) ) {
    return false;
  }

  // 相手がダメージワザを選択していない場合も失敗
  WazaID waza = BTL_ACTION_GetWazaID( &action, bpp );
  if( ( waza == WAZANO_NULL ) ||
      !( WAZADATA_IsDamage( waza ) ) ) { 
    return false;
  }

  // さきどり禁止ワザなら失敗
  if( tables::IsSakidoriFailWaza( waza ) ) {
    return false;
  }

  return true;
}

void handler_Sakidori_Power( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.5) );
  }
}
//----------------------------------------------------------------------------------
/**
 * まねっこ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Manekko( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_REQWAZA_ZENRYOKU_MSG,   handler_Manekko_ZenryokuReqWazaMsg }, // 派生全力技の抽選結果メッセージ
    { BTL_EVENT_REQWAZA_PARAM,          handler_Manekko_CheckParam         }, // 他ワザパラメータチェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

/**
 * @brief 派生した技を全力ブーストしたメッセージ
 * @param myHandle
 * @param flowWk
 * @param pokeID   技を使用したポケモンのID
 * @param work     イベントファクタの作業領域
 *
 * BTL_EVAR_ORG_WAZAID      [in] 派生した技
 * BTL_EVAR_ZENRYOKU_WAZAID [in] 派生した技の全力技
 * BTL_EVAR_WORK_ADRS       [in] カスタムメッセージのパラメータを設定するワークのアドレス
 * BTL_EVAR_GEN_FLAG        [out]カスタムメッセージを表示する場合 true
 */
void handler_Manekko_ZenryokuReqWazaMsg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( myHandle );
  GFL_UNUSED( flowWk );
  GFL_UNUSED( work );
  if( EVENTVAR_GetValue( BTL_EVAR_POKEID ) != pokeID ) {
    return;
  }
  BTL_HANDEX_STR_PARAMS* str = (BTL_HANDEX_STR_PARAMS*)( EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS ) );
  WazaNo actWaza = static_cast<WazaNo>( EVENTVAR_GetValue( BTL_EVAR_ORG_WAZAID ) );
  HANDEX_STR_Setup( str, BTL_STRTYPE_SET, BTL_STRID_SET_AuraReqWaza_Manekko );
  HANDEX_STR_AddArg( str, pokeID );
  HANDEX_STR_AddArg( str, actWaza );
  EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
}

void handler_Manekko_CheckParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    WazaID waza = flowWk->Hnd_GetPrevExeWaza( );
    if( (waza != WAZANO_NULL)
    &&  (!tables::IsManekkoOmmit(waza))
    &&  (!flowWk->Hnd_IsPrevExeZenryokuWaza())
    ){
      BtlPokePos  pos = flowWk->Hnd_ReqWazaTargetAuto( pokeID, waza );
      EVENTVAR_RewriteValue( BTL_EVAR_WAZAID, waza );
      EVENTVAR_RewriteValue( BTL_EVAR_POKEPOS, pos );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * げんしのちから・あやしいかぜ・ぎんいろのかぜ・セブンエボルブースト
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_GensiNoTikara( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_GET_RANKEFF_VALUE,   handler_GensiNoTikara  },  // ワザランク増減効果チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザランク増減効果チェックハンドラ
void handler_GensiNoTikara( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 自分がワザ使用者＆対象のケースで
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  ){
    EVENTVAR_RewriteValue( BTL_EVAR_STATUS_TYPE, pml::wazadata::RANKEFF_MULTI5 );
  }
}

//----------------------------------------------------------------------------------
/**
 * ベノムショック
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_BenomShock( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_BenomShock },    // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_BenomShock( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // どく状態の相手に威力２倍
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( bpp->CheckSick(pml::wazadata::WAZASICK_DOKU) ){
      EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * たたりめ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Tatarime( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER_BASE,   handler_Tatarime },    // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Tatarime( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ポケ系状態状態の相手に威力２倍
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( ( bpp->GetPokeSick() != pml::pokepara::SICK_NULL ) ||
        ( bpp->CheckNemuri( BTL_POKEPARAM::NEMURI_CHECK_INCLUDE_ZETTAINEMURI ) ) )
    {
      HandCommon_MulWazaBasePower( 2 );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * アクロバット
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Acrobat( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER_BASE,   handler_Acrobat },    // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Acrobat( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 自分がどうぐを装備していない時に威力２倍
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->GetItem() == ITEM_DUMMY_DATA ){
      HandCommon_MulWazaBasePower( 2 );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * アシストパワー・つけあがる
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_AsistPower( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER_BASE,   handler_AsistPower },    // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_AsistPower( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  static const   BTL_POKEPARAM::ValueID  RankValue[] = {
    BTL_POKEPARAM::BPP_ATTACK_RANK,      BTL_POKEPARAM::BPP_DEFENCE_RANK,
    BTL_POKEPARAM::BPP_AGILITY_RANK,     BTL_POKEPARAM::BPP_SP_ATTACK_RANK,
    BTL_POKEPARAM::BPP_SP_DEFENCE_RANK,  BTL_POKEPARAM::BPP_HIT_RATIO,
    BTL_POKEPARAM::BPP_AVOID_RATIO,
  };

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 自分の能力変化上昇値合計*20を威力に加算
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u32  upSum = 0;
    int  rank;
    int i;

    for(i=0; i<GFL_NELEMS(RankValue); ++i)
    {
      rank = bpp->GetValue( RankValue[i] ) - BTL_POKEPARAM::RANK_STATUS_DEFAULT;
      if( rank > 0 ){
        upSum += rank;
      }
    }

    if( upSum )
    {
      u32 power = EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
      power += (upSum * 20);
      EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, power );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ヘビーボンバー
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_HeavyBomber( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER_BASE,   handler_HeavyBomber           }, // ワザ威力チェックハンドラ
    { BTL_EVENT_CALC_HIT_CANCEL,   handler_Fumituke_HitCheckSkip }, // 命中計算スキップ
    { BTL_EVENT_WAZA_DMG_PROC3,    handler_Fumituke_DamegeProc   }, // ダメージ最終チェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_HeavyBomber( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );

    int atk_weight = flowWk->Hnd_GetWeight( pokeID );
    int tgt_weight = flowWk->Hnd_GetWeight( target->GetID() );

    int weight_ratio = atk_weight / tgt_weight;
    u32 pow;

    // 相手より重いほど威力が高い
    if( weight_ratio >= 5 ){
      pow = 120;
    }else if( weight_ratio == 4 ){
      pow = 100;
    }else if( weight_ratio == 3 ){
      pow = 80;
    }else if( weight_ratio == 2 ){
      pow = 60;
    }else{
      pow = 40;
    }

    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * ヒートスタンプ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_HeatStamp( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER_BASE,   handler_HeavyBomber     }, // ワザ威力チェックは「ヘビーボンバー」と共通
    { BTL_EVENT_CALC_HIT_CANCEL,   handler_Fumituke_HitCheckSkip }, // 命中計算スキップについて「ふみつけ」と共通
    { BTL_EVENT_WAZA_DMG_PROC3,    handler_Fumituke_DamegeProc   }, // ダメージ最終チェックについて「ふみつけ」と共通
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}


//----------------------------------------------------------------------------------
/**
 * エレクトボール
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_ElectBall( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER_BASE,   handler_ElectBall },    // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_ElectBall( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );

    int  atkSpeed = flowWk->Hnd_CalcAgility( bpp, false );
    int  tgtSpeed = flowWk->Hnd_CalcAgility( target, false );
    int  quot = atkSpeed / tgtSpeed;
    u32 pow;

    // 相手より素早いほど威力が高い
    if( quot >= 4 ){
      pow = 150;
    }else if( quot >= 3 ){
      pow = 120;
    }else if( quot >= 2 ){
      pow = 80;
    }else if( quot >= 1 ){
      pow = 60;
    }else{
      pow = 40;
    }

    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * エコーボイス
 *
 * 毎ターン、場にいる誰かが使うたびにカウンタがインクリメントされ、
 * その都度、ワザの威力が上がっていく。（誰も使わないターンがあればカウンタは０に戻る）
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_EchoVoice( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER_BASE,   handler_EchoVoice },    // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_EchoVoice( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const WazaRec* rec = flowWk->Hnd_GetWazaRecord( );
    WazaNo  wazaID = static_cast<WazaNo>(EVENT_FACTOR_GetSubID( myHandle ));
    int turn_cnt = flowWk->Hnd_GetTurnCount() - 1;
    int cont_cnt = 0;

    BTL_PRINT("[ECHOVOICE] pokeID=%d, turn_count = %d\n", pokeID, turn_cnt );

    while( turn_cnt >= 0 )
    {
      if( rec->IsUsedWaza(wazaID, turn_cnt--) ){
        ++cont_cnt;
        BTL_PRINT("  -> ターン[%d]で使われている cont_count=>%d\n", turn_cnt+1, cont_cnt);
      }
      else{
        break;
      }
    }

    {
      u32 pow;
      switch( cont_cnt ){
      case 0:   pow =  40; break;
      case 1:   pow =  80; break;
      case 2:   pow = 120; break;
      case 3:   pow = 160; break;
      default:  pow = 200; break;
      }
      BTL_PRINT("  -> cont_count=%d, pow=%d\n", cont_cnt, pow);
      EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * かたきうち
 *
 * 前のターンに味方がひん死になっていたら威力が倍になる
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Katakiuti( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_Katakiuti },    // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Katakiuti( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const DeadRec* rec = flowWk->Hnd_GetDeadRecord( );
    u8 deadPokeCnt, deadPokeID, i;

    // 前のターンに味方が死んでいたら威力を倍
    deadPokeCnt = rec->GetCount( 1 );
    BTL_Printf("前ターンに死んだポケ数=%d\n", deadPokeCnt);
    for(i=0; i<deadPokeCnt; ++i)
    {
      deadPokeID = rec->GetPokeID( 1, i );
      BTL_Printf("前ターンに死んだポケID=%d\n", deadPokeID);
      if( flowWk->Hnd_GetMainModule()->IsFriendPokeID( pokeID, deadPokeID ) )
      {
        EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
        break;
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * イカサマ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Ikasama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER_PREV,   handler_Ikasama },    // ワザ威力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Ikasama( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 相手の攻撃力で計算させる
    u8 def_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    EVENTVAR_RewriteValue( BTL_EVAR_SWAP_POKEID, def_pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * みずびたし
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Mizubitasi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Mizubitasi },    // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Mizubitasi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u32 targetCnt = EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
    BTL_HANDEX_PARAM_CHANGE_TYPE* param;
    u32 i;
    for(i=0; i<targetCnt; ++i)
    {
      param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_TYPE *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_TYPE, pokeID ));
        param->next_type = PokeTypePair_MakePure( POKETYPE_MIZU );
        param->exTypeCause = BTL_POKEPARAM::EXTYPE_CAUSE_NONE;
        param->pokeID = EVENTVAR_GetValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1+i) );
        param->fPutFailMsg = true;
      flowWk->HANDEX_Pop( param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * シンプルビーム
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_SimpleBeem( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_SimpleBeem   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_SimpleBeem( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u32 targetCnt = EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
    BTL_HANDEX_PARAM_CHANGE_TOKUSEI*  param;
    const BTL_POKEPARAM* target;
    u32 i, targetPokeID;

    // 対象ポケモンのとくせいを「たんじゅん」に書き換え
    for(i=0; i<targetCnt; ++i)
    {
      targetPokeID = EVENTVAR_GetValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1 + i) );
      target = flowWk->Hnd_GetPokeParam( targetPokeID );
      if( target->GetValue( BTL_POKEPARAM::BPP_TOKUSEI) != TOKUSEI_NAMAKE )
      {
        param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_TOKUSEI *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_TOKUSEI, pokeID ));
          param->pokeID = EVENTVAR_GetValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1 + i) );
          param->tokuseiID = TOKUSEI_TANZYUN;
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_TokuseiChange );
          HANDEX_STR_AddArg( &param->exStr, param->pokeID );
          HANDEX_STR_AddArg( &param->exStr, param->tokuseiID );
        flowWk->HANDEX_Pop( param );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * なかまづくり
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_NakamaDukuri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_NakamaDukuri   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_NakamaDukuri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    TokuseiNo myTokusei = static_cast<TokuseiNo>( bpp->GetValue( BTL_POKEPARAM::BPP_TOKUSEI ) );

    if( !tables::CheckNakamaDukuriFailTokusei(myTokusei) )
    {
      u32 targetCnt = EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
      const BTL_POKEPARAM* target;
      u32 i;
      u8  targetPokeID;

      // 対象ポケモンのとくせいを自分と同じものに書き換え
      for(i=0; i<targetCnt; ++i)
      {
        targetPokeID = EVENTVAR_GetValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1 + i) );
        target = flowWk->Hnd_GetPokeParam( targetPokeID );
        if( target->GetValue( BTL_POKEPARAM::BPP_TOKUSEI) != TOKUSEI_NAMAKE )
        {
          BTL_HANDEX_PARAM_CHANGE_TOKUSEI*  param;
          param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_TOKUSEI *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_TOKUSEI, pokeID ));
            param->pokeID = EVENTVAR_GetValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1 + i) );
            param->tokuseiID = myTokusei;
            HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_TokuseiChange );
            HANDEX_STR_AddArg( &param->exStr, param->pokeID );
            HANDEX_STR_AddArg( &param->exStr, param->tokuseiID );
          flowWk->HANDEX_Pop( param );
        }
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * クリアスモッグ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_ClearSmog( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,  handler_ClearSmog   },         // ダメージ直後
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_ClearSmog( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == false)
  ){
    BTL_HANDEX_PARAM_RESET_RANK* reset_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );

    reset_param = reinterpret_cast<BTL_HANDEX_PARAM_RESET_RANK *>( flowWk->HANDEX_Push( BTL_HANDEX_RESET_RANK, pokeID ));
      reset_param->poke_cnt = 1;
      reset_param->pokeID[0] = targetPokeID;
    flowWk->HANDEX_Pop( reset_param );

    msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_RankReset );
      HANDEX_STR_AddArg( &msg_param->str, targetPokeID );
    flowWk->HANDEX_Pop( msg_param );
  }
}
//----------------------------------------------------------------------------------
/**
 * やきつくす
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Yakitukusu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION_L2,  handler_Yakitukusu   },         // ダメージ直後
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Yakitukusu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == false)
  ){
    u8 defPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( defPokeID );
    u16 itemID = target->GetItem();
    if( ::item::ITEM_CheckNuts(itemID)
    ||  ::item::ITEM_CheckJewel(itemID)
    ){
      BTL_HANDEX_PARAM_SET_ITEM* param = reinterpret_cast<BTL_HANDEX_PARAM_SET_ITEM *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_ITEM, pokeID ));
        param->pokeID = defPokeID;
        param->itemID = ITEM_DUMMY_DATA;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Yakitukusu );
        HANDEX_STR_AddArg( &param->exStr, defPokeID );
        HANDEX_STR_AddArg( &param->exStr, itemID );
      flowWk->HANDEX_Pop( param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ともえなげ・ドラゴンテール
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_TomoeNage( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL,  handler_TomoeNage   },         // ダメージ直後
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_TomoeNage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_PUSHOUT* param = reinterpret_cast<BTL_HANDEX_PARAM_PUSHOUT *>( flowWk->HANDEX_Push( BTL_HANDEX_PUSHOUT, pokeID ));

      param->pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
      param->effectNo = (EVENT_FACTOR_GetSubID(myHandle) == WAZANO_TOMOENAGE)?
                            BTLEFF_TOMOENAGE_RETURN : BTLEFF_DRAGONTAIL_RETURN;

    flowWk->HANDEX_Pop( param );
  }
}

//----------------------------------------------------------------------------------
/**
 * ほえる
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Hoeru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_PUSHOUT_EFFECT_NO,      handler_Hoeru },  // 押し出しワザエフェクトチェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 押し出しワザエフェクトチェックハンドラ
void handler_Hoeru( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u16 effectNo = (EVENT_FACTOR_GetSubID(myHandle) == WAZANO_HOERU)?
                            BTLEFF_HOERU_RETURN : BTLEFF_HUKITOBASI_RETURN;

    EVENTVAR_RewriteValue( BTL_EVAR_EFFECT_NO, effectNo );
  }
}


//----------------------------------------------------------------------------------
/**
 * うちおとす
 */
//----------------------------------------------------------------------------------

const BtlEventHandlerTable*  ADD_Utiotosu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL, handler_Utiotosu               },  // ダメージ直後
    { BTL_EVENT_CHECK_POKE_HIDE,         handler_Kaminari_checkHide     },  // 消えポケヒットチェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージ直後ハンドラ
void handler_Utiotosu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    common_UtiotosuEffect( flowWk, pokeID, targetPokeID );
  }
}
// うちおとす共通処理（うちおとす・サウザンアロー）
// @retval  １体でも効果有り ... true
bool common_UtiotosuEffect( ServerFlow* flowWk, u8 atkPokeID, u8 targetPokeID )
{
  const BTL_POKEPARAM* bppTarget = flowWk->Hnd_GetPokeParam( targetPokeID );
  bool  bStdMsgPut = false;

  // なんにせよフリーフォール実行状態のポケモンには効かない
  if( !flowWk->Hnd_IsFreeFallPoke(targetPokeID) )
  {
    // ふゆう状態でフリーフォールされてる状態じゃないポケ対象
    if( (flowWk->Hnd_IsFloatingPoke( targetPokeID))
    &&  (!bppTarget->CheckSick(pml::wazadata::WAZASICK_FREEFALL))
    ){
      // うちおとす状態にする
      BTL_HANDEX_PARAM_ADD_SICK* param;

      param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>(flowWk->HANDEX_Push(BTL_HANDEX_ADD_SICK, atkPokeID));
        param->pokeID = targetPokeID;
        param->sickID = pml::wazadata::WAZASICK_FLYING_CANCEL;
        param->sickCont = SICKCONT_MakePermanent( atkPokeID );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Utiotosu );
        HANDEX_STR_AddArg( &param->exStr, targetPokeID );
      flowWk->HANDEX_Pop( param );
      bStdMsgPut = true;

      // でんじふゆう状態を消す
      if( bppTarget->CheckSick(pml::wazadata::WAZASICK_FLYING) )
      {
        BTL_HANDEX_PARAM_CURE_SICK* cure_param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>(flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, atkPokeID ));
          cure_param->poke_cnt = 1;
          cure_param->pokeID[0] = targetPokeID;
          cure_param->fStdMsgDisable = true;
          cure_param->sickCode = static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_FLYING);
        flowWk->HANDEX_Pop( cure_param );
      }

      // テレキネシス状態を消す
      if( bppTarget->CheckSick( pml::wazadata::WAZASICK_TELEKINESIS) )
      {
        BTL_HANDEX_PARAM_CURE_SICK* cure_param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>(flowWk->HANDEX_Push(BTL_HANDEX_CURE_SICK, atkPokeID));
          cure_param->poke_cnt = 1;
          cure_param->pokeID[0] = targetPokeID;
          cure_param->fStdMsgDisable = true;
          cure_param->sickCode = static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_TELEKINESIS);
        flowWk->HANDEX_Pop( cure_param );
      }
    }

    // そらを飛んでたらキャンセルさせる
    if( bppTarget->CONTFLAG_Get(BTL_POKEPARAM::CONTFLG_SORAWOTOBU) )
    {
      BTL_HANDEX_PARAM_TAMEHIDE_CANCEL* cancel_param = reinterpret_cast<BTL_HANDEX_PARAM_TAMEHIDE_CANCEL *>(flowWk->HANDEX_Push( BTL_HANDEX_TAMEHIDE_CANCEL, atkPokeID));
        cancel_param->pokeID = targetPokeID;
        cancel_param->flag = BTL_POKEPARAM::CONTFLG_SORAWOTOBU;
        if( !bStdMsgPut ){
          HANDEX_STR_Setup( &cancel_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Utiotosu );
          HANDEX_STR_AddArg( &cancel_param->exStr, targetPokeID );
        }
      flowWk->HANDEX_Pop( cancel_param );
    }
  }

  return bStdMsgPut;
}

//----------------------------------------------------------------------------------
/**
 * からをやぶる
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_KarawoYaburu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_KarawoYaburu },   // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 未分類ワザハンドラ
void handler_KarawoYaburu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
      param->rankType = pml::wazadata::RANKEFF_DEFENCE;
      param->rankVolume = -1;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->fAlmost = true;
      param->fWazaEffect = true;
    flowWk->HANDEX_Pop( param );

    param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
      param->rankType = pml::wazadata::RANKEFF_SP_DEFENCE;
      param->rankVolume = -1;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->fAlmost = true;
      param->fWazaEffect = true;
    flowWk->HANDEX_Pop( param );

    param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
      param->rankType = pml::wazadata::RANKEFF_ATTACK;
      param->rankVolume = 2;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->fAlmost = true;
      param->fWazaEffect = true;
    flowWk->HANDEX_Pop( param );

    param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
      param->rankType = pml::wazadata::RANKEFF_SP_ATTACK;
      param->rankVolume = 2;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->fAlmost = true;
      param->fWazaEffect = true;
    flowWk->HANDEX_Pop( param );

    param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
      param->rankType = pml::wazadata::RANKEFF_AGILITY;
      param->rankVolume = 2;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->fAlmost = true;
      param->fWazaEffect = true;
    flowWk->HANDEX_Pop( param );
  }
}
//----------------------------------------------------------------------------------
/**
 * ミラータイプ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_MirrorType( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_MirrorType },   // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_MirrorType( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8                   targetPokeID   = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target         = flowWk->Hnd_GetPokeParam( targetPokeID );
    const PokeTypePair   targetTypePair = target->GetPokeType();

    if( PokeTypePair_IsAnyTypeExist( targetTypePair ) ) // 相手がタイプ「なし」なら失敗
    {
      {
        BTL_HANDEX_PARAM_CHANGE_TYPE* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_TYPE *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_TYPE, pokeID ));
          param->pokeID = pokeID;
          param->next_type = target->GetPokeType();
          param->exTypeCause = target->GetExTypeCause();
          param->fStdMsgDisable = true;
        flowWk->HANDEX_Pop( param );
      }

      {
        BTL_HANDEX_PARAM_MESSAGE* msg_p = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
          msg_p->header.failSkipFlag = true;
          HANDEX_STR_Setup( &msg_p->str, BTL_STRTYPE_SET, BTL_STRID_SET_MirrorType );
          HANDEX_STR_AddArg( &msg_p->str, pokeID );
          HANDEX_STR_AddArg( &msg_p->str, targetPokeID );
        flowWk->HANDEX_Pop( msg_p );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ボディパージ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_BodyPurge( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_RANKEFF_FIXED,  handler_BodyPurge },   // ワザによるランク増減効果確定後
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_BodyPurge( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  enum {
    PURGE_WEIGHT = 1000,    // 0.1kg単位
  };

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u32 weight;

  // SVFTOOL を介すと「かるいし」などの効果を受けてしまうのでダメ。元体重をチェックする。
//    weight = flowWk->Hnd_GetWeight( pokeID );
    weight = bpp->GetWeight();

    if( weight > BTL_POKE_WEIGHT_MIN )
    {
      BTL_HANDEX_PARAM_SET_WEIGHT* param = reinterpret_cast<BTL_HANDEX_PARAM_SET_WEIGHT *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_WEIGHT, pokeID ));

      BTL_PRINT("ボディパージによる体重減少 %d -> ", weight);

        if( weight > (BTL_POKE_WEIGHT_MIN + PURGE_WEIGHT) ){
          weight -= PURGE_WEIGHT;
        }else{
          weight = BTL_POKE_WEIGHT_MIN;
        }

        param->pokeID = pokeID;
        param->weight = weight;

      BTL_PRINT("%d\n", weight);

        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_BodyPurge );
        HANDEX_STR_AddArg( &param->exStr, pokeID );

      flowWk->HANDEX_Pop( param );


      // 継続フラグをセット
      BTL_HANDEX_PARAM_SET_CONTFLAG* setContFlag = reinterpret_cast<BTL_HANDEX_PARAM_SET_CONTFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_CONTFLAG, pokeID ) );
        setContFlag->pokeID = pokeID;
        setContFlag->flag   = BTL_POKEPARAM::CONTFLG_BODYPURGE;
      flowWk->HANDEX_Pop( setContFlag );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * サイコショック
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_PsycoShock( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DEFENDER_GUARD_PREV,  handler_PsycoShock },   // 防御側能力値計算
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_PsycoShock( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_VID) == BTL_POKEPARAM::BPP_SP_DEFENCE )
    {
      int swap_cnt = EVENTVAR_GetValue( BTL_EVAR_VID_SWAP_CNT );
      ++swap_cnt;
      EVENTVAR_RewriteValue( BTL_EVAR_VID_SWAP_CNT, swap_cnt );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * なしくずし・せいなるつるぎ・ＤＤラリアット
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_NasiKuzusi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DEFENDER_GUARD_PREV,  handler_NasiKuzusi_CalcDmg  },  // 防御側能力値計算
    { BTL_EVENT_WAZA_HIT_RANK,        handler_NasiKuzusi_HitCheck }   // 回避チェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 防御側能力値計算
void handler_NasiKuzusi_CalcDmg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
  }
}
// 回避チェック
void handler_NasiKuzusi_HitCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_FLAT_FLAG, true );
  }
}

//----------------------------------------------------------------------------------
/**
 * ワンダールーム
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_WonderRoom( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL, handler_WonderRoom },    // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_WonderRoom( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( !flowWk->Hnd_CheckFieldEffect(FieldStatus::EFF_WONDERROOM) )
    {
      BTL_HANDEX_PARAM_ADD_FLDEFF* param;

      param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_FLDEFF *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_FLDEFF, pokeID ));
        param->effect = FieldStatus::EFF_WONDERROOM;
        param->cont = SICKCONT_MakeTurn( pokeID, 5 );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, BTL_STRID_STD_WonderRoom_Start );
      flowWk->HANDEX_Pop( param );
    }
    else
    {
      BTL_HANDEX_PARAM_REMOVE_FLDEFF* param;
        param = reinterpret_cast<BTL_HANDEX_PARAM_REMOVE_FLDEFF *>( flowWk->HANDEX_Push( BTL_HANDEX_REMOVE_FLDEFF, pokeID ));
        param->effect = FieldStatus::EFF_WONDERROOM;
      flowWk->HANDEX_Pop( param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * マジックルーム
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_MagicRoom( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL, handler_MagicRoom },    // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_MagicRoom( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( !flowWk->Hnd_CheckFieldEffect(FieldStatus::EFF_MAGICROOM) )
    {
      BTL_HANDEX_PARAM_ADD_FLDEFF* param;

      param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_FLDEFF *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_FLDEFF, pokeID ));
        param->effect = FieldStatus::EFF_MAGICROOM;
        param->cont = SICKCONT_MakeTurn( pokeID, 5 );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, BTL_STRID_STD_MagicRoom_Start );
      flowWk->HANDEX_Pop( param );
    }
    else
    {
      BTL_HANDEX_PARAM_REMOVE_FLDEFF* param;
        param = reinterpret_cast<BTL_HANDEX_PARAM_REMOVE_FLDEFF *>( flowWk->HANDEX_Push( BTL_HANDEX_REMOVE_FLDEFF, pokeID ));
        param->effect = FieldStatus::EFF_MAGICROOM;
      flowWk->HANDEX_Pop( param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * はじけるほのお
 */
//----------------------------------------------------------------------------------
#define HAJIKERU_HONOO_0622   (1)
const BtlEventHandlerTable*  ADD_HajikeruHonoo( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
#if   HAJIKERU_HONOO_0622
    { BTL_EVENT_WAZA_DMG_REACTION,    handler_HajikeruHonoo },    // ダメージ処理最終ハンドラ
#else
//    こちらだと「レッドカード」や「ジャポのみ」に処理順で負ける
    { BTL_EVENT_DAMAGEPROC_END_HIT_L2,   handler_HajikeruHonoo },    // ダメージ処理最終ハンドラ
#endif
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_HajikeruHonoo( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
#if   HAJIKERU_HONOO_0622
    u8 damagedPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
#else
    u8 damagedPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
#endif

    BtlPokePos pos = flowWk->Hnd_GetPokeLastPos( damagedPokeID );

    if( pos != BTL_POS_NULL )
    {
      BtlExPos exPos = hajikeruHonoo_GetAdditionalDamagePos( flowWk, pokeID, damagedPokeID );
      u8  targetPokeID[ BTL_POSIDX_MAX ];
      u8  targetCnt;
      u8  i;

      targetCnt = flowWk->Hnd_ExpandPokeID( exPos, targetPokeID );
      for(i=0; i<targetCnt; ++i)
      {
        if( targetPokeID[i] != damagedPokeID )
        {
          const BTL_POKEPARAM* bpp;
          BTL_HANDEX_PARAM_DAMAGE* param;

          bpp = flowWk->Hnd_GetPokeParam( targetPokeID[i] );
          param = reinterpret_cast<BTL_HANDEX_PARAM_DAMAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_DAMAGE, pokeID ));
            param->pokeID = targetPokeID[ i ];
            param->damage = calc::QuotMaxHP( bpp, 16 );
            param->damageCause = DAMAGE_CAUSE_HAJIKERUHONOO_SPARKLE;
            param->damageCausePokeID = pokeID;
            param->fAvoidHidePoke = true;
            HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_HajikeruHonoo_Side );
            HANDEX_STR_AddArg( &param->exStr, param->pokeID );
          flowWk->HANDEX_Pop( param );
        }
      }
    }
  }
}

/**
 * @brief 「はじけるほのお」の追加ダメージの対象となる拡張位置を取得する
 * @param attackPokeID  「はじけるほのお」を撃ったポケモンのID
 * @param damagePokeID  「はじけるほのお」を受けたポケモンのID
 */
BtlExPos hajikeruHonoo_GetAdditionalDamagePos( ServerFlow* flowWk, u8 attackPokeID, u8 damagePokeID )
{
  BtlRule rule = flowWk->Hnd_GetRule();

  if( rule == BTL_RULE_ROYAL )
  {
    BtlPokePos attackPos = flowWk->Hnd_GetPokeLastPos( attackPokeID );
    return EXPOS_MAKE( BTL_EXPOS_FULL_ENEMY, attackPos );
  }
  else
  {
    BtlPokePos damagePos = flowWk->Hnd_GetPokeLastPos( damagePokeID );
    return EXPOS_MAKE( BTL_EXPOS_NEXT_FRIENDS, damagePos );
  }
}

//----------------------------------------------------------------------------------
/**
 * シンクロノイズ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_SyncroNoise( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2, handler_SyncroNoise },    // ワザ無効チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_SyncroNoise( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetPokeID );
    const BTL_POKEPARAM* user = flowWk->Hnd_GetPokeParam( pokeID );

    PokeTypePair myType, targetType;

    myType = user->GetPokeType( );
    targetType = target->GetPokeType( );

    if( !PokeTypePair_IsMatchEither(myType, targetType) )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, true );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ギフトパス
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_GiftPass( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_GiftPass },   // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_GiftPass( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetPokeID );
    const BTL_POKEPARAM* user = flowWk->Hnd_GetPokeParam( pokeID );
    u16 myItemID = user->GetItem( );
    if( (myItemID != ITEM_DUMMY_DATA)
    &&  (!calc::ITEM_IsMail(myItemID))
    &&  (target->GetItem() == ITEM_DUMMY_DATA)
    &&  (!HandCommon_CheckCantChangeItemPoke(flowWk, pokeID))
    &&  (!HandCommon_CheckCantChangeItemPoke(flowWk, targetPokeID))
    &&  (!HandCommon_IsUnbreakablePokeItem(flowWk, target->GetMonsNo(), myItemID))
    ){
      BTL_HANDEX_PARAM_SET_ITEM* param;

      param = reinterpret_cast<BTL_HANDEX_PARAM_SET_ITEM *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_ITEM, pokeID ));
        param->pokeID = pokeID;
        param->itemID = ITEM_DUMMY_DATA;
      flowWk->HANDEX_Pop( param );

      param = reinterpret_cast<BTL_HANDEX_PARAM_SET_ITEM *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_ITEM, pokeID ));
        param->pokeID = targetPokeID;
        param->itemID = myItemID;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_GiftPass );
        HANDEX_STR_AddArg( &param->exStr, targetPokeID );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
        HANDEX_STR_AddArg( &param->exStr, myItemID );
      flowWk->HANDEX_Pop( param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * いのちがけ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Inotigake( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC1,        handler_Inotigake_CalcDamage  },  // ダメージ計算
    { BTL_EVENT_CHECK_ATTACKER_DEAD,   handler_Inotigake_CheckDead   },  // 攻撃側ひん死チェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Inotigake_CalcDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u32 hp = bpp->GetValue( BTL_POKEPARAM::BPP_HP );
    EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE, hp );
    EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE_FLAG, true );
  }
}
// 攻撃側ひん死チェック
void handler_Inotigake_CheckDead( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( EVENTVAR_RewriteValue(BTL_EVAR_GEN_FLAG, true) ){
      BTL_HANDEX_PARAM_KILL* param = reinterpret_cast<BTL_HANDEX_PARAM_KILL *>( flowWk->HANDEX_Push( BTL_HANDEX_KILL, pokeID ));
        param->pokeID = pokeID;
        param->deadCause = DAMAGE_CAUSE_INOTIGAKE;
      flowWk->HANDEX_Pop( param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * おさきにどうぞ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_OsakiniDouzo( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_OsakiniDouzo  },     // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_OsakiniDouzo( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_INTR_POKE* param = reinterpret_cast<BTL_HANDEX_PARAM_INTR_POKE *>( flowWk->HANDEX_Push( BTL_HANDEX_INTR_POKE, pokeID ));
      param->pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_OsakiniDouzo );
      HANDEX_STR_AddArg( &param->exStr, param->pokeID );
    flowWk->HANDEX_Pop( param );
  }
}
//----------------------------------------------------------------------------------
/**
 * さきおくり
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Sakiokuri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Sakiokuri  },     // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Sakiokuri( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SEND_LAST* param = reinterpret_cast<BTL_HANDEX_PARAM_SEND_LAST *>( flowWk->HANDEX_Push( BTL_HANDEX_SEND_LAST, pokeID ));
      param->pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Sakiokuri );
      HANDEX_STR_AddArg( &param->exStr, param->pokeID );
    flowWk->HANDEX_Pop( param );
  }
}
//----------------------------------------------------------------------------------
/**
 * りんしょう
 *
 * 同一ターンにこのワザを出したポケモンの行動は、全て連続して処理される。
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Rinsyou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXE_START,    handler_Rinsyou  },     // ワザだし確定
    { BTL_EVENT_WAZA_POWER_BASE,   handler_Rinsyou_Pow },  // 威力計算
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Rinsyou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  // このターンにまだ誰も同じワザを使っていなければ、同じワザを使う予定のポケを繰り上げる
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const WazaRec* rec = flowWk->Hnd_GetWazaRecord( );
    WazaNo  wazaID = static_cast<WazaNo>(EVENT_FACTOR_GetSubID( myHandle ));
    u32 thisTurn = flowWk->Hnd_GetTurnCount( );
    if( rec->GetUsedWazaCount(wazaID, thisTurn) == 1 )
    {
      BTL_HANDEX_PARAM_INTR_WAZA* param = reinterpret_cast<BTL_HANDEX_PARAM_INTR_WAZA *>( flowWk->HANDEX_Push( BTL_HANDEX_INTR_WAZA, pokeID ));
        param->waza = wazaID;
      flowWk->HANDEX_Pop( param );
    }
  }
}
void handler_Rinsyou_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  // このターンに同じワザを使っていれば威力が倍
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const WazaRec* rec = flowWk->Hnd_GetWazaRecord( );
    WazaNo  wazaID = static_cast<WazaNo>(EVENT_FACTOR_GetSubID( myHandle ));
    u32 thisTurn = flowWk->Hnd_GetTurnCount( );
    // 既に自分のワザがレコードされているので１件を越えてたら２番目以降と判定
    if( rec->GetUsedWazaCount(wazaID, thisTurn) > 1)
    {
      HandCommon_MulWazaBasePower( 2 );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ファストガード
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_FastGuard( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD,      handler_WideGuard_ExeCheck  },  // ワザ出し成否チェックハンドラ
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET, handler_FastGuard           },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_FastGuard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  BTL_SICKCONT  cont = SICKCONT_MakeTurn( pokeID, 1 );
  BtlSide side = flowWk->Hnd_GetMainModule()->PokeIDtoSide( pokeID );
  common_SideEffectStdMsg( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_FASTGUARD, cont, BTL_STRID_STD_FastGuard );
  IncrementMamoruCounter( flowWk, pokeID, true );
}
//----------------------------------------------------------------------------------
/**
 * サイドチェンジ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_SideChange( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_SideChange   },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_SideChange( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  BtlPokePos myPos = flowWk->Hnd_PokeIDtoPokePos( pokeID );
  u8 target_pokeID = pokeID;
  BtlPokePos targetPos;

  switch( flowWk->Hnd_GetRule() ){
  case BTL_RULE_DOUBLE:
    {
      u8 myClientID = MainModule::PokeIDtoClientID( pokeID );
      if( flowWk->Hnd_GetFriendClientID( myClientID) == BTL_CLIENTID_NULL )
      {
        targetPos = MainModule::GetFriendPokePos( BTL_RULE_DOUBLE, myPos, 0 );
        if( targetPos == myPos ){
          targetPos = MainModule::GetFriendPokePos( BTL_RULE_DOUBLE, myPos, 1 );
        }
        target_pokeID = flowWk->Hnd_PokePosToPokeID( targetPos );
      }
    }
    break;
  }

  if( (target_pokeID != pokeID) )
  {
    BTL_HANDEX_PARAM_SWAP_POKE* param = reinterpret_cast<BTL_HANDEX_PARAM_SWAP_POKE *>( flowWk->HANDEX_Push( BTL_HANDEX_SWAP_POKE, pokeID ));
      param->pokeID1 = pokeID;
      param->pokeID2 = target_pokeID;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_SideChange );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, target_pokeID );
    flowWk->HANDEX_Pop( param );
  }
}
//----------------------------------------------------------------------------------
/**
 * テレキネシス
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Telekinesis( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD, handler_Telekinesis_CheckFail  },  // ワザ出し成否チェックハンドラ
    { BTL_EVENT_NOEFFECT_CHECK_TOKUSEI, handler_Telekinesis_NoEffCheck },  // 無効チェック
    { BTL_EVENT_WAZASICK_SPECIAL,       handler_Telekinesis            },  // 特殊状態異常
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザ出し成否チェックハンドラ
void handler_Telekinesis_CheckFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // 場にじゅうりょく効果が存在するなら失敗
    if( flowWk->Hnd_CheckFieldEffect( FieldStatus::EFF_JURYOKU ) )
    {
      EVENTVAR_RewriteValue(BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_JURYOKU);
    }
  }
}
// 無効チェック
void handler_Telekinesis_NoEffCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 defPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* bppDef = flowWk->Hnd_GetPokeParam( defPokeID );
    u16 defMonsNo = bppDef->GetMonsNo( );

    if( (defMonsNo == MONSNO_DHIGUDA)
    ||  (defMonsNo == MONSNO_DAGUTORIO)
    ||  (defMonsNo == MONSNO_SUNABA1)
    ||  (defMonsNo == MONSNO_SUNABA2)
    ||  ((defMonsNo == MONSNO_GENGAA) && bppDef->IsMegaEvolved())
    ||  (bppDef->CheckSick(pml::wazadata::WAZASICK_FLYING_CANCEL))
    ||  (bppDef->CheckSick(pml::wazadata::WAZASICK_NEWOHARU))
    ){
      EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, true );
    }
  }
}
// 特殊状態異常
void handler_Telekinesis( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_STR_PARAMS* str = (BTL_HANDEX_STR_PARAMS*)EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );
    EVENTVAR_RewriteValue( BTL_EVAR_SICKID, pml::wazadata::WAZASICK_TELEKINESIS );
    HANDEX_STR_Setup( str, BTL_STRTYPE_SET, BTL_STRID_SET_Telekinesis );
    HANDEX_STR_AddArg( str, EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
  }
}
//----------------------------------------------------------------------------------
/**
 * フリーフォール
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_FreeFall( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM,             handler_FreeFall_WazaParam   },   // ワザタイプ決定ハンドラ
    { BTL_EVENT_CHECK_TAMETURN_FAIL,    handler_FreeFall_TameFail    },   // 溜め失敗判定
    { BTL_EVENT_TAME_START,             handler_FreeFall_TameStart   },   // 溜め開始
    { BTL_EVENT_TAME_RELEASE,           handler_FreeFall_TameRelease },   // 溜め解放
    { BTL_EVENT_TEMPT_TARGET,           handler_FreeFall_Target      },  // ターゲット決定
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD, handler_FreeFall_FailCheck   },
    { BTL_EVENT_NOEFFECT_CHECK_L2,      handler_FreeFall_TypeCheck   },
    { BTL_EVENT_DAMAGEPROC_END,         handler_FreeFall_DmgProcEnd  },
    { BTL_EVENT_TAMERELEASE_FAILED,     handler_FreeFall_DmgProcEnd  },
    { BTL_EVENT_WAZA_EXECUTE_NO_EFFECT, handler_FreeFall_NotEffect   },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザタイプ決定ハンドラ
// NMCat[4306] フリーフォールの連れ去り時にニードルガードのダメージで瀕死になると、ポケモンが行動不能になってしまう。
// 溜めターン( 相手を上空に連れ去るターン )では、非接触扱いにする
void handler_FreeFall_WazaParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) != pokeID )
  {
    return;
  }

  // 既にフリーフォール使用中なら、解放ターンなので何もしない
  if( flowWk->Hnd_IsFreeFallUserPoke(pokeID) )
  {
    return;
  }

  // 非接触に変更
  EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TOUCH_FLAG, false );
}
// 溜め失敗判定
void handler_FreeFall_TameFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    if( (targetPokeID != BTL_POKEID_NULL)
    &&  (flowWk->Hnd_GetMainModule()->IsFriendPokeID(pokeID, targetPokeID))
    ){
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
    }
  }
}
// 溜め開始
void handler_FreeFall_TameStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    static ServerFlow::WAZAPARAM  wazaParam;
    ServerFlow::WAZAPARAM* src = (ServerFlow::WAZAPARAM*)( EVENTVAR_GetValue(BTL_EVAR_WORK_ADRS) );
    gfl2::std::MemCopy( src, &wazaParam, sizeof(wazaParam) );

    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    bool fFailMsgDisped = false;

    if( (targetPokeID == BTL_POKEID_NULL)
    ||  (!flowWk->Hnd_FreeFallStart(pokeID, targetPokeID, &wazaParam, &fFailMsgDisped ))
    ){
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
      if( fFailMsgDisped ){
        EVENTVAR_RewriteValue( BTL_EVAR_MSG_FLAG, true );
      }
    }
    else
    {
      if( EVENTVAR_RewriteValue(BTL_EVAR_GEN_FLAG, true) )
      {
        BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
          HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_FreeFall );
          HANDEX_STR_AddArg( &param->str, pokeID );
          HANDEX_STR_AddArg( &param->str, targetPokeID );
        flowWk->HANDEX_Pop( param );
      }
    }
  }
}
// 溜め解放
void handler_FreeFall_TameRelease( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u8 targetPokeID = BTL_POKEPARAM::FreeFallCounterToPokeID( bpp->COUNTER_Get(BTL_POKEPARAM::COUNTER_FREEFALL) );
    if( (targetPokeID == BTL_POKEID_NULL)
    ||  (flowWk->Hnd_GetExistFrontPokePos(targetPokeID) == BTL_POS_NULL)
    ){
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
    }

    flowWk->Hnd_FreeFallRelease( pokeID, false, false );

    work[0] = 1;
    work[1] = targetPokeID;
  }
}
// ダメージプロセス終了
void handler_FreeFall_DmgProcEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  // 演出の都合上、ここで相手側の表示エフェクトを行う
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    freefall_common_recoverTarget( flowWk, work );
  }
}
// ワザ効果なし時ハンドラ
void handler_FreeFall_NotEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  // ワザ失敗した時はこちらで相手側の表示エフェクトを行う
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    freefall_common_recoverTarget( flowWk, work );
  }
}
// フリーフォール共通：つかんだ相手の表示を元に戻す
void freefall_common_recoverTarget( ServerFlow* flowWk, int* work )
{
  if( work[0] )
  {
    u8 targetPokeID = work[1];
    flowWk->Hnd_FreeFallAppearTarget( targetPokeID );
    work[0] = 0;
  }
}
// ターゲット決定
void handler_FreeFall_Target( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( flowWk->Hnd_GetRule() != BTL_RULE_SINGLE )
  {
    // 掴んでいるポケモンをターゲットにする
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u8 targetPokeID;

    targetPokeID = BTL_POKEPARAM::FreeFallCounterToPokeID( bpp->COUNTER_Get( BTL_POKEPARAM::COUNTER_FREEFALL) );
    if( targetPokeID != BTL_POKEID_NULL )
    {
      HandCommon_TemptTarget( targetPokeID, TEMPT_TARGET_PRIORITY_WAZA_1, TEMPT_TARGET_CAUSE_OTHER );
    }
  }
}

// ワザ出し失敗チェック
void handler_FreeFall_FailCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u8 capturedPokeID = BTL_POKEPARAM::FreeFallCounterToPokeID( bpp->COUNTER_Get( BTL_POKEPARAM::COUNTER_FREEFALL) );

    if( (capturedPokeID != BTL_POKEID_NULL)
    &&  (!flowWk->Hnd_IsExistPokemon( capturedPokeID))
    ){
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
    }
  }
}
void handler_FreeFall_TypeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 飛行タイプへは攻撃が当たらない
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( target->IsMatchType(POKETYPE_HIKOU) )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, true );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * いにしえのうた
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_InisieNoUta( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_L2,    handler_InisieNoUta   },   // ダメージ反応ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージ反応ハンドラ
void handler_InisieNoUta( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

  if( bpp->GetMonsNo() == MONSNO_CN_MERODHIA )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK)==pokeID )
    {
      BTL_HANDEX_PARAM_CHANGE_FORM* param;

      u8 nextForm = (bpp->GetFormNo() == FORMNO_CN_MERODHIA_VOICE)?
              FORMNO_CN_MERODHIA_STEP : FORMNO_CN_MERODHIA_VOICE;
      param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_FORM *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_FORM, pokeID ));
        param->pokeID = pokeID;
        param->formNo = nextForm;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_ChangeForm );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
      flowWk->HANDEX_Pop( param );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * みやぶる・かぎわける
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Miyaburu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASICK_PARAM,    handler_Miyaburu                },  // 状態異常パラメータ調整ハンドラ
//    { BTL_EVENT_MIGAWARI_EXCLUDE,  handler_Common_MigawariEffctive },  // みがわりチェック

  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 状態異常パラメータ調整ハンドラ
void handler_Miyaburu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_SICKCONT* cont = reinterpret_cast<BTL_SICKCONT*>( EVENTVAR_GetValue( BTL_EVAR_SICK_CONT_ADRS ) );
    SICKCONT_AddParam( cont, POKETYPE_GHOST );
  }
}
//----------------------------------------------------------------------------------
/**
 * ミラクルアイ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_MiracleEye( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASICK_PARAM, handler_MiracleEye },    // 状態異常パラメータ調整ハンドラ
//    { BTL_EVENT_MIGAWARI_EXCLUDE,  handler_Common_MigawariEffctive },  // みがわりチェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 状態異常パラメータ調整ハンドラ
void handler_MiracleEye( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_SICKCONT* cont = reinterpret_cast<BTL_SICKCONT*>( EVENTVAR_GetValue( BTL_EVAR_SICK_CONT_ADRS ) );
    SICKCONT_AddParam( cont, POKETYPE_AKU );
  }
}
//----------------------------------------------------------------------------------
/**
 * せいちょう
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Seityou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_GET_RANKEFF_VALUE, handler_Seityou },    // ランク増減効果調整ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ランク増減効果調整ハンドラ
void handler_Seityou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( calc::IsShineWeather(flowWk->Hnd_GetWeather()) )
    {
      int volume = EVENTVAR_GetValue( BTL_EVAR_VOLUME );
      if( volume == 1 ){
        ++volume;
      }
      EVENTVAR_RewriteValue( BTL_EVAR_VOLUME, volume );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * フリーズボルト・コールドフレア
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_FreezeBolt( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_FreezeBolt_TameStart },     // 溜め開始
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_FreezeBolt_TameStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u16 strID = (EVENT_FACTOR_GetSubID(myHandle) == WAZANO_HURIIZUBORUTO)?
      BTL_STRID_SET_FreezeBolt : BTL_STRID_SET_ColdFlare;

    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, strID );
      HANDEX_STR_AddArg( &param->str, pokeID );
    flowWk->HANDEX_Pop( param );
  }
}

//----------------------------------------------------------------------------------
/**
 * フレイムソウル・サンダーソウル
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_FlameSoul( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,          handler_FlameSoul_Pow       }, // 威力計算
  };

  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_FlameSoul_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  // このターン、直前に対になるワザを誰かが使っていたら威力が倍
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const WazaRec* rec = flowWk->Hnd_GetWazaRecord( );
    WazaNo  myWazaID = static_cast<WazaNo>(EVENT_FACTOR_GetSubID( myHandle ));
    WazaNo  combiWazaID;

    switch( myWazaID ){
    case WAZANO_KUROSUHUREIMU: combiWazaID = WAZANO_KUROSUSANDAA; break;
    case WAZANO_KUROSUSANDAA:  combiWazaID = WAZANO_KUROSUHUREIMU; break;
    default:
      combiWazaID = WAZANO_NULL; break;
    }

    if( combiWazaID != WAZANO_NULL )
    {
      u32 thisTurn = flowWk->Hnd_GetTurnCount( );

      if( rec->GetPrevEffectiveWaza(thisTurn) == combiWazaID )
      {
        EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
        flowWk->Hnd_SetWazaEffectIndex( BTLV_WAZAEFF_LINKWAZA_LINK );
      }
    }
  }
}

//-----------------------------------------------------------------------------------------------
// 合体ワザ関連ハンドラ＆サブルーチン
//-----------------------------------------------------------------------------------------------


CombiEffectType GetCombiWazaType( WazaID waza1, WazaID waza2 )
{
  static const struct {
    WazaID  waza1;
    WazaID  waza2;
    CombiEffectType  effect;
  }CombiTbl[] = {
    { WAZANO_MIZUNOTIKAI, WAZANO_HONOONOTIKAI,    COMBI_EFFECT_RAINBOW },
    { WAZANO_KUSANOTIKAI, WAZANO_HONOONOTIKAI,    COMBI_EFFECT_BURNING },
    { WAZANO_MIZUNOTIKAI, WAZANO_KUSANOTIKAI,     COMBI_EFFECT_MOOR    },
  };
  u32 i;

  for(i=0; i<GFL_NELEMS(CombiTbl); ++i)
  {
    if( ((CombiTbl[i].waza1 == waza1) && (CombiTbl[i].waza2 == waza2))
    ||  ((CombiTbl[i].waza1 == waza2) && (CombiTbl[i].waza2 == waza1))
    ){
      return CombiTbl[ i ].effect;
    }
  }
  return COMBI_EFFECT_NULL;
}
//----------------------------------------------------------------------------------
/**
 * みずのちかい・ほのおのちかい・くさのちかい
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_CombiWazaCommon( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_COMBIWAZA_CHECK,     handler_CombiWaza_CheckExe  }, // 合体ワザ発動チェック
    { BTL_EVENT_WAZA_EXE_DECIDE,     handler_CombiWaza_Decide    }, // ワザ出し確定
//    { BTL_EVENT_WAZA_PARAM,          handler_CombiWaza_WazaParam }, // ワザパラメータチェック
    { BTL_EVENT_TYPEMATCH_CHECK,       handler_CombiWaza_TypeMatch }, // タイプマッチチェック
    { BTL_EVENT_WAZA_POWER_BASE,       handler_CombiWaza_Pow       }, // 威力計算
    { BTL_EVENT_WAZA_EXE_START,        handler_CombiWaza_ChangeEff },
    { BTL_EVENT_DAMAGEPROC_END_HIT_L2, handler_CombiWaza_AfterDmg  }, // ダメージ処理終了後
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// 発動チェックハンドラ
void handler_CombiWaza_CheckExe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    WazaID  combiWazaID;
    u8 combiPokeID;

    if( bpp->CombiWaza_GetParam(&combiPokeID, &combiWazaID) )
    {
      CombiEffectType  effectType = GetCombiWazaType( static_cast<WazaNo>(EVENT_FACTOR_GetSubID(myHandle)), combiWazaID );
      u8 wazaType = EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
      BTL_N_Printf( DBGSTR_HANDWAZA_CombiWazaExe, pokeID, combiPokeID, combiWazaID, effectType );
      if( effectType != COMBI_EFFECT_NULL )
      {
        work[ 0 ] = 1;  // 合体発動フラグ
        work[ 1 ] = effectType;
        work[ 2 ] = combiPokeID;

        switch( effectType ){
        case COMBI_EFFECT_RAINBOW:   wazaType = POKETYPE_MIZU;  break;
        case COMBI_EFFECT_BURNING:   wazaType = POKETYPE_HONOO; break;
        case COMBI_EFFECT_MOOR:      wazaType = POKETYPE_KUSA;  break;
        }

        EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, wazaType );
      }
    }
  }
}
// ワザ出し確定ハンドラ
void handler_CombiWaza_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( work[0] )
    {
      BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_CombiWazaExe );
//      HANDEX_STR_AddArg( &param->str, work[2] );
      flowWk->HANDEX_Pop( param );
    }
  }
}
// タイプマッチチェックハンドラ
void handler_CombiWaza_TypeMatch( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  {
    if( work[0] )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
    }
  }
}
// ワザ威力計算ハンドラ
void handler_CombiWaza_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  // 合体発動していたら威力=150
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( work[0] )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, 150 );
    }
  }
}
//
void handler_CombiWaza_ChangeEff( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (work[0])
  ){
    WazaID  effWazaID = WAZANO_NULL;
    switch( work[1] ){
    case COMBI_EFFECT_RAINBOW:   effWazaID = WAZANO_MIZUNOTIKAI;  break;
    case COMBI_EFFECT_BURNING:   effWazaID = WAZANO_HONOONOTIKAI; break;
    case COMBI_EFFECT_MOOR:      effWazaID = WAZANO_KUSANOTIKAI;  break;
    }
    EVENTVAR_RewriteValue( BTL_EVAR_EFFECT_WAZAID, effWazaID );
  }
}
// ダメージ処理終了後ハンドラ
void handler_CombiWaza_AfterDmg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( work[0] )
    {
      BtlSideEffect eff  = BTL_SIDEEFF_NULL;
      BtlSide mySide     = flowWk->Hnd_GetMainModule()->PokeIDtoSide( pokeID );
      BtlSide targetSide = flowWk->Hnd_GetMainModule()->PokeIDtoSide( EVENTVAR_GetValue(BTL_EVAR_POKEID_TARGET1) );
      BtlSide side       = mySide;
      u32 viewEffectNo = 0;
      u16 strID = 0;
      u8  viewEffectTargetPokeID = BTL_POKEID_NULL;

      switch( work[1] ){
      case COMBI_EFFECT_RAINBOW:
        eff = BTL_SIDEEFF_RAINBOW;
        strID = BTL_STRID_STD_Rainbow;
        side = mySide;
        viewEffectNo = BTLEFF_RAINBOW;
        viewEffectTargetPokeID = pokeID;
        break;
      case COMBI_EFFECT_BURNING:
        eff = BTL_SIDEEFF_BURNING;
        strID = BTL_STRID_STD_Burning;
        side = targetSide;
        viewEffectNo = BTLEFF_BURNING;
        viewEffectTargetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
        break;
      case COMBI_EFFECT_MOOR:
        eff   = BTL_SIDEEFF_MOOR;
        strID = BTL_STRID_STD_Moor;
        side  = targetSide;
        viewEffectNo = BTLEFF_MOOR;
        viewEffectTargetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
        break;
      }

      if( eff != BTL_SIDEEFF_NULL )
      {
        BTL_HANDEX_PARAM_SIDEEFF_ADD* param = reinterpret_cast<BTL_HANDEX_PARAM_SIDEEFF_ADD *>( flowWk->HANDEX_Push( BTL_HANDEX_SIDEEFF_ADD, pokeID ));
          param->effect = eff;
          param->side = side;
          param->cont = SICKCONT_MakeTurn( pokeID, 4 );
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, strID );
          HANDEX_STR_AddArg( &param->exStr, side );
        flowWk->HANDEX_Pop( param );

        if( viewEffectTargetPokeID != BTL_POKEID_NULL )
        {
          BtlPokePos viewEffectPos = flowWk->Hnd_GetPokeLastPos( viewEffectTargetPokeID );
          if( viewEffectPos != BTL_POS_NULL )
          {
            BTL_HANDEX_PARAM_ADD_EFFECT*  viewEff_param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_EFFECT, pokeID ));
              viewEff_param->pos_from = viewEffectPos;
              viewEff_param->pos_to   = BTL_POS_NULL;
              viewEff_param->effectNo = viewEffectNo;
            flowWk->HANDEX_Pop( viewEff_param );
          }
        }
      }
    }
  }
}


//----------------------------------------------------------------------------------
/**
 * ハロウィン
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Halloween( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Halloween },          // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Halloween( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( target_pokeID );

    if( !target->IsMatchType(POKETYPE_GHOST) )
    {
      BTL_HANDEX_PARAM_EX_TYPE*    param = (BTL_HANDEX_PARAM_EX_TYPE*)( flowWk->HANDEX_Push( BTL_HANDEX_EX_TYPE, pokeID ) );
        param->ex_type = POKETYPE_GHOST;
        param->exTypeCause = BTL_POKEPARAM::EXTYPE_CAUSE_HALLOWEEN;
        param->pokeID  = target_pokeID;
      flowWk->HANDEX_Pop( param );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * もりののろい
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Morinonoroi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Morinonoroi },          // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Morinonoroi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( target_pokeID );

    if( !target->IsMatchType(POKETYPE_KUSA) )
    {
      BTL_HANDEX_PARAM_EX_TYPE*    param = (BTL_HANDEX_PARAM_EX_TYPE*)( flowWk->HANDEX_Push( BTL_HANDEX_EX_TYPE, pokeID ) );
        param->ex_type = POKETYPE_KUSA;
        param->exTypeCause = BTL_POKEPARAM::EXTYPE_CAUSE_MORINONOROI;
        param->pokeID  = target_pokeID;
      flowWk->HANDEX_Pop( param );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * おばけごっこ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_ObakeGokko( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_ObakeGokko },          // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_ObakeGokko( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( target_pokeID );

    if( (!target->IsMatchType(POKETYPE_GHOST))
    &&  (target->IsMatchType(POKETYPE_NORMAL))
    ){
      PokeTypePair  type_pair = target->GetPokeType();

      pml::PokeType  type1, type2, typeEx;
      PokeTypePair_Split( type_pair, &type1, &type2, &typeEx );

      if( type1 == POKETYPE_NORMAL ){ type1 = POKETYPE_GHOST; }
      if( type2 == POKETYPE_NORMAL ){ type2 = POKETYPE_GHOST; }
      if( typeEx == POKETYPE_NORMAL ){ typeEx = POKETYPE_GHOST; }

      BTL_HANDEX_PARAM_CHANGE_TYPE*    param = (BTL_HANDEX_PARAM_CHANGE_TYPE*)( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_TYPE, pokeID ) );
        param->next_type = PokeTypePair_Make( type1, type2, typeEx );
        param->pokeID  = target_pokeID;
      flowWk->HANDEX_Pop( param );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * きょうめい
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Kyoumei( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Kyoumei },          // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Kyoumei( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( target_pokeID );

    BTL_HANDEX_PARAM_SET_STATUS*    param = (BTL_HANDEX_PARAM_SET_STATUS*)( flowWk->HANDEX_Push( BTL_HANDEX_SET_STATUS, pokeID ) );
      param->pokeID  = pokeID;
      param->attack  = target->GetValue_Base(BTL_POKEPARAM::BPP_ATTACK);
      param->sp_attack  = target->GetValue_Base(BTL_POKEPARAM::BPP_SP_ATTACK);
      param->fAttackEnable = true;
      param->fSpAttackEnable = true;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Kyoumei );
      HANDEX_STR_AddArg( &param->exStr, target_pokeID );
    flowWk->HANDEX_Pop( param );
  }
}

//----------------------------------------------------------------------------------
/**
 * メンタルケア
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_MentalCare( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_MentalCare },          // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_MentalCare( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u32 targetCnt = EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
    const BTL_POKEPARAM* target;
    u32 i;
    u8  targetPokeID;

    // 対象ポケモンのメンタル系状態異常を治す
    for(i=0; i<targetCnt; ++i)
    {
      targetPokeID = EVENTVAR_GetValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1 + i) );
      target = flowWk->Hnd_GetPokeParam( targetPokeID );
      if( calc::CheckMentalSick(target) != pml::wazadata::WAZASICK_NULL )
      {
        BTL_HANDEX_PARAM_CURE_SICK* param;

        param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
          param->sickCode = WAZASICK_EX_MENTAL;
          param->poke_cnt = 1;
          param->pokeID[0] = targetPokeID;
        flowWk->HANDEX_Pop( param );
      }
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * たがやす
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Tagayasu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_GET_RANKEFF_VALUE,    handler_Tagayasu            }, // ランク増原効果決定ハンドラ
    { BTL_EVENT_CHECK_POKE_HIDE,      handler_Tagayasu_CheckHide  }, // 消えてるポケへの命中チェックハンドラ
    { BTL_EVENT_NOEFFECT_CHECK_L2,    handler_Tagayasu_CheckHit   }, // 攻撃無効化チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ランク増減効果決定ハンドラ
void handler_Tagayasu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  // このハンドラはGFNMcat[5688]への対処（handler_Tagayasu_CheckHit）により
  // 不要になっているはずだが、万が一の見落としが怖いので残しておく。  taya

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    if( common_isTagayasuNoEffect(flowWk, targetPokeID) )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_STATUS_TYPE, pml::wazadata::RANKEFF_NULL );
    }
  }
}
// 消えてるポケへの命中チェックハンドラ
void handler_Tagayasu_CheckHide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // あたらなかったメッセージを出さない
    EVENTVAR_RewriteValue( BTL_EVAR_MSG_FLAG, false );
  }
}
// 攻撃無効化チェックハンドラ
void handler_Tagayasu_CheckHit( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    if( common_isTagayasuNoEffect(flowWk, targetPokeID) )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, true );
    }
  }
}
// 「たがやす」効果の対象外となるポケモンか否かを判定
bool common_isTagayasuNoEffect( ServerFlow* flowWk, u8 pokeID )
{
  const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( pokeID );

  // 対象が接地していない or 草タイプでないなら無効
  if( flowWk->Hnd_IsFloatingPoke(pokeID)
  ||  !(target->IsMatchType(POKETYPE_KUSA))
  ){
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------
/**
 * フラワーガード
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_FlowerGuard( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_POKE_HIDE,       handler_Tagayasu_CheckHide  }, // 消えてるポケへの命中チェックハンドラ
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_FlowerGuard },          // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

void handler_FlowerGuard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    common_GuardUpByPokeType( flowWk, pokeID, POKETYPE_KUSA );
  }
}

// 特定タイプの対象ポケモンの防御を１段階アップさせる (@BTL_EVENT_UNCATEGORIZE_WAZA)
void common_GuardUpByPokeType( ServerFlow* flowWk, u8 pokeID, pml::PokeType pokeType )
{
  u32 targetCnt = EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
  const BTL_POKEPARAM* target;
  u8    targetPokeID;

  // 対象ポケモンが指定のタイプなら防御アップ
  for(u32 i=0; i<targetCnt; ++i)
  {
    targetPokeID = EVENTVAR_GetValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1 + i) );
    target = flowWk->Hnd_GetPokeParam( targetPokeID );
    if( target->IsMatchType(pokeType) )
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* param;

      param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
        param->pokeID[0] = targetPokeID;
        param->poke_cnt = 1;
        param->rankType = pml::wazadata::RANKEFF_DEFENCE;
        param->rankVolume = 1;
        param->bMigawariCheck = true;
        param->fAlmost = true;
        param->fWazaEffect = true;
      flowWk->HANDEX_Pop( param );
    }
  }
}


//----------------------------------------------------------------------------------
/**
 * いざなう
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Izanau( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Izanau },          // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Izanau( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( flowWk->Hnd_GetWeather() == BTL_WEATHER_SNOW )
    {
      u8 targetPokeID = EVENTVAR_GetValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1) );
      const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetPokeID );
      if( target->CheckNemuri( BTL_POKEPARAM::NEMURI_CHECK_INCLUDE_ZETTAINEMURI ) )
      {
        BTL_HANDEX_PARAM_KILL* param = reinterpret_cast<BTL_HANDEX_PARAM_KILL *>( flowWk->HANDEX_Push( BTL_HANDEX_KILL, pokeID ));
          param->pokeID = targetPokeID;
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Izanau );
          HANDEX_STR_AddArg( &param->exStr, targetPokeID );
        flowWk->HANDEX_Pop( param );
      }
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * とどめばり
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_TodomeBari( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL,     handler_TodomeBari },          // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_TodomeBari( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( HandCommon_GetKillCount(myHandle, flowWk, pokeID, work) )
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
      param->poke_cnt   = 1;
      param->pokeID[0]  = pokeID;
      param->rankType   = pml::wazadata::RANKEFF_ATTACK;
      param->rankVolume = 3;
      param->fWazaEffect = true;
    flowWk->HANDEX_Pop( param );
  }
}


//----------------------------------------------------------------------------------
/**
 * こごえるはどう
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_KogoeruHadou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL, handler_KogoeruHadou }, // ダメージプロセス終了ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージプロセス終了ハンドラ
void handler_KogoeruHadou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( flowWk->Hnd_GetWeather() == BTL_WEATHER_SNOW )
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
        param->poke_cnt   = 1;
        param->pokeID[0]  = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
        param->rankType   = pml::wazadata::RANKEFF_AGILITY;
        param->rankVolume = -2;
        param->fWazaEffect = true;
      flowWk->HANDEX_Pop( param );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * ひっくりかえす
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Hikkurikaesu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Hikkurikaesu },          // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Hikkurikaesu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = EVENTVAR_GetValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1) );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetPokeID );

    bool bEffective = false;

    int* tmpWk = (int*)(flowWk->Hnd_GetTmpWork( sizeof(int)*BTL_POKEPARAM::BPP_RANKVALUE_RANGE ));
    for(u32 i=0; i<BTL_POKEPARAM::BPP_RANKVALUE_RANGE; ++i)
    {
      tmpWk[ i ] = target->GetValue( (BTL_POKEPARAM::ValueID)(BTL_POKEPARAM::BPP_RANKVALUE_START+i) );
      BTL_PRINT("pokeID(%d) rank_%d value=%d\n", targetPokeID, BTL_POKEPARAM::BPP_RANKVALUE_START+i, tmpWk[i] );
      if( tmpWk[i] != BTL_POKEPARAM::RANK_STATUS_DEFAULT )
      {
        tmpWk[ i ] -= BTL_POKEPARAM::RANK_STATUS_DEFAULT;
        tmpWk[i] *= -1;
        BTL_PRINT("  effective! -> %d\n", tmpWk[i] );
        tmpWk[i] += BTL_POKEPARAM::RANK_STATUS_DEFAULT;
        bEffective = true;
      }
    }

    if( bEffective )
    {
      BTL_HANDEX_PARAM_SET_RANK* param = reinterpret_cast<BTL_HANDEX_PARAM_SET_RANK *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_RANK, pokeID ));
        param->pokeID = targetPokeID;                          ///< 対象ポケモンID
        param->attack      = tmpWk[ BTL_POKEPARAM::BPP_ATTACK_RANK     - BTL_POKEPARAM::BPP_RANKVALUE_START ];
        param->defence     = tmpWk[ BTL_POKEPARAM::BPP_DEFENCE_RANK    - BTL_POKEPARAM::BPP_RANKVALUE_START ];
        param->sp_attack   = tmpWk[ BTL_POKEPARAM::BPP_SP_ATTACK_RANK  - BTL_POKEPARAM::BPP_RANKVALUE_START ];
        param->sp_defence  = tmpWk[ BTL_POKEPARAM::BPP_SP_DEFENCE_RANK - BTL_POKEPARAM::BPP_RANKVALUE_START ];
        param->agility     = tmpWk[ BTL_POKEPARAM::BPP_AGILITY_RANK    - BTL_POKEPARAM::BPP_RANKVALUE_START ];
        param->hit_ratio   = tmpWk[ BTL_POKEPARAM::BPP_HIT_RATIO       - BTL_POKEPARAM::BPP_RANKVALUE_START ];
        param->avoid_ratio = tmpWk[ BTL_POKEPARAM::BPP_AVOID_RATIO     - BTL_POKEPARAM::BPP_RANKVALUE_START ];
        param->critical_rank = target->GetCriticalRankPure();
      flowWk->HANDEX_Pop( param );

      BTL_HANDEX_PARAM_MESSAGE* msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Hikkurikaesu );
        HANDEX_STR_AddArg( &msg_param->str, targetPokeID );
      flowWk->HANDEX_Pop( msg_param );
    }
  }
}


//----------------------------------------------------------------------------------
/**
 * ねらいパンチ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_NeraiPunch( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TEMPT_TARGET,          handler_NeraiPunch     },    // ターゲット決定
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_NeraiPunch( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 default_target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* default_target = flowWk->Hnd_GetPokeParam( default_target_pokeID );

    // 元々の対象が「まもる」を使っている場合、それ以外のポケを対象に変える
    if( default_target->TURNFLAG_Get(BTL_POKEPARAM::TURNFLG_MAMORU) )
    {
      BtlExPos  exPos;
      u8  pokeIDAry[ BTL_EXIST_POS_MAX ];
      u8  poke_cnt;

      exPos = EXPOS_MAKE( BTL_EXPOS_AREA_ENEMY, flowWk->Hnd_PokeIDtoPokePos(pokeID) );
      poke_cnt = flowWk->Hnd_ExpandPokeID( exPos, pokeIDAry );
      if( poke_cnt > 1 )
      {
        u32 target_cnt = 0;

        for(u32 i=0; i<poke_cnt; ++i)
        {
          if( pokeIDAry[i] == default_target_pokeID ){ continue; }
          const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeIDAry[i] );
          if( !bpp->TURNFLAG_Get(BTL_POKEPARAM::TURNFLG_MAMORU) )
          {
            work[target_cnt++] = pokeIDAry[i];
          }
        }

        if( target_cnt )
        {
          u8 idx = calc::GetRand( target_cnt );
          HandCommon_TemptTarget( work[idx], TEMPT_TARGET_PRIORITY_WAZA_1, TEMPT_TARGET_CAUSE_OTHER );
        }
      }
    }
  }
}


//----------------------------------------------------------------------------------
/**
 * すてぜりふ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_SuteZerifu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNDAMAGEPROC_END,          handler_TonboGaeri     },    // ターゲット決定
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

//----------------------------------------------------------------------------------
/**
 * フライングプレス
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_FlyingPress( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_AFFINITY,               handler_FlyingPress           }, // 相性計算
    { BTL_EVENT_CHECK_AFFINITY_ONLY_ATTACKER, handler_FlyingPress           }, // 相性計算( 攻撃側のイベントハンドラのみが反応し、防御側の特性等を考慮しない )
    { BTL_EVENT_CALC_HIT_CANCEL,              handler_Fumituke_HitCheckSkip }, // 命中計算スキップについて「ふみつけ」と共通
    { BTL_EVENT_WAZA_DMG_PROC3,               handler_Fumituke_DamegeProc   }, // ダメージ最終チェックについて「ふみつけ」と共通
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_FlyingPress( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE_EX, POKETYPE_HIKOU );
  }
}

//----------------------------------------------------------------------------------
/**
 * フリーズドライ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_FreezDry( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_AFFINITY,                handler_FreezDry  }, // 相性計算
    { BTL_EVENT_CHECK_AFFINITY_ONLY_ATTACKER,  handler_FreezDry  }, // 相性計算( 攻撃側のイベントハンドラのみが反応し、防御側の特性等を考慮しない )
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_FreezDry( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_POKE_TYPE) == POKETYPE_MIZU )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_TYPEAFF, pml::battle::TypeAffinity::TYPEAFF_2 );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * そうでん
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Souden( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Souden },          // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Souden( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = EVENTVAR_GetValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1) );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetPokeID );

    if( !(target->TURNFLAG_Get(BTL_POKEPARAM::TURNFLG_ACTION_DONE)) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param;

      param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
        param->pokeID = targetPokeID;
        param->sickID = pml::wazadata::WAZASICK_FORCE_WAZATYPE;
        param->sickCont = SICKCONT_MakeTurnParam( pokeID, 1, POKETYPE_DENKI );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Souden );
        HANDEX_STR_AddArg( &param->exStr, targetPokeID );
      flowWk->HANDEX_Pop( param );
    }
  }
}




//----------------------------------------------------------------------------------
/**
 * グラスフィールド
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_GrassField( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL,  handler_GrassField   },  // フィールド効果
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_GrassField( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  handler_common_GroundSet( myHandle, flowWk, pokeID, work, BTL_GROUND_GRASS, BTL_STRID_STD_GrassGround_Start );
}
//----------------------------------------------------------------------------------
/**
 * ミストフィールド
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_MistField( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL,  handler_MistField   },  // フィールド効果
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_MistField( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  handler_common_GroundSet( myHandle, flowWk, pokeID, work, BTL_GROUND_MIST, BTL_STRID_STD_MistGround_Start );
}
//----------------------------------------------------------------------------------
/**
 * エレキフィールド
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_ElecField( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL,  handler_ElecField   },  // フィールド効果
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_ElecField( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  handler_common_GroundSet( myHandle, flowWk, pokeID, work, BTL_GROUND_ELEKI, BTL_STRID_STD_ElecField_Start );
}
//----------------------------------------------------------------------------------
/**
 * サイコフィールド
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_PhychoField( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL,  handler_PhychoField   },  // フィールド効果
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_PhychoField( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  handler_common_GroundSet( myHandle, flowWk, pokeID, work, BTL_GROUND_PHYCHO, BTL_STRID_STD_PhychoField_Start );
}



// フィールド系共通
void handler_common_GroundSet( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work, BtlGround ground, u16 strID )
{
  GFL_UNUSED( work );
  GFL_UNUSED( myHandle );

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( flowWk->Hnd_GetGround() != ground )
    {
      BTL_HANDEX_PARAM_ADD_FLDEFF* param;

      param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_FLDEFF *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_FLDEFF, pokeID ));
        param->effect = FieldStatus::EFF_GROUND;
        param->ground = ground;
        param->cont = SICKCONT_MakeTurn( pokeID, 5 );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, strID );
      flowWk->HANDEX_Pop( param );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * キングシールド
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_KingShield( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASEQ_START,           handler_Mamoru_StartSeq             }, // ワザ処理開始
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD,  handler_Mamoru_ExeCheck             }, // ワザ出し成否チェックハンドラ
    { BTL_EVENT_WAZA_EXECUTE_FAIL,       handler_Mamoru_ExeFail              }, // ワザだし失敗確定
    { BTL_EVENT_UNCATEGORIZE_WAZA,       handler_KingShield                  }, // 未分類ワザハンドラ
    { BTL_EVENT_MAMORU_SUCCESS,          handler_KingShield_Success          }, // まもる効果成功
    { BTL_EVENT_WAZA_DMG_PROC_Z,         handler_KingShield_DmgZ             }, // Z技に対するダメージ補正
    { BTL_EVENT_AFTER_CRITICAL,          handler_KingShield_MsgAfterCritical }, // 急所メッセージ後
    { BTL_EVENT_WAZA_DMG_REACTION_L2,    handler_KingShield_DmgReaction      }, // ダメージ反応ハンドラ
    { BTL_EVENT_TURNCHECK_BEGIN,         handler_Mamoru_Off                  }, // ターンチェック開始ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_KingShield( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( myHandle );

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    handler_Mamoru_Core( flowWk, pokeID );

    BTL_HANDEX_PARAM_TURNFLAG* flagParam;
    flagParam = reinterpret_cast<BTL_HANDEX_PARAM_TURNFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_TURNFLAG, pokeID ));
      flagParam->pokeID = pokeID;
      flagParam->flag = BTL_POKEPARAM::TURNFLG_MAMORU_ONLY_DAMAGE_WAZA;
    flowWk->HANDEX_Pop( flagParam );

    work[ WORKIDX_STICK ] = 1;
  }
}
void handler_KingShield_Success( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  kingShield_Success( flowWk, pokeID );
}

// Z技に対するダメージ補正( Z技によるダメージを減少させる )
void handler_KingShield_DmgZ( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  mamoru_DmgZ( flowWk, pokeID, work );
}

// 急所メッセージ後( Z技によるダメージを減少させたメッセージを表示する )
void handler_KingShield_MsgAfterCritical( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  work[1] = mamoru_MsgAfterCritical( flowWk, pokeID, work );
}

// ダメージ反応ハンドラ( Z技によるダメージを減少させた後、相手にダメージを与える )
void handler_KingShield_DmgReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( work[1] )
  {
    kingShield_Success( flowWk, pokeID );
    work[1] = 0;
  }
}

/**
 * @brief [キングシールド] 相手の攻撃を守ったときの処理
 */
void kingShield_Success( ServerFlow* flowWk, u8 pokeID )
{
  if( ( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID ) &&
      ( EVENTVAR_GetValue(BTL_EVAR_WAZA_TOUCH_FLAG)) )
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* rank_param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
      rank_param->rankType = pml::wazadata::RANKEFF_ATTACK;
      rank_param->rankVolume = -2;
      rank_param->fAlmost = true;
      rank_param->fWazaEffect = true;
      rank_param->poke_cnt = 1;
      rank_param->bMigawariThrew = true;
      rank_param->cause = RANKEFFECT_CAUSE_KINGSHIELD;
      rank_param->pokeID[0] = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    flowWk->HANDEX_Pop( rank_param );
  }
}


//----------------------------------------------------------------------------------
/**
 * サウザンアロー
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_ThousanArrow( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_AFFINE_FLOATING_CANCEL,  handler_ThousanArrow_CancelFloat  }, // 相性計算の時にふゆうチェックをキャンセル
    { BTL_EVENT_CHECK_AFFINITY_ENABLE,   handler_ThousanArrow_AffEnable    }, // 相性無効チェックを必ず行う
    { BTL_EVENT_REWRITE_AFFINITY,        handler_ThousanArrow_CheckAffine  }, // タイプ相性による無効チェック
    { BTL_EVENT_CHECK_POKE_HIDE,         handler_Kaminari_checkHide        }, // 消えポケヒットチェック
//    { BTL_EVENT_UNCATEGORIZE_WAZA,       handler_ThousanArrow              }, // 未分類ワザ
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL, handler_ThousanArrow              },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 相性計算の時にふゆうチェックをキャンセル
void handler_ThousanArrow_CancelFloat( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
  }
}
// 相性無効チェックを必ず行う
void handler_ThousanArrow_AffEnable( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
  }
}

// タイプ相性による無効チェックハンドラ
void handler_ThousanArrow_CheckAffine( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( ( EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK ) == pokeID ) && 
      ( EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE ) == POKETYPE_JIMEN ) &&
      ( EVENTVAR_GetValue( BTL_EVAR_TYPEAFF ) == pml::battle::TypeAffinity::TYPEAFF_0 ) )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_TYPEAFF, pml::battle::TypeAffinity::TYPEAFF_1 );
  }
}
// ダメージ処理後（みがわり除く）ハンドラ
void handler_ThousanArrow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u32 targetCount = EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
    for(u32 i=0; i<targetCount; ++i)
    {
      u8 targetPokeID = EVENTVAR_GetValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1+i) );
      common_UtiotosuEffect( flowWk, pokeID, targetPokeID );
    }
  }
}


//----------------------------------------------------------------------------------
/**
 * ハッピータイム
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_HappyTime( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,       handler_HappyTime    }, // 未分類ワザ効果
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 未分類ワザ効果ハンドラ
void handler_HappyTime( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    flowWk->Hnd_SetMoneyDblUp( pokeID, MONEY_DBLUP_EXTRA );

    BTL_HANDEX_PARAM_MESSAGE* msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_STD, BTL_STRID_STD_HappyTime );
    flowWk->HANDEX_Pop( msg_param );
  }
}

//----------------------------------------------------------------------------------
/**
 * じばそうさ・アシストギア
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_ZibaSousa( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_GET_RANKEFF_VALUE,       handler_ZibaSousa    }, // ランク増原効果決定ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

/**
 * @brief ランク増原効果決定ハンドラ
 *
 * 技ターゲットの特性が「プラス」か「マイナス」でないなら、ランク効果を無効化する
 */
void handler_ZibaSousa( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetPokeID );
    u32  tokusei = target->GetValue( BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE );

    if( (tokusei != TOKUSEI_PURASU) && (tokusei != TOKUSEI_MAINASU) ){
      EVENTVAR_RewriteValue( BTL_EVAR_STATUS_TYPE, pml::wazadata::RANKEFF_NULL );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * ベノムトラップ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_BenomTrap( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_GET_RANKEFF_VALUE,       handler_BenomTrap    }, // ランク増原効果決定ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ランク増原効果決定ハンドラ
void handler_BenomTrap( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_PRINT("[[!>BP btl_PokeParam.cpp#709]]\n");
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetPokeID );

    if( !target->CheckSick(pml::wazadata::WAZASICK_DOKU) )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_STATUS_TYPE, pml::wazadata::RANKEFF_NULL );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * プラズマシャワー
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_PlasmaShower( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_L1, handler_PlasmaShower },    // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_PlasmaShower( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( work );
  GFL_UNUSED( myHandle );

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_FLDEFF* param;
      param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_FLDEFF *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_FLDEFF, pokeID ));
      param->effect = FieldStatus::EFF_PLASMASHOWER;
      param->cont = SICKCONT_MakeTurn( pokeID, 1 );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, BTL_STRID_STD_PlasmaShower );
    flowWk->HANDEX_Pop( param );
  }
}

//----------------------------------------------------------------------------------
/**
 * フェアリーロック
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_FairyLock( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL, handler_FairyLock },    // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_FairyLock( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( work );
  GFL_UNUSED( myHandle );

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_FLDEFF* param;
      param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_FLDEFF *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_FLDEFF, pokeID ));
      param->effect = FieldStatus::EFF_FAIRY_LOCK;
      param->cont = SICKCONT_MakeTurn( pokeID, 2 );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, BTL_STRID_STD_FairyLock );
    flowWk->HANDEX_Pop( param );
  }
}


//----------------------------------------------------------------------------------
/**
 * ふんじん
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Funjin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,       handler_Funjin    }, // 未分類ワザ効果
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Funjin( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u32 targetCount = EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
    for(u32 i=0; i<targetCount; ++i)
    {
      u8 targetPokeID = EVENTVAR_GetValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1 + i) );

      BTL_HANDEX_PARAM_ADD_SICK* param;

      param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
        param->pokeID = targetPokeID;
        param->sickID = pml::wazadata::WAZASICK_FUNJIN;
        param->sickCont = SICKCONT_MakeTurn( pokeID, 1 );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Funjin );
        HANDEX_STR_AddArg( &param->exStr, targetPokeID );
      flowWk->HANDEX_Pop( param );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * ジオコントロール
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_GeoControl( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_GeoControl_TameStart },     // 溜め開始
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_GeoControl_TameStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_GeoCtrl );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    flowWk->HANDEX_Pop( msg_param );
  }
}
//----------------------------------------------------------------------------------
/**
 * トリックガード
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_TrickGuard( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD,      handler_WideGuard_ExeCheck },  // ワザ出し成否チェックハンドラ
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET, handler_TrickGuard         },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_TrickGuard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  BTL_SICKCONT  cont = SICKCONT_MakeTurn( pokeID, 1 );
  BtlSide side = flowWk->Hnd_GetMainModule()->PokeIDtoSide( pokeID );
  common_SideEffectStdMsg( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_TRICKGUARD, cont, BTL_STRID_STD_TrickGuard );
}
//----------------------------------------------------------------------------------
/**
 * ニードルガード
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_NeedleGuard( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASEQ_START,           handler_Mamoru_StartSeq              }, // ワザ処理開始
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD,  handler_Mamoru_ExeCheck              }, // ワザ出し成否チェックハンドラ
    { BTL_EVENT_WAZA_EXECUTE_FAIL,       handler_Mamoru_ExeFail               }, // ワザだし失敗確定
    { BTL_EVENT_UNCATEGORIZE_WAZA,       handler_NeedleGuard                  }, // 未分類ワザハンドラ
    { BTL_EVENT_MAMORU_SUCCESS,          handler_NeedleGuard_Success          }, // まもる効果成功
    { BTL_EVENT_WAZA_DMG_PROC_Z,         handler_NeedleGuard_DmgZ             }, // Z技に対するダメージ補正
    { BTL_EVENT_AFTER_CRITICAL,          handler_NeedleGuard_MsgAfterCritical }, // 急所メッセージ後
    { BTL_EVENT_WAZA_DMG_REACTION_L2,    handler_NeedleGuard_DmgReaction      }, // ダメージ反応ハンドラ
    { BTL_EVENT_TURNCHECK_BEGIN,         handler_Mamoru_Off                   }, // ターンチェック開始ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_NeedleGuard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  handler_Mamoru_Core( flowWk, pokeID );
  work[ WORKIDX_STICK ] = 1;
}
// まもる効果成功ハンドラ
void handler_NeedleGuard_Success( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  needleGuard_Success( flowWk, pokeID );
}

// Z技に対するダメージ補正( Z技によるダメージを減少させる )
void handler_NeedleGuard_DmgZ( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  mamoru_DmgZ( flowWk, pokeID, work );
}

// 急所メッセージ後( Z技によるダメージを減少させたメッセージを表示する )
void handler_NeedleGuard_MsgAfterCritical( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  work[1] = mamoru_MsgAfterCritical( flowWk, pokeID, work );
}

// ダメージ反応ハンドラ( Z技によるダメージを減少させた後、相手にダメージを与える )
void handler_NeedleGuard_DmgReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( work[1] )
  {
    needleGuard_Success( flowWk, pokeID );
    work[1] = 0;
  }
}

/**
 * @brief [ニードルガード] 相手の攻撃を守ったときの処理
 */
void needleGuard_Success( ServerFlow* flowWk, u8 pokeID )
{
  if( ( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID ) &&
      ( EVENTVAR_GetValue(BTL_EVAR_WAZA_TOUCH_FLAG)) )
  {
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( targetPokeID );

    BTL_HANDEX_PARAM_DAMAGE* param;
    param = reinterpret_cast<BTL_HANDEX_PARAM_DAMAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_DAMAGE, pokeID ));
      param->pokeID = targetPokeID;
      param->damage = calc::QuotMaxHP( bpp, 8 );
      param->fAvoidHidePoke = false;
      param->damageCause = DAMAGE_CAUSE_NEEDLEGUARD;
      param->damageCausePokeID = pokeID;
      param->fDisableDeadProcess = true;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_NeedleGuard );
      HANDEX_STR_AddArg( &param->exStr, targetPokeID );
    flowWk->HANDEX_Pop( param );
  }
}

//----------------------------------------------------------------------------------
/**
 * サウザンウェーブ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_SouthernWave( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL, handler_southernWave },   // ダメージ処理後（みがわり除く）
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージ処理後
void handler_southernWave( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    uint32_t targetCnt = EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
    for(uint32_t i=0; i<targetCnt; ++i)
    {
      u8 targetPokeID = EVENTVAR_GetValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1 + i) );
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( targetPokeID );
      if( !bpp->CheckSick(pml::wazadata::WAZASICK_TOOSENBOU) )
      {
        BTL_HANDEX_PARAM_ADD_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));

          param->pokeID = targetPokeID;
          param->sickID = pml::wazadata::WAZASICK_TOOSENBOU;
          param->sickCont = calc::MakeWazaSickCont_Poke( pokeID, pokeID );
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_CantEscWaza );
          HANDEX_STR_AddArg( &param->exStr,targetPokeID );

        flowWk->HANDEX_Pop( param );
      }
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * いじげんラッシュ
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_IjigenRush( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD, handler_IjigenRush },    // ワザ出し成否チェックハンドラ

    { BTL_EVENT_CHECK_MAMORU_BREAK,    handler_Feint_MamoruBreak      },   // まもる無効化チェック
    { BTL_EVENT_NOEFFECT_CHECK_SIDEEFF_GUARD_BEGIN, handler_Feint_NoEffCheckBegin },  // 無効化チェック開始
    { BTL_EVENT_NOEFFECT_CHECK_SIDEEFF_GUARD_END,   handler_Feint_NoEffCheckEnd   },  // 無効化チェック終了
    { BTL_EVENT_WAZA_DMG_DETERMINE,   handler_IjigenRush_AfterDamage     }, // ダメージ処理後

  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

void handler_IjigenRush( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    //GetHenshinMonsNo()はいじげんラッシュ専用！
    if( bpp->GetHenshinMonsNo() != MONSNO_HUUPA )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_IJIGEN_OTHER );
    }
    else
    if( bpp->GetFormNo() != FORMNO_HUUPA_EARNEST )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_IJIGEN_FOOPA );
    }

  }
}

void handler_IjigenRush_AfterDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_feint_proc( flowWk, pokeID, BTL_STRID_SET_MamoruBreak );
}

//----------------------------------------------------------------------------------
/**
 * ダークホール
 *
 * ダークライ専用技
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_DarkHole( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD, handler_DarkHole },  // ワザ出し成否チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// ワザ出し成否チェックハンドラ
void handler_DarkHole( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->GetHenshinMonsNo() != MONSNO_DAAKURAI )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_DARKHOLE_OTHER );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * トーチカ
 *
 * まもる。
 * まもったタイミングで接触攻撃を受けると、相手を「どく」にする。
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Tootika( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASEQ_START,           handler_Mamoru_StartSeq          }, // ワザ処理開始
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD,  handler_Mamoru_ExeCheck          }, // ワザ出し成否チェックハンドラ
    { BTL_EVENT_WAZA_EXECUTE_FAIL,       handler_Mamoru_ExeFail           }, // ワザだし失敗確定
    { BTL_EVENT_UNCATEGORIZE_WAZA,       handler_Tootika                  }, // 未分類ワザハンドラ
    { BTL_EVENT_MAMORU_SUCCESS,          handler_Tootika_Success          }, // まもる効果成功
    { BTL_EVENT_WAZA_DMG_PROC_Z,         handler_Tootika_DmgZ             }, // Z技に対するダメージ補正
    { BTL_EVENT_AFTER_CRITICAL,          handler_Tootika_MsgAfterCritical }, // 急所メッセージ後の特殊メッセージ
    { BTL_EVENT_WAZA_DMG_REACTION_L2,    handler_Tootika_DmgReaction      }, // ダメージ反応ハンドラ
    { BTL_EVENT_TURNCHECK_BEGIN,         handler_Mamoru_Off               }, // ターンチェック開始ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Tootika( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  handler_Mamoru_Core( flowWk, pokeID );
  work[ WORKIDX_STICK ] = 1;
}
// まもる効果成功ハンドラ
void handler_Tootika_Success( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  tootika_Success( flowWk, pokeID );
}

// Z技に対するダメージ補正( Z技によるダメージを減少させる )
void handler_Tootika_DmgZ( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  mamoru_DmgZ( flowWk, pokeID, work );
}

// 急所メッセージ後( Z技によるダメージを減少させたメッセージを表示する )
void handler_Tootika_MsgAfterCritical( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  work[1] = mamoru_MsgAfterCritical( flowWk, pokeID, work );
}

// ダメージ反応ハンドラ( Z技によるダメージを減少させた後、相手にダメージを与える )
void handler_Tootika_DmgReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( work[1] )
  {
    tootika_Success( flowWk, pokeID );
    work[1] = 0;
  }
}

/**
 * @brief [トーチカ] 相手の攻撃を守ったときの処理
 */
void tootika_Success( ServerFlow* flowWk, u8 pokeID )
{
  if( ( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID ) &&
      ( EVENTVAR_GetValue(BTL_EVAR_WAZA_TOUCH_FLAG)) )
  {
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );

    BTL_HANDEX_PARAM_ADD_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK*>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ) );
      param->pokeID    = targetPokeID;
      param->sickID    = pml::wazadata::WAZASICK_DOKU;
      param->sickCont  = calc::MakeDefaultPokeSickCont( pml::pokepara::SICK_DOKU, pokeID );
      param->sickCause = SICK_CAUSE_TOOTIKA;
    flowWk->HANDEX_Pop( param );
  }
}

//----------------------------------------------------------------------------------
/**
 * めざめるダンス
 *
 * 技のタイプがポケモンの第１タイプと同じになる
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_MezameruDance( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM,  handler_MezameruDance_WazaParam },  // わざパラメータチェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

/**
 * @brief わざパラメータチェック
 */
void handler_MezameruDance_WazaParam( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) != pokeID )
  {
    return;
  }

  // 技を撃つポケモンの第１タイプと同じタイプにする
  const BTL_POKEPARAM* attacker = flowWk->Hnd_GetPokeParam( pokeID );
  const PokeTypePair typePair = attacker->GetPokeType();
  pml::PokeType type = PokeTypePair_GetType1( typePair );
  if( type == POKETYPE_NULL ){
    type = PokeTypePair_GetTypeEx( typePair );
  }

  EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, type );
}

//----------------------------------------------------------------------------------
/**
 * かふんだんご
 *
 * 味方に出すと味方を1/2回復する
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Kahundango( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND,       handler_Kahundango_ExecuteCheck   }, // ワザ出し失敗チェック
    { BTL_EVENT_DMG_TO_RECOVER_CHECK,         handler_Kahundango_RecoverCheck   }, // ダメージワザ回復化チェックハンドラ
    { BTL_EVENT_DMG_TO_RECOVER_FIX,           handler_Kahundango_RecoverFix     }, // ダメージワザ回復化確定ハンドラ
    { BTL_EVENT_CHECK_AFFINITY_ONLY_ATTACKER, handler_Kahundango_Check_Affinity }, // 相性計算( 攻撃側のイベントハンドラのみが反応し、防御側の特性等を考慮しない )
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
/**
 * ワザ出し失敗チェックハンドラ
 */
void handler_Kahundango_ExecuteCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) != pokeID ){
    return;
  }
  if( EVENTVAR_GetValue(BTL_EVAR_TARGET_POKECNT) == 0 ){
    return;
  }

  const MainModule* mainModule = flowWk->Hnd_GetMainModule();
  u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
  if( mainModule->IsFriendPokeID(pokeID, targetPokeID) )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->CheckSick(pml::wazadata::WAZASICK_KAIHUKUHUUJI) )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_KAIHUKUHUUJI );
    }
  }
}
/**
 * ダメージワザ回復化チェックハンドラ
 */
void handler_Kahundango_RecoverCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID )
  {
    return;
  }

  // ターゲットが味方なら、回復化する
  const MainModule* mainModule = flowWk->Hnd_GetMainModule();
  const u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  if( mainModule->IsFriendPokeID( pokeID, targetPokeID ) )
  {
    work[0] = EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
  }

  // 技エフェクトを変更
  const u8 effIndex = ( work[0] ) ? ( BTLV_WAZAEFF_KAHUNDANGO_RECOVER ) : ( BTLV_WAZAEFF_KAHUNDANGO_DAMAGE );
  flowWk->Hnd_SetWazaEffectIndex( effIndex );
}

/**
 * @brief ダメージワザ回復化確定ハンドラ
 */
void handler_Kahundango_RecoverFix( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( ( EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK ) != pokeID ) ||
     !( work[0] ) )
  {
    return;
  }

  const u8 target_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( target_pokeID );

  if( !bpp->IsHPFull() )
  {
    BTL_HANDEX_PARAM_RECOVER_HP* param = reinterpret_cast<BTL_HANDEX_PARAM_RECOVER_HP*>( flowWk->HANDEX_Push( BTL_HANDEX_RECOVER_HP, pokeID ) );
      param->pokeID = target_pokeID;
      param->recoverHP = calc::QuotMaxHP( bpp, 2 );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_HP_Recover );
      HANDEX_STR_AddArg( &param->exStr, target_pokeID );
    flowWk->HANDEX_Pop( param );
  }
  else
  {
    BTL_HANDEX_PARAM_MESSAGE* msg_param;
      msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_NoEffect );
      HANDEX_STR_AddArg( &msg_param->str, target_pokeID );
    flowWk->HANDEX_Pop( msg_param );
  }
}

/**
* 相性計算( 攻撃側のイベントハンドラのみが反応し、防御側の特性等を考慮しない )
*/
void handler_Kahundango_Check_Affinity(BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work)
{
  GFL_UNUSED(myHandle);
  if (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID)
  {
    return;
  }

  // ターゲットが味方なら、効果表記を一律こうかありにする
  const MainModule* mainModule = flowWk->Hnd_GetMainModule();
  const u8 targetPokeID = EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF);
  if (mainModule->IsFriendPokeID(pokeID, targetPokeID))
  {
    EVENTVAR_RewriteValue(BTL_EVAR_TYPEAFF, pml::battle::TypeAffinity::TYPEAFF_1);
  }
}

//----------------------------------------------------------------------------------
/**
 * コアパニッシャー
 *
 * そのターンに技を使用していた相手の特性を消す
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_CorePunisher( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION_PREV, handler_CorePunisher_HitReal },   // ダメージ処理後（みがわり除く）
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// ダメージ処理後
void handler_CorePunisher_HitReal( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID )
  {
    return;
  }

  u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  const BTL_POKEPARAM* targetPoke   = flowWk->Hnd_GetPokeParam( targetPokeID );

  if( targetPoke->TURNFLAG_Get( BTL_POKEPARAM::TURNFLG_ACTION_DONE ) &&
     !targetPoke->CheckSick( pml::wazadata::WAZASICK_IEKI ) )
  {
    BTL_HANDEX_PARAM_ADD_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ) );
      param->pokeID   = targetPokeID;
      param->sickID   = pml::wazadata::WAZASICK_IEKI;
      param->sickCont = SICKCONT_MakePermanent( pokeID );
      // 「とびだすなかみ」はひんしでも発動するとくせいなので、これで強制的に無効化する。
      if( targetPoke->GetValue(BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE) == TOKUSEI_TOBIDASUNAKAMI ){
        param->fEffectiveToDeadPoke = 1;
      }
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_CorePunisher );
      HANDEX_STR_AddArg( &param->exStr,targetPokeID );
    flowWk->HANDEX_Pop( param );
  }
}

//----------------------------------------------------------------------------------
/**
 * がげぬい・アンカーショット
 *
 * 相手を逃げられない状態にする
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Kagenui( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL, handler_Kagenui_HitReal },   // ダメージ処理後（みがわり除く）
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// ダメージ処理後
void handler_Kagenui_HitReal( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID )
  {
    return;
  }
  // 相手が「りんぷん」または自分が「ちからづく」なら効果なし
  if( EVENTVAR_GetValue(BTL_EVAR_RINPUNGUARD_FLG)
  ||  EVENTVAR_GetValue(BTL_EVAR_TIKARAZUKU_FLG)
    ){
      return;
  }


  const u32 targetCount = EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
  for( u32 i=0; i<targetCount; ++i )
  {
    const u8 targetPokeID = EVENTVAR_GetValue( static_cast<BtlEvVarLabel>( BTL_EVAR_POKEID_TARGET1 + i ) );
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( targetPokeID );
    if( !( bpp->CheckSick( pml::wazadata::WAZASICK_TOOSENBOU ) ) && 
        !( bpp->IsMatchType( POKETYPE_GHOST ) ) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ) );
        param->pokeID   = targetPokeID;
        param->sickID   = pml::wazadata::WAZASICK_TOOSENBOU;
        param->sickCont = calc::MakeWazaSickCont_Poke( pokeID, pokeID );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_CantEscWaza );
        HANDEX_STR_AddArg( &param->exStr,targetPokeID );
      flowWk->HANDEX_Pop( param );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * じょうか
 *
 * 相手の状態異常を治す。治すと自分のHP1/2回復。
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Zyouka( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Zyouka },    // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

void handler_Zyouka( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID )
  {
    return;
  }

  bool isSickCured = false;

  // ターゲットの状態異常を治す
  const u32 targetCount = EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
  for( u32 i=0; i<targetCount; ++i )
  {
    const u8             targetPokeID = EVENTVAR_GetValue( static_cast<BtlEvVarLabel>( BTL_EVAR_POKEID_TARGET1 + i ) );
    const BTL_POKEPARAM* targetPoke   = flowWk->Hnd_GetPokeParam( targetPokeID );
    const PokeSick       targetSick   = targetPoke->GetPokeSick();
    if( targetSick != pml::pokepara::SICK_NULL )
    {
      BTL_HANDEX_PARAM_CURE_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK*>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ) );
        param->sickCode  = static_cast<BtlWazaSickEx>( targetSick );
        param->pokeID[0] = targetPokeID;
        param->poke_cnt  = 1;
      flowWk->HANDEX_Pop( param );

      isSickCured = true;
    }
  }

  // 状態異常を治したら、自身のＨＰを回復
  if( isSickCured )
  {
    const BTL_POKEPARAM* pokeParam = flowWk->Hnd_GetPokeParam( pokeID );

    BTL_HANDEX_PARAM_RECOVER_HP* param = reinterpret_cast<BTL_HANDEX_PARAM_RECOVER_HP*>( flowWk->HANDEX_Push( BTL_HANDEX_RECOVER_HP, pokeID ) );
      param->pokeID         = pokeID;
      param->recoverHP      = calc::QuotMaxHP( pokeParam, 2 );
      param->fFailCheckThru = ( EVENTVAR_GetValue( BTL_EVAR_ZENRYOKU_WAZA_KIND ) == pml::waza::ZENRYOKUWAZA_KIND_BOOST ); //「Zじょうか」なら「かいふくふうじ」を突破できる
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_HP_Recover );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
    flowWk->HANDEX_Pop( param );
  }
}

//----------------------------------------------------------------------------------
/**
 * ちからをすいとる
 *
 * 相手の「こうげき」ランクを１段階ダウンさせて、自分のHPを回復する。
 * 相手の「こうげき」の数値と同じだけ回復する。
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Tikarawosuitoru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Tikarawosuitoru },    // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// 未分類ワザハンドラ
void handler_Tikarawosuitoru( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID )
  {
    return;
  }

  static const s32 RANK_EFFECT_VALUE = -1;

  const u8             targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
  const BTL_POKEPARAM* targetPoke   = flowWk->Hnd_GetPokeParam( targetPokeID );

  if( targetPoke->IsRankEffectValid( BTL_POKEPARAM::BPP_ATTACK_RANK, RANK_EFFECT_VALUE ) )
  {
    // ランクが下がる前の「こうげき」を記憶
    const u32 oldAttack = targetPoke->GetValue( BTL_POKEPARAM::BPP_ATTACK );

    // 相手の「こうげき」ランクを下げる
    BTL_HANDEX_PARAM_RANK_EFFECT* rank_param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
      rank_param->rankType       = pml::wazadata::RANKEFF_ATTACK;
      rank_param->rankVolume     = RANK_EFFECT_VALUE;
      rank_param->fAlmost        = true;
      rank_param->fWazaEffect    = true;
      rank_param->bMigawariCheck = true;
      rank_param->poke_cnt       = 1;
      rank_param->pokeID[0]      = targetPokeID;
    flowWk->HANDEX_Pop( rank_param );

    // 相手の「こうげき」ランクが下がる前の「こうげき」値の分だけＨＰを回復( 相手が特性「ヘドロえき」ならダメージを受けるため、BTL_HANDEX_RECOVER_HP ではなく BTL_HANDEX_DRAIN )
    const u32 recoverValue = oldAttack;
    if( 0 < recoverValue )
    {
      BTL_HANDEX_PARAM_DRAIN* param = reinterpret_cast<BTL_HANDEX_PARAM_DRAIN*>( flowWk->HANDEX_Push( BTL_HANDEX_DRAIN, pokeID ) );
        param->recoverPokeID = pokeID;
        param->damagedPokeID = targetPokeID;
        param->recoverHP     = recoverValue;
        param->fFailCheckThru = ( EVENTVAR_GetValue( BTL_EVAR_ZENRYOKU_WAZA_KIND ) == pml::waza::ZENRYOKUWAZA_KIND_BOOST ); //「Zちからをすいとる」なら「かいふくふうじ」を突破できる
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_HP_Recover );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
      flowWk->HANDEX_Pop( param );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * とぎすます
 *
 * 次のターンに出す攻撃が必ず急所に当たる。
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Togisumasu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Togisumasu },    // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// 未分類ワザハンドラ
void handler_Togisumasu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID )
  {
    return;
  }

  const u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );

  // とぎすます状態をセット
  BTL_HANDEX_PARAM_ADD_SICK* sick_param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ) );
    sick_param->sickID        = pml::wazadata::WAZASICK_TOGISUMASU;
    sick_param->sickCont      = SICKCONT_MakeTurn( pokeID, 2 );
    sick_param->pokeID        = targetPokeID;
    sick_param->overWriteMode = BTL_SICK_OW_STRONG;  // すでに「とぎすます」状態でも失敗させない
    HANDEX_STR_Setup( &sick_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Togisumasu );
    HANDEX_STR_AddArg( &sick_param->exStr, targetPokeID );
  flowWk->HANDEX_Pop( sick_param );
}

//----------------------------------------------------------------------------------
/**
 * スピードスワップ
 *
 * 相手の素早さと自分の素早さをいれかえる
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_SpeedSwap( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_SpeedSwap },    // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// 未分類ワザハンドラ
void handler_SpeedSwap( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID )
  {
    return;
  }

  const u8             targetPokeID  = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
  const BTL_POKEPARAM* targetPoke    = flowWk->Hnd_GetPokeParam( targetPokeID );
  const u32            targetAgility = targetPoke->GetValue_Base( BTL_POKEPARAM::BPP_AGILITY );

  const BTL_POKEPARAM* attackerPoke    = flowWk->Hnd_GetPokeParam( pokeID );
  const u32            attackerAgility = attackerPoke->GetValue_Base( BTL_POKEPARAM::BPP_AGILITY );

  // ターゲットの「すばやさ」を変更
  {
    BTL_HANDEX_PARAM_SET_STATUS* param = reinterpret_cast<BTL_HANDEX_PARAM_SET_STATUS *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_STATUS, pokeID ));
      param->pokeID         = targetPokeID;
      param->agility        = attackerAgility;
      param->fAgilityEnable = true;
    flowWk->HANDEX_Pop( param );
  }

  // 自身の「すばやさ」を変更
  {
    BTL_HANDEX_PARAM_SET_STATUS* param = reinterpret_cast<BTL_HANDEX_PARAM_SET_STATUS *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_STATUS, pokeID ));
      param->pokeID         = pokeID;
      param->agility        = targetAgility;
      param->fAgilityEnable = true;
    flowWk->HANDEX_Pop( param );
  }

  //「おたがいの　スピードを　いれかえた！」
  {
    BTL_HANDEX_PARAM_MESSAGE* msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE*>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_SpeedSwap );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    flowWk->HANDEX_Pop( msg_param );
  }
}

//----------------------------------------------------------------------------------
/**
 * もえつきる
 *
 * ほのおタイプのポケモンしかこの技は使えない。
 * 攻撃後、自身の「ほのお」タイプが消える。
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Moetukiru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FAIL_WAZAMELT_CHECK,    handler_Moetukiru_WazaMeltCheck }, // ワザ出しによるこおり解除チェックハンドラ
    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND, handler_Moetukiru_ExeCheck      }, // ワザ出し成否チェックハンドラ
    { BTL_EVENT_DAMAGEPROC_END,         handler_Moetukiru_DamageProcEnd }, // ダメージ技シーケンス終了
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザ出しによるこおり解除チェックハンドラ
void handler_Moetukiru_WazaMeltCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 技を撃つポケモンが「ほのお」タイプでないなら失敗
    const BTL_POKEPARAM* attacker     = flowWk->Hnd_GetPokeParam( pokeID );
    if( !(attacker->IsMatchType(POKETYPE_HONOO)) )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
    }
  }
}

// ワザ出し成否チェックハンドラ
void handler_Moetukiru_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) != pokeID )
  {
    return;
  }

  // 技を撃つポケモンが「ほのお」タイプでないなら失敗
  const BTL_POKEPARAM* attacker     = flowWk->Hnd_GetPokeParam( pokeID );
  if( !(attacker->IsMatchType(POKETYPE_HONOO)) )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
  }
}

// ダメージ技シーケンス終了
void handler_Moetukiru_DamageProcEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID )
  {
    return;
  }

  const u8                         attackerPokeId      = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
  const BTL_POKEPARAM*             attacker            = flowWk->Hnd_GetPokeParam( attackerPokeId );
  const PokeTypePair               attackerType        = attacker->GetPokeType();
  const BTL_POKEPARAM::ExTypeCause attackerExTypeCause = attacker->GetExTypeCause();

  // 継続フラグセット
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = reinterpret_cast<BTL_HANDEX_PARAM_SET_CONTFLAG*>( flowWk->HANDEX_Push( BTL_HANDEX_SET_CONTFLAG, pokeID ) );
      param->pokeID = attackerPokeId;
      param->flag   = BTL_POKEPARAM::CONTFLG_MOETUKIRU;
    flowWk->HANDEX_Pop( param );
  }

  // ほのおタイプを消す
  {
    const PokeTypePair               next_type        = PokeTypePair_Replace( attackerType, POKETYPE_HONOO, POKETYPE_NULL );
    const pml::PokeType              next_exType      = PokeTypePair_GetTypeEx( next_type );
    const BTL_POKEPARAM::ExTypeCause next_exTypeCause = ( next_exType == POKETYPE_NULL ) ? ( BTL_POKEPARAM::EXTYPE_CAUSE_NONE ) : ( attackerExTypeCause ); // 拡張タイプが消えたら、拡張タイプの原因も消す

    BTL_HANDEX_PARAM_CHANGE_TYPE* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_TYPE*>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_TYPE, pokeID ) );
      param->next_type      = next_type;
      param->exTypeCause    = next_exTypeCause;
      param->pokeID         = attackerPokeId;
      param->fStdMsgDisable = true;
    flowWk->HANDEX_Pop( param );
  }

  //「もえつきた！」
  {
    BTL_HANDEX_PARAM_MESSAGE* msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE*>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ) );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Moetukiru );
      HANDEX_STR_AddArg( &msg_param->str, attackerPokeId );
    flowWk->HANDEX_Pop( msg_param );
  }
}

//----------------------------------------------------------------------------------
/**
 * スポットライト
 *
 * シングルバトルでは効果なし。
 * ダブルバトルで、味方を対象にすると、相手の攻撃が、対象にいく。
 * ダブルバトルで、相手を対象にすると、味方の攻撃が、対象にいく。
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_SpotLight( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_SpotLight },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// 未分類ワザ
void handler_SpotLight( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK ) != pokeID )
  {
    return;
  }

  const u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
  BtlSide  targetSide   = flowWk->Hnd_GetMainModule()->PokeIDtoSide( targetPokeID );
  BTL_SICKCONT cont     = SICKCONT_MakePokeTurn( targetPokeID, targetPokeID, 1 );


  // サイドエフェクト「スポットライト」をセット
  common_SideEffectCore( flowWk, pokeID, targetSide, BTL_SIDEEFF_SPOTLIGHT, cont,
      BTL_STRTYPE_SET, BTL_STRID_SET_SpotLight, targetPokeID, false );
}

//----------------------------------------------------------------------------------
/**
 * くちばしキャノン
 *
 * ターン内で溜めて攻撃
 * 溜め中に接触されると相手をやけどにする
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_KutibasiCanon( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_BEFORE_FIGHT,      handler_KutibasiCanon_BeforeFight    }, // ターン最初のワザシーケンス直前
    { BTL_EVENT_WAZA_DMG_REACTION, handler_KutibasiCanon_DamageReaction }, // ダメージ直後ハンドラ
    { BTL_EVENT_WAZASEQ_END,       handler_KutibasiCanon_WazaSeqEnd     }, // ワザ処理終了ハンドラ
    { BTL_EVENT_REPLACE_ACT_WAZA,  handler_KutibasiCanon_ReplaceWaza    }, // ワザ処理開始ハンドラ
    { BTL_EVENT_ACTPROC_CANCELED,  handler_KutibasiCanon_Canceled       }, // アクション処理キャンセルハンドラ
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD, handler_KutibasiCanon_ExeCheck  }, // 失敗チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// ターン最初のワザシーケンス直前
void handler_KutibasiCanon_BeforeFight( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) != pokeID )
  {
    return;
  }

  // フリーフォールで掴まれている状態なら発動しない
  if( flowWk->Hnd_IsFreeFallPoke(pokeID) ) {
    return;
  }

  // ためエフェクト
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( !bpp->IsWazaHide() )
    {
      BTL_HANDEX_PARAM_ADD_EFFECT* eff_param;
      eff_param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_EFFECT, pokeID ));
        eff_param->effectNo = BTLEFF_KUTIBASICANON_TAME;
        eff_param->pos_from = flowWk->Hnd_PokeIDtoPokePos( pokeID );
        eff_param->pos_to = BTL_POS_NULL;
        eff_param->fMsgWinVanish = true;
      flowWk->HANDEX_Pop( eff_param );
    }
  }

  //「クチバシを　かねつ　しはじめた！」
  {
    BTL_HANDEX_PARAM_MESSAGE* msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_KutibasiCanon );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    flowWk->HANDEX_Pop( msg_param );
  }

  // ハンドラの貼り付き開始
  work[ WORKIDX_STICK ] = 1;
}

// ダメージ直後ハンドラ
void handler_KutibasiCanon_DamageReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  // 技を受けるのが自分でないなら、何もしない
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) != pokeID )
  {
    return;
  }

  //「みがわり」が存在するなら、何もしない
  if( EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) )
  {
    return;
  }

  // 受ける技が接触技でないなら、何もしない
  if( !EVENTVAR_GetValue(BTL_EVAR_WAZA_TOUCH_FLAG) )
  {
    return;
  }

  // 攻撃したポケモンを「やけど」にする
  {
    BTL_HANDEX_PARAM_ADD_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
      param->sickID    = pml::wazadata::WAZASICK_YAKEDO;
      param->sickCont  = calc::MakeDefaultPokeSickCont( static_cast<PokeSick>( pml::wazadata::WAZASICK_YAKEDO ), pokeID );
      param->sickCause = SICK_CAUSE_TOKUSEI_TOUCH_COUNTER;
      param->fAlmost   = false;   // 「やけど」にならなかった場合に、その旨をメッセージで表示しない
      param->pokeID    = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    flowWk->HANDEX_Pop( param );
  }
}

// ワザ処理終了ハンドラ
void handler_KutibasiCanon_WazaSeqEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( !HandCommon_IsMyEvent(myHandle, BTL_EVAR_POKEID, pokeID) )
  {
    return;
  }

  // ハンドラの貼り付き終了
  work[ WORKIDX_STICK ] = 0;
}

// ワザ書き換え処理ハンドラ（アンコール効果）
void handler_KutibasiCanon_ReplaceWaza( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  // 張り付き中、「くちばしキャノン」以外のワザを使おうとしたら張り付き解除し、カウンター効果も無効化する。
  // （アンコールであり得る）
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_ORG_WAZAID) == EVENT_FACTOR_GetSubID(myHandle))
  ){
    work[ WORKIDX_STICK ] = 0;
    EVENT_FACTOR_Remove( myHandle );
  }
}
// アクション処理キャンセルハンドラ（フリーフォール効果などによる）
void handler_KutibasiCanon_Canceled( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  // フリーフォールなどによりワザ実行をキャンセルされた時、張り付き解除し、カウンター効果も無効化する。
  if( HandCommon_IsMyEvent(myHandle, BTL_EVAR_POKEID, pokeID) )
  {
    work[ WORKIDX_STICK ] = 0;
    EVENT_FACTOR_Remove( myHandle );
  }
}
// 失敗チェックハンドラ
void handler_KutibasiCanon_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( HandCommon_IsMyEvent(myHandle, BTL_EVAR_POKEID, pokeID) )
  {
    // 貼り付き解除されていたら失敗
    if( (work[ WORKIDX_STICK ] == 0)
    &&  (EVENTVAR_GetValue(BTL_EVAR_FAIL_CAUSE) == ServerFlow::SV_WAZAFAIL_NULL)
    ){
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
    }
  }
}




//----------------------------------------------------------------------------------
/**
 * トラップシェル
 *
 * ターンのはじめにトラップを仕掛ける。
 * そのターン内で物理技を当てられると爆発して技が発動する。
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_TrapShell( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_BEFORE_FIGHT,           handler_TrapShell_BeforeFight   },  // ターン最初のワザシーケンス直前
    { BTL_EVENT_DAMAGEPROC_END_HIT_L5,  handler_TrapShell_DamageProcEnd },  // ダメージワザ処理終了処理ハンドラ
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD, handler_TrapShell_ExeCheck      },  // ワザ出し成否チェック
    { BTL_EVENT_WAZASEQ_END,            handler_TrapShell_WazaEnd       },  // ワザ処理終了ハンドラ
    { BTL_EVENT_ACTPROC_CANCELED,       handler_KutibasiCanon_Canceled  },  // アクション処理キャンセルハンドラ（くちばしキャノンと同様）
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// ターン最初のワザシーケンス直前
void handler_TrapShell_BeforeFight( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  if( !HandCommon_IsMyEvent(myHandle, BTL_EVAR_POKEID, pokeID) )
  {
    return;
  }

  // フリーフォールで掴まれている状態なら発動しない
  if( flowWk->Hnd_IsFreeFallPoke(pokeID) ) {
    return;
  }

  // 溜めエフェクト
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( !bpp->IsWazaHide() )
    {
      BTL_HANDEX_PARAM_ADD_EFFECT* eff_param;
      eff_param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_EFFECT, pokeID ));
        eff_param->effectNo = BTLEFF_TRAPSHELL_TAME;
        eff_param->pos_from = flowWk->Hnd_PokeIDtoPokePos( pokeID );
        eff_param->pos_to = BTL_POS_NULL;
        eff_param->fMsgWinVanish = true;
      flowWk->HANDEX_Pop( eff_param );
    }
  }

  //「トラップシェルを　しかけた！」
  {
    BTL_HANDEX_PARAM_MESSAGE* msg_param;
    msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_TrapShell_Start );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    flowWk->HANDEX_Pop( msg_param );
  }

  // トラップシェル待機中フラグをセット
  {
    BTL_HANDEX_PARAM_TURNFLAG* param = reinterpret_cast<BTL_HANDEX_PARAM_TURNFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_TURNFLAG, pokeID ));
      param->pokeID = pokeID;
      param->flag = BTL_POKEPARAM::TURNFLG_TRAPPSHELL_READY;
    flowWk->HANDEX_Pop( param );
  }

  // ハンドラの張り付き開始
  work[ WORKIDX_STICK ] = 1;
}

// ダメージワザ処理終了処理ハンドラ
void handler_TrapShell_DamageProcEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  // 自身がターゲットに含まれているかチェック
  bool amITarget = false;
  u32 targetCnt = EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
  for( u32 i=0; i<targetCnt; ++i )
  {
    u8 targetPokeID = EVENTVAR_GetValue( (BtlEvVarLabel)( BTL_EVAR_POKEID_TARGET1 + i ) );
    if( targetPokeID == pokeID )
    {
      amITarget = true;
    }
  }

  // 自身がターゲットでないなら、何もしない
  if( !amITarget )
  {
    return;
  }

  // 物理技でないなら、何もしない
  if( EVENTVAR_GetValue( BTL_EVAR_DAMAGE_TYPE ) != pml::wazadata::DMG_PHYSIC )
  {
    return;
  }

  // 技を撃ったのが味方なら、何もしない
  if( flowWk->Hnd_GetMainModule()->IsFriendPokeID( pokeID, EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK ) ) )
  {
    return;
  }

  // 自身の行動順番を繰り上げる
  {
    BTL_HANDEX_PARAM_INTR_POKE* param = reinterpret_cast<BTL_HANDEX_PARAM_INTR_POKE *>( flowWk->HANDEX_Push( BTL_HANDEX_INTR_POKE, pokeID ));
      param->pokeID = pokeID;
    flowWk->HANDEX_Pop( param );
  }

  // 技を受けたことを記憶
  work[0] = 1;
}

// ワザ出し成否チェック
void handler_TrapShell_ExeCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( myHandle );

  if( !HandCommon_IsMyEvent(myHandle, BTL_EVAR_POKEID, pokeID) )
  {
    return;
  }

  // 物理技を受けていなければ失敗
  if( work[0] != 1 )
  {
    if( EVENTVAR_GetValue( BTL_EVAR_FAIL_CAUSE ) == ServerFlow::SV_WAZAFAIL_NULL )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_TRAPSHELL );
    }
  }


  // 貼り付き解除されていたら失敗
  if( work[ WORKIDX_STICK ] == 0 )
  {
    if( EVENTVAR_GetValue( BTL_EVAR_FAIL_CAUSE ) == ServerFlow::SV_WAZAFAIL_NULL )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_TRAPSHELL );
    }
  }

}

// ワザ処理終了ハンドラ
void handler_TrapShell_WazaEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);

  if( !HandCommon_IsMyEvent(myHandle, BTL_EVAR_POKEID, pokeID) )
  {
    return;
  }

  // トラップシェル待機中フラグをリセット
  {
    BTL_HANDEX_PARAM_TURNFLAG* flg_param = reinterpret_cast<BTL_HANDEX_PARAM_TURNFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_RESET_TURNFLAG, pokeID ));
      flg_param->pokeID = pokeID;
      flg_param->flag = BTL_POKEPARAM::TURNFLG_TRAPPSHELL_READY;
    flowWk->HANDEX_Pop( flg_param );
  }

  // ハンドラの張り付き解除
  work[ WORKIDX_STICK ] = 0;
}

//----------------------------------------------------------------------------------
/**
 * じだんだ
 *
 * 前のターン攻撃を外していたら威力が倍になる。
 * ・前のターン、命中率で技が外れた時、威力が上がる。
 * ・前のターン、特性で効果がなかった時、威力が上がる。
 * ・前のターン、タイプで効果がなかった時、威力が上がる。
 * ・まもるで攻撃を避けられてもあがらない。
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Zidanda( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER_BASE,   handler_Zidanda_Dmg      },  // ダメージ計算最終チェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// ダメージ計算最終チェックハンドラ
void handler_Zidanda_Dmg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID )
  {
    return;
  }

  // 前ターン、まもる系のワザ効果以外でこちらのワザが無効だったら威力２倍
  if(  (flowWk->Hnd_CheckActionRecord( pokeID, 1, ActionRecorder::ACTION_WAZA_FAILED_MASTER))
  &&  !(flowWk->Hnd_CheckActionRecord( pokeID, 1, ActionRecorder::ACTION_WAZA_FAILED_GUARD_WAZA))
  ){
    HandCommon_MulWazaBasePower( 2 );
  }
}

//----------------------------------------------------------------------------------
/**
 * オーロラベール
 *
 * ５ターンの間、敵の物理攻撃、特殊攻撃のダメージを半減させる。
 * 天気があられ状態の時にしか効果がない。
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_AuroraVeil( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_3RD,      handler_AuroraVeil_CheckExe },  // ワザ出し成功判定
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET, handler_AuroraVeil          },  // 未分類ワザ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// 成否チェック
void handler_AuroraVeil_CheckExe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) != pokeID )
  {
    return;
  }

  // 天候が「あられ」でないなら失敗
  const BtlWeather weather = flowWk->Hnd_GetWeather();
  if( weather != BTL_WEATHER_SNOW )
  {
    if( EVENTVAR_GetValue( BTL_EVAR_FAIL_CAUSE ) == ServerFlow::SV_WAZAFAIL_NULL )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, ServerFlow::SV_WAZAFAIL_OTHER );
    }
  }
}

// 未分類ワザ
void handler_AuroraVeil( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  BTL_SICKCONT  cont = SICKCONT_MakeTurn( pokeID, 5 );
  BtlSide side = flowWk->Hnd_GetMainModule()->PokeIDtoSide( pokeID );
  common_SideEffectStdMsg( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_AURORAVEIL, cont, BTL_STRID_STD_AuroraVeil );
}

//----------------------------------------------------------------------------------
/**
 * さいはい
 *
 * 指定した相手に技を出させる。
 * 相手が出す技は前回出した技。
 * 対象選択がある技は前回出した時の対象相手に技を出す。
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Saihai( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Saihai }, // 未分類ワザハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// 未分類ワザハンドラ
void handler_Saihai( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK ) != pokeID )
  {
    return;
  }

  const u8 saihaiTargetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
  const BTL_POKEPARAM* saihaiTarget = flowWk->Hnd_GetPokeParam( saihaiTargetPokeID );
  const WazaID prevWaza = saihaiTarget->GetPrevOrgWazaID();

  if( ( tables::IsSaihaiOmmit( prevWaza ) ) ||
      ( WAZADATA_GetFlag( prevWaza, pml::wazadata::WAZAFLAG_Tame ) ) ||
      ( WAZADATA_GetFlag( prevWaza, pml::wazadata::WAZAFLAG_Tire ) ) ||  // @fix NMCat[4242]
      ( saihaiTarget->WAZA_GetPP_ByNumber( prevWaza ) == 0 ) ||
      ( saihaiTarget->IsPrevWazaZenryoku() ) ||
      ( flowWk->Hnd_IsFreeFallPoke( saihaiTargetPokeID ) ) ||
      ( saihaiTarget->CONTFLAG_Get( BTL_POKEPARAM::CONTFLG_CANT_ACTION ) ) ||
      ( saihaiTarget->TURNFLAG_Get( BTL_POKEPARAM::TURNFLG_CANT_ACTION ) ) ||
      ( saihaiTarget->TURNFLAG_Get( BTL_POKEPARAM::TURNFLG_KIAI_READY ) ) ||
      ( saihaiTarget->TURNFLAG_Get( BTL_POKEPARAM::TURNFLG_TRAPPSHELL_READY ) ) )
  {
    return;
  }

  //「さいはい」対象のポケモンの技アクションを追加
  {
    BTL_HANDEX_PARAM_INSERT_WAZA_ACTION* param = reinterpret_cast<BTL_HANDEX_PARAM_INSERT_WAZA_ACTION *>( flowWk->HANDEX_Push( BTL_HANDEX_INSERT_WAZA_ACTION, pokeID ));
      param->actPokeID = saihaiTargetPokeID;
      param->actWazaNo = prevWaza;
      param->targetPos = saihaiTarget->GetPrevTargetPos(); 
      param->actionDesc.isSaihaiReaction = true;
      HANDEX_STR_Setup( &param->successMessage, BTL_STRTYPE_SET, BTL_STRID_SET_Saihai );  //「○○の指示で　××は　技を繰り出した！」
      HANDEX_STR_AddArg( &param->successMessage, pokeID );
      HANDEX_STR_AddArg( &param->successMessage, saihaiTargetPokeID );
    flowWk->HANDEX_Pop( param );
  }
}

//----------------------------------------------------------------------------------
/**
 * オリジンズスーパーノヴァ
 *
 * 場が「サイコフィールド」状態になる
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_OriginsSuperNova( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = 
  {
    { BTL_EVENT_DAMAGEPROC_END_HIT_L1, handler_OriginsSuperNova_AfterDamage  }, // ダメージ処理終了後
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// ダメージ処理終了後ハンドラ
void handler_OriginsSuperNova_AfterDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);

  // 自分が使い手で「ちからづく」なら効果なし
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  EVENTVAR_GetValue(BTL_EVAR_TIKARAZUKU_FLG)
  ){
      return;
  }

  handler_common_GroundSet( myHandle, flowWk, pokeID, work, BTL_GROUND_PHYCHO, BTL_STRID_STD_PhychoField_Start );
}

  
//------------------------------------------------------------------------------
/**
 * メテオドライブ・シャドーレイ
 *
 * かたやぶり状態で攻撃できる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_MeteorDrive( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = 
  {
    { BTL_EVENT_WAZASEQ_START,  handler_MeteorDrive_WazaSeqStart },  // ワザ処理開始ハンドラ
    { BTL_EVENT_WAZASEQ_END,    handler_MeteorDrive_WazaSeqEnd   },  // ワザ処理終了ハンドラ
  };

  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// ワザ処理開始ハンドラ
void handler_MeteorDrive_WazaSeqStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    EVENT_FACTOR_AttachSkipCheckHandler( myHandle, HandCommon_Katayaburi_SkipCheck );
  }
}

// ワザ処理終了ハンドラ
void handler_MeteorDrive_WazaSeqEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    EVENT_FACTOR_DettachSkipCheckHandler( myHandle );
  }
}

//----------------------------------------------------------------------------------
/**
 * シャドースチール
 *
 * 相手の能力アップを奪って攻撃する
 */
//----------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_ShadowSteal( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START, handler_ShadowSteal_DamageProcStart },  // ダメージ確定ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// ダメージ確定ハンドラ
void handler_ShadowSteal_DamageProcStart( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED( myHandle );
  GFL_UNUSED( work );

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID )
  {
    return;
  }

  if( EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT ) == 0 )
  {
    return;
  }

  const u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
  const BTL_POKEPARAM* targetPoke = flowWk->Hnd_GetPokeParam( targetPokeID );
  bool isStealEnable = false;

  // 現在の能力ランク一覧を取得( 0ならフラット )
  s32 targetRank[ BTL_POKEPARAM::BPP_RANKVALUE_RANGE + 1 ];
  for( u32 i=BTL_POKEPARAM::BPP_RANKVALUE_START; i<=BTL_POKEPARAM::BPP_RANKVALUE_END; ++i )
  {
    BTL_POKEPARAM::ValueID valueId = static_cast<BTL_POKEPARAM::ValueID>( i );
    targetRank[ valueId ] = targetPoke->GetValue( valueId ) - BTL_POKEPARAM::RANK_STATUS_DEFAULT;

    if( 0 < targetRank[ valueId ] )
    {
      isStealEnable = true;
    }
  }

  if( isStealEnable )
  {
    // ためエフェクト
    {
      const BTL_POKEPARAM* attacker = flowWk->Hnd_GetPokeParam( pokeID );
      if( !attacker->IsWazaHide() )
      {
        BTL_HANDEX_PARAM_ADD_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_EFFECT, pokeID ));
          param->effectNo = BTLEFF_SHADOWSTEAL;
          param->pos_from = flowWk->Hnd_PokeIDtoPokePos( pokeID );
          param->pos_to = flowWk->Hnd_PokeIDtoPokePos( targetPokeID );
          param->fMsgWinVanish = true;
        flowWk->HANDEX_Pop( param );
      }
    }

    // 防御側の上昇している能力ランクをフラットにする
    {
      BTL_HANDEX_PARAM_RESET_RANKUP* param = reinterpret_cast<BTL_HANDEX_PARAM_RESET_RANKUP *>( flowWk->HANDEX_Push( BTL_HANDEX_RESET_RANKUP, pokeID ));
        param->pokeID = targetPokeID;
      flowWk->HANDEX_Pop( param );
    }

    //「あがった　のうりょくを　うばいとった！」
    {
      BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_ShadowSteal ); 
        HANDEX_STR_AddArg( &param->str, pokeID );
      flowWk->HANDEX_Pop( param );
    }

    // 攻撃側の能力ランクを上げる
    for( u32 i=BTL_POKEPARAM::BPP_RANKVALUE_START; i<=BTL_POKEPARAM::BPP_RANKVALUE_END; ++i )
    {
      if( 0 < targetRank[i] )
      {
        BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
          param->poke_cnt    = 1;
          param->pokeID[0]   = pokeID;
          param->rankType    = static_cast<WazaRankEffect>( i );
          param->rankVolume  = targetRank[i];
          param->fWazaEffect = true;
        flowWk->HANDEX_Pop( param );
      }
    }
  }
}

//----------------------------------------------------------------------------------
/**
* ぜんりょくすとーんえっじ
* フィールド効果を打ち消す
*/
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_FieldDelete(u32* numElems)
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_L1, handler_FieldDelete },  // フィールド効果削除
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
void handler_FieldDelete(BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work)
{
  GFL_UNUSED(work);
  GFL_UNUSED(myHandle);

  if (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  {
    // フィールドが何か展開されている時のみ
    if (flowWk->Hnd_CheckFieldEffect(FieldStatus::EFF_GROUND))
    {
      // フィールド効果削除
      BTL_HANDEX_PARAM_REMOVE_FLDEFF* param;
      param = reinterpret_cast<BTL_HANDEX_PARAM_REMOVE_FLDEFF *>(flowWk->HANDEX_Push(BTL_HANDEX_REMOVE_FLDEFF, pokeID));
      param->effect = FieldStatus::EFF_GROUND;
      flowWk->HANDEX_Pop(param);
    }
  }
}

//----------------------------------------------------------------------------------
/**
* プリズムアタック
*
* ダメージタイプが特殊の技だが、技を使用する時に自身の「こうげき」と「とくこう」のパラメータを比べて、
* 「こうげき」の方が高い場合は、ダメージタイプが攻撃に変化します。
* ※技や、道具の効果で、パラメータが変動している場合は、その変動も考慮して比べます。
* 追加効果で、相手の特性の効果を無視して攻撃出来ます。
*/
//----------------------------------------------------------------------------------
const BtlEventHandlerTable* ADD_PrismAttack(u32* numElems)
{
  static const BtlEventHandlerTable HandlerTable[] =
  {
    { BTL_EVENT_WAZA_PARAM, handler_PrismAttack_WazaParam },        // ワザパラメータ決定ハンドラ
    { BTL_EVENT_WAZASEQ_START, handler_MeteorDrive_WazaSeqStart },  // ワザ処理開始ハンドラ
    { BTL_EVENT_WAZASEQ_END, handler_MeteorDrive_WazaSeqEnd },      // ワザ処理終了ハンドラ
  };

  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// ワザパラメータ決定ハンドラ
void handler_PrismAttack_WazaParam(BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work)
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam(pokeID);
    // ダメージタイプが特殊の技だが、技を使用する時に自身の「こうげき」と「とくこう」のパラメータを比べて、
    //「こうげき」の方が高い場合は、ダメージタイプが攻撃に変化します。
    if (bpp->GetValue(BTL_POKEPARAM::BPP_ATTACK) > bpp->GetValue(BTL_POKEPARAM::BPP_SP_ATTACK))
    {
      EVENTVAR_RewriteValue(BTL_EVAR_DAMAGE_TYPE, pml::wazadata::DMG_PHYSIC);
    }
  }
}

#if 0
//----------------------------------------------------------------------------------
/**
* ぜんりょくじゃれつく
* 相手の全能力を１段階下げるZENRYOKUZYARETUKU
*/
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_ZenryokuZyaretuku(u32* numElems)
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_GET_RANKEFF_VALUE, handler_ZenryokuZyaretuku },  // ワザランク増減効果チェックハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ワザランク増減効果チェックハンドラ
void handler_ZenryokuZyaretuku(BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work)
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 自分がワザ使用者で
  if (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  {
    EVENTVAR_RewriteValue(BTL_EVAR_STATUS_TYPE, pml::wazadata::RANKEFF_MULTI5);
  }
}
#endif

//----------------------------------------------------------------------------------
/**
* ビックリヘッド
* 最大HPの半分のダメージを受ける
*/
//----------------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_Hanabisenyou(u32* numElems)
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_DETERMINE,     handler_Hanabisenyou_dmg_determine },  // ダメージ確定ハンドラ
    { BTL_EVENT_WAZA_EXECUTE_NO_EFFECT, handler_Hanabisenyou_no_effect }, // 効果なかったハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

// 技出し決定→ダメージ決定後の処理
void handler_Hanabisenyou_dmg_determine(BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work)
{
  // @fix MMCat[170] 特定条件の時に「ビックリヘッド/はなびせんよう」を使用すると反動ダメージが2回発生してしまう
  // BTL_EVENT_WAZA_DMG_DETERMINEはダメージを受けたポケモン分呼ばれるため、上記のバグが起きてしまっていた
  if (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID && work[0] == 0)
  {
    common_HanabisenyouReaction(myHandle, flowWk, pokeID, work);
  }
}

// 技出し決定後に無効化された（ダメージなし）後の処理
void handler_Hanabisenyou_no_effect(BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work)
{
  if (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  {
    common_HanabisenyouReaction(myHandle, flowWk, pokeID, work);
  }
}

/**
*  ビックリヘッドダメージ処理共通
*/
void  common_HanabisenyouReaction(BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work)
{
  GFL_UNUSED(myHandle);
  // @fix MMCat[170] 特定条件の時に「ビックリヘッド/はなびせんよう」を使用すると反動ダメージが2回発生してしまう
  // 反動ダメージは一回のみ
  work[0] = 1;

  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam(pokeID);
  // 最大HPが奇数又は、計算結果が０の場合は、計算結果＋１となる
  u32 damage = calc::MulRatioInt(bpp->GetValue(BTL_POKEPARAM::BPP_MAX_HP), 50);

  BTL_HANDEX_PARAM_DAMAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_DAMAGE*>
    (flowWk->HANDEX_Push(BTL_HANDEX_DAMAGE, pokeID));
  param->pokeID = pokeID;
  param->damage = damage;
  param->damageCause = DAMAGE_CAUSE_OTHER;
  param->damageCausePokeID = pokeID;
  flowWk->HANDEX_Pop(param);

}

} // end of namespace 'waza'
} // end of namespace 'handler'
GFL_NAMESPACE_END(btl)




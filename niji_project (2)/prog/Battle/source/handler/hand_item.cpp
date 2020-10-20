//=============================================================================================
/**
 * @file  hand_item.cpp
 * @brief ポケモンXY バトルシステム イベントファクター [アイテム]
 * @author  taya
 *
 * @date  2011.01.18  作成
 */
//=============================================================================================
#include <niji_conv_header/sound/SoundMiddleID.h>
#include <niji_conv_header/message/msg_btl_std.h>
#include <niji_conv_header/message/msg_btl_set.h>

#include "../btl_calc.h"
#include "../btl_tables.h"
#include "./hand_common.h"
#include "./hand_item.h"


GFL_NAMESPACE_BEGIN(btl)
namespace handler {
namespace item    {


enum {
  WORKIDX_TMP_FLAG = EVENT_HANDLER_WORK_ELEMS-1,  ///< 一時利用アイテム
};


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
BTL_EVENT_FACTOR* addItemEventCore( const BTL_POKEPARAM* bpp, u16 itemID );
bool item_IsExePer( ServerFlow* flowWk, u8 per );
s32 common_GetItemParam( BTL_EVENT_FACTOR* myHandle, u16 paramID );
fx32 item_AttackValue_to_Ratio( BTL_EVENT_FACTOR* myHandle );
void itemPushRun( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID );
void handler_KuraboNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KuraboNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KuraboNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_KuraboNomi( u32* numElems );
void handler_KagoNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KagoNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KagoNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_KagoNomi( u32* numElems );
void handler_ChigoNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_ChigoNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_ChigoNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_ChigoNomi( u32* numElems );
void handler_NanasiNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_NanasiNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_NanasiNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_NanasiNomi( u32* numElems );
void handler_KiiNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KiiNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KiiNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_KiiNomi( u32* numElems );
void handler_MomonNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MomonNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MomonNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_MomonNomi( u32* numElems );
void handler_RamNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_RamNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_RamNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_RamNomi( u32* numElems );
void common_sickReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, BtlWazaSickEx sickCode );
void common_useForSick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, BtlWazaSickEx sickCode );
bool common_sickcode_match( ServerFlow* flowWk, u8 pokeID, BtlWazaSickEx sickCode );
const BtlEventHandlerTable* HAND_ADD_ITEM_HimeriNomi( u32* numElems );
void handler_HimeriNomi_wazaEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_HimeriNomi_reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_HimeriNomi_get( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_HimeriNomi_ppDec( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_HimeriNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_HimeriNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
u8 common_Himeri_LastWazaIdx( ServerFlow* flowWk, u8 pokeID );
u8 common_Himeri_EnableWazaIdx( ServerFlow* flowWk, u8 pokeID );
bool handler_HimeriNomi_common( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work, bool fUseTmp );
u8 handler_HimeriNomi_GetTargetWazaIndex( ServerFlow* flowWk, u8 pokeID, bool fUseTmp );
const BtlEventHandlerTable* HAND_ADD_ITEM_OrenNomi( u32* numElems );
void handler_OrenNomi_Reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_OrenNomi_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_OrenNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_KinomiJuice( u32* numElems );
const BtlEventHandlerTable* HAND_ADD_ITEM_ObonNomi( u32* numElems );
void handler_ObonNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_ObonNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_TarapuNoMi( u32* numElems );
void handler_TarapuNoMi_Reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_TarapuNoMi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_FiraNomi( u32* numElems );
void handler_FiraNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_FiraNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_UiNomi( u32* numElems );
void handler_UiNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_UiNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_MagoNomi( u32* numElems );
void handler_MagoNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MagoNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_BanziNomi( u32* numElems );
void handler_BanjiNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_BanjiNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_IaNomi( u32* numElems );
void handler_IaNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_IaNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_common_KaifukuKonran_Reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_common_KaifukuKonran_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void common_KaifukuKonran( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work, pml::pokepara::Taste taste );
const BtlEventHandlerTable* HAND_ADD_ITEM_TiiraNomi( u32* numElems );
void handler_TiiraNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_TiiraNomi_Tmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_RyugaNomi( u32* numElems );
void handler_RyugaNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_RyugaNomi_Tmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_KamuraNomi( u32* numElems );
void handler_KamuraNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KamuraNomi_Tmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_YatapiNomi( u32* numElems );
void handler_YatapiNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_YatapiNomi_Tmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_ZuaNomi( u32* numElems );
void handler_ZuaNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_ZuaNomi_Tmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_SanNomi( u32* numElems );
void handler_SanNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SanNomi_Tmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_SutaaNomi( u32* numElems );
void handler_SutaaNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SutaaNomi_Tmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void common_PinchRankup( ServerFlow* flowWk, u8 pokeID, WazaRankEffect rankType, u8 volume );
const BtlEventHandlerTable* HAND_ADD_ITEM_NazoNomi( u32* numElems );
void handler_NazoNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_NazoNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_OkkaNomi( u32* numElems );
void handler_OkkaNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_ItokeNomi( u32* numElems );
void handler_ItokeNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_SokunoNomi( u32* numElems );
void handler_SokunoNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_RindoNomi( u32* numElems );
void handler_RindoNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_YacheNomi( u32* numElems );
void handler_YacheNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_YopuNomi( u32* numElems );
void handler_YopuNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_BiarNomi( u32* numElems );
void handler_BiarNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_SyukaNomi( u32* numElems );
void handler_SyukaNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_BakouNomi( u32* numElems );
void handler_BakouNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_UtanNomi( u32* numElems );
void handler_UtanNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_TangaNomi( u32* numElems );
void handler_TangaNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_YorogiNomi( u32* numElems );
void handler_YorogiNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_KasibuNomi( u32* numElems );
void handler_KasibuNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_HabanNomi( u32* numElems );
void handler_HabanNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_NamoNomi( u32* numElems );
void handler_NamoNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_RiribaNomi( u32* numElems );
void handler_RiribaNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_HozuNomi( u32* numElems );
void handler_HozuNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_RozeruNomi( u32* numElems );
void handler_RozeruNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
bool common_WeakAff_Relieve( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work, pml::PokeType type, bool fIgnoreAffine );
void handler_common_WeakAff_DmgAfter( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_PinchReactCommon( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_PinchReactMemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void common_DamageReact( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, u32 n, bool fCheckReactionType );
bool common_DamageReactCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, u32 n );
bool common_DamageReactCheckCore( ServerFlow* flowWk, u8 pokeID, u32 n );
const BtlEventHandlerTable* HAND_ADD_ITEM_IbanNomi( u32* numElems );
void handler_IbanNomi_SpPriorityCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_IbanNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_MikuruNomi( u32* numElems );
void handler_MikuruNomi_Reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MikuruNomi_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MikuruNomi_ActProcEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MikuruNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MikuruNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_JapoNomi( u32* numElems );
void handler_JapoNomi_Damage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_RenbuNomi( u32* numElems );
void handler_RenbuNomi_Damage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void common_JapoRenbu_Reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work, WazaDamageType dmgType );
void handler_JapoNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_JyakutenHoken( u32* numElems );
void handler_JyakutenHoken_Damage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_JyakutenHoken_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_SiroiHerb( u32* numElems );
void handler_SiroiHerb_ActCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SiroiHerb_MemberInComp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SiroiHerb_ActEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SiroiHerb_TurnCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SiroiHerb_Reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SiroiHerb_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SiroiHerb_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_MentalHerb( u32* numElems );
void handler_MentalHerb_React( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MentalHerb_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MentalHerb_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_HikarinoKona( u32* numElems );
void handler_HikarinoKona( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_KyouseiGipusu( u32* numElems );
void handler_KyouseiGipusu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_SenseiNoTume( u32* numElems );
void handler_SenseiNoTume_SpPriorityCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SenseiNoTume_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_SenseiNoTume_TurnCheckDone( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_KoukouNoSippo( u32* numElems );
void handler_KoukouNoSippo( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_OujaNoSirusi( u32* numElems );
void handler_OujaNoSirusi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_OujaNoSirusi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_SurudoiKiba( u32* numElems );
const BtlEventHandlerTable* HAND_ADD_ITEM_KoukakuLens( u32* numElems );
void handler_KoukakuLens( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_PintLens( u32* numElems );
void handler_PintLens( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_LuckyPunch( u32* numElems );
void handler_LuckyPunch( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_Naganegi( u32* numElems );
void handler_Naganegi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_FocusLens( u32* numElems );
void handler_FocusLens( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_NonkiNoOkou( u32* numElems );
void handler_NonkiNoOkou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_ChikaraNoHachimaki( u32* numElems );
void handler_ChikaraNoHachimaki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_MonosiriMegane( u32* numElems );
void handler_MonosiriMegane( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_SinkaiNoKiba( u32* numElems );
void handler_SinkaiNoKiba( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_SinkaiNoUroko( u32* numElems );
void handler_SinkaiNoUroko( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_MetalPowder( u32* numElems );
void handler_MetalPowder( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_SpeedPowder( u32* numElems );
void handler_SpeedPowder( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_KokoroNoSizuku( u32* numElems );
void handler_KokoroNoSizuku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_FutoiHone( u32* numElems );
void handler_FutoiHone( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_KodawariHachimaki( u32* numElems );
void handler_KodawariHachimaki_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_KodawariMegane( u32* numElems );
void handler_KodawariMegane_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_KodawariScarf( u32* numElems );
void handler_KodawariScarf( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Kodawari_Common_WazaExe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Kodawari_Common_ItemChange( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_KiaiNoTasuki( u32* numElems );
void handler_KiaiNoTasuki_Check( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KiaiNoTasuki_Exe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_KiaiNoHachimaki( u32* numElems );
void handler_KiaiNoHachimaki_Check( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KiaiNoHachimaki_Exe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KiaiNoHachimaki_UseItem( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_TatsujinNoObi( u32* numElems );
void handler_TatsujinNoObi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_InochiNoTama( u32* numElems );
void handler_InochiNoTama_Reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_InochiNoTama_Damage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_MetroNome( u32* numElems );
void handler_MetroNome( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_NebariNoKagidume( u32* numElems );
void handler_NebariNoKagidume( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_KaigaraNoSuzu( u32* numElems );
void handler_KaigaraNoSuzu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_HikariNoNendo( u32* numElems );
void handler_HikariNoNendo( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_PowefulHarb( u32* numElems );
void handler_PowefulHarb_CheckTameSkip( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_PowefulHarb_FixTameSkip( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_PowefulHarb_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_Tabenokosi( u32* numElems );
void handler_Tabenokosi_Reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Tabenokosi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_KuroiHedoro( u32* numElems );
void handler_KuroiHedoro( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_AkaiIto( u32* numElems );
void handler_AkaiIto( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_KuttukiBari( u32* numElems );
void handler_KuttukiBari_DamageReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KuttukiBari_TurnCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_PowerWrist( u32* numElems );
const BtlEventHandlerTable* HAND_ADD_ITEM_PowerBelt( u32* numElems );
const BtlEventHandlerTable* HAND_ADD_ITEM_PowerLens( u32* numElems );
const BtlEventHandlerTable* HAND_ADD_ITEM_PowerBand( u32* numElems );
const BtlEventHandlerTable* HAND_ADD_ITEM_PowerAnkle( u32* numElems );
const BtlEventHandlerTable* HAND_ADD_ITEM_PowerWeight( u32* numElems );
void handler_PowerXXX_CalcAgility( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_HinotamaPlate( u32* numElems );
const BtlEventHandlerTable* HAND_ADD_ITEM_SizukuPlate( u32* numElems );
const BtlEventHandlerTable* HAND_ADD_ITEM_IkazutiPlate( u32* numElems );
const BtlEventHandlerTable* HAND_ADD_ITEM_MirodinoPlate( u32* numElems );
const BtlEventHandlerTable* HAND_ADD_ITEM_TuraranoPlate( u32* numElems );
const BtlEventHandlerTable* HAND_ADD_ITEM_KobusinoPlate( u32* numElems );
const BtlEventHandlerTable* HAND_ADD_ITEM_MoudokuPlate( u32* numElems );
const BtlEventHandlerTable* HAND_ADD_ITEM_DaitinoPlate( u32* numElems );
const BtlEventHandlerTable* HAND_ADD_ITEM_AozoraPlate( u32* numElems );
const BtlEventHandlerTable* HAND_ADD_ITEM_HusiginoPlate( u32* numElems );
const BtlEventHandlerTable* HAND_ADD_ITEM_TamamusiPlate( u32* numElems );
const BtlEventHandlerTable* HAND_ADD_ITEM_GansekiPlate( u32* numElems );
const BtlEventHandlerTable* HAND_ADD_ITEM_MononokePlate( u32* numElems );
const BtlEventHandlerTable* HAND_ADD_ITEM_RyuunoPlate( u32* numElems );
const BtlEventHandlerTable* HAND_ADD_ITEM_KowamotePlate( u32* numElems );
const BtlEventHandlerTable* HAND_ADD_ITEM_KoutetsuPlate( u32* numElems );
const BtlEventHandlerTable* HAND_ADD_ITEM_SeireiPlate( u32* numElems );
const BtlEventHandlerTable* HAND_ADD_ITEM_OokinaNekko( u32* numElems );
void handler_SeireiPlate( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_OokinaNekko( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
 const BtlEventHandlerTable*  HAND_ADD_ITEM_Kemuridama( u32* numElems );
void handler_Kemuridama( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Kemuridama_Msg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
 const BtlEventHandlerTable*  HAND_ADD_ITEM_OmamoriKoban( u32* numElems );
void handler_OmamoriKoban( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable*  HAND_ADD_ITEM_HakkinDama( u32* numElems );
void handler_HakkinDama( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_TumetaiIwa( u32* numElems );
void handler_TumetaiIwa( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_SarasaraIwa( u32* numElems );
void handler_SarasaraIwa( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_AtuiIwa( u32* numElems );
void handler_AtuiIwa( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_SimettaIwa( u32* numElems );
void handler_SimettaIwa( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void common_WazaWeatherTurnUp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, BtlWeather weather );
const BtlEventHandlerTable* HAND_ADD_ITEM_DenkiDama( u32* numElems );
void handler_DenkiDama( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_DenkiDama_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_DokudokuDama( u32* numElems );
void handler_DokudokuDama( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_DokudokuDama_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_KaenDama( u32* numElems );
void handler_KaenDama( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KaenDama_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_GinNoKona( u32* numElems );
void handler_GinNoKona( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_YawarakaiSuna( u32* numElems );
void handler_YawarakaiSuna( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_KataiIsi( u32* numElems );
void handler_KataiIsi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_KisekiNoTane( u32* numElems );
void handler_KisekiNoTane( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_KuroiMegane( u32* numElems );
void handler_KuroiMegane( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_Kuroobi( u32* numElems );
void handler_Kuroobi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_Zisyaku( u32* numElems );
void handler_Zisyaku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_MetalCoat( u32* numElems );
void handler_MetalCoat( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_SinpiNoSizuku( u32* numElems );
void handler_SinpiNoSizuku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_SurudoiKutibasi( u32* numElems );
void handler_SurudoiKutibasi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_Dokubari( u32* numElems );
void handler_Dokubari( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Dokubari_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_TokenaiKoori( u32* numElems );
void handler_TokenaiKoori( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_NoroiNoOfuda( u32* numElems );
void handler_NoroiNoOfuda( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_MagattaSupuun( u32* numElems );
void handler_MagattaSupuun( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_Mokutan( u32* numElems );
void handler_Mokutan( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_RyuunoKiba( u32* numElems );
void handler_RyuunoKiba( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_SirukuNoSukaafu( u32* numElems );
void handler_SirukuNoSukaafu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_AyasiiOkou( u32* numElems );
void handler_AyasiiOkou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_GansekiOkou( u32* numElems );
void handler_GansekiOkou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_SazanamiNoOkou( u32* numElems );
void handler_SazanamiNoOkou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_UsioNoOkou( u32* numElems );
void handler_UsioNoOkou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_OhanaNoOkou( u32* numElems );
void handler_OhanaNoOkou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void common_PowerUpSpecificType( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, pml::PokeType type );
const BtlEventHandlerTable* HAND_ADD_ITEM_Siratama( u32* numElems );
void handler_Siratama( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_Kongoudama( u32* numElems );
void handler_Kongoudama( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_KuroiTekkyuu( u32* numElems );
void handler_KuroiTekkyuu_Agility( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KuroiTekkyuu_CheckFly( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_Karuisi( u32* numElems );
void handler_Karuisi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_SinkanoKiseki( u32* numElems );
void handler_SinkanoKiseki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_GotugotuMet( u32* numElems );
void handler_GotugotuMet( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_Huusen( u32* numElems );
void handler_Huusen_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Huusen_CheckFlying( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Huusen_DamageReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Huusen_ItemSetFixed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_RedCard( u32* numElems );
void handler_RedCard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_NerainoMato( u32* numElems );
void handler_NerainoMato( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_SimetukeBand( u32* numElems );
void handler_SimetukeBand( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_Kyuukon( u32* numElems );
void handler_Kyuukon_DmgReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Kyuukon_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_HikariGoke( u32* numElems );
void handler_HikariGoke_DmgReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_HikariGoke_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_Juudenti( u32* numElems );
void handler_Juudenti_DmgReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Juudenti_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_DassyutuButton( u32* numElems );
void handler_DassyutuButton_Reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_DassyutuButton_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_HonooNoJuel( u32* numElems );
void handler_HonooNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_HonooNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_MizuNoJuel( u32* numElems );
void handler_MizuNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MizuNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_DenkiNoJuel( u32* numElems );
void handler_DenkiNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_DenkiNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_KusaNoJuel( u32* numElems );
void handler_KusaNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KusaNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_KooriNoJuel( u32* numElems );
void handler_KooriNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KooriNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_KakutouJuel( u32* numElems );
void handler_KakutouJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_KakutouJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_DokuNoJuel( u32* numElems );
void handler_DokuNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_DokuNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_JimenNoJuel( u32* numElems );
void handler_JimenNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_JimenNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_HikouNoJuel( u32* numElems );
void handler_HikouNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_HikouNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_EsperJuel( u32* numElems );
void handler_EsperJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_EsperJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_MusiNoJuel( u32* numElems );
void handler_MusiNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MusiNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_IwaNoJuel( u32* numElems );
void handler_IwaNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_IwaNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_GhostJuel( u32* numElems );
void handler_GhostJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_GhostJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_DragonJuel( u32* numElems );
void handler_DragonNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_DragonNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_AkuNoJuel( u32* numElems );
void handler_AkuNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_AkuNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_HaganeNoJuel( u32* numElems );
void handler_HaganeNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_HaganeNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_NormalJuel( u32* numElems );
void handler_NormalJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_NormalJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_YouseiJuel( u32* numElems );
void handler_YouseiJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_YouseiJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void common_Juel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work, pml::PokeType type );
void common_Juel_Power( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work, pml::PokeType type );
void handler_Juel_EndDmgSeq( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_TotugekiChokki( u32* numElems );
void handler_totugekiChokki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_AkkiNomi( u32* numElems );
void handler_AkkiNomi_DmgReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_AkkiNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_BoujinGoggle( u32* numElems );
void handler_BoujinGoggle_CalcDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_BoujinGoggle_WazaNoEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void hand_common_TypeDamageRankUp_Reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, pml::PokeType wazaType, BTL_POKEPARAM::ValueID rankValue );
const BtlEventHandlerTable* HAND_ADD_ITEM_Yukidama( u32* numElems );
void handler_Yukidama_DmgReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_Aiironotama( u32* numElems );
void handler_Aiironotama_Shinka( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_Beniironotama( u32* numElems );
void handler_Beniironotama_Shinka( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_GroundCoat( u32* numElems );
void handler_GroundCoat( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_Bibiridama( u32* numElems );
void handler_Bibiridama_RankEffectLastCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Bibiridama_RankEffectFailed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_Bibiridama_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_ElecSeed( u32* numElems );
void handler_ElecSeed_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_ElecSeed_ChangeGroundAfter( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_ElecSeed_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_PhychoSeed( u32* numElems );
void handler_PhychoSeed_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_PhychoSeed_ChangeGroundAfter( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_PhychoSeed_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_MistSeed( u32* numElems );
void handler_MistSeed_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MistSeed_ChangeGroundAfter( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_MistSeed_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
const BtlEventHandlerTable* HAND_ADD_ITEM_GrassSeed( u32* numElems );
void handler_GrassSeed_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_GrassSeed_ChangeGroundAfter( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_GrassSeed_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void common_Seed_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, BtlGround ground );
void common_Seed_ChangeGroundAfter( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, BtlGround ground );
void common_Seed_Use( ServerFlow* flowWk, u8 pokeID, WazaRankEffect rankType );
const BtlEventHandlerTable*  HAND_ADD_ITEM_BougoPad( u32* numElems );
void handler_BougoPad_SimpleDamageEnable( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_BougoPad_SimpleDamageFailed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_BougoPad_AddSick_CheckFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_BougoPad_RankEffect_LastCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_BougoPad_RankEffect_Failed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_BougoPad_TokuseiChangeCheckFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void handler_BougoPad_TokuseiChangeFailed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
void BougoPad_Guard( int* work, bool isGuardMessageEnable );
void BougoPad_GuardMessage( ServerFlow* flowWk, u8 pokeID, int* work );


// イベントハンドラ登録関数の型定義
typedef const BtlEventHandlerTable* (*pEventAddFunc)( u32* numHandlers );

const struct {
  u32             itemID;
  pEventAddFunc   func;
}EventAddFuncTbl[] = {

  { ITEM_KURABONOMI,    HAND_ADD_ITEM_KuraboNomi  },
  { ITEM_KAGONOMI,      HAND_ADD_ITEM_KagoNomi    },
  { ITEM_TIIGONOMI,     HAND_ADD_ITEM_ChigoNomi   },
  { ITEM_NANASINOMI,    HAND_ADD_ITEM_NanasiNomi  },
  { ITEM_KIINOMI,       HAND_ADD_ITEM_KiiNomi     },
  { ITEM_MOMONNOMI,     HAND_ADD_ITEM_MomonNomi   },
  { ITEM_RAMUNOMI,      HAND_ADD_ITEM_RamNomi     },
  { ITEM_HIMERINOMI,    HAND_ADD_ITEM_HimeriNomi  },
  { ITEM_ORENNOMI,      HAND_ADD_ITEM_OrenNomi    },
  { ITEM_KINOMIZYUUSU,  HAND_ADD_ITEM_KinomiJuice },
  { ITEM_OBONNOMI,      HAND_ADD_ITEM_ObonNomi    },
  { ITEM_FIRANOMI,      HAND_ADD_ITEM_FiraNomi    },
  { ITEM_UINOMI,        HAND_ADD_ITEM_UiNomi      },
  { ITEM_MAGONOMI,      HAND_ADD_ITEM_MagoNomi    },
  { ITEM_BANZINOMI,     HAND_ADD_ITEM_BanziNomi   },
  { ITEM_IANOMI,        HAND_ADD_ITEM_IaNomi      },
  { ITEM_TIIRANOMI,     HAND_ADD_ITEM_TiiraNomi   },
  { ITEM_RYUGANOMI,     HAND_ADD_ITEM_RyugaNomi   },
  { ITEM_KAMURANOMI,    HAND_ADD_ITEM_KamuraNomi  },
  { ITEM_YATAPINOMI,    HAND_ADD_ITEM_YatapiNomi  },
  { ITEM_ZUANOMI,       HAND_ADD_ITEM_ZuaNomi     },
  { ITEM_SANNOMI,       HAND_ADD_ITEM_SanNomi     },
  { ITEM_SUTAANOMI,     HAND_ADD_ITEM_SutaaNomi   },
  { ITEM_NAZONOMI,      HAND_ADD_ITEM_NazoNomi    },
  { ITEM_OKKANOMI,      HAND_ADD_ITEM_OkkaNomi    },
  { ITEM_ITOKENOMI,     HAND_ADD_ITEM_ItokeNomi   },
  { ITEM_SOKUNONOMI,    HAND_ADD_ITEM_SokunoNomi  },
  { ITEM_RINDONOMI,     HAND_ADD_ITEM_RindoNomi   },
  { ITEM_YATHENOMI,     HAND_ADD_ITEM_YacheNomi   },
  { ITEM_YOPUNOMI,      HAND_ADD_ITEM_YopuNomi    },
  { ITEM_BIAANOMI,      HAND_ADD_ITEM_BiarNomi    },
  { ITEM_SYUKANOMI,     HAND_ADD_ITEM_SyukaNomi   },
  { ITEM_BAKOUNOMI,     HAND_ADD_ITEM_BakouNomi   },
  { ITEM_UTANNOMI,      HAND_ADD_ITEM_UtanNomi    },
  { ITEM_TANGANOMI,     HAND_ADD_ITEM_TangaNomi   },
  { ITEM_YOROGINOMI,    HAND_ADD_ITEM_YorogiNomi  },
  { ITEM_KASIBUNOMI,    HAND_ADD_ITEM_KasibuNomi  },
  { ITEM_HABANNOMI,     HAND_ADD_ITEM_HabanNomi   },
  { ITEM_NAMONOMI,      HAND_ADD_ITEM_NamoNomi    },
  { ITEM_RIRIBANOMI,    HAND_ADD_ITEM_RiribaNomi  },
  { ITEM_HOZUNOMI,      HAND_ADD_ITEM_HozuNomi    },
  { ITEM_IBANNOMI,      HAND_ADD_ITEM_IbanNomi    },
  { ITEM_MIKURUNOMI,    HAND_ADD_ITEM_MikuruNomi  },
  { ITEM_ZYAPONOMI,     HAND_ADD_ITEM_JapoNomi    },
  { ITEM_RENBUNOMI,     HAND_ADD_ITEM_RenbuNomi   },
  { ITEM_SIROIHAABU,        HAND_ADD_ITEM_SiroiHerb         },
  { ITEM_MENTARUHAABU,      HAND_ADD_ITEM_MentalHerb        },
  { ITEM_HIKARINOKONA,      HAND_ADD_ITEM_HikarinoKona      },
  { ITEM_KYOUSEIGIPUSU,     HAND_ADD_ITEM_KyouseiGipusu     },
  { ITEM_SENSEINOTUME,      HAND_ADD_ITEM_SenseiNoTume      },
  { ITEM_KOUKOUNOSIPPO,     HAND_ADD_ITEM_KoukouNoSippo     },
  { ITEM_MANPUKUOKOU,       HAND_ADD_ITEM_KoukouNoSippo     },  // まんぷくおこう=こうこうのしっぽ等価
  { ITEM_OUZYANOSIRUSI,     HAND_ADD_ITEM_OujaNoSirusi      },
  { ITEM_SURUDOITUME,       HAND_ADD_ITEM_PintLens          },  // するどいツメ = ピントレンズと等価
  { ITEM_KOUKAKURENZU,      HAND_ADD_ITEM_KoukakuLens       },
  { ITEM_PINTORENZU,        HAND_ADD_ITEM_PintLens          },
  { ITEM_FOOKASURENZU,      HAND_ADD_ITEM_FocusLens         },
  { ITEM_NONKINOOKOU,       HAND_ADD_ITEM_NonkiNoOkou       },
  { ITEM_TIKARANOHATIMAKI,  HAND_ADD_ITEM_ChikaraNoHachimaki},
  { ITEM_MONOSIRIMEGANE,    HAND_ADD_ITEM_MonosiriMegane    },
  { ITEM_SINKAINOKIBA,      HAND_ADD_ITEM_SinkaiNoKiba      },
  { ITEM_SINKAINOUROKO,     HAND_ADD_ITEM_SinkaiNoUroko     },
  { ITEM_METARUPAUDAA,      HAND_ADD_ITEM_MetalPowder       },
  { ITEM_SUPIIDOPAUDAA,     HAND_ADD_ITEM_SpeedPowder       },
  { ITEM_DENKIDAMA,         HAND_ADD_ITEM_DenkiDama         },
  { ITEM_RAKKIIPANTI,       HAND_ADD_ITEM_LuckyPunch        },
  { ITEM_NAGANEGI,          HAND_ADD_ITEM_Naganegi          },
  { ITEM_KOKORONOSIZUKU,    HAND_ADD_ITEM_KokoroNoSizuku    },
  { ITEM_HUTOIHONE,         HAND_ADD_ITEM_FutoiHone         },
  { ITEM_KODAWARIHATIMAKI,  HAND_ADD_ITEM_KodawariHachimaki },
  { ITEM_KUROIHEDORO,       HAND_ADD_ITEM_KuroiHedoro       },
  { ITEM_KODAWARIMEGANE,    HAND_ADD_ITEM_KodawariMegane    },
  { ITEM_KODAWARISUKAAHU,   HAND_ADD_ITEM_KodawariScarf     },
  { ITEM_GINNOKONA,         HAND_ADD_ITEM_GinNoKona         },
  { ITEM_YAWARAKAISUNA,     HAND_ADD_ITEM_YawarakaiSuna     },
  { ITEM_KATAIISI,          HAND_ADD_ITEM_KataiIsi          },
  { ITEM_KISEKINOTANE,      HAND_ADD_ITEM_KisekiNoTane      },
  { ITEM_KUROIMEGANE,       HAND_ADD_ITEM_KuroiMegane       },
  { ITEM_KUROOBI,           HAND_ADD_ITEM_Kuroobi           },
  { ITEM_ZISYAKU,           HAND_ADD_ITEM_Zisyaku           },
  { ITEM_METARUKOOTO,       HAND_ADD_ITEM_MetalCoat         },
  { ITEM_SINPINOSIZUKU,     HAND_ADD_ITEM_SinpiNoSizuku     },
  { ITEM_SURUDOIKUTIBASI,   HAND_ADD_ITEM_SurudoiKutibasi   },
  { ITEM_SURUDOIKIBA,       HAND_ADD_ITEM_SurudoiKiba       },
  { ITEM_DOKUBARI,          HAND_ADD_ITEM_Dokubari          },
  { ITEM_TOKENAIKOORI,      HAND_ADD_ITEM_TokenaiKoori      },
  { ITEM_NOROINOOHUDA,      HAND_ADD_ITEM_NoroiNoOfuda      },
  { ITEM_MAGATTASUPUUN,     HAND_ADD_ITEM_MagattaSupuun     },
  { ITEM_MOKUTAN,           HAND_ADD_ITEM_Mokutan           },
  { ITEM_RYUUNOKIBA,        HAND_ADD_ITEM_RyuunoKiba        },
  { ITEM_SIRUKUNOSUKAAHU,   HAND_ADD_ITEM_SirukuNoSukaafu   },
  { ITEM_AYASIIOKOU,        HAND_ADD_ITEM_AyasiiOkou        },
  { ITEM_GANSEKIOKOU,       HAND_ADD_ITEM_GansekiOkou       },
  { ITEM_SAZANAMINOOKOU,    HAND_ADD_ITEM_SazanamiNoOkou    },
  { ITEM_USIONOOKOU,        HAND_ADD_ITEM_UsioNoOkou        },
  { ITEM_OHANANOOKOU,       HAND_ADD_ITEM_OhanaNoOkou       },
  { ITEM_KIAINOTASUKI,      HAND_ADD_ITEM_KiaiNoTasuki      },
  { ITEM_KIAINOHATIMAKI,    HAND_ADD_ITEM_KiaiNoHachimaki   },
  { ITEM_TATUZINNOOBI,      HAND_ADD_ITEM_TatsujinNoObi     },
  { ITEM_INOTINOTAMA,       HAND_ADD_ITEM_InochiNoTama      },
  { ITEM_METORONOOMU,       HAND_ADD_ITEM_MetroNome         },
  { ITEM_NEBARINOKAGIDUME,  HAND_ADD_ITEM_NebariNoKagidume  },
  { ITEM_KAIGARANOSUZU,     HAND_ADD_ITEM_KaigaraNoSuzu     },
  { ITEM_HIKARINONENDO,     HAND_ADD_ITEM_HikariNoNendo     },
  { ITEM_PAWAHURUHAABU,     HAND_ADD_ITEM_PowefulHarb       },
  { ITEM_TABENOKOSI,        HAND_ADD_ITEM_Tabenokosi        },
  { ITEM_DOKUDOKUDAMA,      HAND_ADD_ITEM_DokudokuDama      },
  { ITEM_KAENDAMA,          HAND_ADD_ITEM_KaenDama          },
  { ITEM_SIRATAMA,          HAND_ADD_ITEM_Siratama          },
  { ITEM_KONGOUDAMA,        HAND_ADD_ITEM_Kongoudama        },
  { ITEM_KUROITEKKYUU,      HAND_ADD_ITEM_KuroiTekkyuu      },
  { ITEM_AKAIITO,           HAND_ADD_ITEM_AkaiIto           },
  { ITEM_TUMETAIIWA,        HAND_ADD_ITEM_TumetaiIwa        },
  { ITEM_SARASARAIWA,       HAND_ADD_ITEM_SarasaraIwa       },
  { ITEM_ATUIIWA,           HAND_ADD_ITEM_AtuiIwa           },
  { ITEM_SIMETTAIWA,        HAND_ADD_ITEM_SimettaIwa        },
  { ITEM_KUTTUKIBARI,       HAND_ADD_ITEM_KuttukiBari       },
  { ITEM_PAWAARISUTO,       HAND_ADD_ITEM_PowerWrist        },
  { ITEM_PAWAABERUTO,       HAND_ADD_ITEM_PowerBelt         },
  { ITEM_PAWAARENZU,        HAND_ADD_ITEM_PowerLens         },
  { ITEM_PAWAABANDO,        HAND_ADD_ITEM_PowerBand         },
  { ITEM_PAWAAANKURU,       HAND_ADD_ITEM_PowerAnkle        },
  { ITEM_PAWAAUEITO,        HAND_ADD_ITEM_PowerWeight       },
  { ITEM_HINOTAMAPUREETO,   HAND_ADD_ITEM_HinotamaPlate     },
  { ITEM_SIZUKUPUREETO,     HAND_ADD_ITEM_SizukuPlate       },
  { ITEM_IKAZUTIPUREETO,    HAND_ADD_ITEM_IkazutiPlate      },
  { ITEM_MIDORINOPUREETO,   HAND_ADD_ITEM_MirodinoPlate     },
  { ITEM_TURARANOPUREETO,   HAND_ADD_ITEM_TuraranoPlate     },
  { ITEM_KOBUSINOPUREETO,   HAND_ADD_ITEM_KobusinoPlate     },
  { ITEM_MOUDOKUPUREETO,    HAND_ADD_ITEM_MoudokuPlate      },
  { ITEM_DAITINOPUREETO,    HAND_ADD_ITEM_DaitinoPlate      },
  { ITEM_AOZORAPUREETO,     HAND_ADD_ITEM_AozoraPlate       },
  { ITEM_HUSIGINOPUREETO,   HAND_ADD_ITEM_HusiginoPlate     },
  { ITEM_TAMAMUSIPUREETO,   HAND_ADD_ITEM_TamamusiPlate     },
  { ITEM_GANSEKIPUREETO,    HAND_ADD_ITEM_GansekiPlate      },
  { ITEM_MONONOKEPUREETO,   HAND_ADD_ITEM_MononokePlate     },
  { ITEM_RYUUNOPUREETO,     HAND_ADD_ITEM_RyuunoPlate       },
  { ITEM_KOWAMOTEPUREETO,   HAND_ADD_ITEM_KowamotePlate     },
  { ITEM_KOUTETUPUREETO,    HAND_ADD_ITEM_KoutetsuPlate     },
  { ITEM_SEIREIPUREETO,     HAND_ADD_ITEM_SeireiPlate       },
  { ITEM_OOKINANEKKO,       HAND_ADD_ITEM_OokinaNekko       },
  { ITEM_KEMURIDAMA,        HAND_ADD_ITEM_Kemuridama        },
  { ITEM_OMAMORIKOBAN,      HAND_ADD_ITEM_OmamoriKoban      },
  { ITEM_KOUUNNOOKOU,       HAND_ADD_ITEM_OmamoriKoban      },
  { ITEM_HAKKINDAMA,        HAND_ADD_ITEM_HakkinDama        },
  { ITEM_KARUISI,           HAND_ADD_ITEM_Karuisi         },  // かるいし
  { ITEM_SINKANOKISEKI,     HAND_ADD_ITEM_SinkanoKiseki   },  // しんかのきせき
  { ITEM_GOTUGOTUMETTO,     HAND_ADD_ITEM_GotugotuMet     },  // ゴツゴツメット
  { ITEM_HUUSEN,            HAND_ADD_ITEM_Huusen          },  // ふうせん
  { ITEM_REDDOKAADO,        HAND_ADD_ITEM_RedCard         },  // レッドカード
  { ITEM_NERAINOMATO,       HAND_ADD_ITEM_NerainoMato     },  // ねらいのまと
  { ITEM_SIMETUKEBANDO,     HAND_ADD_ITEM_SimetukeBand    },  // しめつけバンド
  { ITEM_KYUUKON,           HAND_ADD_ITEM_Kyuukon         },  // きゅうこん
  { ITEM_ZYUUDENTI,         HAND_ADD_ITEM_Juudenti        },  // じゅうでんち
  { ITEM_DASSYUTUBOTAN,     HAND_ADD_ITEM_DassyutuButton  },  // だっしゅつボタン
  { ITEM_HONOONOZYUERU,     HAND_ADD_ITEM_HonooNoJuel     },  // ほのおのジュエル
  { ITEM_MIZUNOZYUERU,      HAND_ADD_ITEM_MizuNoJuel      },  // みずのジュエル
  { ITEM_DENKINOZYUERU,     HAND_ADD_ITEM_DenkiNoJuel     },  // でんきのジュエル
  { ITEM_KUSANOZYUERU,      HAND_ADD_ITEM_KusaNoJuel      },  // くさのジュエル
  { ITEM_KOORINOZYUERU,     HAND_ADD_ITEM_KooriNoJuel     },  // こおりのジュエル
  { ITEM_KAKUTOUZYUERU,     HAND_ADD_ITEM_KakutouJuel     },  // かくとうジュエル
  { ITEM_DOKUNOZYUERU,      HAND_ADD_ITEM_DokuNoJuel      },  // どくのジュエル
  { ITEM_ZIMENNOZYUERU,     HAND_ADD_ITEM_JimenNoJuel     },  // じめんのジュエル
  { ITEM_HIKOUNOZYUERU,     HAND_ADD_ITEM_HikouNoJuel     },  // ひこうのジュエル
  { ITEM_ESUPAAZYUERU,      HAND_ADD_ITEM_EsperJuel       },  // エスパージュエル
  { ITEM_MUSINOZYUERU,      HAND_ADD_ITEM_MusiNoJuel      },  // むしのジュエル
  { ITEM_IWANOZYUERU,       HAND_ADD_ITEM_IwaNoJuel       },  // いわのジュエル
  { ITEM_GOOSUTOZYUERU,     HAND_ADD_ITEM_GhostJuel       },  // ゴーストジュエル
  { ITEM_DORAGONZYUERU,     HAND_ADD_ITEM_DragonJuel      },  // ドラゴンジュエル
  { ITEM_AKUNOZYUERU,       HAND_ADD_ITEM_AkuNoJuel       },  // あくのジュエル
  { ITEM_HAGANENOZYUERU,    HAND_ADD_ITEM_HaganeNoJuel    },  // はがねのジュエル
  { ITEM_NOOMARUZYUERU,     HAND_ADD_ITEM_NormalJuel      },  // ノーマルジュエル
  { ITEM_TOTUGEKITYOKKI,    HAND_ADD_ITEM_TotugekiChokki  },  // とつげきチョッキ
  { ITEM_ZYAKUTENHOKEN,     HAND_ADD_ITEM_JyakutenHoken   },  // じゃくてんほけん
  { ITEM_HIKARIGOKE,        HAND_ADD_ITEM_HikariGoke      },  // ひかりごけ
  { ITEM_ROZERUNOMI,        HAND_ADD_ITEM_RozeruNomi      },  // ロゼルのみ
  { ITEM_TARAPUNOMI,        HAND_ADD_ITEM_TarapuNoMi      },  // タラプのみ
  { ITEM_AKKINOMI,          HAND_ADD_ITEM_AkkiNomi        },  // アッキのみ
  { ITEM_BOUZINGOOGURU,     HAND_ADD_ITEM_BoujinGoggle    },  // ぼうじんゴーグル
  { ITEM_YUKIDAMA,          HAND_ADD_ITEM_Yukidama        },  // ゆきだま
  { ITEM_YOUSEIZYUERU,      HAND_ADD_ITEM_YouseiJuel      },  // ようせいジュエル
  { ITEM_AIIRONOTAMA,       HAND_ADD_ITEM_Aiironotama     },  // あいいろのたま(カイオーガ進化)
  { ITEM_BENIIRONOTAMA,     HAND_ADD_ITEM_Beniironotama   },  // べにいろのたま(グラードン進化)
  { ITEM_GURANDOKOOTO,      HAND_ADD_ITEM_GroundCoat      },  // グランドコート
  { ITEM_BIBIRIDAMA,        HAND_ADD_ITEM_Bibiridama      },  // びびりだま
  { ITEM_EREKISIIDO,        HAND_ADD_ITEM_ElecSeed        },  // エレキシード
  { ITEM_SAIKOSIIDO,        HAND_ADD_ITEM_PhychoSeed      },  // サイコシード
  { ITEM_MISUTOSIIDO,       HAND_ADD_ITEM_MistSeed        },  // ミストシード
  { ITEM_GURASUSIIDO,       HAND_ADD_ITEM_GrassSeed       },  // グラスシード
  { ITEM_BOUGOPATTO,        HAND_ADD_ITEM_BougoPad        },  // ぼうごパッド

  { 0, NULL },
};


//----------------------------------------------------------------------------------
/**
 *
 *
 * @param   bpp
 * @param   itemID
 *
 * @retval  BTL_EVENT_FACTOR*
 */
//----------------------------------------------------------------------------------
BTL_EVENT_FACTOR* AddEventCore( const BTL_POKEPARAM* bpp, u32 itemID )
{
  for(u32 i=0; i<GFL_NELEMS(EventAddFuncTbl); ++i)
  {
    if( EventAddFuncTbl[i].itemID == itemID )
    {
      u16 priority = bpp->GetValue_Base( BTL_POKEPARAM::BPP_AGILITY );
      u8 pokeID = bpp->GetID();
      u32 numHandlers;
      const BtlEventHandlerTable* handlerTable;

      handlerTable = EventAddFuncTbl[i].func( &numHandlers );
      return EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID,
              BTL_EVPRI_ITEM_DEFAULT, priority, pokeID, handlerTable, numHandlers );
    }
  }
  return NULL;
}


//=============================================================================================
/**
 * ポケモンの装備しているアイテムのハンドラを追加
 *
 * @param   bpp
 *
 * @retval  BTL_EVENT_FACTOR*
 */
//=============================================================================================
BTL_EVENT_FACTOR*  Add( const BTL_POKEPARAM* bpp )
{
  u32 itemID = bpp->GetItem();

  if( itemID != ITEM_DUMMY_DATA )
  {
    BTL_EVENT_FACTOR* factor = AddEventCore( bpp, itemID );
    TAYA_PRINT("[ItemH] ハンドラAdd pokeID=%d, item=%d, factor[%p]\n", bpp->GetID(), itemID, factor );
    return factor;
  }
  return NULL;
}
//=============================================================================================
/**
 * 特定ポケモンの「アイテム」ハンドラをシステムから全て削除
 *
 * @param   pp
 *
 */
//=============================================================================================
void  Remove( const BTL_POKEPARAM* bpp )
{
  BTL_EVENT_FACTOR *factor, *next;
  u8 pokeID = bpp->GetID();

  factor = EVENT_SeekFactor( BTL_EVENT_FACTOR_ITEM, pokeID );
  while( factor )
  {
    next = EVENT_GetNextFactor( factor );
    if( EVENT_FACTOR_GetWorkValue(factor, WORKIDX_TMP_FLAG) == 0 ){
      TAYA_PRINT(" [ItemH] ハンドラRemove pokeID=%d, itemID=%d, factor[%p] ...\n", pokeID, EVENT_FACTOR_GetSubID(factor), factor );
      EVENT_FACTOR_Remove( factor );
      BTL_PRINT(" skip\n");
    }
    else{
      BTL_PRINT(" done\n");
    }
    factor = next;
  }
}


//=============================================================================================
/**
 * 装備アイテムとは別に一時的なハンドラ追加
 *
 * @param   bpp
 * @param   itemID
 *
 * @retval  BTL_EVENT_FACTOR*
 */
//=============================================================================================
BTL_EVENT_FACTOR*  TMP_Add( const BTL_POKEPARAM* bpp, u16 itemID )
{
  if( itemID != ITEM_DUMMY_DATA )
  {
    BTL_EVENT_FACTOR* factor = addItemEventCore( bpp, itemID );
    if( factor )
    {
      EVENT_FACTOR_SetTmpItemFlag( factor );
      EVENT_FACTOR_SetWorkValue( factor, WORKIDX_TMP_FLAG, true );
      return factor;
    }
  }
  return NULL;
}
//=============================================================================================
/**
 * 一時的な追加ハンドラを削除する
 *
 * @param   bpp
 *
 * @retval  BTL_EVENT_FACTOR*
 */
//=============================================================================================
void TMP_Remove( BTL_EVENT_FACTOR* factor )
{
  EVENT_FACTOR_Remove( factor );
}
//=============================================================================================
/**
 * 指定ポケモンのアイテムハンドラを一時的に休止（ローテーション用）
 *
 * @param   bpp
 */
//=============================================================================================
void RotationSleep( const BTL_POKEPARAM* bpp )
{
  u8 pokeID = bpp->GetID();
  EVENT_SleepFactorRotation( pokeID, BTL_EVENT_FACTOR_ITEM );

}
//=============================================================================================
/**
 * 指定ポケモンのアイテムハンドラを休止から復帰（ローテーション用）
 *
 * @param   bpp
 */
//=============================================================================================
void RotationWake( const BTL_POKEPARAM* bpp )
{
  u8 pokeID = bpp->GetID();
  if( EVENT_WakeFactorRotation(pokeID, BTL_EVENT_FACTOR_ITEM) == false )
  {
    Add( bpp );
  }
}
//=============================================================================================
/**
* 指定ポケモンのアイテムハンドラを休止から復帰（momiji用）
*
* @param   bpp
*/
//=============================================================================================
void RotationWakeNoAdd( const BTL_POKEPARAM* bpp )
{
  u8 pokeID = bpp->GetID();
  EVENT_WakeFactorRotation( pokeID, BTL_EVENT_FACTOR_ITEM );
}
//----------------------------------------------------------------------------------
/**
 *
 *
 * @param   bpp
 * @param   itemID
 *
 * @retval  BTL_EVENT_FACTOR*
 */
//----------------------------------------------------------------------------------
BTL_EVENT_FACTOR* addItemEventCore( const BTL_POKEPARAM* bpp, u16 itemID )
{
  for(u32 i=0; i<GFL_NELEMS(EventAddFuncTbl); ++i)
  {
    if( EventAddFuncTbl[i].itemID == itemID )
    {
      u16 priority = bpp->GetValue_Base( BTL_POKEPARAM::BPP_AGILITY );
      u8 pokeID = bpp->GetID();
      u32 numHandlers;
      const BtlEventHandlerTable* handlerTable;

      handlerTable = EventAddFuncTbl[i].func( &numHandlers );
      return EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID,
              BTL_EVPRI_ITEM_DEFAULT, priority, pokeID, handlerTable, numHandlers );
    }
  }
  return NULL;
}

//==============================================================================================
//==============================================================================================

//----------------------------------------------------------------------------------
/**
 * ランダム発動する装備効果の共通チェックルーチン
 *
 * @param   flowWk
 * @param   per
 *
 * @retval  bool
 */
//----------------------------------------------------------------------------------
bool item_IsExePer( ServerFlow* flowWk, u8 per )
{
  if( calc::IsOccurPer(per) ){
    return true;
  }
  if( flowWk->Hnd_GetDebugFlag(BTL_DEBUGFLAG_MUST_ITEM) ){
    return true;
  }
  return false;
}
//----------------------------------------------------------------------------------
/**
 * アイテムパラメータ取得
 *
 * @param   myHandle
 * @param   paramID   パラメータID（item/item.h)
 *
 * @retval  s32
 */
//----------------------------------------------------------------------------------
s32 common_GetItemParam( BTL_EVENT_FACTOR* myHandle, u16 paramID )
{
  u16     itemID = EVENT_FACTOR_GetSubID( myHandle );
  return calc::ITEM_GetParam( itemID, paramID );
}

//----------------------------------------------------------------------------------
/**
 * アイテムパラメータの「威力値」を、威力・命中率などの倍率に変換する際の共通ルーチン
 *
 * @param   itemID    アイテムID
 *
 * @retval  fx32      「威力値」を倍率値に変換したもの。
 *                    例）威力値20 なら FX32_CONST(1.2）相当の値が返る
 */
//----------------------------------------------------------------------------------
fx32 item_AttackValue_to_Ratio( BTL_EVENT_FACTOR* myHandle )
{
  int pow = common_GetItemParam( myHandle, ::item::ITEM_DATA::PRM_ID_ATTACK );
  return FX32_CONST(100+pow) / 100;
}

//----------------------------------------------------------------------------------
/**
 * 自分自身の再呼び出し許可後、アイテム使用イベント呼び出し
 *
 * @param   myHandle
 * @param   flowWk
 * @param   pokeID
 */
//----------------------------------------------------------------------------------
void itemPushRun( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID )
{
  EVENT_FACTOR_SetRecallEnable( myHandle );
  flowWk->HANDEX_PushRun( BTL_HANDEX_USE_ITEM, pokeID );
}



//==============================================================================================
//==============================================================================================

//------------------------------------------------------------------------------
/**
 *  クラボのみ
 */
//------------------------------------------------------------------------------
void handler_KuraboNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_sickReaction( myHandle, flowWk, pokeID, static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_MAHI) );
}
void handler_KuraboNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_useForSick( myHandle, flowWk, pokeID, static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_MAHI) );
}
void handler_KuraboNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_useForSick( myHandle, flowWk, pokeID, static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_MAHI) );
}

const BtlEventHandlerTable* HAND_ADD_ITEM_KuraboNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,           handler_KuraboNomi     }, // メンバー入場チェックハンドラ
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_KuraboNomi     }, // 状態異常チェックハンドラ
    { BTL_EVENT_ACTPROC_END,         handler_KuraboNomi     },
    { BTL_EVENT_USE_ITEM,            handler_KuraboNomi_Use }, // 通常道具使用
    { BTL_EVENT_USE_ITEM_TMP,        handler_KuraboNomi_Use }, // 強制一時使用（ついばむなど）
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  カゴのみ
 */
//------------------------------------------------------------------------------
void handler_KagoNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_sickReaction( myHandle, flowWk, pokeID, static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_NEMURI) );
}
void handler_KagoNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_useForSick( myHandle, flowWk, pokeID, static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_NEMURI) );
}
void handler_KagoNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    common_useForSick( myHandle, flowWk, pokeID, static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_NEMURI) );
  }
}
const BtlEventHandlerTable* HAND_ADD_ITEM_KagoNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,           handler_KagoNomi     }, // メンバー入場チェックハンドラ
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_KagoNomi     }, // 状態異常チェックハンドラ
    { BTL_EVENT_ACTPROC_END,         handler_KagoNomi     },
    { BTL_EVENT_USE_ITEM,            handler_KagoNomi_Use }, // 通常道具使用
    { BTL_EVENT_USE_ITEM_TMP,        handler_KagoNomi_Use }, // 強制一時使用（ついばむなど）
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  チーゴのみ
 */
//------------------------------------------------------------------------------
void handler_ChigoNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_sickReaction( myHandle, flowWk, pokeID, static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_YAKEDO) );
}
void handler_ChigoNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_useForSick( myHandle, flowWk, pokeID, static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_YAKEDO) );
}
void handler_ChigoNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    common_useForSick( myHandle, flowWk, pokeID, static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_YAKEDO) );
  }
}
const BtlEventHandlerTable* HAND_ADD_ITEM_ChigoNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,           handler_ChigoNomi     },  // メンバー入場チェックハンドラ
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_ChigoNomi     },  // 状態異常チェックハンドラ
    { BTL_EVENT_ACTPROC_END,         handler_ChigoNomi     },
    { BTL_EVENT_USE_ITEM,            handler_ChigoNomi_Use },  // 通常道具使用
    { BTL_EVENT_USE_ITEM_TMP,        handler_ChigoNomi_Use },  // 強制一時使用（ついばむなど）
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  ナナシのみ
 */
//------------------------------------------------------------------------------
void handler_NanasiNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_sickReaction( myHandle, flowWk, pokeID, static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_KOORI) );
}
void handler_NanasiNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_useForSick( myHandle, flowWk, pokeID, static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_KOORI) );
}
void handler_NanasiNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    common_useForSick( myHandle, flowWk, pokeID, static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_KOORI) );
  }
}
const BtlEventHandlerTable* HAND_ADD_ITEM_NanasiNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,           handler_NanasiNomi     }, // メンバー入場チェックハンドラ
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_NanasiNomi     }, // 状態異常チェックハンドラ
    { BTL_EVENT_ACTPROC_END,         handler_NanasiNomi     },
    { BTL_EVENT_USE_ITEM,            handler_NanasiNomi_Use }, // 通常道具使用
    { BTL_EVENT_USE_ITEM_TMP,        handler_NanasiNomi_Use }, // 強制一時使用（ついばむなど）
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  キーのみ
 */
//------------------------------------------------------------------------------
void handler_KiiNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_sickReaction( myHandle, flowWk, pokeID, static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_KONRAN) );
}
void handler_KiiNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_useForSick( myHandle, flowWk, pokeID, static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_KONRAN) );
}
void handler_KiiNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    common_useForSick( myHandle, flowWk, pokeID, static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_KONRAN) );
  }
}
const BtlEventHandlerTable* HAND_ADD_ITEM_KiiNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,           handler_KiiNomi     }, // メンバー入場チェックハンドラ
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_KiiNomi     }, // 状態異常チェックハンドラ
    { BTL_EVENT_ACTPROC_END,         handler_KiiNomi     },
    { BTL_EVENT_USE_ITEM,            handler_KiiNomi_Use }, // 通常道具使用
    { BTL_EVENT_USE_ITEM_TMP,        handler_KiiNomi_Use }, // 強制一時使用（ついばむなど）
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  モモンのみ
 */
//------------------------------------------------------------------------------
void handler_MomonNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_sickReaction( myHandle, flowWk, pokeID, static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_DOKU) );
}
void handler_MomonNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_useForSick( myHandle, flowWk, pokeID, static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_DOKU) );
}
void handler_MomonNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    common_useForSick( myHandle, flowWk, pokeID, static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_DOKU) );
  }
}
const BtlEventHandlerTable* HAND_ADD_ITEM_MomonNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,           handler_MomonNomi     },  // 状態異常チェックハンドラ
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_MomonNomi     },  // 状態異常チェックハンドラ
    { BTL_EVENT_ACTPROC_END,         handler_MomonNomi     },
    { BTL_EVENT_USE_ITEM,            handler_MomonNomi_Use },  // 通常道具使用
    { BTL_EVENT_USE_ITEM_TMP,        handler_MomonNomi_Use },  // 強制一時使用（ついばむなど）
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  ラムのみ
 */
//------------------------------------------------------------------------------
void handler_RamNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_sickReaction( myHandle, flowWk, pokeID, WAZASICK_EX_POKEFULL_PLUS );
}
void handler_RamNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_useForSick( myHandle, flowWk, pokeID, WAZASICK_EX_POKEFULL_PLUS );
}
void handler_RamNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    common_useForSick( myHandle, flowWk, pokeID, WAZASICK_EX_POKEFULL_PLUS );
  }
}
const BtlEventHandlerTable* HAND_ADD_ITEM_RamNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,           handler_RamNomi     },  // 状態異常チェックハンドラ
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_RamNomi     },  // 状態異常チェックハンドラ
    { BTL_EVENT_ACTPROC_END,         handler_RamNomi     },
    { BTL_EVENT_USE_ITEM,            handler_RamNomi_Use },  // 通常道具使用
    { BTL_EVENT_USE_ITEM_TMP,        handler_RamNomi_Use },  // 強制一時使用（ついばむなど）
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//--------------------------------------------------------------------------
/**
 * 状態異常に反応するアイテム群の共通処理
 *
 * @param   flowWk
 * @param   pokeID
 * @param   sickID    反応する状態異常
 *
 */
//--------------------------------------------------------------------------
void common_sickReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, BtlWazaSickEx sickCode )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // 該当する反応タイプでなければ何もしない
    int reactionType;
    if( EVENTVAR_GetValueIfExist(BTL_EVAR_ITEM_REACTION, &reactionType) )
    {
      if( !(reactionType & BTL_ITEMREACTION_SICK) ){
        return;
      }
    }

    if( common_sickcode_match(flowWk, pokeID, sickCode) )
    {
      itemPushRun( myHandle, flowWk, pokeID );
    }
  }
}
void common_useForSick( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, BtlWazaSickEx sickCode )
{
  GFL_UNUSED(myHandle);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( common_sickcode_match(flowWk, pokeID, sickCode) )
    {
      BTL_HANDEX_PARAM_CURE_SICK* param;

      param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
        param->sickCode = sickCode;
        param->poke_cnt = 1;
        param->pokeID[0] = pokeID;
      flowWk->HANDEX_Pop( param );
    }
  }
}
bool common_sickcode_match( ServerFlow* flowWk, u8 pokeID, BtlWazaSickEx sickCode )
{
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

  switch( sickCode ){
  case WAZASICK_EX_POKEFULL_PLUS:
    if( (bpp->GetPokeSick() != pml::pokepara::SICK_NULL)
    ||  (bpp->CheckSick(pml::wazadata::WAZASICK_KONRAN))
    ){
      return true;
    }
    break;

  case WAZASICK_EX_POKEFULL:
    return  (bpp->GetPokeSick() != pml::pokepara::SICK_NULL);

  case WAZASICK_EX_MENTAL:
    {
      WazaSick sickID = calc::CheckMentalSick( bpp );
      if( sickID != pml::wazadata::WAZASICK_NULL ){
        return true;
       }
    }
    return false;

  default:
    return bpp->CheckSick( static_cast<WazaSick>(sickCode) );
  }
  return false;
}
//------------------------------------------------------------------------------
/**
 *  ヒメリのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_HimeriNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASEQ_END,            handler_HimeriNomi_wazaEnd },
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_HimeriNomi_reaction},
    { BTL_EVENT_MEMBER_IN,              handler_HimeriNomi_get     },
    { BTL_EVENT_ITEMSET_FIXED,          handler_HimeriNomi_get     },
    { BTL_EVENT_DECREMENT_PP_DONE,      handler_HimeriNomi_ppDec   },
    { BTL_EVENT_USE_ITEM,               handler_HimeriNomi_Use     },
    { BTL_EVENT_USE_ITEM_TMP,           handler_HimeriNomi_UseTmp  },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザ最後
void handler_HimeriNomi_wazaEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( common_Himeri_LastWazaIdx(flowWk, pokeID) != PTL_WAZA_MAX )
    {
      itemPushRun( myHandle, flowWk, pokeID );
    }
  }
}
// アイテム反応汎用
void handler_HimeriNomi_reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BtlItemReaction reactionType = static_cast<BtlItemReaction>(EVENTVAR_GetValue( BTL_EVAR_ITEM_REACTION ));
    if( reactionType & BTL_ITEMREACTION_PP )
    {
      if( common_Himeri_EnableWazaIdx(flowWk, pokeID) != PTL_WAZA_MAX )
      {
        itemPushRun( myHandle, flowWk, pokeID );
      }
    }
  }
}
// アイテム入手
void handler_HimeriNomi_get( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  ){
    if( common_Himeri_EnableWazaIdx(flowWk, pokeID) != PTL_WAZA_MAX )
    {
      itemPushRun( myHandle, flowWk, pokeID );
    }
  }
}


// PPが削られた
void handler_HimeriNomi_ppDec( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( common_Himeri_LastWazaIdx(flowWk, pokeID) != PTL_WAZA_MAX )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
    }
  }
}
void handler_HimeriNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    handler_HimeriNomi_common( myHandle, flowWk, pokeID, work, false );
  }
}
void handler_HimeriNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] )
  {
    handler_HimeriNomi_common( myHandle, flowWk, pokeID, work, true );
  }
}
/**
 *  ヒメリのみ用：直前に使ったワザのPPが0の場合、そのワザのIndexを返す／それ以外 PTL_WAZA_MAX を返す
 */
u8 common_Himeri_LastWazaIdx( ServerFlow* flowWk, u8 pokeID )
{
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
      // 直前に使ったワザのPPが0なら最優先
  WazaID waza = bpp->GetPrevOrgWazaID( );
  u8  wazaIdx = bpp->WAZA_SearchIdx( waza );

  if( (wazaIdx != PTL_WAZA_MAX)
  &&  (bpp->WAZA_GetPP(wazaIdx) == 0)
  ){
    return wazaIdx;
  }

  return PTL_WAZA_MAX;
}
/**
 *  ヒメリのみ用：直前に使ったワザのPPが0の場合、そのワザのIndexを返す／それ以外 PTL_WAZA_MAX を返す
 */
u8 common_Himeri_EnableWazaIdx( ServerFlow* flowWk, u8 pokeID )
{
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

  u32 waza_cnt = bpp->WAZA_GetCount( );
  u32 i;

  for(i=0; i<waza_cnt; ++i)
  {
    if(bpp->WAZA_GetPP( i ) == 0){
      return i;
    }
  }

  return PTL_WAZA_MAX;
}
/**
 *  いろんな使われ方をするヒメリのみの共通処理
 */
bool handler_HimeriNomi_common( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work, bool fUseTmp )
{
  GFL_UNUSED(work);
  
  u8 wazaIdx = handler_HimeriNomi_GetTargetWazaIndex( flowWk, pokeID, fUseTmp );

  if( wazaIdx != PTL_WAZA_MAX )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

    BTL_HANDEX_PARAM_PP* param;

    param = reinterpret_cast<BTL_HANDEX_PARAM_PP *>( flowWk->HANDEX_Push( BTL_HANDEX_RECOVER_PP, pokeID ));
      param->volume = 10;
      param->pokeID = pokeID;
      param->wazaIdx = wazaIdx;
      param->fSurfacePP = true;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_RecoverPP );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, EVENT_FACTOR_GetSubID(myHandle) );
      HANDEX_STR_AddArg( &param->exStr, bpp->WAZA_GetID( wazaIdx) );
    flowWk->HANDEX_Pop( param );
    return true;
  }
  return false;
}

/**
 * @brief 「ヒメリのみ」によってＰＰが回復する技のインデックスを取得する
 */
u8 handler_HimeriNomi_GetTargetWazaIndex( ServerFlow* flowWk, u8 pokeID, bool fUseTmp )
{
  u8 wazaIdx = PTL_WAZA_MAX;

  // 直前に使ったワザのPPが0なら最優先
  wazaIdx = common_Himeri_LastWazaIdx( flowWk, pokeID );
  if( wazaIdx != PTL_WAZA_MAX )
  {
    return wazaIdx;
  }

  // それ以外にPP=0のワザがあれば使う（0番から順サーチ）
  wazaIdx = common_Himeri_EnableWazaIdx( flowWk, pokeID );
  if( wazaIdx != PTL_WAZA_MAX )
  {
    return wazaIdx;
  }

  // ついばむ・なげつける効果の時はPPが１つでも減っていたら使う（0番から順サーチ）
  if( fUseTmp )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u32 waza_cnt = bpp->WAZA_GetCount();
    for(u32 i=0; i<waza_cnt; ++i)
    {
      if(bpp->WAZA_GetPPShort( i) != 0){
        return i;
      }
    }
  }

  return PTL_WAZA_MAX;
}

//------------------------------------------------------------------------------
/**
 *  オレンのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_OrenNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_OrenNomi_Reaction   },
    { BTL_EVENT_MEMBER_IN,              handler_OrenNomi_MemberIn   },
    { BTL_EVENT_USE_ITEM,               handler_OrenNomi_Use        },
    { BTL_EVENT_USE_ITEM_TMP,           handler_OrenNomi_Use        },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_OrenNomi_Reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);

  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  if( !bpp->CheckSick( pml::wazadata::WAZASICK_KAIHUKUHUUJI) ){
    common_DamageReact( myHandle, flowWk, pokeID, 2, true );
  }
}
void handler_OrenNomi_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  if( !bpp->CheckSick( pml::wazadata::WAZASICK_KAIHUKUHUUJI) ){
    common_DamageReact( myHandle, flowWk, pokeID, 2, false );
  }
}


void handler_OrenNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_RECOVER_HP* param = reinterpret_cast<BTL_HANDEX_PARAM_RECOVER_HP *>( flowWk->HANDEX_Push( BTL_HANDEX_RECOVER_HP, pokeID ));
      param->pokeID = pokeID;
      param->recoverHP = static_cast<u16>(common_GetItemParam( myHandle, ::item::ITEM_DATA::PRM_ID_ATTACK ));
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_RecoverHP );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, EVENT_FACTOR_GetSubID(myHandle) );
    flowWk->HANDEX_Pop( param );
  }
}
//------------------------------------------------------------------------------
/**
 *  きのみジュース
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_KinomiJuice( u32* numElems )
{
  // 投げつけ効果以外は“オレンのみ”と同じで良い
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_OrenNomi_Reaction },
    { BTL_EVENT_MEMBER_IN,              handler_OrenNomi_MemberIn },
    { BTL_EVENT_USE_ITEM,               handler_OrenNomi_Use      },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  オボンのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_ObonNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_OrenNomi_Reaction   },
    { BTL_EVENT_MEMBER_IN,              handler_OrenNomi_MemberIn   },
    { BTL_EVENT_USE_ITEM,               handler_ObonNomi_Use        },
    { BTL_EVENT_USE_ITEM_TMP,           handler_ObonNomi_Use        },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_ObonNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    TAYA_PRINT("[ItemH] オボンのみ使う pokeID=%d, factor[%p]\n", pokeID, myHandle );
    BTL_HANDEX_PARAM_RECOVER_HP* param;
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u32 maxHP = bpp->GetValue( BTL_POKEPARAM::BPP_MAX_HP );
    u8 ratio = static_cast<u8>(common_GetItemParam( myHandle, ::item::ITEM_DATA::PRM_ID_ATTACK ));

    param = reinterpret_cast<BTL_HANDEX_PARAM_RECOVER_HP *>( flowWk->HANDEX_Push( BTL_HANDEX_RECOVER_HP, pokeID ));
      param->pokeID = pokeID;
      param->recoverHP = (maxHP * ratio) / 100;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_RecoverHP );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, EVENT_FACTOR_GetSubID(myHandle) );
    flowWk->HANDEX_Pop( param );
  }
}
void handler_ObonNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    handler_ObonNomi_Use( myHandle, flowWk, pokeID, work );
  }
}

//------------------------------------------------------------------------------
/**
 *  タラプのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_TarapuNoMi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL, handler_TarapuNoMi_Reaction },  // ダメージ処理終了ハンドラ
    { BTL_EVENT_USE_ITEM,                handler_TarapuNoMi_Use      },  // どうぐ使用ハンドラ
    { BTL_EVENT_USE_ITEM_TMP,            handler_TarapuNoMi_Use      },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_TarapuNoMi_Reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  // 自分が特殊ダメージを受けた時、特攻がまだ上がるなら反応する
  if( HandCommon_CheckTargetPokeID(pokeID) )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == pml::wazadata::DMG_SPECIAL )
    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
      if( bpp->IsRankEffectValid(BTL_POKEPARAM::BPP_SP_DEFENCE_RANK, 1) )
      {
        itemPushRun( myHandle, flowWk, pokeID );
      }
    }
  }
}
void handler_TarapuNoMi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // なげつける・ついばむでココに来ることをふまえ上昇余地のチェックはもう１回やる
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    const BTL_POKEPARAM::ValueID  rankType = BTL_POKEPARAM::BPP_SP_DEFENCE_RANK;
    const int rankUpVolume = 1;
    if( bpp->IsRankEffectValid(rankType, rankUpVolume) )
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
        param->poke_cnt   = 1;
        param->pokeID[0]  = pokeID;
        param->rankType   = (WazaRankEffect)rankType;
        param->rankVolume = rankUpVolume;
      flowWk->HANDEX_Pop( param );
    }
  }
}

//------------------------------------------------------------------------------
/**
 *  フィラのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_FiraNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_common_KaifukuKonran_Reaction   },
    { BTL_EVENT_MEMBER_IN,              handler_common_KaifukuKonran_MemberIn   },
    { BTL_EVENT_USE_ITEM,               handler_FiraNomi_Use                    },
    { BTL_EVENT_USE_ITEM_TMP,           handler_FiraNomi_UseTmp                 },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_FiraNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_KaifukuKonran( myHandle, flowWk, pokeID, work, pml::pokepara::TASTE_KARAI );
}
void handler_FiraNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    common_KaifukuKonran( myHandle, flowWk, pokeID, work, pml::pokepara::TASTE_KARAI );
  }
}

//------------------------------------------------------------------------------
/**
 *  ウイのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_UiNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_common_KaifukuKonran_Reaction   },
    { BTL_EVENT_MEMBER_IN,              handler_common_KaifukuKonran_MemberIn   },
    { BTL_EVENT_USE_ITEM,               handler_UiNomi_Use                      },
    { BTL_EVENT_USE_ITEM_TMP,           handler_UiNomi_UseTmp                   },

  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_UiNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_KaifukuKonran( myHandle, flowWk, pokeID, work, pml::pokepara::TASTE_SIBUI );
}
void handler_UiNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    common_KaifukuKonran( myHandle, flowWk, pokeID, work, pml::pokepara::TASTE_SIBUI );
  }
}
//------------------------------------------------------------------------------
/**
 *  マゴのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_MagoNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_common_KaifukuKonran_Reaction   },
    { BTL_EVENT_MEMBER_IN,              handler_common_KaifukuKonran_MemberIn   },
    { BTL_EVENT_USE_ITEM,               handler_MagoNomi_Use                    },
    { BTL_EVENT_USE_ITEM_TMP,           handler_MagoNomi_UseTmp                 },

  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_MagoNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_KaifukuKonran( myHandle, flowWk, pokeID, work, pml::pokepara::TASTE_AMAI );
}
void handler_MagoNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    common_KaifukuKonran( myHandle, flowWk, pokeID, work, pml::pokepara::TASTE_AMAI );
  }
}

//------------------------------------------------------------------------------
/**
 *  バンジのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_BanziNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_common_KaifukuKonran_Reaction },
    { BTL_EVENT_MEMBER_IN,              handler_common_KaifukuKonran_MemberIn },
    { BTL_EVENT_USE_ITEM,               handler_BanjiNomi_Use                 },
    { BTL_EVENT_USE_ITEM_TMP,           handler_BanjiNomi_UseTmp              },

  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_BanjiNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_KaifukuKonran( myHandle, flowWk, pokeID, work, pml::pokepara::TASTE_NIGAI );
}
void handler_BanjiNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    common_KaifukuKonran( myHandle, flowWk, pokeID, work, pml::pokepara::TASTE_NIGAI );
  }
}

//------------------------------------------------------------------------------
/**
 *  イアのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_IaNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_common_KaifukuKonran_Reaction   },
    { BTL_EVENT_MEMBER_IN,              handler_common_KaifukuKonran_MemberIn   },
    { BTL_EVENT_USE_ITEM,               handler_IaNomi_Use                      },
    { BTL_EVENT_USE_ITEM_TMP,           handler_IaNomi_UseTmp                   },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_IaNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_KaifukuKonran( myHandle, flowWk, pokeID, work, pml::pokepara::TASTE_SUPPAI );
}
void handler_IaNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    common_KaifukuKonran( myHandle, flowWk, pokeID, work, pml::pokepara::TASTE_SUPPAI );
  }
}

/**
 *  回復こんらん木の実の共通リアクションハンドラ
 */
void handler_common_KaifukuKonran_Reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[0] == 0 )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( !bpp->CheckSick( pml::wazadata::WAZASICK_KAIHUKUHUUJI) ){
      common_DamageReact( myHandle, flowWk, pokeID, 4, true );
    }
  }
}
/**
 *  回復こんらん木の実の共通メンバー入場時ハンドラ
 */
void handler_common_KaifukuKonran_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[0] == 0 )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( !bpp->CheckSick( pml::wazadata::WAZASICK_KAIHUKUHUUJI) ){
      common_DamageReact( myHandle, flowWk, pokeID, 4, false );
    }
  }
}
/**
 *  回復こんらん木の実の共通どうぐ使用ハンドラ
 */
void common_KaifukuKonran( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work, pml::pokepara::Taste taste )
{
  if( work[0] == 0 )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
    {
      BTL_HANDEX_PARAM_RECOVER_HP* param;
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

      param = reinterpret_cast<BTL_HANDEX_PARAM_RECOVER_HP *>( flowWk->HANDEX_Push( BTL_HANDEX_RECOVER_HP, pokeID ));
        param->pokeID = pokeID;
        param->recoverHP = calc::QuotMaxHP( bpp, common_GetItemParam(myHandle, ::item::ITEM_DATA::PRM_ID_ATTACK) );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_RecoverHP );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
        HANDEX_STR_AddArg( &param->exStr, EVENT_FACTOR_GetSubID(myHandle) );
      flowWk->HANDEX_Pop( param );

      if( bpp->GetSrcData()->JudgeTaste(taste) == pml::pokepara::TASTE_JUDGE_DISLIKE )
      {
        BTL_HANDEX_PARAM_ADD_SICK* sick_param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
          sick_param->pokeID = pokeID;
          sick_param->sickID = pml::wazadata::WAZASICK_KONRAN;
          calc::MakeDefaultWazaSickCont( sick_param->sickID, bpp, &(sick_param->sickCont) );
        flowWk->HANDEX_Pop( sick_param );
      }

      work[0] = 1;
    }
  }
}

//------------------------------------------------------------------------------
/**
 *  チイラのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_TiiraNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_PinchReactCommon     },
    { BTL_EVENT_MEMBER_IN,              handler_PinchReactMemberIn   },
    { BTL_EVENT_USE_ITEM,               handler_TiiraNomi            },
    { BTL_EVENT_USE_ITEM_TMP,           handler_TiiraNomi            },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_TiiraNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_PinchRankup( flowWk, pokeID, pml::wazadata::RANKEFF_ATTACK, 1 );
}
void handler_TiiraNomi_Tmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG ] ){
    handler_TiiraNomi( myHandle, flowWk, pokeID, work );
  }
}
//------------------------------------------------------------------------------
/**
 *  リュガのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_RyugaNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_PinchReactCommon     },
    { BTL_EVENT_MEMBER_IN,              handler_PinchReactMemberIn   },
    { BTL_EVENT_USE_ITEM,               handler_RyugaNomi            },
    { BTL_EVENT_USE_ITEM_TMP,           handler_RyugaNomi            },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_RyugaNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_PinchRankup( flowWk, pokeID, pml::wazadata::RANKEFF_DEFENCE, 1 );
}
void handler_RyugaNomi_Tmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG ] ){
    handler_RyugaNomi( myHandle, flowWk, pokeID, work );
  }
}
//------------------------------------------------------------------------------
/**
 *  カムラのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_KamuraNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_PinchReactCommon     },
    { BTL_EVENT_MEMBER_IN,              handler_PinchReactMemberIn   },
    { BTL_EVENT_USE_ITEM,               handler_KamuraNomi           },
    { BTL_EVENT_USE_ITEM_TMP,           handler_KamuraNomi           },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_KamuraNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_PinchRankup( flowWk, pokeID, pml::wazadata::RANKEFF_AGILITY, 1 );
}
void handler_KamuraNomi_Tmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG ] ){
    handler_KamuraNomi( myHandle, flowWk, pokeID, work );
  }
}
//------------------------------------------------------------------------------
/**
 *  ヤタピのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_YatapiNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,     handler_PinchReactCommon     },
    { BTL_EVENT_MEMBER_IN,               handler_PinchReactMemberIn   },
    { BTL_EVENT_USE_ITEM,                handler_YatapiNomi           },
    { BTL_EVENT_USE_ITEM_TMP,            handler_YatapiNomi           },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_YatapiNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_PinchRankup( flowWk, pokeID, pml::wazadata::RANKEFF_SP_ATTACK, 1 );
}
void handler_YatapiNomi_Tmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG ] ){
    handler_YatapiNomi( myHandle, flowWk, pokeID, work );
  }
}
//------------------------------------------------------------------------------
/**
 *  ズアのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_ZuaNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,     handler_PinchReactCommon     },
    { BTL_EVENT_MEMBER_IN,               handler_PinchReactMemberIn   },
    { BTL_EVENT_USE_ITEM,                handler_ZuaNomi              },
    { BTL_EVENT_USE_ITEM_TMP,            handler_ZuaNomi              },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_ZuaNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_PinchRankup( flowWk, pokeID, pml::wazadata::RANKEFF_SP_DEFENCE, 1 );
}
void handler_ZuaNomi_Tmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG ] ){
    handler_ZuaNomi( myHandle, flowWk, pokeID, work );
  }
}
//------------------------------------------------------------------------------
/**
 *  サンのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_SanNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,     handler_PinchReactCommon     },
    { BTL_EVENT_MEMBER_IN,               handler_PinchReactMemberIn   },
    { BTL_EVENT_USE_ITEM,                handler_SanNomi              },
    { BTL_EVENT_USE_ITEM_TMP,            handler_SanNomi              },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_SanNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
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
        HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_Rankup_Critical );
        HANDEX_STR_AddArg( &msg_param->str, pokeID );
        HANDEX_STR_AddArg( &msg_param->str, EVENT_FACTOR_GetSubID(myHandle) );
      flowWk->HANDEX_Pop( msg_param );
    }
  }
}
void handler_SanNomi_Tmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG ] ){
    handler_SanNomi( myHandle, flowWk, pokeID, work );
  }
}
//------------------------------------------------------------------------------
/**
 *  スターのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_SutaaNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,     handler_PinchReactCommon     },
    { BTL_EVENT_MEMBER_IN,               handler_PinchReactMemberIn   },
    { BTL_EVENT_USE_ITEM,                handler_SutaaNomi            },
    { BTL_EVENT_USE_ITEM_TMP,            handler_SutaaNomi            },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_SutaaNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  static const u8 rankTypeTbl[] = {
    pml::wazadata::RANKEFF_ATTACK, pml::wazadata::RANKEFF_DEFENCE,
    pml::wazadata::RANKEFF_SP_ATTACK, pml::wazadata::RANKEFF_SP_DEFENCE,
    pml::wazadata::RANKEFF_AGILITY,
  };

  u32 cnt, i;
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  u8* typeAry = reinterpret_cast<u8*>(flowWk->Hnd_GetTmpWork( sizeof(rankTypeTbl) ));

  for(i=0, cnt=0; i<GFL_NELEMS(rankTypeTbl); ++i)
  {
    if( bpp->IsRankEffectValid( static_cast<BTL_POKEPARAM::ValueID>(rankTypeTbl[i]), 1) )
    {
      typeAry[ cnt++ ] = rankTypeTbl[ i ];
    }
  }

  if( cnt )
  {
    i = calc::GetRand( cnt );
    common_PinchRankup( flowWk, pokeID, static_cast<WazaRankEffect>(typeAry[i]), 2 );
  }
}
void handler_SutaaNomi_Tmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG ] ){
    handler_SutaaNomi( myHandle, flowWk, pokeID, work );
  }
}

//----------------------------------------------------------------------------------
/**
 * ピンチによって能力ランクアップするどうぐの共通処理
 *
 * @param   flowWk
 * @param   pokeID
 * @param   rankType
 * @param   volume
 */
//----------------------------------------------------------------------------------
void common_PinchRankup( ServerFlow* flowWk, u8 pokeID, WazaRankEffect rankType, u8 volume )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
      param->rankType = rankType;
      param->rankVolume = volume;
      param->poke_cnt = 1;
//      param->fAlmost = true;
      param->pokeID[0] = pokeID;
    flowWk->HANDEX_Pop( param );
  }
}
//------------------------------------------------------------------------------
/**
 *  ナゾのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_NazoNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_NazoNomi     },
    { BTL_EVENT_USE_ITEM,              handler_NazoNomi_Use },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_NazoNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == false)
  ){
    if( EVENTVAR_GetValue(BTL_EVAR_TYPEAFF) > pml::battle::TypeAffinity::TYPEAFF_1 )
    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
      if( !bpp->CheckSick( pml::wazadata::WAZASICK_KAIHUKUHUUJI) ){
        itemPushRun( myHandle, flowWk, pokeID );
      }
    }
  }
}
void handler_NazoNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u8 item_pow = static_cast<u8>(common_GetItemParam( myHandle, ::item::ITEM_DATA::PRM_ID_ATTACK ));

    BTL_HANDEX_PARAM_RECOVER_HP* param = reinterpret_cast<BTL_HANDEX_PARAM_RECOVER_HP *>( flowWk->HANDEX_Push( BTL_HANDEX_RECOVER_HP, pokeID ));
      param->pokeID = pokeID;
      param->recoverHP = calc::QuotMaxHP( bpp, item_pow );
    flowWk->HANDEX_Pop( param );
  }
}
//------------------------------------------------------------------------------
/**
 *  オッカのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_OkkaNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3,       handler_OkkaNomi                },
    { BTL_EVENT_WAZA_DMG_REACTION,    handler_common_WeakAff_DmgAfter },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_OkkaNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_HONOO, false );
}
//------------------------------------------------------------------------------
/**
 *  イトケのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_ItokeNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3,     handler_ItokeNomi               },
    { BTL_EVENT_WAZA_DMG_REACTION,  handler_common_WeakAff_DmgAfter },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_ItokeNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_MIZU, false );
}
//------------------------------------------------------------------------------
/**
 *  ソクノのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_SokunoNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3,        handler_SokunoNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_common_WeakAff_DmgAfter },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_SokunoNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_DENKI, false );
}
//------------------------------------------------------------------------------
/**
 *  リンドのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_RindoNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3,        handler_RindoNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_common_WeakAff_DmgAfter },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_RindoNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_KUSA, false );
}
//------------------------------------------------------------------------------
/**
 *  ヤチェのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_YacheNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3,          handler_YacheNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,       handler_common_WeakAff_DmgAfter },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_YacheNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_KOORI, false );
}
//------------------------------------------------------------------------------
/**
 *  ヨプのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_YopuNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3,         handler_YopuNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,      handler_common_WeakAff_DmgAfter },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_YopuNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_KAKUTOU, false );
}
//------------------------------------------------------------------------------
/**
 *  ビアーのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_BiarNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3,        handler_BiarNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_common_WeakAff_DmgAfter },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_BiarNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_DOKU, false );
}
//------------------------------------------------------------------------------
/**
 *  シュカのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_SyukaNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3,         handler_SyukaNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,      handler_common_WeakAff_DmgAfter },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_SyukaNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_JIMEN, false );
}
//------------------------------------------------------------------------------
/**
 *  バコウのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_BakouNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3,         handler_BakouNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,      handler_common_WeakAff_DmgAfter },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_BakouNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_HIKOU, false );
}
//------------------------------------------------------------------------------
/**
 *  ウタンのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_UtanNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3,        handler_UtanNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_common_WeakAff_DmgAfter },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_UtanNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_ESPER, false );
}
//------------------------------------------------------------------------------
/**
 *  タンガのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_TangaNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3,        handler_TangaNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_common_WeakAff_DmgAfter },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_TangaNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_MUSHI, false );
}
//------------------------------------------------------------------------------
/**
 *  ヨロギのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_YorogiNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3,        handler_YorogiNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_common_WeakAff_DmgAfter },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_YorogiNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_IWA, false );
}
//------------------------------------------------------------------------------
/**
 *  カシブのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_KasibuNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3,        handler_KasibuNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_common_WeakAff_DmgAfter },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_KasibuNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_GHOST, false );
}
//------------------------------------------------------------------------------
/**
 *  ハバンのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_HabanNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3,         handler_HabanNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,      handler_common_WeakAff_DmgAfter },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_HabanNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_DRAGON, false );
}
//------------------------------------------------------------------------------
/**
 *  ナモのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_NamoNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3,         handler_NamoNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,      handler_common_WeakAff_DmgAfter },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_NamoNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_AKU, false );
}
//------------------------------------------------------------------------------
/**
 *  リリバのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_RiribaNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3,        handler_RiribaNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_common_WeakAff_DmgAfter },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_RiribaNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_HAGANE, false );
}
//------------------------------------------------------------------------------
/**
 *  ホズのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_HozuNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3,       handler_HozuNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,    handler_common_WeakAff_DmgAfter },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_HozuNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_NORMAL, true );
}
//------------------------------------------------------------------------------
/**
 *  ロゼルのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_RozeruNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3,         handler_RozeruNomi              },
    { BTL_EVENT_WAZA_DMG_REACTION,      handler_common_WeakAff_DmgAfter },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_RozeruNomi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_FAIRY, false );
}

/**
 *  威力半減させたらtrueを返す
 */
bool common_WeakAff_Relieve( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work, pml::PokeType type, bool fIgnoreAffine )
{
  GFL_UNUSED(myHandle);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == type)
  &&  (!EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG))
  &&  ((fIgnoreAffine) || (EVENTVAR_GetValue(BTL_EVAR_TYPEAFF) > pml::battle::TypeAffinity::TYPEAFF_1))
  ){
    if( work[0] == 0 )
    {
      EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5) );
      if( !flowWk->Hnd_IsSimulationMode() )
      {
        work[0] = 1;

        BTL_HANDEX_PARAM_TURNFLAG* flagParam = reinterpret_cast<BTL_HANDEX_PARAM_TURNFLAG *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_TURNFLAG, pokeID ));
          flagParam->pokeID = pokeID;
          flagParam->flag = BTL_POKEPARAM::TURNFLG_RESERVE_ITEM_SPEND;
        flowWk->HANDEX_Pop( flagParam );
      }
      return true;
    }
  }
  return false;
}
void handler_common_WeakAff_DmgAfter( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[0] > 0)
  {
    if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
    {
      work[0] = 0;

      flowWk->Hnd_EnableWazaSubEffect( pokeID );
      BTL_HANDEX_PARAM_CONSUME_ITEM* param = reinterpret_cast<BTL_HANDEX_PARAM_CONSUME_ITEM *>( flowWk->HANDEX_Push( BTL_HANDEX_CONSUME_ITEM, pokeID ));
        param->fNoAction = true;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Item_DamageShrink );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
        HANDEX_STR_AddArg( &param->exStr, EVENT_FACTOR_GetSubID(myHandle) );
      flowWk->HANDEX_Pop( param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ピンチ時（HP <= 1/アイテム威力値）に反応する木の実の共通処理（ダメージ反応）
 *
 * @param   myHandle
 * @param   flowWk
 * @param   pokeID
 * @param   work
 */
//----------------------------------------------------------------------------------
void handler_PinchReactCommon( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    s32 item_pow = common_GetItemParam( myHandle, ::item::ITEM_DATA::PRM_ID_ATTACK );
    common_DamageReact( myHandle, flowWk, pokeID, item_pow, true );
  }
}
//----------------------------------------------------------------------------------
/**
 * ピンチ時（HP <= 1/アイテム威力値）に反応する木の実の共通処理（メンバー入場）
 *
 * @param   myHandle
 * @param   flowWk
 * @param   pokeID
 * @param   work
 */
//----------------------------------------------------------------------------------
void handler_PinchReactMemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    s32 item_pow = common_GetItemParam( myHandle, ::item::ITEM_DATA::PRM_ID_ATTACK );
    common_DamageReact( myHandle, flowWk, pokeID, item_pow, false );
  }
}


//----------------------------------------------------------------------------------
/**
 * HP 1/n 以下で反応するアイテムの共通処理 - 条件を満たしていたらアイテム使用ハンドラ呼び出し
 *
 * @param   flowWk
 * @param   pokeID
 * @param   n
 */
//----------------------------------------------------------------------------------
void common_DamageReact( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, u32 n, bool fCheckReactionType )
{

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    bool fReaction;

    if( fCheckReactionType )
    {
      fReaction = common_DamageReactCheck( myHandle, flowWk, pokeID, n );
    }
    else
    {
      fReaction = common_DamageReactCheckCore( flowWk, pokeID, n );
    }

//    if( BTL_EVENT_FACTOR_GetPokeID

    if( fReaction )
    {
      BTL_N_PrintfSimple( DBGSTR_Item_PinchReactOn );
      itemPushRun( myHandle, flowWk, pokeID );
    }
  }
}
//--------------------------------------------------------------
/**
 * HP 1/n 以下で反応するアイテムの共通処理 - 条件を満たしてるかチェック
 */
//--------------------------------------------------------------
bool common_DamageReactCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, u32 n )
{
  GFL_UNUSED(myHandle);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BtlItemReaction reactionType = EVENTVAR_GetValue( BTL_EVAR_ITEM_REACTION );
    if( reactionType & BTL_ITEMREACTION_HP )
    {
      return common_DamageReactCheckCore( flowWk, pokeID, n );
    }
  }
  return false;
}
bool common_DamageReactCheckCore( ServerFlow* flowWk, u8 pokeID, u32 n )
{
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  u32 maxHP;

  if( bpp->GetValue( BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE) == TOKUSEI_KUISINBOU )
  {
    if( n > 2 ){
      n /= 2;
    }
  }
  if( n == 0 ){
    GFL_ASSERT(0);
    n = 1;
  }

  BTL_PRINT("ここきた\n");

  maxHP = bpp->GetValue( BTL_POKEPARAM::BPP_MAX_HP );
  if( maxHP > 1 )
  {
    u32 hp = bpp->GetValue( BTL_POKEPARAM::BPP_HP );
    BTL_PRINT("hp/maxHP = %d/%d\n", hp, maxHP);

    if( hp <= calc::QuotMaxHP_Zero(bpp, n) )
    {
      BTL_PRINT("true\n");
      return true;
    }
  }
  BTL_PRINT("false\n");
  return false;
}

//------------------------------------------------------------------------------
/**
 *  イバンのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_IbanNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_SP_PRIORITY,    handler_IbanNomi_SpPriorityCheck  },  // 特殊行動プライオリティチェック
    { BTL_EVENT_USE_ITEM,             handler_IbanNomi_Use              },  // アイテム使用ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 特殊行動プライオリティチェック
void handler_IbanNomi_SpPriorityCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);

  // 既に書き換わっているなら、何もしない
  if( EVENTVAR_GetValue( BTL_EVAR_SP_PRIORITY ) != BTL_SPPRI_DEFAULT )
  {
    return;
  }

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // @fix NMCat[189] 行動後に「せんせいのツメ」「イバンのみ」が発動する
    //「おいうち」で相手の入れ替えに割り込んだ場合など、この時すでに行動済みの場合があります。
    // 行動済みなら、発動しないようにします。
    if( flowWk->Hnd_IsMyAllActDone( pokeID ) )
    {
      return;
    }

    if( common_DamageReactCheckCore(flowWk, pokeID, 4) )
    {
      if( EVENTVAR_RewriteValue(BTL_EVAR_SP_PRIORITY, BTL_SPPRI_HIGH) )
      {
        itemPushRun( myHandle, flowWk, pokeID );
      }
    }
  }
}

// アイテム使用ハンドラ
void handler_IbanNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_PriorityUpOnce );
      HANDEX_STR_AddArg( &param->str, pokeID );
      HANDEX_STR_AddArg( &param->str, EVENT_FACTOR_GetSubID(myHandle) );
    flowWk->HANDEX_Pop( param );

    work[0] = 0;
  }
}

//------------------------------------------------------------------------------
/**
 *  ミクルのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_MikuruNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,   handler_MikuruNomi_Reaction   },
    { BTL_EVENT_MEMBER_IN,             handler_MikuruNomi_MemberIn   },
    { BTL_EVENT_ACTPROC_END,           handler_MikuruNomi_ActProcEnd },
    { BTL_EVENT_USE_ITEM,              handler_MikuruNomi_Use        }, // アイテム使用ハンドラ
    { BTL_EVENT_USE_ITEM_TMP,          handler_MikuruNomi_UseTmp     }, // アイテム使用ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// アイテム反応チェック
void handler_MikuruNomi_Reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    u8 quot = static_cast<u8>(common_GetItemParam( myHandle, ::item::ITEM_DATA::PRM_ID_ATTACK ));
    if( common_DamageReactCheck(myHandle, flowWk, pokeID, quot) )
    {
      itemPushRun( myHandle, flowWk, pokeID );
    }
  }
}
// メンバー入場
void handler_MikuruNomi_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    u8 quot = static_cast<u8>(common_GetItemParam( myHandle, ::item::ITEM_DATA::PRM_ID_ATTACK ));
    if( common_DamageReactCheckCore(flowWk, pokeID, quot) )
    {
      itemPushRun( myHandle, flowWk, pokeID );
    }
  }
}
// アクション終了
void handler_MikuruNomi_ActProcEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  handler_MikuruNomi_MemberIn( myHandle, flowWk, pokeID, work );
}
// アイテム使用ハンドラ
void handler_MikuruNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
      param->pokeID = pokeID;
      param->sickID = pml::wazadata::WAZASICK_HITRATIO_UP;
      param->sickCont = SICKCONT_MakePermanentParam( pokeID, 120 );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_HitRatioUpOnce );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, EVENT_FACTOR_GetSubID(myHandle) );
    flowWk->HANDEX_Pop( param );
  }
}
void handler_MikuruNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    handler_MikuruNomi_Use( myHandle, flowWk, pokeID, work );
  }
}


//------------------------------------------------------------------------------
/**
 *  ジャポのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_JapoNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION_L2, handler_JapoNomi_Damage     }, // ダメージ反応ハンドラ
    { BTL_EVENT_USE_ITEM,             handler_JapoNomi_Use        }, // アイテム使用ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージ反応ハンドラ
void handler_JapoNomi_Damage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_JapoRenbu_Reaction( myHandle, flowWk, pokeID, work, pml::wazadata::DMG_PHYSIC );
}
//------------------------------------------------------------------------------
/**
 *  レンブのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_RenbuNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION_L2, handler_RenbuNomi_Damage     }, // ダメージ反応ハンドラ
    { BTL_EVENT_USE_ITEM,             handler_JapoNomi_Use         }, // アイテム使用ハンドラ（ジャポのみと同じ）
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージ反応ハンドラ
void handler_RenbuNomi_Damage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_JapoRenbu_Reaction( myHandle, flowWk, pokeID, work, pml::wazadata::DMG_SPECIAL );
}



/**
 *  ジャポのみ・レンブのみ共用：ダメージ反応処理
 */
void common_JapoRenbu_Reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work, WazaDamageType dmgType )
{
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == false)
  ){
    WazaID waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    if( (!tables::IsDelayAttackWaza(waza))
    &&  (EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == dmgType)
    ){
      BTL_HANDEX_PARAM_USE_ITEM* param;

      EVENT_FACTOR_SetRecallEnable( myHandle );
      work[0] = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK ) + 1;

      param = reinterpret_cast<BTL_HANDEX_PARAM_USE_ITEM *>( flowWk->HANDEX_Push( BTL_HANDEX_USE_ITEM, pokeID ));
        param->fUseDead = true;
      flowWk->HANDEX_Pop( param );
    }
  }
}
/**
 *  ジャポのみ・レンブのみ共用：アイテム使用ハンドラ
 */
void handler_JapoNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( work[0] != 0 )
    {
      u8 targetPokeID = work[0] - 1;
      const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( targetPokeID );
      if( !target->IsDead() )
      {
        u8 quot = static_cast<u8>(common_GetItemParam( myHandle, ::item::ITEM_DATA::PRM_ID_ATTACK ));

        // ダメージを与える
        // 
        // このダメージでは、死亡した場合でも退場処理を行いません。
        // ここで退場処理を行ってしまうと、両陣営が同時に全滅した場合の勝敗判定が反転してしまうためです。
        //
        // 両陣営が全滅した場合、後に退場したほうの勝ちとなりますが、
        // ここで退場処理を行ってしまうと
        // 防御側が技ダメージによって退場する前に
        // 攻撃側が「ジャポのみ」のダメージで退場してしまうため、攻撃側が負けになってしまいます。
        BTL_HANDEX_PARAM_DAMAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_DAMAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_DAMAGE, pokeID ));
          param->pokeID              = targetPokeID;
          param->damage              = calc::QuotMaxHP( target, quot );
          param->damageCause         = DAMAGE_CAUSE_JAPO_RENBU;
          param->damageCausePokeID   = pokeID;
          param->fDisableDeadProcess = true;                         // 死亡した場合でも退場処理を行わない
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_DamageOpponent );
          HANDEX_STR_AddArg( &param->exStr, param->pokeID );
          HANDEX_STR_AddArg( &param->exStr, pokeID );
          HANDEX_STR_AddArg( &param->exStr, EVENT_FACTOR_GetSubID(myHandle) );
        flowWk->HANDEX_Pop( param );
      }
    }
  }
}


//------------------------------------------------------------------------------
/**
 *  とつげきチョッキ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_TotugekiChokki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DEFENDER_GUARD,  handler_totugekiChokki      },  // 防御力決定ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 防御力決定ハンドラ
void handler_totugekiChokki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  // 常に特防1.5倍
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == pml::wazadata::DMG_SPECIAL)
  ){
    EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.5f) );
  }
}

//------------------------------------------------------------------------------
/**
 *  じゃくてんほけん
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_JyakutenHoken( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION, handler_JyakutenHoken_Damage     }, // ダメージ反応ハンドラ
    { BTL_EVENT_USE_ITEM,          handler_JyakutenHoken_Use        }, // アイテム使用ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージ反応ハンドラ
void handler_JyakutenHoken_Damage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (!EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG))
  &&  (EVENTVAR_GetValue(BTL_EVAR_TYPEAFF) > pml::battle::TypeAffinity::TYPEAFF_1)
  ){
    BTL_HANDEX_PARAM_USE_ITEM* param;

    EVENT_FACTOR_SetRecallEnable( myHandle );
    work[0] = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK ) + 1;

    param = reinterpret_cast<BTL_HANDEX_PARAM_USE_ITEM *>( flowWk->HANDEX_Push( BTL_HANDEX_USE_ITEM, pokeID ));
      param->fUseDead = true;
    flowWk->HANDEX_Pop( param );
  }
}
// アイテム使用ハンドラ
void handler_JyakutenHoken_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param;
    param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
      param->rankType = pml::wazadata::RANKEFF_ATTACK;
      param->rankVolume = 2;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
    flowWk->HANDEX_Pop( param );

    param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
      param->rankType = pml::wazadata::RANKEFF_SP_ATTACK;
      param->rankVolume = 2;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
    flowWk->HANDEX_Pop( param );
  }
}


//------------------------------------------------------------------------------
/**
 *  しろいハーブ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_SiroiHerb( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_SiroiHerb_Reaction     },
//    { BTL_EVENT_RANKEFF_FIXED,       handler_SiroiHerb_TurnCheck },
    { BTL_EVENT_MEMBER_IN_COMP,      handler_SiroiHerb_MemberInComp },
    { BTL_EVENT_ACTPROC_END,         handler_SiroiHerb_ActEnd       },
    { BTL_EVENT_TURNCHECK_END,       handler_SiroiHerb_TurnCheck    },
    { BTL_EVENT_USE_ITEM,            handler_SiroiHerb_Use          },
    { BTL_EVENT_USE_ITEM_TMP,        handler_SiroiHerb_UseTmp       },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_SiroiHerb_ActCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  if( bpp->IsRankEffectDowned() )
  {
    itemPushRun( myHandle, flowWk, pokeID );
  }
}
void handler_SiroiHerb_MemberInComp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  if( bpp->IsRankEffectDowned() )
  {
    itemPushRun( myHandle, flowWk, pokeID );
  }
}
void handler_SiroiHerb_ActEnd( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  handler_SiroiHerb_MemberInComp( myHandle, flowWk, pokeID, work );
}

void handler_SiroiHerb_TurnCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->IsRankEffectDowned() )
    {
      itemPushRun( myHandle, flowWk, pokeID );
    }
  }
}
void handler_SiroiHerb_Reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  // アイテムリアクションとしては、入手した場合のみに反応する
  BtlItemReaction reactionType = static_cast<BtlItemReaction>(EVENTVAR_GetValue( BTL_EVAR_ITEM_REACTION ));
  if( reactionType & BTL_ITEMREACTION_RENEW ){
    handler_SiroiHerb_TurnCheck( myHandle, flowWk, pokeID, work );
  }
}

void handler_SiroiHerb_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_RECOVER_RANK* recover_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    TAYA_Printf("pokeID=%d, しろいハーブで回復するよ\n", pokeID);
    recover_param = reinterpret_cast<BTL_HANDEX_PARAM_RECOVER_RANK *>( flowWk->HANDEX_Push( BTL_HANDEX_RECOVER_RANK, pokeID ));
      recover_param->pokeID = pokeID;
    flowWk->HANDEX_Pop( recover_param );

    TAYA_Printf("pokeID=%d, しろいハーブメッセージ出すよ\n", pokeID);

    msg_param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_RankRecoverItem );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
      HANDEX_STR_AddArg( &msg_param->str, EVENT_FACTOR_GetSubID(myHandle) );
    flowWk->HANDEX_Pop( msg_param );
  }
}
void handler_SiroiHerb_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    handler_SiroiHerb_Use( myHandle, flowWk, pokeID, work );
  }
}
//------------------------------------------------------------------------------
/**
 *  メンタルハーブ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_MentalHerb( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,  handler_MentalHerb_React  }, // 状態異常チェックハンドラ
    { BTL_EVENT_MEMBER_IN,            handler_MentalHerb_React  },
    { BTL_EVENT_USE_ITEM,             handler_MentalHerb_Use    },
    { BTL_EVENT_USE_ITEM_TMP,         handler_MentalHerb_UseTmp },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_MentalHerb_React( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( calc::CheckMentalSick(bpp) != pml::wazadata::WAZASICK_NULL )
    {
      itemPushRun( myHandle, flowWk, pokeID );
    }
  }
}
void handler_MentalHerb_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_useForSick( myHandle, flowWk, pokeID, WAZASICK_EX_MENTAL );
}
void handler_MentalHerb_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    handler_MentalHerb_Use( myHandle, flowWk, pokeID, work );
  }
}
//------------------------------------------------------------------------------
/**
 *  ひかりのこな
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_HikarinoKona( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO,     handler_HikarinoKona },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_HikarinoKona( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    u32 per = 100 - common_GetItemParam( myHandle, ::item::ITEM_DATA::PRM_ID_ATTACK );
    fx32 r = FX32_CONST(per) / 100;
    EVENTVAR_MulValue( BTL_EVAR_RATIO, r );
  }
}
//------------------------------------------------------------------------------
/**
 *  きょうせいギプス
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_KyouseiGipusu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,     handler_KyouseiGipusu },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_KyouseiGipusu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
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
 *  せんせいのツメ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_SenseiNoTume( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_SP_PRIORITY,    handler_SenseiNoTume_SpPriorityCheck  },  // 特殊行動プライオリティチェック
    { BTL_EVENT_USE_ITEM,             handler_SenseiNoTume_Use              },  // アイテム使用ハンドラ
    { BTL_EVENT_TURNCHECK_DONE,       handler_SenseiNoTume_TurnCheckDone    },  // ターンチェック終了ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

enum SenseiNoTumeWorkIdx
{
  WIDX_SENSEINOTUME_RAND_CHECK_DONE = 0,  // 乱数抽選済みか？
  WIDX_SENSEINOTUME_RAND_CHECK_CLEAR,     // 乱数抽選の結果、発動するか？
  WIDX_SENSEINOTUME_USE_DONE,             // 道具使用処理を実行済みか？
};

// 特殊行動プライオリティチェック
void handler_SenseiNoTume_SpPriorityCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);

  // 既に書き換わっているなら、何もしない
  if( EVENTVAR_GetValue( BTL_EVAR_SP_PRIORITY ) != BTL_SPPRI_DEFAULT )
  {
    return;
  }

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // @fix NMCat[189] 行動後に「せんせいのツメ」「イバンのみ」が発動する
    //「おいうち」で相手の入れ替えに割り込んだ場合など、この時すでに行動済みの場合があります。
    // 行動済みなら、発動しないようにします。
    if( flowWk->Hnd_IsMyAllActDone( pokeID ) )
    {
      return;
    }

    // 乱数抽選
    if( work[ WIDX_SENSEINOTUME_RAND_CHECK_DONE ] == 0 )
    {
      u8 per = static_cast<u8>(common_GetItemParam( myHandle, ::item::ITEM_DATA::PRM_ID_ATTACK ));
      work[ WIDX_SENSEINOTUME_RAND_CHECK_CLEAR ] = item_IsExePer(flowWk, per);
      work[ WIDX_SENSEINOTUME_RAND_CHECK_DONE ] = 1;
    }
    
    // 発動
    if( work[ WIDX_SENSEINOTUME_RAND_CHECK_CLEAR ] )
    {
      if( EVENTVAR_RewriteValue(BTL_EVAR_SP_PRIORITY, BTL_SPPRI_HIGH) )
      {
        if( work[ WIDX_SENSEINOTUME_USE_DONE ] == 0 )
        {
          itemPushRun( myHandle, flowWk, pokeID );
        }
      }
    }
  }
}

// アイテム使用ハンドラ
void handler_SenseiNoTume_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));

      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_PriorityUpOnce );
      HANDEX_STR_AddArg( &param->str, pokeID );
      HANDEX_STR_AddArg( &param->str, EVENT_FACTOR_GetSubID(myHandle) );

    flowWk->HANDEX_Pop( param );

    work[ WIDX_SENSEINOTUME_USE_DONE ] = 1;
  }
}

// ターンチェック終了ハンドラ
void handler_SenseiNoTume_TurnCheckDone( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    work[ WIDX_SENSEINOTUME_RAND_CHECK_DONE ] = 0;
    work[ WIDX_SENSEINOTUME_RAND_CHECK_CLEAR ] = 0;
    work[ WIDX_SENSEINOTUME_USE_DONE ] = 0;
  }
}

//------------------------------------------------------------------------------
/**
 *  こうこうのしっぽ、まんぷくおこう
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_KoukouNoSippo( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_SP_PRIORITY,      handler_KoukouNoSippo },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_KoukouNoSippo( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
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
    //「おいうち」で相手の入れ替えに割り込んだ場合など、この時すでに行動済みの場合があります。
    // 行動済みなら、発動しないようにします。
    if( flowWk->Hnd_IsMyAllActDone( pokeID ) )
    {
      return;
    }

    EVENTVAR_RewriteValue( BTL_EVAR_SP_PRIORITY, BTL_SPPRI_LOW );
  }
}
//------------------------------------------------------------------------------
/**
 *  おうじゃのしるし
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_OujaNoSirusi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_SHRINK_PER,     handler_OujaNoSirusi        },
    { BTL_EVENT_USE_ITEM_TMP,        handler_OujaNoSirusi_UseTmp },   // なげつける等
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_OujaNoSirusi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 自分が攻撃側で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ひるみ確率０なら、アイテム威力値に書き換え
    if( EVENTVAR_GetValue(BTL_EVAR_ADD_DEFAULT_PER) == 0 )
    {
      u8 per = static_cast<u8>(common_GetItemParam( myHandle, ::item::ITEM_DATA::PRM_ID_ATTACK ));
      #ifdef PM_BTL_DEBUG
      if( flowWk->Hnd_GetDebugFlag( BTL_DEBUGFLAG_MUST_ITEM) ){
        EVENTVAR_RewriteValue( BTL_EVAR_ADD_PER, 100 );
        return;
      }
      #endif

      EVENTVAR_RewriteValue( BTL_EVAR_ADD_PER, per );
      BTL_N_Printf( DBGSTR_HANDITEM_OujaEffective, per );
    }
  }
}
void handler_OujaNoSirusi_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_SHRINK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SHRINK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SHRINK, pokeID ));
      param->pokeID = pokeID;
      param->per = 100;
    flowWk->HANDEX_Pop( param );
  }
}
//------------------------------------------------------------------------------
/**
 *  するどいキバ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_SurudoiKiba( u32* numElems )
{
  // どちらも「おうじゃのしるし」と等価
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_SHRINK_PER,     handler_OujaNoSirusi        },
    { BTL_EVENT_USE_ITEM_TMP,        handler_OujaNoSirusi_UseTmp },   // なげつける等
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  こうかくレンズ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_KoukakuLens( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO,     handler_KoukakuLens },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_KoukakuLens( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 自分が攻撃側のとき
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 命中率上昇
    fx32 ratio = item_AttackValue_to_Ratio( myHandle );
    EVENTVAR_MulValue( BTL_EVAR_RATIO, ratio );
  }
}
//------------------------------------------------------------------------------
/**
 *  ピントレンズ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_PintLens( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CRITICAL_CHECK,     handler_PintLens },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_PintLens( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 自分が攻撃側のとき
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 rank = EVENTVAR_GetValue( BTL_EVAR_CRITICAL_RANK );
    rank += 1;
    EVENTVAR_RewriteValue( BTL_EVAR_CRITICAL_RANK, rank );
  }
}
//------------------------------------------------------------------------------
/**
 *  ラッキーパンチ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_LuckyPunch( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CRITICAL_CHECK, handler_LuckyPunch   },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_LuckyPunch( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 自分が攻撃側で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ラッキーなら
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u32 monsNo = bpp->GetMonsNo( );
    if( monsNo == MONSNO_RAKKII )
    {
      // クリティカル２段階上昇
      u8 rank = EVENTVAR_GetValue( BTL_EVAR_CRITICAL_RANK );
      rank += 2;
      EVENTVAR_RewriteValue( BTL_EVAR_CRITICAL_RANK, rank );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  ながねぎ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_Naganegi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CRITICAL_CHECK, handler_Naganegi   },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Naganegi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 自分が攻撃側で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // カモネギなら
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u32 monsNo = bpp->GetMonsNo( );
    if( monsNo == MONSNO_KAMONEGI )
    {
      // クリティカル２段階上昇
      u8 rank = EVENTVAR_GetValue( BTL_EVAR_CRITICAL_RANK );
      rank += 2;
      EVENTVAR_RewriteValue( BTL_EVAR_CRITICAL_RANK, rank );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  フォーカスレンズ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_FocusLens( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO,     handler_FocusLens },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_FocusLens( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( pokeID == EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) )
  {
    u8 def_pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* defender = flowWk->Hnd_GetPokeParam( def_pokeID );

    if( defender->TURNFLAG_Get( BTL_POKEPARAM::TURNFLG_ACTION_DONE) )
    {
      fx32 ratio = item_AttackValue_to_Ratio( myHandle );
      EVENTVAR_MulValue( BTL_EVAR_RATIO, ratio );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  のんきのおこう
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_NonkiNoOkou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO,     handler_NonkiNoOkou },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_NonkiNoOkou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( pokeID == EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) )
  {
    u16 ratio = 100 - static_cast<u16>(common_GetItemParam( myHandle, ::item::ITEM_DATA::PRM_ID_ATTACK ));
    fx32 fx_ratio = FX32_CONST(ratio) / 100;
    EVENTVAR_MulValue( BTL_EVAR_RATIO, fx_ratio );
  }
}
//------------------------------------------------------------------------------
/**
 *  ちからのハチマキ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_ChikaraNoHachimaki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_ChikaraNoHachimaki },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_ChikaraNoHachimaki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 自分が攻撃側で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ぶつり攻撃のとき、威力上昇
    if( EVENTVAR_GetValue( BTL_EVAR_DAMAGE_TYPE ) == pml::wazadata::DMG_PHYSIC )
    {
      fx32 ratio = item_AttackValue_to_Ratio( myHandle );
      EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, ratio );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  ものしりメガネ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_MonosiriMegane( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_MonosiriMegane },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_MonosiriMegane( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 自分が攻撃側で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // とくしゅ攻撃のとき、威力上昇
    if( EVENTVAR_GetValue( BTL_EVAR_DAMAGE_TYPE ) == pml::wazadata::DMG_SPECIAL )
    {
      fx32 ratio = item_AttackValue_to_Ratio( myHandle );
      EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, ratio );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  しんかいのキバ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_SinkaiNoKiba( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_SinkaiNoKiba   },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_SinkaiNoKiba( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 自分が攻撃側で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // パールルなら
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u32 monsNo = bpp->GetMonsNo( );
    if( monsNo == MONSNO_PAARURU )
    {
      // とくこう２倍
      if( EVENTVAR_GetValue( BTL_EVAR_DAMAGE_TYPE ) == pml::wazadata::DMG_SPECIAL )
      {
        EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  しんかいのウロコ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_SinkaiNoUroko( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DEFENDER_GUARD, handler_SinkaiNoUroko   },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_SinkaiNoUroko( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 自分が防御側で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // パールルなら
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u32 monsNo = bpp->GetMonsNo( );
    if( monsNo == MONSNO_PAARURU )
    {
      // とくぼう２倍
      if( EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == pml::wazadata::DMG_SPECIAL )
      {
        EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  メタルパウダー
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_MetalPowder( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DEFENDER_GUARD, handler_MetalPowder   },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_MetalPowder( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 自分が防御側で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // メタモン（へんしん前）なら
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( (bpp->GetMonsNo() == MONSNO_METAMON)
    &&  (!bpp->HENSIN_Check())
    ){
      // ぼうぎょ２倍
      if( EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == pml::wazadata::DMG_PHYSIC )
      {
        EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  スピードパウダー
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_SpeedPowder( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY, handler_SpeedPowder   },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_SpeedPowder( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // 自分がメタモンで変身前なら素早さ２倍
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( (bpp->GetMonsNo() == MONSNO_METAMON)
    &&  (!bpp->HENSIN_Check())
    ){
      EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  ふといほね
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_FutoiHone( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER,     handler_FutoiHone },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_FutoiHone( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 自分が攻撃側で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // カラカラ・ガラガラなら
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u32 monsNo = bpp->GetMonsNo( );
    if( (monsNo == MONSNO_KARAKARA) || (monsNo == MONSNO_GARAGARA) )
    {
      // こうげき上昇
      if( EVENTVAR_GetValue( BTL_EVAR_DAMAGE_TYPE ) == pml::wazadata::DMG_PHYSIC )
      {
        EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  こだわりハチマキ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_KodawariHachimaki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_CALL_DECIDE,   handler_Kodawari_Common_WazaExe    }, // ワザ名出力確定ハンドラ
    { BTL_EVENT_ITEMSET_DECIDE,     handler_Kodawari_Common_ItemChange }, // アイテム変更確定ハンドラ
    { BTL_EVENT_ATTACKER_POWER,     handler_KodawariHachimaki_Pow      }, // 攻撃力決定ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 攻撃力決定ハンドラ
void handler_KodawariHachimaki_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 自分が攻撃側なら
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 物理こうげき上昇
    if( EVENTVAR_GetValue( BTL_EVAR_DAMAGE_TYPE ) == pml::wazadata::DMG_PHYSIC )
    {
      EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.5) );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  こだわりメガネ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_KodawariMegane( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_CALL_DECIDE,    handler_Kodawari_Common_WazaExe    }, // ワザ名出力確定ハンドラ
    { BTL_EVENT_ITEMSET_DECIDE,      handler_Kodawari_Common_ItemChange }, // アイテム変更確定ハンドラ
    { BTL_EVENT_ATTACKER_POWER,      handler_KodawariMegane_Pow         }, // 攻撃力決定ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 攻撃力決定ハンドラ
void handler_KodawariMegane_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  // 自分が攻撃側なら
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 特殊こうげき上昇
    if( EVENTVAR_GetValue( BTL_EVAR_DAMAGE_TYPE ) == pml::wazadata::DMG_SPECIAL )
    {
      EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.5) );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  こだわりスカーフ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_KodawariScarf( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_CALL_DECIDE,    handler_Kodawari_Common_WazaExe      }, // ワザ名出力確定ハンドラ
    { BTL_EVENT_ITEMSET_DECIDE,      handler_Kodawari_Common_ItemChange   }, // アイテム変更確定ハンドラ
    { BTL_EVENT_CALC_AGILITY,        handler_KodawariScarf                }, // 素早さ計算ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 素早さ計算ハンドラ
void handler_KodawariScarf( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID ){
    // すばやさ上昇
    EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.5) );
  }
}

// ワザ出し確定ハンドラ（こだわり系共通）
void handler_Kodawari_Common_WazaExe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    WazaID  waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_ORG_WAZAID ));

    if( waza != WAZANO_WARUAGAKI )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
        param->pokeID = pokeID;
        param->sickID = pml::wazadata::WAZASICK_KODAWARI;
        param->sickCont = SICKCONT_MakePermanentParam( pokeID, waza );
        param->overWriteMode = BTL_SICK_OW_STRONG;
      flowWk->HANDEX_Pop( param );
    }
  }
}
// アイテム変更確定ハンドラ
void handler_Kodawari_Common_ItemChange( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_CURE_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
      param->sickCode  = static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_KODAWARI);
      param->pokeID[0] = pokeID;
      param->poke_cnt  = 1;
      param->fStdMsgDisable = true;
    flowWk->HANDEX_Pop( param );
  }
}



//------------------------------------------------------------------------------
/**
 *  きあいのタスキ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_KiaiNoTasuki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_KORAERU_CHECK,  handler_KiaiNoTasuki_Check },    // こらえるチェックハンドラ
    { BTL_EVENT_KORAERU_EXE,    handler_KiaiNoTasuki_Exe      }, // こらえる発動ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// こらえるチェックハンドラ
void handler_KiaiNoTasuki_Check( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->IsHPFull() )
    {
      work[0] = EVENTVAR_RewriteValue( BTL_EVAR_KORAERU_CAUSE, BTL_POKEPARAM::KORAE_ITEM );
    }
    else{
      work[0] = 0;
    }
  }
}
// こらえる発動ハンドラ
void handler_KiaiNoTasuki_Exe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( work[0] )
    {
      BTL_HANDEX_PARAM_CONSUME_ITEM* param = reinterpret_cast<BTL_HANDEX_PARAM_CONSUME_ITEM *>( flowWk->HANDEX_Push( BTL_HANDEX_CONSUME_ITEM, pokeID ));
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_KoraeItem );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
        HANDEX_STR_AddArg( &param->exStr, EVENT_FACTOR_GetSubID(myHandle) );
      flowWk->HANDEX_Pop( param );

      work[0] = 0;
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  きあいのハチマキ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_KiaiNoHachimaki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_KORAERU_CHECK,  handler_KiaiNoHachimaki_Check   }, // こらえるチェックハンドラ
    { BTL_EVENT_KORAERU_EXE,    handler_KiaiNoHachimaki_Exe     }, // こらえる発動ハンドラ
    { BTL_EVENT_USE_ITEM,       handler_KiaiNoHachimaki_UseItem }, // アイテム使用ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// こらえるチェックハンドラ
void handler_KiaiNoHachimaki_Check( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    u8 per = static_cast<u8>(common_GetItemParam( myHandle, ::item::ITEM_DATA::PRM_ID_ATTACK ));
    if( item_IsExePer(flowWk, per) )
    {
      work[0] = EVENTVAR_RewriteValue( BTL_EVAR_KORAERU_CAUSE, BTL_POKEPARAM::KORAE_ITEM );
    }
    else{
      work[0] = 0;
    }
  }
}
// こらえる発動ハンドラ
void handler_KiaiNoHachimaki_Exe( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (work[0] != 0)
  ){
    itemPushRun( myHandle, flowWk, pokeID );
    work[0] = 0;
  }
}
// アイテム使用ハンドラ
void handler_KiaiNoHachimaki_UseItem( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_KoraeItem );
      HANDEX_STR_AddArg( &param->str, pokeID );
      HANDEX_STR_AddArg( &param->str, EVENT_FACTOR_GetSubID(myHandle) );
    flowWk->HANDEX_Pop( param );
  }
}

//------------------------------------------------------------------------------
/**
 *  たつじんのおび
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_TatsujinNoObi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3, handler_TatsujinNoObi },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_TatsujinNoObi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_TYPEAFF) > pml::battle::TypeAffinity::TYPEAFF_1)
  ){
    fx32 ratio = item_AttackValue_to_Ratio( myHandle );
    EVENTVAR_MulValue( BTL_EVAR_RATIO, ratio );
  }
}
//------------------------------------------------------------------------------
/**
 *  いのちのたま
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_InochiNoTama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_L2,   handler_InochiNoTama_Reaction },   // ダメージ処理後ハンドラ
    { BTL_EVENT_WAZA_DMG_PROC3,          handler_InochiNoTama_Damage   },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージ処理後ハンドラ
void handler_InochiNoTama_Reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    BTL_HANDEX_PARAM_DAMAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_DAMAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_DAMAGE, pokeID ));
      param->pokeID = pokeID;
      param->damage = calc::QuotMaxHP( bpp, 10 );
      param->damageCause = DAMAGE_CAUSE_INOTINOTAMA;
      param->damageCausePokeID = pokeID;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_InochiNoTama );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
    flowWk->HANDEX_Pop( param );
  }
}
void handler_InochiNoTama_Damage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    fx32 ratio = item_AttackValue_to_Ratio( myHandle );
    EVENTVAR_MulValue( BTL_EVAR_RATIO, ratio );
  }
}
//------------------------------------------------------------------------------
/**
 *  メトロノーム
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_MetroNome( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC3, handler_MetroNome},
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_MetroNome( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (!tables::IsDelayAttackWaza(static_cast<WazaNo>(EVENTVAR_GetValue(BTL_EVAR_WAZAID))))
  ){
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u16 counter = bpp->GetWazaContCounter( );
    if( counter >= 1 )
    {
      WazaID  currentWaza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
      WazaID  prevWaza    = bpp->GetPrevWazaID();

      if( currentWaza == prevWaza )
      {
        fx32 ratio;
        s32  pow;

        pow = 100 + (common_GetItemParam(myHandle, ::item::ITEM_DATA::PRM_ID_ATTACK) * (counter));
        if( pow > 200 ){ pow = 200; }
        ratio = (FX32_CONST(pow) / 100);

//        TAYA_Printf("カウンタ=%d, pow=%d, ratio=%08x\n", counter, pow, ratio );
        EVENTVAR_MulValue( BTL_EVAR_RATIO, ratio );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  ねばりのかぎづめ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_NebariNoKagidume( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASICK_PARAM, handler_NebariNoKagidume },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_NebariNoKagidume( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_SICKID) == pml::wazadata::WAZASICK_BIND)
  ){
    BTL_SICKCONT* cont = reinterpret_cast<BTL_SICKCONT*>( EVENTVAR_GetValue( BTL_EVAR_SICK_CONT_ADRS ) );
    SICKCONT_SetTurn( cont, 8 );
  }
}

//------------------------------------------------------------------------------
/**
 *  かいがらのすず
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_KaigaraNoSuzu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_L2, handler_KaigaraNoSuzu },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_KaigaraNoSuzu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_DELAY_ATTACK_FLAG) == false)
  ){
    u32 damage_sum = EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
    if( damage_sum )
    {
      damage_sum /= common_GetItemParam( myHandle, ::item::ITEM_DATA::PRM_ID_ATTACK );
      if( damage_sum == 0 ){
        damage_sum = 1;
      }

      {
        BTL_HANDEX_PARAM_RECOVER_HP* param = reinterpret_cast<BTL_HANDEX_PARAM_RECOVER_HP *>( flowWk->HANDEX_Push( BTL_HANDEX_RECOVER_HP, pokeID ));
          param->pokeID = pokeID;
          param->recoverHP = damage_sum;
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_RecoverLittle );
          HANDEX_STR_AddArg( &param->exStr, pokeID );
          HANDEX_STR_AddArg( &param->exStr, EVENT_FACTOR_GetSubID(myHandle) );
        flowWk->HANDEX_Pop( param );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  ひかりのねんど
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_HikariNoNendo( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_SIDEEFF_PARAM, handler_HikariNoNendo },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_HikariNoNendo( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BtlSideEffect sideEff = static_cast<BtlSideEffect>(EVENTVAR_GetValue( BTL_EVAR_SIDE_EFFECT ));
    if( ( sideEff == BTL_SIDEEFF_HIKARINOKABE ) ||
        ( sideEff == BTL_SIDEEFF_REFRECTOR ) ||
        ( sideEff == BTL_SIDEEFF_AURORAVEIL ) )
    {
      BTL_SICKCONT* cont = reinterpret_cast<BTL_SICKCONT*>( EVENTVAR_GetValue( BTL_EVAR_SICK_CONT_ADRS ) );
      u8 inc_turns;
      inc_turns = static_cast<u8>(common_GetItemParam( myHandle, ::item::ITEM_DATA::PRM_ID_ATTACK ));
//      BTL_Printf("サイドエフェクトの継続ターン数を %d ターン増加\n", inc_turns);
      SICKCONT_IncTurn( cont, inc_turns );
      SICKCONT_AddParam( cont, inc_turns );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  パワフルハーブ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_PowefulHarb( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_TAMETURN_SKIP,   handler_PowefulHarb_CheckTameSkip }, // 溜めスキップチェック
    { BTL_EVENT_TAME_SKIP,             handler_PowefulHarb_FixTameSkip   }, // 溜めスキップ確定
    { BTL_EVENT_USE_ITEM,              handler_PowefulHarb_Use           }, // アイテム使用
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 溜めスキップチェック
void handler_PowefulHarb_CheckTameSkip( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_WAZAID) != WAZANO_HURIIFOORU)
  ){
    if( EVENTVAR_RewriteValue(BTL_EVAR_GEN_FLAG, true) ){
      work[0] = 1;
    }
  }
}
// 溜めスキップ確定
void handler_PowefulHarb_FixTameSkip( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( work[0] )
    {
      itemPushRun( myHandle, flowWk, pokeID );
    }
  }
}
// アイテム使用
void handler_PowefulHarb_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_TameSkip );
      HANDEX_STR_AddArg( &param->str, pokeID );
      HANDEX_STR_AddArg( &param->str, EVENT_FACTOR_GetSubID(myHandle) );
    flowWk->HANDEX_Pop( param );
  }
}

//------------------------------------------------------------------------------
/**
 *  たべのこし
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_Tabenokosi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_BEGIN, handler_Tabenokosi_Reaction },
    { BTL_EVENT_USE_ITEM,        handler_Tabenokosi_Use      },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Tabenokosi_Reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( !bpp->IsHPFull() )
    {
      BTL_HANDEX_PARAM_USE_ITEM* param = reinterpret_cast<BTL_HANDEX_PARAM_USE_ITEM *>( flowWk->HANDEX_Push( BTL_HANDEX_USE_ITEM, pokeID ));
        param->fSkipHPFull = true;
        EVENT_FACTOR_SetRecallEnable( myHandle );
      flowWk->HANDEX_Pop( param );
    }
  }
}
void handler_Tabenokosi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( !bpp->IsHPFull() )
    {
      BTL_HANDEX_PARAM_RECOVER_HP* param = reinterpret_cast<BTL_HANDEX_PARAM_RECOVER_HP *>( flowWk->HANDEX_Push( BTL_HANDEX_RECOVER_HP, pokeID ));
        param->pokeID = pokeID;
        param->recoverHP = calc::QuotMaxHP( bpp, 16 );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_RecoverLittle );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
        HANDEX_STR_AddArg( &param->exStr, EVENT_FACTOR_GetSubID(myHandle) );
      flowWk->HANDEX_Pop( param );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  くろいヘドロ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_KuroiHedoro( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_BEGIN, handler_KuroiHedoro  },  // ターンチェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ターンチェックハンドラ
void handler_KuroiHedoro( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (!flowWk->Hnd_CheckShowDown())
  ){
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

    if( PokeTypePair_IsMatch(bpp->GetPokeType(), POKETYPE_DOKU) )
    {
      BTL_HANDEX_PARAM_RECOVER_HP* param = reinterpret_cast<BTL_HANDEX_PARAM_RECOVER_HP *>( flowWk->HANDEX_Push( BTL_HANDEX_RECOVER_HP, pokeID ));
        param->pokeID = pokeID;
        param->recoverHP = calc::QuotMaxHP( bpp, common_GetItemParam(myHandle, ::item::ITEM_DATA::PRM_ID_ATTACK) );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_RecoverLittle );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
        HANDEX_STR_AddArg( &param->exStr, EVENT_FACTOR_GetSubID(myHandle) );
      flowWk->HANDEX_Pop( param );
    }
    else
    {
      BTL_HANDEX_PARAM_DAMAGE* param;

      param = reinterpret_cast<BTL_HANDEX_PARAM_DAMAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_DAMAGE, pokeID ));
        param->pokeID = pokeID;
        param->damage = calc::QuotMaxHP( bpp, 8 );
        param->damageCause = DAMAGE_CAUSE_KUROIHEDORO;
        param->damageCausePokeID = pokeID;
        param->fExEffect = true;
        param->effectNo = BTLEFF_SOUBI_ITEM;
        param->pos_from = flowWk->Hnd_PokeIDtoPokePos( pokeID );
        param->pos_to = BTL_POS_NULL;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_Damage );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
        HANDEX_STR_AddArg( &param->exStr, EVENT_FACTOR_GetSubID(myHandle) );
      flowWk->HANDEX_Pop( param );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  あかいいと
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_AkaiIto( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASICK_FIXED, handler_AkaiIto  },  // ワザ系状態異常確定
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザ系状態異常確定
void handler_AkaiIto( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_SICKID) == pml::wazadata::WAZASICK_MEROMERO)
  ){
    u8 atkPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    if( atkPokeID != BTL_POKEID_NULL )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
        param->pokeID = atkPokeID;
        param->sickID = pml::wazadata::WAZASICK_MEROMERO;
        param->sickCont = SICKCONT_MakePoke( pokeID, pokeID );
        param->fAlmost = true;
        param->fStdMsgDisable = true;

        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_MeromeroGetItem );
        HANDEX_STR_AddArg( &param->exStr, atkPokeID );
        HANDEX_STR_AddArg( &param->exStr, EVENT_FACTOR_GetSubID(myHandle) );

      flowWk->HANDEX_Pop( param );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  くっつきばり
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_KuttukiBari( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION, handler_KuttukiBari_DamageReaction  },  // ワザダメージ反応ハンドラ
    { BTL_EVENT_TURNCHECK_END,     handler_KuttukiBari_TurnCheck       },  // ターンチェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザダメージ反応ハンドラ
void handler_KuttukiBari_DamageReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == false)
  &&  (EVENTVAR_GetValue(BTL_EVAR_WAZA_TOUCH_FLAG))
  ){
    u8 atkPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    const BTL_POKEPARAM* attacker = flowWk->Hnd_GetPokeParam( atkPokeID );
    if( attacker->GetItem() == ITEM_DUMMY_DATA )
    {
      BTL_HANDEX_PARAM_SWAP_ITEM* param = reinterpret_cast<BTL_HANDEX_PARAM_SWAP_ITEM *>( flowWk->HANDEX_Push( BTL_HANDEX_SWAP_ITEM, pokeID ));
        param->pokeID = atkPokeID;
      flowWk->HANDEX_Pop( param );
    }
  }
}
// ターンチェックハンドラ
void handler_KuttukiBari_TurnCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );

    BTL_HANDEX_PARAM_DAMAGE* param;

    param = reinterpret_cast<BTL_HANDEX_PARAM_DAMAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_DAMAGE, pokeID ));
      param->pokeID = pokeID;
      param->damage = calc::QuotMaxHP( bpp, common_GetItemParam(myHandle, ::item::ITEM_DATA::PRM_ID_ATTACK) );
      param->damageCause = DAMAGE_CAUSE_KUTTUKIBARI;
      param->damageCausePokeID = pokeID;
      param->fExEffect = true;
      param->effectNo = BTLEFF_SOUBI_ITEM;
      param->pos_from = flowWk->Hnd_PokeIDtoPokePos( pokeID );
      param->pos_to = BTL_POS_NULL;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_Damage );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, EVENT_FACTOR_GetSubID(myHandle) );
    flowWk->HANDEX_Pop( param );
  }
}

//------------------------------------------------------------------------------
/**
 *  パワーリスト
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_PowerWrist( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,      handler_PowerXXX_CalcAgility  },  // すばやさ計算ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  パワーベルト
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_PowerBelt( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,      handler_PowerXXX_CalcAgility  },  // すばやさ計算ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  パワーレンズ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_PowerLens( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,      handler_PowerXXX_CalcAgility  },  // すばやさ計算ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  パワーバンド
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_PowerBand( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,      handler_PowerXXX_CalcAgility  },  // すばやさ計算ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  パワーアンクル
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_PowerAnkle( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,      handler_PowerXXX_CalcAgility  },  // すばやさ計算ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  パワーウェイト
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_PowerWeight( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,      handler_PowerXXX_CalcAgility  },  // すばやさ計算ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

/**
 *  パワー○○○（努力値アップアイテム）共通：すばやさ計算ハンドラ
 */
void handler_PowerXXX_CalcAgility( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
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
 *  ひのたまプレート
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_HinotamaPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Mokutan },  // ワザ威力ハンドラ：「もくたん」と等価
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  しずくプレート
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_SizukuPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_SinpiNoSizuku },  // ワザ威力ハンドラ：「しんぴのしずく」と等価
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  いかずちプレート
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_IkazutiPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Zisyaku },  // ワザ威力ハンドラ：「じしゃく」と等価
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  みどりのプレート
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_MirodinoPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_KisekiNoTane },  // ワザ威力ハンドラ：「きせきのたね」と等価
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  つららのプレート
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_TuraranoPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_TokenaiKoori },  // ワザ威力ハンドラ：「とけないこおり」と等価
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  こぶしのプレート
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_KobusinoPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Kuroobi },  // ワザ威力ハンドラ：「くろおび」と等価
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  もうどくプレート
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_MoudokuPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Dokubari },  // ワザ威力ハンドラ：「どくばり」と等価
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  だいちのプレート
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_DaitinoPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_YawarakaiSuna },  // ワザ威力ハンドラ：「やわらかいすな」と等価
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  あおぞらプレート
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_AozoraPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_SurudoiKutibasi },  // ワザ威力ハンドラ：「するどいくちばし」と等価
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  ふしぎのプレート
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_HusiginoPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_MagattaSupuun },  // ワザ威力ハンドラ：「まがったスプーン」と等価
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  たまむしプレート
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_TamamusiPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_GinNoKona },  // ワザ威力ハンドラ：「ぎんのこな」と等価
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  がんせきプレート
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_GansekiPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_KataiIsi },  // ワザ威力ハンドラ：「かたいいし」と等価
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  もののけプレート
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_MononokePlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_NoroiNoOfuda },  // ワザ威力ハンドラ：「のろいのおふだ」と等価
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  りゅうのプレート
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_RyuunoPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_RyuunoKiba },  // ワザ威力ハンドラ：「りゅうのキバ」と等価
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  こわもてプレート
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_KowamotePlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_KuroiMegane },  // ワザ威力ハンドラ：「くろいめがね」と等価
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  こうてつプレート
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_KoutetsuPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_MetalCoat },  // ワザ威力ハンドラ：「メタルコート」と等価
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  せいれいプレート
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_SeireiPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_SeireiPlate },  // ワザ威力ハンドラ：「メタルコート」と等価
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_SeireiPlate( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_FAIRY );
}


//------------------------------------------------------------------------------
/**
 *  おおきなねっこ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_OokinaNekko( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_DRAIN,     handler_OokinaNekko },  // ドレイン量計算ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ドレイン量計算ハンドラ
void handler_OokinaNekko( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    fx32 ratio = item_AttackValue_to_Ratio( myHandle );
    EVENTVAR_MulValue( BTL_EVAR_RATIO, ratio );
  }
}
//------------------------------------------------------------------------------
/**
 *  けむりだま
 */
//------------------------------------------------------------------------------
 const BtlEventHandlerTable*  HAND_ADD_ITEM_Kemuridama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_SKIP_NIGERU_CALC,       handler_Kemuridama     }, // にげるチェックハンドラ
    { BTL_EVENT_NIGERU_EXMSG,           handler_Kemuridama_Msg }, // 逃げ特殊メッセージハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// にげるチェックハンドラ
void handler_Kemuridama( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  HandCommon_NigeruCalcSkip( myHandle, flowWk, pokeID, work );
}
// 逃げ特殊メッセージハンドラ
void handler_Kemuridama_Msg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( HandCommon_CheckNigeruExMsg(myHandle, flowWk, pokeID) )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_Kemuridama );
      HANDEX_STR_AddArg( &param->str, pokeID );
      HANDEX_STR_AddArg( &param->str, EVENT_FACTOR_GetSubID(myHandle) );
      HANDEX_STR_AddSE( &param->str, (u32)SEQ_SE_BT_NIGERU );
    flowWk->HANDEX_Pop( param );
  }
}
//------------------------------------------------------------------------------
/**
 *  おまもりこばん
 */
//------------------------------------------------------------------------------
 const BtlEventHandlerTable*  HAND_ADD_ITEM_OmamoriKoban( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,       handler_OmamoriKoban },  // メンバー入場ハンドラ
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// メンバー入場ハンドラ
void handler_OmamoriKoban( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    flowWk->Hnd_SetMoneyDblUp( pokeID, MONEY_DBLUP_STD );
  }
}

//------------------------------------------------------------------------------
/**
 *  はっきんだま
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_ADD_ITEM_HakkinDama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_HakkinDama },
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}
// ワザ威力計算ハンドラ
void handler_HakkinDama( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->GetMonsNo() == MONSNO_GIRATHINA )
    {
      pml::PokeType wazaType = EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
      if( (wazaType == POKETYPE_DRAGON) || (wazaType == POKETYPE_GHOST) )
      {
        EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.2) );
      }
    }
  }
}




//------------------------------------------------------------------------------
/**
 *  つめたいいわ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_TumetaiIwa( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_WEATHER_TURNCNT, handler_TumetaiIwa  },  // 天候変化ワザのターン数チェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 天候変化ワザのターン数チェック
void handler_TumetaiIwa( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_WazaWeatherTurnUp( myHandle, flowWk, pokeID, BTL_WEATHER_SNOW );
}
//------------------------------------------------------------------------------
/**
 *  さらさらいわ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_SarasaraIwa( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_WEATHER_TURNCNT, handler_SarasaraIwa  },  // 天候変化ワザのターン数チェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 天候変化ワザのターン数チェック
void handler_SarasaraIwa( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_WazaWeatherTurnUp( myHandle, flowWk, pokeID, BTL_WEATHER_SAND );
}
//------------------------------------------------------------------------------
/**
 *  あついいわ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_AtuiIwa( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_WEATHER_TURNCNT, handler_AtuiIwa  },  // 天候変化ワザのターン数チェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 天候変化ワザのターン数チェック
void handler_AtuiIwa( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_WazaWeatherTurnUp( myHandle, flowWk, pokeID, BTL_WEATHER_SHINE );
}
//------------------------------------------------------------------------------
/**
 *  しめったいわ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_SimettaIwa( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_WEATHER_TURNCNT, handler_SimettaIwa  },  // 天候変化ワザのターン数チェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 天候変化ワザのターン数チェック
void handler_SimettaIwa( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_WazaWeatherTurnUp( myHandle, flowWk, pokeID, BTL_WEATHER_RAIN );
}

/*
 *  天候変化ワザターン数増加アイテムの共通ルーチン
 */
void common_WazaWeatherTurnUp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, BtlWeather weather )
{
  GFL_UNUSED(flowWk);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_WEATHER) == weather)
  ){
    EVENTVAR_RewriteValue( BTL_EVAR_TURN_COUNT, common_GetItemParam(myHandle, ::item::ITEM_DATA::PRM_ID_ATTACK) );
  }
}

//------------------------------------------------------------------------------
/**
 *  でんきだま
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_DenkiDama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_DenkiDama   },
    { BTL_EVENT_USE_ITEM_TMP,   handler_DenkiDama_UseTmp},
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 装備効果
void handler_DenkiDama( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // 自分が攻撃側で
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ピカチュウなら
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    u32 monsNo = bpp->GetMonsNo( );
    if( monsNo == MONSNO_PIKATYUU )
    {
      // こうげき・とくこう２倍
      EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
// なげつけ効果
void handler_DenkiDama_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    u8 atkPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    BTL_HANDEX_PARAM_ADD_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, atkPokeID ));
      param->pokeID = pokeID;
      param->sickID = pml::wazadata::WAZASICK_MAHI;
      param->sickCont = calc::MakeDefaultPokeSickCont( pml::pokepara::SICK_MAHI, atkPokeID );
    flowWk->HANDEX_Pop( param );
//    param->fAlmost = true;
  }
}
//------------------------------------------------------------------------------
/**
 *  どくどくだま
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_DokudokuDama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_END,    handler_DokudokuDama        },
    { BTL_EVENT_USE_ITEM_TMP,     handler_DokudokuDama_UseTmp },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_DokudokuDama( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
      param->sickID = pml::wazadata::WAZASICK_DOKU;
      param->sickCont = SICKCONT_MakeMoudokuCont( pokeID );
      param->fAlmost = false;
      param->pokeID = pokeID;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_MoudokuGetSP );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, EVENT_FACTOR_GetSubID(myHandle) );
    flowWk->HANDEX_Pop( param );
  }
}
// なげつけ効果
void handler_DokudokuDama_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    u8 atkPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    BTL_HANDEX_PARAM_ADD_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, atkPokeID ));
      param->pokeID = pokeID;
      param->sickID = pml::wazadata::WAZASICK_DOKU;
      param->sickCont = SICKCONT_MakeMoudokuCont( atkPokeID );
//    param->fAlmost = true;
    flowWk->HANDEX_Pop( param );
  }
}
//------------------------------------------------------------------------------
/**
 *  かえんだま
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_KaenDama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_END,  handler_KaenDama        },
    { BTL_EVENT_USE_ITEM_TMP,   handler_KaenDama_UseTmp },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_KaenDama( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, pokeID ));
      param->sickID = pml::wazadata::WAZASICK_YAKEDO;
      param->sickCont = calc::MakeDefaultPokeSickCont( pml::pokepara::SICK_YAKEDO, pokeID );
      param->fAlmost = false;
      param->pokeID = pokeID;
      param->exStr.args[1] = EVENT_FACTOR_GetSubID( myHandle );;

      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_YakedoGetSP );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, EVENT_FACTOR_GetSubID(myHandle) );
    flowWk->HANDEX_Pop( param );
  }
}
// なげつけ効果
void handler_KaenDama_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    u8 atkPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    BTL_HANDEX_PARAM_ADD_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, atkPokeID ));
      param->pokeID = pokeID;
      param->sickID = pml::wazadata::WAZASICK_YAKEDO;
      param->sickCont = calc::MakeDefaultPokeSickCont( static_cast<PokeSick>(param->sickID), atkPokeID );
//    param->fAlmost = true;
    flowWk->HANDEX_Pop( param );
  }
}


//------------------------------------------------------------------------------
/**
 *  ぎんのこな
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_GinNoKona( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_GinNoKona },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_GinNoKona( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_MUSHI );
}
//------------------------------------------------------------------------------
/**
 *  やわらかいすな
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_YawarakaiSuna( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_YawarakaiSuna },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_YawarakaiSuna( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_JIMEN );
}
//------------------------------------------------------------------------------
/**
 *  かたいいし
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_KataiIsi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_KataiIsi },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_KataiIsi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_IWA );
}
//------------------------------------------------------------------------------
/**
 *  きせきのたね
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_KisekiNoTane( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_KisekiNoTane },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_KisekiNoTane( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_KUSA );
}
//------------------------------------------------------------------------------
/**
 *  くろいめがね
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_KuroiMegane( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_KuroiMegane },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_KuroiMegane( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_AKU );
}
//------------------------------------------------------------------------------
/**
 *  くろおび
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_Kuroobi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Kuroobi },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Kuroobi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_KAKUTOU );
}
//------------------------------------------------------------------------------
/**
 *  じしゃく
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_Zisyaku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Zisyaku },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Zisyaku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_DENKI );
}
//------------------------------------------------------------------------------
/**
 *  メタルコート
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_MetalCoat( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_MetalCoat },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_MetalCoat( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_HAGANE );
}
//------------------------------------------------------------------------------
/**
 *  しんぴのしずく
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_SinpiNoSizuku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_SinpiNoSizuku },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_SinpiNoSizuku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_MIZU );
}
//------------------------------------------------------------------------------
/**
 *  するどいくちばし
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_SurudoiKutibasi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_SurudoiKutibasi },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_SurudoiKutibasi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_HIKOU );
}
//------------------------------------------------------------------------------
/**
 *  どくばり
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_Dokubari( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Dokubari },
    { BTL_EVENT_USE_ITEM_TMP,   handler_Dokubari_UseTmp },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Dokubari( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_DOKU );
}
// なげつけ効果
void handler_Dokubari_UseTmp( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    u8 atkPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    BTL_HANDEX_PARAM_ADD_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_ADD_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_ADD_SICK, atkPokeID ));
      param->pokeID = pokeID;
      param->sickID = pml::wazadata::WAZASICK_DOKU;
      param->sickCont = calc::MakeDefaultPokeSickCont( pml::pokepara::SICK_DOKU, atkPokeID );
    flowWk->HANDEX_Pop( param );
  }
}
//------------------------------------------------------------------------------
/**
 *  とけないこおり
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_TokenaiKoori( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_TokenaiKoori },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_TokenaiKoori( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_KOORI );
}
//------------------------------------------------------------------------------
/**
 *  のろいのおふだ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_NoroiNoOfuda( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_NoroiNoOfuda },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_NoroiNoOfuda( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_GHOST );
}
//------------------------------------------------------------------------------
/**
 *  まがったスプーン
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_MagattaSupuun( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_MagattaSupuun },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_MagattaSupuun( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_ESPER );
}
//------------------------------------------------------------------------------
/**
 *  もくたん
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_Mokutan( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Mokutan },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Mokutan( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_HONOO );
}
//------------------------------------------------------------------------------
/**
 *  りゅうのキバ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_RyuunoKiba( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_RyuunoKiba },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_RyuunoKiba( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_DRAGON );
}
//------------------------------------------------------------------------------
/**
 *  シルクのスカーフ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_SirukuNoSukaafu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_SirukuNoSukaafu },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_SirukuNoSukaafu( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_NORMAL );
}
//------------------------------------------------------------------------------
/**
 *  あやしいおこう
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_AyasiiOkou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_AyasiiOkou },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_AyasiiOkou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_ESPER );
}
//------------------------------------------------------------------------------
/**
 *  がんせきおこう
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_GansekiOkou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_GansekiOkou },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_GansekiOkou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_IWA );
}
//------------------------------------------------------------------------------
/**
 *  さざなみのおこう
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_SazanamiNoOkou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_SazanamiNoOkou },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_SazanamiNoOkou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_MIZU );
}
//------------------------------------------------------------------------------
/**
 *  うしおのおこう
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_UsioNoOkou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_UsioNoOkou },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_UsioNoOkou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_MIZU );
}
//------------------------------------------------------------------------------
/**
 *  おはなのおこう
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_OhanaNoOkou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_OhanaNoOkou },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_OhanaNoOkou( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_KUSA );
}
//----------------------------------------------------------------------------------
/**
 * とくていタイプのワザを威力アップさせる道具の共通処理
 *
 * @param   myHandle
 * @param   flowWk
 * @param   pokeID
 * @param   type
 */
//----------------------------------------------------------------------------------
void common_PowerUpSpecificType( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, pml::PokeType type )
{
  GFL_UNUSED(flowWk);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == type )
    {
      fx32 ratio = item_AttackValue_to_Ratio( myHandle );
      EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, ratio );
    }
  }
}

//------------------------------------------------------------------------------
/**
 *  こころのしずく
 *
 * 「ラティアス」「ラティオス」専用
 * 「エスパー」「ドラゴン」タイプの技の威力が１．２倍になる
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_KokoroNoSizuku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_KokoroNoSizuku },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_KokoroNoSizuku( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    const MonsNo monsno = static_cast<MonsNo>( bpp->GetMonsNo() );
    if( ( monsno == MONSNO_RATHIASU ) ||
        ( monsno == MONSNO_RATHIOSU ) )
    {
      pml::PokeType  waza_type =  EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
      if( (waza_type == POKETYPE_DRAGON) || (waza_type == POKETYPE_ESPER) )
      {
        EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.2f) );
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 *  しらたま
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_Siratama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Siratama },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Siratama( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->GetMonsNo() == MONSNO_PARUKIA )
    {
      pml::PokeType  waza_type =  EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
      if( (waza_type == POKETYPE_DRAGON) || (waza_type == POKETYPE_MIZU) )
      {
        fx32 ratio = item_AttackValue_to_Ratio( myHandle );
        EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, ratio );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  こんごうだま
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_Kongoudama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Kongoudama },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Kongoudama( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->GetMonsNo() == MONSNO_DHIARUGA )
    {
      pml::PokeType  waza_type =  EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
      if( (waza_type == POKETYPE_DRAGON) || (waza_type == POKETYPE_HAGANE) )
      {
        fx32 ratio = item_AttackValue_to_Ratio( myHandle );
        EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, ratio );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  くろいてっきゅう
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_KuroiTekkyuu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,     handler_KuroiTekkyuu_Agility  },  // すばやさ計算ハンドラ
    { BTL_EVENT_CHECK_FLYING,     handler_KuroiTekkyuu_CheckFly },  // ふゆうチェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// すばやさ計算ハンドラ
void handler_KuroiTekkyuu_Agility( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5) );
  }
}
// ふゆうチェックハンドラ
void handler_KuroiTekkyuu_CheckFly( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
  }
}
//------------------------------------------------------------------------------
/**
 *  かるいし
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_Karuisi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WEIGHT_RATIO,     handler_Karuisi },   // 体重倍率チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_Karuisi( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
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
 *  しんかのきせき
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_SinkanoKiseki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DEFENDER_GUARD,     handler_SinkanoKiseki },   // 防御側ガード力チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 防御側ガード力チェックハンドラ
void handler_SinkanoKiseki( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);


  // 自分が防御側で進化前だったら防御・特防1.5倍
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // フラエッテはフォルム hakai だったら効果なし
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( (bpp->GetMonsNo() == MONSNO_HURAETTE)
    &&  (bpp->GetFormNo() == FORMNO_714_HAKAI)
    ){
      return;
    }

    if( flowWk->Hnd_CheckSinkaMae(pokeID) )
    {
      EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.5) );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  ゴツゴツメット
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_GotugotuMet( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_GotugotuMet },   // ダメージ反応ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージ反応ハンドラ
void handler_GotugotuMet( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == false)
  &&  (EVENTVAR_GetValue(BTL_EVAR_WAZA_TOUCH_FLAG))
  ){
    // @fix NMCat[4281]対応
    // ダメージを与える
    // 
    // このダメージでは、死亡した場合でも退場処理を行いません。
    // ここで退場処理を行ってしまうと、両陣営が同時に全滅した場合の勝敗判定が反転してしまうためです。
    // 
    // 両陣営が全滅した場合、後に退場したほうの勝ちとなりますが、
    // ここで退場処理を行ってしまうと
    // 防御側が技ダメージによって退場する前に
    // 攻撃側が「ゴツゴツメット」のダメージで退場してしまうため、攻撃側が負けになってしまいます。
    BTL_HANDEX_PARAM_DAMAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_DAMAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_DAMAGE, pokeID ));
      param->pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
      {
        const BTL_POKEPARAM* target = flowWk->Hnd_GetPokeParam( param->pokeID );
        u8 itemAtk = static_cast<u8>(common_GetItemParam( myHandle, ::item::ITEM_DATA::PRM_ID_ATTACK ));
        param->damage = calc::QuotMaxHP( target, itemAtk );
        param->damageCause = DAMAGE_CAUSE_GOTUGOTUMETTO;
        param->damageCausePokeID = pokeID;
        param->fDisableDeadProcess = true;                         // 死亡した場合でも退場処理を行わない

        HANDEX_STR_Setup( &(param->exStr), BTL_STRTYPE_SET, BTL_STRID_SET_GotuGotuMet );
        HANDEX_STR_AddArg( &(param->exStr), param->pokeID );
      }
    flowWk->HANDEX_Pop( param );
  }
}
//------------------------------------------------------------------------------
/**
 *  ふうせん
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_Huusen( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,           handler_Huusen_MemberIn       },   // メンバー入場ハンドラ
    { BTL_EVENT_CHECK_FLYING,        handler_Huusen_CheckFlying    },   // 飛行フラグチェックハンドラ
    { BTL_EVENT_WAZA_DMG_REACTION,   handler_Huusen_DamageReaction },   // ダメージ反応ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// メンバー入場ハンドラ
void handler_Huusen_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( !flowWk->Hnd_CheckFieldEffect(FieldStatus::EFF_JURYOKU) )
    {
      BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_FuusenIn );
        HANDEX_STR_AddArg( &param->str, pokeID );
      flowWk->HANDEX_Pop( param );
    }
  }
}
// 飛行フラグチェックハンドラ
void handler_Huusen_CheckFlying( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, true );
  }
}
// ダメージ反応ハンドラ
void handler_Huusen_DamageReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_ITEM* param = reinterpret_cast<BTL_HANDEX_PARAM_SET_ITEM *>( flowWk->HANDEX_Push( BTL_HANDEX_SET_ITEM, pokeID ));
      param->pokeID = pokeID;
      param->itemID = ITEM_DUMMY_DATA;
      param->fCallConsumedEvent = true;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_FuusenBreak );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
    flowWk->HANDEX_Pop( param );
  }
}
// アイテム書き換え完了ハンドラ
void handler_Huusen_ItemSetFixed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // 「ふうせん」を持たされたら飛行禁止状態を解除
    BTL_HANDEX_PARAM_CURE_SICK* param = reinterpret_cast<BTL_HANDEX_PARAM_CURE_SICK *>( flowWk->HANDEX_Push( BTL_HANDEX_CURE_SICK, pokeID ));
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->sickCode = static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_FLYING_CANCEL);
      param->fStdMsgDisable = true;
    flowWk->HANDEX_Pop( param );
  }
}

//------------------------------------------------------------------------------
/**
 *  レッドカード
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_RedCard( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL,     handler_RedCard },   /// ダメージ処理終了
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
/// ダメージ処理終了ハンドラ
void handler_RedCard( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( HandCommon_CheckTargetPokeID(pokeID) )
  {
    // NMCat[2294] 対処 - 
    u8 targetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    if( (flowWk->Hnd_GetExistFrontPokePos( targetPokeID) != BTL_POS_NULL)
    &&  (EVENTVAR_GetValue(BTL_EVAR_DELAY_ATTACK_FLAG) == false )
    &&  (flowWk->Hnd_IsExistBenchPoke(targetPokeID))
    &&  (flowWk->Hnd_IsExistChangeEnablePoke(targetPokeID))
    &&  (!flowWk->Hnd_CheckShowDown())
    ){
      BTL_HANDEX_PARAM_CONSUME_ITEM* consume_param = reinterpret_cast<BTL_HANDEX_PARAM_CONSUME_ITEM *>( flowWk->HANDEX_Push( BTL_HANDEX_CONSUME_ITEM, pokeID ));
        HANDEX_STR_Setup( &consume_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_RedCard );
        HANDEX_STR_AddArg( &consume_param->exStr, pokeID );
        HANDEX_STR_AddArg( &consume_param->exStr, targetPokeID );
      flowWk->HANDEX_Pop( consume_param );

      {
        BTL_HANDEX_PARAM_PUSHOUT* push_param = reinterpret_cast<BTL_HANDEX_PARAM_PUSHOUT *>( flowWk->HANDEX_Push( BTL_HANDEX_PUSHOUT, pokeID ));
          push_param->pokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
          push_param->fIgnoreLevel = true;
          push_param->effectNo = BTLEFF_POKEMON_MODOSU;
        flowWk->HANDEX_Pop( push_param );
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 *  ねらいのまと
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_NerainoMato( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_AFFINITY,     handler_NerainoMato },   // タイプ相性による無効化チェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// タイプ相性による無効化チェック
void handler_NerainoMato( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  // シミュレーション時にはなにもしない
  if( !flowWk->Hnd_IsSimulationMode() )
  {
    // 自分が防御側の時、相性により無効化されていれば有効に書き換え
    if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_FLAT_FLAG, true );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  しめつけバンド
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_SimetukeBand( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASICK_PARAM,     handler_SimetukeBand },   // 状態異常パラメータチェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 状態異常パラメータチェックハンドラ
void handler_SimetukeBand( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_SICKCONT* cont = reinterpret_cast<BTL_SICKCONT*>( EVENTVAR_GetValue( BTL_EVAR_SICK_CONT_ADRS ) );
    SICKCONT_SetFlag( cont, true );
  }
}
//------------------------------------------------------------------------------
/**
 *  きゅうこん
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_Kyuukon( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_Kyuukon_DmgReaction },   // ダメージ反応ハンドラ
    { BTL_EVENT_USE_ITEM,              handler_Kyuukon_Use   },         // どうぐ使用ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージ反応ハンドラ
void handler_Kyuukon_DmgReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  // 自分が水タイプのワザダメージを受けた時、特攻がまだ上がるなら反応する
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == false)
  ){
    if( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_MIZU )
    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
      if( bpp->IsRankEffectValid(BTL_POKEPARAM::BPP_SP_ATTACK_RANK, 1) )
      {
        itemPushRun( myHandle, flowWk, pokeID );
      }
    }
  }
}
// どうぐ使用ハンドラ
void handler_Kyuukon_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
      param->poke_cnt   = 1;
      param->pokeID[0]  = pokeID;
      param->rankType   = pml::wazadata::RANKEFF_SP_ATTACK;
      param->rankVolume = 1;
    flowWk->HANDEX_Pop( param );
  }
}
//------------------------------------------------------------------------------
/**
 *  アッキのみ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_AkkiNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL, handler_AkkiNomi_DmgReaction   }, // ダメージ反応ハンドラ
    { BTL_EVENT_USE_ITEM,                handler_AkkiNomi_Use           }, // アイテム使用ハンドラ
    { BTL_EVENT_USE_ITEM_TMP,            handler_AkkiNomi_Use           },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージ反応ハンドラ
void handler_AkkiNomi_DmgReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  // 自分が物理ダメージを受けた時、防御がまだ上がるなら反応する
  if( HandCommon_CheckTargetPokeID(pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == pml::wazadata::DMG_PHYSIC)
  ){
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    if( bpp->IsRankEffectValid(BTL_POKEPARAM::BPP_DEFENCE_RANK, 1) )
    {
      itemPushRun( myHandle, flowWk, pokeID );
    }
  }
}
// アイテム使用ハンドラ
void handler_AkkiNomi_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // なげつける・ついばむでココに来ることをふまえ上昇余地のチェックはもう１回やる
    const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
    const BTL_POKEPARAM::ValueID  rankType = BTL_POKEPARAM::BPP_DEFENCE_RANK;
    const int  rankupVolume = 1;
    if( bpp->IsRankEffectValid(rankType, rankupVolume) )
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
        param->poke_cnt   = 1;
        param->pokeID[0]  = pokeID;
        param->rankType   = (WazaRankEffect)rankType;
        param->rankVolume = rankupVolume;
      flowWk->HANDEX_Pop( param );
    }
  }
}


//------------------------------------------------------------------------------
/**
 *  ひかりごけ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_HikariGoke( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_HikariGoke_DmgReaction },   // ダメージ反応ハンドラ
    { BTL_EVENT_USE_ITEM,              handler_HikariGoke_Use   },         // どうぐ使用ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージ反応ハンドラ
void handler_HikariGoke_DmgReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  // 自分が水タイプのワザダメージを受けた時、特攻がまだ上がるなら反応する
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == false)
  ){
    if( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_MIZU )
    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
      if( bpp->IsRankEffectValid(BTL_POKEPARAM::BPP_SP_DEFENCE_RANK, 1) )
      {
        itemPushRun( myHandle, flowWk, pokeID );
      }
    }
  }
}
// どうぐ使用ハンドラ
void handler_HikariGoke_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
      param->poke_cnt   = 1;
      param->pokeID[0]  = pokeID;
      param->rankType   = pml::wazadata::RANKEFF_SP_DEFENCE;
      param->rankVolume = 1;
    flowWk->HANDEX_Pop( param );
  }
}
//------------------------------------------------------------------------------
/**
 * ゆきだま・じゅうでんち共通（特定タイプワザダメージ受けで、能力ランク上昇）
 */
//------------------------------------------------------------------------------
// ダメージ反応ハンドラより
void hand_common_TypeDamageRankUp_Reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, pml::PokeType wazaType, BTL_POKEPARAM::ValueID rankValue )
{
  // 自分が特定タイプのワザダメージを受けた時、ランクがまだ上がるなら反応する
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == false)
  ){
    if( EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == wazaType )
    {
      const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
      if( bpp->IsRankEffectValid( rankValue, 1) )
      {
        itemPushRun( myHandle, flowWk, pokeID );
      }
    }
  }
}


//------------------------------------------------------------------------------
/**
 * ゆきだま
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_Yukidama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_Yukidama_DmgReaction },   // ダメージ反応ハンドラ
    { BTL_EVENT_USE_ITEM,              handler_Juudenti_Use         },   // どうぐ使用ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージ反応ハンドラ
void handler_Yukidama_DmgReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  // 自分がこおりタイプのワザダメージを受けた時、攻撃がまだ上がるなら反応する
  hand_common_TypeDamageRankUp_Reaction( myHandle, flowWk, pokeID, POKETYPE_KOORI, BTL_POKEPARAM::BPP_ATTACK_RANK );
}

//------------------------------------------------------------------------------
/**
 *  じゅうでんち
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_Juudenti( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_Juudenti_DmgReaction },   // ダメージ反応ハンドラ
    { BTL_EVENT_USE_ITEM,              handler_Juudenti_Use   },         // どうぐ使用ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージ反応ハンドラ
void handler_Juudenti_DmgReaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);

  // 自分が電気タイプのワザダメージを受けた時、攻撃がまだ上がるなら反応する
  hand_common_TypeDamageRankUp_Reaction( myHandle, flowWk, pokeID, POKETYPE_DENKI, BTL_POKEPARAM::BPP_ATTACK_RANK );
}
// どうぐ使用ハンドラ
void handler_Juudenti_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT *>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ));
      param->poke_cnt   = 1;
      param->pokeID[0]  = pokeID;
      param->rankType   = pml::wazadata::RANKEFF_ATTACK;
      param->rankVolume = 1;
    flowWk->HANDEX_Pop( param );
  }
}

//------------------------------------------------------------------------------
/**
 *  だっしゅつボタン
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_DassyutuButton( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL,     handler_DassyutuButton_Reaction },  // ダメージ処理終了ハンドラ
    { BTL_EVENT_USE_ITEM,                handler_DassyutuButton_Use      },  // どうぐ使用ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージ処理終了ハンドラ
void handler_DassyutuButton_Reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  // 自分がダメージを受けた側
  if( (HandCommon_CheckTargetPokeID(pokeID))
  &&  (EVENTVAR_GetValue(BTL_EVAR_DELAY_ATTACK_FLAG) == false )
  &&  (flowWk->Hnd_IsFreeFallPoke(pokeID) == false)
  &&  (!flowWk->Hnd_CheckShowDown())
  ){
    // 控えに交替可能メンバーがいるなら、どうぐ使用ハンドラ呼び出し
    //（割り込みアクション時＆すでにメンバー入れ替え行動を取ろうとしているポケモンがいる場合を除く）
    if( (!flowWk->Hnd_IsMemberOutIntr())
    &&  (flowWk->Hnd_IsExistBenchPoke(pokeID))
    &&  (flowWk->Hnd_IsExistChangeEnablePoke(pokeID))
    &&  (!flowWk->Hnd_CheckReservedMemberChangeAction())
    ){
      itemPushRun( myHandle, flowWk, pokeID );
    }
  }
}
// どうぐ使用ハンドラ
void handler_DassyutuButton_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( flowWk->Hnd_IsExistBenchPoke( pokeID) )
    {
      #if 0
      BTL_HANDEX_PARAM_PUSHOUT* param = flowWk->HANDEX_Push( BTL_HANDEX_PUSHOUT, pokeID );
        param->pokeID = pokeID;
        param->fForceChange = true;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_DassyutuPod );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
      flowWk->HANDEX_Pop( param );
      #else
      {
        // おいうちを受けるタイミングを作るためにメッセージだけをまずは出す
        BTL_HANDEX_PARAM_MESSAGE* msgParam = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE*>
                                              (flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
          HANDEX_STR_Setup( &msgParam->str, BTL_STRTYPE_SET, BTL_STRID_SET_DassyutuPod );
          HANDEX_STR_AddArg( &msgParam->str, pokeID );
        flowWk->HANDEX_Pop( msgParam );
      }
      {
        BTL_HANDEX_PARAM_CHANGE_MEMBER* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_MEMBER *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_MEMBER, pokeID ));
          param->pokeID = pokeID;
          param->fIntrDisable = true;
//        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_DassyutuPod );
//        HANDEX_STR_AddArg( &param->exStr, pokeID );
        flowWk->HANDEX_Pop( param );
      }
      #endif
    }
  }
}

//------------------------------------------------------------------------------
/**
 *  ぼうじんゴーグル
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_BoujinGoggle( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WEATHER_REACTION,    handler_BoujinGoggle_CalcDamage   }, // 天候ダメージ計算ハンドラ
    { BTL_EVENT_NOEFFECT_CHECK_TOKUSEI,   handler_BoujinGoggle_WazaNoEffect }, // ワザ無効化チェックハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
// 天候ダメージ計算ハンドラ
void handler_BoujinGoggle_CalcDamage( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);

  HandCommon_Boujin_WeatherReaction( pokeID );
}
// ワザ無効化チェックハンドラ
void handler_BoujinGoggle_WazaNoEffect( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    if( HandCommon_Boujin_WazaNoEffect(pokeID) )
    {
      u32 itemID = EVENT_FACTOR_GetSubID( myHandle );
      u32 wazaID = EVENTVAR_GetValue( BTL_EVAR_WAZAID );

      BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_BoujinGoggle );
        HANDEX_STR_AddArg( &param->str, pokeID );
        HANDEX_STR_AddArg( &param->str, itemID );
        HANDEX_STR_AddArg( &param->str, wazaID );
      flowWk->HANDEX_Pop( param );
    }
  }
}

//------------------------------------------------------------------------------
/**
 *  ○○○ジュエル
 */
//------------------------------------------------------------------------------

/**
 *  ほのおのジュエル
 */
const BtlEventHandlerTable* HAND_ADD_ITEM_HonooNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_HonooNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_HonooNoJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq     },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_HonooNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_HONOO );
}
void handler_HonooNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_HONOO );
}
/**
 *  みずのジュエル
 */
const BtlEventHandlerTable* HAND_ADD_ITEM_MizuNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_MizuNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_MizuNoJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq    },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_MizuNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_MIZU );
}
void handler_MizuNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_MIZU );
}
/**
 *  でんきのジュエル
 */
const BtlEventHandlerTable* HAND_ADD_ITEM_DenkiNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_DenkiNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_DenkiNoJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq     },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_DenkiNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_DENKI );
}
void handler_DenkiNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_DENKI );
}
/**
 *  くさのジュエル
 */
const BtlEventHandlerTable* HAND_ADD_ITEM_KusaNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_KusaNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_KusaNoJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq    },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_KusaNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_KUSA );
}
void handler_KusaNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_KUSA );
}
/**
 *  こおりのジュエル
 */
const BtlEventHandlerTable* HAND_ADD_ITEM_KooriNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_KooriNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_KooriNoJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq     },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_KooriNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_KOORI );
}
void handler_KooriNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_KOORI );
}
/**
 *  かくとうジュエル
 */
const BtlEventHandlerTable* HAND_ADD_ITEM_KakutouJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_KakutouJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_KakutouJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq     },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_KakutouJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_KAKUTOU );
}
void handler_KakutouJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_KAKUTOU );
}
/**
 *  どくのジュエル
 */
const BtlEventHandlerTable* HAND_ADD_ITEM_DokuNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_DokuNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_DokuNoJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq    },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_DokuNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_DOKU );
}
void handler_DokuNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_DOKU );
}
/**
 *  じめんのジュエル
 */
const BtlEventHandlerTable* HAND_ADD_ITEM_JimenNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_JimenNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_JimenNoJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq     },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_JimenNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_JIMEN );
}
void handler_JimenNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_JIMEN );
}
/**
 *  ひこうのジュエル
 */
const BtlEventHandlerTable* HAND_ADD_ITEM_HikouNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_HikouNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_HikouNoJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq     },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_HikouNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_HIKOU );
}
void handler_HikouNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_HIKOU );
}
/**
 *  エスパージュエル
 */
const BtlEventHandlerTable* HAND_ADD_ITEM_EsperJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_EsperJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_EsperJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq   },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_EsperJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_ESPER );
}
void handler_EsperJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_ESPER );
}
/**
 *  むしのジュエル
 */
const BtlEventHandlerTable* HAND_ADD_ITEM_MusiNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_MusiNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_MusiNoJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq    },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_MusiNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_MUSHI );
}
void handler_MusiNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_MUSHI );
}
/**
 *  いわのジュエル
 */
const BtlEventHandlerTable* HAND_ADD_ITEM_IwaNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_IwaNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_IwaNoJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq   },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_IwaNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_IWA );
}
void handler_IwaNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_IWA );
}
/**
 *  ゴーストジュエル
 */
const BtlEventHandlerTable* HAND_ADD_ITEM_GhostJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_GhostJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_GhostJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq   },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_GhostJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_GHOST );
}
void handler_GhostJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_GHOST );
}
/**
 *  ドラゴンジュエル
 */
const BtlEventHandlerTable* HAND_ADD_ITEM_DragonJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_DragonNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_DragonNoJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq      },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_DragonNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_DRAGON );
}
void handler_DragonNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_DRAGON );
}
/**
 *  あくのジュエル
 */
const BtlEventHandlerTable* HAND_ADD_ITEM_AkuNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_AkuNoJuel_Decide  },
    { BTL_EVENT_WAZA_POWER,             handler_AkuNoJuel_Pow     },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq    },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_AkuNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_AKU );
}
void handler_AkuNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_AKU );
}
/**
 *  はがねのジュエル
 */
const BtlEventHandlerTable* HAND_ADD_ITEM_HaganeNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_HaganeNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_HaganeNoJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq      },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_HaganeNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_HAGANE );
}
void handler_HaganeNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_HAGANE );
}
/**
 *  ノーマルジュエル
 */
const BtlEventHandlerTable* HAND_ADD_ITEM_NormalJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_NormalJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_NormalJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq    },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_NormalJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_NORMAL );
}
void handler_NormalJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_NORMAL );
}
/**
 *  ようせいジュエル
 */
const BtlEventHandlerTable* HAND_ADD_ITEM_YouseiJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_YouseiJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_YouseiJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq      },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}
void handler_YouseiJuel_Pow( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_FAIRY );
}
void handler_YouseiJuel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_FAIRY );
}




enum {
  JUEL_SEQ_NULL = 0,
  JUEL_SEQ_STANDBY,
  JUEL_SEQ_END,
};

// ジュエル：ダメージ処理プロセス開始ハンドラ共通
void common_Juel_Decide( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work, pml::PokeType type )
{
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == type)
  ){
    WazaID waza = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    if( (WAZADATA_IsDamage(waza))
    &&  (!tables::IsCombiWaza(waza) )
    &&  (!EVENT_FACTOR_IsIsolateMode(myHandle))
    ){
      if( !flowWk->Hnd_IsSimulationMode() )
      {
        BTL_HANDEX_PARAM_CONSUME_ITEM* param;

        EVENT_FACTOR_ConvertForIsolate( myHandle );

        param = reinterpret_cast<BTL_HANDEX_PARAM_CONSUME_ITEM *>( flowWk->HANDEX_Push( BTL_HANDEX_CONSUME_ITEM, pokeID ));
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, BTL_STRID_STD_Juel );
          HANDEX_STR_AddArg( &param->exStr, EVENT_FACTOR_GetSubID(myHandle) );
          HANDEX_STR_AddArg( &param->exStr, EVENTVAR_GetValue(BTL_EVAR_WAZAID) );
        flowWk->HANDEX_Pop( param );

        work[0] = JUEL_SEQ_STANDBY;
      }
    }
  }
}
// ジュエル：威力計算ハンドラ共通
void common_Juel_Power( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work, pml::PokeType type )
{
  GFL_UNUSED(myHandle);
  // 自分が攻撃側＆タイプがマッチしていたら威力を増加
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == type)
  &&  ((work[0] == JUEL_SEQ_STANDBY) || (flowWk->Hnd_IsSimulationMode()))
  ){
    EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.3f) );
  }
}
// ジュエル：ダメージ処理プロセス終了ハンドラ共通
void handler_Juel_EndDmgSeq( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  if( (EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (work[0] == JUEL_SEQ_STANDBY)
  ){
    work[0] = JUEL_SEQ_END;
  }
}

//カイグラ進化 
/*
//あいいろのたま
//カイオーガ進化用
*/
void handler_Aiironotama_Shinka( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( bpp->GetMonsNo() == MONSNO_KAIOOGA && bpp->GetFormNo() == FORMNO_KAIOOGA_NORMAL )
    {
      BTL_HANDEX_PARAM_CHANGE_FORM* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_FORM *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_FORM, pokeID ));
        param->pokeID = pokeID;
        param->formNo = FORMNO_KAIOOGA_ALPHA;
        param->header.tokwin_flag = false;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_AlphaForm_00 );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
      flowWk->HANDEX_Pop( param );
    }
  }
}

const BtlEventHandlerTable* HAND_ADD_ITEM_Aiironotama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN_EVO,       handler_Aiironotama_Shinka },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

/*
//あいいろのたま
//カイオーガ進化用
*/
void handler_Beniironotama_Shinka( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  const BTL_POKEPARAM* bpp = flowWk->Hnd_GetPokeParam( pokeID );
  
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( bpp->GetMonsNo() == MONSNO_GURAADON && bpp->GetFormNo() == FORMNO_GURAADON_NORMAL )
    {
      BTL_HANDEX_PARAM_CHANGE_FORM* param = reinterpret_cast<BTL_HANDEX_PARAM_CHANGE_FORM *>( flowWk->HANDEX_Push( BTL_HANDEX_CHANGE_FORM, pokeID ));
        param->pokeID = pokeID;
        param->formNo = FORMNO_GURAADON_OMEGA;
        param->header.tokwin_flag = false;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_OmegaForm_01 );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
      flowWk->HANDEX_Pop( param );
    }
  }
}

const BtlEventHandlerTable* HAND_ADD_ITEM_Beniironotama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN_EVO,       handler_Beniironotama_Shinka },
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  グランドコート
 *
 * 地形効果の継続ターンを8ターンにする
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_GroundCoat( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = 
  {
    { BTL_EVENT_CHANGE_GROUND_BEFORE,  handler_GroundCoat },  // 地形効果変更前
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// 地形効果変更前
void handler_GroundCoat( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);

  // 自分が地形を変えた場合にのみ適用
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) != pokeID )
  {
    return;
  }

  // 地形の継続ターン数を増加
  {
    u8 addTurnCount = EVENTVAR_GetValue( BTL_EVAR_TURN_COUNT );
    addTurnCount += 3;
    EVENTVAR_RewriteValue( BTL_EVAR_TURN_COUNT, addTurnCount );
  }
}

//------------------------------------------------------------------------------
/**
 *  ビビリだま
 *
 * いかくされると素早さが上がる消費道具。
 *
 * work[0] == 0  デフォルト
 * work[0] == 1  「いかく」を受けた状態
 * work[0] == 2  「いかく」を受けた後、自身を使用した状態
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_Bibiridama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = 
  {
    { BTL_EVENT_RANKEFF_LAST_CHECK,  handler_Bibiridama_RankEffectLastCheck },  // ランク増減成否チェック
    { BTL_EVENT_RANKEFF_FAILED,      handler_Bibiridama_RankEffectFailed    },  // 能力ランク増減失敗確定
    { BTL_EVENT_RANKEFF_FIXED,       handler_Bibiridama_RankEffectFailed    },  // 能力ランク増減処理後
    { BTL_EVENT_USE_ITEM,            handler_Bibiridama_Use                 },  // どうぐ使用ハンドラ
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// ランク増減成否チェック
void handler_Bibiridama_RankEffectLastCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);

  if( ( work[0] == 0 ) &&
      ( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID ) &&
      ( EVENTVAR_GetValue(BTL_EVAR_RANKEFFECT_CAUSE) == RANKEFFECT_CAUSE_IKAKU ) )
  {
    work[0] = 1;
  }
}

// 能力ランク増減失敗確定
void handler_Bibiridama_RankEffectFailed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) != pokeID )
  {
    return;
  }

  if( work[0] == 1 )
  {
    work[0] = 2;
    itemPushRun( myHandle, flowWk, pokeID );
  }
}

// どうぐ使用ハンドラ
void handler_Bibiridama_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) != pokeID )
  {
    return;
  }

  // すばやさ上昇
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT*>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ) );
      param->poke_cnt   = 1;
      param->pokeID[0]  = pokeID;
      param->rankType   = pml::wazadata::RANKEFF_AGILITY;
      param->rankVolume = 1;
    flowWk->HANDEX_Pop( param );
  }
}

//------------------------------------------------------------------------------
/**
 * エレキシード
 *
 * エレキフィールドになった時、消費して「ぼうぎょ」が１段階あがる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_ElecSeed( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = 
  {
    { BTL_EVENT_MEMBER_IN_EVO,        handler_ElecSeed_MemberIn          },  // メンバー入場チェックハンドラ
    { BTL_EVENT_CHANGE_GROUND_AFTER,  handler_ElecSeed_ChangeGroundAfter },  // 地形効果変更後
    { BTL_EVENT_USE_ITEM,             handler_ElecSeed_Use               },  // どうぐ使用ハンドラ
    { BTL_EVENT_CHECK_ITEM_REACTION,  handler_ElecSeed_MemberIn          },  // アイテム反応チェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// メンバー入場チェックハンドラ
void handler_ElecSeed_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Seed_MemberIn( myHandle, flowWk, pokeID, BTL_GROUND_ELEKI );
}

// 地形効果変更後
void handler_ElecSeed_ChangeGroundAfter( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_Seed_ChangeGroundAfter( myHandle, flowWk, pokeID, BTL_GROUND_ELEKI );
}

// どうぐ使用ハンドラ
void handler_ElecSeed_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_Seed_Use( flowWk, pokeID, pml::wazadata::RANKEFF_DEFENCE );
}

//------------------------------------------------------------------------------
/**
 * サイコシード
 *
 * サイコフィールドになった時、消費して「とくぼう」が１段階あがる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_PhychoSeed( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = 
  {
    { BTL_EVENT_MEMBER_IN_EVO,       handler_PhychoSeed_MemberIn          },  // メンバー入場チェックハンドラ
    { BTL_EVENT_CHANGE_GROUND_AFTER, handler_PhychoSeed_ChangeGroundAfter },  // 地形効果変更後
    { BTL_EVENT_USE_ITEM,            handler_PhychoSeed_Use               },  // どうぐ使用ハンドラ
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_PhychoSeed_MemberIn          },  // アイテム反応チェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// メンバー入場チェックハンドラ
void handler_PhychoSeed_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Seed_MemberIn( myHandle, flowWk, pokeID, BTL_GROUND_PHYCHO );
}

// 地形効果変更後
void handler_PhychoSeed_ChangeGroundAfter( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_Seed_ChangeGroundAfter( myHandle, flowWk, pokeID, BTL_GROUND_PHYCHO );
}

// どうぐ使用ハンドラ
void handler_PhychoSeed_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_Seed_Use( flowWk, pokeID, pml::wazadata::RANKEFF_SP_DEFENCE );
}

//------------------------------------------------------------------------------
/**
 * ミストシード
 *
 * ミストフィールドになった時、消費して「とくぼう」が１段階あがる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_MistSeed( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = 
  {
    { BTL_EVENT_MEMBER_IN_EVO,       handler_MistSeed_MemberIn          },  // メンバー入場チェックハンドラ
    { BTL_EVENT_CHANGE_GROUND_AFTER, handler_MistSeed_ChangeGroundAfter },  // 地形効果変更後
    { BTL_EVENT_USE_ITEM,            handler_MistSeed_Use               },  // どうぐ使用ハンドラ
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_MistSeed_MemberIn          },  // アイテム反応チェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// メンバー入場チェックハンドラ
void handler_MistSeed_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Seed_MemberIn( myHandle, flowWk, pokeID, BTL_GROUND_MIST );
}

// 地形効果変更後
void handler_MistSeed_ChangeGroundAfter( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_Seed_ChangeGroundAfter( myHandle, flowWk, pokeID, BTL_GROUND_MIST );
}

// どうぐ使用ハンドラ
void handler_MistSeed_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_Seed_Use( flowWk, pokeID, pml::wazadata::RANKEFF_SP_DEFENCE );
}

//------------------------------------------------------------------------------
/**
 * グラスシード
 *
 * グラスフィールドになった時、消費して「ぼうぎょ」が１段階あがる。
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable* HAND_ADD_ITEM_GrassSeed( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = 
  {
    { BTL_EVENT_MEMBER_IN_EVO,       handler_GrassSeed_MemberIn          },  // メンバー入場チェックハンドラ
    { BTL_EVENT_CHANGE_GROUND_AFTER, handler_GrassSeed_ChangeGroundAfter },  // 地形効果変更後
    { BTL_EVENT_USE_ITEM,            handler_GrassSeed_Use               },  // どうぐ使用ハンドラ
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_GrassSeed_MemberIn          },  // アイテム反応チェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

// メンバー入場チェックハンドラ
void handler_GrassSeed_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  common_Seed_MemberIn( myHandle, flowWk, pokeID, BTL_GROUND_GRASS );
}

// 地形効果変更後
void handler_GrassSeed_ChangeGroundAfter( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(work);
  common_Seed_ChangeGroundAfter( myHandle, flowWk, pokeID, BTL_GROUND_GRASS );
}

// どうぐ使用ハンドラ
void handler_GrassSeed_Use( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(work);
  common_Seed_Use( flowWk, pokeID, pml::wazadata::RANKEFF_DEFENCE );
}

/**
 * @brief ポケモンが入場した際の「○○シード」の共通処理
 * @param pokeID  シードを持っているポケモン
 * @param ground  反応対象の地形
 */
void common_Seed_MemberIn( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, BtlGround ground )
{
  if( ( EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID ) &&
      ( flowWk->Hnd_GetGround() == ground ) )
  {
    itemPushRun( myHandle, flowWk, pokeID );
  }
}

/**
 * @brief 地形効果変更後の「○○シード」の共通処理
 * @param pokeID  シードを持っているポケモン
 * @param ground  変更後の地形効果
 */
void common_Seed_ChangeGroundAfter( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, BtlGround ground )
{
  if( EVENTVAR_GetValue(BTL_EVAR_GROUND) == ground )
  {
    itemPushRun( myHandle, flowWk, pokeID );
  }
}

/**
 * @brief 「○○シード」の使用処理
 * @param pokeID    シードを持っているポケモン
 * @param rankType  使用時に上昇するパラメータの種類
 */
void common_Seed_Use( ServerFlow* flowWk, u8 pokeID, WazaRankEffect rankType )
{
  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) != pokeID )
  {
    return;
  }

  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = reinterpret_cast<BTL_HANDEX_PARAM_RANK_EFFECT*>( flowWk->HANDEX_Push( BTL_HANDEX_RANK_EFFECT, pokeID ) );
      param->poke_cnt   = 1;
      param->pokeID[0]  = pokeID;
      param->rankType   = rankType;
      param->rankVolume = 1;
    flowWk->HANDEX_Pop( param );
  }
}

//------------------------------------------------------------------------------
/**
 * ぼうごパット
 *
 * 接触技に対するカウンター効果を防ぐ
 */
//------------------------------------------------------------------------------
const BtlEventHandlerTable*  HAND_ADD_ITEM_BougoPad( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_SIMPLE_DAMAGE_ENABLE,     handler_BougoPad_SimpleDamageEnable     }, // ワザ以外ダメージチェックハンドラ
    { BTL_EVENT_SIMPLE_DAMAGE_FAILED,     handler_BougoPad_SimpleDamageFailed     }, // ワザ以外ダメージ失敗
    { BTL_EVENT_ADDSICK_CHECKFAIL,        handler_BougoPad_AddSick_CheckFail      }, // ポケモン系状態異常処理ハンドラ
    { BTL_EVENT_RANKEFF_LAST_CHECK,       handler_BougoPad_RankEffect_LastCheck   }, // ランクダウン成功チェック
    { BTL_EVENT_RANKEFF_FAILED,           handler_BougoPad_RankEffect_Failed      }, // 能力ランク増減失敗確定
    { BTL_EVENT_TOKUSEI_CHANGE_CHECKFAIL, handler_BougoPad_TokuseiChangeCheckFail }, // 特性の変更が可能かチェック
    { BTL_EVENT_TOKUSEI_CHANGE_FAILED,    handler_BougoPad_TokuseiChangeFailed    }, // 特性の変更に失敗
  };
  *numElems = GFL_NELEMS(HandlerTable);
  return HandlerTable;
}

enum
{
  BOUGOPAD_WORK_INDEX_GUARD,    // ぼうごパットによって防いだか？
  BOUGOPAD_WORK_INDEX_MESSAGE,  // ぼうごパットによって防いだメッセージを表示するか？
};

// ワザ以外ダメージチェックハンドラ
void handler_BougoPad_SimpleDamageEnable( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) != pokeID )
  {
    return;
  }

  // 防護対象リスト
  static const struct 
  {
    DamageCause damageCause;   // ダメージの原因
    bool        messageEnable; // ダメージを防いだ後、メッセージ表示を行うか？
  }
  GUARD_TARGETS[] =
  {
    { DAMAGE_CAUSE_SAMEHADA,       true  },
    { DAMAGE_CAUSE_TETUNOTOGE,     true  },
    { DAMAGE_CAUSE_YUUBAKU,        true  },
    { DAMAGE_CAUSE_NEEDLEGUARD,    false },
    { DAMAGE_CAUSE_GOTUGOTUMETTO,  false },
  };

  for( u32 i=0; i<GFL_NELEMS(GUARD_TARGETS); ++i )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_DAMAGE_CAUSE) == GUARD_TARGETS[i].damageCause )
    {
      if( EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, false ) )
      {
        BougoPad_Guard( work, GUARD_TARGETS[i].messageEnable );
      }
      break;
    }
  }
}

// ワザ以外ダメージ失敗
void handler_BougoPad_SimpleDamageFailed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) != pokeID )
  {
    return;
  }

  BougoPad_GuardMessage( flowWk, pokeID, work );
}

// ポケモン系状態異常処理ハンドラ
void handler_BougoPad_AddSick_CheckFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) != pokeID )
  {
    return;
  }

  static const SickCause FAIL_CAUSE[] = 
  {
    SICK_CAUSE_TOKUSEI_TOUCH_COUNTER,
    SICK_CAUSE_TOOTIKA,
  };

  for( u32 i=0; i<GFL_NELEMS(FAIL_CAUSE); ++i )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_SICK_CAUSE) == FAIL_CAUSE[i] )
    {
      EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true );
      break;
    }
  }
}

// ランクダウン成功チェック
void handler_BougoPad_RankEffect_LastCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) != pokeID )
  {
    return;
  }

  // 防護対象リスト
  static const struct 
  {
    RankEffectCause effectCause;   // ランク効果の原因
    bool            messageEnable; // ランク効果を防いだ後、メッセージ表示を行うか？
  }
  GUARD_TARGETS[] = 
  {
    { RANKEFFECT_CAUSE_NUMENUME,   true },
    { RANKEFFECT_CAUSE_KINGSHIELD, false },
  };

  for( u32 i=0; i<GFL_NELEMS(GUARD_TARGETS); ++i )
  {
    if( EVENTVAR_GetValue(BTL_EVAR_RANKEFFECT_CAUSE) == GUARD_TARGETS[i].effectCause )
    {
      if( EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true ) )
      {
        BougoPad_Guard( work, GUARD_TARGETS[i].messageEnable );
      }
      break;
    }
  }
}

// 能力ランク増減失敗確定
void handler_BougoPad_RankEffect_Failed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) != pokeID )
  {
    return;
  }

  BougoPad_GuardMessage( flowWk, pokeID, work );
}

// 特性の変更が可能かチェック
void handler_BougoPad_TokuseiChangeCheckFail( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) != pokeID )
  {
    return;
  }

  if( EVENTVAR_GetValue(BTL_EVAR_TOKUSEI_CHANGE_CAUSE) == TOKUSEI_CHANGE_CAUSE_MIIRA )
  {
    if( EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, true ) )
    {
      BougoPad_Guard( work, true );
    }
  }
}

// 特性の変更に失敗
void handler_BougoPad_TokuseiChangeFailed( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work )
{
  GFL_UNUSED(myHandle);
  GFL_UNUSED(flowWk);
  GFL_UNUSED(work);

  if( EVENTVAR_GetValue(BTL_EVAR_POKEID) != pokeID )
  {
    return;
  }

  BougoPad_GuardMessage( flowWk, pokeID, work );
}

/**
 * @brief 「ぼうごパット」共通処理：防いだことを記録する
 * @param isGuardMessageEnable  防いだメッセージを表示するか？
 */
void BougoPad_Guard( int* work, bool isGuardMessageEnable )
{
  work[ BOUGOPAD_WORK_INDEX_GUARD ] = 1;
  work[ BOUGOPAD_WORK_INDEX_MESSAGE ] = ( isGuardMessageEnable ) ? ( 1 ) : ( 0 );
}

/**
 * @brief 「ぼうごパット」共通処理：防いだメッセージを表示
 */
void BougoPad_GuardMessage( ServerFlow* flowWk, u8 pokeID, int* work )
{
  if( work[ BOUGOPAD_WORK_INDEX_GUARD ] &&
      work[ BOUGOPAD_WORK_INDEX_MESSAGE ] )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = reinterpret_cast<BTL_HANDEX_PARAM_MESSAGE *>( flowWk->HANDEX_Push( BTL_HANDEX_MESSAGE, pokeID ));
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_BougoPat );
      HANDEX_STR_AddArg( &param->str, pokeID );
      HANDEX_STR_SetFailMsgFlag( &param->str );
    flowWk->HANDEX_Pop( param );
  }

  work[ BOUGOPAD_WORK_INDEX_GUARD ] = 0;
  work[ BOUGOPAD_WORK_INDEX_MESSAGE ] = 0;
}



//----------------------------------------------------------------------------------
} // end of namespace 'item'
} // end of namespace 'handler'
GFL_NAMESPACE_END(btl)



//=============================================================================================
/**
 * @file  hand_common.h
 * @brief ポケモンXY バトルシステム イベントファクター 共通ヘッダ
 * @author  taya
 *
 * @date  2010.02.03  作成
 */
//=============================================================================================
#ifndef __BTL_HAND_COMMON_H__
#define __BTL_HAND_COMMON_H__

#include "pml/include/item/item.h"

#include "../btl_common.h"
#include "../btl_ServerFlow.h"
#include "../btl_EventFactor.h"


GFL_NAMESPACE_BEGIN(btl)

typedef struct {
  u8  pokeCnt;
  u8  pokeID[ BTL_POS_NUM ];
}HANDWORK_POKEID;

/**
 *  共有ツール関数群
 */
extern bool HandCommon_CheckTargetPokeID( u8 pokeID );
extern bool HandCommon_CheckTargetSide( ServerFlow* flowWk, BtlSide side );
extern bool HandCommon_IsPokeOrderLast( ServerFlow* flowWk, u8 pokeID );
extern bool HandCommon_IsNekodamashiFail( ServerFlow* flowWk, u8 pokeID );
extern bool HandCommon_IsUnbreakablePokeItem( ServerFlow* flowWk, u16 monsno, u16 itemID );
extern bool HandCommon_CheckCantChangeItemPoke( ServerFlow* wk, u8 pokeID );
extern bool HandCommon_CheckCantStealWildPoke( ServerFlow* flowWk, u8 attackPokeID );
extern bool HandCommon_CheckCantStealPoke( ServerFlow* flowWk, u8 attackPokeID, u8 targetPokeID );
extern void HandCommon_MagicCoat_CheckSideEffWaza( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
extern void HandCommon_MagicCoat_Wait( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
extern void HandCommon_MagicCoat_Reaction( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
extern void HandCommon_MulWazaBasePower( u32 ratio );
extern void HandCommon_NigeruCalcSkip( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
extern bool HandCommon_CheckNigeruExMsg( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID );
extern u32  HandCommon_GetKillCount( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );
extern bool HandCommon_Boujin_WeatherReaction( u8 pokeID );
extern bool HandCommon_Boujin_WazaNoEffect( u8 pokeID );
extern pml::PokeType HandCommon_GetMezapaType( ServerFlow* flowWk, u8 pokeID );
extern bool HandCommon_GuardWazaSick( ServerFlow* flowWk, u8 pokeID, WazaSick guardSick );
extern bool HandCommon_Katayaburi_SkipCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, BtlEventFactorType factorType, BtlEventType eventType, u16 subID, u8 pokeID );

//=============================================================================================
/**
 * 「どろぼう」等 ダメージプロセス開始処理
 *
 * @param        flowWk  ServerFlow ポインタ
 * @param        pokeID  ワザ使用するポケモンID
 * @param[out]   work    ワザ使用するポケモンが道具を保持していたら work[0] に true, いなければ false を入れる
 */
//=============================================================================================
extern void HandCommon_DorobouStart( ServerFlow* flowWk, u8 pokeID, int* work );

//=============================================================================================
/**
 * 「どろぼう」の追加効果を発動できるか判定
 *
 * @param        flowWk  ServerFlow ポインタ
 * @param        pokeID  ワザ使用するポケモンID
 * @param[in]    work    HandCommon_DorobouStartの結果が格納されている work
 *
 * @retval    bool     「どろぼう」の追加効果が発動できる場合 true
 */
//=============================================================================================
extern bool HandCommon_IsDorobouEnable( ServerFlow* flowWk, u8 pokeID, int* work );

//=============================================================================================
/**
 * 指定ポケモンIDを素早さ順にソートする
 *
 * @param       flowWk
 * @param[io]   targetPokeIDAry
 * @param       targetCnt
 * @param       myPokeID
 * @param       bDownFriendPriority   味方のプライオリティを下げる（敵より確実に下になる）
 */
//=============================================================================================
extern void  HandCommon_SortTargetByAgility( ServerFlow* flowWk, u8* targetPokeIDAry, uint32_t targetCnt, u8 myPokeID, bool bDownFriendPriority );

//=============================================================================================
/**
 * ポケモンID をキーにして状態異常を取得する
 *
 * @param     flowWk
 * @param     pokeID
 *
 * @retval    pml::pokepara::Sick     状態異常コード
 */
//=============================================================================================
extern pml::pokepara::Sick  HandCommon_CheckPokeSick( ServerFlow* flowWk, u8 pokeID );

//=============================================================================================
/**
 * ワザがプラスの効果しかない「シンプルエフェクト」タイプかどうかを判定
 * @param waza
 * @return  プラス効果しかない「シンプルエフェクト」なら true 
 */
//=============================================================================================
extern bool HandCommon_IsSimpleEffectOnlyAdvantage( WazaID waza );

//=============================================================================================
/**
 * @brief 技「このゆびとまれ」による、技ターゲットの誘導
 *
 * @param myHandle
 * @param flowWk
 * @param konoyubiPokeId  「このゆびとまれ」を使用しているポケモンのID
 * @param temptPriority   ターゲット誘導の優先度
 * @param temptCause      ターゲット誘導の要因
 * @param factorWork      イベントファクターの作業領域
 */
//=============================================================================================
void HandCommon_KonoyubiTomare_TemptTarget( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 konoyubiPokeId, TemptTargetPriority temptPriority, TemptTargetCause temptCause, int* factorWork );

//=============================================================================================
/**
 * @brief 技ターゲットの誘導
 *
 * @param temptPokeId     誘導先ポケモンのＩＤ
 * @param temptPriority   ターゲット誘導の優先度
 * @param temptCause      ターゲット誘導の要因
 */
//=============================================================================================
void HandCommon_TemptTarget( u8 temptPokeId, TemptTargetPriority temptPriority, TemptTargetCause temptCause );


//=============================================================================================
/**
 * @brief 乱入時に味方が残ってるか判定
 *
 * @param flowWk    サーバーフローハンドラ
 * @param pokeID    対象ポケモンID（野生側）
 * @retval  bool    乱入線であり、pokeID の味方が場に存在している場合に true 
 */
//=============================================================================================
bool HandCommon_IsExistIntrudeFriend( ServerFlow* flowWk, u8 pokeID );

//=============================================================================================
/**
 * @brief  自分が対処すべきイベントかどうか判定（ポケモンID、ワザIDの両チェック）
 *
 * @param[in]  myHandle     イベントハンドラ
 * @param[in]  pokeID_label ポケモンIDを取得するためのラベルナンバー
 * @param[in]  pokeID       自分のポケモンID
 * @return     自分が対処すべきイベントなら true 
 *
 * @detail  従来ならポケモンIDの一致のみを確認すればよかったケースでも、
 *          今作（niji）より、（おどりこ・さいはい効果）１ターン中に複数のワザを出すことが起こりうるようになったため、
 *          張り付いているハンドラが反応すべき処理か否かを判定するために、ワザIDの一致も確認する必要が生じた。
 *          そのようなケースで使うことを想定している。
 */
//=============================================================================================
bool HandCommon_IsMyEvent( BTL_EVENT_FACTOR* myHandle, BtlEvVarLabel pokeID_label, u8 pokeID );

GFL_NAMESPACE_END(btl)

#endif

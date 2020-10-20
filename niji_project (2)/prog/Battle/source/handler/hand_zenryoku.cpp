//-----------------------------------------------------------------------------
/**
 * @file    hand_zenryoku.cpp
 * @brief   ポケモンXY バトルシステム イベントファクター [全力技]
 * @author  obata_toshihiro
 *
 * @date  2015.06.04  作成
 */
//-----------------------------------------------------------------------------
#include "../btl_str.h"
#include "../btl_ServerFlow.h"
#include "./hand_common.h"
#include "./hand_zenryoku.h"


namespace btl {
namespace handler {
namespace zenryoku {


const BtlEventHandlerTable* ADD_KonoyubiTomare( u32* numElems );
void handler_KonoyubiTomare_Target( BTL_EVENT_FACTOR* myHandle, ServerFlow* serverFlow, u8 zenryokuUsePokeId, int* factorWork );
void handler_KonoyubiTomare_TurnCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* serverFlow, u8 zenryokuUsePokeId, int* factorWork );




/**
 * @brief イベントハンドラテーブルを取得する関数の型
 * @param[out]  ハンドラテーブルの要素数
 * @return ハンドラテーブル
 */
typedef const BtlEventHandlerTable* (*pEventAddFunc)( u32* numHandlers );


static const struct 
{
  ZenryokuWazaEffect zenryokuEffect; // 全力技効果
  pEventAddFunc      addFunc;        // 全力技効果に対するハンドラテーブルを取得する関数
}
FACTOR_TABLE[] = 
{
  { pml::wazadata::ZENRYOKUWAZA_EFFECT_KONOYUBITOMARE, ADD_KonoyubiTomare },
};



//-----------------------------------------------------------------------------
/**
 * @brief 全力技効果に対するイベントファクターをシステムに追加する
 *
 * @param zenryokuUsePoke  全力技を発動するポケモン
 * @param zenryokuEffect   全力技効果
 *
 * @retval true   ハンドラを追加した
 * @retval false  ハンドラを追加できなかった
 */
//-----------------------------------------------------------------------------
bool Add( const BTL_POKEPARAM* zenryokuUsePoke, ZenryokuWazaEffect zenryokuEffect )
{
  if( pml::wazadata::ZENRYOKUWAZA_EFFECT_NUM <= zenryokuEffect ) {
    GFL_ASSERT(0);
    return false;
  }

  for( u32 i=0; i<GFL_NELEMS(FACTOR_TABLE); ++i )
  {
    if( FACTOR_TABLE[i].zenryokuEffect != zenryokuEffect ) {
      continue;
    }

    u32 numHandlers;
    const BtlEventHandlerTable* handlerTable = FACTOR_TABLE[i].addFunc( &numHandlers );
    u32 subPriority = zenryokuUsePoke->GetValue_Base( BTL_POKEPARAM::BPP_AGILITY );
    u8 dependId = zenryokuUsePoke->GetID();
    BTL_EVENT_FACTOR* factor = EVENT_AddFactor( BTL_EVENT_FACTOR_ZENRYOKUWAZA, zenryokuEffect, BTL_EVPRI_ZENRYOKUWAZA_DEFAULT, subPriority, dependId, handlerTable, numHandlers );
    return ( factor != NULL );
  }

  return false;
}

/**
 * @brief 指定したイベントファクタを削除する
 * @param zenryokuUsePokeId  全力技を発動したポケモンのID
 * @param zenryokuEffect     全力技効果
 */
void removeFactorForce( u8 zenryokuUsePokeId, ZenryokuWazaEffect zenryokuEffect )
{
  BTL_EVENT_FACTOR* factor = EVENT_SeekFactor( BTL_EVENT_FACTOR_ZENRYOKUWAZA, zenryokuUsePokeId );
  while( factor )
  {
    if( EVENT_FACTOR_GetSubID(factor) == zenryokuEffect ) 
    {
      EVENT_FACTOR_Remove( factor );
      factor = EVENT_SeekFactor( BTL_EVENT_FACTOR_ZENRYOKUWAZA, zenryokuUsePokeId );
    }
    else
    {
      factor = EVENT_GetNextFactor( factor );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * 全力技効果「このゆびとまれ」
 *
 * 全力技技を使用したターンの間、技「このゆびとまれ」の効果
 */
//-----------------------------------------------------------------------------
const BtlEventHandlerTable*  ADD_KonoyubiTomare( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TEMPT_TARGET,       handler_KonoyubiTomare_Target    },  // ターゲット決定
    { BTL_EVENT_TURNCHECK_BEGIN,    handler_KonoyubiTomare_TurnCheck },  // ターンチェック
  };
  *numElems = GFL_NELEMS( HandlerTable );
  return HandlerTable;
}

/**
 * @brief ターゲット決定イベント
 *        相手の技ターゲットを自分に向ける
 *
 * @param myHandle
 * @param serverFlow
 * @param zenryokuUsePokeId  全力技を発動したポケモンのID
 * @param factorWork     自身のファクターで使用できる作業領域
 */
void handler_KonoyubiTomare_Target( BTL_EVENT_FACTOR* myHandle, ServerFlow* serverFlow, u8 zenryokuUsePokeId, int* factorWork )
{
  HandCommon_KonoyubiTomare_TemptTarget( myHandle, serverFlow, zenryokuUsePokeId, TEMPT_TARGET_PRIORITY_AURA, TEMPT_TARGET_CAUSE_OTHER, factorWork );
}

/**
 * @brief ターンチェック
 *        ターン終了時に、全力技効果を終了する
 *
 * @param myHandle
 * @param serverFlow
 * @param zenryokuUsePokeId  全力技を発動したポケモンのID
 * @param factorWork     自身のファクターで使用できる作業領域
 */
void handler_KonoyubiTomare_TurnCheck( BTL_EVENT_FACTOR* myHandle, ServerFlow* serverFlow, u8 zenryokuUsePokeId, int* factorWork )
{
  GFL_UNUSED( serverFlow );
  GFL_UNUSED( factorWork );
  ZenryokuWazaEffect zenryokuEffect = static_cast<ZenryokuWazaEffect>( EVENT_FACTOR_GetSubID( myHandle ) );
  removeFactorForce( zenryokuUsePokeId, zenryokuEffect );
}


} // namespace zenryoku
} // namespace handler
} // namespace btl


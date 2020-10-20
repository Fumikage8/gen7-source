#if !defined( __BTL_EVENTFACTOR_H__ )
#define __BTL_EVENTFACTOR_H__
//=============================================================================================
/**
 * @file    btl_EventFactor.h
 * @brief   ポケモンXY バトルシステム イベント＆ハンドラ処理（FACTOR側にのみ公開）
 * @author  taya
 *
 * @date  2011.01.06  作成
 */
//=============================================================================================
#pragma once

#include "btl_Event.h"

GFL_NAMESPACE_BEGIN(btl)

class ServerFlow;

enum {
  EVENT_HANDLER_WORK_ELEMS = 7,       ///< ハンドラごとに割り当てられるローカルワーク要素数(int)
  EVENT_WAZA_STICK_MAX = 8,           ///< １体につき貼り付くことが出来るワザ数の最大値
};

//--------------------------------------------------------------
/**
* イベント実体型
*/
//--------------------------------------------------------------
typedef struct _EVENT_FACTOR  BTL_EVENT_FACTOR;

//--------------------------------------------------------------
/**
* イベントハンドラ型
*
* myHandle  自身削除用のハンドラ
* flowWk    サーバフローワーク
* pokeID    主体となるポケモンのID
* work      ワーク用配列（要素数はEVENT_HANDLER_WORK_ELEMS, 初回呼び出し時ゼロクリアが保証される）
*/
//--------------------------------------------------------------
typedef void (*BtlEventHandler)( BTL_EVENT_FACTOR* myHandle, ServerFlow* flowWk, u8 pokeID, int* work );


//--------------------------------------------------------------
/**
* イベントスキップチェックハンドラ型
* 特定のハンドラ呼び出しをスキップするかどうか判定に反応する場合、このハンドラを追加でアタッチする
* （※現状、とくせい「かたやぶり」が特定のとくせいを無効化する場合に使用することを想定）
*
* myHandle      自分自身のハンドラ
* factorType    反応ファクタータイプ（ワザ、とくせい、アイテム等）
* eventType     イベントID
* subID         ファクタータイプごとのサブID（ワザなら「なみのり」，とくせいなら「いかく」などのID)
* pokeID        主体となるポケモンのID
*
* return  スキップする場合TRUE
*/
//--------------------------------------------------------------
typedef bool (*BtlEventSkipCheckHandler)( BTL_EVENT_FACTOR* myHandle, ServerFlow* wk, BtlEventFactorType factorType, BtlEventType eventType, u16 subID, u8 pokeID );

//--------------------------------------------------------------
/**
* イベントハンドラ＆タイプの関連付けテーブル
*/
//--------------------------------------------------------------
typedef struct {
  BtlEventType    eventType;
  BtlEventHandler   handler;
}BtlEventHandlerTable;



//=============================================================================================
/**
 * イベント反応要素を追加
 *
 * @param   factorType      ファクタータイプ
 * @param   subID           タイプごとの個別ID（とくせいID，アイテムID等と一致）
 * @param   priority        プライオリティ ( = 数値が高いほど先に呼び出される )
 * @param   pokeID
 * @param   handlerTable
 *
 * @retval  BTL_EVENT_FACTOR*
 */
//=============================================================================================
BTL_EVENT_FACTOR* EVENT_AddFactor( BtlEventFactorType factorType, u16 subID,
  BtlEventPriority mainPri, u32 subPri, u8 dependID,
  const BtlEventHandlerTable* handlerTable, u16 numHandlers );



//=============================================================================================
/**
 * タイプ＆依存IDをキーにしてイベント要素を探す
 *
 * @param   factorType
 * @param   dependID    依存ID（通常はポケモンID／位置イベントなどは位置ID）
 *
 * @retval  extern BTL_EVENT_FACTOR*
 */
//=============================================================================================
extern BTL_EVENT_FACTOR* EVENT_SeekFactor( BtlEventFactorType factorType, u8 dependID );

extern BTL_EVENT_FACTOR* EVENT_GetNextFactor( BTL_EVENT_FACTOR* factor );

extern void         EVENT_FACTOR_Remove( BTL_EVENT_FACTOR* factor );
extern u16          EVENT_FACTOR_GetSubID( const BTL_EVENT_FACTOR* factor );
extern int          EVENT_FACTOR_GetWorkValue( const BTL_EVENT_FACTOR* factor, u8 workIdx );
extern void         EVENT_FACTOR_SetWorkValue( BTL_EVENT_FACTOR* factor, u8 workIdx, int value );
extern void         EVENT_FACTOR_SetTmpItemFlag( BTL_EVENT_FACTOR* factor );
extern void         EVENT_FACTOR_SetRecallEnable( BTL_EVENT_FACTOR* factor );
extern u8           EVENT_FACTOR_GetPokeID( const BTL_EVENT_FACTOR* factor );
extern void         EVENT_FACTOR_AttachSkipCheckHandler( BTL_EVENT_FACTOR* factor, BtlEventSkipCheckHandler handler );
extern void         EVENT_FACTOR_DettachSkipCheckHandler( BTL_EVENT_FACTOR* factor );
extern void         EVENT_FACTOR_ConvertForIsolate( BTL_EVENT_FACTOR* factor );
extern bool         EVENT_FACTOR_IsIsolateMode( BTL_EVENT_FACTOR* factor );
extern bool         EVENT_FACTOR_Sleep( BTL_EVENT_FACTOR* factor );
extern bool         EVENT_FACTOR_Weak( BTL_EVENT_FACTOR* factor );

//=============================================================================================
/**
 * 自己が抱える Work の内容を初期状態にクリアする
 *
 * @param   factor
 */
//=============================================================================================
extern void EVENT_FACTOR_ClearWork( BTL_EVENT_FACTOR* factor );


GFL_NAMESPACE_END(btl)
#endif // __BTL_EVENTFACTOR_H__

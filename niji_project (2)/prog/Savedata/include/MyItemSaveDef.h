//============================================================================
/**
 *
 *	@file		sv_myitem_def.h
 *	@brief  手持ちアイテム スクリプトからも参照する定義
 *	@author		hosaka genya
 *	@data		2009.10.09
 *	@note   myitem_savededa.h から切り出し
 *	@note   アセンブラで読み込むためenum禁止
 *
 */
//============================================================================
#if !defined( __SV_MYITEM_DEF_H__ )
#define __SV_MYITEM_DEF_H__

#include "niji_conv_header/poke_lib/item/itempocket_def.h"

//=============================================================================
/**
 *								定数定義
 */
//=============================================================================
//--------------------------------------------------------------
//	この定義はitemconv_dp の datamake.cppと同じでなければいけない 2009.07.21
//	コンバータから吐き出された定数を受け渡すようにした 2009.10.10 by genya hosaka
//--------------------------------------------------------------
// ポケットID
#define BAG_POCKET_DRUG     ( ITEMPOCKET_DRUG )     // 体力回復/状態回復/PP回復
#define BAG_POCKET_NORMAL   ( ITEMPOCKET_NORMAL )   // ノーマル/戦闘/ボール
#define BAG_POCKET_WAZA     ( ITEMPOCKET_WAZA )     // 技マシン
#define BAG_POCKET_NUTS     ( ITEMPOCKET_NUTS )     // 木の実
#define BAG_POCKET_EVENT    ( ITEMPOCKET_EVENT )    // 大切な道具
#define BAG_POCKET_BEADS    ( ITEMPOCKET_BEADS )    // ビーズ
#define BAG_POCKET_ROTOPON  ( ITEMPOCKET_ROTOPON )  // ロトポン
#define BAG_POCKET_NONE     ( ITEMPOCKET_NONE )     // 保存先なし（バッグに入らない）

#define BAG_POCKET_FREE   ( 8 )                   // フリースペース

#define FIELD_BAG_POCKET_MAX    ( 7 )   // フィールドポケット最大数

#define BATTLE_BAG_POCKET_MAX   ( 5 )   // バトルバッグのポケット最大数

#define ITEM_HAVE_MAX ( 999 )  // 各アイテムの最大保持数

#endif // __SV_MYITEM_DEF_H__

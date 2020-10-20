#ifndef __BOX_APP_PARAM_H__
#define __BOX_APP_PARAM_H__
//=============================================================================
/**
 * @file	 BoxAppParam.h
 * @brief	 ボックス用アプリパラメータ
 * @author k.ohno
 * @date	 2015.8.8
 */
//=============================================================================
#pragma once

#include "PokeTool/include/PokeTool.h"
#include "SaveData/include/BoxSave.h"
#include "SaveData/include/BoxPokemonSave.h"



GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( fs )
class AsyncFileManager;
GFL_NAMESPACE_END( fs )
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class AppRenderingManager;
class Heap;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( ui )
class UIView;
GFL_NAMESPACE_END( ui )
GFL_NAMESPACE_END( app )


GFL_NAMESPACE_BEGIN( applib )
GFL_NAMESPACE_BEGIN( frame )
class Manager;
GFL_NAMESPACE_END( frame )
GFL_NAMESPACE_END( applib )


GFL_NAMESPACE_BEGIN(Savedata)
class BOX;
class BoxPokemon;
class ResortSave;
class MyItem;
GFL_NAMESPACE_END(Savedata)


GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN(tool)
class PokeIcon;
class ItemIcon;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(P2PTrade)
class P2PTradeRequestClient;
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetAppLib)



GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )
class BoxUppChatDraw;
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

enum BOX_CALL_TYPE{
  BOX_CALL_PC,        //パソコン ※デフォルト
  BOX_CALL_TRADE,    //P2P交換
  BOX_CALL_GTS,      //GTS
  BOX_CALL_MIRACLE,   //ミラクル交換
  BOX_CALL_BATTLE_TEAM,   //バトルチーム選択
  BOX_CALL_RESORT_GIM,   //GIM
  BOX_CALL_FIELD_TRADE, //フィールドの交換
  BOX_CALL_RESORT_HOTSPA,   //おんせん
  BOX_CALL_MAX
};


enum BOX_CHANGE_EDITTYPE{
  BOX_CHANGE_EDITTYPE_POKEMON,
  BOX_CHANGE_EDITTYPE_ITEM,
  BOX_CHANGE_EDITTYPE_BATTLE_TEAM,
  BOX_CHANGE_EDITTYPE_MAX,
};


enum BOX_CHANGE_SELECTMODE{
  BOX_CHANGE_SELECTMODE_SINGLE,
  BOX_CHANGE_SELECTMODE_SHORTCUT,
  BOX_CHANGE_SELECTMODE_GROUP,
  BOX_CHANGE_SELECTMODE_MAX,
};


static const u16 EMPTY_BOXNO = 0xffff;

//シーケンスにかかわる値
static const u8 BOXJUMP_STATUS = 0xfc;  // ステータス画面
static const u8 BOXJUMP_FIND   = 0xfd;  // 検索画面に移動する場合
static const u8 BOXJUMP_CANCEL = 0xfe;  // キャンセルした場合
static const u8 BOXJUMP_OK     = 0xfb;  //OK 決定
static const u8 BOXJUMP_ITEM = 0xfa;  // アイテム画面
static const u8 BOXJUMP_NAME = 0xf9;  // 名前入力画面
static const u8 BOXJUMP_TEAM_NAME = 0xf8;  // チーム名前入力画面

static const u8 RETURN_TEMOTI = 0xff;  // 手持ちポケモンを選択した場合



struct TradeData {
  u16 MonsNo;  //欲しいモンスター番号 MONSNO_NULLが入っていると ポケモン番号を見ない
  u8 Gender;   //欲しい性別        pml::SEX_NUMが入っていると性別は確認しない
  u8 Level;  //欲しいレベル帯      ０が入っているとレベルは確認しない 1~10 11~20...以下略
  u8 bOn;    //何かを指定した場合1
  u8 dummy1;
  u8 dummy2;
  u8 dummy3;
};


struct RangeData{  ///<範囲移動用データ
  u16 num;
  u16 page;
  u16 cursor;
  u16 x;
  u16 y;
  u16 width;
  u16 height;
  u16 skip;  //一個飛ばしの場合TRUE
};


struct PokemonIconDraw{
  PokeTool::SimpleParam simp;  ///< simple param
  u8 bTeam[6];     /// チームマーク表示（最大６チーム登録）
  u8 bblack;     // くろいろつけてるとき
  u8 bItem;    //アイテムを持っている
  u8 bLock;      // ロックマーク表示
  u8 bPassive;   //半透明のとき１
  u8 bHide;   //消している時1 ついてるとき０
  u8 nowRedraw;   //描画しないとき０ ICONSTATECHANGE=1  ICONTEXCHANGE=2
  u8 reserveRedraw;  //上のステートの予約
  u8 bwhite;  //白くする時
  u8 carryup; //持ち上げる
  u8 dummy2;
};


/// 外部設定パラメータ（初期化データ・終了時のデータなど）
struct APP_PARAM {
	// [ in ]
  Savedata::BOX* pBoxData;            // ボックス表示を行う為のInterfaceクラス
  Savedata::BoxPokemon* pBoxPokemon;  //ポケモンデータの実際の入れ替えを行う為のIF
  Savedata::ResortSave* pResortSave;  //リゾートのセーブデータ
  Savedata::ZukanData* pZukanData;    // 図鑑のセーブデータ
  Savedata::MyItem* pMyItem;      // アイテムのセーブデータ
//  pml::PokeParty* pTemochi;           // てもちぽけもんのデータ
  pml::PokeParty* pTemochi2;           // てもちぽけもんのデータ
  NetAppLib::P2PTrade::P2PTradeRequestClient* pP2PTradeRequestClient;
  gfl2::fs::AsyncFileManager* mpAsyncFileManager;   //ファイルマネージャー
  app::util::AppRenderingManager* mpAppRenderingManager;   //レンダリングマネージャー
  app::util::Heap* mpAppHeap;            //appheap
  app::util::Heap* mpAppHeapUpper;       //appheap
  applib::frame::Manager* mpAppFrameManager;
  BOX_CALL_TYPE type;  //呼び出し場所
  TradeData trade;  //欲しいポケモン指定
  bool bBoxJudge;  //ジャッジ画面開放かどうか
  // [temp]  アプリを切り替えられても困らないように
  bool bAllBoxMode;  //全体ボックス画面だったかどうかを記憶 検索から戻ってくる時
  pml::pokepara::PokemonParam* pSelectPokemonParam;  //選択したポケモン
  pml::pokepara::PokemonParam* pWantPokemonParam;  // 相手が欲しいといったポケモン
  pml::pokepara::PokemonParam* pTargetPokemonParam;  //通信の相手が選択したポケモン
  pml::pokepara::PokemonParam* pMyWantPokemonParam;  // 自分が欲しいといったポケモン
  
  gfl2::str::MsgData* pMsgData;       ///< メッセージデータ BOXの 途中で読み込まれる
  app::tool::ItemIcon * pItemIcon;
  app::tool::PokeIcon * pPokeIcon;
  App::Box::BoxUppChatDraw* pBoxUppChatDraw;
  u16 chatPokemonOther;  //相手のポケモン
  u16 chatPokemonSelf;  //自分のポケモン
  u16 nowDispTray;   ///< 表示トレイ
  u8 nowBattleTray;  ///バトルトレイ
  u8 chatMsgSelf;
  u8 chatMsgOther;  //相手のチャット
  u8 bPokeChange;  //アイテム画面でアイテムをつけたとき
  u8 mbNetEndRequest;  //終了リクエストを記憶
  u8 mbNetEndSuccess;  //終了完了を記憶
  u8 folderCursor;  //フォルダーのカーソル位置
  // momiji追加
  b8 isNijiCompatibility;    // niji通信互換対応用フラグ：momijiで追加された新要素を弾くならtrue;

#if PM_DEBUG  
  //  デバッグ用にカーソル位置が必要
  u8  cursorPos;
  u8 dmy1;
#endif

  // [ out ]
  u8 outJumpMode;      /// 移動する場合 ※特殊な戻り値参照
	u8 outTray;			            ///< 選んだトレイ番号
	u8 outPos;			            ///< 選んだ場所番号
  u8 outTeam;   ///選んだチーム
};


GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )

#endif //__BOX_APP_PARAM_H__

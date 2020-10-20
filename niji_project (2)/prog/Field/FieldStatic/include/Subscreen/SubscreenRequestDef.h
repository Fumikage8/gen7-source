//=============================================================================
/**
 * @file    SubscreenRequestDef.h
 * @brief   フィールドサブスクリーンのリクエスト定義
 * @author  Hiroyuki Nakamura
 * @data    2015.09.01
 */
//=============================================================================
#if !defined( SUBSCREEN_REQUEST_DEF_H_INCLUDED )
#define SUBSCREEN_REQUEST_DEF_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>


GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(SubScreen)

//! フィールドからメニューへのリクエストID
enum FieldMenuInRequestID
{
  IN_REQ_ID_NONE = 0,         //!< リクエストなし

  IN_REQ_ID_MENU_END,         //!< モジュール終了

  IN_REQ_ID_MENU_OPEN,        //!< メニューを開く

  IN_REQ_ID_INPUT_DESABLE,    //!< 入力を無効にする
  IN_REQ_ID_INPUT_ENABLE,     //!< 入力を有効にする

  IN_REQ_ID_RIDE_OPEN,        //!< ライドポケモンのリストを開く

  IN_REQ_ID_REPORT_OPEN,      //!< レポート画面を開く

  IN_REQ_ID_QUICKMATCH_OPEN,  //!< クイックマッチ下画面を開く

  IN_REQ_ID_INPUT_ENABLE_MENU_OPEN,   //!< 入力を有効にしてメニューを開く @note 看板イベント中にメニューを開くために追加

  IN_REQ_ID_CALL_APP,         //!< アプリが呼ばれた

  IN_REQ_ID_START_ROTOPON,              //!< ロトポン開始
  IN_REQ_ID_ROTOPON_ITEM_EVENT_RETURN,  //!< ロトポン道具入手スクリプト終了
};

//! メニューからフィールドへのリクエストID
enum FieldMenuOutRequestID
{
  OUT_REQ_ID_NONE = 0,          //!< リクエストなし

  OUT_REQ_ID_MENU_OPEN,         //!< メニューを開く
  OUT_REQ_ID_MENU_CLOSE,        //!< メニューを閉じる

  OUT_REQ_ID_POKELIST,          //!< ポケモンリスト
  OUT_REQ_ID_ZUKAN,             //!< 図鑑
  OUT_REQ_ID_BAG,               //!< バッグ
  OUT_REQ_ID_ALBUM,             //!< マイアルバム
  OUT_REQ_ID_NET_PLAY,          //!< 通信で遊ぶ
  OUT_REQ_ID_JOIN_FESTA,        //!< ジョインフェスタ

  OUT_REQ_ID_TR_CARD,           //!< トレーナーカード
  OUT_REQ_ID_POKE_PLANET,       //!< ポケプラネット
  OUT_REQ_ID_POKE_PARURE,       //!< ポケパルレ
  OUT_REQ_ID_QR_READ,           //!< QR読み込み
  OUT_REQ_ID_BATTLE_VIDEO,      //!< バトルビデオ
  OUT_REQ_ID_CONFIG,            //!< 設定

  OUT_REQ_ID_REPORT,            //!< レポート

  OUT_REQ_ID_POKEFINDER,        //!< ポケファインダー

  OUT_REQ_ID_RIDE_OPEN,         //!< ライドポケモンのリストを開く
  OUT_REQ_ID_RIDE_CLOSE,        //!< ライドポケモンのリストを閉じる（キャンセル）
  OUT_REQ_ID_RIDE_DECIDE,       //!< ライドポケモンのリストで選択された

  OUT_REQ_ID_RIDE_POKEMON,      //!< ライドポケモン

  OUT_REQ_ID_REPORT_CLOSE,      //!< レポート画面を閉じる
  OUT_REQ_ID_REPORT_KEEP_MENU,  //!< レポート画面終了後、メニューを開いた状態にする

  OUT_REQ_ID_EVENTLIST,         //!< イベントリストを選択
  OUT_REQ_ID_MAIN_EVENT,        //!< メインイベントアイコンを選択
  OUT_REQ_ID_SUB_EVENT,         //!< サブイベントアイコンを選択

  OUT_REQ_ID_QUICKMATCH_CLOSE,      //!< クイックマッチ下画面を閉じる
  OUT_REQ_ID_QUICKMATCH_KEEP_MENU,  //!< クイックマッチ下画面終了後、メニューを開いた状態にする
  OUT_REQ_ID_QUICKMATCH_CALL,       //!< クイックマッチアプリを呼び出す

  OUT_REQ_ID_TOWNMAP,           //!< タウンマップを選択

  OUT_REQ_ID_RIDE_INFO,         //!< ライドの説明画面を呼び出す

  OUT_REQ_ID_START_ROTOPON,       //!< ロトポン開始
  OUT_REQ_ID_END_ROTOPON,         //!< ロトポン終了
  OUT_REQ_ID_ROTOPON_ITEM_EVENT,  //!< ロトポンの道具入手スクリプト呼び出し

  OUT_REQ_ID_FINDER_STUDIO,     //!< ファインダースタジオ呼び出し

  //==================================
  // ジョインフェスタ追加分
  OUT_REQ_ID_JF_END,                            //!< ジョインフェスタからフィールドへ戻る
  OUT_REQ_ID_MIRACLE,                           //!< ミラクル交換
  OUT_REQ_ID_GTS,                               //!< GTS
  OUT_REQ_ID_CHANGE_WIFI,                       //!< ジョインフェスタで通信をWIFIに切り替える
  OUT_REQ_ID_CHANGE_LOCAL,                      //!< ジョインフェスタで通信をローカルに切り替える
  OUT_REQ_ID_PLAYER_LIST_SHOW,                  //!< プレイヤーリスト（管理閲覧モード３）
  OUT_REQ_ID_PROFILE,                           //!< プロフィールアプリ
  OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_S,       //!< プレイヤーリスト（シングル対戦選択モード１）
  OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_D,       //!< プレイヤーリスト（ダブル対戦選択モード１）
  OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_M,       //!< プレイヤーリスト（マルチ対戦選択モード２）
  OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_R,       //!< プレイヤーリスト（ロイヤル対戦選択モード２）
  OUT_REQ_ID_PLAYER_LIST_SELECT_TRADE,          //!< プレイヤーリスト（交換選択モード１）
  OUT_REQ_ID_BEACON_MATCHING,                   //!< ビーコンマッチングアプリ(お誘いがきて承諾した場合直接このアプリに行く)
  OUT_REQ_ID_OPEN_LIST,                         //!< 募集一覧のリスト（ストリーミングタッチで行く）
  OUT_REQ_ID_ATTRACTION_RETIRE,                 //!< ローカルアトラクションのリタイア選択
  OUT_REQ_ID_BATTLE_SPOT,                       //!< バトルスポット
  OUT_REQ_ID_REGULATION_DOWNLOAD,               //!< レギュレーションダウンロード
  OUT_REQ_ID_GAME_SYNC,                         //!< ゲームシンク
  OUT_REQ_ID_WIFI_BTLVIDEO,                     //!< インターネット版バトルビデオ
  OUT_REQ_ID_GLOBAL_ATTRACTION,                 //!< グローバルアトラクションアプリ
};


GFL_NAMESPACE_END(SubScreen)
GFL_NAMESPACE_END(Field)


#endif  // SUBSCREEN_REQUEST_DEF_H_INCLUDED

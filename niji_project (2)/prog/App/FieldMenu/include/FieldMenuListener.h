//=============================================================================
/**
 * @file    FieldMenuListener.h
 * @brief   フィールドメニューリスナー
 * @author  Hiroyuki Nakamura
 * @date    2015.05.21
 */
//=============================================================================
#if !defined( FIELD_MENU_LISTENER_H_INCLUDED )
#define FIELD_MENU_LISTENER_H_INCLUDED

#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)

//=============================================================================
/**
 * @class FrameListener
 * @brief フィールドメニューリスナークラス
 * @date  2015.03.04
 */
//=============================================================================
class FrameListener
{
public:
  //! 動作ID
  enum ActionID
  {
    ACTION_CALL_RIDE = 0,   //!< ライドポケモン
    ACTION_CALL_FINDER,     //!< ポケファインダー

    ACTION_CALL_MENU,       //!< メニュー開閉（Xボタン）
    ACTION_CLOSE_MENU,      //!< メニューを閉じる

    ACTION_POKELIST,        //!< ポケモンリスト
    ACTION_ZUKAN,           //!< 図鑑
    ACTION_BAG,             //!< バッグ
    ACTION_ALBUM,           //!< マイアルバム
    ACTION_NET_PLAY,        //!< 通信で遊ぶ
    ACTION_JOIN_FESTA,      //!< ジョインフェスタ

    ACTION_TR_CARD,         //!< トレーナーカード
    ACTION_POKE_PLANET,     //!< ポケプラネット
    ACTION_POKE_PARURE,     //!< ポケパルレ
    ACTION_QR_READ,         //!< QR読み込み
    ACTION_BATTLE_VIDEO,    //!< バトルビデオ
    ACTION_CONFIG,          //!< 設定

    ACTION_REPORT,          //!< レポート

    ACTION_CHANGE_PAGE_LEFT,  //!< ページ切り替え：左
    ACTION_CHANGE_PAGE_RIGHT, //!< ページ切り替え：右

    ACTION_PAGE_00,           //!< ページ切り替えボタン０
    ACTION_PAGE_01,           //!< ページ切り替えボタン１

    ACTION_MENU_POS_UP,       //!< カーソル移動：上
    ACTION_MENU_POS_DOWN,     //!< カーソル移動：下
    ACTION_MENU_POS_LEFT,     //!< カーソル移動：左
    ACTION_MENU_POS_RIGHT,    //!< カーソル移動：右

    ACTION_CATCH_POKELIST,      //!< キャッチ動作：ポケモンリスト
    ACTION_CATCH_BAG,           //!< キャッチ動作：バッグ
    ACTION_CATCH_ZUKAN,         //!< キャッチ動作：図鑑
    ACTION_CATCH_TR_CARD,       //!< キャッチ動作：トレーナーカード
    ACTION_CATCH_CONFIG,        //!< キャッチ動作：設定
    ACTION_CATCH_ALBUM,         //!< キャッチ動作：マイアルバム

    ACTION_CATCH_POKE_PLANET,   //!< キャッチ動作：ポケプラネット
    ACTION_CATCH_POKE_PARURE,   //!< キャッチ動作：ポケパルレ
    ACTION_CATCH_NET_PLAY,      //!< キャッチ動作：通信で遊ぶ
    ACTION_CATCH_JOIN_FESTA,    //!< キャッチ動作：ジョインフェスタ
    ACTION_CATCH_BATTLE_VIDEO,  //!< キャッチ動作：バトルビデオ
    ACTION_CATCH_QR_READ,       //!< キャッチ動作：QR読み込み

    ACTION_MENU_PAGE_DRAG_START,  //!< スライド操作：ドラッグ開始
    ACTION_MENU_PAGE_DRAG,        //!< スライド操作：ドラッグ中
    ACTION_MENU_PAGE_RELEASE,     //!< スライド操作：リリース
    ACTION_MENU_PAGE_SCROLL,      //!< スライド操作：スクロール

    ACTION_RIDE_SC_UP,      //!< ライドポケモンショートカット：上
    ACTION_RIDE_SC_DOWN,    //!< ライドポケモンショートカット：下
    ACTION_RIDE_SC_LEFT,    //!< ライドポケモンショートカット：左
    ACTION_RIDE_SC_RIGHT,   //!< ライドポケモンショートカット：右

    ACTION_CALL_TOWNMAP,    //!< タウンマップ

    ACTION_ROTOM_CLOSE,       //!< ロトムメッセージを閉じる
    ACTION_ROTOM_OPEN,        //!< ロトムメッセージを開く
    ACTION_ROTOPON_START,     //!< ロトポン開始
    ACTION_ROTOPON_END,       //!< ロトポン終了
    ACTION_ROTOPON_ITEMEVENT, //!< ロトポン道具入手

    ACTION_CALL_FINDER_STUDIO,  //!< ファインダースタジオ

    ACTION_END,   //!< 終了
  };


public:
  //--------------------------------------------------------------------------
  /**
   * @func    OnAction
   * @brief   入力リスナー
   * @date    2015.05.21
   *
   * @param   id  入力ID
   */
  //--------------------------------------------------------------------------
  virtual void OnAction( u32 id ) = 0;
};

GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)

#endif  // FIELD_MENU_LISTENER_H_INCLUDED

#ifndef NIJI_PROJECT_APP_POKELIST_MENUWINDOW_DRAW_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_MENUWINDOW_DRAW_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    PokeListMenuWindowDraw.h
 * @author  fukushima_yuya
 * @date    2015.03.19
 * @brief   ポケモンリストメニューウィンドウ
 */
//==============================================================================


// niji
#include "App/PokeList/include/PokeListSetupParam.h"
#include "App/PokeList/source/PokeListConst.h"

// pml
#include <pml/include/pmlib.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class MenuWindow;
class MenuWindowListener;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )
class DrawerBase;   //!< 共通描画処理クラス
GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )



GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
// @brief   メニューウィンドウ
//------------------------------------------------------------------------------
class MenuWindowDraw
{
  GFL_FORBID_COPY_AND_ASSIGN( MenuWindowDraw );

public: //! 構造体
  //! @brief    メニュー生成パラメータ
  struct CreateMenuParam
  {
    app::util::AppRenderingManager* pRenderingManager;  //!< メニューウィンドウの生成に必要
    app::tool::MenuWindowListener*  pListner;           //!< メニューウィンドウ用のリスナー
    DrawerBase*   pBase;      //!< DrawerBase
    void*           pCursorBuf; //!< カーソルのリソースバッファ
  };


public: //! コンストラクタ・デストラクタ
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   param   メニュー生成パラメータ
   */
  //------------------------------------------------------------------
  MenuWindowDraw( CreateMenuParam param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~MenuWindowDraw( void );


private: //! 生成・破棄
  //------------------------------------------------------------------
  /**
   * @brief   生成
   * @param   param   メニュー生成パラメータ
   */
  //------------------------------------------------------------------
  void CreateMenu( CreateMenuParam param );

public: //! 生成・破棄
  //------------------------------------------------------------------
  /**
   * @brief   破棄
   */
  //------------------------------------------------------------------
  bool DeleteMenu( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief   メニューウィンドウの取得
   * @param   poke_index  メンバーID(0〜5)
   * @param   menu_id     メニューID
   * @return  メニューウィンドウ
   */
  //------------------------------------------------------------------
  app::tool::MenuWindow* GetMenuWindow( u32 poke_index, MenuID menu_id ) const;

  //------------------------------------------------------------------
  /**
   * @brief   準備完了か？
   * @return  true == 完了
   */
  //------------------------------------------------------------------
  bool IsReady( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンのメニューを生成する
   * @param   param   生成パラメータ
   * @param   pPoke   対象のポケモン
   * @param   id      メンバーID(0〜5)
   * @return  ポケモンのメニューウィンドウ
   */
  //------------------------------------------------------------------
  app::tool::MenuWindow* CreateMenu_POKE(
    CreateMenuParam param, const pml::pokepara::PokemonParam* pPoke, u32 id );

  //------------------------------------------------------------------
  /**
   * @brief   道具メニューを生成する
   * @param   param   生成パラメータ
   * @param   pPoke   対象のポケモン
   * @param   id      メンバーID(0〜5)
   * @return  メニューウィンドウ
   */
  //------------------------------------------------------------------
  app::tool::MenuWindow* CreateMenu_ITEM( CreateMenuParam param, u32 id );

  //------------------------------------------------------------------
  /**
   * @brief   参加メニュー( 未登録 )を生成する
   * @param   param   生成パラメータ
   * @param   pPoke   対象のポケモン
   * @param   id      メンバーID(0〜5)
   * @return  メニューウィンドウ
   */
  //------------------------------------------------------------------
  app::tool::MenuWindow* CreateMenu_JOIN( CreateMenuParam param, u32 id );

  //------------------------------------------------------------------
  /**
   * @brief   参加メニュー( 登録済み )を生成する
   * @param   param   生成パラメータ
   * @param   pPoke   対象のポケモン
   * @param   id      メンバーID(0〜5)
   * @return  メニューウィンドウ
   */
  //------------------------------------------------------------------
  app::tool::MenuWindow* CreateMenu_JOIN_ALREADY( CreateMenuParam param, u32 id );

  //------------------------------------------------------------------
  /**
   * @brief   参加メニュー( 登録できない )を生成する
   * @param   param   生成パラメータ
   * @param   id      メンバーID(0〜5)
   * @return  メニューウィンドウ
   */
  //------------------------------------------------------------------
  app::tool::MenuWindow* CreateMenu_JOIN_DISABLE( CreateMenuParam param, u32 id );
  
  //------------------------------------------------------------------
  /**
   * @brief   育て屋メニューを生成する
   * @param   param   生成パラメータ
   * @param   pPoke   対象のポケモン
   * @param   id      メンバーID(0〜5)
   * @return  メニューウィンドウ
   */
  //------------------------------------------------------------------
  app::tool::MenuWindow* CreateMenu_SODATEYA(
    CreateMenuParam param, const pml::pokepara::PokemonParam* pPoke, u32 id );

  //------------------------------------------------------------------
  /**
   * @brief   二択メニューを生成する
   * @param   param   生成パラメータ
   * @param   id      メンバーID(0〜5)
   * @return  メニューウィンドウ
   */
  //------------------------------------------------------------------
  app::tool::MenuWindow* CreateMenu_YESNO( CreateMenuParam param, u32 id );

  //------------------------------------------------------------------
  /**
   * @brief   プロモ選択メニューを生成する
   * @param   param   生成パラメータ
   * @param   pPoke   対象のポケモン
   * @param   id      メンバーID(0〜5)
   * @return  メニューウィンドウ
   */
  //------------------------------------------------------------------
  app::tool::MenuWindow* CreateMenu_PROMOTION(
    CreateMenuParam param, const pml::pokepara::PokemonParam* pPoke, u32 id );

  //------------------------------------------------------------------
  /**
   * @brief   ウィンドウの生成(共通処理)
   * @param   param     生成パラメータ
   * @param   menu_id   メニューID
   * @param   pAlts     メニュー項目IDのリスト
   * @param   alt_cnt   メニュー項目数
   * @param   id        メンバーID(0〜5)
   * @return  メニューウィンドウ
   */
  //------------------------------------------------------------------
  app::tool::MenuWindow* CreateMenu_COMMON(
    CreateMenuParam param, MenuID menu_id, const MenuAltID* pAlts, u32 alt_cnt, u32 id );


public:   //! 開始・終了
  //------------------------------------------------------------------
  /**
   * @brief   メニューウィンドウを開く
   */
  //------------------------------------------------------------------
  void OpenWindow( void );

  //------------------------------------------------------------------
  /**
   * @brief   メニューウィンドウを開く
   * @param   menuID        メニューID
   * @param   fade_enable   フェードの許可フラグ(デフォルトはtrueでフェードアニメを行う)
   */
  //------------------------------------------------------------------
  void OpenWindow( MenuID menuID, bool fade_enable = true );

  //------------------------------------------------------------------
  /**
   * @brief   メニューウィンドウを閉じる
   * @param   sub_close   サブウィンドウを閉じるフラグ(デフォルトtrue)
   * @param   item_close  アイテム欄を閉じるフラグ(デフォルトfalse)
   */
  //------------------------------------------------------------------
  void CloseWindow( bool sub_close = true, bool item_close = false );

private:  //! 開始・終了
  //------------------------------------------------------------------
  /**
   * @brief   メニューの開始
   * @param   menuID    メニューID
   * @param   cpos      カーソル位置(開くポケモンの位置)
   * @param   pokeIndex カーソル位置のポケモンのインデックス
   */
  //------------------------------------------------------------------
  bool StartMenu( MenuID menuID, CursorPos cpos, u8 pokeIndex );

  //------------------------------------------------------------------
  /**
   * @brief   メニューの終了
   */
  //------------------------------------------------------------------
  void EndMenu( void );


public:
  //------------------------------------------------------------------
  /**
   * @brief   アクティブなメニューのメニューID
   * @return  MENU_NULL == メニューを開いていない
   */
  //------------------------------------------------------------------
  MenuID GetActMenuID( void );

  //------------------------------------------------------------------
  /**
   * @brief   アクティブなメニューのカーソル位置
   * @return  CPOS_NULL == メニューを開いていない
   */
  //------------------------------------------------------------------
  CursorPos GetCursorPos( void );

  //------------------------------------------------------------------
  /**
   * @brief   アクティブなメニューのポケモンのインデックス
   * @return  6以上はメニューを開いていない
   */
  //------------------------------------------------------------------
  u8 GetPokeIndex( void );

  //------------------------------------------------------------------
  /**
   * @brief   選択したメニュー項目IDを取得
   * @param   select_id 選択した項目
   */
  //------------------------------------------------------------------
  MenuAltID GetSelectAltID( u32 select_id );


private:
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンが育て屋に預けられるかどうかを調べる
   * @param   pPokeParam    チェック対象のポケモン
   * @return  true    預けられる
   * @return  false   預けられない
   */
  //------------------------------------------------------------------
  bool IsPokemonSodateyaEnable( const pml::pokepara::PokemonParam* pPokeParam ) const;

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンがプロモに出られるかどうかを調べる
   * @param   pPokeParam    チェック対象のポケモン
   * @return  true    出られる
   * @return  false   出られない
   */
  //------------------------------------------------------------------
  bool IsPokemonPromotioneEnable( const pml::pokepara::PokemonParam* pPokeParam ) const;

  //------------------------------------------------------------------
  /**
   * @brief   メッセージの取得
   * @param   param       生成パラメータ
   * @param   message_id  メッセージID
   * @return  gfl2::str::StrBuf
   */
  //------------------------------------------------------------------
  gfl2::str::StrBuf& GetMessageID( CreateMenuParam param, u32 message_id );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージの取得
   * @param   param     生成パラメータ
   * @param   waza_id   技ID
   * @return  gfl2::str::StrBuf
   */
  //------------------------------------------------------------------
  gfl2::str::StrBuf& GetWazaMessage( CreateMenuParam param, u32 waza_id );


private:
  struct AltDesc
  {
    u32 strID;              //!< 表示する文字列
    bool BButtonLink;       //!< Bボタンと連動するか？
  };

private:
  static const AltDesc ALT_DESC[ALT_NUM];   //!< 選択肢ごとの設定

private:
  DrawerBase*   m_pBase;

  app::tool::MenuWindow* m_pMenu[pml::PokeParty::MAX_MEMBERS][MENU_NUM];    //!< メニューウィンドウ
  MenuAltID m_alts[pml::PokeParty::MAX_MEMBERS][MENU_NUM][8];

  MenuID    m_ActMenuID;
  CursorPos m_ActMenuCPos;
  u8        m_ActPokeIndex;

  u32   m_PrevPos;
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

#endif  // #ifndef NIJI_PROJECT_APP_POKELIST_MENUWINDOW_DRAW_H_INCLUDED

#ifndef NIJI_PROJECT_APP_POKELIST_CONST_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_CONST_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    PokeListConst.h
 * @author  fukushima_yuya
 * @date    2015.03.03
 * @brief   共通定義ヘッダー
 */
//==============================================================================

// pml
#include <App/PokeList/include/PokeListAppParam.h>
#include <App/PokeList/include/PokeListSetupParam.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <Layout/include/gfl2_LytSys.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
// niji
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class AppCommonGrpIconData;
class AppToolHPGaugePartsLayout;
class ButtonManager;
class ItemIconTool;
class MenuCursor;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_BEGIN( ui )
class UIView;
GFL_NAMESPACE_END( ui )
GFL_NAMESPACE_BEGIN( util )
class AppRenderingManager;
class G2DUtil;
class Heap;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( print )
class WordSet;
GFL_NAMESPACE_END( print )

GFL_NAMESPACE_BEGIN( GameSys )
class GameManager;
GFL_NAMESPACE_END( GameSys )

GFL_NAMESPACE_BEGIN( Savedata )
class MyItem;
class MyStatus;
class ZukanData;
GFL_NAMESPACE_END( Savedata )


// gflib2
GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( str )
class MsgData;
class StrBuf;
GFL_NAMESPACE_END( str )
GFL_NAMESPACE_BEGIN( ui )
class Button;
class DeviceManager;
class TouchPanel;
GFL_NAMESPACE_END( ui )
GFL_NAMESPACE_END( gfl2 )

// pml
GFL_NAMESPACE_BEGIN( pml )
GFL_NAMESPACE_BEGIN( pokepara )
class PokemonParam;
GFL_NAMESPACE_END( pokepara )
GFL_NAMESPACE_END( pml )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//! @brief メニューID
enum MenuID
{
  MENU_POKE,            //!< ポケモンのメニュー
  MENU_ITEM,            //!< 道具
  MENU_JOIN,            //!< 参加選択(未登録)
  MENU_JOIN_ALREADY,    //!< 参加選択(登録済み)
  MENU_JOIN_DISABLE,    //!< 参加選択(参加できない)
  MENU_SODATEYA,        //!< 育て屋
  MENU_YESNO,           //!< 「はい・いいえ」二択
  MENU_PROMOTION,       //!< プロモ選択用
  MENU_NUM,
  MENU_NULL
};


//! @brief メニュー項目ID
enum MenuAltID
{
  ALT_POKE_STATUS,      //!< MENU_POKE つよさをみる
  ALT_POKE_REPAIR,      //!< MENU_POKE かいふくする
  ALT_POKE_ITEM,        //!< MENU_POKE もちもの
  ALT_POKE_RETURN,      //!< MENU_POKE やめる
  ALT_ITEM_USE,         //!< MENU_ITEM つかう/もたせる
  ALT_ITEM_STORE,       //!< MENU_ITEM あづかる
  ALT_ITEM_HAND,        //!< MENU_ITEM わたす
  ALT_ITEM_RETURN,      //!< MENU_ITEM やめる
  ALT_JOIN_JOIN,        //!< MENU_JOIN さんかする！
  ALT_JOIN_CANCEL,      //!< MENU_JOIN さんかしない
  ALT_JOIN_STATUS,      //!< MENU_JOIN つよさをみる
  ALT_JOIN_RETURN,      //!< MENU_JOIN やめる
  ALT_SODATEYA_DECIDE,  //!< MENU_SODATEYA あずける
  ALT_SODATEYA_STATUS,  //!< MENU_SODATEYA つよさをみる
  ALT_SODATEYA_RETURN,  //!< MENU_SODATEYA やめる
  ALT_YESNO_YES,        //!< MENU_YESNO はい
  ALT_YESNO_NO,         //!< MENU_YESNO いいえ
  ALT_PROMOTION_DECIDE, //!< MENU_PROMOTION だす
  ALT_PROMOTION_STATUS, //!< MENU_PROMOTION つよさをみる
  ALT_PROMOTION_RETURN, //!< MENU_PROMOTION やめる
  ALT_POKE_FLY,         //!< MENU_POKE おおぞらをとぶ
  ALT_NUM,
  ALT_NULL
}; 


//! @brief プレート
enum PlateID
{
  PLATE_00,     //!< ポケモンのプレート00
  PLATE_01,     //!< ポケモンのプレート01
  PLATE_02,     //!< ポケモンのプレート02
  PLATE_03,     //!< ポケモンのプレート03
  PLATE_04,     //!< ポケモンのプレート04
  PLATE_05,     //!< ポケモンのプレート05
  PLATE_NUM,
  PLATE_NULL
};


//! @brief プレートの表示モード
enum PlateMode
{
  PLATE_MODE_PARAM,     //!< パラメータ表示
  PLATE_MODE_ITEM,      //!< 所持アイテム表示
  PLATE_MODE_JOIN,      //!< 参加選択表示
  PLATE_MODE_WAZAOSHIE, //!< 技教え表示
  PLATE_MODE_SIMPLE2,   //!< 対象選択２
  PLATE_MODE_NUM
};


//! @brief ボタングループ
enum ButtonGroup
{
  BTNGRP_TB,     //!< タッチバー上のボタン
  BTNGRP_WAZA,   //!< 技ボタン
  BTNGRP_PLATE,  //!< プレート
  BTNGRP_DECIDE, //!< 決定・やめる
  BTNGRP_NUM
};


//! @brief タッチバー上のボタン
enum TBButtonID
{
  TBBTN_POKE,    //!< ポケモン入れ替えボタン
  TBBTN_ITEM,    //!< アイテム入れ替えボタン
  TBBTN_EXIT,    //!< 終了ボタン
  TBBTN_RETURN,  //!< 戻るボタン
  TBBTN_NUM,
  TBBTN_HOLD,    //!< タッチ中
  TBBTN_NULL
};


//! @brief 技ボタン
enum WazaButtonID
{
  WBTN_00,
  WBTN_01,
  WBTN_02,
  WBTN_03,
  WBTN_04,
  WBTN_05,
  WBTN_NUM,
  WBTN_NULL
};


//! @brief 決定・止めるボタン
enum DecideButtonID
{
  DBTN_DECIDE,
  DBTN_CANCEL,
  DBTN_NUM,
  DBTN_NULL
};


//! @brief カーソルタイプ
//! @note  上から表示優先順位が高い順
enum CursorID
{
  CID_DEST,  //!< ポケモン( 道具 )の移動先
  CID_MOVE,  //!< 移動するポケモン( 道具 )
  CID_POKE,  //!< ポケモン選択
  CID_NUM
};


//! @brief カーソル位置
enum CursorPos
{
  CPOS_PLATE_TL,   //!< ポケモン( 左上 )
  CPOS_PLATE_TR,   //!< ポケモン( 右上 )
  CPOS_PLATE_ML,   //!< ポケモン( 左中 )
  CPOS_PLATE_MR,   //!< ポケモン( 右中 )
  CPOS_PLATE_BL,   //!< ポケモン( 左下 )
  CPOS_PLATE_BR,   //!< ポケモン( 右下 )
  CPOS_DECIDE,     //!< けってい
  CPOS_NUM,
  CPOS_DECIDE_R = CPOS_NUM,    //!< けってい(右)
  CPOS_NULL,
  CPOS_PLATE_NUM = CPOS_PLATE_BR + 1
};


//! @brief 動作結果
enum OperationResult
{
  RESULT_NONE,            //!< 動作継続中
  RESULT_EXIT,            //!<「おわる×」を選択
  RESULT_RETURN,          //!<「もどる→」を選択
  RESULT_CANCEL,          //!<「やめる→」を選択
  RESULT_STATUS,          //!<「つよさをみる」を選択
  RESULT_FWAZA,           //!<「わざをつかう」を選択
  RESULT_REPAIR,          //!<「かいふくする」を選択
  RESULT_ITEM,            //!<「どうぐをつかう/もたせる」を選択
  RESULT_JOIN,            //!< 参加させるポケモンを「けってい」
  RESULT_TIMEUP,          //!< 時間切れ
  RESULT_SELECTED,        //!< ポケモンを選択した
  RESULT_SORAWOTOBU,      //!<「そらをとぶ」を選択
  RESULT_OOZORAWOTOBU,    //!<「おおぞらをとぶ」を選択

  RESULT_FORCED_RETURN_MANAGER,  //!< ForcedReturnManagerによる終了

  RESULT_NUM
};

//! @brief  ボタンID
enum ButtonID
{
  BTN_ID_PLATE_TL,    //!< プレート左上
  BTN_ID_PLATE_TR,    //!< プレート右上
  BTN_ID_PLATE_ML,    //!< プレート左中
  BTN_ID_PLATE_MR,    //!< プレート右中
  BTN_ID_PLATE_BL,    //!< プレート左下
  BTN_ID_PLATE_BR,    //!< プレート右下
  BTN_ID_DECIDE,      //!< 決定
  BTN_ID_CANCEL,      //!< キャンセル
  BTN_ID_X,           //!< 機能ボタンＸ
  BTN_ID_Y,           //!< 機能ボタンＹ
  BTN_ID_NUM
};

//! @brier  アイコンID
enum IconID
{
  POKE_ICON_TL = 0,
  POKE_ICON_TR,
  POKE_ICON_ML,
  POKE_ICON_MR,
  POKE_ICON_BL,
  POKE_ICON_BR,
  POKE_ICON_NUM,

  JOIN_POKE_ICON_NUM = POKE_ICON_NUM * 4,

  ITEM_ICON_TL = 0,
  ITEM_ICON_TR,
  ITEM_ICON_ML,
  ITEM_ICON_MR,
  ITEM_ICON_BL,
  ITEM_ICON_BR,
  ITEM_ICON_WND,
  ITEM_ICON_NUM
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

#endif  // #ifndef NIJI_PROJECT_APP_POKELIST_CONST_H_INCLUDED

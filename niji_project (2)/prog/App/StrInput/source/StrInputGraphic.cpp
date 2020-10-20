//==============================================================================
/**
 * @file    StrInputGraphic.cpp
 * @brief   グラフィック関連
 * @author  fukushima_yuya
 * @date    2015.05.22
 * @note    sango_project app strinput_graphic.hから移植
 */
//==============================================================================

// niji
#include "App/StrInput/include/StrInputFrame.h"
#include "App/StrInput/source/StrInputDef.h"
#include "App/StrInput/source/StrInputGraphic.h"

#include <AppLib/include/Tool/app_tool_TimeIcon.h>
#include <AppLib/include/Tool/app_tool_LytTexReplaceSystem.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>

// gflib2
#include <math/include/gfl2_math.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/strinput.gaix>
#include <arc_index/message.gaix>
#include <niji_conv_header/app/strinput/strinput.h>
#include <niji_conv_header/app/strinput/strinput_pane.h>
#include <niji_conv_header/app/strinput/strinput_anim_list.h>
#include <niji_conv_header/app/common/graphic_font/strinput_grpfont/StrInputGrpFont.h>
#include <niji_conv_header/app/common/graphic_font/strinput_grpfont/StrInputGrpFont_chn.h>
#include <niji_conv_header/app/common/graphic_font/strinput_grpfont/StrInputGrpFont_deu.h>
#include <niji_conv_header/app/common/graphic_font/strinput_grpfont/StrInputGrpFont_esp.h>
#include <niji_conv_header/app/common/graphic_font/strinput_grpfont/StrInputGrpFont_fra.h>
#include <niji_conv_header/app/common/graphic_font/strinput_grpfont/StrInputGrpFont_ita.h>
#include <niji_conv_header/app/common/graphic_font/strinput_grpfont/StrInputGrpFont_kor.h>
#include <niji_conv_header/app/common/graphic_font/strinput_grpfont/StrInputGrpFont_twn.h>
#include <niji_conv_header/app/common/graphic_font/strinput_grpfont/StrInputGrpFont_usa.h>
#include <niji_conv_header/message/msg_strinput.h>



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace  app::strinput
 * @brief      文字入力画面
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( StrInput )


//! @brief  文字入力画面のリソースに追加するリソースの合計
//static const u32 STRINPUT_RES_ADD_MAX = LYTRES_MSG_WINDOW_RES_END + LYTRES_TEXTURE_END + LYTRES_GRPFONT_STRINPUT_END;

//! @brief  リソースID
enum {
  RES_ID_STRINPUT = 0,    //!< 00: 文字入力画面のリソース
  RES_ID_MSG_WINDOW,      //!< 01: 共通リソース: メッセージウィンドウ
  RES_ID_STRINPUT_TEX,    //!< 02: 文字入力画面のテクスチャ
  RES_ID_STRINPUT_GRAFNT  //!< 03: 文字入力画面グラフィックフォント
};

//! @brief  入力フィールドのペインテーブル
static const gfl2::lyt::LytPaneIndex InputPaneTable[] = {
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_00,    //!< 入力フィールド00
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_01,    //!< 入力フィールド01
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_02,    //!< 入力フィールド02
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_03,    //!< 入力フィールド03
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_04,    //!< 入力フィールド04
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_05,    //!< 入力フィールド05
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_06,    //!< 入力フィールド06
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_07,    //!< 入力フィールド07
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_08,    //!< 入力フィールド08
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_09,    //!< 入力フィールド09
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_10,    //!< 入力フィールド10
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_11,    //!< 入力フィールド11
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_12,    //!< 入力フィールド12
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_13,    //!< 入力フィールド13
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_14,    //!< 入力フィールド14
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_15,    //!< 入力フィールド15
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_16,    //!< 入力フィールド16
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_17,    //!< 入力フィールド17（変換用）
};

//! @brief  入力フィールドのバーのペインテーブル
//static const gfl2::lyt::LytPaneIndex InputBarPaneTable[] = {
//  PANENAME_NAMEBAR_LOWER_PANE_BAR_00,
//  PANENAME_NAMEBAR_LOWER_PANE_BAR_01,
//  PANENAME_NAMEBAR_LOWER_PANE_BAR_02,
//  PANENAME_NAMEBAR_LOWER_PANE_BAR_03,
//  PANENAME_NAMEBAR_LOWER_PANE_BAR_04,
//  PANENAME_NAMEBAR_LOWER_PANE_BAR_05,
//  PANENAME_NAMEBAR_LOWER_PANE_BAR_06,
//  PANENAME_NAMEBAR_LOWER_PANE_BAR_07,
//  PANENAME_NAMEBAR_LOWER_PANE_BAR_08,
//  PANENAME_NAMEBAR_LOWER_PANE_BAR_09,
//  PANENAME_NAMEBAR_LOWER_PANE_BAR_10,
//  PANENAME_NAMEBAR_LOWER_PANE_BAR_11,
//  PANENAME_NAMEBAR_LOWER_PANE_BAR_12,
//  PANENAME_NAMEBAR_LOWER_PANE_BAR_13,
//  PANENAME_NAMEBAR_LOWER_PANE_BAR_14,
//  PANENAME_NAMEBAR_LOWER_PANE_BAR_15,
//};

//! @brief  50音入力のキーマップのペインテーブル
static const gfl2::lyt::LytPaneIndex NormalMapTextPaneTable[] = {
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_00,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_05,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_10,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_15,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_20,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_25,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_30,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_35,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_40,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_45,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_50,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_55,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_60,

  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_01,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_06,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_11,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_16,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_21,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_26,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_31,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_36,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_41,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_46,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_51,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_56,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_61,

  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_02,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_07,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_12,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_17,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_22,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_27,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_32,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_37,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_42,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_47,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_52,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_57,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_62,

  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_03,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_08,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_13,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_18,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_23,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_28,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_33,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_38,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_43,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_48,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_53,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_58,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_63,

  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_04,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_09,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_14,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_19,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_24,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_29,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_34,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_39,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_44,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_49,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_54,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_59,
  PANENAME_STRINPUT_MIN_LOW_000_PANE_WORD_64,
};

//! @brief  キーボード入力のキーマップのペインテーブル
static const gfl2::lyt::LytPaneIndex KeyBordMapTextPaneTable[] = {
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_00,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_01,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_02,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_03,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_04,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_05,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_06,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_07,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_08,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_09,

  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_10,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_11,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_12,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_13,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_14,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_15,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_16,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_17,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_18,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_19,

  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_20,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_21,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_22,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_23,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_24,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_25,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_26,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_27,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_28,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_29,

  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_30,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_31,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_32,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_33,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_34,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_35,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_36,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_37,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_38,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_39,

  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_40,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_41,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_42,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_43,
  PANENAME_STRINPUT_MIN_LOW_001_PANE_WORD_44,
};

////! @brief  入力フィールドのアニメデータ
//static const gfl2::lyt::LytArcIndex Input_LowerAnmIndex[] = {
//  LYTRES_STRINPUT_RES_NAMEBAR_LOWER_TOUCH_BCLAN,
//  LYTRES_STRINPUT_RES_NAMEBAR_LOWER_RELEASE_BCLAN,
//  LYTRES_STRINPUT_RES_NAMEBAR_LOWER_CANSEL_BCLAN,
//  LYTRES_STRINPUT_RES_NAMEBAR_LOWER_PASSIVE_BCLAN,
//  LYTRES_STRINPUT_RES_NAMEBAR_LOWER_ACTIVE_BCLAN,
//  LYTRES_STRINPUT_RES_NAMEBAR_LOWER_TYPE_12_BCLAN,
//  LYTRES_STRINPUT_RES_NAMEBAR_LOWER_TYPE_16_BCLAN,
//  LYTRES_STRINPUT_RES_NAMEBAR_LOWER_BAR_UP_BCLAN,
//  LYTRES_STRINPUT_RES_NAMEBAR_LOWER_BAR_DOWN_BCLAN,
//};
////! @brief  キーマップ切り替えのアニメデータ
//static const gfl2::lyt::LytArcIndex Change_LowerAnmIndex[] = {
//  LYTRES_STRINPUT_RES_NAMECHANGE_LOWER_ARROW_ANIME_BCLAN,
//  LYTRES_STRINPUT_RES_NAMECHANGE_LOWER_TOUCH_BCLAN,
//  LYTRES_STRINPUT_RES_NAMECHANGE_LOWER_RELEASE_BCLAN,
//  LYTRES_STRINPUT_RES_NAMECHANGE_LOWER_CANSEL_BCLAN,
//  LYTRES_STRINPUT_RES_NAMECHANGE_LOWER_PASSIVE_BCLAN,
//  LYTRES_STRINPUT_RES_NAMECHANGE_LOWER_ACTIVE_BCLAN,
//  LYTRES_STRINPUT_RES_NAMECHANGE_LOWER_TOUCH_RELEASE_BCLAN,
//};
////! @brief  キーマップ(50音)のアニメデータ
//static const gfl2::lyt::LytArcIndex KeyMap00_LowerAnmIndex[] = {
//  LYTRES_STRINPUT_RES_NAMEINPUT1_LOWER_TOUCH_BCLAN,
//  LYTRES_STRINPUT_RES_NAMEINPUT1_LOWER_RELEASE_BCLAN,
//  LYTRES_STRINPUT_RES_NAMEINPUT1_LOWER_CANSEL_BCLAN,
//  LYTRES_STRINPUT_RES_NAMEINPUT1_LOWER_PASSIVE_BCLAN,
//  LYTRES_STRINPUT_RES_NAMEINPUT1_LOWER_ACTIVE_BCLAN,
//  LYTRES_STRINPUT_RES_NAMEINPUT1_LOWER_TOUCH_RELEASE_BCLAN,
//  LYTRES_STRINPUT_RES_NAMEINPUT1_LOWER_CURSOL_BCLAN,
//};
////! @brief  キーマップ(キーボード)のアニメデータ
//static const gfl2::lyt::LytArcIndex KeyMap01_LowerAnmIndex[] = {
//  LYTRES_STRINPUT_RES_NAMEINPUT2_LOWER_TOUCH_BCLAN,
//  LYTRES_STRINPUT_RES_NAMEINPUT2_LOWER_RELEASE_BCLAN,
//  LYTRES_STRINPUT_RES_NAMEINPUT2_LOWER_CANSEL_BCLAN,
//  LYTRES_STRINPUT_RES_NAMEINPUT2_LOWER_PASSIVE_BCLAN,
//  LYTRES_STRINPUT_RES_NAMEINPUT2_LOWER_ACTIVE_BCLAN,
//  LYTRES_STRINPUT_RES_NAMEINPUT2_LOWER_TOUCH_RELEASE_BCLAN,
//  LYTRES_STRINPUT_RES_NAMEINPUT2_LOWER_CURSOL_BCLAN,
//};

//! @brief  入力エリアのバーのアニメデータ
static const u32 InputBarAnimIndex[GraphicWork::ANMID_INPUT_BAR_PANE_MAX][GraphicWork::ANMID_INPUT_BAR_PAT_MAX] = {
  { LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_00_BAR_UP, LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_00_BAR_DOWN },
  { LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_01_BAR_UP, LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_01_BAR_DOWN },
  { LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_02_BAR_UP, LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_02_BAR_DOWN },
  { LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_03_BAR_UP, LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_03_BAR_DOWN },
  { LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_04_BAR_UP, LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_04_BAR_DOWN },
  { LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_05_BAR_UP, LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_05_BAR_DOWN },
  { LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_06_BAR_UP, LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_06_BAR_DOWN },
  { LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_07_BAR_UP, LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_07_BAR_DOWN },
  { LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_08_BAR_UP, LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_08_BAR_DOWN },
  { LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_09_BAR_UP, LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_09_BAR_DOWN },
  { LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_10_BAR_UP, LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_10_BAR_DOWN },
  { LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_11_BAR_UP, LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_11_BAR_DOWN },
  { LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_12_BAR_UP, LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_12_BAR_DOWN },
  { LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_13_BAR_UP, LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_13_BAR_DOWN },
  { LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_14_BAR_UP, LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_14_BAR_DOWN },
  { LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_15_BAR_UP, LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_15_BAR_DOWN },
  { LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_16_BAR_UP, LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_16_BAR_DOWN },
  { LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_17_BAR_UP, LA_STRINPUT_STRINPUT_INP_LOW_000__INPUT_17_BAR_DOWN },
};

#include "StrInputButtonAnimList.cdat"

static const f32 INPUT_AREA_PX = -90.0f;  //!< 入力エリア開始X座標
static const f32 INPUT_AREA_SX = 18.0f;    //!< 入力エリアのXサイズ（１文字分）

static const f32 PUT_INPUT_CURSOR_X_OFS = -9.0f;  //!< 入力バー表示Xオフセット


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap    ヒープ
 * @param   isInitApplicationMemory アプリケーションメモリを割り当て直すならtrueを指定
 */
//------------------------------------------------------------------------------
GraphicWork::GraphicWork( app::util::Heap * heap, gfl2::fs::AsyncFileManager * pFileManager, app::util::AppRenderingManager * pRenderingManager, void * pAppLytBuff, void * pTexLytBuff, bool isInitApplicationMemory, bool isFieldSubProc )
  : m_pAsyncFileManager( pFileManager )
  , m_pAppRenderingManager( pRenderingManager )
  , m_pG2D( NULL )
  , m_isInitApplicationMemory( isInitApplicationMemory )
  , m_pPokeIcon( NULL )
  //, m_pTimeIcon( NULL )
  , m_pPokeName( NULL )
  , m_pPokeNameBuff( NULL )
  , m_pPokeNameBuffSize( 0 )
  , m_pokeNameArcID( 0 )
  , m_KeyMapMode( 0 )
  , m_pUppMsgTextBox00( NULL )
  , m_pLowBgrChg00( NULL )
  , m_pLowBgrChg00Bound( NULL )
  , m_pLowBgrChg01( NULL )
  , m_pLowBgrChg01Bound( NULL )
  , m_pPrevWord( NULL )
  , m_pCursorSizePane( NULL )
{
  CreateGraphicsSystem( heap, isInitApplicationMemory );

  Create2D( heap, pAppLytBuff, pTexLytBuff, isFieldSubProc );
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
GraphicWork::~GraphicWork( void )
{
  Delete2D();

  DeleteGraphicsSystem();
}


//------------------------------------------------------------------------------
/**
 * @brief   アップデート
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::Update( void )
{
  m_pG2D->Update2D();

  //m_pTimeIcon->Update();
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::Draw( gfl2::gfx::CtrDisplayNo no )
{
  m_pG2D->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, LAYOUT_ID_UPPER );
  //m_pTimeIcon->Draw( gfl2::lyt::DISPLAY_UPPER );

  m_pG2D->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, LAYOUT_ID_LOWER_CHANGE );
  m_pG2D->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, LAYOUT_ID_LOWER );
  m_pG2D->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, LAYOUT_ID_LOWER_KEYBOARD );
  m_pG2D->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, LAYOUT_ID_LOWER_INPUT );
}

//------------------------------------------------------------------------------
/**
 * @brief   レイアウトワークの取得
 *
 * @param   id    レイアウトID
 *
 * @return  レイアウトワーク
 */
//------------------------------------------------------------------------------
gfl2::lyt::LytWk* GraphicWork::GetLayoutWork( u32 id )
{
  return m_pG2D->GetLayoutWork( id );
}

//----------------------------------------------------------------------------
/**
 * @brief   レイアウトリソースIDの取得
 *
 * @param   id    レイアウトID
 *
 * @return  レイアウトリソースID
 */
//----------------------------------------------------------------------------
gfl2::lyt::LytMultiResID GraphicWork::GetLayoutResourceID( u32 id )
{
  return m_pG2D->GetLayoutResourceID( id );
}

//------------------------------------------------------------------------------
/**
 * @brief   デフォルト文字列描画
 *
 * @param   heap  ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::PutDefaultString( app::util::Heap * heap )
{
  gfl2::lyt::LytWk* pLytWk = m_pG2D->GetLayoutWork( LAYOUT_ID_LOWER );
  gfl2::lyt::LytMultiResID resID  = m_pG2D->GetLayoutResourceID( 0 );
  gfl2::lyt::LytParts* pParts;
  gfl2::lyt::LytTextBox* pTextBox;

  // 消す
  {
    pParts = pLytWk->GetPartsPane( PANENAME_STRINPUT_MIN_LOW_000_PANE_DELETE );
    pTextBox = pLytWk->GetTextBoxPane( pParts, PANENAME_STRINPUT_BTN_LOW_002_PANE_TEXTBOX_00, &resID );
    m_pG2D->SetTextBoxPaneString( pTextBox, msg_strinput_button_01 );
  }
  // おわり
  {
    pParts = pLytWk->GetPartsPane( PANENAME_STRINPUT_MIN_LOW_000_PANE_END );
    pTextBox = pLytWk->GetTextBoxPane( pParts, PANENAME_STRINPUT_BTN_LOW_002_PANE_TEXTBOX_00, &resID );
    m_pG2D->SetTextBoxPaneString( pTextBox, msg_strinput_button_02 );
  }

  pLytWk = m_pG2D->GetLayoutWork( LAYOUT_ID_LOWER_KEYBOARD );
  // space
  {
    pParts = pLytWk->GetPartsPane( PANENAME_STRINPUT_MIN_LOW_001_PANE_SPACE );
    pTextBox = pLytWk->GetTextBoxPane( pParts, PANENAME_STRINPUT_BTN_LOW_002_PANE_TEXTBOX_00, &resID );
    m_pG2D->SetTextBoxPaneString( pTextBox, msg_strinput_button_03 );
  }
  // delete
  {
    pParts = pLytWk->GetPartsPane( PANENAME_STRINPUT_MIN_LOW_001_PANE_DELETE );
    pTextBox = pLytWk->GetTextBoxPane( pParts, PANENAME_STRINPUT_BTN_LOW_002_PANE_TEXTBOX_00, &resID );
    m_pG2D->SetTextBoxPaneString( pTextBox, msg_strinput_button_04 );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   文字コード描画
 *
 * @param   id    レイアウトID
 * @param   code  文字コード
 * @param   pane  ペイン
 * @param   heap  ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::PutStrCode( u32 lytID, const gfl2::str::STRCODE * code, gfl2::lyt::LytPaneIndex pane, app::util::Heap * heap )
{
  gfl2::str::STRCODE tmp[2];
  tmp[0] = *code;
  tmp[1] = gfl2::str::EOM_CODE;
  gfl2::str::StrBuf str( tmp, heap->GetDeviceHeap() );
  m_pG2D->SetTextBoxPaneString( lytID, pane, str.GetPtr() );
}

//------------------------------------------------------------------------------
/**
 * @brief   文字コード描画
 *
 * @param   id    レイアウトID
 * @param   code  文字コード
 * @param   parts パーツペイン
 * @param   pane  ペイン
 * @param   heap  ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::PutStrCode( u32 lytID, const gfl2::str::STRCODE * code, gfl2::lyt::LytPaneIndex parts, gfl2::lyt::LytPaneIndex pane, app::util::Heap * heap )
{
  gfl2::str::STRCODE tmp[2];
  tmp[0] = *code;
  tmp[1] = gfl2::str::EOM_CODE;
  gfl2::str::StrBuf str( tmp, heap->GetDeviceHeap() );

  gfl2::lyt::LytWk * lytwk = m_pG2D->GetLayoutWork( lytID );
  gfl2::lyt::LytMultiResID resID = m_pG2D->GetLayoutResourceID( 0 );

  gfl2::lyt::LytParts * pParts = lytwk->GetPartsPane( parts );
  gfl2::lyt::LytTextBox * pTextBox = lytwk->GetTextBoxPane( pParts, pane, &resID );

  m_pG2D->SetTextBoxPaneString( pTextBox, str.GetPtr() );
}

//------------------------------------------------------------------------------
/**
 * @brief   入力フィールド表示初期化
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::InitInputField( void )
{
  gfl2::lyt::LytWk * lytwk = m_pG2D->GetLayoutWork( LAYOUT_ID_LOWER_INPUT );
  gfl2::lyt::LytMultiResID resID = m_pG2D->GetLayoutResourceID( 0 );

  // ローマ字変換分を＋１している
  for( int i=0; i<INPUT_AREA_MAX+1; i++ )
  {
    gfl2::lyt::LytParts* parts = lytwk->GetPartsPane( InputPaneTable[i] );
    gfl2::lyt::LytPane*  pane  = lytwk->GetPane( parts, PANENAME_STRINPUT_BTN_LOW_003_PANE_INPUT_00, &resID );

    m_pG2D->SetPaneVisible( pane, false );
  }

  SetOverwriteCursorVisible( false );
}

//------------------------------------------------------------------------------
/**
 * @brief   入力文字描画
 *
 * @param   lytID レイアウトID
 * @param   code  文字コード
 * @param   index 入力位置
 * @param   heap  ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::PutInputStr( u32 lytID, const gfl2::str::STRCODE * code, u32 index, app::util::Heap * heap )
{
  gfl2::lyt::LytPaneIndex paneIndex = PANENAME_STRINPUT_BTN_LOW_003_PANE_INPUT_00;

  // 文字コード描画
  PutStrCode( lytID, code, InputPaneTable[index], paneIndex, heap );

  gfl2::lyt::LytWk * lytwk = m_pG2D->GetLayoutWork( lytID );
  gfl2::lyt::LytMultiResID resID = m_pG2D->GetLayoutResourceID( 0 );
  gfl2::lyt::LytParts* parts = lytwk->GetPartsPane( InputPaneTable[index] );
  gfl2::lyt::LytPane*  pane  = lytwk->GetPane( parts, paneIndex, &resID );

  // 入力ペインの表示
  m_pG2D->SetPaneVisible( pane, true );
}

//------------------------------------------------------------------------------
/**
 * @brief   入力文字を消す
 *
 * @param   lytID レイアウトID
 * @param   index 入力位置
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::ClearInputStr( u32 lytID, u32 index )
{
  gfl2::lyt::LytWk * lytwk = m_pG2D->GetLayoutWork( lytID );
  gfl2::lyt::LytMultiResID resID = m_pG2D->GetLayoutResourceID( 0 );
  gfl2::lyt::LytParts* parts = lytwk->GetPartsPane( InputPaneTable[index] );
  gfl2::lyt::LytPane*  pane  = lytwk->GetPane( parts, PANENAME_STRINPUT_BTN_LOW_003_PANE_INPUT_00, &resID );

  // 非表示にする
  m_pG2D->SetPaneVisible( pane, false );
}

//------------------------------------------------------------------------------
/**
 * @brief   入力文字色を変更
 *
 * @param   lytID レイアウトID
 * @param   index 入力位置
 * @param   mode  色モード
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::SetInputStrColor( u32 lytID, u32 index, u32 mode )
{
  static const gfl2::lyt::ColorU8 normal( 0, 0, 0, 255 );
  static const gfl2::lyt::ColorU8 romaji( 0, 128, 255, 255 );

  gfl2::lyt::LytWk* pLytWk = m_pG2D->GetLayoutWork( lytID );
  gfl2::lyt::LytMultiResID resID  = m_pG2D->GetLayoutResourceID( 0 );
  gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( InputPaneTable[index] );
  gfl2::lyt::LytTextBox* pTextBox = pLytWk->GetTextBoxPane( pParts, PANENAME_STRINPUT_BTN_LOW_003_PANE_INPUT_00, &resID );

  if( mode == INPUT_COLOR_MODE_NORMAL )
  {
    m_pG2D->SetTextBoxPaneColor( pTextBox, normal, normal );
  }
  else {
    m_pG2D->SetTextBoxPaneColor( pTextBox, romaji, romaji );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   入力位置カーソル表示
 *
 * @param   lytID レイアウトID
 * @param   index 入力位置
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::PutInputCursor( u32 lytID, u32 index )
{
  gfl2::math::VEC3 vec = m_pG2D->GetPaneLocalPos( lytID, InputPaneTable[index] );
  gfl2::math::Vector3 pos = gfl2::math::ConvertNwVec3ToGfVec3( vec );
  pos.x += PUT_INPUT_CURSOR_X_OFS;
  vec = gfl2::math::ConvertGfVec3ToNwVec3( pos );
  m_pG2D->SetPanePos( lytID, PANENAME_STRINPUT_INP_LOW_000_PANE_CURSOR_01, &vec );
}

//------------------------------------------------------------------------------
/**
 * @brief   上書き用カーソル表示切替
 *
 * @param   flg   true = 表示、false = 非表示
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::SetOverwriteCursorVisible( bool flg )
{
  m_pG2D->SetPaneVisible( LAYOUT_ID_LOWER_INPUT, PANENAME_STRINPUT_INP_LOW_000_PANE_CURSOR_02, flg );
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソル移動
 *
 * @param   mode  入力モード
 * @param   base  動先のペイン
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::MoveCursor( u32 mode, gfl2::lyt::LytPaneIndex parts, gfl2::lyt::LytPaneIndex pane )
{
  gfl2::lyt::LytWk * lytwk = NULL;
  gfl2::lyt::LytParts * cursor = NULL;
  gfl2::lyt::LytMultiResID resID = m_pG2D->GetLayoutResourceID(0);
  u32 paneID = 0;

  // モードごとのカーソルを取得
  if( mode == INPUT_MODE_AIUEO )
  {
    lytwk = m_pG2D->GetLayoutWork( LAYOUT_ID_LOWER );
    paneID = PANENAME_STRINPUT_MIN_LOW_000_PANE_CURSOR_00;
  }
  else {
    lytwk = m_pG2D->GetLayoutWork( LAYOUT_ID_LOWER_KEYBOARD );
    paneID = PANENAME_STRINPUT_MIN_LOW_001_PANE_CURSOR_00;
  }

  // カーソルパーツ
  cursor = lytwk->GetPartsPane( paneID );
  // カーソル部品内のウィンドウペイン
  m_pCursorWindow = lytwk->GetWindowPane( cursor, PANENAME_STRINPUT_CUR_LOW_000_PANE_WINDOW_00, &resID );

  gfl2::lyt::LytParts* pParts = lytwk->GetPartsPane( parts );
  m_pCursorSizePane = lytwk->GetPane( pParts, pane, &resID ); //!< 境界ペインをカーソルのサイズ指定で使用

  PutCursor( lytwk, cursor, pParts );

  m_KeyMapMode = mode;
  m_pPrevWord = pParts;
}

//------------------------------------------------------------------------------
/**
 * @brief   上書き用カーソル配置
 *
 * @param   lytID レイアウトID
 * @param   pane  カーソルペイン
 * @param   pos   入力開始位置
 * @param   size  表示サイズ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::PutInputSize( u32 lytID, gfl2::lyt::LytPaneIndex pane, u32 pos, u32 size )
{
  if( size == 0 )
  {
    m_pG2D->SetPaneVisible( lytID, pane, false );
  }
  else {
    gfl2::math::Vector2 scale( 1.0f, 1.0f );
    scale.x *= (f32)size;
    gfl2::math::VEC2 scaleConv = gfl2::math::ConvertGfVec2ToNwVec2(scale);
    m_pG2D->SetScale( lytID, pane, scaleConv );

    gfl2::math::VEC3 trans = m_pG2D->GetPaneLocalPos( lytID, pane );
    trans.x = INPUT_AREA_PX + INPUT_AREA_SX * (f32)pos + ( INPUT_AREA_SX * (f32)( size - 1 ) ) / 2.0f;
    m_pG2D->SetPanePos( lytID, pane, &trans );

    m_pG2D->SetPaneVisible( lytID, pane, true );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   上画面メッセージ表示
 *
 * @param   mode  画面モード
 * @param   prm   パラメータ(ポケモン番号など)
 * @param   heap  ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::PutUpperMessage( u32 mode, u32 prm, app::util::Heap * heap )
{
  u32 msgID = 0;

  switch( mode )
  {
  case TYPE_CHARACTER_NAME:    // 人物名
#if PM_DEBUG
  case TYPE_CHARACTER_NAME_DEBUG:  // 人物名デバッグ
#endif
    msgID = msg_strinput_01_01;
    m_pG2D->SetTextBoxPaneString( m_pUppMsgTextBox00, msgID );
    break;
  case TYPE_POKEMON_NAME:      // ポケモン名
    m_pG2D->SetRegisterMonsNameNo( 0, static_cast<MonsNo>(prm) );
    msgID = msg_strinput_01_02;
    m_pG2D->SetTextBoxPaneStringExpand( m_pUppMsgTextBox00, msgID );
    break;
  case TYPE_BOX_NAME:          // ボックス名
    msgID = msg_strinput_01_03;
    m_pG2D->SetTextBoxPaneString( m_pUppMsgTextBox00, msgID );
    break;
  case TYPE_FREE_WORD_PSS:    // 一言メッセージ ( PSS )
    //msgID = msg_strinput_01_04;
    //m_pG2D->SetTextBoxPaneString( m_pUppMsgTextBox00, msgID );
    break;
  case TYPE_FREE_WORD_GTS:    // 一言メッセージ ( GTS )
    //msgID = msg_strinput_01_06;
    //m_pG2D->SetTextBoxPaneString( m_pUppMsgTextBox00, msgID );
    break;
  case TYPE_NICKNAME:          // ニックネーム
    //msgID = msg_strinput_01_05;
    //m_pG2D->SetTextBoxPaneString( m_pUppMsgTextBox00, msgID );
    break;
  case TYPE_POKE_SEARCH:      // GTSポケモン検索
    //msgID = msg_strinput_01_07;
    //m_pG2D->SetTextBoxPaneString( m_pUppMsgTextBox00, msgID );
    break;
  case TYPE_POKE_WANT:        // GTSポケモン検索（欲しいポケモン）
    msgID = msg_strinput_01_09;
    m_pG2D->SetTextBoxPaneString( m_pUppMsgTextBox00, msgID );
    break;
  case TYPE_HAPPY_SIGN:        // ハッピーサイン
    // EFIGS文法タグの展開のためExpand
    //msgID = msg_strinput_01_08;
    //m_pG2D->SetTextBoxPaneStringExpand( m_pUppMsgTextBox00, msgID );
    break;

  case TYPE_INTERVIEW_LONG:        // 一言メッセージ ( インタビュー: 16文字 )
  case TYPE_INTERVIEW_SHORT:      // 一言メッセージ ( インタビュー: 8文字 )
  case TYPE_INTERVIEW_VERY_SHORT:  // 一言メッセージ ( インタビュー: 6文字 )
    //msgID = msg_strinput_01_10;
    //m_pG2D->SetTextBoxPaneString( m_pUppMsgTextBox00, msgID );
    break;

  case TYPE_BASE_WORD:        // 秘密基地：団名と合言葉
    //msgID = msg_strinput_01_11;
    //m_pG2D->SetTextBoxPaneString( m_pUppMsgTextBox00, msgID );
    break;

  case TYPE_BASE_HAPPY:        // 秘密基地：嬉しいときの言葉
    // EFIGS文法タグの展開のためExpand
    //msgID = msg_strinput_01_12;
    //m_pG2D->SetTextBoxPaneStringExpand( m_pUppMsgTextBox00, msgID );
    break;

  case TYPE_BASE_CHEER:        // 秘密基地：励ますときの言葉
    // EFIGS文法タグの展開のためExpand
    //msgID = msg_strinput_01_13;
    //m_pG2D->SetTextBoxPaneStringExpand( m_pUppMsgTextBox00, msgID );
    break;

  case TYPE_BASE_GREETING:    // 秘密基地：挨拶
    //msgID = msg_strinput_01_14;
    //m_pG2D->SetTextBoxPaneString( m_pUppMsgTextBox00, msgID );
    break;

  case TYPE_BASE_GOODS:        // 秘密基地：グッズ
    //msgID = msg_strinput_01_15;
    //m_pG2D->SetTextBoxPaneString( m_pUppMsgTextBox00, msgID );
    break;

  case TYPE_TEAM_NAME:          // チーム名
    msgID = msg_strinput_01_16;
    m_pG2D->SetTextBoxPaneString( m_pUppMsgTextBox00, msgID );
    break;

  case TYPE_FESCIRCLE_NAME:     // フェスサークル名
    msgID = msg_strinput_01_17;
    m_pG2D->SetTextBoxPaneString( m_pUppMsgTextBox00, msgID );
    break;

  default:
    GFL_ASSERT( 0 );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   エラーメッセージ表示
 *
 * @param   id    エラーID
 * @param   heap  ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::PutErrorMessage( u32 id, app::util::Heap * heap )
{
  switch( id )
  {
  case ERR_ID_NG_WORD:        // NGワード
    m_pG2D->SetTextBoxPaneString( m_pUppMsgTextBox00, msg_strinput_NG_00 );
    break;
  case ERR_ID_NUMBER:          // 数字入力数オーバー
    m_pG2D->SetTextBoxPaneString( m_pUppMsgTextBox00, msg_strinput_NG_01 );
    break;
  case ERR_ID_FAST:            // 連打入力
    m_pG2D->SetTextBoxPaneString( m_pUppMsgTextBox00, msg_strinput_NG_02 );
    break;
  case ERR_ID_CHARNAME_NULL:  // 空欄（名前入力時）
    m_pG2D->SetTextBoxPaneString( m_pUppMsgTextBox00, msg_strinput_NG_04 );
    break;
  case ERR_ID_NICKNAME_NULL:  // 空欄（ニックネーム入力時）
    //m_pG2D->SetTextBoxPaneString( m_pUppMsgTextBox00, msg_strinput_NG_03 );
    break;
  case ERR_ID_INTERVIEW_NULL:  // 空欄（インタビュー入力時）
    //m_pG2D->SetTextBoxPaneString( m_pUppMsgTextBox00, msg_strinput_NG_05 );
    break;
  default:
    GFL_ASSERT( 0 );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   キーボード表示切替
 *
 * @param   mode  入力モード
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::PutKeyBoard( u32 mode )
{
  if( mode == INPUT_MODE_AIUEO )
  {
    m_pG2D->SetLayoutDrawEnable( LAYOUT_ID_LOWER, true );
    m_pG2D->SetLayoutDrawEnable( LAYOUT_ID_LOWER_KEYBOARD, false );
    // 左矢印
    //m_pG2D->SetPaneVisible( LAYOUT_ID_LOWER_CHANGE, PANENAME_NAMECHANGE_LOWER_PANE_CHANGE_BOUND00, false );
    //m_pG2D->SetPaneVisible( LAYOUT_ID_LOWER_CHANGE, PANENAME_NAMECHANGE_LOWER_PANE_CHANGE0, false );
    m_pG2D->SetPaneVisible( m_pLowBgrChg00Bound, false );
    m_pG2D->SetPaneVisible( m_pLowBgrChg00, false );
    // 右矢印
    //m_pG2D->SetPaneVisible( LAYOUT_ID_LOWER_CHANGE, PANENAME_NAMECHANGE_LOWER_PANE_CHANGE_BOUND01, true );
    //m_pG2D->SetPaneVisible( LAYOUT_ID_LOWER_CHANGE, PANENAME_NAMECHANGE_LOWER_PANE_CHANGE1, true );
    m_pG2D->SetPaneVisible( m_pLowBgrChg01Bound, true );
    m_pG2D->SetPaneVisible( m_pLowBgrChg01, true );
  }
  else {
    m_pG2D->SetLayoutDrawEnable( LAYOUT_ID_LOWER, false );
    m_pG2D->SetLayoutDrawEnable( LAYOUT_ID_LOWER_KEYBOARD, true );
    // 左矢印
    //m_pG2D->SetPaneVisible( LAYOUT_ID_LOWER_CHANGE, PANENAME_NAMECHANGE_LOWER_PANE_CHANGE_BOUND00, true );
    //m_pG2D->SetPaneVisible( LAYOUT_ID_LOWER_CHANGE, PANENAME_NAMECHANGE_LOWER_PANE_CHANGE0, true );
    m_pG2D->SetPaneVisible( m_pLowBgrChg00Bound, true );
    m_pG2D->SetPaneVisible( m_pLowBgrChg00, true );
    // 右矢印
    //m_pG2D->SetPaneVisible( LAYOUT_ID_LOWER_CHANGE, PANENAME_NAMECHANGE_LOWER_PANE_CHANGE_BOUND01, false );
    //m_pG2D->SetPaneVisible( LAYOUT_ID_LOWER_CHANGE, PANENAME_NAMECHANGE_LOWER_PANE_CHANGE1, false );
    m_pG2D->SetPaneVisible( m_pLowBgrChg01Bound, false );
    m_pG2D->SetPaneVisible( m_pLowBgrChg01, false );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   キーボードマップのペインを取得
 *
 * @param   mode  入力モード
 * @param   id    取得位置
 *
 * @return  none
 */
//------------------------------------------------------------------------------
gfl2::lyt::LytPaneIndex GraphicWork::GetKeyMapTextPane( u32 mode, u32 id )
{
  if( mode == INPUT_MODE_AIUEO )
  {
    return NormalMapTextPaneTable[id];
  }

  return KeyBordMapTextPaneTable[id];
}

//------------------------------------------------------------------------------
/**
 * @brief   モード切替矢印のループアニメ設定
 *
 * @param   flg   true = 再生、false = 停止
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::SetArrowAutoAnime( bool flg )
{
  u32 anmID_ArwL = LA_STRINPUT_STRINPUT_BGR_LOW_000__CHANGE_00_ACTIVE;
  u32 anmID_ArwR = LA_STRINPUT_STRINPUT_BGR_LOW_000__CHANGE_01_ACTIVE;

  if( flg == true )
  {
    m_pG2D->StartAnime( LAYOUT_ID_LOWER_CHANGE, anmID_ArwL );
    m_pG2D->StartAnime( LAYOUT_ID_LOWER_CHANGE, anmID_ArwR );
  }
  else {
    m_pG2D->StopAnime( LAYOUT_ID_LOWER_CHANGE, anmID_ArwL );
    m_pG2D->StopAnime( LAYOUT_ID_LOWER_CHANGE, anmID_ArwR );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   モード切替矢印の選択アニメ再生
 *
 * @param   mv    切り替え方向
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::SetArrowSelectAnime( u32 mv )
{
  u32  anmID;

  if( mv == 0 )
  {
    anmID = LA_STRINPUT_STRINPUT_BGR_LOW_000__CHANGE_00_TOUCH_RELEASE;
  }
  else {
    anmID = LA_STRINPUT_STRINPUT_BGR_LOW_000__CHANGE_01_TOUCH_RELEASE;
  }

  m_pG2D->StopAnime( LAYOUT_ID_LOWER_CHANGE, anmID );
  m_pG2D->StartAnime( LAYOUT_ID_LOWER_CHANGE, anmID );
}

//------------------------------------------------------------------------------
/**
 * @brief   入力タイプのマーク表示
 *
 * @param   mode    入力モード
 * @param   type    入力タイプ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::PutTypeMark( u32 mode, u32 type )
{
  static const gfl2::lyt::LytPaneIndex c_KeyMap_N_TypeBtn[] = {
    PANENAME_STRINPUT_MIN_LOW_000_PANE_BUTTON_00,
    PANENAME_STRINPUT_MIN_LOW_000_PANE_BUTTON_01,
    PANENAME_STRINPUT_MIN_LOW_000_PANE_BUTTON_02,
    PANENAME_STRINPUT_MIN_LOW_000_PANE_BUTTON_03,
  };

  static const gfl2::lyt::LytPaneIndex c_KeyMap_B_TypeBtn[] = {
    PANENAME_STRINPUT_MIN_LOW_001_PANE_BUTTON_00,
    PANENAME_STRINPUT_MIN_LOW_001_PANE_BUTTON_01,
    PANENAME_STRINPUT_MIN_LOW_001_PANE_BUTTON_02,
  };

  gfl2::lyt::LytWk * lytwk;
  gfl2::lyt::LytMultiResID resID = m_pG2D->GetLayoutResourceID( 0 );

  const gfl2::lyt::LytPaneIndex* btnIdx;
  const gfl2::lyt::LytPaneIndex markIdx = PANENAME_STRINPUT_BTN_LOW_001_PANE_MARK_01;
  u32 btnNum;

  if( mode == INPUT_MODE_AIUEO )
  {
    lytwk  = m_pG2D->GetLayoutWork( LAYOUT_ID_LOWER );
    btnIdx = c_KeyMap_N_TypeBtn;
    btnNum = GFL_NELEMS(c_KeyMap_N_TypeBtn);
  }
  else {
    lytwk  = m_pG2D->GetLayoutWork( LAYOUT_ID_LOWER_KEYBOARD );
    btnIdx = c_KeyMap_B_TypeBtn;
    btnNum = GFL_NELEMS(c_KeyMap_B_TypeBtn);
  }

  for( u32 i=0; i<btnNum; ++i )
  {
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( btnIdx[i] );
    gfl2::lyt::LytPane * mark = lytwk->GetPane( parts, markIdx, &resID );

    m_pG2D->SetPaneVisible( mark, (type == i) );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   シフトマーク表示切り替え
 *
 * @param   flg   true = 表示、false = 非表示
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::PutShiftMark( bool flg )
{
  gfl2::lyt::LytWk* lytwk = m_pG2D->GetLayoutWork( LAYOUT_ID_LOWER_KEYBOARD );
  gfl2::lyt::LytMultiResID resID = m_pG2D->GetLayoutResourceID( 0 );
  gfl2::lyt::LytParts* lytparts = lytwk->GetPartsPane( PANENAME_STRINPUT_MIN_LOW_001_PANE_SHIFT );
  gfl2::lyt::LytPane* mark = lytwk->GetPane( lytparts, PANENAME_STRINPUT_BTN_LOW_002_PANE_MARK_01, &resID );

  m_pG2D->SetPaneVisible( mark, flg );
  //m_pG2D->SetPaneVisible( LAYOUT_ID_LOWER_KEYBOARD, PANENAME_NAMEINPUT2_LOWER_PANE_MARK_02, flg );
}

//------------------------------------------------------------------------------
/**
 * @brief   キー選択アニメ再生
 *
 * @param   mode  入力モード
 * @param   pos   入力位置
 *
 * @return  再生するアニメ
 */
//------------------------------------------------------------------------------
u32 GraphicWork::StartKeySelectAnime( u32 mode, u32 pos )
{
  u32  lytID;
  u32  anmID;

  if( mode == INPUT_MODE_AIUEO )
  {
    lytID = LAYOUT_ID_LOWER;
    anmID = KeyMap50onBtnTopAnm[pos] + ANMID_KEYMAP_N_TOUCH_RELEASE;
  }
  else {
    lytID = LAYOUT_ID_LOWER_KEYBOARD;
    anmID = KeyMapKeyboardBtnTopAnm[pos] + ANMID_KEYMAP_B_TOUCH_RELEASE;
  }

  m_pG2D->StopAnime( lytID, anmID );
  m_pG2D->StartAnime( lytID, anmID );

  return anmID;
}

//------------------------------------------------------------------------------
/**
 * @brief   入力バー表示
 *
 * @param   size  入力可能文字数
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::PutInputBar( u32 size )
{
  u32 lytID = LAYOUT_ID_LOWER_INPUT;

  gfl2::lyt::LytWk* pLytWk = m_pG2D->GetLayoutWork( lytID );
  gfl2::lyt::LytMultiResID resID  = m_pG2D->GetLayoutResourceID( 0 );

  for( int i=size; i<INPUT_MAX; i++ )
  {
    gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( InputPaneTable[i] );
    gfl2::lyt::LytPane*  pPane  = pLytWk->GetPane( pParts, PANENAME_STRINPUT_BTN_LOW_003_PANE_BAR_00, &resID );

    //m_pG2D->SetPaneVisible( LAYOUT_ID_LOWER_INPUT, InputBarPaneTable[i], false );
    m_pG2D->SetPaneVisible( pPane, false );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   入力バーアニメ設定
 *
 * @param   pos   入力開始位置
 * @param   size  入力サイズ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::SetInputBarAnime( u32 pos, u32 size )
{
  if( size != 0 )
  {
    size -= 1;
  }

  for( u32 i=0; i<INPUT_MAX; i++ )
  {
    const u32 * pBarAnm = InputBarAnimIndex[i];

    if( i >= pos && i <= pos+size )
    {
      m_pG2D->StopAnime( LAYOUT_ID_LOWER_INPUT, pBarAnm[ANMID_INPUT_BAR_UP] );
      m_pG2D->StartAnime( LAYOUT_ID_LOWER_INPUT, pBarAnm[ANMID_INPUT_BAR_DOWN] );
    }
    else {
      m_pG2D->StopAnime( LAYOUT_ID_LOWER_INPUT, pBarAnm[ANMID_INPUT_BAR_DOWN] );
      m_pG2D->StartAnime( LAYOUT_ID_LOWER_INPUT, pBarAnm[ANMID_INPUT_BAR_UP] );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンアイコン作成
 *
 * @param   heap  ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::CreatePokeIcon( app::util::Heap * heap )
{
  gfl2::heap::HeapBase* pDevHeap = heap->GetDeviceHeap();

  m_pPokeIcon = GFL_NEW( pDevHeap ) app::tool::PokeIcon( pDevHeap, pDevHeap, 1 );
  //m_pPokeIcon->OpenArcFile( pDevHeap, gfl2::fs::ArcFile::OPEN_FASTMODE );
  m_pPokeIcon->FileOpenSync( pDevHeap );  //!< 同期読み込み
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンアイコン削除
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::DeletePokeIcon( void )
{
  if( m_pPokeIcon != NULL )
  {
    GFL_DELETE m_pPokeIcon;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンアイコン配置
 *
 * @param   pp    ポケモンデータ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::PutPokeIcon( const pml::pokepara::CoreParam * pp )
{
  gfl2::lyt::LytWk* lytwk = m_pG2D->GetLayoutWork( LAYOUT_ID_LOWER_INPUT );
  gfl2::lyt::LytMultiResID resID = m_pG2D->GetLayoutResourceID( 0 );
  gfl2::lyt::LytParts* lytparts = lytwk->GetPartsPane( PANENAME_STRINPUT_INP_LOW_000_PANE_TARGETICON );
  gfl2::lyt::LytPicture* pokeIcon = lytwk->GetPicturePane( lytparts, PANENAME_STRINPUT_BTN_LOW_004_PANE_POKE_ICON_00, &resID );
  gfl2::lyt::LytParts* genderIconParts = lytwk->GetPartsPane( lytparts, PANENAME_STRINPUT_BTN_LOW_004_PANE_GENDERICON, &resID );

  //m_pPokeIcon->ReplacePaneTexture(
  //  0, pp, 0, m_pG2D->GetLayoutWork(LAYOUT_ID_LOWER_INPUT)->GetPicturePane(PANENAME_NAMEBAR_LOWER_PANE_POKE_ICON_00) );
  //m_pG2D->SetPaneVisible( LAYOUT_ID_LOWER_INPUT, PANENAME_NAMEBAR_LOWER_PANE_POKE_ICON_00, true );
  m_pPokeIcon->ReplacePaneTexture( 0, pp, pokeIcon );
  m_pG2D->SetPaneVisible( pokeIcon, true );

  pml::Sex sex = pp->GetSex();
  if( sex == pml::SEX_MALE )
  {
    //m_pG2D->SetTextBoxPaneString( LAYOUT_ID_LOWER_INPUT, PANENAME_NAMEBAR_LOWER_PANE_POKE_SEX, msg_strinput_sex_male, NULL );
    //m_pG2D->SetPaneVisible( LAYOUT_ID_LOWER_INPUT, PANENAME_NAMEBAR_LOWER_PANE_POKE_SEX, true );
    m_pG2D->StartAnime( LAYOUT_ID_LOWER_INPUT, LA_STRINPUT_STRINPUT_INP_LOW_000__TARGETICON__GENDERICON_ICON_MALE);
    m_pG2D->SetPaneVisible( genderIconParts, true );
  }
  else if( sex == pml::SEX_FEMALE )
  {
    //m_pG2D->SetTextBoxPaneString( LAYOUT_ID_LOWER_INPUT, PANENAME_NAMEBAR_LOWER_PANE_POKE_SEX, msg_strinput_sex_female, NULL );
    //m_pG2D->SetPaneVisible( LAYOUT_ID_LOWER_INPUT, PANENAME_NAMEBAR_LOWER_PANE_POKE_SEX, true );
    m_pG2D->StartAnime( LAYOUT_ID_LOWER_INPUT, LA_STRINPUT_STRINPUT_INP_LOW_000__TARGETICON__GENDERICON_ICON_FEMALE);
    m_pG2D->SetPaneVisible( genderIconParts, true );
  }
  else {
    //m_pG2D->SetPaneVisible( LAYOUT_ID_LOWER_INPUT, PANENAME_NAMEBAR_LOWER_PANE_POKE_SEX, false );
    m_pG2D->SetPaneVisible( genderIconParts, false );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボックスアイコン配置
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::PutBoxIcon( void )
{
  gfl2::lyt::LytWk* lytwk = m_pG2D->GetLayoutWork( LAYOUT_ID_LOWER_INPUT );
  gfl2::lyt::LytMultiResID resID = m_pG2D->GetLayoutResourceID( 0 );
  gfl2::lyt::LytParts* lytparts = lytwk->GetPartsPane( PANENAME_STRINPUT_INP_LOW_000_PANE_TARGETICON );
  gfl2::lyt::LytPicture* boxIcon = lytwk->GetPicturePane( lytparts, PANENAME_STRINPUT_BTN_LOW_004_PANE_BOX_ICON_00, &resID );

  //m_pG2D->SetPaneVisible( LAYOUT_ID_LOWER_INPUT, PANENAME_NAMEBAR_LOWER_PANE_BOX_ICON_00, true );
  m_pG2D->SetPaneVisible( boxIcon, true );   //!< ボックスアイコンを表示
}

//------------------------------------------------------------------------------
/**
 * @brief   プレイヤーアイコン表示
 *
 * @param   man   ファイルリードマネージャ
 * @param   heap  ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::CreatePlayerIcon( gfl2::fs::AsyncFileManager * man, app::util::Heap * heap )
{
  // PlayerIcon
  //m_pPlayerIconMan = GFL_NEW( heap->GetDeviceHeap() ) common::resource::PlayerIconTexManager( man, heap->GetDeviceHeap(), 1, 1 );
}

//------------------------------------------------------------------------------
/**
 * @brief   プレイヤーアイコン削除
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::DeletePlayerIcon( void )
{
  // PlayerIcon
  //if( m_pPlayerIconTex != NULL )
  //{
  //  m_pPlayerIconMan->DestroyIcon( m_pPlayerIconTex, false, 0 );
  //}

  //if( m_pPlayerIconMan != NULL )
  //{
  //  GFL_DELETE m_pPlayerIconMan;
  //}
}

//------------------------------------------------------------------------------
/**
 * @brief   プレイヤーアイコン生成リクエスト
 *
 * @param   myst  プレイヤーデータ
 * @param   heap  ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::CreatePlayerIconTexRequest( Savedata::MyStatus * myst, app::util::Heap * heap )
{
  // PlayerIcon
  //common::resource::PlayerIconTexManager::IconDesc desc;
  //common::resource::PlayerIconTexManager::IconParam param;

  //// 主人公♂
  //if( myst->GetSex() == PM_MALE )
  //{
  //  param.fix_id = xy_system::player_icon::TR_ICON_ID_0038;
  //// 主人公♀
  //}
  //else {
  //  param.fix_id = xy_system::player_icon::TR_ICON_ID_0039;
  //}
  //param.fashion_buffer = &myst->GetDressUpData();

  //desc.replace_info_num_max = 1;

  //m_pPlayerIconTex = m_pPlayerIconMan->RequestCreatePlayerIconTex(
  //                    heap->GetSystemHeap(),
  //                    heap->GetDeviceHeap(),
  //                    heap->GetSystemHeap(),
  //                    heap->GetDeviceHeap(),
  //                    param,
  //                    desc );

  //common::resource::PlayerIconTex::ReplaceInfo info;
  //info.replace_mode = common::resource::PlayerIconTex::REPLACE_MODE_WAIT;
  //info.pane = m_pG2D->GetLayoutWork(LAYOUT_ID_LOWER_INPUT)->GetPicturePane(PANENAME_NAMEBAR_LOWER_PANE_FACE_ICON_00);

  //m_pPlayerIconTex->AddReplaceInfo( info );

  //m_pG2D->SetPaneVisible( LAYOUT_ID_LOWER_INPUT, PANENAME_NAMEBAR_LOWER_PANE_FACE_ICON_00, true );
}

//------------------------------------------------------------------------------
/**
 * @brief   プレイヤーアイコン生成チェック
 *
 * @param   none
 *
 * @return  "true = 生成完了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool GraphicWork::CheckCreatePlayerIconTex( void )
{
  // PlayerIcon
  //m_pPlayerIconMan->Update();

  //if( m_pPlayerIconMan->IsIconCreated( m_pPlayerIconTex ) == true )
  //{
  //  return true;
  //}

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   画面開始アニメ再生
 *
 * @param   anm   開始アニメ番号
 * @param   flg   アイコン表示フラグ true = 表示
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::SetStartAnime( u32 anm, bool flg )
{
  // 背景アニメの再生
  m_pG2D->StartAnime( LAYOUT_ID_UPPER, LA_STRINPUT_STRINPUT_DSC_UPP_000__BG_WAVE_BLUE_UPP_00_BG_LOOP_00 );

  m_pG2D->StartAnime( LAYOUT_ID_LOWER_INPUT, anm );
  //m_pG2D->StartAnime( LAYOUT_ID_LOWER_INPUT, LA_STRINPUT_STRINPUT_INP_LOW_000_IN );
  //m_pG2D->SetPaneVisible( LAYOUT_ID_LOWER_CHANGE, PANENAME_NAMECHANGE_LOWER_PANE_OBON_00, flg );
  m_pG2D->SetPaneVisible( LAYOUT_ID_LOWER_INPUT, PANENAME_STRINPUT_INP_LOW_000_PANE_OBON_00, flg );

  //m_pG2D->StartAnime( LAYOUT_ID_LOWER, LA_STRINPUT_STRINPUT_MIN_LOW_000_IN );
  //m_pG2D->StartAnime( LAYOUT_ID_LOWER_KEYBOARD, LA_STRINPUT_STRINPUT_MIN_LOW_001_IN );
  //m_pG2D->StartAnime( LAYOUT_ID_LOWER_CHANGE, LA_STRINPUT_STRINPUT_BGR_LOW_000_IN );
  //m_pG2D->StartAnime( LAYOUT_ID_LOWER_KEYBOARD, LA_STRINPUT_STRINPUT_MIN_LOW_001_IN );

  if( flg == false )
  {
    gfl2::lyt::LytWk* lytWk = m_pG2D->GetLayoutWork( LAYOUT_ID_LOWER_INPUT );
    gfl2::lyt::LytMultiResID resID = m_pG2D->GetLayoutResourceID( 0 );

    gfl2::lyt::LytParts* pParts = lytWk->GetPartsPane( PANENAME_STRINPUT_INP_LOW_000_PANE_TARGETICON );
    gfl2::lyt::LytPane*  pBound = lytWk->GetPane( pParts, PANENAME_STRINPUT_BTN_LOW_004_PANE_ICON_BOUND, &resID );

    //m_pG2D->SetPaneVisible( LAYOUT_ID_LOWER_INPUT, PANENAME_NAMEBAR_LOWER_PANE_ICON_BOUND, false );
    m_pG2D->SetPaneVisible( pBound, false );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   画面開始アニメ再生チェック
 *
 * @param   anm   開始アニメ番号
 *
 * @return  "false = 再生中"
 * @return  "true = それ以外"
 */
//------------------------------------------------------------------------------
bool GraphicWork::CheckStartAnime( u32 anm )
{
  if( m_pG2D->IsAnimeEnd( LAYOUT_ID_LOWER_INPUT, anm ) == false )
  {
    m_pG2D->StopAnime( LAYOUT_ID_LOWER_INPUT, anm );
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   タイマーアイコン表示切り替え
 *
 * @param   flg   true = 表示
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::SetVisibleTimeIcon( bool flg )
{
  // タイマーアイコンのひょうじきりかえ
  u32 anm = (flg) ? LA_STRINPUT_STRINPUT_DSC_UPP_000__TIMERICON_IN : LA_STRINPUT_STRINPUT_DSC_UPP_000__TIMERICON_OUT;
  m_pG2D->StartAnime( LAYOUT_ID_UPPER, anm );

  if( flg )
  {
    m_pG2D->SetPaneVisible( LAYOUT_ID_UPPER, PANENAME_STRINPUT_DSC_UPP_000_PANE_TIMERICON, flg );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   文字列がポケモン名かどうかをチェック（ゲーム中の言語のみ）
 *
 * @param   str   文字列
 *
 * @return  "true = ポケモン名"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool GraphicWork::CheckPokemonName( gfl2::str::STRCODE * str )
{
  gfl2::str::StrBuf * buf = m_pG2D->GetTempStrBuf( 1 );

  for( int i=MONSNO_NULL+1; i<=MONSNO_END; i++ )
  {
    m_pG2D->SetRegisterMonsNameNo( 0, static_cast<MonsNo>(i) );
    m_pG2D->GetStringExpand( buf, msg_strinput_pokemon_check );

//    if( gfl2::str::StrComp( buf->GetPtr(), str ) == true )
    if( ToLowerComp( buf->GetPtr(), str ) != false )
    {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   多言語ポケモン名のオープン
 *
 * @param   lang    言語ID
 * @param   heap    ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::StartOpenPokeNameMsgData( u32 lang, gfl2::heap::HeapBase * heap )
{
  typedef struct {
    u32 lang;
    System::MSGLANGID msg;
  }LOAD_MSG;

  static const LOAD_MSG tbl[] =
  {
    { POKEMON_LANG_JAPAN,   System::MSGLANGID_KANA    },
    { POKEMON_LANG_ENGLISH, System::MSGLANGID_ENGLISH },
    { POKEMON_LANG_FRANCE,  System::MSGLANGID_FRENCH  },
    { POKEMON_LANG_ITALY,   System::MSGLANGID_ITALIAN },
    { POKEMON_LANG_GERMANY, System::MSGLANGID_GERMAN  },
    { POKEMON_LANG_SPAIN,   System::MSGLANGID_SPANISH },
    { POKEMON_LANG_KOREA,   System::MSGLANGID_KOREAN  },
  };

  for( int i=0; i<GFL_NELEMS(tbl); i++ )
  {
    if( lang == tbl[i].lang )
    {
      m_pokeNameArcID = print::GetMessageArcId( tbl[i].msg );
      app::util::FileAccessor::FileOpen( m_pokeNameArcID, heap->GetLowerHandle() );
      break;
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   多言語ポケモン名のオープン待ち
 *
 * @param   none
 *
 * @return  "true = 終了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool GraphicWork::WaitOpenPokeNameMsgData( void )
{
  return app::util::FileAccessor::IsFileOpen( m_pokeNameArcID );
}

//------------------------------------------------------------------------------
/**
 * @brief   多言語ポケモン名の読み込み
 *
 * @param   heap    ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::StartLoadPokeNameMsgData( gfl2::heap::HeapBase * heap )
{
  app::util::FileAccessor::FileLoad(
    m_pokeNameArcID,
    GARC_message_monsname_DAT,
    &m_pPokeNameBuff,
    heap,
    false );
}

//------------------------------------------------------------------------------
/**
 * @brief   多言語ポケモン名の読み込み待ち
 *
 * @param   none
 *
 * @return  "true = 終了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool GraphicWork::WaitLoadPokeNameMsgData( gfl2::heap::HeapBase * heap )
{
  bool result = app::util::FileAccessor::IsFileLoad( &m_pPokeNameBuff );

  if( result )
  {
    m_pPokeName = GFL_NEW( heap ) gfl2::str::MsgData( m_pPokeNameBuff, heap );
  }

  return result;
}

//------------------------------------------------------------------------------
/**
 * @brief   多言語ポケモン名のクローズ
 *
 * @param   heap    ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::StartClosePokeNameMsgData( gfl2::heap::HeapBase * heap )
{
  app::util::FileAccessor::FileClose( m_pokeNameArcID, heap->GetLowerHandle() );
}

//------------------------------------------------------------------------------
/**
 * @brief   多言語ポケモン名のクローズ待ち
 *
 * @param   none
 *
 * @return  "true = 終了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool GraphicWork::WaitClosePokeNameMsgData( void )
{
  return app::util::FileAccessor::IsFileClose( m_pokeNameArcID );
}

//------------------------------------------------------------------------------
/**
 * @brief   他言語ポケモン名を読み込み
 *
 * @param   lang  読み込む言語
 * @param   frman ファイルリードマネージャ
 * @param   heap  ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::StartLoadPokeNameMsgData( u32 lang, gfl2::fs::AsyncFileManager * frman, gfl2::heap::HeapBase * heap )
{
  typedef struct {
    u32 lang;
    System::MSGLANGID msg;
  }LOAD_MSG;

  static const LOAD_MSG tbl[] =
  {
    { POKEMON_LANG_JAPAN,   System::MSGLANGID_KANA    },
    { POKEMON_LANG_ENGLISH, System::MSGLANGID_ENGLISH },
    { POKEMON_LANG_FRANCE,  System::MSGLANGID_FRENCH  },
    { POKEMON_LANG_ITALY,   System::MSGLANGID_ITALIAN },
    { POKEMON_LANG_GERMANY, System::MSGLANGID_GERMAN  },
    { POKEMON_LANG_SPAIN,   System::MSGLANGID_SPANISH },
    { POKEMON_LANG_KOREA,   System::MSGLANGID_KOREAN  },
  };

  for( int i=0; i<GFL_NELEMS(tbl); i++ )
  {
    if( lang == tbl[i].lang )
    {
      // fs::AsyncFileManager
      m_pokeNameArcID = print::GetMessageArcId( tbl[i].msg );
      //frman->AppendManageFile( heap, m_pokeNameArcID );

      //gfl2::fs::AsyncFileManager::ReadRequest req;
      //req.requestPriority     = STRINPUT_NG_WORD_CHECK_THREAD_PRIORITY;
      //req.arcID               = pokeNameArcID;
      //req.datID               = GARC_message_monsname_DAT;
      //req.compressed          = false;
      //req.heapForWork         = heap->GetLowerHandle();
      //req.heapForUncomp       = heap;
      //req.heapForBuffer       = heap;
      //req.destBufferAlignment = 4;
      //req.ppDestBuffer        = &pPokeNameBuff;
      //req.pDestBufferSize     = &pPokeNameBuffSize;
      //frman->AddReadRequest( req );
      //break;
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   他言語ポケモン名削除
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::EndLoadPokeNameMsgData( void )
{
  GFL_DELETE m_pPokeName;

  GflHeapFreeMemory( m_pPokeNameBuff );
}

//------------------------------------------------------------------------------
/**
 * @brief   他言語ポケモン名読み込み待ち
 *
 * @param   frman ファイルマネージャ
 * @param   heap  ヒープ
 *
 * @return  "true = 終了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool GraphicWork::WaitLoadPokeNameMsgData( gfl2::fs::AsyncFileManager * frman, gfl2::heap::HeapBase * heap )
{
  // AsyncFileManager
  //if( frman->IsReadFinished( &m_pPokeNameBuff ) == false )
  //{
  //  return false;
  //}

  //frman->RemoveManageFile( m_pokeNameArcID );

  if( frman->IsArcFileLoadDataFinished( &m_pPokeNameBuff ) )
  {
    return false;
  }

  m_pPokeName = GFL_NEW( heap ) gfl2::str::MsgData( m_pPokeNameBuff, heap );
  
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   文字列がポケモン名かどうかをチェック（ゲーム中の言語以外）
 *
 * @param   str   文字列
 *
 * @return  "true = ポケモン名"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool GraphicWork::CheckPokemonNameLocalize( gfl2::str::STRCODE * str )
{
  gfl2::str::StrBuf * buf = m_pG2D->GetTempStrBuf( 1 );

  for( int i=MONSNO_NULL+1; i<=MONSNO_END; i++ )
  {
    m_pPokeName->GetString( i, *buf );

//    if( gfl2::str::StrComp( buf->GetPtr(), str ) == true )
    if( ToLowerComp( buf->GetPtr(), str ) != false )
    {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   UGCメッセージ表示
 *
 * @param   flg   true = 表示、false = 非表示
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::PutUgcMessage( bool flg )
{
  if( flg == true )
  {
    //m_pG2D->SetTextBoxPaneString( LAYOUT_ID_UPPER, PANENAME_STRINPUT_DSC_UPP_000_PANE_TEXT_01, msg_strinput_information_01 );
  }

  m_pG2D->SetPaneVisible( LAYOUT_ID_UPPER, PANENAME_STRINPUT_DSC_UPP_000_PANE_TEXT_POS, flg );
}


//------------------------------------------------------------------------------
/**
 * @brief   グラフィックシステム作成
 *
 * @param   heap  ヒープ
 * @param   isInitApplicationMemory アプリケーションメモリを割り当て直すならtrueを指定
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::CreateGraphicsSystem( app::util::Heap * heap, bool isInitApplicationMemory )
{
  // GraphicsSystemは使用しない
  //gamesystem::GameManager * gm = GFL_SINGLETON_INSTANCE( gamesystem::GameManager );
  //pGraSystem = gm->GetGraphicsSystem();

  if( isInitApplicationMemory )
  {
    //pGraSystem->InitializeApplicationMemory( heap->pDevHeapAllocator );
    //gfl2::grp::util::DrawUtil::PushAllocator( heap->GetDeviceHeap()Allocator, heap->pDevHeapAllocator );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   グラフィックシステム削除
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::DeleteGraphicsSystem( void )
{
  if( m_isInitApplicationMemory ){
    //gfl2::grp::util::DrawUtil::PopAllocator();
    //pGraSystem->FinalizeApplicationMemory();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 *
 * @param   heap  ヒープ
 * @param   pAppLytBuff   レイアウトバッファ
 * @param   isFieldSubProc
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::Create2D( app::util::Heap * heap, void* pAppLytBuff, void* pTexLytBuff, bool isFieldSubProc )
{
  m_pG2D = GFL_NEW( heap->GetDeviceHeap() ) app::util::G2DUtil( heap );

  // アーカイブIDテーブル
  //static const app::util::G2DUtil::LYT_RES_DATA resTbl[] = {
  //  { ARCID_STRINPUT_GRA, GARC_strinput_res_strinput_res_lz_LYTDAT, STRINPUT_RES_ADD_MAX },  //!< 00: 文字入力画面リソース
  //  { ARCID_COMMON_MSGWINDOW, 0, 0 },                                                        //!< 01: 共通リソース: メッセージウィンドウ
  //  { ARCID_STRINPUT_GRA, GARC_strinput_res_texture_lz_LYTDAT, 0 },                          //!< 02: 文字入力画面リソース
  //  { ARCID_GRPFONT_STRINPUT, GARC_graphic_font_strinput_grpfont_strinput_LYTDAT_LZ, 0 },    //!< 03: 文字入力画面グラフィックフォント
  //};
  const app::util::G2DUtil::LytResData resTbl[] = {
    { pAppLytBuff, 1, static_cast<u32>(app::util::G2DUtil::ATTACH_ROOT) },
    { pTexLytBuff, LYTRES_STRINPUTGRPFONT_END, static_cast<u32>(app::util::G2DUtil::ATTACH_ROOT) },
  };

  // レイアウトデータ
  //static const app::util::G2DUtil::LYTWK_DATA layoutData[] = {
  const app::util::G2DUtil::LytwkData layoutData[] = {
    {  // 上画面
      0,                                            //!< 使用するリソースの番号
      LYTRES_STRINPUT_STRINPUT_DSC_UPP_000_BFLYT,   //!< レイアウトデータID ( *_BFLYT )
      LA_STRINPUT_STRINPUT_DSC_UPP_000___NUM,       //!< アニメ数
      pAppLytBuff,                                  //!< リソースデータ
      false,                                        //!< 非表示ペインに行列計算を行うか
      app::util::G2DUtil::SETUP_UPPER,              //!< ディスプレイ
      true,                                         //!< 表示設定
    },
    {  // 下画面・50音入力
      0,
      LYTRES_STRINPUT_STRINPUT_MIN_LOW_000_BFLYT,
      LA_STRINPUT_STRINPUT_MIN_LOW_000___NUM,
      pAppLytBuff,
      false,
      app::util::G2DUtil::SETUP_LOWER,
      true,
    },
    {  // 下画面・キーボード
      0,
      LYTRES_STRINPUT_STRINPUT_MIN_LOW_001_BFLYT,
      LA_STRINPUT_STRINPUT_MIN_LOW_001___NUM,
      pAppLytBuff,
      false,
      app::util::G2DUtil::SETUP_LOWER,
      false,
    },
    {  // 下画面・入力フィールド
      0,
      LYTRES_STRINPUT_STRINPUT_INP_LOW_000_BFLYT,
      LA_STRINPUT_STRINPUT_INP_LOW_000___NUM,
      pAppLytBuff,
      false,                                      //!< 非表示ペインに行列計算を行うか
      app::util::G2DUtil::SETUP_LOWER,
      true,
    },
    {  // 下画面・ページ切り替え
      0,
      LYTRES_STRINPUT_STRINPUT_BGR_LOW_000_BFLYT,
      LA_STRINPUT_STRINPUT_BGR_LOW_000___NUM,
      pAppLytBuff,
      false,                                      //!< 非表示ペインに行列計算を行うか
      app::util::G2DUtil::SETUP_LOWER,
      true,
    },
  };

  // ArcFile
  //gfl2::fs::ArcFile::SetArcLang( System::GetLang() );   //!< 言語ごとのアーカイブファイルを読み込む
#if 0
  m_pG2D->CreateLayoutSystem( heap );
  m_pG2D->CreateLayoutEnv( heap, false );

  //  m_pG2D->CreateLayoutWork( resTbl, GFL_NELEMS(resTbl), LAYOUT_ID_MAX, heap, true );
  m_pG2D->CreateLayoutWork( GFL_NELEMS(layoutData), heap );
  m_pG2D->CreateLayoutResource( GFL_NELEMS(resTbl), heap );
  m_pG2D->LoadLayoutResource( 0, resTbl, GFL_NELEMS(resTbl), heap, true );
  m_pG2D->TransferLayoutTexture( /*gfl2::grp::MEMORY_AREA_FCRAM*/ );
  m_pG2D->RegistLayoutResource( RES_ID_MSG_WINDOW, RES_ID_STRINPUT );
  m_pG2D->RegistLayoutResource( RES_ID_STRINPUT_TEX, RES_ID_STRINPUT );
  m_pG2D->RegistLayoutResource( RES_ID_STRINPUT_GRAFNT, RES_ID_STRINPUT );

  if( m_isInitApplicationMemory ){
    //m_pG2D->SetLayoutWork( layoutData, 0, GFL_NELEMS(layoutData), true );
    m_pG2D->SetLayoutWork( 0, layoutData, GFL_NELEMS(layoutData), NULL, NULL, true );
  }
  else{
    //m_pG2D->SetLayoutWork( heap->GetSystemAllocator(), heap->GetDeviceAllocator(), layoutData, 0, GFL_NELEMS(layoutData), true );
    m_pG2D->SetLayoutWork( 0, layoutData, GFL_NELEMS(layoutData), heap->GetSystemAllocator(), heap->GetDeviceAllocator(), true );
  }
#else
  m_pG2D->CreateAppLytAccessor( heap );

  m_pG2D->CreateLayoutSystem( heap );
  m_pG2D->CreateLayoutEnv( heap, false );

  m_pG2D->CreateLayoutWork( resTbl, GFL_NELEMS(resTbl), GFL_NELEMS(layoutData), heap, gfl2::lyt::MEMORY_AREA_FCRAM );

  m_pG2D->AttachLayoutResourceReference( resTbl, GFL_NELEMS(resTbl) );

  if( m_isInitApplicationMemory )
  {
    m_pG2D->SetLayoutWork( 0, layoutData, GFL_NELEMS(layoutData), NULL, NULL, false, false );
  }
  else {
    m_pG2D->SetLayoutWork( 0, layoutData, GFL_NELEMS(layoutData), heap->GetDeviceAllocator(), heap->GetDeviceAllocator(), false, false );
  }
#endif
  m_pG2D->CreateMessageData( GARC_message_strinput_DAT, gfl2::str::MsgData::LOAD_FULL, heap );

  SetLayoutPane();

  SetShareAnime_Input( heap );
  SetShareAnime_Change( heap );
  SetShareAnime_KeyMapNormal( heap );
  SetShareAnime_KeyMapKeyBoard( heap );

  SetTextureGraphicFont();
  PutDefaultString( heap );
  SetArrowAutoAnime( true );
  
  m_pG2D->StartAnime( LAYOUT_ID_LOWER, ANMID_KEYMAP_N_CURSOR );
  m_pG2D->StartAnime( LAYOUT_ID_LOWER_KEYBOARD, ANMID_KEYMAP_B_CURSOR );

  CreateTimeIcon( heap, isFieldSubProc );
}

//------------------------------------------------------------------------------
/**
 * @brief   2D関連削除
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::Delete2D( void )
{
  DeleteTimeIcon();

  m_pG2D->DeleteMessageData();
  m_pG2D->DeleteLayoutWork();
  m_pG2D->DeleteLayoutEnv();
  m_pG2D->DeleteLayoutSystem();
  m_pG2D->DeleteAppLytAccessor();

  GFL_DELETE  m_pG2D;
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソル配置
 *
 * @param   lytwk   レイアウトワーク
 * @param   cursor  カーソルペイン
 * @param   base    配置位置のペイン
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::PutCursor( gfl2::lyt::LytWk * lytwk, gfl2::lyt::LytPane * cursor, gfl2::lyt::LytPane * base )
{
  // カーソルペインを親から切り離す
  gfl2::lyt::LytPane * parent = cursor->GetParent();
  if( parent != NULL )
  {
    //parent->RemoveChild( cursor );
  }
  
#if 0
  // ペースペインの親にカーソルペインを追加
  parent = base->GetParent();
  if( parent != NULL && parent != base->GetParent() )
  {
    parent->AppendChild( cursor );
    cursor->SetTranslate( parent->GetTranslate() );
  }
  else {
    base->AppendChild( cursor );
    cursor->SetTranslate( base->GetTranslate() );
  }
#else
  //base->AppendChild( cursor );
  //cursor->SetTranslate( gfl2::math::VEC3(0, 0, 0) );
  cursor->SetTranslate( base->GetTranslate() );
#endif

  if( m_pCursorSizePane && m_pCursorWindow )
  {
    nw::lyt::Size size = m_pCursorSizePane->GetSize();

    // カーソルのサイズを境界ペインのサイズを基準にする
    size.width += 20;
    size.height += 20;

    // カーソルペインのサイズを設定
    m_pCursorWindow->SetSize( size );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   共有アニメ登録（入力フィールド）
 *
 * @param   heap  ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::SetShareAnime_Input( app::util::Heap * heap )
{
  // 共有アニメは使用しない
  //app::util::LytUtil::ShareAnimeDesc * desc = GFL_NEW_LOW_ARRAY( heap->GetDeviceHeap()) app::util::LytUtil::ShareAnimeDesc[ANMID_INPUT_SHARE_MAX];

  //for( int i=0; i<ANMID_INPUT_BAR_SHARE_PANE_MAX; i++ )
  //{
  //  for( int j=0; j<ANMID_INPUT_BAR_SHARE_PAT_MAX; j++ )
  //  {
  //    u32  index = i * ANMID_INPUT_BAR_SHARE_PAT_MAX + j;
  //    app::util::LytUtil::ShareAnimeDesc * buf = &desc[index];
  //    buf->start_anime_index = ANMID_INPUT_BAR_SHARE + index;               //!< アニメ先頭インデックス
  //    buf->max_share_num     = 1;                                           //!< 最大共有数
  //    buf->anime_data        = Input_LowerAnmIndex[ANMID_INPUT_BAR_UP+j];   //!< アニメデータインデックス
  //    buf->share_group_index = PANENAME_NAMEBAR_LOWER_GROUP_BAR_ACTION;     //!< シェア対象のグループ
  //    buf->share_pane_index  = InputBarPaneTable[i+1];                      //!< シェア対象のペイン
  //  }
  //}

  //for( int i=0; i<ANMID_INPUT_ICON_SHARE_MAX; i++ )
  //{
  //  app::util::LytUtil::ShareAnimeDesc * buf = &desc[ANMID_INPUT_BAR_SHARE_MAX+i];
  //  buf->start_anime_index = i + ANMID_INPUT_ICON_SHARE;                    //!< アニメ先頭インデックス
  //  buf->max_share_num     = 1;                                             //!< 最大共有数
  //  buf->anime_data        = Input_LowerAnmIndex[i];                        //!< アニメデータインデックス
  //  buf->share_group_index = PANENAME_NAMEBAR_LOWER_GROUP_BUTON_ANIME;      //!< シェア対象のグループ
  //  //buf->share_pane_index  = PANENAME_NAMEBAR_LOWER_PANE_POKE_ICON_00;    //!< シェア対象のペイン
  //  buf->share_pane_index  = PANENAME_NAMEBAR_LOWER_PANE_ICON_BOUND;        //!< シェア対象のペイン
  //}

  //m_pG2D->CreateShareAnime( LAYOUT_ID_LOWER_INPUT, desc, ANMID_INPUT_SHARE_MAX );

  //GFL_DELETE_ARRAY desc;
}

//------------------------------------------------------------------------------
/**
 * @brief   共有アニメ登録（モード切替矢印）
 *
 * @param   heap  ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::SetShareAnime_Change( app::util::Heap * heap )
{
  // 共有アニメは使用しない
  //static const gfl2::grp::g2d::LytPaneIndex pane[] = {
  //  PANENAME_NAMECHANGE_LOWER_PANE_CHANGE_00,
  //  PANENAME_NAMECHANGE_LOWER_PANE_CHANGE_01,
  //};

  //app::util::LytUtil::ShareAnimeDesc * desc = GFL_NEW_LOW_ARRAY( heap->GetDeviceHeap()) app::util::LytUtil::ShareAnimeDesc[ANMID_CHANGE_SHARE_MAX];

  //for( int i=0; i<ANMID_CHANGE_SHARE_PANE_MAX; i++ )
  //{
  //  for( int j=0; j<BTNANM_MAX+1; j++ )
  //  {
  //    u32  index = i * (BTNANM_MAX+1) + j;
  //    app::util::LytUtil::ShareAnimeDesc * buf = &desc[index];
  //    buf->start_anime_index = index + ANMID_CHANGE_SHARE;                        //!< アニメ先頭インデックス
  //    buf->max_share_num     = 1;                                                 //!< 最大共有数
  //    buf->anime_data        = Change_LowerAnmIndex[ANMID_CHANGE_ARROW_TOUCH+j];  //!< アニメデータインデックス
  //    buf->share_group_index = PANENAME_NAMECHANGE_LOWER_GROUP_BUTON_ANIME;       //!< シェア対象のグループ
  //    buf->share_pane_index  = pane[i];                                           //!< シェア対象のペイン
  //  }
  //}

  //m_pG2D->CreateShareAnime( LAYOUT_ID_LOWER_CHANGE, desc, ANMID_CHANGE_SHARE_MAX );

  //GFL_DELETE_ARRAY desc;
}

//------------------------------------------------------------------------------
/**
 * @brief   共有アニメ登録（50音入力のキーマップ）
 *
 * @param   heap  ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::SetShareAnime_KeyMapNormal( app::util::Heap * heap )
{
  // 共有アニメは使用しない
  //static const gfl2::lyt::LytPaneIndex pane[] = {
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_00,    //!< 00: "あ"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_05,    //!< 01: "か"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_10,    //!< 02: "さ"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_15,    //!< 03: "た"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_20,    //!< 04: "な"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_25,    //!< 05: "は"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_30,    //!< 06: "ま"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_35,    //!< 07: "や"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_40,    //!< 08: "ら"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_45,    //!< 09: "わ"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_50,    //!< 10: "ぁ"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_55,    //!< 11: "ゃ"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_60,    //!< 12: "゛"

  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_01,    //!< 13: "い"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_06,    //!< 14: "き"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_11,    //!< 15: "し"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_16,    //!< 16: "ち"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_21,    //!< 17: "に"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_26,    //!< 18: "ひ"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_31,    //!< 19: "み"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_36,    //!< 20: "　"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_41,    //!< 21: "り"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_46,    //!< 22: "　"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_51,    //!< 23: "ぃ"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_56,    //!< 24: "ゅ"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_61,    //!< 25: "゜"

  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_02,    //!< 26: "う"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_07,    //!< 27: "く"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_12,    //!< 28: "す"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_17,    //!< 29: "つ"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_22,    //!< 30: "ぬ"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_27,    //!< 31: "ふ"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_32,    //!< 32: "む"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_37,    //!< 33: "ゆ"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_42,    //!< 34: "る"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_47,    //!< 35: "を"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_52,    //!< 36: "ぅ"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_57,    //!< 37: "ょ"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_62,    //!< 38: "ー"

  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_03,    //!< 39: "え"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_08,    //!< 40: "け"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_13,    //!< 41: "せ"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_18,    //!< 42: "て"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_23,    //!< 43: "ね"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_28,    //!< 44: "へ"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_33,    //!< 45: "め"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_38,    //!< 46: "　"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_43,    //!< 47: "れ"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_48,    //!< 48: "　"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_53,    //!< 49: "ぇ"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_58,    //!< 50: "っ"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_63,    //!< 51: "、"

  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_04,    //!< 52: "お"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_09,    //!< 53: "こ"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_14,    //!< 54: "そ"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_19,    //!< 55: "と"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_24,    //!< 56: "の"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_29,    //!< 57: "ほ"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_34,    //!< 58: "も"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_39,    //!< 59: "よ"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_44,    //!< 60: "ろ"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_49,    //!< 61: "ん"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_54,    //!< 62: "ぉ"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_59,    //!< 63: "　"
  //  PANENAME_NAMEINPUT1_LOWER_PANE_WORD_64,    //!< 64: "。"

  //  PANENAME_NAMEINPUT1_LOWER_PANE_BUTTON_00,  //!< 65: かな
  //  PANENAME_NAMEINPUT1_LOWER_PANE_BUTTON_01,  //!< 66: カナ
  //  PANENAME_NAMEINPUT1_LOWER_PANE_BUTTON_02,  //!< 67: ABC
  //  PANENAME_NAMEINPUT1_LOWER_PANE_BUTTON_03,  //!< 68: 1/♪
  //  PANENAME_NAMEINPUT1_LOWER_PANE_DELETE,     //!< 69: けす
  //  PANENAME_NAMEINPUT1_LOWER_PANE_END,        //!< 70: おわり
  //};

  //app::util::LytUtil::ShareAnimeDesc * desc = GFL_NEW_LOW_ARRAY( heap->GetDeviceHeap()) app::util::LytUtil::ShareAnimeDesc[ANMID_KEYMAP_N_SHARE_MAX];

  //for( int i=0; i<ANMID_KEYMAP_N_SHARE_PANE_MAX; i++ )
  //{
  //  for( int j=0; j<BTNANM_MAX+1; j++ )
  //  {
  //    u32  index = i * (BTNANM_MAX+1) + j;
  //    app::util::LytUtil::ShareAnimeDesc * buf = &desc[index];
  //    buf->start_anime_index = index + ANMID_KEYMAP_N_SHARE;                  //!< アニメ先頭インデックス
  //    buf->max_share_num     = 1;                                             //!< 最大共有数
  //    buf->anime_data        = KeyMap00_LowerAnmIndex[j];                     //!< アニメデータインデックス
  //    buf->share_group_index = PANENAME_NAMEINPUT1_LOWER_GROUP_BUTTON_ANIME;  //!< シェア対象のグループ
  //    buf->share_pane_index  = pane[i];                                       //!< シェア対象のペイン
  //  }
  //}

  //m_pG2D->CreateShareAnime( LAYOUT_ID_LOWER, desc, ANMID_KEYMAP_N_SHARE_MAX );

  //GFL_DELETE_ARRAY desc;
}

//!<------------------------------------------------------------------------------
/**
 * @brief   共有アニメ登録（キーボードのキーマップ）
 *
 * @param   heap  ヒープ
 *
 * @return  none
 */
//!<------------------------------------------------------------------------------
void GraphicWork::SetShareAnime_KeyMapKeyBoard( app::util::Heap * heap )
{
  // 共有アニメは使用しない
  //static const gfl2::lyt::LytPaneIndex pane[] = {
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_00,  //!< 00: "１"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_01,  //!< 01: "２"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_02,  //!< 02: "３"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_03,  //!< 03: "４"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_04,  //!< 04: "５"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_05,  //!< 05: "６"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_06,  //!< 06: "７"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_07,  //!< 07: "８"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_08,  //!< 08: "９"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_09,  //!< 09: "０"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_10,  //!< 10: "ー"

  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_11,  //!< 11: "Ｑ"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_12,  //!< 12: "Ｗ"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_13,  //!< 13: "Ｅ"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_14,  //!< 14: "Ｒ"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_15,  //!< 15: "Ｔ"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_16,  //!< 16: "Ｙ"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_17,  //!< 17: "Ｕ"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_18,  //!< 18: "Ｉ"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_19,  //!< 19: "Ｏ"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_20,  //!< 20: "Ｐ"

  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_21,  //!< 21: "Ａ"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_22,  //!< 22: "Ｓ"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_23,  //!< 23: "Ｄ"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_24,  //!< 24: "Ｆ"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_25,  //!< 25: "Ｇ"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_26,  //!< 26: "Ｈ"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_27,  //!< 27: "Ｊ"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_28,  //!< 28: "Ｋ"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_29,  //!< 29: "Ｌ"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_30,  //!< 30: "："
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_31,  //!< 31: "「"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_32,  //!< 32: "」"

  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_33,  //!< 33: "Ｚ"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_34,  //!< 34: "Ｘ"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_35,  //!< 35: "Ｃ"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_36,  //!< 36: "Ｖ"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_37,  //!< 37: "Ｂ"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_38,  //!< 38: "Ｎ"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_39,  //!< 39: "Ｍ"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_40,  //!< 40: "、"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_41,  //!< 41: "。"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_42,  //!< 42: "／"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_43,  //!< 43: "！"
  //  PANENAME_NAMEINPUT2_LOWER_PANE_WORD_44,  //!< 44: "？"

  //  PANENAME_NAMEINPUT2_LOWER_PANE_DELETE,   //!< 45: ×ボタン
  //  PANENAME_NAMEINPUT2_LOWER_PANE_END,      //!< 46: ENTER
  //  PANENAME_NAMEINPUT2_LOWER_PANE_BUTTON_00,//!< 47: かな
  //  PANENAME_NAMEINPUT2_LOWER_PANE_BUTTON_01,//!< 48: カナ
  //  PANENAME_NAMEINPUT2_LOWER_PANE_BUTTON_02,//!< 49: ABC
  //  PANENAME_NAMEINPUT2_LOWER_PANE_SPACE,    //!< 50: SPACE
  //  PANENAME_NAMEINPUT2_LOWER_PANE_SHIFT,    //!< 51: SHIFT
  //};

  //app::util::LytUtil::ShareAnimeDesc * desc = GFL_NEW_LOW_ARRAY( heap->GetDeviceHeap()) app::util::LytUtil::ShareAnimeDesc[ANMID_KEYMAP_B_SHARE_MAX];

  //for( int i=0; i<ANMID_KEYMAP_B_SHARE_PANE_MAX; i++ )
  //{
  //  for( int j=0; j<BTNANM_MAX+1; j++ )
  //  {
  //    u32  index = i * (BTNANM_MAX+1) + j;
  //    app::util::LytUtil::ShareAnimeDesc * buf = &desc[index];
  //    buf->start_anime_index = index + ANMID_KEYMAP_B_SHARE;                  //!< アニメ先頭インデックス
  //    buf->max_share_num     = 1;                                             //!< 最大共有数
  //    buf->anime_data        = KeyMap01_LowerAnmIndex[j];                     //!< アニメデータインデックス
  //    buf->share_group_index = PANENAME_NAMEINPUT2_LOWER_GROUP_BUTTON_ANIME;  //!< シェア対象のグループ
  //    buf->share_pane_index  = pane[i];                                       //!< シェア対象のペイン
  //  }
  //}

  //m_pG2D->CreateShareAnime( LAYOUT_ID_LOWER_KEYBOARD, desc, ANMID_KEYMAP_B_SHARE_MAX );

  //GFL_DELETE_ARRAY desc;
}

//------------------------------------------------------------------------------
/**
 * @brief   タイマーアイコン作成
 *
 * @param   heap  ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::CreateTimeIcon( app::util::Heap * heap, bool isFieldSubProc )
{
  static const u32 lytID = LAYOUT_ID_UPPER;
  const GameSys::FrameMode frameMode = (!isFieldSubProc) ? GameSys::FRAMEMODE_DEFAULT : GameSys::FRAMEMODE_30;

  gfl2::lyt::LytWk * lytwk = m_pG2D->GetLayoutWork( lytID );

  App::Tool::TimeIcon::StartAnime( lytwk, LA_STRINPUT_STRINPUT_DSC_UPP_000__TIMERICON_KEEP, frameMode );
}

//------------------------------------------------------------------------------
/**
 * @brief   タイマーアイコン削除
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::DeleteTimeIcon( void )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインのセット
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void GraphicWork::SetLayoutPane( void )
{
  gfl2::lyt::LytWk* pLytWkTbl[LAYOUT_ID_MAX];
  gfl2::lyt::LytMultiResID resID = m_pG2D->GetLayoutResourceID(0);

  for( u32 i=0; i<LAYOUT_ID_MAX; ++i )
  {
    pLytWkTbl[i] = m_pG2D->GetLayoutWork(i);
  }

  gfl2::lyt::LytWk* pLytWk;

  // 上画面
  {
    pLytWk = pLytWkTbl[LAYOUT_ID_UPPER];

    gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( PANENAME_STRINPUT_DSC_UPP_000_PANE_MESSAGE );
    m_pUppMsgTextBox00 = pLytWk->GetTextBoxPane( pParts, PANENAME_COMMON_MSG_UPP_000_PANE_TEXTBOX_00, &resID );
    m_pG2D->SetPaneVisible( m_pUppMsgTextBox00, true );
  }

  // 下画面(BGR)
  {
    pLytWk = pLytWkTbl[LAYOUT_ID_LOWER_CHANGE];

    m_pLowBgrChg00 = pLytWk->GetPartsPane( PANENAME_STRINPUT_BGR_LOW_000_PANE_CHANGE_00 );
    m_pLowBgrChg01 = pLytWk->GetPartsPane( PANENAME_STRINPUT_BGR_LOW_000_PANE_CHANGE_01 );

    m_pLowBgrChg00Bound = pLytWk->GetPane( m_pLowBgrChg00, PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00, &resID );
    m_pLowBgrChg01Bound = pLytWk->GetPane( m_pLowBgrChg01, PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00, &resID );
  }
}


//----------------------------------------------------------------------------
/**
 * @brief   グラフィックフォントのセット
 *
 * @return  none
 */
//----------------------------------------------------------------------------
void GraphicWork::SetTextureGraphicFont( void )
{
  static const u32 c_LytWkIDTbl[] = {
    LAYOUT_ID_LOWER,
    LAYOUT_ID_LOWER,
    LAYOUT_ID_LOWER,
    LAYOUT_ID_LOWER_KEYBOARD,
    LAYOUT_ID_LOWER_KEYBOARD,
  };
  static const u32 c_BtnPartsTbl[] = {
    PANENAME_STRINPUT_MIN_LOW_000_PANE_BUTTON_00,
    PANENAME_STRINPUT_MIN_LOW_000_PANE_BUTTON_01,
    PANENAME_STRINPUT_MIN_LOW_000_PANE_BUTTON_03,
    PANENAME_STRINPUT_MIN_LOW_001_PANE_BUTTON_00,
    PANENAME_STRINPUT_MIN_LOW_001_PANE_BUTTON_01,
  };
  static const u32 c_BtnTexTbl[] = {
    LYTRES_STRINPUTGRPFONT_MODE01_BFLIM,
    LYTRES_STRINPUTGRPFONT_MODE02_BFLIM,
    LYTRES_STRINPUTGRPFONT_MODE04_BFLIM,
    LYTRES_STRINPUTGRPFONT_MODE01_BFLIM,
    LYTRES_STRINPUTGRPFONT_MODE02_BFLIM,
  };

  for( u32 i=0; i<GFL_NELEMS(c_LytWkIDTbl); i++ )
  {
    ReplaceTextureGraphicFont( c_LytWkIDTbl[i], c_BtnPartsTbl[i], c_BtnTexTbl[i] );
  }
}

//----------------------------------------------------------------------------
/**
 * @brief   グラフィックフォントのセット：韓国語版
 *
 * @return  none
 *
 * @fix     NMCat[914]
 */
//----------------------------------------------------------------------------
void GraphicWork::SetTextureGraphicFont_KOR( void )
{
  static const u32 c_LytWkIDTbl_KOR[] = {
    LAYOUT_ID_LOWER,
    LAYOUT_ID_LOWER_KEYBOARD,
  };
  static const u32 c_BtnPartsTbl_KOR[] = {
    PANENAME_STRINPUT_MIN_LOW_000_PANE_BUTTON_02,
    PANENAME_STRINPUT_MIN_LOW_001_PANE_BUTTON_02,
  };
  static const u32 c_BtnTexTbl_KOR[] = {
    LYTRES_STRINPUTGRPFONT_MODE02_BFLIM,
    LYTRES_STRINPUTGRPFONT_MODE02_BFLIM,
  };

  for( u32 i=0; i<GFL_NELEMS(c_LytWkIDTbl_KOR); i++ )
  {
    ReplaceTextureGraphicFont( c_LytWkIDTbl_KOR[i], c_BtnPartsTbl_KOR[i], c_BtnTexTbl_KOR[i] );
  }
}

//----------------------------------------------------------------------------
/**
 * @brief   グラフィックフォントのセット
 *
 * @return  none
 */
//----------------------------------------------------------------------------
void GraphicWork::ReplaceTextureGraphicFont( const u32 lytID, const u32 partsID, const u32 texID )
{
  gfl2::lyt::LytMultiResID resID = m_pG2D->GetLayoutResourceID( 1 );

  // テクスチャの取得
  const gfl2::lyt::LytResTextureInfo* texInfo =
    m_pG2D->GetLayoutSystem()->GetResource( &resID )->GetTexture( texID, resID.GetMultiID() );

  gfl2::lyt::LytWk* lytwk = m_pG2D->GetLayoutWork( lytID );
  {
    // 差し替え先のペインの取得
    gfl2::lyt::LytParts* parts = lytwk->GetPartsPane( partsID );
    gfl2::lyt::LytPicture* pic = lytwk->GetPicturePane( parts, PANENAME_STRINPUT_BTN_LOW_001_PANE_MODE01, &resID );

    // 差し替え
    app::tool::LytTexReplaceSystem::ReplaceTextureStatic( texInfo, pic, 0, 0 );
  }
}


//-----------------------------------------------------------------------------
/**
 * @brief   半角大文字を小文字に変換
 *
 * @param   code  文字コード
 *
 * @return  変換後の文字コード
 */
//-----------------------------------------------------------------------------
gfl2::str::STRCODE GraphicWork::ToLower( gfl2::str::STRCODE code )
{
  static const gfl2::str::STRCODE tbl[][2] =
  {
    { 0x0041, 0x0061 }, // A  a
    { 0x0042, 0x0062 }, // B  b
    { 0x0043, 0x0063 }, // C  c
    { 0x0044, 0x0064 }, // D  d
    { 0x0045, 0x0065 }, // E  e
    { 0x0046, 0x0066 }, // F  f
    { 0x0047, 0x0067 }, // G  g
    { 0x0048, 0x0068 }, // H  h
    { 0x0049, 0x0069 }, // I  i
    { 0x004A, 0x006A }, // J  j
    { 0x004B, 0x006B }, // K  k
    { 0x004C, 0x006C }, // L  l
    { 0x004D, 0x006D }, // M  m
    { 0x004E, 0x006E }, // N  n
    { 0x004F, 0x006F }, // O  o
    { 0x0050, 0x0070 }, // P  p
    { 0x0051, 0x0071 }, // Q  q
    { 0x0052, 0x0072 }, // R  r
    { 0x0053, 0x0073 }, // S  s
    { 0x0054, 0x0074 }, // T  t
    { 0x0055, 0x0075 }, // U  u
    { 0x0056, 0x0076 }, // V  v
    { 0x0057, 0x0077 }, // W  w
    { 0x0058, 0x0078 }, // X  x
    { 0x0059, 0x0079 }, // Y  y
    { 0x005A, 0x007A }, // Z  z
    { 0x00C9, 0x00E9 }, // É  é
    { 0x00C7, 0x00E7 }, // Ç  ç
    { 0x00C8, 0x00E8 }, // È  è
    { 0x00D4, 0x00F4 }, // Ô  ô
    { 0x00CF, 0x00EF }, // Ï  ï
    { 0x00C2, 0x00E2 }, // Â  â
    { 0x00C4, 0x00E4 }, // Ä  ä
    { 0x00DC, 0x00FC }, // Ü  ü
    { 0x0152, 0x0153 }, // Œ  œ
//    { 0x1E9E, 0x00DF }, // ẞ  ß  ※入力なし
  };

  for( int i=0; i<GFL_NELEMS(tbl); i++ )
  {
    if( tbl[i][0] == code )
    {
      return tbl[i][1];
    }
  }

  return code;
}

//-----------------------------------------------------------------------------
/**
 * @brief   文字列を小文字に変換して比較
 *
 * @param   str1  文字列１
 * @param   str2  文字列２
 *
 * @retval  true  = 一致
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool GraphicWork::ToLowerComp( const gfl2::str::STRCODE * str1, const gfl2::str::STRCODE * str2 )
{
  bool result = false;
  u32 i = 0;

  for ever{
    if( str1[i] == gfl2::str::EOM_CODE )
    {
      if( str2[i] == gfl2::str::EOM_CODE )
      {
        result = true;
        GFL_PRINT( "★文字列一致 [%ls] = [%ls]\n", str1, str2 );
      }
      break;
    }

    if( ToLower(str1[i]) != ToLower(str2[i]) )
    {
      break;
    }

    i++;
  }

  return result;
}


GFL_NAMESPACE_END( StrInput )
GFL_NAMESPACE_END( App )

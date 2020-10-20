//==============================================================================
/**
 @file    ZukanViewInfo.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の情報画面のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <ui/include/gfl2_UI.h>
#include <Layout/include/gfl2_Layout.h>

// nijiのインクルード
#include "AppLib/include/Tool/app_tool_ButtonManager.h"

#include <Sound/include/Sound.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/zukan/info.h>
#include <niji_conv_header/app/zukan/info_pane.h>
#include <niji_conv_header/app/zukan/info_anim_list.h>

// 図鑑のインクルード
#include "ZukanType.h"
#include "ZukanViewInfo.h"

#include "ZukanViewInfoDefine.h"



GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
//
//  ZukanViewInfo
//
//==============================================================================


#define PL(lang) POKEMON_LANG_##lang
// git_program/niji_project/prog/System/include/PokemonVersion.h  POKEMON_LANG_JAPANなど
const u8 ZukanViewInfo::ZUKAN_LANG_ORDER_TABLE[ZukanType::ZUKAN_LANG_NUM][ZukanType::ZUKAN_LANG_NUM] =  // [i][0]のときiの言語が表示される。
{
  { PL(JAPAN  ), PL(ENGLISH), PL(FRANCE ), PL(GERMANY), PL(ITALY  ), PL(KOREA  ), PL(SPAIN  ), PL(CHINA  ), PL(TAIWAN ) },  // ZUKAN_LANG_JAPAN  
  { PL(ENGLISH), PL(FRANCE ), PL(GERMANY), PL(ITALY  ), PL(JAPAN  ), PL(KOREA  ), PL(SPAIN  ), PL(CHINA  ), PL(TAIWAN ) },  // ZUKAN_LANG_ENGLISH
  { PL(FRANCE ), PL(ENGLISH), PL(GERMANY), PL(SPAIN  ), PL(ITALY  ), PL(JAPAN  ), PL(KOREA  ), PL(CHINA  ), PL(TAIWAN ) },  // ZUKAN_LANG_FRANCE 
  { PL(ITALY  ), PL(ENGLISH), PL(FRANCE ), PL(GERMANY), PL(SPAIN  ), PL(JAPAN  ), PL(KOREA  ), PL(CHINA  ), PL(TAIWAN ) },  // ZUKAN_LANG_ITALY  
  { PL(GERMANY), PL(ENGLISH), PL(FRANCE ), PL(ITALY  ), PL(SPAIN  ), PL(CHINA  ), PL(TAIWAN ), PL(JAPAN  ), PL(KOREA  ) },  // ZUKAN_LANG_GERMANY
  { PL(SPAIN  ), PL(ENGLISH), PL(FRANCE ), PL(GERMANY), PL(ITALY  ), PL(JAPAN  ), PL(KOREA  ), PL(CHINA  ), PL(TAIWAN ) },  // ZUKAN_LANG_SPAIN  
  { PL(KOREA  ), PL(ENGLISH), PL(FRANCE ), PL(GERMANY), PL(ITALY  ), PL(JAPAN  ), PL(SPAIN  ), PL(CHINA  ), PL(TAIWAN ) },  // ZUKAN_LANG_KOREA  
  { PL(CHINA  ), PL(TAIWAN ), PL(ENGLISH), PL(FRANCE ), PL(GERMANY), PL(ITALY  ), PL(JAPAN  ), PL(KOREA  ), PL(SPAIN  ) },  // ZUKAN_LANG_CHINA  
  { PL(TAIWAN ), PL(CHINA  ), PL(ENGLISH), PL(FRANCE ), PL(GERMANY), PL(ITALY  ), PL(JAPAN  ), PL(KOREA  ), PL(SPAIN  ) },  // ZUKAN_LANG_TAIWAN 
};
#undef PL




#define APP_ZUKAN_INFO_EXPLAIN_LANG_BUTTON(buttonId,paneName,animName) \
  {                                                                 \
    buttonId,                                                       \
    PANENAME_ZUKAN_MIN_LOW_002_PANE_##paneName,                     \
    PANENAME_ZUKAN_BTN_LOW_003_PANE_ALL,                            \
    PANENAME_ZUKAN_BTN_LOW_003_PANE_BOUND_00,                       \
    gfl2::lyt::LYTSYS_PANEINDEX_ERROR,                              \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_TOUCH,                  \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_RELEASE,                \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_CANSEL,                 \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_TOUCH_RELEASE,          \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_ACTIVE,                 \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_PASSIVE,                \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_SELECT_##animName,      \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_UNSELECT_##animName,    \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_IMPOSSIBLE_##animName,  \
    false,                                                          \
    gfl2::ui::BUTTON_INVALID,                                       \
    app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER,                \
    SEQ_SE_SELECT1                                                  \
  },


#define APP_ZUKAN_INFO_UPPER_BUTTON(buttonId,paneName,partsName,keyName,seId) \
  {                                                         \
    buttonId,                                               \
    PANENAME_ZUKAN_OTR_UPP_000_PANE_##paneName,             \
    PANENAME_##partsName##_PANE_MAIN,                       \
    PANENAME_##partsName##_PANE_BOUND_00,                   \
    gfl2::lyt::LYTSYS_PANEINDEX_ERROR,                      \
    LA_INFO_ZUKAN_OTR_UPP_000__##paneName##_TOUCH,          \
    LA_INFO_ZUKAN_OTR_UPP_000__##paneName##_RELEASE,        \
    LA_INFO_ZUKAN_OTR_UPP_000__##paneName##_CANSEL,         \
    LA_INFO_ZUKAN_OTR_UPP_000__##paneName##_TOUCH_RELEASE,  \
    LA_INFO_ZUKAN_OTR_UPP_000__##paneName##_ACTIVE,         \
    LA_INFO_ZUKAN_OTR_UPP_000__##paneName##_PASSIVE,        \
    app::tool::ButtonManager::ANIMATION_NULL,               \
    app::tool::ButtonManager::ANIMATION_NULL,               \
    app::tool::ButtonManager::ANIMATION_NULL,               \
    true,                                                   \
    gfl2::ui::keyName,                                      \
    app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER,        \
    seId                                                    \
  }, 


const ZukanType::ButtonConstInfo ZukanViewInfo::BUTTON_CONST_INFO_FOR_EXPLAIN[BUTTON_ID_EXPLAIN_NUM] =
{
  ///////////////////////////
  // 情報画面の説明ページ
  ///////////////////////////
  APP_ZUKAN_INFO_EXPLAIN_LANG_BUTTON(BUTTON_ID_EXPLAIN_LANG00, TAB_00, JPN)
  APP_ZUKAN_INFO_EXPLAIN_LANG_BUTTON(BUTTON_ID_EXPLAIN_LANG01, TAB_01, JPN)
  APP_ZUKAN_INFO_EXPLAIN_LANG_BUTTON(BUTTON_ID_EXPLAIN_LANG02, TAB_02, JPN)
  APP_ZUKAN_INFO_EXPLAIN_LANG_BUTTON(BUTTON_ID_EXPLAIN_LANG03, TAB_03, JPN)
  APP_ZUKAN_INFO_EXPLAIN_LANG_BUTTON(BUTTON_ID_EXPLAIN_LANG04, TAB_04, JPN)
  APP_ZUKAN_INFO_EXPLAIN_LANG_BUTTON(BUTTON_ID_EXPLAIN_LANG05, TAB_05, JPN)
  APP_ZUKAN_INFO_EXPLAIN_LANG_BUTTON(BUTTON_ID_EXPLAIN_LANG06, TAB_06, JPN)
  APP_ZUKAN_INFO_EXPLAIN_LANG_BUTTON(BUTTON_ID_EXPLAIN_LANG07, TAB_07, JPN)
  APP_ZUKAN_INFO_EXPLAIN_LANG_BUTTON(BUTTON_ID_EXPLAIN_LANG08, TAB_08, JPN)
  APP_ZUKAN_INFO_MARU_BUTTON(BUTTON_ID_EXPLAIN_PREV_MONS, INFO_BUTTON_UP,   BUTTON_UP,   REPEAT, SEQ_SE_SELECT1)
  APP_ZUKAN_INFO_MARU_BUTTON(BUTTON_ID_EXPLAIN_NEXT_MONS, INFO_BUTTON_DOWN, BUTTON_DOWN, REPEAT, SEQ_SE_SELECT1)
  APP_ZUKAN_INFO_PAGE_BUTTON(BUTTON_ID_EXPLAIN_MAP,       BUTTON_Y4_INFO, COMMON_BTN_LOW_013, BUTTON_A, SEQ_SE_DECIDE1)
  APP_ZUKAN_INFO_PAGE_BUTTON(BUTTON_ID_EXPLAIN_FORMVOICE, BUTTON_A8,      COMMON_BTN_LOW_019, BUTTON_Y, SEQ_SE_DECIDE1)
  APP_ZUKAN_INFO_PAGE_BUTTON(BUTTON_ID_EXPLAIN_BACK,      BTN_BACK,       COMMON_BTN_LOW_000, BUTTON_B, SEQ_SE_CANCEL1)
  //APP_ZUKAN_INFO_UPPER_BUTTON(BUTTON_ID_EXPLAIN_QR,       BUTTON_X,       COMMON_BTN_LOW_013, BUTTON_X, SEQ_SE_DECIDE1)
};

const ZukanType::ButtonConstInfo ZukanViewInfo::BUTTON_CONST_INFO_FOR_FORMVOICE[BUTTON_ID_FORMVOICE_NUM] =
{
  ///////////////////////////////
  // 情報画面の姿＆鳴き声ページ
  ///////////////////////////////
  APP_ZUKAN_INFO_MARU_BUTTON(BUTTON_ID_FORMVOICE_PREV_MONS, TOP_ICON,   BUTTON_UP,    REPEAT, SEQ_SE_SELECT1)
  APP_ZUKAN_INFO_MARU_BUTTON(BUTTON_ID_FORMVOICE_NEXT_MONS, UNDER_ICON, BUTTON_DOWN,  REPEAT, SEQ_SE_SELECT1)
  APP_ZUKAN_INFO_MARU_BUTTON(BUTTON_ID_FORMVOICE_PREV_FORM, LEFT_ICON,  BUTTON_LEFT,  REPEAT, SEQ_SE_SELECT1)
  APP_ZUKAN_INFO_MARU_BUTTON(BUTTON_ID_FORMVOICE_NEXT_FORM, RIGHT_ICON, BUTTON_RIGHT, REPEAT, SEQ_SE_SELECT1) 
  APP_ZUKAN_INFO_PAGE_BUTTON(BUTTON_ID_FORMVOICE_BACK,   BTN_BACK_00,     COMMON_BTN_LOW_000, BUTTON_B, SEQ_SE_CANCEL1)
  APP_ZUKAN_INFO_PAGE_BUTTON(BUTTON_ID_FORMVOICE_VOICE,  BUTTON_Y4_CRIES, COMMON_BTN_LOW_013, BUTTON_Y, app::tool::ButtonManager::SE_NULL)
  APP_ZUKAN_INFO_PAGE_BUTTON(BUTTON_ID_FORMVOICE_MOTION, BUTTON_X4,       COMMON_BTN_LOW_013, BUTTON_X, SEQ_SE_DECIDE1)
  APP_ZUKAN_INFO_MARU_BUTTON(BUTTON_ID_FORMVOICE_PAUSE, STOP_ICON_00, BUTTON_START|gfl2::ui::BUTTON_A, TRIGGER, SEQ_SE_SELECT1)  // 一時停止音が欲しい。ピッって感じの軽い音が。SEQ_SE_MESSAGEから変更。
};

const ZukanType::ButtonConstInfo ZukanViewInfo::BUTTON_CONST_INFO_FOR_REGISTER[BUTTON_ID_REGISTER_NUM] =
{
  ///////////////////////////////
  // 登録画面
  ///////////////////////////////
  APP_ZUKAN_INFO_PAGE_BUTTON(BUTTON_ID_REGISTER_OK,     BUTTON_A3,       COMMON_BTN_LOW_018, BUTTON_A, SEQ_SE_DECIDE1)
  APP_ZUKAN_INFO_PAGE_BUTTON(BUTTON_ID_REGISTER_VOICE,  BUTTON_Y4_ENTRY, COMMON_BTN_LOW_013, BUTTON_Y, app::tool::ButtonManager::SE_NULL)
  APP_ZUKAN_INFO_PAGE_BUTTON(BUTTON_ID_REGISTER_MOTION, BUTTON_X4_00,    COMMON_BTN_LOW_013, BUTTON_X, SEQ_SE_DECIDE1)
  APP_ZUKAN_INFO_MARU_BUTTON(BUTTON_ID_REGISTER_PAUSE, STOP_ICON, BUTTON_START, TRIGGER, SEQ_SE_SELECT1)  // 一時停止音が欲しい。ピッって感じの軽い音が。SEQ_SE_MESSAGEから変更。
};

const ZukanType::ScrollBarConstInfo ZukanViewInfo::SCROLL_BAR_CONST_INFO =
{
  PANENAME_ZUKAN_MIN_LOW_002_PANE_SCROLL_S,
  PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER,
  PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP,
  PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM,
};

#undef APP_ZUKAN_INFO_EXPLAIN_LANG_BUTTON

  
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


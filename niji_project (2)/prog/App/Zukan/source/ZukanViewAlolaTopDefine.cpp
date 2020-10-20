//==============================================================================
/**
 @file    ZukanViewAlolaTop.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑のアローラ図鑑トップ画面のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <ui/include/gfl2_UI.h>
#include <Layout/include/gfl2_Layout.h>

// nijiのインクルード
#include "AppLib/include/Tool/app_tool_ButtonManager.h"

#include <Sound/include/Sound.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/zukan/alola_top.h>
#include <niji_conv_header/app/zukan/alola_top_pane.h>
#include <niji_conv_header/app/zukan/alola_top_anim_list.h>

// 図鑑のインクルード
#include "ZukanType.h"
#include "ZukanViewAlolaTop.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
//
//  ZukanViewAlolaTop
//
//==============================================================================


#define APP_ZUKAN_ALOLA_TOP_PANE_LIST_ELEM_FOR_ID(paneName) \
  {                                                             \
    PANENAME_ZUKAN_MIN_LOW_000_PANE_##paneName,                 \
    PANENAME_ZUKAN_BTN_LOW_000_PANE_TEXTBOX_00,                 \
    PANENAME_ZUKAN_BTN_LOW_000_PANE_BOUND_00,                   \
    PANENAME_ZUKAN_BTN_LOW_000_PANE_NULL_CUR,                   \
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_000__##paneName##_TOUCH,         \
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_000__##paneName##_RELEASE,       \
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_000__##paneName##_CANSEL,        \
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_000__##paneName##_TOUCH_RELEASE, \
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_000__##paneName##_STATUS,        \
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_000__##paneName##_IMPOSSIBLE,    \
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_000__##paneName##_ACTIVE,        \
  },

#define APP_ZUKAN_ALOLA_TOP_PANE_LIST_ELEM_FOR_SPEC(paneName) \
  {                                                             \
    PANENAME_ZUKAN_MIN_LOW_004_PANE_##paneName,                 \
    PANENAME_ZUKAN_BTN_LOW_006_PANE_TEXTBOX_00,                 \
    PANENAME_ZUKAN_BTN_LOW_006_PANE_BOUND_00,                   \
    PANENAME_ZUKAN_BTN_LOW_006_PANE_NULL_CUR,                   \
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_004__##paneName##_TOUCH,         \
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_004__##paneName##_RELEASE,       \
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_004__##paneName##_CANSEL,        \
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_004__##paneName##_TOUCH_RELEASE, \
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_004__##paneName##_STATUS,        \
    app::tool::ButtonManager::ANIMATION_NULL,                   \
    app::tool::ButtonManager::ANIMATION_NULL,                   \
  },


const ZukanType::ButtonConstInfo ZukanViewAlolaTop::BUTTON_CONST_INFO_FOR_ID[BUTTON_ID_NUM] =
{
  {
    BUTTON_ID_SEARCH,
    PANENAME_ZUKAN_MIN_LOW_000_PANE_BTN_EXAMINE,
    PANENAME_GTS_BTN_LOW_000_PANE_MAIN,
    PANENAME_GTS_BTN_LOW_000_PANE_BOUND_00,
    gfl2::lyt::LYTSYS_PANEINDEX_ERROR,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_000__BTN_EXAMINE_TOUCH,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_000__BTN_EXAMINE_RELEASE,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_000__BTN_EXAMINE_CANSEL,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_000__BTN_EXAMINE_TOUCH_RELEASE,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_000__BTN_EXAMINE_ACTIVE,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_000__BTN_EXAMINE_PASSIVE,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_000__BTN_EXAMINE_SEARCH_OFF,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_000__BTN_EXAMINE_SEARCH_ON,
    app::tool::ButtonManager::ANIMATION_NULL,
    true,
    gfl2::ui::BUTTON_Y,
    app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER,
    SEQ_SE_DECIDE1
  }, 
  {
    BUTTON_ID_BACK,
    PANENAME_ZUKAN_MIN_LOW_000_PANE_BTN_BACK,
    PANENAME_COMMON_BTN_LOW_000_PANE_MAIN,
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
    gfl2::lyt::LYTSYS_PANEINDEX_ERROR,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_000__BTN_BACK_TOUCH,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_000__BTN_BACK_RELEASE,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_000__BTN_BACK_CANSEL,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_000__BTN_BACK_TOUCH_RELEASE,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_000__BTN_BACK_ACTIVE,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_000__BTN_BACK_PASSIVE,
    app::tool::ButtonManager::ANIMATION_NULL,
    app::tool::ButtonManager::ANIMATION_NULL,
    app::tool::ButtonManager::ANIMATION_NULL,
    true,
    gfl2::ui::BUTTON_B,
    app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER,
    SEQ_SE_CANCEL1
  }, 
};


const ZukanType::ButtonConstInfo ZukanViewAlolaTop::BUTTON_CONST_INFO_FOR_SPEC[BUTTON_ID_NUM] =
{
  {
    BUTTON_ID_SEARCH,
    PANENAME_ZUKAN_MIN_LOW_004_PANE_BTN_EXAMINE,
    PANENAME_GTS_BTN_LOW_000_PANE_MAIN,
    PANENAME_GTS_BTN_LOW_000_PANE_BOUND_00,
    gfl2::lyt::LYTSYS_PANEINDEX_ERROR,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_004__BTN_EXAMINE_TOUCH,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_004__BTN_EXAMINE_RELEASE,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_004__BTN_EXAMINE_CANSEL,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_004__BTN_EXAMINE_TOUCH_RELEASE,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_004__BTN_EXAMINE_ACTIVE,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_004__BTN_EXAMINE_PASSIVE,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_004__BTN_EXAMINE_SEARCH_OFF,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_004__BTN_EXAMINE_SEARCH_ON,
    app::tool::ButtonManager::ANIMATION_NULL,
    true,
    gfl2::ui::BUTTON_Y,
    app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER,
    SEQ_SE_DECIDE1
  }, 
  {
    BUTTON_ID_BACK,
    PANENAME_ZUKAN_MIN_LOW_004_PANE_BTN_BACK,
    PANENAME_COMMON_BTN_LOW_000_PANE_MAIN,
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
    gfl2::lyt::LYTSYS_PANEINDEX_ERROR,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_004__BTN_BACK_TOUCH,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_004__BTN_BACK_RELEASE,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_004__BTN_BACK_CANSEL,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_004__BTN_BACK_TOUCH_RELEASE,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_004__BTN_BACK_ACTIVE,
    LA_ALOLA_TOP_ZUKAN_MIN_LOW_004__BTN_BACK_PASSIVE,
    app::tool::ButtonManager::ANIMATION_NULL,
    app::tool::ButtonManager::ANIMATION_NULL,
    app::tool::ButtonManager::ANIMATION_NULL,
    true,
    gfl2::ui::BUTTON_B,
    app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER,
    SEQ_SE_CANCEL1
  }, 
};


const ZukanType::PaneListConstInfo ZukanViewAlolaTop::PANE_LIST_CONST_INFO_FOR_ID =
{
  6,
  101,
  218,
  39,
  -1
};

const ZukanType::PaneListConstInfo ZukanViewAlolaTop::PANE_LIST_CONST_INFO_FOR_SPEC =
{
  4,
  101,
  218,
  57,
  -1
};

const ZukanType::PaneListElemConstInfo ZukanViewAlolaTop::PANE_LIST_ELEM_CONST_INFO_FOR_ID[PANE_LIST_ELEM_NUM_FOR_ID] =
{
  APP_ZUKAN_ALOLA_TOP_PANE_LIST_ELEM_FOR_ID(LIST_00)
  APP_ZUKAN_ALOLA_TOP_PANE_LIST_ELEM_FOR_ID(LIST_01)
  APP_ZUKAN_ALOLA_TOP_PANE_LIST_ELEM_FOR_ID(LIST_02)
  APP_ZUKAN_ALOLA_TOP_PANE_LIST_ELEM_FOR_ID(LIST_03)
  APP_ZUKAN_ALOLA_TOP_PANE_LIST_ELEM_FOR_ID(LIST_04)
  APP_ZUKAN_ALOLA_TOP_PANE_LIST_ELEM_FOR_ID(LIST_05)
  APP_ZUKAN_ALOLA_TOP_PANE_LIST_ELEM_FOR_ID(LIST_06)
  APP_ZUKAN_ALOLA_TOP_PANE_LIST_ELEM_FOR_ID(LIST_07)
};

const ZukanType::PaneListElemConstInfo ZukanViewAlolaTop::PANE_LIST_ELEM_CONST_INFO_FOR_SPEC[PANE_LIST_ELEM_NUM_FOR_SPEC] =
{
  APP_ZUKAN_ALOLA_TOP_PANE_LIST_ELEM_FOR_SPEC(LIST_00)
  APP_ZUKAN_ALOLA_TOP_PANE_LIST_ELEM_FOR_SPEC(LIST_01)
  APP_ZUKAN_ALOLA_TOP_PANE_LIST_ELEM_FOR_SPEC(LIST_02)
  APP_ZUKAN_ALOLA_TOP_PANE_LIST_ELEM_FOR_SPEC(LIST_03)
  APP_ZUKAN_ALOLA_TOP_PANE_LIST_ELEM_FOR_SPEC(LIST_04)
  APP_ZUKAN_ALOLA_TOP_PANE_LIST_ELEM_FOR_SPEC(LIST_05)
};

const ZukanType::ScrollBarConstInfo ZukanViewAlolaTop::SCROLL_BAR_CONST_INFO_FOR_ID =
{
  PANENAME_ZUKAN_MIN_LOW_000_PANE_SCROLL_S,
  PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER,
  PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP,
  PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM
};

const ZukanType::ScrollBarConstInfo ZukanViewAlolaTop::SCROLL_BAR_CONST_INFO_FOR_SPEC =
{
  PANENAME_ZUKAN_MIN_LOW_004_PANE_SCROLL_S_00,
  PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER,
  PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP,
  PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM
};


#undef APP_ZUKAN_ALOLA_TOP_PANE_LIST_ELEM_FOR_ID
#undef APP_ZUKAN_ALOLA_TOP_PANE_LIST_ELEM_FOR_SPEC


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


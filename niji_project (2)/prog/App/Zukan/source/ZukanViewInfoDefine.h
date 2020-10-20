#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWINFODEFINE_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWINFODEFINE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanViewInfoDefine.h
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


//==============================================================================
//
//  ZukanViewInfo
//
//==============================================================================


#define APP_ZUKAN_INFO_MARU_BUTTON(buttonId,paneName,keyName,keyMode,seId) \
  {                                                        \
    buttonId,                                              \
    PANENAME_ZUKAN_MIN_LOW_002_PANE_##paneName,            \
    PANENAME_ZUKAN_BTN_LOW_004_PANE_MAIN,                  \
    PANENAME_ZUKAN_BTN_LOW_004_PANE_BOUND_00,              \
    gfl2::lyt::LYTSYS_PANEINDEX_ERROR,                     \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_TOUCH,         \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_RELEASE,       \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_CANSEL,        \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_TOUCH_RELEASE, \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_ACTIVE,        \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_PASSIVE,       \
    app::tool::ButtonManager::ANIMATION_NULL,              \
    app::tool::ButtonManager::ANIMATION_NULL,              \
    app::tool::ButtonManager::ANIMATION_NULL,              \
    true,                                                  \
    gfl2::ui::keyName,                                     \
    app::tool::ButtonManager::BIND_KEY_MODE_##keyMode,     \
    seId                                                   \
  }, 
 

#define APP_ZUKAN_INFO_PAGE_BUTTON(buttonId,paneName,partsName,keyName,seId) \
  {                                                         \
    buttonId,                                               \
    PANENAME_ZUKAN_MIN_LOW_002_PANE_##paneName,             \
    PANENAME_##partsName##_PANE_MAIN,                       \
    PANENAME_##partsName##_PANE_BOUND_00,                   \
    gfl2::lyt::LYTSYS_PANEINDEX_ERROR,                      \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_TOUCH,          \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_RELEASE,        \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_CANSEL,         \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_TOUCH_RELEASE,  \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_ACTIVE,         \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_PASSIVE,        \
    app::tool::ButtonManager::ANIMATION_NULL,               \
    app::tool::ButtonManager::ANIMATION_NULL,               \
    app::tool::ButtonManager::ANIMATION_NULL,               \
    true,                                                   \
    gfl2::ui::keyName,                                      \
    app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER,        \
    seId                                                    \
  }, 


#define APP_ZUKAN_INFO_PAGE_BUTTON_AB(buttonId,paneName,partsName,seId) \
  {                                                         \
    buttonId,                                               \
    PANENAME_ZUKAN_MIN_LOW_002_PANE_##paneName,             \
    PANENAME_##partsName##_PANE_MAIN,                       \
    PANENAME_##partsName##_PANE_BOUND_00,                   \
    gfl2::lyt::LYTSYS_PANEINDEX_ERROR,                      \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_TOUCH,          \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_RELEASE,        \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_CANSEL,         \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_TOUCH_RELEASE,  \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_ACTIVE,         \
    LA_INFO_ZUKAN_MIN_LOW_002__##paneName##_PASSIVE,        \
    app::tool::ButtonManager::ANIMATION_NULL,               \
    app::tool::ButtonManager::ANIMATION_NULL,               \
    app::tool::ButtonManager::ANIMATION_NULL,               \
    true,                                                   \
    gfl2::ui::BUTTON_A|gfl2::ui::BUTTON_B,                  \
    app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER,        \
    seId                                                    \
  }, 


//#undef APP_ZUKAN_INFO_PAGE_BUTTON_AB
//#undef APP_ZUKAN_INFO_PAGE_BUTTON
//#undef APP_ZUKAN_INFO_MARU_BUTTON


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWINFODEFINE_H_INCLUDED


//==============================================================================
/**
 @file    ZukanViewMap.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の分布画面のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <Layout/include/gfl2_Layout.h>

// nijiのインクルード
#include <System/include/GflUse.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <App/TownMapParts/include/TownMapPartsDraw.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Util/app_util_2d.h>

#include <Sound/include/Sound.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/zukan/map.h>
#include <niji_conv_header/app/zukan/map_pane.h>
#include <niji_conv_header/app/zukan/map_anim_list.h>

#include <niji_conv_header/message/msg_zukan.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProcParam.h>
#include "ZukanType.h"
#include "ZukanDataManager.h"
#include "ZukanTool.h"
#include "ZukanContents.h"
#include "ZukanDistributionPokeDataAccessor.h"
#include "ZukanDistributionPositionDataAccessor.h"
#include "ZukanFrameMap.h"

#include "ZukanViewMap.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
//
//  ZukanViewMap
//
//==============================================================================


#define APP_ZUKAN_MAP_NUMBER_BUTTON(buttonId,paneName) \
  {                                                         \
    buttonId,                                               \
    PANENAME_ZUKAN_MIN_LOW_005_PANE_##paneName,             \
    PANENAME_ZUKAN_BTN_LOW_011_PANE_MAIN,                   \
    PANENAME_ZUKAN_BTN_LOW_011_PANE_BOUND_00,               \
    gfl2::lyt::LYTSYS_PANEINDEX_ERROR,                      \
    LA_MAP_ZUKAN_MIN_LOW_005__##paneName##_TOUCH,        \
    LA_MAP_ZUKAN_MIN_LOW_005__##paneName##_RELEASE,      \
    LA_MAP_ZUKAN_MIN_LOW_005__##paneName##_CANSEL,       \
    LA_MAP_ZUKAN_MIN_LOW_005__##paneName##_TOUCH_RELEASE,\
    LA_MAP_ZUKAN_MIN_LOW_005__##paneName##_ACTIVE,       \
    LA_MAP_ZUKAN_MIN_LOW_005__##paneName##_PASSIVE,      \
    app::tool::ButtonManager::ANIMATION_NULL,               \
    app::tool::ButtonManager::ANIMATION_NULL,               \
    app::tool::ButtonManager::ANIMATION_NULL,               \
    false,                                                  \
    gfl2::ui::BUTTON_INVALID,                               \
    app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER,        \
    SEQ_SE_DECIDE1                                          \
  }, 

#define APP_ZUKAN_MAP_PAGE_BUTTON(buttonId,paneName,partsName,keyName,seId) \
  {                                                         \
    buttonId,                                               \
    PANENAME_ZUKAN_MIN_LOW_005_PANE_##paneName,             \
    PANENAME_##partsName##_PANE_MAIN,                       \
    PANENAME_##partsName##_PANE_BOUND_00,                   \
    gfl2::lyt::LYTSYS_PANEINDEX_ERROR,                      \
    LA_MAP_ZUKAN_MIN_LOW_005__##paneName##_TOUCH,          \
    LA_MAP_ZUKAN_MIN_LOW_005__##paneName##_RELEASE,        \
    LA_MAP_ZUKAN_MIN_LOW_005__##paneName##_CANSEL,         \
    LA_MAP_ZUKAN_MIN_LOW_005__##paneName##_TOUCH_RELEASE,  \
    LA_MAP_ZUKAN_MIN_LOW_005__##paneName##_ACTIVE,         \
    LA_MAP_ZUKAN_MIN_LOW_005__##paneName##_PASSIVE,        \
    app::tool::ButtonManager::ANIMATION_NULL,               \
    app::tool::ButtonManager::ANIMATION_NULL,               \
    app::tool::ButtonManager::ANIMATION_NULL,               \
    true,                                                   \
    gfl2::ui::keyName,                                      \
    app::tool::ButtonManager::BIND_KEY_MODE_REPEAT,         \
    seId                                                    \
  }, 


const ZukanType::ButtonConstInfo ZukanViewMap::BUTTON_CONST_INFO[BUTTON_ID_NUM] =
{
  APP_ZUKAN_MAP_NUMBER_BUTTON(BUTTON_ID_ISLAND1,PARTS_00)
  APP_ZUKAN_MAP_NUMBER_BUTTON(BUTTON_ID_ISLAND2,PARTS_01)
  APP_ZUKAN_MAP_NUMBER_BUTTON(BUTTON_ID_ISLAND3,PARTS_02)
  APP_ZUKAN_MAP_NUMBER_BUTTON(BUTTON_ID_ISLAND4,PARTS_03)
  
  APP_ZUKAN_MAP_PAGE_BUTTON(BUTTON_ID_PREV_MONS,L_BTN,ZUKAN_BTN_LOW_012,BUTTON_L,SEQ_SE_SELECT1)
  APP_ZUKAN_MAP_PAGE_BUTTON(BUTTON_ID_NEXT_MONS,R_BTN,ZUKAN_BTN_LOW_012,BUTTON_R,SEQ_SE_SELECT1)
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


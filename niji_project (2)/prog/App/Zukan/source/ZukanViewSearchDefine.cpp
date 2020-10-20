//==============================================================================
/**
 @file    ZukanViewSearch.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の検索画面のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <ui/include/gfl2_UI.h>
#include <Layout/include/gfl2_Layout.h>

// nijiのインクルード
#include "AppLib/include/Tool/app_tool_ButtonManager.h"

#include <Sound/include/Sound.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/zukan/search.h>
#include <niji_conv_header/app/zukan/search_pane.h>
#include <niji_conv_header/app/zukan/search_anim_list.h>

#include <niji_conv_header/message/msg_zukan.h>
#include <niji_conv_header/message/msg_initial.h>

// 図鑑のインクルード
#include "ZukanType.h"
#include "ZukanSearchPokeIconDisplay.h"
#include "ZukanViewSearch.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
//
//  ZukanViewSearch
//
//==============================================================================


#define APP_ZUKAN_SEARCH_PANE_LIST_ELEM(paneName,partsName) \
  {                                                               \
    PANENAME_ZUKAN_MIN_LOW_006_PANE_##paneName,                   \
    gfl2::lyt::LYTSYS_PANEINDEX_ERROR,                            \
    PANENAME_##partsName##_PANE_BOUND_00,                         \
    PANENAME_##partsName##_PANE_NULL_CUR,                         \
    LA_SEARCH_ZUKAN_MIN_LOW_006__##paneName##_TOUCH,              \
    LA_SEARCH_ZUKAN_MIN_LOW_006__##paneName##_RELEASE,            \
    LA_SEARCH_ZUKAN_MIN_LOW_006__##paneName##_CANSEL,             \
    LA_SEARCH_ZUKAN_MIN_LOW_006__##paneName##_TOUCH_RELEASE,      \
    LA_SEARCH_ZUKAN_MIN_LOW_006__##paneName##__ITEM_ITEM,         \
    LA_SEARCH_ZUKAN_MIN_LOW_006__##paneName##__ITEM_STYLEPATTERN, \
    app::tool::ButtonManager::ANIMATION_NULL,                     \
  },


#define APP_ZUKAN_SEARCH_PAGE_BUTTON(buttonId,paneName,partsName,keyName,seId) \
  {                                                         \
    buttonId,                                               \
    PANENAME_ZUKAN_MIN_LOW_006_PANE_##paneName,             \
    PANENAME_##partsName##_PANE_MAIN,                       \
    PANENAME_##partsName##_PANE_BOUND_00,                   \
    gfl2::lyt::LYTSYS_PANEINDEX_ERROR,                      \
    LA_SEARCH_ZUKAN_MIN_LOW_006__##paneName##_TOUCH,          \
    LA_SEARCH_ZUKAN_MIN_LOW_006__##paneName##_RELEASE,        \
    LA_SEARCH_ZUKAN_MIN_LOW_006__##paneName##_CANSEL,         \
    LA_SEARCH_ZUKAN_MIN_LOW_006__##paneName##_TOUCH_RELEASE,  \
    LA_SEARCH_ZUKAN_MIN_LOW_006__##paneName##_ACTIVE,         \
    LA_SEARCH_ZUKAN_MIN_LOW_006__##paneName##_PASSIVE,        \
    app::tool::ButtonManager::ANIMATION_NULL,               \
    app::tool::ButtonManager::ANIMATION_NULL,               \
    app::tool::ButtonManager::ANIMATION_NULL,               \
    true,                                                   \
    gfl2::ui::keyName,                                      \
    app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER,        \
    seId                                                    \
  }, 


const ZukanType::ButtonConstInfo ZukanViewSearch::BUTTON_CONST_INFO_FOR_LOWER_MAIN[BUTTON_ID_NUM_FOR_LOWER_MAIN] =
{
  APP_ZUKAN_SEARCH_PAGE_BUTTON(BUTTON_ID_BACK, BTN_BACK, COMMON_BTN_LOW_000, BUTTON_B, SEQ_SE_CANCEL1)
  APP_ZUKAN_SEARCH_PAGE_BUTTON(BUTTON_ID_RESET, BTN_X, COMMON_BTN_LOW_013, BUTTON_X, SEQ_SE_DECIDE1)
  APP_ZUKAN_SEARCH_PAGE_BUTTON(BUTTON_ID_DECIDE, BTN_Y, COMMON_BTN_LOW_013, BUTTON_Y, SEQ_SE_DECIDE1)
};
const ZukanType::ButtonConstInfo ZukanViewSearch::BUTTON_CONST_INFO_FOR_LOWER_SUB[BUTTON_ID_NUM_FOR_LOWER_SUB] =
{
  APP_ZUKAN_SEARCH_PAGE_BUTTON(BUTTON_ID_BACK, BTN_BACK, COMMON_BTN_LOW_000, BUTTON_B, SEQ_SE_CANCEL1)
};


const ZukanType::PaneListConstInfo     ZukanViewSearch::PANE_LIST_CONST_INFO_FOR_LOWER_MAIN =
{
  5,    // 押せるペイン数
  97,   // 押せるペインの中で1番上にあるもののY座標
  306,  // ペインの横幅
  42,   // ペインの縦幅(=ペインとペインの距離)
  -1    // 押せるペインの上に何ペインあるかを負の値で
};
const ZukanType::PaneListConstInfo     ZukanViewSearch::PANE_LIST_CONST_INFO_FOR_LOWER_SUB =
{
  5,
  90,
  190,
  42,
  -1
};


const ZukanType::PaneListElemConstInfo ZukanViewSearch::PANE_LIST_ELEM_CONST_INFO_FOR_LOWER_MAIN[ZukanViewSearch::PANE_LIST_ELEM_NUM_FOR_LOWER_MAIN] =
{
  APP_ZUKAN_SEARCH_PANE_LIST_ELEM(FRAME_00, ZUKAN_BTN_LOW_008)
  APP_ZUKAN_SEARCH_PANE_LIST_ELEM(FRAME_01, ZUKAN_BTN_LOW_008)
  APP_ZUKAN_SEARCH_PANE_LIST_ELEM(FRAME_02, ZUKAN_BTN_LOW_008)
  APP_ZUKAN_SEARCH_PANE_LIST_ELEM(FRAME_03, ZUKAN_BTN_LOW_008)
  APP_ZUKAN_SEARCH_PANE_LIST_ELEM(FRAME_04, ZUKAN_BTN_LOW_008)
  APP_ZUKAN_SEARCH_PANE_LIST_ELEM(FRAME_05, ZUKAN_BTN_LOW_008)
  APP_ZUKAN_SEARCH_PANE_LIST_ELEM(FRAME_06, ZUKAN_BTN_LOW_008)
};
const ZukanType::PaneListElemConstInfo ZukanViewSearch::PANE_LIST_ELEM_CONST_INFO_FOR_LOWER_SUB[ZukanViewSearch::PANE_LIST_ELEM_NUM_FOR_LOWER_SUB] =
{
  APP_ZUKAN_SEARCH_PANE_LIST_ELEM(FRAME_S_06, ZUKAN_BTN_LOW_010)
  APP_ZUKAN_SEARCH_PANE_LIST_ELEM(FRAME_S_00, ZUKAN_BTN_LOW_010)
  APP_ZUKAN_SEARCH_PANE_LIST_ELEM(FRAME_S_01, ZUKAN_BTN_LOW_010)
  APP_ZUKAN_SEARCH_PANE_LIST_ELEM(FRAME_S_02, ZUKAN_BTN_LOW_010)
  APP_ZUKAN_SEARCH_PANE_LIST_ELEM(FRAME_S_03, ZUKAN_BTN_LOW_010)
  APP_ZUKAN_SEARCH_PANE_LIST_ELEM(FRAME_S_04, ZUKAN_BTN_LOW_010)
  APP_ZUKAN_SEARCH_PANE_LIST_ELEM(FRAME_S_05, ZUKAN_BTN_LOW_010)
};


const ZukanType::ScrollBarConstInfo    ZukanViewSearch::SCROLL_BAR_CONST_INFO_FOR_LOWER_MAIN =
{
  PANENAME_ZUKAN_MIN_LOW_006_PANE_SCROLL_S,
  PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER,
  PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP,
  PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM
};
const ZukanType::ScrollBarConstInfo    ZukanViewSearch::SCROLL_BAR_CONST_INFO_FOR_LOWER_SUB =
{
  PANENAME_ZUKAN_MIN_LOW_006_PANE_SCROLL_S_00,
  PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER,
  PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP,
  PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM
};


#undef APP_ZUKAN_SEARCH_PAGE_BUTTON
#undef APP_ZUKAN_SEARCH_PANE_LIST_ELEM




u32 ZukanViewSearch::getSubjectMsgId(ZukanType::Subject subjectId)
{
  static const u32 SUBJECT_MSG_ID[ZukanType::SUBJECT_NUM] =
  {
    msg_zkn_search_01_00,
    msg_zkn_search_02_00,
    msg_zkn_search_03_00,
    msg_zkn_search_03_01,
    msg_zkn_search_04_00,
    msg_zkn_search_05_00,
    msg_zkn_search_06_00,
    msg_zkn_search_07_00,
  };
  if(subjectId >= ZukanType::SUBJECT_NUM)
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSRT
    subjectId = static_cast<ZukanType::Subject>(0);
  }
  return SUBJECT_MSG_ID[subjectId];
}
u32 ZukanViewSearch::getOrderMsgId(ZukanType::OrderType orderType)
{
  static const u32 ORDER_MSG_ID[ZukanType::ORDER_TYPE_NUM] =
  {
    msg_zkn_search_01_01,
    msg_zkn_search_01_02,
    msg_zkn_search_01_03,
    msg_zkn_search_01_04,
    msg_zkn_search_01_05,
    msg_zkn_search_01_06,
  };
  if(orderType >= ZukanType::ORDER_TYPE_NUM)
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSRT
    orderType = static_cast<ZukanType::OrderType>(0);
  }
  return ORDER_MSG_ID[orderType];
}
u32 ZukanViewSearch::getInitialMsgId(ZukanType::InitialType initialType, ZukanType::ZukanLang zukanLang)
{
  if(static_cast<s32>(initialType) >= static_cast<s32>(ZukanType::GetInitialTypeNum(zukanLang)))
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSRT
    initialType = static_cast<ZukanType::InitialType>(0);
  }
  return initialType;
}
u32 ZukanViewSearch::getColorMsgId(ZukanType::ColorType colorType)
{
  static const u32 COLOR_MSG_ID[ZukanType::COLOR_TYPE_NUM] =
  {
    msg_zkn_search_04_01,  // COLOR_RED   
    msg_zkn_search_04_02,  // COLOR_BLUE
    msg_zkn_search_04_03,  // COLOR_YELLOW
    msg_zkn_search_04_04,  // COLOR_GREEN
    msg_zkn_search_04_05,  // COLOR_BLACK
    msg_zkn_search_04_06,  // COLOR_BROWN
    msg_zkn_search_04_07,  // COLOR_PERPLE
    msg_zkn_search_04_08,  // COLOR_GRAY
    msg_zkn_search_04_09,  // COLOR_WHITE
    msg_zkn_search_04_10,  // COLOR_PINK  
  };
  if(colorType >= ZukanType::COLOR_TYPE_NUM)
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSRT
    colorType = static_cast<ZukanType::ColorType>(0);
  }
  return COLOR_MSG_ID[colorType];
}
u32 ZukanViewSearch::getStyleTexPatternIndex(ZukanType::StyleType styleType)
{
  static const u32 STYLE_TEX_PATTERN_INDEX[ZukanType::STYLE_TYPE_NUM] =
  {
     5 ,  // "丸型"=>0,
     9 ,  // "脚型"=>1,
    13 ,  // "魚型"=>2,
    11 ,  // "昆虫型"=>3,
     2 ,  // "四足型"=>4,
    10 ,  // "四枚羽型"=>5,
    12 ,  // "集合型"=>6,
     7 ,  // "多足型"=>7,
     3 ,  // "直立胴型"=>8,
     8 ,  // "二足尻尾型"=>9,
     4 ,  // "二足人型"=>10,
     6 ,  // "二枚羽型"=>11,
     1 ,  // "腹這脚無型"=>12,
     0 ,  // "腕型"=>13,       
  };
  if(styleType >= ZukanType::STYLE_TYPE_NUM)
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSRT
    styleType = static_cast<ZukanType::StyleType>(0);
  }
  return STYLE_TEX_PATTERN_INDEX[styleType];
}
u32 ZukanViewSearch::getFigureMsgId(ZukanType::FigureType figureType)
{
  static const u32 FIGURE_MSG_ID[ZukanType::FIGURE_TYPE_NUM] =
  {
    msg_zkn_search_06_03,
    msg_zkn_search_06_01,
    msg_zkn_search_06_02,
  };
  if(figureType >= ZukanType::FIGURE_TYPE_NUM)
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSRT
    figureType = static_cast<ZukanType::FigureType>(0);
  }
  return FIGURE_MSG_ID[figureType];
}
u32 ZukanViewSearch::getGetMsgId(ZukanType::GetType getType)
{
  static const u32 GET_MSG_ID[ZukanType::GET_TYPE_NUM] =
  {
    msg_zkn_search_07_01,
    msg_zkn_search_07_02,
  };
  if(getType >= ZukanType::GET_TYPE_NUM)
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSRT
    getType = static_cast<ZukanType::GetType>(0);
  }
  return GET_MSG_ID[getType];
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


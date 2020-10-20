//==============================================================================
/**
 @file    ZukanSearchPokeIconDisplayDefine.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.10.08
 @brief   図鑑の検索のポケアイコン表示クラス
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




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


#define APP_ZUKAN_SEARCH_POKE_ICON_PARTS(paneName) \
  {                                                         \
    PANENAME_ZUKAN_SRC_UPP_000_PANE_##paneName,             \
    PANENAME_ZUKAN_BTN_LOW_005_PANE_POKE_ICON,              \
    PANENAME_ZUKAN_BTN_LOW_005_PANE_ICON_BALL_00,           \
    PANENAME_ZUKAN_BTN_LOW_005_PANE_KIRAKIRA,               \
    LA_SEARCH_ZUKAN_SRC_UPP_000__##paneName##_STATUS,       \
  },

#define APP_ZUKAN_SEARCH_POKE_ICON_PARTS_ROW(panePrefixName) \
  {                                                         \
    APP_ZUKAN_SEARCH_POKE_ICON_PARTS(panePrefixName##_00)   \
    APP_ZUKAN_SEARCH_POKE_ICON_PARTS(panePrefixName##_01)   \
    APP_ZUKAN_SEARCH_POKE_ICON_PARTS(panePrefixName##_02)   \
    APP_ZUKAN_SEARCH_POKE_ICON_PARTS(panePrefixName##_03)   \
    APP_ZUKAN_SEARCH_POKE_ICON_PARTS(panePrefixName##_04)   \
    APP_ZUKAN_SEARCH_POKE_ICON_PARTS(panePrefixName##_05)   \
    APP_ZUKAN_SEARCH_POKE_ICON_PARTS(panePrefixName##_06)   \
    APP_ZUKAN_SEARCH_POKE_ICON_PARTS(panePrefixName##_07)   \
    APP_ZUKAN_SEARCH_POKE_ICON_PARTS(panePrefixName##_08)   \
    APP_ZUKAN_SEARCH_POKE_ICON_PARTS(panePrefixName##_09)   \
    APP_ZUKAN_SEARCH_POKE_ICON_PARTS(panePrefixName##_10)   \
    APP_ZUKAN_SEARCH_POKE_ICON_PARTS(panePrefixName##_11)   \
    APP_ZUKAN_SEARCH_POKE_ICON_PARTS(panePrefixName##_12)   \
  },


const ZukanSearchPokeIconDisplay::PokeIconPartsConstInfo  ZukanSearchPokeIconDisplay::POKE_ICON_PARTS_CONST_INFO[POKE_ICON_Y_NUM][POKE_ICON_X_NUM] =
{
  APP_ZUKAN_SEARCH_POKE_ICON_PARTS_ROW(A)
  APP_ZUKAN_SEARCH_POKE_ICON_PARTS_ROW(B)
  APP_ZUKAN_SEARCH_POKE_ICON_PARTS_ROW(C)
  APP_ZUKAN_SEARCH_POKE_ICON_PARTS_ROW(D)
  APP_ZUKAN_SEARCH_POKE_ICON_PARTS_ROW(E)
  APP_ZUKAN_SEARCH_POKE_ICON_PARTS_ROW(F)
  APP_ZUKAN_SEARCH_POKE_ICON_PARTS_ROW(G)
  APP_ZUKAN_SEARCH_POKE_ICON_PARTS_ROW(H)
  APP_ZUKAN_SEARCH_POKE_ICON_PARTS_ROW(I)
  APP_ZUKAN_SEARCH_POKE_ICON_PARTS_ROW(J)
  APP_ZUKAN_SEARCH_POKE_ICON_PARTS_ROW(K)
  APP_ZUKAN_SEARCH_POKE_ICON_PARTS_ROW(L)
  APP_ZUKAN_SEARCH_POKE_ICON_PARTS_ROW(M)
  APP_ZUKAN_SEARCH_POKE_ICON_PARTS_ROW(N)
  APP_ZUKAN_SEARCH_POKE_ICON_PARTS_ROW(O)
};


#undef APP_ZUKAN_SEARCH_POKE_ICON_PARTS_ROW
#undef APP_ZUKAN_SEARCH_POKE_ICON_PARTS


const u32 ZukanSearchPokeIconDisplay::POKE_ICON_PARENT_PANE_ID[POKE_ICON_Y_NUM] =
{
  PANENAME_ZUKAN_SRC_UPP_000_PANE_A,
  PANENAME_ZUKAN_SRC_UPP_000_PANE_B,
  PANENAME_ZUKAN_SRC_UPP_000_PANE_C,
  PANENAME_ZUKAN_SRC_UPP_000_PANE_D,
  PANENAME_ZUKAN_SRC_UPP_000_PANE_E,
  PANENAME_ZUKAN_SRC_UPP_000_PANE_F,
  PANENAME_ZUKAN_SRC_UPP_000_PANE_G,
  PANENAME_ZUKAN_SRC_UPP_000_PANE_H,
  PANENAME_ZUKAN_SRC_UPP_000_PANE_I,
  PANENAME_ZUKAN_SRC_UPP_000_PANE_J,
  PANENAME_ZUKAN_SRC_UPP_000_PANE_K,
  PANENAME_ZUKAN_SRC_UPP_000_PANE_L,
  PANENAME_ZUKAN_SRC_UPP_000_PANE_M,
  PANENAME_ZUKAN_SRC_UPP_000_PANE_N,
  PANENAME_ZUKAN_SRC_UPP_000_PANE_O,
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


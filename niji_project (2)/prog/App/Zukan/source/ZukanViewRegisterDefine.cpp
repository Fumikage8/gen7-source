//==============================================================================
/**
 @file    ZukanViewRegister.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の登録画面のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <Layout/include/gfl2_Layout.h>

// pmlのインクルード
#include <pml/include/pmlib.h>

// nijiのインクルード
#include <System/include/GflUse.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Util/app_util_2d.h>

#include <Sound/include/Sound.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/zukan/info.h>
#include <niji_conv_header/app/zukan/info_pane.h>
#include <niji_conv_header/app/zukan/info_anim_list.h>

#include <niji_conv_header/message/msg_zukan.h>
#include <niji_conv_header/message/msg_zkn_type.h>
#include <niji_conv_header/message/msg_zkn_form.h>
#include <niji_conv_header/message/msg_zkn_height.h>
#include <niji_conv_header/message/msg_zkn_weight.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProcParam.h>
#include "ZukanType.h"
#include "ZukanDataManager.h"
#include "ZukanTool.h"
#include "ZukanContents.h"
#include "ZukanFrameRegister.h"

#include "ZukanViewRegister.h"

#include "ZukanViewInfoDefine.h"



GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
//
//  ZukanViewRegister
//
//==============================================================================


const ZukanType::ButtonConstInfo ZukanViewRegister::BUTTON_CONST_INFO[BUTTON_ID_NUM] =
{
  ///////////////////////////////
  // 登録画面
  ///////////////////////////////
  APP_ZUKAN_INFO_PAGE_BUTTON_AB(BUTTON_ID_OK,     BUTTON_A3,       COMMON_BTN_LOW_018, SEQ_SE_DECIDE1)
  APP_ZUKAN_INFO_PAGE_BUTTON(BUTTON_ID_VOICE,  BUTTON_Y4_ENTRY, COMMON_BTN_LOW_013, BUTTON_Y, app::tool::ButtonManager::SE_NULL)
  APP_ZUKAN_INFO_PAGE_BUTTON(BUTTON_ID_MOTION, BUTTON_X4_00,    COMMON_BTN_LOW_013, BUTTON_X, SEQ_SE_DECIDE1)
  APP_ZUKAN_INFO_MARU_BUTTON(BUTTON_ID_PAUSE, STOP_ICON, BUTTON_START, TRIGGER, SEQ_SE_SELECT1)  // 一時停止音が欲しい。ピッって感じの軽い音が。SEQ_SE_MESSAGEから変更。
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


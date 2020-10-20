//======================================================================
/**
* @file CharaViewerListener.h
* @date 2015/07/23 17:50:56
* @author araki_syo
* @brief 簡単な説明
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if PM_DEBUG

#if !defined CHARA_VIEWER_LISTENER_H_INCLUDED
#define CHARA_VIEWER_LISTENER_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <AppLib/include/Ui/UIInputListener.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/chara_viewer/res2d/down1.h>
#include <niji_conv_header/app/chara_viewer/res2d/down1_pane.h>
#include <niji_conv_header/app/chara_viewer/res2d/down1_anim_list.h>

#include "CharaViewerDataManager.h"
#include "CharaViewerView.h"
#include "CharaViewerFrame.h"

namespace test{
  namespace chara_viewer{
    class CharaViewerView;
    class CharaViewerListener;
  }
}

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(chara_viewer)

/**
 * @class     CharaViewListener
 * @brief     CharaViewerのリスナー
 */
class CharaViewerListener : public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(CharaViewerListener);

public:
  CharaViewerListener(CharaViewerView* view);
  ~CharaViewerListener();

public:
  virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  app::ui::UIInputListener::ListenerResult OnTouchEvent_LockPane( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_Page1( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_Page2( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_Page3( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_Page4( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_Page5( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );

public:
  app::ui::UIInputListener::ListenerResult OnTouchEvent_CharaSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_OriginLocator( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_ScaleSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );

  app::ui::UIInputListener::ListenerResult OnTouchEvent_PartsSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_SkinSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_HairStyleSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_HeadWearSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_HairAccessorySelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_EyeWearSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_TopsSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_OnepieceSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_BottomsSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_SocksSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_ShoesSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_BagSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_BangleSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_CharmSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_SituationSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_EyeSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_MouthSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );

  app::ui::UIInputListener::ListenerResult OnTouchEvent_ColorPalletSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );

  app::ui::UIInputListener::ListenerResult OnTouchEvent_Controller( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_PlayList( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );

  app::ui::UIInputListener::ListenerResult OnTouchEvent_LightController( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_CameraController( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_ClearColor( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  app::ui::UIInputListener::ListenerResult OnTouchEvent_GeneralConfig( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  void ResetCameraAngleCheckBox();
  void ResetCameraNearFarCheckBox();

public:
  CharaViewerView* m_view;
};

GFL_NAMESPACE_END(chara_viewer)
GFL_NAMESPACE_END(test)

#endif // CHARA_VIEWER_LISTENER_H_INCLUDED

#endif // PM_DEBUG

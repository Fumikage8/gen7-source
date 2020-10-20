//======================================================================
/**
 * @file CharaViewerListenerPage3.cpp
 * @date 2015/07/24 19:00:44
 * @author araki_syo
 * @brief CharaViewer3ページ目のリスナー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include <macro/include/gfl2_Macros.h>

#include "../include/CharaViewerListener.h"

GFL_NAMESPACE_BEGIN(test)
  GFL_NAMESPACE_BEGIN(chara_viewer)


  /**
   * @fn
   * ここに関数の説明を書く
   * @brief 要約説明
   * @param[in] var1 var1の説明
   * @param[out] var2 var2の説明
   * @param[in,out] var3 var3の説明
   * @return 戻り値の説明
   * @sa 参照すべき関数を書けばリンクが貼れる
   * @detail 詳細な説明
   */
app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_Page3( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  // ビーズON/OFF
  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_ZEN_RING_ON_OFF_CHECKBOX))
  {
    m_view->ToggleBeadsOnOff();
    return app::ui::UIInputListener::ENABLE_ACCESS;
  }
  
  // 証ON/OFF
  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_MATERIAL_BAG_CHAR_ON_OFF_CHECK_OF_BAG_CHAR_ON_OFF_CHECK))
  {
    m_view->ToggleCharmOnOff();
    return app::ui::UIInputListener::ENABLE_ACCESS;
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PARTSSELECTORBASE_01))
  {
    return OnTouchEvent_PartsSelector( pTouchPanel, isTouch);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_COLORPALLETBASE_01))
  {
    return OnTouchEvent_ColorPalletSelector( pTouchPanel, isTouch );
  }

  return app::ui::UIInputListener::DISABLE_ACCESS;
}

GFL_NAMESPACE_END(chara_viewer)
  GFL_NAMESPACE_END(test)

#endif // PM_DEBUG

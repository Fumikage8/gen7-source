//======================================================================
/**
 * @file PokeViewerListenerPage5.cpp
 * @date 2015/12/03 12:07:39
 * @author araki_syo
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include "../include/PokeViewerListener.h"

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(pokeviewer)

app::ui::UIInputListener::ListenerResult PokeViewerListener::OnTouchEvent_Page5(gfl2::ui::TouchPanel* pTouchPanel, bool isTouch)
{
  gfl2::lyt::LytPaneIndex ret = m_pView->GetG2DUtil()->GetTouchTriggerPaneIndex(0, PANENAME_DOWN1_GROUP_HITGROUP);

  switch (ret)
  {
    // 影の表示設定
  case PANENAME_DOWN1_PANE_DISPBUTTON_03:
    m_pModelManager->SetShadowVisibility(!m_pModelManager->GetShadowVisibility());
    break;

    // インフレーム設定切り替え
  case PANENAME_DOWN1_PANE_DISPBUTTON_06:

    switch(m_pModelManager->GetCameraMode())
    {
    case PokeViewerPokeModelManager::CAMERA_MODE_FREE:
      m_pModelManager->SetCameraMode(PokeViewerPokeModelManager::CAMERA_MODE_INFRAME_CTR);
      break;

    case PokeViewerPokeModelManager::CAMERA_MODE_INFRAME_CTR:
      m_pModelManager->SetCameraMode(PokeViewerPokeModelManager::CAMERA_MODE_INFRAME_BTM);
      break;

    case PokeViewerPokeModelManager::CAMERA_MODE_INFRAME_BTM:
      m_pModelManager->SetCameraMode(PokeViewerPokeModelManager::CAMERA_MODE_FREE);
      break;
    }
    break;

    // 背景色設定
  case PANENAME_DOWN1_PANE_COL_R_UP:
    {
      gfl2::gfx::Color color = m_pModelManager->GetClearColor();
      color.SetR( gfl2::math::clamp( color.GetR() + 0.1f, 0.0f, 1.0f) );
      m_pModelManager->SetClearColor(color);
    }
    break;

  case PANENAME_DOWN1_PANE_COL_G_UP:
    {
      gfl2::gfx::Color color = m_pModelManager->GetClearColor();
      color.SetG( gfl2::math::clamp( color.GetG() + 0.1f, 0.0f, 1.0f) );
      m_pModelManager->SetClearColor(color);
    }
    break;

  case PANENAME_DOWN1_PANE_COL_B_UP:
    {
      gfl2::gfx::Color color = m_pModelManager->GetClearColor();
      color.SetB( gfl2::math::clamp( color.GetB() + 0.1f, 0.0f, 1.0f) );
      m_pModelManager->SetClearColor(color);
    }
    break;

  case PANENAME_DOWN1_PANE_COL_R_DOWN:
    {
      gfl2::gfx::Color color = m_pModelManager->GetClearColor();
      color.SetR( gfl2::math::clamp( color.GetR() - 0.1f, 0.0f, 1.0f) );
      m_pModelManager->SetClearColor(color);
    }
    break;

  case PANENAME_DOWN1_PANE_COL_G_DOWN:
    {
      gfl2::gfx::Color color = m_pModelManager->GetClearColor();
      color.SetG( gfl2::math::clamp( color.GetG() - 0.1f, 0.0f, 1.0f) );
      m_pModelManager->SetClearColor(color);
    }
    break;

  case PANENAME_DOWN1_PANE_COL_B_DOWN:
    {
      gfl2::gfx::Color color = m_pModelManager->GetClearColor();
      color.SetB( gfl2::math::clamp( color.GetB() - 0.1f, 0.0f, 1.0f) );
      m_pModelManager->SetClearColor(color);
    }
    break;

    // エッジ色設定
  case PANENAME_DOWN1_PANE_DISPBUTTON_10:
    {
      static b32 isBlack = false;
      if(isBlack)
      {
        m_pModelManager->SetEdgeColor(gfl2::gfx::Color(0.4f, 0.4f, 0.4f));
        isBlack = false;
      }
      else
      {
        m_pModelManager->SetEdgeColor(gfl2::gfx::Color(0.0f, 0.0f, 0.0f));
        isBlack = true;
      }
    }
    break;

  default:
    break;
  }

  return app::ui::UIInputListener::ENABLE_ACCESS;
}

GFL_NAMESPACE_END(pokeviewer)
GFL_NAMESPACE_END(test)

#endif // PM_DEBUG

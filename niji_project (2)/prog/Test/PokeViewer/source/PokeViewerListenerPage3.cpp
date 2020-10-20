//======================================================================
/**
 * @file PokeViewerListenerPage3.cpp
 * @date 2015/12/03 12:06:40
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

app::ui::UIInputListener::ListenerResult PokeViewerListener::OnTouchEvent_Page3(gfl2::ui::TouchPanel* pTouchPanel, bool isTouch)
{
  gfl2::lyt::LytPaneIndex ret = m_pView->GetG2DUtil()->GetTouchTriggerPaneIndex(0, PANENAME_DOWN1_GROUP_HITGROUP);

  switch(ret)
  {

  // カラーシェーダ設定
  case PANENAME_DOWN1_PANE_SHADER_C_R_U:
    {
      gfl2::gfx::Color color = m_pModelManager->GetPokeShaderColor();
      color.SetR( gfl2::math::clamp( color.GetR() + 0.1f, 0.0f, 1.0f) );
      m_pModelManager->SetPokeShaderColor(color);
    }
    break;

  case PANENAME_DOWN1_PANE_SHADER_C_G_U:
    {
      gfl2::gfx::Color color = m_pModelManager->GetPokeShaderColor();
      color.SetG( gfl2::math::clamp( color.GetG() + 0.1f, 0.0f, 1.0f) );
      m_pModelManager->SetPokeShaderColor(color);
    }
    break;

  case PANENAME_DOWN1_PANE_SHADER_C_B_U:
    {
      gfl2::gfx::Color color = m_pModelManager->GetPokeShaderColor();
      color.SetB( gfl2::math::clamp( color.GetB() + 0.1f, 0.0f, 1.0f) );
      m_pModelManager->SetPokeShaderColor(color);
    }
    break;

  case PANENAME_DOWN1_PANE_SHADER_C_R_D:
    {
      gfl2::gfx::Color color = m_pModelManager->GetPokeShaderColor();
      color.SetR( gfl2::math::clamp( color.GetR() - 0.1f, 0.0f, 1.0f) );
      m_pModelManager->SetPokeShaderColor(color);
    }
    break;

  case PANENAME_DOWN1_PANE_SHADER_C_G_D:
    {
      gfl2::gfx::Color color = m_pModelManager->GetPokeShaderColor();
      color.SetG( gfl2::math::clamp( color.GetG() - 0.1f, 0.0f, 1.0f) );
      m_pModelManager->SetPokeShaderColor(color);
    }
    break;

  case PANENAME_DOWN1_PANE_SHADER_C_B_D:
    {
      gfl2::gfx::Color color = m_pModelManager->GetPokeShaderColor();
      color.SetB( gfl2::math::clamp( color.GetB() - 0.1f, 0.0f, 1.0f) );
      m_pModelManager->SetPokeShaderColor(color);
    }
    break;

    // カラーシェーダ係り具合
  case PANENAME_DOWN1_PANE_SHADER_POW_U:
    {
      gfl2::gfx::Color color = m_pModelManager->GetPokeShaderColor();
      color.SetA( gfl2::math::clamp( color.GetA() + 0.1f, 0.0f, 1.0f) );
      m_pModelManager->SetPokeShaderColor(color);
    }
    break;

  case PANENAME_DOWN1_PANE_SHADER_POW_D:
    {
      gfl2::gfx::Color color = m_pModelManager->GetPokeShaderColor();
      color.SetA( gfl2::math::clamp( color.GetA() - 0.1f, 0.0f, 1.0f) );
      m_pModelManager->SetPokeShaderColor(color);
    }
    break;

    // カラーシェーダON/OFF
  case PANENAME_DOWN1_PANE_SHADER_C_BUT:
    {
      m_pModelManager->SetPokeShaderEnable(!m_pModelManager->IsPokeShaderEnable());
    }
    break;

    // カラーシェーダプリセット
  case PANENAME_DOWN1_PANE_SHADER_C_DOKU:
    {
      m_pModelManager->SetPokeShaderColor(gfl2::gfx::Color(0.5f, 0.0f, 1.0f, 0.5f));
    }
    break;

  case PANENAME_DOWN1_PANE_SHADER_C_MAHI:
    {
      m_pModelManager->SetPokeShaderColor(gfl2::gfx::Color(1.0f, 1.0f, 0.0f, 0.5f));
    }
    break;

  case PANENAME_DOWN1_PANE_SHADER_C_YAKEDO:
    {
      m_pModelManager->SetPokeShaderColor(gfl2::gfx::Color(1.0f, 0.25f, 0.25f, 0.75f));
    }
    break;

  case PANENAME_DOWN1_PANE_SHADER_C_KOORI:
    {
      m_pModelManager->SetPokeShaderColor(gfl2::gfx::Color(0.5f, 1.0f, 1.0f, 1.0f));
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

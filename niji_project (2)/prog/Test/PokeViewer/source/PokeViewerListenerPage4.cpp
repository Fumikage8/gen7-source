//======================================================================
/**
 * @file PokeViewerListenerPage4.cpp
 * @date 2015/12/03 12:07:15
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

app::ui::UIInputListener::ListenerResult PokeViewerListener::OnTouchEvent_Page4(gfl2::ui::TouchPanel* pTouchPanel, bool isTouch)
{
  gfl2::lyt::LytPaneIndex ret = m_pView->GetG2DUtil()->GetTouchTriggerPaneIndex(0, PANENAME_DOWN1_GROUP_HITGROUP);

  switch (ret)
  {
  case PANENAME_DOWN1_PANE_LOAD_ROM_BTN:
    m_pView->LoadPokeModel();
    break;

    // 図鑑番号
  case PANENAME_DOWN1_PANE_MONSNO_U_00:
    m_pModelManager->SetMonsNo(std::min((s32)m_pModelManager->GetMonsNo() + 100, (s32)MONSNO_TAMAGO));
    break;

  case PANENAME_DOWN1_PANE_MONSNO_U_01:
    m_pModelManager->SetMonsNo(std::min((s32)m_pModelManager->GetMonsNo() + 10, (s32)MONSNO_TAMAGO));
    break;

  case PANENAME_DOWN1_PANE_MONSNO_U_02:
    m_pModelManager->SetMonsNo(std::min((s32)m_pModelManager->GetMonsNo() + 1, (s32)MONSNO_TAMAGO));
    break;

  case PANENAME_DOWN1_PANE_MONSNO_D_00:
    m_pModelManager->SetMonsNo(std::max((s32)m_pModelManager->GetMonsNo() - 100, (s32)MONSNO_HUSIGIDANE));
    break;

  case PANENAME_DOWN1_PANE_MONSNO_D_01:
    m_pModelManager->SetMonsNo(std::max((s32)m_pModelManager->GetMonsNo() - 10, (s32)MONSNO_HUSIGIDANE));
    break;

  case PANENAME_DOWN1_PANE_MONSNO_D_02:
    m_pModelManager->SetMonsNo(std::max((s32)m_pModelManager->GetMonsNo() - 1, (s32)MONSNO_HUSIGIDANE));
    break;

    // フォルム切り替え
  case PANENAME_DOWN1_PANE_FORMNO_U_00:
    m_pModelManager->ChangeFormNo(10);
    break;

  case PANENAME_DOWN1_PANE_FORMNO_U_01:
    m_pModelManager->ChangeFormNo(1);
    break;

  case PANENAME_DOWN1_PANE_FORMNO_D_00:
    m_pModelManager->ChangeFormNo(-10);
    break;

  case PANENAME_DOWN1_PANE_FORMNO_D_01:
    m_pModelManager->ChangeFormNo(-1);
    break;

    // 性別切り替えボタン
  case PANENAME_DOWN1_PANE_SEX_BTN:
    m_pModelManager->ToggleSex();
    break;

    // 色違い切り替えボタン
  case PANENAME_DOWN1_PANE_RARE_BTN:
    m_pModelManager->ToggleRare();
    break;

    // ポケモンのモーションタイプ切り替えボタン
  case PANENAME_DOWN1_PANE_MOTION_TYPE_BTN_BT:
    m_pModelManager->SetAnimeType( (m_pModelManager->GetAnimeType() == PokeTool::MODEL_ANIMETYPE_BATTLE? PokeTool::MODEL_ANIMETYPE_KAWAIGARI : PokeTool::MODEL_ANIMETYPE_BATTLE) );
    break;

    // 単体キャプチャボタン
  case PANENAME_DOWN1_PANE_CAPTURE_BTN:
    m_pModelManager->SetCaptureMode(PokeViewerPokeModelManager::UPDATE_TYPE_CAPTURE);
    break;

  default:
    break;
  }

  return app::ui::UIInputListener::ENABLE_ACCESS;
}

GFL_NAMESPACE_END(pokeviewer)
GFL_NAMESPACE_END(test)

#endif // PM_DEBUG

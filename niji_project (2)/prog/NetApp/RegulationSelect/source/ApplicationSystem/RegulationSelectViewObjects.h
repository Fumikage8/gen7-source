// ============================================================================
/*
 * @file RegulationSelectViewObjects.h
 * @brief レギュレーション選択アプリのDrawクラスを管理するクラスです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#if !defined( REGULATIONSELECT_VIEWOBJECTS_H_INCLUDE )
#define REGULATIONSELECT_VIEWOBJECTS_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(UI)
class NetAppMessageMenu;
GFL_NAMESPACE_END(UI)
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)

  GFL_NAMESPACE_BEGIN(Draw)
class RegulationSelectMainUpperView;
class RegulationSelectBGUpperView;
class RegulationSelectMainLowerView;
class RegulationSelectMessageMenuLowerView;
class RegulationDetailLowerView;
GFL_NAMESPACE_END(Draw)

GFL_NAMESPACE_BEGIN(ApplicationSystem)

class RegulationSelectWork;

class RegulationSelectViewObjects
{
  GFL_FORBID_COPY_AND_ASSIGN( RegulationSelectViewObjects );
public:
  RegulationSelectViewObjects();
  virtual~RegulationSelectViewObjects();

  void RemoveFromSuperViewAll();

  void CreateDraw( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork, NetAppLib::UI::NetAppMessageMenu& netAppMessageMenu );

  void InputDisableAll();

  bool IsDrawing();

  NetApp::RegulationSelect::Draw::RegulationSelectMainUpperView*            GetRegulationSelectMainUpperView() { return m_pRegulationSelectMainUpperView; }
  NetApp::RegulationSelect::Draw::RegulationSelectBGUpperView*              GetRegulationSelectBGUpperView() { return m_pRegulationSelectBGUpperView; }
  NetApp::RegulationSelect::Draw::RegulationSelectMainLowerView*            GetRegulationSelectMainLowerView() { return m_pRegulationSelectMainLowerView; }
  NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerView*     GetRegulationSelectMessageMenuLowerView() { return m_pRegulationSelectMessageMenuLowerView; }
  NetApp::RegulationSelect::Draw::RegulationDetailLowerView*                GetRegulationDetailLowerView() { return m_pRegulationDetailLowerView; }

private:

  NetApp::RegulationSelect::Draw::RegulationSelectMainUpperView*            m_pRegulationSelectMainUpperView;
  NetApp::RegulationSelect::Draw::RegulationSelectBGUpperView*              m_pRegulationSelectBGUpperView;
  NetApp::RegulationSelect::Draw::RegulationSelectMainLowerView*            m_pRegulationSelectMainLowerView;
  NetApp::RegulationSelect::Draw::RegulationSelectMessageMenuLowerView*     m_pRegulationSelectMessageMenuLowerView;
  NetApp::RegulationSelect::Draw::RegulationDetailLowerView*                m_pRegulationDetailLowerView;

};

GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)


#endif // REGULATIONSELECT_VIEWOBJECTS_H_INCLUDE

// ============================================================================
/*
 * @file RegulationDetailLowerView.h
 * @brief レギュレーション詳細の下画面です。
 * @date 2015.06.18
 * @author endo_akira
 */
// ============================================================================
#if !defined( REGULATIONDETAILLOWERVIEW_H_INCLUDE )
#define REGULATIONDETAILLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectWork.h"
#include "AppLib/include/Ui/UIInputListener.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(Draw)


class RegulationDetailLowerView :
  public NetAppLib::System::NetApplicationViewBase,
  public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( RegulationDetailLowerView );
public:
  RegulationDetailLowerView( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork );
  virtual~RegulationDetailLowerView();

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );


private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_REGULATIONSELECT,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };


private:

  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork*            m_pRegulationSelectWork;

};


GFL_NAMESPACE_END(Draw)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONDETAILLOWERVIEW_H_INCLUDE

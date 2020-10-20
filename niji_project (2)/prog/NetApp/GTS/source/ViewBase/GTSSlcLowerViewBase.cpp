// ============================================================================
/*
 * @file GTSSlcLowerViewBase.cpp
 * @brief GTS_SLC_LOW_000.flytを扱うDrawクラスのベースとなるクラスです。
 * @date 2015.05.14
 */
// ============================================================================
#include "NetApp/GTS/source/ViewBase/GTSSlcLowerViewBase.h"
#include "NetApp/GTS/source/GTSUtility.h"
#include "NetApp/GTS/source/GTSResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include <arc_index/message.gaix>
#include <niji_conv_header/app/GTS/GTS.h>
#include <niji_conv_header/app/GTS/GTS_anim_list.h>
#include <niji_conv_header/app/GTS/GTS_pane.h>
#include <niji_conv_header/message/msg_gts.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(ViewBase)


GTSSlcLowerViewBase::GTSSlcLowerViewBase( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork ) :
  NetAppLib::UI::NetAppCursorView( pGTSWork )
{
  Create2DOneLayout( pGTSWork,
                     NetApp::GTS::GTS_RESOURCE_ID_LAYOUT_GTS,
                     LYTRES_GTS_GTS_SLC_LOW_000_BFLYT,
                     LA_GTS_GTS_SLC_LOW_000___NUM,
                     app::util::G2DUtil::SETUP_LOWER,
                     pGTSWork->GetMessageUtility(),
                     GARC_message_gts_DAT );
}


GTSSlcLowerViewBase::~GTSSlcLowerViewBase()
{
}


void GTSSlcLowerViewBase::Update()
{
  NetAppLib::UI::NetAppCursorView::Update();
}


void GTSSlcLowerViewBase::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  NetAppLib::UI::NetAppCursorView::Draw( displayNo );
}


void GTSSlcLowerViewBase::SetVisibleBackButton( bool bVisible )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytParts* pBtnBack = pLower->GetPartsPane( PANENAME_GTS_SLC_LOW_000_PANE_BTN_BACK );
  pBtnBack->SetVisible( bVisible );
}


void GTSSlcLowerViewBase::SetVisibleBlackFilter( bool bVisible )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytPicture* pBg00 = pLower->GetPicturePane( PANENAME_GTS_SLC_LOW_000_PANE_BG_00 );
  pBg00->SetVisible( bVisible );
}


GFL_NAMESPACE_END(ViewBase)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

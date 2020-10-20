
//==============================================================================
/**
 * @file   LowerView_SelectionBase.cpp
 * @date   2015/07/31 Fri. 15:55:29
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "AppLib/include/Util/app_util_GetPaneHelper.h"

#include  "../../ApplicationSystem/GiftWork.h"


#include  "../../GiftResource.h"
#include  "./LowerView_SelectionBase.h"

// =============================================================================
/**/
//==============================================================================
/*
  local
*/
namespace
{


}

namespace NetApp  {
namespace Gift {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     LowerView_SelectionBase
  *  @brief    ctor
  */
//------------------------------------------------------------------
LowerView_SelectionBase::LowerView_SelectionBase(GiftWork*  pWork)
  : NetAppLib::UI::NetAppCursorView( pWork )
  , m_pWork(pWork)
{
  {
    Create2DOneLayout (
      m_pWork,
      RESOURCE_Layout,
      LYTRES_GIFT_GIFT_SLC_LOW_000_BFLYT,
      LA_GIFT_GIFT_SLC_LOW_000___NUM,
      app::util::G2DUtil::SETUP_LOWER,
      &(pWork->GetMessageUtility()),
      GIFT_MESSAGE_GRAC_ID
    );
  }


  {
    app::util::GetPaneHelper  helper(GetG2DUtil()->GetLayoutWork(LAYOUTWORK_SelectionLow), &(GetG2DUtil()->GetLayoutResourceID(0)));

    m_pTextBoxes[TEXTBOX_Large1of3]         = helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONL_00).GetTextBoxPane(PANENAME_GIFT_BTN_LOW_002_PANE_TEXTBOX_00);
    m_pTextBoxes[TEXTBOX_Large2of3]         = helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONL_01).GetTextBoxPane(PANENAME_GIFT_BTN_LOW_002_PANE_TEXTBOX_00);
    m_pTextBoxes[TEXTBOX_Large3of3]         = helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONL_02).GetTextBoxPane(PANENAME_GIFT_BTN_LOW_002_PANE_TEXTBOX_00);

    m_pTextBoxes[TEXTBOX_Small1of9]         = helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_00).GetTextBoxPane(PANENAME_GIFT_BTN_LOW_003_PANE_TEXTBOX_00);
    m_pTextBoxes[TEXTBOX_Small2of9]         = helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_01).GetTextBoxPane(PANENAME_GIFT_BTN_LOW_003_PANE_TEXTBOX_00);
    m_pTextBoxes[TEXTBOX_Small3of9]         = helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_02).GetTextBoxPane(PANENAME_GIFT_BTN_LOW_003_PANE_TEXTBOX_00);
    m_pTextBoxes[TEXTBOX_Small4of9]         = helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_03).GetTextBoxPane(PANENAME_GIFT_BTN_LOW_003_PANE_TEXTBOX_00);
    m_pTextBoxes[TEXTBOX_Small5of9]         = helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_04).GetTextBoxPane(PANENAME_GIFT_BTN_LOW_003_PANE_TEXTBOX_00);
    m_pTextBoxes[TEXTBOX_Small6of9]         = helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_05).GetTextBoxPane(PANENAME_GIFT_BTN_LOW_003_PANE_TEXTBOX_00);
    m_pTextBoxes[TEXTBOX_Small7of9]         = helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_06).GetTextBoxPane(PANENAME_GIFT_BTN_LOW_003_PANE_TEXTBOX_00);
    m_pTextBoxes[TEXTBOX_Small8of9]         = helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_07).GetTextBoxPane(PANENAME_GIFT_BTN_LOW_003_PANE_TEXTBOX_00);
    m_pTextBoxes[TEXTBOX_Small9of9]         = helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_08).GetTextBoxPane(PANENAME_GIFT_BTN_LOW_003_PANE_TEXTBOX_00);

    m_pTextBoxes[TEXTBOX_EventInformation]  = helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_SCROLLMESSAGE).GetTextBoxPane(PANENAME_GIFT_PLT_LOW_000_PANE_TEXTBOX_00);
  }
}

//------------------------------------------------------------------
/**
  *  @func     ~LowerView_SelectionBase
  *  @brief    dtor
  */
//------------------------------------------------------------------
LowerView_SelectionBase::~LowerView_SelectionBase()
{
  ReleaseCursor();
  Delete2D();
  DeleteGraphicsSystem();
}


//--------------------------------------------------------------------------------------------
/**
* @brief  更新処理
*         子ウインドウについても自動的にコールする(子→親の順）
*/
//--------------------------------------------------------------------------------------------
void  LowerView_SelectionBase::Update(void)
{
  NetAppLib::UI::NetAppCursorView::Update();    /*  super  */
}


//--------------------------------------------------------------------------------------------
/**
* @brief  描画処理
*         子ウインドウについても自動的にコールする(親→子の順）
*/
//--------------------------------------------------------------------------------------------
void  LowerView_SelectionBase::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  GetG2DUtil()->AddDrawableLytWkForOneFrame(m_pWork->GetAppRenderingManager(), displayNo, LAYOUTWORK_SelectionLow);
  NetAppLib::UI::NetAppCursorView::Draw(displayNo);
}


} /*  namespace Gift */
} /*  namespace NetApp      */


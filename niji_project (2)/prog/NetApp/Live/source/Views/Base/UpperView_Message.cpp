//==============================================================================
/**
 * @file   UpperView_Message.cpp
 * @date   2015/07/13 Mon. 19:51:42
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
#include  "../../LiveResource.h"
#include  "../../ApplicationSystem/LiveWork.h"

#include  "./UpperView_Message.h"


// =============================================================================
/**/
//==============================================================================
namespace
{
  enum  {
    LAYOUTWORK_Message,

    LAYOUTWORK_MAX
  };
} /*  namespace  */




namespace NetApp  {
namespace Live    {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     UpperView_Message
  *  @brief    ctor
  */
//------------------------------------------------------------------
UpperView_Message::UpperView_Message(LiveWork* pWork)
  : NetAppLib::System::NetApplicationViewBase(
      pWork
      , RESOURCE_Layout
      , LYTRES_LIVE_LIVE_MSG_UPP_000_BFLYT
      , LA_LIVE_LIVE_MSG_UPP_000___NUM
      , app::util::G2DUtil::SETUP_UPPER
      , &(pWork->GetMessageUtility())
      , LIVE_MESSAGE_GRAC_ID
    )
  , m_pWork(pWork)
{


  /*  pane  */
  {
    app::util::G2DUtil*         pG2DUtil        = GetG2DUtil();
    gfl2::lyt::LytWk*           pLayoutWork     = pG2DUtil->GetLayoutWork(LAYOUTWORK_Message);
    gfl2::lyt::LytMultiResID&   rLytMultiResID  = pG2DUtil->GetLayoutResourceID(LAYOUTWORK_Message);
    app::util::GetPaneHelper    helper(pLayoutWork, &rLytMultiResID);


    m_TextBoxes[TEXTBOX_Title]        = helper.Push(PANENAME_LIVE_MSG_UPP_000_PANE_MESSAGE).GetTextBoxPane(PANENAME_COMMON_MSG_UPP_001_PANE_TEXTBOX_00);
    m_TextBoxes[TEXTBOX_Information]  = helper.Clear().GetTextBoxPane(PANENAME_LIVE_MSG_UPP_000_PANE_TEXTBOX_00);

    /*    */
    SetTextboxPaneMessage( m_TextBoxes[TEXTBOX_Title],        msg_lt_sys_01 );  /*  ライブ大会      */
    SetTextboxPaneMessage( m_TextBoxes[TEXTBOX_Information],  msg_lt_sys_02 );  /*  ライブ大会は～  */
  }
}


//--------------------------------------------------------------------------------------------
/**
* @brief    デストラクタ
*/
//--------------------------------------------------------------------------------------------
UpperView_Message::~UpperView_Message()
{
  Delete2D();
  DeleteGraphicsSystem();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Message::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  GetG2DUtil()->AddDrawableLytWkForOneFrame(m_pWork->GetAppRenderingManager(), displayNo, 0);
}



} /*  namespace Live    */
} /*  namespace NetApp  */

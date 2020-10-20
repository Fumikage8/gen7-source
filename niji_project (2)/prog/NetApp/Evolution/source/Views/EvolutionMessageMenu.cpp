
//==============================================================================
/**
 * @file   EvolutionMessageMenu.cpp
 * @date   2015/08/28 Fri. 18:30:58
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../System/EvolutionWork.h"

#include  "../EvolutionResource.h"
#include  "./EvolutionMessageMenu.h"


// =============================================================================
/**/
//==============================================================================
namespace {
  static const u32  MESSAGE_GARC_ID = NetApp::Evolution::EVOLUTION_MESSAGE_GRAC_ID;
}




namespace NetApp  {
namespace Evolution {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
EvolutionMessageMenu::EvolutionMessageMenu(EvolutionWork* pWork, NetAppLib::UI::NetAppMessageMenu::IEventHandler* pHandler)
  : NetAppLib::UI::NetAppMessageMenu(pWork, pHandler)
  , m_pWork(pWork)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
EvolutionMessageMenu::~EvolutionMessageMenu()
{
  Delete2D();
  DeleteGraphicsSystem();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    EvolutionMessageMenu専用のSetup
  */
//------------------------------------------------------------------
//void  EvolutionMessageMenu::EvolutionMessageMenuSetup(app::ui::UIView* pTargetView)
//{
//  m_pCommonMessageWindowTargetView  = pTargetView;
//  Setup(RESOURCE_Layout, &m_pWork->GetMessageUtility());
//}


//------------------------------------------------------------------
/**
  *  @brief    パラメータなし汎用
  */
//------------------------------------------------------------------
void  EvolutionMessageMenu::Show(const Mode mode)
{

/*
  void  ShowMessage(const gfl2::str::MsgData& rWindowMessageData, const u32 messageID, const bool isImmediateMessage = false, const bool isShowTimerIcon = false, const bool isShowBG = false);
*/
  PreShow();
  switch(mode)
  {
//    case  MODE_Welcome:           /**<  ライブ大会へようこそ  */
//      ShowMessage(LIVE_MESSAGE_GRAC_ID, msg_lt_win_01, true);
//      break;


    default:
      /*  nop  */
      break;
  }

}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    showの前に共通でやっておくこと
  */
//------------------------------------------------------------------
void  EvolutionMessageMenu::PreShow(void)
{
  SetYNListMode(false);
  SetFinishMode_None();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  EvolutionMessageMenu::ShowYN(const u32 messageID, const bool isImmediate)
{
#if 0
  SetYNListMode(true);
  ShowMessageWithListMenu(
    MESSAGE_GARC_ID,
    messageID,
    isImmediate,
    MESSAGE_GARC_ID,
    msg_lt_sel_02,
    msg_lt_sel_03
  );
#endif
}


} /*  namespace Evolution   */
} /*  namespace NetApp      */


//==============================================================================
/**
 * @file   UpperView_Demo.cpp
 * @date   2015/11/24 Tue. 13:27:59
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

#include  "../EggHatchingResource.h"
#include  "../System/EggHatchingWork.h"

#include  "./UpperView_Demo.h"

namespace NetApp    {
namespace EggHatching {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
UpperView_Demo::UpperView_Demo(EggHatchingWork* pWork)
  : NetAppLib::System::NetApplicationViewBase(
      pWork
      , RESOURCE_Layout
      , LYTRES_EVOLUTION_EVOLUTION_MIN_UPP_000_BFLYT
      , LA_EVOLUTION_EVOLUTION_MIN_UPP_000___NUM
      , app::util::G2DUtil::SETUP_UPPER
      , &(pWork->GetMessageUtility())
      , EGGHATCHING_MESSAGE_GRAC_ID
    )
  , m_pWork(pWork)
  , m_MessageWindowManipulator()
  , m_pHandler(NULL)
  , m_pMessageWindowPane(NULL)
{
  app::util::G2DUtil*         pG2DUtil        = GetG2DUtil();
  gfl2::lyt::LytWk*           pLayoutWork     = pG2DUtil->GetLayoutWork(0);
  gfl2::lyt::LytMultiResID&   rLytMultiResID  = pG2DUtil->GetLayoutResourceID(0);
  app::util::GetPaneHelper    helper(pLayoutWork, &rLytMultiResID);



  /*  manip  */
  {
    App::Tool::CommonMessageWindowManipulator::SSetupInfo setupInfo;


    setupInfo.pHeap               = m_pWork->GetAppHeap();

    setupInfo.pTargetView         = this;            /**<  各種リソースを持つUIView。G2DUtilを使うのでCreate2D済みであること */
    setupInfo.layoutWorkID        = 0;           /**<  リソースが含まれるLayoutWorkのID                                  */
    setupInfo.pHandler            = this;               /**<  各種イベントハンドラ、不要な場合はNULL                            */


//    setupInfo.animID_TimerIcon;       /**<  タイマアイコンのアニメーション                                    */
    setupInfo.animID_MsgCursor    = LA_EVOLUTION_EVOLUTION_MIN_UPP_000__CURSOR_00_KEEP;   /**<  メッセージカーソルのアニメーション                                */
    setupInfo.paneID_TextBox2     = PANENAME_EVOLUTION_MIN_UPP_000_PANE_TEXTBOX_00;       /**<  即時表示用TextBoxPane                                             */
    setupInfo.paneID_NullText1    = PANENAME_EVOLUTION_MIN_UPP_000_PANE_NULL_TEXT1;       /**<  ストリーム表示TextBoxPaneがまとめられているPaneのID、即時表示との切り替えに使う  */
    setupInfo.paneID_TextBox1a    = PANENAME_EVOLUTION_MIN_UPP_000_PANE_TEXTBOX_01;       /**<  ストリーム表示TextBoxPane1/2                                      */
    setupInfo.paneID_TextBox2a    = PANENAME_EVOLUTION_MIN_UPP_000_PANE_TEXTBOX_02;       /**<  ストリーム表示TextBoxPane2/2                                      */
    setupInfo.paneID_MsgCursor    = PANENAME_EVOLUTION_MIN_UPP_000_PANE_CURSOR_00;        /**<  （ユーザ入力待ちなどの）メッセージカーソルのPaneID                */


    m_MessageWindowManipulator.Setup(setupInfo, NULL);
    pG2DUtil->StartAnime(0, LA_EVOLUTION_EVOLUTION_MIN_UPP_000_LOOP);
    pG2DUtil->StartAnime(0, LA_EVOLUTION_EVOLUTION_MIN_UPP_000_OUT);
  }

  m_pMessageWindowPane  = helper.Clear().GetPane(PANENAME_EVOLUTION_MIN_UPP_000_PANE_ALL);
  m_pMessageWindowPane->SetVisible(false);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
UpperView_Demo::~UpperView_Demo()
{
}


//------------------------------------------------------------------
/**
  *  @brief    タマゴがかえって
  */
//------------------------------------------------------------------
void  UpperView_Demo::SetViewMode_Hatched(const pml::pokepara::CoreParam* pCoreParam)
{
  app::util::G2DUtil*   pG2DUtil = GetG2DUtil();
  gfl2::str::StrBuf*    pStrBuf  = pG2DUtil->GetTempStrBuf( 1 );


  pG2DUtil->SetRegisterMonsName(0, pCoreParam);
  pG2DUtil->GetStringExpand(pStrBuf, msg_egg_event_02_01);

  m_MessageWindowManipulator.SetFinishMode_UserInput();
  m_MessageWindowManipulator.ShowMessage(pStrBuf);

  m_pMessageWindowPane->SetVisible(true);
  GetG2DUtil()->StartAnime(0, LA_EVOLUTION_EVOLUTION_MIN_UPP_000_IN);
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  UpperView_Demo::Update(void)
{
  NetAppLib::System::NetApplicationViewBase::Update();
  m_MessageWindowManipulator.UpdateWindow();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Demo::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  GetG2DUtil()->AddDrawableLytWkForOneFrame(m_pWork->GetAppRenderingManager(), displayNo, 0);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  UpperView_Demo::CommonMessageWindowManipulator_OnEvent(const App::Tool::CommonMessageWindowManipulator::EventID eventID)
{
  switch(eventID)
  {
    case  App::Tool::CommonMessageWindowManipulator::EVENT_OnEOM:      /**<  メッセージの終端まで表示された（RES_DONE相当、ShowMessage後の一度のみ発生）  */
      Event(IEventHandler::EVENT_EOM);
      break;

    case  App::Tool::CommonMessageWindowManipulator::EVENT_OnFinish:   /**<  設定されたフィニッシュ待ちまで終了（RES_FINISH相当）                        */
      GetG2DUtil()->StartAnime(0, LA_EVOLUTION_EVOLUTION_MIN_UPP_000_OUT);
      Event(IEventHandler::EVENT_MessageFinish);
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  UpperView_Demo::Event(const IEventHandler::EventCode eventCode)
{
  if(m_pHandler)
  {
    m_pHandler->UpperView_Demo_OnEvent(eventCode);
  }
}


} /*  namespace EggHatching */
} /*  namespace NetApp    */

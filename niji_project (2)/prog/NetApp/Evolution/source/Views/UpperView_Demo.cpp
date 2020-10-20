//==============================================================================
/**
 * @file   UpperView_Demo.cpp
 * @date   2015/09/01 Tue. 12:05:34
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

#include  "../EvolutionResource.h"
#include  "../System/EvolutionWork.h"

#include  "./UpperView_Demo.h"


// =============================================================================
/**/
//==============================================================================
namespace
{
  static const u32  EVOLUTION_MEID_WazaOboe   = STRM_ME_LVUP;         /**<  技を覚えたときのSEID  */
  static const u32  EVOLUTION_SEID_WazaWasure = SEQ_SE_WAZAWASURE;    /**<  技を忘れたときのSEID  */
}



namespace NetApp    {
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
UpperView_Demo::UpperView_Demo(EvolutionWork* pWork)
  : NetAppLib::System::NetApplicationViewBase(
      pWork
      , RESOURCE_Layout
      , LYTRES_EVOLUTION_EVOLUTION_MIN_UPP_000_BFLYT
      , LA_EVOLUTION_EVOLUTION_MIN_UPP_000___NUM
      , app::util::G2DUtil::SETUP_UPPER
      , &(pWork->GetMessageUtility())
      , EVOLUTION_MESSAGE_GRAC_ID
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
    setupInfo.animID_MsgCursor    = LA_EVOLUTION_EVOLUTION_MIN_UPP_000__CURSOR_00_KEEP;       /**<  メッセージカーソルのアニメーション                                */


//    setupInfo.paneID_WindowParts  = helper.GetPane(PANENAME_COMMON_CUR_LOW_001_PANE_ALL);     /**<  メッセージウインドウ部品のIndex                                   */

    setupInfo.paneID_TextBox2     = PANENAME_EVOLUTION_MIN_UPP_000_PANE_TEXTBOX_00;        /**<  即時表示用TextBoxPane                                             */

    setupInfo.paneID_NullText1    = PANENAME_EVOLUTION_MIN_UPP_000_PANE_NULL_TEXT1;       /**<  ストリーム表示TextBoxPaneがまとめられているPaneのID、即時表示との切り替えに使う  */
    setupInfo.paneID_TextBox1a    = PANENAME_EVOLUTION_MIN_UPP_000_PANE_TEXTBOX_01;       /**<  ストリーム表示TextBoxPane1/2                                      */
    setupInfo.paneID_TextBox2a    = PANENAME_EVOLUTION_MIN_UPP_000_PANE_TEXTBOX_02;       /**<  ストリーム表示TextBoxPane2/2                                      */

//    setupInfo.paneID_TimerIcon    = ;       /**<  タイマアイコンのPaneID、不要な場合は gfl2::lyt::LYTSYS_PANEINDEX_ERROR  */

    setupInfo.paneID_MsgCursor    = PANENAME_EVOLUTION_MIN_UPP_000_PANE_CURSOR_00;       /**<  （ユーザ入力待ちなどの）メッセージカーソルのPaneID                */



    m_MessageWindowManipulator.Setup(setupInfo, NULL);
    pG2DUtil->StartAnime(0, LA_EVOLUTION_EVOLUTION_MIN_UPP_000_OUT);

    /*  callback  */
    pG2DUtil->GetMsgWin()->SetMessageCallback(this);
  }


  m_pMessageWindowPane  = helper.Clear().GetPane(PANENAME_EVOLUTION_MIN_UPP_000_PANE_ALL);
  m_pMessageWindowPane->SetVisible(false);


  /*  pane  */
/*
  {
    m_TextBoxes[TEXTBOX_Date]             = helper.GetTextBoxPane(PANENAME_LIVE_MIN_UPP_000_PANE_TEXT_DAY);
    m_TextBoxes[TEXTBOX_LiveName1of2]     = helper.GetTextBoxPane(PANENAME_LIVE_MIN_UPP_000_PANE_TEXT_LIVENAME_00);
    m_TextBoxes[TEXTBOX_LiveName2of2]     = helper.GetTextBoxPane(PANENAME_LIVE_MIN_UPP_000_PANE_TEXT_LIVENAME_01);
    m_TextBoxes[TEXTBOX_Description]      = helper.GetTextBoxPane(PANENAME_LIVE_MIN_UPP_000_PANE_TEXT_DESCRIPTION);
    
    m_TextBoxes[TEXTBOX_PlayerInfo1of4]   = helper.GetTextBoxPane(PANENAME_LIVE_MIN_UPP_000_PANE_TEXT_PLAYER_00);
    m_TextBoxes[TEXTBOX_PlayerInfo2of4]   = helper.GetTextBoxPane(PANENAME_LIVE_MIN_UPP_000_PANE_TEXT_PLAYER_01);
    m_TextBoxes[TEXTBOX_PlayerInfo3of4]   = helper.GetTextBoxPane(PANENAME_LIVE_MIN_UPP_000_PANE_TEXT_PLAYER_02);
    m_TextBoxes[TEXTBOX_PlayerInfo4of4]   = helper.GetTextBoxPane(PANENAME_LIVE_MIN_UPP_000_PANE_TEXT_PLAYER_03);
  }
*/

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
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  UpperView_Demo::SetViewMode_Evolving(const pml::pokepara::CoreParam* pCoreParam)                                                /**<  ～のようすが…                      */
{
  app::util::G2DUtil*   pG2DUtil = GetG2DUtil();
  gfl2::str::StrBuf*    pStrBuf  = pG2DUtil->GetTempStrBuf( 1 );

  pG2DUtil->SetRegisterPokeNickName(0, pCoreParam);
  pG2DUtil->GetStringExpand(pStrBuf, msg_shinka_01_01);

//  m_MessageWindowManipulator.SetFinishMode_None();
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
void  UpperView_Demo::SetViewMode_Evolved(const pml::pokepara::CoreParam* pCoreParam, const MonsNo after)                            /**<  ～は～にしんかした                  */
{
  app::util::G2DUtil*   pG2DUtil = GetG2DUtil();
  gfl2::str::StrBuf*    pStrBuf  = pG2DUtil->GetTempStrBuf( 1 );


  pG2DUtil->SetRegisterPokeNickName(0, pCoreParam);
  pG2DUtil->SetRegisterMonsNameNo(1, after);
  pG2DUtil->GetStringExpand(pStrBuf, msg_shinka_03_01);

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
void  UpperView_Demo::SetViewMode_EvolutionCancel(const pml::pokepara::CoreParam* pCoreParam)                                                /**<  ～のへんかがとまった                */
{
  app::util::G2DUtil*   pG2DUtil = GetG2DUtil();
  gfl2::str::StrBuf*    pStrBuf  = pG2DUtil->GetTempStrBuf( 1 );


  pG2DUtil->SetRegisterPokeNickName(0, pCoreParam);
  pG2DUtil->GetStringExpand(pStrBuf, msg_shinka_02_01);

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
void  UpperView_Demo::SetViewMode_WazaConfirm(const pml::pokepara::CoreParam* pCoreParam, const WazaNo wazaNo)                           /**<  ～は～をおぼえたい                  */
{
  app::util::G2DUtil*   pG2DUtil = GetG2DUtil();
  gfl2::str::StrBuf*    pStrBuf  = pG2DUtil->GetTempStrBuf( 1 );


  pG2DUtil->SetRegisterPokeNickName(0, pCoreParam);
  pG2DUtil->SetRegisterWazaName(1, wazaNo);
  pG2DUtil->GetStringExpand(pStrBuf, msg_shinka_07_01);

  m_MessageWindowManipulator.SetFinishMode_None();
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
void  UpperView_Demo::SetViewMode_NewWaza(const pml::pokepara::CoreParam* pCoreParam, const WazaNo wazaNo)                           /**<  ～は～をおぼえた                    */
{
  app::util::G2DUtil*   pG2DUtil = GetG2DUtil();
  gfl2::str::StrBuf*    pStrBuf  = pG2DUtil->GetTempStrBuf( 1 );

  pG2DUtil->SetRegisterPokeNickName(0, pCoreParam);
  pG2DUtil->SetRegisterWazaName(1, wazaNo);
  pG2DUtil->GetStringExpand(pStrBuf, msg_shinka_04_01);

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
void  UpperView_Demo::SetViewMode_NewWazaAndForget(const pml::pokepara::CoreParam* pCoreParam, const WazaNo oldWaza, const WazaNo newWaza)    /**<  ～は～をわすれたそして～をおぼえた  */
{
  app::util::G2DUtil*   pG2DUtil = GetG2DUtil();
  gfl2::str::StrBuf*    pStrBuf  = pG2DUtil->GetTempStrBuf( 1 );


  pG2DUtil->SetRegisterPokeNickName(0, pCoreParam);
  pG2DUtil->SetRegisterWazaName(1, oldWaza);
  pG2DUtil->SetRegisterWazaName(2, newWaza);
  pG2DUtil->GetStringExpand(pStrBuf, msg_shinka_05_01);

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
void  UpperView_Demo::SetViewMode_NewWazaCancel(const pml::pokepara::CoreParam* pCoreParam, const WazaNo waza)                             /**<  ～は～をおぼえずに                  */
{
  app::util::G2DUtil*   pG2DUtil = GetG2DUtil();
  gfl2::str::StrBuf*    pStrBuf  = pG2DUtil->GetTempStrBuf( 1 );


  pG2DUtil->SetRegisterPokeNickName(0, pCoreParam);
  pG2DUtil->SetRegisterWazaName(1, waza);
  pG2DUtil->GetStringExpand(pStrBuf, msg_shinka_06_01);

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



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
#if 0
void  UpperView_Demo::SetViewMode(const ViewMode viewMode)
{
  u32 animID  = UINT_MAX;


  switch(viewMode)
  {
    case  VIEWMODE_TournamentName:                animID  = LA_LIVE_LIVE_SLC_LOW_000_PATERN_00; break;  /**<  大会名のみ              */
    case  VIEWMODE_TournamentNameWithID:          animID  = LA_LIVE_LIVE_SLC_LOW_000_PATERN_01; break;  /**<  大会名 + ID情報         */
    case  VIEWMODE_TournamentNameWith2Line:       animID  = LA_LIVE_LIVE_SLC_LOW_000_PATERN_02; break;  /**<  大会名 + 2行情報        */
    case  VIEWMODE_TournamentNameWithPlayerInfo:  animID  = LA_LIVE_LIVE_SLC_LOW_000_PATERN_03; break;  /**<  大会名 + プレイヤー情報 */

    default:
      /*  nop  */
      break;
  }

  if(animID  != UINT_MAX)
  {
    GetG2DUtil()->StartAnime(0, animID);
  }
}
#endif


//------------------------------------------------------------------
/**
  *  @brief    mstxtに埋め込まれたコールバックタグのprocedure
  */
//------------------------------------------------------------------
bool  UpperView_Demo::UpdateMsgCallback(u16 arg)
{
  bool  isPause = false;


  switch(arg)
  {
    case 0:   // SEウェイト
      if(Sound::IsSEFinished(EVOLUTION_SEID_WazaWasure))
      {
        /*  nop  */
      }
      else
      {
        isPause = true;
      }
      break;

    case 1:   // MEウェイト
      if(Sound::IsMEFinished(EVOLUTION_MEID_WazaOboe))
      {
        Sound::StartBaseBGMFromME();
      }
      else
      {
        isPause = true;
      }
      break;

    case 3:   // 技マシン忘れ（ポカン）
      Sound::PlaySE(EVOLUTION_SEID_WazaWasure);
      break;

    case 4:   // 技を覚えた
      Sound::StartME(EVOLUTION_MEID_WazaOboe);
      break;


    default:
      GFL_PRINT("\"%s\"(%d): %s unknown arg %d\n", __FILE__, __LINE__, __FUNCTION__, arg);
      break;
  };



  return  isPause;
}


} /*  namespace Evolution */
} /*  namespace NetApp    */

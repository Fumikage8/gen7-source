//==============================================================================
/**
 * @file   LowerView_Selection.cpp
 * @date   2015/07/16 Thu. 18:53:31
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
#include  "../LiveMessageMenu.h"

#include  "./LowerView_Selection.h"


// =============================================================================
/**/
//==============================================================================
/*  local  */
namespace
{
  enum
  {
    BUTTON_Back,

    BUTTON_Small_0,
    BUTTON_Small_1,
    BUTTON_Small_2,

    BUTTON_Large_0,
    BUTTON_Large_1,

    BUTTON_MAX
  };


  /*  button  */
  void  CreateButton(app::util::Heap* pHeap, app::ui::UIResponder* pResponder, gfl2::lyt::LytWk* pLayoutWork, gfl2::lyt::LytMultiResID& rLytMultiResID)
  {
    app::util::GetPaneHelper  helper(pLayoutWork, &rLytMultiResID);
    
    app::ui::ButtonInfoEx buttonInfo[] =
    {
      {
        BUTTON_Back,
        helper.Clear().Push(PANENAME_LIVE_SLC_LOW_000_PANE_BTN_BACK).Peek(),
        helper.GetBoundingPane(PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00),
        LA_LIVE_LIVE_SLC_LOW_000__BTN_BACK_TOUCH,
        LA_LIVE_LIVE_SLC_LOW_000__BTN_BACK_RELEASE,
        LA_LIVE_LIVE_SLC_LOW_000__BTN_BACK_CANSEL,
        LA_LIVE_LIVE_SLC_LOW_000__BTN_BACK_TOUCH_RELEASE,
        LA_LIVE_LIVE_SLC_LOW_000__BTN_BACK_ACTIVE,
        LA_LIVE_LIVE_SLC_LOW_000__BTN_BACK_PASSIVE,
      },
      {
        BUTTON_Small_0,
        helper.Clear().Push(PANENAME_LIVE_SLC_LOW_000_PANE_BTNS_00).Peek(),
        helper.GetBoundingPane(PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00),
        LA_LIVE_LIVE_SLC_LOW_000__BTNS_00_TOUCH,
        LA_LIVE_LIVE_SLC_LOW_000__BTNS_00_RELEASE,
        LA_LIVE_LIVE_SLC_LOW_000__BTNS_00_CANSEL,
        LA_LIVE_LIVE_SLC_LOW_000__BTNS_00_TOUCH_RELEASE,
        LA_LIVE_LIVE_SLC_LOW_000__BTNS_00_ACTIVE,
        LA_LIVE_LIVE_SLC_LOW_000__BTNS_00_PASSIVE,
      },
      {
        BUTTON_Small_1,
        helper.Clear().Push(PANENAME_LIVE_SLC_LOW_000_PANE_BTNS_01).Peek(),
        helper.GetBoundingPane(PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00),
        LA_LIVE_LIVE_SLC_LOW_000__BTNS_01_TOUCH,
        LA_LIVE_LIVE_SLC_LOW_000__BTNS_01_RELEASE,
        LA_LIVE_LIVE_SLC_LOW_000__BTNS_01_CANSEL,
        LA_LIVE_LIVE_SLC_LOW_000__BTNS_01_TOUCH_RELEASE,
        LA_LIVE_LIVE_SLC_LOW_000__BTNS_01_ACTIVE,
        LA_LIVE_LIVE_SLC_LOW_000__BTNS_01_PASSIVE,
      },
      {
        BUTTON_Small_2,
        helper.Clear().Push(PANENAME_LIVE_SLC_LOW_000_PANE_BTNS_02).Peek(),
        helper.GetBoundingPane(PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00),
        LA_LIVE_LIVE_SLC_LOW_000__BTNS_02_TOUCH,
        LA_LIVE_LIVE_SLC_LOW_000__BTNS_02_RELEASE,
        LA_LIVE_LIVE_SLC_LOW_000__BTNS_02_CANSEL,
        LA_LIVE_LIVE_SLC_LOW_000__BTNS_02_TOUCH_RELEASE,
        LA_LIVE_LIVE_SLC_LOW_000__BTNS_02_ACTIVE,
        LA_LIVE_LIVE_SLC_LOW_000__BTNS_02_PASSIVE,
      },
      {
        BUTTON_Large_0,
        helper.Clear().Push(PANENAME_LIVE_SLC_LOW_000_PANE_BTNSEARCH_00).Peek(),
        helper.GetBoundingPane(PANENAME_COMMON_BTN_LOW_004_PANE_BOUND_00),
        LA_LIVE_LIVE_SLC_LOW_000__BTNSEARCH_00_TOUCH,
        LA_LIVE_LIVE_SLC_LOW_000__BTNSEARCH_00_RELEASE,
        LA_LIVE_LIVE_SLC_LOW_000__BTNSEARCH_00_CANSEL,
        LA_LIVE_LIVE_SLC_LOW_000__BTNSEARCH_00_TOUCH_RELEASE,
        LA_LIVE_LIVE_SLC_LOW_000__BTNSEARCH_00_ACTIVE,
        LA_LIVE_LIVE_SLC_LOW_000__BTNSEARCH_00_PASSIVE,
      },
      {
        BUTTON_Large_1,
        helper.Clear().Push(PANENAME_LIVE_SLC_LOW_000_PANE_BTNSEARCH_01).Peek(),
        helper.GetBoundingPane(PANENAME_COMMON_BTN_LOW_004_PANE_BOUND_00),
        LA_LIVE_LIVE_SLC_LOW_000__BTNSEARCH_01_TOUCH,
        LA_LIVE_LIVE_SLC_LOW_000__BTNSEARCH_01_RELEASE,
        LA_LIVE_LIVE_SLC_LOW_000__BTNSEARCH_01_CANSEL,
        LA_LIVE_LIVE_SLC_LOW_000__BTNSEARCH_01_TOUCH_RELEASE,
        LA_LIVE_LIVE_SLC_LOW_000__BTNSEARCH_01_ACTIVE,
        LA_LIVE_LIVE_SLC_LOW_000__BTNSEARCH_01_PASSIVE,
      },
    };

    pResponder->CreateButtonManager( pHeap, pLayoutWork, buttonInfo, GFL_NELEMS(buttonInfo) );

    /*  SE  */
    {
      app::tool::ButtonManager* pButtonManager  = pResponder->GetButtonManager();

      pButtonManager->SetButtonSelectSE(BUTTON_Small_0, NetApp::Live::SEID_DECIDE);
      pButtonManager->SetButtonSelectSE(BUTTON_Small_1, NetApp::Live::SEID_DECIDE);
      pButtonManager->SetButtonSelectSE(BUTTON_Small_2, NetApp::Live::SEID_DECIDE);
      pButtonManager->SetButtonSelectSE(BUTTON_Large_0, NetApp::Live::SEID_DECIDE);
      pButtonManager->SetButtonSelectSE(BUTTON_Large_1, NetApp::Live::SEID_DECIDE);

      pButtonManager->SetButtonSelectSE(BUTTON_Back,    NetApp::Live::SEID_CANCEL);
    }
  }
}


namespace NetApp  {
namespace Live    {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
LowerView_Selection::LowerView_Selection(LiveWork* pWork)
  : NetAppLib::UI::NetAppCursorView(pWork)
  , NetAppLib::UI::NetAppTrainerIconVisibilityControl(pWork->GetAppHeap()->GetDeviceHeap(), 1)
  , m_pWork(pWork)
  , m_pHandler(NULL)
  , m_ViewMode(VIEWMODE_NONE)
  , m_pLiveMessageMenu(NULL)
  , m_ReservedLayoutModeOnMessageFinished(LAYOUT_NONE)
  , m_bReservedLayoutModeAvtivateOnEOM(false)
  , m_pButtonEventArray(NULL)
  , m_PartyUtil(m_pWork)
  , m_IsAButtonEnable(false)
  , m_IsBButtonEnable(false)
{
  /*  create2d  */
  Create2DOneLayout(
    pWork
    , RESOURCE_Layout
    , LYTRES_LIVE_LIVE_SLC_LOW_000_BFLYT
    , LA_LIVE_LIVE_SLC_LOW_000___NUM
    , app::util::G2DUtil::SETUP_LOWER
    , &(pWork->GetMessageUtility())
    , LIVE_MESSAGE_GRAC_ID
  );


  app::util::G2DUtil*           pG2DUtil        = GetG2DUtil();
  gfl2::lyt::LytWk*             pLayoutWork     = pG2DUtil->GetLayoutWork(0);
  gfl2::lyt::LytMultiResID&     rLytMultiResID  = pG2DUtil->GetLayoutResourceID(0);
  gfl2::heap::HeapBase*         pHeapBase       = m_pWork->GetAppHeap()->GetDeviceHeap();
  app::util::GetPaneHelper      helper(pLayoutWork, &rLytMultiResID);


  /*  create button  */
  CreateButton(m_pWork->GetAppHeap(), this, pLayoutWork, rLytMultiResID);
  SetInputListener( this );

  /*  getpane  */
  {
    m_pTextBoxes[TEXTBOX_OptionButton_1]    = helper.Clear().Push(PANENAME_LIVE_SLC_LOW_000_PANE_BTNS_00).GetTextBoxPane(PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00);
    m_pTextBoxes[TEXTBOX_OptionButton_2]    = helper.Clear().Push(PANENAME_LIVE_SLC_LOW_000_PANE_BTNS_01).GetTextBoxPane(PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00);
    m_pTextBoxes[TEXTBOX_OptionButton_3]    = helper.Clear().Push(PANENAME_LIVE_SLC_LOW_000_PANE_BTNS_02).GetTextBoxPane(PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00);

    m_pTextBoxes[TEXTBOX_SearchButton_1]    = helper.Clear().Push(PANENAME_LIVE_SLC_LOW_000_PANE_BTNSEARCH_00).GetTextBoxPane(PANENAME_COMMON_BTN_LOW_004_PANE_TEXTBOX_00);
    m_pTextBoxes[TEXTBOX_SearchButton_2]    = helper.Clear().Push(PANENAME_LIVE_SLC_LOW_000_PANE_BTNSEARCH_01).GetTextBoxPane(PANENAME_COMMON_BTN_LOW_004_PANE_TEXTBOX_00);


    /*  LAYOUT_IDCard */
    helper.Clear().Push(PANENAME_LIVE_SLC_LOW_000_PANE_IDCARD);
    m_pTextBoxes[TEXTBOX_CARD_REMAIN_MATCH] = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXT_99_00);
    m_pTextBoxes[TEXTBOX_CARD_TOTAL_MATCH]  = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXT_99_01);
    m_pTextBoxes[TEXTBOX_CARD_WINS]         = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXT_99_02);
    m_pTextBoxes[TEXTBOX_CARD_LOSES]        = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXT_99_03);
    m_pTextBoxes[TEXTBOX_CARD_TEAMNAME]     = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXTBOX_TEAMNAME);

    m_pPicture[PICTURE_Mine_Card]           = helper.GetPicturePane(PANENAME_LIVE_PLT_000_PANE_PLAYERPHOTO);

    pG2DUtil->SetTextBoxPaneString(helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXTBOX_01), m_pWork->GetString(LIVE_MESSAGE_GRAC_ID, msg_lt_sys_07));
    pG2DUtil->SetTextBoxPaneString(helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXTBOX_02), m_pWork->GetString(LIVE_MESSAGE_GRAC_ID, msg_lt_sys_11));
    pG2DUtil->SetTextBoxPaneString(helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_TEXTBOX_03), m_pWork->GetString(LIVE_MESSAGE_GRAC_ID, msg_lt_sys_13));

    /*  trainericon  */
    {
      const Savedata::MyStatus& rMyStatus = *(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus());
      
      app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA iconObjectData;

      iconObjectData.ConvFromMyStatusIconData( rMyStatus.GetIcon() );

      helper.Clear();
      iconObjectData.pReplacePane             = m_pPicture[PICTURE_Mine_Card];
      iconObjectData.pSyncVisiblePane         = helper.GetPane(PANENAME_LIVE_SLC_LOW_000_PANE_NULL_ICONCTRL);
      iconObjectData.pInLoadVisibleChangePane = helper.GetPicturePane(PANENAME_LIVE_PLT_000_PANE_WINDOW_PLAYER_FRAME);
      iconObjectData.pTimerIconRootPane       = NULL;

      m_pWork->m_pTrainerIconUtil->SetIconObjectData( 3, iconObjectData );

      /*  iconctrl  */
      RegisterTrainerIconVisibilityControlPane(0, helper.Clear().GetPane(PANENAME_LIVE_SLC_LOW_000_PANE_NULL_ICONCTRL));
      ApplyTrainerIconVisibility();
    }
  }


  /*  PokePartyUtility  */
  {
    NetAppLib::UI::NetAppPokeparaUtility::RegisterInfo  info;


    info.pNameStringSource  = NULL;
    info.pLevelStringSource = m_pWork->GetString(LIVE_MESSAGE_GRAC_ID, msg_lt_sys_18);

    m_PartyUtil.Setup(1, this, 0);
    helper.Clear().Push(PANENAME_LIVE_SLC_LOW_000_PANE_IDCARD);

    info.pVisibilityControl     = helper.GetPane(PANENAME_LIVE_PLT_000_PANE_POKE_00);
    info.pIconPicture           = helper.Push(PANENAME_LIVE_PLT_000_PANE_POKEICON_00).GetPicturePane(PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON); helper.Pop();
    info.pTextBox_Level         = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_POKE_00);
    info.pGenderIconParts       = helper.GetPartsPane(PANENAME_LIVE_PLT_000_PANE_GENDER_00);
    info.genderIconAnim_Male    = LA_LIVE_LIVE_SLC_LOW_000__IDCARD__GENDER_00_ICON_MALE;
    info.genderIconAnim_Female  = LA_LIVE_LIVE_SLC_LOW_000__IDCARD__GENDER_00_ICON_FEMALE;
    m_PartyUtil.RegisterStatusResource(0, 0, info);

    info.pVisibilityControl     = helper.GetPane(PANENAME_LIVE_PLT_000_PANE_POKE_01);
    info.pIconPicture           = helper.Push(PANENAME_LIVE_PLT_000_PANE_POKEICON_01).GetPicturePane(PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON); helper.Pop();
    info.pTextBox_Level         = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_POKE_01);
    info.pGenderIconParts       = helper.GetPartsPane(PANENAME_LIVE_PLT_000_PANE_GENDER_01);
    info.genderIconAnim_Male    = LA_LIVE_LIVE_SLC_LOW_000__IDCARD__GENDER_01_ICON_MALE;
    info.genderIconAnim_Female  = LA_LIVE_LIVE_SLC_LOW_000__IDCARD__GENDER_01_ICON_FEMALE;
    m_PartyUtil.RegisterStatusResource(0, 1, info);

    info.pVisibilityControl     = helper.GetPane(PANENAME_LIVE_PLT_000_PANE_POKE_02);
    info.pIconPicture           = helper.Push(PANENAME_LIVE_PLT_000_PANE_POKEICON_02).GetPicturePane(PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON); helper.Pop();
    info.pTextBox_Level         = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_POKE_02);
    info.pGenderIconParts       = helper.GetPartsPane(PANENAME_LIVE_PLT_000_PANE_GENDER_02);
    info.genderIconAnim_Male    = LA_LIVE_LIVE_SLC_LOW_000__IDCARD__GENDER_02_ICON_MALE;
    info.genderIconAnim_Female  = LA_LIVE_LIVE_SLC_LOW_000__IDCARD__GENDER_02_ICON_FEMALE;
    m_PartyUtil.RegisterStatusResource(0, 2, info);

    info.pVisibilityControl     = helper.GetPane(PANENAME_LIVE_PLT_000_PANE_POKE_03);
    info.pIconPicture           = helper.Push(PANENAME_LIVE_PLT_000_PANE_POKEICON_03).GetPicturePane(PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON); helper.Pop();
    info.pTextBox_Level         = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_POKE_03);
    info.pGenderIconParts       = helper.GetPartsPane(PANENAME_LIVE_PLT_000_PANE_GENDER_03);
    info.genderIconAnim_Male    = LA_LIVE_LIVE_SLC_LOW_000__IDCARD__GENDER_03_ICON_MALE;
    info.genderIconAnim_Female  = LA_LIVE_LIVE_SLC_LOW_000__IDCARD__GENDER_03_ICON_FEMALE;
    m_PartyUtil.RegisterStatusResource(0, 3, info);

    info.pVisibilityControl     = helper.GetPane(PANENAME_LIVE_PLT_000_PANE_POKE_04);
    info.pIconPicture           = helper.Push(PANENAME_LIVE_PLT_000_PANE_POKEICON_04).GetPicturePane(PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON); helper.Pop();
    info.pTextBox_Level         = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_POKE_04);
    info.pGenderIconParts       = helper.GetPartsPane(PANENAME_LIVE_PLT_000_PANE_GENDER_04);
    info.genderIconAnim_Male    = LA_LIVE_LIVE_SLC_LOW_000__IDCARD__GENDER_04_ICON_MALE;
    info.genderIconAnim_Female  = LA_LIVE_LIVE_SLC_LOW_000__IDCARD__GENDER_04_ICON_FEMALE;
    m_PartyUtil.RegisterStatusResource(0, 4, info);

    info.pVisibilityControl     = helper.GetPane(PANENAME_LIVE_PLT_000_PANE_POKE_05);
    info.pIconPicture           = helper.Push(PANENAME_LIVE_PLT_000_PANE_POKEICON_05).GetPicturePane(PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON); helper.Pop();
    info.pTextBox_Level         = helper.GetTextBoxPane(PANENAME_LIVE_PLT_000_PANE_POKE_05);
    info.pGenderIconParts       = helper.GetPartsPane(PANENAME_LIVE_PLT_000_PANE_GENDER_05);
    info.genderIconAnim_Male    = LA_LIVE_LIVE_SLC_LOW_000__IDCARD__GENDER_05_ICON_MALE;
    info.genderIconAnim_Female  = LA_LIVE_LIVE_SLC_LOW_000__IDCARD__GENDER_05_ICON_FEMALE;
    m_PartyUtil.RegisterStatusResource(0, 5, info);
  }



  /*  messagemenu  */
  m_pLiveMessageMenu  = GFL_NEW(pHeapBase) LiveMessageMenu(m_pWork, this);
  m_pLiveMessageMenu->LiveMessageMenuSetup(this);
  AddSubView(m_pLiveMessageMenu);

  /*  buttonEvent  */
  m_pButtonEventArray = GFL_NEW(pHeapBase) IEventHandler::EventCode [BUTTON_MAX];
  ClearButtonEvent();


  /*  cursor  */
  CreateCursor(
    RESOURCE_LayoutCommonMenuCursor,
    0,
    0
  );
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
LowerView_Selection::~LowerView_Selection()
{
  UnregisterAllTrainerIconVisibilityControlPane();


  if(m_pLiveMessageMenu)  m_pLiveMessageMenu->RemoveFromSuperView();


  GFL_SAFE_DELETE_ARRAY(m_pButtonEventArray);
  GFL_SAFE_DELETE(m_pLiveMessageMenu);
  ReleaseCursor();
  Delete2D();
  DeleteGraphicsSystem();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Selection::Update(void)
{
  NetAppLib::UI::NetAppCursorView::Update();  /*  super  */
  m_PartyUtil.OnUpdate();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  LowerView_Selection::IsDrawing(void) const
{
  bool  isDrawing = false;

  isDrawing |= !m_PartyUtil.Cleanup();
  isDrawing |= NetAppLib::UI::NetAppCursorView::IsDrawing();

  return isDrawing;
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Selection::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  GetG2DUtil()->AddDrawableLytWkForOneFrame(m_pWork->GetAppRenderingManager(), displayNo, 0);

  /*  super  */
  NetAppLib::UI::NetAppCursorView::Draw(displayNo);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    動作モードの変更
  */
//------------------------------------------------------------------
void  LowerView_Selection::SetViewMode(const ViewMode viewMode)
{
  ClearButtonEvent();
  m_pButtonEventArray[BUTTON_Back]    = IEventHandler::EVENT_Exit;

  switch(viewMode)
  {
    /*
      メインメニュー
    */
    case  VIEWMODE_MainMenu_NoReplay:           /**<  メインメニュー：｛はじめる｝                */
      SetLayoutMode(LAYOUT_Option_1);
      m_pButtonEventArray[BUTTON_Small_0] = IEventHandler::EVENT_SelectedStart;
      SetTextBoxMessage(TEXTBOX_OptionButton_1, msg_lt_sel_01);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_Welcome);   /*  ライブ大会へようこそ  */
      break;

    case  VIEWMODE_MainMenu_Full:               /**<  メインメニュー：｛はじめる|前回の成績｝       */
      SetLayoutMode(LAYOUT_Option_2);
      m_pButtonEventArray[BUTTON_Small_0] = IEventHandler::EVENT_SelectedStart;
      m_pButtonEventArray[BUTTON_Small_1] = IEventHandler::EVENT_SelectedReplay;
      SetTextBoxMessage(TEXTBOX_OptionButton_1, msg_lt_sel_01);
      SetTextBoxMessage(TEXTBOX_OptionButton_2, msg_lt_sel_08);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_Welcome);   /*  ライブ大会へようこそ  */
      break;

    case  VIEWMODE_MainMenu_ReceivePlayerPass:  /**<  メインメニュー：｛選手証をうけとる｝        */
      SetLayoutModeAfterOnMessageFinish(LAYOUT_Message, LAYOUT_Option_2, true);
      SetTextBoxMessage(TEXTBOX_OptionButton_1, msg_lt_sel_09);   /*  赤外線で  */
      SetTextBoxMessage(TEXTBOX_OptionButton_2, msg_lt_sel_10);   /*  QRで      */
      m_pButtonEventArray[BUTTON_Small_0] = IEventHandler::EVENT_ConfirmReceivePlayerPass_Ir;
      m_pButtonEventArray[BUTTON_Small_1] = IEventHandler::EVENT_ConfirmReceivePlayerPass_QR;
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_NeedPlayerPass);   /*  選手証が必要  */
      break;

    /*
      選手証受信
    */
    case  VIEWMODE_ReceivePlayerPass_ReadyIrPort:
      SetLayoutMode(LAYOUT_MessageWithBack);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_ReadyIrPortForReceiving);   /*  赤外線をあわせて */
      break;

    case  VIEWMODE_ReceivePlayerPass_Receiving:       /**<  選手証受信：受信しています  */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_ReceivingPlayerPass);   /*  受信しています  */
      break;

    case  VIEWMODE_ReceivePlayerPass_Received:        /**<  選手証受信：受信完了        */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_ReceivedPlayerPass);            /**<  受信しました        */
      break;


    case  VIEWMODE_ReceivePlayerPass_ReceiveCancel:   /**<  選手証受信：受信キャンセル  */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_ReceivingPlayerPassCancel);     /**<  キャンセルしました  */
      break;


    case  VIEWMODE_ReceivePlayerPass_ReceiveFailed:   /**<  選手証受信：受信失敗        */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_ReceivingPlayerPassFailed);     /**<  失敗しました        */
      break;

    case  VIEWMODE_ReceivePlayerPass_HaveParticipated:   /**<  選手証受信：すでに参加済み  */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_ReceivingPlayerPasshaveParticipated);     /**<  すでに参加済み         */
      break;

    case  VIEWMODE_ReceivePlayerPass_Rejected:              /**<  選手証受信：参加できない    */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_ReceivingPlayerPassRejected);                /**<  参加できません         */
      break;

    case  VIEWMODE_ReceivePlayerPass_Saving:                  /**<  選手証受信：セーブしています    */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_Common_Saving);
      break;


    /*
      チーム登録
    */
    case  VIEWMODE_Registration_ConfirmRegistration:        /**<  チーム登録：｛登録する｝                        */
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_Registration_ConfirmRegistration);          /**<  チームを登録してください */

      SetLayoutMode(LAYOUT_Option_1);
      m_pButtonEventArray[BUTTON_Small_0] = IEventHandler::EVENT_Registration_ConfirmRegistration_Start;
      SetTextBoxMessage(TEXTBOX_OptionButton_1, msg_ymadd_02);

      break;

    case  VIEWMODE_Registration_ConfirmCancel:              /**<  チーム登録：登録を中止する？                    */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_Registration_ConfirmCancel);                /**<  チーム登録：登録を中止する？                    */
      break;

    case  VIEWMODE_Registration_ConfirmDeregistration:      /**<  チーム登録：登録解除する？                      */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_Registration_ConfirmDeregistration);        /**<  チーム登録：登録解除する？                      */
      break;

    case  VIEWMODE_Registration_ConfirmDeregistration2:     /**<  チーム登録：復帰できなくなるが登録解除する？    */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_Registration_ConfirmDeregistration2);       /**<  チーム登録：復帰できなくなるが登録解除する？    */
      break;

    case  VIEWMODE_Registration_CancelRegistration:         /**<  チーム登録：登録キャンセル                  */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_Registration_CancelRegistration);           /**<  チーム登録：登録キャンセル                  */
      break;

    case  VIEWMODE_Registration_DeregistrationDone:         /**<  チーム登録：登録解除キャンセルした              */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_Registration_DeregistrationDone);           /**<  チーム登録：登録解除キャンセルした              */
      break;

    case  VIEWMODE_Registration_Saving:
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_Registration_Saving);           /**<  チーム登録：登録解除キャンセルした              */
      break;


    case  VIEWMODE_CheckRemainingMatches_Completed:         /**<  残試合数：達成  */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_CheckRemainingMatches_Completed);           /**<  残試合確認：達成  */
      break;

    case  VIEWMODE_CheckRemainingMatches_NotComplete:       /**<  残試合数：未達  */
      SetLayoutMode(LAYOUT_Message);
      break;

    case  VIEWMODE_CheckRemainingMatches_Saving:              /**<  残試合数：セーブ中        */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_Common_Saving);
      break;

    case  VIEWMODE_MatchMenu_Full:                          /**<  大会メニュー：｛挑戦する|振り返る|解除する｝  */
      SetLayoutMode(LAYOUT_Option_3);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_MatchMenu_WhatWantToDo);          /**<  何をしますか？ */
      m_pButtonEventArray[BUTTON_Small_0] = IEventHandler::EVENT_MatchMenu_SelectedStart;
      m_pButtonEventArray[BUTTON_Small_1] = IEventHandler::EVENT_MatchMenu_SelectedReplay;
      m_pButtonEventArray[BUTTON_Small_2] = IEventHandler::EVENT_MatchMenu_SelectedDeregistration;
      SetTextBoxMessage(TEXTBOX_OptionButton_1, msg_lt_sel_04);   /*  挑戦する  */
      SetTextBoxMessage(TEXTBOX_OptionButton_2, msg_lt_sel_05);   /*  振り返る  */
      SetTextBoxMessage(TEXTBOX_OptionButton_3, msg_lt_sel_06);   /*  解除する  */
      break;

    case  VIEWMODE_MatchMenu_ReplayOnly:                    /**<  大会メニュー：｛振り返る｝           */
      SetLayoutMode(LAYOUT_Option_1);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_MatchMenu_WhatWantToDo);          /**<  何をしますか？ */
      m_pButtonEventArray[BUTTON_Small_0] = IEventHandler::EVENT_MatchMenu_SelectedReplay;
      SetTextBoxMessage(TEXTBOX_OptionButton_1, msg_lt_sel_05);   /*  振り返る  */
      break;

    case  VIEWMODE_MatchMenu_NoReplayData:                   /**<  大会メニュー： 対戦の　記録がみつかりませんでした▼          */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_MatchMenu_NoReplayData); 
      break;

    case  VIEWMODE_ReplayMatch_CheckingSDCard:              /**<  対戦リプレイ：SDカードを確認中      */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_ReplayMatch_CheckingSDCard);                /**<  対戦リプレイ：SDカードを確認中      */
      break;

    case  VIEWMODE_ReplayMatch_ConfirmReplay:               /**<  対戦リプレイ：この対戦を振り返る？  */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_ReplayMatch_ConfirmReplay);                 /**<  対戦リプレイ：この対戦を振り返る？  */
      break;

    case  VIEWMODE_ReplayMatch_NoData:                      /**<  対戦リプレイ：データがなかった      */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_ReplayMatch_NoData);
      break;

    case  VIEWMODE_ReplayMatch_BrokenData:                  /**<  対戦リプレイ：データが壊れている    */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_ReplayMatch_BrokenData);                    /**<  対戦リプレイ：データが壊れている    */
      break;


    case  VIEWMODE_Deregistration_ConfirmUnregister:          /**<  登録解除：復帰できませんが解除しますか？  */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_Deregistration_ConfirmUnregister);          /**<  登録解除：復帰できませんが解除しますか？  */
      break;

    case  VIEWMODE_Deregistration_ConfirmUnregister2:         /**<  登録解除：本当に解除しますか？  */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_Deregistration_ConfirmUnregister2);         /**<  登録解除：本当に解除しますか？  */
      break;

    case  VIEWMODE_Deregistration_Saving:                     /**<  登録解除：セーブしています  */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_Deregistration_Saving);                     /**<  登録解除：セーブしています  */
      break;

    case  VIEWMODE_Deregistration_DeregistrationInformation:  /**<  登録解除：解除しました  */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_Deregistration_DeregistrationInformation);  /**<  登録解除：解除しました  */
      break;

    case  VIEWMODE_Matching_Searching:                        /**<  マッチング：対戦相手を探しています              */
      SetLayoutMode(LAYOUT_IDCard);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_Matching_Searching);
      break;

    case  VIEWMODE_Matching_Connecting:                       /**<  マッチング：通信中  */
      SetLayoutMode(LAYOUT_IDCard);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_Matching_Connecting);                       /**<  マッチング：通信中  */
      break;

    case  VIEWMODE_Matching_SendDataToCheatCheckROM:          /**<  マッチング：不正チェックROMへのデータ送信完了  */
      SetLayoutMode(LAYOUT_IDCard);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_Matching_SendDataToCheatCheckROM);          /**<  マッチング：不正チェックROMへのデータ送信完了  */
      break;

    case  VIEWMODE_Matching_ConfirmCancel:                    /**<  マッチング：通信をキャンセルしますか？         */
      SetLayoutMode(LAYOUT_IDCard);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_Matching_ConfirmCancel);                    /**<  マッチング：通信をキャンセルしますか？         */
      break;

    case  VIEWMODE_Matching_Cancel:                           /**<  マッチング：通信をキャンセルしています         */
      SetLayoutMode(LAYOUT_IDCard);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_Matching_Cancel);                           /**<  マッチング：通信をキャンセルしています         */
      break;

    case  VIEWMODE_Matching_RegulationNotMatched:             /**<  マッチング：レギュレーション不一致             */
      SetLayoutMode(LAYOUT_IDCard);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_Matching_RegulationNotMatched);             /**<  マッチング：レギュレーション不一致             */
      break;

    case  VIEWMODE_Matching_WaitingInstruction:               /**<  マッチング：スタッフ指示待ち                   */
      SetLayoutMode(LAYOUT_IDCard);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_Matching_WaitingInstruction);               /**<  マッチング：スタッフ指示待ち                   */
      break;

    case  VIEWMODE_Matching_Matching:                         /**<  マッチング：通信中（マッチング）               */
      SetLayoutMode(LAYOUT_IDCard);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_Matching_Matching);                         /**<  マッチング：通信中（マッチング）               */
      break;


    case  VIEWMODE_MatchResult_Win:                           /**<  対戦結果：勝利            */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_MatchResult_Win);                           /**<  対戦結果：勝利            */
      break;

    case  VIEWMODE_MatchResult_Win_Decision:                  /**<  対戦結果：判定勝ち        */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_MatchResult_Win_Decision);                  /**<  対戦結果：判定勝ち        */
      break;

    case  VIEWMODE_MatchResult_Win_Timeout:                   /**<  対戦結果：時間勝利        */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_MatchResult_Win_Timeout);                   /**<  対戦結果：時間勝利        */
      break;

    case  VIEWMODE_MatchResult_Win_Surrender:                 /**<  対戦結果：降参による勝利  */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_MatchResult_Win_Surrender);                 /**<  対戦結果：降参による勝利  */
      break;

    case  VIEWMODE_MatchResult_Draw:                          /**<  対戦結果：ひきわけ        */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_MatchResult_Draw);                          /**<  対戦結果：ひきわけ        */
      break;

    case  VIEWMODE_MatchResult_Saving:                        /**<  対戦結果： セーブ中       */
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_MatchResult_Saving);                        /**<  対戦結果：セーブ中        */
      break;

    /*  セーブ失敗：SDFull  */
    case  VIEWMODE_MatchResult_SaveFailed_SDFull:
      SetLayoutMode(LAYOUT_Message);
      m_pLiveMessageMenu->Show(LiveMessageMenu::MODE_MatchResult_SaveFailed_SDFull);
      break;

    default:
      /*  nop  */
      break;
  }

  m_ViewMode  = viewMode;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_Selection::UpdateIDCard(void)
{
  app::util::G2DUtil*                           pG2DUtil    = GetG2DUtil();
  APP_PARAM*                                    pAppParam   = m_pWork->m_pAppParam;
  Regulation*                                   pRegulation = pAppParam->m_pSavedRegulation;
  const Savedata::LiveMatchData::LIVE_RESULTS*  pResults    = pAppParam->m_pLiveMatchData->GetLiveResults();
  const u32                                     digits      = 2;
  const print::NumberDispType                   dispType    = print::NUM_DISP_SPACE;

  m_PartyUtil.SetPokeParty(0, pAppParam->m_pMyBattleParty->GetPokeParty());

  pG2DUtil->SetTextBoxPaneNumber(m_pTextBoxes[TEXTBOX_CARD_REMAIN_MATCH], m_pWork->GetString(LIVE_MESSAGE_GRAC_ID, msg_lt_sys_10), pResults->battle_num,  digits, 0, dispType, print::NUM_CODE_DEFAULT);
  pG2DUtil->SetTextBoxPaneNumber(m_pTextBoxes[TEXTBOX_CARD_TOTAL_MATCH],  m_pWork->GetString(LIVE_MESSAGE_GRAC_ID, msg_lt_sys_08), pRegulation->GetValueParam(Regulation::BATTLE_NUM), digits, 0, dispType, print::NUM_CODE_DEFAULT);
  pG2DUtil->SetTextBoxPaneNumber(m_pTextBoxes[TEXTBOX_CARD_WINS],         m_pWork->GetString(LIVE_MESSAGE_GRAC_ID, msg_lt_sys_12), pResults->win_num,     digits, 0, dispType, print::NUM_CODE_DEFAULT);
  pG2DUtil->SetTextBoxPaneNumber(m_pTextBoxes[TEXTBOX_CARD_LOSES],        m_pWork->GetString(LIVE_MESSAGE_GRAC_ID, msg_lt_sys_14), pResults->lose_num,    digits, 0, dispType, print::NUM_CODE_DEFAULT);


#if 0
  rWord.RegisterWord(0, pAppParam->m_pMyBattleParty->GetTeamName());
  pG2DUtil->SetTextBoxPaneStringExpand(m_TextBoxes[TEXTBOX_CARD_TEAMNAME],  msg_lt_sys_15);
#else
  pG2DUtil->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_CARD_TEAMNAME],  pAppParam->m_pMyBattleParty->GetTeamName());
#endif



}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Selection::SetLayoutModeAfterOnMessageFinish(const LayoutMode layoutModeImmediate, const LayoutMode reserveLayoutMode, const bool isActivateOnEOM)
{
  SetLayoutMode(layoutModeImmediate);
  m_ReservedLayoutModeOnMessageFinished = reserveLayoutMode;
  m_bReservedLayoutModeAvtivateOnEOM    = isActivateOnEOM;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    MessageMenuからの通知
  */
//------------------------------------------------------------------
void  LowerView_Selection::MessageMenuView_OnEvent(const LiveMessageMenu::IEventHandler::EventCode event)
{
  switch(event)
  {
    case  LiveMessageMenu::IEventHandler::EVENT_OnEOM:            /**<  メッセージ表示が終端に到達                                  */
      if(m_bReservedLayoutModeAvtivateOnEOM)
      {
        SetLayoutMode(m_ReservedLayoutModeOnMessageFinished);
        m_ReservedLayoutModeOnMessageFinished = LAYOUT_NONE;
      }
      Event(IEventHandler::EVENT_MessageFinish);
      break;

    case  LiveMessageMenu::IEventHandler::EVENT_OnFinish:         /**<  EOM後、ユーザの入力まちなどまでが完了した                   */
/*
  一律▼とする
*/
//      SetLayoutMode(m_ReservedLayoutModeOnMessageFinished);
//      m_ReservedLayoutModeOnMessageFinished = LAYOUT_NONE;
//      Event(IEventHandler::EVENT_MessageFinish);
      break;

    case  LiveMessageMenu::IEventHandler::EVENT_OnSelectedItem:   /**<  リスト選択でItemが選択された: GetSelectedItemIndex()で取得  */
      break;

    case  LiveMessageMenu::IEventHandler::EVENT_OnSelectedYes:    /**<  Yes/NoモードでYesが選択された                               */
    {
           if(m_ViewMode == VIEWMODE_Registration_ConfirmCancel)          Event(IEventHandler::EVENT_Registration_ConfirmCancel_Yes);           /**<  チーム登録：登録を中止する？                    */
      else if(m_ViewMode == VIEWMODE_Registration_ConfirmDeregistration)  Event(IEventHandler::EVENT_Registration_ConfirmDeregistration_Yes);   /**<  チーム登録：登録解除する？                      */
      else if(m_ViewMode == VIEWMODE_Registration_ConfirmDeregistration2) Event(IEventHandler::EVENT_Registration_ConfirmDeregistration2_Yes);  /**<  チーム登録：復帰できなくなるが登録解除する？    */
      else if(m_ViewMode == VIEWMODE_ReplayMatch_ConfirmReplay)           Event(IEventHandler::EVENT_ReplayMatch_Replay_Yes);                   /**<  対戦リプレイ：この対戦を振り返る？  */
      else if(m_ViewMode == VIEWMODE_Deregistration_ConfirmUnregister)    Event(IEventHandler::EVENT_Deregistration_ConfirmUnregister_Yes);     /**<  登録解除：復帰できませんが解除しますか？  */
      else if(m_ViewMode == VIEWMODE_Deregistration_ConfirmUnregister2)   Event(IEventHandler::EVENT_Deregistration_ConfirmUnregister2_Yes);    /**<  登録解除：本当に解除しますか？  */
      else if(m_ViewMode == VIEWMODE_Matching_ConfirmCancel)              Event(IEventHandler::EVENT_Matching_ConfirmCancel_Yes);               /**<  マッチング：通信をキャンセルしますか？         */
    }
      break;

    case  LiveMessageMenu::IEventHandler::EVENT_OnSelectedNo:     /**<  Yes/NoモードでNoが選択された                                */
    {
           if(m_ViewMode == VIEWMODE_Registration_ConfirmCancel)          Event(IEventHandler::EVENT_Registration_ConfirmCancel_No);            /**<  チーム登録：登録を中止する？                    */
      else if(m_ViewMode == VIEWMODE_Registration_ConfirmDeregistration)  Event(IEventHandler::EVENT_Registration_ConfirmDeregistration_No);    /**<  チーム登録：登録解除する？                      */
      else if(m_ViewMode == VIEWMODE_Registration_ConfirmDeregistration2) Event(IEventHandler::EVENT_Registration_ConfirmDeregistration2_No);   /**<  チーム登録：復帰できなくなるが登録解除する？    */
      else if(m_ViewMode == VIEWMODE_ReplayMatch_ConfirmReplay)           Event(IEventHandler::EVENT_ReplayMatch_Replay_No);                    /**<  対戦リプレイ：この対戦を振り返る？  */
      else if(m_ViewMode == VIEWMODE_Deregistration_ConfirmUnregister)    Event(IEventHandler::EVENT_Deregistration_ConfirmUnregister_No);      /**<  登録解除：復帰できませんが解除しますか？  */
      else if(m_ViewMode == VIEWMODE_Deregistration_ConfirmUnregister2)   Event(IEventHandler::EVENT_Deregistration_ConfirmUnregister2_No);     /**<  登録解除：本当に解除しますか？  */
      else if(m_ViewMode == VIEWMODE_Matching_ConfirmCancel)              Event(IEventHandler::EVENT_Matching_ConfirmCancel_No);                /**<  マッチング：通信をキャンセルしますか？         */
    }
      break;

    default:
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult  LowerView_Selection::OnLayoutPaneEvent( const u32 buttonID )
{
  Event(m_pButtonEventArray[buttonID]);

  return app::ui::UIInputListener::DISABLE_ACCESS;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult  LowerView_Selection::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  app::ui::UIInputListener::ListenerResult resultSuper = ENABLE_ACCESS;


  if( IsButtonAnimation() )
  {
    return DISABLE_ACCESS;
  }
  /*
    shortcut
  */
  if(m_IsAButtonEnable)
  {
    if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
    {
      Event(IEventHandler::EVENT_PushA);
      return app::ui::UIInputListener::DISABLE_ACCESS;
    }
  }
  else if(m_IsBButtonEnable)
  {
    if(pButton->IsTrigger(gfl2::ui::BUTTON_B))
    {
      Event(IEventHandler::EVENT_Exit);
      return app::ui::UIInputListener::DISABLE_ACCESS;
    }
  }



  /*  super  */
  /*  cursorViewのショートカット機能を後で処理する  */
  return NetAppLib::UI::NetAppCursorView::OnKeyAction(pButton, pKey, pStick);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Selection::SetTextBoxMessage(const TextBoxID textBoxID, const u32 messageID)
{
  SetTextboxPaneMessage( m_pTextBoxes[textBoxID], messageID );
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Selection::SetLayoutMode(const LayoutMode layoutMode)
{
  SetAllTrainerIconVisibilityInternal(false);
  switch(layoutMode)
  {
    case  LAYOUT_Option_1:
    {
      const u32 buttonIDs[] = {
        BUTTON_Small_0,
      };

      SetCursorVisible(true);
      GetG2DUtil()->StartAnime(0, LA_LIVE_LIVE_SLC_LOW_000_PATERN_00);
      GetG2DUtil()->Update2D();
      ApplyButtonAndCursor(buttonIDs, GFL_NELEMS(buttonIDs));
      SetButtonEnable(BUTTON_Back, true, false);
    }
      break;

    case  LAYOUT_Option_2:
    {
      const u32 buttonIDs[] = {
        BUTTON_Small_0,
        BUTTON_Small_1,
      };

      SetCursorVisible(true);
      GetG2DUtil()->StartAnime(0, LA_LIVE_LIVE_SLC_LOW_000_PATERN_01);
      GetG2DUtil()->Update2D();
      ApplyButtonAndCursor(buttonIDs, GFL_NELEMS(buttonIDs));
      SetButtonEnable(BUTTON_Back, true, false);
    }
      break;

    case  LAYOUT_Option_3:
    {
      const u32 buttonIDs[] = {
        BUTTON_Small_0,
        BUTTON_Small_1,
        BUTTON_Small_2,
      };

      SetCursorVisible(true);
      GetG2DUtil()->StartAnime(0, LA_LIVE_LIVE_SLC_LOW_000_PATERN_02);
      GetG2DUtil()->Update2D();
      ApplyButtonAndCursor(buttonIDs, GFL_NELEMS(buttonIDs));
      SetButtonEnable(BUTTON_Back, true, false);
    }
      break;

    case  LAYOUT_Search_1:
    {
      const u32 buttonIDs[] = {
        BUTTON_Large_0,
      };

      SetCursorVisible(true);
      GetG2DUtil()->StartAnime(0, LA_LIVE_LIVE_SLC_LOW_000_PATERN_03);
      GetG2DUtil()->Update2D();
      ApplyButtonAndCursor(buttonIDs, GFL_NELEMS(buttonIDs));
      SetButtonEnable(BUTTON_Back, true, false);
    }
      break;

    case  LAYOUT_Search_2:
    {
      const u32 buttonIDs[] = {
        BUTTON_Large_0,
        BUTTON_Large_1,
      };

      SetCursorVisible(true);
      GetG2DUtil()->StartAnime(0, LA_LIVE_LIVE_SLC_LOW_000_PATERN_04);
      GetG2DUtil()->Update2D();
      ApplyButtonAndCursor(buttonIDs, GFL_NELEMS(buttonIDs));
      SetButtonEnable(BUTTON_Back, true, false);
    }
      break;

    case  LAYOUT_IDCard:
      SetTrainerIconVisibilityInternal(0, true);
      UpdateIDCard();
      SetCursorVisible(false);
      ApplyButtonAndCursor(NULL, 0);
      GetG2DUtil()->StartAnime(0, LA_LIVE_LIVE_SLC_LOW_000_PATERN_05);
      GetG2DUtil()->Update2D();
      break;

    case  LAYOUT_Message:
      SetCursorVisible(false);
      GetG2DUtil()->StartAnime(0, LA_LIVE_LIVE_SLC_LOW_000_PATERN_06);
      GetG2DUtil()->Update2D();
      ApplyButtonAndCursor(NULL, 0);
      break;

    case  LAYOUT_MessageWithBack:
    {
      SetCursorVisible(false);
      GetG2DUtil()->StartAnime(0, LA_LIVE_LIVE_SLC_LOW_000_PATERN_07);
      GetG2DUtil()->Update2D();
      ApplyButtonAndCursor(NULL, 0);
      SetButtonEnable(BUTTON_Back, true, false);
    }
      break;


    case  LAYOUT_NONE:  /*  through  */
    default:
      /*  nop  */
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
void  LowerView_Selection::ApplyButtonAndCursor(const u32 buttonIDs[], const u32 idNum)
{

  /*  一旦全てのボタンを無効にした後、使用するもののみ有効化  */
  for(u32 num = 0; num < BUTTON_MAX; ++num)
  {
    SetButtonEnable(num, false, false);
  }
//  ReleaseCursor();

  if(idNum)
  {
    gfl2::lyt::LytPane*       cursorPosPanes[BUTTON_MAX];
    gfl2::lyt::LytPane*       curPosPaneTable[BUTTON_MAX];
    app::util::GetPaneHelper  helper(GetG2DUtil()->GetLayoutWork(0), &(GetG2DUtil()->GetLayoutResourceID(0)));

    {
      cursorPosPanes[BUTTON_Back]     = NULL;
      cursorPosPanes[BUTTON_Small_0]  = helper.Clear().Push(PANENAME_LIVE_SLC_LOW_000_PANE_BTNS_00).GetPane(PANENAME_COMMON_BTN_LOW_012_PANE_NULL_CUR);
      cursorPosPanes[BUTTON_Small_1]  = helper.Clear().Push(PANENAME_LIVE_SLC_LOW_000_PANE_BTNS_01).GetPane(PANENAME_COMMON_BTN_LOW_012_PANE_NULL_CUR);
      cursorPosPanes[BUTTON_Small_2]  = helper.Clear().Push(PANENAME_LIVE_SLC_LOW_000_PANE_BTNS_02).GetPane(PANENAME_COMMON_BTN_LOW_012_PANE_NULL_CUR);
      cursorPosPanes[BUTTON_Large_0]  = helper.Clear().Push(PANENAME_LIVE_SLC_LOW_000_PANE_BTNSEARCH_00).GetPane(PANENAME_COMMON_BTN_LOW_004_PANE_NULL_CUR);
      cursorPosPanes[BUTTON_Large_1]  = helper.Clear().Push(PANENAME_LIVE_SLC_LOW_000_PANE_BTNSEARCH_00).GetPane(PANENAME_COMMON_BTN_LOW_004_PANE_NULL_CUR);
    }

    for(u32 index = 0; index < idNum; ++index)
    {
      curPosPaneTable[index] = cursorPosPanes[buttonIDs[index]];
      SetButtonEnable(buttonIDs[index], true, false);
    }
/*
    CreateCursor(
      RESOURCE_LayoutCommonMenuCursor,
      0,
      0,
      curPosPaneTable,
      buttonIDs,
      (s32)idNum
    );
*/
    SetCusorPosPanes(curPosPaneTable, buttonIDs, (s32)idNum);

    RegisterShortcut_B(BUTTON_Back);
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
void  LowerView_Selection::Event(const IEventHandler::EventCode event)
{
  if(m_pHandler && (event != IEventHandler::EVENT_NONE))
  {
    m_pHandler->LowerView_Selection_OnEvent(event);
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
void  LowerView_Selection::ClearButtonEvent(void)
{
  for(u32 index = 0; index < BUTTON_MAX; ++index)
  {
    m_pButtonEventArray[index]  = IEventHandler::EVENT_NONE;
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
void LowerView_Selection::OnSetInputEnabled( bool isEnable )
{
  m_pLiveMessageMenu->SetInputEnabled(isEnable);
}



} /*  namespace Live    */
} /*  namespace NetApp  */

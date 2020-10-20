
//==============================================================================
/**
 * @file   LowerView_Selection.cpp
 * @date   2015/07/31 Fri. 16:07:44
 * @author muto_yu
*  @brief  ふしぎなおくりもの 項目選択 下画面
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <AppLib/include/Tool/app_tool_PaneList.h>

#include  "AppLib/include/Util/app_util_GetPaneHelper.h"

#include  "../ApplicationSystem/GiftWork.h"
#include  "../GiftResource.h"
#include  "./LowerView_Selection.h"



// =============================================================================
/**/
//==============================================================================
/*
  local
*/
namespace
{
  /**
    ButtonID
  */
  enum  {
    BUTTON_L00,   /**<  大  */
    BUTTON_L01,   /**<  大  */
    BUTTON_L02,   /**<  大  */

    BUTTON_S00,   /**<  小  */
    BUTTON_S01,   /**<  小  */
    BUTTON_S02,   /**<  小  */
    BUTTON_S03,   /**<  小  */
    BUTTON_S04,   /**<  小  */

    BUTTON_Back,  /**<  戻る  */
    
    BUTTON_MAX
  };


  /*
    ボタンのグルーピング
  */
  static const u32  ButtonGroup_Large[] = {
    BUTTON_L00,   /**<  大  */
    BUTTON_L01,   /**<  大  */
    BUTTON_L02,   /**<  大  */
    BUTTON_Back,  /**<  戻る  */
  };

  static const u32  ButtonGroup_ReceivingMethod[] = {
    BUTTON_S00,   /**<  小  */
    BUTTON_S01,   /**<  小  */
    BUTTON_S02,   /**<  小  */
    BUTTON_S03,   /**<  小  */
//    BUTTON_S04,   /**<  小  */
    BUTTON_Back,  /**<  戻る  */
  };


  /*  button  */
  void  CreateButton(app::util::Heap* pHeap, app::ui::UIResponder* pResponder, gfl2::lyt::LytWk* pLayoutWork, gfl2::lyt::LytMultiResID& rLytMultiResID)
  {
    app::util::GetPaneHelper  helper(pLayoutWork, &rLytMultiResID);

    const app::ui::ButtonInfoEx   buttonInfoEx[]  =
    {
      /*  L  */
      {
        BUTTON_L00,                                                             //  app::tool::ButtonManager::ButtonId button_id;
        helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONL_00).Peek(),  //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(PANENAME_GIFT_BTN_LOW_002_PANE_BOUND_00),        //  gfl2::lyt::LytPane* bound_pane;
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONL_00_TOUCH,           
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONL_00_RELEASE,         
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONL_00_CANSEL,          
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONL_00_TOUCH_RELEASE,   
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONL_00_ACTIVE,          
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONL_00_PASSIVE,
      },
      {
        BUTTON_L01,                                                             //  app::tool::ButtonManager::ButtonId button_id;
        helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONL_01).Peek(),  //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(PANENAME_GIFT_BTN_LOW_002_PANE_BOUND_00),        //  gfl2::lyt::LytPane* bound_pane;
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONL_01_TOUCH,           
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONL_01_RELEASE,         
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONL_01_CANSEL,          
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONL_01_TOUCH_RELEASE,   
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONL_01_ACTIVE,          
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONL_01_PASSIVE,
      },
      {
        BUTTON_L02,                                                             //  app::tool::ButtonManager::ButtonId button_id;
        helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONL_02).Peek(),  //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(PANENAME_GIFT_BTN_LOW_002_PANE_BOUND_00),        //  gfl2::lyt::LytPane* bound_pane;
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONL_02_TOUCH,           
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONL_02_RELEASE,         
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONL_02_CANSEL,          
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONL_02_TOUCH_RELEASE,   
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONL_02_ACTIVE,          
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONL_02_PASSIVE,
      },

      /*  S  */
      /*  受信方法選択時は2つ目のボタンから使用する  */
      {
        BUTTON_S00,                                           
        helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_01).Peek(),  //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(PANENAME_GIFT_BTN_LOW_003_PANE_BOUND_00),        //  gfl2::lyt::LytPane* bound_pane;
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_01_TOUCH,
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_01_RELEASE,
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_01_CANSEL,
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_01_TOUCH_RELEASE,
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_01_ACTIVE,
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_01_PASSIVE,
      },
      {
        BUTTON_S01,                                           
        helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_02).Peek(),  //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(PANENAME_GIFT_BTN_LOW_003_PANE_BOUND_00),        //  gfl2::lyt::LytPane* bound_pane;
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_02_TOUCH,
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_02_RELEASE,
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_02_CANSEL,
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_02_TOUCH_RELEASE,
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_02_ACTIVE,
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_02_PASSIVE,
      },
      {
        BUTTON_S02,                                           
        helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_03).Peek(),  //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(PANENAME_GIFT_BTN_LOW_003_PANE_BOUND_00),        //  gfl2::lyt::LytPane* bound_pane;
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_03_TOUCH,
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_03_RELEASE,
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_03_CANSEL,
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_03_TOUCH_RELEASE,
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_03_ACTIVE,
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_03_PASSIVE,
      },
      {
        BUTTON_S03,                                           
        helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_04).Peek(),  //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(PANENAME_GIFT_BTN_LOW_003_PANE_BOUND_00),        //  gfl2::lyt::LytPane* bound_pane;
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_04_TOUCH,
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_04_RELEASE,
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_04_CANSEL,
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_04_TOUCH_RELEASE,
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_04_ACTIVE,
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_04_PASSIVE,
      },
#if 0
      {
        BUTTON_S04,                                           
        helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_05).Peek(),  //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(PANENAME_GIFT_BTN_LOW_003_PANE_BOUND_00),        //  gfl2::lyt::LytPane* bound_pane;
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_05_TOUCH,
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_05_RELEASE,
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_05_CANSEL,
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_05_TOUCH_RELEASE,
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_05_ACTIVE,
        LA_GIFT_GIFT_SLC_LOW_000__BUTTONS_05_PASSIVE,
      },
#endif

      {
        BUTTON_Back,                                          
        helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BACKBUTTON).Peek(),  //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(PANENAME_GIFT_BTN_LOW_002_PANE_BOUND_00),        //  gfl2::lyt::LytPane* bound_pane;
        LA_GIFT_GIFT_SLC_LOW_000__BACKBUTTON_TOUCH,
        LA_GIFT_GIFT_SLC_LOW_000__BACKBUTTON_RELEASE,
        LA_GIFT_GIFT_SLC_LOW_000__BACKBUTTON_CANSEL,
        LA_GIFT_GIFT_SLC_LOW_000__BACKBUTTON_TOUCH_RELEASE,
        LA_GIFT_GIFT_SLC_LOW_000__BACKBUTTON_ACTIVE,
        LA_GIFT_GIFT_SLC_LOW_000__BACKBUTTON_PASSIVE,
      },
    };

    gfl2::lyt::LytPane* pPane = helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BACKBUTTON).GetPane(PANENAME_COMMON_BTN_LOW_000_PANE_MAIN);

    GFL_PRINT("alpha:%d\n", pPane->GetAlpha());
    pResponder->CreateButtonManager( pHeap, pLayoutWork, buttonInfoEx, GFL_NELEMS(buttonInfoEx) );
  }




} /*  namespace (noname)  */






namespace NetApp  {
namespace Gift {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     Draw_SelectionLower
  *  @brief    ctor
  */
//------------------------------------------------------------------
LowerView_Selection::LowerView_Selection(GiftWork*  pWork)
  : LowerView_SelectionBase( pWork )
  , m_pWork(pWork)
  , m_ViewMode(VIEWMODE_NONE)
  , m_pHandler(NULL)
  , m_pButtonEventArray(NULL)
  , m_ReservedLayoutModeOnMessageFinished(LAYOUT_NONE)
  , m_TelopProcessor()
  , m_TelopStrIndex(0)
  , m_TelopVisible(false)
  , m_pMessageMenu(NULL)
  , m_AmountSpace(0)
{
  app::util::Heap*     pHeap  = m_pWork->GetAppHeap();

  /*  button  */
  {
    app::util::G2DUtil*           pG2DUtil        = GetG2DUtil();
    gfl2::lyt::LytWk*             pLayout         = pG2DUtil->GetLayoutWork( 0 );
    gfl2::lyt::LytMultiResID&     rLytMultiResID  = pG2DUtil->GetLayoutResourceID( 0 );


    CreateButton(m_pWork->GetAppHeap(), this, pLayout, rLytMultiResID);
    SetInputListener( this );


    /*    */
    {
      app::tool::ButtonManager* pButtonManager  = GetButtonManager();
      
      for(u32 index = 0; index < BUTTON_MAX; ++index)
      {
        pButtonManager->SetButtonSelectSE(index, SEID_DECIDE);
      }
      pButtonManager->SetButtonSelectSE(BUTTON_Back, SEID_CANCEL);
    }

    /*  shortcutはCreateCursor以降に設定するため、ここでは登録しない  */
  }

  /*  buttonEvent  */
  m_pButtonEventArray = GFL_NEW(pWork->GetAppHeap()->GetDeviceHeap()) IEventHandler::EventCode [BUTTON_MAX];
  ClearButtonEvent();

  /*  cursor  */
  CreateCursor(
    RESOURCE_LayoutCommonMenuCursor,
    0,
    0
  );

  /*  message  */
  {
    m_pMessageMenu  = GFL_NEW(pHeap->GetDeviceHeap()) GiftMessageMenu(m_pWork);
    m_pMessageMenu->SetEventHandler(this);
    m_pMessageMenu->SetValueProvider(this);
    
    AddSubView(m_pMessageMenu);
  }

  /*  strbufs  */
  {
    gfl2::heap::HeapBase* pHeapBase = pHeap->GetDeviceHeap();
    
    for(u32 index = 0; index < GFL_NELEMS(m_pStrBufs); ++index)
    {
      m_pStrBufs[index] = GFL_NEW(pHeapBase)  gfl2::str::StrBuf(128, pHeapBase);
    }
  }


  /*  telop  */
  {
    app::util::GetPaneHelper  helper(GetG2DUtil());
    const nw::lyt::Size       paneSize  = helper.Push(PANENAME_GIFT_SLC_LOW_000_PANE_SCROLLMESSAGE).GetPane(PANENAME_GIFT_PLT_LOW_000_PANE_NULL_TELOPW)->GetSize();

    m_TelopProcessor.SetScrollSpeed(300.0f, 100.0f);
    m_TelopProcessor.SetRectWidth(paneSize.width);

    /*  元のサイズを保存したら折り返し抑制（Widthを超えると自動的にサイズ調整されてしまうので）  */
    m_pTextBoxes[TEXTBOX_EventInformation]->SetWidthLimitEnable(false);     /**<  折り返し抑制  */
  }
}


//--------------------------------------------------------------------------------------------
/**
* @brief    デストラクタ
*/
//--------------------------------------------------------------------------------------------
LowerView_Selection::~LowerView_Selection()
{
  ReleaseCursor();

  if(m_pMessageMenu)  m_pMessageMenu->RemoveFromSuperView();


  GFL_SAFE_DELETE_ARRAY(m_pButtonEventArray);
  GFL_SAFE_DELETE(m_pMessageMenu);
  /*  strbug  */
  for(u32 index = 0; index < GFL_NELEMS(m_pStrBufs); ++index)
  {
    GFL_SAFE_DELETE(m_pStrBufs[index]);
  }
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Selection::ShowDeliveryEventInformation(const bool isImmediate)
{
  app::util::G2DUtil*     pG2DUtil    = GetG2DUtil();
  const u32               animID      = LA_GIFT_GIFT_SLC_LOW_000_BUTTON_DESC_00;


//  print::SetStringForLytTextBox(m_pTextBoxes[TEXTBOX_EventInformation], pString );

  m_TelopProcessor.ResetFinishFlag();
  m_TelopProcessor.ResetScroll();
  m_TelopStrIndex = 0;
  SetNextTelopString(true);
  m_pTextBoxes[TEXTBOX_EventInformation]->SetTagProcessor(&m_TelopProcessor);   /*  SetしなおすことでTagProcessorに処理が回る  */
  m_pTextBoxes[TEXTBOX_EventInformation]->SetDirtyFlag(true);
  m_TelopProcessor.Update();
  
  pG2DUtil->StopAnime(0, LA_GIFT_GIFT_SLC_LOW_000_BUTTON_DESC_01);
  pG2DUtil->StartAnime(0, animID);
  if(isImmediate)
  {
    pG2DUtil->SetAnimeFrame( 0, animID, pG2DUtil->GetAnimeMaxFrame(0, animID) );
  }

  m_TelopVisible  = true;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Selection::SetDeliveryEventInformation( const u32 index, const wchar_t* pString)
{
  GFL_ASSERT(index < GFL_NELEMS(m_pStrBufs));
  
  if(index < GFL_NELEMS(m_pStrBufs))
  {
    m_pStrBufs[index]->SetStr(pString);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Selection::ClearDeliveryEventInformation(void)
{
  for(u32 index = 0; index < GFL_NELEMS(m_pStrBufs); ++index)
  {
    m_pStrBufs[index]->Clear();
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Selection::HideDeliveryEventInformation( const bool isImmediate)
{
  app::util::G2DUtil*   pG2DUtil  = GetG2DUtil();
  const u32 animID      = LA_GIFT_GIFT_SLC_LOW_000_BUTTON_DESC_01;

  pG2DUtil->StopAnime(0, LA_GIFT_GIFT_SLC_LOW_000_BUTTON_DESC_00);
  pG2DUtil->StartAnime(0, animID);
  if(isImmediate)
  {
    pG2DUtil->SetAnimeFrame( 0, animID, pG2DUtil->GetAnimeMaxFrame(0, animID) );
  }

  m_TelopVisible  = false;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Selection::Update(void)
{
  NetAppLib::UI::NetAppCursorView::Update();    /*  super  */

  CursorMoveToIndex(m_pCursorController->GetSelectedIndex());

  /*    */
  if(m_TelopVisible && (m_ViewMode  == VIEWMODE_Receive_ReceivingMethod))
  {
    /*  scrollFinishは描画時に変更される  */
    if(m_TelopProcessor.IsScrollFinish())
    {
      SetNextTelopString(false);
      m_TelopProcessor.ResetFinishFlag();
    }
    m_pTextBoxes[TEXTBOX_EventInformation]->SetTagProcessor(&m_TelopProcessor);   /*  SetしなおすことでTagProcessorに処理が回る  */
    m_pTextBoxes[TEXTBOX_EventInformation]->SetDirtyFlag(true);
    m_TelopProcessor.Update();
  }
}



//--------------------------------------------------------------------------------------------
/**
* @brief  描画処理
*         子ウインドウについても自動的にコールする(親→子の順）
*/
//--------------------------------------------------------------------------------------------
void  LowerView_Selection::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  GetG2DUtil()->AddDrawableLytWkForOneFrame(m_pWork->GetAppRenderingManager(), displayNo, LAYOUTWORK_SelectionLow);
  NetAppLib::UI::NetAppCursorView::Draw(displayNo);
}




//--------------------------------------------------------------------------------------------
/**
  * @brief ペインイベントの検知
  */
//--------------------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult  LowerView_Selection::OnLayoutPaneEvent( const u32 buttonID )
{
  Event(m_pButtonEventArray[buttonID]);

  return app::ui::UIInputListener::DISABLE_ACCESS;
}


//------------------------------------------------------------------
/**
  *  @func     SetupViewMode
  *  @brief    モード切り替え
  */
//------------------------------------------------------------------
void  LowerView_Selection::SetViewMode(const ViewMode viewMode)
{
  ClearButtonEvent();
  m_pCursorController->MoveTo(0);
  m_pButtonEventArray[BUTTON_Back]    = IEventHandler::EVENT_Exit;
  
  switch(viewMode)  {
    case  VIEWMODE_MainMenu_ChooseFunction:    /**<  機能選択      */
    {
      SetLayoutMode(LAYOUT_Large);
      SetText(TEXTBOX_Large1of3, msg_mystery_sel_01);
      SetText(TEXTBOX_Large2of3, msg_mystery_sel_02);
      SetText(TEXTBOX_Large3of3, msg_mystery_sel_03);

      m_pButtonEventArray[BUTTON_L00] = IEventHandler::EVENT_MainMenu_SelectedReceive;
      m_pButtonEventArray[BUTTON_L01] = IEventHandler::EVENT_MainMenu_SelectedCardAlbum;
      m_pButtonEventArray[BUTTON_L02] = IEventHandler::EVENT_MainMenu_SelectedInstruction;
    }
      break;


    case  VIEWMODE_Instructin_ChooseInstruction:    /**<  説明項目選択*/
    {
      SetLayoutMode(LAYOUT_Large);
      SetText(TEXTBOX_Large1of3, msg_mystery_sel_12);
      SetText(TEXTBOX_Large2of3, msg_mystery_sel_10);
      SetText(TEXTBOX_Large3of3, msg_mystery_sel_11);

      m_pButtonEventArray[BUTTON_L00] = IEventHandler::EVENT_Instruction_SelectedGift;
      m_pButtonEventArray[BUTTON_L01] = IEventHandler::EVENT_Instruction_SelectedAlbum;
      m_pButtonEventArray[BUTTON_L02] = IEventHandler::EVENT_Instruction_SelectedAlbumFull;
    }
      break;


    case  VIEWMODE_Receive_ReceivingMethod:   /**<  受信方法選択  */
    {
      SetLayoutMode(LAYOUT_ReceivingMethod);
#if 0
  msg_mystery_sel_13  /*  ローカルつうしんで　うけとる  */
  msg_mystery_sel_14  /*  インターネットで　うけとる    */
  msg_mystery_sel_15  /*  せきがいせんで　うけとる      */
  msg_mystery_sel_16  /*  シリアルコードで　うけとる    */
  msg_mystery_sel_17  /*  あいことばで　うけとる        */
#endif
      SetText(TEXTBOX_Small2of9, msg_mystery_sel_13);   
      SetText(TEXTBOX_Small3of9, msg_mystery_sel_14);   
      SetText(TEXTBOX_Small4of9, msg_mystery_sel_15);   
      SetText(TEXTBOX_Small5of9, msg_mystery_sel_16);   
//      SetText(TEXTBOX_Small6of9, msg_mystery_sel_17);   

      m_pButtonEventArray[BUTTON_S00] = IEventHandler::EVENT_Receive_Selected_UDS;
      m_pButtonEventArray[BUTTON_S01] = IEventHandler::EVENT_Receive_Selected_Internet;
      m_pButtonEventArray[BUTTON_S02] = IEventHandler::EVENT_Receive_Selected_Ir;
      m_pButtonEventArray[BUTTON_S03] = IEventHandler::EVENT_Receive_Selected_Serial;
//      m_pButtonEventArray[BUTTON_S04] = IEventHandler::EVENT_Receive_Selected_Password;
    }
      break;

    default:
      GFL_PRINT("%s:[warning]:invalid viewmode (%d)\n", __FUNCTION__, static_cast<u32>(viewMode));
      break;
  };

  m_ViewMode  = viewMode;
}


//------------------------------------------------------------------
/**
  *  @func     SetText
  *  @brief    TextBoxのTextを設定
  */
//------------------------------------------------------------------
void  LowerView_Selection::SetText(const TextBoxID textBoxID, const u32 messageID)
{
  GetG2DUtil()->SetTextBoxPaneString( m_pTextBoxes[textBoxID], messageID );
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Selection::SetLayoutMode(const LayoutMode layoutModeImmediate, const LayoutMode layoutModeAfterMessageFinish)
{
  const bool  isAnimation = false;

  /*  不要となった  */
//  LayoutMode  mode  = layoutModeAfterMessageFinish; 

  switch(layoutModeImmediate)
  {
    case  LAYOUT_Large:                     /**<  大ボタン              */
    {
      const u32 buttonIDs[] = {
        BUTTON_L00,   /**<  大  */
        BUTTON_L01,   /**<  大  */
        BUTTON_L02,   /**<  大  */

      };

      GetG2DUtil()->StartAnime(0, LA_GIFT_GIFT_SLC_LOW_000_BUTTON_PATERN_L);
      GetG2DUtil()->Update2D();
      ApplyButtonAndCursor(buttonIDs, GFL_NELEMS(buttonIDs));
      SetButtonEnable(BUTTON_Back, true, isAnimation);
    }
      break;

    case  LAYOUT_ReceivingMethod:           /**<  小ボタン（受信方法）  */
    {
      const u32 buttonIDs[] = {
        BUTTON_S00,   /**<  小  */
        BUTTON_S01,   /**<  小  */
        BUTTON_S02,   /**<  小  */
        BUTTON_S03,   /**<  小  */
//        BUTTON_S04,   /**<  小  */
      };

      GetG2DUtil()->StartAnime(0, LA_GIFT_GIFT_SLC_LOW_000_BUTTON_PATERN_WAY);
      GetG2DUtil()->Update2D();
      ApplyButtonAndCursor(buttonIDs, GFL_NELEMS(buttonIDs));
      SetButtonEnable(BUTTON_Back, true, isAnimation);
    }
      break;

    case  LAYOUT_NONE:  /*  through  */
    default:
      /*  nop  */
      break;
  };
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
  const bool  isAnimation = false;


  /*  一旦全てのボタンを無効にした後、使用するもののみ有効化  */
  for(u32 num = 0; num < BUTTON_MAX; ++num)
  {
    SetButtonEnable(num, false, isAnimation);
  }
//  ReleaseCursor();

  if(idNum)
  {
    gfl2::lyt::LytPane*       cursorPosPanes[BUTTON_MAX]  = {NULL};
    gfl2::lyt::LytPane*       curPosPaneTable[BUTTON_MAX] = {NULL};
    app::util::GetPaneHelper  helper(GetG2DUtil()->GetLayoutWork(0), &(GetG2DUtil()->GetLayoutResourceID(0)));

    /*  全てのボタンのカーソル位置  */
    {
      cursorPosPanes[BUTTON_L00]  = helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONL_00).GetPane(PANENAME_GIFT_BTN_LOW_002_PANE_NULL_CUR);   /**<  大  */
      cursorPosPanes[BUTTON_L01]  = helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONL_01).GetPane(PANENAME_GIFT_BTN_LOW_002_PANE_NULL_CUR);   /**<  大  */
      cursorPosPanes[BUTTON_L02]  = helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONL_02).GetPane(PANENAME_GIFT_BTN_LOW_002_PANE_NULL_CUR);   /**<  大  */

      cursorPosPanes[BUTTON_S00]  = helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_01).GetPane(PANENAME_GIFT_BTN_LOW_003_PANE_NULL_CUR);   /**<  小  */
      cursorPosPanes[BUTTON_S01]  = helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_02).GetPane(PANENAME_GIFT_BTN_LOW_003_PANE_NULL_CUR);   /**<  小  */
      cursorPosPanes[BUTTON_S02]  = helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_03).GetPane(PANENAME_GIFT_BTN_LOW_003_PANE_NULL_CUR);   /**<  小  */
      cursorPosPanes[BUTTON_S03]  = helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_04).GetPane(PANENAME_GIFT_BTN_LOW_003_PANE_NULL_CUR);   /**<  小  */
//      cursorPosPanes[BUTTON_S04]  = helper.Clear().Push(PANENAME_GIFT_SLC_LOW_000_PANE_BUTTONS_05).GetPane(PANENAME_GIFT_BTN_LOW_003_PANE_NULL_CUR);   /**<  小  */
    }

    /*  使う分だけ詰める  */
    for(u32 index = 0; index < idNum; ++index)
    {
      curPosPaneTable[index] = cursorPosPanes[buttonIDs[index]];
      SetButtonEnable(buttonIDs[index], true, isAnimation);
    }
/*
    CreateCursor(
      m_pWork,
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
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Selection::SetNextTelopString(const bool _isFirst)
{
  const u32 strBufNum = GFL_NELEMS(m_pStrBufs);
  bool      isFirst   = _isFirst;
  
  for(u32 index = 0; index < strBufNum; ++index)
  {
    if(!isFirst)
    {
      ++m_TelopStrIndex;
    }
    m_TelopStrIndex %= strBufNum;
    
    if(m_pStrBufs[m_TelopStrIndex]->GetLength())
    {
      break;
    }
    isFirst = false;
  }

  const u32 wordLength  = m_pStrBufs[m_TelopStrIndex]->GetLength();


  GFL_ASSERT(wordLength);


  if(wordLength)
  {
    NetAppLib::Message::MessageUtility& rMsgUtil  = m_pWork->GetMessageUtility();
    print::WordSet*                     pWordSet  = m_pWork->GetWordSet();
    
    
    pWordSet->RegisterWord(0, *(m_pStrBufs[m_TelopStrIndex]));
    GetG2DUtil()->SetTextBoxPaneStringExpand(m_pTextBoxes[TEXTBOX_EventInformation], msg_mystery_cap_15);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Selection::MessageMenuView_OnEvent(const GiftMessageMenu::IEventHandler::EventCode event)
{
  switch(event)
  {
    case  GiftMessageMenu::IEventHandler::EVENT_OnFinish:         /**<  EOM後、ユーザの入力まちなどまでが完了した                   */
//      Event(IEventHandler::EVENT_MessageFinish);
      break;

    case  GiftMessageMenu::IEventHandler::EVENT_OnEOM:            /**<  メッセージ表示が終端に到達                                  */
      Event(IEventHandler::EVENT_MessageFinish);
      break;

    case  GiftMessageMenu::IEventHandler::EVENT_OnSelectedItem:   /**<  リスト選択でItemが選択された: GetSelectedItemIndex()で取得  */
    case  GiftMessageMenu::IEventHandler::EVENT_OnSelectedYes:    /**<  Yes/NoモードでYesが選択された                               */
    case  GiftMessageMenu::IEventHandler::EVENT_OnSelectedNo:     /**<  Yes/NoモードでNoが選択された                                */
      /*  nop */
      break;
  }
}




} /*  namespace MysteryGift */
} /*  namespace NetApp      */


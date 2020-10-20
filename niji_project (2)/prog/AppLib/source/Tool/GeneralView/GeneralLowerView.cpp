//==============================================================================
/**
 * @file   GeneralLowerView.cpp
 * @date   2015/09/30 Wed. 20:10:47
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
/*  arc  */
#include  <arc_def_index/arc_def.h>

/*  cursor  */
#include  <arc_index/MenuCursor.gaix>

/*  layout  */
#include  <arc_index/MenuWindow2.gaix>
#include  <niji_conv_header/app/common_menu_2/MenuWindow2.h>
#include  <niji_conv_header/app/common_menu_2/MenuWindow2_anim_list.h>
#include  <niji_conv_header/app/common_menu_2/MenuWindow2_pane.h>



#include  "Sound/include/Sound.h"

#include  "AppLib/include/Util/app_util_GetPaneHelper.h"
#include  "AppLib/include/Util/app_util_FileAccessor.h"

#include  "./Module/ResourceLoader.h"
#include  "./Module/CursorController.h"
#include  "AppLib/include/Tool/GeneralView/GeneralLowerView.h"

// =============================================================================
/**/
//==============================================================================
namespace
{
  enum
  {
    STATE_Initial,
    STATE_Loading,
    STATE_Setup,
    STATE_Running,
    
    STATE_
  };

  /*
    SE
  */
  static const u32  SEID_MOVEUP   = SEQ_SE_SELECT1;
  static const u32  SEID_MOVEDOWN = SEQ_SE_SELECT1;
  static const u32  SEID_DECIDE   = SEQ_SE_DECIDE1;
  static const u32  SEID_CANCEL   = SEQ_SE_CANCEL1;

  static const struct
  {
    u32 buttonID;
    u32 seID;
  }seTable[app::tool::GeneralLowerView::BUTTON_MAX]  = {
    {app::tool::GeneralLowerView::BUTTON_0,     SEID_DECIDE},
    {app::tool::GeneralLowerView::BUTTON_1,     SEID_DECIDE},
    {app::tool::GeneralLowerView::BUTTON_2,     SEID_DECIDE},
    {app::tool::GeneralLowerView::BUTTON_3,     SEID_DECIDE},
    {app::tool::GeneralLowerView::BUTTON_Back,  SEID_CANCEL},
  };


  /*
    resource
  */
  enum
  {
    RESOURCE_Layout,
    RESOURCE_Cursor,

    RESOURCE_NUM
  };

  static const struct
  {
    gfl2::fs::ArcFile::ARCID    arcID;
    gfl2::fs::ArcFile::ARCDATID arcDatID;
    bool                        isCompressed;
  }resourceTable[RESOURCE_NUM]  = {
    {ARCID_MENU_WINDOW2,  GARC_MenuWindow2_MenuWindow2_applyt_COMP, true},      /*  layout  */
    {ARCID_MENU_CURSOR,   GARC_MenuCursor_MenuCursor_applyt_COMP,   true},  
  };

}


// =============================================================================
/**/
//==============================================================================


namespace app   {
namespace tool  {

// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/09/28 Mon. 19:56:54
  */
//==============================================================================
class GeneralLowerView::DefaultResourceProvider
  : public  IResourceProvider
{
  GFL_FORBID_COPY_AND_ASSIGN(DefaultResourceProvider);
public:
  DefaultResourceProvider(GeneralLowerView* pOwner)
    : m_pOwner(pOwner)
  {}
  virtual ~DefaultResourceProvider()
  {
  }

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual void*  IResourceProvider_OnResourceRequest(const gfl2::fs::ArcFile::ARCID arcID, const gfl2::fs::ArcFile::ARCDATID arcDatID)
  {
    for(u32 index = 0; index < GetResourceNum(); ++index)
    {
      if((arcID == resourceTable[index].arcID) && (arcDatID == resourceTable[index].arcDatID))
      {
        return m_pOwner->m_pResourceLoaders[index]->GetData();
      }
    }
    
    return NULL;
  }

protected:
  GeneralLowerView*   m_pOwner;
};


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
GeneralLowerView::GeneralLowerView(
  app::util::Heap*                              pHeap,
  app::util::AppRenderingManager*               pRenderingManager,
  const app::util::G2DUtil::SETUP_DISPLAY_TYPE  displayType
)
  : app::ui::UIView(pHeap)
  , m_pHeap(pHeap)
  , m_pRenderingManager(pRenderingManager)
  , m_DisplayType(displayType)
  , m_State(STATE_Initial)
  , m_LayoutMode(LAYOUTMODE_INVALID)
  /*  handler  */
  , m_pEventHandler(NULL)
  , m_pResourceProvider(NULL)
  /*  cursor  */
  , m_CursorController(pHeap, pRenderingManager)
  /*  pane  */
  , m_pBGPane(NULL)
  /*  shortcut  */
  , m_ShortcutButton_A(ButtonManager::BUTTON_ID_NULL)
  , m_ShortcutButton_B(ButtonManager::BUTTON_ID_NULL)
  , m_ShortcutButton_X(ButtonManager::BUTTON_ID_NULL)
  , m_ShortcutButton_Y(ButtonManager::BUTTON_ID_NULL)
  , m_ShortcutButton_L(ButtonManager::BUTTON_ID_NULL)
  , m_ShortcutButton_R(ButtonManager::BUTTON_ID_NULL)

{
  /*  textbox  */
  for(u32 index = 0; index < GFL_NELEMS(m_pTextBoxes); ++index)
  {
    m_pTextBoxes[index] = NULL;
  }


  /*  resource  */
  GFL_ASSERT(GetResourceNum() <= RESOURCE_LOADER_MAX);
  for(u32 index = 0; index < GFL_NELEMS(m_pResourceLoaders); ++index)
  {
    m_pResourceLoaders[index] = NULL;
  }

  SetInputListener( this );
  RegisterShortcut_B(BUTTON_Back);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
GeneralLowerView::~GeneralLowerView()
{
  /*  this  */
  Delete2D();
  DeleteGraphicsSystem();

  /*  loaders  */
  for(u32 index = 0; index < GFL_NELEMS(m_pResourceLoaders); ++index)
  {
    GFL_SAFE_DELETE(m_pResourceLoaders[index]);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  GeneralLowerView::ObserveSetup(void)
{
  bool  bContinue   = false;

  do
  {
    bContinue  = false;

    switch(m_State)
    {
      /*
        初期状態
        resourceProviderが提供されていればLoadingを省略し、Setupへ
        resourceProviderが提供されなければ、ResourceLoaderを作成してLoadingへ
      */
      case  STATE_Initial:
        if(m_pResourceProvider)
        {
          m_State   = STATE_Setup;
          bContinue = true;         /*  同Frameで行う  */
        }
        else
        {
          for(u32 index = 0; index < GetResourceNum(); ++index)
          {
            m_pResourceLoaders[index] = GFL_NEW(m_pHeap->GetDeviceHeap())  ResourceLoader(m_pHeap);
            m_pResourceLoaders[index]->RequestLoad(resourceTable[index].arcID, resourceTable[index].arcDatID, resourceTable[index].isCompressed);
          }
          m_State = STATE_Loading;
        }
        break;

      /*
        リソース読み込み
      */
      case  STATE_Loading:
      {
        bool  bLoadedAll  = true;
        for(u32 index = 0; index < GFL_NELEMS(m_pResourceLoaders); ++index)
        {
          if(m_pResourceLoaders[index] && (!m_pResourceLoaders[index]->ObserveLoad()))
          {
            bLoadedAll  = false;
            break;    /*  ←複数のリソースを同時に読み込めるならコメントアウト  */
          }
        }
        if(bLoadedAll)
        {
          m_State = STATE_Setup;
        }
      }
        break;

      case  STATE_Setup:
        Setup();    /*  setupの中でStateが変わる  */
        break;

      case  STATE_Running:      /*  fallthrough  */
      default:
        break;
    }

  }while(bContinue);


  return (m_State  == STATE_Running);
}


//------------------------------------------------------------------
/**
  *  @brief    表示するオブジェクトを変更する
  */
//------------------------------------------------------------------
void  GeneralLowerView::SetLayoutMode(const LayoutMode layoutMode, const bool bEnableBackButton, const bool bShowBG)
{
  app::util::G2DUtil* pG2DUtil  = GetG2DUtil();

  /*  item  */
  {
    m_LayoutMode  = layoutMode;

    switch(layoutMode)
    {
      /*  ボタンなし                              */
      case  LAYOUTMODE_NoButton:              
        pG2DUtil->StartAnime(0, LA_MENUWINDOW2_MENU2_MIN_LOW_001_TAG_NONE);
        m_CursorController.SetLocationNum(0);
        break;

      /*  1ボタン                                 */
      case  LAYOUTMODE_1Button:               
        pG2DUtil->StartAnime(0, LA_MENUWINDOW2_MENU2_MIN_LOW_001_TAG_BUTTONS_1_NOMSG);
        m_CursorController.SetLocationNum(1);
        break;

      /*  2ボタン                                 */
      case  LAYOUTMODE_2Buttons:              
        pG2DUtil->StartAnime(0, LA_MENUWINDOW2_MENU2_MIN_LOW_001_TAG_BUTTONS_2_NOMSG);
        m_CursorController.SetLocationNum(2);
        break;

      /*  3ボタン                                 */
      case  LAYOUTMODE_3Buttons:              
        pG2DUtil->StartAnime(0, LA_MENUWINDOW2_MENU2_MIN_LOW_001_TAG_BUTTONS_3_NOMSG);
        m_CursorController.SetLocationNum(3);
        break;

      /*  4ボタン                                 */
      case  LAYOUTMODE_4Buttons:              
        pG2DUtil->StartAnime(0, LA_MENUWINDOW2_MENU2_MIN_LOW_001_TAG_BUTTONS_4_NOMSG);
        m_CursorController.SetLocationNum(4);
        break;

      /*  ボタンなし（メッセージウインドウあり）  */
      case  LAYOUTMODE_NoButton_WithMessage:  
        pG2DUtil->StartAnime(0, LA_MENUWINDOW2_MENU2_MIN_LOW_001_TAG_NONE_WITHMSG);
        m_CursorController.SetLocationNum(0);
        break;

      /*  4ボタン（メッセージウインドウあり）     */
      case  LAYOUTMODE_1Button_WithMessage:   
        pG2DUtil->StartAnime(0, LA_MENUWINDOW2_MENU2_MIN_LOW_001_TAG_BUTTONS_1_WITHMSG);
        m_CursorController.SetLocationNum(1);
        break;

      /*  4ボタン（メッセージウインドウあり）     */
      case  LAYOUTMODE_2Buttons_WithMessage:  
        pG2DUtil->StartAnime(0, LA_MENUWINDOW2_MENU2_MIN_LOW_001_TAG_BUTTONS_2_WITHMSG);
        m_CursorController.SetLocationNum(2);
        break;

      /*  4ボタン（メッセージウインドウあり）     */
      case  LAYOUTMODE_3Buttons_WithMessage:  
        pG2DUtil->StartAnime(0, LA_MENUWINDOW2_MENU2_MIN_LOW_001_TAG_BUTTONS_3_WITHMSG);
        m_CursorController.SetLocationNum(3);
        break;

      /*  4ボタン（メッセージウインドウあり）     */
      case  LAYOUTMODE_4Buttons_WithMessage:  
        pG2DUtil->StartAnime(0, LA_MENUWINDOW2_MENU2_MIN_LOW_001_TAG_BUTTONS_4_WITHMSG);
        m_CursorController.SetLocationNum(4);
        break;

    };

    pG2DUtil->Update2D();
    m_CursorController.UpdatePos();
  }


  /*  backButton  */
  {
    pG2DUtil->StartAnime(0, bEnableBackButton ? LA_MENUWINDOW2_MENU2_MIN_LOW_001_TAG_BUTTONBACK_ENABLE : LA_MENUWINDOW2_MENU2_MIN_LOW_001_TAG_BUTTONBACK_DISABLE);
  }

  /*  bg  */
  {
    SetBGVisibility(bShowBG);
  }

  pG2DUtil->Update2D();
  Event(GeneralLowerView::IEventHandler::EVENT_OnViewMode);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  GeneralLowerView::SetBGVisibility(const bool isVisible)
{
  m_pBGPane->SetVisible(isVisible);
}



//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  GeneralLowerView::SetTextBoxString(const TextBoxID textBoxID, gfl2::str::StrBuf* pStr, const bool isExpand)
{
  if(m_State  == STATE_Running)
  {
    if(isExpand)  GetG2DUtil()->SetTextBoxPaneStringExpand(m_pTextBoxes[textBoxID], *pStr);
    else          GetG2DUtil()->SetTextBoxPaneString(m_pTextBoxes[textBoxID], pStr);
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    要求リソース数を取得
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
u32  GeneralLowerView::GetResourceNum(void)
{
  return  GFL_NELEMS(resourceTable);

}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void GeneralLowerView::GetResourceIDs(IResourceIDHandler& rHandler)
{
  const u32 resourceNum = GetResourceNum();

  for(u32 index = 0; index < resourceNum; ++index)
  {
    rHandler.IResourceIDHandler_NotifyResourceID(resourceTable[index].arcID, resourceTable[index].arcDatID, resourceTable[index].isCompressed);
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  GeneralLowerView::Update(void)
{
  /*  super  */
  app::ui::UIView::Update();

  /*  cursor  */
  m_CursorController.OnUpdate();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  GeneralLowerView::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  /*    */
  if(m_State  == STATE_Running)
  {
    GetG2DUtil()->AddDrawableLytWkForOneFrame(m_pRenderingManager, displayNo, 0);
  }

  /*  cursor  */
  m_CursorController.OnDraw(displayNo);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  GeneralLowerView::IsDrawing(void) const
{
  return  (m_CursorController.Cleanup() && this->app::ui::UIView::IsDrawing());
}







/*  implement UIInputListener I/F  */
//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  GeneralLowerView::OnLayoutPaneTouchTriggerEvent( const u32 buttonID )
{
  switch(buttonID)
  {
    case  BUTTON_0: m_CursorController.MoveTo(0); break;
    case  BUTTON_1: m_CursorController.MoveTo(1); break;
    case  BUTTON_2: m_CursorController.MoveTo(2); break;
    case  BUTTON_3: m_CursorController.MoveTo(3); break;
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
app::ui::UIInputListener::ListenerResult  GeneralLowerView::OnLayoutPaneEvent( const u32 buttonID )
{
  app::ui::UIInputListener::ListenerResult  result  = ENABLE_ACCESS;

  switch(buttonID)
  {
    case  BUTTON_0:      /**<  ボタン0が押された     */
      Event(GeneralLowerView::IEventHandler::EVENT_OnButton0);
      result  = DISABLE_ACCESS;
      break;

    case  BUTTON_1:      /**<  ボタン1が押された     */
      Event(GeneralLowerView::IEventHandler::EVENT_OnButton1);
      result  = DISABLE_ACCESS;
      break;

    case  BUTTON_2:      /**<  ボタン2が押された     */
      Event(GeneralLowerView::IEventHandler::EVENT_OnButton2);
      result  = DISABLE_ACCESS;
      break;

    case  BUTTON_3:      /**<  ボタン3が押された     */
      Event(GeneralLowerView::IEventHandler::EVENT_OnButton3);
      result  = DISABLE_ACCESS;
      break;

    case  BUTTON_Back:   /**<  戻るボタンが押された  */
      Event(GeneralLowerView::IEventHandler::EVENT_OnButtonBack);
      result  = DISABLE_ACCESS;
      break;
  }

  return result;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @note     buttonManagerにもBindKeyというショートカットキー機能がある
  *            しかしAボタンでUIを起動するにはOnKeyActionでStartSelectedActをコールする必要があり、
  *            その他のUIをBindKey実装としても管理箇所が分散されるだけでメリットがない（キーの排他も担保しにくくなる）ため、
  *            協議の結果敢えて自前で対応する方針を採ることとした
  */
//------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult  GeneralLowerView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  app::ui::UIInputListener::ListenerResult  result            = ENABLE_ACCESS;
  app::tool::ButtonManager*                 pUIButtonManager  = GetButtonManager();

  if( IsButtonAnimation() )
  {
    return DISABLE_ACCESS;
  }

  /*  cursor  */
  result  = m_CursorController.OnKeyAction(pButton, pKey, pStick);
  if(result != ENABLE_ACCESS) return result;



  /*
    shortcut
  */
  {
    const struct  {
      u32                                 triggerButtonID;
      app::tool::ButtonManager::ButtonId  shotrcutUIButtonID;
    }shortcutTable[]  = {
      {gfl2::ui::BUTTON_A, m_ShortcutButton_A},
      {gfl2::ui::BUTTON_B, m_ShortcutButton_B},
      {gfl2::ui::BUTTON_X, m_ShortcutButton_X},
      {gfl2::ui::BUTTON_Y, m_ShortcutButton_Y},
      {gfl2::ui::BUTTON_L, m_ShortcutButton_L},
      {gfl2::ui::BUTTON_R, m_ShortcutButton_R},
    };

    for(int num = 0; num < GFL_NELEMS(shortcutTable); ++num)
    {
      if((shortcutTable[num].shotrcutUIButtonID != app::tool::ButtonManager::BUTTON_ID_NULL) && pButton->IsTrigger(shortcutTable[num].triggerButtonID))
      {
        /*  入力に対応したUIボタンを動作させる  */
        pUIButtonManager->StartSelectedAct( shortcutTable[num].shotrcutUIButtonID );
        return DISABLE_ACCESS;
      }
    }
  }

  return result;
}
/*  end implement UIInputListener I/F  */


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
/*  implement CursorController::IEventHandler  */
void  GeneralLowerView::CursorController_OnEvent(CursorController* pSender, const CursorController::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  CursorController::IEventHandler::EVENT_OnDecided:   /**<  カーソルで決定された  */
      GetButtonManager()->StartSelectedAct( pSender->GetSelectedIndex() );
      break;


    case  CursorController::IEventHandler::EVENT_OnMove:      /**<  カーソルが移動した    */
    default:
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
void  GeneralLowerView::Event(const IEventHandler::EventCode eventCode)
{
  if(m_pEventHandler)
  {
    m_pEventHandler->GeneralLowerView_OnEvent(this, eventCode);
  }
}




//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
void  GeneralLowerView::Setup(void)
{
  DefaultResourceProvider defaultResourceProvider(this);
  IResourceProvider*      pResourceProvider = m_pResourceProvider ? m_pResourceProvider : &defaultResourceProvider;


  void* pLayoutResourceData = pResourceProvider->IResourceProvider_OnResourceRequest(resourceTable[RESOURCE_Layout].arcID, resourceTable[RESOURCE_Layout].arcDatID);
  void* pCursorResourceData = pResourceProvider->IResourceProvider_OnResourceRequest(resourceTable[RESOURCE_Cursor].arcID, resourceTable[RESOURCE_Cursor].arcDatID);

  GFL_ASSERT(m_State == STATE_Setup);
  GFL_ASSERT(pLayoutResourceData);
  GFL_ASSERT(pCursorResourceData);


  /*  setupLayoutSystem  */
  {
    const app::util::G2DUtil::LytResData  lytResDataTable[] =
    {
      { pLayoutResourceData, 0, app::util::G2DUtil::ATTACH_NONE },
    };

    const app::util::G2DUtil::LytwkData   layoutWorkTable[] =
    {
      {
        0,                                            //!< 使用するリソースの番号         : lytResDataTable
        LYTRES_MENUWINDOW2_MENU2_MIN_LOW_001_BFLYT,   //!< レイアウトデータID ( *_BFLYT ) : 
        LA_MENUWINDOW2_MENU2_MIN_LOW_001___NUM,       //!< アニメ数                       : 
        pLayoutResourceData,                          //!< リソースデータ                 : pWork->ResourceManager->ResourceBuffer
        true,                                         //!< 非表示ペインに行列計算を行うか
        m_DisplayType,                                //!< ディスプレイ
        true                                          //!< 表示設定
      },
    };


    /*
      UIView
    */
    SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
    CreateGraphicsSystem( m_pHeap, false );

    Create2D(
      m_pHeap,                          /*  app::util::Heap*    */
      NULL,                             /*  gfl2::lyt::LytSys                 (NULLなら内部で新規に生成する。既存のものを使用する場合にはここで指定する)  */
      1,                                /*  u32                               layoutを扱う数（capacity）。内部ワークの数  */
      lytResDataTable,                  /*  app::util::G2DUtil::LytResData*   リソーステーブル        */
      GFL_NELEMS(lytResDataTable),      /*  u32                               リソーステーブル要素数  */
      layoutWorkTable,                  /*  app::util::G2DUtil::LytwkData*    ワークテーブル          */
      GFL_NELEMS(layoutWorkTable),      /*  u32                               ワークテーブル要素数    */
      app::ui::UIView::UNUSE_MESSAGE_DATA
    );
  }


  app::util::G2DUtil*         pG2DUtil        = GetG2DUtil();
  gfl2::lyt::LytWk*           pLayout         = pG2DUtil->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID&   rLytMultiResID  = pG2DUtil->GetLayoutResourceID( 0 );
  app::util::GetPaneHelper    helper(pLayout, &rLytMultiResID);


  /*  textbox  */
  {
    m_pTextBoxes[TEXTBOX_Button0] = helper
                                      .Clear()
                                      .Push(PANENAME_MENU2_MIN_LOW_001_PANE_BUTTON_00)
                                      .GetTextBoxPane(PANENAME_FIELDMENU_BTN_LOW_000_PANE_TEXTBOX_00)
                                    ;

    m_pTextBoxes[TEXTBOX_Button1] =  helper
                                      .Clear()
                                      .Push(PANENAME_MENU2_MIN_LOW_001_PANE_BUTTON_01)
                                      .GetTextBoxPane(PANENAME_FIELDMENU_BTN_LOW_000_PANE_TEXTBOX_00)
                                    ;
    m_pTextBoxes[TEXTBOX_Button2] = helper
                                      .Clear()
                                      .Push(PANENAME_MENU2_MIN_LOW_001_PANE_BUTTON_02)
                                      .GetTextBoxPane(PANENAME_FIELDMENU_BTN_LOW_000_PANE_TEXTBOX_00)
                                    ;

    m_pTextBoxes[TEXTBOX_Button3] =  helper
                                      .Clear()
                                      .Push(PANENAME_MENU2_MIN_LOW_001_PANE_BUTTON_03)
                                      .GetTextBoxPane(PANENAME_FIELDMENU_BTN_LOW_000_PANE_TEXTBOX_00)
                                    ;
  }

  /*  bg  */
  {
    m_pBGPane = helper
                  .Clear()
                  .GetPane(PANENAME_MENU2_MIN_LOW_001_PANE_NULL_BG)
                ;

  }


  /*  buttons  */
  {
    const app::ui::ButtonInfoEx   buttonInfoEx[BUTTON_MAX]  =
    {
      {
        BUTTON_0,                                                                 //  app::tool::ButtonManager::ButtonId button_id;
        helper.Clear().Push(PANENAME_MENU2_MIN_LOW_001_PANE_BUTTON_00).Peek(),    //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(PANENAME_FIELDMENU_BTN_LOW_000_PANE_BOUND_00),     //  gfl2::lyt::LytPane* bound_pane;
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_00_TOUCH,
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_00_RELEASE,
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_00_CANSEL,
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_00_TOUCH_RELEASE,
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_00_ACTIVE,
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_00_PASSIVE,
      },
      {
        BUTTON_1,                                                                 //  app::tool::ButtonManager::ButtonId button_id;
        helper.Clear().Push(PANENAME_MENU2_MIN_LOW_001_PANE_BUTTON_01).Peek(),    //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(PANENAME_FIELDMENU_BTN_LOW_000_PANE_BOUND_00),     //  gfl2::lyt::LytPane* bound_pane;
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_01_TOUCH,
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_01_RELEASE,
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_01_CANSEL,
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_01_TOUCH_RELEASE,
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_01_ACTIVE,
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_01_PASSIVE,
      },
      {
        BUTTON_2,                                                                 //  app::tool::ButtonManager::ButtonId button_id;
        helper.Clear().Push(PANENAME_MENU2_MIN_LOW_001_PANE_BUTTON_02).Peek(),    //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(PANENAME_FIELDMENU_BTN_LOW_000_PANE_BOUND_00),     //  gfl2::lyt::LytPane* bound_pane;
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_02_TOUCH,
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_02_RELEASE,
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_02_CANSEL,
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_02_TOUCH_RELEASE,
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_02_ACTIVE,
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_02_PASSIVE,
      },
      {
        BUTTON_3,                                                                 //  app::tool::ButtonManager::ButtonId button_id;
        helper.Clear().Push(PANENAME_MENU2_MIN_LOW_001_PANE_BUTTON_03).Peek(),    //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(PANENAME_FIELDMENU_BTN_LOW_000_PANE_BOUND_00),     //  gfl2::lyt::LytPane* bound_pane;
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_03_TOUCH,
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_03_RELEASE,
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_03_CANSEL,
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_03_TOUCH_RELEASE,
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_03_ACTIVE,
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_03_PASSIVE,
      },

      {
        BUTTON_Back,                                                                //  app::tool::ButtonManager::ButtonId button_id;
        helper.Clear().Push(PANENAME_MENU2_MIN_LOW_001_PANE_BUTTON_BACK).Peek(),    //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00),          //  gfl2::lyt::LytPane* bound_pane;
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_BACK_TOUCH,
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_BACK_RELEASE,
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_BACK_CANSEL,
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_BACK_TOUCH_RELEASE,
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_BACK_ACTIVE,
        LA_MENUWINDOW2_MENU2_MIN_LOW_001__BUTTON_BACK_PASSIVE,
      },
    };

    CreateButtonManager( m_pHeap, pLayout, buttonInfoEx, GFL_NELEMS(buttonInfoEx) );

    /*  SE  */
    SetButtonSEtoDefault();
  }

  /*
    cursor
  */
  {
    m_CursorController.Setup(
      pCursorResourceData,
      pG2DUtil->GetLayoutSystem(),
      pG2DUtil->GetLayoutWorkSetup( (m_DisplayType == app::util::G2DUtil::SETUP_UPPER) ? gfl2::lyt::DISPLAY_UPPER : gfl2::lyt::DISPLAY_LOWER ),
      pG2DUtil->GetAppLytAccessor(),
      (m_DisplayType == app::util::G2DUtil::SETUP_UPPER)
    );
    m_CursorController.SetEventHandler(this);

    /*  cursorPos  */
    m_CursorController.RegisterLocation(0, pLayout, helper.Clear().Push(PANENAME_MENU2_MIN_LOW_001_PANE_BUTTON_00).GetPane(PANENAME_FIELDMENU_BTN_LOW_000_PANE_NULL_CUR));
    m_CursorController.RegisterLocation(1, pLayout, helper.Clear().Push(PANENAME_MENU2_MIN_LOW_001_PANE_BUTTON_01).GetPane(PANENAME_FIELDMENU_BTN_LOW_000_PANE_NULL_CUR));
    m_CursorController.RegisterLocation(2, pLayout, helper.Clear().Push(PANENAME_MENU2_MIN_LOW_001_PANE_BUTTON_02).GetPane(PANENAME_FIELDMENU_BTN_LOW_000_PANE_NULL_CUR));
    m_CursorController.RegisterLocation(3, pLayout, helper.Clear().Push(PANENAME_MENU2_MIN_LOW_001_PANE_BUTTON_03).GetPane(PANENAME_FIELDMENU_BTN_LOW_000_PANE_NULL_CUR));
  }

  m_State  = STATE_Running;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void   GeneralLowerView::SetButtonSEtoDefault(void)
{
  app::tool::ButtonManager* pButtonManager  = GetButtonManager();
  
  for(u32 index = 0; index < GFL_NELEMS(seTable); ++index)
  {
    pButtonManager->SetButtonSelectSE(seTable[index].buttonID, seTable[index].seID);
  }
}


} /*  namespace app   */
} /*  namespace tool  */

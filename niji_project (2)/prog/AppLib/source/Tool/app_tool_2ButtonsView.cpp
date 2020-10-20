//==============================================================================
/**
 * @file   app_tool_2ButtonsView.cpp
 * @date   2015/09/08 Tue. 14:56:12
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

/*  menuwindow2  */
#include  <arc_index/MenuWindow2.gaix>
#include  <niji_conv_header/app/common_menu_2/MenuWindow2.h>
#include  <niji_conv_header/app/common_menu_2/MenuWindow2_anim_list.h>
#include  <niji_conv_header/app/common_menu_2/MenuWindow2_pane.h>

#include  "Sound/include/Sound.h"

#include  "AppLib/include/Util/app_util_GetPaneHelper.h"
#include  "AppLib/include/Util/app_util_FileAccessor.h"
#include  "AppLib/include/Tool/app_tool_2ButtonsView.h"


// =============================================================================
/**/
//==============================================================================
namespace
{
  enum
  {
    STATE_Initial,
    STATE_Loading,
    STATE_SetupWait,
    STATE_Running,
    
    STATE_
  };

  static const u32  SEID_DECIDE   = SEQ_SE_DECIDE1;
  static const u32  SEID_CANCEL   = SEQ_SE_CANCEL1;
}





namespace app   {
namespace tool  {
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/09/08 Tue. 17:14:25
  */
//==============================================================================
class TwoButtonsView::DefaultResourceLoader
  : public  app::tool::TwoButtonsView::ResourceLoaderBase
{
  GFL_FORBID_COPY_AND_ASSIGN(DefaultResourceLoader);
public:
  DefaultResourceLoader(app::util::Heap* pHeap)
    : app::tool::TwoButtonsView::ResourceLoaderBase(pHeap)
    , m_ArcID(gfl2::fs::ArcFile::ARCID_NULL)
    , m_ArcDatID(gfl2::fs::ArcFile::ARCDATID_NULL)
    , m_State(STATE_Initial)
  {}
  virtual ~DefaultResourceLoader()
  {
  }
  
  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual bool  RequestLoad(const gfl2::fs::ArcFile::ARCID arcID, const gfl2::fs::ArcFile::ARCDATID arcDatID)
  {
    if(m_State != STATE_Initial) return false;

    m_ArcID     = arcID;
    m_ArcDatID  = arcDatID;
    m_State  = STATE_Open;

    return true;
  }

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual bool  ObserveLoad(void)
  {
    gfl2::heap::HeapBase*   pSysLowerHeapBase = m_pHeap->GetSystemHeap()->GetLowerHandle();
    gfl2::heap::HeapBase*   pHeapBase         = m_pHeap->GetDeviceHeap();

    switch(m_State)
    {
      case  STATE_Initial:
        break;

      case  STATE_Open:
        app::util::FileAccessor::FileOpen(m_ArcID, pSysLowerHeapBase);
        m_State = STATE_Opening;
        /*  fallthrough  */

      case  STATE_Opening:
        if(app::util::FileAccessor::IsFileOpen(m_ArcID))
        {
          app::util::FileAccessor::FileLoad(
            m_ArcID,
            m_ArcDatID,
            &m_pData,
            pHeapBase,
            true,
            nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT
          );
          
          m_State  = STATE_Loading;
        }
        break;

      case  STATE_Loading:
        if(app::util::FileAccessor::IsFileLoad(&m_pData))
        {
          app::util::FileAccessor::FileClose(m_ArcID, pSysLowerHeapBase );
          m_State  = STATE_Closing;
        }
        break;

      case  STATE_Closing:
        if(app::util::FileAccessor::IsFileClose( m_ArcID ))
        {
          m_State  = STATE_Loaded;
        }
        break;

      case  STATE_Loaded:
        break;
    }

    return (m_State == STATE_Loaded);
  }

protected:
  gfl2::fs::ArcFile::ARCID      m_ArcID;
  gfl2::fs::ArcFile::ARCDATID   m_ArcDatID;

  enum  State
  {
    STATE_Initial,
    STATE_Open,
    STATE_Opening,
    STATE_Loading,
    STATE_Closing,
    STATE_Loaded,
    STATE_
  }m_State;
};





//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
TwoButtonsView::TwoButtonsView(
  app::util::Heap*                              pHeap,
  app::util::AppRenderingManager*               pRenderingManager,
  const bool                                    isAutoSetup,
  const app::util::G2DUtil::SETUP_DISPLAY_TYPE  displayType
)
  : app::ui::UIView(pHeap)
  , m_pHeap(pHeap)
  , m_pRenderingManager(pRenderingManager)
  , m_DisplayType(displayType)
  , m_pHandler(NULL)
  , m_State(STATE_Initial)
  /*  cursor  */
  , m_CursorController(pHeap, pRenderingManager)
  /*  shortcut  */
  , m_ShortcutButton_A(ButtonManager::BUTTON_ID_NULL)
  , m_ShortcutButton_B(ButtonManager::BUTTON_ID_NULL)
  , m_ShortcutButton_X(ButtonManager::BUTTON_ID_NULL)
  , m_ShortcutButton_Y(ButtonManager::BUTTON_ID_NULL)
  , m_ShortcutButton_L(ButtonManager::BUTTON_ID_NULL)
  , m_ShortcutButton_R(ButtonManager::BUTTON_ID_NULL)
{
  gfl2::heap::HeapBase*   pHeapBase = m_pHeap->GetDeviceHeap();

  /*  textbox  */
  for(u32 index = 0; index < GFL_NELEMS(m_pTextBoxes); ++index)
  {
    m_pTextBoxes[index] = NULL;
  }


  /*  resource  */
  if(isAutoSetup)
  {
    m_pResourceLoaders[LOADER_Layout] = GFL_NEW(pHeapBase)  DefaultResourceLoader(m_pHeap);
    m_pResourceLoaders[LOADER_Cursor] = GFL_NEW(pHeapBase)  DefaultResourceLoader(m_pHeap);

    m_pResourceLoaders[LOADER_Layout]->RequestLoad(ARCID_MENU_WINDOW2,  GARC_MenuWindow2_MenuWindow2_applyt_COMP);
    m_pResourceLoaders[LOADER_Cursor]->RequestLoad(ARCID_MENU_CURSOR,   GARC_MenuCursor_MenuCursor_applyt_COMP);

    m_State        = STATE_Loading;
  }
  else
  {
    for(u32 index = 0; index < GFL_NELEMS(m_pResourceLoaders); ++index)
    {
      m_pResourceLoaders[index] = NULL;
    }

    m_State        = STATE_SetupWait;
  }

  SetInputListener( this );
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
TwoButtonsView::~TwoButtonsView()
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
  *  @brief    isAutoLoadAndSetup==false時に手動でsetupする
  */
//------------------------------------------------------------------
void  TwoButtonsView::Setup(void* pLayoutResourceData, void* pCursorResourceData)
{
  GFL_ASSERT(m_State == STATE_SetupWait);
  GFL_ASSERT(pLayoutResourceData);
  GFL_ASSERT(pCursorResourceData);


  gfl2::heap::HeapBase*   pHeapBase = m_pHeap->GetDeviceHeap();


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
        LYTRES_MENUWINDOW2_MENU2_MIN_LOW_000_BFLYT,   //!< レイアウトデータID ( *_BFLYT ) : niji_conv_header/app/gift/Gift.h
        LA_MENUWINDOW2_MENU2_MIN_LOW_000___NUM,       //!< アニメ数                       : niji_conv_header/app/gift/Gift_anim_list.h
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
    m_pTextBoxes[TEXTBOX_Button1] = helper
                                      .Clear()
                                      .Push(PANENAME_MENU2_MIN_LOW_000_PANE_BUTTON_00)
                                      .GetTextBoxPane(PANENAME_FIELDMENU_BTN_LOW_000_PANE_TEXTBOX_00)
                                    ;

    m_pTextBoxes[TEXTBOX_Button2] =  helper
                                      .Clear()
                                      .Push(PANENAME_MENU2_MIN_LOW_000_PANE_BUTTON_01)
                                      .GetTextBoxPane(PANENAME_FIELDMENU_BTN_LOW_000_PANE_TEXTBOX_00)
                                    ;
  }


  /*  buttons  */
  {
    const app::ui::ButtonInfoEx   buttonInfoEx[BUTTON_MAX]  =
    {
      {
        BUTTON_1of2,                                                            //  app::tool::ButtonManager::ButtonId button_id;
        helper.Clear().Push(PANENAME_MENU2_MIN_LOW_000_PANE_BUTTON_00).Peek(),  //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(PANENAME_FIELDMENU_BTN_LOW_000_PANE_BOUND_00),   //  gfl2::lyt::LytPane* bound_pane;
        LA_MENUWINDOW2_MENU2_MIN_LOW_000__BUTTON_00_TOUCH,
        LA_MENUWINDOW2_MENU2_MIN_LOW_000__BUTTON_00_RELEASE,
        LA_MENUWINDOW2_MENU2_MIN_LOW_000__BUTTON_00_CANSEL,
        LA_MENUWINDOW2_MENU2_MIN_LOW_000__BUTTON_00_TOUCH_RELEASE,
        LA_MENUWINDOW2_MENU2_MIN_LOW_000__BUTTON_00_ACTIVE,
        LA_MENUWINDOW2_MENU2_MIN_LOW_000__BUTTON_00_PASSIVE,
      },
      {
        BUTTON_2of2,                                                            //  app::tool::ButtonManager::ButtonId button_id;
        helper.Clear().Push(PANENAME_MENU2_MIN_LOW_000_PANE_BUTTON_01).Peek(),  //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(PANENAME_FIELDMENU_BTN_LOW_000_PANE_BOUND_00),   //  gfl2::lyt::LytPane* bound_pane;
        LA_MENUWINDOW2_MENU2_MIN_LOW_000__BUTTON_01_TOUCH,
        LA_MENUWINDOW2_MENU2_MIN_LOW_000__BUTTON_01_RELEASE,
        LA_MENUWINDOW2_MENU2_MIN_LOW_000__BUTTON_01_CANSEL,
        LA_MENUWINDOW2_MENU2_MIN_LOW_000__BUTTON_01_TOUCH_RELEASE,
        LA_MENUWINDOW2_MENU2_MIN_LOW_000__BUTTON_01_ACTIVE,
        LA_MENUWINDOW2_MENU2_MIN_LOW_000__BUTTON_01_PASSIVE,
      },
    };

    CreateButtonManager( m_pHeap, pLayout, buttonInfoEx, GFL_NELEMS(buttonInfoEx) );
    SetButtonSE_Preset();
  }

  /*
    cursor
  */
  {
    m_CursorController.Setup(
                              pCursorResourceData,
                              pG2DUtil->GetLayoutSystem(),
                              pG2DUtil->GetLayoutWorkSetup((m_DisplayType == app::util::G2DUtil::SETUP_UPPER) ? gfl2::lyt::DISPLAY_UPPER : gfl2::lyt::DISPLAY_LOWER),
                              pG2DUtil->GetAppLytAccessor(),
                              (m_DisplayType == app::util::G2DUtil::SETUP_UPPER)
                            );
    m_CursorController.SetEventHandler(this);
    m_CursorController.SetLocationNum(BUTTON_MAX);
    m_CursorController.RegisterLocation(BUTTON_1of2, pLayout, helper.Clear().Push(PANENAME_MENU2_MIN_LOW_000_PANE_BUTTON_00).GetPane(PANENAME_FIELDMENU_BTN_LOW_000_PANE_NULL_CUR));
    m_CursorController.RegisterLocation(BUTTON_2of2, pLayout, helper.Clear().Push(PANENAME_MENU2_MIN_LOW_000_PANE_BUTTON_01).GetPane(PANENAME_FIELDMENU_BTN_LOW_000_PANE_NULL_CUR));
  }


  m_State  = STATE_Running;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  TwoButtonsView::ObserveSetup(void)
{
  if(m_State  == STATE_Loading)
  {
    for(u32 index = 0; index < GFL_NELEMS(m_pResourceLoaders); ++index)
    {
      if(!m_pResourceLoaders[index]->ObserveLoad())
      {
        return false;
      }
    }
    m_State = STATE_SetupWait;

    /*  auto setup  */
    Setup(m_pResourceLoaders[LOADER_Layout]->GetData(), m_pResourceLoaders[LOADER_Cursor]->GetData());
  }


  return (m_State  == STATE_Running);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  TwoButtonsView::SetTextBoxString(const TextBoxID textBoxID, gfl2::str::StrBuf* pStr, const bool isExpand)
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
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  TwoButtonsView::SetButtonSE_Preset(const SEPreset presetID)
{
  ButtonManager*  pButtonManager = GetButtonManager();

  if(!pButtonManager) {
    return;
  }

  switch(presetID)
  {
    case  SEPRESET_None:              /**<  無音              */
      SetButtonSE(BUTTON_1of2, ButtonManager::SE_NULL);
      SetButtonSE(BUTTON_2of2, ButtonManager::SE_NULL);
      break;

    case  SEPRESET_Decide_Cancel:     /**<  決定/キャンセル   */
      SetButtonSE(BUTTON_1of2, SEID_DECIDE);
      SetButtonSE(BUTTON_2of2, SEID_CANCEL);
      break;

    case  SEPRESET_Decide:            /**<  決定/決定         */
    default:
      SetButtonSE(BUTTON_1of2, SEID_DECIDE);
      SetButtonSE(BUTTON_2of2, SEID_DECIDE);
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  TwoButtonsView::Update(void)
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
void  TwoButtonsView::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  /*    */
  if(m_State  == STATE_Running)
  {
    GetG2DUtil()->AddDrawableLytWkForOneFrame(m_pRenderingManager, displayNo, 0);
  }

  /*  cursor  */
  m_CursorController.OnDraw(displayNo);
}


/*  implement UIInputListener I/F  */
//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  TwoButtonsView::OnLayoutPaneTouchTriggerEvent( const u32 buttonID )
{
  switch(buttonID)
  {
    case  BUTTON_1of2: m_CursorController.MoveTo(0); break;
    case  BUTTON_2of2: m_CursorController.MoveTo(1); break;
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
app::ui::UIInputListener::ListenerResult  TwoButtonsView::OnLayoutPaneEvent( const u32 buttonID )
{
  app::ui::UIInputListener::ListenerResult  result  = ENABLE_ACCESS;

  switch(buttonID)
  {
    case  BUTTON_1of2:      /**<  ボタン0が押された     */
      Event(IEventHandler::EVENT_OnButton1Click);
      result  = DISABLE_ACCESS;
      break;

    case  BUTTON_2of2:      /**<  ボタン1が押された     */
      Event(IEventHandler::EVENT_OnButton2Click);
      result  = DISABLE_ACCESS;
      break;
  }

  return result;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult  TwoButtonsView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
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
void  TwoButtonsView::CursorController_OnEvent(CursorController* pSender, const CursorController::IEventHandler::EventCode eventCode)
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
bool  TwoButtonsView::IsCursorVisible(void) const
{
  return (m_CursorController.IsVisible());
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  TwoButtonsView::CursorMoveTo(const u32 index)
{
  m_CursorController.MoveTo(index);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  TwoButtonsView::Event(const IEventHandler::EventCode eventCode)
{
  if(m_pHandler)
  {
    m_pHandler->TwoButtonsView_OnEvent(eventCode);
  }
}


} /*  namespace tool  */
} /*  namespace app   */

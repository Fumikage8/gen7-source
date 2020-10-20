//==============================================================================
/**
 * @file   CommonMessageWindowView.cpp
 * @date   2015/07/03 Fri. 14:23:14
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <niji_conv_header/app/common_menu/MenuWindow.h>
#include  <niji_conv_header/app/common_menu/MenuWindow_anim_list.h>
#include  <niji_conv_header/app/common_menu/MenuWindow_pane.h>

#include  "AppLib/include/Tool/app_tool_MenuWindow.h"
#include  "AppLib/include/Tool/CommonMessageWindowView.h"


// =============================================================================
/**/
//==============================================================================
namespace
{
  enum  {
    LAYOUTWORK_MessageWindow,

    LAYOUTWORK_MAX
  };
}


namespace App {
namespace Tool  {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     CommonMessageWindowView
  *  @brief    ctor
  */
//------------------------------------------------------------------
CommonMessageWindowView::CommonMessageWindowView(const PARAM& rParam, IEventHandler* pHandler)
  : app::ui::UIView(rParam.pHeap)
  , m_Param(rParam)
  , m_MessageWindowManipulator()
  , m_pHandler(pHandler)
  , m_pPictureBG(NULL)
  , m_StrBuf(STR_MAX, m_Param.pHeap->GetDeviceHeap())
  , m_WordSet( m_Param.pHeap->GetDeviceHeap() )
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    dtor
  */
//------------------------------------------------------------------
CommonMessageWindowView::~CommonMessageWindowView()
{
  Delete2D();
  DeleteGraphicsSystem();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  CommonMessageWindowView::Setup(void* pLayoutBuffer)
{
  GFL_ASSERT(!GetG2DUtil());    /*  create2dされていないこと  */
  GFL_ASSERT(pLayoutBuffer);

  app::util::Heap*  pHeap = m_Param.pHeap;


  /*  setupLayoutSystem  */
  {
    const app::util::G2DUtil::LytResData  lytResDataTable[] =
    {
      { pLayoutBuffer, 0, app::util::G2DUtil::ATTACH_NONE },
    };

    const app::util::G2DUtil::LytwkData   layoutWorkTable[LAYOUTWORK_MAX] =
    {
      {
        0,                                          //!< 使用するリソースの番号         : lytResDataTable
        LYTRES_MENUWINDOW_MENU_MIN_LOW_000_BFLYT,   //!< レイアウトデータID ( *_BFLYT ) : niji_conv_header/app/gift/Gift.h
        LA_MENUWINDOW_MENU_MIN_LOW_000___NUM,       //!< アニメ数                       : niji_conv_header/app/gift/Gift_anim_list.h
        pLayoutBuffer,                              //!< リソースデータ                 : pWork->ResourceManager->ResourceBuffer
        true,                                       //!< 非表示ペインに行列計算を行うか
        app::util::G2DUtil::SETUP_LOWER,            //!< ディスプレイ
        true                                        //!< 表示設定
      },
    };


    /*
      UIView
    */
    SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
    CreateGraphicsSystem( pHeap, false );

    Create2D(
      pHeap,                            /*  app::util::Heap*    */
      NULL,                             /*  gfl2::lyt::LytSys                 (NULLなら内部で新規に生成する。既存のものを使用する場合にはここで指定する)  */
      LAYOUTWORK_MAX,                   /*  u32                               layoutを扱う数（capacity）。内部ワークの数  */
      lytResDataTable,                  /*  app::util::G2DUtil::LytResData*   リソーステーブル        */
      GFL_NELEMS(lytResDataTable),      /*  u32                               リソーステーブル用素数  */
      layoutWorkTable,                  /*  app::util::G2DUtil::LytwkData*    ワークテーブル          */
      GFL_NELEMS(layoutWorkTable),      /*  u32                               ワークテーブル用素数    */
      (gfl2::str::MsgData*)NULL,
      &m_WordSet  // 使わないので必要ないが、ないとapp_util_2dでアサートが出る
    );


    /*  BGpane取得  */
    m_pPictureBG  = GetG2DUtil()->GetLayoutWork(LAYOUTWORK_MessageWindow)->GetPicturePane(PANENAME_MENU_MIN_LOW_000_PANE_BG_00);
    m_pPictureBG->SetVisible(false);
  }

  /*  汎用メッセージウインドウ  */
  {
    CommonMessageWindowManipulator::SSetupInfo setupInfo;

    setupInfo.pHeap               = pHeap;
    setupInfo.pTargetView         = this;
    setupInfo.layoutWorkID        = LAYOUTWORK_MessageWindow;
    setupInfo.pHandler            = this;

//    setupInfo.animID_TimerIcon    = LA_MENUWINDOW_MENU_MIN_LOW_000__TIMERICON_KEEP;

    setupInfo.animID_TimerIcon      = LA_MENUWINDOW_MENU_MIN_LOW_000__MESSAGE__TIMERICON_KEEP;
    setupInfo.animID_TimerIcon_In   = LA_MENUWINDOW_MENU_MIN_LOW_000__MESSAGE__TIMERICON_IN;
    setupInfo.animID_TimerIcon_Out  = LA_MENUWINDOW_MENU_MIN_LOW_000__MESSAGE__TIMERICON_OUT;







    setupInfo.animID_MsgCursor    = LA_MENUWINDOW_MENU_MIN_LOW_000__MESSAGE__CURSOR_00_KEEP;
    setupInfo.paneID_WindowParts  = PANENAME_MENU_MIN_LOW_000_PANE_MESSAGE;
    setupInfo.paneID_TextBox2     = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2;
    setupInfo.paneID_NullText1    = PANENAME_COMMON_MSG_LOW_000_PANE_NULL_TEXT1;
    setupInfo.paneID_TextBox1a    = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1A;
    setupInfo.paneID_TextBox2a    = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1B;


//    setupInfo.paneID_TimerIcon    = PANENAME_MENU_MIN_LOW_000_PANE_TIMERICON;
    setupInfo.paneID_TimerIcon    = PANENAME_COMMON_MSG_LOW_000_PANE_TIMERICON;



    setupInfo.paneID_MsgCursor    = PANENAME_COMMON_MSG_LOW_000_PANE_CURSOR_00;

    m_MessageWindowManipulator.Setup(setupInfo);
  }
}


//------------------------------------------------------------------
/**
  *  @func     SetMessageSpeed
  *  @brief    メッセージスピードを設定する
  */
//------------------------------------------------------------------
void  CommonMessageWindowView::SetMessageSpeed(Savedata::ConfigSave::MSG_SPEED spd)
{
  m_MessageWindowManipulator.SetMessageSpeed(spd);
}


//--------------------------------------------------------------------------------------------
/**
  * @brief     メッセージ表示
  */
//--------------------------------------------------------------------------------------------
void  CommonMessageWindowView::ShowMessage(const gfl2::str::MsgData& rMessageData, const u32 messageID, const bool isImmediateMessage, const bool isShowTimerIcon, const bool isShowBG)
{
  m_StrBuf.Clear();

  const_cast<gfl2::str::MsgData&>(rMessageData).GetString(messageID, m_StrBuf);

  ShowMessage(&m_StrBuf, isImmediateMessage, isShowTimerIcon, isShowBG);
}



//--------------------------------------------------------------------------------------------
/**
  * @brief     メッセージ表示
  */
//--------------------------------------------------------------------------------------------
void  CommonMessageWindowView::ShowMessage(const gfl2::str::StrBuf* pStrBuf, const bool isImmediateMessage, const bool isShowTimerIcon, const bool isShowBG)
{
  m_MessageWindowManipulator.ShowMessage(pStrBuf, isImmediateMessage, isShowTimerIcon);
  if(m_pPictureBG)
  {
    m_pPictureBG->SetVisible(isShowBG);
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  CommonMessageWindowView::HideMessage(void)
{
  m_MessageWindowManipulator.HideMessage();
  if(m_pPictureBG)
  {
    m_pPictureBG->SetVisible(false);
  }
}

/* -------------------------------------------------------------------------*/
/**
 * @brief キー入力可否設定
 * @note  trueで一時キー入力無効
 */
/* -------------------------------------------------------------------------*/
void CommonMessageWindowView::SetSuspendKey( bool flag )
{
  m_MessageWindowManipulator.SetSuspendKey( flag );
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  CommonMessageWindowView::Update(void)
{
  app::ui::UIView::Update();    /*  super  */

  m_MessageWindowManipulator.UpdateWindow();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  CommonMessageWindowView::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  GetG2DUtil()->AddDrawableLytWkForOneFrame( m_Param.pRenderingManager, displayNo, LAYOUTWORK_MessageWindow );
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    manipulatorからのeventをハンドリング
  */
//------------------------------------------------------------------
void  CommonMessageWindowView::CommonMessageWindowManipulator_OnEvent(const CommonMessageWindowManipulator::EventID event)
{
  if(!m_pHandler)  return;

  switch(event)
  {
    case  CommonMessageWindowManipulator::EVENT_OnEOM:
      m_pHandler->CommonMessageWindowView_OnEvent(IEventHandler::EVENT_OnEOM);
      break;

    case  CommonMessageWindowManipulator::EVENT_OnFinish:
      m_pHandler->CommonMessageWindowView_OnEvent(IEventHandler::EVENT_OnFinish);
      break;
  }
}



} /*  namespace Tool  */
} /*  namespace App   */


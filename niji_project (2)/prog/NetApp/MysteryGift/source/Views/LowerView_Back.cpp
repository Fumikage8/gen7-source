
//==============================================================================
/**
 * @file   LowerView_Back.cpp
 * @date   2015/08/04 Tue. 11:29:04
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================

// =============================================================================
/**/
//==============================================================================
#include  "../ApplicationSystem/GiftWork.h"

#include  "../GiftResource.h"
#include  "../Common/ButtonHelper.h"

#include  "./LowerView_Back.h"

// =============================================================================
/**/
//==============================================================================
/*
  local
*/
namespace
{
  /** layout work  */
  enum  {
    LAYOUTWORK_BackLow,

    LAYOUTWORK_MAX
  };

  /**
    ButtonID
  */
  enum  {
    BUTTON_Back,      /**<  戻る        */

    BUTTON_MAX
  };



  /**
    ボタン定義
  */
  const NetApp::Gift::ButtonHelperInfo buttonHelperInfo[] = {
      /*  終了  */
      NetApp::Gift::ButtonHelperInfo(
        BUTTON_Back
        , PANENAME_GIFT_BCK_LOW_000_PANE_BTN_BACK
        , PANENAME_GIFT_BTN_LOW_002_PANE_BOUND_00
        , true
        , LA_GIFT_GIFT_BCK_LOW_000__BTN_BACK_TOUCH
        , LA_GIFT_GIFT_BCK_LOW_000__BTN_BACK_RELEASE
        , LA_GIFT_GIFT_BCK_LOW_000__BTN_BACK_CANSEL
        , LA_GIFT_GIFT_BCK_LOW_000__BTN_BACK_TOUCH_RELEASE
        , LA_GIFT_GIFT_BCK_LOW_000__BTN_BACK_PASSIVE
        , LA_GIFT_GIFT_BCK_LOW_000__BTN_BACK_ACTIVE
      ),
    };

} /*  namespace (noname)  */


namespace NetApp  {
namespace Gift    {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @brief    ctor
  */
//------------------------------------------------------------------
LowerView_Back::LowerView_Back(GiftWork* pWork)
  : NetAppLib::UI::NetAppCursorView(pWork)
  , m_pWork(pWork)
  , m_pHandler(NULL)
  , m_pMessageMenu(NULL)
  , m_ViewMode(VIEWMODE_None)
{
  app::util::Heap*     pHeap  = m_pWork->GetAppHeap();

  /*  setupLayoutSystem  */
  {
    NetAppLib::System::ResourceManager* pResourceManager  = m_pWork->GetResourceManager();
    void*                               pLayoutBuffer     = pResourceManager->GetResourceBuffer(RESOURCE_Layout);

    const app::util::G2DUtil::LytResData  lytResDataTable[] = {
      { pLayoutBuffer, 0, app::util::G2DUtil::ATTACH_NONE },
    };

    const app::util::G2DUtil::LytwkData   layoutWorkTable[LAYOUTWORK_MAX] =
    {
      {
        0,                                    //!< 使用するリソースの番号         : lytResDataTable
        LYTRES_GIFT_GIFT_BCK_LOW_000_BFLYT,   //!< レイアウトデータID ( *_BFLYT ) : niji_conv_header/app/gift/Gift.h
        LA_GIFT_GIFT_BCK_LOW_000___NUM,       //!< アニメ数                       : niji_conv_header/app/gift/Gift_anim_list.h
        pLayoutBuffer,                        //!< リソースデータ                 : pWork->ResourceManager->ResourceBuffer
        true,                                 //!< 非表示ペインに行列計算を行うか
        app::util::G2DUtil::SETUP_LOWER,      //!< ディスプレイ
        true                                  //!< 表示設定
      },
    };


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
      m_pWork->GetMessageData(),        /*  gfl2::str::MsgData *messageData   */
      m_pWork->GetWordSet()             /*  print::WordSet *wordset,          */
    );
  }

  /*  messageMenu  */
  {
    m_pMessageMenu  = GFL_NEW(pHeap->GetDeviceHeap()) GiftMessageMenu(pWork, this);
    AddSubView(m_pMessageMenu);
  }


  /*  button  */
  CreateButton(pHeap, this, LAYOUTWORK_BackLow, 0, buttonHelperInfo, GFL_NELEMS(buttonHelperInfo));
  GetButtonManager()->SetButtonSelectSE(BUTTON_Back, SEID_CANCEL);
  SetInputListener( this );
  RegisterShortcut_B(BUTTON_Back);
}


//------------------------------------------------------------------
/**
  *  @brief    dtor
  */
//------------------------------------------------------------------
LowerView_Back::~LowerView_Back()
{
  if(m_pMessageMenu)  m_pMessageMenu->RemoveFromSuperView();

  GFL_SAFE_DELETE(m_pMessageMenu);
  Delete2D();
}


//--------------------------------------------------------------------------------------------
/**
* @brief  描画処理
*         子ウインドウについても自動的にコールする(親→子の順）
*/
//--------------------------------------------------------------------------------------------
void  LowerView_Back::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  GetG2DUtil()->AddDrawableLytWkForOneFrame(m_pWork->GetAppRenderingManager(), displayNo, LAYOUTWORK_BackLow);
  NetAppLib::UI::NetAppCursorView::Draw(displayNo);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_Back::SetViewMode(const ViewMode viewMode)
{
  m_ViewMode  = viewMode;

  switch(m_ViewMode)
  {
    case  VIEWMODE_None:
      m_pMessageMenu->HideMessage();
      break;

    case  VIEWMODE_Finalizing:              /**<  終了しています                */
      m_pMessageMenu->Show(GiftMessageMenu::MODE_Finalizing);
      break;

    /*  common  */
    case  VIEWMODE_Searching:               /**<  おくりものを探しています      */
      m_pMessageMenu->Show(GiftMessageMenu::MODE_Searching);
      break;

    case  VIEWMODE_ReceiveCancelConfirm:    /**<  うけとるのをやめますか Y/N    */
      m_pMessageMenu->Show(GiftMessageMenu::MODE_ReceiveCancelConfirm);
      break;

    /*  error  */
    case  VIEWMODE_Error_ReceiveFailed:    /**<  受け取りに失敗した  */
      m_pMessageMenu->Show(GiftMessageMenu::MODE_Failed);
      break;


    case  VIEWMODE_Error_NotFound:         /**<  見つからなかった    */
      m_pMessageMenu->Show(GiftMessageMenu::MODE_GiftNotThere);
      break;

    case  VIEWMODE_Error_FoundButCannotReceive:   /**<  見つけたけど受け取れない  */
      m_pMessageMenu->Show(GiftMessageMenu::MODE_FoundButCannotReceive);
      break;


    case  VIEWMODE_Error_NoData:         /**<  ありませんでした    */
      m_pMessageMenu->Show(GiftMessageMenu::MODE_GiftNotThere);
      break;


    case  VIEWMODE_Error_WirelessOff:      /**<  無線OFFです                   */
      m_pMessageMenu->Show(GiftMessageMenu::MODE_WirelessSwitchOff);
      break;


    case  VIEWMODE_Error_TimeoutPenalty:   /**<  続けて失敗したためしばらくたってから                */
      m_pMessageMenu->Show(GiftMessageMenu::MODE_Serial_Error_TimeoutPenalty);
      break;






    case  VIEWMODE_IrConfirm:               /**<  赤外線を向かい合わせて Y/N    */
      m_pMessageMenu->Show(GiftMessageMenu::MODE_StartIr);
      break;

    case  VIEWMODE_Serial_1stConfirm:           /**<  ご注意！～シリアルコードで受け取りますか？ Y/N      */
      m_pMessageMenu->Show(GiftMessageMenu::MODE_Serial_1stConfirm);
      break;

    case  VIEWMODE_Serial_InternetConfirm:      /**<  インターネットに接続します Y/N                      */
      m_pMessageMenu->Show(GiftMessageMenu::MODE_InternetConnectionConfirm);
      break;

    case  VIEWMODE_Serial_Connecting:           /**<  接続しています                                      */
      m_pMessageMenu->Show(GiftMessageMenu::MODE_Connecting);
      break;

    case  VIEWMODE_Serial_UpdateFound:          /**<  追加データが見つかりました eshopにいきますか？ Y/N  */
      m_pMessageMenu->Show(GiftMessageMenu::MODE_UpdateFound);
      break;

    case  VIEWMODE_Serial_InternetNotAvailable: /**<  インターネットに接続できないため終了                */
      m_pMessageMenu->Show(GiftMessageMenu::MODE_InternetNotAvailable);
      break;

    case  VIEWMODE_Serial_EshopNotAvailable:    /**<  eshopがないので更新してください                     */
      m_pMessageMenu->Show(GiftMessageMenu::MODE_EshopNotAvailable);
      break;

    case  VIEWMODE_Serial_InputSerial:          /**<  お手持ちのシリアルコードを入力してください          */
      m_pMessageMenu->Show(GiftMessageMenu::MODE_Serial_InputSerial);
      break;

    case  VIEWMODE_Serial_Authentication:       /**<  シリアルコードを確認しています                      */
      m_pMessageMenu->Show(GiftMessageMenu::MODE_Serial_Authentication);
      break;

    case  VIEWMODE_Serial_RetryConfirm:         /**<  もう一度シリアルコードを入れますか？ Y/N            */
      m_pMessageMenu->Show(GiftMessageMenu::MODE_Serial_RetryConfirm);
      break;
  }
}



//--------------------------------------------------------------------------------------------
/**
  * @brief ペインイベントの検知
  */
//--------------------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult  LowerView_Back::OnLayoutPaneEvent( const u32 buttonID )
{
  app::ui::UIInputListener::ListenerResult  result  = app::ui::UIInputListener::ENABLE_ACCESS;


  GFL_PRINT("button[%02d] event\n", buttonID);
  switch(buttonID)
  {
    /*
      exit
    */
    case  BUTTON_Back:  /**<  戻る  */
      Event(IEventHandler::EVENT_Exit);
      result  = app::ui::UIInputListener::DISABLE_ACCESS;
      break;

    default:
      break;
  }


  return  result;
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Back::Event(const IEventHandler::EventCode event)
{
  if(m_pHandler)
  {
    m_pHandler->LowerView_Back_OnEvent(event);
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
void  LowerView_Back::MessageMenuView_OnEvent(const GiftMessageMenu::IEventHandler::EventCode event)
{
  switch(event)
  {
    case  GiftMessageMenu::IEventHandler::EVENT_OnEOM:            /**<  メッセージ表示が終端に到達                                  */
//    case  GiftMessageMenu::IEventHandler::EVENT_OnFinish:         /**<  EOM後、ユーザの入力まちなどまでが完了した                   */
      Event(IEventHandler::EVENT_MessageFinish);
      break;

    case  GiftMessageMenu::IEventHandler::EVENT_OnSelectedYes:    /**<  Yes/NoモードでYesが選択された                               */
      if(m_ViewMode == VIEWMODE_ReceiveCancelConfirm)         Event(IEventHandler::EVENT_ReceiveCancelConfirm_Yes);
      else if(m_ViewMode == VIEWMODE_IrConfirm)               Event(IEventHandler::EVENT_IrConfirm_Yes);
      else if(m_ViewMode == VIEWMODE_Serial_1stConfirm)       Event(IEventHandler::EVENT_1stConfirm_Yes);             /**<  ご注意！～シリアルコードで受け取りますか？ Y/N      */
      else if(m_ViewMode == VIEWMODE_Serial_InternetConfirm)  Event(IEventHandler::EVENT_InternetConfirm_Yes);        /**<  インターネットに接続します Y/N                      */
      else if(m_ViewMode == VIEWMODE_Serial_UpdateFound)      Event(IEventHandler::EVENT_TransitToEshopConfirm_Yes);  /**<  追加データが見つかりました eshopにいきますか？ Y/N  */
      else if(m_ViewMode == VIEWMODE_Serial_RetryConfirm)     Event(IEventHandler::EVENT_SerialRetryConfirm_Yes);     /**<  もう一度シリアルコードを入れますか？ Y/N            */
      break;

    case  GiftMessageMenu::IEventHandler::EVENT_OnSelectedNo:     /**<  Yes/NoモードでNoが選択された                                */
      if(m_ViewMode == VIEWMODE_ReceiveCancelConfirm)         Event(IEventHandler::EVENT_ReceiveCancelConfirm_No);
      else if(m_ViewMode == VIEWMODE_IrConfirm)               Event(IEventHandler::EVENT_IrConfirm_No);
      else if(m_ViewMode == VIEWMODE_Serial_1stConfirm)       Event(IEventHandler::EVENT_1stConfirm_No);              /**<  ご注意！～シリアルコードで受け取りますか？ Y/N      */
      else if(m_ViewMode == VIEWMODE_Serial_InternetConfirm)  Event(IEventHandler::EVENT_InternetConfirm_No);         /**<  インターネットに接続します Y/N                      */
      else if(m_ViewMode == VIEWMODE_Serial_UpdateFound)      Event(IEventHandler::EVENT_TransitToEshopConfirm_No);   /**<  追加データが見つかりました eshopにいきますか？ Y/N  */
      else if(m_ViewMode == VIEWMODE_Serial_RetryConfirm)     Event(IEventHandler::EVENT_SerialRetryConfirm_No);      /**<  もう一度シリアルコードを入れますか？ Y/N            */
      break;

    case  GiftMessageMenu::IEventHandler::EVENT_OnSelectedItem:   /**<  リスト選択でItemが選択された: GetSelectedItemIndex()で取得  */
    default:
      /*  nop  */
      break;
  }
}





} /*  namespace Gift    */
} /*  namespace NetApp  */

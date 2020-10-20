
//==============================================================================
/**
 * @file   UpperView_MessageBase.cpp
 * @date   2015/07/31 Fri. 20:23:23
 * @author muto_yu
 * @brief  ふしぎなおくりもの 上画面 メッセージ表示
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "AppLib/include/Util/app_util_GetPaneHelper.h"
#include  "../../ApplicationSystem/GiftWork.h"

#include  "../../GiftResource.h"
#include  "./UpperView_MessageBase.h"

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
namespace Gift    {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     UpperView_MessageBase
  *  @brief    ctor
  */
//------------------------------------------------------------------
UpperView_MessageBase::UpperView_MessageBase(GiftWork* pWork, const bool isLower)
  : NetAppLib::UI::NetAppEmptyView( pWork )
  , m_pWork(pWork)
  , m_p2LineMessageWindow(NULL)
  , m_pTextBoxForImmediate(NULL)
  , m_pWindowRootPane(NULL)
  , m_pTimerIconPartsPane(NULL)
{
  app::util::Heap*  pHeap = m_pWork->GetAppHeap();
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
        LYTRES_GIFT_GIFT_MSG_UPP_000_BFLYT,   //!< レイアウトデータID ( *_BFLYT ) : niji_conv_header/app/gift/Gift.h
        LA_GIFT_GIFT_MSG_UPP_000___NUM,       //!< アニメ数                       : niji_conv_header/app/gift/Gift_anim_list.h
        pLayoutBuffer,                        //!< リソースデータ                 : pWork->ResourceManager->ResourceBuffer
        true,                                 //!< 非表示ペインに行列計算を行うか
        isLower ? app::util::G2DUtil::SETUP_LOWER : app::util::G2DUtil::SETUP_UPPER,      //!< ディスプレイ
        true                                  //!< 表示設定
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
      m_pWork->GetMessageData(),        /*  gfl2::str::MsgData *messageData   */
      m_pWork->GetWordSet()             /*  print::WordSet *wordset,          */
    );

  }

  /*  2行表示ウインドウ  */
  {
    gfl2::lyt::LytWk*                 pLayoutWork     = GetG2DUtil()->GetLayoutWork(LAYOUTWORK_Message);

    m_p2LineMessageWindow   = GFL_NEW( pHeap->GetDeviceHeap() )
                                print::MessageWindow (
                                  GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager(),
                                  GetG2DUtil()->GetLayoutWork(LAYOUTWORK_Message),
                                  2,
                                  pHeap->GetDeviceHeap()
                                );
    m_p2LineMessageWindow->AddLinePaneIdxPtr(pLayoutWork->GetTextBoxPane(PANENAME_GIFT_MSG_UPP_000_PANE_TEXT_L2_1));
    m_p2LineMessageWindow->AddLinePaneIdxPtr(pLayoutWork->GetTextBoxPane(PANENAME_GIFT_MSG_UPP_000_PANE_TEXT_L2_2));
  }


  {
    gfl2::lyt::LytMultiResID&   rLytMultiResID  = GetG2DUtil()->GetLayoutResourceID(LAYOUTWORK_Message);
    gfl2::lyt::LytWk*           pLayoutWork     = GetG2DUtil()->GetLayoutWork(LAYOUTWORK_Message);
    app::util::GetPaneHelper    helper(pLayoutWork, &rLytMultiResID);

    /*  各種Paneを保持  */
    helper.Clear();
    m_pTextBoxes[TEXTBOX_Large]                   = helper.GetTextBoxPane(PANENAME_GIFT_MSG_UPP_000_PANE_TEXT_L10   );  /**<  大                 */
    m_pTextBoxes[TEXTBOX_1of2]                    = helper.GetTextBoxPane(PANENAME_GIFT_MSG_UPP_000_PANE_TEXT_L2_1  );  /**<  2行表示1行目       */
    m_pTextBoxes[TEXTBOX_2of2]                    = helper.GetTextBoxPane(PANENAME_GIFT_MSG_UPP_000_PANE_TEXT_L2_2  );  /**<  2行表示2行目       */
    m_pTextBoxes[TEXTBOX_Serial_1of4]             = helper.GetTextBoxPane(PANENAME_GIFT_MSG_UPP_000_PANE_TEXTCODE_00);  /**<  シリアルコード 1/4 */
    m_pTextBoxes[TEXTBOX_Serial_2of4]             = helper.GetTextBoxPane(PANENAME_GIFT_MSG_UPP_000_PANE_TEXTCODE_01);  /**<  シリアルコード 2/4 */
    m_pTextBoxes[TEXTBOX_Serial_3of4]             = helper.GetTextBoxPane(PANENAME_GIFT_MSG_UPP_000_PANE_TEXTCODE_02);  /**<  シリアルコード 3/4 */
    m_pTextBoxes[TEXTBOX_Serial_4of4]             = helper.GetTextBoxPane(PANENAME_GIFT_MSG_UPP_000_PANE_TEXTCODE_03);  /**<  シリアルコード 4/4 */
    m_pTextBoxes[TEXTBOX_DataInfo_EventName]      = helper.GetTextBoxPane(PANENAME_GIFT_MSG_UPP_000_PANE_TEXT_L1_00);   /**<  受信データ情報：配信名       */
    m_pTextBoxes[TEXTBOX_DataInfo_Description]    = helper.GetTextBoxPane(PANENAME_GIFT_MSG_UPP_000_PANE_TEXT_L7);      /**<  受信データ情報：説明         */
    m_pTextBoxes[TEXTBOX_DataInfo_Status]         = helper.GetTextBoxPane(PANENAME_GIFT_MSG_UPP_000_PANE_TEXT_L1_01);   /**<  受信データ情報：受け取り状態 */


    /*  メッセージウインドウ  */
    helper.Push(PANENAME_GIFT_MSG_UPP_000_PANE_MESSAGE);

    GetG2DUtil()->CreateMessageWindow(
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager(),
      LAYOUTWORK_Message,
      helper.GetTextBoxPane(PANENAME_COMMON_MSG_UPP_000_PANE_TEXTBOX_01),
      helper.GetTextBoxPane(PANENAME_COMMON_MSG_UPP_000_PANE_TEXTBOX_02),
      pHeap
    );

    m_pTextBoxForImmediate    = helper.GetTextBoxPane(PANENAME_COMMON_MSG_UPP_000_PANE_TEXTBOX_00);
    m_pWindowRootPane         = helper.GetPane(PANENAME_COMMON_MSG_UPP_000_PANE_NULL_TEXT1);
    m_pTimerIconPartsPane     = helper.Clear().GetPartsPane(PANENAME_GIFT_MSG_UPP_000_PANE_TIMERICON);

    /*  timerIcon  */
    GetG2DUtil()->StartAnime(LAYOUTWORK_Message, LA_GIFT_GIFT_MSG_UPP_000__TIMERICON_KEEP);
  }
}


//--------------------------------------------------------------------------------------------
/**
* @brief    デストラクタ
*/
//--------------------------------------------------------------------------------------------
UpperView_MessageBase::~UpperView_MessageBase()
{
  m_p2LineMessageWindow->Clear();
  GFL_SAFE_DELETE( m_p2LineMessageWindow );
  Delete2D();
  DeleteGraphicsSystem();
}



//--------------------------------------------------------------------------------------------
/**
  * @brief  更新処理
  */
//--------------------------------------------------------------------------------------------
void  UpperView_MessageBase::Update(void)
{
  NetAppLib::System::NetApplicationViewBase::Update();    /*  super  */

  m_p2LineMessageWindow->Proc();
  GetG2DUtil()->PrintMessage();   /*  messageWindowのupdate  */
}


//--------------------------------------------------------------------------------------------
/**
  * @brief  描画処理
  *         子ウインドウについても自動的にコールする(親→子の順）
*/
//--------------------------------------------------------------------------------------------
void  UpperView_MessageBase::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  GetG2DUtil()->AddDrawableLytWkForOneFrame(m_pWork->GetAppRenderingManager(), displayNo, LAYOUTWORK_Message);
}


//------------------------------------------------------------------
/**
  *  @brief    表示モード切り替え
  */
//------------------------------------------------------------------
void  UpperView_MessageBase::SetLayoutMode(const LayoutMode layoutMode)
{
  app::util::G2DUtil*   pG2DUtil  = GetG2DUtil();

  switch(layoutMode)
  {
    case  LAYOUT_MessageLarge:    /**<  メッセージプレート大    */
      pG2DUtil->StartAnime( LAYOUTWORK_Message, LA_GIFT_GIFT_MSG_UPP_000_WIN_LARGE);
      pG2DUtil->Update2D();
      break;

    case  LAYOUT_MessageSmall:    /**<  メッセージプレート小    */
      pG2DUtil->StartAnime( LAYOUTWORK_Message, LA_GIFT_GIFT_MSG_UPP_000_WIN_SMALL);
      pG2DUtil->Update2D();
      break;

    case  LAYOUT_SerialCode:      /**<  シリアルコード          */
      pG2DUtil->StartAnime( LAYOUTWORK_Message, LA_GIFT_GIFT_MSG_UPP_000_WIN_CODE);
      pG2DUtil->Update2D();
      break;

	case  LAYOUT_MessageWindow:       /**<  メッセージプレートのみ  */
      pG2DUtil->StartAnime( LAYOUTWORK_Message, LA_GIFT_GIFT_MSG_UPP_000_WIN_MESSAGE);
      pG2DUtil->Update2D();
      break;

  case  LAYOUT_ReceivedData:
      pG2DUtil->StartAnime( LAYOUTWORK_Message, LA_GIFT_GIFT_MSG_UPP_000_WIN_LARGE_01);
      pG2DUtil->Update2D();
      break;

    default:
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     SetText
  *  @brief    TextBoxのTextを設定
  */
//------------------------------------------------------------------
void  UpperView_MessageBase::SetText(const TextBoxID textBoxID, const u32 messageID)
{
  GetG2DUtil()->SetTextBoxPaneString( m_pTextBoxes[textBoxID], messageID );

}

//------------------------------------------------------------------
/**
  *  @func     Set2LineText
  *  @brief    2行表示用プレート（LAYOUT_MessageSmall）へのメッセージ設定
  */
//------------------------------------------------------------------
void  UpperView_MessageBase::Set2LineText(const u32 messageID)
{
  m_p2LineMessageWindow->SetString(*m_pWork->GetString(GARC_message_mystery_DAT, messageID));
}


//------------------------------------------------------------------
/**
  *  @func     ShowMessageWindow
  *  @brief    メッセージウインドウの表示
  */
//------------------------------------------------------------------
void  UpperView_MessageBase::ShowMessageWindow(const u32 messageID, const bool isImmediateMessage, const bool isShowTimerIcon)
{
  gfl2::str::StrBuf*  pStrBuf       = m_pWork->GetString(GARC_message_mystery_DAT, messageID);


  m_pTextBoxForImmediate->SetVisible(isImmediateMessage);
  m_pWindowRootPane->SetVisible(!isImmediateMessage);

  if(isImmediateMessage)
  {
    SetTextboxPaneString( m_pTextBoxForImmediate, pStrBuf );
  }
  else
  {
  //  void GetG2DUtil()->SetMessageCallback( print::MsgCallback * cb );
  //  void GetG2DUtil()->SetMessageFinishMode( gfl2::str::StrWin::FinishType type, u8 frame );
    GetG2DUtil()->SetMessage( pStrBuf );
  }

  ShowTimerIcon(isShowTimerIcon);
}



//------------------------------------------------------------------
/**
  *  @func     ShowTimerIcon
  *  @brief    タイマーアイコンの表示制御
  */
//------------------------------------------------------------------
void  UpperView_MessageBase::ShowTimerIcon(const bool isVisible)
{
  m_pTimerIconPartsPane->SetVisible(true);
  GetG2DUtil()->StartAnime(LAYOUTWORK_Message, isVisible ? LA_GIFT_GIFT_MSG_UPP_000__TIMERICON_IN : LA_GIFT_GIFT_MSG_UPP_000__TIMERICON_OUT);
}


//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
void  UpperView_MessageBase::UpdateReceivedDataInfo(const Savedata::MysteryGiftSave::MYSTERY_GIFT_RECV_DATA* pReceiveData, const bool bEnableNGMessage)
{
  app::util::G2DUtil*                                   pG2DUtil      = GetG2DUtil();
  NetAppLib::Message::MessageUtility&                   rMessageUtil  = m_pWork->GetMessageUtility();
  const Savedata::MysteryGiftSave::MYSTERY_GIFT_DATA*   pGiftData     = &(pReceiveData->dataBody);


  /*  配信名  */
  {
    gfl2::str::StrBuf tempStr(pGiftData->eventName, m_pWork->GetDeviceHeap()->GetLowerHandle());
    pG2DUtil->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_DataInfo_EventName], &tempStr);
  }
  /*  detailText  */
//  pG2DUtil->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_DataInfo_Description], &(rMessageUtil.GetString(GIFT_MESSAGE_CARD_GRAC_ID, pGiftData->cardMessageID)));
  pG2DUtil->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_DataInfo_Description], reinterpret_cast<const gfl2::str::STRCODE*>(pReceiveData->detailText));

  /*  受け取り可/不可  */
  if(bEnableNGMessage)
  {
    m_pTextBoxes[TEXTBOX_DataInfo_Status]->SetVisible(true);
    switch(Savedata::MysteryGiftSave::ReceiveCheck(pGiftData))
    {
      case  Savedata::MysteryGiftSave::RECEIVE_CHECK_RESULT_NG_TODAY:       /**<  NG   一日1回*/ 
        pG2DUtil->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_DataInfo_Status], &(rMessageUtil.GetString(GIFT_MESSAGE_GRAC_ID, msg_mystery_cap_25)));   /*  きょうはもううけとれません   */
        break;

      case  Savedata::MysteryGiftSave::REDEIVE_CHECK_RESULT_NG_TODAY_FULL:  /**<  NG   一日1回の履歴が一杯*/ 
        pG2DUtil->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_DataInfo_Status], &(rMessageUtil.GetString(GIFT_MESSAGE_GRAC_ID, msg_mystery_cap_25)));   /*  きょうはもううけとれません   */
        break;

      case  Savedata::MysteryGiftSave::RECEIVE_CHECK_RESULT_NG:             /**<  NG  受け取り済み*/ 
        pG2DUtil->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_DataInfo_Status], &(rMessageUtil.GetString(GIFT_MESSAGE_GRAC_ID, msg_mystery_cap_24)));   /*  すでにうけとっています       */
        break;

      case Savedata::MysteryGiftSave::RECEIVE_CHECK_RESULT_OK:
      case  Savedata::MysteryGiftSave::RECEIVE_CHECK_RESULT_NG_FULL:        /**<  NG カードアルバム一杯*/
      default:
        m_pTextBoxes[TEXTBOX_DataInfo_Status]->SetVisible(false);
        break;
    }
  }
  else
  {
    m_pTextBoxes[TEXTBOX_DataInfo_Status]->SetVisible(false);
  }
}


} /*  namespace Gift    */
} /*  namespace NetApp  */


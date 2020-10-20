//=============================================================================
/**
 * @file	 ListSampleFrame.cpp
 * @brief	 継承したテスト
 * @author k.ohno
 * @date	 2014.2.17
 */
//=============================================================================
#if PM_DEBUG

#include "ListSampleFrame.h"
#include <niji_conv_header/message/script/msg_test_event_0010.h>

#include <arc_def_index/arc_def.h>
#include <arc_index/msgwin_test.gaix>
#include <arc_index/fieldwindow.gaix>

#include <arc_index/script_message.gaix>


#include "AppLib/include/Frame/AppFrameManager.h"

#include "AppLib/include/Tool/ListMenu.h"   //


#include "arc_index/message.gaix"
#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_menuparts.h>



GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( List )


enum{
  MM_SEQ_NONE,
  MM_SEQ_YESNO,
  MM_SEQ_END,
};



//------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------
ListSampleFrame::ListSampleFrame(void)
{
  mReadSeq=0;
  m_visible=0;
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
ListSampleFrame::~ListSampleFrame()
{
}


void ListSampleFrame::Setup( APP_PARAM* pAppParam )
{
  mpAppHeap = pAppParam->mpAppHeap;
  mpAppRenderingManager = pAppParam->mpAppRenderingManager;
  mpAppParam = pAppParam;
}

enum{
  LOCAL_OPEN_START,
  LOCAL_READ_START,
  LOCAL_READ_WAIT,
  LOCAL_CLOSE_WAIT,
};






//------------------------------------------------------------------
/**
 * @brief	  PROCの初期化を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------
applib::frame::Result ListSampleFrame::InitFunc(void)
{
  GFL_ASSERT(mpAppParam);  //Setupしないうちに起動してはいけない  128

  gfl2::fs::AsyncFileManager* pAsyncFileMan = mpAppParam->mpAsyncFileManager;

    {
      App::Tool::ListMenu::PARAM param;

      param.pHeap = mpAppHeap;
      param.pAppRenderingManager = mpAppRenderingManager;
      param.pFileManager = pAsyncFileMan;
      param.lytRes = NULL;
      param.initPos = 0;
      param.listMax = 3;
      param.posTypeX = 3;
      param.posTypeY = 3;
      param.x = 3.0f;
      param.y = 3.0f;
      param.strPosX = App::Tool::ListMenu::HORIZONTALPOS_CENTER;
      param.startWait = 1;
      param.bMode = false;
      param.visible = true;
      param.autoDraw = false;
      param.winType = App::Tool::ListMenu::WIN_TYPE_WHITE;

      mpListMenu = GFL_NEW(mpAppHeap->GetDeviceHeap()) App::Tool::ListMenu(&param);

      mpMsgData = GFL_NEW(mpAppHeap->GetDeviceHeap()) gfl2::str::MsgData( print::GetMessageArcId_Debug(),
                                                                          GARC_debug_message_d_menuparts_DAT,
                                                                          mpAppHeap->GetDeviceHeap(),
                                                                          gfl2::str::MsgData::LOAD_FULL );

      mpTempStrBuf1 = GFL_NEW( mpAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, mpAppHeap->GetDeviceHeap() );
      mpTempStrBuf2 = GFL_NEW( mpAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, mpAppHeap->GetDeviceHeap() );
      mpTempStrBuf3 = GFL_NEW( mpAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, mpAppHeap->GetDeviceHeap() );

      mpMsgData->GetString(debug_menu_parts_launcher_02, *mpTempStrBuf1);
      mpListMenu->AddItem( 0, mpTempStrBuf1 );
      mpMsgData->GetString(debug_menu_parts_launcher_03, *mpTempStrBuf2);
      mpListMenu->AddItem( 1, mpTempStrBuf2 );
      mpMsgData->GetString(debug_menu_parts_launcher_04, *mpTempStrBuf3);
      mpListMenu->AddItem( 2, mpTempStrBuf3 );

      mpListMenu->Start();
      
      
     // mpListMenu->LoadResource();
      
    }
  return applib::frame::RES_FINISH;


}

//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン更新を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
applib::frame::Result ListSampleFrame::UpdateFunc(void)
{

  
  mpListMenu->UpdateTree();

  if(mpListMenu->IsInstanceDelete()){
    GFL_PRINT("選択番号は %d\n",mpListMenu->GetSelectId());
    return applib::frame::RES_FINISH;
    
  }

  gfl2::ui::TouchPanel * pTouchPanel;            //!< タッチパネル
  pTouchPanel = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );
  if(pTouchPanel->IsTouchTrigger()){
    mpListMenu->SetVisible( GFL_BOOL_CAST(m_visible) );
    m_visible = 1 - m_visible;
  }

  return applib::frame::RES_CONTINUE;
}


//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------
void ListSampleFrame::DrawFunc(gfl2::gfx::CtrDisplayNo no)
{
//  if(no == gfl2::gfx::CtrDisplayNo::LEFT){
    mpListMenu->DrawTree(no);
 // }

}

//------------------------------------------------------------------
/**
 * @brief	  PROCの終了を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------
applib::frame::Result ListSampleFrame::EndFunc(void)
{
  mpListMenu->DeleteMenu();
  GFL_DELETE mpListMenu;


  GFL_DELETE mpMsgData;

  GFL_DELETE mpTempStrBuf1;
  GFL_DELETE mpTempStrBuf2;
  GFL_DELETE mpTempStrBuf3;

  return applib::frame::RES_FINISH;
}




//--------------------------------------------------------------------------
/**
 * @brief   ボタン押下通知
 */
//--------------------------------------------------------------------------
void ListSampleFrame::OnButtonAction( int button_no )
{
}


//--------------------------------------------------------------------------
/**
 * @brief   キー操作通知
 * @param		dir  キー方向
 * @retval  true = 操作実行
 * @retval  false = 操作失敗
 */
//--------------------------------------------------------------------------
bool ListSampleFrame::OnKeyAction( u32 dir )
{






//  SetSubSeq( MM_SEQ_END );
	return true;

}

GFL_NAMESPACE_END( List )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG

//=============================================================================
/**
 * @file	 TalkWindowSampleFrame.cpp
 * @brief	 継承したテスト
 * @author k.ohno
 * @date	 2014.2.17
 */
//=============================================================================
#if PM_DEBUG

#include "TalkWindowSampleFrame.h"
//#include "TalkWindowSampleDraw.h"
#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_menuparts.h>
#include <niji_conv_header/message/script/msg_test_event_0010.h>

#include <arc_def_index/arc_def.h>
#include <arc_index/msgwin_test.gaix>
#include <arc_index/fieldwindow.gaix>

#include <arc_index/script_message.gaix>

#include "AppLib/include/Frame/AppFrameManager.h"

#include "AppLib/include/Tool/TalkWindow.h"   //

#include <Fade/include/gfl2_FadeManager.h>
#include <GameSys/include/GameData.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( TalkWindowTest )


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
TalkWindowSampleFrame::TalkWindowSampleFrame(void)
{
  mReadSeq=0;
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
TalkWindowSampleFrame::~TalkWindowSampleFrame()
{
}


void TalkWindowSampleFrame::Setup( APP_PARAM* pAppParam )
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
  LOCAL_CLOSE_WAIT2,
};






//------------------------------------------------------------------
/**
 * @brief	  PROCの初期化を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------
applib::frame::Result TalkWindowSampleFrame::InitFunc(void)
{
  mTestSeq=0;

  GFL_ASSERT(mpAppParam);  //Setupしないうちに起動してはいけない  128

  gfl2::fs::AsyncFileManager* pAsyncFileMan = mpAppParam->mpAsyncFileManager;
  int arcid = ARCID_FIELDWINDOW;
  int datid = GARC_fieldwindow_fieldwindow_applyt_COMP;

  switch(mReadSeq){  ///読み込み開始をここに記述      heapとり方修正  2015.05.20
  case LOCAL_OPEN_START:
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq oreq;
      oreq.arcId       = arcid;
      oreq.heapForFile = mpAppHeap->GetFileReadHeap()->GetLowerHandle();  // ファイル情報のメモリ確保に使う。クローズするまでメモリを取られ続ける。
                                                                            // 今回は読み込みが完了したらすぐクローズするので[一時的]であるため後方確保。
                                                                            // （最後までクローズしない場合は最後までメモリを使い続けるので前方確保がいいです。）
      oreq.heapForReq  = mpAppHeap->GetFileReadHeap()->GetLowerHandle();  // オープンが完了するまで[一時的]に使うメモリなので後方確保。
      pAsyncFileMan->AddArcFileOpenReq(oreq);
    }

    mpStrBuf = GFL_NEW(mpAppHeap->GetDeviceHeap()) gfl2::str::StrBuf(200, mpAppHeap->GetDeviceHeap());

    mReadSeq = LOCAL_READ_START;
    //break;
  case LOCAL_READ_START:
    if(  pAsyncFileMan->IsArcFileOpenFinished( arcid )){
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
      mpArcReadBuff         = NULL;
      req.arcId             = arcid;
      req.datId             = datid;
      req.ppBuf             = &mpArcReadBuff;
      req.heapForBuf        = mpAppHeap->GetDeviceHeap();                    // 読み込んだもののメモリ確保に使う。自分で解放するまでメモリを取られ続ける。
      req.heapForReq        = mpAppHeap->GetFileReadHeap()->GetLowerHandle();  // 読み込みが完了するまで[一時的]に使うメモリなので後方確保。
      req.heapForCompressed = mpAppHeap->GetFileReadHeap()->GetLowerHandle();  // 解凍が完了するまで[一時的]に使うメモリなので後方確保。
      req.align             = 128;
      pAsyncFileMan->AddArcFileLoadDataReq(req);
      mReadSeq = LOCAL_READ_WAIT;
    }
    else{
      break;
    }
  case LOCAL_READ_WAIT:
    if(pAsyncFileMan->IsArcFileLoadDataFinished(&mpArcReadBuff) ){

      mpTalkWindow = GFL_NEW(mpAppHeap->GetDeviceHeap()) App::Tool::TalkWindow(mpAppHeap, mpAppRenderingManager);

      mpTalkWindow->SetWindowType(App::Tool::WIN_TYPE_TELOP_WHITE);
      mpTalkWindow->Setup(mpArcReadBuff);
    //  mpTalkWindow->InitMsgData(print::GetMessageArcId_Script(), GARC_script_message_test_event_0010_DAT);
      mpTalkWindow->InitMsgData(print::GetMessageArcId_Debug(), GARC_debug_message_d_menuparts_DAT);
      mpTalkWindow->InitTrainerMsgData();

      mpWordSet = GFL_NEW( mpAppHeap->GetDeviceHeap() ) print::WordSet( 100, print::WordSet::DEFAULT_BUF_LEN , mpAppHeap->GetSystemHeap() );
      mpTalkWindow->SetScriptWordSet(mpWordSet);

//      mpTalkWindow->GetG2DUtil()->CreateMessageData(GARC_debug_message_d_menuparts_DAT, gfl2::str::MsgData::LOAD_FULL ,mpAppHeap,256);
      
      {
        gfl2::fs::AsyncFileManager::ArcFileCloseReq creq;
        creq.arcId       = arcid;
        creq.heapForReq  = mpAppHeap->GetFileReadHeap()->GetLowerHandle();  // クローズが完了するまで[一時的]に使うメモリなので後方確保。
        pAsyncFileMan->AddArcFileCloseReq(creq);
      }
      mReadSeq = LOCAL_CLOSE_WAIT;
    }
    else{
      break;
    }
  case LOCAL_CLOSE_WAIT:
    if(pAsyncFileMan->IsArcFileCloseFinished(arcid) ){

      mpWordSet->RegisterNumber( 0, 123456, 7, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
  /*    
			mpMsgData = GFL_NEW(mpAppHeap->GetDeviceHeap()) gfl2::str::MsgData( print::GetMessageArcId_Debug(), GARC_debug_message_d_menuparts_DAT,
				mpAppHeap->GetDeviceHeap(), gfl2::str::MsgData::LOAD_FULL );



  
      mpMsgData->GetString(debug_menu_parts_launcher_07, *mpStrBuf);
      mpWordSet->RegisterWord(0,*mpStrBuf);
			mpMsgData->GetString(debug_menu_parts_launcher_08, *mpStrBuf);
			mpWordSet->RegisterWord(1,*mpStrBuf);
			mpMsgData->GetString(debug_menu_parts_launcher_09, *mpStrBuf);
			mpWordSet->RegisterWord(2,*mpStrBuf);
			mpMsgData->GetString(debug_menu_parts_launcher_11, *mpStrBuf);
			mpWordSet->RegisterWord(3,*mpStrBuf);
      
      mpTalkWindow->SetMessage((u32)debug_menu_parts_launcher_18);
//      mpTalkWindow->SetMessage((u32)debug_menu_parts_launcher_22);
 //     mpTalkWindow->SetMessage((u32)FieldTest_NewText_002);
     */

   //   mpWordSet->RegisterPokeMonsNameNo(0,MONSNO_MARUMAIN);
 //     mpWordSet->RegisterItemName(1,ITEM_DOKUKESI,1);  //どくけし
 //  mpWordSet->RegisterItemName(1,ITEM_NEMUKEZAMASI,1);  //ねむけざまし
//    mpWordSet->RegisterItemName(1,381,1);  //ピーピーエイド

    //  mpTalkWindow->SetScriptWordSet(  mpTalkWindow->GetG2DUtil()->GetWordSet() );

      Savedata::MyStatus* pMy = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();

      mpStrBuf->SetStr(L"Yvonne");
      
      pMy->SetNameString(mpStrBuf);
      
      mpWordSet->RegisterPlayerName(1,pMy);
      
      mpTalkWindow->SetMessage((u32)debug_menu_parts_launcher_19);


      mpTalkWindow->CallMsgWin();
      mReadSeq = LOCAL_CLOSE_WAIT2;

    }
    break;
  case LOCAL_CLOSE_WAIT2:
    {
      mpTalkWindow->UpdateTree();
    }
		return applib::frame::RES_FINISH;
  }
  return applib::frame::RES_CONTINUE;


}

//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン更新を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
applib::frame::Result TalkWindowSampleFrame::UpdateFunc(void)
{
  mTestSeq++;

  

  if(mTestSeq==50){


    
//    static const gfl2::math::Vector4 scou(0.0f,0.0f,0.0f,0.0f);
//    static const gfl2::math::Vector4 ecou(128.0f,0.0f,0.0f,255.0f);
//    static const gfl2::math::Vector4 scol(255.0f,255.0f,255.0f,0.0f);
//    static const gfl2::math::Vector4 ecol(255.0f,255.0f,255.0f,255.0f);
//		GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_DOUBLE, &scou, &ecou,30);
//		GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_LOWER, &scol, &ecol,40);
   // GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestDefaultOut(gfl2::Fade::DISP_DOUBLE);

  }
  if(mTestSeq==1000){
	//	GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA,30);
//		GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_LOWER,40);
    return applib::frame::RES_FINISH;

  }
  
  
  mpTalkWindow->UpdateTree();

  if(GetSubSeq()==MM_SEQ_YESNO){
    SetSubSeq(MM_SEQ_NONE);
  }

//  if(mpTalkWindow->GetStatus() == App::Tool::TalkWindow::WIN_STATUS_WIN_EXIT){
//    mpTalkWindow->CloseMsgWin();
//    return applib::frame::RES_FINISH;
//  }


  
 // if(GetSubSeq()==MM_SEQ_END){
  //  return applib::frame::RES_FINISH;
//  }

//  if(mTestSeq==180){
 //   mpTalkWindow->CloseMsgWin();
   // return applib::frame::RES_FINISH;
//  }

  return applib::frame::RES_CONTINUE;
}


//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------
void TalkWindowSampleFrame::DrawFunc(gfl2::gfx::CtrDisplayNo no)
{
  //if(no == gfl2::gfx::CtrDisplayNo::LEFT){
    mpTalkWindow->DrawTree(no);
//  }

}

//------------------------------------------------------------------
/**
 * @brief	  PROCの終了を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------
applib::frame::Result TalkWindowSampleFrame::EndFunc(void)
{
  mpTalkWindow->End();
	GFL_DELETE mpStrBuf;
	GFL_DELETE mpMsgData;
  GFL_DELETE mpTalkWindow;
  GFL_DELETE mpWordSet;
  GflHeapFreeMemory( mpArcReadBuff );
	return applib::frame::RES_FINISH;
}




//--------------------------------------------------------------------------
/**
 * @brief   ボタン押下通知
 */
//--------------------------------------------------------------------------
void TalkWindowSampleFrame::OnButtonAction( int button_no )
{
  if(button_no == 1){


    SetSubSeq( MM_SEQ_YESNO );
  }
  else{
    SetSubSeq( MM_SEQ_END );
  }
}


//--------------------------------------------------------------------------
/**
 * @brief   キー操作通知
 * @param		dir  キー方向
 * @retval  true = 操作実行
 * @retval  false = 操作失敗
 */
//--------------------------------------------------------------------------
bool TalkWindowSampleFrame::OnKeyAction( u32 dir )
{

  SetSubSeq( MM_SEQ_END );
	return true;

}


GFL_NAMESPACE_END( TalkWindowTest )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG

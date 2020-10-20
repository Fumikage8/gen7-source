//=============================================================================
/**
 * @file	 FieldMessageSampleFrame.cpp
 * @brief	 継承したテスト
 * @author k.ohno
 * @date	 2014.2.17
 */
//=============================================================================
#if PM_DEBUG

#include "FieldMessageSampleFrame.h"
#include "FieldMessageSampleDraw.h"
#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_menuparts.h>
#include <niji_conv_header/message/script/msg_test_event_0010.h>

#include <arc_def_index/arc_def.h>
#include <arc_index/msgwin_test.gaix>
#include <arc_index/fieldwindow.gaix>

#include "AppLib/include/Frame/AppFrameManager.h"

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldMessageTest )


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
FieldMessageSampleFrame::FieldMessageSampleFrame(void)
{
  mReadSeq=0;
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
FieldMessageSampleFrame::~FieldMessageSampleFrame()
{
}


void FieldMessageSampleFrame::Setup( APP_PARAM* pAppParam )
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
applib::frame::Result FieldMessageSampleFrame::InitFunc(void)
{
  GFL_ASSERT(mpAppParam);  //Setupしないうちに起動してはいけない  128

  gfl2::fs::AsyncFileManager* pAsyncFileMan = mpAppParam->mpAsyncFileManager;
  int arcid = ARCID_FIELDWINDOW;
  int datid = GARC_fieldwindow_fieldwindow_applyt_COMP;

  switch(mReadSeq){  ///読み込み開始をここに記述
  case LOCAL_OPEN_START:
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq oreq;
      oreq.arcId       = arcid;
      oreq.heapForFile = mpAppHeap->GetDeviceHeap()->GetLowerHandle();  // ファイル情報のメモリ確保に使う。クローズするまでメモリを取られ続ける。
                                                                            // 今回は読み込みが完了したらすぐクローズするので[一時的]であるため後方確保。
                                                                            // （最後までクローズしない場合は最後までメモリを使い続けるので前方確保がいいです。）
      oreq.heapForReq  = mpAppHeap->GetDeviceHeap()->GetLowerHandle();  // オープンが完了するまで[一時的]に使うメモリなので後方確保。
      pAsyncFileMan->AddArcFileOpenReq(oreq);
    }
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
      req.heapForReq        = mpAppHeap->GetDeviceHeap()->GetLowerHandle();  // 読み込みが完了するまで[一時的]に使うメモリなので後方確保。
      req.heapForCompressed = mpAppHeap->GetDeviceHeap()->GetLowerHandle();  // 解凍が完了するまで[一時的]に使うメモリなので後方確保。
      req.align             = 128;
      pAsyncFileMan->AddArcFileLoadDataReq(req);
      mReadSeq = LOCAL_READ_WAIT;
    }
    else{
      break;
    }
  case LOCAL_READ_WAIT:
    if(pAsyncFileMan->IsArcFileLoadDataFinished(&mpArcReadBuff) ){
      mpFieldMessageSampleDraw = GFL_NEW(mpAppHeap->GetDeviceHeap()) FieldMessageSampleDraw(mpAppHeap, mpAppRenderingManager);
      mpFieldMessageSampleDraw->Init(mpAppParam,mpArcReadBuff);
      mpFieldMessageSampleDraw->SetTitleViewUIListener(this);
      {
        gfl2::fs::AsyncFileManager::ArcFileCloseReq creq;
        creq.arcId       = arcid;
        creq.heapForReq  = mpAppHeap->GetDeviceHeap()->GetLowerHandle();  // クローズが完了するまで[一時的]に使うメモリなので後方確保。
        pAsyncFileMan->AddArcFileCloseReq(creq);
      }
      mReadSeq = LOCAL_CLOSE_WAIT;
    }
    else{
      break;
    }
  case LOCAL_CLOSE_WAIT:
    if(pAsyncFileMan->IsArcFileCloseFinished(arcid) ){

   //   mpFieldMessageSampleDraw->SetMessage( debug_menu_parts_launcher_06 );
      mpFieldMessageSampleDraw->SetMessage( FieldTest_NewText_001 );

      return applib::frame::RES_FINISH;
    }
    break;
  }
  return applib::frame::RES_CONTINUE;


}

//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン更新を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
applib::frame::Result FieldMessageSampleFrame::UpdateFunc(void)
{

  
  mpFieldMessageSampleDraw->UpdateTree();

  if(GetSubSeq()==MM_SEQ_YESNO){
    SetSubSeq(MM_SEQ_NONE);
  }
  
  if(GetSubSeq()==MM_SEQ_END){
    return applib::frame::RES_FINISH;
  }


  return applib::frame::RES_CONTINUE;
}


//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------
void FieldMessageSampleFrame::DrawFunc(gfl2::gfx::CtrDisplayNo no)
{
  if(no == gfl2::gfx::CtrDisplayNo::LEFT){
    mpFieldMessageSampleDraw->DrawTree(no);
  }

}

//------------------------------------------------------------------
/**
 * @brief	  PROCの終了を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------
applib::frame::Result FieldMessageSampleFrame::EndFunc(void)
{
  if(false == mpFieldMessageSampleDraw->End()){
    return applib::frame::RES_CONTINUE;
  }
  GflHeapFreeMemory( mpArcReadBuff );
  GFL_DELETE mpFieldMessageSampleDraw;
	return applib::frame::RES_FINISH;
}




//--------------------------------------------------------------------------
/**
 * @brief   ボタン押下通知
 */
//--------------------------------------------------------------------------
void FieldMessageSampleFrame::OnButtonAction( int button_no )
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
bool FieldMessageSampleFrame::OnKeyAction( u32 dir )
{

  SetSubSeq( MM_SEQ_END );
	return true;

}


GFL_NAMESPACE_END( FieldMessageTest )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG

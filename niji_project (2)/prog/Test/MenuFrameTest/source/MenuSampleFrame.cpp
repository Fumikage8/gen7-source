//=============================================================================
/**
 * @file	 MenuSampleFrame.cpp
 * @brief	 継承したテスト
 * @author k.ohno
 * @date	 2014.2.17
 */
//=============================================================================
#if PM_DEBUG

#include "MenuSampleFrame.h"
#include "MenuSampleDraw.h"
#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_menuparts.h>

#include <arc_def_index/arc_def.h>
#include <arc_index/parts_test.gaix>

#include "YesNoSampleFrame.h"
#include "AppLib/include/Frame/AppFrameManager.h"

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( MenuFrameTest )


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
MenuSampleFrame::MenuSampleFrame(void)
{
  mFileSeq = 0;
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
MenuSampleFrame::~MenuSampleFrame()
{
}


void MenuSampleFrame::Setup( APP_PARAM* pAppParam )
{
  mpAppHeap = pAppParam->mpAppHeap;
  mpAppRenderingManager = pAppParam->mpAppRenderingManager;
  mpAppParam = pAppParam;
}


//------------------------------------------------------------------
/**
 * @brief	  ファイル読み込みようテーブル初期化
 */
//------------------------------------------------------------------

void MenuSampleFrame::fileReadStructSetup(app::util::FileAccessor::RequestData* pfrs, int arcid, int dataid, app::util::Heap* pAppHeap, void** pReadBuffAddr)
{
  pfrs->openr.arcId       = arcid;

  // ファイル情報のメモリ確保に使う。クローズするまでメモリを取られ続ける。
  // 今回は読み込みが完了したらすぐクローズするので[一時的]であるため後方確保。
  // （最後までクローズしない場合は最後までメモリを使い続けるので前方確保がいいです。）
  pfrs->openr.heapForFile = pAppHeap->GetDeviceHeap()->GetLowerHandle();

  // オープンが完了するまで[一時的]に使うメモリなので後方確保。
  pfrs->openr.heapForReq  = pAppHeap->GetDeviceHeap()->GetLowerHandle();

  pfrs->loadr.arcId             = arcid;
  pfrs->loadr.datId             = dataid;
  pfrs->loadr.ppBuf             = pReadBuffAddr;
  pfrs->loadr.heapForBuf        = pAppHeap->GetDeviceHeap();                    // 読み込んだもののメモリ確保に使う。自分で解放するまでメモリを取られ続ける。
  pfrs->loadr.heapForReq        = pAppHeap->GetDeviceHeap()->GetLowerHandle();  // 読み込みが完了するまで[一時的]に使うメモリなので後方確保。
  pfrs->loadr.heapForCompressed = pAppHeap->GetDeviceHeap()->GetLowerHandle();  // 解凍が完了するまで[一時的]に使うメモリなので後方確保。
  pfrs->loadr.align             = 128;

  pfrs->closer.arcId       = arcid;
  pfrs->closer.heapForReq  = pAppHeap->GetDeviceHeap()->GetLowerHandle();  // クローズが完了するまで[一時的]に使うメモリなので後方確保。

}

enum{
  LOCAL_OPEN_START,
  LOCAL_READ_START,
  LOCAL_READ_WAIT,
  LOCAL_CLOSE_WAIT,
  LOCAL_CLOSE_END,
};


bool MenuSampleFrame::fileReadSequece(void)
{

  gfl2::fs::AsyncFileManager* pAsyncFileMan = mpAppParam->mpAsyncFileManager;

  app::util::FileAccessor::RequestData frs;

  fileReadStructSetup(&frs, ARCID_LAYOUTPARTS_TEST, GARC_parts_test_parts_test_applyt_COMP,mpAppHeap, &mpArcReadBuff);


  switch(mFileSeq){  ///読み込み開始をここに記述
  case LOCAL_OPEN_START:
    {
      mpArcReadBuff         = NULL;
      pAsyncFileMan->AddArcFileOpenReq(frs.openr);
    }
    mFileSeq = LOCAL_READ_START;
    //break;
  case LOCAL_READ_START:
    if(  pAsyncFileMan->IsArcFileOpenFinished( frs.openr.arcId )){
      pAsyncFileMan->AddArcFileLoadDataReq(frs.loadr);
      mFileSeq = LOCAL_READ_WAIT;
    }
    else{
      break;
    }
  case LOCAL_READ_WAIT:
    if(pAsyncFileMan->IsArcFileLoadDataFinished( frs.loadr.ppBuf ) ){

      pAsyncFileMan->AddArcFileCloseReq(frs.closer);
      mFileSeq = LOCAL_CLOSE_WAIT;
    }
    else{
      break;
    }
  case LOCAL_CLOSE_WAIT:
    if(pAsyncFileMan->IsArcFileCloseFinished( frs.openr.arcId ) ){
      mFileSeq = LOCAL_CLOSE_END;
      return true;
    }
    break;
  }
  return false;
}




//------------------------------------------------------------------
/**
 * @brief	  PROCの初期化を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------
applib::frame::Result MenuSampleFrame::InitFunc(void)
{
  GFL_ASSERT(mpAppParam);  //Setupしないうちに起動してはいけない  128


  if(fileReadSequece()){

    mpMenuSampleDraw = GFL_NEW(mpAppHeap->GetSystemHeap()) MenuSampleDraw(mpAppHeap, mpAppRenderingManager);
    mpMenuSampleDraw->Init(mpAppParam,mpArcReadBuff);
    mpMenuSampleDraw->SetTitleViewUIListener(this);

    mpMenuSampleDraw->ExpandPokemon(0,101);
    mpMenuSampleDraw->ExpandItem(1,303);  //どくけし
//    mpMenuSampleDraw->ExpandItem(1,326);  //ねむけざまし
//    mpMenuSampleDraw->ExpandItem(1,381);  //ピーピーエイド

    
    mpMenuSampleDraw->SetMessage( debug_menu_parts_launcher_17 );

    mpAppParam->mpUIView = mpMenuSampleDraw;
    
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
applib::frame::Result MenuSampleFrame::UpdateFunc(void)
{


  mpMenuSampleDraw->UpdateTree();

  if(GetSubSeq()==MM_SEQ_YESNO){
    //    YesNoSampleFrame* pSampleFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, YesNoSampleFrame>( GetMyManager() );
    YesNoSampleFrame* pSampleFrame = MENU_FRAME_CALL_MODELESS<applib::frame::Manager, YesNoSampleFrame>( GetMyManager() );

    pSampleFrame->Setup( mpAppParam );
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
void MenuSampleFrame::DrawFunc(gfl2::gfx::CtrDisplayNo no)
{
  if(no == gfl2::gfx::CtrDisplayNo::LEFT){
    mpMenuSampleDraw->DrawTree(no);
  }

}

//------------------------------------------------------------------
/**
 * @brief	  PROCの終了を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------
applib::frame::Result MenuSampleFrame::EndFunc(void)
{
  if(false == mpMenuSampleDraw->End()){
    return applib::frame::RES_CONTINUE;
  }
  GflHeapFreeMemory( mpArcReadBuff );
  mpAppParam->mpUIView = NULL;
  GFL_DELETE mpMenuSampleDraw;
  return applib::frame::RES_FINISH;
}




//--------------------------------------------------------------------------
/**
 * @brief   ボタン押下通知
 */
//--------------------------------------------------------------------------
void MenuSampleFrame::OnButtonAction( int button_no )
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
bool MenuSampleFrame::OnKeyAction( u32 dir )
{

  SetSubSeq( MM_SEQ_END );
  return true;

}


GFL_NAMESPACE_END( MenuFrameTest )
GFL_NAMESPACE_END( Test )


#endif //PM_DEBUG

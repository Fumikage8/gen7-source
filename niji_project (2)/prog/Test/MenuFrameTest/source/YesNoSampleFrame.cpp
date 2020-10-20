//=============================================================================
/**
 * @file	 MenuSampleFrame.cpp
 * @brief	 継承したテスト
 * @author k.ohno
 * @date	 2014.2.17
 */
//=============================================================================
#if PM_DEBUG

#include "YesNoSampleFrame.h"
#include "YesNoSampleDraw.h"
#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_menuparts.h>

#include <arc_def_index/arc_def.h>
#include <arc_index/yesno_test.gaix>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( MenuFrameTest )


enum{
  MM_SEQ_NONE,
  MM_SEQ_END,
};

#define ADD_SUB_VIEW_TEST 1



//------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------
YesNoSampleFrame::YesNoSampleFrame(void)
{
  mReadSeq=0;
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
YesNoSampleFrame::~YesNoSampleFrame()
{
}


void YesNoSampleFrame::Setup( APP_PARAM* pAppParam )
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
applib::frame::Result YesNoSampleFrame::InitFunc(void)
{
  GFL_ASSERT(mpAppParam);  //Setupしないうちに起動してはいけない  128

  gfl2::fs::AsyncFileManager* pAsyncFileMan = mpAppParam->mpAsyncFileManager;
  int arcid = ARCID_LAYOUTYESNO_TEST;

  switch(mReadSeq){  ///読み込み開始をここに記述
  case LOCAL_OPEN_START:
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq oreq;
      oreq.arcId       = arcid;
      oreq.heapForFile = mpAppHeap->GetDeviceHeap();
      oreq.heapForReq  = mpAppHeap->GetDeviceHeap();
      pAsyncFileMan->AddArcFileOpenReq(oreq);
    }
    mReadSeq = LOCAL_READ_START;
    //break;
  case LOCAL_READ_START:
    if(  pAsyncFileMan->IsArcFileOpenFinished( arcid )){
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
      mpArcReadBuff=NULL;
      req.arcId       = arcid;
      req.datId = GARC_yesno_test_yesno_test_APPLYT;
			req.ppBuf = &mpArcReadBuff;
      req.heapForBuf = mpAppHeap->GetDeviceHeap();
      req.heapForReq  = mpAppHeap->GetDeviceHeap();
      req.align = 128;
      pAsyncFileMan->AddArcFileLoadDataReq(req);
      mReadSeq = LOCAL_READ_WAIT;
    }
    else{
      break;
    }
  case LOCAL_READ_WAIT:
    if(pAsyncFileMan->IsArcFileLoadDataFinished(&mpArcReadBuff) ){
      mpYesNoSampleDraw = GFL_NEW(mpAppHeap->GetDeviceHeap()) YesNoSampleDraw(mpAppHeap, mpAppRenderingManager);
      mpYesNoSampleDraw->Init(mpAppParam,mpArcReadBuff);
      mpYesNoSampleDraw->SetTitleViewUIListener(this);

#if ADD_SUB_VIEW_TEST
      mpAppParam->mpUIView->AddSubView(mpYesNoSampleDraw);  //addsubview使用
 //     mpYesNoSampleDraw->SetModal();  //モーダルにする
#endif
      
      {
        gfl2::fs::AsyncFileManager::ArcFileCloseReq creq;
        creq.arcId       = ARCID_LAYOUTYESNO_TEST;
        creq.heapForReq  = mpAppHeap->GetDeviceHeap();
        pAsyncFileMan->AddArcFileCloseReq(creq);
      }
      mReadSeq = LOCAL_CLOSE_WAIT;
    }
    else{
      break;
    }
  case LOCAL_CLOSE_WAIT:
    if(pAsyncFileMan->IsArcFileCloseFinished(ARCID_LAYOUTPARTS_TEST) ){


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
applib::frame::Result YesNoSampleFrame::UpdateFunc(void)
{

#if ADD_SUB_VIEW_TEST
#else
  mpYesNoSampleDraw->UpdateTree();
#endif
  
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
void YesNoSampleFrame::DrawFunc(gfl2::gfx::CtrDisplayNo no)
{

#if ADD_SUB_VIEW_TEST
#else
  if(no == gfl2::gfx::CtrDisplayNo::LEFT){
    mpYesNoSampleDraw->DrawTree();
  }
#endif



}

//------------------------------------------------------------------
/**
 * @brief	  PROCの終了を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------
applib::frame::Result YesNoSampleFrame::EndFunc(void)
{
#if ADD_SUB_VIEW_TEST
  mpYesNoSampleDraw->RemoveFromSuperView();
#endif
  
  if(false == mpYesNoSampleDraw->End()){
    return applib::frame::RES_CONTINUE;
  }
  GflHeapFreeMemory( mpArcReadBuff );
  GFL_DELETE mpYesNoSampleDraw;
	return applib::frame::RES_FINISH;
}




//--------------------------------------------------------------------------
/**
 * @brief   ボタン押下通知
 */
//--------------------------------------------------------------------------
void YesNoSampleFrame::OnButtonAction( int button_no )
{

  SetSubSeq( MM_SEQ_END );

}


//--------------------------------------------------------------------------
/**
 * @brief   キー操作通知
 * @param		dir  キー方向
 * @retval  true = 操作実行
 * @retval  false = 操作失敗
 */
//--------------------------------------------------------------------------
bool YesNoSampleFrame::OnKeyAction( u32 dir )
{

  SetSubSeq( MM_SEQ_END );
	return true;

}


GFL_NAMESPACE_END( MenuFrameTest )
GFL_NAMESPACE_END( Test )


#endif //PM_DEBUG

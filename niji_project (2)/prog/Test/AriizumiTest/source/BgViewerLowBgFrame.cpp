//=============================================================================
/**
 * @file	 BgViewerLowBgFrame.cpp
 * @brief	 下画面の操作や制御を管理
 * @author k.ohno
 * @date	 2015.7.14
 */
//=============================================================================

#if PM_DEBUG

#include "BgViewerLowBgFrame.h"
#include "BgViewerLowBgDraw.h"
#include <arc_index/debug_message.gaix>

#include <arc_index/battlebgview.gaix>


#include <arc_def_index/arc_def.h>

#include "AppLib/include/Frame/AppFrameManager.h"

#include <Fade/include/gfl2_FadeManager.h>


#include "BgViewerLowBgDrawListener.h"
#include "Test/AriizumiTest/include/BgViewerActionListener.h"






GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Ariizumi )


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
BgViewerLowBgFrame::BgViewerLowBgFrame(void)
{
  mReadSeq=0;
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
BgViewerLowBgFrame::~BgViewerLowBgFrame()
{
}


void BgViewerLowBgFrame::Setup( APP_PARAM* pAppParam )
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
  LOCAL_POKEICON_CREATE,
};






//------------------------------------------------------------------
/**
 * @brief	  PROCの初期化を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------
applib::frame::Result BgViewerLowBgFrame::InitFunc(void)
{

  GFL_ASSERT(mpAppParam);  //Setupしないうちに起動してはいけない  128

  gfl2::fs::AsyncFileManager* pAsyncFileMan = mpAppParam->mpAsyncFileManager;
  int arcid = ARCID_DEBUG_BTLBGVIEWER;
  int datid = GARC_battlebgview_battlebgview_applyt_COMP;

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
    if(!pAsyncFileMan->IsArcFileCloseFinished(arcid) ){
      break;
    }
    mReadSeq = LOCAL_POKEICON_CREATE;
    break;
  case LOCAL_POKEICON_CREATE:
    {
      mpBoxBgLowDraw = GFL_NEW(mpAppHeap->GetSystemHeap()) BgViewerLowBgDraw(mpAppHeap, mpAppRenderingManager);
      mpBoxBgLowDraw->Init( mpAppParam, mpArcReadBuff);
      mpBoxBgLowDraw->SetUIListener(this);
      return applib::frame::RES_FINISH;
    }
  }
  return applib::frame::RES_CONTINUE;


}

//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン更新を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
applib::frame::Result BgViewerLowBgFrame::UpdateFunc(void)
{
 
  if(GetSubSeq()==MM_SEQ_END){
    return applib::frame::RES_FINISH;
  }
  mpBoxBgLowDraw->Update();
  return applib::frame::RES_CONTINUE;
}


//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------
void BgViewerLowBgFrame::DrawFunc(gfl2::gfx::CtrDisplayNo no)
{
  mpBoxBgLowDraw->Draw(no);

}

//------------------------------------------------------------------
/**
 * @brief	  PROCの終了を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------
applib::frame::Result BgViewerLowBgFrame::EndFunc(void)
{
  mpBoxBgLowDraw->End();
  GFL_SAFE_DELETE(mpBoxBgLowDraw);
  GflHeapFreeMemory( mpArcReadBuff );
  return applib::frame::RES_FINISH;
}




//--------------------------------------------------------------------------
/**
 * @brief   ボタン押下通知
 */
//--------------------------------------------------------------------------
void BgViewerLowBgFrame::OnButtonAction( int button_no )
{
  if(BgViewerLowBgDrawListener::BTN_ID_CANCEL==button_no){
    SetSubSeq( MM_SEQ_END );
  }
  if(BgViewerLowBgDrawListener::BTN_ID_A==button_no){
    mpAppParam->pBgViewerActionListener->OnActionStart(BgViewerActionListener::ACTION_READ_FIELD);
  }

  if(BgViewerLowBgDrawListener::BTN_ID_ANIM==button_no){
    int i = mpAppParam->pBgViewerActionListener->OnActionStart(BgViewerActionListener::ACTION_ANIM_START);
    mpBoxBgLowDraw->AnimStartOnOff(i);
  }

  if(BgViewerLowBgDrawListener::BTN_ID_TEN_R == button_no){
    mpAppParam->pBgViewerActionListener->OnActionStart(BgViewerActionListener::ACTION_SKY_ON);
    mpBoxBgLowDraw->SkyboxOnOff(true);
  }
  if(BgViewerLowBgDrawListener::BTN_ID_TEN_L == button_no){
    mpAppParam->pBgViewerActionListener->OnActionStart(BgViewerActionListener::ACTION_SKY_OFF);
    mpBoxBgLowDraw->SkyboxOnOff(false);
  }
  if(BgViewerLowBgDrawListener::BTN_ID_ANIM_R == button_no){
    int i = mpAppParam->pBgViewerActionListener->OnActionStart(BgViewerActionListener::ACTION_ANIM_ADD);
    mpBoxBgLowDraw->AnimChange(i);
  }
  if(BgViewerLowBgDrawListener::BTN_ID_ANIM_L == button_no){
    int i = mpAppParam->pBgViewerActionListener->OnActionStart(BgViewerActionListener::ACTION_ANIM_SUB);
    mpBoxBgLowDraw->AnimChange(i);
  }

  if(BgViewerLowBgDrawListener::BTN_ID_GROUND_R == button_no){
    int i = mpAppParam->pBgViewerActionListener->OnActionStart(BgViewerActionListener::ACTION_GROUND_ADD);
    mpBoxBgLowDraw->BackGroundChange(i,0);
  }
  if(BgViewerLowBgDrawListener::BTN_ID_GROUND_L == button_no){
    int i = mpAppParam->pBgViewerActionListener->OnActionStart(BgViewerActionListener::ACTION_GROUND_SUB);
    mpBoxBgLowDraw->BackGroundChange(i,0);
  }
  if(BgViewerLowBgDrawListener::BTN_ID_NEAR_R == button_no){
    int i = mpAppParam->pBgViewerActionListener->OnActionStart(BgViewerActionListener::ACTION_NEAR_ADD);
    mpBoxBgLowDraw->BackGroundChange(i,1);
  }
  if(BgViewerLowBgDrawListener::BTN_ID_NEAR_L == button_no){
    int i = mpAppParam->pBgViewerActionListener->OnActionStart(BgViewerActionListener::ACTION_NEAR_SUB);
    mpBoxBgLowDraw->BackGroundChange(i,1);
  }
  if(BgViewerLowBgDrawListener::BTN_ID_FAR_R == button_no){
    int i = mpAppParam->pBgViewerActionListener->OnActionStart(BgViewerActionListener::ACTION_FAR_ADD);
    mpBoxBgLowDraw->BackGroundChange(i,2);
  }
  if(BgViewerLowBgDrawListener::BTN_ID_FAR_L == button_no){
    int i = mpAppParam->pBgViewerActionListener->OnActionStart(BgViewerActionListener::ACTION_FAR_SUB);
    mpBoxBgLowDraw->BackGroundChange(i,2);
  }

  if(BgViewerLowBgDrawListener::BTN_ID_ANIM_REQ == button_no){
    mpAppParam->pBgViewerActionListener->OnActionStart(BgViewerActionListener::ACTION_ANIMREQ);

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
bool BgViewerLowBgFrame::OnKeyAction( u32 dir )
{
  if(BgViewerLowBgDrawListener::BTN_ID_CANCEL==dir){
    SetSubSeq( MM_SEQ_END );
  }
	return true;

}








GFL_NAMESPACE_END( Ariizumi )
GFL_NAMESPACE_END( Test )




#endif //PM_DEBUG

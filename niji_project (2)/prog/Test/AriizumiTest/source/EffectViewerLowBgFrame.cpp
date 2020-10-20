//======================================================================
/**
 * @file EffectViewerLowBgFrame.cpp
 * @date 2015/12/01 18:48:18
 * @author kijima_satoshi
 * @brief エフェクトビューワー用Frameクラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include <arc_def_index/arc_def.h>
#include <arc_index/effect_viewer.gaix>

#include "EffectViewerLowBgFrame.h"
#include "EffectViewerLowBgDraw.h"

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Ariizumi )


// サブシーケンス用
  enum
{
  SUB_SEQ_CONTINUE,
  SUB_SEQ_END,
};

// 初期化シーケンス用
enum{
  LOCAL_OPEN_START,    // レイアウトファイルのロード
  LOCAL_READ_START,
  LOCAL_READ_WAIT,
  LOCAL_CLOSE_WAIT,
  LOCAL_LOWBG_CREATE   // 下画面描画用クラスインスタンスの作成
};

// -------------------------
// Constructor & Destructor
// -------------------------
EffectViewerLowBgFrame::EffectViewerLowBgFrame()
{
  mReadSeq = LOCAL_OPEN_START;
  SetSubSeq( SUB_SEQ_CONTINUE );
}

EffectViewerLowBgFrame::~EffectViewerLowBgFrame()
{
}

//-------------------------------------------------------------------------
/**
 * @brief 受け渡し用構造体のセット
 *
 *InitFunc()前に呼ばれる必要がある
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgFrame::Setup( EFFECT_APP_PARAM* pAppParam )
{
  mpAppParam = pAppParam;
}

//------------------------------------------------------------------
/**
 * @brief  PROCの初期化を行う関数
 * @return Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
applib::frame::Result EffectViewerLowBgFrame::InitFunc(void)
{

  //Setupしないうちに起動してはいけない
  GFL_ASSERT(mpAppParam);
  
  app::util::Heap *mpAppHeap = mpAppParam->mpAppHeap;
  app::util::AppRenderingManager *mpAppRenderingManager = mpAppParam->mpAppRenderingManager;


  // レイアウトファイルのロード
  gfl2::fs::AsyncFileManager* pAsyncFileMan = mpAppParam->mpAsyncFileManager;
  //  int arcid = ARCID_DEBUG_BTLBGVIEWER;
  //  int datid = GARC_battlebgview_battlebgview_applyt_COMP;
  int arcid = ARCID_DEBUG_BTLEFFVIEWER;
  int datid = GARC_effect_viewer_eff_view_APPLYT;

  switch(mReadSeq)
    {

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
      if(  pAsyncFileMan->IsArcFileOpenFinished( arcid ))
        {
          gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
          mpArcReadBuff         = NULL;
          req.arcId             = arcid;
          req.datId             = datid;
          req.ppBuf             = &mpArcReadBuff;
          req.heapForBuf        = mpAppHeap->GetDeviceHeap();                    // 読み込んだもののメモリ確保に使う。自分で解放するまでメモリを取られ続ける。
          req.heapForReq        = mpAppHeap->GetFileReadHeap()->GetLowerHandle();  // 読み込みが完了するまで[一時的]に使うメモリなので後方確保。
          // 非圧縮ファイルの場合はいらない     
          //req.heapForCompressed = mpAppHeap->GetFileReadHeap()->GetLowerHandle();  // 解凍が完了するまで[一時的]に使うメモリなので後方確保。
          req.align             = 128;
          pAsyncFileMan->AddArcFileLoadDataReq(req);
          mReadSeq = LOCAL_READ_WAIT;
        }
      else
        {
          break;
        }
      
    case LOCAL_READ_WAIT:
      if(pAsyncFileMan->IsArcFileLoadDataFinished(&mpArcReadBuff) )
        {
          {
            gfl2::fs::AsyncFileManager::ArcFileCloseReq creq;
            creq.arcId       = arcid;
            creq.heapForReq  = mpAppHeap->GetFileReadHeap()->GetLowerHandle();  // クローズが完了するまで[一時的]に使うメモリなので後方確保。
            pAsyncFileMan->AddArcFileCloseReq(creq);
          }
          mReadSeq = LOCAL_CLOSE_WAIT;
        }
      else
        {
          break;
        }

    case LOCAL_CLOSE_WAIT:
      if(!pAsyncFileMan->IsArcFileCloseFinished(arcid) )
        {
          break;
        }
      mReadSeq = LOCAL_LOWBG_CREATE;
      break;

    case LOCAL_LOWBG_CREATE:
      {
        // 下画面描画用クラスインスタンスの作成
        mpLowBgDraw = GFL_NEW(mpAppHeap->GetSystemHeap()) EffectViewerLowBgDraw(mpAppHeap, mpAppRenderingManager);
        mpLowBgDraw->Init( mpAppParam, mpArcReadBuff );
        mpLowBgDraw->SetUIListener( this );     // UIリスナの設定

        return applib::frame::RES_FINISH;
      }
    }
    
  return applib::frame::RES_CONTINUE;

}

//------------------------------------------------------------------
/**
 * @brief  PROCのメイン更新を行う関数
 * @return Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
applib::frame::Result EffectViewerLowBgFrame::UpdateFunc(void)
{

  if( GetSubSeq() == SUB_SEQ_END )
    {
      return applib::frame::RES_FINISH;
    }


  {
    const gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
    gfl2::ui::Button *btn = uiMng->GetButton(0);

    if (!btn->IsHold(gfl2::ui::BUTTON_L))
    {
  mpLowBgDraw->Update();     // 下画面描画オブジェクトのUpdate
    }
  }


  return applib::frame::RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief  PROCのメイン描画を行う関数
 *         純粋仮想ではありません。
 */
void EffectViewerLowBgFrame::DrawFunc(gfl2::gfx::CtrDisplayNo no)
{
  {
    const gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
    gfl2::ui::Button *btn = uiMng->GetButton(0);

    if (!btn->IsHold(gfl2::ui::BUTTON_L))
    {
  mpLowBgDraw->Draw(no);     // 下画面描画オブジェクトのDraw
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief  PROCの終了を行う関数
 * @return Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
applib::frame::Result EffectViewerLowBgFrame::EndFunc(void)
{
  mpLowBgDraw->End();       // 下画面描画オブジェクトの後処理

  GFL_SAFE_DELETE( mpLowBgDraw );
  GflHeapFreeMemory( mpArcReadBuff );

  return applib::frame::RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * @brief  ボタン押下通知
 */
void EffectViewerLowBgFrame::OnButtonAction( int button_no )
{
  //  if(EffectViewerLowBgListener::BTN_ID_CANCEL == button_no)
  //    {
  //      SetSubSeq( SUB_SEQ_END );
  //    }
}

//--------------------------------------------------------------------------
/**
 * @brief   キー操作通知
 * @param   dir  キー方向
 * @retval  true = 操作実行
 * @retval  false = 操作失敗
 */
bool EffectViewerLowBgFrame::OnKeyAction( u32 dir )
{
  //  if(EffectViewerLowBgListener::BTN_ID_CANCEL == dir)
  //    {
  //      SetSubSeq( SUB_SEQ_END );
  //    }

  return true;
}


GFL_NAMESPACE_END( Ariizumi )
GFL_NAMESPACE_END( Test )

#endif //PM_DEBUG

//==============================================================================
/**
 @file    BattleVideoPlayerLocalProc.h
 @date    2015.09.24
 @note
          バトルビデオ再生のローカルデータプロセス
 */
//==============================================================================
#include "NetApp/BattleVideoPlayer/include/BattleVideoPlayerLocalProc.h"

//  gflib2のインクルード
#include <Fade/include/gfl2_FadeManager.h>

//  nijiのインクルード
#include "GameSys/include/GameData.h"

//  メッセージのインクルード
#include "niji_conv_header/message/msg_battlevideo_player.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)

  LocalProc::LocalProc( PROC_APP_PARAM* pAppParam ) : BaseProc( pAppParam )
  {
  }

  LocalProc::~LocalProc()
  {
  }

  gfl2::proc::Result LocalProc::UpdateFunc( gfl2::proc::Manager* pManager )
  {
    int seq = GameProc::GetSubSeq();

    gfl2::proc::Result  procResult  = gfl2::proc::RES_CONTINUE;

    //  基本クラスでシーケンスが変わっていることがあるのであらためて再取得
    seq = GameProc::GetSubSeq();

    if( seq == UPDATE_SEQ_ROOT_NON_DATA_END )
    {
      //  ビデオデータがないので終了
      // @fix NMCat[355] アプリキャンセル終了シーケンスへ
      seq = _StartSeq( UPDATA_SEQ_ROOT_APP_CANCEL );
    }
    else
    if( seq == UPDATA_SEQ_ROOT_FADE_IN_END )
    {
    }
    else
    if( seq == UPDATE_SEQ_ROOT_SD_CARD_CHK_END )
    {
      //  SDカードチェックをしたがビデオデータがないのでデータがないフローへ
      if( m_pAppParam->pVideoDataManager->GetListDataNum() <= 0 )
      {
        seq = _StartSeq( UPDATE_SEQ_ROOT_NON_DATA_START );
      }
      else
      {
        //  SDカード確認終った!
        seq = _StartSeq( UPDATE_SEQ_ROOT_DATA_LIST );
      }
    }

    //  バトルビデオリスト
    if( seq == UPDATE_SEQ_ROOT_DATA_LIST )
    {
      if( m_pFrameManager->IsMainHandle() == false )
      {
        if( m_dataListFrameParam.out.result == DataListFrame::RESULT_CANCEL )
        {
          //  データを選択せずにキャンセルしたのでアプリ終了
          seq = _StartSeq( UPDATA_SEQ_ROOT_APP_CANCEL );
        }
        else
        if( m_dataListFrameParam.out.result == DataListFrame::RESULT_NON_DATA )
        {
          //  ビデオデータがもうない
          seq = _StartSeq( UPDATE_SEQ_ROOT_NON_DATA_START );
        }
        else
        if( m_dataListFrameParam.out.result == DataListFrame::RESULT_DATA_SELECT )
        {
          //  データ選択したので操作に移る
          seq = _StartDataCtrlSeq( m_dataListFrameParam.out.selectDataIdx, DATA_CTRL_FRAME_MODE_LOCAL );
        }
        else
        if( m_dataListFrameParam.out.result == DataListFrame::RESULT_DATA_FATAL )
        {
          //  失敗設定してアプリ終了
          seq = _StartSeq( UPDATE_SEQ_ROOT_APP_FATAL );          
        }
      }
    }
    else
    if( seq == UPDATE_SEQ_ROOT_DATA_CTRL )
    {
      if( m_pFrameManager->IsMainHandle() == false )
      {
        //  キャンセルをした
        DataCtrlBaseFrame::Result result  = m_dataCtrlFrameParam.out.result;
        if( result == DataCtrlBaseFrame::RESULT_CANCEL )
        {
          seq = _StartSeq( UPDATE_SEQ_ROOT_DATA_LIST );
        }
        else
        if( result == DataCtrlBaseFrame::RESULT_FATAL )
        {
          //  失敗設定してアプリ終了
          seq = _StartSeq( UPDATE_SEQ_ROOT_APP_FATAL );
        }
        else
        //  ビデオ再生選択した
        if( result == DataCtrlBaseFrame::RESULT_VIDEO_PLAY)
        {
          //  再生処理
          seq = _StartSeq( UPDATA_SEQ_ROOT_BTL_PLAY );
        }
        //  模擬戦を選択した
        else
        if( result == DataCtrlBaseFrame::RESULT_BATTLE_AI )
        {
          //  模擬戦処理
          m_pAppParam->out.dualTargetIdx  = m_dataCtrlFrameParam.out.dualTargetIdx;
          seq = _StartSeq( UPDATE_SEQ_ROOT_BTL_AI );
        }
        //  削除した
        else
        if( result == DataCtrlBaseFrame::RESULT_VIDEO_DEL )
        {
          //  データがないか
          if( m_pVideoDataManager->GetListDataNum() <= 0 )
          {
            //  ビデオデータがない
            seq = _StartSeq( UPDATE_SEQ_ROOT_NON_DATA_START );
          }
          else
          {
            //  データがある
            seq = _StartSeq( UPDATE_SEQ_ROOT_DATA_LIST );
          }
        }
      }
    }

    GameProc::SetSubSeq( seq );

    procResult  = BaseProc::UpdateFunc( pManager );

    return procResult;
  }

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetApp)

//==============================================================================
/**
 @file    BattleVideoPlayerNetProc.h
 @date    2015.11.16
 @note
          バトルビデオ再生の通信データプロセス
 */
//==============================================================================
#include "NetApp/BattleVideoPlayer/include/BattleVideoPlayerNetProc.h"

//  gflib2のインクルード
#include <Fade/include/gfl2_FadeManager.h>

//  nijiのインクルード
#include "GameSys/include/GameData.h"
#include "App/NumberInput/include/NumberInputFrame.h"
#include "NetStatic/NetAppLib/include/Util/NetAppConvertUtility.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

//  メッセージのインクルード
#include "niji_conv_header/message/msg_battlevideo_player.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)

  NetProc::NetProc( PROC_APP_PARAM* pAppParam ) : BaseProc( pAppParam )
  {
    _Clear();
  }

  NetProc::~NetProc()
  {
  }

  gfl2::proc::Result NetProc::InitFunc( gfl2::proc::Manager* pManager )
  {
    //  基本クラスの初期化が先
    gfl2::proc::Result  resultProc  = BaseProc::InitFunc( pManager );

    if( resultProc == gfl2::proc::RES_FINISH )
    {
      //  専用初期化をする
      //  再生を完走した場合は完走シーケンスへ遷移
      if( m_pAppParam->in.bPlayCompletion == true )
      {
        //  バトル再生完走通知(真っ黒画面中に送信しておく)
        enum
        {
          BATTLE_VIDEO_PLAYER_NET_PROC_INIT_FUNC_START_SEND_COMP  = 0,
          BATTLE_VIDEO_PLAYER_NET_PROC_INIT_FUNC_WAIT_SEND_COMP,
          BATTLE_VIDEO_PLAYER_NET_PROC_INIT_FUNC_END_SEND_COMP,
        };

        switch( m_subSeq )
        {
          case  BATTLE_VIDEO_PLAYER_NET_PROC_INIT_FUNC_START_SEND_COMP:
          {
            if( m_pAppParam->in.procRunType != PROC_RUN_TYPE_RETURN )
            {
              //  復帰処理のみ限定処理なのでそれ以外だとアウト
              GFL_ASSERT( 0 );
            }
            else
            {
              resultProc  = gfl2::proc::RES_CONTINUE;
              m_pVideoDataManager->StartSendPlayComplete( m_pAppParam->procRunTypeReturnData.pActiveVideoData );

              ++m_subSeq;
            }

            //break;
          }

          case BATTLE_VIDEO_PLAYER_NET_PROC_INIT_FUNC_WAIT_SEND_COMP:
          {
            resultProc  = gfl2::proc::RES_CONTINUE;

            NetAppLib::BattleVideoPlayer::VideoDataManager::SendPlayComplateResult  result  = m_pVideoDataManager->UpdateSendPlayComplate();
            if( result == NetAppLib::BattleVideoPlayer::VideoDataManager::SEND_PLAY_COMPLATE_RESULT_SUCCESS )
            {
              //  最初の処理に戻る
              m_pAppParam->in.bPlayCompletion = false;
              ++m_subSeq;
            }
            else
            if( result == NetAppLib::BattleVideoPlayer::VideoDataManager::SEND_PLAY_COMPLATE_RESULT_ERROR )
            {
                //  最初の処理に戻る
              m_pAppParam->in.bPlayCompletion = false;
              ++m_subSeq;
            }
            else
            {
              break;
            }
          }

          case BATTLE_VIDEO_PLAYER_NET_PROC_INIT_FUNC_END_SEND_COMP:
          {
            resultProc  = gfl2::proc::RES_FINISH;
            break;
          }
        }
      }
    }

    return  resultProc;
  }

  gfl2::proc::Result NetProc::UpdateFunc( gfl2::proc::Manager* pManager )
  {
    int seq = GameProc::GetSubSeq();

    gfl2::proc::Result  procResult  = gfl2::proc::RES_CONTINUE;

    //  基本クラスでシーケンスが変わっていることがあるのであらためて再取得
    seq = GameProc::GetSubSeq();

    if( seq == UPDATE_SEQ_ROOT_NON_DATA_END )
    {
      //  ビデオデータがないので先頭シーケンスへ
      seq = _StartSeq( UPDATE_SEQ_TOP );
    }
    else
    if( seq == UPDATA_SEQ_ROOT_FADE_IN_END )
    {
    }
    else
    if( seq == UPDATE_SEQ_ROOT_SD_CARD_CHK_END )
    {
      //  SDカード確認終った!
      seq = _StartSeq( UPDATE_SEQ_TOP );
    }
    else
    //  トップ選択
    if( seq == UPDATE_SEQ_TOP )
    {
      if( m_pFrameManager->IsMainHandle() == false )
      {
        if( m_netTopSelectFrameParam.out.result == NetTopSelectFrame::RESULT_SELECT_VIDEO_PUBLIC )
        {
          if( m_pAppParam->pVideoDataManager->GetListDataNum() <= 0 )
          {
            //  データがないのでメッセージを出す
            seq = _StartSeq( UPDATE_SEQ_ROOT_NON_DATA_START );
          }
          else
          {
            //  ビデオ公開へ
            seq = _StartSeq( UPDATE_SEQ_ROOT_DATA_LIST );
          }
        }
        else
        if( m_netTopSelectFrameParam.out.result == NetTopSelectFrame::RESULT_SELECT_VIDEO_SEARCH )
        {
          //  シリアル入力へ
          seq = _StartSeq( UPDATE_SEQ_SERIAL_NO_INPUT );
        }
        else
        if( m_netTopSelectFrameParam.out.result == NetTopSelectFrame::RESULT_CANCEL )
        {
          //  選択せずにキャンセルしたのでアプリ終了
          seq = _StartSeq( UPDATA_SEQ_ROOT_APP_CANCEL );
        }
      }
    }
    else
    if( seq == UPDATE_SEQ_SERIAL_NO_INPUT )
    {
      // シリアル入力画面が必要
      if( m_subSeq == UPDATE_SEQ_SERIAL_NO_INPUT_SUB_FADE_OUT_START )
      {
        //  フェードアウト開始
        gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
        pFadeManager->RequestDefaultOut( gfl2::Fade::DISP_DOUBLE );
        m_subSeq  = UPDATE_SEQ_SERIAL_NO_INPUT_SUB_FADE_OUT_WAIT;
      }

      if( m_subSeq == UPDATE_SEQ_SERIAL_NO_INPUT_SUB_FADE_OUT_WAIT )
      {
        //  フェードアウト中
        gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
        if( pFadeManager->IsEnd( gfl2::Fade::DISP_DOUBLE ) == true )
        {
          m_subSeq  = UPDATE_SEQ_SERIAL_NO_INPUT_SUB_FRAME_CREATE;
        }
      }

      if( m_subSeq == UPDATE_SEQ_SERIAL_NO_INPUT_SUB_FRAME_CREATE )
      {
        // @fix GFNMCat[719] 入力フレーム内では通信エラーを解除
        GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->NotNotifySpanOn();

        // NumberInput DLL化のため追加
        gfl2::ro::RoManager* roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
        {
          m_pNumInputModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "NumberInput.cro");
          roMgr->StartModule( m_pNumInputModule, true );
        }

        //  シリアル入力フレーム作成
        m_mumberInputAppParam.callMode  = App::NumberInput::CALL_MODE_ALPHABET;
        m_mumberInputAppParam.callType  = App::NumberInput::CALL_TYPE_BATTLE_RECORDER;

        App::NumberInput::NumberInputFrame* pNumberInputFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, App::NumberInput::NumberInputFrame>( m_pFrameManager );
        pNumberInputFrame->Setup( &m_mumberInputAppParam, m_pRenderingManager );
        pNumberInputFrame->SetGameManager( GFL_SINGLETON_INSTANCE( GameSys::GameManager ) );

        m_subSeq  = UPDATE_SEQ_SERIAL_NO_INPUT_SUB_FRAME_WAIT;
      }
      else
      if( m_subSeq == UPDATE_SEQ_SERIAL_NO_INPUT_SUB_FRAME_WAIT )
      {
        //  シリアル入力フレーム待ち
        if( m_pFrameManager->IsMainHandle() == false )
        {
          // NumberInput DLL化のため追加
          gfl2::ro::RoManager* roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
          roMgr->DisposeModule( m_pNumInputModule );

          // @fix GFNMCat[719] 入力フレーム内では通信エラーを解除を戻して、エラーになっているかチェック。
          GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->NotNotifySpanOff();
          if( GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->IsError() == true )
          {
            // @fix GFNMCat[719]エラーになっている場合はProcを殺す(ここは手動にしかできない)
            return  gfl2::proc::RES_FINISH;
          }
          else
          {
            // @fix GFNMCat[812] シリアル入力が終ったらシステムメッセージが残っているので消す
            m_pUIViewPool->HideLowerSystemMessage();

            m_subSeq  = UPDATE_SEQ_SERIAL_NO_INPUT_SUB_FADE_IN_START;
          }
        }
      }
        
      if( m_subSeq == UPDATE_SEQ_SERIAL_NO_INPUT_SUB_FADE_IN_START )
      {
        //  フェードイン開始
        gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
        pFadeManager->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA );
        m_subSeq  = UPDATE_SEQ_SERIAL_NO_INPUT_SUB_FADE_IN_WAIT;
      }
      
      if( m_subSeq == UPDATE_SEQ_SERIAL_NO_INPUT_SUB_FADE_IN_WAIT )
      {
        //  フェードイン待ち
        gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
        if( pFadeManager->IsEnd( gfl2::Fade::DISP_DOUBLE ) )
        {
          m_subSeq  = UPDATE_SEQ_SERIAL_NO_INPUT_SUB_END;
        }
      }

      if( m_subSeq == UPDATE_SEQ_SERIAL_NO_INPUT_SUB_END )
      {
        //  シリアル入力検索遷移
        seq = _StartSeq( UPDATE_SEQ_SERIAL_NO_CREATE_DATA );
      }
    }
    else
    if( seq == UPDATE_SEQ_SERIAL_NO_CREATE_DATA )
    {
      enum
      {
        UPDATE_SEQ_SERIAL_NO_CREATE_DATA_SUB_SEQ_LOAD = 0,
        UPDATE_SEQ_SERIAL_NO_CREATE_DATA_SUB_SEQ_ERROR,
      };

      if( m_subSeq == UPDATE_SEQ_SERIAL_NO_CREATE_DATA_SUB_SEQ_LOAD )
      {
        NetAppLib::BattleVideoPlayer::VideoDataManager::SerialNoLoadResult  result  = m_pAppParam->pVideoDataManager->LoadSerialNoData( m_serialNo, m_pAppMsgData, m_pWordSet );
        if( result != NetAppLib::BattleVideoPlayer::VideoDataManager::SERIAL_NO_LOAD_RESULT_RUN )
        {
          if( result == NetAppLib::BattleVideoPlayer::VideoDataManager::SERIAL_NO_LOAD_RESULT_SUCCESS )
          {
            //  ビデオ操作へ
            seq = BaseProc::_StartDataCtrlSeq( m_pAppParam->pVideoDataManager->GetSerialData(), DATA_CTRL_FRAME_MODE_NET );

            //  システムメッセージを非表示 
            m_pUIViewPool->HideLowerSystemMessage();
          }
          else
          if( result == NetAppLib::BattleVideoPlayer::VideoDataManager::SERIAL_NO_LOAD_RESULT_NOT_FOUND )
          {
            //  見つからない
            //  メッセージ表示
            {
              const UIViewPool::INIT_PARAM& rInitParam  = m_pUIViewPool->GetInitParam();
              m_pUIViewPool->ShowLowerSystemMessage( rInitParam.pAppMsgData, msg_bvp_win_20, false, false, false );
            }
            m_subSeq = UPDATE_SEQ_SERIAL_NO_CREATE_DATA_SUB_SEQ_ERROR;
          }
          else
          if( result == NetAppLib::BattleVideoPlayer::VideoDataManager::SERIAL_NO_LOAD_RESULT_ERROR_BATCH_VERSION )
          {
            //  バッチバージョンエラー
            //  メッセージ表示
            {
              const UIViewPool::INIT_PARAM& rInitParam  = m_pUIViewPool->GetInitParam();
              m_pUIViewPool->ShowLowerSystemMessage( rInitParam.pAppMsgData, msg_bvp_win_21, false, false, false );
            }
            m_subSeq = UPDATE_SEQ_SERIAL_NO_CREATE_DATA_SUB_SEQ_ERROR;
          }
          else
          if( result == NetAppLib::BattleVideoPlayer::VideoDataManager::SERIAL_NO_LOAD_RESULT_ERROR_SERVER_VERSION )
          {
            //  サーバーバージョンエラー
            //  メッセージ表示
            {
              const UIViewPool::INIT_PARAM& rInitParam  = m_pUIViewPool->GetInitParam();
              m_pUIViewPool->ShowLowerSystemMessage( rInitParam.pAppMsgData, msg_bvp_win_22, false, false, false );
            }
            m_subSeq = UPDATE_SEQ_SERIAL_NO_CREATE_DATA_SUB_SEQ_ERROR;
          }
          else
          if( result == NetAppLib::BattleVideoPlayer::VideoDataManager::SERIAL_NO_LOAD_RESULT_ERROR )
          {
            //  汎用エラー
            //  メッセージ表示
            {
              const UIViewPool::INIT_PARAM& rInitParam  = m_pUIViewPool->GetInitParam();
              m_pUIViewPool->ShowLowerSystemMessage( rInitParam.pAppMsgData, msg_bvp_win_22, false, false, false );
            }
            m_subSeq = UPDATE_SEQ_SERIAL_NO_CREATE_DATA_SUB_SEQ_ERROR;
          }
        }
        else
        //  まだロード中
        {
          // @fix GFNMCat[812] すぐに終了するケースがあるので、カウント待ちする
          if( m_pUIViewPool->IsEnableLowerSystemMessage() == false )
          {
            const u32 cDwLoadMessageWait  = 3;
            if( cDwLoadMessageWait == m_commonCount )
            {
              //  メッセージ表示(通信中)
              const UIViewPool::INIT_PARAM& rInitParam  = m_pUIViewPool->GetInitParam();
              m_pUIViewPool->ShowLowerSystemMessage( rInitParam.pAppMsgData, msg_bvp_win_19, true, true, false );

              ++m_commonCount;
            }
            else
            {
              ++m_commonCount;
            }
          }
        }
      }
      else
      if( m_subSeq == UPDATE_SEQ_SERIAL_NO_CREATE_DATA_SUB_SEQ_ERROR )
      {
        if( m_pUIViewPool->IsEndProcLowerSystemMessage() == true )
        {
          seq = _StartSeq( UPDATE_SEQ_TOP );
        }
      }
      else
      {
        //  ロード中
      }
    }
    else
    //  バトルビデオリスト
    if( seq == UPDATE_SEQ_ROOT_DATA_LIST )
    {
      if( m_pFrameManager->IsMainHandle() == false )
      {
        if( m_dataListFrameParam.out.result == DataListFrame::RESULT_CANCEL )
        {
          //  データを選択せずにキャンセルしたのでトップシーケンスへ戻る
          seq = _StartSeq( UPDATE_SEQ_TOP );
        }
        else
        if( m_dataListFrameParam.out.result == DataListFrame::RESULT_DATA_FATAL )
        {
          //  失敗設定してアプリ終了
          seq = _StartSeq( UPDATE_SEQ_ROOT_APP_FATAL );          
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
          seq = _StartDataCtrlSeq( m_dataListFrameParam.out.selectDataIdx, DATA_CTRL_FRAME_MODE_NET );
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
          if( m_dataCtrlFrameParam.in.pVideoData->bServerDataFlg == 1 )
          {
            //  シリアルNOの入力のデータなので２択に戻る
            seq = _StartSeq( UPDATE_SEQ_TOP );
          }
          else
          {
            //  ローカルデータなのでリストに戻る
            seq = _StartSeq( UPDATE_SEQ_ROOT_DATA_LIST );
          }
        }
        else
        if( result == DataCtrlBaseFrame::RESULT_FATAL )
        {
          //  失敗設定してアプリ終了
          seq = _StartSeq( UPDATE_SEQ_ROOT_APP_FATAL );
        }
        else
        //  ビデオ再生選択した
        if( result == DataCtrlBaseFrame::RESULT_VIDEO_PLAY )
        {
          //  再生処理
          //  データが他人のなら再生回数を上げる
          if( m_dataCtrlFrameParam.in.pVideoData->bServerDataFlg == 1 )
          {
            seq = _StartSeq( UPDATA_SEQ_INC_PLAY_COUNT );
          }
          else
          {
            seq = _StartSeq( UPDATA_SEQ_ROOT_BTL_PLAY );
          }
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
    else
    if( seq == UPDATA_SEQ_INC_PLAY_COUNT )
    {
      NetAppLib::BattleVideoPlayer::VideoDataManager::IncPlayCountResult  result  = m_pVideoDataManager->UpdateIncPlayCountVideoData();
      if( result == NetAppLib::BattleVideoPlayer::VideoDataManager::INC_PLAY_COUNT_RESULT_SUCCESS )
      {
        m_pUIViewPool->HideLowerSystemMessage();
        seq = _StartSeq( UPDATA_SEQ_ROOT_BTL_PLAY );
      }
      else
      if( result == NetAppLib::BattleVideoPlayer::VideoDataManager::INC_PLAY_COUNT_RESULT_ERROR )
      {
        m_pUIViewPool->HideLowerSystemMessage();
        seq = _StartSeq( UPDATA_SEQ_ROOT_BTL_PLAY );
      }
    }

    GameProc::SetSubSeq( seq );

    procResult  = BaseProc::UpdateFunc( pManager );

    return procResult;
  }

  u32 NetProc::_StartSeq( const u32 seq )
  {
    u32 newSeq  = seq;

    newSeq = BaseProc::_StartSeq( newSeq );
    if( newSeq < UPDATE_SEQ_ROOT_OTHER )
    {
      //  基本クラス内のシーケンス設定なので終了
      return  newSeq;
    }

    if( newSeq == UPDATE_SEQ_TOP )
    {
      //  トップ選択を実行

      //  フレーム作成
      {
        m_netTopSelectFrameParam.in.pUIViewPool       = m_pUIViewPool;

        NetTopSelectFrame* pFrame  = MENU_FRAME_CALL_MODAL<applib::frame::Manager, NetTopSelectFrame>( m_pFrameManager );
        pFrame->Initialize( &m_netTopSelectFrameParam );
      }
    }
    else
    if( newSeq == UPDATE_SEQ_SERIAL_NO_INPUT )
    {
      //  Bボタンを非表示
      m_pUIViewPool->GetLowRootView()->SetVisiblePushBtn( LowRootView::BUTTON_ID_BACK, false );
    }
    else
    if( newSeq == UPDATE_SEQ_SERIAL_NO_CREATE_DATA )
    {
      // 入力された16進数文字列を数値に変換
      {
        gfl2::str::STRCODE inputStr[ App::NumberInput::PART_MAX ][App::NumberInput::ALPHABET_INPUT_MAX + 1 ];

        gfl2::str::StrBuf inStr( NetApp::Util::ConvertUtility::STR_MAX + System::EOM_LEN, m_pDevMem );

        // 終端文字
        inputStr[0][4] = L'\0';
        inputStr[1][4] = L'\0';
        inputStr[2][4] = L'\0';
        inputStr[3][4] = L'\0';

        for( u32 i = 0 ; i < App::NumberInput::PART_MAX ; ++i )
        {
          for( u32 j = 0 ; j < App::NumberInput::ALPHABET_INPUT_MAX ; ++j )
          {
            wchar_t wc = m_mumberInputAppParam.inputStr[ i ] [ j ];
            //GFL_PRINT("wc == %d\n",wc);
            if( wc >= L'０' && wc <= L'９' )
            {
              wc -= L'０';
              inputStr[ i ][ j ] = wc + L'0';
            }
            else
            if( wc >= L'Ａ' && wc <= L'Ｚ' )
            {
              wc -= L'Ａ';
              inputStr[ i ][ j ] = wc + L'A';
            }
            else
            {
              inputStr[ i ][ j ] = wc;
            }

            inStr.AddCode( inputStr[ i ][ j ] );
          }
        }

        GFL_PRINT("シリアル入力 %ls\n", inStr.GetPtr() );

        m_serialNo  = NetApp::Util::ConvertUtility::StrToU64( &inStr );
        GFL_PRINT("数値に変換 %llu\n", m_serialNo );

        // 要望：0で埋められた番号だった時は、入力履歴を削除する
        {
          gfl2::str::StrBuf zeroStr( L"0000000000000000", m_pDevMem );

          // 比較
          if( inStr.Compare( zeroStr ) == true )
          {
            m_mumberInputAppParam.inputStr[ 0 ][ 0 ] = gfl2::str::EOM_CODE;
            GFL_PRINT("文字入力履歴クリア\n");
          }
        }
      }

      //  Bボタンを非表示
      m_pUIViewPool->GetLowRootView()->SetVisiblePushBtn( LowRootView::BUTTON_ID_BACK, false );

      //  シリアルNOからビデオデータ生成
      if( m_pAppParam->pVideoDataManager->InitSerialNoData() == false )
      {
        //  終了
        GFL_ASSERT( 0 );
        newSeq = _StartSeq( UPDATE_SEQ_TOP );
      }
      else
      {
        //  システムメッセージを非表示 
        m_pUIViewPool->HideLowerSystemMessage();
      }
    }
    else
    if( newSeq == UPDATA_SEQ_INC_PLAY_COUNT )
    {
      //  シリアルデータの再生カウントをあげる
      if( m_pVideoDataManager->StartIncPlayCountVideoData( m_dataCtrlFrameParam.in.pVideoData ) == false )
      {
        //  失敗したが再生する
        newSeq  = _StartSeq( UPDATA_SEQ_ROOT_BTL_PLAY );
      }
      else
      {
        const UIViewPool::INIT_PARAM& rInitParam  = m_pUIViewPool->GetInitParam();
        m_pUIViewPool->ShowLowerSystemMessage( rInitParam.pAppMsgData, msg_bvp_win_23, true, true, false );
      }
    }

    m_subSeq  = 0;

    return  newSeq;
  }

  //  復帰データからシーケンス開始
  u32 NetProc::_StartReturnSeq( const PROC_RUN_TYPE_RETURN_DATA& rReturnData )
  {
    return  BaseProc::_StartReturnSeq( rReturnData );
  }

  //  フレーム管理のみの描画にするか
  bool NetProc::_GetFrameMangerProcOnly()
  {
    int seq = GameProc::GetSubSeq();
    if( seq == UPDATE_SEQ_SERIAL_NO_INPUT )
    {
      //  シリアル番号入力時はこちらのViewの処理はしない
      if( m_subSeq == UPDATE_SEQ_SERIAL_NO_INPUT_SUB_FRAME_CREATE )
      {
        return  true;
      }
      else
      if( m_subSeq == UPDATE_SEQ_SERIAL_NO_INPUT_SUB_FRAME_WAIT )
      {
        return  true;
      }
    }

    return  false;
  }

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetApp)

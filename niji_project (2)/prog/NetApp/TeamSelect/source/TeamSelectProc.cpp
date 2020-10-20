//==============================================================================
/**
 @file    TeamSelectProc.h
 @date    2015.06.23
 @note
          チーム選択プロセス
 */
//==============================================================================
#include "NetApp/TeamSelect/include/TeamSelectProc.h"
#include "NetApp/TeamSelect/source/TeamSelectUIViewPool.h"
#include "NetApp/TeamSelect/source/TeamSelectShareParam.h"

#include "NetStatic/NetAppLib/include/UI/NetAppRegulationDrawInfo.h"

//  gflib2
#include <Fade/include/gfl2_FadeManager.h>
#include <Print/include/WordSet.h>
#include <Print/include/SystemFont.h>

//  niji
#include "System/include/HeapDefine.h"
#include "Applib/include/Frame/AppFrameManager.h"
#include "Applib/include/Util/AppRenderingManager.h"
#include "Applib/include/Util/AppUtilFileControlState.h"
#include "Applib/include/Tool/app_tool_MenuCursor.h"
#include "Applib/include/Tool/PokeIcon.h"
#include "AppLib/include/Ui/UIViewManager.h"

#include "PokeTool/include/PokeToolBattleParty.h"

#include "arc_def_index/arc_def.h"
#include "arc_index/teamSelect.gaix"
#include "arc_index/message.gaix"

#include "niji_conv_header/message/msg_battleteam_select.h"

#include "GameSys/include/GameData.h"

// レギュレーション
#include "Battle/Regulation/include/RegulationDebug.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  TeamSelectProc::TeamSelectProc( APP_PARAM* pAppParam ) :
  GameProc()
  {
    GFL_ASSERT( pAppParam );

    _Clear();

    m_pAppParam = pAppParam;

#if PM_DEBUG
    // レギュレーションデバッグ機能に操作したいレギュレーション渡す
    gfl2::heap::HeapBase* pDebugHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG);
    m_pRegulationDebug = GFL_NEW( pDebugHeap ) RegulationDebug( &pAppParam->in.pRegulationDrawInfo->GetRegulation() );
#endif
  }

  TeamSelectProc::~TeamSelectProc()
  {
#if PM_DEBUG
    // レギュレーションデバッグ機能破棄
    GFL_SAFE_DELETE( m_pRegulationDebug );
#endif
  }

  gfl2::proc::Result TeamSelectProc::InitFunc( gfl2::proc::Manager* /*pManager*/ )
  {
    //  アプリ内で使用するメモリ量
    static const s32 HEAP_MEM_SIZE = 0xf0000;
    static const s32 DEV_MEM_SIZE  = 1024 * 1024 * 30;

    static const s32 LOCAL_HEAP_SIZE = 1024 * 600;
    static const s32 LOCAL_DEVICE_HEAP_SIZE  = 1024 * 1024 * 25;

    enum
    {
      SEQ_INIT  = 0,
      SEQ_LOAD,
      SEQ_LOAD_POKE_ICON,
      SEQ_WAIT_LOAD_POKE_ICON,
      SEQ_OBJ_CREATE,
      SEQ_WAIT_OBJ_CREATE,
      SEQ_END
    };

    int seq = GameProc::GetSubSeq();

    //  初期化( メモリなど必要なのを作成 )
    if( seq == SEQ_INIT )
    {
      //  フェードを使って、黒で覆う
      {
        gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
        pFadeManager->ForceDefaultOut( gfl2::Fade::DISP_DOUBLE );
      }

      //  全体が使用するメモリを確保
      {
        gfl2::heap::HeapBase* parentHeapMem = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
        m_pHeapMem = GFL_CREATE_LOCAL_HEAP( parentHeapMem, HEAP_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );

        gfl2::heap::HeapBase* parentDevMem = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
        m_pDevMem = GFL_CREATE_LOCAL_HEAP( parentDevMem, DEV_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
      }

      // ローカルヒープ生成
      {
        m_pUtilheap = GFL_NEW( m_pDevMem ) app::util::Heap();
        m_pUtilheap->LocalHeapCreate( m_pHeapMem, m_pDevMem, LOCAL_HEAP_SIZE, LOCAL_DEVICE_HEAP_SIZE );
      }

      //  数字フォントは常駐化
      {
        print::SystemFont_CreateNumFont( m_pUtilheap->GetSystemHeap(), m_pUtilheap->GetDeviceHeap() );
        print::SystemFont_SetLytSystemNumFont();
      }

      // パイプライン生成
      {
        app::util::AppRenderingManager::Description renderingMgrDesc;
        renderingMgrDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
        renderingMgrDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
        m_pRenderingManager = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) app::util::AppRenderingManager();
        m_pRenderingManager->SyncCreate( renderingMgrDesc, m_pUtilheap, NULL, GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() );
        GFL_ASSERT( m_pRenderingManager->IsCreated() );  // 同期生成なので m_pAppRenderingMgr->IsCreated() では待たない。
      }

      //  フレーム管理生成
      {
        // このマネージャーに必要なメモリを作成  これはヒープが穴になるのでここから
        m_pFrameManagerHeap = GFL_CREATE_LOCAL_HEAP( m_pUtilheap->GetDeviceHeap(), 4096, gfl2::heap::HEAP_TYPE_EXP, false );

        m_pFrameManager = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) applib::frame::Manager( m_pFrameManagerHeap );
      }

      //  サーバークライアント作成
      {
        m_pServerRequestClient  = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) TeamSelectRequestClient( m_pUtilheap->GetDeviceHeap() );
      }

      //  UIView生成ファクトリーを生成
      {
        m_pUIViewPool  = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) TeamSelectUIViewPool();
      }

      //  レイアウトファイルロード処理開始
      {
        m_pLayoutFileCtrlState  = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) app::util::AppUtilFileControlState( ARCID_TEAM_SELECT, GARC_teamSelect_teamSelect_applyt_COMP, m_pUtilheap->GetDeviceHeap(), true, 128 );
        m_pLayoutFileCtrlState->Start();
      }

      //  UI画面で使う共通メッセージ
      //  黒画面中なので同期読みでいいや
      m_pAppMsgData = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) gfl2::str::MsgData( print::GetMessageArcId( System::MSGLANGID_MAX ), GARC_message_battleteam_select_DAT, m_pUtilheap->GetDeviceHeap(), gfl2::str::MsgData::LOAD_FULL );
      GFL_ASSERT( m_pAppMsgData );

      //  UIで使用するwordset
      m_pWordSet = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) print::WordSet( print::WordSet::DEFAULT_WORD_MAX, print::WordSet::DEFAULT_BUF_LEN , m_pUtilheap->GetDeviceHeap() );
      GFL_ASSERT( m_pWordSet );

      //  メニューカーソルのリソース作成
      m_pMenuCursor = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) app::tool::MenuCursor( m_pUtilheap, &m_pMenuCursorLytBuffer );
      GFL_ASSERT( m_pMenuCursor );

      //  チーム描画パラメータ作成
      {
        m_teamNum = m_pAppParam->in.teamDataNum;
        if( 0 < m_teamNum )
        {
          m_pTeamViewParamList  = GFL_NEW_ARRAY( m_pUtilheap->GetDeviceHeap() ) struct TEAM_VIEW_PARAM[ m_teamNum ];
          for( u32 i = 0; i < m_teamNum; ++i )
          {
            //  手持ちポケモンのチームデータの場合はチーム名を個別で文字列に出力
            if( m_pAppParam->in.aTeamData[ i ].bPokeTemochiData == true )
            {
              // @fix GFNMCat[1420] 「てもち」の名前を上書きしない
//              m_pAppMsgData->GetString( msg_bts_sys_10, *( m_pAppParam->in.aTeamData[ i ].pPokeParty->GetTeamName() ) );

              //  初回リスト選択idxを決める
              //  手持ちがあるなら最初は手持ちを指定
              if( m_pAppParam->in.currentTeamListIdx == CURRENT_TEAM_SELECT_IDX_DEFAULT )
              {
                m_pAppParam->in.currentTeamListIdx  = i;
              }
            }

            m_pTeamViewParamList[ i ].pTeamData = &m_pAppParam->in.aTeamData[ i ];

            gfl2::std::MemClear( m_pTeamViewParamList[ i ].aPokeIconDataId, sizeof( m_pTeamViewParamList[ i ].aPokeIconDataId ) );
          }

          //  チームリストがあるのにカレント位置がまだデフォルトの場合は0で初期化
          if( m_pAppParam->in.currentTeamListIdx == CURRENT_TEAM_SELECT_IDX_DEFAULT )
          {
            m_pAppParam->in.currentTeamListIdx  = 0;
          }
        }
      }

      //  ポケモンアイコンのリソース作成
      //  チームデータがあるときだけ作成する(データがない時はアイコン表示するケースがないから)
      {
        if( 0 < m_teamNum )
        {
          m_pPokeIconResManager = GFL_NEW( m_pUtilheap->GetDeviceHeap() ) app::tool::PokeIcon( m_pUtilheap->GetDeviceHeap(), m_pUtilheap->GetDeviceHeap(), m_teamNum * pml::PokeParty::MAX_MEMBERS );
          //  ファイルを開く
          m_pPokeIconResManager->FileOpen( m_pUtilheap->GetDeviceHeap() );
        }
      }

      ++seq;
    }
    //  ファイルロード( ファイルの読み込みをする )
    if( seq == SEQ_LOAD )
    {
      bool  bLoaded = true;

      m_pLayoutFileCtrlState->Update();
      if( m_pMenuCursor->LoadResource() == false )
      {
        bLoaded = false;
      }

      if( m_pLayoutFileCtrlState->IsEnd() == false )
      {
        bLoaded = false;
      }

      //  チームがひとつもなくて作成する必要がないケースがあるのでチェックが必要
      if( ( m_pPokeIconResManager != NULL ) && ( m_pPokeIconResManager->IsFileOpen() == false ) )
      {
        bLoaded = false;
      }

      if( bLoaded == true )
      {
        ++seq;
      }
    }
    //  ポケモンアイコンをロード
    if( seq == SEQ_LOAD_POKE_ICON )
    {
      //  チームがひとつもなくて作成する必要がないケースであればスキップする
      if( m_pPokeIconResManager == NULL )
      {
        ++seq;
      }
      else
      {
        //  ポケモンアイコンを事前にロード
        u32 pokePartyMemberCnt        = 0;
        pml::PokeParty*  pPokeParty   = NULL;

        u32 texDataArrayIdx = 0;

        for( u32 i = 0; i < m_teamNum; ++i )
        {
          pPokeParty  = m_pTeamViewParamList[ i ].pTeamData->pPokeParty->GetPokeParty();
          pokePartyMemberCnt  = pPokeParty->GetMemberCount();

          for( u32 pokePartyIdx = 0; pokePartyIdx < pokePartyMemberCnt; ++pokePartyIdx )
          {
            //  重複アイコンをロードするがサイズが小さいので、気にないでいく
            m_pPokeIconResManager->ReadRequest( texDataArrayIdx, pPokeParty->GetMemberPointer( pokePartyIdx ) );

            m_pTeamViewParamList[ i ].aPokeIconDataId[ pokePartyIdx ] = texDataArrayIdx;

            ++texDataArrayIdx;
          }
        }

        //  次へ
        ++seq;
      }
    }
    //  アイコンロード待ち
    if( seq == SEQ_WAIT_LOAD_POKE_ICON )
    {
      u32 pokePartyMemberCnt        = 0;
      pml::PokeParty*  pPokeParty   = NULL;

      bool  bEnd  = true;
      if( m_pPokeIconResManager != NULL )
      {
        for( u32 i = 0; i < m_teamNum; ++i )
        {
          pPokeParty  = m_pTeamViewParamList[ i ].pTeamData->pPokeParty->GetPokeParty();
          pokePartyMemberCnt  = pPokeParty->GetMemberCount();

          for( u32 pokePartyIdx = 0; pokePartyIdx < pokePartyMemberCnt; ++pokePartyIdx )
          {
            //  アイコンのロード待ち
            if( m_pPokeIconResManager->IsLoadFinished( m_pTeamViewParamList[ i ].aPokeIconDataId[ pokePartyIdx ] ) == false )
            {
              bEnd  = false;
              break;
            }
          }
        }
      }

      if( bEnd == true )
      {
        ++seq;
      }
    }
    //  オブジェクトを作成( Viewはすべてここで作成する )
    if( seq == SEQ_OBJ_CREATE )
    {
      struct TeamSelectUIViewPool::INIT_PARAM initParam;
      initParam.pAppMsgData         = m_pAppMsgData;
      initParam.pLytBinData         = m_pLayoutFileCtrlState->GetLoadDataBuffer();
      initParam.pMenuCursorBinData  = m_pMenuCursorLytBuffer;
      initParam.pRenderManager      = m_pRenderingManager;
      initParam.pUtilHeap           = m_pUtilheap;
      initParam.pWordSet            = m_pWordSet;
      initParam.pPokeIconTexManager = m_pPokeIconResManager;
      initParam.pTeamData           = m_pTeamViewParamList;
      initParam.teamDataNum         = m_teamNum;
      initParam.firstTeamCurrentIdx = m_pAppParam->in.currentTeamListIdx;

      m_pUIViewPool->Initialize( initParam );

      ++seq;
    }
    
    if( seq == SEQ_WAIT_OBJ_CREATE )
    {
      if( m_pUIViewPool->IsReady() == true )
      {
        TeamSelectUppRootView*  pUppRootView  = m_pUIViewPool->GetUppRootView();
        pUppRootView->ShowMessageWindow( msg_bts_sys_01 );

        ++seq;
      }
    }

    //  終了
    if( seq == SEQ_END )
    {
      // @fix GFNMCat[2415] ボタンアニメ中は入力封鎖を設定
      app::ui::UIViewManager* pUIViewMan = GFL_SINGLETON_INSTANCE( app::ui::UIViewManager );
      m_bOldInputExclusiveKey = pUIViewMan->GetInputExclusiveKeyByButtonAnimation();
      pUIViewMan->SetInputExclusiveKeyByButtonAnimation( true );

      return gfl2::proc::RES_FINISH;
    }

    GameProc::SetSubSeq( seq );

    return gfl2::proc::RES_CONTINUE;
  }

  gfl2::proc::Result TeamSelectProc::UpdateFunc( gfl2::proc::Manager* /*pManager*/ )
  {
    int seq = GameProc::GetSubSeq();

    //  上下のルートViewを取得
    TeamSelectLowRootView*  pLowRootView  = m_pUIViewPool->GetLowRootView();
    TeamSelectUppRootView*  pUppRootView  = m_pUIViewPool->GetUppRootView();

    //  フェードイン開始
    if( seq == UPDATE_SEQ_FADE_IN_START )
    {
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      pFadeManager->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA );

      ++seq;
    }
    //  フェードインの完了を待っている
    if( seq == UPDATE_SEQ_FADE_IN_WAIT )
    {
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      if( pFadeManager->IsEnd( gfl2::Fade::DISP_DOUBLE ) )
      {
        bool  bNextDefaultFlow = true;
        u32 enterListIdx = _GetAppintEnterTeamDataIdx();
        if( enterListIdx != SELECT_TEAM_DATA_IDX_ERROR )
        {
          //  ポケモンがいる
          if( m_pAppParam->in.aTeamData[ enterListIdx ].pPokeParty != NULL )
          {
            if( 0 < m_pAppParam->in.aTeamData[ enterListIdx ].pPokeParty->GetPokeParty()->GetMemberCount() )
            {
              //  初期参照するチームデータidx
              m_topPokePartySelectFrameAppParam.out.selectTeamIdx = enterListIdx;

              //  すでにチームが選択されている
              seq = _SetSeq( UPDATE_SEQ_TEAM_ENTRY_CONFIRMATION_START );

              bNextDefaultFlow = false;
            }
          }
        }
        
        if( bNextDefaultFlow == true )
        {
          //  デフォルト選択
          ++seq;
        }
      }
    }
    //  トップ画面処理開始
    if( seq == UPDATE_SEQ_TOP_START )
    {
      //  トップ画面のframe作成
      if( _IsTeamData() == true )
      {
        //  フレーム生成
        m_pTopSeqSelectFrame  = NULL;
        m_pTopPokePartySelectFrame  = MENU_FRAME_CALL_MODAL<applib::frame::Manager, TeamSelectTopPokePartySelectFrame>( m_pFrameManager );

        //  必要なパラメータを設定
        {
          m_topPokePartySelectFrameAppParam.Reset();
          m_topPokePartySelectFrameAppParam.in.pUIViewPool          = m_pUIViewPool;
          m_topPokePartySelectFrameAppParam.in.pPokeIconResManager  = m_pPokeIconResManager;
        }

        //  パラメータを設定して初期化
        m_pTopPokePartySelectFrame->Initialize( &m_topPokePartySelectFrameAppParam );
      }
      else
      {
        //  フレーム生成
        m_pTopPokePartySelectFrame  = NULL;
        m_pTopSeqSelectFrame  = MENU_FRAME_CALL_MODAL<applib::frame::Manager, TeamSelectTopSeqSelectFrame>( m_pFrameManager );

        //  必要なパラメータを設定
        {
          m_topSeqSelectFrameAppParam.Reset();
          m_topSeqSelectFrameAppParam.pUIViewPool = m_pUIViewPool;
          m_topSeqSelectFrameAppParam.bOpenRegulationButton = m_pAppParam->in.bOpenRegulationButton;
          m_topSeqSelectFrameAppParam.bQRPokePartyButton    = m_pAppParam->in.bQRPokePartyButton;
        }

        //  パラメータを設定して初期化
        m_pTopSeqSelectFrame->Initialize( &m_topSeqSelectFrameAppParam );
      }

      ++seq;
    }
    //  トップ画面処理の待ちを待っている
    if( seq == UPDATE_SEQ_TOP_WAIT )
    {
      if( pLowRootView->IsPushBackBtn() == true )
      {
        //  キャンセルボタンを押している
        ++seq;
      }
      else
      {
        if( m_pTopPokePartySelectFrame != NULL )
        {
          //  チーム選択モード

          //  終了待ち
          if( m_topPokePartySelectFrameAppParam.out.bEnd == true )
          {
            ++seq;
          }
        }
        else
        if( m_pTopSeqSelectFrame != NULL )
        {
          //  QR選択モード

          //  終了待ち
          if( m_topSeqSelectFrameAppParam.bEnd == true )
          {
            ++seq;
          }
        }
      }
    }
    //  トップ画面の終了
    if( seq == UPDATE_SEQ_TOP_END )
    {
      //  トップ画面の終了処理をしているか
      if( m_pFrameManager->End() == false )
      {
        //  アプリ終了
        if( pLowRootView->IsPushBackBtn() == true )
        {
          m_pAppParam->out.procResultType = PROC_RESULT_TYPE_CANCEL;
          seq = _SetSeq( UPDATE_SEQ_END );
        }
        else
        {
          if( m_pTopPokePartySelectFrame != NULL )
          {
            m_pTopPokePartySelectFrame  = NULL;

            //  チーム選択idxを設定
            struct TeamSelectTopPokePartySelectFrame::APP_PARAM::_tag_out_* pSelectPokePartyOutParam  = &m_topPokePartySelectFrameAppParam.out;
            m_pAppParam->out.selectTeamIdx  = pSelectPokePartyOutParam->selectTeamIdx;

            //  選択したチームの確認に繊維
            if( m_topPokePartySelectFrameAppParam.out.push == TeamSelectTopPokePartySelectFrame::PUSH_SELECT_POKE_PARTY )
            {
              //  ポケモンパーティを無事選択
              seq = _SetSeq( UPDATE_SEQ_TEAM_ENTRY_CONFIRMATION_START );
            }
            else
            if( m_topPokePartySelectFrameAppParam.out.push == TeamSelectTopPokePartySelectFrame::PUSH_TEAM_SETTING )
            {
              //  ボックスに繊維
              m_pAppParam->out.procResultType = PROC_RESULT_TYPE_NEXT_TEAM_SETTING_SCENE;
              seq = _SetSeq( UPDATE_SEQ_END );
            }
            else
            if( m_topPokePartySelectFrameAppParam.out.push == TeamSelectTopPokePartySelectFrame::PUSH_SELECT_QR_PARTY )
            {
              m_pAppParam->out.procResultType = PROC_RESULT_TYPE_NEXT_QR_READ_SCENE;
              seq = _SetSeq( UPDATE_SEQ_END );
            }
          }
          else
          if( m_pTopSeqSelectFrame != NULL )
          {
            m_pTopSeqSelectFrame  = NULL;

            if( m_topSeqSelectFrameAppParam.push == TeamSelectTopSeqSelectFrame::PUSH_QR_READ )
            {
              //  QRコードに繊維
              m_pAppParam->out.procResultType = PROC_RESULT_TYPE_NEXT_QR_READ_SCENE;
              seq = _SetSeq( UPDATE_SEQ_END );
            }
            else
            if( m_topSeqSelectFrameAppParam.push == TeamSelectTopSeqSelectFrame::PUSH_TEAM_SETTING )
            {
              //  ボックスに繊維
              m_pAppParam->out.procResultType = PROC_RESULT_TYPE_NEXT_TEAM_SETTING_SCENE;
              seq = _SetSeq( UPDATE_SEQ_END );
            }
            else
            if( m_topSeqSelectFrameAppParam.push == TeamSelectTopSeqSelectFrame::PUSH_REQULATION_SHOW )
            {
              //  レギュレーション確認に繊維
              m_pAppParam->out.procResultType = PROC_RESULT_TYPE_NEXT_REQULATION_CHK_SCENE;
              seq = _SetSeq( UPDATE_SEQ_END );
            }
            else
            {
              GFL_ASSERT( 0 );
            }
          }
        }
      }
    }
    //  チーム選択決定のyes/no開始
    if( seq == UPDATE_SEQ_TEAM_ENTRY_CONFIRMATION_START )
    {
      //  フレーム生成
      TeamSelectConfirmSelectPokePartyFrame*  pFrame  = MENU_FRAME_CALL_MODAL<applib::frame::Manager, TeamSelectConfirmSelectPokePartyFrame>( m_pFrameManager );

      //  必要なパラメータを設定
      {
        GFL_ASSERT( m_topPokePartySelectFrameAppParam.out.selectTeamIdx < m_teamNum );
        m_confirmSelectPokePartyFrameAppParam.Reset();
        m_confirmSelectPokePartyFrameAppParam.in.pUIViewPool           = m_pUIViewPool;
        m_confirmSelectPokePartyFrameAppParam.in.pSelectTeamViewParam  = &m_pTeamViewParamList[ m_topPokePartySelectFrameAppParam.out.selectTeamIdx ];
        m_confirmSelectPokePartyFrameAppParam.in.tournamentType        = m_pAppParam->in.tournamentType;
      }

      //  パラメータを設定して初期化
      pFrame->Initialize( &m_confirmSelectPokePartyFrameAppParam );

      ++seq;
    }
    //  チーム選択決定のyes/no待ち
    if( seq == UPDATE_SEQ_TEAM_ENTRY_CONFIRMATION_WAIT )
    {
      if( m_confirmSelectPokePartyFrameAppParam.out.bEnd == true )
      {
        //  m_pFrameManager->End() で終了させる
        ++seq;
      }
    }
    //  チーム選択決定のyes/no終了
    if( seq == UPDATE_SEQ_TEAM_ENTRY_CONFIRMATION_END )
    {
      //  関連するフレームしかないのが前提
      if( m_pFrameManager->End() == false )
      {
        struct TeamSelectServerRequestRegulationFrame::APP_PARAM::_tag_in_param_* pInParam  = &m_serverRequestRegulationFrameAppParam.in;
        pInParam->serverRequestClientParam.isQRTeam = m_pTeamViewParamList[ m_topPokePartySelectFrameAppParam.out.selectTeamIdx ].pTeamData->bQRReaderData;

        //  QRロードフラグは消す（トップ画面に戻った時にＱＲ項目しかでないのを防ぐため）
        m_pTeamViewParamList[ m_topPokePartySelectFrameAppParam.out.selectTeamIdx ].pTeamData->bQRReaderData = false;

        if( m_confirmSelectPokePartyFrameAppParam.out.push == TeamSelectConfirmSelectPokePartyFrame::PUSH_YES )
        {
          //  アプリ結果出力に選択したチームidxを設定
          {
            m_pAppParam->out.selectTeamIdx  = m_topPokePartySelectFrameAppParam.out.selectTeamIdx;
          }

          //  レギュレーション確認へ繊維
          seq = _SetSeq( UPDATE_SEQ_CHK_REQULATION_START );
        }
        else
        if( m_confirmSelectPokePartyFrameAppParam.out.push == TeamSelectConfirmSelectPokePartyFrame::PUSH_NO )
        {
          //  キャンセルならパーティ選択に
          seq = _SetSeq( UPDATE_SEQ_TOP_START );
        }
        else
        {
          GFL_ASSERT( 0 );
        }
      }
    }
    //  レギュレーション確認開始
    if( seq == UPDATE_SEQ_CHK_REQULATION_START )
    {
      //  フレーム生成
      TeamSelectServerRequestRegulationFrame*  pFrame  = MENU_FRAME_CALL_MODAL<applib::frame::Manager, TeamSelectServerRequestRegulationFrame>( m_pFrameManager );

      //  必要なパラメータを設定
      {
        m_serverRequestRegulationFrameAppParam.Reset();

        struct TeamSelectTopPokePartySelectFrame::APP_PARAM::_tag_out_* pSelectPokePartyOutParam  = &m_topPokePartySelectFrameAppParam.out;

        struct TeamSelectServerRequestRegulationFrame::APP_PARAM::_tag_in_param_* pInParam  = &m_serverRequestRegulationFrameAppParam.in;
        pInParam->pUIViewPool           = m_pUIViewPool;
        pInParam->pServerRequestClient  = m_pServerRequestClient;

        GFL_ASSERT( pSelectPokePartyOutParam->selectTeamIdx < m_teamNum );
        pInParam->serverRequestClientParam.pPokeParty   = m_pAppParam->in.aTeamData[ pSelectPokePartyOutParam->selectTeamIdx ].pPokeParty->GetPokeParty();

        if( m_pAppParam->in.pRegulationDrawInfo != NULL )
        {
          pInParam->serverRequestClientParam.pRegulation  = &m_pAppParam->in.pRegulationDrawInfo->GetRegulation();
        }
        else
        {
          pInParam->serverRequestClientParam.pRegulation  = NULL;
        }
      }

      //  パラメータを設定して初期化
      pFrame->Initialize( &m_serverRequestRegulationFrameAppParam );

      ++seq;
    }
    //  レギュレーション確認待ち
    if( seq == UPDATE_SEQ_CHK_REQULATION_WAIT )
    {
      //  処理が終了するまで待つ
      if( m_serverRequestRegulationFrameAppParam.out.bEnd == true )
      {
        ++seq;
      }
    }
    //  レギュレーション確認終了
    if( seq == UPDATE_SEQ_CHK_REQULATION_END )
    {
      //  関連するフレームしかないのが前提
      if( m_pFrameManager->End() == false )
      {
        TeamSelectServerRequestRegulationFrame::RESULT_SERVER_DATA* pResultData = &m_serverRequestRegulationFrameAppParam.out.resultServerData;

        //  成功
        if( pResultData->type == TeamSelectServerRequestRegulationFrame::SERVER_RESULT_TYPE_SUCCESS )
        {
          //  チーム設定をする
          m_pAppParam->out.procResultType = PROC_RESULT_TYPE_TEAM_SET;

          //  チーム結果を設定
          {
            struct TeamSelectTopPokePartySelectFrame::APP_PARAM::_tag_out_* pSelectPokePartyOutParam  = &m_topPokePartySelectFrameAppParam.out;
            m_pAppParam->out.selectTeamIdx  = pSelectPokePartyOutParam->selectTeamIdx;
          }

          seq = _SetSeq( UPDATE_SEQ_END );
        }
        //  レギュレーション違反
        else
        if( pResultData->type == TeamSelectServerRequestRegulationFrame::SERVER_RESULT_TYPE_REGULATION_NG )
        {
          //  レギュレーション違反へ繊維
          seq = _SetSeq( UPDATE_SEQ_VIOLATION_REQULATION_START );
        }
        //  ポケモン不正
        else
        if( pResultData->type == TeamSelectServerRequestRegulationFrame::SERVER_RESULT_TYPE_POKE_ERROR )
        {
          //  ポケモンパーティ選択にもどる
          seq = _SetSeq( UPDATE_SEQ_TOP_START );
        }
        //  エラー
        else
        if( pResultData->type == TeamSelectServerRequestRegulationFrame::SERVER_RESULT_TYPE_ERROR )
        {
          //  ポケモンパーティ選択にもどる
          seq = _SetSeq( UPDATE_SEQ_TOP_START );
        }
        else
        {
          //  論理エラー
          GFL_ASSERT( 0 );
        }
      }
    }
    //  レギュレーション違反確認画面開始
    if( seq == UPDATE_SEQ_VIOLATION_REQULATION_START )
    {
      //  フレーム生成
      TeamSelectErrorRegulationFrame*  pFrame  = MENU_FRAME_CALL_MODAL<applib::frame::Manager, TeamSelectErrorRegulationFrame>( m_pFrameManager );

      //  必要なパラメータを設定
      {
        m_errorRegulationFrameAppParam.Reset();
        m_errorRegulationFrameAppParam.in.pUIViewPool = m_pUIViewPool;
        m_errorRegulationFrameAppParam.in.bOpenRegulationButton = m_pAppParam->in.bOpenRegulationButton;

        //  違反データを設定
        {
          TeamSelectServerRequestRegulationFrame::APP_PARAM::_tag_out_param_* pRequestRequlationAppParamOutput  = &m_serverRequestRegulationFrameAppParam.out;
          m_errorRegulationFrameAppParam.in.NGTypeNum   = pRequestRequlationAppParamOutput->resultServerData.data.regulationNG.NGTypeNum;
          gfl2::std::MemCopy( pRequestRequlationAppParamOutput->resultServerData.data.regulationNG.aResultList, m_errorRegulationFrameAppParam.in.aResultList, sizeof( m_errorRegulationFrameAppParam.in.aResultList ) );
        }
      }

      //  パラメータを設定して初期化
      pFrame->Initialize( &m_errorRegulationFrameAppParam );

      ++seq;
    }
    //  レギュレーション違反確認画面待ち
    if( seq == UPDATE_SEQ_VIOLATION_REQULATION_WAIT )
    {
      if( pLowRootView->IsPushBackBtn() == true )
      {
        //  キャンセル
        ++seq;
      }
      else
      if( m_errorRegulationFrameAppParam.out.bEnd == true )
      {
        ++seq;
      }
    }
    //  レギュレーション違反確認画面終了
    if( seq == UPDATE_SEQ_VIOLATION_REQULATION_END )
    {
      if( m_pFrameManager->End() == false )
      {
        if( m_errorRegulationFrameAppParam.out.push == TeamSelectErrorRegulationFrame::PUSH_CANCEL )
        {
          //  ポケモンパーティ選択にもどる
          seq = _SetSeq( UPDATE_SEQ_TOP_START );
        }
        else
        if( m_errorRegulationFrameAppParam.out.push == TeamSelectErrorRegulationFrame::PUSH_CHK_REGULATION )
        {
          //  レギュレーション表示アプリへ遷移
          m_pAppParam->out.procResultType = PROC_RESULT_TYPE_NEXT_REQULATION_CHK_SCENE;
          seq = _SetSeq( UPDATE_SEQ_END );
        }
      }
    }
     //  エラーメッセージ処理開始
    if( seq == UPDATE_SEQ_ERROR_MSG_START )
    {
      ++seq;
    }
    //  エラーメッセージ処理待ち
    if( seq == UPDATE_SEQ_ERROR_MSG_WAIT )
    {
      ++seq;
    }
    //  エラーメッセージ処理終了
    if( seq == UPDATE_SEQ_ERROR_MSG_END )
    {
      ++seq;
    }
    //  自身で終了させる
    //  ここにくる条件
    //    通信エラーで最後まで正常動作して終了した場合は自身で消す
    //    トップ画面からバッグに戻る
    if( seq == UPDATE_SEQ_END )
    {
      //  フェードアウトで画面を暗転
      if( m_subSeq == 0 )
      {
        gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
        pFadeManager->RequestDefaultOut( gfl2::Fade::DISP_DOUBLE );

        ++m_subSeq;
      }

      if( m_subSeq == 1 )
      {
        gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
        if( pFadeManager->IsEnd( gfl2::Fade::DISP_DOUBLE ) == true )
        {
          return gfl2::proc::RES_FINISH;
        }
      }
    }

    m_pFrameManager->Main();

    GameProc::SetSubSeq( seq );

    //  View更新
    {
      pLowRootView->UpdateTree();
      pUppRootView->UpdateTree();
    }

    m_pRenderingManager->Update();

    return gfl2::proc::RES_CONTINUE;
  }

  void TeamSelectProc::PreDrawFunc( gfl2::proc::Manager* /*pManager*/ )
  {
    m_pRenderingManager->PreDraw();
  }

  void TeamSelectProc::DrawFunc( gfl2::proc::Manager* /*pManager*/, gfl2::gfx::CtrDisplayNo displayNo )
  {
    //  上下のルートViewを取得
    TeamSelectLowRootView*  pLowRootView  = m_pUIViewPool->GetLowRootView();
    TeamSelectUppRootView*  pUppRootView  = m_pUIViewPool->GetUppRootView();

    pLowRootView->DrawTree( displayNo );
    pUppRootView->DrawTree( displayNo );

    m_pFrameManager->Draw( displayNo );
    m_pRenderingManager->Draw( displayNo );
  }

  gfl2::proc::Result TeamSelectProc::EndFunc( gfl2::proc::Manager* /*pManager*/ )
  {
    int seq = GameProc::GetSubSeq();

    enum
    {
      SEQ_FADE_OUT = 0,
      SEQ_FRAME_END,
      SEQ_OBJ_DELETE,
      SEQ_OBJ_IS_DELETE,
      SEQ_CORE_DELETE,
      SEQ_END
    };

    //  フェードアウト強制状態
    //  なぜなら通信エラーでいきなりフェードアウトする間もなく終る可能性があるから
    if( seq == SEQ_FADE_OUT )
    {
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      pFadeManager->ForceDefaultOut( gfl2::Fade::DISP_DOUBLE );

      ++seq;
    }
    else
    //  フレームを破棄する
    if( seq == SEQ_FRAME_END )
    {
      // 終了処理が完了するまで待つ
      if( m_pFrameManager->End() == false )
      {
        ++seq;
      }
    }
    else
    //  UIVIEWを破棄する
    if( seq == SEQ_OBJ_DELETE )
    {
      // 終了処理が完了するまで待つ
      m_pUIViewPool->Terminate();

      //  ポケモンアイコンを閉じる
      if( m_pPokeIconResManager != NULL )
      {
        m_pPokeIconResManager->FileClose( m_pUtilheap->GetDeviceHeap() );
      }

      ++seq;
    }
    else
    //  破棄終了まで待つ
    if( seq == SEQ_OBJ_IS_DELETE )
    {
      bool  bEnd  = true;

      if( m_pUIViewPool->IsTerminate() == false )
      {
        bEnd  = false;
      }

      if( m_pPokeIconResManager != NULL )
      {
        if( m_pPokeIconResManager->IsFileClose() == false )
        {
          bEnd  = false;
        }
        else
        if( m_pPokeIconResManager->IsModuleFree() == false )
        {
          bEnd  = false;
        }
      }

      if( bEnd == true )
      {
        ++seq;
      }
    }
    else
    if( seq == SEQ_CORE_DELETE )
    {
      //  ロードしたフォントを開放
      print::SystemFont_ResetDefaultFontInfo();
      print::SystemFont_DeleteNumFont();

      GFL_SAFE_DELETE( m_pUIViewPool );
      GFL_SAFE_DELETE( m_pLayoutFileCtrlState );

      m_pMenuCursor->DeleteResource();

      GFL_SAFE_DELETE( m_pServerRequestClient );
      
      GFL_ASSERT( m_pRenderingManager->CanDestroy() );  // 同期生成なので m_pRenderingManager->CanDestroy() では待たない。
      GFL_SAFE_DELETE( m_pRenderingManager );

      GFL_SAFE_DELETE( m_pFrameManager );
      GFL_SAFE_DELETE( m_pMenuCursor );
      GFL_SAFE_DELETE( m_pWordSet );
      GFL_SAFE_DELETE( m_pAppMsgData );
      GFL_SAFE_DELETE( m_pPokeIconResManager );

      GFL_SAFE_DELETE_ARRAY( m_pTeamViewParamList );

      GFL_DELETE_HEAP( m_pFrameManagerHeap );
      
      GFL_SAFE_DELETE( m_pUtilheap );

      if( m_pDevMem )
      {
        GFL_DELETE_HEAP(m_pDevMem);
        m_pDevMem = NULL;
      }

      if( m_pHeapMem )
      {
        GFL_DELETE_HEAP(m_pHeapMem);
        m_pHeapMem = NULL;
      }

      // @fix GFNMCat[2415] ボタンアニメ中は入力封鎖を設定を元に戻す
      app::ui::UIViewManager* pUIViewMan = GFL_SINGLETON_INSTANCE( app::ui::UIViewManager );
      pUIViewMan->SetInputExclusiveKeyByButtonAnimation( m_bOldInputExclusiveKey );

      return gfl2::proc::RES_FINISH;
    }

    GameProc::SetSubSeq( seq );

    return gfl2::proc::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
    * @brief   シーケンス変更
    */
  //------------------------------------------------------------------
  u32 TeamSelectProc::_SetSeq( const u32 seq )
  {
    m_subSeq  = 0;
    return  seq;
  }

  //------------------------------------------------------------------
  /**
    * @brief   すでに選択決定が予約しているチームデータのlistIdxを取得
    * @return  選択決定チームデータidx / ない場合は SELECT_TEAM_DATA_IDX_ERROR
    */
  //------------------------------------------------------------------
  u32 TeamSelectProc::_GetAppintEnterTeamDataIdx()
  {
    if( 0 <= m_pAppParam->in.appointEnterTeamListIdx )
    {
      if( m_pAppParam->in.appointEnterTeamListIdx < (s32)m_teamNum )
      {
        return  (u32)m_pAppParam->in.appointEnterTeamListIdx;
      }
    }

    return  SELECT_TEAM_DATA_IDX_ERROR;
  }

  //  チームデータがあるか
  bool TeamSelectProc::_IsTeamData()
  {
    //  QR項目しかないかチェック
    {
      u32 qrItemNum = 0;
      for( u32 i = 0; i < m_teamNum; ++i )
      {
        if( m_pAppParam->in.aTeamData[ i ].status == TEAM_STATUS_QR_READ_REQUEST )
        {
          //  QR項目だがQRポケモンパーティをロードしている場合はQR扱いにはしない
          if( m_pAppParam->in.aTeamData[ i ].bQRReaderData == false )
          {
            ++qrItemNum;
          }
        }
      }

      //  QR項目しかないのでチーム選択はしない
      if( qrItemNum == m_teamNum )
      {
        return  false;
      }
    }  
    
    return  true;  
  }

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)

//======================================================================
/**
 * @file TrainerPhotoProc.cpp
 * @date 2015/08/31 18:12:20
 * @author uchida_yuto
 * @brief 証明写真アプリProc
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include <macro/include/gfl2_Macros.h>

#include "App/TrainerPhoto/include/TrainerPhotoProc.h"
#include "App/TrainerPhoto/source/TrainerPhotoUIViewPool.h"

//  gflib2のインクルード
#include <Fade/include/gfl2_FadeManager.h>

// nijiのインクルード
#include "System/include/ArcIdGetter.h"
#include "System/include/HeapDefine.h"
#include "System/include/nijiAllocator.h"
#include "System/include/DressUp.h"
#include "AppLib/include/Frame/AppFrameManager.h"

#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/AppUtilFileControlState.h"

#include "AppLib/include/Tool/AppToolTrainerIconRendering.h"
#include "AppLib/include/Tool/AppToolCharaSimpleModel.h"
#include "GameSys/include/GameData.h"

// モデルインデックス
#include <niji_conv_header/battle/chara/chara_model_id.h>

//  リソースのインクルード
// 動的ヘッダシステムを利用するときは以下のヘッダをインクルードする
#include <debug/include/DynamicHeader/gfl2_DebugDynamicHeader.h>

//  アプリ全体で使う動的パラメータ
#define NIJI_DEBUG_TRAINER_PHOTO_MODEL_VIEWER_HEADER_FILEPATH "niji_conv_header/app/id_photo/TrainerPhotoModelDynamic.h"

//  モデルビューアの動的パラメータ
#define NIJI_DEBUG_VIEWR_SEQ_TRAINER_PHOTO_MODEL_VIEWER_HEADER_FILEPATH "niji_conv_header/app/dynamicHeader/AppModelViewrDynamic.h"

// 動的ヘッダシステムで利用するヘッダは以下のマクロを通してincludeしてください
#include GFL_DYNAMIC_HEADER(NIJI_DEBUG_TRAINER_PHOTO_MODEL_VIEWER_HEADER_FILEPATH)
#include GFL_DYNAMIC_HEADER(NIJI_DEBUG_VIEWR_SEQ_TRAINER_PHOTO_MODEL_VIEWER_HEADER_FILEPATH)

#include "arc_def_index/arc_def.h"
#include "arc_index/idPhotoLyt.gaix"
#include "arc_index/message.gaix"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(photo)

  TrainerPhotoProc::TrainerPhotoProc() :
  GameProc()
  {
    _Clear();
  }

  TrainerPhotoProc::~TrainerPhotoProc()
  {}

  gfl2::proc::Result TrainerPhotoProc::InitFunc( gfl2::proc::Manager* pManager )
  {
    static const s32 HEAP_MEM_SIZE = 0xf0000 + app::tool::AppToolTrainerIconRendering::GetUseSystemHeapSize( 1 );
    static const s32 DEV_MEM_SIZE  = 1024 * 1024 * 5 + app::tool::AppToolTrainerIconRendering::GetUseDeviceHeapSize( 1 ) + app::tool::CharaSimpleModel::USE_FULL_HEAP_SIZE;

    static const s32 LOCAL_HEAP_SIZE = HEAP_MEM_SIZE - 1024;
    static const s32 LOCAL_DEVICE_HEAP_SIZE  = DEV_MEM_SIZE - 1024;

    enum
    {
      SEQ_RES_CREATE = 0,
      SEQ_RES_CREATE_WAIT,
      SEQ_VIEW_CREATE,
      SEQ_VIEW_CREAET_WAIT
    };

    s32 seq = GameSys::GameProc::GetSubSeq();

    if( seq == SEQ_RES_CREATE )
    {
      // ヘッダファイルのロード
      // ファイルパスと同時にテーブルのキャパシティ(最大定義数)も指定する必要がある
      GFL_LOAD_HEADER_HASH(NIJI_DEBUG_TRAINER_PHOTO_MODEL_VIEWER_HEADER_FILEPATH, 64);
      GFL_LOAD_HEADER_HASH(NIJI_DEBUG_VIEWR_SEQ_TRAINER_PHOTO_MODEL_VIEWER_HEADER_FILEPATH, 64);

      //  全体が使用するメモリを確保
      {
        gfl2::heap::HeapBase* parentHeapMem = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
        m_pHeapMem = GFL_CREATE_LOCAL_HEAP( parentHeapMem, HEAP_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
  
        gfl2::heap::HeapBase* parentDevMem = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
        m_pDevMem = GFL_CREATE_LOCAL_HEAP( parentDevMem, DEV_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
      }

      // ローカルヒープ生成
      {
        m_pUtilHeap = GFL_NEW( m_pDevMem ) app::util::Heap();
        m_pUtilHeap->LocalHeapCreate( m_pHeapMem, m_pDevMem, LOCAL_HEAP_SIZE, LOCAL_DEVICE_HEAP_SIZE );
      }

      // パイプライン生成
      {
        m_pNijiAllocatorForEdgeMapTexture = GFL_NEW( m_pUtilHeap->GetDeviceHeap() ) System::nijiAllocator( m_pUtilHeap->GetDeviceHeap() );

        app::util::AppRenderingManager::Description appRenderingManagerDesc;
        //  下画面はレイアウト描画
        appRenderingManagerDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;

        //  上画面はレイアウト->モデル->レイアウト描画
        appRenderingManagerDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder  = app::util::AppRenderingManager::RenderOrderType::LAYOUT0_POKEMODEL_LAYOUT1;

        //  トレーナーアイコンを表示するためのターゲットレンダーテクスチャを扱うための設定
        appRenderingManagerDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN].renderOrder     = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;

        m_pRenderingManager = GFL_NEW( m_pUtilHeap->GetDeviceHeap() ) app::util::AppRenderingManager();

        m_pRenderingManager->StartAsyncCreate(
            appRenderingManagerDesc,
            m_pUtilHeap,
            m_pNijiAllocatorForEdgeMapTexture,
            GameSys::GameProc::GetGameManager()->GetAsyncFileManager()
        );
      }

      //  モデル表示に使うカメラ作成
      {
        app::util::AppRenderingManager::CreateDefaultCamera( m_pUtilHeap, &m_pCharaModelDrawEnv, &m_pCharaModelCamera );
      }

      //  表示するキャラ作成
      //  トレーナーアイコン描画クラス「app::tool::AppToolTrainerIconRendering」に合わせたモデルデータをロードする
      {
        m_pCharaModel                   = GFL_NEW( m_pUtilHeap->GetDeviceHeap() ) app::tool::CharaSimpleModel();

        //  プレイヤーのトレーナー情報を取得してモデル表示
        Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();

        // 性別に合わせたトレーナーidを取得
        m_trainerId = app::tool::AppToolTrainerIconRendering::GetHeroTrainerId( pMyStatus->GetSex() );

        //  トレーナーidからキャラモデルidを取得
        u32 modelCharaId  = app::tool::AppToolTrainerIconRendering::ConvTrainerIdToModelResID( m_trainerId );

        app::tool::CharaSimpleModel::SETUP_PARAM  setupParam;
        {
          setupParam.aRenderPlaceType[ 0 ]  = app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY;
          setupParam.pRenderingManager      = m_pRenderingManager;
          setupParam.pUseHeap               = m_pUtilHeap->GetDeviceHeap();
        }

        m_pCharaModel->Setup( setupParam, GameSys::GameProc::GetGameManager()->GetAsyncFileManager(), app::tool::AppToolTrainerIconRendering::GetTrainerModelArcId(), System::DressUp::GetDressUpArcIdListBattle() );

        m_pCharaModel->StartLoad( modelCharaId, pMyStatus->GetDressup() );
        m_pCharaModel->SetMotionAnime( app::tool::AppToolTrainerIconRendering::ConvTrainerIdToModelWaitMotionAnimeId( m_trainerId ), 0, true );
        m_pCharaModel->PlayEyeAnime();
        m_pCharaModel->SetVisible( false );
      }

      //  フレーム管理生成
      {
        m_pFrameManager = GFL_NEW( m_pUtilHeap->GetDeviceHeap() ) applib::frame::Manager( m_pUtilHeap->GetDeviceHeap() );
      }

      //  レイアウトファイル制御作成
      {
        m_pLytFileCtrlState = GFL_NEW( m_pUtilHeap->GetDeviceHeap() ) app::util::AppUtilFileControlState( NIJI_ARCID_NUM( ARCID_TRAINER_PHOTO_LYT_DATA ), GARC_idPhotoLyt_idPhotoLyt_applyt_COMP, m_pUtilHeap->GetDeviceHeap(), true, 128 );
        //  ロード開始
        m_pLytFileCtrlState->Start();
      }

      //  UI画面で使う共通メッセージ
      {
        //  黒画面中なので同期読みでいいや
        m_pAppMsgData = GFL_NEW( m_pUtilHeap->GetDeviceHeap() ) gfl2::str::MsgData( print::GetMessageArcId( System::MSGLANGID_MAX ), GARC_message_id_photo_DAT, m_pUtilHeap->GetDeviceHeap(), gfl2::str::MsgData::LOAD_FULL );
        GFL_ASSERT( m_pAppMsgData );
      }

      //  UIで使用するwordset
      {
        m_pAppWordSet = GFL_NEW( m_pUtilHeap->GetDeviceHeap() ) print::WordSet( print::WordSet::DEFAULT_WORD_MAX, print::WordSet::DEFAULT_BUF_LEN , m_pUtilHeap->GetDeviceHeap() );
        GFL_ASSERT( m_pAppWordSet );
      }

      ++seq;
    }
    else
    if( seq == SEQ_RES_CREATE_WAIT )
    {
      bool  bCreate = true;

      m_pRenderingManager->Update();
      if( m_pRenderingManager->IsCreated() == false )
      {
        bCreate = false;
      }

      if( m_pCharaModel->IsReady() == false )
      {
        bCreate = false;
      }

      m_pLytFileCtrlState->Update();
      if( m_pLytFileCtrlState->IsEnd() == false )
      {
        bCreate = false;
      }

      if( bCreate == true )
      {
        ++seq;
      }
    }
    else
    if( seq == SEQ_VIEW_CREATE )
    {
      //  トレーナー顔描画生成
      {
        m_pTrainerIconRendering = GFL_NEW( m_pUtilHeap->GetDeviceHeap() ) app::tool::AppToolTrainerIconRendering();
        m_pTrainerIconRendering->Initialize( m_pRenderingManager, m_pUtilHeap, GetGameManager()->GetAsyncFileManager(), 1, app::util::G2DUtil::SETUP_UPPER );
      }

      //  キャラモデル用のライト設定
      {
        m_pCharaModelLightDrawEnv = app::util::AppRenderingManager::CreateDefaultDrawEnv( m_pUtilHeap, app::util::AppRenderingManager::PresetType::TRAINER );        
        m_pRenderingManager->AddDrawEnv( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pCharaModelLightDrawEnv );
      }

      //  アプリで使うUIViewプールを作成
      {
        TrainerPhotoUIViewPool::INIT_PARAM  initParam;
        {
          initParam.pUtilHeap       = m_pUtilHeap;
          initParam.pLytBinData     = m_pLytFileCtrlState->GetLoadDataBuffer();
          initParam.pAppMsgData     = m_pAppMsgData;
          initParam.pWordSet        = m_pAppWordSet;
          initParam.pRenderManager  = m_pRenderingManager;
        }

        m_pUIViewPool = GFL_NEW( m_pUtilHeap->GetDeviceHeap() ) TrainerPhotoUIViewPool( initParam );
      }

      ++seq;
    }
    else
    if( seq == SEQ_VIEW_CREAET_WAIT )
    {
      bool  bCreate = true;
      if( m_pTrainerIconRendering->IsReady() == false )
      {
        bCreate = false;
      }

      if( m_pUIViewPool->InitFunc() == false )
      {
        bCreate = false;
      }

      if( bCreate == true )
      {
        //  3Dモデルのカメラ設定
        {
          const app::tool::CharaSimpleModel::SETUP_PARAM& rModelSetupParam  = m_pCharaModel->GetSetupParam();
          u32 loopNum = GFL_NELEMS( rModelSetupParam.aRenderPlaceType );
          for( u32 i = 0; i < loopNum; ++i )
          {
            if( rModelSetupParam.aRenderPlaceType[ i ] != app::util::AppRenderingManager::RenderPlaceType::NUM )
            {
              rModelSetupParam.pRenderingManager->SetCamera( rModelSetupParam.aRenderPlaceType[ i ], m_pCharaModelCamera );
            }
          }
        }

        ++seq;
      }
    }
    else
    {
      return gfl2::proc::RES_FINISH;
    }

    GameSys::GameProc::SetSubSeq( seq );

    return gfl2::proc::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
    * @brief	  動作に必要な引数を登録
    * @note
    *    必ずインスタンス生成の段階で呼び出さないと後の動作保障はなし
    */
  //------------------------------------------------------------------
  void TrainerPhotoProc::Initialize( APP_PARAM* pAppParam )
  {
    GFL_ASSERT( pAppParam );
    m_pAppParam = pAppParam;
  }

  gfl2::proc::Result TrainerPhotoProc::UpdateFunc( gfl2::proc::Manager* /*pManager*/ )
  {
    enum
    {
      TRAINER_PHOTO_PRCC_SEQ_FADE_IN  = 0,
      TRAINER_PHOTO_PRCC_SEQ_FADE_IN_WAIT,
      TRAINER_PHOTO_PRCC_SEQ_IDLE,
      TRAINER_PHOTO_PRCC_SEQ_FADE_OUT,
      TRAINER_PHOTO_PRCC_SEQ_FADE_OUT_WAIT
    };

    app::ui::UIView*  pRootUppView = m_pUIViewPool->GetRootUppView();
    app::ui::UIView*  pRootLowView = m_pUIViewPool->GetRootLowView();

    s32 seq = GameSys::GameProc::GetSubSeq();

    if( seq == TRAINER_PHOTO_PRCC_SEQ_FADE_IN )
    {
      if( m_pFrameManager->GetBaseProcessInitialized() == true )
      {
        //  フレームの初期化処理が終了して更新に入った段階で描画もしているので
        //  フェードイン開始
        gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
        pFadeManager->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA );

        ++seq;
      }
    }

    if( seq == TRAINER_PHOTO_PRCC_SEQ_FADE_IN_WAIT )
    {
      //  フェードイン待ち
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      if( pFadeManager->IsEnd( gfl2::Fade::DISP_DOUBLE ) == true )
      {
        ++seq;
      }
    }

    if( m_pFrameManager->Main() == applib::frame::MAINSTATUS_NULL )
    {
      if( m_pNowFrameCell != NULL )
      {
        m_pNowFrameCell = NULL;

        //  すがたフレームが終了した
        if( m_nowFrame == FRAME_MODEL_VIEWER )
        {
          //  撮影ボタンを押したら撮影フレームに
          if( m_modelViewrFrameParam.out.pushBtn == TrainerPhotoModelViewerFrame::PUSH_BTN_MODEL_SHOOT )
          {
            m_bDefaultShootViewerParam  = true;
            m_nowFrame = FRAME_MODEL_SHOOT;
          }
          else
          if( m_modelViewrFrameParam.out.pushBtn == TrainerPhotoModelViewerFrame::PUSH_BTN_CANCEL )
          {
            //  キャンセルボタンを押したらproc終了
            m_pAppParam->out_param.ret  = PROC_RET_SHOOT_CANCEL;
            m_nowFrame  = FRAME_END;
          }
          else
          {
            GFL_ASSERT( 0 );
            m_nowFrame  = FRAME_END;
          }
        }
        else 
        //  写真撮影フレームが終了した
        if( m_nowFrame == FRAME_MODEL_SHOOT )
        {
          if( m_modelShootFrameParam.out.pushBtn == TrainerPhotoModelShootFrame::PUSH_BTN_CANCEL )
          {
            //  ビューアーフレームに戻る
            m_nowFrame  = FRAME_MODEL_VIEWER;
          }
          else
          if( m_modelShootFrameParam.out.pushBtn == TrainerPhotoModelShootFrame::PUSH_BTN_MODEL_SHOOT_RUN )
          {
            //  撮影したので確認フレームへ
            m_nowFrame  = FRAME_SHOOT_CONFIRM;
          }
          else
          {
            GFL_ASSERT( 0 );
            m_nowFrame  = FRAME_END;
          }
        }
        else
        //  撮影写真確認フレーム終了した
        if( m_nowFrame == FRAME_SHOOT_CONFIRM )
        {
          if( m_shootConfirmFrameParam.out.pushBtn == TrainerPhotoShootConfirmFrame::PUSH_BTN_CANCEL )
          {
            //  撮影フレームに遷移
            //  この時カメラの位置は現在のカメラ位置をそのまま使う
            m_bDefaultShootViewerParam  = false;

            m_nowFrame  = FRAME_MODEL_SHOOT;
          }
          else
          if( m_shootConfirmFrameParam.out.pushBtn == TrainerPhotoShootConfirmFrame::PUSH_BTN_SHOOT_SAVE )
          {
            //  終了
            m_nowFrame  = FRAME_END;
          }
          else
          {
            GFL_ASSERT( 0 );
            m_nowFrame  = FRAME_END;
          }
        }
      }

      //  フレーム作成
      if( m_pNowFrameCell == NULL )
      {
        //  すがたフレーム作成
        if( m_nowFrame == FRAME_MODEL_VIEWER )
        {
          m_pNowFrameCell = _CreateFrame( m_nowFrame );
          GFL_ASSERT( m_pNowFrameCell != NULL );
        }
        else
        //  写真撮影フレーム作成
        if( m_nowFrame == FRAME_MODEL_SHOOT )
        {
          m_pNowFrameCell = _CreateFrame( m_nowFrame );
          GFL_ASSERT( m_pNowFrameCell != NULL );
        }
        else
        //  撮影写真確認フレーム作成
        if( m_nowFrame == FRAME_SHOOT_CONFIRM )
        {
          m_pNowFrameCell = _CreateFrame( m_nowFrame );
          GFL_ASSERT( m_pNowFrameCell != NULL );
        }
      }

      if( m_pNowFrameCell == NULL )
      {
        //  一覧のフレームによるシーケンス処理は終わったのでフェードアウトしてアプリ終了させる
        if( seq == TRAINER_PHOTO_PRCC_SEQ_IDLE )
        {
          seq = TRAINER_PHOTO_PRCC_SEQ_FADE_OUT;
        }
      }
    }

    if( seq == TRAINER_PHOTO_PRCC_SEQ_FADE_OUT )
    {
      //  フェードアウト開始
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      pFadeManager->RequestDefaultOut( gfl2::Fade::DISP_DOUBLE );

      ++seq;
    }

    if( seq == TRAINER_PHOTO_PRCC_SEQ_FADE_OUT_WAIT )
    {
      //  フェードアウト待ち
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      if( pFadeManager->IsEnd( gfl2::Fade::DISP_DOUBLE ) == true )
      {
        return gfl2::proc::RES_FINISH;
      }
    }

    m_pTrainerIconRendering->Update();
    m_pCharaModel->Update();

    pRootUppView->UpdateTree();
    pRootLowView->UpdateTree();

    GameProc::SetSubSeq( seq );

    m_pRenderingManager->Update();

    return gfl2::proc::RES_CONTINUE;
  }

  void TrainerPhotoProc::PreDrawFunc( gfl2::proc::Manager* /*pManager*/ )
  {
    m_pRenderingManager->PreDraw();
  }

  void TrainerPhotoProc::DrawFunc( gfl2::proc::Manager* /*pManager*/, gfl2::gfx::CtrDisplayNo displayNo )
  {
    app::ui::UIView*  pRootUppView = m_pUIViewPool->GetRootUppView();
    app::ui::UIView*  pRootLowView = m_pUIViewPool->GetRootLowView();

    pRootUppView->DrawTree( displayNo );
    pRootLowView->DrawTree( displayNo );

    m_pFrameManager->Draw( displayNo );

    m_pTrainerIconRendering->Draw( displayNo );
    m_pRenderingManager->Draw( displayNo );
  }

  gfl2::proc::Result TrainerPhotoProc::EndFunc( gfl2::proc::Manager* /*pManager*/ )
  {
    enum
    {
      SEQ_FADE_OUT  = 0,
      SEQ_DATA_SAVE,
      SEQ_UNSET_CAM,
      SEQ_FRAME_END,
      SEQ_UIVIEW_POOL_END_FUNC,
      SEQ_CHARA_MODEL_TERMINATE,
      SEQ_CHARA_MODEL_WAIT_DELETE,
      SEQ_TRAINER_TERMINATE,
      SEQ_TRAINER_WAIT_DELETE,
      SEQ_RENDERING_DESTROY,
      SEQ_END
    };

    s32 seq = GameSys::GameProc::GetSubSeq();
    if( seq == SEQ_FADE_OUT )
    {
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      pFadeManager->ForceDefaultOut( gfl2::Fade::DISP_DOUBLE );
      ++seq;
    }
    else
    if( seq == SEQ_DATA_SAVE )
    {
      //  証明写真のデータセーブするか
      if( m_shootConfirmFrameParam.out.pushBtn == TrainerPhotoShootConfirmFrame::PUSH_BTN_SHOOT_SAVE )
      {
        //  写真情報をセーブする
        Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
        
        //  アプリで扱っているアイコンデータからセーブデータ用に変換して設定
        app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA* pTrainerIconObjectData = m_pTrainerIconRendering->GetIconObjectData( 0 );
        if( pTrainerIconObjectData != NULL )
        {
          Savedata::MyStatus::ICON_DATA iconData;

          pTrainerIconObjectData->ConvToMyStatusIcon( &iconData );

          //  すでに着せ替え設定をしたので着せ替えデフォルトは不要
          iconData.bDressDefault  = false;
          pMyStatus->SetIcon( iconData );
        }
        else
        {
          GFL_ASSERT( 0 );
        }
      }

      ++seq;
    }
    else
    if( seq == SEQ_UNSET_CAM )
    {
      //  3Dモデルのカメラ設定
      {
        const app::tool::CharaSimpleModel::SETUP_PARAM& rModelSetupParam  = m_pCharaModel->GetSetupParam();
        u32 loopNum = GFL_NELEMS( rModelSetupParam.aRenderPlaceType );
        for( u32 i = 0; i < loopNum; ++i )
        {
          if( rModelSetupParam.aRenderPlaceType[ i ] != app::util::AppRenderingManager::RenderPlaceType::NUM )
          {
            rModelSetupParam.pRenderingManager->UnsetCamera( rModelSetupParam.aRenderPlaceType[ i ] );
          }
        }
      }

      ++seq;
    }
    else
    if( seq == SEQ_FRAME_END )
    {
      // 終了処理が完了するまで待つ
      if( m_pFrameManager->End() == false )
      {
        ++seq;
      }
    }
    else
    if( seq == SEQ_UIVIEW_POOL_END_FUNC )
    {
      if( m_pUIViewPool->EndFunc() == true )
      {
        ++seq;
      }
    }
    if( seq == SEQ_CHARA_MODEL_TERMINATE )
    {
      m_pCharaModel->Terminate();
      ++seq;
    }
    else
    if( seq == SEQ_CHARA_MODEL_WAIT_DELETE )
    {
      if( m_pCharaModel->IsDelete() == true )
      {
        ++seq;
      }
    }
    else
    if( seq == SEQ_TRAINER_TERMINATE )
    {
       m_pTrainerIconRendering->Terminate();
       ++seq;
    }
    else
    if( seq == SEQ_TRAINER_WAIT_DELETE )
    {
      if( m_pTrainerIconRendering->IsDelete() == true )
      {
        ++seq;
      }
    }
    else
    if( seq == SEQ_RENDERING_DESTROY )
    {
      if( m_pRenderingManager->CanDestroy() == true )
      {
        ++seq;
      }
    }
    else
    if( seq == SEQ_END )
    {
      //  ヘッダーファイルのアンロード
      GFL_UNLOAD_HEADER_HASH(NIJI_DEBUG_VIEWR_SEQ_TRAINER_PHOTO_MODEL_VIEWER_HEADER_FILEPATH);
      GFL_UNLOAD_HEADER_HASH(NIJI_DEBUG_TRAINER_PHOTO_MODEL_VIEWER_HEADER_FILEPATH);

      GFL_SAFE_DELETE( m_pUIViewPool );
      GFL_SAFE_DELETE( m_pFrameManager );

      GFL_SAFE_DELETE( m_pLytFileCtrlState );

      GFL_SAFE_DELETE( m_pCharaModel );
      GFL_SAFE_DELETE( m_pCharaModelDrawEnv );
      GFL_SAFE_DELETE( m_pCharaModelLightDrawEnv );
      GFL_SAFE_DELETE( m_pCharaModelCamera );

      GFL_SAFE_DELETE( m_pAppMsgData );
      GFL_SAFE_DELETE( m_pAppWordSet );

      GFL_SAFE_DELETE( m_pTrainerIconRendering );
      GFL_SAFE_DELETE( m_pRenderingManager );
      GFL_SAFE_DELETE( m_pNijiAllocatorForEdgeMapTexture );
      GFL_SAFE_DELETE( m_pUtilHeap );

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

      return gfl2::proc::RES_FINISH;
    }

    GameSys::GameProc::SetSubSeq( seq );

    return gfl2::proc::RES_CONTINUE;
  }

  //  フレーム作成
  applib::frame::CellSuper* TrainerPhotoProc::_CreateFrame( const Frame frame )
  {
    if( frame == FRAME_MODEL_VIEWER )
    {
      TrainerPhotoModelViewerFrame* pFrame  = MENU_FRAME_CALL_MODAL< applib::frame::Manager, TrainerPhotoModelViewerFrame >( m_pFrameManager );
      GFL_ASSERT( pFrame != NULL );

      {
        m_modelViewrFrameParam.in.Reset();
        m_modelViewrFrameParam.in.pCharaModel     = m_pCharaModel;
        m_modelViewrFrameParam.in.pCharaModelCam  = m_pCharaModelCamera;
        m_modelViewrFrameParam.in.pUIViewPool     = m_pUIViewPool;
      }

      pFrame->Initialize( &m_modelViewrFrameParam, m_bDefaultViewerParam );

      return  pFrame;
    }
    else
    if( frame == FRAME_MODEL_SHOOT )
    {
      TrainerPhotoModelShootFrame*  pFrame  = MENU_FRAME_CALL_MODAL< applib::frame::Manager, TrainerPhotoModelShootFrame >( m_pFrameManager );
      GFL_ASSERT( pFrame != NULL );
      
      {
        m_modelShootFrameParam.in.Reset();
        m_modelShootFrameParam.in.pCharaModel         = m_pCharaModel;
        m_modelShootFrameParam.in.pCharaModelCam      = m_pCharaModelCamera;
        m_modelShootFrameParam.in.pUIViewPool         = m_pUIViewPool;
        m_modelShootFrameParam.in.continueViewParam   = m_modelShootFrameParam.out.viewParam;
        m_modelShootFrameParam.in.pTrainerIconRender  = m_pTrainerIconRendering;
        m_modelShootFrameParam.in.trainerId           = m_trainerId;

        m_modelShootFrameParam.in.bViewParamContinue  = m_bDefaultShootViewerParam == true ? false : true;
      }

      pFrame->Initialize( &m_modelShootFrameParam );

      return  pFrame;
    }
    else
    if( frame == FRAME_SHOOT_CONFIRM )
    {
      TrainerPhotoShootConfirmFrame*  pFrame  = MENU_FRAME_CALL_MODAL< applib::frame::Manager, TrainerPhotoShootConfirmFrame >( m_pFrameManager );
      GFL_ASSERT( pFrame != NULL );

      {
        m_shootConfirmFrameParam.in.Reset();
        m_shootConfirmFrameParam.in.pCharaModel         = m_pCharaModel;
        m_shootConfirmFrameParam.in.pCharaModelCam      = m_pCharaModelCamera;
        m_shootConfirmFrameParam.in.pUIViewPool         = m_pUIViewPool;
        m_shootConfirmFrameParam.in.pTrainerIconRender  = m_pTrainerIconRendering;
        m_shootConfirmFrameParam.in.trainerId           = m_trainerId;
      }

      pFrame->Initialize( &m_shootConfirmFrameParam );

      return  pFrame;
    }

    return  NULL;
  }

GFL_NAMESPACE_END(photo)
GFL_NAMESPACE_END(app)

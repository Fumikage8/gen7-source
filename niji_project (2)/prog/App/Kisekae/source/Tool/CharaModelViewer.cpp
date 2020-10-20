//======================================================================
/**
 * @file CharaModelViewer.cpp
 * @date 2015/10/13 13:31:14
 * @author uchida_yuto
 * @brief キャラ専用のビューアー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "App/Kisekae/source/Tool/CharaModelViewer.h"
#include "AppLib/include/Tool/TimerIconView.h"

//  gfl2のインクルード
#include <model/include/gfl2_BaseCamera.h>

//  リソースのインクルード
// 動的ヘッダシステムを利用するときは以下のヘッダをインクルードする
#include <debug/include/DynamicHeader/gfl2_DebugDynamicHeader.h>

//  アプリ全体で使う動的パラメータ
#include "niji_conv_header/app/dynamicHeader/AppModelViewrDynamic.h"

//  サウンド
#include <Sound/include/Sound.h>

#define NIJI_DEBUG_APP_MODEL_VIEWER_HEADER_FILEPATH "niji_conv_header/app/dynamicHeader/AppModelViewrDynamic.h"

// 動的ヘッダシステムで利用するヘッダは以下のマクロを通してincludeしてください
#include GFL_DYNAMIC_HEADER(NIJI_DEBUG_APP_MODEL_VIEWER_HEADER_FILEPATH)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  CharaModelViewer::CharaModelViewer( const INIT_DATA& rInitData )
  {
    _Clear();

    m_initData  = rInitData;

    m_pCharaModel = GFL_NEW( m_initData.charaModelSetupParam.pUseHeap ) app::tool::CharaSimpleModel();
    m_pModelViewrSequence = GFL_NEW( m_initData.charaModelSetupParam.pUseHeap ) app::tool::ModelViewerSequence();

    //  タイマーアイコン作成
    {
      m_pTimerIconView  = GFL_NEW( m_initData.charaModelSetupParam.pUseHeap ) 
        app::tool::TimerIconUIView( m_initData.pUtilHeap, m_initData.charaModelSetupParam.pRenderingManager,
                                    m_initData.pTimerIconBin, app::util::G2DUtil::SETUP_UPPER );

      m_pTimerIconView->SetVisible( false );

      gfl2::math::VEC3  pos( 0.0f, 0.0f, 0.0f );
      m_pTimerIconView->SetIconPos( pos );
    }

    //  キャラモデル用のライト設定
    {
      m_pCharaModelLightDrawEnv = app::util::AppRenderingManager::CreateDefaultDrawEnv( rInitData.pUtilHeap, app::util::AppRenderingManager::PresetType::TRAINER );
      rInitData.charaModelSetupParam.pRenderingManager->AddDrawEnv( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pCharaModelLightDrawEnv );
    }
  }

  CharaModelViewer::~CharaModelViewer()
  {
    GFL_SAFE_DELETE( m_pCharaModelLightDrawEnv );
    GFL_SAFE_DELETE( m_pModelViewrSequence );
    GFL_SAFE_DELETE( m_pTimerIconView );
    GFL_SAFE_DELETE( m_pCharaModel );
  }

  //  初期化
  bool CharaModelViewer::InitFunc()
  {
    if( m_initSeq == 0 )
    {
      // ヘッダファイルのロード
      // ファイルパスと同時にテーブルのキャパシティ(最大定義数)も指定する必要がある
      GFL_LOAD_HEADER_HASH(NIJI_DEBUG_APP_MODEL_VIEWER_HEADER_FILEPATH, 64);

      //  モデル作成
      {
        m_pCharaModel->Setup( m_initData.charaModelSetupParam, m_initData.pFileManager, m_initData.charaModelArcId, m_initData.pCharaModelDressupArcIdList );

        m_pCharaModel->StartLoad( m_initData.modelCharaId, m_initData.dressUpParam );
        m_pCharaModel->SetMotionAnime( m_initData.modelCharaAnimWaitId, 0, true );
        m_pCharaModel->PlayEyeAnime();
        m_pCharaModel->SetVisible( false );
        {
          gfl2::math::Vector3 pos(
              GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_KISEKAE_CHARA_POS_X ),
              GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_KISEKAE_CHARA_POS_Y ),
              GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_KISEKAE_CHARA_POS_Z ) );

          m_pCharaModel->SetPos( pos );
        }

        //  デフォルトモデル位置
        m_defaultModelPos = m_pCharaModel->GetPos();

        //  カメラの初回設定
        {
          //  デフォルト設定を確保
          m_defaultCamPos   = 
            gfl2::math::Vector3( 
              GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_KISEKAE_DEFALT_CAM_POS_X ),
              GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_KISEKAE_DEFALT_CAM_POS_Y ),
              GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_KISEKAE_DEFALT_CAM_POS_Z ) );

          m_defaultCamFovy = gfl2::math::ConvDegToRad( GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_KISEKAE_DEFALT_CAM_ZOOM_FAR ) );

          //  デフォルト更新
          _UpdateDefaultViewerParam();
        }
      }

      ++m_initSeq;
    }
    else
    if( m_initSeq == 1 )
    {
      bool  bNext = true;

      if( m_pCharaModel->IsReady() == false )
      {
        bNext  = false;
      }

      if( bNext == true )
      {
        ++m_initSeq;
      }
    }
    else
    if( m_initSeq == 2 )
    {
      bool  bNext = true;
      if( m_pModelViewrSequence->InitFunc() == false )
      {
        bNext = false;
      }

      if( bNext == true )
      {
        //  初回開始
        _StartViewer( 0, 0 );

        ++m_initSeq;
      }
    }
    else
    {
      return  true;
    }

    return false;
  }

  //  終了
  bool CharaModelViewer::EndFunc()
  {
    if( m_endSeq == 0 )
    {
      //  ヘッダーファイルのアンロード
      GFL_UNLOAD_HEADER_HASH(NIJI_DEBUG_APP_MODEL_VIEWER_HEADER_FILEPATH);

      m_pCharaModel->Terminate();

      ++m_endSeq;
    }
    else
    if( m_endSeq == 1 )
    {
      bool  bNext = true;
      if( m_pCharaModel->IsDelete() == false )
      {
        bNext = false;
      }

      if( bNext == true )
      {
        ++m_endSeq;
      }
    }
    else
    if( m_endSeq == 2 )
    {
      bool  bNext = true;
      if( m_pModelViewrSequence->EndFunc() == false )
      {
        bNext = false;
      }

      if( bNext == true )
      {
        ++m_endSeq;
      }
    }
    else
    {
      return  true;
    }

    return false;
  }

  //  更新
  void CharaModelViewer::UpdateFunc()
  {
    if( m_pCharaModel == NULL )
    {
      return;
    }

    //  待機状態
    if( m_state == STATE_IDLE )
    {
    }
    else
    //  回転状態
    if( m_state == STATE_ROT )
    {
      const gfl2::math::Vector3&  rRot  = m_pCharaModel->GetRot();
      gfl2::math::Vector3 newRot  = rRot;
      newRot  += m_addRot;

      m_pCharaModel->SetRot( newRot );
    }
    else
    //  着せ替え中状態
    if( m_state == STATE_DRESS_UP )
    {
      if( m_pCharaModel->IsReady() == true )
      {
        //  待機にする
        StartIdleingEvent();

        //  タイマーアイコンを消す
        m_pTimerIconView->EndAnime();

        //  コールバックを呼ぶ
        if( m_pDelegate != NULL )
        {
          m_pDelegate->CharaModelViewer_OnDressupEnd();
        }
      }
    }
    //  ビューアー状態
    else
    if( m_state == STATE_SHOW )
    {
      m_pModelViewrSequence->UpdateFunc();
    }

    m_pCharaModel->Update();
    m_pTimerIconView->UpdateTree();
  }

  //  描画
  void CharaModelViewer::DrawFunc( gfl2::gfx::CtrDisplayNo displayNo )
  {
    m_pTimerIconView->DrawTree( displayNo );
  }

  //  モデルの着せ替え設定
  bool CharaModelViewer::StartDressupEvent( const poke_3d::model::DressUpParam& rDressUpParam )
  {
    m_pCharaModel->SetVisible( true );

    //  きせかえの組み合わせが悪い場合は着せ替え失敗
    if( CheckDressUpCombination( rDressUpParam ) == false )
    {
      return  false;
    }

    //  きせかえが変わっているか
    bool  bChange = false;
    {
      const poke_3d::model::DressUpParam& rNowDressUpParam  = m_pCharaModel->GetDressUpParam();

      u32 loopNum = GFL_NELEMS( rDressUpParam.bodyParams );
      for( u32 i = 0; i < loopNum; ++i )
      {
        if( rNowDressUpParam.bodyParams[ i ] != rDressUpParam.bodyParams[ i ] )
        {
          bChange = true;
          break;
        }
      }

      if( bChange == false )
      {
        loopNum = GFL_NELEMS( rDressUpParam.itemParams );
        for( u32 i = 0; i < loopNum; ++i )
        {
          if( rNowDressUpParam.itemParams[ i ] != rDressUpParam.itemParams[ i ] )
          {
            bChange = true;
            break;
          }
        }
      }
    }

    if( bChange == true )
    {
      //  モデルを再ロード
      m_pCharaModel->StartLoad( m_initData.modelCharaId, rDressUpParam );

      //  タイマーアイコンを表示
      m_pTimerIconView->StartAnime();
      m_pTimerIconView->SetVisible( true );

      m_state = STATE_DRESS_UP;
    }

    return  bChange;
  }

  //  お披露目イベント開始
  void CharaModelViewer::StartShowEvent()
  {
    //  お披露目イベント
    _StartViewer( 1, 1 );

    m_state = STATE_SHOW;
  }

  //  待機イベント開始
  void CharaModelViewer::StartIdleingEvent()
  {
    if( m_state == STATE_SHOW )
    {
      //  ビューアーの場合はデフォルトに
      _UpdateDefaultViewerParam();
    }

    m_pCharaModel->SetVisible( true );

    m_state = STATE_IDLE;
  }

  //  回転イベント開始
  void CharaModelViewer::StartRotYEvent( const gfl2::math::Vector3& rRot )
  {
    StartIdleingEvent();
    /*
    if( m_state == STATE_SHOW )
    {
      //  ビューアーの場合はデフォルトに
      _UpdateDefaultViewerParam();
    }

    m_pCharaModel->SetVisible( true );

    m_addRot = rRot;
    m_state = STATE_ROT;
    */
  }

  //  回転入力イベント開始
  void CharaModelViewer::StartRotYInputEvent( const bool bDefaultViewParam )
  {
    if( bDefaultViewParam == true )
    {
      //  ビューアーの場合はデフォルトに
      _UpdateDefaultViewerParam();
    }

    m_pCharaModel->SetVisible( true );
    m_state = STATE_ROT_INPUT;
  }

  //------------------------------------------------------------------
  /**
    * @brief   キー入力取得リスナ
    * @param[in] pButton  ボタン入力の情報
    * @param[in] pKey     キー入力の情報
    * @param[in] pStick   スティック入力の情報
    * @return 入力処理を続けるかを返す
    */
  //------------------------------------------------------------------
  bool CharaModelViewer::NoticeKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    f32 rotMoveRadVal = gfl2::math::ConvDegToRad( GFL_VALUE_DEFINE_IN_HEADER ( APP_MODEL_VIEWER_PHOTO_MODEL_MODEL_ROT_MOVE_ANGLE_NUM ) );
    f32 camMove       = GFL_VALUE_DEFINE_IN_HEADER ( APP_MODEL_VIEWER_KISEKAE_CAM_POS_MOVE );

    if( m_state == STATE_ROT_INPUT )
    {
      if( pKey != NULL )
      {
        ModelViewerSequence::CmdId  cmdId = ModelViewerSequence::CMD_ID_NONE;
        ModelViewerSequence::CMD_DATA cmdData;

        // @fix 大田哲司さん要望　回転をＹボタンからＬＲボタンに切り替え
        if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
        {
          //  モデルの右回転
          cmdId = ModelViewerSequence::CMD_ID_MOVE_ROT;
          cmdData.mainData.rotMoveData.y = rotMoveRadVal;
        }
        else
        if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
        {
          cmdId = ModelViewerSequence::CMD_ID_MOVE_ROT;
          cmdData.mainData.rotMoveData.y = -rotMoveRadVal;
        }

        if( cmdId != ModelViewerSequence::CMD_ID_NONE )
        {
          m_pModelViewrSequence->NoticeCmd( cmdId, cmdData );
        }
      }
    }
    else
    if( m_state == STATE_SHOW )
    {
      //  コマンド通知
      if( pKey != NULL )
      {
        ModelViewerSequence::CmdId  cmdId = ModelViewerSequence::CMD_ID_NONE;
        ModelViewerSequence::CMD_DATA cmdData;

        if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
        {
          //  モデルの右回転（ユーザ視点）
          cmdId = ModelViewerSequence::CMD_ID_MOVE_ROT;
          cmdData.mainData.rotMoveData.y = rotMoveRadVal;
        }
        else
        if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
        {
          //  モデルの左回転（ユーザ視点）
          cmdId = ModelViewerSequence::CMD_ID_MOVE_ROT;
          cmdData.mainData.rotMoveData.y = -rotMoveRadVal;
        }
        else
        if( pKey->IsHold( gfl2::ui::DIR_DOWN ) )
        {
          //  カメラの上移動
          if( 0 < m_pModelViewrSequence->GetZoomLevel() )
          {
            cmdId = ModelViewerSequence::CMD_ID_MOVE_CAM;
            cmdData.mainData.camMoveData.y  = camMove;
          }
        }
        else
        if( pKey->IsHold( gfl2::ui::DIR_UP ) )
        {
          //  カメラの下移動
          if( 0 < m_pModelViewrSequence->GetZoomLevel() )
          {
            cmdId = ModelViewerSequence::CMD_ID_MOVE_CAM;
            cmdData.mainData.camMoveData.y  = -camMove;
          }
        }
        else
        if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
        {
          Sound::PlaySE( SEQ_SE_DECIDE1 );

          //  ズーム切り替え
          if( 0 < m_pModelViewrSequence->GetZoomLevel() )
          {
            cmdId = ModelViewerSequence::CMD_ID_ZOOM_OUT;
          }
          else
          {
            cmdId = ModelViewerSequence::CMD_ID_ZOOM_IN;
          }
        }

        if( cmdId != ModelViewerSequence::CMD_ID_NONE )
        {
          m_pModelViewrSequence->NoticeCmd( cmdId, cmdData );
        }
      }
    }

    return  false;
  }

  //  着せ替えの組み合わせで問題ないか
  bool CharaModelViewer::CheckDressUpCombination( const poke_3d::model::DressUpParam& rDressUpParam )
  {
    return  ( m_pCharaModel->CheckDressUpCombination( rDressUpParam ) );
  }

  //  モデルのきせかえリソースコア取得
  poke_3d::model::DressUpModelResourceManagerCore*  CharaModelViewer::GetDressUpModelResourceManagerCore( const poke_3d::model::DressUpParam::Sex sex )
  {
    return  ( m_pCharaModel->GetDressUpModelResourceManagerCore( sex ) );
  }

  //  デフォルトビューアーパラメータ設定
  void CharaModelViewer::_UpdateDefaultViewerParam()
  {
    m_initData.pCamera->SetPosition( m_defaultCamPos );
    m_initData.pCamera->SetFovy( m_defaultCamFovy );
    m_pCharaModel->SetPos( m_defaultModelPos );
  }

  void CharaModelViewer::_StartViewer( const u32 zoomNum, const u32 zoomMax )
  {
    ModelViewerSequence::START_PARAM  param;

    //  Viewerパラメータ
    {
      param.viewerParam.camFovy = m_defaultCamFovy;
      param.viewerParam.camPos  = m_defaultCamPos;
      param.viewerParam.modelRot  = m_pCharaModel->GetRot();
    }

    //  カメラとモデルを設定
    param.pBaseCam    = m_initData.pCamera;
    param.pBaseModel  = &m_pCharaModel->GetBaseModel();

    //  カメラのズーム設定
    param.zoomMoveTimeMillSec = (f32)GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWR_KISEKAE_CAM_ZOOM_EVENT_TIME_MILL_SEC );
    param.zoomLvMax   = zoomMax;
    param.zoomLvNow   = zoomNum;
    param.zoomFarDeg     = gfl2::math::ConvRadToDeg( m_defaultCamFovy );
    param.zoomNearDeg    = GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWR_KISEKAE_DEFAULT_CAM_ZOOM_NEAR );
    param.zoomNearTargetCamPos = m_pCharaModel->GetPos();
    param.zoomNearTargetCamPos.y = GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWR_KISEKAE_CAM_ZOOM_IN_MAX_CAM_POS_Y );
    param.camFramePivotType = ModelViewerSequence::CAM_FRAME_PIVOT_TYPE_FIX_POS;
    param.camFramePivotFixPos = param.viewerParam.camPos;

    //  カメラ最大ズームイン時のカメラ移動フレームサイズ
    param.camMoveFrameSizeNear = gfl2::math::Vector2( GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWR_KISEKAE_CAM_POS_LIMIT_FRAME_WIDTH_NEAR ), GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWR_KISEKAE_CAM_POS_LIMIT_FRAME_HEIGHT_NEAR ) );

    //  カメラ最大ズームアウト時のカメラ移動フレームサイズ
    param.camMoveFrameSizeFar = gfl2::math::Vector2( GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWR_KISEKAE_CAM_POS_LIMIT_FRAME_WIDTH_FAR ), GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWR_KISEKAE_CAM_POS_LIMIT_FRAME_HEIGHT_FAR ) );

    //  デフォルト設定をしない
    param.bDefaultViewerParam = false;

    m_pModelViewrSequence->Start( param );
  }

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

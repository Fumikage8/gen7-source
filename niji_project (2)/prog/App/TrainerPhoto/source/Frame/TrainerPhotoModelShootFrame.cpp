//=============================================================================
/**
 * @file	 TrainerPhotoModelShootFrame.h
 * @brief	 証明写真の撮影フレーム
 * @author yuto_uchida
 * @date	 2015.09.01
 */
//=============================================================================
#pragma once

#include "App/TrainerPhoto/source/frame/TrainerPhotoModelShootFrame.h"

//  gfl2のインクルード
#include <model/include/gfl2_BaseCamera.h>

//  nijiにインクルード
#include "AppLib/include/Tool/AppToolCharaSimpleModel.h"
#include "AppLib/include/Tool/AppToolTrainerIconRendering.h"

//  リソースのインクルード
// 動的ヘッダシステムを利用するときは以下のヘッダをインクルードする
#include <debug/include/DynamicHeader/gfl2_DebugDynamicHeader.h>

//  アプリ全体で使う動的パラメータ
#include "niji_conv_header/app/id_photo/TrainerPhotoModelDynamic.h"

//  モデルビューアの動的パラメータ
#include "niji_conv_header/app/dynamicHeader/AppModelViewrDynamic.h"

//  メッセージのインクルード
#include "niji_conv_header/message/msg_id_photo.h"

//  サウンド
#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( photo )

  //------------------------------------------------------------------
  /**
   * @brief  コンストラクタ
   */
  //------------------------------------------------------------------
  TrainerPhotoModelShootFrame::TrainerPhotoModelShootFrame(void)
  {
    _Clear();
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  TrainerPhotoModelShootFrame::~TrainerPhotoModelShootFrame()
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief	  動作に必要な引数を登録
    * @note
    *    必ずインスタンス生成の段階で呼び出さないと後の動作保障はなし
    */
  //------------------------------------------------------------------
  void TrainerPhotoModelShootFrame::Initialize( FRAME_PARAM* pFrameParam )
  {
    GFL_ASSERT( pFrameParam != NULL );

    m_pFrameParam = pFrameParam;

    //  出力パラメータをリセット
    m_pFrameParam->out.Reset();
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  applib::frame::Result TrainerPhotoModelShootFrame::InitFunc(void)
  {
    enum
    {
      PHOTO_MODEL_SHOOT_FRAME_INIT_FUNC_SET_CAMERA  = 0,
      PHOTO_MODEL_SHOOT_FRAME_INIT_FUNC_SET_PARAM,
      PHOTO_MODEL_SHOOT_FRAME_INIT_FUNC_END
    };

    u32 seq = applib::frame::CellSuper::GetSubSeq();

    if( seq == PHOTO_MODEL_SHOOT_FRAME_INIT_FUNC_SET_CAMERA )
    {
      if( m_modelViewrSequence.InitFunc() == false )
      {
      }
      else
      {
        ++seq;
      }
    }

    if( seq == PHOTO_MODEL_SHOOT_FRAME_INIT_FUNC_SET_PARAM )
    {
      if( m_pFrameParam->in.bViewParamContinue == false )
      {
        //  カメラとモデルを設定
        app::tool::ModelViewerSequence::START_PARAM  param;
        param.pBaseCam    = m_pFrameParam->in.pCharaModelCam;
        param.pBaseModel  = &m_pFrameParam->in.pCharaModel->GetBaseModel();

        //  Viewerパラメータ
        {
          param.viewerParam.camFovy   = gfl2::math::ConvDegToRad( GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_SHOOT_CAM_ZOOM_FAR ) );
          param.viewerParam.camPos    = 
              gfl2::math::Vector3( 
                GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_SHOOT_DEFALT_CAM_POS_X ), 
                GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_SHOOT_DEFALT_CAM_POS_Y ), 
                GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_SHOOT_DEFALT_CAM_POS_Z ) );

          param.viewerParam.modelRot  = m_pFrameParam->in.pCharaModel->GetRot();
        }

        //  カメラのズーム設定
        param.zoomMoveTimeMillSec = (f32)GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_SHOOT_CAM_ZOOM_EVENT_TIME_MILL_SEC );
        param.zoomLvMax      = GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_SHOOT_CAM_ZOOM_LEVEL_MAX );
        param.zoomLvNow      = 0;
        param.zoomFarDeg     = GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_SHOOT_CAM_ZOOM_FAR );
        param.zoomNearDeg    = GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_SHOOT_CAM_ZOOM_NEAR );
        param.zoomNearTargetCamPos = param.viewerParam.camPos;
        param.zoomNearTargetCamPos.z  = 0;

        //  カメラ最大ズームイン時のカメラ移動フレームサイズ
        param.camMoveFrameSizeNear = gfl2::math::Vector2( GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_SHOOT_DEFALT_CAM_POS_LIMIT_FRAME_WIDTH ) * 0.5f, GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_SHOOT_DEFALT_CAM_POS_LIMIT_FRAME_HEIGHT ) * 0.5f );

        //  カメラ最大ズームアウト時のカメラ移動フレームサイズ
        param.camMoveFrameSizeFar = gfl2::math::Vector2( GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_SHOOT_DEFALT_CAM_POS_LIMIT_FRAME_WIDTH ), GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_SHOOT_DEFALT_CAM_POS_LIMIT_FRAME_HEIGHT ) );

        //  デフォルト設定をしない
        param.bDefaultViewerParam = false;

        param.camFramePivotType = app::tool::ModelViewerSequence::CAM_FRAME_PIVOT_TYPE_TARGET_POS;

        m_modelViewrSequence.Start( param );
      }
      else
      {
        //  データを引き継いで開始
        m_modelViewrSequence.Continue( m_pFrameParam->in.continueViewParam );
      }

     ++seq;
    }

    if( seq == PHOTO_MODEL_SHOOT_FRAME_INIT_FUNC_END )
    {
      //  UIViewの状態を初期化
      {
        TrainerPhotoUIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;

        //  下
        {
          TrainerPhotoLowRootView*  pLowRootView  = pUIViewPool->GetRootLowView();
          pLowRootView->RemoveAllSubView();
          pLowRootView->SetState( TrainerPhotoLowRootView::STATE_IDLE );

          pLowRootView->SetInputDelegate( this );

          //  操作説明テキストを表示
          {
            gfl2::str::StrBuf*  pTempStrBuf = pLowRootView->GetG2DUtil()->GetTempStrBuf( 0 );
            pUIViewPool->GetInitParam().pAppMsgData->GetString( msg_id_photo_02_02, *pTempStrBuf );
            pLowRootView->ShowMessageWindow( pTempStrBuf );
          }

          //  決定ボタンテキストを設定
          {
            gfl2::str::StrBuf*  pTempStrBuf = pLowRootView->GetG2DUtil()->GetTempStrBuf( 0 );
            pUIViewPool->GetInitParam().pAppMsgData->GetString( msg_id_photo_01_02, *pTempStrBuf );

            pLowRootView->SetEnterButtonText( pTempStrBuf );
          }
        }

        //  上
        {
          TrainerPhotoUppShootFrameView*  pUppShootFrameView  = pUIViewPool->GetShootFrameUppView();
          pUppShootFrameView->SetState( TrainerPhotoUppShootFrameView::STATE_IDLE );

          TrainerPhotoUppRootView*  pUppRootView  = pUIViewPool->GetRootUppView();
          pUppRootView->SetState( TrainerPhotoUppRootView::STATE_IDLE );

          pUppRootView->RemoveAllSubView();
          pUppRootView->AddSubView( pUppShootFrameView );

          pUppRootView->GetG2DUtil()->SetPaneVisible( pUppRootView->GetTrainerIconRootPane(), false );
        }
      }

      //  3Dモデル表示
      {
        m_pFrameParam->in.pCharaModel->SetVisible( true );
        m_pFrameParam->in.pCharaModel->SetStopAnime( false );
      }

      return  applib::frame::RES_FINISH;
    }

    applib::frame::CellSuper::SetSubSeq( seq );

    return applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン更新を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  applib::frame::Result TrainerPhotoModelShootFrame::UpdateFunc(void)
  {
    enum
    {
      SEQ_PHOTO_SHOOT_USER_INPUT = 0,
      SEQ_PHOTO_SHOOT_RUN,
      SEQ_PHOTO_SHOOT_END
    };

    u32 seq = applib::frame::CellSuper::GetSubSeq();

    TrainerPhotoUIViewPool* pUIViewPool     = m_pFrameParam->in.pUIViewPool;
    TrainerPhotoLowRootView*  pLowRootView  = pUIViewPool->GetRootLowView();

    m_modelViewrSequence.UpdateFunc();

    if( seq == SEQ_PHOTO_SHOOT_USER_INPUT )
    {
      if( pLowRootView->IsInput() == false )
      {
      }
      else
      {
        //  ボタンを押したら終了
        {
          TrainerPhotoLowRootView::ButtonId pushButtonId  = pLowRootView->GetPushBtn();
          if( pushButtonId == TrainerPhotoLowRootView::BUTTON_ID_BACK )
          {
            m_pFrameParam->out.pushBtn  = PUSH_BTN_CANCEL;
            seq = SEQ_PHOTO_SHOOT_END;
          }
          else
          if( pushButtonId == TrainerPhotoLowRootView::BUTTON_ID_ENTER )
          {
            //  シャッター開始
            pUIViewPool->DoEventShutter();

            //  モデルのアニメを停止
            m_pFrameParam->in.pCharaModel->SetStopAnime( true );

            //  トレーナーアイコンモデルロード
            {
              TrainerPhotoUppRootView*  pUIViewUppRootView  = pUIViewPool->GetRootUppView();

              app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA  objData;
              objData.trainerId         = m_pFrameParam->in.trainerId;
              objData.pReplacePane      = pUIViewUppRootView->GetTrainerIconPicturePane();
              objData.pSyncVisiblePane  = pUIViewUppRootView->GetTrainerIconRootPane();
        
              //  テクスチャのモデル関連を設定
              {
                app::tool::AppToolTrainerIconRendering::HERO_TRAINER_ICON_DATA* pHeroTrainerIconData  = &objData.heroTrainerData;
                pHeroTrainerIconData->dressUpParam  = m_pFrameParam->in.pCharaModel->GetDressUpParam();
                pHeroTrainerIconData->camFovy       = m_pFrameParam->in.pCharaModelCam->GetFovy();
                pHeroTrainerIconData->camPos        = m_pFrameParam->in.pCharaModelCam->GetPosition();
                pHeroTrainerIconData->modelPos      = m_pFrameParam->in.pCharaModel->GetPos();
                pHeroTrainerIconData->modelRot      = m_pFrameParam->in.pCharaModel->GetRot();
                pHeroTrainerIconData->animeId       = m_pFrameParam->in.pCharaModel->GetMotionAnimeId();
                pHeroTrainerIconData->animeFrameNum = m_pFrameParam->in.pCharaModel->GetNowMotionAnimeFrame();
                pHeroTrainerIconData->eyeIndex      = m_pFrameParam->in.pCharaModel->GetEye();

      //  証明写真のアイコンパラメータ
              GFL_PRINT( "撮った証明写真のアイコンパラメータ：開始\n" );

                GFL_PRINT( "app::tool::AppToolTrainerIconRendering::HERO_TRAINER_ICON_DATA::camFovy       = %lf\n",               pHeroTrainerIconData->camFovy );
                GFL_PRINT( "app::tool::AppToolTrainerIconRendering::HERO_TRAINER_ICON_DATA::camPos        = (%lf, %lf, %lf) \n",  pHeroTrainerIconData->camPos.x, pHeroTrainerIconData->camPos.y, pHeroTrainerIconData->camPos.z );
                GFL_PRINT( "app::tool::AppToolTrainerIconRendering::HERO_TRAINER_ICON_DATA::modelPos      = (%lf, %lf, %lf)\n",   pHeroTrainerIconData->modelPos.x, pHeroTrainerIconData->modelPos.y, pHeroTrainerIconData->modelPos.z );
                GFL_PRINT( "app::tool::AppToolTrainerIconRendering::HERO_TRAINER_ICON_DATA::modelRot      = (%lf, %lf, %lf)\n",   pHeroTrainerIconData->modelRot.x, pHeroTrainerIconData->modelRot.y, pHeroTrainerIconData->modelRot.z );
                GFL_PRINT( "app::tool::AppToolTrainerIconRendering::HERO_TRAINER_ICON_DATA::animeId       = %d\n",                pHeroTrainerIconData->animeId );
                GFL_PRINT( "app::tool::AppToolTrainerIconRendering::HERO_TRAINER_ICON_DATA::animeFrameNum = %d\n",                pHeroTrainerIconData->animeFrameNum );
                GFL_PRINT( "app::tool::AppToolTrainerIconRendering::HERO_TRAINER_ICON_DATA::eyeIndex      = %d\n",                pHeroTrainerIconData->eyeIndex);

              GFL_PRINT( "撮った証明写真のアイコンパラメータ：終了\n" );
              }

              m_pFrameParam->in.pTrainerIconRender->SetIconObjectData( 0, objData );
            }

            seq = SEQ_PHOTO_SHOOT_RUN;
          }
        }
      }
    }

    if( seq == SEQ_PHOTO_SHOOT_RUN )
    {
      //  シャッターが終了
      if( pUIViewPool->IsEventShutter() == false )
      {
        m_pFrameParam->out.pushBtn  = PUSH_BTN_MODEL_SHOOT_RUN;

        seq = SEQ_PHOTO_SHOOT_END;
      }
    }

    if( seq == SEQ_PHOTO_SHOOT_END )
    {
      return applib::frame::RES_FINISH;
    }

    applib::frame::CellSuper::SetSubSeq( seq );

    return applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  void TrainerPhotoModelShootFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
  {
  }

  //------------------------------------------------------------------
  /**
   * @brief   PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  applib::frame::Result TrainerPhotoModelShootFrame::EndFunc(void)
  {
    m_pFrameParam->out.viewParam  = m_modelViewrSequence.GetUpdateParam();

    return applib::frame::RES_FINISH;
  }

  //------------------------------------------------------------------
  /**
    * @brief   キー入力通知用リスナ
    * @param[in] pButton  ボタン入力の情報
    * @param[in] pKey     キー入力の情報
    * @param[in] pStick   スティック入力の情報
    * @return 入力処理を続けるかを返す
    */
  //------------------------------------------------------------------
  void TrainerPhotoModelShootFrame::OnTrainerPhotoLowRootViewKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    f32 rotMoveRadVal = gfl2::math::ConvDegToRad( GFL_VALUE_DEFINE_IN_HEADER ( APP_MODEL_VIEWER_PHOTO_MODEL_MODEL_ROT_MOVE_ANGLE_NUM ) );
    f32 camPoaMoveVal = GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_SHOOT_CAM_POS_MOVE );

    f32 zoomSpeedVal = (f32)GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_SHOOT_ZOOM_SPEED_VAL ) / (f32)GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_SHOOT_ZOOM_SPEED_VAL_UNIT );

    //  コマンド通知
    if( pKey != NULL )
    {
      app::tool::ModelViewerSequence::CmdId  cmdId = app::tool::ModelViewerSequence::CMD_ID_NONE;
      app::tool::ModelViewerSequence::CMD_DATA cmdData;

      if( pKey->IsHold( gfl2::ui::DIR_RIGHT ) )
      {
        //  カメラの左移動(キャラは右移動)
        cmdId = app::tool::ModelViewerSequence::CMD_ID_MOVE_CAM;
        cmdData.mainData.camMoveData.x  = -camPoaMoveVal;
      }
      else
      if( pKey->IsHold( gfl2::ui::DIR_LEFT ) )
      {
        //  カメラの右移動(キャラは左移動)
        cmdId = app::tool::ModelViewerSequence::CMD_ID_MOVE_CAM;
        cmdData.mainData.camMoveData.x  = camPoaMoveVal;
      }
      else
      if( pKey->IsHold( gfl2::ui::DIR_DOWN ) )
      {
        //  カメラの上移動(キャラは下移動)
        cmdId = app::tool::ModelViewerSequence::CMD_ID_MOVE_CAM;
        cmdData.mainData.camMoveData.y  = camPoaMoveVal;
      }
      else
      if( pKey->IsHold( gfl2::ui::DIR_UP ) )
      {
        //  カメラの下移動(キャラは上移動)
        cmdId = app::tool::ModelViewerSequence::CMD_ID_MOVE_CAM;
        cmdData.mainData.camMoveData.y  = -camPoaMoveVal;
      }
      else
      if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
      {
        //  モデルの左回転（ユーザ視点）
        cmdId = app::tool::ModelViewerSequence::CMD_ID_MOVE_ROT;
        cmdData.mainData.rotMoveData.y = -rotMoveRadVal;
      }
      else
      if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
      {
        //  モデルの右回転（ユーザ視点）
        cmdId = app::tool::ModelViewerSequence::CMD_ID_MOVE_ROT;
        cmdData.mainData.rotMoveData.y = rotMoveRadVal;
      }
      else
      if( pButton->IsHold( gfl2::ui::BUTTON_X ) )
      {
        cmdId = app::tool::ModelViewerSequence::CMD_ID_ZOOM_MANUAL;
        cmdData.mainData.zoomManualData.zoomVal = zoomSpeedVal;
      }
      else
      if( pButton->IsHold( gfl2::ui::BUTTON_Y ) )
      {
        cmdId = app::tool::ModelViewerSequence::CMD_ID_ZOOM_MANUAL;
        cmdData.mainData.zoomManualData.zoomVal = -zoomSpeedVal;
      }

      if( cmdId != app::tool::ModelViewerSequence::CMD_ID_NONE )
      {
        m_modelViewrSequence.NoticeCmd( cmdId, cmdData );
      }
    }
  }

GFL_NAMESPACE_END( photo )
GFL_NAMESPACE_END( app )

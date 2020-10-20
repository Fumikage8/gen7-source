//=============================================================================
/**
 * @file	 TrainerPhotoModelViewerFrame.h
 * @brief	 証明写真のモデルビューアーフレーム
 * @author yuto_uchida
 * @date	 2015.09.01
 */
//=============================================================================
#pragma once
#include "App/TrainerPhoto/source/frame/TrainerPhotoModelViewerFrame.h"

//  gfl2のインクルード
#include <model/include/gfl2_BaseCamera.h>

//  nijiにインクルード
#include "AppLib/include/Tool/AppToolCharaSimpleModel.h"

//  リソースのインクルード
// 動的ヘッダシステムを利用するときは以下のヘッダをインクルードする
#include <debug/include/DynamicHeader/gfl2_DebugDynamicHeader.h>

//  アプリ全体で使う動的パラメータ
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
  TrainerPhotoModelViewerFrame::TrainerPhotoModelViewerFrame(void)
  {
    _Clear();
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  TrainerPhotoModelViewerFrame::~TrainerPhotoModelViewerFrame()
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief	  動作に必要な引数を登録
    * @note
    *    必ずインスタンス生成の段階で呼び出さないと後の動作保障はなし
    */
  //------------------------------------------------------------------
  void TrainerPhotoModelViewerFrame::Initialize( FRAME_PARAM* pFrameParam, const bool bDefaltViwewrParam )
  {
    GFL_ASSERT( pFrameParam );
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
  applib::frame::Result TrainerPhotoModelViewerFrame::InitFunc(void)
  {
    enum
    {
      PHOTO_MODEL_VIEWER_FRAME_INIT_FUNC_VIEW_SEQ_INIT  = 0,
      PHOTO_MODEL_VIEWER_FRAME_INIT_FUNC_SET_PARAM,
      PHOTO_MODEL_VIEWER_FRAME_INIT_FUNC_END
    };

    u32 seq = applib::frame::CellSuper::GetSubSeq();

    if( seq == PHOTO_MODEL_VIEWER_FRAME_INIT_FUNC_VIEW_SEQ_INIT )
    {
      if( m_modelViewrSequence.InitFunc() == false )
      {
      }
      else
      {
        ++seq;
      }
    }

    if( seq == PHOTO_MODEL_VIEWER_FRAME_INIT_FUNC_SET_PARAM )
    {
      //  カメラとモデルを設定
      app::tool::ModelViewerSequence::START_PARAM  param;
      param.pBaseCam    = m_pFrameParam->in.pCharaModelCam;
      param.pBaseModel  = &m_pFrameParam->in.pCharaModel->GetBaseModel();

      //  Viewerパラメータ
      {
        param.viewerParam.camFovy   = gfl2::math::ConvDegToRad( GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_VIEWER_CAM_ZOOM_FAR ) );
        param.viewerParam.camPos    = 
            gfl2::math::Vector3( 
              GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_VIEWER_DEFALT_CAM_POS_X ),
              GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_VIEWER_DEFALT_CAM_POS_Y ),
              GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_VIEWER_DEFALT_CAM_POS_Z ) );

        param.viewerParam.modelRot  = m_pFrameParam->in.pCharaModel->GetRot();
      }

      //  カメラのズーム設定
      param.zoomMoveTimeMillSec = (f32)GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_VIEWER_CAM_ZOOM_EVENT_TIME_MILL_SEC );
      param.zoomLvMax      = 1;
      param.zoomLvNow      = 0;
      param.zoomFarDeg     = GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_VIEWER_CAM_ZOOM_FAR );
      param.zoomNearDeg    = GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_VIEWER_CAM_ZOOM_NEAR );
      param.zoomNearTargetCamPos = m_pFrameParam->in.pCharaModel->GetPos();
      param.zoomNearTargetCamPos.y = GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_VIEWER_CAM_ZOOM_IN_MAX_CAM_POS_Y );
      //  指定した固定座標値を中心にカメラ移動範囲を決める
      param.camFramePivotType      = app::tool::ModelViewerSequence::CAM_FRAME_PIVOT_TYPE_FIX_POS;
      param.camFramePivotFixPos    = param.viewerParam.camPos;

      //  カメラ最大ズームイン時のカメラ移動フレームサイズ
      param.camMoveFrameSizeNear = gfl2::math::Vector2( 
      GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_VIEWER_CAM_POS_LIMIT_FRAME_WIDHT_NAER ), 
      GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_VIEWER_CAM_POS_LIMIT_FRAME_HEIGHT_NAER )  );

      //  カメラ最大ズームアウト時のカメラ移動フレームサイズ
      param.camMoveFrameSizeFar = gfl2::math::Vector2( 
      GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_VIEWER_CAM_POS_LIMIT_FRAME_WIDHT_FAR ), 
      GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_VIEWER_CAM_POS_LIMIT_FRAME_HEIGHT_FAR ) );

      //  デフォルト設定をしない
      param.bDefaultViewerParam = false;

      m_modelViewrSequence.Start( param );

     ++seq;
    }

    if( seq == PHOTO_MODEL_VIEWER_FRAME_INIT_FUNC_END )
    {
      //  3Dモデル表示
      {
        m_pFrameParam->in.pCharaModel->SetVisible( true );
        m_pFrameParam->in.pCharaModel->SetStopAnime( false );
      }

      //  UIViewの状態を初期化
      {
        TrainerPhotoUIViewPool* pUIViewPool = m_pFrameParam->in.pUIViewPool;

        //  下
        {
          TrainerPhotoLowRootView*  pLowRootView  = pUIViewPool->GetRootLowView();
          pLowRootView->RemoveAllSubView();
          pLowRootView->SetState( TrainerPhotoLowRootView::STATE_IDLE );

          pLowRootView->SetInputDelegate( this );

          //  操作説明テキスト表示
          {
            gfl2::str::StrBuf*  pTempStrBuf = pLowRootView->GetG2DUtil()->GetTempStrBuf( 0 );
            pUIViewPool->GetInitParam().pAppMsgData->GetString( msg_id_photo_02_01, *pTempStrBuf );
            pLowRootView->ShowMessageWindow( pTempStrBuf );
          }

          //  ボタンテキスト設定
          {
            gfl2::str::StrBuf*  pTempStrBuf = pLowRootView->GetG2DUtil()->GetTempStrBuf( 0 );
            pUIViewPool->GetInitParam().pAppMsgData->GetString( msg_id_photo_01_01, *pTempStrBuf );
            pLowRootView->SetEnterButtonText( pTempStrBuf );
          }
        }

        //  上
        {
          TrainerPhotoUppRootView*  pUppRootView  = pUIViewPool->GetRootUppView();
          pUppRootView->RemoveAllSubView();
          pUppRootView->SetState( TrainerPhotoUppRootView::STATE_IDLE );

          pUppRootView->GetG2DUtil()->SetPaneVisible( pUppRootView->GetTrainerIconRootPane(), false );
        }
      }

      return applib::frame::RES_FINISH;
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
  applib::frame::Result TrainerPhotoModelViewerFrame::UpdateFunc(void)
  {
    m_modelViewrSequence.UpdateFunc();

    TrainerPhotoLowRootView*  pLowRootView  = m_pFrameParam->in.pUIViewPool->GetRootLowView();
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
          return applib::frame::RES_FINISH;
        }
        else
        if( pushButtonId == TrainerPhotoLowRootView::BUTTON_ID_ENTER )
        {
          m_pFrameParam->out.pushBtn  = PUSH_BTN_MODEL_SHOOT;
          return applib::frame::RES_FINISH;
        }
      }
    }

    return applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  void TrainerPhotoModelViewerFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
  {
  }

  //------------------------------------------------------------------
  /**
   * @brief   PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  applib::frame::Result TrainerPhotoModelViewerFrame::EndFunc(void)
  {
    if( m_modelViewrSequence.EndFunc() == true )
    {
      return applib::frame::RES_FINISH;
    }

    return applib::frame::RES_CONTINUE;
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
  void TrainerPhotoModelViewerFrame::OnTrainerPhotoLowRootViewKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    f32 rotMoveRadVal = gfl2::math::ConvDegToRad( GFL_VALUE_DEFINE_IN_HEADER ( APP_MODEL_VIEWER_PHOTO_MODEL_MODEL_ROT_MOVE_ANGLE_NUM ) );
    f32 camPoaMoveVal = GFL_VALUE_DEFINE_IN_HEADER( APP_MODEL_VIEWER_PHOTO_MODEL_VIEWER_CAM_POS_MOVE );

    //  コマンド通知
    if( pKey != NULL )
    {
      app::tool::ModelViewerSequence::CmdId  cmdId = app::tool::ModelViewerSequence::CMD_ID_NONE;
      app::tool::ModelViewerSequence::CMD_DATA cmdData;

      if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
      {
        //  モデルの右回転（ユーザ視点）
        cmdId = app::tool::ModelViewerSequence::CMD_ID_MOVE_ROT;
        cmdData.mainData.rotMoveData.y = rotMoveRadVal;
      }
      else
      if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
      {
        //  モデルの左回転（ユーザ視点）
        cmdId = app::tool::ModelViewerSequence::CMD_ID_MOVE_ROT;
        cmdData.mainData.rotMoveData.y = -rotMoveRadVal;
      }
      else
      if( pKey->IsHold( gfl2::ui::DIR_DOWN ) )
      {
        //  カメラの上移動
        if( 0 < m_modelViewrSequence.GetZoomLevel() )
        {
          cmdId = app::tool::ModelViewerSequence::CMD_ID_MOVE_CAM;
          cmdData.mainData.camMoveData.y  = camPoaMoveVal;
        }
      }
      else
      if( pKey->IsHold( gfl2::ui::DIR_UP ) )
      {
        //  カメラの下移動
        if( 0 < m_modelViewrSequence.GetZoomLevel() )
        {
          cmdId = app::tool::ModelViewerSequence::CMD_ID_MOVE_CAM;
          cmdData.mainData.camMoveData.y  = -camPoaMoveVal;
        }
      }
      else
      if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
      {
        Sound::PlaySE( SEQ_SE_DECIDE1 );

        //  ズーム切り替え
        if( 0 < m_modelViewrSequence.GetZoomLevel() )
        {
          cmdId = app::tool::ModelViewerSequence::CMD_ID_ZOOM_OUT;
        }
        else
        {
          cmdId = app::tool::ModelViewerSequence::CMD_ID_ZOOM_IN;
        }
      }

      if( cmdId != app::tool::ModelViewerSequence::CMD_ID_NONE )
      {
        m_modelViewrSequence.NoticeCmd( cmdId, cmdData );
      }
    }
  }

GFL_NAMESPACE_END( photo )
GFL_NAMESPACE_END( app )

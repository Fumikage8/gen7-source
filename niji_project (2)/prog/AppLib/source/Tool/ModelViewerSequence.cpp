//=============================================================================
/**
 * @file	 ModelViewerSequence.h
 * @brief	 モデルビューのシーケンス
 * @author yuto_uchida
 * @date	 2015.10.15
 */
//=============================================================================
#pragma once

#include "AppLib/include/Tool/ModelViewerSequence.h"

//  gfl2のインクルード
#include <model/include/gfl2_BaseCamera.h>
#include <model/include/gfl2_BaseModel.h>

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )

  //------------------------------------------------------------------
  /**
   * @brief  コンストラクタ
   */
  //------------------------------------------------------------------
  ModelViewerSequence::ModelViewerSequence(void)
  {
    _Clear();
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ModelViewerSequence::~ModelViewerSequence()
  {
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  bool ModelViewerSequence::InitFunc(void)
  {
    enum
    {
      INIT_FUNC_SET  = 0,
    };

    //  最初にカメラの設定をしてからモデルを表示する
    if( m_initSeq == INIT_FUNC_SET )
    {
      //  カメラの初期設定
      {
        m_defaltViewrParam.camPos  = gfl2::math::Vector3( 0, 0, 0 );
      }

      //  カメラの初回Zoom
      {
        m_defaltViewrParam.camFovy = gfl2::math::ConvDegToRad( 30 );
      }

      //  モデルの回転軸
      m_defaltViewrParam.modelRot  = gfl2::math::Vector3( 0.f, 0.f, 0.f );

      ++m_initSeq;
    }
    else
    {
      return  true;
    }

    return false;
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン更新を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  void ModelViewerSequence::UpdateFunc(void)
  {
    if( m_mainSeq == SEQ_VIEWER_CMD_CATCH )
    {
      //  コマンド受け取り中
    }
    else
    if( m_mainSeq == SEQ_VIEWER_ZOOM_ANIME )
    {
      //  ズーム演出中
      {
        f32 progressionVal  = m_zoomMoveData.eventTimeLimit.GetProgression();

        if( _ProcCamZoom( progressionVal ) == false )
        {
          m_mainSeq = SEQ_VIEWER_CMD_CATCH;
        }
      }

      m_zoomMoveData.eventTimeLimit.Update();

      _UpdateViewerCamParam();
    }
  }

  //------------------------------------------------------------------
  /**
   * @brief   PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  bool ModelViewerSequence::EndFunc(void)
  {
    m_initSeq = 0;
    return true;
  }

  //------------------------------------------------------------------
  /**
    * @brief	  処理開始
    * @note    処理を開始するのに呼び出す
    */
  //------------------------------------------------------------------
  void ModelViewerSequence::Start( const START_PARAM& rStartParam )
  {
    m_mainSeq = SEQ_VIEWER_CMD_CATCH;
    m_startParam  = rStartParam;

    if( m_startParam.bDefaultViewerParam == true )
    {
      m_startParam.viewerParam  = m_defaltViewrParam;
    }

    m_startCamPos = rStartParam.viewerParam.camPos;

    GFL_ASSERT( m_startParam.pBaseCam );
    GFL_ASSERT( m_startParam.pBaseModel );

    //  カメラとモデルのパラメータを更新
    _UpdateViewerCamParam();
    _UpdateViewerModelParam();

    //  初回ズーム設定
    _SetZoomLevel( m_startParam.zoomLvNow );
  }

  void ModelViewerSequence::Continue( const START_PARAM& rStartParam )
  {
    m_mainSeq = SEQ_VIEWER_CMD_CATCH;
    m_startParam  = rStartParam;

    //  カメラとモデルのパラメータを更新
    _UpdateViewerCamParam();
    _UpdateViewerModelParam();

    m_startCamPos = rStartParam.viewerParam.camPos;
    m_zoomLevel = m_startParam.zoomLvNow;
    // @fix NMCat[377]  保持したスケール値を設定
    m_zoomParamScaleVal = m_startParam.nowZoomVal;

    if( m_startParam.camFramePivotType == CAM_FRAME_PIVOT_TYPE_CAM_POS )
    {
      _UpdataCamFrameRect( m_startParam.viewerParam.camPos, m_zoomParamScaleVal );
    }
    else
    if( m_startParam.camFramePivotType == CAM_FRAME_PIVOT_TYPE_TARGET_POS )
    {
      _UpdataCamFrameRect( m_startParam.zoomNearTargetCamPos, m_zoomParamScaleVal );
    }
    else
    {
      GFL_ASSERT( 0 );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief	  コマンド通知
    */
  //------------------------------------------------------------------
  bool ModelViewerSequence::NoticeCmd( const CmdId id, const CMD_DATA& rData )
  {
    if( m_mainSeq == SEQ_VIEWER_CMD_CATCH )
    {
      //  コマンド受け取り
      if( id == CMD_ID_ZOOM_IN )
      {
        //  ズームイン
        if( _ChangeZoomLevel( 1 ) == true )
        {
          _UpdateViewerCamParam();
        }
      }
      else
      if( id == CMD_ID_ZOOM_OUT )
      {
        //  ズームアウト
        if( _ChangeZoomLevel( -1 ) == true )
        {
          _UpdateViewerCamParam();
        }
      }
      else
      if( id == CMD_ID_MOVE_ROT )
      {
        //  モデル回転左
        m_startParam.viewerParam.modelRot.x += rData.mainData.rotMoveData.x;
        m_startParam.viewerParam.modelRot.y += rData.mainData.rotMoveData.y;
        m_startParam.viewerParam.modelRot.z += rData.mainData.rotMoveData.z;

        _UpdateViewerModelParam();
      }
      else
      if( id == CMD_ID_MOVE_CAM )
      {
        //  カメラ移動
        m_startParam.viewerParam.camPos.x += rData.mainData.camMoveData.x;
        m_startParam.viewerParam.camPos.y += rData.mainData.camMoveData.y;
        m_startParam.viewerParam.camPos.z += rData.mainData.camMoveData.z;

        //  移動範囲に収める
        {
          m_startParam.viewerParam.camPos.x = gfl2::math::Clamp<f32>( m_startParam.viewerParam.camPos.x, m_camPosMin.x, m_camPosMax.x );
          m_startParam.viewerParam.camPos.y = gfl2::math::Clamp<f32>( m_startParam.viewerParam.camPos.y, m_camPosMin.y, m_camPosMax.y );
        }

        _UpdateViewerCamParam();
      }
      else
      if( id == CMD_ID_ZOOM_MANUAL )
      {
        //  ズームを手動
        f32 nowFovy = m_startParam.pBaseCam->GetFovy() + rData.mainData.zoomManualData.zoomVal;

        //  視野角を調整
        {
          f32 zoomFarRad  = gfl2::math::ConvDegToRad( m_startParam.zoomFarDeg );
          f32 zoomNearRad = gfl2::math::ConvDegToRad( m_startParam.zoomNearDeg );
          nowFovy = gfl2::math::Clamp<f32>( nowFovy, zoomNearRad, zoomFarRad );

          m_startParam.viewerParam.camFovy  = nowFovy;
        }

        f32 scaleVal  = 0.0f;
        {
          f32 subLengthFovy = m_startParam.zoomFarDeg - m_startParam.zoomNearDeg;
          f32 subLengthNowFovy  = ( gfl2::math::ConvRadToDeg( nowFovy ) - m_startParam.zoomNearDeg );

          if( subLengthNowFovy <= 0 )
          {
          }
          else
          {
            scaleVal = subLengthNowFovy / subLengthFovy;
          }

          scaleVal = gfl2::math::Clamp<f32>( scaleVal, 0.0f, 1.0f );
          //  反転させるnearの小さい値が比率が１でfarの大きいな値が比率は０にならないといけない
          scaleVal  = 1.0f - scaleVal;
        }

        if( m_startParam.camFramePivotType == CAM_FRAME_PIVOT_TYPE_TARGET_POS )
        {
          //  移動範囲を作成
          _UpdataCamFrameRect( m_startParam.zoomNearTargetCamPos, scaleVal );
        }
        else
        if( m_startParam.camFramePivotType == CAM_FRAME_PIVOT_TYPE_FIX_POS )
        {
          //  移動範囲を作成
          _UpdataCamFrameRect( m_startParam.camFramePivotFixPos, scaleVal );
        }
        
        // @fix NMCat[377]  変更したスケール値を保持
        m_startParam.nowZoomVal = m_zoomParamScaleVal = scaleVal;

        //  移動範囲に収める
        {
          m_startParam.viewerParam.camPos.x = gfl2::math::Clamp<f32>( m_startParam.viewerParam.camPos.x, m_camPosMin.x, m_camPosMax.x );
          m_startParam.viewerParam.camPos.y = gfl2::math::Clamp<f32>( m_startParam.viewerParam.camPos.y, m_camPosMin.y, m_camPosMax.y );
        }

        _UpdateViewerModelParam();
        _UpdateViewerCamParam();
      }

      return  true;
    }

    return  false;
  }

  //  ビューアのパラメータ更新
  void ModelViewerSequence::_UpdateViewerModelParam()
  {
    //  モデルの回転
    m_startParam.pBaseModel->SetRotation( m_startParam.viewerParam.modelRot );
  }

  void ModelViewerSequence::_UpdateViewerCamParam()
  {
    //  カメラの位置
    m_startParam.pBaseCam->SetPosition( m_startParam.viewerParam.camPos );

    //  注視点のXYを合わせる
    gfl2::math::Vector3 targetPos = m_startParam.pBaseCam->GetTargetPosition();
    targetPos.SetX( m_startParam.viewerParam.camPos.x );
    targetPos.SetY( m_startParam.viewerParam.camPos.y );

    m_startParam.pBaseCam->SetTargetPosition( targetPos );

    m_startParam.pBaseCam->SetFovy( m_startParam.viewerParam.camFovy );

    //  near/far
    // @fix NMCat[374] 深度バッファの幅が少なくなるのでモデルが崩れるのを防ぐ
    m_startParam.pBaseCam->SetNear( 300.0f );
    m_startParam.pBaseCam->SetFar( 1300.0f );
  }

  //  ズーム設定
  void ModelViewerSequence::_SetZoomLevel( const u32 lv )
  {
    //  データ生成
    _CreateCamZoomData( lv );

    //  更新
    _ProcCamZoom( 1.0f );
  }

  //  カメラズームモード切替
  bool ModelViewerSequence::_ChangeZoomLevel( const s32 add )
  {
    bool  bChange = false;

    if( m_startParam.zoomLvMax <= 0 )
    {
      //  失敗
      return  false;
    }

    if( add < 0 )
    {
      //  ズームアウト
      if( 0 < m_zoomLevel )
      {
        m_zoomLevel += add;

        bChange = true;
      }
    }
    else
    if( 0 < add )
    {
      //  ズームイン
      if( (u32)m_zoomLevel < m_startParam.zoomLvMax )
      {
        m_zoomLevel += add;

        bChange = true;
      }
    }

    if( bChange == false )
    {
      return  false;
    }

    //  データ生成
    _CreateCamZoomData( m_zoomLevel );

    //  ズーム時間を設定
    u64 zoomEventTimeMillSec  = m_startParam.zoomMoveTimeMillSec;
    m_zoomMoveData.eventTimeLimit.Setup( zoomEventTimeMillSec, App::Tool::TimeLimit::UNIT_TYPE_MILL_SEC );

    m_mainSeq = SEQ_VIEWER_ZOOM_ANIME;

    return  true;
  }

  //  カメラズームデータ作成
  void ModelViewerSequence::_CreateCamZoomData( const u32 lv )
  {
    m_zoomLevel = lv;
    m_startParam.zoomLvNow  = m_zoomLevel;

    //  ズームパラメータ生成
    {
      m_zoomParamScaleVal = 0.0f;
      if( 0 < m_zoomLevel )
      {
        m_zoomParamScaleVal = (f32)m_zoomLevel / (f32)m_startParam.zoomLvMax;
      }
      //  ズーム値の目的値を取得
      m_startParam.nowZoomVal = m_zoomParamScaleVal;

      //  カメラズーム時の見かた取得
      {
        m_zoomMoveData.startForv  = m_startParam.pBaseCam->GetFovy();

        m_zoomMoveData.endForv    = gfl2::math::Lerp( m_startParam.zoomFarDeg, m_startParam.zoomNearDeg, m_zoomParamScaleVal );
        m_zoomMoveData.endForv    = gfl2::math::ConvDegToRad( m_zoomMoveData.endForv );
      }

      //  カメラズーム時の座標更新
      {
        m_zoomMoveData.startCamPos.y    = m_startParam.pBaseCam->GetPosition().y;
        m_zoomMoveData.startCamPos.x    = m_startParam.pBaseCam->GetPosition().x;
        m_zoomMoveData.endCamPos.x      = gfl2::math::Lerp( m_zoomMoveData.startCamPos.x, m_startParam.zoomNearTargetCamPos.x, m_zoomParamScaleVal );
        m_zoomMoveData.endCamPos.y      = gfl2::math::Lerp( m_startCamPos.y, m_startParam.zoomNearTargetCamPos.y, m_zoomParamScaleVal );

        if( m_startParam.camFramePivotType == CAM_FRAME_PIVOT_TYPE_TARGET_POS )
        {
          //  移動範囲を作成
          _UpdataCamFrameRect( m_startParam.zoomNearTargetCamPos, m_zoomParamScaleVal );

          //  目標位置を中心した移動範囲に収めるように修正
          m_zoomMoveData.endCamPos.x = gfl2::math::Clamp<f32>( m_zoomMoveData.endCamPos.x, m_camPosMin.x, m_camPosMax.x );
          m_zoomMoveData.endCamPos.y = gfl2::math::Clamp<f32>( m_zoomMoveData.endCamPos.y, m_camPosMin.y, m_camPosMax.y );
        }
        else
        if( m_startParam.camFramePivotType == CAM_FRAME_PIVOT_TYPE_FIX_POS )
        {
          //  移動範囲を作成
          _UpdataCamFrameRect( m_startParam.camFramePivotFixPos, m_zoomParamScaleVal );

          //  目標位置を中心した移動範囲に収めるように修正
          m_zoomMoveData.endCamPos.x = gfl2::math::Clamp<f32>( m_zoomMoveData.endCamPos.x, m_camPosMin.x, m_camPosMax.x );
          m_zoomMoveData.endCamPos.y = gfl2::math::Clamp<f32>( m_zoomMoveData.endCamPos.y, m_camPosMin.y, m_camPosMax.y );
        }
      }
    }
  }

  //  カメラズーム処理更新
  bool ModelViewerSequence::_ProcCamZoom( const f32 progress )
  {
    //  カメラのズーム取得
    m_startParam.viewerParam.camFovy  = gfl2::math::Lerp( m_zoomMoveData.startForv, m_zoomMoveData.endForv, progress );

    //  カメラのズームによるカメラY座標を線形補間で取得
    m_startParam.viewerParam.camPos.y = gfl2::math::Lerp( m_zoomMoveData.startCamPos.y, m_zoomMoveData.endCamPos.y, progress );
    m_startParam.viewerParam.camPos.x = gfl2::math::Lerp( m_zoomMoveData.startCamPos.x, m_zoomMoveData.endCamPos.x, progress );

    _UpdateViewerModelParam();
    _UpdateViewerCamParam();

    if( 1.0f <= progress )
    {
      //  カメラ移動範囲計算
      if( m_startParam.camFramePivotType == CAM_FRAME_PIVOT_TYPE_CAM_POS )
      {
        //  移動後のカメラ同期
        _UpdataCamFrameRect( m_startParam.viewerParam.camPos, m_zoomParamScaleVal );
      }

      return false;
    }

    return true;
  }

  void ModelViewerSequence::_UpdataCamFrameRect( const gfl2::math::Vector3& rCenterPos, const f32 rateVal )
  {
    //  カメラズームになるほど範囲を小さくする
    float camLimitFrameHalfWidth  = gfl2::math::Lerp( m_startParam.camMoveFrameSizeFar.x * 0.5f, m_startParam.camMoveFrameSizeNear.x * 0.5f, rateVal );
    float camLimitFrameHalfHeight  = gfl2::math::Lerp( m_startParam.camMoveFrameSizeFar.y * 0.5f, m_startParam.camMoveFrameSizeNear.y * 0.5f, rateVal );

    m_camPosMax.x = rCenterPos.x + camLimitFrameHalfWidth;
    m_camPosMin.x = rCenterPos.x - camLimitFrameHalfWidth;

    m_camPosMax.y = rCenterPos.y + camLimitFrameHalfHeight;
    m_camPosMin.y = rCenterPos.y - camLimitFrameHalfHeight;
  }

GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )

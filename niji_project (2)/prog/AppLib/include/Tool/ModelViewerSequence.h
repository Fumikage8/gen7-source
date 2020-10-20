//=============================================================================
/**
 * @file	 ModelViewerSequence.h
 * @brief	 モデルビューのシーケンス
 * @author yuto_uchida
 * @date	 2015.10.15
 */
//=============================================================================
#ifndef APP_TOOL_MODEL_VIEWER_SEQUENCE_H_INCLUDED
#define APP_TOOL_MODEL_VIEWER_SEQUENCE_H_INCLUDED
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_math.h>
#include <util/include/gfl2_std_string.h>

//  nijiのインクルード
#include "AppLib/include/Frame/AppFrameCellSuper.h"
#include "AppLib/include/Tool/AppToolTimeLimit.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(poke_3d)
GFL_NAMESPACE_BEGIN(model)

  class BaseModel;
  class BaseCamera;

GFL_NAMESPACE_END(model)
GFL_NAMESPACE_END(poke_3d)

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )

  /** モデルのビューアーシーケンス */
  class ModelViewerSequence
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(ModelViewerSequence);
  public:
    enum CmdId
    {
      CMD_ID_NONE = 0,
      CMD_ID_ZOOM_IN,
      CMD_ID_ZOOM_OUT,
      CMD_ID_MOVE_ROT,
      CMD_ID_MOVE_CAM,
      CMD_ID_ZOOM_MANUAL,
    };

    //  カメラの移動範囲の中心タイプ
    enum CamFramePivotType
    {
      //  常にカメラの座標を中心に移動範囲を決める
      CAM_FRAME_PIVOT_TYPE_CAM_POS = 0,
      //  ズーム時のカメラの注視点を中心に移動範囲を決める
      CAM_FRAME_PIVOT_TYPE_TARGET_POS,
      //  固定設定した位置を中心の移動範囲で決める( camFramePivotFixPos の値を設定しないとNG )
      CAM_FRAME_PIVOT_TYPE_FIX_POS,
    };
    
    struct CMD_DATA
    {
      CMD_DATA()
      {
        gfl2::std::MemClear( &mainData, sizeof( mainData ) );
      }

      union MAIN
      {
        // コマンド[CMD_ID_MOVE_ROT]のデータ
        struct MODEL_DATA
        {
          f32 x, y, z;
        } rotMoveData;

        //  コマンド[CMD_ID_MOVE_CAM]のデータ
        struct CAM_MOVE_DATA
        {
          f32 x, y, z;
        } camMoveData;

        //  コマンド「CMD_ID_ZOOM_MANUAL」のデータ
        struct ZOOM_MANUAL_DATA
        {
          f32 zoomVal;
        } zoomManualData;

      } mainData;
    };

    //  3D描画パラメータ
    struct VIEWER_PARAM
    {
      VIEWER_PARAM()
      {
        camFovy = 0.0f;
      }

      f32                 camFovy;

      gfl2::math::Vector3 modelRot;

      gfl2::math::Vector3 camPos;
    };

    //  開始パラメータ
    struct START_PARAM
    {
      START_PARAM()
      {
        Reset();
      }

      void Reset()
      {
        pBaseModel  = NULL;
        pBaseCam    = NULL;
        zoomLvMax   = 0;
        zoomLvNow   = 0;
        zoomFarDeg  = 0.0f;
        zoomNearDeg = 0.0f;
        nowZoomVal  = 0.0f;
        zoomMoveTimeMillSec = 0.0f;
        camFramePivotType = CAM_FRAME_PIVOT_TYPE_CAM_POS;
        bDefaultViewerParam = true;
      }

      //  ズームレベルの初回、最大値
      u32                 zoomLvNow;
      u32                 zoomLvMax;

      //  ズーム時のアニメ時間(ミリ秒単位)
      f32                 zoomMoveTimeMillSec;

      //  ズームの遠近値
      f32                 zoomFarDeg, zoomNearDeg;

      //  カメラ移動枠のサイズの最大・最小
      gfl2::math::Vector2 camMoveFrameSizeFar, camMoveFrameSizeNear;

      //  ズームのもっとも近い時のカメラ位置
      gfl2::math::Vector3 zoomNearTargetCamPos;

      //  モデルオブジェクト
      poke_3d::model::BaseModel*  pBaseModel;

      //  カメラ
      poke_3d::model::BaseCamera* pBaseCam;
      
      //  3D描画パラメータ
      VIEWER_PARAM  viewerParam;

      CamFramePivotType camFramePivotType;

      gfl2::math::Vector3 camFramePivotFixPos;

      //  カメラ現在ズーム値
      f32   nowZoomVal;

      bool  bDefaultViewerParam;
    };

    //------------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     */
    //------------------------------------------------------------------
    ModelViewerSequence();

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~ModelViewerSequence();

    //------------------------------------------------------------------
    /**
     * @brief	  PROCの初期化を行う関数
     */
    //------------------------------------------------------------------
    bool  InitFunc();

    //------------------------------------------------------------------
    /**
     * @brief	  PROCのメイン更新を行う関数
     */
    //------------------------------------------------------------------
    void UpdateFunc();

    //------------------------------------------------------------------
    /**
     * @brief	  PROCの終了を行う関数
     */
    //------------------------------------------------------------------
    bool EndFunc();

    //------------------------------------------------------------------
    /**
     * @brief	    処理開始
     * @note      処理を開始するのに呼び出す
     */
    //------------------------------------------------------------------
    void Start( const START_PARAM& rStartParam );

    //  GetUpdateParam()から取得した値を設定して続きから始める場合に呼び出す
    void Continue( const START_PARAM& rStartParam );

    //------------------------------------------------------------------
    /**
     * @brief	  コマンド通知
     * @return  コマンド成功 -> true / 失敗 -> false
     */
    //------------------------------------------------------------------
    bool NoticeCmd( const CmdId id, const CMD_DATA& rData );

    //  ズームレベル
    //  0 = ズームしていない
    //  1以上がズームインして拡大している
    u32 GetZoomLevel() const { return (u32)m_zoomLevel; }

    //  クラス内で更新しているパラメータを取得
    const START_PARAM& GetUpdateParam() const { return m_startParam; }

  private:
    enum
    {
      SEQ_VIEWER_NONT = 0,
      SEQ_VIEWER_CMD_CATCH,
      SEQ_VIEWER_ZOOM_ANIME,
    };

    //  ズーム移動用のデータ
    struct ZOOM_MOVE_DATA
    {
      void Reset()
      {
        startForv = 0;
        endForv   = 0;
      }

      f32 startForv, endForv;
      gfl2::math::Vector3 startCamPos, endCamPos;

      App::Tool::TimeLimit  eventTimeLimit;
    };

    //  ビューアのパラメータ更新
    void _UpdateViewerModelParam();
    void _UpdateViewerCamParam();

    //  カメラズームモード切替
    bool _ChangeZoomLevel( const s32 add );

    //  ズーム設定
    void _SetZoomLevel( const u32 lv );

    //  カメラズームデータ作成
    void _CreateCamZoomData( const u32 lv );

    //  カメラズーム処理更新
    bool _ProcCamZoom( const f32 progress );

    void _UpdataCamFrameRect( const gfl2::math::Vector3& rCenterPos, const f32 rateVal );

    //  変数初期化
    void _Clear()
    {
      m_zoomLevel   = 0;
      m_initSeq     = 0;
      m_mainSeq     = 0;
      m_zoomParamScaleVal = 0;
    }

    gfl2::math::Vector2 m_camPosMin;
    gfl2::math::Vector2 m_camPosMax;

    gfl2::math::Vector3 m_startCamPos;

    u32                 m_initSeq;
    u32                 m_mainSeq;

    f32                 m_zoomParamScaleVal;
    ZOOM_MOVE_DATA      m_zoomMoveData;
    s32                 m_zoomLevel;

    START_PARAM         m_startParam;

    VIEWER_PARAM        m_defaltViewrParam;
};

GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )


#endif // APP_TOOL_MODEL_VIEWER_SEQUENCE_H_INCLUDED

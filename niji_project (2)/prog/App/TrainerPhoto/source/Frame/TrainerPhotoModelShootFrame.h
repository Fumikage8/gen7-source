//=============================================================================
/**
 * @file	 TrainerPhotoModelShootFrame.h
 * @brief	 証明写真の撮影フレーム
 * @author yuto_uchida
 * @date	 2015.09.01
 */
//=============================================================================
#ifndef TRAINER_PHOTO_MODEL_SHOOT_FRAME_H_INCLUDED
#define TRAINER_PHOTO_MODEL_SHOOT_FRAME_H_INCLUDED
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

//  nijiのインクルード
#include "AppLib/include/Frame/AppFrameCellSuper.h"
#include "AppLib/include/Tool/AppToolTimeLimit.h"
#include "AppLib/include/Tool/ModelViewerSequence.h"

//  証明写真アプリ内のUViewプール
#include "App/TrainerPhoto/source/TrainerPhotoUIViewPool.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class AppToolTrainerIconRendering;
  class CharaSimpleModel;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(poke_3d)
GFL_NAMESPACE_BEGIN(model)

  class BaseCamera;

GFL_NAMESPACE_END(model)
GFL_NAMESPACE_END(poke_3d)

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( photo )

  /** モデル撮影フレーム */
  class TrainerPhotoModelShootFrame : public applib::frame::CellSuper, public TrainerPhotoLowRootViewInputDelegate
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(TrainerPhotoModelShootFrame);
  public:
    enum PushBtn
    {
      PUSH_BTN_NONE = 0,
      PUSH_BTN_CANCEL,
      PUSH_BTN_MODEL_SHOOT_RUN
    };

    struct FRAME_PARAM
    {
      struct IN_DATA
      {
        IN_DATA()
        {
          Reset();
        }

        void Reset()
        {
          pCharaModel     = NULL;
          pUIViewPool     = NULL;
          pCharaModelCam  = NULL;
          pTrainerIconRender  = NULL;

          bViewParamContinue  = false;
          trainerId = 0;
        }

        //  モデルオブジェクト
        app::tool::CharaSimpleModel*  pCharaModel;

        //  トレーナーアイコンレンダー
        app::tool::AppToolTrainerIconRendering* pTrainerIconRender;

        //  トレーナーアイコンのid
        u32 trainerId;

        //  UIViewプール
        TrainerPhotoUIViewPool* pUIViewPool;

        //  カメラ
        poke_3d::model::BaseCamera* pCharaModelCam;
        
        //  bViewParamContinue == true の時に使うViewの引継ぎデータ
        app::tool::ModelViewerSequence::START_PARAM continueViewParam;

        bool          bViewParamContinue;
      } in;

      struct OUT_DATA
      {
        OUT_DATA()
        {
          Reset();
        }

        void Reset()
        {
          pushBtn = PUSH_BTN_NONE;
        }

        PushBtn pushBtn;

        app::tool::ModelViewerSequence::START_PARAM viewParam;

      } out;
    };

    //------------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     */
    //------------------------------------------------------------------
    TrainerPhotoModelShootFrame();

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~TrainerPhotoModelShootFrame();

    //------------------------------------------------------------------
    /**
     * @brief	  動作に必要な引数を登録
     * @note
     *    必ずインスタンス生成の段階で呼び出さないと後の動作保障はなし
     */
    //------------------------------------------------------------------
    void Initialize( FRAME_PARAM* pFrameParam );

    //------------------------------------------------------------------
    /**
     * @brief	  PROCの初期化を行う関数
     * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
     */
    //------------------------------------------------------------------
    virtual applib::frame::Result InitFunc();

    //------------------------------------------------------------------
    /**
     * @brief	  PROCのメイン更新を行う関数
     * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
     */
    //------------------------------------------------------------------
    virtual applib::frame::Result UpdateFunc();

    //------------------------------------------------------------------
    /**
     * @brief	  PROCのメイン描画を行う関数
     *          純粋仮想ではありません。
     */
    //------------------------------------------------------------------
    virtual void DrawFunc(gfl2::gfx::CtrDisplayNo no);

    //------------------------------------------------------------------
    /**
     * @brief	  PROCの終了を行う関数
     * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
     */
    //------------------------------------------------------------------
    virtual applib::frame::Result EndFunc();

    //------------------------------------------------------------------
    /**
      * @brief   キー入力通知用リスナ
      * @param[in] pButton  ボタン入力の情報
      * @param[in] pKey     キー入力の情報
      * @param[in] pStick   スティック入力の情報
      * @return 入力処理を続けるかを返す
      */
    //------------------------------------------------------------------
    virtual void OnTrainerPhotoLowRootViewKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  private:
    //  変数初期化
    void _Clear()
    {
      m_pFrameParam = NULL;
    }

    FRAME_PARAM*  m_pFrameParam;

    app::tool::ModelViewerSequence m_modelViewrSequence;
};

GFL_NAMESPACE_END( photo )
GFL_NAMESPACE_END( app )


#endif // TRAINER_PHOTO_MODEL_SHOOT_FRAME_H_INCLUDED

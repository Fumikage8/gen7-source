//=============================================================================
/**
 * @file	 TrainerPhotoShootConfirmFrame.h
 * @brief	 証明写真撮影確認フレーム
 * @author yuto_uchida
 * @date	 2015.05.19
 */
//=============================================================================
#ifndef TRAINER_PHOTO_SHOOT_CONFIRM_FRAME_H_INCLUDED
#define TRAINER_PHOTO_SHOOT_CONFIRM_FRAME_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include "AppLib/include/Frame/AppFrameCellSuper.h"
#include "AppLib/include/Tool/app_tool_MenuWindowListener.h"

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

  class TrainerPhotoUIViewPool;

  /** 撮影保存制御フレーム */
  class TrainerPhotoShootConfirmFrame : public applib::frame::CellSuper, app::tool::MenuWindowListener
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(TrainerPhotoShootConfirmFrame);

  public:
    enum PushBtn
    {
      PUSH_BTN_NONE = 0,
      PUSH_BTN_CANCEL,
      PUSH_BTN_SHOOT_SAVE
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
          pUIViewPool         = NULL;
          pCharaModel         = NULL;
          pTrainerIconRender  = NULL;
          pCharaModelCam      = NULL;
          trainerId = 0;
        }

        //  モデルオブジェクト
        app::tool::CharaSimpleModel*  pCharaModel;

        //  トレーナーアイコンレンダー
        app::tool::AppToolTrainerIconRendering* pTrainerIconRender;

        //  トレーナーアイコンのid
        u32 trainerId;

        //  カメラ
        poke_3d::model::BaseCamera* pCharaModelCam;

        //  UIViewプール
        TrainerPhotoUIViewPool* pUIViewPool;

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
      } out;
    };

    //------------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     */
    //------------------------------------------------------------------
    TrainerPhotoShootConfirmFrame();

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~TrainerPhotoShootConfirmFrame();

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

    //-----------------------------------------------------------------------------
    /**
     * @func    SelectMenu
     * @brief   MenuWindwo項目選択時のリスナー
     *
     * @param   id  メニュー項目のID
     * @note    app::tool::MenuWindowListenerのリスナ
     */
    //-----------------------------------------------------------------------------
    virtual void SelectMenu( u32 id );

  private:
    //  変数初期化
    void _Clear()
    {
      m_pFrameParam     = NULL;
      m_bEndMenuWindow  = false;
    }

    bool          m_bEndMenuWindow;
    FRAME_PARAM*  m_pFrameParam;
};

GFL_NAMESPACE_END( photo )
GFL_NAMESPACE_END( app )


#endif // TRAINER_PHOTO_SHOOT_CONFIRM_FRAME_H_INCLUDED

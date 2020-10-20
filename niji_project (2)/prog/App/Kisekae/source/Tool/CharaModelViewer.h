//======================================================================
/**
 * @file CharaModelViewer.h
 * @date 2015/10/13 13:31:00
 * @author uchida_yuto
 * @brief きせかえ用のモデルビュー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __APP_TOOL_CHARA_MODEL_VIEWER_HEADER_H_INCLUDED__
#define __APP_TOOL_CHARA_MODEL_VIEWER_HEADER_H_INCLUDED__
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <ui/include/gfl2_UI.h>

#include "AppLib/include/Tool/AppToolCharaSimpleModel.h"
#include "AppLib/include/Tool/ModelViewerSequence.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  class Heap;

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  //  前方宣言
  class TimerIconUIView;

  //  デリゲータ
  class CharaModelViewerDelegate
  {
  public:
    //  きせかえ終了通知
    virtual void CharaModelViewer_OnDressupEnd()  = 0;
  };

  class CharaModelViewer
  {
  public:
    struct INIT_DATA
    {
      INIT_DATA()
      {
        pFileManager  = NULL;
        charaModelArcId = gfl2::fs::ArcFile::ARCID_NULL;
        pCharaModelDressupArcIdList = NULL;
        pTimerIconBin = NULL;
        pCamera       = NULL;
        pUtilHeap     = NULL;

        modelCharaId  = 0;
        modelCharaAnimWaitId  = 0;
      };

      CharaSimpleModel::SETUP_PARAM   charaModelSetupParam;
      gfl2::fs::AsyncFileManager*     pFileManager;
      gfl2::fs::ArcFile::ARCID        charaModelArcId;
      s32*                            pCharaModelDressupArcIdList;

      u32                             modelCharaId;
      u32                             modelCharaAnimWaitId;

      poke_3d::model::DressUpParam    dressUpParam;

      //  カメラ
      poke_3d::model::BaseCamera*     pCamera;
      app::util::Heap*                pUtilHeap;

      void*                           pTimerIconBin;
    };

    CharaModelViewer( const INIT_DATA& rInitData );
    virtual ~CharaModelViewer();

    //  初期化
    bool InitFunc();

    //  終了
    bool EndFunc();

    //  更新
    void UpdateFunc();

    //  描画
    void DrawFunc( gfl2::gfx::CtrDisplayNo displayNo );

    //  デリゲーター設定
    void SetDelegate( CharaModelViewerDelegate* pDelegate )
    {
      m_pDelegate = pDelegate;
    }

    //  モデルの着せ替え設定
    bool StartDressupEvent( const poke_3d::model::DressUpParam& rDressUpParam );

    //  待機イベント開始
    void StartIdleingEvent();

    //  回転イベント開始
    void StartRotYEvent( const gfl2::math::Vector3& rRot );

    //  回転入力イベント開始
    //  デフォルトのViewパラメータ設定するか
    void StartRotYInputEvent( const bool bDefaultViewParam = true );

    //  お披露目イベント開始
    void StartShowEvent();

    //  現在モデルのきせかえパラメータを取得
    const poke_3d::model::DressUpParam& GetFirstCharaModelDressUpParam() const { return m_initData.dressUpParam; }

    //  現在モデルのきせかえパラメータを取得
    const poke_3d::model::DressUpParam& GetNowCharaModelDressUpParam() const { return m_pCharaModel->GetDressUpParam(); }

    //  モデルロード中か
    bool IsLoadModel() { return ( m_pCharaModel->IsReady() == false ); }

    //------------------------------------------------------------------
    /**
      * @brief   キー入力取得リスナ
      * @param[in] pButton  ボタン入力の情報
      * @param[in] pKey     キー入力の情報
      * @param[in] pStick   スティック入力の情報
      * @return 処理成功かどうか
      */
    //------------------------------------------------------------------
    bool NoticeKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

    //  着せ替えの組み合わせで問題ないか
    bool CheckDressUpCombination( const poke_3d::model::DressUpParam& rDressUpParam );

    //  モデルのきせかえリソースコア取得
    poke_3d::model::DressUpModelResourceManagerCore*  GetDressUpModelResourceManagerCore( const poke_3d::model::DressUpParam::Sex sex );

  private:
    enum State
    {
      STATE_IDLE  = 0,
      STATE_DRESS_UP,
      STATE_ROT,
      STATE_ROT_INPUT,
      STATE_SHOW
    };

    //  view開始
    void _StartViewer( const u32 zoomNum, const u32 zoomMax );

    //  デフォルトパラメータを設定
    void _UpdateDefaultViewerParam();

    void _Clear()
    {
      m_pCharaModel     = NULL;
      m_pTimerIconView  = NULL;
      m_pDelegate       = NULL;
      m_initSeq         = 0;
      m_endSeq          = 0;
      m_state           = STATE_IDLE;
      m_defaultCamFovy  = 0.0f;

      m_pModelViewrSequence = NULL;
      m_pCharaModelLightDrawEnv = NULL;
    }

    //  キャラモデル
    CharaSimpleModel* m_pCharaModel;

    //  タイマーアイコンView
    TimerIconUIView*  m_pTimerIconView;

    INIT_DATA         m_initData;

    State             m_state;
    gfl2::math::Vector3 m_addRot;

    u32               m_initSeq, m_endSeq;

    CharaModelViewerDelegate* m_pDelegate;

    gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pCharaModelLightDrawEnv;

    ModelViewerSequence* m_pModelViewrSequence;

    gfl2::math::Vector3 m_defaultCamPos;
    gfl2::math::Vector3 m_defaultModelPos;
    f32                 m_defaultCamFovy;
  };

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

#endif // __APP_TOOL_CHARA_MODEL_VIEWER_HEADER_H_INCLUDED__
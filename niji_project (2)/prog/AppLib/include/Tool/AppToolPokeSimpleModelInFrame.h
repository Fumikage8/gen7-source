//=============================================================================
/**
 * @file AppToolPokeSimpleModelInFrame.h
 * @brief ポケモン簡易モデル  インフレーム版
 * @author kawada_koji@gamefreak.co.jp
 * @date 2015.12.03
 * @note
 *      PokeSimpleModelの派生クラス。PokeSimpleModelにインフレーム機能を追加した。
 */
//=============================================================================
#if !defined( NIJI_PROJECT_APP_TOOL_POKE_SIMPLE_MODEL_INFRAME_H_INCLUDED )
#define NIJI_PROJECT_APP_TOOL_POKE_SIMPLE_MODEL_INFRAME_H_INCLUDED
#pragma once

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

#include <PokeTool/include/PokeModel.h>

//  nijiのインクルード
#include "AppLib/include/Util/AppRenderingManager.h"

#include "System/include/Camera/ModelViewerInFrame.h"
#include "System/include/Camera/InFrameCamera.h"
#include "AppLib/include/Tool/AppToolPokeSimpleModel.h"

//前方宣言
GFL_NAMESPACE_BEGIN(PokeTool)
  class PokeModelSystem;
GFL_NAMESPACE_END(PokeTool)

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )

  /** ポケモン簡易モデル  インフレーム版*/
  class PokeSimpleModelInFrame : public PokeSimpleModel
  {
    GFL_FORBID_COPY_AND_ASSIGN( PokeSimpleModelInFrame );  // コピーを禁止

    public:

      //---------------------------------------------------------------------------------
      // コンストラクタ・デストラクタ
      //---------------------------------------------------------------------------------
      /**
      * @brief コンストラクタ
      */
      PokeSimpleModelInFrame( const PokeSimpleModel::INIT_PARAM& param );

      /**
      * @brief デストラクタ
      */
      virtual ~PokeSimpleModelInFrame();

      //------------------------------------------------------------------
      /**
       * @brief   終了処理
       */
      //------------------------------------------------------------------
      virtual void Terminate();

      //  破棄可能か
      virtual bool IsDelete();

      //  表示有効設定
      virtual void SetVisible( const bool bFlg );

      /**
      * @brief 更新
      */
      virtual void Update();

      //---------------------------------------------------------------------------------
      // インフレーム
      //---------------------------------------------------------------------------------
      /**
      * @brief  インフレームシステム取得
      */
      System::Camera::CModelViewerInFrame* GetInFrameSys() { return &m_inFrameSys; }
      /**
      * @brief  インフレームカメラ取得
      */
      //System::Camera::InFrameCamera* GetInFrameCamera() const { return &m_inFrameCamera; }

      //------------------------------------------------------------------
      /**
       * @brief  インフレームを初期化する
       *
       * @note  引数に関しては
       * System::Camera::CModelViewerInFrame::Init(const EScreen eScreen, const int nLeft, const int nTop, const int nRight, const int nBottom, const ETurnDirection eTurnDir)
       * System::Camera::CModelViewerInFrame::SetDrawPosition(const EDrawPosition eDrawPosi)
       * で確認して下さい。
       */
      //------------------------------------------------------------------
      void InitInFrame(
          const System::Camera::CModelInFrame::EScreen              eScreen   ,
          const System::Camera::CModelViewerInFrame::ETurnDirection eTurnDir  = System::Camera::CModelViewerInFrame::TURN_DIRECTION_LEFT  ,
          const System::Camera::CModelViewerInFrame::EDrawPosition  eDrawPosi = System::Camera::CModelViewerInFrame::DRAW_POSITION_BOTTOM
      );
      //------------------------------------------------------------------
      /**
       * @brief  枠を設定する
       *
       * @note  引数に関しては
       * System::Camera::CModelViewerInFrame::SetFrame(const int nLeft, const int nTop, const int nRight, const int nBottom)
       * で確認して下さい。
       *
       * 上画面なら左上が(nLeft=0, nTop=0), 右下が(nRight=400, nBottom=240)となる。
       * 下画面なら左上が(nLeft=0, nTop=0), 右下が(nRight=320, nBottom=240)となる。
       */
      //------------------------------------------------------------------
      void SetInFrameFrame(const int nLeft, const int nTop, const int nRight, const int nBottom);
      //------------------------------------------------------------------
      /**
       * @brief  モデルを描画する位置を設定する
       *
       * @note  引数に関しては
       * System::Camera::CModelViewerInFrame::SetDrawPosition(const EDrawPosition eDrawPosi)
       * で確認して下さい。
       */
      //------------------------------------------------------------------
      void SetInFrameDrawPosition(const System::Camera::CModelViewerInFrame::EDrawPosition eDrawPosi);


    public:
      //---------------------------------------------------------------------------------
      // static関数
      //---------------------------------------------------------------------------------

      //------------------------------------------------------------------
      /**
       * @brief  インフレームで最適な設定をライトに施す
       *
       * @param[in]  pLightNode  設定を施されるライト
       *                         DrawEnvNodeから  pLightNode = pDrawEnvNode->GetLightSet(0)->GetLightNode(0);  というふうに取得できます
       * @param[in]  eTurnDir    ポケモンの向き
       */
      //------------------------------------------------------------------
      static void SetLightParamForInFrame(
        gfl2::renderingengine::scenegraph::instance::LightNode*   pLightNode,
        const System::Camera::CModelViewerInFrame::ETurnDirection eTurnDir
      );


    private:
      System::Camera::CModelViewerInFrame  m_inFrameSys;
      System::Camera::InFrameCamera        m_inFrameCamera;
  };

GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )

#endif // NIJI_PROJECT_APP_TOOL_POKE_SIMPLE_MODEL_INFRAME_H_INCLUDED 

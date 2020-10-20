//=============================================================================
/**
 * @file AppPokeSimpleModel.h
 * @brief ポケモン簡易モデル
 * @author yuto_uchida
 * @date 2015.07.23
 * @note
 *      PokeSimpleModelの派生クラス。PokeSimpleModelにインフレーム機能を追加した。
 */
//=============================================================================
#include "AppLib/include/Tool/AppToolPokeSimpleModelInFrame.h"

#include "PokeTool/include/PokeModelSystem.h"

// nijiのインクルード
#include <AppLib/include/Util/AppRenderingManager.h>

// poke_3d_libのインクルード
#include <model/include/gfl2_BaseCamera.h>

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )

  //-------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //-------------------------------------------------------------------------
  PokeSimpleModelInFrame::PokeSimpleModelInFrame( const INIT_PARAM& param )
    : PokeSimpleModel(param),
      m_inFrameSys(),
      m_inFrameCamera()
  {
    PokeSimpleModel::INIT_PARAM* initParam = PokeSimpleModel::GetInitParam();
    initParam->pRenderingManager->SetInFrameCamera( initParam->renderPlace, &m_inFrameCamera, initParam->layerNo );
  }

  //-------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //-------------------------------------------------------------------------
  PokeSimpleModelInFrame::~PokeSimpleModelInFrame()
  {
    PokeSimpleModel::INIT_PARAM* initParam = PokeSimpleModel::GetInitParam();
    initParam->pRenderingManager->UnsetInFrameCamera( initParam->renderPlace, initParam->layerNo );
  }

  //------------------------------------------------------------------
  /**
    * @brief   終了処理
    */
  //------------------------------------------------------------------
  void PokeSimpleModelInFrame::Terminate()
  {
    PokeSimpleModel::Terminate();
    m_inFrameSys.ResetModel();
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 破棄可能か
    */
  //-------------------------------------------------------------------------
  bool PokeSimpleModelInFrame::IsDelete()
  {
    bool ret = PokeSimpleModel::IsDelete();
    m_inFrameSys.ResetModel();
    return ret;
  }

  //------------------------------------------------------------------
  /**
    * @brief   表示有効設定
    */
  //------------------------------------------------------------------
  void PokeSimpleModelInFrame::SetVisible( const bool bFlg )
  {
    // インフレーム使用時の表示状態の変更は、インフレームにだけやってもらう

    // 変更前の表示状態
    bool inFrameVisible = m_inFrameSys.IsModelOnDisplay();
   
    // 基底クラスの表示設定
    PokeSimpleModel::SetVisible( bFlg );
    
    // 基底クラスの表示設定で実際にポケモンモデルの表示状態を変更していた場合は、インフレームの表示状態に戻しておく
    PokeSimpleModel::StateType state = PokeSimpleModel::GetState();
    if(state == PokeSimpleModel::STATE_TYPE_UPDATE)
    {
      PokeTool::PokeModel* pPokeModel = PokeSimpleModel::GetPokeModel();
      if(pPokeModel)
      {
        pPokeModel->SetVisible(inFrameVisible);
      }
    }

    // インフレームの表示設定
    m_inFrameSys.SetModelVisible( bFlg );
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 更新処理
    */
  //-------------------------------------------------------------------------
  void PokeSimpleModelInFrame::Update()
  {
    PokeSimpleModel::StateType state0 = PokeSimpleModel::GetState();
    PokeSimpleModel::Update();
    PokeSimpleModel::StateType state1 = PokeSimpleModel::GetState();

    if(state0 == PokeSimpleModel::STATE_TYPE_LOAD && state1 == PokeSimpleModel::STATE_TYPE_UPDATE)
    {
      m_inFrameSys.SetAndInitModel(PokeSimpleModel::GetPokeModel());
    }
    if(state0 == PokeSimpleModel::STATE_TYPE_END || state1 == PokeSimpleModel::STATE_TYPE_END)
    {
      m_inFrameSys.ResetModel();
    }

    m_inFrameSys.Update(&m_inFrameCamera);
  }

  //---------------------------------------------------------------------------------
  // インフレーム
  //---------------------------------------------------------------------------------
  void PokeSimpleModelInFrame::InitInFrame(
      const System::Camera::CModelInFrame::EScreen              eScreen   ,
      const System::Camera::CModelViewerInFrame::ETurnDirection eTurnDir  ,
      const System::Camera::CModelViewerInFrame::EDrawPosition  eDrawPosi
  )
  {
    m_inFrameSys.Init(eScreen, 0, 0, 240, 240, eTurnDir);
    m_inFrameSys.SetDrawPosition(eDrawPosi);
  }
  void PokeSimpleModelInFrame::SetInFrameFrame(const int nLeft, const int nTop, const int nRight, const int nBottom)
  {
    m_inFrameSys.SetFrame(nLeft, nTop, nRight, nBottom);
  }
  void PokeSimpleModelInFrame::SetInFrameDrawPosition(const System::Camera::CModelViewerInFrame::EDrawPosition eDrawPosi)
  {
    m_inFrameSys.SetDrawPosition(eDrawPosi);
  }

  //---------------------------------------------------------------------------------
  // static関数
  //---------------------------------------------------------------------------------
  void PokeSimpleModelInFrame::SetLightParamForInFrame(
    gfl2::renderingengine::scenegraph::instance::LightNode*   pLightNode,
    const System::Camera::CModelViewerInFrame::ETurnDirection eTurnDir
  )
  {
    if(eTurnDir == System::Camera::CModelViewerInFrame::TURN_DIRECTION_RIGHT)
    {
      pLightNode->SetDirection( gfl2::math::Vector3( 0.129392f, -0.865968f, -0.482898f) );
    }
    else
    {
      pLightNode->SetDirection( gfl2::math::Vector3(-0.129392f, -0.865968f, -0.482898f) );
    }

    pLightNode->SetColor( gfl2::math::Vector3(1.0f, 1.0f, 1.0f) );
  }


GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )

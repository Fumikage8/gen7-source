#if !defined(GFLIB2_ANIMATION_JOINTCONTROLLER_H_INCLUDED)
#define GFLIB2_ANIMATION_JOINTCONTROLLER_H_INCLUDED
#pragma once

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_MotionConfigInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <Animation/include/gfl2_JointFunctionBase.h>
#include <Animation/include/gfl2_InterestJointFunction.h>
#include <Animation/include/gfl2_SpringJointFunction.h>
#include <Animation/include/gfl2_IKJointFunction.h>

#include <Animation/include/gfl2_InterestController.h>
#include <Animation/include/gfl2_SpringController.h>
#include <Animation/include/gfl2_IKController.h>

namespace gfl2 { namespace animation {

/**
 * @brief ジョイントコントローラー：アニメーション以外のジョイント制御をまとめたクラス
 */
class JointController
{
public:

  /**
   * @brief コンストラクタ
   */
  JointController();

  /**
   * @brief デストラクタ
   */
  ~JointController();

  /**
   * @brief 初期化
   *
   * @param pModelNode モデルノード
   * @param pMotionConfigNode モーションコンフィグノード
   */
  void Initialize(gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelNode, gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode* pMotionConfigNode);

  /**
   * @brief 終了
   */
  void Finalize();

  /**
   * @brief 更新
   */
  void Update();

  /**
   * @brief 注目コントローラ取得
   *
   * @return 注目コントローラを返す
   */
  InterestController* GetInterestController()
  {
    return &m_InterestController;
  }

  /**
   * @brief バネコントローラ取得
   *
   * @return バネコントローラを返す
   */
  SpringController* GetSpringController()
  {
    return &m_SpringController;
  }

  /**
   * @brief IKコントローラ取得
   *
   * @return IKコントローラを返す
   */
  IKController* GetIKController()
  {
    return &m_IKController;
  }

private:

	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*           m_pModelNode;                 ///< 反映対象のモデルノード
	gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode*    m_pMotionConfigNode;          ///< モーション設定ノード

  InterestController                                                        m_InterestController;         ///< 注目コントローラー
  SpringController                                                          m_SpringController;           ///< バネコントローラー
  IKController                                                              m_IKController;               ///< IKコントローラー
};

}}

#endif
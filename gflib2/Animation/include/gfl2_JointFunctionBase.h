#ifndef GFLIB2_JOINT_FUNCTION_BASE_H_INCLUDED
#define GFLIB2_JOINT_FUNCTION_BASE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MotionConfigResourceNode.h>

namespace gfl2 { namespace animation {

/**
 * @brief ジョイントファンクション：基底クラス
 */
class JointFunctionBase
{
public:

  /**
   * @brief コンストラクタ
   */
  JointFunctionBase();

  /**
   * @brief デストラクタ
   */
  virtual ~JointFunctionBase();

  /**
   * @brief 初期化
   */
  void Initialize(gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelNode);

  /**
   * @brief 終了
   */
  void Finalize();

  /**
   * @brief ジョイントノード設定
   *
   * @param pJointNode ジョイントノード
   */
  void SetJointNode(gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pJointNode)
  {
    m_pJointNode = pJointNode;
  }

  /**
   * @brief ジョイントノード取得
   *
   * @return ジョイントノードを返す
   */
  gfl2::renderingengine::scenegraph::instance::JointInstanceNode* GetJointNode() const
  {
    return m_pJointNode;
  }

  /**
   * @brief ウェイト設定
   *
   * @param weight ウェイト (0.0 ~ 1.0)
   */
  void SetWeight(f32 weight)
  {
    m_Weight = weight;
  }

  /**
   * @brief ウェイト取得
   *
   * @return ウェイトを返す (0.0 ~ 1.0)
   */
  f32 GetWeight() const
  {
    return m_Weight;
  }

private:

	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*   m_pModelNode;             ///< 反映対象のモデルノード
  gfl2::renderingengine::scenegraph::instance::JointInstanceNode*   m_pJointNode;             ///< 反映対象のジョイントノード

  f32                                                               m_Weight;                 ///< ウェイト
};

}}

#endif
#ifndef GFLIB2_IK_CONTROLLER_H_INCLUDED
#define GFLIB2_IK_CONTROLLER_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_MotionConfigInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <Animation/include/gfl2_JointFunctionBase.h>
#include <Animation/include/gfl2_InterestJointFunction.h>
#include <Animation/include/gfl2_SpringJointFunction.h>
#include <Animation/include/gfl2_IKJointFunction.h>

#include <Animation/include/gfl2_InterestController.h>
#include <Animation/include/gfl2_SpringController.h>

namespace gfl2 { namespace animation {

/**
 * @brief IKコントローラー：アニメーション以外のジョイント制御をまとめたクラス
 */
class IKController
{
public:

  /**
   * @brief コンストラクタ
   */
  IKController();

  /**
   * @brief デストラクタ
   */
  ~IKController();

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
   * @brief コールバックを設定
   *
   * @param pCallback コールバック
   * @param pUserData ユーザーデータ
   */
  void SetCallback(IKCallback* pCallback, void* pUserData)
  {
    for (u32 i = 0; i < m_IKJointFunctionCount; ++i)
    {
      m_pIKJointFunctionList[i].SetCallback(pCallback, pUserData);
    }
  }

#if 0
  void SetIKTargetPosition(u32 index, const gfl2::math::Vector& targetPosition)
  {
    m_pIKJointFunctionList[index].SetTargetPosition(targetPosition);
  }
#endif

private:

	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*           m_pModelNode;                 ///< 反映対象のモデルノード
	gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode*    m_pMotionConfigNode;          ///< モーション設定リソースノード

  IKJointFunction*                                                          m_pIKJointFunctionList;       ///< IKジョイントファンクションリスト
  u32                                                                       m_IKJointFunctionCount;       ///< IKジョイントファンクション数
};

}}

#endif
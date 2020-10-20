#ifndef GFLIB2_IK_JOINT_FUNCTION_H_INCLUDED
#define GFLIB2_IK_JOINT_FUNCTION_H_INCLUDED

#include <Animation/include/gfl2_JointFunctionBase.h>

namespace gfl2 { namespace animation {

/**
 * @brief IKコールバック
 *          対象のジョイントノードを渡すので、ジョイントノードのワールド座標から地形の位置座標と法線を返してください。
 *
 * @param out_pPosition 出力用：位置座標
 * @param out_pNormal 出力用：法線
 * @param pJointNode ジョイントノード
 * @param pUserData ユーザーデータ
 *
 * @returns 位置座標と法線を取得できた場合はtrue, できない場合はfalse
 */
typedef bool (IKCallback)(math::Vector3* out_pPosition, math::Vector3* out_pNormal, const gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pJointNode, void* pUserData);

/**
 * @brief IKジョイントファンクション
 */
class IKJointFunction : public JointFunctionBase
{
public:

  /**
   * @brief コンストラクタ
   */
  IKJointFunction();

  /**
   * @brief デストラクタ
   */
  virtual ~IKJointFunction();

  /**
   * @brief 更新
   */
  virtual void Update();

  /**
   * @brief チェインジョイント設定
   *
   * @param pChainJoint チェインジョイント
   */
  void SetChainJoint(gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pChainJoint)
  {
    m_pChainJoint = pChainJoint;
  }

  /**
   * @brief チェインジョイント取得
   *
   * @return チェインジョイントを返す
   */
  gfl2::renderingengine::scenegraph::instance::JointInstanceNode* GetChainJoint() const
  {
    return m_pChainJoint;
  }

  /**
   * @brief エフェクタージョイント設定
   *
   * @param pEffectorJoint エフェクタージョイント
   */
  void SetEffectorJoint(gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pEffectorJoint)
  {
    m_pEffectorJoint = pEffectorJoint;
  }

  /**
   * @brief エフェクタージョイント取得
   *
   * @return エフェクタージョイントを返す
   */
  gfl2::renderingengine::scenegraph::instance::JointInstanceNode* GetEffectorJoint() const
  {
    return m_pEffectorJoint;
  }

  /**
   * @brief つま先ジョイント設定
   *
   * @param pTipJoint IK終了ジョイント
   */
  void SetTipJoint(gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pTipJoint)
  {
    m_pTipJoint = pTipJoint;
  }

  /**
   * @brief つま先ジョイント取得
   *
   * @return つま先ジョイントを返す
   */
  gfl2::renderingengine::scenegraph::instance::JointInstanceNode* GetTipJoint() const
  {
    return m_pTipJoint;
  }

  /**
   * @brief コールバックを設定
   *
   * @param pCallback コールバック
   * @param pUserData ユーザーデータ
   */
  void SetCallback(IKCallback* pCallback, void* pUserData)
  {
    m_pCallback = pCallback;
    m_pUserData = pUserData;
  }

  /**
   * @brief ジョイントの長さを計算
   */
  void CalcJointLength()
  {
    m_JointLength0 = this->GetJointNode()->GetDefaultTranslate().Length();
    m_JointLength1 = this->GetEffectorJoint()->GetDefaultTranslate().Length();
  }

private:

private:

  // mcnfから読み込む
  gfl2::renderingengine::scenegraph::instance::JointInstanceNode* m_pChainJoint;    ///< ももなど
  gfl2::renderingengine::scenegraph::instance::JointInstanceNode* m_pEffectorJoint; ///< かかとなど
  gfl2::renderingengine::scenegraph::instance::JointInstanceNode* m_pTipJoint;      ///< つま先など
  f32 m_JointLength0;                                                               ///< 開始点から中間点までの長さ
  f32 m_JointLength1;                                                               ///< 中間点から終了点までの長さ

  // 内部用
  IKCallback* m_pCallback;                                                          ///< コールバック
  void* m_pUserData;                                                                ///< ユーザーデータ

  // アプリからコールバックで設定してもらう
  gfl2::math::Vector3 m_TargetPosition;                                              ///< 目標位置
  gfl2::math::Vector3 m_TargetNormal;                                                ///< 目標法線
};

}}

#endif
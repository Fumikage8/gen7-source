#ifndef GFLIB2_SPRING_CONTROLLER_H_INCLUDED
#define GFLIB2_SPRING_CONTROLLER_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_MotionConfigInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <Animation/include/gfl2_JointFunctionBase.h>
#include <Animation/include/gfl2_SpringJointFunction.h>

#include <heap/include/gfl2_Heap.h>

namespace gfl2 { namespace animation {

/**
 * @brief バネコントローラー
 */
class SpringController
{
public:

  /**
   * @brief 制御位置
   */
  enum Side
  {
    SIDE_L,     // 左
    SIDE_R,     // 右

    SIDE_COUNT  // 総数
  };

public:

  /**
   * @brief コンストラクタ
   */
  SpringController();

  /**
   * @brief デストラクタ
   */
  ~SpringController();

  /**
   * @brief 初期化
   *
   * @param pHeap ヒープ
   * @param pModelNode モデルノード
   * @param pMotionConfigNode モーションコンフィグノード
   */
  void Initialize(gfl2::heap::HeapBase* pHeap, gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelNode, const gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode* pMotionConfigNode);

  /**
   * @brief 終了
   */
  void Finalize();

  /**
   * @brief 更新
   */
  void Update();

  /**
   * @brief バネ制御有効無効を設定
   *
   * @param enable true:バネ制御有効 false:バネ制御無効
   */
  void SetEnable(bool enable);

  /**
   * @brief バネ制御有効無効を取得
   *
   * @return true:バネ制御有効 false:バネ制御無効
   */
  bool IsEnable() const;

  /**
   * @brief 減衰設定
   *
   * @param lr SIDE_L or SIDE_R
   * @param damping 減衰
   */
  void SetDamping(Side lr, f32 damping);

  /**
   * @brief 減衰取得
   *
   * @param lr SIDE_L or SIDE_R
   * @return 減衰を返す
   */
  f32 GetDamping(Side lr) const;

  /**
   * @brief バネ定数設定
   *
   * @param lr SIDE_L or SIDE_R
   * @param stiffness バネ定数
   */
  void SetStiffness(Side lr, f32 stiffness);

  /**
   * @brief バネ定数取得
   *
   * @param lr SIDE_L or SIDE_R
   * @return バネ定数を返す
   */
  f32 GetStiffness(Side lr) const;

  /**
   * @brief 質量設定
   *
   * @param lr SIDE_L or SIDE_R
   * @param mass 質量
   */
  void SetMass(Side lr, f32 mass);

  /**
   * @brief 質量取得
   *
   * @param lr SIDE_L or SIDE_R
   * @return 質量を返す
   */
  f32 GetMass(Side lr) const;

  /**
   * @brief 重力設定
   *
   * @param lr SIDE_L or SIDE_R
   * @param gravity 重力
   */
  void SetGravity(Side lr, f32 gravity);

  /**
   * @brief 重力取得
   *
   * @param lr SIDE_L or SIDE_R
   * @return 重力を返す
   */
  f32 GetGravity(Side lr) const;

  /**
   * @brief Ｙ軸回転だけ行う設定
   *
   * @param lr SIDE_L or SIDE_R
   * @param flag Ｙ軸回転だけならtrue, 全て回転させるならfalse
   */
  void SetRotOnlyY(Side lr, b32 flag);

  /**
   * @brief Ｙ軸回転だけ行うか？
   *
   * @param lr SIDE_L or SIDE_R
   * @return Ｙ軸回転だけならtrue, 全て回転させるならfalse
   */
  b32 IsRotOnlyY(Side lr) const;

  /**
   * @brief 外力設定
   *
   * @param lr SIDE_L or SIDE_R
   * @param externalForce 外力
   */
  void SetExternalForce(Side lr, const math::Vector3& externalForce);

  /**
   * @brief 外力取得
   *
   * @param lr SIDE_L or SIDE_R
   * @return 外力を返す
   */
  const math::Vector3 GetExternalForce(Side lr) const;

  /**
   * @brief ダウンフォース設定
   *
   * @param lr SIDE_L or SIDE_R
   * @param downForce ダウンフォース
   */
  void SetDownForce(Side lr, f32 downForce);

  /**
   * @brief ダウンフォース取得
   *
   * @param lr SIDE_L or SIDE_R
   *
   * @return ダウンフォースを返す
   */
  f32 GetDownForce(Side lr) const;

  /**
   * @brief パラメータのリセットリクエスト
   *
   * @param lr SIDE_L or SIDE_R
   */
  void RequestParameterReset(Side lr);

private:

	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*           m_pModelNode;                 ///< 反映対象のモデルノード
	const gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode*    m_pMotionConfigNode;    ///< モーション設定リソースノード

  SpringJointFunction*                                                      m_pSpringJointFunctionList;   ///< バネジョイントファンクションリスト
  u32                                                                       m_SpringJointFunctionCount;   ///< バネジョイントファンクション数
  SpringJointFunction*                                                      m_pSpringJointFunctionLR[SIDE_COUNT]; ///< バネジョイントファンクション：LR別
  b8                                                                        m_SpringEnable;               ///< バネ制御有効かどうかのフラグ
  b8                                                                        m_SpringEnableReq;            ///< バネ制御有効かどうかのフラグ：リクエスト用
};

}}

#endif
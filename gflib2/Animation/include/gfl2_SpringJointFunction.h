#ifndef GFLIB2_SPRING_JOINT_FUNCTION_H_INCLUDED
#define GFLIB2_SPRING_JOINT_FUNCTION_H_INCLUDED

#include <Animation/include/gfl2_JointFunctionBase.h>

namespace gfl2 { namespace animation {

/**
 * @brief バネジョイントファンクション
 */
class SpringJointFunction : public JointFunctionBase
{
public:

  /**
   * @brief コンストラクタ
   */
  SpringJointFunction();

  /**
   * @brief デストラクタ
   */
  virtual ~SpringJointFunction();

  /**
   * @brief 更新
   *
   * @param springEnable バネのシミュレーション有効フラグ
   */
  virtual bool Update(bool springEnable);

  /**
   * @brief 減衰設定
   *
   * @param damping 減衰
   */
  void SetDamping(f32 damping)
  {
    m_Damping = damping;
  }

  /**
   * @brief 減衰取得
   *
   * @return 減衰を返す
   */
  f32 GetDamping() const
  {
    return m_Damping;
  }

  /**
   * @brief バネ定数設定
   *
   * @param stiffness バネ定数
   */
  void SetStiffness(f32 stiffness)
  {
    m_Stiffness = stiffness;
  }

  /**
   * @brief バネ定数取得
   *
   * @return バネ定数を返す
   */
  f32 GetStiffness() const
  {
    return m_Stiffness;
  }

  /**
   * @brief 質量設定
   *
   * @param mass 質量
   */
  void SetMass(f32 mass)
  {
    m_Mass = mass;
  }

  /**
   * @brief 質量取得
   *
   * @return 質量を返す
   */
  f32 GetMass() const
  {
    return m_Mass;
  }

  /**
   * @brief 重力設定
   *
   * @param gravity 重力
   */
  void SetGravity(f32 gravity)
  {
    m_Gravity = gravity;
  }

  /**
   * @brief 重力取得
   *
   * @return 重力を返す
   */
  f32 GetGravity() const
  {
    return m_Gravity;
  }

  /**
   * @brief Ｙ軸回転だけ行う設定
   *
   * @param flag Ｙ軸回転だけならtrue, 全て回転させるならfalse
   */
  void SetRotOnlyY(b32 flag)
  {
    m_RotOnlyY = flag;
  }

  /**
   * @brief Ｙ軸回転だけ行うか？
   *
   * @return Ｙ軸回転だけならtrue, 全て回転させるならfalse
   */
  b32 IsRotOnlyY() const
  {
    return m_RotOnlyY;
  }

  /**
   * @brief 外力設定
   *
   * @param externalForce 外力
   */
  void SetExternalForce(const math::Vector3& externalForce)
  {
    m_ExternalForce = externalForce;
  }

  /**
   * @brief 外力取得
   *
   * @return 外力を返す
   */
  const math::Vector3& GetExternalForce() const
  {
    return m_ExternalForce;
  }

  /**
   * @brief ダウンフォース設定
   *
   * @param downForce ダウンフォース
   */
  void SetDownForce(f32 downForce)
  {
    m_DownForce = downForce;
  }

  /**
   * @brief ダウンフォース取得
   *
   * @return ダウンフォースを返す
   */
  f32 GetDownForce() const
  {
    return m_DownForce;
  }

  /**
   * @brief パラメータのリセットリクエスト
   */
  void RequestParameterReset()
  {
    m_ResetFlag = true;
  }

private:

private:

  // mcnfから読み込む
  f32 m_Damping;                  ///< 減衰
  f32 m_Stiffness;                ///< バネ定数
  f32 m_Mass;                     ///< 質量
  f32 m_Gravity;                  ///< 重力

  b32 m_RotOnlyY;                 ///< Y軸回転のみ

  // アプリから設定してもらう
  math::Vector3 m_ExternalForce;   ///< 外力
  f32 m_DownForce;                ///< 外力とは違う下向きの力
  b32 m_ResetFlag;                ///< リセットフラグ

  // 完全内部用
  math::Vector3 m_Position;        ///< 位置
  math::Vector3 m_Velocity;        ///< 速度
};

}}

#endif
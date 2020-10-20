#ifndef GFLIB2_JOINT_ANIMATION_SLOT_H_INCLUDED
#define GFLIB2_JOINT_ANIMATION_SLOT_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_JointInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <Animation/include/gfl2_AnimationSlotBase.h>

#include <Animation/include/gfl2_CurvePlayer.h>

namespace gfl2 { namespace animation {

/**
 * @brief ジョイントデータセット
 */
struct JointDataSet
{
  enum JointCurveIndex
  {
    SCALE_X,
    SCALE_Y,
    SCALE_Z,

    ROTATE_X,
    ROTATE_Y,
    ROTATE_Z,

    TRANSLATE_X,
    TRANSLATE_Y,
    TRANSLATE_Z,

    INDEX_COUNT,
  };

  JointDataSet() : 
    m_Enabled(true),
    m_IsBaseJoint(false),
    m_pAnimBody(NULL),
    m_HasQuatLnCurve(0)
  {
  }

  bool m_Enabled;       ///< 再生有効か？：部分アニメーション用
  bool m_IsBaseJoint;   ///< アニメーション共有で基準になるジョイントか？：おそらく通常は"Waist"

  const gfl2::renderingengine::scenegraph::resource::AnimationResourceNode::JointAnimBody* m_pAnimBody;
  gfl2::animation::CurvePlayer m_CurvePlayerList[INDEX_COUNT];

  // @attention メモリが厳しいようなら最適化
  struct CurvePlayerIndexList
  {
    u32 m_PlayerIndexList[INDEX_COUNT];
    u32 m_PlayerCount;
  };
  CurvePlayerIndexList m_StaticPlayerIndexList;
  CurvePlayerIndexList m_HermiteU16PlayerIndexList;
  CurvePlayerIndexList m_HermiteF32PlayerIndexList;
  u8 m_HasEulerCurve;
  u8 m_HasQuatLnCurve;
};

/**
 * @brief ジョイントアニメーションスロット
 */
class JointAnimationSlot : public AnimationSlotBase
{
public:

  /**
   * @brief コンストラクタ
   */
  JointAnimationSlot();

  /**
   * @brief デストラクタ
   */
  virtual ~JointAnimationSlot();

  /**
   * @brief アニメーションが含まれているか？
   *
   * @return アニメーションが含まれているならtrue, 含まれていないならfalse
   */
  virtual bool HasAnimation() const
  {
    if (m_pResourceNode == NULL)
    {
      return false;
    }
    if (m_pResourceNode->m_pJointAnimHeader == NULL)
    {
      return false;
    }

    return true;
  }

  /**
   * @brief 部分アニメーション設定
   *
   * @param pJointName ジョイント名
   * @param flag 有効ならtrue, 無効ならfalse
   */
  void SetPartialAnimation(const c8* pJointName, bool flag);

  /**
   * @brief 部分アニメーション設定
   *
   * @param jointIndex ジョイントインデックス
   * @param flag 有効ならtrue, 無効ならfalse
   */
  void SetPartialAnimation(u32 jointIndex, bool flag);

  /**
   * @brief 共有アニメーション設定
   *
   * @param pBaseJointName 基準ジョイント名：おそらく通常は"Waist"
   * @param flag 有効ならtrue, 無効ならfalse
   */
  void SetDivertAnimation(const c8* pBaseJointName, bool flag);

  /**
   * @brief 共有アニメーション設定
   *
   * @param baseJointIndex 基準ジョイントインデックス
   * @param flag 有効ならtrue, 無効ならfalse
   */
  void SetDivertAnimation(u32 baseJointIndex, bool flag);

  /**
   * @brief ジョイントインデックスからジョイントSRTを取得できるか確認
   *
   * @param jointIndex ジョイントインデックス
   * @return true:取得できる, false:取得できない
   */
  bool CanGetJointSRT(u32 jointIndex) const;

  /**
   * @brief ジョイントSRTを取得
   *
   * @param out_pJointSRT 出力用のバッファ
   * @param jointIndex ジョイントインデックス
   */
  void GetJointSRT(math::SRT* out_pJointSRT, u32 jointIndex);

  /**
   * @brief AABBの取得
   */
  const math::AABB& GetAABB() const
  {
    return m_AABB;
  }

  /**
   * @brief 歩き用座標を取得：ポケモン用の拡張データ
   *
   * @param out_pPosition 出力用のバッファ
   * @param frame 取得するフレーム数
   */
  void GetWalkPosition(math::Vector3* out_pPosition, f32 frame);

  /**
   * @brief 線形補間用に前回のSRTを保存
   */
  void SetOldSRT(u32 jointIndex, const gfl2::math::SRT& oldSRT);
  /**
   * @brief 線形補間用に前回のSRTを取得
   */
  void GetOldSRT(u32 jointIndex, gfl2::math::SRT* out_pOldSRT);

private:

  /**
   * @brief 派生クラスでの初期化
   *
   * @param pHeap ヒープ
   */
  virtual void InitializeDerived(gfl2::heap::HeapBase* pHeap);

  /**
   * @brief 派生クラスでの終了
   */
  virtual void FinalizeDerived();

  /**
   * @brief 派生クラスでのアニメーションリソース設定
   *
   * @param pHeap ヒープ
   */
  virtual void SetAnimationResourceDerived(gfl2::heap::HeapBase* pHeap);

  /**
   * @brief ジョイントデータセット取得
   *
   * @param jointIndex ジョイント番号
   *
   * @return ジョイントデータセット
   */
  JointDataSet* GetJointDataSet(u32 jointIndex)
  {
    return &m_pJointDataSetList[jointIndex];
  }
  const JointDataSet* GetJointDataSet(u32 jointIndex) const
  {
    return &m_pJointDataSetList[jointIndex];
  }

private:

	JointDataSet* m_pJointDataSetList;                                            ///< ジョイントデータセット
  CurvePlayer   m_WalkCurvePositionY;                                           ///< 歩き用の位置カーブＹ
  CurvePlayer   m_WalkCurvePositionZ;                                           ///< 歩き用の位置カーブＺ

  bool m_DivertEnabled;                                                         ///< 共有有効か？
  f32 m_DivertScale;                                                            ///< 共有用スケール

  math::AABB m_AABB;                                                            ///< 再生中のモーションのAABB
};

}}

#endif
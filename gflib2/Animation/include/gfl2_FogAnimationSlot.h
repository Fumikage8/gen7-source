#ifndef GFLIB2_FOG_ANIMATION_SLOT_H_INCLUDED
#define GFLIB2_FOG_ANIMATION_SLOT_H_INCLUDED

//kawa20150514

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>
#include <Animation/include/gfl2_AnimationSlotBase.h>

#include <Animation/include/gfl2_CurvePlayer.h>

namespace gfl2 { namespace animation {

/**
 * @brief フォグデータセット
 */
struct FogDataSet
{
  enum FogCurveIndex
  {
    NEAR_LENGTH,
    FAR_LENGTH,
    COLOR_R,
    COLOR_G,
    COLOR_B,
    STRENGTH,

    INDEX_COUNT
  };

  FogDataSet() : 
    m_pAnimBody(NULL)
  {
  }

  const gfl2::renderingengine::scenegraph::resource::AnimationResourceNode::EnvFogAnimBody* m_pAnimBody;
  gfl2::animation::CurvePlayer m_CurvePlayerList[INDEX_COUNT];
};

/**
 * @brief フォグアニメーションスロット
 */
class FogAnimationSlot : public AnimationSlotBase
{
public:

  /**
   * @brief コンストラクタ
   */
  FogAnimationSlot();

  /**
   * @brief デストラクタ
   */
  virtual ~FogAnimationSlot();

  /**
   * @brief フォグを適用
   *
   * @param out_pValueList 出力する値リスト
   * @param fogIndex 適用するフォグインデックス
   */
  void ApplyFog(f32* out_pValueList, u32 fogIndex);

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
    if (m_pResourceNode->m_pEnvFogAnimHeader == NULL)
    {
      return false;
    }

    return true;
  }

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

private:

	FogDataSet* m_pFogDataSetList[gfl2::renderingengine::scenegraph::instance::DrawEnvNode::FogMax];        ///< フォグデータセット  // 配列のインデックスとfogNoが一致するようにしておく。
};

}}

#endif

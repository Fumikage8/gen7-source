#ifndef GFLIB2_VISIBILITY_ANIMATION_SLOT_H_INCLUDED
#define GFLIB2_VISIBILITY_ANIMATION_SLOT_H_INCLUDED

#include <Animation/include/gfl2_AnimationSlotBase.h>

namespace gfl2 { namespace animation {

/**
 * @brief ビジビリティデータセット
 */
struct VisibilityDataSet
{
  VisibilityDataSet() : 
    m_pAnimBody(NULL)
  {
  }

  const gfl2::renderingengine::scenegraph::resource::AnimationResourceNode::VisibilityAnimBody* m_pAnimBody;
};

/**
 * @brief ビジビリティアニメーションスロット
 */
class VisibilityAnimationSlot : public AnimationSlotBase
{
public:

  /**
   * @brief コンストラクタ
   */
  VisibilityAnimationSlot();

  /**
   * @brief デストラクタ
   */
  virtual ~VisibilityAnimationSlot();

  /**
   * @brief ビジビリティを適用
   *
   * @param meshIndex メッシュインデックス
   */
  void ApplyVisibility(u32 meshIndex);

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
    if (m_pResourceNode->m_pVisibilityAnimHeader == NULL)
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

	VisibilityDataSet* m_pVisibilityDataSetList;                                            ///< ビジビリティデータセット
};

}}

#endif
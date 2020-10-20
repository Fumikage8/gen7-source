#ifndef GFLIB2_BLEND_SHAPE_ANIMATION_SLOT_H_INCLUDED
#define GFLIB2_BLEND_SHAPE_ANIMATION_SLOT_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_BlendShapeAnimationResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <Animation/include/gfl2_AnimationSlotBase.h>

namespace gfl2 { namespace animation {

/**
 * @brief ブレンドシェイプデータセット
 */
struct BlendShapeDataSet
{
  BlendShapeDataSet() : 
    m_pBlendShapeAnimationResourceNode(NULL)
  {
  }

  gfl2::renderingengine::scenegraph::resource::BlendShapeAnimationResourceNode*     m_pBlendShapeAnimationResourceNode;
};

/**
 * @brief ブレンドシェイプアニメーションスロット
 */
class BlendShapeAnimationSlot : public AnimationSlotBase
{
public:

  /**
   * @brief コンストラクタ
   */
  BlendShapeAnimationSlot();

  /**
   * @brief デストラクタ
   */
  virtual ~BlendShapeAnimationSlot();

  /**
   * @brief ブレンドシェイプを適用
   *
   * @param meshIndex メッシュインデックス
   */
  void ApplyBlendShape(u32 meshIndex);

  /**
   * @brief アニメーションが含まれているか？
   *
   * @return アニメーションが含まれているならtrue, 含まれていないならfalse
   */
  virtual bool HasAnimation() const
  {
    // @attention 今回ブレンドシェイプ未対応
    return false;
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

  /**
   * @brief ブレンドシェイプアニメーションリソース巡回
   */
	void Visit(gfl2::renderingengine::scenegraph::resource::BlendShapeAnimationResourceNode* pResourceNode);

private:

	class BlendShapeAnimationResourceNodeVisitor
	{
	public:
		void SetThisPtr(BlendShapeAnimationSlot* pThis) { m_pThis = pThis; }

		inline void Visit( gfl2::renderingengine::scenegraph::resource::BlendShapeAnimationResourceNode* pNode )
		{
			m_pThis->Visit( pNode );
		}

	private:
		BlendShapeAnimationSlot* m_pThis;
	};

private:

	BlendShapeDataSet* m_pVisibilityDataSetList;                                            ///< ビジビリティデータセット
};

}}

#endif
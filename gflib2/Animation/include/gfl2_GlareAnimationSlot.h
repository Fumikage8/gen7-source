#ifndef GFLIB2_GLARE_ANIMATION_SLOT_H_INCLUDED
#define GFLIB2_GLARE_ANIMATION_SLOT_H_INCLUDED

//kawa20150514

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>
#include <Animation/include/gfl2_AnimationSlotBase.h>

#include <Animation/include/gfl2_CurvePlayer.h>

namespace gfl2 { namespace animation {

/**
 * @brief グレア(ブルーム)データセット
 */
struct GlareDataSet
{
  enum GlareCurveIndex
  {
    GLARE_STRENGTH,
    GLARE_RANGE,
    GLARE_INTENSITY,
    GLARE_WEIGHT_R,
    GLARE_WEIGHT_G,
    GLARE_WEIGHT_B,

    INDEX_COUNT
  };

  GlareDataSet() : 
    m_pAnimBody(NULL)
  {
  }

  const gfl2::renderingengine::scenegraph::resource::AnimationResourceNode::EnvGlareAnimBody* m_pAnimBody;
  gfl2::animation::CurvePlayer m_CurvePlayerList[INDEX_COUNT];
};

/**
 * @brief グレア(ブルーム)アニメーションスロット
 */
class GlareAnimationSlot : public AnimationSlotBase
{
public:

  /**
   * @brief コンストラクタ
   */
  GlareAnimationSlot();

  /**
   * @brief デストラクタ
   */
  virtual ~GlareAnimationSlot();

  /**
   * @brief グレア(ブルーム)を適用
   *
   * @param out_pValueList 出力する値リスト
   * @note  グレア(ブルーム)は1個しかないのでインデックスは必要ない。
   */
  void ApplyGlare(f32* out_pValueList);

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
    if (m_pResourceNode->m_pEnvGlareAnimHeader == NULL)
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

	GlareDataSet* m_pGlareDataSet;        ///< グレア(ブルーム)データセット  // 1個のDrawEnvNode内には、1個のグレア(ブルーム)しかない。
};

}}

#endif

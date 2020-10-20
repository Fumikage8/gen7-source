#ifndef GFLIB2_MATERIAL_ANIMATION_SLOT_H_INCLUDED
#define GFLIB2_MATERIAL_ANIMATION_SLOT_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_MaterialInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <Animation/include/gfl2_AnimationSlotBase.h>
#include <Animation/include/gfl2_CurvePlayer.h>

namespace gfl2 { namespace animation {

/**
 * @brief マテリアルデータセット
 */
struct MaterialDataSet
{
  enum UvCurveIndex
  {
    UV_SCALE_U,
    UV_SCALE_V,

    UV_ROTATE,

    UV_TRANSLATE_U,
    UV_TRANSLATE_V,

    UV_INDEX_COUNT
  };

  enum ColorCurveIndex
  {
    COLOR_R,
    COLOR_G,
    COLOR_B,
    COLOR_A,

    COLOR_INDEX_COUNT
  };

  static const u32 ANIMATION_TEXTURE_SLOT_COUNT = 3;  // アニメーションする可能性があるのはスロット０〜２まで
  static const u32 ANIMATION_COLOR_COUNT = 6;         // コンスタントカラーは０〜５まで

  struct SlotData
  {
    const gfl2::renderingengine::scenegraph::resource::AnimationResourceNode::UvAnimBody* m_pUvAnimBody;
    const gfl2::renderingengine::scenegraph::resource::AnimationResourceNode::TexturePatternAnimBody* m_pTexturePatternAnimBody;
    const gfl2::renderingengine::scenegraph::resource::AnimationResourceNode::TexturePatternAnimBody::Key* m_pTexturePatternLastKey;
    const gfl2::renderingengine::scenegraph::resource::TextureResourceNode* m_pTexturePatternLastTexture;
    u32 m_TexturePatternLastKeyNextFrame;
    gfl2::animation::CurvePlayer m_UvCurvePlayerList[UV_INDEX_COUNT];
  };

  struct ColorData
  {
    const gfl2::renderingengine::scenegraph::resource::AnimationResourceNode::ShaderAnimBody* m_pColorAnimBody;
    gfl2::animation::CurvePlayer m_ColorCurvePlayerList[COLOR_INDEX_COUNT];
  };

  MaterialDataSet() : 
    m_UvAnimSlotCount(0),
    m_TexturePatternAnimSlotCount(0),
    m_ColorAnimCount(0)
  {
  }

  SlotData m_SlotDataList[ANIMATION_TEXTURE_SLOT_COUNT];
  u32 m_UvAnimSlotCount;
  u32 m_TexturePatternAnimSlotCount;

  ColorData m_ColorDataList[ANIMATION_COLOR_COUNT];
  u32 m_ColorAnimCount;
};

/**
 * @brief マテリアルアニメーションスロット
 */
class MaterialAnimationSlot : public AnimationSlotBase
{
public:

  /**
   * @brief コンストラクタ
   */
  MaterialAnimationSlot();

  /**
   * @brief デストラクタ
   */
  virtual ~MaterialAnimationSlot();

  /**
   * @brief マテリアルを適用
   *
   * @param materialIndex マテリアルインデックス
   */
  void ApplyMaterial(u32 materialIndex);

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
    if ((m_pResourceNode->m_pUvAnimHeader == NULL)
      && (m_pResourceNode->m_pTexturePatternAnimHeader == NULL)
      && (m_pResourceNode->m_pShaderAnimHeader == NULL))
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

  /**
   * @brief ＵＶアニメーションセットアップ
   *
   * @param pHeap ヒープ
   */
  void SetupUvAnimation(gfl2::heap::HeapBase* pHeap);

  /**
   * @brief テクスチャパターンアニメーションセットアップ
   *
   * @param pHeap ヒープ
   */
  void SetupTexturePatternAnimation(gfl2::heap::HeapBase* pHeap);

  /**
   * @brief シェーダーアニメーションセットアップ
   *
   * @param pHeap ヒープ
   */
  void SetupShaderAnimation(gfl2::heap::HeapBase* pHeap);

private:

  /**
   * @brief 名前からマテリアルデータセットを検索して作成
   *
   * @param pHeap ヒープ
   */
  MaterialDataSet* CreateMaterialDataSet(gfl2::heap::HeapBase* pHeap, const gfl2::renderingengine::scenegraph::resource::AnimationResourceNode::Name* pName);

private:

	MaterialDataSet** m_pMaterialDataSetList;                                                         ///< マテリアルデータセット
  u32 m_MaterialDataSetCount;                                                                       ///< マテリアルデータセット数
  const gfl2::renderingengine::scenegraph::resource::AnimationResourceNode::Name* m_pTextureIdList; ///< テクスチャＩＤリスト
};

}}

#endif
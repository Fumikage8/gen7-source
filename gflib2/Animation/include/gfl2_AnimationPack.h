#ifndef GFLIB2_ANIMATION_PACK_H_INCLUDED
#define GFLIB2_ANIMATION_PACK_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>

#include <heap/include/gfl2_Heap.h>

namespace gfl2 { namespace animation {

/**
 * @brief アニメーションパック
 */
class AnimationPack
{
public:

  /**
   * @brief コンストラクタ
   */
  AnimationPack();

  /**
   * @brief デストラクタ
   */
  ~AnimationPack();

  /**
   * @brief データ読み込み
   *
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   * @param pHeap 読み込みに使用するヒープ
   * @param pData 読み込むデータ
   */
  void LoadData(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pHeap, c8* pData);

  /**
   * @brief データ読み込み済みか？
   */
  bool IsLoaded() const;

  /**
   * @brief データ開放
   */
  void Unload();

  /**
   * @brief 含まれるリソース数を返す
   *
   * @return 含まれるリソース数、読み込み完了前なら0を返す
   */
  u32 GetResourceCount() const
  {
    return m_ResourceCount;
  }

  /**
   * @brief リソースノードを返す
   *
   * @return リソースノードを返す
   */
  gfl2::renderingengine::scenegraph::resource::ResourceNode* GetResourceNode(u32 no)
  {
    GFL_ASSERT(no < m_ResourceCount);
    return m_ppResourceNodeList[no];
  }

  /**
   * @brief リソースノードを返す
   *
   * @return リソースノードを返す
   */
  const gfl2::renderingengine::scenegraph::resource::ResourceNode* GetResourceNode(u32 no) const
  {
    GFL_ASSERT(no < m_ResourceCount);
    return m_ppResourceNodeList[no];
  }

private:

  gfl2::renderingengine::scenegraph::resource::ResourceNode** m_ppResourceNodeList;   ///< 生成されたリソースノードリスト
  u32                                                         m_ResourceCount;        ///< リソース数
};

}}

#endif // #ifndef GFLIB2_ANIMATION_PACK_H_INCLUDED

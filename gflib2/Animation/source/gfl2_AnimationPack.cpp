#include <Animation/include/gfl2_AnimationPack.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>

using namespace gfl2;
using namespace gfl2::renderingengine;

namespace gfl2 { namespace animation { 

/**
 * @brief コンストラクタ
 */
AnimationPack::AnimationPack() :
  m_ppResourceNodeList(NULL),
  m_ResourceCount(0)
{
}

/**
 * @brief デストラクタ
 */
AnimationPack::~AnimationPack()
{
  this->Unload();
}

/**
 * @brief データ読み込み
 *
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param pHeap 読み込みに使用するヒープ
 * @param pData 読み込むデータ
 */
void AnimationPack::LoadData(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pHeap, c8* pData)
{
  if (pData == NULL)
  {
    return;
  }

  c8* pTopPtr = pData;

  // ファイル数
  std::memcpy(&m_ResourceCount, pData, sizeof(m_ResourceCount));
  pData += sizeof(m_ResourceCount);

  // リソースリスト作成
  m_ppResourceNodeList = GFL_NEW_ARRAY(pHeap) scenegraph::resource::ResourceNode*[m_ResourceCount];

  for (u32 i = 0; i < m_ResourceCount; ++i)
  {
    u32 offset = 0;
    std::memcpy(&offset, pData, sizeof(offset));
    pData += sizeof(offset);

    if (offset == 0)
    {
      m_ppResourceNodeList[i] = NULL;
      continue;
    }

    scenegraph::resource::GfBinaryMotData animationResourceData;
    animationResourceData.SetMotionData(pTopPtr + offset + 4);

    m_ppResourceNodeList[i] = scenegraph::resource::ResourceManager::CreateResourceNodeIndex(pGfxAllocator, &animationResourceData, 0);
  }
}

/**
 * @brief データ読み込み済みか？
 */
bool AnimationPack::IsLoaded() const
{
  return m_ppResourceNodeList != NULL;
}

/**
 * @brief データ開放
 */
void AnimationPack::Unload()
{
  for (u32 i = 0; i < m_ResourceCount; ++i)
  {
    GFL_SAFE_DELETE(m_ppResourceNodeList[i]);
  }
  GFL_SAFE_DELETE_ARRAY(m_ppResourceNodeList);
  m_ResourceCount = 0;
}

}}
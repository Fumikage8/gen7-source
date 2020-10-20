#include <Animation/include/gfl2_AnimationPackList.h>

namespace gfl2 { namespace animation { 

/**
 * @brief コンストラクタ
 */
AnimationPackList::AnimationPackList() :
  m_pAnimationPackList(NULL),
  m_AnimationPackCount(0)
{
}

/**
 * @brief デストラクタ
 */
AnimationPackList::~AnimationPackList()
{
  this->Finalize();
}

/**
 * @brief 初期化
 *
 * @param pHeap メモリ確保に使用するヒープ
 * @param packCount 確保するパック数
 */
void AnimationPackList::Initialize(gfl2::heap::HeapBase* pHeap, u32 packCount)
{
  this->Finalize();
  m_pAnimationPackList = GFL_NEW_ARRAY(pHeap) AnimationPack[packCount];
  m_AnimationPackCount = packCount;
}

/**
 * @brief 終了処理
 */
void AnimationPackList::Finalize()
{
  this->UnloadAll();
  GFL_SAFE_DELETE_ARRAY(m_pAnimationPackList);
  m_AnimationPackCount = 0;
}

/**
 * @brief データ読み込み
 *
 * @param packNo パック番号
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param pHeap 読み込みに使用するヒープ
 * @param pData 読み込むデータ
 */
void AnimationPackList::LoadData(u32 packNo, gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pHeap, c8* pData)
{
  this->GetAnimationPack(packNo)->LoadData(pGfxAllocator, pHeap, pData);
}

/**
 * @brief データ読み込み済みか？
 *
 * @param packNo パック番号
 */
bool AnimationPackList::IsLoaded(u32 packNo) const
{
  return this->GetAnimationPack(packNo)->IsLoaded();
}

/**
 * @brief データ開放
 *
 * @param packNo パック番号
 */
void AnimationPackList::Unload(u32 packNo)
{
  this->GetAnimationPack(packNo)->Unload();
}

/**
 * @brief 全データ開放
 */
void AnimationPackList::UnloadAll()
{
  for (u32 i = 0; i < m_AnimationPackCount; ++i)
  {
    this->Unload(i);
  }
}

/**
 * @brief 含まれるリソース数を返す
 *
 * @param packNo パック番号
 *
 * @return 含まれるリソース数、読み込み完了前なら0を返す
 */
u32 AnimationPackList::GetResourceCount(u32 packNo) const
{
  return this->GetAnimationPack(packNo)->GetResourceCount();
}

/**
 * @brief リソースノードを返す
 *
 * @param packNo パック番号
 * @param animationNo アニメーション番号
 *
 * @return リソースノードを返す
 */
gfl2::renderingengine::scenegraph::resource::ResourceNode* AnimationPackList::GetResourceNode(u32 packNo, u32 animationNo)
{
  return this->GetAnimationPack(packNo)->GetResourceNode(animationNo);
}

/**
 * @brief リソースノードを返す
 *
 * @param packNo パック番号
 * @param animationNo アニメーション番号
 *
 * @return リソースノードを返す
 */
const gfl2::renderingengine::scenegraph::resource::ResourceNode* AnimationPackList::GetResourceNode(u32 packNo, u32 animationNo) const
{
  return this->GetAnimationPack(packNo)->GetResourceNode(animationNo);
}

/**
 * @brief アニメーションパックの取得
 *
 * @param packNo パック番号
 *
 * @return アニメーションパックを返す、無効なパック番号ならNULL
 */
AnimationPack* AnimationPackList::GetAnimationPack(u32 packNo)
{
  GFL_ASSERT(packNo < m_AnimationPackCount);
  return &m_pAnimationPackList[packNo];
}

/**
 * @brief アニメーションパックの取得
 *
 * @param packNo パック番号
 *
 * @return アニメーションパックを返す、無効なパック番号ならNULL
 */
const AnimationPack* AnimationPackList::GetAnimationPack(u32 packNo) const
{
  GFL_ASSERT(packNo < m_AnimationPackCount);
  return &m_pAnimationPackList[packNo];
}

}}
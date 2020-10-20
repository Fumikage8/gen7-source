#ifndef GFLIB2_ANIMATION_PACK_LIST_H_INCLUDED
#define GFLIB2_ANIMATION_PACK_LIST_H_INCLUDED

#include <Animation/include/gfl2_AnimationPack.h>

namespace gfl2 { namespace animation {

/**
 * @brief アニメーションパックリスト：複数アニメーションパック用にリストにしたもの
 */
class AnimationPackList
{
public:

  /**
   * @brief コンストラクタ
   */
  AnimationPackList();

  /**
   * @brief デストラクタ
   */
  ~AnimationPackList();

  /**
   * @brief 初期化
   *
   * @param pHeap メモリ確保に使用するヒープ
   * @param packCount 確保するパック数
   */
  void Initialize(gfl2::heap::HeapBase* pHeap, u32 packCount);

  /**
   * @brief 終了処理
   */
  void Finalize();

  /**
   * @brief アニメーションパック数を取得する
   *
   * @return アニメーションパック数を返す
   */
  u32 GetAnimationPackCount() const
  {
    return m_AnimationPackCount;
  }

  /**
   * @brief データ読み込み
   *
   * @param packNo パック番号
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   * @param pHeap 読み込みに使用するヒープ
   * @param pData 読み込むデータ
   */
  void LoadData(u32 packNo, gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pHeap, c8* pData);

  /**
   * @brief データ読み込み済みか？
   *
   * @param packNo パック番号
   */
  bool IsLoaded(u32 packNo) const;

  /**
   * @brief データ開放
   *
   * @param packNo パック番号
   */
  void Unload(u32 packNo);

  /**
   * @brief 全データ開放
   */
  void UnloadAll();

  /**
   * @brief 含まれるリソース数を返す
   *
   * @param packNo パック番号
   *
   * @return 含まれるリソース数、読み込み完了前なら0を返す
   */
  u32 GetResourceCount(u32 packNo) const;

  /**
   * @brief リソースノードを返す
   *
   * @param packNo パック番号
   * @param animationNo アニメーション番号
   *
   * @return リソースノードを返す
   */
  gfl2::renderingengine::scenegraph::resource::ResourceNode* GetResourceNode(u32 packNo, u32 animationNo);

  /**
   * @brief リソースノードを返す
   *
   * @param packNo パック番号
   * @param animationNo アニメーション番号
   *
   * @return リソースノードを返す
   */
  const gfl2::renderingengine::scenegraph::resource::ResourceNode* GetResourceNode(u32 packNo, u32 animationNo) const;

  /**
   * @brief アニメーションパックの取得
   *
   * @param packNo パック番号
   *
   * @return アニメーションパックを返す、無効なパック番号ならNULL
   */
  AnimationPack* GetAnimationPack(u32 packNo);

  /**
   * @brief アニメーションパックの取得
   *
   * @param packNo パック番号
   *
   * @return アニメーションパックを返す、無効なパック番号ならNULL
   */
  const AnimationPack* GetAnimationPack(u32 packNo) const;

private:

  AnimationPack* m_pAnimationPackList;    ///< アニメーションパックのリスト
  u32 m_AnimationPackCount;               ///< アニメーションパック数
};

}}

#endif // #ifndef GFLIB2_ANIMATION_PACK_LIST_H_INCLUDED

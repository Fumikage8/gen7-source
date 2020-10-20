#if !defined(POKE_3D_LIB_MODEL_BASE_MODEL_RESOURCE_UNIT_H_INCLUDED)
#define POKE_3D_LIB_MODEL_BASE_MODEL_RESOURCE_UNIT_H_INCLUDED

#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <heap/include/gfl2_Heap.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <Animation/include/gfl2_AnimationPackList.h>

namespace poke_3d { namespace model {

/**
 * @brief リソースの管理単位：基底クラス
 */
class BaseModelResourceUnit
{
public:

  /**
   * @brief BinLinkerでこの順番にパックしている
   */
  enum BaseBinLinkerIndex
  {
    BASE_BIN_LINKER_INDEX_MODEL,           ///< モデル
    BASE_BIN_LINKER_INDEX_ANIMATION,       ///< アニメーション

    BASE_BIN_LINKER_INDEX_COUNT            ///< パック総数
  };

public:

  /**
   * @brief コンストラクタ
   */
  BaseModelResourceUnit();

  /**
   * @brief デストラクタ
   */
  virtual ~BaseModelResourceUnit();

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
   * @brief BinLinkerにパックされているデータ数を取得
   *
   * @param pFileManager ファイルマネージャ
   *
   * @return BinLinkerにパックされているデータ数を返す、まだデータが読み込まれていないなら0
   */
  u32 GetBinLinkerDataCount(gfl2::fs::AsyncFileManager* pFileManager) const;

  /**
   * @brief BinLinkerにパックされているデータサイズを取得
   *
   * @param pFileManager ファイルマネージャ
   * @param index BinLinkerでパックした際のインデックス
   *
   * @return BinLinkerにパックされているデータサイズを返す、まだデータが読み込まれていないなら0
   */
  u32 GetBinLinkerDataSize(gfl2::fs::AsyncFileManager* pFileManager, u32 index) const;

  /**
   * @brief BinLinkerにパックされているデータを取得
   *
   * @param pFileManager ファイルマネージャ
   * @param index BinLinkerでパックした際のインデックス
   *
   * @return BinLinkerにパックされているデータを返す、まだデータが読み込まれていないならNULL、不正なインデックスならNULL
   */
  const void* GetBinLinkerData(gfl2::fs::AsyncFileManager* pFileManager, u32 index) const;

  //---------------------------------------------------------------------------------
  // ■ 静的データ

  /**
   * @brief 静的データ読み込み開始
   *
   * @param isAsync true:非同期読み込み false:同期読み込み
   * @param arcId アーカイブ番号
   * @param datId アーカイブ内のデータ番号
   * @param pFileHeap ファイルヒープ
   * @param pFileManager ファイルマネージャ
   */
  void LoadStaticData(bool isAsync, u32 arcId, u32 datId, gfl2::heap::HeapBase* pFileHeap, gfl2::fs::AsyncFileManager* pFileManager);

  /**
   * @brief 静的データポインタの設定
   *
   * @param void * pData  静的データポインタ
   */
  void LoadStaticData( void * pData );

  /**
   * @brief 静的データ非同期読み込み完了したか？
   *
   * @param pFileManager ファイルマネージャ
   *
   * @return 完了していたらtrue, まだならfalse
   */
  bool IsStaticDataLoaded(gfl2::fs::AsyncFileManager* pFileManager) const;

  /**
   * @brief 読み込んだ静的データの開放
   */
  void UnloadStaticData();

  /**
   * @brief 読み込んだ静的データの開放が可能か確認
   */
  bool CanUnloadStaticData() const;

  /**
   * @brief モデルリソースノードセットアップ
   *
   * @param pFileManager ファイルマネージャ
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   *
   * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
   */
  bool SetupModelResourceNode(gfl2::fs::AsyncFileManager* pFileManager, gfl2::gfx::IGLAllocator* pGfxAllocator);

  /**
   * @brief モデルリソースノード取得
   *
   * @return モデルリソースノードを返す。まだデータが読み込まれていないかセットアップ前ならNULL
   */
  gfl2::renderingengine::scenegraph::resource::ResourceNode* GetModelResourceNode();

  /**
   * @brief アニメーションパックリストセットアップ
   *
   * @param pFileManager ファイルマネージャ
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   * @param pFileHeap ファイルヒープ
   *
   * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
   */
  bool SetupAnimationPackList(gfl2::fs::AsyncFileManager* pFileManager, gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pFileHeap);

  /**
   * @brief アニメーションパックリスト取得
   *
   * @return アニメーションパックを返す。まだデータが読み込まれていないならNULL
   */
  gfl2::animation::AnimationPackList* GetAnimationPackList();

  //---------------------------------------------------------------------------------
  // ■ 動的データ

  /**
   * @brief 動的アニメーションデータ同期読み込み開始
   *
   * @param arcId アーカイブ番号
   * @param datId アーカイブ内のデータ番号
   * @param packId パック番号
   * @param pFileHeap ファイルヒープ
   * @param pFileManager ファイルマネージャ
   */
  void LoadDynamicAnimationDataSync(u32 arcId, u32 datId, u32 packId, gfl2::heap::HeapBase* pFileHeap, gfl2::fs::AsyncFileManager* pFileManager);

  /**
   * @brief 動的アニメーションデータ同期読み込み開始
   *
   * @param arcId アーカイブ番号
   * @param datId アーカイブ内のデータ番号
   * @param packId パック番号
   * @param pFileHeap ファイルヒープ
   * @param pFileManager ファイルマネージャ
   */
  void LoadDynamicAnimationDataAsync(u32 arcId, u32 datId, u32 packId, gfl2::heap::HeapBase* pFileHeap, gfl2::fs::AsyncFileManager* pFileManager);

  /**
   * @brief 動的アニメーションデータ非同期読み込み完了したか？
   *
   * @param packId パック番号
   * @param pFileManager ファイルマネージャ
   *
   * @return 完了していたらtrue, まだならfalse
   */
  bool IsDynamicAnimationDataLoaded(u32 packId, gfl2::fs::AsyncFileManager* pFileManager) const;

  /**
   * @brief 読み込んだ動的アニメーションデータのセットアップ
   *
   * @param packId パック番号
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   * @param pSetupHeap セットアップ用のヒープ
   */
  void SetupDynamicAnimationData(u32 packId, gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pSetupHeap);

  /**
   * @brief 読み込んだ動的アニメーションデータの開放
   *
   * @param packId パック番号
   */
  void UnloadDynamicAnimationData(u32 packId);

protected:

  /**
   * @brief 読み込んだ静的データの開放：派生クラス用
   */
  virtual void UnloadStaticDataDerived() = 0;

private:

  /**
   * @brief 静的データのファイル読み込みリクエスト作成
   */
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq MakeStaticDataLoadReq(u32 arcId, u32 datId, gfl2::heap::HeapBase* pFileHeap);

  /**
   * @brief 動的アニメーションデータのファイル読み込みリクエスト作成
   */
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq MakeDynamicAnimationDataLoadReq(u32 arcId, u32 datId, u32 packId, gfl2::heap::HeapBase* pFileHeap);

private:

  void* m_pStaticDataBuffer;                                                                  ///< 静的データの読み込み用バッファ
  b32   m_IsStaticDataBufferManaged;                                                          ///< StaticDataBufferメモリは自身の管理下か

  void** m_ppDynamicAnimationDataBufferList;                                                  ///< 動的アニメーションデータの読み込み用バッファリスト
  u32 m_DynamicAnimationDataCount;                                                            ///< 動的アニメーション数

  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pModelResourceNode;            ///< 読み込み用バッファから生成したモデルリソースノード
  gfl2::animation::AnimationPackList m_AnimationPackList;                                     ///< 読み込み用バッファから生成したアニメーションパックのリスト：複数パック対応
};

}}

#endif // POKE_3D_LIB_MODEL_BASE_MODEL_RESOURCE_UNIT_H_INCLUDED

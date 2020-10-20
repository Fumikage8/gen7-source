#include "gfl2_BaseModelResourceUnit.h"

#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMcnfData.h>

namespace poke_3d { namespace model {

/**
 * @brief コンストラクタ
 */
BaseModelResourceUnit::BaseModelResourceUnit() :
  m_pStaticDataBuffer(NULL),
  m_IsStaticDataBufferManaged(true),
  m_ppDynamicAnimationDataBufferList(NULL),
  m_DynamicAnimationDataCount(0),
  m_pModelResourceNode(NULL)
{
}

/**
 * @brief デストラクタ
 */
BaseModelResourceUnit::~BaseModelResourceUnit()
{
}

/**
 * @brief 初期化
 *
 * @param pHeap メモリ確保に使用するヒープ
 * @param packCount 確保するパック数
 */
void BaseModelResourceUnit::Initialize(gfl2::heap::HeapBase* pHeap, u32 packCount)
{
  m_AnimationPackList.Initialize(pHeap, packCount);

  // 動的アニメーションデータ用に管理用メモリ確保
  m_DynamicAnimationDataCount = packCount - 1; // 静的パック分を引く
  if (m_DynamicAnimationDataCount > 0)
  {
    m_ppDynamicAnimationDataBufferList = (void**)GflHeapAllocMemory(pHeap, sizeof(void*) * m_DynamicAnimationDataCount);
    for (u32 i = 0; i < m_DynamicAnimationDataCount; ++i)
    {
      m_ppDynamicAnimationDataBufferList[i] = NULL;
    }
  }
}

/**
 * @brief 終了処理
 */
void BaseModelResourceUnit::Finalize()
{
  this->UnloadStaticData();
  for (u32 i = 0; i < m_DynamicAnimationDataCount; ++i)
  {
    GflHeapSafeFreeMemory(m_ppDynamicAnimationDataBufferList[i]);
  }
  m_DynamicAnimationDataCount = 0;
  GflHeapSafeFreeMemory(m_ppDynamicAnimationDataBufferList);

  m_AnimationPackList.Finalize();
}


/**
 * @brief 静的データのファイル読み込みリクエスト作成
 */
gfl2::fs::AsyncFileManager::ArcFileLoadDataReq BaseModelResourceUnit::MakeStaticDataLoadReq(u32 arcId, u32 datId, gfl2::heap::HeapBase* pFileHeap)
{
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  req.arcId      = arcId;
  req.datId      = datId;
  req.ppBuf      = &m_pStaticDataBuffer;
  req.heapForBuf = pFileHeap;
  req.align      = 128; // テクスチャは128バイトアラインメント
  req.heapForReq = pFileHeap->GetLowerHandle();
  req.heapForCompressed = NULL;

  m_IsStaticDataBufferManaged = true;

  return req;
}

/**
 * @brief 動的アニメーションデータのファイル読み込みリクエスト作成
 */
gfl2::fs::AsyncFileManager::ArcFileLoadDataReq BaseModelResourceUnit::MakeDynamicAnimationDataLoadReq(u32 arcId, u32 datId, u32 packId, gfl2::heap::HeapBase* pFileHeap)
{
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  req.arcId      = arcId;
  req.datId      = datId;
  req.ppBuf      = &m_ppDynamicAnimationDataBufferList[packId - 1];
  req.heapForBuf = pFileHeap;
  req.align      = 4;
  req.heapForReq = pFileHeap->GetLowerHandle();
  req.heapForCompressed = NULL;

  return req;
}

/**
 * @brief BinLinkerにパックされているデータ数を取得
 *
 * @param pFileManager ファイルマネージャ
 *
 * @return BinLinkerにパックされているデータ数を返す、まだデータが読み込まれていないなら0
 */
u32 BaseModelResourceUnit::GetBinLinkerDataCount(gfl2::fs::AsyncFileManager* pFileManager) const
{
  // ファイルが読み込まれていないなら無視
  if (!this->IsStaticDataLoaded(pFileManager))
  {
    return 0;
  }

  // BinLinkerからデータ数取得
  gfl2::fs::BinLinkerAccessor binLinkerAccessor(m_pStaticDataBuffer);
  return binLinkerAccessor.GetDataMax();
}

/**
 * @brief BinLinkerにパックされているデータサイズを取得
 *
 * @param pFileManager ファイルマネージャ
 * @param index BinLinkerでパックした際のインデックス
 *
 * @return BinLinkerにパックされているデータサイズを返す、まだデータが読み込まれていないなら0
 */
u32 BaseModelResourceUnit::GetBinLinkerDataSize(gfl2::fs::AsyncFileManager* pFileManager, u32 index) const
{
  // ファイルが読み込まれていないなら無視
  if (!this->IsStaticDataLoaded(pFileManager))
  {
    return 0;
  }

  // BinLinkerからデータサイズ取得
  gfl2::fs::BinLinkerAccessor binLinkerAccessor(m_pStaticDataBuffer);
  return binLinkerAccessor.GetDataSize(index);
}

/**
 * @brief BinLinkerにパックされているデータを取得
 *
 * @param pFileManager ファイルマネージャ
 * @param BinLinkerでパックした際のインデックス
 *
 * @return BinLinkerにパックされているデータを返す、まだデータが読み込まれていないならNULL、不正なインデックスならNULL
 */
const void* BaseModelResourceUnit::GetBinLinkerData(gfl2::fs::AsyncFileManager* pFileManager, u32 index) const
{
  // ファイルが読み込まれていないなら無視
  if (pFileManager != NULL)
  {
    if (!this->IsStaticDataLoaded(pFileManager))
    {
      return NULL;
    }
  }

  // BinLinkerからデータ取得
  gfl2::fs::BinLinkerAccessor binLinkerAccessor(m_pStaticDataBuffer);
  return binLinkerAccessor.GetData(index);
}

/**
 * @brief 静的データ同期読み込み開始
 *
 * @param isAsync true:非同期読み込み false:同期読み込み
 * @param arcId アーカイブ番号
 * @param datId アーカイブ内のデータ番号
 * @param pFileHeap ファイルヒープ
 * @param pFileManager ファイルマネージャ
 */
void BaseModelResourceUnit::LoadStaticData(bool isAsync, u32 arcId, u32 datId, gfl2::heap::HeapBase* pFileHeap, gfl2::fs::AsyncFileManager* pFileManager)
{
  // 既に読み込み完了しているなら読み込まない
  if (m_pStaticDataBuffer != NULL)
  {
    return;
  }

  // 既に非同期読み込み中なら読み込まない
  if (!pFileManager->IsArcFileLoadDataFinished((void**)&m_pStaticDataBuffer))
  {
    return;
  }

  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req = this->MakeStaticDataLoadReq(arcId, datId, pFileHeap); 
  if (isAsync)
  {
    // 非同期読み込み
    pFileManager->AddArcFileLoadDataReq(req);
  }
  else
  {
    // 同期読み込み
    pFileManager->SyncArcFileLoadData(req);
  }
}

/**
 * @brief 静的データポインタの設定
 *
 * @param void * pData  静的データポインタ
 */
void BaseModelResourceUnit::LoadStaticData( void * pData )
{
  m_pStaticDataBuffer = pData;
  m_IsStaticDataBufferManaged = false;
}

/**
 * @brief 静的データ非同期読み込み完了したか？
 *
 * @param pFileManager ファイルマネージャ
 *
 * @return 完了していたらtrue, まだならfalse
 */
bool BaseModelResourceUnit::IsStaticDataLoaded(gfl2::fs::AsyncFileManager* pFileManager) const
{
  if( m_IsStaticDataBufferManaged )
  {
    if(m_pStaticDataBuffer == NULL)
    {
      return false;
    }
    else
    {
      // ファイルマネージャに問い合わせ
      return pFileManager->IsArcFileLoadDataFinished((void**)&m_pStaticDataBuffer);
    }
  }
  else
  {
    return true;
  }
}

/**
 * @brief 読み込んだ静的データの開放
 */
void BaseModelResourceUnit::UnloadStaticData()
{
  // 派生クラスでの開放
  this->UnloadStaticDataDerived();

  // リソースノードとバッファを開放
  GFL_SAFE_DELETE(m_pModelResourceNode);
  m_AnimationPackList.UnloadAll();

  // メモリ管理下であれば開放
  if( m_IsStaticDataBufferManaged )
  {
    GflHeapSafeFreeMemory(m_pStaticDataBuffer);
  }
  else
  {
    m_pStaticDataBuffer = NULL;
  }
}

/**
 * @brief 読み込んだ静的データの開放が可能か確認
 */
bool BaseModelResourceUnit::CanUnloadStaticData() const
{
  // 読み込まれていない
  if (m_pModelResourceNode == NULL)
  {
    return true;
  }

  // 参照カウントが０なら安全に解放できる
  return (m_pModelResourceNode->GetReferenceCnt() == 0);
}

/**
 * @brief モデルリソースノードセットアップ
 *
 * @param pFileManager ファイルマネージャ
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 *
 * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
 */
bool BaseModelResourceUnit::SetupModelResourceNode(gfl2::fs::AsyncFileManager* pFileManager, gfl2::gfx::IGLAllocator* pGfxAllocator)
{
  // すでにリソースノードが生成されているなら何もしない
  if (m_pModelResourceNode != NULL)
  {
    return true;
  }

  // BinLinkerからモデルデータを取得する
  const void* pModelData = this->GetBinLinkerData(pFileManager, BASE_BIN_LINKER_INDEX_MODEL);
  if (pModelData == NULL)
  {
    return false;
  }

  // リソースノードを生成する
  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack mdlDataPack;
  mdlDataPack.SetModelData((c8*)pModelData);

  m_pModelResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNodeIndex(pGfxAllocator, &mdlDataPack, 0);

  return (m_pModelResourceNode != NULL);
}

/**
 * @brief モデルリソースノード取得
 *
 * @return モデルリソースノードを返す。まだデータが読み込まれていないならNULL
 */
gfl2::renderingengine::scenegraph::resource::ResourceNode* BaseModelResourceUnit::GetModelResourceNode()
{
  return m_pModelResourceNode;
}

/**
 * @brief アニメーションパックリストセットアップ
 *
 * @param pFileManager ファイルマネージャ
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param pFileHeap ファイルヒープ
 *
 * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
 */
bool BaseModelResourceUnit::SetupAnimationPackList(gfl2::fs::AsyncFileManager* pFileManager, gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pFileHeap)
{
  // すでに常駐モーションが読み込み済みなら何もしない
  if (m_AnimationPackList.IsLoaded(0))
  {
    return true;
  }

  // BinLinkerからアニメーションデータを取得する
  const void* pAnimationData = this->GetBinLinkerData(pFileManager, BASE_BIN_LINKER_INDEX_ANIMATION);
  if (pAnimationData == NULL)
  {
    return NULL;
  }

  // アニメーションパックにデータを読み込む
  m_AnimationPackList.LoadData(0, pGfxAllocator, pFileHeap, (c8*)pAnimationData);
  return m_AnimationPackList.IsLoaded(0);
}

/**
 * @brief アニメーションパックリスト取得
 *
 * @return アニメーションパックを返す。まだデータが読み込まれていないならNULL
 */
gfl2::animation::AnimationPackList* BaseModelResourceUnit::GetAnimationPackList()
{
  return &m_AnimationPackList;
}

/**
 * @brief 動的アニメーションデータ同期読み込み開始
 *
 * @param arcId アーカイブ番号
 * @param datId アーカイブ内のデータ番号
 * @param packId パック番号
 * @param pFileHeap ファイルヒープ
 * @param pFileManager ファイルマネージャ
 */
void BaseModelResourceUnit::LoadDynamicAnimationDataSync(u32 arcId, u32 datId, u32 packId, gfl2::heap::HeapBase* pFileHeap, gfl2::fs::AsyncFileManager* pFileManager)
{
  // 既に読み込み完了しているなら読み込まない
  if (m_ppDynamicAnimationDataBufferList[packId - 1] != NULL)
  {
    return;
  }

  // 既に非同期読み込み中なら読み込まない
  if (!pFileManager->IsArcFileLoadDataFinished((void**)&m_ppDynamicAnimationDataBufferList[packId - 1]))
  {
    return;
  }

  pFileManager->SyncArcFileLoadData(this->MakeDynamicAnimationDataLoadReq(arcId, datId, packId, pFileHeap));
}

/**
 * @brief 動的アニメーションデータ同期読み込み開始
 *
 * @param arcId アーカイブ番号
 * @param datId アーカイブ内のデータ番号
 * @param packId パック番号
 * @param pFileHeap ファイルヒープ
 * @param pFileManager ファイルマネージャ
 */
void BaseModelResourceUnit::LoadDynamicAnimationDataAsync(u32 arcId, u32 datId, u32 packId, gfl2::heap::HeapBase* pFileHeap, gfl2::fs::AsyncFileManager* pFileManager)
{
  // 既に読み込み完了しているなら読み込まない
  if (m_ppDynamicAnimationDataBufferList[packId - 1] != NULL)
  {
    return;
  }

  // 既に非同期読み込み中なら読み込まない
  if (!pFileManager->IsArcFileLoadDataFinished((void**)&m_ppDynamicAnimationDataBufferList[packId - 1]))
  {
    return;
  }

  pFileManager->AddArcFileLoadDataReq(this->MakeDynamicAnimationDataLoadReq(arcId, datId, packId, pFileHeap));
}

/**
 * @brief 動的アニメーションデータ非同期読み込み完了したか？
 *
 * @param packId パック番号
 * @param pFileManager ファイルマネージャ
 *
 * @return 完了していたらtrue, まだならfalse
 */
bool BaseModelResourceUnit::IsDynamicAnimationDataLoaded(u32 packId, gfl2::fs::AsyncFileManager* pFileManager) const
{
  // ファイルマネージャに問い合わせ
  return pFileManager->IsArcFileLoadDataFinished(&m_ppDynamicAnimationDataBufferList[packId - 1]);
}

/**
 * @brief 読み込んだ動的アニメーションデータのセットアップ
 *
 * @param packId パック番号
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param pSetupHeap セットアップ用のヒープ
 */
void BaseModelResourceUnit::SetupDynamicAnimationData(u32 packId, gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pSetupHeap)
{
  if( m_AnimationPackList.IsLoaded(packId) == false )
  {
    m_AnimationPackList.LoadData(packId, pGfxAllocator, pSetupHeap, (c8*)m_ppDynamicAnimationDataBufferList[packId - 1]);
  }
}

/**
 * @brief 読み込んだ動的アニメーションデータの開放
 *
 * @param packId パック番号
 */
void BaseModelResourceUnit::UnloadDynamicAnimationData(u32 packId)
{
  m_AnimationPackList.Unload(packId);
  GflHeapSafeFreeMemory(m_ppDynamicAnimationDataBufferList[packId - 1]);
}


}}

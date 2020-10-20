//===================================================================
/**
 * @file    SkyboxResourceManager.cpp
 * @brief   天球グラフィックリソース管理クラス
 * @author  saita_kazuki
 * @date    2015.04.27
 */
//===================================================================

#include "./SkyboxResourceManager.h"

#include <arc_index/skybox.gaix>

#include "GameSys/include/GameManager.h"  // AsyncFileManager取得のため
#include <arc_def_index/arc_def.h>
#include "System/include/ArcIdGetter.h"
#include "GameSys/include/GameData.h"

// debug
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

#include <util/include/gfl2_std_string.h>

GFL_NAMESPACE_BEGIN( System )
GFL_NAMESPACE_BEGIN( Skybox )

/**
 * @brief コンストラクタ
 */
ResourceManager::ResourceManager() :
  m_seq( 0),
  m_pHeap( NULL),
  m_pTemporaryHeap( NULL),
  m_accessor(),
  m_pAsyncFileManager( NULL)
{
  gfl2::std::MemClear( m_resource, sizeof( m_resource));
}

/**
 * @brief デストラクタ
 */
ResourceManager::~ResourceManager()
{
}

/**
 * @brief 初期化
 * @param リソース読み込みで使用するヒープの親ヒープ
 * @param pTemporaryHeap リソース読み込みに使用する一時確保用ヒープ
 */
void ResourceManager::Initialize( gfl2::heap::HeapBase* pParent, gfl2::heap::HeapBase* pTemporaryHeap)
{
  const u32 LOCAL_HEAP_SIZE = 0x99700;      ///< ローカルヒープのサイズ

  // ローカルヒープ作成
  m_pHeap = GFL_CREATE_LOCAL_HEAP_NAME(
    pParent,
    LOCAL_HEAP_SIZE,
    gfl2::heap::HEAP_TYPE_EXP,
    false,
    "SkyboxResource"
    );

  const u32 LOCAL_TEMPORARY_HEAP_SIZE = 0x43800;      ///< ローカルヒープ(一時確保用)のサイズ。270KB

  // ローカルヒープ作成
  m_pTemporaryHeap = GFL_CREATE_LOCAL_HEAP_NAME(
    pTemporaryHeap->GetLowerHandle(),
    LOCAL_TEMPORARY_HEAP_SIZE,
    gfl2::heap::HEAP_TYPE_EXP,
    false,
    "SkyboxTemporary"
    );

  for( s32 i = 0; i < RESOURCE_TYPE_MAX; ++i)
  {
    m_resource[i].data = NULL;
    m_resource[i].size = 0;
    m_accessor[i].Initialize( NULL);
  }

  m_seq = 0;

  m_pAsyncFileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
}

/**
 * @brief 終了
 */
void ResourceManager::Terminate()
{
  for( s32 i = 0; i < RESOURCE_TYPE_MAX; ++i)
  {
    if( m_resource[i].data)
    {
      GflHeapFreeMemory( m_resource[i].data);
      m_resource[i].data = NULL;
      m_resource[i].size = 0;
    }

    m_accessor[i].Initialize( NULL);
  }

  if( m_pHeap)
  {
    GFL_DELETE_HEAP( m_pHeap);
    m_pHeap = NULL;
  }

  if( m_pTemporaryHeap)
  {
    GFL_DELETE_HEAP( m_pTemporaryHeap);
    m_pTemporaryHeap = NULL;
  }

  m_seq = 0;
  m_pAsyncFileManager = NULL;
}

/**
 * @brief リソース非同期読み込み開始
 */
void ResourceManager::StartAsyncLoad()
{
  this->AddOpenReq();
  m_seq = 0;
}

/**
 * @brief リソース非同期読み込み完了待ち
 * @retval true 完了
 * @retval false 実行中
 */
bool ResourceManager::WaitAsyncLoad()
{
  // シーケンス
  enum
  {
    WAIT_OPEN,        // garcオープン待ち
    WAIT_READ_COMMON, // 共通データ読み込み待ち
    WAIT_CLOSE,       // garcクローズ待ち
    END,              // 終了
  };

  switch( m_seq)
  {
  case WAIT_OPEN:
    if( m_pAsyncFileManager->IsArcFileOpenFinished( this->GetArcID()))
    {
      this->AddAsyncLoadRequest(
        GARC_skybox_common_COMP,
        &m_resource[RESOURCE_TYPE_COMMON].data,
        &m_resource[RESOURCE_TYPE_COMMON].size
        );
      
      m_seq++;
    }
    break;

  case WAIT_READ_COMMON:
    if( m_pAsyncFileManager->IsArcFileLoadDataFinished( &m_resource[RESOURCE_TYPE_COMMON].data))
    {
      m_accessor[RESOURCE_TYPE_COMMON].Initialize( m_resource[RESOURCE_TYPE_COMMON].data);
      this->AddCloseReq();
      m_seq++;
    }
    break;

  case WAIT_CLOSE:
    if( m_pAsyncFileManager->IsArcFileCloseFinished( this->GetArcID()))
    {
      // 一時確保用ヒープを削除
      GFL_DELETE_HEAP( m_pTemporaryHeap);
      m_pTemporaryHeap = NULL;

#if defined( DEBUG_ONLY_FOR_saita_kazuki )
      m_pHeap->PrintHeapInfo( "Skybox Resource Manager");
#endif
      m_seq++;
      return true;
    }
    break;

  case END:
    return true;
    //break;

  default:
    GFL_ASSERT( 0);
    break;
  }

  return false;
}

/**
 * @brief アクセサーの取得
 * @param type  リソースの種類
 */
gfl2::fs::BinLinkerAccessor* ResourceManager::GetAccessor( ResourceType type)
{
  if( type < RESOURCE_TYPE_MAX)
  {
    GFL_ASSERT( m_resource[type].data);
    return &m_accessor[type];
  }
  GFL_ASSERT( 0);
  return NULL;
}

/**
 * @brief パック内のバイナリデータの取得
 * @param type パック種類
 * @param index パック内インデックス
 * @return バイナリデータ。ダミーの場合はNULL
 */
void* ResourceManager::GetData( u32 type, u32 index) const
{
  u32 size = m_accessor[type].GetDataSize( index);
  if( size > 0)
  {
    GFL_ASSERT( index < m_accessor[type].GetDataMax());
    return m_accessor[type].GetData( index);
  }
  return NULL;
}

/**
 * @brief ARCIDの取得
 * @return 天球garcのARCID
 */
gfl2::fs::ArcFile::ARCID ResourceManager::GetArcID() const
{
  Field::EventWork * evwk = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
  //@fix MMCat[379]:天候「暗黒」時のポニのビーチ、ナッシー・アイランドの状態について >> Mcatコメント内有るaの案件対処
  bool set_dark = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSkyboxDarkFlag();
  if (set_dark)
  {
    return NIJI_ARCID_NUM(ARCID_SKYBOXDARK);
  }

  return NIJI_ARCID_NUM( ARCID_SKYBOX);
}

/**
 * @brief 非同期オープンリクエスト
 */
void ResourceManager::AddOpenReq()
{
  gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
  req.arcId        = this->GetArcID();
  req.prio         = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL + 1;
  req.heapForFile  = m_pTemporaryHeap->GetLowerHandle();  // Closeするまで保持される
  req.heapForReq   = m_pTemporaryHeap->GetLowerHandle();  // リクエスト終了まで一時的に保持される

  m_pAsyncFileManager->AddArcFileOpenReq( req);
}

/**
 * @brief 非同期クローズリクエスト
 */
void ResourceManager::AddCloseReq()
{
  gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
  req.arcId        = this->GetArcID();
  req.prio         = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL + 1;
  req.heapForReq   = m_pTemporaryHeap->GetLowerHandle();  // リクエスト終了まで一時的に保持される

  m_pAsyncFileManager->AddArcFileCloseReq( req);
}

/**
 * @brief 非同期読み込みリクエスト
 * @param datId garcデータID
 * @param ppBuff 確保した読み込みバッファが渡されるポインタ
 * @param pSize サイズのポインタ
 */
void ResourceManager::AddAsyncLoadRequest( u32 datId, void** ppBuff, u32* pSize)
{
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  req.arcId              = this->GetArcID();
  req.datId              = datId;
  req.prio               = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL + 1;
  req.ppBuf              = ppBuff;
  req.pBufSize           = pSize;
  req.heapForBuf         = m_pHeap;                   // ファイルから読み込んだ内容を入れておくバッファを確保する
  req.align              = 128;
  req.heapForReq         = m_pTemporaryHeap->GetLowerHandle(); // リクエスト終了まで一時的に使用される
  req.heapForCompressed  = m_pTemporaryHeap->GetLowerHandle(); // 解凍時に一時的に使用される

  m_pAsyncFileManager->AddArcFileLoadDataReq( req);
}

GFL_NAMESPACE_END( Skybox )
GFL_NAMESPACE_END( System )

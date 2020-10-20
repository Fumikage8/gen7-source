//===================================================================
/**
 * @file    Skybox.cpp
 * @brief   天球
 * @author  saita_kazuki
 * @date    2015.02.19
 */
//===================================================================

#include "Test/FieldTest/include/Skybox/Skybox.h"
#include "Test/FieldTest/include/Skybox/Sky.h"
#include "Test/FieldTest/include/Skybox/Cloud.h"

// デバッグ
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

// メモリ
#include <gfx/include/gfl2_GLMemory.h>

// fs
#include <fs/include/gfl2_FsAsyncFileManager.h>

// arc
#include <arc_def_index/arc_def.h>

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldTest )
GFL_NAMESPACE_BEGIN( Skybox )

/**
 * @brief   コンストラクタ
 */
Skybox::Skybox() :
  m_isEnable( false),
  m_seq( 0),
  m_createState( CREATA_STATE_NONE),
  m_pSky( NULL),
  m_pCloud( NULL),
  m_pHeap( NULL),
  m_pGLAllocator( NULL),
  m_pAsyncFileManager( NULL)
{
}

///< デストラクタ
Skybox::~Skybox()
{
}

/**
 * @brief   生成開始
 * @param   pHeap アプリケーションヒープ
 * @param   pGLAllocator リソース確保用アロケータ
 * @param   pAsyncFileManager garc読み込みクラス
 */
void Skybox::StartCreate( gfl2::heap::HeapBase* pHeap, gfl2::gfx::IGLAllocator* pGLAllocator, gfl2::fs::AsyncFileManager* pAsyncFileManager)
{
  // @todo ひとまず親ヒープをそのまま使用する
  m_pHeap = pHeap;

  m_pGLAllocator = pGLAllocator;

  m_pAsyncFileManager = pAsyncFileManager;

  m_createState = CREATE_STATE_UPDATE;
}

/**
 * @brief   生成処理の更新
 * @retval  true 完了
 * @retval  false 実行中
 */
b8 Skybox::UpdateCreate()
{
  // @todo 今は全て同期読み込み。

  if( this->IsFinishCreate())
  {
    return true;
  }

  enum CreateSequence
  {
    OPEN_GARC,
    CREATE_SKY,
    CREATE_CLOUD,
    CLOSE_GARC,

    FINISH,
  };

  switch( m_seq)
  {
  case OPEN_GARC:
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
      req.arcId        = ARCID_SKYBOX_TEST;
      req.heapForFile  = m_pHeap;
      req.heapForReq   = m_pHeap;

      m_pAsyncFileManager->SyncArcFileOpen( req);
    }
    //break;

  case CREATE_SKY:
    m_pSky = GFL_NEW( m_pHeap) Sky();
    m_pSky->Create( m_pHeap, m_pGLAllocator, m_pAsyncFileManager);
    m_seq++;
    //break;

  case CREATE_CLOUD:
    m_pCloud = GFL_NEW( m_pHeap) Cloud();
    m_pCloud->Create( m_pHeap, m_pGLAllocator, m_pAsyncFileManager);
    m_seq++;
    //break;

  case CLOSE_GARC:
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
      req.arcId       = ARCID_SKYBOX;
      req.heapForReq  = m_pHeap;

      m_pAsyncFileManager->SyncArcFileClose( req);
    }
    //break;

  case FINISH:
    m_seq = 0;
    m_isEnable = true;
    m_createState = CREATE_STATE_FINISH;
    return true;
    //break;

  default:
    GFL_ASSERT( 0);
    break;
  }

  return false;
}

/**
 * @brief   生成処理が完了しているか
 * @retval  true 完了
 * @retval  false 実行中
 */
b8 Skybox::IsFinishCreate() const
{
  return ( m_createState == CREATE_STATE_FINISH );
}

/**
 * @brief   破棄
 * @retval  true  破棄完了
 * @retval  false 破棄実行中
 */
b8 Skybox::Destroy()
{
  // @todo テストProcの破棄関数がシーケンス制御に対応したら、こちらもシーケンス制御する
  // 現状は全てを1フレームで解放してtrueを返す

  enum
  {
    DELETE_CLOUD,
    DELETE_SKY,

    FINISH,
  };

  switch( m_seq)
  {
  case DELETE_CLOUD:
    if( m_pCloud)
    {
      if( m_pCloud->Destroy())
      {
        GFL_SAFE_DELETE( m_pCloud);
        m_seq++;
      }
    }
    else
    {
      m_seq++;
    }
    //break;

  case DELETE_SKY:
    if( m_pSky)
    {
      if( m_pSky->Destroy())
      {
        GFL_SAFE_DELETE( m_pSky);
        m_seq++;
      }
    }
    else
    {
      m_seq++;
    }
    //break;

  case FINISH:
    return true;
    //break;

  default:
    GFL_ASSERT( 0);
    break;
  }

  return false;
}

/**
 * @brief   初期化
 */
void Skybox::Initialize()
{
  if( m_pSky)
  {
    m_pSky->Initialzie();
  }

  if( m_pCloud)
  {
    m_pCloud->Initialzie();
  }
}

/**
 * @brief   更新
 */
void Skybox::Update()
{
  if( m_isEnable == false)
  {
    return;
  }
}

/**
 * @brief   描画
 */
void Skybox::Draw()
{
  if( m_isEnable == false)
  {
    return;
  }
}

/**
 * @brief   有効/無効フラグ切り替え
 * @param   isEnable trueで有効、falseで無効
 * @note    falseにすると更新、描画の処理が無効となる
 */
void Skybox::SetEnable( b8 isEnable)
{
  m_isEnable = isEnable;
  if( m_pSky)
  {
    m_pSky->SetVisible( isEnable);
  }
  if( m_pCloud)
  {
    m_pCloud->SetVisible( isEnable);
  }
}

/**
 * @brief   有効/無効フラグ取得
 * @retval  true 有効
 * @retval  false 無効
 */
b8 Skybox::IsEnable() const
{
  return m_isEnable;
}

/**
 * @brief   空の取得
 */
Sky* Skybox::GetSky()
{
  return m_pSky;
}

/**
 * @brief   雲の取得
 */
Cloud* Skybox::GetCloud()
{
  return m_pCloud;
}

GFL_NAMESPACE_END( Skybox )
GFL_NAMESPACE_END( FieldTest )
GFL_NAMESPACE_END( Test )

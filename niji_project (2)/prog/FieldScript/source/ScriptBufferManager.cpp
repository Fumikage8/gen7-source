//=============================================================================
/**
 * @file    ScriptBufferManager.h
 * @brief   スクリプト読み込み用バッファ管理クラス
 * @author  tamada
 * @date    2012.08.19
 */
//=============================================================================

#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>
#include <debug/include/gfl2_Assert.h>
#include <arc_def_index/arc_def.h>
#include <GameSys/include/GameManager.h>

#include "FieldScript/include/ScriptObject.h"
#include "FieldScript/include/ScriptBufferManager.h"
#include "FieldScript/include/FieldScriptSystem.h"


//gfl2::fs::ArcFile::ARCID ARCID_FIELD_SCRIPT = gfl2::fs::ArcFile::ARCID_NULL;

//=============================================================================
//=============================================================================
/* ------------------------------------------------------------------------- */
/**
 * @brief フィールド
 */
/* ------------------------------------------------------------------------- */
GFL_NAMESPACE_BEGIN( Field )
/* ------------------------------------------------------------------------- */
/**
 * @brief スクリプト
 */
/* ------------------------------------------------------------------------- */
GFL_NAMESPACE_BEGIN( FieldScript )

//--------------------------------------------------------------
/// 負荷計測用定義(debug用）
//==============================================================
enum
{ 
  DEBUG_TICK_PRINT_BUFFER_OPEN, ///< バッファオープン

  DEBUG_TICK_PRINT_BUFFER_MAX,
};


#if PM_DEBUG
#define   _FIELD_SCRIPT   ARCID_FIELD_SCRIPTD
#else
#define   _FIELD_SCRIPT   ARCID_FIELD_SCRIPT
#endif


/// 負荷計測クラスセットアップ
//GFL_DEBUG_TICK_SETUP( SCRIPT_BUFFER, DEBUG_TICK_PRINT_BUFFER_MAX );

//-----------------------------------------------------------------------------
/**
 * @struct  ScriptBuffer
 */
//-----------------------------------------------------------------------------
struct ScriptBuffer {
  void * m_buffer;  ///<読み込んだバッファへのポインタ
  u32 m_size;       ///<読み込んだバッファのサイズ
  u16 m_AmxId;      ///<読み込んだAMXのID
  u16 m_ArcId;      ///<読み込み先アーカイブID（未使用）
  u32 m_RefCount;   ///<参照カウンタ
  /* ------------------------------------------------------------------------- */
  /**
   * @brief コンストラクタ
   */
  /* ------------------------------------------------------------------------- */
  ScriptBuffer() : m_buffer( NULL ), m_size( 0 ), m_AmxId( ScriptObject::AMX_ID_NULL ), m_ArcId(0), m_RefCount( 0 )
  {
  }

  /* ------------------------------------------------------------------------- */
  /**
   * @brief デストラクタ
   */
  /* ------------------------------------------------------------------------- */
  ~ScriptBuffer() 
  {
    if ( m_buffer ) GflHeapSafeFreeMemory( m_buffer ); 
  }
};


/* ------------------------------------------------------------------------- */
/**
 * @brief コンストラクタ
 *
 * @param p_heap ヒープへのポインタ
 * @param size バッファサイズ
 */
/* ------------------------------------------------------------------------- */
ScriptBufferManager::ScriptBufferManager( gfl2::heap::HeapBase * p_heap, u32 size )
{
  m_BufferNum = size;
  m_pScrBuffer = GFL_NEW_ARRAY( p_heap ) ScriptBuffer[ m_BufferNum ];
  m_pAsyncFileManager = NULL;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief デストラクタ
 */
/* ------------------------------------------------------------------------- */
ScriptBufferManager::~ScriptBufferManager()
{
  for ( int i = 0; i < m_BufferNum; i ++ )
  {
    if ( m_pScrBuffer[i].m_AmxId != ScriptObject::AMX_ID_NULL )
    {
      GFL_ASSERT_MSG( 0, "ロードされたamxデータが開放されていません(amx=%d)",  m_pScrBuffer[i].m_AmxId );
    }
  }
  GFL_DELETE_ARRAY( m_pScrBuffer );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief セットアップ
 *
 * @param p_fileread ファイル読み込みマネージャへのポインタ
 * @param p_heap ファイル読み込みテーブル作成につかうヒープ
 * @param use_hio_flag HIOを使用するフラグ
 */
/* ------------------------------------------------------------------------- */
void ScriptBufferManager::Setup( gfl2::fs::AsyncFileManager * p_fileread, gfl2::heap::HeapBase * p_heap, gfl2::heap::HeapBase * p_heapWork, bool use_hio_flag )
{
  m_pAsyncFileManager = p_fileread;

  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId       = _FIELD_SCRIPT;
    openReq.heapForFile = p_heap;
    openReq.heapForReq  = p_heapWork->GetLowerHandle();
    if( use_hio_flag ){
      openReq.arcSrcCreateSetting = gfl2::fs::AsyncFileManager::ArcSrcCreateSetting::ALWAYS_CREATE;
    }
    
    m_pAsyncFileManager->AddArcFileOpenReq( openReq );
  }
}

/* ------------------------------------------------------------------------- */
/**
 * @brief セットアップ待ち
 *
 * @retval  true  セットアップ完了
 */
/* ------------------------------------------------------------------------- */
bool ScriptBufferManager::SetupWait( void )
{
  return m_pAsyncFileManager->IsArcFileOpenFinished( _FIELD_SCRIPT );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief クリーンアップ
 */
/* ------------------------------------------------------------------------- */
void ScriptBufferManager::Cleanup( void )
{
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
    closeReq.arcId = _FIELD_SCRIPT;
    m_pAsyncFileManager->SyncArcFileClose( closeReq );
  }
  m_pAsyncFileManager = NULL;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief AMXIDからインデックスを取得
 *
 * @param amx_id AMXID
 *
 * @return インデックス
 */
/* ------------------------------------------------------------------------- */
u32 ScriptBufferManager::GetIndex( u16 amx_id ) const
{
  for( int i = 0; i < m_BufferNum; i ++ )
  {
    if ( m_pScrBuffer[i].m_AmxId == amx_id )
    {
      return i;
    }
  }
  return NOT_FOUND;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief ファイル読み込みリクエスト
 *
 * @param p_heap 読み込みに使うヒープ
 * @param amx_id AMX ID
 * @param amxname AMX名
 *
 * @return 生成したバッファのインデックス
 */
/* ------------------------------------------------------------------------- */
u32 ScriptBufferManager::RequestReadFile( gfl2::heap::HeapBase * p_heap, u16 amx_id, const char* amxname )
{
  //読み込み済みの場合は参照カウンタを回す
  u32 index = GetIndex( amx_id );
  if (index != NOT_FOUND )
  { 
    m_pScrBuffer[index].m_RefCount++;
    return index; 
  }
  
  for ( int i = 0; i < m_BufferNum; i ++ )
  {
    if ( m_pScrBuffer[i].m_AmxId == ScriptObject::AMX_ID_NULL )
    {
      ScriptBuffer* buffer = &m_pScrBuffer[i];

      buffer->m_AmxId = amx_id;
      buffer->m_RefCount = 1;

      {
        gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
        req.fileName = "";//amxname;
        req.arcId = _FIELD_SCRIPT;
        req.datId = amx_id;
        req.heapForReq = p_heap->GetLowerHandle();
        req.heapForBuf = p_heap;
        req.align = 4;
        req.ppBuf = &buffer->m_buffer;
        req.pBufSize = &buffer->m_size;
        if( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldScriptSystem()->GetFieldScriptLocalRead() ){
          req.arcSrcUseSetting = gfl2::fs::AsyncFileManager::ArcSrcUseSetting::ALWAYS_USE;
        }
        
        m_pAsyncFileManager->AddArcFileLoadDataReq( req );
      }

      return i;
    }
  }

  GFL_ASSERT( 0 );

  return NOT_FOUND;
}


//------------------------------------------------------------------------------
/**
 * @func        SetAmxData
 * @brief       AmxDataのセット. RequestReadFile の外部データセット版.
 * @author      N.Takeda
 * @date        2016/02/09(火) 14:21:08
 *
 * @param[in,out] gfl2::heap::HeapBase *  p_heap      .
 * @param[in,out] void *                  temp_buffer バッファ.
 * @param[in]     u32                     temp_size   サイズ.
 * @param[in]     u16                     amx_id      .
 *
 * @return      u32 
 */
//------------------------------------------------------------------------------
u32 ScriptBufferManager::SetAmxData( gfl2::heap::HeapBase * p_heap, void * temp_buffer, u32 temp_size, u16 amx_id )
{
  //読み込み済みの場合は参照カウンタを回す
  u32 index = GetIndex( amx_id );
  if (index != NOT_FOUND )
  { 
    m_pScrBuffer[index].m_RefCount++;
    return index; 
  }
  
  for ( int i = 0; i < m_BufferNum; i ++ )
  {
    if ( m_pScrBuffer[i].m_AmxId == ScriptObject::AMX_ID_NULL )
    {
      ScriptBuffer* buffer = &m_pScrBuffer[i];

      buffer->m_AmxId = amx_id;
      buffer->m_RefCount = 1;
      buffer->m_buffer = GflHeapAllocMemory( p_heap, temp_size );
      memcpy( buffer->m_buffer, temp_buffer, temp_size );
      buffer->m_size = temp_size;
      return i;
    }
  }
  GFL_ASSERT( 0 );
  return NOT_FOUND;
}


/* ------------------------------------------------------------------------- */
/**
 * @brief AMXインデックスからバッファのポインタを取得
 *
 * @param amx_id AMXインデックス
 *
 * @return バッファへのポインタ
 */
/* ------------------------------------------------------------------------- */
void * ScriptBufferManager::GetBuffer( u32 amx_id ) const
{
  u32 index = GetIndex( amx_id );
  GFL_ASSERT( index != NOT_FOUND );
  GFL_ASSERT( m_pScrBuffer[index].m_AmxId != ScriptObject::AMX_ID_NULL );
  return m_pScrBuffer[index].m_buffer;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief AMXインデックスからバッファのサイズを取得
 *
 * @param amx_id AMXインデックス
 *
 * @return バッファのサイズ
 */
/* ------------------------------------------------------------------------- */
size_t ScriptBufferManager::GetBufferSize( u32 amx_id ) const
{
  u32 index = GetIndex( amx_id );
  GFL_ASSERT( index != NOT_FOUND );
  GFL_ASSERT( m_pScrBuffer[index].m_AmxId != ScriptObject::AMX_ID_NULL );
  return m_pScrBuffer[index].m_size;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief AMXインデックスを指定してバッファを解放
 *
 * @param amx_id AMXインデックス
 *
 * @return true:解放できた, false:解放失敗
 */
/* ------------------------------------------------------------------------- */
bool ScriptBufferManager::ReleaseBuffer( u32 amx_id )
{
  u32 index = GetIndex( amx_id );
  GFL_ASSERT( index != NOT_FOUND );
  if (m_pScrBuffer[index].m_AmxId != ScriptObject::AMX_ID_NULL )
  {
    // 他から参照されていた場合は参照カウンタを減らすだけ
    if( m_pScrBuffer[index].m_RefCount > 1 )
    {
      m_pScrBuffer[index].m_RefCount--;
      return false;
    }

    GflHeapSafeFreeMemory( m_pScrBuffer[index].m_buffer );
    m_pScrBuffer[index].m_AmxId = ScriptObject::AMX_ID_NULL;
    m_pScrBuffer[index].m_size = 0;
    m_pScrBuffer[index].m_RefCount = 0;
    m_pScrBuffer[index].m_buffer = NULL;
    return true;
  }
  return false;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief バッファ読み込み終了チェック
 *
 * @param buffer バッファ
 *
 * @return true:終了, false:未終了
 */
/* ------------------------------------------------------------------------- */
bool ScriptBufferManager::isReadFinished( ScriptBuffer* buffer ) const
{ 
  return m_pAsyncFileManager->IsArcFileLoadDataFinished( &buffer->m_buffer );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief AMXインデックスを指定して、対象バッファの読み込みが終了しているか判定
 *
 * @param amx_id AMXインデックス
 *
 * @return true:完了している, false:完了していない
 */
/* ------------------------------------------------------------------------- */
bool ScriptBufferManager::IsReadFinished( u32 amx_id ) const
{
  u32 index = GetIndex( amx_id );
  GFL_ASSERT( index != NOT_FOUND );
  return isReadFinished( &m_pScrBuffer[index] );
}
/* ------------------------------------------------------------------------- */
/**
 * @brief すべてのバッファ読み込みが完了しているか判定
 *
 * @return true:完了している, false:完了していない
 */
/* ------------------------------------------------------------------------- */
bool ScriptBufferManager::IsReadAllFinished( void ) const
{
  bool ret = true;
  for ( int i = 0; i < m_BufferNum; i ++ )
  {
    if( isReadFinished( &m_pScrBuffer[i] ) == false )
    {
      ret = false;
    }
  }
  return ret;
}


GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )


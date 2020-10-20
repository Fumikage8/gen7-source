#ifdef  GF_PLATFORM_WIN32
/**
* @file    gfl2_WinHeap.cpp
* @brief   Windowsでの開発時に使用するヒープクラス
* @author  ikuta_junya
* @date    2014.07.15
*/

#pragma warning(disable:4505)  //warning C4505: 'sys_alloc' : 参照されていないローカル関数は削除されました。

#include <heap/include/gfl2_WinHeap.h>
#include <debug/include/gfl2_DebugPrint.h>

#include <errno.h>
#include <heap/include/malloc.c>


// @note メモリ領域はチャンクとして確保される、チャンクの中身は下記のようになっている
// [チャンク管理領域][GF管理領域][アプリケーション使用領域]
// チャンク領域型(malloc_chunk)は32バイトだが、チャンクが使用状態の場合は前16バイト分しか使われず後ろ16バイト部分は使用領域となります

#if 1
// 実装中用のデバッグ表示をする
#define WIN_HEAP_DEBUG_PRINT(...) GFL_PRINT( __VA_ARGS__)
#define WIN_HEAP_DEBUG
#else
// 実装中用のデバッグ表示をしない
#define WIN_HEAP_DEBUG_PRINT(...) 
#endif

GFL_NAMESPACE_BEGIN( gfl2 );
GFL_NAMESPACE_BEGIN( heap );

namespace{

/**
* @brief DumpUseChunkFilter用、対象チャンクを識別ためのフィルター
*/
struct CHUNK_FILTER
{
  u8 id;   ///< ユーザー指定ID
};

/**
* @brief SearchUseChunk用、検索情報をやり取りする為のコンテナ
*/
struct SERCH_CONTAINER
{
  // 検索内容
  const void* pTarget;    ///< 検索対象アドレス

  // 検索結果
  b8  result;       ///< 見つかったならtrue、見つからなければfalse
};

/**
* @brief   ヒープ領域の一番前にある使用領域を取得する
*
* @param   msp        対象ヒープ領域
*
* @return  使用している領域の先頭アドレス
* @retval  NULL 使用している領域がありません
*/
void* GetTopUseMemory( mspace msp )
{
  mstate m = (mstate)msp;
  
  if (!ok_magic(m)) {
    USAGE_ERROR_ACTION(m,m);
  }

  ensure_initialization();
  if (!PREACTION(m)) {
    check_malloc_state(m);
    if(is_initialized(m)) {

      // @note セグメントが複数になっている際の検証ができていません
      // @note システム領域から追加取得をしない限り、セグメントが複数になることは無いはず
      msegmentptr s = &m->seg;
      while (s != 0) {
        mchunkptr q = align_as_chunk(s->base);
        
        // @note 領域の最初はシステムが確保した領域(malloc_state型)なのでスキップする
        // このif文は念のため
        if( 
          segment_holds(s, q) &&
          q->head != FENCEPOST_HEAD
          )
        {
          q = next_chunk(q);
        }

        while (
          segment_holds(s, q) &&
          // topも実行する
          // q != m->top && 
          q->head != FENCEPOST_HEAD
          ) 
        {
          // 使用しているなら領域を返す
          if( is_inuse( q ) )
          {
            return chunk2mem( q );
          }

          // @note topの先はpadding領域なので打ち切る
          // @note padding領域のサイズはTOP_FOOT_SIZEになる
          if( q == m->top )
          {
            break;
          }

          q = next_chunk(q);
        }
        s = s->next;
      }
    }
    POSTACTION(m);
  }

  return NULL;
}

/**
* @brief   指定実行処理を全てのチャンクに対して行う
* @note    管理領域にあるチャンクには行わない
*
* @param   msp        対象領域
* @param   pCallFunc  実行処理
* @param   pParam     実行処理に伝える値
*
* @return  無し 
*/
void VisitAllChunks( mspace msp, b32 (*pCallFunc)( mchunkptr, void* ), void* pParam = NULL )
{
  mstate m = (mstate)msp;
  
  if (!ok_magic(m)) {
    USAGE_ERROR_ACTION(m,m);
  }

  ensure_initialization();
  if (!PREACTION(m)) {
    check_malloc_state(m);
    if(is_initialized(m)) {

      // @note セグメントが複数になっている際の検証ができていません
      // @note システム領域から追加取得をしない限り、セグメントが複数になることは無いはず
      msegmentptr s = &m->seg;
      while (s != 0) {
        mchunkptr q = align_as_chunk(s->base);
        
        // @note 領域の最初はシステムが確保した領域(malloc_state型)なのでスキップする
        // このif文は念のため
        if( 
          segment_holds(s, q) &&
          q->head != FENCEPOST_HEAD
          )
        {
          q = next_chunk(q);
        }

        while (
          segment_holds(s, q) &&
          // topも実行する
          // q != m->top && 
          q->head != FENCEPOST_HEAD
          ) 
        {
          if( pCallFunc( q, pParam ) == false )
          {
            break;
          }

          // @note topの先はpadding領域なので打ち切る
          // @note padding領域のサイズはTOP_FOOT_SIZEになる
          if( q == m->top )
          {
            break;
          }

          q = next_chunk(q);
        }
        s = s->next;
      }
    }
    POSTACTION(m);
  }
}


/**
* @brief   チャンクの状況を表示する(チャンクアドレス版)
*
* @param   pChunk 対象チャンク
*
* @return  無し
*/
void PrintChunkData( mchunkptr pChunk )
{
#if GFL_DEBUG_PRINT
  size_t chunkSize = chunksize( pChunk );
  void* pMemory = chunk2mem( pChunk );
  MEMORY_HEADER *pHeader = static_cast<MEMORY_HEADER*>( pMemory );
  MEMORY_USER_HEADER *pUserHeader = reinterpret_cast<MEMORY_USER_HEADER*>( pHeader->userData );
  GFL_PRINT("0x%08p 0x%-6x %-24s %-5d %-3d\n", pChunk, chunkSize, pUserHeader->fileName, pUserHeader->line, pUserHeader->id );
#endif //GFL_DEBUG_PRINT
  
  // WIN_HEAP_DEBUG_PRINT("0x%08p next_chunk\n", next_chunk( pChunk ));
}

/**
* @brief   チャンクの状況を表示する(使用アドレス版)
*
* @param   pChunk 対象チャンク
*
* @return  無し
*/
void PrintMemoryData( mchunkptr pChunk )
{
#if GFL_DEBUG_PRINT
  size_t chunkSize = chunksize( pChunk );
  void* pMemory = chunk2mem( pChunk );
  MEMORY_HEADER *pHeader = static_cast<MEMORY_HEADER*>( pMemory );
  MEMORY_USER_HEADER *pUserHeader = reinterpret_cast<MEMORY_USER_HEADER*>( pHeader->userData );
  GFL_PRINT("0x%08p 0x%-6x %-24s %-5d %-3d\n", pHeader, chunkSize - TWO_SIZE_T_SIZES, pUserHeader->fileName, pUserHeader->line, pUserHeader->id );
#endif //GFL_DEBUG_PRINT
}

/**
* @brief   使用されているチャンクの状況を表示する
*
* @param   pChunk 対象チャンク
* @param   pParam ユーザー指定パラメータ
*
* @return  走査処理を継続するならtrue、処理を終了するならfalse
*/
b32 DumpUseChunk( mchunkptr pChunk, void* pParam )
{
  if( is_inuse( pChunk ) )
  {
    // PrintChunkData( pChunk );
    PrintMemoryData( pChunk );
  }
  else
  {
    WIN_HEAP_DEBUG_PRINT("0x%08p is free chunk \n", pChunk );
  }

  return true;
}

/**
* @brief   チャンクの状況を表示する
*          特定のチャンクのみ表示
*
* @param   pChunk 対象チャンク
* @param   pParam ユーザー指定パラメータ
*
* @return  走査処理を継続するならtrue、処理を終了するならfalse
*/
b32 DumpUseChunkFilter( mchunkptr pChunk, void* pParam )
{
  //size_t chunkSize = chunksize( pChunk );
  u8 id = ( static_cast<CHUNK_FILTER*>( pParam ) )->id;

  if( is_inuse( pChunk ) )
  {
    void* pMemory = chunk2mem( pChunk );
    MEMORY_HEADER *pHeader = static_cast<MEMORY_HEADER*>( pMemory );
    MEMORY_USER_HEADER *pUserHeader = reinterpret_cast<MEMORY_USER_HEADER*>( pHeader->userData );

    if( pUserHeader->id == id )
    {
      PrintChunkData( pChunk );
    }
  }

  return true;
}

/**
* @brief   指定メモリーを管理しているチャンクを検索する
*
* @param   pChunk 対象チャンク
* @param   pParam ユーザー指定パラメータ ( SERCH_CONTAINER )
*
* @return  走査処理を継続するならtrue、処理を終了するならfalse
*/
b32 SearchForUseChunk( mchunkptr pChunk, void* pParam )
{
  SERCH_CONTAINER* pSerchContainer = static_cast<SERCH_CONTAINER*>( pParam );

  if( is_inuse( pChunk ) )
  {
    void* pMemory = chunk2mem( pChunk );

    if( pMemory == pSerchContainer->pTarget )
    {
      // 一致するメモリが見つかったので処理を終了
      pSerchContainer->result = true;
      return false;
    }
  }

  // 走査処理を継続
  return true;
}

} //namespace

/**
* @brief   コンストラクタ
*/
WinHeap::WinHeap(void) : m_pSpace( NULL ), m_Size( 0 )
{
}

/**
* @brief   デストラクタ
*/
WinHeap::~WinHeap(void)
{
}


/**
*  @brief   ヒープの作成
*           friendでgfl::heap::Managerからのみ呼び出し
*
*  @param   pBuf     確保用バッファ
*  @param   size    バッファサイズ
*  @param   param   ユーザーパラメーター
*
*  @return  無し
*/
void WinHeap::CreateHeap( void *pBuf, const int size, void *param )
{
  GFL_UNUSED(param);
  m_pSpace = create_mspace_with_base( pBuf, size, 0 );
  m_Size = size;
}

/**
*  @brief   ヒープの削除
*           friendでgfl::heap::Managerからのみ呼び出し
*
*  @return  無し
*/
void WinHeap::DeleteHeap( void )
{
  destroy_mspace( m_pSpace );
  m_pSpace = NULL;
  m_Size = 0;
}

//
// 実際のメモリ確保・開放処理
// 

/**
* @brief   メモリ確保処理
*
* @param   size   確保サイズ
* @param   align  メモリアライメントサイズ
*
* @return  確保したアドレス
* @retval  NULL 確保に失敗
*/
void* WinHeap::AllocAlign( const int size, const int align )
{
  int memAlign = align;


#if !defined(__cplusplus_cli) // C++/CLIはネイティブスレッドを使えないらしいのでかこった
   // スレッドによる同時処理を防ぐ
  mAllocLock.lock();
#endif

  
  // @attencion 後方確保時はアライメント指定がマイナスでくる、WinHeapでは今のところ非対応
  if( memAlign < 0 )
  {
     memAlign *= -1;
  }

  void* p = mspace_memalign( m_pSpace, memAlign, size );

#ifdef WIN_HEAP_DEBUG
  if( p == NULL )
  {
#if !defined(__cplusplus_cli) // C++/CLIはネイティブスレッドを使えないらしいのでかこった
    // スレッドによる同時処理を防ぐ
    mAllocLock.unlock();
#endif
    return NULL;
  }

#if GFL_DEBUG_PRINT
  mchunkptr pChunk = mem2chunk( p );
  if( this->GetHeapId() == 4 )
  {
    WIN_HEAP_DEBUG_PRINT( "AllocAlign size %d align %d \n", size, align );
    size_t usableSize = mspace_usable_size( p );
    void* pMemoryTop = GetMemoryTop( p );
    WIN_HEAP_DEBUG_PRINT("usableSize %d \n", usableSize );
    WIN_HEAP_DEBUG_PRINT("chunk size %d \n", chunksize( pChunk ) );
    WIN_HEAP_DEBUG_PRINT("chunk offset %d \n", (char*)pMemoryTop - (char*)pChunk );
    WIN_HEAP_DEBUG_PRINT("TWO_SIZE_T_SIZES %d \n", TWO_SIZE_T_SIZES );
    WIN_HEAP_DEBUG_PRINT("malloc_chunk size %d \n", sizeof( malloc_chunk ) );
  }
#endif //GFL_DEBUG_PRINT

  int usableSize = mspace_usable_size( p );
  if( usableSize < size )
  {
    WIN_HEAP_DEBUG_PRINT("実際に使用する領域がusableSizeをはみ出している\n");
  }
#endif //WIN_HEAP_DEBUG

#if !defined(__cplusplus_cli) // C++/CLIはネイティブスレッドを使えないらしいのでかこった
  // スレッドによる同時処理を防ぐ
  mAllocLock.unlock();
#endif
  return p;
}

/**
* @brief   メモリ開放処理
* @note    確保元が同じヒープである必要があります
*
* @param   pBuf  開放対象アドレス
*
* @return  確保したアドレス
* @retval  NULL 確保に失敗
*/
void WinHeap::Free( void* pBuf )
{
#if !defined(__cplusplus_cli) // C++/CLIはネイティブスレッドを使えないらしいのでかこった
   // スレッドによる同時処理を防ぐ
  mAllocLock.lock();
#endif
  mspace_free( m_pSpace, pBuf );
#if !defined(__cplusplus_cli) // C++/CLIはネイティブスレッドを使えないらしいのでかこった
  // スレッドによる同時処理を防ぐ
  mAllocLock.unlock();
#endif
}

/**
* @brief   デフォルトで使用するアライメントサイズを取得
*
* @return  アライメントサイズ
*/
int WinHeap::GetDefaultAlign( void )
{
  return MALLOC_ALIGNMENT;
}

/**
*  @brief  メモリ状況の表示
*
*  @return 無し
*/
void WinHeap::Dump(void)
{
  GFL_PRINT("\n");
  GFL_PRINT("---------------- WinHeap Dump Start ---------------\n");
  GFL_PRINT("HEAPID         :%d\n", GetHeapId());
  GFL_PRINT("HeapCreated    :%s(%d)\n",GetCreateDataFilename(),GetCreateDataLine());
  GFL_PRINT("HeapName       :%s\n",GetHeapName());

#if GFL_HEAP_DEBUG
  if( this->GetCreateParentFlag() == true )
  {
    GFL_PRINT("PARENT HEAPID  :%d\n", GetMemoryHeader(this)->heapId );
  }
  else
  {
    GFL_PRINT("PARENT HEAPID  :--\n");
  }
#endif

  GFL_PRINT("TotalSize      :0x%08x\n", GetTotalSize() );
  GFL_PRINT("FreeSize       :0x%08x\n", GetTotalFreeSize() );
  GFL_PRINT("AllocatableSize:0x%08x\n", GetTotalAllocatableSize() );

  GFL_PRINT("Address----------- Size---------- FileName---------- Line- Id-\n");
  VisitAllChunks( m_pSpace, DumpUseChunk, NULL );
  GFL_PRINT("Address----------- Size---------- FileName---------- Line- Id-\n");
  
  GFL_PRINT("---------------- WinHeap Dump End -----------------\n");
  GFL_PRINT("\n");
}

/**
*  @brief  メモリ状況の表示
*          独自で用意したMemoryHeaderを元にダンプします。　指定したIDの情報のみダンプする版
*
*  @param  id   SetDebugUserID でしていていたID
*
*  @return 無し
*/
void WinHeap::DumpId( u8 id )
{
  GFL_PRINT("\n");
  GFL_PRINT("---------------- WinHeap Dump Start ---------------\n");
  GFL_PRINT("TARGETID       :%d\n", id );
  GFL_PRINT("HEAPID         :%d\n", GetHeapId() );

#if GFL_HEAP_DEBUG
  if( this->GetCreateParentFlag() == true )
  {
    GFL_PRINT("PARENT HEAPID  :%d\n", GetMemoryHeader(this)->heapId );
  }
  else
  {
    GFL_PRINT("PARENT HEAPID  :--\n");
  }
#endif

  GFL_PRINT("TotalSize      :0x%08x\n", GetTotalSize() );
  GFL_PRINT("FreeSize       :0x%08x\n", GetTotalFreeSize() );
  GFL_PRINT("AllocatableSize:0x%08x\n", GetTotalAllocatableSize() );

  GFL_PRINT("Address----------- Size---------- FileName---------- Line- Id-\n");
  CHUNK_FILTER filter;
  filter.id = id;
  VisitAllChunks( m_pSpace, DumpUseChunkFilter, &filter );
  GFL_PRINT("Address----------- Size---------- FileName---------- Line- Id-\n");
  
  GFL_PRINT("---------------- WinHeap Dump End -----------------\n");
  GFL_PRINT("\n");
}

/**
*  @brief  メモリ状況の表示
*          ライブラリなどで用意された関数を使いダンプします
*
*  @param  無し
*/
void WinHeap::SystemDump(void)
{
#if GFL_DEBUG_PRINT
  mallinfoEx info = mspace_mallinfoEx( m_pSpace );
  
  GFL_PRINT("  - non-mmapped space allocated from system : %d\n", info.arena );
  GFL_PRINT("  - number of free chunks                   : %d\n", info.ordblks );
  GFL_PRINT("  - space in mmapped regions                : %d\n", info.hblkhd );
  GFL_PRINT("  - maximum total allocated space           : %d\n", info.usmblks );
  GFL_PRINT("  - total allocated space                   : %d\n", info.uordblks );
  GFL_PRINT("  - total free space                        : %d\n", info.fordblks );
  GFL_PRINT("  - releasable (via malloc_trim) space      : %d\n", info.keepcost );
  GFL_PRINT("  - releasable space                        : %d\n", info.releasable );
#endif //GFL_DEBUG_PRINT
}

/**
*  @brief   ヒープのサイズを取得します
*
*  @return  サイズ
*/
int WinHeap::GetTotalSize(void)
{
  mallinfoEx info = mspace_mallinfoEx( m_pSpace );
  return info.usmblks;
}

/**
*  @brief   ヒープの合計空きサイズを取得します
*
*  @return  サイズ
*/
int WinHeap::GetTotalFreeSize(void)
{
  mallinfoEx info = mspace_mallinfoEx( m_pSpace );
  return info.fordblks;
}

/**
*  @brief   ヒープで確保できる最大のサイズを取得します
*
*  @return  サイズ
*/
int WinHeap::GetTotalAllocatableSize(void)
{
  mallinfoEx info = mspace_mallinfoEx( m_pSpace );
  return info.releasable;
}

/**
*  @brief  確保された領域のサイズを取得(HeapManagerにもあります)
*
*  @param  mem  対象[アプリケーション使用領域]の先頭アドレス
*
*  @return [アプリケーション使用領域]サイズ(-1は失敗)
*/
int WinHeap::GetMemorySize( const void *mem )
{  
  // [GF管理領域]の先頭アドレスを取得
  void* pMemoryTop = GetMemoryTop( mem );

#if 1
  // [GF管理領域]ヘッダーを取得
  MEMORY_HEADER *pHeader = GetMemoryHeader( mem );
  
  // [GF管理領域]+[アプリケーション使用領域]取得
  size_t usableSize = mspace_usable_size( pMemoryTop );
  
  // @note mspace_usable_sizeを使用すると、8バイト分次の領域まで跨る
  return usableSize - pHeader->offset;
#else

  // [GF管理領域]ヘッダーを取得
  MEMORY_HEADER *pHeader = GetMemoryHeader( mem );

  // [チャンク管理領域]を取得
  mchunkptr pChunk = mem2chunk( pMemoryTop );

  // [アプリケーション使用領域]のサイズを求め、返す
  // chunkSize == [チャンク管理領域][GF管理領域][アプリケーション使用領域]のサイズ
  // pHeader->offset == [GF管理領域]のサイズ
  // TWO_SIZE_T_SIZES == [チャンク管理領域](使用中)のサイズ
  size_t chunkSize = chunksize( pChunk );

  return chunkSize - pHeader->offset - TWO_SIZE_T_SIZES;
#endif

  // GFL_PRINT("foot 0x%08p \n", (char*)(pChunk) + chunksize( pChunk ));
  // GFL_PRINT("foot 0x%08p \n", (char*)(pMemoryTop) + usableSize );
  // #define next_chunk(p) ((mchunkptr)( ((char*)(p)) + ((p)->head & ~FLAG_BITS)))
  // GFL_PRINT("next_chunk 0x%08p\n", next_chunk( pChunk ));

#if 0
  GFL_PRINT("chunksize %d \n", chunksize( pChunk ) );
  GFL_PRINT("head offset %d \n", (u32)mem - (u32)pMemoryTop );
  GFL_PRINT("chunk offset %d \n", (char*)pMemoryTop - (char*)pChunk );
  GFL_PRINT("TWO_SIZE_T_SIZES %d \n", TWO_SIZE_T_SIZES );
  GFL_PRINT("malloc_chunk size %d \n", sizeof( malloc_chunk ) );
#endif
  
# if 0
  // [チャンク管理領域]を取得
  mchunkptr pChunk = mem2chunk( pMemoryTop );

  // [GF管理領域]ヘッダーを取得
  MEMORY_HEADER *pHeader = GetMemoryHeader( mem );
  
  // [アプリケーション使用領域]のサイズを求め、返す
  // chunkSize == [チャンク管理領域][GF管理領域][アプリケーション使用領域]のサイズ
  // pHeader->offset == [GF管理領域]のサイズ
  // CHUNK_OVERHEAD == [チャンク管理領域]のサイズ
  size_t chunkSize = chunksize( pChunk );
  
  GFL_PRINT("TWO_SIZE_T_SIZES %d \n", TWO_SIZE_T_SIZES );
  
  return chunkSize - pHeader->offset - TWO_SIZE_T_SIZES;
#endif
}

/**
*  @brief  対象メモリが管理しているメモリかチェックする
*  @note   アドレスリストに対して検索を行うため、処理負荷に注意
*
*  @param  pMemory 対象メモリ
*
*  @return 管理しているメモリならtrue、管理外ならfalseを返す
*/
b32 WinHeap::IsManaged( const void *pMemory ) const
{
  if( m_pSpace == NULL )
  {
    return false;
  }

  // 検索条件を設定
  SERCH_CONTAINER serchContainer;
  serchContainer.pTarget = pMemory;
  serchContainer.result = false;

  VisitAllChunks( m_pSpace, SearchForUseChunk, &serchContainer );
  return serchContainer.result;
}

/**
*  @brief  渡されたメモリが管理しているメモリかチェックする
*  @note   初期確保サイズの範囲チェックのみを行う
*
*  @param  pMemory 調べるメモリ
*
*  @return 管理しているメモリならtrue、管理外ならfalseを返す
*/
b32 WinHeap::IsManagedFast( const void *pMemory ) const
{
  if( m_pSpace == NULL )
  {
    return false;
  }

  uintptr_t in = reinterpret_cast< uintptr_t >( pMemory );
  uintptr_t start = reinterpret_cast< uintptr_t >( m_pSpace );
  uintptr_t end = start + m_Size;

  if( start <= in && in < end )
  {
    return true;
  }
  else
  {
    return false;
  }
}

/**
*  @brief  すべてのメモリを解放します
*
*  @return 成功したらtrue、失敗したらfalse
*/
bool WinHeap::FreeAll(void)
{
  // ヒープ領域の一番前にある使用領域を取得し、開放する
  // 高速化の余地がある
  void* pMemory = GetTopUseMemory( m_pSpace );
  while( pMemory )
  {
    this->DecAllocCount();
    this->Free( pMemory );
    pMemory = GetTopUseMemory( m_pSpace );
  }

  WIN_HEAP_DEBUG_PRINT("FreeAll After AllocCount %d \n", GetAllocCount() );
  return true;
}


GFL_NAMESPACE_END( heap );
GFL_NAMESPACE_END( gfl2 );
#endif //GF_PLATFORM_WIN32

//======================================================================
/**
 * @file	gfl2_HeapBase.cpp
 * @brief	GFLヒープ基底クラス
 * @author	ariizumi
 * @data	10/10/05
 */
//======================================================================
#include <string.h>

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include "gfl2_heap_local.h"
#include <math/include/gfl2_math_control.h>
#include <util/include/gfl2_std_string.h>

#include <heap/include/gfl2_heap_base_lower.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(heap)

#if GFL_HEAP_DEBUG
  u8 GflHeapBase::mDebugFreeMemClear = GflHeapBase::DEBUG_FREE_MEMC_NONE;
#endif // GFL_HEAP_DEBUG

//======================================================================
//	define
//======================================================================
//#pragma mark [> define

//----------------------------------------------------------------------------
/**
 *  @brief  メモリを確保
 *          メモリの確保はこのマクロを使って行ってください。
 *          AllocMemoryBlockとAllocMemoryBlockAlignを直接使用しないでください。
 *
 *  @param  heap             HeapBaseクラス(推奨) もしくは HEAPID(呼び出す関数をオーバーロードしています)
 *  @param  const int size   確保サイズ
 *  @param  const int align  アライメント(マイナスで後方から確保)
 *
 *  @return void* 確保メモリ(0クリアされています)
 */
//-----------------------------------------------------------------------------
#if GFL_HEAP_DEBUG
//AllocMemoryBlock HeapBase版
void *GflHeapAllocMemoryBlock( HeapBase *heapBase ,  const int size , const char *filename , const int linenum )
{
  return heapBase->AllocMemory( size , filename , linenum );
}
#else
void *GflHeapAllocMemoryBlock( HeapBase *heapBase ,  const int size )
{
  return heapBase->AllocMemory( size );
}
#endif

#if GFL_HEAP_DEBUG
//AllocMemoryBlock HEAPID版
void *GflHeapAllocMemoryBlock( HEAPID heapId ,  const int size , const char *filename , const int linenum )
{
  return Manager::GetHeapByHeapId(heapId)->AllocMemory( size , filename , linenum );
}
#else
void *GflHeapAllocMemoryBlock( HEAPID heapId ,  const int size )
{
  return Manager::GetHeapByHeapId(heapId)->AllocMemory( size );
}
#endif

#if GFL_HEAP_DEBUG
//AllocMemoryBlockAlign HeapBase版
void *GflHeapAllocMemoryBlockAlign( HeapBase *heapBase ,  const int size , const int align , const char *filename , const int linenum )
{
  return heapBase->AllocMemoryAlign( size , align , filename , linenum );
}
#else
void *GflHeapAllocMemoryBlockAlign( HeapBase *heapBase ,  const int size , const int align )
{
  return heapBase->AllocMemoryAlign( size , align );
}
#endif

#if GFL_HEAP_DEBUG
//AllocMemoryBlockAlign HEAPID版
void *GflHeapAllocMemoryBlockAlign( HEAPID heapId ,  const int size , const int align , const char *filename , const int linenum )
{
  return Manager::GetHeapByHeapId(heapId)->AllocMemoryAlign( size , align , filename , linenum );
}
#else
void *GflHeapAllocMemoryBlockAlign( HEAPID heapId ,  const int size , const int align )
{
  return Manager::GetHeapByHeapId(heapId)->AllocMemoryAlign( size , align );
}
#endif

#if GFL_HEAP_DEBUG
//AllocMemoryBlockAlign4096
void *GflHeapAllocMemoryBlockAlign4096( HeapBase *heapBase ,  const int size , const char *filename , const int linenum )
{
  return heapBase->AllocMemory4096( size , true , filename , linenum );
}
#else
void *GflHeapAllocMemoryBlockAlign4096( HeapBase *heapBase ,  const int size )
{
  return heapBase->AllocMemory4096( size , true );
}
#endif

#if GFL_HEAP_DEBUG
//AllocMemoryBlockAlign4096
void *GflHeapAllocMemoryBlockAlign4096Lower( HeapBase *heapBase ,  const int size , const char *filename , const int linenum )
{
  return heapBase->AllocMemory4096Lower( size , true , filename , linenum );
}
#else
void *GflHeapAllocMemoryBlockAlign4096Lower( HeapBase *heapBase ,  const int size )
{
  return heapBase->AllocMemory4096Lower( size , true );
}
#endif
//----------------------------------------------------------------------------
/**
 *  @brief  メモリを開放
 *          関数名統一のため用意しています
 *
 *  @param  void  開放メモリ
 */
//-----------------------------------------------------------------------------
void GflHeapFreeMemoryBlock(void* buf)
{
  gfl2::heap::GflHeapBase::FreeMemoryAuto(buf);
}

//#pragma mark [> HeapBase

GflHeapBase::GflHeapBase()
:m_lowerHandle(NULL)
//,mAllocCntLock()
{
//  mAllocCntLock.Initialize();
}

GflHeapBase::~GflHeapBase()
{
  //mAllocCntLock.Finalize();
}


//GflHeapBaseレベルの初期化
//friendでgfl::heap::Managerからのみ呼び出し
//確実に初期化時に呼ばれます
void GflHeapBase::InitHeapBase( const HEAPID heapId )
{
  m_heapId = heapId;
  m_allocCount = 0;
  
#if GFL_HEAP_DEBUG
  mDebugFlg = false;
  mDebugUserID = 0;
#endif

#if defined(GF_PLATFORM_WIN32)
  mCreateHeapScale = 1;
#endif 
}


//GflHeapBaseレベルの開放
//friendでgfl::heap::Managerからのみ呼び出し
//確実に開放時に呼ばれます(DeleteHeapの前です)
void GflHeapBase::TermHeapBase(bool isCheck)
{
  if( GetAllocCount() != 0 )
  {
    if( isCheck )
    {
      Dump();
      GFL_ASSERT_MSG( GetAllocCount() == 0 , "GFL_HEAP Memory is remained![%d]\n",GetAllocCount() );  //@check
    }
    FreeAll();
  }
  
#if GFL_HEAP_DEBUG
  if( mDebugFlg )
  {
    GFL_PRINT("GFL_HEAP HeapId[%d] Minimal memory [0x%08x]\n",m_heapId,mMinLeast);
  }
#endif// GFL_HEAP_DEBUG
}

//friendでgfl::heap::Managerからのみ呼び出しLowerHandleの作成
void GflHeapBase::SetLowerHandle( GflHeapBaseLower *handle )
{
  m_lowerHandle = handle;
#if defined(GF_PLATFORM_WIN32)
  m_lowerHandle->SetCreateHeapScale( this->GetCreateHeapScale() );
#endif //
}

//----------------------------------------------------------------------------
/**
 *  @brief  ヒープを常に後方確保するハンドルを取得します
 */
//-----------------------------------------------------------------------------
HeapBase* GflHeapBase::GetLowerHandle()
{
  return m_lowerHandle;
}


#if GFL_HEAP_DEBUG
void GflHeapBase::SetDebugModeFlg( bool flg )
{
  mDebugFlg = flg;
  if( flg )
  {
    mMinLeast = GetTotalAllocatableSize();
  }
  else
  {
    mMinLeast = 0;
  }
}
#endif

//----------------------------------------------------------------------------
/**
 *  @brief  メモリを確保
 *          メモリの確保はこのマクロを使って行ってください。
 *          AllocMemoryBlockとAllocMemoryBlockAlignを直接使用しないでください。
 *
 *  @param  const int size   確保サイズ
 *
 *  @return void* 確保メモリ(0クリアされています)
 */
//-----------------------------------------------------------------------------
#if GFL_HEAP_DEBUG
void* GflHeapBase::AllocMemory( const int size , const char *filename , const int linenum )
{
  return AllocMemoryCore( size , GetDefaultAlign() , true , filename , linenum );
}
#else
void* GflHeapBase::AllocMemory( const int size )
{
  return AllocMemoryCore( size , GetDefaultAlign() , true );
}
#endif

//----------------------------------------------------------------------------
/**
 *  @brief  メモリを確保(アラインメント)
 *          直接使用しないで、GflHeapAllocMemoryBlockとGflHeapAllocMemoryBlockAlignを
 *          使用してください。
 *
 *  @param  const int size   確保サイズ
 *  @param  const int align  アライメント(マイナスで後方から確保)
 *
 *  @return void* 確保メモリ(0クリアされています)
 */
//-----------------------------------------------------------------------------
#if GFL_HEAP_DEBUG
void* GflHeapBase::AllocMemoryAlign( const int size , const int align , const char *filename , const int linenum )
{
  return AllocMemoryCore( size , align , true , filename , linenum );
}
#else
void* GflHeapBase::AllocMemoryAlign( const int size , const int align )
{
  return AllocMemoryCore( size , align , true );
}
#endif

//メモリ確保コア処理
#if GFL_HEAP_DEBUG
void* GflHeapBase::AllocMemoryCore(  const int size , const int align , const bool isClearMemory , const char *filename , const u16 linenum )
#else
void* GflHeapBase::AllocMemoryCore(  const int size , const int align , const bool isClearMemory )
#endif
{
  if( size == 0 )
  {
    GFL_PRINT("GFL_HEAP Alloc Memory size = zero!\n");
    //GFL_ASSERT(0); //@check
    //return NULL;
  }
  
//  GFL_PRINT("alloc : %s : %d \n",filename, linenum);
  
#if defined(GF_CONFIG_DEBUG)  //Debug Buildでのみ有効
  if( Manager::IsExistHeapId(0) )
  {
    GFL_ASSERT( Manager::GetHeapByHeapId(0)->Check() ); //@check
  }
#endif //GF_CONFIG_DEBUG
  
  void *buf;
  void *retBuf;
  //アライメントからヘッダー分のオフセットを求める
  int adjustSize = CalcAllocateAdjustSize( align );

  buf = AllocAlign(size+adjustSize,align);
  
#if defined(GF_CONFIG_DEBUG)  //Debug Buildでのみ有効
  if( Manager::IsExistHeapId(0) )
  {
    GFL_ASSERT( Manager::GetHeapByHeapId(0)->Check() ); //@check
  }
#endif //GF_CONFIG_DEBUG

  if( buf == NULL )
  {
    /*  //こっちの情報はDumpの中で出るから不要
    GFL_PRINT("GFL_HEAP Alloc failed!\n");
    GFL_PRINT("HeapCreated      %s(%d)\n",GetCreateDataFilename(),GetCreateDataLine());
    GFL_PRINT("HeapName         %s\n",GetHeapName());
    GFL_PRINT("Request size     [0x%x] + [0x%x]\n",size,adjustSize);
    GFL_PRINT("Allocatable size [0x%x]\n",GetTotalAllocatableSize());
    GFL_PRINT("Total free size  [0x%x]\n",GetTotalFreeSize());
    */
    Dump();
    GFL_RELEASE_PRINT("GFL_RELEASE_HEAP Alloc failed!\n");
    GFL_RELEASE_PRINT("HeapCreated     :%s(%d)\n",GetCreateDataFilename(),GetCreateDataLine());
    GFL_RELEASE_PRINT("HeapName        :%s\n",GetHeapName());
    GFL_RELEASE_PRINT("Request size    :[0x%x] + [0x%x]\n",size,adjustSize);
    GFL_RELEASE_PRINT("Allocatable size:[0x%x]\n",GetTotalAllocatableSize());
    GFL_RELEASE_PRINT("Total free size :[0x%x]\n",GetTotalFreeSize());
#if GFL_HEAP_DEBUG
    GFL_ASSERT_MSG( buf,"GFL_HEAP Alloc failed!\n[%x:%s:%d]\n",size,filename,linenum ); //@check
#else
    GFL_ASSERT_MSG( buf,"GFL_HEAP Alloc failed!\n",size ); //@check
#endif
#if !(GFL_DEBUG)
    GFL_ASSERT_STOP(0);  //本番はASSERTSTOP=エラー
#endif
#ifdef __KLOCWORK__
    GFL_ASSERT_STOP(0);  //KLOCWORKの時もHALT
#endif
    return NULL;
  }

  //メモリクリア
  if( isClearMemory == true )
  {
    memset( buf ,0 , size+adjustSize );
  }

  //ヘッダを埋め込む処理
  retBuf = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(buf) + adjustSize);
  
#if GFL_HEAP_DEBUG
  SetMemoryHeader( GetMemoryHeader(retBuf) , adjustSize , filename , linenum );
#else
  SetMemoryHeader( GetMemoryHeader(retBuf) , adjustSize );
#endif

  if( adjustSize > MEMORY_HEADER_SIZE )
  {
    // @attencion MEMORY_HEADER_SIZEが2の倍数になっていない場合、領域がかぶってしまう
    //ヘッダを先頭にコピー
    //メモリ巡回なので実際の先頭アドレスをもらった際に対応可能にしておく
    memcpy( buf ,GetMemoryHeader(retBuf) , MEMORY_HEADER_SIZE );
  }
  IncAllocCount();
#if GFL_HEAP_DEBUG
  if( mDebugFlg )
  {
    const int allocatableSize = GetTotalAllocatableSize();
    if( mMinLeast > allocatableSize )
    {
      GFL_PRINT_EX( HEAP_PRINT2 , "HeapId[%d] LeastSize update[0x%08x]->[0x%08x]\n",m_heapId,mMinLeast,allocatableSize);
      mMinLeast = allocatableSize;
    }
  }
#endif
  
#if 0 //GFL_HEAP_DEBUG
  GFL_PRINT_EX( HEAP_PRINT1 , "GFL_HEAP Alloc[0x%p(0x%p)][size0x%x(0x%x)][Id %d]\n",buf,(void*)((int)buf - adjustSize),size,size+adjustSize,GetHeapId());
  GFL_PRINT_EX( HEAP_PRINT1 , "              [%s][%d]\n",filename,linenum);
  GFL_PRINT_EX( HEAP_PRINT2 , "GFL_HEAP Alloc allocCnt[%d]\n",GetAllocCount());
#endif

#if GFL_HIO_ENABLE
  AllocDump(buf);
#endif

  return retBuf;
}

//メモリ確保コア処理
#if GFL_HEAP_DEBUG
void* GflHeapBase::AllocMemory4096(  const int size , const bool isClearMemory , const char *filename , const u16 linenum )
{
  return AllocMemory4096Core( size , isClearMemory , false , filename , linenum);
}
void* GflHeapBase::AllocMemory4096Lower(  const int size , const bool isClearMemory , const char *filename , const u16 linenum )
{
  return AllocMemory4096Core( size , isClearMemory , true , filename , linenum);
}
#else
void* GflHeapBase::AllocMemory4096(  const int size , const bool isClearMemory )
{
  return AllocMemory4096Core( size , isClearMemory , false );
}
void* GflHeapBase::AllocMemory4096Lower(  const int size , const bool isClearMemory )
{
  return AllocMemory4096Core( size , isClearMemory , true );
}
#endif

#if GFL_HEAP_DEBUG
void* GflHeapBase::AllocMemory4096Core(  const int size , const bool isClearMemory ,  bool isLower , const char *filename , const u16 linenum )
#else
void* GflHeapBase::AllocMemory4096Core(  const int size , const bool isClearMemory , bool isLower )
#endif
{

  if( size == 0 )
  {
    GFL_PRINT("GFL_HEAP Alloc Memory size = zero!\n");
    //GFL_ASSERT(0); //@check
    //return NULL;
  }
  
  void *buf;
  void *retBuf;
  //アライメントからヘッダー分のオフセットを求める
  int adjustSize = 4096+MEMORY_HEADER_SIZE;
  
  if( isLower )
  {
    buf = AllocAlign(size+adjustSize,-MEMORY_HEADER_SIZE);
  }
  else
  {
    buf = AllocAlign(size+adjustSize,MEMORY_HEADER_SIZE);
  }

  if( buf == NULL )
  {
    GFL_PRINT("GFL_HEAP Alloc failed!\n");
    GFL_PRINT("size[%x+%x] least[%x]\n",size,adjustSize,GetTotalAllocatableSize());
    Dump();
#if GFL_HEAP_DEBUG
    GFL_ASSERT_MSG( buf,"GFL_HEAP Alloc failed!\n[%x:%s:%d]\n",size,filename,linenum ); //@check
#else
    GFL_ASSERT_MSG( buf,"GFL_HEAP Alloc failed!\n",size ); //@check
#endif
#if !(GFL_DEBUG)
    GFL_ASSERT_STOP(0);  //本番はASSERTSTOP=エラー
#endif
#ifdef __KLOCWORK__
    GFL_ASSERT_STOP(0);  //KLOCWORKの時もHALT
#endif
    return NULL;
  }

  //メモリクリア
  if( isClearMemory == true )
  {
    memset( buf ,0, size+adjustSize );
  }
  
  uintptr_t topPos = reinterpret_cast<uintptr_t>(buf);
  uintptr_t bufPos = topPos+MEMORY_HEADER_SIZE;
  bufPos = gfl2::math::RoundUp( bufPos , 4096 );
  GFL_ASSERT( bufPos-topPos >= MEMORY_HEADER_SIZE ); //@check 理論上ないはず
  
  adjustSize = bufPos-topPos;
  
  //ヘッダを埋め込む処理
  retBuf = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(buf) + adjustSize);
  
#if GFL_HEAP_DEBUG
  SetMemoryHeader( GetMemoryHeader(retBuf) , adjustSize , filename , linenum );
#else
  SetMemoryHeader( GetMemoryHeader(retBuf) , adjustSize );
#endif

  if( adjustSize > MEMORY_HEADER_SIZE )
  {
    //ヘッダを先頭にコピー
    //メモリ巡回なので実際の先頭アドレスをもらった際に対応可能にしておく
    memcpy( buf , GetMemoryHeader(retBuf) ,MEMORY_HEADER_SIZE );
  }
  IncAllocCount();
#if GFL_HEAP_DEBUG
  if( mDebugFlg )
  {
    const int allocatableSize = GetTotalAllocatableSize();
    if( mMinLeast > allocatableSize )
    {
      GFL_PRINT_EX( HEAP_PRINT2 , "HeapId[%d] LeastSize update[0x%08x]->[0x%08x]\n",m_heapId,mMinLeast,allocatableSize);
      mMinLeast = allocatableSize;
    }
  }
#endif
  
#if GFL_HEAP_DEBUG
  GFL_PRINT_EX( HEAP_PRINT1 , "GFL_HEAP Alloc[0x%p(0x%p)][size0x%x(0x%x)][Id %d]\n",buf,(void*)((int)buf - adjustSize),size,size+adjustSize,GetHeapId());
  GFL_PRINT_EX( HEAP_PRINT1 , "              [%s][%d]\n",filename,linenum);
  GFL_PRINT_EX( HEAP_PRINT2 , "GFL_HEAP Alloc allocCnt[%d]\n",GetAllocCount());
#endif

#if GFL_HIO_ENABLE
  AllocDump(buf);
#endif

  return retBuf;  
}



//----------------------------------------------------------------------------
/**
 *  @brief  メモリを開放
 *          GflHeapFreeMemoryBlockを使用してください
 *          確保を行ったHeapBaseを使って開放してください
 *
 *  @param  void  開放メモリ
 */
//-----------------------------------------------------------------------------
void GflHeapBase::FreeMemory( void* buf )
{
  MEMORY_HEADER *header = GetMemoryHeader(buf);
  
  //親ヒープ違い
  if( header->heapId != GetHeapId() )
  {
    GFL_ASSERT_MSG( false ,"GFL_HEAP Free failed!\n Parent heap in not match[bufId %d selfId %d]\n",header->heapId,GetHeapId() ); //@check
    
    HeapBase *parentHeap = Manager::GetHeapByHeapId( header->heapId );
    parentHeap->Free( GetMemoryTop(buf) );
    
    parentHeap->DecAllocCount();
    GFL_PRINT_EX( HEAP_PRINT1 , "GFL_HEAP Free[0x%p(0x%p)]!!\n",buf,GetMemoryTop(buf));
    return;
  }

  DecAllocCount();

#if GFL_HEAP_DEBUG
  // 解放メモリのクリア
  if( mDebugFreeMemClear == DEBUG_FREE_MEMC_ZERO ) memset( buf, 0, GetMemorySize(buf) );
  else if( mDebugFreeMemClear == DEBUG_FREE_MEMC_FF ) memset( buf, 0xFF, GetMemorySize(buf) );
#endif // GFL_HEAP_DEBUG

	//void * p = GetMemoryTop(buf);
  Free(GetMemoryTop(buf));
//  GFL_PRINT_EX( HEAP_PRINT1 , "GFL_HEAP Free[0x%p(0x%p)]\n",buf,p);
}

//----------------------------------------------------------------------------
/**
 *  @brief  自動で親ヒープを判断しメモリを開放
 *          GflHeapFreeMemoryBlockを使用してください
 *
 *  @param  void  開放メモリ
 */
//-----------------------------------------------------------------------------
void GflHeapBase::FreeMemoryAuto( void* buf )
{
  if( buf == NULL )
  {
    GFL_PRINT_EX( HEAP_PRINT1 , "GFL_HEAP buf = NULL!\n");
    //GFL_ASSERT(0); //@check
    return;
  }

  MEMORY_HEADER *header = GetMemoryHeader(buf);
  HeapBase *parentHeap = Manager::GetHeapByHeapId( header->heapId );
  //GFL_PRINT_EX( HEAP_PRINT2 , "GFL_HEAP FreeAuto[0x%p(0x%p)][Id %d]\n",buf,GetMemoryTop(buf),header->heapId);
  parentHeap->FreeMemory( buf );
}

//ヘッダーセット
#if GFL_HEAP_DEBUG
void GflHeapBase::SetMemoryHeader( MEMORY_HEADER *buf , const u32 offset , const char *filename , const u16 linenum )
#else
void GflHeapBase::SetMemoryHeader( MEMORY_HEADER *buf , const u32 offset )
#endif
{
  MEMORY_HEADER *header = static_cast<MEMORY_HEADER*>(buf);
  
  //ヘッダーに親HEAPIDとメモリ先頭までのオフセット設定
  header->heapId = GetHeapId();
  header->offset = offset;
  
#if GFL_HEAP_DEBUG
  SetMemoryHeader_UserData( reinterpret_cast<void*>(header->userData) , filename , linenum );
#else
  SetMemoryHeader_UserData( reinterpret_cast<void*>(header->userData) );
#endif
}

//ユーザーヘッダーセット(デフォルト)
#if GFL_HEAP_DEBUG
void GflHeapBase::SetMemoryHeader_UserData( void *userHeader , const char *filename , const u16 linenum )
#else
void GflHeapBase::SetMemoryHeader_UserData( void *userHeader )
#endif
{
#if GFL_HEAP_DEBUG
  MEMORY_USER_HEADER *header = static_cast<MEMORY_USER_HEADER*>(userHeader);
  header->line = linenum;
  header->id   = mDebugUserID;
	SetFilename( header->fileName, filename );
#endif
}

//----------------------------------------------------------------------------
/**
 *  @brief  アライメントを渡しヘッダー込みで実際のメモリ確保量を計算
 *          32バイト以上のアライメントを設定した場合、そのサイズ分ヘッダーが
 *          確保されます。
 *
 *  @return  int 実確保サイズ
 */
//-----------------------------------------------------------------------------
int GflHeapBase::CalcAllocateAdjustSize( const int align ) const 
{
  int adjustSize = MEMORY_HEADER_SIZE;
  if( adjustSize < gfl2::math::Abs(align) )
  {
    adjustSize = gfl2::math::Abs(align);
  }
  else
  {
    //アライメントがMEMSIZEと合わない場合
    //アサートで対応(MEMORY_HEADER_SIZEが32の限り無いと思う)
    GFL_ASSERT_MSG( (MEMORY_HEADER_SIZE % align ) == 0 , "GFL_HEAP Align size error! align[%d]" , align );
    
  }
  return adjustSize;
}

//----------------------------------------------------------------------------
/**
 *  @brief  渡されたメモリが正当かどうかをチェックします。
 *
 *  @param  const void mem 調べるメモリ
 *  @return bool チェック失敗でfalse
 */
//-----------------------------------------------------------------------------
bool GflHeapBase::CheckMemoryBlock( const void *mem )
{
  GFL_PRINT("チェックできませんでした。\n");
  return true;
}

/**
*  @brief  対象メモリが管理しているメモリかチェックする
*  @note   アドレスリストに対して検索を行うため、処理負荷に注意
*  @note   Win32版のみ動作
*
*  @param  pMemory 対象メモリ
*
*  @return 管理しているメモリならtrue、管理外ならfalseを返す
*/
b32 GflHeapBase::IsManaged( const void *pMemory )
{
  GFL_PRINT("IsManaged 未実装\n");
  return false;
}

/**
*  @brief  渡されたメモリが管理しているメモリかチェックする
*  @note   初期確保サイズの範囲チェックのみを行う
*  @note   Win32版のみ動作
*
*  @param  pMemory 調べるメモリ
*
*  @return 管理しているメモリならtrue、管理外ならfalseを返す
*/ 
b32 GflHeapBase::IsManagedFast( const void *pMemory ) const
{
  GFL_PRINT("IsManagedFast 未実装\n");
  return false;
}

#if GFL_DEBUG
//-----------------------------------------------------------------------------
/**
 * @brief ヒープ情報を出力
 * @param title この出力のタイトル
 * @note 出力されるのは、サイズ、合計空きサイズ、合計使用サイズ、断片化しているかしていないか
 * @note 断片化はCTRでの確認を推奨( Win32は後方確保がサポートされていないため )
 */
//-----------------------------------------------------------------------------
void GflHeapBase::PrintHeapInfo( c8* title)
{
#if GFL_DEBUG_PRINT
  int  totalSize     = this->GetTotalSize();
  int  totalFreeSize = this->GetTotalFreeSize();
  int  useSize       = totalSize - totalFreeSize;
  bool isFragment    = this->IsFragment();
  GFL_PRINT( "------------------------------------------------------\n");
  GFL_PRINT( "%s\n", title);
  GFL_PRINT( "Heap Info\n");
  GFL_PRINT( "  Total Size : 0x%x\n", totalSize);
  GFL_PRINT( "  Free  Size : 0x%x\n", totalFreeSize);
  GFL_PRINT( "  Use   Size : 0x%x\n", useSize);
  GFL_PRINT( "  %s\n", isFragment ? "断片化している" : "断片化していない");
  GFL_PRINT( "------------------------------------------------------\n");
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief ヒープが断片化しているか
 * @retval true 断片化している
 * @retval false 断片化していない
 */
//-----------------------------------------------------------------------------
bool GflHeapBase::IsFragment()
{
  int totalFreeSize         = this->GetTotalFreeSize();
  int totalAllocatableSize  = this->GetTotalAllocatableSize();
  return ( (totalFreeSize - totalAllocatableSize) > gfl2::heap::MEMORY_HEADER_SIZE );
}
#endif // GFL_DEBUG

//ヘッダー位置取得
MEMORY_HEADER* GflHeapBase::GetMemoryHeader( const void *buf )
{
  return reinterpret_cast<MEMORY_HEADER*>(reinterpret_cast<uintptr_t>(buf)-MEMORY_HEADER_SIZE);
}

//実際のメモリ先頭確保
void* GflHeapBase::GetMemoryTop( const void *buf )
{
  MEMORY_HEADER* memHead = GetMemoryHeader(buf);
  return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(buf)-memHead->offset);
}

//ヘッダーサイズ取得(GetMemoryTopの前のアドレスを渡す
int GflHeapBase::GetMemoryHeaderSize( const void *buf )
{
  MEMORY_HEADER* memHead = GetMemoryHeader(buf);
  return memHead->offset;
}

void GflHeapBase::IncAllocCount(void)
{
#if !defined(__cplusplus_cli) // C++/CLIはネイティブスレッドを使えないらしいのでかこった
  mAllocCntLock.lock();
  m_allocCount++; 
  mAllocCntLock.unlock();
#else
  m_allocCount++;
#endif
}

void GflHeapBase::DecAllocCount(void)
{ 
#if !defined(__cplusplus_cli) // C++/CLIはネイティブスレッドを使えないらしいのでかこった
  mAllocCntLock.lock();
  m_allocCount--; 
  mAllocCntLock.unlock();
#else
  m_allocCount--;
#endif
}

#if GFL_HEAP_DEBUG
void GflHeapBase::SetFilename( char * buf, const char * filename )
{
  //名前がフルパスで入っているので・・・
  //\記号までか文字列分をヘッダに格納
  int nameLen = strlen(filename)-3;
  buf[MEMORY_USER_HEADER_FILE_BUFLEN-1] = '\0';
  for( int i=MEMORY_USER_HEADER_FILE_BUFLEN-2  ;i>=0;i-- )
  {
    if( nameLen >= 0 && filename[nameLen] != '\\' )
    {
      buf[i] = filename[nameLen];
      nameLen--;
    }
    else
    {
      buf[i] = ' ';
    }
  }
}
void GflHeapBase::SetCreateData( const char * filename, const int linenum, const char *heapName, bool parent )
{
  gfl2::std::MemClear(&createData,sizeof(MEMORY_USER_CREATE_DATA));

	createData.parentFlag = parent;
	createData.sourceFile.line = linenum;
  if( heapName )
  {
    gfl2::std::MemCopy(heapName,createData.heapName,DEBUG_HEAP_NAME_LEN);
  }
  SetFilename( createData.sourceFile.fileName, filename );
}
const char * GflHeapBase::GetCreateDataFilename(void)
{
	return createData.sourceFile.fileName;
}
int GflHeapBase::GetCreateDataLine(void)
{
	return createData.sourceFile.line;
}
bool GflHeapBase::GetCreateParentFlag(void)
{
	return createData.parentFlag;
}
void GflHeapBase::SetDebugUserID( u8 id )
{
  mDebugUserID = id;
}
u8 GflHeapBase::GetDebugUserID(void)
{
  return mDebugUserID;
}

const char* GflHeapBase::GetHeapName(void)
{
  return createData.heapName;
}

#endif	// GFL_HEAP_DEBUG

#if defined(GF_PLATFORM_WIN32)
void GflHeapBase::SetCreateHeapScale( int scale )
{
  mCreateHeapScale = scale;
  if( m_lowerHandle )
  {
    m_lowerHandle->SetCreateHeapScale(scale);
  }
}
#endif

GFL_NAMESPACE_END(heap)
GFL_NAMESPACE_END(gfl2)


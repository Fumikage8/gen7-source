//======================================================================
/**
 * @file	gfl2_CtrExpHeap.cpp
 * @brief	3DSの拡張ヒープ
 * @author	ariizumi
 * @data	10/10/18
 */
//======================================================================

#ifdef GF_PLATFORM_CTR

#include <nn.h>
//#include <nn/Result.h>
//#include <nn/config.h>
//#include <nn/assert.h>
#include <nn/fnd/fnd_ExpHeap.h>
#include <nn/init/init_Allocator.h>
#include <nn/os/os_LockPolicy.h>
#include <nn/os/os_Memory.h>
#include <nn/os/os_MemoryBlock.h>
//#include <nn/os/os_MemoryMapSelect.h>
#include <nn/os/os_CriticalSection.h>
//#include <nn/svc/svc_stub.h>
//#include <nn/util/util_Result.h>
//#include <nn/util/util_TypeTraits.h>

#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

#include <heap/include/gfl2_CtrExpHeap.h>
#include "gfl2_heap_local.h"

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(heap)

#if GFL_HEAP_DEBUG
b8 CTRExpHeap::m_debugReverseAllocFlag = false;
#ifdef GFL_DEBUG_HIO_LOG_IN_MEMORY
b8 CTRExpHeap::m_debugHioDumpInMemory = false;
HEAPID CTRExpHeap::m_dumpLogHeapId = 0;
#endif
#endif

//======================================================================
//	define
//======================================================================
//#pragma mark [> define

//======================================================================
//	enum
//======================================================================
//#pragma mark [> enum

//======================================================================
//	Class HeapCTRExp
//======================================================================
//#pragma mark [> HeapCTRExp
void CTRExpHeap::CreateHeap( void *buf , int size , void *param )
{
  GFL_UNUSED(param);
  m_heapHandle = new (reinterpret_cast<void*>(handleBuff)) CtrExpHeapClass((uptr)buf,size);

#if GFL_HEAP_DEBUG
  m_debugNowReverseAllocFlag = m_debugReverseAllocFlag;
#endif
}

void CTRExpHeap::DeleteHeap(void)
{
#if GFL_HIO_ENABLE
  if(m_dumpFlag)
  {
    hfprintf(&m_hostFile, "-----------------CTRExpHeap Log End-----------------\n");
    m_hostFile.Close();
    m_dumpFlag = false;
  }
#endif // GFL_HIO_ENABLE

  m_heapHandle->Finalize();
  //deleteはできないので明示的にデストラクタ
  m_heapHandle->~CtrExpHeapClass();
}

void* CTRExpHeap::AllocAlign( const int size , const int align )
{
#if GFL_HEAP_DEBUG
  if( m_debugNowReverseAllocFlag )
  {
    void *buf = m_heapHandle->Allocate(size,-align);
    return buf;
  }
  else
#endif
  {
    void *buf = m_heapHandle->Allocate(size,align);
    return buf;
  }
}

#if GFL_HIO_ENABLE
void CTRExpHeap::AllocDump( void* buf )
{
  if (m_dumpFlag)
  {
    MEMORY_HEADER *header = reinterpret_cast<MEMORY_HEADER*>(buf);
    MEMORY_USER_HEADER *userHeader = reinterpret_cast<MEMORY_USER_HEADER*>(header->userData);
    hfprintf(&m_hostFile, "ALC 0x%08p 0x%-6x %24s %4d %3d\n", buf, m_heapHandle->GetSizeOf(buf), userHeader->fileName, userHeader->line, userHeader->id);
  }
}
#endif // GFL_HIO_ENABLE

void CTRExpHeap::Free( void* buf )
{
#if GFL_HIO_ENABLE
  if (m_dumpFlag)
  {
    MEMORY_HEADER *header = reinterpret_cast<MEMORY_HEADER*>(buf);
    MEMORY_USER_HEADER *userHeader = reinterpret_cast<MEMORY_USER_HEADER*>(header->userData);
    hfprintf(&m_hostFile, "DEL 0x%08p 0x%-6x %24s %4d %3d\n", buf, m_heapHandle->GetSizeOf(buf), userHeader->fileName, userHeader->line, userHeader->id);
  }
#endif // GFL_HIO_ENABLE
  m_heapHandle->Free(buf);
}

void CTRExpHeap::dump( bool chk_id, u8 id )
{
  void *cls = this;
  GFL_RELEASE_PRINT("-----------------CTRExpHeap Dump-----------------\n");
  GFL_RELEASE_PRINT("HEAPID         :%d\n",GetHeapId());
  GFL_RELEASE_PRINT("HeapCreated    :%s(%d)\n",GetCreateDataFilename(),GetCreateDataLine());
  GFL_RELEASE_PRINT("HeapName       :%s\n",GetHeapName());
#if GFL_HEAP_DEBUG
	if( this->GetCreateParentFlag() == true ){
		GFL_RELEASE_PRINT("PARENT HEAPID  :%d\n",GetMemoryHeader(this)->heapId);
	}else{
		GFL_RELEASE_PRINT("PARENT HEAPID  :--\n");
	}
#endif
  GFL_RELEASE_PRINT("TotalSize      :0x%08x\n",GetTotalSize());
  GFL_RELEASE_PRINT("FreeSize       :0x%08x\n",GetTotalFreeSize());
  GFL_RELEASE_PRINT("AllocatableSize:0x%08x\n",GetTotalAllocatableSize());
  
  GFL_RELEASE_PRINT("Address--- Size---- FileName---------------- Line\n");
  if( chk_id ){ // IDチェックを行うバージョン
    m_heapHandle->VisitAllBlocks( CTRExpHeap::BlockVisitorCallbackID , id );
  }else{        // 通常バージョン
    m_heapHandle->VisitAllBlocks( CTRExpHeap::BlockVisitorCallback , reinterpret_cast<uptr>(cls) );
  }
  GFL_RELEASE_PRINT("Address--- Size---- FileName---------------- Line\n");
  GFL_RELEASE_PRINT("-----------------CTRExpHeap Dump-----------------\n");
}

void CTRExpHeap::Dump(void)
{
  dump( false, 0 );
}
void CTRExpHeap::DumpId(u8 id)
{
  dump( true, id );
}

//Dump用コールバック
void CTRExpHeap::BlockVisitorCallback( void* pBlock , const nn::fnd::ExpHeapBase *expHeapBase , uptr param )
{
#if GFL_DEBUG //Warnningが消えないので、defineで処理自体を抑制
  GFL_UNUSED( param );
  GFL_UNUSED( expHeapBase );
  
  MEMORY_HEADER *header = static_cast<MEMORY_HEADER*>(pBlock);
  MEMORY_USER_HEADER *userHeader = reinterpret_cast<MEMORY_USER_HEADER*>(header->userData);
  GFL_RELEASE_PRINT("0x%08p 0x%-6x %24s %4d %3d\n",pBlock,expHeapBase->GetSizeOf(pBlock),userHeader->fileName,userHeader->line, userHeader->id);
#endif
}
void CTRExpHeap::BlockVisitorCallbackID( void* pBlock , const nn::fnd::ExpHeapBase *expHeapBase , uptr param )
{
  GFL_UNUSED( expHeapBase );
  u8 id = param;

  
  MEMORY_HEADER *header = static_cast<MEMORY_HEADER*>(pBlock);
  MEMORY_USER_HEADER *userHeader = reinterpret_cast<MEMORY_USER_HEADER*>(header->userData);
  if( userHeader->id == id ){
    GFL_RELEASE_PRINT("0x%08p 0x%-6x %24s %4d %3d\n",pBlock,expHeapBase->GetSizeOf(pBlock),userHeader->fileName,userHeader->line, userHeader->id);
  }
}

#if GFL_HIO_ENABLE
void CTRExpHeap::BlockVisitorHioCallback( void* pBlock , const nn::fnd::ExpHeapBase *expHeapBase , uptr param )
{
  GFL_UNUSED( expHeapBase );

  MEMORY_HEADER *header = static_cast<MEMORY_HEADER*>(pBlock);
  MEMORY_USER_HEADER *userHeader = reinterpret_cast<MEMORY_USER_HEADER*>(header->userData);

  CTRExpHeap *self = reinterpret_cast<CTRExpHeap*>(param);

  self->hfprintf( &self->m_hostFile, "0x%08p 0x%-6x %24s %4d %3d\n",pBlock,expHeapBase->GetSizeOf(pBlock),userHeader->fileName,userHeader->line, userHeader->id);
}

void CTRExpHeap::BlockVisitorHioCallbackID( void* pBlock , const nn::fnd::ExpHeapBase *expHeapBase , uptr param )
{
  return;
}
#endif // GFL_HIO_ENABLE

void CTRExpHeap::SystemDump(void)
{
  m_heapHandle->Dump();
}

bool CTRExpHeap::Check(void)
{
  return m_heapHandle->CheckHeap();
}

//----------------------------------------------------------------------------
/**
 *  @brief  ヒープのサイズを取得します
 *
 *  @return  int サイズ
 */
//-----------------------------------------------------------------------------
int CTRExpHeap::GetTotalSize(void)
{
  return static_cast<int>(m_heapHandle->GetTotalSize());
}

//----------------------------------------------------------------------------
/**
 *  @brief  ヒープの合計空きサイズを取得します
 *
 *  @return  int サイズ
 */
//-----------------------------------------------------------------------------
int CTRExpHeap::GetTotalFreeSize(void)
{
  return static_cast<int>(m_heapHandle->GetTotalFreeSize());
}

//----------------------------------------------------------------------------
/**
 *  @brief  ヒープで確保できる最大のサイズを取得します
 *
 *  @return  int サイズ
 */
//-----------------------------------------------------------------------------
int CTRExpHeap::GetTotalAllocatableSize(void)
{
  //管理領域を引くように修正
  return static_cast<int>(m_heapHandle->GetAllocatableSize())-MEMORY_HEADER_SIZE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ヒープの先頭アドレスを返します(管理領域を含まない
 *
 *  @return  void* 
 */
//-----------------------------------------------------------------------------
void* CTRExpHeap::GetStartAddress(void)
{
  return m_heapHandle->GetStartAddress();
}

//----------------------------------------------------------------------------
/**
 *  @brief  確保したメモリのリサイズ
 *
 *  @param  void* buf リサイズするメモリ  
 *  @param  int       変更後のサイズ
 */
//-----------------------------------------------------------------------------
void CTRExpHeap::ResizeMemory( void* buf , const int newSize)
{
#if GFL_HIO_ENABLE
  if (m_dumpFlag)
  {
    MEMORY_HEADER *header = reinterpret_cast<MEMORY_HEADER*>(buf);
    MEMORY_USER_HEADER *userHeader = reinterpret_cast<MEMORY_USER_HEADER*>(header->userData);
    hfprintf(&m_hostFile, "RES 0x%08p 0x%-6x %24s %4d %3d\n", buf, newSize, userHeader->fileName, userHeader->line, userHeader->id);
  }
#endif // GFL_HIO_ENABLE

  const bool ret = m_heapHandle->ResizeBlock( GetMemoryTop(buf) , newSize+GetMemoryHeaderSize(buf) );
  GFL_ASSERT_MSG( ret , "GFL_HEAP Resize memory is failed![0x%x][size:%x]\n",buf,newSize );
}

//----------------------------------------------------------------------------
/**
 *  @brief  確保された領域のサイズを取得(HeapManagerにもあります)
 *          継承していなければ非対応
 *
 *  @param  const void mem 調べるメモリ
 *  @return int メモリサイズ(-1は失敗)
 */
//-----------------------------------------------------------------------------
int CTRExpHeap::GetMemorySize( const void *mem )
{
  const MEMORY_HEADER *header = GetMemoryHeader(mem);
  const int size = static_cast<int>(m_heapHandle->GetSizeOf( GetMemoryTop(mem) ));
  return size - header->offset;
}

//----------------------------------------------------------------------------
/**
 *  @brief  渡されたメモリが正当かどうかをチェックします。
 *
 *  @param  const void mem 調べるメモリ
 *  @return bool チェック失敗でfalse
 */
//-----------------------------------------------------------------------------
bool CTRExpHeap::CheckMemoryBlock( const void *mem )
{
  return m_heapHandle->CheckBlock( GetMemoryTop(mem) );
}


//----------------------------------------------------------------------------
/**
 *  @brief  すべてのメモリを解放します。
 *
 *  @return bool 失敗したらfalse
 */
//-----------------------------------------------------------------------------
bool CTRExpHeap::FreeAll(void)
{
  void *cls = this;
  m_heapHandle->VisitAllBlocks( CTRExpHeap::BlockVisitorCallback_Free , reinterpret_cast<uptr>(cls) );
  
  return true;
}

//FreeAll用コールバック
void CTRExpHeap::BlockVisitorCallback_Free( void* pBlock , const nn::fnd::ExpHeapBase *expHeapBase , uptr param )
{
  CTRExpHeap *heapCls = reinterpret_cast<CTRExpHeap*>(param);

  heapCls->DecAllocCount();
  heapCls->Free(pBlock);
}

#if GFL_HIO_ENABLE
//----------------------------------------------------------------------------
/**
 *  @brief  HIO経由のメモリダンプを開始します
 *  @detail PC側ファイルをオープンします
 *          クローズはEndHioDumpの呼び出し、もしくはデストラクタで行われます
 */
//-----------------------------------------------------------------------------
void CTRExpHeap::StartHioDump()
{
  char filename[256];

  sprintf(filename, "CTRExpHeapDump%d", GetHeapId());

  //同じローカルヒープIDのものなど同じファイルを上書いてしまうため、上書きしないことにした
  bool isExist = false;
  nn::Result ret =  m_hostFile.Exists( &isExist, filename );
  if( isExist || ret.IsFailure() ) 
  { 
    GFL_PRINT("!!! HioDump Log File Exist [%s] !!!\n", filename);
    return;
  }

#ifdef GFL_DEBUG_HIO_LOG_IN_MEMORY
  m_debugNowHioDumpInMemory = m_debugHioDumpInMemory;
  m_nowDumpLogHeapId = m_dumpLogHeapId;
  if( m_debugNowHioDumpInMemory == 0 )
#endif
  {
    m_hostFile.Open(filename, nn::hio::CTR::HostFile::ACCESS_MODE_WRITE, nn::hio::CTR::HostFile::OPEN_DISP_CREATE_ALWAYS);
  }

  m_dumpFlag = true;
  HioDump();
  hfprintf(&m_hostFile, "-----------------CTRExpHeap Log Start-----------------\n");
}

//----------------------------------------------------------------------------
/**
 *  @brief  HIO経由のメモリダンプを終了します
 */
//-----------------------------------------------------------------------------
void CTRExpHeap::EndHioDump()
{
  if( m_dumpFlag )
  {
    m_dumpFlag = false;

#ifdef GFL_DEBUG_HIO_LOG_IN_MEMORY
    if( m_debugNowHioDumpInMemory != 0 )
    {
      char filename[256];
      sprintf(filename, "CTRExpHeapDump%d", GetHeapId());
      m_hostFile.Open(filename, nn::hio::CTR::HostFile::ACCESS_MODE_WRITE, nn::hio::CTR::HostFile::OPEN_DISP_CREATE_ALWAYS);

      //メモリの内容をすべて書き込み
      m_debugNowHioDumpInMemory = 0;
      WriteLog(&m_hostFile);
    }
#endif

    hfprintf(&m_hostFile, "-----------------CTRExpHeap Log End-----------------\n");
    m_hostFile.Close();
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  HIO経由のメモリダンプを中断しファイルを消す
 */
//-----------------------------------------------------------------------------
void CTRExpHeap::AbortHioDump()
{
  if( m_dumpFlag )
  {
    m_dumpFlag = false;

#ifdef GFL_DEBUG_HIO_LOG_IN_MEMORY
    if( m_debugNowHioDumpInMemory == 0 )
#endif
    {
      m_hostFile.Close();

      char filename[256];
      sprintf(filename, "CTRExpHeapDump%d", GetHeapId());
      nn::hio::DeleteFile(filename );
    }
  }
}

void CTRExpHeap::HioDump( bool chk_id, u8 id )
{
  hfprintf(&m_hostFile, "-----------------CTRExpHeap Dump-----------------\n");
  hfprintf(&m_hostFile, "HEAPID         :%d\n",GetHeapId());
  hfprintf(&m_hostFile, "HeapCreated    :%s(%d)\n",GetCreateDataFilename(),GetCreateDataLine());
  hfprintf(&m_hostFile, "HeapName       :%s\n",GetHeapName());
#if GFL_HEAP_DEBUG
  if( this->GetCreateParentFlag() == true ){
    hfprintf(&m_hostFile, "PARENT HEAPID  :%d\n",GetMemoryHeader(this)->heapId);
  }else{
    hfprintf(&m_hostFile, "PARENT HEAPID  :--\n");
  }
#endif
  hfprintf(&m_hostFile, "Address        :0x%08x\n",GetStartAddress());
  hfprintf(&m_hostFile, "TotalSize      :0x%08x\n",GetTotalSize());
  hfprintf(&m_hostFile, "FreeSize       :0x%08x\n",GetTotalFreeSize());
  hfprintf(&m_hostFile, "AllocatableSize:0x%08x\n",GetTotalAllocatableSize());

  hfprintf(&m_hostFile, "Address--- Size---- FileName---------------- Line\n");
  m_heapHandle->VisitAllBlocks( CTRExpHeap::BlockVisitorHioCallback , reinterpret_cast<uptr>(this) );
  hfprintf(&m_hostFile, "Address--- Size---- FileName---------------- Line\n");
  hfprintf(&m_hostFile, "-----------------CTRExpHeap Dump-----------------\n");
}

void CTRExpHeap::hfprintf(nn::hio::CTR::HostFile *file, const char* pszFormat, ...)
{
  va_list argp;
  va_start(argp, pszFormat);

  char buf[512];
  size_t length = vsprintf(buf, pszFormat, argp); // vsprintf_sがないので代用、デバッグのみ

  va_end(argp);

#ifdef GFL_DEBUG_HIO_LOG_IN_MEMORY
  if( m_debugNowHioDumpInMemory != 0 )
  {
    StoreLog( buf, length );
  }
  else
#endif
  {
    file->Write(buf, length);
  }
}

#ifdef GFL_DEBUG_HIO_LOG_IN_MEMORY
void CTRExpHeap::StoreLog( char * string, int length )
{
  gfl2::heap::HeapBase * heap = gfl2::heap::Manager::GetHeapByHeapId( m_nowDumpLogHeapId );
  DumpLog * log = static_cast<DumpLog*>( GflHeapAllocMemoryAlign( heap, sizeof(DumpLog) + length + 1, 4 ) );  //+1はnull文字分
  
  log->length = length;
  log->next = NULL;
  memcpy(log->log, string, log->length);
  log->log[log->length] = '\0'; //NULL文字を入れる

  if( m_dumpLogLast )
  {
    m_dumpLogLast->next = log;
    m_dumpLogLast = log;
  }
  else
  {
    m_dumpLogList = log;
    m_dumpLogLast = log;
  }
}
void CTRExpHeap::WriteLog( nn::hio::CTR::HostFile *file )
{
  DumpLog *now = m_dumpLogList;
  while( now != NULL )
  {
    DumpLog *next = now->next;
    file->Write(now->log, now->length);
    GflHeapSafeFreeMemory( now );
    now = next;
  }
}
#endif //GFL_DEBUG_HIO_LOG_IN_MEMORY
#endif // GFL_HIO_ENABLE

GFL_NAMESPACE_END(heap)
GFL_NAMESPACE_END(gfl2)

#endif //GF_PLATFORM_CTR


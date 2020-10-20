//======================================================================
/**
 * @file	gfl_CtrExpHeap.cpp
 * @brief	3DSの拡張ヒープ
 * @author	ariizumi
 * @data	10/10/18
 */
//======================================================================
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

#include <gflib.h>
#include "gfl_HeapLocal.h"

namespace gfl
{
namespace heap
{

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
}

void CTRExpHeap::DeleteHeap(void)
{
  m_heapHandle->Finalize();
  //deleteはできないので明示的にデストラクタ
  m_heapHandle->~CtrExpHeapClass();
}

void* CTRExpHeap::AllocAlign( const int size , const int align )
{
  void *buf = m_heapHandle->Allocate(size,align);
  return buf;
}

void CTRExpHeap::Free( void* buf )
{
  m_heapHandle->Free(buf);
}

void CTRExpHeap::dump( bool chk_id, u8 id )
{
  void *cls = this;
  GFL_PRINT("-----------------CTRExpHeap Dump-----------------\n");
  GFL_PRINT("HEAPID         :%d\n",GetHeapId());
#if GFL_HEAP_DEBUG
	if( this->GetCreateParentFlag() == true ){
		GFL_PRINT("PARENT HEAPID  :%d\n",GetMemoryHeader(this)->heapId);
	}else{
		GFL_PRINT("PARENT HEAPID  :--\n");
	}
#endif
  GFL_PRINT("TotalSize      :0x%08x\n",GetTotalSize());
  GFL_PRINT("FreeSize       :0x%08x\n",GetTotalFreeSize());
  GFL_PRINT("AllocatableSize:0x%08x\n",GetTotalAllocatableSize());
  
  GFL_PRINT("Address--- Size---- FileName---------------- Line\n");
  if( chk_id ){ // IDチェックを行うバージョン
    m_heapHandle->VisitAllBlocks( CTRExpHeap::BlockVisitorCallbackID , id );
  }else{        // 通常バージョン
    m_heapHandle->VisitAllBlocks( CTRExpHeap::BlockVisitorCallback , reinterpret_cast<uptr>(cls) );
  }
  GFL_PRINT("Address--- Size---- FileName---------------- Line\n");
  GFL_PRINT("-----------------CTRExpHeap Dump-----------------\n");
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
  GFL_UNUSED( param );
  GFL_UNUSED( expHeapBase );
  
  MEMORY_HEADER *header = static_cast<MEMORY_HEADER*>(pBlock);
  MEMORY_USER_HEADER *userHeader = reinterpret_cast<MEMORY_USER_HEADER*>(header->userData);
  GFL_PRINT("0x%08p 0x%-6x %24s %4d %3d\n",pBlock,expHeapBase->GetSizeOf(pBlock),userHeader->fileName,userHeader->line, userHeader->id);
}
void CTRExpHeap::BlockVisitorCallbackID( void* pBlock , const nn::fnd::ExpHeapBase *expHeapBase , uptr param )
{
  GFL_UNUSED( expHeapBase );
  u8 id = param;

  
  MEMORY_HEADER *header = static_cast<MEMORY_HEADER*>(pBlock);
  MEMORY_USER_HEADER *userHeader = reinterpret_cast<MEMORY_USER_HEADER*>(header->userData);
  if( userHeader->id == id ){
    GFL_PRINT("0x%08p 0x%-6x %24s %4d %3d\n",pBlock,expHeapBase->GetSizeOf(pBlock),userHeader->fileName,userHeader->line, userHeader->id);
  }
}

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
  const bool ret = m_heapHandle->ResizeBlock( GetMemoryTop(buf) , newSize+GetMemoryHeaderSize(buf) );
  GFL_ASSERT_MSG( ret , "GFL_HEAP Resize memory is failued![0x%x][size:%x]\n",buf,newSize );
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
 *  @brief  HIOでのメモリ状況の監視のON/OFF
 *
 *  @param[in]  devHeap デバイスヒープ
 *  @param[in]  flag        true:監視開始 false:監視終了
 */
//-----------------------------------------------------------------------------
void  CTRExpHeap::HioDump( bool flag )
{ 
  if( mHioDump == flag ) return;
  mHioDump = flag;
  if( flag != false )
  { 
    gfl::hio::HioSharedMemory* hsm = gfl::hio::HioSharedMemory::GetInstance();
    if( hsm )
    { 
      char  read_buf[ 4 ];
      hsm->Read( read_buf, 4, 0 );
      if( strncmp( read_buf, "FCC", 3 ) == 0 )
      { 
        hsm->Initialize();
        hsm->Write( "XXX", 3, 0 );
      }
    }
    void *cls = this;
    m_heapHandle->VisitAllBlocks( CTRExpHeap::BlockVisitorHioCallback, reinterpret_cast<uptr>(cls) );
  }
}

void CTRExpHeap::BlockVisitorHioCallback( void* pBlock , const nn::fnd::ExpHeapBase *expHeapBase , uptr param )
{
  GFL_UNUSED( param );
  GFL_UNUSED( expHeapBase );

  CTRExpHeap *heapCls = reinterpret_cast<CTRExpHeap*>(param);
  MEMORY_HEADER *header = static_cast<MEMORY_HEADER*>(pBlock);
  MEMORY_USER_HEADER *userHeader = reinterpret_cast<MEMORY_USER_HEADER*>(header->userData);

  heapCls->HioLineAdd( pBlock, expHeapBase->GetSizeOf(pBlock), userHeader->fileName, userHeader->line, userHeader->align );
}
#endif

} //namespace heap
} //namespace gfl

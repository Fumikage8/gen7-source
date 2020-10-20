/**
* @brief  ファイルリードクラス
* @file   FieldTerrainFactory.h
* @author ikuta_junya
* @data   2015.04.19
*/

#if !defined( __READVIEWERDATA_H_INCLUDED__ )
#define __READVIEWERDATA_H_INCLUDED__
#pragma once
// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <gfx/include/gfl2_GLMemory.h>



//-----------------------------------------------------------------------------
/*
*          内部クラス宣言(リソースアロケータ)
*          @class GLAllocator
*/
//-----------------------------------------------------------------------------
class GLResourceAllocator : public gfl2::gfx::IGLAllocator
{
	public:
		
	//----------------------------------------------------------------------------
	/**
	*  @brief  コンストラクタ
	*/
	//-----------------------------------------------------------------------------
	GLResourceAllocator():IGLAllocator(),m_pHeap(NULL) {}

	//----------------------------------------------------------------------------
	/**
	*  @brief  デストラクタ
	*/
	//-----------------------------------------------------------------------------
	virtual			~GLResourceAllocator(){}
		
  //----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  void  init(gfl2::heap::HeapBase* pHeap){ m_pHeap = pHeap; }

	//----------------------------------------------------------------------------
	/**
	*  @brief  メインメモリの取得
	*  @param  取得したいメモリサイズ
	*  @param  アライメント
	*  @return メモリ
	*/
	//-----------------------------------------------------------------------------
#if GFL_DEBUG
  virtual void*  _SystemMemoryMallocAlign(u32 size, u32 alignment,const char *filename , const int linenum){ return m_pHeap->AllocMemoryAlign(size,alignment,filename,linenum);}
#else
  virtual void*  SystemMemoryMallocAlign(u32 size, u32 alignment){ return GflHeapAllocMemoryAlign( m_pHeap , size , alignment ); }
#endif

	//----------------------------------------------------------------------------
	/**
	*  @brief  メインメモリの解放
	*  @param  解放したいメモリのポインタ
	*/
	//-----------------------------------------------------------------------------
	virtual void	SystemMemoryFree(void* const address){ return GflHeapFreeMemory( address ); }

	//----------------------------------------------------------------------------
	/**
	*  @brief  ビデオメモリの取得
	*  @param  取得したいメモリサイズ
	*  @param  アライメント
	*  @return メモリ
	*/
	//-----------------------------------------------------------------------------
	virtual void*	LocalMemoryMalloc(u32 size, u32 alignment){ return NULL; }

	//----------------------------------------------------------------------------
	/**
	*  @brief  ビデオメモリの解放
	*  @param  解放したいメモリのポインタ
	*/
	//-----------------------------------------------------------------------------
	virtual void	LocalMemoryFree(void* const address){ }

	private:
    
    gfl2::heap::HeapBase *m_pHeap;

};

extern void* ReadData( const char* pFilePath,GLResourceAllocator *pAllocator );

	#endif // __READVIEWERDATA_H_INCLUDED__
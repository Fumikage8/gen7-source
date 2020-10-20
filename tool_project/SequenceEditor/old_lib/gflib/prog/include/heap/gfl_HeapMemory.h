#ifndef __GFL_HEAPMEMORY_H__
#define __GFL_HEAPMEMORY_H__
//======================================================================
/**
 * @file	gfl_HeapMemory.h
 * @brief	new deleteのオーバーライド
 * @author	ariizumi
 * @data	10/10/19
 */
//======================================================================
#pragma once

#include <new>
#include <nn.h>
#undef  GFL_DEBUG
#define  GFL_DEBUG 0
#undef  GFL_RELEASE
#define GFL_RELEASE

extern "C" void gfinitStartup( size_t device_memory_size, size_t heap_memory_size );

//------------------------------------------------------------------
//  Heapを使うnew
//  GFL_NEWもGFL_NEW_ARRAYも一緒だが、形式をあわせるためにGFL_DELETEを
//  用意した際、DELETEとDELETE_ARRAYを別ける必要が出たのでNEWも別けた
//------------------------------------------------------------------
#if GFL_DEBUG
#define GFL_NEW(heap)  new(heap, __FILE__, __LINE__ )
#define GFL_NEW_ARRAY(heap)  new(heap, __FILE__, __LINE__ )
#define GFL_NEW_LOW(heap)  new(heap,true, __FILE__, __LINE__ )
#define GFL_NEW_LOW_ARRAY(heap)  new(heap,true, __FILE__, __LINE__ )
#else
#define GFL_NEW(heap)  new(heap)
#define GFL_NEW_ARRAY(heap)  new(heap)
#define GFL_NEW_LOW(heap)  new(heap,true)
#define GFL_NEW_LOW_ARRAY(heap)  new(heap,true)
#endif

//形式をあわせるため
#define GFL_DELETE delete
#define GFL_DELETE_ARRAY delete[]
#define GFL_SAFE_DELETE( p )       { if( p ) { GFL_DELETE( p );		(p)=0; } }
#define GFL_SAFE_DELETE_ARRAY( p ) { if( p ) { GFL_DELETE_ARRAY( p );		(p)=0; } }

// placement new, 配置new, 配置構文new  // 使用するにはプログラムディレクタの許可をもらって下さい。  // placement deleteは用意していません。
#define GFL_NEW_PLACEMENT new


//------------------------------------------------------------------
//------------------------------------------------------------------
//void* operator new
//(size_t size, const char * filename, int line_no) throw();

//------------------------------------------------------------------
//------------------------------------------------------------------
//void* operator new[]
//(size_t size, const char * filename, int line_no ) throw();

#if GFL_DEBUG
void* operator new (size_t size, gfl::heap::HeapBase *heapBase , const char * filename, int line_no) throw();
void* operator new[] (size_t size, gfl::heap::HeapBase *heapBase , const char * filename, int line_no ) throw();
void* operator new (size_t size, gfl::heap::HeapBase *heapBase , const bool isLow , const char * filename, int line_no) throw();
void* operator new[] (size_t size, gfl::heap::HeapBase *heapBase , const bool isLow , const char * filename, int line_no ) throw();
#else
void* operator new (size_t size, gfl::heap::HeapBase *heapBase ) throw();
void* operator new[] (size_t size, gfl::heap::HeapBase *heapBase ) throw();
void* operator new (size_t size, gfl::heap::HeapBase *heapBase , const bool isLow ) throw();
void* operator new[] (size_t size, gfl::heap::HeapBase *heapBase , const bool isLow ) throw();
#endif



#if GFL_DEBUG
//デフォルトNewを禁止する
void GflDebugDefaultHeapInterdiction(bool flg);
bool IsGflDebugDefaultHeapInterdiction(void);
#endif

#endif // __GFL_HEAPMEMORY_H__

/**
* @file    gfl2_NewAndDeleteOperators.h
* @brief   new/delete演算子のOverLoadとOverRideの実装を記述する
* @author  ikuta_junya
* @date    2014.09.15
*/

#if !defined( GFLIB2_HEAP_GFL2_NEWANDDELETEOPERATORS_H_INCLUDED )
#define GFLIB2_HEAP_GFL2_NEWANDDELETEOPERATORS_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN( gfl2 );
GFL_NAMESPACE_BEGIN( heap );

#if GFL_DEBUG
GFL_NAMESPACE_BEGIN( debug );

/**
* @brief   通常new禁止アサートを有効化する
* @note Win32版では機能しません
*
* @return  無し 
*/
void EnableDefaultNewAssert( void );

/**
* @brief   通常new禁止アサートを無効化する
* @note Win32版では機能しません
*
* @return  無し 
*/
void DisableDefaultNewAssert( void );

/**
* @brief   通常new禁止アサートが有効か確認する
* @note Win32版では機能しません
*
* @return  trueなら使用可能、falseなら使用禁止
*/
b32 IsDefaultNewAssertEnabled( void );

GFL_NAMESPACE_END( debug );
#endif // GFL_DEBUG

GFL_NAMESPACE_END( heap );
GFL_NAMESPACE_END( gfl2 );

//
// new演算子 オーバーライド
//

#if GFL_DEBUG
void* operator new ( size_t size, gfl2::heap::HeapBase* pHeapBase, const char* pFileName, s32 lineNo ) throw();
void* operator new[] ( size_t size, gfl2::heap::HeapBase* pHeapBase, const char* pFileName, s32 lineNo ) throw();
void* operator new ( size_t size, gfl2::heap::HeapBase* pHeapBase, const b32 isLow, const char * pFileName, s32 lineNo ) throw();
void* operator new[] ( size_t size, gfl2::heap::HeapBase* pHeapBase, const b32 isLow, const char * pFileName, s32 lineNo ) throw();
#else
void* operator new ( size_t size, gfl2::heap::HeapBase* pHeapBase ) throw();
void* operator new[] ( size_t size, gfl2::heap::HeapBase* pHeapBase ) throw();
void* operator new ( size_t size, gfl2::heap::HeapBase* pHeapBase, const b32 isLow ) throw();
void* operator new[] ( size_t size, gfl2::heap::HeapBase* pHeapBase, const b32 isLow ) throw();
#endif // #if GFL_DEBUG


//
// delete演算子 オーバーライド
// 仮引数が一致するdeleteがない場合、new対象オブジェクトのコンストラクターが例外をスローした場合にメモリの開放が行われない(warning C4291)
//
#ifdef  GF_PLATFORM_WIN32 //ただしCTRではWarningが出るためWinのみとする
#if GFL_DEBUG
void operator delete ( void* p, gfl2::heap::HeapBase* pHeapBase, const char* pFileName, s32 lineNo ) throw();
void operator delete[] ( void* p, gfl2::heap::HeapBase* pHeapBase, const char* pFileName, s32 lineNo ) throw();
void operator delete ( void* p, gfl2::heap::HeapBase* pHeapBase, const b32 isLow, const char * pFileName, s32 lineNo ) throw();
void operator delete[] ( void* p, gfl2::heap::HeapBase* pHeapBase, const b32 isLow, const char * pFileName, s32 lineNo ) throw();
#else
void operator delete ( void* p, gfl2::heap::HeapBase* pHeapBase ) throw();
void operator delete[] ( void* p, gfl2::heap::HeapBase* pHeapBase ) throw();
void operator delete ( void* p, gfl2::heap::HeapBase* pHeapBase, const b32 isLow ) throw();
void operator delete[] ( void* p, gfl2::heap::HeapBase* pHeapBase, const b32 isLow ) throw();
#endif // #if GFL_DEBUG
#endif

#endif // #if !defined( GFLIB2_HEAP_GFL2_NEWANDDELETEOPERATORS_H_INCLUDED )
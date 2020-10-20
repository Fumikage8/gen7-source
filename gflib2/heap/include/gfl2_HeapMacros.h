/**
* @file    gfl2_HeapMacros.h
* @brief   メモリ確保/開放用マクロ定義
* @author  ikuta_junya
* @date    2014.09.15
*/

#if !defined( GFLIB2_HEAP_GFL2_HEAPMACROS_H_INCLUDED )
#define GFLIB2_HEAP_GFL2_HEAPMACROS_H_INCLUDED 
#pragma once

#include <heap/include/gfl2_NewAndDeleteOperators.h>

//
// メモリ確保/開放用マクロ定義
// @note ヒープからメモリ確保/開放を行う際は必ずこのマクロを使用してください、HeapBaseの機能を直接使うとコンストラクタ/デストラクタが実行されません。
//

// メモリ確保用マクロ
#if GFL_DEBUG
#define GFL_NEW( heap )  new( heap, __FILE__, __LINE__ )
#define GFL_NEW_ARRAY( heap )  new( heap, __FILE__, __LINE__ )
#define GFL_NEW_LOW( heap )  new( heap, true, __FILE__, __LINE__ )
#define GFL_NEW_LOW_ARRAY( heap )  new( heap, true, __FILE__, __LINE__ )
#else
#define GFL_NEW( heap )  new( heap )
#define GFL_NEW_ARRAY( heap )  new( heap )
#define GFL_NEW_LOW( heap )  new( heap, true )
#define GFL_NEW_LOW_ARRAY( heap )  new( heap, true )
#endif // if GFL_DEBUG

// メモリ開放用マクロ
#define GFL_DELETE delete
#define GFL_DELETE_ARRAY delete[]
#define GFL_SAFE_DELETE( p )       { if( p ) { GFL_DELETE( p );		( p ) = 0; } }
#define GFL_SAFE_DELETE_ARRAY( p ) { if( p ) { GFL_DELETE_ARRAY( p );		( p ) = 0; } }

// placement new, 配置new, 配置構文new  @note 使用許可制
#define GFL_NEW_PLACEMENT new


#endif // #if !defined( GFLIB2_HEAP_GFL2_HEAPMACROS_H_INCLUDED )
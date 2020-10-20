//=============================================================================
/**
* @brief  動的ヘッダシステムを利用するためにインクルードするヘッダ
* @file   gfl2_DebugDynamicHeader.h
* @author Kanamaru Masanori
* @date   2015.2.20
*/
//=============================================================================

#ifndef __GFL_DEBUG_DYNAMICHEADER_H__
#define __GFL_DEBUG_DYNAMICHEADER_H__
#pragma once


#include <debug/include/DynamicHeader/internal/gfl2_DebugHeaderHashManager.h>

/**
* ユーザが使用するマクロ
*/


/**
* @def
* @brief	動的ヘッダシステム利用で利用するファイルをincludeする際のマクロ
*/
#define GFL_DYNAMIC_HEADER(path) path

#if GFL_ENABLE_DYNAMIC_HEADER

/**
* @def
* @brief	動的ヘッダシステムの初期化
*
* @param pHeap           使用するヒープへのポインタ
* @param capacity        最大同時読み込みヘッダファイル数
* @param heapAlertLimit  ヒープ使用量警告の閾値
* @param rootPath        ロードされるヘッダパスのルート
*/
#define GFL_DYNAMIC_HEADER_MANAGER_INITIALIZE(pHeap, capacity, heapAlertLimit, rootPath) gfl2::debug::DynamicHeader::HeaderHashManager::Initialize(pHeap, capacity, heapAlertLimit, rootPath)

/**
* @def
* @brief	動的ヘッダシステムの終了処理
*/
#define GFL_DYNAMIC_HEADER_MANAGER_FINALIZE() gfl2::debug::DynamicHeader::HeaderHashManager::Finalize()

/**
* @def
* @brief	ヘッダファイルを読み込む
*			読み込み済みのファイルを指定した場合は参照カウントのみ進めます
*
* @param	filePath	読み込むファイルパス(const char*)
* @param	capacity	ファイルから読み出す定義数 (実際に読み出した数がこの値を超えるとアサートを出します)
*/
#define GFL_LOAD_HEADER_HASH(filePath, capacity) gfl2::debug::DynamicHeader::HeaderHashManager::LoadHeaderHash(filePath, capacity)

/**
* @def
* @brief	読み込んだヘッダファイルをアンロードする
*			参照カウントが残っている場合はアンロードしません
*
* @param	filePath	対象ファイルパス(const char*)
*/
#define GFL_UNLOAD_HEADER_HASH(filePath) gfl2::debug::DynamicHeader::HeaderHashManager::UnLoadHeaderHash(filePath)

/**
* @def
* @brief	定義値を参照する
*
* @param	keyword		定数名(const char*)
*/
#define GFL_VALUE_DEFINE_IN_HEADER(keyword) gfl2::debug::DynamicHeader::HeaderHashManager::GetValue(#keyword, keyword)

/**
* @def
* @brief	定義値を参照する(ヘッダファイル指定版)
*
* @param	keyword		定数名(const char*)
* @param	filePath	対象ファイルパス
*/
#define GFL_VALUE_DEFINE_IN_HEADER_FAST(keyword, filePath) gfl2::debug::DynamicHeader::HeaderHashManager::GetValue(#keyword, keyword, filePath)

#define GFL_DYNAMIC_HEADER_DUMP() gfl2::debug::DynamicHeader::HeaderHashManager::Dump()


#else

#define GFL_DYNAMIC_HEADER_MANAGER_INITIALIZE(pHeap, capacity, heapAlertLimit, rootPath) 
#define GFL_DYNAMIC_HEADER_MANAGER_FINALIZE() 

#define GFL_LOAD_HEADER_HASH(filePath, capacity) 
#define GFL_UNLOAD_HEADER_HASH(filePath) 
#define GFL_VALUE_DEFINE_IN_HEADER(keyword) keyword
#define GFL_VALUE_DEFINE_IN_HEADER_FAST(keyword, filePath) keyword
#define GFL_DYNAMIC_HEADER_DUMP() 

#endif

#endif

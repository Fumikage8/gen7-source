//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldDebugInstanceMemoryCollection.h
 *	@brief  インスタンスのメモリ使用量収集処理
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_DEBUG_INSTANCE_MEMORY_COLLECTION_H__ )
#define __FIELD_DEBUG_INSTANCE_MEMORY_COLLECTION_H__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <heap/include/gfl2_heap_manager.h>

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(FieldTest)


#if PM_DEBUG

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

/**
 *  @class インスタンスのメモリ使用量を集計するクラス
 */
class DebugInstanceMemoryCollection
{
public:
  enum
  {
    COLLECTION_INDEX_MAX = 512,  // インデックス最大数
  };
  
  struct Data
  {
    char  name[128];        ///< レコード名
    u32 system_heap;          ///< システムヒープ使用量
    u32 device_heap;          ///< デバイスヒープ使用量
    u32 field_heap;           ///< フィールドヒープ使用量
    u32 field_device_heap;    ///< フィールドデバイスヒープ使用量
    u32 app_heap;           ///< フィールドヒープ使用量
    u32 app_device_heap;    ///< フィールドデバイスヒープ使用量


    u32 s_system_heap_size;         ///< システムヒープ使用量
    u32 s_device_heap_size;         ///< デバイスヒープ使用量
    u32 s_field_heap_size;          ///< フィールドヒープ使用量
    u32 s_field_device_heap_size;   ///< フィールドデバイスヒープ使用量
    u32 s_app_heap_size;            ///< フィールドヒープ使用量
    u32 s_app_device_heap_size;     ///< フィールドデバイスヒープ使用量
  };

  // 初期化
  static void Initialize();

  // 監視情報を登録
  static u32 AddCollectionData(  const char* name );

  // 計測開始
  static void StartCollection( u32 index );
  // 計測終了
  static void EndCollection( u32 index );

  // ダンプ表示する
  static void DumpAll();
  
private:

  // 計測結果情報
  static Data m_DataArray[ COLLECTION_INDEX_MAX ];
  static int m_UseNum;

  static gfl2::heap::HeapBase * m_pSystemHeap;
  static gfl2::heap::HeapBase * m_pDeviceHeap;
  static gfl2::heap::HeapBase * m_pAppHeap;
  static gfl2::heap::HeapBase * m_pAppDeviceHeap;
  static gfl2::heap::HeapBase * m_pFieldHeap;
  static gfl2::heap::HeapBase * m_pFieldDeviceHeap;
};

#define FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_Initialize() Test::FieldTest::DebugInstanceMemoryCollection::Initialize()
#define FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData(name) Test::FieldTest::DebugInstanceMemoryCollection::AddCollectionData( (name) )
#define FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(index) Test::FieldTest::DebugInstanceMemoryCollection::StartCollection( (index) )
#define FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(index) Test::FieldTest::DebugInstanceMemoryCollection::EndCollection( (index) )
#define FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_DumpAll() Test::FieldTest::DebugInstanceMemoryCollection::DumpAll()

#else // PM_DEBUG

#define FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_Initialize() ((void)0)
#define FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData(index,name) ((void)0)
#define FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(index) ((void)0)
#define FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(index) ((void)0)
#define FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_DumpAll() ((void)0)

#endif // PM_DEBUG

GFL_NAMESPACE_END(FieldTest)
GFL_NAMESPACE_END(Test)

#endif	// __FIELD_DEBUG_INSTANCE_MEMORY_COLLECTION_H__



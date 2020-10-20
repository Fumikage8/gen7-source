//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldDebugInstanceMemoryCollection.cpp
 *	@brief  インスタンスのメモリ使用量収集処理
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "test/FieldTest/include/debug/FieldDebugInstanceMemoryCollection.h"
#include "System/include/HeapDefine.h"

#include "test/FieldTest/include/FieldTestUtility.h"


#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(FieldTest)


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------

DebugInstanceMemoryCollection::Data DebugInstanceMemoryCollection::m_DataArray[ DebugInstanceMemoryCollection::COLLECTION_INDEX_MAX ];


gfl2::heap::HeapBase * DebugInstanceMemoryCollection::m_pSystemHeap = NULL;
gfl2::heap::HeapBase * DebugInstanceMemoryCollection::m_pDeviceHeap = NULL;
gfl2::heap::HeapBase * DebugInstanceMemoryCollection::m_pAppHeap = NULL;
gfl2::heap::HeapBase * DebugInstanceMemoryCollection::m_pAppDeviceHeap = NULL;
gfl2::heap::HeapBase * DebugInstanceMemoryCollection::m_pFieldHeap = NULL;
gfl2::heap::HeapBase * DebugInstanceMemoryCollection::m_pFieldDeviceHeap = NULL;
int DebugInstanceMemoryCollection::m_UseNum = 0;

/**
 *  @brief 初期化
 */
void DebugInstanceMemoryCollection::Initialize()
{
  m_pSystemHeap = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );
  m_pDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEVICE );

  m_pAppHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP );
  m_pAppDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  m_pFieldHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FIELD );
  m_pFieldDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FIELD_DEVICE );


  for( int i=0; i<DebugInstanceMemoryCollection::COLLECTION_INDEX_MAX; ++i )
  {
    memset( m_DataArray[i].name, 0, 128 );
    m_DataArray[i].system_heap = 0;
    m_DataArray[i].device_heap = 0;
    m_DataArray[i].field_heap = 0;
    m_DataArray[i].field_device_heap = 0;
    m_DataArray[i].app_heap = 0;
    m_DataArray[i].app_device_heap = 0;

    m_DataArray[i].s_system_heap_size = 0;         ///< システムヒープ使用量
    m_DataArray[i].s_device_heap_size = 0;         ///< デバイスヒープ使用量
    m_DataArray[i].s_field_heap_size = 0;          ///< フィールドヒープ使用量
    m_DataArray[i].s_field_device_heap_size = 0;   ///< フィールドデバイスヒープ使用量
    m_DataArray[i].s_app_heap_size = 0;            ///< フィールドヒープ使用量
    m_DataArray[i].s_app_device_heap_size = 0;     ///< フィールドデバイスヒープ使用量

  }

  m_UseNum = 0;
}

/**
 *  @brief 収集レコード情報の追加
 */
u32 DebugInstanceMemoryCollection::AddCollectionData( const char* name )
{
  u32 index = m_UseNum;

  if( index < DebugInstanceMemoryCollection::COLLECTION_INDEX_MAX )
  {
    GFL_ASSERT( Test::FieldTest::Utility::Strnlen( m_DataArray[index].name, 128 ) == 0 );
    Test::FieldTest::Utility::Strcpy_s( m_DataArray[index].name, name, 128 );
    m_UseNum ++;
  }
  else
  {
    GFL_ASSERT(0);
  }

  return index;
}

/**
 *  @brief 計測結果の収集開始
 */
void DebugInstanceMemoryCollection::StartCollection( u32 index )
{
  if( index >= DebugInstanceMemoryCollection::COLLECTION_INDEX_MAX )
  {
    return ;
  }
  if( Test::FieldTest::Utility::Strnlen( m_DataArray[index].name, 128 ) == 0 )
  {
    return ;
  }

  m_DataArray[index].s_system_heap_size = m_pSystemHeap->GetTotalFreeSize();
  m_DataArray[index].s_device_heap_size = m_pDeviceHeap->GetTotalFreeSize();
  
  m_DataArray[index].s_app_heap_size = m_pAppHeap->GetTotalFreeSize();
  m_DataArray[index].s_app_device_heap_size = m_pAppDeviceHeap->GetTotalFreeSize();

  m_DataArray[index].s_field_heap_size = m_pFieldHeap->GetTotalFreeSize();
  m_DataArray[index].s_field_device_heap_size = m_pFieldDeviceHeap->GetTotalFreeSize();
}

/**
 *  @brief 計測結果の収集終了
 */
void DebugInstanceMemoryCollection::EndCollection( u32 index )
{
  if( index >= DebugInstanceMemoryCollection::COLLECTION_INDEX_MAX )
  {
    return ;
  }
  if( Test::FieldTest::Utility::Strnlen( m_DataArray[index].name, 128 ) == 0 )
  {
    return ;
  }

  m_DataArray[index].system_heap += m_DataArray[index].s_system_heap_size - m_pSystemHeap->GetTotalFreeSize();
  m_DataArray[index].device_heap += m_DataArray[index].s_device_heap_size - m_pDeviceHeap->GetTotalFreeSize();
  
  m_DataArray[index].app_heap         += m_DataArray[index].s_app_heap_size - m_pAppHeap->GetTotalFreeSize();
  m_DataArray[index].app_device_heap  += m_DataArray[index].s_app_device_heap_size - m_pAppDeviceHeap->GetTotalFreeSize();

  m_DataArray[index].field_heap         += m_DataArray[index].s_field_heap_size - m_pFieldHeap->GetTotalFreeSize();
  m_DataArray[index].field_device_heap  += m_DataArray[index].s_field_device_heap_size - m_pFieldDeviceHeap->GetTotalFreeSize();
}

// ダンプ表示する
void DebugInstanceMemoryCollection::DumpAll()
{
#if 0
  GFL_RELEASE_PRINT( "===================================================\n" );
  GFL_RELEASE_PRINT( "DebugInstanceMemoryCollection\n" );
  for( int i=0; i<DebugInstanceMemoryCollection::COLLECTION_INDEX_MAX; ++i )
  {
    if( Test::FieldTest::Utility::Strnlen( m_DataArray[i].name, 128 ) == 0 )
    {
      continue;
    }

    GFL_RELEASE_PRINT( "InstanceName %s\n", m_DataArray[i].name );
    GFL_RELEASE_PRINT( "\tsysTotal         0x%x\n", m_DataArray[i].system_heap + m_DataArray[i].app_heap + m_DataArray[i].field_heap );
    GFL_RELEASE_PRINT( "\tdevTotal         0x%x\n", m_DataArray[i].device_heap + m_DataArray[i].app_device_heap + m_DataArray[i].field_device_heap );
    GFL_RELEASE_PRINT( "\tsysHeap         0x%x\n", m_DataArray[i].system_heap );
    GFL_RELEASE_PRINT( "\tdevHeap         0x%x\n", m_DataArray[i].device_heap );
    GFL_RELEASE_PRINT( "\tappHeap         0x%x\n", m_DataArray[i].app_heap );
    GFL_RELEASE_PRINT( "\tappDeviceHeap   0x%x\n", m_DataArray[i].app_device_heap );
    GFL_RELEASE_PRINT( "\tfieldHeap       0x%x\n", m_DataArray[i].field_heap );
    GFL_RELEASE_PRINT( "\tfieldDeviceHeap 0x%x\n", m_DataArray[i].field_device_heap );
  }
  GFL_RELEASE_PRINT( "===================================================\n" );
#elif 0
  GFL_RELEASE_PRINT( "===================================================\n" );
  GFL_RELEASE_PRINT( "DebugInstanceMemoryCollection\n" );
  for( int i=0; i<DebugInstanceMemoryCollection::COLLECTION_INDEX_MAX; ++i )
  {
    if( Test::FieldTest::Utility::Strnlen( m_DataArray[i].name, 128 ) == 0 )
    {
      continue;
    }

    GFL_RELEASE_PRINT( "InstanceName=\"%s\" sys=\"0x%x\" dev=\"0x%x\"\n", m_DataArray[i].name, m_DataArray[i].system_heap + m_DataArray[i].app_heap + m_DataArray[i].field_heap, m_DataArray[i].device_heap + m_DataArray[i].app_device_heap + m_DataArray[i].field_device_heap );
  }
  GFL_RELEASE_PRINT( "===================================================\n" );
#else
  GFL_RELEASE_PRINT( "===================================================\n" );
  GFL_RELEASE_PRINT( "DebugInstanceMemoryCollection\n" );
  for( int i=0; i<DebugInstanceMemoryCollection::COLLECTION_INDEX_MAX; ++i )
  {
    if( Test::FieldTest::Utility::Strnlen( m_DataArray[i].name, 128 ) == 0 )
    {
      continue;
    }

    u32 size = m_DataArray[i].device_heap + m_DataArray[i].app_device_heap + m_DataArray[i].field_device_heap;
    GFL_RELEASE_PRINT( "%s,0x%x,%d,%f\n", m_DataArray[i].name, size, size, size / 1024.0f );
  }
  GFL_RELEASE_PRINT( "===================================================\n" );
#endif
}



GFL_NAMESPACE_END(FieldTest)
GFL_NAMESPACE_END(Test)
 
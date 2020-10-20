/**
* @file    gfl2_NewAndDeleteOperators.cpp
* @brief   new/delete演算子のOverLoadとOverRideの実装を記述する
* @author  ikuta_junya
* @date    2014.09.15
*/

#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

// @attention 隠蔽したい
//現状基底クラスはGflHeapBaseで外に公開しているのはHeapBase。
//で、HeapBaseは各プラットフォーム別に継承したものがtypedefで使われている。
//なぜHeapBaseを基底として使っていないかは不明。問題も起きていないし、修正が大きいのでnijiでは調査、対応を見送る
#if defined(GF_PLATFORM_CTR)
#include <heap/include/gfl2_CtrExpHeap.h>        
#elif defined(GF_PLATFORM_WIN32)
#include <heap/include/gfl2_WinHeap.h>        
#endif  //プラットフォーム分岐
#include <heap/include/gfl2_heap_manager.h>

#include <heap/source/gfl2_heap_local.h>       // for HEAP_PRINT
#include <heap/include/gfl2_heap_base.h>
#include <heap/include/gfl2_NewAndDeleteOperators.h> 

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( heap )

// @note ↓を有効化する場合はスタティック/グローバル変数のコンストラクタ、デストラクタに注意する(例 std::stringをグローバル変数で定義した場合、デストラクタ(delete)がヒープシステム破棄後に実行される
// #define GFL_DEFALT_NEW_OVER_LOAD_ENABLE        // この定義が有効な場合は通常newをヒープシステムでオーバーロードする

#if GFL_DEBUG
GFL_NAMESPACE_BEGIN( debug )
static b32 g_DebugDefaultNewAssertEnabled = false;  ///< trueなら通常newが際にアサートが発生する

/**
* @brief   通常new禁止アサートを有効化する
* @note Win32版では機能しません
*
* @return  無し 
*/
void EnableDefaultNewAssert( void )
{
  g_DebugDefaultNewAssertEnabled = true;
}

/**
* @brief   通常new禁止アサートを無効化する
* @note Win32版では機能しません
*
* @return  無し 
*/
void DisableDefaultNewAssert( void )
{
  g_DebugDefaultNewAssertEnabled = false;
}
  
/**
* @brief   通常new禁止アサートが有効か確認する
* @note Win32版では機能しません
*
* @return  trueなら使用可能、falseなら使用禁止
*/
b32 IsDefaultNewAssertEnabled( void )
{
  return g_DebugDefaultNewAssertEnabled;
}

GFL_NAMESPACE_END( debug )
#endif // GFL_DEBUG

GFL_NAMESPACE_END( heap )
GFL_NAMESPACE_END( gfl2 )

// @note kujira世代では通常new禁止をABIオーバーライドで行っており、通常newのオーバーロードを行っていなかった。
//       windows環境ではヒープシステム初期化の為にmallocを使用するため、通常newをオーバーロードして実装している。
// @attencion 上記実装はターゲットが決まり次第、再考する(CTRではなくなった場合の対応が必要)
#ifdef GF_PLATFORM_CTR

//
namespace {
extern "C" {
 
  // void* __cdecl malloc(_In_ size_t size)
  void* malloc( size_t size )
  {
    gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetDefaultNewUseHeap();
    GFL_ASSERT_STOP( pHeap );
    void* pMemory = GflHeapAllocMemory( pHeap, size );
      
#if GFL_DEBUG
    if( gfl2::heap::debug::g_DebugDefaultNewAssertEnabled )
    {
      GFL_ASSERT_MSG( 0, "通常new禁止\n" );
    }
    else
    {
      GFL_PRINT_EX( HEAP_PRINT1, "通常newが使用された 0x%08p 0x%-6x \n", pMemory, size );
    }
#endif        
    return pMemory;
  }

  void free( void* p )
  {
    if( p )
    {
      GFL_PRINT_EX( HEAP_PRINT1 , "free 0x%08 \n", p );
      gfl2::heap::GflHeapBase::FreeMemoryAuto( p );
    }
  }
} // extern "C"
} //namespace
#endif //GF_PLATFORM_CTR


//
// new演算子 オーバーロード/オーバーライド
//

#if GFL_DEBUG
#ifdef GFL_DEFALT_NEW_OVER_LOAD_ENABLE
// 通常new
void* operator new ( size_t size ) throw()
{
	gfl2::heap::HeapBase* pHeapBase = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );
  void *buf = pHeapBase->AllocMemory( size, "--", 0 );
  GFL_ASSERT_STOP(buf);
  return buf;
}
void* operator new[] ( size_t size ) throw()
{
	gfl2::heap::HeapBase* pHeapBase = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );
  void *buf = pHeapBase->AllocMemory( size, "--", 0 );
  GFL_ASSERT_STOP(buf);
  return buf;
}
#endif // #ifdef GFL_DEFALT_NEW_OVER_LOAD_ENABLE
// GFL_NEW(通常確保用)
void* operator new ( size_t size, gfl2::heap::HeapBase* pHeapBase, const char* pFileName, s32 lineNo ) throw()
{
  //GFL_PRINT_EX( HEAP_PRINT2, "GFL_NEW[%s][%d]\n", pFileName, lineNo );
  void *buf = pHeapBase->AllocMemory( size, pFileName, lineNo );
  GFL_ASSERT_STOP(buf);
  return buf;
}
void* operator new[] (size_t size, gfl2::heap::HeapBase* pHeapBase, const char* pFileName, s32 lineNo ) throw()
{
  //GFL_PRINT_EX( HEAP_PRINT2, "GFL_NEW[] [%s][%d]\n", pFileName, lineNo );
  void *buf = pHeapBase->AllocMemory( size, pFileName, lineNo );
  GFL_ASSERT_STOP(buf);
  return buf;
}

// GFL_NEW(後方確保用)
void* operator new ( size_t size, gfl2::heap::HeapBase* pHeapBase, const b32 isLow, const char* pFileName, s32 lineNo ) throw()
{
  GFL_UNUSED( isLow );
  //GFL_PRINT_EX( HEAP_PRINT2, "GFL_NEW_LOW[%s][%d]\n", pFileName, lineNo );
  void *buf = pHeapBase->AllocMemoryAlign( size, -pHeapBase->GetDefaultAlign(), pFileName, lineNo );
  GFL_ASSERT_STOP(buf);
  return buf;
}
void* operator new[] ( size_t size, gfl2::heap::HeapBase* pHeapBase, const b32 isLow, const char* pFileName, s32 lineNo ) throw()
{
  GFL_UNUSED( isLow );
  //GFL_PRINT_EX( HEAP_PRINT2, "GFL_NEW_LOW[] [%s][%d]\n", pFileName, lineNo );
  void *buf = pHeapBase->AllocMemoryAlign( size, -pHeapBase->GetDefaultAlign(), pFileName, lineNo );
  GFL_ASSERT_STOP(buf);
  return buf;
}

#else
#ifdef GFL_DEFALT_NEW_OVER_LOAD_ENABLE
// 通常new
void* operator new ( size_t size ) throw()
{
	gfl2::heap::HeapBase* pHeapBase = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );
  void *buf = pHeapBase->AllocMemory( size );
  GFL_ASSERT_STOP(buf);
  return buf;
}
void* operator new[] ( size_t size ) throw()
{
	gfl2::heap::HeapBase* pHeapBase = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );
  void *buf = pHeapBase->AllocMemory( size );
  GFL_ASSERT_STOP(buf);
  return buf;
}
#endif // #ifdef GFL_DEFALT_NEW_OVER_LOAD_ENABLE
// GFL_NEW(通常確保用)
void* operator new ( size_t size, gfl2::heap::HeapBase* pHeapBase ) throw()
{
  void *buf = pHeapBase->AllocMemory( size );
  GFL_ASSERT_STOP(buf);
  return buf;
}

void* operator new[] ( size_t size, gfl2::heap::HeapBase* pHeapBase ) throw()
{
  void *buf = pHeapBase->AllocMemory( size );
  GFL_ASSERT_STOP(buf);
  return buf;
}

// GFL_NEW(後方確保用)
void* operator new ( size_t size, gfl2::heap::HeapBase* pHeapBase, const b32 isLow ) throw()
{
  GFL_UNUSED( isLow );
  void *buf = pHeapBase->AllocMemoryAlign( size, -pHeapBase->GetDefaultAlign() );
  GFL_ASSERT_STOP(buf);
  return buf;
}

void* operator new[] ( size_t size, gfl2::heap::HeapBase* pHeapBase, const b32 isLow ) throw()
{
  GFL_UNUSED( isLow );
  void *buf = pHeapBase->AllocMemoryAlign( size, -pHeapBase->GetDefaultAlign() );
  GFL_ASSERT_STOP(buf);
  return buf;
}

#endif // #if GFL_DEBUG


//
// delete演算子 オーバーロード
//

#if 0
// kujira世代の実装( ABIオーバーロード )
void operator delete ( void* p ) throw()
{
  free( p );
}

void operator delete[] ( void* p ) throw()
{
  operator delete( p );
}

#else

void operator delete ( void* p ) throw()
{
#if defined(GF_PLATFORM_CTR)

  gfl2::heap::GflHeapBase::FreeMemoryAuto( p );

#elif defined(GF_PLATFORM_WIN32)

  // 各ヒープの確保範囲チェックで管理メモリかチェックする
  // if( gfl2::heap::Manager::IsManaged( p ) )
  if( gfl2::heap::Manager::IsManagedFast( p ) )
  {
    gfl2::heap::GflHeapBase::FreeMemoryAuto( p );
  }
  else
  {
    // ここにくるのは下記2パターン
    // 1. GFL_NEW以外で確保したメモリの開放(通常new、STL内部のnewなど)
    // 2. GFL_NEWで確保したメモリをヒープシステム破棄後に開放
    // 1の場合、管理外のメモリを使っているので修正を推奨
    // 2の場合、ロジック破綻しているので必ず修正(ヒープシステム破棄時にアサートが発生するはず)

    // @note GFL_DEFALT_NEW_OVER_LOAD_ENABLEが宣言されている場合、通常newもヒープシステム管理化になる
    
    if(gfl2::heap::Manager::GetUnmanagedDeleteFunc())
    {
      GFL_PRINT("operator delete( 0x%08p ): ヒープシステム管理外メモリです、m_unmanagedDeleteFuncで開放します。通常newで確保していませんか？ \n", p );
      (gfl2::heap::Manager::GetUnmanagedDeleteFunc())(p);
    }
    else
    {
      //GFL_PRINT("operator delete( 0x%08p ): ヒープシステム管理外メモリです、free()で開放します。通常newで確保していませんか？ \n", p );
      free( p );
    }
  }

#endif // #elif defined(GF_PLATFORM_WIN32)
}

void operator delete[] ( void* p ) throw()
{
  operator delete( p );
}

#endif // if 0

//
// delete演算子 オーバーライド
// 仮引数が一致するdeleteがない場合、new対象オブジェクトのコンストラクターが例外をスローした場合にメモリの開放が行われない(warning C4291)
//
#ifdef  GF_PLATFORM_WIN32 //ただしCTRではWarningが出るためWinのみとする
#if GFL_DEBUG

// GFL_NEW(通常確保用)
void operator delete ( void* p, gfl2::heap::HeapBase* pHeapBase, const char* pFileName, s32 lineNo ) throw()
{
  gfl2::heap::GflHeapBase::FreeMemoryAuto( p );
}
void operator delete[] ( void* p, gfl2::heap::HeapBase* pHeapBase, const char* pFileName, s32 lineNo ) throw()
{
  operator delete( p );
}

// GFL_NEW(後方確保用)
void operator delete ( void* p, gfl2::heap::HeapBase* pHeapBase, const b32 isLow, const char * pFileName, s32 lineNo ) throw()
{
  gfl2::heap::GflHeapBase::FreeMemoryAuto( p );
}
void operator delete[] ( void* p, gfl2::heap::HeapBase* pHeapBase, const b32 isLow, const char * pFileName, s32 lineNo ) throw()
{
  operator delete( p );
}

#else

// GFL_NEW(通常確保用)
void operator delete ( void* p, gfl2::heap::HeapBase* pHeapBase ) throw()
{
  gfl2::heap::GflHeapBase::FreeMemoryAuto( p );
}
void operator delete[] ( void* p, gfl2::heap::HeapBase* pHeapBase ) throw()
{
  operator delete( p );
}

// GFL_NEW(後方確保用)
void operator delete ( void* p, gfl2::heap::HeapBase* pHeapBase, const b32 isLow ) throw()
{
  gfl2::heap::GflHeapBase::FreeMemoryAuto( p );
}
void operator delete[] ( void* p, gfl2::heap::HeapBase* pHeapBase, const b32 isLow ) throw()
{
  operator delete( p );
}

#endif // #if GFL_DEBUG
#endif //GF_PLATFORM_WIN32


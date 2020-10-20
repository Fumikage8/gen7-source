//=============================================================================
/**
 * @file   gfl_AutoStackManager.cpp
 * @brief  StackMemoryAutoStackManager実装クラス
 * @author suzuki_hisanao
 * @date   2012.6.18
 */
//=============================================================================
#include <base/gfl_AutoStackManager.h>

namespace gfl {
namespace base {



/***************************************************************************
* @brief コンストラクタ
*
* @param heap スタック領域として使用されるヒープエリア。<br>
*             Nexで使用することを考慮し、デバイスヒープを指定してはならない。
*             また、本メモリを他のモジュールが使用してはならない。
*
* @return none
***************************************************************************/
AutoStackManager::AutoStackManager( gfl::heap::HeapBase* heap ) : nn::os::StackMemoryAutoStackManager()
{
  m_allocator = GFL_NEW( heap ) gfl::heap::FndAllocator( heap );
  GFL_ASSERT_STOP( (m_allocator!=NULL) );

  // メモリをBottomから取得するように設定
  m_allocator->SetIsAllocLow( true );

  Initialize( m_allocator, m_stackForDestruct );
}




/***************************************************************************
* @brief デストラクタ
*
* @return none
***************************************************************************/
AutoStackManager::~AutoStackManager( void )
{
  Finalize();

  if( m_allocator != NULL )
  {
    GFL_DELETE( m_allocator );
    m_allocator = NULL;
  }
}


/***************************************************************************
 * @brief スタック用のメモリを確保する
 *
 * @param stackSize 確保すべきスタックのサイズ
 *                  サイズが4096の倍数でない場合、自動的に4096の倍数分確保される
 *
 * @return 確保したスタック用メモリの最後尾へのポインタ
 ***************************************************************************/
void* AutoStackManager::Construct( size_t stackSize )
{
  // 4096アライメント
  stackSize = nn::math::RoundUp( stackSize, 4096 );

  return nn::os::StackMemoryAutoStackManager::Construct( stackSize );
}


/***************************************************************************
* @brief スタック用のメモリを解放する
*
* @param pStackBottom 解放すべきスタック。
* @param isError エラーに伴う解放か否かを表す
*
* @return none
***************************************************************************/
void AutoStackManager::Destruct( void * pStackBottom, bool isError )
{
  nn::os::StackMemoryAutoStackManager::Destruct( pStackBottom, isError );
}




} // namespace base
} // namespace gfl

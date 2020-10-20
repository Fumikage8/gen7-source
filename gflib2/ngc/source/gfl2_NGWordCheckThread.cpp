//=============================================================================
/**
 * @file   gfl_NGWordCheckThread.cpp
 * @brief  ＮＧワードチェック用スレッド
 * @author h.suzuki
 * @date   2012.2.15
 */
//=============================================================================

#include "gfl2_NGWordCheckThread.h"
#include "../include/gfl2_NGWordChecker.h"
#include "../include/gfl2_NGWordCheckListener.h"

namespace gfl2 {
namespace ngc {

//--------------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 *
 * @param heap        スレッドのスタック領域に使用するヒープ
 * @param stack_size  スレッドのスタック領域のサイズ
 * @param listener    チェック完了通知用リスナ
 */
//--------------------------------------------------------------------------------
NGWordCheckThread::NGWordCheckThread( gfl2::heap::HeapBase* heap, u32 stack_size, NGWordCheckListener *listener )
  //: gfl::base::Thread( heap, stack_size, gfl::base::Thread::STACK_TYPE_SAFE )
{
  m_pThreadFunc = GFL_NEW(heap) ThreadFunc( this, listener );
  m_pThread = GFL_NEW(heap) gfl2::thread::Thread( m_pThreadFunc, heap, true, stack_size );
}

//--------------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//--------------------------------------------------------------------------------
NGWordCheckThread::~NGWordCheckThread()
{
  GFL_SAFE_DELETE( m_pThread );
  GFL_SAFE_DELETE( m_pThreadFunc );
}


//--------------------------------------------------------------------------------
/**
 * @brief  文章をチェックし、問題のある語句を発見した時の挙動を設定
 *
 * @param  nPatternCode    チェックを行うリージョンをビットで指定する
 * @param  pText           検査対象となるNULL終端文字列。NG箇所が*に置き換えられる
 * @param  checker         ＮＧワードチェッカー
 */
//--------------------------------------------------------------------------------
#ifdef GF_PLATFORM_CTR
void NGWordCheckThread::Setup( nn::ngc::CTR::ProfanityFilterPatternList nPatternCode, wchar_t *pText, NGWordChecker* checker )
{
  if( m_pThreadFunc ) m_pThreadFunc->Setup( nPatternCode, pText, checker );
}
#else
void NGWordCheckThread::Setup( wchar_t *pText, NGWordChecker* checker )
{
  if( m_pThreadFunc ) m_pThreadFunc->Setup( pText, checker );
}
#endif

////--------------------------------------------------------------------------------
///**
// * @brief 初期化処理
// */
////--------------------------------------------------------------------------------
//void NGWordCheckThread::Initialize( void )
//{
//}
//
//
////--------------------------------------------------------------------------------
///**
// * @brief 削除処理
// */
////--------------------------------------------------------------------------------
//void NGWordCheckThread::OnKill( void )
//{
//}
//
//
////--------------------------------------------------------------------------------
///**
// * @brief 終了処理
// */
////--------------------------------------------------------------------------------
//void NGWordCheckThread::Finalize( void )
//{
//}
//
//
////--------------------------------------------------------------------------------
///**
// * @brief スレッドメイン処理
// */
////--------------------------------------------------------------------------------
//void NGWordCheckThread::Main( void )
//{
//  if( m_checker != NULL )
//  {
//#ifdef GF_PLATFORM_CTR
//    gfl2::ngc::Result ret = m_checker->CheckFreewordWithRegulation( m_nPatternCode, m_pText );
//
//    if( m_listener ){
//      m_listener->OnNGWordCheckFinish( ret );
//    }
//#endif
//  }
//
//  this->Kill();
//}

//--------------------------------------------------------------------------------
/**
 * @brief   
 */
//--------------------------------------------------------------------------------
void NGWordCheckThread::Start( u8 threadPriority )
{
  m_pThread->Start( threadPriority );
}

//--------------------------------------------------------------------------------
/**
 * @brief   
 */
//--------------------------------------------------------------------------------
void NGWordCheckThread::Kill( void )
{
  m_pThread->Kill();
}

//--------------------------------------------------------------------------------
/**
 * @brief   
 */
//--------------------------------------------------------------------------------
bool NGWordCheckThread::IsAlive( void )
{
  return m_pThread->IsAlive();
}



//--------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//--------------------------------------------------------------------------------
NGWordCheckThread::ThreadFunc::ThreadFunc( NGWordCheckThread* pThread, NGWordCheckListener *listener )
  : m_pNGCThread( pThread )
#ifdef GF_PLATFORM_CTR
  , m_nPatternCode()
#endif
  , m_listener( listener )
  , m_pText( NULL )
  , m_checker( NULL )
{
  ;
}

//--------------------------------------------------------------------------------
/**
 * @brief   
 */
//--------------------------------------------------------------------------------
void NGWordCheckThread::ThreadFunc::Function( void )
{
  if( m_checker != NULL )
  {
#ifdef GF_PLATFORM_CTR
    gfl2::ngc::Result ret = m_checker->CheckFreewordWithRegulation( m_nPatternCode, m_pText );

    if( m_listener ){
      m_listener->OnNGWordCheckFinish( ret );
    }
#endif
  }

  m_pNGCThread->Kill();
}

//--------------------------------------------------------------------------------
/**
 * @brief   セットアップ
 */
//--------------------------------------------------------------------------------
#ifdef GF_PLATFORM_CTR
void NGWordCheckThread::ThreadFunc::Setup( nn::ngc::CTR::ProfanityFilterPatternList nPatternCode, wchar_t* pText, NGWordChecker* pChecker )
{
  m_nPatternCode = nPatternCode;
  m_pText = pText;
  m_checker = pChecker;
}
#else
void NGWordCheckThread::ThreadFunc::Setup( wchar_t* pText, NGWordChecker* pChecker )
{
  m_pText = pText;
  m_checker = pChecker;
}
#endif

} // ngc
} // gfl2



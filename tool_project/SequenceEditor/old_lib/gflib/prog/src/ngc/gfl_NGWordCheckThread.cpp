//=============================================================================
/**
 * @file   gfl_NGWordCheckThread.cpp
 * @brief  ＮＧワードチェック用スレッド
 * @author h.suzuki
 * @date   2012.2.15
 */
//=============================================================================

#include <ngc/gfl_NGWordCheckListener.h>
#include <ngc/gfl_NGWordChecker.h>
//
#include "gfl_NGWordCheckThread.h"

namespace gfl {
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
NGWordCheckThread::NGWordCheckThread( gfl::heap::HeapBase* heap, u32 stack_size, NGWordCheckListener *listener ) :
  gfl::base::Thread( heap, stack_size, gfl::base::Thread::STACK_TYPE_SAFE ),
  m_listener(listener), m_pText(NULL), m_checker(NULL)
{
}

//--------------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//--------------------------------------------------------------------------------
NGWordCheckThread::~NGWordCheckThread()
{
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
void NGWordCheckThread::Setup( nn::ngc::CTR::ProfanityFilterPatternList nPatternCode, wchar_t *pText, NGWordChecker* checker )
{
  m_nPatternCode = nPatternCode;
  m_pText        = pText;
  m_checker      = checker;
}


//--------------------------------------------------------------------------------
/**
 * @brief 初期化処理
 */
//--------------------------------------------------------------------------------
void NGWordCheckThread::Initialize( void )
{
}


//--------------------------------------------------------------------------------
/**
 * @brief 削除処理
 */
//--------------------------------------------------------------------------------
void NGWordCheckThread::OnKill( void )
{
}


//--------------------------------------------------------------------------------
/**
 * @brief 終了処理
 */
//--------------------------------------------------------------------------------
void NGWordCheckThread::Finalize( void )
{
}


//--------------------------------------------------------------------------------
/**
 * @brief スレッドメイン処理
 */
//--------------------------------------------------------------------------------
void NGWordCheckThread::Main( void )
{
  if( m_checker != NULL )
  {
    gfl::ngc::Result ret = m_checker->CheckFreewordWithRegulation( m_nPatternCode, m_pText );

    if( m_listener ){
      m_listener->OnNGWordCheckFinish( ret );
    }
  }

  this->Kill();
}


} // ngc
} // gfl



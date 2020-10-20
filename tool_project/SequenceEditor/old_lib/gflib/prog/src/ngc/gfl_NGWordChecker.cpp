//================================================================================
/**
 * @file   gfl_NGWordChecker.cpp
 * @brief  NGワードチェック用クラス
 * @author hisanao suzuki
 * @date   2012.2.15
 */
//================================================================================
#include <base/gfl_Singleton.h>
#include <ngc/gfl_NGWordChecker.h>
#include "gfl_NGWordCheckThread.h"

//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
#if GFL_ENABLE_RO
SINGLETON_INSTANCE_DEFINITION(gfl::ngc::NGWordChecker);
template class gfl::base::SingletonAccessor<gfl::ngc::NGWordChecker>;
#endif


namespace gfl {
namespace ngc {


/** ---------------------------------------------------------------------
* @brief  コンストラクタ
* @return 無し
// ---------------------------------------------------------------------*/
NGWordChecker::NGWordChecker( void )
{
  m_isOverWrite = false;
  m_isInit      = false;
  m_workMemory  = NULL;
  m_thread      = NULL;
}


/** ---------------------------------------------------------------------
* @brief  デストラクタ
* @param  無し
* @return 無し
// ---------------------------------------------------------------------*/
NGWordChecker::~NGWordChecker()
{
  Finalize();
}


/** ---------------------------------------------------------------------
* @brief  初期化処理
* @param  heap ワーク領域として使用されるヒープエリア。
*         およそ64KBが消費される。
* @return 正常終了時はtrue、以上終了はfalseを返却
// ---------------------------------------------------------------------*/
bool NGWordChecker::Initialize( gfl::heap::HeapBase *heap )
{
  nn::Result result = nn::ResultSuccess();

  if( !m_isInit )
  {
    if( m_workMemory == NULL )
    {
      m_workMemory = GflHeapAllocMemoryAlign( heap, WORKMEMORY_SIZE, 4 );
      if( m_workMemory == NULL ){
        return false;
      }
    }

    result = m_filter.Initialize( (uptr)m_workMemory );
    if( result == nn::ResultSuccess() )
    {
      m_isInit = true;
      SetMaskMode( m_isOverWrite );
    }
  }
  return result.IsSuccess();
}


/** ---------------------------------------------------------------------
* @brief  終了処理
* @param  無し
* @return 正常終了時はtrue、以上終了はfalseを返却
// ---------------------------------------------------------------------*/
bool NGWordChecker::Finalize( void )
{
  nn::Result result = nn::ResultSuccess();

  this->CancelAsync();
  this->DeleteAsync();

  if( m_isInit )
  {
    result = m_filter.Finalize();
    if( result == nn::ResultSuccess() )
    {
      m_isInit = false;
    }
    if( m_workMemory )
    {
      GflHeapFreeMemory( m_workMemory );
      m_workMemory = NULL;
    }
  }
  return result.IsSuccess();
}


/** ---------------------------------------------------------------------
* @brief  文字列に含まれる、数を表す文字の個数を取得する
* @param  pString 調べる対象のNULL終端文字列
* @return 数を表す文字の個数
// ---------------------------------------------------------------------*/
int NGWordChecker::CountNumbers( const wchar_t * pString )
{
  return nn::ngc::CTR::CountNumbers( pString );
}


/** ---------------------------------------------------------------------
* @brief  文章をチェックし、問題のある語句を発見した時の挙動を設定
* @param  bOverWrite [true]  問題語句の文字すべてをアスタリスク記号で上書き
*                    [false] 問題語句を文字数によらず1文字のアスタリスク記号に置換
* @return 無し
// ---------------------------------------------------------------------*/
void NGWordChecker::SetMaskMode( bool bOverWrite )
{
  m_isOverWrite = bOverWrite;
  m_filter.SetMaskMode( bOverWrite );
}


/** ---------------------------------------------------------------------
* @brief  単語をチェックし、問題のある語句を発見した時の挙動を設定
* @param  checkResult    NGワードが存在するならtrueを格納
* @param  nPatternCode   チェックを行うリージョンをビットで指定する
* @param  ppWords        検査対象となるNULL終端文字列の配列（文字コード：UTF16リトルエンディアン）
* @param  nWordCount     検査対象文字列の配列数(最大16個まで）
* @return 実行結果
// ---------------------------------------------------------------------*/
nn::Result NGWordChecker::CheckProfanityWords( bool* checkResult, ProfanityFilterPatternList nPatternCode, const wchar_t ** ppWords, size_t nWordCount )
{
  bit32      ngBits;
  nn::Result result;

  if( nPatternCode == ALL_REGION ){
    result = m_filter.CheckProfanityWords( &ngBits, ppWords, nWordCount );
  }
  else{
    result = m_filter.CheckProfanityWords( &ngBits, nPatternCode, ppWords, nWordCount );
  }

  if( result.IsSuccess() )
  {
    if( checkResult != NULL ){
      *checkResult = (ngBits!=0);
    }
  }

  return result;
}


/** ---------------------------------------------------------------------
* @brief  任天堂のレギュレーションと照らし合わせ、フリーワードが問題ないかチェックする(非同期関数)
*         ・CheckProfanityWords()でNGとならない
*         ・MaskProfanityWordsInText()でNGとならない
*         ・16文字以内
*         ・数字が5文字以内
*
* @attention 終了イベント通知後、必ずDeleteAsync()をコールしてください。
*
* @param  nPatternCode    チェックを行うリージョンをビットで指定する
* @param  pText           検査対象となるNULL終端文字列
* @param  listener        検査完了を通知するためのリスナ
* @param  thread_priority スレッドの実行優先度
*
* @return 検査が開始できたならtrue、失敗したならfalseを返却
// ---------------------------------------------------------------------*/
bool NGWordChecker::CheckFreewordWithRegulationASync( gfl::heap::HeapBase* heap,
                                                      ProfanityFilterPatternList nPatternCode, wchar_t *pText,
                                                      NGWordCheckListener* listener, gfl::base::Thread::Priority thread_priority )
{
  this->CancelAsync();
  this->DeleteAsync();

  m_thread = GFL_NEW_LOW( heap ) NGWordCheckThread( heap, NGWORD_STACK_SIZE, listener );

  if( m_thread == NULL ){
    return false;
  }
  m_thread->Setup( nPatternCode, pText, this );
  m_thread->Start( thread_priority );
  
  return true;
}


/** ---------------------------------------------------------------------
* @brief  文章をチェックし、問題のある語句を発見した時の挙動を設定
* @param  pProfanityWordCount 文章中に不正文字列が何回現れたかを取得するためのポインタ
* @param  nPatternCode        チェックを行うリージョンをビットで指定する
* @param  pText               検査対象となるNULL終端文字列。NG箇所が*に置き換えられる
* @return 実行結果
// ---------------------------------------------------------------------*/
nn::Result NGWordChecker::MaskProfanityWordsInText( int* pProfanityWordCount, ProfanityFilterPatternList nPatternCode, wchar_t *pText )
{
  if( nPatternCode == ALL_REGION ){
    return m_filter.MaskProfanityWordsInText( pProfanityWordCount, pText );
  }
  else{
    return m_filter.MaskProfanityWordsInText( pProfanityWordCount, nPatternCode, pText );
  }
}


/** ---------------------------------------------------------------------
* @brief  任天堂のレギュレーションと照らし合わせ、フリーワードが問題ないかチェックする
*         ・CheckProfanityWords()でNGとならない
*         ・MaskProfanityWordsInText()でNGとならない
*         ・１６文字以内
*         ・数字が5文字以内
*
* @param  nPatternCode    チェックを行うリージョンをビットで指定する
* @param  pText           検査対象となるNULL終端文字列
* @return 検査結果
// ---------------------------------------------------------------------*/
gfl::ngc::Result NGWordChecker::CheckFreewordWithRegulation( ProfanityFilterPatternList nPatternCode, wchar_t *pText )
{
  if( pText == NULL ){
    return E_SUCCESS;
  }
  // 16文字オーバー
  if( ::std::wcslen( pText ) > ARROW_FREEWORD_CHAR_COUNT ){
    return E_ERROR_FREEWORD_CHAR_COUNT;
  }
  // 数字文字数オーバー
  if( CountNumbers(pText) > ARROW_NUMBER_COUNT ){
    return E_ERROR_NUMBER_COUNT;
  }
  ::std::wcscpy( m_tempArea, pText );

  // CheckProfanityWords()
  bool isError = false;
  const wchar_t* ppWords[1] = { m_tempArea };
  CheckProfanityWords( &isError, nPatternCode, ppWords, 1 );
  if( isError ){
    return E_ERROR_WORD;
  }

  // MaskProfanityWordsInText()
  int profanityWordCount = 0;
  MaskProfanityWordsInText( &profanityWordCount, nPatternCode, m_tempArea );
  if( profanityWordCount > 0 ){
    return E_ERROR_WORD;
  }

  return E_SUCCESS;
}


//-------------------------------------------------------------------------------
/**
 * @brief 非同期通信が実行中かどうかを調べる
 * @retval true   実行中
 * @retval false  実行していない
 */
//-------------------------------------------------------------------------------
bool NGWordChecker::CheckAsync( void ) const
{
  return ( m_thread && m_thread->IsAlive() );
}


//-------------------------------------------------------------------------------
/**
 * @brief 実行中の非同期通信をキャンセルする
 *
 * @note
 * CheckAsync()で非同期処理の終了を確認した後,
 * EndAsync()を呼んでください。
 */
//-------------------------------------------------------------------------------
void NGWordChecker::CancelAsync( void )
{
  if( this->CheckAsync() ) {
    m_thread->Kill();
  }
}


//-------------------------------------------------------------------------------
/**
 * @brief スレッドの終了待ちを行います。
 */
//-------------------------------------------------------------------------------
void NGWordChecker::WaitAsyncFinished( void )
{
  while( this->CheckAsync() ) {
    nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
  }
}


//-------------------------------------------------------------------------------
/**
 * @brief 非同期処理スレッドを破棄する
 */
//-------------------------------------------------------------------------------
void NGWordChecker::DeleteAsync( void )
{
  this->WaitAsyncFinished();

  if( m_thread )
  {
    GFL_DELETE m_thread;
    m_thread = NULL;
  }
}


} // ngc
} // gfl



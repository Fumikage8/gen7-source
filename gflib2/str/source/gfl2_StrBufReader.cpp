//===========================================================================
/**
 * @file    gfl2_StrBufReader.cpp
 * @brief   GAMEFREAK Library  String Print System ( StrBuf Reader )
 * @author  GAMEFREAK inc.
 * @date    2010.10.27
 */
//===========================================================================

#include <str/include/gfl2_StrBufReader.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)

enum {
  READER_PRINT_FLAG = (gfl2::debug::PRINT_FLAG_CATEGORY_STRING) | (gfl2::debug::PRINT_FLAG_LEVEL1)
};


//----------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param[in]   srcBuf  読み取る元文字列
 * @param[in]   pHeap   メモリ確保用ヒープハンドル
 */
//----------------------------------------------------------------------

StrBufReader::StrBufReader( gfl2::heap::HeapBase* pHeap )
 : m_readPtr(0), m_replaceCode(EOM_CODE), m_tagProceeded(false), m_bAvailable(false)
{
  m_latestTagCode = TAG_USER_NULL;
  m_srcLength = 0;
  m_bufLength = TEMP_BUFFER_SIZE_MIN;
  m_buffer = GFL_NEW(pHeap) StrBuf( m_bufLength, pHeap );
  if( m_buffer )
  {
    m_replaceCode = *(m_buffer->GetPtr());
    m_bAvailable = true;
  }
}

//----------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//----------------------------------------------------------------------
StrBufReader::~StrBufReader()
{
  if( m_buffer )
  {
    GFL_DELETE( m_buffer );
  }
}

//----------------------------------------------------------------------
/**
 * @brief   このメソッドを呼び出すごとに１文字ずつ内容が増えた StrBuf への参照が返る
 *          タグ開始コード〜タグパラメータ部分は除外される
 *          このメソッド呼び出しによりタグ開始コードが除外されたかどうか判定したい場合、GetLatestTagCode を使う
 *
 * @retval  const StrBuf&
 */
//----------------------------------------------------------------------
const StrBuf&
StrBufReader::ReadNext( void )
{
  GFL_ASSERT(m_bAvailable);

  if( m_readPtr < m_srcLength )
  {
    if( m_replaceCode != TAG_START_CODE )
    {
      // u16 prevCode = m_replaceCode;
      m_buffer->replaceCode( m_readPtr++, m_replaceCode );
      m_replaceCode = m_buffer->replaceCode( m_readPtr, EOM_CODE );
      m_latestTagCode = TAG_USER_NULL;
      m_tagProceeded = false;
    }
    else
    {
      // タグ開始コード書き戻し
      m_buffer->replaceCode( m_readPtr, m_replaceCode );

      const STRCODE* sp = m_buffer->GetPtr() + m_readPtr;
      const STRCODE* next_sp;

      // タグパラメータを保存
      m_latestTagCode = GetTagCode( sp );
      m_latestTagParamCount = GetTagParamCount( sp );

      next_sp = SkipTagCode( sp );
      while( sp != next_sp ){
        m_buffer->replaceCode( m_readPtr, *sp );
        ++m_readPtr;
        ++sp;
      }

      m_replaceCode = m_buffer->replaceCode( m_readPtr, EOM_CODE );
      m_tagProceeded = true;

    }
  }

  return *(m_buffer);
}

//----------------------------------------------------------------------
/**
 * @brief     ReadNextで処理されたタグコードを取得
 *
 * @param[out]   dst      処理されたタグコードを受け取る
 *
 * @retval       bool     直前のReadNextでタグコードを処理していた場合、dstに有効な値を設定しtrueを返す。 それ以外 false。
 */
//----------------------------------------------------------------------
bool
StrBufReader::GetLatestTagCode( STRCODE* dst ) const
{
  if( m_tagProceeded )
  {
    *dst = m_latestTagCode;
    return true;
  }
  else
  {
    *dst = 0;
    return false;
  }
}

//----------------------------------------------------------------------
/**
 * @brief     最後まで読み出したか判定
 *
 * @retval    bool    最後まで読んでいたら true
 */
//----------------------------------------------------------------------
bool
StrBufReader::IsEnd( void ) const
{
  GFL_ASSERT(m_bAvailable);

  return (m_readPtr >= m_srcLength);
}




//----------------------------------------------------------------------
/**
 * 次に読み出す文字コードを取得
 *
 * @retval  STRCODE
 */
//----------------------------------------------------------------------
STRCODE
StrBufReader::GetNextCode( void ) const
{
  return m_replaceCode;
}

//----------------------------------------------------------------------
/**
 * @brief       読み出す文字列を切り替える
 *
 * @param[in]   srcBuf    読み出したい文字列
 * @param[in]   pHeap     内包するバッファが srcBuf を格納しきれない場合、
 *                        新たにバッファを構築し直すためのヒープ
 */
//----------------------------------------------------------------------
void
StrBufReader::SetString( const StrBuf& srcBuf, gfl2::heap::HeapBase* pHeap )
{
  GFL_ASSERT(m_bAvailable);

  if( m_bAvailable )
  {
    // 新しく渡された文字列が現在のバッファで収まりきらない場合に拡張する
    m_srcLength = srcBuf.GetLength();
    if( m_srcLength > m_bufLength )
    {
      m_bufLength = m_srcLength;
      GFL_DELETE( m_buffer );
      m_buffer = GFL_NEW(pHeap) StrBuf( m_bufLength, pHeap );
    }
    if( m_buffer != NULL )
    {
      m_readPtr = 0;
      m_buffer->SetStr( srcBuf );
      m_replaceCode = *(m_buffer->GetPtr());
    }
    else
    {
      m_bAvailable = false;
    }
  }
}

void StrBufReader::SetString( const STRCODE* str, u32 strLen, gfl2::heap::HeapBase* pHeap )
{
  GFL_ASSERT(m_bAvailable);

  if( m_bAvailable )
  {
    // 新しく渡された文字列が現在のバッファで収まりきらない場合に拡張する
    m_srcLength = strLen;
    if( m_srcLength > m_bufLength )
    {
      m_bufLength = m_srcLength;
      GFL_DELETE( m_buffer );
      m_buffer = GFL_NEW(pHeap) StrBuf( m_bufLength, pHeap );
    }
    if( m_buffer != NULL )
    {
      m_buffer->Clear();
      while( strLen-- )
      {
        m_buffer->AddCode( *str++ );
      }
      m_readPtr = 0;
      m_replaceCode = *(m_buffer->GetPtr());
    }
    else
    {
      m_bAvailable = false;
    }
  }
}

//----------------------------------------------------------------------
/**
 * @brief     読み出しポインタをリセット（次に ReadNext() を呼ぶと、最初の１文字分が返る）
 */
//----------------------------------------------------------------------
void
StrBufReader::Reset( void )
{
  GFL_ASSERT(m_bAvailable);

  if( m_bAvailable )
  {
    m_readPtr = 0;
    m_replaceCode = *(m_buffer->GetPtr());
  }
}


GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)

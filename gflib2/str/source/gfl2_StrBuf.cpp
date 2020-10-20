//===========================================================================
/**
 * @file    gfl2_StrBuf.cpp
 * @brief   GAMEFREAK Library  String Print System ( 文字列バッファクラス )
 * @author  GAMEFREAK inc.
 * @date    2010.10.27
 */
//===========================================================================

#include <str/include/gfl2_StrBuf.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)

//----------------------------------------------------------------------
// デバッグ出力フラグ
//----------------------------------------------------------------------
enum {
  STRBUF_PRINT_FLAG = (gfl2::debug::PRINT_FLAG_CATEGORY_STRING) | (gfl2::debug::PRINT_FLAG_LEVEL3)
};

//----------------------------------------------------------------------
/**
 * コンストラクタ
 */
//----------------------------------------------------------------------

// リテラル文字列を渡して構築
StrBuf::StrBuf( const STRCODE* string, gfl2::heap::HeapBase* pHeap )
  : m_bAvailable(false)
{
  GFL_ASSERT(string);

  m_strLength      = StrLen( string );
  m_numBufferElems = m_strLength + 1;
  m_buffer         = static_cast<STRCODE*> GflHeapAllocMemory( pHeap, sizeof(STRCODE) * m_numBufferElems );
  if( m_buffer )
  {
    StrNCopy( m_buffer, string, m_numBufferElems );
    m_bAvailable = true;
  }
}

// StrBuf を渡して構築（Copy）
StrBuf::StrBuf( const StrBuf& src, gfl2::heap::HeapBase* pHeap )
  : m_bAvailable(false)
{
  m_strLength      = src.m_strLength;
  m_numBufferElems = src.m_numBufferElems;
  m_buffer         = static_cast<STRCODE*> GflHeapAllocMemory( pHeap, sizeof(STRCODE) * m_numBufferElems );
  if( m_buffer )
  {
    StrNCopy( m_buffer, src.m_buffer, m_numBufferElems );
    m_bAvailable = true;
  }
}

// 格納できる文字数（終端文字含む）を指定して初期化
StrBuf::StrBuf( u32 numCharMax, gfl2::heap::HeapBase* pHeap )
  : m_bAvailable(false)
{
  GFL_ASSERT(numCharMax);

  m_strLength      = 0;
  m_numBufferElems = numCharMax;
  m_buffer         = static_cast<STRCODE*> GflHeapAllocMemory( pHeap, sizeof(STRCODE) * m_numBufferElems );
  if( m_buffer )
  {
    m_buffer[0] = EOM_CODE;
    m_bAvailable = true;
  }
}

//----------------------------------------------------------------------
/**
 * デストラクタ
 */
//----------------------------------------------------------------------
StrBuf::~StrBuf()
{
  if( m_buffer )
  {
    GflHeapFreeMemory( m_buffer );
  }
}

//==========================================================================================
//==========================================================================================

//----------------------------------------------------------------------
/**
 * @brief   文字列ポインタを取得
 *
 * @retval  const STRCODE*  格納されている文字列へのポインタ
 */
//----------------------------------------------------------------------
const STRCODE* StrBuf::GetPtr( void ) const
{
  GFL_ASSERT(m_bAvailable);
  return m_buffer;
}

//----------------------------------------------------------------------
/**
 * @brief   格納されている文字数（終端文字を含まない）を取得
 *
 * @retval  u32   文字列の長さ
 */
//----------------------------------------------------------------------
u32 StrBuf::GetLength( void ) const
{
  GFL_ASSERT(m_bAvailable);
  return m_strLength;
}

//----------------------------------------------------------------------
/**
 * @brief   格納可能な文字数（終端文字を含む）を取得
 *
 * @retval  u32   格納可能な文字数
 */
//----------------------------------------------------------------------
u32 StrBuf::GetSize( void ) const
{
  GFL_ASSERT(m_bAvailable);
  return m_numBufferElems;
}

//----------------------------------------------------------------------
/**
 * @brief   文字列を外部バッファに出力
 *
 * @param[out]  array        出力先配列
 * @param[in]   numAryElems  array の要素数
 *
 * @retval      u32    コピーした文字数（終端文字を含まない）
 */
//----------------------------------------------------------------------
u32 StrBuf::PutStr( STRCODE* array, u32 numAryElems ) const
{
  GFL_ASSERT(m_bAvailable);
  GFL_ASSERT(numAryElems);

  if( m_bAvailable && numAryElems )
  {
    return StrNCopy( array, m_buffer, numAryElems);
  }

  return 0;
}

//----------------------------------------------------------------------
/**
 * @brief   文字列をセットする（クラス使用）
 *
 * @param[in]   src      セットする文字列（クラス）
 *
 * @retval      u32      セットした文字数（終端文字を含まない）
 */
//----------------------------------------------------------------------
u32 StrBuf::SetStr( const StrBuf& src )
{
  GFL_ASSERT(m_bAvailable);

  if( m_bAvailable )
  {
    m_strLength =  StrNCopy( m_buffer, src.GetPtr(), m_numBufferElems);
    GFL_PRINT_EX(
      STRBUF_PRINT_FLAG,
      "[STRBUF] Set from Class, 1stCode=%04x, len=%d\n",
      *(src.GetPtr()), m_strLength);
    return m_strLength;
  }
  return 0;
}
//----------------------------------------------------------------------
/**
 * @brief   文字列をセットする（ null文字終端 配列使用）
 *
 * @param[in]   src      セットする文字列（配列）
 *
 * @retval      u32      セットした文字数（終端文字を含まない）
 */
//----------------------------------------------------------------------
u32 StrBuf::SetStr( const STRCODE* src )
{
  GFL_ASSERT(m_bAvailable);

  if( m_bAvailable )
  {
    m_strLength = StrNCopy( m_buffer, src, m_numBufferElems );
    return m_strLength;
  }
  return 0;
}

//----------------------------------------------------------------------
/**
 * @brief       終端に文字列追加
 *
 * @param[in]   buf   追加する文字列
 *
 * @retval      u32   連結後の文字数（終端文字を含まない）
 */
//----------------------------------------------------------------------
u32 StrBuf::AddStr( const StrBuf& buf )
{
  GFL_ASSERT(m_bAvailable);

  if( m_bAvailable )
  {
    u32  restLen = m_numBufferElems - m_strLength;
    m_strLength += StrNCopy( &m_buffer[m_strLength], buf.GetPtr(), restLen );
    return m_strLength;
  }
  return 0;
}
//----------------------------------------------------------------------
/**
 * @brief   終端に１文字追加
 *
 * @param[in]   code     追加文字列
 */
//----------------------------------------------------------------------
bool StrBuf::AddCode( STRCODE code )
{
  GFL_ASSERT(m_bAvailable);

  if( m_bAvailable )
  {
    if( (m_strLength+1) < m_numBufferElems )
    {
      m_buffer[ m_strLength++ ] = code;
      //1文字追加したら直後に終端も書き込む
      m_buffer[ m_strLength ] = EOM_CODE;
      return true;
    }
    else if( (m_strLength+1) == m_numBufferElems && code == EOM_CODE )
    {
      //バッファ末尾に書き込めるのは終端コードだけ
      m_buffer[ m_strLength++ ] = EOM_CODE;
      return true;
    }
    else
    {
      GFL_ASSERT_MSG(0, "bufSize=%d, strLen=%d", m_numBufferElems, m_strLength);
    }
  }
  return false;
}

//----------------------------------------------------------------------
/**
 *  @brief  AddCodeで文字列を挿入された後、内部の文字列長を計測しなおす
 */
//----------------------------------------------------------------------
void StrBuf::AddCodeFinish( void )
{
  if( m_bAvailable )
  {
    m_strLength = StrLen( m_buffer );
  }
}


//----------------------------------------------------------------------
/**
 * @brief   空文字列をセットする
 */
//----------------------------------------------------------------------
void StrBuf::Clear( void )
{
  GFL_ASSERT(m_bAvailable);

  if( m_bAvailable )
  {
    m_buffer[0] = EOM_CODE;
    m_strLength = 0;
  }
}

//----------------------------------------------------------------------
/**
 * @brief   文字列の比較
 *
 * @param[in]   buf      比較文字列
 *
 * @retval      bool     比較文字列と内容が一致していれば true
 */
//----------------------------------------------------------------------
bool StrBuf::Compare( const StrBuf& buf ) const
{
  GFL_ASSERT(m_bAvailable);

  if( m_bAvailable )
  {
    return StrComp( m_buffer, buf.m_buffer );
  }
  return false;
}

//================================================================================
// private methods
//================================================================================

//----------------------------------------------------------------------
/**
 * @brief   指定位置の文字コードを取得
 *
 * @param[in]   index     取得する文字列内位置（0 origin）
 *
 * @retval      STRCODE   置換前の文字コード
 */
//----------------------------------------------------------------------
STRCODE StrBuf::getCode( u32 index ) const
{
  GFL_ASSERT(m_bAvailable);

  if( index < m_numBufferElems )
  {
    return m_buffer[ index ];
  }
  else
  {
    GFL_ASSERT( 0 );
    return EOM_CODE;
  }
}

//----------------------------------------------------------------------
/**
 * @brief   指定位置の文字コードを置換
 *
 * @param[in]   index     置換する文字列内位置（0 origin）
 * @param[in]   code      置換後文字コード
 *
 * @retval      STRCODE   置換前の文字コード
 */
//----------------------------------------------------------------------
STRCODE StrBuf::replaceCode( u32 index, STRCODE code )
{
  GFL_ASSERT(m_bAvailable);

  STRCODE oldCode = EOM_CODE;

  if( index < m_numBufferElems )
  {
    oldCode = m_buffer[ index ];
    m_buffer[ index ] = code;
    m_strLength = StrLen( m_buffer );
  }
  else
  {
    GFL_ASSERT( 0 );
  }

  return oldCode;
}
//----------------------------------------------------------------------
/**
 * @brief   指定位置から文字列を上書きコピーする
 *
 * @param[in]   index     上書き開始する文字列内位置（0 origin）
 * @param[in]   src       上書きする文字列
 */
  //----------------------------------------------------------------------
void StrBuf::replaceStr( u32 index, const STRCODE* src )
{
  GFL_ASSERT(m_bAvailable);

  if( index < m_numBufferElems )
  {
    unsigned int buf_len = m_numBufferElems - index;
    StrNCopy( &m_buffer[index], src, buf_len );
    m_strLength = StrLen( m_buffer );
  }
}


GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)

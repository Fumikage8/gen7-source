// ============================================================================
/*
 * @file gfl2_String.h
 * @brief 文字列クラスです。
 * @date 2015.02.20
 */
// ============================================================================
#if !defined( GFL2_STRING_H_INCLUDED )
#define GFL2_STRING_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_DebugPrint.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(util)


class String
{
private:

  static const u32 HASH_MAGIC_NUMBER = 0x83;

  // --------------------------------------------------------------------------
  /**
   * @brief  コピーコンストラクタです。
   *         Heapを渡してメモリを確保できないので禁止としてます。
   */
  // --------------------------------------------------------------------------
  String( const String& string )
  {
  }

public:
  // --------------------------------------------------------------------------
  /**
   * @brief  デフォルトコンストラクタです。
   *         CreateBuffer で内部のバッファを確保してから使用する形となります。
   */
  // --------------------------------------------------------------------------
  String() :
    m_pBuffer( NULL ),
    m_BufferSize( 0 )
  {
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  ヒープとバッファサイズを指定したバージョンのコンストラクタです。
   * @param  pHeap 内部のバッファを確保する為のヒープです。
   * @param  bufferSize バッファサイズです。
   */
  // --------------------------------------------------------------------------
  String( gfl2::heap::HeapBase* pHeap, u32 bufferSize )
  {
    CreateBuffer( pHeap, bufferSize );
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  ヒープとテキストを指定したバージョンのコンストラクタです。
   *         テキストの長さを取得して内部のバッファを確保します。
   * @param  pHeap 内部のバッファを確保する為のヒープです。
   * @param  bufferSize バッファサイズです。
   */
  // --------------------------------------------------------------------------
  String( gfl2::heap::HeapBase* pHeap, const char* pText )
  {
    CreateBuffer( pHeap, strlen( pText ) );
    *this = pText;
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  デストラクタです。
   *         内部のバッファをクリアします。
   */
  // --------------------------------------------------------------------------
  ~String()
  {
    Clear();
  }
  
  // --------------------------------------------------------------------------
  /**
   * @brief  内部のバッファを確保します。
   * @param  pHeap 内部のバッファを確保する為のヒープです。
   * @param  bufferSize バッファサイズです。
   */
  // --------------------------------------------------------------------------
  void CreateBuffer( gfl2::heap::HeapBase* pHeap, u32 bufferSize )
  {
    m_pBuffer = GFL_NEW_ARRAY( pHeap ) char[ bufferSize ];
    m_BufferSize = bufferSize;
    ::std::memset( m_pBuffer, 0, m_BufferSize );
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  文字列の長さを取得します。
   * @return 文字列の長さ
   */
  // --------------------------------------------------------------------------
  u32 Length() const
  {
    if( m_pBuffer )
    {
      return strlen( m_pBuffer );
    }
    return 0;
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  内部のバッファをクリアします。
   */
  // --------------------------------------------------------------------------
  void Clear()
  {
    GFL_SAFE_DELETE_ARRAY( m_pBuffer );
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  String の代入です。
   * @param  string 代入する文字列です。最後にNULL文字が入ってる必要があります。
   * @return 代入後の文字列を返します
   */
  // --------------------------------------------------------------------------
  String& operator = ( const String& string )
  {
    safeStrCopy( string.c_str() );
    return *this;
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  const char* の代入です。
   * @param  pText 代入する文字列です。最後にNULL文字が入ってる必要があります。
   * @return 代入後の文字列を返します
   */
  // --------------------------------------------------------------------------
  String& operator = ( const char* pText )
  {
    safeStrCopy( pText );
    return *this;
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  String の連結を行います。
   * @param  string 連結する文字列です。最後にNULL文字が入ってる必要があります。
   * @return 連結後の文字列を返します
   */
  // --------------------------------------------------------------------------
  String& operator += ( const String& string )
  {
    safeStrCat( string.c_str() );
    return *this;
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  const char* の連結を行います。
   * @param  pText 連結する文字列です。最後にNULL文字が入ってる必要があります。
   * @return 連結後の文字列を返します
   */
  // --------------------------------------------------------------------------
  String& operator += ( const char* pText )
  {
    safeStrCat( pText );
    return *this;
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  文字列のポインタを取得します。
   * @return 文字列のポインタ
   */
  // --------------------------------------------------------------------------
  const char* c_str() const
  {
    return m_pBuffer;
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  ハッシュ値を計算します
   * @return ハッシュ値
   */
  // --------------------------------------------------------------------------
  u32 CalcHash()
  {
    return String::CalcHash( m_pBuffer );
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  ハッシュ値を計算します
   * @param  str ハッシュ値を計算したい文字列
   * @return ハッシュ値
   */
  // --------------------------------------------------------------------------
  static u32 CalcHash( const char* str )
  {
    u32 value = 0;
		while(*str) {
			value *= HASH_MAGIC_NUMBER;
			value ^= static_cast<u32> (*str++);
		}
		return value;
  }

#if GFL_DEBUG
  u32 GetBufferSize() const
  {
    return m_BufferSize;
  }
#endif// GFL_DEBUG

private:
  
  void safeStrCopy( const char* pSource )
  {
    u32 copyTextSize = strlen( pSource );
    if( m_BufferSize > copyTextSize )
    {
      ::std::memset( m_pBuffer, 0, m_BufferSize );
      ::std::memcpy( m_pBuffer, pSource, copyTextSize );
    }
    else
    {
      GFL_WARNING( "Warning : safeStrCopy failed. m_BufferSize = %d, copyTextSize = %d\n", m_BufferSize, copyTextSize );
    }
  }

  void safeStrCat( const char* pSource )
  {
    u32 bufferTextSize = strlen( m_pBuffer );
    u32 copyTextSize = strlen( pSource );
    if( m_BufferSize > bufferTextSize + copyTextSize )
    {
      ::std::memcpy( &m_pBuffer[bufferTextSize], pSource, copyTextSize );
    }
    else
    {
      GFL_WARNING( "Warning : safeStrCat failed. m_BufferSize = %d, bufferTextSize = %d, TextSize = %d\n", m_BufferSize, bufferTextSize, copyTextSize );
    }
  }


private:
  char*     m_pBuffer;
  u32       m_BufferSize;
};


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_STRING_H_INCLUDED

#ifndef __GFL_STD_STRING_H__
#define __GFL_STD_STRING_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file	gfl_std_string.h
 *	@brief  C言語文字列処理関数 をラップしている関数
 *          POKEMONで扱う文字列はSTRBUFなどを利用すること。
 *          こちらはASCII等やデバッグ用途の利用であることを理解して使用してください
 *  @auther k.ohno
 *	@date	2014.05.22
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <string>
#include <stdlib.h>
#include <Types/include/gfl2_Typedef.h>

namespace gfl2 {
namespace std {

//-----------------------------------------------------------------------------
/**
 *					メモリ操作
*/
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  メモリコピー関数
 *
 *	@param	src   コピー元アドレス
 *	@param	dest  コピー先アドレス
 *	@param  size  コピーサイズ
 */
//-----------------------------------------------------------------------------
inline void MemCopy( const void* src, void* dest, size_t size ) { ::std::memcpy(dest,src,size); }


//----------------------------------------------------------------------------
/**
 *	@brief  メモリを指定データで埋めます。（8bit限定）
 *
 *	@param	dest    指定アドレス
 *	@param	data    データ(8bit)
 *	@param	size    埋めるサイズ
 */
//-----------------------------------------------------------------------------
inline void MemFill( void* dest, u8 data, size_t size ) { ::std::memset(dest,data,size); }

//----------------------------------------------------------------------------
/**
 *	@brief  メモリをクリア関数
 *
 *	@param	dest    指定アドレス
 *	@param	size    埋めるサイズ
 */
//-----------------------------------------------------------------------------
inline void MemClear( void* dest, size_t size ) { ::std::memset(dest,0,size); }

//----------------------------------------------------------------------------
/**
 *	@brief  メモリ比較関数
 *
 *	@param	dest    指定アドレス
 *	@param	size    埋めるサイズ
 *
 *	@retval == 0  等しい
 *	@retval != 0  異なる
 */
//-----------------------------------------------------------------------------
inline int MemComp( const void * s1, const void * s2, size_t size ) { return ::std::memcmp(s1,s2,size); }

//  --------------------------------------------------------------------
//! @brief  アドレスを加算する関数
//! @param  adr 入力アドレス
//! @param  val 値
//! @return 加算された値
//! @remark ポインターは符号無しなので減算の場合には SubAddress を使用してください。
//  -------------------------------------------------------------------- 
template<typename T, typename S>
inline T AddAddress( T adr, S val ) {
  return reinterpret_cast<T>( reinterpret_cast<size_t>(adr) + static_cast<size_t>(val) );
}

//  --------------------------------------------------------------------
//! @brief  アドレスを減算する関数
//! @param  adr 入力アドレス
//! @param  val 値 減算したい値の「絶対値」
//! @return 減算された値
//  -------------------------------------------------------------------- 
template<typename T, typename S>
inline T SubAddress( T adr, S val ) {
  return reinterpret_cast<T>( reinterpret_cast<size_t>(adr) - static_cast<size_t>(val) );
}

//-----------------------------------------------------------------------------
/**
 *					文字列操作
*/
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  文字列比較関数 ( 文字列*s1 と文字列*s2 を先頭からn文字比較します。'\0'以降の比較は行いません。)
 *	@param	str1  文字列１
 *	@param  str2  文字列２
 *	@param  n     長さ
 *	@retval == 0  等しい
 *	@retval != 0  異なる
 */
//-----------------------------------------------------------------------------
inline int StringCmp( const char * str1, const char * str2 ,size_t n) { return ::std::strncmp(str1, str2, n); }


//----------------------------------------------------------------------------
/**
 *	@brief   マルチバイト文字列をワイド文字列に変換
 *	@param   dest:変換後の文字列の格納バッファ
 *	@param   src:変換対象文字列
 *	@param   max:最大変換文字数
 *  @return  変換した文字数 失敗時-1
 */
//-----------------------------------------------------------------------------

inline size_t MultiByteStr2WideStr(  size_t *pReturnValue, wchar_t *wcs, size_t sizeInWords, const char *mbs, size_t count ){
#if defined(GF_PLATFORM_CTR)
	return mbstowcs( wcs, mbs , sizeInWords );
#elif defined(GF_PLATFORM_WIN32)
	return mbstowcs_s( pReturnValue, wcs, sizeInWords, mbs, count );
#endif

}

//----------------------------------------------------------------------------
/**
 *	@brief   ワイド文字列をマルチバイト文字列に変換
 *	@param   dest:変換後の文字列の格納バッファ
 *	@param   src:変換対象文字列
 *	@param   max:最大変換文字数
 *  @return  変換した文字数 失敗時-1
 */
//-----------------------------------------------------------------------------

inline size_t WideStr2MultiByteStr( size_t *pReturnValue, char *mbs, size_t sizeInBytes, const wchar_t *wcs, size_t count ){
#if defined(GF_PLATFORM_CTR)
	return wcstombs( mbs, wcs, sizeInBytes);
#elif defined(GF_PLATFORM_WIN32)
	return wcstombs_s(pReturnValue, mbs, sizeInBytes, wcs, count);
#endif
	
}



} /* namespace std end */
} /* namespace gfl2 end */
#endif // __GFL_STD_STRING_H__

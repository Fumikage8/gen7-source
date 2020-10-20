#ifndef __GFL_STD_H__
#define __GFL_STD_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_std.h
 *	@brief  スタンダードライブラリ
 *	@date		2010.10.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <cstring>
#include <nw.h>

#include <std/gfl_SoftwareKeyboard.h>
#include <std/gfl_PhotoSelect.h>
#include <std/gfl_Date.h>

namespace gfl {
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
inline void MemCopy( const void* src, void* dest, u32 size ) { ::std::memcpy(dest,src,size); }


//----------------------------------------------------------------------------
/**
 *	@brief  メモリを指定データで埋めます。（8bit限定）
 *
 *	@param	dest    指定アドレス
 *	@param	data    データ(8bit)
 *	@param	size    埋めるサイズ
 */
//-----------------------------------------------------------------------------
inline void MemFill( void* dest, u8 data, u32 size ) { ::std::memset(dest,data,size); }

//----------------------------------------------------------------------------
/**
 *	@brief  メモリをクリア関数
 *
 *	@param	dest    指定アドレス
 *	@param	size    埋めるサイズ
 */
//-----------------------------------------------------------------------------
inline void MemClear( void* dest, u32 size ) { ::std::memset(dest,0,size); }

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
inline int MemComp( const void * s1, const void * s2, u32 size ) { return ::std::memcmp(s1,s2,size); }


//----------------------------------------------------------------------------
/**
 *	@brief  文字列比較関数
 *
 *	@param	str1  文字列１
 *	@param  str2  文字列２
 *
 *	@retval == 0  等しい
 *	@retval != 0  異なる
 */
//-----------------------------------------------------------------------------
inline int StringCmp( const char * str1, const char * str2 ) { return nw::ut::stricmp( str1, str2 ); }


//----------------------------------------------------------------------------
/**
 *	@brief   マルチバイト文字列をワイド文字列に変換
 *	@param   dest:変換後の文字列の格納バッファ
 *	@param   src:変換対象文字列
 *	@param   max:最大変換文字数
 *  @return  変換した文字数 失敗時-1
 */
//-----------------------------------------------------------------------------

inline size_t MultiByteStr2WideStr( wchar_t *dest,const char *src,size_t max ){ return ::std::mbstowcs(dest,src,max); }

//----------------------------------------------------------------------------
/**
 *	@brief   ワイド文字列をマルチバイト文字列に変換
 *	@param   dest:変換後の文字列の格納バッファ
 *	@param   src:変換対象文字列
 *	@param   max:最大変換文字数
 *  @return  変換した文字数 失敗時-1
 */
//-----------------------------------------------------------------------------

inline size_t WideStr2MultiByteStr( char *dest,const wchar_t* src,size_t max ){ return ::std::wcstombs(dest,src,max); }




//----------------------------------------------------------------------------
/**
 *  @brief     STL
 */
//-----------------------------------------------------------------------------

// ::std::sort
template <class RandIter, class Comp> void Sort(RandIter start, RandIter end, Comp cmpfn) { return ::std::sort(start, end, cmpfn); }

// ::std::for_each
template <class InIter, class Func> Func ForEach(InIter start, InIter end, Func fn) { return ::std::for_each(start, end, fn); }

// ::std::bind1st
template <class Operation, class T> ::std::binder1st<Operation> Bind1st(const Operation& op, const T& x) { return ::std::bind1st(op, x); }

// ::std::mem_fun
//template <class Result, class T> ::std::mem_fun_t<Result, T> MemFun(Result (T::*func)()) { return ::std::mem_fun(func); }  // @todo mem_funcをうまく置き換えられない

// ::std::unary_function
template <class Argument, class Result> struct UnaryFunction
{
  typedef Argument argument_type;
  typedef Result   result_type;
};



//-----------------------------------------------------------------------------
/**
 *					アドレス計算
*/
//-----------------------------------------------------------------------------

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


} /* namespace std end */
} /* namespace gfl end */
#endif // __GFL_STD_H__

#ifndef __GFL_STD_H__
#define __GFL_STD_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_std.h
 *	@brief  �X�^���_�[�h���C�u����
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
 *					����������
*/
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �������R�s�[�֐�
 *
 *	@param	src   �R�s�[���A�h���X
 *	@param	dest  �R�s�[��A�h���X
 *	@param  size  �R�s�[�T�C�Y
 */
//-----------------------------------------------------------------------------
inline void MemCopy( const void* src, void* dest, u32 size ) { ::std::memcpy(dest,src,size); }


//----------------------------------------------------------------------------
/**
 *	@brief  ���������w��f�[�^�Ŗ��߂܂��B�i8bit����j
 *
 *	@param	dest    �w��A�h���X
 *	@param	data    �f�[�^(8bit)
 *	@param	size    ���߂�T�C�Y
 */
//-----------------------------------------------------------------------------
inline void MemFill( void* dest, u8 data, u32 size ) { ::std::memset(dest,data,size); }

//----------------------------------------------------------------------------
/**
 *	@brief  ���������N���A�֐�
 *
 *	@param	dest    �w��A�h���X
 *	@param	size    ���߂�T�C�Y
 */
//-----------------------------------------------------------------------------
inline void MemClear( void* dest, u32 size ) { ::std::memset(dest,0,size); }

//----------------------------------------------------------------------------
/**
 *	@brief  ��������r�֐�
 *
 *	@param	dest    �w��A�h���X
 *	@param	size    ���߂�T�C�Y
 *
 *	@retval == 0  ������
 *	@retval != 0  �قȂ�
 */
//-----------------------------------------------------------------------------
inline int MemComp( const void * s1, const void * s2, u32 size ) { return ::std::memcmp(s1,s2,size); }


//----------------------------------------------------------------------------
/**
 *	@brief  �������r�֐�
 *
 *	@param	str1  ������P
 *	@param  str2  ������Q
 *
 *	@retval == 0  ������
 *	@retval != 0  �قȂ�
 */
//-----------------------------------------------------------------------------
inline int StringCmp( const char * str1, const char * str2 ) { return nw::ut::stricmp( str1, str2 ); }


//----------------------------------------------------------------------------
/**
 *	@brief   �}���`�o�C�g����������C�h������ɕϊ�
 *	@param   dest:�ϊ���̕�����̊i�[�o�b�t�@
 *	@param   src:�ϊ��Ώە�����
 *	@param   max:�ő�ϊ�������
 *  @return  �ϊ����������� ���s��-1
 */
//-----------------------------------------------------------------------------

inline size_t MultiByteStr2WideStr( wchar_t *dest,const char *src,size_t max ){ return ::std::mbstowcs(dest,src,max); }

//----------------------------------------------------------------------------
/**
 *	@brief   ���C�h��������}���`�o�C�g������ɕϊ�
 *	@param   dest:�ϊ���̕�����̊i�[�o�b�t�@
 *	@param   src:�ϊ��Ώە�����
 *	@param   max:�ő�ϊ�������
 *  @return  �ϊ����������� ���s��-1
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
//template <class Result, class T> ::std::mem_fun_t<Result, T> MemFun(Result (T::*func)()) { return ::std::mem_fun(func); }  // @todo mem_func�����܂��u���������Ȃ�

// ::std::unary_function
template <class Argument, class Result> struct UnaryFunction
{
  typedef Argument argument_type;
  typedef Result   result_type;
};



//-----------------------------------------------------------------------------
/**
 *					�A�h���X�v�Z
*/
//-----------------------------------------------------------------------------

//  --------------------------------------------------------------------
//! @brief  �A�h���X�����Z����֐�
//! @param  adr ���̓A�h���X
//! @param  val �l
//! @return ���Z���ꂽ�l
//! @remark �|�C���^�[�͕��������Ȃ̂Ō��Z�̏ꍇ�ɂ� SubAddress ���g�p���Ă��������B
//  -------------------------------------------------------------------- 
template<typename T, typename S>
inline T AddAddress( T adr, S val ) {
  return reinterpret_cast<T>( reinterpret_cast<size_t>(adr) + static_cast<size_t>(val) );
}

//  --------------------------------------------------------------------
//! @brief  �A�h���X�����Z����֐�
//! @param  adr ���̓A�h���X
//! @param  val �l ���Z�������l�́u��Βl�v
//! @return ���Z���ꂽ�l
//  -------------------------------------------------------------------- 
template<typename T, typename S>
inline T SubAddress( T adr, S val ) {
  return reinterpret_cast<T>( reinterpret_cast<size_t>(adr) - static_cast<size_t>(val) );
}


} /* namespace std end */
} /* namespace gfl end */
#endif // __GFL_STD_H__

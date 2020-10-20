#if !defined(GFL2_FSWIN32FUNC_H_INCLUDED)
#define GFL2_FSWIN32FUNC_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsWin32Func.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   Win32ファイル便利関数
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


// fsのインクルード
#include <fs/include/gfl2_FsToolDefine.h>
#include <fs/include/gfl2_FsDefine.h>
#include <fs/include/gfl2_FsResult.h>


#if   defined(GF_PLATFORM_WIN32)


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


//==============================================================================
/**
 @class     Win32Func
 @brief     Win32ファイル便利関数
 */
//==============================================================================
class Win32Func
{
  GFL_FORBID_COPY_AND_ASSIGN(Win32Func);

  friend class Win32FileCore;
  friend class Win32Util;

private:
  //! @name  friendからアクセスする
  //@{

  //! @brief  ディレクトリをつくる
  //! @param[in]  fullName  フルパスでないとダメ？　/区切りでないとダメ？　使用箇所が限定されているのでテスト不十分です。
  static Result CreateDir(const char* fullName);

  //@}
};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GF_PLATFORM_WIN32


#endif  // GFL2_FSWIN32FUNC_H_INCLUDED

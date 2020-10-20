#if !defined(GFL2_FSPCUTIL_H_INCLUDED)
#define GFL2_FSPCUTIL_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsPcUtil.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.03.17
 @brief   PCファイルユーティリティ

 @note    PCにあるファイルを扱うクラス。
          Win32版はそのままPCにアクセスできる。
          CTR版はHIOを使ってPCにアクセスしている。
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


// fsのインクルード
#include <fs/include/gfl2_FsToolDefine.h>
#include <fs/include/gfl2_FsDefine.h>

#if   defined(GF_PLATFORM_WIN32)
  #include "../source/Win32/gfl2_FsWin32Util.h"
#elif defined(GF_PLATFORM_CTR)
  #include "../source/CTR/gfl2_FsCtrHioUtil.h"
#endif  // GF_PLATFORM_


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


//==============================================================================
/**
 @class     PcUtil
 @brief     PCファイルユーティリティ
 */
//==============================================================================
#if   defined(GF_PLATFORM_WIN32)
  typedef Win32Util  PcUtil;
#elif defined(GF_PLATFORM_CTR)
  typedef CtrHioUtil PcUtil;
#endif  // GF_PLATFORM_


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL2_FSPCUTIL_H_INCLUDED


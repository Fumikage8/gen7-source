#if !defined(GFL2_FS_H_INCLUDED)
#define GFL2_FS_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_Fs.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   ファイルシステム
 */
//==============================================================================


// fsプロジェクトフォルダ外はこのファイルだけをインクルードして下さい。


// gfl2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

// fs
#include <fs/include/gfl2_FsToolDefine.h>
#include <fs/include/gfl2_FsDefine.h>
#include <fs/include/gfl2_FsResult.h>
#include <fs/include/gfl2_FsSystem.h>
#include <fs/include/gfl2_FsArcFile.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <fs/include/gfl2_FsNameTable.h>
#include <fs/include/gfl2_FsDirectSdmcFile.h>
#include <fs/include/gfl2_FsPcUtil.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)

class DeviceBase;

GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL2_FS_H_INCLUDED

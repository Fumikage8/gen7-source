#if defined(GF_PLATFORM_WIN32)
//*************************************************************************************************
/**
 @file file_reader.h
 @brief データファイル読み込み
 @date 2015.01.22
 @author saita_kazuki
 */
//*************************************************************************************************

#ifndef __SKELTON_MAPJUMP_DATALOADER_H__
#define __SKELTON_MAPJUMP_DATALOADER_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)

//=================================================================================================
// 設定データ読み込みクラス
//=================================================================================================
class FileReader
{
public:

  FileReader();
  virtual ~FileReader();
  
  /**
  * @brief データ読み込み
  * @param pFilePath データパス
  * @return  無し 
  */
  c8* ReadFile( const c8* pFilePath, s32* pSize, s32 align = 16);
  c8* ReadFile( const c16* pFilePath, s32* pSize, s32 align = 16);
};

GFL_NAMESPACE_END(Skelton)
GFL_NAMESPACE_END(Test)

#endif // __SKELTON_MAPJUMP_DATALOADER_H__
#endif // GF_PLATFORM_WIN32
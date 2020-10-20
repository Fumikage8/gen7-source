#if !defined(GFL2_FSCTRROMDEVICE_H_INCLUDED)
#define GFL2_FSCTRROMDEVICE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsCtrRomDevice.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   CTR(3DS)のrom
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
#include "../gfl2_FsDeviceBase.h"


#if   defined(GF_PLATFORM_CTR)


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


//==============================================================================
/**
 @class     CtrRomDevice
 @brief     CTR(3DS)のrom
 */
//==============================================================================
class CtrRomDevice : public DeviceBase
{
  GFL_FORBID_COPY_AND_ASSIGN(CtrRomDevice);

  friend class System;

private:
  //! @brief  コンストラクタ
  CtrRomDevice(void);
  //! @brief  デストラクタ
  virtual ~CtrRomDevice();

public:
  //! @brief  デバイスタイプを取得する
  virtual Define::DeviceType::Tag GetDeviceType(void) const;

  //! @brief  マウント
  //! @param[in] heap  ヒープ。パス名の記憶に使用。romアーカイブの動作に使用するメモリ領域の確保も行う。Unmountするまで左記のものが保持され続ける。
  //! @param[in] archiveName  アーカイブ名。終端は'\0'になっていること。
  //                          例：archiveName="rom:" ("/"はどこにも入りません)
  //! @param[in] path  パス。先頭の'/'は書いて下さい。末尾の'/'は書いても書かなくてもどちらでもよい。終端は'\0'になっていること。
  //                   以降ファイル名を指定するときはpathより後を指定する。
  //                   例：path="/data/"("C:/home/niji/data/"があり、romfiles="C:/home/niji"となっているときの例)
  //                       fileName="sample/sample_data.txt"
  //                       とすると、"rom:/data/sample/sample_data.txt"即ち"C:/home/niji/data/sample/sample_data.txt"が対象となる。
  //                       (実際は、romfilesはアーカイブされており、そこから呼び出します。PCから呼び出すわけではありません。)
  Result Mount(gfl2::heap::HeapBase* heap, const char* archiveName, const char* path, size_t maxFile, size_t maxDirectory, bool useCache);

  //! @brief  アンマウント
  virtual Result Unmount(void);

  //! @brief  フルパスファイル名を得る
  virtual Result GetFullPathFileName(u32* fullNameLen, char* fullName, u32 fullNameBufSize, const char* fileName) const;

private:
  char* m_archiveName;     // 終端の'\0'の分までメモリを確保しているので、(m_archiveNameLen +1)ある。終端は'\0'になっている。"rom:"とか。
  u32   m_archiveNameLen;  // 終端の'\0'は含まない長さ
  char* m_path;     // 終端の'\0'の分までメモリを確保しているので、(m_pathLen +1)ある。終端は'\0'になっている。区切りは/になっている。末尾には/が付いている。
  u32   m_pathLen;  // 終端の'\0'は含まない長さ

  size_t m_maxFile;
  size_t m_maxDirectory;
  bool   m_useCache;
  void*  m_workingMemory;
  size_t m_workingMemorySize;
};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GF_PLATFORM_CTR


#endif  // GFL2_FSCTRROMDEVICE_H_INCLUDED


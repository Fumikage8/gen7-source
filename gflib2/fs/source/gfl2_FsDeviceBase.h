#if !defined(GFL2_FSDEVICEBASE_H_INCLUDED)
#define GFL2_FSDEVICEBASE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsDeviceBase.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   デバイス(ボリューム、イメージ)
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


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


//==============================================================================
/**
 @class     DeviceBase
 @brief     デバイス
 */
//==============================================================================
class DeviceBase
{
  GFL_FORBID_COPY_AND_ASSIGN(DeviceBase);

  friend class System;

protected:
  //! @brief  コンストラクタ
  DeviceBase(void);
  //! @brief  デストラクタ
  virtual ~DeviceBase();

public:
  //! @brief  デバイスタイプを取得する
  virtual Define::DeviceType::Tag GetDeviceType(void) const;

  //! @brief  マウント
  //! @param[in] heap  ヒープ。パス名の記憶に使用。Unmountするまで左記のものが保持され続ける。
  //! @param[in] path  パス。末尾の'/'は書いても書かなくてもどちらでもよい。終端は'\0'になっていること。
  //                   以降ファイル名を指定するときはpathより後を指定する。
  //                   例：path="C:/home/niji/" (path="C:/home/niji", path="C:\\home\\niji\\", path="C:\\home\\niji" どれでも同じ)
  //                       fileName="git_gflib2/fs/Sample/data/read_data.txt"
  //                       とすると、"C:/home/niji/git_gflib2/fs/Sample/data/read_data.txt" が対象となる。
  //! @note  派生クラスで実装して下さい。
  // Result Mount(gfl2::heap::HeapBase* heap, const char* path);
  
  //! @brief  アンマウント
  //! @note  派生クラスで実装して下さい。
  virtual Result Unmount(void);

  //! @brief  フルパスファイル名を得る
  //! @param[out] fullNameLen      fullNameに入れたフルパスファイル名の長さ。終端の'\0'は含まない長さ。
  //! @param[out] fullName         呼び出し元であらかじめメモリを確保しておいたバッファ。ここにフルパスファイル名を入れる。
  //!                              終端は'\0'にして入れます。区切りは/にして入れます。
  //! @param[in]  fullNameBufSize  fullNameに確保した長さ。終端の'\0'も含めた長さ。fullNameのバイト数。
  //! @param[in]  fileName         ファイル名。終端は'\0'になっていること。区切りは\でも/でもよい。
  //! @note  派生クラスで実装して下さい。
  virtual Result GetFullPathFileName(u32* fullNameLen, char* fullName, u32 fullNameBufSize, const char* fileName) const;

protected:
  bool m_mountFlag;
};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL2_FSDEVICEBASE_H_INCLUDED


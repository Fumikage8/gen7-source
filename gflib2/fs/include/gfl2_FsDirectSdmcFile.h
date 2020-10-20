#if !defined(GFL2_FSDIRECTSDMCFILE_H_INCLUDED)
#define GFL2_FSDIRECTSDMCFILE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsDirectSdmcFile.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.03.30
 @brief   DirectSdmc用のファイル
          Win32ではPCが、CTRではSDカード(デバッグ用)がデバイスになります。
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
#include <fs/include/internal/gfl2_FsFile.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


//==============================================================================
/**
 @class     DirectSdmcFile
 @brief     DirectSdmc用のファイル
 @note      各関数の説明はFile, FileBaseを見て下さい。
 */
//==============================================================================
class DirectSdmcFile : public File
{
  GFL_FORBID_COPY_AND_ASSIGN(DirectSdmcFile);

  friend class System;


  //! @name  friendがアクセスできる関数
  //@{

private:
  //! @brief  コンストラクタ
  DirectSdmcFile(void);
  //! @brief  デストラクタ
  virtual ~DirectSdmcFile();

  //! @name  オープン/クローズ
  //@{
 
  //! @brief  ファイルを開く
  Result Open(gfl2::heap::HeapBase* heap, const DeviceBase* device, const char* fileName, Define::OpenMode::Tag openMode);
  //! @brief  ファイルを閉じる
  virtual Result Close(void);

  //@}

  //@}


  /*
public:
  // Fileにある下記のpublic関数が使えます。
  Result Write(const void* pBuf, size_t bufSize);

  // FileBaseにある下記のpublic関数が使えます。
  Result GetFileSize(size_t* pFileSize) const;
  Result Read(void** ppBuf, size_t* pBufSize, size_t* pRealReadSize, size_t readSize, gfl2::heap::HeapBase* heap, u32 align);
  Result Read(void** ppBuf, size_t* pBufSize, size_t* pRealReadSize, size_t pos, size_t readSize, gfl2::heap::HeapBase* heap, u32 align);
  Result Read(size_t* pRealReadSize, void* pBuf, size_t bufSize, size_t readSize);
  Result Read(size_t* pRealReadSize, void* pBuf, size_t bufSize, size_t pos, size_t readSize);
  Result Seek(size_t offset, Define::Origin::Tag origin);
  Result Tell(size_t* pos) const;
  */


};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL2_FSDIRECTSDMCFILE_H_INCLUDED


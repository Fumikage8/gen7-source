#if !defined(GFL2_FSFILE_H_INCLUDED)
#define GFL2_FSFILE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsFile.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   通常ファイル
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
#include <fs/include/internal/gfl2_FsFileBase.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


//==============================================================================
/**
 @class     File
 @brief     通常ファイル
 @note      各関数の説明はFileBaseを見て下さい。
 */
//==============================================================================
class File : public FileBase
{
  GFL_FORBID_COPY_AND_ASSIGN(File);

public:
  //! @brief  コンストラクタ
  File(void);
  //! @brief  デストラクタ
  virtual ~File();


  //! @name  オープン/クローズ
  //@{
 
  //! @brief  ファイルを開く
  //! @param[in] heap      ヒープ。ファイル名の記憶、ファイル核の生成に使用。Closeするまで左記のものが保持される。
  //! @param[in] device    デバイス。
  //! @param[in] fileName  ファイル名。終端は'\0'になっていること。区切りは\でも/でもよい。
  //! @param[in] openMode  オープンモード。
  virtual Result Open(gfl2::heap::HeapBase* heap, const DeviceBase* device, const char* fileName, Define::OpenMode::Tag openMode);

  //! @brief  ファイルを閉じる
  virtual Result Close(void);

  //@}


  //! @name  リード/ライト
  //@{

  //! @brief  ファイルに書き込む
  //! @param[in]  pBuf          ファイルに書き込む内容。
  //! @param[in]  bufSize       pBufのサイズ。
  //! @attention  ファイルのアクセス位置が移動する
  Result Write(const void* pBuf, size_t bufSize);
  
  //@}
};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL2_FSFILE_H_INCLUDED


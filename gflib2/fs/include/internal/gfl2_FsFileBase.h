#if !defined(GFL2_FSFILEBASE_H_INCLUDED)
#define GFL2_FSFILEBASE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsFileBase.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   ファイル基底クラス
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
#include <fs/include/internal/gfl2_FsFileCoreBase.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


// 前方宣言
class DeviceBase;


//==============================================================================
/**
 @class     FileBase
 @brief     ファイル基底クラス
 @note      各関数の説明はFileCoreBaseを見て下さい。
 */
//==============================================================================
class FileBase
{
  GFL_FORBID_COPY_AND_ASSIGN(FileBase);

public:
  //! @brief  コンストラクタ
  FileBase(void);
  //! @brief  デストラクタ
  virtual ~FileBase();


  //! @name  オープン/クローズ
  //@{
  
  //! @brief  ファイルを開く
  //! @note  派生クラスで実装して下さい。
  // Result Open(...);

  //! @brief  ファイルを閉じる
  //! @note  派生クラスで実装して下さい。
  virtual Result Close(void);

  //@}


  //! @brief  ファイル名が同じかチェックする
  Result CheckFileNameIsSame(bool* sameFlag, const char* fileName) const;

  //! @brief  ファイルサイズを得る
  Result GetFileSize(size_t* pFileSize) const;


  //! @name  リード/ライト
  //@{

  //! @brief  ファイルから読み込む
  Result Read(void** ppBuf, size_t* pBufSize, size_t* pRealReadSize, size_t readSize, gfl2::heap::HeapBase* heap, u32 align);

  //! @brief  ファイルから読み込む
  Result Read(void** ppBuf, size_t* pBufSize, size_t* pRealReadSize, size_t pos, size_t readSize, gfl2::heap::HeapBase* heap, u32 align);

  //! @brief  ファイルから読み込む
  Result Read(size_t* pRealReadSize, void* pBuf, size_t bufSize, size_t readSize);
 
  //! @brief  ファイルから読み込む
  Result Read(size_t* pRealReadSize, void* pBuf, size_t bufSize, size_t pos, size_t readSize);

  //! @brief  ファイルに書き込む
  //! @note  派生クラスで実装して下さい。
  // Result Write(...);
  
  //@}


  //! @brief  ファイルのアクセス位置を移動する
  Result Seek(size_t offset, Define::Origin::Tag origin);

  //! @brief  ファイルの先頭からのアクセス位置を得る
  Result Tell(size_t* pos) const;


  //! @brief  ファイル名を得る
  const char* GetFileName(void) const { if(m_fileCore) return m_fileCore->GetFileName(); else return NULL; }


protected:
  //! @brief  ファイルを開く
  //! @param[in] heap      ヒープ。ファイル名の記憶、ファイル核の生成に使用。Closeするまで左記のものが保持される。
  //! @param[in] device    デバイス。
  //! @param[in] fileName  ファイル名。終端は'\0'になっていること。区切りは\でも/でもよい。
  //! @param[in] openMode  オープンモード。
  Result open(gfl2::heap::HeapBase* heap, const DeviceBase* device, const char* fileName, Define::OpenMode::Tag openMode);
  
  //! @brief  ファイルを閉じる
  Result close(void);


protected:
  FileCoreBase* m_fileCore;
};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL2_FSFILEBASE_H_INCLUDED


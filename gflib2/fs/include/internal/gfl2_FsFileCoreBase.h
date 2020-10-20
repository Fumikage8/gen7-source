#if !defined(GFL2_FSFILECOREBASE_H_INCLUDED)
#define GFL2_FSFILECOREBASE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsFileCoreBase.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   ファイル核
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


// 前方宣言
class DeviceBase;


//==============================================================================
/**
 @class     FileCoreBase
 @brief     ファイル核
 */
//==============================================================================
class FileCoreBase
{
  GFL_FORBID_COPY_AND_ASSIGN(FileCoreBase);

public:
  //! @brief  コンストラクタ
  FileCoreBase(void);
  //! @brief  デストラクタ
  virtual ~FileCoreBase();


  //! @name  オープン/クローズ
  //@{

  //! @brief  ファイルを開く
  //! @param[in] heap      ヒープ。ファイル名の記憶に使用。Closeするまで左記のものが保持される。
  //! @param[in] device    デバイス。
  //! @param[in] fileName  ファイル名。終端は'\0'になっていること。区切りは\でも/でもよい。
  //! @param[in] openMode  オープンモード。
  Result Open(gfl2::heap::HeapBase* heap, const DeviceBase* device, const char* fileName, Define::OpenMode::Tag openMode);

  //! @brief  ファイルを開く
  //! @param[in] device    デバイス。
  //! @param[in] fileName  ファイル名。終端は'\0'になっていること。区切りは\でも/でもよい。
  //! @param[in] openMode  オープンモード。
  //! @note  ファイル名は記憶されない。
  Result Open(const DeviceBase* device, const char* fileName, Define::OpenMode::Tag openMode);
  
  //! @brief  ファイルを閉じる
  Result Close(void);

  //@}


  //! @brief  ファイル名が同じかチェックする
  //! @param[out] sameFlag  同じならtrue、異なるならfalseを入れて返す。
  //! @param[in]  fileName  ファイル名。終端は'\0'になっていること。区切りは\でも/でもよい。
  Result CheckFileNameIsSame(bool* sameFlag, const char* fileName) const;

  //! @brief  ファイルサイズを得る
  Result GetFileSize(size_t* pFileSize) const;


  //! @name  リード/ライト
  //@{

  //! @brief  ファイルから読み込む
  //! @param[out] ppBuf          heapから確保したメモリが渡されるポインタ。呼び出し元でGflHeapFreeMemory(*ppBuf)で解放して下さい。
  //! @param[out] pBufSize       heapから確保したメモリのサイズ。heapの管理領域のサイズは含まれていない。NULLでもいい。
  //! @param[out] pRealReadSize  実際に読み込んだサイズ。(*pRealReadSize <= readSize)となる。NULLでもいい。
  //! @param[in]  readSize       読み込むサイズ。(*pBufSize >= readSize)となる。
  //! @param[in]  heap           ファイルから読み込んだ内容を入れておくバッファを確保するためのヒープ。
  //! @param[in]  align          heapから確保する際のアラインメント。2のべき乗(1, 2, 4, 8, 16, 32, 64, 128, ...)にして下さい。
  //! @attention  ファイルのアクセス位置が移動する
  Result Read(void** ppBuf, size_t* pBufSize, size_t* pRealReadSize, size_t readSize, gfl2::heap::HeapBase* heap, u32 align);

  //! @brief  ファイルから読み込む
  //! @param[out] ppBuf          heapから確保したメモリが渡されるポインタ。呼び出し元でGflHeapFreeMemory(*ppBuf)で解放して下さい。
  //! @param[out] pBufSize       heapから確保したメモリのサイズ。heapの管理領域のサイズは含まれていない。NULLでもいい。
  //! @param[out] pRealReadSize  実際に読み込んだサイズ。(*pRealReadSize <= readSize)となる。NULLでもいい。
  //! @param[in]  pos            読み込み開始位置(ファイルの先頭からの位置)。
  //! @param[in]  readSize       読み込むサイズ。(*pBufSize >= readSize)となる。
  //! @param[in]  heap           ファイルから読み込んだ内容を入れておくバッファを確保するためのヒープ。
  //! @param[in]  align          heapから確保する際のアラインメント。2のべき乗(1, 2, 4, 8, 16, 32, 64, 128, ...)にして下さい。
  //! @attention  ファイルのアクセス位置が移動する
  Result Read(void** ppBuf, size_t* pBufSize, size_t* pRealReadSize, size_t pos, size_t readSize, gfl2::heap::HeapBase* heap, u32 align);

  //! @brief  ファイルから読み込む
  //! @param[out] pRealReadSize  実際に読み込んだサイズ。(*pRealReadSize <= readSize)となる。NULLでもいい。
  //! @param[out] pBuf           呼び出し元であらかじめメモリを確保しておいたバッファ。ここにファイルから読み込んだ内容を入れる。
  //! @param[in]  bufSize        pBufのサイズ。readSize以上にして下さい。
  //! @param[in]  readSize       読み込むサイズ。
  //! @attention  ファイルのアクセス位置が移動する
  Result Read(size_t* pRealReadSize, void* pBuf, size_t bufSize, size_t readSize);
 
  //! @brief  ファイルから読み込む
  //! @param[out] *pRealReadSize  実際に読み込んだサイズ。(*pRealReadSize <= readSize)となる。NULLでもいい。
  //! @param[out] *pBuf           呼び出し元であらかじめメモリを確保しておいたバッファ。ここにファイルから読み込んだ内容を入れる。
  //! @param[in]  bufSize         bufのサイズ。readSize以上にして下さい。
  //! @param[in]  pos             読み込み開始位置(ファイルの先頭からの位置)。
  //! @param[in]  readSize        読み込むサイズ。
  //! @attention  ファイルのアクセス位置が移動する
  Result Read(size_t* pRealReadSize, void* pBuf, size_t bufSize, size_t pos, size_t readSize);

  //! @brief  ファイルに書き込む
  //! @param[in]  pBuf          ファイルに書き込む内容。
  //! @param[in]  bufSize       pBufのサイズ。
  //! @attention  ファイルのアクセス位置が移動する
  Result Write(const void* pBuf, size_t bufSize);

  //@}


  //! @brief  ファイルのアクセス位置を移動する
  //! @param[in]  offset  originからの位置
  //! @param[in]  origin  アクセス位置の基準
  Result Seek(size_t offset, Define::Origin::Tag origin);

  //! @brief  ファイルのアクセス位置を得る
  //! @param[out]  pos  ファイルの先頭からの位置
  Result Tell(size_t* pos) const;


  //! @brief  ファイル名を得る
  const char* GetFileName(void) const { return m_fileName; }


private:
  Result open(const DeviceBase* device, const char* fileName, Define::OpenMode::Tag openMode);


protected:
  //! @name  デバイスに応じた処理
  //@{
  
  //! @note ForDevice関数はデバイスに応じた処理を行うので、デバイスに応じた派生クラスで実装する。
  //! @note ForDevice関数内で使用するメンバ変数は、m_deviceと派生クラス内のメンバ変数だけ。

  virtual Result openForDevice(const char* fullName, Define::OpenMode::Tag openMode) { Result res; return res; }
  virtual Result closeForDevice(Define::OpenMode::Tag openMode) { Result res; return res; }
  virtual Result getFileSizeForDevice(size_t* pFileSize) const { Result res; return res; }

  //! @brief  ファイルから読み込む
  //! @param[out] pRealReadSize  実際に読み込んだサイズ。(*pRealReadSize <= readSize)となる。NULLでもいい。
  //! @param[out] pBuf           呼び出し元であらかじめメモリを確保しておいたバッファ。ここにファイルから読み込んだ内容を入れる。
  //! @param[in]  bufSize        pBufのサイズ。readSize以上にして下さい。
  //! @param[in]  readSize       読み込むサイズ。
  //! @attention  ファイルのアクセス位置が移動する
  virtual Result readForDevice(size_t* pRealReadSize, void* pBuf, size_t bufSize, size_t readSize) { Result res; return res; }
  
  virtual Result writeForDevice(const void* pBuf, size_t bufSize) { Result res; return res; }
  virtual Result seekForDevice(size_t offset, Define::Origin::Tag origin) { Result res; return res; }
  virtual Result tellForDevice(size_t* pos) const { Result res; return res; }

  //@}


protected:
  const DeviceBase*      m_device;

private:
  char*  m_fileName;     // 終端の'\0'の分までメモリを確保しているので、(m_fileNameLen +1)ある。終端は'\0'になっている。区切りは/になっている。
  u32    m_fileNameLen;  // 終端の'\0'は含まない長さ
  Define::OpenMode::Tag  m_openMode;
  bool   m_openFlag;
};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL2_FSFILECOREBASE_H_INCLUDED


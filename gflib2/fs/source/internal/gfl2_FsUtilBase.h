#if !defined(GFL2_FSUTILBASE_H_INCLUDED)
#define GFL2_FSUTILBASE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsUtilBase.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.03.17
 @brief   ファイルユーティリティ基底クラス
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
class AsyncFileManager;


//==============================================================================
/**
 @class     UtilBase
 @brief     ファイルユーティリティ基底クラス
 */
//==============================================================================
class UtilBase
{
  GFL_FORBID_COPY_AND_ASSIGN(UtilBase);

public:
  //! @brief  コンストラクタ
  UtilBase(void);
  //! @brief  デストラクタ
  virtual ~UtilBase();

public:
  //! @brief  PCへのアクセスが有効になっているか否か
  //! @param[out]  有効になっているときtrue。有効になっていないときfalse。
  //! @note        Win32版では必ずtrueになる。
  //!              CTR版ではHIOが有効のときだけtrueになる。
  //! @return      異常がなければres.IsSuccess()がtrueになるが、そのときenableFlagはtrueもfalseもあり得る。
  //!              異常があればfalseになり、そのときenableFlagは必ずfalseになる。
  static Result IsPcAccessEnableFlag(bool* enableFlag);

  //! @brief  環境変数の値(文字列)を取得する
  //! @param[out] pEnvVarLen    pEnvVarに入れた値(文字列)の長さ。終端の'\0'は含まない長さ。NULLでもよい。
  //! @param[out] pEnvVar       呼び出し元であらかじめ確保しておいたバッファ。ここに環境変数の値(文字列)を入れる。
  //! @param[in]  envVarBufSize pEnvVarに確保した長さ。終端の'\0'も含めた長さ。pEnvVarのバイト数。
  //! @param[in]  envName       環境変数名。
  //! @return  環境変数が存在しない場合でもres.IsSuccess()が真となります。
  //!          環境変数が存在しない場合もしくは環境変数の値(文字列)が空の場合は*pEnvVarLen=0, pEnvVar[0]='\0'となります。
  static Result GetProcessEnvironmentVariable(u32* pEnvVarLen, char* pEnvVar, u32 envVarBufSize, const char* envName);

  //! @brief  カレントディレクトリを取得する
  //! @param[out]  path             呼び出し元であらかじめメモリを確保しておいたバッファ。ここにカレントディレクトリを入れる。
  //!                               終端は'\0'にして入れます。区切りは不明。末尾に'/'があるかは不明。例：C:\home\niji  例：C:/home/niji
  //! @param[in]   pathBufSize      pathに確保した長さ。終端の'\0'も含めた長さ。pathのバイト数。
  static Result GetCurrentWorkDirectory(char* path, u32 pathBufSize);

  //! @brief  カレントディレクトリを設定する
  //! @param[in]   path             終端は'\0'になっていること。例：C:\home\niji  例：C:/home/niji
  //! @param[out]  prevPath         呼び出し元であらかじめメモリを確保しておいたバッファ。ここに変更前のカレントディレクトリを入れる。
  //!                               終端は'\0'にして入れます。区切りは不明。末尾に'/'があるかは不明。例：C:\home\niji  例：C:/home/niji
  //!                               NULLでもよい。
  //! @param[in]   prevPathBufSize  prevPathに確保した長さ。終端の'\0'も含めた長さ。prevPathのバイト数。
  //!                               prevPathがNULLのときは使われない。
  static Result SetCurrentWorkDirectory(const char* path, char* prevPath, u32 prevPathBufSize);

  //! @brief  ファイルの有無を確認する
  //! @param[out] pExist       ファイルが存在する場合はtrueを格納し、存在しない場合はfalseを格納する。
  //! @param[in]  fileName     ファイル名。終端は'\0'になっていること。
  //! @return  ファイルが存在しない場合でもres.IsSuccess()が真となります。
  //!          ファイルが存在しない場合は*pExist=falseとなります。
  static Result CheckFileExist(bool* pExist, const char* fileName);

  //! @brief  ファイルサイズを得る
  //! @param[out] pFileSize    ファイルサイズ
  //! @param[in]  fileName     ファイル名。終端は'\0'になっていること。
  static Result GetFileSize(size_t* pFileSize, const char* fileName);

  //! @brief  ファイルから読み込む
  //! @param[out] ppBuf        heapForBufから確保したメモリが渡されるポインタ。呼び出し元でGflHeapFreeMemory(*ppBuf)で解放して下さい。
  //! @param[out] pBufSize     heapForBufから確保したメモリのサイズ。heapの管理領域のサイズは含まれていない。NULLでもよい。
  //! @param[out] pFileSize    ファイルサイズ。(*pBufSize >= *pFileSize)となる。NULLでもよい。
  //! @param[in]  fileName     ファイル名。終端は'\0'になっていること。
  //! @param[in]  heapforBuf   ファイルから読み込んだ内容を入れておくバッファを確保するためのヒープ。
  //! @param[in]  align        heapForBufから確保する際のアラインメント。2のべき乗(1, 2, 4, 8, 16, 32, 64, 128, ...)にして下さい。
  static Result ReadFile(void** ppBuf, size_t* pBufSize, size_t* pFileSize, const char* fileName, gfl2::heap::HeapBase* heapForBuf, u32 align);
  
  //! @brief  ファイルから読み込む
  //! @param[out] pFileSize    ファイルサイズ。NULLでもよい。
  //! @param[out] pBuf         呼び出し元であらかじめメモリを確保しておいたバッファ。ここにファイルから読み込んだ内容を入れる。
  //! @param[in]  bufSize      pBufのサイズ。*pFielSize以上にして下さい。
  //! @param[in]  fileName     ファイル名。終端は'\0'になっていること。
  static Result ReadFile(size_t* pFileSize, void* pBuf, size_t bufSize, const char* fileName);

  //! @brief  ファイルに書き込む
  //! @param[in]  fileName     ファイル名。終端は'\0'になっていること。
  //! @param[in]  pBuf         ファイルに書き込む内容。
  //! @param[in]  bufSize      pBufのサイズ。
  static Result WriteFile(const char* fileName, const void* pBuf, size_t bufSize);

  //! @brief  フルパスファイル名を得る
  //! @param[out] fullNameLen      fullNameに入れたフルパスファイル名の長さ。終端の'\0'は含まない長さ。
  //! @param[out] fullName         呼び出し元であらかじめメモリを確保しておいたバッファ。ここにフルパスファイル名を入れる。
  //!                              終端は'\0'にして入れます。区切りは/にして入れます。
  //! @param[in]  fullNameBufSize  fullNameに確保した長さ。終端の'\0'も含めた長さ。fullNameのバイト数。
  //! @param[in]  manager          ファイルマネージャ
  //! @param[in]  fileName         ファイル名。終端は'\0'になっていること。区切りは\でも/でもよい。
  //!                              ファイルマネージャmanagerに渡すリクエストに書くファイル名と同じものを書いて下さい。
  static Result GetFullPathFileName(
      u32* fullNameLen, char* fullName, u32 fullNameBufSize,
      AsyncFileManager* manager,
      const char* fileName
  );
 
  //! @brief  ディレクトリを生成する
  static Result CreateDir(const char* fullName);
};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL2_FSUTILBASE_H_INCLUDED

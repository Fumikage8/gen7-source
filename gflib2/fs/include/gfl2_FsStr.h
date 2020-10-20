#if !defined(GFL2_FSSTR_H_INCLUDED)
#define GFL2_FSSTR_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsStr.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   ファイルシステムで扱う文字列
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


#if   defined(GF_PLATFORM_CTR)
  #define GFL_FS_STR_SPRINTF(buf,bufSize,mes,...) sprintf(buf,mes,__VA_ARGS__)
#elif defined(GF_PLATFORM_WIN32)
  #define GFL_FS_STR_SPRINTF(buf,bufSize,mes,...) sprintf_s(buf,bufSize,mes,__VA_ARGS__)
#endif  // GF_PLATFORM_


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


//==============================================================================
/**
 @class     Str
 @brief     ファイルシステムで扱う文字列
 */
//==============================================================================
class Str
{
  GFL_FORBID_COPY_AND_ASSIGN(Str);

  friend class DeviceBase;
  friend class Win32Device;
  friend class CtrRomDevice;
  friend class CtrHioDevice;
  friend class CtrSdmcDevice;
  friend class CtrSdmcPathDevice;
  friend class FileCoreBase;
  friend class Win32FileCore;
  friend class CtrFileCore;
  friend class CtrRomFileCore;
  friend class CtrHioFileCore;
  friend class FileBase;
  friend class File;
  friend class ArcFile;
  friend class AsyncFileManager;
  friend class Win32Func;
  friend class Win32Util;
  friend class CtrHioUtil;

private:
  //! @name  friendからアクセスする
  //@{

  //! @brief  名前を収めたバッファを作成する
  //! @param[in]  heap  ヒープ。
  //! @param[out] outNameLen  outNameに入れた名前の長さ。終端の'\0'は含まない長さ。
  //! @param[out] outName     名前を入れたバッファを受け取るポインタ。不要になったらGflHeapFreeMemoryで解放して下さい。
  //!                         終端は'\0'にして入れます。
  //! @param[in]  inName      名前。終端は'\0'になっていること。
  //! @note  例：inName  = "rom:";
  //!            outName = "rom:";  outPathLen = 4;
  static Result CreateName(gfl2::heap::HeapBase* heap, u32* outNameLen, char** outName, const char* inName);

  //! @brief  ファイル名を収めたバッファを作成する
  //! @param[in]  heap  ヒープ。
  //! @param[out] outFileNameLen  outFileNameに入れたファイル名の長さ。終端の'\0'は含まない長さ。
  //! @param[out] outFileName     ファイル名を入れたバッファを受け取るポインタ。不要になったらGflHeapFreeMemoryで解放して下さい。
  //!                             終端は'\0'にして入れます。区切りは/にして入れます。
  //! @param[in]  inPrefixName    名前の最初に追加する文字列。追加する文字列がないときはNULLでもよい。
  //!                             終端は'\0'になっていること。
  //!                             区切りは\でも/でもよいが、ディレクトリ名を足したいなら終端は\か/にしておくこと。
  //! @param[in]  inFileName      ファイル名。終端は'\0'になっていること。区切りは\でも/でもよい。
  //! @note  例：inFileNmae  = "git_gflib2\\fs\\Sample\\data\\read_data.txt";
  //!            inFileNmae  = "git_gflib2/fs/Sample/data/read_data.txt";
  //!            outFileName = "git_gflib2/fs/Sample/data/read_data.txt";
  static Result CreateFileName(gfl2::heap::HeapBase* heap, u32* outFileNameLen, char** outFileName, const char* inPrefixName, const char* inFileName);

  //! @brief  ファイル名が同じかチェックする
  //! @param[in] fileName_a  ファイル名。終端は'\0'になっていること。区切りは/でなければならない。
  //! @param[in] fileName_b  ファイル名。終端は'\0'になっていること。区切りは\でも/でもよい。
  //! @return  同じならtrueを返す
  static bool CheckFileNameIsSame(const char* fileName_a, const char* fileName_b);
  
  //! @brief  ファイル名が同じかチェックする
  //! @param[in] fileName_a  ファイル名。終端は'\0'になっていること。区切りは\でも/でもよい。
  //! @param[in] fileName_b  ファイル名。終端は'\0'になっていること。区切りは\でも/でもよい。
  //! @return  同じならtrueを返す
  static bool CheckFileNameIsSameStrictly(const char* fileName_a, const char* fileName_b);

  //! @brief  パスを収めたバッファを作成する
  //! @param[in]  heap  ヒープ。
  //! @param[out] outPathLen  outPathに入れたパスの長さ。終端の'\0'は含まない長さ。
  //! @param[out] outPath     パスを入れたバッファを受け取るポインタ。不要になったらGflHeapFreeMemoryで解放して下さい。
  //!                         終端は'\0'にして入れます。区切りは/にして入れます。末尾に/があるようにします。
  //! @param[in]  inPath      パス。終端は'\0'になっていること。区切りは\でも/でもよい。末尾に\や/があってもなくてもよい。
  //! @note  例：inPath  = "C:\\home\\niji";
  //!            inPath  = "C:\\home\\niji\\";
  //!            inPath  = "C:/home/niji";
  //!            inPath  = "C:/home/niji/";
  //!            outPath = "C:/home/niji/";  outPathLen = 13;
  static Result CreatePath(gfl2::heap::HeapBase* heap, u32* outPathLen, char** outPath, const char* inPath);

  //! @brief  フルパスファイル名を得る
  //! @param[out] fullNameLen      fullNameに入れたフルパスファイル名の長さ。終端の'\0'は含まない長さ。
  //! @param[out] fullName         呼び出し元であらかじめメモリを確保しておいたバッファ。ここにフルパスファイル名を入れる。
  //!                              終端は'\0'にして入れます。区切りは/にして入れます。
  //! @param[in]  fullNameBufSize  fullNameに確保した長さ。終端の'\0'も含めた長さ。fullNameのバイト数。
  //! @param[in]  archiveNameLen   アーカイブ名の長さ。終端の'\0'は含まない長さ。archiveNameがNULLのときは0で。
  //! @param[in]  archiveName      アーカイブ名。終端は'\0'になっていること。NULLでもよい。
  //! @param[in]  pathLen          パスの長さ。終端の'\0'は含まない長さ。pathがNULLのときは0で。
  //! @param[in]  path             パス。\ではなく/で区切っていること。末尾に'/'があること。終端は'\0'になっていること。NULLでもよい。
  //! @param[in]  fileName         ファイル名。終端は'\0'になっていること。区切りは\でも/でもよい。
  //! @note  例１：archiveNameLen = 0;  archiveName = NULL;
  //               pathLen = 13;  path = "C:/home/niji/";
  //               fileName = "git_gflib2\\fs\\Sample\\data\\read_data.txt";
  //               fullName = "C:/home/niji/git_gflib2/fs/Sample/data/read_data.txt";
  //         例２：archiveNameLen = "4";  archiveName = "rom:";
  //               pathLen = 6;  path = "/data/";
  //               fileName = "fs\\sample\\read_data.txt";
  //               fullName = "rom:/data/fs/sample/read_data.txt";
  static Result GetFullPathFileName(
      u32* fullNameLen, char* fullName, u32 fullNameBufSize,
      u32 archiveNameLen, const char* archiveName,
      u32 pathLen, const char* path,
      const char* fileName
  );
 
  //! @brief  文字列の長さ(バイト数)を得る(strlen)
  //! @param[in] str  文字列。終端は'\0'になっていること。NULLでもよい。
  //! @return  strの長さ(バイト数)。終端の'\0'は含まない長さ。
  //!          strがNULLのときもしくはstrが""のときは、0。
  static u32 GetStrLen(const char* str);

  //! @brief  文字列をコピーする(strcpy)
  //! @param[out]  dst        コピー先。終端を'\0'にして返す。
  //! @param[in]   dstBufSize dstに確保した長さ。終端の'\0'も含めた長さ。
  //! @param[in]   src        コピー元。終端は'\0'になっていること。
  static Result CopySafely(char* dst, u32 dstBufSize, const char* src);

  //! @brief  指定した文字数だけファイル名をコピーする。コピーの仕方にはルールがある。
  //! @param[out]  dst        コピー先。終端を'\0'にして返す。区切りの\や/は特に変更せずそのまま。srcがNULLのときはdst[0]='\0'にして返す。
  //! @param[in]   dstBufSize dstに確保した長さ。終端の'\0'も含めた長さ。
  //! @param[in]   src        コピー元。終端は'\0'になっていること。NULLでもよい。
  //!                         以下のどのような書き方のファイル名でもいい。
  //!                             aaa\bbb\ccc.ddd
  //!                             aaa/bbb/ccc.ddd
  //!                             ccc.ddd
  //!                             aaa/bbb/ccc
  //!                             ccc
  //!                         コピーの仕方のルールは以下の通り。上から優先。
  //!                             (1) 全部dstBufSizeにおさまるなら、全部コピーする。
  //!                             (2) 拡張子をなくせばdstBufSizeにおさまるなら、拡張子が切れるようなコピーの仕方をする。
  //!                             (3) 拡張子をなくした(.もなくす)「パスなしファイル名」がdstBufSizeにおさまるなら、おさまる分だけパスの後半を含んでコピーする(拡張子はなし(.もなし))
  //!                             (4) パスをなくしてコピーする。aaa/bbb/ccc.dddだったら、ccc.dddになるがファイル名の後半が切れる。
  static void SetFileNameInBuf(char* dst, u32 dstBufSize, const char* src);

  //! @brief  パスの長さを得る
  //! @param[in]  fileName    ファイル名。\ではなく/で区切っていること。終端は'\0'になっていること。
  //!                         ディレクトリ名のときは、末尾に'/'があること。
  //!                         次のことは許可する。
  //!                             ・/が複数続いている(一番手前の/だけ数える)
  //!                             ・.や..がある
  //!                             ・アーカイブ名がない
  //!                             ・相対パス
  //! @param[in]  slashCount  何個目の/で区切るか。0始まり。有効な/だけを数えています。
  //! @return  0のときslashCountで指定したところでパスを区切れない。
  //!          0より大のときslashCountで指定したところで区切ったときのパスの長さを返す。
  //! @note  例
  //!        fileName   = aaa:/bbb/ccc/../ddd/./eee//fff/ggg///hhh/iii.jjj   (ファイル名の例)
  //!        fileName   = aaa:/bbb/ccc/../ddd/./eee//fff/ggg///hhh/          (ディレクトリ名の例)
  //!        fileName   =     /bbb/ccc/../ddd/./eee//fff/ggg///hhh/../       (アーカイブ名がないときの例)
  //!        fileName   =      bbb/ccc/../ddd/./eee//fff/ggg///hhh/./iii.jjj (相対パスのときの例)
  //!        slashCount           0   1      2     3    4   5     6
  //!        戻り値               8   11     18    24   28  32    38
  //!        slashCountが7以上のとき0を返す。
  //!        slashCountが6以下のとき0を返す。
  static u32 GetPathLenBySlash(const char* fileName, u32 slashCount);

  //@}

private:
  //! @brief  文字列をコピーする(strcpy)
  //! @param[out]  dst        コピー先。終端を'\0'にして返す。
  //! @param[in]   dstBufSize dstに確保した長さ。終端の'\0'も含めた長さ。
  //! @param[in]   src        コピー元。終端は'\0'になっていること。
  //! @note  srcの長さがdstBufSizeよりも小さいこと(srcの長さ+1がdstBufSize以下)が確定しているときに使う。
  static void copy(char* dst, u32 dstBufSize, const char* src);

  //! @brief  指定した文字数だけ文字列をコピーする(strncpy)
  //! @param[out]  dst        コピー先。終端を'\0'にして返す。
  //! @param[in]   dstBufSize dstに確保した長さ。終端の'\0'も含めた長さ。
  //! @param[in]   src        コピー元。終端は'\0'になっていること。
  //! @param[in]   srcCopyLen srcの先頭から長さsrcCopyLenだけコピーする。終端の'\0'は含まない長さ。
  //!                         (1) コピーする長さに終端の'\0'を含めた長さがdstBufSizeより大きいとき・・・(dstBufSize-1)だけコピーしてその後に'\0'を付ける。
  //!                         (2) コピーする長さに終端の'\0'を含めた長さがdstBufSize以下のとき・・・
  //!                             (2.1) srcの長さがsrcCopyLen以上のとき、srcCopyLenだけコピーしてその後に'\0'を付ける。
  //!                             (2.2) srcの長さがsrcCopyLen未満のとき、srcを全部コピーしてその後に'\0'を付ける。
  static void copyByLen(char* dst, u32 dstBufSize, const char* src, u32 srcCopyLen);

  //! @brief  文字列の最後から文字を検索する(strrchr)
  //! @param[in] str  cを検索される文字列。終端は'\0'になっていること。
  //! @param[in] c    strから検索する文字。'\0'も可能です。
  //! @return  見付かったときはその文字のアドレスを返す。
  //!          見付からなかったときはNULLを返す。
  static const char* searchCharReversely(const char* str, char c);

  //! @brief  パス中のバックスラッシュ\をスラッシュ/に置き換える
  //! @param[in,out] path     パス。終端は'\0'になっていること。\を/に置き換えて返す。
  //! @param[in]     pathLen  パスの長さ。終端の'\0'は含まない長さ。
  //! @note  例：[in]  path = "git_gflib2\\fs\\Sample\\data\\read_data.txt";
  //             [out] path = "git_gflib2/fs/Sample/data/read_data.txt";
  static void replaceBackSlashBySlash(char* path, u32 pathLen);

  //! @brief  パスの末尾が「バックスラッシュ\もしくはスラッシュ/」であるか確認し、
  //          末尾を「\もしくは/」としたときのパスの長さを得る。
  //! @param[out] pathLen  何も変更していない引数で与えたままのpathの長さ。終端の'\0'は含まない長さ。
  //! @param[in]  path     パス。終端は'\0'になっていること。区切りは\でも/でもよい。
  //                       末尾に「\もしくは/」があってもなくてもよい。
  //! @return  末尾を「\もしくは/」としたときのpathの長さ。終端の'\0'は含まない長さ。
  //! @note  例：path = "C:/home/niji/";  *pathLen = 13;  戻り値 = 13("C:/home/niji/");  末尾が「\もしくは/」の例。
  //         例：path = "C:\\home\\niji";  *pathLen = 12;  戻り値 = 13("C:\\home\\niji\\");  末尾が「\もしくは/」でないときの例。
  static u32 getLenWithTailSlash(u32* pathLen, const char* path);

  //! @brief  パスの末尾にスラッシュ/を付ける(もともと/になっていたら何もしない)
  //! @param[out]    pathLenOut  この関数を経た後のpathの長さ。終端の'\0'は含まない長さ。
  //! @param[in,out] path        パス。\ではなく/で区切っていること。末尾に'/'があってもなくてもよい。終端は'\0'になっていること。
  //                             末尾が/でない場合は末尾に/を付けて返す。もともと末尾が/になっていたら何も変更せず返す。
  //! @param[in]     pathBufSize pathに確保した長さ。終端の'\0'も含めた長さ。
  //! @param[in]     pathLenIn   この関数に渡したときのpathの長さ。終端の'\0'は含まない長さ。
  static Result addTailSlash(u32* pathLenOut, char* path, u32 pathBufSize, u32 pathLenIn);
};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL2_FSSTR_H_INCLUDED

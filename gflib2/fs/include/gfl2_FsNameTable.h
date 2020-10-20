#if !defined(GFL2_FSNAMETABLE_H_INCLUDED)
#define GFL2_FSNAMETABLE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsNameTable.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.12.11
 @brief   名前テーブル
 */
//==============================================================================


/*
ファイルの中身
  ===========================================================================================
  名前を収めたバイナリデータ。名前の文字コードはutf-8。数値はリトルエンディアン。
  ------------------------------------------------------------------------------
      シグネチャ NTBL(4バイト) Name TaBLe
      バージョン(2バイトu16)
      要素数n(2バイトu16)
      名前[0]の開始バイト位置(4バイトu32)
      名前[1]の開始バイト位置(4バイトu32)
      ...
      名前[n-1]の開始バイト位置(4バイトu32)
      【ここに至るとき4バイトの倍数になっているので、ここではパディングは要らない】
      名前[0]最後は0【4バイトの倍数になっていなかったらパディング(1-3バイト)】
      名前[1]最後は0【4バイトの倍数になっていなかったらパディング(1-3バイト)】
      ...
      名前[n-1]最後は0【4バイトの倍数になっていなかったらパディング(1-3バイト)】
      【ここに至るとき4バイトの倍数になっているので、ここではパディングは要らない】
  ------------------------------------------------------------------------------
  ===========================================================================================
*/


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


// fsのインクルード
#include <fs/include/gfl2_FsToolDefine.h>
#include <fs/include/gfl2_FsDefine.h>
#include <fs/include/gfl2_FsResult.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <fs/include/gfl2_FsPcUtil.h>




// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)
class AsyncFileManager;
GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)




GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


//==============================================================================
/**
 @class     NameTable
 @brief     名前テーブル
 */
//==============================================================================
class NameTable
{
  GFL_FORBID_COPY_AND_ASSIGN(NameTable);

public:
  //! @brief  コンストラクタ
  NameTable(void);
  //! @brief  デストラクタ
  virtual ~NameTable();

  //! @brief  名前を収めたバイナリファイルを読み込み、テーブルを作成する。
  //!         同期版。
  //! @param[in]  fileManager        ファイルマネージャ
  //! @param[in]  fileName           ファイル名。終端は'\0'になっていること。
  //! @param[in]  heapForBuf         ファイルから読み込んだ内容を入れておくバッファを確保するためのヒープ。配列にした名前テーブルの作成にも使用する。
  //! @param[in]  heapForReq         リクエスト終了まで一時的に使用するヒープ。ファイルオープンのためにも使用する。
  //! @param[in]  heapForCompressed  解凍する場合に一時的に使用するヒープ。解凍するときはヒープのポインタを渡し、解凍しないときはNULLを渡して下さい。
  //! @param[in]  hioUseSetting      どこにあるファイルを扱うかの設定
  void SyncCreateTable(
      AsyncFileManager*                    fileManager,
      const char*                          fileName,        
      gfl2::heap::HeapBase*                heapForBuf,      
      gfl2::heap::HeapBase*                heapForReq,      
      gfl2::heap::HeapBase*                heapForCompressed,
      AsyncFileManager::HioUseSetting::Tag hioUseSetting
  );
  
  //! @brief  名前を収めたバイナリデータから、テーブルを作成する。
  //! @param[in] heap  配列にした名前テーブルの作成に使用する。
  //! @param[in] buf   名前を収めたバイナリデータ。
  //!                  所有権をこのクラスに渡すので、DestroyTableにてGflHeapSafeFreeMemoryで破棄される。
  void CreateTable(gfl2::heap::HeapBase* heap, void* buf);

  //! @brief  テーブルを破棄する。
  void DestroyTable(void);

  //! @brief  配列にした名前テーブルを得る
  char** GetNameTable(void) const { return m_nameTable; }
  //! @brief  配列にした名前テーブルの要素数を得る
  size_t       GetNameNum(void) const { return m_nameNum; }

private:
  //! @brief  名前を収めたバイナリデータから、テーブルを作成する。
  void createTable(gfl2::heap::HeapBase* heap, void* buf);
  //! @brief  テーブルを破棄する。
  void destroyTable(void);

private:
  static const u32 FILE_ALIGN = 4;

  void*   m_buf;
  char**  m_nameTable;
  size_t  m_nameNum;
};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL2_FSNAMETABLE_H_INCLUDED


#if !defined(__ARCSRC_ACCESSOR_H__)
#define __ARCSRC_ACCESSOR_H__
#pragma once


//=============================================================================
/**
 * @brief  「garcのソースを記録したファイル」にアクセスする
 * @file   arcsrc_Accessor.h
 * @author kawada_koji
 * @date   2014.12.12
 */
//=============================================================================


/*
「garcのソースを記録したファイル」の中身
===========================================================================================
名前を収めたバイナリデータ。名前の文字コードはutf-8。数値はリトルエンディアン。
------------------------------------------------------------------------------

シグネチャ GASC(4バイト) GArcSourCe
バージョン(2バイトu16)
【2バイトのパディング】

データの数n(4バイトu32)(言語違いのデータがあるときは言語違いをまとめて1個というふうにカウントしている)

datId=0のデータ情報の開始位置(4バイトu32)
datId=1のデータ情報の開始位置(4バイトu32)
...
datId=n-1のデータ情報の開始位置(4バイトu32)

作業ディレクトリ名(最後は0)
【4バイトの倍数になっていなかったらパディング(1-3バイト)】

datId=0のデータ情報
    言語ビット(4バイトu32)
    言語DEFのファイル情報の開始位置(4バイトu32)
    言語JPNのファイル情報の開始位置(4バイトu32)
    ...
    言語PRTのファイル情報の開始位置(4バイトu32)
    【ビットの立っている言語だけファイル情報がある】
datId=1のデータ情報
...
datId=n-1のデータ情報

datId=0の言語DEFのファイル情報
    garcファイル内でのデータの位置(4バイトu32)
    ファイル名(最後は0)
    【4バイトの倍数になっていなかったらパディング(1-3バイト)】
datId=0の言語JPNのファイル情報
...
datId=0の言語PRTのファイル情報

datId=1の言語DEFのファイル情報
datId=1の言語JPNのファイル情報
...
datId=1の言語PRTのファイル情報

datId=n-1の言語DEFのファイル情報
datId=n-1の言語JPNのファイル情報
...
datId=n-1の言語PRTのファイル情報

------------------------------------------------------------------------------
===========================================================================================
*/


#include "arcsrc_ToolDefine.h"


namespace arcsrc
{


class Accessor
{
public:
  Accessor(void);
  virtual ~Accessor();

  //! @brief  「garcのソースを記録したファイル」を読み込んだバッファを設定する
  //! @param[in] buf  このAccessorインスタンスが生存中は、呼び出し元でbufを保持していて下さい。
  void SetBuf(const void* buf);

  //! @brief  データの数を得る(言語違いのデータがあるときは言語違いをまとめて1個というふうにカウントしている)
  //! @return  エラーのときは0を返す
  size_t GetDataCount(void) const;

  //! @brief  作業ディレクトリ名を得る
  //! @return  「garcファイルを生成するexe」を実行したときのカレントディレクトリ名。終端は'\0'になっている。
  //! @return  エラーのときはNULLを返す
  const char* GetWorkDirName(void) const;

  //! @brief  garcファイル内でのデータの位置(garcファイルの先頭から何バイト目か)を得る
  //! @return  エラーのときは0を返す
  size_t GetDataPosInArcFile(ARCDATID datId, ToolDefine::ArcLangType::Tag lang) const;

  //! @brief  ファイル名を得る
  //! @return  「garcファイルを生成するexe」に渡したファイル名。終端は'\0'になっている。
  //! @return  エラーのときはNULLを返す
  const char* GetFileName(ARCDATID datId, ToolDefine::ArcLangType::Tag lang) const;

private:
  //! @brief  データ情報の開始位置を得る
  //! @return  エラーのときは0を返す
  size_t getDataInfoPos(ARCDATID datId) const;

  //! @brief  ファイル情報の開始位置を得る
  //! @return  エラーのときは0を返す
  size_t getFileInfoPos(ARCDATID datId, ToolDefine::ArcLangType::Tag lang) const;

private:
  const void* m_buf;

  size_t      m_dataNum;
  size_t      m_tablePosOfDataInfoPos;  // 「データ情報の開始位置」のテーブルの位置
  const char* m_workDirName;
};


}  // arcsrc


#endif // __ARCSRC_ACCESSOR_H__

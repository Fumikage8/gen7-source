#if !defined(GFL2_FSRESULT_H_INCLUDED)
#define GFL2_FSRESULT_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsResult.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   ファイルシステムにおける結果
 */
//==============================================================================


#if   defined(GF_PLATFORM_CTR)
#include <nn.h>
#endif  // GF_PLATFORM_CTR


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


//==============================================================================
/**
 @class     Result
 @brief     ファイルシステムにおける結果
 */
//==============================================================================
class Result
{
  // GFL_FORBID_COPY_AND_ASSIGNなし、デストラクタなし

  friend class Str;
  friend class System;
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
  friend class Win32Func;
  friend class Util;
  friend class Win32Util;
  friend class CtrHioUtil;
  friend class FileThread;
  friend class OtherThread;
  friend class AsyncFileManager;

private:
  //! @name  friendからアクセスする
  //@{

  //! @brief  結果のビットを立てた値
  class Bit
  {
  public:
    enum Tag
    {
      SUCCESS                                  = 0,        //!< 正常
    };

    enum Tag0
    {
      // エラー  // 値を変えることがあるので、順番不変のつもりでの記述はしないで下さい。
      FS_ERROR                                 = 1 <<  0,  //!< その他エラー
      MOUNT_ALREADY                            = 1 <<  1,  //!< 既にマウントされている
      FILE_OPEN_FAIL                           = 1 <<  2,  //!< ファイルオープン失敗
      FILE_NAME_LEN_TOO_LONG                   = 1 <<  3,  //!< ファイル名の長さが長過ぎる
      ALIGN_INCORRECT                          = 1 <<  4,  //!< アラインメントの値が間違っている
      DUMMY_0                                  = 1 <<  5,  //!< 欠番
      HEAP_ALLOC_FAIL                          = 1 <<  6,  //!< ヒープからのメモリ確保エラー
      BUF_SIZE_TOO_SMALL                       = 1 <<  7,  //!< 渡してもらったバッファのサイズが足りない
      FILE_WRITE_FAIL                          = 1 <<  8,  //!< ファイル書き込み失敗
      FILE_OPEN_ALREADY                        = 1 <<  9,  //!< 既にファイルがオープンされている
      FILE_NOT_OPEN                            = 1 << 10,  //!< ファイルがオープンされていない
      FILE_SIGNATURE_INCORRECT                 = 1 << 11,  //!< ファイルのシグネチャが間違っている
      FILE_VERSION_TOO_OLD                     = 1 << 12,  //!< ファイルのバージョンが古過ぎる
      ARC_FILE_ACCESSOR_CREATE_FAIL            = 1 << 13,  //!< ArcFileAccessorの生成失敗
      ARC_FILE_BLOCK_DATA_CREATE_FAIL          = 1 << 14,  //!< ArcFileのブロックデータの生成失敗
      ARC_FILE_ACCESS_INFO_TABLE_CREATE_FAIL   = 1 << 15,  //!< ArcFileの索引テーブルの生成失敗
      DUMMY_1                                  = 1 << 16,  //!< 欠番
      ARC_FILE_ARCDATID_INCORRECT              = 1 << 17,  //!< ArcFileのARCDATID不正
      FILE_NAME_NOT_KEEP                       = 1 << 18,  //!< ファイル名を保持していない
      MANAGER_ALREADY_EXIST                    = 1 << 19,  //!< 既にマネージャがある
      DEVICE_NOT_MATCH                         = 1 << 20,  //!< デバイスの指定が一致していない
      FILE_READ_FAIL                           = 1 << 21,  //!< ファイル読み込み失敗
      FILE_FLUSH_FAIL                          = 1 << 22,  //!< ファイルキャッシュをデバイスに書き戻すのに失敗
      FILE_GET_SIZE_FAIL                       = 1 << 23,  //!< ファイルサイズ取得に失敗
      FILE_TELL_FAIL                           = 1 << 24,  //!< ファイルのアクセス位置の取得に失敗
      FILE_SEEK_FAIL                           = 1 << 25,  //!< ファイルのアクセス位置の移動に失敗
      FILE_CORE_NOT_EXIST                      = 1 << 26,  //!< ファイル核がない
      FILE_CORE_CREATE_FAIL                    = 1 << 27,  //!< ファイル核の生成失敗
      ARC_FILE_LANG_NOT_MATCH                  = 1 << 28,  //!< ArcFileの索引テーブルの言語と一致しない
      ASYNC_FILE_MANAGER_NOT_MANAGE_ARC_FILE   = 1 << 29,  //!< AsyncArcFileManagerがそのArcFileを管理していない
      UNCOMPRESSED_DATA_SIZE_ZERO              = 1 << 30,  //!< 圧縮データのサイズが0
    };

    enum Tag1
    {
      // エラー  // 値を変えることがあるので、順番不変のつもりでの記述はしないで下さい。
      UNCOMPRESS_FAIL                          = 1 <<  0,  //!< 圧縮データの展開に失敗
      WIDE_CHAR_TO_MULTI_BYTE_CHAR_FAIL        = 1 <<  1,  //!< ワイド文字からマルチバイト文字への変換に失敗
      MULTI_BYTE_CHAR_TO_WIDE_CHAR_FAIL        = 1 <<  2,  //!< マルチバイト文字からワイド文字への変換に失敗
      CURRENT_DIRECTORY_SET_FAIL               = 1 <<  3,  //!< カレントディレクトリの設定に失敗
      UNMOUNT_FAIL                             = 1 <<  4,  //!< アンマウントに失敗
      ARCSRC_DEVICE_NOT_EXIST                  = 1 <<  5,  //!< ArcSrcデバイスが存在しない
      HIO_INITIALIZE_FAIL                      = 1 <<  6,  //!< CTRのHIOの初期化処理に失敗
      HIO_TERMINATE_FAIL                       = 1 <<  7,  //!< CTRのHIOの終了処理に失敗
      HIO_GET_DAEMON_STATUS_FAIL               = 1 <<  8,  //!< CTRのHIOのホスト側デーモンの起動状態の取得に失敗
      CURRENT_DIRECTORY_GET_FAIL               = 1 <<  9,  //!< カレントディレクトリの取得に失敗
      FILE_CLOSE_FAIL                          = 1 << 10,  //!< ファイルクローズ失敗
      ENVIRONMENT_VARIABLE_GET_FAIL            = 1 << 11,  //!< 環境変数の取得に失敗
      HIO_NOT_ENABLE                           = 1 << 12,  //!< CTRのHIOが有効でない
      ARC_FILE_NAME_TABLE_ALREADY_EXIST        = 1 << 13,  //!< garcファイルの名前テーブルが既に設定されている
      MOUNT_FAIL                               = 1 << 14,  //!< マウントに失敗
      SDMC_NOT_ENABLE                          = 1 << 15,  //!< CTRのSDカード(デバッグ用)が有効でない
      DEVICE_NOT_MOUNT                         = 1 << 16,  //!< デバイスがマウントされていない      
      FILE_WRITE_DIRECTORY_CREATE_NOT_ENABLE   = 1 << 17,  //!< 「ファイル書き込み、ディレクトリ作成」が有効でない
      FILE_EXIST_CHECK_FAIL                    = 1 << 18,  //!< ファイルの有無の確認に失敗
    };

    enum
    {
      TAG0_MAX = 30,  // 「1 <<  n」している最大値n。この値nまで含む。
      TAG1_MAX = 18,
    };
  };
  
  //@}

public:
  //! @brief  コンストラクタ
  Result(void);
  
  //! @brief  結果が正常ならtrue、異常ならfalseを返す。
  bool IsSuccess(void) const;

  //! @brief  結果をクリアする
  void Clear(void);

  //! @brief  |=演算子
  Result operator |= (const Result& res)
  {
    this->m_bitFlag[0] |= res.m_bitFlag[0];
    this->m_bitFlag[1] |= res.m_bitFlag[1];
    return *this;
  }
  Result operator |= (Bit::Tag0 bit)
  {
    this->m_bitFlag[0] |= bit;
    return *this;
  }
  Result operator |= (Bit::Tag1 bit)
  {
    this->m_bitFlag[1] |= bit;
    return *this;
  }

  //! @brief  結果を出力する
  void Print(void) const;


#if   defined(GF_PLATFORM_CTR)
  //! @brief  nn::fsのnn::Resultを出力する
  static void PrintNnResult(nn::Result nnRes);
#endif  // GF_PLATFORM_CTR


private:
  u32  m_bitFlag[2];
};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL2_FSRESULT_H_INCLUDED

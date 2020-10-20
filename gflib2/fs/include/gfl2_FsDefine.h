#if !defined(GFL2_FSDEFINE_H_INCLUDED)
#define GFL2_FSDEFINE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsDefine.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   ファイルシステムで使う定義
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


//==============================================================================
//
// デバッグ版だけの機能
//
//==============================================================================
// ファイルシステムのデバッグ用のログを取る
#define GFL_FS_DEBUG_LOG     (GFL_DEBUG)


// ArcFileオープンクローズリクエスト発行者のリストをつくる
#define GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST    (GFL_DEBUG)

// ArcFileロード間隔チェックを行う
#define GFL_FS_DEBUG_CHECK_ARC_FILE_LOAD_INTERVAL    (GFL_DEBUG)


// 「リクエスト発行」と「リクエスト終了待ち」が対応しているかリストをつくって確認する
#define GFL_FS_DEBUG_MAKE_ADD_REQ_IS_FINISHED_MATCH_CHECK_LIST    (GFL_DEBUG)


// CTRのHIOの使用を許可する
#if defined(GF_PLATFORM_CTR)
  #ifdef NN_SWITCH_ENABLE_HOST_IO
    #define GFL_FS_HIO_ENABLE  (GFL_DEBUG)  // これの外側でもGF_PLATFORM_CTRで括って下さい。ソースが読み易くなるので。
                                            // CTRのHIOのプログラムコードが有効か無効かを切り替える定義。
  #else
    #define GFL_FS_HIO_ENABLE  (0)
  #endif
#elif defined(GF_PLATFORM_WIN32)
  #define GFL_FS_HIO_ENABLE  (0)
#endif  // GF_PLATFORM_


// CTRのSDカード(デバッグ用)の使用を許可する
#if defined(GF_PLATFORM_CTR)
  #define GFL_FS_SDMC_ENABLE  (GFL_DEBUG)  // これの外側でもGF_PLATFORM_CTRで括って下さい。ソースが読み易くなるので。
                                           // CTRのSDカード(デバッグ用)のプログラムコードが有効か無効かを切り替える定義。
#elif defined(GF_PLATFORM_WIN32)
  #define GFL_FS_SDMC_ENABLE  (0)
#endif  // GF_PLATFORM_


// 開発中専用デバイスDirectSdmcの使用を許可する
#define GFL_FS_DIRECT_SDMC_DEVICE_USE (GFL_DEBUG)  // Win32ではPCが、CTRではSDカード(デバッグ用)がデバイスになります。

// DirectSdmcにあるgarcファイルを使う
#define GFL_FS_DIRECT_SDMC_ARC_FILE_ENABLE ((GFL_FS_DIRECT_SDMC_DEVICE_USE)&&(GFL_DEBUG))


// 「garcのソースを記録したファイル」の使用を許可する
#if defined(GF_PLATFORM_CTR)
  #if GFL_FS_HIO_ENABLE  // CTRのHIOが有効でないと、CTR版のArcSrcは有効にならない。
    #define  GFL_FS_GASC_ENABLE  (GFL_DEBUG)
  #else
    #define  GFL_FS_GASC_ENABLE  (0)
  #endif
#elif defined(GF_PLATFORM_WIN32)
  #define  GFL_FS_GASC_ENABLE  (GFL_DEBUG)
#endif  // GF_PLATFORM_




GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


//==============================================================================
/**
 @class     Define
 @brief     ファイルシステムで使う定義
 */
//==============================================================================
class Define
{
  // GFL_FORBID_COPY_AND_ASSIGNなし、デストラクタなし

public:
  //! @brief  デバイスタイプ
  class DeviceType
  {
  public:
    enum Tag
    {
      DEVICE_BASE,               //!< デバイス
      WIN32_DEVICE,              //!< WIN32デバイス
      CTR_ROM_DEVICE,            //!< CTR(3DS)のrom
      CTR_HIO_DEVICE,            //!< CTR(3DS)のHIO
      CTR_SAVE_DATA_DEVICE,      //!< CTR(3DS)のセーブデータ
      CTR_EXT_SAVE_DATA_DEVICE,  //!< CTR(3DS)の拡張セーブデータ
      CTR_SDMC_DEVICE,           //!< CTR(3DS)のSDカード(デバッグ用)
      CTR_EXTRA_ROM_DEVICE,      //!< CTR(3DS)のパッチ側のrom
    };
  };

  //! @brief  ファイルのオープンモード
  class OpenMode
  {
  public:
    enum Tag
    {
      // この順番を利用しているところがあるので、順番を変更しないで下さい。
      READ,    //!< 読み込みモード
      WRITE,   //!< 書き込みモード
      NUM
    };
  };

  //! @brief  ファイルのアクセス位置の基準
  class Origin
  {
  public:
    enum Tag
    {
      // この順番を利用しているところがあるので、順番を変更しないで下さい。
      BEGIN,    //!< ファイルの先頭
      CURRENT,  //!< 現在の位置
      END,      //!< ファイルの末尾
      NUM
    };
  };

  //! @brief  ファイル名の長さ
  static const u32 FILE_NAME_FULL_LEN_MAX = 512;  // 終端の'\0'を含めた長さ。

  //! @brief  無限ループ対策
  static const u32 ENDLESS_LOOP_COUNT_MAX      = 4096;  // ループがこの回数を越えたら止める。
  static const u32 ENDLESS_LOOP_COUNT_MAX_LONG = 2147483647-1;  // ループがこの回数を越えたら止める。

  //! @brief  メモリを確保する際に必要な余分なサイズ
  static const u32 MEM_REQUIRED_EXTRA_SIZE = 128;  // Win32版で必要。
};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL2_FSDEFINE_H_INCLUDED


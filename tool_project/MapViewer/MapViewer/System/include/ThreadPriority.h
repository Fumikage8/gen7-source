#if !defined(__H_THREAD_PRIORITY_H__)
#define __H_THREAD_PRIORITY_H__
#pragma once


//==============================================================================
/**
 @file    thread_priotiry.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.03.12
 @brief   スレッドのプライオリティをまとめて書いておくファイル
 */
//==============================================================================

#include <thread/include/gfl2_Thread.h>

GFL_NAMESPACE_BEGIN(System)


class ThreadPriority
{

public:

  //! @brief  スレッドのプライオリティ
  enum
  {
    // fs
    ROM_ASYNC_FILE_MANAGER_FILE_THREAD_PRIORITY         = gfl2::thread::Thread::THREAD_PRI_NORMAL -1,    //!< ROMファイルマネージャのファイルスレッドのプライオリティ
    ROM_ASYNC_FILE_MANAGER_OTHER_THREAD_PRIORITY        = gfl2::thread::Thread::THREAD_PRI_NORMAL,       //!< ROMファイルマネージャのその他スレッドのプライオリティ

    // cec
    ASYNC_CEC_THREAD_PRIORITY                           = gfl2::thread::Thread::THREAD_PRI_NORMAL +1,    //!< すれ違い通信のスレッドのプライオリティ

    // qr
    ASYNC_QR_ENCODE_THREAD_PRIORITY                     = gfl2::thread::Thread::THREAD_PRI_NORMAL +1,    //!< QRコードライブラリのエンコードスレッドのプライオリティ
    ASYNC_QR_DECODE_THREAD_PRIORITY                     = gfl2::thread::Thread::THREAD_PRI_NORMAL +1,    //!< QRコードライブラリのデコードスレッドのプライオリティ

    // crypto
    ASYNC_AES_CRYPTO_THREAD_PRIORITY                    = gfl2::thread::Thread::THREAD_PRI_NORMAL +1,    //!< クリプトライブラリのAES暗号化復号化スレッドのプライオリティ
    ASYNC_RSA_CRYPTO_THREAD_PRIORITY                    = gfl2::thread::Thread::THREAD_PRI_NORMAL +1,    //!< クリプトライブラリのRSA署名作成検証スレッドのプライオリティ
    ASYNC_SHA256_CRYPTO_THREAD_PRIORITY                 = gfl2::thread::Thread::THREAD_PRI_NORMAL +1,    //!< クリプトライブラリのSHA256ハッシュ計算スレッドのプライオリティ

    // Wifi
    WIFI_CONNECT_THREAD_PRIORITY                        = gfl2::thread::Thread::THREAD_PRI_NORMAL -1,    //!< Wifi接続時のスレッドのプライオリティ
  };

};


GFL_NAMESPACE_END(System)

#endif  // __H_THREAD_PRIORITY_H__


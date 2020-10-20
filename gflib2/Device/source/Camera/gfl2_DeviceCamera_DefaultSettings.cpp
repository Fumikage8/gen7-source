#if GF_PLATFORM_CTR
/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl2_DeviceCamera_DefaultSettings.cpp
 * @brief  カメラシステムのデフォルト設定
 * @author obata_toshihiro
 * @date   2010.12.01
 */
/////////////////////////////////////////////////////////////////////////////////////////
#include <Device/include/Camera/gfl2_DeviceCamera_Types.h>
#include <Device/include/Camera/gfl2_DeviceCamera_System.h>
#include <Device/include/Camera/gfl2_DeviceCamera_DefaultSettings.h>
#include <thread/include/gfl2_Thread.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(device)
GFL_NAMESPACE_BEGIN(camera)



      // カメラを無効にする定義
#define GFL_DEVICE_CAMERA_INVALID_SETTING \
      {\
        false,\
        gfl2::device::camera::DEVICE_ID_IN,\
        gfl2::device::camera::CAPTURE_SIZE_CTR_BOTTOM_LCD,\
        320, 240,\
        512, 256,\
        gfl2::device::camera::OUTPUT_R8G8B8,\
        gfl2::device::camera::BLOCK_8_BY_8,\
        0xff,\
        true,\
        true,\
        160, 0, 640, 480,\
        gfl2::device::camera::WHITE_BALANCE_NORMAL,\
        true,\
        160, 0, 640, 480,\
        0,\
        gfl2::device::camera::FLIP_NONE,\
        gfl2::device::camera::FRAME_RATE_30,\
        gfl2::device::camera::PHOTO_MODE_NORMAL,\
        gfl2::device::camera::EFFECT_NONE,\
        gfl2::device::camera::CONTRAST_NORMAL,\
        gfl2::device::camera::LENS_CORRECTION_NORMAL,\
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief シングルカメラ( 外側 )
       */
      //---------------------------------------------------------------------------------
      const System::SetupParam DefaultSettings_Single = 
      {
        NULL, // システムが使用するヒープ
        NULL, // システムが使用するデバイスヒープ

        // カメラごとの設定
        {
          // CAMERA_1
          {
            true,                                           // 以降のパラメータが有効かどうか
            gfl2::device::camera::DEVICE_ID_OUT_L,           // どのカメラなのか？
            gfl2::device::camera::CAPTURE_SIZE_CTR_TOP_LCD,  // カメラがキャプチャする画像の解像度
            400, 240,                                       // キャプチャ画像(YUV)のトリミング後の幅, 高さ
            512, 256,                                       // 出力画像(RGB)の幅・高さ
            gfl2::device::camera::OUTPUT_R8G8B8,             // 出力ピクセルのRGBフォーマット
            gfl2::device::camera::BLOCK_8_BY_8,              // 出力画像のRGBデータの並び順
            0xff,                                           // 出力画像(RGB)のアルファ値 ( R8G8B8A8 or R5G5B5A1 の場合のみ有効 )
            true,                                           // ノイズフィルターの有無
            true,                                           // 自動露出機能の有無
            0, 0, 640, 480,                                 // 自動露出の計算に使用する矩形領域 左上x座標, 左上y座標, 幅, 高さ
            gfl2::device::camera::WHITE_BALANCE_NORMAL,      // ホワイトバランスの指定
            true,                                           // オートホワイトバランスの有無
            0, 0, 640, 480,                                 // オートホワイトバランスの計算に使用する矩形領域 左上x座標, 左上y座標, 幅, 高さ
            0,                                              // 鮮明度 [-4, 5]
            gfl2::device::camera::FLIP_NONE,                 // 反転処理
            gfl2::device::camera::FRAME_RATE_30,             // フレームレート
            gfl2::device::camera::PHOTO_MODE_NORMAL,         // 撮影モード
            gfl2::device::camera::EFFECT_NONE,               // エフェクト
            gfl2::device::camera::CONTRAST_NORMAL,           // コントラスト
            gfl2::device::camera::LENS_CORRECTION_NORMAL,    // レンズ補正の程度
          },

          // CAMERA_2
          GFL_DEVICE_CAMERA_INVALID_SETTING,
        },
        
        // 各カメラ制御スレッドの優先度
        {
          gfl2::thread::Thread::THREAD_PRI_NORMAL,
          gfl2::thread::Thread::THREAD_PRI_NORMAL,
        },
        
        // キャプチャ画像処理スレッドの優先度
        gfl2::thread::Thread::THREAD_PRI_NORMAL,
      };

      //---------------------------------------------------------------------------------
      /**
       * @brief シングルカメラ( 内側 )
       */
      //---------------------------------------------------------------------------------
      const System::SetupParam DefaultSettings_SingleInside = 
      {
        NULL, // システムが使用するヒープ
        NULL, // システムが使用するデバイスヒープ

        // カメラごとの設定
        {
          // CAMERA_1
          {
            true,                                           // 以降のパラメータが有効かどうか
            gfl2::device::camera::DEVICE_ID_IN,              // どのカメラなのか？
            gfl2::device::camera::CAPTURE_SIZE_CTR_TOP_LCD,  // カメラがキャプチャする画像の解像度
            400, 240,                                       // キャプチャ画像(YUV)のトリミング後の幅, 高さ
            512, 256,                                       // 出力画像(RGB)の幅・高さ
            gfl2::device::camera::OUTPUT_R8G8B8,             // 出力ピクセルのRGBフォーマット
            gfl2::device::camera::BLOCK_8_BY_8,              // 出力画像のRGBデータの並び順
            0xff,                                           // 出力画像(RGB)のアルファ値 ( R8G8B8A8 or R5G5B5A1 の場合のみ有効 )
            true,                                           // ノイズフィルターの有無
            true,                                           // 自動露出機能の有無
            0, 0, 640, 480,                                 // 自動露出の計算に使用する矩形領域 左上x座標, 左上y座標, 幅, 高さ
            gfl2::device::camera::WHITE_BALANCE_NORMAL,      // ホワイトバランスの指定
            true,                                           // オートホワイトバランスの有無
            0, 0, 640, 480,                                 // オートホワイトバランスの計算に使用する矩形領域 左上x座標, 左上y座標, 幅, 高さ
            0,                                              // 鮮明度 [-4, 5]
            gfl2::device::camera::FLIP_NONE,                 // 反転処理
            gfl2::device::camera::FRAME_RATE_30,             // フレームレート
            gfl2::device::camera::PHOTO_MODE_NORMAL,         // 撮影モード
            gfl2::device::camera::EFFECT_NONE,               // エフェクト
            gfl2::device::camera::CONTRAST_NORMAL,           // コントラスト
            gfl2::device::camera::LENS_CORRECTION_NORMAL,    // レンズ補正の程度
          },

          // CAMERA_2
          GFL_DEVICE_CAMERA_INVALID_SETTING,
        },
        
        // 各カメラ制御スレッドの優先度
        {
          gfl2::thread::Thread::THREAD_PRI_NORMAL,
          gfl2::thread::Thread::THREAD_PRI_NORMAL,
        },
        
        // キャプチャ画像処理スレッドの優先度
        gfl2::thread::Thread::THREAD_PRI_NORMAL,
      };




      //---------------------------------------------------------------------------------
      /**
       * @brief ステレオカメラ (外側カメラを2つ同時使用 ==> 立体視)
       */
      //---------------------------------------------------------------------------------
      const System::SetupParam DefaultSettings_Stereo = 
      {
        NULL, // システムが使用するヒープ
        NULL, // システムが使用するデバイスヒープ

        // カメラごとの設定
        {
          // CAMERA_1
          {
            true,                                           // 以降のパラメータが有効かどうか
            gfl2::device::camera::DEVICE_ID_OUT_L,           // どのカメラなのか？
            gfl2::device::camera::CAPTURE_SIZE_CTR_TOP_LCD,  // カメラがキャプチャする画像の解像度
            400, 240,                                       // キャプチャ画像(YUV)のトリミング後の幅, 高さ
            512, 256,                                       // 出力画像(RGB)の幅・高さ
            gfl2::device::camera::OUTPUT_R8G8B8,             // 出力ピクセルのRGBフォーマット
            gfl2::device::camera::BLOCK_8_BY_8,              // 出力画像のRGBデータの並び順
            0xff,                                           // 出力画像(RGB)のアルファ値 ( R8G8B8A8 or R5G5B5A1 の場合のみ有効 )
            false,                                          // ノイズフィルターの有無
            true,                                           // 自動露出機能の有無
            0, 0, 480, 480,                                 // 自動露出の計算に使用する矩形領域 左上x座標, 左上y座標, 幅, 高さ
            gfl2::device::camera::WHITE_BALANCE_NORMAL,      // ホワイトバランスの指定
            true,                                           // オートホワイトバランスの有無
            0, 0, 480, 480,                                 // オートホワイトバランスの計算に使用する矩形領域 左上x座標, 左上y座標, 幅, 高さ
            0,                                              // 鮮明度 [-4, 5]
            gfl2::device::camera::FLIP_NONE,                 // 反転処理
            gfl2::device::camera::FRAME_RATE_15,             // フレームレート
            gfl2::device::camera::PHOTO_MODE_NORMAL,         // 撮影モード
            gfl2::device::camera::EFFECT_NONE,               // エフェクト
            gfl2::device::camera::CONTRAST_NORMAL,           // コントラスト
            gfl2::device::camera::LENS_CORRECTION_NORMAL,    // レンズ補正の程度
          },

          // CAMERA_2
          {
            true,                                           // 以降のパラメータが有効かどうか
            gfl2::device::camera::DEVICE_ID_OUT_R,           // どのカメラなのか？
            gfl2::device::camera::CAPTURE_SIZE_CTR_TOP_LCD,  // カメラがキャプチャする画像の解像度
            400, 240,                                       // キャプチャ画像(YUV)のトリミング後の幅, 高さ
            512, 256,                                       // 出力画像(RGB)の幅・高さ
            gfl2::device::camera::OUTPUT_R8G8B8,             // 出力ピクセルのRGBフォーマット
            gfl2::device::camera::BLOCK_8_BY_8,              // 出力画像のRGBデータの並び順
            0xff,                                           // 出力画像(RGB)のアルファ値 ( R8G8B8A8 or R5G5B5A1 の場合のみ有効 )
            false,                                          // ノイズフィルターの有無
            true,                                           // 自動露出機能の有無
            160, 0, 480, 480,                               // 自動露出の計算に使用する矩形領域 左上x座標, 左上y座標, 幅, 高さ
            gfl2::device::camera::WHITE_BALANCE_NORMAL,      // ホワイトバランスの指定
            true,                                           // オートホワイトバランスの有無
            160, 0, 480, 480,                               // オートホワイトバランスの計算に使用する矩形領域 左上x座標, 左上y座標, 幅, 高さ
            0,                                              // 鮮明度 [-4, 5]
            gfl2::device::camera::FLIP_NONE,                 // 反転処理
            gfl2::device::camera::FRAME_RATE_15,             // フレームレート
            gfl2::device::camera::PHOTO_MODE_NORMAL,         // 撮影モード
            gfl2::device::camera::EFFECT_NONE,               // エフェクト
            gfl2::device::camera::CONTRAST_NORMAL,           // コントラスト
            gfl2::device::camera::LENS_CORRECTION_NORMAL,    // レンズ補正の程度
          },
        },
        
        // 各カメラ制御スレッドの優先度
        {
          gfl2::thread::Thread::THREAD_PRI_NORMAL,
          gfl2::thread::Thread::THREAD_PRI_NORMAL,
        },
        
        // キャプチャ画像処理スレッドの優先度
        gfl2::thread::Thread::THREAD_PRI_NORMAL,
      };

      //---------------------------------------------------------------------------------
      /**
       * @brief マルチカメラ (内側カメラと外側カメラを同時使用)
       */
      //---------------------------------------------------------------------------------
      const System::SetupParam DefaultSettings_Multi = 
      {
        NULL, // システムが使用するヒープ
        NULL, // システムが使用するデバイスヒープ

        // カメラごとの設定
        {
          // CAMERA_1
          {
            true,                                           // 以降のパラメータが有効かどうか
            gfl2::device::camera::DEVICE_ID_OUT_L,           // どのカメラなのか？
            gfl2::device::camera::CAPTURE_SIZE_CTR_TOP_LCD,  // カメラがキャプチャする画像の解像度
            400, 240,                                       // キャプチャ画像(YUV)のトリミング後の幅, 高さ
            512, 256,                                       // 出力画像(RGB)の幅・高さ
            gfl2::device::camera::OUTPUT_R8G8B8,             // 出力ピクセルのRGBフォーマット
            gfl2::device::camera::BLOCK_8_BY_8,              // 出力画像のRGBデータの並び順
            0xff,                                           // 出力画像(RGB)のアルファ値 ( R8G8B8A8 or R5G5B5A1 の場合のみ有効 )
            true,                                           // ノイズフィルターの有無
            true,                                           // 自動露出機能の有無
            0, 0, 640, 480,                                 // 自動露出の計算に使用する矩形領域 左上x座標, 左上y座標, 幅, 高さ
            gfl2::device::camera::WHITE_BALANCE_NORMAL,      // ホワイトバランスの指定
            true,                                           // オートホワイトバランスの有無
            0, 0, 640, 480,                                 // オートホワイトバランスの計算に使用する矩形領域 左上x座標, 左上y座標, 幅, 高さ
            0,                                              // 鮮明度 [-4, 5]
            gfl2::device::camera::FLIP_NONE,                 // 反転処理
            gfl2::device::camera::FRAME_RATE_30,             // フレームレート
            gfl2::device::camera::PHOTO_MODE_NORMAL,         // 撮影モード
            gfl2::device::camera::EFFECT_NONE,               // エフェクト
            gfl2::device::camera::CONTRAST_NORMAL,           // コントラスト
            gfl2::device::camera::LENS_CORRECTION_NORMAL,    // レンズ補正の程度
          },

          // CAMERA_2
          {
            true,                                           // 以降のパラメータが有効かどうか
            gfl2::device::camera::DEVICE_ID_IN,              // どのカメラなのか？
            gfl2::device::camera::CAPTURE_SIZE_CTR_BOTTOM_LCD, // カメラがキャプチャする画像の解像度
            320, 240,                                       // キャプチャ画像(YUV)のトリミング後の幅, 高さ
            512, 256,                                       // 出力画像(RGB)の幅・高さ
            gfl2::device::camera::OUTPUT_R8G8B8,             // 出力ピクセルのRGBフォーマット
            gfl2::device::camera::BLOCK_8_BY_8,              // 出力画像のRGBデータの並び順
            0xff,                                           // 出力画像(RGB)のアルファ値 ( R8G8B8A8 or R5G5B5A1 の場合のみ有効 )
            true,                                           // ノイズフィルターの有無
            true,                                           // 自動露出機能の有無
            160, 0, 640, 480,                               // 自動露出の計算に使用する矩形領域 左上x座標, 左上y座標, 幅, 高さ
            gfl2::device::camera::WHITE_BALANCE_NORMAL,      // ホワイトバランスの指定
            true,                                           // オートホワイトバランスの有無
            160, 0, 640, 480,                               // オートホワイトバランスの計算に使用する矩形領域 左上x座標, 左上y座標, 幅, 高さ
            0,                                              // 鮮明度 [-4, 5]
            gfl2::device::camera::FLIP_NONE,                 // 反転処理
            gfl2::device::camera::FRAME_RATE_30,             // フレームレート
            gfl2::device::camera::PHOTO_MODE_NORMAL,         // 撮影モード
            gfl2::device::camera::EFFECT_NONE,               // エフェクト
            gfl2::device::camera::CONTRAST_NORMAL,           // コントラスト
            gfl2::device::camera::LENS_CORRECTION_NORMAL,    // レンズ補正の程度
          },
        },
        
        // 各カメラ制御スレッドの優先度
        {
          gfl2::thread::Thread::THREAD_PRI_NORMAL,
          gfl2::thread::Thread::THREAD_PRI_NORMAL,
        },
        
        // キャプチャ画像処理スレッドの優先度
        gfl2::thread::Thread::THREAD_PRI_NORMAL,
      };



#if 0
      const System::SetupParam TestSettings = 
      {
        NULL, 
        NULL,

        {
          // CAMERA_1
          {
            true,
            gfl2::device::camera::DEVICE_ID_IN,
            gfl2::device::camera::CAPTURE_SIZE_CTR_TOP_LCD,
            400, 240,
            512, 256,
            gfl2::device::camera::OUTPUT_R8G8B8,
            gfl2::device::camera::BLOCK_8_BY_8,
            0xff,
            false,
            true,
            0, 0, 480, 480,
            gfl2::device::camera::WHITE_BALANCE_NORMAL,
            true,
            0, 0, 480, 480,
            0,
            gfl2::device::camera::FLIP_NONE,
            gfl2::device::camera::FRAME_RATE_15,
            gfl2::device::camera::PHOTO_MODE_NORMAL,
            gfl2::device::camera::EFFECT_NONE,
            gfl2::device::camera::CONTRAST_NORMAL,
            gfl2::device::camera::LENS_CORRECTION_NORMAL,
          },

          // CAMERA_2
          {
            true,
            gfl2::device::camera::DEVICE_ID_OUT_L,
            gfl2::device::camera::CAPTURE_SIZE_VGA,
            256, 256, 
            256, 256, 
            gfl2::device::camera::OUTPUT_R8G8B8,
            gfl2::device::camera::BLOCK_8_BY_8,
            0xff,
            false,
            true,
            160, 0, 480, 480,
            gfl2::device::camera::WHITE_BALANCE_NORMAL,
            true,
            160, 0, 480, 480,
            0,
            gfl2::device::camera::FLIP_NONE,
            gfl2::device::camera::FRAME_RATE_15,
            gfl2::device::camera::PHOTO_MODE_NORMAL,
            gfl2::device::camera::EFFECT_NONE,
            gfl2::device::camera::CONTRAST_NORMAL,
            gfl2::device::camera::LENS_CORRECTION_NORMAL,
          },
        },
        
        // 各カメラ制御スレッドの優先度
        {
          gfl::base::Thread::DefaultPriority,
          gfl::base::Thread::DefaultPriority,
        },
        
        // キャプチャ画像処理スレッドの優先度
        gfl::base::Thread::DefaultPriority,
      };
#endif


GFL_NAMESPACE_END(camera)
GFL_NAMESPACE_END(device)
GFL_NAMESPACE_END(gfl2)
#endif // GF_PLATFORM_CTR

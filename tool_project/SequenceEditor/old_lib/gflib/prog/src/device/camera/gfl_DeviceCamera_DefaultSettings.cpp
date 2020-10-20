/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl_DeviceCamera_DefaultSettings.cpp
 * @brief  カメラシステムのデフォルト設定
 * @author obata_toshihiro
 * @date   2010.12.01
 */
/////////////////////////////////////////////////////////////////////////////////////////
#include <device/camera/gfl_DeviceCamera_Types.h>
#include <device/camera/gfl_DeviceCamera_System.h>
#include <device/camera/gfl_DeviceCamera_DefaultSettings.h>

namespace gfl {
  namespace device { 
    namespace camera { 



      // カメラを無効にする定義
#define GFL_DEVICE_CAMERA_INVALID_SETTING \
      {\
        false,\
        gfl::device::camera::DEVICE_ID_IN,\
        gfl::device::camera::CAPTURE_SIZE_CTR_BOTTOM_LCD,\
        320, 240,\
        512, 256,\
        gfl::device::camera::OUTPUT_R8G8B8,\
        gfl::device::camera::BLOCK_8_BY_8,\
        0xff,\
        true,\
        true,\
        160, 0, 640, 480,\
        gfl::device::camera::WHITE_BALANCE_NORMAL,\
        true,\
        160, 0, 640, 480,\
        0,\
        gfl::device::camera::FLIP_NONE,\
        gfl::device::camera::FRAME_RATE_30,\
        gfl::device::camera::PHOTO_MODE_NORMAL,\
        gfl::device::camera::EFFECT_NONE,\
        gfl::device::camera::CONTRAST_NORMAL,\
        gfl::device::camera::LENS_CORRECTION_NORMAL,\
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
            gfl::device::camera::DEVICE_ID_OUT_L,           // どのカメラなのか？
            gfl::device::camera::CAPTURE_SIZE_CTR_TOP_LCD,  // カメラがキャプチャする画像の解像度
            400, 240,                                       // キャプチャ画像(YUV)のトリミング後の幅, 高さ
            512, 256,                                       // 出力画像(RGB)の幅・高さ
            gfl::device::camera::OUTPUT_R8G8B8,             // 出力ピクセルのRGBフォーマット
            gfl::device::camera::BLOCK_8_BY_8,              // 出力画像のRGBデータの並び順
            0xff,                                           // 出力画像(RGB)のアルファ値 ( R8G8B8A8 or R5G5B5A1 の場合のみ有効 )
            true,                                           // ノイズフィルターの有無
            true,                                           // 自動露出機能の有無
            0, 0, 640, 480,                                 // 自動露出の計算に使用する矩形領域 左上x座標, 左上y座標, 幅, 高さ
            gfl::device::camera::WHITE_BALANCE_NORMAL,      // ホワイトバランスの指定
            true,                                           // オートホワイトバランスの有無
            0, 0, 640, 480,                                 // オートホワイトバランスの計算に使用する矩形領域 左上x座標, 左上y座標, 幅, 高さ
            0,                                              // 鮮明度 [-4, 5]
            gfl::device::camera::FLIP_NONE,                 // 反転処理
            gfl::device::camera::FRAME_RATE_30,             // フレームレート
            gfl::device::camera::PHOTO_MODE_NORMAL,         // 撮影モード
            gfl::device::camera::EFFECT_NONE,               // エフェクト
            gfl::device::camera::CONTRAST_NORMAL,           // コントラスト
            gfl::device::camera::LENS_CORRECTION_NORMAL,    // レンズ補正の程度
          },

          // CAMERA_2
          GFL_DEVICE_CAMERA_INVALID_SETTING,
        },
        
        // 各カメラ制御スレッドの優先度
        {
          gfl::base::Thread::DefaultPriority,
          gfl::base::Thread::DefaultPriority,
        },
        
        // キャプチャ画像処理スレッドの優先度
        gfl::base::Thread::DefaultPriority,
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
            gfl::device::camera::DEVICE_ID_IN,              // どのカメラなのか？
            gfl::device::camera::CAPTURE_SIZE_CTR_TOP_LCD,  // カメラがキャプチャする画像の解像度
            400, 240,                                       // キャプチャ画像(YUV)のトリミング後の幅, 高さ
            512, 256,                                       // 出力画像(RGB)の幅・高さ
            gfl::device::camera::OUTPUT_R8G8B8,             // 出力ピクセルのRGBフォーマット
            gfl::device::camera::BLOCK_8_BY_8,              // 出力画像のRGBデータの並び順
            0xff,                                           // 出力画像(RGB)のアルファ値 ( R8G8B8A8 or R5G5B5A1 の場合のみ有効 )
            true,                                           // ノイズフィルターの有無
            true,                                           // 自動露出機能の有無
            0, 0, 640, 480,                                 // 自動露出の計算に使用する矩形領域 左上x座標, 左上y座標, 幅, 高さ
            gfl::device::camera::WHITE_BALANCE_NORMAL,      // ホワイトバランスの指定
            true,                                           // オートホワイトバランスの有無
            0, 0, 640, 480,                                 // オートホワイトバランスの計算に使用する矩形領域 左上x座標, 左上y座標, 幅, 高さ
            0,                                              // 鮮明度 [-4, 5]
            gfl::device::camera::FLIP_NONE,                 // 反転処理
            gfl::device::camera::FRAME_RATE_30,             // フレームレート
            gfl::device::camera::PHOTO_MODE_NORMAL,         // 撮影モード
            gfl::device::camera::EFFECT_NONE,               // エフェクト
            gfl::device::camera::CONTRAST_NORMAL,           // コントラスト
            gfl::device::camera::LENS_CORRECTION_NORMAL,    // レンズ補正の程度
          },

          // CAMERA_2
          GFL_DEVICE_CAMERA_INVALID_SETTING,
        },
        
        // 各カメラ制御スレッドの優先度
        {
          gfl::base::Thread::DefaultPriority,
          gfl::base::Thread::DefaultPriority,
        },
        
        // キャプチャ画像処理スレッドの優先度
        gfl::base::Thread::DefaultPriority,
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
            gfl::device::camera::DEVICE_ID_OUT_L,           // どのカメラなのか？
            gfl::device::camera::CAPTURE_SIZE_CTR_TOP_LCD,  // カメラがキャプチャする画像の解像度
            400, 240,                                       // キャプチャ画像(YUV)のトリミング後の幅, 高さ
            512, 256,                                       // 出力画像(RGB)の幅・高さ
            gfl::device::camera::OUTPUT_R8G8B8,             // 出力ピクセルのRGBフォーマット
            gfl::device::camera::BLOCK_8_BY_8,              // 出力画像のRGBデータの並び順
            0xff,                                           // 出力画像(RGB)のアルファ値 ( R8G8B8A8 or R5G5B5A1 の場合のみ有効 )
            false,                                          // ノイズフィルターの有無
            true,                                           // 自動露出機能の有無
            0, 0, 480, 480,                                 // 自動露出の計算に使用する矩形領域 左上x座標, 左上y座標, 幅, 高さ
            gfl::device::camera::WHITE_BALANCE_NORMAL,      // ホワイトバランスの指定
            true,                                           // オートホワイトバランスの有無
            0, 0, 480, 480,                                 // オートホワイトバランスの計算に使用する矩形領域 左上x座標, 左上y座標, 幅, 高さ
            0,                                              // 鮮明度 [-4, 5]
            gfl::device::camera::FLIP_NONE,                 // 反転処理
            gfl::device::camera::FRAME_RATE_15,             // フレームレート
            gfl::device::camera::PHOTO_MODE_NORMAL,         // 撮影モード
            gfl::device::camera::EFFECT_NONE,               // エフェクト
            gfl::device::camera::CONTRAST_NORMAL,           // コントラスト
            gfl::device::camera::LENS_CORRECTION_NORMAL,    // レンズ補正の程度
          },

          // CAMERA_2
          {
            true,                                           // 以降のパラメータが有効かどうか
            gfl::device::camera::DEVICE_ID_OUT_R,           // どのカメラなのか？
            gfl::device::camera::CAPTURE_SIZE_CTR_TOP_LCD,  // カメラがキャプチャする画像の解像度
            400, 240,                                       // キャプチャ画像(YUV)のトリミング後の幅, 高さ
            512, 256,                                       // 出力画像(RGB)の幅・高さ
            gfl::device::camera::OUTPUT_R8G8B8,             // 出力ピクセルのRGBフォーマット
            gfl::device::camera::BLOCK_8_BY_8,              // 出力画像のRGBデータの並び順
            0xff,                                           // 出力画像(RGB)のアルファ値 ( R8G8B8A8 or R5G5B5A1 の場合のみ有効 )
            false,                                          // ノイズフィルターの有無
            true,                                           // 自動露出機能の有無
            160, 0, 480, 480,                               // 自動露出の計算に使用する矩形領域 左上x座標, 左上y座標, 幅, 高さ
            gfl::device::camera::WHITE_BALANCE_NORMAL,      // ホワイトバランスの指定
            true,                                           // オートホワイトバランスの有無
            160, 0, 480, 480,                               // オートホワイトバランスの計算に使用する矩形領域 左上x座標, 左上y座標, 幅, 高さ
            0,                                              // 鮮明度 [-4, 5]
            gfl::device::camera::FLIP_NONE,                 // 反転処理
            gfl::device::camera::FRAME_RATE_15,             // フレームレート
            gfl::device::camera::PHOTO_MODE_NORMAL,         // 撮影モード
            gfl::device::camera::EFFECT_NONE,               // エフェクト
            gfl::device::camera::CONTRAST_NORMAL,           // コントラスト
            gfl::device::camera::LENS_CORRECTION_NORMAL,    // レンズ補正の程度
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
            gfl::device::camera::DEVICE_ID_OUT_L,           // どのカメラなのか？
            gfl::device::camera::CAPTURE_SIZE_CTR_TOP_LCD,  // カメラがキャプチャする画像の解像度
            400, 240,                                       // キャプチャ画像(YUV)のトリミング後の幅, 高さ
            512, 256,                                       // 出力画像(RGB)の幅・高さ
            gfl::device::camera::OUTPUT_R8G8B8,             // 出力ピクセルのRGBフォーマット
            gfl::device::camera::BLOCK_8_BY_8,              // 出力画像のRGBデータの並び順
            0xff,                                           // 出力画像(RGB)のアルファ値 ( R8G8B8A8 or R5G5B5A1 の場合のみ有効 )
            true,                                           // ノイズフィルターの有無
            true,                                           // 自動露出機能の有無
            0, 0, 640, 480,                                 // 自動露出の計算に使用する矩形領域 左上x座標, 左上y座標, 幅, 高さ
            gfl::device::camera::WHITE_BALANCE_NORMAL,      // ホワイトバランスの指定
            true,                                           // オートホワイトバランスの有無
            0, 0, 640, 480,                                 // オートホワイトバランスの計算に使用する矩形領域 左上x座標, 左上y座標, 幅, 高さ
            0,                                              // 鮮明度 [-4, 5]
            gfl::device::camera::FLIP_NONE,                 // 反転処理
            gfl::device::camera::FRAME_RATE_30,             // フレームレート
            gfl::device::camera::PHOTO_MODE_NORMAL,         // 撮影モード
            gfl::device::camera::EFFECT_NONE,               // エフェクト
            gfl::device::camera::CONTRAST_NORMAL,           // コントラスト
            gfl::device::camera::LENS_CORRECTION_NORMAL,    // レンズ補正の程度
          },

          // CAMERA_2
          {
            true,                                           // 以降のパラメータが有効かどうか
            gfl::device::camera::DEVICE_ID_IN,              // どのカメラなのか？
            gfl::device::camera::CAPTURE_SIZE_CTR_BOTTOM_LCD, // カメラがキャプチャする画像の解像度
            320, 240,                                       // キャプチャ画像(YUV)のトリミング後の幅, 高さ
            512, 256,                                       // 出力画像(RGB)の幅・高さ
            gfl::device::camera::OUTPUT_R8G8B8,             // 出力ピクセルのRGBフォーマット
            gfl::device::camera::BLOCK_8_BY_8,              // 出力画像のRGBデータの並び順
            0xff,                                           // 出力画像(RGB)のアルファ値 ( R8G8B8A8 or R5G5B5A1 の場合のみ有効 )
            true,                                           // ノイズフィルターの有無
            true,                                           // 自動露出機能の有無
            160, 0, 640, 480,                               // 自動露出の計算に使用する矩形領域 左上x座標, 左上y座標, 幅, 高さ
            gfl::device::camera::WHITE_BALANCE_NORMAL,      // ホワイトバランスの指定
            true,                                           // オートホワイトバランスの有無
            160, 0, 640, 480,                               // オートホワイトバランスの計算に使用する矩形領域 左上x座標, 左上y座標, 幅, 高さ
            0,                                              // 鮮明度 [-4, 5]
            gfl::device::camera::FLIP_NONE,                 // 反転処理
            gfl::device::camera::FRAME_RATE_30,             // フレームレート
            gfl::device::camera::PHOTO_MODE_NORMAL,         // 撮影モード
            gfl::device::camera::EFFECT_NONE,               // エフェクト
            gfl::device::camera::CONTRAST_NORMAL,           // コントラスト
            gfl::device::camera::LENS_CORRECTION_NORMAL,    // レンズ補正の程度
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



#if 0
      const System::SetupParam TestSettings = 
      {
        NULL, 
        NULL,

        {
          // CAMERA_1
          {
            true,
            gfl::device::camera::DEVICE_ID_IN,
            gfl::device::camera::CAPTURE_SIZE_CTR_TOP_LCD,
            400, 240,
            512, 256,
            gfl::device::camera::OUTPUT_R8G8B8,
            gfl::device::camera::BLOCK_8_BY_8,
            0xff,
            false,
            true,
            0, 0, 480, 480,
            gfl::device::camera::WHITE_BALANCE_NORMAL,
            true,
            0, 0, 480, 480,
            0,
            gfl::device::camera::FLIP_NONE,
            gfl::device::camera::FRAME_RATE_15,
            gfl::device::camera::PHOTO_MODE_NORMAL,
            gfl::device::camera::EFFECT_NONE,
            gfl::device::camera::CONTRAST_NORMAL,
            gfl::device::camera::LENS_CORRECTION_NORMAL,
          },

          // CAMERA_2
          {
            true,
            gfl::device::camera::DEVICE_ID_OUT_L,
            gfl::device::camera::CAPTURE_SIZE_VGA,
            256, 256, 
            256, 256, 
            gfl::device::camera::OUTPUT_R8G8B8,
            gfl::device::camera::BLOCK_8_BY_8,
            0xff,
            false,
            true,
            160, 0, 480, 480,
            gfl::device::camera::WHITE_BALANCE_NORMAL,
            true,
            160, 0, 480, 480,
            0,
            gfl::device::camera::FLIP_NONE,
            gfl::device::camera::FRAME_RATE_15,
            gfl::device::camera::PHOTO_MODE_NORMAL,
            gfl::device::camera::EFFECT_NONE,
            gfl::device::camera::CONTRAST_NORMAL,
            gfl::device::camera::LENS_CORRECTION_NORMAL,
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


    } // namespace camera
  } // namespace device
} // namespace gfl

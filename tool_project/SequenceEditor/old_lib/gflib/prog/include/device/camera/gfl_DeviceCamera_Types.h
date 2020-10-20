#ifndef __GFL_DEVICECAMERA_TYPES_H__
#define __GFL_DEVICECAMERA_TYPES_H__
/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl_DeviceCamera_Types.h
 * @brief  カメラシステムで共通の型, 定数
 * @author obata_toshihiro
 * @date   2010.11.19
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <nn.h>
#include <nn/y2r.h>

namespace gfl {
  namespace device { 
    namespace camera { 


      /**
       * @brief デバイス識別子
       * @note どのカメラか？を指定する.
       */
      enum DeviceID {
        DEVICE_ID_IN,    // 内側カメラ
        DEVICE_ID_OUT_L, // 外側カメラ(L)
        DEVICE_ID_OUT_R, // 外側カメラ(R) 
        DEVICE_ID_NUM,
      }; 

      /**
       * @brief カメラ識別子
       * @note 2機同時に使用できるので, カメラIDで識別する.  
       */
      enum CameraID {
        CAMERA_1,
        CAMERA_2,
        CAMERA_NUM,
      }; 

      /**
       * @brief キャプチャ画像の解像度
       */
      enum CaptureSize {
        CAPTURE_SIZE_VGA            = nn::camera::SIZE_VGA,            // 640 x 480
        CAPTURE_SIZE_QVGA           = nn::camera::SIZE_QVGA,           // 320 x 240
        CAPTURE_SIZE_QQVGA          = nn::camera::SIZE_QQVGA,          // 160 x 120
        CAPTURE_SIZE_CIF            = nn::camera::SIZE_CIF,            // 352 x 288
        CAPTURE_SIZE_QCIF           = nn::camera::SIZE_QCIF,           // 176 x 144
        CAPTURE_SIZE_DS_LCD         = nn::camera::SIZE_DS_LCD,         // 256 x 192
        CAPTURE_SIZE_DS_LCDx4       = nn::camera::SIZE_DS_LCDx4,       // 512 x 384
        CAPTURE_SIZE_CTR_TOP_LCD    = nn::camera::SIZE_CTR_TOP_LCD,    // 400 x 240
        // alias
        CAPTURE_SIZE_CTR_BOTTOM_LCD = nn::camera::SIZE_CTR_BOTTOM_LCD, // 320 x 240
      }; 

      /**
       * @brief 出力画像のフォーマット
       */
      enum OutputFormat {
        OUTPUT_R8G8B8A8 = nn::y2r::OUTPUT_RGB_32,     // 32bit
        OUTPUT_R8G8B8   = nn::y2r::OUTPUT_RGB_24,     // 24bit
        OUTPUT_R5G5B5A1 = nn::y2r::OUTPUT_RGB_16_555, // 16bit
        OUTPUT_R5G6B5   = nn::y2r::OUTPUT_RGB_16_565, // 16bit
      };
      static const u32 OUTPUT_BIT_PER_PIXEL[] =
      {
        32,
        24,
        16,
        16,
      };

      /**
       * @brief RGB データの並び順
       */
      enum BlockAlignment {
        BLOCK_LINE   = nn::y2r::BLOCK_LINE,   // 水平ラインフォーマット
        BLOCK_8_BY_8 = nn::y2r::BLOCK_8_BY_8, // 8x8ブロックフォーマット
      };

      /**
       * @brief ホワイトバランス
       */
      enum WhiteBalance {
        WHITE_BALANCE_AUTO  = nn::camera::WHITE_BALANCE_AUTO, 
        WHITE_BALANCE_3200K = nn::camera::WHITE_BALANCE_3200K,
        WHITE_BALANCE_4150K = nn::camera::WHITE_BALANCE_4150K,
        WHITE_BALANCE_5200K = nn::camera::WHITE_BALANCE_5200K,
        WHITE_BALANCE_6000K = nn::camera::WHITE_BALANCE_6000K,
        WHITE_BALANCE_7000K = nn::camera::WHITE_BALANCE_7000K, 
        WHITE_BALANCE_MAX   = nn::camera::WHITE_BALANCE_MAX, 
        // alias
        WHITE_BALANCE_NORMAL   = nn::camera::WHITE_BALANCE_NORMAL,   // オートホワイトバランス
        WHITE_BALANCE_TUNGSTEN = nn::camera::WHITE_BALANCE_TUNGSTEN, // タングステン光(白熱電球)
        WHITE_BALANCE_WHITE_FLUORESCENT_LIGHT = nn::camera::WHITE_BALANCE_WHITE_FLUORESCENT_LIGHT, // 白色蛍光灯
        WHITE_BALANCE_DAYLIGHT = nn::camera::WHITE_BALANCE_DAYLIGHT, // 太陽光
        WHITE_BALANCE_CLOUDY   = nn::camera::WHITE_BALANCE_CLOUDY,   // くもり
        WHITE_BALANCE_HORIZON  = nn::camera::WHITE_BALANCE_HORIZON,  // 夕焼け
        WHITE_BALANCE_SHADE    = nn::camera::WHITE_BALANCE_SHADE,    // 日陰
      };

      /**
       * @brief 反転処理
       */
      enum Flip {
        FLIP_NONE       = nn::camera::FLIP_NONE,       // 反転処理を行わない
        FLIP_HORIZONTAL = nn::camera::FLIP_HORIZONTAL, // 左右反転を行う
        FLIP_VERTICAL   = nn::camera::FLIP_VERTICAL,   // 上下反転を行う
        FLIP_REVERSE    = nn::camera::FLIP_REVERSE,    // 180度回転を行う
      }; 

      /**
       * @brief フレームレート
       */
      enum FrameRate { 
        FRAME_RATE_5        = nn::camera::FRAME_RATE_5,        // 5fps固定
        FRAME_RATE_8_5      = nn::camera::FRAME_RATE_8_5,      // 8.5fps固定
        FRAME_RATE_10       = nn::camera::FRAME_RATE_10,       // 10fps固定
        FRAME_RATE_15       = nn::camera::FRAME_RATE_15,       // 15fps固定
        FRAME_RATE_20       = nn::camera::FRAME_RATE_20,       // 20fps固定
        FRAME_RATE_30       = nn::camera::FRAME_RATE_30,       // 30fps固定
        FRAME_RATE_15_TO_2  = nn::camera::FRAME_RATE_15_TO_2,  // 明るさに応じて15fpsから2fpsの間で自動的に変化
        FRAME_RATE_15_TO_5  = nn::camera::FRAME_RATE_15_TO_5,  // 明るさに応じて15fpsから5fpsの間で自動的に変化
        FRAME_RATE_15_TO_10 = nn::camera::FRAME_RATE_15_TO_10, // 明るさに応じて15fpsから10fpsの間で自動的に変化
        FRAME_RATE_20_TO_5  = nn::camera::FRAME_RATE_20_TO_5,  // 明るさに応じて20fpsから5fpsの間で自動的に変化
        FRAME_RATE_20_TO_10 = nn::camera::FRAME_RATE_20_TO_10, // 明るさに応じて20fpsから10fpsの間で自動的に変化
        FRAME_RATE_30_TO_5  = nn::camera::FRAME_RATE_30_TO_5,  // 明るさに応じて30fpsから5fpsの間で自動的に変化
        FRAME_RATE_30_TO_10 = nn::camera::FRAME_RATE_30_TO_10, // 明るさに応じて30fpsから10fpsの間で自動的に変化
      }; 

      /**
       * @brief 撮影モード
       */
      enum PhotoMode {
        PHOTO_MODE_NORMAL    = nn::camera::PHOTO_MODE_NORMAL,    // 補正なし
        PHOTO_MODE_PORTRAIT  = nn::camera::PHOTO_MODE_PORTRAIT,  // 人物
        PHOTO_MODE_LANDSCAPE = nn::camera::PHOTO_MODE_LANDSCAPE, // 風景
        PHOTO_MODE_NIGHTVIEW = nn::camera::PHOTO_MODE_NIGHTVIEW, // 暗視
        PHOTO_MODE_LETTER    = nn::camera::PHOTO_MODE_LETTER,    // 文字撮影 (QRコード)
      }; 

      /**
       * @brief エフェクト
       */
      enum Effect {
        EFFECT_NONE          = nn::camera::EFFECT_NONE,           // エフェクトなし
        EFFECT_MONO          = nn::camera::EFFECT_MONO,           // モノクロ調
        EFFECT_SEPIA         = nn::camera::EFFECT_SEPIA,          // セピア調
        EFFECT_NEGATIVE      = nn::camera::EFFECT_NEGATIVE,       // ネガポジ反転
        EFFECT_NEGAFILM      = nn::camera::EFFECT_NEGAFILM,       // フィルム調のネガポジ反転(NEGATIVEに対して、UとVの順番が入れ替わっている)
        EFFECT_SEPIA01       = nn::camera::EFFECT_SEPIA01,        // セピア調
      };

      /**
       * @brief コントラスト
       */
      enum Contrast {
        CONTRAST_PATTERN_01 = nn::camera::CONTRAST_PATTERN_01,    // コントラストのパターンNo.1
        CONTRAST_PATTERN_02 = nn::camera::CONTRAST_PATTERN_02,    // コントラストのパターンNo.2
        CONTRAST_PATTERN_03 = nn::camera::CONTRAST_PATTERN_03,    // コントラストのパターンNo.3
        CONTRAST_PATTERN_04 = nn::camera::CONTRAST_PATTERN_04,    // コントラストのパターンNo.4
        CONTRAST_PATTERN_05 = nn::camera::CONTRAST_PATTERN_05,    // コントラストのパターンNo.5
        CONTRAST_PATTERN_06 = nn::camera::CONTRAST_PATTERN_06,    // コントラストのパターンNo.6
        CONTRAST_PATTERN_07 = nn::camera::CONTRAST_PATTERN_07,    // コントラストのパターンNo.7
        CONTRAST_PATTERN_08 = nn::camera::CONTRAST_PATTERN_08,    // コントラストのパターンNo.8
        CONTRAST_PATTERN_09 = nn::camera::CONTRAST_PATTERN_09,    // コントラストのパターンNo.9
        CONTRAST_PATTERN_10 = nn::camera::CONTRAST_PATTERN_10,    // コントラストのパターンNo.10
        CONTRAST_PATTERN_11 = nn::camera::CONTRAST_PATTERN_11,    // コントラストのパターンNo.11 
        // alias
        CONTRAST_LOW    = nn::camera::CONTRAST_LOW,    // デフォルトよりもコントラスト比が低くなる設定
        CONTRAST_NORMAL = nn::camera::CONTRAST_NORMAL, // デフォルトの設定
        CONTRAST_HIGH   = nn::camera::CONTRAST_HIGH,   // デフォルトよりもコントラスト比が高くなる設定
      }; 

      /**
       * @brief レンズ補正の程度
       */
      enum LensCorrection {
        LENS_CORRECTION_OFF   = nn::camera::LENS_CORRECTION_OFF,   // レンズ補正をOFFにする
        LENS_CORRECTION_ON_70 = nn::camera::LENS_CORRECTION_ON_70, // 画像の中心と周辺の明るさの比率が70%になるように周辺の明るさを調整する設定
        LENS_CORRECTION_ON_90 = nn::camera::LENS_CORRECTION_ON_90, // 画像の中心と周辺の明るさの比率が90%になるように周辺の明るさを調整する設定
        // alias
        LENS_CORRECTION_DARK   = nn::camera::LENS_CORRECTION_DARK,   // デフォルト設定よりも画像の周辺が暗くなる設定
        LENS_CORRECTION_NORMAL = nn::camera::LENS_CORRECTION_NORMAL, // デフォルト設定
        LENS_CORRECTION_BRIGHT = nn::camera::LENS_CORRECTION_BRIGHT, // デフォルト設定よりも画像の周辺が明るくなる設定
      };

      /**
       * @brief シャッター音
       */
      enum ShutterSoundType {
        SHUTTER_SOUND_TYPE_NORMAL    = nn::camera::SHUTTER_SOUND_TYPE_NORMAL,     // 通常の撮影用シャッター 
        SHUTTER_SOUND_TYPE_MOVIE     = nn::camera::SHUTTER_SOUND_TYPE_MOVIE,      // 動画撮影開始音 
        SHUTTER_SOUND_TYPE_MOVIE_END = nn::camera::SHUTTER_SOUND_TYPE_MOVIE_END,  // 動画撮影終了音 
        SHUTTER_SOUND_TYPE_MAX       = nn::camera::SHUTTER_SOUND_TYPE_MAX                          
      };

      /**
       * @brief エラーのビット
       */
      enum
      {
        ERROR_BIT_NONE                                                   = 0,         // エラーなし

        ERROR_BIT_CAMERA_INITIALIZE_FATAL_ERROR                          = 1 <<  0,   // nn::camera::Initialzeがnn::camera::ResultFatalError
        ERROR_BIT_CAMERA_ACTIVATE_FATAL_ERROR                            = 1 <<  1,   // nn::camera::Activateがnn::camera::ResultFatalError
        ERROR_BIT_CAMERA_GETSUITABLEY2RSTANDARDCOEFFICIENT_FATAL_ERROR   = 1 <<  2,   // nn::camera::GetSuitableY2rStandardCoefficientがnn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETSIZE_FATAL_ERROR                             = 1 <<  3,   // nn::camera::SetSizeがnn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETNOISEFILTER_FATAL_ERROR                      = 1 <<  4,   // nn::camera::SetNoiseFilterがnn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETAUTOEXPOSURE_FATAL_ERROR                     = 1 <<  5,   // nn::camera::SetAutoExposureがnn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETAUTOEXPOSUREWINDOW_FATAL_ERROR               = 1 <<  6,   // nn::camera::SetAutoExposureWindowがnn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETEXPOSURE_FATAL_ERROR                         = 1 <<  7,   // nn::camera::SetExposureがnn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETWHITEBALANCE_FATAL_ERROR                     = 1 <<  8,   // nn::camera::SetWhiteBalanceがnn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETAUTOWHITEBALANCE_FATAL_ERROR                 = 1 <<  9,   // nn::camera::SetAutoWhiteBalanceがnn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETAUTOWHITEBALANCEWINDOW_FATAL_ERROR           = 1 << 10,   // nn::camera::SetAutoWhiteBalanceWindowがnn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETSHARPNESS_FATAL_ERROR                        = 1 << 11,   // nn::camera::SetSharpnessがnn::camera::ResultFatalError
        ERROR_BIT_CAMERA_FLIPIMAGE_FATAL_ERROR                           = 1 << 12,   // nn::camera::FlipImageがnn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETFRAMERATE_FATAL_ERROR                        = 1 << 13,   // nn::camera::SetFrameRateがnn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETPHOTOMODE_FATAL_ERROR                        = 1 << 14,   // nn::camera::SetPhotoModeがnn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETEFFECT_FATAL_ERROR                           = 1 << 15,   // nn::camera::SetEffectがnn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETCONTRAST_FATAL_ERROR                         = 1 << 16,   // nn::camera::SetContrastがnn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETLENSCORRECTION_FATAL_ERROR                   = 1 << 17,   // nn::camera::SetLensCorrectionがnn::camera::ResultFatalError
        ERROR_BIT_CAMERA_PLAYSHUTTERSOUND_FATAL_ERROR                    = 1 << 18,   // nn::camera::PlayShutterSoundがnn::camera::ResultFatalError
        
        ERROR_BIT_Y2R_INITIALIZE_ERROR                                   = 1 << 19,   // nn::y2r::Initializeがfalse
        
#if GFL_DEBUG
        ERROR_BIT_DEBUG_FATAL_ERROR_OCCUR                                = 1 << 20,   // 実行中にデバッグ機能で起こしたエラー。初期化前にデバッグ機能でエラーを起こしたときはERROR_BIT_CAMERA_INITIALIZE_FATAL_ERRORを立てています。
#endif
      };


    } // namespace camera
  } // namespace device
} // namespace gfl
#endif // __GFL_DEVICECAMERA_TYPES_H__

#if GF_PLATFORM_CTR
#if !defined( GFL2_DEVICECAMERA_DEFAULTSETTINGS_H_INCLUDED )
#define GFL2_DEVICECAMERA_DEFAULTSETTINGS_H_INCLUDED
/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl2_DeviceCamera_DefaultSettings.h
 * @brief  カメラシステムのデフォルト設定
 * @author obata_toshihiro
 * @date   2010.12.01
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(device)
GFL_NAMESPACE_BEGIN(camera)

      class System;


#if 0
      // テスト用
      extern const System::SetupParam TestSettings;
#endif

      // シングルカメラ (外側カメラを使用)
      // CAMERA_1 ==> 外側カメラ
      // CAMERA_2 ==> 無効
      extern const System::SetupParam DefaultSettings_Single;

      // シングルカメラ (内側カメラを使用)
      // CAMERA_1 ==> 内側カメラ
      // CAMERA_2 ==> 無効
      extern const System::SetupParam DefaultSettings_SingleInside;

      // ステレオカメラ (外側カメラを2つ同時使用 ==> 立体視)
      // CAMERA_1 ==> 外側カメラ(L)
      // CAMERA_2 ==> 外側カメラ(R)
      extern const System::SetupParam DefaultSettings_Stereo;

      // マルチカメラ (内側カメラと外側カメラを同時使用)
      // CAMERA_1 ==> 外側カメラ(L)
      // CAMERA_2 ==> 内側カメラ
      extern const System::SetupParam DefaultSettings_Multi;


GFL_NAMESPACE_END(camera)
GFL_NAMESPACE_END(device)
GFL_NAMESPACE_END(gfl2)
#endif // GFL2_DEVICECAMERA_DEFAULTSETTINGS_H_INCLUDED
#endif // GF_PLATFORM_CTR

#ifndef __GFL_DEVICECAMERA_DEFAULTSETTINGS_H__
#define __GFL_DEVICECAMERA_DEFAULTSETTINGS_H__
/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl_DeviceCamera_DefaultSettings.h
 * @brief  カメラシステムのデフォルト設定
 * @author obata_toshihiro
 * @date   2010.12.01
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace gfl {
  namespace device { 
    namespace camera { 

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


    } // namespace camera
  } // namespace device
} // namespace gfl
#endif // __GFL_DEVICECAMERA_DEFAULTSETTINGS_H__

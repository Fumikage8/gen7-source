#ifndef __GFL_DEVICECAMERA_DEFAULTSETTINGS_H__
#define __GFL_DEVICECAMERA_DEFAULTSETTINGS_H__
/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl_DeviceCamera_DefaultSettings.h
 * @brief  �J�����V�X�e���̃f�t�H���g�ݒ�
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
      // �e�X�g�p
      extern const System::SetupParam TestSettings;
#endif

      // �V���O���J���� (�O���J�������g�p)
      // CAMERA_1 ==> �O���J����
      // CAMERA_2 ==> ����
      extern const System::SetupParam DefaultSettings_Single;

      // �V���O���J���� (�����J�������g�p)
      // CAMERA_1 ==> �����J����
      // CAMERA_2 ==> ����
      extern const System::SetupParam DefaultSettings_SingleInside;

      // �X�e���I�J���� (�O���J������2�����g�p ==> ���̎�)
      // CAMERA_1 ==> �O���J����(L)
      // CAMERA_2 ==> �O���J����(R)
      extern const System::SetupParam DefaultSettings_Stereo;

      // �}���`�J���� (�����J�����ƊO���J�����𓯎��g�p)
      // CAMERA_1 ==> �O���J����(L)
      // CAMERA_2 ==> �����J����
      extern const System::SetupParam DefaultSettings_Multi;


    } // namespace camera
  } // namespace device
} // namespace gfl
#endif // __GFL_DEVICECAMERA_DEFAULTSETTINGS_H__

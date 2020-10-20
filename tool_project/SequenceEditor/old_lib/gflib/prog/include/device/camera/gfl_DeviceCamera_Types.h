#ifndef __GFL_DEVICECAMERA_TYPES_H__
#define __GFL_DEVICECAMERA_TYPES_H__
/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl_DeviceCamera_Types.h
 * @brief  �J�����V�X�e���ŋ��ʂ̌^, �萔
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
       * @brief �f�o�C�X���ʎq
       * @note �ǂ̃J�������H���w�肷��.
       */
      enum DeviceID {
        DEVICE_ID_IN,    // �����J����
        DEVICE_ID_OUT_L, // �O���J����(L)
        DEVICE_ID_OUT_R, // �O���J����(R) 
        DEVICE_ID_NUM,
      }; 

      /**
       * @brief �J�������ʎq
       * @note 2�@�����Ɏg�p�ł���̂�, �J����ID�Ŏ��ʂ���.  
       */
      enum CameraID {
        CAMERA_1,
        CAMERA_2,
        CAMERA_NUM,
      }; 

      /**
       * @brief �L���v�`���摜�̉𑜓x
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
       * @brief �o�͉摜�̃t�H�[�}�b�g
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
       * @brief RGB �f�[�^�̕��я�
       */
      enum BlockAlignment {
        BLOCK_LINE   = nn::y2r::BLOCK_LINE,   // �������C���t�H�[�}�b�g
        BLOCK_8_BY_8 = nn::y2r::BLOCK_8_BY_8, // 8x8�u���b�N�t�H�[�}�b�g
      };

      /**
       * @brief �z���C�g�o�����X
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
        WHITE_BALANCE_NORMAL   = nn::camera::WHITE_BALANCE_NORMAL,   // �I�[�g�z���C�g�o�����X
        WHITE_BALANCE_TUNGSTEN = nn::camera::WHITE_BALANCE_TUNGSTEN, // �^���O�X�e����(���M�d��)
        WHITE_BALANCE_WHITE_FLUORESCENT_LIGHT = nn::camera::WHITE_BALANCE_WHITE_FLUORESCENT_LIGHT, // ���F�u����
        WHITE_BALANCE_DAYLIGHT = nn::camera::WHITE_BALANCE_DAYLIGHT, // ���z��
        WHITE_BALANCE_CLOUDY   = nn::camera::WHITE_BALANCE_CLOUDY,   // ������
        WHITE_BALANCE_HORIZON  = nn::camera::WHITE_BALANCE_HORIZON,  // �[�Ă�
        WHITE_BALANCE_SHADE    = nn::camera::WHITE_BALANCE_SHADE,    // ���A
      };

      /**
       * @brief ���]����
       */
      enum Flip {
        FLIP_NONE       = nn::camera::FLIP_NONE,       // ���]�������s��Ȃ�
        FLIP_HORIZONTAL = nn::camera::FLIP_HORIZONTAL, // ���E���]���s��
        FLIP_VERTICAL   = nn::camera::FLIP_VERTICAL,   // �㉺���]���s��
        FLIP_REVERSE    = nn::camera::FLIP_REVERSE,    // 180�x��]���s��
      }; 

      /**
       * @brief �t���[�����[�g
       */
      enum FrameRate { 
        FRAME_RATE_5        = nn::camera::FRAME_RATE_5,        // 5fps�Œ�
        FRAME_RATE_8_5      = nn::camera::FRAME_RATE_8_5,      // 8.5fps�Œ�
        FRAME_RATE_10       = nn::camera::FRAME_RATE_10,       // 10fps�Œ�
        FRAME_RATE_15       = nn::camera::FRAME_RATE_15,       // 15fps�Œ�
        FRAME_RATE_20       = nn::camera::FRAME_RATE_20,       // 20fps�Œ�
        FRAME_RATE_30       = nn::camera::FRAME_RATE_30,       // 30fps�Œ�
        FRAME_RATE_15_TO_2  = nn::camera::FRAME_RATE_15_TO_2,  // ���邳�ɉ�����15fps����2fps�̊ԂŎ����I�ɕω�
        FRAME_RATE_15_TO_5  = nn::camera::FRAME_RATE_15_TO_5,  // ���邳�ɉ�����15fps����5fps�̊ԂŎ����I�ɕω�
        FRAME_RATE_15_TO_10 = nn::camera::FRAME_RATE_15_TO_10, // ���邳�ɉ�����15fps����10fps�̊ԂŎ����I�ɕω�
        FRAME_RATE_20_TO_5  = nn::camera::FRAME_RATE_20_TO_5,  // ���邳�ɉ�����20fps����5fps�̊ԂŎ����I�ɕω�
        FRAME_RATE_20_TO_10 = nn::camera::FRAME_RATE_20_TO_10, // ���邳�ɉ�����20fps����10fps�̊ԂŎ����I�ɕω�
        FRAME_RATE_30_TO_5  = nn::camera::FRAME_RATE_30_TO_5,  // ���邳�ɉ�����30fps����5fps�̊ԂŎ����I�ɕω�
        FRAME_RATE_30_TO_10 = nn::camera::FRAME_RATE_30_TO_10, // ���邳�ɉ�����30fps����10fps�̊ԂŎ����I�ɕω�
      }; 

      /**
       * @brief �B�e���[�h
       */
      enum PhotoMode {
        PHOTO_MODE_NORMAL    = nn::camera::PHOTO_MODE_NORMAL,    // �␳�Ȃ�
        PHOTO_MODE_PORTRAIT  = nn::camera::PHOTO_MODE_PORTRAIT,  // �l��
        PHOTO_MODE_LANDSCAPE = nn::camera::PHOTO_MODE_LANDSCAPE, // ���i
        PHOTO_MODE_NIGHTVIEW = nn::camera::PHOTO_MODE_NIGHTVIEW, // �Î�
        PHOTO_MODE_LETTER    = nn::camera::PHOTO_MODE_LETTER,    // �����B�e (QR�R�[�h)
      }; 

      /**
       * @brief �G�t�F�N�g
       */
      enum Effect {
        EFFECT_NONE          = nn::camera::EFFECT_NONE,           // �G�t�F�N�g�Ȃ�
        EFFECT_MONO          = nn::camera::EFFECT_MONO,           // ���m�N����
        EFFECT_SEPIA         = nn::camera::EFFECT_SEPIA,          // �Z�s�A��
        EFFECT_NEGATIVE      = nn::camera::EFFECT_NEGATIVE,       // �l�K�|�W���]
        EFFECT_NEGAFILM      = nn::camera::EFFECT_NEGAFILM,       // �t�B�������̃l�K�|�W���](NEGATIVE�ɑ΂��āAU��V�̏��Ԃ�����ւ���Ă���)
        EFFECT_SEPIA01       = nn::camera::EFFECT_SEPIA01,        // �Z�s�A��
      };

      /**
       * @brief �R���g���X�g
       */
      enum Contrast {
        CONTRAST_PATTERN_01 = nn::camera::CONTRAST_PATTERN_01,    // �R���g���X�g�̃p�^�[��No.1
        CONTRAST_PATTERN_02 = nn::camera::CONTRAST_PATTERN_02,    // �R���g���X�g�̃p�^�[��No.2
        CONTRAST_PATTERN_03 = nn::camera::CONTRAST_PATTERN_03,    // �R���g���X�g�̃p�^�[��No.3
        CONTRAST_PATTERN_04 = nn::camera::CONTRAST_PATTERN_04,    // �R���g���X�g�̃p�^�[��No.4
        CONTRAST_PATTERN_05 = nn::camera::CONTRAST_PATTERN_05,    // �R���g���X�g�̃p�^�[��No.5
        CONTRAST_PATTERN_06 = nn::camera::CONTRAST_PATTERN_06,    // �R���g���X�g�̃p�^�[��No.6
        CONTRAST_PATTERN_07 = nn::camera::CONTRAST_PATTERN_07,    // �R���g���X�g�̃p�^�[��No.7
        CONTRAST_PATTERN_08 = nn::camera::CONTRAST_PATTERN_08,    // �R���g���X�g�̃p�^�[��No.8
        CONTRAST_PATTERN_09 = nn::camera::CONTRAST_PATTERN_09,    // �R���g���X�g�̃p�^�[��No.9
        CONTRAST_PATTERN_10 = nn::camera::CONTRAST_PATTERN_10,    // �R���g���X�g�̃p�^�[��No.10
        CONTRAST_PATTERN_11 = nn::camera::CONTRAST_PATTERN_11,    // �R���g���X�g�̃p�^�[��No.11 
        // alias
        CONTRAST_LOW    = nn::camera::CONTRAST_LOW,    // �f�t�H���g�����R���g���X�g�䂪�Ⴍ�Ȃ�ݒ�
        CONTRAST_NORMAL = nn::camera::CONTRAST_NORMAL, // �f�t�H���g�̐ݒ�
        CONTRAST_HIGH   = nn::camera::CONTRAST_HIGH,   // �f�t�H���g�����R���g���X�g�䂪�����Ȃ�ݒ�
      }; 

      /**
       * @brief �����Y�␳�̒��x
       */
      enum LensCorrection {
        LENS_CORRECTION_OFF   = nn::camera::LENS_CORRECTION_OFF,   // �����Y�␳��OFF�ɂ���
        LENS_CORRECTION_ON_70 = nn::camera::LENS_CORRECTION_ON_70, // �摜�̒��S�Ǝ��ӂ̖��邳�̔䗦��70%�ɂȂ�悤�Ɏ��ӂ̖��邳�𒲐�����ݒ�
        LENS_CORRECTION_ON_90 = nn::camera::LENS_CORRECTION_ON_90, // �摜�̒��S�Ǝ��ӂ̖��邳�̔䗦��90%�ɂȂ�悤�Ɏ��ӂ̖��邳�𒲐�����ݒ�
        // alias
        LENS_CORRECTION_DARK   = nn::camera::LENS_CORRECTION_DARK,   // �f�t�H���g�ݒ�����摜�̎��ӂ��Â��Ȃ�ݒ�
        LENS_CORRECTION_NORMAL = nn::camera::LENS_CORRECTION_NORMAL, // �f�t�H���g�ݒ�
        LENS_CORRECTION_BRIGHT = nn::camera::LENS_CORRECTION_BRIGHT, // �f�t�H���g�ݒ�����摜�̎��ӂ����邭�Ȃ�ݒ�
      };

      /**
       * @brief �V���b�^�[��
       */
      enum ShutterSoundType {
        SHUTTER_SOUND_TYPE_NORMAL    = nn::camera::SHUTTER_SOUND_TYPE_NORMAL,     // �ʏ�̎B�e�p�V���b�^�[ 
        SHUTTER_SOUND_TYPE_MOVIE     = nn::camera::SHUTTER_SOUND_TYPE_MOVIE,      // ����B�e�J�n�� 
        SHUTTER_SOUND_TYPE_MOVIE_END = nn::camera::SHUTTER_SOUND_TYPE_MOVIE_END,  // ����B�e�I���� 
        SHUTTER_SOUND_TYPE_MAX       = nn::camera::SHUTTER_SOUND_TYPE_MAX                          
      };

      /**
       * @brief �G���[�̃r�b�g
       */
      enum
      {
        ERROR_BIT_NONE                                                   = 0,         // �G���[�Ȃ�

        ERROR_BIT_CAMERA_INITIALIZE_FATAL_ERROR                          = 1 <<  0,   // nn::camera::Initialze��nn::camera::ResultFatalError
        ERROR_BIT_CAMERA_ACTIVATE_FATAL_ERROR                            = 1 <<  1,   // nn::camera::Activate��nn::camera::ResultFatalError
        ERROR_BIT_CAMERA_GETSUITABLEY2RSTANDARDCOEFFICIENT_FATAL_ERROR   = 1 <<  2,   // nn::camera::GetSuitableY2rStandardCoefficient��nn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETSIZE_FATAL_ERROR                             = 1 <<  3,   // nn::camera::SetSize��nn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETNOISEFILTER_FATAL_ERROR                      = 1 <<  4,   // nn::camera::SetNoiseFilter��nn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETAUTOEXPOSURE_FATAL_ERROR                     = 1 <<  5,   // nn::camera::SetAutoExposure��nn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETAUTOEXPOSUREWINDOW_FATAL_ERROR               = 1 <<  6,   // nn::camera::SetAutoExposureWindow��nn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETEXPOSURE_FATAL_ERROR                         = 1 <<  7,   // nn::camera::SetExposure��nn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETWHITEBALANCE_FATAL_ERROR                     = 1 <<  8,   // nn::camera::SetWhiteBalance��nn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETAUTOWHITEBALANCE_FATAL_ERROR                 = 1 <<  9,   // nn::camera::SetAutoWhiteBalance��nn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETAUTOWHITEBALANCEWINDOW_FATAL_ERROR           = 1 << 10,   // nn::camera::SetAutoWhiteBalanceWindow��nn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETSHARPNESS_FATAL_ERROR                        = 1 << 11,   // nn::camera::SetSharpness��nn::camera::ResultFatalError
        ERROR_BIT_CAMERA_FLIPIMAGE_FATAL_ERROR                           = 1 << 12,   // nn::camera::FlipImage��nn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETFRAMERATE_FATAL_ERROR                        = 1 << 13,   // nn::camera::SetFrameRate��nn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETPHOTOMODE_FATAL_ERROR                        = 1 << 14,   // nn::camera::SetPhotoMode��nn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETEFFECT_FATAL_ERROR                           = 1 << 15,   // nn::camera::SetEffect��nn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETCONTRAST_FATAL_ERROR                         = 1 << 16,   // nn::camera::SetContrast��nn::camera::ResultFatalError
        ERROR_BIT_CAMERA_SETLENSCORRECTION_FATAL_ERROR                   = 1 << 17,   // nn::camera::SetLensCorrection��nn::camera::ResultFatalError
        ERROR_BIT_CAMERA_PLAYSHUTTERSOUND_FATAL_ERROR                    = 1 << 18,   // nn::camera::PlayShutterSound��nn::camera::ResultFatalError
        
        ERROR_BIT_Y2R_INITIALIZE_ERROR                                   = 1 << 19,   // nn::y2r::Initialize��false
        
#if GFL_DEBUG
        ERROR_BIT_DEBUG_FATAL_ERROR_OCCUR                                = 1 << 20,   // ���s���Ƀf�o�b�O�@�\�ŋN�������G���[�B�������O�Ƀf�o�b�O�@�\�ŃG���[���N�������Ƃ���ERROR_BIT_CAMERA_INITIALIZE_FATAL_ERROR�𗧂ĂĂ��܂��B
#endif
      };


    } // namespace camera
  } // namespace device
} // namespace gfl
#endif // __GFL_DEVICECAMERA_TYPES_H__

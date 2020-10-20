/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl_DeviceCamera_DefaultSettings.cpp
 * @brief  �J�����V�X�e���̃f�t�H���g�ݒ�
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



      // �J�����𖳌��ɂ����`
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
       * @brief �V���O���J����( �O�� )
       */
      //---------------------------------------------------------------------------------
      const System::SetupParam DefaultSettings_Single = 
      {
        NULL, // �V�X�e�����g�p����q�[�v
        NULL, // �V�X�e�����g�p����f�o�C�X�q�[�v

        // �J�������Ƃ̐ݒ�
        {
          // CAMERA_1
          {
            true,                                           // �ȍ~�̃p�����[�^���L�����ǂ���
            gfl::device::camera::DEVICE_ID_OUT_L,           // �ǂ̃J�����Ȃ̂��H
            gfl::device::camera::CAPTURE_SIZE_CTR_TOP_LCD,  // �J�������L���v�`������摜�̉𑜓x
            400, 240,                                       // �L���v�`���摜(YUV)�̃g���~���O��̕�, ����
            512, 256,                                       // �o�͉摜(RGB)�̕��E����
            gfl::device::camera::OUTPUT_R8G8B8,             // �o�̓s�N�Z����RGB�t�H�[�}�b�g
            gfl::device::camera::BLOCK_8_BY_8,              // �o�͉摜��RGB�f�[�^�̕��я�
            0xff,                                           // �o�͉摜(RGB)�̃A���t�@�l ( R8G8B8A8 or R5G5B5A1 �̏ꍇ�̂ݗL�� )
            true,                                           // �m�C�Y�t�B���^�[�̗L��
            true,                                           // �����I�o�@�\�̗L��
            0, 0, 640, 480,                                 // �����I�o�̌v�Z�Ɏg�p�����`�̈� ����x���W, ����y���W, ��, ����
            gfl::device::camera::WHITE_BALANCE_NORMAL,      // �z���C�g�o�����X�̎w��
            true,                                           // �I�[�g�z���C�g�o�����X�̗L��
            0, 0, 640, 480,                                 // �I�[�g�z���C�g�o�����X�̌v�Z�Ɏg�p�����`�̈� ����x���W, ����y���W, ��, ����
            0,                                              // �N���x [-4, 5]
            gfl::device::camera::FLIP_NONE,                 // ���]����
            gfl::device::camera::FRAME_RATE_30,             // �t���[�����[�g
            gfl::device::camera::PHOTO_MODE_NORMAL,         // �B�e���[�h
            gfl::device::camera::EFFECT_NONE,               // �G�t�F�N�g
            gfl::device::camera::CONTRAST_NORMAL,           // �R���g���X�g
            gfl::device::camera::LENS_CORRECTION_NORMAL,    // �����Y�␳�̒��x
          },

          // CAMERA_2
          GFL_DEVICE_CAMERA_INVALID_SETTING,
        },
        
        // �e�J��������X���b�h�̗D��x
        {
          gfl::base::Thread::DefaultPriority,
          gfl::base::Thread::DefaultPriority,
        },
        
        // �L���v�`���摜�����X���b�h�̗D��x
        gfl::base::Thread::DefaultPriority,
      };

      //---------------------------------------------------------------------------------
      /**
       * @brief �V���O���J����( ���� )
       */
      //---------------------------------------------------------------------------------
      const System::SetupParam DefaultSettings_SingleInside = 
      {
        NULL, // �V�X�e�����g�p����q�[�v
        NULL, // �V�X�e�����g�p����f�o�C�X�q�[�v

        // �J�������Ƃ̐ݒ�
        {
          // CAMERA_1
          {
            true,                                           // �ȍ~�̃p�����[�^���L�����ǂ���
            gfl::device::camera::DEVICE_ID_IN,              // �ǂ̃J�����Ȃ̂��H
            gfl::device::camera::CAPTURE_SIZE_CTR_TOP_LCD,  // �J�������L���v�`������摜�̉𑜓x
            400, 240,                                       // �L���v�`���摜(YUV)�̃g���~���O��̕�, ����
            512, 256,                                       // �o�͉摜(RGB)�̕��E����
            gfl::device::camera::OUTPUT_R8G8B8,             // �o�̓s�N�Z����RGB�t�H�[�}�b�g
            gfl::device::camera::BLOCK_8_BY_8,              // �o�͉摜��RGB�f�[�^�̕��я�
            0xff,                                           // �o�͉摜(RGB)�̃A���t�@�l ( R8G8B8A8 or R5G5B5A1 �̏ꍇ�̂ݗL�� )
            true,                                           // �m�C�Y�t�B���^�[�̗L��
            true,                                           // �����I�o�@�\�̗L��
            0, 0, 640, 480,                                 // �����I�o�̌v�Z�Ɏg�p�����`�̈� ����x���W, ����y���W, ��, ����
            gfl::device::camera::WHITE_BALANCE_NORMAL,      // �z���C�g�o�����X�̎w��
            true,                                           // �I�[�g�z���C�g�o�����X�̗L��
            0, 0, 640, 480,                                 // �I�[�g�z���C�g�o�����X�̌v�Z�Ɏg�p�����`�̈� ����x���W, ����y���W, ��, ����
            0,                                              // �N���x [-4, 5]
            gfl::device::camera::FLIP_NONE,                 // ���]����
            gfl::device::camera::FRAME_RATE_30,             // �t���[�����[�g
            gfl::device::camera::PHOTO_MODE_NORMAL,         // �B�e���[�h
            gfl::device::camera::EFFECT_NONE,               // �G�t�F�N�g
            gfl::device::camera::CONTRAST_NORMAL,           // �R���g���X�g
            gfl::device::camera::LENS_CORRECTION_NORMAL,    // �����Y�␳�̒��x
          },

          // CAMERA_2
          GFL_DEVICE_CAMERA_INVALID_SETTING,
        },
        
        // �e�J��������X���b�h�̗D��x
        {
          gfl::base::Thread::DefaultPriority,
          gfl::base::Thread::DefaultPriority,
        },
        
        // �L���v�`���摜�����X���b�h�̗D��x
        gfl::base::Thread::DefaultPriority,
      };




      //---------------------------------------------------------------------------------
      /**
       * @brief �X�e���I�J���� (�O���J������2�����g�p ==> ���̎�)
       */
      //---------------------------------------------------------------------------------
      const System::SetupParam DefaultSettings_Stereo = 
      {
        NULL, // �V�X�e�����g�p����q�[�v
        NULL, // �V�X�e�����g�p����f�o�C�X�q�[�v

        // �J�������Ƃ̐ݒ�
        {
          // CAMERA_1
          {
            true,                                           // �ȍ~�̃p�����[�^���L�����ǂ���
            gfl::device::camera::DEVICE_ID_OUT_L,           // �ǂ̃J�����Ȃ̂��H
            gfl::device::camera::CAPTURE_SIZE_CTR_TOP_LCD,  // �J�������L���v�`������摜�̉𑜓x
            400, 240,                                       // �L���v�`���摜(YUV)�̃g���~���O��̕�, ����
            512, 256,                                       // �o�͉摜(RGB)�̕��E����
            gfl::device::camera::OUTPUT_R8G8B8,             // �o�̓s�N�Z����RGB�t�H�[�}�b�g
            gfl::device::camera::BLOCK_8_BY_8,              // �o�͉摜��RGB�f�[�^�̕��я�
            0xff,                                           // �o�͉摜(RGB)�̃A���t�@�l ( R8G8B8A8 or R5G5B5A1 �̏ꍇ�̂ݗL�� )
            false,                                          // �m�C�Y�t�B���^�[�̗L��
            true,                                           // �����I�o�@�\�̗L��
            0, 0, 480, 480,                                 // �����I�o�̌v�Z�Ɏg�p�����`�̈� ����x���W, ����y���W, ��, ����
            gfl::device::camera::WHITE_BALANCE_NORMAL,      // �z���C�g�o�����X�̎w��
            true,                                           // �I�[�g�z���C�g�o�����X�̗L��
            0, 0, 480, 480,                                 // �I�[�g�z���C�g�o�����X�̌v�Z�Ɏg�p�����`�̈� ����x���W, ����y���W, ��, ����
            0,                                              // �N���x [-4, 5]
            gfl::device::camera::FLIP_NONE,                 // ���]����
            gfl::device::camera::FRAME_RATE_15,             // �t���[�����[�g
            gfl::device::camera::PHOTO_MODE_NORMAL,         // �B�e���[�h
            gfl::device::camera::EFFECT_NONE,               // �G�t�F�N�g
            gfl::device::camera::CONTRAST_NORMAL,           // �R���g���X�g
            gfl::device::camera::LENS_CORRECTION_NORMAL,    // �����Y�␳�̒��x
          },

          // CAMERA_2
          {
            true,                                           // �ȍ~�̃p�����[�^���L�����ǂ���
            gfl::device::camera::DEVICE_ID_OUT_R,           // �ǂ̃J�����Ȃ̂��H
            gfl::device::camera::CAPTURE_SIZE_CTR_TOP_LCD,  // �J�������L���v�`������摜�̉𑜓x
            400, 240,                                       // �L���v�`���摜(YUV)�̃g���~���O��̕�, ����
            512, 256,                                       // �o�͉摜(RGB)�̕��E����
            gfl::device::camera::OUTPUT_R8G8B8,             // �o�̓s�N�Z����RGB�t�H�[�}�b�g
            gfl::device::camera::BLOCK_8_BY_8,              // �o�͉摜��RGB�f�[�^�̕��я�
            0xff,                                           // �o�͉摜(RGB)�̃A���t�@�l ( R8G8B8A8 or R5G5B5A1 �̏ꍇ�̂ݗL�� )
            false,                                          // �m�C�Y�t�B���^�[�̗L��
            true,                                           // �����I�o�@�\�̗L��
            160, 0, 480, 480,                               // �����I�o�̌v�Z�Ɏg�p�����`�̈� ����x���W, ����y���W, ��, ����
            gfl::device::camera::WHITE_BALANCE_NORMAL,      // �z���C�g�o�����X�̎w��
            true,                                           // �I�[�g�z���C�g�o�����X�̗L��
            160, 0, 480, 480,                               // �I�[�g�z���C�g�o�����X�̌v�Z�Ɏg�p�����`�̈� ����x���W, ����y���W, ��, ����
            0,                                              // �N���x [-4, 5]
            gfl::device::camera::FLIP_NONE,                 // ���]����
            gfl::device::camera::FRAME_RATE_15,             // �t���[�����[�g
            gfl::device::camera::PHOTO_MODE_NORMAL,         // �B�e���[�h
            gfl::device::camera::EFFECT_NONE,               // �G�t�F�N�g
            gfl::device::camera::CONTRAST_NORMAL,           // �R���g���X�g
            gfl::device::camera::LENS_CORRECTION_NORMAL,    // �����Y�␳�̒��x
          },
        },
        
        // �e�J��������X���b�h�̗D��x
        {
          gfl::base::Thread::DefaultPriority,
          gfl::base::Thread::DefaultPriority,
        },
        
        // �L���v�`���摜�����X���b�h�̗D��x
        gfl::base::Thread::DefaultPriority,
      };

      //---------------------------------------------------------------------------------
      /**
       * @brief �}���`�J���� (�����J�����ƊO���J�����𓯎��g�p)
       */
      //---------------------------------------------------------------------------------
      const System::SetupParam DefaultSettings_Multi = 
      {
        NULL, // �V�X�e�����g�p����q�[�v
        NULL, // �V�X�e�����g�p����f�o�C�X�q�[�v

        // �J�������Ƃ̐ݒ�
        {
          // CAMERA_1
          {
            true,                                           // �ȍ~�̃p�����[�^���L�����ǂ���
            gfl::device::camera::DEVICE_ID_OUT_L,           // �ǂ̃J�����Ȃ̂��H
            gfl::device::camera::CAPTURE_SIZE_CTR_TOP_LCD,  // �J�������L���v�`������摜�̉𑜓x
            400, 240,                                       // �L���v�`���摜(YUV)�̃g���~���O��̕�, ����
            512, 256,                                       // �o�͉摜(RGB)�̕��E����
            gfl::device::camera::OUTPUT_R8G8B8,             // �o�̓s�N�Z����RGB�t�H�[�}�b�g
            gfl::device::camera::BLOCK_8_BY_8,              // �o�͉摜��RGB�f�[�^�̕��я�
            0xff,                                           // �o�͉摜(RGB)�̃A���t�@�l ( R8G8B8A8 or R5G5B5A1 �̏ꍇ�̂ݗL�� )
            true,                                           // �m�C�Y�t�B���^�[�̗L��
            true,                                           // �����I�o�@�\�̗L��
            0, 0, 640, 480,                                 // �����I�o�̌v�Z�Ɏg�p�����`�̈� ����x���W, ����y���W, ��, ����
            gfl::device::camera::WHITE_BALANCE_NORMAL,      // �z���C�g�o�����X�̎w��
            true,                                           // �I�[�g�z���C�g�o�����X�̗L��
            0, 0, 640, 480,                                 // �I�[�g�z���C�g�o�����X�̌v�Z�Ɏg�p�����`�̈� ����x���W, ����y���W, ��, ����
            0,                                              // �N���x [-4, 5]
            gfl::device::camera::FLIP_NONE,                 // ���]����
            gfl::device::camera::FRAME_RATE_30,             // �t���[�����[�g
            gfl::device::camera::PHOTO_MODE_NORMAL,         // �B�e���[�h
            gfl::device::camera::EFFECT_NONE,               // �G�t�F�N�g
            gfl::device::camera::CONTRAST_NORMAL,           // �R���g���X�g
            gfl::device::camera::LENS_CORRECTION_NORMAL,    // �����Y�␳�̒��x
          },

          // CAMERA_2
          {
            true,                                           // �ȍ~�̃p�����[�^���L�����ǂ���
            gfl::device::camera::DEVICE_ID_IN,              // �ǂ̃J�����Ȃ̂��H
            gfl::device::camera::CAPTURE_SIZE_CTR_BOTTOM_LCD, // �J�������L���v�`������摜�̉𑜓x
            320, 240,                                       // �L���v�`���摜(YUV)�̃g���~���O��̕�, ����
            512, 256,                                       // �o�͉摜(RGB)�̕��E����
            gfl::device::camera::OUTPUT_R8G8B8,             // �o�̓s�N�Z����RGB�t�H�[�}�b�g
            gfl::device::camera::BLOCK_8_BY_8,              // �o�͉摜��RGB�f�[�^�̕��я�
            0xff,                                           // �o�͉摜(RGB)�̃A���t�@�l ( R8G8B8A8 or R5G5B5A1 �̏ꍇ�̂ݗL�� )
            true,                                           // �m�C�Y�t�B���^�[�̗L��
            true,                                           // �����I�o�@�\�̗L��
            160, 0, 640, 480,                               // �����I�o�̌v�Z�Ɏg�p�����`�̈� ����x���W, ����y���W, ��, ����
            gfl::device::camera::WHITE_BALANCE_NORMAL,      // �z���C�g�o�����X�̎w��
            true,                                           // �I�[�g�z���C�g�o�����X�̗L��
            160, 0, 640, 480,                               // �I�[�g�z���C�g�o�����X�̌v�Z�Ɏg�p�����`�̈� ����x���W, ����y���W, ��, ����
            0,                                              // �N���x [-4, 5]
            gfl::device::camera::FLIP_NONE,                 // ���]����
            gfl::device::camera::FRAME_RATE_30,             // �t���[�����[�g
            gfl::device::camera::PHOTO_MODE_NORMAL,         // �B�e���[�h
            gfl::device::camera::EFFECT_NONE,               // �G�t�F�N�g
            gfl::device::camera::CONTRAST_NORMAL,           // �R���g���X�g
            gfl::device::camera::LENS_CORRECTION_NORMAL,    // �����Y�␳�̒��x
          },
        },
        
        // �e�J��������X���b�h�̗D��x
        {
          gfl::base::Thread::DefaultPriority,
          gfl::base::Thread::DefaultPriority,
        },
        
        // �L���v�`���摜�����X���b�h�̗D��x
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
        
        // �e�J��������X���b�h�̗D��x
        {
          gfl::base::Thread::DefaultPriority,
          gfl::base::Thread::DefaultPriority,
        },
        
        // �L���v�`���摜�����X���b�h�̗D��x
        gfl::base::Thread::DefaultPriority,
      };
#endif


    } // namespace camera
  } // namespace device
} // namespace gfl

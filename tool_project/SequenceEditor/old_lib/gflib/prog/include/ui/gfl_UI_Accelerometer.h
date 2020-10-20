//=============================================================================
/**
 * @brief  �����x�Z���T�̃C���^�[�t�F�[�X
 * @file   gfl_UI_Accelerometer.h
 * @author obata_toshihiro
 * @date   2011.03.24
 */
//=============================================================================
#ifndef __GFL_UI_ACCELEROMETER_H__
#define __GFL_UI_ACCELEROMETER_H__
#pragma once

#include <gfl_Heap.h>
#include <gfl_Macros.h>
#include <ui/gfl_UI_Types.h>
#include <ui/gfl_UI_Device.h>

namespace gfl {
  namespace ui {

    class DeviceImplementerForAccelerometer;


    class Accelerometer : public Device
    {
      GFL_FORBID_COPY_AND_ASSIGN( Accelerometer );
      friend class DeviceManager;

      public:


      // ���o�f�[�^
      struct DetectiveData 
      {
        // �L���ȃf�[�^���ǂ���
        bool is_valid;

        // �v���l
        f32 measured_value_x;
        f32 measured_value_y;
        f32 measured_value_z;

        // �����x
        f32 acceleration_x;
        f32 acceleration_y;
        f32 acceleration_z;
      }; 


      // �����f�[�^
      struct EffectiveData 
      {
        // �L���ȃf�[�^���ǂ���
        bool is_valid;

        // �v���l
        f32 measured_value_x;
        f32 measured_value_y;
        f32 measured_value_z;

        // �����x
        f32 acceleration_x;
        f32 acceleration_y;
        f32 acceleration_z;
      };


      static const s16 MAX_MESURED_VALUE; // ����l�̐�Βl
      static const s16 MAX_PLAY_RANGE;    // ����l�́u�V�сv���̍ő�l
      static const s16 MAX_SENSITIVITY;   // ���o���x�̍ő�l


      //-----------------------------------------------------------------------
      /**
       * @brief �L���ȃf�[�^�������Ă��邩�ǂ����𒲂ׂ�
       *
       * @retval true   �L���ȃf�[�^�������Ă���
       * @retval false  �L���ȃf�[�^�������Ă��Ȃ�
       */
      //-----------------------------------------------------------------------
      bool IsValid( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �v���l���擾����
       *
       * @param[out] buffer  �擾�����l�̊i�[��
       *
       * @note �e���̌���
       * x��+  �\���{�^���̍�����
       * y��+  �^�b�`�p�l���̎����ʕ���
       * z��+  �\���{�^���̏����
       */
      //-----------------------------------------------------------------------
      void GetMeasuredValue( gfl::math::VEC3* buffer ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �����x���擾����
       *
       * @param[out] buffer  �擾�����l�̊i�[��
       */
      //-----------------------------------------------------------------------
      void GetAcceleration( gfl::math::VEC3* buffer ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief ����l�́u�V�сv����ݒ肷��
       *
       * @param range  �V�ѕ�[0, MAX_PLAY_RANGE]
       */
      //-----------------------------------------------------------------------
      void SetPlayRange( s16 range );

      //-----------------------------------------------------------------------
      /**
       * @brief ����l�́u�V�сv�����擾����
       *
       * @retval [0, MAX_PLAY_RANGE]
       */
      //-----------------------------------------------------------------------
      s16 GetPlayRange( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief ���o���x��ݒ肷��
       *
       * @param sensitivity  ���o���x[0, MAX_SENSITIVITY]
       */
      //-----------------------------------------------------------------------
      void SetSensitivity( s16 sensitivity );

      //-----------------------------------------------------------------------
      /**
       * @brief ���o���x���擾����
       *
       * @retval [0, MAX_SENSITIVITY]
       */
      //-----------------------------------------------------------------------
      s16 GetSensitivity( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief ����l�ւ̃I�t�Z�b�g��L���ɂ���
       */
      //-----------------------------------------------------------------------
      void EnableOffset( void );

      //-----------------------------------------------------------------------
      /**
       * @brief ����l�ւ̃I�t�Z�b�g�𖳌��ɂ���
       */
      //-----------------------------------------------------------------------
      void DisableOffset( void );

      //-----------------------------------------------------------------------
      /**
       * @brief ����l�ւ̃I�t�Z�b�g���L�����ǂ����𒲂ׂ�
       *
       * @retval true   �I�t�Z�b�g���L��
       * @retval false  �I�t�Z�b�g������
       */
      //-----------------------------------------------------------------------
      bool IsOffsetEnable( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief ���݂̃I�t�Z�b�g�l���擾����
       *
       * @param[out] buffer  �擾�����I�t�Z�b�g�l���i�[����o�b�t�@
       */
      //-----------------------------------------------------------------------
      void GetOffset( gfl::math::VEC3* buffer ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief ����l�ւ̃I�t�Z�b�g�l�𒼐ڎw�肷��
       *
       * @param offset  ����l�ɉ��Z����I�t�Z�b�g�l
       *
       * @note ����l�ɒP���ɉ��Z����܂�
       */
      //-----------------------------------------------------------------------
      void SetOffset( const gfl::math::VEC3& offset );


      //-----------------------------------------------------------------------
      /**
       * @brief �I�t�Z�b�g�l�����Z�b�g����( �I�t�Z�b�g�������Ȓl�ɂ��� )
       */
      //-----------------------------------------------------------------------
      void ResetOffset( void );

      //-----------------------------------------------------------------------
      /**
       * @brief ����]��L���ɂ���
       */
      //-----------------------------------------------------------------------
      void EnableAxisRotation( void );

      //-----------------------------------------------------------------------
      /**
       * @brief ����]�𖳌��ɂ���
       */
      //-----------------------------------------------------------------------
      void DisableAxisRotation( void );

      //-----------------------------------------------------------------------
      /**
       * @brief ����]���L�����ǂ����𒲂ׂ�
       *
       * @retval true  ����]���L��
       * @retval false ����]������
       */
      //-----------------------------------------------------------------------
      bool IsAxisRotationEnable( void );

      //-----------------------------------------------------------------------
      /**
       * @brief ����]�Ɏg�p���Ă����]�s����擾����
       *
       * @param buffer  �擾�����s��̊i�[��
       */
      //-----------------------------------------------------------------------
      void GetAxisRotationMatrix( gfl::math::MTX34* buffer ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief ����]�Ɏg�p�����]�s���ݒ肷��
       *
       * @param matrix  ����]�Ɏg�p�����]�s��
       */
      //-----------------------------------------------------------------------
      void SetAxisRotationMatrix( const gfl::math::MTX34& matrix );

      //-----------------------------------------------------------------------
      /**
       * @brief ����]�Ɏg�p�����]�s������Z�b�g����( ����]���Ȃ���Ԃɂ��� )
       */
      //-----------------------------------------------------------------------
      void ResetAxisRotationMatrix( void );



      protected:

      /**
       * @brief �R���X�g���N�^
       *
       * @param heap         �g�p����q�[�v
       * @param implementer  ���ۂɓ��͂̓ǂݎ����s���I�u�W�F�N�g
       */
      Accelerometer( gfl::heap::HeapBase* heap, DeviceImplementerForAccelerometer* implementer );

      /**
       * @brief �f�X�g���N�^
       */
      virtual ~Accelerometer(); 

      /**
       * @brief �����f�[�^��~�ς���
       *
       * @param buffer               �f�[�^�̊i�[��
       * @param detective_data       ����̌��o�f�[�^
       * @param prev_detective_data  �O��̌��o�f�[�^
       */
      virtual void StoreEffectiveData( void* buffer, const void* detective_data, const void* prev_detective_data ) const;



      private:
      DeviceImplementerForAccelerometer* m_imple_for_accelerometer;

      const EffectiveData& GetEffectiveData( void ) const;
    }; 


  }  //namespace ui
}  //namespace gfl 


#endif  //__GFL_UI_ACCELEROMETER_H__

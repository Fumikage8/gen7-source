//=============================================================================
/**
 * @file   gfl_UI_VectorDevice.h
 * @brief  �������̓f�o�C�X�̃A�N�Z�b�T
 * @author obata_toshihiro
 * @date   2010.10.21
 */
//=============================================================================
#ifndef __GFL_UI_VECTORDEVICE_H__
#define __GFL_UI_VECTORDEVICE_H__
#pragma once

#include "gfl_Heap.h"
#include "gfl_Macros.h"
#include "math/gfl_MathType.h"
#include "ui/gfl_UI_Types.h"
#include "ui/gfl_UI_Device.h"


namespace gfl {
  namespace ui {

    class DeviceImplementerForVectorDevice;


    class VectorDevice : public Device 
    {
      GFL_FORBID_COPY_AND_ASSIGN( VectorDevice );
      friend class DeviceManager;

      public:


      // ���o�f�[�^
      struct DetectiveData 
      {
        f32 x;  // x ��������
        f32 y;  // y ��������
      };

      // �����f�[�^
      struct EffectiveData 
      {
        f32 x;        // x��������
        f32 y;        // y��������
        u32 hold;     // �����_�Łu������Ă���v�Ɣ��肳�������̘_���a
        u32 trigger;  // �����_�Łu�����ꂽ�v�Ɣ��肳�������̘_���a
        u32 release;  // �����_�Łu�����ꂽ�v�Ɣ��肳�������̘_���a
        u32 repeat;   // �����_�Łu�����ꑱ���Ă���v�Ɣ��肳�������̘_���a
      };



      //-----------------------------------------------------------------------
      /**
       * @brief ���͂��ꂽ�������擾����
       *
       * @return ���͕�����x����[-1, 1]
       */
      //-----------------------------------------------------------------------
      f32 GetX( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief ���͂��ꂽ�������擾����
       *
       * @return ���͕�����y����[-1, 1]
       */
      //-----------------------------------------------------------------------
      f32 GetY( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief ���͂��ꂽ�������擾����
       *
       * @param dest_x  �擾����x����[-1, 1]�̊i�[��
       * @param dest_y  �擾����y����[-1, 1]�̊i�[��
       */
      //-----------------------------------------------------------------------
      void GetVector( f32* dest_x, f32* dest_y ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief ���͂��ꂽ�������擾����
       *
       * @param dest_vec  �擾�������͕����x�N�g���̊i�[��
       */
      //-----------------------------------------------------------------------
      void GetVector( gfl::math::VEC2* dest_vec ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief ���͂��ꂽ�������擾����
       *
       * @param dest_vec  �擾�������͕����x�N�g���̊i�[��
       */
      //-----------------------------------------------------------------------
      void GetVector( gfl::math::VEC3* dest_vec ) const; 




      //-----------------------------------------------------------------------
      /**
       * @brief �w�肵�����������͂���Ă��邩�ǂ����𒲂ׂ�
       *
       * @param dir  ���肷�����( gfl::ui::DIR_xxxx )
       *
       * @retval true   ���͂���Ă���
       * @retval false  ���͂���Ă��Ȃ�
       */
      //-----------------------------------------------------------------------
      bool IsHold( u32 dir ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �w�肵�����������͂��ꂽ���ǂ����𒲂ׂ�
       *
       * @param dir  ���肷�����( gfl::ui::DIR_xxxx )
       *
       * @retval true   ���͂��ꂽ
       * @retval false  ���͂���Ă��Ȃ�
       */
      //-----------------------------------------------------------------------
      bool IsTrigger( u32 dir ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �w�肵�������̓��͂������Ȃ������ǂ����𒲂ׂ�
       *
       * @param dir  ���肷�����( gfl::ui::DIR_xxxx )
       *
       * @retval true   ���͂������Ȃ���
       * @retval false  ���͂������Ȃ��Ă��Ȃ�
       */
      //-----------------------------------------------------------------------
      bool IsRelease( u32 dir ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �w�肵�������̃��s�[�g�����o�������ǂ����𒲂ׂ�
       *
       * @param dir  ���肷�����( gfl::ui::DIR_xxxx )
       *
       * @retval true   ���s�[�g�����o����
       * @retval false  �����s�[�g�����o���Ă��Ȃ�
       */
      //-----------------------------------------------------------------------
      bool IsRepeat( u32 dir ) const;



      //-----------------------------------------------------------------------
      /**
       * @brief ������Ă�������̏����擾����
       * @return ������Ă���S�Ă̕���( gfl::ui::DIR_xxxx )�̘_���a
       */
      //-----------------------------------------------------------------------
      u32 GetHold( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �����ꂽ�����̏����擾����
       * @return �����ꂽ�S�Ă̕���( gfl::ui::DIR_xxxx )�̘_���a
       */
      //-----------------------------------------------------------------------
      u32 GetTrigger( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �����ꂽ�������̏����擾����
       * @return �����ꂽ�S�Ă̕���( gfl::ui::DIR_xxxx )�̘_���a
       */
      //-----------------------------------------------------------------------
      u32 GetRelease( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief ���s�[�g�����o���������̏����擾����
       * @return ���s�[�g�����o�����S�Ă̕���( gfl::ui::DIR_xxxx )�̘_���a
       */
      //-----------------------------------------------------------------------
      u32 GetRepeat( void ) const;



      // �ŐV�̌��o�����擾����
      // �����̃��\�b�h�ł�, ����t���[�����[�g�̐ݒ�ɍS�炸
      // �Ō�̍X�V�Ō��o�����l��Ԃ��܂�.
      f32 GetLatestX( void ) const;
      f32 GetLatestY( void ) const;
      bool IsHoldLatest( u32 dir ) const;
      bool IsTriggerLatest( u32 dir ) const;
      bool IsReleaseLatest( u32 dir ) const;
      bool IsRepeatLatest( u32 dir ) const;



      // �A�i���O�X�e�B�b�N�̃N�����v���[�h
      enum StickClampMode {
        STICK_CLAMP_MODE_CIRCLE,   // �~�`�N�����v
        STICK_CLAMP_MODE_CROSS,    // �\���`�N�����v
        STICK_CLAMP_MODE_MINIMUM,  // �ŏ��N�����v
      };

      //-------------------------------------------------------------------------------
      /**
       * @brief �A�i���O�X�e�B�b�N�̃N�����v���[�h���擾����
       */
      //-------------------------------------------------------------------------------
      StickClampMode GetStickClampMode( void ) const;

      //-------------------------------------------------------------------------------
      /**
       * @brief �A�i���O�X�e�B�b�N�̃N�����v���[�h��ݒ肷��
       * @param clamp_mode �ݒ肷��N�����v���[�h
       */
      //-------------------------------------------------------------------------------
      void SetStickClampMode( StickClampMode clamp_mode );


      // ���[�h�ʂ̃N�����v�p�����[�^����l�E�����l
      static const s16 MAX_STICK_CLAMP_PARAM_ON_CIRCLE_MODE;
      static const s16 MIN_STICK_CLAMP_PARAM_ON_CIRCLE_MODE;
      static const s16 MAX_STICK_CLAMP_PARAM_ON_CROSS_MODE;
      static const s16 MIN_STICK_CLAMP_PARAM_ON_CROSS_MODE;
      static const s16 MAX_STICK_CLAMP_PARAM_ON_MINIMUM_MODE;
      static const s16 MIN_STICK_CLAMP_PARAM_ON_MINIMUM_MODE;

      //-------------------------------------------------------------------------------
      /**
       * @brief �A�i���O�X�e�B�b�N�̃N�����v�p�����[�^���擾����
       *
       * @param[out] min  �ŏ��N�����v�l�̊i�[��
       * @param[out] max  �ő�N�����v�l�̊i�[��
       */
      //-------------------------------------------------------------------------------
      void GetStickClampParam( s16* min, s16* max ) const;

      //-------------------------------------------------------------------------------
      /**
       * @brief �A�i���O�X�e�B�b�N�̃N�����v�p�����[�^��ݒ肷��
       *
       * @param[in] min  �ŏ��N�����v�l
       * @param[in] max  �ő�N�����v�l
       */
      //-------------------------------------------------------------------------------
      void SetStickClampParam( s16 min, s16 max );





      protected:

      /**
       * @brief �R���X�g���N�^
       *
       * @param heap         �g�p����q�[�v
       * @param implementer  ���ۂɓ��͂̓ǂݎ����s���C���X�^���X
       */
      VectorDevice( gfl::heap::HeapBase* heap, DeviceImplementerForVectorDevice* implementer );

      /**
       * @brief �f�X�g���N�^
       */
      virtual ~VectorDevice(); 

      /**
       * @brief ���݂̌��o�f�[�^�ƑO��̌��o�f�[�^���r��, �����f�[�^�������Ă��邩�ǂ����𒲂ׂ�
       * @retval true   �O��̌��o�f�[�^�ƍ���̌��o�f�[�^������
       * @retval false  �O��̌��o�f�[�^�ƍ���̌��o�f�[�^���قȂ�
       */
      virtual bool IsRepeatingSameInput( void ) const; 

      /**
       * @brief �����f�[�^��~�ς���
       * @param[out] buffer               �f�[�^�̊i�[��
       * @param[in]  detective_data       ����̌��o�f�[�^
       * @param[in]  prev_detective_data  �O��̌��o�f�[�^
       */
      virtual void StoreEffectiveData( 
          void* buffer,
          const void* detective_data,
          const void* prev_detective_data ) const;



      private:
      DeviceImplementerForVectorDevice* m_pImplementer;

      u32 GetHoldState( f32, f32 ) const;
      void MergeDetectData( EffectiveData*, u32, u32, Direction ) const;
      const EffectiveData& GetEffectiveData( void ) const;

    };


  } // namespace ui
} // namespace gfl

#endif // __GFL_UI_VECTORDEVICE_H__

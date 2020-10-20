//=============================================================================
/**
 * @file   gfl_UI_Button.h
 * @brief  �{�^���̃A�N�Z�b�T
 * @author obata_toshihiro
 * @date   2010.10.21
 */
//=============================================================================
#ifndef __GFL_UI_BUTTON_H__
#define __GFL_UI_BUTTON_H__
#pragma once

#include "gfl_Heap.h"
#include "gfl_Macros.h"
#include "ui/gfl_UI_Types.h"
#include "ui/gfl_UI_Device.h"

namespace gfl {
  namespace ui {

    class DeviceImplementer;


    class Button : public Device 
    {
      GFL_FORBID_COPY_AND_ASSIGN( Button );
      friend class DeviceManager;


      public: 

      // ���o�f�[�^
      struct DetectiveData 
      {
        u32 hold;  // ������Ă���{�^��
      }; 

      // ���͏��
      struct InputState
      {
        u32 hold;     //�u������Ă���v�{�^���̘_���a
        u32 trigger;  //�u�����ꂽ�v�{�^���̘_���a
        u32 release;  //�u�����ꂽ�v�{�^���̘_���a
        u32 repeat;   //�u�����ꑱ���Ă���v�{�^���̘_���a
      };

      // ���͏�Ԃ̎��ʎq
      enum InputStateID {
        INPUT_STATE_ASSIGNED,  // �{�^���̊��蓖�Ă��K�p���ꂽ���
        INPUT_STATE_ORIGINAL,  // �I���W�i���̏��
        INPUT_STATE_NUM,
      };

      // �����f�[�^
      struct EffectiveData 
      {
        InputState inputState[ INPUT_STATE_NUM ];
      };

      // �{�^�����̏��
      struct ButtonInfo
      {
        u32 assignedButton;
      };

      //-----------------------------------------------------------------------
      /**
       * @brief �w�肵���{�^����������Ă��邩�ǂ������ׂ�
       * @param button       ���ׂ�{�^�� ( gfl::ui::BUTTON_xxxx )
       * @param targetState  �`�F�b�N�Ώۂ̓��͏��
       * @retval true   ������Ă���
       * @retval false  ������Ă��Ȃ�
       */
      //-----------------------------------------------------------------------
      bool IsHold( u32 button, InputStateID targetState=INPUT_STATE_ASSIGNED ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �w�肵���{�^���������ꂽ���ǂ����𒲂ׂ�
       * @param button       ���ׂ�{�^�� ( gfl::ui::BUTTON_xxxx )
       * @param targetState  �`�F�b�N�Ώۂ̓��͏��
       * @retval true   �����ꂽ
       * @retval false  ������Ă��Ȃ�
       */
      //-----------------------------------------------------------------------
      bool IsTrigger( u32 button, InputStateID targetState=INPUT_STATE_ASSIGNED ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �w�肵���{�^���������ꂽ���ǂ����𒲂ׂ�
       * @param button       ���ׂ�{�^�� ( gfl::ui::BUTTON_xxxx )
       * @param targetState  �`�F�b�N�Ώۂ̓��͏��
       * @retval true   �����ꂽ
       * @retval false  ������Ă��Ȃ�
       */
      //-----------------------------------------------------------------------
      bool IsRelease( u32 button, InputStateID targetState=INPUT_STATE_ASSIGNED ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �w�肵���{�^���������ꑱ���Ă��邩�ǂ����𒲂ׂ�
       * @param button       ���ׂ�{�^�� ( gfl::ui::BUTTON_xxxx )
       * @param targetState  �`�F�b�N�Ώۂ̓��͏��
       * @retval true   �����ꑱ���Ă���, ���s�[�g�^�C�~���O�����o����
       * @retval false  �����ꑱ���Ă��Ȃ�, �܂��̓��s�[�g�^�C�~���O�łȂ�
       */
      //-----------------------------------------------------------------------
      bool IsRepeat( u32 button, InputStateID targetState=INPUT_STATE_ASSIGNED ) const; 

      //-----------------------------------------------------------------------
      /**
       * @brief ������Ă���{�^���̏����擾����
       * @param targetState  �`�F�b�N�Ώۂ̓��͏��
       * @return ������Ă���S�Ẵ{�^��( gfl::ui::BUTTON_xxxx )�̘_���a
       */
      //-----------------------------------------------------------------------
      u32 GetHold( InputStateID targetState=INPUT_STATE_ASSIGNED ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �����ꂽ�{�^���̏����擾����
       * @param targetState  �`�F�b�N�Ώۂ̓��͏��
       * @return �����ꂽ�S�Ẵ{�^��( gfl::ui::BUTTON_xxxx )�̘_���a
       */
      //-----------------------------------------------------------------------
      u32 GetTrigger( InputStateID targetState=INPUT_STATE_ASSIGNED ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �����ꂽ���{�^���̏����擾����
       * @param targetState  �`�F�b�N�Ώۂ̓��͏��
       * @return �����ꂽ�S�Ẵ{�^��( gfl::ui::BUTTON_xxxx )�̘_���a
       */
      //-----------------------------------------------------------------------
      u32 GetRelease( InputStateID targetState=INPUT_STATE_ASSIGNED ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief ���s�[�g�����o�����{�^���̏����擾����
       * @param targetState  �`�F�b�N�Ώۂ̓��͏��
       * @return ���s�[�g�����o�����S�Ẵ{�^��( gfl::ui::BUTTON_xxxx )�̘_���a
       */
      //-----------------------------------------------------------------------
      u32 GetRepeat( InputStateID targetState=INPUT_STATE_ASSIGNED ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �{�^�����蓖�ď���ǉ�����
       * @param targetButton  ����Ώۂ̃{�^��
       * @param assignButton  ���蓖�Ă�{�^��
       *
       * @note AddButtonAssignment( gfl::ui::BUTTONID_A, gfl::ui::BUTTONID_L ) �Ƃ����ꍇ,
       *       L�{�^����������, L�{�^����A�{�^���������ꂽ����ɂȂ�܂�.
       */
      //-----------------------------------------------------------------------
      void AddButtonAssignment( ButtonID targetButton, ButtonID assignButton );

      //-----------------------------------------------------------------------
      /**
       * @brief �{�^�����蓖�ď�����������
       * @param targetButton  ����Ώۂ̃{�^��
       * @param removeButton  ���蓖�Ă���������{�^��
       *
       * @note RemoveButtonAssignment( gfl::ui::BUTTONID_A, gfl::ui::BUTTONID_L ) �Ƃ����ꍇ,
       *       L�{�^���������Ă�, A�{�^���������ꂽ����ɂȂ�Ȃ��Ȃ�܂�.
       */
      //-----------------------------------------------------------------------
      void RemoveButtonAssignment( ButtonID targetButton, ButtonID removeButton );

      //-----------------------------------------------------------------------
      /**
       * @brief �{�^�����蓖�ď�������������
       */
      //-----------------------------------------------------------------------
      void InitButtonAssignment( void );



      protected:

      /**
       * @brief �R���X�g���N�^
       * @param heap         �g�p����q�[�v
       * @param implementer  ���ۂɓ��͂�ǂݎ��C���X�^���X
       */
      Button( gfl::heap::HeapBase* heap, DeviceImplementer* implementer );

      /**
       * @brief �f�X�g���N�^
       */
      virtual ~Button(); 

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
      virtual void StoreEffectiveData( void* buffer, const void* detective_data, const void* prev_detective_data ) const;

      /**
       * @brief �O��ƍŐV�̓��͂���, ���͏�Ԃ��쐬����
       * @param[out] buffer     �쐬�����f�[�^�̊i�[��
       * @param[in]  now_hold   ����̃z�[���h���
       * @param[in]  prev_hold  �O��̃z�[���h���
       * @param[in]  button     ��������{�^��
       */
      void CalcInputState( InputState* buffer, u32 now_hold, u32 prev_hold, u32 button ) const;


      private:
      ButtonInfo m_buttonInfo[ BUTTON_NUM ];

      private:
      void CalcAssignedDetectiveData( DetectiveData*, const DetectiveData& ) const;
      const InputState& GetEffectiveInputState( InputStateID ) const;
      const EffectiveData& GetEffectiveData( void ) const;
    };


  }  //namespace ui
}  //namespace gfl

#endif // __GFL_UI_BUTTON_H__

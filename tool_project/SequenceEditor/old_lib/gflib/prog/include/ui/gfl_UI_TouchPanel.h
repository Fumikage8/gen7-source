//=============================================================================
/**
 * @file   gfl_UI_TouchPanel.h
 * @brief  �^�b�`�p�l���̃A�N�Z�b�T
 * @author toshihiro_obata
 * @date   2010.10.22
 */
//=============================================================================
#ifndef __GFL_UI_TOUCHPANEL_H__
#define __GFL_UI_TOUCHPANEL_H__
#pragma once

#include "gfl_Macros.h"
#include "gfl_Math.h"
#include "gfl_Heap.h"
#include "ui/gfl_UI_Types.h"
#include "ui/gfl_UI_Device.h"


namespace gfl {
  namespace ui {

    class DeviceImplementer;


    class TouchPanel : public Device 
    {
      GFL_FORBID_COPY_AND_ASSIGN( TouchPanel );
      friend class DeviceManager;

      public:

      // ���o�f�[�^
      struct DetectiveData 
      {
        u16  x;      // �^�b�`����Ă���x���W
        u16  y;      // �^�b�`����Ă���y���W
        bool touch;  // �^�b�`����Ă��邩�H
      };

      // �����f�[�^
      struct EffectiveData 
      {
        u16 last_x;    // �Ō�Ƀ^�b�`���ꂽx���W
        u16 last_y;    // �Ō�Ƀ^�b�`���ꂽy���W
        u16 prev_x;    // �O��^�b�`���ꂽx���W
        u16 prev_y;    // �O��^�b�`���ꂽy���W
        bool hold;     // �^�b�`����Ă��邩�H
        bool trigger;  // �^�b�`���ꂽ���H
        bool release;  // �^�b�`�������ꂽ���H
        bool repeat;   // �^�b�`���ꑱ���Ă��邩�H

        EffectiveData( void ) : 
          last_x( 0 ),
          last_y( 0 ),
          prev_x( 0 ),
          prev_y( 0 ),
          hold( false ),
          trigger( false ),
          release( false ),
          repeat( false ) { }
      };

      //-----------------------------------------------------------------------
      // �^�b�`����
      //-----------------------------------------------------------------------
      /**
       * @brief �^�b�`����Ă��邩�ǂ����𒲂ׂ�
       * @retval true   �^�b�`����Ă���
       * @retval false  �^�b�`����Ă��Ȃ�
       */
      bool IsTouch( void ) const;

      /**
       * @brief �^�b�`���ꂽ���ǂ����𒲂ׂ�
       * @retval true   �^�b�`���ꂽ
       * @retval false  �^�b�`����ĂȂ�
       */
      bool IsTouchTrigger( void ) const;

      /**
       * @brief �^�b�`�������ꂽ���ǂ����𒲂ׂ�
       * @retval true   �^�b�`�������ꂽ
       * @retval false  �^�b�`��������Ă��Ȃ�
       */
      bool IsTouchRelease( void ) const;

      /**
       * @brief �^�b�`�̃��s�[�g���͂����o�������ǂ����𒲂ׂ�
       * @retval true   ���s�[�g���͂����o����
       * @retval false  ���s�[�g���͂����o���Ă��Ȃ�
       */
      bool IsTouchRepeat( void ) const;

      /**
       * @brief �_�u���N���b�N�����o�������ǂ����𒲂ׂ�
       * @retval true   ���o����
       * @retval false  ���o���Ă��Ȃ�
       */
      bool IsDoubleClick( void ) const; 

      //-----------------------------------------------------------------------
      // ���݃^�b�`����Ă�����W�̎擾
      //-----------------------------------------------------------------------
      /**
       * @brief �^�b�`����Ă���x���W���擾����
       * @retval ���݃^�b�`����Ă���x���W( �^�b�`����Ă��Ȃ��ꍇ�� 0 )
       */
      u16 GetX( void ) const;

      /**
       * @brief �^�b�`����Ă���y���W���擾����
       * @retval ���݃^�b�`����Ă���y���W( �^�b�`����Ă��Ȃ��ꍇ�� 0 )
       */
      u16 GetY( void ) const;

      /**
       * @brief �^�b�`����Ă�����W���擾����
       * @param[out] out_x  �擾����x���W�̊i�[��
       * @param[out] out_y  �擾����y���W�̊i�[��
       */
      void GetXY( u16* out_x, u16* out_y ) const;

      /**
       * @brief �^�b�`����Ă�����W���擾����
       * @param[out] vec  �擾�������W�̊i�[��
       *
       * @note vec->z �͕ύX���܂���B
       */
      void GetXY( gfl::math::VEC3* vec ) const;

      //-----------------------------------------------------------------------
      // �O��^�b�`����Ă������W�̎擾
      //-----------------------------------------------------------------------
      /**
       * @brief �O��^�b�`���ꂽx���W���擾����
       * @retval ���݃^�b�`����Ă���x���W( �^�b�`����Ă��Ȃ��ꍇ�� 0 )
       */
      u16 GetPrevX( void ) const;

      /**
       * @brief �O��^�b�`���ꂽy���W���擾����
       * @retval ���݃^�b�`����Ă���y���W( �^�b�`����Ă��Ȃ��ꍇ�� 0 )
       */
      u16 GetPrevY( void ) const;

      /**
       * @brief �O��^�b�`���ꂽ���W���擾����
       * @param[out] out_x  �擾����x���W�̊i�[��
       * @param[out] out_y  �擾����y���W�̊i�[��
       */
      void GetPrevXY( u16* out_x, u16* out_y ) const;

      /**
       * @brief �O��^�b�`���ꂽ���W���擾����
       * @param[out] vec  �擾�������W�̊i�[��
       *
       * @note vec->z �͕ύX���܂���B
       */
      void GetPrevXY( gfl::math::VEC3* vec ) const;

      //-----------------------------------------------------------------------
      // �^�b�`���W�̕ω��ʂ��擾����
      //-----------------------------------------------------------------------
      /**
       * @brief �^�b�`���W��x�ω��ʂ��擾����
       * @return �O�񂩂��x���W�̕ω���
       *
       * @note
       * �^�b�`����Ă��Ȃ��Ԃ̓��͍͂��W(0,0)�Ƃ��Ĉ����Ă��܂�.
       * �����, �^�b�`���ꂽ�u�Ԃ�(0,0)����̕ω���(=���ݒl)���Ԃ�܂�.
       */
      int GetMoveX( void ) const;

      /**
       * @brief �^�b�`���W��y�ω��ʂ��擾����
       * @return �O�񂩂��y���W�̕ω���
       *
       * @note
       * �^�b�`����Ă��Ȃ��Ԃ̓��͍͂��W(0,0)�Ƃ��Ĉ����Ă��܂�.
       * �����, �^�b�`���ꂽ�u�Ԃ�(0,0)����̕ω���(=���ݒl)���Ԃ�܂�.
       */
      int GetMoveY( void ) const;

      /**
       * @brief �^�b�`���W�̕ω��ʂ��擾����
       * @param[out] out_x  x�����̕ω��ʂ̊i�[��
       * @param[out] out_y  y�����̕ω��ʂ̊i�[��
       *
       * @note
       * �^�b�`����Ă��Ȃ��Ԃ̓��͍͂��W(0,0)�Ƃ��Ĉ����Ă��܂�.
       * �����, �^�b�`���ꂽ�u�Ԃ�(0,0)����̕ω���(=���ݒl)���Ԃ�܂�.
       */
      void GetMoveXY( int* out_x, int* out_y ) const;

      /**
       * @brief �^�b�`���W�̕ω��ʂ��擾����
       * @param vec  �擾�����ω��ʂ̊i�[��
       *
       * @note
       * �^�b�`����Ă��Ȃ��Ԃ̓��͍͂��W(0,0)�Ƃ��Ĉ����Ă��܂�.
       * �����, �^�b�`���ꂽ�u�Ԃ�(0,0)����̕ω���(=���ݒl)���Ԃ�܂�.
       */
      void GetMoveXY( gfl::math::VEC3* vec ) const; 

      //-----------------------------------------------------------------------
      // �_�u���N���b�N�̐ݒ�
      //-----------------------------------------------------------------------
     
      // �_�u���N���b�N�Ƃ݂Ȃ��̈�̌`
      enum DoubleClickRange {
        DCLICK_RANGE_RECT,    // ��`( �f�t�H���g )
        DCLICK_RANGE_CIRCLE,  // �~�`
      };

      // �_�u���N���b�N�̔������
      struct DoubleClickCond 
      {
        DoubleClickRange range;  // ����̈�̌`
        u32 radius;              // ����̈�̑傫��( RECT:��ӂ̒�����1/2, CIRCLE:���a )
        u32 frame;               // �N���b�N�Ԃ̃t���[����
      };

      // �f�t�H���g�̐ݒ�
      static const u32 DEFAULT_DCLICK_RADIUS = 20;
      static const u32 DEFAULT_DCLICK_FRAME  = 20; 
      static const DoubleClickCond DEFAULT_DCLICK_COND;

      /**
       * @brief �_�u���N���b�N�̏�����ݒ肷��
       * @param cond �ݒ肷�����
       */
      void SetDoubleClickCond( const DoubleClickCond& cond );

      /**
       * @brief �_�u���N���b�N�̏������擾����
       */
      const DoubleClickCond& GetDoubleClickCond( void ) const;



      protected: 

      //-----------------------------------------------------------------------
      // �R���X�g���N�^�E�f�X�g���N�^
      //-----------------------------------------------------------------------
      /**
       * @brief �R���X�g���N�^
       * @param heap         �g�p����q�[�v
       * @param implementer  ���ۂɃ^�b�`�p�l�����͂̓ǂݎ����s���C���X�^���X
       */
      TouchPanel( gfl::heap::HeapBase* heap, DeviceImplementer* implementer );

      /**
       * @brief �f�X�g���N�^
       */
      virtual ~TouchPanel(); 

      //-----------------------------------------------------------------------
      // �f�o�C�X�Ƃ��Ă̓�����`���郁�\�b�h
      //-----------------------------------------------------------------------
      /**
       * @brief ���݂̓��͂ƑO��̓��͂��r��, �����f�[�^�������Ă��邩�ǂ����𒲂ׂ�
       * @retval true   �O��ƍ���̓��͓͂���
       * @retval false  �O��ƍ���̓��͈͂قȂ�
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

      //-----------------------------------------------------------------------
      // �^�b�`�p�l���ŗL�̍X�V����
      //-----------------------------------------------------------------------
      /**
       * @brief �X�V����
       */
      void UpdateTouchPanel( void );



      private: 

      struct DoubleClickState 
      {
        bool detected;   // �_�u���N���b�N�����o�������H
        bool first;      // 1�N���b�N�ڂ����o�ς݂��H
        u16 first_x;     // 1�N���b�N�ڂ�x���W
        u16 first_y;     // 1�N���b�N�ڂ�y���W
        u32 count;       // �_�u���N���b�N�Ƃ݂Ȃ��c��t���[����
      };

      DoubleClickCond m_dclick_cond;
      DoubleClickState m_dclick_state;

      void UpdateDClickState( void );
      bool CheckDClickRange( u16 x, u16 y ) const; 
      bool m_update_flag;//30,60�����肩���邽�߂̃t���O

      const EffectiveData& GetEffectiveData( void ) const;

    };


  }  //namespace ui
}  //namespace gfl

#endif //__GFL_UI_TOUCHPANEL_H__

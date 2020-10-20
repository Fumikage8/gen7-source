//=============================================================================
/**
 * @file   gfl_UI_Button.cpp
 * @brief  �{�^���̃A�N�Z�b�T
 * @author obata_toshihiro
 * @date   2010.10.22
 */
//=============================================================================
#include <nn.h>
#include "gfl_Macros.h"
#include "ui/gfl_UI_Button.h"
#include "./gfl_UI_DeviceImplementer.h"
#include "./gfl_UI_RepeatTimer.h"

namespace gfl {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     * @param heap         �g�p����q�[�v
     * @param implementer  ���ۂɓ��͂�ǂݎ��C���X�^���X
     */
    //-------------------------------------------------------------------------
    Button::Button( gfl::heap::HeapBase* heap, DeviceImplementer* implementer ) :
      Device( heap, implementer ),
      m_buttonInfo()
    {
      this->InitButtonAssignment();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    Button::~Button()
    {
    }




    //-------------------------------------------------------------------------
    /**
     * @brief �w�肵���{�^����������Ă��邩�ǂ������ׂ�
     * @param button       ���ׂ�{�^�� ( gfl::ui::BUTTON_xxxx )
     * @param targetState  �`�F�b�N�Ώۂ̓��͏��
     * @retval true   ������Ă���
     * @retval false  ������Ă��Ȃ�
     */
    //-------------------------------------------------------------------------
    bool Button::IsHold( u32 button, InputStateID targetState ) const
    {
      const u32 hold = this->GetHold( targetState );
      return ( hold & button ) != 0;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �w�肵���{�^���������ꂽ���ǂ����𒲂ׂ�
     * @param button       ���ׂ�{�^�� ( gfl::ui::BUTTON_xxxx )
     * @param targetState  �`�F�b�N�Ώۂ̓��͏��
     * @retval true   �����ꂽ
     * @retval false  ������Ă��Ȃ�
     */
    //-------------------------------------------------------------------------
    bool Button::IsTrigger( u32 button, InputStateID targetState ) const
    {
      const u32 trigger = this->GetTrigger( targetState );
      return ( trigger & button ) != 0;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �w�肵���{�^���������ꂽ���ǂ����𒲂ׂ�
     * @param button       ���ׂ�{�^�� ( gfl::ui::BUTTON_xxxx )
     * @param targetState  �`�F�b�N�Ώۂ̓��͏��
     * @retval true   �����ꂽ
     * @retval false  ������Ă��Ȃ�
     */
    //-------------------------------------------------------------------------
    bool Button::IsRelease( u32 button, InputStateID targetState ) const
    {
      const u32 release = this->GetRelease( targetState );
      return ( release & button ) != 0;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �w�肵���{�^���������ꑱ���Ă��邩�ǂ����𒲂ׂ�
     * @param button       ���ׂ�{�^�� ( gfl::ui::BUTTON_xxxx )
     * @param targetState  �`�F�b�N�Ώۂ̓��͏��
     * @retval true   �����ꑱ���Ă���, ���s�[�g�^�C�~���O�����o����
     * @retval false  �����ꑱ���Ă��Ȃ�, �܂��̓��s�[�g�^�C�~���O�łȂ�
     */
    //-------------------------------------------------------------------------
    bool Button::IsRepeat( u32 button, InputStateID targetState ) const
    {
      const u32 repeat = this->GetRepeat( targetState );
      return ( repeat & button ) != 0;
    }




    //-------------------------------------------------------------------------
    /**
     * @brief ������Ă���{�^���̏����擾����
     * @param targetState  �`�F�b�N�Ώۂ̓��͏��
     * @return ������Ă���S�Ẵ{�^��( gfl::ui::BUTTON_xxxx )�̘_���a
     */
    //-------------------------------------------------------------------------
    u32 Button::GetHold( InputStateID targetState ) const
    {
      const InputState& state = this->GetEffectiveInputState( targetState );
      return state.hold;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����ꂽ�{�^���̏����擾����
     * @param targetState  �`�F�b�N�Ώۂ̓��͏��
     * @return �����ꂽ�S�Ẵ{�^��( gfl::ui::BUTTON_xxxx )�̘_���a
     */
    //-------------------------------------------------------------------------
    u32 Button::GetTrigger( InputStateID targetState ) const
    {
      const InputState& state = this->GetEffectiveInputState( targetState );
      return state.trigger;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �����ꂽ���{�^���̏����擾����
     * @param targetState  �`�F�b�N�Ώۂ̓��͏��
     * @return �����ꂽ�S�Ẵ{�^��( gfl::ui::BUTTON_xxxx )�̘_���a
     */
    //-------------------------------------------------------------------------
    u32 Button::GetRelease( InputStateID targetState ) const
    {
      const InputState& state = this->GetEffectiveInputState( targetState );
      return state.release;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���s�[�g�����o�����{�^���̏����擾����
     * @param targetState  �`�F�b�N�Ώۂ̓��͏��
     * @return ���s�[�g�����o�����S�Ẵ{�^��( gfl::ui::BUTTON_xxxx )�̘_���a
     */
    //-------------------------------------------------------------------------
    u32 Button::GetRepeat( InputStateID targetState ) const
    {
      const InputState& state = this->GetEffectiveInputState( targetState );
      return ( state.trigger | state.repeat );  // ���s�[�g�̓g���K���܂�
    }

    /**
     * @brief ���ۂɎQ�Ƃ��ׂ����͏�Ԃ��擾����
     * @param targetState  �`�F�b�N�Ώۂ̓��͏��
     */
    const Button::InputState& Button::GetEffectiveInputState( InputStateID targetState ) const
    {
      const EffectiveData& data = this->GetEffectiveData();

      if( INPUT_STATE_NUM <= targetState ) {
        GFL_ASSERT(0); //@check
        return data.inputState[ INPUT_STATE_ASSIGNED ];
      } 
      else {
        return data.inputState[ targetState ];
      }
    }

    /**
     * @brief ���ۂɎQ�Ƃ��ׂ����s�f�[�^���擾����
     */
    const Button::EffectiveData& Button::GetEffectiveData( void ) const
    {
      const void* data = this->GetRunningEffectiveData();
      const EffectiveData* effective_data = reinterpret_cast<const EffectiveData*>( data );
      return *effective_data;
    }




    //-------------------------------------------------------------------------
    /**
     * @brief ���݂̌��o�f�[�^�ƑO��̌��o�f�[�^���r��, �����f�[�^�������Ă��邩�ǂ����𒲂ׂ�
     * @retval true   �O��̌��o�f�[�^�ƍ���̌��o�f�[�^������
     * @retval false  �O��̌��o�f�[�^�ƍ���̌��o�f�[�^���قȂ�
     */
    //-------------------------------------------------------------------------
    bool Button::IsRepeatingSameInput( void ) const
    {
      const DetectiveData* now_data  = reinterpret_cast<DetectiveData*>(m_nowDetectiveData);
      const DetectiveData* prev_data = reinterpret_cast<DetectiveData*>(m_prevDetectiveData);

      if( ( now_data->hold != 0 ) && 
          ( now_data->hold == prev_data->hold ) )
      {
        return true;
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����f�[�^��~�ς���
     * @param[out] buffer               �f�[�^�̊i�[��
     * @param[in]  detective_data       ����̌��o�f�[�^
     * @param[in]  prev_detective_data  �O��̌��o�f�[�^
     */
    //-------------------------------------------------------------------------
    void Button::StoreEffectiveData( 
        void* buffer,
        const void* detective_data,
        const void* prev_detective_data ) const
    {
      const DetectiveData* now_data = reinterpret_cast<const DetectiveData*>( detective_data ); 
      const DetectiveData* prev_data = reinterpret_cast<const DetectiveData*>( prev_detective_data ); 

      DetectiveData assigned_now_data, assigned_prev_data;
      this->CalcAssignedDetectiveData( &assigned_now_data, *now_data );
      this->CalcAssignedDetectiveData( &assigned_prev_data, *prev_data );

      EffectiveData* effective_data = reinterpret_cast<EffectiveData*>( buffer ); 
      for( u32 i=0; i<BUTTON_NUM; i++ ) {
        const u32 button_bit = ( 1 << i );
        this->CalcInputState( &(effective_data->inputState[ INPUT_STATE_ASSIGNED ]), assigned_now_data.hold, assigned_prev_data.hold, button_bit );
        this->CalcInputState( &(effective_data->inputState[ INPUT_STATE_ORIGINAL ]), now_data->hold, prev_data->hold, button_bit );
      }
    }

    /**
     * @brief ���o�f�[�^�ɑ΂�, �{�^���̊��蓖�ď���K�p����
     * @param[out] pDestData  �{�^�����蓖�ď���K�p�����f�[�^
     * @param[in]  srcData    �{�^�����蓖�ď���K�p���錳�f�[�^
     */
    void Button::CalcAssignedDetectiveData( DetectiveData* pDestData, const DetectiveData& srcData ) const
    {
      pDestData->hold = 0;

      for( u32 i=0; i<BUTTON_NUM; i++ )
      {
        const u32 button_bit = ( 1 << i );
        if( srcData.hold & m_buttonInfo[i].assignedButton ) {
          pDestData->hold |= (1<<i);
        }
      }
    }

    /**
     * @brief �O��ƍŐV�̓��͂���, ���͏�Ԃ��쐬����
     * @param[out] buffer     �쐬�����f�[�^�̊i�[��
     * @param[in]  now_hold   ����̃z�[���h���
     * @param[in]  prev_hold  �O��̃z�[���h���
     * @param[in]  button     ��������{�^��
     */
    void Button::CalcInputState( InputState* buffer, u32 now_hold, u32 prev_hold, u32 button ) const
    {
      bool hold    = ( (               now_hold & button ) != 0 );
      bool trigger = ( ( ~prev_hold &  now_hold & button ) != 0 );
      bool release = ( (  prev_hold & ~now_hold & button ) != 0 );
      bool repeat  = m_pRepeatTimer->IsRepeat() && hold;

      if( release )
      {
        if( buffer->trigger & button )
        {
          // �O�t���[���̃g���K�𖳌���
          buffer->hold    &= ~button;
          buffer->trigger &= ~button;
          buffer->release &= ~button;
          buffer->repeat  &= ~button;
        }
        else
        {
          // �����[�X���ꂽ
          buffer->hold    &= ~button;
          buffer->trigger &= ~button;
          buffer->release |=  button;
          buffer->repeat  &= ~button;
        }
      }
      else if( ( buffer->release & button ) && trigger )
      {
        // �O�t���[���̃����[�X�𖳌���
        buffer->hold    |=  button;
        buffer->trigger &= ~button;
        buffer->release &= ~button;

        if( repeat ) {
          buffer->repeat |= button;
        }
        else {
          buffer->repeat &= ~button;
        }
      }
      else
      {
        buffer->hold    |= ( hold ) ? ( button ) : ( 0 );
        buffer->trigger |= ( trigger ) ? ( button ) : ( 0 );
        buffer->release |= ( release ) ? ( button ) : ( 0 );
        buffer->repeat  |= ( repeat ) ? ( button ) : ( 0 );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �{�^�����蓖�ď���ǉ�����
     * @param targetButton  ����Ώۂ̃{�^��
     * @param assignButton  ���蓖�Ă�{�^��
     *
     * @note AddButtonAssignment( gfl::ui::BUTTONID_A, gfl::ui::BUTTONID_L ) �Ƃ����ꍇ,
     *       L�{�^����������, A�{�^���������ꂽ����ɂȂ�܂�.
     */
    //-------------------------------------------------------------------------
    void Button::AddButtonAssignment( ButtonID targetButton, ButtonID assignButton )
    {
      m_buttonInfo[ targetButton ].assignedButton |= (1<<assignButton);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �{�^�����蓖�ď�����������
     * @param targetButton  ����Ώۂ̃{�^��
     * @param removeButton  ���蓖�Ă���������{�^��
     *
     * @note RemoveButtonAssignment( gfl::ui::BUTTONID_A, gfl::ui::BUTTONID_L ) �Ƃ����ꍇ,
     *       L�{�^���������Ă�, A�{�^���������ꂽ����ɂȂ�Ȃ��Ȃ�܂�.
     */
    //-------------------------------------------------------------------------
    void Button::RemoveButtonAssignment( ButtonID targetButton, ButtonID removeButton )
    {
      m_buttonInfo[ targetButton ].assignedButton &= ~(1<<removeButton);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �{�^�����蓖�ď�������������
     */
    //-------------------------------------------------------------------------
    void Button::InitButtonAssignment( void )
    {
      for( u32 i=0; i<BUTTON_NUM; i++ ) {
        m_buttonInfo[i].assignedButton = ( 1 << i );
      }
    }



  } // namespace ui
} // namespace gfl

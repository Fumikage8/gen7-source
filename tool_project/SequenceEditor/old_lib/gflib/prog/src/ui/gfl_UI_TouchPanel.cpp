//=============================================================================
/**
 * @file gfl_UI_TouchPanel.cpp
 * @brief �^�b�`�p�l���̃A�N�Z�b�T
 * @author toshihiro_obata
 * @date 2010.10.22
 */
//=============================================================================
#include <nn.h>
#include "gfl_Macros.h"
#include "gfl_Math.h"
#include "ui/gfl_UI_TouchPanel.h"
#include "./gfl_UI_DeviceImplementer.h"
#include "./gfl_UI_RepeatTimer.h"

namespace gfl {
  namespace ui {


    // �_�u���N���b�N����̃f�t�H���g�ݒ�
    const TouchPanel::DoubleClickCond TouchPanel::DEFAULT_DCLICK_COND = 
    {
      DCLICK_RANGE_RECT,
      DEFAULT_DCLICK_RADIUS,
      DEFAULT_DCLICK_FRAME,
    };




    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     * @param heap         �g�p����q�[�v
     * @param implementer  ���ۂɃ^�b�`�p�l�����͂̓ǂݎ����s���C���X�^���X
     */
    //-------------------------------------------------------------------------
    TouchPanel::TouchPanel( 
        gfl::heap::HeapBase* heap, DeviceImplementer* implementer ) :
      Device( heap, implementer ),
      m_dclick_cond( DEFAULT_DCLICK_COND ),
      m_dclick_state(),
      m_update_flag(true)
    {
      m_dclick_state.detected = false;
      m_dclick_state.first = false;
      m_dclick_state.first_x = 0;
      m_dclick_state.first_y = 0;
      m_dclick_state.count = 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    TouchPanel::~TouchPanel()
    {
    }




    //-------------------------------------------------------------------------
    /**
     * @brief �^�b�`����Ă���x���W���擾����
     * @retval ���݃^�b�`����Ă���x���W( �^�b�`����Ă��Ȃ��ꍇ�� 0 )
     */
    //-------------------------------------------------------------------------
    u16 TouchPanel::GetX( void ) const 
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.last_x;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �^�b�`����Ă���y���W���擾����
     * @retval ���݃^�b�`����Ă���y���W( �^�b�`����Ă��Ȃ��ꍇ�� 0 )
     */
    //-------------------------------------------------------------------------
    u16 TouchPanel::GetY( void ) const 
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.last_y;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �^�b�`����Ă�����W���擾����
     * @param[out] out_x  �擾����x���W�̊i�[��
     * @param[out] out_y  �擾����y���W�̊i�[��
     */
    //-------------------------------------------------------------------------
    void TouchPanel::GetXY( u16* out_x, u16* out_y ) const
    {
      *out_x = this->GetX();
      *out_y = this->GetY();
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �^�b�`����Ă�����W���擾����
     * @param[out] vec  �擾�������W�̊i�[��
     *
     * @note vec->z �͕ύX���܂���B
     */
    //-------------------------------------------------------------------------
    void TouchPanel::GetXY( gfl::math::VEC3* vec ) const
    {
      vec->x = this->GetX();
      vec->y = this->GetY();
    } 



    //-------------------------------------------------------------------------
    /**
     * @brief �O��^�b�`���ꂽx���W���擾����
     * @retval ���݃^�b�`����Ă���x���W( �^�b�`����Ă��Ȃ��ꍇ�� 0 )
     */
    //-------------------------------------------------------------------------
    u16 TouchPanel::GetPrevX( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.prev_x;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �O��^�b�`���ꂽy���W���擾����
     * @retval ���݃^�b�`����Ă���y���W( �^�b�`����Ă��Ȃ��ꍇ�� 0 )
     */
    //-------------------------------------------------------------------------
    u16 TouchPanel::GetPrevY( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.prev_y;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �O��^�b�`���ꂽ���W���擾����
     * @param[out] out_x  �擾����x���W�̊i�[��
     * @param[out] out_y  �擾����y���W�̊i�[��
     */
    //-------------------------------------------------------------------------
    void TouchPanel::GetPrevXY( u16* out_x, u16* out_y ) const
    {
      *out_x = this->GetPrevX();
      *out_y = this->GetPrevY();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �O��^�b�`���ꂽ���W���擾����
     * @param[out] vec  �擾�������W�̊i�[��
     *
     * @note vec->z �͕ύX���܂���B
     */
    //-------------------------------------------------------------------------
    void TouchPanel::GetPrevXY( gfl::math::VEC3* vec ) const
    {
      vec->x = this->GetPrevX();
      vec->y = this->GetPrevY();
    }




    //-------------------------------------------------------------------------
    /**
     * @brief �^�b�`���W��x�ω��ʂ��擾����
     * @return �O�񂩂��x���W�̕ω���
     *
     * @note
     * �^�b�`����Ă��Ȃ��Ԃ̓��͍͂��W(0,0)�Ƃ��Ĉ����Ă��܂�.
     * �����, �^�b�`���ꂽ�u�Ԃ�(0,0)����̕ω���(=���ݒl)���Ԃ�܂�.
     */
    //-------------------------------------------------------------------------
    int TouchPanel::GetMoveX( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.last_x - data.prev_x;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �^�b�`���W��y�ω��ʂ��擾����
     * @return �O�񂩂��y���W�̕ω���
     *
     * @note
     * �^�b�`����Ă��Ȃ��Ԃ̓��͍͂��W(0,0)�Ƃ��Ĉ����Ă��܂�.
     * �����, �^�b�`���ꂽ�u�Ԃ�(0,0)����̕ω���(=���ݒl)���Ԃ�܂�.
     */
    //-------------------------------------------------------------------------
    int TouchPanel::GetMoveY( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.last_y - data.prev_y;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �^�b�`���W�̕ω��ʂ��擾����
     * @param[out] out_x  x�����̕ω��ʂ̊i�[��
     * @param[out] out_y  y�����̕ω��ʂ̊i�[��
     *
     * @note
     * �^�b�`����Ă��Ȃ��Ԃ̓��͍͂��W(0,0)�Ƃ��Ĉ����Ă��܂�.
     * �����, �^�b�`���ꂽ�u�Ԃ�(0,0)����̕ω���(=���ݒl)���Ԃ�܂�.
     */
    //-------------------------------------------------------------------------
    void TouchPanel::GetMoveXY( int* out_x, int* out_y ) const
    {
      *out_x = this->GetMoveX();
      *out_y = this->GetMoveY();
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �^�b�`���W�̕ω��ʂ��擾����
     * @param vec  �擾�����ω��ʂ̊i�[��
     *
     * @note
     * �^�b�`����Ă��Ȃ��Ԃ̓��͍͂��W(0,0)�Ƃ��Ĉ����Ă��܂�.
     * �����, �^�b�`���ꂽ�u�Ԃ�(0,0)����̕ω���(=���ݒl)���Ԃ�܂�.
     */
    //-------------------------------------------------------------------------
    void TouchPanel::GetMoveXY( gfl::math::VEC3* vec ) const
    {
      vec->x = this->GetMoveX();
      vec->y = this->GetMoveY();
    }






    //-------------------------------------------------------------------------
    /**
     * @brief �^�b�`����Ă��邩�ǂ����𒲂ׂ�
     * @retval true   �^�b�`����Ă���
     * @retval false  �^�b�`����Ă��Ȃ�
     */
    //-------------------------------------------------------------------------
    bool TouchPanel::IsTouch( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.hold;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �^�b�`���ꂽ���ǂ����𒲂ׂ�
     * @retval true   �^�b�`���ꂽ
     * @retval false  �^�b�`����ĂȂ�
     */
    //-------------------------------------------------------------------------
    bool TouchPanel::IsTouchTrigger( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.trigger;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �^�b�`�������ꂽ���ǂ����𒲂ׂ�
     * @retval true   �^�b�`�������ꂽ
     * @retval false  �^�b�`��������Ă��Ȃ�
     */
    //-------------------------------------------------------------------------
    bool TouchPanel::IsTouchRelease( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.release;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �^�b�`�̃��s�[�g���͂����o�������ǂ����𒲂ׂ�
     * @retval true   ���s�[�g���͂����o����
     * @retval false  ���s�[�g���͂����o���Ă��Ȃ�
     */
    //-------------------------------------------------------------------------
    bool TouchPanel::IsTouchRepeat( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return ( data.repeat || data.trigger );  // ���s�[�g�̓g���K���܂�
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �_�u���N���b�N�����o�������ǂ����𒲂ׂ�
     * @retval true   ���o����
     * @retval false  ���o���Ă��Ȃ�
     */
    //-------------------------------------------------------------------------
    bool TouchPanel::IsDoubleClick( void ) const
    {
      return m_dclick_state.detected;
    }

    /**
     * @brief ���ۂɎQ�Ƃ��ׂ����s�f�[�^���擾����
     */
    const TouchPanel::EffectiveData& TouchPanel::GetEffectiveData( void ) const
    {
      const void* data = this->GetRunningEffectiveData();
      const EffectiveData* effective_data = reinterpret_cast<const EffectiveData*>( data );
      return *effective_data;
    }





    //-------------------------------------------------------------------------
    /**
     * @brief �^�b�`�p�l���ŗL�̍X�V����
     */
    //-------------------------------------------------------------------------
    void TouchPanel::UpdateTouchPanel( void )
    {
      if( this->GetFrameRate() == FRAME_RATE_30 )
      {
        m_update_flag ^= 1;
      }
      else
      {
        m_update_flag = 1;
      }
        
      if( m_update_flag )
      {
        this->UpdateDClickState();
      }
    }

    /**
     * @brief �_�u���N���b�N�̏�Ԃ��X�V����
     */
    void TouchPanel::UpdateDClickState( void )
    {
      m_dclick_state.detected = false;

      // �c�莞�Ԃ��f�N�������g
      if( m_dclick_state.first ) {
        --m_dclick_state.count;
        if( m_dclick_state.count <= 0 ) {
          m_dclick_state.first = false;
        }
      }

      if( this->IsTouchTrigger() ) 
      {
        u16 touch_x = 0;
        u16 touch_y = 0;
        this->GetXY( &touch_x, &touch_y );

        if( m_dclick_state.first ) {  // 2�N���b�N��
          if( this->CheckDClickRange( touch_x, touch_y ) ) {
            m_dclick_state.detected = true;
            m_dclick_state.first = false;
          }
        }
        else { // 1�N���b�N�ڂ����o
          m_dclick_state.first = true;
          m_dclick_state.count = m_dclick_cond.frame;
          m_dclick_state.first_x = touch_x; 
          m_dclick_state.first_y = touch_y;
        }
      }
    }

    /**
     * @brief �_�u���N���b�N�͈̔̓`�F�b�N
     * @param x  �`�F�b�N����x���W
     * @param y  �`�F�b�N����y���W
     * @retval true   �͈͓�
     * @retval false  �͈͊O
     */
    bool TouchPanel::CheckDClickRange( u16 x, u16 y ) const
    {
      s16 diff_x = x - m_dclick_state.first_x;
      s16 diff_y = y - m_dclick_state.first_y;

      if( m_dclick_cond.range == DCLICK_RANGE_RECT )
      {
        return ( 
            ( gfl::math::Abs(diff_x) <= m_dclick_cond.radius ) &&
            ( gfl::math::Abs(diff_y) <= m_dclick_cond.radius ) );
      }
      else
      {
        u32 dist = ( diff_x * diff_x ) + ( diff_y + diff_y );
        u32 min = m_dclick_cond.radius * m_dclick_cond.radius;
        return ( dist <= min );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �_�u���N���b�N�Ƃ݂Ȃ��^�b�`�Ԃ̃t���[������ݒ肷��
     * @param cond �ݒ肷�����
     */
    //-------------------------------------------------------------------------
    void TouchPanel::SetDoubleClickCond( const DoubleClickCond& cond )
    {
      m_dclick_cond = cond;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �_�u���N���b�N�Ƃ݂Ȃ��^�b�`�Ԃ̃t���[�������擾����
     */
    //-------------------------------------------------------------------------
    const TouchPanel::DoubleClickCond& TouchPanel::GetDoubleClickCond( void ) const
    {
      return m_dclick_cond;
    }





    //-------------------------------------------------------------------------
    /**
     * @brief ���݂̓��͂ƑO��̓��͂��r��, �����f�[�^�������Ă��邩�ǂ����𒲂ׂ�
     * @retval true   �O��ƍ���̓��͓͂���
     * @retval false  �O��ƍ���̓��͈͂قȂ�
     */
    //-------------------------------------------------------------------------
    bool TouchPanel::IsRepeatingSameInput( void ) const
    {
      const DetectiveData* now_data  = reinterpret_cast<DetectiveData*>(m_nowDetectiveData);
      const DetectiveData* prev_data = reinterpret_cast<DetectiveData*>(m_prevDetectiveData);
      return ( now_data->touch && prev_data->touch );  // �^�b�`�������Ă���΃��s�[�g�Ƃ���
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �����f�[�^��~�ς���
     * @param buffer               �f�[�^�̊i�[��
     * @param detective_data       ����̌��o�f�[�^
     * @param prev_detective_data  �O��̌��o�f�[�^
     */
    //-------------------------------------------------------------------------
    void TouchPanel::StoreEffectiveData( 
        void* buffer, const void* detective_data, const void* prev_detective_data ) const
    {
      const DetectiveData* now_data = reinterpret_cast<const DetectiveData*>( detective_data ); 
      const DetectiveData* prev_data = reinterpret_cast<const DetectiveData*>( prev_detective_data ); 
      EffectiveData* buffer_data = reinterpret_cast<EffectiveData*>( buffer );

      if( now_data->touch ) {
        buffer_data->last_x = now_data->x;
        buffer_data->last_y = now_data->y;
      } 

      if( prev_data->touch ) {
        buffer_data->prev_x = prev_data->x;
        buffer_data->prev_y = prev_data->y;
      }

      bool release = (   prev_data->touch  & (~now_data->touch) );
      bool trigger = ( (~prev_data->touch) &   now_data->touch );

      if( buffer_data->trigger && release )
      {
        // �O�t���[���� trigger �𖳌���
        buffer_data->hold    = false;
        buffer_data->trigger = false;
        buffer_data->release = false;
        buffer_data->repeat  = false;
      }
      else if( buffer_data->release && trigger )
      {
        // �O�t���[���� release �𖳌���
        buffer_data->hold    = true;
        buffer_data->trigger = false;
        buffer_data->release = false;
        buffer_data->repeat  = m_pRepeatTimer->IsRepeat();
      }
      else
      {
        buffer_data->hold     = now_data->touch;
        buffer_data->trigger |= ( (~prev_data->touch) & now_data->touch );
        buffer_data->release |= ( prev_data->touch & (~now_data->touch) );
        buffer_data->repeat  |= m_pRepeatTimer->IsRepeat() ? now_data->touch : 0;
      }
    }



  } // namespace ui
} // namespace gfl

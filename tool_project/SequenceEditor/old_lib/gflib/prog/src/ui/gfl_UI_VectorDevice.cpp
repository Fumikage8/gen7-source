//=============================================================================
/**
 * @file   gfl_UI_VectorDevice.cpp
 * @brief  �������̓f�o�C�X�̃A�N�Z�b�T
 * @author obata_toshihiro
 * @date   2010.10.21
 */
//=============================================================================
#include <nn.h>
#include "gfl_Macros.h"
#include "ui/gfl_UI_VectorDevice.h"
#include "./gfl_UI_RepeatTimer.h"
#include "./gfl_UI_DeviceImplementerForVectorDevice.h"


namespace gfl {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     *
     * @param heap         �g�p����q�[�v
     * @param implementer  ���ۂɓ��͂̓ǂݎ����s���C���X�^���X
     */
    //-------------------------------------------------------------------------
    VectorDevice::VectorDevice( 
        gfl::heap::HeapBase* heap, DeviceImplementerForVectorDevice* implementer ) :
      Device( heap, implementer ),
      m_pImplementer( implementer )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    VectorDevice::~VectorDevice()
    {
    }




    //--------------------------------------------------------------------------
    /**
     * @brief �A�i���O�X�e�B�b�N�̃N�����v���[�h���擾����
     */
    //--------------------------------------------------------------------------
    VectorDevice::StickClampMode VectorDevice::GetStickClampMode( void ) const
    {
      return m_pImplementer->GetStickClampMode();
    }

    //--------------------------------------------------------------------------
    /**
     * @brief �A�i���O�X�e�B�b�N�̃N�����v���[�h��ݒ肷��
     * @param clamp_mode �ݒ肷��N�����v���[�h
     */
    //--------------------------------------------------------------------------
    void VectorDevice::SetStickClampMode( VectorDevice::StickClampMode clamp_mode )
    {
      m_pImplementer->SetStickClampMode( clamp_mode );
    }

    //--------------------------------------------------------------------------
    /**
     * @brief �A�i���O�X�e�B�b�N�̃N�����v�p�����[�^���擾����
     *
     * @param[out] min  �ŏ��N�����v�l�̊i�[��
     * @param[out] max  �ő�N�����v�l�̊i�[��
     */
    //--------------------------------------------------------------------------
    void VectorDevice::GetStickClampParam( s16* min, s16* max ) const
    {
      m_pImplementer->GetStickClampParam( min, max );
    }

    //--------------------------------------------------------------------------
    /**
     * @brief �A�i���O�X�e�B�b�N�̃N�����v�p�����[�^��ݒ肷��
     *
     * @param[in] min  �ŏ��N�����v�l
     * @param[in] max  �ő�N�����v�l
     */
    //--------------------------------------------------------------------------
    void VectorDevice::SetStickClampParam( s16 min, s16 max )
    {
      m_pImplementer->SetStickClampParam( min, max );
    }





    //-------------------------------------------------------------------------
    /**
     * @brief ���͂��ꂽ�������擾����
     *
     * @return ���͕�����x����[-1, 1]
     */
    //-------------------------------------------------------------------------
    f32 VectorDevice::GetX( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.x;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���͂��ꂽ�������擾����
     *
     * @return ���͕�����y����[-1, 1]
     */
    //-------------------------------------------------------------------------
    f32 VectorDevice::GetY( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.y;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ���͂��ꂽ�������擾����
     *
     * @param dest_x  �擾����x����[-1, 1]�̊i�[��
     * @param dest_y  �擾����y����[-1, 1]�̊i�[��
     */
    //-------------------------------------------------------------------------
    void VectorDevice::GetVector( f32* dest_x, f32* dest_y ) const
    {
      *dest_x = this->GetX();
      *dest_y = this->GetY();
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ���͂��ꂽ�������擾����
     *
     * @param dest_vec  �擾�������͕����x�N�g���̊i�[��
     */
    //-------------------------------------------------------------------------
    void VectorDevice::GetVector( gfl::math::VEC2* dest_vec ) const
    {
      dest_vec->x = this->GetX();
      dest_vec->y = this->GetY();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���͂��ꂽ�������擾����
     *
     * @param dest_vec  �擾�������͕����x�N�g���̊i�[��
     */
    //-------------------------------------------------------------------------
    void VectorDevice::GetVector( gfl::math::VEC3* dest_vec ) const
    {
      dest_vec->x = this->GetX();
      dest_vec->y = this->GetY();
    }




    //-------------------------------------------------------------------------
    /**
     * @brief �w�肵�����������͂���Ă��邩�ǂ����𒲂ׂ�
     *
     * @param dir  ���肷�����( gfl::ui::DIR_xxxx )
     *
     * @retval true   ���͂���Ă���
     * @retval false  ���͂���Ă��Ȃ�
     */
    //-------------------------------------------------------------------------
    bool VectorDevice::IsHold( u32 dir ) const
    {
      return ( this->GetHold() & dir ) != 0;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �w�肵�����������͂��ꂽ���ǂ����𒲂ׂ�
     *
     * @param dir  ���肷�����( gfl::ui::DIR_xxxx )
     *
     * @retval true   ���͂��ꂽ
     * @retval false  ���͂���Ă��Ȃ�
     */
    //-------------------------------------------------------------------------
    bool VectorDevice::IsTrigger( u32 dir ) const
    {
      return ( this->GetTrigger() & dir ) != 0;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �w�肵�������̓��͂������Ȃ������ǂ����𒲂ׂ�
     *
     * @param dir  ���肷�����( gfl::ui::DIR_xxxx )
     *
     * @retval true   ���͂������Ȃ���
     * @retval false  ���͂������Ȃ��Ă��Ȃ�
     */
    //-------------------------------------------------------------------------
    bool VectorDevice::IsRelease( u32 dir ) const
    {
      return ( this->GetRelease() & dir ) != 0;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �w�肵�������̃��s�[�g�����o�������ǂ����𒲂ׂ�
     *
     * @param dir  ���肷�����( gfl::ui::DIR_xxxx )
     *
     * @retval true   ���s�[�g�����o����
     * @retval false  �����s�[�g�����o���Ă��Ȃ�
     */
    //-------------------------------------------------------------------------
    bool VectorDevice::IsRepeat( u32 dir ) const
    {
      return ( this->GetRepeat() & dir ) != 0;
    } 




    //-------------------------------------------------------------------------
    /**
     * @brief ������Ă�������̏����擾����
     * @return ������Ă���S�Ă̕���( gfl::ui::DIR_xxxx )�̘_���a
     */
    //-------------------------------------------------------------------------
    u32 VectorDevice::GetHold( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.hold;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����ꂽ�����̏����擾����
     * @return �����ꂽ�S�Ă̕���( gfl::ui::DIR_xxxx )�̘_���a
     */
    //-------------------------------------------------------------------------
    u32 VectorDevice::GetTrigger( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.trigger;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����ꂽ�������̏����擾����
     * @return �����ꂽ�S�Ă̕���( gfl::ui::DIR_xxxx )�̘_���a
     */
    //-------------------------------------------------------------------------
    u32 VectorDevice::GetRelease( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.release;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���s�[�g�����o���������̏����擾����
     * @return ���s�[�g�����o�����S�Ă̕���( gfl::ui::DIR_xxxx )�̘_���a
     */
    //-------------------------------------------------------------------------
    u32 VectorDevice::GetRepeat( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return ( data.trigger | data.repeat );  // ���s�[�g�̓g���K���܂�
    }

    /**
     * @brief ���ۂɎQ�Ƃ��ׂ����s�f�[�^���擾����
     */
    const VectorDevice::EffectiveData& VectorDevice::GetEffectiveData( void ) const
    {
      const void* data = this->GetRunningEffectiveData();
      const EffectiveData* effective_data = reinterpret_cast<const EffectiveData*>( data );
      return *effective_data;
    }






    //-------------------------------------------------------------------------
    /**
     * @brief �ŐV�̌��o�f�[�^������͕�����x����[-1, 1]���擾����
     */
    //-------------------------------------------------------------------------
    f32 VectorDevice::GetLatestX( void ) const
    {
      const DetectiveData* now_detect = reinterpret_cast<DetectiveData*>( m_nowDetectiveData ); 
      return now_detect->x;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �ŐV�̌��o�f�[�^������͕�����y����[-1, 1] ���擾����
     */
    //-------------------------------------------------------------------------
    f32 VectorDevice::GetLatestY( void ) const
    {
      const DetectiveData* now_detect = reinterpret_cast<DetectiveData*>( m_nowDetectiveData ); 
      return now_detect->y;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �ŐV�̌��o�f�[�^�ɂ���, �w�肵�����������͂���Ă��邩�ǂ����𒲂ׂ�
     */
    //-------------------------------------------------------------------------
    bool VectorDevice::IsHoldLatest( u32 dir ) const
    {
      const DetectiveData* now_detect = reinterpret_cast<DetectiveData*>( m_nowDetectiveData ); 
      u32 now_hold = this->GetHoldState( now_detect->x, now_detect->y );
      return ( now_hold & dir ) != 0;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �ŐV�̌��o�f�[�^�ɂ���, �w�肵�����������͂��ꂽ���ǂ����𒲂ׂ�
     */
    //-------------------------------------------------------------------------
    bool VectorDevice::IsTriggerLatest( u32 dir ) const
    {
      const DetectiveData* now_detect = reinterpret_cast<DetectiveData*>( m_nowDetectiveData ); 
      const DetectiveData* prev_detect = reinterpret_cast<DetectiveData*>( m_prevDetectiveData ); 
      u32 now_hold = this->GetHoldState( now_detect->x, now_detect->y );
      u32 prev_hold = this->GetHoldState( prev_detect->x, prev_detect->y );
      return ( ( prev_hold & dir ) == 0 ) && ( ( now_hold & dir ) != 0 );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �ŐV�̌��o�f�[�^�ɂ���, �w�肵�������̓��͂������Ȃ������ǂ����𒲂ׂ�
     */
    //-------------------------------------------------------------------------
    bool VectorDevice::IsReleaseLatest( u32 dir ) const
    {
      const DetectiveData* now_detect = reinterpret_cast<DetectiveData*>( m_nowDetectiveData ); 
      const DetectiveData* prev_detect = reinterpret_cast<DetectiveData*>( m_prevDetectiveData ); 
      u32 now_hold = this->GetHoldState( now_detect->x, now_detect->y );
      u32 prev_hold = this->GetHoldState( prev_detect->x, prev_detect->y );
      return ( ( prev_hold & dir ) != 0 ) && ( ( now_hold & dir ) == 0 );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �ŐV�̌��o�f�[�^�ɂ���, �w�肵�������̃��s�[�g�����o�������ǂ����𒲂ׂ�
     */
    //-------------------------------------------------------------------------
    bool VectorDevice::IsRepeatLatest( u32 dir ) const
    {
      if( !m_pRepeatTimer->IsRepeat() ) {
        return false;
      }
      return this->IsHoldLatest( dir );
    } 




    //-------------------------------------------------------------------------
    /**
     * @brief ���݂̌��o�f�[�^�ƑO��̌��o�f�[�^���r��, �����f�[�^�������Ă��邩�ǂ����𒲂ׂ�
     * @retval true   �O��̌��o�f�[�^�ƍ���̌��o�f�[�^������
     * @retval false  �O��̌��o�f�[�^�ƍ���̌��o�f�[�^���قȂ�
     */
    //-------------------------------------------------------------------------
    bool VectorDevice::IsRepeatingSameInput( void ) const
    {
      const DetectiveData* now_data  = reinterpret_cast<DetectiveData*>(m_nowDetectiveData);
      const DetectiveData* prev_data = reinterpret_cast<DetectiveData*>(m_prevDetectiveData);

      if( ( ( now_data->x != 0 ) || ( now_data->y != 0 ) ) &&
          ( now_data->x == prev_data->x ) &&
          ( now_data->y == prev_data->y ) )
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
    void VectorDevice::StoreEffectiveData( 
        void* buffer,
        const void* detective_data,
        const void* prev_detective_data ) const
    {
      const DetectiveData* now_data = reinterpret_cast<const DetectiveData*>( detective_data ); 
      const DetectiveData* prev_data = reinterpret_cast<const DetectiveData*>( prev_detective_data ); 

      EffectiveData* buffer_data = reinterpret_cast<EffectiveData*>( buffer );
      buffer_data->x = now_data->x;
      buffer_data->y = now_data->y;

      const u32 now_hold = this->GetHoldState( now_data->x, now_data->y );
      const u32 prev_hold = this->GetHoldState( prev_data->x, prev_data->y );
      this->MergeDetectData( buffer_data, now_hold, prev_hold, DIR_UP );
      this->MergeDetectData( buffer_data, now_hold, prev_hold, DIR_DOWN );
      this->MergeDetectData( buffer_data, now_hold, prev_hold, DIR_LEFT );
      this->MergeDetectData( buffer_data, now_hold, prev_hold, DIR_RIGHT );
    }

    /**
     * @brief ���̓f�[�^����, ���͂���Ă�������̏����擾����
     * @param x x��������
     * @param y y��������
     * @return �w�肵�����͂Łu������Ă���v�����̘_���a
     */
    u32 VectorDevice::GetHoldState( f32 x, f32 y ) const
    {
      u32 hold = 0;
      hold |= ( 0 < y ) ? DIR_UP : 0;
      hold |= ( y < 0 ) ? DIR_DOWN : 0;
      hold |= ( x < 0 ) ? DIR_LEFT : 0;
      hold |= ( 0 < x ) ? DIR_RIGHT : 0;
      return hold;
    }

    /**
     * @brief ���o�f�[�^���}�[�W��, �����f�[�^�𐶐�����
     * @param[out] buffer     �������������f�[�^�̊i�[��
     * @param[in]  now_hold   ����̃z�[���h���
     * @param[in]  prev_hold  �O��̃z�[���h���
     * @param[in]  dir        �����Ώۂ̕���
     */
    void VectorDevice::MergeDetectData( EffectiveData* buffer, u32 now_hold, u32 prev_hold, Direction dir ) const
    {
      bool hold    = ( (               now_hold & dir ) != 0 );
      bool release = ( (  prev_hold & ~now_hold & dir ) != 0 );
      bool trigger = ( ( ~prev_hold &  now_hold & dir ) != 0 );
      bool repeat  = m_pRepeatTimer->IsRepeat() && hold;

      if( release )
      {
        if( buffer->trigger & dir )
        {
          // �O�t���[���̃g���K�𖳌���
          buffer->hold    &= ~dir;
          buffer->trigger &= ~dir;
          buffer->release &= ~dir;
          buffer->repeat  &= ~dir;
        }
        else
        {
          // �����[�X���ꂽ
          buffer->hold    &= ~dir;
          buffer->trigger &= ~dir;
          buffer->release |=  dir;
          buffer->repeat  &= ~dir;
        }
      }
      else if( ( buffer->release & dir ) && trigger )
      {
        // �O�t���[���̃����[�X�𖳌���
        buffer->hold    |=  dir;
        buffer->trigger &= ~dir;
        buffer->release &= ~dir;

        if( repeat ) {
          buffer->repeat |= dir;
        }
        else {
          buffer->repeat &= ~dir;
        }
      }
      else
      {
        buffer->hold    |= ( hold ) ? ( dir ) : ( 0 );
        buffer->trigger |= ( trigger ) ? ( dir ) : ( 0 );
        buffer->release |= ( release ) ? ( dir ) : ( 0 );
        buffer->repeat  |= ( repeat ) ? ( dir ) : ( 0 );
      }
    }


  } //namespace ui
} //namespace gfl

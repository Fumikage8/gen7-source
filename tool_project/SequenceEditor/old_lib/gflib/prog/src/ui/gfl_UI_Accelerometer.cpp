//=============================================================================
/**
 * @brief  �����x�Z���T�̃C���^�[�t�F�[�X
 * @file   gfl_UI_Accelerometer.cpp
 * @author obata_toshihiro
 * @date   2011.03.24
 */
//=============================================================================
#include <nn.h>
#include <nw.h>
#include "ui/gfl_UI_Accelerometer.h"
#include "./gfl_UI_DeviceImplementerForAccelerometer.h"


namespace gfl {
  namespace ui {


    // ����l�̐�Βl
    const s16 Accelerometer::MAX_MESURED_VALUE = nn::hid::MAX_OF_ACCELEROMETER_VALUE;

    // ����l�́u�V�сv���̍ő�l
    const s16 Accelerometer::MAX_PLAY_RANGE = nn::hid::MAX_OF_ACCELEROMETER_PLAY;

    // ���o���x�̍ő�l
    const s16 Accelerometer::MAX_SENSITIVITY = nn::hid::MAX_OF_ACCELEROMETER_SENSITIVITY;



    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     *
     * @param heap         �g�p����q�[�v
     * @param implementer  ���ۂɓ��͂̓ǂݎ����s���I�u�W�F�N�g
     */
    //-------------------------------------------------------------------------
    Accelerometer::Accelerometer( 
        gfl::heap::HeapBase* heap, DeviceImplementerForAccelerometer* implementer ) :
      Device( heap, implementer ),
      m_imple_for_accelerometer( implementer )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    Accelerometer::~Accelerometer()
    {
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �L���ȃf�[�^�������Ă��邩�ǂ����𒲂ׂ�
     * @retval true   �L���ȃf�[�^�������Ă���
     * @retval false  �L���ȃf�[�^�������Ă��Ȃ�
     */
    //-------------------------------------------------------------------------
    bool Accelerometer::IsValid( void ) const
    {
      const EffectiveData* data = reinterpret_cast<EffectiveData*>( m_effectiveData );
      return data->is_valid;
    } 

    //-------------------------------------------------------------------------
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
    //-------------------------------------------------------------------------
    void Accelerometer::GetMeasuredValue( gfl::math::VEC3* buffer ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      buffer->x = data.measured_value_x;
      buffer->y = data.measured_value_y;
      buffer->z = data.measured_value_z;
    }


    //-------------------------------------------------------------------------
    /**
     * @brief �����x���擾����
     *
     * @param[out] buffer  �擾�����l�̊i�[��
     */
    //-------------------------------------------------------------------------
    void Accelerometer::GetAcceleration( gfl::math::VEC3* buffer ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      buffer->x = data.acceleration_x;
      buffer->y = data.acceleration_y;
      buffer->z = data.acceleration_z;
    }

    /**
     * @brief ���ۂɎQ�Ƃ��ׂ����s�f�[�^���擾����
     */
    const Accelerometer::EffectiveData& Accelerometer::GetEffectiveData( void ) const
    {
      const void* data = this->GetRunningEffectiveData();
      const EffectiveData* effective_data = reinterpret_cast<const EffectiveData*>( data );
      return *effective_data;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ����l�́u�V�сv����ݒ肷��
     *
     * @param range  �V�ѕ�[0, MAX_PLAY_RANGE]
     */
    //-------------------------------------------------------------------------
    void Accelerometer::SetPlayRange( s16 range )
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        m_imple_for_accelerometer->SetPlayRange( range );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ����l�́u�V�сv�����擾����
     *
     * @retval [0, MAX_PLAY_RANGE]
     */
    //-------------------------------------------------------------------------
    s16 Accelerometer::GetPlayRange( void ) const
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      s16 play_range = 0;
      if( m_imple_for_accelerometer ) {
        play_range = m_imple_for_accelerometer->GetPlayRange();
      }
      return play_range;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���o���x��ݒ肷��
     *
     * @param sensitivity  ���o���x[0, MAX_SENSITIVITY]
     */
    //-------------------------------------------------------------------------
    void Accelerometer::SetSensitivity( s16 sensitivity )
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        m_imple_for_accelerometer->SetSensitivity( sensitivity );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���o���x���擾����
     *
     * @retval [0, MAX_SENSITIVITY]
     */
    //-------------------------------------------------------------------------
    s16 Accelerometer::GetSensitivity( void ) const
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      s16 sensitivity = 0;
      if( m_imple_for_accelerometer ) {
        sensitivity = m_imple_for_accelerometer->GetSensitivity();
      }
      return sensitivity;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ����l�ւ̃I�t�Z�b�g��L���ɂ���
     */
    //-------------------------------------------------------------------------
    void Accelerometer::EnableOffset( void )
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        m_imple_for_accelerometer->EnableOffset();
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ����l�ւ̃I�t�Z�b�g�𖳌��ɂ���
     */
    //-------------------------------------------------------------------------
    void Accelerometer::DisableOffset( void )
    { 
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        m_imple_for_accelerometer->DisableOffset();
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ����l�ւ̃I�t�Z�b�g���L�����ǂ����𒲂ׂ�
     *
     * @retval true   �I�t�Z�b�g���L��
     * @retval false  �I�t�Z�b�g������
     */
    //-------------------------------------------------------------------------
    bool Accelerometer::IsOffsetEnable( void ) const
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        return m_imple_for_accelerometer->IsOffsetEnable();
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���݂̃I�t�Z�b�g�l���擾����
     *
     * @param[out] buffer  �擾�����I�t�Z�b�g�l���i�[����o�b�t�@
     */
    //-------------------------------------------------------------------------
    void Accelerometer::GetOffset( gfl::math::VEC3* buffer ) const
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        m_imple_for_accelerometer->GetOffset( buffer );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ����l�ւ̃I�t�Z�b�g�l�𒼐ڎw�肷��
     *
     * @param offset  ����l�ɉ��Z����I�t�Z�b�g�l
     *
     * @note ����l�ɒP���ɉ��Z����܂�
     */
    //-------------------------------------------------------------------------
    void Accelerometer::SetOffset( const gfl::math::VEC3& offset )
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        m_imple_for_accelerometer->SetOffset( offset );
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �I�t�Z�b�g�l�����Z�b�g����( �I�t�Z�b�g�������Ȓl�ɂ��� )
     */
    //-------------------------------------------------------------------------
    void Accelerometer::ResetOffset( void )
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        m_imple_for_accelerometer->ResetOffset();
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ����]��L���ɂ���
     */
    //-------------------------------------------------------------------------
    void Accelerometer::EnableAxisRotation( void )
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        m_imple_for_accelerometer->EnableAxisRotation();
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ����]�𖳌��ɂ���
     */
    //-------------------------------------------------------------------------
    void Accelerometer::DisableAxisRotation( void )
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        m_imple_for_accelerometer->DisableAxisRotation();
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ����]���L�����ǂ����𒲂ׂ�
     *
     * @retval true  ����]���L��
     * @retval false ����]������
     */
    //-------------------------------------------------------------------------
    bool Accelerometer::IsAxisRotationEnable( void )
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        return m_imple_for_accelerometer->IsAxisRotationEnable();
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ����]�Ɏg�p���Ă����]�s����擾����
     *
     * @param buffer  �擾�����s��̊i�[��
     */
    //-------------------------------------------------------------------------
    void Accelerometer::GetAxisRotationMatrix( gfl::math::MTX34* buffer ) const
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        m_imple_for_accelerometer->GetAxisRotationMatrix( buffer );
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ����]�Ɏg�p�����]�s���ݒ肷��
     *
     * @param matrix  ����]�Ɏg�p�����]�s��
     */
    //-------------------------------------------------------------------------
    void Accelerometer::SetAxisRotationMatrix( const gfl::math::MTX34& matrix )
    { 
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        m_imple_for_accelerometer->SetAxisRotationMatrix( matrix );
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ����]�Ɏg�p�����]�s������Z�b�g����( ����]���Ȃ���Ԃɂ��� )
     */
    //-------------------------------------------------------------------------
    void Accelerometer::ResetAxisRotationMatrix( void )
    {
      GFL_ASSERT( m_imple_for_accelerometer ); //@check

      if( m_imple_for_accelerometer ) {
        m_imple_for_accelerometer->ResetAxisRotationMatrix();
      }
    }



    //-------------------------------------------------------------------------
    /**
     * @brief �����f�[�^��~�ς���
     *
     * @param buffer               �f�[�^�̊i�[��
     * @param detective_data       ����̌��o�f�[�^
     * @param prev_detective_data  �O��̌��o�f�[�^
     */
    //-------------------------------------------------------------------------
    void Accelerometer::StoreEffectiveData(
        void* buffer, 
        const void* detective_data,
        const void* prev_detective_data ) const
    {
      GFL_UNUSED( prev_detective_data );
      GFL_ASSERT( sizeof(EffectiveData) == sizeof(DetectiveData) ); //@check
      const DetectiveData* now_data = reinterpret_cast<const DetectiveData*>( detective_data ); 
      EffectiveData* effective_data = reinterpret_cast<EffectiveData*>( buffer );
      gfl::std::MemCopy( now_data, effective_data, sizeof(EffectiveData) );
    }


  }  //namespace ui
}  //namespace gfl

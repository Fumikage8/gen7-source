//=============================================================================
/**
 * @brief  �W���C���Z���T�̃C���^�[�t�F�[�X
 * @file   gfl_UI_Gyroscope.cpp
 * @author obata_toshihiro
 * @date   2011.03.24
 */
//=============================================================================
#ifdef GFL_UI_USE_GYROSCOPE

#include "gfl_Heap.h"
#include "gfl_Macros.h"
#include "ui/gfl_UI_Types.h"
#include "ui/gfl_UI_Device.h"
#include "ui/gfl_UI_Gyroscope.h"
#include "./gfl_UI_DeviceImplementerForGyroscope.h"


namespace gfl
{
  namespace ui
  {


    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     *
     * @param heap         �g�p����q�[�v
     * @param implementer  ���ۂɓ��͂̓ǂݎ����s���I�u�W�F�N�g
     */
    //-------------------------------------------------------------------------

    Gyroscope::Gyroscope( gfl::heap::HeapBase* heap, DeviceImplementerForGyroscope* implementer ) :
    Device( heap, implementer ),
    m_pImplementer( implementer )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------

    Gyroscope::~Gyroscope( )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �L���ȃf�[�^�������Ă��邩�ǂ����𒲂ׂ�
     * @retval true   �L���ȃf�[�^�������Ă���
     * @retval false  �L���ȃf�[�^�������Ă��Ȃ�
     */
    //-------------------------------------------------------------------------

    bool Gyroscope::IsValid( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.is_valid;
    }

    /**
     * @brief ���ۂɎQ�Ƃ��ׂ����s�f�[�^���擾����
     */
    const Gyroscope::EffectiveData& Gyroscope::GetEffectiveData( void ) const
    {
      const void* data = this->GetRunningEffectiveData();
      const EffectiveData* effective_data = reinterpret_cast<const EffectiveData*>( data );
      return *effective_data;
    }




    //-------------------------------------------------------------------------
    /**
     * @brief �p���x���擾����
     *
     * @retval[x] �s�b�`�����̒l
     * @retval[y] ���[�����̒l
     * @retval[z] ���[�������̒l
     *
     * @note �e�����̒l��, 360dps��1.0�Ƃ���l�ł�.
     */
    //-------------------------------------------------------------------------

    const gfl::math::VEC3& Gyroscope::GetSpeed( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.speed;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �p���x���v�Z����ۂɊ|����{�����擾����
     *
     * @retval[x] �s�b�`�����̊p���x�v�Z�̔{��
     * @retval[y] ���[�����̊p���x�v�Z�̔{��
     * @retval[z] ���[�������̊p���x�v�Z�̔{��
     */
    //-------------------------------------------------------------------------

    const gfl::math::VEC3& Gyroscope::GetAngleMagnification( void ) const
    {
      return m_pImplementer->GetAngleMagnification( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �p���x���v�Z����ۂɊ|����{����ݒ肷��
     *
     * @retval[x] �s�b�`�����̊p���x�v�Z�̔{��
     * @retval[y] ���[�����̊p���x�v�Z�̔{��
     * @retval[z] ���[�������̊p���x�v�Z�̔{��
     *
     * @note �f�t�H���g�l��(1.0, 1.0, 1.0)
     */
    //-------------------------------------------------------------------------

    void Gyroscope::SetAngleMagnification( const gfl::math::VEC3& value )
    {
      m_pImplementer->SetAngleMagnification( value );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �p���x�ɑ΂��鎲��]���L�����ǂ����𒲂ׂ�
     *
     * @retval true   �p���x�ւ̉�]�������L��
     * @retval false  �p���x�ւ̉�]����������
     *
     * @note �f�t�H���g�͖���
     */
    //-------------------------------------------------------------------------

    bool Gyroscope::IsAxisRotationEnable( void ) const
    {
      return m_pImplementer->IsAxisRotationEnable( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �p���x�ɑ΂��鎲��]��L���ɂ���
     */
    //-------------------------------------------------------------------------

    void Gyroscope::EnableAxisRotation( void )
    {
      m_pImplementer->EnableAxisRotation( );
    }

    //-------------------------------------------------------------------------
    /*
     * @brief �p���x�ɑ΂��鎲��]�𖳌��ɂ���
     */
    //-------------------------------------------------------------------------

    void Gyroscope::DisableAxisRotation( void )
    {
      m_pImplementer->DisableAxisRotation( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �p���x�ɑ΂��鎲��]�s����擾����
     * @param[out] ��]�s��̊i�[��
     */
    //-------------------------------------------------------------------------

    void Gyroscope::GetAxisRotationMatrix( gfl::math::MTX34* buffer ) const
    {
      m_pImplementer->GetAxisRotationMatrix( buffer );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �p���x�ɑ΂��鎲��]�s���ݒ肷��
     * @param matrix �ݒ肷���]�s��
     */
    //-------------------------------------------------------------------------

    void Gyroscope::SetAxisRotationMatrix( const gfl::math::MTX34& matrix )
    {
      m_pImplementer->SetAxisRotationMatrix( matrix );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �p���x�ɑ΂��鎲��]�s���������( ��]���Ȃ���Ԃ� )����
     */
    //-------------------------------------------------------------------------

    void Gyroscope::ResetAxisRotationMatrix( void )
    {
      m_pImplementer->ResetAxisRotationMatrix( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �e�������݂̎p���x�N�g���ɂȂ�悤�Ȏ���]�s���ݒ肷��
     */
    //-------------------------------------------------------------------------

    void Gyroscope::SetAxisRotationMatrixFromCurrentDirection( void )
    {
      m_pImplementer->SetAxisRotationMatrixFromCurrentDirection( );
    }




    //-------------------------------------------------------------------------
    /**
     * @brief ��]�p���擾����
     *
     * @retval[x] �s�b�`�����̒l
     * @retval[y] ���[�����̒l
     * @retval[z] ���[�������̒l
     *
     * @note �e�����̒l��, 360�x��1.0�Ƃ���l�ł�.
     */
    //-------------------------------------------------------------------------

    const gfl::math::VEC3& Gyroscope::GetAngle( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.angle;
    }




    //-------------------------------------------------------------------------
    /**
     * @brief �p���x�N�g��( x�� )���擾����
     */
    //-------------------------------------------------------------------------

    const gfl::math::VEC3& Gyroscope::GetDirectionOfX( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.direction_x;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �p���x�N�g��( y�� )���擾����
     */
    //-------------------------------------------------------------------------

    const gfl::math::VEC3& Gyroscope::GetDirectionOfY( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.direction_y;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �p���x�N�g��( z�� )���擾����
     */
    //-------------------------------------------------------------------------

    const gfl::math::VEC3& Gyroscope::GetDirectionOfZ( void ) const
    {
      const EffectiveData& data = this->GetEffectiveData();
      return data.direction_z;
    }

    //-----------------------------------------------------------------------
    /**
     * @brief �p���x�N�g����ݒ肷��
     */
    //-----------------------------------------------------------------------

    void Gyroscope::SetDirection( const gfl::math::MTX33& mtx )
    {
      m_pImplementer->SetDirection( mtx );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �p���x�N�g���̌v�Z�Ɏg�p����p���x�ɓK�p����{�����擾����
     */
    //-------------------------------------------------------------------------

    f32 Gyroscope::GetDirectionMagnification( void ) const
    {
      return m_pImplementer->GetDirectionMagnification( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �p���x�N�g���̌v�Z�Ɏg�p����p���x�ɓK�p����{����ݒ肷��
     *
     * @note �f�t�H���g�l��1.0
     */
    //-------------------------------------------------------------------------

    void Gyroscope::SetDirectionMagnification( f32 value )
    {
      m_pImplementer->SetDirectionMagnification( value );
    }




    //-------------------------------------------------------------------------
    /**
     * @brief �[���_�h���t�g�̕␳���L�����ǂ����𒲂ׂ�
     *
     * @retval true   �[���_�h���t�g�̕␳���L��
     * @retval false  �[���_�h���t�g�̕␳������
     *
     * @note �f�t�H���g�͗L��
     */
    //-------------------------------------------------------------------------

    bool Gyroscope::IsZeroDriftEnable( void ) const
    {
      return m_pImplementer->IsZeroDriftEnable( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �[���_�h���t�g�̕␳��L���ɂ���
     */
    //-------------------------------------------------------------------------

    void Gyroscope::EnableZeroDrift( void )
    {
      m_pImplementer->EnableZeroDrift( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �[���_�h���t�g�̕␳�𖳌��ɂ���
     */
    //-------------------------------------------------------------------------

    void Gyroscope::DisableZeroDrift( void )
    {
      m_pImplementer->DisableZeroDrift( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �[���_�h���t�g�̕␳���[�h���擾����
     */
    //-------------------------------------------------------------------------

    Gyroscope::ZeroDriftMode Gyroscope::GetZeroDriftMode( void ) const
    {
      return m_pImplementer->GetZeroDriftMode( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �[���_�h���t�g�̕␳���[�h��ݒ肷��
     *
     * @note �f�t�H���g�� ZERO_DRIFT_MODE_STANDARD
     */
    //-------------------------------------------------------------------------

    void Gyroscope::SetZeroDriftMode( Gyroscope::ZeroDriftMode next_mode )
    {
      m_pImplementer->SetZeroDriftMode( next_mode );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �[���_�h���t�g�␳�����̓�������擾����
     *
     * @retval ����  �␳�����������̏ꍇ
     * @retval ����  0.0�͕␳���s���Ȃ�����( ���������m���� )���Ƃ�����
     *               1.0�ɋ߂Â��قǈ��肵�Ă���( ���������Ȃ� )���Ƃ�����
     */
    //-------------------------------------------------------------------------

    f32 Gyroscope::GetZeroDriftEffect( void ) const
    {
      return m_pImplementer->GetZeroDriftEffect( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �[���_�̗V�ѕ␳���L�����ǂ����𒲂ׂ�
     *
     * @retval true   �[���_�̗V�ѕ␳���L��
     * @retval false  �[���_�̗V�ѕ␳������
     *
     * @note �f�t�H���g�͖���
     */
    //-------------------------------------------------------------------------

    bool Gyroscope::IsZeroPlayParamEnable( void ) const
    {
      return m_pImplementer->IsZeroPlayParamEnable( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �[���_�̗V�ѕ␳��L���ɂ���
     */
    //-------------------------------------------------------------------------

    void Gyroscope::EnableZeroPlayParam( void )
    {
      m_pImplementer->EnableZeroPlayParam( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �[���_�̗V�ѕ␳�𖳌��ɂ���
     */
    //-------------------------------------------------------------------------

    void Gyroscope::DisableZeroPlayParam( void )
    {
      m_pImplementer->DisableZeroPlayParam( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �[���_�̗V�ѕ␳��臒l���擾����
     */
    //-------------------------------------------------------------------------

    f32 Gyroscope::GetZeroPlayParam( void ) const
    {
      return m_pImplementer->GetZeroPlayParam( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �[���_�̗V�ѕ␳��臒l��ݒ肷��
     */
    //-------------------------------------------------------------------------

    void Gyroscope::SetZeroPlayParam( f32 value )
    {
      m_pImplementer->SetZeroPlayParam( value );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �[���_�̗V�ѕ␳�����̓�������擾����
     *
     * @retval ����  �␳�����������̏ꍇ
     * @retval ����  �p���x���ݒ�l�ɋ߂��Ȃ�ق�0.0�ɋ߂Â�
     *               0.0�̏ꍇ�͕␳���s���Ȃ��������Ƃ�����
     */
    //-------------------------------------------------------------------------

    f32 Gyroscope::GetZeroPlayEffect( void ) const
    {
      return m_pImplementer->GetZeroPlayEffect( );
    }




    //-------------------------------------------------------------------------
    /**
     * @brief �����x�ɂ��O�����p���̕␳���L�����ǂ����𒲂ׂ�
     *
     * @retval true   �����x�ɂ��O�����p���̕␳���L��
     * @retval false  �����x�ɂ��O�����p���̕␳������
     *
     * @note �f�t�H���g�͗L��
     */
    //-------------------------------------------------------------------------

    bool Gyroscope::IsRevisionByAccelerationEnable( void ) const
    {
      return m_pImplementer->IsRevisionByAccelerationEnable( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����x�ɂ��O�����p���̕␳��L���ɂ���
     */
    //-------------------------------------------------------------------------

    void Gyroscope::EnableRevisionByAcceleration( void )
    {
      m_pImplementer->EnableRevisionByAcceleration( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����x�ɂ��O�����p���̕␳�𖳌��ɂ���
     */
    //-------------------------------------------------------------------------

    void Gyroscope::DisableRevisionByAcceleration( void )
    {
      m_pImplementer->DisableRevisionByAcceleration( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����x�ɂ��O�����p���̕␳�̏d�݂��擾����
     *
     * @note �f�t�H���g�l��0.03
     */
    //-------------------------------------------------------------------------

    f32 Gyroscope::GetRevisionPowerByAcceleration( void ) const
    {
      return m_pImplementer->GetRevisionPowerByAcceleration( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����x�ɂ��O�����p���̕␳�̏d�݂�ݒ肷��
     *
     * @note �l���傫���قǋ}���ȕ␳���|����܂�
     */
    //-------------------------------------------------------------------------

    void Gyroscope::SetRevisionPowerByAcceleration( f32 value )
    {
      m_pImplementer->SetRevisionPowerByAcceleration( value );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����x�ɂ��O�����p���̕␳�̗L���͈͂��擾����
     *
     * @note �f�t�H���g�l��0.4
     */
    //-------------------------------------------------------------------------

    f32 Gyroscope::GetRevisionRangeByAcceleration( void ) const
    {
      return m_pImplementer->GetRevisionRangeByAcceleration( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����x�ɂ��O�����p���̕␳�̗L���͈͂�ݒ肷��
     *
     * @note
     * �Ⴆ��, 0.4 ���w�肵���ꍇ�� 
     * 0.6G �` 1.4G �͈̔͂Ȃ�Ε␳�v�Z�ɉ����x���g�p���܂�.
     *
     * @note
     * �␳�̃p�����[�^�͂��ׂĂ̕����Ɉ�l�ɓK�p����܂�.
     * 
     */
    //-------------------------------------------------------------------------

    void Gyroscope::SetRevisionRangeByAcceleration( f32 value )
    {
      m_pImplementer->SetRevisionRangeByAcceleration( value );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����x�ɂ��O�����p���̕␳�̏d�݂ƗL���͈͂�����������
     */
    //-------------------------------------------------------------------------

    void Gyroscope::ResetRevisionParamByAcceleration( void )
    {
      m_pImplementer->ResetRevisionParamByAcceleration( );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����x�ɂ��O�����p���̕␳�����̓�������擾����
     *
     * @retval 0      �␳�������̏ꍇ
     * @retval 0�ȏ�  1.0�ɋ߂Â��ق�, 
     *                �O�����p���̕����������x�̕����ɋ߂Â��悤�ɕ␳�������Ƃ�����
     */
    //-------------------------------------------------------------------------

    f32 Gyroscope::GetRevisionEffectByAcceleration( void ) const
    {
      return m_pImplementer->GetRevisionEffectByAcceleration( );
    }




    //-------------------------------------------------------------------------
    /**
     * @brief �����f�[�^��~�ς���
     * @param[out] buffer               �f�[�^�̊i�[��
     * @param[in]  detective_data       ����̌��o�f�[�^
     * @param[in]  prev_detective_data  �O��̌��o�f�[�^
     */
    //-------------------------------------------------------------------------

    void Gyroscope::StoreEffectiveData(
                                        void* buffer,
                                        const void* detective_data,
                                        const void* prev_detective_data ) const
    {
      GFL_UNUSED( prev_detective_data );
      GFL_ASSERT( sizeof (EffectiveData ) == sizeof (DetectiveData ) ); //@check
      const DetectiveData* now_data = reinterpret_cast < const DetectiveData* >( detective_data );
      EffectiveData* effective_data = reinterpret_cast < EffectiveData* >( buffer );
      gfl::std::MemCopy( now_data, effective_data, sizeof (EffectiveData ) );
    }




  } //namespace ui
} //namespace gfl 

#endif  // GFL_UI_USE_GYROSCOPE

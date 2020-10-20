//=============================================================================
/**
 * @file   gfl_UI_CTR_Gyroscope.cpp
 * @brief  3DS�̃W���C���Z���T���͂����o����N���X
 * @author obata_toshihiro
 * @date   2011.03.31
 */
//=============================================================================
#ifdef GFL_UI_USE_GYROSCOPE

#include "gfl_Math.h"
#include "ui/gfl_UI_Gyroscope.h"
#include "./gfl_UI_CTR_Gyroscope.h"
#include "../gfl_UI_DeviceImplementerForGyroscope.h"

namespace gfl {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     *
     * @param reader  ���͂̎擾�Ɏg�p����C���X�^���X
     * @param status  �擾�������͂��i�[����o�b�t�@
     */
    //-------------------------------------------------------------------------
    CTR_Gyroscope::CTR_Gyroscope( 
        nn::hid::GyroscopeReader* reader,
        nn::hid::GyroscopeStatus* status ) :
      DeviceImplementerForGyroscope(),
      m_pReader( reader ),
      m_pStatus( status ),
      m_angleMagnification( 1.0f, 1.0f, 1.0f ),
      m_dirMagnification( 1.0f )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    CTR_Gyroscope::~CTR_Gyroscope()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���̓f�[�^���擾����
     *
     * @param buffer  �擾�����f�[�^�̊i�[�� 
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::GetDetectiveData( void* buffer ) const
    {
      bool valid = m_pReader->ReadLatest( m_pStatus );
      Gyroscope::DetectiveData* data = reinterpret_cast<Gyroscope::DetectiveData*>( buffer );
      data->is_valid = valid;
      data->speed = m_pStatus->speed;
      data->angle = m_pStatus->angle;
      data->direction_x = m_pStatus->direction.x;
      data->direction_y = m_pStatus->direction.y;
      data->direction_z = m_pStatus->direction.z;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���C������
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::Main( void )
    {
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
    const gfl::math::VEC3& CTR_Gyroscope::GetAngleMagnification( void ) const
    {
      return m_angleMagnification;
    }

    //--------------------------------------------------------------------------
    /**
     * @brief 
     */
    //--------------------------------------------------------------------------
    void CTR_Gyroscope::SetDirection( const gfl::math::MTX33& mtx )
    {
      m_pReader->SetDirection( mtx );
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
    void CTR_Gyroscope::SetAngleMagnification( const gfl::math::VEC3& value )
    {
      m_angleMagnification = value;
      m_pReader->SetAngleMagnification( value.x, value.y, value.z );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �p���x�N�g���̌v�Z�Ɏg�p����p���x�ɓK�p����{�����擾����
     */
    //-------------------------------------------------------------------------
    f32 CTR_Gyroscope::GetDirectionMagnification( void ) const
    {
      return m_dirMagnification;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �p���x�N�g���̌v�Z�Ɏg�p����p���x�ɓK�p����{����ݒ肷��
     *
     * @note �f�t�H���g�l��1.0
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::SetDirectionMagnification( f32 value )
    {
      m_dirMagnification = value;
      m_pReader->SetDirectionMagnification( value );
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
    bool CTR_Gyroscope::IsZeroDriftEnable( void ) const
    {
      return m_pReader->IsEnableZeroDrift();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �[���_�h���t�g�̕␳��L���ɂ���
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::EnableZeroDrift( void )
    {
      m_pReader->EnableZeroDrift();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �[���_�h���t�g�̕␳�𖳌��ɂ���
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::DisableZeroDrift( void )
    {
      m_pReader->DisableZeroDrift();
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �[���_�h���t�g�̕␳���[�h���擾����
     */
    //-------------------------------------------------------------------------
    Gyroscope::ZeroDriftMode CTR_Gyroscope::GetZeroDriftMode( void ) const
    {
      nn::hid::ZeroDriftMode mode;
      m_pReader->GetZeroDriftMode( mode );
      switch( mode ) {
      case nn::hid::GYROSCOPE_ZERODRIFT_LOOSE:    return Gyroscope::ZERO_DRIFT_MODE_LOOSE;
      case nn::hid::GYROSCOPE_ZERODRIFT_STANDARD: return Gyroscope::ZERO_DRIFT_MODE_STANDARD;
      case nn::hid::GYROSCOPE_ZERODRIFT_TIGHT:    return Gyroscope::ZERO_DRIFT_MODE_TIGHT;
      default:
                                                  GFL_ASSERT(0); //@check
                                                  break;
      }
      return Gyroscope::ZERO_DRIFT_MODE_STANDARD;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �[���_�h���t�g�̕␳���[�h��ݒ肷��
     *
     * @note �f�t�H���g�� ZERO_DRIFT_MODE_STANDARD
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::SetZeroDriftMode( Gyroscope::ZeroDriftMode next_mode )
    {
      nn::hid::ZeroDriftMode mode = nn::hid::GYROSCOPE_ZERODRIFT_STANDARD;
      switch( next_mode ) {
      case Gyroscope::ZERO_DRIFT_MODE_LOOSE:    mode = nn::hid::GYROSCOPE_ZERODRIFT_LOOSE;  break;
      case Gyroscope::ZERO_DRIFT_MODE_STANDARD: mode = nn::hid::GYROSCOPE_ZERODRIFT_STANDARD;  break;
      case Gyroscope::ZERO_DRIFT_MODE_TIGHT:    mode = nn::hid::GYROSCOPE_ZERODRIFT_TIGHT;  break;
      default:  
                                                GFL_ASSERT(0); //@check
                                                break;
      }
      m_pReader->SetZeroDriftMode( mode );
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
    f32 CTR_Gyroscope::GetZeroDriftEffect( void ) const
    {
      return m_pReader->GetZeroDriftEffect();
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
    bool CTR_Gyroscope::IsZeroPlayParamEnable( void ) const
    {
      return m_pReader->IsEnableZeroPlay();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �[���_�̗V�ѕ␳��L���ɂ���
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::EnableZeroPlayParam( void )
    {
      m_pReader->EnableZeroPlay();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �[���_�̗V�ѕ␳�𖳌��ɂ���
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::DisableZeroPlayParam( void )
    {
      m_pReader->DisableZeroPlay();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �[���_�̗V�ѕ␳��臒l���擾����
     */
    //-------------------------------------------------------------------------
    f32 CTR_Gyroscope::GetZeroPlayParam( void ) const
    {
      f32 value = 0.0f;
      m_pReader->GetZeroPlayParam( value );
      return value;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �[���_�̗V�ѕ␳��臒l��ݒ肷��
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::SetZeroPlayParam( f32 value )
    {
      m_pReader->SetZeroPlayParam( value );
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
    f32 CTR_Gyroscope::GetZeroPlayEffect( void ) const
    {
      return m_pReader->GetZeroPlayEffect();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �p���x�̎���]���L�����ǂ����𒲂ׂ�
     *
     * @retval true   �p���x�ւ̉�]�������L��
     * @retval false  �p���x�ւ̉�]����������
     *
     * @note �f�t�H���g�͖���
     */
    //-------------------------------------------------------------------------
    bool CTR_Gyroscope::IsAxisRotationEnable( void ) const
    {
      return m_pReader->IsEnableAxisRotation();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �p���x�̎���]��L���ɂ���
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::EnableAxisRotation( void )
    {
      m_pReader->EnableAxisRotation();
    }

    //-------------------------------------------------------------------------
    /*
     * @brief �p���x�̎���]�𖳌��ɂ���
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::DisableAxisRotation( void )
    {
      m_pReader->DisableAxisRotation();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �p���x�̎���]�s����擾����
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::GetAxisRotationMatrix( gfl::math::MTX34* buffer ) const
    {
      m_pReader->GetAxisRotationMatrix( buffer );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �p���x�̎���]�s���ݒ肷��
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::SetAxisRotationMatrix( const gfl::math::MTX34& matrix )
    {
      m_pReader->SetAxisRotationMatrix( matrix );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �p���x�̎���]�s���������( ��]���Ȃ���Ԃ� )��;
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::ResetAxisRotationMatrix( void )
    {
      m_pReader->ResetAxisRotationMatrix();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �e�������݂̎p���x�N�g���ɂȂ�悤�Ȏ���]�s���ݒ肷��
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::SetAxisRotationMatrixFromCurrentDirection( void )
    {
      gfl::math::MTX34 matrix;
      matrix.SetupIdentity();
      matrix._00 = m_pStatus->direction.x.x;
      matrix._10 = m_pStatus->direction.x.y;
      matrix._20 = m_pStatus->direction.x.z;
      matrix._01 = m_pStatus->direction.y.x;
      matrix._11 = m_pStatus->direction.y.y;
      matrix._21 = m_pStatus->direction.y.z;
      matrix._02 = m_pStatus->direction.z.x;
      matrix._12 = m_pStatus->direction.z.y;
      matrix._22 = m_pStatus->direction.z.z;
      this->SetAxisRotationMatrix( matrix ); 
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
    bool CTR_Gyroscope::IsRevisionByAccelerationEnable( void ) const
    {
      return m_pReader->IsEnableAccRevise();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����x�ɂ��O�����p���̕␳��L���ɂ���
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::EnableRevisionByAcceleration( void )
    {
      m_pReader->EnableAccRevise();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����x�ɂ��O�����p���̕␳�𖳌��ɂ���
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::DisableRevisionByAcceleration( void )
    {
      m_pReader->DisableAccRevise();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����x�ɂ��O�����p���̕␳�̏d�݂��擾����
     *
     * @note �f�t�H���g�l��0.03
     */
    //-------------------------------------------------------------------------
    f32 CTR_Gyroscope::GetRevisionPowerByAcceleration( void ) const
    {
      f32 power = 0.0f;
      f32 range = 0.0f;
      m_pReader->GetAccReviseParam( power, range );
      return power;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����x�ɂ��O�����p���̕␳�̏d�݂�ݒ肷��
     *
     * @note �l���傫���قǋ}���ȕ␳���|����܂�
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::SetRevisionPowerByAcceleration( f32 value )
    {
      f32 range = this->GetRevisionRangeByAcceleration();
      m_pReader->SetAccReviseParam( value, range );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����x�ɂ��O�����p���̕␳�̗L���͈͂��擾����
     *
     * @note �f�t�H���g�l��0.4
     */
    //-------------------------------------------------------------------------
    f32 CTR_Gyroscope::GetRevisionRangeByAcceleration( void ) const
    {
      f32 power = 0.0f;
      f32 range = 0.0f;
      m_pReader->GetAccReviseParam( power, range );
      return range;
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
    void CTR_Gyroscope::SetRevisionRangeByAcceleration( f32 value )
    { 
      f32 power = this->GetRevisionPowerByAcceleration();
      m_pReader->SetAccReviseParam( power, value );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����x�ɂ��O�����p���̕␳�̏d�݂ƗL���͈͂�����������
     */
    //-------------------------------------------------------------------------
    void CTR_Gyroscope::ResetRevisionParamByAcceleration( void )
    {
      m_pReader->ResetAccReviseParam();
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
    f32 CTR_Gyroscope::GetRevisionEffectByAcceleration( void ) const
    {
      return m_pReader->GetAccReviseEffect();
    }


  } //namespace ui
} //namespace gfl

#endif  // GFL_UI_USE_GYROSCOPE

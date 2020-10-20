//=============================================================================
/**
 * @file   gfl_UI_CTR_Accelerometer.cpp
 * @brief  3DS�̉����x�Z���T�̓��͂����o����N���X
 * @author obata_toshihiro
 * @date   2011.03.25
 */
//=============================================================================
#include "./gfl_UI_CTR_Accelerometer.h"

namespace gfl {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     *
     * @param reader  �{�^�����͂̓ǂݎ����s���C���X�^���X
     * @param status  �ǂݎ�����{�^�����͂��擾����o�b�t�@
     */
    //-------------------------------------------------------------------------
    CTR_Accelerometer::CTR_Accelerometer( 
        nn::hid::AccelerometerReader* reader, nn::hid::AccelerometerStatus* status ) :
      DeviceImplementerForAccelerometer(),
      m_pReader( reader ),
      m_pStatus( status )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    CTR_Accelerometer::~CTR_Accelerometer( void )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���̓f�[�^���擾����
     *
     * @param[out] buffer  �擾�����f�[�^�̊i�[�� 
     */
    //-------------------------------------------------------------------------
    void CTR_Accelerometer::GetDetectiveData( void* buffer ) const
    {
      bool valid = m_pReader->ReadLatest( m_pStatus );
      Accelerometer::DetectiveData* data = reinterpret_cast<Accelerometer::DetectiveData*>( buffer );
      data->is_valid = valid;
      data->measured_value_x = static_cast<f32>( m_pStatus->x );
      data->measured_value_y = static_cast<f32>( m_pStatus->y );
      data->measured_value_z = static_cast<f32>( m_pStatus->z );

      nn::hid::AccelerationFloat accel;
      m_pReader->ConvertToAcceleration( &accel, 1, m_pStatus );
      data->acceleration_x = accel.x;
      data->acceleration_y = accel.y;
      data->acceleration_z = accel.z;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ����l�́u�V�сv����ݒ肷��
     */
    //-------------------------------------------------------------------------
    s16 CTR_Accelerometer::GetPlayRange( void ) const
    {
      s16 play = 0;
      s16 sensitivity = 0;
      m_pReader->GetSensitivity( &play, &sensitivity );
      return play;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ����l�́u�V�сv�����擾����
     */
    //-------------------------------------------------------------------------
    void CTR_Accelerometer::SetPlayRange( s16 value )
    {
      s16 sensitivity = this->GetSensitivity();
      s16 play = value;
      play = ( play < 0 ) ? 0 : play;
      play = ( Accelerometer::MAX_PLAY_RANGE < play ) ? Accelerometer::MAX_PLAY_RANGE : play;
      m_pReader->SetSensitivity( play, sensitivity );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ���o���x��ݒ肷��
     */
    //-------------------------------------------------------------------------
    s16 CTR_Accelerometer::GetSensitivity( void ) const
    {
      s16 play = 0;
      s16 sensitivity = 0;
      m_pReader->GetSensitivity( &play, &sensitivity );
      return sensitivity;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���o���x��ݒ肷��
     */
    //-------------------------------------------------------------------------
    void CTR_Accelerometer::SetSensitivity( s16 value )
    {
      s16 play = this->GetPlayRange();
      s16 sensitivity = value;
      sensitivity = ( sensitivity < 0 ) ? 0 : sensitivity;
      sensitivity = ( Accelerometer::MAX_SENSITIVITY < sensitivity ) ? Accelerometer::MAX_SENSITIVITY : sensitivity;
      m_pReader->SetSensitivity( play, sensitivity );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ����l�ւ̃I�t�Z�b�g��L���ɂ���
     */
    //-------------------------------------------------------------------------
    void CTR_Accelerometer::EnableOffset( void )
    {
      m_pReader->EnableOffset();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ����l�ւ̃I�t�Z�b�g�𖳌��ɂ���
     */
    //-------------------------------------------------------------------------
    void CTR_Accelerometer::DisableOffset( void )
    {
      m_pReader->DisableOffset();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ����l�ւ̃I�t�Z�b�g���L�����ǂ����𒲂ׂ�
     *
     * @retval true   �I�t�Z�b�g���L��
     * @retval false  �I�t�Z�b�g������
     */
    //-------------------------------------------------------------------------
    bool CTR_Accelerometer::IsOffsetEnable( void ) const
    {
      return m_pReader->IsEnableOffset();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���݂̃I�t�Z�b�g�l���擾����
     *
     * @param[out] buffer  �擾�����I�t�Z�b�g�l���i�[����o�b�t�@
     */
    //-------------------------------------------------------------------------
    void CTR_Accelerometer::GetOffset( gfl::math::VEC3* buffer ) const
    {
      s16 offset_x, offset_y, offset_z;
      m_pReader->GetOffset( &offset_x, &offset_y, &offset_z );

      buffer->x = static_cast<f32>( offset_x );
      buffer->y = static_cast<f32>( offset_y );
      buffer->z = static_cast<f32>( offset_z );
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
    void CTR_Accelerometer::SetOffset( const gfl::math::VEC3& offset )
    {
      m_pReader->SetOffset( 
          static_cast<s16>( offset.x ), 
          static_cast<s16>( offset.y ), 
          static_cast<s16>( offset.z ) );
    }

    //----------------------------------------------------------------------------------
    /**
     * @brief �w�肵������l�̂Ƃ��ɐ����ɂȂ�悤�ȃI�t�Z�b�g�l��ݒ肷��
     *
     * @param level_value  �����Ƃ݂Ȃ�����l
     */
    //----------------------------------------------------------------------------------
    void CTR_Accelerometer::ResetOffset( void )
    {
      m_pReader->ResetOffset();
    }

    //----------------------------------------------------------------------------------
    /**
     * @brief ����l�ɑ΂��鎲��]��L���ɂ���
     */
    //----------------------------------------------------------------------------------
    void CTR_Accelerometer::EnableAxisRotation( void )
    {
      m_pReader->EnableAxisRotation();
    } 

    //----------------------------------------------------------------------------------
    /**
     * @brief ����l�ɑ΂��鎲��]�𖳌��ɂ���
     */
    //----------------------------------------------------------------------------------
    void CTR_Accelerometer::DisableAxisRotation( void )
    {
      m_pReader->DisableAxisRotation();
    } 

    //----------------------------------------------------------------------------------
    /**
     * @brief ����l�ɑ΂��鎲��]���L�����ǂ����𒲂ׂ�
     *
     * @retval true  ����]���L��
     * @retval false ����]������
     */
    //----------------------------------------------------------------------------------
    bool CTR_Accelerometer::IsAxisRotationEnable( void )
    {
      return m_pReader->IsEnableAxisRotation();
    }

    //----------------------------------------------------------------------------------
    /**
     * @brief ����l�ɑ΂��鎲��]�Ƃ��ēK�p���Ă����]�s����擾����
     *
     * @param buffer  �擾�����s��̊i�[��
     */
    //----------------------------------------------------------------------------------
    void CTR_Accelerometer::GetAxisRotationMatrix( gfl::math::MTX34* buffer ) const
    {
      m_pReader->GetAxisRotationMatrix( buffer );
    } 

    //----------------------------------------------------------------------------------
    /**
     * @brief ����l�ɑ΂��鎲��]�Ƃ��ēK�p�����]�s���ݒ肷��
     *
     * @param matrix  ����]�Ɏg�p�����]�s��
     */
    //----------------------------------------------------------------------------------
    void CTR_Accelerometer::SetAxisRotationMatrix( const gfl::math::MTX34& matrix )
    { 
      m_pReader->SetAxisRotationMatrix( matrix );
    } 

    //----------------------------------------------------------------------------------
    /**
     * @brief ����]�Ɏg�p�����]�s������Z�b�g����( ����]���Ȃ���Ԃɂ��� )
     */
    //----------------------------------------------------------------------------------
    void CTR_Accelerometer::ResetAxisRotationMatrix( void )
    {
      m_pReader->ResetAxisRotationMatrix();
    }


  } //namespace ui
} //namespace gfl

//=============================================================================
/**
 * @file   gfl_UI_CTR_Stick.cpp
 * @brief  3DS�̃A�i���O�X�e�B�b�N���͂����o����N���X
 * @author obata_toshihiro
 * @date   2010.10.21
 */
//=============================================================================
#include <nn.h>
#include "ui/gfl_UI_VectorDevice.h"
#include "./gfl_UI_CTR_Stick.h"

namespace gfl {
  namespace ui {


    // ���[�h�ʂ̃N�����v�p�����[�^����l�E�����l
    const s16 VectorDevice::MAX_STICK_CLAMP_PARAM_ON_CIRCLE_MODE  = nn::hid::LIMIT_OF_STICK_CLAMP_MAX;
    const s16 VectorDevice::MIN_STICK_CLAMP_PARAM_ON_CIRCLE_MODE  = nn::hid::MIN_OF_STICK_CLAMP_MODE_CIRCLE;
    const s16 VectorDevice::MAX_STICK_CLAMP_PARAM_ON_CROSS_MODE   = nn::hid::LIMIT_OF_STICK_CLAMP_MAX;
    const s16 VectorDevice::MIN_STICK_CLAMP_PARAM_ON_CROSS_MODE   = nn::hid::MIN_OF_STICK_CLAMP_MODE_CROSS;
    const s16 VectorDevice::MAX_STICK_CLAMP_PARAM_ON_MINIMUM_MODE = nn::hid::LIMIT_OF_STICK_CLAMP_MAX;
    const s16 VectorDevice::MIN_STICK_CLAMP_PARAM_ON_MINIMUM_MODE = nn::hid::MIN_OF_STICK_CLAMP_MODE_CIRCLE;


    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     *
     * @param p_reader  ���͂̓ǂݎ������s����C���X�^���X
     * @param p_status  �ǂݎ�������͂��擾����o�b�t�@
     */
    //-------------------------------------------------------------------------
    CTR_Stick::CTR_Stick(
        nn::hid::PadReader* p_reader,
        nn::hid::PadStatus* p_status ) :
      DeviceImplementerForVectorDevice(),
      m_pPadReader( p_reader ),
      m_pPadStatus( p_status )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    CTR_Stick::~CTR_Stick()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���C������
     */
    //-------------------------------------------------------------------------
    void CTR_Stick::Main( void )
    {
      // �������Ȃ�
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���̓f�[�^���擾����
     *
     * @param p_buffer  �擾�����f�[�^�̊i�[�� 
     */
    //-------------------------------------------------------------------------
    void CTR_Stick::GetDetectiveData( void* p_buffer ) const
    {
      VectorDevice::DetectiveData* data = reinterpret_cast<VectorDevice::DetectiveData*>(p_buffer);
      data->x = m_pPadReader->NormalizeStick( m_pPadStatus->stick.x );
      data->y = m_pPadReader->NormalizeStick( m_pPadStatus->stick.y );
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief �A�i���O�X�e�B�b�N�̃N�����v���[�h���擾����
     */
    //-------------------------------------------------------------------------------
    VectorDevice::StickClampMode CTR_Stick::GetStickClampMode( void ) const
    {
      nn::hid::PadReader::StickClampMode nn_mode = m_pPadReader->GetStickClampMode();
      switch( nn_mode ) {
      case nn::hid::PadReader::STICK_CLAMP_MODE_CIRCLE:  return VectorDevice::STICK_CLAMP_MODE_CIRCLE;
      case nn::hid::PadReader::STICK_CLAMP_MODE_CROSS:  return VectorDevice::STICK_CLAMP_MODE_CROSS;
      case nn::hid::PadReader::STICK_CLAMP_MODE_MINIMUM:  return VectorDevice::STICK_CLAMP_MODE_MINIMUM;
      default: 
        GFL_ASSERT(0); //@check
        break;
      }
      return VectorDevice::STICK_CLAMP_MODE_CIRCLE;
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief �A�i���O�X�e�B�b�N�̃N�����v���[�h��ݒ肷��
     */
    //-------------------------------------------------------------------------------
    void CTR_Stick::SetStickClampMode( VectorDevice::StickClampMode clamp_mode )
    {
      nn::hid::PadReader::StickClampMode nn_mode;
      switch( clamp_mode ) {
      case VectorDevice::STICK_CLAMP_MODE_CIRCLE:  nn_mode = nn::hid::PadReader::STICK_CLAMP_MODE_CIRCLE;  break;
      case VectorDevice::STICK_CLAMP_MODE_CROSS:  nn_mode = nn::hid::PadReader::STICK_CLAMP_MODE_CROSS;  break;
      case VectorDevice::STICK_CLAMP_MODE_MINIMUM:  nn_mode = nn::hid::PadReader::STICK_CLAMP_MODE_MINIMUM;  break;
      default:
        GFL_ASSERT(0); //@check
        return;
      }
      m_pPadReader->SetStickClampMode( nn_mode );
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief �A�i���O�X�e�B�b�N�̃N�����v�p�����[�^���擾����
     *
     * @param[out] min  �ŏ��N�����v�l�̊i�[��
     * @param[out] max  �ő�N�����v�l�̊i�[��
     */
    //-------------------------------------------------------------------------------
    void CTR_Stick::GetStickClampParam( s16* min, s16* max ) const
    {
      m_pPadReader->GetStickClamp( min, max );
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief �A�i���O�X�e�B�b�N�̃N�����v�p�����[�^��ݒ肷��
     *
     * @param[in] min  �ŏ��N�����v�l
     * @param[in] max  �ő�N�����v�l
     */
    //-------------------------------------------------------------------------------
    void CTR_Stick::SetStickClampParam( s16 min, s16 max )
    {
      m_pPadReader->SetStickClamp( min, max );
    }


  } //namespace ui
} //namespace gfl 

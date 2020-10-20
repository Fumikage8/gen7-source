//=============================================================================
/**
 * @file   gfl_UI_CTR_DebugStick.cpp
 * @brief  3DS�̃f�o�b�O�p�b�h�̃A�i���O�X�e�B�b�N���͂����o����N���X
 * @author obata_toshihiro
 * @date   2011.02.09
 */
//=============================================================================
#ifdef GFL_UI_USE_DEBUG_PAD
#include <nn.h>
#include "ui/gfl_UI_Types.h"
#include "ui/gfl_UI_VectorDevice.h"
#include "./gfl_UI_CTR_DebugStick.h"

namespace gfl {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     *
     * @param stick_id  �ǂ̃X�e�B�b�N��
     * @param p_reader  ���͂̓ǂݎ������s����C���X�^���X
     * @param p_status  �ǂݎ�������͂��擾����o�b�t�@
     */
    //-------------------------------------------------------------------------
    CTR_DebugStick::CTR_DebugStick( 
        StickID stick_id,
        nn::hid::DebugPadReader* p_reader, nn::hid::DebugPadStatus* p_status ) :
      DeviceImplementerForVectorDevice(),
      m_stickId( stick_id ),
      m_pPadReader( p_reader ),
      m_pPadStatus( p_status )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    CTR_DebugStick::~CTR_DebugStick()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���C������
     */
    //-------------------------------------------------------------------------
    void CTR_DebugStick::Main( void )
    {
      // �������Ȃ�
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���̓f�[�^���擾����
     *
     * @param void* p_buffer �擾�����f�[�^�̊i�[�� 
     */
    //-------------------------------------------------------------------------
    void CTR_DebugStick::GetDetectiveData( void* p_buffer ) const
    {
      VectorDevice::DetectiveData* data = reinterpret_cast<VectorDevice::DetectiveData*>(p_buffer);

      if( m_stickId == STICK_ID_LEFT )
      {
        data->x = m_pPadStatus->leftStickX;
        data->y = m_pPadStatus->leftStickY;
      }
      else if( m_stickId == STICK_ID_RIGHT )
      {
        data->x = m_pPadStatus->rightStickX;
        data->y = m_pPadStatus->rightStickY;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �A�i���O�X�e�B�b�N�̃N�����v���[�h���擾����
     */
    //-------------------------------------------------------------------------
    VectorDevice::StickClampMode CTR_DebugStick::GetStickClampMode( void ) const
    {
      return VectorDevice::STICK_CLAMP_MODE_CIRCLE;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �A�i���O�X�e�B�b�N�̃N�����v���[�h��ݒ肷��
     */
    //-------------------------------------------------------------------------
    void CTR_DebugStick::SetStickClampMode( VectorDevice::StickClampMode clamp_mode )
    {
      GFL_UNUSED(clamp_mode);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �A�i���O�X�e�B�b�N�̃N�����v�p�����[�^���擾����
     *
     * @param[out] min  �ŏ��N�����v�l�̊i�[��
     * @param[out] max  �ő�N�����v�l�̊i�[��
     */
    //-------------------------------------------------------------------------
    void CTR_DebugStick::GetStickClampParam( s16* min, s16* max ) const
    {
      GFL_UNUSED(min);
      GFL_UNUSED(max);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �A�i���O�X�e�B�b�N�̃N�����v�p�����[�^��ݒ肷��
     *
     * @param[in] min  �ŏ��N�����v�l
     * @param[in] max  �ő�N�����v�l
     */
    //-------------------------------------------------------------------------
    void CTR_DebugStick::SetStickClampParam( s16 min, s16 max )
    {
      GFL_UNUSED(min);
      GFL_UNUSED(max);
    }


  } //namespace ui
} //namespace gfl 

#endif //GFL_UI_USE_DEBUG_PAD

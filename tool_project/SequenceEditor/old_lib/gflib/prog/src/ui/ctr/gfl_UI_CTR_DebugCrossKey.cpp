//=============================================================================
/**
 * @file   gfl_UI_CTR_DebugCrossKey.cpp
 * @brief  3DS�̃f�o�b�O�p�b�h�̏\���L�[���͂����o����N���X
 * @author obata_toshihiro
 * @date   2011.02.09
 */
//=============================================================================
#ifdef GFL_UI_USE_DEBUG_PAD
#include <nn.h>
#include "ui/gfl_UI_Types.h"
#include "ui/gfl_UI_VectorDevice.h"
#include "./gfl_UI_CTR_DebugCrossKey.h"

namespace gfl {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     *
     * @param p_reader  �p�b�h���͂̓ǂݎ����s���C���X�^���X
     * @param p_status  �ǂݎ�������͂��擾����o�b�t�@
     */
    //-------------------------------------------------------------------------
    CTR_DebugCrossKey::CTR_DebugCrossKey( 
        nn::hid::DebugPadReader* p_reader, nn::hid::DebugPadStatus* p_status ) :
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
    CTR_DebugCrossKey::~CTR_DebugCrossKey()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���C������
     */
    //-------------------------------------------------------------------------
    void CTR_DebugCrossKey::Main( void )
    {
      // �������Ȃ�
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���̓f�[�^���擾����
     *
     * @param  p_buffer �擾�����f�[�^�̊i�[�� 
     */
    //-------------------------------------------------------------------------
    void CTR_DebugCrossKey::GetDetectiveData( void* p_buffer ) const
    {
      VectorDevice::DetectiveData* data = reinterpret_cast<VectorDevice::DetectiveData*>(p_buffer);

      // ���E�L�[
      if( m_pPadStatus->hold & nn::hid::DEBUG_PAD_BUTTON_LEFT ) {
        data->x = -1.0f;
      }
      else if( m_pPadStatus->hold & nn::hid::DEBUG_PAD_BUTTON_RIGHT ) {
        data->x = 1.0f;
      }
      else {
        data->x = 0.0f;
      }

      // �㉺�L�[
      if( m_pPadStatus->hold & nn::hid::DEBUG_PAD_BUTTON_DOWN ) {
        data->y =  -1.0f;
      }
      else if( m_pPadStatus->hold & nn::hid::DEBUG_PAD_BUTTON_UP ) {
        data->y =  1.0f;
      }
      else {
        data->y = 0.0f;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �A�i���O�X�e�B�b�N�̃N�����v���[�h���擾����
     */
    //-------------------------------------------------------------------------
    VectorDevice::StickClampMode CTR_DebugCrossKey::GetStickClampMode( void ) const
    {
      return VectorDevice::STICK_CLAMP_MODE_CROSS;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �A�i���O�X�e�B�b�N�̃N�����v���[�h��ݒ肷��
     */
    //-------------------------------------------------------------------------
    void CTR_DebugCrossKey::SetStickClampMode( VectorDevice::StickClampMode clamp_mode )
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
    void CTR_DebugCrossKey::GetStickClampParam( s16* min, s16* max ) const
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
    void CTR_DebugCrossKey::SetStickClampParam( s16 min, s16 max )
    {
      GFL_UNUSED(min);
      GFL_UNUSED(max);
    }


  } //namespace ui
} //namespace gfl 

#endif //GFL_UI_USE_DEBUG_PAD

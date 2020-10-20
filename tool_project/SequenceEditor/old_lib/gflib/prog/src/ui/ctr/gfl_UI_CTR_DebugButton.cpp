//=============================================================================
/**
 * @file   gfl_UI_CTR_DebugButton.cpp
 * @brief  3DS�̃f�o�b�O�p�b�h�{�^���̓��͂����o����N���X
 * @author obata_toshihiro
 * @date   2011.02.09
 */
//=============================================================================
#ifdef GFL_UI_USE_DEBUG_PAD
#include <nn.h>
#include "ui/gfl_UI_Types.h"
#include "ui/gfl_UI_Button.h"
#include "./gfl_UI_CTR_DebugButton.h"

namespace gfl {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     *
     * @param p_reader  �f�o�b�O�p�b�h���͂̓ǂݎ������s����C���X�^���X
     * @param p_status  �ǂݍ��񂾓��͂��擾����o�b�t�@
     */
    //-------------------------------------------------------------------------
    CTR_DebugButton::CTR_DebugButton( 
        nn::hid::DebugPadReader* p_reader, nn::hid::DebugPadStatus* p_status ) :
      DeviceImplementer(),
      m_pPadReader( p_reader ),
      m_pPadStatus( p_status )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    CTR_DebugButton::~CTR_DebugButton()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���C������
     */
    //-------------------------------------------------------------------------
    void CTR_DebugButton::Main( void )
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
    void CTR_DebugButton::GetDetectiveData( void* p_buffer ) const
    {
      Button::DetectiveData* data = reinterpret_cast<Button::DetectiveData*>(p_buffer);

      data->hold = 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::DEBUG_PAD_BUTTON_A) ? BUTTON_A : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::DEBUG_PAD_BUTTON_B) ? BUTTON_B : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::DEBUG_PAD_BUTTON_X) ? BUTTON_X : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::DEBUG_PAD_BUTTON_Y) ? BUTTON_Y : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::DEBUG_PAD_TRIGGER_L) ? BUTTON_L : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::DEBUG_PAD_TRIGGER_R) ? BUTTON_R : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::DEBUG_PAD_TRIGGER_ZL) ? BUTTON_ZL : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::DEBUG_PAD_TRIGGER_ZR) ? BUTTON_ZR : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::DEBUG_PAD_BUTTON_HOME) ? BUTTON_HOME : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::DEBUG_PAD_BUTTON_PLUS) ? BUTTON_PLUS : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::DEBUG_PAD_BUTTON_MINUS) ? BUTTON_MINUS : 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���̓f�[�^�̃T�C�Y���擾����
     */
    //-------------------------------------------------------------------------
    u32 CTR_DebugButton::GetDetectiveDataSize( void ) const
    {
      return sizeof(Button::DetectiveData);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����f�[�^�̃T�C�Y���擾����
     */
    //-------------------------------------------------------------------------
    u32 CTR_DebugButton::GetEffectiveDataSize( void ) const
    {
      return sizeof(Button::EffectiveData);
    }


  } //namespace ui
} //namespace gfl 

#endif //GFL_UI_USE_DEBUG_PAD

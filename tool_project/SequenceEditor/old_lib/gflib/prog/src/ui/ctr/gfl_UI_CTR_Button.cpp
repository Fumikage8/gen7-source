//=============================================================================
/**
 * @file   gfl_UI_CTR_Button.cpp
 * @brief  3DS�̃{�^�����͂����o����N���X
 * @author obata_toshihiro
 * @date   2010.10.21
 */
//=============================================================================
#include <nn.h>
#include "ui/gfl_UI_Types.h"
#include "ui/gfl_UI_Button.h"
#include "./gfl_UI_CTR_Button.h"

namespace gfl {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     * @param[in] reader  �{�^�����͂̓ǂݎ����s���C���X�^���X
     * @param[in] status  �ǂݎ�����{�^�����͂��擾����o�b�t�@
     */
    //-------------------------------------------------------------------------
    CTR_Button::CTR_Button( 
        nn::hid::PadReader* reader, nn::hid::PadStatus* status ) :
      DeviceImplementer(),
      m_pPadReader( reader ),
      m_pPadStatus( status ),
      m_isStickEmulationEnable( false )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    CTR_Button::~CTR_Button()
    {
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ���C������
     */
    //-------------------------------------------------------------------------
    void CTR_Button::Main( void )
    {
      // �������Ȃ�
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �A�i���O�X�e�B�b�N�ɂ��G�~�����[�V������L���ɂ���
     */
    //-------------------------------------------------------------------------
    void CTR_Button::SetStickEmulationEnable( void )
    {
      m_isStickEmulationEnable = true;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �A�i���O�X�e�B�b�N�ɂ��G�~�����[�V�����𖳌��ɂ���
     */
    //-------------------------------------------------------------------------
    void CTR_Button::SetStickEmulationDisable( void )
    {
      m_isStickEmulationEnable = false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���̓f�[�^���擾����
     *
     * @param[out] buffer  �擾�����f�[�^�̊i�[�� 
     */
    //-------------------------------------------------------------------------
    void CTR_Button::GetDetectiveData( void* buffer ) const
    {
      Button::DetectiveData* data = reinterpret_cast<Button::DetectiveData*>( buffer );

      data->hold = 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::BUTTON_LEFT) ? BUTTON_LEFT : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::BUTTON_RIGHT) ? BUTTON_RIGHT : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::BUTTON_UP) ? BUTTON_UP : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::BUTTON_DOWN) ? BUTTON_DOWN : 0; 
      data->hold |= (m_pPadStatus->hold & nn::hid::BUTTON_A) ? BUTTON_A : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::BUTTON_B) ? BUTTON_B : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::BUTTON_X) ? BUTTON_X : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::BUTTON_Y) ? BUTTON_Y : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::BUTTON_L) ? BUTTON_L : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::BUTTON_R) ? BUTTON_R : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::BUTTON_START) ? BUTTON_START : 0;

      if( m_isStickEmulationEnable ) {
        data->hold |= (m_pPadStatus->hold & nn::hid::BUTTON_EMULATION_LEFT) ? BUTTON_LEFT : 0;
        data->hold |= (m_pPadStatus->hold & nn::hid::BUTTON_EMULATION_RIGHT) ? BUTTON_RIGHT : 0;
        data->hold |= (m_pPadStatus->hold & nn::hid::BUTTON_EMULATION_UP) ? BUTTON_UP : 0;
        data->hold |= (m_pPadStatus->hold & nn::hid::BUTTON_EMULATION_DOWN) ? BUTTON_DOWN : 0; 
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���̓f�[�^�̃T�C�Y���擾����
     */
    //-------------------------------------------------------------------------
    u32 CTR_Button::GetDetectiveDataSize( void ) const
    {
      return sizeof(Button::DetectiveData);
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �����f�[�^�̃T�C�Y���擾����
     */
    //-------------------------------------------------------------------------
    u32 CTR_Button::GetEffectiveDataSize( void ) const
    {
      return sizeof(Button::EffectiveData);
    }


  } //namespace ui
} //namespace gfl 

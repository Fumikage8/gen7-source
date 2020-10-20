//=============================================================================
/**
 * @file   gfl_UI_CTR_TouchPanel.cpp
 * @brief  3DS�̃^�b�`�p�l�����͂����o����N���X
 * @author toshihiro_obata
 * @date   2010.10.21
 */
//=============================================================================
#include <nn.h>
#include "ui/gfl_UI_TouchPanel.h"
#include "./gfl_UI_CTR_TouchPanel.h"

namespace gfl {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     *
     * @param p_reader  ���͂̓ǂݎ������s����C���X�^���X
     * @param p_status  �ǂݎ�������͂��擾����o�b�t�@
     */
    //-------------------------------------------------------------------------
    CTR_TouchPanel::CTR_TouchPanel( 
        nn::hid::TouchPanelReader* p_reader,
        nn::hid::TouchPanelStatus* p_status ) : 
      DeviceImplementer(),
      m_pTPReader( p_reader ),
      m_pTPStatus( p_status )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    CTR_TouchPanel::~CTR_TouchPanel()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���C������
     */
    //-------------------------------------------------------------------------
    void CTR_TouchPanel::Main( void )
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
    void CTR_TouchPanel::GetDetectiveData( void* p_buffer ) const
    {
      TouchPanel::DetectiveData* data = reinterpret_cast<TouchPanel::DetectiveData*>(p_buffer);

      data->x = m_pTPStatus->x;
      data->y = m_pTPStatus->y; 
      data->touch = (m_pTPStatus->touch != 0);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���̓f�[�^�̃T�C�Y���擾����
     */
    //-------------------------------------------------------------------------
    u32 CTR_TouchPanel::GetDetectiveDataSize( void ) const
    {
      return sizeof(TouchPanel::DetectiveData);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����f�[�^�̃T�C�Y���擾����
     */
    //-------------------------------------------------------------------------
    u32 CTR_TouchPanel::GetEffectiveDataSize( void ) const
    {
      return sizeof( TouchPanel::EffectiveData );
    }


  } //namespace ui
} //namespace gfl 

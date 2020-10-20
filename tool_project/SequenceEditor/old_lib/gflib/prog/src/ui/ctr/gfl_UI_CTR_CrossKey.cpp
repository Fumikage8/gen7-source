//=============================================================================
/**
 * @file   gfl_UI_CTR_CrossKey.cpp
 * @brief  3DS�̏\���L�[���͂����o����N���X
 * @author obata_toshihiro
 * @date   2010.10.21
 */
//=============================================================================
#include <nn.h>
#include "ui/gfl_UI_VectorDevice.h"
#include "./gfl_UI_CTR_CrossKey.h"

namespace gfl {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     * @param[in] reader  �{�^�����͂̓ǂݎ����s���C���X�^���X
     * @param[in] status  �ǂݎ�����{�^�����͂��擾����o�b�t�@
     */
    //-------------------------------------------------------------------------
    CTR_CrossKey::CTR_CrossKey( 
        nn::hid::PadReader* reader, nn::hid::PadStatus* status ) :
      DeviceImplementerForVectorDevice(),
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
    CTR_CrossKey::~CTR_CrossKey()
    {
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ���C������
     */
    //-------------------------------------------------------------------------
    void CTR_CrossKey::Main( void )
    {
      // �������Ȃ�
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �A�i���O�X�e�B�b�N�ɂ��G�~�����[�V������L���ɂ���
     */
    //-------------------------------------------------------------------------
    void CTR_CrossKey::SetStickEmulationEnable( void )
    {
      m_isStickEmulationEnable = true;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �A�i���O�X�e�B�b�N�ɂ��G�~�����[�V�����𖳌��ɂ���
     */
    //-------------------------------------------------------------------------
    void CTR_CrossKey::SetStickEmulationDisable( void )
    {
      m_isStickEmulationEnable = false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���̓f�[�^���擾����
     * @param buffer  �擾�����f�[�^�̊i�[�� 
     */
    //-------------------------------------------------------------------------
    void CTR_CrossKey::GetDetectiveData( void* buffer ) const
    {
      VectorDevice::DetectiveData* data = 
        reinterpret_cast<VectorDevice::DetectiveData*>(buffer);

      // ���E�L�[
      if( m_pPadStatus->hold & nn::hid::BUTTON_LEFT ) {
        data->x = -1.0f;
      }
      else if( m_pPadStatus->hold & nn::hid::BUTTON_RIGHT ) {
        data->x = 1.0f;
      }
      else {
        data->x = 0.0f;
      }

      // ���E�L�[�̃X�e�B�b�N�G�~�����[�V����
      if( ( data->x == 0.0f ) && ( m_isStickEmulationEnable ) )
      {
        if( m_pPadStatus->hold & nn::hid::BUTTON_EMULATION_LEFT ) {
          data->x = -1.0f; 
        }
        else if( m_pPadStatus->hold & nn::hid::BUTTON_EMULATION_RIGHT ) {
          data->x = 1.0f;
        }
      }

      // �㉺�L�[
      if( m_pPadStatus->hold & nn::hid::BUTTON_DOWN ) {
        data->y =  -1.0f;
      }
      else if( m_pPadStatus->hold & nn::hid::BUTTON_UP ) {
        data->y =  1.0f;
      }
      else {
        data->y = 0.0f;
      }

      // �㉺�L�[�̃X�e�B�b�N�G�~�����[�V����
      if( ( data->y == 0.0f ) && ( m_isStickEmulationEnable ) )
      {
        if( m_pPadStatus->hold & nn::hid::BUTTON_EMULATION_DOWN ) {
          data->y = -1.0f; 
        }
        else if( m_pPadStatus->hold & nn::hid::BUTTON_EMULATION_UP ) {
          data->y = 1.0f;
        }
      }
    }

    //---------------------------------------------------------------------
    /**
     * @brief �A�i���O�X�e�B�b�N�̃N�����v���[�h���擾����
     */
    //---------------------------------------------------------------------
    VectorDevice::StickClampMode CTR_CrossKey::GetStickClampMode( void ) const
    {
      return VectorDevice::STICK_CLAMP_MODE_CROSS;
    }

    //---------------------------------------------------------------------
    /**
     * @brief �A�i���O�X�e�B�b�N�̃N�����v���[�h��ݒ肷��
     */
    //---------------------------------------------------------------------
    void CTR_CrossKey::SetStickClampMode( VectorDevice::StickClampMode clamp_mode )
    {
      GFL_UNUSED(clamp_mode);
    }

    //---------------------------------------------------------------------
    /**
     * @brief �A�i���O�X�e�B�b�N�̃N�����v�p�����[�^���擾����
     * @param[out] min  �ŏ��N�����v�l�̊i�[��
     * @param[out] max  �ő�N�����v�l�̊i�[��
     */
    //---------------------------------------------------------------------
    void CTR_CrossKey::GetStickClampParam( s16* min, s16* max ) const
    {
      GFL_UNUSED(min);
      GFL_UNUSED(max);
    }

    //---------------------------------------------------------------------
    /**
     * @brief �A�i���O�X�e�B�b�N�̃N�����v�p�����[�^��ݒ肷��
     * @param[in] min  �ŏ��N�����v�l
     * @param[in] max  �ő�N�����v�l
     */
    //---------------------------------------------------------------------
    void CTR_CrossKey::SetStickClampParam( s16 min, s16 max )
    {
      GFL_UNUSED(min);
      GFL_UNUSED(max);
    }


  } // namespace ui
} // namespace gfl 

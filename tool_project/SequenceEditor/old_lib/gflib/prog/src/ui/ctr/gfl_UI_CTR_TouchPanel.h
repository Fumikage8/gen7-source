//=============================================================================
/**
 * @file   gfl_UI_TouchPanel_CTR.h
 * @brief  3DS�̃^�b�`�p�l�����͂����o����N���X
 * @author toshihiro_obata
 * @date   2010.10.21
 */
//=============================================================================
#ifndef __GFL_UI_CTR_TOUCHPANEL_H__
#define __GFL_UI_CTR_TOUCHPANEL_H__
#include "gfl_Macros.h"
#include "../gfl_UI_DeviceImplementer.h"


namespace gfl {
  namespace ui {


    class CTR_TouchPanel : public DeviceImplementer 
    {
      GFL_FORBID_COPY_AND_ASSIGN( CTR_TouchPanel );

      public:

      //-----------------------------------------------------------------------
      /**
       * @brief �R���X�g���N�^
       *
       * @param p_reader  ���͂̓ǂݎ������s����C���X�^���X
       * @param p_status  �ǂݎ�������͂��擾����o�b�t�@
       */
      //-----------------------------------------------------------------------
      CTR_TouchPanel( nn::hid::TouchPanelReader* p_reader, nn::hid::TouchPanelStatus* p_status );

      //-----------------------------------------------------------------------
      /**
       * @brief �f�X�g���N�^
       */
      //-----------------------------------------------------------------------
      virtual ~CTR_TouchPanel();

      //-----------------------------------------------------------------------
      /**
       * @brief ���C������
       */
      //-----------------------------------------------------------------------
      virtual void Main( void );

      //-----------------------------------------------------------------------
      /**
       * @brief ���̓f�[�^���擾����
       *
       * @param p_buffer  �擾�����f�[�^�̊i�[�� 
       */
      //-----------------------------------------------------------------------
      virtual void GetDetectiveData( void* p_buffer ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief ���̓f�[�^�̃T�C�Y���擾����
       */
      //-----------------------------------------------------------------------
      virtual u32 GetDetectiveDataSize( void ) const;

      //------------------------------------------------------------------------
      /**
       * @brief �����f�[�^�̃T�C�Y���擾����
       */
      //------------------------------------------------------------------------
      virtual u32 GetEffectiveDataSize( void ) const;


      protected:
      nn::hid::TouchPanelReader* m_pTPReader;
      nn::hid::TouchPanelStatus* m_pTPStatus;
    };


  } // namespace ui
} // namespace gfl

#endif //__GFL_UI_CTR_TOUCHPANEL_H__

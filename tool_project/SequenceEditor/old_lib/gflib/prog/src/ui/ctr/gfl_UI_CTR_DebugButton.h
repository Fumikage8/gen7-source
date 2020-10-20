//=============================================================================
/**
 * @file   gfl_UI_CTR_DebugButton.h
 * @brief  3DS�̃f�o�b�O�p�b�h�{�^���̓��͂����o����N���X
 * @author obata_toshihiro
 * @date   2011.02.09
 */
//=============================================================================
#ifndef __GFL_UI_CTR_DEBUGBUTTON_H__
#define __GFL_UI_CTR_DEBUGBUTTON_H__
#include "gfl_Macros.h"
#include "../gfl_UI_DeviceImplementer.h"

namespace gfl {
  namespace ui {


    class CTR_DebugButton : public DeviceImplementer 
    { 
      GFL_FORBID_COPY_AND_ASSIGN( CTR_DebugButton );

      public:

      //-----------------------------------------------------------------------
      /**
       * @brief �R���X�g���N�^
       *
       * @param p_reader  �f�o�b�O�p�b�h���͂̓ǂݎ������s����C���X�^���X
       * @param p_status  �ǂݍ��񂾓��͂��擾����o�b�t�@
       */
      //-----------------------------------------------------------------------
      CTR_DebugButton( nn::hid::DebugPadReader* p_reader, nn::hid::DebugPadStatus* p_status );

      //-----------------------------------------------------------------------
      /**
       * @brief �f�X�g���N�^
       */
      //-----------------------------------------------------------------------
      virtual ~CTR_DebugButton();

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

      //-----------------------------------------------------------------------
      /**
       * @brief �����f�[�^�̃T�C�Y���擾����
       */
      //-----------------------------------------------------------------------
      virtual u32 GetEffectiveDataSize( void ) const;


      protected:
      nn::hid::DebugPadReader* m_pPadReader;
      nn::hid::DebugPadStatus* m_pPadStatus;
    };


  } //namespace ui
} //namespace gfl 

#endif //__GFL_UI_CTR_DEBUGBUTTON_H__

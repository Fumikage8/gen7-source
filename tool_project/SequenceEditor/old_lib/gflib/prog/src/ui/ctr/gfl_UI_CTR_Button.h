//=============================================================================
/**
 * @file   gfl_UI_CTR_Button.h
 * @brief  3DS�̃{�^�����͂����o����N���X
 * @author obata_toshihiro
 * @date   2010.10.21
 */
//=============================================================================
#ifndef __GFL_UI_CTRBUTTON_H__
#define __GFL_UI_CTRBUTTON_H__
#include "gfl_Macros.h"
#include "../gfl_UI_DeviceImplementer.h"

namespace gfl {
  namespace ui {


    class CTR_Button : public DeviceImplementer 
    { 
      GFL_FORBID_COPY_AND_ASSIGN( CTR_Button );

      public:

      //-----------------------------------------------------------------------
      // �R���X�g���N�^�E�f�X�g���N�^
      //-----------------------------------------------------------------------
      /**
       * @brief �R���X�g���N�^
       * @param[in] reader  �{�^�����͂̓ǂݎ����s���C���X�^���X
       * @param[in] status  �ǂݎ�����{�^�����͂��擾����o�b�t�@
       */
      CTR_Button( nn::hid::PadReader* reader, nn::hid::PadStatus* status );

      /**
       * @brief �f�X�g���N�^
       */
      virtual ~CTR_Button();

      //-----------------------------------------------------------------------
      // ��{����
      //-----------------------------------------------------------------------
      /**
       * @brief ���C������
       */
      virtual void Main( void ); 

      /**
       * @brief ���̓f�[�^���擾����
       * @param[out] buffer  �擾�����f�[�^�̊i�[�� 
       */
      virtual void GetDetectiveData( void* buffer ) const;

      /**
       * @brief ���̓f�[�^�̃T�C�Y���擾����
       */
      virtual u32 GetDetectiveDataSize( void ) const;

      /**
       * @brief �����f�[�^�̃T�C�Y���擾����
       */
      virtual u32 GetEffectiveDataSize( void ) const;

      //-----------------------------------------------------------------------
      // �X�e�B�b�N�ɂ��G�~�����[�V����
      //----------------------------------------------------------------------- 
      /**
       * @brief �A�i���O�X�e�B�b�N�ɂ��G�~�����[�V������L���ɂ���
       */
      void SetStickEmulationEnable( void );

      /**
       * @brief �A�i���O�X�e�B�b�N�ɂ��G�~�����[�V�����𖳌��ɂ���
       */
      void SetStickEmulationDisable( void );


      protected:
      nn::hid::PadReader* m_pPadReader;
      nn::hid::PadStatus* m_pPadStatus;
      bool m_isStickEmulationEnable;
    };


  } //namespace ui
} //namespace gfl

#endif //__GFL_UI_CTRBUTTON_H__

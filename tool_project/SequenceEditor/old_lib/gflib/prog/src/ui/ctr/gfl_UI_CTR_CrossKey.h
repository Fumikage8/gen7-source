//=============================================================================
/**
 * @file   gfl_UI_CTR_CrossKey.h
 * @brief  3DS�̏\���L�[���͂����o����N���X
 * @author obata_toshihiro
 * @date   2010.10.21
 */
//=============================================================================
#ifndef __GFL_UI_CTRCROSSKEY_H__
#define __GFL_UI_CTRCROSSKEY_H__
#include "gfl_Macros.h"
#include "../gfl_UI_DeviceImplementerForVectorDevice.h"

namespace gfl {
  namespace ui {


    class CTR_CrossKey : public DeviceImplementerForVectorDevice 
    {
      GFL_FORBID_COPY_AND_ASSIGN( CTR_CrossKey );

      public: 

      //-----------------------------------------------------------------------
      // �R���X�g���N�^�E�f�X�g���N�^
      //-----------------------------------------------------------------------
      /**
       * @brief �R���X�g���N�^
       * @param[in] reader  �{�^�����͂̓ǂݎ����s���C���X�^���X
       * @param[in] status  �ǂݎ�����{�^�����͂��擾����o�b�t�@
       */
      CTR_CrossKey( nn::hid::PadReader* reader, nn::hid::PadStatus* status );

      /**
       * @brief �f�X�g���N�^
       */
      virtual ~CTR_CrossKey(); 

      //-----------------------------------------------------------------------
      // ��{����
      //-----------------------------------------------------------------------
      /**
       * @brief ���C������
       */
      virtual void Main( void ); 

      /**
       * @brief ���̓f�[�^���擾����
       * @param buffer  �擾�����f�[�^�̊i�[�� 
       */
      virtual void GetDetectiveData( void* buffer ) const;

      /**
       * @brief �A�i���O�X�e�B�b�N�̃N�����v���[�h���擾����
       */
      virtual VectorDevice::StickClampMode GetStickClampMode( void ) const;

      /**
       * @brief �A�i���O�X�e�B�b�N�̃N�����v���[�h��ݒ肷��
       */
      virtual void SetStickClampMode( VectorDevice::StickClampMode clamp_mode );

      /**
       * @brief �A�i���O�X�e�B�b�N�̃N�����v�p�����[�^���擾����
       * @param[out] min  �ŏ��N�����v�l�̊i�[��
       * @param[out] max  �ő�N�����v�l�̊i�[��
       */
      virtual void GetStickClampParam( s16* min, s16* max ) const;

      /**
       * @brief �A�i���O�X�e�B�b�N�̃N�����v�p�����[�^��ݒ肷��
       * @param[in] min  �ŏ��N�����v�l
       * @param[in] max  �ő�N�����v�l
       */
      virtual void SetStickClampParam( s16 min, s16 max );

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

#endif //__GFL_UI_CTRCROSSKEY_H__

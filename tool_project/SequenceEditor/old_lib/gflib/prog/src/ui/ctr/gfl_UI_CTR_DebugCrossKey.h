//=============================================================================
/**
 * @file   gfl_UI_CTR_DebugCrossKey.h
 * @brief  3DS�̃f�o�b�O�p�b�h�̏\���L�[���͂����o����N���X
 * @author obata_toshihiro
 * @date   2011.02.09
 */
//=============================================================================
#ifndef __GFL_UI_CTR_DEBUGCROSSKEY_H__
#define __GFL_UI_CTR_DEBUGCROSSKEY_H__
#include "gfl_Macros.h"
#include "../gfl_UI_DeviceImplementerForVectorDevice.h"

namespace gfl {
  namespace ui {


    class CTR_DebugCrossKey : public DeviceImplementerForVectorDevice 
    { 
      GFL_FORBID_COPY_AND_ASSIGN( CTR_DebugCrossKey );

      public: 

      //-----------------------------------------------------------------------
      /**
       * @brief �R���X�g���N�^
       *
       * @param p_reader  �p�b�h���͂̓ǂݎ����s���C���X�^���X
       * @param p_status  �ǂݎ�������͂��擾����o�b�t�@
       */
      //-----------------------------------------------------------------------
      CTR_DebugCrossKey( nn::hid::DebugPadReader* p_reader, nn::hid::DebugPadStatus* p_status );

      //-----------------------------------------------------------------------
      /**
       * @brief �f�X�g���N�^
       */
      //-----------------------------------------------------------------------
      virtual ~CTR_DebugCrossKey();

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
       * @brief �A�i���O�X�e�B�b�N�̃N�����v���[�h���擾����
       */
      //-----------------------------------------------------------------------
      virtual VectorDevice::StickClampMode GetStickClampMode( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �A�i���O�X�e�B�b�N�̃N�����v���[�h��ݒ肷��
       */
      //-----------------------------------------------------------------------
      virtual void SetStickClampMode( VectorDevice::StickClampMode clamp_mode );

      //-----------------------------------------------------------------------
      /**
       * @brief �A�i���O�X�e�B�b�N�̃N�����v�p�����[�^���擾����
       *
       * @param[out] min  �ŏ��N�����v�l�̊i�[��
       * @param[out] max  �ő�N�����v�l�̊i�[��
       */
      //-----------------------------------------------------------------------
      virtual void GetStickClampParam( s16* min, s16* max ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �A�i���O�X�e�B�b�N�̃N�����v�p�����[�^��ݒ肷��
       *
       * @param[in] min  �ŏ��N�����v�l
       * @param[in] max  �ő�N�����v�l
       */
      //-----------------------------------------------------------------------
      virtual void SetStickClampParam( s16 min, s16 max );


      protected:
      nn::hid::DebugPadReader* m_pPadReader;
      nn::hid::DebugPadStatus* m_pPadStatus;
    };


  } //namespace ui
} //namespace gfl

#endif //__GFL_UI_CTR_DEBUGCROSSKEY_H__

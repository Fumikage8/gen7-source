//=============================================================================
/**
 *
 *	@file		gfl_HardConfig.h
 *	@brief  �{�̐ݒ�A�N�Z�T
 *	@author hosaka genya
 *	@data		2012.02.21
 *
 */
//=============================================================================
#ifndef __GFL_HARDCONFIG_H__
#define __GFL_HARDCONFIG_H__
#pragma once

#include <nn.h>
#include <nn/cfg.h>

#include <core/gfl_Config.h>
#include <core/gfl_Macro.h>

GFL_NAMESPACE_BEGIN( gfl );
GFL_NAMESPACE_BEGIN( cfg );

typedef nn::cfg::CTR::UserName UserName; // wchar_t userName, bool isNgUserName
typedef nn::cfg::CTR::Birthday Birthday; // s8 month, s8 day
typedef nn::cfg::CTR::CfgCountryCode CfgCountryCode;
typedef nn::cfg::CTR::CfgLanguageCode CfgLanguageCode;
typedef nn::cfg::CTR::SimpleAddress SimpleAddress;
typedef nn::cfg::CTR::SimpleAddressId SimpleAddressId;
typedef nn::cfg::CTR::CfgRegionCode CfgRegionCode;

class Config
{ 
  GFL_FORBID_COPY_AND_ASSIGN( Config );
public:
  //-----------------------------------------------------------------------------
  /**
   *	@brief  �R���X�g���N�^
   */
  //-----------------------------------------------------------------------------
  Config( void );
  
  //-----------------------------------------------------------------------------
  /**
   *	@brief  �f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  ~Config( void );

  //-----------------------------------------------------------------------------
  /**
   *	@brief  ������
   */
  //-----------------------------------------------------------------------------
  void Initialize( void );
  
  //-----------------------------------------------------------------------------
  /**
   *	@brief  �j��
   */
  //-----------------------------------------------------------------------------
  void Finalize( void );
  
  //-----------------------------------------------------------------------------
  /**
   *	@brief  ���[�U�[���̎擾
   */
  //-----------------------------------------------------------------------------
  void GetUserName( UserName* p_username ) const;
  
  //-----------------------------------------------------------------------------
  /**
   *	@brief  �a�����̎擾
   */
  //-----------------------------------------------------------------------------
  void GetBirthday( Birthday * p_birthday ) const;
  
  //-----------------------------------------------------------------------------
  /**
   *	@brief  ���R�[�h�̎擾
   */
  //-----------------------------------------------------------------------------
  void GetCountry( CfgCountryCode* p_country_code ) const;
  
  //-----------------------------------------------------------------------------
  /**
   *	@brief  ����R�[�h�̎擾
   */
  //-----------------------------------------------------------------------------
  void GetLanguage( CfgLanguageCode* p_language_code ) const;
  
  //-----------------------------------------------------------------------------
  /**
   *	@brief  �ȈՃA�h���X���̎擾
   */
  //-----------------------------------------------------------------------------
  void GetSimpleAddress( SimpleAddress* p_simple_address ) const;
  
  //-----------------------------------------------------------------------------
  /**
   *	@brief  �ȈՃA�h���X����ID�݂̂��擾
   */
  //-----------------------------------------------------------------------------
  void GetSimpleAddressId( SimpleAddressId* p_simple_address ) const;
  
  //-----------------------------------------------------------------------------
  /**
   *	@brief  ���[�W�����R�[�h��Ԃ�
   */
  //-----------------------------------------------------------------------------
  CfgRegionCode GetRegion( void ) const;
  
private:
  b32 m_bInitialize;
};


GFL_NAMESPACE_END( cfg );
GFL_NAMESPACE_END( gfl );

#endif // __GFL_HARDCONFIG_H__

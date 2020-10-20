#if defined(GF_PLATFORM_CTR)
//=============================================================================
/**
 *
 *	@file		gfl_HardConfig.h
 *	@brief  本体設定アクセサ
 *	@author hosaka genya
 *	@data		2012.02.21
 *
 */
//=============================================================================
#ifndef __GFL_HARDCONFIG_H__
#define __GFL_HARDCONFIG_H__
#pragma once
#include <nn.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_DebugPrint.h>

GFL_NAMESPACE_BEGIN( System )
GFL_NAMESPACE_BEGIN( CTR )

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
   *	@brief  コンストラクタ
   */
  //-----------------------------------------------------------------------------
  Config( void );
  
  //-----------------------------------------------------------------------------
  /**
   *	@brief  デストラクタ
   */
  //-----------------------------------------------------------------------------
  ~Config( void );

  //-----------------------------------------------------------------------------
  /**
   *	@brief  初期化
   */
  //-----------------------------------------------------------------------------
  void Initialize( void );
  
  //-----------------------------------------------------------------------------
  /**
   *	@brief  破棄
   */
  //-----------------------------------------------------------------------------
  void Finalize( void );
  
  //-----------------------------------------------------------------------------
  /**
   *	@brief  ユーザー名の取得
   */
  //-----------------------------------------------------------------------------
  void GetUserName( UserName* p_username ) const;
  
  //-----------------------------------------------------------------------------
  /**
   *	@brief  誕生日の取得
   */
  //-----------------------------------------------------------------------------
  void GetBirthday( Birthday * p_birthday ) const;
  
  //-----------------------------------------------------------------------------
  /**
   *	@brief  国コードの取得
   */
  //-----------------------------------------------------------------------------
  void GetCountry( CfgCountryCode* p_country_code ) const;
  
  //-----------------------------------------------------------------------------
  /**
   *	@brief  言語コードの取得
   */
  //-----------------------------------------------------------------------------
  void GetLanguage( CfgLanguageCode* p_language_code ) const;
  
  //-----------------------------------------------------------------------------
  /**
   *	@brief  簡易アドレス情報の取得
   */
  //-----------------------------------------------------------------------------
  void GetSimpleAddress( SimpleAddress* p_simple_address ) const;
  
  //-----------------------------------------------------------------------------
  /**
   *	@brief  簡易アドレス情報のIDのみを取得
   */
  //-----------------------------------------------------------------------------
  void GetSimpleAddressId( SimpleAddressId* p_simple_address ) const;
  
  //-----------------------------------------------------------------------------
  /**
   *	@brief  リージョンコードを返す
   */
  //-----------------------------------------------------------------------------
  CfgRegionCode GetRegion( void ) const;
  
private:
  b32 m_bInitialize;
};

GFL_NAMESPACE_END( CTR )
GFL_NAMESPACE_END( System )

#endif // __GFL_HARDCONFIG_H__
#endif
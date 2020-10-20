#if defined(GF_PLATFORM_CTR)
//=============================================================================
/**
 *
 *	@file		gfl_HardConfig.cpp
 *	@brief  本体設定アクセサ
 *	@author hosaka genya
 *	@data		2012.02.21
 *
 */
//=============================================================================
#include "System/include/CTR/CTR_Config.h" 
#include <debug/include/gfl2_assert.h>
GFL_NAMESPACE_BEGIN( System )
GFL_NAMESPACE_BEGIN( CTR )


//-----------------------------------------------------------------------------
/**
 *	@brief  コンストラクタ
 */
//-----------------------------------------------------------------------------
Config::Config( void ) : m_bInitialize( false )
{
}

//-----------------------------------------------------------------------------
/**
 *	@brief  デストラクタ
 */
//-----------------------------------------------------------------------------
Config::~Config( void )
{
}

//-----------------------------------------------------------------------------
/**
 *	@brief  初期化
 */
//-----------------------------------------------------------------------------
void Config::Initialize( void )
{
  nn::cfg::Initialize();
  m_bInitialize = true;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  破棄
 */
//-----------------------------------------------------------------------------
void Config::Finalize( void )
{
  GFL_ASSERT( m_bInitialize ); //@check
  nn::cfg::Finalize();
  m_bInitialize = false;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ユーザー名の取得
 */
//-----------------------------------------------------------------------------
void Config::GetUserName( UserName* p_username ) const
{
  GFL_ASSERT( m_bInitialize ); //@check

  if ( p_username == NULL )
  {
    GFL_ASSERT( 0 );    //@fix
    return;
  }

  nn::cfg::CTR::GetUserName( p_username );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  誕生日の取得
 */
//-----------------------------------------------------------------------------
void Config::GetBirthday( Birthday * p_birthday ) const
{
  GFL_ASSERT( m_bInitialize ); //@check
  
  if (p_birthday == NULL )
  {
    GFL_ASSERT( 0 );    //@fix
    return;
  }

  nn::cfg::CTR::GetBirthday( p_birthday );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  国コードの取得
 */
//-----------------------------------------------------------------------------
void Config::GetCountry( CfgCountryCode* p_country_code ) const
{
  GFL_ASSERT( m_bInitialize );  //@check
  
  if ( p_country_code == NULL )
  {
    GFL_ASSERT( 0 );    //@fix
    return;
  }
  
  *p_country_code = nn::cfg::CTR::GetCountry();
}

//-----------------------------------------------------------------------------
/**
 *	@brief  言語コードの取得
 */
//-----------------------------------------------------------------------------
void Config::GetLanguage( CfgLanguageCode* p_language_code ) const
{
  GFL_ASSERT( m_bInitialize );   //@check

  if ( p_language_code == NULL )
  {
    GFL_ASSERT( 0 ); //@fix
    return;
  }

  *p_language_code = nn::cfg::CTR::GetLanguage();
}

//-----------------------------------------------------------------------------
/**
 *	@brief  簡易アドレス情報の取得
 */
//-----------------------------------------------------------------------------
void Config::GetSimpleAddress( SimpleAddress* p_simple_address ) const
{
  GFL_ASSERT( m_bInitialize ); //@check

  if ( p_simple_address == NULL )
  {
    GFL_ASSERT( 0 ); //@fix
    return;
  }

  nn::cfg::CTR::GetSimpleAddress( p_simple_address );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  簡易アドレス情報のIDのみを取得
 */
//-----------------------------------------------------------------------------
void Config::GetSimpleAddressId( SimpleAddressId* p_simple_addressId ) const
{
  GFL_ASSERT( m_bInitialize ); //@check
  
  if ( p_simple_addressId == NULL )
  {
    GFL_ASSERT( 0 ); //@fix
    return;
  }

  nn::cfg::CTR::GetSimpleAddressId( p_simple_addressId );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  リージョンコードを返す
 */
//-----------------------------------------------------------------------------
CfgRegionCode Config::GetRegion( void ) const 
{ 
  GFL_ASSERT( m_bInitialize ); //@check
  
  return nn::cfg::CTR::GetRegion();
}

GFL_NAMESPACE_END( CTR )
GFL_NAMESPACE_END( System )

#endif
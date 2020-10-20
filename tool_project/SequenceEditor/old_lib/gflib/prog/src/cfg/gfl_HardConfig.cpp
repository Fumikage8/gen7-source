//=============================================================================
/**
 *
 *	@file		gfl_HardConfig.cpp
 *	@brief  �{�̐ݒ�A�N�Z�T
 *	@author hosaka genya
 *	@data		2012.02.21
 *
 */
//=============================================================================
#include "cfg/gfl_HardConfig.h" 

GFL_NAMESPACE_BEGIN( gfl );
GFL_NAMESPACE_BEGIN( cfg );


//-----------------------------------------------------------------------------
/**
 *	@brief  �R���X�g���N�^
 */
//-----------------------------------------------------------------------------
Config::Config( void ) : m_bInitialize( false )
{
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
Config::~Config( void )
{
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ������
 */
//-----------------------------------------------------------------------------
void Config::Initialize( void )
{
  nn::cfg::Initialize();
  m_bInitialize = true;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �j��
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
 *	@brief  ���[�U�[���̎擾
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
 *	@brief  �a�����̎擾
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
 *	@brief  ���R�[�h�̎擾
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
 *	@brief  ����R�[�h�̎擾
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
 *	@brief  �ȈՃA�h���X���̎擾
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
 *	@brief  �ȈՃA�h���X����ID�݂̂��擾
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
 *	@brief  ���[�W�����R�[�h��Ԃ�
 */
//-----------------------------------------------------------------------------
CfgRegionCode Config::GetRegion( void ) const 
{ 
  GFL_ASSERT( m_bInitialize ); //@check
  
  return nn::cfg::CTR::GetRegion();
}

GFL_NAMESPACE_END( cfg );
GFL_NAMESPACE_END( gfl );


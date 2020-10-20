#include "qt/Shader/Common/SecureData.h"

namespace gfl { namespace maya { namespace Common {

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
SecureData SecureData::DeCode( const UInt8 *pData, int size )
{
	SecureData			data;

	if ( size == 0 )
		return data;

	data.m_pData = New UInt8[size+1];
	data.m_Size = size;

	memset( data.m_pData, 0, data.m_Size + 1 );
	memcpy( data.m_pData, pData, data.m_Size );

	Random			random;
	for( int i = 0; i < data.m_Size; ++i )
	{
		UInt8		code = data.m_pData[ i ];
		int			shift = random.Get() % 8;

		unsigned char lCode;
		unsigned char rCode;

		rCode = ((code >> shift) & 0xFF);

		shift = 8 - shift;
		lCode = ((code << shift) & 0xFF);

		code = ( lCode + rCode );

		data.m_pData[ i ] = code;
	}

	return data;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
SecureData::SecureData()
{
	m_pData = NULL;
	m_Size = 0;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
SecureData::SecureData( const SecureData& other )
{
	*this = other;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
SecureData::~SecureData()
{
	SAFE_DELETE( m_pData );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
const UInt8* SecureData::GetData()
{
	return m_pData;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
SecureData & SecureData::operator =( const SecureData& rhs )
{
	this->m_pData = New UInt8[rhs.m_Size + 1];
	this->m_Size = rhs.m_Size;

	memset( this->m_pData, 0, this->m_Size + 1 );
	memcpy( this->m_pData, rhs.m_pData, this->m_Size );

	return *this;
}

}}}



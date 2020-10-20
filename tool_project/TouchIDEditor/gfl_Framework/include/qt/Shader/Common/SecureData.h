#ifndef GFL_MAYA_COMMON_SECURE_DATA_H_
#define GFL_MAYA_COMMON_SECURE_DATA_H_

#include "Type.h"
#include "Random.h"

namespace gfl { namespace maya { namespace Common {

class SecureData
{
public:
	static SecureData DeCode( const UInt8 *pData, int size );

	SecureData();
	SecureData( const SecureData& other );

	~SecureData();

	const UInt8* GetData();
	SecureData&  operator =( const SecureData& rhs );

private:

	UInt8						*m_pData;
	int							m_Size;
};

}}}

#endif
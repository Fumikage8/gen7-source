#if defined(GF_PLATFORM_WIN32)//Windowsでしか提供するつもりなし

//なのでSTL使っちゃう！わーい。けど、このcpp内にとどめる。
#include <vector>
#include <string>
#include <xml/include/rapidxml.hpp> // no need to link

#include <math/include/gfl2_Vector4.h>
#include <math/include/gfl2_Matrix44.h>
#include <util/include/gfl2_FnvHash.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfLocData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>
#include <gfx/include/gfl2_types.h>
#include <debug/include/gfl2_Assert.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/gfl2_RenderingEngineEnums.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_LocatorResourceNodeFactory.h>

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")  

#include <d3d11.h>
#include <d3dcompiler.h>

using namespace rapidxml;

#define					ARRAY_SIZE(array)		(sizeof(array) / sizeof(array[0]))

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  LocatorData構造体
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
struct LocatorData
{
	struct LocatorNode{
		LocatorNode()
		{
			memset( m_Name, 0, sizeof(m_Name) );
			memset( m_ParentName, 0, sizeof(m_ParentName) );
		}

		c8							m_Name[Constant::LOCATOR_NAME_MAX];
		c8							m_ParentName[Constant::LOCATOR_NAME_MAX];
		math::Matrix44		m_LocalMatrix;
	};

	LocatorData():m_pBinaryData(NULL){}
	~LocatorData(){}

	void destroy()
	{
		if ( m_pBinaryData )
			delete[] m_pBinaryData;
	}

	void ReadLocatorsNode( const xml_node<>* locatorsNode );
	void CreateBinary();
	bool WriteFile(std::string filePath);

	u8																*m_pBinaryData;
	u32																m_BinarySize;
	std::vector<LocatorNode>					m_LocatorNodes;
};

void LocatorData::ReadLocatorsNode( const xml_node<>* locatorsNode )
{
	struct LocalFunc{
		static std::string GetName( xml_attribute<> *nameAttr )
		{
			std::string								str;
			std::wstring							utf16;

			if ( nameAttr )
			{
				str = nameAttr->value();
				utf16 = ParserUtil::UTF8toUTF16( str );
				str = ParserUtil::UTF16toShitJIS( utf16 );

				std::vector<std::string>			names = ParserUtil::SplitStr( str, ":" );
				str = names[names.size()-1];
			}

			return str;
		}

		static math::Matrix44 GetMatrix( const xml_node<>	*matrixNode )
		{
			math::Matrix44									matrix = math::Matrix44::GetIdentity();      

			if ( matrixNode )
			{
				std::vector<math::Vector4>			column;
				
				ParserUtil::ParseFloatArray( &column, matrixNode, 4, 0.0f );

        // Load as TRANSPOSE_MTX:
				for( u32 i = 0; i < 4; ++i )
        {
          matrix.SetElem(0,i,column[i].x);
          matrix.SetElem(1,i,column[i].y);
          matrix.SetElem(2,i,column[i].z);
          matrix.SetElem(3,i,column[i].w);
        }				
        
			}
      
			return matrix;
		}
	};

	for( const xml_node<>* locatorNode = locatorsNode->first_node("Locator"); locatorNode; locatorNode = locatorNode->next_sibling("Locator") )
	{
		LocatorNode			node;
		std::string			str;

		str = LocalFunc::GetName( locatorNode->first_attribute( "Name" ) );
		strcpy_s( node.m_Name, str.c_str() ); 

		str = LocalFunc::GetName( locatorNode->first_attribute( "ParentName" ) );
		strcpy_s( node.m_ParentName, str.c_str() ); 

		{
			const xml_node<>	*localMatrixNode = locatorNode->first_node( "LocalMatrix");
			node.m_LocalMatrix = LocalFunc::GetMatrix( localMatrixNode );
		}

		m_LocatorNodes.push_back( node );
	}
}

void LocatorData::CreateBinary()
{
	BinaryMaker					binary;
	LocatorResourceNodeFactory::LocatorDataHeader		header;

	header.locatorNum = static_cast<s32>(m_LocatorNodes.size());

	binary.Write( &header, sizeof(header) );

	{
		for( u32 i = 0; i < m_LocatorNodes.size(); ++i )
		{
			LocatorNode			&rLocatorNodes = m_LocatorNodes[ i ];

			binary.Write( rLocatorNodes.m_Name, sizeof(rLocatorNodes.m_Name) );
			binary.Write( rLocatorNodes.m_ParentName, sizeof(rLocatorNodes.m_ParentName) );
			binary.Write( &rLocatorNodes.m_LocalMatrix, sizeof(math::Matrix44) );
		}
	}

	m_pBinaryData = binary.CreateFixedBuffer();
	m_BinarySize = binary.GetBinarySize();
}

bool LocatorData::WriteFile( std::string filePath )
{
	{
		BinaryMaker		bm;	

		static const u32 sc_Version = 0;///GfModelVersion::SegmentScale15061117;
		u32						dataCount = 1;

		// バージョン
		bm.Write(&sc_Version, sizeof(sc_Version));//4

		// データ数
		bm.Write(&dataCount, sizeof(dataCount));//8

		//
		GfBinaryMdlData::ChunkHeader		modelHeader(IResourceData::Type("locator").m_ID, m_BinarySize);
		bm.Write(&modelHeader, sizeof(modelHeader));//24
		bm.Write(m_pBinaryData, m_BinarySize);

		// キャッシュ作成
		u8* buffer = bm.CreateFixedBuffer();
		u32 bufferSize = bm.GetBinarySize();
		ParserUtil::FileCreate( filePath.c_str(), reinterpret_cast<char*>(buffer), bufferSize );
		delete[] buffer;
	}

	return true;
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  GfLocData::Implクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
class GfLocData::Impl : public gfx::GLMemory, public rapidxml::xml_document<>
{
public:
	Impl( c8* pData );
	virtual ~Impl();

	s32 GetSize( void );
	const void* GetData( u32 index );
	u32 GetDataSize( u32 index );
	IResourceData::Type GetType( u32 index );
	bool WriteFile(std::string filePath);

	LocatorData		m_LocatorData;
};

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
GfLocData::Impl::Impl( c8* pData ):gfx::GLMemory(),rapidxml::xml_document<>()
{
	this->parse< parse_no_utf8 | parse_no_entity_translation >( pData );

	const xml_node<>* top_node = first_node("GfLocator");

	{
		const xml_node<>	*locatorsNode = top_node->first_node( "Locators");
		m_LocatorData.ReadLocatorsNode( locatorsNode );
	}

	m_LocatorData.CreateBinary();
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
GfLocData::Impl::~Impl()
{
	
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
s32 GfLocData::Impl::GetSize( void )
{
	return 1;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
bool GfLocData::Impl::WriteFile(std::string filePath)
{
		return m_LocatorData.WriteFile(filePath);
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
const void* GfLocData::Impl::GetData( u32 index )
{
	return m_LocatorData.m_pBinaryData;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
u32 GfLocData::Impl::GetDataSize( u32 index )
{
	return m_LocatorData.m_BinarySize;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
IResourceData::Type GfLocData::Impl::GetType( u32 index )
{
	return IResourceData::Type("locator");
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  GfLocDataクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
GfLocData::GfLocData( c8* pData ) : IResourceData()
{
	GFL_ASSERT(pData);
	m_pImpl = new Impl( pData );
}

GfLocData::~GfLocData()
{
	delete m_pImpl;
}

//! @brief 全データの数の取得
s32 GfLocData::GetSize( void )
{
	return m_pImpl->GetSize();
}

bool GfLocData::WriteFile(std::string filePath)
{
		return( m_pImpl->WriteFile(filePath) );
}

//! @brief データの取得
const void* GfLocData::GetData( u32 index )
{
	return m_pImpl->GetData(index);
}

//! @brief 個々のデータのサイズ取得
u32 GfLocData::GetDataSize( u32 index )
{
	return m_pImpl->GetDataSize(index);
}

//! @brief タイプデータの取得
IResourceData::Type GfLocData::GetType( u32 index )
{
	return m_pImpl->GetType(index);
}


}}}}


#endif
#if defined(WIN32)//Windowsでしか提供するつもりなし

//なのでSTL使っちゃう！わーい。けど、このcpp内にとどめる。
#include <vector>
#include <string>
#include <xml/include/rapidxml.hpp> // no need to link

#include <math/include/gfl2_Vector4.h>
#include <math/include/gfl2_Matrix44.h>
#include <util/include/gfl2_FnvHash.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>
#include <gfx/include/gfl2_types.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>

#include "EffectNodeData.h"

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
struct EffectNodeData
{
	struct EffectNodeDataHeader{//!@todo 適切な場所から参照
		u32				locatorNum;
		u32				padding[3];
	};

	static const u32				LOCATOR_NAME_MAX = 64;//!@todo 適切な場所から参照

	struct EffectNode{
		EffectNode()
		{
			memset( m_Name, 0, sizeof(m_Name) );
			memset( m_ParentName, 0, sizeof(m_ParentName) );
      m_LocalMatrix = math::Matrix44::GetIdentity();
		}

		c8							m_Name[LOCATOR_NAME_MAX];
		c8							m_ParentName[LOCATOR_NAME_MAX];
		math::Matrix44	m_LocalMatrix;
	};

	EffectNodeData():m_pBinaryData(NULL){}
	~EffectNodeData(){}

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
	std::vector<EffectNode>					m_LocatorNodes;
};

void EffectNodeData::ReadLocatorsNode( const xml_node<>* locatorsNode )
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

	};

	for( const xml_node<>* locatorNode = locatorsNode->first_node(); locatorNode; locatorNode = locatorNode->next_sibling() )
	{
		EffectNode			node;
		std::string			str;
    
	  strcpy_s( node.m_Name, locatorNode->name() ); 
    
		str = LocalFunc::GetName( locatorNode->first_attribute( "Parent" ) );
		strcpy_s( node.m_ParentName, str.c_str() ); 
        
    // X:
    str = LocalFunc::GetName( locatorNode->first_attribute( "x" ) );
    float x = (float)atof(str.c_str());

    // Y:
    str = LocalFunc::GetName( locatorNode->first_attribute( "y" ) );
    float y = (float)atof(str.c_str());

    // Z:
    str = LocalFunc::GetName( locatorNode->first_attribute( "z" ) );
    float z = (float)atof(str.c_str());
    
    // Set Translation: ( TRANSPOSE_MTX )
    node.m_LocalMatrix.SetElem(0,3,x);
    node.m_LocalMatrix.SetElem(1,3,y);
    node.m_LocalMatrix.SetElem(2,3,z);
        
		m_LocatorNodes.push_back( node );
    
	}
}

void EffectNodeData::CreateBinary()
{
	BinaryMaker					binary;
	EffectNodeDataHeader		header;  

	header.locatorNum = static_cast<s32>(m_LocatorNodes.size());

	binary.Write( &header, sizeof(header) );

	{
		for( u32 i = 0; i < m_LocatorNodes.size(); ++i )
		{
			EffectNode			&rLocatorNodes = m_LocatorNodes[ i ];

			binary.Write( rLocatorNodes.m_Name, sizeof(rLocatorNodes.m_Name) );
			binary.Write( rLocatorNodes.m_ParentName, sizeof(rLocatorNodes.m_ParentName) );
			binary.Write( &rLocatorNodes.m_LocalMatrix, sizeof(math::Matrix44) );
		}
	}

	m_pBinaryData = binary.CreateFixedBuffer();
	m_BinarySize = binary.GetBinarySize();
}

bool EffectNodeData::WriteFile( std::string filePath )
{
  if(m_LocatorNodes.size()==0) return false;
	{
		BinaryMaker		bm;	

		static const u32 sc_Version = 0;
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
//! @brief  GfEffectNodeData::Implクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
class GfEffectNodeData::Impl : public rapidxml::xml_document<>
{
public:
	Impl( c8* pData );
	virtual ~Impl();

	s32 GetSize( void );
	const void* GetData( u32 index );
	u32 GetDataSize( u32 index );
	IResourceData::Type GetType( u32 index );
	bool WriteFile(std::string filePath);

	EffectNodeData		m_LocatorData;
};

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
GfEffectNodeData::Impl::Impl( c8* pData ):rapidxml::xml_document<>()
{
	this->parse< parse_no_utf8 | parse_no_entity_translation >( pData );

	const xml_node<>	*locatorsNode = first_node( "root");
	m_LocatorData.ReadLocatorsNode( locatorsNode );

	m_LocatorData.CreateBinary();
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
GfEffectNodeData::Impl::~Impl()
{
	
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
s32 GfEffectNodeData::Impl::GetSize( void )
{
	return 1;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
bool GfEffectNodeData::Impl::WriteFile(std::string filePath)
{
		return m_LocatorData.WriteFile(filePath);
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
const void* GfEffectNodeData::Impl::GetData( u32 index )
{
	return m_LocatorData.m_pBinaryData;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
u32 GfEffectNodeData::Impl::GetDataSize( u32 index )
{
	return m_LocatorData.m_BinarySize;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
IResourceData::Type GfEffectNodeData::Impl::GetType( u32 index )
{
	return IResourceData::Type("locator");
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  GfLocDataクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
GfEffectNodeData::GfEffectNodeData( c8* pData ) : IResourceData()
{
	assert(pData);
	m_pImpl = new Impl( pData );
}

GfEffectNodeData::~GfEffectNodeData()
{
	delete m_pImpl;
}

//! @brief 全データの数の取得
s32 GfEffectNodeData::GetSize( void )
{
	return m_pImpl->GetSize();
}

bool GfEffectNodeData::WriteFile(std::string filePath)
{
		return( m_pImpl->WriteFile(filePath) );
}

//! @brief データの取得
const void* GfEffectNodeData::GetData( u32 index )
{
	return m_pImpl->GetData(index);
}

//! @brief 個々のデータのサイズ取得
u32 GfEffectNodeData::GetDataSize( u32 index )
{
	return m_pImpl->GetDataSize(index);
}

//! @brief タイプデータの取得
IResourceData::Type GfEffectNodeData::GetType( u32 index )
{
	return m_pImpl->GetType(index);
}


}}}}


#endif
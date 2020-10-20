#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_PARSERUTIL_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_PARSERUTIL_H_INCLUDED

//後日Windowsアプリ用拡張ライブラリを用意して、そちらで提供いたします。
#if defined(GF_PLATFORM_WIN32)

#include <vector>
#include <string>
#include <xml/include/rapidxml.hpp> // no need to link

#include <stdio.h>
#include <types/include/gfl2_Typedef.h>

#include <math/include/gfl2_Vector4.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

//-----------------------------------------------------------------------------
/**
 * @class	GfMdlData
 * @brief	GfMdlDataクラス
 */
class ParserUtil
{
public:

    static const int GFL_MAX_FILEPATH = 2048;

	//! @brief 	文字置き換え関数
	static void ReplaceStr(std::string& str, const std::string& from, const std::string& to);

	//! @brief 	文字列分離
	static std::vector<std::string> SplitStr( std::string& SrcCode, const std::string& separate );

	static void ParseNameArray( std::vector<std::string> *p_out, const rapidxml::xml_node<>* name_array_node );
	static void ParseFloatArray( std::vector<math::Vector4> *p_out, const rapidxml::xml_node<>* float_array_node, int stride, float padding );

	static void FileCreate( const char* filename, const char* p_data, int size );

	//-----------------------------------------------------------------------------
	/// @brief	ファイルパスのみを取得
	//-----------------------------------------------------------------------------
	static void GetFilePath( char* p_out, const char* filename );
	static std::string GetFilePathStr( const char* filename );

	//-----------------------------------------------------------------------------
	/// @brief	ファイル名のみを取得
	//-----------------------------------------------------------------------------
	static int GetFileName( c8* p_out, const c8* filename );
	static std::string GetFileNameStr( const char* filename );

	//-----------------------------------------------------------------------------
	/// @brief	ノード名のみを取得
	//-----------------------------------------------------------------------------
	static void GetNodeName( c8* p_out, s32 bufferSize, const c8* filename );

	//いずれＩＯクラスができたら置き換える。
	static c8* ReadFile( const char* filename, s32* p_size, s32 align = 16 );
	static c8* ReadFile( const char* filename, s32* p_file, s32* p_size, s32 align = 16 );
	static void RemoveFile( const char* filename );

	static std::wstring ShitJIStoUTF16( const std::string& str );
	static std::string UTF16toUTF8( const std::wstring& str );
	static std::string UTF16toShitJIS( const std::wstring& wstr );
	static std::wstring UTF8toUTF16( const std::string& str );

private:
	ParserUtil(){}
	~ParserUtil(){}
};

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  BinaryMakerクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
class BinaryMaker
{
public:
	BinaryMaker(){}
	~BinaryMaker(){}

	void Write( const void* pSrc, u32 size )
	{
		const u8* pData = reinterpret_cast<const u8*>(pSrc);
    m_Buffer.insert(m_Buffer.end(), pData, pData + size);
	}

	u8* CreateFixedBuffer()
	{
		u8* pOut = new u8[m_Buffer.size()];		
    memcpy(pOut, &m_Buffer[0], m_Buffer.size());

    return pOut;
	}

	u32 GetBinarySize()
	{
		return static_cast<u32>( m_Buffer.size() );
	}

	std::vector<u8>				m_Buffer;
};

}}}}

#endif

#endif



/**
* @file    NameManglingManager.cpp
* @brief   名前修飾を管理クラス
* @author  ikuta_junya
* @date    2014.10.09
*/

// @note skeltonから移植、stdやxmlパーサーを使うのでwin32専用
#if defined(GF_PLATFORM_WIN32)

#include "Field/FieldRo/include/Debug/FieldDebugNameManglingManager.h"

//#define RAPIDXML_NO_EXCEPTIONS
#include <xml/include/rapidxml.hpp> // no need to link
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>

#if 0
void rapidxml::parse_error_handler(const char* what, void* where) {
    //printf("Parse error(@%p): %s\n", where, what);
    //std::abort();
   MessageBoxA( NULL, (LPCSTR)what, "GFL_ASSERT_STOP", MB_ICONERROR );
}
#endif


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Debug );
GFL_NAMESPACE_BEGIN( NameMangling );

//
// S32 型
//

NameManglingS32Node::NameManglingS32Node( void ) : m_pInstance( NULL )
{
  m_pHash = new GroupNode("hash");

  // b32対応
  AddHash("true", 1);
  AddHash("false", 0);
}

NameManglingS32Node::~NameManglingS32Node( void )
{
  delete m_pHash;
}

// 名前修飾を設定
void NameManglingS32Node::SetNameMangling( s32* pInstance, const char* pName )
{
  m_pInstance = pInstance;
  m_Name = std::string( pName );
}

// 値を設定
void NameManglingS32Node::SetValue( s32 value ) 
{
  if( m_pInstance == NULL )
  {
    return;
  }

  *m_pInstance = value;
}

// 値をテキストフォーマットで設定
b32 NameManglingS32Node::SetValue( const char* pValue )
{
  if( m_pInstance == NULL )
  {
    return false;
  }

  s32 prev = *m_pInstance;
  s32 work;
  if( sscanf_s( pValue, "%d", &work ) == 1 )
  {
    *m_pInstance = work;
  }
  else if( m_pHash )
  {
    if( m_pHash->GetValue( pValue, &work ) )
    {
      *m_pInstance = work;
    }
  }

  return ( prev != *m_pInstance );
}

// ハッシュ値を追加
b32 NameManglingS32Node::AddHash( const char* pValue, s32 value )
{
  m_pHash->AddNameMangling( pValue, value );
  return true;
}

//
// U32 型
//

NameManglingU32Node::NameManglingU32Node( void ) : m_pInstance( NULL )
{
}

NameManglingU32Node::~NameManglingU32Node( void )
{
}

// 名前修飾を設定
void NameManglingU32Node::SetNameMangling( u32* pInstance, const char* pName )
{
  m_pInstance = pInstance;
  m_Name = std::string( pName );
}

// 値を設定
void NameManglingU32Node::SetValue( u32 value ) 
{
  if( m_pInstance == NULL )
  {
    return;
  }

  *m_pInstance = value;
}

// 値をテキストフォーマットで設定
b32 NameManglingU32Node::SetValue( const char* pValue )
{
  if( m_pInstance == NULL )
  {
    return false;
  }

  u32 prev = *m_pInstance;
  u32 work;
  if( sscanf_s( pValue, "%d", &work ) == 1 )
  {
    *m_pInstance = work;
  }

  return ( prev != *m_pInstance );
}

//
// U16 型
//

NameManglingU16Node::NameManglingU16Node( void ) : m_pInstance( NULL )
{
}

NameManglingU16Node::~NameManglingU16Node( void )
{
}

// 名前修飾を設定
void NameManglingU16Node::SetNameMangling( u16* pInstance, const char* pName )
{
  m_pInstance = pInstance;
  m_Name = std::string( pName );
}

// 値を設定
void NameManglingU16Node::SetValue( u16 value ) 
{
  if( m_pInstance == NULL )
  {
    return;
  }

  *m_pInstance = value;
}

// 値をテキストフォーマットで設定
b32 NameManglingU16Node::SetValue( const char* pValue )
{
  if( m_pInstance == NULL )
  {
    return false;
  }

  u16 prev = *m_pInstance;
  s32 work;
  if( sscanf_s( pValue, "%d", &work ) == 1 )
  {
    *m_pInstance = work;
  }

  return ( prev != *m_pInstance );
}

//
// string(c8) 型
//

NameManglingStringNode::NameManglingStringNode( void ) : m_pInstance( NULL ), m_Size( 0 )
{
}

NameManglingStringNode::~NameManglingStringNode( void )
{

}

// 名前修飾を設定
void NameManglingStringNode::SetNameMangling( const char* pName, c8* pInstance, u32 size )
{
  m_Name = std::string( pName );
  m_pInstance = pInstance;
  m_Size = size;
}

// 値をテキストフォーマットで設定
b32 NameManglingStringNode::SetValue( const char* pValue )
{
  if( m_pInstance == NULL || m_Size <= 1 )
  {
    return false;
  }

  if( strcmp( m_pInstance, pValue ) != 0 )
  {
    strncpy_s( m_pInstance, m_Size, pValue, m_Size - 1 );
    return true;
  }
  else
  {
    return false;
  }

}

//
// wstring(c16) 型
//

NameManglingWStringNode::NameManglingWStringNode( void ) : m_pInstance( NULL ), m_Size( 0 )
{
}

NameManglingWStringNode::~NameManglingWStringNode( void )
{

}

// 名前修飾を設定
void NameManglingWStringNode::SetNameMangling( const char* pName, c16* pInstance, u32 size )
{
  m_Name = std::string( pName );
  m_pInstance = pInstance;
  m_Size = size;
}

// 値をテキストフォーマットで設定
b32 NameManglingWStringNode::SetValue( const char* pValue )
{
  if( m_pInstance == NULL || m_Size <= 1 )
  {
    return false;
  }

  std::wstring text( gfl2::renderingengine::scenegraph::resource::ParserUtil::ShitJIStoUTF16( pValue ) );
  if( wcscmp( text.c_str(), m_pInstance ) != 0 )
  {
    size_t size = text._Copy_s( m_pInstance, m_Size, m_Size - 1 );
    m_pInstance[ size ] = L'\0';
    return true;
  }

  return false;
}


b32 LoadXml( rapidxml::xml_node<>* pXmlNode, GroupNode* pGroupNode )
{
  b32 valueChangeFlag( false );

  for ( rapidxml::xml_node<> *pChildNode  = pXmlNode->first_node(); pChildNode; pChildNode = pChildNode->next_sibling() ) 
  {
    // 設定ノード
    if( pChildNode->value_size() > 0 )
    {
      if( pGroupNode->SetValue( pChildNode->name(), pChildNode->value() ) )
      {
        valueChangeFlag = true;
      }
    }
    // グループノード
    else
    {
      // グループノード
      GroupNode* pChildGroupNode = pGroupNode->GetGroupNode( pChildNode->name() );
      if( pChildGroupNode )
      {
        if( LoadXml( pChildNode, pChildGroupNode ) )
        {
          valueChangeFlag = true;
        }
      }
    }
  }

  if( pGroupNode->GetCallBack() )
  {
    pGroupNode->GetCallBack()->Callback( pGroupNode, valueChangeFlag );
  }

  return valueChangeFlag;
}


b32 GroupNode::LoadAndUpdate( const char* pFilePath )
{
  s32 size;
  c8* pFileData = ReadFile( pFilePath, &size );

  if( pFileData )
  {
    b32 result = UpdateImpl( pFileData );
    delete pFileData;
    return result;
  }
  else
  {
    // ファイルのロードに失敗した
    return false;
  }
}

b32 GroupNode::Update( const char* pData )
{
  std::string data( pData );
  return UpdateImpl( const_cast< c8* >( data.c_str() ) );
}

b32 GroupNode::UpdateImpl( c8* pData )
{
  rapidxml::xml_document<> doc;

  try
  {
    doc.parse<0>( pData );
  }
  catch( rapidxml::parse_error &e )
  {
    GFL_UNUSED( e );
    // MessageBoxA( NULL, "XMLの構文が間違っています", "エラー", MB_ICONERROR );
    return false;
  }

  LoadXml( &doc, this );
  return true;
}

c8* GroupNode::ReadFile( const char* pFileName, s32* pSize, s32 align )
{
	FILE		*hFile;
	int			file_size;
	char		*read_buff;

	fopen_s( &hFile, pFileName, "rb" );

	if ( hFile == NULL ){
		return NULL;
	}

	fseek( hFile, 0, SEEK_END );
	file_size = ftell( hFile );
	*pSize = file_size;
	fseek( hFile, 0, SEEK_SET );

	file_size += align;//必ずNULLが入るようにしたい。
	u32 buf_size = file_size + ((-file_size) & (align-1));

	read_buff = new c8[ buf_size ];
	memset( read_buff, 0, buf_size );
	fread( read_buff, file_size, 1, hFile );
	fclose( hFile );
	
	return( read_buff );
}

GFL_NAMESPACE_END( NameMangling );
GFL_NAMESPACE_END( Debug );
GFL_NAMESPACE_END( Field );


#endif // #if defined(GF_PLATFORM_WIN32)
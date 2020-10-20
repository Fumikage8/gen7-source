//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldTestUtility.h
 *	@brief  フィールドモック環境　一旦汎用的なクラス、や関数置き場
 *	@author	
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "test/FieldTest/include/FieldTestUtility.h"

#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>



// for NodeCountVisitor
#include <util/include/gfl2_FnvHash.h>



GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldTest )
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
*  @brief  ファイル読み込み
*  @param	pFilePath   ファイルパス
*  @param	alignment   アライメント
*/
//-----------------------------------------------------------------------------
void* Utility::ReadData( const c16* pFilePath, u32 alignment )
{
  #if defined(GF_PLATFORM_CTR)
	nn::fs::FileReader file( pFilePath);
	u32 fileSize = file.GetSize();
	void* pBinary = gfl2::gfx::GLMemory::Malloc(fileSize, alignment);
	file.Read( pBinary, fileSize);
	return pBinary;
#elif defined(GF_PLATFORM_WIN32)
	FILE		*hFile;
	int			file_size;
	char		*read_buff;

	_wfopen_s( &hFile, pFilePath, L"rb" );

	if ( hFile == NULL )
	{
    GFL_ASSERT( 0);
		return NULL;
	}

	fseek( hFile, 0, SEEK_END );
	file_size = ftell( hFile );
	fseek( hFile, 0, SEEK_SET );

	u32		align = 16;
	file_size += align;//必ずNULLが入るようにしたい。
	u32 buf_size = file_size + ((-file_size) & (align-1));

	read_buff = reinterpret_cast<c8*>( gfl2::gfx::GLMemory::Malloc(buf_size, alignment) );
	memset( read_buff, 0, buf_size );
	fread( read_buff, file_size, 1, hFile );
	fclose( hFile );

	return read_buff;
#endif
}

//----------------------------------------------------------------------------
/**
*  @brief  ファイル読み込み
*  @param  pHeap       ヒープ
*  @param  pFilePath   ファイルパス
*  @param  alignment   アライメント
*/
//-----------------------------------------------------------------------------
void* Utility::ReadData( gfl2::heap::HeapBase *pHeap, const c16* pFilePath, u32 alignmen )
{
#if defined(GF_PLATFORM_CTR)
  nn::fs::FileReader file( pFilePath );
  u32 fileSize = file.GetSize();
  void* pShaderBinary =  GflHeapAllocMemoryAlign( pHeap,fileSize,alignmen );
  file.Read(pShaderBinary, fileSize);
  return pShaderBinary;
#else
  FILE    *hFile;
  int      file_size;
  char    *read_buff;

  _wfopen_s( &hFile, pFilePath, L"rb" );

  if ( hFile == NULL )
  {
    GFL_ASSERT(0);
    return NULL;
  }

  fseek( hFile, 0, SEEK_END );
  file_size = ftell( hFile );
  fseek( hFile, 0, SEEK_SET );

  u32    align = alignmen;
  file_size += align;//必ずNULLが入るようにしたい。
  u32 buf_size = file_size + ((-file_size) & (align-1));

  read_buff = reinterpret_cast<c8*>( GflHeapAllocMemoryAlign( pHeap,buf_size,alignmen) );
  memset( read_buff, 0, buf_size );
  fread( read_buff, file_size, 1, hFile );
  fclose( hFile );

  return read_buff;
#endif
}

// Node数をカウントするvisitor

NodeCountVisitor::NodeCountVisitor() : m_NodeCount( 0 ), m_UnknownNodeCount( 0 ), m_UnknownNodeCountOfInheritance( 0 )
{
  for( u32 i = 0; i < GFL_NELEMS( m_NodeCountTable ); i++ )
  {
    m_NodeCountTable[i].pNodeName = sc_NodeCountVisitTargetNameTable[i];
    m_NodeCountTable[i].hash = gfl2::util::FnvHash( reinterpret_cast<u8*>( const_cast<char*>( m_NodeCountTable[i].pNodeName ) ) );
    m_NodeCountTable[i].count = 0;
    m_NodeCountTable[i].countOfInheritance = 0;
  }
}

NodeCountVisitor::~NodeCountVisitor()
{

}

void NodeCountVisitor::Visit( gfl2::renderingengine::scenegraph::DagNode* pNode )
{
  m_NodeCount++;
 
  u32 inheritanceCount = 0;
  const gfl2::renderingengine::scenegraph::Inheritance* pInheritance = pNode->GetInheritance();
  while( pInheritance )
  {
    b32 hitFlag = false;
    for( u32 i = 0; i < GFL_NELEMS( m_NodeCountTable ); i++ )
    {
      // ハッシュが同じならカウント
      if( pInheritance->m_ThistTypeValue == m_NodeCountTable[i].hash )
      {
        hitFlag = true;

        if( inheritanceCount == 0 )
        {
          // 実ノード数
          m_NodeCountTable[ i ].count += 1;
        }
        else
        {
          // 継承されている数
          m_NodeCountTable[ i ].countOfInheritance += 1;
        }
      }
    }

    // 知らない型
    if( hitFlag == false )
    {

      if( inheritanceCount == 0 )
      {
        // 実ノード数
        m_UnknownNodeCount += 1;
      }
      else
      {
        // 継承されている数
        m_UnknownNodeCountOfInheritance += 1;
      }
    }

    pInheritance = pInheritance->m_pBaseType;
    inheritanceCount += 1;
  }

}

b32 NodeCountVisitor::GetNodeCountData( u32 index, NODE_COUNT_DATA* pOut ) const
{
  if( index >= GetNodeCountDataNum() )
  {
    return false;
  }

  *pOut = m_NodeCountTable[ index ];
  return true;

}

GFL_NAMESPACE_END( FieldTest )
GFL_NAMESPACE_END( Test )

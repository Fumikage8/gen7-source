﻿//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl2_BinLinkerAccessor.cpp
 *	@brief  tools/tools/binlinker.rbで生成したデータへアクセスするためのクラス
 *	@author	tomoya takahashi
 *	@date		2011.01.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include <stddef.h> //NULL
#include <fs/include/gfl2_BinLinkerAccessor.h>


namespace gfl2{
namespace fs{


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
 *	@brief    コンストラクタ    初期化も行う。
 *
 *	@param	p_buff   BinLinkerで生成したデータのポインタ
 */
//-----------------------------------------------------------------------------
BinLinkerAccessor::BinLinkerAccessor( void* p_buff ) :
  m_pBuff(NULL)
{
  Initialize( p_buff );
}

//----------------------------------------------------------------------------
/**
 *	@brief    コンストラクタ    初期化は行わない
 */
//-----------------------------------------------------------------------------
BinLinkerAccessor::BinLinkerAccessor( void ) :
  m_pBuff(NULL)
{
}


//----------------------------------------------------------------------------
/**
 *	@brief    デストラクタ
 */
//-----------------------------------------------------------------------------
BinLinkerAccessor::~BinLinkerAccessor()
{
}


//----------------------------------------------------------------------------
/**
 *	@brief  初期化    何度呼んでも大丈夫です。
 *
 *	@param	p_buff   BinLinkerで生成したデータのポインタ
 */
//-----------------------------------------------------------------------------
void BinLinkerAccessor::Initialize( void* p_buff )
{
  m_pBuff = reinterpret_cast<BinLinkerHeader*>(p_buff);
}


//----------------------------------------------------------------------------
/**
 *	@brief  パッケージされているファイルの最大数を取得
 *
 *	@return パッケージされているファイルの最大数
 */
//-----------------------------------------------------------------------------
u16 BinLinkerAccessor::GetDataMax( void ) const
{
  u16 filenum = 0;
  if( m_pBuff ){
    filenum = m_pBuff->filenum;
  }
  return filenum;
}


//----------------------------------------------------------------------------
/**
 *	@brief  パッケージされているファイルのポインタを取得
 *
 *	@param	idx インデックス
 *
 *	@retval ファイルのポインタ
 *	@retval NULL idxが不正または、初期化されていない。
 */
//-----------------------------------------------------------------------------
u32 BinLinkerAccessor::GetDataSize( u32 idx ) const
{
  if( m_pBuff ){
    if( m_pBuff->filenum > idx ){
      return m_pBuff->offset[idx + 1] - m_pBuff->offset[idx];
    }
  }
  return 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  パッケージされているファイルのポインタを取得
 *
 *	@param	idx インデックス
 *
 *	@retval ファイルのポインタ
 *	@retval NULL idxが不正または、初期化されていない。
 */
//-----------------------------------------------------------------------------
void* BinLinkerAccessor::GetData( u32 idx ) const
{
  void* p_fileptr = NULL;
  if( m_pBuff ){
    if( m_pBuff->filenum > idx ){
      uptr file_top = reinterpret_cast<uptr>(m_pBuff);
      p_fileptr = reinterpret_cast<void*>(file_top + m_pBuff->offset[idx]);
    }
  }
  return p_fileptr;
}

//----------------------------------------------------------------------------
/**
 *	@brief  シグネチャの取得
 *
 *	@return データのシグネチャ
 */
//-----------------------------------------------------------------------------
u16 BinLinkerAccessor::GetSignature( void ) const
{
  if( m_pBuff ){
    return m_pBuff->signature;
  }
  return 0;
}


} // namespace fs
} // namespace gfl2

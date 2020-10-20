#if PM_DEBUG

//======================================================================
/**
 * @file IconTestPokeIcon.cpp
 * @date 2015/07/17 18:26:02
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "Test/FukushimaTest/source/IconTest/IconTestPokeIcon.h"

#include <AppLib/include/Tool/PokeIcon.h>
#include <AppLib/include/Util/app_util_heap.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

//------------------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------------------
PokeIconTool::PokeIconTool( app::util::Heap* pHeap, u32 max )
  : m_pHeap( pHeap )
  , m_pPokeIcon( NULL )
  , m_UpdateSeq( 0 )
  , m_DeleteSeq( 0 )
  , m_IconMax( max )
  , m_RequestTable( NULL )
{
  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  // リクエスト配列
  m_RequestTable = GFL_NEW_ARRAY(devHeap) bool[m_IconMax];
  GFL_ASSERT( m_RequestTable );
  for( u32 i=0; i<m_IconMax; ++i )
  {
    m_RequestTable[i] = false;
  }

  // ポケモンアイコンの生成
  m_pPokeIcon = GFL_NEW(devHeap) app::tool::PokeIcon( devHeap, devHeap, m_IconMax );
  GFL_ASSERT( m_pPokeIcon );
  m_pPokeIcon->FileOpenSync( devHeap );   //!< 同期版
}

//------------------------------------------------------------------------------
/**
 * @brief	  デストラクタ
 */
//------------------------------------------------------------------------------
PokeIconTool::~PokeIconTool( void )
{
  GFL_SAFE_DELETE( m_pPokeIcon );

  GFL_SAFE_DELETE_ARRAY( m_RequestTable );
}


//------------------------------------------------------------------------------
/**
 * @brief	  終了処理
 */
//------------------------------------------------------------------------------
bool PokeIconTool::DeleteFunc( void )
{
  switch( m_UpdateSeq )
  {
  case 0:   //!< 終了の開始
    {
      m_pPokeIcon->FileCloseSync();

      m_DeleteSeq++;
    } // break;

  case 1:
    {
      if( !m_pPokeIcon->IsModuleFree() ) break;

      m_DeleteSeq++;
    } // break;

  case 2:
    {
      return true;
    } // break;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンアイコンの読み込み
 */
//------------------------------------------------------------------------------
void PokeIconTool::LoadPokeIcon( u32 id, const pml::pokepara::PokemonParam* pp )
{
  if( m_RequestTable[id] )
  {
    // 読み込みをキャンセル
    bool res = m_pPokeIcon->CancelLoadTextureRequest( id );

    GFL_ASSERT( res );    //!< 読み込みキャンセルに失敗

    m_RequestTable[id] = false;
  }

  // 読み込みリクエスト
  if( m_pPokeIcon->ReadRequest( id, pp ) )
  {
    m_RequestTable[id] = true;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   テクスチャの置き換え
 * @return  "true" = 成功
 * @return  "false" = 失敗
 */
//------------------------------------------------------------------------------
bool PokeIconTool::ReplaceTexture( u32 id, gfl2::lyt::LytPicture* pane )
{
  // 置き換えるペイン情報がない
  if( !pane ) return false;
  // リクエストしていない
  if( !m_RequestTable[id] ) return false;
  // テクスチャの読み込みができていない
  if( !m_pPokeIcon->IsLoadFinished( id ) ) return false;

  // テクスチャの置き換え
  m_pPokeIcon->ReplaceReadTexture( id, pane );

  m_RequestTable[id] = false;

  return true;
}


GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG
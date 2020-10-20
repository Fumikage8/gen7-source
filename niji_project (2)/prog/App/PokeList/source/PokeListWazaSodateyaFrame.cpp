//==============================================================================
/**
 * @file    PokeListWazaSodateyaFrame.cpp
 * @author  fukushima_yuya
 * @date    2015.04.08
 * @brief   ポケモンリスト画面　技教え・育て屋
 */
//==============================================================================

// niji
#include "App/PokeList/source/PokeListWazaSodateyaFrame.h"
#include "App/PokeList/source/PokeListWazaSodateyaDraw.h"
#include "App/PokeList/source/work/PokeListStaticWork.h"

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/common_pokelist.gaix>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
PokeListWazaSodateyaFrame::PokeListWazaSodateyaFrame( void )
  : m_pPokeListDraw( NULL )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
PokeListWazaSodateyaFrame::~PokeListWazaSodateyaFrame( void )
{
  ;
}



//------------------------------------------------------------------------------
/**
 * @brief   PokeListのセットアップ
 */
//------------------------------------------------------------------------------
void PokeListWazaSodateyaFrame::SetupPokeList( void )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   PokeListDrawの生成
 */
//------------------------------------------------------------------------------
void PokeListWazaSodateyaFrame::CreatePokeList( void )
{
  gfl2::heap::HeapBase * devHeap = m_pAppHeap->GetDeviceHeap();

  m_pPokeListDraw = GFL_NEW(devHeap) PokeListWazaSodateyaDraw( m_pAppHeap, m_pAppRenderingManager );
  m_pPokeListDraw->Init( m_pAppParam, m_pArcReadBuff, m_pStaticWork, m_pLocalWork );
  m_pPokeListDraw->SetTitleViewUIListener( this );
  this->SetBaseDraw( m_pPokeListDraw );
}

//------------------------------------------------------------------------------
/**
 * @brief   PokeListDrawの破棄
 */
//------------------------------------------------------------------------------
bool PokeListWazaSodateyaFrame::EndPokeList( void )
{
  if( !m_pPokeListDraw->End() ) return false;

  GFL_SAFE_DELETE( m_pPokeListDraw );

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   PokeListDrawの更新
 */
//------------------------------------------------------------------------------
void PokeListWazaSodateyaFrame::UpdatePokeList( void )
{
  if( m_pPokeListDraw ) m_pPokeListDraw->UpdateTree();
}

//------------------------------------------------------------------------------
/**
 * @brief   PokeListDrawの描画
 */
//------------------------------------------------------------------------------
void PokeListWazaSodateyaFrame::DrawPokeList( gfl2::gfx::CtrDisplayNo no )
{
  if( m_pPokeListDraw ) m_pPokeListDraw->DrawTree( no );
}



//------------------------------------------------------------------------------
/**
 * @brief   ポケモンリストの終了
 */
//------------------------------------------------------------------------------
void PokeListWazaSodateyaFrame::PokeListEnd( void )
{
  m_pContext->pokelist_wait = true;

  SetSubSeq( MM_SEQ_END );
}

//------------------------------------------------------------------------------
/**
 * @brief   外部設定パラメータの設定
 * @param   endMode   終了モード
 */
//------------------------------------------------------------------------------
void PokeListWazaSodateyaFrame::SetupOutData( EndMode endMode )
{
  switch( endMode )
  {
  case END_MODE_SELECT:   //!< 選択したポケモンのインデックスを返す
    {
      CursorPos cpos = m_pStaticWork->GetCursorPos( CID_POKE );
      m_pContext->poke_index = m_pStaticWork->GetPokeIndex( cpos );

      m_pPokeListDraw->PokeListClose( endMode );
    } break;
  }
}


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

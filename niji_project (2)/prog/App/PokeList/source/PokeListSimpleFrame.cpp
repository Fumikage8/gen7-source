//==============================================================================
/**
 * @file    PokeListSimpleFrame.cpp
 * @author  fukushima_yuya
 * @date    2015.03.23
 * @brief   ポケモンリスト画面対象選択フレーム
 */
//==============================================================================

// niji
#include "App/PokeList/source/PokeListSimpleFrame.h"
#include "App/PokeList/source/PokeListSimpleDraw.h"
#include "App/PokeList/source/parts/PokeListCursorDraw.h"
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
PokeListSimpleFrame::PokeListSimpleFrame( void )
: m_pPokeListDraw( NULL )
, m_Closed( true )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
PokeListSimpleFrame::~PokeListSimpleFrame( void )
{
  ;
}



//------------------------------------------------------------------------------
/**
 * @brief   PokeListのセットアップ
 */
//------------------------------------------------------------------------------
void PokeListSimpleFrame::SetupPokeList( void )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   PokeListDrawの生成
 */
//------------------------------------------------------------------------------
void PokeListSimpleFrame::CreatePokeList( void )
{
  gfl2::heap::HeapBase * devHeap = m_pAppHeap->GetDeviceHeap();

  m_pPokeListDraw = GFL_NEW(devHeap) PokeListSimpleDraw( m_pAppHeap, m_pAppRenderingManager );
  m_pPokeListDraw->Init( m_pAppParam, m_pArcReadBuff, m_pStaticWork, m_pLocalWork );
  m_pPokeListDraw->SetTitleViewUIListener( this );
  this->SetBaseDraw( m_pPokeListDraw );

  m_Closed = false;
}

//------------------------------------------------------------------------------
/**
 * @brief   PokeListDrawの破棄
 */
//------------------------------------------------------------------------------
bool PokeListSimpleFrame::EndPokeList( void )
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
void PokeListSimpleFrame::UpdatePokeList( void )
{
  if( m_pPokeListDraw ) m_pPokeListDraw->UpdateTree();

  // 外部からの強制終了のチェック
  CheckListBreak();
}

//------------------------------------------------------------------------------
/**
 * @brief   PokeListDrawの描画
 */
//------------------------------------------------------------------------------
void PokeListSimpleFrame::DrawPokeList( gfl2::gfx::CtrDisplayNo no )
{
  if( m_pPokeListDraw ) m_pPokeListDraw->DrawTree( no );
}


//------------------------------------------------------------------------------
/**
 * @brief   ポーズ中かどうか
 *
 * @return  "true  = ポーズ中"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool PokeListSimpleFrame::IsPause( void ) const
{
  if( m_pPokeListDraw )
  {
    bool isSeqMain   = m_pPokeListDraw->IsSeqMain();
    bool inputEnable = m_pPokeListDraw->IsInputEnabled();

    // 入力が無効の場合、ポーズ中と判定
    return (isSeqMain && !inputEnable);
  }

  return false;
}


//------------------------------------------------------------------------------
/**
 * @brief   外部からの強制終了のチェック
 */
//------------------------------------------------------------------------------
void PokeListSimpleFrame::CheckListBreak( void )
{
  if( m_pStaticWork->GetProcMode() != MODE_BATTLE )
  {
    // バトル以外は何もしない
    return;
  }

  if( m_pPokeListDraw && !m_pPokeListDraw->IsSeqMain() )
  {
    // メインシーケンスでなければ何もしない
    return;
  }

  if( m_Closed )
  {
    // 閉じるをリクエスト済み
    return;
  }

  if( m_pStaticWork->IsListBreak() )
  {
    if( m_pPokeListDraw ) m_pPokeListDraw->PokeListClose( END_MODE_TIMEUP );   // @fix NMCat[4207]

    m_Closed = true;
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   ポケモンリストの終了
 */
//------------------------------------------------------------------------------
void PokeListSimpleFrame::PokeListEnd( void )
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
void PokeListSimpleFrame::SetupOutData( EndMode endMode )
{
  if( endMode == END_MODE_SELECT )
  {
    // 選択したポケモンのインデックスを返す
    CursorPos cpos = m_pStaticWork->GetCursorPos( CID_POKE );
    m_pContext->poke_index = m_pStaticWork->GetPokeIndex( cpos );

    if( m_pStaticWork->GetProcMode() == MODE_BATTLE )
    {
      m_pContext->select_poke_addr = m_pStaticWork->GetSelectCursorPos( cpos );
    }
  }
}


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

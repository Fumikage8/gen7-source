#if PM_DEBUG

//==============================================================================
/**
 * @file    PokeListTestFieldFrame.cpp
 * @author  fukushima_yuya
 * @date    2015.06.22
 * @brief   フィールド用ポケモンリストテストフレーム
 */
//==============================================================================

// niji
#include "Test/FukushimaTest/source/PokeListTest/Field/PokeListTestFieldDraw.h"
#include "Test/FukushimaTest/source/PokeListTest/Field/PokeListTestFieldFrame.h"

#include <App/PokeList/source/work/PokeListStaticWork.h>
#include <AppLib/include/Util/app_util_heap.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
TestPokeListFieldFrame::TestPokeListFieldFrame( void )
  : m_pPokeListDraw( NULL )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
TestPokeListFieldFrame::~TestPokeListFieldFrame( void )
{
  ;
}



//------------------------------------------------------------------------------
/**
 * @brief   PokeListのセットアップ
 */
//------------------------------------------------------------------------------
void TestPokeListFieldFrame::SetupPokeList( void )
{
  gfl2::heap::HeapBase * devHeap = m_pAppHeap->GetDeviceHeap();

  m_pStaticWork = GFL_NEW(devHeap) App::PokeList::StaticWork( m_pAppParam->mpGameManager, devHeap, m_pContext, NULL );
}

//------------------------------------------------------------------------------
/**
 * @brief   PokeListDrawの生成
 */
//------------------------------------------------------------------------------
void TestPokeListFieldFrame::CreatePokeList( void )
{
  gfl2::heap::HeapBase * devHeap = m_pAppHeap->GetDeviceHeap();

  m_pPokeListDraw = GFL_NEW(devHeap) TestPokeListFieldDraw( m_pAppHeap, m_pAppRenderingManager );
  m_pPokeListDraw->Init( m_pAppParam, m_pArcReadBuff, m_pStaticWork, NULL );
  m_pPokeListDraw->SetTitleViewUIListener( this );
}

//------------------------------------------------------------------------------
/**
 * @brief   PokeListDrawの破棄
 */
//------------------------------------------------------------------------------
bool TestPokeListFieldFrame::EndPokeList( void )
{
  if( !m_pPokeListDraw->End() ) return false;

  GFL_SAFE_DELETE( m_pPokeListDraw );
  GFL_SAFE_DELETE( m_pStaticWork );

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   PokeListDrawの更新
 */
//------------------------------------------------------------------------------
void TestPokeListFieldFrame::UpdatePokeList( void )
{
  if( m_pPokeListDraw ) m_pPokeListDraw->UpdateTree();
}

//------------------------------------------------------------------------------
/**
 * @brief   PokeListDrawの描画
 */
//------------------------------------------------------------------------------
void TestPokeListFieldFrame::DrawPokeList( gfl2::gfx::CtrDisplayNo no )
{
  if( m_pPokeListDraw ) m_pPokeListDraw->DrawTree( no );
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンリストの終了
 */
//------------------------------------------------------------------------------
void TestPokeListFieldFrame::PokeListEnd( void )
{
  SetSubSeq( MM_SEQ_END );
}

//------------------------------------------------------------------------------
/**
 * @brief   外部設定パラメータの設定
 * @param   endMode   終了モード
 */
//------------------------------------------------------------------------------
void TestPokeListFieldFrame::SetupOutData( App::PokeList::EndMode endMode )
{

}


GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )


#endif // PM_DEBUG
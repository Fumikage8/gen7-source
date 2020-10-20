// ============================================================================
/**
 * @file	MutoTest_TestProc.h
 * @brief	Test Process
 * @date	2015.05.11
 */
// ============================================================================
#if PM_DEBUG

// ============================================================================
/*	includes	*/
// ============================================================================
#include <heap/include/gfl2_Heap.h>
#include <System/include/HeapDefine.h>

#include  "../include/Mutotest_TestProc.h"



// ============================================================================
/*  defs  */
// ============================================================================


// ============================================================================
/*  local prms  */
// ============================================================================
namespace	{
  static const int    frameMax  = 30;
}


/*
  Test::MutoTest
*/
namespace Test  {
namespace MutoTest  {
// ============================================================================
/*  funcs */
// ============================================================================

//-----------------------------------------------------------------------------
/**
 *  @func   TestProc
 *  @brief  ctor
 */
//-----------------------------------------------------------------------------
TestProc::TestProc(void)
  : GameSys::GameProc()
  , m_Heap()
  , m_TextDrawer()
{
}


//-----------------------------------------------------------------------------
/**
 *  @func   ~TestProc
 *  @brief  dtor
 */
//-----------------------------------------------------------------------------
TestProc::~TestProc()
{
}


//-----------------------------------------------------------------------------
/**
 *  @func     InitFunc
 *  @brief    PROCの初期化を行う関数
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result	TestProc::InitFunc(gfl2::proc::Manager* pManager)
{
  /*
    RES_CONTINUE,		///<動作継続中
    RES_FINISH,				///<動作終了
  */
  gfl2::proc::Result	result	= gfl2::proc::RES_CONTINUE;


  CreateHeap();
  m_TextDrawer.Initialize(&m_Heap);

  result	= gfl2::proc::RES_FINISH;
  return result;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
gfl2::proc::Result TestProc::EndFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result	result	= gfl2::proc::RES_CONTINUE;



  m_TextDrawer.Finalize();
  DeleteHeap();

  result	= gfl2::proc::RES_FINISH;
  return result;
}


//-----------------------------------------------------------------------------
/**
  * @func UpdateFunc
  * @brief	PROCのメイン更新を行う関数
  * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
  */
//-----------------------------------------------------------------------------
gfl2::proc::Result  TestProc::UpdateFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result	result	= gfl2::proc::RES_CONTINUE;


  m_TextDrawer.CLS();
  m_TextDrawer.LOC(0,0);
  m_TextDrawer.Print(L"this is test");
  m_TextDrawer.LOC(0,1);
  m_TextDrawer.Print(L"this is test too");


  return result;
}


//-----------------------------------------------------------------------------
/**
  * @func   PreDrawFunc
  * @brief  PROCの描画処理(下準備)
  * @param pManager   プロセス管理マネージャー
  * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
  * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
  */
//-----------------------------------------------------------------------------
void  TestProc::PreDrawFunc( gfl2::proc::Manager* pManager )
{
}


//------------------------------------------------------------------
/**
  * @func   DrawFunc
  * @brief  PROCの描画処理
  * @param  pManager   プロセス管理マネージャー
  * @param  displayNo  描画対象画面
  */
//------------------------------------------------------------------
void  TestProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
#if 0
  ../../../../../gflib2/gfx/include/gfl2_enums.h

  //! @brief CTRのディスプレイ番号
  DECLARE_ENUM_BEGIN( CtrDisplayNo )

    LEFT,           //!< 3DSの左画面 400x240
    RIGHT,          //!< 3DSの右画面 400x240
    DOWN,           //!< 3DSの下画面 320x240
    
  	NumberOf,

  DECLARE_ENUM_END( CtrDisplayNo )
#endif
  m_TextDrawer.Draw(displayNo);
}





/*
*/
void TestProc::CreateHeap( void )
{
  gfl2::heap::HeapBase*   pAppHeapBase  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  gfl2::heap::HeapBase*   pDevHeapBase  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  const size_t            appHeapSize   = pAppHeapBase->GetTotalAllocatableSize() * 2 / 10 - 0x0100;
  const size_t            devHeapSize   = pDevHeapBase->GetTotalAllocatableSize() * 8 / 10 - 0x0100;

  m_Heap.LocalHeapCreate( pAppHeapBase, pDevHeapBase, appHeapSize, devHeapSize);
}


/*
*/
void TestProc::DeleteHeap( void )
{
  m_Heap.Delete();
}




} /*  namespace MutoTest  */
} /*  namespace Test      */

#endif // PM_DEBUG

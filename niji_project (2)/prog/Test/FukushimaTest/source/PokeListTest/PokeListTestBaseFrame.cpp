#if PM_DEBUG

//==============================================================================
/**
 * @file    PokeListTestBaseFrame.h
 * @author  fukushima_yuya
 * @date    2015.06.22
 * @brief   ポケモンリストベースフレーム
 */
//==============================================================================

// niji
#include "Test/FukushimaTest/source/PokeListTest/PokeListTestBaseFrame.h"

#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/app_util_heap.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/common_pokelist.gaix>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace
{
  enum{
    MM_SEQ_NONE,
    MM_SEQ_END
  };

  enum{
    LOCAL_OPEN_START,
    LOCAL_READ_START,
    LOCAL_READ_WAIT,
    LOCAL_CLOSE_WAIT
  };
}


//------------------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------------------
TestPokeListBaseFrame::TestPokeListBaseFrame( void )
{
  
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
TestPokeListBaseFrame::~TestPokeListBaseFrame()
{
  
}

//------------------------------------------------------------------------------
/**
 * @brief	  PROCの初期化を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------------------
applib::frame::Result TestPokeListBaseFrame::InitFunc( void )
{
  GFL_ASSERT( m_pAppParam );

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();
  u32 arcID = ARCID_CMN_POKELIST;
  u32 dataID = GARC_common_pokeList_common_pokeList_applyt_COMP;

  switch( m_ReadSeq )
  {
  case LOCAL_OPEN_START:
    {
      app::util::FileAccessor::FileOpen( arcID, devHeap );
      m_ReadSeq = LOCAL_READ_START;
    }
    //break;  //!< no break

  case LOCAL_READ_START:
    {
      if( !app::util::FileAccessor::IsFileOpen( arcID ) ) break;

      app::util::FileAccessor::FileLoad(
        arcID,
        dataID,
        &m_pArcReadBuff,
        devHeap,
        true,
        nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );

      m_ReadSeq = LOCAL_READ_WAIT;
    }
    //break;  //!< no break

  case LOCAL_READ_WAIT:
    {
      if( !app::util::FileAccessor::IsFileLoad( &m_pArcReadBuff ) ) break;

      CreatePokeList();   //!< 継承先で個別にDRAWを生成する

      app::util::FileAccessor::FileClose( arcID, devHeap );

      m_ReadSeq = LOCAL_CLOSE_WAIT;
    }
    //break;  //!< no break

  case LOCAL_CLOSE_WAIT:
    {
      if( !app::util::FileAccessor::IsFileClose( arcID ) ) break;

      m_ReadSeq = 0;

      return applib::frame::RES_FINISH;
    }
    //break;  //!< no break
  }

  return applib::frame::RES_CONTINUE;
}

//------------------------------------------------------------------------------
/**
 * @brief	  PROCのメイン更新を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------------------
applib::frame::Result TestPokeListBaseFrame::UpdateFunc( void )
{
  //if( m_pPokeListDraw ) m_pPokeListDraw->UpdateTree();
  UpdatePokeList();

  applib::frame::Result result;

  switch( GetSubSeq() )
  {
  case MM_SEQ_END:
    {
      result = applib::frame::RES_FINISH;
    } break;

  default:
    {
      result = applib::frame::RES_CONTINUE;
    }
  }

  return result;
}

//------------------------------------------------------------------------------
/**
 * @brief	  PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------------------
void TestPokeListBaseFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  //if( m_pPokeListDraw ) m_pPokeListDraw->DrawTree();
  DrawPokeList( no );
}

//------------------------------------------------------------------------------
/**
 * @brief	  PROCの終了を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------------------
applib::frame::Result TestPokeListBaseFrame::EndFunc( void )
{
  if( !EndPokeList() )
  {
    return applib::frame::RES_CONTINUE;
  }

  GflHeapFreeMemory( m_pArcReadBuff );

  return applib::frame::RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief	  動作に必要な引数を登録
 */
//------------------------------------------------------------------------------
void TestPokeListBaseFrame::Setup( App::PokeList::APP_PARAM* pAppParam, App::PokeList::CONTEXT_PARAM* pContext )
{
  m_pAppParam = pAppParam;
  m_pContext = pContext;

  m_pAppHeap = m_pAppParam->mpAppHeap;
  m_pAppRenderingManager = m_pAppParam->mpAppRenderingManager;

  SetupPokeList();
  
  //m_pStaticWork = GFL_NEW(m_pAppHeap->GetDeviceHeap())
  //  StaticWork( NULL, m_pAppHeap->GetDeviceHeap(), m_pContext, m_pTimeLimit );
}

GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

#endif // PM_DEBUG
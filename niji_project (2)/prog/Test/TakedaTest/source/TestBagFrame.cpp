//=============================================================================
/**
 * @file    BagFrame.cpp
 * @brief   バッグ画面フレームワーク
 * @author  Hiroyuki Nakamura
 * @date    2015.03.04
 */
//=============================================================================

#if PM_DEBUG

// module
#include "Test/TakedaTest/include/TestBagFrame.h"
#include "Field/FieldRo/include/Window/FieldTalkWindow.h"
#include "Field/FieldRo/include/Window/FieldYesNoWindow.h"
#include "Field/FieldRo/include/Window/FieldWindowManager.h"

#include "GameSys/include/DllProc.h"
#include "App/FieldBag/include/BagProc.h"
#include "App/AppEvent/include/Bag/BagEvent.h"

#include "arc_index/message.gaix"
#include "niji_conv_header/message/msg_bag.h"

GFL_NAMESPACE_BEGIN(takedaTest_app)

enum MainSequenceID
{
  SEQ_START = 0,
  SEQ_MAIN,
  SEQ_WAIT_END,
  SEQ_END
};


//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
BagFrame::BagFrame(void)
  : m_heap( NULL )
  , m_pTalkWindow( NULL )
  , m_pYesNoWindow( NULL )
  , m_mainSeq( 0 )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
BagFrame::~BagFrame()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    Setup
 * @brief   外部設定パラメータをセット
 * @date    2015.03.04
 *
 * @param   param             外部設定パラメータ
 * @parma   heap              ヒープ
 * @param   rendering_manager 描画マネージャー
 */
//-----------------------------------------------------------------------------
void BagFrame::Setup( app::util::Heap * heap, s32 idx )
{
  m_heap             = heap;
  m_iIdx             = idx;
}


//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   フレーム初期化
 * @date    2015.03.04
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result BagFrame::InitFunc(void)
{
  /*  デバッグランチャーでのテスト起動ではFieldMapは作られないので、以下のメソッドを gamemanager をに追加する.
    r:/home/niji/git_program/niji_project/prog/GameSys/include/GameManager.h(94):   public:
  #if PM_DEBUG
    Field::FieldWindow::FieldWindowManager  *m_pFieldWindowManager;
    void  SetFieldWindowManager( Field::FieldWindow::FieldWindowManager *_p ){    m_pFieldWindowManager = _p; }
    Field::FieldWindow::FieldWindowManager  *GetFieldWindowManager(){ return m_pFieldWindowManager; }
    #define USE_GAMESYS_FIELDWINDOW
  #endif
    以下参考
    TestBagProc.cpp(98):   GameSys::GameManager::GetInstance()->SetFieldWindowManager( m_pFieldWindowManager );
  */
#ifdef USE_GAMESYS_FIELDWINDOW
  #define _PARAM     GameSys::GameManager::GetInstance()->GetFieldWindowManager()
  #define _PARAM2    GameSys::GameManager::GetInstance()->GetFieldmap()
#else
  #define _PARAM     NULL
  #define _PARAM2    NULL
#endif

  switch( m_iIdx )
  {
    case 1:
      {
        App::Event::BagEvent * ev_bag = App::Event::BagEvent::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager) );
        App::Event::BagEvent::SetupLauncherCall( true );
      }
      //m_pTalkWindow = GFL_NEW(m_heap->GetDeviceHeap()) Field::FieldWindow::FieldTalkWindow( static_cast<Field::Fieldmap*>(_PARAM2) );
      break;
    case 2:
      m_pYesNoWindow = GFL_NEW(m_heap->GetDeviceHeap()) Field::FieldWindow::FieldYesNoWindow( _PARAM );
      break;
  }
  
  m_mainSeq = 0;
  return applib::frame::RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFunc
 * @brief   フレーム終了
 * @date    2015.03.04
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result BagFrame::EndFunc(void)
{
  switch( m_mainSeq )
  {
  case 0:
    if( DeleteBagDraw() == false )
    {
      return applib::frame::RES_CONTINUE;
    }
    m_mainSeq++;
    /* FALL THROUGH */
  case 1:
    m_mainSeq++;
  case 2:
    m_mainSeq = 0;
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateFunc
 * @brief   フレーム更新
 * @date    2015.03.04
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result BagFrame::UpdateFunc(void)
{
  if( m_pTalkWindow )
  {
    m_pTalkWindow->Update();
    if( m_pTalkWindow->GetDecide() )
    {
      SetSubSeq( SEQ_END );
    }
  }
  if( m_pYesNoWindow )
  {
    if( m_pYesNoWindow->GetDecide() )
    {
      SetSubSeq( SEQ_END );
    }
  }

  switch( GetSubSeq() )
  {
  case SEQ_END:
    return applib::frame::RES_FINISH;
  }
  return applib::frame::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    DrawFunc
 * @brief   フレーム描画
 * @date    2015.03.04
 *
 * @param   no  描画先のディスプレイ番号
 */
//-----------------------------------------------------------------------------
void BagFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
}



bool BagFrame::DeleteBagDraw(void)
{
  GFL_SAFE_DELETE( m_pTalkWindow );
  GFL_SAFE_DELETE( m_pYesNoWindow );
  
  return true;
}

GFL_NAMESPACE_END(takedaTest_app)

#endif // PM_DEBUG

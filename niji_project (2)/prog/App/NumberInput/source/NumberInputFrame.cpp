//======================================================================
/**
 * @file    NumberInputFrame.cpp
 * @date    2015/11/24 12:29:05
 * @author  fukushima_yuya
 * @brief   英数字入力アプリ：Frame
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/NumberInput/include/NumberInputFrame.h>

#include "NumberInputDraw.h"
#include "NumberInputUI.h"
#include "NumberInputSndDef.h"

#include <App/NumberInput/include/NumberInputAppParam.h>
#include <AppLib/include/Util/app_util_Common.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <GameSys/include/GameProcManager.h>
#include <GameSys/include/GameManager.h>
#include <system/include/HeapDefine.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/number_input.gaix>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(NumberInput)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
enum {
  MAIN_SEQ_INIT,          //!< 初期化
  MAIN_SEQ_RELEASE,       //!< 解放
  MAIN_SEQ_FADE,          //!< フェード待ち
  MAIN_SEQ_START_WAIT,    //!< 開始待ち
  MAIN_SEQ_MAIN,          //!< メイン処理
  MAIN_SEQ_END,           //!< 終了定義
};

typedef u32 (NumberInputFrame::*pSeqFunc)(void);    //!< メインシーケンス型

//! メインシーケンス関数テーブル
static const pSeqFunc MainSeq[] = {
  &NumberInputFrame::MainSeq_Init,
  &NumberInputFrame::MainSeq_Release,
  &NumberInputFrame::MainSeq_Fade,
  &NumberInputFrame::MainSeq_StartWait,
  &NumberInputFrame::MainSeq_Main,
};

static const u32 NUMBER_INPUT_APP_HEAP_SIZE     = 0x100000;   //!< 1M:アプリ用ローカルヒープサイズ
static const u32 NUMBER_INPUT_DEVICE_HEAP_SIZE  = 0x100000;   //!< 1M:アプリ用ローカルデバイスヒープサイズ


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
NumberInputFrame::NumberInputFrame( void )
  : m_SubWork( NULL )
  , m_pArcReadBuff( NULL )
  , m_HeapSeq( 0 )
  , m_LytLoadSeq( 0 )
{
  m_MainWork.mainSeq = MAIN_SEQ_INIT;
}


//------------------------------------------------------------------------------
/**
 * @brief   Proc初期化処理
 *
 * @param   proc_manager    gfl2::proc::Manager
 *
 * @return  gfl2::proc::Result
 */
//------------------------------------------------------------------------------
applib::frame::Result NumberInputFrame::InitFunc( void )
{
  // ヒープの生成
  if( !CreateHeap() ) return applib::frame::RES_CONTINUE;
  // レイアウトデータの読み込み
  if( !LoadLayoutData() ) return applib::frame::RES_CONTINUE;

  return applib::frame::RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief   Proc終了処理
 *
 * @param   proc_manager    gfl2::proc::Manager
 *
 * @return  gfl2::proc::Result
 */
//------------------------------------------------------------------------------
applib::frame::Result NumberInputFrame::EndFunc( void )
{
  GflHeapSafeFreeMemory( m_pArcReadBuff );

  m_Heap.Delete();
  
  return applib::frame::RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief   Procアップデート処理
 *
 * @param   proc_manager    gfl2::proc::Manager
 *
 * @return  gfl2::proc::Result
 */
//------------------------------------------------------------------------------
applib::frame::Result NumberInputFrame::UpdateFunc( void )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  mwk->mainSeq = (this->*MainSeq[mwk->mainSeq])();

  if( mwk->mainSeq == MAIN_SEQ_END )
  {
    return applib::frame::RES_FINISH;
  }

  if( swk != NULL )
  {
    swk->graWork->Update();
  }

  return applib::frame::RES_CONTINUE;
}

//------------------------------------------------------------------------------
/**
  * @brief   Proc描画処理
  *
  * @param   no    ディスプレイNo
  *
  * @return  none
  */
//------------------------------------------------------------------------------
void NumberInputFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  SUB_WORK * swk = m_SubWork;

  if( swk )
  {
    swk->graWork->Draw( no );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   ゲームマネージャ関連取得
 *
 * @param   man   GameSys::GameManagerクラスポインタ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void NumberInputFrame::SetGameManager( GameSys::GameManager* man )
{
  MAIN_WORK * mwk = &m_MainWork;

  mwk->gameManager = man;
  mwk->devManager  = man->GetUiDeviceManager();
  mwk->fadeManager = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
}

//------------------------------------------------------------------------------
/**
 * @brief   セットアップ
 *
 * @param   prm   外部設定パラメータ
 * @param   man   レンダリングマネージャ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void NumberInputFrame::Setup( APP_PARAM* prm, app::util::AppRenderingManager* man )
{
  MAIN_WORK * mwk = &m_MainWork;

  mwk->prm = prm;
  mwk->renderManager = man;
}


//------------------------------------------------------------------------------
/**
 * @brief   ヒープの生成
 *
 * @param   none
 *
 * @return  "true  = 終了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool NumberInputFrame::CreateHeap( void )
{
  enum {
    HEAP_SEQ_CREATE,
    HEAP_SEQ_END,
  };

  switch( m_HeapSeq )
  {
  case HEAP_SEQ_CREATE:
    {
      gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

      m_Heap.Create( heap, heap, HEAPID_STRINPUT_APP, NUMBER_INPUT_APP_HEAP_SIZE, HEAPID_STRINPUT_DEVICE, NUMBER_INPUT_DEVICE_HEAP_SIZE );

      m_HeapSeq++;
    } break;

  case HEAP_SEQ_END:
    {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   レイアウトデータの読み込み
 *
 * @param   none
 *
 * @return  "true  = 終了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool NumberInputFrame::LoadLayoutData( void )
{
  enum {
    LYT_SEQ_OPEN,
    LYT_SEQ_LOAD,
    LYT_SEQ_WAIT,
    LYT_SEQ_CLOSE,
    LYT_SEQ_END,
  };

  gfl2::heap::HeapBase* devHeap = m_Heap.GetDeviceHeap();
  u32 arcID = ARCID_NUMBER_INPUT;
  u32 dataID = GARC_number_input_number_input_applyt_COMP;

  switch( m_LytLoadSeq )
  {
  case LYT_SEQ_OPEN:
    {
      app::util::FileAccessor::FileOpen( arcID, devHeap );
      m_LytLoadSeq = LYT_SEQ_LOAD;
    } // no break;

  case LYT_SEQ_LOAD:
    {
      if( !app::util::FileAccessor::IsFileOpen( arcID ) ) break;

      app::util::FileAccessor::FileLoad(
        arcID,
        dataID,
        &m_pArcReadBuff,
        devHeap,
        true,
        nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );

      m_LytLoadSeq = LYT_SEQ_WAIT;
    } // no break;

  case LYT_SEQ_WAIT:
    {
      if( !app::util::FileAccessor::IsFileLoad( &m_pArcReadBuff ) ) break;

      app::util::FileAccessor::FileClose( arcID, devHeap );
      m_LytLoadSeq = LYT_SEQ_CLOSE;
    } // no break;

  case LYT_SEQ_CLOSE:
    {
      if( !app::util::FileAccessor::IsFileClose( arcID ) ) break;

      m_LytLoadSeq = LYT_SEQ_END;
    } // no break;

  case LYT_SEQ_END:
    {
      return true;
    } // no break;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   サブワークの作成
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void NumberInputFrame::CreateSubWork( void )
{
  FUKUSHIMA_PRINT( "%s\n", __FUNCTION__ );

  gfl2::heap::HeapBase* sysHeap = m_Heap.GetSystemHeap();

  // サブワークの生成
  m_SubWork = GFL_NEW(sysHeap) SUB_WORK();
  
  // グラフィックワークの生成
  m_SubWork->graWork = GFL_NEW(sysHeap) NumberInputDraw( &m_Heap, m_MainWork.gameManager->GetAsyncFileManager(), m_MainWork.renderManager );
  m_SubWork->graWork->Initialize( m_pArcReadBuff );

  gfl2::lyt::LytWk* lytwk = m_SubWork->graWork->GetLayoutWork( NumberInputDraw::LYTID_LOWER );
  gfl2::lyt::LytMultiResID resID = m_SubWork->graWork->GetLayoutResourceID();

  // UIの生成
  if( m_MainWork.prm->callMode == CALL_MODE_NUMBER )
  {
    FUKUSHIMA_PRINT( "CALL_MODE_NUMBER\n" );
    m_SubWork->ui = GFL_NEW(sysHeap) UI( m_MainWork.devManager, &m_Heap, false, lytwk, resID );
  }
  else {
    FUKUSHIMA_PRINT( "CALL_MODE_ALPHABET\n" );
    m_SubWork->ui = GFL_NEW(sysHeap) UI( m_MainWork.devManager, &m_Heap, true, lytwk, resID );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   サブワークの削除
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
bool NumberInputFrame::DeleteSubWork( void )
{
  if( m_SubWork->graWork->Finalize() == false )
  {
    return false;
  }

  GFL_DELETE m_SubWork->graWork;

  GFL_DELETE m_SubWork->ui;

  GFL_DELETE m_SubWork;

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   フェードインのセット
 *
 * @param   next    フェード後のシーケンス
 *
 * @return  MAINSEQ_FADE
 */
//------------------------------------------------------------------------------
u32 NumberInputFrame::SetFadeIn( u32 next )
{
  MAIN_WORK * mwk = &m_MainWork;

  mwk->fadeManager->RequestIn(
    gfl2::Fade::DISP_DOUBLE,
    gfl2::Fade::FADE_TYPE_ALPHA,
    gfl2::Fade::FADE_DEFAULT_SYNC );

  mwk->fadeNextSeq = next;

  return MAIN_SEQ_FADE;
}

//------------------------------------------------------------------------------
/**
 * @brief   フェードアウトのセット
 *
 * @param   next    フェード後のシーケンス
 *
 * @return  MAINSEQ_FADE
 */
//------------------------------------------------------------------------------
u32 NumberInputFrame::SetFadeOut( u32 next )
{
  MAIN_WORK * mwk = &m_MainWork;

  gfl2::math::Vector4 color( 0, 0, 0, 255 );

  mwk->fadeManager->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );

  mwk->fadeNextSeq = next;

  return MAIN_SEQ_FADE;
}

//------------------------------------------------------------------------------
/**
 * @brief   キーボード入力
 *
 * @param   id    入力データID
 *
 * @return  次のシーケンス
 */
//------------------------------------------------------------------------------
u32 NumberInputFrame::DecideKeyBoard( u32 id )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  if( id == UI::RET_DELETE )
  {
    if( mwk->inputPos == 0 )
    {
      Sound::PlaySE( NUMINPUT_SE_ERROR );
    }
    else {
      Sound::PlaySE( NUMINPUT_SE_DELETE );

      mwk->inputPos--;
      mwk->inputData[mwk->inputPos] = INPUT_DEFAULT_CODE;

      swk->graWork->PutInputCode( mwk->inputPos, mwk->inputData[mwk->inputPos] );
      swk->graWork->PutInputCursor( mwk->inputPos );
      swk->graWork->SetVisibleInputCursor( true );
    }
  }
  else if( id == UI::RET_END )
  {
    Sound::PlaySE( NUMINPUT_SE_COMPLETE );

    if( mwk->inputPos != mwk->inputMax )
    {
      for( u32 i=mwk->inputPos; i<mwk->inputMax; ++i )
      {
        //mwk->inputData[i] = 9;  // [ 0 ]で埋める
        //swk->graWork->PutInputCode( i, mwk->inputData[i] );
        mwk->inputData[i] = INPUT_DEFAULT_CODE; // -1で埋める
      }
      swk->graWork->SetVisibleInputCursor( false );
    }
    SetEndParam();
    mwk->nextSeq = MAIN_SEQ_END;

    return SetFadeOut( MAIN_SEQ_RELEASE );
  }
  else {
    if( mwk->inputPos != mwk->inputMax )
    {
      Sound::PlaySE( NUMINPUT_SE_DECIDE );

      mwk->inputData[mwk->inputPos] = id;
      swk->graWork->PutInputCode( mwk->inputPos, mwk->inputData[mwk->inputPos] );
      mwk->inputPos++;
      swk->graWork->PutInputCursor( mwk->inputPos );

      if( mwk->inputPos == mwk->inputMax )
      {
        swk->graWork->SetVisibleInputCursor( false );
        swk->ui->SetCursorPosID( UI::RET_END );
        swk->graWork->PutCursor( swk->ui->GetButtonParts(), swk->ui->GetButtonPane() );
      }
    }
    else {
      Sound::PlaySE( NUMINPUT_SE_ERROR );
    }
  }

  return MAIN_SEQ_MAIN;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了パラメータ設定
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void NumberInputFrame::SetEndParam( void )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  // バッファ初期化
  for( u32 i=0; i<PART_MAX; ++i )
  {
    for( u32 j=0; j<ALPHABET_INPUT_MAX+1; ++j )
    {
      mwk->prm->inputStr[i][j] = gfl2::str::EOM_CODE;
    }
  }

  if( mwk->prm->callMode == CALL_MODE_NUMBER )
  {
    /*
      @note
        [0]埋め排除の要望対応で[-1]が代入されているため、
        GetInputStrCode関数では予期しない値になるのでアサートを挿入。
    */
    GFL_ASSERT( 0 );

    mwk->prm->inputValue = 0;

    for( u32 i=0; i<INPUT_NUM_MAX; ++i )
    {
      mwk->prm->inputValue = mwk->prm->inputValue * 10 + ((mwk->inputData[i] + 1) % 10);
    }

    u32 i=0;
    for( i=0; i<2; ++i )
    {
      mwk->prm->inputStr[0][i] = swk->graWork->GetInputStrCode( mwk->inputData[i] );
    }

    // ダミー処理
    for( u32 j=0; j<ALPHABET_INPUT_MAX; ++j )
    {
      mwk->prm->inputStr[0][j] = swk->graWork->GetInputStrCode( mwk->inputData[0] );
    }

    for( u32 j=1; j<PART_MAX; ++j )
    {
      for( u32 k=0; k<ALPHABET_INPUT_MAX; ++k )
      {
        mwk->prm->inputStr[j][k] = swk->graWork->GetInputStrCode( mwk->inputData[i] );
      }
    }
  }
  else {
    for( u32 i=0; i<PART_MAX; ++i )
    {
      for( u32 j=0; j<ALPHABET_INPUT_MAX; ++j )
      {
        s32 code = mwk->inputData[i * ALPHABET_INPUT_MAX + j];

        if( code != INPUT_DEFAULT_CODE )
        {
          mwk->prm->inputStr[i][j] = swk->graWork->GetInputStrCode( code );
        }
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   入力データ初期化
 *
 * @param   none
 *
 * @return  "true  = 外部から指定されたデータで初期化"
 * @return  "false = 指定されたデータがないので何もしなかった"
 */
//------------------------------------------------------------------------------
bool NumberInputFrame::InitInputData( void )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  if( mwk->prm->inputStr[0][0] == gfl2::str::EOM_CODE )
  {
    FUKUSHIMA_PRINT( "mwk->prm->inputStr[0][0] == gfl2::str::EOM_CODE\n" );
    return false;
  }

  if( mwk->prm->callMode == CALL_MODE_NUMBER )
  {
    u32 i;

    for( i=0; i<2; ++i )
    {
      mwk->inputData[i] = swk->graWork->GetStrCodePos( mwk->prm->inputStr[0][i] );
      swk->graWork->PutInputCode( i, mwk->inputData[i] );
    }

    for( u32 j=1; j<PART_MAX; ++j )
    {
      for( u32 k=0; k<ALPHABET_INPUT_MAX; ++k )
      {
        mwk->inputData[i] = swk->graWork->GetStrCodePos( mwk->prm->inputStr[j][k] );
        swk->graWork->PutInputCode( i, mwk->inputData[i] );
        ++i;
      }
    }
  }
  else {
    mwk->inputPos = 0;
    for( u32 i=0; i<PART_MAX; ++i )
    {
      for( u32 j=0; j<ALPHABET_INPUT_MAX; ++j )
      {
        if( mwk->prm->inputStr[i][j] != gfl2::str::EOM_CODE )
        {
          mwk->inputData[i * ALPHABET_INPUT_MAX + j] = swk->graWork->GetStrCodePos( mwk->prm->inputStr[i][j] );
          //  文字入力位置更新
          ++mwk->inputPos;
        }
        else
        {
          //  終端
          mwk->inputData[i * ALPHABET_INPUT_MAX + j ] = INPUT_DEFAULT_CODE;
        }

        swk->graWork->PutInputCode( i * ALPHABET_INPUT_MAX + j, mwk->inputData[i * ALPHABET_INPUT_MAX + j] );
      }
    }
  }

  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   メインシーケンス：初期化
 *
 * @param   none
 *
 * @return  次のシーケンス
 *
 * 定義：MAINSEQ_INIT
 */
//------------------------------------------------------------------------------
u32 NumberInputFrame::MainSeq_Init( void )
{
  if( !m_SubWork )
  {
    CreateSubWork();
  }

  if( m_SubWork->graWork->CheckFileRead() == true )
  {
    return MAIN_SEQ_INIT;
  }

  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

#if 0 //!< 仕様上16文字入力モードしかない
  if( mwk->prm->callMode == CALL_MODE_NUMBER )
  {
    mwk->inputMax = INPUT_NUM_MAX;
    swk->graWork->InitMode( false );
  }
  else {
    mwk->inputMax = INPUT_MAX;
    swk->graWork->InitMode( true );
  }
#else
  mwk->inputMax = INPUT_MAX;
  swk->graWork->InitMode( true );
#endif

  if( InitInputData() )
  {
  //  mwk->inputPos = mwk->inputMax;

  }
  else {
    mwk->inputPos = 0;
  }

  swk->graWork->SetVisibleInputCursor( false );
  swk->graWork->PutUpperString( mwk->prm->callType );

  return SetFadeIn( MAIN_SEQ_START_WAIT );
}

//------------------------------------------------------------------------------
/**
 * @brief   メインシーケンス：解放
 *
 * @param   none
 *
 * @return  次のシーケンス
 *
 * 定義：MAINSEQ_RELEASE
 */
//------------------------------------------------------------------------------
u32 NumberInputFrame::MainSeq_Release( void )
{
  if( DeleteSubWork() )
  {
    return m_MainWork.nextSeq;
  }

  return MAIN_SEQ_RELEASE;
}

//------------------------------------------------------------------------------
/**
 * @brief   メインシーケンス：フェード待ち
 *
 * @param   none
 *
 * @return  次のシーケンス
 *
 * 定義：MAINSEQ_FADE
 */
//------------------------------------------------------------------------------
u32 NumberInputFrame::MainSeq_Fade( void )
{
  MAIN_WORK * mwk = &m_MainWork;

  if( mwk->fadeManager->IsEnd( gfl2::Fade::DISP_DOUBLE ) )
  {
    return mwk->fadeNextSeq;
  }

  return MAIN_SEQ_FADE;
}

//------------------------------------------------------------------------------
/**
 * @brief   メインシーケンス：開始待ち
 *
 * @param   none
 *
 * @return  次のシーケンス
 *
 * 定義：MAINSEQ_START_WAIT
 */
//------------------------------------------------------------------------------
u32 NumberInputFrame::MainSeq_StartWait( void )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  if( !swk->graWork->CheckModeAnime() )
  {
    swk->graWork->PutInputCursor( mwk->inputPos );

    if( mwk->inputPos == mwk->inputMax )
    {
      //  終端であれば出さない
      swk->graWork->SetVisibleInputCursor( false );
    }
    else
    {
      swk->graWork->SetVisibleInputCursor( true );
    }

    return MAIN_SEQ_MAIN;
  }

  return MAIN_SEQ_START_WAIT;
}

//------------------------------------------------------------------------------
/**
 * @brief   メインシーケンス：メイン処理
 *
 * @param   none
 *
 * @return  次のシーケンス
 *
 * 定義：MAINSEQ_MAIN
 */
//------------------------------------------------------------------------------
u32 NumberInputFrame::MainSeq_Main( void )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  u32 ret = swk->ui->Update();

  switch( ret )
  {
  case UI::RET_CURSOR_MOVE:
    {
      Sound::PlaySE( NUMINPUT_SE_CURSOR_MOVE );
      swk->graWork->PutCursor( swk->ui->GetButtonParts(), swk->ui->GetButtonPane() );
    } break;

  case UI::RET_TOUCH:
    {
      u32 id = swk->ui->GetHoldButtonID();

      if( id != swk->ui->GetCursorPosID() )
      {
        swk->ui->SetCursorPosID( id );
        swk->graWork->PutCursor( swk->ui->GetButtonParts(), swk->ui->GetButtonPane() );
      }
    } break;

  case UI::RET_DECIDE_KEY:
    {
      u32 id = swk->ui->GetCursorPosID();
      swk->graWork->StartKeyBoardSelectAnime( id );
      return DecideKeyBoard( id );
    } // no break;

  case UI::RET_DELETE_KEY:
    {
      swk->graWork->StartKeyBoardSelectAnime( UI::RET_DELETE );
      return DecideKeyBoard( UI::RET_DELETE );
    } // no break;

  case UI::RET_NONE:
    {

    } break;

  case UI::RET_DELETE:
  case UI::RET_END:
  default:
    {
      return DecideKeyBoard( ret );
    }
  }

  return MAIN_SEQ_MAIN;
}

GFL_NAMESPACE_END(NumberInput)
GFL_NAMESPACE_END(App)

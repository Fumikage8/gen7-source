//======================================================================
/**
 * @file    ResortTalkMessage.cpp
 * @date    2016/02/10 20:12:19
 * @author  fukushima_yuya
 * @brief   ポケリゾート：管理人の会話メッセージ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortTalkMessage.h"
#include "ResortCharaModel.h"

#include <Sound/include/Sound.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   param   生成パラメータ
 */
//------------------------------------------------------------------------------
TalkMessage::TalkMessage( CreateParam& param )
 : m_pHeap(param.heap)
 , m_pAppRenderingManager(param.renderMan)
 , m_pTalkWindow(NULL)
 , m_pSystemWindow(NULL)
 , m_pActiveWindow(NULL)
 , m_pCharaModelListener(NULL)
 , m_WindowType(App::Tool::WIN_TYPE_TALK_DOWN)
 , m_MsgEndFlag(true)
 , m_PlaySE(false)
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void TalkMessage::Initialize( InitParam& param )
{
  gfl2::heap::HeapBase* heap = m_pHeap->GetDeviceHeap();
  {
    // 会話用ウィンドウ
    m_pTalkWindow = GFL_NEW(heap) App::Tool::TalkWindow( m_pHeap, m_pAppRenderingManager );
    {
      GFL_ASSERT( m_pTalkWindow );
      
      m_pTalkWindow->SetWindowType( App::Tool::WIN_TYPE_TALK_DOWN );
      m_pTalkWindow->Setup( param.lytBuff, TALK_WINDOW_CHARA_MAX );
      m_pTalkWindow->InitMsgData( param.msgWnd );
      m_pTalkWindow->InitTrainerMsgData();
      m_pTalkWindow->SetScriptWordSet( param.wordSet );

      m_pTalkWindow->GetG2DUtil()->SetMessageCallback( this );
    }
    // システムメッセージ用ウィンドウ
    m_pSystemWindow = GFL_NEW(heap) App::Tool::TalkWindow( m_pHeap, m_pAppRenderingManager );
    {
      GFL_ASSERT( m_pSystemWindow );

      m_pSystemWindow->SetWindowType( App::Tool::WIN_TYPE_SYS_DOWN );
      m_pSystemWindow->Setup( param.lytBuff, TALK_WINDOW_CHARA_MAX );
      m_pSystemWindow->InitMsgData( param.msgWnd );
      m_pSystemWindow->InitTrainerMsgData();
      m_pSystemWindow->SetScriptWordSet( param.wordSet );

      m_pSystemWindow->GetG2DUtil()->SetMessageCallback( this );
    }

    m_pActiveWindow = m_pTalkWindow;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   破棄
 */
//------------------------------------------------------------------------------
bool TalkMessage::Terminate( void )
{
  m_pActiveWindow = NULL;

  if( m_pSystemWindow )
  {
    m_pSystemWindow->End();

    GFL_SAFE_DELETE( m_pSystemWindow );
  }
  if( m_pTalkWindow )
  {
    m_pTalkWindow->End();

    GFL_SAFE_DELETE( m_pTalkWindow );
  }
  
  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void TalkMessage::Update( void )
{
  if( m_pTalkWindow )
  {
    m_pTalkWindow->Update();
  }
  if( m_pSystemWindow )
  {
    m_pSystemWindow->Update();
  }

  gfl2::str::StrWin::Result result = m_pActiveWindow->GetStrWinResult();
  {
    if( !m_MsgEndFlag )
    {
      if( result == gfl2::str::StrWin::RES_DONE )
      {
        // メッセージの終了
        MessageEnd();

        m_MsgEndFlag = true;
      }
    }

    if( !m_PlaySE )
    {
      if( result == gfl2::str::StrWin::RES_FINISH )
      {
        Sound::PlaySE( SEQ_SE_MESSAGE );

        m_PlaySE = true;
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void TalkMessage::Draw( gfl2::gfx::CtrDisplayNo no )
{
  if( m_pActiveWindow )
  {
    m_pActiveWindow->Draw( no, 2, 1 );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   会話の開始
 *
 * @param   msgID       メッセージID
 * @param   finType     終了タイプ
 * @param   wndType     ウィンドウタイプ
 */
//------------------------------------------------------------------------------
void TalkMessage::TalkStart( u32 msgID, gfl2::str::StrWin::FinishType finType, App::Tool::TalkWindowType wndType )
{
  if( finType == gfl2::str::StrWin::FINISH_NONE )
  {
    m_Result = gfl2::str::StrWin::RES_DONE;
  }
  else if( finType == gfl2::str::StrWin::FINISH_USER )
  {
    m_Result = gfl2::str::StrWin::RES_FINISH;
  }

  // ウィンドウタイプの変更
  if( m_WindowType != wndType )
  {
    SetWindowType( wndType );
  }

  if( m_WindowType == App::Tool::WIN_TYPE_TALK_DOWN )
  {
    // 会話開始アニメの再生リクエスト
    m_pCharaModelListener->RequestStartCharaAnim();
    // 口パクの再生リクエスト
    m_pCharaModelListener->RequestStartMouthAnim();
  }

  // カーソルを非表示
  m_pTalkWindow->SetMsgCursorUserVisible( false, false );

  if( m_pActiveWindow )
  {
    // メッセージの設定
    m_pActiveWindow->SetMessage( msgID );
    
    // 終了タイプの設定
    m_pActiveWindow->SetMessageFinishMode( finType, 0 );
    
    // ウィンドウの表示
    m_pActiveWindow->CallMsgWin();
  }

  m_MsgEndFlag = false;
  m_PlaySE     = false;
  
  FUKUSHIMA_PRINT( "TalkStart : msgID[%d]\n", msgID );
}

//------------------------------------------------------------------------------
/**
 * @brief   会話の終了
 */
//------------------------------------------------------------------------------
void TalkMessage::TalkEnd( bool anim )
{
  if( m_pActiveWindow )
  {
    // ウィンドウを閉じる
    m_pActiveWindow->CloseMsgWin();
  }
  // 終了モーション再生リクエスト
  if( anim )
  {
    m_pCharaModelListener->RequestEndCharaAnim();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   会話が終了したか？
 *
 * @return  "true  == 終了"
 * @return  "false == それ以外"
 */
//------------------------------------------------------------------------------
bool TalkMessage::IsTalkEnd( void )
{
  if( m_pActiveWindow )
  {
    gfl2::str::StrWin::Result result = m_pActiveWindow->GetStrWinResult();

    // 会話の終了
    if( result == m_Result )
    {
      return true;
    }

    return false;
  }
  
  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   ウィンドウタイプの変更
 *
 * @param   wndType   ウィンドウタイプ
 */
//------------------------------------------------------------------------------
void TalkMessage::SetWindowType( App::Tool::TalkWindowType wndType )
{
  if( wndType == App::Tool::WIN_TYPE_TALK_DOWN )
  {
    m_pActiveWindow = m_pTalkWindow;
  }
  else if( wndType == App::Tool::WIN_TYPE_SYS_DOWN )
  {
    m_pActiveWindow = m_pSystemWindow;
  }

  m_WindowType = wndType;
}



//------------------------------------------------------------------------------
/**
 * @brief   メッセージの表示終了
 */
//------------------------------------------------------------------------------
void TalkMessage::MessageEnd( void )
{
  if( m_WindowType == App::Tool::WIN_TYPE_SYS_DOWN ) return;

  // 終端カーソルの表示
  if( m_Result == gfl2::str::StrWin::RES_FINISH )
  {
    if( m_pTalkWindow )   // @fix cov_ctr[10176]
    {
      m_pTalkWindow->SetMsgCursorUserVisible( true, true );
    }
  }
  // 口パクの停止リクエスト
  m_pCharaModelListener->RequestCloseMouth();
}


bool TalkMessage::UpdateMsgCallback( u16 arg )
{
  switch( arg )
  {
  case 0:
    if( Sound::IsSEFinished( SEQ_SE_DECIDE1 ) != 0 )
    {
      Sound::StartME( STRM_ME_ITEM );
      return false;
    }
    return true;

  case 1:   // MEウェイト
    if( Sound::IsMEFinished( STRM_ME_ITEM ) != 0 )
    {
      Sound::StartBaseBGMFromME();
      return false;
    }
    return true;

  case 2:
    if( Sound::IsSEFinished( SEQ_SE_MESSAGE ) != 0 )
    {
      Sound::StartME( STRM_ME_NJ02 );
      return false;
    }
    return true;

  case 3:
    if( Sound::IsMEFinished( STRM_ME_NJ02 ) )
    {
      Sound::StartBaseBGMFromME();
      return false;
    }
    return true;

  case 4:
    if( Sound::IsSEFinished( SEQ_SE_MESSAGE ) != 0 )
    {
      Sound::PlaySE( SEQ_SE_SYS_NJ_048 );   // @fix GFNMCat[3418]
      return false;
    }
    return true;

  case 5:
    if( Sound::IsSEFinished( SEQ_SE_SYS_NJ_048 ) != 0 )   // @fix GFNMCat[3418]
    {
      return false;
    }
    return true;

  case 6: // @fix NMCat[2642]
    if( Sound::IsMEFinished( STRM_ME_RESORT_FINISH ) != 0 )
    {
      Sound::StartBaseBGMFromME();
      return false;
    }
    return true;
  }

  return false;
}


GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)


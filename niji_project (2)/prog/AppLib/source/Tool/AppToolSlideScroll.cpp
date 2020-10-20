//=============================================================================
/**
 * @file    AppToolSlideScroll.cpp
 * @brief   スライド操作でのスクロール判定処理
 * @author  Hiroyuki Nakamura
 * @date    2015.06.04
 */
//=============================================================================

// module
#include "AppLib/include/Tool/AppToolSlideScroll.h"
// gflib2
#include <Layout/include/gfl2_GraphicsType.h>
#include <debug/include/gfl2_DebugPrint.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   mode  スライド操作モード
 */
//-----------------------------------------------------------------------------
SlideScroll::SlideScroll( SlideMode mode )
 : m_result( RESULT_NONE )
 , m_mode( mode )
 , m_initDragX( 0 )
 , m_initDragY( 0 )
 , m_lastDragX( 0 )
 , m_lastDragY( 0 )
 , m_oldDragX( 0 )
 , m_oldDragY( 0 )
 , m_seq( SEQ_WAIT )
 , m_touchUY( 0 )
 , m_touchDY( DISPLAY_LOWER_HEIGHT )
 , m_touchLX( 0 )
 , m_touchRX( DISPLAY_LOWER_WIDTH )
 , m_exTouchFlag( false )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief   デズトラクタ
 */
//-----------------------------------------------------------------------------
SlideScroll::~SlideScroll()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    SetTouchAreaDisp
 * @brief   タッチ有効範囲を設定（左上(0,0)座標）
 * @date    2015.06.05
 *
 * @param   up      上Y座標
 * @param   down    下Y座標
 * @param   left    左X座標
 * @param   right   右X座標
 */
//-----------------------------------------------------------------------------
void SlideScroll::SetTouchAreaDisp( u16 up, u16 down, u16 left, u16 right )
{
  m_touchUY = up;
  m_touchDY = down;
  m_touchLX = left;
  m_touchRX = right;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetTouchArea
 * @brief   タッチ有効範囲を設定（中心(0,0)座標）
 * @date    2015.06.05
 *
 * @param   up      上Y座標
 * @param   down    下Y座標
 * @param   left    左X座標
 * @param   right   右X座標
 */
//-----------------------------------------------------------------------------
void SlideScroll::SetTouchArea( f32 up, f32 down, f32 left, f32 right )
{
  m_touchUY = static_cast<u16>( DISPLAY_LOWER_HEIGHT / 2 - static_cast<s32>(up) );
  m_touchDY = static_cast<u16>( DISPLAY_LOWER_HEIGHT / 2 - static_cast<s32>(down) );
  m_touchLX = static_cast<u16>( static_cast<s32>(left)  + DISPLAY_LOWER_WIDTH / 2 );
  m_touchRX = static_cast<u16>( static_cast<s32>(right) + DISPLAY_LOWER_WIDTH / 2 );
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateSlide
 * @brief   更新処理
 * @date    2015.06.04
 *
 * @param   tp        タッチパネル
 * @param   is_touch  true = タッチされている
 *
 * @return  動作結果
 */
//-----------------------------------------------------------------------------
SlideScroll::Result SlideScroll::UpdateSlide( gfl2::ui::TouchPanel * tp, bool is_touch )
{
  m_result = RESULT_NONE;

  // 呼び出し元が途中で他の処理に遷移した場合の対処
  if( is_touch != false && tp->IsTouchTrigger() != false )
  {
    if( m_seq != SEQ_WAIT )
    {
      InitUpdateWork();
    }
  }

  switch( m_seq )
  {
  case SEQ_WAIT:
    if( is_touch != false && ( tp->IsTouchTrigger() != false || m_exTouchFlag != false ) )
    {
      u16 x, y;
      tp->GetXY( &x, &y );
      if( IsTouchArea( x, y ) != false )
      {
        m_initDragX = x;
        m_initDragY = y;
        m_lastDragX = x;
        m_lastDragY = y;
        m_oldDragX  = x;
        m_oldDragY  = y;
        m_seq = SEQ_TOUCH;
      }
      m_exTouchFlag = false;
    }
    break;

  case SEQ_TOUCH:
    {
      u16 x, y;
      tp->GetXY( &x, &y );
      if( IsTouchArea( x, y ) == false || is_touch == false )
      {
        m_seq = SEQ_WAIT;
      }
      else
      {
        if( IsStartDrag( x, y ) != false )
        {
          m_lastDragX = x;
          m_lastDragY = y;
          m_oldDragX  = x;
          m_oldDragY  = y;
          m_seq = SEQ_DRAG;
          m_result = RESULT_DRAG_START;
        }
      }
    }
    break;

  case SEQ_DRAG:
    {
      u16 x, y;
      tp->GetXY( &x, &y );
      // @note 終了は範囲チェックなしのほうが違和感がない
//      if( IsTouchArea( x, y ) != false && is_touch != false )
      if( is_touch != false )
      {
        m_oldDragX = m_lastDragX;
        m_oldDragY = m_lastDragY;
        m_lastDragX = x;
        m_lastDragY = y;
        m_result = RESULT_DRAG;
      }
      else
      {
        m_seq = SEQ_WAIT;
        m_result = CheckSlideScroll();
      }
    }
    break;
  }

  return m_result;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetResult
 * @brief   動作結果を取得
 * @date    2015.06.04
 *
 * @return  動作結果
 */
//-----------------------------------------------------------------------------
SlideScroll::Result SlideScroll::GetResult(void) const
{
  return m_result;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsTouchArea
 * @brief   指定座標がタッチ有効範囲内か
 * @date    2015.06.05
 *
 * @retval  true  = 範囲内
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool SlideScroll::IsTouchArea( u16 x, u16 y ) const
{
  if( x >= m_touchLX && x <= m_touchRX && y >= m_touchUY && y <= m_touchDY )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetOffset
 * @brief   座標の差分を取得
 * @date    2015.06.04
 *
 * @param   x1  X座標１
 * @param   x2  X座標２
 * @param   y1  Y座標１
 * @param   y2  Y座標２
 *
 * @return  モード別の座標の差分
 */
//-----------------------------------------------------------------------------
int SlideScroll::GetOffset( int x1, int x2, int y1, int y2 ) const
{
  if( m_mode == MODE_H )
  {
    return ( x1 - x2 );
  }
  else
  {
    return ( y1 - y2 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsStartDrag
 * @brief   ドラッグを開始するか
 * @date    2015.06.04
 *
 * @param   x   タッチX座標
 * @param   y   タッチY座標
 *
 * @retval  true  = 開始
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool SlideScroll::IsStartDrag( u16 x, u16 y ) const
{
  if( gfl2::math::Abs( GetOffset(x,m_initDragX,y,m_initDragY) ) >= DRAG_START_ABS )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckSlideScroll
 * @brief   スライドするかをチェック
 * @date    2015.06.04
 *
 * @retval  RESULT_SLIDE_PLUS  = ＋方向にスライド
 * @retval  RESULT_SLIDE_MINUS = −方向にスライド
 * @retval  RESULT_RELEASE     = スライドしない
 */
//-----------------------------------------------------------------------------
SlideScroll::Result SlideScroll::CheckSlideScroll(void) const
{
  int offs = GetOffset( m_oldDragX, m_lastDragX, m_oldDragY, m_lastDragY );

  if( gfl2::math::Abs(offs) >= SLIDE_START_ABS )
  {
    if( offs < 0 )
    {
      return RESULT_SLIDE_MINUS;
    }
    else
    {
      return RESULT_SLIDE_PLUS;
    }
  }

  return RESULT_RELEASE;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetDragOffset
 * @brief   ドラッグ中の座標の差分を取得
 * @date    2015.06.04
 *
 * @return  開始からの座標の差分
 */
//-----------------------------------------------------------------------------
int SlideScroll::GetDragOffset(void) const
{
  return GetOffset( m_lastDragX, m_initDragX, m_lastDragY, m_initDragY );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetSlideLevel
 * @brief   スライド時の速度レベルを取得
 * @date    2015.07.27
 *
 * @return  速度レベル
 *
 * @li  PaneListで使用（計算は旧PaneListと踏襲）
 */
//-----------------------------------------------------------------------------
int SlideScroll::GetSlideLevel(void) const
{
  int offs = GetOffset( m_oldDragX, m_lastDragX, m_oldDragY, m_lastDragY );
  return ( ( offs - SLIDE_START_ABS ) / 8 );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetExTouchFlag
 * @brief   トリガー情報を無視してドラッグを開始するフラグをセット
 * @date    2015.07.28
 */
//-----------------------------------------------------------------------------
void SlideScroll::SetExTouchFlag(void)
{
  m_exTouchFlag = true;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitWork
 * @brief   Update用のワークを初期化
 * @date    2015.06.17
 *
 * @li  UpdateSlide()の呼び出し元で別処理に遷移した場合に使用してください
 */
//-----------------------------------------------------------------------------
void SlideScroll::InitUpdateWork(void)
{
  m_seq = SEQ_WAIT;
  m_initDragX = 0;
  m_initDragY = 0;
  m_lastDragX = 0;
  m_lastDragY = 0;
  m_oldDragX  = 0;
  m_oldDragY  = 0;
}


GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

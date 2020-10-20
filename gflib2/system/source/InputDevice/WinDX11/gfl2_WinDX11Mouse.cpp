#ifdef  GF_PLATFORM_WIN32

#include <string>
#include <windows.h>

#include <system/include/InputDevice/gfl2_InputTypes.h>
#include <system/include/InputDevice/WinDX11/gfl2_WinDX11Mouse.h>


namespace gfl2 { namespace system { namespace InputDevice { namespace WinDX11 {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  グローバル
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

WinDX11Mouse				*WinDX11Mouse::s_Gp = NULL;

static const s32 SC_MOUSEBIT[ MouseButton::NumberOf ] = 
{
  VK_LBUTTON,   // Left
  VK_MBUTTON,   // Middle
  VK_RBUTTON,   // Right
};

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  WinDX11Mouseクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
WinDX11Mouse::WinDX11Mouse()
{
	s_Gp = this;
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
WinDX11Mouse::~WinDX11Mouse()
{
	s_Gp = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  パッドバイスの初期化
 */
//-----------------------------------------------------------------------------
b32 WinDX11Mouse::Initialize_( void )
{
	return true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  パッドデバイスの解放
 */
//-----------------------------------------------------------------------------
void WinDX11Mouse::Terminate_( void )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief  パッドデバイスの更新
 */
//-----------------------------------------------------------------------------
void WinDX11Mouse::Update_( void )
{
  // 一旦クリア
  this->ClearData_();

  // 座標取得
  POINT point;
  if (GetCursorPos(&point) != 0)
  {
    HWND hWnd = WindowFromPoint(point);

    // ウィンドウサイズ取得
    RECT rect;
    if (GetClientRect(hWnd, &rect) != 0)
    {
      m_WindowHeight = rect.bottom;
      m_WindowWidth = rect.right;
    }

    // クライアント座標に変換
    ScreenToClient(hWnd, &point);

    m_MousePos.x = static_cast<f32>(point.x);
    m_MousePos.y = static_cast<f32>(point.y);
  }

  // クリック状況取得
  for (u32 i = 0; i < MouseButton::NumberOf; ++i)
  {
    SHORT ret = GetAsyncKeyState(SC_MOUSEBIT[i]);

    // ボタンが押されている
    if (ret != 0)
    {
      m_MousePress |= 1 << i;
    }

    if (ret & 0x0001)
    {
      m_MousePush |= 1 << i;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
b32 WinDX11Mouse::IsPress_( MouseButton button )
{
  return m_MousePress & (1 << button);
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
b32 WinDX11Mouse::IsPush_( MouseButton button )
{
  return m_MousePush & (1 << button);
}

//! @brief 左上原点の座標を取得
MousePos WinDX11Mouse::GetLeftUpOriginPos_( )
{
  return m_MousePos;
}

//! @brief 左下原点の座標を取得
MousePos WinDX11Mouse::GetLeftDownOriginPos_( )
{
  MousePos pos = this->GetLeftUpOriginPos_();
  pos.y = m_WindowHeight - pos.y;
  return pos;
}

//! @brief 左上原点の座標を0.0f〜1.0fの割合で取得
MousePos WinDX11Mouse::GetLeftUpOriginPosRatio_( )
{
  MousePos pos = this->GetLeftUpOriginPos_();
  pos.x /= m_WindowWidth;
  pos.y /= m_WindowHeight;
  return pos;
}

//! @brief 左下原点の座標を0.0f〜1.0fの割合で取得
MousePos WinDX11Mouse::GetLeftDownOriginPosRatio_( )
{
  MousePos pos = this->GetLeftDownOriginPos_();
  pos.x /= m_WindowWidth;
  pos.y /= m_WindowHeight;
  return pos;
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
void WinDX11Mouse::ClearData_()
{
  m_MousePos.x = 0.0f;
  m_MousePos.y = 0.0f;
  m_MousePress = 0;
  m_MousePush = 0;
}


}}}}

#endif
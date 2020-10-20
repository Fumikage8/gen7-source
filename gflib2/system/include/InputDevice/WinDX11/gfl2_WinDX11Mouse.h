#ifndef GFLIB2_SYSTEM_INPUTDEVICE_WINDX11_WINDX11MOUSE_H_INCLUDED
#define GFLIB2_SYSTEM_INPUTDEVICE_WINDX11_WINDX11MOUSE_H_INCLUDED

#include <stdio.h>
#include <Windows.h>
#include <types/include/gfl2_Typedef.h>

namespace gfl2 { namespace system { namespace InputDevice { namespace WinDX11 {

class WinDX11Mouse
{
public:

protected:

	WinDX11Mouse();
	inline virtual ~WinDX11Mouse();

	//! @brief パッドバイスの初期化
	b32 Initialize_( void );

	//! @brief パッドデバイスの解放
	void Terminate_( void );

	//! @brief パッドデバイスの更新
	void Update_( void );

	//! @brief ボタンが押されているか？
	b32 IsPress_( MouseButton button );

	//! @brief ボタンが押されたか？
	b32 IsPush_( MouseButton button );

	//! @brief	入力データを一時的に初期化する
	void ClearData_( );

	//! @brief 左上原点の座標を取得
	MousePos GetLeftUpOriginPos_( );

	//! @brief 左下原点の座標を取得
	MousePos GetLeftDownOriginPos_( );

	//! @brief 左上原点の座標を0.0f〜1.0fの割合で取得
	MousePos GetLeftUpOriginPosRatio_( );

	//! @brief 左下原点の座標を0.0f〜1.0fの割合で取得
	MousePos GetLeftDownOriginPosRatio_( );

private:

	static WinDX11Mouse								*s_Gp;

  u32															m_MousePress;
  u32															m_MousePush;
  MousePos												m_MousePos;

  s32                             m_WindowHeight;
  s32                             m_WindowWidth;
};

}}}}


#endif
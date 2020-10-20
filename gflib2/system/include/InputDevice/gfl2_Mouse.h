#ifndef GFLIB2_SYSTEM_INPUTDEVICE_MOUSE_H_INCLUDED
#define GFLIB2_SYSTEM_INPUTDEVICE_MOUSE_H_INCLUDED

#include <stdio.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include "gfl2_InputTypes.h"

#if defined(GF_PLATFORM_WIN32)

#include "WinDx11/gfl2_WinDX11Mouse.h"

#endif

namespace gfl2 { namespace system { namespace InputDevice {

template <class Implement_>
class MouseBase : public Implement_
{
public:

	//! @brief マウスデバイスの初期化
	static b32 Initialize( gfl2::heap::HeapBase *pHeap )
	{
		if ( s_Gp == NULL )
			s_Gp = GFL_NEW(pHeap) MouseBase<Implement_>();

		return s_Gp->Initialize_();
	}

	//! @brief マウスデバイスの解放
	static void Terminate( void )
	{
		if ( s_Gp )
		{
			s_Gp->Terminate_();
			delete s_Gp;
			s_Gp = NULL;
		}
	}

	//! @brief マウスデバイスの更新
	static void Update( void )
	{
		s_Gp->Update_();
	}

	//! @brief ボタンが押されているか？
	static b32 IsPress( MouseButton button )
	{
		return s_Gp->IsPress_( button );
	}

	//! @brief ボタンが押されたか？
	static b32 IsPush( MouseButton button )
	{
		return s_Gp->IsPush_( button );
	}

	//! @brief	入力データを一時的に初期化する
	static void ClearData( )
	{
		s_Gp->ClearData(  );
	}

	//! @brief 左上原点の座標を取得
	static MousePos GetLeftUpOriginPos( )
	{
		return s_Gp->GetLeftUpOriginPos_( );
	}

	//! @brief 左下原点の座標を取得
	static MousePos GetLeftDownOriginPos( )
	{
		return s_Gp->GetLeftDownOriginPos_( );
	}

	//! @brief 左上原点の座標を0.0f〜1.0fの割合で取得
	static MousePos GetLeftUpOriginPosRatio( )
	{
		return s_Gp->GetLeftUpOriginPosRatio_( );
	}

	//! @brief 左下原点の座標を0.0f〜1.0fの割合で取得
	static MousePos GetLeftDownOriginPosRatio( )
	{
		return s_Gp->GetLeftDownOriginPosRatio_( );
	}

private:

	MouseBase():Implement_(){}
	inline virtual ~MouseBase(){}

	static MouseBase					*s_Gp;

};

#if defined(GF_PLATFORM_WIN32)

typedef		MouseBase<WinDX11::WinDX11Mouse>					Mouse;

#endif


}}}

#endif

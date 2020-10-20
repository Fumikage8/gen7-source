#ifndef GFLIB2_SYSTEM_INPUTDEVICE_KEY_BOARD_H_INCLUDED
#define GFLIB2_SYSTEM_INPUTDEVICE_KEY_BOARD_H_INCLUDED

#include <stdio.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include "gfl2_InputTypes.h"

#if defined(GF_PLATFORM_WIN32)

#include "WinDx11/gfl2_WinDX11KeyBoard.h"

#endif

namespace gfl2 { namespace system { namespace InputDevice {

template <class Implement_>
class KeyBoardBase : public Implement_
{
public:

	//! @brief パッドバイスの初期化
#if defined(GF_PLATFORM_WIN32)
	static b32 Initialize( gfl2::heap::HeapBase *pHeap = NULL, HWND hWnd = NULL )
#else
	static b32 Initialize( gfl2::heap::HeapBase *pHeap = NULL )
#endif
	{
		if ( s_Gp == NULL )
		{
			if ( pHeap )
				s_Gp = GFL_NEW(pHeap) KeyBoardBase<Implement_>();
			else
				s_Gp = new KeyBoardBase<Implement_>();//skeltonではヒープを渡せない場所で、Initializeが呼ばれている。skeltonの運用が終わったら削除
		}

#if defined(GF_PLATFORM_WIN32)
    return s_Gp->Initialize_( hWnd );
#else
    return s_Gp->Initialize_();
#endif
	}

	//! @brief パッドデバイスの解放
	static void Terminate( void )
	{
		if ( s_Gp )
		{
			s_Gp->Terminate_();
			delete s_Gp;
			s_Gp = NULL;
		}
	}

	//! @brief パッドデバイスの更新
	static void Update( void )
	{
		s_Gp->Update_();
	}

	//! @brief ボタンが押されているか？
  static b32 IsHold( KeyBoardKey key )
	{
		return s_Gp->IsPress_( key );
	}

	//! @brief ボタンが押されたか？
	static b32 IsTrigger( KeyBoardKey key )
	{
		return s_Gp->IsTrigger_( key );
	}

	//! @brief	入力データを一時的に初期化する
	static void ClearData( void )
	{
		s_Gp->ClearData();
	}

  //! @brief  フレームレート設定
  static void SetFrameRate( FrameRate rate )
  {
    s_Gp->SetFrameRate_( rate );
  }

private:

	KeyBoardBase():Implement_(){}
	inline virtual ~KeyBoardBase(){}

	static KeyBoardBase					*s_Gp;

};

#if defined(GF_PLATFORM_WIN32)

	typedef		KeyBoardBase<WinDX11::WinDX11KeyBoard>					KeyBoard;

#endif


}}}

#endif
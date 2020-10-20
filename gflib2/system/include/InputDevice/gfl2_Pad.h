#ifndef GFLIB2_SYSTEM_INPUTDEVICE_PAD_H_INCLUDED
#define GFLIB2_SYSTEM_INPUTDEVICE_PAD_H_INCLUDED

#include <stdio.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include "gfl2_InputTypes.h"

#if defined(GF_PLATFORM_WIN32)

#include "WinDx11/gfl2_WinDX11Pad.h"

#elif defined(GF_PLATFORM_CTR)

#include "CTR/gfl2_CTRPad.h"

#endif

namespace gfl2 { namespace system { namespace InputDevice {

template <class Implement_>
class PadBase : public Implement_
{
public:

	//! @brief パッドバイスの初期化
	static b32 Initialize( gfl2::heap::HeapBase *pHeap = NULL )
	{
		if ( s_Gp == NULL )
		{
			if ( pHeap )
				s_Gp = GFL_NEW(pHeap) PadBase<Implement_>();
			else
				s_Gp = new PadBase<Implement_>();//skeltonではヒープを渡せない場所で、Initializeが呼ばれている。skeltonの運用が終わったら削除
		}

		return s_Gp->Initialize_();
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

	//! @brief パッドが接続されているか？
	static b32 IsConnecting( u32 pno )
	{
		return s_Gp->IsConnecting_( pno );
	}

	//! @brief ボタンが押されているか？
	static b32 IsPress( u32 pno, Button button )
	{
		return s_Gp->IsPress_( pno,button );
	}

	//! @brief ボタンが押されたか？
	static b32 IsPush( u32 pno, Button button )
	{
		return s_Gp->IsPush_( pno, button );
	}

	//! @brief	入力データを一時的に初期化する
	static void ClearData( u32 pno )
	{
		s_Gp->ClearData( pno );
	}

	//! @brief スティックの傾斜状態を取得
	static const StickPos& GetStick( u32 pno, Stick stick )
	{
		return s_Gp->GetStick_( pno, stick );
	}
  
  //! @brief  フレームレート設定
  static void SetFrameRate( FrameRate rate )
  {
    s_Gp->SetFrameRate_( rate );
  }

	// その他リピート処理、遊び幅設定などはアプリ側にお任せします。
	// 振動制御は、要望があれば対応いたします。

private:

	PadBase():Implement_(){}
	inline virtual ~PadBase(){}

	static PadBase					*s_Gp;

};

#if defined(GF_PLATFORM_WIN32)

	typedef		PadBase<WinDX11::WinDX11Pad>					Pad;

#elif defined(GF_PLATFORM_CTR)

typedef		PadBase<CTR::CTRPad>					Pad;

#endif


}}}

#endif

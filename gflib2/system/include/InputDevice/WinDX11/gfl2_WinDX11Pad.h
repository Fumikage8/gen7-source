#ifndef GFLIB2_SYSTEM_INPUTDEVICE_WINDX11_WINDX11PAD_H_INCLUDED
#define GFLIB2_SYSTEM_INPUTDEVICE_WINDX11_WINDX11PAD_H_INCLUDED

#include <stdio.h>
#include <types/include/gfl2_Typedef.h>
#include <util/include/gfl2_AutoComPtr.h>

#define	DIRECTINPUT_VERSION	0x800
#include <dinput.h>
#pragma comment( lib, "dxguid" )
#pragma comment( lib, "dinput8" )
# undef DrawText
#pragma comment( lib, "winmm" )

namespace gfl2 { namespace system { namespace InputDevice { namespace WinDX11 {

class WinDX11Pad
{
public:
	//機種ごとの拡張コードでユーザ−に与えたい物のみ、ここに出す。
	//例えばパッド付属のタッチパネルインターフェイスなど
	static const u32								MAX_USE_PAD = 8;

	//! @brief アナログトリガー値取得（360コントローラーのみ有効）
	static f32 GetAnalogTrigger( u32 pno );

protected:

	WinDX11Pad();
	inline virtual ~WinDX11Pad();

	//! @brief パッドバイスの初期化
	b32 Initialize_( void );

	//! @brief パッドデバイスの解放
	void Terminate_( void );

	//! @brief パッドデバイスの更新
	void Update_( void );

	//! @brief パッドが接続されているか？
	b32 IsConnecting_( u32 pno );

	//! @brief ボタンが押されているか？
	b32 IsPress_( u32 pno, Button button );

	//! @brief ボタンが押されたか？
	b32 IsPush_( u32 pno, Button button );

	//! @brief	入力データを一時的に初期化する
	void ClearData_( u32 pno );

	//! @brief スティックの傾斜状態を取得
	const StickPos& GetStick_( u32 pno, Stick stick );
   
  //! @brief  フレームレート設定
  void SetFrameRate_( FrameRate rate );

private:

	static const char BUTTON_MAP_XBOX360_PAD[ Button::NumberOf ];
	static const s32 PADBIT[ Button::NumberOf ];

	static BOOL CALLBACK EnumObjectInstance(const DIDEVICEOBJECTINSTANCE* pDIObjInstance, void *context);
	static BOOL CALLBACK EnumPadCallback( const DIDEVICEINSTANCE* devinst, void* ptr );

	static WinDX11Pad								*s_Gp;

	AutoComPtr<IDirectInput8>					m_pInput8;

	class PadInstance
	{
		public:
			PadInstance();
			~PadInstance();

			void ClearData();
			void Update();
			void Release();

			const char*											m_pPadButtonMap;
			DIDEVCAPS												m_PadCaps;
			AutoComPtr<IDirectInputDevice8>		m_pPad;
			b32															m_PadConnecting;

			u32															m_PadPress;
			u32															m_PadPush;
			StickPos												m_PadStick[ Stick::NumberOf ];
			f32															m_AnalogTrigger;
	};

	PadInstance											m_PadInstance[ MAX_USE_PAD ];
	u32															m_PadCount;
    FrameRate                         m_Rate;
    u8                                m_RateBit;

};

}}}}


#endif
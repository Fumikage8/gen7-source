#ifdef  GF_PLATFORM_WIN32

#include <string>
#include <windows.h>

#include <system/include/InputDevice/gfl2_InputTypes.h>
#include <system/include/InputDevice/WinDX11/gfl2_WinDx11pAD.h>

namespace gfl2 { namespace system { namespace InputDevice { namespace WinDX11 {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  グローバル
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

WinDX11Pad				*WinDX11Pad::s_Gp = NULL;

const char WinDX11Pad::BUTTON_MAP_XBOX360_PAD[ Button::NumberOf ] = 
{
	0,	//A,
	1,	//B,
	2,	//X,
	3,	//Y,
	-1,	//L2,
	-1,	//R2,
	4,	//L1,
	5,	//R1,
	6,	//Select,
	8,	//L3,
	9,	//R3,
	7,	//Start,
	-1,	//Up,
	-1,	//Right,
	-1,	//Down,
	-1,	//Left,
};

const s32 WinDX11Pad::PADBIT[ Button::NumberOf ] = {
	(1 <<  0),	//A,
	(1 <<  1),	//B,
	(1 <<  2),	//X,
	(1 <<  3),	//Y,

	(1 <<  4),	//Select,
	(1 <<  5),	//L2,
	(1 <<  6),	//R2,
	(1 <<  7),	//L1,
	(1 <<  8),	//R1,
	
	(1 <<  9),	//L3,
	(1 << 10),	//R3,
	(1 << 11),	//Start,
	(1 << 12),	//Up,
	(1 << 13),	//Right,
	(1 << 14),	//Down,
	(1 << 15),	//Left,
};

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
BOOL CALLBACK WinDX11Pad::EnumObjectInstance(const DIDEVICEOBJECTINSTANCE* pDIObjInstance, void *context)
{
	LPDIRECTINPUTDEVICE8 device = static_cast<LPDIRECTINPUTDEVICE8>(context);

	if(pDIObjInstance->dwType & DIDFT_AXIS)
	{
		{
			DIPROPRANGE propRange;
			propRange.diph.dwSize = sizeof(DIPROPRANGE);
			propRange.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			propRange.diph.dwHow = DIPH_BYID;
			propRange.diph.dwObj = pDIObjInstance->dwType;
			propRange.lMin = 0x0000;
			propRange.lMax = 0xffff;

			if(FAILED(device->SetProperty(DIPROP_RANGE, &propRange.diph)))
			{
				return DIENUM_STOP;
			}
		}
	}

	return DIENUM_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
BOOL CALLBACK WinDX11Pad::EnumPadCallback( const DIDEVICEINSTANCE* devinst, void* ptr )
{
	HRESULT hr = S_OK;

	AutoComPtr< IDirectInputDevice8 >	pDevice;
	PadInstance												&rPad = s_Gp->m_PadInstance[ s_Gp->m_PadCount ];

	//	デバイスを作成
	hr = s_Gp->m_pInput8->CreateDevice( devinst->guidInstance, &pDevice, NULL );
	if( hr )			return DIENUM_CONTINUE;	//	エラーが発生したのでスキップ

	//	デバイスを初期化
	hr = pDevice->SetDataFormat( &c_dfDIJoystick2 );

	//	ハードウェアごとのボタン対応マップを設定
	DWORD vendorID = devinst->guidProduct.Data1 & 0x0000ffff;
	//DWORD productID = (devinst->guidProduct.Data1>>16) & 0x0000ffff;
	const char* buttonMap = NULL;

	//↓360コントローラー以外をサポートする場合は、キーマップを追加すべし
	switch( vendorID ){
	case 0x045E://Microsoft Corp.
		buttonMap = BUTTON_MAP_XBOX360_PAD;
		break;
	default:
		break;
	}

	rPad.m_pPadButtonMap = buttonMap;

	rPad.m_PadCaps.dwSize = sizeof( rPad.m_PadCaps );
	hr = pDevice->GetCapabilities( &rPad.m_PadCaps );
	if( hr )			return DIENUM_CONTINUE;	//	エラーが発生したのでスキップ
		
	hr = pDevice->EnumObjects(WinDX11Pad::EnumObjectInstance, pDevice.get(), DIDFT_ALL);
	if( hr )			return DIENUM_CONTINUE;	//	エラーが発生したのでスキップ

	//	デバイスを退避
	rPad.m_pPad = pDevice;

	++s_Gp->m_PadCount;
	if( s_Gp->m_PadCount < MAX_USE_PAD ){
		return DIENUM_CONTINUE;
	}

	return DIENUM_STOP;
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  WinDX11Padクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
WinDX11Pad::WinDX11Pad():
	m_pInput8( NULL ),
	m_PadCount( 0 )
    ,m_Rate( FrameRate::RATE_60 )
    ,m_RateBit( 0 )
{
	s_Gp = this;
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
WinDX11Pad::~WinDX11Pad()
{
	s_Gp = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  パッドバイスの初期化
 */
//-----------------------------------------------------------------------------
b32 WinDX11Pad::Initialize_( void )
{
	HRESULT						hr;
	const HINSTANCE		hInstance = ::GetModuleHandle( NULL );

	hr = DirectInput8Create( hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast< void** >( &m_pInput8 ), NULL );

	if(FAILED(hr))
		return false;

	hr = m_pInput8->EnumDevices( DI8DEVCLASS_GAMECTRL, EnumPadCallback, NULL, DIEDFL_ATTACHEDONLY );

	if(FAILED(hr))
		return false;

	return true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  パッドデバイスの解放
 */
//-----------------------------------------------------------------------------
void WinDX11Pad::Terminate_( void )
{
	for( u32 i = 0; i < MAX_USE_PAD; ++i )
	{
		m_PadInstance[i].Release();
	}

	m_pInput8.reset();
}

//----------------------------------------------------------------------------
/**
 *	@brief  パッドデバイスの更新
 */
//-----------------------------------------------------------------------------
void WinDX11Pad::Update_( void )
{
  // フレームレート制御、1/60で実行される前提
  switch( m_Rate )
  {
  case FrameRate::RATE_60: break;
  case FrameRate::RATE_30:
    {
      m_RateBit = !m_RateBit;

      if( m_RateBit )
      {
        return;
      }
      break;
    }
  }

	for( u32 i = 0; i < MAX_USE_PAD; ++i )
	{
		this->m_PadInstance[i].Update();
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
b32 WinDX11Pad::IsConnecting_( u32 pno )
{
	return m_PadInstance[pno].m_PadConnecting;
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
b32 WinDX11Pad::IsPress_( u32 pno, Button button )
{
	return (m_PadInstance[pno].m_PadPress & PADBIT[ button ])? true: false;
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
b32 WinDX11Pad::IsPush_( u32 pno, Button button )
{
	return (m_PadInstance[pno].m_PadPush & PADBIT[ button ])? true: false;
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
const StickPos& WinDX11Pad::GetStick_( u32 pno, Stick stick )
{
	return m_PadInstance[pno].m_PadStick[ stick ];
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
void WinDX11Pad::ClearData_( u32 pno )
{
	m_PadInstance[pno].ClearData();
}

//----------------------------------------------------------------------------
/**
 *	@brief アナログトリガー値取得（360コントローラーのみ有効） 
 */
//-----------------------------------------------------------------------------
f32 WinDX11Pad::GetAnalogTrigger( u32 pno )
{
	if ( s_Gp == NULL )
		return 0.0f;
	return s_Gp->m_PadInstance[pno].m_AnalogTrigger;
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  WinDX11Pad::PadInstanceクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
WinDX11Pad::PadInstance::PadInstance() :
	m_pPadButtonMap( NULL ),
	m_pPad( NULL ),
	m_PadConnecting(false)
{
	ClearData();
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
WinDX11Pad::PadInstance::~PadInstance()
{
	
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
void WinDX11Pad::PadInstance::ClearData()
{
	m_PadPress = 0;
	m_PadPush = 0;
	memset( &m_PadStick, 0, sizeof(m_PadStick) );
	m_AnalogTrigger = 0.0f;
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
void WinDX11Pad::PadInstance::Update( void )
{	
	HRESULT hr = S_OK;
	
	if( m_pPad.get() == NULL || m_pPadButtonMap == NULL )
	{
		m_PadConnecting = false;
		return;
	}

	DIJOYSTATE2 state;

	hr = m_pPad->Poll();
	if ( hr )
		m_pPad->Acquire();
	
	hr = m_pPad->GetDeviceState( sizeof(state), &state );
	if( hr )
	{
		m_PadConnecting = false;
		return;
	}

	const DIDEVCAPS& caps = m_PadCaps;

	{
		s32 press = 0;

		for( s32 Lj = 0; Lj < Button::NumberOf; ++Lj )
		{
			const s32 button = m_pPadButtonMap[ Lj ];
			if( button >= 0 && state.rgbButtons[ button ] & 0x80 )
			{
				press |= PADBIT[ Lj ];
			}
		}

		if ( caps.dwPOVs > 0 )
		{
			s32 pov = state.rgdwPOV[0];
			if ( (pov&0xffff) != 0xffff)
			{
				pov /= 100;
				if (( pov >= 0 && pov <= 45 ) || pov >= 315 )
				{
					press |= PADBIT[ Button::Up ];
				}
				if ( pov >= 45 && pov <= 135 )
				{
					press |= PADBIT[ Button::Right ];
				}
				if ( pov >= 135 && pov <= 225 )
				{
					press |= PADBIT[ Button::Down ];
				}
				if ( pov >= 225 && pov <= 315 )
				{
					press |= PADBIT[ Button::Left ];
				}
			}
		}

		if ( m_pPadButtonMap == BUTTON_MAP_XBOX360_PAD )
		{//アナログだがボタンとして処理
			if ( static_cast<s32>(state.lZ) < 0x4000)
			{
				press |= PADBIT[ Button::R2 ];
			}
			if ( static_cast<s32>(state.lZ) > 0xc000)
			{
				press |= PADBIT[ Button::L2 ];
			}

			m_AnalogTrigger = static_cast<f32>(state.lZ) / static_cast<f32>(0xFFFF);
		}

		m_PadPush = (m_PadPress ^ press) & press;
		m_PadPress = press;
	}

	{
		const s32	THRESHOLD = 6000;
		s32				anaX = static_cast<s32>( state.lX ) - 0x8000;

		anaX = (abs(anaX) < THRESHOLD)? 0: anaX;
		m_PadStick[ Stick::Left ].x = (anaX / 32678.0f);

		s32 anaY = static_cast<s32>( state.lY ) - 0x8000;
		anaY = (abs(anaY) < THRESHOLD)? 0: anaY;
		m_PadStick[ Stick::Left ].y = (anaY / 32678.0f);

		s32 anaRX =0;
		s32 anaRY =0;

		if ( m_pPadButtonMap == BUTTON_MAP_XBOX360_PAD )
		{
			anaRX = static_cast<s32>(state.lRx) - 0x8000;
			anaRY = static_cast<s32>(state.lRy) - 0x8000;
		}
		
		anaRX = (abs(anaRX) < THRESHOLD)? 0: anaRX;
		anaRY = (abs(anaRY) < THRESHOLD)? 0: anaRY;

		m_PadStick[ Stick::Right ].x = anaRX/32768.0f;
		m_PadStick[ Stick::Right ].y = anaRY/32768.0f;
	}

	m_PadConnecting = true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
void WinDX11Pad::PadInstance::Release()
{
	if ( m_pPad.get() )
	{
		m_pPad->Unacquire();
		m_pPad.reset();
	}

	m_pPadButtonMap = NULL;
}


//! @brief  フレームレート設定
void WinDX11Pad::SetFrameRate_( FrameRate rate )
{
  m_Rate = rate;
  m_RateBit = 0;
}


}}}}

#endif
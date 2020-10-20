#ifdef  GF_PLATFORM_WIN32

#include <string>
#include <windows.h>

#include <system/include/InputDevice/gfl2_InputTypes.h>
#include <system/include/InputDevice/WinDX11/gfl2_WinDx11KeyBoard.h>

namespace gfl2 { namespace system { namespace InputDevice { namespace WinDX11 {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  グローバル
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

WinDX11KeyBoard				*WinDX11KeyBoard::s_Gp = NULL;

const char WinDX11KeyBoard::KEY_MAP_DEFAULT[ KeyBoardKey::NumberOf ] = 
{
  DIK_ESCAPE,
  DIK_1,
  DIK_2,
  DIK_3,
  DIK_4,
  DIK_5,
  DIK_6,
  DIK_7,
  DIK_8,
  DIK_9,
  DIK_0,
  DIK_MINUS,

  DIK_BACK,
  DIK_TAB,
  DIK_Q,
  DIK_W,
  DIK_E,
  DIK_R,
  DIK_T,
  DIK_Y,
  DIK_U,
  DIK_I,
  DIK_O,
  DIK_P,
  DIK_LBRACKET,
  DIK_RBRACKET,
  DIK_RETURN,
  DIK_LCONTROL,
  DIK_A,
  DIK_S,
  DIK_D,
  DIK_F,
  DIK_G,
  DIK_H,
  DIK_J,
  DIK_K,
  DIK_L,
  DIK_SEMICOLON,

  DIK_LSHIFT,
  DIK_BACKSLASH,
  DIK_Z,
  DIK_X,
  DIK_C,
  DIK_V,
  DIK_B,
  DIK_N,
  DIK_M,
  DIK_COMMA,
  DIK_PERIOD,
  DIK_SLASH,
  DIK_RSHIFT,
  DIK_MULTIPLY,
  DIK_LMENU,
  DIK_SPACE,
  DIK_CAPITAL,
  DIK_F1,
  DIK_F2,
  DIK_F3,
  DIK_F4,
  DIK_F5,
  DIK_F6,
  DIK_F7,
  DIK_F8,
  DIK_F9,
  DIK_F10,
  DIK_NUMLOCK,
  DIK_SCROLL,
  DIK_NUMPAD7,
  DIK_NUMPAD8,
  DIK_NUMPAD9,
  DIK_SUBTRACT,
  DIK_NUMPAD4,
  DIK_NUMPAD5,	
  DIK_NUMPAD6,
  DIK_ADD,
  DIK_NUMPAD1,
  DIK_NUMPAD2,	
  DIK_NUMPAD3,
  DIK_NUMPAD0,
  DIK_DECIMAL,

  DIK_F11,
  DIK_F12,

  DIK_KANA,

  DIK_CONVERT,

  DIK_NOCONVERT,

  DIK_YEN,

  DIK_CIRCUMFLEX,
  DIK_AT,
  DIK_COLON,

  DIK_KANJI,

  DIK_NUMPADENTER,
  DIK_RCONTROL,

  DIK_DIVIDE,

  DIK_SYSRQ,
  DIK_RMENU,

  DIK_PAUSE,

  DIK_HOME,
  DIK_UP,
  DIK_PRIOR,

  DIK_LEFT,

  DIK_RIGHT,

  DIK_END,
  DIK_DOWN,
  DIK_NEXT,
  DIK_INSERT,
  DIK_DELETE,

  DIK_LWIN,
  DIK_RWIN,
  DIK_APPS,
};


//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  WinDX11KeyBoardクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
WinDX11KeyBoard::WinDX11KeyBoard():
	m_pInput8( NULL )
    ,m_pKeyBoard( NULL )
    ,m_Rate( FrameRate::RATE_60 )
    ,m_RateBit( 0 )
{
	s_Gp = this;

  // ステータスバッファの初期化
  ZeroMemory(&m_KeyBoardStatusTable, sizeof(m_KeyBoardStatusTable));
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
WinDX11KeyBoard::~WinDX11KeyBoard()
{
	s_Gp = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  パッドバイスの初期化
 */
//-----------------------------------------------------------------------------
b32 WinDX11KeyBoard::Initialize_( HWND hWnd )
{
  HRESULT hr;
	const HINSTANCE		hInstance = ::GetModuleHandle( NULL );
	
  // ステータスバッファの初期化
  ZeroMemory(&m_KeyBoardStatusTable, sizeof(m_KeyBoardStatusTable));
  
  // DirectInput生成 @attention PADでも作っているので無駄が発生している、nijiでは製品コードではないので許容する
  hr = DirectInput8Create( hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast< void** >( &m_pInput8 ), NULL );
  
  // システムキーボードデバイスを探す
  hr = m_pInput8->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, NULL);
  if(FAILED(hr)){ return false; }
  
  // DirectInputから受け取るデータフォーマットをデバイスに設定
  // c_dfDIKeyboardはDirectInputが用意した固定値グローバル変数
  hr = m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);
  if(FAILED(hr)){ return false; }

  // システムや他のアプリとの協調レベルを設定
  hr = m_pKeyBoard->SetCooperativeLevel(
    hWnd, // hWindow,
    DISCL_NONEXCLUSIVE |	// 非排他的アクセス権を要求する
    DISCL_FOREGROUND |		// フォアグラウンドの時のみアクセス権取得
    //DISCL_BACKGROUND |		// いつでもアクセス権取得
    DISCL_NOWINKEY );		// Windowsキー無効化
  if(FAILED(hr)){ return hr; }

  // キーボード入力を有効にする
  m_pKeyBoard->Acquire();
  

	return true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  パッドデバイスの解放
 */
//-----------------------------------------------------------------------------
void WinDX11KeyBoard::Terminate_( void )
{
  if( m_pKeyBoard.get() )
  {
    m_pKeyBoard->Unacquire();
    m_pKeyBoard.release();
  }

	m_pInput8.reset();
}

//----------------------------------------------------------------------------
/**
 *	@brief  パッドデバイスの更新
 */
//-----------------------------------------------------------------------------
void WinDX11KeyBoard::Update_( void )
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


  HRESULT hr = S_OK;
  BYTE	diks[256];	// キーボードデバイス状態取得配列（既定）
	ZeroMemory(diks, sizeof(diks));
	
  if( m_pKeyBoard.get() == NULL )
	{
		return;
	}

	hr = m_pKeyBoard->GetDeviceState( sizeof(diks), &diks );
	if( FAILED( hr ) )
	{
      m_pKeyBoard->Acquire();
      // キーロスト時は押されていない扱いにする、こうしないとトリガー入力判定がクリアされない
      for(UINT i=0; i< KeyBoardKey::NumberOf; i++){ StatusSet(&m_KeyBoardStatusTable[i], false ); }
		return;
	}

  // キーステータス格納処理
  for(UINT i=0; i < KeyBoardKey::NumberOf; i++){ StatusSet(&m_KeyBoardStatusTable[i], diks[ KEY_MAP_DEFAULT[i] ] & 0x80); }

}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
b32 WinDX11KeyBoard::IsHold_( KeyBoardKey key )
{
  return m_KeyBoardStatusTable[ key ].contOn;
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
b32 WinDX11KeyBoard::IsTrigger_( KeyBoardKey key )
{
  return m_KeyBoardStatusTable[ key ].trgOn;
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
void WinDX11KeyBoard::ClearData_( void )
{
}

//! @brief  フレームレート設定
void WinDX11KeyBoard::SetFrameRate_( FrameRate rate )
{
  m_Rate = rate;
  m_RateBit = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  
 */
//-----------------------------------------------------------------------------
void WinDX11KeyBoard::StatusSet(KEYBUTTON_STATUS* keybutton, BOOL trg)
{
  keybutton->trgOn = 0;
  keybutton->trgOff = 0;
  keybutton->trgOnDouble = 0;

  // ダブルクリック受付可能時間減少
  if(keybutton->trgOnDelayWait){ keybutton->trgOnDelayWait--; }

  if(trg){
    if(!keybutton->contOn){
      keybutton->trgOn = 1;

      if(keybutton->trgOnDelayWait){
        // ダブルクリック
        keybutton->trgOnDouble = 1;
        keybutton->trgOnDelayWait = 0;
      } else {
        // シングルクリック
        keybutton->trgOnDelayWait = 8;
      }
    }
    keybutton->contOn = 1;
  } else {
    if(keybutton->contOn){
      keybutton->trgOff = 1;
    }
    keybutton->contOn = 0;
  }
}

}}}}

#endif
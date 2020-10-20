#include <system/include/InputDevice/gfl2_InputTypes.h>
#include <system/include/InputDevice/CTR/gfl2_CTRPad.h>

namespace gfl2 { namespace system { namespace InputDevice { namespace CTR {

const s32 CTRPad::PADBIT[ Button::NumberOf ] = 
{
	nn::hid::BUTTON_A,//A,	//!< Aボタン(MS, Nintendo系)
	nn::hid::BUTTON_B,//B,	//!< Bボタン(MS, Nintendo系)
	nn::hid::BUTTON_X,//X,	//!< Xボタン(MS, Nintendo系)
	nn::hid::BUTTON_Y,//Y,	//!< Yボタン(MS, Nintendo系)
	0,//L2,
	0,//R2,
	nn::hid::BUTTON_L,//L1,
	nn::hid::BUTTON_R,//R1,
	nn::hid::PSEUDO_BUTTON_SELECT,//Select,
	0,//L3,
	0,//R3,
	nn::hid::BUTTON_START,//Start,
	nn::hid::BUTTON_UP,//Up,
	nn::hid::BUTTON_RIGHT,//Right,
	nn::hid::BUTTON_DOWN,//Down,
	nn::hid::BUTTON_LEFT,//Left,
};

CTRPad::CTRPad():
	padReadLen( 0 )
{

}

CTRPad::~CTRPad()
{

}

//! @brief パッドバイスの初期化
b32 CTRPad::Initialize_( void )
{
	return true;
}

//! @brief パッドデバイスの解放
void CTRPad::Terminate_( void )
{

}

//! @brief パッドデバイスの更新
void CTRPad::Update_( void )
{
	padReader.Read( padStatusArray,&padReadLen, 32 );
	padReader.ReadLatest( &padStatus );

	m_PadStick.x = padStatus.stick.x;
	m_PadStick.y = padStatus.stick.y;
	m_PadStick.x /= static_cast<f32>( nn::hid::CTR::LIMIT_OF_STICK_CLAMP_MAX );
	m_PadStick.y /= static_cast<f32>( nn::hid::CTR::LIMIT_OF_STICK_CLAMP_MAX );
}

//! @brief パッドが接続されているか？
b32 CTRPad::IsConnecting_( u32 pno )
{
	if ( pno )		return false;

	return true;
}

//! @brief ボタンが押されているか？
b32 CTRPad::IsPress_( u32 pno, Button button )
{
	if ( pno )		return false;

	if ( padStatus.hold & PADBIT[button] )
		return true;

	return false;
}

//! @brief ボタンが押されたか？
b32 CTRPad::IsPush_( u32 pno, Button button )
{
	if ( pno )		return false;

	if ( padStatus.trigger & PADBIT[button] )
		return true;

	return false;
}

//! @brief	入力データを一時的に初期化する
void CTRPad::ClearData_( u32 pno )
{
	
}

//! @brief スティックの傾斜状態を取得
const StickPos& CTRPad::GetStick_( u32 pno, Stick stick )
{
	static const StickPos			dummy;

	if ( pno )													return dummy;
	if ( stick == Stick::Right )				return dummy;

	return m_PadStick;
}

}}}}
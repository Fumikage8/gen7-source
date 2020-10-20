#ifndef GFLIB2_SYSTEM_INPUTDEVICE_TYPES_H_INCLUDED
#define GFLIB2_SYSTEM_INPUTDEVICE_TYPES_H_INCLUDED

#include <types/include/gfl2_Typedef.h>
#include <assert.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DECLARE_ENUM_BEGIN( CLASSNAME )														\
	class CLASSNAME {                                               \
	public:																                          \
		enum Enum {
#define DECLARE_ENUM_END( CLASSNAME )									            \
		};																                            \
	public:																                          \
		CLASSNAME(){}												                          \
		explicit CLASSNAME( int value ) : value_( Enum( value ) ) {}	\
		CLASSNAME( Enum value ) : value_( value ) {}					        \
		operator Enum () const { return value_; }					            \
	private:															                          \
		Enum value_;													                        \
	};

namespace gfl2 { namespace system { namespace InputDevice {
  //-------------------------------------------------------------------------------------------------------------------------------------------------
  //! @brief 動作フレームレート
  DECLARE_ENUM_BEGIN( FrameRate )
    RATE_60,
    RATE_30,
    NumberOf,

  DECLARE_ENUM_END( FrameRate )
  
  //-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief パッドのボタン
	DECLARE_ENUM_BEGIN( Button )

		A,	//!< Aボタン(MS, Nintendo系)
		B,	//!< Bボタン(MS, Nintendo系)
		X,	//!< Xボタン(MS, Nintendo系)
		Y,	//!< Yボタン(MS, Nintendo系)

		L2,
		R2,
		L1,
		R1,

		Select,
		L3,
		R3,
		Start,
		Up,
		Right,
		Down,
		Left,

		NumberOf,

		Cross = A,			//!< ×ボタン(Sony系)
		Circle = B,			//!< ○ボタン(Sony系)
		Square = X,			//!< □ボタン(Sony系)
		Triangle = Y,		//!< △ボタン(Sony系)

		//海外では×ボタンが決定だったりするので、決定ボタン定義なんかは入れちゃだめよ。

	DECLARE_ENUM_END( Button )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief パッドのスティック
	DECLARE_ENUM_BEGIN( Stick )

		Right = 0,
		Left  = 1,

		NumberOf,

	DECLARE_ENUM_END( Stick )	

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief アナログスティック座標
	struct StickPos {
		StickPos():x(0),y(0){}
		float x;
		float y;
	};

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief マウスのボタン
	DECLARE_ENUM_BEGIN( MouseButton )

  Left,
  Middle,
  Right,

  NumberOf,

	DECLARE_ENUM_END( MouseButton )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief キーボードのキー
  DECLARE_ENUM_BEGIN( KeyBoardKey )

  KBKEY_ESC = 0,
    KBKEY_1,
    KBKEY_2,
    KBKEY_3,
    KBKEY_4,
    KBKEY_5,
    KBKEY_6,
    KBKEY_7,
    KBKEY_8,
    KBKEY_9,
    KBKEY_0,
    KBKEY_HYPHEN,

    KBKEY_BACKSPACE,
    KBKEY_TAB,
    KBKEY_Q,
    KBKEY_W,
    KBKEY_E,
    KBKEY_R,
    KBKEY_T,
    KBKEY_Y,
    KBKEY_U,
    KBKEY_I,
    KBKEY_O,
    KBKEY_P,
    KBKEY_BRACKET_L,
    KBKEY_BRACKET_R,
    KBKEY_ENTER,
    KBKEY_CTRL_L,
    KBKEY_A,
    KBKEY_S,
    KBKEY_D,
    KBKEY_F,
    KBKEY_G,
    KBKEY_H,
    KBKEY_J,
    KBKEY_K,
    KBKEY_L,
    KBKEY_SEMICOLON,

    KBKEY_SHIFT_L,
    KBKEY_BACKSLASH,
    KBKEY_Z,
    KBKEY_X,
    KBKEY_C,
    KBKEY_V,
    KBKEY_B,
    KBKEY_N,
    KBKEY_M,
    KBKEY_COMMA,
    KBKEY_PERIOD,
    KBKEY_SLASH,
    KBKEY_SHIFT_R,
    KBKEY_NUM_MUL,
    KBKEY_ALT_L,
    KBKEY_SPACE,
    KBKEY_CAPSLOCK,
    KBKEY_F1,
    KBKEY_F2,
    KBKEY_F3,
    KBKEY_F4,
    KBKEY_F5,
    KBKEY_F6,
    KBKEY_F7,
    KBKEY_F8,
    KBKEY_F9,
    KBKEY_F10,
    KBKEY_NUMLOCK,
    KBKEY_SCRLOCK,
    KBKEY_NUM_7,
    KBKEY_NUM_8,
    KBKEY_NUM_9,
    KBKEY_NUM_SUB,
    KBKEY_NUM_4,
    KBKEY_NUM_5,
    KBKEY_NUM_6,
    KBKEY_NUM_ADD,
    KBKEY_NUM_1,
    KBKEY_NUM_2,
    KBKEY_NUM_3,
    KBKEY_NUM_0,
    KBKEY_NUM_DEC,

    KBKEY_F11,
    KBKEY_F12,

    KBKEY_KANA,

    KBKEY_CONVERT,

    KBKEY_NOCONVERT,

    KBKEY_YENSIGN,

    KBKEY_CARET,
    KBKEY_ATSIGN,
    KBKEY_COLON,

    KBKEY_SIZE,

    KBKEY_NUM_ENTER,
    KBKEY_CTRL_R,

    KBKEY_NUM_DIV,

    KBKEY_PRINTSCRN,
    KBKEY_ALT_R,

    KBKEY_PAUSE,

    KBKEY_HOME,
    KBKEY_ARROW_U,
    KBKEY_PAGEUP,

    KBKEY_ARROW_L,

    KBKEY_ARROW_R,

    KBKEY_END,
    KBKEY_ARROW_D,
    KBKEY_PAGEDOWN,
    KBKEY_INSERT,
    KBKEY_DELETE,

    KBKEY_WINDOWS_L,
    KBKEY_WINDOWS_R,
    KBKEY_APP,
    NumberOf,

    DECLARE_ENUM_END( KeyBoardKey )

  //-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief マウス座標
	struct MousePos {
		float x;
		float y;
	};

}}}

#undef DECLARE_ENUM_BEGIN
#undef DECLARE_ENUM_END

#endif


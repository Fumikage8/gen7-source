/* 
 * File:   gfl_Fcurve.h
 * Author: kimura_shin
 *
 * Created on 2010/12/02, 16:54
 */

#ifndef GFL_FCCALC_H
#define GFL_FCCALC_H
#pragma once

#ifndef USE_SPLINE_SLOPE_0
#define USE_SPLINE_SLOPE_0
#endif
//#define FC_KEY_FLOAT
#define KEYSUM_WITHOUT_BEGIN_END
//#define PIVOT_DATA
//#define USE_TEXTURE_ANIMATION // テクスチャーアニメーションデータを使用する

namespace gfl {
namespace motion {

/**
 * Function Curve Class
 */

class Fcurve {
	GFL_FORBID_COPY_AND_ASSIGN( Fcurve );
private:
	// internal structure
	/**
	 * @brief The FcurveKey struct
	 */
	struct FcurveKey {
		s32 _Kind;
		u16 _Sum;
		u16 _KeyNum;
		f32* _pValue;
#ifndef FC_KEY_FLOAT
		void* _pKeyFrame;
#endif
	};

	/**
	 * @brief The PivotKey struct
	 */
	struct PivotKey {
		u16 _Sum;
		u16 _KeyNum;
		u8* _pSide;
		void* _pKeyFrame;

		PivotKey( void ) : _Sum( 0 ) {
		}
	};

#ifdef USE_TEXTURE_ANIMATION
	/**
	 * @brief The TextureKey struct
	 */
	struct TextureKey {

		TextureKey( ) : m_Value( -1.0f ) {
		}
		f32 m_Value;
		u16 m_Sum;
		u16 m_KeyNum;
		u8* m_pValue;
		void* m_pKeyFrame;
	};
#endif

	PivotKey _Pivot;

	FcurveKey* _pFck;
#ifdef USE_TEXTURE_ANIMATION
	TextureKey *m_pTxk;
#endif
#ifdef FC_KEY_FLOAT
	f32* _pKeyFrame;
#endif
	// template なので関数ポインタが取れない。実体を宣言して２回ジャンプするのとどっちが…
	//		typedef bool (*GetValueByFrameFunc)( f32 *value, const f32 frame, s32 end );
	//		GetValueByFrameFunc mpGetValue;

	f32 _Frame;
	f32 _OldFrame;
	f32 _FrameCount;
	f32 _FrameSpeed; // 1.0f で標準　少なければスロー
	u16 _FcCount;
	u8 m_TextureCount;
	u8 m_Dummy8;

public:
	/**
	 * ファンクションカーブの種類
	 */

	enum {
#ifdef USE_SPLINE_SLOPE_0
		FC_STATIC_0,
		FC_STATIC_BASEPOSE,
		FC_STATIC_HALF_PI,
		FC_STATIC_PI,
		FC_STATIC_ONE_HALF_PI,
		FC_STATIC_DATA,
		// above here static data
		// below here spline data
		FC_SPLINE_EXIST,
		FC_SPLINE_SLOPE_0 = FC_SPLINE_EXIST,
		FC_SPLINE_DATA,
		FC_DATA_MASK = 7 // 3bit mask
#else
		FC_STATIC_0,
		FC_STATIC_DATA,
		FC_STATIC_BASEPOSE,
		FC_SPLINE_EXIST,
		FC_SPLINE_DATA = FC_SPLINE_EXIST,
		FC_DATA_MASK = 3 // 2bit mask
#endif
	};

	static const f32 ONE_FRAME;
	static const f32 ONE_FRAME_TIME;

	/**
	 * @brief Fcurve コンストラクタ
	 */

	Fcurve( void ) : _pFck( 0 )
#ifdef USE_TEXTURE_ANIMATION
	, m_pTxk( 0 )
#endif
#ifdef FC_KEY_FLOAT
	, _pKeyFrame( 0 )
#endif
	, _Frame( 0.0f ), _OldFrame( 0.0f ), _FrameCount( 0.0f ), _FrameSpeed( 1.0f ), _FcCount( 0 ) {
	}

	~Fcurve( void ) {
		GFL_DELETE_ARRAY _pFck;
#ifdef USE_TEXTURE_ANIMATION
		GFL_DELETE_ARRAY m_pTxk;
#endif
#ifdef FC_KEY_FLOAT
		Free( _pKeyFrame );
#endif
	}

	/**
	 * @brief FrameToTime
	 * @param f フレーム数
	 * @return 時間（秒）
	 */
	static inline f32 FrameToTime( const s32 f ) {
		return ONE_FRAME_TIME * (f32) f;
	}

	/**
	 * @brief FrameToTime
	 * @param f フレーム数
	 * @return 時間（秒）
	 */
	static inline f32 FrameToTime( const f32 f ) {
		return ONE_FRAME_TIME * f;
	}

	/**
	 * @brief TimeToFrame
	 * @param t 時間（秒）
	 * @return フレーム数
	 */
	static inline f32 TimeToFrame( const f32 t ) {
		return ONE_FRAME * t;
	}

	/**
	 * @brief Initialize 初期化
	 * @param adr　モーションデータポインタ
	 * @param max_size　Fcurve数　（いらない？）
	 * @param heap　ヒープアドレス
	 * @return 真　成功　偽　失敗
	 */
	bool Initialize( const void* adr, const s32 max_size, gfl::heap::HeapBase* heap );
	/**
	 * @brief GetValueBegin ０フレームの値を計算
	 * @param value　出力値ポインタ
	 */
	void GetValueBegin( f32 *value );
	/**
	 * @brief GetValueEnd 最後のフレームの値を計算
	 * @param value　出力値ポインタ
	 * @param end　どの Fcurve まで計算するか　０なら最後まで
	 */
	void GetValueEnd( f32 *value, s32 end = 0 );

	/**
	 * @brief QueryEnd モーションが終わったか？
	 * @return 真　終わった　偽　終わっていない
	 */
	bool QueryEnd( void ) {
		return GetFrameCount( ) <= GetFrame( ); // 最後のフレームを含まない
		//			return _OldFrame <= GetFrameCount( ) && GetFrameCount( ) < GetFrame( ); // 最後のフレームを含む
	}
	/**
	 * @brief QueryEndBefore
	 * @return　真　終わった　偽　終わっていない
	 * @remark もういらないかも
	 */
	bool QueryEndBefore( void ) {
		return GetFrameCount( )-1 <= GetFrame( ); // 最後のフレームを含まない
	}
#ifdef FC_KEY_FLOAT
	bool GetValueByFrame( f32 *value, const f32 frame, s32 end = 0 );
#else
	template<typename T>
	bool GetValueByFrame_Type( f32 *value, const f32 frame, s32 end = 0 );

	/**
	 * @brief GetValueByFrame
	 * @param value 設定したい値
	 * @param frame　入力フレーム
	 * @param end　何番目のジョイントまで計算するか。指定されなければ最後まで。
	 * @return 真　計算成功　偽　計算失敗
	 */
	bool GetValueByFrame( f32 *value, const f32 frame, s32 end = 0 ) {
		return ( _FrameCount < 256.0f) ?
			GetValueByFrame_Type<u8 > (value, frame, end) :
			GetValueByFrame_Type<u16 > (value, frame, end);
	}
#endif

	/**
	 * @brief GetValueByDeltaFrame デルタフレームから値を計算
	 * @param value 出力値ポインタ
	 * @param f デルタフレーム
	 * @param end　どの Fcurve まで計算するか　０なら最後まで
	 * @return
	 */
	inline bool GetValueByDeltaFrame( f32 *value, const f32 f, s32 end = 0 ) {
		return GetValueByFrame( value, _Frame + f, end );
	}

	/**
	 * @brief GetFcCount ファンクションカーブの数を取得
	 * @return ファンクションカーブ数
	 */
	s32 GetFcCount( void ) const {
		return _FcCount;
	}

	/**
	 * @brief GetFrameCount 総フレーム数を取得
	 * @return　総フレーム数
	 */
	f32 GetFrameCount( void ) const {
		return _FrameCount;
	}

	/**
	 * @brief GetFrame 現在のフレームを取得
	 * @return　フレーム
	 */
	f32 GetFrame( void ) const {
		return _Frame;
	}

	/**
	 * @brief SetFrame フレームを設定
	 * @param frame　フレーム
	 */
	void SetFrame( const f32 frame ) {
		_OldFrame = _Frame;
		_Frame = frame;
	}

	/**
	 *   再生スピードを取得
	 * @param  なし
	 * @return 再生スピード
	 */

	f32 GetFrameSpeed( void ) const {
		return _FrameSpeed;
	}

	/**
	 *   再生スピードを設定
	 * @param  spd 再生スピード
	 * @return なし
	 */

	void SetFrameSpeed( const f32 spd ) {
		_FrameSpeed = spd;
	}

	/**
	 * @brief ClearKey 保存してある前フレーム値をクリアする
	 */
	void ClearKey( void );

	f32 GetTextureValue( const s32 id ) const {
#ifdef USE_TEXTURE_ANIMATION
		if( m_pTxk ){
			return m_pTxk[id].m_Value;
		}
#endif
		return -1.0f;
	}

	void SetTextureCount( u8 TextureCount ) {
		this->m_TextureCount = TextureCount;
	}

	u8 GetTextureCount( ) const {
		return m_TextureCount;
	}

	/**
	 * @brief GetOldFrame 一つ前に計算したフレームを取得
	 * @return 一つ前に計算したフレーム
	 */
	f32 GetOldFrame( ) const {
		return _OldFrame;
	}

	/**
	 * @brief CheckLocatorYPosExist ロケーターYが存在しているか？
	 * @return 真　存在している　偽　存在していない
	 */
	s32 CheckLocatorYPosExist( ) const {
		return _pFck[1]._Kind;
	}
};

}
}

#endif // GFL_FCCALC_H

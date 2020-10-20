/* 
 * File:   gfl_Utility.h
 * Author: s-kimura
 *
 * Created on 2010/09/18, 15:32
 */

#ifndef GFL_UTILITY_H
#define	GFL_UTILITY_H
#pragma once

namespace gfl {
namespace core {

/**
 *  ユーティリティクラス
 */

class Utility {
public:

	/**
	 *   サイズでアラインして切り上げる関数
	 * @param  val  入力変数
	 * @param  size アラインされるサイズ
	 * @return アラインされた値
	 */

	template<typename T, typename S>
	static inline T AlignAddress( T val, S size ) {
		return (T) (((size_t) val + (size - 1)) & ~(size - 1));
	}

	/**
	 *   アドレスを加算する関数
	 * @param  adr 入力アドレス
	 * @param  val 値
	 * @return 加算された値
	 * @remark ポインターは符号無しなので減算の場合には SubAddress を使用してください。
	 */

	template<typename T, typename S>
	static inline T AddAddress( T adr, S val ) {
		return reinterpret_cast<T> ((size_t) adr + (size_t) val);
	}

	/**
	 *   アドレスを減算する関数
	 * @param  adr 入力アドレス
	 * @param  val 値 減算したい値の「絶対値」
	 * @return 減算された値
	 */

	template<typename T, typename S>
	static inline T SubAddress( T adr, S val ) {
		return reinterpret_cast<T> ((size_t) adr - (size_t) val);
	}

	/**
	 * パディングを入れる
     * @param dat データ
     * @param aln　アライン　4, 8, 16 等 2^x
     * @return　アラインされたデータ
     */
	static u8* PaddingAddress( u8* dat, u32 aln ){
		u32 pad = ((u32)dat) & (aln-1);
		if(pad){
			pad = aln - pad;
			for(u32 i = 0; i < pad; ++i){
				*dat++ = 0x00;
			}
		}
		return dat;
	}

	/**
	 *   文字列からハッシュを求める
	 * @param  str 入力文字列
	 * @return ハッシュ
	 */

	static u32 CalcHash( const c8* str ) {
		u32 value = 0;
		while(*str) {
			value *= HASH_MAGIC_NUMBER;
			value ^= static_cast<u32> (*str++);
		}
		return value;
	}

	/**
	 *   メモリーコピー関数
	 * @param  out 出力バッファー
	 * @param  in 入力バッファー
	 * @param  size サイズ
	 * @return 無し
	 * @remark in と out の領域はオーバーラップしていてはいけない。
	 * 各プラットフォーム用に高速な命令を使う予定。
	 */

	static inline void Memcpy( void* out, const void* in, size_t size ) {
		memcpy( out, in, size );
	}

	/**
	 *   メモリーコピー関数
	 * @param  out 出力バッファー
	 * @param  in 入力バッファー
	 * @param  size サイズ
	 * @return 無し
	 * @remark in と out の領域はオーバーラップしていてもいいが、Memcpy より低速。
	 * 各プラットフォーム用に高速な命令を使う予定。
	 */

	static inline void Memmove( void* out, const void* in, size_t size ) {
		memmove( out, in, size );
	}

	static inline void Memset( void* out, const int val, size_t size ) {
		memset( out, val, size );
	}

	static inline s32 Strcmp( const c8 * const str1, const c8 * const str2 ) {
#ifdef GFL_PLATFORM_MICROSOFT
#ifdef GFL_UNICODE
		return lstrcmp( (LPCWSTR) str1, (LPCWSTR) str2 );
#else
		return lstrcmp( str1, str2 );
#endif
#else
		return strcmp( str1, str2 );
#endif
	}

	static inline s32 Strncmp( const c8 * const str1, const c8 * const str2, const size_t size ) {
		//#ifdef GFL_PLATFORM_MICROSOFT
		//			return lstrncmp(str1,str2,size);
		//#else
		return strncmp( str1, str2, size );
		//#endif
	}

#ifdef GFL_PLATFORM_PC
	// -------------------------------------------------------------------------
	// Scanf の不定形対応版
	// -------------------------------------------------------------------------

	template<typename T>
	static s32 SscanfInteger( T* data, const c8* text ) {
		c8* ptr = const_cast<c8*> (text);
		c8 buf[0x20]; // これで収まるでしょ
		c8 c;
		c8* buf_ptr = buf;
		s32 count = 0;
		for(;; ){
			c = *ptr++;
			if( c == ' ' || c == '\n' ){
				if( buf_ptr != buf ){
					*buf_ptr = 0x00;
					// 文字列から整数に変換して係数を掛ける
					*data++ = static_cast<T> (atoi( buf ));
					count++;
					buf_ptr = buf;
				}
			} else if( c != '\r' ){
				*buf_ptr++ = c;
			}
			if( !c ){
				if( buf != buf_ptr && (isdigit( buf[0] ) || buf[0] == '-') ){
					*buf_ptr = 0x00;
					// 文字列から整数に変換して係数を掛ける
					*data = static_cast<T> (atoi( buf ));
					count++;
				}
				break;
			}
		}
		return count;
	}

	static s32 SscanfFloat( f32* data, const c8* text, const f32 mul );
	static c8* DecodeBase64( const c8* src, u32* size = 0 );
	static c8* EncodeBase64( const c8* src, u32 src_size, u32* size = 0 );
#endif

private:
#ifdef GFL_PLATFORM_PC
	static const c8 * const s_pBase64;
#endif
	static const u32 HASH_MAGIC_NUMBER = 0x83;
};

/**
 *  平均クラス
 */

template <typename T>
class Average {
private:
	T m_Value;
	T m_Average;

	T m_WorkSum;
	u32 m_WorkCount;
	u32 m_ResetCount;

	T GetWorkSum( void ) const {
		return m_WorkSum;
	}

	void SetWorkSum( T val ) {
		m_WorkSum = val;
	}

	void SetAverage( T val ) {
		m_Average = val;
	}

	void SetWorkCount( u32 val ) {
		m_WorkCount = val;
	}

public:

	Average( const u32 reset_count = 0 ) : m_Value( 0 ), m_Average( 0 ),
	m_WorkSum( 0 ), m_WorkCount( 0 ), m_ResetCount( reset_count ) {
	}

	/**
	 *   Value　のゲッター
	 */

	T GetValue( void ) const {
		return m_Value;
	}

	void SetValue( T val ) {
		m_Value = val;
	}

	/**
	 *   Average　のゲッター
	 */

	T GetAverage( void ) const {
		return m_Average;
	}

	u32 GetWorkCount( void ) const {
		return m_WorkCount;
	}

	void SetResetCount( u32 val ) {
		m_ResetCount = val;
	}

	/**
	 *   更新
	 * @param  val 入力値
	 * @return 無し
	 */

	bool Update( const T val ) {
		SetWorkSum( val + GetWorkSum( ) );
		SetValue( val );
		++m_WorkCount;
		if( m_ResetCount && m_ResetCount <= m_WorkCount ){
			Reset( );
			return true;
		}
		return false;
	}

	/**
	 *   値のリセット
	 * @param  無し
	 * @return 無し
	 */

	void Reset( void ) {
		SetAverage( (f32) GetWorkSum( ) / (f32) GetWorkCount( ) );
		SetWorkSum( 0 );
		SetWorkCount( 0 );
	}
}; // class Average

/**
 *  最小最大平均クラス
 */

template <typename T>
class MinMaxAverage : public Average<T> {
private:
	T m_Min;
	T m_Max;

	T m_WorkMin;
	T m_WorkMax;

	T GetWorkMin( void ) const {
		return m_WorkMin;
	}

	void SetWorkMin( T val ) {
		m_WorkMin = val;
	}

	T GetWorkMax( void ) const {
		return m_WorkMax;
	}

	void SetWorkMax( T val ) {
		m_WorkMax = val;
	}

	void SetMin( T val ) {
		m_Min = val;
	}

	void SetMax( T val ) {
		m_Max = val;
	}

public:

	void SetWorkMinInit( f32 ) {
		m_WorkMin = Math::F32_MAX;
	}

	void SetWorkMinInit( u32 ) {
		m_WorkMin = Math::U32_MAX;
	}

	void SetWorkMinInit( u64 ) {
		m_WorkMin = Math::U64_MAX;
	}

	void SetWorkMinInit( s64 ) {
		m_WorkMin = Math::S64_MAX;
	}

	void SetWorkMinInit( s32 ) {
		m_WorkMin = Math::S32_MAX;
	}

	MinMaxAverage( const u32 reset_count = 0 ) : Average<T>(reset_count), m_Min( 0 ), m_Max( 0 ), m_WorkMax( 0 ) {
		SetWorkMinInit( (T) 0 );
	}

	/**
	 *   Min　のゲッター
	 */

	T GetMin( void ) const {
		return m_Min;
	}

	/**
	 *   Max　のゲッター
	 */

	T GetMax( void ) const {
		return m_Max;
	}

	/**
	 *   更新
	 * @param  val 入力値
	 * @return 無し
	 */

	bool Update( const T val ) {
		if( val < GetWorkMin( ) ){
			SetWorkMin( val );
		}
		if( GetWorkMax( ) < val ){
			SetWorkMax( val );
		}
		return Average<T>::Update( val );
	}

	/**
	 *   値のリセット
	 * @param  無し
	 * @return 無し
	 */

	void Reset( void ) {
		SetMin( GetWorkMin( ) );
		SetMax( GetWorkMax( ) );

		Average<T>::Reset( );

		SetWorkMinInit( (T) 0 );
		SetWorkMax( 0 );
	}
}; // class MinMaxAverage

}
}

#endif	/* GFL_UTILITY_H */

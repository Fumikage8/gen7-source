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
 *  ���[�e�B���e�B�N���X
 */

class Utility {
public:

	/**
	 *   �T�C�Y�ŃA���C�����Đ؂�グ��֐�
	 * @param  val  ���͕ϐ�
	 * @param  size �A���C�������T�C�Y
	 * @return �A���C�����ꂽ�l
	 */

	template<typename T, typename S>
	static inline T AlignAddress( T val, S size ) {
		return (T) (((size_t) val + (size - 1)) & ~(size - 1));
	}

	/**
	 *   �A�h���X�����Z����֐�
	 * @param  adr ���̓A�h���X
	 * @param  val �l
	 * @return ���Z���ꂽ�l
	 * @remark �|�C���^�[�͕��������Ȃ̂Ō��Z�̏ꍇ�ɂ� SubAddress ���g�p���Ă��������B
	 */

	template<typename T, typename S>
	static inline T AddAddress( T adr, S val ) {
		return reinterpret_cast<T> ((size_t) adr + (size_t) val);
	}

	/**
	 *   �A�h���X�����Z����֐�
	 * @param  adr ���̓A�h���X
	 * @param  val �l ���Z�������l�́u��Βl�v
	 * @return ���Z���ꂽ�l
	 */

	template<typename T, typename S>
	static inline T SubAddress( T adr, S val ) {
		return reinterpret_cast<T> ((size_t) adr - (size_t) val);
	}

	/**
	 * �p�f�B���O������
     * @param dat �f�[�^
     * @param aln�@�A���C���@4, 8, 16 �� 2^x
     * @return�@�A���C�����ꂽ�f�[�^
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
	 *   �����񂩂�n�b�V�������߂�
	 * @param  str ���͕�����
	 * @return �n�b�V��
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
	 *   �������[�R�s�[�֐�
	 * @param  out �o�̓o�b�t�@�[
	 * @param  in ���̓o�b�t�@�[
	 * @param  size �T�C�Y
	 * @return ����
	 * @remark in �� out �̗̈�̓I�[�o�[���b�v���Ă��Ă͂����Ȃ��B
	 * �e�v���b�g�t�H�[���p�ɍ����Ȗ��߂��g���\��B
	 */

	static inline void Memcpy( void* out, const void* in, size_t size ) {
		memcpy( out, in, size );
	}

	/**
	 *   �������[�R�s�[�֐�
	 * @param  out �o�̓o�b�t�@�[
	 * @param  in ���̓o�b�t�@�[
	 * @param  size �T�C�Y
	 * @return ����
	 * @remark in �� out �̗̈�̓I�[�o�[���b�v���Ă��Ă��������AMemcpy ���ᑬ�B
	 * �e�v���b�g�t�H�[���p�ɍ����Ȗ��߂��g���\��B
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
	// Scanf �̕s��`�Ή���
	// -------------------------------------------------------------------------

	template<typename T>
	static s32 SscanfInteger( T* data, const c8* text ) {
		c8* ptr = const_cast<c8*> (text);
		c8 buf[0x20]; // ����Ŏ��܂�ł���
		c8 c;
		c8* buf_ptr = buf;
		s32 count = 0;
		for(;; ){
			c = *ptr++;
			if( c == ' ' || c == '\n' ){
				if( buf_ptr != buf ){
					*buf_ptr = 0x00;
					// �����񂩂琮���ɕϊ����ČW�����|����
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
					// �����񂩂琮���ɕϊ����ČW�����|����
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
 *  ���σN���X
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
	 *   Value�@�̃Q�b�^�[
	 */

	T GetValue( void ) const {
		return m_Value;
	}

	void SetValue( T val ) {
		m_Value = val;
	}

	/**
	 *   Average�@�̃Q�b�^�[
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
	 *   �X�V
	 * @param  val ���͒l
	 * @return ����
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
	 *   �l�̃��Z�b�g
	 * @param  ����
	 * @return ����
	 */

	void Reset( void ) {
		SetAverage( (f32) GetWorkSum( ) / (f32) GetWorkCount( ) );
		SetWorkSum( 0 );
		SetWorkCount( 0 );
	}
}; // class Average

/**
 *  �ŏ��ő啽�σN���X
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
	 *   Min�@�̃Q�b�^�[
	 */

	T GetMin( void ) const {
		return m_Min;
	}

	/**
	 *   Max�@�̃Q�b�^�[
	 */

	T GetMax( void ) const {
		return m_Max;
	}

	/**
	 *   �X�V
	 * @param  val ���͒l
	 * @return ����
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
	 *   �l�̃��Z�b�g
	 * @param  ����
	 * @return ����
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

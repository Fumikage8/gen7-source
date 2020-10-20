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
//#define USE_TEXTURE_ANIMATION // �e�N�X�`���[�A�j���[�V�����f�[�^���g�p����

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
	// template �Ȃ̂Ŋ֐��|�C���^�����Ȃ��B���̂�錾���ĂQ��W�����v����̂Ƃǂ������c
	//		typedef bool (*GetValueByFrameFunc)( f32 *value, const f32 frame, s32 end );
	//		GetValueByFrameFunc mpGetValue;

	f32 _Frame;
	f32 _OldFrame;
	f32 _FrameCount;
	f32 _FrameSpeed; // 1.0f �ŕW���@���Ȃ���΃X���[
	u16 _FcCount;
	u8 m_TextureCount;
	u8 m_Dummy8;

public:
	/**
	 * �t�@���N�V�����J�[�u�̎��
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
	 * @brief Fcurve �R���X�g���N�^
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
	 * @param f �t���[����
	 * @return ���ԁi�b�j
	 */
	static inline f32 FrameToTime( const s32 f ) {
		return ONE_FRAME_TIME * (f32) f;
	}

	/**
	 * @brief FrameToTime
	 * @param f �t���[����
	 * @return ���ԁi�b�j
	 */
	static inline f32 FrameToTime( const f32 f ) {
		return ONE_FRAME_TIME * f;
	}

	/**
	 * @brief TimeToFrame
	 * @param t ���ԁi�b�j
	 * @return �t���[����
	 */
	static inline f32 TimeToFrame( const f32 t ) {
		return ONE_FRAME * t;
	}

	/**
	 * @brief Initialize ������
	 * @param adr�@���[�V�����f�[�^�|�C���^
	 * @param max_size�@Fcurve���@�i����Ȃ��H�j
	 * @param heap�@�q�[�v�A�h���X
	 * @return �^�@�����@�U�@���s
	 */
	bool Initialize( const void* adr, const s32 max_size, gfl::heap::HeapBase* heap );
	/**
	 * @brief GetValueBegin �O�t���[���̒l���v�Z
	 * @param value�@�o�͒l�|�C���^
	 */
	void GetValueBegin( f32 *value );
	/**
	 * @brief GetValueEnd �Ō�̃t���[���̒l���v�Z
	 * @param value�@�o�͒l�|�C���^
	 * @param end�@�ǂ� Fcurve �܂Ōv�Z���邩�@�O�Ȃ�Ō�܂�
	 */
	void GetValueEnd( f32 *value, s32 end = 0 );

	/**
	 * @brief QueryEnd ���[�V�������I��������H
	 * @return �^�@�I������@�U�@�I����Ă��Ȃ�
	 */
	bool QueryEnd( void ) {
		return GetFrameCount( ) <= GetFrame( ); // �Ō�̃t���[�����܂܂Ȃ�
		//			return _OldFrame <= GetFrameCount( ) && GetFrameCount( ) < GetFrame( ); // �Ō�̃t���[�����܂�
	}
	/**
	 * @brief QueryEndBefore
	 * @return�@�^�@�I������@�U�@�I����Ă��Ȃ�
	 * @remark ��������Ȃ�����
	 */
	bool QueryEndBefore( void ) {
		return GetFrameCount( )-1 <= GetFrame( ); // �Ō�̃t���[�����܂܂Ȃ�
	}
#ifdef FC_KEY_FLOAT
	bool GetValueByFrame( f32 *value, const f32 frame, s32 end = 0 );
#else
	template<typename T>
	bool GetValueByFrame_Type( f32 *value, const f32 frame, s32 end = 0 );

	/**
	 * @brief GetValueByFrame
	 * @param value �ݒ肵�����l
	 * @param frame�@���̓t���[��
	 * @param end�@���Ԗڂ̃W���C���g�܂Ōv�Z���邩�B�w�肳��Ȃ���΍Ō�܂ŁB
	 * @return �^�@�v�Z�����@�U�@�v�Z���s
	 */
	bool GetValueByFrame( f32 *value, const f32 frame, s32 end = 0 ) {
		return ( _FrameCount < 256.0f) ?
			GetValueByFrame_Type<u8 > (value, frame, end) :
			GetValueByFrame_Type<u16 > (value, frame, end);
	}
#endif

	/**
	 * @brief GetValueByDeltaFrame �f���^�t���[������l���v�Z
	 * @param value �o�͒l�|�C���^
	 * @param f �f���^�t���[��
	 * @param end�@�ǂ� Fcurve �܂Ōv�Z���邩�@�O�Ȃ�Ō�܂�
	 * @return
	 */
	inline bool GetValueByDeltaFrame( f32 *value, const f32 f, s32 end = 0 ) {
		return GetValueByFrame( value, _Frame + f, end );
	}

	/**
	 * @brief GetFcCount �t�@���N�V�����J�[�u�̐����擾
	 * @return �t�@���N�V�����J�[�u��
	 */
	s32 GetFcCount( void ) const {
		return _FcCount;
	}

	/**
	 * @brief GetFrameCount ���t���[�������擾
	 * @return�@���t���[����
	 */
	f32 GetFrameCount( void ) const {
		return _FrameCount;
	}

	/**
	 * @brief GetFrame ���݂̃t���[�����擾
	 * @return�@�t���[��
	 */
	f32 GetFrame( void ) const {
		return _Frame;
	}

	/**
	 * @brief SetFrame �t���[����ݒ�
	 * @param frame�@�t���[��
	 */
	void SetFrame( const f32 frame ) {
		_OldFrame = _Frame;
		_Frame = frame;
	}

	/**
	 *   �Đ��X�s�[�h���擾
	 * @param  �Ȃ�
	 * @return �Đ��X�s�[�h
	 */

	f32 GetFrameSpeed( void ) const {
		return _FrameSpeed;
	}

	/**
	 *   �Đ��X�s�[�h��ݒ�
	 * @param  spd �Đ��X�s�[�h
	 * @return �Ȃ�
	 */

	void SetFrameSpeed( const f32 spd ) {
		_FrameSpeed = spd;
	}

	/**
	 * @brief ClearKey �ۑ����Ă���O�t���[���l���N���A����
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
	 * @brief GetOldFrame ��O�Ɍv�Z�����t���[�����擾
	 * @return ��O�Ɍv�Z�����t���[��
	 */
	f32 GetOldFrame( ) const {
		return _OldFrame;
	}

	/**
	 * @brief CheckLocatorYPosExist ���P�[�^�[Y�����݂��Ă��邩�H
	 * @return �^�@���݂��Ă���@�U�@���݂��Ă��Ȃ�
	 */
	s32 CheckLocatorYPosExist( ) const {
		return _pFck[1]._Kind;
	}
};

}
}

#endif // GFL_FCCALC_H

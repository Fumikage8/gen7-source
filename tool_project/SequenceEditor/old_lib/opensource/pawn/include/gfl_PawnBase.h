//==============================================================================
/**
 * @file	gfl_PawnBase.h
 * 	Pawn �̃��b�p�[�N���X
 * @author	kimura_shin
 * @data	2011/02/02, 12:18
 */
// =============================================================================

#ifndef GFL_PAWNBASE_H
#define	GFL_PAWNBASE_H

#include <gfl_Amx.h>

namespace gfl {
namespace pawn {

/**
 * �G���f�B�A���X���b�v�֐�  �Q�o�C�g�P�U�r�b�g��
 * @param a �X���b�v����ϐ��܂��̓N���X
 * @return �G���f�B�A���X���b�v���ꂽ�l
 */
static inline u16 EndianSwap16( u16 a ) {
	return ((a & 0xff) << 8) | ((a & 0xff00) >> 8);
}

/**
 * �G���f�B�A���X���b�v�֐�  �S�o�C�g�R�Q�r�b�g��
 * @param a �X���b�v����ϐ��܂��̓N���X
 * @return �G���f�B�A���X���b�v���ꂽ�l
 */
static inline u32 EndianSwap32( u32 a ) {
	return ((a & 0xff) << 24) | ((a & 0xff00) << 8) | ((a & 0xff0000) >> 8) | ((a & 0xff000000) >> 24);
}

/**
 * PawnBase Pawn���b�p�[���N���X�@�f�o�b�O�����܂߂Ȃ�
 */
class PawnBase {
private:
	AMX m_Amx; // pawn ����
	const void* m_pBuffer; // �X�}�|�ɂ�������?
	u32 m_BufferSize;

protected:
	gfl::heap::HeapBase* m_pHeap;
	s32 m_SuspendFrame;
	s16 m_ExecuteReturnValue;
	u16 m_HaltFlag; // 1bit is enough
	//	cell m_PawnReturnValue;

	PawnBase( gfl::heap::HeapBase* heap );
	virtual ~PawnBase( void );

public:
	// Pawn �������ł��郆�[�U�[�f�[�^�|�C���^

	enum {
		GFL_AMX_THIS,
		GFL_AMX_MAX_STACK,
		GFL_AMX_MAX_HEAP,
		GFL_AMX_END // AMX_USERNUM �𒴂��Ă͂����Ȃ��B���݂͂S�܂ŁB
	};

	/**
	 * �������@�������A���P�[�^�쐬
	 * @param size
	 */
	//	static void Initialize( const size_t size );

	/**
	 * ���[�h
	 * @param buffer�@Pawn�X�N���v�g�f�[�^�|�C���^
	 * @param size�@�f�[�^�T�C�Y
	 */
	virtual void Load( const void* buffer, const size_t size, const c8 * const amxname = "" );
	virtual void LoadShare( const PawnBase* org );

	/**
	 * Pawn �֐��̃C���f�b�N�X���֐�������擾
	 * @param com �֐���
	 * @return �C���f�b�N�X ����������@?�P
	 */
	s32 GetIndex( const c8 * const com );

	/**
	 * �X�^�b�N�ɐ�����ς�
	 * @param val ����
	 */
	inline void PushInteger( s32 val ) {
		amx_Push( GetAmx( ), val );
	}

	/**
	 * �X�^�b�N�ɔz���ς�
	 * @param val�@�z��
	 * @param cnt�@�z��T�C�Y
	 * @return �m�ۂ��ꂽ�������|�C���^
	 */
	template<typename T>
	inline cell* PushArray( T val[], s32 cnt ) {
		cell* adr;
		amx_Push( GetAmx( ), cnt ); // Pawn�Ŏ󂯎��ꍇ�͈����̏��Ԃ��t�ɂȂ�I
		amx_PushArray( GetAmx( ), 0, &adr, (cell*) val, cnt );
		return adr;
	}

	/**
	 * �z����v�b�V���������ɂ͊J�����K�v
	 * @param val
	 */
	inline void ReleaseArray( cell* adr ) {
		amx_Release( GetAmx( ), (cell) adr );
	}

	/**
	 * �X�^�b�N�ɕ���������ς�
	 * @param val ��������
	 */
	inline void PushFloat( f32 val ) {
		amx_Push( GetAmx( ), amx_ftoc( val ) );
	}

	/**
	 * �X�^�b�N�ɕ������ς�
	 * @param str ������
	 */
	inline void PushString( c8* str ) {
		amx_PushString( GetAmx( ), 0, 0, str, 0, 0 );
	}

	/**
	 *   C �̊֐����e�[�u���ɃZ�b�g����}�N��
	 * @param  func �֐�
	 * @remark �����񖼁A�֐��ɓW�J����
	 */
#define	GFL_PAWN_FUNC(func) { # func, func },

	/**
	 *   C �̊֐����e�[�u���ɃZ�b�g����}�N��
	 * @param  pre �֐��̓��ɂ��镶����
	 * @param  func �֐�
	 * @remark �����񖼁A�֐��ɓW�J����B���C�u�����[���������Ƃ��ɕ֗�
	 */
#define	GFL_PAWN_FUNC_PREFIX(pre,func) { # func, pre ## func },

	/**
	 *   �֐��e�[�u���̏I�[
	 */
#define	GFL_PAWN_FUNC_END { 0, 0 }

	/**
	 *   C �̊֐��e�[�u���錾
	 * @remark ��jstatic const GFL_PAWN_FUNC_TABLE sFuncTable[] = {\n
	 *		GFL_PAWN_FUNC(Function)\n
	 *		GFL_PAWN_FUNC_END\n
	 *	};\n
	 */
#define GFL_PAWN_FUNC_TABLE AMX_NATIVE_INFO

	/**
	 *   C �̊֐��� Pawn �ɓo�^����B
	 * @param  native_func_tbl C �̊֐��̃e�[�u��
	 * @return ����
	 */

	inline void RegisterPawnFunction( const GFL_PAWN_FUNC_TABLE native_func_tbl[] ) {
		amx_Register( GetAmx( ), native_func_tbl, -1 );
	}
	/**
	 * Pawn �̃p�u���b�N�ϐ��̃|�C���^���擾
	 * @param str�@�ϐ���
	 * @return �ϐ��|�C���^
	 */
	cell* GetGlobalParameterPointer( const c8 * const str );

	inline s32 GetGlobalParameterInteger( const c8 * const str, const s32 def = 0 ) {
		cell* ptr = GetGlobalParameterPointer( str );
		return ptr ? * reinterpret_cast<s32*> (ptr) : def;
	}

	inline f32 GetGlobalParameterFloat( const c8 * const str, const f32 def = 0.0f ) {
		cell* ptr = GetGlobalParameterPointer( str );
		return ptr ? amx_ctof( *ptr ) : def;
	}

	inline void SetGlobalParameterInteger( const c8 * const str, s32 val ) {
		cell* ptr = GetGlobalParameterPointer( str );
		if( ptr ){
			*reinterpret_cast<s32*> (ptr) = val;
		}
	}

	inline void SetGlobalParameterFloat( const c8 * const str, f32 val ) {
		cell* ptr = GetGlobalParameterPointer( str );
		if( ptr ){
			*reinterpret_cast<f32*> (ptr) = val;
		}
	}

	/**
	 * �C���f�b�N�X�ԍ��̊֐������s
	 * @param index�@�֐��C���f�b�N�X
	 * @return Pawn����̕Ԃ�l
	 */
	cell CallPawnFunction( const s32 index );

	/**
	 *   �֐����ło�������֐����R�[������
	 * @param  name �֐���
	 * @return �o�������֐�����̕Ԃ�l
	 */

	inline cell CallPawnFunction( const c8 * const name ) {
		const s32 idx = GetIndex( name );
		if( 0 <= idx ){
			return CallPawnFunction( idx );
		}
		return -1;
	}

	/**
	 * �֐��𖼑O�Ŏ��s
	 * @param name�@�֐���
	 * @return Pawn����̕Ԃ�l
	 */
	//	cell Execute( const c8 * const name, ... );

	/**
	 * ���C���֐������s
	 * @return Pawn����̕Ԃ�l
	 */
	virtual s32 Execute( void );
	s32 ExecuteStep( void );

	/**
	 * amx ���擾
	 * @return amx �|�C���^
	 */
	AMX* GetAmx( ) {
		return &m_Amx;
	}

	const AMX* GetAmx( ) const {
		return &m_Amx;
	}

	/**
	 *  ���[�U�[��`�����N���X�|�C���^
	 * @param amx AMX ����N���X���擾
	 * @return �N���X�|�C���^
	 */

	template<typename T>
	static T GetClass( AMX* amx ) {
		return static_cast<T> (amx->userdata[GFL_AMX_THIS]);
	}

	void* IsAvailable( void ) {
		return GetAmx( )->userdata[GFL_AMX_THIS];
	}

	void InitializeAmx( void );
	void ClearAmx( void );

	const u8* GetBuffer( ) const {
		return (u8*) m_pBuffer;
	}

	u32 GetBufferSize( void ) const {
		return m_BufferSize;
	}

	void SetHeap( gfl::heap::HeapBase* Heap ) {
		this->m_pHeap = Heap;
	}

	// Update() �̕Ԃ�l

	enum {
		GFL_PAWN_NOT_AVAILABLE = -3,
		GFL_PAWN_DEBUGGING,
		GFL_PAWN_SUSPENDING,
		GFL_PAWN_FINISHED = AMX_ERR_NONE,
		GFL_PAWN_EXECUTE = AMX_ERR_SLEEP,
	};

	// �t���[����i�߂Ă����̂��H

	inline bool IsExecuted( s32 ret ) {
		return GFL_PAWN_SUSPENDING < ret;
	}

	inline bool IsFinished( s32 ret ) {
		return ret == GFL_PAWN_FINISHED;
	}

/**
 * �f�o�b�O�����܂�AMX�Ȃ̂��H
 * @param buffer�@AMX�o�b�t�@
 * @return �^�@�f�o�b�O�����܂�AMX
 */	
	inline bool IsDebug( const void* buffer){
		return ( reinterpret_cast < const AMX_HEADER* >( buffer )->flags & AMX_FLAG_DEBUG  );
	}
	/**
	 *   �X�V
	 * @param  ����
	 * @return�@GFL_PAWN_*
	 */

	s32 Update( void );

	/**
	 * �X���[�v��Ԃ���������Main�̏��߂Ƀv���O�����|�C���^��߂�
	 */
	void ClearSleepAndRestart( void ) {
		AMX* amx = GetAmx( );
		if( amx->base ){
			amx->hea=amx->hlw; // �q�[�v�����Z�b�g
			amx->stk=amx->stp; // �X�^�b�N�����Z�b�g
		}
		m_SuspendFrame = 0;
		m_ExecuteReturnValue = AMX_ERR_NONE;
	}

	inline void SetSuspendFrame( s32 SuspendFrame ) {
		this->m_SuspendFrame = SuspendFrame;
	}

	inline bool IsFinished( void ) {
		return m_ExecuteReturnValue == GFL_PAWN_FINISHED;
	}

	gfl::heap::HeapBase* GetHeap( ) const {
		return m_pHeap;
	}

	/**
	 * �X�N���v�g�̋������f
	 */
	void Halt( void ) {
		m_HaltFlag = true;
		// ���f������
		amx_RaiseError( GetAmx( ), AMX_ERR_SLEEP );
	}

	/**
	 * �������f���ꂽ�X�N���v�g���ĊJ
	 */
	void Resume( void ) {
		m_HaltFlag = false;
	}

	/**
	 * �������f�����H
	 * @return �^�@���f���@�U�@���f���łȂ�
	 */
	bool IsHalting( ) const {
		return m_HaltFlag;
	}

#if GFL_PAWN_DEBUG
	static const c8 * const s_cErrorMessage[];
#define PAWN_ASSERT(err,mes) GFL_ASSERT_MSG( (err == AMX_ERR_NONE || err == AMX_ERR_SLEEP), "%s : %s\n", mes, gfl::pawn::PawnBase::s_cErrorMessage[err] )
#else
#define PAWN_ASSERT(err,mes)
#endif
};

}
}

#endif	/* GFL_PAWNBASE_H */


/* 
 * File:   gfl_SmartPointer.h
 * Author: s-kimura
 *
 * Created on 2010/06/04, 15:06
 */

#ifndef GFL_SMARTPOINTER_H
#define	GFL_SMARTPOINTER_H
#pragma once

namespace gfl {
namespace core {

// =============================================================================
// ���̃t�@�C���Ɋ܂܂��N���X�Q
// =============================================================================
class AtomicInteger; // dummy ...
class BaseCount;
template<typename T> class CountClass;
class WeakCount;
class SharedCount;
template<typename T> class SharedPointer;
template<typename T> class WeakPointer;
class ByteArray;

// =============================================================================
// AtomicInteger
// =============================================================================

class AtomicInteger {
public:

	explicit AtomicInteger( s32 val = 0 ) {
		SetValue( val );
	}
#ifdef GFL_PLATFORM_3DS

	s32 GetValue( void ) {
		m_Mutex.Lock( );
		s32 val = m_Value;
		m_Mutex.UnLock( );
		return val;
	}

	s32 SetValue( s32 val ) {
		m_Mutex.Lock( );
		m_Value = val;
		m_Mutex.UnLock( );
		return val;
	}

	s32 Increment( void ) {
		m_Mutex.Lock( );
		s32 val = m_Value + 1;
		m_Value = val;
		m_Mutex.UnLock( );
		return val;
	}

	s32 Decrement( void ) {
		m_Mutex.Lock( );
		s32 val = m_Value - 1;
		m_Value = val;
		m_Mutex.UnLock( );
		return val;
	}

	s32 Add( s32 val ) {
		m_Mutex.Lock( );
		val += m_Value;
		m_Value = val;
		m_Mutex.UnLock( );
		return val;
	}

#elif defined(GFL_PLATFORM_MICROSOFT)

	s32 GetValue( void ) const {
		return InterlockedExchangeAdd( (LPLONG) & m_Value, 0 );
	}

	s32 SetValue( s32 val ) {
		return InterlockedExchange( (LPLONG) & m_Value, val );
	}

	s32 Increment( void ) {
		return InterlockedIncrement( (LPLONG) & m_Value );
	}

	s32 Decrement( void ) {
		return InterlockedDecrement( (LPLONG) & m_Value );
	}

	s32 Add( s32 val ) {
		return InterlockedExchangeAdd( (LPLONG) & m_Value, val ) + val;
	}

#endif

	inline operator int() {
		return GetValue( );
	}

	inline s32 operator=(s32 val) {
		return SetValue( val );
	}

	inline bool operator==(s32 val) {
		return GetValue( ) == val;
	}

	inline bool operator!=(s32 val) {
		return GetValue( ) != val;
	}

	inline s32 operator+=(s32 val) {
		return Add( val );
	}

	inline s32 operator-=(s32 val) {
		return Add( -val );
	}

	inline s32 operator++(void) {
		return Increment( );
	}

	inline s32 operator++(int) {
		return Increment( ) - 1;
	}

	inline s32 operator--(void) {
		return Decrement( );
	}

	inline s32 operator--(int) {
		return Decrement( ) + 1;
	}

private:
	volatile s32 m_Value;
#ifdef GFL_PLATFORM_3DS
	Mutex m_Mutex;
#endif
};

/**
 *  SharedPointer �̃����o�[�ϐ��ɂ��邽�߂̊��N���X
 */

class BaseCount {
private:
	// ���t�@�����X�J�E���^�[
	AtomicInteger m_RefCount;
	// �E�B�[�N���t�@�����X�J�E���^�[
	AtomicInteger m_WeakRefCount;

public:

	/**
	 *   �f�t�H���g�R���X�g���N�^
	 * @param  ����
	 * @return ����
	 * @remark ���t�@�����X�J�E���^�̏����l�͂P
	 */

	BaseCount( void ) : m_RefCount( 1 ), m_WeakRefCount( 1 ) {
	}

	/**
	 *   �f�X�X�g���N�^
	 * @param  ����
	 * @return ����
	 * @remark ���z�֐�������̂ŉ��z�f�X�g���N�^
	 */

	virtual ~BaseCount( void ) {
	}

	/**
	 *   �f���[�g�֐�
	 * @param  ����
	 * @return ����
	 * @remark �������z�֐��@�N���X���̂��������B�K�؂ȃf�X�g���N�^���Ă΂��
	 */
	virtual void Destroy( void ) = 0;

	/**
	 *   ���t�@�����X�J�E���^�����炵�A�O�ɂȂ�����f���[�g����
	 * @param  ����
	 * @return ����
	 */

	void Release( void ) {
		if( DecRefCount( ) ){
			Destroy( );
			WeakRelease( );
		}
	}

	/**
	 *   �E�B�[�N�|�C���^�̌��Z
	 * @param  ����
	 * @return ����
	 */

	void WeakRelease( void ) {
		if( m_WeakRefCount.Decrement( ) <= 0 ){
			// �����f���[�g
			delete this;
		}
	}

	/**
	 *   ���t�@�����X�J�E���^�𑝂₷
	 * @param  ����
	 * @return ����
	 */

	inline void IncRefCount( void ) {
		m_RefCount.Increment( );
	}

	/**
	 *   �E�B�[�N���t�@�����X�J�E���^�𑝂₷
	 * @param  ����
	 * @return ����
	 */

	inline void IncWeakRefCount( void ) {
		m_WeakRefCount.Increment( );
	}

	/**
	 *   ���t�@�����X�J�E���^�����炷
	 * @param  ����
	 * @return �^�@���t�@�����X�J�E���^���O�ȉ��ɂȂ����@�U�@����ȊO
	 */

	inline bool DecRefCount( void ) {
		return (m_RefCount.Decrement( ) <= 0);
	}

	/**
	 *   ���t�@�����X�J�E���^���擾
	 * @param  ����
	 * @return ���t�@�����X�J�E���^
	 */

	inline s32 GetRefCount( void ) {
		return m_RefCount;
	}
};

/**
 *   ���ۂ̃N���X�|�C���^������N���X
 * @remark �K�؂ȃf�X�g���N�^���ĂԂ���
 */

template<typename T>
class CountClass : public BaseCount {
private:
	// ���ۂ̌^�̃N���X
	T* m_pClass;
public:

	/**
	 *   �e���v���[�g�R���X�g���N�^
	 * @param  cls �N���X�|�C���^
	 * @return ����
	 */

	explicit CountClass( T* cls ) {
		m_pClass = cls;
	}

	/**
	 *   �f�X�X�g���N�^
	 * @param  ����
	 * @return ����
	 * @remark ���z�֐�������̂ŉ��z�f�X�g���N�^
	 */

	virtual ~CountClass( void ) {
	}

	/**
	 *   �f���[�g�֐�
	 * @param  ����
	 * @return ����
	 */

	virtual void Destroy( void ) {
		GFL_DELETE m_pClass;
		m_pClass = 0;
	}
};

/**
 *  �V�F�A�[�h�J�E���g�N���X
 */

class SharedCount {
private:
	// ������ CountClass<T> ������
	BaseCount* m_pCount;

public:

	/**
	 *   �f�t�H���g�R���X�g���N�^
	 * @param  ����
	 * @return ����
	 */

	SharedCount( void ) : m_pCount( 0 ) {
	}

	/**
	 *   �e���v���[�g�R���X�g���N�^
	 * @param  cls �N���X�|�C���^
	 * @return ����
	 */

	template<typename S>
	explicit SharedCount( S* cls ) {
		m_pCount = new CountClass<S > (cls);
	}

	/**
	 *   ���t�@�����X�R���X�g���N�^
	 * @param  ref ���t�@�����X
	 * @return ����
	 * @remark �Öق̕ϊ��ŌĂяo�����
	 */

	explicit SharedCount( const SharedCount& ref ) : m_pCount( ref.m_pCount ) {
		if( m_pCount ){
			m_pCount->IncRefCount( );
		}
	}

	/**
	 *   ���t�@�����X�R���X�g���N�^
	 * @param  ref ���t�@�����X
	 * @return ����
	 * @remark �Öق̕ϊ��ŌĂяo�����
	 */
	explicit SharedCount( const WeakCount& ref );

	/**
	 *   �f�X�X�g���N�^
	 * @param  ����
	 * @return ����
	 */

	~SharedCount( void ) {
		DecRefCount( );
	}

	/**
	 *   ���t�@�����X�J�E���^�����炷
	 * @param  ����
	 * @return ����
	 */

	void DecRefCount( void ) {
		if( m_pCount ){
			m_pCount->Release( );
		}
	}

	/**
	 *   �I�y���[�^�@�R�s�[
	 * @param  ref ���t�@�����X
	 * @return �R�s�[���ꂽ�J�E���^�̎Q��
	 */
	SharedCount& operator=(const SharedCount& ref);

	friend class WeakCount;
};

/**
 *  �V�F�A�[�h�|�C���^
 * @remark boost::shared_Pointer �Ɠ��������i�̂͂��j
 */

template<typename T>
class SharedPointer {
private:
	T* m_pClass;
	SharedCount m_Count;

public:

	/**
	 *   �f�t�H���g�R���X�g���N�^
	 * @param  ����
	 * @return ����
	 */

	SharedPointer( void ) : m_pClass( 0 ) {
	}

	/**
	 *   �e���v���[�g�R���X�g���N�^
	 * @param  cls �N���X�|�C���^
	 * @return ����
	 */

	template<typename S>
	explicit SharedPointer( S* cls ) : m_pClass( cls ), m_Count( cls ) {
	}

	/**
	 *   ���t�@�����X�R���X�g���N�^
	 * @param  ref ���t�@�����X
	 * @return ����
	 * @remark �Öق̕ϊ��ŌĂяo�����
	 */

	template<typename S>
	SharedPointer( const SharedPointer<S>& ref ) : m_pClass( ref.m_pClass ), m_Count( ref.m_Count ) {
	}

	/**
	 *   ���t�@�����X�R���X�g���N�^
	 * @param  ref ���t�@�����X
	 * @return ����
	 * @remark �Öق̕ϊ��ŌĂяo�����
	 */

	template<typename S>
	SharedPointer( const WeakPointer<S>& ref ) : m_pClass( ref.m_pClass ), m_Count( ref.m_Count ) {
	}

	/**
	 *   �f�X�X�g���N�^
	 * @param  ����
	 * @return ����
	 */

	~SharedPointer( void ) {
	}

	/**
	 *   �R�s�[
	 * @param  ref �V�F�A�[�h�|�C���^�Q��
	 * @return ����
	 */

	SharedPointer& operator=(const SharedPointer<T>& ref) {
		m_pClass = ref.m_pClass;
		m_Count = ref.m_Count;
		return *this;
	}

	/**
	 *   �R�s�[
	 * @param  ref �V�F�A�[�h�|�C���^�Q��
	 * @return ����
	 */

	template<class S>
	SharedPointer& operator=(const SharedPointer<S>& ref) {
		m_pClass = ref.m_pClass;
		m_Count = ref.m_Count;
		return *this;
	}

	/**
	 *   �N���X�|�C���^�̎擾
	 * @param  ����
	 * @return �N���X�|�C���^
	 */

	inline T* operator->(void) {
		return m_pClass;
	}

	/**
	 *   �N���X�|�C���^�̎擾
	 * @param  ����
	 * @return �N���X�|�C���^
	 */

	inline const T * operator->(void) const {
		return m_pClass;
	}

	/**
	 *   �N���X�|�C���^�̎擾
	 * @param  ����
	 * @return �N���X�|�C���^
	 */

	inline T* get( void ) const {
		return m_pClass;
	}

	/**
	 *   �N���X�Q�Ƃ̎擾
	 * @param  ����
	 * @return �N���X�Q��
	 */

	inline T& operator*(void) const {
		return *m_pClass;
	}

	/**
	 *   �I�y���[�^�[�@��r�֐�
	 * @param  ����
	 * @return �^�U
	 */
	typedef T* SharedPointer<T>::*Check;

	operator Check( void ) const {
		return (m_pClass) ? &SharedPointer<T>::m_pClass : 0;
	}

	/**
	 *   ���b�N
	 * @param  ����
	 * @return �K���@�^
	 * @remark WeakPointer �ƃ\�[�X�����L�����邽�߂̋���̍�
	 */

	bool lock( void ) {
		return m_pClass != 0;
	}

	// ���ꂪ�Ȃ��Ƒ��̃N���X�����ł��Ȃ�
	template<typename S> friend class SharedPointer;
	template<typename S> friend class WeakPointer;
};

/**
 *  �E�B�[�N�J�E���g�N���X
 */

class WeakCount {
private:
	// ������ CountClass<T> ������
	BaseCount* m_pCount;

public:

	/**
	 *   �f�t�H���g�R���X�g���N�^
	 * @param  ����
	 * @return ����
	 */

	WeakCount( void ) : m_pCount( 0 ) {
	}

	/**
	 *   ���t�@�����X�R���X�g���N�^
	 * @param  ref ���t�@�����X
	 * @return ����
	 * @remark �Öق̕ϊ��ŌĂяo�����
	 */

	WeakCount( const WeakCount& ref ) : m_pCount( ref.m_pCount ) {
		if( m_pCount ){
			m_pCount->IncRefCount( );
		}
	}

	/**
	 *   ���t�@�����X�R���X�g���N�^
	 * @param  ref ���t�@�����X
	 * @return ����
	 * @remark �Öق̕ϊ��ŌĂяo�����
	 */

	WeakCount( const SharedCount& ref ) : m_pCount( ref.m_pCount ) {
		if( m_pCount ){
			m_pCount->IncRefCount( );
		}
	}

	/**
	 *   �f�X�X�g���N�^
	 * @param  ����
	 * @return ����
	 */

	~WeakCount( void ) {
		DecRefCount( );
	}

	/**
	 *   ���t�@�����X�J�E���^�����炷
	 * @param  ����
	 * @return ����
	 */

	void DecRefCount( void ) {
		if( m_pCount ){
			m_pCount->WeakRelease( );
		}
	}

	/**
	 *   �I�y���[�^�@�R�s�[
	 * @param  ref ���t�@�����X
	 * @return �R�s�[���ꂽ�J�E���^�̎Q��
	 */
	WeakCount& operator=(const SharedCount& ref);

	/**
	 *   �I�y���[�^�@�R�s�[
	 * @param  ref ���t�@�����X
	 * @return �R�s�[���ꂽ�J�E���^�̎Q��
	 */
	WeakCount& operator=(const WeakCount& ref);

	/**
	 *   ���t�@�����X�J�E���^�̎擾
	 * @param  ����
	 * @return ���t�@�����X�J�E���^
	 */

	inline s32 GetCounter( void ) const {
		return (m_pCount) ? m_pCount->GetRefCount( ) : 0;
	}

	friend class SharedCount;
};

/**
 *  �E�B�[�N�|�C���^
 * @remark boost::weak_Pointer �Ɠ��������i�̂͂��j
 */

template<typename T>
class WeakPointer {
private:
	T* m_pClass;
	WeakCount m_Count;

public:

	/**
	 *   �f�t�H���g�R���X�g���N�^
	 * @param  ����
	 * @return ����
	 */

	WeakPointer( void ) : m_pClass( 0 ) {
	}

	/**
	 *   ���t�@�����X�R���X�g���N�^
	 * @param  ref ���t�@�����X
	 * @return ����
	 * @remark �Öق̕ϊ��ŌĂяo�����
	 */

	template<typename S>
	WeakPointer( const WeakPointer<S>& ref ) : m_Count( ref.m_Count ) {
		m_pClass = ref.lock( ).m_pClass;
	}

	/**
	 *   ���t�@�����X�R���X�g���N�^
	 * @param  ref ���t�@�����X
	 * @return ����
	 * @remark �Öق̕ϊ��ŌĂяo�����
	 */

	template<typename S>
	WeakPointer( const SharedPointer<S>& ref ) : m_pClass( ref.m_pClass ), m_Count( ref.m_Count ) {
	}

	/**
	 *   �f�X�X�g���N�^
	 * @param  ����
	 * @return ����
	 */

	~WeakPointer( void ) {
	}

	/**
	 *   �R�s�[
	 * @param  ref �E�B�[�N�|�C���^�Q��
	 * @return ����
	 */

	template<class S>
	WeakPointer& operator=(const WeakPointer<S>& ref) {
		m_pClass = ref.lock( ).m_pClass; // �������Ⴄ�I
		m_Count = ref.m_Count;
		return *this;
	}

	/**
	 *   �R�s�[
	 * @param  ref �V�F�A�[�h�|�C���^�Q��
	 * @return ����
	 */

	template<class S>
	WeakPointer& operator=(const SharedPointer<S>& ref) {
		m_pClass = ref.m_pClass; // �������Ⴄ�I
		m_Count = ref.m_Count;
		return *this;
	}

	/**
	 *   lock
	 * @param  ����
	 * @return �V�F�A�[�h�|�C���^�̎��̂�Ԃ�
	 * @remark �N���X�����݂��Ă���̂��m�F���邽�߂̖���
	 */

	inline SharedPointer<T> lock( void ) const {
		if( m_Count.GetCounter( ) ){
			return SharedPointer<T > (*this);
		} else {
			return SharedPointer<T > ();
		}
	}

	/**
	 *   get
	 * @param  ����
	 * @return �N���X�|�C���^��Ԃ�
	 * @remark ���������̂��߂̔������B�G���W���ȊO�ł͎g�p�s�B
	 */

	inline T* get( void ) const {
		return m_pClass;
	}

	template<typename S> friend class SharedPointer;
	template<typename S> friend class WeakPointer;
};

class ByteArray {
public:

	ByteArray( u8* data = 0, size_t size = 0 ) : m_pData( data ), m_Size( size ) {
	}

	~ByteArray( ) {
		if(m_pData){
			GFL_DELETE_ARRAY m_pData;
			m_pData = 0;
		}
	}

	void SetSize( size_t Size ) {
		this->m_Size = Size;
	}

	size_t GetSize( ) const {
		return m_Size;
	}

	void SetData( u8* Data ) {
		this->m_pData = Data;
	}

	u8* GetData( ) const {
		return m_pData;
	}

private:
	u8* m_pData;
	size_t m_Size;
};

typedef SharedPointer<ByteArray> ByteArrayPointer;

}
}

#endif // GFL_SMARTPOINTER_H

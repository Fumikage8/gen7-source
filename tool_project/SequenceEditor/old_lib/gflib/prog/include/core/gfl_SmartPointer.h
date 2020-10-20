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
// このファイルに含まれるクラス群
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
 *  SharedPointer のメンバー変数にするための基底クラス
 */

class BaseCount {
private:
	// リファレンスカウンター
	AtomicInteger m_RefCount;
	// ウィークリファレンスカウンター
	AtomicInteger m_WeakRefCount;

public:

	/**
	 *   デフォルトコンストラクタ
	 * @param  無し
	 * @return 無し
	 * @remark リファレンスカウンタの初期値は１
	 */

	BaseCount( void ) : m_RefCount( 1 ), m_WeakRefCount( 1 ) {
	}

	/**
	 *   デスストラクタ
	 * @param  無し
	 * @return 無し
	 * @remark 仮想関数があるので仮想デストラクタ
	 */

	virtual ~BaseCount( void ) {
	}

	/**
	 *   デリート関数
	 * @param  無し
	 * @return 無し
	 * @remark 純粋仮想関数　クラス自体を解放する。適切なデストラクタが呼ばれる
	 */
	virtual void Destroy( void ) = 0;

	/**
	 *   リファレンスカウンタを減らし、０になったらデリートする
	 * @param  無し
	 * @return 無し
	 */

	void Release( void ) {
		if( DecRefCount( ) ){
			Destroy( );
			WeakRelease( );
		}
	}

	/**
	 *   ウィークポインタの減算
	 * @param  無し
	 * @return 無し
	 */

	void WeakRelease( void ) {
		if( m_WeakRefCount.Decrement( ) <= 0 ){
			// 強制デリート
			delete this;
		}
	}

	/**
	 *   リファレンスカウンタを増やす
	 * @param  無し
	 * @return 無し
	 */

	inline void IncRefCount( void ) {
		m_RefCount.Increment( );
	}

	/**
	 *   ウィークリファレンスカウンタを増やす
	 * @param  無し
	 * @return 無し
	 */

	inline void IncWeakRefCount( void ) {
		m_WeakRefCount.Increment( );
	}

	/**
	 *   リファレンスカウンタを減らす
	 * @param  無し
	 * @return 真　リファレンスカウンタが０以下になった　偽　それ以外
	 */

	inline bool DecRefCount( void ) {
		return (m_RefCount.Decrement( ) <= 0);
	}

	/**
	 *   リファレンスカウンタを取得
	 * @param  無し
	 * @return リファレンスカウンタ
	 */

	inline s32 GetRefCount( void ) {
		return m_RefCount;
	}
};

/**
 *   実際のクラスポインタが入るクラス
 * @remark 適切なデストラクタを呼ぶため
 */

template<typename T>
class CountClass : public BaseCount {
private:
	// 実際の型のクラス
	T* m_pClass;
public:

	/**
	 *   テンプレートコンストラクタ
	 * @param  cls クラスポインタ
	 * @return 無し
	 */

	explicit CountClass( T* cls ) {
		m_pClass = cls;
	}

	/**
	 *   デスストラクタ
	 * @param  無し
	 * @return 無し
	 * @remark 仮想関数があるので仮想デストラクタ
	 */

	virtual ~CountClass( void ) {
	}

	/**
	 *   デリート関数
	 * @param  無し
	 * @return 無し
	 */

	virtual void Destroy( void ) {
		GFL_DELETE m_pClass;
		m_pClass = 0;
	}
};

/**
 *  シェアードカウントクラス
 */

class SharedCount {
private:
	// ここに CountClass<T> を入れる
	BaseCount* m_pCount;

public:

	/**
	 *   デフォルトコンストラクタ
	 * @param  無し
	 * @return 無し
	 */

	SharedCount( void ) : m_pCount( 0 ) {
	}

	/**
	 *   テンプレートコンストラクタ
	 * @param  cls クラスポインタ
	 * @return 無し
	 */

	template<typename S>
	explicit SharedCount( S* cls ) {
		m_pCount = new CountClass<S > (cls);
	}

	/**
	 *   リファレンスコンストラクタ
	 * @param  ref リファレンス
	 * @return 無し
	 * @remark 暗黙の変換で呼び出される
	 */

	explicit SharedCount( const SharedCount& ref ) : m_pCount( ref.m_pCount ) {
		if( m_pCount ){
			m_pCount->IncRefCount( );
		}
	}

	/**
	 *   リファレンスコンストラクタ
	 * @param  ref リファレンス
	 * @return 無し
	 * @remark 暗黙の変換で呼び出される
	 */
	explicit SharedCount( const WeakCount& ref );

	/**
	 *   デスストラクタ
	 * @param  無し
	 * @return 無し
	 */

	~SharedCount( void ) {
		DecRefCount( );
	}

	/**
	 *   リファレンスカウンタを減らす
	 * @param  無し
	 * @return 無し
	 */

	void DecRefCount( void ) {
		if( m_pCount ){
			m_pCount->Release( );
		}
	}

	/**
	 *   オペレータ　コピー
	 * @param  ref リファレンス
	 * @return コピーされたカウンタの参照
	 */
	SharedCount& operator=(const SharedCount& ref);

	friend class WeakCount;
};

/**
 *  シェアードポインタ
 * @remark boost::shared_Pointer と同じ挙動（のはず）
 */

template<typename T>
class SharedPointer {
private:
	T* m_pClass;
	SharedCount m_Count;

public:

	/**
	 *   デフォルトコンストラクタ
	 * @param  無し
	 * @return 無し
	 */

	SharedPointer( void ) : m_pClass( 0 ) {
	}

	/**
	 *   テンプレートコンストラクタ
	 * @param  cls クラスポインタ
	 * @return 無し
	 */

	template<typename S>
	explicit SharedPointer( S* cls ) : m_pClass( cls ), m_Count( cls ) {
	}

	/**
	 *   リファレンスコンストラクタ
	 * @param  ref リファレンス
	 * @return 無し
	 * @remark 暗黙の変換で呼び出される
	 */

	template<typename S>
	SharedPointer( const SharedPointer<S>& ref ) : m_pClass( ref.m_pClass ), m_Count( ref.m_Count ) {
	}

	/**
	 *   リファレンスコンストラクタ
	 * @param  ref リファレンス
	 * @return 無し
	 * @remark 暗黙の変換で呼び出される
	 */

	template<typename S>
	SharedPointer( const WeakPointer<S>& ref ) : m_pClass( ref.m_pClass ), m_Count( ref.m_Count ) {
	}

	/**
	 *   デスストラクタ
	 * @param  無し
	 * @return 無し
	 */

	~SharedPointer( void ) {
	}

	/**
	 *   コピー
	 * @param  ref シェアードポインタ参照
	 * @return 無し
	 */

	SharedPointer& operator=(const SharedPointer<T>& ref) {
		m_pClass = ref.m_pClass;
		m_Count = ref.m_Count;
		return *this;
	}

	/**
	 *   コピー
	 * @param  ref シェアードポインタ参照
	 * @return 無し
	 */

	template<class S>
	SharedPointer& operator=(const SharedPointer<S>& ref) {
		m_pClass = ref.m_pClass;
		m_Count = ref.m_Count;
		return *this;
	}

	/**
	 *   クラスポインタの取得
	 * @param  無し
	 * @return クラスポインタ
	 */

	inline T* operator->(void) {
		return m_pClass;
	}

	/**
	 *   クラスポインタの取得
	 * @param  無し
	 * @return クラスポインタ
	 */

	inline const T * operator->(void) const {
		return m_pClass;
	}

	/**
	 *   クラスポインタの取得
	 * @param  無し
	 * @return クラスポインタ
	 */

	inline T* get( void ) const {
		return m_pClass;
	}

	/**
	 *   クラス参照の取得
	 * @param  無し
	 * @return クラス参照
	 */

	inline T& operator*(void) const {
		return *m_pClass;
	}

	/**
	 *   オペレーター　比較関数
	 * @param  無し
	 * @return 真偽
	 */
	typedef T* SharedPointer<T>::*Check;

	operator Check( void ) const {
		return (m_pClass) ? &SharedPointer<T>::m_pClass : 0;
	}

	/**
	 *   ロック
	 * @param  無し
	 * @return 必ず　真
	 * @remark WeakPointer とソースを共有させるための苦肉の策
	 */

	bool lock( void ) {
		return m_pClass != 0;
	}

	// これがないと他のクラスを代入できない
	template<typename S> friend class SharedPointer;
	template<typename S> friend class WeakPointer;
};

/**
 *  ウィークカウントクラス
 */

class WeakCount {
private:
	// ここに CountClass<T> を入れる
	BaseCount* m_pCount;

public:

	/**
	 *   デフォルトコンストラクタ
	 * @param  無し
	 * @return 無し
	 */

	WeakCount( void ) : m_pCount( 0 ) {
	}

	/**
	 *   リファレンスコンストラクタ
	 * @param  ref リファレンス
	 * @return 無し
	 * @remark 暗黙の変換で呼び出される
	 */

	WeakCount( const WeakCount& ref ) : m_pCount( ref.m_pCount ) {
		if( m_pCount ){
			m_pCount->IncRefCount( );
		}
	}

	/**
	 *   リファレンスコンストラクタ
	 * @param  ref リファレンス
	 * @return 無し
	 * @remark 暗黙の変換で呼び出される
	 */

	WeakCount( const SharedCount& ref ) : m_pCount( ref.m_pCount ) {
		if( m_pCount ){
			m_pCount->IncRefCount( );
		}
	}

	/**
	 *   デスストラクタ
	 * @param  無し
	 * @return 無し
	 */

	~WeakCount( void ) {
		DecRefCount( );
	}

	/**
	 *   リファレンスカウンタを減らす
	 * @param  無し
	 * @return 無し
	 */

	void DecRefCount( void ) {
		if( m_pCount ){
			m_pCount->WeakRelease( );
		}
	}

	/**
	 *   オペレータ　コピー
	 * @param  ref リファレンス
	 * @return コピーされたカウンタの参照
	 */
	WeakCount& operator=(const SharedCount& ref);

	/**
	 *   オペレータ　コピー
	 * @param  ref リファレンス
	 * @return コピーされたカウンタの参照
	 */
	WeakCount& operator=(const WeakCount& ref);

	/**
	 *   リファレンスカウンタの取得
	 * @param  無し
	 * @return リファレンスカウンタ
	 */

	inline s32 GetCounter( void ) const {
		return (m_pCount) ? m_pCount->GetRefCount( ) : 0;
	}

	friend class SharedCount;
};

/**
 *  ウィークポインタ
 * @remark boost::weak_Pointer と同じ挙動（のはず）
 */

template<typename T>
class WeakPointer {
private:
	T* m_pClass;
	WeakCount m_Count;

public:

	/**
	 *   デフォルトコンストラクタ
	 * @param  無し
	 * @return 無し
	 */

	WeakPointer( void ) : m_pClass( 0 ) {
	}

	/**
	 *   リファレンスコンストラクタ
	 * @param  ref リファレンス
	 * @return 無し
	 * @remark 暗黙の変換で呼び出される
	 */

	template<typename S>
	WeakPointer( const WeakPointer<S>& ref ) : m_Count( ref.m_Count ) {
		m_pClass = ref.lock( ).m_pClass;
	}

	/**
	 *   リファレンスコンストラクタ
	 * @param  ref リファレンス
	 * @return 無し
	 * @remark 暗黙の変換で呼び出される
	 */

	template<typename S>
	WeakPointer( const SharedPointer<S>& ref ) : m_pClass( ref.m_pClass ), m_Count( ref.m_Count ) {
	}

	/**
	 *   デスストラクタ
	 * @param  無し
	 * @return 無し
	 */

	~WeakPointer( void ) {
	}

	/**
	 *   コピー
	 * @param  ref ウィークポインタ参照
	 * @return 無し
	 */

	template<class S>
	WeakPointer& operator=(const WeakPointer<S>& ref) {
		m_pClass = ref.lock( ).m_pClass; // ここが違う！
		m_Count = ref.m_Count;
		return *this;
	}

	/**
	 *   コピー
	 * @param  ref シェアードポインタ参照
	 * @return 無し
	 */

	template<class S>
	WeakPointer& operator=(const SharedPointer<S>& ref) {
		m_pClass = ref.m_pClass; // ここが違う！
		m_Count = ref.m_Count;
		return *this;
	}

	/**
	 *   lock
	 * @param  無し
	 * @return シェアードポインタの実体を返す
	 * @remark クラスが存在しているのか確認するための命令
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
	 * @param  無し
	 * @return クラスポインタを返す
	 * @remark 高速処理のための抜け道。エンジン以外では使用不可。
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

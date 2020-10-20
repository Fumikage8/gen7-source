/* 
 * File:   gfl_Singleton.h
 * Author: s-kimura
 *
 * Created on 2010/06/04, 15:06
 */

#ifndef GFL_SINGLETON_H
#define	GFL_SINGLETON_H
#pragma once

#if 0 // これだとインテリセンスがうまく効かない けど互換性のため残す

namespace gfl {
namespace core {

/**
 *  シングルトンクラス
 */
template<typename T, typename Parent>
class Singleton : public Parent {
public:

	/**
	 *   シングルトンインスタンス作成
	 * @param  無し
	 * @return シングルトンインスタンスポインター
	 * @remark 作成はシングルスレッドからコールしてください
	 */

	inline static T* CreateInstance( gfl::heap::HeapBase* heap ) {
		T* ins = _spInstance;
		if( !ins ){
			_spInstance = ins = GFL_NEW( heap ) T;
		}
		return ins;
	}

	/**
	 *   シングルトンインスタンス廃棄
	 * @param  無し
	 * @return 無し
	 */

	inline static void DestroyInstance( void ) {
		GFL_DELETE _spInstance;
		_spInstance = 0;
	}

	/**
	 *   シングルトンインスタンス取得
	 * @param  無し
	 * @return シングルトンポインター
	 */

	inline static T* GetInstance( void ) {
		return _spInstance;
	}

	/**
	 *   シングルトンインスタンス設定
	 * @param  adr シングルトンポインター
	 * @return 無し
	 * @remark CreateInstance時に設定されるので普通は必要ないが、コンストラクタ内でインスタンスにアクセスする場合には必要になることがあるかも。
	 */

	inline static void SetInstance( T* adr ) {
		_spInstance = adr;
	}

protected:

	Singleton( void ) {
	}

	virtual ~Singleton( void ) {
		SetInstance( 0 );
	}

private:
	static T* _spInstance;
};

template<typename T, typename Parent> T* Singleton<T, Parent>::_spInstance;
}
}

#define GFL_SINGLETON(name)
#define GFL_SINGLETON_SUBSTANCE(name)

#else

#if 1 //
#define GFL_SINGLETON_SUBSTANCE(name) name* name::_spInstance=0;

#define GFL_SINGLETON(name) \
private: \
static name* _spInstance; \
public: \
inline static name* CreateInstance(gfl::heap::HeapBase* heap=0) \
{ \
	name* ins = _spInstance; \
	if( !ins ){ \
		_spInstance = ins = GFL_NEW(heap) name(heap); \
	} \
	return ins; \
} \
inline static name* GetInstance(void) \
{ \
return _spInstance; \
} \
inline static void SetInstance( name* adr ) { \
	_spInstance = adr; \
} \
inline static void DestroyInstance(void) \
{ \
GFL_DELETE _spInstance; \
_spInstance=0; \
}
#else
#define GFL_SINGLETON(name) \
public: \
static name* GetInstance(void) \
 { \
	static name Instance;	\
return &Instance;	\
}
#endif

#if 0 //
namespace gfl {
namespace core {

class SingletonList : public HashList<ObjectBase> {
public:

	SingletonList( void );

	~SingletonList( void ) {
		clear_and_delete<ObjectBase > ();
	}
};

}
}
#endif
#endif

namespace gfl{
namespace core{

template<class Derived, class Base> class SingletonInherit : public Base {

public:
	static Derived* CreateInstance( gfl::heap::HeapBase* heap = 0 )
	{
		if( !_spInstance ){
			_spInstance = GFL_NEW( heap ) Derived(heap);
		}
		return _spInstance;
	}

	static Derived* GetInstance()
	{
		return _spInstance;
	}

	static void SetInstance(Derived* adr)
	{
		_spInstance = adr;
	}

	static void DestroyInstance(void)
	{
		GFL_DELETE _spInstance;
		_spInstance = NULL;
	}

protected:

	SingletonInherit( void )
	{
	}

	virtual ~SingletonInherit( void )
	{
		SetInstance( 0 );
	}

private:
	static Derived* _spInstance;
};

template<class Derived, class Base> Derived* SingletonInherit<Derived, Base>::_spInstance;
}
}

#endif	/* GFL_SINGLETON_H */


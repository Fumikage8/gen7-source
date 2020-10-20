/* 
 * File:   gfl_Singleton.h
 * Author: s-kimura
 *
 * Created on 2010/06/04, 15:06
 */

#ifndef GFL_SINGLETON_H
#define	GFL_SINGLETON_H
#pragma once

#if 0 // ���ꂾ�ƃC���e���Z���X�����܂������Ȃ� ���ǌ݊����̂��ߎc��

namespace gfl {
namespace core {

/**
 *  �V���O���g���N���X
 */
template<typename T, typename Parent>
class Singleton : public Parent {
public:

	/**
	 *   �V���O���g���C���X�^���X�쐬
	 * @param  ����
	 * @return �V���O���g���C���X�^���X�|�C���^�[
	 * @remark �쐬�̓V���O���X���b�h����R�[�����Ă�������
	 */

	inline static T* CreateInstance( gfl::heap::HeapBase* heap ) {
		T* ins = _spInstance;
		if( !ins ){
			_spInstance = ins = GFL_NEW( heap ) T;
		}
		return ins;
	}

	/**
	 *   �V���O���g���C���X�^���X�p��
	 * @param  ����
	 * @return ����
	 */

	inline static void DestroyInstance( void ) {
		GFL_DELETE _spInstance;
		_spInstance = 0;
	}

	/**
	 *   �V���O���g���C���X�^���X�擾
	 * @param  ����
	 * @return �V���O���g���|�C���^�[
	 */

	inline static T* GetInstance( void ) {
		return _spInstance;
	}

	/**
	 *   �V���O���g���C���X�^���X�ݒ�
	 * @param  adr �V���O���g���|�C���^�[
	 * @return ����
	 * @remark CreateInstance���ɐݒ肳���̂ŕ��ʂ͕K�v�Ȃ����A�R���X�g���N�^���ŃC���X�^���X�ɃA�N�Z�X����ꍇ�ɂ͕K�v�ɂȂ邱�Ƃ����邩���B
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

#endif	/* GFL_SINGLETON_H */


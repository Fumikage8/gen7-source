//==============================================================================
/**
 * @file	gfl_Property.h
 * 	
 * @author	kimura_shin
 * @data	2011/03/29, 14:31
 */
// =============================================================================

#ifndef GFL_PROPERTY_H
#define	GFL_PROPERTY_H
#pragma once

#include <rtti/gfl_PropertyBase.h>

#ifdef USE_PROPERTY

namespace gfl {
namespace rtti {

template<typename Class> class Rtti; // Rtti ��錾

/**
 * �v���p�e�B�N���X
 */
template<typename Class>
class Property : public PropertyBase {
public:
	typedef void( Class::*Function)(void*);

	template<typename T>
	Property( const c8 * const name, const u32 ofs, Rtti<Class>* rtti, const T& val, const c8 * const text )
	: PropertyBase( name, ofs, typeid (val), text ), m_pRtti( rtti ) {
	}

	// ���ꂪ���Ȃ�g���b�L�[�Btypename T ��RTTI���~�����̂Ŗ����������

	template<typename T>
	Property( const c8 * const name, const u32 ofs, Rtti<T>* rtti, const T& val, const c8 * const text )
	: PropertyBase( name, ofs, typeid (val), text ), m_pRtti( reinterpret_cast<Rtti<Class>*> (rtti) ) {
	}

	// �N���X�̃|�C���^�̏ꍇ

	template<typename T>
	Property( const c8 * const name, const u32 ofs, Rtti<T>* rtti, T * const val, const c8 * const text )
	: PropertyBase( name, ofs, typeid (val), text, TYPE_POINTER ), m_pRtti( reinterpret_cast<Rtti<Class>*> (rtti) ) {
	}

	Property( const c8 * const name, Function func, Rtti<Class>* rtti, void* val, const c8 * const text )
	: PropertyBase( name, val, typeid (0), text, TYPE_POINTER ), m_pRtti( rtti ), m_pFunction( func ) {
	}

	~Property( void ) {
	}

	inline void* GetAddress( void* const cls ) const {
		return gfl::core::Utility::AddAddress( cls, m_Offset );
	}

	inline Rtti<Class>* GetRtti( void ) {
		return m_pRtti;
	}

	void CallProperty( Class *cls, void* value ) {
		(cls->*m_pFunction)(value);
	}

private:
	Rtti<Class>* m_pRtti;
	Function m_pFunction;
};

/**
 * �v���p�e�B���X�g�N���X
 */

class PropertyList : public gfl::core::HashList<PropertyBase> {
public:

	PropertyList( void ) : gfl::core::HashList<PropertyBase>(PropertyBase::GetPropertyName, 16) {
	}

	~PropertyList( void ) {
		clear_and_delete<PropertyBase > ();
	}
};

}
}

#endif // USE_PROERTY

#endif	/* GFL_PROPERTY_H */


//==============================================================================
/**
 * @file	gfl_PropertyBase.h
 * 	
 * @author	kimura_shin
 * @data	2011/03/31, 16:04
 */
// =============================================================================

#ifndef GFL_PROPERTYBASE_H
#define	GFL_PROPERTYBASE_H
#pragma once

#include <core/gfl_Config.h>

// プロパティUIを使用するか？　デバッグとPCのみで有効？
#ifdef GFL_PLATFORM_PC
//#if GFL_DEBUG
#undef USE_PROPERTY
#define USE_PROPERTY
#ifdef GFL_PLATFORM_PC
#define USE_PROPERTY_GUI
#endif
//#elif defined(GFL_PLATFORM_3DS)
//#undef USE_PROPERTY
//#define USE_PROPERTY
#endif

#ifdef USE_PROPERTY

#include <typeinfo>

namespace gfl {
namespace rtti {

/**
 * ベースプロパティクラス
 */

class PropertyBase {
protected:
	const c8* m_pPropertyName;
	const u32 m_Offset;
	const u32 m_Type;

#ifdef USE_PROPERTY_GUI
	const c8 * const m_pLabel;
#endif

public:

	enum {
		TYPE_NONE,
		TYPE_F32,
		TYPE_S32,
		TYPE_U32,
		TYPE_S16,
		TYPE_U16,
		TYPE_S8,
		TYPE_U8,
		TYPE_C8P,
		TYPE_CLASS,

		TYPE_NOT_POINTER = 0,
		TYPE_POINTER = (1 << 30), // 31 だと３DSではエラー

		TYPE_MASK = ~TYPE_POINTER
	};

#ifdef USE_PROPERTY_GUI

	PropertyBase( const c8* name, const u32 ofs, const ::std::type_info& type
				 , const c8 * const label, const u32 attr = TYPE_NOT_POINTER )
	: m_pPropertyName( name ), m_Offset( ofs ), m_Type( AnalyzeType( type ) | attr ), m_pLabel( label ) {
	}

	PropertyBase( const c8* name, void* val, const ::std::type_info& type
				 , const c8 * const label, const u32 attr = TYPE_POINTER )
	: m_pPropertyName( name ), m_Offset( (u32) val ), m_Type( AnalyzeType( type ) | attr ), m_pLabel( label ) {
	}
#else

	PropertyBase( const c8* name, const u32 ofs, const ::std::type_info& type
				 , const c8 * const, const u32 attr = TYPE_NOT_POINTER )
	: m_pPropertyName( name ), m_Offset( ofs ), m_Type( AnalyzeType( type ) | attr ) {
	}

	PropertyBase( const c8* name, void* val, const ::std::type_info& type
				 , const c8 * const, const u32 attr = TYPE_POINTER )
	: m_pPropertyName( name ), m_Offset( (u32) val ), m_Type( AnalyzeType( type ) | attr ) {
	}
#endif

	~PropertyBase( void ) {
	}

	const c8* GetPropertyName( void ) const {
		return m_pPropertyName;
	}

	static const c8 * GetPropertyName( const void* cls ) {
		return static_cast<const PropertyBase*> (cls)->m_pPropertyName;
	}

	u32 IsPointer( void ) const {
		return m_Type & TYPE_POINTER;
	}

	u32 GetType( ) const {
		return m_Type & TYPE_MASK;
	}

	u32 AnalyzeType( const ::std::type_info& type ) const {
		u32 ret;

		if( type == typeid (f32) ){
			ret = TYPE_F32;
		} else if( type == typeid (s32) ){
			ret = TYPE_S32;
		} else if( type == typeid (u32) ){
			ret = TYPE_U32;
		} else if( type == typeid (s16) ){
			ret = TYPE_S16;
		} else if( type == typeid (u16) ){
			ret = TYPE_U16;
		} else if( type == typeid (s8) ){
			ret = TYPE_S8;
		} else if( type == typeid (u8) ){
			ret = TYPE_U8;
		} else if( type == typeid (c8*) ){
			ret = TYPE_C8P;
			//			} else if( 0 == Utility::Strncmp( "class", type.name( ), 5 ) ){ // type.name() はメモリーリークになる
		} else {
			ret = TYPE_CLASS;
		}
		return ret;
	}

#ifdef USE_PROPERTY_GUI

	const c8 * GetLabel( void ) const {
		return m_pLabel;
	}
#endif // USE_PROPERTY_GUI

};

}
}

#endif

#endif	/* GFL_PROPERTYBASE_H */


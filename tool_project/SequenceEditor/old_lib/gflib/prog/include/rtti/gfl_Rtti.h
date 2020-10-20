/**
 * @file	gfl_Rtti.h
 * 	
 * @author	kimura_shin
 * @data	2011/03/29, 12:18
 */

#ifndef GFL_RTTI_H
#define	GFL_RTTI_H
#pragma once

namespace gfl {
namespace rtti {

#ifdef USE_PROPERTY

void SeparateString( c8* func, c8* val, c8* buffer );

enum {
	BUFFER_SIZE = 0x400
};

template<typename T>
void CallFunction( T* cls, void* data ) {
	c8 func[BUFFER_SIZE], val[BUFFER_SIZE];
	SeparateString( func, val, reinterpret_cast<c8*> (data) );
	cls->s_Rtti.CallProperty( cls, func, reinterpret_cast<void*> (val) );
}

/**
 * RTTI クラス
 */

template<typename Class>
class Rtti : public RttiBase {
public:

	/**
	 * コンストラクタ
	 * @param name クラス名
	 * @param parent 親 RTTI
	 */
	Rtti( const c8 * const name, RttiBase* parent ) : RttiBase( name, parent ) {
	}

	virtual ~Rtti( void ) {
	}

	class Table {
	public:

		// クラス以外の変数を定義
#ifdef USE_PROPERTY_GUI
#define PROPERTY_TABLE( type ) \
			Table( const c8 * const name, u32 ofs, Rtti<Class>* rtti, const type val, const c8 * const text ) { \
				rtti->GetPropertyList( )->Append( new Property<Class > (name, ofs, rtti, val, text) ); \
				rtti->GetNameList( )->append( name ); \
			}
#else
#define PROPERTY_TABLE( type ) \
			Table( const c8 * const name, u32 ofs, Rtti<Class>* rtti, const type val, const c8 * const text ) { \
				rtti->GetPropertyList( )->Append( new Property<Class > (name, ofs, rtti, val, text) ); \
			}
#endif

		PROPERTY_TABLE( f32 )
		PROPERTY_TABLE( s32 )
		PROPERTY_TABLE( u32 )
		PROPERTY_TABLE( s16 )
		PROPERTY_TABLE( u16 )
		PROPERTY_TABLE( s8 )
		PROPERTY_TABLE( u8 )

		// char[]　のRTTIが欲しい。
		Table( const c8 * const name, u32 ofs, Rtti<Class>* rtti, c8 * const val, const c8 * const text ) {
			rtti->GetPropertyList( )->Append( new Property<Class > (name, ofs, rtti, val, text) );
#ifdef USE_PROPERTY_GUI
			rtti->GetNameList( )->append( name );
#endif
		}

		// T　のRTTIが欲しい。　T　はクラスのみ可

		template<typename T>
		Table( const c8 * const name, u32 ofs, Rtti<Class>* rtti, const T val, const c8 * const text ) {
			rtti->GetPropertyList( )->Append( new Property<Class > (name, ofs, val.GetRtti( ), val, text) );
#ifdef USE_PROPERTY_GUI
			rtti->GetNameList( )->append( name );
#endif
		}

		// T　のRTTIが欲しい。　T　はクラスポインタのみ可

		template<typename T>
		Table( const c8 * const name, u32 ofs, Rtti<Class>* rtti, T * const val, const c8 * const text ) {
			rtti->GetPropertyList( )->Append( new Property<Class > (name, ofs, val->GetRtti( ), val, text) );
#ifdef USE_PROPERTY_GUI
			rtti->GetNameList( )->append( name );
#endif
		}
	};

	/**
	 * プロパティの取得
	 * @param cls クラスポインタ
	 * @param name プロパティ名
	 * @return プロパティ値
	 */
	Property<Class>* GetProperty( Class * const cls, const c8 * const name ) const {
		const void* tbl = RttiBase::GetProperty( name );
		if( tbl ){
			return const_cast<Property<Class>*> (static_cast<const Property<Class>*> (tbl));
		}
		// 本当は SuperClass だけど動くからよし
		Rtti<Class>* parent = (Rtti<Class>*)GetRttiParent( );
		if( parent ){ // 親をさかのぼって検索
			return parent->GetProperty( cls, name );
		} else {
			return 0;
		}
	}

	/**
	 * プロパティ値の取得
	 * @param cls クラスポインタ
	 * @param name プロパティ名
	 * @return プロパティ値
	 */
	template<typename T>
	T GetPropertyValue( Class * const cls, const c8 * const name ) const {
		Property<Class>* prop = GetProperty( cls, name );
		if( prop ){
			return *(reinterpret_cast<T*> (prop->GetAddress( cls )));
		}
		return 0;
	}

	/**
	 * プロパティクラスポインタの取得
	 * @param cls クラスポインタ
	 * @param name プロパティ名
	 * @return プロパティクラスポインタ
	 */

	template<typename T>
	T GetPropertyClass( Class * const cls, const c8 * const name ) const {
		Property<Class>* prop = GetProperty( cls, name );
		if( prop ){
			return reinterpret_cast<T> (prop->GetAddress( cls ));
		}
		return 0;
	}

	/** インデックス付きプロパティの取得
	 * @param  cls  クラスポインタ
	 * @param  name プロパティ名
	 * @param  idx   インデックス
	 * @return プロパティ値
	 */

	template<typename T>
	T GetPropertyValue( Class * const cls, const c8 * const name, const u32 idx ) const {
		Property<Class>* prop = GetProperty( cls, name );
		if( prop ){
			return (reinterpret_cast<T*> (prop->GetAddress( cls )))[idx];
		}
		return 0;
	}

	/** プロパティの設定
	 * @param  cls  クラスポインタ
	 * @param  name プロパティ名
	 * @param  value プロパティ値
	 * @return 無し
	 */

	template<typename T>
	void SetPropertyValue( Class *cls, const c8 * const name, const T value ) {
		Property<Class>* prop = GetProperty( cls, name );
		if( prop ){
			*(reinterpret_cast<T*> (prop->GetAddress( cls ))) = value;
		}
	}

	/** インデックス付きプロパティの設定
	 * @param  cls  クラスポインタ
	 * @param  name プロパティ名
	 * @param  value プロパティ値
	 * @param  idx   インデックス
	 * @return 無し
	 */

	template<typename T>
	void SetPropertyValue( Class *cls, const c8 * const name, const T value, const u32 idx ) {
		Property<Class>* prop = GetProperty( cls, name );
		if( prop ){
			(reinterpret_cast<T*> (prop->GetAddress( cls )))[idx] = value;
		}
	}

	/**
	 * プロパティタイトルの取得
	 * @param name
	 * @return プロパティタイトル
	 */
	const c8 * GetPropertyLabel( const c8 * const name ) const {
		Property<Class>* prop = GetProperty( (Class *) 0, name );
		if( prop ){
			return prop->GetLabel( );
		}
		return 0;
	}

	void CallProperty( Class *cls, const c8 * const name, void* value ) {
		Property<Class>* prop = GetProperty( cls, name );
		if( prop ){
			prop->CallProperty( cls, value );
		}
	}

	/**
	 *   プロパティの登録
	 * @param  name プロパティ名
	 * @param  getter 取得関数
	 * @param  setter 設定関数
	 * @param  dti  ＤＴＩ
	 * @return 無し
	 */

	void RegisterProperty( const c8 * const name, typename Property<Class>::Function func, gfl::heap::HeapBase* heap, void* val = 0 ) {
		GetPropertyList( )->Append( GFL_NEW(heap) Property<Class > (name, func, this, val, name) );
	}

#ifdef USE_PROPERTY_GUI
	// 内部クラスのGUIを作成する関数

	virtual QWidget* CreateInternalGui( void* cls, QWidget* parent ) {
		return CreateGui( reinterpret_cast<Class *> (cls), parent );
	}

	QWidget* CreateGui( Class *cls, QWidget* parent = 0 ) {

		QtPropertyDialog* dia = new QtPropertyDialog( parent );

		dia->setWindowTitle( GetRttiName( ) );
		dia->setAttribute( Qt::WA_DeleteOnClose, true );
		QVBoxLayout* v_layout = new QVBoxLayout;

		for( s32 i = 0; i < GetNameList( )->size( ); ++i ){
			QHBoxLayout* h_layout = new QHBoxLayout;

			Property<Class>* prop = GetProperty( cls, GetNameList( )->at( i ).toAscii( ).data( ) );
			h_layout->addWidget( new QLabel( prop->GetLabel( ) ) );

			const s32 type = prop->GetType( );

			if( type == PropertyBase::TYPE_CLASS ){
				QtPropertyClass* bt;
				bt = new QtPropertyClass( GetNameList( )->at( i ),
										 prop->IsPointer( )
										 ? reinterpret_cast<void*> (*reinterpret_cast<size_t*> (prop->GetAddress( cls )))
										 : prop->GetAddress( cls ),
										 prop->GetRtti( ),
										 dia );
				h_layout->addWidget( bt );
			} else {
				if( type == PropertyBase::TYPE_F32 ){
					QtPropertyFloat* dsp = new QtPropertyFloat( prop->GetAddress( cls ) );
					dsp->setMinimum( gfl::core::Math::F32_MIN );
					dsp->setMaximum( gfl::core::Math::F32_MAX );
					dsp->setValue( *(reinterpret_cast<f32*> (prop->GetAddress( cls ))) );
					dsp->setAlignment( Qt::AlignRight );
					h_layout->addWidget( dsp );
					dsp->connect( dsp, SIGNAL( editingFinished( ) ), dsp, SLOT( SetValue_F32( ) ) );
				} else if( type == PropertyBase::TYPE_C8P ){
					QLabel* lb = new QLabel( (reinterpret_cast<c8*> (prop->GetAddress( cls ))) );
					lb->setAlignment( Qt::AlignRight );
					h_layout->addWidget( lb );
				} else {
					QtProperty* sp = new QtProperty( prop->GetAddress( cls ), prop->GetType( ) );

					switch( type ) {
					case PropertyBase::TYPE_S32:
						sp->setMinimum( gfl::core::Math::S32_MIN );
						sp->setMaximum( gfl::core::Math::S32_MAX );
						sp->setValue( *(reinterpret_cast<s32*> (prop->GetAddress( cls ))) );
						//						sp->connect( sp, SIGNAL( valueChanged( int ) ), sp, SLOT( SetValue_S32( int ) ) );
						sp->connect( sp, SIGNAL( editingFinished( ) ), sp, SLOT( SetValue_S32( ) ) );
						break;
					case PropertyBase::TYPE_U32:
						sp->setMaximum( gfl::core::Math::S32_MAX ); // U32_MAX じゃなくて S32_MAX
						sp->setValue( *(reinterpret_cast<u32*> (prop->GetAddress( cls ))) );
						sp->connect( sp, SIGNAL( editingFinished( ) ), sp, SLOT( SetValue_U32( ) ) );
						break;
					case PropertyBase::TYPE_S16:
						sp->setMinimum( gfl::core::Math::S16_MIN );
						sp->setMaximum( gfl::core::Math::S16_MAX );
						sp->setValue( *(reinterpret_cast<s16*> (prop->GetAddress( cls ))) );
						sp->connect( sp, SIGNAL( editingFinished( ) ), sp, SLOT( SetValue_S16( ) ) );
						break;
					case PropertyBase::TYPE_U16:
						sp->setMaximum( gfl::core::Math::U16_MAX );
						sp->setValue( *(reinterpret_cast<u16*> (prop->GetAddress( cls ))) );
						sp->connect( sp, SIGNAL( editingFinished( ) ), sp, SLOT( SetValue_U16( ) ) );
						break;
					case PropertyBase::TYPE_S8:
						sp->setMinimum( gfl::core::Math::S8_MIN );
						sp->setMaximum( gfl::core::Math::S8_MAX );
						sp->setValue( *(reinterpret_cast<s8*> (prop->GetAddress( cls ))) );
						sp->connect( sp, SIGNAL( editingFinished( ) ), sp, SLOT( SetValue_S8( ) ) );
						break;
					case PropertyBase::TYPE_U8:
						sp->setMaximum( gfl::core::Math::U8_MAX );
						sp->setValue( *(reinterpret_cast<u8*> (prop->GetAddress( cls ))) );
						sp->connect( sp, SIGNAL( editingFinished( ) ), sp, SLOT( SetValue_U8( ) ) );
					}
					sp->setAlignment( Qt::AlignRight );
					h_layout->addWidget( sp );
				}
			}
			v_layout->addLayout( h_layout );
		}

		dia->setLayout( v_layout );
		if( parent ){
			dia->move( parent->frameGeometry( ).x( ) + parent->frameGeometry( ).width( ), parent->frameGeometry( ).y( ) );
		}
		dia->show( );

		return dia;
	}
#endif
};

#else

template<typename T>
void CallFunction( T*, void* ) {
}

#endif // USE_PROPERTY

}
}

#endif	/* GFL_RTTI_H */


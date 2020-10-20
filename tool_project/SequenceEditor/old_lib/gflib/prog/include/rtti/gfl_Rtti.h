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
 * RTTI �N���X
 */

template<typename Class>
class Rtti : public RttiBase {
public:

	/**
	 * �R���X�g���N�^
	 * @param name �N���X��
	 * @param parent �e RTTI
	 */
	Rtti( const c8 * const name, RttiBase* parent ) : RttiBase( name, parent ) {
	}

	virtual ~Rtti( void ) {
	}

	class Table {
	public:

		// �N���X�ȊO�̕ϐ����`
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

		// char[]�@��RTTI���~�����B
		Table( const c8 * const name, u32 ofs, Rtti<Class>* rtti, c8 * const val, const c8 * const text ) {
			rtti->GetPropertyList( )->Append( new Property<Class > (name, ofs, rtti, val, text) );
#ifdef USE_PROPERTY_GUI
			rtti->GetNameList( )->append( name );
#endif
		}

		// T�@��RTTI���~�����B�@T�@�̓N���X�̂݉�

		template<typename T>
		Table( const c8 * const name, u32 ofs, Rtti<Class>* rtti, const T val, const c8 * const text ) {
			rtti->GetPropertyList( )->Append( new Property<Class > (name, ofs, val.GetRtti( ), val, text) );
#ifdef USE_PROPERTY_GUI
			rtti->GetNameList( )->append( name );
#endif
		}

		// T�@��RTTI���~�����B�@T�@�̓N���X�|�C���^�̂݉�

		template<typename T>
		Table( const c8 * const name, u32 ofs, Rtti<Class>* rtti, T * const val, const c8 * const text ) {
			rtti->GetPropertyList( )->Append( new Property<Class > (name, ofs, val->GetRtti( ), val, text) );
#ifdef USE_PROPERTY_GUI
			rtti->GetNameList( )->append( name );
#endif
		}
	};

	/**
	 * �v���p�e�B�̎擾
	 * @param cls �N���X�|�C���^
	 * @param name �v���p�e�B��
	 * @return �v���p�e�B�l
	 */
	Property<Class>* GetProperty( Class * const cls, const c8 * const name ) const {
		const void* tbl = RttiBase::GetProperty( name );
		if( tbl ){
			return const_cast<Property<Class>*> (static_cast<const Property<Class>*> (tbl));
		}
		// �{���� SuperClass �����Ǔ�������悵
		Rtti<Class>* parent = (Rtti<Class>*)GetRttiParent( );
		if( parent ){ // �e�������̂ڂ��Č���
			return parent->GetProperty( cls, name );
		} else {
			return 0;
		}
	}

	/**
	 * �v���p�e�B�l�̎擾
	 * @param cls �N���X�|�C���^
	 * @param name �v���p�e�B��
	 * @return �v���p�e�B�l
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
	 * �v���p�e�B�N���X�|�C���^�̎擾
	 * @param cls �N���X�|�C���^
	 * @param name �v���p�e�B��
	 * @return �v���p�e�B�N���X�|�C���^
	 */

	template<typename T>
	T GetPropertyClass( Class * const cls, const c8 * const name ) const {
		Property<Class>* prop = GetProperty( cls, name );
		if( prop ){
			return reinterpret_cast<T> (prop->GetAddress( cls ));
		}
		return 0;
	}

	/** �C���f�b�N�X�t���v���p�e�B�̎擾
	 * @param  cls  �N���X�|�C���^
	 * @param  name �v���p�e�B��
	 * @param  idx   �C���f�b�N�X
	 * @return �v���p�e�B�l
	 */

	template<typename T>
	T GetPropertyValue( Class * const cls, const c8 * const name, const u32 idx ) const {
		Property<Class>* prop = GetProperty( cls, name );
		if( prop ){
			return (reinterpret_cast<T*> (prop->GetAddress( cls )))[idx];
		}
		return 0;
	}

	/** �v���p�e�B�̐ݒ�
	 * @param  cls  �N���X�|�C���^
	 * @param  name �v���p�e�B��
	 * @param  value �v���p�e�B�l
	 * @return ����
	 */

	template<typename T>
	void SetPropertyValue( Class *cls, const c8 * const name, const T value ) {
		Property<Class>* prop = GetProperty( cls, name );
		if( prop ){
			*(reinterpret_cast<T*> (prop->GetAddress( cls ))) = value;
		}
	}

	/** �C���f�b�N�X�t���v���p�e�B�̐ݒ�
	 * @param  cls  �N���X�|�C���^
	 * @param  name �v���p�e�B��
	 * @param  value �v���p�e�B�l
	 * @param  idx   �C���f�b�N�X
	 * @return ����
	 */

	template<typename T>
	void SetPropertyValue( Class *cls, const c8 * const name, const T value, const u32 idx ) {
		Property<Class>* prop = GetProperty( cls, name );
		if( prop ){
			(reinterpret_cast<T*> (prop->GetAddress( cls )))[idx] = value;
		}
	}

	/**
	 * �v���p�e�B�^�C�g���̎擾
	 * @param name
	 * @return �v���p�e�B�^�C�g��
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
	 *   �v���p�e�B�̓o�^
	 * @param  name �v���p�e�B��
	 * @param  getter �擾�֐�
	 * @param  setter �ݒ�֐�
	 * @param  dti  �c�s�h
	 * @return ����
	 */

	void RegisterProperty( const c8 * const name, typename Property<Class>::Function func, gfl::heap::HeapBase* heap, void* val = 0 ) {
		GetPropertyList( )->Append( GFL_NEW(heap) Property<Class > (name, func, this, val, name) );
	}

#ifdef USE_PROPERTY_GUI
	// �����N���X��GUI���쐬����֐�

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
						sp->setMaximum( gfl::core::Math::S32_MAX ); // U32_MAX ����Ȃ��� S32_MAX
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


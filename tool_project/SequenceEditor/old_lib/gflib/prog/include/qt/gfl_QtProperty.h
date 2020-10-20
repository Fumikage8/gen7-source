//==============================================================================
/**
 * @file	gfl_QtProperty.h
 * 	
 * @author	kimura_shin
 * @data	2011/03/31, 10:41
 */
// =============================================================================

#ifndef GFL_QTPROPERTY_H
#define	GFL_QTPROPERTY_H

#include <core/gfl_Config.h>
#include <gfl_Qt.h>

// MOC_が必要な場合はネームスペースマクロ化不能 しかも２段の深さは無理っぽい…　namespace qt {
namespace gfl {

/**
 * 整数プロパティGUI
 */
class QtProperty : public QSpinBox {
	Q_OBJECT
private:
	void* const m_pAddress;
	const s32 m_Type;

public:

	QtProperty( void* const adr, const s32 type ) : m_pAddress( adr ), m_Type( type ) {
	}

	void Update( void );

	// 矢印上下で変更された場合に更新する

	virtual void stepBy( int step ) {
		QSpinBox::stepBy( step );
		switch( m_Type ) {
		case gfl::rtti::PropertyBase::TYPE_S32:
			SetValue_S32( );
			break;
		case gfl::rtti::PropertyBase::TYPE_U32:
			SetValue_U32( );
			break;
		case gfl::rtti::PropertyBase::TYPE_S16:
			SetValue_S16( );
			break;
		case gfl::rtti::PropertyBase::TYPE_U16:
			SetValue_U16( );
			break;
		case gfl::rtti::PropertyBase::TYPE_S8:
			SetValue_S8( );
			break;
		case gfl::rtti::PropertyBase::TYPE_U8:
			SetValue_U8( );
		}
	}

public slots:

	void SetValue_S32( ) {
		*(reinterpret_cast<s32*> (m_pAddress)) = value( );
	}

	void SetValue_U32( ) {
		*(reinterpret_cast<u32*> (m_pAddress)) = value( );
	}

	void SetValue_S16( ) {
		*(reinterpret_cast<s16*> (m_pAddress)) = value( );
	}

	void SetValue_U16( ) {
		*(reinterpret_cast<u16*> (m_pAddress)) = value( );
	}

	void SetValue_S8( ) {
		*(reinterpret_cast<s8*> (m_pAddress)) = value( );
	}

	void SetValue_U8( ) {
		*(reinterpret_cast<u8*> (m_pAddress)) = value( );
	}
};

/**
 * 浮動小数プロパティGUI
 */
class QtPropertyFloat : public QDoubleSpinBox {
	Q_OBJECT
private:
	void* const m_pAddress;

public:

	QtPropertyFloat( void* const adr ) : m_pAddress( adr ) {
	}

	void Update( void ) {
		setValue( *(reinterpret_cast<f32*> (m_pAddress)) );
	}

	// 矢印上下で変更された場合に更新する

	virtual void stepBy( int step ) {
		QDoubleSpinBox::stepBy( step );
		SetValue_F32( );
	}

public slots:
#if 0 // これだと編集中に変わっちゃうので

	void SetValue_F32( double val ) {
		*(reinterpret_cast<f32*> (m_pAddress)) = val;
	}
#else

	void SetValue_F32( void ) {
		*(reinterpret_cast<f32*> (m_pAddress)) = value( );
	}
#endif
};

/**
 * クラスプロパティGUI
 */
class QtPropertyClass : public QPushButton {
	Q_OBJECT
private:
	void* const m_pAddress;
	gfl::rtti::RttiBase* m_pRttiBase;
	QWidget* m_pParent;

public:

	QtPropertyClass( const QString& title, void* const adr, gfl::rtti::RttiBase* prop, QWidget* parent )
	: QPushButton( title ), m_pAddress( adr ), m_pRttiBase( prop ), m_pParent( parent ) {
		connect( this, SIGNAL( clicked( bool ) ), this, SLOT( Clicked( bool ) ) );
	}

public slots:
	void Clicked( bool );
};

/**
 * プロパティルートダイアログ
 */
class QtPropertyDialog : public QDialog {
	QPushButton* m_pButton;
public:

	QtPropertyDialog( QWidget* parent ) : QDialog( parent ), m_pButton( 0 ) {
	}

	virtual void closeEvent( QCloseEvent* ) {
		if( m_pButton ){
			m_pButton->setDisabled( false );
		}
	}

	void SetButton( QPushButton* Button ) {
		this->m_pButton = Button;
	}
};

/**
 * プロパティルートダイアログ
 */
class QtPropertyDialogRoot : public QDialog {
public:

	QtPropertyDialogRoot( void ) {
		startTimer( gfl::core::Time::GFL_MILLI_SECOND / 30 );
	}

	virtual void timerEvent( QTimerEvent *event );

	template<typename T>
	void CreateGui( T* cls ) {
		QPushButton* bt = static_cast<QPushButton*> (sender( ));
		cls->CreateGui( this )->SetButton( bt );
		bt->setDisabled( true );
	}
};
//	}
}

#endif	/* GFL_QTPROPERTY_H */

//==============================================================================
/**
 * @file	ExQtProperty.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2011/04/04, 11:00
 */
// =============================================================================
void gfl::QtProperty::Update( void )
{
	switch( m_Type ){
	case gfl::rtti::PropertyBase::TYPE_S32:
		setValue( *( reinterpret_cast < s32* >( m_pAddress ) ) );
		break;
	case gfl::rtti::PropertyBase::TYPE_U32:
		setValue( *( reinterpret_cast < u32* >( m_pAddress ) ) );
		break;
	case gfl::rtti::PropertyBase::TYPE_S16:
		setValue( *( reinterpret_cast < s16* >( m_pAddress ) ) );
		break;
	case gfl::rtti::PropertyBase::TYPE_U16:
		setValue( *( reinterpret_cast < u16* >( m_pAddress ) ) );
		break;
	case gfl::rtti::PropertyBase::TYPE_S8:
		setValue( *( reinterpret_cast < s8* >( m_pAddress ) ) );
		break;
	case gfl::rtti::PropertyBase::TYPE_U8:
		setValue( *( reinterpret_cast < u8* >( m_pAddress ) ) );
		break;
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void gfl::QtPropertyClass::Clicked( bool )
{
	( static_cast < QtPropertyDialog* >( m_pRttiBase->CreateInternalGui( m_pAddress, m_pParent ) ) )->SetButton( this );
	setDisabled( true );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void gfl::QtPropertyDialogRoot::timerEvent( QTimerEvent *event )
{
	QList<QtProperty*> lst = findChildren<QtProperty*>( );
	for( s32 i = 0; i < lst.size( ); ++i ){
		lst.at( i )->Update( );
	}
	QList<QtPropertyFloat*> lst_f = findChildren<QtPropertyFloat*>( );
	for( s32 i = 0; i < lst_f.size( ); ++i ){
		lst_f.at( i )->Update( );
	}
}

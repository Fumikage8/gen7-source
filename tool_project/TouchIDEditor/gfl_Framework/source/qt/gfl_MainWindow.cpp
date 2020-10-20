//==============================================================================
/**
 * @file	gfl_MainWindow.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2012/05/25, 14:25
 */
// =============================================================================

#include <qt/gfl_MainWindow.h>

namespace gfl {
namespace qt {

const c8 * const MainWindow::KEY_RECENT_FILE = "recentFileList";

// -----------------------------------------------------------------------------
// Construct
// -----------------------------------------------------------------------------
MainWindow::MainWindow( gfl::heap::HeapBase* ) : m_pSettings( 0 ), m_RecentFileActs(0)
{
	setAttribute( Qt::WA_DeleteOnClose ); // クローズ時にデストラクトする
	setAttribute( Qt::WA_ShowModal, true );
}

// ---------------------------------------------------------------------------
// Destruct
// ---------------------------------------------------------------------------
MainWindow::~MainWindow( void )
{
	delete m_pSettings;
	delete [] m_RecentFileActs;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void MainWindow::CreateRecentFileAction( const s32 max )
{
	m_MaxRecentFiles = max;
	m_RecentFileActs = DbgNew QAction* [max];
	for( int i = 0; i < max; ++i ){
		m_RecentFileActs[i] = DbgNew QAction( this );
		m_RecentFileActs[i]->setVisible( false );
		connect( m_RecentFileActs[i], SIGNAL( triggered( ) ), this, SLOT( OpenRecentFile( ) ) );
	}
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void MainWindow::CreateRecentFileMenu( QMenu* menu )
{
	menu->addSeparator( );
	for( int i = 0; i < m_MaxRecentFiles; ++i ){
		menu->addAction( m_RecentFileActs[i] );
	}
	UpdateRecentFileActions( );
	menu->addSeparator( );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MainWindow::SetCurrentFile( const QString &fileName )
{
	setWindowFilePath( fileName );
	if( !m_pSettings ){
		QMessageBox::critical( this, "Error", "QSetting が設定されていません" );
		return;
	}
	QStringList files = m_pSettings->value( KEY_RECENT_FILE ).toStringList( );
	files.removeAll( fileName );
	files.prepend( fileName );
	while( files.size( ) > m_MaxRecentFiles )
		files.removeLast( );

	m_pSettings->setValue( KEY_RECENT_FILE, files );
	UpdateRecentFileActions( );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MainWindow::OpenRecentFile( )
{
	QAction *action = qobject_cast<QAction *>( sender( ) );
	if( action ){
		QString fname( action->data( ).toString( ) );
		if( LoadFile( fname ) ){
			SetCurrentFile( fname );
		}
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MainWindow::UpdateRecentFileActions( )
{
	if( !m_pSettings ){
		QMessageBox::critical( this, "Error", "QSetting が設定されていません" );
		return;
	}
	QStringList files = m_pSettings->value( KEY_RECENT_FILE ).toStringList( );

	int numRecentFiles = qMin( files.size( ), ( int )m_MaxRecentFiles );
	if(numRecentFiles){
		m_RecentFileName = files[0];
	}
	for( int i = 0; i < numRecentFiles; ++i ){
		m_RecentFileActs[i]->setText( StrippedName( files[i] ) );
		m_RecentFileActs[i]->setToolTip( files[i] );
		m_RecentFileActs[i]->setData( files[i] );
		m_RecentFileActs[i]->setVisible( true );
	}
	for( int j = numRecentFiles; j < m_MaxRecentFiles; ++j ){
		m_RecentFileActs[j]->setVisible( false );
	}
}

}
}

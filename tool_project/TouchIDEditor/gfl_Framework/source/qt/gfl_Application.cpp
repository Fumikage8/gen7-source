gfl::qt::Application* gfl::qt::Application::_spInstance = 0;

gfl::qt::Application::Application( int & argc, char ** argv ) : QApplication( argc, argv )
{
	// コーデック設定
	QTextCodec::setCodecForCStrings( QTextCodec::codecForLocale( ) );
	//	QTextCodec::setCodecForCStrings( QTextCodec::codecForName( "UTF-8" ) );
	//QTextCodec::setCodecForCStrings( QTextCodec::codecForName( "Shift-Jis" ) );
}


//==============================================================================
//==============================================================================
gfl::qt::Application* gfl::qt::Application::CreateInstance( int& argc, char** argv, const c8* installer_path, const c8* uninstaller )
{
	gfl::qt::Application* ins = CreateInstance( argc, argv );

#ifndef GFL_DEBUG // デバッグじゃなかったらDOS窓を出さない
	FreeConsole( );
#endif
	// 現在のアンインストーラーの修正時間よりインストーラが新しい場合はアップデートする

	QString installer(QString(installer_path).section('/',-1) );
	
	if( QFile::exists( uninstaller ) && QFileInfo( uninstaller ).lastModified( )
		< QFileInfo( installer_path ).lastModified( ) ){
		QMessageBox::StandardButton ret = QMessageBox::question( 0, "update"
																 , "ソフトウェアをアップデートしますか？"
//			, "ソフトウェアの更新が確認されました。\nアップデートしますか？"
																 , QMessageBox::Apply | QMessageBox::Cancel );
		if( ret == QMessageBox::Apply ){
//			QString tmp(getenv("TEMP"));
//			tmp += "\\" + installer;			
//			if(QFile::exists(tmp)){
//				QFile::remove(tmp);
//			}
			
//			if(QFile::copy(installer_path,tmp)){
				// Sysinternals/ListDLLs/EulaAccepted 1 もし listdll を使用するならこのレジストリをハックする
				QProcess::startDetached( installer_path );
//			}else{
//				QMessageBox::critical(0,"Error","Failed to copy installer..." + tmp);
//			}
			return 0;
		}
	}

	return ins;
}

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
gfl::qt::ProgressDialog::ProgressDialog( QObject* parent, const QString& txt ) : QProgressDialog( ( QWidget* )parent )
{
	// 文字表示
	setLabelText( txt );
	// ％で出せないのでぐるぐる回るバーを表示　両方０なら行ってよし
	setMaximum( 0 );
	setMinimum( 0 );
	setWindowModality( Qt::WindowModal );
//	setCancelButton( 0 ); // キャンセルボタンを表示しない
	setWindowFlags( Qt::Dialog | Qt::CustomizeWindowHint );
}

void gfl::qt::TimerMessage( const QString& str , const s32 sec )
{
	QDialog dia;
	QVBoxLayout* lay = new QVBoxLayout;
	QLabel* lb = new QLabel( str );
	QFont fnt;
	fnt.setBold(true);
	lb->setFont(fnt);
//	lb->setFrameStyle(QFrame::Panel );
	lb->setAlignment(Qt::AlignBottom | Qt::AlignCenter);
	lay->addWidget( lb );
	dia.setLayout(lay);
	QTimer::singleShot( sec * 1000, &dia, SLOT( close() ));
	dia.setModal(true);
	dia.exec();
}

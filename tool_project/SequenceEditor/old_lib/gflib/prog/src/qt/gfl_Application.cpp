gfl::qt::Application* gfl::qt::Application::_spInstance = 0;

gfl::qt::Application::Application( int & argc, char ** argv ) : QApplication( argc, argv )
{
	// �R�[�f�b�N�ݒ�
	QTextCodec::setCodecForCStrings( QTextCodec::codecForLocale( ) );
	//	QTextCodec::setCodecForCStrings( QTextCodec::codecForName( "UTF-8" ) );
	//QTextCodec::setCodecForCStrings( QTextCodec::codecForName( "Shift-Jis" ) );
}


//==============================================================================
//==============================================================================
gfl::qt::Application* gfl::qt::Application::CreateInstance( int& argc, char** argv, const c8* installer_path, const c8* uninstaller )
{
	gfl::qt::Application* ins = CreateInstance( argc, argv );

#ifndef GFL_DEBUG // �f�o�b�O����Ȃ�������DOS�����o���Ȃ�
	FreeConsole( );
#endif
	// ���݂̃A���C���X�g�[���[�̏C�����Ԃ��C���X�g�[�����V�����ꍇ�̓A�b�v�f�[�g����

	QString installer(QString(installer_path).section('/',-1) );
	
	if( QFile::exists( uninstaller ) && QFileInfo( uninstaller ).lastModified( )
		< QFileInfo( installer_path ).lastModified( ) ){
		QMessageBox::StandardButton ret = QMessageBox::question( 0, "update"
																 , "�\�t�g�E�F�A���A�b�v�f�[�g���܂����H"
//			, "�\�t�g�E�F�A�̍X�V���m�F����܂����B\n�A�b�v�f�[�g���܂����H"
																 , QMessageBox::Apply | QMessageBox::Cancel );
		if( ret == QMessageBox::Apply ){
//			QString tmp(getenv("TEMP"));
//			tmp += "\\" + installer;			
//			if(QFile::exists(tmp)){
//				QFile::remove(tmp);
//			}
			
//			if(QFile::copy(installer_path,tmp)){
				// Sysinternals/ListDLLs/EulaAccepted 1 ���� listdll ���g�p����Ȃ炱�̃��W�X�g�����n�b�N����
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
	// �����\��
	setLabelText( txt );
	// ���ŏo���Ȃ��̂ł��邮����o�[��\���@�����O�Ȃ�s���Ă悵
	setMaximum( 0 );
	setMinimum( 0 );
	setWindowModality( Qt::WindowModal );
//	setCancelButton( 0 ); // �L�����Z���{�^����\�����Ȃ�
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

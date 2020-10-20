//==============================================================================
/**
 * @file	gfl_MainWindow.h
 * 	
 * @author	kimura_shin
 * @data	2012/05/25, 14:22
 */
// =============================================================================

#if !defined( __GFL_MAINWINDOW_H__ )
#define	__GFL_MAINWINDOW_H__

#include <qt/gfl_QtGui.h>

// moc_ ������ꍇ��GFL_NAMESPACE�@�}�N�����g���Ȃ�
namespace gfl {
	namespace qt {

		class MainWindow : public QMainWindow {
			Q_OBJECT
		public:
			MainWindow( gfl::heap::HeapBase* heap = 0 );
			virtual ~MainWindow( void );
			void UpdateRecentFileActions( );
			/**
			 * ���[�h�����t�@�C������ݒ�
             * @param fileName
             */
			void SetCurrentFile( const QString &fileName );

			/**
			 * �ݒ背�W�X�g�����擾
             * @return �ݒ背�W�X�g��
             */
            QSettings* GetSettings( ) const {
            	return m_pSettings;
            }

            const QString& GetRecentFileName( ) const {
            	return m_RecentFileName;
            }
#if 0 // 
			virtual void closeEvent( QCloseEvent* event ){
				if(  ){ // �ύX�����������H
					QMessageBox::StandardButton ret = QMessageBox::question(this,"Save ?", "�ύX����Ă��܂��B�ۑ����܂����H"
						,QMessageBox::Apply | QMessageBox::Cancel  );
					if( ret == QMessageBox::Apply ){
						// �ۑ����s
						event->accept();
					}else{
						event->ignore();
					}
				}else{
					event->accept();
				}
			}
#endif
		protected:
			QSettings* m_pSettings;
			QAction **m_RecentFileActs;
//			QMenu *m_RecentFilesMenu;
			enum {
				MaxRecentFiles = 5
			};
			s32 m_MaxRecentFiles;

			void CreateRecentFileAction( const s32 max = MaxRecentFiles );
			void CreateRecentFileMenu( QMenu* menu );
			virtual bool LoadFile( const QString& fname ) = 0;

			QString StrippedName( const QString &fullFileName ) {
				return QFileInfo( fullFileName ).fileName( );
			}

		private:
			QString m_RecentFileName;
			static const c8 * const KEY_RECENT_FILE;

		public slots:
			void OpenRecentFile( );
		};

	}
}

#endif	/* __GFL_MAINWINDOW_H__ */


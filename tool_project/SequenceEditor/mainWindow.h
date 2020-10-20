/* 
 * File:   mainWindow.h
 * Author: ariizumi_nobuhiko
 *
 * Created on 2011/02/24, 12:34
 */

#ifndef MAINWINDOW_H
#define	MAINWINDOW_H

#include "localDef.h"
#include "widgetSeqBar.h"
#include "widgetSeqLabel.h"
#include "widgetFrameScale.h"
#include "widgetNumBox.h"

#define SEQ_EXT ".seq"

class DataEditor;

namespace gfl{
  namespace xml{
    class Document;
  };
};

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(int argc , char *argv[]);

public Q_SLOTS:
	void UpdateHScrollBar(){UpdateHScrollBarAdjust();}
	void UpdateHScrollBarAdjust(int min = -1, int max = -1);
	void UpdateHScrollNowPos(int val);
	void UpdateHScrollDraw();

  void CommandListChangeItem(QTreeWidgetItem* item,QTreeWidgetItem* prevItem);

	void newFile( );
	void open( );
	bool save( );
	bool saveAs( );

	void undo( );
	void redo( );

  void cut( );
	void copy( );
	void paste( );

  void ConvertBinary();
  void ResetStatusBar();

  void SetDispSizeS();
  void SetDispSizeM();
  void SetDispSizeL();

  void ApplyDefaultLock();

  void LoadSetting();
  void SaveSetting();
  
  void about();
protected:
	void keyPressEvent( QKeyEvent *ev );
	void keyReleaseEvent( QKeyEvent *ev );
  void dragEnterEvent( QDragEnterEvent *event );
  void dropEvent( QDropEvent *event );
  void closeEvent ( QCloseEvent *event );
  void wheelEvent ( QWheelEvent * event );

public:
  //取得系
  QTreeWidget* GetCommandList(){return mCommandList;}
  QWidget* GetTitleAreaWidget(){return mTitleAreaWidget;}
  QVBoxLayout* GetBarLytWidget(){return mBarLyt;}
  QVBoxLayout* GetTitleAreaLytWidget(){return mTitleAreaLyt;}
  WidFrameScale*	GetFrameScaleWidget(){return mFrameScaleWidget;}

  void PopLockWarnning(int type = 0 );

private:
	bool SaveFile( const QString& fname );
	bool LoadFile( const QString& fname );
  bool CloseConfirm(void);
	void CreateFileMenu( void );
	void CreateEditMenu( void );
	void CreateGroupMenu( void );

  void MainWindow::CreateSeqXmlFile(gfl::xml::Document *doc );

  void SetStatusBarMessage( char* msg );

	//全体のエリア
	QWidget     *mMainWidget;
	QGridLayout *mMainLyt;

  QSplitter *mMainLytUp;

	//コマンドリスト
	QTreeWidget *mCommandList;

	//バーのあるスクロールエリア
	QScrollArea *mBarArea;
	QWidget			*mBarAreaWidget;

	//スクロールエリアの内部
	QVBoxLayout *mBarLyt;			//バーがあるレイアウト
	QWidget			*mBarLytWidget;			//バーがあるレイアウト
	QVBoxLayout *mBarAreaLyt;	//↑とスペーサー

	//左のタイトルエリア
	QScrollArea *mTitleArea;
	QWidget			*mTitleAreaWidget;
	QVBoxLayout	*mTitleAreaLyt;

	//下段、フレームラベル
	QGridLayout	*mFrameScaleLyt;
	WidNumBox		*mScaleMaxWidget;
	WidNumBox		*mScaleMinWidget;
	WidNumBox		*mScaleNowWidget;
	WidFrameScale	*mFrameScaleWidget;

	//上のフレームラベル
	WidSequenceLabel *mBarLabel;

	//ヘルプウィンドウ
	QTextEdit *mHelpWin;

  //ステータスバー
	QLabel *mStatusLabel;

  QAction* mActUndo;
  QAction* mActRedo;

  QAction* mActDispS;
  QAction* mActDispM;
  QAction* mActDispL;

  QAction* mActDefaultLock;

	DataEditor *mDataEditor;
	
	QString m_CurrentFileName;

  QString mCommandFilePath;
};


#endif	/* MAINWINDOW_H */


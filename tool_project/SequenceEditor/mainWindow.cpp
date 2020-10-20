//==============================================================================
/**
 * @file	mainWindow.cpp
 * @brief	
 * @author	ariizumi_nobuhiko
 * @data	2011/02/24, 12:34
 */
// =============================================================================

#include "mainWindow.h"
#include "widgetSeqGroup.h"
#include "widgetSeqLabel.h"
#include "widgetTitleName.h"
#include "dialogAddGroup.h"
#include "dataEditor.h"

#include "save_xml_define.h"
#include "converter/convertSeqFile.h"

#include <gflib.h>
#include <rapidxml/gfl_Xml.h>

MainWindow::MainWindow(int argc , char *argv[])
{
/*
  mSeqBarTest = new WidSequenceBar(this);
  setCentralWidget( mSeqBarTest );
*/
/*
  QWidget *mainWidget = new QWidget(this);

  QVBoxLayout *layout = new QVBoxLayout(mainWidget);
  layout->setSpacing(0);

  for( int i=0;i<10;i++ )
  {
    WidSequenceBar *bar = new WidSequenceBar(mainWidget);
    layout->addWidget(bar);
  }

  setCentralWidget( mainWidget );

*/
  mCommandFilePath = "./command.xml";

  for( int i=0 ; i<argc ; i++ )
  {
    if( strcmp( "-c" , argv[i] ) == 0 )
    {
      mCommandFilePath = argv[i+1];
    }
  }


	mDataEditor = new DataEditor(this);

	//全体のレイアウト
	mMainWidget = new QWidget(this);

	//左のコマンドリスト
	mCommandList = new QTreeWidget(this);
	//mCommandList->setMinimumWidth( 180 );
  //mCommandList->headerItem()->setText(0,"コマンドリスト");
  mCommandList->header()->setVisible(false);
  mCommandList->header()->setMinimumWidth(400);
  mCommandList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  mCommandList->setIndentation(8);

	//中央スクロールエリア
	mBarArea = new QScrollArea();
	mBarArea->setFrameStyle(0);
	mBarArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	mBarArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	mBarAreaWidget = new QWidget();

	//スクロールエリア内部
	mBarLyt = new QVBoxLayout();
  mBarLyt->setSpacing(0);
  mBarLyt->setMargin(0);

	mBarAreaLyt = new QVBoxLayout();
  mBarAreaLyt->setSpacing(0);
  mBarAreaLyt->setMargin(0);
	mBarAreaLyt->setSizeConstraint(QLayout::SetFixedSize);

	//上段ラベル
	mBarLabel = new WidSequenceLabel(mMainWidget);

	//左のコマンドタイトル表示
	mTitleArea = new QScrollArea();
	mTitleArea->setFrameStyle(0);
	mTitleArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	mTitleArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	mTitleArea->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  mTitleArea->setMinimumWidth(SeqEditor::GetTitleAreaSize());
	mTitleAreaWidget = new QWidget();
	mTitleAreaLyt = new QVBoxLayout();
  mTitleAreaLyt->setSpacing(0);
  mTitleAreaLyt->setContentsMargins(0,0,0,18);  //メインエリアにはスクロールバーがあるので、その分の余白
	mTitleAreaLyt->setSizeConstraint(QLayout::SetFixedSize);

  mTitleAreaWidget->setLayout(mTitleAreaLyt);
	mTitleArea->setWidget(mTitleAreaWidget);

	//下段、フレームラベル
	mScaleMaxWidget = new WidNumBox(this);
	mScaleMinWidget = new WidNumBox(this);
	mScaleNowWidget = new WidNumBox(this);
	mFrameScaleWidget = new WidFrameScale(mDataEditor,this);
	mFrameScaleLyt = new QGridLayout();

	mFrameScaleLyt->addWidget( mScaleNowWidget , 0,0,1,1 );
	mFrameScaleLyt->addWidget( mScaleMinWidget , 1,0,1,1 );
	mFrameScaleLyt->addWidget( mFrameScaleWidget , 0,1,2,1 );
	mFrameScaleLyt->addWidget( mScaleMaxWidget , 1,2,1,1 );

	//最下段　ヘルプウィン
	mHelpWin = new QTextEdit(this);
	mHelpWin->setReadOnly(true);
	mHelpWin->setMaximumHeight(96);
  //@todo 表示テスト
	QString tmpText("<b>テスト！</b>\n<i>テスト２</i>\n<font size=24 color=red>テスト３</font>\nあ\nあ\nあ\nあ\nあ\nあ\nあ\nあ");
	tmpText.replace("\n","<br>");
	mHelpWin->setText(tmpText );
  
  //ステータスバー
  mStatusLabel =new QLabel();
  mStatusLabel->setIndent(3);
  SetStatusBarMessage("ようこそSequenceEditorへ！");
  statusBar()->addWidget(mStatusLabel);


	//シグナルの接続
	//目盛りとメインウィンドウの水平スクロール
	QScrollBar *hbar = mBarArea->horizontalScrollBar();
	connect( hbar , SIGNAL(valueChanged(int)) , mBarLabel , SLOT(UpdateScroll(int)) );
	connect( hbar , SIGNAL(valueChanged(int)) , this , SLOT(UpdateHScrollDraw()) );	
	QScrollBar *vbar = mBarArea->verticalScrollBar();
	QScrollBar *vbar2 = mTitleArea->verticalScrollBar();
	connect( vbar , SIGNAL(valueChanged(int)) , vbar2 , SLOT(setValue(int)) );
	connect( vbar2 , SIGNAL(valueChanged(int)) , vbar , SLOT(setValue(int)) );


	//下段　フレームラベルの接続
	//ラベル→NumBox
	connect( mFrameScaleWidget , SIGNAL(ChangeMaxScale(int)) , 
					 mScaleMaxWidget , SLOT(setValue(int)) );
	connect( mFrameScaleWidget , SIGNAL(ChangeMinScale(int)) , 
					 mScaleMinWidget , SLOT(setValue(int)) );
	connect( mFrameScaleWidget , SIGNAL(ChangeNowScale(int)) , 
					 mScaleNowWidget , SLOT(setValue(int)) );
	//NumBox→ラベル
	connect( mScaleMaxWidget , SIGNAL(FinishInput(int)) , 
					 mFrameScaleWidget , SLOT(SetMaxScale(int)) );
	connect( mScaleMinWidget , SIGNAL(FinishInput(int)) , 
					 mFrameScaleWidget , SLOT(SetMinScale(int)) );
	connect( mScaleNowWidget , SIGNAL(FinishInput(int)) , 
					 mFrameScaleWidget , SLOT(SetNowScale(int)) );

	//DataEditorとの接続
	//NowFrame
	connect( mFrameScaleWidget , SIGNAL(ChangeNowScale(int)) , 
					 mDataEditor , SLOT(SetNowFrame(int)) );
	connect( mDataEditor , SIGNAL(UpdateNowFrame(int)) , 
					 mFrameScaleWidget , SLOT(SetNowScale(int)) );

	//水平スクロールの稼動幅制御
	connect( hbar , SIGNAL(rangeChanged(int,int)) , 
					 this , SLOT(UpdateHScrollBarAdjust(int,int)) );
	connect( mFrameScaleWidget , SIGNAL(ChangeMaxScale(int)) , 
					 this , SLOT(UpdateHScrollBarAdjust()) );
	connect( mFrameScaleWidget , SIGNAL(ChangeMinScale(int)) , 
					 this , SLOT(UpdateHScrollBarAdjust()) );

	//水平スクロールの現在値制御
	connect( mDataEditor , SIGNAL(UpdateNowFrame(int)) , 
					 this , SLOT(UpdateHScrollNowPos(int)) );

  //コマンドリストからの追加処理
	connect( mCommandList , SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)) , 
					 mDataEditor , SLOT(AddItemToCommandList(QTreeWidgetItem*,int)) );
	connect( mCommandList , SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)) , 
					 this , SLOT(CommandListChangeItem(QTreeWidgetItem*,QTreeWidgetItem*)) );
  

	//各種接続
	mBarAreaLyt->addLayout(mBarLyt);
	mBarAreaLyt->addStretch();
	mBarAreaWidget->setLayout(mBarAreaLyt);
	mBarArea->setWidget(mBarAreaWidget);
	
	//mMainLyt->addWidget(mCommandList , 0 , 0 , 3 , 1 );
	//mMainLyt->addWidget(mTitleArea , 1 , 1 , 1 , 1 );
	//mMainLyt->addWidget(mBarLabel , 0 , 2 , 1 , 1 );
	//mMainLyt->addWidget(mBarArea , 1 , 2 , 1 , 1 );
	//mMainLyt->addLayout(mFrameScaleLyt , 2 , 1 , 1 , 2 );
	//mMainLyt->addWidget(mHelpWin , 3 , 0 , 1 , 3 );

  mMainLytUp = new QSplitter(Qt::Horizontal);
  QWidget *widget = new QWidget(mMainLytUp);

  mMainLyt = new QGridLayout(widget);
  mMainLyt->setSpacing(0);
	mMainLyt->addWidget(mTitleArea    , 1 , 0 , 1 , 1 );
	mMainLyt->addWidget(mBarLabel     , 0 , 1 , 1 , 1 );
	mMainLyt->addWidget(mBarArea      , 1 , 1 , 1 , 1 );
	mMainLyt->addLayout(mFrameScaleLyt, 2 , 0 , 1 , 2 );

  mMainLytUp->addWidget( mCommandList );
  mMainLytUp->addWidget( widget );
  mMainLytUp->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

	QVBoxLayout *mainLytAll = new QVBoxLayout();
  mainLytAll->addWidget( mMainLytUp );
  mainLytAll->addWidget( mHelpWin );
  mainLytAll->setSpacing(0);
	mainLytAll->setSizeConstraint(QLayout::SetNoConstraint);

  mMainWidget->setLayout(mainLytAll);

  //mBarArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  //mBarArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setCentralWidget( mMainWidget );

	//初期設定
	mFrameScaleWidget->SetMinScale(0);
	mFrameScaleWidget->SetMaxScale(60);

  LoadSetting();

  // アクションメニュー作成
  CreateFileMenu( );
  CreateEditMenu( );

	//各アクションの作成
	mDataEditor->CreateAction();

  // アクションメニュー作成2
	CreateGroupMenu( );

  //命令リスト作成
  mDataEditor->LoadMacroList(mCommandFilePath);
  
  setAcceptDrops(true);
  mDataEditor->SetCurrentCommand(NULL);
  mDataEditor->SetCurrentGroup(NULL);


	/*
  QScrollArea *area = new QScrollArea();
  QWidget *areaWidget = new QWidget(this);

  QVBoxLayout *layout = new QVBoxLayout(area);
  layout->setSpacing(0);

  WidSequenceLabel *seqLb = new WidSequenceLabel(area);
  layout->addWidget(seqLb);

  WidSequenceGroup *seqBar = new WidSequenceGroup(area);
  layout->addWidget(seqBar);

  for( int i=0;i<20;i++ )
  {
    WidSequenceBar *bar = new WidSequenceBar(area);
    layout->addWidget(bar);
  }
  layout->addStretch();
//  area->setLayout(layout);
  areaWidget->setLayout( layout );
  area->setWidget(areaWidget);
  //area->resize( QSize(1024,1024) );
  //area->setWidgetResizable(true);
  area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setCentralWidget( area );
*/
  //@todo 仮でコマンド追加
	for( int j=0;j<3;j++ )
	{
//		DataGroup *group1 = mDataEditor->AddGroup();
//		group1->SetTitleStr( "group" );
		
		
  /*
		for( int i=0;i<5;i++ )
		{
			DataCommand *com = group1->AddCommand();
			com->SetFrame( gfl::core::Math::Random(30) , gfl::core::Math::Random(30) );
      DataValue *val = com->AddDataValue( SDT_INT );
      val->SetValueInt( gfl::core::Math::Random(256) );
			mTitleAreaWidget->update();
		}
  */
	}
}

void MainWindow::UpdateHScrollBarAdjust(int min , int max )
{
	static const int OFS = 8;

	QScrollBar *hbar = mBarArea->horizontalScrollBar();
	QScrollBar *vbar = mBarArea->verticalScrollBar();
	int barMax = mFrameScaleWidget->GetMaxScale() * SeqEditor::GetFrameWidth();
	int barMin = mFrameScaleWidget->GetMinScale() * SeqEditor::GetFrameWidth();
	barMax -= (mBarArea->size().width() - vbar->size().width() - OFS*2 );
	barMin -= OFS;
	if( barMin > barMax )
	{
		barMax = barMin;
	}

	if( barMax != max || barMin != min )
	{
		hbar->setMaximum( barMax );
		hbar->setMinimum( barMin );

	}
}

void MainWindow::UpdateHScrollNowPos(int val)
{
	QScrollBar *hbar = mBarArea->horizontalScrollBar();
	QScrollBar *vbar = mBarArea->verticalScrollBar();

	int nowPos = mDataEditor->GetNowFrame() * SeqEditor::GetFrameWidth();

	nowPos -= (mBarArea->size().width() - vbar->size().width())/2;

	hbar->setValue( nowPos );
  UpdateHScrollDraw();
}

void MainWindow::UpdateHScrollDraw()
{
	mBarArea->repaint(mBarArea->rect());
}

void MainWindow::CommandListChangeItem(QTreeWidgetItem* item,QTreeWidgetItem* prevItem)
{
  if( item )
  {
    gfl::xml::Node *macroNode = mDataEditor->GetMacroData()->GetMacroNode( item->data(0,Qt::UserRole).toInt() );

    if( macroNode )
    {
      //@todo 表示テスト
      QString tmpText(macroNode->AttributeString("comment"));
	    //tmpText.replace("\n","<br>");
      tmpText.replace("{","<");
      tmpText.replace("}",">");
	    mHelpWin->setText(tmpText );
    }
  }

}

void MainWindow::keyPressEvent( QKeyEvent *ev )
{
	static DataGroup *tmpGroup = NULL;
  mDataEditor->SetKeyHoldState( ev->modifiers() );
  gfl::core::Debug::PrintConsole("[%x][%d]!\n",ev->key(),ev->modifiers());
	if( ev->key() == Qt::Key_W ||
      ev->key() == Qt::Key_Comma )
	{
    DataCommand *com = mDataEditor->GetCurrentCommand();
    DataGroup   *grp = mDataEditor->GetCurrentGroup();
    if( grp )
    {
      if( com )
      {
        mDataEditor->MoveCommandUp( com );
        mDataEditor->SetCurrentCommand( com );
      }
      else
      {
        mDataEditor->MoveGroupUp( grp );
        mDataEditor->SetCurrentGroup( grp );
      }
    }
	}
  else
	if( ev->key() == Qt::Key_S ||
      ev->key() == Qt::Key_Period )
	{
    DataCommand *com = mDataEditor->GetCurrentCommand();
    DataGroup   *grp = mDataEditor->GetCurrentGroup();
    if( grp )
    {
      if( com )
      {
        mDataEditor->MoveCommandDown( com );
        mDataEditor->SetCurrentCommand( com );
      }
      else
      {
        mDataEditor->MoveGroupDown( grp );
        mDataEditor->SetCurrentGroup( grp );
      }
    }
	}
  else
	if( ev->key() == Qt::Key_D || ev->key() == Qt::Key_A ||
      ev->key() == Qt::Key_At || ev->key() == Qt::Key_BracketLeft||       //開始フレームのみ＋
      ev->key() == Qt::Key_Colon || ev->key() == Qt::Key_BracketRight ||  //終了フレームのみ＋
      ev->key() == Qt::Key_QuoteLeft || ev->key() == Qt::Key_BraceLeft || //開始フレームのみ＋1(キーが特殊でShift併用すると別のキーになる
      ev->key() == Qt::Key_Asterisk || ev->key() == Qt::Key_BraceRight)   //終了フレームのみ＋1(キーが特殊でShift併用すると別のキーになる
  {
    DataCommand *com = mDataEditor->GetCurrentCommand();
    DataGroup   *grp = mDataEditor->GetCurrentGroup();
    
    int sDiff = 0;
    int eDiff = 0;
    switch( ev->key() )
    {
    case Qt::Key_D:
      sDiff = 1;
      eDiff = 1;
      break;
    case Qt::Key_A:
      sDiff = -1;
      eDiff = -1;
      break;
    case Qt::Key_BracketLeft:
    case Qt::Key_BraceLeft:   //Shift併用で別キーになる
      sDiff = 1;
      break;
    case Qt::Key_At:
    case Qt::Key_QuoteLeft:   //Shift併用で別キーになる
      sDiff = -1;
      break;
    case Qt::Key_BracketRight:
    case Qt::Key_BraceRight:   //Shift併用で別キーになる
      eDiff = 1;
      break;
    case Qt::Key_Colon:
    case Qt::Key_Asterisk:   //Shift併用で別キーになる
      eDiff = -1;
      break;
    }
    
    if( mDataEditor->GetKeyHoldState() & Qt::SHIFT )
    {
      sDiff *= 5;
      eDiff *= 5;
    }

    if( grp )
    {
      if( grp->IsLock() )
      {
        return;
      }
      if( com )
      {
        const int sFrame = com->GetStartFrame(); 
        const int eFrame = com->GetEndFrame(); 
        com->SetFrame(sFrame+sDiff,eFrame+eDiff);

        mDataEditor->GetActionManager()->StartSetAction();
        mDataEditor->GetActionManager()->ActionChangeCommandFrame( com , sFrame , eFrame );
        mDataEditor->GetActionManager()->EndSetAction();
      }
      else
      {
        DataComList *comList = grp->GetComList();
        DataComListIt it = comList->begin();

        mDataEditor->GetActionManager()->StartSetAction();
        while( it != comList->end() )
        {
          DataCommand *com = (*it);

          const int sFrame = com->GetStartFrame(); 
          const int eFrame = com->GetEndFrame(); 
          com->SetFrame(sFrame+sDiff,eFrame+eDiff);
          mDataEditor->GetActionManager()->ActionChangeCommandFrame( com , sFrame , eFrame );

          it++;
        }
        mDataEditor->GetActionManager()->EndSetAction();
      }
    }
  }
  else
	if( ev->key() == Qt::Key_E )
	{
    DataCommand *com = mDataEditor->GetCurrentCommand();
    DataGroup   *grp = mDataEditor->GetCurrentGroup();
    if( grp )
    {
      if( grp->IsLock() )
      {
        return;
      }
      if( com )
      {
        com->SetActive( !com->GetActive() );
      }
      else
      {
        bool isFirst = true;
        bool isActive = false;

        DataComList* comList = grp->GetCommandList();
        DataComListIt it = comList->begin();
        while( it != comList->end() )
        {
          DataCommand *com = (*it);
          if( isFirst )
          {
            isFirst = false;
            isActive = !com->GetActive();
          }
          com->SetActive(isActive);
          it++;
        }
      }
      grp->GetTitleGroup()->update();
    }
	}
  else
	if( ev->key() == Qt::Key_Escape )
	{
    mDataEditor->SetCurrentCommand(NULL);
    mDataEditor->SetCurrentGroup(NULL);
  }
  else
	if( ev->key() == Qt::Key_1 )
	{
    SetDispSizeS();
  }
  else
	if( ev->key() == Qt::Key_2 )
	{
    SetDispSizeM();
  }
  else
	if( ev->key() == Qt::Key_3 )
	{
    SetDispSizeL();
  }
  else
  if( ev->key() == Qt::Key_L )
  {
    mDataEditor->ActLockFlipCurrent();
  }
  else
  {
    QMainWindow::keyPressEvent(ev);
  }
/*
	if( ev->key() == Qt::Key_B )
	{
    mDataEditor->ActAddGroup();
		DialogAddGroup dialog(this);

		int ret = dialog.exec();
		if( ret == QDialog::Accepted )
		{
			DataGroup *group = mDataEditor->AddGroup();
			group->SetTitleStr( dialog.lineEdit->text() );

      gfl::core::Debug::PrintConsole("add2[%s]!\n",dialog.lineEdit->text().toLocal8Bit().constData());

			tmpGroup = group;
		}
	}
*/

/*
  if( ev->key() == Qt::Key_C )
	{
		if( tmpGroup != NULL )
		{
			DataCommand *com = tmpGroup->AddCommand();
			com->SetFrame( gfl::core::Math::Random(30) , gfl::core::Math::Random(30) );
			
			mTitleAreaWidget->update();
			gfl::core::Debug::PrintConsole("add3!\n");
		}
	}

	if( ev->key() == Qt::Key_D )
	{
		QScrollBar *hbar = mBarArea->horizontalScrollBar();
		hbar->setMaximum( 160 );
		hbar->setMinimum( 0 );
	}
*/
}

void MainWindow::keyReleaseEvent( QKeyEvent *ev )
{
  mDataEditor->SetKeyHoldState( ev->modifiers() );
  QMainWindow::keyReleaseEvent(ev);
}

void MainWindow::wheelEvent( QWheelEvent * ev )
{
  gfl::core::Debug::PrintConsole("[%d][%d]!\n",ev->delta(),ev->globalY());
  //deltaがホイール
}

void MainWindow::dragEnterEvent( QDragEnterEvent *event )
{
  if( event->mimeData()->hasFormat("text/uri-list"))
  {
    event->acceptProposedAction();
  }
}

void MainWindow::dropEvent( QDropEvent *event )
{
  QList<QUrl> urls = event->mimeData()->urls();
  if( urls.isEmpty() )
  {
    return;
  }

  QString fileName = urls.first().toLocalFile();
  if( fileName.isEmpty() )
  {
    return;
  }

  bool ret = LoadFile(fileName);
  if( ret )
  {
    m_CurrentFileName = fileName;
    QFileInfo fileInfo( fileName );
    setWindowTitle( tr("%1[*] - %2").arg( fileInfo.completeBaseName()).arg(tr("SequenceEditor")));
  }
}

void MainWindow::closeEvent( QCloseEvent *event )
{
  if( CloseConfirm() )
  {
    QMainWindow::closeEvent(event);
    SaveSetting();
  }
  else
  {
    event->ignore();
  }
}

//==============================================================================
//==============================================================================
void MainWindow::CreateFileMenu( void )
{
	QAction* newAct = new QAction( QIcon( ":new.png" ), tr( "新規作成(&N)" ), this );
  //newAct->setShortcuts( QKeySequence::New );
	newAct->setShortcut( tr("Ctrl+N") );
	newAct->setStatusTip( tr( "Create a new file" ) );
	connect( newAct, SIGNAL( triggered( ) ), this, SLOT( newFile( ) ) );

	QAction* openAct = new QAction( QIcon( ":open.png" ), tr( "開く(&O)" ), this );
	openAct->setShortcuts( QKeySequence::Open );
	openAct->setStatusTip( tr( "Open an existing file" ) );
	connect( openAct, SIGNAL( triggered( ) ), this, SLOT( open( ) ) );

	QAction* saveAct = new QAction( QIcon( ":save.png" ), tr( "上書き保存(&S)" ), this );
	saveAct->setShortcuts( QKeySequence::Save );
	saveAct->setStatusTip( tr( "Save the document to disk" ) );
	connect( saveAct, SIGNAL( triggered( ) ), this, SLOT( save( ) ) );

	QAction* saveAsAct = new QAction( QIcon( ":saveas.png" ), tr( "名前をつけて保存(&A)" ), this );
	//saveAsAct->setShortcuts( QKeySequence::SaveAs );
	saveAsAct->setShortcut( tr("Shift+Ctrl+S") );
	saveAsAct->setStatusTip( tr( "Save the document under a new name" ) );
	connect( saveAsAct, SIGNAL( triggered( ) ), this, SLOT( saveAs( ) ) );

	QAction* exitAct = new QAction( QIcon( ":exit.png" ), tr( "終了(&X)" ), this );
	//	exitAct->setShortcuts( QKeySequence::Quit ); // 4.5 does not have ...
	exitAct->setShortcut( tr("Alt+F4") );
	exitAct->setStatusTip( tr( "Exit the application" ) );
	connect( exitAct, SIGNAL( triggered( ) ), this, SLOT( close( ) ) );

	QAction* convertAct = new QAction( QIcon( ":convert.png" ), tr( "バイナリコンバート(&C)" ), this );
	convertAct->setShortcut( tr("F5") );
	convertAct->setStatusTip( tr( "Convert binary file" ) );
	connect( convertAct, SIGNAL( triggered( ) ), this, SLOT( ConvertBinary( ) ) );

	// メニューに追加
	QMenu* fileMenu = menuBar( )->addMenu( tr( "ファイル(&F)" ) );
	fileMenu->addAction( newAct );
	fileMenu->addAction( openAct );
	fileMenu->addAction( saveAct );
	fileMenu->addAction( saveAsAct );
	fileMenu->addAction( exitAct );
  fileMenu->addSeparator();
	fileMenu->addAction( convertAct );

  QToolBar* fileTool = addToolBar( tr( "ファイル(&F)" ) );
  fileTool->setIconSize( QSize(16,16) );
	fileTool->addAction( newAct );
	fileTool->addAction( openAct );
	fileTool->addAction( saveAct );
	fileTool->addAction( saveAsAct );
	fileTool->addAction( exitAct );
  fileTool->addSeparator();
	fileTool->addAction( convertAct );

#if 0 // 
	// ツールバーに追加
	QToolBar* fileToolBar = addToolBar( tr( "File" ) );
	fileToolBar->addAction( newAct );
	fileToolBar->addAction( openAct );
	fileToolBar->addAction( saveAct );
#endif
}

//==============================================================================
//==============================================================================
void MainWindow::CreateEditMenu( void )
{
	QAction* undoAct = new QAction( QIcon( ":undo.png" ), tr( "元に戻す(&U)" ), this );
  undoAct->setShortcuts( QKeySequence::Undo );
	undoAct->setStatusTip( tr( "Undo action." ) );
	connect( undoAct, SIGNAL( triggered( ) ), this, SLOT( undo( ) ) );
  mActUndo = undoAct;

  QAction* redoAct = new QAction( QIcon( ":redo.png" ), tr( "やり直し(&U)" ), this );
  //redoAct->setShortcuts( QKeySequence::Redo );
	redoAct->setShortcut( tr("Shift+Ctrl+Z") );
	redoAct->setStatusTip( tr( "Redo action." ) );
	connect( redoAct, SIGNAL( triggered( ) ), this, SLOT( redo( ) ) );
  mActRedo = redoAct;

	QAction* cutAct = new QAction( QIcon( ":cut.png" ), tr( "切り取り(&T)" ), this );
	cutAct->setShortcuts( QKeySequence::Cut );
	cutAct->setStatusTip( tr( "Cut the current selection's contents to the "
							 "clipboard" ) );
	connect( cutAct, SIGNAL( triggered( ) ), this, SLOT( cut( ) ) );

  QAction* copyAct = new QAction( QIcon( ":copy.png" ), tr( "コピー(&C)" ), this );
	copyAct->setShortcuts( QKeySequence::Copy );
	copyAct->setStatusTip( tr( "Copy the current selection's contents to the "
							 "clipboard" ) );
	connect( copyAct, SIGNAL( triggered( ) ), this, SLOT( copy( ) ) );

	QAction* pasteAct = new QAction( QIcon( ":paste.png" ), tr( "貼り付け(&P)" ), this );
	pasteAct->setShortcuts( QKeySequence::Paste );
	pasteAct->setStatusTip( tr( "Paste the clipboard's contents into the current "
							 "selection" ) );
	connect( pasteAct, SIGNAL( triggered( ) ), this, SLOT( paste( ) ) );

	// メニューに追加
	QMenu* editMenu = menuBar( )->addMenu( tr( "編集(&E)" ) );
	editMenu->addAction( undoAct );
	editMenu->addAction( redoAct );
  editMenu->addSeparator();
	editMenu->addAction( cutAct );
	editMenu->addAction( copyAct );
	editMenu->addAction( pasteAct );

  QToolBar* editTool = addToolBar( tr( "編集(&E)" ) );
  editTool->setIconSize( QSize(16,16) );
	editTool->addAction( undoAct );
	editTool->addAction( redoAct );
  editTool->addSeparator();
	editTool->addAction( cutAct );
	editTool->addAction( copyAct );
	editTool->addAction( pasteAct );
  
  //表示系
	mActDispS = new QAction( QIcon( ":size_s.png" ), tr( "表示サイズS" ), this );
	mActDispS->setStatusTip("表示サイズをSにせってします。");
	connect( mActDispS, SIGNAL( triggered( ) ), this, SLOT( SetDispSizeS( ) ) );

	mActDispM = new QAction( QIcon( ":size_m.png" ), tr( "表示サイズM" ), this );
	mActDispM->setStatusTip("表示サイズをMにせってします。");
	connect( mActDispM, SIGNAL( triggered( ) ), this, SLOT( SetDispSizeM( ) ) );

	mActDispL = new QAction( QIcon( ":size_l.png" ), tr( "表示サイズL" ), this );
	mActDispL->setStatusTip("表示サイズをLにせってします。");
	connect( mActDispL, SIGNAL( triggered( ) ), this, SLOT( SetDispSizeL( ) ) );

	QMenu* dispMenu = menuBar( )->addMenu( tr( "表示(&V)" ) );
	dispMenu->addAction( mActDispS );
	dispMenu->addAction( mActDispM );
	dispMenu->addAction( mActDispL );

  //表示系
  mActDefaultLock = new QAction( tr( "デフォルトロック" ), this );
  mActDefaultLock->setStatusTip("ロード時にグループロックをデフォルトで行います。");
  mActDefaultLock->setCheckable(true);
  if( SeqEditor::GetIsDefaultLock())
  {
    mActDefaultLock->setChecked(true);
  }
  else
  {
    mActDefaultLock->setChecked(false);
  }
  ApplyDefaultLock();
  connect( mActDefaultLock, SIGNAL( triggered( ) ), this, SLOT( ApplyDefaultLock() ) );

#if 0 // 
	// ツールバーに追加
	menuBar( )->addSeparator( );

	QToolBar* editToolBar = addToolBar( tr( "&Edit" ) );
	editToolBar->addAction( cutAct );
	editToolBar->addAction( copyAct );
	editToolBar->addAction( pasteAct );
#endif
}


void MainWindow::CreateGroupMenu( void )
{
	QMenu* editMenu = menuBar( )->addMenu( tr( "グループ(&G)" ) );
  editMenu->addAction( mDataEditor->GetActOpenGroup() );
	editMenu->addAction( mDataEditor->GetActCloseGroup() );
	editMenu->addAction( mDataEditor->GetActAddGroup() );
	editMenu->addAction( mDataEditor->GetActDeleteGroup() );
  editMenu->addAction( mDataEditor->GetActOpenAllGroup() );
	editMenu->addAction( mDataEditor->GetActCloseAllGroup() );
  editMenu->addAction( mDataEditor->GetActOpenCloseGroup() );

	QMenu* frameMenu = menuBar( )->addMenu( tr( "フレーム(&R)" ) );
  frameMenu->addAction( mDataEditor->GetActAddFrameAddAll() );
	frameMenu->addAction( mDataEditor->GetActAddFrameDelAll() );

  QMenu* lockMenu = menuBar( )->addMenu( tr( "ロック(&L)" ) );
  lockMenu->addAction( mDataEditor->GetActLockOn() );
  lockMenu->addAction( mDataEditor->GetActLockOff() );
  lockMenu->addAction( mDataEditor->GetActLockOnAll() );
  lockMenu->addAction( mDataEditor->GetActLockOffAll() );
  lockMenu->addAction( mActDefaultLock );

  QToolBar* editGroup = addToolBar( tr( "グループ(&G)" ) );
	editGroup->addAction( mDataEditor->GetActAddGroup() );
	editGroup->addAction( mDataEditor->GetActDeleteGroup() );
  editGroup->addAction( mDataEditor->GetActOpenGroup() );
	editGroup->addAction( mDataEditor->GetActCloseGroup() );
	editGroup->addAction( mDataEditor->GetActMoveGroupUp() );
	editGroup->addAction( mDataEditor->GetActMoveGroupDown() );
  editGroup->addAction( mDataEditor->GetActOpenAllGroup() );
	editGroup->addAction( mDataEditor->GetActCloseAllGroup() );
  editGroup->setIconSize( QSize(16,16) );

  QToolBar* editComGroup = addToolBar( tr( "コマンド(&C)" ) );
  editComGroup->addAction( mDataEditor->GetActEditCommand() );
	editComGroup->addAction( mDataEditor->GetActDeleteCommand() );
  editComGroup->addAction( mDataEditor->GetActMoveCommandUp() );
	editComGroup->addAction( mDataEditor->GetActMoveCommandDown() );
  editComGroup->setIconSize( QSize(16,16) );

  QToolBar* frameGroup = addToolBar( tr( "フレーム(&F)" ) );
  frameGroup->addAction( mDataEditor->GetActAddFrameAddAll() );
	frameGroup->addAction( mDataEditor->GetActAddFrameDelAll() );
  frameGroup->setIconSize( QSize(16,16) );

  QToolBar* dispGroup = addToolBar( tr( "表示(&D)" ) );
  dispGroup->addAction( mActDispS );
	dispGroup->addAction( mActDispM );
	dispGroup->addAction( mActDispL );
  dispGroup->setIconSize( QSize(16,16) );

  QToolBar* lockGroup = addToolBar( tr( "ロック(&L)" ) );
  lockGroup->addAction( mDataEditor->GetActLockOn() );
  lockGroup->addAction( mDataEditor->GetActLockOff() );
  lockGroup->addAction( mDataEditor->GetActLockOnAll() );
  lockGroup->addAction( mDataEditor->GetActLockOffAll() );

  //QComboBox* cb = new QComboBox;
	//dispGroup->addWidget( cb );

  //ヘルプメニュー
	QAction* aboutQt = new QAction( tr( "About Qt" ), this );
	connect( aboutQt, SIGNAL( triggered( ) ), qApp, SLOT( aboutQt( ) ) );

	QAction* aboutSeq = new QAction( tr( "About SequenceEditor" ), this );
	connect( aboutSeq, SIGNAL( triggered( ) ), this, SLOT( about( ) ) );

  QMenu* aboutMenu = menuBar( )->addMenu( tr( "ヘルプ(&H)" ) );
	aboutMenu->addAction( aboutQt );
	aboutMenu->addAction( aboutSeq );
}

void MainWindow::PopLockWarnning(int type)
{
  QMessageBox msgBox;
  int rand = qrand()%300;
  msgBox.setText("ロックしています。");
#if 1
  switch(rand)
  {
  case 0:
    msgBox.setIconPixmap(QPixmap( ":lock_1.png" ));
    break;
  case 1:
    msgBox.setIconPixmap(QPixmap( ":lock_2.png" ));
    break;
  case 2:
    msgBox.setIconPixmap(QPixmap( ":lock_3.png" ));
    break;
  default:
    msgBox.setIconPixmap(QPixmap( ":lock_on.png" ));
    break;
  }
#else
  msgBox.setIconPixmap(QPixmap( ":lock_on.png" ));
#endif
  msgBox.exec();

  //QMessageBox::warning( this , "警告" , "ロックしています。" , QMessageBox::Default );
}

//------------------------------------------------------------------------------
//セーブ機能部
//------------------------------------------------------------------------------
bool MainWindow::SaveFile(const QString& fname)
{
  mDataEditor->GetXmlData()->Save("temp.xml");

  CreateSeqXmlFile( mDataEditor->GetXmlData() );

  m_CurrentFileName = fname;

	return mDataEditor->GetXmlData()->Save(fname);
}

//------------------------------------------------------------------------------
//ロード機能部
//------------------------------------------------------------------------------
bool MainWindow::LoadFile(const QString& fname)
{
  if( CloseConfirm() )
  {
    gfl::xml::Document *doc = new gfl::xml::Document();
    mDataEditor->ClearData();
    
    bool ret = doc->Load(fname);
    if( ret )
    {
      mDataEditor->SetXmlData( doc );

      gfl::xml::Node *rootNode = doc->RootNode();
      gfl::xml::Node *topNode = rootNode->FirstNode( XML_NAME_MAIN_NODE );

      if( topNode )
      {
        char *str = topNode->AttributeString( XML_NAME_MAIN_ATTR_CMDPATH , NULL );
        if( str )
        {
#if 0
          //文字列の比較がうまく行っていないので保留
          if( mCommandFilePath != str )
          {
            QString str;
            str.sprintf("コマンド定義ファイルが違います。読み直しますか？\n今[%s]\n新[%s]",mCommandFilePath.toLocal8Bit().constData(),str);
            int mesRet = QMessageBox::warning( this , "警告" , str , 
                                            QMessageBox::Yes | QMessageBox::Default,
                                            QMessageBox::No );
            if( mesRet == QMessageBox::Yes )
            {
              mCommandFilePath = str;
              mDataEditor->LoadMacroList(mCommandFilePath);
            }
          }
#else
        mCommandFilePath = str;
        mDataEditor->LoadMacroList(mCommandFilePath);
#endif
        }

        //コマンド部分のロード
        mDataEditor->LoadCommandData( topNode );
      }
    }
    else
    {
      delete doc;
    }
    return ret;
  }
  return false;
}

void MainWindow::CreateSeqXmlFile(gfl::xml::Document *doc )
{
  gfl::xml::Node *rootNode = doc->RootNode();
  gfl::xml::Node *topNode = rootNode->FirstNode( XML_NAME_MAIN_NODE );
  
  QFileInfo fileInfo( mCommandFilePath );

  char *str = topNode->AttributeString( XML_NAME_MAIN_ATTR_CMDPATH , NULL );
  if( str == NULL )
  {
    topNode->AddAttribute( XML_NAME_MAIN_ATTR_CMDPATH , fileInfo.absoluteFilePath() );
  }

  //コマンド部分のセーブ
  mDataEditor->SaveCommandData( topNode );
}

bool MainWindow::CloseConfirm(void)
{
  int mesRet = QMessageBox::warning( this , "警告" , "現在の編集を破棄してもよろしいですか？" , 
                                  QMessageBox::Yes | QMessageBox::Default,
                                  QMessageBox::No );
  if( mesRet == QMessageBox::Yes )
  {
    return true;
  }
  return false;
}

void MainWindow::SetStatusBarMessage( char* msg )
{
  mStatusLabel->setText(msg);
  QTimer::singleShot(2000, this, SLOT(ResetStatusBar()));
}


//==============================================================================
// スロット群
//==============================================================================
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MainWindow::newFile( )
{
  if( CloseConfirm() )
  {
    mDataEditor->ClearData();
    m_CurrentFileName.clear();

    setWindowTitle( tr("%1[*] - %2").arg( tr("無題")).arg(tr("SequenceEditor")));
  }
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MainWindow::open( )
{
	QString fileName = QFileDialog::getOpenFileName( this, "open seq file", QDir::currentPath( ), "SeqFile (*"SEQ_EXT")" );
	if( !fileName.isEmpty( ) )
  {
    bool ret = LoadFile(fileName);
    if( ret )
    {
      m_CurrentFileName = fileName;
      QFileInfo fileInfo( fileName );
      setWindowTitle( tr("%1[*] - %2").arg( fileInfo.completeBaseName()).arg(tr("SequenceEditor")));
    }
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool MainWindow::save( )
{
	if( m_CurrentFileName.isEmpty( ) )
  {
		return saveAs();
  }

  return SaveFile(m_CurrentFileName);
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool MainWindow::saveAs( )
{
	QString fileName = QFileDialog::getSaveFileName( this, "save seq file", QDir::currentPath( ), "SeqFile (*"SEQ_EXT")" );
	if( fileName.isEmpty( ) )
		return false;
  
  QFileInfo fileInfo( fileName );
  setWindowTitle( tr("%1[*] - %2").arg( fileInfo.completeBaseName()).arg(tr("SequenceEditor")));
	return SaveFile(fileName);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MainWindow::undo( )
{
  mDataEditor->UndoFunc();
  SetStatusBarMessage("取り消しをしました。");
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MainWindow::redo( )
{
  mDataEditor->RedoFunc();
  SetStatusBarMessage("繰り返しをしました。");
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MainWindow::cut( )
{
  mDataEditor->CutFunc();
  SetStatusBarMessage("切り取りをしました。");
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MainWindow::copy( )
{
  mDataEditor->CopyFunc();
  SetStatusBarMessage("コピーをしました。");
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MainWindow::paste( )
{
  mDataEditor->PasteFunc();
  SetStatusBarMessage("貼り付けをしました。");
}

void MainWindow::SetDispSizeS()
{
  SeqEditor::SetFrameWidth( 4 );
  SeqEditor::SetChipWidth( 4 );
  SeqEditor::SetFrameHeight( 12 );
  SeqEditor::SetChipHeight( 10 );
  update();
  updateGeometry();
  mDataEditor->UpdateDraw();
}

void MainWindow::SetDispSizeM()
{
  SeqEditor::SetFrameWidth( 8 );
  SeqEditor::SetChipWidth( 6 );
  SeqEditor::SetFrameHeight( 20 );
  SeqEditor::SetChipHeight( 16 );
  update();
  updateGeometry();
  mDataEditor->UpdateDraw();
}

void MainWindow::SetDispSizeL()
{
  SeqEditor::SetFrameWidth( 16 );
  SeqEditor::SetChipWidth( 8 );
  SeqEditor::SetFrameHeight( 20 );
  SeqEditor::SetChipHeight( 16 );
  update();
  updateGeometry();
  mDataEditor->UpdateDraw();
}

void MainWindow::ApplyDefaultLock()
{
  if( mActDefaultLock->isChecked())
  {
    SeqEditor::SetIsDefaultLock(true);
    mActDefaultLock->setIcon(QIcon( ":lock_on.png" ));
  }
  else
  {
    SeqEditor::SetIsDefaultLock(false);
    mActDefaultLock->setIcon(QIcon( ":lock_off.png" ));
  }
}

void MainWindow::ConvertBinary()
{
  bool ret;
  
  QFileInfo fileInfo( mCommandFilePath );
  
  char pathName[512];
  strcpy( pathName , fileInfo.absolutePath().toLocal8Bit().constData() );
  strcat( pathName , "/test.bseq" );

  CreateSeqXmlFile( mDataEditor->GetXmlData() );
  ret = SeqConv::ConverSequenceFile(mDataEditor->GetXmlData(),mDataEditor->GetMacroData()->GetMacroDoc(),pathName);
  gfl::core::Debug::PrintConsole("[%d]\n",ret);

  SetStatusBarMessage("バイナリコンバートをしました。");
}

void MainWindow::ResetStatusBar()
{
  mStatusLabel->setText("");
}

#define SETTING_WIN_SIZE "WinGeometry"
#define SETTING_WIN_SPLITTER "WinSplitter"
#define SETTING_FRAME_WIDTH  "FrameWidth"
#define SETTING_FRAME_HEIGHT "FrameHeight"
#define SETTING_CHIP_WIDTH  "ChipWidth"
#define SETTING_CHIP_HEIGHT "ChipHeight"
#define SETTING_DEFAULT_LOCK "DefaultLock"

void MainWindow::LoadSetting()
{
  QSettings setting("GameFreak Inc.","SequenceEditor");
  
  QRect rect = setting.value(SETTING_WIN_SIZE,QRect(200,200,400,400)).toRect();
  move(rect.topLeft());
  resize(rect.size());

  int fw = setting.value(SETTING_FRAME_WIDTH  ,SEQEDIT_FRAME_WIDTH).toInt();
  int fh = setting.value(SETTING_FRAME_HEIGHT ,SEQEDIT_FRAME_HEIGHT).toInt();
  int cw = setting.value(SETTING_CHIP_WIDTH   ,SEQEDIT_CHIP_WIDTH).toInt();
  int ch = setting.value(SETTING_CHIP_HEIGHT  ,SEQEDIT_CHIP_HEIGHT).toInt();

  SeqEditor::SetFrameWidth( fw );
  SeqEditor::SetChipWidth( cw );
  SeqEditor::SetFrameHeight( fh );
  SeqEditor::SetChipHeight( ch );
  
  QByteArray splitSize = setting.value(SETTING_WIN_SPLITTER).toByteArray();
  mMainLytUp->restoreState(splitSize);

  SeqEditor::SetIsDefaultLock( setting.value(SETTING_DEFAULT_LOCK).toBool() );
}

void MainWindow::SaveSetting()
{
  QSettings setting("GameFreak Inc.","SequenceEditor");

  setting.setValue(SETTING_WIN_SIZE,geometry());
  setting.setValue(SETTING_FRAME_WIDTH  ,SeqEditor::GetFrameWidth());
  setting.setValue(SETTING_FRAME_HEIGHT ,SeqEditor::GetFrameHeight());
  setting.setValue(SETTING_CHIP_WIDTH   ,SeqEditor::GetChipWidth());
  setting.setValue(SETTING_CHIP_HEIGHT  ,SeqEditor::GetChipHeight());
  setting.setValue(SETTING_WIN_SPLITTER ,mMainLytUp->saveState());
  setting.setValue(SETTING_DEFAULT_LOCK ,SeqEditor::GetIsDefaultLock());
}

void MainWindow::about()
{
  QMessageBox::about( this , tr("About SequenceEditor") ,
    tr( "<h2>SeqenceEditor 2.0</h2><br>"
        "Copyright &copy; 2013 GameFreak Inc.<br>"
        "Created by Nobuhiko Ariizumi<br>"
        "使い方は<a href=\"http://xy.gamefreak.co.jp/techwiki/index.php?Sequence%20Editor\">コチラ</a><br>"
      )
    );

}


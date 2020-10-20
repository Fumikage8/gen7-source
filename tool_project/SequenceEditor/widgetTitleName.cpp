//==============================================================================
/**
 * @file	widgetTitleName.cpp
 * @brief	コマンドのタイトル部分
 * @author	ariizumi_nobuhiko
 * @data	2011/02/24, 13:20
 */
// =============================================================================

#include <QtGui/QWidget>
#include "mainWindow.h"
#include "dataEditor.h"
#include "dataEditor.h"
#include "dataGroup.h"

WidTitleNameGroup::WidTitleNameGroup(DataEditor *dataEditor , DataGroup *dataGroup , QWidget *parent )
:QWidget(parent)
,mDataGroup(dataGroup)
,mDataEditor(dataEditor)
{
  //setSizePolicy( QSizePolicy::Fixed,QSizePolicy::Minimum);
  //グループ開閉イベント
	connect(  mDataGroup , SIGNAL( ChangedOpenGroup(bool) ) ,
						this , SLOT( SetIsOpen(bool) ) );
}

WidTitleNameGroup::~WidTitleNameGroup()
{

}

void WidTitleNameGroup::GetDrawString(QString *str )
{
  if( mDataGroup->GetOptionStr() != "" )
  {
    gfl::xml::Node *node = mDataEditor->GetMacroData()->GetGroupOptNode()->FirstNode( mDataGroup->GetOptionStr() );
    *str = "[" + QString(node->AttributeString("label_s")) + "]" + mDataGroup->GetTitleStr() + "[" + QString::number(mDataGroup->GetGroupNo()) + "]";
  }
  else
  {
    *str = mDataGroup->GetTitleStr() + "[" + QString::number(mDataGroup->GetGroupNo()) + "]";
  }
  if( mDataGroup->IsLock() )
  {
    *str = *str + "[Lock]";
  }
}

void WidTitleNameGroup::AddCommand( DataCommand *com )
{
	//SizeHint変更通知
	this->updateGeometry();
  this->update();
}

void WidTitleNameGroup::RemoveCommand( DataCommand *com )
{
	//SizeHint変更通知
	this->updateGeometry();
  this->update();
}

void WidTitleNameGroup::SetIsOpen( bool flg )
{
	this->updateGeometry();
  this->update();
}

//継承
void WidTitleNameGroup::paintEvent(QPaintEvent *ev)
{
  bool isFlipColor = false;
  if( mDataEditor->GetCurrentGroup() == mDataGroup &&
    mDataEditor->GetCurrentCommand() == NULL )
  {
    isFlipColor = true;
  }

  //矩形
	QRect drawArea;//ev->rect();
	drawArea.setWidth( sizeHint().width() -1 );
	drawArea.setHeight( sizeHint().height() -1 );
	
	const QFont defFont = font();
	QPainter painter(this);
  if( isFlipColor )
  {
    QColor col;
    col.setRed(255-mDataGroup->GetBgColor().red());
    col.setGreen(255-mDataGroup->GetBgColor().green());
    col.setBlue(255-mDataGroup->GetBgColor().blue());
    painter.fillRect( drawArea , col);
    painter.setPen(QPen(Qt::white));
  }
  else
  {
    painter.fillRect( drawArea , mDataGroup->GetBgColor());
    painter.setPen(QPen(Qt::black));
  }
  painter.drawRect(drawArea);

	//グループ名
	QRect GroupTextArea(0,0,ev->rect().width()-4,SeqEditor::GetFrameHeight());
	QString str;
  GetDrawString(&str);
	
	if( mDataEditor->GetCurrentGroup() == mDataGroup )
	{
		painter.setFont( QFont( defFont.family() , defFont.pointSize() , QFont::Bold ) );
		//painter.font().setBold(true);
	}
  if( mDataGroup->IsDebug() )
  {
    if( isFlipColor )
    {
      painter.setPen(QPen(Qt::cyan));
    }
    else
    {
      painter.setPen(QPen(Qt::darkRed));
    }
  }

	painter.drawText( GroupTextArea , (Qt::AlignRight|Qt::AlignHCenter) , str );
	painter.setFont( defFont );
  if( isFlipColor )
  {
  	painter.setPen(QPen(Qt::white));
  }
  else
  {
    painter.setPen(QPen(Qt::black));
  }

	//コマンド名
	if( mDataGroup->IsOpen() )
	{
		int num = 0;
		QRect ComTextArea(0,0,ev->rect().width()-8,SeqEditor::GetFrameHeight());
		DataComListIt it = mDataGroup->GetCommandList()->begin();
		while( it != mDataGroup->GetCommandList()->end() )
		{
			DataCommand *com = (*it);
      str = com->GetTitleStr();
			ComTextArea.setTop((num+1)*(SeqEditor::GetFrameHeight()));
			ComTextArea.setHeight(SeqEditor::GetFrameHeight());
      
      QFont textFont = defFont;

			if( mDataEditor->GetCurrentCommand() == com )
			{
        textFont.setBold(true);
			}
      if( com->GetActive() == false )
			{
        textFont.setStrikeOut(true);
			}

			painter.setFont( textFont );
			painter.drawText( ComTextArea , (Qt::AlignRight|Qt::AlignHCenter) , str );
			painter.setFont( defFont );
			//painter.font().setBold(false);
			
			num++;
			it++;
		}
	}

}

QSize WidTitleNameGroup::sizeHint() const
{
	if( mDataGroup->IsOpen() )
	{
		return QSize( SeqEditor::GetTitleAreaSize() , (mDataGroup->GetCommandNum() + 1 )*(SeqEditor::GetFrameHeight()) );
	}
	else
	{
		return QSize( SeqEditor::GetTitleAreaSize() , SeqEditor::GetFrameHeight() );
	}
}

void WidTitleNameGroup::mousePressEvent( QMouseEvent *ev)
{
  //クリック時、アクティブにする
  if( ev->button() == Qt::LeftButton )
  {
    MouseClickCore(ev->x(),ev->y());
	}
}

void WidTitleNameGroup::contextMenuEvent(QContextMenuEvent *ev)
{
	QMenu *menu = new QMenu(this);
  
  //メニューをカレントにする処理
  MouseClickCore(ev->x(),ev->y());

  //右クリックメニュー

  //グループ開閉
	menu->addAction( mDataEditor->GetActOpenGroup() );
	menu->addAction( mDataEditor->GetActCloseGroup() );
  menu->addSeparator();
  menu->addAction( mDataEditor->GetActEditGroup() );
  menu->addAction( mDataEditor->GetActAddGroup() );
	menu->addAction( mDataEditor->GetActDeleteGroup() );
  menu->addSeparator();
  menu->addAction( mDataEditor->GetActEditCommand() );
	menu->addAction( mDataEditor->GetActDeleteCommand() );

	menu->move(ev->globalPos());
  
  mDataEditor->SetLockTempDelete(true);
	menu->exec();
  mDataEditor->SetLockTempDelete(false);

	delete menu;
  
  mDataEditor->CheckDeleteReserve();
}

void WidTitleNameGroup::mouseDoubleClickEvent ( QMouseEvent *ev )
{
  int comIdx = ev->y()/SeqEditor::GetFrameHeight();
  
  if( mDataGroup->IsLock() )
  {
    mDataEditor->GetMainWindow()->PopLockWarnning();
    return;
  }

  if( comIdx == 0 )
  {
    mDataEditor->ActEditGroup();
  }
  else
  {
    comIdx--;
    if( comIdx >= 0 && comIdx < mDataGroup->GetCommandNum() )
    {
      mDataEditor->ActEditCommand();
    }
  }
}

void WidTitleNameGroup::MouseClickCore( const int posX , const int posY )
{
	if( mDataEditor->GetCurrentGroup() != mDataGroup )
	{
		mDataEditor->SetCurrentGroup( mDataGroup );
		mDataEditor->SetCurrentCommand( NULL );
	}
  
  const int comIdx = posY/SeqEditor::GetFrameHeight()-1;
  if( comIdx >= 0 && comIdx < mDataGroup->GetCommandNum() )
  {
		mDataEditor->SetCurrentCommand( mDataGroup->GetCommandByIndex( comIdx ) );
  }
  else
  {
		mDataEditor->SetCurrentCommand( NULL );
  }

}


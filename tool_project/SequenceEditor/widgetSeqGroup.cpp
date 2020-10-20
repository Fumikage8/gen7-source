//==============================================================================
/**
 * @file	widgetSeqGroup.cpp
 * @brief	
 * @author	ariizumi_nobuhiko
 * @data	2011/03/01, 15:19
 */
// =============================================================================

#include "widgetSeqGroup.h"
#include "dataEditor.h"
#include "dataGroup.h"

WidSequenceGroup::WidSequenceGroup(DataEditor *dataEditor , DataGroup *dataGroupe , QWidget* parent)
	:QWidget(parent)
	,mDataGroup(dataGroupe)
	,mDataEditor(dataEditor)
{
	mMainLyt = new QVBoxLayout();
	mMainLyt->setMargin(0);
	mMainLyt->setSpacing(0);
	mMainLyt->setSizeConstraint(QLayout::SetFixedSize);

	mMainBar = new WidSequenceBarList(mDataEditor,mDataGroup->GetCommandList() );

	setLayout( mMainLyt );
	mMainLyt->addWidget(mMainBar);

  SetIsOpen(dataGroupe->IsOpen());

	connect( mDataEditor , SIGNAL( UpdateNowFrame(int ) ) ,
					 mMainBar , SLOT( update() ) );

  gfl::core::Debug::PrintConsole("GrpOpen[%d]\n",dataGroupe->IsOpen());
}

WidSequenceGroup::~WidSequenceGroup()
{
  delete mMainBar;
  delete mMainLyt;
	/*
  SeqBarListIt it = mBarList.begin();
  while( it != mBarList.end() )
  {
    WidSequenceBar *seqBar = &(*it);
    it++;
    delete seqBar;
  }
	*/
}

void WidSequenceGroup::AddCommand( DataCommand *com , DataCommand *nextCom )
{
	WidSequenceBarCommand *comBar;
  if( com->GetBarType() == BAR_TYPE_ONE_FRAME )
  {
    comBar = new WidSequenceBarCommand_OneFrame(mDataEditor,com);
  }
  else
  {
    comBar = new WidSequenceBarCommand(mDataEditor,com);
  }
  comBar->setParent( this );
	connect( comBar , SIGNAL(ChangeValue()) , 
					 this , SLOT(update()) );
	connect( mDataEditor , SIGNAL( UpdateNowFrame(int) ) ,
					 this , SLOT(update()) );
	

	connect( this , SIGNAL( SetVisibleSeqBar(bool) ) ,
					 comBar , SLOT(setVisible(bool)) );

  if( nextCom )
  {
    mMainLyt->insertWidget( mMainLyt->indexOf(nextCom->GetCommandBar()), comBar);
  }
  else
  {
  	mMainLyt->addWidget(comBar);
  }

	com->SetCommandBar(comBar);
  comBar->setVisible(mDataGroup->IsOpen());
}

void WidSequenceGroup::RemoveCommand( DataCommand *com )
{
  WidSequenceBarCommand *comBar = com->GetCommandBar();
  delete comBar;
}
	

void WidSequenceGroup::SetIsOpen( bool isOpen )
{
	SetVisibleSeqBar(isOpen);
	mMainBar->SetOpenGroup( isOpen );
}


void WidSequenceGroup::update()
{
  QWidget::update();
  mMainBar->update();

  DataComListIt it = mDataGroup->GetCommandList()->begin();
  while( it != mDataGroup->GetCommandList()->end() )
  {
    DataCommand *com = (*it);
    com->GetCommandBar()->update();
    it++;
  }
}
void WidSequenceGroup::updateGeometry()
{
  QWidget::updateGeometry();
  mMainBar->updateGeometry();

  DataComListIt it = mDataGroup->GetCommandList()->begin();
  while( it != mDataGroup->GetCommandList()->end() )
  {
    DataCommand *com = (*it);
    com->GetCommandBar()->updateGeometry();
    it++;
  }
}

void WidSequenceGroup::paintEvent(QPaintEvent *ev)
{
  QRect drawArea = ev->rect();

  QPainter painter(this);
  painter.eraseRect( rect() );
  painter.fillRect( drawArea , mDataGroup->GetBgColor());
	painter.setPen( Qt::black );
  painter.drawLine( drawArea.left() , sizeHint().height()-1 , drawArea.right() , sizeHint().height()-1 );
}


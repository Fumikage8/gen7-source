//==============================================================================
/**
 * @file	widgetSeqBar.cpp
 * @brief	�R�}���h�̃o�[
 * @author	ariizumi_nobuhiko
 * @data	2011/02/24, 13:20
 */
// =============================================================================

#include "widgetSeqBar.h"
#include "DataEditor.h"
#include "DataCommand.h"
#include "mainWindow.h"

//Seq�o�[�̊��(���̃v���O�����ł��g���܂킹��悤�ɁE�E�E)
WidSequenceBar::WidSequenceBar(DataEditor *data , QWidget* parent)
{
	mDataEditor = data;
  mChipCol = Qt::red;

  mHoldState = HOLD_NONE;
  mMode = MODE_BAR;

  mIsFocus = false;

  setSizePolicy( QSizePolicy::Expanding,QSizePolicy::Fixed);

  mMinFrame = SEQEDIT_MIN_FRAME_VAL;
  mMaxFrame = SEQEDIT_MAX_FRAME_VAL;
}

void WidSequenceBar::paintEvent(QPaintEvent* ev)
{
  QRect drawArea = ev->rect();
  {
    QPainter painter(this);
    painter.setPen(Qt::lightGray);
    
    if( mIsFocus )
    {
      painter.fillRect( drawArea , Qt::gray);
    }

  }
  DrawGrid(&drawArea);

  DrawBar(&drawArea,mStartFrame,mEndFrame);
}

void WidSequenceBar::mousePressEvent(QMouseEvent *ev)
{
  //�N���b�N���C�x���g
  if( ev->button() == Qt::LeftButton )
  {
    if( IsMoveLock() )return;

    const int frame = CalcPosxToFrame(ev->pos().x());

    //�ǂ��炩�̃`�b�v���N���b�N������
    if( frame == mStartFrame )
    {
      mHoldState = HOLD_START;
    }
    else
    if( frame == mEndFrame )
    {
      mHoldState = HOLD_END;
    }
    else
    {
      mHoldState = HOLD_NONE;
    }
  }
}

void WidSequenceBar::mouseMoveEvent(QMouseEvent *ev)
{
  //�h���b�O�C�x���g
  if( mHoldState != HOLD_NONE )
  {
    int *checkFrame;
    if( mHoldState == HOLD_START )
    {
      checkFrame = &mStartFrame;
    }
    else
    {
      checkFrame = &mEndFrame;
    }
    const int frame = CalcPosxToFrame(ev->pos().x());

    if( *checkFrame != frame )
    {
      int diffFrame = 0;
      if( mDataEditor->GetKeyHoldState() & Qt::SHIFT )
      {
        diffFrame = frame - *checkFrame;
      }
      //���ʂ̃N���X���t���[���ύX��virtual�Ŏ��Ȃ��I
      //*checkFrame = frame;
      if( mHoldState == HOLD_START )
      {
        SetFrame( frame , mEndFrame+diffFrame , false );
      }
      else
      {
        SetFrame( mStartFrame+diffFrame , frame , false );
      }

      if( UpdateFrame() == true )
      {
        if( mHoldState == HOLD_START )
        {
          mHoldState = HOLD_END;
        }
        else
        {
          mHoldState = HOLD_START;
        }
      }
			ChangeValue();
      update();
    }
  }
}

void WidSequenceBar::mouseReleaseEvent(QMouseEvent *ev)
{
  if( ev->button() == Qt::LeftButton )
  {
    mHoldState = HOLD_NONE;
  }
}

QSize WidSequenceBar::sizeHint() const
{
	QSize size( mDataEditor->GetLimitFrame() * SeqEditor::GetFrameWidth() + 2  , SeqEditor::GetFrameHeight() );
  return size;
}

void WidSequenceBar::DrawGrid(QRect *drawArea)
{
  //�O���b�h�`��
  QPainter painter(this);
  painter.setPen(Qt::lightGray);

  const int startFrame = (drawArea->x() - CalcFrameToPosX(0))/SeqEditor::GetFrameWidth() - 1;
  const int endFrame = (drawArea->x()+drawArea->width() - CalcFrameToPosX(0))/SeqEditor::GetFrameWidth() + 1;
  
  for( int i = startFrame ; i<=endFrame;i++ )
  {
    const int posX = CalcFrameToPosX(i);

		if( mDataEditor->GetNowFrame() == i )
		{
			painter.setPen( QPen(Qt::red,3) );
	    painter.drawLine( posX , 0 , posX , this->height() );
			painter.setPen( QPen(Qt::lightGray,1) );

		}
    painter.drawLine( posX , 0 , posX , this->height() );
  }
}

void WidSequenceBar::DrawBar(const QRect *rect, int start , int end )
{
  //�o�[�`��(���E�̃`�b�v�͕ʊ֐�)
  
  if( mMode == MODE_BAR )
  {
    if( start != end )
    {
      //�t���[��������
      QPainter painter(this);
  	  painter.setPen(QPen(Qt::black));

      painter.fillRect( start*SeqEditor::GetFrameWidth()+SeqEditor::GetChipWidth() ,
											  SeqEditor::GetChipHeight()/2+1 - SeqEditor::GetLineSize()/2 ,
                        ((end-start)+1) * SeqEditor::GetFrameWidth() - SeqEditor::GetChipWidth()*2 ,
                        SeqEditor::GetLineSize() ,
                        Qt::black);
      DrawChip(	SeqEditor::GetFrameWidth() * start,
							  1,
							  SeqEditor::GetChipWidth(),
							  SeqEditor::GetChipHeight());
      DrawChip(	SeqEditor::GetFrameWidth() * (end+1) - SeqEditor::GetChipWidth(),
							  1,
							  SeqEditor::GetChipWidth(),
							  SeqEditor::GetChipHeight());
    }
    else
    {
      //�t���[��������
      DrawChip(	SeqEditor::GetFrameWidth() * start +(SeqEditor::GetFrameWidth()-SeqEditor::GetChipWidth())/2 ,
							  1,
							  SeqEditor::GetChipWidth(),
							  SeqEditor::GetChipHeight());
    }  
  }
  else
  if( mMode == MODE_COMMENT )
  {
    int len = (SeqEditor::GetFrameWidth() * (end+1-start));

    DrawChip(	SeqEditor::GetFrameWidth() * start,
							1,
							len,
							SeqEditor::GetChipHeight());

    QPainter painter(this);
  	painter.setPen(QPen(Qt::black));
  	painter.setFont( font() );
    
    if( GetDrawStr() )
    {
      int posx = SeqEditor::GetFrameWidth() * start+2;
      if( rect->x()+2 > posx && rect->x() < len-4 )
      {
        posx = rect->x()+2;
      }
       
      painter.drawText( posx, 
                        2 , 
                        len-4,
                        SeqEditor::GetChipHeight()-4 , 
                        (Qt::AlignLeft|Qt::AlignVCenter) , 
                        *GetDrawStr() );
    }
  }
}

void WidSequenceBar::DrawChip( int x , int y , int w , int h )
{
  QPainter painter(this);

  painter.fillRect(x+1,y+1,w-1,h-1,GetColor());

  painter.setPen(Qt::black);
  painter.drawRect(x,y,w,h);
}

void WidSequenceBar::SetFrame( const int start , const int end , bool isUpdate )
{
	if( mStartFrame != start ||
			mEndFrame != end )
	{
		mStartFrame = start;
		mEndFrame = end;

    if( mStartFrame < mMinFrame )
    {
      mStartFrame = mMinFrame;
    }
    if( mEndFrame > mMaxFrame )
    {
      mEndFrame = mMaxFrame;
    }

    if( isUpdate )
    {
		  UpdateFrame();
		  ChangeValue();
    }
	}
}

bool WidSequenceBar::UpdateFrame()
{
  //StartFrame��EndFrame������ւ��������
  if( mStartFrame > mEndFrame )
  {
    const int tmp = mStartFrame;
    mStartFrame = mEndFrame;
    mEndFrame = tmp;

		OutputDebugString( "Flip\n" );

		return true;
  }
  return false;
}

int WidSequenceBar::CalcFrameToPosX(const int frame)
{
  return frame * SeqEditor::GetFrameWidth();
}

int WidSequenceBar::CalcPosxToFrame(const int posX)
{
  return posX / SeqEditor::GetFrameWidth();
}

void WidSequenceBar::SetColor( QColor &col )
{
  mChipCol = col;
}
QColor& WidSequenceBar::GetColor()
{
  return mChipCol;
}

WidSequenceBarCommand_OneFrame::WidSequenceBarCommand_OneFrame(DataEditor *editorData , DataCommand *comData ,QWidget *parent )
:WidSequenceBarCommand(editorData,comData,parent)
{
}

void WidSequenceBarCommand_OneFrame::SetFrame( const int start , const int end , bool isUpdate )
{
  if( GetStartFrame() != start )
  {
    WidSequenceBar::SetFrame( start , start , isUpdate);
  }
  else
  if( GetEndFrame() != end )
  {
    WidSequenceBar::SetFrame( end , end , isUpdate);
  }
}


//-----------------------------------------------------------------------------
//Seq�o�[��SeqEditor�p
WidSequenceBarCommand::WidSequenceBarCommand(DataEditor *editorData , DataCommand *comData , QWidget *parent)
:WidSequenceBar(editorData,parent)
,mComData(comData)
{
	comData->SetCommandBar(this);
  SetFrame( comData->GetStartFrame() , comData->GetEndFrame() );
}

const QString* WidSequenceBarCommand::GetDrawStr(void)
{
  return &mComData->GetCommentStr();
}

void WidSequenceBarCommand::mousePressEvent(QMouseEvent *ev)
{
  if( ev->button() == Qt::LeftButton )
  {
		mDataEditor->SetCurrentCommand( mComData );
		mDataEditor->SetCurrentGroup( mComData->GetParent() );
	}

	WidSequenceBar::mousePressEvent(ev);

  if( mHoldState != HOLD_NONE )
  {
    mBefStartFrame = mStartFrame;
    mBefEndFrame = mEndFrame;
  }
}

void WidSequenceBarCommand::mouseReleaseEvent(QMouseEvent *ev)
{
  if( ev->button() == Qt::LeftButton )
  {
    if( mHoldState != HOLD_NONE )
    {
      if( mBefStartFrame != mStartFrame ||  
          mBefEndFrame != mEndFrame )
      {
        mDataEditor->GetActionManager()->StartSetAction();
        mDataEditor->GetActionManager()->ActionChangeCommandFrame( mComData , mBefStartFrame , mBefEndFrame );
        mDataEditor->GetActionManager()->EndSetAction();
      }
    }
  }
	WidSequenceBar::mouseReleaseEvent(ev);
}

void WidSequenceBarCommand::mouseDoubleClickEvent ( QMouseEvent *ev )
{
  if( mComData->IsLock() )
  {
    mDataEditor->GetMainWindow()->PopLockWarnning();
    return;
  }

	mDataEditor->SetCurrentGroup( mComData->GetParent() );
	mDataEditor->SetCurrentCommand( mComData );
  mDataEditor->ActEditCommand();
}

bool WidSequenceBarCommand::UpdateFrame()
{
  if( mDataEditor->GetCurrentCommand() == mComData )
  {
    SetIsFocus( true );
  }
  else
  {
    SetIsFocus( false );
  }

	const bool ret = WidSequenceBar::UpdateFrame();
	mComData->SetFrame( GetStartFrame() , GetEndFrame() );
	return ret;
}

QColor& WidSequenceBarCommand::GetColor()
{
  return mComData->GetColor();
}

bool WidSequenceBarCommand::IsMoveLock()
{
  return mComData->IsLock();
}

//-----------------------------------------------------------------------------
//Seq�o�[��SeqEditor�p �����̃o�[�̏W��(�O���[�v�`��p)
WidSequenceBarList::WidSequenceBarList(DataEditor *editorData ,  DataComList* comList , QWidget *parent)
	:WidSequenceBar(editorData,parent)
	,mComList(comList)
	,mIsOpenGroup(true)
{

}

void WidSequenceBarList::paintEvent(QPaintEvent *ev)
{
  QRect drawArea = ev->rect();
  DrawGrid(&drawArea);

	if( !mIsOpenGroup )
	{
		DataComListIt it = mComList->begin();
		while( it != mComList->end() )
		{
			DataCommand *com = (*it);
      SetColor( com->GetColor() );
			DrawBar( &drawArea,com->GetStartFrame() , com->GetEndFrame() );
			it++;
		}
	}
}

void WidSequenceBarList::SetOpenGroup(bool flg)
{
	mIsOpenGroup = flg;
	update();
  updateGeometry();
}

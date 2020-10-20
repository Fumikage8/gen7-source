/*
 * File:   widgetSeqLabel.cpp
 * Author: ariizumi_nobuhiko
 *
 * Created on 2011/03/04
 */

//ˆê”Ôã’i‚Ì–Ú·‚è

#include "widgetSeqLabel.h"

WidSequenceLabel::WidSequenceLabel(QWidget *parent)
:QWidget(parent)
,mScrollX(0)
{
  setSizePolicy( QSizePolicy::Expanding,QSizePolicy::Fixed);
}

void WidSequenceLabel::UpdateScroll(int val)
{
	mScrollX = val;
	update();
}

void WidSequenceLabel::paintEvent(QPaintEvent *ev)
{
  QRect drawArea = ev->rect();
  QPainter painter(this);
	painter.setPen(QPen(Qt::lightGray));

	const int startFrame = (mScrollX+drawArea.x() - util::CalcFrameToPosX(0,SeqEditor::GetFrameWidth()))/SeqEditor::GetFrameWidth() - 1;
	const int endFrame = (mScrollX+drawArea.x()+drawArea.width() - util::CalcFrameToPosX(0,SeqEditor::GetFrameWidth()))/SeqEditor::GetFrameWidth() + 1;
  
  for( int i = startFrame ; i<=endFrame;i++ )
  {
		const int posX = util::CalcFrameToPosX(i,SeqEditor::GetFrameWidth())-mScrollX;
		if( i%5 == 0 )
		{
			QString num;
			num.setNum(i);
	    painter.drawLine( posX , this->height()/3 , posX , this->height() );
			painter.setPen( Qt::black );
			painter.drawText( posX-16 , 0 , 32,12 , Qt::AlignCenter , num );
			painter.setPen( Qt::lightGray );
		}
		else
		{
	    painter.drawLine( posX , this->height()/3*2 , posX , this->height() );
		}
  }

	painter.setPen( Qt::black );

	painter.drawLine( drawArea.left() , sizeHint().height()-1 , drawArea.right() , sizeHint().height()-1 );
}

QSize WidSequenceLabel::sizeHint() const
{
  QSize size( SEQEDIT_TEMP_WIDTH , 32 );
  return size;
}
/*
 * File:   widgetSeqLabel.cpp
 * Author: ariizumi_nobuhiko
 *
 * Created on 2011/03/04
 */

#include "widgetFrameScale.h"
#include "dataEditor.h"

WidFrameScale::WidFrameScale(DataEditor *data , QWidget *parent)
:QWidget(parent)
,mMinScale(0)
,mMaxScale(256)
,mNowScale(0)
,mDataEditor(data)
{
  setSizePolicy( QSizePolicy::Expanding,QSizePolicy::Fixed);
	setMinimumHeight( 32 );
	setMinimumWidth( 32 );
}

void WidFrameScale::paintEvent(QPaintEvent *ev)
{
	const int LenScare = mMaxScale - mMinScale;

  QRect drawArea = ev->rect();
	
	int ScaleWidth = drawArea.width() - (OFS_X*2);
	int memoriNum = ScaleWidth/MEMORI_SIZE;
	if( memoriNum == 0 )
	{
		memoriNum = 1;
	}
	int memoriHaba = ((int)(LenScare/memoriNum/MEMORI_MIN))*MEMORI_MIN;
	if( memoriHaba < MEMORI_MIN )
	{
		memoriHaba = MEMORI_MIN;
	}
	const int tmpWidth = LenScare/memoriHaba;
	const int memorWidth = ScaleWidth/(tmpWidth==0?1:tmpWidth);
  QPainter painter(this);

	//現在値
	{
		const int posX = (mNowScale-mMinScale)*ScaleWidth/LenScare + OFS_X;
		painter.setPen( QPen(Qt::red,3) );
    painter.drawLine( posX , this->height()/5 , posX , this->height() );

		painter.setPen( QPen(Qt::lightGray,1) );
	}
	//目盛り
	for( int i=0;i<=LenScare;i++ )
	{
		const int val = i + mMinScale;
		const int posX = i*ScaleWidth/LenScare + OFS_X;
		if( val % memoriHaba == 0 )
		{
			QString num;
			num.setNum(val);
			painter.setPen( Qt::lightGray );
	    painter.drawLine( posX , this->height()/2 , posX , this->height() );

			painter.setPen( Qt::black );
			painter.drawText( posX-16 , 0 , 32,12 , Qt::AlignCenter , num );
		}
		else if( memorWidth > 40 && memoriHaba == MEMORI_MIN )	//適当サブ目盛り
		{
			painter.setPen( Qt::lightGray );
	    painter.drawLine( posX , this->height()/4*3 , posX , this->height() );
		}
	}

	//両端	
	{
		//QString num;
		int posX = OFS_X;
		//num.setNum(mMinScale);
		painter.setPen( Qt::black );
	  painter.drawLine( posX , this->height()/3 , posX , this->height() );
		//painter.drawText( posX-16 , 0 , 32,12 , Qt::AlignCenter , num );

		posX = drawArea.width()-OFS_X;
		//num.setNum(mMaxScale);
		painter.setPen( Qt::black );
    painter.drawLine( posX , this->height()/3 , posX , this->height() );
		//painter.drawText( posX-16 , 0 , 32,12 , Qt::AlignCenter , num );
	}
/*
	painter.setPen(QPen(Qt::lightGray));

	{
		QRect rectArea = ev->rect();
		rectArea.setWidth( rectArea.width()-1);
		rectArea.setHeight( rectArea.height()-1);
		painter.drawRect( rectArea );
	}
*/
}

void WidFrameScale::mousePressEvent(QMouseEvent *ev)
{
	if( ev->button() & Qt::LeftButton )
  {
		MoveEventFunc( ev->pos().x() );
	}
}

void WidFrameScale::mouseMoveEvent(QMouseEvent *ev)
{
	if( ev->buttons() & Qt::LeftButton )
  {
		MoveEventFunc( ev->pos().x() );
	}
}

void WidFrameScale::MoveEventFunc( int mousePosX )
{
		const int width = this->size().width() - (OFS_X*2);
		const int Len = mMaxScale - mMinScale;
		
		//そのままだと、フレーム位置から次のフレーム位置までなので半分ずらす
		const int ofs = width/Len/2;
		const int posX = mousePosX - OFS_X + ofs;

		int frame = posX*Len/width + mMinScale;
		SetNowScale( frame );

}


void WidFrameScale::SetMaxScale(int val)
{
	if( mMaxScale != val )
	{
		mMaxScale = val;
		if( mMaxScale <= mMinScale )
		{
			mMaxScale = mMinScale+1;
		}
		if( mMaxScale > mDataEditor->GetLimitFrame() )
		{
			mMaxScale = mDataEditor->GetLimitFrame();
		}
		ChangeMaxScale(mMaxScale);

		if( mNowScale > mMaxScale )
		{
			SetNowScale( mMaxScale );
		}
		update();
	}
}

void WidFrameScale::SetMinScale(int val)
{
	if( mMinScale != val )
	{
		mMinScale = val;
		if( mMinScale >= mMaxScale )
		{
			mMinScale = mMaxScale-1;
		}
		ChangeMinScale(mMinScale);
		if( mNowScale < mMinScale )
		{
			SetNowScale( mMinScale );
		}
		update();
	}
}

void WidFrameScale::SetNowScale(int val)
{
	if( mNowScale != val )
	{
		mNowScale = val;
		if( mNowScale > mMaxScale )
		{
			mNowScale = mMaxScale;
		}
		if( mNowScale < mMinScale )
		{
			mNowScale = mMinScale;
		}
		ChangeNowScale(mNowScale);
		update();
	}
}

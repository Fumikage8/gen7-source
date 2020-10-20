/*
 * File:   widgetSeqLabel.h
 * Author: ariizumi_nobuhiko
 *
 * Created on 2011/03/04
 */

#ifndef __WIDGET_FRAME_SCALE_H__
#define __WIDGET_FRAME_SCALE_H__

#include "localDef.h"

class WidFrameScale : public QWidget
{
	Q_OBJECT
public:
	static const int OFS_X = 8;
	static const int MEMORI_SIZE = 32;
	static const int MEMORI_MIN = 5;
	
	WidFrameScale(DataEditor *data , QWidget *parent = NULL);

	int GetMinScale(void){return mMinScale;}
	int GetMaxScale(void){return mMaxScale;}

public Q_SLOTS:
	void SetMaxScale(int val);
	void SetMinScale(int val);
	void SetNowScale(int val);

Q_SIGNALS:
	void ChangeMaxScale(int val);	
	void ChangeMinScale(int val);	
	void ChangeNowScale(int val);	

protected:
  //åpè≥
  virtual void paintEvent(QPaintEvent *ev);
  virtual void mousePressEvent(QMouseEvent *ev);
  virtual void mouseMoveEvent(QMouseEvent *ev);
private:

	void MoveEventFunc( int mousePosX );
		
	int mMinScale;
	int mMaxScale;
	int mNowScale;

	DataEditor *mDataEditor;
};




#endif  __WIDGET_FRAME_SCALE_H__

/*
 * File:   widgetSeqLabel.h
 * Author: ariizumi_nobuhiko
 *
 * Created on 2011/03/04
 */

//àÍî‘è„íiÇÃñ⁄ê∑ÇË

#ifndef __WIDGET_SEQ_LABEL_H__
#define __WIDGET_SEQ_LABEL_H__

#include "localDef.h"

class WidSequenceLabel : public QWidget
{
	Q_OBJECT
public:
  WidSequenceLabel(QWidget *parent = NULL);

public Q_SLOTS:
	void UpdateScroll(int val);

protected:
  //åpè≥
  virtual void paintEvent(QPaintEvent *ev);
  virtual QSize sizeHint() const;


private:
	int mScrollX;
};




#endif  __WIDGET_SEQ_LABEL_H__

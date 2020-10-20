/*
 * File:   widgetSeqLabel.h
 * Author: ariizumi_nobuhiko
 *
 * Created on 2011/03/04
 */

#ifndef __WIDGET_NUM_BOX_H__
#define __WIDGET_NUM_BOX_H__

#include "localDef.h"

//ƒtƒŒ[ƒ€”—p”šbox
class WidNumBox : public QSpinBox
{
	Q_OBJECT
public:
  WidNumBox(QWidget *parent = NULL);

public Q_SLOTS:
	void SendValueSlot();

Q_SIGNALS:
	void FinishInput(int);

protected:
  //Œp³

private:
};




#endif  __WIDGET_NUM_BOX_H__

/*
 * File:   widgetSeqLabel.cpp
 * Author: ariizumi_nobuhiko
 *
 * Created on 2011/03/04
 */

#include "widgetNumBox.h"

//ƒtƒŒ[ƒ€”—p”šbox
WidNumBox::WidNumBox(QWidget *parent)
:QSpinBox(parent)
{
	setButtonSymbols(QAbstractSpinBox::NoButtons);
	setMinimumWidth(64);
	setMinimum(0);
	setMaximum(99999);
	connect( this , SIGNAL(editingFinished()) , 
					 this , SLOT(SendValueSlot()));
}

void WidNumBox::SendValueSlot()
{
	FinishInput(value());
}

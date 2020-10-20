/*
 * File:   dialogMoveFrame.h
 * Author: ariizumi_nobuhiko
 *
 * Created on 2011/03/04
 */

#ifndef __DIALOG_MOVE_FRAME_H__
#define __DIALOG_MOVE_FRAME_H__

#include <rapidxml/gfl_Xml.h>
#include "localDef.h"

class DialogMoveFrame : public QDialog
{
	Q_OBJECT
public:
  DialogMoveFrame(QWidget *parent = NULL);
  void setupUi(QDialog *Dialog);
  void retranslateUi(QDialog *Dialog);

  //QDialogButtonBox *buttonBox;

  QPushButton *okButton;
  QPushButton *cancelButton;
  QVBoxLayout *buttonLyt;

  //ÉOÉãÅ[Évñº
  QHBoxLayout *horizontalLayout;
  QLabel *label;
  QSpinBox *valueBox;

  QVBoxLayout *valLyt;
  QHBoxLayout *mainLyt;

protected:
  //åpè≥

private:


};




#endif  __DIALOG_MOVE_FRAME_H__

/*
 * File:   widgetSeqLabel.h
 * Author: ariizumi_nobuhiko
 *
 * Created on 2011/03/04
 */

#ifndef __DIALOG_ADD_GROUP_H__
#define __DIALOG_ADD_GROUP_H__

#include <rapidxml/gfl_Xml.h>
#include "localDef.h"
#include "dataGroup.h"

class DialogAddGroup : public QDialog
{
	Q_OBJECT
public Q_SLOTS:
  void changeColorAll();
  void changeBgColor();

public:
  DialogAddGroup(QWidget *parent = NULL,DataGroup *grp=NULL);
  void setupUi(QDialog *Dialog);
  void retranslateUi(QDialog *Dialog);

  void SetGroupOption( gfl::xml::Node *optNode , QString *defStr = NULL );


  //QDialogButtonBox *buttonBox;

  QPushButton *okButton;
  QPushButton *cancelButton;
  QPushButton *colButton;
  QPushButton *colButton2;
  QVBoxLayout *buttonLyt;



  //グループ名
  QHBoxLayout *horizontalLayout;
  QLabel *label;
  QLineEdit *lineEdit;

  //デバッグ設定
  QHBoxLayout *debugHLyt;
  QLabel *debugLabel;
	QCheckBox *debugCheck;

  //グループ番号指定
  QHBoxLayout *grpNoLyt;
  QLabel *grpNoLabel;
  QSpinBox *grpNoVal;
  QHBoxLayout *grpOptLyt;
  QLabel *grpOptLabel;
  QComboBox *grpOption;

  QVBoxLayout *valLyt;
  QHBoxLayout *mainLyt;

  QColor bgColor;

protected:
  //継承

private:
  
  DataGroup *mGrp;

};




#endif  __DIALOG_ADD_GROUP_H__

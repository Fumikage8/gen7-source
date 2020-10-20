/*
 * File:   dialogMoveFrame.cpp
 * Author: ariizumi_nobuhiko
 *
 * Created on 2011/03/04
 */

//フレーム操作ダイアログ
#include "dialogMoveFrame.h"

DialogMoveFrame::DialogMoveFrame(QWidget *parent)
:QDialog(parent)
{
	setupUi( this );
}

void DialogMoveFrame::setupUi(QDialog *Dialog)
{
  Dialog->setObjectName(QString::fromUtf8("Dialog"));
  Dialog->resize(293, 72);

  okButton = new QPushButton;
  cancelButton = new QPushButton;

  buttonLyt = new QVBoxLayout;
  buttonLyt->addWidget( okButton );
  buttonLyt->addWidget( cancelButton );
  buttonLyt->setSpacing(0);

/*
  buttonBox = new QDialogButtonBox(Dialog);
  buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
  //buttonBox->setGeometry(QRect(210, 10, 75, 52));
  buttonBox->setOrientation(Qt::Vertical);
  buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
*/

  label = new QLabel;
  valueBox = new QSpinBox;

  horizontalLayout = new QHBoxLayout;
  horizontalLayout->addWidget(label);
  horizontalLayout->addWidget(valueBox);

  valLyt = new QVBoxLayout;
  valLyt->addLayout( horizontalLayout );
  valLyt->setSpacing(0);
  //valLyt->addWidget

  mainLyt = new QHBoxLayout;
  mainLyt->addLayout( valLyt );
  mainLyt->addLayout( buttonLyt );
  mainLyt->setSpacing(0);

	setLayout( mainLyt );


  retranslateUi(Dialog);
  QObject::connect(okButton, SIGNAL(pressed()), Dialog, SLOT(accept()));
  QObject::connect(cancelButton, SIGNAL(pressed()), Dialog, SLOT(reject()));

  QMetaObject::connectSlotsByName(Dialog);

	valueBox->setFocus();
  valueBox->selectAll();
  valueBox->setMaximum( SEQEDIT_MAX_FRAME_VAL );


}

void DialogMoveFrame::retranslateUi(QDialog *Dialog)
{
  okButton->setText("ok");
  cancelButton->setText("cancel");

  Dialog->setWindowTitle(QString::fromLocal8Bit("フレーム操作"));
  label->setText(QString::fromLocal8Bit("フレーム数"));

}



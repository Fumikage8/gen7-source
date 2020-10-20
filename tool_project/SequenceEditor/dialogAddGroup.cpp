/*
 * File:   widgetSeqLabel.cpp
 * Author: ariizumi_nobuhiko
 *
 * Created on 2011/03/04
 */

//グループ追加ダイアログ(仮？)
#include "dialogAddGroup.h"
#include "dataGroup.h"

DialogAddGroup::DialogAddGroup(QWidget *parent,DataGroup *grp)
:QDialog(parent)
,mGrp(grp)
{
  bgColor.setRgb(240,240,240);
	setupUi( this );
}

void DialogAddGroup::setupUi(QDialog *Dialog)
{
  Dialog->setObjectName(QString::fromUtf8("Dialog"));
  Dialog->resize(293, 72);

  okButton = new QPushButton;
  cancelButton = new QPushButton;
  colButton = new QPushButton;
  colButton2 = new QPushButton;

  buttonLyt = new QVBoxLayout;
  buttonLyt->addWidget( okButton );
  buttonLyt->addWidget( cancelButton );
  buttonLyt->addWidget( colButton );
  if( mGrp == NULL ||
      mGrp->GetCommandNum() <= 0 )
  {
    colButton->setEnabled(false);
  }
  buttonLyt->addWidget( colButton2 );
  buttonLyt->setSpacing(0);

/*
  buttonBox = new QDialogButtonBox(Dialog);
  buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
  //buttonBox->setGeometry(QRect(210, 10, 75, 52));
  buttonBox->setOrientation(Qt::Vertical);
  buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
*/

  label = new QLabel;
  lineEdit = new QLineEdit;

  horizontalLayout = new QHBoxLayout;
  horizontalLayout->addWidget(label);
  horizontalLayout->addWidget(lineEdit);

  grpNoLabel = new QLabel;
  grpNoVal = new QSpinBox;
  grpNoVal->setMaximum(255);

  grpNoLyt = new QHBoxLayout;
  grpNoLyt->addWidget(grpNoLabel);
  grpNoLyt->addWidget(grpNoVal);

  debugHLyt = new QHBoxLayout;
  debugLabel = new QLabel;
	debugCheck = new QCheckBox ;
  debugHLyt->addWidget(debugLabel);
  debugHLyt->addWidget(debugCheck);

  grpOptLabel= new QLabel;
  grpOption = new QComboBox;

  grpOptLyt = new QHBoxLayout;
  grpOptLyt->addWidget(grpOptLabel);
  grpOptLyt->addWidget(grpOption);

  valLyt = new QVBoxLayout;
  valLyt->addLayout( horizontalLayout );
  valLyt->addLayout( grpNoLyt );
  valLyt->addLayout( debugHLyt );
  valLyt->addLayout( grpOptLyt );
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
  QObject::connect(colButton, SIGNAL(pressed()), Dialog, SLOT(changeColorAll()));
  QObject::connect(colButton2, SIGNAL(pressed()), Dialog, SLOT(changeBgColor()));

  QMetaObject::connectSlotsByName(Dialog);

	lineEdit->setFocus();

}

void DialogAddGroup::retranslateUi(QDialog *Dialog)
{
  okButton->setText("ok");
  cancelButton->setText("cancel");
  colButton->setText("一括色変更");
  colButton2->setText("背景色変更");

  Dialog->setWindowTitle(QString::fromLocal8Bit("グループ追加"));
  label->setText(QString::fromLocal8Bit("グループ名"));

  grpNoLabel->setText("グループ番号");

  debugLabel->setText("デバッグ用");
}

void DialogAddGroup::SetGroupOption( gfl::xml::Node *optNode , QString *defStr )
{
  grpOptLabel->setText("オプション");
  
  if( optNode )
  {
    int idx = 0;  
  	gfl::xml::Node* node = optNode->FirstNode( );
	  for(; node; node = node->NextSibling( ) )
    {
      QVariant val( node->name() );
      //QListWidgetItem *item = new QListWidgetItem( node->AttributeString( "brief" ) );
      //item->setData(Qt::UserRole,idx);
		  grpOption->addItem( node->AttributeString( "label" ),val );
      
      QString grpStr( node->name() );
      if( defStr && defStr == grpStr )
      {
        grpOption->setCurrentIndex(idx);
      }
      //mMacroNodeList.append( node );
      idx++;
	  }
  }
}

void DialogAddGroup::changeColorAll()
{
  if( mGrp == NULL )
  {
    return;
  }

  DataComListIt it = mGrp->GetComList()->begin();
  DataCommand *com = (*it);

  QColorDialog colDiag(com->GetColor(),this);

  int ret = colDiag.exec();
	if( ret == QDialog::Accepted )
	{
    QColor col = colDiag.selectedColor();

    while( it != mGrp->GetComList()->end() )
    {
      DataCommand *com = (*it);
      it++;
      com->SetColor(col);
    }
  }

}

void DialogAddGroup::changeBgColor()
{
  QColorDialog colDiag(bgColor,this);

  int ret = colDiag.exec();
	if( ret == QDialog::Accepted )
	{
    bgColor = colDiag.selectedColor();
  }
}

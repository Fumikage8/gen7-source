/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "window.h"
#include "resource.h"


#ifndef QT_NO_SYSTEMTRAYICON

//#include <QtGui/QtGui>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QApplication>

#include <QtWidgets/QAction>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMenu>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QMessageBox>
#include <QtGui/QPixmap>
#include <QtCore/QTextCodec>
#include <QtCore/QDateTime>

//! [0]
MyWindow::MyWindow(ipcnet::StartListener* pSL)
{
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("Shift-JIS"));

	mbWindowEnd = false;
	
	mpQFile=NULL;

	mpStartListener = pSL;

//	QMainWindow::setVisible(true);


  createOptionGroupBox();
	commandGroupBoxCreate();
  createMessageGroupBox();

  createActions();



  //connect(logSaveCheckBox, SIGNAL(toggled(bool)), trayIcon, SLOT(setVisible(bool)));

		//connect(softAInitButton, SIGNAL(clicked()), this, SLOT(quit()));


	connect(softAInitButton, SIGNAL(clicked()), this, SLOT(abuttonClicked()));
	connect(softBInitButton, SIGNAL(clicked()), this, SLOT(bbuttonClicked()));
	connect(command1Button, SIGNAL(clicked()), this, SLOT(button1Clicked()));
	connect(command2Button, SIGNAL(clicked()), this, SLOT(button2Clicked()));
//  connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
 //         this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(iconGroupBox);
  mainLayout->addWidget(commandGroupBox);
  mainLayout->addWidget(messageGroupBox);
  setLayout(mainLayout);

	QWidget::setVisible(true);

  setWindowTitle(tr("GF IPC CLIENT TEST"));

	

  resize(400, 300);
}

MyWindow::~MyWindow()
{

	CloseFile();
}


void MyWindow::abuttonClicked(void)
{
	QString stg = ipTextEdit->text();

	mpStartListener->StartCallback(0, stg.utf16());
}

void MyWindow::bbuttonClicked(void)
{
	QString stg = ipTextEdit->text();

	mpStartListener->StartCallback(1, stg.utf16());
}


void MyWindow::button2Clicked(void)
{
	//テストコマンド送信
	mpStartListener->TestCommand(1);
}


void MyWindow::button1Clicked(void)
{
	//テストコマンド送信
	mpStartListener->TestCommand(0);
}


bool MyWindow::OutputCallback( char* pOut )
{
	bodyEdit->insertPlainText(QString::fromLocal8Bit(pOut));

	return true;
}


/*
void MyWindow::closeEvent(QCloseEvent *event)
{
	event->ignore();

}
*/

//! [4]
void MyWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        break;
    case QSystemTrayIcon::MiddleClick:
        showMessage();
        break;
    default:
        ;
    }
}
//! [4]

//! [5]//   タスクトレイにメッセージ表示
void MyWindow::showMessage()
{
}
//! [5]

//! [6]
void MyWindow::messageClicked()
{
    QMessageBox::information(0, ("GF IPC_SERVER"),
                             ("Sorry, I already gave what help I could.\n"
                                "Maybe you should try asking a human?"));
}
//! [6]

///< オプションのレイアウト作成
void MyWindow::createOptionGroupBox()
{
	iconGroupBox = new QGroupBox(QString::fromLocal8Bit("起動方法"));

	softAInitButton = new QPushButton(QString::fromLocal8Bit("ソフトＡで起動"));

  softBInitButton = new QPushButton(QString::fromLocal8Bit("ソフトＢで起動"));


	ipTextEdit = new QLineEdit(QString::fromLocal8Bit("127.0.0.1"));


  QVBoxLayout *iconLayout = new QVBoxLayout;
  iconLayout->addWidget(softAInitButton);
  iconLayout->addWidget(softBInitButton);
	iconLayout->addWidget(ipTextEdit);
  iconGroupBox->setLayout(iconLayout);
}



///< 
void MyWindow::commandGroupBoxCreate()
{
	commandGroupBox = new QGroupBox(QString::fromLocal8Bit("通信"));

	command1Button = new QPushButton(QString::fromLocal8Bit("通信１"));

  command2Button = new QPushButton(QString::fromLocal8Bit("通信２"));



  QVBoxLayout *iconLayout = new QVBoxLayout;
  iconLayout->addWidget(command1Button);
  iconLayout->addWidget(command2Button);
  commandGroupBox->setLayout(iconLayout);



}







void MyWindow::createMessageGroupBox()
{
    messageGroupBox = new QGroupBox(QString::fromLocal8Bit("ログ"));


    bodyEdit = new QTextEdit;


    QGridLayout *messageLayout = new QGridLayout;
    messageLayout->addWidget(bodyEdit, 3, 1, 2, 4);
    messageLayout->setColumnStretch(3, 1);
    messageLayout->setRowStretch(4, 1);
    messageGroupBox->setLayout(messageLayout);
}

//タスクトレイのメニュー一覧 
void MyWindow::createActions()
{
}


void MyWindow::endWindow()
{
	mbWindowEnd=true;
}



void MyWindow::OpenFile(void)
{
}

void MyWindow::WriteFile(char* pOut)
{
}


void MyWindow::CloseFile(void)
{
}




#endif

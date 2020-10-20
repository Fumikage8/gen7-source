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
MyWindow::MyWindow(HINSTANCE hInst)
{
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("Shift-JIS"));

	mbWindowEnd = false;
	
	mhInst = hInst;
	mpQFile=NULL;


  createOptionGroupBox();
  createMessageGroupBox();

  createActions();
  createTrayIcon();

  connect(logSaveCheckBox, SIGNAL(toggled(bool)), trayIcon, SLOT(setVisible(bool)));
  connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
  connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
          this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(iconGroupBox);
  mainLayout->addWidget(messageGroupBox);
  setLayout(mainLayout);

	setIcon();
  trayIcon->show();

  setWindowTitle(tr("GF IPC SERVER"));
  resize(400, 300);
}

MyWindow::~MyWindow()
{

	if(trayIcon){
		delete trayIcon;
		trayIcon=NULL;
	}
	CloseFile();
}



void MyTray::closeEvent(QCloseEvent *event)
{

	mpMyWindow->CloseFile();
	event->accept();


}


MyTray::MyTray( MyWindow* pwin) : QSystemTrayIcon(pwin)
{
	mpMyWindow = pwin;

}


MyTray::~MyTray()
{
}



void MyWindow::setVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    QDialog::setVisible(visible);
}
//! [1]


bool MyWindow::OutputCallback( char* pOut )
{
	bodyEdit->insertPlainText(QString::fromLocal8Bit(pOut));
	OpenFile();
	WriteFile(pOut);
	return true;

}



//! [2]
void MyWindow::closeEvent(QCloseEvent *event)
{
	if (trayIcon->isVisible()) {
		hide();
		event->ignore();
  }

}
//! [2]

//! [3]
void MyWindow::setIcon(void)
{
    QIcon icon =  QIcon(QPixmap( "ipc2.ico" ));    ////iconComboBox->itemIcon(index);
    trayIcon->setIcon(icon);
    setWindowIcon(icon);

//    trayIcon->setToolTip(iconComboBox->itemText(index));
    trayIcon->setToolTip("GF IPC_SERVER");
}
//! [3]

//! [4]
void MyWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
//        iconComboBox->setCurrentIndex((iconComboBox->currentIndex() + 1) % iconComboBox->count());
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
    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(
            typeComboBox->itemData(typeComboBox->currentIndex()).toInt());
    trayIcon->showMessage(titleEdit->text(), bodyEdit->toPlainText(), icon,
                          10 * 1000);
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
	iconGroupBox = new QGroupBox(QString::fromLocal8Bit("オプション"));

	logSaveCheckBox = new QCheckBox(QString::fromLocal8Bit("ログの保存"));
	logSaveCheckBox->setChecked(true);

    logRotateCheckBox = new QCheckBox(QString::fromLocal8Bit("ログの定期消去"));
    logRotateCheckBox->setChecked(true);



    QVBoxLayout *iconLayout = new QVBoxLayout;
    iconLayout->addWidget(logSaveCheckBox);
    iconLayout->addWidget(logRotateCheckBox);
    iconGroupBox->setLayout(iconLayout);
}

void MyWindow::createMessageGroupBox()
{
    messageGroupBox = new QGroupBox(QString::fromLocal8Bit("ログ"));

   // typeLabel = new QLabel(QString::fromLocal8Bit("ログ"));

    typeComboBox = new QComboBox;
    typeComboBox->addItem(("None"), QSystemTrayIcon::NoIcon);
    typeComboBox->addItem(style()->standardIcon(
            QStyle::SP_MessageBoxInformation), ("Information"),
            QSystemTrayIcon::Information);
    typeComboBox->addItem(style()->standardIcon(
            QStyle::SP_MessageBoxWarning), ("Warning"),
            QSystemTrayIcon::Warning);
    typeComboBox->addItem(style()->standardIcon(
            QStyle::SP_MessageBoxCritical),("Critical"),
            QSystemTrayIcon::Critical);
    typeComboBox->setCurrentIndex(1);


    bodyEdit = new QTextEdit;
//    bodyEdit->setPlainText(tr("Don't believe me. Honestly, I don't have a "
//                              "clue.\nClick this balloon for details."));


    QGridLayout *messageLayout = new QGridLayout;
    messageLayout->addWidget(bodyEdit, 3, 1, 2, 4);
    messageLayout->setColumnStretch(3, 1);
    messageLayout->setRowStretch(4, 1);
    messageGroupBox->setLayout(messageLayout);
}

//タスクトレイのメニュー一覧 
void MyWindow::createActions()
{
    minimizeAction = new QAction(QString::fromLocal8Bit("最小化"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(QString::fromLocal8Bit("最大化"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(QString::fromLocal8Bit("戻す"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(QString::fromLocal8Bit("終了"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(endWindow()));
}

//タスクトレイのアイコン
void MyWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

//  trayIcon = new QSystemTrayIcon(this);
		
    trayIcon = new MyTray(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void MyWindow::endWindow()
{
	mbWindowEnd=true;
}



void MyWindow::OpenFile(void)
{
	if(!logSaveCheckBox->isChecked()){
		return;
	}
	if(mpQFile==NULL){
		QDateTime qti = QDateTime::currentDateTime();
    QString name = qti.toString("yyMMdd_hhmm");
    QString head("log");
    QString foot(".txt");
    head += name;
    head += foot;
		mpQFile = new QFile( head );
		mpQFile->open(QIODevice::WriteOnly);
	}
}

void MyWindow::WriteFile(char* pOut,bool bFlash)
{
	if(mpQFile){
		mpQFile->write(pOut);
		if(bFlash){
			mpQFile->flush();
		}
	}
}


void MyWindow::CloseFile(void)
{
	if(mpQFile){
		mpQFile->close();
		mpQFile = NULL;
	}
}




#endif

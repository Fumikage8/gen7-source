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

#ifndef WINDOW_H
#define WINDOW_H

#include <QtWidgets/QSystemTrayIcon>
#include "../include/output_listener.h"
#include "../include/start_listener.h"


#ifndef QT_NO_SYSTEMTRAYICON

#include <QtWidgets/QDialog>
#include <QtWidgets/QMainWindow>


QT_BEGIN_NAMESPACE
class QAction;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QPushButton;
class QSpinBox;
class QTextEdit;
class QFile;
QT_END_NAMESPACE


class MyWindow;




//! [0]
class MyWindow : public QWidget , public ipcnet::OutputListener
{
    Q_OBJECT

public:
    MyWindow(ipcnet::StartListener* mpSL);
		~MyWindow();

  //  void setVisible(bool visible);

		bool isWindowEnd(void){ return mbWindowEnd; };

public:
	virtual bool OutputCallback( char* pOut );

//    void closeEvent(QCloseEvent *event);

private slots:
   // void setIcon(void);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showMessage();
    void messageClicked();
		void endWindow();
	void abuttonClicked(void);
	void bbuttonClicked(void);

	void button1Clicked(void);
	void button2Clicked(void);



public:
		void OpenFile(void);
		void CloseFile(void);
		void WriteFile(char* pOut);

  
private:
    void createOptionGroupBox();
    void createMessageGroupBox();
    void commandGroupBoxCreate(void);
    void createActions();

    QGroupBox *iconGroupBox;
    QPushButton *softAInitButton;        //‹N“®ƒ{ƒ^ƒ“
    QPushButton *softBInitButton;

    QGroupBox *messageGroupBox;
   QComboBox *typeComboBox;
    QLineEdit *titleEdit;
    QTextEdit *bodyEdit;

		QLineEdit* ipTextEdit;

    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

		ipcnet::StartListener* mpStartListener;

		QFile* mpQFile;

	QLabel* myLabel;
	QLineEdit* myNameTextEdit;


	QGroupBox* commandGroupBox;

	QPushButton* command1Button;

  QPushButton* command2Button;

	bool mbWindowEnd;

};
//! [0]

#endif // QT_NO_SYSTEMTRAYICON

#endif

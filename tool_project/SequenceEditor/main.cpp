//==============================================================================
/**
 * @file	main.cpp
 * @brief	
 * @author	ariizumi_nobuhiko
 * @data	2011/02/24, 12:34
 */
// =============================================================================

#include "mainWindow.h"
#include "localDef.h"

int main(int argc , char *argv[])
{
	SeqEditor::Init();


  QApplication app(argc,argv);
	//•¶Žš‰»‚¯–hŽ~
	QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
  QTextCodec::setCodecForTr(QTextCodec::codecForLocale());

  MainWindow mainWin(argc , argv);
  mainWin.show();
	
	

	const int ret = app.exec();


	
	return ret;

}
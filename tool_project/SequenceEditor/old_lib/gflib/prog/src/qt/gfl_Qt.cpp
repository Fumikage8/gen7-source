//==============================================================================
/**
 * @file	gfl_Graphic.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2011/04/05, 15:02
 */
// =============================================================================

// ファイルをまとめてコンパイル時間を大幅短縮

#ifdef GFL_PLATFORM_PC // ======================================================

#include "gfl_Qt.h"

#ifdef GFL_COMPILER_VC
#pragma message ("gfl_Render.cpp")
#endif
#include "gfl_Render.cpp"

#ifdef GFL_COMPILER_VC
#pragma message ("gfl_Model.cpp")
#endif
#include "gfl_QtModel.cpp"

#ifdef GFL_COMPILER_VC
#pragma message ("gfl_CmdlModel.cpp")
#endif
#include "gfl_CmdlModel.cpp"

#ifdef GFL_COMPILER_VC
#pragma message ("gfl_Application.cpp")
#endif
#include "gfl_Application.cpp"

#ifdef GFL_COMPILER_VC
#pragma message ("gfl_GLWidget.cpp")
#endif
#include "gfl_GLWidget.cpp"

#ifdef GFL_COMPILER_VC
#pragma message ("gfl_GLWindow.cpp")
#endif
#include "gfl_GLWindow.cpp"

#ifdef GFL_COMPILER_VC
#pragma message ("gfl_MainWindow.cpp")
#endif
#include "gfl_MainWindow.cpp"

#ifdef GFL_COMPILER_VC
#pragma message ("gfl_File.cpp")
#endif
#include "gfl_QtFile.cpp"

#ifdef GFL_COMPILER_VC
#pragma message ("gfl_Zip.cpp")
#endif
#include "gfl_Zip.cpp"

#ifndef NOT_USE_HIO_DEV
#ifdef GFL_COMPILER_VC
#pragma message ("gfl_ClientBase.cpp")
#endif
#include "gfl_ClientBase.cpp"

#ifdef GFL_COMPILER_VC
#pragma message ("gfl_ClientRead.cpp")
#endif
#include "gfl_ClientRead.cpp"

#ifdef GFL_COMPILER_VC
#pragma message ("gfl_ClientWrite.cpp")
#endif
#include "gfl_ClientWrite.cpp"

#ifdef GFL_COMPILER_VC
#pragma message ("gfl_Client.cpp")
#endif
#        include "gfl_Client.cpp"
#    endif

#ifdef USE_PROPERTY_GUI
#    ifdef GFL_COMPILER_VC
#        pragma message( "gfl_QtProperty.cpp" )
#    endif
#    include "gfl_QtProperty.cpp"
//#    ifdef GFL_COMPILER_VC
//#        pragma message ("moc_gfl_QtProperty.cpp")
//#    endif
//#    include "../../include/qt/moc_gfl_QtProperty.cpp"
#endif

#endif

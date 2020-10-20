#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc.exe
CCC=g++.exe
CXX=g++.exe
FC=g77.exe
AS=as.exe

# Macros
CND_PLATFORM=Cygwin-Windows
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Qt4/qscilexercsharp.o \
	${OBJECTDIR}/lexers/LexTAL.o \
	${OBJECTDIR}/lexers/LexAU3.o \
	${OBJECTDIR}/lexers/LexEScript.o \
	${OBJECTDIR}/Qt4/qscicommandset.o \
	${OBJECTDIR}/lexers/LexErlang.o \
	${OBJECTDIR}/Qt4/qscilexerspice.o \
	${OBJECTDIR}/src/Editor.o \
	${OBJECTDIR}/Qt4/qscilexerproperties.o \
	${OBJECTDIR}/Qt4/qscilexerpostscript.o \
	${OBJECTDIR}/lexers/LexEiffel.o \
	${OBJECTDIR}/lexers/LexBaan.o \
	${OBJECTDIR}/lexers/LexMPT.o \
	${OBJECTDIR}/lexers/LexModula.o \
	${OBJECTDIR}/src/ExternalLexer.o \
	${OBJECTDIR}/lexers/LexD.o \
	${OBJECTDIR}/Qt4/qscilexerpov.o \
	${OBJECTDIR}/lexers/LexSQL.o \
	${OBJECTDIR}/lexers/LexAPDL.o \
	${OBJECTDIR}/lexers/LexSmalltalk.o \
	${OBJECTDIR}/Qt4/qscilexerhtml.o \
	${OBJECTDIR}/lexers/LexMatlab.o \
	${OBJECTDIR}/lexers/LexTACL.o \
	${OBJECTDIR}/lexers/LexSorcus.o \
	${OBJECTDIR}/lexers/LexCrontab.o \
	${OBJECTDIR}/Qt4/qscidocument.o \
	${OBJECTDIR}/lexlib/WordList.o \
	${OBJECTDIR}/lexlib/LexerNoExceptions.o \
	${OBJECTDIR}/example-Qt4/main.o \
	${OBJECTDIR}/lexers/LexVerilog.o \
	${OBJECTDIR}/Qt4/qscilexerpascal.o \
	${OBJECTDIR}/lexers/LexVB.o \
	${OBJECTDIR}/Qt4/qsciapis.o \
	${OBJECTDIR}/Qt4/qscilexeroctave.o \
	${OBJECTDIR}/lexers/LexKix.o \
	${OBJECTDIR}/lexers/LexTxt2tags.o \
	${OBJECTDIR}/lexers/LexPS.o \
	${OBJECTDIR}/Qt4/qscilexercpp.o \
	${OBJECTDIR}/src/PositionCache.o \
	${OBJECTDIR}/lexers/LexMMIXAL.o \
	${OBJECTDIR}/Qt4/qscilexerjavascript.o \
	${OBJECTDIR}/lexers/LexPython.o \
	${OBJECTDIR}/src/CallTip.o \
	${OBJECTDIR}/Qt4/qscimacro.o \
	${OBJECTDIR}/lexers/LexASY.o \
	${OBJECTDIR}/lexers/LexSpice.o \
	${OBJECTDIR}/lexers/LexA68k.o \
	${OBJECTDIR}/Qt4/qsciscintillabase.o \
	${OBJECTDIR}/lexers/LexPascal.o \
	${OBJECTDIR}/Qt4/qscicommand.o \
	${OBJECTDIR}/lexlib/LexerSimple.o \
	${OBJECTDIR}/lexers/LexPB.o \
	${OBJECTDIR}/Qt4/qscilexerbatch.o \
	${OBJECTDIR}/lexers/LexCOBOL.o \
	${OBJECTDIR}/lexers/LexAVE.o \
	${OBJECTDIR}/lexers/LexTADS3.o \
	${OBJECTDIR}/Qt4/ScintillaQt.o \
	${OBJECTDIR}/lexers/LexAsn1.o \
	${OBJECTDIR}/lexers/LexForth.o \
	${OBJECTDIR}/Qt4/qscilexerxml.o \
	${OBJECTDIR}/src/Catalogue.o \
	${OBJECTDIR}/lexers/LexCmake.o \
	${OBJECTDIR}/lexers/LexCSS.o \
	${OBJECTDIR}/Qt4/qscilexersql.o \
	${OBJECTDIR}/lexers/LexHTML.o \
	${OBJECTDIR}/src/Style.o \
	${OBJECTDIR}/lexers/LexPowerPro.o \
	${OBJECTDIR}/Qt4/qscilexercss.o \
	${OBJECTDIR}/lexers/LexCsound.o \
	${OBJECTDIR}/lexers/LexSpecman.o \
	${OBJECTDIR}/src/CharClassify.o \
	${OBJECTDIR}/lexers/LexAbaqus.o \
	${OBJECTDIR}/Qt4/qscilexerbash.o \
	${OBJECTDIR}/lexlib/Accessor.o \
	${OBJECTDIR}/Qt4/qscilexerfortran77.o \
	${OBJECTDIR}/lexers/LexScriptol.o \
	${OBJECTDIR}/lexers/LexMSSQL.o \
	${OBJECTDIR}/lexers/LexLisp.o \
	${OBJECTDIR}/lexers/LexFlagship.o \
	${OBJECTDIR}/Qt4/qscistyle.o \
	${OBJECTDIR}/src/LineMarker.o \
	${OBJECTDIR}/lexers/LexNsis.o \
	${OBJECTDIR}/src/PerLine.o \
	${OBJECTDIR}/src/UniConversion.o \
	${OBJECTDIR}/lexlib/CharacterSet.o \
	${OBJECTDIR}/lexers/LexCLW.o \
	${OBJECTDIR}/Qt4/qscilexermatlab.o \
	${OBJECTDIR}/lexers/LexYAML.o \
	${OBJECTDIR}/Qt4/qscilexerjava.o \
	${OBJECTDIR}/Qt4/qsciscintilla.o \
	${OBJECTDIR}/lexers/LexNimrod.o \
	${OBJECTDIR}/src/Selection.o \
	${OBJECTDIR}/Qt4/qscilexercmake.o \
	${OBJECTDIR}/src/XPM.o \
	${OBJECTDIR}/lexers/LexFortran.o \
	${OBJECTDIR}/Qt4/qscilexerverilog.o \
	${OBJECTDIR}/Qt4/qscilexeryaml.o \
	${OBJECTDIR}/Qt4/qscilexervhdl.o \
	${OBJECTDIR}/Qt4/qscilexertex.o \
	${OBJECTDIR}/Qt4/qscilexeridl.o \
	${OBJECTDIR}/lexers/LexConf.o \
	${OBJECTDIR}/src/Indicator.o \
	${OBJECTDIR}/Qt4/PlatQt.o \
	${OBJECTDIR}/Qt4/qscilexermakefile.o \
	${OBJECTDIR}/src/KeyMap.o \
	${OBJECTDIR}/lexers/LexRuby.o \
	${OBJECTDIR}/src/RunStyles.o \
	${OBJECTDIR}/lexers/LexPerl.o \
	${OBJECTDIR}/src/CellBuffer.o \
	${OBJECTDIR}/lexers/LexVHDL.o \
	${OBJECTDIR}/lexers/LexR.o \
	${OBJECTDIR}/lexers/LexBash.o \
	${OBJECTDIR}/lexers/LexLout.o \
	${OBJECTDIR}/lexers/LexOthers.o \
	${OBJECTDIR}/lexers/LexMarkdown.o \
	${OBJECTDIR}/src/Document.o \
	${OBJECTDIR}/lexers/LexOpal.o \
	${OBJECTDIR}/lexers/LexLua.o \
	${OBJECTDIR}/Qt4/qscilexerpython.o \
	${OBJECTDIR}/src/ViewStyle.o \
	${OBJECTDIR}/lexers/LexRebol.o \
	${OBJECTDIR}/src/Decoration.o \
	${OBJECTDIR}/Qt4/qscilexerperl.o \
	${OBJECTDIR}/Qt4/qscilexerpawn.o \
	${OBJECTDIR}/Qt4/qscilexerlua.o \
	${OBJECTDIR}/lexlib/LexerModule.o \
	${OBJECTDIR}/src/ScintillaBase.o \
	${OBJECTDIR}/src/AutoComplete.o \
	${OBJECTDIR}/lexlib/StyleContext.o \
	${OBJECTDIR}/lexers/LexInno.o \
	${OBJECTDIR}/lexers/LexBullant.o \
	${OBJECTDIR}/lexers/LexHaskell.o \
	${OBJECTDIR}/lexlib/LexerBase.o \
	${OBJECTDIR}/lexers/LexCPP.o \
	${OBJECTDIR}/lexlib/PropSetSimple.o \
	${OBJECTDIR}/lexers/LexAsm.o \
	${OBJECTDIR}/Qt4/ListBoxQt.o \
	${OBJECTDIR}/lexers/LexGui4Cli.o \
	${OBJECTDIR}/lexers/LexPawn.o \
	${OBJECTDIR}/Qt4/qscilexerfortran.o \
	${OBJECTDIR}/Qt4/qscistyledtext.o \
	${OBJECTDIR}/src/ContractionState.o \
	${OBJECTDIR}/lexers/LexTeX.o \
	${OBJECTDIR}/example-Qt4/mainwindow.o \
	${OBJECTDIR}/lexers/LexPLM.o \
	${OBJECTDIR}/lexers/LexPOV.o \
	${OBJECTDIR}/Qt4/SciClasses.o \
	${OBJECTDIR}/lexers/LexCaml.o \
	${OBJECTDIR}/lexers/LexGAP.o \
	${OBJECTDIR}/Qt4/qscilexercustom.o \
	${OBJECTDIR}/Qt4/qscilexer.o \
	${OBJECTDIR}/lexers/LexMySQL.o \
	${OBJECTDIR}/lexers/LexSML.o \
	${OBJECTDIR}/lexers/LexAda.o \
	${OBJECTDIR}/lexers/LexBasic.o \
	${OBJECTDIR}/lexers/LexMagik.o \
	${OBJECTDIR}/Qt4/qsciabstractapis.o \
	${OBJECTDIR}/lexers/LexPowerShell.o \
	${OBJECTDIR}/lexers/LexMetapost.o \
	${OBJECTDIR}/Qt4/qscilexerd.o \
	${OBJECTDIR}/Qt4/qsciprinter.o \
	${OBJECTDIR}/lexers/LexTCL.o \
	${OBJECTDIR}/Qt4/qscilexertcl.o \
	${OBJECTDIR}/Qt4/qscilexerruby.o \
	${OBJECTDIR}/lexers/LexProgress.o \
	${OBJECTDIR}/src/RESearch.o \
	${OBJECTDIR}/Qt4/qscilexerdiff.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/qscintilla.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/qscintilla.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/qscintilla ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/Qt4/qscilexercsharp.o: Qt4/qscilexercsharp.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexercsharp.o Qt4/qscilexercsharp.cpp

${OBJECTDIR}/lexers/LexTAL.o: lexers/LexTAL.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexTAL.o lexers/LexTAL.cpp

${OBJECTDIR}/lexers/LexAU3.o: lexers/LexAU3.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexAU3.o lexers/LexAU3.cpp

${OBJECTDIR}/lexers/LexEScript.o: lexers/LexEScript.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexEScript.o lexers/LexEScript.cpp

${OBJECTDIR}/Qt4/qscicommandset.o: Qt4/qscicommandset.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscicommandset.o Qt4/qscicommandset.cpp

${OBJECTDIR}/lexers/LexErlang.o: lexers/LexErlang.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexErlang.o lexers/LexErlang.cpp

${OBJECTDIR}/Qt4/qscilexerspice.o: Qt4/qscilexerspice.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexerspice.o Qt4/qscilexerspice.cpp

${OBJECTDIR}/src/Editor.o: src/Editor.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Editor.o src/Editor.cpp

${OBJECTDIR}/Qt4/qscilexerproperties.o: Qt4/qscilexerproperties.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexerproperties.o Qt4/qscilexerproperties.cpp

${OBJECTDIR}/Qt4/qscilexerpostscript.o: Qt4/qscilexerpostscript.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexerpostscript.o Qt4/qscilexerpostscript.cpp

${OBJECTDIR}/lexers/LexEiffel.o: lexers/LexEiffel.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexEiffel.o lexers/LexEiffel.cpp

${OBJECTDIR}/lexers/LexBaan.o: lexers/LexBaan.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexBaan.o lexers/LexBaan.cpp

${OBJECTDIR}/lexers/LexMPT.o: lexers/LexMPT.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexMPT.o lexers/LexMPT.cpp

${OBJECTDIR}/lexers/LexModula.o: lexers/LexModula.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexModula.o lexers/LexModula.cpp

${OBJECTDIR}/src/ExternalLexer.o: src/ExternalLexer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/ExternalLexer.o src/ExternalLexer.cpp

${OBJECTDIR}/lexers/LexD.o: lexers/LexD.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexD.o lexers/LexD.cpp

${OBJECTDIR}/Qt4/qscilexerpov.o: Qt4/qscilexerpov.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexerpov.o Qt4/qscilexerpov.cpp

${OBJECTDIR}/lexers/LexSQL.o: lexers/LexSQL.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexSQL.o lexers/LexSQL.cpp

${OBJECTDIR}/lexers/LexAPDL.o: lexers/LexAPDL.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexAPDL.o lexers/LexAPDL.cpp

${OBJECTDIR}/lexers/LexSmalltalk.o: lexers/LexSmalltalk.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexSmalltalk.o lexers/LexSmalltalk.cpp

${OBJECTDIR}/Qt4/qscilexerhtml.o: Qt4/qscilexerhtml.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexerhtml.o Qt4/qscilexerhtml.cpp

${OBJECTDIR}/lexers/LexMatlab.o: lexers/LexMatlab.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexMatlab.o lexers/LexMatlab.cpp

${OBJECTDIR}/lexers/LexTACL.o: lexers/LexTACL.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexTACL.o lexers/LexTACL.cpp

${OBJECTDIR}/lexers/LexSorcus.o: lexers/LexSorcus.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexSorcus.o lexers/LexSorcus.cpp

${OBJECTDIR}/lexers/LexCrontab.o: lexers/LexCrontab.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexCrontab.o lexers/LexCrontab.cpp

${OBJECTDIR}/Qt4/qscidocument.o: Qt4/qscidocument.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscidocument.o Qt4/qscidocument.cpp

${OBJECTDIR}/lexlib/WordList.o: lexlib/WordList.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexlib
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexlib/WordList.o lexlib/WordList.cpp

${OBJECTDIR}/lexlib/LexerNoExceptions.o: lexlib/LexerNoExceptions.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexlib
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexlib/LexerNoExceptions.o lexlib/LexerNoExceptions.cpp

${OBJECTDIR}/example-Qt4/main.o: example-Qt4/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/example-Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/example-Qt4/main.o example-Qt4/main.cpp

${OBJECTDIR}/lexers/LexVerilog.o: lexers/LexVerilog.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexVerilog.o lexers/LexVerilog.cpp

${OBJECTDIR}/Qt4/qscilexerpascal.o: Qt4/qscilexerpascal.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexerpascal.o Qt4/qscilexerpascal.cpp

${OBJECTDIR}/lexers/LexVB.o: lexers/LexVB.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexVB.o lexers/LexVB.cpp

${OBJECTDIR}/Qt4/qsciapis.o: Qt4/qsciapis.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qsciapis.o Qt4/qsciapis.cpp

${OBJECTDIR}/Qt4/qscilexeroctave.o: Qt4/qscilexeroctave.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexeroctave.o Qt4/qscilexeroctave.cpp

${OBJECTDIR}/lexers/LexKix.o: lexers/LexKix.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexKix.o lexers/LexKix.cpp

${OBJECTDIR}/lexers/LexTxt2tags.o: lexers/LexTxt2tags.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexTxt2tags.o lexers/LexTxt2tags.cpp

${OBJECTDIR}/lexers/LexPS.o: lexers/LexPS.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexPS.o lexers/LexPS.cpp

${OBJECTDIR}/Qt4/qscilexercpp.o: Qt4/qscilexercpp.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexercpp.o Qt4/qscilexercpp.cpp

${OBJECTDIR}/src/PositionCache.o: src/PositionCache.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/PositionCache.o src/PositionCache.cpp

${OBJECTDIR}/lexers/LexMMIXAL.o: lexers/LexMMIXAL.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexMMIXAL.o lexers/LexMMIXAL.cpp

${OBJECTDIR}/Qt4/qscilexerjavascript.o: Qt4/qscilexerjavascript.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexerjavascript.o Qt4/qscilexerjavascript.cpp

${OBJECTDIR}/lexers/LexPython.o: lexers/LexPython.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexPython.o lexers/LexPython.cpp

${OBJECTDIR}/src/CallTip.o: src/CallTip.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/CallTip.o src/CallTip.cpp

${OBJECTDIR}/Qt4/qscimacro.o: Qt4/qscimacro.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscimacro.o Qt4/qscimacro.cpp

${OBJECTDIR}/lexers/LexASY.o: lexers/LexASY.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexASY.o lexers/LexASY.cpp

${OBJECTDIR}/lexers/LexSpice.o: lexers/LexSpice.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexSpice.o lexers/LexSpice.cpp

${OBJECTDIR}/lexers/LexA68k.o: lexers/LexA68k.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexA68k.o lexers/LexA68k.cpp

${OBJECTDIR}/Qt4/qsciscintillabase.o: Qt4/qsciscintillabase.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qsciscintillabase.o Qt4/qsciscintillabase.cpp

${OBJECTDIR}/lexers/LexPascal.o: lexers/LexPascal.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexPascal.o lexers/LexPascal.cpp

${OBJECTDIR}/Qt4/qscicommand.o: Qt4/qscicommand.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscicommand.o Qt4/qscicommand.cpp

${OBJECTDIR}/lexlib/LexerSimple.o: lexlib/LexerSimple.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexlib
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexlib/LexerSimple.o lexlib/LexerSimple.cpp

${OBJECTDIR}/lexers/LexPB.o: lexers/LexPB.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexPB.o lexers/LexPB.cpp

${OBJECTDIR}/Qt4/qscilexerbatch.o: Qt4/qscilexerbatch.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexerbatch.o Qt4/qscilexerbatch.cpp

${OBJECTDIR}/lexers/LexCOBOL.o: lexers/LexCOBOL.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexCOBOL.o lexers/LexCOBOL.cpp

${OBJECTDIR}/lexers/LexAVE.o: lexers/LexAVE.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexAVE.o lexers/LexAVE.cpp

${OBJECTDIR}/lexers/LexTADS3.o: lexers/LexTADS3.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexTADS3.o lexers/LexTADS3.cpp

${OBJECTDIR}/Qt4/ScintillaQt.o: Qt4/ScintillaQt.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/ScintillaQt.o Qt4/ScintillaQt.cpp

${OBJECTDIR}/lexers/LexAsn1.o: lexers/LexAsn1.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexAsn1.o lexers/LexAsn1.cpp

${OBJECTDIR}/lexers/LexForth.o: lexers/LexForth.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexForth.o lexers/LexForth.cpp

${OBJECTDIR}/Qt4/qscilexerxml.o: Qt4/qscilexerxml.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexerxml.o Qt4/qscilexerxml.cpp

${OBJECTDIR}/src/Catalogue.o: src/Catalogue.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Catalogue.o src/Catalogue.cpp

${OBJECTDIR}/lexers/LexCmake.o: lexers/LexCmake.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexCmake.o lexers/LexCmake.cpp

${OBJECTDIR}/lexers/LexCSS.o: lexers/LexCSS.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexCSS.o lexers/LexCSS.cpp

${OBJECTDIR}/Qt4/qscilexersql.o: Qt4/qscilexersql.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexersql.o Qt4/qscilexersql.cpp

${OBJECTDIR}/lexers/LexHTML.o: lexers/LexHTML.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexHTML.o lexers/LexHTML.cpp

${OBJECTDIR}/src/Style.o: src/Style.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Style.o src/Style.cpp

${OBJECTDIR}/lexers/LexPowerPro.o: lexers/LexPowerPro.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexPowerPro.o lexers/LexPowerPro.cpp

${OBJECTDIR}/Qt4/qscilexercss.o: Qt4/qscilexercss.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexercss.o Qt4/qscilexercss.cpp

${OBJECTDIR}/lexers/LexCsound.o: lexers/LexCsound.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexCsound.o lexers/LexCsound.cpp

${OBJECTDIR}/lexers/LexSpecman.o: lexers/LexSpecman.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexSpecman.o lexers/LexSpecman.cpp

${OBJECTDIR}/src/CharClassify.o: src/CharClassify.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/CharClassify.o src/CharClassify.cpp

${OBJECTDIR}/lexers/LexAbaqus.o: lexers/LexAbaqus.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexAbaqus.o lexers/LexAbaqus.cpp

${OBJECTDIR}/Qt4/qscilexerbash.o: Qt4/qscilexerbash.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexerbash.o Qt4/qscilexerbash.cpp

${OBJECTDIR}/lexlib/Accessor.o: lexlib/Accessor.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexlib
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexlib/Accessor.o lexlib/Accessor.cpp

${OBJECTDIR}/Qt4/qscilexerfortran77.o: Qt4/qscilexerfortran77.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexerfortran77.o Qt4/qscilexerfortran77.cpp

${OBJECTDIR}/lexers/LexScriptol.o: lexers/LexScriptol.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexScriptol.o lexers/LexScriptol.cpp

${OBJECTDIR}/lexers/LexMSSQL.o: lexers/LexMSSQL.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexMSSQL.o lexers/LexMSSQL.cpp

${OBJECTDIR}/lexers/LexLisp.o: lexers/LexLisp.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexLisp.o lexers/LexLisp.cpp

${OBJECTDIR}/lexers/LexFlagship.o: lexers/LexFlagship.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexFlagship.o lexers/LexFlagship.cpp

${OBJECTDIR}/Qt4/qscistyle.o: Qt4/qscistyle.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscistyle.o Qt4/qscistyle.cpp

${OBJECTDIR}/src/LineMarker.o: src/LineMarker.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/LineMarker.o src/LineMarker.cpp

${OBJECTDIR}/lexers/LexNsis.o: lexers/LexNsis.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexNsis.o lexers/LexNsis.cpp

${OBJECTDIR}/src/PerLine.o: src/PerLine.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/PerLine.o src/PerLine.cpp

${OBJECTDIR}/src/UniConversion.o: src/UniConversion.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/UniConversion.o src/UniConversion.cpp

${OBJECTDIR}/lexlib/CharacterSet.o: lexlib/CharacterSet.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexlib
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexlib/CharacterSet.o lexlib/CharacterSet.cpp

${OBJECTDIR}/lexers/LexCLW.o: lexers/LexCLW.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexCLW.o lexers/LexCLW.cpp

${OBJECTDIR}/Qt4/qscilexermatlab.o: Qt4/qscilexermatlab.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexermatlab.o Qt4/qscilexermatlab.cpp

${OBJECTDIR}/lexers/LexYAML.o: lexers/LexYAML.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexYAML.o lexers/LexYAML.cpp

${OBJECTDIR}/Qt4/qscilexerjava.o: Qt4/qscilexerjava.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexerjava.o Qt4/qscilexerjava.cpp

${OBJECTDIR}/Qt4/qsciscintilla.o: Qt4/qsciscintilla.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qsciscintilla.o Qt4/qsciscintilla.cpp

${OBJECTDIR}/lexers/LexNimrod.o: lexers/LexNimrod.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexNimrod.o lexers/LexNimrod.cpp

${OBJECTDIR}/src/Selection.o: src/Selection.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Selection.o src/Selection.cpp

${OBJECTDIR}/Qt4/qscilexercmake.o: Qt4/qscilexercmake.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexercmake.o Qt4/qscilexercmake.cpp

${OBJECTDIR}/src/XPM.o: src/XPM.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/XPM.o src/XPM.cpp

${OBJECTDIR}/lexers/LexFortran.o: lexers/LexFortran.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexFortran.o lexers/LexFortran.cpp

${OBJECTDIR}/Qt4/qscilexerverilog.o: Qt4/qscilexerverilog.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexerverilog.o Qt4/qscilexerverilog.cpp

${OBJECTDIR}/Qt4/qscilexeryaml.o: Qt4/qscilexeryaml.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexeryaml.o Qt4/qscilexeryaml.cpp

${OBJECTDIR}/Qt4/qscilexervhdl.o: Qt4/qscilexervhdl.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexervhdl.o Qt4/qscilexervhdl.cpp

${OBJECTDIR}/Qt4/qscilexertex.o: Qt4/qscilexertex.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexertex.o Qt4/qscilexertex.cpp

${OBJECTDIR}/Qt4/qscilexeridl.o: Qt4/qscilexeridl.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexeridl.o Qt4/qscilexeridl.cpp

${OBJECTDIR}/lexers/LexConf.o: lexers/LexConf.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexConf.o lexers/LexConf.cpp

${OBJECTDIR}/src/Indicator.o: src/Indicator.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Indicator.o src/Indicator.cpp

${OBJECTDIR}/Qt4/PlatQt.o: Qt4/PlatQt.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/PlatQt.o Qt4/PlatQt.cpp

${OBJECTDIR}/Qt4/qscilexermakefile.o: Qt4/qscilexermakefile.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexermakefile.o Qt4/qscilexermakefile.cpp

${OBJECTDIR}/src/KeyMap.o: src/KeyMap.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/KeyMap.o src/KeyMap.cpp

${OBJECTDIR}/lexers/LexRuby.o: lexers/LexRuby.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexRuby.o lexers/LexRuby.cpp

${OBJECTDIR}/src/RunStyles.o: src/RunStyles.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/RunStyles.o src/RunStyles.cpp

${OBJECTDIR}/lexers/LexPerl.o: lexers/LexPerl.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexPerl.o lexers/LexPerl.cpp

${OBJECTDIR}/src/CellBuffer.o: src/CellBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/CellBuffer.o src/CellBuffer.cpp

${OBJECTDIR}/lexers/LexVHDL.o: lexers/LexVHDL.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexVHDL.o lexers/LexVHDL.cpp

${OBJECTDIR}/lexers/LexR.o: lexers/LexR.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexR.o lexers/LexR.cpp

${OBJECTDIR}/lexers/LexBash.o: lexers/LexBash.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexBash.o lexers/LexBash.cpp

${OBJECTDIR}/lexers/LexLout.o: lexers/LexLout.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexLout.o lexers/LexLout.cpp

${OBJECTDIR}/lexers/LexOthers.o: lexers/LexOthers.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexOthers.o lexers/LexOthers.cpp

${OBJECTDIR}/lexers/LexMarkdown.o: lexers/LexMarkdown.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexMarkdown.o lexers/LexMarkdown.cpp

${OBJECTDIR}/src/Document.o: src/Document.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Document.o src/Document.cpp

${OBJECTDIR}/lexers/LexOpal.o: lexers/LexOpal.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexOpal.o lexers/LexOpal.cpp

${OBJECTDIR}/lexers/LexLua.o: lexers/LexLua.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexLua.o lexers/LexLua.cpp

${OBJECTDIR}/Qt4/qscilexerpython.o: Qt4/qscilexerpython.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexerpython.o Qt4/qscilexerpython.cpp

${OBJECTDIR}/src/ViewStyle.o: src/ViewStyle.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/ViewStyle.o src/ViewStyle.cpp

${OBJECTDIR}/lexers/LexRebol.o: lexers/LexRebol.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexRebol.o lexers/LexRebol.cpp

${OBJECTDIR}/src/Decoration.o: src/Decoration.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Decoration.o src/Decoration.cpp

${OBJECTDIR}/Qt4/qscilexerperl.o: Qt4/qscilexerperl.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexerperl.o Qt4/qscilexerperl.cpp

${OBJECTDIR}/Qt4/qscilexerpawn.o: Qt4/qscilexerpawn.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexerpawn.o Qt4/qscilexerpawn.cpp

${OBJECTDIR}/Qt4/qscilexerlua.o: Qt4/qscilexerlua.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexerlua.o Qt4/qscilexerlua.cpp

${OBJECTDIR}/lexlib/LexerModule.o: lexlib/LexerModule.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexlib
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexlib/LexerModule.o lexlib/LexerModule.cpp

${OBJECTDIR}/src/ScintillaBase.o: src/ScintillaBase.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/ScintillaBase.o src/ScintillaBase.cpp

${OBJECTDIR}/src/AutoComplete.o: src/AutoComplete.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/AutoComplete.o src/AutoComplete.cpp

${OBJECTDIR}/lexlib/StyleContext.o: lexlib/StyleContext.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexlib
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexlib/StyleContext.o lexlib/StyleContext.cpp

${OBJECTDIR}/lexers/LexInno.o: lexers/LexInno.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexInno.o lexers/LexInno.cpp

${OBJECTDIR}/lexers/LexBullant.o: lexers/LexBullant.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexBullant.o lexers/LexBullant.cpp

${OBJECTDIR}/lexers/LexHaskell.o: lexers/LexHaskell.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexHaskell.o lexers/LexHaskell.cpp

${OBJECTDIR}/lexlib/LexerBase.o: lexlib/LexerBase.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexlib
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexlib/LexerBase.o lexlib/LexerBase.cpp

${OBJECTDIR}/lexers/LexCPP.o: lexers/LexCPP.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexCPP.o lexers/LexCPP.cpp

${OBJECTDIR}/lexlib/PropSetSimple.o: lexlib/PropSetSimple.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexlib
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexlib/PropSetSimple.o lexlib/PropSetSimple.cpp

${OBJECTDIR}/lexers/LexAsm.o: lexers/LexAsm.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexAsm.o lexers/LexAsm.cpp

${OBJECTDIR}/Qt4/ListBoxQt.o: Qt4/ListBoxQt.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/ListBoxQt.o Qt4/ListBoxQt.cpp

${OBJECTDIR}/lexers/LexGui4Cli.o: lexers/LexGui4Cli.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexGui4Cli.o lexers/LexGui4Cli.cpp

${OBJECTDIR}/lexers/LexPawn.o: lexers/LexPawn.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexPawn.o lexers/LexPawn.cpp

${OBJECTDIR}/Qt4/qscilexerfortran.o: Qt4/qscilexerfortran.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexerfortran.o Qt4/qscilexerfortran.cpp

${OBJECTDIR}/Qt4/qscistyledtext.o: Qt4/qscistyledtext.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscistyledtext.o Qt4/qscistyledtext.cpp

${OBJECTDIR}/src/ContractionState.o: src/ContractionState.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/ContractionState.o src/ContractionState.cpp

${OBJECTDIR}/lexers/LexTeX.o: lexers/LexTeX.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexTeX.o lexers/LexTeX.cpp

${OBJECTDIR}/example-Qt4/mainwindow.o: example-Qt4/mainwindow.cpp 
	${MKDIR} -p ${OBJECTDIR}/example-Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/example-Qt4/mainwindow.o example-Qt4/mainwindow.cpp

${OBJECTDIR}/lexers/LexPLM.o: lexers/LexPLM.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexPLM.o lexers/LexPLM.cpp

${OBJECTDIR}/lexers/LexPOV.o: lexers/LexPOV.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexPOV.o lexers/LexPOV.cpp

${OBJECTDIR}/Qt4/SciClasses.o: Qt4/SciClasses.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/SciClasses.o Qt4/SciClasses.cpp

${OBJECTDIR}/lexers/LexCaml.o: lexers/LexCaml.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexCaml.o lexers/LexCaml.cpp

${OBJECTDIR}/lexers/LexGAP.o: lexers/LexGAP.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexGAP.o lexers/LexGAP.cpp

${OBJECTDIR}/Qt4/qscilexercustom.o: Qt4/qscilexercustom.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexercustom.o Qt4/qscilexercustom.cpp

${OBJECTDIR}/Qt4/qscilexer.o: Qt4/qscilexer.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexer.o Qt4/qscilexer.cpp

${OBJECTDIR}/lexers/LexMySQL.o: lexers/LexMySQL.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexMySQL.o lexers/LexMySQL.cpp

${OBJECTDIR}/lexers/LexSML.o: lexers/LexSML.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexSML.o lexers/LexSML.cpp

${OBJECTDIR}/lexers/LexAda.o: lexers/LexAda.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexAda.o lexers/LexAda.cpp

${OBJECTDIR}/lexers/LexBasic.o: lexers/LexBasic.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexBasic.o lexers/LexBasic.cpp

${OBJECTDIR}/lexers/LexMagik.o: lexers/LexMagik.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexMagik.o lexers/LexMagik.cpp

${OBJECTDIR}/Qt4/qsciabstractapis.o: Qt4/qsciabstractapis.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qsciabstractapis.o Qt4/qsciabstractapis.cpp

${OBJECTDIR}/lexers/LexPowerShell.o: lexers/LexPowerShell.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexPowerShell.o lexers/LexPowerShell.cpp

${OBJECTDIR}/lexers/LexMetapost.o: lexers/LexMetapost.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexMetapost.o lexers/LexMetapost.cpp

${OBJECTDIR}/Qt4/qscilexerd.o: Qt4/qscilexerd.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexerd.o Qt4/qscilexerd.cpp

${OBJECTDIR}/Qt4/qsciprinter.o: Qt4/qsciprinter.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qsciprinter.o Qt4/qsciprinter.cpp

${OBJECTDIR}/lexers/LexTCL.o: lexers/LexTCL.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexTCL.o lexers/LexTCL.cpp

${OBJECTDIR}/Qt4/qscilexertcl.o: Qt4/qscilexertcl.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexertcl.o Qt4/qscilexertcl.cpp

${OBJECTDIR}/Qt4/qscilexerruby.o: Qt4/qscilexerruby.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexerruby.o Qt4/qscilexerruby.cpp

${OBJECTDIR}/lexers/LexProgress.o: lexers/LexProgress.cpp 
	${MKDIR} -p ${OBJECTDIR}/lexers
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexers/LexProgress.o lexers/LexProgress.cpp

${OBJECTDIR}/src/RESearch.o: src/RESearch.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/RESearch.o src/RESearch.cpp

${OBJECTDIR}/Qt4/qscilexerdiff.o: Qt4/qscilexerdiff.cpp 
	${MKDIR} -p ${OBJECTDIR}/Qt4
	${RM} $@.d
	$(COMPILE.cc) -DGFL_DEBUG=1 -I../../../../xy_ctrlib/CTR_SDK/include -I../../../../xy_ctrlib/NintendoWare/CTR/include -Iinclude -I../../../../gflib_cpp/gflib/prog/include -I../../../../../Qt/4.5.3/include/QtCore -IQt4 -I../../../../../Qt/4.5.3/include/QtGui -Ilexlib -I../../../../../Qt/4.5.3/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Qt4/qscilexerdiff.o Qt4/qscilexerdiff.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/qscintilla.exe

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc

/* 
 * File:   convertSeqFile.h
 * Author: ariizumi_nobuhiko
 *
 * Created on 2011/07/29
 */

#ifndef __CONVERT_SEQ_FILE_H__
#define __CONVERT_SEQ_FILE_H__


#include <rapidxml/gfl_Xml.h>

namespace SeqConv
{

bool ConverSequenceFile( gfl::xml::Document *seqFile , gfl::xml::Document *macroFile , const char* fileName );
bool OutputHeaderFile( gfl::xml::Document *macroFile , const char* fileBaseName );

};//SeqConv

#endif  //__CONVERT_SEQ_FILE_H__


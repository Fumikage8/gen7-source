/* 
 * File:   convertSeqFile.cpp
 * Author: ariizumi_nobuhiko
 *
 * Created on 2011/07/29
 */

#include <stdlib.h>		// _splitpath() のために必要
#include <time.h>
#include <iostream>
#include <fstream>

#include "convertSeqFile.h"
#include "converterDef.h"
#include "../save_xml_define.h"

#define USE_DEBUG_PRINT 0

namespace SeqConv
{

bool LoadData( CONVERTER_DATA *data , gfl::xml::Node *rootNode );
bool LoadCommandData( CONVERTER_DATA *data , gfl::xml::Node *node , gfl::xml::Node *grpNode );

void SortData( CONVERTER_DATA *data );

bool OutputData( CONVERTER_DATA *data , const char *fileName );
bool OutputHeader( CONVERTER_DATA *data , std::ofstream *file );
bool OutputMain( CONVERTER_DATA *data , std::ofstream *file );
bool OutputCommand( CONVERTER_DATA *data , std::ofstream *file , CommandData* com );
int  GetCommandIndex( gfl::xml::Node *macroListNode , char *comName );

void AllDump(CONVERTER_DATA *data );

bool ConverSequenceFile( gfl::xml::Document *seqFile , gfl::xml::Document *macroFile , const char* fileName )
{
  bool ret;
  CONVERTER_DATA data;

  data.mMacroNode = macroFile->RootNode( )->FirstNode( "macro" );
  data.mDefineNode= macroFile->RootNode( )->FirstNode( "define" );
  data.mPathDefNode= macroFile->RootNode( )->FirstNode( "path_define" );
  data.mGroupOptionNode = macroFile->RootNode( )->FirstNode( "group_option" );

  gfl::xml::Node *rootNode = seqFile->RootNode();

  ret = LoadData( &data , rootNode );
  SortData(&data);

  return OutputData( &data , fileName );
}

//xmlファイルの読み込み
bool LoadData( CONVERTER_DATA *data , gfl::xml::Node *rootNode )
{
  gfl::xml::Node *topNode = rootNode->FirstNode( XML_NAME_MAIN_NODE );
  if( !topNode )
  {
    return false;
  }
  SeqDebugPrint(0," Main\n");
  gfl::xml::Node *grpTopNode = topNode->FirstNode( XML_NAME_GROUP_DATA );
  if( !grpTopNode )
  {
    return false;
  }
  SeqDebugPrint(0,"  GrpTop\n");
  
  gfl::xml::Node *grpNode = grpTopNode->FirstNode( XML_NAME_GROUP );
  while( grpNode )
  {
    SeqDebugPrint(0,"   Grp\n");
    gfl::xml::Node *comNode = grpNode->FirstNode( XML_NAME_COMMAND );
    while( comNode )
    {
      SeqDebugPrint(0,"    Com\n");
      const bool ret = LoadCommandData( data,comNode,grpNode );
      if( !ret )
      {
        return false;
      }
      comNode = comNode->NextSibling( XML_NAME_COMMAND );
    }
    grpNode = grpNode->NextSibling( XML_NAME_GROUP );

  }

  return true;
}

//xmlファイルの読み込み(コマンド部分解釈
bool LoadCommandData( CONVERTER_DATA *data , gfl::xml::Node *node , gfl::xml::Node *grpNode )
{
  const int startFrame = node->AttributeInteger( XML_NAME_COMMAND_ATTR_SFRAME , 0 );
  const int endFrame   = node->AttributeInteger( XML_NAME_COMMAND_ATTR_EFRAME , 0 );
  const int isActive   = node->AttributeInteger( XML_NAME_COMMAND_ATTR_IS_ACITVE , 1 );
  
  CommandData *com = new CommandData(startFrame,endFrame,node,grpNode);
  if( isActive )
  {
    data->mComList.push_back(com);
  }
  return true;
}

//コマンドソート用比較関数
bool SortFunc(CommandData* a, CommandData* b)
{
  if(a->GetStartFrame() < b->GetStartFrame())
  {
    return true;
  }
  else
  {
    return false;
  }
}

//コマンドソート
void SortData( CONVERTER_DATA *data )
{
  AllDump(data);

  data->mComList.sort(SortFunc);

  AllDump(data);
}

//データ出力
bool OutputData( CONVERTER_DATA *data , const char *fileName )
{
  std::ofstream file(fileName , std::ios::out|std::ios::binary );
  if( file )
  {
    OutputHeader(data,&file);
    const bool ret = OutputMain(data,&file);
    /*
    int val1 = 100;
    file.write( (char*)&val1 , 2);
    val1 = 65535;
    file.write( (char*)&val1 , 4);
    float val2 = 3.14;
    file.write( (char*)&val2 , 4);
    val2 = 1984.0127;
    file.write( (char*)&val2 , 4);
    */
    file.close();
    return ret;
  }
  return false;
}

//データ出力(ヘッダー
bool OutputHeader( CONVERTER_DATA *data , std::ofstream *file )
{
  //識別ヘッダ
  char header[] = SEQ_EDITOR_DATA_HEADER;
  file->write( header , 4);
  //メインバージョン
  int mver = SEQ_EDITOR_MAIN_VERSION;
  file->write( (char*)&mver , 4 );
  //サブバージョン
  int sver = SEQ_EDITOR_SUB_VERSION;
  file->write( (char*)&sver , 4 );

  //最終フレーム
  int maxFarme = 0;
  ComDataListIt it = data->mComList.begin();
  while( it != data->mComList.end() )
  {
    CommandData *com = (*it);
    it++;

    if( maxFarme < com->GetEndFrame() )
    {
      maxFarme = com->GetEndFrame();
    }
  }
  file->write( (char*)&maxFarme , 4 );

  return true;
}

//データ出力(実データ
bool OutputMain( CONVERTER_DATA *data , std::ofstream *file )
{
  ComDataListIt it = data->mComList.begin();
  while( it != data->mComList.end() )
  {
    CommandData *com = (*it);
    it++;
    
    const bool ret = OutputCommand( data,file,com);
    if( ret == false )
    {
      return false;
    }
  }

  //終端データ
  int frame;
  frame = -1;
  file->write( (char*)&frame , 4);

  return true;
}

//データ出力(コマンド毎
bool OutputCommand( CONVERTER_DATA *data , std::ofstream *file , CommandData* com )
{
  gfl::xml::Node *comNode = com->GetComNode()->FirstNode();
  gfl::xml::Node *grpNode = com->GetGrpNode();
  gfl::xml::Node *macroNode = data->mMacroNode->FirstNode( comNode->name() );

  //フレーム
  int frame;
  frame = com->GetStartFrame();
  file->write( (char*)&frame , 4);
  frame = com->GetEndFrame();
  file->write( (char*)&frame , 4);

  u16 grpNo = grpNode->AttributeInteger( XML_NAME_GROUP_ATTR_NO , 0 );
  file->write( (char*)&grpNo , 2);

  QString grpOptName = QString(grpNode->AttributeString( XML_NAME_GROUP_ATTR_OPT , "" ));
  u16 grpOptNo = 0;
  if( grpOptName != "" )
  {
    gfl::xml::Node *optNode = data->mGroupOptionNode->FirstNode( grpOptName );
    if( optNode )
    {
      grpOptNo = optNode->AttributeInteger("value",0);
    }
  }
  file->write( (char*)&grpOptNo , 2);

  u16 isDebug = grpNode->AttributeInteger( XML_NAME_GROUP_ATTR_DEBUG , 0 );
  file->write( (char*)&isDebug , 2);
  
  //コマンド番号
  //@todo
  u16 comNo = GetCommandIndex(data->mMacroNode,comNode->name());
  file->write( (char*)&comNo , 2);
  //gfl::core::Debug::PrintConsole("[%d:%s][%d][%d]\n",comNo,comNode->name(),grpNo,grpOptNo);  
#if USE_DEBUG_PRINT
#endif

  gfl::xml::Node *node;
  //属性の出力
  //Macroデータを基準にしないと並び準が変わる・・・
	for( node = macroNode->FirstNode( ); node; node = node->NextSibling( ) )
  {
    gfl::xml::Node *valNode = comNode->FirstNode( node->name() );
    //gfl::core::Debug::PrintConsole("[%s][%d]\n",node->AttributeString("comment"),valNode->AttributeInteger("value0"));
#if USE_DEBUG_PRINT
    gfl::core::Debug::PrintConsole(" [%s]\n",node->name());  
#endif

		QString type( node->AttributeString( "type" ) );
    if( type == "VALUE_INT" ||
        type == "VALUE_ANGLE" ||
        type == "VALUE_SLIDER" ||
        type == "VALUE_CHECKBOX")
    {
      int val = node->AttributeInteger( "init0" );
      if( valNode )
      {
        val = valNode->AttributeInteger( "value0" );
      }
      file->write( (char*)&val , 4);
    }
    else
    if( type == "VALUE_FX32" )
    {
      float val = node->AttributeFloat( "init0" );
      if( valNode )
      {
        val = valNode->AttributeFloat( "value0" );
      }
      file->write( (char*)&val , 4);
    }
    else
    if( type == "VALUE_VECINT" )
    {
      int valx = node->AttributeInteger( "init0" );
      int valy = node->AttributeInteger( "init1" );
      int valz = node->AttributeInteger( "init2" );

      if( valNode )
      {
        valx = valNode->AttributeInteger( "value0" );
        valy = valNode->AttributeInteger( "value1" );
        valz = valNode->AttributeInteger( "value2" );
      }
      file->write( (char*)&valx , 4);
      file->write( (char*)&valy , 4);
      file->write( (char*)&valz , 4);
    }
    else
    if( type == "VALUE_VECFX32" )
    {
      float valx = node->AttributeFloat( "init0" );
      float valy = node->AttributeFloat( "init1" );
      float valz = node->AttributeFloat( "init2" );

      if( valNode )
      {
        valx = valNode->AttributeFloat( "value0" );
        valy = valNode->AttributeFloat( "value1" );
        valz = valNode->AttributeFloat( "value2" );
      }
      file->write( (char*)&valx , 4);
      file->write( (char*)&valy , 4);
      file->write( (char*)&valz , 4);
    }
    else
    if( type == "COMBOBOX_TEXT" )
    {
      const char *defStr = node->AttributeString("init",NULL);
      int val = 0;
      if( valNode )
      {
        const char *str = valNode->AttributeString("value0",NULL);
        if( str != NULL )
        {
          gfl::xml::Node *defNode = data->mDefineNode->FirstNode( str );
          if( defNode )
          {
            val = defNode->AttributeInteger( "value" );
          }
        }
        else 
        if( defStr )
        {
          gfl::xml::Node *defNode = data->mDefineNode->FirstNode( defStr );
          if( defNode )
          {
            val = defNode->AttributeInteger( "value" );
          }
        }
      }
      file->write( (char*)&val , 4);
    }
    else
    if( type == "VALUE_STRING" )
    {
      char workStr[512];
      memset(workStr, 0, 512);
      const char *str = valNode->AttributeString("value0");
      int len = node->AttributeInteger("length",SEQEDIT_VALUE_STR_LEN_DEFAULT);
      //gfl::core::Debug::PrintConsole("[%s][%d]\n",str,len);
      if( len > 512 )
      {
        gfl::core::Debug::PrintConsole("BufferOVer!!!\n");
        len = 512;
      }
      
      strcpy(workStr,str);
      file->write( workStr , len);
    }
    else
    if( type == "FILE_GARC" )
    {
      int val = 0;
      if( valNode )
      {
        const char *str = valNode->AttributeString("value0");
        
        gfl::xml::Node *defNode = data->mPathDefNode->FirstNode( node->AttributeString("file") );

        if( defNode )
        {
          QString path = ConvertPathDefine( data->mPathDefNode , node->AttributeString( "file" ) );
          QFile gaixFile(path);
          if( gaixFile.open(QIODevice::ReadOnly) )
          {

            QTextStream gaixText(&gaixFile); 
            int readSeq = 0;
            bool isHit = false;
            while( !gaixText.atEnd() )
            {
              QString str2 = gaixText.readLine();
              QStringList list = str2.split(" ", QString::SkipEmptyParts);
              //QString label = list[0].right(list[0].size()-6-baseName.size()); //[GARC_ファイル名_]を排除
              if( list.size() > 0 && str == list[0] )
              {
                isHit = true;
                val = list[2].left(list[2].size()-1 ).toInt();  //カンマ切り
  #if USE_DEBUG_PRINT
                gfl::core::Debug::PrintConsole("  [%s][%s]!!\n",str,list[2].left(list[2].size()-1 ).toLocal8Bit().constData());
  #endif
              }
              
            }
            if( isHit == false )
            {
              gfl::core::Debug::PrintConsole("gaix file[%s][%s] is not found!!\n",defNode->AttributeString("value"),str);
              return false;
            }
          }
          else
          {
            gfl::core::Debug::PrintConsole("gaix [%s] is not found!!\n",defNode->AttributeString("value"));
          }          
        }
      }
      file->write( (char*)&val , 4);

    } //( type == "FILE_GARC" )
    else
    if( type == "FILE_CSID" )
    {
      int val = 0;
      if( valNode )
      {
        const char *str = valNode->AttributeString("value0");
        
        gfl::xml::Node *defNode = data->mPathDefNode->FirstNode( node->AttributeString("file") );
        if( defNode )
        {
          QString defStr("Default");
          if( str == defStr )
          {
            val = node->AttributeInteger("use_default",0);
          }
          else
          {
            QString path = ConvertPathDefine( data->mPathDefNode , node->AttributeString( "file" ) );
            QFile gaixFile(path);
            if( gaixFile.open(QIODevice::ReadOnly) )
            {

              QTextStream gaixText(&gaixFile); 
              int readSeq = 0;
              while( !gaixText.atEnd() )
              {
                QString str2 = gaixText.readLine();
                QStringList list = str2.split(" ", QString::SkipEmptyParts);
                //QString label = list[0].right(list[0].size()-6-baseName.size()); //[GARC_ファイル名_]を排除
                if( list.size() > 0 && str == list[0] )
                {
                  bool ok;
                  QString temp = list[2].left(list[2].size()-1 );  //カンマ切り
                  val = temp.right(temp.size()-2).toInt(&ok,16);  //0x切り かつ 16進変換
  #if USE_DEBUG_PRINT
                  gfl::core::Debug::PrintConsole("  [%s][%s][%d]!!\n",str,temp.right(temp.size()-2).toLocal8Bit().constData(),val);
  #endif
                }
                
              }
            }
            else
            {
              gfl::core::Debug::PrintConsole("gaix [%s] is not found!!\n",defNode->AttributeString("value"));
            }          
          }
        }
      }
      else
      {
        //なかった時対応
        val = node->AttributeInteger("use_default",0);
      }
      file->write( (char*)&val , 4);
    }//( type == "FILE_CSID" )
    else
    if( type == "FILE_CSID_MID" )
    {
      int val = 0;
      if( valNode )
      {
        const char *str = valNode->AttributeString("value0");
        
        gfl::xml::Node *defNode = data->mPathDefNode->FirstNode( node->AttributeString("file") );
        if( defNode )
        {
          QString defStr("Default");
          if( str == defStr )
          {
            val = node->AttributeInteger("use_default",0);
          }
          else
          {
            QString path = ConvertPathDefine( data->mPathDefNode , node->AttributeString( "file" ) );
            QFile gaixFile(path);
            if( gaixFile.open(QIODevice::ReadOnly) )
            {

              QTextStream gaixText(&gaixFile); 
              int readSeq = 0;
              while( !gaixText.atEnd() )
              {
                QString str2 = gaixText.readLine();
                QStringList list = str2.split(" ", QString::SkipEmptyParts);
                //QString label = list[0].right(list[0].size()-6-baseName.size()); //[GARC_ファイル名_]を排除
                if( list.size() > 0 && str == list[0] )
                {
                  QString temp = list[2].left(list[2].size()-1 );  //カンマ切り
                  val = temp.toInt();  //0x切り かつ 16進変換
  #if USE_DEBUG_PRINT
                  gfl::core::Debug::PrintConsole("  [%s][%s][%d]!!\n",str,temp.right(temp.size()-2).toLocal8Bit().constData(),val);
  #endif
                }
                
              }
            }
            else
            {
              gfl::core::Debug::PrintConsole("gaix [%s] is not found!!\n",defNode->AttributeString("value"));
            }          
          }
        }
      }
      else
      {
        //なかった時対応
        val = node->AttributeInteger("use_default",0);
      }
      file->write( (char*)&val , 4);
    }//( type == "FILE_CSID_MID" )
    else
    if( type == "FILE_MESSAGE" )
    {
      int val = 0;
      if( valNode )
      {
        const char *str = valNode->AttributeString("value0");
        
        gfl::xml::Node *defNode = data->mPathDefNode->FirstNode( node->AttributeString("file") );
        if( defNode )
        {
          QString defStr("Default");
          if( str == defStr )
          {
            val = node->AttributeInteger("use_default",0);
          }
          else
          {
            QString path = ConvertPathDefine( data->mPathDefNode , node->AttributeString( "file" ) );
            QFile gaixFile(path);
            if( gaixFile.open(QIODevice::ReadOnly) )
            {

              QTextStream gaixText(&gaixFile); 
              int readSeq = 0;
              while( !gaixText.atEnd() )
              {
                QString str2 = gaixText.readLine();
                QStringList list = str2.split("\t", QString::SkipEmptyParts);
                //QString label = list[0].right(list[0].size()-6-baseName.size()); //[GARC_ファイル名_]を排除
                if( list.size() >= 2 && str == list[1] )
                {
                  bool ok;
                  QString temp = list[2].left(list[2].size()-1 );  //後ろ）切り
                  val = temp.right(temp.size()-1).toInt(&ok);  //前（切り
  #if USE_DEBUG_PRINT
                  gfl::core::Debug::PrintConsole("  [%s][%s][%d]!!\n",str,temp.right(temp.size()-1).toLocal8Bit().constData(),val);
  #endif
                }
                
              }
            }
            else
            {
              gfl::core::Debug::PrintConsole("gaix [%s] is not found!!\n",defNode->AttributeString("value"));
            }          
          }
        }
      }
      else
      {
        //なかった時対応
        val = node->AttributeInteger("use_default",0);
      }
      file->write( (char*)&val , 4);
    }//( type == "FILE_MESSAGE" )

    if( !valNode )
    {
      //gfl::core::Debug::PrintConsole(" [%d]->[%d][%s] : ",com->GetStartFrame(),com->GetEndFrame(),comNode->name());  
      //gfl::core::Debug::PrintConsole(" Value [%s] is not found!!\n",node->name());  
    }
  }
  return true;
}

int GetCommandIndex( gfl::xml::Node *macroListNode , char *comName )
{
  int i = 0;
  gfl::xml::Node *node;
	for( node = macroListNode->FirstNode( ); node; node = node->NextSibling( ) )
  {
    
    if( strcmp( node->name() , comName ) == 0 )
    {
      return i;
    }
    i++;
  }

  gfl::core::Debug::PrintConsole("[%s] is not found!\n",comName);  
  
  return -1;
}

#define OUTPUT_STREAM(stream,text) stream.write(text,strlen(text))

void ConvertFileNameToIncludeGuard( char* fileName , char* ret , int retLen )
{
  char drive[_MAX_DRIVE];	// ドライブ名
  char dir[_MAX_DIR];		// ディレクトリ名
  char fname[_MAX_FNAME];	// ファイル名
  char ext[_MAX_EXT];		// 拡張子

  _splitpath(fileName, drive, dir, fname, ext );
  
  int idx = 0;

  ret[idx] = '_';
  idx++;
  if( idx >= retLen )return;
  ret[idx] = '_';
  idx++;
  if( idx >= retLen )return;

  for( int i=0;i<(int)strlen(fname);i++ )
  {
    ret[idx] = toupper(fname[i]);
    if( ret[idx] == '.' )
    {
      ret[idx] = '_';
    }
    idx++;
    if( idx >= retLen )return;
  }

  for( int i=0;i<(int)strlen(ext);i++ )
  {
    ret[idx] = toupper(ext[i]);
    if( ret[idx] == '.' )
    {
      ret[idx] = '_';
    }
    idx++;
    if( idx >= retLen )return;
  }

  ret[idx] = '_';
  idx++;
  if( idx >= retLen )return;
  ret[idx] = '_';
  idx++;
  if( idx >= retLen )return;
  ret[idx] = '\0';
  idx++;
  if( idx >= retLen )return;

}

bool OutputHeaderFile( gfl::xml::Document *macroFile , const char* fileBaseName )
{
  gfl::xml::Node *defNode = macroFile->RootNode( )->FirstNode( "define" );
  gfl::xml::Node *grpOptNode = macroFile->RootNode( )->FirstNode( "group_option" );
  gfl::xml::Node *macroNode = macroFile->RootNode( )->FirstNode( "macro" );
  
  //デファイン出力
  char defFileName[128];
  strcpy( defFileName , fileBaseName );
  strcat( defFileName , "define.h" );

  time_t timer;
  time(&timer);
  tm *timeData = localtime(&timer);
  if( timeData == NULL )
  {
    return false;
  }

  static const int bufSize = 256;
  char tempStr[bufSize];
  std::ofstream defFile(defFileName , std::ios::out );
  if( defFile )
  {
    char drive[_MAX_DRIVE];	// ドライブ名
    char dir[_MAX_DIR];		// ディレクトリ名
    char fname[_MAX_FNAME];	// ファイル名
    char ext[_MAX_EXT];		// 拡張子

    _splitpath(defFileName, drive, dir, fname, ext );
    
    char incGuard[64];
    ConvertFileNameToIncludeGuard( defFileName , incGuard , 64 );


    OUTPUT_STREAM(defFile,"//======================================================================\n");
    OUTPUT_STREAM(defFile,"/**\n");
    sprintf_s(tempStr,bufSize," * @file   %s%s\n",fname,ext);
    OUTPUT_STREAM(defFile,tempStr);
    OUTPUT_STREAM(defFile," * @author SquenceConverter.exe\n");
    sprintf_s(tempStr,bufSize," * @data   %4d/%02d/%02d %2d:%02d\n",timeData->tm_year+1900,timeData->tm_mon,timeData->tm_mday,timeData->tm_hour,timeData->tm_min);
    OUTPUT_STREAM(defFile,tempStr);
    OUTPUT_STREAM(defFile,"*/\n");
    OUTPUT_STREAM(defFile,"//======================================================================\n");

    sprintf_s(tempStr,bufSize,"#ifndef %s\n",incGuard);
    OUTPUT_STREAM(defFile,tempStr);
    sprintf_s(tempStr,bufSize,"#define %s\n",incGuard);
    OUTPUT_STREAM(defFile,tempStr);
    
    if( defNode )
    {
      gfl::xml::Node *node;
      OUTPUT_STREAM(defFile,"\n");
      OUTPUT_STREAM(defFile,"//-------------------------------------------\n");
      OUTPUT_STREAM(defFile,"//Sequence内Define定義\n");
      OUTPUT_STREAM(defFile,"//-------------------------------------------\n");
      //属性の出力
	    for( node = defNode->FirstNode( ); node; node = node->NextSibling( ) )
      {
        char *name =node->name();
        int val = node->AttributeInteger("value",0);
        char *comment =node->AttributeString("comment","no_comment");

        sprintf_s(tempStr,bufSize,"#define %s (%d)  //%s\n",name,val,comment);
        OUTPUT_STREAM(defFile,tempStr);
      }
    }

    if( grpOptNode )
    {
      gfl::xml::Node *node;
      OUTPUT_STREAM(defFile,"\n");
      OUTPUT_STREAM(defFile,"//-------------------------------------------\n");
      OUTPUT_STREAM(defFile,"//Sequence内グループオプション定義\n");
      OUTPUT_STREAM(defFile,"//-------------------------------------------\n");
      //属性の出力
	    for( node = grpOptNode->FirstNode( ); node; node = node->NextSibling( ) )
      {
        char *name =node->name();
        int val = node->AttributeInteger("value",0);
        char *comment =node->AttributeString("label","no_comment");

        sprintf_s(tempStr,bufSize,"#define %s (%d)  //%s\n",name,val,comment);
        OUTPUT_STREAM(defFile,tempStr);
      }
    }

    OUTPUT_STREAM(defFile,"\n");

    OUTPUT_STREAM(defFile,"\n");
    OUTPUT_STREAM(defFile,"//SKIP使用時のコマンドサイズ配列\n");
    OUTPUT_STREAM(defFile,"\n");
    sprintf_s(tempStr,bufSize,"static const int %s_size_table[] =\n",fname);
    OUTPUT_STREAM(defFile,tempStr);
    OUTPUT_STREAM(defFile,"{\n");

    gfl::xml::Node *commandNode;
	  for( commandNode = macroNode->FirstNode( ); commandNode; commandNode = commandNode->NextSibling( ) )
    {
      char *comName = commandNode->name();

      gfl::xml::Node *node;
      int comSize = 0;
	    for( node = commandNode->FirstNode( ); node; node = node->NextSibling( ) )
      {
        char *name = node->AttributeString("type");
        if( strcmp( name ,"VALUE_INT" ) == 0 ||
            strcmp( name ,"VALUE_ANGLE" ) == 0 ||
            strcmp( name ,"VALUE_SLIDER" ) == 0||
            strcmp( name ,"VALUE_FX32" ) == 0||
            strcmp( name ,"VALUE_CHECKBOX" ) == 0||
            strcmp( name ,"COMBOBOX_TEXT" ) == 0||
            strcmp( name ,"FILE_GARC" ) == 0||
            strcmp( name ,"FILE_CSID" ) == 0||
            strcmp( name ,"FILE_CSID_MID" ) == 0||
            strcmp( name ,"FILE_MESSAGE" ) == 0 )
        {
          comSize += 4;
        }
        else
        if( strcmp( name ,"VALUE_VECINT" ) == 0 ||
            strcmp( name ,"VALUE_VECFX32" ) == 0 )
        {
          comSize += 12;
        }
        else
        if( strcmp( name ,"SPLIT_LINE" ) == 0 )
        {
          //no func!(サイズ0)
        }
        else
        if( strcmp( name ,"VALUE_STRING" ) == 0 )
        {
          int len = node->AttributeInteger("length",SEQEDIT_VALUE_STR_LEN_DEFAULT);
          comSize += len;
        }
        else
        {
          gfl::core::Debug::PrintConsole("Error![%s][%s]\n",comName,name);
        }
      }

      sprintf_s(tempStr,bufSize,"  %2d,//%s\n",comSize,comName);
      OUTPUT_STREAM(defFile,tempStr);

    }
    OUTPUT_STREAM(defFile,"};\n");

    sprintf_s(tempStr,bufSize,"#endif //%s\n",incGuard);
    OUTPUT_STREAM(defFile,tempStr);
  }

  return true;
}


//デバッグ用
void AllDump(CONVERTER_DATA *data )

{
#if USE_DEBUG_PRINT
  SeqDebugPrint( 1 , "AllDump---------------------------\n");

  ComDataListIt it = data->mComList.begin();
  while( it != data->mComList.end() )
  {
    CommandData *com = (*it);
    it++;

    com->Dump();
  }

  SeqDebugPrint( 1 , "AllDump---------------------------\n");
#endif
}


};



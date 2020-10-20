/* 
 * File:   localDef.h
 * Author: ariizumi_nobuhiko
 *
 * Created on 2011/03/01, 16:49
 */


#include "localDef.h"
#include "util.h"

int SeqEditor::mFrameWidth;
int SeqEditor::mFrameHeight;
int SeqEditor::mChipWidth;
int SeqEditor::mChipHeight;
int SeqEditor::mLineSize;
int SeqEditor::mTitleAreaSize;
bool SeqEditor::mDefaultLock;


DataValue::DataValue(SEQEDIT_DATA_TYPE type)
{
  CreateValue(type);
}

DataValue::~DataValue()
{
  if( mValue )
  {
    delete mValue;
  }
}

void DataValue::CreateValue(SEQEDIT_DATA_TYPE type)
{
  mType = type;
  switch( mType )
  {
  case SDT_INT:
    mValue = new int;
    break;
  case SDT_FLOAT:
    mValue = new float;
    break;
  case SDT_VEC3:
    mValue = new VEC3;
    break;
  case SDT_COLOR:
    mValue = new COLOR4;
    break;
  }
}

void DataValue::SetValueInt( int val )
{
  if( mType == SDT_INT )
  {
    *(int*)mValue = val;
  }
}

void DataValue::SetValueFloat( float val )
{
  if( mType == SDT_FLOAT )
  {
    *(float*)mValue = val;
  }
}

void DataValue::SetValueVec3( VEC3 *val )
{
  if( mType == SDT_VEC3 )
  {
    *(VEC3*)mValue = *val;
  }
}
void DataValue::SetValueVec3( float x,float y,float z )
{
  if( mType == SDT_VEC3 )
  {
    ((VEC3*)mValue)->Set(x,y,z);
  }
}

void DataValue::SetValueColor4( COLOR4 *val )
{
  if( mType == SDT_COLOR )
  {
    *(COLOR4*)mValue = *val;
  }
}
void DataValue::SetValueColor4( int r,int g,int b,int a )
{
  if( mType == SDT_COLOR )
  {
    ((COLOR4*)mValue)->Set(r,g,b,a);
  }
}

int DataValue::GetValueInt()
{
  if( mType == SDT_INT )
  {
    return *(int*)mValue;
  }
  return 0;
}

float DataValue::GetValueFloat()
{
  if( mType == SDT_FLOAT )
  {
    return *(float*)mValue;
  }
  return 0;
}

VEC3* DataValue::GetValueVec3()
{
  if( mType == SDT_VEC3 )
  {
    return (VEC3*)mValue;
  }
  return NULL;
}

COLOR4* DataValue::GetValueColor4()
{
  if( mType == SDT_COLOR )
  {
    return (COLOR4*)mValue;
  }
  return NULL;
}

QString ConvertPathDefine( gfl::xml::Node *pathDefNode , c8 *pathName )
{
  gfl::xml::Node *defNode = pathDefNode->FirstNode( pathName );
  QString path = defNode->AttributeString("value");

  c8 *parent = defNode->AttributeString("parent" , NULL );
  if( parent )
  {
    QString parentStr = ConvertPathDefine( pathDefNode , parent );
    path = parentStr + path;
  }
  else
  {
    c8 *envpath = defNode->AttributeString("envpath" , NULL );
    if( envpath )
    {
      QString envPathStr(std::getenv(envpath));
      path = envPathStr + path;
    }
  }

  return path;
}

#include <stdarg.h>

void SeqDebugPrint( int level , char *str , ... )
{

  if( level == 1 )
  {
	  const size_t GFL_DEBUG_BUFFER_SIZE = 0x200;
	  char tmp[GFL_DEBUG_BUFFER_SIZE];

	  va_list VA;
	  va_start( VA, str );
	  vsprintf( tmp, str, VA );
	  va_end( VA );

    gfl::core::Debug::PrintConsoleDirect( tmp );
  }
}

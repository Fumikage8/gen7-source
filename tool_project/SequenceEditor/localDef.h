/* 
 * File:   localDef.h
 * Author: ariizumi_nobuhiko
 *
 * Created on 2011/03/01, 16:49
 */

#ifndef LOCALDEF_H
#define	LOCALDEF_H

#include "util.h"
#include <rapidxml/gfl_Xml.h>

// SequenceEditorSequenceFile
#define SEQ_EDITOR_DATA_HEADER "SESD"
#define SEQ_EDITOR_MAIN_VERSION (3)
#define SEQ_EDITOR_SUB_VERSION  (0)

class WidSequenceBar;
class DataEditor;
class DataGroup;
class DataCommand;

typedef std::list<WidSequenceBar> SeqBarList;
typedef SeqBarList::iterator SeqBarListIt;

static const int SEQEDIT_FRAME_WIDTH = 16;
static const int SEQEDIT_FRAME_HEIGHT = 20;
static const int SEQEDIT_CHIP_WIDTH  =  6;
static const int SEQEDIT_CHIP_HEIGHT = 16;
static const int SEQEDIT_LINE_SIZE   = 4;
static const int SEQEDIT_TITLE_AREA_SIZE = 192;

//1個のコマンドが持つ引数の数
static const int SEQEDIT_DATA_MAX   = 5;

static const int SEQEDIT_TEMP_WIDTH  = 65536;

static const int SEQEDIT_VALUE_STR_LEN_DEFAULT = 32;

static const int SEQEDIT_MIN_FRAME_VAL = 0;
static const int SEQEDIT_MAX_FRAME_VAL = 65535;

class SeqEditor
{
public:
	static void Init()
	{
		mFrameWidth = SEQEDIT_FRAME_WIDTH;
		mFrameHeight = SEQEDIT_FRAME_HEIGHT;
		mChipWidth  = SEQEDIT_CHIP_WIDTH;
		mChipHeight = SEQEDIT_CHIP_HEIGHT;
		mLineSize   = SEQEDIT_LINE_SIZE;
    mTitleAreaSize = SEQEDIT_TITLE_AREA_SIZE;
    mDefaultLock = false;
	}

#define GETTER_MACRO(name,val) static int Get ## name (){return val;}static void Set ## name (const int temp){val = temp;}
/*
	static void SetFrameWidth(const int val){mFrameWidth = val;}
	static int GetFrameWidth(){return mFrameWidth;}
	static void SetFrameHeight(const int val){mFrameHeight = val;}
	static int GetFrameHeight(){return mFrameHeight;}
	static void SetChipWidth(const int val){mChipWidth = val;}
	static int GetChipWidth(){return mChipWidth;}
	static void SetChipHeight(const int val){mChipHeight = val;}
	static int GetChipHeight(){return mChipHeight;}
	static void SetLineSize(const int val){mLineSize = val;}
	static int GetLineSize(){return mLineSize;}
	static void SetTitleAreaSize(const int val){mTitleAreaSize = val;}
	static int GetTitleAreaSize(){return mTitleAreaSize;}
*/  
  GETTER_MACRO(FrameWidth,mFrameWidth)
  GETTER_MACRO(FrameHeight,mFrameHeight)
  GETTER_MACRO(ChipWidth,mChipWidth)
  GETTER_MACRO(ChipHeight,mChipHeight)
  GETTER_MACRO(LineSize,mLineSize)
  GETTER_MACRO(TitleAreaSize,mTitleAreaSize)

#undef GETTER_MACRO

  static bool GetIsDefaultLock(){return mDefaultLock;}
  static void SetIsDefaultLock(bool flg){mDefaultLock = flg;}

private:
  static int mFrameWidth;
  static int mFrameHeight;
  static int mChipWidth;
  static int mChipHeight;
  static int mLineSize;
  static int mTitleAreaSize;
  static bool mDefaultLock;
};

typedef enum
{
  SDT_NONE,

  SDT_INT,
  SDT_FLOAT,
  SDT_VEC3,
  SDT_COLOR,

  SDT_MAX
}SEQEDIT_DATA_TYPE;

class VEC3
{
public:
  VEC3()
    :x(0),y(0),z(0){}
  VEC3(float x,float y,float z)
    :x(x),y(y),z(z){}
  void Set( float tx,float ty,float tz){x=tx;y=ty;z=tz;}
  float x;
  float y;
  float z;
};

class COLOR4
{
public:
  COLOR4()
    :r(0),g(0),b(0),a(255){}
  COLOR4(u8 r,u8 g,u8 b,u8 a = 255)
    :r(r),g(g),b(b),a(a){}
  void Set(u8 tr,u8 tg,u8 tb,u8 ta = 255){r=tr;g=tg;b=tb;a=ta;}
  u8 r;
  u8 b;
  u8 g;
  u8 a;
};

class DataValue
{
public:
  DataValue(SEQEDIT_DATA_TYPE type);
  ~DataValue();
  
  void SetValueInt( int val );
  void SetValueFloat( float val );
  void SetValueVec3( VEC3 *val );
  void SetValueVec3( float x,float y,float z );
  void SetValueColor4( COLOR4 *val );
  void SetValueColor4( int r,int g,int b,int a=255 );

  int GetValueInt();
  float GetValueFloat();
  VEC3* GetValueVec3();
  COLOR4* GetValueColor4();

  SEQEDIT_DATA_TYPE GetDataType(){return mType;}
private:
  
  void CreateValue(SEQEDIT_DATA_TYPE type);

  SEQEDIT_DATA_TYPE mType;
  void *mValue;
};

typedef std::vector<DataValue*> DataValVec;

QString ConvertPathDefine( gfl::xml::Node *pathDefNode , c8 *pathName );

void SeqDebugPrint( int level , char *str , ... );

#endif	/* LOCALDEF_H */


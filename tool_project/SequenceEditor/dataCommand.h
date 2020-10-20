//======================================================================
/**
 * @file	dataCommand.h
 * @brief	
 * @author	ariizumi
 * @data	2011/03/07
 */
//======================================================================

#ifndef __DATA_COMMAND_H__
#define __DATA_COMMAND_H__

#include <gflib.h>
#include <rapidxml/gfl_Xml.h>
#include "localDef.h"

class WidSequenceBarCommand;
class TitleName;
class DataGroup;

typedef enum
{
  BAR_TYPE_NORMAL,
  BAR_TYPE_ONE_FRAME,
}COM_BAR_TYPE;

//----------------------------------------------------
class DataCommand
{
public:
	DataCommand(DataEditor *dataEditor);
	~DataCommand();

	void SetFrame( int start , int end );

	int GetStartFrame(){	return mStartFrame;	}
	int GetEndFrame()  {	return mEndFrame;   }
	int GetFrameLen()  {	return mEndFrame - mStartFrame;   }

	void SetTitleStr(QString &str){ mTitleStr = str; }
	const QString& GetTitleStr(){ return mTitleStr; }
	void SetCommentStr(QString &str);
	void SetCommentStr(c8 *str);
	const QString& GetCommentStr(){ return mCommentStr; }
	void SetCommandBar( WidSequenceBarCommand *bar ){ mBar = bar;}
	WidSequenceBarCommand* GetCommandBar(){ return mBar;}

	void SetParent( DataGroup *group ){ mParent = group;}
	DataGroup* GetParent(void){return mParent;}
  
  DataValue* AddDataValue( SEQEDIT_DATA_TYPE type );
  DataValue* GetDataValue( int idx );

  gfl::xml::Node* GetCommandNode(){return mComNode;}
  void SetCommandNode(gfl::xml::Node* node ){ mComNode = node;}
  void SetCommandNodeApplySetting(gfl::xml::Node* node );

  void SaveCommandData();
  bool LoadCommandData( gfl::xml::Node *node );
  
  void SetColor(QColor& col);
  QColor& GetColor(){return mChipCol;}

  void SetActive(bool flg){mIsActive = flg;SaveCommandData();}
  bool GetActive(void){return mIsActive;}

  void SetBarType( COM_BAR_TYPE type ){mBarType = type;}
  COM_BAR_TYPE GetBarType(){return mBarType;}

  bool IsLock();

private:
	int mStartFrame;
	int mEndFrame;
	
	QString mTitleStr;
  QString mCommentStr;
  DataValVec mDataVec;
  QColor mChipCol;
  bool mIsActive;

	TitleName					 *mTitle;
	WidSequenceBarCommand *mBar;
  COM_BAR_TYPE mBarType;

  DataEditor *mDataEditor;
	DataGroup *mParent;

  gfl::xml::Node *mComNode;
};

typedef std::list<DataCommand*> DataComList;
typedef DataComList::iterator DataComListIt;

#endif //__DATA_COMMAND_H__
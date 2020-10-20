//======================================================================
/**
 * @file	dataGroup.h
 * @brief	
 * @author	ariizumi
 * @data	2011/03/07
 */
//======================================================================

#ifndef __DATA_GROUP_H__
#define __DATA_GROUP_H__

#include <rapidxml/gfl_Xml.h>
#include "widgetSeqGroup.h"
#include "widgetTitleName.h"
#include "dataCommand.h"

class DataGroup : public QObject
{
	Q_OBJECT

public:
	DataGroup(DataEditor *dataEditor);
	~DataGroup();

	DataComList* GetCommandList(){return &mComList;};

	DataCommand* AddCommand( QString &type );
  //NextComの手前にComを入れる、NULLの場合は末尾
	void AddCommandDirect( DataCommand *com , DataCommand *nextCom = NULL ); //Undo・Redo用
	void DeleteCommand( DataCommand *com , bool isDelete = true );

  DataCommand* GetCommandByIndex( int idx );
  DataCommand* GetNextCommand( DataCommand* com );
  DataCommand* GetBeforeCommand( DataCommand* com );

	const QString& GetTitleStr(){ return mTitleStr; }
	void SetTitleStr( const QString &str );

  const int GetGroupNo(){ return mGroupNo; }
	void SetGroupNo( const int no );

  void SetOptionStr( QString str ){mOptStr = str;}
  QString GetOptionStr(void){return mOptStr;}

	const int GetCommandNum(){ return mComList.size(); }
	void SetWidgetGroup( WidSequenceGroup *group ){mGroup = group;}
	void SetWidgetTitle( WidTitleNameGroup *title ){mTitle = title;}
	WidSequenceGroup* GetSeqGroup(){return mGroup;}
	WidTitleNameGroup* GetTitleGroup(){return mTitle;}

	const bool IsOpen()const{return mIsOpen;}	
  void SetIsDebug(bool flg){mIsDebug = flg;}
	const bool IsDebug()const{return mIsDebug;}	

  void SaveGroupData();
  bool LoadGroupData( gfl::xml::Node *node , int *maxFrame );
  void SetNodeApplySetting(gfl::xml::Node* node );

  void SetXmlNode(gfl::xml::Node *node){mNode = node;}
  gfl::xml::Node* GetXmlNode(){return mNode;}
  gfl::xml::Node* AddCommandNode();
  void RemoveCommandNode(gfl::xml::Node* node );

  DataComList* GetComList(){return &mComList;}

  QColor& GetBgColor(){return mBgColor;}
  void SetBgColor(QColor &col){mBgColor = col;}

  void SetIsLock(bool flg){mIsLock = flg;}
  bool IsLock(){return mIsLock;}

  void Dump();

public Q_SLOTS:
	void OpenGroup();
	void CloseGroup();

Q_SIGNALS:
	void ChangedOpenGroup(bool flg);

private:
  DataEditor *mDataEditor;
	DataComList mComList;

	QString mTitleStr;
	bool		mIsOpen;
  bool    mIsDebug;
  int     mGroupNo;
	QString mOptStr;
  QColor  mBgColor;
  bool    mIsLock;

	WidSequenceGroup *mGroup;
	WidTitleNameGroup *mTitle;

  gfl::xml::Node *mNode;
};

typedef std::list<DataGroup*> DataGroupList;
typedef DataGroupList::iterator DataGroupListIt;

#endif //__DATA_GROUP_H__
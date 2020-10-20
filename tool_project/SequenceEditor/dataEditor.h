//======================================================================
/**
 * @file	dataGroup.h
 * @brief	
 * @author	ariizumi
 * @data	2011/03/07
 */
//======================================================================

#ifndef __DATA_EDITOR_H__
#define __DATA_EDITOR_H__

#include <rapidxml/gfl_Xml.h>
#include "dataGroup.h"
#include "dataMacro.h"
#include "dataAction.h"

class MainWindow;
class DataCommand;
class DataGroup;

class DataEditor : public QObject
{
	Q_OBJECT
public:
	DataEditor(MainWindow *mainWin);

  void UpdateDraw();
	
  void LoadMacroList( QString &filePath );

	int GetNowFrame(void)const{return mNowFrame;}
	int GetLimitFrame(void)const{return mLimitFrame;}

  MainWindow* GetMainWindow(){return mMainWin;}

	DataCommand* GetCurrentCommand(void){return mCurCommand;}
	void SetCurrentCommand(DataCommand *com);
	DataGroup* GetCurrentGroup(void){return mCurGroup;}
	void SetCurrentGroup(DataGroup *group );

  DataGroup* GetNextGroup( DataGroup* grp );
  DataGroup* GetBeforeGroup( DataGroup* grp );

  DataMacro* GetMacroData(){return &mMacroData;}
  DataActionManager* GetActionManager(){return &mActionManager;}

  void SetKeyHoldState(int flg){mKeyHoldFlg=flg;}
  int GetKeyHoldState(void){return mKeyHoldFlg;}/*Qt::Modifier*/

  void SetLockTempDelete( bool flg ){mLockTempDelete = flg;}

	void CheckDeleteReserve();

  void ClearData();
  
  void SetXmlData( gfl::xml::Document *doc);
  gfl::xml::Document* GetXmlData(){return mXmlData;}

  void MoveCommandUp( DataCommand* com , bool isAddAction = true );
  void MoveCommandDown( DataCommand* com , bool isAddAction = true );
  void MoveGroupUp( DataGroup* grp , bool isAddAction = true );
  void MoveGroupDown( DataGroup* grp , bool isAddAction = true );

  void FrameAddAll(int start,int value,bool isAddAction = true);

  void SaveCommandData( gfl::xml::Node *node );
  bool LoadCommandData( gfl::xml::Node *node );

  void UndoFunc();
  void RedoFunc();

  void UndoFunc_CreateCommand( ACTION_DATA *act );
  void UndoFunc_DeleteCommand( ACTION_DATA *act );
  void UndoFunc_ChangeCommand( ACTION_DATA *act );
  void UndoFunc_CreateGroup( ACTION_DATA *act );
  void UndoFunc_DeleteGroup( ACTION_DATA *act );
  void UndoFunc_ChangeGroup( ACTION_DATA *act );
  void UndoFunc_ChangeCommandFrame( ACTION_DATA *act );

  void CutFunc();
  void CopyFunc();
  void CopyFuncCommand(QString *strbuf,DataCommand* com);

  void PasteFunc();
  int PasteFunc_Command( DataGroup *grp , QStringList *strList , int startIdx , bool addAction );

public Q_SLOTS:
	void SetNowFrame(int val);
  void AddItemToCommandList(QTreeWidgetItem *item,int column);

Q_SIGNALS:
	void UpdateNowFrame(int);

private:
	DataGroup* AddGroup(); 
	DataGroup* InsertGroup(DataGroup *grp);
	void AddGroupCore(DataGroup* grp,DataGroup *nextGrp); 
	void DeleteGroup( DataGroup *grp , bool isDelete = true , bool isKeepCmd = false ); 

  void UpdateTargetCommand(void);
	void UpdateTargetGroup(void);
	DataGroup* GetTargetGroup(void);
	DataCommand* GetTargetCommand(void);

	int mNowFrame;
	int mLimitFrame;

  MainWindow *mMainWin;

	DataCommand *mCurCommand;
	DataGroup *mCurGroup;
	DataCommand *mTempDeleteCommand;
	DataGroup *mTempDeleteGroup;
  bool  mLockTempDelete;  //è„ÇÃêßå‰

  int mKeyHoldFlg;/*Qt::Modifier*/

	DataGroupList mGroupList;
  DataMacro     mMacroData;

  DataActionManager mActionManager;

  gfl::xml::Document *mXmlData;
  gfl::xml::Node     *mTopNode;
  gfl::xml::Node     *mGroupNode;

//------------------------------------------
//Action
public Q_SLOTS:
	void ActOpenGroup();
	void ActCloseGroup();
	void ActOpenAllGroup();
	void ActCloseAllGroup();
	void ActOpenCloseGroup();
	void ActAddGroup();
	void ActDeleteGroup();
	void ActEditGroup();
	void ActEditCommand();
	void ActDeleteCommand();
	void ActMoveGroupUp();
	void ActMoveGroupDown();
	void ActMoveCommandUp();
	void ActMoveCommandDown();
	void ActFrameAddAll();
  void ActFrameDelAll();
  void ActLockOn();
  void ActLockOff();
  void ActLockOnAll();
  void ActLockOffAll();
  void ActLockFlipCurrent();  //ñ¢ìoò^

public:
	void CreateAction(void); 

	QAction* GetActOpenGroup(void){return mActOpenGroup;}
	QAction* GetActCloseGroup(void){return mActCloseGroup;}
	QAction* GetActOpenAllGroup(void){return mActOpenAllGroup;}
	QAction* GetActCloseAllGroup(void){return mActCloseAllGroup;}
	QAction* GetActOpenCloseGroup(void){return mActOpenCloseGroup;}
	QAction* GetActAddGroup(void){return mActAddGroup;}
	QAction* GetActDeleteGroup(void){return mActDeleteGroup;}
	QAction* GetActEditGroup(void){return mActEditGroup;}
	QAction* GetActEditCommand(void){return mActEditCommand;}
	QAction* GetActDeleteCommand(void){return mActDeleteCommand;}
	QAction* GetActMoveGroupUp(void){return mActMoveGroupUp;}
	QAction* GetActMoveGroupDown(void){return mActMoveGroupDown;}
	QAction* GetActMoveCommandUp(void){return mActMoveCommandUp;}
	QAction* GetActMoveCommandDown(void){return mActMoveCommandDown;}
	QAction* GetActAddFrameAddAll(void){return mActFrameAddAll;}
  QAction* GetActAddFrameDelAll(void){return mActFrameDelAll;}
  QAction* GetActLockOff(void){return mActLockOff;}
  QAction* GetActLockOn(void){return mActLockOn;}
  QAction* GetActLockOffAll(void){return mActLockOffAll;}
  QAction* GetActLockOnAll(void){return mActLockOnAll;}

private:
	QAction	*mActOpenGroup;
	QAction	*mActCloseGroup;
	QAction	*mActOpenAllGroup;
	QAction	*mActCloseAllGroup;
	QAction	*mActOpenCloseGroup;
	QAction	*mActAddGroup;
	QAction	*mActDeleteGroup;
	QAction	*mActEditGroup;

  QAction	*mActEditCommand;
  QAction	*mActDeleteCommand;

  QAction	*mActMoveGroupUp;
  QAction	*mActMoveGroupDown;
  QAction	*mActMoveCommandUp;
  QAction	*mActMoveCommandDown;

  QAction	*mActFrameAddAll;
  QAction	*mActFrameDelAll;

  QAction	*mActLockOn;
  QAction	*mActLockOff;
  QAction	*mActLockOnAll;
  QAction	*mActLockOffAll;

};

#endif //__DATA_EDITOR_H__
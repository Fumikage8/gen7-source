//======================================================================
/**
 * @file	dataAction.h
 * @brief	UndoとRedoの管理
 * @author	ariizumi
 * @data	2011/10/25
 */
//======================================================================

#ifndef __DATA_ACTION_H__
#define __DATA_ACTION_H__

typedef enum
{
  //管理コマンド
  ACT_SYSTEM_START,
  ACT_SYSTEM_END,

  //アクションコマンド
  ACT_CHANGE_COMMAND,
  ACT_CREATE_COMMAND,
  ACT_DELETE_COMMAND,
  ACT_CREATE_GROUP,
  ACT_DELETE_GROUP,
  ACT_CHANGE_GROUP,
  ACT_CHANGE_COMMAND_FRAME,

  ACT_MOVEUP_COMMAND,
  ACT_MOVEDOWN_COMMAND,
  ACT_MOVEUP_GROUP,
  ACT_MOVEDOWN_GROUP,
}ACTION_TYPE;

typedef struct
{
  ACTION_TYPE mType;
  void           *mData;
  void           *mNextData;
  void           *mParentData;
  gfl::xml::Node *mNodeData;
  gfl::xml::Node *mNextNodeData;
  int   mValue[2];  //Frame用
}ACTION_DATA;

typedef std::list<ACTION_DATA*> DataActList;
typedef DataActList::iterator DataActListIt;

class DataCommand;
class DataGroup;

class DataActionManager
{
public:
  DataActionManager();
  ~DataActionManager();

  //アクションの記録開始、記録停止
  void StartSetAction();
  void EndSetAction();

  //個々のアクションの追加
  void ActionCreateCommand( DataCommand *com , DataGroup *parentGrp );
  void ActionDeleteCommand( DataCommand *com , DataGroup *parentGrp );
  void ActionChangeCommand( DataCommand *com , gfl::xml::Node *oldData , DataGroup *parentGrp );
  void ActionCreateGroup( DataGroup *grp , DataGroup *nextGrp );
  void ActionDeleteGroup( DataGroup *grp , DataGroup *nextGrp );
  void ActionChangeGroup( DataGroup *grp , gfl::xml::Node *oldData );
  void ActionChangeCommandFrame( DataCommand *com , const int befStart , const int befEnd );
  void ActionMoveUpCommand( DataCommand *com );
  void ActionMoveDownCommand( DataCommand *com );
  void ActionMoveUpGroup( DataGroup *grp );
  void ActionMoveDownGroup( DataGroup *grp );


  ACTION_DATA *GetCurrentAction();
  ACTION_DATA *GetNextAction();
  void MoveCurrentPrev();
  void MoveCurrentNext();

  void AllClear();

private:

  ACTION_DATA* CreateActionData( ACTION_TYPE type );
  void DeleteActionData( ACTION_DATA* act , bool isUndoed );
  void PushActionData( ACTION_DATA *act );

  DataActList mActList;
  DataActListIt mCurAction;
};

#endif //__DATA_ACTION_H__
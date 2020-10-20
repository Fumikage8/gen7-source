//======================================================================
/**
 * @file	dataAction.cpp
 * @brief	UndoとRedoの管理
 * @author	ariizumi
 * @data	2011/10/25
 */
//======================================================================

#include <rapidxml/gfl_Xml.h>
#include "dataAction.h"

#include "dataGroup.h"
#include "dataCommand.h"

DataActionManager::DataActionManager()
{
  mCurAction = mActList.end();
}

DataActionManager::~DataActionManager()
{
  
}

//アクションの記録開始、記録停止
void DataActionManager::StartSetAction()
{
  ACTION_DATA *data = CreateActionData(ACT_SYSTEM_START);

  PushActionData(data);
}

void DataActionManager::EndSetAction()
{
  ACTION_DATA *data = CreateActionData(ACT_SYSTEM_END);

  PushActionData(data);
}

//------------------------------------------------------------------------------
//個々のアクションの追加
//------------------------------------------------------------------------------
void DataActionManager::ActionCreateCommand( DataCommand *com , DataGroup *parentGrp )
{
  ACTION_DATA *data = CreateActionData(ACT_CREATE_COMMAND);
  
  data->mData = com;
  data->mParentData = parentGrp;
  data->mNodeData = com->GetCommandNode();
  
  data->mNextData = com->GetParent()->GetNextCommand(com);
  data->mNextNodeData = data->mNodeData->NextSibling();

  gfl::core::Debug::PrintConsole("[%x][%x]\n",data->mNodeData,data->mNextNodeData);

  PushActionData(data);
}

void DataActionManager::ActionDeleteCommand( DataCommand *com , DataGroup *parentGrp )
{
  ACTION_DATA *data = CreateActionData(ACT_DELETE_COMMAND);
  
  data->mData = com;
  data->mParentData = parentGrp;
  data->mNodeData = com->GetCommandNode();
  
  data->mNextData = com->GetParent()->GetNextCommand(com);
  data->mNextNodeData = data->mNodeData->NextSibling();

  PushActionData(data);
}

void DataActionManager::ActionChangeCommand( DataCommand *com , gfl::xml::Node *oldData , DataGroup *parentGrp )
{
  ACTION_DATA *data = CreateActionData(ACT_CHANGE_COMMAND);

  data->mData = com;
  data->mParentData = parentGrp;
  data->mNodeData = oldData;
  
  PushActionData(data);
}

void DataActionManager::ActionCreateGroup( DataGroup *grp , DataGroup *nextGrp )
{
  ACTION_DATA *data = CreateActionData(ACT_CREATE_GROUP);

  data->mData = grp;
  data->mNextData = nextGrp;
  data->mNodeData = grp->GetXmlNode();

  PushActionData(data);
}

void DataActionManager::ActionDeleteGroup( DataGroup *grp , DataGroup *nextGrp )
{
  ACTION_DATA *data = CreateActionData(ACT_DELETE_GROUP);

  data->mData = grp;
  data->mNextData = nextGrp;
  data->mNodeData = grp->GetXmlNode();

  PushActionData(data);
}

void DataActionManager::ActionChangeGroup( DataGroup *grp , gfl::xml::Node *oldData )
{
  ACTION_DATA *data = CreateActionData(ACT_CHANGE_GROUP);

  data->mData = grp;
  data->mNodeData = oldData;
  
  PushActionData(data);
}

void DataActionManager::ActionChangeCommandFrame( DataCommand *com , const int befStart , const int befEnd )
{
  ACTION_DATA *data = CreateActionData(ACT_CHANGE_COMMAND_FRAME);

  data->mData = com;
  data->mValue[0] = befStart;
  data->mValue[1] = befEnd;

  PushActionData(data);
}

void DataActionManager::ActionMoveUpCommand( DataCommand *com )
{
  ACTION_DATA *data = CreateActionData(ACT_MOVEUP_COMMAND);

  data->mData = com;

  PushActionData(data);
}
void DataActionManager::ActionMoveDownCommand( DataCommand *com )
{
  ACTION_DATA *data = CreateActionData(ACT_MOVEDOWN_COMMAND);

  data->mData = com;

  PushActionData(data);
}

void DataActionManager::ActionMoveUpGroup( DataGroup *grp )
{
  ACTION_DATA *data = CreateActionData(ACT_MOVEUP_GROUP);

  data->mData = grp;

  PushActionData(data);
}

void DataActionManager::ActionMoveDownGroup( DataGroup *grp )
{
  ACTION_DATA *data = CreateActionData(ACT_MOVEDOWN_GROUP);

  data->mData = grp;

  PushActionData(data);
}

//------------------------------------------------------------------------------
//  処理系
//------------------------------------------------------------------------------
ACTION_DATA* DataActionManager::GetCurrentAction()
{
  if( mCurAction == mActList.begin() )
  {
    return NULL;
  }
  else
  {
    DataActListIt tempAct = mCurAction;
    tempAct--;
    return (ACTION_DATA*)(*tempAct);
  }
}

ACTION_DATA* DataActionManager::GetNextAction()
{
  if( mCurAction == mActList.end() )
  {
    return NULL;
  }
  else
  {
    return (ACTION_DATA*)(*mCurAction);
  }
}


ACTION_DATA* DataActionManager::CreateActionData( ACTION_TYPE type )
{
  ACTION_DATA *data = new ACTION_DATA;
  data->mType = type;
  data->mData = NULL;
  data->mNextData = NULL;
  data->mParentData = NULL;
  data->mNodeData = NULL;
  data->mValue[0] = 0;
  data->mValue[1] = 0;
  
  return data;
}

void DataActionManager::DeleteActionData( ACTION_DATA* act , bool isUndoed )
{
  if( isUndoed )
  {
    //Undoされた処理が通る場所
    switch( act->mType )
    {
    case ACT_CHANGE_COMMAND:
      break;
    case ACT_CREATE_COMMAND:
      {
        DataCommand *com = (DataCommand*)act->mData;
        delete com;
      }
      break;
    case ACT_DELETE_COMMAND:
      break;
    case ACT_CREATE_GROUP:
      {
        DataGroup *grp = (DataGroup*)act->mData;
        delete grp;
      }
      break;
    case ACT_DELETE_GROUP:
      break;
    case ACT_CHANGE_COMMAND_FRAME:
      break;
    }
  }
  else
  {
    switch( act->mType )
    {
    case ACT_CHANGE_COMMAND:
      break;
    case ACT_CREATE_COMMAND:
      break;
    case ACT_DELETE_COMMAND:
      {
        DataCommand *com = (DataCommand*)act->mData;
        delete com;
      }
      break;
    case ACT_CREATE_GROUP:
      break;
    case ACT_DELETE_GROUP:
      {
        DataGroup *grp = (DataGroup*)act->mData;
        delete grp;
      }
      break;
    case ACT_CHANGE_COMMAND_FRAME:
      break;
    }
  }

  delete act;
}

void DataActionManager::MoveCurrentPrev()
{
  if( mCurAction != mActList.begin() )
  {
    mCurAction--;
  }
}

void DataActionManager::MoveCurrentNext()
{
  if( mCurAction != mActList.end() )
  {
    mCurAction++;
  }
}

void DataActionManager::AllClear()
{
  bool isUndoed = false;
  DataActListIt it = mActList.begin();
  while( it != mActList.end() )
  {
    ACTION_DATA *actData = *it;
    
    if( isUndoed == false && 
        it == mCurAction )
    {
      isUndoed = true;
    }

    it++;

    mActList.remove( actData );
    DeleteActionData( actData , isUndoed);
  }
  mCurAction = mActList.end();
}


void DataActionManager::PushActionData( ACTION_DATA *act )
{
  DataActListIt tempAct = mCurAction;
  while( tempAct != mActList.end() )
  {
    ACTION_DATA *actData = *tempAct;
    tempAct++;
  
    mActList.remove( actData );
    DeleteActionData( actData , true  );
  }

  mActList.push_back( act );
  mCurAction = mActList.end();
  //mCurAction--;

//  gfl::core::Debug::PrintConsole("[%x][%x]\n",act,(ACTION_DATA*)(*mCurAction));
//  gfl::core::Debug::PrintConsole("[%x][%x]\n",(ACTION_DATA*)(*mActList.begin()),(ACTION_DATA*)(*mCurAction));
}


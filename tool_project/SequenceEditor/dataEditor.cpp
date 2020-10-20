//======================================================================
/**
 * @file	dataGroup.cpp
 * @brief	
 * @author	ariizumi
 * @data	2011/03/07
 */
//======================================================================
#include <algorithm>

#include "MainWindow.h"
#include "dataEditor.h"
#include "dataGroup.h"
#include "widgetTitleName.h"
#include "dialogAddGroup.h"
#include "dialogMoveFrame.h"
#include "macroDialog.h"
#include "save_xml_define.h"

DataEditor::DataEditor(MainWindow *mainWin)
	:mNowFrame(0)
	,mLimitFrame(54000)	//60*60*15
  ,mMainWin(mainWin)
	,mCurCommand(NULL)
	,mCurGroup(NULL)
  ,mTempDeleteCommand(NULL)
  ,mTempDeleteGroup(NULL)
  ,mMacroData(this)
  ,mXmlData(NULL)
  ,mKeyHoldFlg(0)
  ,mLockTempDelete(false)
{
  mXmlData = new gfl::xml::Document();
  mTopNode = mXmlData->RootNode()->AddNode( XML_NAME_MAIN_NODE );
  mGroupNode = mTopNode->AddNode( XML_NAME_GROUP_DATA );
}

void DataEditor::UpdateDraw()
{
  DataGroupListIt grpIt = mGroupList.begin();
  while( grpIt != mGroupList.end() )
  {
    DataGroup *grpData = (*grpIt);
    grpData->GetTitleGroup()->update();
    grpData->GetSeqGroup()->update();
    grpData->GetTitleGroup()->updateGeometry();
    grpData->GetSeqGroup()->updateGeometry();
    grpIt++;
  }
}

void DataEditor::LoadMacroList( QString &filePath )
{
  mMacroData.LoadListXml(filePath);
}

//エディタ上のフレームの設定
void DataEditor::SetNowFrame(int val)
{
	if( val != mNowFrame )
	{
		mNowFrame = val;
		UpdateNowFrame(mNowFrame);
	}
}

//コマンド追加
void DataEditor::AddItemToCommandList(QTreeWidgetItem *item,int column)
{
  QString str = item->text(0);
  gfl::core::Debug::PrintConsole("[%d]%s\n",item->data(0,Qt::UserRole).toInt() ,str.toLocal8Bit().constData());
	if( GetCurrentGroup() )
	{
    gfl::xml::Node *macroNode = mMacroData.GetMacroNode( item->data(0,Qt::UserRole).toInt() );

    QString parentSetting(macroNode->AttributeString( "pos" )); 
    if( parentSetting == "parent" )
    {
      //トップグループは使わない。
      return;
    }
    if( GetCurrentGroup()->IsLock() )
    {
      mMainWin->PopLockWarnning();
      return;
    }

    gfl::xml::Node *comNode = GetCurrentGroup()->AddCommandNode();
    comNode->SetAttribute( XML_NAME_COMMAND_ATTR_SFRAME , GetNowFrame() );
    comNode->SetAttribute( XML_NAME_COMMAND_ATTR_EFRAME , GetNowFrame() );
    
    QColor col;
    col.setNamedColor( macroNode->AttributeString( "color" , "#FF0000" ));
    int r,g,b;
    col.getRgb(&r,&g,&b);
    comNode->SetAttribute( XML_NAME_COMMAND_ATTR_COL_R , r );
    comNode->SetAttribute( XML_NAME_COMMAND_ATTR_COL_G , g );
    comNode->SetAttribute( XML_NAME_COMMAND_ATTR_COL_B , b );

    MacroDialog dialog( GetMainWindow() , macroNode , comNode , mMacroData.GetDefineNode(), mMacroData.GetPathDefNode() ); 

		int ret = dialog.exec();
		if( ret == QDialog::Accepted )
		{
      QString type = macroNode->AttributeString( "type" , "normal" );
      DataCommand *com = GetCurrentGroup()->AddCommand( type );
      com->SetCommandNodeApplySetting( comNode );

      mActionManager.StartSetAction();
      mActionManager.ActionCreateCommand(com,GetCurrentGroup());
      mActionManager.EndSetAction();

		  mMainWin->GetTitleAreaWidget()->update();
    }
    else
    {
      GetCurrentGroup()->RemoveCommandNode( comNode );
    }
  }
  else
  {
    //@todo　警告ダイアログ
  }
}

//選択されているコマンドの設定
void DataEditor::SetCurrentCommand(DataCommand *com)
{
	DataCommand *oldCom = mCurCommand;
	mCurCommand = com;

	if( oldCom != NULL )
	{
    oldCom->GetCommandBar()->SetIsFocus(false);
		oldCom->GetParent()->GetSeqGroup()->update();
		oldCom->GetParent()->GetTitleGroup()->update();
	}
	if( mCurCommand != NULL )
	{
    mCurCommand->GetCommandBar()->SetIsFocus(true);
		mCurCommand->GetParent()->GetSeqGroup()->update();
		mCurCommand->GetParent()->GetTitleGroup()->update();
	}
	UpdateTargetCommand();
}

//選択されているグループの設定
void DataEditor::SetCurrentGroup(DataGroup *group )
{
	DataGroup *oldGrp = mCurGroup;
	mCurGroup = group;
	if( oldGrp != NULL )
	{
		oldGrp->GetSeqGroup()->update();
		oldGrp->GetTitleGroup()->update();
	}
	if( mCurGroup != NULL )
	{
		mCurGroup->GetSeqGroup()->update();
		mCurGroup->GetTitleGroup()->update();
	}
	UpdateTargetGroup();
}

//削除予約(メニューの処理内で行うと自身を削除する場合があるため)
void DataEditor::CheckDeleteReserve()
{
  if( mLockTempDelete )
  {
    return;
  }
  if( mTempDeleteCommand )
  {
    mActionManager.StartSetAction();
    mActionManager.ActionDeleteCommand(mTempDeleteCommand,mTempDeleteCommand->GetParent());
    mActionManager.EndSetAction();

    mTempDeleteCommand->GetParent()->DeleteCommand( mTempDeleteCommand , false );

    mTempDeleteCommand = NULL;
  }
  if( mTempDeleteGroup )
  {
    mActionManager.StartSetAction();


    DataComList* comList = mTempDeleteGroup->GetComList();
    DataComListIt it = comList->begin();
    while( it != comList->end() )
    {
      DataCommand *com = (*it);
      it++;
      mActionManager.ActionDeleteCommand(com,com->GetParent());
    }
    mActionManager.ActionDeleteGroup( mTempDeleteGroup ,GetNextGroup(mTempDeleteGroup));
    mActionManager.EndSetAction();

    DeleteGroup( mTempDeleteGroup , false );
    mTempDeleteGroup = NULL;
  }
}

//CurrentCommand変更時の処理
void DataEditor::UpdateTargetCommand(void)
{
  if( GetTargetCommand() != NULL )
  {
    mActEditCommand->setEnabled(true);
    mActDeleteCommand->setEnabled(true);

    DataCommand *com = GetTargetCommand();
    DataGroup *grp = com->GetParent();
    DataComList *comList = grp->GetComList();

    if( (*comList->begin()) != com )
    {
      mActMoveCommandUp->setEnabled(true);
    }
    else
    {
      mActMoveCommandUp->setEnabled(false);
    }
    DataComListIt tempIt = comList->end();
    tempIt--;
    if( (*tempIt) != com )
    {
      mActMoveCommandDown->setEnabled(true);
    }
    else
    {
      mActMoveCommandDown->setEnabled(false);
    }
  }
  else
  {
    mActEditCommand->setEnabled(false);
    mActDeleteCommand->setEnabled(false);
    mActMoveCommandUp->setEnabled(false);
    mActMoveCommandDown->setEnabled(false);
  }
}

//CurrentGroup変更時の処理
void DataEditor::UpdateTargetGroup(void)
{
	if( GetTargetGroup() != NULL )
	{
		if( GetTargetGroup()->IsOpen() )
		{
			mActOpenGroup->setEnabled(false);
			mActCloseGroup->setEnabled(true);
		}
		else
		{
			mActOpenGroup->setEnabled(true);
			mActCloseGroup->setEnabled(false);
		}
    mActDeleteGroup->setEnabled(true);
    
    DataGroup *grp = GetTargetGroup();
    DataGroupListIt it = mGroupList.begin();
    if( (*mGroupList.begin()) != grp )
    {
      mActMoveGroupUp->setEnabled(true);
    }
    else
    {
      mActMoveGroupUp->setEnabled(false);
    }

    DataGroupListIt tempIt = mGroupList.end();
    tempIt--;
    if( (*tempIt) != grp )
    {
      mActMoveGroupDown->setEnabled(true);
    }
    else
    {
      mActMoveGroupDown->setEnabled(false);
    }
	}
	else
	{
		mActOpenGroup->setEnabled(false);
		mActCloseGroup->setEnabled(false);
    mActDeleteGroup->setEnabled(false);
    mActMoveGroupUp->setEnabled(false);
    mActMoveGroupDown->setEnabled(false);
	}
	UpdateTargetCommand();
}

//今はほぼ意味無い
DataGroup* DataEditor::GetTargetGroup(void)
{
	return mCurGroup;
}

//今はほぼ意味無い
DataCommand* DataEditor::GetTargetCommand(void)
{
	return mCurCommand;
}

//グループの追加
DataGroup* DataEditor::AddGroup()
{
  DataGroup *grp = new DataGroup(this);

  AddGroupCore(grp,NULL);

  return grp;
}

//グループの挿入
DataGroup* DataEditor::InsertGroup(DataGroup *befGrp)
{
  DataGroup *grp = new DataGroup(this);

  int idx = 0;
  DataGroupListIt grpIt = mGroupList.begin();
  while( grpIt != mGroupList.end() )
  {
    DataGroup *grpData = (*grpIt);
    if( grpData == befGrp )
    {
      break;
    }
    idx++;
    grpIt++;
  }

  if( grpIt != mGroupList.end() )
  {
    AddGroupCore(grp,(DataGroup*)(*grpIt));
  }
  else
  {
    AddGroupCore(grp,NULL);
  }

  return grp;
}

//グループの追加(コア処理
void DataEditor::AddGroupCore(DataGroup* grp,DataGroup *nextGrp)
{
	WidSequenceGroup *groupWidget = new WidSequenceGroup(this, grp );
	WidTitleNameGroup *groupTitle = new WidTitleNameGroup(this , grp);

  if( nextGrp )
  {
    DataGroupListIt grpIt = mGroupList.begin();
    while( grpIt != mGroupList.end() )
    {
      DataGroup *tempGrp = (*grpIt);
      if( tempGrp == nextGrp )
      {
        break;
      }
      grpIt++;
    }
    mGroupList.insert(grpIt,grp);

    const int barIdx = mMainWin->GetBarLytWidget()->indexOf( nextGrp->GetSeqGroup() );
    mMainWin->GetBarLytWidget()->insertWidget(barIdx,groupWidget);
    const int titleIdx = mMainWin->GetTitleAreaLytWidget()->indexOf( nextGrp->GetTitleGroup() );
	  mMainWin->GetTitleAreaLytWidget()->insertWidget(titleIdx,groupTitle );
  }
  else
  {
  	mGroupList.push_back(grp);

	  mMainWin->GetBarLytWidget()->addWidget(groupWidget);
	  mMainWin->GetTitleAreaLytWidget()->addWidget(groupTitle );
  }

	grp->SetWidgetGroup(groupWidget);
	grp->SetWidgetTitle(groupTitle);

  SetCurrentGroup( grp );
}

//グループの削除
void DataEditor::DeleteGroup( DataGroup *grp , bool isDelete , bool isKeepCmd )
{
  if( GetCurrentGroup() == grp )
  {
    SetCurrentGroup( NULL );
  }

  gfl::xml::Node *node = grp->GetXmlNode();
  mGroupNode->remove_node( node );

  mGroupList.remove( grp );

  DataComList* comList = grp->GetComList();

  if( isKeepCmd == false )
  {
    DataComListIt it = comList->begin();
    while( it != comList->end() )
    {
      DataCommand *com = (*it);
      it++;
      grp->DeleteCommand( com , isDelete );
    }
  }

  delete grp->GetSeqGroup();
  delete grp->GetTitleGroup();


  //delete grp;
}

DataGroup* DataEditor::GetNextGroup( DataGroup* grp )
{
  DataGroupListIt it = mGroupList.begin();
  while( it != mGroupList.end() )
  {
    DataGroup *grpBef = (*it);
    it++;
    if( grp == grpBef &&
        it != mGroupList.end() )
    {
      return (DataGroup*)(*it);
    }
  }

  return NULL;
}

DataGroup* DataEditor::GetBeforeGroup( DataGroup* grp )
{
  DataGroupListIt it = mGroupList.begin();
  DataGroupListIt befIt = mGroupList.begin();
  while( it != mGroupList.end() )
  {
    //grp2はなぜか定義されている・・・
    DataGroup *grp_2 = (*it);
    if( grp == grp_2 &&
        it != mGroupList.begin() )
    {
      return (DataGroup*)(*befIt);
    }
    befIt = it;
    it++;
  }

  return NULL;
}


//データクリア
void DataEditor::ClearData()
{
  DataGroupListIt grpIt = mGroupList.begin();
  while( grpIt != mGroupList.end() )
  {
    DataGroup *grpData = (*grpIt);
    grpIt++;

    DeleteGroup( grpData );
  }
  mGroupNode->remove_all_nodes();
  
  mActionManager.AllClear();
}

void DataEditor::SetXmlData( gfl::xml::Document *doc)
{
  mXmlData = doc;
}


//コマンド入れ替え(上移動)
void DataEditor::MoveCommandUp( DataCommand* com , bool isAddAction )
{
  if( com )
  {
    if( com->IsLock() )
    {
      return;
    }

    DataGroup *grp = com->GetParent();
    DataComList *comList = grp->GetComList();

    if( (*comList->begin()) != com )
    {
      DataComListIt it = comList->begin();
      while( it != comList->end() )
      {
        if( (*it) == com )
        {
          DataCommand *befCom = grp->GetBeforeCommand(com);
          if( befCom )
          {
            grp->DeleteCommand(com,false);
            grp->AddCommandDirect(com,befCom);
            com->SetCommandNodeApplySetting(com->GetCommandNode());
            if( isAddAction )
            {
              mActionManager.StartSetAction();
              mActionManager.ActionMoveUpCommand(com);
              mActionManager.EndSetAction();
            }
          }
          break;
        }
        it++;
      }
    }
  }
}

void DataEditor::MoveCommandDown( DataCommand* com , bool isAddAction )
{
  if( com )
  {
    if( com->IsLock() )
    {
      return;
    }

    DataGroup *grp = com->GetParent();
    DataComList *comList = grp->GetComList();
    
    DataComListIt tempIt = comList->end();
    tempIt--;
    if( (*tempIt) != com )
    {
      DataComListIt it = comList->begin();
      while( it != comList->end() )
      {
        if( (*it) == com )
        {
          DataCommand *nextCom = grp->GetNextCommand(com);
          if( nextCom )
          {
            grp->DeleteCommand(com,false);
            grp->AddCommandDirect(com,grp->GetNextCommand(nextCom));
            com->SetCommandNodeApplySetting(com->GetCommandNode());
            if( isAddAction )
            {
              mActionManager.StartSetAction();
              mActionManager.ActionMoveDownCommand(com);
              mActionManager.EndSetAction();
            }
          }
          break;
        }
        it++;
      }
    }
  }
}


//グループ入れ替え(上移動)
void DataEditor::MoveGroupUp( DataGroup* grp , bool isAddAction )
{
  if( grp )
  {
    if( grp->IsLock() )
    {
      return;
    }

    if( (*mGroupList.begin()) != grp )
    {
      DataGroupListIt it = mGroupList.begin();
      while( it != mGroupList.end() )
      {
        if( (*it) == grp )
        {
          DataGroup *befGrp = GetBeforeGroup(grp);
          if( befGrp )
          {
            gfl::xml::Node* node = grp->GetXmlNode();
            int comNum = grp->GetCommandNum();
            DataCommand** comNodeArr = NULL;
            if( comNum > 0 )
            {
              comNodeArr = new DataCommand*[comNum];
              
              for( int i=0;i<comNum;i++ )
              {
                comNodeArr[i] = grp->GetCommandByIndex(i);
              }
            }

            DeleteGroup(grp,false);
            //AddGroupCore(grp,befGrp);

            AddGroupCore(grp,befGrp);
            mGroupNode->insert_node( befGrp->GetXmlNode() , node );
            grp->SetXmlNode(node);

            if( comNum > 0 )
            {
              for( int i=0;i<comNum;i++ )
              {
                grp->AddCommandDirect(comNodeArr[i]);
                comNodeArr[i]->SetCommandNodeApplySetting(comNodeArr[i]->GetCommandNode());
              }
              delete [] comNodeArr;
            }
            
            if( isAddAction )
            {
              mActionManager.StartSetAction();
              mActionManager.ActionMoveUpGroup(grp);
              mActionManager.EndSetAction();
            }
          }
          break;
        }
        it++;
      }
    }
  }
}


//グループ入れ替え(下移動)
void DataEditor::MoveGroupDown( DataGroup* grp , bool isAddAction )
{
  if( grp )
  {
    if( grp->IsLock() )
    {
      return;
    }

    DataGroupListIt tempIt = mGroupList.end();
    tempIt--;
    if( (*tempIt) != grp )
    {
      DataGroupListIt it = mGroupList.begin();
      while( it != mGroupList.end() )
      {
        if( (*it) == grp )
        {
          DataGroup *nextGrp = GetNextGroup(grp);
          if( nextGrp )
          {
            gfl::xml::Node* node = grp->GetXmlNode();
            int comNum = grp->GetCommandNum();
            
            DataCommand** comNodeArr = NULL;
            if( comNum > 0 )
            {
              comNodeArr = new DataCommand*[comNum];
              
              for( int i=0;i<comNum;i++ )
              {
                comNodeArr[i] = grp->GetCommandByIndex(i);
              }
            }

            DeleteGroup(grp,false);
            //AddGroupCore(grp,befGrp);

            DataGroup *nextGrp2 = GetNextGroup(nextGrp);
            if( nextGrp2 )
            {
              AddGroupCore(grp,nextGrp2);
              mGroupNode->insert_node( nextGrp2->GetXmlNode() , node );
            }
            else
            {
              AddGroupCore(grp,NULL);
              mGroupNode->append_node( node );
            }
            grp->SetXmlNode(node);

            if( comNum > 0 )
            {
              for( int i=0;i<comNum;i++ )
              {
                grp->AddCommandDirect(comNodeArr[i]);
                comNodeArr[i]->SetCommandNodeApplySetting(comNodeArr[i]->GetCommandNode());
              }
              delete [] comNodeArr;
            }
            
            if( isAddAction )
            {
              mActionManager.StartSetAction();
              mActionManager.ActionMoveDownGroup(grp);
              mActionManager.EndSetAction();
            }
          }
          break;
        }
        it++;
      }
    }
  }
}

void DataEditor::FrameAddAll(int start,int value,bool isAddAction )
{
  DataGroupListIt grpIt = mGroupList.begin();

  while( grpIt != mGroupList.end() )
  {
    DataGroup *grpData = (*grpIt);
    if( grpData->IsLock() )
    {
      GetMainWindow()->PopLockWarnning();
      return;
    }
    grpIt++;
  }

  if( isAddAction )
  {
    mActionManager.StartSetAction();
  }

  grpIt = mGroupList.begin();
  while( grpIt != mGroupList.end() )
  {
    DataGroup *grpData = (*grpIt);
    
  	DataComList *comList = grpData->GetCommandList();
    DataComListIt comIt = comList->begin();
    while( comIt != comList->end() )
    {
      DataCommand *comData = (*comIt);
      
      int sFrame = comData->GetStartFrame();
      int eFrame = comData->GetEndFrame();
      if( sFrame >= start )
      {
        sFrame += value;
        if( sFrame < start )
        {
          sFrame = start;
        }
      }
      if( eFrame >= start )
      {
        eFrame += value;
        if( eFrame < start )
        {
          eFrame = start;
        }
      }

      if( sFrame != comData->GetStartFrame() ||
          eFrame != comData->GetEndFrame() )
      {

        if( isAddAction )
        {
          gfl::xml::Node *comNode = comData->GetCommandNode();
          gfl::xml::Node *befNode = static_cast<gfl::xml::Node*>(mXmlData->clone_node( comNode ));
          mActionManager.ActionChangeCommand(comData,befNode,comData->GetParent());

        }
        comData->SetFrame( sFrame,eFrame );
      }

      comIt++;
    }
    grpIt++;
  }
  if( isAddAction )
  {
    mActionManager.EndSetAction();
  }
}

//セーブ処理
void DataEditor::SaveCommandData( gfl::xml::Node *node )
{
  gfl::xml::Node *topNode = node->FirstNode( XML_NAME_GROUP_DATA );

  DataGroupListIt grpIt = mGroupList.begin();
  while( grpIt != mGroupList.end() )
  {
    DataGroup *grpData = (*grpIt);
    grpIt++;
    grpData->SaveGroupData();
  }

  //xml再接続
  topNode->remove_all_nodes();
  grpIt = mGroupList.begin();
  while( grpIt != mGroupList.end() )
  {
    DataGroup *grpData = (*grpIt);
    grpIt++;
    topNode->append_node( grpData->GetXmlNode() );
  }


}

//ロード処理
bool DataEditor::LoadCommandData( gfl::xml::Node *node )
{
  bool ret = true;

  mTopNode = mXmlData->RootNode()->FirstNode( XML_NAME_MAIN_NODE );
  mGroupNode = mTopNode->FirstNode( XML_NAME_GROUP_DATA );

  if( !mGroupNode )
  {
    return false;
  }
  else
  {
    int maxFrame = 20;
    gfl::xml::Node *grpNode = mGroupNode->FirstNode( XML_NAME_GROUP );

    while( ret && grpNode )
    {
  		DataGroup *grpData = AddGroup();
      ret = grpData->LoadGroupData( grpNode , &maxFrame );

      grpNode = grpNode->NextSibling( XML_NAME_GROUP );
    }
    GetMainWindow()->GetFrameScaleWidget()->SetMaxScale( maxFrame+1 );
  }
  
  return true;
}

//Undoコア処理
void DataEditor::UndoFunc()
{
  bool isLoop = true;
  while(isLoop)
  {
    ACTION_DATA *act = mActionManager.GetCurrentAction();
    
    if( act )
    {
      switch( act->mType )
      {
      case ACT_SYSTEM_START:
        isLoop = false;
        break;
      case ACT_SYSTEM_END:
        break;
      //アクションコマンド
      case ACT_CHANGE_COMMAND:
        {
          UndoFunc_ChangeCommand(act);
        }
        break;
      case ACT_CREATE_COMMAND:
        {
          UndoFunc_DeleteCommand(act);
        }
        break;
      case ACT_DELETE_COMMAND:
        {
          UndoFunc_CreateCommand(act);
        }
        break;
      case ACT_CREATE_GROUP:
        {
          UndoFunc_DeleteGroup(act);
        }
        break;
      case ACT_DELETE_GROUP:
        {
          UndoFunc_CreateGroup(act);
        }
        break;
      case ACT_CHANGE_GROUP:
        {
          UndoFunc_ChangeGroup(act);
        }
        break;
      case ACT_CHANGE_COMMAND_FRAME:
        {
          UndoFunc_ChangeCommandFrame(act);
        }
        break;
      case ACT_MOVEUP_COMMAND:
        {
          DataCommand *com = (DataCommand*)act->mData;
          MoveCommandDown( com , false );
        }
        break;
      case ACT_MOVEDOWN_COMMAND:
        {
          DataCommand *com = (DataCommand*)act->mData;
          MoveCommandUp( com , false );
        }
        break;
      case ACT_MOVEUP_GROUP:
        {
          DataGroup *grp = (DataGroup*)act->mData;
          MoveGroupDown( grp , false );
        }
        break;
      case ACT_MOVEDOWN_GROUP:
        {
          DataGroup *grp = (DataGroup*)act->mData;
          MoveGroupUp( grp , false );
        }
        break;
      }

      mActionManager.MoveCurrentPrev();
    }
    else
    {
      isLoop = false;
    }
  }
}

//Redoコア処理
void DataEditor::RedoFunc()
{
  bool isLoop = true;
  while(isLoop)
  {
    ACTION_DATA *act = mActionManager.GetNextAction();
    
    if( act )
    {
      switch( act->mType )
      {
      case ACT_SYSTEM_START:
        break;
      case ACT_SYSTEM_END:
        isLoop = false;
        break;
      //アクションコマンド
      case ACT_CHANGE_COMMAND:
        {
          UndoFunc_ChangeCommand(act);
        }
        break;
      case ACT_CREATE_COMMAND:
        {
          UndoFunc_CreateCommand(act);
        }
        break;
      case ACT_DELETE_COMMAND:
        {
          UndoFunc_DeleteCommand(act);
        }
        break;
      case ACT_CREATE_GROUP:
        {
          UndoFunc_CreateGroup(act);
        }
        break;
      case ACT_DELETE_GROUP:
        {
          UndoFunc_DeleteGroup(act);
        }
        break;
      case ACT_CHANGE_GROUP:
        {
          UndoFunc_ChangeGroup(act);
        }
        break;
      case ACT_CHANGE_COMMAND_FRAME:
        {
          UndoFunc_ChangeCommandFrame(act);
        }
        break;
      case ACT_MOVEUP_COMMAND:
        {
          DataCommand *com = (DataCommand*)act->mData;
          MoveCommandUp( com , false );
        }
        break;
      case ACT_MOVEDOWN_COMMAND:
        {
          DataCommand *com = (DataCommand*)act->mData;
          MoveCommandDown( com , false );
        }
        break;
      case ACT_MOVEUP_GROUP:
        {
          DataGroup *grp = (DataGroup*)act->mData;
          MoveGroupUp( grp , false );
        }
        break;
      case ACT_MOVEDOWN_GROUP:
        {
          DataGroup *grp = (DataGroup*)act->mData;
          MoveGroupDown( grp , false );
        }
        break;
      }

      mActionManager.MoveCurrentNext();
    }
    else
    {
      isLoop = false;
    }
  }
}

//Undo・Refo用処理郡
void DataEditor::UndoFunc_CreateCommand( ACTION_DATA *act )
{
  DataCommand *com = (DataCommand*)act->mData;
  DataCommand *nextCom = (DataCommand*)act->mNextData;
  DataGroup   *grp = (DataGroup*)act->mParentData;
  gfl::xml::Node *node = (gfl::xml::Node*)act->mNodeData;
  gfl::xml::Node *nextNode = act->mNextNodeData;

  grp->AddCommandDirect(com,nextCom);
  com->SetCommandNodeApplySetting(com->GetCommandNode());

  //Undo用処理
  //act->mType = ACT_DELETE_COMMAND;
}

void DataEditor::UndoFunc_DeleteCommand( ACTION_DATA *act )
{
  DataCommand *com = (DataCommand*)act->mData;
  DataGroup   *grp = (DataGroup*)act->mParentData;
  gfl::xml::Node *node = (gfl::xml::Node*)act->mNodeData;

  grp->DeleteCommand(com,false);

  //Redo用処理
  //act->mType = ACT_DELETE_COMMAND;
}

void DataEditor::UndoFunc_ChangeCommand( ACTION_DATA *act )
{
  DataCommand *com = (DataCommand*)act->mData;
  DataGroup   *grp = (DataGroup*)act->mParentData;
  gfl::xml::Node *node = (gfl::xml::Node*)act->mNodeData;
  
  gfl::xml::Node *comNode = com->GetCommandNode();
  gfl::xml::Node *befNode = static_cast<gfl::xml::Node*>(mXmlData->clone_node( comNode ));
  
  mXmlData->clone_node( node , comNode  );
  mXmlData->clone_node( befNode , (gfl::xml::Node*)act->mNodeData  );

  com->SetCommandNodeApplySetting( comNode );
  com->GetCommandBar()->repaint();

  act->mNodeData = befNode;
}

void DataEditor::UndoFunc_CreateGroup( ACTION_DATA *act )
{
  DataGroup   *grp = (DataGroup*)act->mData;
  DataGroup   *nextGrp = (DataGroup*)act->mNextData;

  AddGroupCore(grp,nextGrp);
  if( nextGrp )
  {
    mGroupNode->insert_node( nextGrp->GetXmlNode() , act->mNodeData );
  }
  else
  {
    mGroupNode->append_node( act->mNodeData );
  }
  grp->SetXmlNode(act->mNodeData);
}

void DataEditor::UndoFunc_DeleteGroup( ACTION_DATA *act )
{
  DataGroup   *grp = (DataGroup*)act->mData;

  DeleteGroup( grp , false );
}

void DataEditor::UndoFunc_ChangeGroup( ACTION_DATA *act )
{
  DataGroup   *grp = (DataGroup*)act->mData;
  gfl::xml::Node *node = act->mNodeData;
  
  gfl::xml::Node *grpNode = grp->GetXmlNode();
  gfl::xml::Node *befNode = static_cast<gfl::xml::Node*>(mXmlData->clone_node( grpNode ));
  
  //nodeのクローンだと子のポインタまで変わってしまう・・・
  /*
  
  mXmlData->clone_node( node , grpNode  );
  mXmlData->clone_node( befNode , act->mNodeData  );
  */

  QString titleStr_o = act->mNodeData->AttributeString( XML_NAME_GROUP_ATTR_NAME , "NoNameGroup" );
  const int grpNo_o  = act->mNodeData->AttributeInteger( XML_NAME_GROUP_ATTR_NO , 0 );
  const int isOpen_o = act->mNodeData->AttributeInteger( XML_NAME_GROUP_ATTR_OPEN , 1 );
  const int isDebug_o = act->mNodeData->AttributeInteger( XML_NAME_GROUP_ATTR_DEBUG , 0 );
  QString grpOpt_o = act->mNodeData->AttributeString( XML_NAME_GROUP_ATTR_OPT , "" );
  const int r   = act->mNodeData->AttributeInteger( XML_NAME_GROUP_ATTR_BG_CLO_R , 240 );
  const int g   = act->mNodeData->AttributeInteger( XML_NAME_GROUP_ATTR_BG_CLO_G , 240 );
  const int b   = act->mNodeData->AttributeInteger( XML_NAME_GROUP_ATTR_BG_CLO_B , 240 );

  //QString titleStr_n = grpNode->AttributeString( XML_NAME_GROUP_ATTR_NAME , "NoNameGroup" );
  //const int grpNo_n  = grpNode->AttributeInteger( XML_NAME_GROUP_ATTR_NO , 0 );
  //const int isOpen_n = grpNode->AttributeInteger( XML_NAME_GROUP_ATTR_OPEN , 1 );

  grpNode->SetAttribute( XML_NAME_GROUP_ATTR_NAME , titleStr_o ); 
  grpNode->SetAttribute( XML_NAME_GROUP_ATTR_OPEN , isOpen_o ); 
  grpNode->SetAttribute( XML_NAME_GROUP_ATTR_DEBUG , isDebug_o ); 
  grpNode->SetAttribute( XML_NAME_GROUP_ATTR_NO , grpNo_o ); 
  grpNode->SetAttribute( XML_NAME_GROUP_ATTR_OPT , grpOpt_o ); 
  grpNode->SetAttribute( XML_NAME_GROUP_ATTR_BG_CLO_R , r );
  grpNode->SetAttribute( XML_NAME_GROUP_ATTR_BG_CLO_G , g );
  grpNode->SetAttribute( XML_NAME_GROUP_ATTR_BG_CLO_B , b );

  //cloneしたnodeだとparentが無いからallocateができない・・・
  //act->mNodeData->SetAttribute( XML_NAME_GROUP_ATTR_NAME , titleStr_n ); 
  //act->mNodeData->SetAttribute( XML_NAME_GROUP_ATTR_OPEN , isOpen_n ); 
  //act->mNodeData->SetAttribute( XML_NAME_GROUP_ATTR_NO , grpNo_n ); 

  grp->SetNodeApplySetting( grpNode );

  act->mNodeData = befNode;

}

void DataEditor::UndoFunc_ChangeCommandFrame( ACTION_DATA *act )
{
  DataCommand *com = (DataCommand*)act->mData;

  int befStart = com->GetStartFrame();
  int befEnd = com->GetEndFrame();
  com->SetFrame( act->mValue[0] , act->mValue[1] );
  act->mValue[0] = befStart;
  act->mValue[1] = befEnd;

}

void DataEditor::CutFunc()
{
  if( GetCurrentCommand() )
  {
    if( GetCurrentCommand()->IsLock() )
    {
      GetMainWindow()->PopLockWarnning();
      return;
    }

    CopyFunc();

    mActionManager.StartSetAction();
    mActionManager.ActionDeleteCommand(GetCurrentCommand(),GetCurrentCommand()->GetParent());
    mActionManager.EndSetAction();

    mCurCommand->GetParent()->DeleteCommand( GetCurrentCommand() , false );
    SetCurrentCommand(NULL);
  }
  else
  if(GetCurrentGroup())
  {
    if( GetCurrentGroup()->IsLock() )
    {
      GetMainWindow()->PopLockWarnning();
      return;
    }

    CopyFunc();

    mTempDeleteGroup = GetCurrentGroup();
    CheckDeleteReserve();
  }
}


void NodePrintFunc( QString *strbuf , const gfl::xml::Node* node , const char *key , char *def )
{
  *strbuf += key;
  *strbuf += ",";
  *strbuf += node->AttributeString(key,def);
  *strbuf += "\n";

}

static const char* CLIPBOARD_START_GROUP = "SeqEditorData_StartGroup";
static const char* CLIPBOARD_END_GROUP = "SeqEditorData_EndGroup";
static const char* CLIPBOARD_START_COMMAND = "SeqEditorData_StartCommand";
static const char* CLIPBOARD_END_COMMAND = "SeqEditorData_EndCommand";
static const char* CLIPBOARD_START_VALUE_SECTION = "StartValueSection";

void DataEditor::CopyFunc()
{
  if( GetCurrentCommand() )
  {
    QString strbuf;
    CopyFuncCommand(&strbuf,GetCurrentCommand());

    QApplication::clipboard()->setText( strbuf );
    gfl::core::Debug::PrintConsole("%s\n",strbuf.toLocal8Bit().constData());
  }
  else
  if( GetCurrentGroup() )
  {
    QString strbuf;
    DataGroup *grp = GetCurrentGroup();
    grp->SaveGroupData();
    gfl::xml::Node* node = grp->GetXmlNode();
    
    strbuf += CLIPBOARD_START_GROUP;
    strbuf += "\n";
    NodePrintFunc( &strbuf , node , XML_NAME_GROUP_ATTR_NAME , "NoNameGroup");
    NodePrintFunc( &strbuf , node , XML_NAME_GROUP_ATTR_NO , "0");
    NodePrintFunc( &strbuf , node , XML_NAME_GROUP_ATTR_OPEN , "1");
    NodePrintFunc( &strbuf , node , XML_NAME_GROUP_ATTR_DEBUG , "0");
    NodePrintFunc( &strbuf , node , XML_NAME_GROUP_ATTR_OPT , "");
    NodePrintFunc( &strbuf , node , XML_NAME_GROUP_ATTR_BG_CLO_R , "240");
    NodePrintFunc( &strbuf , node , XML_NAME_GROUP_ATTR_BG_CLO_G , "240");
    NodePrintFunc( &strbuf , node , XML_NAME_GROUP_ATTR_BG_CLO_B , "240");

    int comNum = grp->GetCommandNum();
    for( int i=0;i<comNum;i++ )
    {
      CopyFuncCommand( &strbuf,grp->GetCommandByIndex(i) );
    }

    strbuf += CLIPBOARD_END_GROUP;
    strbuf += "\n";

    QApplication::clipboard()->setText( strbuf );
    //gfl::core::Debug::PrintConsole("%s\n",strbuf.toLocal8Bit().constData());
  }
}

void DataEditor::CopyFuncCommand(QString *strbuf,DataCommand* com)
{
  gfl::xml::Node* node = com->GetCommandNode();
  
  *strbuf += CLIPBOARD_START_COMMAND;
  *strbuf += "\n";
  NodePrintFunc( strbuf , node , XML_NAME_COMMAND_ATTR_SFRAME , "0");
  NodePrintFunc( strbuf , node , XML_NAME_COMMAND_ATTR_EFRAME , "0");
  NodePrintFunc( strbuf , node , XML_NAME_COMMAND_ATTR_COMMENT , "");
  NodePrintFunc( strbuf , node , XML_NAME_COMMAND_ATTR_COL_R , "0");
  NodePrintFunc( strbuf , node , XML_NAME_COMMAND_ATTR_COL_G , "0");
  NodePrintFunc( strbuf , node , XML_NAME_COMMAND_ATTR_COL_B , "0");
  NodePrintFunc( strbuf , node , XML_NAME_COMMAND_ATTR_IS_ACITVE , "1");
  
  *strbuf += CLIPBOARD_START_VALUE_SECTION;
  *strbuf += "\n";

  gfl::xml::Node* comNode = node->FirstNode();
  *strbuf += comNode->name();
  *strbuf += "\n";

  gfl::xml::Node* dataNode = comNode->FirstNode();
  while( dataNode )
  {
    *strbuf += dataNode->name();
    
    char valueName[10] = "value0\0";
    bool isLoop = true; 
    while( isLoop )
    {
      const char *val = dataNode->AttributeString(valueName,NULL);
      if( val )
      {
        *strbuf += ",";
        *strbuf += val;
      }
      else
      {
        isLoop = false;
      }
      valueName[5] += 1;
    }
    *strbuf += "\n";
    dataNode = dataNode->NextSibling();
  }
  *strbuf += CLIPBOARD_END_COMMAND;
  *strbuf += "\n";
  
}

void DataEditor::PasteFunc()
{
  QString strbuf = QApplication::clipboard()->text();
  //gfl::core::Debug::PrintConsole("%s\n",strbuf.toLocal8Bit().constData());
  
  QStringList strList = strbuf.split("\n");
  if( strList.value(0) == CLIPBOARD_START_COMMAND )
  {
    if( GetCurrentGroup() )
    {
      if( GetCurrentGroup()->IsLock() )
      {
        GetMainWindow()->PopLockWarnning();
        return;
      }
      PasteFunc_Command( GetCurrentGroup() , &strList,0 , true );
    }
  }
  else
  if( strList.value(0) == CLIPBOARD_START_GROUP )
  {
    DataGroup *grp;
    if( GetCurrentGroup() )
    {
      grp = InsertGroup(GetCurrentGroup());
    }
    else
    {
      grp = AddGroup();
    }
    gfl::xml::Node *node = mGroupNode->AddNode( XML_NAME_GROUP );
    grp->SetXmlNode( node );

    int step = 0;
    bool isLoop = true;
    int i = 1;
    QColor col;
    col.setRgb(240,240,240);
    while(isLoop)
    {
      gfl::core::Debug::PrintConsole("[%d]%s\n",i,strList.value(i).toLocal8Bit().constData());
      if( step == 0 )
      {
        QStringList strList2 = strList.value(i).split(",");
        if( strList2.value(0) == XML_NAME_GROUP_ATTR_NAME )
        {
          node->SetAttribute( XML_NAME_GROUP_ATTR_NAME , strList2.value(1) );
          i++;
        }
        else
        if( strList2.value(0) == XML_NAME_GROUP_ATTR_NO )
        {
          node->SetAttribute( XML_NAME_GROUP_ATTR_NO , strList2.value(1) );
          i++;
        }
        else
        if( strList2.value(0) == XML_NAME_GROUP_ATTR_OPEN )
        {
          node->SetAttribute( XML_NAME_GROUP_ATTR_OPEN , strList2.value(1) );
          i++;
        }
        else
        if( strList2.value(0) == XML_NAME_GROUP_ATTR_DEBUG )
        {
          node->SetAttribute( XML_NAME_GROUP_ATTR_DEBUG , strList2.value(1) );
          i++;
        }
        else
        if( strList2.value(0) == XML_NAME_GROUP_ATTR_OPT )
        {
          node->SetAttribute( XML_NAME_GROUP_ATTR_OPT , strList2.value(1) );
          i++;
        }
        else
        if( strList2.value(0) == XML_NAME_GROUP_ATTR_BG_CLO_R )
        {
          col.setRed(strList2.value(1).toInt());
          node->SetAttribute( XML_NAME_GROUP_ATTR_BG_CLO_R , strList2.value(1) );
          i++;
        }
        else
        if( strList2.value(0) == XML_NAME_GROUP_ATTR_BG_CLO_G )
        {
          col.setGreen(strList2.value(1).toInt());
          node->SetAttribute( XML_NAME_GROUP_ATTR_BG_CLO_G , strList2.value(1) );
          i++;
        }
        else
        if( strList2.value(0) == XML_NAME_GROUP_ATTR_BG_CLO_B )
        {
          col.setBlue(strList2.value(1).toInt());
          node->SetAttribute( XML_NAME_GROUP_ATTR_BG_CLO_B , strList2.value(1) );
          i++;
        }
        else
        if( strList2.value(0) == CLIPBOARD_START_COMMAND )
        {
          grp->SetBgColor(col);
          step = 1;
        }
        else
        if( strList2.value(0) == CLIPBOARD_END_GROUP )
        {
          isLoop = false;
        }
        else
        {
          gfl::core::Debug::PrintConsole("UndefineState[%s]\n",strList2.value(0));
          i++;
        }
      }
      else
      if( step == 1 )
      {
        int ret = PasteFunc_Command( grp , &strList , i , false );
        if( ret != -1 )
        {
          i = ret;
          step = 0;
        }
        else
        {
          //失敗
        }
      }
    }//while
    //設定反映のためセットし直し。
    grp->SetNodeApplySetting(node);

    mActionManager.StartSetAction();
    mActionManager.ActionCreateGroup(grp,GetNextGroup(grp));
    mActionManager.EndSetAction();
  }
}

int DataEditor::PasteFunc_Command( DataGroup *grp , QStringList *strList , int startIdx , bool addAction )
{
  //解析開始
  if( strList->value(startIdx) == CLIPBOARD_START_COMMAND )
  {
    gfl::xml::Node* node = grp->AddCommandNode();
    gfl::xml::Node* comNode = NULL;

    int i = startIdx+1;
    int step = 0;
    
    QString comName;
    bool isLoop = true;
    
    while( isLoop )
    {
      QStringList strList2 = strList->value(i).split(",");
      i++;

      if( step == 0 )
      {
        if( strList2.value(0) == XML_NAME_COMMAND_ATTR_SFRAME )
        {
          node->SetAttribute( XML_NAME_COMMAND_ATTR_SFRAME , strList2.value(1) );
        }
        else
        if( strList2.value(0) == XML_NAME_COMMAND_ATTR_EFRAME )
        {
          node->SetAttribute( XML_NAME_COMMAND_ATTR_EFRAME , strList2.value(1) );
        }
        else
        if( strList2.value(0) == XML_NAME_COMMAND_ATTR_COMMENT )
        {
          node->SetAttribute( XML_NAME_COMMAND_ATTR_COMMENT , strList2.value(1) );
        }
        else
        if( strList2.value(0) == XML_NAME_COMMAND_ATTR_COL_R )
        {
          node->SetAttribute( XML_NAME_COMMAND_ATTR_COL_R , strList2.value(1) );
        }
        else
        if( strList2.value(0) == XML_NAME_COMMAND_ATTR_COL_G )
        {
          node->SetAttribute( XML_NAME_COMMAND_ATTR_COL_G , strList2.value(1) );
        }
        else
        if( strList2.value(0) == XML_NAME_COMMAND_ATTR_COL_B )
        {
          node->SetAttribute( XML_NAME_COMMAND_ATTR_COL_B , strList2.value(1) );
        }
        else
        if( strList2.value(0) == XML_NAME_COMMAND_ATTR_IS_ACITVE )
        {
          node->SetAttribute( XML_NAME_COMMAND_ATTR_IS_ACITVE , strList2.value(1) );
        }
        else
        if( strList2.value(0) == CLIPBOARD_START_VALUE_SECTION )
        {
          step = 1;
        }
        else
        {
          gfl::core::Debug::PrintConsole("UndefineState[%s]\n",strList2.value(0));
        }
      }
      else if(step == 1)
      {
        comName = strList2.value(0);
        comNode = node->AddNode( comName );
        step = 2;
      }
      else if(step == 2)
      {
        if( strList2.value(0) == CLIPBOARD_END_COMMAND )
        {
          if( comNode != NULL &&
              comName != "" )
          {
            gfl::xml::Node *macroNode = mMacroData.GetMacroNode( comName );

            QString type = macroNode->AttributeString( "type" , "normal" );
            DataCommand *com = grp->AddCommand( type );
            com->LoadCommandData( node );
            
            if( addAction )
            {
              mActionManager.StartSetAction();
              mActionManager.ActionCreateCommand(com,grp);
              mActionManager.EndSetAction();
            }

	          mMainWin->GetTitleAreaWidget()->update();

          }
          else
          {
            GetCurrentGroup()->RemoveCommandNode( node );
          }
          isLoop = false;
          return i;
        }
        else
        {
          gfl::xml::Node* valNode = comNode->AddNode( strList2.value(0) );
          for( int j=1; j<strList2.size() ; j++ )
          {
            char valueName[10] = "value0\0";
            valueName[5] += j-1;
            valNode->SetAttribute( valueName , strList2.value(j) );
          }
        }
      }
    }
  }
  return -1;
}
void DataEditor::CreateAction(void)
{
	//グループを開く
	mActOpenGroup = new QAction( QIcon( ":grp_open.png" ), tr("グループ開く(&O)"), this );
	mActOpenGroup->setStatusTip("グループを開きます。");
	connect( mActOpenGroup , SIGNAL( triggered() ) ,
					 this , SLOT( ActOpenGroup() ) );

	//グループを閉じる
	mActCloseGroup = new QAction( QIcon( ":grp_close.png" ), tr("グループ閉じる(&C)"), this );
	mActCloseGroup->setStatusTip("グループを閉じます。");
	connect( mActCloseGroup , SIGNAL( triggered() ) ,
					 this , SLOT( ActCloseGroup() ) );

  //グループを全部開く
	mActOpenAllGroup = new QAction( QIcon( ":grp_open_all.png" ), tr("全グループ開く"), this );
	mActOpenAllGroup->setStatusTip("全部のグループを開きます。");
	connect( mActOpenAllGroup , SIGNAL( triggered() ) ,
					 this , SLOT( ActOpenAllGroup() ) );

	//グループを全部閉じる
	mActCloseAllGroup = new QAction( QIcon( ":grp_close_all.png" ), tr("全グループ閉じる"), this );
	mActCloseAllGroup->setStatusTip("全部のグループを閉じます。");
	connect( mActCloseAllGroup , SIGNAL( triggered() ) ,
					 this , SLOT( ActCloseAllGroup() ) );

	//グループ開閉
	mActOpenCloseGroup = new QAction( QIcon( ":grp_open_close.png" ), tr("グループ開閉じる"), this );
	mActOpenCloseGroup->setStatusTip("グループを開閉します。");
	mActOpenCloseGroup->setShortcut( tr("Shift+C") );
	connect( mActOpenCloseGroup , SIGNAL( triggered() ) ,
					 this , SLOT( ActOpenCloseGroup() ) );

  

	//グループを追加する
	mActAddGroup = new QAction( QIcon( ":grp_add.png" ), tr("グループ追加(&G)"), this );
	mActAddGroup->setStatusTip("グループを追加します。");
	connect( mActAddGroup , SIGNAL( triggered() ) ,
					 this , SLOT( ActAddGroup() ) );

	//グループを削除する
	mActDeleteGroup = new QAction( QIcon( ":grp_del.png" ), tr("グループ削除(&L)"), this );
	mActDeleteGroup->setStatusTip("グループを削除します。");
	connect( mActDeleteGroup , SIGNAL( triggered() ) ,
					 this , SLOT( ActDeleteGroup() ) );

  //グループを編集する
	mActEditGroup = new QAction( QIcon( ":grp_edit.png" ), tr("グループ編集(&I)"), this );
	mActEditGroup->setStatusTip("グループを編集します。");
	connect( mActEditGroup , SIGNAL( triggered() ) ,
					 this , SLOT( ActEditGroup() ) );

	//コマンドの編集
	mActEditCommand = new QAction( QIcon( ":com_edit.png" ), tr("コマンド編集(&E)"), this );
	mActEditCommand->setStatusTip("コマンドを編集します。");
	connect( mActEditCommand , SIGNAL( triggered() ) ,
					 this , SLOT( ActEditCommand() ) );

  //コマンドの削除
	mActDeleteCommand = new QAction( QIcon( ":com_del.png" ), tr("コマンド削除(&D)"), this );
	mActDeleteCommand->setStatusTip("コマンドを削除します。");
	connect( mActDeleteCommand , SIGNAL( triggered() ) ,
					 this , SLOT( ActDeleteCommand() ) );

  //グループ移動
	mActMoveGroupUp = new QAction( QIcon( ":grp_up.png" ), tr("グループ上移動"), this );
	mActMoveGroupUp->setStatusTip("グループを上に移動させます。");
	connect( mActMoveGroupUp , SIGNAL( triggered() ) ,
					 this , SLOT( ActMoveGroupUp() ) );
	mActMoveGroupDown = new QAction( QIcon( ":grp_down.png" ), tr("グループ下移動"), this );
	mActMoveGroupDown->setStatusTip("グループを下に移動させます。");
	connect( mActMoveGroupDown , SIGNAL( triggered() ) ,
					 this , SLOT( ActMoveGroupDown() ) );

  //コマンド移動
	mActMoveCommandUp = new QAction( QIcon( ":com_up.png" ), tr("コマンド上移動"), this );
	mActMoveCommandUp->setStatusTip("コマンドを上に移動させます。");
	connect( mActMoveCommandUp , SIGNAL( triggered() ) ,
					 this , SLOT( ActMoveCommandUp() ) );
	mActMoveCommandDown = new QAction( QIcon( ":com_down.png" ), tr("コマンド下移動"), this );
	mActMoveCommandDown->setStatusTip("コマンドを下に移動させます。");
	connect( mActMoveCommandDown , SIGNAL( triggered() ) ,
					 this , SLOT( ActMoveCommandDown() ) );

  //フレーム操作
	mActFrameAddAll = new QAction( QIcon( ":frame_add.png" ), tr("フレーム追加"), this );
	mActFrameAddAll->setStatusTip("現フレーム位置にフレームを追加します。");
	connect( mActFrameAddAll , SIGNAL( triggered() ) ,
					 this , SLOT( ActFrameAddAll() ) );
	mActFrameDelAll = new QAction( QIcon( ":frame_del.png" ), tr("フレーム削除"), this );
	mActFrameDelAll->setStatusTip("現フレーム位置からフレームを削除します。");
	connect( mActFrameDelAll , SIGNAL( triggered() ) ,
					 this , SLOT( ActFrameDelAll() ) );

  //ロック関係
  mActLockOn = new QAction( QIcon( ":lock_on.png" ), tr("ロック設定"), this );
  mActLockOn->setStatusTip("選択グループにロックを設定します。");
  connect( mActLockOn , SIGNAL( triggered() ) , this , SLOT( ActLockOn() ) );

  mActLockOff = new QAction( QIcon( ":lock_off.png" ), tr("ロック解除"), this );
  mActLockOff->setStatusTip("選択グループのロックを解除します。");
  connect( mActLockOff , SIGNAL( triggered() ) , this , SLOT( ActLockOff() ) );

  mActLockOnAll = new QAction( QIcon( ":lock_on_all.png" ), tr("全ロック設定"), this );
  mActLockOnAll->setStatusTip("すべてのグループにロックを設定します。");
  connect( mActLockOnAll , SIGNAL( triggered() ) , this , SLOT( ActLockOnAll() ) );

  mActLockOffAll = new QAction( QIcon( ":lock_off_all.png" ), tr("全ロック解除"), this );
  mActLockOffAll->setStatusTip("すべてのグループのロックを解除します。");
  connect( mActLockOffAll , SIGNAL( triggered() ) , this , SLOT( ActLockOffAll() ) );
}

void DataEditor::ActOpenGroup()
{
	if( GetTargetGroup() != NULL )
	{
		GetTargetGroup()->OpenGroup();
    //グループの開閉ステートが変わるため
    UpdateTargetGroup();
	}
}

void DataEditor::ActCloseGroup()
{
	if( GetTargetGroup() != NULL )
	{
		GetTargetGroup()->CloseGroup();
    //グループの開閉ステートが変わるため
    UpdateTargetGroup();
	}
}

void DataEditor::ActOpenAllGroup()
{
  DataGroupListIt grpIt = mGroupList.begin();
  while( grpIt != mGroupList.end() )
  {
    DataGroup *grpData = (*grpIt);
    grpData->OpenGroup();
    grpIt++;
  }
  //グループの開閉ステートが変わるため
  UpdateTargetGroup();
}

void DataEditor::ActCloseAllGroup()
{
  DataGroupListIt grpIt = mGroupList.begin();
  while( grpIt != mGroupList.end() )
  {
    DataGroup *grpData = (*grpIt);
    grpData->CloseGroup();
    grpIt++;
  }
  //グループの開閉ステートが変わるため
  UpdateTargetGroup();
}

//グループ開閉
void DataEditor::ActOpenCloseGroup()
{
	if( GetTargetGroup() != NULL )
	{
    if( GetTargetGroup()->IsOpen() )
    {
		  GetTargetGroup()->CloseGroup();
    }
    else
    {
		  GetTargetGroup()->OpenGroup();
    }
    //グループの開閉ステートが変わるため
    UpdateTargetGroup();
	}
}

void DataEditor::ActAddGroup()
{
	DialogAddGroup dialog(mMainWin,NULL);
  dialog.SetGroupOption( mMacroData.GetGroupOptNode() );

	int ret = dialog.exec();
	if( ret == QDialog::Accepted )
	{
    DataGroup *group;
	  if( GetTargetGroup() != NULL )
	  {
		  group = InsertGroup(GetTargetGroup());
	  }
    else
    {
		  group = AddGroup();
    }
    //xml
    gfl::xml::Node *node = mGroupNode->AddNode( XML_NAME_GROUP );
    group->SetXmlNode( node );

    group->SetTitleStr( dialog.lineEdit->text() );
    group->SetGroupNo( dialog.grpNoVal->value() );
    group->SetBgColor(dialog.bgColor);
    group->SetIsDebug(dialog.debugCheck->isChecked());
    group->SetIsLock(false);
    
    if( mMacroData.GetGroupOptNode() )
    {
      group->SetOptionStr( dialog.grpOption->itemData(dialog.grpOption->currentIndex()).toString() );
    }
    else
    {
      group->SetOptionStr( "" );
    }

    mActionManager.StartSetAction();
    mActionManager.ActionCreateGroup(group,GetNextGroup(group));
    mActionManager.EndSetAction();

  }
}

void DataEditor::ActDeleteGroup()
{
  if( GetTargetGroup() != NULL )
  {
    if( GetTargetGroup()->IsLock() )
    {
      GetMainWindow()->PopLockWarnning();
      return;
    }
    //ここでdeleteすると呼び出し元自体を削除することがあるため、
    //ポインタをとっておき、後ほど消す。
    //フラグ制御は右クリックメニューの時だけ行っている
    mTempDeleteGroup = GetTargetGroup();
    CheckDeleteReserve();
  }
}

void DataEditor::ActEditGroup()
{
  DataGroup *grp = GetTargetGroup();
	if( grp != NULL )
	{
    if( grp->IsLock() )
    {
      GetMainWindow()->PopLockWarnning();
      return;
    }

    gfl::xml::Node *befNode = static_cast<gfl::xml::Node*>(mXmlData->clone_node( grp->GetXmlNode() ));
    
    DialogAddGroup dialog(mMainWin,grp);
    dialog.lineEdit->setText( grp->GetTitleStr() );
    dialog.grpNoVal->setValue( grp->GetGroupNo() );
    dialog.SetGroupOption( mMacroData.GetGroupOptNode(), &grp->GetOptionStr() );
    dialog.bgColor = grp->GetBgColor();
    dialog.debugCheck->setChecked(grp->IsDebug());

    int ret = dialog.exec();
	  if( ret == QDialog::Accepted )
	  {
  	  grp->SetTitleStr( dialog.lineEdit->text() );
      grp->SetGroupNo( dialog.grpNoVal->value() );
      grp->SetBgColor(dialog.bgColor);
      grp->SetIsDebug(dialog.debugCheck->isChecked());

      if( mMacroData.GetGroupOptNode() )
      {
        grp->SetOptionStr( dialog.grpOption->itemData(dialog.grpOption->currentIndex()).toString() );
      }
      else
      {
        grp->SetOptionStr( "" );
      }
      grp->SaveGroupData();

      mActionManager.StartSetAction();
      mActionManager.ActionChangeGroup(grp,befNode);
      mActionManager.EndSetAction();

    }
  }
}

void DataEditor::ActEditCommand()
{
  DataCommand *com = GetTargetCommand();
  if( com != NULL )
  {
    if( com->IsLock() )
    {
      GetMainWindow()->PopLockWarnning();
      return;
    }
    gfl::xml::Node *comNode = com->GetCommandNode();
    gfl::xml::Node *macroNode = mMacroData.GetMacroNode( comNode->FirstNode()->name() );
    comNode->SetAttribute( XML_NAME_COMMAND_ATTR_SFRAME , com->GetStartFrame() );
    comNode->SetAttribute( XML_NAME_COMMAND_ATTR_EFRAME , com->GetEndFrame() );

    gfl::xml::Node *befNode = static_cast<gfl::xml::Node*>(mXmlData->clone_node( comNode ));

    MacroDialog dialog( GetMainWindow() , macroNode , comNode , mMacroData.GetDefineNode() , mMacroData.GetPathDefNode() ); 

		int ret = dialog.exec();
		if( ret == QDialog::Accepted )
		{
      com->SetCommandNodeApplySetting( comNode );
      com->SetFrame( comNode->AttributeInteger(XML_NAME_COMMAND_ATTR_SFRAME) , comNode->AttributeInteger(XML_NAME_COMMAND_ATTR_EFRAME) );
      com->SetCommentStr( comNode->AttributeString(XML_NAME_COMMAND_ATTR_COMMENT) );
      com->GetCommandBar()->repaint();
      
		  mMainWin->GetTitleAreaWidget()->update();

      mActionManager.StartSetAction();
      mActionManager.ActionChangeCommand(com,befNode,com->GetParent());
      mActionManager.EndSetAction();
    }
  }
}

void DataEditor::ActDeleteCommand()
{
  if( GetTargetCommand() != NULL )
  {
    if( GetTargetCommand()->IsLock() )
    {
      GetMainWindow()->PopLockWarnning();
      return;
    }
    //ここでdeleteすると呼び出し元自体を削除することがあるため、
    //ポインタをとっておき、後ほど消す。
    mTempDeleteCommand = GetTargetCommand();
  }
}

void DataEditor::ActMoveGroupUp()
{
  DataGroup *grp = GetCurrentGroup();
  if( grp )
  {
    MoveGroupUp( grp );
    SetCurrentGroup( grp );
  }
}

void DataEditor::ActMoveGroupDown()
{
  DataGroup *grp = GetCurrentGroup();
  if( grp )
  {
    MoveGroupDown( grp );
    SetCurrentGroup( grp );
  }
}

void DataEditor::ActMoveCommandUp()
{
  DataCommand *com = GetCurrentCommand();
  if( com )
  {
    MoveCommandUp( com );
    SetCurrentCommand( com );
  }
}

void DataEditor::ActMoveCommandDown()
{
  DataCommand *com = GetCurrentCommand();
  if( com )
  {
    MoveCommandDown( com );
    SetCurrentCommand( com );
  }
}

void DataEditor::ActFrameAddAll()
{
  DialogMoveFrame dialog( GetMainWindow() ); 

	int ret = dialog.exec();
	if( ret == QDialog::Accepted )
	{
    /*
    com->SetCommandNode( comNode );
    com->SetFrame( comNode->AttributeInteger(XML_NAME_COMMAND_ATTR_SFRAME) , comNode->AttributeInteger(XML_NAME_COMMAND_ATTR_EFRAME) );
    com->SetCommentStr( comNode->AttributeString(XML_NAME_COMMAND_ATTR_COMMENT) );
    
	  mMainWin->GetTitleAreaWidget()->update();

    mActionManager.StartSetAction();
    mActionManager.ActionChangeCommand(com,befNode,com->GetParent());
    mActionManager.EndSetAction();
    */
    FrameAddAll(mNowFrame,dialog.valueBox->value());
    
  }

}

void DataEditor::ActFrameDelAll()
{
  DialogMoveFrame dialog( GetMainWindow() ); 

	int ret = dialog.exec();
	if( ret == QDialog::Accepted )
	{
    /*
    com->SetCommandNode( comNode );
    com->SetFrame( comNode->AttributeInteger(XML_NAME_COMMAND_ATTR_SFRAME) , comNode->AttributeInteger(XML_NAME_COMMAND_ATTR_EFRAME) );
    com->SetCommentStr( comNode->AttributeString(XML_NAME_COMMAND_ATTR_COMMENT) );
    
	  mMainWin->GetTitleAreaWidget()->update();

    mActionManager.StartSetAction();
    mActionManager.ActionChangeCommand(com,befNode,com->GetParent());
    mActionManager.EndSetAction();
    */

    FrameAddAll(mNowFrame,-dialog.valueBox->value());
  }
}

void DataEditor::ActLockOn()
{
  DataGroup *grp = GetCurrentGroup();
  if( grp )
  {
    grp->SetIsLock(true);
    UpdateDraw();
  }
}
void DataEditor::ActLockOff()
{
  DataGroup *grp = GetCurrentGroup();
  if( grp )
  {
    grp->SetIsLock(false);
    UpdateDraw();
  }
}
void DataEditor::ActLockOnAll()
{
  DataGroupListIt grpIt = mGroupList.begin();
  while( grpIt != mGroupList.end() )
  {
    DataGroup *grpData = (*grpIt);
    grpData->SetIsLock(true);
    grpIt++;
  }
  UpdateDraw();
}
void DataEditor::ActLockOffAll()
{
  DataGroupListIt grpIt = mGroupList.begin();
  while( grpIt != mGroupList.end() )
  {
    DataGroup *grpData = (*grpIt);
    grpData->SetIsLock(false);
    grpIt++;
  }
  UpdateDraw();
}

void DataEditor::ActLockFlipCurrent()
{
  DataGroup *grp = GetCurrentGroup();
  if( grp )
  {
    grp->SetIsLock(!grp->IsLock());
    UpdateDraw();
  }

}

//======================================================================
/**
 * @file	dataGroup.cpp
 * @brief	
 * @author	ariizumi
 * @data	2011/03/07
 */
//======================================================================

#include "dataEditor.h"
#include "dataGroup.h"
#include "widgetTitleName.h"
#include "save_xml_define.h"

DataGroup::DataGroup(DataEditor *dataEditor)
:mDataEditor(dataEditor)
,mNode(NULL)
{
	mIsOpen = true;
  mIsDebug = false;
	mTitleStr = "NoName";
  mGroupNo = 0;
  mBgColor.setRgb(240,240,240);
  mIsLock = SeqEditor::GetIsDefaultLock();
}

DataGroup::~DataGroup()
{
}

DataCommand* DataGroup::AddCommand( QString &type )
{
  DataCommand *com = new DataCommand(mDataEditor);

  COM_BAR_TYPE barType = BAR_TYPE_NORMAL;
  if( type == "one_frame" )
  {
    barType = BAR_TYPE_ONE_FRAME;
  }

  com->SetBarType(barType);
	mComList.push_back(com);
	mGroup->AddCommand(com);
	mTitle->AddCommand(com);
	
	com->SetParent(this);
  return com;
}

void DataGroup::AddCommandDirect( DataCommand *com , DataCommand *nextCom ) //Undo・Redo用
{

  if( nextCom )
  {
    mNode->insert_node( nextCom->GetCommandNode() , com->GetCommandNode() );

    DataComListIt it = mComList.begin();
    while( it != mComList.end() )
    {
      DataCommand *tempCom = (*it);
      if( tempCom == nextCom )
      {
        break;
      }
      it++;
    }
    
    mComList.insert(it,com);
  }
  else
  {
    mNode->append_node( com->GetCommandNode() );
  	mComList.push_back(com);
  }
	mGroup->AddCommand(com,nextCom);
	mTitle->AddCommand(com);
	
	com->SetParent(this);
}

void DataGroup::DeleteCommand( DataCommand *com , bool isDelete )
{
  if( mDataEditor->GetCurrentCommand() == com )
  {
    mDataEditor->SetCurrentCommand( NULL );
  }
  mNode->remove_node( com->GetCommandNode() );
  mGroup->RemoveCommand(com);
	mTitle->RemoveCommand(com);
  mComList.remove( com );
  if( isDelete )
  {
    //Undo用
    delete com;
  }
}

DataCommand* DataGroup::GetCommandByIndex( int idx )
{
  DataComListIt it = mComList.begin();
  for( int i=0;i<idx;i++ )
  {
    it++;
  }
  return (*it);
}

DataCommand* DataGroup::GetNextCommand( DataCommand* com )
{
  DataComListIt it = mComList.begin();
  while( it != mComList.end() )
  {
    DataCommand *com2 = (*it);
    it++;
    if( com == com2 &&
        it != mComList.end() )
    {
      return (DataCommand*)(*it);
    }
  }

  return NULL;
}

DataCommand* DataGroup::GetBeforeCommand( DataCommand* com )
{
  DataComListIt it = mComList.begin();
  DataComListIt befIt = mComList.begin();
  while( it != mComList.end() )
  {
    DataCommand *com2 = (*it);
    if( com == com2 &&
        it != mComList.begin() )
    {
      return (DataCommand*)(*befIt);
    }
    befIt = it;
    it++;
  }

  return NULL;
}

void DataGroup::SetTitleStr( const QString &str )
{ 
  mTitleStr = str; 
  mNode->SetAttribute( XML_NAME_GROUP_ATTR_NAME , mTitleStr ); 
}
void DataGroup::SetGroupNo( const int no )
{
  mGroupNo = no; 
  mNode->SetAttribute( XML_NAME_GROUP_ATTR_NO , mGroupNo ); 
}


void DataGroup::OpenGroup()
{
	mIsOpen = true;

	mGroup->SetIsOpen(mIsOpen);
	ChangedOpenGroup( mIsOpen );
  mNode->SetAttribute( XML_NAME_GROUP_ATTR_OPEN , mIsOpen ); 
}

void DataGroup::CloseGroup()
{
	mIsOpen = false;

	mGroup->SetIsOpen(mIsOpen);
	ChangedOpenGroup( mIsOpen );
  mNode->SetAttribute( XML_NAME_GROUP_ATTR_OPEN , mIsOpen ); 
}


gfl::xml::Node* DataGroup::AddCommandNode()
{
  return mNode->AddNode( XML_NAME_COMMAND );
}

void DataGroup::RemoveCommandNode(gfl::xml::Node* node )
{
  mNode->remove_node( node );
}

void DataGroup::SaveGroupData()
{
  //項目増えたらUndoRedoも対応！
  mNode->SetAttribute( XML_NAME_GROUP_ATTR_NAME , mTitleStr ); 
  mNode->SetAttribute( XML_NAME_GROUP_ATTR_OPEN , mIsOpen ); 
  mNode->SetAttribute( XML_NAME_GROUP_ATTR_DEBUG , mIsDebug ); 
  mNode->SetAttribute( XML_NAME_GROUP_ATTR_NO , mGroupNo ); 
  mNode->SetAttribute( XML_NAME_GROUP_ATTR_OPT , mOptStr );

  int r,g,b;
  mBgColor.getRgb(&r,&g,&b);

  mNode->SetAttribute( XML_NAME_GROUP_ATTR_BG_CLO_R , r );
  mNode->SetAttribute( XML_NAME_GROUP_ATTR_BG_CLO_G , g );
  mNode->SetAttribute( XML_NAME_GROUP_ATTR_BG_CLO_B , b );

  DataComListIt it = mComList.begin();
  while( it != mComList.end() )
  {
    DataCommand *com = (*it);
    it++;
    com->SaveCommandData();
  }
}

bool DataGroup::LoadGroupData( gfl::xml::Node *node , int *maxFrame )
{
  SetNodeApplySetting( node );
  bool ret = true;
  gfl::xml::Node *comNode = node->FirstNode( XML_NAME_COMMAND );
  
  while( ret && comNode )
  {
    gfl::xml::Node *macroNode = mDataEditor->GetMacroData()->GetMacroNode( comNode->FirstNode()->name() );
    QString type = macroNode->AttributeString( "type" , "normal" );

		DataCommand *com = AddCommand( type );
    ret = com->LoadCommandData( comNode );

    if( *maxFrame < com->GetEndFrame() )
    {
      *maxFrame = com->GetEndFrame();
    }

    comNode = comNode->NextSibling( XML_NAME_COMMAND );
  }
  //グループの開閉を見るために、コマンド追加後にもう一回。
  SetNodeApplySetting( node );
  return ret;
}

void DataGroup::SetNodeApplySetting(gfl::xml::Node* node )
{
  //項目増えたらUndoRedoも対応！
  mNode = node;

  QString titleStr;
  titleStr = node->AttributeString( XML_NAME_GROUP_ATTR_NAME , "NoNameGroup" );
  SetTitleStr( titleStr );
  mOptStr = node->AttributeString( XML_NAME_GROUP_ATTR_OPT , "" );

  mGroupNo = node->AttributeInteger( XML_NAME_GROUP_ATTR_NO , 0 );

  const int isOpen = node->AttributeInteger( XML_NAME_GROUP_ATTR_OPEN , 1 );
  if( isOpen )
  {
    OpenGroup();
  }
  else
  {
    CloseGroup();
  }
  mIsDebug = node->AttributeInteger( XML_NAME_GROUP_ATTR_DEBUG , 0 );

  const int r   = node->AttributeInteger( XML_NAME_GROUP_ATTR_BG_CLO_R , 240 );
  const int g   = node->AttributeInteger( XML_NAME_GROUP_ATTR_BG_CLO_G , 240 );
  const int b   = node->AttributeInteger( XML_NAME_GROUP_ATTR_BG_CLO_B , 240 );
  mBgColor.setRgb(r,g,b);

}

void DataGroup::Dump()
{
  DataComListIt it = mComList.begin();
  gfl::core::Debug::PrintConsole("---------Dump group start---------\n");
  while( it != mComList.end() )
  {
    DataCommand *com = (*it);
    gfl::core::Debug::PrintConsole("[%x]\n",com);
    it++;
  }
  gfl::core::Debug::PrintConsole("----------Dump group end----------\n");

}

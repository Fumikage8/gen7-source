//======================================================================
/**
 * @file	dataCommand.cpp
 * @brief	
 * @author	ariizumi
 * @data	2011/03/07
 */
//======================================================================

#include "dataEditor.h"
#include "dataCommand.h"
#include "widgetSeqBar.h"
#include "save_xml_define.h"

//----------------------------------------------------
DataCommand::DataCommand(DataEditor *dataEditor)
	:mStartFrame(0)
	,mEndFrame(0)
	,mBar(NULL)
  ,mDataEditor(dataEditor)
	,mParent(NULL)
  ,mComNode(NULL)
{
  mIsActive = true;
  mChipCol = Qt::red;
	mTitleStr = "ƒRƒ}ƒ“ƒh‚P" ;
  mDataVec.reserve(SEQEDIT_DATA_MAX);
}

DataCommand::~DataCommand()
{
  for( unsigned int i=0 ; i<mDataVec.size() ; i++ )
  {
    delete mDataVec.at(i);
  }
  mDataVec.clear();
}

void DataCommand::SetFrame( int start , int end )
{
//	if( mStartFrame != start ||
//			mEndFrame != end )
	{
		mStartFrame = start;
		mEndFrame = end;

    if( mStartFrame < SEQEDIT_MIN_FRAME_VAL )
    {
      mStartFrame = SEQEDIT_MIN_FRAME_VAL;
    }
    if( mEndFrame > SEQEDIT_MAX_FRAME_VAL )
    {
      mEndFrame = SEQEDIT_MAX_FRAME_VAL;
    }

		if( mBar != NULL )
		{
			mBar->SetFrame( mStartFrame , mEndFrame );
		}
    if( mComNode )
    {
      mComNode->SetAttribute( XML_NAME_COMMAND_ATTR_SFRAME , mStartFrame );
      mComNode->SetAttribute( XML_NAME_COMMAND_ATTR_EFRAME , mEndFrame );
    }
	}
}

DataValue* DataCommand::AddDataValue( SEQEDIT_DATA_TYPE type )
{
  DataValue *data = new DataValue(type);
  mDataVec.push_back(data);
  return data;
}

DataValue* DataCommand::GetDataValue( int idx )
{
  return mDataVec.at(idx);
}

void DataCommand::SetCommandNodeApplySetting(gfl::xml::Node* node )
{
  const int startFrame = node->AttributeInteger( XML_NAME_COMMAND_ATTR_SFRAME , 0 );
  const int endFrame   = node->AttributeInteger( XML_NAME_COMMAND_ATTR_EFRAME , 0 );
  const int r   = node->AttributeInteger( XML_NAME_COMMAND_ATTR_COL_R , 255 );
  const int g   = node->AttributeInteger( XML_NAME_COMMAND_ATTR_COL_G , 0 );
  const int b   = node->AttributeInteger( XML_NAME_COMMAND_ATTR_COL_B , 0 );
  const int isActive = node->AttributeInteger( XML_NAME_COMMAND_ATTR_IS_ACITVE , 1 );

  gfl::core::Debug::PrintConsole("SetCommandNodeApplySetting[%d][%d]\n",startFrame,endFrame);

  SetCommandNode( node );

  mChipCol.setRgb( r,g,b );
  SetFrame( startFrame , endFrame );
  SetActive( isActive );
  
  gfl::xml::Node *macroNode = mDataEditor->GetMacroData()->GetMacroNode( node->FirstNode()->name() );
  if( macroNode )
  {
    mTitleStr = macroNode->AttributeString( "brief" );
  }
  else
  {
    gfl::core::Debug::PrintConsole("[%s]not found!\n",node->FirstNode()->name());
  }
  SetCommentStr( node->AttributeString(XML_NAME_COMMAND_ATTR_COMMENT) );
  
  QString modeType(macroNode->AttributeString( "mode" )); 
  if( modeType == "comment" )
  {
    mBar->SetMode(MODE_COMMENT);
  }
  gfl::core::Debug::PrintConsole("[%s:%s]\n",macroNode->AttributeString( "brief" ),macroNode->AttributeString( "mode" ));
}

void DataCommand::SetCommentStr(QString &str)
{
  mCommentStr = str;
  GetCommandBar()->setToolTip( mCommentStr );
}

void DataCommand::SetCommentStr(c8 *str)
{
  mCommentStr = str;
  GetCommandBar()->setToolTip( mCommentStr );
}


void DataCommand::SetColor(QColor& col)
{
  mChipCol = col;
  int r,g,b;
  mChipCol.getRgb(&r,&g,&b);

  mComNode->SetAttribute( XML_NAME_COMMAND_ATTR_COL_R , r );
  mComNode->SetAttribute( XML_NAME_COMMAND_ATTR_COL_G , g );
  mComNode->SetAttribute( XML_NAME_COMMAND_ATTR_COL_B , b );
}

void DataCommand::SaveCommandData()
{
  mComNode->SetAttribute( XML_NAME_COMMAND_ATTR_SFRAME , mStartFrame );
  mComNode->SetAttribute( XML_NAME_COMMAND_ATTR_EFRAME , mEndFrame );
  
  int r,g,b;
  mChipCol.getRgb(&r,&g,&b);

  mComNode->SetAttribute( XML_NAME_COMMAND_ATTR_COL_R , r );
  mComNode->SetAttribute( XML_NAME_COMMAND_ATTR_COL_G , g );
  mComNode->SetAttribute( XML_NAME_COMMAND_ATTR_COL_B , b );

  mComNode->SetAttribute( XML_NAME_COMMAND_ATTR_IS_ACITVE , (int)mIsActive );
}

bool DataCommand::LoadCommandData( gfl::xml::Node *node )
{
  SetCommandNodeApplySetting(node);
  return true;
}

bool DataCommand::IsLock()
{
  return mParent->IsLock();
}

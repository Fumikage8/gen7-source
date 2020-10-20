//======================================================================
/**
 * @file	dataMacro.cpp
 * @brief	–½—ßƒf[ƒ^
 * @author	ariizumi
 * @data	2011/03/07
 */
//======================================================================

#include "MainWindow.h"
#include "dataEditor.h"
#include "dataMacro.h"

//----------------------------------------------------
DataMacro::DataMacro( DataEditor *dataEditor )
:mDataEditor(dataEditor)
,mMacroDoc(NULL)
{
}

DataMacro::~DataMacro()
{
}

void DataMacro::LoadListXml( QString &filePath )
{

  QFileInfo fileInfo( filePath );

  mMacroDoc.Load( fileInfo.absoluteFilePath() );

  gfl::core::Debug::PrintConsole("LoadList:%s\n",filePath.toLocal8Bit().constData());

	gfl::xml::Node* node = mMacroDoc.RootNode( )->FirstNode( "macro" )->FirstNode( );
  QTreeWidget* comList = mDataEditor->GetMainWindow()->GetCommandList();

  mMacroNodeList.clear();
  comList->clear();

  int idx = 0;
  QTreeWidgetItem *topNode = NULL;
	for(; node; node = node->NextSibling( ) )
  {
  	QStringList comName(node->AttributeString( "brief" ));
    QTreeWidgetItem *item = new QTreeWidgetItem( comName );
    item->setData(0,Qt::UserRole,idx);

    QString parentSetting(node->AttributeString( "pos" )); 
    if( parentSetting == "parent" )
    {
		  comList->addTopLevelItem( item );
      topNode = item;
    }
    else
    if( parentSetting == "top" )
    {
		  comList->addTopLevelItem( item );
      topNode = NULL;
    }
    else
    {
      if( topNode )
      {
  		  topNode->addChild( item );
      }
      else
      {
  		  comList->addTopLevelItem( item );
      }
    }

    mMacroNodeList.append( node );
    idx++;
	}

  mDefineNode = mMacroDoc.RootNode( )->FirstNode( "define" );
  mPathDefNode = mMacroDoc.RootNode( )->FirstNode( "path_define" );
  mGroupOptionNode = mMacroDoc.RootNode( )->FirstNode( "group_option" );
}

gfl::xml::Node* DataMacro::GetMacroNode(const c8 *name)
{
	gfl::xml::Node* node = mMacroDoc.RootNode( )->FirstNode( "macro" )->FirstNode(name );
  return node;
}
gfl::xml::Node* DataMacro::GetMacroNode( const QString &name )
{
	gfl::xml::Node* node = mMacroDoc.RootNode( )->FirstNode( "macro" )->FirstNode(name );
  return node;
}


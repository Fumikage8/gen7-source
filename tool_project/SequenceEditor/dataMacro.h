//======================================================================
/**
 * @file	dataMacro.h
 * @brief	ñΩóﬂÉfÅ[É^
 * @author	ariizumi
 * @data	2011/08/02
 */
//======================================================================

#ifndef __DATA_MACRO_H__
#define __DATA_MACRO_H__

#include <gflib.h>
#include <rapidxml/gfl_Xml.h>
#include "localDef.h"

//----------------------------------------------------
class DataMacro
{
public:
	DataMacro( DataEditor *dataEditor );
	~DataMacro();

  void LoadListXml( QString &filePath );

  gfl::xml::Document* GetMacroDoc(){return &mMacroDoc;}
  gfl::xml::Node* GetMacroNode(int idx){return mMacroNodeList.value(idx);}
  gfl::xml::Node* GetMacroNode( const c8* name );
  gfl::xml::Node* GetMacroNode( const QString &name );

  gfl::xml::Node* GetDefineNode(){return mDefineNode;}
  gfl::xml::Node* GetPathDefNode(){return mPathDefNode;}
  gfl::xml::Node* GetGroupOptNode(){return mGroupOptionNode;}

private:

  DataEditor *mDataEditor;

  gfl::xml::Document mMacroDoc;
	QList<gfl::xml::Node*> mMacroNodeList;

  gfl::xml::Node *mDefineNode;
  gfl::xml::Node *mPathDefNode;
  gfl::xml::Node *mGroupOptionNode;
};

#endif //__DATA_MACRO_H__
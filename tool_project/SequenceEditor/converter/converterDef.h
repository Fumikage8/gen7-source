/* 
 * File:   convertDef.h
 * Author: ariizumi_nobuhiko
 *
 * Created on 2011/07/29
 */

#ifndef __CONVERTER_DEF_H__
#define __CONVERTER_DEF_H__

#include "../localDef.h"
#include <rapidxml/gfl_Xml.h>

class CommandData
{
public:
  CommandData( int startFrame , int endFrame , gfl::xml::Node *node , gfl::xml::Node *grpNode );
  
  //int GetDataNum(){return mDataVec.size();}

  const int GetStartFrame(){return mStartFrame;}
  const int GetEndFrame(){return mEndFrame;}
  gfl::xml::Node* GetComNode(){return mNode;}
  gfl::xml::Node* GetGrpNode(){return mGrpNode;}
  
  void Dump();
private:
  int mStartFrame;
  int mEndFrame;

  gfl::xml::Node *mNode;
  gfl::xml::Node *mGrpNode;


};

typedef std::list<CommandData*> ComDataList;
typedef ComDataList::iterator ComDataListIt;

typedef struct
{
  ComDataList mComList;
  gfl::xml::Node *mMacroNode;
  gfl::xml::Node *mDefineNode;
  gfl::xml::Node *mPathDefNode;
  gfl::xml::Node *mGroupOptionNode;
}CONVERTER_DATA;

#endif //__CONVERTER_DEF_H__

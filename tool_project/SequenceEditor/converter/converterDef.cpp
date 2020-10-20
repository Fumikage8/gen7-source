/* 
 * File:   convertDef.cpp
 * Author: ariizumi_nobuhiko
 *
 * Created on 2011/07/29
 */

#include "converterDef.h"

CommandData::CommandData( int startFrame , int endFrame , gfl::xml::Node *node , gfl::xml::Node *grpNode )
:mStartFrame(startFrame)
,mEndFrame(endFrame)
,mNode(node)
,mGrpNode(grpNode)
{

}


void CommandData::Dump()
{
  SeqDebugPrint( 1 , "F[%3d]-[%3d]\n",mStartFrame,mEndFrame);
}

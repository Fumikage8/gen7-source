//======================================================================
/**
 * @file	SequenceCommand.h
 * @brief	シーケンスコマンド自体
 * @author	PETE
 * @data	2015.04.27
 */
//======================================================================
#if !defined( __SEQUENCE_COMMAND_H__ )
#define __SEQUENCE_COMMAND_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include "Demolib/SeqEditor/include/SeqFile.h"
///#include "Demolib/SequencePlayer/include/Player.h"

GFL_NAMESPACE_BEGIN(DemoLib)
	GFL_NAMESPACE_BEGIN(Sequence)
	
// Command Def:
#define SEQPLAYER_CMD_DEF(name) extern void SEQ_CMD_ ## name ( DemoLib::SeqEditor::SeqFile *file ,void *userWork , const DemoLib::SeqEditor::CommandParam &param );
#include "DemoLib/SequencePlayer/include/seq_com.cdat"

#define SEQPLAYER_CMD_DEF(name) SEQCOM_ ## name ,
	enum
{
#include "DemoLib/SequencePlayer/include/seq_com.cdat"
	SEQPLAYER_CMD_MAX,
};
	
	GFL_NAMESPACE_END(Sequence)
GFL_NAMESPACE_END(DemoLib)

#endif //__SEQUENCE_COMMAND_H__
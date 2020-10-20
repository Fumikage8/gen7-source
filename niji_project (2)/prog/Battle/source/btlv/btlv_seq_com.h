#if !defined( __BTLV_SEQ_COM_H__ )
#define __BTLV_SEQ_COM_H__
//======================================================================
/**
 * @file	btlv_seq_com.h
 * @brief	バトル描画 シーケンスコマンド
 * @author	ariizumi
 * @data	15/4/22
 */
//======================================================================

namespace btl
{

#define BTL_SEQ_DEF(name) extern void BTLV_SEQCOM_ ## name ( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
#include "btlv_seq_com.cdat"


#define BTL_SEQ_DEF(name) SEQCOM_ ## name ,
enum
{
#include "btlv_seq_com.cdat"

  BTLV_SEQCOM_MAX,
};

}//namespace btl
#endif // __BTLV_SEQ_COM_H__

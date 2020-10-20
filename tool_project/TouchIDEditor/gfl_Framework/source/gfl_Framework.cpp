//==============================================================================
/**
 * @file	gfl_Unity.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2011/04/05, 15:11
 */
// =============================================================================

#include "core/gfl_Core.cpp"

#include "motion/gfl_Motion.cpp"

#include "physic/gfl_Physic.cpp"

#include "qt/gfl_Qt.cpp"

#ifdef USE_PROPERTY
void gfl::rtti::SeparateString( c8* func, c8* val, c8* buffer )
{
	const size_t size = strlen( buffer );
	size_t step = 0;
	for( size_t i = 0; i < size; ++i ){
		if(!step){
			if( buffer[i] == ' '){
				func[i] = 0x00;
				step = i+1;
			}else{
				func[i] = buffer[i];
			}
		}else{ // スペースで区切られた引数が続く場合があるので全部とる
			val[i-step] = buffer[i];
		}
	}
	if(step){
		val[size-step] = 0x00;
	}else{ // 引数なし関数
		func[size] = 0x00;
		val[0] = 0x00;
	}
//	gfl::core::Debug::PrintConsole( "func %s\n",func );
//	gfl::core::Debug::PrintConsole( "\tval %s %d-%d=%d\n",val,size,step,size-step );
}
#endif

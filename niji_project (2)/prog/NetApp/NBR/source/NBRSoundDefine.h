//==============================================================================
/**
 * @file	NBRSoundDefine.h
 * @brief	通信対戦受付　サウンド定義
 * @author	ichiraku_katsuhiko
 * @date	2015/06/26
 */
// =============================================================================

#if !defined( __NBR_SOUND_DEFINE_H__ )
#define	__NBR_SOUND_DEFINE_H__

#include <Sound/include/Sound.h>

namespace NetApp{
namespace NBR{

#define SE_DECIDE     ( SEQ_SE_DECIDE1 )      ///< 決定
#define SE_RETURN     ( SEQ_SE_CANCEL1 )      ///< 戻り
#define SE_MESSAGE    ( SEQ_SE_MESSAGE )      ///< メッセージ

}
}

#endif	/* __NBR_SOUND_DEFINE_H__ */


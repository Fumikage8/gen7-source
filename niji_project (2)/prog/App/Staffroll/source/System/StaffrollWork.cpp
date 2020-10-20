// ============================================================================
/*
 * @file StaffrollWork.cpp
 * @brief スタッフロールアプリのワーククラスです。
 * @date 2016.01.06
 * @author uchida_yuto
 */
// ============================================================================
#include "App/Staffroll/source/System/StaffrollWork.h"

//  gflib2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <util/include/gfl2_std_string.h>

//  nijiのインクルード
#include "System/include/ArcIdGetter.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

//  リソースのインクルード
#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Staffroll)
GFL_NAMESPACE_BEGIN(System)


  StaffrollWork::StaffrollWork( const u8 romVersion, bool bFirstEvent ) :
    ApplicationWorkBase( APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE )
  {
    _Clear();

    m_romVersion  = romVersion;
    m_bFirstEvent = bFirstEvent;
  }


  StaffrollWork::~StaffrollWork()
  {
  }

  void StaffrollWork::SetEventTextParam( StaffrollParamBinaryFormat& rParam )
  {
    gfl2::std::MemCopy( &rParam, &m_eventTextParam, sizeof( m_eventTextParam ) );
  }

/*
  //  １枚絵の画像arcid
  u32 StaffrollWork::GetImgTexArcId()
  {
    return ARCID_STAFF_ROLL_LYT_TEXTURE;
  }
*/

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Staffroll)
GFL_NAMESPACE_END(App)

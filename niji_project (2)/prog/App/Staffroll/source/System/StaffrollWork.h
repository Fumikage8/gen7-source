// ============================================================================
/*
 * @file StaffrollWork.h
 * @brief スタッフロールアプリのワーククラスです。
 * @date 2016.01.06
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( STAFFROLL_WORK_H_INCLUDE )
#define STAFFROLL_WORK_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "util/include/gfl2_std_string.h"

#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"

//  リソースのインクルード
#include <niji_conv_header/app/staffroll/data/staffroll_data.h>

//  前方宣言
GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(Util)

  class NetAppTrainerIconUtility;

GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(NetAppLib)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Staffroll)
GFL_NAMESPACE_BEGIN(System)

  //  前方宣言
  class StaffrollViewSystemAccessor;

  class StaffrollWork : public NetAppLib::System::ApplicationWorkBase
  {
    GFL_FORBID_COPY_AND_ASSIGN( StaffrollWork );

  private:

    static const int  APP_HEAP_SIZE             = 1024 * 1024 * 4;    //  4MB
    static const int  APP_DEVICE_HEAP_SIZE      = 1024 * 1024 * 16;   // 16MB

  public:
    StaffrollWork( const u8 romVersion, bool bFirstEvent );
    virtual~StaffrollWork();

    void SetViewSystemAccessor( StaffrollViewSystemAccessor* pAccessor )
    {
      GFL_ASSERT( pAccessor );
      m_pViewSystemAcceeor  = pAccessor;
    }

    //  Viewアクセスの取得・設定
    StaffrollViewSystemAccessor*  GetViewSystemAccessor() const { return m_pViewSystemAcceeor; }

    //  初回起動か
    bool  GetFirstEvent() const { return m_bFirstEvent; }

    //  ロムバージョン
    u8    GetRomVersion() const { return m_romVersion; }

    //  再生速度
    s32   GetPlaySpeed() const { return m_playSpeed; }
    void  SetPlaySpeed( const s32 num ) { m_playSpeed = num; }

    //  イベントテキストのパラメータ設定と取得
    void SetEventTextParam( StaffrollParamBinaryFormat& rParam );
    const StaffrollParamBinaryFormat& GetEventTextParam() const { return m_eventTextParam; }

    //  １枚絵の画像arcid
//    u32 GetImgTexArcId();

    //  １枚絵の画像ファイルが圧縮しているか
    bool IsImgTexFileComp() const { return true; }

    void AddPlaySpeed()
    {
      ++m_playSpeed;
      if( 255 <= m_playSpeed )
      {
        m_playSpeed = 255;
      }
    }

    void SubPlaySpeed()
    {
      --m_playSpeed;
      if( m_playSpeed <= 1 )
      {
        m_playSpeed = 1;
      }
    }

  private:
    void _Clear()
    {
      m_pViewSystemAcceeor  = NULL;
      m_bFirstEvent         = true;
      m_romVersion          = 0;
      m_playSpeed           = 1;

      gfl2::std::MemClear( &m_eventTextParam, sizeof( m_eventTextParam ) );
    }
    
    StaffrollViewSystemAccessor*  m_pViewSystemAcceeor;
    bool                          m_bFirstEvent;
    u8                            m_romVersion;
    s32                           m_playSpeed;

    StaffrollParamBinaryFormat    m_eventTextParam;
  };

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Staffroll)
GFL_NAMESPACE_END(App)

#endif // STAFFROLLWORK_H_INCLUDE

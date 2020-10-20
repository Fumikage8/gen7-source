
#ifndef __SCENECHANGECONTROL_H__
#define __SCENECHANGECONTROL_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include "FieldScript/include/ScriptTypes.h"  //SCRIPT_TYPE
#include "Savedata/include/EventWork.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( FieldScript )

//=============================================================================
/**
* @class SceneChangeControl
* @brief シーン遷移スクリプト制御用クラス
*/
//=============================================================================
class SceneChangeControl
{
private:
  /* ------------------------------------------------------------------------- */
  /**
  * @brief シーン遷移構造体
  */
  /* ------------------------------------------------------------------------- */
  struct SceneChange
  {
    SCRIPTID scrid;   ///< スクリプトID
    u16 wkno;         ///< ワークNO
    u16 wkval;        ///< ワーク値
    s32 fade_mode;    ///< フェードコントロールモード enum SceneChangeFadeMode
  };

  static const int SCENE_CHANGE_WORK_MAX = 8;             ///< シーン遷移ワーク数
  SceneChange m_SceneChangeWork[SCENE_CHANGE_WORK_MAX];   ///< シーン遷移ワーク
  u8 m_SceneChangeWorkCnt;                                ///< シーン遷移ワークカウンタ
  u8   m_uPad1;
  u16  m_uPad2;

public:
  /* ------------------------------------------------------------------------- */
  /**
  * @brief コンストラクタ
  */
  /* ------------------------------------------------------------------------- */
  SceneChangeControl()
  {
    Clear();
  }

  /* ------------------------------------------------------------------------- */
  /**
  * @brief デストラクタ
  */
  /* ------------------------------------------------------------------------- */
  ~SceneChangeControl(){}

  /* ------------------------------------------------------------------------- */
  /**
  * @brief シーン遷移スクリプトワーククリア
  */
  /* ------------------------------------------------------------------------- */
  void Clear()
  {
    for( u32 i=0; i<SCENE_CHANGE_WORK_MAX; ++i )
    {
      m_SceneChangeWork[i].scrid = SCRID_DUMMY;
      m_SceneChangeWork[i].wkno = 0;
      m_SceneChangeWork[i].wkval = 0;
      m_SceneChangeWork[i].fade_mode = SCENE_CHANGE_FADE_AUTO;
    }
    m_SceneChangeWorkCnt = 0;

    m_uPad1 = 0;
    m_uPad2 = 0;
  }

  /* ------------------------------------------------------------------------- */
  /**
  * @brief シーン遷移スクリプトを登録
  * @param scrid スクリプトID
  * @param wkno ワークNO
  * @param wkval ワーク値
  */
  /* ------------------------------------------------------------------------- */
  void Add( SCRIPTID scrid, u16 wkno, u16 wkval, s32 fade_mode )
  {
    if( m_SceneChangeWorkCnt < SCENE_CHANGE_WORK_MAX )
    {
      m_SceneChangeWork[ m_SceneChangeWorkCnt ].scrid = scrid;
      m_SceneChangeWork[ m_SceneChangeWorkCnt ].wkno = wkno;
      m_SceneChangeWork[ m_SceneChangeWorkCnt ].wkval = wkval;
      m_SceneChangeWork[ m_SceneChangeWorkCnt ].fade_mode = fade_mode;
      m_SceneChangeWorkCnt++;
    }
  }

  /* ------------------------------------------------------------------------- */
  /**
  * @brief シーン遷移スクリプトを検索
  * @param ev  EventWorkへのポインタ
  *
  * @return  u32 スクリプトID、存在しない場合はSCRID_DUMMY
  */
  /* ------------------------------------------------------------------------- */
  u32 Search( EventWork * ev ) const
  {
    for( u32 i=0; i<SCENE_CHANGE_WORK_MAX; ++i )
    {
      SCRIPTID scrid = m_SceneChangeWork[i].scrid;
      u16 wkno = m_SceneChangeWork[i].wkno;
      u16 wkval = m_SceneChangeWork[i].wkval;

      if( m_SceneChangeWork[i].scrid != SCRID_DUMMY )
      {
        u16 val = *ev->GetEventWorkAdrs( wkno );
        if( val == wkval )
        {
          return scrid;
        }
      }
    }
    return SCRID_DUMMY;
  }

  /* ------------------------------------------------------------------------- */
  /**
  * @brief シーン遷移スクリプトを検索し、マップチェンジ時のフェードオプションを取得する
  * @param ev  EventWorkへのポインタ
  *
  * @return  u32 スクリプトID、存在しない場合はSCRID_DUMMY
  */
  /* ------------------------------------------------------------------------- */
  SceneChangeFadeMode CheckFadeMode( EventWork * ev ) const
  {
    for( u32 i=0; i<SCENE_CHANGE_WORK_MAX; ++i )
    {
      u16 wkno = m_SceneChangeWork[i].wkno;
      u16 wkval = m_SceneChangeWork[i].wkval;

      if( m_SceneChangeWork[i].scrid != SCRID_DUMMY )
      {
        u16 val = *ev->GetEventWorkAdrs( wkno );
        if( val == wkval )
        {
          return static_cast<SceneChangeFadeMode>(m_SceneChangeWork[i].fade_mode);
        }
      }
    }
    return SCENE_CHANGE_FADE_AUTO;
  }

};      // End of Class SceneChangeControl

GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )

#endif // __SCENECHANGECONTROL_H__

//==============================================================================
/**
 * @file        ScriptEffectManager.h
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/06/11(木) 18:22:07
 */
//==============================================================================

#if !defined(__SCRIPTEFFECTMANAGER_H__) // 重複定義防止
#define __SCRIPTEFFECTMANAGER_H__ // 重複定義防止マクロ
#pragma once

#include <fs/include/gfl2_Fs.h>
#include <GameSys/include/GameManager.h>
#include "Field/FieldRo/include/Effect/FieldEffectManager.h"


GFL_NAMESPACE_BEGIN( Field );
class Fieldmap;
GFL_NAMESPACE_BEGIN( FieldScript );


//------------------------------------------------------------------------------
/**
 * @class       ScriptEffectManager
 * @brief       
 * @author      N.Takeda
 * @date        2015/06/11(木) 18:35:10
 */
//------------------------------------------------------------------------------
class ScriptEffectManager
{
private:
  enum{
    EFFECTMAX = Field::Effect::EffectManager::EVT_EFFECT_NUM,
    DYNAMIC_EFFECT_LOAD_MAX = EFFECTMAX*2, //覚えておける非常駐リソース履歴の段数
  };
  
  enum{ NO_ID = -1 };
  
  struct StEffect
  {
    s32                           m_iId;      ///< 識別番号,
    bool                          m_bNoLoop;  ///< ループでないか？
    Field::Effect::IEffectBase    *m_pPtr;    ///< エフェクトのポインタ.
  };
  
  StEffect                        m_Effect[ EFFECTMAX ];
  s32                             m_iEntry;
  GameSys::GameManager            *m_pGameMan;

  u32                             m_DynamicLoad[ DYNAMIC_EFFECT_LOAD_MAX ];
  s32                             m_DynamicLoadNum;
  
  u32                             m_uReleaseRequest[ EFFECTMAX ];
  u32                             m_iReleaseRequestNum;
  
  void  effDelete( const s32 iId );

  void loadDynamicResource( const Effect::Type iType );
  void unloadDynamicResource( const Effect::Type iType );
  
  void initEffect( const s32 ii )
  {
    m_Effect[ ii ].m_iId = NO_ID;
    m_Effect[ ii ].m_pPtr = NULL;
    m_Effect[ ii ].m_bNoLoop = true;
  }
public:
  void  SetPos( const s32 iId, const s32 iEffectType, const f32 fX, const f32 fY, const f32 fZ, bool se_play =true );
  void  SetPosScaleRot(const s32 iId, const s32 iEffectType, const f32 fX, const f32 fY, const f32 fZ, const f32 fScale = 1.0f, bool se_play = true, const f32 fRotX = 0.0f, const f32 fRotY = 0.0f, const f32 fRotZ = 0.0f);
  void  SetChr( const s32 iId, const s32 iEffectType, const s32 chrID, bool se_play =true, f32 stepFrame=1.0f, f32 scale=1.0f);
  void  SetTrainerEye( const s32 iId, const gfl2::math::VEC2& pos, const f32 rot, u32 eye_se_id );
  bool  IsEnd( const s32 iId );
  void  Delete( const s32 iId );
  void  AllDelete(){  Terminate();  }
  bool  CheckAllEnd();
  
  void  Update();

  static Field::Effect::Type TranslateId( const s32 iId );
  
  void  SetScale( const s32 iId, const f32 _fScale );

  void LoadDynamicResource( const s32 iEffectType ); // エフェクトタイポウ指定で非常駐リソースを読み込む
  void UnloadDynamicResource( const s32 iEffectType );  // エフェクトタイプの非常駐リソースを破棄
  void UnloadDynamicResource(void);// 読み込んだ非常駐リソースを破棄
  
  void  Terminate();

  Field::Effect::IEffectBase* GetEffect( const s32 iId );
  bool  ResourceReleaseRequest( const u32 _uEffectType );
  
  ScriptEffectManager( gfl2::heap::HeapBase * p_heap, GameSys::GameManager *pGameMan );
  virtual ~ScriptEffectManager();
};

GFL_NAMESPACE_END( FieldScript );
GFL_NAMESPACE_END( Field );

#endif // __SCRIPTEFFECTMANAGER_H__ 重複定義防止

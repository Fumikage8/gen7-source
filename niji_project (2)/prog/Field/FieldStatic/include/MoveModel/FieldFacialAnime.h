#pragma once
//=================================================================================================
/**
*  @file   FieldMoveModelFacialAnime.h
*  @brief  フィールド用 動作モデル表情制御マネージャー 
*  @author Miyuki Iwasawa@gamefreak.co.jp
*  @date   2015.10.30
*/
//=================================================================================================
#ifndef __FIELDMOVEMODELFACIALMANAGER_H__
#define __FIELDMOVEMODELFACIALMANAGER_H__

#include <debug/include/gfl2_DebugPrint.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/FieldNode.h"
//#include "../../../../../../poke_3d_lib/model/include/gfl2_CharaModelFactory.h"
//#include <model/include/gfl2_CharaModel.h>

namespace poke_3d { namespace model {
   class CharaModel;
   class CharaModelFactory;
}}

namespace Field {
  class MyRenderingPipeLine;
}

//=================================================================================================
//
// ※注意※  このクラスでは座標/姿勢/拡縮を保持してはいけません。
//           座標/姿勢/拡縮を保持して良いのは描画インスタンス(poke_3d::model::CharaModel)のみです。
//
//=================================================================================================
namespace Field{ namespace MoveModel {

class FieldFacialAnimeBase
{
public:
  struct CommandData{
    s32 anime_id;
    s32 frame;
  };
  enum{
    COMMAND_TABLE_SIZE = 32,
  };
  enum State{
    ANIME_STATE_NONE,
    ANIME_STATE_AUTO,
    ANIME_STATE_CHANGE,
    ANIME_STATE_INIT,
    ANIME_STATE_PLAY,
    ANIME_STATE_END,
  };
public:
  FieldFacialAnimeBase();
  virtual ~FieldFacialAnimeBase();

  virtual bool AnimeCommandInit();
  virtual bool AnimeCommandEntry( s32 anime_id, s32 frame );
  virtual bool AnimeCommandPlay();
  virtual s32  AnimeCommandUpdate();
  virtual bool AnimeCommandPlayCheck();
  virtual bool AnimeCommandInitCheck();
  
  virtual void Initialize( poke_3d::model::CharaModel* pCharaDrawInstance ){m_pCharaDrawInstance=pCharaDrawInstance;}
  virtual void PrevUpdate(){}
  virtual bool CheckAutoAnime(){return false;}
  virtual void SetAutoAnime( bool set ){}
  virtual void ChangeAnime( s32 eyeIndex ){}
  virtual void ResetAnime(){}

  s32 GetMouthIndex() const{  return (m_pCharaDrawInstance) ? m_pCharaDrawInstance->GetMouthIndex() : 0; }
  
protected:
  State     m_state;      ///<ステータス
  u32       m_seq;        ///<シーケンス
  u32       m_frame;      ///<アニメーションフレーム
  u32       m_comNum;     ///<テーブルに登録されたアニメ数
  CommandData m_comTable[COMMAND_TABLE_SIZE];
  
  poke_3d::model::CharaModel* m_pCharaDrawInstance;
};

class FieldFacialAnimeEye : public FieldFacialAnimeBase
{
public:
  FieldFacialAnimeEye();
  virtual ~FieldFacialAnimeEye(){};

  virtual void Initialize( poke_3d::model::CharaModel* pCharaDrawInstance );
  virtual void PrevUpdate();
  virtual bool CheckAutoAnime();
  virtual void SetAutoAnime( bool set );
  virtual void ChangeAnime( s32 eyeIndex );
  virtual void ResetAnime();
  
};

class FieldFacialAnimeMouth : public FieldFacialAnimeBase
{
public:
  FieldFacialAnimeMouth();
  virtual ~FieldFacialAnimeMouth(){};

  virtual void Initialize( poke_3d::model::CharaModel* pCharaDrawInstance );
  virtual void PrevUpdate();
  virtual bool CheckAutoAnime();
  virtual void SetAutoAnime( bool set );
  virtual void ChangeAnime( s32 eyeIndex );
  virtual void ResetAnime();
  
  void AutoAnimeUpdate();

private:
  bool m_isAutoAnime;
  u32  m_autoAnimeSequence;
  u32  m_autoAnimeFrame;
  u32  m_autoAnimeInterval;

};



} //end of namespace MoveModel
} //end of namespace Field

#endif //__FIELDMOVEMODELFACIALMANAGER_H__


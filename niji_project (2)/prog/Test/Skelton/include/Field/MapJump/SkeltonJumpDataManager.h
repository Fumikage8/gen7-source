#if defined(GF_PLATFORM_WIN32)
//*************************************************************************************************
/**
 @file jump_data_managera.h
 @brief 擬似マップジャンプのデータ管理
 @date 2015.01.22
 @author saita_kazuki
 */
//*************************************************************************************************

#ifndef __SKELTON_MAPJUMP_JUMPDATAMANAGER_H__
#define __SKELTON_MAPJUMP_JUMPDATAMANAGER_H__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include <math/include/gfl2_Vector4.h>
#include <xml/include/rapidxml.hpp> // no need to link

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)


// マップジャンプデータ
struct JUMP_DATA
{
  u32                 id;         // 通し番号
  c8                  name[512];  // 名前(メッシュ名と一致するもの)
  gfl2::math::Vector4  pos;        // ジャンプ後位置
  u32                 priority;   // 優先度(0〜。値が小さいほど優先度高い)
  u32                 fadeFrame;  // 暗転フレーム数
  f32                 angle;      // ジャンプ後向き
};

// マップジャンプデータ管理
class JumpDataManager
{
public:
  static const u32   JUMP_DATA_MAX;         // データ最大数
  static const f32   JUMP_DATA_ANGLE_NONE;  // ジャンプデータの向きが格納されていないとする値

public:

  JumpDataManager();
  virtual ~JumpDataManager();

  void Initialize();
  void Terminate();

  b32 LoadData( const c8* pFilePath);
  b32 LoadData( const c16* pFilePath);
  void CreateXml( const c16* pFilePath );

  /**
  * @brief データ取得
  * @param pName 検索するデータ名
  * @return マップジャンプデータ
  */
  const JUMP_DATA* GetData( const c8* pName, bool isForce = false) const;
  const JUMP_DATA* GetDataByIndex( s32 index, bool isForce = false) const;

  void Lock();
  void Unlock();

private:

  b32 UpdateImpl( c8* pData );
  b32 LoadJumpData( const rapidxml::xml_node<>* pJumpDataNode, JUMP_DATA* pJumpData);

  JUMP_DATA*  m_pJumpDataTable;   // ジャンプデータテーブル xmlから読み込んだ情報を格納する
  u32         m_useDataNum;       // データ数
  bool        m_isLock;           // データ取得をロックしているか
};

GFL_NAMESPACE_END(Skelton)
GFL_NAMESPACE_END(Test)

#endif // __SKELTON_MAPJUMP_JUMPDATAMANAGER_H__
#endif // GF_PLATFORM_WIN32



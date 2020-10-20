#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANPRISMSHEEt_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANPRISMSHEEt_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanPrismSheet.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2016.02.20
 @brief   図鑑のキラカード
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <Layout/include/gfl2_Layout.h>

// 図鑑のインクルード
#include "ZukanDefine.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
  class G2DUtil;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)




#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanPrismSheet
 @brief     図鑑のキラカード
 */
//==============================================================================
class ZukanPrismSheet
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanPrismSheet);

public:
  // キラ状態に対するリクエスト
  enum ReqId
  {
    REQ_ID_NONE,       //!< リクエストなし(システムで使う)
    REQ_ID_PRISM_ON,   //!< キラON
    REQ_ID_PRISM_OFF,  //!< キラOFF  // 必ず処理する
  };

public:
  ZukanPrismSheet(
    app::util::Heap*          appHeap,
    gfl2::lyt::LytWk*         pLytWk,
    u32                       lytWkId
  );
  virtual ~ZukanPrismSheet();
  bool CanDestroy(void) const;

  void Update(void);

  void SetVisible(bool bVisible);

  // キラ状態に対するリクエスト
  void Req(ReqId reqId);
 
private:
  enum PiecePosType
  {
    PIECE_POS_TYPE_RT,
    PIECE_POS_TYPE_RB,
    PIECE_POS_TYPE_LB,
    PIECE_POS_TYPE_LT,
    PIECE_POS_TYPE_NUM,
  };
  
  // ジャイロ状態
  enum GyroState
  {
    GYRO_STATE_NONE,  //!< ジャイロ無効
    GYRO_STATE_INIT,  //!< ジャイロ初期化中
    GYRO_STATE_WORK,  //!< ジャイロ稼働中
  };
  
  // キラ状態
  enum PrismState
  {
    PRISM_STATE_OFF,      //!< キラOFF
    PRISM_STATE_ON_IN,    //!< キラON開始中
    PRISM_STATE_ON_WORK,  //!< キラON
    PRISM_STATE_ON_OUT,   //!< キラON終了中  // 必ず最後まで処理する
  };

  // 明るさ減衰状態
  enum ReduceState
  {
    REDUCE_STATE_STOP,            //!< 停止中。減衰なし。
    REDUCE_STATE_STOP_TO_MOVE,    //!< 動き始めた。減衰が大きくなっていく。
    REDUCE_STATE_MOVE,            //!< 動いている。減衰が最大。
    REDUCE_STATE_MOVE_TO_STOP,    //!< 止まり始めた。減衰から回復中。
  };

  // ジャイロ稼動中の角度
  enum GyroMoveRatio
  {
    GYRO_MOVE_RATIO_STOP,         //!< 動いていない
    GYRO_MOVE_RATIO_MOVE_SMALL,   //!< 小さく動いている
    GYRO_MOVE_RATIO_MOVE_BIG,     //!< 大きく動いている
  };

  static const s32 BLOCK_X_NUM;
  static const s32 BLOCK_Y_NUM;
  static const gfl2::math::VEC2 ORIGIN_BLOCK_POS;
  static const gfl2::math::VEC2 BLOCK_SIZE;
  static const gfl2::math::VEC2 PIECE_SIZE;
  static const s32 GYRO_INIT_COUNT_MAX;
  static const s32 GYRO_LOG_NUM_MAX;

  class Block
  {
  public:
    gfl2::lyt::LytPane*    blockPane;
    gfl2::lyt::LytPicture* piecePane[PIECE_POS_TYPE_NUM];
  public:
    Block(void)
      : blockPane(NULL)
    {
      for(s32 i=0; i<PIECE_POS_TYPE_NUM; ++i)
      {
        piecePane[i] = NULL;
      }
    }
  };

private:
  void createBlock(void);
  void destroyBlock(void);

  // ジャイロ状態
  void initGyro(void);
  void termGyro(void);
  void updateGyro(void);

  // キラ状態を更新する
  void updatePrism(void);

  // ジャイロのログを更新する
  void getGyroAngle(void);

  // 判定関数
  GyroMoveRatio getGyroMoveRatio(void) const;

  // 明るさ減衰状態
  void changeReduceState(ReduceState reduceState)
  {
    m_reduceState = reduceState;
    m_reduceStateCount = 0;
  }

  // 変換関数
  static f32 normalizeAngle(f32 angle);  // angleを0.0f<=angle<1.0fにして返す
  static u8 convertAngleToColor(f32 angle);  // 0.0f(=0度)<=angle<1.0f(=360度)
  static u8 convertAngleToColorFast(f32 angle);  // angleは必ず0.0f(=0度)<=angle<1.0f(=360度)にして渡すこと
  static u8 convertF32ColorToU8Color(f32 color);  // 0.0f<=f32<=1.0fの色を0<=u8<=255にして返す。(color<0.0f||1.0f<color)のときは範囲内におさめられる。

  // 計算
  static u8 calcVertexColor(s32 targetPlace, s32 targetColor, s32 x, s32 y, const gfl2::math::VEC3& angleDegree, f32 angleDegreeAverage);

private:
  app::util::Heap*                    m_appHeap;
  app::util::G2DUtil*                 m_pG2DUtil;
  gfl2::lyt::LytMultiResID            m_lytRes;
  gfl2::lyt::LytWk*                   m_pLytWk;
  u32                                 m_lytWkId;

  bool  m_canDestroyFlag;

  gfl2::lyt::LytPane*     m_rootPane;
  gfl2::lyt::LytPane*     m_blockTopPane;
  Block                   m_originalBlock;

  Block**                 m_block;  // m_blook[BLOCK_X_NUM][BLOCK_Y_NUM]となるようにメモリを確保する

  // ジャイロ状態(ジャイロ起動/終了に関する状態。ユーザが本体を傾けているか否かは扱っていない)
  bool       m_gyroCreateFlag;
  GyroState  m_gyroState;
  s32        m_gyroInitCount;

  // キラ状態(キラ表示に関する状態。本体傾きによるON/OFF、ユーザリクエストによるON/OFFを扱っている)
  PrismState  m_prismState;
  f32         m_prismFadeRate;  // 0.0fで完全OFF、1.0fで完全ON
  bool        m_reqIsOn;  // ユーザのリクエストがON状態ならtrue

  // 明るさ減衰
  ReduceState  m_reduceState;
  s32          m_reduceStateCount;
  f32          m_reduceRate;

  // ジャイロのログ(ジャイロ稼働中の角度を保持している)
  gfl2::math::VEC3*  m_gyroAngle;       // m_gyroAngle[GYRO_LOG_NUM_MAX]となるようにメモリを確保する。[0]が今、[1]が1個前、[2]が2個前、、、[GYRO_LOG_NUM_MAX-1]が(GYRO_LOG_NUM_MAX-1)個前。
  bool*              m_gyroAngleValid;  // m_gyroAngleValid[GYRO_LOG_NUM_MAX]となるようにメモリを確保する。m_gyroAngle[i]が有効か否か。
  s32                m_gyroAngleNum;    // GYRO_LOG_NUM_MAXまで数える。つまり最大GYRO_LOG_NUM_MAXまでしかならない。

  // kawa_test2
  // 頂点二次元配列
  s32              m_vertexXNum;
  s32              m_vertexYNum;
  nw::ut::Color8** m_vertexColor;  // m_vertexColor[m_vertexXNum][m_vertexYNum]となるようにメモリを確保する。左上の頂点
  bool isVertexPosBlockCenter(s32 vertexX, s32 vertexY) const;
  void getVertexPos(s32* vertexX, s32* vertexY, const s32 blockX, const s32 blockY, const s32 piecePos, const u32 vertexIndex) const;
  // bg
  gfl2::lyt::LytPicture*  m_alphaValDrawPane;
  gfl2::lyt::LytPicture*  m_bgPane;
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANPRISMSHEEt_H_INCLUDED


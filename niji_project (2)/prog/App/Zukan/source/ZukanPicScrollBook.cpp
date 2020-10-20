//==============================================================================
/**
 @file    ZukanPicScrollBook.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.11.11
 @brief   図鑑の絵巻ブック
 */
//==============================================================================


// gfl2のインクルード
#include <Layout/include/gfl2_Layout.h>

// nijiのインクルード
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <Savedata/include/ZukanSave.h>

#include <PokeTool/include/PokeToolExtendData.h>

#include <AppLib/include/Util/app_util_2d.h>

#include <Sound/include/Sound.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/zukan/alola_top.h>
#include <niji_conv_header/app/zukan/alola_top_pane.h>
#include <niji_conv_header/app/zukan/alola_top_anim_list.h>

#include <niji_conv_header/message/msg_zukan.h>

// 図鑑のインクルード
#include "ZukanContents.h"
#include "ZukanPokePicManager.h"
#include "ZukanPicScrollDataAccessor.h"
#include "ZukanPicScrollSheet.h"
#include "ZukanPicScrollUtil.h"

#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
#include "ZukanPrismSheet.h"
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE

#include "ZukanPicScrollBook.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)

//==============================================================================
//
//  ZukanPicScrollSheetInfo
//
//==============================================================================
class ZukanPicScrollSheetInfo
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanPicScrollSheetInfo);

public:
  enum State
  {
    STATE_NONE,  // 未使用状態
    STATE_INIT,  // Init呼び出し後の状態
    STATE_TERM,  // Term呼び出し後の状態
  };

public:
  ZukanPicScrollSheetInfo(void)
    : m_sheet(NULL),
      m_sheetIndex(ZukanPicScrollBook::SHEET_INDEX_NONE),
      m_placeGroupTopAlolaNo(0),
      m_state(STATE_NONE)
  {
  }
  virtual ~ZukanPicScrollSheetInfo()
  {
    GFL_SAFE_DELETE(m_sheet);
  }

  void Setup(
    app::util::Heap*                    pAppHeap,
    app::util::G2DUtil*                 pG2DUtil,
    gfl2::lyt::LytMultiResID            lytRes,
    gfl2::lyt::LytWk*                   pLytWk,
    u32                                 lytWkId,
    const ZukanPicScrollDataAccessor*   pPicScrollDataAccessor,
    const PokeTool::ExtendData*         pPokeExtendData,
    gfl2::str::MsgData*                 pMsgData,  // <niji_conv_header/message/msg_zukan.h>
    print::WordSet*                     pWordSet,
    ZukanContents*                      pContents,
    ZukanPokePicManager*                pPokePicManager
  ) 
  {
    m_sheet = GFL_NEW(pAppHeap->GetDeviceHeap()) ZukanPicScrollSheet(
        pAppHeap,
        pG2DUtil,
        lytRes,
        pLytWk,
        lytWkId,
        pPicScrollDataAccessor,
        pPokeExtendData,
        pMsgData,
        pWordSet,
        pContents,
        pPokePicManager
    );
  }

  void InitSheet(s32 sheetIndex, u32 placeGroupTopAlolaNo)
  {
    m_sheetIndex           = sheetIndex;
    m_placeGroupTopAlolaNo = placeGroupTopAlolaNo;
    m_state                = STATE_INIT;

    // 初期表示をつくる
    m_sheet->InitSheet(placeGroupTopAlolaNo);
  }
  void TermSheet(void)
  {
    m_state = STATE_TERM;

    // 表示しない
    m_sheet->UnuseSheet();
  }
  void UnuseSheet(void)
  {
    m_sheetIndex           = ZukanPicScrollBook::SHEET_INDEX_NONE;
    m_placeGroupTopAlolaNo = 0;
    m_state                = STATE_NONE;

    // 表示しない
    m_sheet->UnuseSheet();
  }

  void SetPosY(f32 posY)
  {
    m_sheet->SetPosY(posY);
  }

  void UpdateSheet(void)
  {
    m_sheet->UpdateSheet();
  }

  void ReqInAnim(void)
  {
    m_sheet->ReqInAnim();
  }
  void ReqOutAnim(void)
  {
    m_sheet->ReqOutAnim();
  }

  s32 GetSheetIndex(void) const  // 使用しないシートのときはZukanPicScrollBook::SHEET_INDEX_NONEが返ってくる
  {
    return m_sheetIndex;
  }
  State GetState(void) const
  {
    return m_state;
  }

private:
  // 一度設定したら固定のもの
  ZukanPicScrollSheet*  m_sheet;

  // 変動するもの
  s32    m_sheetIndex;  // なしのときはZukanPicScrollBook::SHEET_INDEX_NONE
  u32    m_placeGroupTopAlolaNo;
  State  m_state;
};




//==============================================================================
//
//  ZukanPicScrollBgInfo
//
//==============================================================================
class ZukanPicScrollBgInfo
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanPicScrollBgInfo);

public:
  ZukanPicScrollBgInfo(void)
    : m_pG2DUtil(NULL),
      m_pCurtainLytWk(NULL),
      m_curtainLytWkId(0),
      m_pBackmostLytWk(NULL),
      m_backmostLytWkId(0),
      m_bgInfo(NULL),
      m_gradeVal(ZukanPicScrollDataAccessor::GRADE_VAL_NORMAL),
      m_state(STATE_OFF),
      m_reqExist(false),
      m_reqGradeVal(ZukanPicScrollDataAccessor::GRADE_VAL_NORMAL)
  {}
  virtual ~ZukanPicScrollBgInfo()
  {
    GFL_SAFE_DELETE(m_bgInfo);
  }

  void Setup(
    app::util::Heap*    pAppHeap,
    app::util::G2DUtil* pG2DUtil,
    gfl2::lyt::LytWk*   pCurtainLytWk,
    u32                 curtainLytWkId,
    gfl2::lyt::LytWk*   pBackmostLytWk,
    u32                 backmostLytWkId
  )
  {
    m_pG2DUtil         = pG2DUtil;
    m_pCurtainLytWk    = pCurtainLytWk;
    m_curtainLytWkId   = curtainLytWkId; 
    m_pBackmostLytWk   = pBackmostLytWk;
    m_backmostLytWkId  = backmostLytWkId; 

    m_bgInfo = GFL_NEW(pAppHeap->GetDeviceHeap()) ZukanPicScrollUtil::BgInfo;
    ZukanPicScrollUtil::SetupBgInfo(
        m_bgInfo,
        pCurtainLytWk,
        curtainLytWkId,
        pBackmostLytWk,
        backmostLytWkId
    );
  }

  void Req(ZukanPicScrollDataAccessor::GradeVal gradeVal)
  {
    if(m_state == STATE_OFF)
    {
      m_gradeVal = gradeVal;
      m_reqExist = false;  // すぐ開始するのでリクエストなし
      this->startInAnim();
    }
    else
    {
      if(m_state == STATE_IN || m_state == STATE_LOOP)
      {
        if(m_gradeVal == gradeVal)
        {
          m_reqExist = false;  // このままでいいのでリクエストなし
        }
        else
        {
          m_reqExist = true;
          m_reqGradeVal = gradeVal;
          if(m_state == STATE_LOOP)
          {
            this->startOutAnim();
          }
        }
      }
      else  // if(m_state == STATE_OUT)
      {
        m_reqExist = true;
        m_reqGradeVal = gradeVal;
      }
    }
  }
  
  void Update(void)
  {
    switch(m_state)
    {
    case STATE_IN:
      {
        if(    
               m_pG2DUtil->IsAnimeEnd( m_curtainLytWkId , m_bgInfo->bgInAnimId[ZukanPicScrollUtil::BgInfo::BG_CURTAIN ][m_gradeVal] )
            && m_pG2DUtil->IsAnimeEnd( m_backmostLytWkId, m_bgInfo->bgInAnimId[ZukanPicScrollUtil::BgInfo::BG_BACKMOST][m_gradeVal] )
        )  // BG_CURTAINとBG_BACKMOSTのインアニメの尺は同じはず
        {
          m_pG2DUtil->StartAnime( m_curtainLytWkId , m_bgInfo->bgLoopAnimId[ZukanPicScrollUtil::BgInfo::BG_CURTAIN ][m_gradeVal] );
          m_pG2DUtil->StartAnime( m_backmostLytWkId, m_bgInfo->bgLoopAnimId[ZukanPicScrollUtil::BgInfo::BG_BACKMOST][m_gradeVal] );
          m_state = STATE_LOOP;
          //breakしない
        }
        else
        {
          break;
        }
      }
    case STATE_LOOP:
      {
        if(m_reqExist)
        {
          this->startOutAnim();
          //breakしない
        }
        else
        {
          break;
        }
      }
    case STATE_OUT:
      {
        if( 
               m_pG2DUtil->IsAnimeEnd( m_curtainLytWkId , m_bgInfo->bgOutAnimId[ZukanPicScrollUtil::BgInfo::BG_CURTAIN ][m_gradeVal] )
            && m_pG2DUtil->IsAnimeEnd( m_backmostLytWkId, m_bgInfo->bgOutAnimId[ZukanPicScrollUtil::BgInfo::BG_BACKMOST][m_gradeVal] )
        )  // BG_CURTAINとBG_BACKMOSTのインアニメの尺は同じはず
        {
          this->finishOutAnim();
        }
        break;
      }
    case STATE_OFF:
    default:
      {
        // 何もしない
        break;
      }
    }
  }

private:
  enum State
  {
    STATE_OFF,   // 何も表示していない(最初だけの状態)
    STATE_IN,    // インアニメ中
    STATE_LOOP,  // ループ中
    STATE_OUT,   // アウトアニメ中
  };

private:
  void startInAnim(void)  // m_stateを変更する
  {
    static const u32 SE_BG_IN[ZukanPicScrollDataAccessor::GRADE_VAL_NUM] =
    {
      SEQ_SE_SYS_NJ_065,  // 登録演出の背景が出る　ノーマル
      SEQ_SE_SYS_NJ_066,  // 登録演出の背景が出る　準伝
      SEQ_SE_SYS_NJ_067,  // 登録演出の背景が出る　伝説
    };

    for(s32 j=0; j<ZukanPicScrollUtil::BgInfo::BG_NUM; ++j)
    {
      for(s32 i=0; i<ZukanPicScrollDataAccessor::GRADE_VAL_NUM; ++i)
      {
        m_bgInfo->bgPane[j][i]->SetVisible(i==m_gradeVal);
      }
    }

    m_pG2DUtil->StartAnime( m_curtainLytWkId , m_bgInfo->bgInAnimId[ZukanPicScrollUtil::BgInfo::BG_CURTAIN ][m_gradeVal] );
    m_pG2DUtil->StartAnime( m_backmostLytWkId, m_bgInfo->bgInAnimId[ZukanPicScrollUtil::BgInfo::BG_BACKMOST][m_gradeVal] );
    Sound::PlaySE(SE_BG_IN[m_gradeVal]);
    Sound::ChangeSEVolume(SE_BG_IN[m_gradeVal], 0.4f);

    m_state = STATE_IN;
  }
  void startOutAnim(void)  // m_stateを変更する
  {
    m_pG2DUtil->StartAnime( m_curtainLytWkId , m_bgInfo->bgOutAnimId[ZukanPicScrollUtil::BgInfo::BG_CURTAIN ][m_gradeVal] );
    m_pG2DUtil->StartAnime( m_backmostLytWkId, m_bgInfo->bgOutAnimId[ZukanPicScrollUtil::BgInfo::BG_BACKMOST][m_gradeVal] );
    m_state = STATE_OUT;
  } 
  void finishOutAnim(void)  // m_stateを変更する
  {
    m_pG2DUtil->StopAnime( m_curtainLytWkId , m_bgInfo->bgOutAnimId[ZukanPicScrollUtil::BgInfo::BG_CURTAIN ][m_gradeVal], false );
    m_pG2DUtil->StopAnime( m_backmostLytWkId, m_bgInfo->bgOutAnimId[ZukanPicScrollUtil::BgInfo::BG_BACKMOST][m_gradeVal], false );
    if(m_reqExist)
    {
      m_gradeVal = m_reqGradeVal;
      m_reqExist = false;  // リクエストがかかっていたことを実行する
      this->startInAnim();
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。次のリクエストは必ずあるはず。
      m_state = STATE_OFF;
    }
  }

private:
  app::util::G2DUtil*  m_pG2DUtil;
  gfl2::lyt::LytWk*    m_pCurtainLytWk;
  u32                  m_curtainLytWkId; 
  gfl2::lyt::LytWk*    m_pBackmostLytWk;
  u32                  m_backmostLytWkId; 
  
  ZukanPicScrollUtil::BgInfo*  m_bgInfo;

  ZukanPicScrollDataAccessor::GradeVal m_gradeVal;
  State                                m_state;

  bool                                 m_reqExist;
  ZukanPicScrollDataAccessor::GradeVal m_reqGradeVal;
};




//==============================================================================
//
//  ZukanPicScrollBook
//
//==============================================================================

ZukanPicScrollBook::ZukanPicScrollBook(
  app::util::Heap*                    pAppHeap,
  app::util::G2DUtil*                 pG2DUtil,
  gfl2::lyt::LytMultiResID            lytRes,
  gfl2::lyt::LytWk*                   pLytWkSheet0,
  gfl2::lyt::LytWk*                   pLytWkSheet1,
  gfl2::lyt::LytWk*                   pLytWkSheet2,
  u32                                 lytWkIdSheet0,
  u32                                 lytWkIdSheet1,
  u32                                 lytWkIdSheet2,
  gfl2::lyt::LytWk*                   pBackmostLytWk,
  gfl2::lyt::LytWk*                   pCurtainLytWk,
  u32                                 backmostLytWkId,
  u32                                 curtainLytWkId,
  const ZukanPicScrollDataAccessor*   pPicScrollDataAccessor,
  const PokeTool::ExtendData*         pPokeExtendData,
  gfl2::str::MsgData*                 pMsgData,
  print::WordSet*                     pWordSet,
  ZukanContents*                      pContents,
  ZukanPokePicManager*                pPokePicManager
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
  , ZukanPrismSheet*                  pPrismSheet
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
)
  : m_pG2DUtil(pG2DUtil),
    m_lytRes(lytRes),
    m_pBackmostLytWk(pBackmostLytWk),
    m_pCurtainLytWk(pCurtainLytWk),
    m_backmostLytWkId(backmostLytWkId),
    m_curtainLytWkId(curtainLytWkId),
    m_pPicScrollDataAccessor(pPicScrollDataAccessor),
    m_pPokeExtendData(pPokeExtendData),
    m_pMsgData(pMsgData),
    m_pWordSet(pWordSet),
    m_pContents(pContents),
    m_pPokePicManager(pPokePicManager),
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
    m_pPrismSheet(pPrismSheet),
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
    m_sheetNum(0),
    m_alolaNo_end(0),
    m_sheetLytWkPtrList(NULL),
    m_sheetIndexList(NULL),
    m_placeGroupTopAlolaNoList(NULL),
    m_sheetInfo(NULL),
    m_bgInfo(NULL),
    m_state(STATE_STAY),
    m_stateSeq(0),
    m_goalSheetIndex(0),
    m_currSheetIndexF32(0.0f),
    m_currSheetIndex(0),
    m_currMoveDir(0),
    m_currMoveSpeed(0.0f)
{
  m_pLytWk[LYT_WK_PTR_SHEET_0] = pLytWkSheet0;
  m_pLytWk[LYT_WK_PTR_SHEET_1] = pLytWkSheet1;
  m_pLytWk[LYT_WK_PTR_SHEET_2] = pLytWkSheet2;

  m_lytWkId[LYT_WK_PTR_SHEET_0] = lytWkIdSheet0;
  m_lytWkId[LYT_WK_PTR_SHEET_1] = lytWkIdSheet1;
  m_lytWkId[LYT_WK_PTR_SHEET_2] = lytWkIdSheet2;

  this->setup(pAppHeap);
}
  
ZukanPicScrollBook::~ZukanPicScrollBook()
{
  GFL_SAFE_DELETE(m_bgInfo);
  GFL_SAFE_DELETE_ARRAY(m_sheetInfo);
  GFL_SAFE_DELETE_ARRAY(m_sheetIndexList);
  GFL_SAFE_DELETE_ARRAY(m_placeGroupTopAlolaNoList);
  GFL_SAFE_DELETE_ARRAY(m_sheetLytWkPtrList);
}

void ZukanPicScrollBook::Update(void)
{
  //////////////////////////////////
  // 前の値を覚えておく
  //////////////////////////////////
  s32 prevSheetIndex = m_currSheetIndex;
  s32 prevMoveDir = m_currMoveDir;

  //////////////////////////////////
  // シートを移動させる
  //////////////////////////////////
  //static const f32 MOVE_SPEED = 0.1f;
  //static const f32 MOVE_SPEED = 0.334f;
  enum
  {
    MOVE_GEAR_1,    // 1つずつ遅い
    MOVE_GEAR_2,    // 1つずつ速い
    MOVE_GEAR_3,    // 1ページずつ
    MOVE_GEAR_4,    // 真逆までワープ
    MOVE_GEAR_NUM,
    MOVE_GEAR_MAX = MOVE_GEAR_4
  };
  static const f32 MOVE_SPEED[MOVE_GEAR_NUM] =
  {
     0.1f,    // 差 <  1
     0.334f,  // 差 <  3
     2.0f,    // 差 <  6
    28.57f,//42.858f,  //  6 <= 差
  };
  static const f32 MOVE_DIFF[MOVE_GEAR_NUM] =
  {
    1.0f,
    3.0f,
    6.0f,
    0.0f,  // 使わない
  };
  static const f32 CURR_MOVE_RATE = 0.7f;

  switch(m_state)
  {
  case STATE_STAY:
    {
      // シートが止まったのでインアニメを再生する
      if(m_stateSeq == 0)
      {
        m_sheetInfo[ m_sheetLytWkPtrList[m_currSheetIndex] ].ReqInAnim();
        m_stateSeq = 1;
      }
      break;
    }
  case STATE_MOVE:
    {
      // シートが動いたのでアウトアニメを再生する
      if(m_stateSeq == 0)
      {
        m_sheetInfo[ m_sheetLytWkPtrList[m_currSheetIndex] ].ReqOutAnim();
        m_stateSeq = 1;
      }

      // 移動させる
      f32 goalSheetIndexF32 = static_cast<f32>(m_goalSheetIndex);
      if( m_currSheetIndexF32 < goalSheetIndexF32 )
      {
        f32 moveDiff = goalSheetIndexF32 - m_currSheetIndexF32;
        f32 moveSpeed = MOVE_SPEED[MOVE_GEAR_MAX];
        for(s32 i=0; i<MOVE_GEAR_MAX; ++i)
        {
          if(moveDiff < MOVE_DIFF[i])
          {
            moveSpeed = MOVE_SPEED[i];
            break;
          }
        }
        if(prevMoveDir > 0)
        {
          m_currMoveSpeed = m_currMoveSpeed*CURR_MOVE_RATE + moveSpeed*(1.0f-CURR_MOVE_RATE);
        }
        else
        {
          m_currMoveSpeed = moveSpeed;
        }
        m_currMoveDir = +1;

        m_currSheetIndexF32 += moveSpeed;
        if(m_currSheetIndexF32 >= goalSheetIndexF32)
        {
          this->changeState(STATE_STAY);
        }
      }
      else  // ( m_currSheetIndexF32 >= goalSheetIndexF32 )
      {
        f32 moveDiff = m_currSheetIndexF32 - goalSheetIndexF32;
        f32 moveSpeed = MOVE_SPEED[MOVE_GEAR_MAX];
        for(s32 i=0; i<MOVE_GEAR_MAX; ++i)
        {
          if(moveDiff < MOVE_DIFF[i])
          {
            moveSpeed = MOVE_SPEED[i];
            break;
          }
        }
        if(prevMoveDir < 0)
        {
          m_currMoveSpeed = m_currMoveSpeed*CURR_MOVE_RATE + moveSpeed*(1.0f-CURR_MOVE_RATE);
        }
        else
        {
          m_currMoveSpeed = moveSpeed;
        }
        m_currMoveDir = -1;

        m_currSheetIndexF32 -= moveSpeed;
        if(m_currSheetIndexF32 <= goalSheetIndexF32)
        {
          this->changeState(STATE_STAY);
        }
      }

      // 到達しているか
      if(m_state == STATE_STAY)
      {
        m_currSheetIndexF32 = goalSheetIndexF32;
        m_currSheetIndex    = m_goalSheetIndex;
      }
      else
      {
        // 整数版をつくっておく
        m_currSheetIndex = static_cast<s32>( m_currSheetIndexF32 + 0.5f );
      }
      break;
    }
  }

  //////////////////////////////////
  // シートの更新
  //////////////////////////////////
  this->initSheet(prevSheetIndex);  // 画面に見えているシートが変わっていたら、新シートを初期状態で表示する
  
  for(u32 i=0; i<LYT_WK_PTR_NUM; ++i)
  {
    if( m_sheetInfo[i].GetSheetIndex() != SHEET_INDEX_NONE )
    {
      m_sheetInfo[i].SetPosY( this->calcDistFromCenter( m_sheetInfo[i].GetSheetIndex() ) );
    }

    m_sheetInfo[i].UpdateSheet();
  }

  //////////////////////////////////
  // 背景の更新
  //////////////////////////////////
  {
    if(0<=m_currSheetIndex && m_currSheetIndex<m_sheetNum)
    {
      ZukanPicScrollDataAccessor::PokeData pokeData;
      m_pPicScrollDataAccessor->GetPokeData(m_placeGroupTopAlolaNoList[m_currSheetIndex], &pokeData);
      m_bgInfo->Req(pokeData.gradeVal);
    }
    m_bgInfo->Update();
  }

#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
  //////////////////////////////////
  // キラカードの更新
  //////////////////////////////////
  {
    if( prevSheetIndex != m_currSheetIndex )
    {
      m_pPrismSheet->Req(ZukanPrismSheet::REQ_ID_PRISM_OFF);
    }
    else
    {
      bool prismOn = false;
      if(0<=m_currSheetIndex && m_currSheetIndex<m_sheetNum)
      {
        ZukanPicScrollDataAccessor::PokeData pokeData;
        m_pPicScrollDataAccessor->GetPokeData(m_placeGroupTopAlolaNoList[m_currSheetIndex], &pokeData);
        if(pokeData.gradeVal ==  ZukanPicScrollDataAccessor::GRADE_VAL_LEGEND)
        {
          u32 pokeNum = ZukanPicScrollDataAccessor::PLACE_POS_NUM[ pokeData.placeType ];
          bool compFlag = true;
          for(u32 i=0; i<pokeNum; ++i)
          {
            u32 alolaNo = m_placeGroupTopAlolaNoList[m_currSheetIndex] + i;
            const ZukanContents::PokeData* contentsPokeData = m_pContents->GetDispPokeData(m_pContents->GetIndexOfDispPokeDataIndexForAlolaDefault(alolaNo));
            if(contentsPokeData->getFlag == 0)
            {
              compFlag = false;
              break;
            }
          }
          if(compFlag) prismOn = true;
        }
      }
      if(prismOn)
      {
        m_pPrismSheet->Req(ZukanPrismSheet::REQ_ID_PRISM_ON);
      }
      else
      {
        m_pPrismSheet->Req(ZukanPrismSheet::REQ_ID_PRISM_OFF);
      }
    }
  }
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
}
 
void ZukanPicScrollBook::SetSheet(u32 alolaNo, bool immediateFlag)
{
  m_goalSheetIndex = this->getSheetIndex(alolaNo);

  // 静止中に変更されたか？
  if(m_state == STATE_STAY)
  {
    if(m_goalSheetIndex != m_currSheetIndex)
    {
      this->changeState(STATE_MOVE);
    }
  }

  // 直ちに表示したいとき
  if(immediateFlag)
  {
    if(m_state == STATE_MOVE)
    {
      s32 prevSheetIndex = m_currSheetIndex;
      this->changeState(STATE_STAY);
      m_currSheetIndexF32 = static_cast<f32>(m_goalSheetIndex);
      m_currSheetIndex    = m_goalSheetIndex;
      this->initSheet(prevSheetIndex);
    }
  }
}

void ZukanPicScrollBook::setup(app::util::Heap* pAppHeap)
{
  m_sheetNum    = m_pPicScrollDataAccessor->GetPlaceGroupNum();
  m_alolaNo_end = m_pPicScrollDataAccessor->GetAlolaNoEndInPicScrollData();

  m_sheetLytWkPtrList        = GFL_NEW_ARRAY(pAppHeap->GetDeviceHeap()) u8[m_sheetNum];
  m_placeGroupTopAlolaNoList = GFL_NEW_ARRAY(pAppHeap->GetDeviceHeap()) u32[m_sheetNum];
  m_sheetIndexList           = GFL_NEW_ARRAY(pAppHeap->GetDeviceHeap()) s32[m_alolaNo_end+1];
  m_sheetInfo                = GFL_NEW_ARRAY(pAppHeap->GetDeviceHeap()) ZukanPicScrollSheetInfo[LYT_WK_PTR_NUM];

  m_bgInfo                   = GFL_NEW(pAppHeap->GetDeviceHeap()) ZukanPicScrollBgInfo;

  for(s32 i=0; i<m_sheetNum; ++i)
  {
    m_sheetLytWkPtrList[i] = i%LYT_WK_PTR_NUM;
    m_placeGroupTopAlolaNoList[i] = 0;  // 取り敢えず0で初期化
  }
  
  m_sheetIndexList[0] = 0;
  for(u32 alolaNo=1; alolaNo<=m_alolaNo_end; ++alolaNo)
  {
    u32 placeGroupNo = m_pPicScrollDataAccessor->GetPlaceGroupNo(alolaNo);
    if( 0<=placeGroupNo && placeGroupNo<(static_cast<u32>(m_sheetNum)) )  // placeGroupNoはなしのときZukanPicScrollDataAccessor::PLACE_GROUP_NO_NONEが返ってくるので注意すること！
    {
      m_sheetIndexList[alolaNo] = placeGroupNo;
      if(m_placeGroupTopAlolaNoList[placeGroupNo] == 0) m_placeGroupTopAlolaNoList[placeGroupNo] = alolaNo;
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
    }
  }

#if PM_DEBUG
  // 確認
  {
    for(s32 i=0; i<m_sheetNum; ++i)
    {
      GFL_ASSERT(m_placeGroupTopAlolaNoList[i] != 0);  // 開発中に気付かせるためのASSERT。値が入っていないところがある。
    }
  }
#endif

  for(u32 i=0; i<LYT_WK_PTR_NUM; ++i)
  {
    m_sheetInfo[i].Setup(
      pAppHeap,
      m_pG2DUtil,
      m_lytRes,
      m_pLytWk[i],
      m_lytWkId[i],
      m_pPicScrollDataAccessor,
      m_pPokeExtendData,
      m_pMsgData,
      m_pWordSet,
      m_pContents,
      m_pPokePicManager
    );
  }

  m_bgInfo->Setup(
      pAppHeap,
      m_pG2DUtil,
      m_pCurtainLytWk,
      m_curtainLytWkId,
      m_pBackmostLytWk,
      m_backmostLytWkId
  );

  // 初期化  // SetSheetで{m_state, m_stateSeq, m_goalSheetIndex, m_currSheetIndexF32, m_currSheetIndex}がうまく初期化されるような設定にしておく
  this->changeState(STATE_MOVE);
  m_currSheetIndex = SHEET_INDEX_NONE;
  this->SetSheet(1, true);  // 後から外から位置を渡す
}

s32 ZukanPicScrollBook::getSheetIndex(u32 alolaNo) const  // なしのときはZukanPicScrollBook::SHEET_INDEX_NONE
{
  if( 1<=alolaNo && alolaNo<=m_alolaNo_end )
  {
    return m_sheetIndexList[alolaNo];
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。
    return SHEET_INDEX_NONE;
  }
}

f32 ZukanPicScrollBook::calcDistFromCenter(s32 sheetIndex) const
{
  static const f32 DIST_BETWEEN_SHEET = 240.0f;
  f32 dist = ( m_currSheetIndexF32 - (static_cast<f32>(sheetIndex)) ) * DIST_BETWEEN_SHEET;
  return dist;
}

// 初期化する必要があるとき、シートを初期化し初期状態の表示をする
void ZukanPicScrollBook::initSheet(s32 /*prevSheetIndex*/)  // 前の画面中央のシートのインデックス
{
  // @fix NMCat[2712] アローラ図鑑でページ送りすると一瞬だけ上画面が乱れることがある
  //                  →ポケモン画像を１フレ遅れ解放するようにした。

  bool useLytWk[LYT_WK_PTR_NUM];
  for(u32 j=0; j<LYT_WK_PTR_NUM; ++j) useLytWk[j] = false;

  // これから表示したいシートインフォのインデックス
  for(s32 i=m_currSheetIndex-1; i<=m_currSheetIndex+1; ++i)  // LYT_WK_PTR_NUM個でループ
  {
    if(0<=i && i<m_sheetNum)
    {
      // これからiを表示したい
      s32 lytWkIndex = m_sheetLytWkPtrList[i];
      useLytWk[lytWkIndex] = true;  // このLytWkは使用する
      if( m_sheetInfo[lytWkIndex].GetSheetIndex() == i )  // iは連続した３値を取る。連続した３値iのm_sheetLytWkPtrList[i]は必ず異なる値になるようにつくってある。
      {
        // iが設定されているのでそのまま使う
        if( m_sheetInfo[lytWkIndex].GetState() != ZukanPicScrollSheetInfo::STATE_INIT )
        {
          // まだ開始していないので、Initを呼んで開始させる
          m_sheetInfo[lytWkIndex].InitSheet(i, m_placeGroupTopAlolaNoList[i]);
        }
      }
      else
      {
        // iではないものが設定されている
        if( m_sheetInfo[lytWkIndex].GetState() == ZukanPicScrollSheetInfo::STATE_INIT )
        {
          // まだ終了していないので、Termを呼んで終了させる
          m_sheetInfo[lytWkIndex].TermSheet();
        }
        else
        {
          // 既に終了しているので、iを開始する
          m_sheetInfo[lytWkIndex].InitSheet(i, m_placeGroupTopAlolaNoList[i]);
        }
      }
    }
  }

  // 未使用となったLytWk
  for(u32 j=0; j<LYT_WK_PTR_NUM; ++j)
  {
    if(useLytWk[j] == false)
    {
      // 未使用にしておく
      m_sheetInfo[j].UnuseSheet();
    }
  }
}

void ZukanPicScrollBook::changeState(State state)
{
  m_state = state;
  m_stateSeq = 0;
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


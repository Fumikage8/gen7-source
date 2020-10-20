//==============================================================================
/**
 @file    ZukanFrame.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.10
 @brief   図鑑のフレーム
 */
//==============================================================================


// poke_libのインクルード
#include <pml/include/pmlib.h>

// nijiのインクルード
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <Savedata/include/ZukanSave.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Ui/UIView.h>

// 図鑑登録の呼び出しEventのインクルード
#include <App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h>

// 図鑑登録チェッカーのインクルード
#include <PokeTool/include/PokeToolPokeGet.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProcParam.h>
#include "ZukanDefine.h"
#include "ZukanType.h"
#include "ZukanDataManager.h"
#include "ZukanTool.h"
#include "ZukanContents.h"
#include "ZukanPokePicManager.h"

#include "ZukanFrameLaunch.h"
#include "ZukanFrameAlolaTop.h"
#include "ZukanFrameIslandTop.h"
#include "ZukanFrameInfo.h"
#include "ZukanFrameRegisterIntro.h"
#include "ZukanFrameRegisterAlolaTop.h"
#include "ZukanFrameRegister.h"
#include "ZukanFrameMap.h"
//#include "ZukanFrameDiscover.h"
#include "ZukanFrameSearch.h"
#include "ZukanFramePokeName.h"
#include "ZukanFramePokePut.h"

#include "ZukanFrame.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
//
//  ZukanFrameListener
//
//==============================================================================
ZukanFrameListener::ZukanFrameListener(void)
  : applib::frame::AppFrameListener()
{}
ZukanFrameListener::~ZukanFrameListener()
{}

void ZukanFrameListener::EndFuncFinish( applib::frame::CellSuper* /*pEndCell*/ )
{}




//==============================================================================
//
//  ZukanFrameCell
//
//==============================================================================

//
// メモ
//
// UIViewの入力可不可制御、UIViewの表示制御はFrameCellで行う。
// フェードはUIViewで行う。
// 3D表示制御はUIViewで行う。
//

ZukanFrameCell::ZukanFrameCell(void)
  : applib::frame::CellSuper(),
    m_param(),
    m_seqCount(0),
    //m_contents(NULL),
    m_sceneParam(),
    m_prevSceneType(ZukanType::SCENE_TYPE_NONE),
    m_currSceneType(ZukanType::SCENE_TYPE_NONE),
    m_nextSceneType(ZukanType::SCENE_TYPE_NONE),
    m_launchSceneCursorIndex(0),
    m_alolaTopSceneChangeFlag(false),
    m_alolaTopScenePaneListSetupParamListPos(0),
    m_alolaTopScenePaneListSetupParamListScroll(0.0f),
    m_launchAndAlolaTopSceneDefaultFlag(false)
{}
ZukanFrameCell::~ZukanFrameCell()
{}

applib::frame::Result ZukanFrameCell::InitFunc(void)
{
  enum
  {
    SEQ_CREATE_CONTENTS       = 0,
    SEQ_SET_SCENE_PARAM          ,
    SEQ_END                      ,
  };

  applib::frame::Result res = applib::frame::RES_CONTINUE;
  
  switch( this->GetSubSeq() )  // 親であるapplib::frame::CellSuperのメンバ
  {
  case SEQ_CREATE_CONTENTS:
    {
      /*
      m_contents = GFL_NEW(m_param.contentsDevMem) ZukanContents(m_param.tool->GetPokeModelSys(), m_param.tool->GetPokeExtendData(), m_param.tool->GetPokePersonalSort());

#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_PRINT_POKE_MODEL_ALL
      m_contents->DebugPrintPokeModelAll();
#endif

      if( m_param.param->caller == ZukanProcParam::CALLER_ZUKAN_REGISTER_EVENT )
      {
        if( PokeTool::PokeGetUtil::DoesSetFlag(m_param.param->zukanRegisterEventParam->pp, m_param.param->zukanRegisterEventParam->callerType) )
        {
          PokeTool::PokeGetUtil::SetFlag(m_param.param->zukanRegisterEventParam->pp);
        }
        m_contents->CreateDataForRegister(m_param.contentsDevMem, m_param.param->zukanRegisterEventParam->pp);
      }
      else if( m_param.param->caller == ZukanProcParam::CALLER_ZUKAN_REGISTER_EVENT_AFTER_POKE_LIST )
      {
        m_contents->CreateDataForRegister(m_param.contentsDevMem, m_param.param->zukanRegisterEventParam->pp);
      }
      else if( m_param.param->caller == ZukanProcParam::CALLER_ZUKAN_REGISTER_EVENT_AFTER_POKE_STATUS )
      {
        m_contents->CreateDataForRegister(m_param.contentsDevMem, m_param.param->zukanRegisterEventParam->pp);
      }
      else  // if(m_param.param->caller == ZukanProcParam::CALLER_ZUKAN_EVENT)
      {
        m_contents->CreateData(m_param.contentsDevMem);
      }
      */

      this->changeSeq(SEQ_SET_SCENE_PARAM);
      //breakしない
    }
  case SEQ_SET_SCENE_PARAM:
    {
      m_sceneParam.param                                     = m_param.param;
      m_sceneParam.appHeap                                   = m_param.instanceAppHeap;
      m_sceneParam.gameManager                               = m_param.gameManager;
      m_sceneParam.dataManager                               = m_param.dataManager;
      m_sceneParam.frameManager                              = m_param.frameManager;
      m_sceneParam.appRenderingManager                       = m_param.appRenderingManager;
      m_sceneParam.lytSys                                    = m_param.lytSys;
      m_sceneParam.pokePicManager                            = m_param.pokePicManager;
      m_sceneParam.tool                                      = m_param.tool;
      m_sceneParam.contents                                  = m_param.contents;
      m_sceneParam.rootView                                  = m_param.rootView;                       
      m_sceneParam.parentView                                = m_param.rootView;                       
      m_sceneParam.frameCell                                 = NULL;
      m_sceneParam.setupState                                = m_param.setupState;
      m_sceneParam.prevSceneType                             = ZukanType::SCENE_TYPE_NONE;
      m_sceneParam.nextSceneType                             = &m_nextSceneType;
      m_sceneParam.launchSceneCursorIndex                    = &m_launchSceneCursorIndex;
      m_sceneParam.alolaTopSceneChangeFlag                   = &m_alolaTopSceneChangeFlag;
      m_sceneParam.alolaTopScenePaneListSetupParamListPos    = &m_alolaTopScenePaneListSetupParamListPos;
      m_sceneParam.alolaTopScenePaneListSetupParamListScroll = &m_alolaTopScenePaneListSetupParamListScroll;
      m_sceneParam.launchAndAlolaTopSceneDefaultFlag         = &m_launchAndAlolaTopSceneDefaultFlag;
      m_sceneParam.pRotomToneGenerator                       = m_param.pRotomToneGenerator;
#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE
      m_sceneParam.debugInfo                                 = m_param.debugInfo;
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE

      if(m_param.param->caller == ZukanProcParam::CALLER_ZUKAN_REGISTER_EVENT)
      {
        m_prevSceneType = ZukanType::SCENE_TYPE_START_ZUKAN_REGISTER;
        m_currSceneType = ZukanType::SCENE_TYPE_ZUKAN_REGISTER_CHECKER;
      }
      else if(m_param.param->caller == ZukanProcParam::CALLER_ZUKAN_REGISTER_EVENT_AFTER_POKE_LIST)
      {
        m_prevSceneType = ZukanType::SCENE_TYPE_START_ZUKAN_REGISTER_AFTER_POKE_LIST;
        m_currSceneType = ZukanType::SCENE_TYPE_POKE_PUT;
      }
      else if(m_param.param->caller == ZukanProcParam::CALLER_ZUKAN_REGISTER_EVENT_AFTER_POKE_STATUS)
      {
        m_prevSceneType = ZukanType::SCENE_TYPE_START_ZUKAN_REGISTER_AFTER_POKE_STATUS;
        m_currSceneType = ZukanType::SCENE_TYPE_POKE_PUT;
      }
      else  // if(m_param.param->caller == ZukanProcParam::CALLER_ZUKAN_EVENT)
      {
        m_prevSceneType = ZukanType::SCENE_TYPE_START_ZUKAN;
        m_currSceneType = ZukanType::SCENE_TYPE_LAUNCH;
      }

      this->changeSeq(SEQ_END);
      //breakしない
    }
  case SEQ_END:
  default:
    {
      res = applib::frame::RES_FINISH;
      break;
    }
  }

  return res;
}
applib::frame::Result ZukanFrameCell::UpdateFunc(void)
{
  applib::frame::Result res = applib::frame::RES_CONTINUE;

  if(m_param.param->caller == ZukanProcParam::CALLER_ZUKAN_REGISTER_EVENT)
  {
    res = this->updateFuncForCallerZukanRegisterEvent();
  }
  else if(m_param.param->caller == ZukanProcParam::CALLER_ZUKAN_REGISTER_EVENT_AFTER_POKE_LIST)
  {
    res = this->updateFuncForCallerZukanRegisterEventAfterPokeListStatus();
  }
  else if(m_param.param->caller == ZukanProcParam::CALLER_ZUKAN_REGISTER_EVENT_AFTER_POKE_STATUS)
  {
    res = this->updateFuncForCallerZukanRegisterEventAfterPokeListStatus();
  }
  else  // if(m_param.param->caller == ZukanProcParam::CALLER_ZUKAN_EVENT)
  {
    res = this->updateFuncForCallerZukanEvent();
  }

  return res;
}
void ZukanFrameCell::DrawFunc(gfl2::gfx::CtrDisplayNo no)
{}
applib::frame::Result ZukanFrameCell::EndFunc(void)
{
  enum
  {
    SEQ_DESTROY_CONTENTS      = 0,
    SEQ_END                      ,
  };

  applib::frame::Result res = applib::frame::RES_CONTINUE;
  
  switch( this->GetSubSeq() )  // 親であるapplib::frame::CellSuperのメンバ
  {
  case SEQ_DESTROY_CONTENTS:
    {
      /*
      GFL_SAFE_DELETE(m_contents);
      */
      this->changeSeq(SEQ_END);
      //breakしない
    }
  case SEQ_END:
  default:
    {
      res = applib::frame::RES_FINISH;
    }
  }

  return res;
}

void ZukanFrameCell::Setup(const Param& param)
{
  m_param = param;
}

void ZukanFrameCell::changeSeq(int seq)
{
  this->SetSubSeq(seq);  // 親であるapplib::frame::CellSuperのメンバ
  m_seqCount = 0;
}

applib::frame::Result ZukanFrameCell::updateFuncForCallerZukanRegisterEvent(void)
{
  applib::frame::Result res = applib::frame::RES_CONTINUE;
  
  Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
 
  s32 loopCount = 1;
  while(loopCount > 0)
  {
    --loopCount;

    switch(m_currSceneType)
    {
    case ZukanType::SCENE_TYPE_ZUKAN_REGISTER_CHECKER:
      {
        // 最初に図鑑登録チェッカーを調べ、falseが返ってきたら何もしないことにする。
        if(
            PokeTool::PokeGetUtil::Check(
                m_param.param->zukanRegisterEventParam->pp,
                m_param.param->zukanRegisterEventParam->callerType
            ) == false
        )
        {
          GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。ZukanProcを呼び出す前に、図鑑登録チェッカーで呼び出しが必要か否か判定して下さい。今回は呼び出しが必要ないはずです。
          // 【重要メモ】
          //       このチェックは万全ではない。万全でない例は以下の通り。
          //       (1) 呼び出し元でCheckがfalseだったので、"配置&フラグ処理"を済ませる。
          //           【詳細例：手持ちが5体だったので、手持ちに1体追加できる。だからこの時点ではfalseと判定される】
          //       (2) その後、誤ってZukanProcが呼び出される。
          //       (3) ZukanProcでのCheckがtrueになることがある。同じポケモンに対してもう一度"配置&フラグ処理"を行うことになり、ポケモンが増殖する。
          //           【詳細例：手持ちが6体になっているので、手持ちかボックスか送り先選択画面で選ぶ必要がある、と判定される。だからここではtrueと判定される】
          //       これを防ぐには、呼び出し元で"配置&フラグ処理"を済ませたときは、ZukanProcを呼び出さないように気を付けてもらうしかない。
          //       これを防ぐために、二重呼び出しを発見する機能をつくり、PM_DEBUG時に有効にしておく。
          //
          //       また、一度も"配置&フラグ処理"を行わないというバグもあるかもしれないので、それを発見する機能もつくり、PM_DEBUG時に有効にしておく。
 
          m_currSceneType = ZukanType::SCENE_TYPE_END;
        }
        else
        {
          if( PokeTool::PokeGetUtil::DoesGoToInfoShow(
                  m_param.param->zukanRegisterEventParam->pp,
                  m_param.param->zukanRegisterEventParam->callerType ) )
          {
            // 演出ZukanFrameRegisterAlolaTopCellを通るルート。
            m_currSceneType = ZukanType::SCENE_TYPE_REGISTER_INTRO;
          }
          else if( PokeTool::PokeGetUtil::DoesGoToPokeNameInput(
                       m_param.param->zukanRegisterEventParam->pp,
                       m_param.param->zukanRegisterEventParam->callerType ) )
          {
            m_currSceneType = ZukanType::SCENE_TYPE_POKE_NAME;

            if( !m_param.param->zukanRegisterEventParam->pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) )  // @fix NMCat[3711] 手持ちが6体の状態でタマゴを受け取ると、うまれたポケモンの図鑑登録が発生しない
                                                                                                       //      →タマゴは図鑑登録画面を呼んだことにしない
                                                                                                       //      ←タマゴにニックネームを付けることはないのでここにタマゴで来ることはないはずだが、念のため書いておく。
            {
              // @fix NMCat[3174] 進化による図鑑登録演出時、「アシマリ」が表示されない 
              //      →演出ZukanFrameRegisterAlolaTopCellを通らないルートなので、ここで図鑑登録画面を呼んだということを知らせておく。
              svZukanData->SetPokeRegisterShow( m_param.param->zukanRegisterEventParam->pp->GetMonsNo() );  // 図鑑登録画面を呼んだ
            }
          }
          else if( PokeTool::PokeGetUtil::DoesGoToPokePutSelect(
                       m_param.param->zukanRegisterEventParam->pp,
                       m_param.param->zukanRegisterEventParam->callerType ) )
          {
            m_currSceneType = ZukanType::SCENE_TYPE_POKE_PUT;
            
            if( !m_param.param->zukanRegisterEventParam->pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) )  // @fix NMCat[3711] 手持ちが6体の状態でタマゴを受け取ると、うまれたポケモンの図鑑登録が発生しない
                                                                                                       //      →タマゴは図鑑登録画面を呼んだことにしない
                                                                                                       //      ←タマゴを配置することはあるのでここにタマゴで来ることはある。
            {
              // @fix NMCat[3174] 進化による図鑑登録演出時、「アシマリ」が表示されない 
              //      →演出ZukanFrameRegisterAlolaTopCellを通らないルートなので、ここで図鑑登録画面を呼んだということを知らせておく。
              svZukanData->SetPokeRegisterShow( m_param.param->zukanRegisterEventParam->pp->GetMonsNo() );  // 図鑑登録画面を呼んだ
            }
          }
          else
          {
            GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。図鑑登録チェッカーで呼び出しが必要と判定されたのに、何もせずに終了してしまった。どこか判定が間違っている可能性あり。
          
            m_currSceneType = ZukanType::SCENE_TYPE_END;
          }
        }
        m_nextSceneType = ZukanType::SCENE_TYPE_NONE;
        loopCount = 1;
        break;
      }
    case ZukanType::SCENE_TYPE_REGISTER_INTRO:
      {
        this->callFrameScene<ZukanFrameRegisterIntroCell>();
        break;
      }
    case ZukanType::SCENE_TYPE_REGISTER_ALOLA_TOP:
      {
        this->callFrameScene<ZukanFrameRegisterAlolaTopCell>();
        break;
      }
    case ZukanType::SCENE_TYPE_REGISTER:
      {
        this->callFrameScene<ZukanFrameRegisterCell>();
        break;
      }
    case ZukanType::SCENE_TYPE_POKE_NAME:
      {
        this->callFrameScene<ZukanFramePokeNameCell>();
        break;
      }
    case ZukanType::SCENE_TYPE_POKE_PUT:
      {
        this->callFrameScene<ZukanFramePokePutCell>();
        break;
      }
    case ZukanType::SCENE_TYPE_WAIT_NEXT:
      {
        m_currSceneType = m_nextSceneType;
        m_nextSceneType = ZukanType::SCENE_TYPE_NONE;
        loopCount = 1;
        break;
      }
    case ZukanType::SCENE_TYPE_NEXT_POKE_LIST:
      {
        m_param.param->returnType = ZukanProcParam::RETURN_TYPE_NEXT_POKE_LIST;
        res = applib::frame::RES_FINISH;
        break;
      }
    case ZukanType::SCENE_TYPE_NEXT_POKE_STATUS:
      {
        m_param.param->returnType = ZukanProcParam::RETURN_TYPE_NEXT_POKE_STATUS;
        res = applib::frame::RES_FINISH;
        break;
      }
    case ZukanType::SCENE_TYPE_END:
    default:
      {
        m_param.param->returnType = ZukanProcParam::RETURN_TYPE_END;
        res = applib::frame::RES_FINISH;
        break;
      }
    }
  }

  return res;
}

applib::frame::Result ZukanFrameCell::updateFuncForCallerZukanRegisterEventAfterPokeListStatus(void)
{
  applib::frame::Result res = applib::frame::RES_CONTINUE;

  s32 loopCount = 1;
  while(loopCount > 0)
  {
    --loopCount;
    
    switch(m_currSceneType)
    {
    case ZukanType::SCENE_TYPE_POKE_PUT:
      {
        this->callFrameScene<ZukanFramePokePutCell>();
        break;
      }
    case ZukanType::SCENE_TYPE_WAIT_NEXT:
      {
        m_currSceneType = m_nextSceneType;
        m_nextSceneType = ZukanType::SCENE_TYPE_NONE;
        loopCount = 1;
        break;
      }
    case ZukanType::SCENE_TYPE_NEXT_POKE_LIST:
      {
        m_param.param->returnType = ZukanProcParam::RETURN_TYPE_NEXT_POKE_LIST;
        res = applib::frame::RES_FINISH;
        break;
      }
    case ZukanType::SCENE_TYPE_NEXT_POKE_STATUS:
      {
        m_param.param->returnType = ZukanProcParam::RETURN_TYPE_NEXT_POKE_STATUS;
        res = applib::frame::RES_FINISH;
        break;
      }
    case ZukanType::SCENE_TYPE_END:
    default:
      {
        m_param.param->returnType = ZukanProcParam::RETURN_TYPE_END;
        res = applib::frame::RES_FINISH;
        break;
      }
    }
  }

  return res;
}

applib::frame::Result ZukanFrameCell::updateFuncForCallerZukanEvent(void)
{
  applib::frame::Result res = applib::frame::RES_CONTINUE;
 
  s32 loopCount = 1;
  while(loopCount > 0)
  {
    --loopCount;

    switch(m_currSceneType)
    {
    case ZukanType::SCENE_TYPE_LAUNCH:
      {
        this->callFrameScene<ZukanFrameLaunchCell>();
        break;
      }
    case ZukanType::SCENE_TYPE_ALOLA_TOP:
      {
        this->callFrameScene<ZukanFrameAlolaTopCell>();
        break;
      }
    case ZukanType::SCENE_TYPE_ISLAND_TOP:
      {
        this->callFrameScene<ZukanFrameIslandTopCell>();
        break;
      }
    case ZukanType::SCENE_TYPE_INFO:
      {
        this->callFrameScene<ZukanFrameInfoCell>();
        break;
      }
    case ZukanType::SCENE_TYPE_MAP:
      {
        this->callFrameScene<ZukanFrameMapCell>();
        break;
      }
    //case ZukanType::SCENE_TYPE_DISCOVER:
    //  {
    //    this->callFrameScene<ZukanFrameDiscoverCell>();
    //    break;
    //  }
    case ZukanType::SCENE_TYPE_SEARCH:
      {
        this->callFrameScene<ZukanFrameSearchCell>();
        break;
      }
    case ZukanType::SCENE_TYPE_WAIT_NEXT:
      {
        m_currSceneType = m_nextSceneType;
        m_nextSceneType = ZukanType::SCENE_TYPE_NONE;
        loopCount = 1;
        break;
      }
    case ZukanType::SCENE_TYPE_END:
    default:
      {
        m_param.param->returnType = ZukanProcParam::RETURN_TYPE_END;
        res = applib::frame::RES_FINISH;
        break;
      }
    }
  }
  
  return res;
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


//======================================================================
/**
 * @file  btlv_seq_sys.h
 * @brief バトル描画シーケンスシステム
 * @author  ariizumi
 * @data  11/08/11
 */
//======================================================================
#ifndef __BTLV_SEQ_SYS_H__
#define __BTLV_SEQ_SYS_H__

#include "DemoLib/SeqEditor/include/SeqSystem.h"

namespace btl
{

class BattleViewSystem;


class BtlvSeqSystem : public DemoLib::SeqEditor::SeqFileCallback
{
public:
  BtlvSeqSystem(BattleViewSystem *core);
  virtual ~BtlvSeqSystem();
  
  void Update(void);
  
  void DebugLoadPath( char *path );
  void LoadSeqFile( int arcId  );
  void LoadQuickHit(void);
  
  bool IsFinishAll(void){return (mSeqSys->GetActiveScriptNum() == 0);}
  
  int GetSeqFrame(void);
  
  void CheckWazaState(void);

  virtual void CommnadCallback( DemoLib::SeqEditor::SeqFile *seqFile , const DemoLib::SeqEditor::CommandParam &param );
  
  void SetPause( bool flg ){mIsPause = flg;}
  bool GetIsPause( void ){return mIsPause;}
  
private:
  //攻撃モーションの再生タイミングの計算
  void CheckAttackMotionTimming( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void CheckSpecialChainAttakDefine( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  
  //各種先読み
  void PreLoad_WazaModel( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_BallModel( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  //こっちはアニメも一緒に読む
  void PreLoad_WazaModel_Cluster( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_WazaModelAnime( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_WazaEffect( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_WazaEffectBall( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_WazaEffectLnad( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_RareEffect( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_WazaCamera( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_WazaCameraTrainer( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_WazaCameraDoubleIntro( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_WazaOuterMotion( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_WazaOuterMotionTiki( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_WazaKisekaeMotion( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_WazaTexture( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_WazaTrainerTexture( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_WazaTrainerLight( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_WazaTrainerLightFile( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_Weather( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
 
  BattleViewSystem* mBtlvCore;

  DemoLib::SeqEditor::SeqSystem *mSeqSys;
  DemoLib::SeqEditor::SeqFile   *mSeqFile;
  
  bool mIsPause;
  
};

}


#endif //__BTLV_SEQ_SYS_H__



//======================================================================
/**
 * @file	SequencePlayer.h
 * @brief	シーケンスエディタで作成したデモファイルを再生する機構
 * @author	PETE
 * @data	2015.04.24
 */
//======================================================================
#if !defined( __SEQUENCE_PLAYER_H__ )
#define __SEQUENCE_PLAYER_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include "Demolib/SeqEditor/include/SeqFile.h"
#include "Demolib/SeqEditor/include/SeqSystem.h"
#include "Demolib/SequencePlayer/include/SequenceCommand.h"
#include "Demolib/SequencePlayer/include/SequencePlayerCore.h"

GFL_NAMESPACE_BEGIN(DemoLib)
	GFL_NAMESPACE_BEGIN(Sequence)

class Player : public DemoLib::SeqEditor::SeqFileCallback
{
public:

	Player(SequenceViewSystem *core);
	~Player();

  void CommandCallbackPreprocess( DemoLib::SeqEditor::SeqFile *seqFile , const DemoLib::SeqEditor::CommandParam &param );	
	void CommnadCallback( DemoLib::SeqEditor::SeqFile *seqFile , const DemoLib::SeqEditor::CommandParam &param );	

	void Update(int step=1);

	void DebugLoadPath( char *path );
	void LoadSeqFile( int dataID  );

	// Accessors:
	bool IsFinishAll(void) { return (mSeqSys->GetActiveScriptNum() == 0); }
	void SetPause( bool flg ) { mPause = flg; }
	bool GetIsPause( void ) { return mPause; }
	int GetSeqFrame(void);
  int GetMaxSeqFrame(void);

	void SetSeqFrame(int n);

  void PreprocessCount(void);
	void PreloadAll(void);

  int GetPokemonCount(void) { return(mPokemonCount); }

  bool UseInframe1(void) { return(mUseInframe1); }
  bool UseInframe2(void) { return(mUseInframe2); }
  bool UseWarpModelDraw(void) { return(mUseWarpModelDraw); }
  bool UsePokeColorShader(void) { return(mUsePokeColorShader); }
  bool UseMotionBlur(void) { return(mUseMotionBlur); }
  bool UseBloom(void) { return(mUseBloom); }

private:
  bool CanRunCommand(int optionType,bool isSkipMode);
  void ForceCreatePokemon(const DemoLib::SeqEditor::CommandParam &param);

  void PreLoad_PokemonModelAndAnimationData( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_PokemonFromParameterData( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_PokemonAnimationData( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_PokemonAnimationDataMulti( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
	
  void PreLoad_CharaModelFieldData( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_CharaModelBattleData( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );

  void PreLoad_CharaModelFieldData_AttachItem( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_CharaModelBattleData_AttachItem( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );


  void PreLoad_CharaVSBattleData( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_CharaModelAnimation( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );

  void PreLoad_BloomMask( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );

  void PreLoad_Model( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
	void PreLoad_ModelAnimation( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_ModelAnimationMulti( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
	
  void PreLoad_CameraAnimation( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
	
  void PreLoad_Layout( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_Layout_MutiLanguage( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );

  void PreLoad_FieldLight( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_Light( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_LightAnimation( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  
  void PreLoad_Skybox( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );

  void PreLoad_ParticleEffect( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );
  void PreLoad_BallModel( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param );

	 DemoLib::SeqEditor::SeqSystem *mSeqSys;
	 DemoLib::SeqEditor::SeqFile *mSeqFile;
	 SequenceViewSystem *mSeqViewCore;
	 bool mPause;
   bool mPreprocess;
   int mPokemonCount;

   bool mUseInframe1,mUseInframe2;
   bool mUseWarpModelDraw;
   bool mUsePokeColorShader;
   bool mUseMotionBlur;
   bool mUseBloom;
};

	GFL_NAMESPACE_END(Sequence)
GFL_NAMESPACE_END(DemoLib)

#endif //__SEQUENCE_PLAYER_H__
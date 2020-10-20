//======================================================================
/**
 * @file	SequenceCommand.C++
 * @brief	シーケンコマンド自体
 * @author	PETE
 * @data	2015.04.27
 */
//======================================================================
#include <debug/DebugWin/include/DebugWinSystem.h>
#include "DemoLib/SequencePlayer/include/Player.h"
#include "DemoLib/SequencePlayer/include/SequencePlayerCore.h"
#include "DemoLib/SequencePlayer/include/SequencePlayerTask.h"
#include "DemoLib/SequencePlayer/include/SequenceObject.h"
#include <model/include/gfl2_CharaModel.h>

#include <DemoLib/Object/include/Object_Layout.h>

#include "Sound/include/Sound.h"

#include <Fade/include/gfl2_FadeManager.h>

#include <arc_def_index/arc_def.h>
#include <niji_conv_header/demoseq/seq_com_define.h>
#include <niji_conv_header/battle/chara/btpc0001_00_select4.h>

#include "DemoLib/Object/include/TaskSystem.h"

// Motion Blur:
#include "System/include/MotionBlur.h"
#include "System/include/GflUse.h"

// モデルのインデックス
#include <niji_conv_header/battle/chara/chara_model_id.h>

GFL_NAMESPACE_BEGIN(DemoLib)
	GFL_NAMESPACE_BEGIN(Sequence)

#ifdef USE_DEBUG
#define DUMP_COMMAND(str) GFL_PRINT(str);
#else
#define DUMP_COMMAND(str) //Dummy
#endif
	
#define SEQPLAYER_CMD_DEF(name) void SEQ_CMD_ ## name ( DemoLib::SeqEditor::SeqFile *file, void *userWork , const DemoLib::SeqEditor::CommandParam &param )

SEQPLAYER_CMD_DEF( LABEL_POKEMON ) { }

SEQPLAYER_CMD_DEF( POKEMON_CREATE ) 
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  int index,type,drawtype,shaderUse,internalAnimation,loadExternalMotion,externalMotion,isLoop;

  file->LoadDataInt(&index);
  ///  file->LoadDataInt(&preload);
  file->LoadDataInt(&type);
  file->LoadDataInt(&drawtype);
  file->LoadDataInt(&shaderUse);
  file->LoadDataInt(&internalAnimation);

  file->LoadDataInt(&loadExternalMotion);
  file->LoadDataInt(&externalMotion);

  file->LoadDataInt(&isLoop);

  // Frame Control: 
  const u32 frame = param.endFrame - core->GetTargetFrame();
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  
  GFL_PRINT("\t\t--->Index:%i  Type:%i  DrawType:%i  Shader:%i\n",index,type,drawtype,shaderUse);

  SequenceObject_Poke *poke = core->GetIndexPokemonAndRemove(index);

  if(poke == NULL)
  {
      GFL_PRINT("\t\t--->ERROR, POKEMON NOT LOADED: Index:%i\n",index);
      return;
  }

  poke->SetCreateFrame(param.startFrame);
  poke->Create(core->GetMemHeap(),core->GetGLAllocator(),true);
  poke->AddRenderPath(drawtype);
  poke->SetVisible(true);

  // Using Poke Inframe:
  switch(drawtype)
  {
    case 1: case 4: // Inframe 1
      core->Inframe_AttachPokemon(0,poke->GetPokeModel()); break;
    case 2: case 5: // Inframe 2
      core->Inframe_AttachPokemon(1,poke->GetPokeModel()); break;
  }
  

  if(core->GetTargetFrame()>=param.endFrame) 
  {
    // Too late!
    poke->SetVisible(false);
  }

  // Internal Motion:
  if(loadExternalMotion==0)
  {  
    poke->ChangeAnimationDirect(internalAnimation);
    poke->SetAnimationIsLoop( isLoop != 0);

    if(!isLoop)
    {
      poke->SetReserveAnimation(PokeTool::MODEL_ANIME_BT_WAIT_A);
    }
    poke->UpdateAnimation(frameDif);

  }
  // External Motion:
  else
  {    
    GFL_PRINT("\t\t--->Setting External Animation: %i\n",externalMotion);
    gfl2::renderingengine::scenegraph::resource::ResourceNode *anmRes = core->GetResourceManager()->GetDataBuf_Anime(ARCID_DEMOSEQUENCE_RESOURCE,externalMotion);
    poke->ChangeAnimationResource(anmRes);    
    poke->SetAnimationIsLoop(isLoop != 0);
    poke->UpdateAnimation(frameDif);
  }

  const u32 grpNo = param.groupNo;

  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  int objNum = 1;

  SequenceObject_Poke **objArr = GFL_NEW_ARRAY(heap)SequenceObject_Poke*[objNum];	
  core->GetPokeObjManager()->SetObjectArr(objArr,objNum,grpNo);

  for( int i=0;i<objNum;i++)
  {
    objArr[i] = poke;
    core->GetObjectManager()->AddObject(objArr[i]);    
    DemoLib::Obj::CreateTask_DeleteObject_Poke(core->GetTaskSystem(),heap,objArr[i],frame);
  }

}

SEQPLAYER_CMD_DEF( POKEMON_CREATEBYPARAM ) 
{  
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  int monsNo,formNo,sexType,type,drawtype,shaderUse,internalAnimation,loadExternalMotion,externalMotion,isLoop;

  file->LoadDataInt(&monsNo);
  file->LoadDataInt(&formNo);
  file->LoadDataInt(&sexType);
  
  file->LoadDataInt(&type);
  file->LoadDataInt(&drawtype);
  file->LoadDataInt(&shaderUse);
  file->LoadDataInt(&internalAnimation);

  file->LoadDataInt(&loadExternalMotion);
  file->LoadDataInt(&externalMotion);

  file->LoadDataInt(&isLoop);


  // Frame Control:
 
  const u32 frame = param.endFrame - core->GetTargetFrame();
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;
  
  GFL_PRINT("\t\t--->MonsParam:%i,%i,%i  Type:%i  DrawType:%i  Shader:%i\n",monsNo,formNo,sexType,type,drawtype,shaderUse);

  SequenceObject_Poke *poke = core->GetIndexPokemonAndRemove(DEMOSEQUENCE_MAX_POKEMON);

  

  if(poke == NULL)
  {
    GFL_PRINT("\t\t--->ERROR, POKEMON NOT LOADED: Index:%i\n",DEMOSEQUENCE_MAX_POKEMON);
    return;
  }

  poke->SetCreateFrame(param.startFrame);
  poke->Create(core->GetMemHeap(),core->GetGLAllocator(),true);
  poke->AddRenderPath(drawtype);
  poke->SetVisible(true);

  // Using Poke Inframe:
  switch(drawtype)
  {
    case 1: case 4: // Inframe 1
      core->Inframe_AttachPokemon(0,poke->GetPokeModel()); break;
    case 2: case 5: // Inframe 2
      core->Inframe_AttachPokemon(1,poke->GetPokeModel()); break;
  }

  if(core->GetTargetFrame()>=param.endFrame) 
  {
    poke->SetVisible(false);
    // Too Late!
  }

  // Internal Motion:
  if(loadExternalMotion==0)
  {  
    poke->ChangeAnimationDirect(internalAnimation);
    poke->SetAnimationIsLoop( isLoop != 0);
    
    if(!isLoop)
    {
      poke->SetReserveAnimation(PokeTool::MODEL_ANIME_BT_WAIT_A);
    }

    poke->UpdateAnimation(frameDif);

  }
  // External Motion:
  else
  {    
    GFL_PRINT("\t\t--->Setting External Animation: %i\n",externalMotion);
    gfl2::renderingengine::scenegraph::resource::ResourceNode *anmRes = core->GetResourceManager()->GetDataBuf_Anime(ARCID_DEMOSEQUENCE_RESOURCE,externalMotion);
    poke->ChangeAnimationResource(anmRes);    
    poke->SetAnimationIsLoop(isLoop != 0);
    poke->UpdateAnimation(frameDif);
  }
 
  const u32 grpNo = param.groupNo;

  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  int objNum = 1;

  SequenceObject_Poke **objArr = GFL_NEW_ARRAY(heap)SequenceObject_Poke*[objNum];	
  core->GetPokeObjManager()->SetObjectArr(objArr,objNum,grpNo);

  for( int i=0;i<objNum;i++)
  {
    objArr[i] = poke;
    core->GetObjectManager()->AddObject(objArr[i]);        
    DemoLib::Obj::CreateTask_DeleteObject_Poke(core->GetTaskSystem(),heap,objArr[i],frame);
  }

}

SEQPLAYER_CMD_DEF( POKEMON_MOVE_POSITION ) 
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  gfl2::math::Vector3 pos;
  int isRelative;

  file->LoadDataVec3( &pos );
  file->LoadDataInt( &isRelative );
 
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  int objNum;
  SequenceObject_Poke **objArr = core->GetPokeObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Poke *obj = objArr[i];
    if( isRelative != 0 )
    {
      pos += obj->GetPosition();
    }

    if( frame == 0 || frame<=frameDif)
    {
      obj->SetPosition(pos);
      obj->UpdatePosition();
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectMove(core->GetTaskSystem(),heap,obj,obj->GetPosition(),pos,frame);

      Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
      t->Update(frameDif);      
      obj->UpdatePosition();
    }
  }
}

SEQPLAYER_CMD_DEF( POKEMON_ROTATE ) 
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  gfl2::math::Vector3 rotBase;
  int isRelative;

  file->LoadDataVec3( &rotBase );
  file->LoadDataInt( &isRelative );

  const u32 frameDif = core->GetTargetFrame() - param.startFrame;


  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  int objNum;
  SequenceObject_Poke **objArr = core->GetPokeObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Poke *obj = objArr[i];

    gfl2::math::Vector3 rot = rotBase;
    if( isRelative != 0 )
    {
      rot += obj->GetRotate();
    }

    if( frame == 0 || frame<=frameDif)
    {
      obj->SetRotate(rot);
      obj->UpdatePosition();
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectRotate(core->GetTaskSystem(),heap,obj,obj->GetRotate(),rot,frame);

      Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
      t->Update(frameDif);      
      obj->UpdatePosition();
    }
  }
}

SEQPLAYER_CMD_DEF( POKEMON_SCALE ) 
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  gfl2::math::Vector3 sclBase;
  int isRelative;

  file->LoadDataVec3( &sclBase );
  file->LoadDataInt( &isRelative );

  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  int objNum;
  SequenceObject_Poke **objArr = core->GetPokeObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Poke *obj = objArr[i];

    gfl2::math::Vector3 scl = sclBase;
    if( isRelative != 0 )
    {
      scl = scl.MulPerElem(obj->GetScale());
    }

    GFL_ASSERT_MSG(scl.x!=0&&scl.y!=0&&scl.z!=0 , "スケールを0にしないでください！\n" );

    if( frame == 0 || frame<=frameDif)
    {
      obj->SetScale(scl);
      obj->UpdatePosition();

    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectScale(core->GetTaskSystem(),heap,obj,obj->GetScale(),scl,frame);

      Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
      t->Update(frameDif);      
      obj->UpdatePosition();
    }
  }
}

SEQPLAYER_CMD_DEF( POKEMON_SETVISIBLE ) 
{
  int isVisible;

  file->LoadDataInt( &isVisible );  
  
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  const u32 grpNo = param.groupNo;

  int objNum;
  SequenceObject_Poke **objArr = core->GetPokeObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Poke *poke = objArr[i];
    poke->SetVisible( isVisible != 0);
    poke->UpdatePosition();
  }
}

SEQPLAYER_CMD_DEF(POKEMON_SETVISIBLE_SHADOW)
{
  int isVisible;

  file->LoadDataInt(&isVisible);

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  const u32 grpNo = param.groupNo;

  int objNum;
  SequenceObject_Poke **objArr = core->GetPokeObjManager()->GetObjectArr(grpNo, &objNum);
  for (int i = 0; i < objNum; i++)
  {
    SequenceObject_Poke *poke = objArr[i];
    poke->SetShadowVisible(isVisible != 0);
    poke->UpdatePosition();
  }
}

SEQPLAYER_CMD_DEF( POKEMON_SHADER_COLOR )
{
  
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  const u32 frameDif = core->GetTargetFrame() - param.startFrame;
  
  gfl2::math::Vector3 sColTemp;
  f32 sAlpha;
  gfl2::math::Vector3 eColTemp;
  f32 eAlpha;

  gfl2::heap::HeapBase* heap = core->GetDevHeap();
  
  file->LoadDataVec3( &sColTemp );
  file->LoadDataF32( &sAlpha );
  file->LoadDataVec3( &eColTemp );
  file->LoadDataF32( &eAlpha );

  gfl2::math::Vector4 sCol( sColTemp.x,sColTemp.y,sColTemp.z,sAlpha);
  gfl2::math::Vector4 eCol( eColTemp.x,eColTemp.y,eColTemp.z,eAlpha);

  int objNum;
  SequenceObject_Poke **objArr = core->GetPokeObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Poke *poke = objArr[i];    
    DemoLib::Obj::CreateTask_ChangePokeColorShader(core->GetTaskSystem(),heap,poke,sCol,eCol,frame);    

    Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
    t->Update(frameDif);          
  }
    
  
}


SEQPLAYER_CMD_DEF( POKEMON_SETANIMATION ) 
{
  int animeType,isLoop;

  file->LoadDataInt( &animeType );
  file->LoadDataInt( &isLoop );


  GFL_PRINT("\t\t--->Setting Animation: %i\n",animeType);

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  const u32 grpNo = param.groupNo;

  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  int objNum;
  SequenceObject_Poke **objArr = core->GetPokeObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Poke *poke = objArr[i];
    
    poke->SetCreateFrame(param.startFrame);
    poke->ChangeAnimationDirect(animeType);
    poke->SetAnimationIsLoop( isLoop != 0);
   
    if(!isLoop)
    {
      poke->SetReserveAnimation(PokeTool::MODEL_ANIME_BT_WAIT_A);
    }

    // Remove any SLOT1  Animation:
    poke->ChangeAnimationResource(NULL,1);    
    poke->SetAnimationIsLoop(false,1);

    poke->UpdateAnimation(frameDif);
  }

}

SEQPLAYER_CMD_DEF( POKEMON_SETANIMATIONEXTERNAL ) 
{
  int externalMotion,isLoop;

  file->LoadDataInt( &externalMotion );
  file->LoadDataInt( &isLoop );


  GFL_PRINT("\t\t--->Setting Animation: %i\n",externalMotion);

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  const u32 grpNo = param.groupNo;

  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  int objNum;
  SequenceObject_Poke **objArr = core->GetPokeObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Poke *poke = objArr[i];

    poke->SetCreateFrame(param.startFrame);
    gfl2::renderingengine::scenegraph::resource::ResourceNode *anmRes = core->GetResourceManager()->GetDataBuf_Anime(ARCID_DEMOSEQUENCE_RESOURCE,externalMotion);
    poke->ChangeAnimationResource(anmRes);    
    poke->SetAnimationIsLoop(isLoop != 0);
    
    // Remove any SLOT1  Animation:
    poke->ChangeAnimationResource(NULL,1);    
    poke->SetAnimationIsLoop(false,1);

    poke->UpdateAnimation(frameDif);
  }
}

SEQPLAYER_CMD_DEF( POKEMON_SETANIMATIONEXTERNAL_MULTI ) 
{
  int externalMotion,isLoop;
  int animeType,playExternalOnly;

  file->LoadDataInt( &animeType );
  file->LoadDataInt( &externalMotion );
  file->LoadDataInt( &isLoop );
  file->LoadDataInt( &playExternalOnly );
  

  GFL_PRINT("\t\t--->Setting Mutli Animation: %i,%i\n",animeType,externalMotion);

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  const u32 grpNo = param.groupNo;

  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  int objNum;
  SequenceObject_Poke **objArr = core->GetPokeObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Poke *poke = objArr[i];

    poke->SetCreateFrame(param.startFrame);

    if(playExternalOnly==0)
    {
      poke->ChangeAnimationDirect(animeType);
      poke->SetAnimationIsLoop( isLoop != 0);      
    }

    // Set external Animation slot 1:
    gfl2::renderingengine::scenegraph::resource::ResourceNode *anmRes = core->GetResourceManager()->GetDataBuf_Anime(ARCID_DEMOSEQUENCE_RESOURCE,externalMotion);
    poke->ChangeAnimationResource(anmRes,1);    
    poke->SetAnimationIsLoop(isLoop != 0,1);
    poke->UpdateAnimation(frameDif);

  }
}

SEQPLAYER_CMD_DEF( POKEMON_SETBASESCALE ) 
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  const u32 grpNo = param.groupNo;

  int objNum;
  SequenceObject_Poke **objArr = core->GetPokeObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Poke *poke = objArr[i];    
    poke->GetPokeModel()->SetAdjustHeight(poke->GetPokeModel()->GetBaseHeight());
  }
}

SEQPLAYER_CMD_DEF( POKEMON_FOLLOW_MODEL )
{
 
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int  targetModel;
  char nodeName[32];
  bool  followPos,followRot,followScale;
  gfl2::math::Vector3 posOfs;
  gfl2::math::Vector3 rotOfs;

  file->LoadDataInt( &targetModel );
  file->LoadDataString(nodeName,32);
  file->LoadDataBool( &followPos );
  file->LoadDataBool( &followRot );
  file->LoadDataBool( &followScale );
  file->LoadDataVec3( &posOfs );
  file->LoadDataVec3( &rotOfs );

  int objNum;
  SequenceObject_Poke **objArr = core->GetPokeObjManager()->GetObjectArr(grpNo,&objNum);

  if( targetModel == -1 )
  {
    //解除
    for( int i=0;i<objNum;i++)
    {
      SequenceObject_Poke *obj = objArr[i];
      obj->SetFollowObject(NULL);
    }
  }
  else
  {
    //追従
    int mdlNum = 0;
    SequenceObject_Gfmdl **mdlArr = core->GetModelObjManager()->GetObjectArr(targetModel,&mdlNum);
    if( objNum != mdlNum )
    {
      GFL_PRINT("SeqCommand:追従対象の個数が一致しません。追従元[%d] 追従先[]\n",objNum,mdlNum);
      return;
    }

    for( int i=0;i<objNum;i++)
    {
      SequenceObject_Poke *obj = objArr[i];
      SequenceObject_Gfmdl *mdl = mdlArr[i];
      obj->SetFollowObject(mdl,nodeName,followPos,followRot,followScale);

      obj->SetOffset(posOfs);
      obj->SetRotateOffset(rotOfs);

    }
  }
}

SEQPLAYER_CMD_DEF( POKEMON_INFRAME_SIZEGROUPEX_SETTING )
{

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	  
  const u32 grpNo = param.groupNo;

  int index,pos;

  int frame_size_x_s,frame_size_y_s;
  int pos_x_s,pos_y_s;

  int frame_size_x_m,frame_size_y_m;
  int pos_x_m,pos_y_m;

  int frame_size_x_l,frame_size_y_l;
  int pos_x_l,pos_y_l;

  file->LoadDataInt(&index);
  file->LoadDataInt(&pos);
  file->LoadDataInt( &frame_size_x_s );
  file->LoadDataInt( &frame_size_y_s );
  file->LoadDataInt( &pos_x_s );
  file->LoadDataInt( &pos_y_s );

  file->LoadDataInt( &frame_size_x_m );
  file->LoadDataInt( &frame_size_y_m );
  file->LoadDataInt( &pos_x_m );
  file->LoadDataInt( &pos_y_m );

  file->LoadDataInt( &frame_size_x_l );
  file->LoadDataInt( &frame_size_y_l );
  file->LoadDataInt( &pos_x_l );
  file->LoadDataInt( &pos_y_l );

  GFL_ASSERT_MSG(frame_size_x_s==frame_size_y_s,"インフレームのXとYが一致していません！\n");
  GFL_ASSERT_MSG(frame_size_x_m==frame_size_y_m,"インフレームのXとYが一致していません！\n");
  GFL_ASSERT_MSG(frame_size_x_l==frame_size_y_l,"インフレームのXとYが一致していません！\n");

  int objNum;
  SequenceObject_Poke **objArr = core->GetPokeObjManager()->GetObjectArr(grpNo,&objNum);

  if(objNum<=0 || objArr[0] == NULL) return;

  SequenceObject_Poke *obj = objArr[0];

  int type = obj->GetPokeModel()->GetPokeSize();

  int frame_size_x,frame_size_y;
  int pos_x,pos_y;

  switch(type)
  {
  case PokeTool::POKE_SIZE_S:
    frame_size_x = frame_size_x_s;
    frame_size_y = frame_size_x_s;
    pos_x = pos_x_s;
    pos_y = pos_y_s;    
    break;
  case PokeTool::POKE_SIZE_M:
    frame_size_x = frame_size_x_m;
    frame_size_y = frame_size_x_m;
    pos_x = pos_x_m;
    pos_y = pos_y_m;    
    break;
  case PokeTool::POKE_SIZE_L:
    frame_size_x = frame_size_x_l;
    frame_size_y = frame_size_x_l;
    pos_x = pos_x_l;
    pos_y = pos_y_l;    
    break;
  default:
    frame_size_x = frame_size_x_m;
    frame_size_y = frame_size_x_m;
    pos_x = 0;
    pos_y = 0;    
    break;
  }

  core->Inframe_SetSizeAndPosition(index,pos,frame_size_x,frame_size_y,pos_x,pos_y);
  
}

SEQPLAYER_CMD_DEF( POKEMON_INFRAME_FOLLOWMODEL )
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int  index,targetModel;
  char nodeName[32];    
  file->LoadDataInt(&index);
  file->LoadDataInt( &targetModel );
  file->LoadDataString(nodeName,32);

  int mdlNum = 0;
  SequenceObject_Gfmdl **mdlArr = core->GetModelObjManager()->GetObjectArr(targetModel,&mdlNum);  

  if(mdlArr == NULL ) return;

  SequenceObject_Gfmdl *obj = mdlArr[0];

  if(mdlArr[0]==NULL) return;

  core->Inframe_SetFollowModel(index,obj,nodeName);
}

SEQPLAYER_CMD_DEF( POKEMON_LIGHT_DIR )
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  gfl2::math::Vector3 dir;
  file->LoadDataVec3( &dir );

  int objNum;
  SequenceObject_Poke **objArr = core->GetPokeObjManager()->GetObjectArr(grpNo,&objNum);

  if(objNum<=0 || objArr[0] == NULL) return;

  SequenceObject_Poke *obj = objArr[0];
  int ligntNo = obj->GetIndex() + LIGHTNO_POKEMON_TOP;

  core->GetRenderSys()->GetRenderingPipeLine()->GetDrawEnvNode(0)->GetLightSet(ligntNo)->GetLightNode(0)->SetDirection(dir);
}

SEQPLAYER_CMD_DEF( LABEL_CHARAMODEL ) { }

SEQPLAYER_CMD_DEF( CHARAMODEL_CREATENPC_FIELD ) 
{
  int index,dress,drawtype;
  file->LoadDataInt(&index);
  file->LoadDataInt(&dress);
  file->LoadDataInt(&drawtype);

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  gfl2::heap::HeapBase* heap = core->GetDevHeap();  

  // Frame Control:
  if(core->GetTargetFrame()>=param.endFrame) return; // Too late!
  const u32 frame = param.endFrame - core->GetTargetFrame();
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  int resourceManagerIndex = 0;

  bool isMale = false;
  if(index == 0) 
  {
    isMale = true;
    resourceManagerIndex = 1;
  }


  SequenceObject_Chara *chara = GFL_NEW(heap)SequenceObject_Chara(core,core->GetRenderSys()->GetRenderingPipeLine(),core->GetCharaModelFactoryField(),core->GetDressUpModelResourceManagerField(resourceManagerIndex));

  DemoLib::Obj::Object_Chara::LOAD_PARAM paramChara;
  paramChara.charaId = index;  
  
  paramChara.dressParam = NULL;

 
  if(dress!=-1)
    paramChara.dressParam = core->GetDressUpParam(dress,isMale);

  chara->SetCreateFrame(param.startFrame);
  chara->CreateNoLoad( paramChara , core->GetMemHeap() , core->GetGLAllocator() );
  chara->AddRenderPath(drawtype);
  chara->ChangeAnimation(0);
  chara->SetAnimationIsLoop(true);
  chara->SetIsFieldModel();
  chara->UpdateAnimation(frameDif);
  

//  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  
  int objNum = 1;

  SequenceObject_Chara **objArr = GFL_NEW_ARRAY(heap)SequenceObject_Chara*[objNum];	
  core->GetCharaObjManager()->SetObjectArr(objArr,objNum,grpNo);

  for( int i=0;i<objNum;i++)
  {
    objArr[i] = chara;
    core->GetObjectManager()->AddObject(objArr[i]);    
    DemoLib::Obj::CreateTask_DeleteObject_Chara(core->GetTaskSystem(),heap,objArr[i],frame);
  }
}

SEQPLAYER_CMD_DEF( CHARAMODEL_CREATENPC_FIELD_ATTACHITEM ) 
{
  int index,dress,drawtype,item;
  char nodeName[32];
  DemoLib::Obj::Object_Chara::LOAD_PARAM paramChara;

  file->LoadDataInt(&index);
  file->LoadDataInt(&dress);
  file->LoadDataInt(&drawtype);
  file->LoadDataInt(&item);
  file->LoadDataString(paramChara.attachJoint,32);


  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  gfl2::heap::HeapBase* heap = core->GetDevHeap();  

  // Frame Control:
  if(core->GetTargetFrame()>=param.endFrame) return; // Too late!
  const u32 frame = param.endFrame - core->GetTargetFrame();
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  int resourceManagerIndex = 0;

  bool isMale = false;
  if(index == 0) 
  {
    isMale = true;
    resourceManagerIndex = 1;
  }


  SequenceObject_Chara *chara = GFL_NEW(heap)SequenceObject_Chara(core,core->GetRenderSys()->GetRenderingPipeLine(),core->GetCharaModelFactoryField(),core->GetDressUpModelResourceManagerField(resourceManagerIndex));

  paramChara.charaId = index;  

  paramChara.dressParam = NULL;
  paramChara.attachModelId = item;

  if(dress!=-1)
    paramChara.dressParam = core->GetDressUpParam(dress,isMale);

  chara->SetCreateFrame(param.startFrame);
  chara->CreateNoLoad( paramChara , core->GetMemHeap() , core->GetGLAllocator() );
  chara->AddRenderPath(drawtype);
  chara->ChangeAnimation(0);
  chara->SetAnimationIsLoop(true);
  chara->SetIsFieldModel();
  chara->UpdateAnimation(frameDif);


  //  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int objNum = 1;

  SequenceObject_Chara **objArr = GFL_NEW_ARRAY(heap)SequenceObject_Chara*[objNum];	
  core->GetCharaObjManager()->SetObjectArr(objArr,objNum,grpNo);

  for( int i=0;i<objNum;i++)
  {
    objArr[i] = chara;
    core->GetObjectManager()->AddObject(objArr[i]);    
    DemoLib::Obj::CreateTask_DeleteObject_Chara(core->GetTaskSystem(),heap,objArr[i],frame);
  }
}



SEQPLAYER_CMD_DEF( CHARAMODEL_CREATENPC_BATTLE ) 
{
  int index,dress,drawtype;
  file->LoadDataInt(&index);
  file->LoadDataInt(&dress);
  file->LoadDataInt(&drawtype);

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  gfl2::heap::HeapBase* heap = core->GetDevHeap();  

  // Frame Control:
  if(core->GetTargetFrame()>=param.endFrame) return; // Too late!
  const u32 frame = param.endFrame - core->GetTargetFrame();
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  int resourceManagerIndex = 0;

  bool isMale = false;
  if(index == 0) 
  {
      isMale = true;
      resourceManagerIndex = 1;
  }
  SequenceObject_Chara *chara = GFL_NEW(heap)SequenceObject_Chara(core,core->GetRenderSys()->GetRenderingPipeLine(),core->GetCharaModelFactoryBattle(),core->GetDressUpModelResourceManagerBattle(resourceManagerIndex));

  DemoLib::Obj::Object_Chara::LOAD_PARAM paramChara;
  paramChara.charaId = index;  
  
  paramChara.dressParam = NULL;


  if(dress!=-1)
    paramChara.dressParam = core->GetDressUpParam(dress,isMale);

  chara->SetCreateFrame(param.startFrame);
  chara->CreateNoLoad( paramChara , core->GetMemHeap() , core->GetGLAllocator() );
  chara->AddRenderPath(drawtype);
  chara->ChangeAnimation(0);
  chara->SetAnimationIsLoop(true);
  chara->SetIsBattleModel();
  chara->UpdateAnimation(frameDif);

  // Monster Ball Model Fix:
  if(index >= CHARA_MODEL_ID_BTOB0004_00 && index<= CHARA_MODEL_ID_BTOB0401_00)
  {
    chara->GetBaseModel()->GetModelInstanceNode()->SetSortPriority(192);
  }

//  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int objNum = 1;

  SequenceObject_Chara **objArr = GFL_NEW_ARRAY(heap)SequenceObject_Chara*[objNum];	
  core->GetCharaObjManager()->SetObjectArr(objArr,objNum,grpNo);

  for( int i=0;i<objNum;i++)
  {
    objArr[i] = chara;
    core->GetObjectManager()->AddObject(objArr[i]);    
    DemoLib::Obj::CreateTask_DeleteObject_Chara(core->GetTaskSystem(),heap,objArr[i],frame);
  }
}

SEQPLAYER_CMD_DEF( CHARAMODEL_CREATENPC_BATTLE_ATTACHITEM ) 
{
  int index,dress,drawtype,item;
  char nodeName[32];
  DemoLib::Obj::Object_Chara::LOAD_PARAM paramChara;

  file->LoadDataInt(&index);
  file->LoadDataInt(&dress);
  file->LoadDataInt(&drawtype);
  file->LoadDataInt(&item);
  file->LoadDataString(paramChara.attachJoint,32);

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  gfl2::heap::HeapBase* heap = core->GetDevHeap();  

  // Frame Control:
  if(core->GetTargetFrame()>=param.endFrame) return; // Too late!
  const u32 frame = param.endFrame - core->GetTargetFrame();
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  int resourceManagerIndex = 0;

  bool isMale = false;
  if(index == 0) 
  {
    isMale = true;
    resourceManagerIndex = 1;
  }
  SequenceObject_Chara *chara = GFL_NEW(heap)SequenceObject_Chara(core,core->GetRenderSys()->GetRenderingPipeLine(),core->GetCharaModelFactoryBattle(),core->GetDressUpModelResourceManagerBattle(resourceManagerIndex));

  paramChara.charaId = index;  

  paramChara.dressParam = NULL;

  paramChara.attachModelId = item;



  if(dress!=-1)
    paramChara.dressParam = core->GetDressUpParam(dress,isMale);

  chara->SetCreateFrame(param.startFrame);
  chara->CreateNoLoad( paramChara , core->GetMemHeap() , core->GetGLAllocator() );
  chara->AddRenderPath(drawtype);
  chara->ChangeAnimation(0);
  chara->SetAnimationIsLoop(true);
  chara->SetIsBattleModel();
  chara->UpdateAnimation(frameDif);


  //  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int objNum = 1;

  SequenceObject_Chara **objArr = GFL_NEW_ARRAY(heap)SequenceObject_Chara*[objNum];	
  core->GetCharaObjManager()->SetObjectArr(objArr,objNum,grpNo);

  for( int i=0;i<objNum;i++)
  {
    objArr[i] = chara;
    core->GetObjectManager()->AddObject(objArr[i]);    
    DemoLib::Obj::CreateTask_DeleteObject_Chara(core->GetTaskSystem(),heap,objArr[i],frame);
  }
}

SEQPLAYER_CMD_DEF( CHARAMODEL_CHANGEANIMATION ) 
{
  int index;
  file->LoadDataInt( &index );

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  const u32 grpNo = param.groupNo;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  int objNum;
  SequenceObject_Chara **objArr = core->GetCharaObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Chara *chara = objArr[i];    

    chara->SetCreateFrame(param.startFrame);
    chara->ChangeAnimation(index);    
    chara->SetAnimationIsLoop(true);
    chara->UpdateAnimation(frameDif);
  }

}

SEQPLAYER_CMD_DEF( CHARAMODEL_SETANIMATIONEXTERNAL ) 
{
  int externalMotion,isLoop;

  file->LoadDataInt( &externalMotion );
  file->LoadDataInt( &isLoop );

  GFL_PRINT("\t\t--->Setting Animation: %i\n",externalMotion);

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  const u32 grpNo = param.groupNo;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  int objNum;
  SequenceObject_Chara **objArr = core->GetCharaObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Chara *chara = objArr[i];

    gfl2::renderingengine::scenegraph::resource::ResourceNode *anmRes = core->GetResourceManager()->GetDataBuf_Anime(ARCID_DEMOSEQUENCE_RESOURCE,externalMotion);
    chara->SetCreateFrame(param.startFrame);
    chara->ChangeAnimationResource(anmRes);    
    chara->SetAnimationIsLoop(isLoop != 0);
    chara->UpdateAnimation(frameDif);
  }
}

SEQPLAYER_CMD_DEF( CHARAMODEL_SETANIMATIONEXTERNAL_DRESSUP )
{
  int externalMotion,isLoop;

  file->LoadDataInt( &externalMotion );
  file->LoadDataInt( &isLoop );

  GFL_PRINT("\t\t--->Setting DressUp Animation: %i\n",externalMotion);

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  const u32 grpNo = param.groupNo;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  int objNum;
  SequenceObject_Chara **objArr = core->GetCharaObjManager()->GetObjectArr(grpNo,&objNum);

  void *buf = core->GetResourceManager()->GetDataBuf(ARCID_DEMOSEQUENCE_RESOURCE,externalMotion);

  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Chara *chara = objArr[i];  
    chara->SetCreateFrame(param.startFrame);
    chara->ChangeAnimationResourceDressUp(buf, core->GetGLAllocator() );    
    chara->SetAnimationIsLoop(isLoop != 0);
    chara->UpdateAnimation(frameDif);
  }
}

SEQPLAYER_CMD_DEF( CHARAMODEL_MOVE_POSITION ) 
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  gfl2::math::Vector3 pos;
  int isRelative;

  file->LoadDataVec3( &pos );
  file->LoadDataInt( &isRelative );

  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  int objNum;
  SequenceObject_Chara **objArr = core->GetCharaObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Chara *obj = objArr[i];
    if( isRelative != 0 )
    {
      pos += obj->GetPosition();
    }

    if( frame == 0 || frame<=frameDif)
    {
      obj->SetPosition(pos);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectMove(core->GetTaskSystem(),heap,obj,obj->GetPosition(),pos,frame);
      Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
      t->Update(frameDif);      
      obj->UpdatePosition();
    }
  }
}

SEQPLAYER_CMD_DEF( CHARAMODEL_ROTATE ) 
{
  gfl2::math::Vector3 rotBase;
  bool isRelative;

  file->LoadDataVec3( &rotBase );
  file->LoadDataBool( &isRelative );

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  const u32 grpNo = param.groupNo;
  const u32 frame = param.endFrame - param.startFrame;  
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;
  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  int objNum;
  SequenceObject_Chara **objArr = core->GetCharaObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Chara *obj = objArr[i];    

    gfl2::math::Vector3 rot = rotBase;
    if( isRelative == true )
    {
      rot += obj->GetRotate();
    }

    if( frame == 0 || frame<=frameDif)
    {
      obj->SetRotate(rot);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectRotate(core->GetTaskSystem(),heap,obj,obj->GetRotate(),rot,frame);
      Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
      t->Update(frameDif);      
      obj->UpdatePosition();
    }
    
  }

}


SEQPLAYER_CMD_DEF( CHARAMODEL_SETVISIBLE ) 
{
  int isVisible;

  file->LoadDataInt( &isVisible );  

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  const u32 grpNo = param.groupNo;

  int objNum;
  SequenceObject_Chara **objArr = core->GetCharaObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Chara *chara = objArr[i];
    chara->SetVisible( isVisible != 0);
  }

}

SEQPLAYER_CMD_DEF( CHARAMODEL_FOLLOW_MODEL )
{

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int  targetModel;
  char nodeName[32];
  bool  followPos,followRot,followScale;
  gfl2::math::Vector3 posOfs;
  gfl2::math::Vector3 rotOfs;

  file->LoadDataInt( &targetModel );
  file->LoadDataString(nodeName,32);
  file->LoadDataBool( &followPos );
  file->LoadDataBool( &followRot );
  file->LoadDataBool( &followScale );
  file->LoadDataVec3( &posOfs );
  file->LoadDataVec3( &rotOfs );

  int objNum;
  SequenceObject_Chara **objArr = core->GetCharaObjManager()->GetObjectArr(grpNo,&objNum);

  if( targetModel == -1 )
  {
    //解除
    for( int i=0;i<objNum;i++)
    {
      SequenceObject_Chara *obj = objArr[i];
      obj->SetFollowObject(NULL);
    }
  }
  else
  {
    //追従
    int mdlNum = 0;
    SequenceObject_Gfmdl **mdlArr = core->GetModelObjManager()->GetObjectArr(targetModel,&mdlNum);
    if( objNum != mdlNum )
    {
      GFL_PRINT("SeqCommand:追従対象の個数が一致しません。追従元[%d] 追従先[]\n",objNum,mdlNum);
      return;
    }

    for( int i=0;i<objNum;i++)
    {
      SequenceObject_Chara *obj = objArr[i];
      SequenceObject_Gfmdl *mdl = mdlArr[i];
      obj->SetFollowObject(mdl,nodeName,followPos,followRot,followScale);

      obj->SetOffset(posOfs);
      obj->SetRotateOffset(rotOfs);
    }
  }
}

SEQPLAYER_CMD_DEF( LABEL_LIGHTING ) { }

SEQPLAYER_CMD_DEF( LIGHT_CREATE ) 
{
  int arcidx;
  file->LoadDataInt(&arcidx);
  
  GFL_PRINT("\t\t--->ArcIdx:%i\n",arcidx);

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  // Frame Control:
  if(core->GetTargetFrame()>=param.endFrame) return; // Too late!
  const u32 frame = param.endFrame - core->GetTargetFrame();
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  int objNum = 1;

  SequenceObject_Light **objArr = GFL_NEW_ARRAY(heap)SequenceObject_Light*[objNum];	
  core->GetLightObjManager()->SetObjectArr(objArr,objNum,grpNo);


  for( int i=0;i<objNum;i++)
  {
    objArr[i] = GFL_NEW(heap)SequenceObject_Light(core,core->GetRenderSys()->GetRenderingPipeLine());
    core->GetObjectManager()->AddObject(objArr[i]);

    objArr[i]->SetCreateFrame(param.startFrame);

    void *buf = core->GetResourceManager()->GetDataBuf(ARCID_DEMOSEQUENCE_RESOURCE,arcidx);

    objArr[i]->CreateBuffer(buf,heap,core->GetGLAllocator());
    objArr[i]->AddRenderPath();

    DemoLib::Obj::CreateTask_DeleteObject_Light(core->GetTaskSystem(),heap,objArr[i],frame);
  }
}

SEQPLAYER_CMD_DEF( LIGHT_CREATE_FIELD ) 
{  
  int arcidx;
  int motIdx1,motIdx2;
  int timeType;
  int weatherType;

  file->LoadDataInt(&arcidx);
  file->LoadDataInt(&motIdx1);
  file->LoadDataInt(&motIdx2);
  file->LoadDataInt(&timeType);
  file->LoadDataInt(&weatherType);

  GFL_PRINT("\t\t--->LightIdx:%i  Motion1:%i  Motion2:%i  TimeSet:%I  Weather:%i\n",arcidx,motIdx1,motIdx2,timeType,weatherType);

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  // Frame Control:
  if(core->GetTargetFrame()>=param.endFrame) return; // Too late!
  const u32 frame = param.endFrame - core->GetTargetFrame();
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  int objNum = 1;

  SequenceObject_Light **objArr = GFL_NEW_ARRAY(heap)SequenceObject_Light*[objNum];	
  core->GetLightObjManager()->SetObjectArr(objArr,objNum,grpNo);


  for( int i=0;i<objNum;i++)
  {
    objArr[i] = GFL_NEW(heap)SequenceObject_Light(core,core->GetRenderSys()->GetRenderingPipeLine(),true);
    core->GetObjectManager()->AddObject(objArr[i]);

    objArr[i]->SetCreateFrame(param.startFrame);

    void *buf = core->GetResourceManager()->GetDataBuf(ARCID_DEMOSEQUENCE_RESOURCE,arcidx);

    objArr[i]->CreateBuffer(buf,heap,core->GetGLAllocator());
    objArr[i]->AddRenderPath(2);

    // Load Motion 1:
    if(weatherType==0)
    {
      gfl2::renderingengine::scenegraph::resource::ResourceNode *anmRes = core->GetResourceManager()->GetDataBuf_Anime(ARCID_DEMOSEQUENCE_RESOURCE,motIdx1);
      objArr[i]->ChangeAnimation(anmRes,heap,false);  
    }
    else // Load Motion 2:
    {
      gfl2::renderingengine::scenegraph::resource::ResourceNode *anmRes = core->GetResourceManager()->GetDataBuf_Anime(ARCID_DEMOSEQUENCE_RESOURCE,motIdx2);
      objArr[i]->ChangeAnimation(anmRes,heap,false);  
    }

    objArr[i]->SetTimeType(timeType);

    DemoLib::Obj::CreateTask_DeleteObject_Light(core->GetTaskSystem(),heap,objArr[i],frame);
  }
}

SEQPLAYER_CMD_DEF( LIGHT_CREATE_NORMAL2 ) 
{
  int arcidx;
  file->LoadDataInt(&arcidx);

  GFL_PRINT("\t\t--->ArcIdx:%i\n",arcidx);

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  // Frame Control:
  if(core->GetTargetFrame()>=param.endFrame) return; // Too late!
  const u32 frame = param.endFrame - core->GetTargetFrame();
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  int objNum = 1;

  SequenceObject_Light **objArr = GFL_NEW_ARRAY(heap)SequenceObject_Light*[objNum];	
  core->GetLightObjManager()->SetObjectArr(objArr,objNum,grpNo);


  for( int i=0;i<objNum;i++)
  {
    objArr[i] = GFL_NEW(heap)SequenceObject_Light(core,core->GetRenderSys()->GetRenderingPipeLine());
    core->GetObjectManager()->AddObject(objArr[i]);

    objArr[i]->SetCreateFrame(param.startFrame);

    void *buf = core->GetResourceManager()->GetDataBuf(ARCID_DEMOSEQUENCE_RESOURCE,arcidx);

    objArr[i]->CreateBuffer(buf,heap,core->GetGLAllocator());
    objArr[i]->AddRenderPath(1);

    DemoLib::Obj::CreateTask_DeleteObject_Light(core->GetTaskSystem(),heap,objArr[i],frame);
  }
}

SEQPLAYER_CMD_DEF( LIGHT_ANIMATION ) 
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  const u32 grpNo = param.groupNo;

  int arcIdx;
  int isLoop;

  file->LoadDataInt( &arcIdx ); //先読み処理の関係上先頭で！
  file->LoadDataInt( &isLoop );

  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  int objNum;
  SequenceObject_Light **objArr = core->GetLightObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Light *obj = objArr[i];

    obj->SetCreateFrame(param.startFrame);
    gfl2::renderingengine::scenegraph::resource::ResourceNode *anmRes = core->GetResourceManager()->GetDataBuf_Anime(ARCID_DEMOSEQUENCE_RESOURCE,arcIdx);
    obj->ChangeAnimation(anmRes,heap,isLoop != 0);  
    obj->UpdateAnimation(frameDif);
  }
}

SEQPLAYER_CMD_DEF( LABEL_CAMERA ) { }

SEQPLAYER_CMD_DEF( CAMERA_MOVE_POSITION ) 
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  gfl2::math::Vector3 pos,trg;
  f32 fov;
  int isRelative,opt;

  file->LoadDataVec3( &pos );
  file->LoadDataVec3( &trg );
  file->LoadDataF32( &fov );
  file->LoadDataInt( &opt );
  file->LoadDataInt( &isRelative );

  if( isRelative )
  {
    pos += core->GetCameraPosition();
    trg += core->GetCameraTarget();
  }

  if( frame == 0 || frameDif>=frame)
  {
    core->SetCameraPosition(pos,trg);
    if(fov != 0)
    {
      core->SetCameraFov(fov);
    }
  }
  else
  {
    CreateTask_MoveCamera(core->GetTaskSystem(),core->GetMemHeap(),core,pos,trg,fov,frame,(DemoLib::Obj::TASK_MOVE_TYPE)opt);
    Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
    t->Update(frameDif);
  }
}


SEQPLAYER_CMD_DEF( CAMERA_MOVE_POSITION_ONLY ) 
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  gfl2::math::Vector3 pos,trg;
  f32 fov;
  int isRelative,opt;

  trg = core->GetCameraTarget();

  file->LoadDataVec3( &pos );  
  file->LoadDataF32( &fov );
  file->LoadDataInt( &opt );
  file->LoadDataInt( &isRelative );

  if( isRelative )
  {
    pos += core->GetCameraPosition();    
  }

  if( frame == 0 || frameDif>=frame)
  {
    core->SetCameraPosition(pos,trg);
    if(fov != 0)
    {
      core->SetCameraFov(fov);
    }
  }
  else
  {
    CreateTask_MoveCamera(core->GetTaskSystem(),core->GetMemHeap(),core,pos,trg,fov,frame,(DemoLib::Obj::TASK_MOVE_TYPE)opt);
    Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
    t->Update(frameDif);
  }
}

SEQPLAYER_CMD_DEF( CAMERA_MOVE_TARGET_ONLY ) 
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  gfl2::math::Vector3 pos,trg;
  f32 fov;
  int isRelative,opt;

  pos = core->GetCameraPosition();
  
  file->LoadDataVec3( &trg );
  file->LoadDataF32( &fov );
  file->LoadDataInt( &opt );
  file->LoadDataInt( &isRelative );

  if( isRelative )
  {
    trg += core->GetCameraTarget();
  }

  if( frame == 0 || frameDif>=frame)
  {
    core->SetCameraPosition(pos,trg);
    if(fov != 0)
    {
      core->SetCameraFov(fov);
    }
  }
  else
  {
    CreateTask_MoveCamera(core->GetTaskSystem(),core->GetMemHeap(),core,pos,trg,fov,frame,(DemoLib::Obj::TASK_MOVE_TYPE)opt);
    Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
    t->Update(frameDif);
  }
}

SEQPLAYER_CMD_DEF( CAMERA_SET_POSITION_POKEMON ) 
{
 SequenceViewSystem *core = (SequenceViewSystem*)userWork;
 
  gfl2::math::Vector3 pos,trg;
  f32 fov;
  int trgPoke,trgNode,adjustScale;  
  
  file->LoadDataInt( &trgPoke );
  file->LoadDataInt( &trgNode );
  file->LoadDataVec3( &pos );  
  file->LoadDataF32( &fov );  
  file->LoadDataInt( &adjustScale );  
  

  int mdlNum = 0;
  SequenceObject_Poke **mdlArr = core->GetPokeObjManager()->GetObjectArr(trgPoke,&mdlNum);
  if( mdlNum != 1 ) return;  

  SequenceObject_Poke *poke = mdlArr[0];       

  PokeTool::MODEL_NODE node;
  int nodeIdx = 0;
  poke->GetFollowNode(trgNode,&node,&nodeIdx);

  const bool ret = poke->GetNodePosition( node , nodeIdx , &trg );
  if( ret )
  {
    gfl2::math::Vector3 scale;
    scale.Set(poke->GetPokeModel()->GetAdjustHeight(),poke->GetPokeModel()->GetAdjustHeight(),poke->GetPokeModel()->GetAdjustHeight());
    if(adjustScale!=0) pos.MulPerElem(scale );
    pos+=trg;

    core->SetCameraPosition(pos,trg);
    if(fov != 0) core->SetCameraFov(fov);
  }
  
}




SEQPLAYER_CMD_DEF( CAMERA_RESET ) 
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  int opt;

  file->LoadDataInt( &opt );

  gfl2::math::Vector3 pos,trg;
  f32 fov;
  core->GetDefaultCamera(pos,trg,fov);

  if( frame == 0 || frameDif>=frame)
  {
    core->SetCameraPosition(pos,trg);
    core->SetCameraFov(fov);
  }
  else
  {
    CreateTask_MoveCamera(core->GetTaskSystem(),core->GetMemHeap(),core,pos,trg,fov,frame,(DemoLib::Obj::TASK_MOVE_TYPE)opt);
    Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
    t->Update(frameDif);
  }
}

//カメラ揺らし
SEQPLAYER_CMD_DEF( CAMERA_SHAKE )
{  
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  f32 sRate,eRate;
  f32 dec;
  int tempAxis;

  file->LoadDataF32( &sRate );
  file->LoadDataF32( &eRate );
  file->LoadDataF32( &dec );
  file->LoadDataInt( &tempAxis );

  DemoLib::Obj::TASK_AXIS_TYPE axis;
  switch( tempAxis )
  {
  case SEQ_DEF_AXIS_X:
    axis = DemoLib::Obj::AXIS_X;
    break;
  case SEQ_DEF_AXIS_Y:
    axis = DemoLib::Obj::AXIS_Y;
    break;
  case SEQ_DEF_AXIS_Z:
    axis = DemoLib::Obj::AXIS_Z;
    break;
  default:
    GFL_ASSERT_MSG(0,"Invalid axis type[%d]\n",tempAxis);
    axis = DemoLib::Obj::AXIS_X;
    break;
  }

  CreateTask_ShakeCamera(core->GetTaskSystem(),core->GetMemHeap(),core,sRate,eRate,dec,frame,axis );
  Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
  t->Update(frameDif);
}

SEQPLAYER_CMD_DEF( CAMERA_ANIMATION ) 
{
	int envIDX;
  int motIDX,isRelative;

	file->LoadDataInt(&envIDX);  
  file->LoadDataInt(&motIDX);
  file->LoadDataInt(&isRelative);
	GFL_PRINT("\t\t--->EnvIDX:%i \n",envIDX);

	SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  core->SetCameraStartAniFrame(param.startFrame);
  core->LoadCameraDrawEnv(ARCID_DEMOSEQUENCE_RESOURCE,envIDX,isRelative!=0);

	int frame = param.endFrame - param.startFrame;
	GFL_PRINT("\t\t--->MotIDX:%i NumFrame:%i\n",motIDX,frame);
	core->LoadCameraMotion(ARCID_DEMOSEQUENCE_RESOURCE,motIDX,frame,isRelative!=0);
  core->UpdateCamera(frameDif);
}


SEQPLAYER_CMD_DEF( LABEL_PARTICLE ) { }

SEQPLAYER_CMD_DEF( PARTICLE_CREATE ) 
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  ///const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int arcIdx;
  int isMulti;
  int drawType;

  file->LoadDataInt( &arcIdx ); //先読み処理の関係上先頭で！
  file->LoadDataInt( &isMulti );
  file->LoadDataInt( &drawType );

  const u32 frame = param.endFrame - core->GetTargetFrame();
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  GFL_PRINT("CREATE PARTICLE FRAMEDIF:%i  Frame:%i TargetFrame:%i\n",frameDif,frame,core->GetTargetFrame());

  // Frame Control:
  if(core->GetTargetFrame()>=param.endFrame) return; // Too late!
    

  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  int objNum = 1;//core->SeqComFunc_GetTargetPokeNum();
  SequenceObject_Effect **objArr = GFL_NEW_ARRAY(heap)SequenceObject_Effect*[objNum];

  core->GetEffectObjManager()->SetObjectArr(objArr,objNum,grpNo);
  //これをEffrctIDの基本にする
  int grpBase = core->GetEffectObjManager()->GetObjectIndex(grpNo);

  for( int i=0;i<objNum;i++)
  {
    objArr[i] = GFL_NEW(heap)SequenceObject_Effect(core);
    core->GetObjectManager()->AddObject(objArr[i]);

    int resIdx = core->GetResourceManager()->GetEffectResIdx(ARCID_DEMOSEQUENCE_RESOURCE,arcIdx);

    objArr[i]->CreateResIdx(core->GetEffectHeap(),resIdx);
    objArr[i]->AddRenderPath(drawType);
    
    DemoLib::Obj::CreateTask_DeleteObject_Effect(core->GetTaskSystem(),heap,objArr[i],frame);
    objArr[i]->ForceUpdate(frameDif);
  }
}

SEQPLAYER_CMD_DEF( PARTICLE_STOP ) 
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int objNum;
  SequenceObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Effect *obj = objArr[i];
    obj->StopParticle();
  }
}

SEQPLAYER_CMD_DEF( PARTICLE_MOVE_POSITION ) 
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  gfl2::math::Vector3 pos;
  bool isRelative;

  file->LoadDataVec3( &pos );
  file->LoadDataBool( &isRelative );

  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  int objNum;
  SequenceObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Effect *obj = objArr[i];

    if( isRelative == true )
    {
      pos += obj->GetPosition();
    }

    if( frame == 0 || frameDif>=frame)
    {
      obj->SetPosition(pos);
      obj->UpdatePosition();
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectMove(core->GetTaskSystem(),heap,obj,obj->GetPosition(),pos,frame);
      Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
   
      if(frameDif) t->Update(frameDif);      
      obj->UpdatePosition();      
    }
  }
}

SEQPLAYER_CMD_DEF( PARTICLE_SCALE ) 
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  GFL_PRINT("*SCALE FrameDif: %i  FrameStart:%i  Target:%i  Frame:%i\n",frameDif,param.startFrame,core->GetTargetFrame(),frame);

  gfl2::math::Vector3 sclBase;
  bool isRelative;

  file->LoadDataVec3( &sclBase );
  file->LoadDataBool( &isRelative );

  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  int objNum;
  SequenceObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Effect *obj = objArr[i];

    gfl2::math::Vector3 scl = sclBase;
    if( isRelative == true )
    {
      scl = scl.MulPerElem(obj->GetScale());
    }

    GFL_ASSERT_MSG(scl.x!=0&&scl.y!=0&&scl.z!=0 , "スケールを0にしないでください！\n" );

    if( frame == 0 || frameDif>=frame)
    {
      obj->SetScale(scl);
      obj->UpdatePosition();      
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectScale(core->GetTaskSystem(),heap,obj,obj->GetScale(),scl,frame);
      Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();

      if(frameDif) t->Update(frameDif);      
      obj->UpdatePosition();             
    }
  }
}

SEQPLAYER_CMD_DEF( PARTICLE_ROTATE ) 
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  gfl2::math::Vector3 rotBase;
  bool isRelative;

  file->LoadDataVec3( &rotBase );
  file->LoadDataBool( &isRelative );

  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  int objNum;
  SequenceObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Effect *obj = objArr[i];

    gfl2::math::Vector3 rot = rotBase;
    if( isRelative == true )
    {
      rot += obj->GetRotate();
    }

    if( frame == 0 || frameDif>=frame)
    {
      obj->SetRotate(rot);
      obj->UpdatePosition();
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectRotate(core->GetTaskSystem(),heap,obj,obj->GetRotate(),rot,frame);
      Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
      if(frameDif) t->Update(frameDif);      
      obj->UpdatePosition();
    }
  }
}

SEQPLAYER_CMD_DEF( PARTICLE_FOLLOW_MODEL ) 
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int  targetModel;
  char nodeName[32];
  bool  followPos,followRot,followScale;
  gfl2::math::Vector3 posOfs;
  gfl2::math::Vector3 rotOfs;

  file->LoadDataInt( &targetModel );
  file->LoadDataString(nodeName,32);
  file->LoadDataBool( &followPos );
  file->LoadDataBool( &followRot );
  file->LoadDataBool( &followScale );
  file->LoadDataVec3( &posOfs );
  file->LoadDataVec3( &rotOfs );

  int objNum;
  SequenceObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);

  if( targetModel == -1 )
  {
    //解除
    for( int i=0;i<objNum;i++)
    {
      SequenceObject_Effect *obj = objArr[i];
      obj->SetFollowObject(NULL);
      obj->Update();
      obj->UpdatePosition();      
    } 
  }
  else
  {
    //追従
    int mdlNum = 0;
    SequenceObject_Gfmdl **mdlArr = core->GetModelObjManager()->GetObjectArr(targetModel,&mdlNum);
    if( objNum != mdlNum )
    {
      GFL_PRINT("SequenceEffectCommand:追従対象の個数が一致しません。追従元[%d] 追従先[]\n",objNum,mdlNum);
      return;
    }

    for( int i=0;i<objNum;i++)
    {
      SequenceObject_Effect *obj = objArr[i];
      SequenceObject_Gfmdl *mdl = mdlArr[i];
      obj->SetFollowObject(mdl,nodeName,followPos,followRot,followScale);

      obj->SetOffset(posOfs);
      obj->SetRotateOffset(rotOfs);

      //優先順位の付け替え(親よりも低いもの)
      core->GetObjectManager()->RemoveObject(obj);
      core->GetObjectManager()->AddObject(obj,mdl->GetPriority()+1);

      obj->Update();
      obj->UpdatePosition();      
    }
  }
}

SEQPLAYER_CMD_DEF( PARTICLE_FOLLOW_POKEMON ) 
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int targetModel;  
  int trgNode;
  gfl2::math::Vector3 ofs;

  file->LoadDataInt( &targetModel );    
  file->LoadDataInt( &trgNode );
  file->LoadDataVec3( &ofs );

  int objNum;
  SequenceObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);

  if( targetModel == -1 )
  {
    //解除
    for( int i=0;i<objNum;i++)
    {
      SequenceObject_Effect *obj = objArr[i];
      obj->SetFollowObject(NULL);
    }
  }
  else
  {
    //追従
    int mdlNum = 0;
    SequenceObject_Poke **mdlArr = core->GetPokeObjManager()->GetObjectArr(targetModel,&mdlNum);
    if( objNum != mdlNum )
    {
      GFL_PRINT("SequenceEffectCommand:追従対象の個数が一致しません。追従元[%d] 追従先[]\n",objNum,mdlNum);
      return;
    }

    PokeTool::MODEL_NODE node;
    int nodeIdx = 0;
    

    for( int i=0;i<objNum;i++)
    {
      SequenceObject_Effect *obj = objArr[i];
      SequenceObject_Poke *poke = mdlArr[i];     
      poke->GetFollowNode(trgNode,&node,&nodeIdx);
      obj->SetFollowPoke(poke,node,nodeIdx);
      obj->SetOffset(ofs);

      //優先順位の付け替え(親よりも低いもの)
      core->GetObjectManager()->RemoveObject(obj);
      core->GetObjectManager()->AddObject(obj,poke->GetPriority()+1);

      obj->Update();
      obj->UpdatePosition();
    }
  }
}

SEQPLAYER_CMD_DEF( PARTICLE_FOLLOW_POKEMON_EX ) 
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int targetModel;  
  int trgNode;
  bool  followRot,followScale;
  gfl2::math::Vector3 posOfs;
  gfl2::math::Vector3 rotOfs;

  file->LoadDataInt( &targetModel );    
  file->LoadDataInt( &trgNode );
  file->LoadDataBool( &followRot );
  file->LoadDataBool( &followScale );
  file->LoadDataVec3( &posOfs );
  file->LoadDataVec3( &rotOfs );

  int objNum;
  SequenceObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);

  if( targetModel == -1 )
  {
    //解除
    for( int i=0;i<objNum;i++)
    {
      SequenceObject_Effect *obj = objArr[i];
      obj->SetFollowObject(NULL);
    }
  }
  else
  {
    //追従
    int mdlNum = 0;
    SequenceObject_Poke **mdlArr = core->GetPokeObjManager()->GetObjectArr(targetModel,&mdlNum);
    if( objNum != mdlNum )
    {
      GFL_PRINT("SequenceEffectCommand:追従対象の個数が一致しません。追従元[%d] 追従先[]\n",objNum,mdlNum);
      return;
    }

    PokeTool::MODEL_NODE node;
    int nodeIdx = 0;


    for( int i=0;i<objNum;i++)
    {
      SequenceObject_Effect *obj = objArr[i];
      SequenceObject_Poke *poke = mdlArr[i];     
      poke->GetFollowNode(trgNode,&node,&nodeIdx);
      obj->SetFollowPoke(poke,node,nodeIdx,followRot,followScale);
      obj->SetOffset(posOfs);
      obj->SetRotateOffset(rotOfs);

      //優先順位の付け替え(親よりも低いもの)
      core->GetObjectManager()->RemoveObject(obj);
      core->GetObjectManager()->AddObject(obj,poke->GetPriority()+1);

      obj->Update();
      obj->UpdatePosition();
    }
  }

}

SEQPLAYER_CMD_DEF( PARTICLE_FOLLOW_CHARA ) 
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int  targetModel;
  char nodeName[32];
  bool  followPos,followRot,followScale;
  gfl2::math::Vector3 posOfs;
  gfl2::math::Vector3 rotOfs;

  file->LoadDataInt( &targetModel );
  file->LoadDataString(nodeName,32);
  file->LoadDataBool( &followPos );
  file->LoadDataBool( &followRot );
  file->LoadDataBool( &followScale );
  file->LoadDataVec3( &posOfs );
  file->LoadDataVec3( &rotOfs );

  int objNum;
  SequenceObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);

  if( targetModel == -1 )
  {
    //解除
    for( int i=0;i<objNum;i++)
    {
      SequenceObject_Effect *obj = objArr[i];
      obj->SetFollowObject(NULL);
    }
  }
  else
  {
    //追従
    int mdlNum = 0;
    SequenceObject_Chara **mdlArr = core->GetCharaObjManager()->GetObjectArr(targetModel,&mdlNum);
    if( objNum != mdlNum )
    {
      GFL_PRINT("SequenceEffectCommand:追従対象の個数が一致しません。追従元[%d] 追従先[]\n",objNum,mdlNum);
      return;
    }

    for( int i=0;i<objNum;i++)
    {
      SequenceObject_Effect *obj = objArr[i];
      SequenceObject_Chara *mdl = mdlArr[i];
      obj->SetFollowObject(mdl,nodeName,followPos,followRot,followScale);

      obj->SetOffset(posOfs);
      obj->SetRotateOffset(rotOfs);

      //優先順位の付け替え(親よりも低いもの)
      core->GetObjectManager()->RemoveObject(obj);
      core->GetObjectManager()->AddObject(obj,mdl->GetPriority()+1);
      obj->Update();
      obj->UpdatePosition();
    }
  }
}

SEQPLAYER_CMD_DEF( PARTICLE_SP_MOVE_RESET ) 
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  gfl2::math::Vector3 pos(0,0,0);
  int objNum;
  SequenceObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Effect *obj = objArr[i];
    if( frame == 0 || frameDif>=frame)
    {
      obj->SetOffset(pos);      
      obj->UpdatePosition();
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectMoveOffset(core->GetTaskSystem(),heap,obj,obj->GetOffset(),pos,frame);
      Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
      t->Update(frameDif);      
      obj->UpdatePosition();
    }
  }
}

SEQPLAYER_CMD_DEF( PARTICLE_SP_MOVE_SHAKE ) 
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  f32 sRate,eRate;
  f32 sDec,eDec;
  int tempAxis;

  file->LoadDataF32( &sRate );
  file->LoadDataF32( &eRate );
  file->LoadDataF32( &sDec );
  file->LoadDataF32( &eDec );
  file->LoadDataInt( &tempAxis );

  DemoLib::Obj::TASK_AXIS_TYPE axis;
  switch( tempAxis )
  {
  case SEQ_DEF_AXIS_X:
    axis = DemoLib::Obj::AXIS_X;
    break;
  case SEQ_DEF_AXIS_Y:
    axis = DemoLib::Obj::AXIS_Y;
    break;
  case SEQ_DEF_AXIS_Z:
    axis = DemoLib::Obj::AXIS_Z;
    break;
  default:
    GFL_ASSERT_MSG(0,"Invalid axis type[%d]\n",tempAxis);
    axis = DemoLib::Obj::AXIS_X;
  }

  int objNum;
  SequenceObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Effect *obj = objArr[i];
    DemoLib::Obj::CreateTask_ObjMoveShake(core->GetTaskSystem(),heap,obj,sRate,eRate,sDec*360,eDec*360,frame,axis );
    Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
    t->Update(frameDif);      
    obj->UpdatePosition();
  }
}

SEQPLAYER_CMD_DEF( PARTICLE_SETVISIBLE ) 
{
  int isVisible;

  file->LoadDataInt( &isVisible );  

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  const u32 grpNo = param.groupNo;

  int objNum;
  SequenceObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Effect *obj = objArr[i];
    obj->SetVisible( isVisible != 0);
    obj->UpdatePosition();
  }

}

SEQPLAYER_CMD_DEF( LABEL_MODEL ) { }

SEQPLAYER_CMD_DEF (	MODEL_CREATE )
{
	int arcid,useMulti,drawType,texSwitch,texSource;
  char texName[32],matName[32];
	file->LoadDataInt(&arcid);
	file->LoadDataInt(&useMulti);

  file->LoadDataInt(&drawType);

  file->LoadDataInt(&texSwitch);

  file->LoadDataString(matName,32);
  file->LoadDataString(texName,32);
  file->LoadDataInt(&texSource);
	
	GFL_PRINT("\t\t--->ArcId:%i UseMulti:%i DrawType:%i\n",arcid,useMulti,drawType);

	SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
	const u32 grpNo = param.groupNo;

  // Frame Control:
  if(core->GetTargetFrame()>=param.endFrame) return; // Too late!
  const u32 frame = param.endFrame - core->GetTargetFrame();
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

	gfl2::heap::HeapBase* heap = core->GetDevHeap();

	int objNum = 1;

	SequenceObject_Gfmdl **objArr = GFL_NEW_ARRAY(heap)SequenceObject_Gfmdl*[objNum];	
	core->GetModelObjManager()->SetObjectArr(objArr,objNum,grpNo);


	for( int i=0;i<objNum;i++)
	{
		objArr[i] = GFL_NEW(heap)SequenceObject_Gfmdl(core,core->GetRenderSys()->GetRenderingPipeLine());
		core->GetObjectManager()->AddObject(objArr[i]);

    objArr[i]->SetCreateFrame(param.startFrame);

		void *buf = core->GetResourceManager()->GetDataBuf(ARCID_DEMOSEQUENCE_RESOURCE,arcid);

    if(texSwitch!=0)
    {
      GFL_PRINT("\t\t--->Creating with texture Replacement %i \n",texSource);
      objArr[i]->CreateBuffer(buf,heap,core->GetGLAllocator());

      gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pTextureResouceNode;
      pTextureResouceNode = reinterpret_cast<gfl2::renderingengine::scenegraph::resource::TextureResourceNode*>( core->GetReplaceTexture(texSource));

      // この機能は演出導入演出のみの使用で、データ形式も変更はない
      u32 texNum = objArr[i]->GetModelNode()->GetMaterialInstanceNode( 0)->GetTextureNum();
      for( u32 texCnt = 0; texCnt < texNum; ++texCnt)
      {
        objArr[i]->GetModelNode()->GetMaterialInstanceNode( 0)->SetTextureResourceNode( texCnt, pTextureResouceNode);
      }

#if defined(GF_PLATFORM_CTR)
      // CTRではUV値を回転させる。
      objArr[i]->GetBaseModel()->SetMaterialTextureCoordinatorRotateUV( 0, 0, GFL_MATH_DEG_TO_RAD(-90));
      // UV U成分を平行移動
      gfl2::math::Vector2 trans( 0.0f, 0.218750f );
      objArr[i]->GetBaseModel()->SetMaterialTextureCoordinatorTranslateUV( 0, 0, trans);
#endif // GF_PLATFORM_WIN32
    }
		else
    {
      objArr[i]->CreateBuffer(buf,heap,core->GetGLAllocator());
    }
		objArr[i]->AddRenderPath(drawType);

		DemoLib::Obj::CreateTask_DeleteObject_GfMdl(core->GetTaskSystem(),heap,objArr[i],frame);
	}
}


SEQPLAYER_CMD_DEF (	MODEL_CREATE_MONSTERBALL	)
{
  
  int ballIdx;
  int drawType;

  file->LoadDataInt( &ballIdx );
  file->LoadDataInt( &drawType );

  GFL_PRINT("\t\t--->BallType:%i DrawType:%i\n",ballIdx,drawType);

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  const u32 grpNo = param.groupNo;

  // Frame Control:
  if(core->GetTargetFrame()>=param.endFrame) return; // Too late!
  const u32 frame = param.endFrame - core->GetTargetFrame();
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  int objNum = 1;
  int ballType = core->SeqComFunc_GetTargetBall(ballIdx);

  SequenceObject_Gfmdl **objArr = GFL_NEW_ARRAY(heap)SequenceObject_Gfmdl*[objNum];	

  core->GetModelObjManager()->SetObjectArr(objArr,objNum,grpNo);


  for( int i=0;i<objNum;i++)
  {
    objArr[i] = GFL_NEW(heap)SequenceObject_Gfmdl(core,core->GetRenderSys()->GetRenderingPipeLine());
    core->GetObjectManager()->AddObject(objArr[i]);

    objArr[i]->SetCreateFrame(param.startFrame);

    void *buf = core->GetResourceManager()->GetDataBuf_BinPack(ARCID_CHARA_MODEL_BATTLE_NORMAL,CHARA_MODEL_ID_BTOB0201_00+ballType-1)->GetData(0);

    objArr[i]->CreateBuffer(buf,heap,core->GetGLAllocator());    
    objArr[i]->AddRenderPath(drawType);

    int matNum = objArr[i]->GetBaseModel()->GetMaterialCount();
    for( int matNo=0;matNo<matNum;matNo++ )
    {
      objArr[i]->GetBaseModel()->SetMaterialLightSetIndex(matNo,0);
    }

    DemoLib::Obj::CreateTask_DeleteObject_GfMdl(core->GetTaskSystem(),heap,objArr[i],frame);
  }
}


SEQPLAYER_CMD_DEF( MODEL_MOVE_POSITION ) 
{
	SequenceViewSystem *core = (SequenceViewSystem*)userWork;
	const u32 frame = param.endFrame - param.startFrame;
	const u32 grpNo = param.groupNo;

	gfl2::math::Vector3 pos;
	int isRelative;

	file->LoadDataVec3( &pos );
	file->LoadDataInt( &isRelative );

	gfl2::heap::HeapBase* heap = core->GetDevHeap();
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

	int objNum;
	SequenceObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);
	for( int i=0;i<objNum;i++)
	{
		SequenceObject_Gfmdl *obj = objArr[i];
		if( isRelative != 0 )
		{
			pos += obj->GetPosition();
		}

		if( frame == 0 || (frame<=frameDif && core->IsRealTimeMode() ) )
		{
			obj->SetPosition(pos);
		}
		else
		{
			DemoLib::Obj::CreateTask_ObjectMove(core->GetTaskSystem(),heap,obj,obj->GetPosition(),pos,frame);
      
      Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();

      if(core->IsRealTimeMode())
      {      
        t->Update(frameDif);      
        obj->UpdatePosition();
      }
		}
	}
}

SEQPLAYER_CMD_DEF( MODEL_SCALE ) 
{
	SequenceViewSystem *core = (SequenceViewSystem*)userWork;
	const u32 frame = param.endFrame - param.startFrame;
	const u32 grpNo = param.groupNo;

	gfl2::math::Vector3 sclBase;
	int isRelative;

	file->LoadDataVec3( &sclBase );
	file->LoadDataInt( &isRelative );

	gfl2::heap::HeapBase* heap = core->GetDevHeap();

  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

	int objNum;
	SequenceObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);
	for( int i=0;i<objNum;i++)
	{
		SequenceObject_Gfmdl *obj = objArr[i];

		gfl2::math::Vector3 scl = sclBase;
		if( isRelative != 0 )
		{
			scl = scl.MulPerElem(obj->GetScale());
		}

		GFL_ASSERT_MSG(scl.x!=0&&scl.y!=0&&scl.z!=0 , "スケールを0にしないでください！\n" );

		if( frame == 0 || (frame<=frameDif && core->IsRealTimeMode() ) )
		{
			obj->SetScale(scl);
		}
		else
		{
			DemoLib::Obj::CreateTask_ObjectScale(core->GetTaskSystem(),heap,obj,obj->GetScale(),scl,frame);
      Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
      
      if(core->IsRealTimeMode())
      {
        t->Update(frameDif);      
        obj->UpdatePosition();
      }
		}
	}
}

SEQPLAYER_CMD_DEF( MODEL_ROTATE ) 
{
	SequenceViewSystem *core = (SequenceViewSystem*)userWork;
	const u32 frame = param.endFrame - param.startFrame;
	const u32 grpNo = param.groupNo;

	gfl2::math::Vector3 rotBase;
	int isRelative;

	file->LoadDataVec3( &rotBase );
	file->LoadDataInt( &isRelative );

	gfl2::heap::HeapBase* heap = core->GetDevHeap();

  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

	int objNum;
	SequenceObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);
	for( int i=0;i<objNum;i++)
	{
		SequenceObject_Gfmdl *obj = objArr[i];

		gfl2::math::Vector3 rot = rotBase;
		if( isRelative != 0 )
		{
			rot += obj->GetRotate();
		}

		if( frame == 0 || (frame<=frameDif && core->IsRealTimeMode() ) )
		{
			obj->SetRotate(rot);
		}
		else
		{
			DemoLib::Obj::CreateTask_ObjectRotate(core->GetTaskSystem(),heap,obj,obj->GetRotate(),rot,frame);
      Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();

      if(core->IsRealTimeMode())
      {      
        t->Update(frameDif);      
        obj->UpdatePosition();
      }
		}
	}
}

SEQPLAYER_CMD_DEF( MODEL_ANIMATION ) 
{
	SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
	const u32 grpNo = param.groupNo;

	int arcIdx;
	int isLoop;

	file->LoadDataInt( &arcIdx ); //先読み処理の関係上先頭で！
	file->LoadDataInt( &isLoop );
	
	void *buf = core->GetResourceManager()->GetDataBuf(ARCID_DEMOSEQUENCE_RESOURCE,arcIdx);

  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

	int objNum;
	SequenceObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);
	for( int i=0;i<objNum;i++)
	{
		SequenceObject_Gfmdl *obj = objArr[i];

    obj->SetCreateFrame(param.startFrame);
		obj->SetAnimation(core->GetGLAllocator(),buf);
		obj->SetAnimationIsLoop(isLoop != 0);

    // Remove any slot 1 animation:
    obj->RemoveAnimation(1);

    obj->UpdateAnimation(frameDif);
	}
}

SEQPLAYER_CMD_DEF( MODEL_ANIMATION_MULTI ) 
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  const u32 grpNo = param.groupNo;

  int motion2,isLoop;
  int motion1,playMotion2Only;

  file->LoadDataInt( &motion1 );
  file->LoadDataInt( &motion2 );
  file->LoadDataInt( &isLoop );
  file->LoadDataInt( &playMotion2Only );

  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  void *buf = core->GetResourceManager()->GetDataBuf(ARCID_DEMOSEQUENCE_RESOURCE,motion1);
  void *buf2 = core->GetResourceManager()->GetDataBuf(ARCID_DEMOSEQUENCE_RESOURCE,motion2);

  int objNum;
  SequenceObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Gfmdl *obj = objArr[i];

    obj->SetCreateFrame(param.startFrame);

    if(playMotion2Only==0)
    {    
      obj->SetAnimation(core->GetGLAllocator(),buf);
      obj->SetAnimationIsLoop(isLoop != 0);
    }

    obj->SetAnimation(core->GetGLAllocator(),buf2,1);
    obj->SetAnimationIsLoop(isLoop != 0,1);

    obj->UpdateAnimation(frameDif);
  }
}

SEQPLAYER_CMD_DEF( MODEL_SETVISIBLE ) 
{
  int isVisible;

  file->LoadDataInt( &isVisible );  

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  const u32 grpNo = param.groupNo;

  int objNum;
  SequenceObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Gfmdl *obj = objArr[i];
    obj->SetVisible( isVisible != 0);
  }

}

SEQPLAYER_CMD_DEF( MODEL_FOLLOW_MODEL )
{

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int  targetModel;
  char nodeName[32];
  bool  followPos,followRot,followScale;
  gfl2::math::Vector3 posOfs;
  gfl2::math::Vector3 rotOfs;

  file->LoadDataInt( &targetModel );
  file->LoadDataString(nodeName,32);
  file->LoadDataBool( &followPos );
  file->LoadDataBool( &followRot );
  file->LoadDataBool( &followScale );
  file->LoadDataVec3( &posOfs );
  file->LoadDataVec3( &rotOfs );

  int objNum;
  SequenceObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);

  if( targetModel == -1 )
  {
    //解除
    for( int i=0;i<objNum;i++)
    {
      SequenceObject_Gfmdl *obj = objArr[i];
      obj->SetFollowObject(NULL);
    }
  }
  else
  {
    //追従
    int mdlNum = 0;
    SequenceObject_Gfmdl **mdlArr = core->GetModelObjManager()->GetObjectArr(targetModel,&mdlNum);
    if( objNum != mdlNum )
    {
      GFL_PRINT("SeqCommand:追従対象の個数が一致しません。追従元[%d] 追従先[]\n",objNum,mdlNum);
      return;
    }

    for( int i=0;i<objNum;i++)
    {
      SequenceObject_Gfmdl *obj = objArr[i];
      SequenceObject_Gfmdl *mdl = mdlArr[i];
      obj->SetFollowObject(mdl,nodeName,followPos,followRot,followScale);

      obj->SetOffset(posOfs);
      obj->SetRotateOffset(rotOfs);
    }
  }
}

SEQPLAYER_CMD_DEF( MODEL_FOLLOW_CHARAMODEL )
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int  targetModel;
  char nodeName[32];
  bool  followPos,followRot,followScale;
  gfl2::math::Vector3 posOfs;
  gfl2::math::Vector3 rotOfs;

  file->LoadDataInt( &targetModel );
  file->LoadDataString(nodeName,32);
  file->LoadDataBool( &followPos );
  file->LoadDataBool( &followRot );
  file->LoadDataBool( &followScale );
  file->LoadDataVec3( &posOfs );
  file->LoadDataVec3( &rotOfs );

  int objNum;
  SequenceObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);

  if( targetModel == -1 )
  {
    //解除
    for( int i=0;i<objNum;i++)
    {
      SequenceObject_Gfmdl *obj = objArr[i];
      obj->SetFollowObject(NULL);
    }
  }
  else
  {
    //追従
    int mdlNum = 0;
    SequenceObject_Chara **mdlArr = core->GetCharaObjManager()->GetObjectArr(targetModel,&mdlNum);
    if( objNum != mdlNum )
    {
      GFL_PRINT("SeqCommand:追従対象の個数が一致しません。追従元[%d] 追従先[]\n",objNum,mdlNum);
      return;
    }

    for( int i=0;i<objNum;i++)
    {
      SequenceObject_Gfmdl *obj = objArr[i];
      SequenceObject_Chara *mdl = mdlArr[i];
      obj->SetFollowObject(mdl,nodeName,followPos,followRot,followScale);

      obj->SetOffset(posOfs);
      obj->SetRotateOffset(rotOfs);
    }
  }

}

SEQPLAYER_CMD_DEF( MODEL_FOLLOW_POKE )
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int targetModel;  
  int trgNode;
  bool  followRot,followScale;
  gfl2::math::Vector3 posOfs;
  gfl2::math::Vector3 rotOfs;

  file->LoadDataInt( &targetModel );    
  file->LoadDataInt( &trgNode );
  file->LoadDataBool( &followRot );
  file->LoadDataBool( &followScale );
  file->LoadDataVec3( &posOfs );
  file->LoadDataVec3( &rotOfs );

  int objNum;
  SequenceObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);

  if( targetModel == -1 )
  {
    //解除
    for( int i=0;i<objNum;i++)
    {
      SequenceObject_Gfmdl *obj = objArr[i];
      obj->SetFollowObject(NULL);
    }
  }
  else
  {
    //追従
    int mdlNum = 0;
    SequenceObject_Poke **mdlArr = core->GetPokeObjManager()->GetObjectArr(targetModel,&mdlNum);
    if( objNum != mdlNum )
    {
      GFL_PRINT("SequenceModelCommand:追従対象の個数が一致しません。追従元[%d] 追従先[]\n",objNum,mdlNum);
      return;
    }

    PokeTool::MODEL_NODE node;
    int nodeIdx = 0;


    for( int i=0;i<objNum;i++)
    {
      SequenceObject_Gfmdl *obj = objArr[i];
      SequenceObject_Poke *poke = mdlArr[i];     
      poke->GetFollowNode(trgNode,&node,&nodeIdx);
      obj->SetFollowPoke(poke,node,nodeIdx,followRot,followScale);
      obj->SetOffset(posOfs);
      obj->SetRotateOffset(rotOfs);

      //優先順位の付け替え(親よりも低いもの)
      core->GetObjectManager()->RemoveObject(obj);
      core->GetObjectManager()->AddObject(obj,poke->GetPriority()+1);

      obj->Update();
      obj->UpdatePosition();
    }
  }

}

SEQPLAYER_CMD_DEF( MODEL_SET_TIMEANIMATION )
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  const u32 grpNo = param.groupNo;

  int motion1;
  int motionTime,isLoop,timeType;

  file->LoadDataInt( &motion1 );
  file->LoadDataInt( &motionTime );
  file->LoadDataInt( &timeType );
  file->LoadDataInt( &isLoop );


  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  void *buf = core->GetResourceManager()->GetDataBuf(ARCID_DEMOSEQUENCE_RESOURCE,motion1);
  void *buf2 = core->GetResourceManager()->GetDataBuf(ARCID_DEMOSEQUENCE_RESOURCE,motionTime);

  int objNum;
  SequenceObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Gfmdl *obj = objArr[i];

    obj->SetCreateFrame(param.startFrame);

    obj->SetAnimation(core->GetGLAllocator(),buf);
    obj->SetAnimationIsLoop(isLoop != 0);

    obj->SetAnimation(core->GetGLAllocator(),buf2,1);
    obj->SetAnimationIsLoop(false,1);

    timeType = -1;
    obj->SetTimeModel(timeType);

    obj->UpdateAnimation(frameDif);
  }
}

SEQPLAYER_CMD_DEF( LABEL_LAYOUT ) { }

SEQPLAYER_CMD_DEF( CREATE_LAYOUT ) 
{
	int arcidx,layoutIdx,numAni;
	
	file->LoadDataInt(&arcidx);
	file->LoadDataInt(&layoutIdx);
	file->LoadDataInt(&numAni);

	GFL_PRINT("\t\t--->ArcIdx:%i\n",arcidx);

	SequenceViewSystem *core = (SequenceViewSystem*)userWork;
	//const u32 frame = param.endFrame - param.startFrame;
	const u32 grpNo = param.groupNo;

  // Frame Control:
  if(core->GetTargetFrame()>=param.endFrame) return; // Too late!
  const u32 frame = param.endFrame - core->GetTargetFrame();
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

	gfl2::heap::HeapBase* heap = core->GetDevHeap();

	int objNum = 1;

	SequenceObject_Layout **objArr = GFL_NEW_ARRAY(heap)SequenceObject_Layout*[objNum];	
	core->GetLayoutObjManager()->SetObjectArr(objArr,objNum,grpNo);

	void *buf = core->GetResourceManager()->GetDataBuf(ARCID_DEMOSEQUENCELAYOUT_RESOURCE,arcidx);

	for( int i=0;i<objNum;i++)
	{
		// Create Layout Object:    
		objArr[i] = GFL_NEW(heap)SequenceObject_Layout(core,layoutIdx,numAni);

		objArr[i]->LoadLayout(buf);

		core->GetObjectManager()->AddObject(objArr[i]);

		DemoLib::Obj::CreateTask_DeleteObject_Layout(core->GetTaskSystem(),heap,objArr[i],frame);
	}
}

SEQPLAYER_CMD_DEF( CREATE_LAYOUT_MULTILANGUAGE ) 
{
  int arcidx,layoutIdx,numAni;

  file->LoadDataInt(&arcidx);
  file->LoadDataInt(&layoutIdx);
  file->LoadDataInt(&numAni);

  GFL_PRINT("\t\t--->ArcIdx:%i\n",arcidx);

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  // Frame Control:
  if(core->GetTargetFrame()>=param.endFrame) return; // Too late!
  const u32 frame = param.endFrame - core->GetTargetFrame();
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  int objNum = 1;

  SequenceObject_Layout **objArr = GFL_NEW_ARRAY(heap)SequenceObject_Layout*[objNum];	
  core->GetLayoutObjManager()->SetObjectArr(objArr,objNum,grpNo);

  int garcID = core->GetLanguageLayoutGARC();
  
  void *buf = core->GetResourceManager()->GetDataBuf( (gfl2::fs::ArcFile::ARCID)garcID,arcidx);

  for( int i=0;i<objNum;i++)
  {
    // Create Layout Object:
    objArr[i] = GFL_NEW(heap)SequenceObject_Layout(core,layoutIdx,numAni);

    objArr[i]->LoadLayout(buf);

    core->GetObjectManager()->AddObject(objArr[i]);

    DemoLib::Obj::CreateTask_DeleteObject_Layout(core->GetTaskSystem(),heap,objArr[i],frame);
  }
}

SEQPLAYER_CMD_DEF( LAYOUT_ANIMATIONSET ) 
{
	int aniIdx;
	SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
	const u32 grpNo = param.groupNo;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

	file->LoadDataInt(&aniIdx); // Animation Index to play
				
	int objNum;
	SequenceObject_Layout **objArr = core->GetLayoutObjManager()->GetObjectArr(grpNo,&objNum);
	
	for( int i=0;i<objNum;i++)
	{
		SequenceObject_Layout *obj = objArr[i];
		obj->SetAnimation(aniIdx);		
    obj->UpdateAnimation(frameDif);
	}

}

SEQPLAYER_CMD_DEF( LAYOUT_PANE_MOVE )
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  gfl2::math::Vector3 pos,sourcePos;
  int isRelative,paneIndex;

  file->LoadDataInt( &paneIndex );
  file->LoadDataVec3( &pos );
  file->LoadDataInt( &isRelative );

  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  int objNum;
  SequenceObject_Layout **objArr = core->GetLayoutObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Layout *obj = objArr[i];
    if( isRelative != 0 )
    {
      obj->GetPanePosition(sourcePos,paneIndex);
      pos += sourcePos;
    }

    if( frame == 0 || frame<=frameDif)
    {
      obj->SetPanePosition(pos,paneIndex);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectMove_LayoutPane(core->GetTaskSystem(),heap,obj,paneIndex,sourcePos,pos,frame);
      Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
      t->Update(frameDif);      
      obj->UpdatePosition();
    }
  }
}

SEQPLAYER_CMD_DEF( LAYOUT_PANE_SCALE )
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;


  gfl2::math::Vector3 sclBase;
  int isRelative,paneIndex;

  file->LoadDataInt( &paneIndex );
  file->LoadDataVec3( &sclBase );
  file->LoadDataInt( &isRelative );

  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  int objNum;
  SequenceObject_Layout **objArr = core->GetLayoutObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Layout *obj = objArr[i];

    gfl2::math::Vector3 scl = sclBase;
    if( isRelative != 0 )
    {
      scl = scl.MulPerElem(obj->GetScale());
    }

    GFL_ASSERT_MSG(scl.x!=0&&scl.y!=0&&scl.z!=0 , "スケールを0にしないでください！\n" );

    if( frame == 0 || frame<=frameDif)
    {
      obj->SetPaneScale(scl,paneIndex);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectScale_LayoutPane(core->GetTaskSystem(),heap,obj,paneIndex,obj->GetScale(),scl,frame);
      Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
      t->Update(frameDif);      
      obj->UpdatePosition();
    }
  }

}

SEQPLAYER_CMD_DEF( LAYOUT_PANE_ALPHA )
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  
  int startAlpha,endAlpha,paneIndex;

  file->LoadDataInt( &paneIndex );
  file->LoadDataInt( &startAlpha );
  file->LoadDataInt( &endAlpha );

  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  int objNum;
  SequenceObject_Layout **objArr = core->GetLayoutObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Layout *obj = objArr[i];
    
    if( frame == 0 || frame<=frameDif)
    {
      obj->SetPaneAlpha(static_cast<u8>(endAlpha),paneIndex);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectAlpha_LayoutPane(core->GetTaskSystem(),heap,obj,paneIndex,static_cast<u8>(startAlpha),static_cast<u8>(endAlpha),frame);
      Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
      t->Update(frameDif);      
      obj->UpdatePosition();
    }
  }
}

SEQPLAYER_CMD_DEF( LAYOUT_PANE_SETVISIBLE )
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  const u32 grpNo = param.groupNo;

  int isVisible,paneIndex;

  file->LoadDataInt( &paneIndex );
  file->LoadDataInt( &isVisible );
  
  int objNum;
  SequenceObject_Layout **objArr = core->GetLayoutObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Layout *obj = objArr[i];    
    obj->SetPaneVisible(isVisible!=0,paneIndex);
  }
}

//------------------------------------------------------------------------------------------------------------
//サウンド関係
//------------------------------------------------------------------------------------------------------------
//数値変換用(kujira移植)
#define ADJUST_SE_PITCH(val) (val<0 ? (val+127)*0.5f/127.0f+0.5f : val/127.0f+1.0f)
#define ADJUST_SE_PAN(val) (val/127.0f)
#define ADJUST_SE_TEMPO(val) (val/127.0f)
#define ADJUST_SE_VOL(val) (val/127.0f)

SEQPLAYER_CMD_DEF( LABEL_SOUND ) { }

SEQPLAYER_CMD_DEF( SOUND_SE_PLAY )
{
	int soundIdx;
	file->LoadDataInt(&soundIdx); // SE Index to play

  // Skip because of Command Skip?
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  if(!core->IsAfterCommandSkip(param.startFrame)) return;

	GFL_PRINT("\t\t--->Idx:%i\n",soundIdx);
	
	// Play Sound Effect:
	Sound::PlaySE( soundIdx );
}

SEQPLAYER_CMD_DEF( SOUND_SE_PLAYEX )
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  const u32 grpNo = param.groupNo;

  // Skip because of Command Skip?  
  if(!core->IsAfterCommandSkip(param.startFrame)) return;

  int soundId;
  int player,tempPan,tempPitch,tempVol,panType,tempTempo;
  file->LoadDataInt( &soundId );


  file->LoadDataInt( &player );
  file->LoadDataInt( &tempVol );
  file->LoadDataInt( &tempPan );
  file->LoadDataInt( &panType );
  file->LoadDataInt( &tempPitch );
  file->LoadDataInt( &tempTempo );


  const f32 pitch = ADJUST_SE_PITCH(tempPitch);
  const f32 pan = ADJUST_SE_PAN(tempPan); //-1～1
  const f32 vol = ADJUST_SE_VOL(tempVol);
  const f32 tempo = ADJUST_SE_TEMPO(tempTempo);

  core->GetSoundObj(grpNo)->SetSoundId(soundId);
  core->GetSoundObj(grpNo)->SetIs3DSound(false);
   
  Sound::PlaySE(soundId,0,grpNo,(player==-1 ? Sound::DEFAULT_PLAYER : player));
  Sound::ChangeSEVolume( soundId , vol , 0 , grpNo );
  Sound::ChangeSEPitch( soundId , pitch ,  grpNo );
  Sound::ChangeSEPan( soundId , pan ,  grpNo );
  Sound::ChangeSETempo( soundId , tempo ,  grpNo );

}

SEQPLAYER_CMD_DEF( SOUND_CTRL_VOLUME )
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	

  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  int sTemp,eTemp;
  file->LoadDataInt( &sTemp );
  file->LoadDataInt( &eTemp );

  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  //if( core->IsRecordStartSkip() == false )
  {
    if( frame == 0 || frameDif>=frame)
    {
      const f32 vol = ADJUST_SE_VOL(eTemp);
      Sound::ChangeSEVolume( core->GetSoundObj(grpNo)->GetSoundId() , vol , 0 , grpNo );
    }
    else
    {
      const f32 start = ADJUST_SE_VOL(sTemp);
      const f32 end   = ADJUST_SE_VOL(eTemp);
      DemoLib::Obj::CreateTask_SeCtrl( core->GetTaskSystem() , heap , start , end , frame , core->GetSoundObj(grpNo)->GetSoundId(),grpNo,DemoLib::Obj::SE_CTRL_VOLUME );
      Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
      t->Update(frameDif);
    }
  }
}

SEQPLAYER_CMD_DEF( SOUND_CTRL_PAN )
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	

  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  int sTemp,eTemp;
  file->LoadDataInt( &sTemp );
  file->LoadDataInt( &eTemp );

  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  //if( core->IsRecordStartSkip() == false )
  {
    if( frame == 0 || frameDif>=frame)
    {
      const f32 vol = ADJUST_SE_PAN(eTemp);
      Sound::ChangeSEVolume( core->GetSoundObj(grpNo)->GetSoundId() , vol , 0 , grpNo );
    }
    else
    {
      const f32 start = ADJUST_SE_PAN(sTemp);
      const f32 end   = ADJUST_SE_PAN(eTemp);
      DemoLib::Obj::CreateTask_SeCtrl( core->GetTaskSystem() , heap , start , end , frame , core->GetSoundObj(grpNo)->GetSoundId(),grpNo,DemoLib::Obj::SE_CTRL_PAN );
      Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
      t->Update(frameDif);
    }
  }
}

SEQPLAYER_CMD_DEF( SOUND_CTRL_PITCH )
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	

  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  int sTemp,eTemp;
  file->LoadDataInt( &sTemp );
  file->LoadDataInt( &eTemp );

  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  //if( core->IsRecordStartSkip() == false )
  {
    if( frame == 0 || frameDif>=frame)
    {
      const f32 vol = ADJUST_SE_PITCH(eTemp);
      Sound::ChangeSEVolume( core->GetSoundObj(grpNo)->GetSoundId() , vol , 0 , grpNo );
    }
    else
    {
      const f32 start = ADJUST_SE_PITCH(sTemp);
      const f32 end   = ADJUST_SE_PITCH(eTemp);
      DemoLib::Obj::CreateTask_SeCtrl( core->GetTaskSystem() , heap , start , end , frame , core->GetSoundObj(grpNo)->GetSoundId(),grpNo,DemoLib::Obj::SE_CTRL_PITCH );
      Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
      t->Update(frameDif);
    }
  }
}

SEQPLAYER_CMD_DEF( SOUND_CTRL_TEMPO )
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	

  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  int sTemp,eTemp;
  file->LoadDataInt( &sTemp );
  file->LoadDataInt( &eTemp );

  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  //if( core->IsRecordStartSkip() == false )
  {
    if( frame == 0 || frameDif>=frame)
    {
      const f32 vol = ADJUST_SE_TEMPO(eTemp);
      Sound::ChangeSEVolume( core->GetSoundObj(grpNo)->GetSoundId() , vol , 0 , grpNo );
    }
    else
    {
      const f32 start = ADJUST_SE_TEMPO(sTemp);
      const f32 end   = ADJUST_SE_TEMPO(eTemp);
      DemoLib::Obj::CreateTask_SeCtrl( core->GetTaskSystem() , heap , start , end , frame , core->GetSoundObj(grpNo)->GetSoundId(),grpNo,DemoLib::Obj::SE_CTRL_TEMPO );
      Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
      t->Update(frameDif);
    }
  }
}

SEQPLAYER_CMD_DEF( SOUND_PLAY_VOICE )
{
  const u32 grpNo = param.groupNo;
  int targetModel;    
  file->LoadDataInt( &targetModel );   
  
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	

  // Skip because of Command Skip?  
  if(!core->IsAfterCommandSkip(param.startFrame)) return;

  int mdlNum = 0;
  SequenceObject_Poke **mdlArr = core->GetPokeObjManager()->GetObjectArr(targetModel,&mdlNum);
  if( mdlNum != 1 ) return;  

  SequenceObject_Poke *poke = mdlArr[0];  

  int monsNo,formNo;

  poke->GetSimpleParam(monsNo,formNo);

  if(!poke->IsEgg())
    Sound::PlayVoice( grpNo , (MonsNo)monsNo , (FormNo)formNo );

}

SEQPLAYER_CMD_DEF( SOUND_PLAY_VOICENO )
{
  const u32 grpNo = param.groupNo;

  int monsNo,formNo;
  file->LoadDataInt( &monsNo );   
  file->LoadDataInt( &formNo );   

  Sound::PlayVoice( grpNo , (MonsNo)monsNo , (FormNo)formNo );

}

SEQPLAYER_CMD_DEF( SOUND_3D_SETPARAM )
{
  int priReducition;
  f32 panRange,sonic,interiaSize,maxVolDist,unitDist;

  file->LoadDataInt( &priReducition );
  file->LoadDataF32( &panRange );
  file->LoadDataF32( &sonic );
  file->LoadDataF32( &interiaSize );
  file->LoadDataF32( &maxVolDist );
  file->LoadDataF32( &unitDist );

  const gfl2::sound::SoundSystem::Sound3DParam sysParam =
  {
    priReducition,
    panRange,
    sonic
  };

  Sound::Set3DParam( sysParam );

  const gfl2::sound::SoundSystem::ListenerParam lisParam =
  {
    interiaSize,
    maxVolDist,
    unitDist,
  };
  Sound::SetListenerParam( lisParam );
}

SEQPLAYER_CMD_DEF( SOUND_3D_PLAY )
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	

  // Skip because of Command Skip?  
  if(!core->IsAfterCommandSkip(param.startFrame)) return;

  const u32 grpNo = param.groupNo;

  int seNo,player,tempPan,tempPitch,tempVol,isDebug;

  file->LoadDataInt( &seNo );
  file->LoadDataInt( &player );
  file->LoadDataInt( &tempVol );
  file->LoadDataInt( &tempPan );
  file->LoadDataInt( &tempPitch );
  file->LoadDataInt( &isDebug );

  GFL_ASSERT_MSG(grpNo<SEQ_EFF_SOUND_NUM,"3Dサウンドの管理数を超えています。[%d]\n",grpNo); //@check エディタ上の数値のチェック、確認時に必ず発覚する

  core->GetSoundObj(grpNo)->SetSoundId(seNo);
  core->GetSoundObj(grpNo)->SetIs3DSound(true);

  nw::snd::SoundStartable::StartInfo seInfo;
  if( player != -1 )
  {
    seInfo.enableFlag = nw::snd::SoundStartable::StartInfo::ENABLE_PLAYER_ID;
    seInfo.playerId = player;
  }

  //if( core->IsRecordStartSkip() == false )
  {
    const f32 pitch = ADJUST_SE_PITCH(tempPitch);
    const f32 pan = ADJUST_SE_PAN(tempPan);
    const f32 vol = ADJUST_SE_VOL(tempVol);
    Sound::Play3DActor( grpNo , seNo , &seInfo );
    Sound::Set3DActorVolume( grpNo , vol );
    Sound::Set3DActorPitch( grpNo , pitch );
    Sound::Set3DActorPan( grpNo , pan );
  }
}

SEQPLAYER_CMD_DEF( SOUND_3D_STOP )
{
  const u32 grpNo = param.groupNo;

  int fade;
  file->LoadDataInt( &fade );

  //if( core->IsRecordStartSkip() == false )
  {
    Sound::Stop3DActor( grpNo , fade );
  }
}

SEQPLAYER_CMD_DEF( SOUND_3D_MOVE_POSITION )
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  gfl2::math::Vector3 pos;
  bool isRelative;

  file->LoadDataVec3( &pos );
  file->LoadDataBool( &isRelative );

  gfl2::heap::HeapBase* heap = core->GetDevHeap();

  SequenceObject_Sound *obj = core->GetSoundObj(grpNo);

  if( isRelative == true )
  {
    pos += obj->GetPosition();
  }

  if( frame == 0 || frameDif>=frame)
  {
    obj->SetPosition(pos);
  }
  else
  {
    DemoLib::Obj::CreateTask_ObjectMove(core->GetTaskSystem(),heap,obj,obj->GetPosition(),pos,frame);
    Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
    t->Update(frameDif);
  }
}

SEQPLAYER_CMD_DEF( SOUND_3D_MOVE_RELATIVE_POKEMON )
{

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  int targetModel;  
  int trgNode;
  gfl2::math::Vector3 ofs;
  gfl2::math::Vector3 pos;

  file->LoadDataInt( &targetModel );    
  file->LoadDataInt( &trgNode );
  file->LoadDataVec3( &ofs );


  int mdlNum = 0;
  SequenceObject_Poke **mdlArr = core->GetPokeObjManager()->GetObjectArr(targetModel,&mdlNum);
  if( mdlNum != 1 ) return;  

  SequenceObject_Poke *poke = mdlArr[0];       
  SequenceObject_Sound *obj = core->GetSoundObj(grpNo);

  pos+=ofs;

  PokeTool::MODEL_NODE node;
  int nodeIdx = 0;
  poke->GetFollowNode(trgNode,&node,&nodeIdx);

  const bool ret = poke->GetNodePosition( node , nodeIdx , &pos );
  if( ret )
  {
    if( frame == 0 || frameDif>=frame)
    {
      obj->SetPosition(pos);
    }
    else 
    {
      DemoLib::Obj::CreateTask_ObjectMove(core->GetTaskSystem(),core->GetDevHeap(),obj,obj->GetPosition(),pos,frame);
      Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
      t->Update(frameDif);
    }
  }

}

SEQPLAYER_CMD_DEF( SOUND_3D_FOLLOW_POKEMON )
{
  
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  const u32 grpNo = param.groupNo;

  int targetModel;  
  int trgNode;
  gfl2::math::Vector3 ofs;
  gfl2::math::Vector3 pos;

  file->LoadDataInt( &targetModel );    
  file->LoadDataInt( &trgNode );
  file->LoadDataVec3( &ofs );

  SequenceObject_Sound *obj = core->GetSoundObj(grpNo);


  int mdlNum = 0;
  SequenceObject_Poke **mdlArr = core->GetPokeObjManager()->GetObjectArr(targetModel,&mdlNum);
  if( mdlNum != 1 ) return;  

  SequenceObject_Poke *poke = mdlArr[0];     
  PokeTool::MODEL_NODE node;
  int nodeIdx = 0;

  poke->GetFollowNode(trgNode,&node,&nodeIdx);

  
  obj->SetFollowPoke(poke,node,nodeIdx);
  obj->SetOffset(ofs);
  
}

SEQPLAYER_CMD_DEF( SOUND_3D_FOLLOW_OBJECT )
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  const u32 grpNo = param.groupNo;

  int trgType,trgIdx;
  gfl2::math::Vector3 ofs;

  file->LoadDataInt( &trgType );
  file->LoadDataInt( &trgIdx );
  file->LoadDataVec3( &ofs );

  SequenceObject_Sound *sndObj = core->GetSoundObj(grpNo);
  
  DemoLib::Obj::Object *obj = NULL;
  if( trgType == SEQ_DEF_FOLLOW_PARTICLE )
  {
    int mdlNum = 0;
    obj = core->GetEffectObjManager()->GetObjectArr(trgIdx,&mdlNum)[0];
  }
  else
    if( trgType == SEQ_DEF_FOLLOW_MODEL )
    {
      int mdlNum = 0;
      obj = core->GetModelObjManager()->GetObjectArr(trgIdx,&mdlNum)[0];
    }

    if( obj )
    {
      sndObj->SetFollowObject( obj ,NULL,true);
      sndObj->SetOffset( ofs );
    }
}

SEQPLAYER_CMD_DEF( SOUND_3D_PLAY_VOICE )
{
  const u32 grpNo = param.groupNo;

  int targetModel;    
  int tempPitch,tempVol;
  file->LoadDataInt( &targetModel );   
  file->LoadDataInt( &tempVol );
  file->LoadDataInt( &tempPitch );

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  
  // Skip because of Command Skip?  
  if(!core->IsAfterCommandSkip(param.startFrame)) return;

  int mdlNum = 0;
  SequenceObject_Poke **mdlArr = core->GetPokeObjManager()->GetObjectArr(targetModel,&mdlNum);
  if( mdlNum != 1 ) return;  

  SequenceObject_Poke *poke = mdlArr[0];  

  int monsNo,formNo;

  poke->GetSimpleParam(monsNo,formNo);

  const f32 pitch = ADJUST_SE_PITCH(tempPitch);
  const f32 vol = ADJUST_SE_VOL(tempVol);

  if(poke->IsEgg()) return;

  Sound::PlayVoice( grpNo , (MonsNo)monsNo , (FormNo)formNo );

  Sound::ChangeVoice3DActorVolume( grpNo , vol );
  Sound::ChangeVoice3DActorPitch( grpNo , pitch );
  
}

SEQPLAYER_CMD_DEF( SOUND_3D_PLAY_VOICENO )
{
  const u32 grpNo = param.groupNo;

  // Skip because of Command Skip?
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  if(!core->IsAfterCommandSkip(param.startFrame)) return;

  int monsNo,formNo;
  int tempPitch,tempVol;  
  file->LoadDataInt( &monsNo );   
  file->LoadDataInt( &formNo );   
  file->LoadDataInt( &tempVol );
  file->LoadDataInt( &tempPitch );

  const f32 pitch = ADJUST_SE_PITCH(tempPitch);
  const f32 vol = ADJUST_SE_VOL(tempVol);

  Sound::PlayVoice( grpNo , (MonsNo)monsNo , (FormNo)formNo );

  Sound::ChangeVoice3DActorVolume( grpNo , vol );
  Sound::ChangeVoice3DActorPitch( grpNo , pitch );

}

SEQPLAYER_CMD_DEF( SOUND_BGM_CHANGE )
{
	int bgmIdx,syncBGM;
	file->LoadDataInt(&bgmIdx); // BGM Index to play
  file->LoadDataInt( &syncBGM ); // Sync time with BGM?
	GFL_PRINT("\t\t--->Idx:%i  Sync: %i\n",bgmIdx,syncBGM);
	
	GFL_PRINT("Preload BGM Pos %i\n",Sound::GetStreamSoundPlaySamplePosition());

	SequenceViewSystem *core = (SequenceViewSystem*)userWork;
	
  if(syncBGM!=0)
  {
     ///  core->AddBGMSync(Sound::GetStreamSoundPlaySamplePosition(), bgmIdx,param.startFrame);
     core->SetSyncBGM(true);
  }
   
  if(param.startFrame==0)
  {
    int skipMilliSec = 0;
#if PM_DEBUG
    skipMilliSec = core->GetSkipMilliSec();

    if( skipMilliSec > core->GetMaxFrame()*1000/30 )
    {
      skipMilliSec = (core->GetMaxFrame()*1000/30) - 1000;
    }
#endif

    Sound::PushBGMReq();
    Sound::PrepareBGMReq(bgmIdx,skipMilliSec);
    core->SetBGMWait(bgmIdx);
  }
  else Sound::StartAppBGMReq(bgmIdx);

}




SEQPLAYER_CMD_DEF( SOUND_BGM_PLAY )
{
  int bgmIdx,syncBGM;
  file->LoadDataInt(&bgmIdx); // BGM Index to play
  file->LoadDataInt( &syncBGM ); // Sync time with BGM?
  GFL_PRINT("\t\t--->Idx:%i  Sync: %i\n",bgmIdx,syncBGM);

  GFL_PRINT("Preload BGM Pos %i\n",Sound::GetStreamSoundPlaySamplePosition());

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;

  if(syncBGM!=0) core->SetSyncBGM(true);

  if(param.startFrame==0)
  {    
    int skipMilliSec = 0;
#if PM_DEBUG
    skipMilliSec = core->GetSkipMilliSec();

    if( skipMilliSec > core->GetMaxFrame()*1000/30 )
    {
      skipMilliSec = (core->GetMaxFrame()*1000/30) - 1000;
    }
#endif

    Sound::PrepareBGMReq(bgmIdx,skipMilliSec);
    core->SetBGMWait(bgmIdx);    
  }
 
   else Sound::ChangeAppBGMReq(bgmIdx);
  
}

SEQPLAYER_CMD_DEF( SOUND_BGM_PLAY_FADE )
{
  int bgmIdx,syncBGM,outFade,inFade;
  file->LoadDataInt(&bgmIdx); // BGM Index to play
  file->LoadDataInt( &syncBGM ); // Sync time with BGM?
  file->LoadDataInt( &outFade ); // Fade Out Speed
  file->LoadDataInt( &inFade ); // Fade In Speed

  GFL_PRINT("\t\t--->Idx:%i  Sync: %i  FadeOut:%i  FadeIn:%i\n",bgmIdx,syncBGM,outFade,inFade);

  GFL_PRINT("Preload BGM Pos %i\n",Sound::GetStreamSoundPlaySamplePosition());

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;

  Sound::ChangeAppBGMReq(bgmIdx,(Sound::FadeFrame)outFade,(Sound::FadeFrame)inFade);

}

SEQPLAYER_CMD_DEF( SOUND_BGM_POP_PLAYNEW )
{
  int fadeOutSpeed,fadeInSpeed,bgmIdx;

  file->LoadDataInt(&fadeOutSpeed); // Now BGM Fade Out Speed
  file->LoadDataInt(&fadeInSpeed); // New BGM Fade In Speed
  file->LoadDataInt(&bgmIdx); // New BGM Index to play

  GFL_PRINT("\t\t--->Idx:%i  FadeIn: %i FadeOut:%i BGM:%i\n",fadeOutSpeed,fadeInSpeed,bgmIdx);

  // Fade Out Current BGM, don't restore stack BGM, Play new BGM Instead:
  Sound::EndBGMReq((Sound::FadeFrame)fadeOutSpeed);

  if(Sound::GetPushedBGMCount()>0)
  {
    Sound::PopBGMReq((Sound::FadeFrame)0);
  }
#if PM_DEBUG
  else
  {
    // BGMが積んでいない状況はデバッグモード以外ありえない
    SequenceViewSystem * core = static_cast<SequenceViewSystem*>(userWork);
    GFL_ASSERT(core->IsTestMode() == true);
  }
#endif
  Sound::StartBGMReq(bgmIdx,(Sound::FadeFrame)fadeInSpeed);

}


SEQPLAYER_CMD_DEF( SOUND_BGM_FADEOUT )
{
  int outFade;
  file->LoadDataInt( &outFade ); // Fade Out Speed
  
  GFL_PRINT("\t\t--->FadeOut:%i\n",outFade);
   
  Sound::FadeOutAppBGMReq((Sound::FadeFrame)outFade);
}


SEQPLAYER_CMD_DEF( SOUND_BGM_STOP )
{
  Sound::EndBGMReq();
}

SEQPLAYER_CMD_DEF( SOUND_BGM_RESTORE )
{
  if(Sound::GetPushedBGMCount()>0)
  {
      Sound::EndAppBGMReq();
  }
#if PM_DEBUG
  else
  {
    // BGMが積んでいない状況はデバッグモード以外ありえない
    SequenceViewSystem * core = static_cast<SequenceViewSystem*>(userWork);
    GFL_ASSERT(core->IsTestMode() == true);
  }
#endif
}

SEQPLAYER_CMD_DEF( SOUND_BGM_VOLUME_CHANGE )
{
  f32 volume;
  int frame;

  file->LoadDataF32(&volume); // Volume

  frame = param.endFrame - param.startFrame;
  frame *= 2; // Sound is 60FPS demo is 30FPS

  GFL_PRINT("\t\t--->Idx:%i  Volume: %f Frame:%i\n",volume,frame);

  // Change BGM Volume:
  Sound::ChangeBGMVolume(volume,(Sound::FadeFrame)frame);

}

SEQPLAYER_CMD_DEF( LABEL_SFX ) { }

SEQPLAYER_CMD_DEF( FADE_OUT ) 
{
	/*
	<FADE_WHITE value="0" comment="白" />
	<FADE_BLACK value="1" comment="黒" />
  <FADE_CROSS value="2" comment="CROSS" />
	*/
	int fadeColor; file->LoadDataInt(&fadeColor); 
	/*
	<FADE_DOUBLE value="0" comment="両画面"/>
	<FADE_UPPER value="1" comment="上画面"/>
	<FADE_LOWER value="2" comment="下画面"/>
	*/
	int fadeType; file->LoadDataInt(&fadeType); 
	const u32 frame = param.endFrame - param.startFrame;

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

	GFL_PRINT("\t\t--->Color:%i  Type: %i\n",fadeColor,fadeType);

	gfl2::math::Vector4 blackCol(0.0f,0.0f,0.0f,0.0f);	
	gfl2::math::Vector4 whiteCol(255.0f,255.0f,255.0f,0.0f);

	gfl2::math::Vector4 blackCol2(0.0f,0.0f,0.0f,255.0f);	
	gfl2::math::Vector4 whiteCol2(255.0f,255.0f,255.0f,255.0f);
	
	gfl2::math::Vector4 *col = &blackCol;
	gfl2::math::Vector4 *col2 = &blackCol2;
	if(fadeColor == 0) 
	{
		col = &whiteCol;
		col2 = &whiteCol2;
	}

  // Sequence is skipped to End:
  if(frameDif>=frame+50000 && core->IsRealTimeMode())
  {    
    return;
  }

  // CrossFade: (SPECIAL)
  if(fadeColor==2)
  {
#if PM_DEBUG // Make sure screen can fade in from debug player:
    if(core->IsTestMode())
    {
      switch(fadeType)
      {
      case 1:
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA,frame); break;
      case 2:
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA,frame); break;
      default:
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA,frame); 
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA,frame);
        break;
      }
    }
    else
#endif
    {
      switch(fadeType)
      {
      case 1:      
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_CROSS,frame);
        break;
      case 2:
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_CROSS,frame);
        break;
      default:
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_CROSS,frame);
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_CROSS,frame);
        break;
      }
    }

  
  }

  else
  {
	  switch(fadeType)
	  {
		  case 1:
			  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, col, col2,frame);
			  break;
		  case 2:
			  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA, col, col2,frame);
			  break;
		  default:
			  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, col, col2,frame);
			  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA, col, col2,frame);
			  break;
	  }
  }

  if(!core->IsRealTimeMode()) return;

  for(int n=0;n<frameDif;n++)
  {
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->Update();
  }
}

SEQPLAYER_CMD_DEF( FADE_IN ) 
{
	/*
	<FADE_DOUBLE value="0" comment="両画面"/>
	<FADE_UPPER value="1" comment="上画面"/>
	<FADE_LOWER value="2" comment="下画面"/>
	*/
	const u32 frame = param.endFrame - param.startFrame;
	int fadeType; file->LoadDataInt(&fadeType); 

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

	GFL_PRINT("\t\t--->Type: %i\n",fadeType);

  // Sequence is skipped to End:
  if(frameDif>=frame+50000 && core->IsRealTimeMode())
  {    
    return;
  }

	switch(fadeType)
	{
		case 1:
			GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA,frame); break;
		case 2:
			GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA,frame); break;
		default:
			GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA,frame); 
			GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA,frame);
			break;
	}

  if(!core->IsRealTimeMode()) return;

  for(int n=0;n<frameDif;n++)
  {
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->Update();
  }

}


SEQPLAYER_CMD_DEF( MOTIONBLUR_START )
{
  int blendType,animeType,displayType;
  f32 startBlend;

  file->LoadDataInt(&blendType); 
  file->LoadDataInt(&animeType);
  file->LoadDataInt(&displayType);
  file->LoadDataF32(&startBlend);

  GFL_PRINT("\t\t--->Type: %i,%i,%i StartBlend:%f\n",blendType,animeType,displayType,startBlend);

  static const System::MotionBlur::Param blurParam = 
  {
    System::MotionBlur::TYPE_SET,
    startBlend,
    1
  };

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  System::MotionBlur *motionBlur = core->GetMotionBlur();

  if(motionBlur)
  {
    motionBlur->RenderStart( static_cast<System::MotionBlur::BlendType>(blendType), static_cast<System::MotionBlur::AnimeType>(animeType),  System::GflUse::GetRenderTargetColor( gfl2::gfx::CtrDisplayNo::LEFT ), true );
    motionBlur->SetBlendControl( blurParam );

    GFL_PRINT("*Set End MotionBlur Frame: %i\n",param.endFrame);
    core->SetMotionBlurEndFrame(param.endFrame);
  }
  else
  {
    GFL_PRINT("\t\t--->XXX Motion Blur Not Initialized! XXX\n");
  }
}

SEQPLAYER_CMD_DEF( MOTIONBLUR_ADJUST )
{
  int type;
  f32 endBlend;

  file->LoadDataInt(&type); // static_cast<System::MotionBlur::Type>(type);
  file->LoadDataF32(&endBlend);

  System::MotionBlur::Param blurParam = 
  {
    static_cast<System::MotionBlur::Type>(type),
    endBlend,
    1
  };

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  System::MotionBlur *motionBlur = core->GetMotionBlur();
  
  if(motionBlur)
  {
    motionBlur->SetBlendControl( blurParam );
  }
  else
  {
    GFL_PRINT("\t\t--->XXX Motion Blur Not Initialized! XXX\n");
  }
}

SEQPLAYER_CMD_DEF( MOTIONBLUR_COLOR )
{
  int type;
  gfl2::math::Vector3 endColor;
  file->LoadDataInt(&type); // static_cast<System::MotionBlur::Type>(type);

  file->LoadDataVec3(&endColor);
  endColor.x /= 255.0f;
  endColor.y /= 255.0f;
  endColor.z /= 255.0f;

  System::MotionBlur::ColorParam blurParam = 
  {
    static_cast<System::MotionBlur::Type>(type),
    endColor,
    1
  };
  
  blurParam.frame = param.endFrame - param.startFrame;

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  System::MotionBlur *motionBlur = core->GetMotionBlur();

  if( motionBlur )
  {
    motionBlur->SetColorFilter( blurParam );
  }
  else
  {
    GFL_PRINT("\t\t--->XXX Motion Blur Not Initialized! XXX\n");
  }
}


SEQPLAYER_CMD_DEF( BLOOM_SETTING )
{
  int arcIdx;

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;

  const u32 frame = param.endFrame - param.startFrame;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  if(frameDif>=frame) return; // Too Late!

  gfl2::math::Vector3 weightRGB_start,weightRGB_end;
  f32 intensity_start,intensity_end;
  f32 range_start,range_end;
  f32 strength_start,strength_end;

  file->LoadDataInt(&arcIdx); // BloomMask file index

  file->LoadDataVec3( &weightRGB_start );
  file->LoadDataF32( &intensity_start );
  file->LoadDataF32( &range_start );
  file->LoadDataF32( &strength_start );

  file->LoadDataVec3( &weightRGB_end );
  file->LoadDataF32( &intensity_end );
  file->LoadDataF32( &range_end );
  file->LoadDataF32( &strength_end );

  GFL_PRINT("\t\t--->MaskFile:%i\n",arcIdx);
  GFL_PRINT("\t\t--->(Start) WeightRGB:%f,%f,%f  Intensity:%f Range:%f Strength:%f\n",weightRGB_start.x,weightRGB_start.y,weightRGB_start.z,intensity_start,range_start,strength_start);
  GFL_PRINT("\t\t--->(End) WeightRGB:%f,%f,%f  Intensity:%f Range:%f Strength:%f\n",weightRGB_end.x,weightRGB_end.y,weightRGB_end.z,intensity_end,range_end,strength_end);

  void *buf = core->GetResourceManager()->GetDataBuf(ARCID_DEMOSEQUENCE_RESOURCE,arcIdx);

  CreateTask_BloomAnimation(buf,core->GetTaskSystem(),core->GetDevHeap(),core,
    weightRGB_start,weightRGB_end,
    intensity_start,intensity_end,
    range_start,range_end,
    strength_start,strength_end,frame);

  if(frameDif>0)
  {  
    Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
    t->Update(frameDif);
  }


}


SEQPLAYER_CMD_DEF( FOG_SETTING )
{

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;

  const u32 frame = param.endFrame - param.startFrame;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  if(frameDif>=frame) return; // Too Late!

  gfl2::math::Vector3 colorRGB_start,colorRGB_end;
  f32 near_start,near_end;
  f32 far_start,far_end;
  f32 strength_start,strength_end;


  file->LoadDataVec3( &colorRGB_start );
  file->LoadDataF32( &near_start );
  file->LoadDataF32( &far_start );
  file->LoadDataF32( &strength_start );

  file->LoadDataVec3( &colorRGB_end );
  file->LoadDataF32( &near_end );
  file->LoadDataF32( &far_end );
  file->LoadDataF32( &strength_end );

  GFL_PRINT("\t\t--->(Start) ColorRGB:%f,%f,%f  Near:%f Far:%f Strength:%f\n",colorRGB_start.x,colorRGB_start.y,colorRGB_start.z,near_start,far_start,strength_start);
  GFL_PRINT("\t\t--->(End) ColorRGB:%f,%f,%f  Near:%f Far:%f Strength:%f\n",colorRGB_end.x,colorRGB_end.y,colorRGB_end.z,near_end,far_end,strength_end);


  CreateTask_FogAnimation(core->GetTaskSystem(),core->GetDevHeap(),core,
    colorRGB_start,colorRGB_end,
    near_start,near_end,
    far_start,far_end,
    strength_start,strength_end,frame);

  if(frameDif>0)
  {  
    Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
    t->Update(frameDif);
  }


}

SEQPLAYER_CMD_DEF( LABEL_BG) { }

SEQPLAYER_CMD_DEF( SKYBOX_CREATE) 
{
  int timeZone,weatherType;  
  f32 rot,scale;
  gfl2::math::Vector3 pos;

  file->LoadDataInt(&timeZone); 
  file->LoadDataInt(&weatherType);
  file->LoadDataF32( &rot );
  file->LoadDataF32( &scale );
  file->LoadDataVec3( &pos );  

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;

  core->SetSkyboxOffFrame(param.endFrame);

  int hour,min,sec;

  hour = min = sec = 0;

  switch(timeZone)
  {
    case 0: // MORNING:
      hour = 9;
      break;

    case 1: // NOON:
      hour = 12;
      break;

    case 2: // EVENING:
      hour = 17;
      min = 15;
      break;

    case 3: // NIGHT:
      hour = 21;
      break;

    case 4: // MIDNIGHT:
      hour = 24;
      break;

    case -1: // REAL-TIME:
      core->GetTime(hour,min,sec);
      break;
  }

  core->AddSkyBoxRendering(weatherType,scale,pos,hour,min,sec);
}

SEQPLAYER_CMD_DEF( LABEL_TIMING_AND_INPUT) { }

SEQPLAYER_CMD_DEF( SYSTEM_INPUT_CANCEL_ON )
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  core->SetCanSetSystemCancel(true);
}

SEQPLAYER_CMD_DEF( SYSTEM_INPUT_CANCEL_OFF )
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  core->SetCanSetSystemCancel(false);
}

SEQPLAYER_CMD_DEF( SYSTEM_TIMING_SYNC )
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;
  core->SetSystemSync(true);
}

SEQPLAYER_CMD_DEF( SYSTEM_KEYWAIT )
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;

  int type,exitOnFrameWait,exitFrameCount;

  file->LoadDataInt(&type); 
  file->LoadDataInt(&exitOnFrameWait);
  file->LoadDataInt(&exitFrameCount);

  GFL_PRINT("\t\t--->Type %i  ExitWait:%s  FrameCount:%i \n",type,(exitFrameCount?"true":"False"),exitFrameCount);

  core->SetSystemSync(true);

  if(type==1) // Text Window A Cursor Wait:
  {
    // Show Blinking wait cursor:
    core->SetWindowCursor(true);
  }

  core->SetKeyWait(type,exitOnFrameWait==1,exitFrameCount);
}

SEQPLAYER_CMD_DEF( SYSTEM_CALLBACK_COMMAND)
{
  int command,paramA,paramB;
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;

  file->LoadDataInt(&command); 
  file->LoadDataInt(&paramA); 
  file->LoadDataInt(&paramB); 

  core->ExecuteCallBack(command,paramA,paramB);
}

SEQPLAYER_CMD_DEF( COMMAND_SKIP )
{
  int executeType,transitionType,transitionFrameCount;

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;

  file->LoadDataInt(&executeType);
  file->LoadDataInt(&transitionType);
  file->LoadDataInt(&transitionFrameCount);

  GFL_PRINT("\t\t--->ExeType:%i  Trans:%i  Frame:%i\n",executeType,transitionType,transitionFrameCount);

  switch(executeType)
  {
  case 0: // Always:
    GFL_PRINT("\t\t--->Skip to frame:%i\n",param.endFrame);
    core->CommandSkip(param.endFrame);
    break;

  case 1: // Execute on cancel:
    core->SetCommandSkipOnCancel(executeType,param.endFrame,transitionFrameCount,transitionType);
    break;    

  case 2: // Execute on sync:
    core->SetCommandSkipOnSync(executeType,param.endFrame,transitionFrameCount,transitionType);
    break;    
  }


}


SEQPLAYER_CMD_DEF( LABEL_NIJI_COMMANDS) { }

SEQPLAYER_CMD_DEF( NIJI_BEADS_MODEL_LOAD) 
{
  int drawType;
  file->LoadDataInt(&drawType); 

}


SEQPLAYER_CMD_DEF( CHARAMODEL_CREATE_VSBATTLE )
{
  int trainerIndex,drawType;

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;

  file->LoadDataInt(&trainerIndex);
  file->LoadDataInt(&drawType);
  GFL_PRINT("\t\t--->Trainer Index:%i  DrawType:%i\n",trainerIndex,drawType);

  gfl2::heap::HeapBase* heap = core->GetDevHeap();  

  // Frame Control:
  if(core->GetTargetFrame()>=param.endFrame) return; // Too late!
  const u32 frame = param.endFrame - core->GetTargetFrame();
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  SequenceObject_Chara *chara = GFL_NEW(heap)SequenceObject_Chara(core,core->GetRenderSys()->GetRenderingPipeLine(),core->GetCharaModelFactoryBattle(),core->GetDressUpModelResourceManagerBattle(trainerIndex));

  DemoLib::Obj::Object_Chara::LOAD_PARAM paramChara;
  paramChara.charaId = core->GetVSBattleModelID(trainerIndex);

  paramChara.dressParam = core->GetDressUpParamVSBattle(trainerIndex);

  chara->SetCreateFrame(param.startFrame);
  chara->CreateNoLoad( paramChara , core->GetMemHeap() , core->GetGLAllocator() );
  chara->AddRenderPath(drawType);
  chara->ChangeAnimation(core->GetVSBattleStartAnimation(trainerIndex));

  if(core->GetVSBattleStartAnimation(trainerIndex) == P1_BASE_BA023_VSWAIT04)
  {        
    // 自動目パチ無効
    chara->GetCharaModel()->SetAutoBlinkMode(poke_3d::model::CharaModel::AUTO_BLINK_MODE_NONE);

    // 自動目パチ更新
    chara->GetCharaModel()->UpdateEye();
  }
  else
  {
    // 自動目パチ開始：バトル用
    chara->GetCharaModel()->SetAutoBlinkMode(poke_3d::model::CharaModel::AUTO_BLINK_MODE_BATTLE);

    // 自動目パチ更新
    chara->GetCharaModel()->UpdateEye();

    // Set Update Eye once per frame:
    chara->SetUpdateEye(true);
  }

  chara->SetAnimationIsLoop(true);
  chara->SetIsBattleModel();
  chara->UpdateAnimation(frameDif);

  //momiji追加処理　モーションをランダムにずらす

  const f32 maxFrame = chara->GetCharaModel()->GetAnimationEndFrame();
  if (maxFrame != 0)
  {
    const s32 frame = System::GflUse::GetPublicRand((u32)maxFrame);
    chara->SetAnimationFrame((f32)frame);
  }


  //  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int objNum = 1;

  SequenceObject_Chara **objArr = GFL_NEW_ARRAY(heap)SequenceObject_Chara*[objNum];	
  core->GetCharaObjManager()->SetObjectArr(objArr,objNum,grpNo);

  for( int i=0;i<objNum;i++)
  {
    objArr[i] = chara;
    core->GetObjectManager()->AddObject(objArr[i]);    
    DemoLib::Obj::CreateTask_DeleteObject_Chara(core->GetTaskSystem(),heap,objArr[i],frame);
  }


}

SEQPLAYER_CMD_DEF( LAYOUT_VSBATTLE_TRAINERSETUP )
{
  int trainerIndex;

  const u32 grpNo = param.groupNo;
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;

  file->LoadDataInt(&trainerIndex);
  GFL_PRINT("\t\t--->Trainer Index:%i\n",trainerIndex);


  int objNum;
  SequenceObject_Layout **objArr = core->GetLayoutObjManager()->GetObjectArr(grpNo,&objNum);

  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Layout *obj = objArr[i];

    if(trainerIndex<4)
      obj->VSBattleSet(trainerIndex);  
    else
      obj->RoyalBattleSet(trainerIndex-4);  
  }

}

SEQPLAYER_CMD_DEF( LAYOUT_DENDOUDEMO_POKEMONSETUP )
{
  const u32 grpNo = param.groupNo;
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;

  int objNum;
  SequenceObject_Layout **objArr = core->GetLayoutObjManager()->GetObjectArr(grpNo,&objNum);

  for( int i=0;i<objNum;i++)
  {
    SequenceObject_Layout *obj = objArr[i];
    obj->DendouDemoSet();
  }

}


SEQPLAYER_CMD_DEF( LABEL_OTHER) { }

SEQPLAYER_CMD_DEF( MESSAGE_DISPLAY)
{  
  int messageID,windowType;
  file->LoadDataInt(&messageID); 
  file->LoadDataInt(&windowType); 

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;

  core->DisplayTextWindowMessage(messageID,param.endFrame,windowType);
}

SEQPLAYER_CMD_DEF( MESSAGE_SETTING)
{  
  int messageSpeed,buttonSpeedUp;
  file->LoadDataInt(&messageSpeed); 
  file->LoadDataInt(&buttonSpeedUp); 

  SequenceViewSystem *core = (SequenceViewSystem*)userWork;

  core->TextWindowMessageSetting(messageSpeed,buttonSpeedUp!=0);
}

SEQPLAYER_CMD_DEF( MESSAGE_ARCFILE_SETTING)
{
  // Do nothing! (ARC Setting is Pre-Loaded)
}

SEQPLAYER_CMD_DEF( MESSAGE_ARCFILE_SETTING_FIELD)
{
  // Do nothing! (ARC Setting is Pre-Loaded)
}

SEQPLAYER_CMD_DEF( DUMMY_LABEL) { }

SEQPLAYER_CMD_DEF( POKEMON_INFRAME_FOLLOW_ROTATESETTING )
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;	
  
  int  index,setting;  
  file->LoadDataInt(&index);
  file->LoadDataInt( &setting );     
  core->Inframe_UseFollowRotate(index,setting!=0);
}

SEQPLAYER_CMD_DEF(NEW_BLOOM_START)
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;

  int arcIdx;

  gfl2::math::Vector3 weightRGB;
  f32 intensity;
  f32 range;
  f32 strength;

  file->LoadDataInt(&arcIdx); // BloomMask file index

  file->LoadDataVec3(&weightRGB);
  file->LoadDataF32(&intensity);
  file->LoadDataF32(&range);
  file->LoadDataF32(&strength);

  void *buf = core->GetResourceManager()->GetDataBuf(ARCID_DEMOSEQUENCE_RESOURCE, arcIdx);

  core->StartBloom(buf);
  core->GetRenderSys()->GetRenderingPipeLine()->SetBloomParameters(
    weightRGB,intensity, range, strength);

}

SEQPLAYER_CMD_DEF(NEW_BLOOM_CHANGE)
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;

  const u32 frame = param.endFrame - param.startFrame;
  const u32 frameDif = core->GetTargetFrame() - param.startFrame;

  if (frameDif >= frame) return; // Too Late!

  gfl2::math::Vector3 weightRGB_start, weightRGB_end;
  f32 intensity_start, intensity_end;
  f32 range_start, range_end;
  f32 strength_start, strength_end;

  file->LoadDataVec3(&weightRGB_start);
  file->LoadDataF32(&intensity_start);
  file->LoadDataF32(&range_start);
  file->LoadDataF32(&strength_start);

  file->LoadDataVec3(&weightRGB_end);
  file->LoadDataF32(&intensity_end);
  file->LoadDataF32(&range_end);
  file->LoadDataF32(&strength_end);

  CreateTask_BloomAnimationParam(core->GetTaskSystem(), core->GetDevHeap(), core,
    weightRGB_start, weightRGB_end,
    intensity_start, intensity_end,
    range_start, range_end,
    strength_start, strength_end, frame);

  if (frameDif > 0)
  {
    Obj::Task* t = core->GetTaskSystem()->GetLastCreatedTask();
    t->Update(frameDif);
  }
}

SEQPLAYER_CMD_DEF(NEW_BLOOM_END)
{
  SequenceViewSystem *core = (SequenceViewSystem*)userWork;

  core->EndBloom();
}

#undef SEQPLAYER_CMD_DEF

	GFL_NAMESPACE_END(Sequence)
GFL_NAMESPACE_END(DemoLib)


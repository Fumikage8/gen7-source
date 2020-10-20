//============================================================================================
/**
 * @file		sv_box.cpp
 * @brief		ボックス画面セーブデータ
 * @author	Hiroyuki Nakamura
 * @date		12.02.15
 */
//============================================================================================
#include "Savedata/include/BoxSave.h"
#include "arc_index/message.gaix"
#include "niji_conv_header/message/msg_boxname.h"
#include "Print/include/PrintSystem.h"


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace	savedata
 * @brief			セーブデータ
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
GFL_NAMESPACE_BEGIN(Savedata)
  
//------------------------------------------------------------------------------------------
/**
 * @brief		コンストラクタ
 */
//------------------------------------------------------------------------------------------
BOX::BOX(void)
{
  //初期化
	gfl2::std::MemClear( &box, sizeof(BoxData) );

}

//------------------------------------------------------------------------------------------
/**
 * @brief		デストラクタ
 */
//------------------------------------------------------------------------------------------
BOX::~BOX()
{
}

//------------------------------------------------------------------------------------------
/**
 * @brief		初期データ設定
 *
 * @param		heap		ヒープ
 *
 * @return	none
 */
//------------------------------------------------------------------------------------------
void BOX::InitBoxData( gfl2::heap::HeapBase * heap )
{
	gfl2::std::MemClear( &box, sizeof(BoxData) );
  int i;

  
	// ボックス名
	gfl2::str::MsgData * mman = GFL_NEW_LOW( heap ) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_boxname_DAT, heap, gfl2::str::MsgData::LOAD_FULL );
	gfl2::str::StrBuf * str = GFL_NEW_LOW( heap ) gfl2::str::StrBuf( TRAY_NAME_BUFSIZE, heap );
	// ボックス名
  for(  i=0; i<TRAY_MAX; i++ ){
		mman->GetString( i, *str );
		str->PutStr( box.trayName[i], TRAY_NAME_BUFSIZE );
	}
  //チーム名
	for(  i=0; i<TEAM_NUM_MAX; i++ ){
		mman->GetString( msg_team_name_01+i, *str );
		str->PutStr( box.teamName[i], TEAM_NAME_BUFSIZE );
	}

  GFL_DELETE	str;
	GFL_DELETE	mman;
	// 壁紙番号
	for( i=0; i<TRAY_MAX; i++ ){
		box.wallPaper[i] = i % NORMAL_WALLPAPER_MAX;
	}
	box.extraWallPaperBitFlag = 0;							// 追加壁紙フラグ
	box.eventTrayOpenFlag     = 0;							// 隠しトレイを公開するフラグ
	box.trayMax               = TRAY_NUM_1ST;		// 解放されているトレイ数
	box.currentTray           = 0;							// 参照中のトレイ番号


  for(int team=0;team < TEAM_MAX;team++){
    for(int num=0;num < TEAM_NUM_MAX;num++){
      box.boxLockPos[team][num] = TEAM_BOX_POKEMON_ENTRY_NONE;
    }
  }

  for(int team=0;team < TEAM_MAX;team++){
    SetTeamLock( team, false );
  }

}




//------------------------------------------------------------------------------------------
/**
 * @brief		ポケモン入れ替え
 *
 * @param		tray1		トレイ番号１
 * @param		pos1		トレイ内位置１
 * @param		tray2		トレイ番号２
 * @param		pos2		トレイ内位置２
 * @attention  ロックの場所を変更している
 * @return	none
 */
//------------------------------------------------------------------------------------------
void BOX::ChangePokemon( u32 tray1, u32 pos1, u32 tray2, u32 pos2 )
{
  u16 t1mix =(tray1<<8) + pos1;
  u16 t2mix =(tray2<<8) + pos2;
  int i,j;

  for( i=0;i<TEAM_MAX;i++)
  {
    for( j=0;j<TEAM_NUM_MAX;j++)
    {
      if(box.boxLockPos[ i ][ j ]==TEAM_BOX_POKEMON_ENTRY_NONE){
      }
      else if(box.boxLockPos[ i ][ j ] == t1mix)
      {
        box.boxLockPos[ i ][ j ] = t2mix;
      }
      else if(box.boxLockPos[ i ][ j ] == t2mix)
      {
        box.boxLockPos[ i ][ j ] = t1mix;
      }
    }
  }
}



//------------------------------------------------------------------------------------------
/**
 * @brief		チームの入れ替え
 *
 * @param		tray1		トレイ番号１
 * @param		tray2		トレイ番号２
 * @return	none
 */
//------------------------------------------------------------------------------------------
void BOX::ChangeTeam( u32 tray1, u32 tray2 )
{
  u16 t1num =(tray1<<8);
  u16 t2num =(tray2<<8);
  int i,j;

  for( i=0;i<TEAM_MAX;i++)
  {
    for( j=0;j<TEAM_NUM_MAX;j++)
    {
      if((box.boxLockPos[ i ][ j ] & 0xff00) == t1num)
      {
        u16 num = box.boxLockPos[ i ][ j ] & 0xff;
        box.boxLockPos[ i ][ j ] = num + t2num;
      }
      else if((box.boxLockPos[ i ][ j ] & 0xff00) == t2num)
      {
        u16 num = box.boxLockPos[ i ][ j ] & 0xff;
        box.boxLockPos[ i ][ j ] = num + t1num;
      }
    }
  }
}



//------------------------------------------------------------------------------------------
/**
 * @brief		トレイ入れ替え
 *
 * @param		tray1		トレイ番号１
 * @param		tray2		トレイ番号２
 * @param		heap		ヒープ
 *
 * @return	none
 */
//------------------------------------------------------------------------------------------
void BOX::ChangeTray( u32 tray1, u32 tray2, gfl2::heap::HeapBase * heap )
{
	// ボックス名
	u32	size = sizeof(gfl2::str::STRCODE) * TRAY_NAME_BUFSIZE;
	gfl2::str::STRCODE * tn1 = box.trayName[tray1];
	gfl2::str::STRCODE * tn2 = box.trayName[tray2];
	gfl2::str::STRCODE * tmp = reinterpret_cast<gfl2::str::STRCODE *>( GflHeapAllocMemoryLow(heap,size) );
	gfl2::std::MemCopy( tn1, tmp, size );
	gfl2::std::MemCopy( tn2, tn1, size );
	gfl2::std::MemCopy( tmp, tn2, size );
	GflHeapFreeMemory( tmp );

	// 壁紙番号
	u8	tmpWP = box.wallPaper[tray1];
	box.wallPaper[tray1] = box.wallPaper[tray2];
	box.wallPaper[tray2] = tmpWP;

  //チームの付け替え
  ChangeTeam(tray1, tray2);
}

//------------------------------------------------------------------------------------------
/**
 * @brief		トレイ名設定
 *
 * @param		tray	トレイ番号
 *
 * @return	none
 */
//------------------------------------------------------------------------------------------
void BOX::SetTrayName( const gfl2::str::STRCODE * str, u32 tray )
{
	gfl2::std::MemCopy( str, box.trayName[tray], sizeof(gfl2::str::STRCODE)*TRAY_NAME_BUFSIZE );
}

//------------------------------------------------------------------------------------------
/**
 * @brief		トレイ名取得
 *
 * @param		tray	トレイ番号
 *
 * @return	トレイ名
 */
//------------------------------------------------------------------------------------------
const gfl2::str::STRCODE * BOX::GetTrayName( u32 tray ) const
{
	return box.trayName[tray];
}


//------------------------------------------------------------------------------------------
/**
 * @brief		壁紙設定
 *
 * @param		tray	トレイ番号
 * @param		id		壁紙ID
 *
 * @return	none
 */
//------------------------------------------------------------------------------------------
void BOX::SetWallPaper( u32 tray, u32 id )
{
	box.wallPaper[tray] = id;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		壁紙取得
 *
 * @param		tray	トレイ番号
 *
 * @return	壁紙ID
 */
//------------------------------------------------------------------------------------------
u32 BOX::GetWallPaper( u32 tray ) const
{
	return box.wallPaper[tray];
}


//------------------------------------------------------------------------------------------
/**
 * @brief		追加壁紙フラグ設定
 *
 * @param		flg		追加壁紙フラグ
 *
 * @return	none
 */
//------------------------------------------------------------------------------------------
void BOX::SetExWallPaper( u32 flg )
{
	box.extraWallPaperBitFlag |= flg;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		追加壁紙が使用できるかを調べる
 *
 * @param		flg		追加壁紙フラグ
 *
 * @retval	"true = 使用可"
 * @retval	"false = 使用不可"
 */
//------------------------------------------------------------------------------------------
bool BOX::CheckExWallPaper( u32 flg ) const
{
	if( ( box.extraWallPaperBitFlag & flg ) != 0 ){
		return true;
	}
	return false;
}


//------------------------------------------------------------------------------------------
/**
 * @brief		使用可能なトレイ数を設定
 *
 * @param		max		トレイ数
 *
 * @return	none
 */
//------------------------------------------------------------------------------------------
void BOX::SetTrayMax( u32 max )
{
	box.trayMax = max;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		使用可能なトレイレベルを取得
 *
 * @param		none
 *
 * @return	トレイレベル0-3
 */
//------------------------------------------------------------------------------------------
u32 BOX::GetTrayLevel(void) const
{
  switch(box.trayMax)
  {
  case TRAY_NUM_1ST:
    return 0;
  case TRAY_NUM_2ND:
    return 1;
  case TRAY_NUM_3RD:
    return 2;
  case TRAY_NUM_4TH:
    return 3;
  default:
    break;
  }
  return 3;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		使用可能なトレイ数を取得
 *
 * @param		none
 *
 * @return	トレイ数
 */
//------------------------------------------------------------------------------------------
u32 BOX::GetTrayMax(void) const
{
	return box.trayMax;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		チーム名取得
 *
 * @param		tray	トレイ番号
 *
 * @return	トレイ名
 */
//------------------------------------------------------------------------------------------
const gfl2::str::STRCODE * BOX::GetTeamName( u32 team ) const
{
  return box.teamName[team];
}

//------------------------------------------------------------------------------------------
/**
 * @brief		チームポケモンの場所取得
 *
 * @param		team	チーム番号
 * @param		no	  パーティーの順番
 * @return	上位がＢＯＸの場所番号、下位がＢＯＸでのトレイ位置
 */
//------------------------------------------------------------------------------------------
u16  BOX::GetTeamPokePos( u32 team, u32 no ) const
{
  GFL_ASSERT(team < TEAM_MAX);
  GFL_ASSERT(no < TEAM_NUM_MAX);
  return box.boxLockPos[team][no];
}


//------------------------------------------------------------------------------------------
/**
 * @brief		チームポケモンが入っているかどうか取得
 * @param		team	チーム番号
 * @return	入っている場合 true 入ってない場合 false
 */
	//------------------------------------------------------------------------------------------
bool BOX::IsTeamPokeIn( u32 team ) const
{
  for(int num=0;num < TEAM_NUM_MAX;num++){
    if(box.boxLockPos[team][num] != TEAM_BOX_POKEMON_ENTRY_NONE){
      return true;
    }
  }
  return false;
}


//------------------------------------------------------------------------------------------
/**
 * @brief		自分が何処かのチームにいるかどうか
 *
 * @param		tray	トレイ番号
 * @param		pos	  位置
 * @return	いたらtrue
 */
//------------------------------------------------------------------------------------------
int BOX::IsTeamInMe( u32 tray, u32 pos ) const
{
  u16 no = (tray<<8)+pos;
  

  for(int team=0;team < TEAM_MAX;team++){
    for(int num=0;num < TEAM_NUM_MAX;num++){
      if(box.boxLockPos[team][num] == no){
        return team;
      }
    }
  }
  return -1;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		自分が指定のチームにいるかどうか
 * @param		tray	トレイ番号
 * @param		pos	  位置
 * @param		team	  チーム番号
 * @return	いたらtrue
 */
//------------------------------------------------------------------------------------------
int BOX::IsTeamInMe( u32 tray, u32 pos , u32 team) const
{
  u16 no = (tray<<8)+pos;

  for(int num=0;num < TEAM_NUM_MAX;num++){
    if(box.boxLockPos[team][num] == no){
      return team;
    }
  }
  return -1;
}


//------------------------------------------------------------------------------------------
/**
 * @brief		チームから消す
 *
 * @param		tray	トレイ番号
 * @param		pos	  位置
 */
//------------------------------------------------------------------------------------------
void BOX::DeleteTeam( u32 tray, u32 pos )
{
  u16 no = (tray<<8)+pos;

  for(int team=0;team < TEAM_MAX;team++){
    for(int num=0;num < TEAM_NUM_MAX;num++){
      if(box.boxLockPos[team][num] == no){
        box.boxLockPos[team][num]=TEAM_BOX_POKEMON_ENTRY_NONE;
      }
    }
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief		チームを詰める
 * @param		team チーム番号
 */
//------------------------------------------------------------------------------------------
void BOX::PackTeam( u32 teamno )
{
  for(int x=0;x<Savedata::BOX::TEAM_NUM_MAX-1;x++){

    for(int j=1;j < Savedata::BOX::TEAM_NUM_MAX; j++){
      u16 nox = GetTeamPokePos( teamno, j-1 );
      if(nox == Savedata::BOX::TEAM_BOX_POKEMON_ENTRY_NONE){
        u16 no = GetTeamPokePos( teamno, j );
        SetTeamPokePos( teamno, j-1, no);
        SetTeamPokePos( teamno, j, Savedata::BOX::TEAM_BOX_POKEMON_ENTRY_NONE);
      }
    }
  }
}


//------------------------------------------------------------------------------------------
/**
 * @brief		チームがロック状態かどうか
 *
 * @param		team	チーム番号
 *
 * @return	ロックしていたらtrue
 */
//------------------------------------------------------------------------------------------
bool BOX::IsTeamLock( u32 team ) const
{
  GFL_ASSERT(team < TEAM_MAX);
  if(box.teamLock[team]){
    return true;
  }
  return false;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		自分がロック状態かどうか
 * @param		tray	トレイ番号
 * @param		pos	  位置
 * @return	ロックしていたらtrue
 */
//------------------------------------------------------------------------------------------
bool BOX::IsMeLock( u32 tray, u32 pos ) const
{
  for(int team=0;team < TEAM_MAX;team++){
    if(-1 != IsTeamInMe(tray,pos,team)){
      if(IsTeamLock(team)){
        return true;
      }
    }
  }
  return false;
}




//------------------------------------------------------------------------------------------
/**
 * @brief		チーム名設定
 * @param		str   チーム名
 * @param		team	チーム番号
 */
//------------------------------------------------------------------------------------------
void BOX::SetTeamName( const gfl2::str::STRCODE * str, u32 team )
{
  GFL_ASSERT(team < TEAM_MAX);
  gfl2::std::MemCopy( str, box.teamName[team], sizeof(gfl2::str::STRCODE)*TEAM_NAME_LEN );
  box.teamName[team][TEAM_NAME_LEN] = gfl2::str::EOM_CODE;
}


//------------------------------------------------------------------------------------------
/**
 * @brief		チームポケモンの場所設定
 *
 * @param		team	チーム番号
 * @param		no	  パーティーの順番
 * @param	  pos   上位がＢＯＸの場所番号、下位がＢＯＸでのトレイ位置
 * @param   重複していた場所
 */
//------------------------------------------------------------------------------------------
int BOX::SetTeamPokePos( u32 team,u32 no, u16 pos )
{
  int i;
  GFL_ASSERT(team < TEAM_MAX);
  GFL_ASSERT(no < TEAM_NUM_MAX);
  for( i=0;i<TEAM_NUM_MAX;i++){
    if(box.boxLockPos[team][i] == pos){
      box.boxLockPos[team][i] = TEAM_BOX_POKEMON_ENTRY_NONE;  //同じものは消す
      break;
    }
  }
  box.boxLockPos[team][no] = pos;
  return i;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		チームがロック状態かどうか設定
 * @param		team	チーム番号
 * @param	  onoff ロックしていたらtrue
 */
//------------------------------------------------------------------------------------------
void BOX::SetTeamLock( u32 team,bool onoff )
{
  GFL_ASSERT(team < TEAM_MAX);
  if(onoff){
    box.teamLock[team] = 1;
  }
  else{
    box.teamLock[team] = 0;
  }
}


//------------------------------------------------------------------------------------------
/**
 * @brief		ロックチームの番号を得る
 * @return		team	チーム番号
 */
//------------------------------------------------------------------------------------------
/*
u8 BOX::GetTeamLockNo(void)
{
  return box.teamLockNo;
}
*/

//------------------------------------------------------------------------------------------
/**
 * @brief		カレントトレイを設定
 *
 * @param		tray	トレイ番号
 *
 * @return	none
 *
 * @li	バトルボックスも設定可 ( tray = BATTLE_BOX_TRAY_NUMBER )
 */
//------------------------------------------------------------------------------------------
void BOX::SetCurrentTray( u32 tray )
{
	box.currentTray = tray;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		カレントトレイを取得
 *
 * @param		none
 *
 * @return	カレントトレイ
 *
 * @li	バトルボックスが設定されている場合があるので注意！
 * @li	戻り値で直接ボックスのポケモンを参照しないでください
 */
//------------------------------------------------------------------------------------------
u32 BOX::GetCurrentTray(void) const
{
	return box.currentTray;
}


//------------------------------------------------------------------------------------------
/**
 * @brief		チュートリアルが終わったかどうか
 * @return	おわっていたらtrue
 */
//------------------------------------------------------------------------------------------
//bool BOX::IsTutorialEnd( void )  //GF調整要望[367]
//{
//  return box.tutorial;
//}

//------------------------------------------------------------------------------------------
/**
 * @brief		チュートリアルが終わった
 */
//------------------------------------------------------------------------------------------
//void BOX::SetTutorialEnd( void )//GF調整要望[367]
//{
//  box.tutorial=true;
//}


GFL_NAMESPACE_END(Savedata)

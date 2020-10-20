//-----------------------------------------------------------------------------
/**
 * @file    hand_zenryoku.h
 * @brief   ポケモンXY バトルシステム イベントファクター [全力技]
 * @author  obata_toshihiro
 * @date    2015.06.04  作成
 */
//-----------------------------------------------------------------------------
#ifndef HAND_ZENRYOKU_H_INCLUDED
#define HAND_ZENRYOKU_H_INCLUDED
#pragma once

namespace btl {

  class BTL_POKEPARAM;


  namespace handler {
    namespace zenryoku {

      
      //-----------------------------------------------------------------------
      /**
       * @brief 全力技効果に対するイベントファクターをシステムに追加する
       *
       * @param poke   全力技を発動するポケモン
       * @param effect 全力技効果
       *
       * @retval true   ハンドラを追加した
       * @retval false  ハンドラを追加できなかった
       */
      //-----------------------------------------------------------------------
      extern bool Add( const BTL_POKEPARAM* poke, ZenryokuWazaEffect effect );


    } // namespace zenryoku
  } // namespace handler
} // namespace btl

#endif // HAND_ZENRYOKU_H_INCLUDED

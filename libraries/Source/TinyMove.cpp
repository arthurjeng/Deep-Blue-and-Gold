// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#include "Definitions.h"
#include "Constants.h"
#include "Utilities.h"

#if (IsDevHost)
#include <cassert>
#endif

#include "Board.h"
#include "Move.h"
#include "TinyMove.h"

void TinyMove::Flip(void)
{
  const sqrType frsqr = GetFrSqr(), tosqr = GetToSqr();
  const mscType msc = GetMsc();

  BuildB0(OtherSqr(frsqr), OtherSqr(tosqr)); BuildB1(OtherSqr(tosqr), msc);
}

void TinyMove::Print(void) const
{
  const mscType msc = GetMsc();

  PrintSqrBasic(GetFrSqr()); PrintSqrBasic(GetToSqr());
  if (IsMscProm(msc)) PrintPiece(cvpromtopiece[cvmsctoprom[msc]]);
}

void TinyMove::ConvertFromMove(const Move& move)
{
  const sqrType frsqr = move.GetFrSqr(), tosqr = move.GetToSqr();
  const mscType msc = move.GetMsc();

  BuildB0(frsqr, tosqr); BuildB1(tosqr, msc);
}

void TinyMove::ConvertToMove(Move& move, const Board& board) const
{
  move.Reset();
  move.PutFrSqr(GetFrSqr()); move.PutFrMan(board.GetMan(move.GetFrSqr()));
  move.PutToSqr(GetToSqr()); move.PutToMan(board.GetMan(move.GetToSqr()));
  move.PutMsc(GetMsc());
}

si TinyMove::CompareTinyMove(const TinyMove& tinymove0, const TinyMove& tinymove1)
{
  // Compare two tiny moves, like strcmp()

  si compare = 0;
  ui index = 0;

  while ((compare == 0) && (index < tmcbLen))
  {
    const ui8 cb0 = tinymove0.GetCodeByte(index), cb1 = tinymove1.GetCodeByte(index);

    if (cb0 == cb1) index++; else compare = (cb0 < cb1) ? -1 : 1;
  };
  return compare;
}

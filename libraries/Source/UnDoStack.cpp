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
#include "FEnv.h"
#include "Move.h"
#include "UnDo.h"
#include "UnDoStack.h"

void UnDoStack::Reset(void)
{
  count = 0;
  for (miType index = 0; index < UndoStackLen; index++) undostack[index].Reset();
}

void UnDoStack::Push(const FEnv& fenv, const Move& move, const tidType capttid)
{
  if (count == UndoStackLen)
  {
    for (miType index = 1; index < UndoStackLen; index++)
      undostack[index - 1] = undostack[index];
    count--;
  };
  undostack[count].RefFEnv() = fenv; undostack[count].RefMove() = move;
  undostack[count].PutCaptTid(capttid);
  count++;
}

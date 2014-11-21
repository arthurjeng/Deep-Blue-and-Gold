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

#include "Counter.h"
#include "Board.h"
#include "FEnv.h"
#include "FPos.h"
#include "Move.h"
#include "UnDo.h"
#include "UnDoStack.h"
#include "ML.h"
#include "Hash.h"
#include "History.h"
#include "TBV.h"
#include "MGS.h"
#include "PEnv.h"
#include "Pos.h"
#include "PVTable.h"
#include "PIR.h"
#include "Search.h"
#include "State.h"
#include "CIB.h"
#include "ICP.h"

void ICP::Test(void) const
{
#if (1)
  // Play an indefinite series of games

  bool done = false;

  while (!done)
  {
    stateptr->NewGame(); CondShowBoard();
    while (!IsGameOver() && !stateptr->TestInterrupt()) ProgMove();
    if (stateptr->TestInterrupt())
    {
      PrintFS(fsMsInterrupt); stateptr->ResetInterrupt();
      stateptr->NewGame(); CondShowBoard(); done = true;
    };
  };
#endif
}

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
#include <csignal>
#include <iosfwd>
#endif

#include "Counter.h"
#include "Board.h"
#include "SS.h"
#include "SSKnight.h"
#include "SSKing.h"
#include "FEnv.h"
#include "FPos.h"
#include "Score.h"
#include "Window.h"
#include "CNPair.h"
#include "Move.h"
#include "UnDo.h"
#include "UnDoStack.h"
#include "TinyMove.h"
#include "SAN.h"
#include "ML.h"
#include "Hash.h"
#include "BookMove.h"
#include "Book.h"
#include "History.h"
#include "TBV.h"
#include "MGS.h"
#include "PEnv.h"
#include "Pos.h"
#include "PVTable.h"
#include "PIR.h"
#include "Search.h"
#include "State.h"
#include "BBMoveNode.h"
#include "BBPosNode.h"
#include "BB.h"
#include "CIB.h"
#include "ICP.h"
#include "XCP.h"
#include "Myopic.h"

static Myopic myopic;
static acsType acs = acsInit;

// User interrupt handler

static void Interrupt(int sigval)
{
  myopic.Interrupt();
  sigval = sigval;
}

// Entry for one time initialization

static void setup(void) {}

// Entry for main loop

static void loop(void)
{
  switch (acs)
  {
    case acsInit: myopic.Init(); acs = acsMain; break;
    case acsMain: myopic.Main(); acs = acsTerm; break;
    case acsTerm: myopic.Term(); acs = acsExit; break;
    default: SwitchFault(); break;
  };
}

int main (void)
{
  signal(SIGINT, Interrupt);
  setup();
  while (acs != acsExit) loop();
  return 0;
}

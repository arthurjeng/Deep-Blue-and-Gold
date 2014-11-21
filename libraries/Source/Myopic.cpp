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
#include "XCP.h"
#include "Myopic.h"

void Myopic::Init(void)
{
  MarkStartTime(); state.OneTimeSetup();
  icp.SetStatePtr(&state); icp.Init();
}

void Myopic::Main(void) {icp.Loop();}

void Myopic::Term(void) {icp.Term();}

void Myopic::Interrupt(void) {icp.Interrupt();}

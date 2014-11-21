// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// License: Creative Commons Attribution-Share Alike 3.0
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
#include "Score.h"
#include "Move.h"
#include "UnDo.h"
#include "UnDoStack.h"
#include "TinyMove.h"
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
#include "BB.h"
#include "CIB.h"
#include "ICP.h"

void ICP::Dispatch(const icpcType icpc) const
{
  switch (icpc)
  {
    case icpcAT: DcAT(); break; // Activate common trace options
    case icpcBB: DcBB(); break; // Build book
    case icpcDB: DcDB(); break; // Display board
    case icpcDF: DcDF(); break; // Display FEN
    case icpcDH: DcDH(); break; // Display position hash
    case icpcDL: DcDL(); break; // Display opening library moves
    case icpcDM: DcDM(); break; // Display moves
    case icpcDO: DcDO(); break; // Display options
    case icpcDS: DcDS(); break; // Display status
    case icpcEM: DcEM(); break; // Enumerate movepaths
    case icpcFM: DcFM(); break; // Fast enumerate movepaths
    case icpcFP: DcFP(); break; // Flip position
    case icpcGG: DcGG(); break; // Go and play to end of game
    case icpcGP: DcGP(); break; // Go and play
    case icpcGS: DcGS(); break; // Go search
    case icpcHM: DcHM(); break; // Help me
    case icpcID: DcID(); break; // Show program identification
    case icpcNG: DcNG(); break; // New game
    case icpcPT: DcPT(); break; // Program test
    case icpcQP: DcQP(); break; // Quit program
    case icpcRO: DcRO(); break; // Reset option(s)
    case icpcRP: DcRP(); break; // Reset program
    case icpcSD: DcSD(); break; // Set depth limit
    case icpcSF: DcSF(); break; // Set FEN
    case icpcSO: DcSO(); break; // Set option(s)
    case icpcST: DcST(); break; // Set time limit
    case icpcTB: DcTB(); break; // Take back move
    default: SwitchFault(); break;
  };
}

void ICP::DcAT(void) const
{
  // Activate common trace options

  NoParameters();
  stateptr->SetOption(optnCB);
  stateptr->SetOption(optnIT);
  stateptr->SetOption(optnPV);
  stateptr->SetOption(optnSR);
  stateptr->SetOption(optnST);
  stateptr->SetOption(optnTS);
}

void ICP::DcBB(void) const
{
  // Build book <input-game-file> <output-code-file>

#if (IsDevHost)
  if (cib.GetTokenCount() != 3) PrintFS(fsUdBadParmCount);
  else
  {
    BB bb;

    bb.MakeBook(stateptr, cib.GetToken(1), cib.GetToken(2));
  };
#endif

#if (IsTarget)
  PrintFS(fsUdDevHostOnly);
#endif
}

void ICP::DcDB(void) const
{
  // Display board

  NoParameters(); ShowBoard();
}

void ICP::DcDF(void) const
{
  // Display FEN

  NoParameters(); ShowFEN();
}

void ICP::DcDH(void) const
{
  // Display position hash

  NoParameters(); ShowPosHash();
}

void ICP::DcDL(void) const
{
  // Display opening library moves

  NoParameters(); stateptr->PrintBookMoveList();
}

void ICP::DcDM(void) const
{
  // Display moves

  NoParameters(); ShowMoves();
}

void ICP::DcDO(void) const
{
  // Display options

  NoParameters(); ShowOptions();
}

void ICP::DcDS(void) const
{
  // Display status

  NoParameters();
  ShowBoard(); ShowMoves(); ShowLimits(); ShowOptions(); ShowFEN(); ShowPosHash();
}

void ICP::DcEM(void) const
{
  // Enumerate movepaths

  if (cib.GetTokenCount() != (1 + 1)) PrintFS(fsUdNeedSingleUIParm);
  else
  {
    const char *cptr = cib.GetToken(1);

    if (!IsStrUnsignedInt(cptr)) PrintFS(fsUdBadParmValue);
    else
    {
      plyType plies = MapStrToUi32(cptr);

      if (plies > MaxPlyLen) plies = MaxPlyLen;
      stateptr->RunMP(plies, false);
    };
  };
}

void ICP::DcFM(void) const
{
  // Fast enumerate movepaths

  if (cib.GetTokenCount() != (1 + 1)) PrintFS(fsUdNeedSingleUIParm);
  else
  {
    const char *cptr = cib.GetToken(1);

    if (!IsStrUnsignedInt(cptr)) PrintFS(fsUdBadParmValue);
    else
    {
      plyType plies = MapStrToUi32(cptr);

      if (plies > MaxPlyLen) plies = MaxPlyLen;
      stateptr->RunMP(plies, true);
    };
  };
}

void ICP::DcFP(void) const
{
  // Flip position

  NoParameters(); stateptr->Flip(); CondShowBoard();
}

void ICP::DcGG(void) const
{
  // Go and play to end of game

  NoParameters();
  if (IsGameOver()) ReportGameOver();
  else
  {
    do ProgMove(); while (!IsGameOver() && !stateptr->TestInterrupt());
    if (stateptr->TestInterrupt())
    {
      PrintFS(fsMsInterrupt); stateptr->ResetInterrupt();
    };
  };
}

void ICP::DcGP(void) const
{
  // Go and play

  NoParameters(); if (!IsGameOver()) ProgMove(); else ReportGameOver();
}

void ICP::DcGS(void) const
{
  // Go search

  NoParameters();
  if (IsGameOver()) ReportGameOver();
  else
  {
    Move move;

    FindMove(move);
    PrintFSL(fsLbMove); move.Print(stateptr->FetchSearchFEnv()); PrintNL();
  };
}

void ICP::DcHM(void) const
{
  // Help me

  NoParameters(); PrintFS(fsIcpHelp);
}

void ICP::DcID(void) const
{
  // Show program identification

  NoParameters(); PrintProgID();
}

void ICP::DcNG(void) const
{
  // New game

  NoParameters(); stateptr->NewGame(); CondShowBoard();
}

void ICP::DcPT(void) const
{
  // Program test

  NoParameters(); Test();
}

void ICP::DcQP(void) const
{
  // Quit program

  NoParameters(); stateptr->SetDone();
}

void ICP::DcRO(void) const
{
  // Reset option(s)

  bool valid = true;
  ui index = 1;

  // Scan through the parameter tokens

  while (valid && (index < cib.GetTokenCount()))
  {
    const optnType optn = DecodeOption(cib.GetToken(index));

    if (IsOptnNil(optn)) {PrintFS(fsUdBadParmValue); valid = false;}
    else
    {
      stateptr->ResetOption(optn); index++;
    };
  };
}

void ICP::DcRP(void) const
{
  // Reset program

  NoParameters();
  stateptr->ResetOptions(); stateptr->SetDefaultLimits(); stateptr->NewGame();
}

void ICP::DcSD(void) const
{
  // Set depth limit

  if (cib.GetTokenCount() != (1 + 1)) PrintFS(fsUdNeedSingleUIParm);
  else
  {
    const char *cptr = cib.GetToken(1);

    if (!IsStrUnsignedInt(cptr)) PrintFS(fsUdBadParmValue);
    else
    {
      plyType plies = MapStrToUi32(cptr);

      if (plies > MaxPlyLen) plies = MaxPlyLen;
      stateptr->PutLimitPly(plies); ShowLimits();
    };
  };
}

void ICP::DcSF(void) const
{
  // Set FEN

  if (cib.GetTokenCount() != (1 + 6)) PrintFS(fsUdBadParmCount);
  else
  {
    // Assemble the six parameter tokens into a FEN string

    FPos fpos;
    char fen[fenLen];
    ui chindex = 0;

    for (ui index = 1; index <= 6; index++)
    {
      const char *cptr = cib.GetToken(index);
      char ch;

      while ((ch = *cptr++)) if (chindex < (fenLen - 1)) fen[chindex++] = ch;
      fen[chindex++] = ((index < 6) ? ' ' : '\0');
    };

    // Test the FEN string for validity

    if (!fpos.LoadFromStr(fen)) PrintFS(fsUdBadParmValue);
    else
    {
      // Load the new position

      stateptr->LoadStateFromFPos(fpos); CondShowBoard();
    };
  };
}

void ICP::DcSO(void) const
{
  // Set option(s)

  bool valid = true;
  ui index = 1;

  // Scan through the parameter tokens

  while (valid && (index < cib.GetTokenCount()))
  {
    const optnType optn = DecodeOption(cib.GetToken(index));

    if (IsOptnNil(optn)) {PrintFS(fsUdBadParmValue); valid = false;}
    else
    {
      stateptr->SetOption(optn); index++;
    };
  };
}

void ICP::DcST(void) const
{
  // Set time limit

  if (cib.GetTokenCount() != (1 + 1)) PrintFS(fsUdNeedSingleUIParm);
  else
  {
    const char *cptr = cib.GetToken(1);

    if (!IsStrUnsignedInt(cptr)) PrintFS(fsUdBadParmValue);
    else
    {
      ui32 secs = MapStrToUi32(cptr);
      const ui32 seclimit = 1000000ul;

      if (secs > seclimit) secs = seclimit;
      stateptr->PutLimitMsec(secs * ((msType) 1000)); ShowLimits();
    };
  };
}

void ICP::DcTB(void) const
{
  // Take back move

  NoParameters();
  if (stateptr->GetUnDoCount() == 0) PrintFS(fsUdNoTakeBackAvail);
  else
  {
    const Move move = stateptr->FetchLastMove();

    stateptr->Unplay();
    PrintFSL(fsLbUnplayedMove); move.Print(stateptr->FetchSearchFEnv()); PrintNL();
    CondShowBoard();
  };
}

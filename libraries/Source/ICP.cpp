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

void ICP::RetrieveIcpcString(const icpcType icpc, char cv[])
{
  // Copy the indicated ICP command string to the given character buffer

  for (ui index = 0; index < icpcstrLen; index++)
    cv[index] = ReadFlashUi8((unsigned char *) fsIcpcStrs + (icpc * icpcstrLen) + index);
  cv[icpcstrLen] = '\0';
}

icpcType ICP::DecodeCommand(const char *str)
{
  // Attempt to decode the supplied string as an ICP command

  icpcType icpc = icpcNil;
  char cv[icpcstrLen + 1];
  si lo = 0, hi = icpcLen - 1;

  // Do a binary search of the ICP command string list to find a match

  while (IsIcpcNil(icpc) && (lo <= hi))
  {
    const si index = (lo + hi) / 2;

    RetrieveIcpcString((icpcType) index, cv);

    const si compare = StrCmp(str, cv);

    if (compare == 0) icpc = (icpcType) index;
    else
    {
      if (compare < 0) hi = index - 1; else lo = index + 1;
    };
  };
  return icpc;
}

void ICP::Init(void) const
{
  // A one time call to initialize the Interactive Command Processor

  PrintNL();
  PrintProgID(); PrintNL(); PrintFS(fsMsCaution); PrintNL(); PrintFS(fsMsAssist); PrintNL();
}

void ICP::Term(void) const
{
  // A one time call to terminate the Interactive Command Processor

  PrintFS(fsMsFinished);
}

void ICP::Loop(void)
{
  // A one time call to repeatedly cycle the Interactive Command Processor

  while (!stateptr->IsDone()) SingleCycle();
}

void ICP::Interrupt(void)
{
  // Process a user generated interrupt

  stateptr->Interrupt();
}

void ICP::SingleCycle(void)
{
  // Clear any spurious interrupt request

  stateptr->ResetInterrupt();

  // Issue a prompt and then read a command line

  PrintFS(fsMsPrompt); cib.Load();

  // Check for a re-seeding of the pseudorandom number generator

  if ((stateptr->GetLoopCount() % MX(8)) == 0) RandomSeed(ElapsedMsec() & MXL(15));

  // Check for end of file

  if (cib.IsEOF()) stateptr->SetDone();
  else
  {
    // At least one token is needed for a command or a move

    if (cib.GetTokenCount())
    {
      // Mark the start time for this input

      const msType ctstartmsec = ElapsedMsec();

      // Try to match a command verb from the first token

      const icpcType icpc = DecodeCommand(cib.GetToken(0));

      // Perform command dispatch if a command verb match was made

      if (IsIcpcNotNil(icpc)) Dispatch(icpc);
      else
      {
        // No command verb match; try to handle a user move sequence

        if (IsGameOver()) ReportGameOver();
        else
        {
          // The game is still in progress; loop through the user move strings

          bool valid = true;
          ui index = 0;

          while (valid && (index < cib.GetTokenCount()))
          {
            // Handle a single user move

            Move move;

            stateptr->MatchMove(cib.GetToken(index), move);
            if (move.IsVoid())
            {
              // Couldn't find the move

              PrintFS(fsUdBadMove); valid = false;
            }
            else
            {
              // Found the move; play it

              UserMove(move);

              // Next token

              index++;
            };
          };

          // Optional program reply

          if (valid && !stateptr->TestOption(optnNR) && !IsGameOver()) ProgMove();
        };
      };

      // Trace: command processing time

      if (stateptr->TestOption(optnCT))
      {
        PrintOptn(optnCT);
        PrintFSL(fsLbSeconds); PrintMsecAsSeconds(ElapsedMsec() - ctstartmsec); PrintNL();
      };
    };
  };

  // Done with this input

  stateptr->IncLoopCount();
}

void ICP::ShowBoard(void) const
{
  // Display the chessboard

  stateptr->PrintGraphic();
}

void ICP::ShowFEN(void) const
{
  // Display the position's Forsyth-Edwards Notation

  PrintFSL(fsLbFen); stateptr->PrintFEN(); PrintNL();
}

void ICP::ShowLimits(void) const
{
  // Display the time and depth limits

  PrintFSL(fsLbTimeLimit); PrintMsecAsSeconds(stateptr->GetLimitMsec()); PrintISM();
  PrintFSL(fsLbDepthLimit); PrintUi16(stateptr->GetLimitPly()); PrintNL();
}

void ICP::ShowMoves(void) const
{
  // Display the moves for the position

  if (IsGameOver()) ReportGameOver();
  else
  {
    PrintFSL(fsLbMoves); stateptr->PrintMoves(); PrintNL();
  };
}

void ICP::ShowOptions(void) const
{
  // Display the list of active options

  bool needspace = false;

  PrintFSL(fsLbOptions);
  for (ui optnindex = 0; optnindex < optnLen; optnindex++)
  {
    const optnType optn = (optnType) optnindex;

    if (stateptr->TestOption(optn))
    {
      char cv[optnstrLen + 1];

      if (needspace) PrintSpace();
      RetrieveOptnString(optn, cv); PrintStr(cv); needspace = true;
    };
  };
  PrintNL();
}

void ICP::ShowPosHash(void) const
{
  // Display the hash for the position

  PrintFSL(fsLbHash); stateptr->FetchPosHash().Print(); PrintNL();
}

void ICP::NoParameters(void) const {if (cib.GetTokenCount() > 1) PrintFS(fsUdIgnoredParms);}

bool ICP::IsGameOver(void) const {return stateptr->IsOver();}

void ICP::ReportGameOver(void) const {PrintGameOver(stateptr->CalcFT()); PrintNL();}

void ICP::Play(const Move& move) const
{
  // Capture a local copy of the move for safety

  const Move playmove = move;

  // Play the move, conditionally display the new position, and handle game over check

  stateptr->Play(playmove); CondShowBoard(); if (IsGameOver()) ReportGameOver();
}

void ICP::ProgMove(void) const
{
  // Find and play a program move

  Move move;

  // Find the program move

  FindMove(move);

  // Conditionally issue audible alert

  if (stateptr->TestOption(optnAA)) Beep();

  // Resign if things look bad

  if (stateptr->IsLoseIn1AfterMove(move)) PrintFS(fsMsResigns);

  // Announce a forced checkmate sequence if present

  if (IsSvMating(move.GetSv())) Score::PrintMateAnnouncement(move.GetSv());

  // Print and play the program move

  PrintFSL(fsLbMyMove); move.Print(stateptr->FetchSearchFEnv()); PrintNL();
  Play(move);
}

void ICP::UserMove(const Move& move) const
{
  // Print and play the user move

  PrintFSL(fsLbYourMove); move.Print(stateptr->FetchSearchFEnv()); PrintNL();
  Play(move);
}

void ICP::FindMove(Move& move) const
{
  // Search for a move for the program

  if (IsGameOver()) move.MakeVoid();
  else
  {
    stateptr->RunABSearch(); move = stateptr->PVMove(0);
  };
}

void ICP::CondShowBoard(void) const {if (stateptr->TestOption(optnCB)) ShowBoard();}

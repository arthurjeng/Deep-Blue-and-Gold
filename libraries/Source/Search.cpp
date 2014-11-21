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

void Search::OneTimeSetup(const ML& priorml, History& history, UnDoStack& undostack)
{
  // This routine is be called only ONCE before any other Search method

  options = 0; rootml.LoadFromPrior(priorml);
  historyptr = &history; undostackptr = &undostack;
  ply = 0; depth = DefaultSD; SetInitialArray();
}

void Search::ResetAux(void)
{
  // Reset work done prior to the start of a search (spos not modified here)

  abort = false; isftdown = true; isprelim = false; fastmp = false; actledstate = false;
  interrupt = false; st = stUnterminated; nodecounter.Reset();
  startmsec = checkusec = ElapsedMsec(); timeoutmsec = startmsec + (DefaultST * ((msType) 1000));
  usedmsec = 0; actleddisplayusec = boarddisplayusec = startmsec; rootml.ResetCount(); mgs.Reset();
  for (plyType index = 0; index < MaxPlyLenP1; index++) pirstack[index].Reset();
  for (plyType index = 0; index < MaxPlyLen; index++) killers[index].MakeVoid();
  pvtable.Reset();
}

void Search::LoadSearchFromFPos(const FPos& fpos)
{
  // Load the search form an FEN position

  ResetAux(); spos.LoadPosFromFPos(fpos);
}

void Search::SetInitialArray(void)
{
  // Set up the initial chess position; called only at ply zero

  ResetAux(); spos.SetInitialArray();
}

void Search::UpdateActivityLED(void) const
{
  // Control the optional activity LED

  if (actledstate) SetActivityLEDState(); else ResetActivityLEDState();
}

bool Search::IsTriggerTimeCheck(void) const
{
  // Determine if a time check should be made

#if (IsDevHost)
  return (nodecounter.GetCodeByte(0) == 0) && (nodecounter.GetCodeByte(1) == 0);
#endif

#if (IsTarget)
  return true;
#endif
}

bool Search::IsTriggerAcitivityLEDUpdate(void)
{
  // Determine if an activity LED update should be made

#if (IsDevHost)
  return false;
#endif

#if (IsTarget)
  const ui updatehertz = 1;
  const msType periodmsec = 1000 / (updatehertz * 2), elapsedmsec = ElapsedMsec();
  const bool update = (elapsedmsec > actleddisplayusec);

  if (update) do actleddisplayusec += periodmsec; while (elapsedmsec > actleddisplayusec);
  return update;
#endif
}

bool Search::IsTriggerBoardDisplayUpdate(void)
{
  // Determine if a board display update should be made

#if (IsDevHost)
  return false;
#endif

#if (IsTarget)
  const ui updatehertz = 8;
  const msType periodmsec = 1000 / updatehertz, elapsedmsec = ElapsedMsec();
  const bool update = (elapsedmsec > boarddisplayusec);

  if (update) do boarddisplayusec += periodmsec; while (elapsedmsec > boarddisplayusec);
  return update;
#endif
}

void Search::DoExecuteAux(const Move& move)
{
  // Update the history and the undo stack, then execute the move

  historyptr->Push(spos.FetchPosHash());
  undostackptr->Push(spos, move, spos.GetCaptTid());
  spos.Execute(move);
}

void Search::DoRetractAux(void)
{
  // Retract the move, then downdate the history and the undo stack

  spos.Retract(
    undostackptr->FetchTopMove(),
    undostackptr->FetchTopFEnv(),
    undostackptr->GetTopCaptTid());
  historyptr->PopDel();
  undostackptr->PopDel();
}

void Search::DoExecute(const Move& move)
{
  // Advance one ply into the future; not called when playing a move over the board

  if (ply == MaxPlyLen) DieFS(fsFeOverflowPS);
  pirstack[ply].RefMGS() = mgs; pirstack[ply].RefPEnv() = *(PEnv *) &spos;
  ply++; --depth;
  DoExecuteAux(move);
}

void Search::DoRetract(void)
{
  // Retreat one ply into the past; not called when unplaying a move over the board

  if (ply == 0) DieFS(fsFeUnderflowPS);
  --ply; depth++;
  mgs = pirstack[ply].FetchMGS(); *(PEnv *) &spos = pirstack[ply].FetchPEnv();
  DoRetractAux();
}

void Search::Play(const Move& move)
{
  // Play a move as if over the board (i.e., on a physical chessboard)

  DoExecuteAux(move); ResetAux();
}

void Search::Unplay(void)
{
  // Unplay a move as if over the board (i.e., on a physical chessboard)

  DoRetractAux(); spos.Regen(); ResetAux();
}

void Search::PrintPVAndScore(void) const {pvtable.PrintPVAndScore(spos);}

void Search::Indent(void) const {PrintSpaces(ply * 2);}

void Search::MarkCV(const ML& priorml)
{
  // Apply marks to the CV (current variation)

  const plyType savedply = ply;

  // Retract to ply zero, saving the MGS objects along the way

  while (ply) {pirstack[ply].RefMGS() = mgs; DoRetract();};

  // Execute to saved ply, marking moves along the way

  while (ply < savedply)
  {
    // First, establish the CV move at this ply

    Move move = undostackptr->FetchNextMove();

    // Then generate/apply any notation flags

    move.Mark(spos, priorml);

    // Advance to the next ply restoring the MGS object

    DoExecute(move); mgs = pirstack[ply].FetchMGS();
  };
}

void Search::MarkPV(const ML& priorml)
{
  // Apply marks to the PV; must be called at ply zero

  while (PVMove(ply).IsNotVoid())
  {
    // Generate/apply any notation flags, then advance to the next ply

    RefPVMove(ply).Mark(spos, priorml); DoExecute(PVMove(ply));
  };

  // Retract all the PV moves to return to ply zero

  while (ply) DoRetract();
}

void Search::PrintCV(void) const
{
  // Print the current variation

  for (plyType index = 0; index < ply; index++)
  {
    if (index) PrintSpace();
    undostackptr->FetchPastMove(ply - index - 1).Print();
  };
}

void Search::MatchMove(const char *str, Move& move)
{
  // Match the given move string in the search position

  ML ml(rootml);

  spos.MatchMove(str, move, ml);
}

void Search::Trace0(void)
{
  // Handle input FEN trace

  if (TestOptionM(optnmIF)) {PrintOptn(optnIF); PrintFSL(fsLbFen); PrintFEN(); PrintNL();};
}

void Search::Trace1(void)
{
  // Handle the search timing statistics trace

  if (TestOptionM(optnmTS))
  {
    PrintOptn(optnTS); PrintFSL(fsLbNodes); nodecounter.Print();
    PrintISM(); PrintFSL(fsLbSeconds); PrintMsecAsSeconds(usedmsec);

    // Avoid division by zero

    if (usedmsec)
    {
      PrintISM(); PrintFSL(fsLbFrequency);
      PrintUi32((ui32) (nodecounter.CalcDoubleValue() / (usedmsec / 1000.0)));
      PrintFS(fsMsSfxHz);
    };
    PrintNL();
  };
}

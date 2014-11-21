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
#include "Score.h"
#include "Move.h"
#include "PVTable.h"

PVTable::PVTable(void)
{
  // Ininialize the ply bases that map the linear array into a triangular semi-matrix

  miType offset = 0;

  for (plyType index = 0; index < MaxPVLen; index++)
  {
    // After being set here, the base data does not change

    bases[index] = offset; offset += (MaxPVLenP1 - index);
  };

  // Reset all the move storage (both prior and trace)

  Reset();
}

void PVTable::ResetPrior(void)
{
  // Reset the prior PV; this is the ply zero PV stored by prior analysis

  for (plyType index = 0; index < MaxPVLenP1; index++) prior[index].MakeVoid();
}

void PVTable::ResetTrace(void)
{
  // Reset the trace triangle; this is potential PV move sequence data stored by ply

  for (miType index = 0; index < PVTableLen; index++) trace[index].MakeVoid();
}

void PVTable::CopyUpToPly(const plyType toply, const Move& move)
{
  // Called when a new PV is to be passed up one ply in the search

  if (toply < MaxPVLen)
  {
    // The destination row must be within the compile time limit

    miType toindex = bases[toply];

    // Copy up the first move of the new PV sequence

    trace[toindex++] = move;

    // Copy the remainder of the PV if the source is within the compile time limit

    if (toply < (MaxPVLen - 1))
    {
      miType frindex = bases[toply + 1];

      while (trace[frindex].IsNotVoid()) trace[toindex++] = trace[frindex++];
    };

    // Ensure void move termination of the new PV sequence

    trace[toindex++].MakeVoid();
  };
}

void PVTable::CopyToPrior(const svType sv)
{
  // The prior PV is updated via a copy of the ply zero PV from the trace data

  plyType index = 0;

  while ((index < MaxPVLen) && trace[index].IsNotVoid())
  {
    prior[index] = trace[index]; index++;
  };

  // Because of various referencing of the prior PV, *all* trailing data *must* be void

  while (index < MaxPVLen) prior[index++].MakeVoid();

  // Finally, add the score data to the first move to give the score result of the search

  prior[0].PutSv(sv);
}

void PVTable::SetSingleMovePV(const Move& move)
{
  // Set up a single move PV

  ResetPrior(); prior[0] = move;
}

void PVTable::PrintRawPV(void) const {Move::PrintVTMS(prior);}

void PVTable::PrintPV(const FEnv& fenv) const {PrintFSL(fsLbPV); Move::PrintVTMS(prior, fenv);}

void PVTable::PrintPVAndScore(const FEnv& fenv) const
{
  PrintPV(fenv); PrintISM(); PrintFSL(fsLbScore); Score::PrintSv(prior[0].GetSv());
}

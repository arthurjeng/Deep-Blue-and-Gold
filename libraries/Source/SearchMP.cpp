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
#include "TBV.h"
#include "MGS.h"
#include "PEnv.h"
#include "Pos.h"
#include "PVTable.h"
#include "PIR.h"
#include "Search.h"

void Search::MPAuxML(const ML& ml)
{
  // Enumerate movepaths for a move list

  if (NotStopped() && !CheckInterrupt())
  {
    for (miType index = 0; NotStopped() && (index < ml.GetCount()); index++)
    {
      Counter counter0, counter1;

      if (ply == 0) counter0 = nodecounter;

      DoExecute(ml.FetchMove(index)); if (!fastmp) RedrawBoardDisplay();

      // Current variation trace

      if (TestOptionM(optnmCV)) {MarkCV(ml); PrintOptn(optnCV); PrintCV(); PrintNL();};

      // Update the activity LED periodically

      if (IsTriggerAcitivityLEDUpdate()) {actledstate = !actledstate; UpdateActivityLED();};

      if (depth == 0) nodecounter.Increment();
      else
      {
        if (fastmp && (depth == 1)) nodecounter.Increment(CountMoves()); else MPAux(ml);
      };

      DoRetract(); if (!fastmp) RedrawBoardDisplay();

      if (!abort & (ply == 0))
      {
        // Calculate partial sum for this ply zero move

        counter1 = nodecounter; counter1.Decrement(counter0);

        // Handle ply zero trace

        if (TestOptionM(optnmPZ))
        {
          PrintOptn(optnPZ); ml.FetchMove(index).Print();
          PrintISM(); counter1.Print(); PrintNL();
        };
      };
    };
  };
}

void Search::MPAux(const ML& priorml)
{
  // Fast enumerate movepaths for the current position

  if (NotStopped() && !CheckInterrupt())
  {
    if (depth == 0) nodecounter.Increment();
    else
    {
      if (ply == 0)
      {
        ML ml(priorml);

        GenCanonical(ml); MPAuxML(ml);
      }
      else
      {
        if (fastmp && (depth == 1)) nodecounter.Increment(CountMoves());
        else
        {
          if (InCheck())
          {
            ML ml(priorml);

            GenEvasion(ml); MPAuxML(ml);
          }
          else
          {
            for (pieceType piece = piecePawn; NotStopped() && (piece <= pieceKing); piece++)
            {
              TBV tbv = spos.FetchTBVBCP(spos.GetGood(), piece);
              tidType tid;
              ML ml(priorml);

              while (NotStopped() && IsTidNotNil(tid = tbv.NextTid()))
              {
                ml.ResetCount();
                spos.GenAddNonEvasionFromSquare(genmAll, spos.GetTidToSqr(tid), ml);
                MPAuxML(ml);
              };
            };
          };
        };
      };
    };
  };
}

void Search::RunMP(const plyType limitply, const bool fast)
{
  // Initial activity LED update

  actledstate = true; UpdateActivityLED();

  // Handle input FEN trace

  Trace0();

  // Handle search input parameters trace

  if (TestOptionM(optnmIP))
  {
    PrintOptn(optnIP); PrintFSL(fsLbDepthLimit); PrintUi16(limitply); PrintNL();
  };

  // Perform the pre-enumeration initialization

  ResetAux(); depth = (depthType) limitply; fastmp = fast;

  // Perform the enumeration with timing

  RedrawBoardDisplay();
  MPAux(rootml); usedmsec = ElapsedMsec() - startmsec;
  RedrawBoardDisplay();

  // Handle search timing statistics trace

  Trace1();

  // Final activity LED update

  actledstate = false; UpdateActivityLED();

  // Output final sum

  PrintFSL(fsLbNodes); nodecounter.Print(); PrintNL();
}

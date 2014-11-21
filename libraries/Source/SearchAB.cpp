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
#include "Score.h"
#include "Window.h"
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

void Search::ScoreAndSortRootML(void)
{
  // Turn on the preliminary scoring flag; this slightly modifies search behavior

  isprelim = true;

  // Perform a simple evaluation and quiescence search for the initial root move ordering

  const depthType saveddepth = depth;
  Window mywindow;

  mywindow.SetFullWidth(); depth = 0;
  for (miType index = 0; index < rootml.GetCount(); index++)
  {
    if (RootMove(index).IsNotCertain())
    {
      DoExecute(RootMove(index));
      RootMove(index).PutSv(ShiftUp(ABRootNode(mywindow, rootml)));
      DoRetract();
    };
  };

  // Turn off the preliminary scoring flag; search behavior back to normal

  isprelim = false; depth = saveddepth;

  // Sort the root move list in descending score value order

  rootml.SortBySv();

  // Handle preliminary score tracing

  if (TestOptionM(optnmPS))
  {
    for (miType index = 0; index < rootml.GetCount(); index++)
    {
      PrintOptn(optnPS); RootMove(index).Print(); PrintISM();
      PrintFSL(fsLbScore); Score::PrintSv(RootMove(index).GetSv()); PrintNL();
    };
  };

  // Set the initial analysis, if any

  pvtable.Reset();
  if (rootml.GetCount() > 0) pvtable.SetSingleMovePV(RootMove(0));
}

miType Search::PickBestUnsearched(const ML& ml)
{
  miType pick = -1;
  svType picksv = svBroken;

  for (miType index = 0; index < ml.GetCount(); index++)
    if (!ml.FetchMove(index).IsSearched() && (ml.FetchMove(index).GetSv() > picksv))
    {
      picksv = ml.FetchMove(index).GetSv(); pick = index;
    };
  return pick;
}

miType Search::Pick(ML& ml)
{
  // Pick a move to search

  miType index = -1;

  while ((index < 0) && (mgs.GetPs() != psTerm))
  {
    switch (mgs.GetPs())
    {
      case psBaseInit:
        // The base node initialization is a special case as the move list is already prepared

        ml.ClearSearchFlags(); mgs.PutPs(psBaseNext);
        break;

      case psBaseNext:
        // The next node at the base is picked sequentially

        if (mgs.GetUsedCount() < ml.GetCount())
          index = mgs.NextBaseMoveIndex();
        else
          mgs.PutPs(psTerm);
        break;

      case psEvadInit:
        // All possible check evasion moves are generated

        GenEvasion(ml); ml.AssignLikelyGain(spos); mgs.PutPs(psEvadPV);
        break;

      case psEvadPV:
        // Try to pick the PV move

        if (isftdown && (ply < MaxPVLen) && PVMove(ply).IsNotVoid())
          index = ml.LocateNoFail(PVMove(ply));
        mgs.PutPs(psEvadNext);
        break;

      case psEvadNext:
        // The next node for check evasion is picked by score

        index = PickBestUnsearched(ml); if (index < 0) mgs.PutPs(psTerm);
        break;

      case psFullInit:
        // Set up for a full width node; move generation done later

        mgs.PutPs(psFullPV);
        break;

      case psFullPV:
        // Try to pick the PV move

        if (isftdown && (ply < MaxPVLen) && PVMove(ply).IsNotVoid())
        {
          // The PV move needs to be generated

          ml.ResetCount(); ml.Push(PVMove(ply)); mgs.SetAvoidPV(); index = 0;
        };
        mgs.PutPs(psFullGainSeqInit);
        break;

      case psFullGainSeqInit:
        // Generate all of the gainer moves and assign likely gain preliminary scores

        GenNonEvasion(genmGain, ml); ml.AssignLikelyGain(spos);

        // If the PV move has been tried, mark it as searched if it was a gainer move

        if (mgs.AvoidPV())
        {
          const miType pvindex = ml.Locate(PVMove(ply));

          if (pvindex >=0) ml.RefMove(index).SetFlagM(mfmSrch);
        };

        // Now go scan the gainer list

        mgs.PutPs(psFullGainSeqMove);
        break;

      case psFullGainSeqMove:
        // Pick the best unsearched move (here, a gainer)

        index = PickBestUnsearched(ml); if (index < 0) mgs.PutPs(psFullKiller);
        break;

      case psFullKiller:
        // Try the killer move (here, a holder), but not the PV move

        if (killers[ply].IsNotVoid())
        {
          // Avoid using the PV move if it's the same as the killer

          if (!(mgs.AvoidPV() && Move::SameMove(killers[ply], PVMove(ply))))
          {
            const sqrType frsqr = killers[ply].GetFrSqr();
            const manType frman = spos.GetMan(frsqr);

            if (cvmantocolor[frman] == spos.GetGood())
            {
              // Generate moves by the killer (move might not be present)

              ml.ResetCount(); spos.GenAddNonEvasionFromSquare(genmHold, frsqr, ml);
              index = ml.Locate(killers[ply]);
              if (index >= 0) mgs.SetAvoidKiller();
            };
          };
        };
        mgs.PutPs(psFullKillerPM2);
        break;

      case psFullKillerPM2:
        // Try the killer move at ply minus two (here, a holder), but not the PV move or killer

        if (ply >= 2)
        {
          const plyType pm2 = ply - 2;

          if (killers[pm2].IsNotVoid() && !Move::SameMove(killers[ply], killers[pm2]))
          {
            // Avoid using the PV move if it's the same as the pm2 killer

            if (!(mgs.AvoidPV() && Move::SameMove(killers[pm2], PVMove(ply))))
            {
              const sqrType frsqr = killers[pm2].GetFrSqr();
              const manType frman = spos.GetMan(frsqr);

              if (cvmantocolor[frman] == spos.GetGood())
              {
                // Generate moves by the pm2 killer (move might not be present)

                ml.ResetCount(); spos.GenAddNonEvasionFromSquare(genmHold, frsqr, ml);
                index = ml.Locate(killers[pm2]);
                if (index >= 0) mgs.SetAvoidKillerPM2();
              };
            };
          };
        };
        mgs.PutPs(psFullHoldSeqInit);
        break;

      case psFullHoldSeqInit:
        // Set up the full width holder outer loop: piece kind from pawn to king

        mgs.PutPiece(piecePawn); mgs.PutPs(psFullHoldSeqPieceTBV);
        break;

      case psFullHoldSeqPieceTBV:
        // Set up the target bit vector for the indicated piece kind; check for exit

        if (mgs.GetPiece() > pieceKing) mgs.PutPs(psTerm);
        else
        {
          // Assign the target bit vector for the indicated piece kind

          mgs.RefGoodTBV() = spos.FetchTBVBCP(spos.GetGood(), mgs.GetPiece());
          mgs.PutPs(psFullHoldSeqPieceTid);
        };
        break;

      case psFullHoldSeqPieceTid:
        {
          // Get the next target ID for the current piece kind

          const tidType goodtid = mgs.RefGoodTBV().NextTid();

          // Check for no more targets for the current piece kind

          if (IsTidNil(goodtid)) {mgs.IncPiece(); mgs.PutPs(psFullHoldSeqPieceTBV);}
          else
          {
            // Generate the holder moves for the current target ID

            const sqrType goodfrsqr = spos.GetTidToSqr(goodtid);

            ml.ResetCount(); spos.GenAddNonEvasionFromSquare(genmHold, goodfrsqr, ml);

            // Mark the PV move as being searched (may not be present)

            if (mgs.AvoidPV() && (PVMove(ply).GetFrSqr() == goodfrsqr))
            {
              const miType avoidindex = ml.Locate(PVMove(ply));

              if (avoidindex >= 0) ml.RefMove(avoidindex).SetFlagM(mfmSrch);
              mgs.ResetAvoidPV();
            };

            // Mark the killer move as being searched (may not be present)

            if (mgs.AvoidKiller() && (killers[ply].GetFrSqr() == goodfrsqr))
            {
              const miType avoidindex = ml.Locate(killers[ply]);

              if (avoidindex >= 0) ml.RefMove(avoidindex).SetFlagM(mfmSrch);
              mgs.ResetAvoidKiller();
            };

            // Mark the pm2 killer move as being searched (may not be present)

            if (mgs.AvoidKillerPM2() && (killers[ply - 2].GetFrSqr() == goodfrsqr))
            {
              const miType avoidindex = ml.Locate(killers[ply - 2]);

              if (avoidindex >= 0) ml.RefMove(avoidindex).SetFlagM(mfmSrch);
              mgs.ResetAvoidKillerPM2();
            };

            // Prepare the moves for a scan

            ml.AssignCenterTropismGain(); mgs.PutPs(psFullHoldSeqPieceMove);
          };
        };
        break;

      case psFullHoldSeqPieceMove:
        // Pick the best unsearched move for the current target

        index = PickBestUnsearched(ml); if (index < 0) mgs.PutPs(psFullHoldSeqPieceTid);
        break;

      case psGainInit:
        // All possible gainer moves are generated

        GenNonEvasion(genmGain, ml); ml.AssignLikelyGain(spos); mgs.PutPs(psGainPV);
        break;

      case psGainPV:
        // Try the predicted variation move if present

        if (isftdown && (ply < MaxPVLen) && PVMove(ply).IsNotVoid())
          index = ml.LocateNoFail(PVMove(ply));
        mgs.PutPs(psGainNext);
        break;

      case psGainNext:
        // The next node for the gainer search is picked by score

        index = PickBestUnsearched(ml); if (index < 0) mgs.PutPs(psTerm);
        break;

      case psTerm:
        // Shouldn't get here
        break;

      default:
        SwitchFault();
        break;
    };
  };

  // Mark the picked move, if any, as being searched

  if (index >= 0) ml.RefMove(index).SetFlagM(mfmSrch);
  return index;
}

svType Search::ABNode(const Window& window, const ML& priorml)
{
  // Establish the local processsing completion status

  bool done = false;

  // Establish the local window; its alpha will be returned as the value of this node

  Window mywindow(window);

  // Reserve a "stand pat" evaluation score; its real value assigned at (depth <= 0) nodes

  svType spscore = svEven;

  // Keep track of the number of calls to this routine

  nodecounter.Increment();

  // Current variation trace

  if (TestOptionM(optnmCV)) {MarkCV(priorml); PrintOptn(optnCV); PrintCV(); PrintNL();};

  // Clear the local predicted variation

  ClearLocalPV();

  // Update the activity LED periodically

  if (IsTriggerAcitivityLEDUpdate()) {actledstate = !actledstate; UpdateActivityLED();};

  // Update the external board display periodically

  if (IsTriggerBoardDisplayUpdate()) RedrawBoardDisplay();

  // Elapsed wall time check for search termination; skipped for preliminary scoring

  if (IsTriggerTimeCheck() && IsPastTime() && !isprelim)
  {
    Stop(stLimitTime); abort = done = true;
  };

  // User interrupt check; skipped for preliminary scoring

  if (!done && !isprelim && CheckInterrupt()) done = true;

  // Check for a likely draw at all nodes that are at least one ply away from the root

  if (!done && (ply > 0) && IsLikelyDraw()) {mywindow.PutAlfa(svEven); done = true;};

  // Check for ply limit encounter; return evaluation if no further analysis is possible

  if (!done && (ply == MaxPlyLen)) {mywindow.PutAlfa(Evaluate()); done = true;};

  // Establish a "stand pat" evaluation for potentially terminal nodes

  if (!done && (depth <= 0) && !InCheck())
  {
    // Note that Evaluate() is called only at (depth <= 0) non-evasion nodes

    spscore = Evaluate();

    // Is alpha improved by the stand pat evaluation?

    if (spscore > mywindow.GetAlfa())
    {
      // Raise alpha and check for a quick cutoff

      mywindow.PutAlfa(spscore); if (mywindow.IsCutOff()) done = true;
    };
  };

  // If no quick exit, then continue to main analysis work

  if (!done)
  {
    // Determine node type (base, evad, full, or gain)

    abnType abn;

    if ((ply == 0) && !isprelim) abn = abnBase;
    else
    {
      if (InCheck()) abn = abnEvad;
      else
      {
        if (depth > 0) abn = abnFull; else abn = abnGain;
      };
    };

    // Establish the local move list; moves already generated for the base node

    ML ml(priorml);

    // Special case move list for base node; moves are already generated and ordered

    if (abn == abnBase) ml.JamAssign(priorml);

    // Initialize the move generation state for this node

    mgs.InitTrack(abn);

    // Establish the best move seen so far at this node

    Move bestmove;

    bestmove.MakeVoid();

    // Save the current depth in case of extensions

    const depthType savedepth = depth;

    // Extension for check evasion

    if (abn == abnEvad) depth++;

    // Establish the "at least one move" flag; used for checkmate/stalemate detection

    bool alo = false;

    // Declare the picked move index; value returned by the Pick() call

    miType index;

    // Run the move loop once per each move available; note stop check and cutoff check

    while (
      NotStopped() &&
      !mywindow.IsCutOff() &&
      ((index = Pick(ml)) >= 0) &&
      !((abn == abnGain) &&
        (spscore + ml.FetchMove(index).GetSv() + svMaxPos) <= mywindow.GetAlfa()) &&
      !((abn == abnGain) && isprelim && alo))
    {
      svType tryscore;

      // At this point at least one move is available, so neither checkmate nor stalemate

      alo = true;

      // Special handling for ply zero moves with certain scores

      if ((ply == 0) && ml.FetchMove(index).IsCertain())
      {
        // Instead of searching this move, just retrieve its certain score

        tryscore = ml.FetchMove(index).GetSv(); ClearLowerPV();
      }
      else
      {
        // Make a temporary window

        Window trywindow = mywindow;

        // Execute the move; now down one ply

        DoExecute(ml.FetchMove(index));

        // Downshift the try window; basically: alpha = -oldbeta, beta = -oldalpha

        trywindow.GoDown();

        // Recursively call this routine to get a try score for the move (note upshift)

        tryscore = ShiftUp(ABNode(trywindow, ml));

        // Retract the move; now back at current ply

        DoRetract();
      };

      // An abort check is needed to avoid examining an invalid score result

      if (!abort && (tryscore > mywindow.GetAlfa()))
      {
        // The returned score is better than alpha; perform best move update

        bestmove = ml.FetchMove(index);

        // Adjust the local window by raising alpha; this may cause a cutoff

        mywindow.PutAlfa(tryscore);

        // If the new score in within the window then a new PV has been found

        if (tryscore < mywindow.GetBeta())
        {
          // The PV move is recorded as the start of the local PV

          CopyUpLocalPV(ml.FetchMove(index));

          // Special extra handling for a new PV at the root

          if ((ply == 0) && !isprelim)
          {
            // Record the new PV as the prior PV and apply marking to the prior PV

            pvtable.CopyToPrior(tryscore); MarkPV(ml);

            // Trace: Predicted variation

            if (TestOptionM(optnmPV)) {PrintOptn(optnPV); PrintPVAndScore(); PrintNL();};

            // Move the new ply zero PV move to the front of the root move list

            ml.ShiftIndexedMoveToFront(index);
          };
        };
      };

      // End of the move loop
    };

    // All moves searched; clean up processing if the search is still in progress

    if (NotStopped())
    {
      // Handle the best move, if any

      if (bestmove.IsNotVoid() && bestmove.IsHolder()) killers[ply] = bestmove;

      // Checkmate/stalemate detection

      if (!alo && (abn != abnGain)) mywindow.PutAlfa(InCheck() ? svLoseIn0 : svEven);
    };

    // Restore the current depth in case of extensions

    depth = savedepth;
  };

  // Clear first time down status and return alpha as this node's value

  isftdown = false; if (abort) mywindow.PutAlfa(svBroken);
  return mywindow.GetAlfa();
}

svType Search::ABRootNode(const Window& window, const ML& priorml)
{
  // Start a tree search with the current position as the root node

  isftdown = true; pvtable.ResetTrace();

  // Perform the search and return the score value result

  return ABNode(window, priorml);
}

void Search::ABIteration(const plyType iteration)
{
  Window mywindow;

  // Trace: Iteration

  if (TestOptionM(optnmIT))
  {
    PrintOptn(optnIT); PrintFSL(fsLbIteration); PrintUi16(iteration + 1); PrintNL();
  };

  // Perform a full width window search iteration

  mywindow.SetFullWidth(); depth = iteration + 1; ABRootNode(mywindow, rootml);
}

void Search::ABIterationSequence(const plyType limitply)
{
  plyType iteration = 0;

  while (NotStopped())
  {
    // Run the iteration

    ABIteration(iteration);

    // Check for forced mate

    if (NotStopped() && IsSvMating(PVMove(0).GetSv())) Stop(stForcedMate);

    // Check for forced lose

    if (NotStopped() && IsSvLosing(PVMove(0).GetSv())) Stop(stForcedLose);

    // Check for another iteration attempt

    if (NotStopped())
    {
      if (iteration < (limitply - 1)) iteration++; else Stop(stLimitDepth);
    };
  };
}

void Search::ABSearch(const plyType limitply)
{
  // Generate the root move list

  GenCanonical(rootml);

  // If not yet finished: Check for a no moves condition termination

  if (NotStopped())
  {
    // Test for no moves available (checkmate or stalemate initial condition)

    if (rootml.GetCount() == 0) Stop(stNoMoves);
  };

  // If not yet finished: Set certain scores (checkmating moves)

  if (NotStopped())
  {
    // The root move list already has notiaton markings including mate markings

    for (miType index = 0; index < rootml.GetCount(); index++)
      if (RootMove(index).IsMate()) RootMove(index).SetCertain(svMateIn1);
  };

  // If not yet finished: Check for a mate in one termination

  if (NotStopped())
  {
    const miType index = rootml.FindBestCertainIndex();

    if ((index >= 0) && (RootMove(index).GetSv() == svMateIn1))
    {
      pvtable.SetSingleMovePV(RootMove(index)); Stop(stMateIn1);
    };
  };

  // If not yet finished: Set certain scores (drawing moves)

  if (NotStopped())
  {
    for (miType index = 0; index < rootml.GetCount(); index++)
    {
      if (RootMove(index).IsNotCertain())
      {
        DoExecute(RootMove(index));
        if (IsDraw()) RootMove(index).SetCertain(svEven);
        DoRetract();
      };
    };
  };

  // If not yet finished: Check for an all moves certain scores termination

  if (NotStopped())
  {
    // All moves might be forced draws

    if (rootml.AllScoresCertain())
    {
      const miType index = rootml.FindBestCertainIndex();

      pvtable.SetSingleMovePV(RootMove(index)); Stop(stAllCertain);
    };
  };

  // If not yet finished: Check for a singleton condition termination

  if (NotStopped())
  {
    // Test for only one move available

    if (rootml.GetCount() == 1)
    {
      RootMove(0).PutSv(svEven);
      pvtable.SetSingleMovePV(RootMove(0)); Stop(stSingleton);
    };
  };

  // If not yet finished: Check for a book move

  if (NotStopped())
  {
    // Test for "no opening library" option deselection

    if (!TestOptionM(optnmNL))
    {
      // Pick a book move (if any)

      const miType index = Book::PickBookMove(rootml, spos);

      if (index >= 0)
      {
        // Opening book move was selected; handle as search result

        RootMove(index).PutSv(svEven);
        pvtable.SetSingleMovePV(RootMove(index)); Stop(stBook);
      };
    };
  };

  // If not yet finished: Perform a general search

  if (NotStopped())
  {
    // Perform the initial scoring and ordering of the root move list

    ScoreAndSortRootML();

    // Either out of time (rarely), or a general search is performed

    if (IsPastTime())
    {
      RootMove(0).PutSv(svEven); pvtable.SetSingleMovePV(RootMove(0)); Stop(stLimitTime);
    }
    else
      ABIterationSequence(limitply);
  };
}

void Search::RunABSearch(const ui32 limitmsec, const plyType limitply)
{
  // Initial activity LED update

  actledstate = true; UpdateActivityLED();

  // Handle input FEN trace

  Trace0();

  // Handle search input parameters trace

  if (TestOptionM(optnmIP))
  {
    PrintOptn(optnIP);
    PrintFSL(fsLbTimeLimit); PrintMsecAsSeconds(limitmsec); PrintISM();
    PrintFSL(fsLbDepthLimit); PrintUi16(limitply); PrintNL();
  };

  // Perform the pre-search initialization

  ResetAux(); timeoutmsec = startmsec + limitmsec;

  // Run the search and get the timing

  RedrawBoardDisplay();
  ABSearch(limitply); usedmsec = ElapsedMsec() - startmsec;
  RedrawBoardDisplay();

  // Search termination trace

  if (TestOptionM(optnmST)) {PrintOptn(optnST); PrintSearchTermination(st); PrintNL();};

  // Search result trace

  if (TestOptionM(optnmSR))
  {
    PrintOptn(optnSR); if (PVMove(0).IsVoid()) PrintFS(fsMsNoResult); else PrintPVAndScore();
    PrintNL();
  };

  // Handle the search timing statistics trace

  Trace1();

  // Final activity LED update

  actledstate = false; UpdateActivityLED();
}

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
#include "FPos.h"
#include "Move.h"
#include "SAN.h"
#include "ML.h"
#include "Hash.h"
#include "TBV.h"
#include "PEnv.h"
#include "Pos.h"

void ML::MarkChecksAndCheckmates(Pos &pos) const
{
  // Add move flags for checks and checkmates as appropriate

  for (miType index = 0; index < count; index++)
  {
    FEnv fenv;
    tidType capttid;
    PEnv penv;

    pos.Execute(FetchMove(index), fenv, capttid, penv);
    if (pos.InCheck())
    {
      RefMove(index).SetFlagM(mfmChck);
      if (pos.NoMoves()) RefMove(index).SetFlagM(mfmMate);
    };
    pos.Retract(FetchMove(index), fenv, capttid, penv);
  };
}

void ML::MarkDisambiguation(const Pos& pos) const
{
  // Add move flags for file and rank disambiguation as appropriate

  ui8 tosqrcount[sqrLen];

  // Initialize the destination square census vector

  for (sqrType sqr = 0; sqr < sqrLen; sqr++) tosqrcount[sqr] = 0;
  for (miType index = 0; index < count; index++) tosqrcount[FetchMove(index).GetToSqr()]++;

  // Run the outer move loop

  for (miType index0 = 0; index0 < count; index0++)
  {
    const manType frman = FetchMove(index0).GetFrMan();

    // Only certain piece kinds may need disambiguation

    switch (cvmantopiece[frman])
    {
      case piecePawn:
      case pieceKing:
        break;

      case pieceKnight:
      case pieceBishop:
      case pieceRook:
      case pieceQueen:

        // Is there more than one piece of the moving piece kind?

        if (pos.GetManCountByMan(frman) > 1)
        {
          const sqrType tosqr = FetchMove(index0).GetToSqr();

          // Is there more than one piece moving to the destination square?

          if (tosqrcount[tosqr] > 1)
          {
            // Prepare to run the inner move loop to find possible tosqr/frman matches

            const sqrType frsqr = FetchMove(index0).GetFrSqr();
            const fileType frfile = MapSqrToFile(frsqr);
            const rankType frrank = MapSqrToRank(frsqr);
            ui frmantosqrmc = 0, frfilemc = 0, frrankmc = 0;

            // Run the inner move loop

            for (miType index1 = 0; index1 < count; index1++)
            {
              // Check for a tosqr/frman match

              if ((FetchMove(index1).GetToSqr() == tosqr) &&
                (FetchMove(index1).GetFrMan() == frman))
              {
                // Count from-file/from-rank matches

                const sqrType auxfrsqr = FetchMove(index1).GetFrSqr();

                frmantosqrmc++;
                if (MapSqrToFile(auxfrsqr) == frfile) frfilemc++;
                if (MapSqrToRank(auxfrsqr) == frrank) frrankmc++;
              };
            };

            // Was there more than the destination square match?

            if (frmantosqrmc > 1)
            {
              // File disambiguation has priority, but may need rank or both file and rank

              if ((frrankmc > 1) || ((frrankmc == 1) && (frfilemc == 1)))
                RefMove(index0).SetFlagM(mfmAdaf);
              if (frfilemc > 1) RefMove(index0).SetFlagM(mfmAdar);
            };
          };
        };
        break;

      default: SwitchFault(); break;
    };
  };
}

void ML::MarkNotation(Pos& pos) const
{
  // Generate/apply notation flags; first check/checkmates, then disambiguation

  MarkChecksAndCheckmates(pos); MarkDisambiguation(pos);
}

void ML::SortBySAN(void) const
{
  // Order moves by ascending SAN (ASCII); a simple bubble sort works well here

  if (count > 1)
  {
    const miType passlimit = count - 1;
    miType pass = 0;
    bool swap = true;

    while (swap && (pass < passlimit))
    {
      const miType pairlimit = passlimit - pass;

      swap = false;
      for (miType index0 = 0; index0 < pairlimit; index0++)
      {
        const miType index1 = index0 + 1;
        const SAN m0san(FetchMove(index0)), m1san(FetchMove(index1));

        if (StrCmp(m0san.FetchStr(), m1san.FetchStr()) > 0)
        {
          const Move tmpmove = FetchMove(index0);

          RefMove(index0) = FetchMove(index1); RefMove(index1) = tmpmove; swap = true;
        };
      };
      pass++;
    };
  };
}

void ML::SortBySv(void) const
{
  // Order moves by descending score value; a simple bubble sort works well here

  if (count > 1)
  {
    const miType passlimit = count - 1;
    miType pass = 0;
    bool swap = true;

    while (swap && (pass < passlimit))
    {
      const miType pairlimit = passlimit - pass;

      swap = false;
      for (miType index0 = 0; index0 < pairlimit; index0++)
      {
        const miType index1 = index0 + 1;
        const svType sv0 = FetchMove(index0).GetSv(), sv1 = FetchMove(index1).GetSv();

        if ((sv1 > sv0) ||
          ((sv1 == sv0) && (FetchMove(index1).GetFrMan() < FetchMove(index0).GetFrMan())))
        {
          const Move tmpmove = FetchMove(index0);

          RefMove(index0) = FetchMove(index1); RefMove(index1) = tmpmove; swap = true;
        };
      };
      pass++;
    };
  };
}

miType ML::LocateNoFail(const Move& move) const
{
  // Return the index of the given move; signal a fatal error if the move is not found

  const miType match = Locate(move);

  if (match < 0) {PrintFS(fsFeLocateFault); move.Print(); PrintNL(); Die();};
  return match;
}

void ML::MatchMove(const char *str, Move& move) const
{
  // Return the move of the given SAN move string; return void move if the move is not found

  miType index = 0;

  move.MakeVoid();
  while (move.IsVoid() && (index < count))
  {
    const SAN san(FetchMove(index));

    if (StrCmp(str, san.FetchStr()) == 0) move = FetchMove(index); else index++;
  };
}

void ML::ClearSearchFlags(void) const
{
  // Clear the "has been searched" flag in all the moves

  for (miType index = 0; index < count; index++) RefMove(index).ResetFlagM(mfmSrch);
}

miType ML::FindBestCertainIndex(void) const
{
  // OF all moves with certain scores, return the index of the best one

  miType bestindex = -1;
  svType bestsv = svBroken;

  for (miType index = 0; index < count; index++)
  {
    if (FetchMove(index).IsCertain() && (FetchMove(index).GetSv() > bestsv))
    {
      bestindex = index; bestsv = FetchMove(index).GetSv();
    };
  };
  return bestindex;
}

bool ML::AllScoresCertain(void) const
{
  // Return true if all the moves have certain score values

  bool allcertain = true;
  miType index = 0;

  while (allcertain && (index < count))
    if (FetchMove(index).IsNotCertain()) allcertain = false; else index++;
  return allcertain;
}

void ML::AssignMaxGain(void) const
{
  // For each move, assign the maximum estimated gain metric

  for (miType index = 0; index < count; index++)
    RefMove(index).PutSv(FetchMove(index).MaxGain());
}

void ML::AssignLikelyGain(const Pos& pos) const
{
  // For each move, assign the likely estimated gain metric

  for (miType index = 0; index < count; index++)
    RefMove(index).PutSv(FetchMove(index).LikelyGain(pos));
}

void ML::AssignCenterTropismGain(void) const
{
  // For each move, assign the center tropism gain metric

  for (miType index = 0; index < count; index++)
    RefMove(index).PutSv(FetchMove(index).CenterTropismGain());
}

void ML::ShiftIndexedMoveToFront(const miType index)
{
  // Move the move with the given index to the front of the list

  if (index > 0)
  {
    const Move newtopmove = FetchMove(index);

    for (miType toindex = index; toindex > 0; toindex--)
      RefMove(toindex) = FetchMove(toindex - 1);
    RefMove(0) = newtopmove;
  };
}

void ML::FilterByFrSqr(const sqrType sqr)
{
  // Remove all moves that don't have the indicated from-square

  for (miType index = 0; index < count; index++)
    if (FetchMove(index).GetFrSqr() != sqr) RefMove(index).MakeVoid();
  RemoveVoidMoves();
}

void ML::FilterByToSqr(const sqrType sqr)
{
  // Remove all moves that don't have the indicated to-square

  for (miType index = 0; index < count; index++)
    if (FetchMove(index).GetToSqr() != sqr) RefMove(index).MakeVoid();
  RemoveVoidMoves();
}

void ML::FilterByFrMan(const manType man)
{
  // Remove all moves that don't have the indicated from-man

  for (miType index = 0; index < count; index++)
    if (FetchMove(index).GetFrMan() != man) RefMove(index).MakeVoid();
  RemoveVoidMoves();
}

void ML::FilterByToMan(const manType man)
{
  // Remove all moves that don't have the indicated to-man

  for (miType index = 0; index < count; index++)
    if (FetchMove(index).GetToMan() != man) RefMove(index).MakeVoid();
  RemoveVoidMoves();
}

void ML::FilterByMsc(mscType msc)
{
  // Remove all moves that don't have the indicated to-man

  for (miType index = 0; index < count; index++)
    if (FetchMove(index).GetMsc() != msc) RefMove(index).MakeVoid();
  RemoveVoidMoves();
}

void ML::FilterByMove(const Move& move)
{
  // Remove all moves that aren't the given move

  for (miType index = 0; index < count; index++)
    if (!Move::SameMove(FetchMove(index), move)) RefMove(index).MakeVoid();
  RemoveVoidMoves();
}

void ML::RemoveVoidMoves(void)
{
  // Remove all moves that are void

  ui delta = 0;

  for (miType index = 0; index < count; index++)
  {
    if (FetchMove(index).IsVoid()) delta++;
    else
    {
      if (delta) RefMove(index - delta) = FetchMove(index);
    };
  };

  if (delta) {count -= delta; nextptr = baseptr + count;};
}

void ML::Print(void) const
{
  // Print the move list

  for (miType index = 0; index < count; index++)
  {
    if (index > 0) PrintSpace();
    FetchMove(index).Print();
  };
}

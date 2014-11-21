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
#include "SS.h"
#include "SSKnight.h"
#include "SSKing.h"
#include "FEnv.h"
#include "FPos.h"
#include "Move.h"
#include "ML.h"
#include "Hash.h"
#include "TBV.h"
#include "PEnv.h"
#include "Pos.h"

void Pos::GenAddNonEvasionFromSquarePawn(
  const genmType genm, const sqrType frsqr, const manType frman, ML& ml) const
{
  const tidType tid = sqrtotid[frsqr];

  // Frozen men are unmovable

  if (!frozentbv.TestTid(tid))
  {
    // Determine pinned status and restriction bidirection, if any

    const bool pinflag = pinnedtbv.TestTid(tid);
    const bdrType bdr =
      (pinflag ? cvsweepdirtobdr[Board::ExtractDirection(LocateGoodKing(), frsqr)] : bdrNil);

    SS ss(frsqr);
    Move move;
    sqrType tosqr;
    manType toman;

    // Fill the move fields with default values for this pawn

    move.PutFrSqr(frsqr); move.PutFrMan(frman);
    move.PutToMan(manVacant); move.PutMsc(mscReg); move.PutFlags(0); move.PutSv(svBroken);

    const rankType frrank = MapSqrToRank(frsqr);
    const bool r2flag = (frrank == r2rank[GetGood()]), r7flag = (frrank == r7rank[GetGood()]);

    // Pawn advancement, including promotion

    if (!pinflag || (bdr == bdrN))
    {
      ss.SetDirection(pawnadvdir[GetGood()]); tosqr = ss.Advance();
      if (IsSqrNotNil(tosqr) && IsOccupantVacant(tosqr))
      {
        move.PutToSqr(tosqr);
        if ((genm & genmGain) && r7flag)
        {
          for (ui promindex = 0; promindex < promLen; promindex++)
          {
            move.PutMsc(cvpromtomsc[promindex]); ml.Push(move);
          };
          move.PutMsc(mscReg);
        };
        if ((genm & genmHold) && !r7flag)
        {
          ml.Push(move);
          if (r2flag)
          {
            tosqr = ss.Advance();
            if (IsOccupantVacant(tosqr)) {move.PutToSqr(tosqr); ml.Push(move);};
          };
        };
      };
    };

    // Pawn capturing

    if (genm & genmGain)
    {
      for (flankType flank = 0; flank < flankLen; flank++)
      {
        const dirType capdir = pawncapdir[GetGood()][flank];

        if (!pinflag || (cvsweepdirtobdr[capdir] == bdr))
        {
          ss.SetDirection(capdir); tosqr = ss.Advance();
          if (IsSqrNotNil(tosqr))
          {
            move.PutToSqr(tosqr);
            if (cvmantocolor[toman = GetMan(tosqr)] == GetEvil())
            {
              move.PutToMan(toman);
              if (r7flag)
              {
                for (ui promindex = 0; promindex < promLen; promindex++)
                {
                  move.PutMsc(cvpromtomsc[promindex]); ml.Push(move);
                };
                move.PutMsc(mscReg);
              }
              else
                ml.Push(move);
              move.PutToMan(manVacant);
            };

            // En passant captures

            if ((GetEpsq() == tosqr) && IsEnPassantLegal(frsqr, LocateGoodKing()))
            {
              move.PutMsc(mscEPC); ml.Push(move); move.PutMsc(mscReg);
            };
          };
        };
      };
    };
  };
}

void Pos::GenAddNonEvasionFromSquareKnight(
  const genmType genm, const sqrType frsqr, const manType frman, ML& ml) const
{
  // Frozen men are unmovable

  if (!frozentbv.TestTid(sqrtotid[frsqr]))
  {
    SSKnight ssknight(frsqr);
    sqrType tosqr;
    Move move;

    // Fill the move fields with default values for this knight

    move.PutFrSqr(frsqr); move.PutFrMan(frman);
    move.PutToMan(manVacant); move.PutMsc(mscReg); move.PutFlags(0); move.PutSv(svBroken);

    // Scan through the run square sequence for this knight

    while (IsSqrNotNil(tosqr = ssknight.Advance()))
    {
      const manType toman = GetMan(tosqr);
      const colorType tocolor = cvmantocolor[toman];

      if (tocolor != GetGood())
      {
        move.PutToSqr(tosqr);
        if ((genm & genmGain) && IsColorEvil(tocolor))
        {
          move.PutToMan(toman); ml.Push(move); move.PutToMan(manVacant);
        };
        if ((genm & genmHold) && IsColorVacant(tocolor)) ml.Push(move);
      };
    };
  };
}

void Pos::GenAddNonEvasionFromSquareSweep(
  const genmType genm, const sqrType frsqr, const manType frman, ML& ml) const
{
  const tidType tid = sqrtotid[frsqr];

  // Frozen men are unmovable

  if (!frozentbv.TestTid(tid))
  {
    const bool pinflag = pinnedtbv.TestTid(tid);
    const bdrType bdr =
      (pinflag ? cvsweepdirtobdr[Board::ExtractDirection(LocateGoodKing(), frsqr)] : bdrNil);

    const dirType stopdir = mands1dir[frman];
    SS ss(frsqr);
    Move move;

    // Fill the move fields with default values for this sweeper

    move.PutFrSqr(frsqr); move.PutFrMan(frman);
    move.PutToMan(manVacant); move.PutMsc(mscReg); move.PutFlags(0); move.PutSv(svBroken);

    // Loop through each sweep direction for this sweeper

    for (dirType dir = mands0dir[frman]; dir <= stopdir; dir++)
    {
      if (!pinflag || (cvsweepdirtobdr[dir] == bdr))
      {
        sqrType tosqr;
        manType toman;

        ss.SetDirection(dir);
        while (IsSqrNotNil(tosqr = ss.Advance()) && IsManVacant(toman = GetMan(tosqr)))
          if (genm & genmHold) {move.PutToSqr(tosqr); ml.Push(move);};
        if (IsSqrNotNil(tosqr) && IsManEvil(toman) && (genm & genmGain))
        {
          move.PutToSqr(tosqr); move.PutToMan(toman); ml.Push(move); move.PutToMan(manVacant);
        };
      };
    };
  };
}

void Pos::GenAddNonEvasionFromSquareKing(
  const genmType genm, const sqrType frsqr, const manType frman, ML& ml) const
{
  SSKing ssking(frsqr);
  sqrType tosqr;
  Move move;

  // Fill the move fields with default values for this king

  move.PutFrSqr(frsqr); move.PutFrMan(frman);
  move.PutToMan(manVacant); move.PutMsc(mscReg); move.PutFlags(0); move.PutSv(svBroken);

  // Castling

  if ((genm & genmHold) && IsSomeCastling())
  {
    for (flankType flank = 0; flank < flankLen; flank++)
    {
      const castType cast = cvcolorflanktocast[GetGood()][flank];

      if (IsCastlingLegal(cast))
      {
        move.PutToSqr(kingawaysqr[cast]); move.PutMsc(cvflanktomsc[flank]); ml.Push(move);
        move.PutMsc(mscReg);
      };
    };
  };

  // Non castling; scan through the run square sequence for this king

  while (IsSqrNotNil(tosqr = ssking.Advance()))
  {
    const manType toman = GetMan(tosqr);
    const colorType tocolor = cvmantocolor[toman];

    if (tocolor != GetGood())
    {
      if (!EvilAttacksSquare(tosqr))
      {
        move.PutToSqr(tosqr);
        if ((genm & genmGain) && IsColorEvil(tocolor))
        {
          move.PutToMan(toman); ml.Push(move); move.PutToMan(manVacant);
        };
        if ((genm & genmHold) && IsColorVacant(tocolor)) ml.Push(move);
      };
    };
  };
}

void Pos::GenAddNonEvasionFromSquare(const genmType genm, sqrType frsqr, ML& ml) const
{
  const manType frman = GetMan(frsqr);

  switch (cvmantopiece[frman])
  {
    case piecePawn:
      GenAddNonEvasionFromSquarePawn(genm, frsqr, frman, ml);
      break;

    case pieceKnight:
      GenAddNonEvasionFromSquareKnight(genm, frsqr, frman, ml);
      break;

    case pieceBishop:
    case pieceRook:
    case pieceQueen:
      GenAddNonEvasionFromSquareSweep(genm, frsqr, frman, ml);
      break;

    case pieceKing:
      GenAddNonEvasionFromSquareKing(genm, frsqr, frman, ml);
      break;

    default:
      SwitchFault();
      break;
  };
}

void Pos::GenNonEvasion(const genmType genm, ML& ml) const
{
  TBV scantbv = tbvbc[GetGood()];
  tidType scantid;

  ml.ResetCount();
  while (IsTidNotNil(scantid = scantbv.NextTid()))
    GenAddNonEvasionFromSquare(genm, tidtosqr[scantid], ml);
}

void Pos::GenEvasion(ML& ml) const
{
  const tidType goodkingtid = cvcolortokingtid[GetGood()];
  const sqrType goodkingsqr = tidtosqr[goodkingtid];
  const manType goodkingman = GetMan(goodkingsqr);
  const manType goodpawnman = GoodMan(piecePawn);
  const bool singlecheck = (checkercount == 1);
  const tidType checkertid = (singlecheck ? checkertbv.FirstTid() : tidNil);
  const sqrType checkersqr = (singlecheck ? tidtosqr[checkertid] : sqrNil);
  const manType checkerman = (singlecheck ? GetMan(checkersqr) : manNil);

  ml.ResetCount();

  // Capture single checker, but not with the king

  if (singlecheck)
  {
    TBV defendertbv;
    tidType defendertid;
    Move move;

    CalcColorAttacksToSquare(GetGood(), checkersqr, defendertbv);
    defendertbv.ResetTid(goodkingtid);
    move.PutToSqr(checkersqr); move.PutToMan(checkerman);
    move.PutMsc(mscReg); move.PutFlags(0); move.PutSv(svBroken);
    while (IsTidNotNil(defendertid = defendertbv.NextTid()))
    {
      if (!pinnedtbv.TestTid(defendertid))
      {
        const sqrType defendersqr = tidtosqr[defendertid];
        const manType defenderman = GetMan(defendersqr);

        move.PutFrSqr(defendersqr); move.PutFrMan(defenderman);
        if ((defenderman == goodpawnman) && (MapSqrToRank(defendersqr) == r7rank[GetGood()]))
        {
          for (ui promindex = 0; promindex < promLen; promindex++)
          {
            move.PutMsc(cvpromtomsc[promindex]); ml.Push(move);
          };
          move.PutMsc(mscReg);
        }
        else
          ml.Push(move);
      };
    };
  };

  // Move king; includes captures

  {
    TBV attackertbv(checkertbv);
    sqrType tosqrs[dirSLen];

    // Move king/pass one: (simple destination validation)

    for (dirType dir = dirE; dir <= dirSE; dir++)
    {
      const sqrType tosqr = Board::FetchNextSquare(goodkingsqr, dir);

      if (IsSqrNil(tosqr)) tosqrs[dir] = sqrNil;
      else
        tosqrs[dir] = IsOccupantGood(tosqr) ? sqrNil : tosqr;
    };

    // Move king/pass two: (shadow square deletion)

    tidType attackertid;

    while (IsTidNotNil(attackertid = attackertbv.NextTid()))
    {
      const sqrType attackersqr = tidtosqr[attackertid];

      if (sweeperflag[GetMan(attackersqr)])
        tosqrs[Board::ExtractDirection(attackersqr, goodkingsqr)] = sqrNil;
    };

    // Move king/pass three: (hazard deletion)

    for (dirType dir = dirE; dir <= dirSE; dir++)
    {
      const sqrType tosqr = tosqrs[dir];

      if (IsSqrNotNil(tosqr) && EvilAttacksSquare(tosqr)) tosqrs[dir] = sqrNil;
    };

    // Move king/pass four: (move generation)

    {
      Move move;

      move.PutFrSqr(goodkingsqr); move.PutFrMan(goodkingman);
      move.PutMsc(mscReg); move.PutFlags(0); move.PutSv(svBroken);
      for (dirType dir = dirE; dir <= dirSE; dir++)
      {
        const sqrType tosqr = tosqrs[dir];

        if (IsSqrNotNil(tosqr))
        {
          move.PutToSqr(tosqr); move.PutToMan(GetMan(tosqr)); ml.Push(move);
        };
      };
    };
  };

  // Interpose against single checking sweeper

  if (singlecheck && sweeperflag[checkerman])
  {
    const dirType dir = Board::ExtractDirection(goodkingsqr, checkersqr);
    const sqrdeltaType del = sqrdels[dir];
    sqrType tosqr = goodkingsqr + del;

    while (tosqr != checkersqr)
    {
      // Non-pawn/non-king interpositions

      TBV interposertbv;
      tidType interposertid;

      CalcColorAttacksToSquare(GetGood(), tosqr, interposertbv);
      interposertbv.ResetTid(goodkingtid);
      while (IsTidNotNil(interposertid = interposertbv.NextTid()))
      {
        const sqrType interposersqr = tidtosqr[interposertid];
        const manType interposerman = GetMan(interposersqr);

        if ((interposerman != goodpawnman) && !pinnedtbv.TestTid(interposertid))
        {
          Move move;

          move.PutFrSqr(interposersqr); move.PutToSqr(tosqr);
          move.PutFrMan(interposerman); move.PutToMan(manVacant);
          move.PutMsc(mscReg); move.PutFlags(0); move.PutSv(svBroken); ml.Push(move);
        };
      };

      // Single square advance pawn interpositions

      const sqrType pawn1frsqr = Board::FetchNextSquare(tosqr, pawnadvdir[GetEvil()]);

      if (IsSqrNotNil(pawn1frsqr))
      {
        const manType pawn1man = GetMan(pawn1frsqr);

        if (pawn1man == goodpawnman)
        {
          const tidType pawn1tid = sqrtotid[pawn1frsqr];

          if (!pinnedtbv.TestTid(pawn1tid))
          {
            Move move;

            move.PutFrSqr(pawn1frsqr); move.PutToSqr(tosqr);
            move.PutFrMan(goodpawnman); move.PutToMan(manVacant);
            move.PutFlags(0); move.PutSv(svBroken);

            if (MapSqrToRank(tosqr) != r8rank[GetGood()])
            {
              move.PutMsc(mscReg); ml.Push(move);
            }
            else
            {
              for (ui promindex = 0; promindex < promLen; promindex++)
              {
                move.PutMsc(cvpromtomsc[promindex]); ml.Push(move);
              };
            };
          };
        }
        else
        {
          // Double square advance pawn interpositions

          if (IsManVacant(pawn1man))
          {
            const sqrType pawn2frsqr = Board::FetchNextSquare(pawn1frsqr, pawnadvdir[GetEvil()]);

            if (IsSqrNotNil(pawn2frsqr) && (MapSqrToRank(pawn2frsqr) == r2rank[GetGood()]))
            {
              const manType pawn2man = GetMan(pawn2frsqr);

              if (pawn2man == goodpawnman)
              {
                const tidType pawn2tid = sqrtotid[pawn2frsqr];

                if (!pinnedtbv.TestTid(pawn2tid))
                {
                  Move move;

                  move.PutFrSqr(pawn2frsqr); move.PutToSqr(tosqr);
                  move.PutFrMan(goodpawnman); move.PutToMan(manVacant);
                  move.PutFlags(0); move.PutMsc(mscReg); move.PutSv(svBroken); ml.Push(move);
                };
              };
            };
          };
        };
      };
      tosqr += del;
    };
  };

  // En passant capture

  if (IsEpsqNotNil())
  {
    for (flankType flank = 0; flank < flankLen; flank++)
    {
      const sqrType frsqr = Board::FetchNextSquare(GetEpsq(), pawncapdir[GetEvil()][flank]);

      if (IsSqrNotNil(frsqr) && (GetMan(frsqr) == goodpawnman) &&
        IsEnPassantLegal(frsqr, goodkingsqr))
      {
        Move move;

        move.PutFrSqr(frsqr); move.PutToSqr(GetEpsq());
        move.PutFrMan(goodpawnman); move.PutToMan(manVacant);
        move.PutMsc(mscEPC); move.PutFlags(0); move.PutSv(svBroken); ml.Push(move);
      };
    };
  };
}

void Pos::GenNonEvasion(ML& ml) const {GenNonEvasion(genmAll, ml);}

void Pos::Gen(ML& ml) const
{
  // Top level move generation is based on in-check status

  if (InCheck()) GenEvasion(ml); else GenNonEvasion(ml);
}

void Pos::GenMarked(ML& ml) {Gen(ml); ml.MarkNotation(*this);}

void Pos::GenCanonical(ML& ml) {GenMarked(ml); ml.SortBySAN();}

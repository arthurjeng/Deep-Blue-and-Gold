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
#include "ML.h"
#include "Hash.h"
#include "TBV.h"
#include "PEnv.h"
#include "Pos.h"

void Pos::AddMan(const sqrType sqr, const manType man, const tidType tid)
{
  // Add a man to the position using incremental updates

  const colorType color = cvmantocolor[man];

  PutMan(sqr, man);
  mcbc[color]++; mcbm[man]++; msbc[color] += cvmantosv[man];
  sqrtotid[sqr] = tid; tidtosqr[tid] = sqr; targettbv.SetTid(tid);
  if (sweeperflag[man]) sweepertbv.SetTid(tid);
  tbvbc[color].SetTid(tid); tbvbm[man].SetTid(tid); pdhash.FoldManSqr(man, sqr);
}

void Pos::DelMan(const sqrType sqr)
{
  // Delete a man fram the position using incremental updates

  const manType man = GetMan(sqr);
  const colorType color = cvmantocolor[man];
  const tidType tid = sqrtotid[sqr];

  ResetMan(sqr);
  --mcbc[color]; --mcbm[man]; msbc[color] -= cvmantosv[man];
  sqrtotid[sqr] = tidNil; tidtosqr[tid] = sqrNil; targettbv.ResetTid(tid);
  if (sweeperflag[man]) sweepertbv.ResetTid(tid);
  tbvbc[color].ResetTid(tid); tbvbm[man].ResetTid(tid); pdhash.FoldManSqr(man, sqr);
}

void Pos::MoveMan(const sqrType frsqr, const sqrType tosqr)
{
  // Move a man in the position using incremental updates

  const manType man = GetMan(frsqr);
  const tidType tid = sqrtotid[frsqr];

  Board::MoveMan(frsqr, tosqr);
  sqrtotid[frsqr] = tidNil; sqrtotid[tosqr] = tid; tidtosqr[tid] = tosqr;
  pdhash.FoldManSqrSqr(man, frsqr, tosqr);
}

void Pos::Execute(const Move& move)
{
  const sqrType frsqr = move.GetFrSqr(), tosqr = move.GetToSqr();
  const mscType msc = move.GetMsc();
  const bool null = move.IsNull();

  // Clear the captured man target ID

  capttid = tidNil;

  // Now do the locomotion for a non-null move

  if (!null)
  {
    // Process according to the move special case

    if (move.IsRegular())
    {
      // Do a regular move

      if (move.IsSimpleCapture()) {capttid = sqrtotid[tosqr]; DelMan(tosqr);};
      MoveMan(frsqr, tosqr);
    }
    else
    {
      if (move.IsEnPassant())
      {
        // Do an en passant move

        const sqrType victimsqr = move.CalcEPVictimSqr();

        capttid = sqrtotid[victimsqr]; DelMan(victimsqr); MoveMan(frsqr, tosqr);
      }
      else
      {
        if (move.IsCastling())
        {
          // Do a castling move

          const castType cast = cvcolorflanktocast[GetGood()][cvmsctoflank[msc]];

          MoveMan(frsqr, tosqr); MoveMan(rookhomesqr[cast], rookawaysqr[cast]);
        }
        else
        {
          // Do a promotion move

          const tidType promtid = sqrtotid[frsqr];

          if (move.IsSimpleCapture()) {capttid = sqrtotid[tosqr]; DelMan(tosqr);};
          DelMan(frsqr);
          AddMan(tosqr, GoodMan(cvpromtopiece[cvmsctoprom[msc]]), promtid);
        };
      };
    };
  };

  // Color update; good and evil are swapped

  ToggleColors();

  // Castling update (non-null move only)

  if (!null && IsSomeCastling())
  {
    // Scan through all four possible castlings

    for (castType cast = 0; cast < castLen; cast++)
    {
      // Is there a potential castling cancellation?

      if (TestCast(cast))
      {
        const sqrType kingsqr = kinghomesqr[cvcasttocolor[cast]];
        const sqrType rooksqr = rookhomesqr[cast];

        // Test for a cancellation

        if ((frsqr == kingsqr) || (frsqr == rooksqr) || (tosqr == rooksqr))
        {
          // Perform the cancellation including hash update

          PutCsab(GetCsab() & ~BX(cast)); pdhash.FoldCast(cast);
        };
      };
    };
  };

  // En passant update: cancel the active en passant target, if any

  if (IsEpsqNotNil()) {pdhash.FoldEpSqr(GetEpsq()); PutEpsq(sqrNil);}

  // En passant update: set a new active en passant target, if any

  if (!null && move.IsPawnTwoStep())
  {
    const fileType epfile = MapSqrToFile(tosqr);
    const manType pawnman = GoodMan(piecePawn);

    // Test for a neighboring enemy pawn to the double step advanced pawn

    if (((epfile > fileA) && (GetMan(tosqr + delW) == pawnman)) ||
      ((epfile < fileH) && (GetMan(tosqr + delE) == pawnman)))
    {
      // A new en passant target square is set; include hash update

      PutEpsq((frsqr + tosqr) >> 1); pdhash.FoldEpFile(epfile);
    };
  };

  // Half move counter and full move number updates

  if (!null && move.IsHmvcReset()) PutHmvc(0); else IncHmvc();
  if (IsWTM()) IncFmvn();

  // Position member regeneration

  Regen();

  // Now that wasn't too hard, was it?

  // if (!IsSane()) DieStr("Pos::Execute: Not sane");
}

void Pos::Execute(const Move& move, FEnv& fenv, tidType& tid, PEnv& penv)
{
  // Save retraction data, then execute the given move

  fenv = *(FEnv *) this; tid = capttid; penv = *(PEnv *) this;
  Execute(move);
}

void Pos::Retract(const Move& move, const FEnv& fenv, const tidType savedtid)
{
  const csabType savedcsab = GetCsab();
  const sqrType savedepsq = GetEpsq();

  // Restore the FEN environment (base class)

  *(FEnv *) this = fenv;

  // Handle the castling hash code restoration

  if (GetCsab() != savedcsab)
  {
    const csabType deltacsab = GetCsab() ^ savedcsab;

    for (castType cast = 0; cast < castLen; cast++)
      if (deltacsab & BX(cast)) pdhash.FoldCast(cast);
  };

  // Handle the en passant hash code restoration

  if (GetEpsq() != savedepsq)
  {
    if (IsSqrNotNil(savedepsq)) pdhash.FoldEpSqr(savedepsq);
    if (IsEpsqNotNil()) pdhash.FoldEpSqr(GetEpsq());
  };

  // Main restoration for a non-null move

  if (!move.IsNull())
  {
    const sqrType frsqr = move.GetFrSqr(), tosqr = move.GetToSqr();
    const manType frman = move.GetFrMan(), toman = move.GetToMan();

    if (move.IsRegular())
    {
      // Undo a regular move

      MoveMan(tosqr, frsqr); if (move.IsSimpleCapture()) AddMan(tosqr, toman, capttid);
    }
    else
    {
      if (move.IsEnPassant())
      {
        // Undo an en passant move

        MoveMan(tosqr, frsqr);
        AddMan(move.CalcEPVictimSqr(), EvilMan(piecePawn), capttid);
      }
      else
      {
        // Undo a castling move

        if (move.IsCastling())
        {
          const castType cast = cvcolorflanktocast[GetGood()][cvmsctoflank[move.GetMsc()]];

          MoveMan(rookawaysqr[cast], rookhomesqr[cast]); MoveMan(tosqr, frsqr);
        }
        else
        {
          // Undo a promotion move

          const tidType promtid = sqrtotid[tosqr];

          DelMan(tosqr); AddMan(frsqr, frman, promtid);
          if (move.IsSimpleCapture()) {AddMan(tosqr, toman, capttid);};
        };
      };
    };
  };

  // Restore the last capture target ID

  capttid = savedtid;

  // Congratulations, you have now traveled backward in time
}

void Pos::Retract(
  const Move& move, const FEnv& fenv, const tidType savedtid, const PEnv& penv)
{
   *(PEnv *) this = penv; Retract(move, fenv, savedtid);
}

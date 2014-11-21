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
#include "Move.h"
#include "SAN.h"

void SAN::LoadFromMove(const Move& move)
{
  // SAN is Standard Algebraic Notation

  char *cptr = cv;

  if (move.IsNullOrVoid())
  {
    // Handle null move and void move encoding

    if (move.IsNull())
    {
      // Shouldn't see too much of this one being printed

      *cptr++ = 'n'; *cptr++ = 'u'; *cptr++ = 'l'; *cptr++ = 'l';
    }
    else
    {
      // Should probably never see this one being printed

      *cptr++ = 'v'; *cptr++ = 'o'; *cptr++ = 'i'; *cptr++ = 'd';
    };
  }
  else
  {
    // Handle a non-null, non-void move

    const char bigo = 'O', chck = '+', dash = '-', mate = '#', prom  = '=', take  = 'x';
    const sqrType frsqr = move.GetFrSqr(), tosqr = move.GetToSqr();
    const manType frman = move.GetFrMan();
    const mscType msc = move.GetMsc();
    const fileType frfile = MapSqrToFile(frsqr), tofile = MapSqrToFile(tosqr);
    const rankType frrank = MapSqrToRank(frsqr), torank = MapSqrToRank(tosqr);

    if (move.IsRegular())
    {
      // Encode a regular move

      if (IsManPawn(frman))
      {
        // Regular pawn move

        *cptr++ = CharOfFile(frfile);
        if (move.IsSimpleCapture()) {*cptr++ = take; *cptr++ = CharOfFile(tofile);};
        *cptr++ = CharOfRank(torank);
      }
      else
      {
        // Regular non-pawn move

        *cptr++ = CharOfPiece(cvmantopiece[frman]);
        if (move.IsDisamFile()) *cptr++ = CharOfFile(frfile);
        if (move.IsDisamRank()) *cptr++ = CharOfRank(frrank);
        if (move.IsSimpleCapture()) *cptr++ = take;
        *cptr++ = CharOfFile(tofile); *cptr++ = CharOfRank(torank);
      };
    }
    else
    {
      if (move.IsEnPassant())
      {
        // Encode an en passant move

        *cptr++ = CharOfFile(frfile); *cptr++ = take;
        *cptr++ = CharOfFile(tofile); *cptr++ = CharOfRank(torank);
      }
      else
      {
        if (move.IsCastling())
        {
          // Encode a castling move

          *cptr++ = bigo; *cptr++ = dash; *cptr++ = bigo;
          if (msc == mscCQS) {*cptr++ = dash; *cptr++ = bigo;};
        }
        else
        {
          // Encode a promotion move

          *cptr++ = CharOfFile(frfile);
          if (move.IsSimpleCapture()) {*cptr++ = take; *cptr++ = CharOfFile(tofile);};
          *cptr++ = CharOfRank(torank);
          *cptr++ = prom; *cptr++ = CharOfPiece(cvpromtopiece[cvmsctoprom[msc]]);
        };
      };
    };

    // Handle markings for checkmate and check

    if (move.IsMate()) *cptr++ = mate; else {if (move.IsCheck()) *cptr++ = chck;};
  };

  // ASCII NUL termination

  *cptr++ = '\0';
}

void SAN::Encode(char **bptrptr) const {EncodeStr(bptrptr, cv);}

void SAN::Print(void) const {PrintStr(cv);}

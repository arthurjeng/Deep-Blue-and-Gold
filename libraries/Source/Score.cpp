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

#include "Score.h"

void Score::PrintMateAnnouncement(const svType sv)
{
  if (IsSvMating(sv))
  {
    const ui16 distance = CalcMateDistance(sv);

    PrintFS(fsMsMateAnnounce0); PrintUi16(distance);
    if (distance == 1) PrintFS(fsMsMateAnnounce1); else PrintFS(fsMsMateAnnounce2);
  };
}

void Score::EncodeSv(char **bptrptr, const svType sv)
{
  // Handle any possible special case scores first

  if (IsSvBroken(sv)) EncodeFS(bptrptr, fsScBroken);
  else
  {
    if (IsSvPosInf(sv)) EncodeFS(bptrptr, fsScPosInf);
    else
    {
      if (IsSvNegInf(sv)) EncodeFS(bptrptr, fsScNegInf);
      else
      {
        // Now handle mate/lose scores

        if (IsSvMating(sv))
        {
          // Mating scores get a numeric suffix

          EncodeFS(bptrptr, fsScMateIn); EncodeUi16(bptrptr, CalcMateDistance(sv));
        }
        else
        {
          if (IsSvLosing(sv))
          {
            // Losing scores get a numeric suffix (except for a checkmated score)

            if (sv == svLoseIn0) EncodeFS(bptrptr, fsScCheckmated);
            else
            {
              EncodeFS(bptrptr, fsScLoseIn); EncodeUi16(bptrptr, CalcLoseDistance(sv));
            };
          }
          else
          {
            // Handle a regular score while avoiding floating point arithmetic

            const ui pcp = (sv >= 0) ? sv : -sv;
            const ui pawns = pcp / 100;
            const ui centipawns = pcp - (pawns * 100);

            if (sv != 0) EncodeChar(bptrptr, ((sv > 0) ? '+' : '-'));
            EncodeUi16(bptrptr, pawns); EncodeChar(bptrptr, '.');
            if (centipawns < 10) EncodeChar(bptrptr, '0');
            EncodeUi16(bptrptr, centipawns);
          };
        };
      };
    };
  };
}

void Score::PrintSv(const svType sv)
{
  // Print a formatted score value

  char cv[16], *cptr = cv;

  EncodeSv(&cptr, sv); *cptr++ = '\0'; PrintStr(cv);
}

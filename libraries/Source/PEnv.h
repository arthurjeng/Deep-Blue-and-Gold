// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_PEnv
#define Included_PEnv

// Position Environment (preservation) class
//
// The PEnv class is used to hold some environmental variables for a general
// position.  It is a base class for the Pos (general position) class.  An
// instance of the PEnv class is saved during each move execution and subsequently
// restored during the corresponding move retraction.

class PEnv
{
  public:
    void Reset(void);

    bool InCheck(void) const {return ischecked;}

    const TBV& FetchPinnedTBV(void) const {return pinnedtbv;}
    const TBV& FetchFrozenTBV(void) const {return frozentbv;}

  protected:
    TBV checkertbv, pinnedtbv, frozentbv;
    ui8 checkercount;
    bool ischecked;
};

#endif

// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_UnDo
#define Included_UnDo

// Move undo class

class UnDo
{
  public:
    void Reset(void);

    const FEnv& FetchFEnv(void) const {return fenv;}
    FEnv& RefFEnv(void) {return fenv;}

    const Move& FetchMove(void) const {return move;}
    Move& RefMove(void) {return move;}

    tidType GetCaptTid(void) const {return capttid;}
    void PutCaptTid(const tidType tid) {capttid = tid;}

  private:
    FEnv fenv;
    Move move;
    tidType capttid;
};

#endif

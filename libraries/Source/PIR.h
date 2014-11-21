// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_PIR
#define Included_PIR

// Ply Indexed Record class

class PIR
{
  public:
    void Reset(void);

    const MGS& FetchMGS(void) const {return mgs;}
    MGS& RefMGS(void) {return mgs;}

    const PEnv& FetchPEnv(void) const {return penv;}
    PEnv& RefPEnv(void) {return penv;}

  private:
    MGS mgs;
    PEnv penv;
};

#endif

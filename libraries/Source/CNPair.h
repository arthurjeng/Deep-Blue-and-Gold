// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_CNPair
#define Included_CNPair

// Forward class declaration(s)

class FEnv;

// Color/number pair class
//
// The CNPair class is used to help provide leading move numbers when
// priting move sequences.

class CNPair
{
  public:
    CNPair(const FEnv& fenv);
    ~CNPair(void) {}

    void Reset(void) {good = colorWhite; fmvn = 1;}
    colorType GetGood(void) const {return good;}
    void Increment(void);
    void Print(void) const;

  private:
    colorType good;
    fmvnType fmvn;
};

#endif

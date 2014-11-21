// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_SS
#define Included_SS

// Square scanner class

class SS
{
  public:
    SS(const sqrType sqr) {originsqr = sqr;}

    void SetDirection(const dirType dir) {advsqr = originsqr; advdir = dir;}
    sqrType Advance(void) {return advsqr = Board::FetchNextSquare(advsqr, advdir);}

  private:
    sqrType originsqr, advsqr;
    dirType advdir;
};

#endif

// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_SSKnight
#define Included_SSKnight

// Byte length of the knight run square table

#define knightspanLen (dirCLen + 1)
#define knightrunLen (sqrLen * knightspanLen)

// Square scanner class for a knight

class SSKnight
{
  public:
    SSKnight(const sqrType sqr) {byteptr = knightrunsqr + (sqr * knightspanLen);}

    sqrType Advance(void) {return (sqrType) ReadFlashSi8(byteptr++);}

  private:
    static const sqrType knightrunsqr[knightrunLen] PROGMEM;

    const sqrType *byteptr;
};

#endif

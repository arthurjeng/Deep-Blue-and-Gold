// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_SSKing
#define Included_SSKing

// Byte length of the king run square table

#define kingspanLen (dirSLen + 1)
#define kingrunLen (sqrLen * kingspanLen)

// Square scanner class for a king

class SSKing
{
  public:
    SSKing(const sqrType sqr) {byteptr = kingrunsqr + (sqr * kingspanLen);}

    sqrType Advance(void) {return (sqrType) ReadFlashSi8(byteptr++);}

  private:
    static const sqrType kingrunsqr[kingrunLen] PROGMEM;

    const sqrType *byteptr;
};

#endif

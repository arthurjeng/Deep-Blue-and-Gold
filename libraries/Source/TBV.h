// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_TBV
#define Included_TBV

#define tbbLen (tidLen / bytebitLen)

#define tbb0 ( bits        & bytemask)
#define tbb1 ((bits >>  8) & bytemask)
#define tbb2 ((bits >> 16) & bytemask)
#define tbb3 ((bits >> 24) & bytemask)

// Forward class declaration(s)

class Pos;

/// Target ID bit vector class

class TBV
{
  public:
    void Reset(void) {bits = 0;}

    bool IsEmpty(void)    const {return bits == 0;}
    bool IsNotEmpty(void) const {return bits != 0;}

    void ResetTid(const tidType tid) {bits &= ~BXL(tid);}
    void SetTid(const tidType tid) {bits |= BXL(tid);}
    bool TestTid(const tidType tid) const {return bits & BXL(tid);}

    ui Card(void) const;
    ui CardByColor(const colorType color) const;

    void OnlyColor(const colorType color) {bits &= (MXL(troopLen) << (color * troopLen));}

    tidType FirstTid(void) const;
    tidType NextTid(void);

    void PrintSqrSet(const Pos& pos) const;
    void PrintManSet(const Pos& pos) const;

    void PrintManSqrSet(const Pos& pos) const;

  private:
    static const ui8 bytebitcount[pageLen] PROGMEM;
    static const si8 bytefirstbit[pageLen] PROGMEM;

    ui32 bits;
};

inline ui TBV::Card(void) const
{
  // Return the count of the number of "on" bits in a target bit vector

  return
    ReadFlashUi8(bytebitcount + tbb0) +
    ReadFlashUi8(bytebitcount + tbb1) +
    ReadFlashUi8(bytebitcount + tbb2) +
    ReadFlashUi8(bytebitcount + tbb3);
}

inline ui TBV::CardByColor(const colorType color) const
{
  // Return the count of the number of "on" bits in a target bit vector for a color

  ui8 sum;

  if (IsColorWhite(color))
    sum = ReadFlashUi8(bytebitcount + tbb0) + ReadFlashUi8(bytebitcount + tbb1);
  else
    sum = ReadFlashUi8(bytebitcount + tbb2) + ReadFlashUi8(bytebitcount + tbb3);
  return sum;
}

inline tidType TBV::FirstTid(void) const
{
  // Return the first "on" bit in a target bit vector (if any)

  tidType tid;

  if (tbb0) tid = ReadFlashSi8(bytefirstbit + tbb0);
  else
  {
    if (tbb1) tid = ReadFlashSi8(bytefirstbit + tbb1) + 8;
    else
    {
      if (tbb2) tid = ReadFlashSi8(bytefirstbit + tbb2) + 16;
      else
      {
        if (tbb3) tid = ReadFlashSi8(bytefirstbit + tbb3) + 24;
        else
          tid = tidNil;
      };
    };
  };
  return tid;
}

inline tidType TBV::NextTid(void)
{
  // Return the first "on" bit in a target bit vector (if any), then clear it

  const tidType tid = FirstTid();

  if (IsTidNotNil(tid)) ResetTid(tid);
  return tid;
}

#endif

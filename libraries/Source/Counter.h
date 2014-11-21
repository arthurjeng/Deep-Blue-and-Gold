// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_Counter
#define Included_Counter

#define counterLen 8

class Counter
{
  public:
    void Reset(void);

    ui8 GetCodeByte(const ui8 index) const {return cb[index];}

    void Increment(void);
    void Increment(const ui value);
    void Increment(const Counter& counter);

    void Decrement(void);
    void Decrement(const ui value);
    void Decrement(const Counter& counter);

    void MultiplyByByteValue(const ui value);

    ui32 CalcUi32Value(void) const;
    double CalcDoubleValue(void) const;

    void Print(void) const;

  private:
    static si CompareCounter(const Counter& Counter0, const Counter& Counter1);

    void LoadByteValue(const ui8 value) {Reset(); cb[0] = value;}

    bool IsZero(void) const;

    void Complement(void);
    void Negate(void);

    ui8 cb[counterLen];
};

#endif

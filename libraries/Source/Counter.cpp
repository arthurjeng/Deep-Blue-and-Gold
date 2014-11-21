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

#include "Counter.h"

si Counter::CompareCounter(const Counter& Counter0, const Counter& Counter1)
{
  // Compare two counters, like strcmp()

  si compare = 0;
  ui index = 0;

  while ((compare == 0) && (index < counterLen))
  {
    const ui alt = (counterLen - 1) - index;
    const ui cb0 = Counter0.GetCodeByte(alt), cb1 = Counter1.GetCodeByte(alt);

    if (cb0 == cb1) index++; else compare = (cb0 < cb1) ? -1 : 1;
  };
  return compare;
}

void Counter::Reset(void)
{
  for (ui index = 0; index < counterLen; index++) cb[index] = 0;
}

bool Counter::IsZero(void) const
{
  bool iszero = true;

  for (ui index = 0; (iszero && (index < counterLen)); index++)
    if (cb[index] != 0) iszero = false;
  return iszero;
}

void Counter::Increment(void)
{
  bool carry = true;

  for (ui index = 0; (carry && (index < counterLen)); index++)
  {
    cb[index]++; if (cb[index] != 0) carry = false;
  };
}

void Counter::Increment(const ui value)
{
  ui overflow = value;

  for (ui index = 0; (overflow > 0) && (index < counterLen); index++)
  {
    const ui sum = (ui) cb[index] + overflow;

    cb[index] = sum & bytemask; overflow = (sum >> 8) & bytemask;
  };
}

void Counter::Increment(const Counter& counter)
{
  ui overflow = 0;

  for (ui index = 0; index < counterLen; index++)
  {
    const ui sum = (ui) cb[index] + (ui) counter.cb[index] + overflow;

    cb[index] = sum & bytemask; overflow = (sum >> 8) & bytemask;
  };
}

void Counter::Decrement(void)
{
  Counter delta;

  delta.LoadByteValue(1); Decrement(delta);
}

void Counter::Decrement(const ui value)
{
  Counter delta;

  delta.LoadByteValue(value); Decrement(delta);
}

void Counter::Decrement(const Counter& counter)
{
  Counter delta(counter);

  delta.Negate(); Increment(delta);
}

void Counter::Complement(void)
{
  for (ui index = 0; index < counterLen; index++) cb[index] = ~cb[index];
}

void Counter::Negate(void) {Complement(); Increment();}

void Counter::MultiplyByByteValue(const ui value)
{
  ui overflow = 0;

  for (ui index = 0; index < counterLen; index++)
  {
    const ui product = ((ui) cb[index] * value) + overflow;

    cb[index] = product & bytemask; overflow = (product >> 8) & bytemask;
  };
}

ui32 Counter::CalcUi32Value(void) const
{
  ui32 value = 0;

  for (ui index = 0; index < counterLen; index++)
  {
    value *= 256; value += cb[(counterLen - 1) - index];
  };
  return value;
}

double Counter::CalcDoubleValue(void) const
{
  double value = 0;

  for (ui index = 0; index < counterLen; index++)
  {
    value *= 256; value += cb[(counterLen - 1) - index];
  };
  return value;
}

void Counter::Print(void) const
{
#define magLen 20 // Covers an unsigned 64 bit value

  if (IsZero()) PrintDigit(0);
  else
  {
    // Initialize the powers of ten array

    Counter Tens[magLen];

    Tens[0].LoadByteValue(1);
    for (ui index = 1; index < magLen; index++)
    {
      Tens[index] = Tens[index - 1]; Tens[index].MultiplyByByteValue(10);
    };

    // Outer loop: scan powers of ten from big to small

    Counter Residue(*this);
    bool zerosupress = true;

    for (ui index = 0; index < magLen; index++)
    {
      // Inner loop: repeatedly subtract a power of ten to get a quotient digit

      const ui mag = (magLen - 1) - index;
      ui digit = 0;

      while (CompareCounter(Residue, Tens[mag]) >= 0)
      {
        Residue.Decrement(Tens[mag]); digit++;
      };

      // Print the quotient digit with leading zero supression

      if (digit) zerosupress = false;
      if (digit || !zerosupress) PrintDigit(digit);
    };
  };
}

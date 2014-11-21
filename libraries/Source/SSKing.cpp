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

#if (IsDevHost)
#include <cassert>
#endif

#include "SSKing.h"

// Lists of king run squares indexed by initial square

const sqrType SSKing::kingrunsqr[kingrunLen] PROGMEM =
{
  sqrB1,  sqrA2,  sqrB2,  sqrNil, sqrNil, sqrNil, sqrNil, sqrNil, sqrNil,
  sqrC1,  sqrB2,  sqrA1,  sqrC2,  sqrA2,  sqrNil, sqrNil, sqrNil, sqrNil,
  sqrD1,  sqrC2,  sqrB1,  sqrD2,  sqrB2,  sqrNil, sqrNil, sqrNil, sqrNil,
  sqrE1,  sqrD2,  sqrC1,  sqrE2,  sqrC2,  sqrNil, sqrNil, sqrNil, sqrNil,
  sqrF1,  sqrE2,  sqrD1,  sqrF2,  sqrD2,  sqrNil, sqrNil, sqrNil, sqrNil,
  sqrG1,  sqrF2,  sqrE1,  sqrG2,  sqrE2,  sqrNil, sqrNil, sqrNil, sqrNil,
  sqrH1,  sqrG2,  sqrF1,  sqrH2,  sqrF2,  sqrNil, sqrNil, sqrNil, sqrNil,
  sqrH2,  sqrG1,  sqrG2,  sqrNil, sqrNil, sqrNil, sqrNil, sqrNil, sqrNil,

  sqrB2,  sqrA3,  sqrA1,  sqrB3,  sqrB1,  sqrNil, sqrNil, sqrNil, sqrNil,
  sqrC2,  sqrB3,  sqrA2,  sqrB1,  sqrC3,  sqrA3,  sqrA1,  sqrC1,  sqrNil,
  sqrD2,  sqrC3,  sqrB2,  sqrC1,  sqrD3,  sqrB3,  sqrB1,  sqrD1,  sqrNil,
  sqrE2,  sqrD3,  sqrC2,  sqrD1,  sqrE3,  sqrC3,  sqrC1,  sqrE1,  sqrNil,
  sqrF2,  sqrE3,  sqrD2,  sqrE1,  sqrF3,  sqrD3,  sqrD1,  sqrF1,  sqrNil,
  sqrG2,  sqrF3,  sqrE2,  sqrF1,  sqrG3,  sqrE3,  sqrE1,  sqrG1,  sqrNil,
  sqrH2,  sqrG3,  sqrF2,  sqrG1,  sqrH3,  sqrF3,  sqrF1,  sqrH1,  sqrNil,
  sqrH3,  sqrG2,  sqrH1,  sqrG3,  sqrG1,  sqrNil, sqrNil, sqrNil, sqrNil,

  sqrB3,  sqrA4,  sqrA2,  sqrB4,  sqrB2,  sqrNil, sqrNil, sqrNil, sqrNil,
  sqrC3,  sqrB4,  sqrA3,  sqrB2,  sqrC4,  sqrA4,  sqrA2,  sqrC2,  sqrNil,
  sqrD3,  sqrC4,  sqrB3,  sqrC2,  sqrD4,  sqrB4,  sqrB2,  sqrD2,  sqrNil,
  sqrE3,  sqrD4,  sqrC3,  sqrD2,  sqrE4,  sqrC4,  sqrC2,  sqrE2,  sqrNil,
  sqrF3,  sqrE4,  sqrD3,  sqrE2,  sqrF4,  sqrD4,  sqrD2,  sqrF2,  sqrNil,
  sqrG3,  sqrF4,  sqrE3,  sqrF2,  sqrG4,  sqrE4,  sqrE2,  sqrG2,  sqrNil,
  sqrH3,  sqrG4,  sqrF3,  sqrG2,  sqrH4,  sqrF4,  sqrF2,  sqrH2,  sqrNil,
  sqrH4,  sqrG3,  sqrH2,  sqrG4,  sqrG2,  sqrNil, sqrNil, sqrNil, sqrNil,

  sqrB4,  sqrA5,  sqrA3,  sqrB5,  sqrB3,  sqrNil, sqrNil, sqrNil, sqrNil,
  sqrC4,  sqrB5,  sqrA4,  sqrB3,  sqrC5,  sqrA5,  sqrA3,  sqrC3,  sqrNil,
  sqrD4,  sqrC5,  sqrB4,  sqrC3,  sqrD5,  sqrB5,  sqrB3,  sqrD3,  sqrNil,
  sqrE4,  sqrD5,  sqrC4,  sqrD3,  sqrE5,  sqrC5,  sqrC3,  sqrE3,  sqrNil,
  sqrF4,  sqrE5,  sqrD4,  sqrE3,  sqrF5,  sqrD5,  sqrD3,  sqrF3,  sqrNil,
  sqrG4,  sqrF5,  sqrE4,  sqrF3,  sqrG5,  sqrE5,  sqrE3,  sqrG3,  sqrNil,
  sqrH4,  sqrG5,  sqrF4,  sqrG3,  sqrH5,  sqrF5,  sqrF3,  sqrH3,  sqrNil,
  sqrH5,  sqrG4,  sqrH3,  sqrG5,  sqrG3,  sqrNil, sqrNil, sqrNil, sqrNil,

  sqrB5,  sqrA6,  sqrA4,  sqrB6,  sqrB4,  sqrNil, sqrNil, sqrNil, sqrNil,
  sqrC5,  sqrB6,  sqrA5,  sqrB4,  sqrC6,  sqrA6,  sqrA4,  sqrC4,  sqrNil,
  sqrD5,  sqrC6,  sqrB5,  sqrC4,  sqrD6,  sqrB6,  sqrB4,  sqrD4,  sqrNil,
  sqrE5,  sqrD6,  sqrC5,  sqrD4,  sqrE6,  sqrC6,  sqrC4,  sqrE4,  sqrNil,
  sqrF5,  sqrE6,  sqrD5,  sqrE4,  sqrF6,  sqrD6,  sqrD4,  sqrF4,  sqrNil,
  sqrG5,  sqrF6,  sqrE5,  sqrF4,  sqrG6,  sqrE6,  sqrE4,  sqrG4,  sqrNil,
  sqrH5,  sqrG6,  sqrF5,  sqrG4,  sqrH6,  sqrF6,  sqrF4,  sqrH4,  sqrNil,
  sqrH6,  sqrG5,  sqrH4,  sqrG6,  sqrG4,  sqrNil, sqrNil, sqrNil, sqrNil,

  sqrB6,  sqrA7,  sqrA5,  sqrB7,  sqrB5,  sqrNil, sqrNil, sqrNil, sqrNil,
  sqrC6,  sqrB7,  sqrA6,  sqrB5,  sqrC7,  sqrA7,  sqrA5,  sqrC5,  sqrNil,
  sqrD6,  sqrC7,  sqrB6,  sqrC5,  sqrD7,  sqrB7,  sqrB5,  sqrD5,  sqrNil,
  sqrE6,  sqrD7,  sqrC6,  sqrD5,  sqrE7,  sqrC7,  sqrC5,  sqrE5,  sqrNil,
  sqrF6,  sqrE7,  sqrD6,  sqrE5,  sqrF7,  sqrD7,  sqrD5,  sqrF5,  sqrNil,
  sqrG6,  sqrF7,  sqrE6,  sqrF5,  sqrG7,  sqrE7,  sqrE5,  sqrG5,  sqrNil,
  sqrH6,  sqrG7,  sqrF6,  sqrG5,  sqrH7,  sqrF7,  sqrF5,  sqrH5,  sqrNil,
  sqrH7,  sqrG6,  sqrH5,  sqrG7,  sqrG5,  sqrNil, sqrNil, sqrNil, sqrNil,

  sqrB7,  sqrA8,  sqrA6,  sqrB8,  sqrB6,  sqrNil, sqrNil, sqrNil, sqrNil,
  sqrC7,  sqrB8,  sqrA7,  sqrB6,  sqrC8,  sqrA8,  sqrA6,  sqrC6,  sqrNil,
  sqrD7,  sqrC8,  sqrB7,  sqrC6,  sqrD8,  sqrB8,  sqrB6,  sqrD6,  sqrNil,
  sqrE7,  sqrD8,  sqrC7,  sqrD6,  sqrE8,  sqrC8,  sqrC6,  sqrE6,  sqrNil,
  sqrF7,  sqrE8,  sqrD7,  sqrE6,  sqrF8,  sqrD8,  sqrD6,  sqrF6,  sqrNil,
  sqrG7,  sqrF8,  sqrE7,  sqrF6,  sqrG8,  sqrE8,  sqrE6,  sqrG6,  sqrNil,
  sqrH7,  sqrG8,  sqrF7,  sqrG6,  sqrH8,  sqrF8,  sqrF6,  sqrH6,  sqrNil,
  sqrH8,  sqrG7,  sqrH6,  sqrG8,  sqrG6,  sqrNil, sqrNil, sqrNil, sqrNil,

  sqrB8,  sqrA7,  sqrB7,  sqrNil, sqrNil, sqrNil, sqrNil, sqrNil, sqrNil,
  sqrC8,  sqrA8,  sqrB7,  sqrA7,  sqrC7,  sqrNil, sqrNil, sqrNil, sqrNil,
  sqrD8,  sqrB8,  sqrC7,  sqrB7,  sqrD7,  sqrNil, sqrNil, sqrNil, sqrNil,
  sqrE8,  sqrC8,  sqrD7,  sqrC7,  sqrE7,  sqrNil, sqrNil, sqrNil, sqrNil,
  sqrF8,  sqrD8,  sqrE7,  sqrD7,  sqrF7,  sqrNil, sqrNil, sqrNil, sqrNil,
  sqrG8,  sqrE8,  sqrF7,  sqrE7,  sqrG7,  sqrNil, sqrNil, sqrNil, sqrNil,
  sqrH8,  sqrF8,  sqrG7,  sqrF7,  sqrH7,  sqrNil, sqrNil, sqrNil, sqrNil,
  sqrG8,  sqrH7,  sqrG7,  sqrNil, sqrNil, sqrNil, sqrNil, sqrNil, sqrNil
};

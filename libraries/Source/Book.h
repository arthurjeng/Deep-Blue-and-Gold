// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_Book
#define Included_Book

// Book index type

typedef si32 biType;

// Maximum library book entry count

#define MaxBookEntryLen (BX(12) - 1)

// Opening library book entry count; must be manually adjusted

#define BookEntryLen 4095

// Opening library book byte length

#define BookByteLen (BookEntryLen * sizeof(BookMove))

// Forward class declaration(s)

class BookMove;
class FPos;
class Hash;
class ML;

// Opening library book class; only statics, no instances

class Book
{
  public:
    static bool BookMark(const ML& ml, const FPos& fpos);
    static ui CalcPopularitySum(const ML& ml);
    static miType PickBookMove(const ML& ml, const FPos& fpos);

  private:
    static void FetchBookMove(const biType index, BookMove& bookmove);
    static si HashBookMoveCompare(const Hash& hash, const BookMove& bookmove);
    static biType FindBase(const Hash& hash);

    static const ui8 TheBookBytes[BookByteLen] PROGMEM;
};

#endif

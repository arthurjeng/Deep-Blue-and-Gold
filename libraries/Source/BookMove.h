// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_BookMove
#define Included_BookMove

// Copied hash code bytes for a book move entry

#define bmhashcbLen 5

// Total code bytes

#define bmcbLen (bmhashcbLen + tmcbLen + 1)

// Opening library move class

class BookMove
{
  public:
    ui8 GetCodeByte(const ui8 index) const {return codebyte[index];}
    void PutCodeByte(const ui8 index, const ui8 cb) {codebyte[index] = cb;}

  private:
    ui8 codebyte[bmcbLen];
};

#endif

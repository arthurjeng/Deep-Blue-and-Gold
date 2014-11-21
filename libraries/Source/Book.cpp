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

#include "Board.h"
#include "FEnv.h"
#include "FPos.h"
#include "Move.h"
#include "TinyMove.h"
#include "ML.h"
#include "Hash.h"
#include "BookMove.h"
#include "Book.h"
#include "TBV.h"
#include "PEnv.h"
#include "Pos.h"

void Book::FetchBookMove(const biType index, BookMove& bookmove)
{
  // Read a single book move from the book stored in flash memory

  for (ui cbindex = 0; cbindex < sizeof(BookMove); cbindex++)
    bookmove.PutCodeByte(
      cbindex,
      ReadFlashUi8(TheBookBytes + (index * sizeof(BookMove)) + cbindex));
}

si Book::HashBookMoveCompare(const Hash& hash, const BookMove& bookmove)
{
  // Return a comparison between a hash and a book move

  si compare = 0;
  ui index = 0;

  // Scan the leading code bytes

  while ((index < bmhashcbLen) && (compare == 0))
  {
    const ui8 hcb = hash.GetCodeByte(index), bmcb = bookmove.GetCodeByte(index);

    // Either a match/advance or a comparison assignment

    if (hcb == bmcb) index++; else compare = ((hcb < bmcb) ? -1 : 1);
  };
  return compare;
}

biType Book::FindBase(const Hash& hash)
{
  // Locate the first (if any) matching book move in the book

  biType base = -1, lo = 0, hi = BookEntryLen - 1;

  // Run a binary search trying to locate a book move that matches the given hash

  while ((base < 0) && (lo <= hi))
  {
    const biType index = (lo + hi) / 2;
    BookMove bookmove;

    FetchBookMove(index, bookmove);

    const si compare = HashBookMoveCompare(hash, bookmove);

    // Either a hash/move match or a bounds adjustment

    if (compare == 0) base = index;
    else
    {
      if (compare < 0) hi = index - 1; else lo = index + 1;
    };
  };

  // Was a matching book move located?

  if (base >= 0)
  {
    bool match = true;

    // Scan backwards in the book to find the first matching entry

    while (match && (base > 0))
    {
      const biType basem1 = base - 1;
      BookMove bookmovem1;

      FetchBookMove(basem1, bookmovem1);
      if (HashBookMoveCompare(hash, bookmovem1) == 0) base = basem1; else match = false;
    };
  };

  return base;
}

bool Book::BookMark(const ML& ml, const FPos& fpos)
{
  // Given a move list and its FEN position, mark the book moves/popularities

  Hash hash;

  // Generate the hash depending on color-to-move

  fpos.CalcPosHashWTM(hash);

  // Locate the base index of the matching book move sequence in the book

  const biType base = FindBase(hash);
  const bool alo = (base >= 0);

  // Is there at least one matching book move in the book?

  if (alo)
  {
    // At least one matching book move; scan book sequence starting at its base

    bool match = true;
    biType bookindex = base;

    while (match && (bookindex < BookEntryLen))
    {
      BookMove bookmove;

      // Read the current book move sequence element

      FetchBookMove(bookindex, bookmove);

      // Has the sequence ended?

      if (HashBookMoveCompare(hash, bookmove) != 0) match = false;
      else
      {
        // Still following the matching book sequence

        TinyMove tinymove;
        Move move;

        // Decode the tiny move from the book move, then convert it to a normal move

        tinymove.Load(bookmove.GetCodeByte(5), bookmove.GetCodeByte(6));
        if (fpos.IsBTM()) tinymove.Flip();
        tinymove.ConvertToMove(move, fpos);

        // Try to find the normal move in the given move list

        const miType moveindex = ml.Locate(move);

        if (moveindex >= 0)
        {
          // Skip the move if it has a certain score

          if (ml.FetchMove(moveindex).IsNotCertain())
          {
            // Mark the matching move in the move list and record its popularity

            ml.RefMove(moveindex).SetFlagM(mfmBook);
            ml.RefMove(moveindex).PutSv(bookmove.GetCodeByte(7));
          };
        };

        // Next book move entry in the book

        bookindex++;
      };
    }
  };

  // Return at-least-one book move located status

  return alo;
}

ui Book::CalcPopularitySum(const ML& ml)
{
  // Return the sum of the popularities of all book moves in the given move list

  ui sum = 0;

  for (miType index = 0; index < ml.GetCount(); index++)
    if (ml.FetchMove(index).IsBook()) sum += ml.FetchMove(index).GetSv();
  return sum;
}

miType Book::PickBookMove(const ML& ml, const FPos& fpos)
{
  // Mark the move list and return a move index of a book move (if any)

  miType moveindex = -1;

  // Apply book markings

  if (BookMark(ml, fpos))
  {
    // There was at least one opening library move found

    const ui popsum = CalcPopularitySum(ml);

    // Was there a positive popularity sum?

    if (popsum > 0)
    {
      // Randomly pick a book move based on relative popularity

      const miType picksum = RandomPick(popsum);
      miType index = 0;
      si sum = 0;

      while ((moveindex < 0) && (sum <= picksum) && (index < ml.GetCount()))
      {
        if (ml.FetchMove(index).IsBook())
        {
          const ui pop = ml.FetchMove(index).GetSv();

          if (pop > 0) {sum += pop; if (sum > picksum) moveindex = index;};
        };
        index++;
      };
    };
  };

  return moveindex;
}

// The opening library data from an external file, produced by the "bb" (build book) command

const ui8 Book::TheBookBytes[BookByteLen] PROGMEM =
{
#include "OpeningBookCodes"
};

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
#include <iostream>
#endif

#if (IsDevHost)

#include "Board.h"
#include "Move.h"
#include "TinyMove.h"
#include "Hash.h"
#include "BookMove.h"
#include "BBMoveNode.h"

static void PrintPrefixedHexByte(std::ostream *ofstpr, const ui value)
{
  // Print a single prefixed hexadecimal data byte to the output code file

  *ofstpr << "0x" << CharOfDigit(value / 16) << CharOfDigit(value % 16) << ", ";
}

BBMoveNode::BBMoveNode(const TinyMove& tinymovevalue)
{
  lptr = rptr = 0; tinymove = tinymovevalue; used = false; count = 1; popularity = 0;
}

BBMoveNode::~BBMoveNode(void) {delete lptr; delete rptr;}

BBMoveNode *BBMoveNode::Locate(const TinyMove& key) const
{
  BBMoveNode *ptr;
  const si compare = TinyMove::CompareTinyMove(key, tinymove);

  if (compare == 0) ptr = (BBMoveNode *) this;
  else
  {
    if (compare < 0)
    {
      if (lptr) ptr = lptr->Locate(key); else ptr = 0;
    }
    else
    {
      if (rptr) ptr = rptr->Locate(key); else ptr = 0;
    };
  };
  return ptr;
}

void BBMoveNode::Insert(const BBMoveNode *ptr)
{
  const si compare = TinyMove::CompareTinyMove(ptr->tinymove, tinymove);

  if (compare == 0) DieStr("BBMoveNode::Insert\n");
  else
  {
    if (compare < 0)
    {
      if (lptr) lptr->Insert(ptr); else lptr = (BBMoveNode *) ptr;
    }
    else
    {
      if (rptr) rptr->Insert(ptr); else rptr = (BBMoveNode *) ptr;
    };
  };
}

BBMoveNode *BBMoveNode::FindBestUnused(void) const
{
  ui32 bcount = 0;
  BBMoveNode *bptr = 0;

  if (lptr)
  {
    BBMoveNode *blptr = lptr->FindBestUnused();

    if (blptr && (blptr->count > bcount)) {bcount = blptr->count; bptr = blptr;};
  };

  if (!used && (count > bcount)) {bcount = count; bptr = (BBMoveNode *) this;};

  if (rptr)
  {
    BBMoveNode *brptr = rptr->FindBestUnused();

    if (brptr && (brptr->count > bcount)) {bcount = brptr->count; bptr = brptr;};
  };

  return bptr;
}

ui32 BBMoveNode::CalcUsedCountSum(void) const
{
  ui32 sum = 0;

  if (lptr) sum += lptr->CalcUsedCountSum();
  if (used) sum += count;
  if (rptr) sum += rptr->CalcUsedCountSum();
  return sum;
}

void BBMoveNode::CalcPopularity(const ui32 usedcountsum)
{
  if (lptr) lptr->CalcPopularity(usedcountsum);
  if (used) popularity = (ui8) (((count * 255.0) / usedcountsum) + 0.5);
  if (rptr) rptr->CalcPopularity(usedcountsum);
}

void BBMoveNode::PrintCode(std::ostream *ofsptr, const Hash& hash) const
{
  if (lptr) lptr->PrintCode(ofsptr, hash);

  if (used)
  {
    // Print a single code line to the output code file

    *ofsptr << "  ";

    // Print the leading bytes of the hash

    for (ui index = 0; index < bmhashcbLen; index++)
      PrintPrefixedHexByte(ofsptr, hash.GetCodeByte(index));

    // Print the tiny move bytes

    for (ui index = 0; index < tmcbLen; index++)
      PrintPrefixedHexByte(ofsptr, tinymove.GetCodeByte(index));

    // Print the relative popularity and a newline

    PrintPrefixedHexByte(ofsptr, popularity); *ofsptr << '\n';
  };

  if (rptr) rptr->PrintCode(ofsptr, hash);
}

#endif

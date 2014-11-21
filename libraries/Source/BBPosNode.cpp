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
#include <iosfwd>
#endif

#if (IsDevHost)

#include "Board.h"
#include "Move.h"
#include "TinyMove.h"
#include "Hash.h"
#include "BBMoveNode.h"
#include "BBPosNode.h"

BBPosNode::BBPosNode(const Hash& hashvalue)
{
  lptr = rptr = 0; mnptr = 0; hash = hashvalue; usedcountsum = 0;
}

BBPosNode::~BBPosNode(void) {delete lptr; delete rptr; delete mnptr;}

BBPosNode *BBPosNode::Locate(const Hash& key) const
{
  BBPosNode *ptr;
  const si compare = Hash::CompareHash(key, hash);

  if (compare == 0) ptr = (BBPosNode *) this;
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

void BBPosNode::Insert(const BBPosNode *ptr)
{
  const si compare = Hash::CompareHash(ptr->hash, hash);

  if (compare == 0) DieStr("BBPosNode::Insert\n");
  else
  {
    if (compare < 0)
    {
      if (lptr) lptr->Insert(ptr); else lptr = (BBPosNode *) ptr;
    }
    else
    {
      if (rptr) rptr->Insert(ptr); else rptr = (BBPosNode *) ptr;
    };
  };
}

void BBPosNode::AddTinyMove(const TinyMove& tinymove)
{
  BBMoveNode *ptr = new BBMoveNode(tinymove);

  if (mnptr == 0) mnptr = ptr; else mnptr->Insert(ptr);
}

BBMoveNode *BBPosNode::FindMoveNode(const TinyMove& tinymove) const
{
  BBMoveNode *ptr;

  if (mnptr == 0) ptr = 0; else ptr = mnptr->Locate(tinymove);
  return ptr;
}

BBMoveNode *BBPosNode::FindBestUnusedMoveNodeLocal(void) const
{
  BBMoveNode *bptr;

  if (mnptr == 0) bptr = 0; else bptr = mnptr->FindBestUnused();
  return bptr;
}

BBMoveNode *BBPosNode::FindBestUnusedMoveNodeGlobal(void) const
{
  ui32 bcount = 0;
  BBMoveNode *bptr = 0;

  if (lptr)
  {
    BBMoveNode *blptr = lptr->FindBestUnusedMoveNodeGlobal();

    if (blptr && (blptr->GetCount() > bcount)) {bcount = blptr->GetCount(); bptr = blptr;};
  };

  BBMoveNode *mptr = FindBestUnusedMoveNodeLocal();

  if (mptr && (mptr->GetCount() > bcount)) {bcount = mptr->GetCount(); bptr = mptr;};

  if (rptr)
  {
    BBMoveNode *brptr = rptr->FindBestUnusedMoveNodeGlobal();

    if (brptr && (brptr->GetCount() > bcount)) {bcount = brptr->GetCount(); bptr = brptr;};
  };

  return bptr;
}

void BBPosNode::AssignUsedCountSum(void)
{
  if (lptr) lptr->AssignUsedCountSum();
  if (mnptr) usedcountsum = mnptr->CalcUsedCountSum();
  if (rptr) rptr->AssignUsedCountSum();
}

void BBPosNode::AssignPopularity(void) const
{
  if (lptr) lptr->AssignPopularity();
  if (mnptr && (usedcountsum > 0)) mnptr->CalcPopularity(usedcountsum);
  if (rptr) rptr->AssignPopularity();
}

void BBPosNode::PrintCode(std::ostream *ofsptr) const
{
  if (lptr) lptr->PrintCode(ofsptr);
  if (mnptr) mnptr->PrintCode(ofsptr, hash);
  if (rptr) rptr->PrintCode(ofsptr);
}

#endif

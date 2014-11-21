// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_BBPosNode
#define Included_BBPosNode

#if (IsDevHost)

// Forward class declaration(s)

class TinyMove;
class BBMoveNode;

// Book builder position node class

class BBPosNode
{
  public:
    BBPosNode(const Hash& hashvalue);
    ~BBPosNode(void);

    BBPosNode *Locate(const Hash& key) const;
    void Insert(const BBPosNode *ptr);

    void AddTinyMove(const TinyMove& tinymove);
    BBMoveNode *FindMoveNode(const TinyMove& tinymove) const;

    BBMoveNode *FindBestUnusedMoveNodeGlobal(void) const;

    void AssignUsedCountSum(void);
    void AssignPopularity(void) const;

    void PrintCode(std::ostream *ofsptr) const;

  private:
    BBMoveNode *FindBestUnusedMoveNodeLocal(void) const;

    BBPosNode *lptr, *rptr;
    BBMoveNode *mnptr;
    Hash hash;
    ui32 usedcountsum;
};

#endif

#endif

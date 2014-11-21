// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_BBMoveNode
#define Included_BBMoveNode

#if (IsDevHost)

// Book builder move node class

class BBMoveNode
{
  public:
    BBMoveNode(const TinyMove& tinymovevalue);
    ~BBMoveNode(void);

    ui32 GetCount(void) const {return count;}
    void IncCount(void) {count++;}

    bool IsUsed(void) const {return used;}
    void SetUsed(void) {used = true;}

    const TinyMove& FetchTinyMove(void) const {return tinymove;}

    BBMoveNode *Locate(const TinyMove& key) const;
    void Insert(const BBMoveNode *ptr);

    BBMoveNode *FindBestUnused(void) const;

    ui32 CalcUsedCountSum(void) const;
    void CalcPopularity(const ui32 usedcountsum);

    void PrintCode(std::ostream *ofsptr, const Hash& hash) const;

  private:
    BBMoveNode *lptr, *rptr;
    TinyMove tinymove;
    bool used;
    ui32 count;
    ui8 popularity;
};

#endif

#endif

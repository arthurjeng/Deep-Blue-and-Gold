// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_History
#define Included_History

// History stack element length

#define HistoryStackLen ((Fifty * colorRLen) + MaxPlyLen)

// History class
//
// The History class is used to represent a stack of position hashes.  A single
// instance of this class is included as a member variable in the State class.

class History
{
  public:
    History(void) {Reset();}
    ~History(void) {}

    void Reset(void);

    void Push(const Hash& hash)
    {
      if (count == HistoryStackLen)
      {
        for (miType index = 1; index < HistoryStackLen; index++)
          hashstack[index - 1] = hashstack[index];
        count--;
      };
      hashstack[count++] = hash;
    }

    void PopDel(void) {count--;}

    bool ALOneRep(const Hash& hash, const miType backcount) const;
    bool ALTwoRep(const Hash& hash, const miType backcount) const;

  private:
    Hash hashstack[HistoryStackLen];
    miType count;
};

#endif

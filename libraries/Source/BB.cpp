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
#include <cstring>
#include <fstream>
#include <iostream>
#endif

#if (IsDevHost)

#include "Counter.h"
#include "Board.h"
#include "FEnv.h"
#include "FPos.h"
#include "Move.h"
#include "UnDo.h"
#include "UnDoStack.h"
#include "TinyMove.h"
#include "ML.h"
#include "Hash.h"
#include "BookMove.h"
#include "Book.h"
#include "History.h"
#include "TBV.h"
#include "MGS.h"
#include "PEnv.h"
#include "Pos.h"
#include "PVTable.h"
#include "PIR.h"
#include "Search.h"
#include "State.h"
#include "BBMoveNode.h"
#include "BBPosNode.h"
#include "BB.h"

#define pgntknLen 8 // Game token length limit

static char RGC(std::ifstream *ifsptr, bool& eof, bool& okay)
{
  // Read a game file character; adjust eof and okay flags as needed

  char ch;

  ifsptr->get(ch);
  if (ifsptr->bad()) {eof = true; okay = false;}
  else
  {
    if (ifsptr->eof()) eof = true;
  };
  return ch;
}

static bool IsTokenGameResult(const char *token)
{
  // Check for a PGN game result indicator token

  return
    (strcmp(token, "*"      ) == 0) || (strcmp(token, "0-1"    ) == 0) ||
    (strcmp(token, "1-0"    ) == 0) || (strcmp(token, "1/2-1/2") == 0);
}

static bool IsTokenMoveNumber(const char *token)
{
  // Check for a PGN move number (dd* or dd*.)

  const char *cptr = token;
  bool ismovenumber = ((strlen(cptr) > 0) && isdigit(*cptr));
  char ch;

  while ((ch = *cptr++) && isdigit(ch));
  if (ch)
  {
    if (ch != '.') ismovenumber = false;
    else
    {
      ch = *cptr++; if (ch) ismovenumber = false;
    };
  };
  return ismovenumber;
}

bool BB::MakeBook(State *stateptr, const char *gamefn, const char *codefn)
{
  // Read a PGN game file to produce an opening library book code file

  bool okay = true;
  std::ifstream *ifsptr = 0;
  std::ofstream *ofsptr = 0;
  BBPosNode *rootptr = 0;

  // Ensure the initiol state is the initial array

  stateptr->NewGame();

  // Open the input game file

  if (okay)
  {
    ifsptr = new std::ifstream(gamefn);
    if (ifsptr->fail()) {std::cerr << "Can't open game input file\n"; okay = false;};
  };

  // Open the output code file

  if (okay)
  {
    ofsptr = new std::ofstream(codefn);
    if (ofsptr->fail()) {std::cerr << "Can't open code output file\n"; okay = false;};
  };

  // Pass one: Read the PGN game file

  if (okay)
  {
    // Initialize the game file scan

    bool eof = false;
    ui32 gamecount = 0, movereadcount = 0, moveusedcount = 0, posnodecount = 0;
    plyType ply = 0;
    char ch = RGC(ifsptr, eof, okay);

    // Game scan loop; maximum one token per iteration

    while (!eof)
    {
      // Skip whitespace

      while (!eof && isspace(ch)) ch = RGC(ifsptr, eof, okay);

      // Continue if not end of file after whitespace

      if (!eof)
      {
        // Handle PGN tag pair line skip; assume the tag pair has its own text line

        if (ch == '[')
        {
          do ch = RGC(ifsptr, eof, okay); while (!eof && (ch != '\n'));
          if (!eof) ch = RGC(ifsptr, eof, okay);
        }
        else
        {
          // Build a token

          char pgntkn[pgntknLen];
          ui tcc = 0;

          do
          {
            if (tcc < (pgntknLen - 1)) pgntkn[tcc++] = ch;
            ch = RGC(ifsptr, eof, okay);
          }
          while (!eof && !isspace(ch));

          // ASCII NUL termination

          pgntkn[tcc] = '\0';

          // Token classification

          if (tcc > 0)
          {
            // End of game?

            if (IsTokenGameResult(pgntkn))
            {
              // Handle the end of this game and the start of the next game

              gamecount++; ply = 0; stateptr->NewGame();
            }
            else
            {
              // Skip over a move number

              if (!IsTokenMoveNumber(pgntkn))
              {
                // At this point the token should be a SAN move; try to match it

                Move move;

                stateptr->MatchMove(pgntkn, move);
                if (move.IsVoid())
                {
                  // Invalid move string encountered

                  std::cerr << "Unrecognized move: " << pgntkn << '\n';
                  eof = true; okay = false;
                }
                else
                {
                  // The token is a valid move

                  movereadcount++; ply++;

                  // Continue if the game is not too long

                  if (ply <= 32)
                  {
                    // Update the used move counter

                    moveusedcount++;

                    // Construct the WTM pasition hash and the WTM tiny move

                    TinyMove tinymove;
                    Hash hash;

                    stateptr->FetchSearchFPos().CalcPosHashWTM(hash);
                    tinymove.ConvertFromMove(move);
                    if (stateptr->FetchSearchFEnv().IsBTM()) tinymove.Flip();

                    // Is this the very first node?

                    if (rootptr == 0)
                    {
                      // Only once per scan, initialize the root node

                      rootptr = new BBPosNode(hash);
                      rootptr->AddTinyMove(tinymove); posnodecount++;
                    }
                    else
                    {
                      // Attempt to locate the position node

                      BBPosNode *posptr = rootptr->Locate(hash);

                      // Has this position been seen before?

                      if (posptr)
                      {
                        // Position seen already; has the move been seen before?

                        BBMoveNode *moveptr = posptr->FindMoveNode(tinymove);

                        if (moveptr) moveptr->IncCount(); else posptr->AddTinyMove(tinymove);
                      }
                      else
                      {
                        // It's the first time for this position

                        posptr = new BBPosNode(hash); posptr->AddTinyMove(tinymove);
                        rootptr->Insert(posptr); posnodecount++;
                      };
                    };
                  };

                  // Update the game state by playing the scanned move

                  stateptr->Play(move);
                };
              };
            };
          };
        };
      };
    };

    // Print the summary for this pass

    std::cout << "PGN game scan summary\n";
    std::cout << "  Games read: " << gamecount << '\n';
    std::cout << "  Moves read: " << movereadcount << '\n';
    std::cout << "  Moves used: " << moveusedcount << '\n';
    std::cout << "  Unique usable positions: " << posnodecount << '\n';
  };

  // Pass two: Mark the best N usable move nodes as being used

  if (okay && rootptr)
  {
    ui32 usedcount = 0;
    BBMoveNode *ptr;

    // Repeatedly locate/mark the most common unused move node

    do
    {
      ptr = rootptr->FindBestUnusedMoveNodeGlobal();
      if (ptr) {ptr->SetUsed(); usedcount++;};
    }
    while ((usedcount < MaxBookEntryLen) && ptr);

    // Print the summary for this pass

    std::cout << "Most common position/move combination marking summary\n";
    std::cout << "  Combinations in use: " << usedcount << '\n';
  };

  // Pass three: Calculate and assign used count sums for each position

  if (okay && rootptr)
  {
    rootptr->AssignUsedCountSum();
    std::cout << "Position used count sum assignments complete\n";
  };

  // Pass four: Assign popularity values to all used move nodes

  if (okay && rootptr)
  {
    rootptr->AssignPopularity();
    std::cout << "Popularity value assignments complete\n";
  };

  // Pass five: Print the usable move nodes to the code file

  if (okay && rootptr)
  {
    rootptr->PrintCode(ofsptr);
    std::cout << "Code file output complete\n";
  };

  // Return the state to the initial array

  stateptr->NewGame();

  // Clean up and return

  delete rootptr; delete ifsptr; delete ofsptr;
  return okay;
}

#endif

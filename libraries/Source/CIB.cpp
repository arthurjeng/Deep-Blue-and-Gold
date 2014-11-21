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

#include "CIB.h"

void CIB::ResetBuffer(void)
{
  for (ui index = 0; index < cibLen; index++ ) cv[index] = '\0';
}

void CIB::ResetTokens(void)
{
  tokencount = 0; for (ui index = 0; index < tokensLen; index++ ) tokens[index] = 0;
}

void CIB::Reset(void) {iseof = false; ResetBuffer(); ResetTokens();}

void CIB::Load(void)
{
  // Load the input buffer

  if (!iseof)
  {
    ResetBuffer(); ResetTokens(); ReadLine(cv, cibLen, iseof);
    if (iseof) ResetBuffer();
    else
    {
      // Assign token pointers and change intertoken whitespace to ASCII NULs

      char *cptr = cv;

      while (*cptr)
      {
        while (IsWS(*cptr)) *cptr++ = '\0';
        if (*cptr)
        {
          if (tokencount < tokensLen) tokens[tokencount++] = cptr;
          while (*cptr && !IsWS(*cptr)) cptr++;
          if (IsWS(*cptr)) *cptr++ = '\0';
        };
      };
    };
  };
}

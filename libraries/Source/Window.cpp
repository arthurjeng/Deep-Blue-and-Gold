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

#include "Score.h"
#include "Window.h"

void Window::Encode(char **bptrptr) const
{
  EncodeChar(bptrptr, '[');
  Score::EncodeSv(bptrptr, beta); EncodeChar(bptrptr, ' '); Score::EncodeSv(bptrptr, alfa);
  EncodeChar(bptrptr, ']');
}

void Window::Print(void) const
{
  char cv[35], *cptr = cv;

  Encode(&cptr); *cptr++ = '\0'; PrintStr(cv);
}

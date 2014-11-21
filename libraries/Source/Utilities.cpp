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
#include <sys/time.h>
#include <cassert>
#include <cstdlib>
#include <iostream>
#endif

#if (IsTarget)
unsigned long int TargetElapsedMsec(void);
bool TargetSerialAvailable(void);
char TargetSerialInput(void);
void TargetSerialOutput(const char ch);
void TargetRandomSeed(const unsigned int seed);
long int TargetRandomPick(const unsigned int limit);
void TargetLEDOutput(const bool flag);
void TargetAudioAlert(void);
#endif

// ******** Audible output

void Beep(void)
{
#if (IsDevHost)
  std::cerr << '\a' << std::flush;
#endif

#if (IsTarget)
  TargetAudioAlert();
#endif
}

// ******** Basic reading

bool IsWS(const char ch)
{
  // In the input character white space?

  return (ch == ' ') || (ch == '\t') || (ch == '\n') || (ch == '\r');
}

#if (IsTarget)
static bool IsLineEndingChar(const char ch)
{
#if (0)
  return (ch == '\n') || (ch == '\r') || (ch == 0x04);
#else
  return (ch == '\n') || (ch == '\r') || (ch == 0x04) || (ch == '$'); // IDE bug
#endif
}
#endif

void ReadLine(char *cv, const ui limit, bool& eof)
{
  // Note: All serial input goes through this routine

#if (IsDevHost)
  std::cin.getline(cv, limit); eof = std::cin.eof();
#endif

#if (IsTarget)
  ui count = 0;
  char ch;

  do
  {
    while (!TargetSerialAvailable());
    ch = TargetSerialInput();
    if (!IsLineEndingChar(ch))
    {
      TargetSerialOutput(ch); if (count < (limit - 1)) cv[count++] = ch;
    };
  }
  while (!IsLineEndingChar(ch));
  PrintNL(); cv[count++] = '\0'; eof = (ch == 0x04);
#endif
}

// ******** General encoding routines

void EncodeChar(char **bptrptr, const char ch) {**bptrptr = ch; (*bptrptr)++;}

void EncodeStr(char **bptrptr, const char *str)
{
  while (*str) EncodeChar(bptrptr, *str++);
}

void EncodeDigit(char **bptrptr, const ui digit)
{
  EncodeChar(bptrptr, CharOfDigit(digit));
}

void EncodeUi16(char **bptrptr, const ui16 value)
{
  if (value == 0) EncodeDigit(bptrptr, 0);
  else
  {
    ui16 residue = value;
    ui count = 0;
    char cv[5];

    while (residue) {cv[count++] = residue % 10; residue /= 10;};
    while (count) EncodeDigit(bptrptr, cv[--count]);
  };
}

void EncodeSi16(char **bptrptr, const si16 value)
{
  if (value >= 0) EncodeUi16(bptrptr, (ui16) value);
  else
  {
    EncodeChar(bptrptr, '-'); EncodeUi16(bptrptr, (ui16) -value);
  };
}

void EncodeUi32(char **bptrptr, const ui32 value)
{
  if (value == 0) EncodeDigit(bptrptr, 0);
  else
  {
    ui32 residue = value;
    ui count = 0;
    char cv[10];

    while (residue) {cv[count++] = residue % 10; residue /= 10;};
    while (count) EncodeDigit(bptrptr, cv[--count]);
  };
}

void EncodeSi32(char **bptrptr, const si32 value)
{
  if (value >= 0) EncodeUi32(bptrptr, (ui32) value);
  else
  {
    EncodeChar(bptrptr, '-'); EncodeUi32(bptrptr, (ui32) -value);
  };
}

// ******** Options

void RetrieveOptnString(const optnType optn, char cv[])
{
  // Copy the indicated option string to the given character buffer

  for (ui index = 0; index < optnstrLen; index++)
    cv[index] = ReadFlashUi8((unsigned char *) fsOptnStrs + (optn * optnstrLen) + index);
  cv[optnstrLen] = '\0';
}

optnType DecodeOption(const char *str)
{
  // Attempt to decode the supplied string as an option

  optnType optn = optnNil;

  char cv[optnstrLen + 1];
  si lo = 0, hi = optnLen - 1;

  // Do a binary search of the control option list to find a match

  while (IsOptnNil(optn) && (lo <= hi))
  {
    const si index = (lo + hi) / 2;

    RetrieveOptnString((optnType) index, cv);

    const si compare = StrCmp(str, cv);

    if (compare == 0) optn = (optnType) index;
    else
    {
      if (compare < 0) hi = index - 1; else lo = index + 1;
    };
  };
  return optn;
}

// ******** Basic printing routines

void PrintChar(const char ch)
{
  // Note: All serial output goes through this routine (except book building)

#if (IsDevHost)
  std::cout << ch << std::flush;
#endif

#if (IsTarget)
  TargetSerialOutput(ch);
#endif
}

void PrintStr(const char *bptr) {while (*bptr) PrintChar(*bptr++);}

void PrintSpace(void) {PrintChar(' ');}

void PrintSpaces(const ui count) {for (ui index = 0; index < count; index++) PrintSpace();}

void PrintISM(void) {PrintSpaces(3);}

void PrintOptn(const optnType optn)
{
  char cv[optnstrLen + 1];

  PrintChar('['); RetrieveOptnString(optn, cv); PrintStr(cv); PrintChar(']'); PrintSpace();
}

void PrintNL(void) {PrintChar('\n');}

void PrintDigit(const ui digit) {PrintChar(CharOfDigit(digit));}

void PrintHexByte(const ui value)
{
  PrintDigit(value / 16); PrintDigit(value % 16);
}

void PrintUi16(const ui16 value)
{
  char cv[6], *cptr = cv;

  EncodeUi16(&cptr, value); *cptr++ = '\0'; PrintStr(cv);
}

void PrintSi16(const si16 value)
{
  char cv[7], *cptr = cv;

  EncodeSi16(&cptr, value); *cptr++ = '\0'; PrintStr(cv);
}

void PrintUi32(const ui32 value)
{
  char cv[11], *cptr = cv;

  EncodeUi32(&cptr, value); *cptr++ = '\0'; PrintStr(cv);
}

void PrintSi32(const si32 value)
{
  char cv[12], *cptr = cv;

  EncodeSi32(&cptr, value); *cptr++ = '\0'; PrintStr(cv);
}

void PrintMsecAsSeconds(const msType msec)
{
  const ui32 remainder = msec % 1000;

  PrintUi32(msec / 1000); PrintChar('.');
  if (remainder < 100) PrintChar('0'); if (remainder < 10) PrintChar('0');
  PrintUi32(remainder);
}

void PrintSearchTermination(const stType st)
{
  switch (st)
  {
    case stAllCertain:   PrintFS(fsStAllCertain);   break;
    case stBook:         PrintFS(fsStBook);         break;
    case stForcedLose:   PrintFS(fsStForcedLose);   break;
    case stForcedMate:   PrintFS(fsStForcedMate);   break;
    case stInterrupt:    PrintFS(fsStInterrupt);    break;
    case stLimitDepth:   PrintFS(fsStLimitDepth);   break;
    case stLimitTime:    PrintFS(fsStLimitTime);    break;
    case stMateIn1:      PrintFS(fsStMateIn1);      break;
    case stNoMoves:      PrintFS(fsStNoMoves);      break;
    case stSingleton:    PrintFS(fsStSingleton);    break;
    case stUnterminated: PrintFS(fsStUnterminated); break;
    default: SwitchFault(); break;
  };
}

void PrintForcedTermination(const ftType ft)
{
  switch (ft)
  {
    case ftCheckmate:        PrintFS(fsFtCheckmate);        break;
    case ftDrawFiftyMoves:   PrintFS(fsFtDrawFiftyMoves);   break;
    case ftDrawInsufficient: PrintFS(fsFtDrawInsufficient); break;
    case ftDrawRepetition:   PrintFS(fsFtDrawRepetition);   break;
    case ftDrawStalemate:    PrintFS(fsFtDrawStalemate);    break;
    case ftUnterminated:     PrintFS(fsFtUnterminated);     break;
    default: SwitchFault(); break;
  };
}

void PrintGameOver(const ftType ft)
{
  if (ft != ftUnterminated) {PrintFSL(fsLbGameOver); PrintForcedTermination(ft);};
}

void PrintProgID(void)
{
  PrintFS(fsBanner0); PrintFS(fsBanner1); PrintFS(fsBanner2); PrintFS(fsBanner3);
}

// ******** Space printing routines

void PrintFile(const fileType file) {PrintChar(CharOfFile(file));}

void PrintRank(const rankType rank) {PrintChar(CharOfRank(rank));}

void PrintSqrBasic(const sqrType sqr)
{
  PrintFile(MapSqrToFile(sqr)); PrintRank(MapSqrToRank(sqr));
}

void PrintSqrMeta(const sqrType sqr)
{
  if (sqr == sqrNil) PrintChar('-'); else PrintSqrBasic(sqr);
}

// ******** Force printing routines

void PrintColor(const colorType color) {PrintChar(CharOfColor(color));}

void PrintPiece(const pieceType piece) {PrintChar(CharOfPiece(piece));}

void PrintMan(const manType man)
{
  PrintColor(cvmantocolor[man]); PrintPiece(cvmantopiece[man]);
}

void PrintManMPD(const manType man) {PrintChar(CharOfMan(man));}

// ******** Motion printing routines

void PrintCast(const castType cast)
{
  if (cast == castNil) PrintChar('?'); else PrintChar(CharOfCast(cast));
}

void PrintCsab(const csabType csab)
{
  if (csab == csabNone) PrintChar('-');
  else
  {
    // Use FEN ordering

    if (csab & csabWK) PrintCast(castWK); if (csab & csabWQ) PrintCast(castWQ);
    if (csab & csabBK) PrintCast(castBK); if (csab & csabBQ) PrintCast(castBQ);
  };
}

// ******** Membership/Mapping routines

bool IsCharDigit(const char ch) {return (ch >= '0') && (ch <= '9');}

ui MapCharToDigit(const char ch) {return ch - '0';}

bool IsCharColor(const char ch) {return IsColorNotNil(MapCharToColor(ch));}

colorType MapCharToColor(const char ch)
{
  colorType color = colorNil;
  ui index = 0;

  while (IsColorNil(color) && (index < colorRLen))
    if (ch == CharOfColor(index)) color = index; else index++;
  return color;
}

bool IsCharFile(const char ch) {return IsFileNotNil(MapCharToFile(ch));}

fileType MapCharToFile(const char ch)
{
  fileType file = fileNil;
  ui index = 0;

  while (IsFileNil(file) && (index < fileLen))
    if (ch == CharOfFile(index)) file = index; else index++;
  return file;
}

bool IsCharRank(const char ch) {return IsRankNotNil(MapCharToRank(ch));}

rankType MapCharToRank(const char ch)
{
  rankType rank = rankNil;
  ui index = 0;

  while (IsRankNil(rank) && (index < rankLen))
    if (ch == CharOfRank(index)) rank = index; else index++;
  return rank;
}

bool IsCharMan(const char ch) {return IsManNotNil(MapCharToMan(ch));}

manType MapCharToMan(const char ch)
{
  manType man = manNil;
  ui index = 0;

  while (IsManNil(man) && (index < manRLen))
    if (ch == CharOfMan(index)) man = index; else index++;
  return man;
}

bool IsCharCast(const char ch) {return IsCastNotNil(MapCharToCast(ch));}

castType MapCharToCast(const char ch)
{
  castType cast = manNil;
  ui index = 0;

  while (IsCastNil(cast) && (index < castLen))
    if (ch == CharOfCast(index)) cast = index; else index++;
  return cast;
}

bool IsStrUnsignedInt(const char *str)
{
  bool valid = StrLen(str) > 0;

  if (valid)
  {
    char ch;

    while (valid && (ch = *str++)) if (!IsCharDigit(ch)) valid = false;
  };
  return valid;
}

ui32 MapStrToUi32(const char *str)
{
  ui32 value = 0;
  char ch;

  while ((ch = *str++)) {value *= 10; value += MapCharToDigit(ch);};
  return value;
}

// ******** Misc routines

si StrCmp(const char *str0, const char *str1)
{
  // Compare two strings, but without bringing in the cstring routines

  unsigned char ch0, ch1;
  si compare = 0;

  do {ch0 = *str0++; ch1 = *str1++;} while (ch0 && (ch0 == ch1));
  if (!ch0 || !ch1)
  {
    if (!ch0 && !ch1) compare = 0; else compare = (!ch0) ? -1 : 1;
  }
  else
    compare = (ch0 < ch1) ? -1 : 1;
  return compare;
}

ui StrLen(const char *str)
{
  // Find the length of a string, but without bringing in the cstring routines

  ui count = 0;

  while (*str++) count++;
  return count;
}

void RandomSeed(const ui seed)
{
#if (IsDevHost)
  srandom(seed);
#endif

#if (IsTarget)
  return TargetRandomSeed(seed);
#endif
}

ui RandomPick(const ui limit)
{
#if (IsDevHost)
  return ((ui) random()) % limit;
#endif

#if (IsTarget)
  return TargetRandomPick(limit);
#endif
}

// ******** Flash string access

void EncodeFS(char **bptrptr, const char *fs)
{
  char ch;

  while ((ch = ReadFlashUi8((unsigned char *) fs++))) EncodeChar(bptrptr, ch);
}

void PrintFS(const char *fs)
{
  char ch;

  while ((ch = ReadFlashUi8((unsigned char *) fs++))) PrintChar(ch);
}

void PrintFSL(const char *fs) {PrintFS(fs); PrintChar(':'); PrintSpace();}

// ******** Activity LED control

#if (IsTarget)
void PutActivityLEDState(const bool flag)
{
  TargetLEDOutput(flag);
}
#endif

void SetActivityLEDState(void)
{
#if (IsTarget)
  PutActivityLEDState(true);
#endif
}

void ResetActivityLEDState(void)
{
#if (IsTarget)
  PutActivityLEDState(false);
#endif
}

// ******** Time routines

#if (IsDevHost)
static si StSec;
static si StUsec;
#endif

void MarkStartTime(void)
{
#if (IsDevHost)
  struct timeval tval;

  gettimeofday(&tval, 0); StSec = (si32) tval.tv_sec; StUsec = (si32) tval.tv_usec;
#endif
}

msType ElapsedMsec(void)
{
#if (IsDevHost)
  struct timeval tval;

  gettimeofday(&tval, 0);
  while (StUsec > tval.tv_usec) {tval.tv_sec--; tval.tv_usec += 1000000;};
  return (msType) ((tval.tv_sec - StSec) * 1000) + ((tval.tv_usec - StUsec) / 1000);
#endif

#if (IsTarget)
  return TargetElapsedMsec();
#endif
}

// ******** Death routines

void SwitchFault(void) {PrintFS(fsFeSwitchFault); Die();}

void Die(void)
{
#if (IsDevHost)
  exit(1);
#endif

#if (IsTarget)
  while (true);
#endif
}

void DieStr(const char *str) {PrintStr(str); Die();}

void DieFS(const char *fs) {PrintFS(fs); Die();}

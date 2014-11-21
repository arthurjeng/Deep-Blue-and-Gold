// Myopic: A simple chess program for small systems
//
// Copyright (C) 2010 by chessnotation@me.com   (Some rights reserved)
//
// License: Creative Commons Attribution-Share Alike 3.0
// See: http://creativecommons.org/licenses/by-sa/3.0/
//
// Caution: No warranty; use at your own risk.

#ifndef Included_Utilities
#define Included_Utilities

// Character mapping

inline char CharOfDigit(const ui digit)
{
  return ReadFlashSi8((const signed char *) charofdigit + digit);
}

inline char CharOfFile(const fileType file)
{
  return ReadFlashSi8((const signed char *) charoffile + file);
}

inline char CharOfRank(const rankType rank)
{
  return ReadFlashSi8((const signed char *) charofrank + rank);
}

inline char CharOfColor(const colorType color)
{
  return ReadFlashSi8((const signed char *) charofcolor + color);
}

inline char CharOfPiece(const pieceType piece)
{
  return ReadFlashSi8((const signed char *) charofpiece + piece);
}

inline char CharOfMan(const manType man)
{
  return ReadFlashSi8((const signed char *) charofman + man);
}

inline char CharOfCast(const castType cast)
{
  return ReadFlashSi8((const signed char *) charofcast + cast);
}

// Misc routines

void Beep(void);

bool IsWS(const char ch);

void ReadLine(char *cv, const ui limit, bool& eof);

void EncodeChar(char **bptrptr, const char ch);
void EncodeStr(char **bptrptr, const char *str);
void EncodeDigit(char **bptrptr, const ui digit);

void EncodeUi16(char **bptrptr, const ui16 value);
void EncodeSi16(char **bptrptr, const si16 value);

void EncodeUi32(char **bptrptr, const ui32 value);
void EncodeSi32(char **bptrptr, const si32 value);

void RetrieveOptnString(const optnType optn, char cv[]);
optnType DecodeOption(const char *str);

void PrintChar(const char ch);
void PrintStr(const char *bptr);

void PrintSpace(void);
void PrintSpaces(const ui count);
void PrintISM(void);

void PrintOptn(const optnType optn);

void PrintNL(void);
void PrintDigit(const ui digit);
void PrintHexByte(const ui value);

void PrintUi16(const ui16 value);
void PrintSi16(const si16 value);

void PrintUi32(const ui32 value);
void PrintSi32(const si32 value);

void PrintMsecAsSeconds(const msType msec);

void PrintSearchTermination(const stType st);
void PrintForcedTermination(const ftType ft);
void PrintGameOver(const ftType ft);

void PrintProgID(void);

bool IsCharDigit(const char ch);
ui MapCharToDigit(const char ch);

bool IsCharFile(const char ch);
fileType MapCharToFile(const char ch);

bool IsCharRank(const char ch);
rankType MapCharToRank(const char ch);

bool IsCharColor(const char ch);
colorType MapCharToColor(const char ch);

bool IsCharMan(const char ch);
manType MapCharToMan(const char ch);

bool IsCharCast(const char ch);
castType MapCharToCast(const char ch);

bool IsStrUnsignedInt(const char *str);
ui32 MapStrToUi32(const char *str);

void PrintFile(const fileType file);
void PrintRank(const rankType rank);
void PrintSqrBasic(const sqrType sqr);
void PrintSqrMeta(const sqrType sqr);

void PrintColor(const colorType color);
void PrintPiece(const pieceType piece);
void PrintMan(const manType man);
void PrintManMPD(const manType man);

void PrintCast(const castType cast);
void PrintCsab(const csabType csab);

si StrCmp(const char *str0, const char *str1);
ui StrLen(const char *str);

void RandomSeed(const ui seed);
ui RandomPick(const ui limit);

void EncodeFS(char **bptrptr, const char *fs);

void PrintFS(const char *fs);
void PrintFSL(const char *fs);

void PutActivityLEDState(const bool flag);

void SetActivityLEDState(void);
void ResetActivityLEDState(void);

void MarkStartTime(void);
msType ElapsedMsec(void);

void SwitchFault(void);
void Die(void);
void DieStr(const char *str);
void DieFS(const char *fs);

#endif

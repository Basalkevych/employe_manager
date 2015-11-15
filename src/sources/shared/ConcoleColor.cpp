#pragma once
#include <windows.h>
#include "ConsoleColor.h"

void  ConsoleColor::SetColor(int text, int background)
{
#ifdef WIN32
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
#endif
}
 
void ConsoleColor::SetColor(Color text, Color background)
{
#ifdef WIN32
   HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
   SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
#endif
}


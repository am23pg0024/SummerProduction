/**
 *	@file
 *	@brief	ログ関連
 */
#include <stdarg.h>
#include <stdio.h>
#include "Logger.h"

/**
 *	@brief	ロガー系初期化
 *	@return	true:初期化成功
 *	@author	Kenji Nishida
 *	@date	2019/12/03
 */
bool InitializeLogger()
{
	remove("unlimited.log");
	return true;
}

/**
 *	@brief	フォーマット付きログ出力
 *	@param	[in] pszFormat	printfの第1引数と同じ
 *	@param	[in] ...		printfの第2引数以降と同じ
 *	@author	Kenji Nishida
 *	@date	2019/12/03
 */
void logPrintf(const char *pszFormat, ...)
{
	va_list vl;
	FILE* fp;
	errno_t err = fopen_s(&fp, "unlimited.log", "a");
	if(err != 0 || fp == NULL) return;

	va_start(vl, pszFormat);
	vfprintf_s(fp, pszFormat, vl);
	va_end(vl);
	fclose(fp);
}
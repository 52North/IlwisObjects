/*IlwisObjects is a framework for analysis, processing and visualization of remote sensing and gis data
Copyright (C) 2018  52n North

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

extern "C" {
#ifdef Q_OS_WIN
#define NULL 0
typedef int (__cdecl *_RTC_error_fn)(int, const char *, int, const char *, const char *, ...);
_RTC_error_fn _CRT_RTC_INITW(void *res0, void **res1, int res2, int res3, int res4);
}
_RTC_error_fn _CRT_RTC_INITW(void *res0, void **res1, int res2, int res3, int res4)
{
return NULL;
#endif
}










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

#ifndef ILWIS3BUF_H
#define ILWIS3BUF_H
#include "kernel_global.h"

namespace Ilwis {
	namespace Ilwis3 {

		template <class T>
		class Buf
		{
		protected:
			T* ptr;
			int iSz;
			bool fExt;
		public:
			int iSize() const;
			T* buf() const { return ptr; }
			Buf() { ptr = 0; iSz = 0; fExt = false; };
			Buf(int iBufSize)
			{
				ptr = 0; iSz = 0; Size(iBufSize); fExt = false;
			}
			Buf(const Buf<T>& buf);
			Buf<T>& operator = (const Buf<T>& buf);
			virtual ~Buf();
			void Size(int iBufSize);
			void Reverse(long iStart = iUNDEF, long iEnd = iUNDEF);
			friend void Swap(Buf<T>& buf1, Buf<T>& buf2)
			{
				T* hptr = buf1.ptr; buf1.ptr = buf2.ptr; buf2.ptr = hptr;
			}
			T  operator[](int i) const
			{
				if (!fExt) { assert(i >= 0 && i < iSz && ptr); } // extended buffers could be there!
				return ptr[i];
			}
			T& operator[](int i)
			{
				if (!fExt) { assert(i >= 0 && i < iSize() && ptr); }
				return ptr[i];
			}
		};

		template <class T> inline int Buf<T>::iSize() const
		{
			return iSz;
		}

		template <class T>
		class BufExt : public Buf<T>
		{
		private:
			int iExtendLo, iExtendHi;
		public:
			int iTotSize() const { return (int)(this->iSz + iExtLo() + iExtHi()); }
			int iExtLo() const { return iExtendLo; }
			int iExtHi() const { return iExtendHi; }
			int iUpper() const { return (int)(this->iSize() + iExtHi() - 1); }
			int iLower() const { return -(int)iExtLo(); }
			BufExt() : Buf<T>() { iExtendLo = iExtendHi = 0; this->fExt = true; };
			BufExt(int iBufSize, int iExtLo = 0, int iExtHi = 0)
				: Buf<T>()
			{
				Size(iBufSize, iExtLo, iExtHi); this->fExt = true;
			}
			BufExt(const BufExt<T>& buf);
			BufExt<T>& operator = (const BufExt<T>& buf);
			virtual ~BufExt();
			void SetPtr()
			{
				this->ptr += iExtLo();
			}
			void ResetPtr()
			{
				this->ptr -= iExtLo();
			}
			void Size(int iBufSize, int iExtLo = 0, int iExtHi = 0);
			friend void Swap(BufExt<T>& buf1, BufExt<T>& buf2)
			{
				T* hptr = buf1.ptr; buf1.ptr = buf2.ptr; buf2.ptr = hptr;
			}
		};

		template <class T>
		Buf<T>::Buf(const Buf<T>& buf)
		{
			ptr = 0;
			iSz = 0;
			fExt = false;
			Size(buf.iSize());
			for (int i = 0; i < iSize(); ++i)
				operator[](i) = buf[i];
			//      memcpy(ptr, buf.buf(), iSize() * sizeof(T));
		}

		template <class T> void Buf<T>::Reverse(long iStart, long iEnd)
		{
			T temp;
			if (iStart == iUNDEF) iStart = 0;
			if (iEnd == iUNDEF) iEnd = iSize();
			for (long i = iStart; i < iEnd / 2; ++i)
			{
				temp = ptr[i];
				ptr[i] = ptr[iEnd - i - 1];
				ptr[iEnd - i - 1] = temp;
			}
		}

		template <class T>
		Buf<T>& Buf<T>::operator = (const Buf<T>& buf) {
			Size(buf.iSize());
			for (int i = 0; i < iSize(); ++i)
				operator[](i) = buf[i];
			//  memcpy(ptr, buf.buf(), iSize() * sizeof(T));
			return *this;
		}

		template <class T>
		Buf<T>::~Buf()
		{
			delete[] ptr;
			ptr = NULL;
			iSz = 0;
		}

		template <class T>
		void Buf<T>::Size(int iBufSize)
		{
			if (iSz == iBufSize)
				return;
			iSz = iBufSize;
			if (ptr) {
				delete[] ptr;
				ptr = 0;
			}
			long iTotSize = sizeof(T) * long(iSize());
			ptr = new T[(long)iSize()];
			//if (0 == ptr)
			//	MemAllocError();
		}

		template <class T>
		BufExt<T>::BufExt(const BufExt<T>& buf) : Buf<T>()
		{
			iExtendLo = buf.iExtLo();
			iExtendHi = buf.iExtHi();
			this->iSz = buf.iSize();
			this->fExt = true;
			this->ptr = new T[(long)buf.iTotSize()];
			//if (0 == ptr)
			//	MemAllocError();
			SetPtr();
			//  memcpy(&ptr[iLower()], &(buf.buf()[iLower()]), iTotSize() * sizeof(T));
			for (int i = iLower(); i <= iUpper(); i++)
				this->operator[](i) = buf[i];
		}

		template <class T>
		BufExt<T>::~BufExt()
		{
			ResetPtr();
		}

		template <class T>
		void BufExt<T>::Size(int iBufSize, int iExtLo, int iExtHi)
		{
			if (this->ptr) {
				ResetPtr();
				delete[] this->ptr;
			}
			this->iSz = iBufSize;
			iExtendLo = iExtLo; iExtendHi = iExtHi;
			if (this->iSz != 0) {
				long iSz = iTotSize();
				this->ptr = new T[iSz];
				//if (0 == ptr)
				//	MemAllocError();
				SetPtr();
			}
			else
				this->ptr = 0;
		}

		template <class T>
		BufExt<T>& BufExt<T>::operator = (const BufExt<T>& buf)
		{
			assert(this->ptr);
			int iMax = min(iUpper(), buf.iUpper());
			int iMin = max(iLower(), buf.iLower());
			for (int i = iMin; i <= iMax; i++)
				this->operator[](i) = buf[i];
			return *this;
		}

		typedef BufExt<byte>    ByteBufExt;
		typedef BufExt<long>    LongBufExt;
		typedef BufExt<double>  RealBufExt;
	}
}

#endif // ILWIS3BUF_H

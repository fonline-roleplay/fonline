#include "Common.h"

namespace FOnline
{
	namespace FileCollection
	{
		class Buffer
		{
			uint Size;
			char* Data;

		public:
			void Resize(uint size)
			{
				if (size == Size)
					return;

				if (Data)
				{
					delete[Size] Data;
					Data = nullptr;
				}

				Size = size;
				if (size != 0)
					Data = new char[size];
			}

			bool ResizeIfLow(uint size)
			{
				if (size <= Size)
					return false;
				Resize(size);
				return true;
			}

			char* Get()
			{
				return Data;
			}
		};

		Buffer MainBuffer;
		
		class File
		{
			Buffer* Buff;
			int RefCounter;
			std::string Path;
		};

		class Collection
		{
			std::map<std::string, File> Lib;

		};
	}
}

#pragma once

#include <string>

namespace Hez::Files
{
	namespace HezIniStringUtil
	{
#ifdef HEZ_PLATFORM_WINDOWS
		const char* const endl = "\r\n";
#else
		const char* const endl = "\n";
#endif

#ifdef HEZ_INI_CASE_SENSITIVE
		inline void toLower(std::string& pStr)
		{
			for (char& c : pStr)
				c = tolower(c);
		}
#endif

		const char* const sWhitespace = " \t\n\r\f\v";

		inline void trim(std::string& pStr)
		{
			pStr.erase(0, pStr.find_first_not_of(sWhitespace));
			pStr.erase(pStr.find_last_not_of(sWhitespace) + 1);
		}

		inline void replace(std::string& pStr, const std::string& pFrom, const std::string& pTo)
		{
			if (pFrom.empty())
				return;

			size_t startPos = 0;
			while ((startPos = pStr.find(pFrom, startPos)) != std::string::npos)
			{
				pStr.replace(startPos, pFrom.length(), pTo);
				startPos += pTo.length();
			}
		}
	}
}
#pragma once

#include <string>
#include <map>

namespace Hez::Files
{
	namespace StringUtil
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

#define HEZ_INI_IFCS_TL(pStr)
#ifdef HEZ_INI_CASE_SENSITIVE \
		HezIniStringUtil::toLower(pStr);
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

	template<typename T>
	class HezIniMap
	{
	public:
		HezIniMap() = default;

		HezIniMap(HezIniMap const& pOther)
		{
			size_t dataSize = pOther.mData.size();

			for (size_t i = 0; i < dataSize; ++i)
			{
				auto const& key = pOther.mData[i].first;
				auto const& value = pOther.mData[i].second;
				mData.emplace_back(key, value);
			}
			mDataIndexMap = TDataIndexMap(pOther.mDataIndexMap);
		}

		T& operator[](const std::string& pKey)
		{
			StringUtil::trim(pKey);

			HEZ_INI_IFCS_TL(pKey);

			auto it = mDataIndexMap.find(pKey);
			if (it == mDataIndexMap.end())
				return mData[setEmpty(pKey)].second;
			else
				return mData[it->second].second;
		}

		T get(const std::string& pKey) const
		{
			StringUtil::trim(pKey);

			HEZ_INI_IFCS_TL(pKey);

			auto it = mDataIndexMap.find(pKey);
			if (it == mDataIndexMap.end())
				return T();
			else
				return mData[it->second].second;
		}

		bool has(const std::string& pKey) const
		{
			StringUtil::trim(pKey);

			HEZ_INI_IFCS_TL(pKey);

			return mDataIndexMap.find(pKey) != mDataIndexMap.end();
		}

		void set(const std::string& pKey, const T& pValue)
		{
			StringUtil::trim(pKey);

			HEZ_INI_IFCS_TL(pKey);

			auto it = mDataIndexMap.find(pKey);
			if (it == mDataIndexMap.end())
				mData[setEmpty(pKey)].second = pValue;
			else
				mData[it->second].second = pValue;
		}

		void set(const TMultiArgs& pArgs)
		{
			for (auto const& it : pArgs)
			{
				auto const& key = it.first;
				auto const& value = it.second;
				set(key, value);
			}
		}

		bool remove(const std::string& pKey)
		{
			StringUtil::trim(pKey);

			HEZ_INI_IFCS_TL(pKey);

			auto it = mDataIndexMap.find(pKey);
			if (it == mDataIndexMap.end())
				return false;

			size_t index = it->second;
			mDataIndexMap.erase(it);
			mData.erase(mData.begin() + index);

			for (auto& it : mDataIndexMap)
			{
				if (it.second > index)
					--it.second;
			}

			return true;
		}

		void clear()
		{
			mDataIndexMap.clear();
			mData.clear();
		}

		size_t size() const
		{
			return mData.size();
		}

		bool empty() const
		{
			return mData.empty();
		}

		constIterator begin() const noexcept { return data().begin(); }
		constIterator end() const noexcept { return data().end(); }

	private:
		inline size_t setEmpty(const std::string& pKey)
		{
			std::size_t index = mData.size();
			mDataIndexMap[pKey] = index;
			mData.emplace_back(pKey, T());
			return index;
		}

	public:
		using constIterator = typename TData::const_iterator;

	private:
		using TDataIndexMap = std::unordered_map<std::string, size_t>;
		using TDataItem = std::pair<std::string, T>;
		using TData = std::vector<TDataItem>;
		using TMultiArgs = std::initializer_list<std::pair<std::string, T>>;

		TDataIndexMap mDataIndexMap;
		TData mData;
	};

	using HezIniStructure = HezIniMap<HezIniMap<std::string>>;

	namespace IniParser
	{
		using TParseValues = std::pair<std::string, std::string>;

		enum class PDataType
		{
			PDATA_NONE,
			PDATA_COMMENT,
			PDATA_SECTION,
			PDATA_KEYVALUE,
			PDATA_UNKNOW
		};

		inline PDataType parseLine(std::string pLine, TParseValues& pParseData)
		{
			pParseData.first.clear();
			pParseData.second.clear();

			StringUtil::trim(pLine);

			if (pLine.empty())
				return PDataType::PDATA_NONE;

			char firstChar = pLine[0];

			if (firstChar == ';')
				return PDataType::PDATA_COMMENT;

			if (firstChar == '[')
			{
				auto commentAt = pLine.find_first_of(';');
				if (commentAt != std::string::npos)
					pLine.erase(commentAt);

				auto closingBracketAt = pLine.find_last_of(']');
				if (closingBracketAt != std::string::npos)
				{
					auto section = pLine.substr(1, closingBracketAt - 1);
					StringUtil::trim(section);
					pParseData.first = section;
					return PDataType::PDATA_SECTION;
				}
			}

			std::string lineNorm = pLine;
			StringUtil::replace(lineNorm, "\\=", "  ");

			auto equalAt = lineNorm.find_first_of('=');
			if (equalAt != std::string::npos)
			{
				auto key = lineNorm.substr(0, equalAt);
				StringUtil::trim(key);
				StringUtil::replace(key, "\\=", "=");

				auto value = lineNorm.substr(equalAt + 1);
				StringUtil::trim(value);

				pParseData.first = key;
				pParseData.second = value;

				return PDataType::PDATA_KEYVALUE;
			}

			return PDataType::PDATA_UNKNOW;
		}
	}
}
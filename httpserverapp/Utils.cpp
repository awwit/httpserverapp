
#include "Utils.h"

#include <cstring>

namespace Utils
{
	void trim(std::string &str)
	{
	//	str.erase(std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun<int, int>(std::isspace) ) ).base(), str.end() );
	//	str.erase(str.begin(), std::find_if(str.begin(), str.end(), std::not1(std::ptr_fun<int, int>(std::isspace) ) ) );

		size_t last = str.find_last_not_of(" \t\n\v\f\r");

		if (std::string::npos == last)
		{
			return str.clear();
		}

		str.assign(str.begin() + str.find_first_not_of(" \t\n\v\f\r"), str.begin() + last + 1);
	}

	char *stlStringToPChar(const std::string &str)
	{
		size_t length = str.length();
		char *s = nullptr;

		if (length)
		{
			s = new char[length + 1];
		#ifdef WIN32
			strcpy_s(s, length + 1, str.c_str() );
		#elif POSIX
			strcpy(s, str.c_str() );
			s[length] = '\0';
		#else
			#error "Undefine platform"
		#endif
		}

		return s;
	}

	void stlMapToRawPairs(Utils::raw_pair *raw[], const std::map<std::string, std::string> &map)
	{
		if (raw && map.size() )
		{
			raw_pair *arr = new raw_pair[map.size()];

			*raw = arr;

			size_t i = 0;

			for (auto it = map.cbegin(); map.cend() != it; ++it, ++i)
			{
				arr[i].key = stlStringToPChar(it->first);
				arr[i].value = stlStringToPChar(it->second);
			}
		}
	}

	void stlUnorderedMapToRawPairs(Utils::raw_pair *raw[], const std::unordered_map<std::string, std::string> &map)
	{
		if (raw && map.size() )
		{
			raw_pair *arr = new raw_pair[map.size()];

			*raw = arr;

			size_t i = 0;

			for (auto it = map.cbegin(); map.cend() != it; ++it, ++i)
			{
				arr[i].key = stlStringToPChar(it->first);
				arr[i].value = stlStringToPChar(it->second);
			}
		}
	}

	void stlUnorderedMultimapToRawPairs(Utils::raw_pair *raw[], const std::unordered_multimap<std::string, std::string> &map)
	{
		if (raw && map.size() )
		{
			raw_pair *arr = new raw_pair[map.size()];

			*raw = arr;

			size_t i = 0;

			for (auto it = map.cbegin(); map.cend() != it; ++it, ++i)
			{
				arr[i].key = stlStringToPChar(it->first);
				arr[i].value = stlStringToPChar(it->second);
			}
		}
	}

	void filesIncomingToRawFilesInfo(Utils::raw_fileinfo *raw[], const std::unordered_multimap<std::string, HttpServer::FileIncoming> &map)
	{
		if (raw && map.size() )
		{
			raw_fileinfo *arr = new raw_fileinfo[map.size()];

			*raw = arr;

			size_t i = 0;

			for (auto it = map.cbegin(); map.cend() != it; ++it, ++i)
			{
				arr[i].key = stlStringToPChar(it->first);

				const HttpServer::FileIncoming &info = it->second;

				arr[i].file_name = stlStringToPChar(info.getName() );
				arr[i].file_type = stlStringToPChar(info.getType() );
				arr[i].file_size = info.getSize();
			}
		}
	}

	void rawPairsToStlMap(std::map<std::string, std::string> &map, const Utils::raw_pair raw[], const size_t count)
	{
		for (size_t i = 0; i < count; ++i)
		{
			map.emplace(raw[i].key, raw[i].value);
		}
	}

	void rawPairsToStlUnorderedMap(std::unordered_map<std::string, std::string> &map, const Utils::raw_pair raw[], const size_t count)
	{
		for (size_t i = 0; i < count; ++i)
		{
			map.emplace(raw[i].key, raw[i].value);
		}
	}

	void rawPairsToStlUnorderedMultimap(std::unordered_multimap<std::string, std::string> &map, const Utils::raw_pair raw[], const size_t count)
	{
		for (size_t i = 0; i < count; ++i)
		{
			map.emplace(raw[i].key, raw[i].value);
		}
	}

	void rawFilesInfoToFilesIncoming(std::unordered_multimap<std::string, HttpServer::FileIncoming> &map, const Utils::raw_fileinfo raw[], const size_t count)
	{
		for (size_t i = 0; i < count; ++i)
		{
			map.emplace(raw[i].key, HttpServer::FileIncoming(raw[i].file_name, raw[i].file_type, raw[i].file_size) );
		}
	}

	void destroyRawPairs(Utils::raw_pair raw[], const size_t count)
	{
		if (raw)
		{
			for (size_t i = 0; i < count; ++i)
			{
				raw_pair &cur = raw[i];

				delete[] cur.key;
				delete[] cur.value;
			}

			delete[] raw;
		}
	}

	void destroyRawFilesInfo(Utils::raw_fileinfo raw[], const size_t count)
	{
		if (raw)
		{
			for (size_t i = 0; i < count; ++i)
			{
				raw_fileinfo &cur = raw[i];

				delete[] cur.key;
				delete[] cur.file_name;
				delete[] cur.file_type;
			}

			delete[] raw;
		}
	}
};
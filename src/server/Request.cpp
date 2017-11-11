
#include "Request.h"

namespace HttpClient
{
	std::string Request::getHeader(const std::string &key) const {
		auto it = headers.find(key);
		return headers.end() != it ? it->second : std::string();
	}

	bool Request::isDataExists(const std::string &key) const {
		return data.cend() != data.find(key);
	}

	std::string Request::getDataAsString(const std::string &key) const {
		auto it = data.find(key);
		return data.end() != it ? it->second : std::string();
	}

	std::vector<std::string> Request::getDataAsArray(const std::string &key) const
	{
		std::vector<std::string> arr;

		size_t count = data.count(key);

		if (count) {
			auto range = data.equal_range(key);

			arr.resize(count);

			size_t i = 0;

			for (auto it = range.first; it != range.second; ++it) {
				arr[i++] = it->second;
			}
		}

		return arr;
	}

	std::string Request::getCookieAsString(const std::string &cookieName) const {
		auto it = cookies.find(cookieName);
		return cookies.end() != it ? it->second : std::string();
	}
}

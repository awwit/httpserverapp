
#include "Request.h"

namespace HttpServer
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

	bool Request::isFileExists(const std::string &key) const {
		return this->files.cend() != this->files.find(key);
	}

	Transfer::FileIncoming Request::getFile(const std::string &key) const {
		auto const it = this->files.find(key);
		return this->files.end() != it ? it->second : Transfer::FileIncoming();
	}

	std::vector<Transfer::FileIncoming> Request::getFilesAsArray(const std::string &key) const
	{
		std::vector<Transfer::FileIncoming> arr;

		const size_t count = this->files.count(key);

		if (count) {
			auto const range = this->files.equal_range(key);

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

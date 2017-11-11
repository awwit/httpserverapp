#pragma once

#include "protocol/ServerProtocol.h"
#include "../transfer/FileIncoming.h"
#include "../transfer/ProtocolVariant.h"

#include <unordered_map>

namespace HttpServer
{
	/**
	 * Структура запроса (входные данные)
	 *
	 * @member const Socket::Adapter &socket - сокет клиента
	 * @member const std::string method - метод применяемый к ресурсу
	 * @member const std::string uri_reference - ссылка на ресурс
	 * @member const std::string document_root - корневая директория приложения
	 * @member const std::unordered_multimap<std::string, std::string> params - параметры ресурса
	 * @member const std::unordered_map<std::string, std::string> headers - заголовки запроса
	 * @member const std::unordered_multimap<std::string, std::string> data - входящие данные запроса
	 * @member const std::unordered_multimap<std::string, Transfer::FileIncoming> files - входящие файлы запроса
	 * @member const std::unordered_multimap<std::string, std::string> cookies - входящие куки запроса
	 */
	struct Request
	{
		ServerProtocol *prot;
		std::string document_root;
		std::string host;
		std::string path;
		std::string method;
		std::unordered_multimap<std::string, std::string> params;
		std::unordered_multimap<std::string, std::string> headers;
		std::unordered_multimap<std::string, std::string> data;
		std::unordered_multimap<std::string, Transfer::FileIncoming> files;
		std::unordered_multimap<std::string, std::string> cookies;

		Transfer::ProtocolVariant protocol_variant;

	public:
		std::string getHeader(const std::string &key) const;

		bool isDataExists(const std::string &key) const;

		std::string getDataAsString(const std::string &key) const;

		std::vector<std::string> getDataAsArray(const std::string &key) const;

		bool isFileExists(const std::string &key) const;

		Transfer::FileIncoming getFile(const std::string &key) const;

		std::vector<Transfer::FileIncoming> getFilesAsArray(const std::string &key) const;

		std::string getCookieAsString(const std::string &cookieName) const;
	};
}

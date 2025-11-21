#pragma once

#include "../Header/Network/NetworkHandler.hpp"

bool NetworkHandler::IsCurlCall(REF(std::string) message)
{
    bool hasHttpMethod = (message.find("POST /") == 0) ||
        (message.find("GET /") == 0) ||
        (message.find("PUT /") == 0) ||
        (message.find("DELETE /") == 0) ||
        (message.find("PATCH /") == 0);

    if (!hasHttpMethod) {
        return false;
    }

    size_t firstLineEnd = message.find("\r\n");
    if (firstLineEnd == std::string::npos) {
        return false;
    }

    std::string firstLine = message.substr(0, firstLineEnd);
    if (firstLine.find("HTTP/1.0") == std::string::npos &&
        firstLine.find("HTTP/1.1") == std::string::npos) {
        return false;
    }

    bool hasHost = message.find("Host:") != std::string::npos;
    bool hasUserAgent = message.find("User-Agent: curl") != std::string::npos;
    bool hasAccept = message.find("Accept: */*") != std::string::npos;

    return hasHost && (hasUserAgent || hasAccept);
}

std::string NetworkHandler::ExtractCurlMessage(REF(std::string) curlMessage)
{
    size_t bodyPos = curlMessage.find("\r\n\r\n");
    if (bodyPos == std::string::npos) {
        return ""; 
    }

    std::string body = curlMessage.substr(bodyPos + 4);

    size_t start = body.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return ""; 
    }

    size_t end = body.find_last_not_of(" \t\r\n");
    return body.substr(start, end - start + 1);
}
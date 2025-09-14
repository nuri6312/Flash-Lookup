#include "dictionary.h"
#include <iostream>
#include <string>
#include <sstream>
#include <regex>
#include <cstdlib>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

using namespace std;

class HttpServer {
private:
    Dictionary dict;
    int serverPort;
    
    string decodeUrl(const string& encoded) {
        string decoded;
        for (size_t i = 0; i < encoded.length(); ++i) {
            if (encoded[i] == '%' && i + 2 < encoded.length()) {
                int value;
                istringstream hex(encoded.substr(i + 1, 2));
                if (hex >> std::hex >> value) {
                    decoded += static_cast<char>(value);
                    i += 2;
                } else {
                    decoded += encoded[i];
                }
            } else if (encoded[i] == '+') {
                decoded += ' ';
            } else {
                decoded += encoded[i];
            }
        }
        return decoded;
    }
    
    string getParam(const string& query, const string& param) {
        regex pattern(param + "=([^&]*)");
        smatch match;
        if (regex_search(query, match, pattern)) {
            return decodeUrl(match[1].str());
        }
        return "";
    }
    
    string escapeJson(const string& text) {
        string escaped;
        for (char c : text) {
            switch (c) {
                case '"': escaped += "\\\""; break;
                case '\\': escaped += "\\\\"; break;
                case '\n': escaped += "\\n"; break;
                case '\r': escaped += "\\r"; break;
                case '\t': escaped += "\\t"; break;
                default: escaped += c; break;
            }
        }
        return escaped;
    }
    
    string searchWord(const string& word) {
        auto result = dict.findWord(word);
        if (result.found) {
            return "{\"found\":true,\"word\":\"" + escapeJson(word) + 
                   "\",\"definition\":\"" + escapeJson(result.definition) + "\"}";
        } else {
            auto corrections = dict.getCorrections(word, 2, 3);
            string json = "[";
            for (size_t i = 0; i < corrections.size(); i++) {
                if (i > 0) json += ",";
                json += "\"" + escapeJson(corrections[i]) + "\"";
            }
            json += "]";
            return "{\"found\":false,\"corrections\":" + json + "}";
        }
    }
    
    string getSuggestions(const string& prefix) {
        auto suggestions = dict.getSuggestions(prefix, 10);
        string json = "[";
        for (size_t i = 0; i < suggestions.size(); i++) {
            if (i > 0) json += ",";
            json += "\"" + escapeJson(suggestions[i]) + "\"";
        }
        json += "]";
        return json;
    }
    
    string getCorrections(const string& word) {
        auto corrections = dict.getCorrections(word, 3, 10);
        string json = "[";
        for (size_t i = 0; i < corrections.size(); i++) {
            if (i > 0) json += ",";
            json += "\"" + escapeJson(corrections[i]) + "\"";
        }
        json += "]";
        return json;
    }
    
    string buildResponse(const string& body, const string& contentType = "application/json") {
        string response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: " + contentType + "\r\n";
        response += "Access-Control-Allow-Origin: *\r\n";
        response += "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
        response += "Access-Control-Allow-Headers: Content-Type\r\n";
        response += "Content-Length: " + to_string(body.length()) + "\r\n";
        response += "\r\n";
        response += body;
        return response;
    }
    
public:
    HttpServer(int port) : serverPort(port) {}
    
    bool loadDictionary(const string& csvFile) {
        cout << "Loading dictionary..." << endl;
        return dict.loadFromFile(csvFile);
    }
    
    void run() {
#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            cerr << "WSAStartup failed" << endl;
            return;
        }
#endif
        
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1) {
            cerr << "Socket creation failed" << endl;
            return;
        }
        
        int opt = 1;
#ifdef _WIN32
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
#else
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif
        
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(serverPort);
        
        if (bind(serverSocket, (struct sockaddr*)&address, sizeof(address)) < 0) {
            cerr << "Bind failed" << endl;
            return;
        }
        
        if (listen(serverSocket, 3) < 0) {
            cerr << "Listen failed" << endl;
            return;
        }
        
        cout << "Dictionary server running on http://localhost:" << serverPort << endl;
        cout << "Dictionary loaded with " << dict.totalSize() << " total entries (" << dict.size() << " unique words)" << endl;
        cout << "CORS enabled" << endl;
        
        while (true) {
            int clientSocket = accept(serverSocket, nullptr, nullptr);
            if (clientSocket < 0) continue;
            
            char buffer[4096] = {0};
            recv(clientSocket, buffer, 4096, 0);
            
            string request(buffer);
            string response;
            
            if (request.find("GET /api/search") != string::npos) {
                size_t queryStart = request.find("?");
                if (queryStart != string::npos) {
                    size_t queryEnd = request.find(" ", queryStart);
                    string query = request.substr(queryStart + 1, queryEnd - queryStart - 1);
                    string word = getParam(query, "word");
                    response = buildResponse(searchWord(word));
                }
            } else if (request.find("GET /api/suggestions") != string::npos) {
                size_t queryStart = request.find("?");
                if (queryStart != string::npos) {
                    size_t queryEnd = request.find(" ", queryStart);
                    string query = request.substr(queryStart + 1, queryEnd - queryStart - 1);
                    string prefix = getParam(query, "prefix");
                    response = buildResponse(getSuggestions(prefix));
                }
            } else if (request.find("GET /api/corrections") != string::npos) {
                size_t queryStart = request.find("?");
                if (queryStart != string::npos) {
                    size_t queryEnd = request.find(" ", queryStart);
                    string query = request.substr(queryStart + 1, queryEnd - queryStart - 1);
                    string word = getParam(query, "word");
                    response = buildResponse(getCorrections(word));
                }
            } else if (request.find("OPTIONS") != string::npos) {
                response = buildResponse("", "text/plain");
            } else {
                response = buildResponse("{\"error\":\"Not found\"}", "application/json");
            }
            
            send(clientSocket, response.c_str(), response.length(), 0);
#ifdef _WIN32
            closesocket(clientSocket);
#else
            close(clientSocket);
#endif
        }
        
#ifdef _WIN32
        closesocket(serverSocket);
        WSACleanup();
#else
        close(serverSocket);
#endif
    }
};

int main() {
    HttpServer server(8080);
    
    if (!server.loadDictionary("dictionary.csv")) {
        cerr << "Failed to load dictionary" << endl;
        return 1;
    }
    
    server.run();
    return 0;
}
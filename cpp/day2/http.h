#pragma once

#include "utils.h"

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <algorithm>
#include <cctype>
#include <cstring>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>


namespace utils {
    
    std::string httpGet(const std::string& url) {
        std::string scheme_prefix = "http://";
        std::string rest = url.substr(scheme_prefix.size());

        std::string host;
        std::string port = "80";
        std::string path = "/";

        // Extract host (possibly [ipv6]) and optional port, and path
        size_t path_pos = rest.find('/');
        if (path_pos != std::string::npos) {
            path = rest.substr(path_pos);
            rest = rest.substr(0, path_pos);
        }

        if (!rest.empty() && rest.front() == '[') {
            // IPv6 literal [addr]:port
            size_t end_bracket = rest.find(']');
            if (end_bracket == std::string::npos)
                throw std::runtime_error("malformed IPv6 host");
            host = rest.substr(1, end_bracket - 1);
            if (end_bracket + 1 < rest.size() && rest[end_bracket + 1] == ':')
                port = rest.substr(end_bracket + 2);
        } else {
            size_t colon = rest.find(':');
            if (colon != std::string::npos) {
                host = rest.substr(0, colon);
                port = rest.substr(colon + 1);
            } else {
                host = rest;
            }
        }
        if (host.empty()) throw std::runtime_error("empty host");

        // Resolve
        addrinfo hints{};
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_family = AF_UNSPEC;

        addrinfo* res = nullptr;
        int err = getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
        if (err != 0) {
            throw std::runtime_error(std::string("getaddrinfo: ") + gai_strerror(err));
        }

        int sock = -1;
        for (addrinfo* rp = res; rp != nullptr; rp = rp->ai_next) {
            sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (sock == -1) continue;
            if (connect(sock, rp->ai_addr, rp->ai_addrlen) == 0) break;
            close(sock);
            sock = -1;
        }
        freeaddrinfo(res);
        if (sock == -1) throw std::runtime_error("failed to connect");

        // Build and send request
        std::ostringstream req;
        req << "GET " << path << " HTTP/1.1\r\n";
        req << "Host: " << host << "\r\n";
        req << "User-Agent: cpp23-http/1.0\r\n";
        req << "Connection: close\r\n";
        req << "\r\n";
        std::string reqs = req.str();
        size_t sent = 0;
        while (sent < reqs.size()) {
            ssize_t n = send(sock, reqs.data() + sent, reqs.size() - sent, 0);
            if (n <= 0) {
                close(sock);
                throw std::runtime_error("send failed");
            }
            sent += static_cast<size_t>(n);
        }

        // Read response
        std::string response;
        char buffer[8192];
        while (true) {
            ssize_t n = recv(sock, buffer, sizeof(buffer), 0);
            if (n < 0) {
                close(sock);
                throw std::runtime_error("recv failed");
            }
            if (n == 0) break;
            response.append(buffer, static_cast<size_t>(n));
        }
        close(sock);

        // Split headers and body
        size_t hdr_end = response.find("\r\n\r\n");
        if (hdr_end == std::string::npos) return ""; // no headers
        std::string header_block = response.substr(0, hdr_end);
        std::string body = response.substr(hdr_end + 4);

        // Parse headers
        std::istringstream hs(header_block);
        std::string status_line;
        std::getline(hs, status_line);
        std::map<std::string, std::string> headers;
        std::string line;
        while (std::getline(hs, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            size_t colon = line.find(':');
            if (colon == std::string::npos) continue;
            std::string name = line.substr(0, colon);
            std::string value = line.substr(colon + 1);
            trim(name);
            trim(value);
            std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c){ return std::tolower(c); });
            headers[name] = value;
        }

        // Handle chunked transfer encoding
        auto iequals = [](const std::string& a, const std::string& b){
            if (a.size() != b.size()) return false;
            for (size_t i=0;i<a.size();++i) if (std::tolower((unsigned char)a[i]) != std::tolower((unsigned char)b[i])) return false;
            return true;
        };

        if (headers.count("transfer-encoding")) {
            std::string te = headers["transfer-encoding"];
            if (te.find("chunked") != std::string::npos || iequals(te, "chunked")) {
                std::string decoded;
                size_t pos = 0;
                while (true) {
                    size_t crlf = body.find("\r\n", pos);
                    if (crlf == std::string::npos) break;
                    std::string lenstr = body.substr(pos, crlf - pos);
                    // ignore chunk extensions
                    size_t semi = lenstr.find(';');
                    if (semi != std::string::npos) lenstr = lenstr.substr(0, semi);
                    unsigned long chunk_len = 0;
                    try {
                        chunk_len = std::stoul(lenstr, nullptr, 16);
                    } catch (...) {
                        throw std::runtime_error("invalid chunk size");
                    }
                    pos = crlf + 2;
                    if (chunk_len == 0) break;
                    if (pos + chunk_len > body.size()) {
                        // incomplete chunk; return what we have
                        decoded.append(body.substr(pos));
                        break;
                    }
                    decoded.append(body.substr(pos, chunk_len));
                    pos += chunk_len;
                    // skip CRLF after chunk
                    if (pos + 2 <= body.size() && body[pos] == '\r' && body[pos+1] == '\n') pos += 2;
                }
                return decoded;
            }
        }

        // If Content-Length present, try to honor it
        if (headers.count("content-length")) {
            try {
                size_t clen = static_cast<size_t>(std::stoull(headers["content-length"]));
                if (body.size() >= clen) return body.substr(0, clen);
                // otherwise return whatever body we have
                return body;
            } catch (...) {
                return body;
            }
        }

        return body;
    }

    std::string httpsGet(const std::string& url) {
        std::string scheme = "https://";
        std::string rest = url.substr(scheme.size());

        std::string hostname;
        std::string port = "443";
        std::string path = "/";

        size_t p = rest.find('/');
        if (p != std::string::npos) {
            path = rest.substr(p);
            rest = rest.substr(0, p);
        }

        if (!rest.empty() && rest.front() == '[') {
            size_t endb = rest.find(']');
            if (endb == std::string::npos) throw std::runtime_error("malformed IPv6 host");
            hostname = rest.substr(1, endb - 1);
            if (endb + 1 < rest.size() && rest[endb + 1] == ':') port = rest.substr(endb + 2);
        } else {
            size_t c = rest.find(':');
            if (c != std::string::npos) {
                hostname = rest.substr(0, c);
                port = rest.substr(c + 1);
            } else {
                hostname = rest;
            }
        }
        if (hostname.empty()) throw std::runtime_error("empty host");

        // Resolve
        addrinfo hints{};
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_family = AF_UNSPEC;
        addrinfo* res = nullptr;
        int gai = getaddrinfo(hostname.c_str(), port.c_str(), &hints, &res);
        if (gai != 0) throw std::runtime_error(std::string("getaddrinfo: ") + gai_strerror(gai));

        int sock = -1;
        for (addrinfo* rp = res; rp != nullptr; rp = rp->ai_next) {
            sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (sock == -1) continue;
            if (connect(sock, rp->ai_addr, rp->ai_addrlen) == 0) break;
            close(sock);
            sock = -1;
        }
        freeaddrinfo(res);
        if (sock == -1) throw std::runtime_error("failed to connect");

        // Initialize OpenSSL
        SSL_library_init();
        SSL_load_error_strings();
        OpenSSL_add_all_algorithms();

        SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
        if (!ctx) {
            close(sock);
            throw std::runtime_error("unable to create SSL_CTX");
        }
        if (SSL_CTX_set_default_verify_paths(ctx) != 1) {
            SSL_CTX_free(ctx);
            close(sock);
            throw std::runtime_error("failed to set default verify paths");
        }
        SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, nullptr);

        SSL* ssl = SSL_new(ctx);
        if (!ssl) {
            SSL_CTX_free(ctx);
            close(sock);
            throw std::runtime_error("unable to create SSL object");
        }

        // SNI
        if (!hostname.empty()) SSL_set_tlsext_host_name(ssl, hostname.c_str());
        if (SSL_set_fd(ssl, sock) != 1) {
            SSL_free(ssl);
            SSL_CTX_free(ctx);
            close(sock);
            throw std::runtime_error("failed to associate socket with SSL");
        }

        if (SSL_connect(ssl) != 1) {
            unsigned long err = ERR_get_error();
            std::string errstr = ERR_error_string(err, nullptr);
            SSL_free(ssl);
            SSL_CTX_free(ctx);
            close(sock);
            throw std::runtime_error(std::string("SSL_connect failed: ") + errstr);
        }

        long vres = SSL_get_verify_result(ssl);
        if (vres != X509_V_OK) {
            SSL_shutdown(ssl);
            SSL_free(ssl);
            SSL_CTX_free(ctx);
            close(sock);
            throw std::runtime_error(std::string("certificate verification failed: ") + X509_verify_cert_error_string(vres));
        }

        // Build request
        std::ostringstream req;
        req << "GET " << path << " HTTP/1.1\r\n";
        req << "Host: " << hostname << (port != "443" ? ":" + port : "") << "\r\n";
        req << "User-Agent: cpp23-https/1.0\r\n";
        req << "Connection: close\r\n";
        req << "\r\n";
        std::string reqs = req.str();

        // Send via SSL
        size_t sent = 0;
        while (sent < reqs.size()) {
            int w = SSL_write(ssl, reqs.data() + sent, static_cast<int>(reqs.size() - sent));
            if (w <= 0) {
                int e = SSL_get_error(ssl, w);
                SSL_shutdown(ssl);
                SSL_free(ssl);
                SSL_CTX_free(ctx);
                close(sock);
                throw std::runtime_error(std::string("SSL_write failed: ") + std::to_string(e));
            }
            sent += static_cast<size_t>(w);
        }

        // Read response
        std::string response;
        char buf[8192];
        for (;;) {
            int r = SSL_read(ssl, buf, sizeof(buf));
            if (r > 0) {
                response.append(buf, static_cast<size_t>(r));
                continue;
            }
            if (r == 0) break;
            int err = SSL_get_error(ssl, r);
            if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) continue;
            unsigned long e = ERR_get_error();
            std::string estr = ERR_error_string(e, nullptr);
            SSL_shutdown(ssl);
            SSL_free(ssl);
            SSL_CTX_free(ctx);
            close(sock);
            throw std::runtime_error(std::string("SSL_read failed: ") + estr);
        }

        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sock);

        // Separate headers/body
        size_t sep = response.find("\r\n\r\n");
        if (sep == std::string::npos) return ""; // no headers
        std::string hdrs = response.substr(0, sep);
        std::string body = response.substr(sep + 4);

        // Parse headers into map
        std::istringstream sh(hdrs);
        std::string status;
        std::getline(sh, status);
        std::map<std::string, std::string> hdrmap;
        std::string line;

        while (std::getline(sh, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            size_t col = line.find(':');
            if (col == std::string::npos) continue;
            std::string n = line.substr(0, col);
            std::string v = line.substr(col + 1);
            trim(n);
            trim(v);
            std::transform(n.begin(), n.end(), n.begin(), [](unsigned char c){ return std::tolower(c); });
            hdrmap[n] = v;
        }

        auto iequals = [](const std::string& a, const std::string& b) {
            if (a.size() != b.size()) return false;
            for (size_t i = 0; i < a.size(); ++i)
                if (std::tolower((unsigned char)a[i]) != std::tolower((unsigned char)b[i])) return false;
            return true;
        };

        // Handle chunked
        if (hdrmap.count("transfer-encoding")) {
            std::string te = hdrmap["transfer-encoding"];
            if (te.find("chunked") != std::string::npos || iequals(te, "chunked")) {
                std::string out;
                size_t pos = 0;
                while (true) {
                    size_t crlf = body.find("\r\n", pos);
                    if (crlf == std::string::npos) break;
                    std::string lenline = body.substr(pos, crlf - pos);
                    size_t semi = lenline.find(';');
                    if (semi != std::string::npos) lenline = lenline.substr(0, semi);
                    unsigned long len = 0;
                    try { len = std::stoul(lenline, nullptr, 16); } catch (...) { throw std::runtime_error("invalid chunk size"); }
                    pos = crlf + 2;
                    if (len == 0) break;
                    if (pos + len > body.size()) {
                        out.append(body.substr(pos));
                        break;
                    }
                    out.append(body.substr(pos, len));
                    pos += len;
                    if (pos + 2 <= body.size() && body[pos] == '\r' && body[pos+1] == '\n') pos += 2;
                }
                return out;
            }
        }

        if (hdrmap.count("content-length")) {
            try {
                size_t clen = static_cast<size_t>(std::stoull(hdrmap["content-length"]));
                if (body.size() >= clen) return body.substr(0, clen);
                return body;
            } catch (...) {
                return body;
            }
        }

        return body;
    }

    std::string getUrl(const std::string& url) {
        if (url.rfind("http://", 0) == 0) {
            return httpGet(url);
        } else if (url.rfind("https://", 0) == 0) {
            return httpsGet(url);
        } else {
            throw std::runtime_error("unsupported URL scheme");
        }
    }

}
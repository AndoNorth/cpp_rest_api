#include <iostream>
#include <string>
#include <vector>
#include <functional> // for template functions for body handlers

#include <boost/asio.hpp>

namespace asio = boost::asio;
                                  
// http client for use later by async calls to resolve endpoints
class HttpClient{
    public:
    HttpClient(asio::io_service& io_service, asio::ip::tcp::resolver& resolver,
    const std::string& host, const std::string& path)
    : _host(host), _path(path), _resolver(resolver), _sock(io_service) {}

    // try to resolve hostname into an IP endpoint
    void Start()
    {
        _resolver.async_resolve
        (
            asio::ip::tcp::resolver::query(_host, "http"),
            [this](const boost::system::error_code& ec,
                        asio::ip::tcp::resolver::iterator it)
            {
                if(ec)
                {
                    std::cout << "Error Resolving " << _host << ": " << ec.message(); // consider using cerr
                    return;
                }
                // assume first endpoint is always available
                std::cout << _host << ": resolved to "
                          << it->endpoint() << std::endl;

                do_connect(it->endpoint());
            }
        );
    }

    private:
    const std::string _host;
    const std::string _path;

    asio::ip::tcp::resolver& _resolver;
    asio::ip::tcp::socket _sock;

    std::string _request;
    asio::streambuf _response;

    void do_connect(const asio::ip::tcp::endpoint& dest)
    {
        _sock.async_connect
        (
            dest, [this](const boost::system::error_code& ec)
            {
                if (ec)
                {
                    std::cout << "Error Connecting" << _host << ": " << ec.message(); // consider using cerr
                    return;
                }
                std::cout << _host << ": connected to "
                          << _sock.remote_endpoint() << std::endl;
                do_send_http_get();
            }
        );
    }
    void do_send_http_get()
    {
        _request = "GET " + _path + " HTTP/1.1\r\nHost: " + _host + "\r\n\r\n"; // 1.1 is human readable http message, \r\n\r\n is to delimit message sections
        asio::async_write
        (
            _sock, asio::buffer(_request),
            [this](const boost::system::error_code& ec, std::size_t size)
            {
                if(ec)
                {
                    std::cout << "Error Sending GET request" << ec; // consider using cerr
                    return;
                }

                std::cout << _host << ": sent " << size << " bytes";
                do_recv_http_get_header();
            }
        );
    }
    void do_recv_http_get_header()
    {
        asio::async_read_until
        (
            _sock, _response, "\r\n\r\n",
            [this](const boost::system::error_code& ec, std::size_t size)
            {
                if(ec)
                {
                    std::cout << "Error Receiving GET header" << ec; // consider using cerr
                    return;
                }
                std::cout << _host << ": received " << size << ", streambuf "
                          << _response.size();
                // extract header from streambuf iterator
                std::string header
                (
                    asio::buffers_begin(_response.data()),
                    asio::buffers_begin(_response.data()) + size
                );
                _response.consume(size);

                std::cout << "----------" << std::endl << _host
                          << ": header length " << header.size() << std::endl
                          << header;

                size_t pos = header.find("Content-Length: ");
                if(pos != std::string::npos)
                {
                    size_t len = std::strtoul
                    (
                        header.c_str() + pos + sizeof("Content-Length: ") - 1,
                        nullptr, 10
                    );
                    do_receive_http_get_body(len - _response.size());
                    return;
                }
                pos = header.find("Transfer-Encoding: chunked");
                if(pos != std::string::npos)
                {
                    do_receive_http_get_chunked_body();
                    return;
                }
                std::cout << "Unknown Body Length";
            }
        );
    }
    // handle two length formats in body
    void do_receive_http_get_body(size_t len)
    {
        asio::async_read
        (
            _sock, _response, asio::transfer_exactly(len),
            std::bind(&HttpClient::handle_http_get_body, this,
            std::placeholders::_1, std::placeholders::_2)
        );
    }
    void do_receive_http_get_chunked_body()
    {
        asio::async_read_until
        (
            _sock, _response, "\r\n\r\n",
            std::bind(&HttpClient::handle_http_get_body, this,
            std::placeholders::_1, std::placeholders::_2)
        );
    }
    void handle_http_get_body(const boost::system::error_code& ec,
    std::size_t size)
    {
        if(ec)
        {
            std::cout << "Error Receiving GET body" << ec; // consider using cerr
            return;
        }
        std::cout << _host << ": received " << size << ", streambuf "
                    << _response.size();
        const auto& data = _response.data();
        std::string body(asio::buffers_begin(data), asio::buffers_end(data));
        _response.consume(size);

        std::cout << "--------" << std::endl << _host << ": body length "
                  << body.size() << std::endl;
        std::cout << body;
    }
};

int main()
{
    asio::io_service io_service;
    asio::ip::tcp::resolver resolver(io_service);

    std::vector<std::unique_ptr<HttpClient>> clients;
    // create http client to resolve end point
    std::unique_ptr<HttpClient> c
    (
        new HttpClient
        (
            io_service, resolver, "swapi.dev", "/api/people/1" // replace with respective URL and URI endpoint
        )
    );
    c->Start();
    clients.push_back(std::move(c));
    io_service.run();

    return 0;
}
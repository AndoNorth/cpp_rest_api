#include <iostream>
#include <string>
#include <vector>

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
            [this](const boost::system::error_code* ec,
                         asio::ip::tcp::resolver::iterator it)
            {
                if(ec)
                {
                    std::cout << "Error Resolving " << _host << ": " << ec->message(); // consider using cerr
                }
                // assume first endpoint is always available
                std::cout << _host << ": resolved to" << it->endpoint();
            }
        );
    }

    private:
    const std::string _host;
    const std::string _path;

    asio::ip::tcp::resolver& _resolver;
    asio::ip::tcp::socket _sock;
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
            io_service, resolver, "hostname", "path"
        )
    );
    c->Start();
    clients.push_back(std::move(c));
    io_service.run();

    return 0;
}
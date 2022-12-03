#pragma once

#include <iostream>
#include <string>

#include <SimpleJSON/json.hpp>
#include <served/multiplexer.hpp>
#include <served/net/server.hpp>

constexpr char kEndpoint[] = "/videos/{id:\\d+}";
constexpr char kIpAddress[] = "127.0.0.1";
constexpr char kPort[] = "5000";
constexpr int kThreads = 10;

class HttpServer
{
public:
    HttpServer(served::multiplexer multiplexer) : multiplexer(multiplexer) {}

    served::served_req_handler HandleGet()
    {
        return [&](served::response &response, const served::request &request)
        {
            json::JSON request_body = json::JSON::Load(request.body());
            std::cout << "GET request recieved" << std::endl;
            request.params["id"];
            /*
            json::JSON video_reponse = json::JSON::Load("get response");
            std::ostringstream stream;
            stream << video_reponse;
            response << stream.str();
            */
            bool get_successful = true;
            get_successful ? served::response::stock_reply(201, response)
                           : served::response::stock_reply(404, response);
        };
    }

    served::served_req_handler HandlePut()
    {
        return [&](served::response &response, const served::request &request)
        {
            json::JSON request_body = json::JSON::Load(request.body());
            std::cout << "PUT request recieved" << std::endl;
            bool insert_successful = true;
            insert_successful ? served::response::stock_reply(200, response)
                              : served::response::stock_reply(404, response);
        };
    }

    served::served_req_handler HandlePost()
    {
        return [&](served::response &response, const served::request &request)
        {
            json::JSON request_body = json::JSON::Load(request.body());
            std::cout << "POST request recieved" << std::endl;
            bool update_successful = true;
            update_successful ? served::response::stock_reply(200, response)
                              : served::response::stock_reply(404, response);
        };
    }

    served::served_req_handler HandleDelete()
    {
        return [&](served::response &response, const served::request &request)
        {
            json::JSON request_body = json::JSON::Load(request.body());
            std::cout << "DELETE request recieved" << std::endl;
            bool delete_successful = true;
            delete_successful ? served::response::stock_reply(200, response)
                              : served::response::stock_reply(404, response);
        };
    }

    void InitialiseEndpoints()
    {
        multiplexer.handle(kEndpoint).get(HandleGet());
        multiplexer.handle(kEndpoint).put(HandlePut());
        multiplexer.handle(kEndpoint).post(HandlePost());
        multiplexer.handle(kEndpoint).del(HandleDelete());
    }

    void StartServer()
    {
        served::net::server server(kIpAddress, kPort, multiplexer);
        std::cout << "Starting server at \"" << kIpAddress
                  << "\" listening on port: " << kPort
                  << "..." << std::endl;
        server.run(kThreads);
    }

private:
    served::multiplexer multiplexer;
};

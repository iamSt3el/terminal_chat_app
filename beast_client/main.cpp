#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include "ui.h"
namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

// Global flag to signal when the user wants to exit
bool exitRequested = false;

void readMsg(websocket::stream<tcp::socket>& ws, UI& ui)
{
    beast::flat_buffer buffer;
    while (!exitRequested)
    {
        try
        {
            // Read a message into our buffer
            ws.read(buffer);

             // Process the message or do something with it
            std::string receivedMsg = boost::beast::buffers_to_string(buffer.data());

            // Process the message or do something with it
           // std::cout << "Received: " << beast::make_printable(buffer.data()) << std::endl;

            // Print the received message to the UI
            std::string sentMessage = "Recevied: " + receivedMsg;
            ui.printMessage(sentMessage);
            // Clear the buffer for the next message
            buffer.consume(buffer.size());
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error in readMsg: " << e.what() << std::endl;
            // Handle the error as needed, for example, you might want to break the loop
            break;
        }
    }
}

int main()
{
    try
    {
        UI ui;
        std::string host = "65.2.10.190";
        auto const port = "443";

        // The io_context is required for all I/O
        net::io_context ioc;

        // These objects perform our I/O
        tcp::resolver resolver{ioc};
        websocket::stream<tcp::socket> ws{ioc};

        // Look up the domain name
        auto const results = resolver.resolve(host, port);

        // Make the connection on the IP address we get from a lookup
        auto ep = net::connect(ws.next_layer(), results);

        // Perform the websocket handshake
        ws.handshake(host, "/");

        // Start a new thread to continuously read messages
        std::thread readerThread(readMsg, std::ref(ws), std::ref(ui));

        ui.initUI();
        // User input loop
        while (true)
        {
            // Simulate sending a message
            mvwprintw(ui.inputWindow, 1, 1, "You: ");
            wrefresh(ui.inputWindow);
            std::string text = ui.getUserInput();
            if (text == "exit")
            {
                exitRequested = true;
                // Gracefully close the WebSocket
                ws.close(websocket::close_code::normal);
                ui.closeUI();
                break;
            }
            else
            {
                ws.write(net::buffer(text));
                std::string sentMessage = "You: " + text;
                ui.printMessage(sentMessage);
            }
        }

        // Join the reader thread to wait for it to finish
        readerThread.join();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
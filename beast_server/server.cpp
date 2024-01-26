#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <mutex>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;


class EchoWebsocket : public std::enable_shared_from_this<EchoWebsocket>
{
	websocket::stream<beast::tcp_stream> ws;
	beast::flat_buffer buffer;
	std::vector<std::shared_ptr<EchoWebsocket>>& connections;
	std::mutex connectionsMutex;

public:
	EchoWebsocket(tcp::socket&& socket, std::vector<std::shared_ptr<EchoWebsocket>>& connections)
		:ws(std::move(socket)), connections(connections) {}

	// Method to initiate close operation
	void close()
	{
		// Check if the connection is already closed
		if (ws.next_layer().socket().is_open())
		{
			ws.async_close(websocket::close_code::going_away, [self{ shared_from_this() }](beast::error_code ec) {
				if (ec)
				{
					std::cerr << "Error during WebSocket closure: " << ec.message() << std::endl;
				}
				});
		}
	}






	~EchoWebsocket()
	{
		std::cout << "Connection closed. Total connections: " << connections.size() << std::endl;
	}




	void run()
	{
		ws.async_accept([self{ shared_from_this() }](beast::error_code ec) {
			if (ec) { std::cout << ec.message() << std::endl; return; }

			self->echo();

			});
	}


	void echo()
	{

		auto shared_buffer = std::make_shared<beast::flat_buffer>();
		ws.async_read(*shared_buffer, [self{ shared_from_this() }, shared_buffer](beast::error_code ec,
			std::size_t bytes_transferred)
			{
				if (ec == websocket::error::closed)
				{
					//Remove this connection form the list when closed
					self->connections.erase(std::remove(self->connections.begin(), self->connections.end(), self), self->connections.end());
					std::cout << "Connection closed" << std::endl;
					return;
				}
				if (ec) { std::cout << ec.message() << std::endl; return; }

				auto out = beast::buffers_to_string(shared_buffer->cdata());
				std::cout << "Received: " << out << std::endl;
				std::cout << "Connection accepted. Total connections: " << self->connections.size() << std::endl;



				//echo to all connected clients
				for (const auto& connection : self->connections)
				{
					if (connection != self)
						connection->send(out);

				}
				self->buffer.consume(self->buffer.size());
				self->echo();

			});
	}

	void send(const std::string& message)
	{
		if (ws.is_open())
		{
			ws.text(ws.got_text());
			ws.write(net::buffer(message));
		}

	}
};


class Listener : public std::enable_shared_from_this<Listener>
{
	net::io_context& ioc;
	tcp::acceptor acceptor;
	std::vector<std::shared_ptr<EchoWebsocket>> connections;

public:
	Listener(net::io_context& ioc,
		unsigned short int port) :ioc(ioc),
		acceptor(ioc, { net::ip::make_address("0.0.0.0"), (port) }) {}

	void asyncAccept()
	{
		acceptor.async_accept(ioc, [self{ shared_from_this() }](boost::system::error_code ec,
			tcp::socket socket) {

				auto echo = std::make_shared<EchoWebsocket>(std::move(socket), self->connections);
				self->connections.push_back(echo);
				echo->run();
				std::cout << "Connection accepted" << std::endl;
				std::cout << "Connection accepted. Total connections: " << self->connections.size() << std::endl;

				self->asyncAccept();
			});
	}
};



int main()
{
	auto const port = 443;
	net::io_context ioc{};


	auto listener = std::make_shared<Listener>(ioc, port);
	listener->asyncAccept();


	ioc.run();



	return 0;
}

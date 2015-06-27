#ifndef CPS_SOCKET_HPP_
#define CPS_SOCKET_HPP_

#include <string>
#include <vector>

#include "Exception.hpp"
#include "Utils.hpp"

#include "boost/bind.hpp"
#include "boost/lambda/lambda.hpp"

#ifndef USE_HEADER_ONLY_ASIO
#include "boost/asio.hpp"
using namespace boost;
#else
#define ASIO_DISABLE_THREADS // To disable linking errors
#include "asio.hpp"
#endif

using namespace std;

namespace CPS
{
class AbstractSocket
{
public:
	AbstractSocket(asio::io_service &io_service) :
		io_service(io_service), deadline(io_service), connected(false) {
		connectTimeout = 5;
		sendTimeout = 30;
		recieveTimeout = 60;
		// No deadline is required until the first socket operation is started. We
		// set the deadline to positive infinity so that the actor takes no action
		// until a specific deadline is set.
		deadline.expires_at(boost::posix_time::pos_infin);

		// Start the persistent actor that checks for deadline expiry.
		check_deadline();
	}
	virtual ~AbstractSocket() {
	}

	virtual void connect(const string &host, int port) = 0;

	virtual vector<unsigned char> send(const string &data) = 0;
	virtual vector<unsigned char> read() = 0;

	bool isConnected() {
		return connected;
	}

	void check_deadline() {
		// Check whether the deadline has passed. We compare the deadline against
		// the current time since a new asynchronous operation may have moved the
		// deadline before this actor had a chance to run.
		if (deadline.expires_at() <= asio::deadline_timer::traits_type::now())
		{
			// The deadline has passed. The socket is closed so that any outstanding
			// asynchronous operations are cancelled. This allows the blocked
			// connect(), read_line() or write_line() functions to return.
			error = asio::error::timed_out;
			handle_timer_expiration();

			// There is no longer an active deadline. The expire is set to positive
			// infinity so that the actor takes no action until a new deadline is set.
			deadline.expires_at(boost::posix_time::pos_infin);
		}

		// Put the actor back to sleep.
		deadline.async_wait(boost::bind(&AbstractSocket::check_deadline, this));
	}

	virtual void handle_timer_expiration() {
		// Nothing to do here.. Should be overloaded in child classes
	}

	int connectTimeout;
	int sendTimeout;
	int recieveTimeout;

protected:
	asio::io_service &io_service;
	asio::deadline_timer deadline;
#ifndef USE_HEADER_ONLY_ASIO
	boost::system::error_code error;
#else
	asio::error_code error;
#endif
	bool connected;
};

class TcpSocket: public AbstractSocket
{
public:
	TcpSocket(asio::io_service &io_service) :
		AbstractSocket(io_service), socket(io_service) {
	}
	virtual ~TcpSocket() {
		socket.close();
		connected = false;
	}

	virtual void connect(const string &host, int port) {
		// Set a deadline for the asynchronous operation.
		deadline.expires_from_now(boost::posix_time::seconds(connectTimeout));

		// Set up the variable that receives the result of the asynchronous
		// operation. The error code is set to would_block to signal that the
		// operation is incomplete. Asio guarantees that its asynchronous
		// operations will never fail with would_block, so any other value in
		// ec indicates completion.
		error = asio::error::would_block;

		asio::ip::tcp::resolver resolver(io_service);
		asio::ip::tcp::resolver::query query(host, Utils::toString(port));
		endpoint_iterator = resolver.resolve(query);

		// Start the asynchronous operation itself. The boost::lambda function
		// object is used as a callback and will update the ec variable when the
		// operation completes.
		socket.async_connect(*endpoint_iterator, boost::lambda::var(error) = boost::lambda::_1);

		// Block until the asynchronous operation has completed.
		do io_service.run_one(); while (error == asio::error::would_block);

		// Determine whether a connection was successfully established. The
		// deadline actor may have had a chance to run and close our socket, even
		// though the connect operation notionally succeeded. Therefore we must
		// check whether the socket is still open before deciding if we succeeded
		// or failed.
		if (error || !socket.is_open()) {
			throw CPS::Exception("Could not connect. " + error.message());
		}
		connected = true;
	}

	virtual vector<unsigned char> send(const string &data) {
		// Set a deadline for the asynchronous operation.
		deadline.expires_from_now(boost::posix_time::seconds(sendTimeout));

		// Set up the variable that receives the result of the asynchronous
		// operation. The error code is set to would_block to signal that the
		// operation is incomplete. Asio guarantees that its asynchronous
		// operations will never fail with would_block, so any other value in
		// ec indicates completion.
		error = asio::error::would_block;

		// Start the asynchronous operation itself. The boost::lambda function
		// object is used as a callback and will update the ec variable when the
		// operation completes.
		asio::async_write(socket, asio::buffer(data, data.size()), boost::lambda::var(error) = boost::lambda::_1);

		// Block until the asynchronous operation has completed.
		do io_service.run_one(); while (error == asio::error::would_block);

		if (error || !socket.is_open()) {
			throw CPS::Exception("Could not send message. " + error.message());
		}
		return read();
	}

	virtual vector<unsigned char> read() {
		// Set a deadline for the asynchronous operation.
		deadline.expires_from_now(boost::posix_time::seconds(recieveTimeout));

		// Set up the variable that receives the result of the asynchronous
		// operation. The error code is set to would_block to signal that the
		// operation is incomplete. Asio guarantees that its asynchronous
		// operations will never fail with would_block, so any other value in
		// ec indicates completion.
		error = asio::error::would_block;

		std::vector<unsigned char> reply(8);
		size_t len = 0, content_len = 0;
		socket.async_read_some(asio::buffer(reply),
				(boost::lambda::var(error) = boost::lambda::_1, boost::lambda::var(len) = boost::lambda::_2));

		// Block until the asynchronous operation has completed.
		do io_service.run_one(); while (error == asio::error::would_block);

		if (error || !socket.is_open() || len != 8
				|| !(reply[0] == 0x09 && reply[1] == 0x09 && reply[2] == 0x00
						&& reply[3] == 0x00)) {
			throw CPS::Exception("Invalid header received. " + error.message());
		}
		content_len = (reply[4]) | (reply[5] << 8) | (reply[6] << 16) | (reply[7] << 24);

		// Read rest of message
		error = asio::error::would_block;
		reply.clear();
		reply.resize(content_len);
		asio::async_read(socket, asio::buffer(reply),
				(boost::lambda::var(error) = boost::lambda::_1, boost::lambda::var(len) = boost::lambda::_2));

		// Block until the asynchronous operation has completed.
		do io_service.run_one(); while (error == asio::error::would_block);

		if (error || !socket.is_open() || len != content_len) {
			throw CPS::Exception("Could not read message. " + error.message());
		}
		return reply;
	}

	void handle_timer_expiration() {
		socket.close();
		connected = false;
	}
protected:
	asio::ip::tcp::socket socket;
	asio::ip::tcp::resolver::iterator endpoint_iterator;
};

#ifdef BOOST_ASIO_HAS_LOCAL_SOCKETS
class UnixSocket: public AbstractSocket
{
public:
	UnixSocket(asio::io_service &io_service) :
		AbstractSocket(io_service), socket(io_service) {
	}
	virtual ~UnixSocket() {
		socket.close();
	}

	virtual void connect(const string &host, int port) {
		asio::local::stream_protocol::endpoint ep(host);

		// Set a deadline for the asynchronous operation.
		deadline.expires_from_now(boost::posix_time::seconds(connectTimeout));

		// Set up the variable that receives the result of the asynchronous
		// operation. The error code is set to would_block to signal that the
		// operation is incomplete. Asio guarantees that its asynchronous
		// operations will never fail with would_block, so any other value in
		// ec indicates completion.
		error = asio::error::would_block;

		// Start the asynchronous operation itself. The boost::lambda function
		// object is used as a callback and will update the ec variable when the
		// operation completes.
		socket.async_connect(ep, boost::lambda::var(error) = boost::lambda::_1);

		// Block until the asynchronous operation has completed.
		do io_service.run_one(); while (error == asio::error::would_block);

		// Determine whether a connection was successfully established. The
		// deadline actor may have had a chance to run and close our socket, even
		// though the connect operation notionally succeeded. Therefore we must
		// check whether the socket is still open before deciding if we succeeded
		// or failed.
		if (error || !socket.is_open()) {
			throw CPS::Exception("Could not connect. " + error.message());
		}
		connected = true;
	}

	virtual vector<unsigned char> send(const string &data) {
		// Set a deadline for the asynchronous operation.
		deadline.expires_from_now(boost::posix_time::seconds(sendTimeout));

		// Set up the variable that receives the result of the asynchronous
		// operation. The error code is set to would_block to signal that the
		// operation is incomplete. Asio guarantees that its asynchronous
		// operations will never fail with would_block, so any other value in
		// ec indicates completion.
		error = asio::error::would_block;

		// Start the asynchronous operation itself. The boost::lambda function
		// object is used as a callback and will update the ec variable when the
		// operation completes.
		asio::async_write(socket, asio::buffer(data, data.size()), boost::lambda::var(error) = boost::lambda::_1);

		// Block until the asynchronous operation has completed.
		do io_service.run_one(); while (error == asio::error::would_block);

		if (error || !socket.is_open()) {
			throw CPS::Exception("Could not send message. " + error.message());
		}
		return read();
	}

	virtual vector<unsigned char> read() {
		// Set a deadline for the asynchronous operation.
		deadline.expires_from_now(boost::posix_time::seconds(recieveTimeout));

		// Set up the variable that receives the result of the asynchronous
		// operation. The error code is set to would_block to signal that the
		// operation is incomplete. Asio guarantees that its asynchronous
		// operations will never fail with would_block, so any other value in
		// ec indicates completion.
		error = asio::error::would_block;

		std::vector<unsigned char> reply(8);
		size_t len = 0, content_len = 0;
		socket.async_read_some(asio::buffer(reply),
				(boost::lambda::var(error) = boost::lambda::_1, boost::lambda::var(len) = boost::lambda::_2));

		// Block until the asynchronous operation has completed.
		do io_service.run_one(); while (error == asio::error::would_block);

		if (error || !socket.is_open() || len != 8
				|| !(reply[0] == 0x09 && reply[1] == 0x09 && reply[2] == 0x00
						&& reply[3] == 0x00)) {
			throw CPS::Exception("Invalid header received. " + error.message());
		}
		content_len = (reply[4]) | (reply[5] << 8) | (reply[6] << 16) | (reply[7] << 24);

		// Read rest of message
		error = asio::error::would_block;
		reply.clear();
		reply.resize(content_len);
		asio::async_read(socket, asio::buffer(reply),
				(boost::lambda::var(error) = boost::lambda::_1, boost::lambda::var(len) = boost::lambda::_2));

		// Block until the asynchronous operation has completed.
		do io_service.run_one(); while (error == asio::error::would_block);

		if (error || !socket.is_open() || len != content_len) {
			throw CPS::Exception("Could not read message. " + error.message());
		}
		return reply;
	}

	void handle_timer_expiration() {
		socket.close(error);
		connected = false;
	}
protected:
	asio::local::stream_protocol::socket socket;
};
#endif

class HttpSocket: public TcpSocket {
public:
	HttpSocket(asio::io_service &io_service, const std::string &host, int port, const std::string &path) :
		TcpSocket(io_service) {
		this->host = host;
		this->port = port;
		this->path = path;
	}
	virtual ~HttpSocket() {
	}

	virtual void connect(const string &host, int port) {
		deadline.expires_from_now(boost::posix_time::seconds(connectTimeout));
		error = asio::error::would_block;
		asio::ip::tcp::resolver resolver(io_service);
		asio::ip::tcp::resolver::query query(host, Utils::toString(port));
		endpoint_iterator = resolver.resolve(query);

		socket.async_connect(*endpoint_iterator, boost::lambda::var(error) = boost::lambda::_1);

		do io_service.run_one(); while (error == asio::error::would_block);

		if (error || !socket.is_open()) {
			throw CPS::Exception("Could not connect. " + error.message());
		}
		connected = true;
	}

	virtual vector<unsigned char> send(const string &data) {
		deadline.expires_from_now(boost::posix_time::seconds(sendTimeout));
		error = asio::error::would_block;
		// Create post headers
		string header = "";
		header += "POST " + path + " HTTP/1.0\r\n";
		header += "Host: " + host + ":" + Utils::toString(port) + "\r\n";
		header += "Content-Length: " + Utils::toString(data.size()) + "\r\n";
		header += "Connection: close\r\n";
		header += "\r\n";

		// Send headers
		asio::async_write(socket, asio::buffer(header, header.size()), boost::lambda::var(error) = boost::lambda::_1);
		// Send data
		asio::async_write(socket, asio::buffer(data, data.size()), boost::lambda::var(error) = boost::lambda::_1);

		do io_service.run_one(); while (error == asio::error::would_block);

		if (error || !socket.is_open()) {
			throw CPS::Exception("Could not send message. " + error.message());
		}
		return this->read();
	}

	virtual vector<unsigned char> read() {
		asio::streambuf response;
#ifndef USE_HEADER_ONLY_ASIO
		boost::system::error_code err;
#else
		asio::error_code err;
#endif
		vector <unsigned char> reply;
		int read = 0;
		bool past_header = false;
		while ((read = asio::read(socket, response, asio::transfer_at_least(1), err)) != 0) {
			char cbuf[response.size() + 1];
			int rc = response.sgetn(cbuf, sizeof(cbuf));
			if (past_header == false) {
				char *data;
				if ((data = strstr(cbuf, "\r\n\r\n")) != NULL) {
					reply.insert(reply.end(), data + 4, cbuf + rc);
					past_header = true;
				}
			} else {
				reply.insert(reply.end(), cbuf, cbuf + rc);
			}
		}
		if (err != asio::error::eof)
			BOOST_THROW_EXCEPTION(Exception("Problem reading from socket"));
		return reply;
	}

public:
	string host;
	int port;
	string path;

protected:
	asio::ip::tcp::resolver::iterator endpoint_iterator;
};
}

#endif //#ifndef CPS_SOCKET_HPP_

#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/bind/bind.hpp>

class Server {
   public:
    using ssl_socket = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;

    Server(unsigned short port)
        : m_ssl(boost::asio::ssl::context::sslv23),
          m_ios(),
          m_acceptor(m_ios,
                     boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),
                                                    port)) {
        memset(m_buffer, 0, MAX_LENGTH);
        m_ssl.set_options(boost::asio::ssl::context::default_workarounds |
                          boost::asio::ssl::context::no_sslv3 |
                          boost::asio::ssl::context::single_dh_use);
        m_ssl.use_certificate_chain_file("server.crt");
        m_ssl.use_private_key_file("server.key",
                                   boost::asio::ssl::context::pem);
        m_ssl.use_tmp_dh_file("dh4096.pem");
    }

    unsigned long run() { return m_ios.run(); }

    void add_accept() {
        ssl_socket* socket = nullptr;
        socket = new ssl_socket(m_ios, m_ssl);
        m_acceptor.async_accept(
            socket->lowest_layer(),
            boost::bind(&Server::accept_handler, this,
                        boost::asio::placeholders::error, socket));
    }

    void accept_handler(const boost::system::error_code& error,
                        ssl_socket* socket) {
        socket->async_handshake(
            boost::asio::ssl::stream_base::server,
            boost::bind(&Server::handle_handshake, this, socket,
                        boost::asio::placeholders::error));
    }

    void handle_handshake(ssl_socket* socket,
                          const boost::system::error_code& error) {
        if (!error) {
            memset(m_buffer, 0, MAX_LENGTH);
            socket->async_read_some(
                boost::asio::buffer(m_buffer, MAX_LENGTH),
                boost::bind(&Server::handle_read, this, socket,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
        }
    }

    void handle_read(ssl_socket* socket,
                     const boost::system::error_code& error,
                     size_t bytes_transferred) {
        if (!error) {
            std::string http_rep = "HTTP/1.1 200 OK\r\n";
            http_rep += "Server: korys\r\n";
            http_rep += "Content-type: text/html\r\n\r\n";
            http_rep += m_buffer;
            memcpy(m_buffer, http_rep.c_str(), http_rep.size());
            socket->async_write_some(
                boost::asio::buffer(m_buffer, http_rep.size()),
                boost::bind(&Server::handle_write, this, socket,
                            boost::asio::placeholders::error));
        }
    }

    void handle_write(ssl_socket* socket,
                      const boost::system::error_code& error) {
        delete socket;
    }

   private:
    boost::asio::ssl::context m_ssl;
    boost::asio::io_service m_ios;
    boost::asio::ip::tcp::acceptor m_acceptor;
    enum { MAX_LENGTH = 1024 };
    char m_buffer[MAX_LENGTH];
};

int main() {
    Server server(443);
    server.add_accept();
    printf("Hello World!\n      --from korys\n");
    return 0;
}
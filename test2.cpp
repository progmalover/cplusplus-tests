    #include <string.h>
    #include <boost/asio.hpp>
    #include <boost/bind.hpp>
    #include <boost/shared_ptr.hpp>
    #include <boost/enable_shared_from_this.hpp>

    using boost::asio::ip::tcp;
    using boost::asio::ip::address;

    class Session: public boost::enable_shared_from_this<Session> {
    public:
        Session(boost::asio::io_service &io_service): socket_(io_service)
        {
        }

        void start() {
            static tcp::no_delay option(true);
            socket_.set_option(option);

            boost::asio::async_read_until(socket_,
                    sbuf_,
                    "\n",
                    boost::bind(&Session::handle_read,
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
        }

        tcp::socket &socket() {
            return socket_;
        }

    private:
        void handle_write(const boost::system::error_code& error, size_t bytes_transferred) {
            boost::asio::async_read_until(socket_,
                    sbuf_,
                    "\n",
                    boost::bind(&Session::handle_read,
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
        }

        void handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
            boost::asio::async_write(socket_,
                    sbuf_,
                    boost::bind(&Session::handle_write,
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
        }

    private:
        tcp::socket socket_;
        boost::asio::streambuf sbuf_;
    };

    typedef boost::shared_ptr<Session> Session_ptr;

    class server {
    public:
            server(boost::asio::io_service &io_service, tcp::endpoint &endpoint)
                : io_service_(io_service), acceptor_(io_service, endpoint)
            {
                Session_ptr new_Session(new Session(io_service_));
                acceptor_.async_accept(new_Session->socket(),
                    boost::bind(&server::handle_accept,
                        this,
                        new_Session,
                        boost::asio::placeholders::error));
            }

            void handle_accept(Session_ptr new_Session, const boost::system::error_code& error) {
                if (error) {
                    return;
                }

                new_Session->start();
                new_Session.reset(new Session(io_service_));
                acceptor_.async_accept(new_Session->socket(),
                    boost::bind(&server::handle_accept,
                        this,
                        new_Session,
                        boost::asio::placeholders::error));
            }

            void run() {
                io_service_.run();
            }

    private:
            boost::asio::io_service &io_service_;
            tcp::acceptor acceptor_;
    };

    int main(int argc, char* argv[])
    {
        boost::asio::io_service io_service;
        tcp::endpoint endpoint(address::from_string("127.0.0.1"), 10028);

        server s(io_service, endpoint);
        s.run();
        return 0;
    }

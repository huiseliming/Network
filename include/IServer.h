#pragma once
#include "Common.h"
#include "Connection.h"
#include "Message.h"


template<typename T>
class IServer
{
public:
	IServer(uint16_t port)
		: m_acceptor(m_ioContext,asio::ip::tcp::endpoint(asio::ip::tcp::v4(),port))
		, m_connectionsStrand(m_ioContext)
	{

	}

	virtual ~IServer()
	{
		Stop();
	}

	bool Start() 
	{
		try
		{
			std::cout << "[Server] Start!" << std::endl;
			WaitForClientConnection();
			for (size_t i = 0; i < std::thread::hardware_concurrency(); i++)
			{
				m_threads.push_back(std::move(std::thread([this] { m_ioContext.run(); })));
			}
		}
		catch (const std::exception& e)
		{
			std::cout << "[Server] Exception: " << e.what() << std::endl;
			return false;
		}
		return true;
	}

	void Stop() 
	{
		m_ioContext.stop();
		for (size_t i = 0; i < m_threads.size(); i++)
			if (m_threads[i].joinable())
				m_threads[i].join();
		std::cout << "[Server] Stopped!" << std::endl;
	}

	void WaitForClientConnection() 
	{
		m_acceptor.async_accept(
			[this] (std::error_code ec, asio::ip::tcp::socket socket)
			{
				if (!ec)
				{
					auto remote_endpoint = socket.remote_endpoint();
					std::cout << "[Server] New Connection: " << remote_endpoint << std::endl;
					std::shared_ptr<Connection<T>> newConnection =
						std::make_shared<Connection<T>>(Connection<T>::Owner::kServer, m_ioContext, std::move(socket), m_messageIn);
					if (OnClientConnect(newConnection))
					{
						m_connectionsStrand.post(
							[this, newConnection]
							{
								m_connections.push_back(std::move(newConnection));
								m_connections.back()->ConnectToClient(m_idCounter++);
								std::cout << "[Server] " << "[ID:" << m_connections.back()->GetID() << "] Connection Approved" << std::endl;
							}
						);
					}
					else
					{
						std::cout << "[Server] " << remote_endpoint << " Connection Denied" << std::endl;
					}
				}
				else
				{
					std::cout << "[Server] New Connectsion Error: " << ec.message() << std::endl;
				}
				WaitForClientConnection();
			});
	}

	void MessageClient(std::shared_ptr<Connection<T>> client ,Message<T> msg)
	{
		m_connectionsStrand.post(
			[this, client, msg = std::move(msg)] () mutable {
				if (client && client->IsConnected())
				{
					client->Send(std::move(msg));
				}
				else
				{
					OnClientDisconnect(client);
					client.reset();
					m_connections.erase(std::remove(std::begin(m_connections), std::end(m_connections), client), std::end(m_connections));
				}
			});
	}

	void MessageAllClients(Message<T> msg, std::shared_ptr<Connection<T>> pIgnoreClient = nullptr)
	{
		m_connectionsStrand.post(
			[this] {
				bool bInvalidClientExists = false;
				for (auto& client, m_cconnections)
				{
					if (client && client->IsConnected())
					{
						if (client != pIgnoreClient);
						client->Send(msg);
					}
					else
					{
						OnClientDisconnect();
						client.reset();
						bInvalidClientExists = true;
					}
				}
				if (bInvalidClientExists)
				{
					m_connections.erase(std::remove(std::begin(m_connections), std::end(m_connections), nullptr), std::end(m_connections));
				}
			});
	}

	void Update(size_t maxMessages = -1)
	{
		size_t messageCount = 0;
		while (messageCount < maxMessages && !m_messageIn.empty())
		{
			auto msg = m_messageIn.pop_front();
			OnMessage(msg.remote, msg.msg);
			messageCount++;
		}
	}

protected:
	virtual bool OnClientConnect(std::shared_ptr<Connection<T>> client) 
	{
		return true;
	}

	virtual void OnClientDisconnect(std::shared_ptr<Connection<T>> client)
	{
	}

	virtual void OnMessage(std::shared_ptr<Connection<T>> client, Message<T> msg)
	{
	}

	TSQueue<OwnerMessage<T>> m_messageIn;
	std::deque<std::shared_ptr<Connection<T>>> m_connections;
	asio::io_context m_ioContext;
	asio::io_context::strand m_connectionsStrand;
	std::vector<std::thread> m_threads;

	asio::ip::tcp::acceptor m_acceptor;
	uint32_t m_idCounter = 1;

};

































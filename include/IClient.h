#pragma once
#include <memory>
#include "Common.h"
#include "Connection.h"


template<typename T>
class IClient
{
public:

	IClient()
	{
	}

	virtual ~IClient() 
	{
	}

	bool Connect(const std::string host, const uint16_t port) 
	{
		try
		{
			asio::ip::tcp::resolver resolver(m_ioContext);
			auto endpoints = resolver.resolve(host,std::to_string(port));
			m_connection = std::make_unique<Connection<T>>(Connection<T>::Owner::kClient, m_ioContext, asio::ip::tcp::socket(m_ioContext), m_messageIn);
			m_connection->ConnectToServer(endpoints);
			m_thread = std::move(std::thread([this] { m_ioContext.run(); }));
		}
		catch (const std::exception& e)
		{
			std::cout << "[Client] Exception: " << e.what() << std::endl;
			return false;
		}
		return true;
	}

	void Disconnect() 
	{
		if (IsConnected())
			m_connection->DisConnect();
	}

	bool IsConnected() 
	{
		if (m_connection)
			return m_connection->IsConnected();
		else
			return false;
	}

	void MessageServer(Message<T>&& msg) 
	{ 
		if (m_connection->IsConnected()) 
			m_connection->Send(std::forward<Message<T>>(msg)); 
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

	virtual void OnMessage(std::shared_ptr<Connection<T>> client, Message<T> msg)
	{

	}



protected:
	asio::io_context m_ioContext;
	std::thread m_thread;

	std::unique_ptr<Connection<T>> m_connection;

private:
	TSQueue<OwnerMessage<T>> m_messageIn;
};



























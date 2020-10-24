#pragma once
#include <vector>
#include <iostream>

using MessageBody = std::vector<uint8_t>;

template<typename T>
struct MessageHeader
{
public:
    T id{};
    uint32_t size = 0;
};

template<typename T>
struct Message
{
public:
    MessageHeader<T> header;
    MessageBody body;
    size_t size() const 
    {
        return sizeof(MessageHeader<T>) + body.size();
    }

    friend std::ostream& operator<<(std::ostream& os, const Message<T>& msg)
    {
        os << "MessageId  : " << static_cast<int>(msg.header.id) <<std::endl 
           << "MessageSize: "<< msg.header.size << std::endl;
        return os;
    }

    template<typename DataType>
    friend Message<T>& operator<<(Message<T>& msg,const DataType& data)
    {
        static_assert(std::is_standard_layout<DataType>::value, "DataType must is standard layout");
        size_t i = msg.body.size();
        msg.body.resize(msg.body.size() + sizeof(DataType));
        std::memcpy(msg.body.data() + i, &data, sizeof(DataType));
        msg.header.size = static_cast<uint32_t>(msg.body.size());
        return msg;
    }

    template<typename DataType>
    friend Message<T>& operator>>(Message<T>& msg, DataType& data)
    {
        static_assert(std::is_standard_layout<DataType>::value, "DataType must is standard layout");
        size_t size = msg.body.size();
        size_t dataOffset = size - sizeof(DataType);
        std::memcpy(&data, msg.body.data() + dataOffset, sizeof(DataType));
        msg.body.resize(msg.body.size() - sizeof(DataType));
        msg.header.size = static_cast<uint32_t>(msg.body.size());
        return msg;
    }

    friend Message<T>& operator<<(Message<T>& msg,const std::string& data)
    {
        size_t i = msg.body.size();
        uint32_t stringLen = static_cast<uint32_t>(data.size());
        msg.body.resize(msg.body.size() + sizeof(uint32_t) + data.size());
        std::memcpy(msg.body.data() + i, data.data(), data.size());
        std::memcpy(msg.body.data() + i + data.size(), &stringLen, sizeof(uint32_t));
        msg.header.size = static_cast<uint32_t>(msg.body.size());
        return msg;
    }

    friend Message<T>& operator>>(Message<T>& msg, std::string& data)
    {
        size_t size = msg.body.size();
        size_t dataOffset = size - sizeof(uint32_t);
        size_t stringLen = 0;
        std::memcpy(&stringLen, msg.body.data() + dataOffset, sizeof(uint32_t));
        data.resize(stringLen);
        dataOffset = size - sizeof(uint32_t) - stringLen;
        std::memcpy(data.data(), msg.body.data() + dataOffset, stringLen);
        msg.body.resize(msg.body.size() - sizeof(uint32_t) + stringLen);
        msg.header.size = static_cast<uint32_t>(msg.body.size());
        return msg;
    }
};

template<typename T>
class Connection;

template<typename T>
struct OwnerMessage 
{
    std::shared_ptr<Connection<T>> remote = nullptr;
    Message<T> msg;
    friend std::ostream& operator<<(std::ostream& os, const OwnerMessage<T>& msg)
    {
        os << msg.msg;
        return os; 
    }
};










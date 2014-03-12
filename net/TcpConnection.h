﻿#ifndef _THEFOX_NET_TCPCONNECTION_H_
#define _THEFOX_NET_TCPCONNECTION_H_

#include <map>
#include <Winsock2.h>
#include <base/Types.h>
#include <base/noncopyable.h>
#include <net/Buffer.h>
#include <net/InetAddress.h>
#include <net/Callbacks.h>
#include <base/MutexLock.h>

namespace thefox
{
namespace net
{

class Iocp;
class Socket;
class IoContext;

class TcpConnection : noncopyable
{
public:
    TcpConnection(Iocp *iocp,
                  SOCKET s, 
                  const String &name, 
                  const InetAddress &localAddr,
                  const InetAddress &peerAddr);
    ~TcpConnection(void);

    void send(const char *data, size_t len);
    void send(const String &data);
    void setConnectionCallback(const ConnectionCallback &cb)
    { _connectionCallback = cb; }
    void setCloseCallback(const CloseCallback &cb)
    { _closeCallback = cb; }
    void setMessageCallback(const MessageCallback &cb)
    { _messageCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb)
    { _writeCompleteCallback = cb; }

    /// @brief connection accept complete
    void connectEstablished();
    void handleEvent(IoContext *io);
    void handleClose();
private:
    enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
    void setState(StateE s) { _state = s; }
    void asyncRead(IoContext *buf = NULL);
    void asyncWrite(IoContext *buf = NULL);
    void sendZeroByteRead(IoContext *buf = NULL);
    void handleRead(IoContext *io);
    void handleReadComplete(IoContext *io);
    void handleWrite(IoContext *io);
    void handleWriteComplete(IoContext *io);
    void handleZeroByteRead(IoContext *io);
    void handleZeroByteReadComplete(IoContext *io);

    const String _name;
    StateE _state;
    Socket *_socket;
    Iocp *_iocp;
    InetAddress _localAddr;
    InetAddress _peerAddr;
    Buffer _readBuffer;
    Buffer _sendBuffer;
    MutexLock _lock;
    uint32_t _numberOfPendlingIO; // 表示还有多少IoContext在等待
    std::map<uint32_t, IoContext *> _readIoContexts;
    std::map<uint32_t, IoContext *> _sendIoContexts;
    ConnectionCallback _connectionCallback;
    CloseCallback _closeCallback;
    MessageCallback _messageCallback;
    WriteCompleteCallback _writeCompleteCallback;
};

} // namespace net
} // namespace thefox

#endif // _THEFOX_NET_TCPCONNECTION_H_
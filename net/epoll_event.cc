#include <net/epoll_event.h>
#include <sys/epoll.h>
#include <log/logging.h>
#include <net/buffer.h>
#include <net/tcp_connection.h>

using namespace thefox;
using namespace thefox::net;

EpollEvent::EpollEvent()
	: _epollfd(-1)
	, _events(32)
{
	THEFOX_TRACE_FUNCTION;
}

EpollEvent::~EpollEvent()
{
	THEFOX_TRACE_FUNCTION;

	if(-1 == ::close(_epollfd))
		THEFOX_LOG(ERROR) << "epoll close() failed";
	_epollfd = -1;
	_events.clear();
}

bool EpollEvent::init()
{
	THEFOX_TRACE_FUNCTION;

	if (-1 == _epollfd) {
		_epollfd = ::epoll_create1(EPOLL_CLOEXEC);
		
		if (-1 == _epollfd {
			THEFOX_LOG(ERROR) << "epoll_create() failed";
			return false;
		}
	}
	return true;
}

bool EpollEvent::addEvent(Event *ev)
{
	THEFOX_TRACE_FUNCTION;
	
	assert(NULL != ev);

	struct epoll_event event;
	event.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP;
	event.data.ptr = ev;

	THEFOX_LOG(DEBUG) << "epoll add connection: fd:" << ev->fd << "ev:" << event.events;

	if (-1 == ::epoll_ctl(_epollfd, EPOLL_CTL_ADD, ev->fd, &event)) {
		THEFOX_LOG(ERROR) << "epoll_ctl(EPOLL_CTL_ADD, " << ev->fd << ") failed";
		return false;
	}
	return true;
}

bool EpollEvent::delEvent(Event *ev)
{
	THEFOX_TRACE_FUNCTION;

	assert(NULL != ev);

	struct epoll_event event;

	THEFOX_LOG(DEBUG) << "epoll del connection: fd:" << ev->fd;

	event.events = 0;
	event.data.ptr = NULL;

	if (-1 == ::epoll_ctl(_epollfd, EPOLL_CTL_DEL, ev->fd, &event)) {
		THEFOX_LOG(ERROR) << "epoll_ctl(EPOLL_CTL_DEL, " << ev->fd << ") failed";
		return false;
	}
	return true;
}

bool EpollEvent::processEvents(uint32_t timer)
{
	THEFOX_TRACE_FUNCTION;

	int numEvents = ::epoll_wait(_epollfd, &*_events.begin(),
								static_cast<int>(_events,size()),
								timer);
	int err = (-1 == numEvents) ? errno : 0;

	if (err) {
		if (EINTR != err) {
			THEFOX_LOG(ERROR) << "errno: EINTR";
		}
		THEFOX_LOG(ERROR) << "epoll_wait() failed";
		return false;
	}

	if (0 == numEvents) {
		THEFOX_LOG(TRACE) << "epoll_wait() returned 0, timeout";
		return true;
	}
	
	// resize max num of wait events;
	if (implicit_cast<size_t>(numEvents) == _events.size())
		_events.resize(events.size() * 2);
	
	for (int i = 0; i < numEvents; ++i) {
		
		ev->handler(static_cast<Event *>(_events[i].data.ptr), _events[i].events);
	}
}

bool EpollEvent::updateRead(const TcpConnectionPtr &conn)
{
	THEFOX_TRACE_FUNCTION;

}

bool EpollEvent::updateWrite(const TcpConnectionPtr &conn)
{
	THEFOX_TRACE_FUNCTION;

	int ret;
	Buffer *buffer = conn->writeBuffer();

	while (buffer->readableBytes() > 0) {
		ret = write(conn->fd(), buffer->peek(), buffer->readableBytes());
		
		if (-1 == ret && EAGAIN != errno) {
			THEFOX_LOG(ERROR) << "write error";
			return false;
		}

		buffer->retrieve(ret);
	}

	return true;
}

void EpollEvent::handler(IoEvent *ev, uint32_t revents)
{
	THEFOX_TRACE_FUNCTION;

	if (revents & (EPOLLERR | EPOLLHUP)) {
		THEFOX_LOG(ERROR) << "error on		";
	}
	

}

bool EpollEvent::handleRead(const TcpConnectionPtr &conn)
{
	int ret;
	Buffer *buffer = conn->readBuffer();

	buffer->ensureWritableBytes(kDefaultBufferSize);
	while ((ret = read(conn->fd(), buffer->beginWrite(), kDefaultBufferSize)) > 0) {
		conn->addReadBytes(ret);
	}

	if (-1 == ret && EAGAIN != error) {
		THEFOX_LOG(ERROR) << "read error";
		return false;
	} // else 读完成
	return true;
}

bool EpollEvent::handleWrite(IoEvent *ev)
{
	THEFOX_TRACE_FUNCTION;

	int ret;
	TcpConnection *conn = ev->conn;
	Buffer *buffer = conn->writeBuffer();

	while (buffer->readableBytes() > 0) {
		ret = write(conn->fd(), buffer->peek(), buffer->readableBytes());
		if (-1 == ret && EAGAIN != errno) {
			THEFOX_LOG(ERROR) << "write error";
			return false;
		}

		buffer->retrieve(ret);
	}
	return true;
}

bool EpollEvent::handleClose(const TcpConnectionPtr &conn)
{


}

#include "network_interface.hpp"
#ifndef CODE_INLINE
#include "network_interface.ipp"
#endif

namespace KBEngine { 
namespace Mercury
{
const int NetworkInterface::RECV_BUFFER_SIZE = 16 * 1024 * 1024; // 16MB
const char * NetworkInterface::USE_KBEMACHINED = "kbemachined";

//-------------------------------------------------------------------------------------
NetworkInterface::NetworkInterface(Mercury::EventDispatcher * pMainDispatcher,
		NetworkInterfaceType networkInterfaceType,
		uint16 listeningPort, const char * listeningInterface) :
	socket_(),
	address_(Address::NONE),
	channelMap_(),
	isExternal_(networkInterfaceType == NETWORK_INTERFACE_EXTERNAL),
	isVerbose_(true),
	pDispatcher_(new EventDispatcher),
	pMainDispatcher_(NULL),
	pExtensionData_(NULL),
	pPacketReceiver_(NULL)
{
	pPacketReceiver_ = new PacketReceiver(socket_, *this);
	this->recreateListeningSocket(listeningPort, listeningInterface);

	if (pMainDispatcher != NULL)
	{
		this->attach(*pMainDispatcher);
	}
}

//-------------------------------------------------------------------------------------
NetworkInterface::~NetworkInterface()
{
	ChannelMap::iterator iter = channelMap_.begin();
	while (iter != channelMap_.end())
	{
		ChannelMap::iterator oldIter = iter++;
		Channel * pChannel = oldIter->second;

		if (pChannel->isOwnedByInterface())
		{
			pChannel->destroy();
		}
		else
		{
			WARNING_MSG("NetworkInterface::~NetworkInterface: "
					"Channel to %s is still registered\n",
				pChannel->c_str());
		}
	}

	this->detach();

	this->closeSocket();

	delete pDispatcher_;
	pDispatcher_ = NULL;
}

//-------------------------------------------------------------------------------------
void NetworkInterface::attach(EventDispatcher & mainDispatcher)
{
	KBE_ASSERT(pMainDispatcher_ == NULL);
	pMainDispatcher_ = &mainDispatcher;
	mainDispatcher.attach(this->dispatcher());
}

//-------------------------------------------------------------------------------------
void NetworkInterface::detach()
{
	if (pMainDispatcher_ != NULL)
	{
		pMainDispatcher_->detach(this->dispatcher());
		pMainDispatcher_ = NULL;
	}
}

//-------------------------------------------------------------------------------------
void NetworkInterface::closeSocket()
{
	if (socket_.good())
	{
		this->dispatcher().deregisterFileDescriptor(socket_);
		socket_.close();
		socket_.detach();
	}
}

//-------------------------------------------------------------------------------------
bool NetworkInterface::recreateListeningSocket(uint16 listeningPort,
	const char * listeningInterface)
{
	this->closeSocket();

	socket_.getInterfaceAddress(listeningInterface, address_.ip);
	address_.port = listeningPort;

	socket_.socket(SOCK_STREAM);
	if (!socket_.good())
	{
		ERROR_MSG("NetworkInterface::recreateListeningSocket: couldn't create a socket\n");
		return false;
	}

	this->dispatcher().registerFileDescriptor(socket_, pPacketReceiver_);

	if (socket_.bind(listeningPort, INADDR_ANY) != 0)
	{
		ERROR_MSG("NetworkInterface::recreateListeningSocket: "
				"Couldn't bind the socket to %s (%s)\n",
			address_.c_str(), strerror(errno));
		
		socket_.close();
		socket_.detach();
		return false;
	}

	INFO_MSG("NetworkInterface::recreateListeningSocket: address %s\n", address_.c_str());
	socket_.setnonblocking(true);
	socket_.setnodelay(true);
	
#ifdef KBE_SERVER
	if (!socket_.setBufferSize(SO_RCVBUF, RECV_BUFFER_SIZE))
	{
		WARNING_MSG("NetworkInterface::recreateListeningSocket: "
			"Operating with a receive buffer of only %d bytes (instead of %d)\n",
			socket_.getBufferSize(SO_RCVBUF), RECV_BUFFER_SIZE);
	}
#endif

	if(socket_.listen(5) == -1)
	{
		ERROR_MSG("NetworkInterface::recreateListeningSocket: "
			"listen to %s (%s)\n",
			address_.c_str(), strerror(errno));

		socket_.close();
		socket_.detach();
		return false;
	}
	
	return true;
}

void NetworkInterface::delayedSend(Channel & channel)
{
	//pDelayedChannels_->add(channel);
}

void NetworkInterface::handleTimeout(TimerHandle handle, void * arg)
{
	INFO_MSG("NetworkInterface::handleTimeout: address %s\n", address_.c_str());
}

Channel * NetworkInterface::findChannel(const Address & addr)
{
	if (addr.ip == 0)
	{
		return NULL;
	}

	ChannelMap::iterator iter = channelMap_.find(addr);
	Channel * pChannel = iter != channelMap_.end() ? iter->second : NULL;

	return pChannel;
}

void NetworkInterface::onChannelGone(Channel * pChannel)
{
}

void NetworkInterface::onChannelTimeOut(Channel * pChannel)
{
	ERROR_MSG("NetworkInterface::onChannelTimeOut: Channel %s timed out.\n",
			pChannel->c_str());
}

void NetworkInterface::send(const Address & address, Bundle & bundle, Channel * pChannel)
{
}

void NetworkInterface::sendPacket(const Address & address,
						Packet * pPacket,
						Channel * pChannel, bool isResend)
{
}

}
}
/*
This source file is part of KBEngine
For the latest info, see http://www.kbengine.org/

Copyright (c) 2008-2012 KBEngine.

KBEngine is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

KBEngine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.
 
You should have received a copy of the GNU Lesser General Public License
along with KBEngine.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "machine.hpp"
#include "machine_interface.hpp"
#include "network/common.hpp"
#include "network/tcp_packet.hpp"
#include "network/udp_packet.hpp"
#include "network/message_handler.hpp"
#include "network/bundle_broadcast.hpp"
#include "thread/threadpool.hpp"
#include "server/componentbridge.hpp"

namespace KBEngine{
	
ServerConfig g_serverConfig;
KBE_SINGLETON_INIT(Machine);

//-------------------------------------------------------------------------------------
Machine::Machine(Mercury::EventDispatcher& dispatcher, 
				 Mercury::NetworkInterface& ninterface, 
				 COMPONENT_TYPE componentType,
				 COMPONENT_ID componentID):
	ServerApp(dispatcher, ninterface, componentType, componentID),
	broadcastAddr_(0),
	ep_(),
	epBroadcast_(),
	epLocal_(),
	pEPPacketReceiver_(NULL),
	pEBPacketReceiver_(NULL),
	pEPLocalPacketReceiver_(NULL)

{
}

//-------------------------------------------------------------------------------------
Machine::~Machine()
{
	ep_.close();
	epBroadcast_.close();
	epLocal_.close();

	SAFE_RELEASE(pEPPacketReceiver_);
	SAFE_RELEASE(pEBPacketReceiver_);
	SAFE_RELEASE(pEPLocalPacketReceiver_);
}

//-------------------------------------------------------------------------------------
void Machine::onBroadcastInterface(Mercury::Channel* pChannel, int32 uid, std::string& username,
								   int8 componentType, uint64 componentID, 
									uint32 intaddr, uint16 intport,
									uint32 extaddr, uint16 extport)
{
	if(componentType == MACHINE_TYPE)
	{
		if(uid == getUserUID() && 
			intaddr == this->getNetworkInterface().intaddr().ip)
		{
			return;
		}
	}
	else if(componentType == DBMGR_TYPE) // 如果是dbmgr重启了则清空所有这个uid的记录。
	{
		const Components::ComponentInfos* pinfos = 
			Componentbridge::getComponents().findComponent(DBMGR_TYPE, uid, componentID);
		
		// 做一个容错处理， windows下没做处理， 广播包会受到多次， linux不会出现。
		// 将来会对windows进行下机制进行调整此处则不会出现不会NULL。
		if(pinfos == NULL)
		{
			INFO_MSG("Machine::onBroadcastInterface: /-----------------------------reset kbengine.-------------------------/\n");
			Componentbridge::getComponents().clear(uid);
			INFO_MSG("Machine::onBroadcastInterface: /-----------------------------end reset kbengine.---------------------/\n");
		}
		else
		{
			return;
		}
	}

	INFO_MSG("Machine::onBroadcastInterface: uid:%d, username:%s, componentType:%s, "
			"componentID:%"PRAppID", intaddr:%s, intport:%u, extaddr:%s, extport:%u.\n", 
		uid, username.c_str(), COMPONENT_NAME_EX((COMPONENT_TYPE)componentType), componentID, 
		inet_ntoa((struct in_addr&)intaddr), ntohs(intport),
		extaddr != 0 ? inet_ntoa((struct in_addr&)extaddr) : "nonsupport", ntohs(extport));

	Componentbridge::getComponents().addComponent(uid, username.c_str(), 
		(KBEngine::COMPONENT_TYPE)componentType, componentID, intaddr, intport, extaddr, extport);
}

//-------------------------------------------------------------------------------------
void Machine::onFindInterfaceAddr(Mercury::Channel* pChannel, int32 uid, std::string& username, int8 componentType, 
								  int8 findComponentType, uint32 finderAddr, uint16 finderRecvPort)
{
	INFO_MSG("Machine::onFindInterfaceAddr: uid:%d, username:%s, componentType:%s, "
		"find:%s, finderaddr:%s, finderRecvPort:%u.\n", 
		uid, username.c_str(), COMPONENT_NAME_EX((COMPONENT_TYPE)componentType),  COMPONENT_NAME_EX((COMPONENT_TYPE)findComponentType), 
		inet_ntoa((struct in_addr&)finderAddr), ntohs(finderRecvPort));

	Mercury::EndPoint ep;
	ep.socket(SOCK_DGRAM);

	if (!ep.good())
	{
		ERROR_MSG("Machine::onFindInterfaceAddr: Failed to create socket.\n");
		return;
	}
	
	const Components::ComponentInfos* pinfos = 
		Componentbridge::getComponents().findComponent((KBEngine::COMPONENT_TYPE)findComponentType, uid, 0);
	
	Mercury::Bundle bundle;

	if(pinfos == NULL)
	{
		WARNING_MSG("Machine::onFindInterfaceAddr: %s not found %s.\n", COMPONENT_NAME_EX((COMPONENT_TYPE)componentType), 
			COMPONENT_NAME_EX((COMPONENT_TYPE)findComponentType));

		MachineInterface::onBroadcastInterfaceArgs8::staticAddToBundle(bundle, 0, 
			"", UNKNOWN_COMPONENT_TYPE, 0, 0, 0, 0, 0);
	}
	else
		MachineInterface::onBroadcastInterfaceArgs8::staticAddToBundle(bundle, pinfos->uid, 
			pinfos->username, findComponentType, pinfos->cid, pinfos->pIntAddr->ip, pinfos->pIntAddr->port,
			pinfos->pExtAddr->ip, pinfos->pExtAddr->port);
	
	if(finderAddr != 0 && finderRecvPort != 0)
		bundle.sendto(ep, finderRecvPort, finderAddr);
	else
		bundle.send(this->getNetworkInterface(), pChannel);
}

//-------------------------------------------------------------------------------------
bool Machine::findBroadcastInterface()
{

	std::map<u_int32_t, std::string> interfaces;
	Mercury::BundleBroadcast bhandler(networkInterface_, KBE_PORT_BROADCAST_DISCOVERY);

	if (!bhandler.epListen().getInterfaces(interfaces))
	{
		ERROR_MSG("Machine::findBroadcastInterface: Failed to discover network interfaces\n");
		return false;
	}

	uint8 data = 1;
	bhandler << data;
	if (!bhandler.broadcast(KBE_PORT_BROADCAST_DISCOVERY))
	{
		ERROR_MSG("Machine::findBroadcastInterface:Failed to send broadcast discovery message. error:%s\n", kbe_strerror());
		return false;
	}
	
	sockaddr_in	sin;

	if(bhandler.receive(NULL, &sin))
	{
		INFO_MSG("Machine::findBroadcastInterface:Machine::findBroadcastInterface: Broadcast discovery receipt from %s.\n",
					inet_ntoa((struct in_addr&)sin.sin_addr.s_addr) );

		std::map< u_int32_t, std::string >::iterator iter;

		iter = interfaces.find( (u_int32_t &)sin.sin_addr.s_addr );
		if (iter != interfaces.end())
		{
			INFO_MSG("Machine::findBroadcastInterface: Confirmed %s (%s) as default broadcast route interface.\n",
				inet_ntoa((struct in_addr&)sin.sin_addr.s_addr),
				iter->second.c_str() );
			broadcastAddr_ = sin.sin_addr.s_addr;
			return true;
		}
	}
	
	std::string sinterface = "\t[";
	std::map< u_int32_t, std::string >::iterator iter = interfaces.begin();
	for(; iter != interfaces.end(); iter++)
	{
		sinterface += inet_ntoa((struct in_addr&)iter->first);
		sinterface += ", ";
	}

	sinterface += "]";
	ERROR_MSG("Machine::findBroadcastInterface: Broadcast discovery [%s] not a valid interface. available interfaces:%s\n",
		inet_ntoa((struct in_addr&)sin.sin_addr.s_addr), sinterface.c_str());

	return false;
}

//-------------------------------------------------------------------------------------
bool Machine::initNetwork()
{
	epBroadcast_.socket(SOCK_DGRAM);
	ep_.socket(SOCK_DGRAM);
	epLocal_.socket(SOCK_DGRAM); 

	Mercury::Address address;
	address.ip = 0;
	address.port = 0;

	if (broadcastAddr_ == 0 && !this->findBroadcastInterface())
	{
		ERROR_MSG("Machine::initNetwork: Failed to determine default broadcast interface. "
				"Make sure that your broadcast route is set correctly. "
				"e.g. /sbin/ip route add broadcast 255.255.255.255 dev eth0\n" );
		return false;
	}

	if (!ep_.good() ||
		 ep_.bind(htons(KBE_MACHINE_BRAODCAST_SEND_PORT), broadcastAddr_) == -1)
	{
		ERROR_MSG("Machine::initNetwork: Failed to bind socket to '%s:%d'. %s.\n",
							inet_ntoa((struct in_addr &)broadcastAddr_), KBE_MACHINE_BRAODCAST_SEND_PORT,
							kbe_strerror());
		return false;
	}

	address.ip = broadcastAddr_;
	address.port = htons(KBE_MACHINE_BRAODCAST_SEND_PORT);
	ep_.setbroadcast( true );
	ep_.setnonblocking(true);
	ep_.addr(address);
	pEPPacketReceiver_ = new Mercury::UDPPacketReceiver(ep_, this->getNetworkInterface());

	if(!this->getMainDispatcher().registerFileDescriptor(ep_, pEPPacketReceiver_))
	{
		ERROR_MSG("Machine::initNetwork: registerFileDescriptor ep is failed!\n");
		return false;
	}
	
#if KBE_PLATFORM == PLATFORM_WIN32
	u_int32_t baddr = htonl(INADDR_ANY);
#else
	u_int32_t baddr = Mercury::BROADCAST;
#endif

	if (!epBroadcast_.good() ||
		epBroadcast_.bind(htons(KBE_MACHINE_BRAODCAST_SEND_PORT), baddr) == -1)
	{
		ERROR_MSG("Machine::initNetwork: Failed to bind socket to '%s:%d'. %s.\n",
							inet_ntoa((struct in_addr &)baddr), KBE_MACHINE_BRAODCAST_SEND_PORT,
							kbe_strerror());
#if KBE_PLATFORM != PLATFORM_WIN32
		return false;
#endif
	}
	else
	{
		address.ip = baddr;
		address.port = htons(KBE_MACHINE_BRAODCAST_SEND_PORT);
		epBroadcast_.setnonblocking(true);
		epBroadcast_.addr(address);
		pEBPacketReceiver_ = new Mercury::UDPPacketReceiver(epBroadcast_, this->getNetworkInterface());
	
		if(!this->getMainDispatcher().registerFileDescriptor(epBroadcast_, pEBPacketReceiver_))
		{
			ERROR_MSG("Machine::initNetwork: registerFileDescriptor epBroadcast is failed!\n");
			return false;
		}

	}

	if (!epLocal_.good() ||
		 epLocal_.bind(htons(KBE_MACHINE_BRAODCAST_SEND_PORT), Mercury::LOCALHOST) == -1)
	{
		ERROR_MSG("Machine::initNetwork: Failed to bind socket to (lo). %s.\n",
							kbe_strerror() );
		return false;
	}

	address.ip = Mercury::LOCALHOST;
	address.port = htons(KBE_MACHINE_BRAODCAST_SEND_PORT);
	epLocal_.setnonblocking(true);
	epLocal_.addr(address);
	pEPLocalPacketReceiver_ = new Mercury::UDPPacketReceiver(epLocal_, this->getNetworkInterface());

	if(!this->getMainDispatcher().registerFileDescriptor(epLocal_, pEPLocalPacketReceiver_))
	{
		ERROR_MSG("Machine::initNetwork: registerFileDescriptor epLocal is failed!\n");
		return false;
	}

	INFO_MSG("Machine::initNetwork: bind broadcast successfully! addr:%s\n", ep_.addr().c_str());
	return true;
}

//-------------------------------------------------------------------------------------
bool Machine::run()
{
	bool ret = true;

	while(!this->getMainDispatcher().isBreakProcessing())
	{
		this->getMainDispatcher().processOnce(false);
		getNetworkInterface().handleChannels(&MachineInterface::messageHandlers);
		KBEngine::sleep(100);
	};

	return ret;
}

//-------------------------------------------------------------------------------------
void Machine::handleTimeout(TimerHandle handle, void * arg)
{
	ServerApp::handleTimeout(handle, arg);
}

//-------------------------------------------------------------------------------------
bool Machine::initializeBegin()
{
	if(!initNetwork())
		return false;

	return true;
}

//-------------------------------------------------------------------------------------
bool Machine::inInitialize()
{
	return true;
}

//-------------------------------------------------------------------------------------
bool Machine::initializeEnd()
{
	pActiveTimerHandle_.cancel(); // machine不需要与其他组件保持活动状态关系
	return true;
}

//-------------------------------------------------------------------------------------
void Machine::finalise()
{
	ServerApp::finalise();
}

//-------------------------------------------------------------------------------------

}

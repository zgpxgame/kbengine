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


#include "serverapp.hpp"
#include "server/serverconfig.hpp"
#include "server/componentbridge.hpp"
#include "network/channel.hpp"
#include "server/componentbridge.hpp"

#include "../../server/baseappmgr/baseappmgr_interface.hpp"
#include "../../server/cellappmgr/cellappmgr_interface.hpp"
#include "../../server/baseapp/baseapp_interface.hpp"
#include "../../server/cellapp/cellapp_interface.hpp"
#include "../../server/dbmgr/dbmgr_interface.hpp"
#include "../../server/loginapp/loginapp_interface.hpp"
#include "../../server/resourcemgr/resourcemgr_interface.hpp"
#include "../../server/tools/message_log/messagelog_interface.hpp"

namespace KBEngine{
COMPONENT_TYPE g_componentType = UNKNOWN_COMPONENT_TYPE;
COMPONENT_ID g_componentID = 0;
COMPONENT_ORDER g_componentOrder = -1;
GAME_TIME g_kbetime = 0;

const float ACTIVE_TICK_TIMEOUT_DEFAULT = 30.0;

//-------------------------------------------------------------------------------------
ServerApp::ServerApp(Mercury::EventDispatcher& dispatcher, 
					 Mercury::NetworkInterface& ninterface, 
					 COMPONENT_TYPE componentType,
					 COMPONENT_ID componentID):
SignalHandler(),
TimerHandler(),
Mercury::ChannelTimeOutHandler(),
Components::ComponentsNotificationHandler(),
componentType_(componentType),
componentID_(componentID),
mainDispatcher_(dispatcher),
networkInterface_(ninterface),
timers_(),
startGlobalOrder_(-1),
startGroupOrder_(-1),
pActiveTimerHandle_()
{
	networkInterface_.pExtensionData(this);
	networkInterface_.pChannelTimeOutHandler(this);
	networkInterface_.pChannelDeregisterHandler(this);

	startActiveTick(ACTIVE_TICK_TIMEOUT_DEFAULT);
}

//-------------------------------------------------------------------------------------
ServerApp::~ServerApp()
{
	pActiveTimerHandle_.cancel();
}

//-------------------------------------------------------------------------------------
bool ServerApp::loadConfig()
{
	return true;
}

//-------------------------------------------------------------------------------------		
bool ServerApp::installSingnals()
{
	g_kbeSignalHandlers.attachApp(this);
	g_kbeSignalHandlers.addSignal(SIGINT, this);
	g_kbeSignalHandlers.addSignal(SIGHUP, this);
	return true;
}

//-------------------------------------------------------------------------------------		
bool ServerApp::initialize()
{
	if(!installSingnals())
		return false;
	
	if(!loadConfig())
		return false;
	
	if(!initializeBegin())
		return false;
	
	if(!inInitialize())
		return false;
	
	// 广播自己的地址给网上上的所有kbemachine
	// 并且从kbemachine获取basappmgr和cellappmgr以及dbmgr地址
	Componentbridge::getSingleton().getComponents().pHandler(this);
	this->getMainDispatcher().addFrequentTask(&Componentbridge::getSingleton());

	return initializeEnd();
}

//-------------------------------------------------------------------------------------		
void ServerApp::finalise(void)
{
	pActiveTimerHandle_.cancel();
}

//-------------------------------------------------------------------------------------		
double ServerApp::gameTimeInSeconds() const
{
	return double(g_kbetime) / g_kbeSrvConfig.gameUpdateHertz();
}

//-------------------------------------------------------------------------------------
void ServerApp::handleTimeout(TimerHandle, void * arg)
{
	switch (reinterpret_cast<uintptr>(arg))
	{
		case TIMEOUT_ACTIVE_TICK:
		{
			int8 findComponentTypes[] = {BASEAPPMGR_TYPE, CELLAPPMGR_TYPE, DBMGR_TYPE, CELLAPP_TYPE, 
								BASEAPP_TYPE, LOGINAPP_TYPE, MESSAGELOG_TYPE, RESOURCEMGR_TYPE, UNKNOWN_COMPONENT_TYPE};
			
			int ifind = 0;
			while(findComponentTypes[ifind] != UNKNOWN_COMPONENT_TYPE)
			{
				COMPONENT_TYPE componentType = (COMPONENT_TYPE)findComponentTypes[ifind];

				Components::COMPONENTS& components = Components::getSingleton().getComponents(componentType);
				Components::COMPONENTS::iterator iter = components.begin();
				for(; iter != components.end(); iter++)
				{
					Mercury::Bundle bundle;
					COMMON_MERCURY_MESSAGE(componentType, bundle, onAppActiveTick);
					
					bundle << g_componentType;
					bundle << componentID_;
					if((*iter).pChannel != NULL)
						bundle.send(getNetworkInterface(), (*iter).pChannel);
				}

				ifind++;
			}
			break;
		}
		default:
			break;
	}
}

//-------------------------------------------------------------------------------------
void ServerApp::startActiveTick(float period)
{
	pActiveTimerHandle_.cancel();
	pActiveTimerHandle_ = getMainDispatcher().addTimer(int(period * 1000000),
									this, (void *)TIMEOUT_ACTIVE_TICK);
}

//-------------------------------------------------------------------------------------
void ServerApp::handleTimers()
{
	timers().process(g_kbetime);
}

//-------------------------------------------------------------------------------------		
bool ServerApp::run(void)
{
	mainDispatcher_.processUntilBreak();
	return true;
}

//-------------------------------------------------------------------------------------	
void ServerApp::shutDown()
{
	INFO_MSG( "ServerApp::shutDown: shutting down\n" );
	mainDispatcher_.breakProcessing();
}

//-------------------------------------------------------------------------------------	
void ServerApp::onSignalled(int sigNum)
{
	switch (sigNum)
	{
	case SIGINT:
	case SIGHUP:
		this->shutDown();
	default:
		break;
	}
}

//-------------------------------------------------------------------------------------	
void ServerApp::onChannelDeregister(Mercury::Channel * pChannel)
{
	if(pChannel->isInternal())
	{
		Componentbridge::getSingleton().onChannelDeregister(pChannel);
	}
}

//-------------------------------------------------------------------------------------	
void ServerApp::onChannelTimeOut(Mercury::Channel * pChannel)
{
	INFO_MSG( "ServerApp::onChannelTimeOut: "
		"Channel %s timed out.\n", pChannel->c_str());

	networkInterface_.deregisterChannel(pChannel);
	pChannel->destroy();
}

//-------------------------------------------------------------------------------------
void ServerApp::onAddComponent(const Components::ComponentInfos* pInfos)
{
	if(pInfos->componentType == MESSAGELOG_TYPE)
	{
		DebugHelper::getSingleton().registerMessagelog(MessagelogInterface::writeLog.msgID, pInfos->pIntAddr.get());
	}
}

//-------------------------------------------------------------------------------------
void ServerApp::onRemoveComponent(const Components::ComponentInfos* pInfos)
{
	if(pInfos->componentType == MESSAGELOG_TYPE)
	{
		DebugHelper::getSingleton().unregisterMessagelog(MessagelogInterface::writeLog.msgID, pInfos->pIntAddr.get());
	}
}

//-------------------------------------------------------------------------------------
void ServerApp::onRegisterNewApp(Mercury::Channel* pChannel, int32 uid, std::string& username, 
						int8 componentType, uint64 componentID, 
						uint32 intaddr, uint16 intport, uint32 extaddr, uint16 extport)
{
	if(pChannel->isExternal())
		return;

	INFO_MSG("ServerApp::onRegisterNewApp: uid:%d, username:%s, componentType:%s, "
			"componentID:%"PRAppID", intaddr:%s, intport:%u, extaddr:%s, extport:%u,  from %s.\n", 
			uid, username.c_str(), COMPONENT_NAME_EX((COMPONENT_TYPE)componentType), componentID, 
			inet_ntoa((struct in_addr&)intaddr), ntohs(intport), 
			extaddr != 0 ? inet_ntoa((struct in_addr&)extaddr) : "nonsupport", ntohs(extport),
			pChannel->c_str());

	Components::ComponentInfos* cinfos = Componentbridge::getComponents().findComponent((
		KBEngine::COMPONENT_TYPE)componentType, uid, componentID);

	if(cinfos == NULL)
	{
		Componentbridge::getComponents().addComponent(uid, username.c_str(), 
			(KBEngine::COMPONENT_TYPE)componentType, componentID, intaddr, intport, extaddr, extport, pChannel);
	}
	else
	{
		KBE_ASSERT(cinfos->pIntAddr->ip == intaddr && cinfos->pIntAddr->port == intport);
		cinfos->pChannel = pChannel;
	}
}

//-------------------------------------------------------------------------------------
void ServerApp::onAppActiveTick(Mercury::Channel* pChannel, COMPONENT_TYPE componentType, COMPONENT_ID componentID)
{
	if(componentType != CLIENT_TYPE)
		if(pChannel->isExternal())
			return;
	
	Mercury::Channel* pTargetChannel = NULL;
	if(componentType != CONSOLE_TYPE)
	{
		Components::ComponentInfos* cinfos = 
			Componentbridge::getComponents().findComponent(componentType, KBEngine::getUserUID(), componentID);

		if(cinfos == NULL)
		{
			ERROR_MSG("ServerApp::onAppActiveTick[%x]: %s:%"PRAppID" not found.\n", 
		pChannel, COMPONENT_NAME_EX(componentType), componentID);
			return;
		}

		pTargetChannel = cinfos->pChannel;
		pTargetChannel->updateLastReceivedTime();
	}
	else
	{
		pChannel->updateLastReceivedTime();
		pTargetChannel = pChannel;
	}

	DEBUG_MSG("ServerApp::onAppActiveTick[%x]: %s:%"PRAppID" lastReceivedTime:%"PRIu64" at %s.\n", 
		pChannel, COMPONENT_NAME_EX(componentType), componentID, pChannel->lastReceivedTime(), pTargetChannel->c_str());
}

//-------------------------------------------------------------------------------------
void ServerApp::reqClose(Mercury::Channel* pChannel)
{
	DEBUG_MSG("ServerApp::reqClose: %s\n", pChannel->c_str());
	// this->getNetworkInterface().deregisterChannel(pChannel);
	// pChannel->destroy();
}

//-------------------------------------------------------------------------------------		
}

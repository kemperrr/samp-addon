#pragma once



#include "addon.h"



extern void *pAMXFunctions;

extern amxMutex *gMutex;
extern amxPool *gPool;
extern amxSocket *gSocket;
extern amxThread *gThread;

logprintf_t logprintf;


std::list<AMX *> amxList;

std::queue<amxConnect> amxConnectQueue;
std::queue<amxConnectError> amxConnectErrorQueue;
std::queue<amxDisconnect> amxDisconnectQueue;
std::queue<amxKey> amxKeyQueue;
std::queue<amxScreenshot> amxScreenshotQueue;
std::queue<amxFileReceive> amxFileReceiveQueue;
std::queue<amxFileSend> amxFileSendQueue;
std::queue<amxFileError> amxFileErrorQueue;





PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() 
{
    return (SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK);
}



PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) 
{
    pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
    logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];

	gMutex = new amxMutex();
	gPool = new amxPool();
	gSocket = new amxSocket();

    logprintf(" samp-addon was loaded");

    return true;
}



PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
	//delete gMutex;
	gMutex->~amxMutex();
	gSocket->Close();

	gPool->pluginInit = false;

    logprintf(" samp-addon was unloaded");
}



PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) 
{
	amxList.push_back(amx);

    return amx_Register(amx, addonNatives, -1);
}



PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) 
{
	for(std::list<AMX *>::iterator i = amxList.begin(); i != amxList.end(); i++)
	{
		if(*i == amx)
		{
			amxList.erase(i);

			break;
		}
	}

    return AMX_ERR_NONE;
}



PLUGIN_EXPORT void PLUGIN_CALL ProcessTick()
{
	int amx_idx;
	cell amxAddress;

	if(!amxConnectQueue.empty())
	{
		amxConnect connectData;

		for(unsigned int i = 0; i < amxConnectQueue.size(); i++)
		{
			gMutex->Lock();
			connectData = amxConnectQueue.front();
			amxConnectQueue.pop();
			gMutex->unLock();

			for(std::list<AMX *>::iterator amx = amxList.begin(); amx != amxList.end(); amx++)
			{
				// public Addon_OnClientConnect(clientid, client_ip[])
				if(!amx_FindPublic(*amx, "Addon_OnClientConnect", &amx_idx))
				{
					amx_PushString(*amx, &amxAddress, NULL, connectData.ip.c_str(), NULL, NULL);
					amx_Push(*amx, connectData.clientID);

					amx_Exec(*amx, NULL, amx_idx);

					amx_Release(*amx, amxAddress);
				}
			}
		}
	}

	if(!amxConnectErrorQueue.empty())
	{
		amxConnectError connectErrorData;

		for(unsigned int i = 0; i < amxConnectErrorQueue.size(); i++)
		{
			gMutex->Lock();
			connectErrorData = amxConnectErrorQueue.front();
			amxConnectErrorQueue.pop();
			gMutex->unLock();

			for(std::list<AMX *>::iterator amx = amxList.begin(); amx != amxList.end(); amx++)
			{
				// public Addon_OnClientConnectError(client_ip[], error[], error_id)
				if(!amx_FindPublic(*amx, "Addon_OnClientConnectError", &amx_idx))
				{
					amx_Push(*amx, connectErrorData.errorCode);
					amx_PushString(*amx, &amxAddress, NULL, connectErrorData.error.c_str(), NULL, NULL);
					amx_PushString(*amx, NULL, NULL, connectErrorData.ip.c_str(), NULL, NULL);

					amx_Exec(*amx, NULL, amx_idx);

					amx_Release(*amx, amxAddress);
				}
			}
		}
	}

	if(!amxDisconnectQueue.empty())
	{
		amxDisconnect disconnectData;

		for(unsigned int i = 0; i < amxDisconnectQueue.size(); i++)
		{
			gMutex->Lock();
			disconnectData = amxDisconnectQueue.front();
			amxDisconnectQueue.pop();
			gMutex->unLock();

			for(std::list<AMX *>::iterator amx = amxList.begin(); amx != amxList.end(); amx++)
			{
				// public Addon_OnClientDisconnect(clientid)
				if(!amx_FindPublic(*amx, "Addon_OnClientDisconnect", &amx_idx))
				{
					amx_Push(*amx, disconnectData.clientID);

					amx_Exec(*amx, NULL, amx_idx);
				}
			}
		}
	}

	if(!amxKeyQueue.empty())
	{
		amxKey keyData;

		for(unsigned int i = 0; i < amxKeyQueue.size(); i++)
		{
			gMutex->Lock();
			keyData = amxKeyQueue.front();
			amxKeyQueue.pop();
			gMutex->unLock();

			for(std::list<AMX *>::iterator amx = amxList.begin(); amx != amxList.end(); amx++)
			{
				// public Addon_OnClientKeyDataUpdate(clientid, keys[])
				if(!amx_FindPublic(*amx, "Addon_OnClientKeyDataUpdate", &amx_idx))
				{
					amx_PushString(*amx, &amxAddress, NULL, keyData.keys, NULL, NULL);
					amx_Push(*amx, keyData.clientID);

					amx_Exec(*amx, NULL, amx_idx);

					amx_Release(*amx, amxAddress);
				}
			}
		}
	}

	if(!amxScreenshotQueue.empty())
	{
		amxScreenshot screenshotData;

		for(unsigned int i = 0; i < amxScreenshotQueue.size(); i++)
		{
			gMutex->Lock();
			screenshotData = amxScreenshotQueue.front();
			amxScreenshotQueue.pop();
			gMutex->unLock();

			for(std::list<AMX *>::iterator amx = amxList.begin(); amx != amxList.end(); amx++)
			{
				// public Addon_OnScreenshotTaken(clientid, remote_file[])
				if(!amx_FindPublic(*amx, "Addon_OnScreenshotTaken", &amx_idx))
				{
					amx_PushString(*amx, &amxAddress, NULL, screenshotData.name, NULL, NULL);
					amx_Push(*amx, screenshotData.clientID);

					amx_Exec(*amx, NULL, amx_idx);

					amx_Release(*amx, amxAddress);
				}
			}
		}
	}

	if(!amxFileReceiveQueue.empty())
	{
		amxFileReceive fileData;

		for(unsigned int i = 0; i < amxFileReceiveQueue.size(); i++)
		{
			gMutex->Lock();
			fileData = amxFileReceiveQueue.front();
			amxFileReceiveQueue.pop();
			gMutex->unLock();

			for(std::list<AMX *>::iterator amx = amxList.begin(); amx != amxList.end(); amx++)
			{
				// public Addon_OnClientFileReceived(clientid, file[])
				if(!amx_FindPublic(*amx, "Addon_OnClientFileReceived", &amx_idx))
				{
					amx_PushString(*amx, &amxAddress, NULL, fileData.file.c_str(), NULL, NULL);
					amx_Push(*amx, fileData.clientid);

					amx_Exec(*amx, NULL, amx_idx);

					amx_Release(*amx, amxAddress);
				}
			}
		}
	}

	if(!amxFileSendQueue.empty())
	{
		amxFileSend fileData;

		for(unsigned int i = 0; i < amxFileSendQueue.size(); i++)
		{
			gMutex->Lock();
			fileData = amxFileSendQueue.front();
			amxFileSendQueue.pop();
			gMutex->unLock();

			for(std::list<AMX *>::iterator amx = amxList.begin(); amx != amxList.end(); amx++)
			{
				// public Addon_OnClientFileSended(clientid, file[])
				if(!amx_FindPublic(*amx, "Addon_OnClientFileSended", &amx_idx))
				{
					amx_PushString(*amx, &amxAddress, NULL, fileData.file.c_str(), NULL, NULL);
					amx_Push(*amx, fileData.clientid);

					amx_Exec(*amx, NULL, amx_idx);

					amx_Release(*amx, amxAddress);
				}
			}
		}
	}

	if(!amxFileErrorQueue.empty())
	{
		amxFileError fileError;

		for(unsigned int i = 0; i < amxFileErrorQueue.size(); i++)
		{
			gMutex->Lock();
			fileError = amxFileErrorQueue.front();
			amxFileErrorQueue.pop();
			gMutex->unLock();

			for(std::list<AMX *>::iterator amx = amxList.begin(); amx != amxList.end(); amx++)
			{
				// public Addon_OnFileTransferError(io_mode, clientid, file[], error[], error_code)
				if(!amx_FindPublic(*amx, "Addon_OnFileTransferError", &amx_idx))
				{
					amx_Push(*amx, fileError.errorCode);
					amx_PushString(*amx, &amxAddress, NULL, fileError.error.c_str(), NULL, NULL);
					amx_PushString(*amx, NULL, NULL, fileError.file.c_str(), NULL, NULL);
					amx_Push(*amx, fileError.clientid);
					amx_Push(*amx, fileError.io);

					amx_Exec(*amx, NULL, amx_idx);

					amx_Release(*amx, amxAddress);
				}
			}
		}
	}
}
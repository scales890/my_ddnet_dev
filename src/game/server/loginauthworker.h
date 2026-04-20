//Here! add file
#ifndef GAME_SERVER_LOGINAUTHWORKER_H
#define GAME_SERVER_LOGINAUTHWORKER_H

#include <engine/server/databases/connection_pool.h>
#include <engine/shared/protocol.h>

class IDbConnection;

struct CLoginAuthResult : ISqlResult
{
	bool m_AuthSuccess = false;
	char m_aBoundName[MAX_NAME_LENGTH] = {0};
	char m_aMessage[128] = {0};
};

struct CSqlLoginAuthRequest : ISqlData
{
	CSqlLoginAuthRequest(std::shared_ptr<CLoginAuthResult> pResult) :
		ISqlData(std::move(pResult))
	{
	}

	int m_ClientId = -1;
	char m_aToken[128] = {0};
	char m_aRequestingPlayer[MAX_NAME_LENGTH] = {0};
};

class CLoginAuthWorker
{
public:
	static bool VerifyToken(IDbConnection *pSqlServer, const ISqlData *pGameData, char *pError, int ErrorSize);
};

#endif

//Here! add file
#include "loginauthworker.h"

#include <base/str.h>
#include <engine/server/databases/connection.h>

bool CLoginAuthWorker::VerifyToken(IDbConnection *pSqlServer, const ISqlData *pGameData, char *pError, int ErrorSize)
{
	const auto *pData = dynamic_cast<const CSqlLoginAuthRequest *>(pGameData);
	auto *pResult = dynamic_cast<CLoginAuthResult *>(pGameData->m_pResult.get());
	if(!pData || !pResult)
		return false;

	char aBuf[512];
	str_format(aBuf, sizeof(aBuf),
		"SELECT Name "
		"FROM %s_tokens "
		"WHERE Token = ? "
		"LIMIT 1",
		pSqlServer->GetPrefix());

	if(!pSqlServer->PrepareStatement(aBuf, pError, ErrorSize))
		return false;

	pSqlServer->BindString(1, pData->m_aToken);

	bool End = false;
	if(!pSqlServer->Step(&End, pError, ErrorSize))
		return false;

	if(End)
	{
		str_copy(pResult->m_aMessage, "Token not found", sizeof(pResult->m_aMessage));
		pResult->m_AuthSuccess = false;
		return true;
	}

	char aBoundName[MAX_NAME_LENGTH];
	pSqlServer->GetString(1, aBoundName, sizeof(aBoundName));

	if(str_comp(aBoundName, pData->m_aRequestingPlayer) != 0)
	{
		str_copy(pResult->m_aMessage, "Token does not match player name", sizeof(pResult->m_aMessage));
		pResult->m_AuthSuccess = false;
		return true;
	}

	str_copy(pResult->m_aBoundName, aBoundName, sizeof(pResult->m_aBoundName));
	str_copy(pResult->m_aMessage, "OK", sizeof(pResult->m_aMessage));
	pResult->m_AuthSuccess = true;
	return true;
}

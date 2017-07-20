#include "AccessServerResult.h"


AccessServerResult::AccessServerResult(void)
{
}

AccessServerResult::~AccessServerResult(void)
{
}

bool AccessServerResult::DealWithJSONFrServer(std::string mRecvJsonStr, int urlTag, std::string urlApi)
{
	return true;
}

void AccessServerResult::CurlError(std::string url, int res, int urlTag)
{

}
#ifndef ACCOUNT_NUMBER_H
#define ACCOUNT_NUMBER_H

#define USRNUMLEN 17
#define PWDPOS "login.userinfo"
#include <vector>

class __declspec(dllexport) AccountNumber
{
public:
	struct USERPWD 
	{
		int userAccount[USRNUMLEN];
		int pwd[USRNUMLEN];
	};

	AccountNumber(void);
	virtual ~AccountNumber(void);

	void ReadUsrInfo(std::vector<AccountNumber::USERPWD *> &outUsrs, int &isFirstLogin, int &outUsrNum, int &outLastUsr);
	void WriteUsrInfo(std::vector<AccountNumber::USERPWD *> &ioUsrs, int isFirstToLogin, int inLastUsrIndex, int removeIndex);
	void ClearUsersVector(std::vector<AccountNumber::USERPWD *> &outUsrs);
	void Incode(int *outNums, const char *inOri);
	char *Decode(int *inNums);
	bool AddOrModifyUsr(std::vector<AccountNumber::USERPWD *> &inUsrs, AccountNumber::USERPWD *inUsr, int &outIndex);
private:
	char *mOrichar;
	bool isEqualAccount(const AccountNumber::USERPWD *inUsr1, const AccountNumber::USERPWD *inUsr2);
};

#endif 
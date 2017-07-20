#include "AccountNumber.h"
#include <vector>
#include "zhfunclib.h"

AccountNumber::AccountNumber(void)
{
	mOrichar = NULL;
}


AccountNumber::~AccountNumber(void)
{
	if (mOrichar != NULL)
	{
		delete []mOrichar;
		mOrichar = NULL;
	}
}

void AccountNumber::ReadUsrInfo(std::vector<AccountNumber::USERPWD *> &outUsrs, int &isFirstLogin, int &outUsrNum, int &outLastUsr)
{
	FILE *fp = NULL;
	char tmpBuf[300];
	sprintf(tmpBuf, "%s/%s", ZHFuncLib::GetWorkPath().c_str(), PWDPOS);
	fp = fopen(tmpBuf, "r");
	outUsrNum = 0;
	outLastUsr = 0;
	if (NULL != fp)
	{
		//CFloatWindowDlg *pWnd = (CFloatWindowDlg *)theApp.m_pMainWnd;
		fread(&isFirstLogin, sizeof(BOOL), 1, fp);
		fread(&outUsrNum, sizeof(int), 1, fp);
		fread(&outLastUsr, sizeof(int), 1, fp);
		for (int i = 0; i < outUsrNum; ++i)
		{
			AccountNumber::USERPWD *usr = new AccountNumber::USERPWD;
			fread(usr, sizeof(AccountNumber::USERPWD), 1, fp);
			outUsrs.push_back(usr);
		}
		fclose(fp);
	}
	if (outUsrNum > 30)  // 说明是错误的
	{
		outUsrNum = 0;
		outLastUsr = 0;
		ClearUsersVector(outUsrs);
	}
}

void AccountNumber::ClearUsersVector(std::vector<AccountNumber::USERPWD *> &outUsrs)
{
	int sz = outUsrs.size();
	for (int i = 0; i < sz; ++i)
	{
		AccountNumber::USERPWD *usr = outUsrs[i];
		delete usr;
		usr = NULL;
	}
	outUsrs.clear();
}

void AccountNumber::WriteUsrInfo(std::vector<AccountNumber::USERPWD *> &ioUsrs, int isFirstToLogin, int inLastUsrIndex, int removeIndex)
{
	int sz = ioUsrs.size();
	if (removeIndex > -1)
		sz -= 1;
	FILE *fp = NULL;
	char tmpBuf[300];
	sprintf(tmpBuf, "%s/%s", ZHFuncLib::GetWorkPath().c_str(), PWDPOS);
	if((fp = fopen(tmpBuf, "w")) != NULL)
	{
		fwrite(&isFirstToLogin, sizeof(BOOL), 1, fp);
		fwrite(&sz, sizeof(int), 1, fp);
		fwrite(&inLastUsrIndex, sizeof(int), 1, fp);
		for (int i = 0; i < sz; ++i)
		{
			if (removeIndex == i)
				continue;
			AccountNumber::USERPWD *usr = ioUsrs[i];
			fwrite(usr, sizeof(AccountNumber::USERPWD), 1, fp);
		}
	}
	fclose(fp);	
}


/************************************************************************/
/* user name is mobile num, there is 12 bytes,  top 12 byte of USRNUMS will be added to mobile num, 
15th byte marks the mobile num lenght*/
/************************************************************************/
#define USRADDNUMS {20, 1, 102, 42, 48, 50, 23, 76, 68, 39, 110, 101, 112, 133, 4, 15}
#define NEWORDER {10, 6, 0, 14, 7, 5, 1, 13, 4, 9, 2, 8, 3, 11, 12, 15}

void AccountNumber::Incode(int *outNums, const char *inOri)
{
	// 打乱顺序
	int neworders[] = NEWORDER;
	int sz = strlen(inOri);
	char *newchar = new char[USRNUMLEN];
	for (int i = 0; i < sz; ++i)
	{
		newchar[neworders[i]] = inOri[i];
	}
	newchar[USRNUMLEN - 1] = 0;
	// 每个字节都加一个数字
	int nums[] = USRADDNUMS;
	for (int i = 0; i < USRNUMLEN - 1; ++i)
	{
		outNums[i] = nums[i] + newchar[i];
	}
	outNums[USRNUMLEN - 1] = sz;
	delete []newchar;
}

char *AccountNumber::Decode(int *inNums)
{
	// 还原原来的字符
	int nums[] = USRADDNUMS;
	char *newchar = new char[USRNUMLEN];
	for (int i = 0; i < USRNUMLEN - 1; ++i)
	{
		newchar[i] = inNums[i] - nums[i];
	}
	// 还原原来的顺序
	// 原字符长度
	int neworders[] = NEWORDER;
	int sz = inNums[USRNUMLEN - 1];
	if (mOrichar == NULL)
		mOrichar = new char[USRNUMLEN];
	for (int i = 0; i < sz; ++i)
	{
		mOrichar[i] = newchar[neworders[i]];
	}
	delete []newchar;
	mOrichar[sz] = 0;
	return mOrichar;
}
#undef USRNUMS
//#undef PWDNUMS


bool AccountNumber::isEqualAccount(const AccountNumber::USERPWD *inUsr1, const AccountNumber::USERPWD *inUsr2)
{
	bool isequal = true;
	if (inUsr1 == NULL || inUsr2 == NULL)
		return false;
	AccountNumber::USERPWD *usr1 = const_cast<AccountNumber::USERPWD *>(inUsr1);
	AccountNumber::USERPWD *usr2 = const_cast<AccountNumber::USERPWD *>(inUsr2);

	char *usrch1 = Decode(usr1->userAccount);
	int len1 = strlen(usrch1);
	char *pUsr1 = new char[len1 + 1];
	for (int i = 0; i < len1; ++i)
		pUsr1[i] = usrch1[i];
	pUsr1[len1] = 0;

	char *usrch2 = Decode(usr2->userAccount);
	int len2 = strlen(usrch2);
	char *pUsr2 = new char[len2 + 1];
	for (int i = 0; i < len2; ++i)
		pUsr2[i] = usrch2[i];
	pUsr2[len2] = 0;

	if (len1 != len2)
		return false;

	for (int i = 0; i < len1; ++i)
	{
		if (pUsr1[i] != pUsr2[i])
		{
			isequal = false;
			break;
		}
	}
	delete []pUsr1;
	delete []pUsr2;
	return isequal;
}

bool AccountNumber::AddOrModifyUsr(std::vector<AccountNumber::USERPWD *> &ioUsrs, AccountNumber::USERPWD *inUsr, int &outIndex)
{
	int sz = ioUsrs.size();
	bool hasthisusr = false;
	for (int i = 0; i < sz; ++i)
	{
		USERPWD *usr = ioUsrs[i];
		if (isEqualAccount(usr, inUsr))
		{
			hasthisusr = true;
			outIndex = i;
			for (int i = 0; i < USRNUMLEN; ++i)  // 更新密码
			{
				usr->pwd[i] = inUsr->pwd[i];
			}
			break;
		}
	}
	if (!hasthisusr && inUsr != NULL)
	{
		USERPWD *usr = new USERPWD;
		*usr = *inUsr;
		ioUsrs.push_back(usr);
		outIndex = sz;
	}
	return !hasthisusr;
}
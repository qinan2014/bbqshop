#ifndef DETOURINJECTION_H
#define DETOURINJECTION_H

#include <QtWidgets/QWidget>
#include "ui_detourinjection.h"

class DetourInjection : public QWidget
{
	Q_OBJECT

public:
	DetourInjection(QWidget *parent = 0);
	~DetourInjection();

private:
	Ui::DetourInjectionClass ui;

	//void EnableDebugPrivilege();
	//LPVOID WriteProcessMem(HANDLE hProcess, LPCTSTR lpText);
	//LPTHREAD_START_ROUTINE GetLoadLibraryAddr();
	//LPVOID WriteFileMapping(HANDLE hMap, CONTENT_FILE_MAPPING content);
	//BOOL CreateRemoteThreadEX(HANDLE hProcess, LPTHREAD_START_ROUTINE pfnStartAddr, LPVOID pRemoteMem);

	void injectDll(unsigned long processID, unsigned long long hookApiFlag, wchar_t *dllPath);

protected:
	virtual bool nativeEvent(const QByteArray & eventType, void * message, long * result);

private slots:
	void readHookFile();
};

#endif // DETOURINJECTION_H


// Redactor.h: основной файл заголовка для приложения Redactor
//
#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"       // основные символы


// CRedactorApp:
// Сведения о реализации этого класса: Redactor.cpp
//

class CRedactorApp : public CWinApp
{
public:
	CRedactorApp() noexcept;


// Переопределение
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Реализация

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	//afx_msg void DwnloadFile();

	afx_msg void SaveAs();
	afx_msg void On32778();
};

extern CRedactorApp theApp;

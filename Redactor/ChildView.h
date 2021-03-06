
// ChildView.h: интерфейс класса CChildView
//
#include<string>

#pragma once


// Окно CChildView

class CChildView : public CWnd
{
// Создание
public:
	CChildView();

	
private:
	int m_width;
	int m_height;
	BYTE* m_pImage=nullptr;
	BITMAPINFO m_ImageInfo;
// Атрибуты
public:
		
// Операции
public:
	
	
// Переопределение
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	

// Реализация
public:
	virtual ~CChildView();
	bool OpenImage(const CString& NameF, CString& error);
	BOOL SaveImage(const CString& NameF, CString& error);
	const BYTE* Image();
	void SetImage(BYTE* rgb);
	void Invert(int colour);
	// Созданные функции схемы сообщений
protected:
	afx_msg void OnPaint();
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void InvertR();
	afx_msg void InvertG();
	afx_msg void InvertB();
	afx_msg void FlipX();
	afx_msg void FlipY();
private:

public:
};


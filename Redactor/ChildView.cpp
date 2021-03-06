
// ChildView.cpp: реализация класса CChildView
//

#include "stdafx.h"
#include "Redactor.h"
#include "ChildView.h"
#include"MainFrm.h"
#include<memory>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
	delete m_pImage;
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_COMMAND(ID_32778, &CChildView::InvertR)
	ON_COMMAND(ID_32779, &CChildView::InvertG)
	ON_COMMAND(ID_32780, &CChildView::InvertB)
	ON_COMMAND(ID_32781, &CChildView::FlipX)
	ON_COMMAND(ID_32782, &CChildView::FlipY)
END_MESSAGE_MAP()



// Обработчики сообщений CChildView


BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), nullptr);

	return TRUE;
}

const BYTE * CChildView::Image()
{
	return m_pImage;
}
void CChildView::SetImage(BYTE* rgb)
{
	if (m_pImage != nullptr)
		delete m_pImage;
	m_pImage = rgb;
	InvalidateRect(NULL);

}


void CChildView::OnPaint() 
{
	CPaintDC dc(this); // контекст устройства для рисования
	SetDIBitsToDevice(dc, 0, 0,m_width ,m_height, 0, 0, 0, m_height, m_pImage,&m_ImageInfo, DIB_RGB_COLORS);	
}

bool CChildView::OpenImage(const CString& NameF, CString& error)
{
	CFile myfile;
	CFileException pError ;
	if (!myfile.Open(NameF, CFile::modeReadWrite | CFile::typeBinary, &pError))
	{
		
		TCHAR szCause[255];
		pError.GetErrorMessage(szCause,255);
		error = szCause;
		return false;
	}

	BITMAPFILEHEADER my_header;
	myfile.Read((char*)&my_header, sizeof(BITMAPFILEHEADER));
	
	

	if (my_header.bfType != 0x4d42)
	{
		error = _T("This format of file is incorrect(not bitmap).");
		return false;
		//int Err = MessageBox(NULL, (LPCWSTR)L"You downloaded the wrong file format.\nTry to upload new file in right format.",  MB_CANCELTRYCONTINUE);
	}

	int sizefil = sizeof(BITMAPFILEHEADER);
	BYTE *info = new BYTE[my_header.bfOffBits-sizefil];
	if (!myfile.Read((char*)info, my_header.bfOffBits - sizefil))
	{
		error = _T("Problems with read file.");
		return false;
	}

	BITMAPINFO *newq = (BITMAPINFO*)info;
	DWORD *pallete = (DWORD*)newq->bmiColors;

	if (newq->bmiHeader.biCompression!= 0)
	{
		error = _T("This file is not in the correct format.BiCompression !=0.");
		return false;
	}
	int bpp;
	m_height = newq->bmiHeader.biHeight;
	m_width = newq->bmiHeader.biWidth;
	bpp = newq->bmiHeader.biBitCount;

	if (bpp != 8 && bpp != 32 && bpp != 24)
	{
		error = _T("Bit per pixel is not in the correct range of values.");
		return false;
	}
	
	

	m_ImageInfo.bmiHeader.biBitCount = 32;
	m_ImageInfo.bmiHeader.biPlanes = 1;
	m_ImageInfo.bmiHeader.biClrImportant = 0;
	m_ImageInfo.bmiHeader.biClrUsed = 0;
	m_ImageInfo.bmiHeader.biCompression = BI_RGB;
	m_ImageInfo.bmiHeader.biHeight = m_height;
	m_ImageInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_ImageInfo.bmiHeader.biSizeImage = 4 *m_width*m_height;
	m_ImageInfo.bmiHeader.biWidth = m_width;
	m_ImageInfo.bmiHeader.biXPelsPerMeter = newq->bmiHeader.biXPelsPerMeter;
	m_ImageInfo.bmiHeader.biYPelsPerMeter = newq->bmiHeader.biYPelsPerMeter;

	int stride;
	stride = (m_width*bpp / 8 + 3)&~3;//байты

	BYTE *Input_image = new BYTE[stride*m_height];

	if (!myfile.Read((char*)Input_image, stride*m_height))
	{
		error = _T("Problems with read file.");
		return false;
	}
	myfile.Close();

	BYTE*ouput_image = new BYTE[m_width*m_height * 4];
	BYTE*pin = Input_image;
	BYTE*pout = ouput_image;
	DWORD* out_image = (DWORD*)ouput_image;
	if (bpp == 24)
	{
		for (int y = 0; y < m_height; y++)//читаем картинку(если 24 bpp)
		{

			for (int x = 0; x < m_width; x++)
			{
				pout[x * 4 + 0] = pin[x * 3 + 0];
				pout[x * 4 + 1] = pin[x * 3 + 1];
				pout[x * 4 + 2] = pin[x * 3 + 2];
				pout[x * 4 + 3] = 0;

			}
			pin += stride;
			pout += 4 * m_width;
		}
	}
	
	if (bpp == 8)
	{
		for (int y = 0; y < m_height; y++)//читаем картинку(если 8 bpp)
		{
			for (int x = 0; x < m_width; x++)
			{
				//DWORD !!!
				out_image[x] = pallete[pin[x]];
				/*pout[x * 4 + 0] = pallete[ pin[x]].rgbBlue;
				pout[x * 4 + 1] = pallete[pin[x]].rgbGreen;
				pout[x * 4 + 2] = pallete[ pin[x]].rgbRed;
				pout[x * 4 + 3] = pallete[pin[x]].rgbReserved;*/
			}
			pin += stride;
			out_image += m_width;
			//pout += 4 * m_width;
		}
	}
	
	if (bpp != 32)
	{

		SetImage(ouput_image);
		delete Input_image;
	}
	else
	{
		SetImage(Input_image);
		delete ouput_image;
	}

	delete info;
	return true;
}

BOOL CChildView::SaveImage(const CString & NameF, CString& error)
{
	if (m_pImage!= nullptr)
	{
		CFile newImage;
		CFileException pError;
		if (!newImage.Open(NameF, CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary, &pError))
		{
			TCHAR szCause[255];
			pError.GetErrorMessage(szCause, 255);
			error = szCause;
			return false;

		}
		BITMAPFILEHEADER my_header;
		my_header.bfType = 0x4D42;
		my_header.bfReserved1 = 0;
		my_header.bfReserved2 = 0;
		my_header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		my_header.bfSize = (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)) / 8 + m_ImageInfo.bmiHeader.biSizeImage;
		newImage.Write((char*)&my_header, sizeof(BITMAPFILEHEADER));
		newImage.Flush();
		newImage.Write((char*)&m_ImageInfo.bmiHeader, sizeof(BITMAPINFOHEADER));
		newImage.Write((char*)m_pImage, m_height*m_width*4);
		newImage.Close();

		InvalidateRect(NULL);
		return true;
	}
	else
	{
		error = _T("You didn't open any image.");
		return false;
	}
}

void CChildView::Invert(int colour)
{
	if (m_pImage != nullptr)
	{
		BYTE *pout = m_pImage + colour;
		int stride = 4 * m_width;

		for (int y = 0; y < m_height; y++)
		{
			for (int x = 0; x < m_width; x++)
				pout[x * 4] ^= 255;
			pout += stride;
		}
		InvalidateRect(NULL);
	}
	else
	{
		int S = MessageBox((LPCWSTR)L"You didn't open any image.\nPlease, start programm again.", NULL, MB_CANCELTRYCONTINUE);
	}
}

void CChildView::InvertR()
{
	Invert(2);
}


void CChildView::InvertG()
{
	Invert(1);
}


void CChildView::InvertB()
{
	Invert(0);
}

void CChildView::FlipX()
{
	if (m_pImage != nullptr)
	{
	
		int mediumy, medow;
		mediumy = m_height / 2;
		
		DWORD *down_pic=(DWORD*)m_pImage;
		DWORD *up_pic=(DWORD*)m_pImage;

		up_pic +=(m_height)*m_width;
		for (int y = 0; y < mediumy; y++)
		{
			up_pic -=m_width;
			for (int x = 0; x < m_width; x++)
			{
				DWORD copy;
				copy = down_pic[x];
				down_pic[x] = up_pic[x];
				up_pic[x] = copy;
				
			}
			down_pic += m_width;
		}

		InvalidateRect(NULL);
	}
	else
		int S = MessageBox((LPCWSTR)L"You didn't open any image.\nPlease, start programm again.", NULL, MB_CANCELTRYCONTINUE);
}


void CChildView::FlipY()
{
	if (m_pImage != nullptr)
	{
		int stride = m_width * 4;
		int mediumx, medow;
		mediumx = m_width / 2;
		medow = mediumx;
		if (m_width % 2 == 1)medow++;

		BYTE *s = new BYTE[medow * 4];
		DWORD *right_part, *left_part;
		right_part = (DWORD*)m_pImage;
		left_part = (DWORD*)m_pImage;
		right_part += m_width-1;

		for (int y = 0; y < m_height; y++)//написать в случае нечетного
		{

			for (int x = 0; x < mediumx; x++)
			{
				DWORD copy;
				copy = left_part[x];
				left_part[x] = right_part[-x];
				right_part[-x] = copy;

			}
			right_part += m_width;
			left_part += m_width;
		}
		InvalidateRect(NULL);
	}
	else
		int S = MessageBox((LPCWSTR)L"You didn't open any image.\nPlease, start programm again.", NULL, MB_CANCELTRYCONTINUE);
}


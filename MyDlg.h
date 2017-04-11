#pragma once
//#include <opencv2\opencv.hpp>
#include <opencv2\opencv.hpp>
#include <cv.h>
#include <opencv2\highgui\highgui.hpp>
#include <vector>
#include "MyButton.h"
#include "afxwin.h"

using namespace cv;
using namespace std;
// MyDlg 对话框

void onSkySelected(int event, int x, int y, int flag, void * ustg);
class MyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MyDlg)

public:
	MyDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~MyDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()

public:
	Mat img1;
	Mat img2;
	Mat result;
	Mat mix;
	bool flg;
	bool flg1, flg2;
	int light;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	virtual BOOL OnInitDialog();
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	void ChangeImageRGB(Mat tag, int r, int flag, Mat res);
	afx_msg void OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlider3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton4();
	friend void onSkySelected(int event, int x, int y, int flag, void * ustg);
	void ImageColorMatch(Mat &src, Mat &ref,int flag);
	double CaculateSemblance(Mat tag, Mat ref);
	CMyButton m_btn1;
	CMyButton m_btn2;
	CMyButton m_btn3;
	CMyButton m_btn4;
	afx_msg void OnCbnSelchangeCombo1();
	CComboBox m_combo;
	afx_msg void OnBnClickedButton5();
	CMyButton m_btn5;
	CMyButton m_btn6;
	CMyButton m_btn7;
	CMyButton m_btn8;
	CMyButton m_btn9;
	CMyButton m_btn10;
	CMyButton m_btn11;
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton11();
};


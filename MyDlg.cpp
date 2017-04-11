// MyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCApplication4.h"
#include "MyDlg.h"
#include "afxdialogex.h"

#include <opencv2\opencv.hpp>
#include <cv.h>
#include <opencv2\highgui\highgui.hpp>
#include <atlconv.h>
#include <iostream>

#define SIZE 2                       //天空模板区域

#define light_blue  RGB(86, 168, 218)
#define blue RGB(20, 112, 194)

using namespace cv;
using namespace std;
// MyDlg 对话框

Point ptB; //左键按下时坐标  
Point ptE; //左键抬起时坐标
vector<Mat> modules;
bool flg3 = false;
Mat me;

IMPLEMENT_DYNAMIC(MyDlg, CDialogEx)

MyDlg::MyDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{
	flg1 = flg2 = flg = false;
}

MyDlg::~MyDlg()
{
}

void MyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_btn1);
	DDX_Control(pDX, IDC_BUTTON2, m_btn2);
	DDX_Control(pDX, IDC_BUTTON3, m_btn3);
	DDX_Control(pDX, IDC_BUTTON4, m_btn4);
	DDX_Control(pDX, IDC_COMBO1, m_combo);
	DDX_Control(pDX, IDC_BUTTON5, m_btn5);
	DDX_Control(pDX, IDC_BUTTON6, m_btn6);
	DDX_Control(pDX, IDC_BUTTON7, m_btn7);
	DDX_Control(pDX, IDC_BUTTON8, m_btn8);
	DDX_Control(pDX, IDC_BUTTON9, m_btn9);
	DDX_Control(pDX, IDC_BUTTON10, m_btn10);
	DDX_Control(pDX, IDC_BUTTON11, m_btn11);
}


BEGIN_MESSAGE_MAP(MyDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &MyDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &MyDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &MyDlg::OnBnClickedButton3)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &MyDlg::OnNMCustomdrawSlider1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER2, &MyDlg::OnNMCustomdrawSlider2)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER3, &MyDlg::OnNMCustomdrawSlider3)
	ON_BN_CLICKED(IDC_BUTTON4, &MyDlg::OnBnClickedButton4)
	ON_CBN_SELCHANGE(IDC_COMBO1, &MyDlg::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON5, &MyDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &MyDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &MyDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &MyDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &MyDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &MyDlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON11, &MyDlg::OnBnClickedButton11)
END_MESSAGE_MAP()


// MyDlg 消息处理程序




void MyDlg::OnBnClickedButton1()
{

	// TODO: 在此添加控件通知处理程序代码
	CString FilePathName;
	CFileDialog fdlg(true);
	if (fdlg.DoModal() == IDOK) {
		FilePathName = fdlg.GetPathName();
		//FilePathName.Replace( _T("\\"), _T("//"));
		int len = FilePathName.GetLength() ;
		

		USES_CONVERSION;
		char *buffer = T2A(FilePathName.GetBuffer(0));
		
		img1 = imread(buffer);
		resize(img1, img1, Size(500, 500));
		me = img1;
		namedWindow("原图像");
		imshow("原图像", img1);
		setMouseCallback("原图像", onSkySelected);
		flg1 = true;
	}
}


void MyDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString FilePathName;
	CFileDialog fdlg(true);
	if (fdlg.DoModal() == IDOK) {
		FilePathName = fdlg.GetPathName();
		//FilePathName.Replace( _T("\\"), _T("//"));

		USES_CONVERSION;
		char *buffer = T2A(FilePathName.GetBuffer(0));

		img2 = imread(buffer);
		imshow("渲染图像", img2);
		flg2 = true;
	}
}


void MyDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!flg1) {
		AfxMessageBox(_T("请选择原图片"));
		return;
	}
	if (!flg2) {
		AfxMessageBox(_T("请选择渲染图片"));
		return;
	}
	if (!flg3) {
		AfxMessageBox(_T("请在原图选择天空区域模板"));
		return;
	}
	Mat bg = img2;
	Mat sl = imread("sunlight.jpg");


	//制作天空Mask
	Mat mask(me.rows, me.cols, CV_8UC1, Scalar::all(0));
	Mat a;// = me.rowRange(80, 80 + size).colRange(240, 240 + size);
	modules[0].copyTo(a);

	for (int y = 0; y < me.rows / SIZE; y = y++) {
		for (int x = 0; x < me.cols / SIZE; x++) {
			Mat t = me.rowRange(y*SIZE, (y + 1)*SIZE).colRange(x*SIZE, (x + 1)*SIZE);
			double semblance = 0;
			double tmpSemblance;
			for (int k = 0; k < modules.size(); k++) {
				tmpSemblance = CaculateSemblance(t, modules[k]);
				semblance = tmpSemblance > semblance ? tmpSemblance : semblance;
			}
			if (abs(semblance - SIZE*SIZE) < 0.01) {                    //小于当前阈值标记为天空
				for (int i = 0; i < SIZE; i++) {
					for (int j = 0; j < SIZE; j++) {
						mask.at<uchar>(y*SIZE + i, x*SIZE + j) = 255;
					}
				}
			}
		}
	}

	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));      //腐蚀（膨胀）算子
	erode(mask, result, element);                                        //膨胀
																		 //GaussianBlur(result, result, Size(3,3), 20);                        //高斯滤波处理蒙版边缘

	Mat out;
	me.copyTo(out);                      
	bilateralFilter(out, mix, 11, 11 * 2, 11 / 2);//双边滤波

	ImageColorMatch(mix, img2,light);
	light = 0;
	/*cvtColor(mix, mix,  CV_BGR2HSV);                                     //转化为HSV进行调色
	for (int y = 0; y < mix.rows; y++) {
		uchar* p = mix.ptr(y);
		int q;
		for (int x = 0; x < mix.cols; x++) {
			q = *(p + x * 3);
			if (q > 35 && q < 99) {
				*(p + x * 3) = 57;
			}
			if (*(p + x * 3 + 2) + 95 <= 230) {
				*(p + x * 3 + 2) += 95;
			}
			if (*(p + x * 3 + 2) < 30) {
				*(p + x * 3 + 2) = 40;
			}
			if (*(p + x * 3 + 1) + 20 < 240) {
				*(p + x * 3 + 1) += 20;
			}
			if (*(p + x * 3 + 1)  < 30) {
				*(p + x * 3 + 1) += 20;
			}
		}
	}
	
	cvtColor(mix, mix, CV_HSV2BGR);*/         
	resize(bg, bg, Size(mix.cols, mix.rows));                         //重定背景大小
	bg.copyTo(mix, result);                                           //将天空背景替换原图像天空区域
	//resize(sl, sl, Size(mix.cols, mix.rows));                       //重定光晕大小
    //addWeighted(sl, 1.2, mix, 1, 0.0, mix);                         //添加光晕

	int num;                                                          //毛玻璃效果
	RNG r;
	for (int y = 0; y < mix.rows - 3; y++) {
		for (int x = 0; x < mix.cols - 3; x++) {
			num = r.uniform(0, 1);
			mix.at<Vec3b>(y, x)[0] = mix.at<Vec3b>(y + num, x + num)[0];
			mix.at<Vec3b>(y, x)[1] = mix.at<Vec3b>(y + num, x + num)[1];
			mix.at<Vec3b>(y, x)[2] = mix.at<Vec3b>(y + num, x + num)[2];
		}
	}

	imshow("结果",mix);
	flg = true;
}


BOOL MyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	((CSliderCtrl *)(GetDlgItem(IDC_SLIDER1)))->SetRange(-255, 255,true);
	((CSliderCtrl *)(GetDlgItem(IDC_SLIDER2)))->SetRange(-255, 255, true);
	((CSliderCtrl *)(GetDlgItem(IDC_SLIDER3)))->SetRange(-255, 255, true);
	((CSliderCtrl *)(GetDlgItem(IDC_SLIDER1)))->SetPos(0);
	((CSliderCtrl *)(GetDlgItem(IDC_SLIDER2)))->SetPos(0);
	((CSliderCtrl *)(GetDlgItem(IDC_SLIDER3)))->SetPos(0);
	m_btn1.SetUpColor(light_blue);
	m_btn1.SetDownColor(blue);
	m_btn1.ResetTextColor(RGB(255, 255, 255));
	m_btn2.SetUpColor(light_blue);
	m_btn2.SetDownColor(blue);
	m_btn2.ResetTextColor(RGB(255, 255, 255));
	m_btn3.SetUpColor(light_blue);
	m_btn3.SetDownColor(blue);
	m_btn3.ResetTextColor(RGB(255, 255, 255));
	m_btn4.SetUpColor(light_blue);
	m_btn4.SetDownColor(blue);
	m_btn4.ResetTextColor(RGB(255, 255, 255));
	m_combo.AddString(_T("天空1"));
	m_combo.AddString(_T("天空2"));
	m_combo.AddString(_T("天空3"));
	m_combo.AddString(_T("夕阳"));
	m_combo.AddString(_T("星空1"));
	m_combo.AddString(_T("星空2"));
	m_combo.AddString(_T("你的名字"));
	m_combo.ShowWindow(SW_HIDE);

	CBitmap bmp[7];
	bmp[0].LoadBitmapW(IDB_BITMAP1);
	m_btn5.SetBmp(&bmp[0]);
	bmp[1].LoadBitmapW(IDB_BITMAP2);
	m_btn6.SetBmp(&bmp[1]);
	bmp[2].LoadBitmapW(IDB_BITMAP3);
	m_btn7.SetBmp(&bmp[2]);
	bmp[3].LoadBitmapW(IDB_BITMAP4);
	m_btn8.SetBmp(&bmp[3]);
	bmp[4].LoadBitmapW(IDB_BITMAP5);
	m_btn9.SetBmp(&bmp[4]);
	bmp[5].LoadBitmapW(IDB_BITMAP6);
	m_btn10.SetBmp(&bmp[5]);
	bmp[6].LoadBitmapW(IDB_BITMAP7);
	m_btn11.SetBmp(&bmp[6]);

	// TODO:  在此添加额外的初始化
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

bool check(uchar *p, int r) {
	if (int(*p) + r > 255 || int(*p) + r < 0)
		return false;
	return true;
}

void MyDlg::ChangeImageRGB(Mat tag, int r, int flag, Mat res) {
	tag.copyTo(result);
	cvtColor(result, result,  CV_BGR2HSV);
	for (int y = 0; y < result.rows; y++) {
		uchar* p = result.ptr(y);
		for (int x = 0; x < result.cols; x++) {
			if (flag == 1) {
				if (check(p + x * 3, r)) {
					*(p + x * 3) += r;
				}
				else if(r > 0) *(p + x * 3) = 255;
				else *(p + x * 3) = 0;
			}
			if (flag == 2) {
				if (check(p + x * 3 + 1, r)) {
					*(p + x * 3 + 1) += r;
				}
				else if(r > 0)*(p + x * 3 + 1) = 255;
				else *(p + x * 3 + 1) = 0;
			}
			if (flag == 3) {
				if (check(p + x * 3 + 2, r)) {
					*(p + x * 3 + 2) += r;
				}
				else if(r > 0) *(p + x * 3 + 2) = 255;
				else  *(p + x * 3 + 2) = 0;
			}
		}
	}
	cvtColor(result, res, CV_HSV2BGR);
}


void MyDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	//AfxMessageBox(_T("ddd"));
	if (flg) {

		int pos1 = ((CSliderCtrl *)(GetDlgItem(IDC_SLIDER1)))->GetPos();
		int pos2 = ((CSliderCtrl *)(GetDlgItem(IDC_SLIDER2)))->GetPos();
		int pos3 = ((CSliderCtrl *)(GetDlgItem(IDC_SLIDER3)))->GetPos();
		ChangeImageRGB(mix, pos1, 1, result);
		ChangeImageRGB(result, pos2, 2, result);
		ChangeImageRGB(result, pos3, 3, result);
		imshow("结果", result);
	}
	*pResult = 0;
}


void MyDlg::OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (flg) {

		int pos1 = ((CSliderCtrl *)(GetDlgItem(IDC_SLIDER1)))->GetPos();
		int pos2 = ((CSliderCtrl *)(GetDlgItem(IDC_SLIDER2)))->GetPos();
		int pos3 = ((CSliderCtrl *)(GetDlgItem(IDC_SLIDER3)))->GetPos();
		ChangeImageRGB(mix, pos1, 1, result);
		ChangeImageRGB(result, pos2, 2, result);
		ChangeImageRGB(result, pos3, 3, result);
		imshow("结果", result);
	}
	*pResult = 0;
}


void MyDlg::OnNMCustomdrawSlider3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (flg) {

		int pos1 = ((CSliderCtrl *)(GetDlgItem(IDC_SLIDER1)))->GetPos();
		int pos2 = ((CSliderCtrl *)(GetDlgItem(IDC_SLIDER2)))->GetPos();
		int pos3 = ((CSliderCtrl *)(GetDlgItem(IDC_SLIDER3)))->GetPos();
		ChangeImageRGB(mix, pos1, 1, result);
		ChangeImageRGB(result, pos2, 2, result);
		ChangeImageRGB(result, pos3, 3, result);
		imshow("结果", result);
	}
	*pResult = 0;
}


void MyDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog fdlg(true);
	if (!flg) {
		AfxMessageBox(_T("当前无渲染结果"));
		return;
	}
	//flg = false;
	if (fdlg.DoModal() == IDOK) {
		CString SavePath = fdlg.GetPathName();
		USES_CONVERSION;
		char *buffer = T2A(SavePath.GetBuffer(0));
		imwrite(buffer, result);
	}
	//flg = true;

}
void onSkySelected(int event, int x, int y, int flag, void * ustg){
	if (event == CV_EVENT_LBUTTONDOWN){
		ptB = Point(x, y);
	}
	if (event == CV_EVENT_LBUTTONUP){
		//对鼠标画出的矩形框超出图像范围做处理，否则会越界崩溃  
		//对鼠标从右下往右上画矩形框的情况做处理  
		ptE = Point(x, y);

		Mat module = me.colRange(ptB.x, ptB.x + SIZE).rowRange(ptB.y, ptB.y + SIZE);
		modules.push_back(module);
		flg3 = true;
	}
}

double MyDlg::CaculateSemblance(Mat tag, Mat ref) {                          //余弦相似度计算
	double semblance = 0;
	uchar tr, tg, tb, rr, rg, rb;
	Vec3b vt, vr;
	cvtColor(tag, tag, CV_BGR2HSV);
	cvtColor(ref, ref, CV_BGR2HSV);
	for (int y = 0; y < ref.rows; y++) {
		for (int x = 0; x < ref.cols; x++) {
			vr = ref.at<Vec3b>(y, x);
			vt = tag.at<Vec3b>(y, x);
			tr = vt[2];
			tg = vt[1];
			tb = vt[0];
			rr = vr[2];
			rg = vr[1];
			rb = vr[0];
			semblance += (double)(tr*rr + tg*rg + tb*rb) / (double)(sqrt(tr*tr + tg*tg + tb*tb)*sqrt(rr*rr + rg*rg + rb*rb));
		}
	}
	cvtColor(tag, tag, CV_HSV2BGR);
	cvtColor(ref, ref, CV_HSV2BGR);
	return semblance;
}

void MyDlg::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	int sel = m_combo.GetCurSel();
	CString str;
	m_combo.GetLBText(sel, str);
	if (str == _T("天空1")) {
		img2 = imread("天空1.jpg");
		flg2 = true;
	}
	else if (str == _T("天空2")) {
		img2 = imread("天空2.jpg");
		flg2 = true;
	}
	else if (str == _T("天空3")) {
		img2 = imread("天空3.jpg");
		flg2 = true;
	}
	else if (str == _T("夕阳")) {
		img2 = imread("夕阳.jpg");
		flg2 = true;
	}
	else if (str == _T("星空1")) {

		img2 = imread("星空1.jpg");
		flg2 = true;
	}
	else if (str == _T("星空2")) {

		img2 = imread("星空2.jpg");
		flg2 = true;
	}
	else if (str == _T("你的名字")) {
		img2 = imread("sky.jpg");
		flg2 = true;
	}
}


void MyDlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	img2 = imread("天空1.jpg");
	light = 0;
	flg2 = true;
	OnBnClickedButton3();
}


void MyDlg::OnBnClickedButton6()
{
	// TODO: 在此添加控件通知处理程序代码
	img2 = imread("天空2.jpg");
	light = 0;
	flg2 = true;
	OnBnClickedButton3();
}


void MyDlg::OnBnClickedButton7()
{
	// TODO: 在此添加控件通知处理程序代码
	img2 = imread("天空3.jpg");
	light = 0;
	flg2 = true;
	OnBnClickedButton3();
}


void MyDlg::OnBnClickedButton8()
{
	// TODO: 在此添加控件通知处理程序代码
	img2 = imread("夕阳.jpg");
	light = 0;
	flg2 = true;
	OnBnClickedButton3();
}


void MyDlg::OnBnClickedButton9()
{
	// TODO: 在此添加控件通知处理程序代码
	img2 = imread("星空1.jpg");
	light = 1;
	flg2 = true;
	OnBnClickedButton3();
}


void MyDlg::OnBnClickedButton10()
{
	// TODO: 在此添加控件通知处理程序代码
	img2 = imread("星空2.jpg");
	light = 0;
	flg2 = true;
	OnBnClickedButton3();
}


void MyDlg::OnBnClickedButton11()
{
	// TODO: 在此添加控件通知处理程序代码
	img2 = imread("sky.jpg");
	light = 0;
	flg2 = true;
	OnBnClickedButton3();
}
void MyDlg::ImageColorMatch(Mat &ref, Mat &src,int flag = 0) {
	resize(src, src, Size(ref.cols, ref.rows));
	cvtColor(ref, ref, CV_BGR2Lab);
	cvtColor(src, src, CV_BGR2Lab);
	Mat m_r, sd_r, m_s, sd_s;
	meanStdDev(ref, m_r, sd_r);
	meanStdDev(src, m_s, sd_s);
	double t;
	for (int y = 0; y < ref.rows; y++) {
		Vec3b v;
		for (int x = 0; x < ref.cols; x++) {
			v = ref.at<Vec3b>(y, x);

			t = ref.at<Vec3b>(y, x)[0];
			t = (t - m_r.at<double>(0, 0))*(sd_s.at<double>(0, 0) / sd_r.at<double>(0, 0)) + m_s.at<double>(0, 0);
			t = t < 0 ? 0 : t;
			t = t > 255 ? 255 : t;
			ref.at<Vec3b>(y, x)[0] = t;

			t = ref.at<Vec3b>(y, x)[1];
			t = (t - m_r.at<double>(1, 0))*(sd_s.at<double>(1, 0) / sd_r.at<double>(1, 0)) + m_s.at<double>(1, 0);
			t = t < 0 ? 0 : t;
			t = t > 255 ? 255 : t;
			ref.at<Vec3b>(y, x)[1] = t;

			t = ref.at<Vec3b>(y, x)[2];
			t = (t - m_r.at<double>(2, 0))*(sd_s.at<double>(2, 0) / sd_r.at<double>(2, 0)) + m_s.at<double>(2, 0);
			t = t < 0 ? 0 : t;
			t = t > 255 ? 255 : t;
			ref.at<Vec3b>(y, x)[2] = t;
		}
	}
	cvtColor(ref, ref, CV_Lab2BGR);
	cvtColor(src, src, CV_Lab2BGR);
	if (flag == 2) {
		for (int y = 0; y < ref.rows; y++) {
			Vec3b v;
			uchar r, g, b;
			for (int x = 0; x < ref.cols; x++) {
				v = ref.at<Vec3b>(y, x);
				ref.at<Vec3b>(y, x)[0] += 20;
				ref.at<Vec3b>(y, x)[1] += 20;
				ref.at<Vec3b>(y, x)[2] += 20;
			}
		}
	}
}

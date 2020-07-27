#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define _AFXDLL
#define STATUS_ERROR            -1
#define IMAGE_NAME_LEN          64
#include <QMainWindow>
#include"Includes/MvCameraControl.h"
#include"vcruntime_string.h"
#include"mycamera.h"
#include"afxwin.h"
#include"cstringt.h"
#include"qmessagebox.h"
#include"QDebug"
#include <QButtonGroup>
#include <QMessageBox>

inline QString  CS2QS(CString cs) { return  QString::fromWCharArray((LPCTSTR)cs, cs.GetLength()); }
inline CString  QS2CS(QString qs) { return  qs.toStdString().c_str(); }

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    QButtonGroup *bg;
    int sel;

private slots:
    void on_ENUM_BUTTON_clicked();
    void on_OPEN_BUTTON_clicked();
    void on_CLOSE_BUTTON_clicked();

    void on_START_GRABBING_BUTTON_clicked();

    void on_CONTINUS_MODE_RADIO_clicked();

    void on_TRIGGER_MODE_RADIO_clicked();

    void on_SOFTWARE_TRIGGER_CHECK_stateChanged(int arg1);

    void on_SOFTWARE_ONCE_BUTTON_clicked();

    void on_STOP_GRABBING_BUTTON_clicked();

    void on_SAVE_BMP_BUTTON_clicked();

    void on_SAVE_JPG_BUTTON_clicked();

private:
    Ui::MainWindow *ui;

    /*ch:自定义变量 | en:User Defined Variable*/
private:
    /*ch:状态 | en:Status*/
    bool  m_bOpenDevice;                        // ch:是否打开设备 | en:Whether to open device
    bool  m_bStartGrabbing;                     // ch:是否开始抓图 | en:Whether to start grabbing
    int   m_nTriggerMode;                       // ch:触发模式 | en:Trigger Mode
    int   m_nTriggerSource;                     // ch:触发源 | en:Trigger Source
    MV_SAVE_IAMGE_TYPE   m_nSaveImageType;      // ch:保存图像格式 | en:Save Image Type

    /*ch:设备相关 | en:Device Related*/
    MyCamera*      m_pcMyCamera;               // ch:CMyCamera封装了常用接口 | en:CMyCamera packed commonly used interface
    HWND  m_hwndDisplay;                        // ch:显示句柄 | en:Display Handle
    MV_CC_DEVICE_INFO_LIST m_stDevList;         // ch:设备信息列表结构体变量，用来存储设备列表
    //en:Device information list structure variable used to store device list

    unsigned char*  m_pBufForSaveImage;         // ch:用于保存图像的缓存 | en:Buffer to save image
    unsigned int    m_nBufSizeForSaveImage;

    unsigned char*  m_pBufForDriver;            // ch:用于从驱动获取图像的缓存 | en:Buffer to get image from driver
    unsigned int    m_nBufSizeForDriver;

    /*ch:内部函数 | en:Built-in function*/
private:
    /*ch:初始化相机操作 | en:Initialization*/
    int OpenDevice(void);                    // ch:打开相机 | en:Open Device

    /*ch:设置、获取参数操作 | en:Set and get parameters operation*/
    int GetTriggerSource(void);              // ch:设置触发源 | en:Set Trigger Source
    int SetTriggerSource(void);

    /*ch:图片保存 | en:Save Image*/
    int SaveImage(void);                     // ch:保存图片 | en:Save Image

    /*ch:关闭销毁操作 | en:Close and deatroy operation*/
    int CloseDevice(void);                   // ch:关闭设备 | en:Close Device
    /*ch:按下控件操作 | en:Control operation*/
public:
    /*ch:初始化 | en:Initialization*/
    void OnBnClickedEnumButton();               // ch:查找设备 | en:Find Devices
    void OnBnClickedOpenButton();               // ch:打开设备 | en:Open Devices
    void OnBnClickedCloseButton();              // ch:关闭设备 | en:Close Devices

    /*ch:图像采集 | en:Image Acquisition*/
   // CButton m_ctrlContinusModeRadio;            // ch:连续模式 | en:Continus Mode
   // CButton m_ctrlTriggerModeRadio;             // ch:触发模式 | en:Trigger mode
   // CButton m_ctrlStartGrabbingButton;          // ch:开始抓图 | en:Start grabbing
    //CButton m_ctrlStopGrabbingButton;           // ch:结束抓图 | en:Stop grabbing
    //CButton m_ctrlSoftwareTriggerCheck;         // ch:软触发 | en:Software trigger
    bool    m_bSoftWareTriggerCheck;
    //CButton m_ctrlSoftwareOnceButton;           // ch:软触发一次 | en:Software Trigger Once

    /*ch:图像保存 | en:Image Save*/
    void OnBnClickedSaveBmpButton();            // ch:保存bmp | en:Save bmp
    void OnBnClickedSaveJpgButton();            // ch:保存jpg | en:Save jpg

    /*ch:设备显示下拉框 | en:Device display drop-down box*/
    //CComboBox m_ctrlDeviceCombo;                // ch:枚举到的设备 | en:Enumerated device
    int      m_nDeviceCombo;

    /*ch:设置、获取参数操作 | en:Set and get parameters operation*/
    int SetTriggerMode(void);                // ch:设置触发模式 | en:Set Trigger Mode
    int GetTriggerMode(void);
};

#endif // MAINWINDOW_H

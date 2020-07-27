#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    bg=new QButtonGroup(this);
    bg->addButton(ui->TRIGGER_MODE_RADIO,0);//一个值为0
    bg->addButton(ui->CONTINUS_MODE_RADIO,1);//一个值为1
    ui->OPEN_BUTTON->setEnabled(false);
    ui->CLOSE_BUTTON->setEnabled(false);
    ui->START_GRABBING_BUTTON->setEnabled(false);
    ui->STOP_GRABBING_BUTTON->setEnabled(false);
    ui->SOFTWARE_ONCE_BUTTON->setEnabled(false);
    ui->SAVE_BMP_BUTTON->setEnabled(false);
    ui->SAVE_JPG_BUTTON->setEnabled(false);
    ui->DETECT_LINE->setEnabled(false);
    ui->DETECT_CIRCLE->setEnabled(false);
    ui->CONTINUS_MODE_RADIO->setEnabled(false);
    ui->TRIGGER_MODE_RADIO->setEnabled(false);
    ui->SOFTWARE_TRIGGER_CHECK->setEnabled(false);
    ui->CONTINUS_MODE_RADIO->setChecked(true);
    m_hwndDisplay=(HWND)ui->label->winId();
    m_pBufForDriver=NULL;
    m_pcMyCamera=NULL;
    m_pBufForSaveImage=NULL;
    m_nBufSizeForSaveImage=0;
    m_nBufSizeForDriver=0;

}

MainWindow::~MainWindow()
{
    delete ui;
}

// ch:按下查找设备按钮:枚举 | en:Click Find Device button:Enumeration
void MainWindow::on_ENUM_BUTTON_clicked()
{
    CString strMsg;
    // ch:清除设备列表框中的信息 | en:Clear Device List Information
    ui->DEVICE_COMBO->clear();
    // ch:初始化设备信息列表 | en:Device Information List Initialization
    memset(&m_stDevList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
    // ch:枚举子网内所有设备 | en:Enumerate all devices within subnet
    int nRet = MyCamera::EnumDevices(&m_stDevList);
    // ch:将值加入到信息列表框中并显示出来 | en:Add value to the information list box and display
    unsigned int i;
    int nIp1, nIp2, nIp3, nIp4;
    for (i = 0; i < m_stDevList.nDeviceNum; i++)
    {
        MV_CC_DEVICE_INFO* pDeviceInfo = m_stDevList.pDeviceInfo[i];
        if (NULL == pDeviceInfo)
        {
            continue;
        }
        if (pDeviceInfo->nTLayerType == MV_GIGE_DEVICE)
        {
            nIp1 = ((m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
            nIp2 = ((m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
            nIp3 = ((m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
            nIp4 = (m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);

            wchar_t* pUserName = NULL;
            if (strcmp("", (LPCSTR)(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName)) != 0)
            {
                DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName), -1, NULL, 0);
                pUserName = new wchar_t[dwLenUserName];
                MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName), -1, pUserName, dwLenUserName);
            }
            else
            {
                char strUserName[256] = {0};
                sprintf_s(strUserName, "%s %s (%s)", pDeviceInfo->SpecialInfo.stGigEInfo.chManufacturerName,
                          pDeviceInfo->SpecialInfo.stGigEInfo.chModelName,
                          pDeviceInfo->SpecialInfo.stGigEInfo.chSerialNumber);
                DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, NULL, 0);
                pUserName = new wchar_t[dwLenUserName];
                MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, pUserName, dwLenUserName);
            }
            strMsg.Format(_T("[%d]GigE:    %s  (%d.%d.%d.%d)"), i,
                          pUserName, nIp1, nIp2, nIp3, nIp4);
            if (NULL != pUserName)
            {
                delete(pUserName);
                pUserName = NULL;
            }

        }
        else if (pDeviceInfo->nTLayerType == MV_USB_DEVICE)
        {
            wchar_t* pUserName = NULL;

            if (strcmp("", (char*)pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName) != 0)
            {
                DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName), -1, NULL, 0);
                pUserName = new wchar_t[dwLenUserName];
                MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName), -1, pUserName, dwLenUserName);
            }
            else
            {
                char strUserName[256];
                sprintf_s(strUserName, "%s %s (%s)", pDeviceInfo->SpecialInfo.stUsb3VInfo.chManufacturerName,
                          pDeviceInfo->SpecialInfo.stUsb3VInfo.chModelName,
                          pDeviceInfo->SpecialInfo.stUsb3VInfo.chSerialNumber);
                DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, NULL, 0);
                pUserName = new wchar_t[dwLenUserName];
                MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, pUserName, dwLenUserName);
            }
            strMsg.Format(_T("[%d]UsbV3:  %s"), i, pUserName);
            if (NULL != pUserName)
            {
                delete(pUserName);
                pUserName = NULL;
            }

        }
        else
        {
            //ShowErrorMsg(TEXT("Unknown device enumerated"), 0);
        }
        QString str_msg=CS2QS(strMsg);
        ui->DEVICE_COMBO->addItem(str_msg);
    }
    //m_ctrlDeviceCombo.SetCurSel(0);
    ui->DEVICE_COMBO->setCurrentIndex(0);
    ui->OPEN_BUTTON->setEnabled(true);
    return;
}
// ch:按下打开设备按钮：打开设备 | en:Click Open button: Open Device
void MainWindow::on_OPEN_BUTTON_clicked()
{
    int nRet = OpenDevice();
    if (MV_OK == nRet){
        qDebug()<<"open succesfully";
    }
    ui->CLOSE_BUTTON->setEnabled(true);
    ui->START_GRABBING_BUTTON->setEnabled(true);
    ui->CONTINUS_MODE_RADIO->setEnabled(true);
    ui->TRIGGER_MODE_RADIO->setEnabled(true);
    ui->SOFTWARE_TRIGGER_CHECK->setEnabled(true);
    ui->OPEN_BUTTON->setEnabled(false);
    //EnableControls(TRUE);
    return;
}
// ch:打开设备 | en:Open Device
int MainWindow::OpenDevice(void)
{
    if (TRUE == m_bOpenDevice)
    {
        return STATUS_ERROR;
    }
    //UpdateData(TRUE);
    //int nIndex = m_nDeviceCombo;
    int nIndex=ui->DEVICE_COMBO->currentIndex();
    m_pcMyCamera = new MyCamera;
    int nRet = m_pcMyCamera->Open(m_stDevList.pDeviceInfo[nIndex]);
    if (MV_OK != nRet)
    {
        qDebug()<<"not opencn";
        delete m_pcMyCamera;
        m_pcMyCamera = NULL;
        return nRet;
    }
    // ch:探测网络最佳包大小(只对GigE相机有效) | en:Detection network optimal package size(It only works for the GigE camera)
    if (m_stDevList.pDeviceInfo[nIndex]->nTLayerType == MV_GIGE_DEVICE)
    {
        int nPacketSize = m_pcMyCamera->GetOptimalPacketSize();
        if (nPacketSize > 0)
        {
            nRet = m_pcMyCamera->SetIntValue("GevSCPSPacketSize",nPacketSize);
        }
        m_bOpenDevice = TRUE;
        return MV_OK;
    }
    m_bOpenDevice = TRUE;
    return MV_OK;
}
// ch:按下关闭设备按钮：关闭设备 | en:Click Close button: Close Device
void MainWindow::on_CLOSE_BUTTON_clicked()
{
    int nRet=CloseDevice();
    if(MV_OK==nRet){
        qDebug()<<"close succesfully";
    }
    ui->CLOSE_BUTTON->setEnabled(false);
    ui->CLOSE_BUTTON->setEnabled(false);
    ui->START_GRABBING_BUTTON->setEnabled(false);
    ui->CONTINUS_MODE_RADIO->setEnabled(false);
    ui->TRIGGER_MODE_RADIO->setEnabled(false);
    ui->SOFTWARE_TRIGGER_CHECK->setEnabled(false);
    ui->OPEN_BUTTON->setEnabled(true);
    ui->STOP_GRABBING_BUTTON->setEnabled(false);
    return;
}
// ch:关闭设备 | en:Close Device
int MainWindow::CloseDevice(void)
{
    if (m_pcMyCamera)
    {
        m_pcMyCamera->Close();
        delete m_pcMyCamera;
        m_pcMyCamera = NULL;
    }

    m_bOpenDevice = FALSE;
    m_bStartGrabbing = FALSE;

    /*if (m_pBufForDriver)
    {
        free(m_pBufForDriver);
        m_pBufForDriver = NULL;
    }
    m_nBufSizeForDriver = 0;

    if (m_pBufForSaveImage)
    {
        free(m_pBufForSaveImage);
        m_pBufForSaveImage = NULL;
    }
    m_nBufSizeForSaveImage  = 0;*/

    return MV_OK;
}
// ch:开启抓图 | en:Start Grabbing
void MainWindow::on_START_GRABBING_BUTTON_clicked()
{
    ui->START_GRABBING_BUTTON->setEnabled(false);
    ui->STOP_GRABBING_BUTTON->setEnabled(true);
    ui->SAVE_BMP_BUTTON->setEnabled(true);
    ui->SAVE_JPG_BUTTON->setEnabled(true);
    if (FALSE == m_bOpenDevice || TRUE == m_bStartGrabbing)
    {
        return;
    }
    int nRet = MV_OK;
    if (NULL != m_pcMyCamera)
    {
        nRet = m_pcMyCamera->StartGrabbing();
        if (nRet == MV_OK)
        {
            nRet = m_pcMyCamera->Display(m_hwndDisplay);
        }
    }
    else
    {
        return ;
    }

    if (MV_OK != nRet)
    {
        return ;
    }
    ui->STOP_GRABBING_BUTTON->setEnabled(true);
    m_bStartGrabbing = TRUE;
    return;
}
// ch:按下连续模式按钮 | en:Click Continues button
void MainWindow::on_CONTINUS_MODE_RADIO_clicked()
{
    m_nTriggerMode = MV_TRIGGER_MODE_OFF;
    int nRet = SetTriggerMode();
    if (MV_OK != nRet)
    {
        return;
    }
    ui->SOFTWARE_ONCE_BUTTON->setEnabled(false);
}
// ch:按下触发模式按钮 | en:Click Trigger Mode button
void MainWindow::on_TRIGGER_MODE_RADIO_clicked()
{
    //UpdateData(TRUE);
    m_nTriggerMode = MV_TRIGGER_MODE_ON;
    int nRet = SetTriggerMode();
    if (m_bStartGrabbing == TRUE)
    {
        if (TRUE == m_bSoftWareTriggerCheck)
        {
            ui->SOFTWARE_ONCE_BUTTON->setEnabled(true);
        }
    }

    return;
}
// ch:设置触发模式 | en:Set Trigger Mode
int MainWindow::SetTriggerMode(void)
{
    //qDebug()<<m_nTriggerMode;
    int nRet = m_pcMyCamera->SetEnumValue("TriggerMode", m_nTriggerMode);
    if (MV_OK != nRet)
    {
        return nRet;
    }
    return MV_OK;
}
// ch:设置软触发模式 | en:Set soft trigger mode
void MainWindow::on_SOFTWARE_TRIGGER_CHECK_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) // "选中"
    {
        m_bSoftWareTriggerCheck=true;
    }
    else // 未选中 - Qt::Unchecked
    {
        m_bSoftWareTriggerCheck=false;
    }
    int nRet = SetTriggerSource();
    if (nRet != MV_OK)
    {
        return;
    }

    return;
}
// ch:设置触发源 | en:Set Trigger Source
int MainWindow::SetTriggerSource(void)
{
    if (m_bSoftWareTriggerCheck)
    {
        m_nTriggerSource = MV_TRIGGER_SOURCE_SOFTWARE;
        int nRet = m_pcMyCamera->SetEnumValue("TriggerSource", m_nTriggerSource);
        ui->SOFTWARE_ONCE_BUTTON->setEnabled(true);
    }
    else
    {
        m_nTriggerSource = MV_TRIGGER_SOURCE_LINE0;
        int nRet = m_pcMyCamera->SetEnumValue("TriggerSource", m_nTriggerSource);
        ui->SOFTWARE_ONCE_BUTTON->setEnabled(false);
    }

    return MV_OK;
}
// ch:按下软触发一次按钮 | en:Click Execute button
void MainWindow::on_SOFTWARE_ONCE_BUTTON_clicked()
{
    if (TRUE != m_bStartGrabbing)
    {
        return;
    }

    int nRet = m_pcMyCamera->CommandExecute("TriggerSoftware");

    return;
}
// ch:按下结束采集按钮 | en:Click Stop button
void MainWindow::on_STOP_GRABBING_BUTTON_clicked()
{
    ui->START_GRABBING_BUTTON->setEnabled(true);
    ui->STOP_GRABBING_BUTTON->setEnabled(false);
    ui->SAVE_BMP_BUTTON->setEnabled(false);
    ui->SAVE_JPG_BUTTON->setEnabled(false);
    if (FALSE == m_bOpenDevice || FALSE == m_bStartGrabbing)
    {
        return;
    }

    int nRet = MV_OK;
    if (NULL != m_pcMyCamera)
    {
        nRet = m_pcMyCamera->StopGrabbing();
    }
    else
    {
        return ;
    }

    if (MV_OK != nRet)
    {
        return ;
    }
    m_bStartGrabbing = FALSE;
    return;
}
// ch:按下保存bmp图片按钮 | en:Click Save BMP button
void MainWindow::on_SAVE_BMP_BUTTON_clicked()
{
    m_nSaveImageType = MV_Image_Bmp;
    int nRet = SaveImage();
    QMessageBox::information(this, "信息", "Save bgm succee");
    return;
}
// ch:保存图片 | en:Save Image
int MainWindow::SaveImage()
{
    if (FALSE == m_bStartGrabbing)
    {
        return STATUS_ERROR;
    }
    // ch:获取1张图 | en:get one image
    unsigned int nRecvBufSize = 0;
    int nRet = MV_OK;

    // ch:仅在第一次保存图像时申请缓存，在 CloseDevice 时释放
    // en:Request buffer only when save image for first time, release after CloseDevice
    if (NULL == m_pBufForDriver)
    {
        // ch:从相机中获取一帧图像大小 | en:Get size of one frame from camera
        nRet = m_pcMyCamera->GetIntValue("PayloadSize", &nRecvBufSize);
        // ch:一帧数据大小
        // en:One frame size
        m_nBufSizeForDriver = nRecvBufSize;
        m_pBufForDriver = (unsigned char *)malloc(m_nBufSizeForDriver);
    }
    MV_FRAME_OUT_INFO_EX stImageInfo = {0};
    memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));

    unsigned int nDataSize = nRecvBufSize;
    unsigned int nImageNum = 1;
    unsigned int nDataLen = 0;

    while(nImageNum)
    {
        nRet = m_pcMyCamera->GetOneFrameTimeout(m_pBufForDriver, &nDataLen, m_nBufSizeForDriver, &stImageInfo, 1000);
        if (nRet == MV_OK)
        {
            nImageNum--;

            // ch:仅在第一次保存图像时申请缓存，在 CloseDevice 时释放
            // en:Request buffer only when save image for first time, release after CloseDevice
            if (NULL == m_pBufForSaveImage)
            {
                // ch:BMP图片大小：width * height * 3 + 2048(预留BMP头大小)
                // en:// BMP image size: width * height * 3 + 2048 (Reserved BMP header size)
                m_nBufSizeForSaveImage = stImageInfo.nWidth * stImageInfo.nHeight * 3 + 2048;

                m_pBufForSaveImage = (unsigned char*)malloc(m_nBufSizeForSaveImage);
                if (NULL == m_pBufForSaveImage)
                {
                    break;
                }
            }
            // ch:设置对应的相机参数 | en:Set camera parameter
            MV_SAVE_IMAGE_PARAM_EX stParam = {0};
            stParam.enImageType = m_nSaveImageType; // ch:需要保存的图像类型 | en:Image format to save
            stParam.enPixelType = stImageInfo.enPixelType;  // ch:相机对应的像素格式 | en:Camera pixel type
            stParam.nWidth      = stImageInfo.nWidth;         // ch:相机对应的宽 | en:Width
            stParam.nHeight     = stImageInfo.nHeight;          // ch:相机对应的高 | en:Height
            stParam.nDataLen    = stImageInfo.nFrameLen;
            stParam.pData       = m_pBufForDriver;
            stParam.pImageBuffer = m_pBufForSaveImage;
            stParam.nBufferSize = m_nBufSizeForSaveImage;  // ch:存储节点的大小 | en:Buffer node size
            stParam.nJpgQuality     = 80;       // ch:jpg编码，仅在保存Jpg图像时有效。保存BMP时SDK内忽略该参数
            // en:jpg encoding, only valid when saving as Jpg. SDK ignore this parameter when saving as BMP

            nRet = m_pcMyCamera->SaveImage(&stParam);
            if(MV_OK != nRet)
            {
                break;
            }
            char chImageName[IMAGE_NAME_LEN] = {0};
            if (MV_Image_Bmp == stParam.enImageType)
            {
                sprintf_s(chImageName, IMAGE_NAME_LEN, "Image_w%d_h%d_fn%03d.bmp", stImageInfo.nWidth, stImageInfo.nHeight, stImageInfo.nFrameNum);
            }
            else if (MV_Image_Jpeg == stParam.enImageType)
            {
                sprintf_s(chImageName, IMAGE_NAME_LEN, "Image_w%d_h%d_fn%03d.jpg", stImageInfo.nWidth, stImageInfo.nHeight, stImageInfo.nFrameNum);
            }

            FILE* fp = fopen(chImageName, "wb");

            fwrite(m_pBufForSaveImage, 1, stParam.nImageLen, fp);
            fclose(fp);
        }
        else
        {
            break;
        }
    }

    return nRet;
}
// ch:按下保存jpg图片按钮 | en:Click Save JPG button
void MainWindow::on_SAVE_JPG_BUTTON_clicked()
{
    m_nSaveImageType = MV_Image_Jpeg;
    int nRet = SaveImage();
    QMessageBox::information(this, "信息", "Save jpg succee");
    return;
}

/************************************************************************/
/* ��C++�ӿ�Ϊ�������Գ��ú������ж��η�װ�������û�ʹ��                */
/************************************************************************/
#pragma once 

#include "MvCameraControl.h"

class  CMyCamera
{
public:
    CMyCamera();
    ~CMyCamera();

    static int EnumDevices(MV_CC_DEVICE_INFO_LIST* pstDevList);

    // ch:���豸 | en:Open Device
    int     Open(MV_CC_DEVICE_INFO* pstDeviceInfo);

    // ch:�ر��豸 | en:Close Device
    int     Close();

    // ch:����ץͼ | en:Start Grabbing
    int     StartGrabbing();

    // ch:ֹͣץͼ | en:Stop Grabbing
    int     StopGrabbing();

    // ch:������ȡһ֡ͼ������ | en:Get one frame initiatively
    int     GetOneFrameTimeout(unsigned char* pData, unsigned int* pnDataLen, unsigned int nDataSize, MV_FRAME_OUT_INFO_EX* pFrameInfo, int nMsec);

    // ch:������ʾ���ھ�� | en:Set Display Window Handle
    int     Display(void* hWnd);

    // ch:����ͼƬ | en:save image
    int     SaveImage(MV_SAVE_IMAGE_PARAM_EX* pstParam);

    // ch:ע��ͼ�����ݻص� | en:Register Image Data CallBack
    int     RegisterImageCallBack(void(__stdcall* cbOutput)(unsigned char * pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser),
                                    void* pUser);

    // ch:ע����Ϣ�쳣�ص� | en:Register Message Exception CallBack
    int     RegisterExceptionCallBack(void(__stdcall* cbException)(unsigned int nMsgType, void* pUser),
                                        void* pUser);

    // ch:��ȡInt�Ͳ������� Width��Height����ϸ���ݲο�SDK��װĿ¼�µ� MvCameraNode.xlsx �ļ�
    // en:Get Int type parameters, such as Width and Height, for details please refer to MvCameraNode.xlsx file under SDK installation directory
    int     GetIntValue(IN const char* strKey, OUT unsigned int *pnValue);
    int     SetIntValue(IN const char* strKey, IN unsigned int nValue);

    // ch:��ȡFloat�Ͳ������� ExposureTime��Gain����ϸ���ݲο�SDK��װĿ¼�µ� MvCameraNode.xlsx �ļ�
    // en:Get Float type parameters, such as ExposureTime and Gain, for details please refer to MvCameraNode.xlsx file under SDK installation directory
    int     GetFloatValue(IN const char* strKey, OUT float *pfValue);
    int     SetFloatValue(IN const char* strKey, IN float fValue);

    // ch:��ȡEnum�Ͳ������� PixelFormat����ϸ���ݲο�SDK��װĿ¼�µ� MvCameraNode.xlsx �ļ�
    // en:Get Enum type parameters, such as PixelFormat, for details please refer to MvCameraNode.xlsx file under SDK installation directory
    int     GetEnumValue(IN const char* strKey, OUT unsigned int *pnValue);
    int     SetEnumValue(IN const char* strKey, IN unsigned int nValue);

    // ch:��ȡBool�Ͳ������� ReverseX����ϸ���ݲο�SDK��װĿ¼�µ� MvCameraNode.xlsx �ļ�
    // en:Get Bool type parameters, such as ReverseX, for details please refer to MvCameraNode.xlsx file under SDK installation directory
    int     GetBoolValue(IN const char* strKey, OUT bool *pbValue);
    int     SetBoolValue(IN const char* strKey, IN bool bValue);

    // ch:��ȡString�Ͳ������� DeviceUserID����ϸ���ݲο�SDK��װĿ¼�µ� MvCameraNode.xlsx �ļ�UserSetSave
    // en:Get String type parameters, such as DeviceUserID, for details please refer to MvCameraNode.xlsx file under SDK installation directory
    int     GetStringValue(IN const char* strKey, IN OUT char* strValue, IN unsigned int nSize);
    int     SetStringValue(IN const char* strKey, IN const char * strValue);

    // ch:ִ��һ��Command������� UserSetSave����ϸ���ݲο�SDK��װĿ¼�µ� MvCameraNode.xlsx �ļ�
    // en:Execute Command once, such as UserSetSave, for details please refer to MvCameraNode.xlsx file under SDK installation directory
    int     CommandExecute(IN const char* strKey);

    // ch:̽��������Ѱ���С(ֻ��GigE�����Ч) | en:Detection network optimal package size(It only works for the GigE camera)
    int     GetOptimalPacketSize();

    // ��ȡͳ�Ʋ���
    int     GetAllMatchInfo(IN void* hDevHandle, IN unsigned int nTLayerTpye, OUT unsigned int *nLostFrame, OUT unsigned int *nFrameCount);

public:
    void*               m_hDevHandle;
    unsigned int     m_nTLayerType;

public:
    unsigned char*  m_pBufForSaveImage;         // ���ڱ���ͼ��Ļ���
    unsigned int    m_nBufSizeForSaveImage;

    unsigned char*  m_pBufForDriver;            // ���ڴ�������ȡͼ��Ļ���
    unsigned int    m_nBufSizeForDriver;

};

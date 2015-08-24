#pragma once
#include "ThostTraderApi\ThostFtdcTraderApi.h"
#include <QtCore>

class MainTraderSpi : public QObject, public CThostFtdcTraderSpi
{
	Q_OBJECT

signals:
	void FrontConnected();
	void UserLogined(int front_id ,int session_id, int iNextOrderRef);
	void SettlementInfoConfirmed();
	void TraderBalanceUpdated(double balance, double closeProfit, double positionProfit);
	void RtnTradeEvent(CThostFtdcTradeField tradeField);

public:
	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected();
	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	virtual void OnFrontDisconnected(int nReason);

	///��¼������Ӧ
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///Ͷ���߽�����ȷ����Ӧ
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///�����ѯ�ʽ��˻���Ӧ
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///����¼��������Ӧ
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///��������������Ӧ
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///����֪ͨ
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);
	///�ɽ�֪ͨ
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

	///����Ӧ��
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
		
	///������ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
	virtual void OnHeartBeatWarning(int nTimeLapse);

private:
	// �Ƿ��յ��ɹ�����Ӧ
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
	// �Ƿ����ڽ��׵ı���
	bool IsTradingOrder(CThostFtdcOrderField *pOrder);
};


class FollowTraderSpi: public QObject, public CThostFtdcTraderSpi
{
    Q_OBJECT

public:
    FollowTraderSpi();
    ~FollowTraderSpi();

private:
    int m_index;                                  // ���
    TThostFtdcAddressType m_FRONT_ADDR;     	  // ǰ�õ�ַ
    TThostFtdcBrokerIDType	m_BROKER_ID;       	  // ���͹�˾����
    TThostFtdcInvestorIDType m_INVESTOR_ID;		  // Ͷ���ߴ���
    TThostFtdcPasswordType  m_PASSWORD;			  // �û����� 
    int m_iRequestID;
    int m_iNextOrderRef;
    // �Ự����
    TThostFtdcFrontIDType	m_FRONT_ID;		//ǰ�ñ��
    TThostFtdcSessionIDType	m_SESSION_ID;	//�Ự���

    CThostFtdcTraderApi* m_pTraderApi;

signals:
    void TraderBalanceUpdated(int index, double balance, double closeProfit, double positionProfit);
    void TraderStatusUpdated(int index, QString message);
    void EventTableUpdated(QString message);
    void TraderLogined(int index);
    void RtnTradeEvent(CThostFtdcTradeField tradeField);
    void RspErrorInfo();

public slots:
    void ReqConnect();

public:
    void Init(int index, const char* frontAddr, const char* brokerID,
        const char* investorID, const char* password);

    // �û���¼����
    void ReqUserLogin();
    ///Ͷ���߽�����ȷ��
    void ReqSettlementInfoConfirm();
    ///�����ѯ�ʽ��˻�
    void ReqQryTradingAccount();

    ///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
    virtual void OnFrontConnected();
    ///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
    virtual void OnFrontDisconnected(int nReason);

    ///��¼������Ӧ
    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    ///Ͷ���߽�����ȷ����Ӧ
    virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    ///�����ѯ�ʽ��˻���Ӧ
    virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///����֪ͨ
    virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);
    ///�ɽ�֪ͨ
    virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);


    ///����Ӧ��
    virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///������ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
    virtual void OnHeartBeatWarning(int nTimeLapse);

private:
    // �Ƿ��յ��ɹ�����Ӧ
    bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
    // �Ƿ����ڽ��׵ı���
    bool IsTradingOrder(CThostFtdcOrderField *pOrder);
    bool IsFlowControl(int iResult);
};
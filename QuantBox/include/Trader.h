#pragma once
#include "ThostTraderApi\ThostFtdcTraderApi.h"
#include "include\TraderSpi.h"
#include <QtCore>

typedef struct
{
	char inverst_id[13];
	char password[41];
	char broker_id[11];
	char broker_name[20];
	char td_front_addr[101];
	int direct;
	float ratio;
} Account;

class CTrader: public QObject
{
	Q_OBJECT

public:
	CTrader(int index, Account accout);
	~CTrader();
	void SetThread(QThread * thread);

private:
	TThostFtdcAddressType m_FRONT_ADDR;						// ǰ�õ�ַ
	TThostFtdcBrokerIDType	m_BROKER_ID;					// ���͹�˾����
	TThostFtdcInvestorIDType m_INVESTOR_ID;					// Ͷ���ߴ���
	TThostFtdcPasswordType  m_PASSWORD;						// �û����� 
	int m_iRequestID;

	// �Ự����
	TThostFtdcFrontIDType	m_FRONT_ID;		//ǰ�ñ��
	TThostFtdcSessionIDType	m_SESSION_ID;	//�Ự���
    int m_iNextOrderRef;
	TThostFtdcOrderRefType	m_ORDER_REF;	//��������
	TThostFtdcOrderRefType	m_EXECORDER_REF;	//ִ����������
	TThostFtdcOrderRefType	m_FORQUOTE_REF;	//ѯ������
	TThostFtdcOrderRefType	m_QUOTE_REF;	//��������

	CTraderSpi * m_pTraderSpi;
	CThostFtdcTraderApi* m_pTraderApi;

	int m_index;  // 0:���˺ţ�1-N����Ϊ�����˺�
	char m_flowPath[100];
    int m_followDirect;
    float m_followRatio;

	QThread * m_pThread;

private:
	bool IsFlowControl(int iResult);

signals:
	void traderStatusUpdated(int index, QString message);
	void traderBalanceUpdated(int index, double balance, double closeProfit, double positionProfit);
    void eventTableUpdated(QString message);
	void RtnTradeEvent(CThostFtdcTradeField tradeField);
    void traderLogined(int index);

public slots:
	void ReqConnect();

    // �û��ǳ�����
	void ReqLogout();

	///�û���¼����
	void ReqUserLogin();
	void rspUserLogin(int front_id ,int session_id, int iNextOrderRef);
	///Ͷ���߽�����ȷ��
	void ReqSettlementInfoConfirm();
	///�����ѯ�ʽ��˻�
	void ReqQryTradingAccount();
	void rspQryTradingAccount(double balance, double closeProfit, double positionProfit);


	///����¼������
	void ReqOrderInsert(TThostFtdcInstrumentIDType INSTRUMENT_ID, TThostFtdcDirectionType DIRECTION, 
        TThostFtdcVolumeType VOLUME, TThostFtdcPriceType LIMIT_PRICE, TThostFtdcOffsetFlagType OFFSET_FLAG);
	///������������
	void ReqOrderAction(CThostFtdcOrderField *pOrder);
	// �Ƿ��ҵı����ر�
	bool IsMyOrder(CThostFtdcOrderField *pOrder);

	// ��������
	void rspTradingAciton();
	void FollowRtnTrade(CThostFtdcTradeField tradeField);
    void ProcessTradeEvent(CThostFtdcTradeField tradeField);

	// ����Ĳ���
	///�����ѯ��Լ
	void ReqQryInstrument(TThostFtdcInstrumentIDType instrument_id);
	///�����ѯͶ���ֲ߳�
	void ReqQryInvestorPosition(TThostFtdcInstrumentIDType instrument_id);

	///ִ������¼������
	void ReqExecOrderInsert(TThostFtdcInstrumentIDType INSTRUMENT_ID);
	///ִ�������������
	void ReqExecOrderAction(CThostFtdcExecOrderField *pExecOrder);
	// �Ƿ��ҵ�ִ������ر�
	bool IsMyExecOrder(CThostFtdcExecOrderField *pExecOrder);

	///ѯ��¼������
	void ReqForQuoteInsert(TThostFtdcInstrumentIDType INSTRUMENT_ID);
	///����¼������
	void ReqQuoteInsert(TThostFtdcInstrumentIDType INSTRUMENT_ID, TThostFtdcPriceType LIMIT_PRICE);
	///���۲�������
	void ReqQuoteAction(CThostFtdcQuoteField *pQuote);
	// �Ƿ��ҵı���
	bool IsMyQuote(CThostFtdcQuoteField *pQuote);

};

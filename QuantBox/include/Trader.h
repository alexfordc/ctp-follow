#include "ThostTraderApi\ThostFtdcTraderApi.h"
#include "include\TraderSpi.h"
#include <QtCore>
#include <map>
#include <string>
using namespace std;

typedef struct
{
    char inverstorID[13];
    char password[41];
    char brokerID[11];
    char brokerName[20];
    char tdFrontAddr[101];
    int direct;
    float ratio;
} Account;

typedef struct
{
	int direct;
	float ratio;
} FollowStrategy;

typedef struct
{
    int id;
    int followType;
    int actionSec;
    int priceType;
    int priceJump;
    int actionTimes;
    int finalType;
} Strategy;

class MainTrader: public QObject
{
	Q_OBJECT

public:
    MainTrader(Account &account);
	~MainTrader();
	void SetThread(QThread * mainThread);
    void AddFollowStrategy(const char* investorID, FollowStrategy &followStrategy);

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

	MainTraderSpi * m_pTraderSpi;
	CThostFtdcTraderApi* m_pTraderApi;

	QThread * m_pThread;
    map<string, FollowStrategy> m_followStrategys;
    vector<Strategy> m_orderStrategys;

private:
	bool IsFlowControl(int iResult);
    void LoadStrategy();

signals:
	void TraderStatusUpdated(QString message);
	void TraderBalanceUpdated(double balance, double closeProfit, double positionProfit);
    void EventTableUpdated(QString message);
    void TraderLogined();

public slots:
	void ReqConnect();
    void ReqDisconnect();
	///�û���¼����
	void ReqUserLogin();
	void RspUserLogin(int front_id ,int session_id, int iNextOrderRef);
	///Ͷ���߽�����ȷ��
	void ReqSettlementInfoConfirm();
	///�����ѯ�ʽ��˻�
	void ReqQryTradingAccount();
	void RspQryTradingAccount(double balance, double closeProfit, double positionProfit);


	///����¼������
	void ReqOrderInsert(TThostFtdcInstrumentIDType INSTRUMENT_ID, TThostFtdcDirectionType DIRECTION, 
        TThostFtdcVolumeType VOLUME, TThostFtdcPriceType LIMIT_PRICE, TThostFtdcOffsetFlagType OFFSET_FLAG);
	///������������
	void ReqOrderAction(CThostFtdcOrderField *pOrder);
	// �Ƿ��ҵı����ر�
	bool IsMyOrder(CThostFtdcOrderField *pOrder);

	// ��������
	void RspTradingAciton();
	void FollowRtnTrade(CThostFtdcTradeField tradeField);
    void ProcessTradeEvent(CThostFtdcTradeField tradeField);
};

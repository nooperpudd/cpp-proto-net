#pragma once

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DBWORD;

struct CThostFtdcDepthMarketDataField;

struct	TFTDCHeader
{
	BYTE	Version;			/**< �汾��	1	�������޷���������Ŀǰ�汾Ϊ1*/
	BYTE	Chain;				/**< ������	1	ASCII���ַ���*/
	WORD	SequenceSeries;		/**< ��������	2	�������޷��Ŷ�������*/
	DBWORD	TransactionId;		/**<��TID��	FTD��Ϣ��������	4	�������޷���������*/
	DBWORD	SequenceNumber;		/**<��SeqNo��	���к�	4	�������޷���������*/
	WORD	FieldCount;			/**< ����������	2	�������޷��Ŷ�������*/
	WORD	FTDCContentLength;	/**< FTDC��Ϣ���ĳ���	2	�������޷��Ŷ����������ֽ�Ϊ��λ��*/
	DBWORD	RequestId;			/**< ������(�ɷ���������ά����Ӧ���л����)  4 �������޷���������*/
};

struct TFieldHeader
{
	WORD FieldID;
	WORD Size;
};

#pragma pack(push)
#pragma pack(1)
///�������ʱ������
class CMarketDataUpdateTimeField
{
public:
	///��Լ����
	char InstrumentID[31];
	///����޸�ʱ��
	char UpdateTime[9];
	///����޸ĺ���
	int	UpdateMillisec;
	///ҵ������
	char ActionDay[9];
};

///�������ż�����
class CMarketDataBestPriceField
{
public:
	///�����һ
	double	BidPrice1;
	///������һ
	int	BidVolume1;
	///������һ
	double	AskPrice1;
	///������һ
	int	AskVolume1;
};

class CMarketDataStaticField
{
public:
	///����
	double	OpenPrice;
	///��߼�
	double	HighestPrice;
	///��ͼ�
	double	LowestPrice;
	///������
	double	ClosePrice;
	///��ͣ���
	double	UpperLimitPrice;
	///��ͣ���
	double	LowerLimitPrice;
	///�����
	double	SettlementPrice;
	///����ʵ��
	double	CurrDelta;
};

class CMarketDataLastMatchField
{
public:
	///���¼�
	double	LastPrice;
	///����
	int	Volume;
	///�ɽ����
	double	Turnover;
	///�ֲ���
	double	OpenInterest;
};

///�����������������
class CMarketDataBid23Field
{
public:
	///����۶�
	double	BidPrice2;
	///��������
	int	BidVolume2;
	///�������
	double	BidPrice3;
	///��������
	int	BidVolume3;
};


///������������������
class CMarketDataAsk23Field
{
public:
	///�����۶�
	double	AskPrice2;
	///��������
	int	AskVolume2;
	///��������
	double	AskPrice3;
	///��������
	int	AskVolume3;
};

///���������ġ�������
class CMarketDataBid45Field
{
public:
	///�������
	double	BidPrice4;
	///��������
	int	BidVolume4;
	///�������
	double	BidPrice5;
	///��������
	int	BidVolume5;
};

///���������ġ�������
class CMarketDataAsk45Field
{
public:
	///��������
	double	AskPrice4;
	///��������
	int	AskVolume4;
	///��������
	double	AskPrice5;
	///��������
	int	AskVolume5;
};
class CFTDMarketDataBaseField
{
public:
	//������
	char TradingDay[9];
	//�ϴν����
	double PreSettlementPrice;
	//������
	double PreClosePrice;
	//��ֲ���
	double PreOpenInterest;
	//����ʵ��
	double PreDelta;
};

#pragma pack(pop)

bool ParseMarketData(char* buffer, int nCount, CThostFtdcDepthMarketDataField* pDepthMarketData);
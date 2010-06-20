#ifndef _MONEYCENTER_H_
#define _MONEYCENTER_H_

#define MONEYCENTER_VERSION         1
#define DIR_MC          MY_BBS_HOME "/etc/moneyCenter/"
#define DIR_STOCK	MY_BBS_HOME "/etc/moneyCenter/stock/"
#define DIR_MC_TEMP     MY_BBS_HOME "/bbstmpfs/tmp/"
#define MC_BOSS_FILE    DIR_MC "mc_boss"

// �ؿ���ÿ�����͵ĺؿ���һ����Ŀ¼, �ؿ���ȡ{1, 2, ... , n}
#define DIR_SHOP        MY_BBS_HOME "/0Announce/game/cardshop/"
//���ֽ������
#define PRIZE_PER       1000000
#define MAX_POOL_MONEY  5000000
#define MAX_MONEY_NUM 100000000
#define STOCK_NUM            10	// ���������й�Ʊ��

#define MONEY_NAME	"��Ϳ��"		//�磺"��Ϳ��"
#define CENTER_NAME	"һ�����"		//�磺"һ����Ϳ"
#define NEWMONEY_NAME "��ʱ�"      //�������
#define ROBUNION "YJRG_Robunion"	//�ڰ�
#define BEGGAR "YJRG_Beggar"		//ؤ��
#define NEWTEST "projectB"          //������Ҳ�����
#define MAX_ONLINE 600          //��������¼�����
#define BRICK_PRICE 200         //שͷ����
#define PAYDAY 7                //����������
#define LISTNUM 20              //���а���¼��Ŀ

#ifndef IDLEN
#define IDLEN 12
#endif

struct LotteryRecord {	//��Ʊ��ע��¼
	char userid[IDLEN + 1];
	int betCount[5];
};

struct myStockUnit {	//���˳��й�Ʊ��¼��Ԫ
	time_t timeStamp;
	short stockid, status;
	int num;
};

struct BoardStock {
	char boardname[24];
	time_t timeStamp;
	int totalStockNum;
	int remainNum;
	int tradeNum;
	float weekPrice[7];
	float todayPrice[4];
	float high, low;
	short sellerNum, buyerNum;
	int status;
	int holderNum;
	int boardscore;
	int unused[5];
};	// 128 bytes per board's stock record

struct TradeRecord {
	char tradeType, stockid, invalid, temp;
	char userid[IDLEN + 1], nouse[3];
	float price;
	int num;
	int unused;
};	// 32 bytes, ���׵�

typedef struct _task {
	time_t time;
	int map_id:8, x:8, y:8, nouse:8;
	int task_flag;
} Task, *pTask;
	

typedef struct {
	char version, mutex, GetLetter, BeIn;
	int cash, credit, loan, interest;
	short robExp, policeExp, begExp, guard;	//robExp ��ʶ begExp �� 
	time_t aliveTime, freeTime;
	time_t loanTime, backTime, depositTime;
	time_t lastSalary, lastActiveTime;
	struct myStockUnit stock[STOCK_NUM];
	int selledExp;
	int health, luck;	//helath ���� luck ��Ʒ
	short Actived, pos_x:8, pos_y:8;	
	int con;		    //���ǣ������ָ��ٶ�
   	int x;
    	int xx;
	int weapon, armor;
	int WorkPoint;     
	Task task;
	int unused[7];	
} mcUserInfo;	// 256 bytes ��������

typedef struct {
	char version, closed, stockOpen, stockTime;
	unsigned char transferRate, depositRate, loanRate, xRate;
	int prize777, prize367, prizeSoccer;
	time_t start367, end367;
	time_t soccerStart, soccerEnd;
	time_t salaryStart, salaryEnd;
	time_t lastUpdateStock;
	int stockPoint[7];
	int tradeNum[7];
	long long Treasury;
	time_t openTime;
	int unused[32];
} MC_Env;	// 256 bytes moneycenter environment

int initData(int type, char *filepath);
void *loadData(char *filepath, size_t filesize);
void unloadData(void *buffer, size_t filesize);

#endif

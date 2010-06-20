/* board.c */
#ifndef __BOARD_H
#define __BOARD_H

#define BOARDAUX ".BOARDAUX"

#define BMNUM 16
struct boardheader {		/* This structure is used to hold data in */
	char filename[24];	/* the BOARDS files */
	char title[24];
	int clubnum;
	unsigned level;
	char flag;
	char secnumber1;
	char secnumber2;
	char type[5];
	char bm[BMNUM][IDLEN + 1];
	int hiretime[BMNUM];
	int board_ctime;
	int board_mtime;
	char sec1[4];
	char sec2[4];
	unsigned char limitchar;	//upper limit/100
	char flag2;
	char nouse[2];
	char unused[156];
};

struct boardmem {		/* used for caching files and boards */
	struct boardheader header;
	int lastpost;
	int total;
	short inboard;
	short bmonline;
	short bmcloak;
	int stocknum;
	int score;
	time_t wwwindext;
	int unused[8];
	char unused1[2];
	unsigned char unused3:4, wwwlink:1, wwwbkn:1, wwwicon:1, wwwindex:1;
	char ban;
};

struct hottopic {
	int thread;
	int num;		//��������
	char title[60];
};

struct lastmark {
	int thread;
	char marked;
	char nouse[3];
	char title[60];
	char authors[60];	//�����ж�����
};

struct relateboard {
	char filename[24];
	char title[24];
};

#define MAXHOTTOPIC 5
#define MAXLASTMARK 9
#define MAXTOPFILE 5
#define MAXRELATE 6

struct boardaux {		// used to save lastmark/hottopic/fix top
	int nhottopic;		// # of hot topics, www only.
	struct hottopic hottopic[MAXHOTTOPIC];	// hot topics
	int nlastmark;		// # of last marked files, www only.
	struct lastmark lastmark[MAXLASTMARK];	// hot last mark files
	int ntopfile;		// # of fix top files, both www and telnet
	struct fileheader topfile[MAXTOPFILE];	// fix top files
	int nousea[5];
	int nrelate;		// relating boards, www only.
	struct relateboard relate[MAXRELATE];	// relating boards
	char intro[200];	// board introduction, www only.
	char nouse[1440];	//total 4k
};

struct boardmanager {		/* record in user directionary */
	char board[24];
	char bmpos;
	char unused;
	short bid;
};

struct myparam1 {		/* just use to pass a param to fillmboard() */
	struct userec user;
	int fd;
	short bid;
};

struct bm_eva {	// for BM evaluation weekly
	char userid[IDLEN + 1];
	char week, total_week;
	char ave_score;
	short weight;
	char leave;
	char last_pass;
	int nouse;
};

struct board_bmstat {
        char boardname[24];
        struct bm_eva bm[BMNUM];
        int boardscore;
        char sec;
        char unused[7];
};

// ���ư���Ĵ���, ȡ��fb2000.dhs.org.     --ecnegrevid
// �����Զ����ղؼ�Ŀ¼. --enhanced by cojie  2005.6

// �ղؼ�Ŀ¼�Ļ���˼·  --by Fishingsnow 2006.10
// ÿ���û����ղؼз��ڼ�Ŀ¼��.favorboard����(ԭ�ȵ�����Ŀ¼����.goodbrd)����
// ��Ҫ�ṹ����struct FolderSet��
// ÿ��Ŀ¼��struct SubFolder folder[FOLDER_NUM]�����е�һ��Ԫ��
// ����folder[0]�Ǹ�Ŀ¼
// ��FolderSet�е�boards[FAVOR_BRD_NUM + 1][20]��һ�����Ա�
// ���������б��ղصİ���
// ��һ�����汻�ղص�ĳ��Ŀ¼folder[i]��ʱ��
// ������folder[i]�е�bidx�����������һ������
// ������ֱ�ʾ���������FolderSet�ṹ��boards[]�����ж�Ӧ��λ��
// �༴��ÿ����Ŀ¼�����������������ƣ���ֻ����boards[]�����е�λ��
// ͬ���أ�boards[]����Ҳ����Ӧ�ղؼе����˽ṹ

#define FAVOR_BRD_NUM	99	//����ղذ�����
#define FOLDER_BRD_NUM	32	//ÿ��Ŀ¼����ղذ�����
#define FOLDER_NUM 	16	//���Ŀ¼��,����0Ϊ��Ŀ¼
#define FAVOR_UPGRADE	1	//�����Ƿ��������ղؼ�
#define FAVOR_DEFAULT	"BBSHelp"
/** ���һ·��ɫĬ���ղؼ� 
* Ԥ��7��λ�á���Ҫʱ��ע�ʹ�
**/
#define FAVOR_YILU_DEFAULT_1	"triangle"
#define FAVOR_YILU_DEFAULT_2	"joke"
#define FAVOR_YILU_DEFAULT_3	"Movie"
#define FAVOR_YILU_DEFAULT_4	"music"
#define FAVOR_YILU_DEFAULT_5	"familylife"
#define FAVOR_YILU_DEFAULT_6	"job"
#define FAVOR_YILU_DEFAULT_7	"stock"
#define FAVOR_YILU_DEFAULT_8	"AdvancedEdu"
#define FAVOR_YILU_DEFAULT_9	"Game"
#define FAVOR_YILU_DEFAULT_10	"China_news"
#define FAVOR_YILU_DEFAULT_11	"Test"
#define FAVOR_YILU_DEFAULT_12	"sysop"
//#define FAVOR_YILU_DEFAULT_13	"triangle"
//#define FAVOR_YILU_DEFAULT_14	"joke"
//#define FAVOR_YILU_DEFAULT_15	"Movie"
//#define FAVOR_YILU_DEFAULT_16	"music"
//#define FAVOR_YILU_DEFAULT_17	"familylife"
//#define FAVOR_YILU_DEFAULT_18	"job"
//#define FAVOR_YILU_DEFAULT_19	"stock"
#define FAVOR_FILE	".favor"
#define FAVOR_OLDFILE1	".goodbrd"
#define FAVOR_OLDFILE2  ".favorboard"

struct SubFolder {
	char name[20];
	char desc[32];
	short bidx[FOLDER_BRD_NUM];	//��������,ָ��FolderSet->boards[]
	int boardNum;			//��Ŀ¼�İ�����
	int unused[2];
};	// 128 bytes

struct FolderSet {
	int folderNum;		//��Ŀ¼��(���������Ϊ0�ĸ�Ŀ¼)
	int boardNum;		//�ܰ�����
	struct SubFolder folder[FOLDER_NUM];
	char boards[FAVOR_BRD_NUM + 1][20];	//�����б�, 0��������
	int unused[10];
};	// 4096 bytes


char *bm2str(char *buf, struct boardheader *bh);
char *sbm2str(char *buf, struct boardheader *bh);
int chk_BM(struct userec *, struct boardheader *bh, int isbig);
int chk_BM_id(char *, struct boardheader *);
int dosetbmstatus(struct boardmem *bcache, char *userid, int online, int visible);
int bmfilesync(struct userec *);
int fillmboard(struct boardheader *bh, struct myparam1 *param);
int getlastpost(char *board, int *lastpost, int *total);
struct boardaux *getboardaux(int bnum);
int addtopfile(int bnum, struct fileheader *fh);
int deltopfile(int bnum, int num);
int updateintro(int bnum, char *filename);

struct FolderSet * _loadFavorFolder(char *userid);
int _unloadFavorFolder(struct FolderSet *Favor);
int _addFavorBoard(char *bname, struct FolderSet *Favor, int currFolder);
int _addFavorFolder(char *foldername, char *desc, struct FolderSet *Favor);
int _delFavorBoard(char *bname, struct FolderSet *Favor, 
		int currFolder, int keepLastBoard);
int _delFavorFolder(struct FolderSet *Favor, int currFolder);
int _moveFavorFolder(struct FolderSet *Favor, int currFolder, int tagFolder);
int _editFavorFolder(struct FolderSet *Favor, int currFolder, 
		char *name, char *desc);
int _inCurrFolder(char *bname, struct FolderSet *Favor, int currFolder);
int _inFavorBoards(char *bname, struct FolderSet *Favor);
int _fixFavorFolder(struct FolderSet *Favor);

#endif

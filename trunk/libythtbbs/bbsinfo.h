#ifndef __BBSINFO_H
#define __BBSINFO_H
#define USHM_SIZE       (MAXACTIVE + 10)
#define UTMPHASH_SIZE 4099

#include "uhashgen.h"

#define BBSLOG_MSQ	3333
#define BBSEVA_MSQ	3334
#define YFTP_SEM	1111
#define YFTP_SHM	1111
#define BCACHE_SHM	7813
#define UCACHE_SHM	7912
#define UCACHE_HASH_SHM	7911
#define UTMP_SHM	3785
#define UTMP_HASH_SHM	3786
#define UINDEX_SHM	3787
#define ACBOARD_SHM	9014
#define GOODBYE_SHM	5003
#define ENDLINE_SHM	5006
#define GAMEROOM_SHM	3451
#define BLOGLOG_MSQ     8888
#define FRIENDS_MSQ	9527
#define KILLER_SHM	9578
#define AID_MSQ		9999
#define WWWCACHE_SHM	37788



struct ucache_hashtable {
	int hash[UCACHE_HASHLINE][26];
};

struct UTMPFILE {
	struct user_info uinfo[USHM_SIZE];
	unsigned short guesthash_head[UTMPHASH_SIZE + 1];
	unsigned short guesthash_next[USHM_SIZE];
	time_t uptime;
	unsigned short activeuser;
	unsigned short maxuser;	//add by gluon
	unsigned short maxtoday;
	unsigned short wwwguestnum;
	time_t activetime;	//time of updating activeuser
	int kickout_time;
	char syncbmonline;
	char nouse[3];
	time_t watchman;
	unsigned int unlock;
	int ave_score;
	int unused[24];
};

struct BCACHE {
	struct boardmem bcache[MAXBOARD];
	int number;
	time_t uptime;
	time_t pollvote;
	int unused[20];
};

struct UCACHEHASH {
	int hash_head[UCACHE_HASHSIZE + 1];
	//next�� MAXUSERS + 1 ��Ԫ��
	//���� next[0] ��δ���� hash ����
	//ȡ������¼Ŀǰ���û���
	//MAXUSERS - next[0] ��Ϊ��ǰע���û���
	int next[MAXUSERS + 1];
	struct ucache_hashtable hashtable;
	time_t uptime;
	int regkey[4];
	int oldregkey[4];
	time_t keytime;
	int nouse[1];
};

struct UINDEX {
	unsigned short user[MAXUSERS][6];	//�����www�ж϶��¼�Ļ����Ƿ�ʹ���޳���telnet�е�5, ���6
	int nouse[10];
};

#define MAX_PROXY_NUM 4

struct WWWCACHE {
	time_t www_version;
	unsigned int www_visit;
	unsigned int home_visit;
	struct in_addr accel_addr;
	unsigned int accel_port;
	unsigned int validproxy[MAX_PROXY_NUM];
	struct in_addr text_accel_addr;
	unsigned int text_accel_port;
	int nouse[21];
};

struct bbsinfo {
	struct BCACHE *bcacheshm;
	struct boardmem *bcache;
	struct UCACHEHASH *ucachehashshm;
	struct UTMPFILE *utmpshm;
	struct UINDEX *uindexshm;
	struct WWWCACHE *wwwcache;
};

key_t getBBSKey(int projID);
int initbbsinfo(struct bbsinfo *bbsinfo);
void update_max_online();
int insertuseridhash(const char *userid, const int num, const int tohead);
int finduseridhash(const char *userid);
int deluseridhash(const char *userid);
int usersum();
int ucache_hashinit();
int load_ucache();
char *u_namearray(char buf[][IDLEN + 1], int max, int *pnum, char *tag,
		  char *atag, int *full);
time_t uhash_uptime();
struct user_info *queryUIndex(int uid, struct user_info *reader, int pid,
			      int *pos);
unsigned int ucache_hash(const char *userid);
int utmp_login(struct user_info *up);
int utmp_logout(int *utmpent, struct user_info *up);
#endif

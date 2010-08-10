#include <stdio.h>
#include <ctype.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

#include "ythtbbs.h"
#include "discuzsql.h"
#include "discuzmodule.h"
#include <iconv.h>    // convert gb2312 to utf8
#include "bbs.h"

static int isoverride(struct override *o, char *id);

char *
sethomepath(char *buf, const char *userid)
{
	sprintf(buf, MY_BBS_HOME "/home/%c/%s", mytoupper(userid[0]), userid);
	return buf;
}

char *
sethomefile(char *buf, const char *userid, const char *filename)
{
	sprintf(buf, MY_BBS_HOME "/home/%c/%s/%s", mytoupper(userid[0]), userid,
		filename);
	return buf;
}

char *
setmailfile(char *buf, const char *userid, const char *filename)
{
	sprintf(buf, MY_BBS_HOME "/mail/%c/%s/%s", mytoupper(userid[0]), userid,
		filename);
	return buf;
}

int
saveuservalue(char *userid, char *key, char *value)
{
	char path[256];
	sethomefile(path, userid, "values");
	return savestrvalue(path, key, value);
}

int
readuservalue(char *userid, char *key, char *value, int size)
{
	char path[256];
	sethomefile(path, userid, "values");
	return readstrvalue(path, key, value, size);
}

#if 0
char *
cuserexp(int exp)
{
	int expbase = 0;

	if (exp == -9999)
		return "û�ȼ�";
	if (exp <= 100 + expbase)
		return "������·";
	if (exp <= 450 + expbase)
		return "һ��վ��";
	if (exp <= 850 + expbase)
		return "�м�վ��";
	if (exp <= 1500 + expbase)
		return "�߼�վ��";
	if (exp <= 2500 + expbase)
		return "��վ��";
	if (exp <= 3000 + expbase)
		return "���ϼ�";
	if (exp <= 5000 + expbase)
		return "��վԪ��";
	return "��������";
}
#else
#define GROUP_NUM 24
#define	LONG_GROUP_NUM 1
char exp_des[GROUP_NUM][12][16] =
{ {"û�ȼ�",	"һ�����",	"˫ϲ����",	"����̩",	"�ļ�ƽ��",	"�帣����",	"������˳",	"���Ǹ���",	"�˷�����",	"�ž�ͬ��",	"ʮȫʮ��",	"һ����Ϳ"},
  {"���޷���", 	"�о����",	"ǧ������",	"��ͽ", 	"�Ĺ�", 	"�Ĺ�", 	"ǧ��",		"�İ�",		"����", 	"����", 	"��ʥ", 	"����"},
  {"��ѧէ��",	"�����ž�",	"��֪һ��",	"��������",	"����С��",	"��Ȼ����",	"����似",	"�����뻯",	"�Ƿ��켫",	"������˫",	"������",	"��ɲ�"},
  {"�޺�ȭ",	"������", 	"һָ��", 	"������", 	"һ��ָ",  	"���浶",	"��������",	"����ʮ����", 	"������", 	"������",	"��ħ����", 	"Ұ��ȭ"},
  {"�����伢", 	"ʳ������",	"�̲�ϡ��", 	"��������", 	"���Ѽ���", 	"������˿", 	"�����Ź�", 	"��������", 	"�������", 	"ɽ�亣ζ", 	"����ȫϯ",	"��˶���"},
  {"������ȫ",	"ѽѽѧ��", 	"�������", 	"��������", 	"��軥��", 	"��������", 	"С¥����", 	"���ߺ͹�", 	"����ɢ", 	"Ц������", 	"����֮��", 	"�˴�����"},
  {"ʯ��ʱ��",	"������ͷ",	"С�ײ�ǹ",	"�Ȼ���",	"����̹��",	"��ը��",	"Ѳ������",	"��Ǳͧ",	"����ĸ��",	"ԭ�ӵ�",	"�����ս",	"���ֲ���"},
  {"�������",	"�������",	"��������",	"�����ܲ�",	"ëëϸ��",	"���꽻��",	"�������",	"�������",	"�ȴ��籩",	"��ˮ����",	"����ĩ��",	"��ˮ�޺�"},
  {"����է��",	"����ۻ�",	"ո¶ͷ��",	"һ������",	"����ս",	"�޵�սʿ",	"��ʤ����",	"��ս���",	"����ս��",	"�������",	"��ղ���",	"ľ����"},
  {"�����ƿ�",	"����ţ��",	"��ԭ����",	"��������",	"��Хɽ��",	"��ëʨ��",	"�������",	"��������",	"������ʯ",	"ʼ����",	"��Ҷ��",	"��ϸ������"},
  {"һ�����",	"С��",		"����",	"��ϼ����",	"ţħ��",	"ɳɮ",		"�˽�",		"�����",	"��ɮ",		"��������",	"��������",	"��սʤ��"},
  {"ָ��Ϊ��",	"��÷����",	"��С�޲�",	"��ǰ����",	"��������",	"����ɽ��",	"��������",	"�»����",	"�ྴ���",	"�ٰ���ü",	"��ͷ����",	"����˫��"},
  {"�ຣ����",	"�����ʽ�",	"�¿��涡",	"��������",	"�ε�Ӱֻ",	"��������",	"�������",	"�ž�����",	"�¶�����",	"��Ȼ����",	"��ɷ����",	"��������"},
  {"�б�",	"��ξ",		"��ξ",		"��ξ",		"��У",		"��У",		"��У",		"��У",		"�ٽ�",		"�н�",		"�Ͻ�",		"��ί��ϯ"},
  {"����",	"��",		"ʶ",		"��ɫ",		"����",		"��",		"��",		"��",		"ȡ",		"��",		"��",		"����"},
  {"���ʽ���",	"��סĸ̥",	"Բ������",	"��浼���",	"��������",	"�Ӽҳ���",	"�п�����",	"�������",	"����ħ��",	"�����Ⱦ�",	"ת���",	"�������"},
  {"С����",	"�׶�԰",	"ѧǰ��",	"Сѧ��",	"������",	"������",	"��ѧ��",	"˶ʿ��",	"��ʿ��",	"��ʿ��",	"����",		"Ժʿ"},
  {"û�ȼ�", 	"������·", 	"һ��վ��", 	"�м�վ��", 	"�߼�վ��", 	"��վ��", 	"���ϼ�", 	"��վԪ��", 	"��������",	"��������",	"��������",	"��������"},
  {"������",	"һ�ľ�",	"������",	"����԰",	"�ļ���",	"�����",	"����˳",	"��ƥ��",	"���ɵ�",	"������",	"ʮ����",	"��һ��"},
  {"СŮ��",	"�Ͻ�Ů",	"������",	"�Ƶ���",	"������",	"�����",	"���Ѿ�",	"����",		"��ʩ",		"��ľ��",	"�¹�Ӣ",	"�绨"},
  {"", "","","","","","","","","","",""},
#if 1
  {"����",	"����",		"����",		"����",		"����",		"�Ϸ�",		"����",		"����",		"����",		"����",		"�װ�",		"ȸ��"},
  {"����",	"������",	"����",		"Ѽ����",	"˫����",	"Ѹ��",		"����",		"����",		"������",	"�ƽ���",	"�л�ʥ��",	"�����"},
#endif
  {"٪�޼�",    "��",           "��ʹ",         "����ʹ",       "Ȩ��ʹ",       "����ʹ",       "����ʹ",       "����ʹ",	"����ʹ",       "����ʹ",       "����ʹ",       "��"}
};
#if 1	//5555555555555:(
char long_exp_des[LONG_GROUP_NUM][101][16] =
{{"����", 	"���ִꪳ���", "������" , 	"���ָ�ξ", 	"������", 	"����Уξ", 	"������", 	"����ִꪳ���", "���¸�ξ", 	"����Уξ",
 "������" , 	"������", 	"���˾��",	"���긱ξ", 	"�з���", 	"����Уξ", 	"������", 	"����˾��", 	"���ڸ�ξ", 	"������", 
 "����Уξ", 	"������", 	"����к�", 	"��⸱ξ", 	"��ɢ��", 	"���Уξ", 	"����ξ", 	"������", 	"�����к�", 	"�¹���ξ", 
 "������", 	"�¹�Уξ", 	"����ξ", 	"ֱͨ��", 	"���˾��", 	"������ξ", 	"������", 	"����Уξ", 	"����ξ", 	"������", 
 "����˾��", 	"���丱ξ", 	"������", 	"����Уξ", 	"����ξ", 	"��ɢ���", 	"����ɽ�", 	"�λ�����", 	"������", 	"���ｫ��", 
 "�ﶼξ", 	"������", 	"�����ɽ�", 	"��Զ����", 	"��ɢ���", 	"��Զ����", 	"���ﶼξ", 	"�д��", 	"������ɽ�", 	"��������", 
 "̫�д��", 	"��������", 	"�ᳵ��ξ", 	"ͨ����", 	"�������ɽ�", 	"׳�佫��", 	"������", 	"���佫��", 	"���ᳵ��ξ", 	"�����»���",
 "��´󽫾�", "���⽫��", 	"����", 	"��½���", 	"��������", 	"�ϻ���", 	"���Ϲ�»���", "�����󽫾�", 	"�ھ��󽫾�", 	"����", 
 "��»���", 	"����󽫾�", 	"�ؽ�", 	"������", 	"�����󽫾�", 	"������ͬ��˾", "����󽫾�", 	"��������", 	"��������", 	"�����ز�",
 "�����غ�", 	"�����ع�", 	"��������", 	"����", 	"����", 	"������", 	"�⽮��", 	"������", 	"����", 	"�ʵ�"}
 };
#endif

char exp_buf[20];
	
char *
cuserexp( int group, int exp)
{
	int lvl, expbase = 0;
	
	if (exp < 0)
		return	(exp_des[0][0]);
	for(lvl=0; exp > 5*lvl*lvl+15*lvl + expbase; lvl++ );

	if(lvl>100)
		lvl = 100;
	

	if(group == 'U')
		return "����";
	else if(group>='A' && group<('A'+GROUP_NUM))
		sprintf(exp_buf,"%d�� %s",lvl, exp_des[group-'A'][lvl/10 + 1] );
#if 1	//555555555555:(
	else if(group == '1')
		return( long_exp_des[group-'1'][lvl-1] );
#endif
	else
		sprintf(exp_buf,"%d�� %s", lvl, exp_des[0][lvl/10 + 1]);
	
	return	exp_buf;
}

#endif

char *
cperf(int perf)
{
	if (perf == -9999)
		return "û�ȼ�";
	if (perf <= 5)
		return "�Ͽ����";
	if (perf <= 12)
		return "Ŭ����";
	if (perf <= 35)
		return "������";
	if (perf <= 50)
		return "�ܺ�";
	if (perf <= 90)
		return "�ŵ���";
	if (perf <= 140)
		return "̫������";
	if (perf <= 200)
		return "��վ֧��";
	if (perf <= 500)
		return "�񡫡�";
	return "�����ˣ�";
}
#if 0
int
countexp(struct userec *urec)
{
	int exp;

	if (!strcmp(urec->userid, "guest"))
		return -9999;
	if(urec->exp_group == 'U')
		return 0;
	exp =
	    urec->numposts/5 + urec->numlogins + (time(0) -
						    urec->firstlogin) / 86400 +
	    urec->stay / 1800 + urec->extraexp1 * 20;
	return exp > 0 ? exp : 0;
}
#else
int
countexp(struct userec *urec, int level)
{      //level 0:��ͳ����ֵ 1:����ֵ 2:�ϼ�
       int exp1 = 0, exp2 = 0;
       struct userdata currentdata;

       if (!strcmp(urec->userid, "guest"))
               return -9999;
       if(level == 0 || level == 2)    //��ͳ����ֵ
               exp1 = urec->numposts/5 + urec->numlogins + (time(0) -
                       urec->firstlogin) / 86400 + urec->stay / 1800;
       if(level == 1 || level == 2){   //����ֵ
               loaduserdata(urec->userid, &currentdata );
               if(currentdata.extraexp > 100000000){	//������һ�£�һ��û��bt����1y��
               		currentdata.extraexp = 0;
               		saveuserdata(urec->userid, &currentdata );
               	}
               exp2 = urec->extraexp1 * 20 + currentdata.extraexp;
       }

       return (exp1+exp2) > 0 ? (exp1+exp2) : 0;
}
#endif

int
countperf(struct userec *urec)
{
	int perf;
	int reg_days;

	if (!strcmp(urec->userid, "guest"))
		return -9999;
	reg_days = (time(0) - urec->firstlogin) / 86400 + 1;
	perf =
	    ((float) (urec->numposts) / (float) urec->numlogins +
	     (float) urec->numlogins / (float) reg_days) * 10;
	return perf > 0 ? perf : 0;
}

int
countlife(struct userec *urec)
{
	int value;

	/* if (urec) has XEMPT permission, don't kick it */
	if ((urec->userlevel & PERM_XEMPT)
	    || strcmp(urec->userid, "guest") == 0)
		return 999;
	value = (time(0) - urec->lastlogin) / 60;	/* min */
#if 0
	if (urec->numlogins <= 3)
		return (15 * 1440 - value) / 1440;
#endif
	if (!(urec->userlevel & PERM_LOGINOK))
		return (30 * 1440 - value) / 1440;
	return (120 * 1440 - value) / 1440 + min(urec->numdays,
						 (unsigned short) 800);
}

int
userlock(char *userid, int locktype)
{
	char path[256];
	int fd;
	sethomefile(path, userid, ".lock");
	fd = open(path, O_RDONLY | O_CREAT, 0660);
	if (fd == -1)
		return -1;
	flock(fd, locktype);
	return fd;
}

int
userunlock(char *userid, int fd)
{
	flock(fd, LOCK_UN);
	close(fd);
	return 0;
}

static int
checkbansitefile(const char *addr, const char *filename)
{
	FILE *fp;
	char temp[STRLEN];
	if ((fp = fopen(filename, "r")) == NULL)
		return 0;
	while (fgets(temp, STRLEN, fp) != NULL) {
		strtok(temp, " \n");
		if ((!strncmp(addr, temp, 16))
		    || (!strncmp(temp, addr, strlen(temp))
			&& temp[strlen(temp) - 1] == '.')
		    || (temp[0] == '.' && strstr(addr, temp) != NULL)) {
			fclose(fp);
			return 1;
		}
	}
	fclose(fp);
	return 0;
}

int
checkbansite(const char *addr)
{
	return checkbansitefile(addr, MY_BBS_HOME "/.bansite")
	    || checkbansitefile(addr, MY_BBS_HOME "/bbstmpfs/dynamic/bansite");
}

int
userbansite(const char *userid, const char *fromhost)
{
	char path[STRLEN];
	FILE *fp;
	char buf[STRLEN];
	int i, deny;
	char addr[STRLEN], mask[STRLEN], allow[STRLEN];
	char *tmp[3] = { addr, mask, allow };
	unsigned int banaddr, banmask;
	unsigned int from;
	from = inet_addr(fromhost);
	sethomefile(path, userid, "bansite");
	if ((fp = fopen(path, "r")) == NULL)
		return 0;
	while (fgets(buf, STRLEN, fp) != NULL) {
		i = mystrtok(buf, ' ', tmp, 3);
		if (i == 1) {	//���� ip
			banaddr = inet_addr(addr);
			banmask = inet_addr("255.255.255.255");
			deny = 1;
		} else if (i == 2) {
			banaddr = inet_addr(addr);
			banmask = inet_addr(mask);
			deny = 1;
		} else if (i == 3) {	//�� allow ��
			banaddr = inet_addr(addr);
			banmask = inet_addr(mask);
			deny = !strcmp(allow, "allow");
		} else		//���У�
			continue;
		if ((from & banmask) == (banaddr & banmask)) {
			fclose(fp);
			return deny;
		}
	}
	fclose(fp);
	return 0;
}

void
logattempt(const char *user, char *from, char *zone, time_t time)
{
	char buf[256], filename[80];
	int fd, len;

	sprintf(buf, "system passerr %s", from);
	newtrace(buf);
	snprintf(buf, 256, "%-12.12s  %-30s %-16s %-6s\n",
		 user, Ctime(time), from, zone);
	len = strlen(buf);
	if ((fd =
	     open(MY_BBS_HOME "/" BADLOGINFILE, O_WRONLY | O_CREAT | O_APPEND,
		  0644)) >= 0) {
		write(fd, buf, len);
		close(fd);
	}
	sethomefile(filename, user, BADLOGINFILE);
	if ((fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644)) >= 0) {
		write(fd, buf, len);
		close(fd);
	}
}

static int
isoverride(struct override *o, char *id)
{
	if (strcasecmp(o->id, id) == 0)
		return 1;
	return 0;
}

int
inoverride(char *who, char *owner, char *file)
{
	char buf[80];
	struct override o;
	sethomefile(buf, owner, file);
	if (search_record(buf, &o, sizeof (o), (void *) isoverride, who) != 0)
		return 1;
	return 0;
}

int
saveuserdata(char *uid, struct userdata *udata)
{
	char newfname[80];
	char fname[80];
	int fd;
	sethomefile(newfname, uid, "newuserdata");
	sethomefile(fname, uid, "userdata");
	fd = open(newfname, O_WRONLY | O_CREAT, 0660);
	if (fd == -1) {
		errlog("open userdata error %s", uid);
		return -1;
	}
	write(fd, udata, sizeof (struct userdata));
	close(fd);
	rename(newfname, fname);
	return 0;
}

int
loaduserdata(char *uid, struct userdata *udata)
{
	char buf[80];
	int fd;
	sethomefile(buf, uid, "userdata");
	bzero(udata, sizeof (struct userdata));
	fd = open(buf, O_RDONLY);
	if (fd == -1) {
		return -1;
	}
	read(fd, udata, sizeof (struct userdata));
	close(fd);
	return 0;
}

int
lock_passwd()
{
	int lockfd;

	lockfd = open(MY_BBS_HOME "/.PASSWDS.lock", O_RDWR | O_CREAT, 0600);
	if (lockfd < 0) {
		errlog("uhash lock err: %s", strerror(errno));
		exit(1);
	}
	flock(lockfd, LOCK_EX);
	return lockfd;
}

int
unlock_passwd(int fd)
{
	flock(fd, LOCK_UN);
	close(fd);
	return 0;
}

/*���ز����passwdλ�ã���һ�����Ϊ 1
  id�Ѿ���ע�ᣬ���� 0
  ����ʧ�ܣ����� -1
*/
int
insertuserec(const struct userec *x)
{
	int lockfd, fd;
	int i;
	lockfd = lock_passwd();
	if (finduseridhash(x->userid) > 0) {
		unlock_passwd(lockfd);
		return 0;
	}
	i = deluseridhash("");
	if (i <= 0) {
		unlock_passwd(lockfd);
		return -1;
	}
	fd = open(".PASSWDS", O_WRONLY);
	if (fd == -1) {
		insertuseridhash("", i, 1);
		unlock_passwd(lockfd);
		return -1;
	}
	if (lseek(fd, (i - 1) * sizeof (struct userec), SEEK_SET) == -1) {
		insertuseridhash("", i, 1);
		close(fd);
		unlock_passwd(lockfd);
		return -1;
	}
	if (write(fd, x, sizeof (struct userec)) != sizeof (struct userec)) {
		close(fd);
		unlock_passwd(lockfd);
		errlog("can't write!");
		return -1;
	};
	close(fd);
	insertuseridhash(x->userid, i, 1);
	unlock_passwd(lockfd);
	return i;
}

//����userid�����urecָ�뵽��ָ��passwdptr����Ӧ��Ŀ���ýṹֻ��
//���ض�Ӧƫ��������һ�����Ϊ 1
//����û��Ѿ�kickout����Ҳ�Ҳ������Ҳ������� 0
int
getuser(const char *userid, struct userec **urec)
{
	int i;
	if (userid[0] == 0 || strchr(userid, '.')) {
		if (urec != NULL)
			*urec = NULL;
		return 0;
	}
	i = finduseridhash(userid);
	if (i > 0 && i <= MAXUSERS
	    && !strcasecmp(passwdptr[i - 1].userid, userid)
	    && !(passwdptr[i - 1].kickout)) {
		if (urec != NULL) {
			*urec = (struct userec *) (passwdptr + (i - 1));
		}
		return i;
	}
	return 0;
}

//����userid���������ݿ⣬�����Ҫ����ʹ��checkdiscuzpasswd()ƥ������
//���û�У����û��Ѿ�kickout����Ҳ�Ҳ������Ҳ������� 0
//����У��ο�register������д��.PASSWDS
int
checkdiscuzuser(const char *userid)
{
	char sqlbuf[512];
	char useridutf8[22];
	MYSQL *mysql = NULL;
	MYSQL_RES *res;

	mysql = mysql_init(mysql);
    mysql = mysql_real_connect(mysql,"localhost",SQLUSER, SQLPASSWD, SQLDB,0, NULL,0);
	if (!mysql) {
		perror("Can not open database\n");
		return -1;
	}
	if(code_convert("gbk","utf8",userid, strlen(userid),useridutf8, 22 )==-1)   // 21 = (IDLEN+2)/2*3   longest utf-8
	{
		printf("convert username error \n");
		mysql_close(mysql);
		return -1;
	}
	mysql_query(mysql, "set names utf8");
	sprintf(sqlbuf,"select username from pre_ucenter_members where username = \'%s\'; " , useridutf8 );
	mysql_query(mysql, sqlbuf);
	res = mysql_store_result(mysql);

	if (mysql_num_rows(res)!=0)
	{
		// userid already in discuz
		mysql_free_result(res);
		mysql_close(mysql);
		return 1;
	}
	else
	{
		mysql_free_result(res);
		mysql_close(mysql);
		return 0;
	}
}

int
checkdiscuzpasswd(char *userid, const char *passbuf)
{
	// userid will update because of Capital letter issues
	char discuzuseridtrans[22];

	char sqlbuf[512];
	char useridutf8[22];
	char discuzpassmd5[DISCUZ_PASSWD_LENGTH + 1];
	MYSQL *mysql = NULL;
	MYSQL_RES *res;
	MYSQL_ROW row;
	if (userid[0] == 0 || strchr(userid, '.')) {
		return 0;
	}

	mysql = mysql_init(mysql);
    mysql = mysql_real_connect(mysql,"localhost",SQLUSER, SQLPASSWD, SQLDB,0, NULL,0);
	if (!mysql) {
		perror("Can not open database\n");
		return -1;
	}
	if(code_convert("gbk","utf8",userid, strlen(userid),useridutf8, 22 )==-1)   // 21 = (IDLEN+2)/2*3   longest utf-8
	{
		printf("convert username error \n");
		mysql_close(mysql);
		return -1;
	}
	mysql_query(mysql, "set names utf8");
	sprintf(sqlbuf,"select password,salt,username from pre_ucenter_members where username = \'%s\'; " , useridutf8 );
	mysql_query(mysql, sqlbuf);
	res = mysql_store_result(mysql);

	if (mysql_num_rows(res)!=0)
	{
		// userid already in discuz
		row = mysql_fetch_row(res);  // row[0] is password , row[1] is salt, row[2] is username in discuz
		gendiscuzpasswd(discuzpassmd5, row[1], passbuf);
/*		// for log
		char logfile[256], errorlog[256];
			sprintf(logfile, "/home/bbs/reclog/telnet.log");
			sprintf(errorlog, "discuzpass=%s  genpass=%s  originalpass=%s\n",
					row[1], discuzpassmd5, passbuf);
			writelog(logfile, errorlog);
		// logend
*/
		// update user name to make sure it's the same (not capitalized) as the one in discuz
		if(code_convert("utf8","gbk",row[2],strlen(row[2]),discuzuseridtrans, 16 )==-1)   // its userid should be less than 16
		{
			printf("convert username error \n");
			mysql_close(mysql);
			return -1;
		}
		sprintf(userid, "%s", discuzuseridtrans);
		return !memcmp(row[0], discuzpassmd5, DISCUZ_PASSWD_LENGTH);
	}
	else
		mysql_free_result(res);
		mysql_close(mysql);
		return 0;
}



//����uid�����urecָ�뵽��ָ��passwdptr����Ӧ��Ŀ���ýṹֻ��
//���ض�Ӧƫ��������һ�����Ϊ 1
//����û��Ѿ�kickout����Ҳ�Ҳ������Ҳ������� 0
int
getuserbynum(const int uid, struct userec **urec)
{
	if (uid > 0 && uid <= MAXUSERS && !(passwdptr[uid - 1].kickout)) {
		if (urec != NULL) {
			*urec = (struct userec *) (passwdptr + (uid - 1));
		}
		return uid;
	}
	return 0;
}
char *
getuseridbynum(const int uid) {
	struct userec *urec;
	if (getuserbynum(uid, &urec) <= 0)
		return 0;
	else
		return urec->userid;
}

int login_get_user(const char *user, struct userec **urec)
{
	if(isdigit(*user))
		return getuserbynum(atoi(user), urec);
	return getuser(user, urec);
}

//�ҵ�userid����passwd��λ��, ��һ��λ�ñ��Ϊ 1
//��kickout�򷵻ظ�ֵ
//�������û�ע��ʱԤ���Ƿ����ע��
//�Ҳ������� 0 
int
user_registered(const char *userid)
{
	int i;
	if (userid[0] == 0 || strchr(userid, '.')) {
		return 0;
	}
	i = finduseridhash(userid);
	if (i > 0 && i <= MAXUSERS
	    && !strcasecmp(passwdptr[i - 1].userid, userid)) {
		if (passwdptr[i - 1].kickout)
			return -i;
		else
			return i;
	}
	return 0;
}

//kickout����hash��ɾ���û����û�flags[0] KICKOUT_FLAGλ ��1
//dietime��Ϊ��ǰʱ��
//���޸�hash
//24Сʱ�󣬵���deluserec��passwd��ɾ��
int
kickoutuserec(const char *userid)
{
	int i;
	struct userec tmpuser;
	time_t now_t;
	int fd;
	char buf[1024];
	i = finduseridhash(userid);
	if (i <= 0 || i > MAXUSERS) {	//����hashʧ��
		return -1;
	}
	if (passwdptr[i - 1].kickout)	//�Ѿ���kick��
		return 0;
	now_t = time(NULL);
	memcpy(&tmpuser, &(passwdptr[i - 1]), sizeof (struct userec));
	tmpuser.kickout = now_t;
	fd = open(".PASSWDS", O_WRONLY);
	if (fd == -1) {
		return -1;
	}
	if (lseek(fd, (i - 1) * sizeof (struct userec), SEEK_SET) == -1) {
		close(fd);
		return -1;
	}
	sprintf(buf, "mail/%c/%s", mytoupper(userid[0]), userid);
	//prints("%s",buf);
	deltree(buf);
	sprintf(buf, "home/%c/%s", mytoupper(userid[0]), userid);
	deltree(buf);
	write(fd, &tmpuser, sizeof (struct userec));
	close(fd);
	return 0;
}

//��passwd��hash��ɾ����Ŀ
//�ɹ����� 0��ʧ�ܷ��� -1
int
deluserec(const char *userid)
{
	int i;
	time_t now_t;
	struct userec tmpuser;
	int fd, lockfd;
	lockfd = lock_passwd();
	i = finduseridhash(userid);
	if (i <= 0 || i > MAXUSERS) {
		errlog("del user error %d", i);
		unlock_passwd(lockfd);
		return 0;
	}
	now_t = time(NULL);
	if (!(passwdptr[i - 1].kickout)
	    || (now_t - passwdptr[i - 1].kickout <= 86400 - 3600)) {
		errlog("del user error 2 %d", (int) passwdptr[i - 1].kickout);
		unlock_passwd(lockfd);
		return 0;
	}
	if (deluseridhash(userid) <= 0) {
		errlog("del user error 3 %s", userid);
		unlock_passwd(lockfd);
		return -1;
	}
	fd = open(".PASSWDS", O_WRONLY);
	if (fd == -1) {
		insertuseridhash(userid, i, 1);
		errlog("del user error open");
		unlock_passwd(lockfd);
		return -1;
	}
	if (lseek(fd, (i - 1) * sizeof (struct userec), SEEK_SET) == -1) {
		insertuseridhash(userid, i, 1);
		errlog("del user error seek");
		close(fd);
		unlock_passwd(lockfd);
		return -1;
	}
	bzero(&tmpuser, sizeof (struct userec));
	write(fd, &tmpuser, sizeof (struct userec));
	close(fd);
	insertuseridhash("", i, 1);
	unlock_passwd(lockfd);
	return 0;
}

/* �޸� x->userid ��Ӧ��ĿΪ x
 * ���޸Ĳ��޸� id
 * Ҳ���޸� hash
 * ��� usernum Ϊ 0����hash�в��ң�����ֱ�Ӽ���Ӧλ���Ƿ��Ǹ��û�
 * usernum�� 1 ��ʼ����
 */
int
updateuserec(const struct userec *x, const int usernum)
{
	int i = usernum, fd;
	if (i == 0)
		i = finduseridhash(x->userid);
	if (i <= 0 || i > MAXUSERS
	    || strcasecmp(passwdptr[i - 1].userid, x->userid)) {
		errlog("update user error. %s %s", passwdptr[i - 1].userid,
		       x->userid);
		return -1;
	}
	if (passwdptr[i - 1].kickout) {
		return -1;
	}
	fd = open(".PASSWDS", O_WRONLY);
	if (fd == -1) {
		return -1;
	}
	if (lseek(fd, (i - 1) * sizeof (struct userec), SEEK_SET) == -1) {
		close(fd);
		return -1;
	}
	write(fd, x, sizeof (struct userec) - sizeof (time_t));
	//FIX ME, maybe it's a bug for 64-bit mode
	close(fd);
	return 0;
}

int
apply_passwd(int (*fptr) (const struct userec *, char *), char *arg)
{
	int i;
	int ret;
	int count;
	count = 0;
	for (i = 1; i <= MAXUSERS; i++) {
		ret = (*fptr) (&(passwdptr[i - 1]), arg);
		if (ret == -1)
			break;
		if (ret == 0)
			count++;
	}
	return count;
}

int
has_fill_form(char *userid)
{
	DIR *dirp;
	struct dirent *direntp;
	FILE *fp;
	int i = 1;
	char tmp[256], buf[256], *ptr;
	dirp = opendir(SCANREGDIR);
	if (dirp == NULL)
		return 0;
	while (i) {
		if ((direntp = readdir(dirp)) != NULL)
			snprintf(tmp, 256, "%s%s", SCANREGDIR, direntp->d_name);
		else {
			snprintf(tmp, 256, "%s/new_register", MY_BBS_HOME);
			i = 0;
		}
		if ((fp = fopen(tmp, "r")) != NULL) {
			while (1) {
				if (fgets(buf, 256, fp) == 0)
					break;
				if ((ptr = strchr(buf, '\n')) != NULL)
					*ptr = '\0';
				if (strncmp(buf, "userid: ", 8) == 0 &&
				    strcmp(buf + 8, userid) == 0) {
					fclose(fp);
					closedir(dirp);
					return 1;
				}
			}
			fclose(fp);
		}
	}
	closedir(dirp);
	return 0;
}

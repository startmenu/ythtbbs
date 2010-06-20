#include <math.h>
#include "mc.h"

MC_Env *mcEnv;
mcUserInfo *myInfo;
McMap *maps;

struct command2func {
        char name;
        int (*fptr) ();
};


const struct command2func mc_c2f[] = {
	{'s', (void*)money_hall}, 
	{'1', (void*)money_bank},
	{'2', (void*)money_gamble},
	{'3', (void*)money_shop},
	{'4', (void*)money_stock},
	{'5', (void*)money_lottery},
	{'d', (void*)money_exp},
	{'6', (void*)money_robber},
	{'7', (void*)money_beggar},
	{'8', (void*)money_cop},
	{'9', (void*)money_task},
	{'\0', (void*)NULL}};


static void* exec_func(char func_name) {
	int i = 0;
	for(i=0; mc_c2f[i].name != '\0'; i++) {
		if (mc_c2f[i].name == _tolower(func_name))
			return (void*)(mc_c2f[i].fptr)();
	}
	return NULL;
}


int
moneycenter()
{
	int ch, retv;
	char filepath[256];
	char buf[256];
	modify_user_mode(MONEY);
	strcpy(currboard, "millionaire");	// for deliverreport() 
	moneycenter_welcome();
// ����ȫ�ֲ������ҵ����� 
	sprintf(filepath, "%s", DIR_MC "mc.env");
	if (!file_exist(filepath))
		initData(0, filepath);
	if((mcEnv = loadData(filepath, sizeof (MC_Env))) == (void*)-1)
		return -1;
	sethomefile(filepath, currentuser->userid, "mc.save");
	if (!file_exist(filepath))
		initData(1, filepath);
	if ((myInfo = loadData(filepath, sizeof (mcUserInfo))) == (void*)-1)
		goto MCENV;
// ����Ƿ���Խ��� 
	retv = check_allow_in();
	if (retv == 0)
		goto UNMAP;
	if (retv == -1)
		goto MUTEX;
//myInfo->aliveTime = time(NULL); //�Զ���������ת�Ƶ��ӽ�վ��ʼ main.c 
	if (newSalary()) {
                      sprintf(buf, "����%d���ڵ�%s������ȡ��������Ϊ������",
                                        PAYDAY, CENTER_NAME);
                      deliverreport("�����С���վ����Ա��ȡ����",
                                              buf);
		}
	if(load_maps() == -1)
		goto UNMAP;

	while (1) {
		nomoney_show_stat("ʮ��·��");
		move(6, 0);
		prints("\033[33;41m " MY_BBS_NAME " \033[m\n\n");
		prints
		    ("\033[1;33m        �x�x�x ������         �x�x�u          �x�x�u\n"
		     "\033[1;33m          ��  ��������  �x�x  �� | ��   �x�x  �� | ��    \033[31m  �x�x�x�x�x\033[m\n"
		     "\033[1;33m          ��  �� �� ��  �� �� �� | ��   �� �� �� | ��    \033[31m���������稇\033[m\n"
		     "\033[1;33m        ���� �x�u ��    �����x�� �񨐨� �����x�� �񨐨�  \033[31m  ����������\033[m");
		move(t_lines - 2, 0);
		prints
		    ("\033[1;44m ��Ҫȥ \033[1;46m [S]����㳡 [1]���� [2]�ĳ� [3]�̵� [4]���� [5]��Ʊ                 \033[m");
		move(t_lines - 1, 0);
		prints
		    ("\033[1;44m        \033[1;46m [D]ˮ��֮�� [6]�ڰ� [7]ؤ�� [8]���� [9]���� [Q]�뿪                 \033[m");
		ch = igetkey();
		if(ch == 'q' || ch == 'Q')
			break;
		exec_func(ch);
	} //end of menu loop
	if(random() % 10 == 0)
		check_top();
	//forcetax();
      UNMAP:
	myInfo->mutex = 0;
      MUTEX:
	unloadData(myInfo, sizeof (mcUserInfo));
      MCENV:
	unloadData(mcEnv, sizeof (MC_Env));
	moneycenter_byebye();
	limit_cpu();
	return 0;
}

/*   �������� add by koyu */
void
update_health()
{
	time_t curtime;
	int add_health, add_point;

	curtime = time(NULL);
	if (myInfo->con > 25)
		myInfo->con = 25;
	add_health = (curtime - myInfo->aliveTime) / (30 - myInfo->con);	//ÿ(30-����)��������1 
	add_point = curtime - myInfo->aliveTime;
	myInfo->WorkPoint += add_point;
	if (myInfo->WorkPoint > 30000)
		myInfo->WorkPoint = 30000;
	if (myInfo->health < 0)
		myInfo->health = 0;
	myInfo->health =
	    MIN((myInfo->health + add_health), 100 + myInfo->con * 2);
	myInfo->aliveTime += add_point;

//��Ʒ[-100, 100]
	if (myInfo->luck > 100)
		myInfo->luck = 100;
	if (myInfo->luck < -100)
		myInfo->luck = -100;
//��ʶ������30000. add by yxk
	if (myInfo->robExp > 30000 || myInfo->robExp < -10000)
		myInfo->robExp = 30000;
	if (myInfo->begExp > 30000 || myInfo->begExp < -10000)
		myInfo->begExp = 30000;
//��Ǯ����20��. add by yxk
	if (myInfo->cash > 2000000000) {
		mcEnv->Treasury += myInfo->cash - 2000000000;
		myInfo->cash = 2000000000;
	}
	if (myInfo->cash < -1000000000) {
		mcEnv->Treasury += 294967296 + myInfo->cash;
		mcEnv->Treasury += 2000000000;
		myInfo->cash = 2000000000;
	}
	if (myInfo->credit > 2000000000) {
		mcEnv->Treasury += myInfo->credit - 2000000000;
		myInfo->credit = 2000000000;
	}
	if (myInfo->credit < -1000000000) {
		mcEnv->Treasury += 294967296 + myInfo->credit;
		mcEnv->Treasury += 2000000000;
		myInfo->credit = 2000000000;
	}
	if (!(random() % (MAX_ONLINE * 25))
	    && (5000000 < myInfo->cash + myInfo->credit))
		forcetax();

	return;
}

/*  ����ж���Ҫ������������������0��������ʾ��Ϣ����1  add by koyu */
int
check_health(int need_health, int x, int y, char *str, int wait)
{
	if (myInfo->health >= need_health)
		return 0;
	else {
		showAt(x, y, str, wait);
		return 1;
	}
}

/* ���ɻ�ɹ������������  add by darkeagle */
int
check_chance(int attack, int defence, int weapon, int armor, int effect,
	     int bonus)
{
	int num;
	double attacktemp;
	attacktemp = attack;
	num =
	    500 +
	    log((attacktemp + 30) / (defence + 30) * (weapon + 5) / (armor +
								     5)) *
	    effect + bonus;
	//���������Գ��������߹��౶ʱ����Ȼ�ɹ�
	if (random() % 1000 < num)
		return 1;
	else
		return 0;
}

void
showAt(int line, int col, char *str, int wait)
{
	move(line, col);
	prints("%s", str);
	if (wait)
		pressanykey();
}

void
moneycenter_welcome()
{
	char buf[256];

	clear();
	move(5, 0);
	prints("                      \033[1;32m����һ��ש���ɵ����磡\n\n");

	prints
	    ("                      \033[1;32m����һ������ð�յ����磡\033[m\n\n");

	prints
	    ("                         \033[1;31m������������棡\033[m\n\n");

	prints
	    ("                         \033[1;31m�����ǿ��Ϊ����\033[m\n\n");

	prints
	    ("              \033[1;35m���������ѡ�Ǯ��Ϊ����֮�� ʤ������ҳ���\033[m\n\n\n");

	prints
	    ("                    \033[0;1;33m��%s 2005�������绶ӭ����  \033[0m\n\n",
	     CENTER_NAME);

	prints
	    ("                    \033[0;1;33m������Ϸϵͳ�ż��Ѿ�����  \033[0m");
	pressanykey();

	clear();
	move(2, 0);
	sprintf(buf, "vote/millionaire/secnotes");
	if (dashf(buf))
		ansimore2stuff(buf, NA, 2, 24);
	pressanykey();
}

void
check_top()
{
	int n, n2, topMoney[LISTNUM], tmpM, ns, i, worth;
	short topRob[LISTNUM], topBeg[LISTNUM], tmpR, tmpB;
	char topIDM[LISTNUM][20], topIDR[LISTNUM][20], topIDB[LISTNUM][20],
	    tmpID[20];
	FILE *fp, *fpnew;
	struct BoardStock *bs;
	size_t filesize;
	void *stockMem;

	//��ʼ��
	for (n = 0; n < LISTNUM; n++) {
		strcpy(topIDM[n], "null.");
		strcpy(topIDR[n], "null.");
		strcpy(topIDB[n], "null.");
		topMoney[n] = 0;
		topRob[n] = 0;
		topBeg[n] = 0;
	}

	//�����Ʊ��ֵ
	worth = 0;
	if (!file_exist(DIR_STOCK "stock"))
		initData(2, DIR_STOCK "stock");
	ns = get_num_records(DIR_STOCK "stock", sizeof (struct BoardStock));
	if (ns <= 0)
		goto SORT;
	filesize = sizeof (struct BoardStock) * ns;
//���ع�����Ϣ 
	if ((stockMem = loadData(DIR_STOCK "stock", filesize)) == (void*)-1)
		goto SORT;

	for (i = 0; i < STOCK_NUM; i++) {
		if (myInfo->stock[i].num <= 0)
			continue;
		bs = stockMem +
		    myInfo->stock[i].stockid * sizeof (struct BoardStock);
		worth += bs->todayPrice[3] * myInfo->stock[i].num;
	}
	unloadData(stockMem, filesize);

      SORT:
	tmpM = myInfo->cash + myInfo->credit + worth - myInfo->loan;
	tmpR = myInfo->robExp;
	tmpB = myInfo->begExp;
	strcpy(tmpID, currentuser->userid);

	//��¼�ļ�������
	if ((fp = fopen(DIR_MC "top", "r")) == NULL) {
		fpnew = fopen(DIR_MC "top.new", "w");
		flock(fileno(fpnew), LOCK_EX);

		strcpy(topIDM[0], tmpID);
		topMoney[0] = tmpM;
		for (n = 0; n < LISTNUM; n++)
			fprintf(fpnew, "%s %d\n", topIDM[n], topMoney[n]);

		strcpy(topIDR[0], tmpID);
		topRob[0] = tmpR;
		for (n = 0; n < LISTNUM; n++)
			fprintf(fpnew, "%s %d\n", topIDR[n], topRob[n]);

		strcpy(topIDB[0], tmpID);
		topBeg[0] = tmpB;
		for (n = 0; n < LISTNUM; n++)
			fprintf(fpnew, "%s %d\n", topIDB[n], topBeg[n]);

		flock(fileno(fpnew), LOCK_UN);
		fclose(fpnew);

		if ((fp = fopen(DIR_MC "top", "r")) == NULL)
			rename(DIR_MC "top.new", DIR_MC "top");
		else {
			fclose(fp);
			deliverreport("ϵͳ����",
				      "\033[1;31m������¼�ļ��Ѿ����ڣ�\033[m");
		}
		return;
	}
	//��¼�ļ�����
	flock(fileno(fp), LOCK_EX);
	//����¼
	for (n = 0; n < LISTNUM; n++)
		fscanf(fp, "%s %d\n", topIDM[n], &topMoney[n]);
	for (n = 0; n < LISTNUM; n++)
		fscanf(fp, "%s %hd\n", topIDR[n], &topRob[n]);
	for (n = 0; n < LISTNUM; n++)
		fscanf(fp, "%s %hd\n", topIDB[n], &topBeg[n]);
	//ID���ϰ�
	for (n = 0; n < LISTNUM; n++) {
		if (!strcmp(topIDM[n], tmpID)) {
			topMoney[n] = tmpM;
			tmpM = -1;
		}
		if (!strcmp(topIDR[n], tmpID)) {
			topRob[n] = tmpR;
			tmpR = -1;
		}
		if (!strcmp(topIDB[n], tmpID)) {
			topBeg[n] = tmpB;
			tmpB = -1;
		}
	}
	//IDδ�ϰ�
	if (tmpM > topMoney[LISTNUM - 1]) {
		strcpy(topIDM[LISTNUM - 1], tmpID);
		topMoney[LISTNUM - 1] = tmpM;
	}
	if (tmpR > topRob[LISTNUM - 1]) {
		strcpy(topIDR[LISTNUM - 1], tmpID);
		topRob[LISTNUM - 1] = tmpR;
	}
	if (tmpB > topBeg[LISTNUM - 1]) {
		strcpy(topIDB[LISTNUM - 1], tmpID);
		topBeg[LISTNUM - 1] = tmpB;
	}
	//����
	tmpM = myInfo->cash + myInfo->credit + worth - myInfo->loan;
	tmpR = myInfo->robExp;
	tmpB = myInfo->begExp;
	for (n = 0; n < LISTNUM - 1; n++)
		for (n2 = n + 1; n2 < LISTNUM; n2++) {
			if (topMoney[n] < topMoney[n2]) {
				strcpy(tmpID, topIDM[n]);
				strcpy(topIDM[n], topIDM[n2]);
				strcpy(topIDM[n2], tmpID);
				tmpM = topMoney[n];
				topMoney[n] = topMoney[n2];
				topMoney[n2] = tmpM;
			}
			if (topRob[n] < topRob[n2]) {
				strcpy(tmpID, topIDR[n]);
				strcpy(topIDR[n], topIDR[n2]);
				strcpy(topIDR[n2], tmpID);
				tmpR = topRob[n];
				topRob[n] = topRob[n2];
				topRob[n2] = tmpR;
			}
			if (topBeg[n] < topBeg[n2]) {
				strcpy(tmpID, topIDB[n]);
				strcpy(topIDB[n], topIDB[n2]);
				strcpy(topIDB[n2], tmpID);
				tmpB = topBeg[n];
				topBeg[n] = topBeg[n2];
				topBeg[n2] = tmpB;
			}
		}
	//д�����ļ�
	fpnew = fopen(DIR_MC "top.new", "w");
	flock(fileno(fpnew), LOCK_EX);

	for (n = 0; n < LISTNUM; n++)
		fprintf(fpnew, "%s %d\n", topIDM[n], topMoney[n]);
	for (n = 0; n < LISTNUM; n++)
		fprintf(fpnew, "%s %d\n", topIDR[n], topRob[n]);
	for (n = 0; n < LISTNUM; n++)
		fprintf(fpnew, "%s %d\n", topIDB[n], topBeg[n]);

	flock(fileno(fpnew), LOCK_UN);
	fclose(fpnew);

	flock(fileno(fp), LOCK_UN);
	fclose(fp);

	rename(DIR_MC "top.new", DIR_MC "top");
	return;
}

void
moneycenter_byebye()
{
	clear();
	showAt(4, 14, "\033[1;32m�ڴ����ٴι��ٴ������磬"
	       "����ʵ���������롣\033[m", YEA);
}

void
show_top()
{
	int m, n, topMoney[LISTNUM], screen_num;
	short topRob[LISTNUM], topBeg[LISTNUM];
	char topIDM[LISTNUM][20], topIDR[LISTNUM][20], topIDB[LISTNUM][20];
	FILE *fp;

	fp = fopen(DIR_MC "top", "r");
	flock(fileno(fp), LOCK_EX);

	for (n = 0; n < LISTNUM; n++)
		fscanf(fp, "%s %d\n", topIDM[n], &topMoney[n]);
	for (n = 0; n < LISTNUM; n++)
		fscanf(fp, "%s %hd\n", topIDR[n], &topRob[n]);
	for (n = 0; n < LISTNUM; n++)
		fscanf(fp, "%s %hd\n", topIDB[n], &topBeg[n]);

	flock(fileno(fp), LOCK_UN);
	fclose(fp);

	screen_num = (LISTNUM + 1) / 20;
	for (m = 0; m <= screen_num - 1; m++) {
		clear();
		prints("\033[1;44;37m  " MY_BBS_NAME
		       " BBS    \033[32m--== ��  ��  �� ==-- "
		       "\033[33m--== ��  ʶ  �� ==-- \033[36m--== ��  ��  �� ==--\033[m\r\n");
		prints
		    ("\033[1;41;37m   ����           ����       ���ʲ�         ����    "
		     "��ʶ         ����    �� \033[m\r\n");

		for (n = 0; n < 20; n++)
			prints
			    ("\033[1;37m%6d\033[32m%18s\033[33m%11d\033[32m%15s"
			     "\033[33m%6d\033[32m%15s\033[33m%6d\033[m\r\n",
			     m * 20 + n + 1, topIDM[m * 20 + n],
			     topMoney[m * 20 + n], topIDR[m * 20 + n],
			     topRob[m * 20 + n], topIDB[m * 20 + n],
			     topBeg[m * 20 + n]);
		prints
		    ("\033[1;41;33m                    ���ǵ�%2d������������鿴��һ��"
		     "                            \033[m\r\n", m + 1);
		pressanykey();
	}
}

int
getOkUser(char *msg, char *uident, int line, int col)
{				// ���������Чid�ŵ�uident��, �ɹ�����1, ���򷵻�0 

	move(line, col);
	usercomplete(msg, uident);
	if (uident[0] == '\0')
		return 0;
	if (!getuser(uident, NULL)) {
		showAt(line + 1, col, "�����ʹ���ߴ���...", YEA);
		return 0;
	}
	return 1;
}

void
whoTakeCharge(int pos, char *boss)
{
	const char feaStr[][20] =
	    { "admin", "bank", "lottery", "gambling", "gang",
		"beggar", "stock", "shop", "police"
	};
	if (readstrvalue(MC_BOSS_FILE, feaStr[pos], boss, IDLEN + 1) != 0)
		*boss = '\0';
}

int
ismaster(char *uid)		//admin return -1, master return 1, none 0 
{
	int i, ret = 0;
	char boss[IDLEN + 1];

	for (i = 0; i <= 8; i++) {
		whoTakeCharge(i, boss);
		if (!strcmp(currentuser->userid, boss)) {
			if (i == 0)
				ret = -1;
			else
				ret = 1;
		}
	}

	return ret;
}

int
userInputValue(int line, int col, char *act, char *name, int inf, int sup)
{
	char buf[STRLEN], content[STRLEN];
	int num;

	snprintf(content, STRLEN - 1, "%s����%s��[%d--%d]", act, name, inf,
		 sup);
	getdata(line, col, content, buf, 10, DOECHO, YEA);
	num = atoi(buf);
	num = MAX(num, inf);
	num = MIN(num, sup);
	move(line + 1, col);
	snprintf(content, STRLEN - 1, "ȷ��%s %d %s��", act, num, name);
	if (askyn(content, NA, NA) == NA)
		return -1;
	return num;
}

#define EVENT_NUM (sizeof(rd_event)/sizeof(rd_event[0]))

int
randomevent()
//  ���¼����Ᵽ���ʽ�500w,�Ա�֤��Ѻ͹��ʵķ���
{
	int num, totle, rat;
	char title[STRLEN], buf[256];
	struct st_Event {
		char desc1[STRLEN], desc2[STRLEN], desc3[STRLEN];
		int type, bonus;	//type:1 �ֽ� 2 ��ʶ  3 �� 4 ���� 5 ���� 6 ��� etc 
	} rd_event[] = {	//ע������˳����ƷԽ�ã���������Ŀ�����Խ�� 
		{
		"��������" MY_BBS_NAME, "���в�������", "", 7, 10}, {
		"��������" MY_BBS_NAME, "������", "�Ĵ��", 6, -5}, {
		"���" MY_BBS_NAME "��������", "�����", "�⳥��", 6, 5}, {
		"���������", "�õ���", "�Ĵ��", 6, 10}, {
		"����С����", "�õ���", "�Ľ���", 0, 5}, {
		"��¶�������������ҩ", "���", "����", 1, 100000}, {
		"�����г�ˤ��", "�������ٵ�", "��", 5, -10}, {
		"�ɹ�����δ����", "��ʶ������", "��", 2, 15}, {
		"����ְȨ������", "׬��", "����֮��", 1, 50000}, {
		"��ש����", "�������ٵ�", "", 5, -20}, {
		"������������", "׬����", "���", 1, 30000}, {
		"�����貨΢��", "�������", "��", 3, 15}, {
		"��ѡվ��ɹ�", "������", "", 1, 200000}, {
		"���˹�������", "�������ٵ�", "", 5, -30}, {
		"��Accusation�����׷�����", "��ʶ����", "��", 2, 10}, {
		"���������FR jj", "ѧϰ�赸���������", "��", 3, 10}, {
		"����������", "��Ʒ����", "��", 4, -5}, {
		"���������¸ұ��", "��ʶ����", "��", 2, 10}, {
		"�ڹ�·��쭳�", "�������", "��", 3, 10}, {
		"���йش���۵�����Ϸ����", "�Ƿ�����", "", 1, 100000}, {
		"��̫��ȭ���С��å", "��ʶ����", "��", 2, 5}, {
		"���Ĵ������࿼�Ŀ�������", "��ʶ����", "��", 2, 15}, {
		"������������,մȾ����", "��Ʒ����", "��", 4, -10}, {
		"���ú�ѧϰ", "�ӿδ�����������", "��", 3, 5}, {
		"��ðȥУҽԺ", "��ʶ����", "��", 2, 5}, {
		"ÿ����11·ֱ�ﳵ�ϰ�", "�������", "��", 3, 5}, {
		"һ�Ķ�������", "��Ʒ����", "��", 4, -15}, {
		"Т����ĸ", "��Ʒ����", "��", 4, 20}, {
		"��ѡվ��ɹ�", "��л֧���߻���", "", 1, -200000}, {
		"����ײ�ϵ��߸�", "��������", "��", 3, -5}, {
		"Υ�±�������ס", "��ʶ������", "��", 2, -5}, {
		"�����蹷", "��ҧ�˺���������", "��", 3, -5}, {
		"��֯���б���ʧ�ܵ���ȫ������Ż��", "�⳥��",
			    "������ʧ��", 1, -200000}, {
		"����ʳ����ɳ��������׷�", "��Ʒ����", "��", 4, 10}, {
		"���ø����ܱ�ץ����", "��ʶ������", "��", 2, -5}, {
		"�ɹ�֤���˽������ı�д�ߵ����ս�թ", "��Ʒ����", "��", 4, 10},
		{
		"�Է���Ϸ��Ҷ���˺Ʊ", "��Ʒ����", "��", 4, -20}, {
		"������ˮ", "������", "�ķ���", 1, -30000}, {
		"�߻��׶ر�ը", "��Ʒ����", "��", 4, -20}, {
		"�ڹ������ϱ���ɧ��", "��ʶ������", "��", 2, -10}, {
		"��Ǯ��", "������", "͵͵�Ž����Լ��ڴ���", 1, 100000}, {
		"��С��å����", "������", "��", 3, -10}, {
		"���������¸ұ��", "Ϊ�˲ɹ����߻���", "���ֽ�", 1, -100000},
		{
		"ʰ����", "��Ʒ����", "��", 4, 5}, {
		"���ϻؼ�·�ϱ�����", "������", "��", 3, -10}, {
		"�չ���ͭ����", "׬��", "", 1, 50000}, {
		"��֯���Ұ���ĸ��ļ��", "��þ��", "", 1, 200000}, {
		"�������г�", "����", "���ֽ�", 1, -50000}, {
		"���ڰ���", "��ʶ������", "��", 2, -10}, {
		"̰С�������˼ٻ�", "��ʧ", "���ֽ�", 1, -100000}, {
		"����ǧ���˲�", "�������ӵ�", "", 5, 100}, {
		"������Ů�������", "��ʶ������", "��", 2, -15}, {
		"���´�����", "�������ӵ�", "", 5, 40}, {
		"����С˥��", "��ʧ��", "���ֽ�", 0, -5}, {
		"�����ڶĲ�", "�����", "�ֽ�", 1, -200000}, {
		"�����³�Passat", "����", "", 1, -100000}, {
		"������˥��", "����һ��", "�Ĵ���", 6, -10}, {
		"�����ֳ�����ˤ��", "������", "��", 3, -15}, {
		"��ֶ�������", "�������ӵ�", "��", 5, 20}, {
		"��ͼ������" MY_BBS_NAME, "ȫ��ָ���½�", "", 7, -10}
	};

	if (!(myInfo->GetLetter == 1)) {
		clear();
		showAt(5, 4, "���Ѿ��ر��˽���������Ϸ���ܣ��뿪����������",
		       YEA);
		return 0;
	}

	myInfo->Actived = 0;
	money_show_stat("��������");

	move(4, 4);
	prints("��֪�����˶�ã����������˹����������Լ�����һ�����صĵط���\n"
	       "    ������֣�ͻȻ����һ����������˵��������ӭ�����������磡��\n"
	       "    ��ش���ȷһ������󣬾ͻ�����һ������¼���ף����ˣ�");
	pressreturn();
	money_show_stat("��������");
	if (check_health(10, 12, 4, "���, û�����ˣ��������˰ɡ�����", YEA))
		return 0;

	if (show_cake()) {
		prints("����Ǹ������˰�...\n");
		pressreturn();
		return 0;
	}

	clear();
	update_health();
	money_show_stat("��������");

	rat = EVENT_NUM * (myInfo->luck + 100) / 800;	//��������Ʒ���¼��û���Ӱ��,���������������ѭ��
	num = random() % (EVENT_NUM - rat);
	if (myInfo->luck == -100)	//-100��Ʒ�ĳͷ�
		num = EVENT_NUM - 1;
	if (!num && (myInfo->luck < 100))	//��Ʒ100��������
		num++;
/*	if (rat < 0)
		num -= rat;

	num = abs(num) % EVENT_NUM;	//���漸������ʲô��������������ڰ�
*/
	switch (rd_event[num].type) {
	case 0:		//��Ǯ�ٷֱ� 
		totle =
		    MIN(myInfo->cash * rd_event[num].bonus / 100,
			MAX_MONEY_NUM / 2);
		move(6, 4);
		if (totle < 0)
			totle = MAX(totle, -myInfo->cash);
		else {
			totle = MIN(totle, (mcEnv->Treasury - 5000000) / 2);
			totle = MAX(totle, 0);
		}
		mcEnv->Treasury -= totle;
		myInfo->cash = MAX(0, (myInfo->cash + totle));
		prints("��%s��%s%d%s%s��", rd_event[num].desc1,
		       rd_event[num].desc2, abs(totle), MONEY_NAME,
		       rd_event[num].desc3);
		sprintf(title, "���¼���%s%s", currentuser->userid,
			rd_event[num].desc1);
		sprintf(buf, "%s%s��%s%d%s%s��", currentuser->userid,
			rd_event[num].desc1, rd_event[num].desc2, abs(totle),
			MONEY_NAME, rd_event[num].desc3);
		break;
	case 1:		//��Ǯ���� 
		if ((myInfo->cash + myInfo->credit <=
		     abs(rd_event[num].bonus) * 10)
		    || (myInfo->cash <= rd_event[num].bonus * 2))
			//���ͼ����ֽ��¼������ֵ�Ӱ��
			totle = rd_event[num].bonus / 5;
		else
			totle = rd_event[num].bonus;
		if (totle < 0)
			totle = MAX(totle, -myInfo->cash);
		else {
			totle = MIN(totle, (mcEnv->Treasury - 5000000) / 2);
			totle = MAX(totle, 0);
		}
		mcEnv->Treasury -= totle;
		move(6, 4);
		myInfo->cash = MAX(0, (myInfo->cash + totle));
		prints("��%s��%s%d%s%s��", rd_event[num].desc1,
		       rd_event[num].desc2, abs(totle), MONEY_NAME,
		       rd_event[num].desc3);
		sprintf(title, "���¼���%s%s", currentuser->userid,
			rd_event[num].desc1);
		sprintf(buf, "%s%s��%s%d%s%s��", currentuser->userid,
			rd_event[num].desc1, rd_event[num].desc2, abs(totle),
			MONEY_NAME, rd_event[num].desc3);
		break;
	case 2:		//��ʶ 
		totle = rd_event[num].bonus;
		move(6, 4);
		myInfo->robExp = MAX(0, (myInfo->robExp + totle));
		prints("��%s��%s%d%s��", rd_event[num].desc1,
		       rd_event[num].desc2, abs(totle), rd_event[num].desc3);
		sprintf(title, "���¼���%s%s", currentuser->userid,
			rd_event[num].desc1);
		sprintf(buf, "%s%s��%s%d%s��", currentuser->userid,
			rd_event[num].desc1, rd_event[num].desc2, abs(totle),
			rd_event[num].desc3);
		break;
	case 3:		//�� 
		totle = rd_event[num].bonus;
		move(6, 4);
		myInfo->begExp = MAX(0, (myInfo->begExp + totle));
		prints("��%s��%s%d%s��", rd_event[num].desc1,
		       rd_event[num].desc2, abs(totle), rd_event[num].desc3);
		sprintf(title, "���¼���%s%s", currentuser->userid,
			rd_event[num].desc1);
		sprintf(buf, "%s%s��%s%d%s��", currentuser->userid,
			rd_event[num].desc1, rd_event[num].desc2, abs(totle),
			rd_event[num].desc3);
		break;
	case 4:		//��Ʒ 
		totle = rd_event[num].bonus;
		if ((myInfo->cash + myInfo->credit < abs(totle) * 100000) && (myInfo->robExp + myInfo->begExp < 1000))	//������Ʒ�¼������ֵ�Ӱ��
			totle /= 5;
		move(6, 4);
		myInfo->luck = MAX(-100, (myInfo->luck + totle));
		prints("��%s��%s%d%s��", rd_event[num].desc1,
		       rd_event[num].desc2, abs(totle), rd_event[num].desc3);
		sprintf(title, "���¼���%s%s", currentuser->userid,
			rd_event[num].desc1);
		sprintf(buf, "%s%s��%s%d%s��", currentuser->userid,
			rd_event[num].desc1, rd_event[num].desc2, abs(totle),
			rd_event[num].desc3);
		break;
	case 5:		//���� 
		update_health();
		totle =
		    MIN(100 + 2 * myInfo->con,
			rd_event[num].bonus + myInfo->health);
		move(6, 4);
		myInfo->health = MAX(totle, 0);
		prints("��%s��%s%d%s��", rd_event[num].desc1,
		       rd_event[num].desc2, myInfo->health,
		       rd_event[num].desc3);
		sprintf(title, "���¼���%s%s", currentuser->userid,
			rd_event[num].desc1);
		sprintf(buf, "%s%s��%s%d%s��", currentuser->userid,
			rd_event[num].desc1, rd_event[num].desc2,
			myInfo->health, rd_event[num].desc3);
		break;
	case 6:		//���ٷֱ� 
		if (rd_event[num].bonus < 0)
			//totle = MAX(-MAX_MONEY_NUM, myInfo->credit * rd_event[num].bonus/100);
			totle =
			    MAX(-2000000,
				myInfo->credit * rd_event[num].bonus / 100);
		else
			//totle = MIN(MAX_MONEY_NUM, myInfo->credit * rd_event[num].bonus/100);
			totle =
			    MIN(2000000,
				myInfo->credit * rd_event[num].bonus / 100);
		if (totle < 0)
			totle = MAX(totle, -myInfo->credit);
		else {
			totle = MIN(totle, (mcEnv->Treasury - 5000000) / 2);
			totle = MAX(totle, 0);
		}
		mcEnv->Treasury -= totle;
		move(6, 4);
		myInfo->credit = MAX(0, (myInfo->credit + totle));
		prints("��%s��%s%d%s%s��", rd_event[num].desc1,
		       rd_event[num].desc2, abs(totle), MONEY_NAME,
		       rd_event[num].desc3);
		if (rd_event[num].bonus <= 3) {
			myInfo->health = 0;
			move(7, 4);
			prints("��������ô����£���æ�������ˡ�");
		}
		sprintf(title, "���¼���%s%s", currentuser->userid,
			rd_event[num].desc1);
		sprintf(buf, "%s%s��%s%d%s%s��", currentuser->userid,
			rd_event[num].desc1, rd_event[num].desc2, abs(totle),
			MONEY_NAME, rd_event[num].desc3);
		break;
	case 7:		//ȫ��
		totle = MIN(myInfo->cash * rd_event[num].bonus / 100, 2000000);
		totle = MIN(totle, (mcEnv->Treasury - 5000000) / 2);
		totle = MAX(totle, 0);
		myInfo->cash += totle;
		mcEnv->Treasury -= totle;
		totle =
		    MIN(myInfo->credit * rd_event[num].bonus / 100, 2000000);
		totle = MIN(totle, (mcEnv->Treasury - 5000000) / 2);
		totle = MAX(totle, 0);
		myInfo->credit += totle;
		mcEnv->Treasury -= totle;
		myInfo->robExp +=
		    MIN(myInfo->robExp * rd_event[num].bonus / 100, 50);
		myInfo->begExp +=
		    MIN(myInfo->begExp * rd_event[num].bonus / 100, 50);
		myInfo->luck += abs(myInfo->luck) * rd_event[num].bonus / 100;
		myInfo->health = 0;
		move(6, 4);
		prints("��%s��%s��\n    ������ô����£���æ�������ˡ�",
		       rd_event[num].desc1, rd_event[num].desc2);
		sprintf(title, "���¼���%s%s", currentuser->userid,
			rd_event[num].desc1);
		sprintf(buf, "%s%s��%s��", currentuser->userid,
			rd_event[num].desc1, rd_event[num].desc2);
		break;
	}
	if (random() % (MAX_ONLINE / 10) == 0 || rd_event[num].type == 7
	    || rd_event[num].type == 6)
		deliverreport(title, buf);
	pressanykey();
	sleep(1);
	return 1;

}

void
money_show_stat(char *position)
{
	clear();
	if (chkmail()) {
		move(0, 30);
		prints("\033[1;5;36m[�����ż�]\033[m");
	}
	if (mcEnv->closed && !USERPERM(currentuser, PERM_SYSOP) && (strcmp("admin", currentuser->userid))) {
		move(12, 4);
                prints("    ���̹ر��ˡ������������´μ��:D");
		unloadData(myInfo, sizeof (mcUserInfo));
		unloadData(mcEnv, sizeof (MC_Env));
		pressreturn();
                Q_Goodbye();
	  }

	if (time(NULL) < myInfo->freeTime) {
		move(12, 4);
		prints("    �������ϴ������·��ˣ��㱻����ץ���ˣ�");
		myInfo->mutex = 0;
		unloadData(myInfo, sizeof (mcUserInfo));
		unloadData(mcEnv, sizeof (MC_Env));
		pressreturn();
		Q_Goodbye();
	}

	move(1, 0);
	update_health();
	prints
	    ("���Ĵ��ţ�\033[1;33m%s\033[m      ��ʶ \033[1;33m%d\033[m  �� \033[1;33m%d\033[m  ��Ʒ \033[1;33m%d\033[m  ���� \033[1;33m%d\033[m  ���� \033[1;33m%d\033[m \n",
	     currentuser->userid, myInfo->robExp, myInfo->begExp, myInfo->luck,
	     myInfo->con, myInfo->health);
	prints("�����ϴ��� \033[1;31m%d\033[m %s��", myInfo->cash, MONEY_NAME);
	prints("��� \033[1;31m%d\033[m %s����ǰλ�� \033[1;33m%s\033[m",
	       myInfo->credit, MONEY_NAME, position);
	move(3, 0);
	prints
	    ("\033[1m--------------------------------------------------------------------------------\033[m");
}

void
nomoney_show_stat(char *position)
{
	clear();
	if (chkmail()) {
		move(0, 30);
		prints("\033[1;5;36m[�����ż�]\033[m");
	}
	if (mcEnv->closed && !USERPERM(currentuser, PERM_SYSOP) && (strcmp("admin", currentuser->userid))) {
	        move(12, 4);
                prints("    ���̹ر��ˡ������������´μ��:D");
                unloadData(myInfo, sizeof (mcUserInfo));
                unloadData(mcEnv, sizeof (MC_Env));
                pressreturn();
                Q_Goodbye();
          }

	if (time(NULL) < myInfo->freeTime) {
		move(12, 4);
		prints("    �������ϴ������·��ˣ��㱻����ץ���ˣ�");
		myInfo->mutex = 0;
		unloadData(myInfo, sizeof (mcUserInfo));
		unloadData(mcEnv, sizeof (MC_Env));
		pressreturn();
		Q_Goodbye();
	}
	update_health();
	if (myInfo->luck > 100)
		myInfo->luck = 100;
	if (myInfo->luck < -100)
		myInfo->luck = -100;
	move(1, 0);
	prints
	    ("���Ĵ��ţ�\033[1;33m%s\033[m      ��ʶ \033[1;33m%d\033[m  �� \033[1;33m%d\033[m  ��Ʒ \033[1;33m%d\033[m  ���� \033[1;33m%d\033[m  ���� \033[1;33m%d\033[m \n",
	     currentuser->userid, myInfo->robExp, myInfo->begExp, myInfo->luck,
	     myInfo->con, myInfo->health);
	prints
	    ("\033[1;32m��ӭ����%s�������磬��ǰλ����\033[0m \033[1;33m%s\033[0m",
	     CENTER_NAME, position);
	move(3, 0);
	prints
	    ("\033[1m--------------------------------------------------------------------------------\033[m");
}

int
check_allow_in()
{
	time_t freeTime, backTime, currTime = time(NULL);
	int day, hour, minute, num;
	char buf[256], admin[IDLEN + 1];

	clear();
	move(9, 8);
	whoTakeCharge(0, admin);
	if (mcEnv->closed && !USERPERM(currentuser, PERM_SYSOP) && (strcmp(admin, currentuser->userid))) {	/* ��������ر� */
		showAt(10, 10, "��������ر���...���Ժ�����", YEA);
		return 0;
	}
	if (mcEnv->openTime > currentuser->lastlogin) {
		showAt(10, 4,
		       "�����޸��˴��룬����Ҫ�˳����д��������µ�¼����������������",
		       YEA);
		return 0;
	}

	if (myInfo->mutex++ && count_uindex_telnet(usernum) > 1) {	// ����ര��, ͬʱ������� 
		showAt(10, 10, "���Ѿ��ڴ�����������!", YEA);
		return -1;
	}
/* ���ﱻ��� */
	clrtoeol();
	freeTime = myInfo->freeTime;
	if (currTime < freeTime) {
		day = (freeTime - currTime) / 86400;
		hour = (freeTime - currTime) % 86400 / 3600;
		minute = (freeTime - currTime) % 3600 / 60 + 1;
		if (seek_in_file(DIR_MC "policemen", currentuser->userid)) {
			prints("��ִ���������˻���Ҫ����%d��%dСʱ%d���ӡ�",
			       day, hour, minute);
			num =
			    (sqrt(myInfo->robExp + myInfo->begExp) / 2 +
			     40) * (freeTime - currTime);
			if (num>2000000000)
				num=2000000000;
			if (num < 0)
				num=MAX_MONEY_NUM; // bug fix, temp
			sprintf(buf, "��˵����ֻҪ����%d%s�Ϳ����ػ�����!", num,
				MONEY_NAME);
			move(11, 8);
			if (askyn(buf, YEA, NA) == NA) {
				pressanykey();
				return 0;
			} else {
				if (myInfo->credit < num) {
					prints("\n���񲻾����������һ���!");
					myInfo->freeTime += 600;
					pressanykey();
					return 0;
				} else {
					myInfo->credit -= num;
					mcEnv->Treasury += num;
					prints
					    ("\n��ͻȻ������̥���ǵĸо�����Ȼ���񼣰�!");
					myInfo->freeTime = 0;
				}
			}
		} else {
			prints("�㱻%s�������ˡ�����%d��%dСʱ%d���ӵļ����",
			       CENTER_NAME, day, hour, minute);
			num =
			    (sqrt(myInfo->robExp + myInfo->begExp) / 2 +
			     40) * (freeTime - currTime);
			if (num>2000000000)
				num=2000000000;
			if(num < 0)
				num = MAX_MONEY_NUM;
			sprintf(buf, "��ֻ��Ҫ����%d%s�Ϳ������»������!",
				num,
				MONEY_NAME);
			move(11, 8);
			if (askyn(buf, YEA, NA) == NA) {
				pressanykey();
				return 0;
			} else {
				if (myInfo->credit < num) {
					prints
					    ("\nûǮ����ϷŪ�������ټ���10���ӣ�");
					myInfo->freeTime += 600;
					pressanykey();
					return 0;
				} else {
					myInfo->credit -= num;
					mcEnv->Treasury += num;
					prints
					    ("\n��ϲ�����»������,��ο�Ҫ�����ؼ�Ŷ!");
					myInfo->freeTime = 0;
				}
			}
		}
		pressanykey();
	} else if (currTime > freeTime && freeTime > 0) {
		myInfo->freeTime = 0;
		if (seek_in_file(DIR_MC "policemen", currentuser->userid))
			showAt(10, 10, "��ϲ��������Ժ��", YEA);
		else
			showAt(10, 10, "�����������ϲ�����»�����ɣ�", YEA);
	}
	clrtoeol();
/* Ƿ��� */
	backTime = myInfo->backTime;
	if (currTime > backTime && backTime > 0) {
		if (askyn("��Ƿ���еĴ�����ˣ��Ͻ����ɣ�", YEA, NA) == NA)
			return 0;
		money_bank();
		return 0;
	}
// ��������
	if (seek_in_file(DIR_MC "bannedID", currentuser->userid)) {
		showAt(10, 10,
		       "�����Ҵ�������������򣬱�ȡ���������������ʸ�\n"
		       "          ��������ܹ���ϵ��", YEA);
		pressanykey();
		return 0;
	}
	return 1;
}

int
positionChange(int pos, char *boss, char *posStr, int type)
{
	char head[16], in[16], end[16];
	char buf[STRLEN], title[STRLEN], letter[2 * STRLEN];
	char posDesc[][20] =
	    { "���������ܹ�", "�����г�", "���ʹ�˾����", "�ĳ�����",
		"�ڰ����", "ؤ�����", "֤�����ϯ",
		"�̳�����", "������"
	};
	char ps[][STRLEN] =
	    { "������������������Ȩı˽����Ϊ��վ������ҵ�ķ�չ�Ϲ����ᡣ",
		"�����������һֱ�����Ĺ�����ʾ��л��ף�Ժ�˳����"
	};
	if (type == 0) {
		strcpy(head, "����");
		strcpy(in, "Ϊ");
		strcpy(end, "");
	} else {
		strcpy(head, "��ȥ");
		strcpy(in, "��");
		strcpy(end, "ְ��");
	}
	move(20, 4);
	snprintf(title, STRLEN - 1, "���ܹܡ�%s%s%s%s%s", head, boss, in,
		 posDesc[pos], end);
	sprintf(genbuf, "ȷ��Ҫ %s ��", (title + 8));	//��ȥ [����] 
	if (askyn(genbuf, YEA, NA) == NA)
		return 0;
	sprintf(genbuf, "%s %s", posStr, boss);
	if (type == 0) {
		addtofile(MC_BOSS_FILE, genbuf);
		sprintf(letter, "%s", ps[0]);
	} else {
		getdata(21, 4, "ԭ��", buf, 40, DOECHO, YEA);
		sprintf(letter, "ԭ��%s\n\n%s", buf, ps[1]);
		del_from_file(MC_BOSS_FILE, posStr);
	}
	deliverreport(title, letter);
	system_mail_buf(letter, strlen(letter), boss, title,
			currentuser->userid);
	showAt(22, 4, "�������", YEA);
	return 1;
}

//  --------------------------    ����    ------------------------  // 

void
specil_MoneyGive()
{
	int money;
	char uident[IDLEN + 1], admin[IDLEN + 1], bank[IDLEN + 1], reason[256],
	    buf[256], title[STRLEN];
	mcUserInfo *mcuInfo;

	money_show_stat("�ر𲦿�С���");
	if (!getOkUser("�㲦���˭��", uident, 6, 4)) {
		move(7, 4);
		prints("���޴���");
		pressanykey();
		return;
	}

	sethomefile(buf, uident, "mc.save");
	if (!file_exist(buf))
		initData(1, buf);
	money = userInputValue(7, 4, "��", "��", 1, 10000);
	if (money < 0)
		return;

	getdata(10, 4, "���ɣ�", reason, 40, DOECHO, YEA);

	if (reason[0] == '\0' || reason[0] == ' ') {
		showAt(12, 4, "û�����ɲ���˽�Բ��", YEA);
		return;
	}

	if (mcEnv->Treasury < money * 10000) {
		showAt(14, 4, "������飬�޷����", YEA);
		return;
	}

	move(11, 4);
	if (askyn("��ȷ��Ҫ������", YEA, NA) == YEA) {
		move(12, 4);
		if (askyn
		    ("ʹ�����ܲ�����ʽ�����ѵ���ʲô�������˵ģ����ã���", NA,
		     NA) == YEA) {
			mcuInfo = loadData(buf, sizeof (mcUserInfo));
			if(mcuInfo == (void*)-1)
				return;
			mcuInfo->credit += 10000 * money;
			//mcEnv->Treasury -= 10000 * money;   //���ܲ��ͨ�����⣬���ڷ��л���
			myInfo->Actived++;
			sprintf(title, "�ܹ�%s���ܲ���%d���%s",
				currentuser->userid, money, uident);
			sprintf(buf,
				"�ܹ�%s���ܲ���%d���%s,����Ҳ����è�塣 \n���ɣ�%s\n",
				currentuser->userid, money, uident, reason);
			system_mail_buf(buf, strlen(buf), uident, title,
					currentuser->userid);
			if (readstrvalue
			    (MC_BOSS_FILE, "admin", admin, IDLEN + 1) != 0)
				admin[0] = '\0';
			system_mail_buf(buf, strlen(buf), admin, title,
					currentuser->userid);
			if (readstrvalue(MC_BOSS_FILE, "bank", bank, IDLEN + 1)
			    != 0)
				bank[0] = '\0';
			system_mail_buf(buf, strlen(buf), bank, title,
					currentuser->userid);

			unloadData(mcuInfo, sizeof (mcUserInfo));
			showAt(16, 4, "����ɹ�", YEA);
		} else {
			mcuInfo = loadData(buf, sizeof (mcUserInfo));
			if(mcuInfo == (void*)-1)
				return;
			mcuInfo->credit += 10000 * money;
			if (mcuInfo->GetLetter == 0)
				mcEnv->Treasury -= 100000 * money;
			else
				mcEnv->Treasury -= 10000 * money;
			myInfo->Actived++;
			sprintf(title, "�ܹ�%s�ر𲦿�%d���%s",
				currentuser->userid, money, uident);
			sprintf(buf, "�ܹ�%s�ر𲦿�%d���%s��\n���ɣ�%s\n",
				currentuser->userid, money, uident, reason);
			system_mail_buf(buf, strlen(buf), uident, title,
					currentuser->userid);
			deliverreport(title, buf);

			unloadData(mcuInfo, sizeof (mcUserInfo));
			showAt(16, 4, "����ɹ�", YEA);
		}
	}
	return;
}

void
promotion(int type, char *prompt)
{
	int pos;
	char boss[IDLEN], buf[STRLEN];
	char *feaStr[] = { "admin", "bank", "lottery", "gambling",
		"gang", "beggar", "stock", "shop", "police"
	};

	getdata(16, 4,
		"ְλ: [0.�ܹ� 1.���� 2.��Ʊ 3.�ĳ� 4.�ڰ� 5.ؤ�� 6.���� 7.�̳� 8.���� ]",
		buf, 2, DOECHO, YEA);
	pos = atoi(buf);
	if (pos > 8 || pos < 0)
		return;
	whoTakeCharge(pos, boss);
	move(17, 4);
	if (boss[0] != '\0' && type == 0) {
		prints("%s�Ѿ������ְλ��", boss);
		pressanykey();
		return;
	}
	if (boss[0] == '\0' && type == 1) {
		showAt(16, 4, "Ŀǰ�����˸����ְλ��", YEA);
		return;
	}
	if (type == 0 && !getOkUser(prompt, boss, 18, 4))
		return;
	positionChange(pos, boss, feaStr[pos], type);
}

int
money_admin()
{
	int ch, quit = 0, money = 0;
	char admin[IDLEN + 1], title[STRLEN], buf[256];

	if (readstrvalue(MC_BOSS_FILE, "admin", admin, IDLEN + 1) != 0)
		admin[0] = '\0';
	if (strcmp(admin, currentuser->userid)
	    && !(currentuser->userlevel & PERM_SYSOP))
		return 0;
	while (!quit) {
		nomoney_show_stat("�����������");
		sprintf(buf, "    ���︺��%s������������¹���\n"
			"    �����ܹ���: \033[1;32m%s\033[m\n"
			"    ���ⴢ����: \033[1;33m%15Ld\033[m %s\n",
			//"    �û��ܲƲ�: \033[1;32m%15Ld\033[m %s",
			CENTER_NAME, admin, mcEnv->Treasury, MONEY_NAME);
		//,all_user_money(), MONEY_NAME);
		showAt(5, 0, buf, NA);
		showAt(10, 0,
		       "        1. ����ְλ                    2. ��ȥְλ\n"
		       "        3. �г�ְλ����                4. �ر�/������������\n"
		       "        5. �ر𲦿�                    6. ӡǮ (���ã�����)\n"
		       "        7. �趨��������                8. ͳ���û���Ϣ\n"
		       "        Q. �˳�", NA);
		ch = igetkey();
		switch (ch) {
		case '1':
			promotion(0, "����˭��");
			break;
		case '2':
			promotion(1, "��ȥ˭��");
			break;
		case '3':
			clear();
			move(1, 0);
			prints("Ŀǰ%s���������ְλ�����", CENTER_NAME);
			listfilecontent(MC_BOSS_FILE);
			FreeNameList();
			pressanykey();
			break;
		case '4':
			move(15, 4);
			if (mcEnv->closed)
				sprintf(genbuf, "%s", "ȷ��Ҫ����ô");
			else
				sprintf(genbuf, "%s", "ȷ��Ҫ�ر�ô");
			if (askyn(genbuf, NA, NA) == NA)
				break;
			mcEnv->closed = !mcEnv->closed;
			if (!mcEnv->closed)
				mcEnv->openTime = time(NULL);
			showAt(16, 4, "�����ɹ�", YEA);
			break;
		case '5':
			update_health();
			if (check_health
			    (1, 12, 4, "������̫�����ˣ���Ϣһ�°ɣ�", YEA))
				break;
			specil_MoneyGive();
			update_health();
			myInfo->health--;
			break;
		case '6':
			update_health();
			if (check_health
			    (1, 12, 4, "������̫�����ˣ���Ϣһ�°ɣ�", YEA))
				break;
			money = userInputValue(12, 4, "����", "��", 1, 10);
			if (money < 0)
				break;
			mcEnv->Treasury += money * 100000000;
			move(15, 4);
			prints("���Ѿ�ӡ��%d��%s��ע���˹��⡣", money,
			       MONEY_NAME);
			sprintf(title, "�����⡿%s�ܹ�%s�����ע����%s",
				CENTER_NAME, currentuser->userid, MONEY_NAME);
			sprintf(buf, "Ϊ���⾭��Σ�����ܹ�%s��׼ӡ��%d��%s",
				currentuser->userid, money, MONEY_NAME);
			deliverreport(title, buf);
			break;
		case '7':
			banID();
			break;
		case '8':
			all_user_money();
			break;
		case 'Q':
		case 'q':
			quit = 1;
			break;
		}
	}
	return 0;
}

void
banID(void)
{
	int found;
	char uident[IDLEN + 1], buf[256], title[STRLEN];
	mcUserInfo *mcuInfo;

	if (!getOkUser("������ID: ", uident, 12, 4))
		return;
	found = seek_in_file(DIR_MC "bannedID", uident);
	if (found) {
		sprintf(buf, "%s���ڻ���������Ҫ�����ͷ���", uident);
		move(13, 4);
		if (askyn(buf, NA, NA) == NA)
			return;
		del_from_file(DIR_MC "bannedID", uident);
		sprintf(title, "���ܹܡ�����%s�����������", uident);
		sprintf(buf,
			"    ����%s���иĻ�֮�ģ������ܹ�%s���������½���������硣",
			uident, currentuser->userid);
		deliverreport(title, buf);
		system_mail_buf(buf, strlen(buf), uident, title,
				currentuser->userid);
		showAt(14, 4, "�����ɹ���", YEA);
		return;
	} else {
		sprintf(buf, "��ȷ����%s���뻵����������������ָ���������㣡",
			uident);
		move(13, 4);
		if (askyn(buf, NA, NA) == NA)
			return;
		addtofile(DIR_MC "bannedID", uident);
		sethomefile(buf, uident, "mc.save");
		if (!file_exist(buf))
			initData(1, buf);
		if ((mcuInfo = loadData(buf, sizeof (mcUserInfo))) == (void*)-1)
			return;
		mcuInfo->robExp = 0;
		mcuInfo->begExp = 0;
		mcuInfo->luck = 0;
		mcEnv->Treasury += mcuInfo->cash + mcuInfo->credit;
		mcuInfo->cash = 0;
		mcuInfo->credit = 0;
		unloadData(mcuInfo, sizeof (mcUserInfo));
		sprintf(title, "���ܹܡ������ܹ�%sȡ��%s�������������ʸ�",
			currentuser->userid, uident);
		sprintf(buf,
			"    ����%s���Ҵ�������������򣬱�ȡ���������������ʸ�\n"
			"    ������ָ���ѱ����㡣��������½���������磬�����ܹ���ϵ��",
			uident);
		deliverreport(title, buf);
		system_mail_buf(buf, strlen(buf), uident, title,
				currentuser->userid);
		showAt(14, 4, "�����ɹ���", YEA);
		return;
	}
}

void
all_user_money()
{
	long long AllUserMoneyA = 0, AllUserMoneyB = 0;
	FILE *fp, *fw;
	char buf[256], fname[STRLEN], admin[IDLEN + 1];
	mcUserInfo *mcuInfo;
	int money;

	nomoney_show_stat("�����������");
	money = userInputValue(12, 4, "����", "�����ϵ��û�", 10, 1000);
	if (money < 0)
		return;
	sprintf(fname, DIR_MC "UserMoney");
	fp = fopen(DIR_MC "mc_user", "r");
	fw = fopen(DIR_MC "UserMoney", "w");
	flock(fileno(fp), LOCK_EX);
	while (fscanf(fp, "%s\n", buf) != EOF) {
		if (!file_exist(buf)) {
			del_from_file(DIR_MC "mc_user", buf);
			continue;
		}
		if ((mcuInfo = loadData(buf, sizeof (mcUserInfo))) == (void*)-1)
			continue;
		if (mcuInfo->GetLetter == 1)
			AllUserMoneyA +=
			    mcuInfo->cash + mcuInfo->credit + myInfo->interest -
			    mcuInfo->loan;
		else
			AllUserMoneyB +=
			    mcuInfo->cash + mcuInfo->credit + myInfo->interest -
			    mcuInfo->loan;
		if (mcuInfo->cash + mcuInfo->credit > 10000 * money) {
			fprintf(fw, "%s  %d\n", buf,
				mcuInfo->cash + mcuInfo->credit +
				myInfo->interest - mcuInfo->loan);
		}
		unloadData(mcuInfo, sizeof (mcUserInfo));
	}

	move(8, 5);
	sprintf(buf, "    �û���Ϸ����: \033[1;32m%15lld\033[m %s",
		AllUserMoneyA, MONEY_NAME);
	prints("%s", buf);
	move(9, 5);
	sprintf(buf, "    �û��������: \033[1;32m%15lld\033[m %s",
		AllUserMoneyB, MONEY_NAME);
	prints("%s", buf);
	pressanykey();
	if (readstrvalue(MC_BOSS_FILE, "admin", admin, IDLEN + 1) != 0)
		admin[0] = '\0';
	system_mail_file(fname, admin, "ͳ�Ʊ���", "deliver");
	flock(fileno(fp), LOCK_UN);
	fclose(fp);
	fclose(fw);
//      unlink(fname);

	return;
}

int
money_game()
{
	char ch, quit = 0;

	while (!quit) {
		money_show_stat("��������");
		move(t_lines - 1, 0);
		prints
		    ("\033[1;44m ѡ�� \033[1;46m [1]��Ϸ���ܿ��� [2]��Ϸ���ܹر� [Q]�뿪\033[m");
		ch = igetkey();
		switch (ch) {
		case '1':
			move(6, 4);
			prints
			    ("�����ͬ�������Ϸ�����Լ��������ܳ��ֵı仯����Ը����뵽��Ϸ��");
			move(8, 4);
			prints("�뿪����Ϸ����,���޾��ʵ�������");
			move(10, 4);
			prints
			    ("������Ϸ����֮����������ʹ�ý���������������Ϸ���ܵ�Ȩ����");
			move(12, 4);
			prints
			    ("��Ӧ�ģ����������ܽ�����������Ϸ��������������ʧ��");
			move(14, 4);
			prints
			    ("����������Ϸ���ܵ�ʱ�������е�������ҽ�����1:10�ı����һ�Ϊ��Ϸ�ҡ�");
			move(16, 4);
			if (askyn("��ȷʵҪ������Ϸ������?", NA, NA) == YEA) {
				if (myInfo->GetLetter == 0) {
					mcEnv->Treasury -=
					    (myInfo->cash + myInfo->credit +
					     myInfo->interest) * 9;
					myInfo->cash *= 10;
					myInfo->credit *= 10;
					myInfo->interest *= 10;
					myInfo->GetLetter = 1;
				} else
					myInfo->GetLetter = 1;
			}
			break;
		case '2':
			move(6, 4);
			prints
			    ("�������Ը����д�����Ϸ����ϣ����ʱ��Ϣһ��ʱ�䣬��ѡ��ر���Ϸ���ܡ�");
			move(8, 4);
			prints
			    ("�ر���Ϸ����֮����������ʹ�ý��������ڳ�ת�ʺʹ������������Ϸ���ܵ�Ȩ����");
			move(10, 4);
			prints
			    ("��Ӧ�ģ���Ҳ������Ϊ���������ڵ���Ϸ���ܶ��ܵ���ʧ��");
			move(12, 4);
			prints("�����ѡ���˹ر���Ϸ����");
			move(14, 4);
			prints
			    ("�����е���Ϸ�ҽ�����20:1�ı����һ�Ϊ������ң�����������Ϸ�����Խ���ʧ10%%��");
			move(16, 4);
			if (askyn("��ȷʵҪ�ر���Ϸ������?", NA, NA) == YEA) {
				if (!(myInfo->GetLetter == 0)) {
					mcEnv->Treasury +=
					    (myInfo->cash + myInfo->credit +
					     myInfo->interest) / 20 * 19;
					myInfo->cash /= 20;
					myInfo->credit /= 20;
					myInfo->interest /= 20;
					myInfo->GetLetter = 0;
					myInfo->robExp -= myInfo->robExp / 10;
					myInfo->begExp -= myInfo->begExp / 10;
				} else
					myInfo->GetLetter = 0;
			}
			break;
		case 'q':
		case 'Q':
			quit = 1;
			break;
		}
	}
	return 1;
}

int
money_hall()
{
	char ch, quit = 0;

	while (!quit) {
		nomoney_show_stat("���������������");
		move(6, 4);
		prints
		    ("������%s����������������������ͨ�����ء����������������֡�"
		     "\n    ��˵����������������������������¼�������",
		     CENTER_NAME);
		move(t_lines - 1, 0);
		prints
		    ("\033[1;44m ѡ�� \033[1;46m [1]���̰� [2]�а����� [3]��Ϸ���� [4]�ܹܰ칫�� [Q]�뿪\033[m");
		ch = igetkey();
		switch (ch) {
		case '1':
			show_top();
			break;
		case '2':
			showAt(12, 4, "\033[1;32m���ܿ����С�\033[m", YEA);
			break;
		case '3':
			money_game();
			break;
		case '4':
			money_admin();
			break;
		case 'q':
		case 'Q':
			quit = 1;
			break;
		}
	}
	return 0;
}

void
policeCheck(void)
{
	char passbuf[32];
	clear();
	nomoney_show_stat("����Ѳ��");
	move(5, 8);
	prints("=============== �� �� Ѳ �� ============");
	move(8, 0);
	prints
	    ("      ����ĳ��ٰ�Ƶ�������Ծ������о��ٽ���Ѳ�ӣ��̲����Ŀ�ꡣ"
	     "\n  ��ѽ�����ã������������˹�����");
	pressanykey();
	move(10, 6);
	if (random() % 3)
		prints("���û��ã�����Ŀ��б�ӵĴ�������߹�ȥ�ˡ�");
	else {
		prints
		    ("�������´�������һ�ۣ�����˹���˵������Ҫ���ȷ�ϣ���");
		getdata(11, 6, "����������(ֻ��һ�λ���): ",
			passbuf, PASSLEN, NOECHO, YEA);
		if (passbuf[0] == '\0' || passbuf[0] == '\n'
		    || !checkpasswd(currentuser->passwd,
				    currentuser->salt, passbuf)) {
			prints
			    ("\n      ����ŭ�𣺡������֤ʧ�ܣ�������һ�ˣ���"
			     "\n      555���㱻û�������ֽ𣬲��ұ����10���ӡ�");
			//mcLog("�ڶĳ�������������ʧ", myInfo->cash, "�ֽ�");
			myInfo->freeTime = time(NULL) + 600;
			mcEnv->Treasury += myInfo->cash;
			myInfo->cash = 0;
			myInfo->mutex = 0;
			unloadData(myInfo, sizeof (mcUserInfo));
			unloadData(mcEnv, sizeof (MC_Env));
			pressreturn();
			Q_Goodbye();
		} else if (random() % 2) {
			if (show_cake()) {
				prints
				    ("\n      ����ŭ�𣺡������֤ʧ�ܣ�������һ�ˣ���"
				     "\n      555���㱻û�������ֽ𣬲��ұ����10���ӡ�");
				//mcLog("��777������������ʧ", myInfo->cash, "�ֽ�");
				myInfo->freeTime = time(NULL) + 600;
				mcEnv->Treasury += myInfo->cash;
				myInfo->cash = 0;
				myInfo->mutex = 0;
				unloadData(myInfo, sizeof (mcUserInfo));
				unloadData(mcEnv, sizeof (MC_Env));
				pressreturn();
				Q_Goodbye();
			}
		}
		move(20, 0);
		prints("\n      ���ٵ��ͷ�����ţ�û��������ɡ���");
	}
	pressanykey();
}

#include "bbs.h"
#include "bbstelnet.h"

// ���н������Ե�ID�б��ļ�
#define ALL_TEST_FILE		MY_BBS_HOME "/etc/friendshipTest.list"
// ����ȫ�ֵ�top 10�ļ�
#define TOPTENHOT_FILE		MY_BBS_HOME "/etc/friendshipTest.top10"
#define NEW_IN_FILE		MY_BBS_HOME "/etc/friendshipTest.newin"

#define MAX_LINE_LENGTH		(STRLEN + 8)
#define QUESTION_START_FLAG	"#"
#define OPTION_START_FLAG	"$"
#define ANSWER_START_FLAG	"@"
#define VALUE_START_FLAG	"+"
#define HINT_START_FLAG		"?"
#define FILE_END_FLAG		"."
#define ERROR_CODE             -1
#define NEED_REPEAT             1
#define ABORT_ALL               2
#define PERSONAL                1
#define ALL_TESTER              0

// ���ݾɰ����������ļ�,�޷�ת��
//#define UPGRADE_OLD_DATA	1


// ����ժҪ��Ϣ
struct testDigest {
	short qNum, score;
	int passwd;
	char canSeeAnswer, canSeeTop10, allowError, feedback;
	int unused;
};

// ��������Ľṹ
struct questionInfo {
	short type, value;
	char question[STRLEN];
	char option[4][STRLEN];
	char hint[STRLEN];
	char answer[STRLEN];
};

struct TOP_TEN_UNIT {
	char name[IDLEN + 1];
	int value;
	time_t time;
};

static void welcomePage();
static void pageProfile();
static int createTest();
static int joinTest();
static int cancelTest();
static void setAttrib(struct testDigest *td, int x, int y);
static int dealWithQuestionInput(FILE * fp, int qCount, int isModify, struct questionInfo *qInfo);
static int showTopTen(int showType, char *top10File, char *userid);
static int showNextQuestion(FILE *fp, FILE *fp2, int qCount, int *score, struct testDigest *td);
static int sortTopTen(char *top10File, struct TOP_TEN_UNIT recordToAdd);
static int countValue(char *ans, char *buf, int value);
static int addToNewIn();
static int showNewIn();
static int topTenList();
static int isInTopTen(char *top10File, char *userid);
static int cmp_record(const void *a, const void *b);
static int mail_test_file(char *filepath);
static int mail_top10_file(char *my_top10_path);
static int management();
static int modifyQuestion();
static int manage_top10();
static int show_adage();
#ifdef  UPGRADE_OLD_DATA
static int runOnceToUpgrade();
#endif

int
friendshipTest()
{
	short ch, quit = 0;

	modify_user_mode(FRIENDTEST);
#ifdef UPGRADE_OLD_DATA
	runOnceToUpgrade();
#endif
	welcomePage();
	while (!quit) {
		pageProfile();
		move(t_lines - 1, 0);
		prints
		    ("\033[1;44m ѡ�� \033[1;46m [1]�½� [2]���� [3]TopTen [4]�鿴���� [5]���� [Q]�뿪\033[0m");
		ch = igetkey();
		switch (ch) {
		case '1':
			createTest();
			break;
		case '2':
			joinTest();
			break;
		case '3':
			topTenList();
			break;
		case '4':
			listfilecontent(ALL_TEST_FILE);
			FreeNameList();
			pressanykey();
			break;
		case '5':
			management();
			break;
		case 'q':
		case 'Q':
			quit = 1;
		}
	}
	return 0;
}

static void
welcomePage()
{
	clear();
	move(4, 4);
	prints("��������������ѵ����꣡");
	move(6, 4);
	prints
	    ("\033[1;31m���˽����Ѷ��٣�\033[0m  \033[1;31m˭������õ����ѣ�\033[0m");
	move(8, 4);
	prints("\033[1;33m�𰸾���������ԣ�\033[0m");
	// print heart-like logo
	move(12, 40);
	prints
	    ("      \033[36m  .\033[1;35m��\033[32m*\033[33m��\033[35m.   \033[m");
	move(13, 40);
	prints
	    (" \033[1;36m.\033[32m*\033[34m�� \033[32m*\033[31m.\033[34m* \033[0;37m   \033[1;36m��\033[0;37m   \033[m");
	move(14, 40);
	prints("\033[1;31m��\033[0;37m           \033[1;32m*    \033[m");
	move(15, 40);
	prints("\033[1;32m��\033[0;37m          \033[1;33m.��   \033[m");
	move(16, 40);
	prints
	    ("\033[1;35m��\033[34m*\033[32m. \033[0;37m  ��  \033[1;35m.      \033[m");
	move(17, 40);
	prints("    \033[1;33m��  \033[36m. ��      \033[m");
	pressanykey();
}

static void
pageProfile()
{
	clear();
	move(1, 30);
	prints("\033[1;32m��ӭ�����������\033[0m");
	move(2, 0);
	prints
	    ("\033[1;37m--------------------------------------------------------------------------------\033[0m");
}

static int
topTenList()
{
	short ch, quit = 0;

	while (!quit) {
		pageProfile();
		move(t_lines - 1, 0);
		prints
		    ("\033[1;44m ѡ�� \033[1;46m [1]��ǿ����TopTen [2]�����ǳ�TopTen [Q]�뿪\033[0m");
		ch = igetkey();
		switch (ch) {
		case '1':
			showTopTen(ALL_TESTER, TOPTENHOT_FILE, NULL);
			break;
		case '2':
			showNewIn();
			break;
		case 'q':
		case 'Q':
			quit = 1;
		}
	}
	return 0;
}

static int
management()
{
	short ch, quit = 0;

	while (!quit) {
		pageProfile();
		move(t_lines - 1, 0);
		prints
		    ("\033[1;44m ѡ�� \033[1;46m [1]�޸��趨 [2]ȡ������ [3]TopTen���� [Q]�뿪\033[0m");
		ch = igetkey();
		switch (ch) {
		case '1':
			modifyQuestion();
			break;
		case '2':
			cancelTest();
			break;
		case '3':
			manage_top10();
			break;
		case 'q':
		case 'Q':
			quit = 1;
		}
	}
	return 0;
}

static void setAttrib(struct testDigest *td, int x, int y) {
	char buf[STRLEN];

	move(x++, y);
	if (askyn("��������Ƿ���Ҫ����[\033[1;33m��ѡ�����뽫�Ļ�����\033[0m]��",
	     NA, NA) == YEA) {
		srandom(time(NULL));
		td->passwd = 100000 + random() % 900000;
		sprintf(buf, "����������Ե������ǣ�%d�������Խ����ż�ת�������ĺ��ѡ�",
			td->passwd);
		system_mail_buf(buf, strlen(buf), currentuser->userid, "�ҵ������������", currentuser->userid);
	} else
		td->passwd = 0;
	move(x++, y);
	if (askyn("�Ƿ񹫿��𰸣�", NA, NA) == YEA)
		td->canSeeAnswer = 1;
	else
		td->canSeeAnswer = 0;
	move(x++, y);
	if (askyn("�Ƿ񹫿������Ե����а�", YEA, NA) == YEA)
		td->canSeeTop10 = 1;
	else
		td->canSeeTop10 = 0;	
	move(x++, y);
	if (askyn("�Ƿ�Ļ����ѵĻش�", NA, NA) == YEA)
		td->feedback  = 1;
	else
		td->feedback = 0;
	move(x++, y);
	if (askyn("�ʴ�����Ƿ����\033[1;32m�ݴ�ģʽ\033[0m��", YEA, NA) ==  YEA)
		td->allowError = 1;
	else
		td->allowError = 0;
	return;
}

static int
createTest()
{
	int i, retv;
	char temp_path[256], home_path[256];
	char buf[STRLEN];	
	FILE *fp;
	struct testDigest td;
	struct questionInfo qInfo;

	pageProfile();
	move(4, 4);

	sethomefile(home_path, currentuser->userid, "friendshipTest.qest");
	if (file_exist(home_path)) {
		prints("���Ѿ������˲��ԡ����Ҫ�½�����ȡ����ǰ�����ȡ�");
		pressanykey();
		return 0;
	}
	if (askyn("Ҫ�����Լ������������", NA, NA) == NA) {
		return 0;
	}
	memset(&td, 0, sizeof(struct testDigest));
	while (1) {
		getdata(5, 4, "������[5-20]��", buf, 3, DOECHO, YEA);
		td.qNum = atoi(buf);
		if (td.qNum >= 5 && td.qNum <= 20)
			break;
	}
	setAttrib(&td, 6, 4);
	sethomefile(temp_path, currentuser->userid, "friendshipTest.tmp");
	fp = fopen(temp_path, "w");
	if (fp == NULL)
		return ERROR_CODE;
	fwrite(&td, sizeof(struct testDigest), 1, fp);

	move(13, 4);
	prints("\033[1;33m��ʼ����ɣ����濪ʼ����������⡣\033[0m");
	pressanykey();

	for (i = 0; i < td.qNum; i++) {
		do {
			memset(&qInfo, 0, sizeof(struct questionInfo));
			retv = dealWithQuestionInput(fp, i + 1, 0, &qInfo);
		} while (retv == NEED_REPEAT);

		if (retv == ABORT_ALL) {
			move(20, 4);
			if (askyn("\033[1;31m���Ҫ��������������\033[0m", NA, NA) == NA) {
				i--;
			} else {
				fclose(fp);
				unlink(temp_path);
				return 0;
			}
		}
		td.score += qInfo.value;
	}
	fputs("\n.", fp);
	rewind(fp);
	fwrite(&td, sizeof(struct testDigest), 1, fp);
	fclose(fp);

	pageProfile();
	move(4, 4);
	if (askyn("\033[1;32m��������ˣ�������Ĳ�����\033[0m", YEA, NA) == YEA) {
		rename(temp_path, home_path);
		addToNewIn();
		addtofile(ALL_TEST_FILE, currentuser->userid);
		move(6, 4);
		if(askyn("\033[1;35mҪ��Ҫ�������Ƿ�������?\033[0m", YEA, YEA) == YEA)
			ov_send();
		move(8, 4);
		prints("\033[1;36m���ˣ��Ͽ�ȥ����������������԰ɣ�\033[0m");
		pressanykey();
	} else {
		unlink(temp_path);
		move(6, 4);
		prints("......");
		pressanykey();
	}
	return 0;
}

static int
dealWithQuestionInput(FILE * fp, int qCount, int isModify, struct questionInfo *qInfo)
{
	int i;
	char buf[STRLEN];
	struct questionInfo newqInfo;

	pageProfile();
	if (isModify) {
		move(4, 4);
		prints("\033[1;36m�ع˵� %d ������:\033[0m", qCount);
		move(5, 4);
		if (qInfo->type == 'B')
			strcpy(buf, "�Ƿ�");
		else if (qInfo->type == 'Q')
			strcpy(buf, "�ʴ�");
		else
			strcpy(buf, "ѡ��");
		prints
		    ("\033[1;32m��������:\033[0m%4s    \033[1;32m��ֵ:\033[0m%2d",
		     buf, qInfo->value);
		move(6, 4);
		prints("\033[1;31m��������:\033[0m%s", qInfo->question + 5);	//���������Ե�����ַ���ʼ
		if (qInfo->type == 'B') {
			move(8, 8);
			prints("A. ��");
			move(9, 8);
			prints("B. ��");
		} else if (qInfo->type == 'S') {
			for (i = 0; i < 4; i++) {
				move(8 + i, 8);
				prints("%s", qInfo->option[i] + 1);
			}
		}
		move(12, 4);
		prints("\033[1;33m��    ʾ:\033[0m%s", qInfo->hint + 1);
		move(13, 4);
		prints("\033[1;35m��    ��:\033[0m%s", qInfo->answer + 1);
		pressanykey();
		move(20, 4);
		if (askyn("\033[1;36mȷ��Ҫ�޸ı�����?\033[0m", NA, NA) == NA)
			return 2;
		pageProfile();
	}
	move(4, 4);
	prints("\033[1;36m%s�� %d �����⣺\033[0m",
	       isModify ? "�޸�" : "�����", qCount);
	while (1) {
		getdata(5, 4,
			"\033[1;32m��ѡ����������[1. �Ƿ�  2. ѡ��  3. �ʴ�]��\033[0m",
			buf, 2, DOECHO, YEA);
		newqInfo.type = atoi(buf);
		if (newqInfo.type >= 1 && newqInfo.type <= 3)
			break;
	}
	while (1) {
		getdata(6, 4, "\033[1;32m����������\033[0m", newqInfo.question, STRLEN,
			DOECHO, YEA);
		if (newqInfo.question[0] != '\0')
			break;
	}
	while (1) {
		getdata(7, 4, "\033[1;32m�Ʒ�[5-20]��\033[0m", buf, 3, DOECHO,
			YEA);
		newqInfo.value = atoi(buf);
		if (newqInfo.value >= 5 && newqInfo.value <= 20)
			break;
	}
	getdata(8, 4, "������ʾ[��\033[1;36mENTER\033[0m�Թ�]��", newqInfo.hint, STRLEN,
		DOECHO, YEA);
	switch (newqInfo.type) {
	case 1:		//�Ƿ�                       
		while (1) {
			getdata(9, 4, "\033[1;31m��[A. �� B. ��]��\033[0m",
				buf, 2, DOECHO, YEA);
			newqInfo.answer[0] = toupper(buf[0]);
			if (newqInfo.answer[0] == 'A' || newqInfo.answer[0] == 'B')
				break;
		}
		getdata(10, 4,
			"\033[1;32m[S]���汾�� [E]���±༭���� [A]�������� [S]��\033[0m",
			buf, 2, DOECHO, YEA);
		if (buf[0] == 'e' || buf[0] == 'E') {
			return NEED_REPEAT;
		}
		if (buf[0] == 'a' || buf[0] == 'A') {
			return ABORT_ALL;
		}
		fprintf(fp, "%s%c%-2d.%s\n", QUESTION_START_FLAG, 'B', qCount,
			newqInfo.question);
		fprintf(fp, "%s%d\n", VALUE_START_FLAG, newqInfo.value);
		fprintf(fp, "%sA. ��\n%sB. ��\n", OPTION_START_FLAG,
			OPTION_START_FLAG);
		fprintf(fp, "%s%s\n", HINT_START_FLAG, newqInfo.hint);
		fprintf(fp, "%s%c\n", ANSWER_START_FLAG, newqInfo.answer[0]);
		break;
	case 2:		//ѡ��
		move(9, 4);
		prints("\033[1;32m����дѡ�\033[0m");
		for (i = 0; i < 4; i++) {
			while (1) {
				sprintf(genbuf, "%c. ", 'A' + i);
				getdata(10 + i, 8, genbuf, newqInfo.option[i],
					STRLEN, DOECHO, YEA);
				if (newqInfo.option[i][0] != '\0')
					break;
			}
		}
		while (1) {
			getdata(14, 4, "\033[1;31m��\033[0m[A B C D]��", buf,
				2, DOECHO, YEA);
			newqInfo.answer[0] = toupper(buf[0]);
			if (newqInfo.answer[0] >= 'A' && newqInfo.answer[0] <= 'D')
				break;
		}

		getdata(15, 4,
			"\033[1;32m[S]���汾�� [E]���±༭���� [A]�������� [S]��\033[0m",
			buf, 2, DOECHO, YEA);
		if (buf[0] == 'e' || buf[0] == 'E') {
			return NEED_REPEAT;
		}
		if (buf[0] == 'a' || buf[0] == 'A') {
			return ABORT_ALL;
		}
		fprintf(fp, "%s%c%-2d.%s\n", QUESTION_START_FLAG, 'S', qCount,
			newqInfo.question);
		fprintf(fp, "%s%d\n", VALUE_START_FLAG, newqInfo.value);
		fprintf(fp, "%sA. %s\n%sB. %s\n%sC. %s\n%sD. %s\n",
			OPTION_START_FLAG, newqInfo.option[0], OPTION_START_FLAG,
			newqInfo.option[1], OPTION_START_FLAG, newqInfo.option[2],
			OPTION_START_FLAG, newqInfo.option[3]);
		fprintf(fp, "%s%s\n", HINT_START_FLAG, newqInfo.hint);
		fprintf(fp, "%s%c\n", ANSWER_START_FLAG, newqInfo.answer[0]);
		break;
	case 3:		//�ʴ�                       
		while (1) {
			getdata(9, 4, "\033[1;31m�𰸣�\033[0m", newqInfo.answer, STRLEN,
				DOECHO, YEA);
			if (newqInfo.answer[0] != '\0') {
				break;
			}
		}
		getdata(10, 4,
			"\033[1;32m[S]���汾�� [E]���±༭���� [A]�������� [S]��\033[0m",
			buf, 2, DOECHO, YEA);
		if (buf[0] == 'e' || buf[0] == 'E') {
			return NEED_REPEAT;
		}
		if (buf[0] == 'a' || buf[0] == 'A') {
			return ABORT_ALL;
		}
		fprintf(fp, "%s%c%-2d.%s\n", QUESTION_START_FLAG, 'Q', qCount,
			newqInfo.question);
		fprintf(fp, "%s%d\n", VALUE_START_FLAG, newqInfo.value);
		fprintf(fp, "%s\n", OPTION_START_FLAG);
		fprintf(fp, "%s%s\n", HINT_START_FLAG, newqInfo.hint);
		fprintf(fp, "%s%s\n", ANSWER_START_FLAG, newqInfo.answer);
		break;
	}
	if(qInfo != NULL)	// ����createʱͳ���ܷ���
		qInfo->value = newqInfo.value;
	pressanykey();
	return 0;
}

static int
cancelTest()
{
	char home_path[256], my_top10_path[256];

	pageProfile();
	sethomefile(my_top10_path, currentuser->userid, "friendshipTest.top10");
	move(4, 4);
	sethomefile(home_path, currentuser->userid, "friendshipTest.qest");
	if (!file_exist(home_path)) {
		prints("����û�н���������ԡ�");
		pressanykey();
		return 0;
	}
	if (askyn("\033[5;31m���棺\033[0mȷ��Ҫȡ����ǰ������", NA, NA) ==
	    YEA) {
		move(5, 4);
		if (askyn("\033[1;32m�Ƿ�Ļز��Ժ����а�\033[0m", NA, NA) ==
		    YEA) {
			mail_test_file(home_path);
			mail_top10_file(my_top10_path);
		}
		unlink(home_path);
		unlink(my_top10_path);
		del_from_file(ALL_TEST_FILE, currentuser->userid);
		saveuservalue(currentuser->userid, "friendTest", "0");
		move(7, 4);
		prints("ȡ��������ɡ�");
		pressanykey();
	}
	return 0;
}

static int
isInTopTen(char *top10File, char *userid)
{
	FILE *fp;
	struct TOP_TEN_UNIT ttu[10];
	int find = 0;
	int i, n;

	fp = fopen(top10File, "rb");
	if (fp == NULL)
		return 0;
	n = fread(ttu, sizeof (struct TOP_TEN_UNIT), 10, fp);
	fclose(fp);
	for (i = 0; i < n; i++) {
		if (!strcmp(ttu[i].name, userid)) {
			find = 1;
			break;
		}
	}
	return find;
}

static int
joinTest()
{
	char home_path[256], top10_path[256], temp_path[256];
	char buf[STRLEN], uident[IDLEN + 1];
	FILE *fp, *fp2 = NULL;
	struct TOP_TEN_UNIT record;
	int retv, score, iCount;
	struct userec *lookupuser;
	struct testDigest td;

	pageProfile();
	move(4, 4);
	usercomplete("����˭�Ĳ��ԣ�", uident);
	if (uident[0] == '\0')
		return 0;
	move(6, 4);
	if (!getuser(uident, &lookupuser)) {
		prints("�����ʹ���ߴ���...");
		pressanykey();
		return 0;
	}
	sethomefile(home_path, uident, "friendshipTest.qest");
	if (!file_exist(home_path)) {
		prints("��������û�н���������ԡ�");
		pressanykey();
		return 0;
	}
	fp = fopen(home_path, "r");
	if (fp == NULL)
		return ERROR_CODE;
	fread(&td, sizeof(struct testDigest), 1, fp);
	if (td.passwd != 0) {
		getdata(6, 4,
			"\033[1;31m��������Ҫ������ܽ��롣���������룺\033[0m",
			buf, 10, NOECHO, YEA);
		if (td.passwd != atoi(buf)) {
			move(7, 4);
			prints("�������!");
			pressanykey();
			fclose(fp);
			return 0;
		}
	}
	sethomefile(top10_path, uident, "friendshipTest.top10");
	if (td.canSeeTop10) {
		showTopTen(PERSONAL, top10_path, uident);
	}
	move(t_lines - 3, 15);
	if (isInTopTen(top10_path, currentuser->userid)) {
		prints("\033[1;36m���Ѿ�����������,��ϲŶ!\033[0m");
		pressanykey();
		fclose(fp);
		return 0;
	}
	if (askyn("\033[1;36m��ʼ������?\033[0m", NA, NA) == NA) {
		fclose(fp);
		return 0;
	}
	if(td.feedback) {
		clear();
		move(6, 4);
		prints("\033[5;31mע��: \033[0m���ڱ����ԵĻش𽫻ᱻ�Ļظ����������!");
		pressanykey();

		sprintf(temp_path, MY_BBS_HOME "/bbstmpfs/tmp/%s.friendshipTest.fbk", uident);
		fp2 = fopen(temp_path, "w");	//���ﲻ�ü��fp2�Ƿ�NULL
	}

	for (iCount = 0, score = 0; iCount < td.qNum; iCount++) {
		retv = showNextQuestion(fp, fp2, iCount + 1, &score, &td);
		if (retv != 0)
			break;
	}
	fclose(fp);

	if(td.feedback && fp2 != NULL) { // �Ļ�����
		fclose(fp2);
		system_mail_file(temp_path, uident, "����������ѵ�����", currentuser->userid);
		unlink(temp_path);
	}

	move(20, 4);
	if (iCount == td.qNum) {
		prints
		    ("\033[1;31m�ܷ֣�\033[0m%d  \033[1;32m���ĵ÷��ǣ�\033[0m%d",
		     td.score, score);
		//������������1
		if (readuservalue(uident, "friendTest", buf, 10) != 0) {
			sprintf(buf, "%d", 1);
		} else {
			sprintf(buf, "%d", atoi(buf) + 1);
		}
		saveuservalue(uident, "friendTest", buf);
		strcpy(record.name, uident);
		record.value = atoi(buf);
		record.time = time(NULL);
		sortTopTen(TOPTENHOT_FILE, record);
		//���˲��Գɼ�����
		strcpy(record.name, currentuser->userid);
		record.value = score;
		record.time = time(NULL);
		sortTopTen(top10_path, record);
		sprintf(buf, "%d %s %s", -(score/td.score) * 50, currentuser->userid, uident);
		friendslog(buf);
	} else {
		prints("\033[1;31m��û��������еĲ�����Ŀ��\033[0m");
	}
	pressanykey();
	return 0;
}

static int
showNextQuestion(FILE *fp, FILE *fp2, int qCount, int *score, struct testDigest *td) 
{
	int iCount, q_score = 0;
	char ans, buf[STRLEN];
	struct questionInfo qInfo;

	if (qCount % 6 == 0)
		show_adage();
	pageProfile();

	memset(&qInfo, 0, sizeof(struct questionInfo));
	fgets(qInfo.question, MAX_LINE_LENGTH, fp);	// ����������
	if(qInfo.question[0] != '#')	// ���һ�¸�ʽ
		return ERROR_CODE;
	qInfo.type = qInfo.question[1];
	move(4, 4);
	prints("��ش�� %d �����⣺", qCount);
	move(5, 4);

	prints("\033[1;32m����������%s\033[0m", qInfo.question + 5); // �����+5��ʼ
	fgets(buf, MAX_LINE_LENGTH, fp);	//��ֵ��
	qInfo.value = atoi(buf + 1);	//line+1�Ƿ�ֵ
	move(6, 4);
	prints("�Ʒ֣�%d", qInfo.value);
	switch (qInfo.type) {
	case 'B':
		for (iCount = 0; iCount < 2; iCount++) {
			fgets(qInfo.option[iCount], MAX_LINE_LENGTH, fp);	//ѡ���� 
			move(7 + iCount, 8);
			prints("%s", qInfo.option[iCount] + 1);
		}
		fgets(qInfo.hint, MAX_LINE_LENGTH, fp);	//��ʾ��
		move(9, 4);
		prints("\033[1;35m��ʾ��%s\033[0m", qInfo.hint + 1);
		fgets(qInfo.answer, MAX_LINE_LENGTH, fp);	//����
		while (1) {
			getdata(9, 4, "\033[1;31m��[A B]��\033[0m", buf, 2,
				DOECHO, YEA);
			ans = toupper(buf[0]);
			if (ans == 'A' || ans == 'B') {
				break;
			}
		}

		if (qInfo.answer[1] == ans) {
			q_score = qInfo.value;
			*score += qInfo.value;
		}
		break;
	case 'S':
		for (iCount = 0; iCount < 4; iCount++) {
			fgets(qInfo.option[iCount], MAX_LINE_LENGTH, fp);	//ѡ���� 
			move(7 + iCount, 8);
			prints("%s", qInfo.option[iCount] + 1);
		}
		fgets(qInfo.hint, MAX_LINE_LENGTH, fp);	//��ʾ��
		move(11, 4);
		prints("\033[1;35m��ʾ��%s\033[0m", qInfo.hint + 1);
		fgets(qInfo.answer, MAX_LINE_LENGTH, fp);	//����
		while (1) {
			getdata(12, 4, "\033[1;31m��[A B C D]��\033[0m", buf,
				2, DOECHO, YEA);
			ans = toupper(buf[0]);
			if (ans == 'A' || ans == 'B' || ans == 'C'
			    || ans == 'D') {
				break;
			}
		}
		if (qInfo.answer[1] == ans) {
			q_score = qInfo.value;
			*score += qInfo.value;
		}
		break;
	case 'Q':
		fgets(qInfo.option[0], MAX_LINE_LENGTH, fp);	//ѡ����
		fgets(qInfo.hint, MAX_LINE_LENGTH, fp);	//��ʾ��
		move(8, 4);
		prints("\033[1;35m��ʾ��%s\033[0m", qInfo.hint + 1);
		fgets(qInfo.answer, MAX_LINE_LENGTH, fp);	//����
		while (1) {
			getdata(9, 4, "\033[1;31m�𰸣�\033[0m", buf, STRLEN,
				DOECHO, YEA);
			if (buf[0] != '\0') {
				move(11, 4);
				if (askyn("ȷ��������", NA, NA) == YEA)
					break;
			}
		}
		qInfo.answer[strlen(qInfo.answer) - 1] = '\0';
		if (!td->allowError) {
			if (!strcmp(buf, qInfo.answer + 1)) {
				q_score = qInfo.value;
				*score += qInfo.value;
			}
		} else {	//�ݴ�ģʽ
			q_score = countValue(qInfo.answer + 1, buf, qInfo.value);
			*score += q_score;
		}
		break;
	}
	if (td->canSeeAnswer) {
		move(15, 4);
		prints("\033[1;32m��ȷ���ǣ�\033[1;36m%s\033[0m", qInfo.answer + 1);
		if (q_score > 0) {
			move(16, 4);
			prints
			    ("\033[1;33m��ϲ���������õ��� \033[1;36m%d\033[1;33m �֣�\033[0m",
			     q_score);
		} else {
			move(16, 4);
			prints("\033[1;36m�����أ�û�д��...\033[0m");
		}
	}
	if (td->feedback && fp2 != NULL) {
		fprintf(fp2, "\033[1;32m%d. %s\033[0m", qCount, qInfo.question + 5);
		if(qInfo.type != 'Q') {
			for(iCount = 0; qInfo.option[iCount][0] != '\0' && iCount < 4; iCount++)
				fprintf(fp2, "    %s", qInfo.option[iCount] + 1);
		}
		fprintf(fp2, "���ѻش�: %s\n\033[1;33m��ȷ��:\033[0m %s\n\n", buf, qInfo.answer + 1);
	}
	pressanykey();
	return 0;
}

static int
showTopTen(int showType, char *top10File, char *userid)
{
	FILE *fp;
	struct TOP_TEN_UNIT recordArray[10];
	int i, n;
	time_t t;
	char home_path[256];
	struct testDigest td;

	pageProfile();
	move(4, 28);
	if(showType == PERSONAL) {
		sethomefile(home_path, userid, "friendshipTest.qest");
		fp = fopen(home_path, "r");
		if(fp == NULL)
			return -1;
		fread(&td, sizeof(struct testDigest), 1, fp);	
		fclose(fp);
		prints("%s�������TOP-TEN (�ܷ�%d)", userid, td.score);
	} else
		prints("%s�������TOP-TEN", MY_BBS_NAME);
	move(6, 15);
	if (showType == PERSONAL) {
		prints("\033[1;32m����  %-12s  �ɼ�  ����ʱ��\033[0m",
		       "UserID");
	} else {
		prints("\033[1;32m����  %-12s  ����  �������\033[0m",
		       "UserID");
	}
	fp = fopen(top10File, "rb");
	if (fp == NULL) {
		return ERROR_CODE;
	}
	n = fread(recordArray, sizeof (struct TOP_TEN_UNIT), 10, fp);
	fclose(fp);
	for (i = 0; i < n; i++) {
		move(8 + i, 15);
		t = recordArray[i].time;
		prints("%-4d  %-12s  %-4d  %s", i + 1, recordArray[i].name,
		       recordArray[i].value, ctime(&t));
	}
	pressanykey();
	return 0;
}

static int
cmp_record(const void *a, const void *b)
{
	return ((struct TOP_TEN_UNIT *) b)->value -
	    ((struct TOP_TEN_UNIT *) a)->value;
}

static int
sortTopTen(char *top10File, struct TOP_TEN_UNIT recordToAdd)
{
	FILE *fp;
	struct TOP_TEN_UNIT recordArray[11];
	int n = 0, find = 0, i;

	fp = fopen(top10File, "rb");
	if (fp != NULL) {
		n = fread(recordArray, sizeof (struct TOP_TEN_UNIT), 10, fp);
		fclose(fp);
		for (i = 0; i < n; i++) {
			if (!strcmp(recordArray[i].name, recordToAdd.name)) {
				recordArray[i] = recordToAdd;
				find = 1;
				break;
			}
		}
		if (!find)
			recordArray[n] = recordToAdd;	//n+1������
		else
			n--;	//n������
		qsort(recordArray, n + 1, sizeof (struct TOP_TEN_UNIT),
		      cmp_record);
	} else {
		recordArray[0] = recordToAdd;
	}
	fp = fopen(top10File, "wb");
	if (fp == NULL)
		return ERROR_CODE;
	fwrite(recordArray, sizeof (struct TOP_TEN_UNIT), min(n + 1, 10), fp);
	fclose(fp);
	return 0;
}

static int
countValue(char *ans, char *buf, int value)
{
	int lenAns, lenBuf;
	int diff;

	if (!strcasecmp(ans, buf))
		return value;
	lenAns = strlen(ans);
	lenBuf = strlen(buf);
	diff = abs(lenAns - lenBuf);

	if (lenAns > 2 * lenBuf || lenBuf > 2 * lenAns)
		return 0;

	if (lenAns > lenBuf) {
		return strcasestr(ans,
				  buf) == NULL ? 0 : value / (1 + diff / 2);
	} else {
		return strcasestr(buf,
				  ans) == NULL ? 0 : value / (1 + diff / 2);
	}
}

static int
showNewIn()
{
	FILE *fp;
	int iCount, n;
	struct TOP_TEN_UNIT recordArray[10];
	time_t t;

	pageProfile();
	move(4, 32);
	prints("\033[1;35m�����ǳ�TOP-TEN\033[0m");
	move(6, 20);
	prints("\033[1;32m%-12s  ����ʱ��", "UserID");
	fp = fopen(NEW_IN_FILE, "rb");
	if (fp == NULL) {
		return ERROR_CODE;
	}
	n = fread(recordArray, sizeof (struct TOP_TEN_UNIT), 10, fp);
	fclose(fp);
	for (iCount = 0; iCount < n; iCount++) {
		t = recordArray[iCount].time;
		move(8 + iCount, 20);
		prints("%-12s  %s", recordArray[iCount].name, ctime(&t));
	}
	pressanykey();
	return 0;
}

static int
addToNewIn()
{
	FILE *fp;
	struct TOP_TEN_UNIT recordArray[10];
	int n = 0;

	strcpy(recordArray[0].name, currentuser->userid);
	recordArray[0].value = 0;
	recordArray[0].time = time(0);

	fp = fopen(NEW_IN_FILE, "rb");
	if (fp != NULL) {
		n = fread(recordArray + 1, sizeof (struct TOP_TEN_UNIT), 9, fp);
		fclose(fp);
	}
	fp = fopen(NEW_IN_FILE, "wb");
	if (fp == NULL)
		return ERROR_CODE;
	fwrite(recordArray, sizeof (struct TOP_TEN_UNIT), n + 1, fp);
	fclose(fp);
	return 0;
}

static int mail_test_file(char *filepath) {
	FILE *fp, *fp2;
	char temp_path[256], line[MAX_LINE_LENGTH];
	struct testDigest td;
	int i = 0;

	fp = fopen(filepath, "r");
	if(fp == NULL)
		return ERROR_CODE;
	sprintf(temp_path, MY_BBS_HOME "/bbstmpfs/tmp/%s.friendtest.mail", currentuser->userid);

	fp2 = fopen(temp_path, "w");
	if(fp2 == NULL) {
		fclose(fp);
		return ERROR_CODE;
	}

	fread(&td, sizeof(struct testDigest), 1, fp);
	fprintf(fp2, "����:%4d\n�ܷ�:%4d\n", td.qNum, td.score);
	printf("%d  %d", td.qNum, td.score);

	while(i++ < td.qNum) {
	        fgets(line, MAX_LINE_LENGTH, fp);
		if(line == NULL)
			break;
		if(line[0] == '.')
			break;
		if(line[0] == '#') {
			fputs(line + 2, fp2);
			fgets(line, MAX_LINE_LENGTH, fp);
			fprintf(fp2, "����:%d\n", atoi(line + 1));
			while(1) {
				fgets(line, MAX_LINE_LENGTH, fp);
				if(line == NULL) {
					fclose(fp);
					fclose(fp2);
					return -1;
				}
				if(line[0] == '$')
					fputs(line + 1, fp2);
				else
					break;
			}
			if(line[0] == '?')
				fprintf(fp2, "��ʾ:%s\n", line + 1);
			fgets(line, MAX_LINE_LENGTH, fp);
			fprintf(fp2, "��:%s\n", line + 1);
		}
	}
	fclose(fp);
	fclose(fp2);
	system_mail_file(temp_path, currentuser->userid, "�ҵ��������(��Ŀ)", currentuser->userid);
	unlink(temp_path);
	return 0;
}

static int
mail_top10_file(char *my_top10_path)
{
	FILE *fp;
	int n = 0, i;
	char temp_path[256];
	struct TOP_TEN_UNIT recordArray[10];

	fp = fopen(my_top10_path, "rb");
	if (fp == NULL)
		return ERROR_CODE;
	n = fread(recordArray, sizeof (struct TOP_TEN_UNIT), 10, fp);
	fclose(fp);
	sprintf(temp_path, MY_BBS_HOME "/bbstmpfs/tmp/%s.top10", currentuser->userid);
	fp = fopen(temp_path, "wb");
	if (fp == NULL)
		return ERROR_CODE;
	fprintf(fp, "����  %-12s  �ɼ�     ����ʱ��\n", "UserID");
	for (i = 0; i < n; i++)
		fprintf(fp, "%4d  %-12s  %4d     %s\n", i + 1,
			recordArray[i].name, recordArray[i].value,
			ctime(&(recordArray[i].time)));
	fclose(fp);
	system_mail_file(temp_path, currentuser->userid, "�ҵ��������(������)", currentuser->userid);
	unlink(temp_path);
	return 0;
}

static int
modifyQuestion()
{
	FILE *fp = NULL, *fp2 = NULL;
	char home_path[256], temp_path[256], buf[STRLEN], line[MAX_LINE_LENGTH];
	int i, num_to_change = 0, add_new = 0, retv = 999, old_value;
	struct testDigest td;
	struct questionInfo qInfo;

	sethomefile(home_path, currentuser->userid, "friendshipTest.qest");
	sethomefile(temp_path, currentuser->userid, "friendshipTest.qest.tmp");
	fp = fopen(home_path, "r");
	if (fp == NULL)
		return ERROR_CODE;
	fread(&td, sizeof(struct testDigest), 1, fp);
	move(4, 4);
	prints("\033[1;33m��ǰ��������:\033[0m");
	move(5, 4);
	prints
	    ("\033[1;35m(N)��    ��:%-3d    (P)��    ��:%-6d    (A)������:%-4s",
	     td.qNum, td.passwd, td.canSeeAnswer ? "Yes" : "No");
	move(6, 4);
	prints
	    ("\033[1;35m(T)��������:%-3s    (M)�ݴ�ģʽ:%-6s    (E)�˳�\033[0m",
	     td.canSeeTop10 ? "Yes" : "No", td.allowError ? "Yes" : "No");
	getdata(8, 4, "��Ҫ����ʲô����[\033[1;33m������ĸ����\033[0m]:",
		buf, 2, DOECHO, YEA);
	switch (toupper(buf[0])) {
	case 'N':
		while(1) {
			getdata(9, 4, "�޸����[1-20]:", buf, 3, DOECHO, YEA);			
			num_to_change = atoi(buf);
			if(num_to_change >= 1 && num_to_change <= 20)
				break;
		}
		if (num_to_change > td.qNum) {
			move(10, 4);
			if (askyn("�⽫����µ�����,������?", YEA, NA) == NA)
				break;
			add_new = 1;
		}
		fp2 = fopen(temp_path, "w");
		if (fp2 == NULL) {
			fclose(fp);
			return ERROR_CODE;
		}
		fwrite(&td, sizeof(struct testDigest), 1, fp2);

		while (1) {
			fgets(line, MAX_LINE_LENGTH, fp);
			if(line == NULL)
				break;
			if (line[0] == '.')
				break;
			if (line[0] == '#' && atoi(line + 2) == num_to_change) {
				memset(&qInfo, 0, sizeof(struct questionInfo));
				qInfo.type = line[1];
				strncpy(qInfo.question, line, STRLEN - 1);
				fgets(line, MAX_LINE_LENGTH, fp);
				qInfo.value = atoi(line + 1);
				old_value = qInfo.value;
				
				if (qInfo.type == 'B') {
					fgets(line, MAX_LINE_LENGTH, fp);
					fgets(line, MAX_LINE_LENGTH, fp);
				} else if (qInfo.type == 'S') {
					for (i = 0; i < 4; i++)
						fgets(qInfo.option[i], MAX_LINE_LENGTH, fp);
				} else {
					fgets(line, MAX_LINE_LENGTH, fp);
				}
				fgets(qInfo.hint, MAX_LINE_LENGTH, fp);
				fgets(qInfo.answer, MAX_LINE_LENGTH, fp);
				do {
					retv = dealWithQuestionInput(fp2, num_to_change, 1, &qInfo);
				} while (retv == NEED_REPEAT);
				td.score += qInfo.value - old_value;
			} else {
				if(line[0] != '\n')	// ���в���copy��ȥ
					fputs(line, fp2);
			}
		}
		if (add_new)
			for(i = td.qNum + 1; i <= num_to_change; i++) {
				do {
					memset(&qInfo, 0, sizeof(struct questionInfo));
					retv = dealWithQuestionInput(fp2, i, 0, &qInfo);
				} while (retv == NEED_REPEAT);
				if (retv == ABORT_ALL) {
					move(20, 4);
                        		if (askyn("\033[1;31m���Ҫ��������������\033[0m", NA, NA) == NA) {
                                		i--;
                        	} else {
                                	fclose(fp);
					fclose(fp2);
                                	unlink(temp_path);
                                	return 0;
                        	}
                	}
			td.qNum = num_to_change;
			td.score += qInfo.value;
		}
		fputs("\n.", fp2);
		rewind(fp2);
		fwrite(&td, sizeof(struct testDigest), 1, fp2);
		fclose(fp2);
		break;
	case 'P':
	case 'A':
	case 'T':
	case 'M':
		setAttrib(&td, 9, 4);
		move(16, 4);
		if (askyn("ȷ���޸���?", NA, NA) == NA)
			break;
		fp2 = fopen(temp_path, "w");
		if (fp2 == NULL) {
			fclose(fp);
			return ERROR_CODE;
		}
		fwrite(&td, sizeof(struct testDigest), 1, fp2);
		while (fgets(line, MAX_LINE_LENGTH, fp)) {
			fputs(line, fp2);
		}
		fclose(fp2);
		retv = 0;
		break;
	} // end of switch...

	fclose(fp);
	move(21, 4);
	if (retv == 0) {
		unlink(home_path);
		rename(temp_path, home_path);
		prints("�޸����!");
	} else {
		unlink(temp_path);
		prints("ȡ���޸�...");
	}
	pressanykey();
	return 0;
}

static int
manage_top10()
{
	char top10_path[256];
	int ch, isSYSOP;

	isSYSOP = USERPERM(currentuser, PERM_SYSOP);

	move(4, 4);
	prints("\033[1;36m��������ĸ�Top Ten��ʾ:\033[0m");
	move(6, 8);
	prints("1. �ҵĲ���Top Ten        Q. �˳�");
	if (isSYSOP) {
		move(7, 8);
		prints("2. ȫվ����Top Ten        3. ���¼���Top Ten");
	}
	ch = igetkey();
	switch (ch) {
	case '1':
		sethomefile(top10_path, currentuser->userid,
			    "friendshipTest.top10");
		break;
	case '2':
		if (isSYSOP)
			strcpy(top10_path, TOPTENHOT_FILE);
		break;
	case '3':
		if (isSYSOP)
			strcpy(top10_path, NEW_IN_FILE);
		break;
	default:
		return 0;
	}
	move(9, 4);
	if (askyn("ȷ�������?", NA, NA) == NA)
		return 0;
	unlink(top10_path);
	move(10, 4);
	prints("�������.");
	pressanykey();
	return 0;
}

static int
show_adage()
{
	int select;
	char *ptr;
	char adages[][100] = {
		"������Զ�����������ֺ���\t�����",
		"һ�������Ѻ��ھ���α���˾������ܶԹ�����ϡ�\t����������",
		"���˲�����Ľ��ͣ�����������Ľ��͡�\tӡ�ȸ���",
		"һ���������������ʵ�����ѣ��͵���������������������\t����ϣ������������˹",
		"�����ѿ��ָ���һ�����ѣ����õ��������֡�\t��Ӣ����.���",
		"�������ǳ���һ���������£������ֵ�һ���ǳ\t��Ӣ����.���",
		"������һ��ɳ����������飬Ҫ���������ͻȻ�������������Ϊ�ɿ���\t��Ӣ������٣�������",
		"����ͻ���һ�������ǵ�һ��ȽϺá�Խ��������Խʹ��������ҲԽ�ܳ��á�\tϯĽ��",
		"����һ��һ����/��Щ���Ӳ�����,һ�仰һ����/һ����һ���ơ�\t�ܻ��������ѡ�",
		"���Ѱ�����/�������������/����������ܲ���/��������ҡ�\t���˷�����ѡ�"
	};

	pageProfile();
	move(4, 4);
	prints("���Ѹ��Թ�����:");
	srandom(time(NULL));
	select = random() % 10;
	ptr = strstr(adages[select], "\t");
	*ptr = '\0';
	move(8, 10);
	prints("%s", adages[select]);
	move(10, 40);
	prints("----%s", ptr + 1);
	pressanykey();
	return 0;
}

#ifdef  UPGRADE_OLD_DATA
// ����оɰ������ļ�,�����иú���һ��,�Ա��������е������ļ�
static int runOnceToUpgrade() {
	FILE *fp, *fp_r, *fp_w;
	int fd;
	int qNum, passwd, canSeeAnswer, canSeeTop10, allowError;
	int skipped = 0;
	char userid[IDLEN + 1], buf[MAX_LINE_LENGTH], home_path[256], temp_path[256], *ptr;
	struct testDigest td;

	fp = fopen(ALL_TEST_FILE, "r");
	if(fp == NULL)
		return -1;
	fd = fileno(fp);
	if(flock(fd, LOCK_EX) == -1) {
		fclose(fp);
		return -1;
	}
	while(fgets(buf, STRLEN, fp)) {
		ptr = strchr(buf, '\n');
		if(ptr)
			*ptr = '\0';
		snprintf(userid, IDLEN + 1, "%s", buf);
		sethomefile(home_path, userid, "friendshipTest.qest");
		fp_r = fopen(home_path, "r");
		if(fp_r == NULL)
			continue;
		fseek(fp_r, -2, SEEK_END);
		fread(buf, 2, 1, fp_r);
		if(!strncmp(buf, "\n.", 2)) {	// �Ѿ����������ļ�
			fclose(fp_r);
			continue;
		}
		rewind(fp_r);
		fscanf(fp_r, "%d %d %d %d %d\n", &qNum, &passwd, &canSeeAnswer,
               		&canSeeTop10, &allowError);
		memset(&td, 0, sizeof(struct testDigest));
		td.qNum = qNum;
		td.score = 0;
		td.passwd = passwd;
		td.canSeeAnswer = canSeeAnswer;
		td.canSeeTop10 = canSeeTop10;
		td.allowError = allowError;

		sethomefile(temp_path, userid, "friendshipTest.qest.tmp");
		fp_w = fopen(temp_path, "w");
		if(fp_w == NULL) {
			fclose(fp_r);
			skipped = 1;
			continue;
		}
		fwrite(&td, sizeof(struct testDigest), 1, fp_w);
		while(fgets(buf, MAX_LINE_LENGTH, fp_r)) {
			if(buf[0] == '+')
				td.score += atoi(buf + 1);
			fputs(buf, fp_w);
		}
		fputs("\n.", fp_w);
		rewind(fp_w);
		fwrite(&td, sizeof(struct testDigest), 1, fp_w);
		fclose(fp_r);
		fclose(fp_w);
		rename(temp_path, home_path);
	}
	flock(fd, LOCK_UN);
	fclose(fp);
	return skipped;
}
#endif

		

		
		
		
		






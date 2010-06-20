#include "mc.h"

static int money_big();

// ------------------------  ��Ʊ  -------------------------- // 
//part one: 36_7 
static int
valid367Bet(char *buf)
{
	int i, j, temp[7], slot = 0;

	if (strlen(buf) != 20)	//  ���ȱ���Ϊ20= 2 * 7 + 6 
		return 0;
	for (i = 0; i < 20; i += 3) {	//  ������ʽ������ȷ 
		if (i + 2 != 20)
			if (buf[i + 2] != '-')	// �ָ�������ȷ 
				return 0;
		if (!isdigit(buf[i]) || !isdigit(buf[i + 1]))	// �������� 
			return 0;
		temp[slot] = (buf[i] - '0') * 10 + (buf[i + 1] - '0');
		if (temp[slot] > 36)
			return 0;
		slot++;
	}
	for (i = 0; i < 7; i++) {	// �������ظ� 
		for (j = 0; j < 7; j++)
			if (temp[j] == temp[i] && i != j)
				return 0;
	}
	return 1;
}

static int
parse367Bet(char *prizeSeq, char *bet, struct LotteryRecord *LR)
{
	int i, j, count = 0;
	int len = strlen(prizeSeq);

	if (strlen(bet) != len)
		return 0;
	for (i = 0; i + 1 < len; i = i + 3) {
		for (j = 0; j + 1 < len; j = j + 3) {
			if (bet[i] == prizeSeq[j]
			    && bet[i + 1] == prizeSeq[j + 1])
				count++;
		}
	}
	if (count >= 3 && count <= 7)
		LR->betCount[7 - count] = 1;
	return count;
}

static void
make367Seq(char *prizeSeq)
{
	int i, j, num, slot = 0, success;
	int temp[7];

	for (i = 0; i < 7; i++) {
		do {		//  ���ֲ�����ͬ 
			success = 1;
			num = 1 + rand() % 36;
			for (j = 0; j <= slot; j++) {
				if (num == temp[j]) {
					success = 0;
					break;
				}
			}
			if (success)
				temp[slot++] = num;
		}
		while (!success);
		prizeSeq[3 * i] = (char) (num / 10 + '0');
		prizeSeq[3 * i + 1] = (char) (num % 10 + '0');
		if (i != 6)
			prizeSeq[3 * i + 2] = '-';
		else
			prizeSeq[3 * i + 2] = '\0';
	}
	sprintf(genbuf, "��������ǣ�  %s  �����н�����", prizeSeq);
	deliverreport("����Ʊ������36ѡ7��Ʊҡ�����", genbuf);
}

//part two: soccer 
static int
validSoccerBet(char *buf)
{
	int i, count = 0, meetSeperator = 1;
	int first = 0, second = 0;

	if (buf[0] == '\0')
		return 0;
	for (i = 0; i < strlen(buf); i++) {
		if (buf[i] == '-') {
			if (meetSeperator == 1)	//�����������-���϶�����ȷ 
				return 0;
			count = 0;
			meetSeperator = 1;
		} else {
			if (buf[i] != '3' && buf[i] != '1' && buf[i] != '0')
				return 0;
			count++;
			if (count > 3)
				return 0;
			if (count == 1) {
				first = buf[i];
			} else if (count == 2) {
				if (buf[i] == first)	//�غ� 
					return 0;
				second = buf[i];
			} else if (count == 3) {
				if (buf[i] == first || buf[i] == second)	//�غ� 
					return 0;
			}
			meetSeperator = 0;
		}
	}
	if (buf[strlen(buf) - 1] == '-')
		return 0;
	return 1;
}

static int
computeSum(char *complexBet)
{				//���㸴ʽע������ 
	int i, countNum = 0, total = 1;
	int len = strlen(complexBet);

	for (i = 0; i < len; i++) {
		if (complexBet[i] == '-') {
			total *= countNum;
			countNum = 0;
		} else
			countNum++;
	}
	total *= countNum;	// �ٳ������һ����Ԫ 
	return total;
}

static int
makeSoccerPrize(char *bet, char *prizeSeq)
{
	int i, diff = 0;
	int n1 = strlen(bet);
	int n2 = strlen(prizeSeq);

	if (n1 != n2)
		return 10;	// ���н� 
	for (i = 0; i < n1; i++) {
		if (bet[i] != prizeSeq[i])
			diff++;
	}
	return diff;
}

static void
parseSoccerBet(char *prizeSeq, char *complexBet, struct LotteryRecord *LR)
{				// ����һ����ʽ��ע,���潱�������LR 
	int i, j, simple = 1, meet = 0, count = 0;
	int firstDivEnd, firstDivStart;
	int len = strlen(complexBet);

	firstDivEnd = len;
	for (i = 1; i < len; i += 2) {
		if (complexBet[i] != '-') {
			simple = 0;
			break;
		}
	}
	if (simple) {		//�򵥱�׼��ʽ 
		int diff;
		char buf[STRLEN];

		for (i = 0, j = 0; i < len; i++) {
			if (complexBet[i] != '-')
				buf[j++] = complexBet[i];
		}
		buf[j] = '\0';
		diff = makeSoccerPrize(prizeSeq, buf);
		if (diff <= 4 && diff >= 0)
			LR->betCount[diff]++;
	} else {
		for (i = 0; i < len; i++) {	//Ѱ�ҵ�һ����ʽ��Ԫ 
			if (complexBet[i] == '-') {
				if (count > 1 && !meet) {
					firstDivEnd = i;
					break;
				} else
					count = 0;
			} else
				count++;
		}
		firstDivStart = firstDivEnd - count;
		firstDivEnd--;

		for (i = 0; i < count; i++) {	//��ÿһ��Ҫ��ֵĵ�Ԫ��Ԫ�� 
			int slot = 0;
			char temp[STRLEN];

//�õ�ǰ��Ĳ��� 
			if (firstDivStart != 0) {
				for (j = 0; j < firstDivStart; j++, slot++)
					temp[slot] = complexBet[j];
			}
			temp[slot] = complexBet[firstDivStart + i];
			slot++;
//�õ�����Ĳ��� 
			for (j = firstDivEnd + 1; j < len; j++, slot++) {
				temp[slot] = complexBet[j];
			}
			temp[slot] = '\0';
//��ÿһ����֣����еݹ���� 
			parseSoccerBet(prizeSeq, temp, LR);
		}

	}
}

//part three: misc 
static int
createLottery(int prizeMode)
{
	char buf[STRLEN];
	char lotteryDesc[][16] = { "-", "36ѡ7��Ʊ", "���" };
	int day;
	time_t *startTime = 0, *endTime = 0, currTime = time(NULL);

	if (prizeMode == 1) {
		startTime = &(mcEnv->start367);
		endTime = &(mcEnv->end367);
	} else {
		startTime = &(mcEnv->soccerStart);
		endTime = &(mcEnv->soccerEnd);
	}
	update_health();
	if (check_health(1, 12, 4, "�������������ˣ�", YEA))
		return 1;
	move(12, 4);
	if (currTime < *endTime) {
		prints("%s�������ڻ��Ƚ��С�", lotteryDesc[prizeMode]);
		pressanykey();
		return 1;
	}
	prints("�½�%s", lotteryDesc[prizeMode]);
	while (1) {
		getdata(14, 4, "��Ʊ��������[3-7]: ", buf, 2, DOECHO, YEA);
		day = atoi(buf);
		if (day >= 3 && day <= 7)
			break;
	}
	*startTime = currTime;
	*endTime = currTime + day * 86400;
	update_health();
	myInfo->health--;
	myInfo->Actived++;
	sprintf(genbuf, "���ڲ�Ʊ���� %d ��󿪽�����ӭ���ӻԾ����", day);
	sprintf(buf, "����Ʊ����һ��%s��ʼ����", lotteryDesc[prizeMode]);
	deliverreport(buf, genbuf);
	showAt(15, 4, "�����ɹ����뵽ʱ������", YEA);
	return 0;
}

static void
savePrizeList(int prizeMode, struct LotteryRecord LR,
	      struct LotteryRecord *totalCount)
{				//���н������userid��������ʱ�ļ� 

	FILE *fp;
	struct LotteryRecord *LR_curr;
	int i, n = 0, miss = 1;
	void *prizeListMem;

	if (prizeMode == 1)
		sprintf(genbuf, "%s", DIR_MC_TEMP "36_7_prizeList");
	else
		sprintf(genbuf, "%s", DIR_MC_TEMP "soccer_prizeList");
	n = get_num_records(genbuf, sizeof (struct LotteryRecord));
	prizeListMem = malloc(sizeof (struct LotteryRecord) * (n + 1));
	if (prizeListMem == NULL)
		return;
	memset(prizeListMem, 0, sizeof (struct LotteryRecord) * (n + 1));
	if (file_exist(genbuf)) {
		if ((fp = fopen(genbuf, "r")) == NULL) {
			free(prizeListMem);
			return;
		}
		fread(prizeListMem, sizeof (struct LotteryRecord), n, fp);
		fclose(fp);
	}
	for (i = 0; i < n; i++) {
		LR_curr = prizeListMem + i * sizeof (struct LotteryRecord);
		if (!strcmp(LR_curr->userid, LR.userid)) {	// ���userid�Ѿ����� 
			for (i = 0; i < 5; i++)
				LR_curr->betCount[i] += LR.betCount[i];
			miss = 0;
			break;
		}
	}
	if (miss)		//userid��¼������, add 
		memcpy(prizeListMem + n * sizeof (struct LotteryRecord), &LR,
		       sizeof (struct LotteryRecord));
	if ((fp = fopen(genbuf, "w")) == NULL) {
		free(prizeListMem);
		return;
	}
	n = miss ? (n + 1) : n;
	fwrite(prizeListMem, sizeof (struct LotteryRecord), n, fp);
	fclose(fp);
	free(prizeListMem);
// ȫ��ͳ���ۼ� 
	for (i = 0; i < 5; i++)
		totalCount->betCount[i] += LR.betCount[i];
	return;
}

static int
sendPrizeMail(struct LotteryRecord *LR, struct LotteryRecord *totalCount)
{
	int i, totalMoney, perPrize, myPrizeMoney;
	char title[STRLEN], buf[128];
	char *prizeName[] = { "NULL", "36ѡ7", "���", NULL };
	char *prizeClass[] = { "�ص�", "һ��", "����", "����", "��ο", NULL };
	float prizeRate[] = { 0.60, 0.20, 0.10, 0.05, 0.02 };
	int prizeMode = strcmp(totalCount->userid, "36_7") == 0 ? 1 : 2;
	mcUserInfo *mcuInfo;

	sethomefile(buf, LR->userid, "mc.save");
	if (!file_exist(buf))
		initData(1, buf);
	if((mcuInfo = loadData(buf, sizeof (mcUserInfo))) == (void*)-1)
		return -1;
	if (prizeMode == 1)
		totalMoney = mcEnv->prize367 + PRIZE_PER;
	else
		totalMoney = mcEnv->prizeSoccer + PRIZE_PER;
	for (i = 0; i < 5; i++) {	// ��ÿ���н���userid,�����ֽ��� 
		if (LR->betCount[i] > 0 && totalCount->betCount[i] > 0) {
			perPrize =
			    prizeRate[i] * totalMoney / totalCount->betCount[i];
			myPrizeMoney = perPrize * LR->betCount[i];
			mcuInfo->cash += myPrizeMoney;
			mcEnv->Treasury -= myPrizeMoney;
			sprintf(genbuf,
				"��һ������ %d ע,�õ��� %d %s�Ľ��𡣹�ϲ��",
				LR->betCount[i], myPrizeMoney, MONEY_NAME);
			sprintf(title, "��ϲ�����%s%s����",
				prizeName[prizeMode], prizeClass[i]);
			if (myInfo->GetLetter == 1)
			    system_mail_buf(genbuf, strlen(genbuf), LR->userid,
				        	    title, currentuser->userid);
		}
	}
	unloadData(mcuInfo, sizeof (mcUserInfo));
	return 0;
}

static int
doOpenLottery(int prizeMode, char *prizeSeq)
{
	FILE *fp;
	char line[256], buf[STRLEN], title[STRLEN];
	char *bet, *userid;
	int totalMoney, remainMoney, i;
	struct LotteryRecord LR, totalCount;
	char *prizeName[] = { "NULL", "36ѡ7", "���", NULL };
	char *prizeClass[] = { "�ص�", "һ��", "����", "����", "��ο", NULL };
	float prizeRate[] = { 0.60, 0.20, 0.10, 0.05, 0.02 };

	if (prizeMode == 1) {
		make367Seq(prizeSeq);	//�������� 
		totalMoney = mcEnv->prize367 + PRIZE_PER;
		fp = fopen(DIR_MC "36_7_list", "r");
	} else {
		totalMoney = mcEnv->prizeSoccer + PRIZE_PER;
		fp = fopen(DIR_MC "soccer_list", "r");
	}
	totalMoney = MIN(totalMoney, MAX_POOL_MONEY);
	if (fp == NULL)
		return -1;
//   ---------------------���㽱��----------------------- 

	memset(&totalCount, 0, sizeof (struct LotteryRecord));	// �ܼƳ�ʼ�� 
	while (fgets(line, 255, fp)) {
		userid = strtok(line, " ");
		bet = strtok(NULL, "\n");
		if (!userid || !bet) {
			continue;
		}
		memset(&LR, 0, sizeof (struct LotteryRecord));
		strcpy(LR.userid, userid);
// ������ע,�н����������LR�� 
		if (prizeMode == 1)
			parse367Bet(prizeSeq, bet, &LR);
		else
			parseSoccerBet(prizeSeq, bet, &LR);
		for (i = 0; i < 5; i++) {	// ����Ƿ��н� 
			if (LR.betCount[i] > 0) {
				savePrizeList(prizeMode, LR, &totalCount);
				break;
			}
		}
	}
	fclose(fp);
//  ------------------------ ���� --------------------- 
	remainMoney = totalMoney;
	if (prizeMode == 1) {
		sprintf(genbuf, "%s", DIR_MC_TEMP "36_7_prizeList");
		strcpy(totalCount.userid, "36_7");
	} else {
		sprintf(genbuf, "%s", DIR_MC_TEMP "soccer_prizeList");
		strcpy(totalCount.userid, "soccer_");

// ����ǰ�����ɵ��н��ļ�,��ÿ���н�ID����. ÿ��ID��һ����¼ 
// ͬʱ�ܼ��ۼ� 
		new_apply_record(genbuf, sizeof (struct LotteryRecord),
				 (void *) sendPrizeMail, &totalCount);

//  ---------------------- ����֪ͨ --------------------- 
		for (i = 0; i < 5; i++) {
			if (totalCount.betCount[i] > 0) {
				sprintf(title, "����Ʊ������%s%s���������",
					prizeName[prizeMode], prizeClass[i]);
				sprintf(buf, "����ע��: %d\n��ע����: %d",
					totalCount.betCount[i],
					(int) (prizeRate[i] * totalMoney /
					       totalCount.betCount[i]));
				deliverreport(title, buf);
				remainMoney -= totalMoney * prizeRate[i];
			}
		}
	}
// ��ɨս�� 
	if (prizeMode == 1) {
		mcEnv->prize367 = remainMoney;
		unlink(DIR_MC "36_7_list");
		unlink(DIR_MC_TEMP "36_7_prizeList");
	} else {
		mcEnv->prizeSoccer = remainMoney;
		unlink(DIR_MC "soccer_list");
		unlink(DIR_MC_TEMP "soccer_prizeList");
	}
	return 0;
}

static int
tryOpenPrize(int prizeMode)
{
	char buf[STRLEN];
	int flag;
	time_t startTime, endTime;

	update_health();
	if (check_health(1, 12, 4, "�������������ˣ�", YEA))
		return 1;
	if (prizeMode == 1) {
		startTime = mcEnv->start367;
		endTime = mcEnv->end367;
	} else {
		startTime = mcEnv->soccerStart;
		endTime = mcEnv->soccerEnd;
	}
	if (startTime == 0) {
		showAt(t_lines - 5, 4, "û���ҵ��ò�Ʊ�ļ�¼...", YEA);
		return -1;
	}
	if (time(NULL) < endTime) {
		showAt(t_lines - 5, 4, "��û�е�������ʱ�䰡!", YEA);
		pressanykey();
		return -1;
	}
	if (prizeMode == 1)
		buf[0] = '\0';
	else {
		getdata(t_lines - 5, 4,
			"������ҽ�����(���� - )[��\033[1;33mENTER\033[m����]: ",
			buf, 55, DOECHO, YEA);
		if (buf[0] == '\0')
			return 0;
	}
	flag = doOpenLottery(prizeMode, buf);
	move(t_lines - 4, 4);
	if (flag == 0)
		prints("�����ɹ���");
	else
		prints("�����������...");
	update_health();
	myInfo->health--;
	myInfo->Actived++;
	pressanykey();
	return 0;
}

static int
buyLottery(int type)
{
	int needMoney, perMoney = 1000;
	int retv, maxBufLen, num, trytime = 0;
	int *poolMoney;
	char letter[128], buf[128], filepath[256];
	time_t startTime, endTime;
	char *desc[] = { "36ѡ7", "����" };

	money_show_stat("��Ʊ����");
	if (check_health(1, 12, 4, "�������������ˣ�", YEA))
		return 0;
	if (type == 0) {	//36ѡ7 
		sprintf(filepath, "%s", DIR_MC "36_7_list");
		maxBufLen = 21;
		poolMoney = &(mcEnv->prize367);
		startTime = mcEnv->start367;
		endTime = mcEnv->end367;
	} else {
		sprintf(filepath, "%s", DIR_MC "soccer_list");
		maxBufLen = 61;
		poolMoney = &(mcEnv->prizeSoccer);
		startTime = mcEnv->soccerStart;
		endTime = mcEnv->soccerEnd;
	}
	if (startTime == 0) {
		sprintf(buf, "��Ǹ����һ�ڵ�%s��Ʊ��δ��ʼ���ۡ�", desc[type]);
		showAt(4, 4, buf, YEA);
		return 0;
	}
	if (time(NULL) >= endTime) {
		showAt(4, 4, "��Ǹ�����ڲ�Ʊ�������Ѿ���������ȴ�������", YEA);
		return 0;
	}
	if (type == 0)
		showAt(4, 4, "���ּ���-���������� 08-13-01-25-34-17-18", NA);
	else
		showAt(4, 0,
		       "    ����ʤ/ƽ/���ֱ�ΪΪ3/1/0������������-������\n"
		       "    ֧�ָ�ʽ��ע�����磺 1-310-1-01-3-30", NA);
	move(7, 4);
	prints("��ǰ����أ�\033[1;31m%d\033[m   �̶�����\033[1;31m%d\033[m",
	       *poolMoney, PRIZE_PER);
	sprintf(genbuf, "ÿע %d %s��ȷ����ע��", perMoney, MONEY_NAME);
	move(9, 4);
	if (askyn(genbuf, NA, NA) == NA)
		return 0;
	while (1) {
		getdata(10, 4, "����д��ע��: ", buf, maxBufLen, DOECHO, YEA);
		if (type == 0)
			retv = valid367Bet(buf);
		else
			retv = validSoccerBet(buf);
		if (retv == 0) {
			showAt(11, 4, "�Բ���������ע����д�ò���ม�", YEA);
			if (trytime++ == 2)
				break;
			continue;
		}
		if (type == 0) {
			num = 1;
			needMoney = perMoney;
		} else {
			num = computeSum(buf);
			needMoney = num * perMoney;
		}
		if (myInfo->cash < needMoney) {
			showAt(11, 4, "�Բ�������Ǯ������", YEA);
			return 0;
		}
		myInfo->cash -= needMoney;
		*poolMoney += needMoney;
		update_health();
		myInfo->health--;
		myInfo->Actived++;
		sprintf(genbuf, "%s %s", currentuser->userid, buf);
		if (type == 0)
			addtofile(DIR_MC "36_7_list", genbuf);
		else
			addtofile(DIR_MC "soccer_list", genbuf);
		sprintf(letter, "��������һע%s��Ʊ��ע���ǣ�%s��", desc[type],
			buf);
		if (myInfo->GetLetter == 1)
		    system_mail_buf(letter, strlen(letter), currentuser->userid,
				            "��Ʊ���Ĺ���ƾ֤", currentuser->userid);
		clrtoeol();
		sprintf(buf, "�ɹ����� %d ע%s��Ʊ ��ף���д󽱣�", num,
			desc[type]);
		sleep(1);
		showAt(11, 4, buf, YEA);
		break;
	}
	return 1;
}

int
money_lottery()
{
	char ch, quit = 0, quitRoom = 0;
	char uident[IDLEN + 1];

       if (!(myInfo->GetLetter == 1)) {
           clear();
           showAt(5,4,"���Ѿ��ر��˽���������Ϸ���ܣ��뿪����������",YEA);
           return 0;
       }

	while (!quit) {
		nomoney_show_stat("��Ʊ����");
		showAt(4, 4, "Ŀǰ�������ֲ�Ʊ: 36ѡ7�������Ʊ����ӭ����",
		       NA);
		move(t_lines - 1, 0);
		prints
		    ("\033[1;44m ѡ�� \033[1;46m [1]36ѡ7 [2]��� [3]������ [Q]�뿪\033[m");
		ch = igetkey();
		switch (ch) {
		case '1':
		case '2':
			buyLottery(ch - '1');
			break;
		case '3':
			nomoney_show_stat("���ʹ�˾������");
			whoTakeCharge(2, uident);
			if (strcmp(currentuser->userid, uident))
				break;
			quitRoom = 0;
			while (!quitRoom) {
				nomoney_show_stat("���ʹ�˾������");
				move(t_lines - 1, 0);
				move(6, 10);
				prints("������Ʊ:    1.  36ѡ7    2.�����Ʊ");
				move(7, 10);
				prints
				    ("��    ��:    3.  36ѡ7    4.�����Ʊ    Q.  �˳�");
				move(10, 4);
				prints("��ѡ��Ҫ�����Ĵ���:");
				ch = igetkey();
				switch (ch) {
				case '1':
				case '2':
					createLottery(ch - '0');
					break;
				case '3':
				case '4':
					tryOpenPrize(ch - '2');
					break;
				case 'q':
				case 'Q':
					quitRoom = 1;
					break;
				}
			}
			break;
		case 'q':
		case 'Q':
			quit = 1;
			break;
		}
		limit_cpu();
	}
	return 0;
}

// --------------------------    �ĳ�    ----------------------- // 
static int
money_dice()
{
	int i, ch, quit = 0, target, win, num, sum;
	unsigned int t[3];

	while (!quit) {
		money_show_stat("�ĳ�������");
		move(4, 0);
		prints("    �ִ�С���ţ�4-10����С��11-17��Ϊ��\n"
		       "    ��ѺС��С������һ���ʽ�Ѻ��ľ�ȫ��ׯ�ҡ�\n"
		       "    ׯ��Ҫ��ҡ��ȫ�����������ӵ���һ������ͨ�Դ�С�ҡ�\n"
		       "    \033[1;31m�����׬���������⣬�����֣�Ը�ķ���!\033[m");
		move(t_lines - 1, 0);
		prints("\033[1;44m ѡ�� \033[1;46m [1]��ע [2]VIP [Q]�뿪\033[m");
		win = 0;
		ch = igetkey();
		switch (ch) {
		case '1':
			update_health();
			if(mcEnv->prize777 < 10000){                 //777 10000 �����ʽ�
		         showAt(11, 4,"�ĳ��ֽ������ͣ���ͣӪҵ��",YEA);
		         break;
			}
			if (check_health(1, 12, 4, "�������������ˣ�", YEA))
				break;
			num =
			    userInputValue(9, 4, "ѹ", MONEY_NAME, 1000,
					   200000);
			if (num == -1)
				break;
			getdata(11, 4, "ѹ��(L)����С(S)��[L]", genbuf, 3,
				DOECHO, YEA);
			if (genbuf[0] == 'S' || genbuf[0] == 's')
				target = 1;
			else
				target = 0;
			sprintf(genbuf,
				"�� \033[1;31m%d\033[m %s�� \033[1;31m%s\033[m��ȷ��ô��",
				num, MONEY_NAME, target ? "С" : "��");
			move(12, 4);
			if (askyn(genbuf, NA, NA) == NA)
				break;
			move(13, 4);
			if (myInfo->cash < num) {
				showAt(13, 4, "ȥȥȥ��û��ô��Ǯ��ʲô�ң�",
				       YEA);
				break;
			}
			myInfo->cash -= num;
			myInfo->health--;
			if(!random()%5)
				myInfo->luck--;
			myInfo->Actived += 2;
			for (i = 0; i < 3; i++) {
				getrandomint(&t[i]);
				t[i] = t[i] % 6 + 1;
			}
			sum = t[0] + t[1] + t[2];
			if ((t[0] == t[1]) && (t[1] == t[2])) {
				mcEnv->prize777 += after_tax(num);
				sprintf(genbuf, "\033[1;32mׯ��ͨɱ��\033[m");
			} else if (sum <= 10) {
				sprintf(genbuf, "%d �㣬\033[1;32mС\033[m",
					sum);
				if (target == 1)
					win = 1;
			} else {
				sprintf(genbuf, "%d �㣬\033[1;32m��\033[m",
					sum);
				if (target == 0)
					win = 1;
			}
			sleep(1);
			prints("���˿���~~  %d %d %d  %s", t[0], t[1], t[2],
			       genbuf);
			move(14, 4);
			if (win) {
				myInfo->cash += 2 * num;
				mcEnv->prize777 -= num;
				prints("��ϲ��������һ�Ѱɣ�");
			} else {
				mcEnv->prize777 += after_tax(num);
				prints("û�й�ϵ�������Ӯ...");
			}
			pressanykey();
			break;
		case '2':
			money_big();
			break;
		case 'Q':
		case 'q':
			quit = 1;
			break;
		}
		limit_cpu();
	}
	return 0;
}

static int
calc777(int t1, int t2, int t3)
{
	if ((t1 == 13) && (t2 == 13) && (t3 == 13))
		return 81;
	if ((t1 % 2 == 0) && (t2 % 2 == 0) && (t3 % 2 == 0))
		return 2;
	if ((t1 % 2 == 0) && (t2 % 2 == 0) && (t3 % 6 == 1) && (t3 !=  13))
		return 3;
	if ((t1 % 2 == 0) && (t2 % 6 == 1) && (t3 % 6 == 1) && (t2 !=  13) && (t3 !=  13))
		return 4;
	if ((t1 % 6 == 1) && (t2 % 6 == 1) && (t3 % 2 == 0) && (t1 !=  13) && (t2 !=  13))
		return 4;
	if ((t1 % 2 == 0) && (t2 % 6 == 3) && (t3 % 6 == 3))
		return 6;
	if ((t1 % 6 == 3) && (t2 % 6 == 3) && (t3 % 2 == 0))
		return 6;
	if ((t1 % 6 == 1) && (t2 % 6 == 1) && (t3 % 6 == 1) && (t1 !=  13) && (t2 !=  13) && (t3 !=  13))
		return 11;
	if ((t1 % 6 == 3) && (t2 % 6 == 3) && (t3 % 6 == 3))
		return 21;
	if ((t1 % 6 == 5) && (t2 % 6 == 5) && (t3 % 6 == 5))
		return 41;
	if ((t1 % 6 == 5) && (t2 == 13) && (t3 == 13))
		return 61;
	return 0;
}

#if 0
static int
get_addedlevel(int level, int base, int num)
{
	if (num - (level + 1) * (level + 1) * base >
	    (level + 2) * (level + 2) * base) {
		level++;
		level = get_addedlevel(level, base, num - level * level * base);
	}
	return level;
}
#endif

static int
money_777()
{
	int i, ch, quit = 0, bid, winrate, num = 0, tax = 0;
	unsigned int t[3];
	char n[15] = "-R-B-6-R-B-6-7";
	char title[STRLEN], buf[256];
	time_t timeA,timeB;

	while (!quit) {
		money_show_stat("�ĳ�777");
		if (mcEnv->prize777 < 0) {
			mcEnv->Treasury += mcEnv->prize777;
			mcEnv->prize777 = 0;
		}
		if (check_health(1, 12, 4, "�������������ˣ�", YEA))
			break;
		/*if (!(random() % (MAX_ONLINE/50)) && myInfo->Actived > MAX_ONLINE/5 ) {
			clear();
			move(4, 4);
			prints("ͻȻ֮�䣬����������ҵ���ת����������");
			pressanykey();
			randomevent();
		}*/
		if (!(random() % (MAX_ONLINE/10)) || myInfo->luck <= -90) {
			move(5, 8);
			prints("=============== �� �� Ѳ �� ============");
			move(8, 4);
			prints
			    ("  ���777�ĳ��ٰ�Ƶ�������Ծ������о��ٽ���Ѳ�ӣ��̲����Ŀ�ꡣ"
			     "\n  ��ѽ�����ã������������˹�����");
			pressanykey();
			move(10, 4);
			if (random() % 2) {
				prints
					("���û��ã�����Ŀ��б�ӵĴ�������߹�ȥ�ˡ�");
			}
/*			
      else {
				prints
				    ("�������´�������һ�ۣ�����˹���˵������Ҫ���ȷ�ϣ���");
				getdata(11, 4, "����������(ֻ��һ�λ���): ",
					passbuf, PASSLEN, NOECHO, YEA);
				if (passbuf[0] == '\0' || passbuf[0] == '\n'
				    || !checkpasswd(currentuser->passwd,
						    currentuser->salt,
						    passbuf)) {
					prints
					    ("\n    ����ŭ�𣺡������֤ʧ�ܣ�������һ�ˣ���"
					     "\n    555���㱻û�������ֽ𣬲��ұ����30���ӡ�");
					myInfo->freeTime = time(NULL) + 1800;
					mcEnv->Treasury += myInfo->cash;
					myInfo->cash = 0;
					myInfo->mutex = 0;
					unloadData(myInfo,sizeof (mcUserInfo));
					unloadData(mcEnv, sizeof (MC_Env));
					pressreturn();
					Q_Goodbye();
				} 
*/
			  else  {
				    timeA = time(NULL);
					if (show_cake()) {
						prints
						    ("\n    ����ŭ�𣺡������֤ʧ�ܣ�������һ�ˣ���"
						     "\n    555���㱻û�������ֽ𣬲��ұ����30���ӡ�");
						myInfo->freeTime = time(NULL) + 1800;
						mcEnv->Treasury += myInfo->cash;
						myInfo->cash = 0;
						myInfo->mutex = 0;
						unloadData(myInfo,sizeof (mcUserInfo));
						unloadData(mcEnv, sizeof (MC_Env));
						pressreturn();
						Q_Goodbye();
					}
					timeB = time(NULL);
					if (timeA + 120 < timeB) {
						prints
						    ("\n    ����ŭ�𣺡���ô�򵥵����ⶼ����ô�ã������˰ɣ���"
						     "\n    555���㱻û�������ֽ𣬲��ұ����30���ӡ�");
						myInfo->freeTime = time(NULL) + 1800;
						mcEnv->Treasury += myInfo->cash;
						myInfo->cash = 0;
						myInfo->mutex = 0;
						unloadData(myInfo,sizeof (mcUserInfo));
						unloadData(mcEnv, sizeof (MC_Env));
						pressreturn();
						Q_Goodbye();
					}
					move(20, 0);
			        prints("\n    ���ٵ��ͷ�����ţ�û��������ɡ���");
			        pressanykey();
				}			
			break;
		}

		move(6, 4);
		prints("--R 1:2    -RR 1:3    RR- 1:3    -BB 1:5    BB- 1:5");
		move(7, 4);
		prints("RRR 1:10   BBB 1:20   666 1:40   677 1:60   --- 1:1");
		move(8, 4);
		prints("777 1:80 (�л���Ӯ�õ�ǰ�ۼƻ����һ�룬��಻����100��)");
		move(9, 4);
		prints
		    ("Ŀǰ�ۻ�������: %d  ��Ӯ��ô��ѹ200���л���ม�",
		     mcEnv->prize777);
		move(t_lines - 1, 0);
		prints
		    ("\033[1;44m ѡ�� \033[1;46m [1] ѹ50 [2] ѹ200 [3] ע�뽱�� [Q]�뿪\033[m");
		ch = igetkey();
		if (ch == 'q' || ch == 'Q')
			break;
		if (ch == '1') {
			if (mcEnv->prize777 < 10000) {
				showAt(11, 4,
				       "Ŀǰ777�����������ȴ�ע�뽱��",
				       YEA);
				return 0;
			}
			bid = 50;
		} else if (ch == '2') {
			if (mcEnv->prize777 < 10000) {
				showAt(11, 4,
				       "Ŀǰ777�����������ȴ�ע�뽱��",
				       YEA);
				return 0;
			}
			bid = 200;
		} else if (ch == '3') {
			if (myInfo->cash < 10000) {
				showAt(11, 4, "������ֽ�̫���ˣ�cmft", YEA);
				continue;
			}
			num =
			    userInputValue(11, 4, "ע��", MONEY_NAME, 10000,
					   myInfo->cash);
			if (num <= 0)
				continue;
			myInfo->cash -= num;
			mcEnv->prize777 += after_tax(num);
			myInfo->luck = MIN(100, myInfo->luck + num / 10000);
			showAt(15, 4, "ע�뽱��ɹ���", YEA);
			continue;
		} else
			continue;
		if (myInfo->cash < bid) {
			showAt(11, 4, "ûǮ�ͱ�����...", YEA);
			continue;
		}
		myInfo->cash -= bid;
		update_health();
		myInfo->health -= 2;
		myInfo->Actived += 2;
		for (i = 0; i < 3; i++) {
			getrandomint(&t[i]);
			t[i] = t[i] % 14;
			move(11, 20 + 2 * i);
			prints("%c", n[t[i]]);
			refresh();
			sleep(1);
		}
		winrate = calc777(t[0], t[1], t[2]);
		if (winrate <= 0) {
			mcEnv->prize777 += after_tax(bid);
			showAt(12, 4,
			       "���ˣ���ע�����ۻ������츣���˵����츣�Լ���",
			       YEA);
			continue;
		}
		mcEnv->prize777 -= MAX(bid * (winrate - 1),0);
		myInfo->cash += MAX(bid * winrate, 0);
		move(12, 4);
		prints("��Ӯ�� %d %s", bid * (winrate - 1), MONEY_NAME);
		if (winrate == 81 && bid == 200) {
			num = MIN(mcEnv->prize777 / 2, 1000000) * (myInfo->luck + 100)/200;	//777�������Ʒ�ҹ�
//			num = MAX(num, random() % (myInfo->luck + 101) * 5000 );
			tax = num - after_tax(num);
			myInfo->cash += num - tax;
			mcEnv->prize777 -= num;
			move(12, 4);
			prints("\033[5;33m��ϲ����ô󽱣�������Ҫ��˰����\033[0m");
			sprintf(title, "���ĳ���%s Ӯ��777�󽱣�",
				currentuser->userid);
			sprintf(buf, "�ĳ�������Ϣ��%s Ӯ��777�� %d %s!\n��˰��ʵ�� %d %s",
				currentuser->userid, num, MONEY_NAME, 
				num - tax, MONEY_NAME);
			deliverreport(title, buf);
		}
		pressanykey();
	}
	return 0;
}

static int
money_big()
{
	int i, ch, quit = 0, target, win, num, sum;
	unsigned int t[3];
	char buf[256];

	if(mcEnv->Treasury < 15000000){                 //����1500w �����ʽ�
		showAt(13, 4,
		       "�����ֽ������ͣ�������ͣӪҵ����",
		       YEA);
		return 0;
	}
	if (myInfo->cash < 200000) {
		showAt(13, 4,
		       "����б�۳���һ�ۣ����ְ�����ס����һ��������Ҳ��������ң������ͷ�󣡣���",
		       YEA);
		return 0;
	}
	move(7, 4);
	sprintf(buf, "�볡��\033[1;32m10��\033[m%s����ȷ��Ҫ��ȥ��",
		MONEY_NAME);
	if (askyn(buf, NA, NA) == NA) {
		prints("\n�ݰݣ�С����");
		return 0;
	}
	myInfo->cash -= 100000;
	mcEnv->Treasury += 100000;
	clear();
	money_show_stat("�ĳ�������");
	move(4, 0);
	prints
	    ("    �����Ǵ����ң�����Ķ�ע������ĸ�Ҫ�����Ը��ӵĴ̼�����\n"
	     "    ��ط�һ����Ǯ��Ȩ�Ĳ��ܽ��������Ͻ�Ѻ�ɣ�\n"
	     "    Ŷ�����ˣ�Ӯ�˵Ļ�Ҫ��ȡ10����С�ѵġ�");
	pressanykey();

	while (!quit) {
		clear();
		money_show_stat("�ĳ�������");
		move(4, 0);
		prints("    ��������飺�ִ�С���ţ�4-10����С��11-17��Ϊ��\n"
		       "    ��ѺС��С������һ���ʽ�Ѻ��ľ�ȫ��ׯ�ҡ�\n"
		       "    ׯ��Ҫ��ҡ��ȫ�����������ӵ���һ������ͨ�Դ�С�ҡ�\n"
		       "    \033[1;31m�����׬���������⣬�����֣�Ը�ķ���!\033[m");
		move(t_lines - 1, 0);
		prints("\033[1;44m ѡ�� \033[1;46m [1]��ע [Q]�뿪\033[m");
		win = 0;
		ch = igetkey();
		switch (ch) {
		case '1':
			update_health();
			if (check_health(3, 12, 4, "�������������ˣ�", YEA))
				break;
			sprintf(buf, "\033[1;32m��\033[m%s", MONEY_NAME);
			num = userInputValue(9, 4, "ѹ", buf, 20, 1000);
			if (num == -1)
				break;
			getdata(11, 4, "ѹ��(L)����С(S)��[L]", genbuf, 3,
				DOECHO, YEA);
			if (genbuf[0] == 'S' || genbuf[0] == 's')
				target = 1;
			else
				target = 0;
			if(mcEnv->Treasury < 15000000){                 //����1500w �����ʽ�
	        	showAt(13, 4,"�����ֽ������ͣ�������ͣӪҵ����",YEA);
		        return 0;
			}
			if(mcEnv->Treasury - 15000000 < num*10000){
				num = mcEnv->Treasury/10000 - 1500;
				sprintf(genbuf,
					"�����ֽ��㣬��ֻ���� \033[1;31m%d\033[m \033[1;32m��\033[m%s�� \033[1;31m%s\033[m��ȷ��ô��",
					num, MONEY_NAME, target ? "С" : "��");
			}
			else{
				sprintf(genbuf,
					"�� \033[1;31m%d\033[m \033[1;32m��\033[m%s�� \033[1;31m%s\033[m��ȷ��ô��",
					num, MONEY_NAME, target ? "С" : "��");
				}
			move(12, 4);
			if (askyn(genbuf, NA, NA) == NA)
				break;
			move(13, 4);
			if (myInfo->cash < (num * 10000)) {
				showAt(13, 4, "ȥȥȥ��û��ô��Ǯ��ʲô�ң�",
				       YEA);
				break;
			}
			myInfo->cash -= (num * 10000);
			myInfo->health -= 3;
			myInfo->luck--;
			myInfo->Actived += 3;
			for (i = 0; i < 3; i++) {
				getrandomint(&t[i]);
				t[i] = t[i] % 6 + 1;
			}
			sum = t[0] + t[1] + t[2];
			if ((t[0] == t[1]) && (t[1] == t[2])) {
				mcEnv->Treasury += 10000 * num;
				sprintf(genbuf, "\033[1;32mׯ��ͨɱ��\033[m");
			} else if (sum <= 10) {
				sprintf(genbuf, "%d �㣬\033[1;32mС\033[m",
					sum);
				if (target == 1)
					win = 1;
			} else {
				sprintf(genbuf, "%d �㣬\033[1;32m��\033[m",
					sum);
				if (target == 0)
					win = 1;
			}
			sleep(1);
			prints("���˿���~~  %d %d %d  %s", t[0], t[1], t[2],
			       genbuf);
			move(14, 4);
			if (win) {
				myInfo->cash += 19000 * num;
				mcEnv->Treasury -= 9000 * num;
				prints("��ϲ������ȡС��%d%s~~ ����һ�Ѱɣ�",
				       1000 * num, MONEY_NAME);
				sprintf(buf, "���նĳ���Ѷ��%s�ոմ�%s�ĳ�Ӯ����%d��%s��",
					    currentuser->userid,CENTER_NAME,num,MONEY_NAME);
				deliverreport("�����š����˶��ĳ�һҹ����", buf);
			} else{
				mcEnv->Treasury += 10000 * num;
				prints("û�й�ϵ�������Ӯ�����ٲ��ø�С����...");
				sprintf(buf, "���նĳ���Ѷ��%s�ոմ�%s�ĳ������%d��%s��\nר�����ѣ��Ĳ��к�����!",
					    currentuser->userid,CENTER_NAME,num,MONEY_NAME);
				deliverreport("�����š���ù����ͷ����", buf);
			}
			pressanykey();
			break;
		case 'Q':
		case 'q':
			quit = 1;
			break;
		}
		limit_cpu();
	}
	return 0;

}

/*
static int
game_prize()
{
	int quit = 0;
	char ch;
	while (!quit) {
		money_show_stat("�齱��");
		move (4, 0);
		prints ("���������Ϸ����������а񣬿���������齱��һ���ϰ��¼ֻ�ܳ�һ�ν���");
		move (t_lines - 1, 0);
		prints
		    ("\033[1;37;44m ѡ�� \033[1;46m [1]������ [2]ɨ�� [3]��Ӧʽɨ�� [4]����˹���� [5]���� [Q]�뿪\033[m");
		ch = igetkey();
		switch (ch) {
		case '1':
			choujiang(MY_BBS_HOME "/etc/worker/worker.rec", "������");
			break;
		case '2':
			choujiang(MY_BBS_HOME "/etc/winmine/mine2.rec", "ɨ��");
			break;
		case '3':
			choujiang(MY_BBS_HOME "/etc/winmine2/mine3.rec",
				  "��Ӧʽɨ��");
			break;
		case '4':
			choujiang(MY_BBS_HOME "/etc/tetris/tetris.rec",
				  "����˹����");
			break;
		case '5':
			choujiang(MY_BBS_HOME "/etc/tt/tt.dat", "������ϰ");
			break;
		case 'q':
		case 'Q':
			quit = 1;
			break;
		}
	}
	return 0;
}

static void
choujiang(char *recfile, char *gametype)
{
	int n, topS[20], topT[20], chou, jiang, tax;
	char topID[20][20], topFROM[20][20], prize[20][3], buf[256],
	    title[STRLEN];
	FILE *fp;

	if ( (fp = fopen(recfile, "r")) == NULL)
		return;
	chou = 0;
	if (!strcmp(gametype, "������")){
		for (n = 0; n <= 19; n++){
			fscanf(fp, "%s %d %d %s %s\n",
				topID[n], &topS[n], &topT[n], topFROM[n], prize[n]);
			if (!strcmp(currentuser->userid, topID[n])
				&& !strcmp(prize[n], "δ")){
				chou = n + 1;
			}
		}
	}else{
		for (n = 0; n <= 19; n++) {
			fscanf(fp, "%s %d %s %s\n", topID[n], &topT[n], topFROM[n],
		       		prize[n]);
			if (!strcmp(currentuser->userid, topID[n])
			    && !strcmp(prize[n], "δ")) {
				chou = n + 1;
			}
		}
	}
	fclose(fp);
	
	if (chou == 0) {
		showAt(6, 4, "\033[1;37m��Ŀǰ�޷��齱\033[m", YEA);
		return;
	}
	move(6, 4);
	prints("\033[1;37m���� %s �� %d ��\033[m", gametype, chou);
	move(7, 4);
	if (askyn("Ҫ�齱��", YEA, NA) == YEA) {
		jiang = 0;
		if (chou == 1)
			jiang = random() % 500 + 500;
		if (chou > 1 && chou <= 5)
			jiang = random() % 500 + 100;
		if (chou > 5 && chou <= 10)
			jiang = random() % 190 + 10;
		if (chou > 10 && chou <= 15)
			jiang = random() % 95 + 5;
		if (chou > 15 && chou <= 20)
			jiang = random() % 49 + 1;
		jiang = jiang * 1000;
		
		if (mcEnv->Treasury > jiang + 10000){
			tax = jiang - after_tax(jiang);
			mcEnv->Treasury -= jiang - tax;
			myInfo->cash += jiang - tax;
			strcpy(prize[chou-1], "��");
			myInfo->Actived += 2;
			sprintf(buf, "\033[1;32m�������� %d %s����˰��ʵ�� %d %s\033[m", 
				jiang, MONEY_NAME, jiang - tax, MONEY_NAME);
			showAt(8, 4, buf, YEA);
			sprintf(title, "���齱��%s ������ %d %s", 
				currentuser->userid, jiang, MONEY_NAME);
			sprintf(buf, "%s �� %s ���˵� %d ����\n�齱�鵽 %d %s��\n��˰��ʵ�� %d %s",
				currentuser->userid, gametype, chou, jiang, MONEY_NAME, 
				jiang - tax, MONEY_NAME);
			deliverreport(title, buf);
		}else{
			showAt(8, 4, "�����ûǮ�ˣ��Ȼ��ٳ�ɡ�", YEA);
		}
		fp = fopen(recfile, "w");
		if (!strcmp(gametype, "������")){
			for (n = 0; n <= 19; n++)
				fprintf(fp, "%s %d %d %s %s\n", 
					topID[n], topS[n], topT[n], topFROM[n], prize[n]);
		}else{
			for (n = 0; n <= 19; n++){
				fprintf(fp, "%s %d %s %s\n", 
					topID[n], topT[n], topFROM[n], prize[n]);
			}
		}
		fclose(fp);
	} else {
		move(8, 4);
		prints("\033[1;32m������齱��\033[m");
	}
	return;
}


//������ÿ����Ϸ����_UNIT����ʼ�����3��5�е�ͼ����
//����5���ߣ�(0,0)-(0,1)-(0,2)-(0,3)-(0,4)��(0,0)-(1,1)-(2,2)-(1,3)-(0,4)
//           (1,0)-(1,1)-(1,2)-(1,3)-(1,4)
//           (2,0)-(1,1)-(0,2)-(1,3)-(2,4)��(2,0)-(2,1)-(2,2)-(2,3)-(2,4)
//���ǰ3��4��5����һ����ͼ�����ͻά����������DIR_MC "monkey_rate"
//��ͬͼ�����ʲ�һ��������Դ����κ�ͼ�����˺ͺ��Ӳ�������Щ��ϡ�
//���ǰ3��4��5�г����ˣ����10�������Ϸ��
//�����Ϸ����������г��ֺ��ӣ�̨�潱���(����Ŀ-1)
static void
monkey_business(void)
{
	int i,j,MONKEY_UNIT, TempMoney=0, FreeGame=0, multi=0, quit=0, total=0, man=0;
	char ch,
	     pic[12][2] ={"��","��","��","��","10","��",
		          "��","��","ʨ","��","��","��"};
	int pattern[3][5], sum[5]={12345,12345,12345,12345,12345}, winrate[100000];
	FILE *fp;

	MONKEY_UNIT = userInputValue(6, 4, "ÿ����Ϸ����",MONEY_NAME, 250, 25000);
	if(MONKEY_UNIT < 0) {
		showAt(8, 4, "������ң�", YEA);
		return;
	}
	//mcLog("����MONKEY��ÿ�λ���", MONKEY_UNIT, "");
	if (myInfo->cash < MONKEY_UNIT) {
		showAt(8, 4, "ûǮ�ͱ���ˡ�", YEA);
		return;
	}

	fp = fopen(DIR_MC "monkey_rate", "r");
	for(i=0;i<100000;i++)
		fscanf(fp,"%d\n",&winrate[i]);
	fclose(fp);									

	while(!quit)
	{
		if (!(random() % MAX_ONLINE/5) && FreeGame <= 0)
			policeCheck();
		
		clear();
		money_show_stat("MONKEY BUSINESS");
		move(5, 0);
		prints("      --== ��Ϸ���� ==--      |\n");
		prints("                              |\n");
		prints("������5�����У�               |\n");
		prints("(1,1)\033[1;31m-\033[0;37m(1,2)\033[1;31m-\033[0;37m(1,3)"
			"\033[1;31m-\033[0;37m(1,4)\033[1;31m-\033[0;37m(1,5) |\033[m\n");
		prints("     \033[1;32m�v\033[0;37m   \033[1;35m�u\033[0;37m     "
			"\033[1;35m�v\033[0;37m   \033[1;32m�u\033[0;37m      |\033[m\n");
		prints("(2,1)\033[1;33m-\033[0;37m(2,2)\033[1;33m-\033[0;37m(2,3)"
			"\033[1;33m-\033[0;37m(2,4)\033[1;33m-\033[0;37m(2,5) |\033[m\n");
		prints("     \033[1;35m�u\033[0;37m   \033[1;32m�v\033[0;37m     "
			"\033[1;32m�u\033[0;37m   \033[1;35m�v\033[0;37m      |\033[m\n");
		prints("(3,1)\033[1;36m-\033[0;37m(3,2)\033[1;36m-\033[0;37m(3,3)"
			"\033[1;36m-\033[0;37m(3,4)\033[1;36m-\033[0;37m(3,5) |\033[m\n");
		prints("���ǰ3��ǰ4��ǰ5��ͼ����ͬ�� |\n");
		prints("����(��):  \033[1;44;37m 3\033[40m \033[46;37m 4\033[40m "
			"\033[44;37m 5\033[0;40;37m           |\033[m\n");
		prints("\033[1m���ˣѣ�10\033[32m \033[44;32m 1\033[40m \033[46;32m 2"
			"\033[40m \033[44;32m 4\033[0;40;37m           |\033[m\n");
		prints("\033[1m     �� ��\033[33m \033[44;33m 4\033[40m \033[46;33m10"
			"\033[40m \033[44;33m20\033[0;40;37m           |\033[m\n");
		prints("\033[1m        ��\033[35m \033[44;35m 6\033[40m \033[46;35m20"
			"\033[40m \033[44;35m30\033[0;40;37m "
			"\033[5;1;31m��\033[0;37m���Դ���|\033[m\n");
		prints("\033[1m     ʨ ��\033[31m \033[44;31m10\033[40m \033[46;31m30"
			"\033[40m \033[44;31m50\033[0;40;37m �κ�ͼ����|\033[m\n");
		prints("���ǰ3��ǰ4��ǰ5�г���\033[1;33m��\033[0;37m���� |\033[m\n");
		prints("�ɻ��10�������Ϸ���ᣬ��ʱ��|\n");
		prints("����3�г���\033[1;33m����\033[0;37m����̨������ |\033[m\n");
		prints("���2����3����4��             |\n");
		
		move(5, 34);
		prints("���룺%d\t��һ����Ҫ��%d", myInfo->cash, MONKEY_UNIT);
		if (FreeGame > 0) {
			move(6, 34);
			prints("��Ѵ�����%d\t̨����룺%d", FreeGame, TempMoney);
		}
		
		showAt(7, 34, "���ո����ʼ��Ϸ����\033[1;32mQ\033[m�˳���", NA);

		ch=igetkey();
		while ( ch != ' ' && ch != 'q' && ch != 'Q')
			ch=igetkey();

		if (mcEnv->prize777 < 50000){                 //777 50000 �����ʽ�
		    showAt(8, 34,"�ĳ��ֽ������ͣ���ͣӪҵ��",YEA);
		    ch = 'q';
		}

		if(myInfo->cash < MONKEY_UNIT) {
			showAt(8, 34, "��ûǮ�ˣ�ֻ���˳���Ϸ��", YEA);
			ch = 'q';
		}
		if(myInfo->health == 0)	{
			showAt(9, 34, "��û�����ˡ�ֻ���˳���Ϸ��", YEA);
			ch = 'q';
		}
		switch(ch) {
		case 'q':
		case 'Q':
			//mcLog("�˳�MONKEY��Ӯ��", TempMoney,"");
			myInfo->cash += TempMoney;
			TempMoney = 0;
			quit = 1;
			break;
		default:
			myInfo->health--;
			myInfo->Actived += 2;
			for (i=0;i<3;i++)
				for (j=0;j<5;j++) {
					if (j==2)
						pattern[i][j]=random()%12;
					else
						pattern[i][j]=random()%11;
				}

			for(j=0;j<5;j++) {
				for(i=0;i<3;i++) {
					move(10 + i, 38 + j*3);
					prints("%2s", pic[pattern[i][j]]);
				}
				prints("\n");
				refresh();
				sleep(1);
			}			
			
			sum[0] = 10000 * (pattern[0][0] % 10) +
				  1000 * (pattern[0][1] % 10) +
				   100 * (pattern[0][2] % 10) +
				    10 * (pattern[0][3] % 10) +
				         pattern[0][4] % 10;
			sum[1] = 10000 * (pattern[0][0] % 10) +
				  1000 * (pattern[1][1] % 10) +
				   100 * (pattern[2][2] % 10) +
				    10 * (pattern[1][3] % 10) +
				         pattern[0][4] % 10;
			sum[2] = 10000 * (pattern[1][0] % 10) +
				  1000 * (pattern[1][1] % 10) +
				   100 * (pattern[1][2] % 10) +
				    10 * (pattern[1][3] % 10) +
				         pattern[1][4] % 10;
			sum[3] = 10000 * (pattern[2][0] % 10) +
				  1000 * (pattern[1][1] % 10) +
				   100 * (pattern[0][2] % 10) +
				    10 * (pattern[1][3] % 10) +
				         pattern[2][4] % 10;
			sum[4] = 10000 * (pattern[2][0] % 10) +
				  1000 * (pattern[2][1] % 10) +
				   100 * (pattern[2][2] % 10) +
				    10 * (pattern[2][3] % 10) +
				         pattern[2][4] % 10;
			
			if(pattern[0][0]==10) {
				if(sum[0] != 9999 && 
				   sum[0]/10 != 999 && 
				   sum[0]%1000 != 999)
					sum[0] = 12345;
				if(sum[1] != 9999 &&
				   sum[1]/10 != 999 &&
				   sum[1]%1000 != 999)
					sum[1] = 12345;
			}
                        if(pattern[0][1]==10)
				if(sum[0] != 999)
					sum[0] = 12345;
			if(pattern[0][2]==10 || pattern[0][2]==11) {
				sum[0] = 12345;
				sum[3] = 12345;
			}
			if(pattern[1][0]==10)
				if(sum[2] != 9999 &&
				   sum[2]/10 != 999 &&
				   sum[2]%1000 != 999)
					sum[2] = 12345;
			if(pattern[1][1]==10) {
				if(sum[1] != 999)
					sum[1] = 12345;
				if(sum[2] != 999)
					sum[2] = 12345;
				if(sum[3] != 999)
					sum[3] = 12345;
			}
			if(pattern[1][2]==10 || pattern[1][2]==11)
				sum[2] = 12345;
			if(pattern[2][0]==10) {
				if(sum[3] != 9999 &&
				   sum[3]/10 != 999 &&
				   sum[3]%1000 != 999)
					sum[3] = 12345;
				if(sum[4] != 9999 &&
				   sum[4]/10 != 999 &&
				   sum[4]%1000 != 999)
					sum[4] = 12345;
			}
			if(pattern[2][1]==10)
				if(sum[4] != 999)
					sum[4] = 12345;
			if(pattern[2][2]==10 || pattern[2][2]==11) {
				sum[1] = 12345;
				sum[4] = 12345;
			}
			
			if(pattern[0][3]==10) {
				if(pattern[0][0] != 0)
					sum[0] = sum[0] / 100 * 100;
				else if(pattern[0][1] != 0)
					sum[0] = sum[0] / 100 * 100;
				else if(pattern[0][2] != 0)
					sum[0] = sum[0] / 100 * 100;
				else
					sum[0] = 11;
			}
			if(pattern[0][4]==10) {
				if(pattern[0][0] != 0)
					sum[0] = sum[0] / 10 * 10;
				else if(pattern[0][1] != 0)
					sum[0] = sum[0] / 10 * 10;
				else if(pattern[0][2] != 0)
					sum[0] = sum[0] / 10 * 10;
				else if(pattern[0][3] != 0)
					sum[0] = sum[0] / 10 * 10;
				else
					sum[0] = 1;
				if(pattern[0][0] != 0)
					sum[1] = sum[1] / 10 * 10;
				else if(pattern[1][1] != 0)                                                                 
					sum[1] = sum[1] / 10 * 10;
				else if(pattern[2][2] != 0)                                                                 
					sum[1] = sum[1] / 10 * 10;
				else if(pattern[1][3] != 0)
					sum[1] = sum[1] / 10 * 10;
				else
					sum[1] = 1;
			}
			if(pattern[1][3]==10) {
				if(pattern[0][0] != 0)
					sum[1] = sum[1] / 100 * 100;
				else if(pattern[1][1] != 0)
					sum[1] = sum[1] / 100 * 100;
				else if(pattern[2][2] != 0)
					sum[1] = sum[1] / 100 * 100;
				else
					sum[1] = 11;
				if(pattern[1][0] != 0)
					sum[2] = sum[2] / 100 * 100;
				else if(pattern[1][1] != 0)
					sum[2] = sum[2] / 100 * 100;
				else if(pattern[1][2] != 0)
					sum[2] = sum[2] / 100 * 100;
				else
					sum[2] = 11;
				if(pattern[2][0] != 0)
					sum[3] = sum[3] / 100 * 100;
				else if(pattern[1][1] != 0)
					sum[3] = sum[3] / 100 * 100;
				else if(pattern[0][2] != 0)
					sum[3] = sum[3] / 100 * 100;
				else
					sum[3] = 11;
			}
			if(pattern[1][4]==10) {
				if(pattern[1][0] != 0)
					sum[2] = sum[2] / 10 * 10;
				else if(pattern[1][1] != 0)
					sum[2] = sum[2] / 10 * 10;
				else if(pattern[1][2] != 0)
					sum[2] = sum[2] / 10 * 10;
				else if(pattern[1][3] != 0)
					sum[2] = sum[2] / 10 * 10;
				else
					sum[2] = 1;
			}
			if(pattern[2][3]==10) {
				if(pattern[2][0] != 0)
					sum[4] = sum[4] / 100 * 100;
				else if(pattern[2][1] != 0)
					sum[4] = sum[4] / 100 * 100;
				else if(pattern[2][2] != 0)
					sum[4] = sum[4] / 100 * 100;
				else
					sum[4] = 11;
			}
			if(pattern[2][4]==10) {
				if(pattern[2][0] != 0)
					sum[3] = sum[3] / 10 * 10;
				else if(pattern[1][1] != 0)
					sum[3] = sum[3] / 10 * 10;
				else if(pattern[0][2] != 0)
					sum[3] = sum[3] / 10 * 10;
				else if(pattern[1][3] != 0)
					sum[3] = sum[3] / 10 * 10;
				else
					sum[3] = 1;
				if(pattern[2][0] != 0)
					sum[4] = sum[4] / 10 * 10;
				else if(pattern[2][1] != 0)
					sum[4] = sum[4] / 10 * 10;
				else if(pattern[2][2] != 0)
					sum[4] = sum[4] / 10 * 10;
				else if(pattern[2][3] != 0)
					sum[4] = sum[4] / 10 * 10;
				else
					sum[4] = 1;
			}

			total = 0;
			for(i=0;i<5;i++)
				  total += winrate[sum[i]];
			total *= MONKEY_UNIT;

			move(15, 34);
			if(total > 0)
				prints("��Ӯ�� %d %s", total,  MONEY_NAME);
			
			if(FreeGame > 0) {
				TempMoney += total;
				mcEnv->prize777 -= total;
				//mcLog("��MONKEY.FreeGameӮ��", total, "");
				multi = 0;
				for(i=0;i<3;i++)
					if(pattern[i][2]==11)
						multi = 1;
				if(multi) {
					//mcLog("MONKEY.FreeGamę����", TempMoney, "");
					mcEnv->prize777 -= TempMoney * (man - 2);
					TempMoney *= man - 1;
					move(16, 34);
					prints("�����˺��ӣ���̨��Ľ����%d��",
							man - 1);
					//mcLog("���ֺ��ӣ�̨��Ǯ��", man-1, "");
				}
				FreeGame--;
				if(FreeGame == 0) {
					//mcLog("MONKEY.FreeGame��Ӯ��", TempMoney, "");
					myInfo->cash += TempMoney;
					TempMoney = 0;
				}
			} else {
				man=0;
				for(j=0;j<5;j++){
					multi = 0;
					for(i=0;i<3;i++)
						if (pattern[i][j]==10)
							multi = 1;
					man += multi;
					if(multi == 0) {
						if(j < 3)
							man = 0;
						goto ENDMAN;
					}
				}
				ENDMAN:
			
				if(man >=3) {
					FreeGame = 10;
					TempMoney = 0;
					move(16, 34);
					prints("������%d���ˣ�����10�������Ϸ���ᡣ\n",
							man);
					//mcLog("MONKEY�г���", man, "���ˣ�10��FreeGame");
					move(17, 34);
					prints("�����Ϸ��������ֺ��ӣ�"
						"��̨��Ľ��𽫻��%d��", man-1);
				}
				myInfo->cash += total - MONKEY_UNIT;
				mcEnv->prize777 -= total - MONKEY_UNIT;
				//mcLog("��MONKEYӮ��", total-MONKEY_UNIT, "");
			}
			move(18, 36);
			prints("\033[1;31m~~~~~  \033[32m~-_-~  \033[33m-----  "
					"\033[35m_-~-_  \033[36m_____\033[m");
			move(19, 34);
			for(i=0;i<5;i++)
				prints("%7d", sum[i]);
			move(20, 33);
			for(i=0;i<5;i++)
				prints("%7d", winrate[sum[i]]);
			pressanykey();
			break;
		}
	}
	return;
}
*/

int
money_gamble()
{
	int ch, quit = 0;
	char uident[IDLEN + 1];

       if (!(myInfo->GetLetter == 1)) {
           clear();
           showAt(5,4,"���Ѿ��ر��˽���������Ϸ���ܣ��뿪����������",YEA);
           return 0;
       }

	while (!quit) {
		money_show_stat("�ĳ�����");
		move(6, 4);
		prints("%s�ĳ���������𣬴�Ҿ��˰���", CENTER_NAME);
		move(t_lines - 1, 0);
		prints
		    ("\033[1;44m ѡ�� \033[1;46m [1]���� [2]777 [3] MONKEY [4] ����칫�� [Q] �뿪\033[m");
		ch = igetkey();
		switch (ch) {
		case '1':
			money_dice();
			break;
		case '2':
			money_777();
			break;
		case '3':
			//monkey_business();  //�����趨�����⣬��ʱ�ر�  
			break;
		case '4':
			whoTakeCharge(3, uident);
			if (strcmp(currentuser->userid, uident))
				break;
			nomoney_show_stat("�ĳ�����칫��");
			showAt(12, 4, "\033[1;32m���ڽ����С�\033[m", YEA);
			break;
		case 'q':
		case 'Q':
			quit = 1;
			break;
		}
	}
	return 0;
}


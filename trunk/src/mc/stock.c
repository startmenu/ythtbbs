#include "mc.h"

//---------------------------------  ����  ---------------------------------// 
static int
userSelectStock(int n, int x, int y)
{
	int stockid;
	char buf[8];

	sprintf(genbuf, "�������Ʊ�Ĵ���[0-%d  \033[1;33mENTER\033[m����]:",
		n - 1);
	while (1) {
		move(x, y);
		getdata(x, y, genbuf, buf, 3, DOECHO, YEA);
		if (buf[0] == '\0')
			return -1;
		stockid = atoi(buf);
		if (stockid >= 0 && stockid < n)
			return stockid;
	}
}

static int
trendInfo(void *stockMem, int n)
{				//��ʾ�������� 
	int i, yeste, today, change, stockid;
	char buf[STRLEN];
	struct BoardStock *bs;
	time_t currTime = time(NULL);
	struct tm *thist = localtime(&currTime);

	today = thist->tm_wday;
	yeste = (today <= 1) ? 6 : (today - 1);	//��������, ����һ������������ 
	change = mcEnv->stockPoint[today] - mcEnv->stockPoint[yeste];
	nomoney_show_stat("��ʱ���ӹ�����");
	sprintf(buf, "��ǰָ��: %d        %s: %d��", mcEnv->stockPoint[today],
		change > 0 ? "��" : "��", abs(change));
	showAt(4, 4, buf, NA);
	showAt(6, 0,
	       "                  ��һ\t  �ܶ�\t  ����\t  ����\t  ����\t  ����\n\n"
	       "    \033[1;36m��������\033[0m    ", NA);
	showAt(11, 0, "    \033[1;36m  �ɽ���\033[0m    ", NA);
	move(8, 16);
	for (i = 1; i < 7; i++)
		prints("%6d\t", mcEnv->stockPoint[i]);
	move(11, 16);
	for (i = 1; i < 7; i++)
		prints("%6d\t", mcEnv->tradeNum[i] / 100);
	showAt(14, 0, "    \033[1;33mһ������\033[0m    ", NA);
	while (1) {
		if ((stockid = userSelectStock(n, 20, 4)) == -1)
			break;
		bs = stockMem + stockid * sizeof (struct BoardStock);
		move(14, 16);
		for (i = 1; i < 7; i++) {
			sprintf(genbuf, "%6.2f\t", bs->weekPrice[i]);
			prints("%s", genbuf);
		}
		pressanykey();
	}
	return 0;
}

static int
tryPutStock(void *stockMem, int n, struct BoardStock *new_bs)
{				//��ͼ��һ֧�¹�Ʊ�����λ���ɹ������±�ֵ 
//����Ʊ�Ѵ��ڻ�û�п�λ, ����-1 
	int i, slot = -1;
	struct BoardStock *bs;

	for (i = 0; i < n; i++) {
		bs = stockMem + i * sizeof (struct BoardStock);
		if (!strcmp(bs->boardname, new_bs->boardname) && bs->timeStamp)
			return -1;
		if (bs->timeStamp == 0 && slot == -1)	//�ŵ���һ����λ 
			slot = i;
	}
	if (slot >= 0) {
		bs = stockMem + slot * sizeof (struct BoardStock);
		memcpy(bs, new_bs, sizeof (struct BoardStock));
	}
	return slot;
}

static int
newStock(void *stockMem, int n)
{				//��Ʊ���� 
	int i, money, num;
	float price;
	char boardname[24], buf[256], title[STRLEN];
	struct boardmem *bp;
	struct BoardStock bs;

	make_blist_full();
	move(10, 4);
	namecomplete("�����뽫Ҫ���е�������: ", boardname);
	FreeNameList();
	if (boardname[0] == '\0') {
		showAt(11, 4, "���������������...", YEA);
		return 0;
	}
	bp = getbcache(boardname);
	if (bp == NULL) {
		showAt(11, 4, "���������������...", YEA);
		return 0;
	}
	while (1) {
		getdata(11, 4, "��������ֵ[��λ:ǧ��, [1-200]]:", genbuf, 4,
			DOECHO, YEA);
		money = atoi(genbuf);
		if (money >= 1 && money <= 200)
			break;
	}
	price = bp->score / 1000.0;
	money *= 10000000;
	num = money / price;
	sprintf(buf, "    �������������� %d %s�Ĺ�Ʊ��\n"
		"    ������ %d ��, ÿ�ɼ۸� %.2f %s��", money, MONEY_NAME, num,
		price, MONEY_NAME);
	showAt(12, 0, buf, NA);
	move(15, 4);
	if (askyn("ȷ��������", NA, NA) == NA)
		return 0;
//��ʼ�� 
	bzero(&bs, sizeof (struct BoardStock));
	strcpy(bs.boardname, boardname);
	bs.totalStockNum = num;
	bs.remainNum = num;
	for (i = 0; i < 7; i++)
		bs.weekPrice[i] = price;
	for (i = 0; i < 4; i++)
		bs.todayPrice[i] = price;
	bs.high = price;
	bs.low = price;
	bs.boardscore = bp->score;
	bs.timeStamp = time(NULL);
	if (tryPutStock(stockMem, n, &bs) == -1)
		append_record(DIR_STOCK "stock", &bs,
			      sizeof (struct BoardStock));
	sprintf(title, "�����С�%s���������", boardname);
	deliverreport(title, buf);
	showAt(16, 4, "������ɡ�", YEA);
	return 1;
}

static void
deleteList(short stockid)
{				//ɾ��һ֧��Ʊ�ı��۶��� 
	char listpath[256];
	sprintf(listpath, "%s%d.sell", DIR_STOCK, stockid);
	unlink(listpath);
	sprintf(listpath, "%s%d.buy", DIR_STOCK, stockid);
	unlink(listpath);
}

static void
stockNews(struct BoardStock *bs)
{
	int n, rate;
	char newsbuf[STRLEN], titlebuf[STRLEN];

	n = random() % 1000;
	if (n < 50) {
		rate = random() % 6 + 5;
		bs->boardscore *= (1 + rate / 100.0);
		sprintf(newsbuf, "��Ʊ%s������������Ϣ�̼��¹ɼ���������%d%%.",
			bs->boardname, rate);
	} else if (n < 100) {
		rate = random() % 6 + 5;
		bs->boardscore *= (1 - rate / 100.0);
		sprintf(newsbuf, "��Ʊ%s�ݴ�ӯ������, �з���ָ�ɼ۽��´�%d%%.",
			bs->boardname, rate);
	}
	if (n < 100) {
		sprintf(titlebuf, "�����С�%s����������Ϣ", CENTER_NAME);
		deliverreport(titlebuf, newsbuf);
	}
}

static void
UpdateBSStatus(struct BoardStock *bs, short today, int newday, int stockid)
{				//����һ֧��Ʊ��״̬ 

	int yeste = (today <= 1) ? 6 : (today - 1);
	float delta = bs->todayPrice[3] / bs->weekPrice[yeste];
	struct boardmem *bp;
	char buf[80];

	if (bs->status == 3)
		return;
	if (delta >= 1.1)
		bs->status = 1;	//��ͣ 
	else if (delta <= 0.9)
		bs->status = 2;	//��ͣ 
	else
		bs->status = 0;	//�ָ����� 
	if (newday) {
		deleteList(stockid);
		bs->tradeNum = 0;
		bs->sellerNum = 0;
		bs->buyerNum = 0;
		bs->weekPrice[yeste] = bs->todayPrice[3];
		memset(bs->todayPrice, 0, sizeof (float) * 4);
		bs->todayPrice[3] = bs->weekPrice[yeste];
		bp = getbcache(bs->boardname);
		if (bp != NULL) {
			bs->boardscore =
			    (bp->score + bs->boardscore * 9) / 10.0;
			stockNews(bs);
		} else {
			sprintf(buf, "û���ҵ�%s�����ݣ��޷����¹�Ʊ",
				bs->boardname);
			deliverreport(buf, "");
		}
	}
}

static int
updateStockEnv(void *stockMem, int n, int flag)
{				//���¹���ͳ����Ϣ 

	int i, hour, newday = 0, totalTradeNum = 0;
	float megaStockNum = 0, megaTotalValue = 0, avgPrice;
	struct BoardStock *bs;
	struct tm thist, lastt;
	time_t currTime = time(NULL);

	if (currTime < mcEnv->lastUpdateStock + 60)	//����Ƶ������ 
		return 0;
	localtime_r(&currTime, &thist);
	hour = thist.tm_hour;
	mcEnv->stockTime = (hour >= 5 && hour < 23) && !(hour >= 13
							 && hour < 15)
	    && thist.tm_wday;
	if (!mcEnv->stockTime)	//����״̬�²����� 
		return 0;
	localtime_r(&mcEnv->lastUpdateStock, &lastt);
	mcEnv->lastUpdateStock = currTime;
	if (thist.tm_wday != lastt.tm_wday || flag)	//�Զ�ɾ�����ս��׵� 
		newday = 1;
	for (i = 0; i < n; i++) {
		bs = stockMem + i * sizeof (struct BoardStock);
		if (!bs->timeStamp)	//��Ч��Ʊ 
			continue;
		megaStockNum += bs->totalStockNum / 1000000.0;
		megaTotalValue += megaStockNum * bs->todayPrice[3];
		UpdateBSStatus(bs, thist.tm_wday, newday, i);	//���¹�Ʊ״̬ 
		totalTradeNum += bs->tradeNum;
	}
// ���¹���ָ�� 
	avgPrice = megaTotalValue / megaStockNum;
	mcEnv->stockPoint[thist.tm_wday] = avgPrice * n;
// ���³ɽ��� 
	mcEnv->tradeNum[thist.tm_wday] = totalTradeNum;
	return 0;
}

static void
showTradeInfo(void *stockMem, int n)
{				//��ʾһ֧��Ʊ�ĵ�ǰ������Ϣ 
	int i, j, m, stockid, col, line;
	int buyerNum = 0, sellerNum = 0;
	size_t filesize;
	char listpath[2][256];
	void *listMem;
	struct BoardStock *bs;
	struct TradeRecord *tr;

	money_show_stat("������Ϣ����");
	showAt(4, 4, "������Բ�ѯ��Ʊ�ĵ�ǰ������Ϣ��", NA);
	if ((stockid = userSelectStock(n, 6, 4)) == -1)
		return;
	bs = stockMem + stockid * sizeof (struct BoardStock);
	sprintf(genbuf, "ȷ��Ҫ��ѯ��Ʊ %s �Ľ�����Ϣô", bs->boardname);
	move(7, 4);
	if (askyn(genbuf, NA, NA) == NA)
		return;
	showAt(9, 0, "    ��ѯ�������[ǰ10��] ��\n"
	       "    [���  ���뱨��    �ཻ����]  ========  [���  ��������    �ཻ����]",
	       NA);
	sprintf(listpath[0], "%s%d.buy", DIR_STOCK, stockid);
	sprintf(listpath[1], "%s%d.sell", DIR_STOCK, stockid);
	for (i = 0; i < 2; i++) {
		col = (i == 0) ? 5 : 45;
		m = get_num_records(listpath[i], sizeof (struct TradeRecord));
		if (m <= 0) {
			showAt(11, col, "û�н��ױ��ۡ�", NA);
			continue;
		}
		filesize = sizeof (struct TradeRecord) * m;
		if((listMem = loadData(listpath[i], filesize)) == (void*)-1)
			continue;
		for (j = 0, line = 11; j < m && j < 10; j++) {	//����ʾǰ10 
			tr = listMem + j * sizeof (struct TradeRecord);
			if (tr->num <= 0)
				continue;
			sprintf(genbuf, "%4d  %8.2f    %8d", line - 11,
				tr->price, tr->num);
			showAt(line++, col, genbuf, NA);
			if (i == 0)
				buyerNum++;
			else
				sellerNum++;
		}
		unloadData(listMem, filesize);
	}
	bs->buyerNum = buyerNum;	//˳��ͳ����������, ����׼ȷ 
	bs->sellerNum = sellerNum;	//��Ϊֻscan��ǰ10����¼ 
	sleep(1);
	pressanykey();
}

static void
showStockDetail(struct BoardStock *bs)
{				//��ʾһ֧��Ʊ����ϸ��Ϣ 
	char strTime[16];
	struct tm *timeStamp = localtime(&(bs->timeStamp));
	char *status[] = { "\033[1;37m����\033[0m", "\033[1;31m��ͣ\033[0m",
		"\033[1;32m��ͣ\033[0m", "\033[1;34m��ͣ\033[0m"
	};

	sprintf(strTime, "%4d-%2d-%2d", timeStamp->tm_year + 1900,
		timeStamp->tm_mon + 1, timeStamp->tm_mday);
	money_show_stat("���״���");
	move(4, 4);
	prints("�����ǹ�Ʊ %s ����ϸ��Ϣ:", bs->boardname);
	move(6, 0);
	prints("      ��������: %10s\t  ������: %10d\n"
	       "      ϵͳ�ع�: %10d\t  ɢ����: %10d\n"
	       "      ��ǰ����: %10d\t��ǰ���: %10d\n"
	       "        ������: %10d\t    ״̬:   %s\n",
	       strTime, bs->totalStockNum, bs->remainNum,
	       bs->holderNum, bs->sellerNum, bs->buyerNum, bs->tradeNum,
	       status[bs->status]);
	sprintf(genbuf,
		"       ���տ���: %7.2f\t���: %7.2f\t���: %7.2f\tƽ��: %7.2f\n"
		"       ��ʷ���: %7.2f\t���: %7.2f\n",
		bs->todayPrice[0], bs->todayPrice[1], bs->todayPrice[2],
		bs->todayPrice[3], bs->high, bs->low);
	move(11, 0);
	prints("%s", genbuf);
}

static int
getMyStock(mcUserInfo * mcuInfo, int stockid)
{
//��������stockid����һ֧��Ʊ 
//���û�иù�Ʊ, ��ô�ҵ�һ����λ, ��ʼ�� 
//����ֵ�ǹ�Ʊ��λ���±�, ����Ѿ�����, ��ô���� -1 
	int i, slot = -1;

	for (i = 0; i < STOCK_NUM; i++) {
		if (mcuInfo->stock[i].stockid == stockid)
			return i;
		if (mcuInfo->stock[i].num <= 0 && slot == -1)
			slot = i;
	}
	if (slot >= 0) {
		bzero(&(mcuInfo->stock[slot]), sizeof (struct myStockUnit));
		mcuInfo->stock[slot].stockid = stockid;
	}
	return slot;
}

static void
newRecord(struct BoardStock *bs, int tradeNum, float tradePrice, int tradeType)
{				//����һ֧��Ʊ����ʷ��¼ 
	time_t currTime = time(NULL);
	struct tm *thist = localtime(&currTime);
	int today = thist->tm_wday;
	int yeste = (today <= 1) ? 6 : (today - 1);
	float UsefulPrice;
	float sup = bs->weekPrice[yeste] * 1.1;
	float inf = bs->weekPrice[yeste] * 0.9;

	if (tradeNum == 0)
		return;
	UsefulPrice = MAX(MIN(tradePrice, sup), inf);

	bs->high = MAX(tradePrice, bs->high);
	bs->low = MIN(tradePrice, bs->low);
	if (bs->todayPrice[0] == 0)	//ÿ�տ��̼� 
		bs->todayPrice[0] = tradePrice;
	bs->todayPrice[1] = MAX(tradePrice, bs->todayPrice[1]);	//ÿ����߼� 
	bs->todayPrice[2] =
	    MIN(tradePrice, (bs->todayPrice[2] == 0) ? 999 : bs->todayPrice[2]);
	bs->todayPrice[3] =
	    (tradeNum * UsefulPrice +
	     bs->todayPrice[3] * bs->tradeNum) / (bs->tradeNum + tradeNum);
	bs->tradeNum += tradeNum;
	mcEnv->tradeNum[thist->tm_wday] += tradeNum;
}

static int
addToWaitingList(struct TradeRecord *mytr, int stockid)
{				//��һ�������׵�������� 
	int i, n, slot = -1;
	char filepath[256];
	void *listMem;
	struct TradeRecord *tr;

	if (mytr->tradeType == 1)
		sprintf(filepath, "%s%d.sell", DIR_STOCK, stockid);
	else
		sprintf(filepath, "%s%d.buy", DIR_STOCK, stockid);
	n = get_num_records(filepath, sizeof (struct TradeRecord));
	if (n > 128)		//filesize <= 4k 
		return 0;
	if (n <= 0) {		//ֱ��append_record 
		append_record(filepath, mytr, sizeof (struct TradeRecord));
		return 1;
	}
	if ((listMem = loadData(filepath, sizeof (struct TradeRecord)*n)) ==(void*) -1)
		return 0;
	for (i = 0; i < n; i++) {
		tr = listMem + i * sizeof (struct TradeRecord);
		if (!strncmp(mytr->userid, tr->userid, IDLEN)) {
			slot = i;
			break;
		}
		if (slot == -1 && tr->num <= 0)	//�ҵ���һ����Ч�Ľ������� 
			slot = i;
	}
	if (slot >= 0) {	//�滻 
		tr = listMem + slot * sizeof (struct TradeRecord);
		memcpy(tr, mytr, sizeof (struct TradeRecord));
		unloadData(listMem, sizeof (struct TradeRecord) * n);
	} else {
		unloadData(listMem, sizeof (struct TradeRecord) * n);
		append_record(filepath, mytr, sizeof (struct TradeRecord));
	}
	return 1;
}

static int
tradeArithmetic(struct BoardStock *bs, struct TradeRecord *trbuy,
		struct TradeRecord *trsell, mcUserInfo * buyer,
		mcUserInfo * seller)
{				//��Ʊ���׵ĺ��ĺ��� 
	int idx, slot, iCanBuyNum, tradeNum, tradeMoney;

	if (buyer->cash < trsell->price || trbuy->price < trsell->price)
		return 0;	//ûǮ��򿪼۲��� 
	idx = getMyStock(seller, trsell->stockid);
	if (idx == -1)		//�����Ѿ�û����֧��Ʊ�� 
		return 0;
	tradeNum = MIN(trbuy->num, trsell->num);
	iCanBuyNum = buyer->cash / trsell->price;
	tradeNum = MIN(tradeNum, iCanBuyNum);
	tradeNum = MIN(tradeNum, seller->stock[idx].num);
	if (tradeNum <= 0)	//����ȷ������˫�����Խ��׵����� 
		return 0;
	tradeMoney = tradeNum * trsell->price;
	if ((slot = getMyStock(buyer, trbuy->stockid)) == -1)
		return 0;
	buyer->cash -= tradeMoney;	//�򷽿�Ǯ 
	seller->stock[idx].num -= tradeNum;	//�������й���Ҫ�� 
	if (seller->stock[idx].num <= 0)	//��������,�ֹ���-- 
		bs->holderNum--;
	if (buyer->stock[slot].num <= 0)	//������,�ֹ���++ 
		bs->holderNum++;
	buyer->stock[slot].num += tradeNum;	//������ 
	buyer->stock[slot].stockid = trbuy->stockid;
	seller->cash += after_tax(tradeMoney);	//������Ǯ 
	trsell->num -= tradeNum;	//������������������ 
	trbuy->num -= tradeNum;	//�򷽹�������ҲҪ�� 
	return tradeNum;
}

static int
tradeWithSys(struct BoardStock *bs, struct TradeRecord *mytr, float sysPrice)
{				//�û���ϵͳ���� 
	int sysTradeNum;
	mcUserInfo mcuInfo;
	struct TradeRecord systr;

	if (mytr->num <= 0)	//���û���� 
		return 0;

	myInfo->health -= 10;
	bzero(&systr, sizeof (struct TradeRecord));
	if (mytr->tradeType == 0) {	//�û����� 
		systr.price = MAX(mytr->price, sysPrice);
		systr.num = bs->remainNum;
	} else {
		systr.price = MIN(sysPrice, mytr->price);
		systr.num = mytr->num;	//ϵͳ���������й������� 
	}
//���湹��ϵͳ�Ľ��׵��ͽ����� 
	systr.stockid = mytr->stockid;
	systr.tradeType = 1 - mytr->tradeType;
	strcpy(systr.userid, "_SYS_");

	bzero(&mcuInfo, sizeof (mcUserInfo));
	mcuInfo.cash = MAX((mcEnv->Treasury - 20000000) / 2, 0);	//ȡ����һ���Ǯ,���� 2000w
	if (mcuInfo.cash <= 0)
		return 0;
	mcEnv->Treasury -= mcuInfo.cash;
	mcuInfo.stock[0].stockid = mytr->stockid;
	mcuInfo.stock[0].num = bs->remainNum;	//! 
	if (mytr->tradeType == 0)	//�û����� 
		sysTradeNum =
		    tradeArithmetic(bs, mytr, &systr, myInfo, &mcuInfo);
	else
		sysTradeNum =
		    tradeArithmetic(bs, &systr, mytr, &mcuInfo, myInfo);
	mcEnv->Treasury += mcuInfo.cash;
	if (sysTradeNum > 0) {
		bs->remainNum += mytr->tradeType ? sysTradeNum : (-sysTradeNum);
		newRecord(bs, sysTradeNum, systr.price, mytr->tradeType);
	}
	return sysTradeNum * systr.price;
}

static void
sendTradeMail(struct TradeRecord *utr, int tradeNum, float tradePrice)
{				//�ż�֪ͨ���׶Է� 
	char content[STRLEN], title[STRLEN];
	char *actionDesc[] = { "����", "����" };
	char *moneyDesc[] = { "֧��", "��ȡ" };
	int type = utr->tradeType;

	sprintf(title, "��%s��Ʊ %d �Ľ������", actionDesc[type],
		utr->stockid);
	sprintf(content, "�˴ν�����Ϊ %d ��, �ɽ��� %.2f, %s%s %d", tradeNum,
		tradePrice, moneyDesc[type], MONEY_NAME,
		(int) (tradeNum * tradePrice));
	system_mail_buf(content, strlen(content), utr->userid, title,
			currentuser->userid);
}

static int
tradeWithUser(struct BoardStock *bs, struct TradeRecord *mytr)
{				//�û���Ľ��� 
	int i, n, tradeNum, tradeMoney = 0;
	float tradePrice;
	char mcu_path[256], list_path[256];
	void *tradeList;
	mcUserInfo *mcuInfo;
	struct TradeRecord *utr;

	if (mytr->tradeType == 0)	//currentuser���� 
		sprintf(list_path, "%s%d.sell", DIR_STOCK, mytr->stockid);
	else
		sprintf(list_path, "%s%d.buy", DIR_STOCK, mytr->stockid);
	n = get_num_records(list_path, sizeof (struct TradeRecord));
	if (n <= 0)
		return 0;
	if ((tradeList = loadData(list_path, sizeof (struct TradeRecord)*n)) == (void*) -1)
		return 0;
	for (i = 0; i < n && mytr->num > 0; i++) {	//���۶��� 
		utr = tradeList + i * sizeof (struct TradeRecord);
		if (utr->userid[0] == '\0' || utr->num <= 0)
			continue;
		sethomefile(mcu_path, utr->userid, "mc.save");
	        if((mcuInfo = loadData(mcu_path, sizeof (mcUserInfo))) == (void*)-1)
			continue;
		if (mytr->tradeType == 0)	//currentuser���� 
			tradeNum =
			    tradeArithmetic(bs, mytr, utr, myInfo, mcuInfo);
		else		//currentuser����, �൱�ڶԷ����� 
			tradeNum =
			    tradeArithmetic(bs, utr, mytr, mcuInfo, myInfo);
		if (tradeNum > 0) {	//���¼�¼��֪ͨ���׷� 
			tradePrice = mytr->tradeType ? mytr->price : utr->price;
			newRecord(bs, tradeNum, tradePrice, mytr->tradeType);
			tradeMoney += tradeNum * tradePrice;
			sendTradeMail(utr, tradeNum, tradePrice);
		}
		unloadData(mcuInfo, sizeof (mcUserInfo));
	}
	unloadData(tradeList, sizeof (struct TradeRecord));
	return tradeMoney;
}

static int
tradeInteractive(struct BoardStock *bs, int stockid, int today, int type)
{				//���׵Ľ������� 
	int idx, inList = 0;
	int iHaveNum, inputNum, userTradeNum, sysTradeNum;
	int userTradeMoney, sysTradeMoney, totalMoney;
	float inputPrice, sysPrice, guidePrice;
	char buf[256];
	char *actionDesc[] = { "����", "����", NULL };
	struct TradeRecord mytr;
	int yeste = (today <= 1) ? 6 : (today - 1);
	float sup = bs->weekPrice[yeste] * 1.1;
	float inf = bs->weekPrice[yeste] * 0.9;

	idx = getMyStock(myInfo, stockid);
	if (idx == -1 && type == 0) {
		showAt(14, 4, "���Ѿ�����10֧������Ʊ��, �����ٶ�����", YEA);
		return 0;
	}
	if (myInfo->stock[idx].num <= 0 && type == 1) {
		showAt(14, 4, "��û�г���֧��Ʊ", YEA);
		return 0;
	}
	if (bs->status == 3) {
		showAt(14, 4, "��֧��Ʊ��ֹͣ����", YEA);
		return 0;
	}
	iHaveNum = myInfo->stock[idx].num;
	move(14, 4);
	sprintf(genbuf, "Ŀǰ������ %d ����֧��Ʊ, �ع��� %.4f��.",
		iHaveNum, iHaveNum / (bs->totalStockNum / 100.0));
	prints("%s", genbuf);
	sprintf(genbuf, "������Ҫ%s�Ĺ���[100]: ", actionDesc[type]);
	while (1) {
		inputNum = 100;
		getdata(15, 4, genbuf, buf, 8, DOECHO, YEA);
		if (buf[0] == '\0' || (inputNum = atoi(buf)) >= 100)
			break;
	}
	guidePrice = bs->boardscore / 1000.0;
	if (type == 0)		//currentuser����, ϵͳҪȡ�߼��� 
		sysPrice = MAX(guidePrice, bs->todayPrice[3]);
	else			//currentuser����, ϵͳ�ͼ����� 
		sysPrice = MIN(0.95 * guidePrice, bs->todayPrice[3]);
	sprintf(genbuf, "������%s����[%.2f]: ", actionDesc[type], sysPrice);
	while (1) {
		inputPrice = sysPrice;
		getdata(16, 4, genbuf, buf, 7, DOECHO, YEA);
		if (buf[0] != '\0')
			inputPrice = atof(buf);
		if (inputPrice < inf - 0.01 || inputPrice > sup + 0.01) {
			move(17, 4);
			if (askyn
			    ("��ı��۳�����ͣ(��ͣ)��λ����ȷ��Ҫ������", NA,
			     NA) == YEA)
				break;
		} else
			break;
	}
	sprintf(buf, "��ȷ���� %.2f �ı���%s %d �ɵ� %s ��Ʊ��?", inputPrice,
		actionDesc[type], inputNum, bs->boardname);
	move(17, 4);
	if (askyn(buf, NA, NA) == NA)
		return 0;
	if (type == 0 && myInfo->cash < inputNum * inputPrice) {
		showAt(18, 4, "����ֽ𲻹��˴ν���...", YEA);
		return 0;
	}
	if (type == 1 && myInfo->stock[idx].num < inputNum) {
		showAt(18, 4, "��û����ô���Ʊ...", YEA);
		return 0;
	}
//�����ҵĽ��׵� 
	bzero(&mytr, sizeof (struct TradeRecord));
	mytr.num = inputNum;
	mytr.tradeType = type;
	mytr.price = inputPrice;
	mytr.stockid = stockid;
	strcpy(mytr.userid, currentuser->userid);
//�û��佻�� 
	userTradeMoney = tradeWithUser(bs, &mytr);
	userTradeNum = abs(inputNum - mytr.num);
	if (userTradeNum > 0) {
		sprintf(buf, "�û�%s�� %d ��, �ɽ���ÿ�� %.2f %s",
			actionDesc[1 - type], userTradeNum,
			1.0 * userTradeMoney / userTradeNum, MONEY_NAME);
		showAt(18, 4, buf, YEA);
	}
//�������ʣ����δ���, ��ô��ϵͳ���� 
	sysTradeMoney = tradeWithSys(bs, &mytr, sysPrice);
	sysTradeNum = abs(inputNum - mytr.num - userTradeNum);
	if (sysTradeNum > 0) {
		sprintf(buf, "ϵͳ%s�� %d ��, �ɽ���ÿ�� %.2f %s",
			actionDesc[1 - type], sysTradeNum,
			1.0 * sysTradeMoney / sysTradeNum, MONEY_NAME);
		showAt(19, 4, buf, YEA);
	}
//������н϶�ʣ����δ����, ��ô���뽻�׶��� 
	if (mytr.num >= 100)
		inList = addToWaitingList(&mytr, stockid);

	move(20, 4);
	if (mytr.num < inputNum) {	//����н����� 
		totalMoney = userTradeMoney + sysTradeMoney;
		prints("���׳ɹ�! ��%s %d��, ���%d %s,�����Զ���˰��",
		       actionDesc[type], abs(inputNum - mytr.num), totalMoney,
		       MONEY_NAME);
		//, mytr.tradeType ? taxMoney : 0);
	}

	if (mytr.num > 0) {	//����δ��� 
		myInfo->stock[idx].status = type + 1;
		move(21, 4);
		prints("ʣ�� %d ��δ����%s", mytr.num,
		       inList ? ", �Ѿ����뱨�۶���" : ".");
	}
	pressanykey();
	return 0;
}

static int
stockTrade(void *stockMem, int n, int type)
{				//���״������� 
	int i, j, yeste, today, pages, offset, stockid;
	float delta;
	char buf[256];
	struct BoardStock *bs;
	struct tm *thist;
	time_t currTime = time(NULL);
	char *status[] = { "\033[1;37m����\033[0m", "\033[1;31m��ͣ\033[0m",
		"\033[1;32m��ͣ\033[0m", "\033[1;34m��ͣ\033[0m"
	};

	if (myInfo->health < 10) {
		showAt(4, 4, "��̫ƣ����", YEA);
		return 0;
	}
	thist = localtime(&currTime);
	today = thist->tm_wday;
	yeste = (today <= 1) ? 6 : (today - 1);
	move(4, 4);
	prints("Ŀǰ�� %d ֧��Ʊ, ��������ָ�� %d ��, ��ǰָ�� %d ��  ״̬: %s",
	       n, mcEnv->stockPoint[yeste], mcEnv->stockPoint[today],
	       mcEnv->stockTime ? "������" : "\033[1;31m������\033[0m");
	sprintf(buf, "\033[1;36m����  %20s  %8s  %8s  %8s \t%s\033[0m",
		"����", "��������", "���ճɽ�", "�Ƿ�", "״̬");
	showAt(6, 4, buf, NA);
	move(7, 4);
	prints
	    ("----------------------------------------------------------------------------------");
	pages = n / 10 + 1;
	for (i = 0;; i++) {	//i���ڿ���ҳ�� 
		for (j = 0; j < 10; j++) {	//ÿ����ʾ���10֧��Ʊ 
			offset = i * 10 + j;
			move(8 + j, 4);
			if (offset >= n || offset < 0) {
				clrtoeol();
				continue;
			}
			bs = stockMem + offset * sizeof (struct BoardStock);
			delta = bs->todayPrice[3] / bs->weekPrice[yeste] - 1;
			sprintf(buf, "[%2d]  %20s  %8.2f  %8.2f  %8.2f%%\t%s",
				offset, bs->boardname, bs->weekPrice[yeste],
				bs->todayPrice[3], delta * 100,
				status[bs->status]);
			prints("%s", buf);
			offset++;
		}
		getdata(19, 4, "[B]ǰҳ [C]��ҳ [T]���� [Q]�˳�: [C]", buf, 2,
			DOECHO, YEA);
		if (toupper(buf[0]) == 'Q')
			return 0;
		if (toupper(buf[0]) == 'T' && mcEnv->stockTime)
			break;
		if (toupper(buf[0]) == 'B')
			i = (i == 0) ? (i - 1) : (i - 2);
		else
			i = (i == pages - 1) ? (i - 1) : i;
	}
	if ((stockid = userSelectStock(n, 19, 4)) == -1)
		return 0;
	bs = stockMem + stockid * sizeof (struct BoardStock);
	showStockDetail(bs);
	tradeInteractive(bs, stockid, today, type);
	return 1;
}

static void
myStockInfo()
{				//��ʾ���˳��й�Ʊ 
	int i, count = 0;
	char *status[] = { "����", "��������", "��������", NULL };

	nomoney_show_stat("���и��˷�������");

	showAt(4, 0, "    ����������˳��й�Ʊ���: \n"
	       "    [����]\t\t��  ��  ��\t\t״  ̬", NA);
	for (i = 0; i < STOCK_NUM; i++) {
		if (myInfo->stock[i].num <= 0)
			continue;
		move(6 + count++, 0);
		prints("    [%4d]\t\t%10d\t\t%-8s", myInfo->stock[i].stockid,
		       myInfo->stock[i].num, status[myInfo->stock[i].status]);
	}
	pressreturn();
}

static int
stockAdmin(void *stockMem, int n)
{				//������� 
	int ch, quit = 0, stockid;
	struct BoardStock *bs;

	while (!quit) {
		nomoney_show_stat("֤���");
		move(4, 4);
		prints("�����ǹ�����еĻ�����������������Թ�������");
		showAt(6, 0, "        1. ��Ʊ����        2. ����/�رչ���\n"
		       "        3. ��Ʊ����        4. ��ͣ/�ָ���Ʊ Q. �˳�\n\n"
		       "    ��ѡ���������: ", NA);
		ch = igetkey();
		update_health();
		if (check_health
		    (1, 12, 4, "��Ҫ̫���࣬�ȱ�����Ъһ��ɣ�", YEA))
			continue;
		switch (ch) {
		case '1':
			myInfo->health--;
			myInfo->Actived++;
			newStock(stockMem, n);
			break;
		case '2':
			if (!mcEnv->stockOpen)
				sprintf(genbuf, "%s", "ȷ��Ҫ����������");
			else
				sprintf(genbuf, "%s", "ȷ��Ҫ�رչ�����");
			move(10, 4);
			if (askyn(genbuf, NA, NA) == NA)
				break;
			mcEnv->stockOpen = !mcEnv->stockOpen;
			update_health();
			myInfo->health--;
			myInfo->Actived++;
			showAt(11, 4, "������ɡ�", YEA);
			break;
		case '3':
			if ((stockid = userSelectStock(n, 10, 4)) == -1)
				break;
			bs = stockMem + stockid * sizeof (struct BoardStock);
			sprintf(genbuf,
				"\033[5;31m���棺\033[0mȷ��Ҫ����Ʊ %s ����ô",
				bs->boardname);
			move(11, 4);
			if (askyn(genbuf, NA, NA) == YEA) {
				bs->timeStamp = 0;	//��Ʊ������־ 
				bs->status = 3;
				deleteList(stockid);
				showAt(12, 4, "������ɡ�", YEA);
			}
			update_health();
			myInfo->health--;
			myInfo->Actived++;
			break;
		case '4':
			if ((stockid = userSelectStock(n, 10, 4)) == -1)
				break;
			bs = stockMem + stockid * sizeof (struct BoardStock);
			sprintf(genbuf, "ȷ��Ҫ\033[1;32m%s\033[0m��Ʊ %s ��",
				(bs->status == 3) ? "�ָ�" : "��ͣ",
				bs->boardname);
			move(11, 4);
			if (askyn(genbuf, NA, NA) == YEA) {
				bs->status = (bs->status == 3) ? 0 : 3;
				showAt(12, 4, "������ɡ�", YEA);
			}
			update_health();
			myInfo->health--;
			myInfo->Actived++;
			break;
		case 'q':
		case 'Q':
			quit = 1;
			break;
		}
	}
	return 0;
}

int
money_stock()
{
	int n, ch, quit = 0;
	size_t filesize;
	void *stockMem;
	char buf[256], uident[IDLEN + 1];

	if (!(myInfo->GetLetter == 1)) {
		clear();
		showAt(5, 4, "���Ѿ��ر��˽���������Ϸ���ܣ��뿪����������",
		       YEA);
		return 0;
	}

	clear();
	whoTakeCharge(6, uident);
	if (!mcEnv->stockOpen && !USERPERM(currentuser, PERM_SYSOP)
	    && (strcmp(currentuser->userid, uident))) {
		showAt(6, 16, "\033[1;31m������ͣ����  ���Ժ�����\033[0m", YEA);
		return 0;
	}
	if (!file_exist(DIR_STOCK "stock"))
		initData(2, DIR_STOCK "stock");
	n = get_num_records(DIR_STOCK "stock", sizeof (struct BoardStock));
	if (n <= 0)
		return 0;
	filesize = sizeof (struct BoardStock) * n;
//���ع�����Ϣ 
	if ((stockMem = loadData(DIR_STOCK "stock", filesize)) == (void*)-1)
		return -1;
	while (!quit) {
		limit_cpu();
		sprintf(buf, "%s����", CENTER_NAME);
		money_show_stat(buf);
		showAt(4, 4, "\033[1;33m��Ʊ�з��գ��ǵ���Ԥ�ϡ�\n"
		       "    ������������Ʋ����Ը�!\033[0m\n\n"
		       "        ���п���ʱ��\n\n"
		       "    ��һ�������� 5�㵽13��\n"
		       "            ����15�㵽23��", NA);
		move(t_lines - 1, 0);
		prints("\033[1;44m ѡ�� \033[1;46m [1]���� [2]���� [3]��Ϣ "
		       "[4]���� [5]���� [9]֤��� [Q]�뿪\033[m");
		ch = igetkey();
		switch (ch) {
		case '1':
		case '2':
			money_show_stat("���״���");
			stockTrade(stockMem, n, ch - '1');
			break;
		case '3':
			showTradeInfo(stockMem, n);
			break;
		case '4':
			trendInfo(stockMem, n);
			break;
		case '5':
			myStockInfo();
			break;
		case '9':
			whoTakeCharge(6, uident);
			if (strcmp(currentuser->userid, uident)
			    && !(currentuser->userlevel & PERM_SYSOP))
				break;
			stockAdmin(stockMem, n);
			break;
		case 'q':
		case 'Q':
			quit = 1;
			break;
		}
		updateStockEnv(stockMem, n, 0);
		if (!mcEnv->stockOpen)	//�ߵ������������ 
			break;
	}
	unloadData(stockMem, filesize);
	return 0;
}

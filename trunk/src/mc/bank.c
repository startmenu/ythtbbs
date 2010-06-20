#include "mc.h"

// ------------------------   ����   ----------------------- //
static int
makeInterest(int basicMoney, time_t lastTime, float rate)
{				// ������Ϣ 
	int calHour;
	time_t currTime = time(NULL);

	if (lastTime > 0 && currTime > lastTime) {
		calHour = (currTime - lastTime) / 3600;
		calHour = MIN(calHour, 87600);
		return basicMoney * rate * calHour / 24;
	}
	return 0;
}

static int
setBankRate(int rateType)
{
	char buf[STRLEN], strRate[10];
	char rateDesc[][10] = { "�����", "������", "ת�ʷ�" };
	unsigned char rate;

	sprintf(buf, "�趨�µ�%s��[10-250]: ", rateDesc[rateType]);
	getdata(12, 4, buf, strRate, 4, DOECHO, YEA);
	rate = atoi(strRate);
	if (rate < 10 || rate > 250) {
		showAt(13, 4, "����������Χ!", YEA);
		return 1;
	}
	move(13, 4);
	sprintf(buf, "�µ�%s����%.2f����ȷ����", rateDesc[rateType],
		rate / 100.0);
	if (askyn(buf, NA, NA) == NA)
		return 1;
	if (rateType == 0)
		mcEnv->depositRate = rate;
	else if (rateType == 1)
		mcEnv->loanRate = rate;
	else
		mcEnv->transferRate = rate;
	update_health();
	myInfo->health--;
	myInfo->Actived++;
	sprintf(genbuf, "�µ�%s��Ϊ %.2f�� ��", rateDesc[rateType],
		rate / 100.0);
	sprintf(buf, "�����С�%s���е���%s��", CENTER_NAME, rateDesc[rateType]);
	deliverreport(buf, genbuf);
	showAt(14, 4, "�������", YEA);
	return 0;
}

static int
bank_saving()
{
	char ch, quit = 0, buf[STRLEN], getInterest;
	float rate = mcEnv->depositRate / 10000.0;
	int num, total_num;

	money_show_stat("���д����");
	sprintf(buf, "������ʣ��գ�Ϊ %.2f��", rate * 100);
	showAt(4, 4, buf, NA);
	move(t_lines - 1, 0);
	prints("\033[1;44m ѡ�� \033[1;46m [1]��� [2]ȡ�� [Q]�뿪\033[m");
	ch = igetkey();
	switch (ch) {
	case '1':
		if (check_health(1, 12, 4, "�������������ˣ�", YEA))
			break;
		num =
		    userInputValue(6, 4, "��", MONEY_NAME, 1000, MAX_MONEY_NUM);
		if (num == -1)
			break;
		if (myInfo->cash < num) {
			showAt(8, 4, "��û����ô��Ǯ���Դ档", YEA);
			break;
		}
		myInfo->cash -= num;
/* ����ԭ�ȴ�����Ϣ */
		myInfo->interest +=
		    makeInterest(myInfo->credit, myInfo->depositTime, rate);
/* �µĴ�ʼʱ�� */
		myInfo->depositTime = time(NULL);
		myInfo->credit += num;
		update_health();
		myInfo->health--;
		myInfo->Actived++;
		move(8, 4);
		prints("���׳ɹ��������ڴ��� %d %s����Ϣ���� %d %s��",
		       myInfo->credit, MONEY_NAME, myInfo->interest,
		       MONEY_NAME);
		pressanykey();
		break;
	case '2':
		if (check_health(1, 12, 4, "�������������ˣ�", YEA))
			break;
		num =
		    userInputValue(6, 4, "ȡ", MONEY_NAME, 1000, MAX_MONEY_NUM);
		if (num == -1)
			break;
		if (num > myInfo->credit) {
			showAt(8, 4, "��û����ô���", YEA);
			break;
		}
		myInfo->interest += makeInterest(num,
						 myInfo->depositTime, rate);
		move(8, 4);
		sprintf(genbuf, "�Ƿ�ȡ�� %d %s�Ĵ����Ϣ",
			myInfo->interest, MONEY_NAME);
		if (askyn(genbuf, NA, NA) == YEA) {
/* ������Ϣ */
			total_num = num + MAX(MIN(myInfo->interest,mcEnv->Treasury-2000000),0);
			mcEnv->Treasury -= MAX(MIN(myInfo->interest,mcEnv->Treasury-2000000),0);
			myInfo->interest = 0;
			getInterest = 1;
		} else {
			total_num = num;
			getInterest = 0;
		}
		myInfo->credit -= num;
		myInfo->cash += total_num;
		update_health();
		myInfo->health--;
		myInfo->Actived++;
		move(9, 4);
		prints("���׳ɹ��������ڴ��� %d %s�������Ϣ���� %d %s��",
		       myInfo->credit, MONEY_NAME, myInfo->interest,
		       MONEY_NAME);
		pressanykey();
		break;
	case 'Q':
	case 'q':
		quit = 1;
		break;
	}
	return quit;
}

static int
bank_loan()
{
	char ch, quit = 0, buf[STRLEN];
	float rate = mcEnv->loanRate / 10000.0;
	int num, total_num, hour, maxLoanMoney;
	time_t currTime = time(NULL);

	money_show_stat("���д����");
	sprintf(buf, "�������ʣ��գ�Ϊ %.2f��", rate * 100);
	showAt(4, 4, buf, NA);
	move(5, 4);
	hour = (myInfo->backTime - currTime) / 3600 + 1;
	total_num =
	    myInfo->loan + makeInterest(myInfo->loan, myInfo->loanTime, rate);
	if (myInfo->GetLetter == 0)
		total_num /= 10;
	if (myInfo->loan > 0) {
		prints("������ %d %s����ǰ��Ϣ���� %d %s���ൽ�� %d Сʱ��",
		       myInfo->loan, MONEY_NAME, total_num, MONEY_NAME, hour);
	} else
		prints("��Ŀǰû�д��");
	move(t_lines - 1, 0);
	prints("\033[1;44m ѡ�� \033[1;46m [1]���� [2]���� [Q]�뿪\033[m");
	ch = igetkey();
	switch (ch) {
	case '1':

		if (!(myInfo->GetLetter == 1)) {
			clear();
			showAt(5, 4,
			       "���Ѿ��ر��˽���������Ϸ���ܣ��뿪����������",
			       YEA);
			return 1;
		}

		if (check_health(1, 12, 4, "�������������ˣ�", YEA))
			break;
		maxLoanMoney = MIN(countexp(currentuser, 2) * 250, 10000000);
		move(6, 4);
		if (maxLoanMoney < 1000) {
			showAt(8, 4, "�Բ�������û�д�����ʸ�", YEA);
			break;
		}
		prints("�������еĹ涨����Ŀǰ������������� %d %s��",
		       maxLoanMoney, MONEY_NAME);
		num =
		    userInputValue(7, 4, "��", MONEY_NAME, 1000, MAX_MONEY_NUM);
		if (num == -1)
			break;
		if (myInfo->loan > 0) {
			showAt(8, 4, "���Ȼ�����", YEA);
			break;
		}
		if (num > maxLoanMoney) {
			showAt(8, 4, "�Բ�����Ҫ�����Ľ������й涨��",
			       YEA);
			break;
		}
		while (1) {
			getdata(8, 4, "��Ҫ��������죿[3-30]: ", buf,
				3, DOECHO, YEA);
			if (atoi(buf) >= 3 && atoi(buf) <= 30)
				break;
		}
		if (mcEnv->Treasury - 10000000 < num) {	//����1000�����
			showAt(8, 4, "�Բ��𣬹�����飬�޷����", YEA);
			break;
		}
		myInfo->loanTime = currTime;
		myInfo->backTime = currTime + atoi(buf) * 86400;
		mcEnv->Treasury -= num;
		myInfo->loan += num;
		myInfo->cash += num;
		update_health();
		myInfo->health--;
		myInfo->Actived++;
		showAt(9, 4, "���Ĵ��������Ѿ���ɡ��뵽�ڻ��", YEA);
		break;
	case '2':
		if (check_health(1, 12, 4, "�������������ˣ�", YEA))
			break;
		if (myInfo->loan == 0) {
			showAt(6, 4, "���Ǵ��˰ɣ�û���ҵ����Ĵ����¼����",
			       YEA);
			break;
		}
		if (time(NULL) < myInfo->loanTime + 86400 * 3) {
			move(6, 4);
			if (askyn
			    ("��Ҫ��ǰ����������(����ȡ1%������)", NA,
			     NA) == NA)
				break;
			total_num *= 1.01;
			move(5, 4);
			prints
			    ("������ %d %s����ǰ��Ϣ���� %d %s���ൽ�� %d Сʱ��",
			     myInfo->loan, MONEY_NAME, total_num, MONEY_NAME,
			     hour);
		} else {
			move(6, 4);
			if (askyn("��Ҫ���ڳ���������", NA, NA) == NA)
				break;
		}
		if (myInfo->cash < total_num) {
			showAt(7, 4, "�Բ�������Ǯ�����������", YEA);
			break;
		}
		myInfo->cash -= total_num;
		mcEnv->Treasury += total_num;
		myInfo->loan = 0;
		myInfo->loanTime = 0;
		myInfo->backTime = 0;
		update_health();
		myInfo->health--;
		myInfo->Actived++;
		showAt(7, 4, "���Ĵ����Ѿ����塣�����ּ����������ĳ��š�", YEA);
		break;
	case 'q':
	case 'Q':
		quit = 1;
		break;
	}
	return quit;
}

#if 0
static int
bank_sploan()
{
	money_show_stat("������ҵ����Ǣ̸��");
	showAt(6, 4,
	       "\033[1;33m��ҵ������Ϊ�˻�����ʱ���ʽ���ת��������õġ�\n    ���������ͨ������ص��ǽ�����Ϣ�ߣ�Ƿ���ͷ��ء�\n\n    ��ҵ����Ŀͻ��Ƕĳ����̵�Ⱦ�Ӫ��ҵ��а��ˣ��ڰ�ؤ������Ÿ����ˡ�\n\n\033[1;32m    ����������ϸ�ﻮ�С�����\033[m",
	       YEA);
	return 1;
}
#endif

static void
moneytransfer()
{
	int ch, num, total_num;
	char uident[IDLEN + 1], title[STRLEN], buf[256], reason[256];
	float transferRate = mcEnv->transferRate / 10000.0;
	float rate = mcEnv->depositRate / 10000.0;
	mcUserInfo *mcuInfo;

	money_show_stat("����ת�˴���");
	if (check_health(1, 12, 4, "�������������ˣ�", YEA))
		return;
	move(4, 4);
	sprintf(genbuf,
		"��Сת�˽�� 1000 %s�������� %.2f�� ",
		MONEY_NAME, transferRate * 100);
	prints("%s", genbuf);
	if (!getOkUser("ת�˸�˭��", uident, 5, 4)) {
		showAt(12, 4, "���޴���", YEA);
		return;
	}
	if (!strcmp(uident, currentuser->userid)) {
		showAt(12, 4, "�ˣ����ǡ���ɶ�أ���", YEA);
		return;
	}
	num = userInputValue(6, 4, "ת��", MONEY_NAME, 1000, MAX_MONEY_NUM);
	if (num == -1)
		return;
	total_num = num + num * transferRate;
	getdata(7, 4, "���ԣ�", reason, 40, DOECHO, YEA);
	prints("\n    ��ѡ�񣺴�[1]�ֽ� [2]��� ת�ʣ�");
	ch = igetkey();
	sethomefile(buf, uident, "mc.save");
	if (!file_exist(buf))
		initData(1, buf);
	if ((mcuInfo = loadData(buf, sizeof (mcUserInfo))) == (void*)-1)
		return;

	switch (ch) {
	case '1':
		if (myInfo->cash < total_num) {
			move(12, 4);
			prints("�����ֽ𲻹����������Ѵ˴ν��׹��� %d %s",
			       total_num, MONEY_NAME);
			pressanykey();
			return;
		}
		myInfo->cash -= total_num;
		mcuInfo->interest +=
		    makeInterest(mcuInfo->credit, mcuInfo->depositTime, rate);
		mcuInfo->depositTime = time(NULL);
		mcEnv->Treasury += num * transferRate;

		if (!(myInfo->GetLetter == 0) && (mcuInfo->GetLetter == 0)) {
			move(12, 4);
			prints
			    ("�Է����е���������ң�����ת�ʵ���Ϸ���Զ�����20:1�һ�Ϊ������ҡ�");
			num /= 20;
			mcEnv->Treasury += num * 19;
		}
		if ((myInfo->GetLetter == 0) && (mcuInfo->GetLetter == 1)) {
			move(12, 4);
			prints
			    ("�Է����е�����Ϸ�ң�����ת�ʵ�������ұ��Զ�����1:10�һ�Ϊ��Ϸ�ҡ�");
			mcEnv->Treasury -= num * 9;
			num *= 10;
		}

		mcuInfo->credit += num;
		myInfo->Actived++;
		break;
	default:
		if (myInfo->credit < total_num) {
			move(12, 4);
			prints("���Ĵ������������Ѵ˴ν��׹��� %d %s",
			       total_num, MONEY_NAME);
			pressanykey();
			return;
		}
		myInfo->credit -= total_num;
		myInfo->interest +=
		    makeInterest(total_num, myInfo->depositTime, rate);
		mcuInfo->interest +=
		    makeInterest(mcuInfo->credit, mcuInfo->depositTime, rate);
		mcuInfo->depositTime = time(NULL);
		mcEnv->Treasury += num * transferRate;

		if (!(myInfo->GetLetter == 0) && (mcuInfo->GetLetter == 0)) {
			move(12, 4);
			prints
			    ("�Է����е���������ң�����ת�ʵ���Ϸ���Զ�����20:1�һ�Ϊ������ҡ�");
			num /= 20;
			mcEnv->Treasury += num * 19;
		}
		if ((myInfo->GetLetter == 0) && (mcuInfo->GetLetter == 1)) {
			move(12, 4);
			prints
			    ("�Է����е�����Ϸ�ң�����ת�ʵ�������ұ��Զ�����1:10�һ�Ϊ��Ϸ�ҡ�");
			mcEnv->Treasury -= num * 9;
			num *= 10;
		}

		mcuInfo->credit += num;
		myInfo->Actived++;
		break;
	}

	update_health();
	myInfo->health--;
	sprintf(title, "�������� %s ������Ǯ����", currentuser->userid);
	sprintf(buf, "%s ͨ��%s���и���ת���� %d %s������ա�\n ����:%s",
		currentuser->userid, CENTER_NAME, num, MONEY_NAME, reason);
//      if (mcuInfo->GetLetter == 1)
	system_mail_buf(buf, strlen(buf), uident, title, currentuser->userid);
	if (!(random() % 10) || num >= 10000000) {
		sprintf(title, "�����С�%s�ʽ�ת��", currentuser->userid);
		sprintf(buf, "%s ͨ��%s����ת���� %d %s ��%s��",
			currentuser->userid, CENTER_NAME, num, MONEY_NAME,
			uident);
		deliverreport(title, buf);
	}
	sprintf(buf, "��%sת��%d%s�ɹ��������Ѿ�֪ͨ���������ѡ�",
		(ch == '1') ? "�ֽ�" : "���", num, MONEY_NAME);
	showAt(14, 4, buf, YEA);
	unloadData(mcuInfo, sizeof (mcUserInfo));
	return;
}

int
newSalary()
{
	time_t currTime = time(NULL);

	if (currTime > mcEnv->salaryEnd) {
		mcEnv->salaryStart = currTime;
		mcEnv->salaryEnd = currTime + PAYDAY * 86400;
		return 1;
	}
	return 0;
}

static int
makeSalary(int *salary, int *exp)
{

	if (currentuser->userlevel & PERM_SYSOP ||
	    currentuser->userlevel & PERM_EXT_IDLE ||
	    currentuser->userlevel & PERM_SPECIAL4) {
		*salary += 150000;
		*exp += 50;
	}
	if (currentuser->userlevel & PERM_OBOARDS ||
	    currentuser->userlevel & PERM_ACCOUNTS ||
	    currentuser->userlevel & PERM_ARBITRATE ||
	    currentuser->userlevel & PERM_SPECIAL5 ||
	    currentuser->userlevel & PERM_SPECIAL6 ||
	    currentuser->userlevel & PERM_ACBOARD) {
		*salary += 100000;
		*exp += 50;
	}
	if (currentuser->userlevel & PERM_BOARDS) {
		*salary += MIN(getbmnum(currentuser->userid) * 50000, 200000);
		*exp += getbmnum(currentuser->userid) * 50;
	}
	*salary = MIN(300000, *salary);
	*exp = MIN(200, *exp);
	return *salary;

}

int
after_tax(int income)
{
	int after;
	after = income;
	if (income > 10000)
		after = MAX(income * 0.95, 10000);
	if (income > 100000)
		after = MAX(income * 0.9, 95000);
	if (income > 1000000)
		after = MAX(income * 0.85, 900000);
	if (income > 10000000)
		after = MAX(income * 0.8, 8500000);
	if (income > 50000000)
		after = MAX(income * 0.7, 40000000);
	mcEnv->Treasury += income - after;
	return after;
}

void
forcetax()
{
	int tax_paid, cash, credit;

	cash = myInfo->cash;
	credit = myInfo->credit;
	myInfo->cash = after_tax(myInfo->cash);
	myInfo->credit = after_tax(myInfo->credit);
	tax_paid = cash + credit - myInfo->cash - myInfo->credit;

	clear();
	move(4, 4);
	if (askyn("������˰��һ������Ӧ����������Ҫ����", YEA, NA) == NA) {
		prints
		    ("\n    ��͵˰©˰��������%d������Ƿ˰��������10���ӣ�",
		     tax_paid);
		myInfo->freeTime = time(NULL) + 600;
		myInfo->mutex = 0;
		unloadData(myInfo, sizeof (mcUserInfo));
		unloadData(mcEnv, sizeof (MC_Env));
		pressreturn();
		Q_Goodbye();
	}
	myInfo->health = 100 + myInfo->con * 2;
	myInfo->robExp += tax_paid / 100000;
	myInfo->begExp += tax_paid / 100000;
	myInfo->luck += tax_paid / 100000;
	move(6, 4);
	prints("������%d��˰����Ʒ������", tax_paid);
	pressanykey();

	return;
}

int
money_bank()
{
	int ch, quit = 0, salary = 0, exp = 0;
	char uident[IDLEN + 1], admin[IDLEN + 1], buf[256];
	struct userdata data;

	while (!quit) {
		sprintf(buf, "%s����", CENTER_NAME);
		money_show_stat(buf);
		move(8, 16);
		prints("%s���л�ӭ���Ĺ��٣�", CENTER_NAME);
		move(t_lines - 1, 0);
		prints("\033[1;44m ѡ�� \033[1;46m [1]ת�� [2]����"
		       " [3]���� [4]���� [5]�г��칫�� [Q]�뿪\033[m");
		ch = igetkey();
		switch (ch) {
		case '1':
			moneytransfer();
			break;
		case '2':
			while (1) {
				if (bank_saving() == 1)
					break;
			}
			break;
		case '3':
			while (1) {
				if (bank_loan() == 1)
					break;
			}
			break;
#if 0
		case '4':
			while (1) {
				if (bank_sploan() == 1)
					break;
			}
			break;
#endif
		case '4':
			money_show_stat("���й��ʴ��촰��");
			if (check_health(1, 12, 4, "�������������ˣ�", YEA))
				break;
			salary = 0;
			exp = 0;
			makeSalary(&salary, &exp);
			if (salary == 0) {
				showAt(10, 10, "�����Ǳ�վ����Ա��û�й��ʡ�",
				       YEA);
				break;
			}

			if (mcEnv->salaryStart == 0
			    || time(NULL) > mcEnv->salaryEnd) {
				showAt(10, 10,
				       "�Բ������л�û���յ����ʻ��", YEA);
				break;
			}

			if (myInfo->lastSalary >= mcEnv->salaryStart) {
				sprintf(genbuf,
					"���Ѿ�����������������ڷܹ����ɣ�\n"
					"          �´η��������ڣ�%16s",
					ctime(&mcEnv->salaryEnd));
				showAt(10, 10, genbuf, YEA);
				break;
			}

			move(6, 4);
			sprintf(genbuf,
				"�����ܵĹ��� %d %s�� %d����ֵ�Ѿ����ʡ�������ȡ��",
				salary, MONEY_NAME, exp);
			if (askyn(genbuf, NA, NA) == NA)
				break;
			if (mcEnv->Treasury < salary) {
				showAt(10, 10,
				       "�Բ�����������û���ֽ������Ժ�������",
				       YEA);
				break;
			}
			myInfo->lastSalary = mcEnv->salaryEnd;
			myInfo->cash += salary;
			if (myInfo->GetLetter == 1)
				mcEnv->Treasury -= salary;
			else
				mcEnv->Treasury -= salary * 10;
			update_health();
			myInfo->health--;
			myInfo->Actived++;
			loaduserdata(currentuser->userid, &data);
			data.extraexp += exp;
			saveuserdata(currentuser->userid, &data);
			showAt(8, 4, "���������Ĺ��ʡ���л���������Ĺ���!",
			       YEA);
			break;
		case '5':
			money_show_stat("�г��칫��");
			move(6, 4);
			whoTakeCharge(1, uident);
			whoTakeCharge(0, admin);
			if ((strcmp(currentuser->userid, uident))
			    && (strcmp(currentuser->userid, admin)))
				break;
			prints("��ѡ���������:");
			move(7, 0);
			sprintf(genbuf,
				"      1. ����������ʡ�Ŀǰ����: %.2f��\n"
				"      2. �����������ʡ�Ŀǰ����: %.2f��\n"
				"      3. ����ת�ʷ��ʡ�Ŀǰ����: %.2f��\n"
				"      4. �����ʡ��´η���������: \033[1;33m%16s\033[m\n"
				"      Q. �˳�", mcEnv->depositRate / 100.0,
				mcEnv->loanRate / 100.0,
				mcEnv->transferRate / 100.0,
				ctime(&mcEnv->salaryEnd));
			prints(genbuf);
			ch = igetkey();
			switch (ch) {
			case '1':
			case '2':
			case '3':
				update_health();
				if (check_health
				    (1, 12, 4, "������̫�����ˣ���Ϣһ�°ɣ�",
				     YEA))
					break;
				setBankRate(ch - '1');
				break;
			case '4':
				update_health();
				if (check_health
				    (1, 12, 4, "������̫�����ˣ���Ϣһ�°ɣ�",
				     YEA))
					break;
				move(12, 4);
				if (!newSalary()) {
					showAt(12, 4, "��δ������ʱ�䡣", YEA);
					break;
				}
				if (askyn("ȷ�����Ź�����", NA, NA) == NA)
					break;
				update_health();
				myInfo->health--;
				myInfo->Actived++;
				sprintf(buf,
					"����%d���ڵ�%s������ȡ��������Ϊ������",
					PAYDAY, CENTER_NAME);
				deliverreport("�����С���վ����Ա��ȡ����",
					      buf);
				showAt(14, 4, "������ɡ�", YEA);
				break;
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

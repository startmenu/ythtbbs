#include "mc.h"

//  ---------------------------   ����     --------------------------  // 
static int
cop_accuse()
{
	char uident[IDLEN + 1], buf[256];
	mcUserInfo *mcuInfo;
	time_t currTime;

	move(4, 4);
	prints("������������ٻ�͵�ԣ���������κ����������򾯷����档");
	move(5, 4);
	prints("\033[1;32m�������������������þ��棡\033[0m");
	if (!getOkUser("�ٱ�˭��", uident, 7, 4))
		return 0;
	move(8, 4);
	sethomefile(buf, uident, "mc.save");
	if (!file_exist(buf))
		initData(1, buf);
	if((mcuInfo = loadData(buf, sizeof (mcUserInfo))) == (void*)-1)
		return 0;
	if (seek_in_file(DIR_MC "policemen", uident)) {
		showAt(8, 4, "�󵨣������ݾ�����Ա�𣿣�", YEA);
		goto UNMAP;
	}
	if (mcuInfo->freeTime > 0) {
		showAt(8, 4, "������Ѿ����������ˡ�", YEA);
		goto UNMAP;
	}
	currTime = time(NULL);
	if ((mcuInfo->robExp == 0) || (mcuInfo->begExp == 0)
	    || (mcuInfo->lastActiveTime + 7200 < currTime) || !(mcuInfo->BeIn)) {
		showAt(8, 4, "���������ܰ��ְ����㲻Ҫ�̰�����Ŷ��", YEA);
		goto UNMAP;
	}
	if (seek_in_file(DIR_MC "criminals_list", uident)) {
		showAt(8, 4, "�����Ѿ�������ͨ���ˣ�������Ȼ�����ʾ��л��",
		       YEA);
		goto UNMAP;
	}
	getdata(8, 4, "��������[\033[1;33mENTER\033[0m����]��",
		genbuf, 40, DOECHO, YEA);
	if (genbuf[0] == '\0')
		goto UNMAP;
	move(9, 4);
	if (askyn("\033[1;33m���򾯷��ṩ��������Ϣ��ʵ��\033[0m", NA, NA) ==
	    NA)
		goto UNMAP;
	snprintf(buf, STRLEN - 1, "%s %s", uident, genbuf);
	addtofile(DIR_MC "criminals_list", buf);
	showAt(10, 4, "�����ǳ���л���ṩ�����������ǽ����������ư���", YEA);
	return 1;

      UNMAP:unloadData(mcuInfo, 1);
	return 0;
}

static int
cop_arrange(int type)
{
	int found;
	char uident[IDLEN + 1], buf[STRLEN], title[STRLEN];
	char *actionDesc[] = { "-", "����", "��ְ", NULL };
	mcUserInfo *mcuInfo;

	if (!getOkUser("������ID: ", uident, 12, 4))
		return 0;
	found = seek_in_file(DIR_MC "policemen", uident);
	move(13, 4);
	if (type == 1 && found) {
		showAt(13, 4, "��ID�Ѿ��Ǿ�Ա�ˡ�", YEA);
		return 0;
	} else if (type == 2 && !found) {
		showAt(13, 4, "��ID���Ǿ���Ա��", YEA);
		return 0;
	}
	if (type == 1 && (clubtest(ROBUNION) || clubtest(BEGGAR))) {
		showAt(13, 4, "��������ϵ���������˹���Ϊ��Ա��", YEA);
		return 0;
	}

	sprintf(buf, "%sȷ��%s��",
		type == 2 ? "����;�Ա����ʧһ�뵨ʶ������" : "",
		actionDesc[type]);
	if (askyn(buf, NA, NA) == NA)
		return 0;
	if (type == 1)
		addtofile(DIR_MC "policemen", uident);
	else {
		sethomefile(buf, uident, "mc.save");
		if (!file_exist(buf))
			initData(1, buf);
		if((mcuInfo = loadData(buf, sizeof (mcUserInfo))) == (void*)-1)
			return 0;
		mcuInfo->robExp /= 2;
		mcuInfo->begExp /= 2;
		mcuInfo->luck = -100;
		del_from_file(DIR_MC "policemen", uident);
		unloadData(mcuInfo, sizeof (mcUserInfo));
	}
	sprintf(title, "������%s��Ա %s", actionDesc[type], uident);
	sprintf(buf, "������%s %s��Ա %s", currentuser->userid,
		actionDesc[type], uident);
	deliverreport(title, buf);
	system_mail_buf(buf, strlen(buf), uident, title, currentuser->userid);
	showAt(14, 4, "�����ɹ���", YEA);
	return 1;
}

static void
cop_Arrest(void)
{
	char uident[IDLEN + 1], police[IDLEN + 1], buf[256], title[STRLEN];
	char *CrimeName[] =
	    { "Ī����", "���˷绯", "С͵С��", "�ƻ��ΰ�", "�������",
		"ɱ�˷Ż�", "�߸�����", "����ᷴ����", "�������", NULL
	};
	int CrimeTime[] = { 1, 2, 3, 4, 6, 8, 12, 16, 24 };
	time_t currtime;
	mcUserInfo *mcuInfo;
	int num, tranum, crime = 0, bonus = 100;

	nomoney_show_stat("�̾���");
	if (!seek_in_file(DIR_MC "policemen", currentuser->userid)) {
		showAt(12, 4, "����ִ�����������˵���رܣ��������ˡ�", YEA);
		return;
	}
	if (clubtest(ROBUNION) || clubtest(BEGGAR)) {
		showAt(12, 4, "���޼���������˰��㣿", YEA);
		return;
	}

	if (myInfo->robExp < 50 || myInfo->begExp < 50) {
		showAt(12, 4,
		       "�¾���ɣ���ȥ��������������Ȼ���������ˡ����͹��������㲻���װ���",
		       YEA);
		return;
	}
	if (check_health(100, 12, 4, "��û�г����������ִ������", YEA))
		return;

	currtime = time(NULL);
	if (myInfo->WorkPoint < 7200) {
		showAt(12, 4, "��Ҫ�ż����ϼ���û���ж�ָʾ��", YEA);
		return;
	}

	move(5, 4);
	prints("�ոսӵ��ϼ�ָʾ������Ҫ����һ��ץ���ж���");

	if (!getOkUser("\n��ѡ�����Ŀ�꣺", uident, 6, 4)) {
		move(8, 4);
		prints("���޴���");
		pressanykey();
		return;
	}
	if (!strcmp(uident, currentuser->userid)) {
		showAt(8, 4, "ţħ���������š��������񾭲�������", YEA);
		return;
	}
	if (!seek_in_file(DIR_MC "criminals_list", uident)) {
		showAt(8, 4, "���˲���ͨ���������档��", YEA);
		return;
	}
	myInfo->Actived += 10;
	move(10, 4);
	prints("������ڴ�������ıؾ�֮·���ȴ�Ŀ��ĳ��֡�");
	sleep(1);

	sethomefile(buf, uident, "mc.save");
	if (!file_exist(buf))
		initData(1, buf);
	if((mcuInfo = loadData(buf, sizeof (mcUserInfo))) == (void*)-1)
		return;

	myInfo->lastActiveTime = currtime;
	myInfo->WorkPoint -= 7200;
	sleep(1);
	if (currtime > 3600 + mcuInfo->lastActiveTime) {
		myInfo->health -= 10;
		showAt(12, 4, "���������һ�죬Ŀ�껹��û�г��֣�ֻ�÷����ˡ�",
		       YEA);
		return;
	}

/*	if (!(random() % 3)) {
		myInfo->health -= 10;
		showAt(12, 4, "���ۿ���Զ����Ӱһ�Σ�Ŀ��õ������ܵ��ˡ�",
		       YEA);
		return;
	}

	if ((myInfo->robExp + myInfo->begExp >=
	     (mcuInfo->robExp + mcuInfo->begExp)
	     * 5 + 50 || myInfo->luck - mcuInfo->luck >= 100 || (random() % 3))
	    && !(mcuInfo->robExp + mcuInfo->begExp >= (myInfo->
						       robExp +
						       myInfo->begExp) * 5 +
		 50)) {
*/
	num =
	    myInfo->robExp + myInfo->begExp + (mcuInfo->robExp +
					       mcuInfo->begExp) * 2;
	if (num > 150000)
		crime = 8;
	else if (num > 100000)
		crime = 7;
	else if (num > 50000)
		crime = 6;
	else if (num > 10000)
		crime = 5;
	else if (num > 5000)
		crime = 4;
	else if (num > 2000)
		crime = 3;
	else if (num > 1000)
		crime = 2;
	else if (num > 500)
		crime = 1;
	else
		crime = 0;
	tranum =
	    MIN((myInfo->robExp + myInfo->begExp + mcuInfo->robExp +
		 mcuInfo->begExp) / 250,
		(mcuInfo->robExp + mcuInfo->begExp) / 50);
	whoTakeCharge(8, police);
	if (!(strcmp(currentuser->userid, police)))
		bonus = 300;
	if (check_chance
	    (myInfo->robExp + myInfo->begExp, mcuInfo->robExp + mcuInfo->begExp,
	     myInfo->weapon, mcuInfo->armor, 200, bonus)) {
		myInfo->begExp += tranum + 10;
		myInfo->robExp += tranum + 10;
		myInfo->health = 0;
		myInfo->cash += mcuInfo->cash;
		mcuInfo->cash = 0;
		mcuInfo->begExp = MAX(0, mcuInfo->begExp - tranum - 10);
		mcuInfo->robExp = MAX(0, mcuInfo->robExp - tranum - 10);
		mcuInfo->luck = MAX(-100, mcuInfo->luck - 10);
//              mcuInfo->freeTime += (currtime + 10800);
		mcuInfo->freeTime = time(NULL) + CrimeTime[crime] * 3600;

		myInfo->luck = MIN(100, myInfo->luck + 10);
		del_from_file(DIR_MC "criminals_list", uident);
		prints("\n    Զ���������˹������㶨��һ����������Ҫץ��%s��"
		       "\n    ���������γ���ǹ���������������㱻���ˣ���"
		       "\n    %sĿ�ɿڴ���ֻ�����־��ܡ�"
		       "\n    ��ĵ�ʶ�����ˣ�	����������ˣ�"
		       "\n    ��������ľ��ˣ�"
		       "\n    �����Ʒ�����ˣ�"
		       "\n    û���ﷸ�����ֽ���Ϊ������", uident, uident);
		sprintf(title, "�����𡿾���ץ��ͨ����%s", uident);
		sprintf(buf,
			"��Ա%s�����غ򣬹��������ڰ�ͨ����%s���ش˹��棬��ʾ���á�\n\n"
			"��%s��ͥ�о�������%s%s�����������д�%dСʱ����ͽ�̡�",
			currentuser->userid, uident, CENTER_NAME, uident,
			CrimeName[crime], CrimeTime[crime]);
		deliverreport(title, buf);
		sprintf(buf,
			"�㱻��Ա%s���ץ��û�������ֽ𣬵�ʶ���½���\n"
			"������ͥ�о�%s���Ѻ%dСʱ��\n"
			"�����˳��Ĳ�Ϊ�ˣ�",
			currentuser->userid, CrimeName[crime],
			CrimeTime[crime]);
		if (mcuInfo->GetLetter == 1)
			system_mail_buf(buf, strlen(buf), uident,
					"�㱻����ץ��", currentuser->userid);
		unloadData(mcuInfo, sizeof (mcUserInfo));
	} else {
		myInfo->begExp = MAX(0, myInfo->begExp - tranum / 2);
		myInfo->robExp = MAX(0, myInfo->robExp - tranum / 2);
		myInfo->health = 0;
		mcuInfo->cash += myInfo->cash;
		myInfo->cash = 0;
		mcuInfo->robExp += tranum / 2;
		mcuInfo->begExp += tranum / 2;
		myInfo->freeTime = time(NULL) + CrimeTime[crime] * 1800 + 1800;
		prints("\n    Զ���������˹������㶨��һ����������Ҫץ��%s��"
		       "\n    ���������γ���ǹ���������������㱻���ˣ���"
		       "\n    ������ʱ�����ϱ������˴�����һ���ƹ���"
		       "\n    ��ĵ�ʶ�����ˣ�"
		       "\n    ����������ˣ�" "\n    ����˹�ȥ��", uident);
		sprintf(title, "�����𡿾�Ա%s����סԺ", currentuser->userid);
		sprintf(buf,
			"��Ա%s��ִ�������ʱ�򣬱��ﷸ����סԺ��Ŀǰ�����"
			"����%dСʱ�Ժ󼴿ɳ�Ժ��", currentuser->userid,
			(CrimeTime[crime] + 1) / 2);
		deliverreport(title, buf);
		sprintf(buf,
			"����%s���ץ�㣬�������ո��˵�����ɵ����Լ���Ȼ"
			"����\n ˳�������������ϵ��ֽ𣬵�ʶ��������̫ˬ��!",
			currentuser->userid);
		if (mcuInfo->GetLetter == 1)
			system_mail_buf(buf, strlen(buf), uident, "�����ѿ�",
					currentuser->userid);
		unloadData(mcuInfo, sizeof (mcUserInfo));
		unloadData(myInfo, sizeof (mcUserInfo));
		unloadData(mcEnv, sizeof (MC_Env));
		pressreturn();
		Q_Goodbye();
	}

	pressreturn();
//      Q_Goodbye();
	return;

}

static void
cop_tax()
{
	char uident[IDLEN + 1], buf[256], title[STRLEN];
	time_t currtime;
	mcUserInfo *mcuInfo;
	int num;

	nomoney_show_stat("˰��");

	if (clubtest(ROBUNION) || clubtest(BEGGAR)) {
		showAt(12, 4, "���޼���������˰��㣿", YEA);
		return;
	}

	if (myInfo->robExp < 500 || myInfo->begExp < 500) {
		showAt(12, 4,
		       "�¾���ɣ���ȥ��������������Ȼ���������ˡ����͹��������㲻���װ���",
		       YEA);
		return;
	}
	if (check_health(100, 12, 4, "��û�г����������ִ������", YEA))
		return;

	currtime = time(NULL);
	if (myInfo->WorkPoint < 9000) {
		showAt(12, 4, "��Ҫ�ż����ϼ���û���ж�ָʾ��", YEA);
		return;
	}

	move(4, 4);
	prints("���͵˰©˰���أ���Ҫ����������ֿ�˰�糱��");

	if (!getOkUser("\n��ѡ�����Ŀ�꣺", uident, 6, 4)) {
		move(8, 4);
		prints("���޴���");
		pressanykey();
		return;
	}
	if (!strcmp(uident, currentuser->userid)) {
		showAt(8, 4, "ţħ���������š��������񾭲�������", YEA);
		return;
	}

	myInfo->Actived += 10;
	move(10, 4);
	prints("���ÿ���%s�ļ��š�", uident);
	sleep(1);

	sethomefile(buf, uident, "mc.save");
	if (!file_exist(buf))
		initData(1, buf);
	if((mcuInfo = loadData(buf, sizeof (mcUserInfo))) == (void*)-1)
		return;

	if (mcuInfo->GetLetter == 0) {
		clear();
		showAt(6, 4, "������ҳ����ߣ��޷���˰", YEA);
		unloadData(mcuInfo, sizeof (mcUserInfo));
		return;
	}

	myInfo->lastActiveTime = currtime;
	myInfo->WorkPoint -= 9000;
	sleep(1);

	if (check_chance
	    (myInfo->robExp + myInfo->begExp, mcuInfo->robExp + mcuInfo->begExp,
	     myInfo->weapon, mcuInfo->armor, 200, 100)) {
		myInfo->begExp += 20;
		myInfo->robExp += 20;
		num = mcuInfo->credit / 3;
		myInfo->health = 0;
		mcuInfo->credit -= num;
		mcEnv->Treasury += num * 2 / 3;
		myInfo->credit += num / 3;

		myInfo->luck = MIN(100, myInfo->luck + 20);
		prints("\n    %s�����������е���ô��ͷ����"
		       "\n    ������ļල������������%d%s��Ƿ˰��"
		       "\n    ����л��1/3 �Ľ�������ĵ�ʶ��������", uident,
		       num, MONEY_NAME);
		sprintf(title, "��˰��%s����Ƿ˰", uident);
		sprintf(buf,
			"�ھ�Ա%s�ļල�£�%s���ڽ�������Ƿ��%d%s˰��ش˹��档",
			currentuser->userid, uident, num, MONEY_NAME);
		deliverreport(title, buf);
		sprintf(buf,
			"��Ա%s��ͬ˰��ǰ����˰�������Ƕ㲻��ȥ�ˡ�\n"
			"���ò�������%d%s��˰���ʹ����",
			currentuser->userid, num, MONEY_NAME);
		if (mcuInfo->GetLetter == 1)
			system_mail_buf(buf, strlen(buf), uident, "�����Ƿ˰",
					currentuser->userid);
	} else {
		myInfo->begExp = MAX(0, myInfo->begExp - 5);
		myInfo->robExp = MAX(0, myInfo->robExp - 5);
		myInfo->health = 0;
		mcuInfo->robExp += 10;
		mcuInfo->begExp += 10;
		prints("\n    %sһ�������¿�ˮ�̵����ӡ�"
		       "\n    ��ҪǮû�У�Ҫ��һ����"
		       "\n    ��Ӳ���ԣ����޼ƿ�ʹ�����ò��Ǳ���ȥ��", uident);
	}
	unloadData(mcuInfo, sizeof (mcUserInfo));
	pressanykey();
	return;
}

static void
SearchCrime()
{
	char uident[IDLEN + 1], buf[256];
	time_t currTime, ActiveTime;
	mcUserInfo *mcuInfo;
	int day, hour, minute;

	nomoney_show_stat("������");
	if (!getOkUser("\n��ѡ���ѯ�ĵ�����", uident, 6, 4)) {
		move(8, 4);
		prints("���޴���");
		pressanykey();
		return;
	}

	sethomefile(buf, uident, "mc.save");
	if (!file_exist(buf))
		initData(1, buf);
	if((mcuInfo = loadData(buf, sizeof (mcUserInfo))) == (void*)-1)
		return;

	ActiveTime = mcuInfo->lastActiveTime;
	currTime = time(NULL);
	if (currTime > ActiveTime) {
		day = (currTime - ActiveTime) / 86400;
		hour = (currTime - ActiveTime) % 86400 / 3600;
		minute = (currTime - ActiveTime) % 3600 / 60 + 1;
		move(9, 1);
		prints("    �����ϴη�������%d��%dСʱ%d������ǰ��", day, hour,
		       minute);
	}
	unloadData(mcuInfo, sizeof (mcUserInfo));
	pressanykey();
	return;
}

static int
cop_police()
{
	int ch, quit = 0;

	while (!quit) {
		nomoney_show_stat("�̾���");

		if (!seek_in_file(DIR_MC "policemen", currentuser->userid)) {
			showAt(12, 4,
			       "����ִ�����������˵���رܣ��������ˡ�", YEA);
			return 0;
		}
		move(t_lines - 1, 0);
		prints
		    ("\033[1;44m ѡ�� \033[1;46m [1]���� [2]��˰ [3]�ƻ���ľ [Q]�뿪\033[m");
		ch = igetkey();
		switch (ch) {
		case '1':
			cop_Arrest();
			break;
		case '2':
			cop_tax();
			break;
		case '3':
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
money_cop()
{
	int ch, quit = 0;
	char uident[IDLEN + 1], buf[256];

	if (!(myInfo->GetLetter == 1)) {
		clear();
		showAt(5, 4, "���Ѿ��ر��˽���������Ϸ���ܣ��뿪����������",
		       YEA);
		return 0;
	}

	while (!quit) {
		sprintf(buf, "%s����", CENTER_NAME);
		nomoney_show_stat(buf);
		move(8, 16);
		prints("������ά���ΰ���");
		move(10, 4);
		prints("���ڷ���������ӣ��̾���Ҳ��ʼ���Լ�ҹ�ļӰࡣ");
		move(t_lines - 1, 0);
		prints("\033[1;44m ѡ�� \033[1;46m [1]���� [2]ͨ����"
		       " [3]�̾��� [4]�𳤰칫�� [Q]�뿪\033[m");
		ch = igetkey();
		switch (ch) {
		case '1':
			nomoney_show_stat("����Ӵ���");
			cop_accuse();
			break;
		case '2':
			clear();
			move(1, 0);
			prints("%s����ǰͨ���ķ���������:", CENTER_NAME);
			listfilecontent(DIR_MC "criminals_list");
			FreeNameList();
			pressanykey();
			break;
		case '3':
			cop_police();
			break;
		case '4':
			nomoney_show_stat("�𳤰칫��");
			whoTakeCharge(8, uident);
			if (strcmp(currentuser->userid, uident))
				break;
			showAt(6, 0, "    ��ѡ���������:\n"
			       "        1. ������Ա         2. ��ְ��Ա\n"
			       "        3. ��Ա����         4. �����ﷸ\n"
			       "        5. ��ѯ             Q. �˳�", NA);
			ch = igetkey();
			switch (ch) {
			case '1':
			case '2':
				cop_arrange(ch - '0');
				break;
			case '3':
				clear();
				move(1, 0);
				prints("Ŀǰ%s����Ա������", CENTER_NAME);
				listfilecontent(DIR_MC "policemen");
				FreeNameList();
				pressanykey();
				break;
			case '4':
				break;
			case '5':
				SearchCrime();
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

#include "mc.h"

static void
Beg_Med()
{
	char buf[256], uident[IDLEN + 1];
	time_t currtime;
	mcUserInfo *mcuInfo;
	int num;

	nomoney_show_stat("��ɽ��ˮ");

	if (check_health
	    (100, 12, 4, "�����ܺķѾ����ģ�Ҫ׼����ֲ��ܿ�ʼ��", YEA))
		return;
	if ((myInfo->robExp < 500) || (myInfo->begExp < 1000)) {
		showAt(12, 4, "������ˮƽҲ������������ ��������ʺ�����",
		       YEA);
		return;
	}

	if (myInfo->luck < 80) {
		showAt(12, 4, "������Ҫƽ�ľ�����", YEA);
		return;
	}

	currtime = time(NULL);
	if (myInfo->WorkPoint < 9000) {
		showAt(12, 4, "æ����ûʱ�侲����������", YEA);
		return;
	}

	move(4, 1);
	if (askyn("������Ҫ������ϡҩ�ģ��ܹ���Ҫ50����ȷ��Ҫ����", NA, NA)
	    == NA) {
		showAt(12, 4, "��������ǲ����ˣ�̫���ˡ�", YEA);
		return;
	}
	if (myInfo->cash < 500000) {
		showAt(12, 4, "һ�ֽ�Ǯһ�ֽ�������û���㹻���ֽ�", YEA);
		return;
	}

	myInfo->lastActiveTime = currtime;
	myInfo->WorkPoint -= 9000;
	myInfo->health -= 50;
	myInfo->Actived += 20;
	myInfo->cash -= 500000;
	mcEnv->prize777 += after_tax(500000);

	move(6, 4);
	prints("������ʮ�����ȥ�ˣ����쿪¯���ɰ��ڴ�һ�٣�");
	pressanykey();

	if (random() % 2) {
		move(7, 4);
		prints("��¯һ����һ�ɶ�������������֮������Ż��"
		       "\n    һ¯���ҩ�ݱ���̣��,��ѧ���ˡ�");
		myInfo->robExp += 5;
		myInfo->begExp += 5;
		move(9, 4);
		prints("������������飬��ʶ��������");
		sprintf(buf,
			"    ���������羰������������Ⱦ��\n"
			"    ������֯���ڵ�����Ⱦ��Դ��\n");
		deliverreport("�����š����������̲��ݻ�", buf);
		pressanykey();
		return;
	}

	myInfo->health -= 50;
	move(9, 4);
	prints("��¯һ����һ�����������𣬶��յ������������˻ر���");

	move(10, 4);
	if (askyn("������Լ�����(Y)���ǳ���(N)��", NA, NA) == YEA) {
		showAt(11, 4, "����ɵ����ܱ����˱��ˣ���Ȼ���Լ������ˣ�",
		       YEA);
		if (check_chance
		    (myInfo->robExp, 2500, myInfo->weapon, 7, 50, 0)) {
			move(12, 4);
			prints("�ɵ�Ч��������ֻ����һ�������Զ����롣"
			       "\n    ��ĵ�ʶ����������125�㡣!�������ȫ��!");
			myInfo->robExp += 125;
			myInfo->begExp += 125;
			myInfo->health = 100 + 2 * myInfo->con;
			sprintf(buf,
				"    ���ձ��и������ξ����ڳ������쾰�������羰���������˱ǣ���������������");
			deliverreport("�����š����գ�����", buf);
			pressanykey();
			return;
		} else {
			move(12, 4);
			prints("����֮�������������ᵽ���������Եĺ��塣"
			       "\n    ��һ����ª�ı����������ڸ�������������¡�"
			       "\n    �ڶ�����ũ�ϵ����ԣ�����ж�������"
			       "\n    ��ĵ�ʶ�����½�20�㣡��������ľ����㲻�ò�סԺ������");
			myInfo->robExp -= 20;
			myInfo->begExp -= 20;
			myInfo->health = 0;
			myInfo->WorkPoint -= 3600;
			sprintf(buf,
				"    ���շ���һ���ؽ����ж����ߣ�ר�����ѹ��ڣ��벻Ҫ��ʳ������������Ʒ��");
			deliverreport("�����š���ע����ʳ����", buf);
			pressanykey();
			return;
		}
	} else {
		if (!getOkUser("�������˭ȥ��С����", uident, 12, 4)) {
			showAt(13, 4, "���޴���", YEA);
			return;
		}

		if (!strcmp(uident, currentuser->userid)) {
			showAt(13, 4, "ţħ���������š��������񾭲�������",
			       YEA);
			return;
		}

		sethomefile(buf, uident, "mc.save");
		if (!file_exist(buf))
			initData(1, buf);
		if((mcuInfo = loadData(buf, sizeof (mcUserInfo))) == (void*)-1)
			return;

		if (mcuInfo->GetLetter == 0) {
			clear();
			showAt(6, 4, "�˼Ҳ�Ը������棬�ǲ��𣬶㻹���а���",
			       YEA);
			unloadData(mcuInfo, sizeof (mcUserInfo));
			return;
		}
		if (check_chance
		    (myInfo->begExp, mcuInfo->begExp, myInfo->weapon,
		     mcuInfo->armor, 200, 0)) {
			num = MIN(myInfo->credit, mcuInfo->credit / 4);
			mcuInfo->credit -= num;
			myInfo->cash += num;
			myInfo->robExp += 30;
			myInfo->begExp += 30;
			myInfo->luck -= 30;
			mcuInfo->robExp = MAX(0, mcuInfo->robExp - 10);
			mcuInfo->begExp = MAX(0, mcuInfo->begExp - 10);

			move(14, 4);
			prints
			    ("���������֮���һ��Ĵ���%sˬ����ͳ���%d%s����������ɵ���"
			     "\n    ��ĵ�ʶ����������30�㡣�����Ʒ�½���30�㡣",
			     uident, num, MONEY_NAME);
			sprintf(buf,
				"    ���շ���ĳЩ������ݵ����������۱���ҩƷ�����������������");
			deliverreport("�����š���ҩƭ�Ӻ���", buf);
			sprintf(buf,
				"�㻨%d%s��%s��������һ����ν���ɵ�������ȥ�Ժ�������к��",
				num, MONEY_NAME, currentuser->userid);
			if (mcuInfo->GetLetter == 1)
				system_mail_buf(buf, strlen(buf), uident,
						"����˼�ҩ",
						currentuser->userid);
		} else {
			move(14, 4);
			prints("��������ιĴ���%s���ǲ�����Ǯ���ز�ū��",
			       uident);
		}
	}
	unloadData(mcuInfo, sizeof (mcUserInfo));
	pressanykey();
}

static void
Beg_Gov()
{

	time_t currtime;
	char buf[256], title[STRLEN];
	int i = 1, num;
	char *Work[] = { "��", "��", "ʡ", "����", NULL };

	nomoney_show_stat("��ѵ����");

	if (check_health
	    (140, 12, 4, "Ϊ���������һ����������飬�����۵ġ�", YEA))
		return;

	if ((myInfo->robExp < 10000) || (myInfo->begExp < 20000)) {
		showAt(12, 4,
		       "��������Ҫ�²ż汸���㻹����Ŭ������Լ����Ļ�ˮƽ��",
		       YEA);
		return;
	}

	if (myInfo->luck < 90) {
		showAt(12, 4,
		       "��������Ҫ�²ż汸���㻹����Ŭ������Լ��ĵ���������",
		       YEA);
		return;
	}

	if (myInfo->cash < 1000000) {
		showAt(12, 4, "������ɬ�����ڼ��˰�", YEA);
		return;
	}

	currtime = time(NULL);
	if (myInfo->WorkPoint < 18000) {
		showAt(12, 4, "ͨ���ûȡ�����ⲻ�ǳ�ȥ��Ͷ������", YEA);
		return;
	}

	nomoney_show_stat("����");
	move(4, 4);

	if (askyn("����Ա���Ա�����100����ȷ��Ҫ������", NA, NA) == NA) {
		showAt(12, 4, "\n    ̫���ˣ�����������ˡ�", YEA);
		return;
	}

	myInfo->lastActiveTime = currtime;
	myInfo->WorkPoint -= 18000;
	myInfo->health = 0;
	myInfo->Actived += 30;
	myInfo->cash -= 1000000;

	move(6, 4);
	prints("\n    %s����Ա���ԣ���������ʮ���������档", CENTER_NAME);
	sleep(1);
	prints
	    ("\n		��Ŀ���Ѱ���3����7���ڶ����أ� �������߶�ʮһ��д��25��ȥ��˵��");
	pressanykey();

	move(9, 4);
	prints("\n    ���ڱ�������׼����С����̫����ˣ�");
	prints("\n    ��˵�еĸ߿Ƽ����ײ������������Ƽ����ǵ�һ������");
	pressanykey();

	if (!(random() % 8)) {
		move(13, 4);
		prints
		    ("\n		û�������ⶼ�ܱ����֡�\n		�㱻�ϳ��˿����������Ʒ�½�");
		myInfo->luck -= 20;
		sprintf(title, "�����š� ����Ա���Ծ������׳���");
		sprintf(buf,
			"    %s�ڹ���Ա��������ͼ���ף����࿼��Ա�������֡�\n",
			currentuser->userid);
		deliverreport(title, buf);
		pressanykey();
		return;
	}

	while (i < 5) {
		nomoney_show_stat("�칫��");
		sprintf(buf,
			"�������Ѿ���%s���ɲ���,�����ǽ�Ϲ��š�Ϊ������񡱡�",
			Work[i - 1]);
		showAt(8, 4, buf, YEA);
		move(10, 4);
		if (askyn("�ܸ����䣬һ��һ��Ϊ�������?", YEA, NA) == YEA) {
			if (check_chance
			    (myInfo->begExp, 18000 + i * 4000, myInfo->weapon,
			     12 + i, 200 + i * 200, 0)) {
				move(12, 4);
				prints
				    ("\n		��ȫ��ȫ��Ϊ���������ף������л�㡣"
				     "\n		��ĵ�ʶ��������");
				myInfo->robExp += i * 25;
				myInfo->begExp += i * 25;
				i++;
				pressanykey();
			} else {
				move(12, 4);
				num =
				    MAX(MIN
					(i * 1000000,
					 mcEnv->Treasury - 20000000), 0);
				prints
				    ("\n		�ù��ҹ�����һ�����ײһ����"
				     "\n		�޹��޹��������ڻ쵽�˷����ʵ�����"
				     "\n		�����˹���%d%s",
				     num, MONEY_NAME);
				myInfo->cash += num;
				mcEnv->Treasury -= num;
				sprintf(title, "�������� %s����",
					currentuser->userid);
				sprintf(buf,
					"    %s�����ԭ���ְ�������������Ĺ��ױ�ʾ��л��\n",
					currentuser->userid);
				deliverreport(title, buf);
				pressanykey();
				return;
			}
		} else {
			showAt(12, 4, "��Ǯ���ջ�̫���ˣ��㻹�Ǿ������֡�",
			       YEA);
			if (check_chance
			    (myInfo->begExp, 18000 + i * 4000, myInfo->armor,
			     12 + i, 200 + i * 200, 0)) {
				move(13, 4);
				num =
				    MAX(MIN
					(i * 3000000,
					 mcEnv->Treasury - 20000000), 0);
				prints
				    ("\n		���̰ײ��̣�����һ��ְȨ�������ܻ�¸��"
				     "\n		��Ȩ���ã��������ϡ�����û�˷��֣����˾��ܰɡ�"
				     "\n		��ĵ�ʶ������!������%d%s�Ĳ���֮��!",
				     num, MONEY_NAME);
				myInfo->robExp += i * 40;
				myInfo->begExp += i * 40;
				myInfo->cash += num;
				mcEnv->Treasury -= num;
				sprintf(title, "�����š� �����߲㾪��̰�۴�");
				sprintf(buf,
					"    ��Ϥ��%s��ְ�ڼ䣬̰�۸ߴ�%d%s��"
					"\n		%s����Я��Ǳ�ӣ����������������Ѳ�",
					currentuser->userid, num,
					MONEY_NAME, currentuser->userid);
				deliverreport(title, buf);
				pressanykey();
				return;
			} else {
				move(13, 4);
				prints
				    ("\n		�����ֻ֣������©��Ī���֣����ֱر�׽��"
				     "\n		�㲻����û�������зǷ����룬����������ְ��"
				     "\n		��ĵ�ʶ���½�!�����Ʒ�½�!");
				myInfo->robExp -= i * 10;
				myInfo->begExp -= i * 10;
				myInfo->luck -= i * 20;
				myInfo->freeTime = time(NULL) + i * 3600;
				sprintf(title, "�����š� �����ֻ֣������©");
				sprintf(buf,
					"    ������%s���Ҿ����������̰��������%dСʱ��",
					currentuser->userid, i);
				deliverreport(title, buf);
				unloadData(myInfo, sizeof (mcUserInfo));
				unloadData(mcEnv, sizeof (MC_Env));
				pressreturn();
				Q_Goodbye();
			}
		}
	}
	nomoney_show_stat("���û�");
	move(8, 4);
	num = MAX(MIN(10000000, mcEnv->Treasury - 20000000), 0);
	prints("\n		��Ϊ���������˽ܳ��Ĺ��ף����񲻻�������ġ�"
	       "\n		������Ը����%d%s���м��㡣", num, MONEY_NAME);
	myInfo->credit += num;
	mcEnv->Treasury -= num;
	sprintf(title, "�����š� ����ĺù���%s", currentuser->userid);
	sprintf(buf, "    %sͬ־����ְ�ڼ䣬���ľ�ְ��ȫ��ȫ��Ϊ����ı������"
		"\n		��������ӵ�������ĺøɲ���",
		currentuser->userid);
	deliverreport(title, buf);
	pressanykey();
	return;
}

int
money_beggar()
{
	char ch, quit = 0, uident[IDLEN + 1];
	void *stockMem;
	int tempMoney, day, hour, minute, i, ns, worth;
	mcUserInfo *mcuInfo;
	time_t ActiveTime, currTime;
	struct BoardStock *bs;
	size_t filesize;
	char *FreeWeapon[] =
	    { "ľ��", "�˵�", "ذ��", "����", "��潣", "�㳦��", "��Ȫ��",
		"���ǽ�", "������", "�޳���", "����", "�ɽ�Īа", "��������",
		    "�Ľ�", "�콣",
		"�޽�", NULL
	};
	char *FreeArmor[] =
	    { "����", "����", "����", "ҹ����", "��ͭ��", "������", "��˿��",
		"��⬼�", "��ʦ����", "����ս��", "�׻�����", "��ȸս��",
		    "����ս��",
		"��������", "ʥ������", "�߲�������", NULL
	};
	char *RobWeapon[] =
	    { "ˮǹ", "�������ǹ", "����ǹ", "��ҩǹ", "��ǹ", "��ǹ",
		"�Զ���ǹ", "���ǹ", "�ѻ�ǹ", "���ǹ", "�ػ�ǹ", "���Ͳ",
		    "�罬ǹ",
		"����ǹ", "������", "������", NULL
	};
	char *RobArmor[] =
	    { "Ӥ����", "���ֳ�", "���ֳ�", "ƽ�峵", "���г�", "��ƿ��",
		"Ħ�г�", "����", "����", "����", "����", "����", "����",
		    "����", "�ֿ�",
		"������", NULL
	};
	char *BegWeapon[] =
	    { "���ɶ���", "���־�", "�»��ֵ�", "���Ի���", "���ݽṹ",
		"Basic����", "Pascal����", "���˼��", "C����", "����Э��",
		    "VC�̳�",
		"Javaǳ��", "����ԭ��", "�������", "��������", "ͼ���", NULL
	};
	char *BegArmor[] =
	    { "����è", "�䵱��ȭ", "�޺�ȭ", "������", "������", "����ȭ",
		"����", "���������", "��ڤ����", "̫��ȭ", "��Ȼ������",
		    "����ʮ����",
		"�׽", "ϴ�辭", "������", "�����澭", NULL
	};

	if (!(myInfo->GetLetter == 1)) {
		clear();
		showAt(5, 4, "���Ѿ��ر��˽���������Ϸ���ܣ��뿪����������",
		       YEA);
		return 0;
	}

	while (!quit) {
		money_show_stat("ؤ���ܶ�");
		showAt(4, 4,
		       "ؤ���Թ����µ�һ������ƶ�����Խ��Խ������ؤ����Ҳ�����ˡ�\n"
		       "    Ϊ�����ƣ��ⲻ����͵���������ɹ�ƭ֮�£���ȻҲ�нٸ���ƶ֮�١�\n\n"
		       "    һ����ؤ�߹����ʵ�����Ҫ������Ϣô��ؤ�����ϵ���������֪��������������",
		       NA);
		move(t_lines - 1, 0);
#if 0
		prints
		    ("\033[1;44m ѡ�� \033[1;46m [1]��̽ [2]��ë�� [3]ؤ�� [4]�������� [Q]�뿪\033[m");
#else
		prints
		    ("\033[1;44m ѡ�� \033[1;46m [1]��̽ [2]ؤ�� [3]�������� [4]�ȼ����� [Q]�뿪\033[m");
#endif
		ch = igetkey();

		switch (ch) {
		case '1':
			update_health();
			if (check_health
			    (2, 12, 4, "Ъ��Ъ�ᣬ��־����Ͳ�Ҫ���ˣ�", YEA))
				break;
			if (!getOkUser("��˭�ļҵף�", uident, 8, 4))
				break;
			if (myInfo->cash < 1000) {
				showAt(9, 4, "������ֻ������ô��Ǯ��", YEA);
				break;
			}
			myInfo->cash -= 1000;
			mcEnv->Treasury += 1000;
			update_health();
			myInfo->health -= 2;
			myInfo->Actived++;
			sethomefile(genbuf, uident, "mc.save");
			if (!file_exist(genbuf))
				initData(1, genbuf);
			if((mcuInfo = loadData(genbuf, sizeof (mcUserInfo))) == (void*)-1)
				break;

			//�����Ʊ��ֵ
			worth = 0;
			if (!file_exist(DIR_STOCK "stock"))
				initData(2, DIR_STOCK "stock");
			ns = get_num_records(DIR_STOCK "stock",
					     sizeof (struct BoardStock));
			if (ns <= 0)
				goto SHOW;
			filesize = sizeof (struct BoardStock) * ns;
			//���ع�����Ϣ 
			if ((stockMem = loadData(DIR_STOCK "stock", filesize)) == (void*)-1)
				goto SHOW;

			for (i = 0; i < STOCK_NUM; i++) {
				if (mcuInfo->stock[i].num <= 0)
					continue;
				bs = stockMem +
				    mcuInfo->stock[i].stockid *
				    sizeof (struct BoardStock);
				worth +=
				    bs->todayPrice[3] * mcuInfo->stock[i].num;
			}
			unloadData(stockMem, filesize);

		      SHOW:
			move(10, 4);
			if (mcuInfo->GetLetter == 0)
				prints("\033[1;31m%s\033[m", uident);
			else
				prints("\033[1;32m%s\033[m", uident);
			prints
			    (" ��Լ�� \033[1;33m%d\033[m ���ֽ� \033[1;33m%d\033[m �Ĵ� �Լ�\033[1;33m%d\033[m �Ĺ�Ʊ",
			     mcuInfo->cash / 5000 * 5000,
			     mcuInfo->credit / 10000 * 10000,
			     worth / 10000 * 10000);
			move(11, 4);
			if (mcuInfo->GetLetter == 0)
				prints("\033[1;31m%s\033[m ", uident);
			else
				prints("\033[1;32m%s\033[m ", uident);
			prints
			    ("�ĵ�ʶ \033[1;33m%d\033[m  �� \033[1;33m%d\033[m  ��Ʒ \033[1;33m%d\033[m  ���� \033[1;33m%d\033[m ���� \033[1;33m%d\033[m  ���ǹ� \033[1;33m%d\033[m ��\n",
			     mcuInfo->robExp, mcuInfo->begExp, mcuInfo->luck,
			     mcuInfo->con, MAX(0, mcuInfo->health),
			     mcuInfo->guard);
			move(12, 4);
			if (mcuInfo->GetLetter == 0)
				prints("\033[1;31m%s\033[m ", uident);
			else
				prints("\033[1;32m%s\033[m ", uident);
			switch (mcuInfo->BeIn) {
			case 0:
				prints
				    ("�ֳ�\033[1;33m%s\033[m, ��\033[1;33m%s\033[m, ",
				     FreeWeapon[mcuInfo->weapon],
				     FreeArmor[mcuInfo->armor]);
				if (seek_in_file(DIR_MC "policemen", uident))
					prints("������һ������");
				else
					prints("��������������");
				break;
			case 1:
				prints
				    ("����\033[1;33m%s\033[m, ��ʻ\033[1;33m%s\033[m, Ŭ���Գ����׼��������",
				     RobWeapon[mcuInfo->weapon],
				     RobArmor[mcuInfo->armor]);
				break;
			case 2:
				prints
				    ("ѧϰ��\033[1;33m%s\033[m, ������\033[1;33m%s\033[m, �������˲�¶�డ",
				     BegWeapon[mcuInfo->weapon],
				     BegArmor[mcuInfo->armor]);
				break;
			}
			unloadData(mcuInfo, sizeof (mcUserInfo));
			pressanykey();
			break;
#if 0
		case '2':
			money_show_stat("������");
			move(6, 4);
			prints
			    ("�в������ⲻ�����˰������ۺ졣��������ؤ����Ϣ��ͨ��\n"
			     "Ԥ��һ�ʼ�ë���ʷѣ����˰������ʱ�򣬾��ܻ�֪��ë�ţ�\n"
			     "ʹ�ð�Ȼ���ļ������ӡ�");
			showAt(12, 4, "\033[1;32m���ڲ߻��С�\033[m", YEA);
			break;
#endif
		case '2':
			if (!clubtest(BEGGAR)) {
				move(12, 4);
				prints("���¹���������ô����ؤ��������");
				pressanykey();
				break;
			}
			if (clubtest(ROBUNION) && clubtest(BEGGAR)) {
				move(12, 4);
				prints
				    ("�����ˣ��ꡫ����ڰ��ؤ��ˮ���ݣ����̤��ֻ�������ǲ�Ҫ��¶Ϊ�á�");
				pressanykey();
				break;
			}
			if (!(myInfo->BeIn == 2)) {
				myInfo->weapon = 0;
				myInfo->armor = 0;
				myInfo->BeIn = 2;
			}
			while (!quit) {
				ActiveTime = myInfo->lastActiveTime;
				currTime = time(NULL);
				money_show_stat("ؤ��");
				if (currTime > ActiveTime) {
					day = (currTime - ActiveTime) / 86400;
					hour =
					    (currTime -
					     ActiveTime) % 86400 / 3600;
					minute =
					    (currTime -
					     ActiveTime) % 3600 / 60 + 1;
					move(9, 1);
					prints
					    ("    ����ǽ�Ͽ̵ļǺţ����ϴθɻ����Ѿ���%d��%dСʱ%d������ǰ�������ˡ�\n\n"
					     "    ��Ϊؤ���Ա��͵����������ˮ���������������ְ������ô����ô��˼��ȡ�أ�",
					     day, hour, minute);
				} else {
					move(9, 1);
					prints
					    ("    �����ϴ�ؤ���У���û�ͷ����������ֻ�����ڴ��Ϻߺ�����");
				}
				move(13, 1);
				prints("    ������ض�\033[1;31m  %d\033[m %%",
				       (30000 - myInfo->WorkPoint) / 300);
				move(15, 1);
				prints
				    ("    ���Ѿ�ѧ����\033[1;33m%s\033[m, ����������\033[1;33m%s\033[m, ���а��Ե��⡣",
				     BegWeapon[myInfo->weapon],
				     BegArmor[myInfo->armor]);
				move(t_lines - 2, 0);
				prints
				    ("\033[1;44m ѡ�� \033[1;46m [1]���� [2]���ֿտ� [3] �������  [4]�������� [5]���Ǵ� [6]����    \033[m");
				move(t_lines - 1, 0);
				prints
				    ("\033[1;44m      \033[1;46m [9]���� [Q]�뿪                                                      \033[m");
				ch = igetkey();
				switch (ch) {
				case '1':
					money_show_stat("������");
					forceGetMoney(1);
					break;
				case '2':
					money_show_stat("��ҵ��");
					forceGetMoney(3);
					break;
				case '3':
					money_show_stat("�ڿ͵۹�");
					stealbank();
					break;
				case '4':
					Beg_Med();
					break;
				case '5':
					money_show_stat("С����");
					RobPeople(1);
					//showAt(12, 4,"\033[1;32m��Ƥ�ӹ��򲻵��ң�˵���˼�Ҳ������\033[m",YEA);
					//randomGetMoney(1); 
					break;
				case '6':
					money_show_stat("������¥");
					Beg_Gov();
					//showAt(12, 4,"\033[1;32m�������������������\033[m",YEA);
					//randomGetMoney(1); 
					break;
				case '9':
					EquipShop(2);
					break;
				case 'q':
				case 'Q':
					quit = 1;
					break;
				}
			}
			quit = 0;
			break;
		case '3':
			whoTakeCharge(5, uident);
			if (strcmp(currentuser->userid, uident))
				break;
			while (!quit) {
				nomoney_show_stat("ؤ�����");
				move(t_lines - 1, 0);
				prints
				    ("\033[1;44m ѡ�� \033[1;46m [1]��ƶ�� [2]�򹷰��� [3]�鿴�����ʲ� [Q]�뿪\033[m");
				ch = igetkey();
				switch (ch) {
				case '1':
					nomoney_show_stat("������");
					showAt(12, 4,
					       "\033[1;32m���ڽ����С�\033[m",
					       YEA);
					break;
				case '2':
					money_show_stat("�����޹�");
					showAt(12, 4,
					       "\033[1;32m��ϰ�С�\033[m", YEA);
//forcerobMoney(2); 
					break;
				case '3':
					money_show_stat("С���");
					showAt(12, 4,
					       "\033[1;32m������ڵ�Ǯ�����Ժ�\033[m",
					       YEA);
					break;
				case 'q':
				case 'Q':
					quit = 1;
					break;
				}
			}
			quit = 0;
			break;
		case '4':
			money_show_stat("����");
			move(12, 4);
			if (askyn
			    ("\033[1;33m���´󺵣��������ա���Ҫ��Ҫ�ȼ�һ�����ˣ�\033[m",
			     YEA, NA) == YEA) {
				tempMoney =
				    userInputValue(13, 4, "Ҫ����", "��", 5,
						   100) * 10000;
				if (tempMoney < 0)
					break;
				if (myInfo->cash < tempMoney) {
					showAt(15, 4,
					       "\033[1;37mûǮ����ʲô���ˡ�\033[m",
					       YEA);
					break;
				}
				update_health();
				if (check_health
				    (1, 15, 4, "������������ˣ�", YEA))
					break;
				move(15, 4);
				prints("\033[1;37m������˷���%d%s����ʳ\n"
				       "    ������ƺ�������ߣ�\033[m",
				       tempMoney, MONEY_NAME);
				myInfo->health--;
				myInfo->Actived += tempMoney / 25000;
				myInfo->cash -= tempMoney;
				myInfo->begExp +=
				    (tempMoney - 30000) / (30000 +
							   (random() % 30000)) +
				    1;
				mcEnv->Treasury += tempMoney;
				pressanykey();
			} else {
				showAt(14, 4,
				       "\033[1;32m����̨��Ҫ��ô��Ǯ��ʲô�ã�\033[m",
				       YEA);
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

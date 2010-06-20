#include "mc.h"

void
Rob_Fight()
{
	char buf[256], title[STRLEN], uident[IDLEN + 1];
	time_t currtime;
	mcUserInfo *mcuInfo;
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

	nomoney_show_stat("�ڰ��ܲ�");

	if (check_health
	    (110, 12, 4, "����ȥ��ܣ��㵱���ݼٰ����ú���Ϣһ�¡�", YEA))
		return;
	if (myInfo->robExp < 5000) {
		showAt(12, 4, "��˵���Ҫ��Ѫ���ÿ��£��ÿ���", YEA);
		return;
	}
	if (myInfo->begExp < 3000) {
		showAt(12, 4, "����ô���ִ��壬��ʱ��򲻹���Ҳ�ܲ��ˡ�", YEA);
		return;
	}
	if (myInfo->luck < 85) {
		showAt(12, 4, "���αװ�����ã��𲻵����䲻���Ч��", YEA);
		return;
	}

	currtime = time(NULL);
	if (myInfo->WorkPoint < 12000) {
		showAt(12, 4, "���춢��̫����ûʱ��ɻ�", YEA);
		return;
	}

	if (!getOkUser("��׼����˭���֣�", uident, 5, 4)) {
		showAt(7, 4, "���޴���", YEA);
		return;
	}

	if (!strcmp(uident, currentuser->userid)) {
		showAt(7, 4, "�Լ�Ź�Լ������������Ⱑ", YEA);
		return;
	}

	sethomefile(buf, uident, "mc.save");
	if (!file_exist(buf))
		initData(1, buf);
	if((mcuInfo = loadData(buf, sizeof (mcUserInfo))) == (void*)-1)
		return;
	if (mcuInfo->GetLetter == 0) {
		showAt(7, 4, "�˼Ҳ�Ը������棬�ǲ��𣬶㻹���а���", YEA);
		unloadData(mcuInfo, sizeof (mcUserInfo));
		return;
	}

	if (mcuInfo->BeIn == 1) {
		showAt(7, 4, "��������Ͻ��ڶ�", YEA);
		unloadData(mcuInfo, sizeof (mcUserInfo));
		return;
	}

	if (!(seek_in_file(DIR_MC "policemen", uident) || (mcuInfo->BeIn == 2))) {
		showAt(7, 4, "�г𱨳𣬺ڰ��Ա��Թ������", YEA);
		unloadData(mcuInfo, sizeof (mcUserInfo));
		return;
	}

	if (mcuInfo->robExp + mcuInfo->begExp < 6000) {
		showAt(7, 4, "��һ�̫���ˣ�����һ��ɾ͸�û�У���м", YEA);
		unloadData(mcuInfo, sizeof (mcUserInfo));
		return;
	}

	move(6, 4);
	if (askyn("��ȷ������С����", NA, NA) == NA) {
		showAt(7, 4, "��û��׼Ŀ�ꣿ�´���˵��", YEA);
		unloadData(mcuInfo, sizeof (mcUserInfo));
		return;
	}

	myInfo->lastActiveTime = currtime;
	myInfo->WorkPoint -= 12000;
	myInfo->health -= 80;
	myInfo->Actived += 30;

	nomoney_show_stat("��������");

	move(6, 4);
	prints
	    ("�㿪���İ���%s,�ս����е�%s,���в�����һЩ����",
	     RobArmor[myInfo->armor], RobWeapon[myInfo->weapon]);
	pressanykey();

	move(7, 4);
	prints("���������鱨����һ�ס������·63�š�");
	move(8, 4);
	prints("��ϵ�����ʱ����Χһ�㾯�䶼û�У���һ�������ͷ�ˣ�");
	sleep(1);

	if (random() % 2) {
		move(9, 4);
		prints
		    ("���ó�%s��񿪻���Χ��ʱ����һƬ�𺣣�����ë��û�ܳ�����",
		     RobWeapon[myInfo->weapon]);
		move(10, 4);
		prints("������Ĵ�Ц���飬��ȫ�������Աߵ�·�ƣ���ʿ·......");
		myInfo->robExp += 10;
		myInfo->begExp += 10;
		myInfo->luck -= 80;
		move(12, 4);
		prints("��ĵ�ʶ��������\n    �����Ʒ����Ƚ���!");
		sprintf(buf,
			"    ��ʿ·�ⲻ����ݵĿֲ�����Ϯ�����޹�ƽ���������ˣ�\n"
			"    ��������ǿ��Ǵ�����ֲֿ������ж�!\n");
		deliverreport("�����š��������ֲܿ�Ϯ��", buf);
		unloadData(mcuInfo, sizeof (mcUserInfo));
		pressanykey();
		return;
	}

	move(9, 4);
	prints("���ó�%s��񿪻���Χ��ʱ����һƬ�𺣡�",
	       RobWeapon[myInfo->weapon]);
	prints("\n    %s�����Ǳ��ĳ��˳�����һ�������ʱҧ���гݵĳ���������",
	       uident);
	prints
	    ("\n	  ����Ĺ������ܹ��ֵ��߿Ƽ����ػ���� �����Ͼ�Ҫ������!");
	pressanykey();

	if (check_chance
	    (myInfo->robExp, mcuInfo->robExp, myInfo->weapon, mcuInfo->armor,
	     300, 50)) {
		myInfo->begExp += 150;
		myInfo->robExp += 150;
		mcuInfo->begExp -= mcuInfo->begExp / 50;
		mcuInfo->robExp -= mcuInfo->robExp / 50;
		myInfo->luck -= 50;
		myInfo->health = 0;
		mcuInfo->health = 0;

		move(13, 4);
		prints("���侲�Ķ�׼%s��������Ư���ĵ��䡣", uident);
		prints
		    ("\n    Ѫ��֮���Ͼ����ֵܵ��߿Ƽ���������%s�޷��������ò��Ǳ���ȥ��",
		     uident);
		prints
		    ("\n	  ��Ȼ����úܿ죬��������û���ӵ��찡����ε��˹�����Ϣһ��ʱ����ˡ�");
		if (!(random() % 100)) {
			if (random() % 2) {
				mcuInfo->weapon--;
				prints
				    ("\n		�Է������ȼ��½�������");
			} else {
				mcuInfo->armor--;
				prints
				    ("\n		�Է����ߵȼ��½�������");
			}
		}

		prints
		    ("\n    ��ĵ�ʶ��������150��!\n    %s�ĵ�ʶ���½�2%%��\n    �����Ʒ�½�30�㡣",
		     uident);
		sprintf(title, "�����š�����·��������ԭ�����");
		sprintf(buf,
			"    %s��סլ���ջ٣����������;�����ʧ���ڽ�һ�������С�\n"
			"    ��������������ĺڰ��йء�\n", uident);
		deliverreport(title, buf);
		sprintf(title, "���⵽͵Ϯ");
		sprintf(buf, "%s���㲻����͵Ϯ���סլ���˳𲻿ɲ�����",
			currentuser->userid);
		if (mcuInfo->GetLetter == 1)
			system_mail_buf(buf, strlen(buf), uident, title,
					currentuser->userid);

	} else {
		myInfo->begExp -= 50;
		myInfo->robExp -= 50;
		mcuInfo->begExp += myInfo->begExp / 100;
		mcuInfo->robExp += myInfo->robExp / 100;
		myInfo->luck -= 30;
		myInfo->WorkPoint -= 7200;
		myInfo->health = 0;

		move(13, 4);
		prints("%s��Ȼ�������Ļ�����������������!", uident);
		prints("\n    ����æ�ӳ����������Ż��ҳ�����!");
		prints
		    ("\n	  ��ɤ��һ��һ����Ѫ���˳������Ͼ��������˰��˰���");
		prints
		    ("\n    ��ĵ�ʶ�����½�50��!\n    %s�ĵ�ʶ������1%%��\n    �����Ʒ�½�30�㡣",
		     uident);

		sprintf(title, "�����š��ڰ﷢���ֲ�Ϯ��");
		sprintf(buf,
			"    ���챾�з���һ����Ժڰ�ֲ�Ϯ���¼���\n"
			"    ����Ŀ���߾ٱ��������Ѿ���������������%s��\n",
			currentuser->userid);
		deliverreport(title, buf);
		sprintf(title, "%s��ͼ͵Ϯ��", currentuser->userid);
		sprintf(buf,
			"%s͵Ϯ���סլ��������Ҳ�ݺݵĸ�����һ�£�����С���ܵġ�",
			currentuser->userid);
		if (mcuInfo->GetLetter == 1)
			system_mail_buf(buf, strlen(buf), uident, title,
					currentuser->userid);

	}
	unloadData(mcuInfo, sizeof (mcUserInfo));
	pressanykey();
	return;

}

void
Rob_Rebellion()
{
	char buf[256], title[STRLEN], uident[IDLEN + 1], quit = 0, ch;
	time_t currtime;
	mcUserInfo *mcuInfo;
	int WorkPoint, num;
	char *RobWeapon[] =
	    { "ˮǹ", "�������ǹ", "����ǹ", "��ҩǹ", "��ǹ", "��ǹ",
		"�Զ���ǹ", "���ǹ", "�ѻ�ǹ", "���ǹ", "�ػ�ǹ", "���Ͳ",
		    "�罬ǹ",
		"����ǹ", "������", "������", NULL
	};

	nomoney_show_stat("˾�");

	if (check_health(140, 12, 4, "׼������־Ͳ�Ҫ�����ˡ�", YEA))
		return;

	if ((myInfo->robExp < 20000) || (myInfo->begExp < 10000)) {
		showAt(12, 4, "��������Ҳ�뱩������ȥ����Ժ������", YEA);
		return;
	}

	if (myInfo->luck < 90) {
		showAt(12, 4, "�ڵ��˵����෢��������һ����Ҫ���õ�αװ", YEA);
		return;
	}

	currtime = time(NULL);
	if (myInfo->WorkPoint < 18000) {
		showAt(12, 4, "������ǰ�ƣ�������ʱ�������ء�", YEA);
		return;
	}

	myInfo->lastActiveTime = currtime;
	myInfo->WorkPoint -= 12000;
	myInfo->health -= 80;
	myInfo->Actived += 30;

	nomoney_show_stat("�׶�");

	move(6, 4);
	prints
	    ("��߳��š������Ǻ��棬�����Ǻ��档�����˸߲��ҵ���%s��������",
	     CENTER_NAME);
	pressanykey();

	move(7, 4);
	prints
	    ("˳������%s�����ſڣ�����������е�%s,�ߺ������������췴�����",
	     CENTER_NAME, RobWeapon[myInfo->weapon]);
	move(8, 4);
	prints("��ΧһȺfq ������壬�·��������ʵ֧���ߡ�");
	sleep(1);

	if (random() % 2) {
		move(9, 4);
		prints("���������������ҵ�������ʱ���Ȼ����ĺ�Х����Զ������");
		move(10, 4);
		prints
		    ("��Щ�һ��Ȼ����ΰ�����ҵ�����˾��񲡣����ǲ��ɾ�ҩ��");
		move(11, 4);
		prints
		    ("���ɳܵ��ǣ����Ǿ�Ȼ������Ŀ��飬ǿ�а������ϾȻ������ͽ��˾���Ժ");
		move(13, 4);
		prints("��ĵ�ʶ���½���    �����Ʒ����!\n");
		myInfo->robExp -= 20;
		myInfo->begExp -= 20;
		myInfo->luck -= 50;
		sprintf(title, "�����š� ���񲡽�ͷ����");
		sprintf(buf,
			"    ���ձ���һ������������ǰ���£���������Χ�ۡ�\n"
			"    ������Ժ��ϣ����˻������������Ծ��񲡣�סԺ����֮���Ѵ��к�ת��");
		deliverreport(title, buf);
		pressanykey();
		return;
	}

	if (check_chance(myInfo->robExp, 23000, myInfo->weapon, 15, -750, 0)) {
		move(9, 4);
		prints
		    ("һȺ�侯������������fq ������һ���ɢ������һ���˹����������ԭ�ء�");
		move(10, 4);
		prints
		    ("�㻹û�д���޴�ķ����з�Ӧ�������侯�Ѿ����ϵĶ���˵���������ӷ�������������Դ�����");
		move(12, 4);
		prints
		    ("��ĵ�ʶ���½���    �����Ʒ����!\n    �������ȫʧ!    �㱻�����Ѻ4Сʱ!");
		myInfo->robExp -= 50;
		myInfo->begExp -= 50;
		myInfo->health = 0;
		myInfo->luck -= 100;
		myInfo->mutex = 0;
		myInfo->freeTime = time(NULL) + 14400;

		sprintf(title, "�����š� %s��ͼ��������������",
			currentuser->userid);
		sprintf(buf,
			"    %s��ͼ�������������䣬��������������ı���������Է��顣\n"
			"    %s�Ѿ����������������Է������ﴦ��4Сʱͽ�̡�",
			currentuser->userid, currentuser->userid);
		deliverreport(title, buf);
		unloadData(myInfo, sizeof (mcUserInfo));
		unloadData(mcEnv, sizeof (MC_Env));
		pressanykey();
		Q_Goodbye();
		return;
	}

	myInfo->begExp += 200;
	myInfo->robExp += 200;
	num = MAX(MIN(3000000, mcEnv->Treasury - 20000000), 0);
	myInfo->cash += num;
	mcEnv->Treasury -= num;
	WorkPoint = myInfo->WorkPoint;
	move(9, 4);
	prints("��Ѹ�ٳ����˾������ǽ�������������¥��");
	prints("\n    ����������ڸ߸�Ʈ����¥����%s����������������!",
	       currentuser->userid);
	prints("\n	  ��ĵ�ʶ������! ��ӹ������ѹ���%d%s��", num,
	       MONEY_NAME);
	sprintf(title, "�����š� %s��������", currentuser->userid);
	sprintf(buf, "    %s�ɹ��������䣬ռ����������¥��\n",
		currentuser->userid);
	deliverreport(title, buf);
	pressanykey();

	while (!(quit || (WorkPoint < 6000))) {
		nomoney_show_stat("�칫��");
		move(7, 4);
		prints("�����Ѿ��ɹ�����ժ��ʵ��ʱ���ˣ����ڿ��Է���ʩ���ˡ�");
		move(t_lines - 1, 0);
		prints
		    ("\033[1;44m ѡ�� \033[1;46m [1]���� [2]ץ�� [3]����        [Q]�뿪                                  \033[m");
		ch = igetkey();
		switch (ch) {
		case '1':
			nomoney_show_stat("������");
			if (!getOkUser("��׼������˭��", uident, 5, 4)) {
				showAt(7, 4, "���޴���", YEA);
				break;
			}

			if (!strcmp(uident, currentuser->userid)) {
				showAt(7, 4, "�Լ������Լ������������Ⱑ", YEA);
				break;
			}

			if (!(seek_in_file(DIR_MC "policemen", uident))) {
				showAt(7, 4,
				       "���Ҳ�������������״��ֻ�ܲݲ����¡�",
				       YEA);
				break;
			}

			sethomefile(buf, uident, "mc.save");
			if (!file_exist(buf))
				initData(1, buf);
			if((mcuInfo = loadData(buf, sizeof (mcUserInfo))) == (void*)-1)
				break;
			if (mcuInfo->GetLetter == 0) {
				showAt(7, 4,
				       "�˼Ҳ�Ը������棬�ǲ��𣬶㻹���а���",
				       YEA);
				unloadData(mcuInfo, sizeof (mcUserInfo));
				break;
			}

			WorkPoint -= 6000;
			myInfo->WorkPoint -= 6000;
			if (check_chance
			    (myInfo->robExp, mcuInfo->robExp, myInfo->weapon,
			     mcuInfo->armor, 300, 300)) {
				move(10, 4);
				prints
				    ("��ҡͷ���Ե�˵:��Ҫ�䶷����Ҫ�Ķ���Ҫ����������ܴ�����ꡣ��"
				     "\n    ��Ŀ���ӵ������һӵ���ϣ�%s����ñ������ˡ�",
				     uident);
				sprintf(title, "�㱻%s��ӵ���ߴ���",
					currentuser->userid);
				sprintf(buf,
					"%sɿ������ӵ���ߴ������㣬�㲻�ò�סԺ4Сʱ��",
					currentuser->userid);
				if (mcuInfo->GetLetter == 1)
					system_mail_buf(buf, strlen(buf),
							uident, title,
							currentuser->userid);
				mcuInfo->freeTime = time(NULL) + 14400;
			} else {
				move(10, 4);
				prints("%s������������Ѿ����۶�û���Ʒ�����",
				       uident);
			}
			pressanykey();
			break;
		case '2':
			nomoney_show_stat("�����");
			if (!getOkUser("��׼������˭��", uident, 5, 4)) {
				showAt(7, 4, "���޴���", YEA);
				break;
			}

			if (!strcmp(uident, currentuser->userid)) {
				showAt(7, 4, "�Լ�ץ�Լ������������Ⱑ", YEA);
				break;
			}

			if (seek_in_file(DIR_MC "policemen", uident)) {
				showAt(7, 4, "���Ҳ���������״��ֻ�ܲݲ����¡�",
				       YEA);
				break;
			}

			sethomefile(buf, uident, "mc.save");
			if (!file_exist(buf))
				initData(1, buf);
			if((mcuInfo = loadData(buf, sizeof (mcUserInfo))) == (void*)-1)
				break;
			if (mcuInfo->GetLetter == 0) {
				showAt(7, 4,
				       "�˼Ҳ�Ը������棬�ǲ��𣬶㻹���а���",
				       YEA);
				unloadData(mcuInfo, sizeof (mcUserInfo));
				break;
			}

			WorkPoint -= 6000;
			myInfo->WorkPoint -= 6000;
			if (check_chance
			    (myInfo->robExp, mcuInfo->robExp, myInfo->weapon,
			     mcuInfo->armor, 300, 300)) {
				move(10, 4);
				prints
				    ("��ڳ�һ������˽�����Ӷ�%s˵:\n		 ���������ƨ���ѿ��ӣ������Ҿ����������������!��",
				     uident);
				sprintf(title, "�㱻%s����",
					currentuser->userid);
				sprintf(buf,
					"%s�۶���Ȩ�Ժ���Ī���е������������㣬������4Сʱ��",
					currentuser->userid);
				if (mcuInfo->GetLetter == 1)
					system_mail_buf(buf, strlen(buf),
							uident, title,
							currentuser->userid);
				mcuInfo->freeTime = time(NULL) + 14400;
			} else {
				move(10, 4);
				prints("%s��Ȼ��װ�ܲ������޿��κε���ȥ�ˡ�",
				       uident);
			}
			pressanykey();
			break;
		case '3':
			WorkPoint -= 6000;
			myInfo->WorkPoint -= 6000;
			if (random() % 2) {
				nomoney_show_stat("ɽ��ˮ��");
				move(7, 4);
				prints
				    ("ÿ��Ϊ�����ͣ����ǷѾ����񰡣��Ҹ�������Ϣһ�¡�"
				     "\n    ������Ϫˮ�У�һ����С���ڻ��ֵ��ζ��������ץ������һ���ڸ���");
				pressanykey();
				if (check_chance
				    (myInfo->robExp, 32000, myInfo->weapon, 15,
				     1000, 0)) {
					move(14, 4);
					prints
					    ("����������һ�ٿ��㣬����������ˬ��"
					     "\n    ��ĵ�ʶ��������");
					myInfo->robExp += 150;
					myInfo->begExp += 150;
				} else {
					move(14, 4);
					prints
					    ("С�㻬�����֣�����æ����ȴ��һ����û��ץס��"
					     "\n    ��ŭ������㣬���㣬��û��ɿ������ô���ˣ���");
				}
			} else {
				nomoney_show_stat("���Գ�");
				move(7, 4);
				prints
				    ("ÿ��Ϊ�����ͣ����ǷѾ����񰡣��Ҹ�������Ϣһ�¡�"
				     "\n    ��������и߷ɵĺ�ӥ�����乭�������һ�����֡�");
				pressanykey();
				if (check_chance
				    (myInfo->robExp, 32000, myInfo->weapon, 15,
				     1000, 0)) {
					move(14, 4);
					prints
					    ("�������ǣ���ӥӦ�����䣬��е������졣"
					     "\n    ��ĵ�ʶ��������");
					myInfo->robExp += 150;
					myInfo->begExp += 150;
				} else {
					move(14, 4);
					prints("ʧ֮���壬��֮ǧ�"
					       "\n    ��ӥ��м�ش���ͷ���ӹ�");
				}
			}
			pressanykey();
			break;
		case 'q':
		case 'Q':
			quit = 1;
			break;
		}
	}

	nomoney_show_stat("���ǽ�ͷ");
	showAt(7, 4, "�����������ˣ��������ʱս���Գ��ˡ�", YEA);
	return;
}

int
forceGetMoney(int type)
{
	int money, cost_health;
	time_t currtime;
	char uident[IDLEN + 1], buf[256], place[STRLEN];
	char *actionDesc[] = { "����", "����", "����", "���ֿտ�", NULL };
	mcUserInfo *mcuInfo;

	if ((type == 2 && myInfo->robExp < 50)
	    || (type == 3 && myInfo->begExp < 50)) {
		move(10, 8);
		prints("�㻹û���㹻��%s����%s��ô������顣",
		       type % 2 ? "��" : "��ʶ", actionDesc[type]);
		pressanykey();
		return 0;
	}
	move(4, 4);
	switch (type) {
	case 0:
		strcpy(place, "������");
		break;
	case 1:
		strcpy(place, "���ĵض�");
		break;
	case 2:
		strcpy(place, "����С��");
		break;
	case 3:
		strcpy(place, "��ҵ��");
		break;
	}
	prints("������%s��%s��ʵ����%s�ĺõط���", CENTER_NAME, place,
	       actionDesc[type]);
	if (!getOkUser("��Ҫ��˭���֣�", uident, 6, 4)) {
		move(7, 4);
		prints("���޴���");
		pressanykey();
		return 0;
	}
	if (!strcmp(uident, currentuser->userid)) {
		showAt(7, 4, "ţħ���������š��������񾭲�������", YEA);
		return 0;
	}
	move(7, 4);
	if ((type % 2 == 0 && !clubtest(ROBUNION))
	    || (type % 2 == 1 && !clubtest(BEGGAR))) {
		prints("��ô����Ҳ�����ǻ�%s���˰���", actionDesc[type]);
		pressanykey();
		return 0;
	}
#if 0				//������Ҳ����͵�������ٻ�������
	if (!t_search(uident, NA, 1) && type <= 1) {
		if (type == 0)
			prints("�㿴�����˰ɣ��չ�����˲���%s����", uident);
		else if (type == 1)
			prints("%s���ڼң������˰�����Ҳû��Ӧ��", uident);
		else if (type == 2)
			prints("����С��ȷʵ���尡����Ȼһ���˶�û�С�������");
		else
			prints("ʮ��·�����������������˰���û�ҵ�Ҫ�ҵ�%s��",
			       uident);
		pressanykey();
		return 0;
	}
#endif
	cost_health = 25 + (type / 2 * 10);
	if (check_health(cost_health, 12, 4, "��������ô���������°���", YEA))
		return 0;

	currtime = time(NULL);
	if (myInfo->WorkPoint < (type / 2 + 1) * 1500) {
		if (type % 4 == 0)
			prints
			    ("������껵�£��˼Ҽ������¡�%sһ�������ԶԶ�Ķ㿪�ˣ�ѹ����������ߡ�",
			     uident);
		else if (type == 1)
			prints
			    ("%sŭ���ɶ������������Ҫ���ģ������ˣ������������",
			     uident);
		else		//���ֿտ� 
			prints
			    ("����Ҫ���֣��������������˺����������ҵ�Ǯ�������ˣ��� \n    %sһ�������ϰ�Ǯ������ס�ˡ���",
			     uident);
		pressanykey();
		return 0;
	}
	myInfo->lastActiveTime = currtime;
	update_health();
	myInfo->WorkPoint -= (type / 2 + 1) * 1500;
	myInfo->health -= cost_health;
	myInfo->Actived += 5;
	myInfo->luck--;
	switch (type) {
	case 0:
		prints("���������ָ�׵�, ����%s�ٺټ�Ц��: ����ȡ��·�ѣ���\n",
		       uident);
		break;
	case 1:
		prints
		    ("�����%s�޺����������õ�廨�ɡ����Ѿ��ü�����û�Է��ˣ��Һö�������\n",
		     uident);
		break;
	case 2:
		prints
		    ("�����õ�Ƭ����%s��ݺݵĺ�������IC~IP~IQ~~����ͳͳ���������룡����\n",
		     uident);
		break;
	case 3:
		prints
		    ("�㲻����ɫ�Ŀ���%s����֪�����İ������������¶�������\n",
		     uident);
		break;
	}

	sleep(1);
	sethomefile(buf, uident, "mc.save");
	if (!file_exist(buf))
		initData(1, buf);
	if((mcuInfo = loadData(buf, sizeof (mcUserInfo))) == (void*)-1)
		return 0;

	if (mcuInfo->GetLetter == 0) {
		clear();
		showAt(6, 4, "�˼Ҳ�Ը������棬�ǲ��𣬶㻹���а���", YEA);
		unloadData(mcuInfo, sizeof (mcUserInfo));
		return 0;
	}

	if ((!(random() % 4)) || mcuInfo->luck - myInfo->luck >= 100) {
		if (type % 2 == 1) {
			prints("    %sһ�Ű����߿�����������л���һ��ȥ��\n",
			       uident);
		} else {
			prints
			    ("    %s����һ�Ű����߷ɡ��ߵ����������Ʋ���������ٵģ�һ�㼼��������û�С���\n",
			     uident);
		}
		prints("\n    ����һ�죬�Ͻ���������ܿ���");
		pressanykey();
		return 0;
	}

	money = MIN(mcuInfo->cash / 10, 1000000);
	money = MIN(money, myInfo->cash * 2);
	move(8, 4);
	if (money == 0) {
		prints("%s����ûǮ������������浹ù...", uident);
		goto UNMAP;
	}
	if (type == 0) {
		if (check_chance
		    (myInfo->robExp, mcuInfo->robExp, myInfo->weapon,
		     mcuInfo->armor, 300, 0)) {
			mcuInfo->cash -= money;
			myInfo->cash += money;
			myInfo->robExp += 1;
			myInfo->health -= 20;
			prints
			    ("%s�ŵ�ֱ�����, �Ͻ��������ó� %d %s���㡣\n\n\033[32m    ��ĵ�ʶ�����ˣ�\033[m",
			     uident, money, MONEY_NAME);
			if (myInfo->luck > -100) {
				myInfo->luck -= 1;
				prints("\n\033[31m    �����Ʒ�����ˣ�");
			}
			sprintf(genbuf, "�㱻%s ������ %d%s��̫�����ˡ�",
				currentuser->userid, money, MONEY_NAME);
			sprintf(buf, "���⵽����");
		} else if (random() % 3) {
			myInfo->robExp += 1;
			myInfo->health -= 20;
			prints
			    ("%s�ŵð��Ⱦ���, ��Ȼû������Ǯ��������ı��ֻ��ǿ�Ȧ�ɵ㡣 \n\n\033[32m    ��ĵ�ʶ�����ˣ�\033[m",
			     uident);
			if (myInfo->luck > -100) {
				myInfo->luck -= 1;
				prints("\n\033[31m    �����Ʒ�����ˣ�");
			}
			pressanykey();
			return 0;
		} else {
			prints("�����о�������ˣ������������\n");
			pressanykey();
			return 0;
		}
	} else if (type == 1) {
		if (check_chance
		    (myInfo->begExp, mcuInfo->begExp, myInfo->weapon,
		     mcuInfo->armor, 300, 0)) {
			mcuInfo->cash -= money;
			myInfo->cash += money;
			myInfo->begExp += 1;
			myInfo->health -= 20;
			prints
			    ("%s��Ȧ��ʱ���ˣ��Ͻ��������ó� %d %s��������ӹ�һ�䡣\n\n\033[32m    ���������ˣ�\033[m",
			     uident, money, MONEY_NAME);
			if (myInfo->luck > -100) {
				myInfo->luck -= 1;
				prints("\n\033[31m    �����Ʒ�����ˣ�");
			}
			sprintf(genbuf,
				"��һʱ���ģ�����%d%s��%s�����˶仨 ���������ǹ�β�Ͳݡ�����",
				money, MONEY_NAME, currentuser->userid);
			sprintf(buf, "����������С��");
		} else {
			if (random() % 3) {
				myInfo->begExp += 1;
				myInfo->health -= 20;
				prints
				    ("%s΢Ц���������ͷ˵����С�ֵܣ����ݵò�����\n\n\033[32m    ���������ˣ�\033[m",
				     uident);
				if (myInfo->luck > -100) {
					myInfo->luck -= 1;
					prints
					    ("\n\033[31m    �����Ʒ�����ˣ�");
				}
				pressanykey();
				return 0;
			} else {
				prints("���ۣ��ǹ����ˣ����ܰ���\n");
				pressanykey();
				return 0;
			}
		}
	} else if (type == 2) {
		if (check_chance
		    (myInfo->robExp, mcuInfo->robExp, myInfo->weapon,
		     mcuInfo->armor, 250, 0)) {
			money = MIN(mcuInfo->cash / 2, 5000000);
			money = MIN(myInfo->cash / 2 * 3, money);
			mcuInfo->cash -= money;
			myInfo->cash += money;
			myInfo->robExp += 5;
			myInfo->health -= 30;
			prints
			    ("%s�ŵð����ϵ�%d%sȫ���˳������޵���������ǧ����ɫ������\n\n\033[32m    ��ĵ�ʶ�����ˣ�\033[m",
			     uident, money, MONEY_NAME);
			if (myInfo->luck > -100) {
				myInfo->luck = MAX((myInfo->luck - 3), -100);
				prints("\n\033[31m    �����Ʒ�����ˣ�");
			}
			sprintf(genbuf,
				"�������ٷˣ��� %s ���� %d%s�������������ᰡ��\n",
				currentuser->userid, money, MONEY_NAME);
			sprintf(buf, "�㱻����");
		} else if (random() % 3) {
			myInfo->robExp += 5;
			myInfo->health -= 30;
			prints
			    ("%s�ŵð����ϵ�%d%sȫ���˳��������ǣ�Ϊʲô�����ûһ���泮Ʊ�أ���\n\n\033[32m    ��ĵ�ʶ�����ˣ�\033[m",
			     uident, money, MONEY_NAME);
			if (myInfo->luck > -100) {
				myInfo->luck = MAX((myInfo->luck - 3), -100);
				prints("\n\033[31m    �����Ʒ�����ˣ�");
			}
			pressanykey();
			return 0;
		} else {
			money = MIN(myInfo->cash / 2, 5000000);
			mcuInfo->cash += money;
			myInfo->cash -= money;
			myInfo->robExp = MAX((myInfo->robExp - 10), 0);
			myInfo->health -= 40;
			prints
			    ("%s���Ų�æ�ĴӶ����ͳ�һ����ǹ����ס�����ţ��������Ǯ�������ɡ�\n\n    ����������֮ͽ����ʧ��%d%s...\n\n\033[31m    ��ĵ�ʶ�����\033[m",
			     uident, money, MONEY_NAME);
			sprintf(genbuf,
				"�ٷ� %s ����ڳԺڣ����� %d%s����ϲ�㡫\n",
				currentuser->userid, money, MONEY_NAME);
			sprintf(buf, "��ڳԺڳɹ�");
		}
	} else {
		if (check_chance
		    (myInfo->begExp, mcuInfo->begExp, myInfo->weapon,
		     mcuInfo->armor, 250, 0)) {
			money = MIN(mcuInfo->cash / 2, 5000000);
			money = MIN(myInfo->cash / 2 * 3, money);
			mcuInfo->cash -= money;
			myInfo->cash += money;
			myInfo->begExp += 5;
			myInfo->health -= 30;
			prints
			    ("�������ɹ��ˣ����õ�Ƭ��%s���¶�����һ������,͵��%d%s��\n\n\033[32m    ���������ˣ�\033[m",
			     uident, money, MONEY_NAME);
			if (myInfo->luck > -100) {
				myInfo->luck = MAX((myInfo->luck - 3), -100);
				prints("\n\033[31m    �����Ʒ�����ˣ�");
			}
			sprintf(genbuf,
				"��ȥ��������������������ڵ�ӰԺ��͵�� %d%s�������������ᰡ��\n",
				money, MONEY_NAME);
			sprintf(buf, "����������");
		} else if (random() % 3) {
			myInfo->begExp += 5;
			myInfo->health -= 30;
			prints
			    ("�������ɹ��ˣ����õ�Ƭ��%s���¶�����һ������,��ϧ��������ȫ�ǲ�ֽ��\n ��޺޵�д�ϣ��벻Ҫ����������Ц��Ӱ���������������˻�ȥ�� \n\n\033[32m    ���������ˣ�\033[m",
			     uident);
			if (myInfo->luck > -100) {
				myInfo->luck = MAX((myInfo->luck - 3), -100);
				prints("\n\033[31m    �����Ʒ�����ˣ�");
			}
			pressanykey();
			return 0;
		} else {
			money = MIN(myInfo->cash / 2, 5000000);
			mcuInfo->cash += money;
			myInfo->cash -= money;
			myInfo->begExp = MAX((myInfo->begExp - 10), 0);
			myInfo->health -= 40;
			prints
			    ("�������ɹ��ˣ����õ�Ƭ��%s���¶�����һ�����ӣ�͵��...�ף�\n    һ����������21������ȱʲô������������\n    ���������壬��ʧ��%d%s...\n\n\033[31m    ������󽵣�\033[m",
			     uident, money, MONEY_NAME);
			sprintf(genbuf,
				"%s ͵�����ɷ�ʴ���ף�����˳��ǣ������ %d%s���ٺ١�\n",
				currentuser->userid, money, MONEY_NAME);
			sprintf(buf, "�ƻ���ľ�ɹ�");
		}
	}
	if (mcuInfo->GetLetter == 1)
		system_mail_buf(genbuf, strlen(genbuf), uident, buf,
				currentuser->userid);
UNMAP:
	unloadData(mcuInfo, sizeof (mcUserInfo));
	pressanykey();
	return 1;
}

static int
money_pat()
{
	int r, x, y, num, count = 0;
	char uident[IDLEN + 1], buf[256];
	struct userec *lookupuser;
	mcUserInfo *mcuInfo;

	sprintf(buf, "%s�ڰ�", CENTER_NAME);
	money_show_stat(buf);
	move(4, 4);
	if (check_health(5, 12, 4, "�������������ˣ�", YEA))
		return 0;
	prints("����İ�ש�ʵ���������ȥ����һ��ʹ�졣\n"
	       "   ���ڴ�����һ���ש�� %d %s��", BRICK_PRICE, MONEY_NAME);
	move(6, 4);
	usercomplete("��Ҫ��˭:", uident);
	if (uident[0] == '\0')
		return 0;
	if (!getuser(uident, &lookupuser)) {
		showAt(7, 4, "�����ʹ���ߴ���...", YEA);
		return 0;
	}

	if (!strcmp(uident, currentuser->userid)) {
		showAt(7, 4, "ţħ���������š��������񾭲�������", YEA);
		return 0;
	}
	if (!t_search(uident, NA, 1)) {
		showAt(7, 4, "��ȵĻ�����л�ˣ�Ŀ�껹��û�г��֡�", YEA);
		return 0;
	}
	count = userInputValue(7, 4, "��", "��ש", 1, 100);
	if (count < 0)
		return 0;
	num = count * BRICK_PRICE;
	if (myInfo->cash < num) {
		move(9, 4);
		prints("����Ǯ����...��Ҫ %d %s", num, MONEY_NAME);
		pressanykey();
		return 0;
	}
	if (myInfo->luck > -100) {
		myInfo->luck = MAX((myInfo->luck - 1), -100);
		prints("\033[31m    �����Ʒ�����ˣ�\033[m");
	}
	myInfo->cash -= num;
	mcEnv->Treasury += num;	//��ש��Ǯ���ع���
	move(10, 8);
	prints("���������͵���͸��٣��㷢��ÿ������7��10��%s��·��Ƨ����",
	       uident);
	move(11, 4);
	prints("���ǵء����������������İ�ש��׼���ж���...");
	move(12, 8);
	prints("��ש�Ǻ�Σ�յ�ร� �㲻�û�������ģ�С�İ���");
	move(13, 4);
	if (askyn("�ϻ���˵���㻹����ô��", YEA, NA) == NA) {
		move(15, 4);
		myInfo->robExp = MAX((myInfo->robExp - 1), 0);
		update_health();
		myInfo->health--;
		myInfo->Actived++;
		prints
		    ("��������ͷ�㺦���ˣ����Բ����ˡ�\n    ��ĵ�ʶ�����ˡ�");
		pressanykey();
		return 0;
	}
#if 0
	x = countexp(currentuser);
	y = countexp(lookupuser);
#endif
	sethomefile(buf, uident, "mc.save");
	if (!file_exist(buf))
		initData(1, buf);
	if((mcuInfo = loadData(buf, sizeof (mcUserInfo))) == (void*)-1)
		return -1;

	if (mcuInfo->GetLetter == 0) {
		clear();
		showAt(6, 4, "�˼Ҳ�Ը������棬�ǲ��𣬶㻹���а���", YEA);
		unloadData(mcuInfo, sizeof (mcUserInfo));
		return 0;
	}

	x = myInfo->robExp * myInfo->robExp + 1;
	y = mcuInfo->robExp * mcuInfo->robExp;
	r = (double) x *100 / (x + y + 1);	//�����˵�ʶ>4634 ʱ����Ϊ����������ĵ�bug
	num = count * BRICK_PRICE / 2 + 6000;	//ҽҩ�� 
	update_health();
	myInfo->health -= 5;
	myInfo->Actived += 2;
	move(16, 4);
	if (r + count < 101) {	//Ŀ�굨ʶ�����Լ�Լ10���ͻ�ط������ٺ� 
		prints("�ܲ��ң���û�����С�����������С�Դ���...");
		move(17, 4);
		prints("����Ѫ��ֹ�����͵�ҽԺ����ʮ���룬�òҰ���");
		move(18, 4);
		if (random() % 2) {
			myInfo->robExp = MAX((myInfo->robExp - 1), 0);
			prints("��ĵ�ʶ�����ˡ�\n    ");
		}
		if (random() % 2) {
			myInfo->begExp = MAX((myInfo->begExp - 1), 0);
			prints("����������ˡ�\n    ");
		}
		mcEnv->Treasury += MIN(myInfo->cash, num * 2);
		myInfo->cash = MAX((myInfo->cash - num * 2), 0);
		update_health();
		myInfo->health = 0;
		prints("��󻹽��� %d %s��ҽҩ�ѣ������Ժ󻹸Ҳ���", num * 2,
		       MONEY_NAME);
		pressanykey();
		return 0;
	}
	if (myInfo->begExp > mcuInfo->begExp * 20 + 50) {
		myInfo->luck--;
		prints("�Է���ô�����Ȼ�������֣�\n    �����Ʒ�����ˡ�");
		pressanykey();
		return 0;
	}
	if (mcuInfo->guard > 0 && rand() % 3) {
		prints("б����ͻȻ���һֻ���ǹ�����һ���񣬰�שȫ�ҿ��ˡ�\n"
		       "    �ۣ����ǹ������˹����ˣ�");
		if (askyn("��Ҫ��Ҫ�ܣ�", YEA, NA) == YEA) {
			if (random() % 2) {
				myInfo->robExp = MAX((myInfo->robExp - 1), 0);
				prints("��ĵ�ʶ�����ˡ�\n    ");
			}
			if (random() % 4) {
				prints
				    ("\n    ��һ������ɿ�����һ��С���ܵ��ˡ����հ���");
				pressanykey();
				return 0;
			} else {
				prints
				    ("\n    ���Ȼ�ܽ���һ������ͬ���ǹ���������......\n һ���췭�ظ�֮��......");
				if (random() % 3) {
					if (myInfo->begExp) {
						myInfo->begExp--;
						prints
						    ("\n    ����������ˣ�");
					}
					update_health();
					myInfo->health = myInfo->health / 2;
					prints("\n    ����������룡");
					sprintf(buf,
						"%s����Ĵ��ǹ�ҧ�ˣ��ǵý����ǹ������ͷŶ��",
						currentuser->userid);
					if (mcuInfo->GetLetter == 1)
						system_mail_buf(buf,
								strlen(buf),
								uident,
								"��Ĵ��ǹ��ɹ�������",
								currentuser->
								userid);
				}
			}
		}

		if (random() % 3) {
			mcuInfo->guard--;
			myInfo->health -= 5;
			sleep(1);
			prints("\n    �������Ҳ����������ڸɵ��˴��ǹ�������");
			sprintf(buf,
				"���һֻ���ǹ���%s�ɵ��ˣ������ڻ�ʣ%dֻ���ǹ���",
				currentuser->userid, mcuInfo->guard);
			if (mcuInfo->GetLetter == 1)
				system_mail_buf(buf, strlen(buf), uident,
						"���һֻ���ǹ�׳������",
						currentuser->userid);
			if (!(random() % 4)) {
				myInfo->robExp++;
				prints("\n    ��ĵ�ʶ�����ˣ�");
			}
			prints("\n    �����������������½���");
		} else {
			sleep(1);
			prints
			    ("\n    �����������������Ǳ����ǹ�������ҧ��һ�ڡ�");
			if (random() % 3) {
				if (myInfo->begExp) {
					myInfo->begExp--;
					prints("\n    ����������ˣ�");
				}
				update_health();
				myInfo->health = myInfo->health / 2;
				prints("\n    ����������룡");
				sprintf(buf,
					"%s����Ĵ��ǹ�ҧ�ˣ��ǵý����ǹ������ͷŶ��",
					currentuser->userid);
				if (mcuInfo->GetLetter == 1)
					system_mail_buf(buf, strlen(buf),
							uident,
							"��Ĵ��ǹ��ɹ�������",
							currentuser->userid);
			}
		}
		goto UNMAP;
	}
	if ((random() % 5 || (myInfo->begExp > mcuInfo->begExp * 10 + 50)) &&
	    !(mcuInfo->begExp > myInfo->begExp * 10 + 50)) {
		prints("���⻵��������͵Ϯ������%s��С�Դ��ϡ�", uident);
		if (mcuInfo->cash < num) {
			move(17, 4);
			update_health();
			mcuInfo->health = 0;
			mcEnv->Treasury += mcuInfo->cash;
			mcuInfo->cash = 0;
			prints("�㶼�ĵ��˼�ûǮ��������...�������°ɣ�");
			sprintf(buf,
				"�㱻%s���˰�ש����ûǮ���ˣ�ֻ��ҧ����ʹ...",
				currentuser->userid);
			if (!(random() % 10)) {
				myInfo->robExp++;
				prints("\n    ��ĵ�ʶ�����ˣ�");
			}
			if (!(random() % 10)) {
				myInfo->begExp++;
				prints("\n    ����������ˣ�");
			}
			if (!(random() % 4))
				mcuInfo->begExp = MAX(0, mcuInfo->begExp--);
		} else {
			mcEnv->Treasury += num;
			mcuInfo->cash -= num;
			mcuInfo->health -= 10;
			if (!(random() % 5))
				mcuInfo->begExp = MAX(0, mcuInfo->begExp--);
			move(17, 4);
			prints("������%s����%d%s���ˣ���ҽԺ�����˺ö��죡",
			       uident, num, MONEY_NAME);
			sprintf(buf, "�㱻%s���˰�ש������%d%s���ˣ�������...",
				currentuser->userid, num, MONEY_NAME);
			if (!(random() % 4)) {
				myInfo->robExp++;
				prints("\n    ��ĵ�ʶ�����ˣ�");
			}
			if (!(random() % 4)) {
				myInfo->begExp++;
				prints("\n    ����������ˣ�");
			}
		}
		if (mcuInfo->GetLetter == 1)
			system_mail_buf(buf, strlen(buf), uident,
					"�㱻���˰�ש", currentuser->userid);
	} else {
		if (random() % 2) {
			mcuInfo->begExp++;
			sprintf(buf,
				"%s�ð�ש������գ�����������ˣ�ŶҮ��\n",
				currentuser->userid);
		} else {
			mcuInfo->robExp++;
			sprintf(buf,
				"%s�ð�ש������գ���ĵ�ʶ�����ˣ�ŶҮ��\n",
				currentuser->userid);
		}
		if (mcuInfo->GetLetter == 1)
			system_mail_buf(buf, strlen(buf), uident,
					"������שϮ��", currentuser->userid);
		prints("��ѽѽ��û���С�����");

	}

UNMAP:
	unloadData(mcuInfo, sizeof (mcUserInfo));
	pressanykey();
	return 0;
}

static void
RobShop()
{
	int num;
	char buf[256], title[STRLEN];
	time_t currtime;

	if (check_health(90, 12, 4, "��ô�������û�г��������޷���ɡ�", YEA))
		return;
	if (myInfo->robExp < 200) {
		showAt(12, 4, "����ԥ�˰��죬����û�Ҷ��֡�����", YEA);
		return;
	}
	if (myInfo->begExp < 100) {
		showAt(12, 4,
		       "����붯�֣������վ�����ͱ����ϵĵ��Ӱ��ˡ�����",
		       YEA);
		return;
	}
	if (myInfo->luck < 60) {
		showAt(12, 4, "���ܾ��ñ�����˫�۾��ڶ����㿴��", YEA);
		return;
	}

	nomoney_show_stat("����");
	move(4, 4);
	currtime = time(NULL);
	if (myInfo->WorkPoint < 4500) {
		showAt(12, 4,
		       "�ղų��˸����ӣ��о����ڸ���Ѳ�ӣ��Ȳ�Ҫ����Ϊ�á�",
		       YEA);
		return;
	}

	if (askyn("��װ���پ�Ҫ��������װ����Ҫ20����ȷ��Ҫ����", NA, NA) ==
	    NA) {
		myInfo->robExp--;
		prints("\n    ����������ˡ�\n    ��ĵ�ʶ���ͣ�");
		return;
	}
	if (myInfo->cash < 200000) {
		showAt(12, 4, "һ�ֽ�Ǯһ�ֽ�������û���㹻���ֽ�", YEA);
		return;
	}
	update_health();
	myInfo->health -= 10;
	prints("\n    ������%s�Ƴ��ǹ������%s��С��ף��ۣ�˧���ˣ�",
	       CENTER_NAME, CENTER_NAME);
	pressanykey();

	nomoney_show_stat("�ĳ�����");
	move(4, 4);
	if (askyn("���ĳ���Σ�յģ������Ҫ������", YEA, NA) == NA) {
		myInfo->robExp -= 2;
		prints
		    ("\n    ����������ˣ���͵͵��װ���ӽ��˸���������Ͱ��\n    ��ĵ�ʶ���ͣ�");
		pressanykey();
		return;
	}

	myInfo->lastActiveTime = currtime;
	update_health();
	myInfo->WorkPoint -= 4500;
	myInfo->health -= 50;
	myInfo->luck -= 10;
	myInfo->Actived += 10;

	move(6, 4);
	prints
	    ("��һ���������ǹ�����ŶĿʹ󺰣���ι�������㣬û�����������𣿣���");
	pressanykey();

	sleep(1);

	move(8, 4);
	if (!(random() % 3)) {
		myInfo->robExp -= 10;
		prints
		    ("��Χ���˹�����Ц���񾭲������ð�ˮǹ���𰡣���\n    �������ڵ�ƭ�ˣ���ת����ܡ�\n    �����������\n    ��ĵ�ʶ����10�㡣");
		sprintf(buf,
			"    һ������ˮǹ����׵��񾭲��ڶĳ���쭺����ݡ�\n"
			"    ϣ����֪�������򾯷��ṩ�й���Ϣ��\n");
		deliverreport("�����š��񾭲��ĳ����", buf);
		pressreturn();
		return;
	}

	update_health();
	if (check_chance(myInfo->robExp, 600, myInfo->weapon, 5, 100, 0)) {
		num = MIN(MAX_MONEY_NUM / 20, mcEnv->prize777) / 2;
		myInfo->cash += num - 200000;
		mcEnv->prize777 -= (num - after_tax(200000));
		myInfo->begExp += 20;
		myInfo->robExp += 30;
		prints
		    ("\n    ������Χ����Ŀ�ɿڴ�֮�ʣ����ó�������777̨�ϵ�Ǯ����װ����\n    Ȼ��������һת���߳����š�\n    ��ĵ�ʶ���ӣ�\n    ��������ӣ�");
		sprintf(title, "�����š���������װ���ٶĳ�");
		sprintf(buf,
			"    ��վ�ո��յ�����Ϣ����Щʱ��һ��������װ���ٶĳ������ݡ�\n"
			"    Ŀǰ������ʽ���������¡�ϣ�����������ṩ���������\n");

	} else if (random() % 3) {
		myInfo->cash -= 200000;
		mcEnv->prize777 += after_tax(200000);
		myInfo->health = 0;
		myInfo->robExp += 30;
		prints
		    ("\n    ˢˢˢ����Χ����һ��ȫ����װ�ľ��죬ԭ��������͵õ����߱���cmft"
		     "\n    ������������ǹһ����ɨ,���ž��������ʱ���æ�ӳ��ĳ�"
		     "\n    �������ȫʧ�� \n    ��ĵ�ʶ������!\n");
		sprintf(title, "�����š������ͽ�ĳ�����");
		sprintf(buf,
			"    ��վ�ո��յ�����Ϣ�������������ȵõ����߱������%s�ĳ��������ڴ�ͽ�����������ͣ�δ�ܽ����ܻ�\n\n"
//                      "    Ŀǰ������ʽ���������¡�ϣ�����������ṩ���������\n"
			"    �����������ע%s���Ž�Ŀ\n"
			"    ���:%s�Ƴ��ǹ��Я�����㣬����һ�����Ӽ����Σ�ɱ�˷Ż�֮�ر���",
			CENTER_NAME, CENTER_NAME, CENTER_NAME);
	} else {
		myInfo->health = 0;
		myInfo->robExp -= 20;
		myInfo->luck -= 20;
		prints
		    ("\n    ˢˢˢ����Χ����һ��ȫ����װ�ľ��죬��ֻ�����־��ܣ�\n    ԭ��������͵õ����߱���cmft\n"
		     "    �������ȫʧ��\n    �����Ʒ���ͣ�\n    ��ĵ�ʶ����20��!\n");
		sprintf(title, "�����š������������ %s���־���",
			currentuser->userid);
		sprintf(buf,
			"    �����������ȵõ����߱������%s�ĳ�����%s��װ���ٵ�ʱ�򣬳ɹ������ܻ�\n"
			"    ����Ѷ�����з���%sΪ���񲡻��ߣ����������������ϣ�ȡ����ҽ��\n",
			CENTER_NAME, currentuser->userid, currentuser->userid);
	}
	deliverreport(title, buf);
	pressanykey();

	return;

}

void
stealbank()
{
	int num;
	struct userec *lookupuser;
	char uident[IDLEN + 1], buf[256], title[STRLEN], police[IDLEN + 1];
	mcUserInfo *mcuInfo;
	time_t currtime;

	if (check_health(90, 12, 4, "��ô�������û�г��������޷���ɡ�", YEA))
		return;
	if (myInfo->robExp < 100) {
		showAt(12, 4, "���±��¡���������ֶ���̫�����ˡ�����", YEA);
		return;
	}
	if (myInfo->begExp < 200) {
		showAt(12, 4, "ƾ�����ڵ������������ɣ���ȥ�����ɡ�����",
		       YEA);
		return;
	}
	if (myInfo->luck < 60) {
		showAt(12, 4, "��������������ؼ����鱨������", YEA);
		return;
	}

	currtime = time(NULL);
	if (myInfo->WorkPoint < 4500) {
		showAt(12, 4, "����ϵͳ�ոջ������㻹Ҫ����ʱ����Ϥһ�¡�",
		       YEA);
		return;
	}
	move(4, 4);
	if (askyn
	    ("��������ϵͳ��Ҫ������ԣ��������Ҫ20����ȷ��Ҫ����", NA,
	     NA) == NA) {
		myInfo->begExp--;
		prints("\n    ����������ˡ�\n    ��������ͣ�");
		return;
	}
	myInfo->health -= 10;

	if (myInfo->cash < 200000) {
		showAt(12, 4, "һ�ֽ�Ǯһ�ֽ�������û���㹻���ֽ�", YEA);
		return;
	}

	myInfo->lastActiveTime = currtime;
	update_health();
	myInfo->WorkPoint -= 4500;
	myInfo->health -= 50;
	myInfo->luck -= 10;
	myInfo->Actived += 10;
	myInfo->cash -= 200000;
	mcEnv->prize777 += after_tax(200000);
	move(5, 4);
	prints
	    ("�����ڵ���ǰ�棬���ݵĵ���һ���̣�Ȼ��������˼��¼��̡�\n    ��Ļ����ʾ����ʼ����%s����ϵͳ��",
	     CENTER_NAME);
	pressanykey();
	move(7, 4);
	prints("�������ӡ��������Ժ�");
	sleep(1);

	move(8, 4);
	if (!(random() % 3)) {
		myInfo->begExp -= 10;
		prints
		    ("��ལ����еĳ��������������������֣���Ͻ��Ͽ������ӡ����գ�\n    �����������\n    ���������10�㡣");
		sprintf(buf,
			"    ����������ոռ��ӵ���һ����������ϵͳ����Ϊ������ͼ��������������ʱ��\n"
			"    �����������ѡ�ϣ����֪�������򾯷��ṩ�й���Ϣ��\n");
		deliverreport("�����š�������ʿ��������ϵͳʧ��", buf);
		pressreturn();
		return;
	}
	prints
	    ("��ϲ�㣡�Ѿ��ɹ����ӵ�������ת��ϵͳ��\n    ����һ�λ���ӱ��˵��ʻ�ת�ʵ����ʻ���\n");
	usercomplete("��Ҫ��˭����ת�ʹ�������Enterȡ����", uident);
	if (uident[0] == '\0') {
		myInfo->robExp -= 10;
		prints
		    ("/n    �㵨�ӷ����ˡ�������\n    �����������\n    ��ĵ�ʶ����10�㡣");
		return;
	}
	if (!getuser(uident, &lookupuser)) {
		myInfo->begExp -= 10;
		showAt(12, 4,
		       "/n    ����һ������ˣ������ȿ��������ڿ�������\n    �����������\n    ���������10�㡣",
		       YEA);
		return;
	}
	if (!strcmp(uident, currentuser->userid)) {
		showAt(7, 4, "��Ŷ����ɽҽԺ��ӭ��������", YEA);
		myInfo->robExp -= 10;
		myInfo->begExp -= 10;
		myInfo->luck -= 10;
		return;
	}
	sethomefile(buf, uident, "mc.save");
	if (!file_exist(buf))
		initData(1, buf);
	if((mcuInfo = loadData(buf, sizeof (mcUserInfo))) == (void*)-1)
		return;

	if (mcuInfo->GetLetter == 0) {
		clear();
		showAt(6, 4, "�˼Ҳ�Ը������棬�ǲ��𣬶㻹���а���", YEA);
		unloadData(mcuInfo, sizeof (mcUserInfo));
		return;
	}

/*	sprintf(buf, "Ҫ��%s���ʻ�ת����", uident);
	num = userInputValue(12, 4, buf, MONEY_NAME, 0, num);
	if ((num > (myInfo->credit + myInfo->cash) * 5)
	    || (myInfo->begExp * 5 + 50 < mcuInfo->begExp))
		if (random() % 4) {
			sleep(1);
			goto FAILED;
		}
*/
	sleep(1);
	num = MIN(MIN(MAX_MONEY_NUM / 20, mcuInfo->credit / 5), myInfo->credit);
	if (check_chance
	    (myInfo->begExp, mcuInfo->begExp, myInfo->weapon, mcuInfo->armor,
	     200, 0)) {
		mcuInfo->credit -= num;
		myInfo->credit += after_tax(num);
		myInfo->begExp += 25;
		myInfo->robExp += 20;
		myInfo->health = 0;
		if ((mcuInfo->robExp + mcuInfo->begExp < 100)
		    && (mcuInfo->begExp * 100 + 500 < myInfo->begExp)) {
			prints
			    ("\n    ����������ᣬ��Ȼӵ�о޶���Ƹ�����е�ʮ�ּ���!");
			if (askyn("\n	һ���������ݣ����һ����ɢ��", NA, NA)
			    == YEA) {
				prints
				    ("\n    �ҵò�������Ҳ����õ����������֮ǰ�������˻�������һ��ľ��"
				     "\n    �����Ʒ�½�20��!");
				myInfo->luck -= 20;
				mcEnv->Treasury += mcuInfo->credit;
				num += mcuInfo->credit;
				mcuInfo->credit = 0;
				if (readstrvalue
				    (MC_BOSS_FILE, "police", police,
				     IDLEN + 1) != 0)
					police[0] = '\0';
				sprintf(title, "ؤ��������Ϣ");
				sprintf(buf,
					"    %s���ұ�%s���������ʻ�����ֲ����ľ��",
					uident, currentuser->userid);
				system_mail_buf(buf, strlen(buf), police, title,
						"deliver");
			}
		}
		sprintf(buf,
			"�ϴ�ȥ���в�ѯ����������%d�Ĵ����о�Ȼ˵�ҼǴ��ˣ�kao��"
			"\n�ϴ�%s�����ˣ��Դ�������������ϵͳ����֪���ǲ��Ǻ����й�ϵ��",
			num, currentuser->userid);
		if (mcuInfo->GetLetter == 1)
			system_mail_buf(buf, strlen(buf), uident, "�ռ�",
					"deliver");
		prints("\n    ��ϲ�㣡ת�ʳɹ���\n    ��ĵ�ʶ���ӣ�\n"
		       "    ��������ӣ�\n    ��������ľ���\n");
		sprintf(title, "��ҥ�ԡ�����������������ת��ϵͳ");
		sprintf(buf,
			"    �ݷ���������ҥ�ԣ����˳ɹ�����������ϵͳ��\n    �����еĳ�����������޼��ӵ�������Ϊ��\n"
			"    Ŀǰ�������ڵ������Ϣ����ʵ�ȡ����λ��ע�Լ����ʻ�����ʱ�ṩ�����Ϣ��\n");
	} else if (random() % 3) {
		myInfo->health = 0;
		myInfo->begExp += 25;
		prints
		    ("    ��ལ��������������Ѳ�������ˣ���ξͷŹ�����,������־͵����ְɡ�\n"
		     "    ����������㣡\n    �����������!\n");
		sprintf(title, "�����š�����������ͼ��������ϵͳ");
		sprintf(buf,
			"    �����ոսӵ�����Ϣ���ڽ����賿������һ����������ͼ����%s����ϵͳ,�������簲ȫ�̾��ɹ���ء�\n"
			"    �ڴˣ�����%s����ϵͳ֣�����������û���ŵ,���ǵ�ϵͳ�Ǿ��԰�ȫ�ɿ��ġ�\n",
			CENTER_NAME, CENTER_NAME);
	} else {
		myInfo->health = 0;
		myInfo->begExp -= 20;
		myInfo->luck = MAX(-100, myInfo->luck - 20);
		num = MIN(MAX_MONEY_NUM / 2, myInfo->credit / 5);
		mcuInfo->credit += after_tax(num / 2);
		mcEnv->prize777 += num / 2;
		myInfo->credit -= num;
		prints
		    ("    ������������ӵ�����Ŀ����ж����㱻ץס�ˣ�\n    ����������㣡\n"
		     "    �����Ʒ����20�㣡\n    ���������20��!\n    �㱻����%d%s!",
		     num, MONEY_NAME);
		sprintf(title, "�����š�ĳ����������ϵͳ��ץ��");
		sprintf(buf,
			"    ����������ոռ��ӵ���ĳ����������ϵͳ����Ϊ�����ɹ�����������\n"
			"    ����������̬�ȽϺã������β����˵�ԭ�򣬲���������������%d%s!\n"
			"    ����һ����ܺ�����Ϊѹ���ѣ�һ��ע��777��    \n    �������Ϊ�䣡\n",
			num, MONEY_NAME);
	}
	deliverreport(title, buf);
	unloadData(mcuInfo, sizeof (mcUserInfo));
	pressanykey();

	return;

}

int
RobPeople(int type)
{
	int transcash, transcredit, transrob, transbeg;
	time_t currtime;
	char uident[IDLEN + 1], buf[256], title[70], content[256],
	    police[IDLEN + 1];
	mcUserInfo *mcuInfo;

	switch (type) {
	case 0:
		showAt(4, 4, "�ڰ��Ա����Ҫ�޶�����", YEA);
		if (!getOkUser("������˭��", uident, 5, 4)) {
			showAt(7, 4, "���޴���", YEA);
			return 0;
		}
		if (!strcmp(uident, currentuser->userid)) {
			showAt(7, 4, "ţħ���������š��������񾭲�������", YEA);
			return 0;
		}
		move(6, 4);
		if (askyn("��ȷ��Ҫ�������", NA, NA) == NA) {
			showAt(7, 4, "ʲô���㻹û��ã����������������", YEA);
			return 0;
		}
		update_health();
		if (check_health(100, 7, 4, "��û����ô��������ܱ��ˡ�", YEA)) {
			return 0;
		}
		if (myInfo->robExp < 1000 || myInfo->begExp < 500) {
			showAt(7, 4, "��û���㹻�ľ���������ô������顣", YEA);
			return 0;
		}
		if (myInfo->luck < 80) {
			showAt(7, 4,
			       "��������ǰ�ƣ�����ʱ�̼������Ļ����û���ж�",
			       YEA);
			return 0;
		}
		if (myInfo->cash < 300000) {
			showAt(7, 4,
			       "���ڸɵ����ף���ɶ����ҪǮ�������ֿտ��ܸ��",
			       YEA);
			return 0;
		}
		currtime = time(NULL);
		if (myInfo->WorkPoint < 9000) {
			showAt(7, 4,
			       "������˸����ӣ����Ǽ������һ�·����ɡ�",
			       YEA);
			return 0;
		}
		showAt(7, 4, "�㿴���о���������ס������Ѳ�ߡ�", YEA);
		myInfo->lastActiveTime = currtime;
		myInfo->WorkPoint -= 9000;
		myInfo->luck -= 10;
		if (random() % 2) {
			showAt(8, 4, "�㱻��ȡ���˰���ж���", YEA);
			sprintf(title, "���ڰ�����ڲ߻�����ж�");
			sprintf(content, "    ���Ҫ���С�ġ�");
			deliverreport(title, content);
			return 0;
		}
		if (askyn("�㻹Ҫ�������а���ж���", NA, NA) == NA) {
			showAt(8, 4, "�㱻��ȡ���˰���ж���", YEA);
			myInfo->luck -= 10;
			myInfo->health -= 50;
			sprintf(title, "���ڰ����Ԥı��ܱ���");
			sprintf(content,
				"    �ҿ�����" MY_BBS_NAME
				"����©�˷������ڴ�ҵĽ̻�������а�����ˡ�");
			deliverreport(title, content);
			return 0;
		}
		myInfo->Actived += 20;
		showAt(8, 4,
		       "��͵͵����300000Ԫ���ſڵľ��죬Ȼ����ĵ����˽�ȥ��",
		       YEA);
		myInfo->cash -= 300000;
		mcEnv->prize777 += after_tax(300000);
		sethomefile(buf, uident, "mc.save");
		if (!file_exist(buf))
			initData(1, buf);
		if((mcuInfo = loadData(buf, sizeof (mcUserInfo))) == (void*)-1)
			return 0;
		if (mcuInfo->GetLetter == 0) {
			clear();
			showAt(6, 4, "�˼Ҳ�Ը������棬�ǲ��𣬶㻹���а���",
			       YEA);
			unloadData(mcuInfo, sizeof (mcUserInfo));
			return 0;
		}

		if (check_chance
		    (myInfo->robExp, mcuInfo->robExp, myInfo->weapon,
		     mcuInfo->armor, -200, 0)) {
			if (random() % 3) {
				myInfo->robExp += 30;
				myInfo->begExp += 30;
				myInfo->health = 0;
				move(10, 0);
				prints
				    ("    ����ȥ�Ժ�һ�����㵹��һ��������190cm�Ĵ��ͷ��\n"
				     "    ������������Ǵ�������õ�����ǹ��\n"
				     "    ���ˣ�Ǯ������ȥ�ˣ�ƴ�˰�\n"
				     "    һ�����ҵĲ�����\n"
				     "    ���ã����񾪶������ˣ��ܰ�!\n"
				     "    ��ĵ�ʶ��������! ��������ľ�!");
				sprintf(title, "���ڰ %s���ʧ��",
					currentuser->userid);
				sprintf(content,
					"    �ڰ��Ա%s��ͼ������ˣ��ҵõ����߷���������δ�ܵó�",
					currentuser->userid);
				deliverreport(title, content);
				sprintf(title, "%s��ͼ�����",
					currentuser->userid);
				sprintf(content,
					"   ��������������ǿ׳����Ҳ���ǳ��صġ������ܣ��ٵȼ���ɣ�");
				if (mcuInfo->GetLetter == 1)
					system_mail_buf(content,
							strlen(content), uident,
							title,
							currentuser->userid);
				unloadData(mcuInfo, sizeof (mcUserInfo));
				pressanykey();
				return 0;
			} else {
				myInfo->robExp -= 30;
				myInfo->begExp -= 30;
				myInfo->health = 0;
				myInfo->luck -= 20;
				myInfo->mutex = 0;
				myInfo->freeTime = time(NULL) + 10800;
//                      myInfo->credit += myInfo->cash;
//                      myInfo->cash = 0;
				move(10, 0);
				prints
				    ("    �㱻����ץס�ˣ��ſ��Ǽһ��Ȼ�Ǹ��ٻ�����\n"
				     "    ��ĵ�ʶ��������30�㣡\n"
				     "    ���������Ϊ0��\n"
				     "    �����Ʒ�½�20�㣡\n"
				     "    �㱻�����Ѻ3Сʱ��\n"
				     "    ��͵�����ɷ�ʴ���ף������������ᰡ��");
				sprintf(title, "���ڰ %s���ʧ��",
					currentuser->userid);
				sprintf(content,
					"    �ڰ��Ա%s��ͼ������ˣ��Һñ����켰ʱ���֡�\n"
					"    ���������Ѻ3Сʱ��",
					currentuser->userid);
				deliverreport(title, content);
				unloadData(myInfo, sizeof (mcUserInfo));
				unloadData(mcEnv, sizeof (MC_Env));
				pressanykey();
				Q_Goodbye();
			}
		}
		myInfo->robExp += 50;
		myInfo->begExp += 50;
		myInfo->luck -= 10;
		transcash = MIN(myInfo->cash * 2, mcuInfo->cash);
		transcredit = MIN(myInfo->credit, mcuInfo->credit / 4);
		mcuInfo->cash -= transcash;
		mcuInfo->credit -= transcredit;
		myInfo->cash += after_tax(transcash);
		myInfo->credit += after_tax(transcredit);
		myInfo->health = 0;
		move(9, 0);
		prints("    ���ܳɹ���\n"
		       "    ��ĵ�ʶ��������50�㣡   �����Ʒ�½�10�㣡\n"
		       "    %s���ϵ��ֽ���ϴ��һ�գ����˻��ó���%d%sΪ������\n"
		       "    ��������ô����£��������ˡ�", uident, transcredit,
		       MONEY_NAME);
		if ((mcuInfo->robExp + mcuInfo->begExp < 100)
		    && (mcuInfo->robExp * 100 + 500 < myInfo->robExp)) {
			if (askyn
			    ("\n 	��һ��Ѿ������ҵ����ˣ�����Ǯ�ѵ��֣�ɱ�����ˣ�",
			     NA, NA) == YEA) {
				prints
				    ("\n    ��Ȼ�õ�����𣬵����ĺ��������㻹�Ǿ���һ�����������ݣ�˺Ʊ��!"
				     "\n    �����Ʒ�½�20��!");
				myInfo->luck -= 20;
				mcEnv->Treasury += mcuInfo->credit;
				mcuInfo->credit = 0;
				sprintf(title, "���ڰ �ڰ�б�����");
				sprintf(content,
					"    %s���ұ��ڰ��ܣ��䰴�ڽ�����������Ա�˺Ʊ��\n"
					"    %s����Դ˶����¼�����߶����ӣ��ѳ���ר���飬�����ư���",
					uident, CENTER_NAME);
				deliverreport(title, content);
				sprintf(title, "�����˺Ʊ!");
				sprintf(content,
					"    �㲻�ұ��ڰ��ܣ�����������Ժ��Ա�˺Ʊ�������Ŀ��!\n    ���Ҳ���Ź���Щ���һ�!!!");
				if (mcuInfo->GetLetter == 1)
					system_mail_buf(content,
							strlen(content), uident,
							title,
							currentuser->userid);
				if (readstrvalue
				    (MC_BOSS_FILE, "police", police,
				     IDLEN + 1) != 0)
					police[0] = '\0';
				sprintf(title, "�ڰ�������Ϣ");
				sprintf(content,
					"    %s���ұ�%s��ܣ�����������Ժ��Ա�˺Ʊ��",
					uident, currentuser->userid);
				system_mail_buf(content, strlen(content),
						police, title, "deliver");
				unloadData(mcuInfo, sizeof (mcUserInfo));
				pressanykey();
				return 1;
			}
		}
		sprintf(title, "���ڰ �ڰ���ʵʩһ�ΰ���ж�");
		sprintf(content,
			"    %s���ұ��ڰ��ܣ������˲Ʋ���1/4��Ϊ����Ѳ��ػ����ɡ�",
			uident);
		deliverreport(title, content);
		sprintf(title, "�㱻�ڰ��ܣ�");
		sprintf(content, "    �㲻�ұ��ڰ��ܣ�����%d�������",
			transcash + transcredit);
		if (mcuInfo->GetLetter == 1)
			system_mail_buf(content, strlen(content), uident, title,
					currentuser->userid);
		unloadData(mcuInfo, sizeof (mcUserInfo));
		pressanykey();
		break;
	case 1:
		showAt(4, 4,
		       "ؤ�������������ѹ�������������Ǵ������Ǻÿ��ģ�",
		       YEA);
		if (!getOkUser("��Ҫ��˭���֣�", uident, 5, 4)) {
			showAt(7, 4, "���޴���", YEA);
			return 0;
		}
		if (!strcmp(uident, currentuser->userid)) {
			showAt(7, 4, "ţħ���������š��������񾭲�������", YEA);
			return 0;
		}
		move(6, 4);
		if (askyn("��ȷ��Ҫ����ʩչ���Ǵ���", NA, NA) == NA) {
			showAt(7, 4, "Ŷ���㻹���������֣����ĳ���Ӳ�������ɡ�",
			       YEA);
			return 0;
		}
		update_health();
		if (check_health
		    (110, 7, 4, "��û����ô������ʩչ���Ǵ󷨡�", YEA)) {
			return 0;
		}
		if (myInfo->robExp < 3000 || myInfo->begExp < 5000) {
			showAt(7, 4, "����񹦻�����򣬼��������ɡ�", YEA);
			return 0;
		}
		if (myInfo->luck < 85) {
			showAt(7, 4,
			       "�������Ӽ��߰ߣ���������ӽ�����û�����֡�",
			       YEA);
			return 0;
		}
		currtime = time(NULL);
		if (myInfo->WorkPoint < 12000) {
			showAt(7, 4, "�㻹��Ҫ�����˹�����ʩչ���Ǵ󷨡�", YEA);
			return 0;
		}
		showAt(7, 4, "��о�����������Ϣ���ѵ��������˽�������������",
		       YEA);
		myInfo->lastActiveTime = currtime;
		myInfo->WorkPoint -= 12000;
		myInfo->luck -= 10;
		if (random() % 2) {
			showAt(8, 4, "�������ʩչ���Ǵ󷨵���ͷ��", YEA);
			sprintf(title, "��ؤ��������������Ǵ�");
			sprintf(content, "    ���Ҫ���С�ģ���Ҫ���˵���");
			deliverreport(title, content);
			return 0;
		}
		if (askyn("�㻹Ҫ�ٳ���һ����", NA, NA) == NA) {
			showAt(8, 4, "�������ʩչ���Ǵ󷨵���ͷ��", YEA);
			myInfo->luck -= 20;
			myInfo->health -= 50;
			sprintf(title, "��ؤ�������ʩչ���Ǵ�");
			sprintf(content,
				"    �ҿ�����������û�м��������캦����¡�");
			deliverreport(title, content);
			return 0;
		}
		myInfo->Actived += 30;
		sethomefile(buf, uident, "mc.save");
		if (!file_exist(buf))
			initData(1, buf);
		if((mcuInfo = loadData(buf, sizeof (mcUserInfo))) == (void*)-1)
			return 0;

		if (mcuInfo->GetLetter == 0) {
			clear();
			showAt(6, 4, "�˼Ҳ�Ը������棬�ǲ��𣬶㻹���а���",
			       YEA);
			unloadData(mcuInfo, sizeof (mcUserInfo));
			return 0;
		}

		if (mcuInfo->BeIn == 2) {
			showAt(7, 4, "��������Ͻ��ڶ�", YEA);
			unloadData(mcuInfo, sizeof (mcUserInfo));
			return 0;
		}

		if (!(seek_in_file(DIR_MC "policemen", uident)
		      || (mcuInfo->BeIn == 1))) {
			showAt(7, 4, "�г𱨳�ؤ���Ա��Թ������", YEA);
			unloadData(mcuInfo, sizeof (mcUserInfo));
			return 0;
		}

		if (mcuInfo->robExp + mcuInfo->begExp < 6000) {
			prints
			    ("    %s����һЦ������ʤ���С��������������δ�������"
			     "    �㱻������Ѫ����!", uident);
			myInfo->luck -= 20;
			myInfo->health = 0;
			sprintf(title, "��ؤ�����ʩչ�����Ǵ�");
			sprintf(content,
				"    �ҿ��ܺ��߹������δ��������غ����");
			deliverreport(title, content);
			unloadData(mcuInfo, sizeof (mcUserInfo));
			pressanykey();
			return 0;
		}

		if (check_chance
		    (myInfo->begExp, mcuInfo->begExp, myInfo->weapon,
		     mcuInfo->armor, -300, -50)) {
			transrob = MIN(myInfo->robExp / 100, mcuInfo->robExp);
			transbeg = MIN(myInfo->begExp / 100, mcuInfo->begExp);
			myInfo->robExp -= transrob;
			mcuInfo->robExp += transrob;
			myInfo->begExp -= transbeg;
			mcuInfo->begExp += transbeg;
			myInfo->luck -= 30;
			myInfo->health = 0;
			//myInfo->lastActiveTime = time(NULL) + 7200;
			myInfo->WorkPoint -= 7200;
			move(9, 0);
			prints
			    ("    ����!���˾�Ȼ������ϳ˵ı�ڤ�񹦣���Ĺ�������ԴԴ���ϵı�����ȥ��\n"
			     "    ��ĵ�ʶ�½�%d�㣬������½�%d�㡣\n"
			     "    ���������Ϊ0��\n" "    �����Ʒ�½�20�㣡\n"
			     "    ��͵�����ɷ�ʴ���ף������������ᰡ��",
			     transrob, transbeg);
			sprintf(title, "��ؤ� %sʩչ���Ǵ�ʧ��",
				currentuser->userid);
			sprintf(content,
				"    ؤ���Ա%s��ͼʹ�����Ǵ���ȡ���˹���������������˷������ˡ�\n",
				currentuser->userid);
			deliverreport(title, content);
			sprintf(title, "%s��ͼ����ʹ�����Ǵ󷨣�",
				currentuser->userid);
			sprintf(content,
				"    �ҿ������������˸�Ϊ����ı�ڤ�񹦣���������%d�ĵ�ʶ��%d����",
				transrob, transbeg);
			if (mcuInfo->GetLetter == 1)
				system_mail_buf(content, strlen(content),
						uident, title,
						currentuser->userid);
			unloadData(mcuInfo, sizeof (mcUserInfo));
			pressanykey();
			return 0;
		}
		transrob = MIN(myInfo->robExp, mcuInfo->robExp / 50);
		transbeg = MIN(myInfo->begExp, mcuInfo->begExp / 50);
		myInfo->robExp += transrob;
		mcuInfo->robExp -= transrob;
		myInfo->begExp += transbeg;
		mcuInfo->begExp -= transbeg;
		myInfo->luck -= 20;
		myInfo->health = 0;
		move(9, 0);
		prints("    ��ʩչ���Ǵ󷨳ɹ���\n"
		       "    ��ĵ�ʶ����%d�㣬������%d�㣡\n"
		       "    �����Ʒ�½�20�㣡\n"
		       "    ��������ô����£��������ˡ�", transrob, transbeg);
//              unloadData(mcuInfo, sizeof (mcUserInfo));
		sprintf(title, "��ؤ� %s���ҳ�Ϊ���Ǵ󷨵��ܺ���", uident);
		sprintf(content, "%s�����Ǵ�͵Ϯ��������ʧ��2%%��", uident);
		deliverreport(title, content);
		sprintf(title, "�㱻���Ǵ����У�");
		sprintf(content,
			"    �㲻�ұ���ʩ�����Ǵ󷨣���ʧ��%d�ĵ�ʶ��%d����",
			transrob, transbeg);
		if (mcuInfo->GetLetter == 1)
			system_mail_buf(content, strlen(content), uident, title,
					currentuser->userid);
		unloadData(mcuInfo, sizeof (mcUserInfo));
		pressanykey();
		break;
	}
	return 1;
}

int
money_robber()
{
	int ch, quit = 0, tempMoney, day, hour, minute;
	time_t ActiveTime, currTime;
	char uident[IDLEN + 1];
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

	if (!(myInfo->GetLetter == 1)) {
		clear();
		showAt(5, 4, "���Ѿ��ر��˽���������Ϸ���ܣ��뿪����������",
		       YEA);
		return 0;
	}

	while (!quit) {
		money_show_stat("�ڰ��ܲ�");
		move(4, 4);
		prints
		    ("����ǰ��%s�ڰ��޶���������һʱ���������ϴ����һ��ʱ�������伣��"
		     "\n    ����������𽥻�Ծ�����������ֶ�Ҳ�������λ���������",
		     CENTER_NAME);
		move(7, 4);
		prints
		    ("һ���������߹���С��˵����Ҫ��שô�����˺��۵ġ��ĺ��˻��ܳ���ʶ������");
		move(t_lines - 1, 0);
#if 0
		prints
		    ("\033[1;44m ѡ�� \033[1;46m [1]��ש [2]�������� [3]�ڰ� [4] �������� [Q]�뿪\033[m");
#else
		prints
		    ("\033[1;44m ѡ�� \033[1;46m [1]��ש [2]�ڰ� [3] �������� [4] �������� [Q]�뿪\033[m");
#endif
		ch = igetkey();
		switch (ch) {
		case '1':
			money_pat();
			break;
#if 0
		case '2':
			money_show_stat("�ڰﱣ�����շѴ�");
			move(6, 4);
			prints
			    ("��ν�������֣����˱����ѣ����ܱ�һ��ʱ��ƽ�������ںڰ��ж�����֮�ڡ�");
			showAt(12, 4, "\033[1;32m���ڲ߻��С�\033[m", YEA);
			break;
#endif
		case '2':
			if (!clubtest(ROBUNION)) {
				move(12, 4);
				prints("�㲻�Ǻڰ�ģ���������������");
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
			if (!(myInfo->BeIn == 1)) {
				myInfo->weapon = 0;
				myInfo->armor = 0;
				myInfo->BeIn = 1;
			}
			while (!quit) {
				ActiveTime = myInfo->lastActiveTime;
				currTime = time(NULL);
				money_show_stat("�ڰ�");
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
					    ("    �����ռǱ������ϴθɻ����Ѿ���%d��%dСʱ%d������ǰ�������ˡ�\n\n"
					     "    ��Ϊ�ڰ��Ա�����캦���޶�������ı�ְ����������ô��˷����Լ��أ�",
					     day, hour, minute);
				} else {
					move(9, 1);
					prints
					    ("    �����ϴκڰ��У����˴������ˣ�����֪��ʲôʱ�������ء�");
				}
				move(13, 1);
				prints("    ������ض�\033[1;31m  %d\033[m %%",
				       (30000 - myInfo->WorkPoint) / 300);
				move(15, 1);
				prints
				    ("    ���䱸�˻���ǿ���\033[1;33m%s\033[m, ��ʻ�������\033[1;33m%s\033[m, Ħȭ���ƣ�׼�����һ����",
				     RobWeapon[myInfo->weapon],
				     RobArmor[myInfo->armor]);
				move(t_lines - 1, 0);
				prints
				    ("\033[1;44m ѡ�� \033[1;46m [1]���� [2]���� [3]�ٶĳ� [4]��Ʊ [5]�� [6]���� [9]����� [Q]�뿪\033[m");
				ch = igetkey();
				switch (ch) {
				case '1':
					money_show_stat("������");
					forceGetMoney(0);
					break;
				case '2':
					money_show_stat("���컯��");
					forceGetMoney(2);
					break;
				case '3':
					money_show_stat("�ĳ�");
					RobShop();
					break;
				case '4':
					money_show_stat("����");
					RobPeople(0);
					//showAt(12, 4,"����������������˭��˭����",YEA);
					break;
				case '5':
					Rob_Fight();
					break;
				case '6':
					Rob_Rebellion();
					break;
				case '9':
					EquipShop(1);
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
			whoTakeCharge(4, uident);
			if (strcmp(currentuser->userid, uident))
				break;
			while (!quit) {
				nomoney_show_stat("�ڰ����");
				move(t_lines - 1, 0);
				prints
				    ("\033[1;44m ѡ�� \033[1;46m [1]������� [2]�ڳԺ� [3]�鿴�����ʲ� [Q]�뿪\033[m");
				ch = igetkey();
				switch (ch) {
				case '1':
					nomoney_show_stat("�����");
					showAt(12, 4,
					       "\033[1;32m���ڽ����С�\033[m",
					       YEA);
					break;
				case '2':
					money_show_stat("�ڳԺ�");
					showAt(12, 4,
					       "\033[1;32m���������С�\033[m",
					       YEA);
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
			money_show_stat("�����ѽ��ѵ�");
			move(12, 4);
			if (askyn
			    ("\033[1;32m����ڰﵽ����٣���Ҫ��Ҫ���������㣿\033[m",
			     YEA, NA) == YEA) {
				tempMoney =
				    userInputValue(13, 4, "Ҫ����", "��", 5,
						   100) * 10000;
				if (tempMoney < 0)
					break;
				if (myInfo->cash < tempMoney) {
					showAt(15, 4,
					       "\033[1;37m�㵨��Ϸˣ�ڰ�ǲ��ǻ�Ĳ��ͷ��ˣ�\033[m",
					       YEA);
					break;
				}
				update_health();
				if (check_health
				    (1, 15, 4, "������������ˣ�", YEA))
					break;
				move(15, 4);
				prints("\033[1;37m�㽻��%d%s������\n"
				       "    ��ĵ�ʶ�ƺ��������ӣ�\033[m",
				       tempMoney, MONEY_NAME);
				myInfo->health--;
				myInfo->Actived += tempMoney / 25000;
				myInfo->cash -= tempMoney;
				myInfo->robExp +=
				    (tempMoney - 30000) / (30000 +
							   (random() % 30000)) +
				    1;
				mcEnv->Treasury += tempMoney;
				pressanykey();
			} else {
				showAt(14, 4,
				       "\033[1;33m�����˴�ٿɱ�˵��û������㣡\033[m",
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

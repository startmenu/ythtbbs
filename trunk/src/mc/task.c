#include "mc.h"
//-------------------------ð���߹���------------------------------//

int
kill_dragon()
{
	int currTime, num;
	char buf[256], title[STRLEN];

	nomoney_show_stat("ɽ�����");
	currTime = time(NULL);
	if (myInfo->WorkPoint < 9000) {
		showAt(12, 4, "����Ϊ��ض���ħ�ް�����������˶��Թ���!", YEA);
		return 0;
	}
	if (myInfo->health < 50) {
		showAt(12, 4,
		       "׷��ħ����һ���������飬�������û˯�ã���ô�������㹻��������",
		       YEA);
		return 0;
	}
	if ((myInfo->robExp < 1240) || (myInfo->begExp < 1250)) {
		showAt(12, 4,
		       "������ֵܣ������Ҵ���㣬�㻹�Ǳ�ȥ��ħ�޵�ʳ����",
		       YEA);
		return 0;
	}
	if (myInfo->luck < 80) {
		showAt(12, 4, "���᲻������ȥ�������ħ����һ��ģ�", YEA);
		return 0;
	}

	myInfo->lastActiveTime = currTime;
	update_health();
	myInfo->WorkPoint -= 9000;
	myInfo->health -= 40;
	myInfo->Actived += 10;
	move(6, 1);
	prints
	    ("��˳��ħ�����µĸ�����˿����һ·Ѱ�ң������ҵ���һ�����ص�ɽ��");
	move(7, 1);
	if (askyn("���������������ħ�޿��Ǻ����͵İ���", NA, NA) == YEA) {
		move(9, 1);
		prints
		    ("	ССħ�������ֵ�����������һ˼���ͳ����ɽ��");
		sleep(1);
		if (check_chance
		    (myInfo->robExp + myInfo->begExp, 6000,
		     myInfo->weapon, 7, 200, 0)) {
			num = MAX(MIN(800000, mcEnv->Treasury - 20000000), 0);
			move(11, 1);
			prints
			    ("	��Ȼħ�޺�ǿ�󣬵�����ô���ܺ�����ymsw,yjxs,yslf�������࿹���أ�\n"
			     "	����һ�����ҵĲ�����������������ħ�ޡ��� һֻʩ��ķ\n"
			     "	��ȥ�����ʱ���˵���������ʩ��ķ�ɣ�����\n"
			     "	�����ͽ�%dԪ!��ĵ�ʶ����������!�����Ʒ������!",
			     num);
			myInfo->cash += num;
			mcEnv->Treasury -= num;
			myInfo->robExp += 30;
			myInfo->begExp += 30;
			myInfo->luck += 20;
			sprintf(title, "�����᡿Ӣ����������");
			sprintf(buf,
				"ð����%s����ԡѪ��ս���ɹ��������ڵ���Ϊ������Ķ���!\n\n"
				"������һ����Ӣ�۵���Ϊ��ʾ�¾�!",
				currentuser->userid);
			deliverreport(title, buf);
			pressanykey();
			return 1;
		} else {
			move(11, 1);
			prints
			    ("	���ɽ��֮�������ҵķ�����һ˫ĥ�̴���۾��������㿴��! ħ��!\n"
			     "	˭tmd �ĸ������⸽��ֻ��һЩСħ�޵ģ����Ӱ�!!\n"
			     "	����ħ����Ź�����������\n"
			     "	��������˵��ӳ���ɽ�ȣ�������Ҳ���������ط���!\n"
			     "	��ĵ�ʶ�����½���!");
			myInfo->robExp -= 20;
			myInfo->begExp -= 20;
			myInfo->WorkPoint -= 3600;
			sprintf(title, "�����᡿ħ�������ڴ�Ӣ������");
			sprintf(buf,
				"%s�����ð���Ŷ��⵽������Ϯ������ʧ����\n"
				"Ȩ����ʿ�Դ��¼��������ۣ�ϣ�����и����Ӣ�۳��֣��������!",
				currentuser->userid);
			deliverreport(title, buf);
			pressanykey();
			return 0;
		}
	} else {
		num = MAX(MIN(500000, mcEnv->Treasury - 20000000), 0);
		move(9, 1);
		prints
		    ("	��������˼�����㻹�Ƿ����˼���Ѱ�ҵ���ͷ���������Ѿ����ֵ���Ϣ���ع���\n"
		     "	������������Ϣ������ħ��\n"
		     "	�����ͽ�%dԪ�������Ʒ������!", num);
		myInfo->cash += num;
		mcEnv->Treasury -= num;
		myInfo->luck += 10;
		sprintf(title, "�����᡿�ͽ����������湦");
		sprintf(buf,
			"����%s���鱨������������Ϊ���ѾõĶ������ش˼ν�",
			currentuser->userid);
		if (!(random() % 10))
			deliverreport(title, buf);
		pressanykey();
		return 1;
	}
	return 0;
}

int
ghoul()
{
	int currTime, num;
	char buf[256], title[STRLEN];

	nomoney_show_stat("�����");
	currTime = time(NULL);
	if (myInfo->WorkPoint < 12000) {
		showAt(12, 4, "��Ĺ������Ĺ���ڵĲ���ˣ������Ұ�", YEA);
		return 0;
	}
	if (myInfo->health < 80) {
		showAt(12, 4, "��̽�գ��������Σ���Ϣ������ȥ", YEA);
		return 0;
	}
	if ((myInfo->robExp < 5000) || (myInfo->begExp < 5000)) {
		showAt(12, 4,
		       "���ڻ����Ҵ������Ĺ�Ĺ��Ȼ������֮�����㻹��ʡʡ��",
		       YEA);
		return 0;
	}
	if (myInfo->luck < 80) {
		showAt(12, 4, "���������鱨�Ǽٵģ����ǲ������㣡", YEA);
		return 0;
	}
	myInfo->lastActiveTime = currTime;
	update_health();
	myInfo->WorkPoint -= 12000;
	myInfo->health -= 60;
	myInfo->Actived += 20;
	move(6, 1);
	prints("���ݹ����ṩ���鱨����˳�����ҵ��˹�Ĺ");
	sleep(1);
	move(7, 1);
	if (askyn
	    ("�ڹ�Ĺ��һ�������۵Ľ�����㷢����һ�����ε�ͨ��������̽����",
	     NA, NA) == NA) {
		num = MAX(MIN(1000000, mcEnv->Treasury - 20000000), 0);
		move(9, 1);
		prints
		    ("	��������˼�����㻹�Ƿ����˼���Ѱ�ҵ���ͷ���������Ѿ����ֵ���Ϣ���ع���\n"
		     "	������������Ϣ�����˹�Ĺ\n"
		     "	�����ͽ�%dԪ�������Ʒ������!", num);
		myInfo->cash += num;
		mcEnv->Treasury -= num;
		myInfo->luck += 15;
		pressanykey();
		return 1;
	}
	move(9, 1);
	prints("	���뻢Ѩ���ɵû��ӣ������������ǰ��");
	sleep(1);
	if (check_chance
	    (myInfo->robExp + myInfo->begExp, 15000, myInfo->weapon, 7, 300,
	     0)) {
		move(11, 1);
		prints
		    ("	��Ȼ���������Ĺ�з��ؾ�ʯ���㾪���ˣ�\n"
		     "	�������ͻ�����������±��һ�Ѷѵ�ʯͷ\n"
		     "	��ĵ�ʶ����������\n");
		myInfo->robExp += 50;
		myInfo->begExp += 50;
		pressanykey();
		move(14, 1);
		if (askyn
		    ("Խ����ߣ����ص�����Խǿ���㽥����Щ�������ģ���������ǰ��?",
		     NA, NA) == YEA) {
			if (check_chance
			    (myInfo->robExp +
			     myInfo->begExp, 25000,
			     myInfo->armor, 10, 300, -100)) {
				num =
				    MAX(MIN
					(3000000,
					 mcEnv->Treasury - 20000000), 0);
				move(15, 1);
				prints
				    ("    ��־���ʤ�������ڣ����һ����ʯ��������Ľ��£�\n"
				     "    ��Ĺ�ı����Ѿ�ȫ��չ���������ǰ�������Ļƽ��鱦����\n"
				     "    �����˼�ֵ��������鱦��");
				myInfo->cash += num;
				mcEnv->Treasury -= num;
				sprintf(title, "�����᡿ð�������ض���");
				sprintf(buf,
					"ð����%s�ڹ�Ĺ�з�����˼�ֵ��������鱦��",
					currentuser->userid);
				deliverreport(title, buf);
				pressanykey();
				return 1;
			}
		} else {
			move(17, 1);
			prints
			    ("    ��������˼�����㻹�Ǿ������˳�����\n"
			     "    ������ɽ�ڣ�����û���գ��һ�������ģ�");
			sprintf(title, "�����᡿��Ĺ����ƻ�");
			sprintf(buf,
				"ð����%s������һ��������õĹ�Ĺ�����������ֳ�Ѱ�����ߡ���",
				currentuser->userid);
			deliverreport(title, buf);
			pressanykey();
			return 0;
		}
	}
	move(15, 1);
	prints
	    ("	�������м��ޣ�ԴԴ���ϵľ�ʯ��ȴ��ɱ��������\n"
	     "	���Ѿ��������ѵĳ���ʱ�䣬����ֻ����ս�����ˡ�\n"
	     "	�����ڳ��˳��������ű�����˺�!\n" "	��ĵ�ʶ�����½���!");
	myInfo->robExp -= 20;
	myInfo->begExp -= 20;
	myInfo->WorkPoint -= 3600;
	sprintf(title, "�����᡿��Ĺ����Ŷ�����");
	sprintf(buf,
		"%s�����ð���Ŷ��ڹ�Ĺ̽����ð��ǰ������ʧ���ء�\n"
		"\n�������ѣ����������С�", currentuser->userid);
	deliverreport(title, buf);
	pressanykey();
	return 0;
}

static int
normal_work()
{
	int ch, quit = 0, day, hour, minute, num;
	time_t ActiveTime, currTime;
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

	if (clubtest(ROBUNION) || clubtest(BEGGAR)) {
		move(12, 4);
		prints("�㲻�ǹ�������, �ȼ�����֯��");
		pressanykey();
		return 0;
	}
	if (!(myInfo->BeIn == 0)) {
		myInfo->weapon = 0;
		myInfo->armor = 0;
		myInfo->BeIn = 0;
	}
	while (!quit) {
		if (!(random() % 10))
			if (show_cake()) {
				prints
				    ("\n    ����ŭ�𣺡������֤ʧ�ܣ�������һ�ˣ���"
				     "\n    555���㱻û�������ֽ𣬲��ұ����30���ӡ�");
				myInfo->freeTime = time(NULL) + 1800;
				mcEnv->Treasury += myInfo->cash;
				myInfo->cash = 0;
				myInfo->mutex = 0;
				unloadData(myInfo, sizeof (mcUserInfo));
				unloadData(mcEnv, sizeof (MC_Env));
				pressreturn();
				Q_Goodbye();
			}
		money_show_stat("����ǰ̨");
/*		if (mcEnv->Treasury<20000000) {
			showAt(5, 4, "����ο��ˣ����������ˣ�cmft", YEA);
			return 0;
	  }
*/
		move(5, 4);
		prints("�����ǰ����ؼ�������Ϸ�Ӫ���ĳ����������¸�����");
		move(7, 4);
		prints
		    ("���ѶȵĹ�����Ȼ�и����룬��Σ����Ҳ��Ӧ���ӣ����Ҫ�����Լ��������ѡ��Ŷ");
		ActiveTime = myInfo->lastActiveTime;
		currTime = time(NULL);
		if (currTime > ActiveTime) {
			day = (currTime - ActiveTime) / 86400;
			hour = (currTime - ActiveTime) % 86400 / 3600;
			minute = (currTime - ActiveTime) % 3600 / 60 + 1;
			prints
			    ("\n\n    ������Ա���˷��ʼǱ��������ĳ��Ķ���˵��"
			     "\n\n	  ���ϴι����Ѿ���%d��%dСʱ%d������ǰ��"
			     "\n\n    �����Ҹ��õĹ��������ף���һ��Ҫע����ϧ��!",
			     day, hour, minute);
		} else {
			prints
			    ("\n\n    �����ϴι����������ˣ�����Ҫ����һ��ʱ��");
		}
		move(15, 1);
		prints("    ƣ�Ͷ�\033[1;31m  %d\033[m %%",
		       (30000 - myInfo->WorkPoint) / 300);
		move(17, 1);
		prints
		    ("    �������\033[1;33m%s\033[m, ������\033[1;33m%s\033[m, һ���������ֵ���ͷ��",
		     FreeWeapon[myInfo->weapon], FreeArmor[myInfo->armor]);
		move(t_lines - 2, 0);
		prints
		    ("\033[1;44m ѡ�� \033[1;46m[1]�������� [2]���񽻾� [3]�������� [4]����װ��\n"
		     "\033[1;44m      \033[1;46m[5]����     [6]�ڷ�     [Q]�뿪\033[0m");
		ch = igetkey();
		switch (ch) {
		case '1':
			nomoney_show_stat("��ٳ�ǰ");
			currTime = time(NULL);
			if (myInfo->WorkPoint < 1800) {
				showAt(12, 4, "�����˶������Բ�����", YEA);
				return 0;
			}
			if (myInfo->health < 30) {
				showAt(12, 4,
				       "��̫ƣ���ˣ�û�о�����������Щ�˶�",
				       YEA);
				return 0;
			}
			myInfo->lastActiveTime = currTime;
			update_health();
			myInfo->WorkPoint -= 1800;
			myInfo->health -= 30;
			myInfo->Actived += 10;
			if (random() % 4) {
				move(6, 1);
				prints
				    ("	�������壬Ϊ�����������50�꣡��\n"
				     "	һ�����ģ���ѽѽ\n");
				if (random() % 2)
					myInfo->robExp++;
				if (random() % 2)
					myInfo->begExp++;
				pressanykey();
				return 1;
			} else {
				move(6, 1);
				prints
				    ("	��Ӵ��Ӵ�������������ˣ��ѽŹ��ˣ����ǵò���ʧ����");
				pressanykey();
				return 0;
			}
			break;
		case '2':
			nomoney_show_stat("ʮ��·��");
			currTime = time(NULL);
			if (myInfo->WorkPoint < 3300) {
				showAt(12, 4,
				       "�����ͨ�������ã�����Ҫ���񽻾�", YEA);
				return 0;
			}
			if (myInfo->health < 50) {
				showAt(12, 4,
				       "������ƣ�������ӣ���ȥ���콻ͨ�¹ʰ���",
				       YEA);
				return 0;
			}
			if ((myInfo->robExp < 50) || (myInfo->begExp < 50)) {
				showAt(12, 4,
				       "������綼���õ������ӣ�ָ�ӽ�ͨ���˼һ�������㸶ҽҩ����!",
				       YEA);
				return 0;
			}
			myInfo->lastActiveTime = currTime;
			update_health();
			myInfo->WorkPoint -= 3300;
			myInfo->health -= 50;
			myInfo->Actived += 10;
			if (random() % 2) {
				num =
				    MAX(MIN(80000, mcEnv->Treasury - 20000000),
					0);
				move(6, 1);
				prints
				    ("	���񽻾��ܼ�����ǻӻ��ֶ���\n"
				     "	�ף�ǰ���߹������ĸ���˭���ѵ��Ǵ�˵�еĲ���\n"
				     "	�Ͽ�����ȥ�����ڣ��ٺ�\n"
				     "	�ۣ����ˣ�����ƾ�ն��%dԪ��\n"
				     "	��Ȼ�����кñ���!", num);
				myInfo->cash += num;
				mcEnv->Treasury -= num;
				myInfo->luck += 2;
				pressanykey();
				return 1;
			} else {
				move(6, 1);
				prints("	�������ָ���Ž�ͨ\n"
				       "	����˵������ת�Ŀ����ߣ���ת�Ŀ����ߣ�ֱ�е�����ߣ�û���ļ����ߡ�\n"
				       "	��ϲ�㣬����˻��ҵ�����£��㶼û�йҵ�\n"
				       "	��ĵ�ʶ����������");
				myInfo->robExp += 2;
				myInfo->begExp += 2;
				pressanykey();
				return 1;
			}
			break;
		case '3':
			nomoney_show_stat("ѧУ");
			currTime = time(NULL);
			if (myInfo->WorkPoint < 4500) {
				showAt(12, 4,
				       "�����ᳫ����ѧ���������γ̲�Ҫ����̫����",
				       YEA);
				return 0;
			}
			if (myInfo->health < 90) {
				showAt(12, 4,
				       "���Լ������ÿ�˯���ˣ�����ȥ�ϿΣ�",
				       YEA);
				return 0;
			}
			if ((myInfo->robExp < 250) || (myInfo->begExp < 250)) {
				showAt(12, 4,
				       "��������������ӣ���ȥ�����ӵ���", YEA);
				return 0;
			}
			if (myInfo->luck < 60) {
				showAt(12, 4,
				       "��������У���������´�����ѧ��!",
				       YEA);
				return 0;
			}
			myInfo->lastActiveTime = currTime;
			update_health();
			myInfo->WorkPoint -= 4500;
			myInfo->health -= 80;
			myInfo->Actived += 10;
			if (random() % 2) {
				num =
				    MAX(MIN(150000, mcEnv->Treasury - 20000000),
					0);
				move(6, 1);
				prints("	���ɽ�����˳���������ն�\n"
				       "	��ӷ���ֱ����ǧ�����뵽��Բ���߷���\n"
				       "	�·����ϵ���������֪\n"
				       "	ѧ�������ý����ζ������Ч��������\n"
				       "	���ù���%dԪ!�����Ʒ������!",
				       num);
				myInfo->cash += num;
				mcEnv->Treasury -= num;
				myInfo->luck += 5;
				pressanykey();
				return 1;
			} else {
				move(6, 1);
				prints
				    ("	���һ���Ͻ�̨��ѧ���ǾͲ������\n"
				     "	һʱ��۱ʺ��鱾��ɣ���Ȫˮƿ�������\n"
				     "	�㲻�ò������������ֶ����˵�ʶ����");
				myInfo->robExp += 5;
				myInfo->begExp += 5;
				pressanykey();
				return 1;
			}
			break;
		case '4':
			EquipShop(0);
			break;
		case '5':
			nomoney_show_stat("ɽ��");
			kill_dragon();
			break;
		case '6':
			nomoney_show_stat("�����");
			ghoul();
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
free_work()
{
	char uident[IDLEN + 1], buf[256], buff[256];
	int ch, quit = 0;
	mcUserInfo *mcuInfo;

	while (!quit) {
		nomoney_show_stat("������´�");
		move(4, 4);
		prints("�������ʲô��Ҫ��ɵ����񣬿��������﷢����");
		move(6, 4);
		prints("ÿ����������100�򣬹������20��");
		move(t_lines - 1, 0);
		prints
		    ("\033[1;44m ѡ�� \033[1;46m [1]�������� [2]������� [3]��ǰ���� [Q]�뿪\033[m");
		ch = igetkey();
		switch (ch) {
		case '1':
			if (myInfo->cash < 1200000) {
				showAt(8, 4, "����Ǯ����ʲô����", YEA);
				break;
			}
			if (seek_in_file
			    (DIR_MC "freework_list", currentuser->userid)) {
				showAt(8, 4, "���Ѿ�������һ�������ˡ�", YEA);
				break;
			}
			getdata(8, 4,
				"������������[\033[1;33mENTER\033[0m����]��",
				genbuf, 40, DOECHO, YEA);
			if (genbuf[0] == '\0')
				break;
			move(9, 4);
			if (askyn("\033[1;33m��ȷ������������\033[0m", NA, NA)
			    == NA)
				break;
			myInfo->cash -= 1200000;
			mcEnv->Treasury += 1200000;
			snprintf(buf, STRLEN - 1, "%s %s", currentuser->userid,
				 genbuf);
			addtofile(DIR_MC "freework_list", buf);
			break;
		case '2':
			if (!
			    (seek_in_file
			     (DIR_MC "freework_list", currentuser->userid))) {
				showAt(8, 4, "��û�з��������񰡡�", YEA);
				break;
			}
			if (!getOkUser("\n˭���������", uident, 6, 4)) {
				move(8, 4);
				prints("���޴���");
				pressanykey();
				break;
			}
			move(9, 4);
			if (askyn("\033[1;33m��ȷ����?\033[0m", NA, NA) == NA)
				break;
			if (!strcmp(uident, currentuser->userid)) {
				showAt(8, 4, "�Լ��������Լ������Ա��˳Ű�",
				       YEA);
				break;
			}
			move(8, 4);
			sethomefile(buf, uident, "mc.save");
			if (!file_exist(buf))
				initData(1, buf);
			if((mcuInfo = loadData(buf, sizeof (mcUserInfo))) == (void*)-1)
				return;
			if (mcuInfo->GetLetter == 0) {
				mcuInfo->cash += 50000;
				mcEnv->Treasury -= 50000;
			} else {
				mcuInfo->cash += 1000000;
				mcEnv->Treasury -= 1000000;
			}
			del_from_file(DIR_MC "freework_list",
				      currentuser->userid);
			sprintf(buff,
				"�������%s �ṩ���������񣬻�ý��� %d%s��",
				currentuser->userid, 1000000, MONEY_NAME);
			if (mcuInfo->GetLetter == 1)
				system_mail_buf(buff, strlen(buff), uident,
						"����֪ͨ", "deliver");
			sprintf(buff,
				"%s�����%s �ṩ���������񣬹������Լν���",
				uident, currentuser->userid);
			deliverreport("[����]��������", buff);
			unloadData(mcuInfo, sizeof (mcUserInfo));
			break;
		case '3':
		 	clear();
			move(1, 0);
			prints("%s���ᵱǰ����:\n", CENTER_NAME);
			listfilecontent(DIR_MC "freework_list");
			FreeNameList();
			pressanykey();
			break; 
		case 'q':
		case 'Q':
			quit = 1;
			break;
		}
	}
	return;
}

static int
get_task()
{
	int i, j;

	nomoney_show_stat("����¥");
	move(4, 4);
	prints("���Ǵ���֮������õ���ɫ֮��, �����Ŀ������ﲻ��, ���ｻ��.\n"
	       "    ƾ�������ֱ��, ��һ��ȥ���ڽ������ҵ���������.\n");
	move(7, 4);
	if (askyn("��Ҫ��ȡ������? �г����~", NA, NA) == NA)
		return 0;
	move(8, 4);
	if (myInfo->task.time > 0) {
		prints("��������û�����!");
		pressanykey();
		return 0;
	}
	if ((myInfo->task.task_flag & TASK_DONE) != 0) {
		prints("���������¶����Ц��, �ݸ��� 100000 %s��Ϊ����.\n",
		       MONEY_NAME);
		myInfo->cash += 100000;
	}
	while (1) {
		i = random() % maps[0].row;
		j = random() % maps[0].col;
		if (maps[0].map[i][j].exit & (~NODE_QUEUED))
			break;
	}
	maps[0].map[i][j].npc_num += 1;
	myInfo->task.time = time(NULL);
	myInfo->task.x = i;
	myInfo->task.y = j;
	myInfo->task.task_flag &= ~TASK_DONE;
	move(9, 4);
	prints("������С����˵: \"ȥа���ԭһ��, ��������������ʾ��.\"");
	pressanykey();
	return 0;
}

static int
cancel_task()
{
	nomoney_show_stat("����¥");
	move(4, 4);
	prints("���Ǵ���֮������õ���ɫ֮��, �����Ŀ������ﲻ��, ���ｻ��.\n"
	       "    ƾ�������ֱ��, ��һ��ȥ���ڽ������ҵ���������.\n");
	move(7, 4);
	if (askyn("��Ҫȡ��������? ", NA, NA) == NA)
		return 0;
	move(8, 4);
	if (myInfo->task.time == 0) {
		prints("��û����ȡ����!");
		pressanykey();
		return 0;
	}
	myInfo->task.task_flag &= ~TASK_DONE;
	if (myInfo->task.time + 3600 < time(NULL)) {
		myInfo->task.time = 0;
		prints("�����˺ݺݵ�˵: \"��������Ѿ���ʱ��! û���¾ͱ��!\"");
		pressanykey();
		return 0;
	}
	if (myInfo->cash > 100000) {
		myInfo->cash -= 100000;
		mcEnv->Treasury += 100000;
		myInfo->task.time = 0;
		myInfo->task.x = -1;
		prints("�����˵�����˵: \"����Ǯ�ķ���, ��ξ�����.\"");
		pressanykey();
		return 1;
	}
	myInfo->freeTime = time(NULL) + 1800;
	prints("��������Ц��: \"��ˣ��?!  ����ѽ!\"");
	refresh();
	sleep(3);
	Q_Goodbye();
	return 0;
}

int
check_task(int flag)
{
	int r = 0;

	if (myInfo->task.time == 0 || myInfo->task.x < 0 || myInfo->task.y < 0)	//no task
		return 0;
	if (maps[0].map[myInfo->task.x][myInfo->task.y].npc_num > 0)	//still task
		return 0;
	if (flag == 0 && myInfo->pos_x == myInfo->task.x && myInfo->pos_y == myInfo->task.y) {	//failed task
		myInfo->task.time = 0;
		myInfo->task.x = -1;
		return 0;
	}
	if (myInfo->task.time + 3600 >= time(NULL)) {	//intime
		myInfo->task.task_flag |= TASK_DONE;
		r = 1;
	}
	myInfo->task.time = 0;
	myInfo->task.x = -1;
	return r;
}

int
money_task()
{
	int ch, quit = 0;
	char uident[IDLEN + 1];
	time_t currTime;

	if (!(myInfo->GetLetter == 1)) {
		clear();
		showAt(5, 4, "���Ѿ��ر��˽���������Ϸ���ܣ��뿪����������",
		       YEA);
		return 0;
	}
	while (!quit) {
		nomoney_show_stat("����֮��");
		move(8, 4);
		prints("������ð���ߵ����ã�Ӣ�۽���η����, ���������¸�ǰ��!");
		move(t_lines - 2, 0);
		prints
		    ("\033[1;44m ѡ�� \033[1;46m [1]��ȡ���� [2]����     [3]ȡ������ [4]�չ�\033[0m\n"
		     "\033[1;44m      \033[1;46m [5]�ͽ����� [6]���� [7]��ӥ��   [Q]�뿪\033[m");
		ch = igetkey();
		switch (ch) {
		case '1':
			//get_task();
			break;
		case '2':
			clear();
			//if (walk_map(&maps[0]) == 1)
			//	return 0;
			break;
			/*prints("%s���ᵱǰ����:\n", CENTER_NAME);
			   listfilecontent(DIR_MC "freework_list");
			   FreeNameList();
			   pressanykey();
			   break; */
		case '3':
			//cancel_task();
			break;
		case '4':
			money_show_stat("�չ���");
			currTime = time(NULL);
			if (currTime - myInfo->lastActiveTime < 21600) {
				if (myInfo->BeIn) {
					move(5, 4);
					prints
					    ("�����̫��Ծ�ˣ�������ʱ�������أ��������أ�һ�̶�����������");
				} else {
					move(5, 4);
					prints
					    ("�ո�����ô��̫�˷��ˣ�����������");
				}
				pressanykey();
				break;
			}
			myInfo->WorkPoint +=
			    (currTime - myInfo->lastActiveTime - 3600);
			update_health();
			myInfo->lastActiveTime = time(NULL);
			if (myInfo->BeIn) {
				move(5, 4);
				prints
				    ("�Ǻǣ��չ���ô�ã�����÷��ɾ����˰ɣ����Դ��һ����");
			} else {
				move(5, 4);
				prints
				    ("�ӱչ��ҳ�������е��Ŀ������������ƣ���Ѿ���Ӱ����");
			}
			pressanykey();
			break;
		case '5':
			free_work();
			break;
		case '6':
			normal_work();
			break;
		case '7':
			nomoney_show_stat("��ӥ��");
			whoTakeCharge(0, uident);
			if (strcmp(currentuser->userid, uident))
				break;

			if (!getOkUser("\n��Ҫɾ��˭������", uident, 6, 4)) {
				move(8, 4);
				prints("���޴���");
				pressanykey();
				break;
			}
			move(9, 4);
			if (askyn("\033[1;33m��ȷ����?\033[0m", NA, NA) == NA)
				break;
			del_from_file(DIR_MC "freework_list", uident);
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

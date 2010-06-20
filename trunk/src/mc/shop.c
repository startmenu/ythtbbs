#include "mc.h"
 
// -------------------------   �̵�   --------------------------  // 
static int 
buy_card(char *cardname, char *filepath) 
{
	int i;
	char uident[IDLEN + 1], note[3][STRLEN], tmpname[STRLEN];
	clear();
	ansimore2(filepath, 0, 0, 25);
	for (i = 0; i < 4; i++) {
		move(2 + i * 5, 2 + i * 20);
		prints("\033[1;5;32m%s\033[m", MY_BBS_DOMAIN);
	}
	move(t_lines - 2, 4);
	sprintf(genbuf, "��ȷ��Ҫ��ؿ�%sô", cardname);
	if (askyn(genbuf, YEA, NA) == NA)
		return 0;
	if (myInfo->cash < 1000) {
		showAt(t_lines - 2, 60, "���Ǯ��������", YEA);
		return 0;
	}
	myInfo->cash -= 1000;
	mcEnv->Treasury += 1000;
	clear();
	if (!getOkUser("Ҫ���⿨Ƭ�͸�˭? ", uident, 0, 0))
		return 0;
	move(2, 0);
	prints("�л�Ҫ�ڿ�Ƭ��˵��[����д3���]");
	bzero(note, sizeof (note));
	for (i = 0; i < 3; i++) {
		getdata(3 + i, 0, ": ", note[i], STRLEN - 1, DOECHO, NA);
		if (note[i][0] == '\0')
			break;
	}
	sprintf(tmpname, "bbstmpfs/tmp/card.%s.%d", currentuser->userid,
		 uinfo.pid);
	copyfile(filepath, tmpname);
	if (i > 0) {
		int j;
		FILE * fp = fopen(tmpname, "a");
		fprintf(fp, "\n������ %s �ĸ���:\n", currentuser->userid);
		for (j = 0; j < i; j++)
			fprintf(fp, "%s", note[j]);
		fclose(fp);
	}
	if (mail_file 
	     (tmpname, uident, "�����Ҵ��̵���������ĺؿ���ϲ����",
	      currentuser->userid) >= 0) {
		showAt(8, 0, "��ĺؿ��Ѿ�����ȥ��", YEA);
	} else {
		showAt(8, 0, "�ؿ�����ʧ�ܣ��Է��ż�����", YEA);
	}
	unlink(tmpname);
	return 0;
}
static int 
shop_card_show() 
{
	DIR * dp;
	struct dirent *dirp;
	char dirNameBuffer[10][32], buf[STRLEN];
	char dirpath[256], filepath[256];
	int numDir, numFile, dirIndex, cardIndex;
	clear();
	sprintf(buf,
		 "    ����ؿ����Ǳ������������ֺؿ���������ԭ��δ�ܱ������ߡ�\n"
		 
		 "    ��ؿ������߶�����Ʒ���ڱ���������飬���뱾վϵͳά����ϵ\n"
		  "    ��վ����ʱ����������Ը����������\n\n" 
		 "                    \033[1;32m%s�ؿ���\033[0m\n",
		 CENTER_NAME);
	showAt(6, 0, buf, YEA);
	sprintf(buf, "%s�ؿ���", CENTER_NAME);
	nomoney_show_stat(buf);
	sprintf(buf, "�ؿ�Ŀǰÿ�Ŷ��� 1000 %s����������������͵ĺؿ�:",
		 MONEY_NAME);
	showAt(4, 4, buf, NA);
	if ((dp = opendir(DIR_SHOP)) == NULL)
		return -1;
	for (numDir = 0; (dirp = readdir(dp)) != NULL && numDir < 10;) {
		snprintf(dirpath, 255, "%s%s", DIR_SHOP, dirp->d_name);
		if (!file_isdir(dirpath) || dirp->d_name[0] == '.')
			continue;
		move(6 + numDir, 8);
		sprintf(buf, "\033[1;%dm%d. %s\033[m", numDir + 31, numDir,
			 dirp->d_name);
		prints(buf);
		strncpy(dirNameBuffer[numDir], dirp->d_name, 31);
		dirNameBuffer[numDir][31] = '\0';
		numDir++;
	}
	while (1) {
		getdata(16, 4, "��ѡ������:", buf, 3, DOECHO, YEA);
		if (buf[0] == '\0')
			return 0;
		dirIndex = atoi(buf);
		if (dirIndex >= 0 && dirIndex < numDir)
			break;
	}
	snprintf(dirpath, 255, "%s%s", DIR_SHOP, dirNameBuffer[dirIndex]);
	if ((dp = opendir(dirpath)) == NULL)
		return -1;
	for (numFile = 0; (dirp = readdir(dp)) != NULL;) {
		snprintf(filepath, 255, "%s/%s", dirpath, dirp->d_name);
		if (file_isfile(filepath) && dirp->d_name[0] != '.')
			numFile++;
	}
	move(17, 4);
	sprintf(genbuf,
		 "%s ���͵Ŀ�Ƭ���� %d ��. ��ѡ��ҪԤ���Ŀ���[ENTER����]: ",
		 dirNameBuffer[dirIndex], numFile);
	while (1) {
		getdata(18, 4, genbuf, buf, 3, DOECHO, YEA);
		if (buf[0] == '\0')
			return 0;
		cardIndex = atoi(buf);
		if (cardIndex >= 1 && cardIndex <= numFile)
			break;
	}
	snprintf(buf, STRLEN - 1, "%s%d", dirNameBuffer[dirIndex], cardIndex);
	snprintf(filepath, 255, "%s/%d", dirpath, cardIndex);
	buy_card(buf, filepath);
	limit_cpu();
	return 0;
}
static int 
shop_sellExp() 
{
	int convertMoney, exp, ch, quit = 0;
	float convertRate;
	char buf[256];
	clear();
	sprintf(buf, "%s����", CENTER_NAME);
	money_show_stat(buf);
	convertRate =
	    MIN(bbsinfo.utmpshm->ave_score / 2000.0 + 0.1, 10) * 1000;
	sprintf(genbuf,
		 "    ������ͨ����������ֵ���%s������ı�����\n" 
		 "    [1]����ֵ��%.1fÿ��\t"  "[2]���ԣ�40000ÿ��\t" 
		 "[3]��Ʒ�� 6000ÿ��\t"  "[Q]�˳�\t\t��ѡ��", MONEY_NAME,
		 convertRate);
	move(4, 0);
	prints("%s", genbuf);
	while (!quit) {
		ch = igetkey();
		switch (ch) {
		case '1':
			exp = countexp(currentuser, 2);
			convertMoney = (exp - myInfo->selledExp) * convertRate;
			if (convertMoney <= 0)
				break;
			sprintf(genbuf,
				 "�����ڵľ���ֵ,���������� %d %s, ��Ҫ������",
				 convertMoney, MONEY_NAME);
			move(7, 4);
			if (askyn(genbuf, NA, NA) == NA)
				break;
			if (mcEnv->Treasury - 10000000 > convertMoney) {	//������������ٱ���1000w �ʽ�����
				myInfo->Actived++;
				mcEnv->Treasury -= convertMoney;
				myInfo->selledExp = exp;
				myInfo->cash += convertMoney;
				move(8, 4);
				prints("���׳ɹ������������� %d %s��",
					convertMoney, MONEY_NAME);
			} else {
				prints("����û����ô��Ǯ����");
			}
			break;
			
/*		case '2':
			convertMoney = userInputValue(7, 4, "��", "�㵨ʶ", 1, 100);
			if (convertMoney < 0)	break;
			if (convertMoney > myInfo->robExp) {
				showAt(8, 4, "��û����ô�ߵĵ�ʶ��", YEA);
				break;
			}
			if (mcEnv->Treasury < convertMoney * 30000) {
				showAt(8, 4, "����û����ô��Ǯ����", YEA);
				break;
			}
			myInfo->Actived++;
			myInfo->robExp -= convertMoney;
			myInfo->cash += convertMoney * 30000;
			mcEnv->Treasury -= convertMoney * 30000;
			showAt(9, 4, "���׳ɹ���", YEA);
			break;
		case '3':
                        convertMoney = userInputValue(7, 4, "��", "����", 1, 100);
			if (convertMoney < 0)   break;
			if (convertMoney > myInfo->begExp) {
				showAt(8, 4, "��û����ô�ߵ�����", YEA);
				break;
			}
			if (mcEnv->Treasury < convertMoney * 30000) {
				showAt(8, 4, "����û����ô��Ǯ����", YEA);
				break;
			}
			myInfo->Actived++;
			myInfo->begExp -= convertMoney;
			myInfo->cash += convertMoney * 30000;
			mcEnv->Treasury -= convertMoney * 30000;
			showAt(9, 4, "���׳ɹ���", YEA);
			break;
*/ 
		case '2':
			convertMoney =
			    userInputValue(7, 4, "��", "������", 1, 100);
			if (convertMoney < 0)
				break;
			if (convertMoney > myInfo->robExp) {
				showAt(8, 4, "��û����ô�ߵĵ�ʶ��", YEA);
				break;
			}
			if (convertMoney > myInfo->begExp) {
				showAt(8, 4, "��û����ô�ߵ�����", YEA);
				break;
			}
			if (mcEnv->Treasury - 25000000 < convertMoney * 40000) {	//�����Թ��Ᵽ��2500w
				showAt(8, 4, "����û����ô��Ǯ����", YEA);
				break;
			}
			myInfo->Actived++;
			myInfo->robExp -= convertMoney;
			myInfo->begExp -= convertMoney;
			myInfo->cash += convertMoney * 40000;
			mcEnv->Treasury -= convertMoney * 40000;
			showAt(9, 4, "���׳ɹ���", YEA);
			break;
		case '3':
			convertMoney =
			    userInputValue(7, 4, "��", "����Ʒ", 1, 100);
			if (convertMoney < 0)
				break;
			if (convertMoney > myInfo->luck) {
				showAt(8, 4, "��û����ô�õ���Ʒ��", YEA);
				break;
			}
			if (mcEnv->Treasury - 25000000 < convertMoney * 6000) {
				showAt(8, 4, "����û����ô��Ǯ����", YEA);
				break;
			}
			myInfo->Actived++;
			myInfo->luck -= convertMoney;
			myInfo->cash += convertMoney * 6000;
			mcEnv->Treasury -= convertMoney * 6000;
			showAt(9, 4, "���׳ɹ���", YEA);
			break;
		case 'q':
		case 'Q':
		default:
			quit = 1;
			break;
		}
	}
	return 1;
}
static void 
buydog() 
{
	int guard_num;
	clear();
	nomoney_show_stat("����԰");
	move(4, 4);
	prints 
	    ("%s����԰���۴��ִ��ǹ�����Ҫ��������ʿ��ÿֻ10000%s��",
	     CENTER_NAME, MONEY_NAME);
	
#if 0
	    if (myInfo->cash < 100000) {
		showAt(8, 4, "�㻹��ʡʡ��?û�˻���������ġ�", YEA);
		return;
	}
	
#endif				/*  */
	    guard_num = (myInfo->robExp / 100) + 1;
	guard_num = (guard_num > 8) ? 8 : guard_num;
	if (myInfo->guard >= guard_num) {
		showAt(8, 4, "���Ѿ����㹻�Ĵ��ǹ��˰���������ô��С�ɣ�",
			YEA);
		return;
	}
	move(6, 4);
	prints("������Ŀǰ����ݵ�λ������%dֻ���ǹ��͹��ˡ�",
		guard_num - myInfo->guard);
	guard_num =
	    userInputValue(7, 4, "��", "ֻ���ǹ�", 1,
			    guard_num - myInfo->guard);
	if (guard_num == -1) {
		showAt(8, 4, "Ŷ�����������˰��������ǻ�ӭ�´�������", YEA);
		return;
	}
	if (myInfo->cash < guard_num * 10000) {
		showAt(8, 4, "������˼������Ǯ���������߲��͡�", YEA);
		return;
	}
	myInfo->cash -= guard_num * 10000;
	mcEnv->prize777 += after_tax(guard_num * 10000);
	myInfo->guard += guard_num;
	move(8, 4);
	showAt(9, 4, "����ǹ��ɹ�,�������һ��ʱ�䰲��̫ƽ�ˡ�", YEA);
	return;
}
int 
money_shop() 
{
	int ch, quit = 0, bonus = 0;
	char buf[256], uident[IDLEN + 1];
	if (!(myInfo->GetLetter == 1)) {
		clear();
		showAt(5, 4, "���Ѿ��ر��˽���������Ϸ���ܣ��뿪����������",
			YEA);
		return 0;
	}
	while (!quit) {
		sprintf(buf, "%s�ٻ���˾", CENTER_NAME);
		nomoney_show_stat(buf);
		move(6, 4);
		prints("%s�̳���������𣬴�Ҿ��ˣ�", CENTER_NAME);
		move(t_lines - 1, 0);
		prints 
		    ("\033[1;44m ѡ�� \033[1;46m [1]���� [2]�ؿ� [3]���� [4]�̳�����칫�� [Q]�뿪\033[m");
		ch = igetkey();
		switch (ch) {
		case '1':
			money_show_stat("�̳������̨");
			move(5, 0);
			prints("\033[1;42m���ܼ�飺\033[m\n" 
				"���ǹ���ʹ�ö����Լ������ã����ر������ٱ���ש���ܣ�\n"
				
				"ƽ������ʹ�ö����Լ������ã�ƽ�⵨ʶ������\n"
				
				"�����ӣ�ʹ�ö����Լ������ã�������ӵ�ʶ������\n"
				 "�����裨ʹ�ö����Լ������ã��������Ʒ��\n"
				
				"ϴ�赤��ʹ�ö����Լ������ã���Ʒ�����Ļ����п��ܳ����ǣ�\n"
				
				"�������ʹ�ö����Լ������ã����ص�ʱ�������\n"
				 "ǧ��Σ�ʹ�ö����Լ������ã�����������\n" 
				"����ˮ��ʹ�ö����Լ������ã�����ƣ��,����ͷ�ԣ�\n"
				
#if 0			      
				"���˵���ʹ�ö��󣺿�ָ�������ã��ɹ���ɶ��ڼ���������\n"
				 "��Ͳ�� (ʹ�ö����Լ������ã����ڼӵ�ʶ)\n"
				
				"͵�Ļ���ʹ�ö��󣺿�ָ�������ã��ɹ���ɼ��䵨ʶ��\n"
				 "���г���ʹ�ö����Լ������ã����ڼ���)\n"
				
				"����Ƥ��ʹ�ö��󣺿�ָ�������ã��ɹ���ɶ��ڼ�������"
				
#endif				/*  */
			    );
			move(t_lines - 2, 0);
			
#if 0
			    prints 
			    ("\033[1;44m ��Ҫ���� \033[1;46m [1]���ǹ� [2]����� [3]������ [4]���˵�\n"
			     
			     "\033[1;44m          \033[1;46m [5]��Ͳ�� [6]͵�Ļ� [7]���г� [8]����Ƥ [Q]�뿪\033[m");
			
#else				/*  */
			    prints
			    ("\033[1;44m ��Ҫ���� \033[1;46m [1]���ǹ� [2]ƽ���� [3]������ [4]������ [5]ϴ�赤 [6]�����\033[0m\n"
			     
			     "\033[1;44m          \033[1;46m [7]ǧ��� [8]����ˮ [Q]�뿪\033[m");
			
#endif				/*  */
			    ch = igetkey();
			switch (ch) {
			case '1':
				update_health();
				if (check_health
				     (1, 19, 4, "���������ˣ�", YEA))
					break;
				myInfo->health--;
				myInfo->Actived++;
				buydog();
				break;
			case '2':
				update_health();
				move(18, 4);
				if (askyn
				     ("ƽ����100,000Ԫһ����ȷʵҪ������", NA,
				      NA) == NA)
					break;
				if (check_health
				     (1, 19, 4, "������������ˣ�", YEA))
					break;
				if (myInfo->cash < 100000) {
					showAt(19, 4, "��û����ô���ֽ�",
						YEA);
					break;
				}
				myInfo->health--;
				myInfo->Actived++;
				myInfo->cash -= 100000;
				mcEnv->Treasury += 100000;
				myInfo->robExp =
				    (myInfo->robExp + myInfo->begExp) / 2;
				myInfo->begExp = myInfo->robExp;
				showAt(20, 4, "ת���ɹ���", YEA);
				break;
			case '3':
				update_health();
				move(18, 4);
				if (askyn
				     ("������500,000Ԫһ����ȷʵҪ������", NA,
				      NA) == NA)
					break;
				if (check_health
				     (20, 19, 4, "������������ˣ�", YEA))
					break;
				if (myInfo->cash < 500000) {
					showAt(19, 4, "��û�����ֽ�", YEA);
					break;
				}
				myInfo->health -= 20;
				myInfo->Actived += 5;
				myInfo->cash -= 500000;
				mcEnv->Treasury += 500000;
				bonus = random() % 15 + 1;
				myInfo->robExp += bonus;
				myInfo->begExp += bonus;
				move(20, 4);
				prints
				    ("\033[1;31m��ϲ��������͵�ʶ��������%d�㣡��",
				     bonus);
				pressanykey();
				break;
			case '4':
				update_health();
				move(18, 4);
				if (askyn
				     ("������500,000Ԫһ�ţ�ȷʵҪ������", NA,
				      NA) == NA)
					break;
				if (check_health
				     (20, 19, 4, "������������ˣ�", YEA))
					break;
				if (myInfo->cash < 500000) {
					showAt(14, 4, "��û�����ֽ�", YEA);
					break;
				}
				myInfo->health -= 20;
				myInfo->Actived += 5;
				myInfo->cash -= 500000;
				mcEnv->Treasury += 500000;
				bonus = random() % (100 - myInfo->luck) + 1;
				myInfo->luck += bonus;
				move(20, 4);
				prints
				    ("\033[1;31m��ϲ�������Ʒ������%d�㣡��",
				     bonus);
				pressanykey();
				break;
			case '5':
				update_health();
				move(18, 4);
				if (askyn
				     ("ϴ����500,000Ԫһ�ţ����ҿ���ûЧ����ȷʵҪ������",
				      NA, NA) == NA)
					break;
				if (check_health
				     (20, 19, 4, "������������ˣ�", YEA))
					break;
				if (myInfo->cash < 500000) {
					showAt(19, 4, "��û�����ֽ�", YEA);
					break;
				}
				myInfo->health -= 20;
				myInfo->Actived += 5;
				myInfo->cash -= 500000;
				mcEnv->Treasury += 500000;
				if ((random() % 2) && (myInfo->luck >= 90)
				     && (myInfo->con < 25)) {
					myInfo->con += random() % 6 + 1;
					move(20, 4);
					prints
					    ("\033[1;31m������һ��ϴ�赤��ֻ�����������죬�������ڷ����˵�仯����");
				}
				
				else {
					move(20, 4);
					prints
					    ("\033[1;31m������һ��ϴ�赤���������һ����ʲô�о���û�С�����");
				}
				pressanykey();
				break;
			case '6':
				update_health();
				move(18, 4);
				if (askyn
				     ("�����500,000Ԫһ̨����˵�ܿ�Խʱ�䣬ȷʵҪ������",
				      NA, NA) == NA)
					break;
				if (check_health
				     (20, 19, 4, "������������ˣ�", YEA))
					break;
				if (myInfo->cash < 500000) {
					showAt(19, 4, "��û�����ֽ�", YEA);
					break;
				}
				myInfo->health -= 20;
				myInfo->Actived += 5;
				myInfo->cash -= 500000;
				mcEnv->Treasury += 500000;
				if ((random() % 4) && (myInfo->luck >= 90)) {
					myInfo->lastActiveTime -= 1800;
					myInfo->WorkPoint += 3600;
					move(20, 4);
					prints
					    ("\033[1;31m�������˴������һ��������ת֮������������е㲻ͬ�ˣ���");
				}
				
				else {
					move(20, 4);
					prints
					    ("\033[1;31m�������˴������ͻȻһ��Ǻ�˵���ζð�˳��������������α�Ӳ�Ʒ��...");
				}
				pressanykey();
				break;
			case '7':
				update_health();
				move(18, 4);
				if (askyn
				     ("ǧ���200,000Ԫһֻ����˵�ָܻ�������ȷʵҪ������",
				      NA, NA) == NA)
					break;
				if (myInfo->cash < 200000) {
					showAt(19, 4, "��û�����ֽ�", YEA);
					break;
				}
				myInfo->health += 10 + random() % 10;
				myInfo->Actived += 5;
				myInfo->cash -= 200000;
				mcEnv->Treasury += 200000;
				move(20, 4);
				prints
				    ("\033[1;31m������һֻǧ���˲Σ�ֻ����������ʼ�ָ�...");
				pressanykey();
				break;
			case '8':
				update_health();
				move(18, 4);
				if (askyn
				     ("����ˮ500,000Ԫһƿ����˵�ܼ���ƣ�ͣ�ȷʵҪ������",
				      NA, NA) == NA)
					break;
				if (check_health
				     (10, 19, 4, "������������ˣ�", YEA))
					break;
				if (myInfo->cash < 500000) {
					showAt(19, 4, "��û�����ֽ�", YEA);
					break;
				}
				myInfo->WorkPoint += 1800 + random() % 1800;
				myInfo->health -= 5;
				myInfo->Actived += 5;
				myInfo->cash -= 500000;
				mcEnv->Treasury += 500000;
				move(20, 4);
				prints
				    ("\033[1;31m�����һƿ����ˮ��ֻ����ͷ�����������...");
				pressanykey();
				break;
			case 'q':
			case 'Q':
				quit = 1;
				break;
			}
			quit = 0;
			break;
		case '2':
			shop_card_show();
			break;
		case '3':
			money_show_stat("���̹�̨");
			update_health();
			if (check_health(1, 12, 4, "���������ˣ�", YEA))
				break;
			myInfo->health--;
			shop_sellExp();
			break;
		case '4':
			money_show_stat("�̳�����칫��");
			whoTakeCharge(7, uident);
			if (strcmp(currentuser->userid, uident))
				break;
			while (!quit) {
				nomoney_show_stat("�̳�ҵ��");
				update_health();
				if (check_health 
				     (1, 12, 4, "Ъ��Ъ�ᣬ���������ˣ�", YEA))
					break;
				move(t_lines - 1, 0);
				prints 
				    ("\033[1;44m ѡ�� \033[1;46m [1]�ɹ� [2]���� [3]�鿴�̵��ʲ� [Q]�뿪\033[m");
				ch = igetkey();
				switch (ch) {
				case '1':
					nomoney_show_stat("�ɹ���");
					showAt(12, 4,
						"\033[1;32m�滮�С�\033[m",
						YEA);
					break;
				case '2':
					money_show_stat("�г���");
					showAt(12, 4,
						"\033[1;32m�滮�С�\033[m",
						YEA);
					break;
				case '3':
					money_show_stat("С���");
					showAt(12, 4,
						"\033[1;32m������ڵ�Ǯ�����Ժ�\033[m",
						YEA);
					break;
				case 'q':
					quit = 1;
					break;
				}
			}
			quit = 0;
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
void
EquipShop(int type) 
{
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
	char ch, quit = 0;
	char buf[256];
	int Wnum = 0, Anum = 0;
	while (!quit) {
		Wnum = myInfo->weapon * myInfo->weapon * myInfo->weapon + 1;
		Anum = myInfo->armor * myInfo->armor * myInfo->armor + 1;
		switch (type) {
		case 0:
			money_show_stat("װ����");
			move(5, 8);
			prints("ĥ�����󿳲�,���װ���°빦��");
			move(t_lines - 1, 0);
			prints 
			    ("\033[1;44m ѡ�� \033[1;46m [1]���� [2]���� [Q]�뿪\033[m");
			break;
		case 1:
			money_show_stat("�ڰ�����");
			move(5, 8);
			prints("Ҫ��ȥ�������������ó��ֵļһ��");
			move(t_lines - 1, 0);
			prints 
			    ("\033[1;44m ѡ�� \033[1;46m [1]ǹ֧ [2]���� [Q]�뿪\033[m");
			break;
		case 2:
			money_show_stat("ؤ������");
			move(5, 8);
			prints("֪ʶ��������������������кô���");
			move(t_lines - 1, 0);
			prints 
			    ("\033[1;44m ѡ�� \033[1;46m [1]�鼮 [2]�书 [Q]�뿪\033[m");
			break;
		}
		ch = igetkey();
		switch (ch) {
		case '1':
			switch (type) {
			case 0:
				if (myInfo->weapon > 14) {
					showAt(7, 8, "С��û�и��õ�������",
						YEA);
					return;
				}
				if ((myInfo->robExp < 10 * Wnum)
				     || (myInfo->begExp < 10 * Wnum)) {
					sprintf(buf,
						 "���\033[1;33m%s\033[m̫�����ˣ����ʺ���,�������Ե�\033[1;33m%d\033[m��˵��",
						 FreeWeapon[myInfo->weapon + 1],
						 10 * Wnum);
					showAt(7, 8, buf, YEA);
					return;
				}
				if (myInfo->cash < 20000 * Wnum) {
					sprintf(buf,
						 "���\033[1;33m%s\033[m%d%s,�㻹�Ǵ���Ǯ������",
						 FreeWeapon[myInfo->weapon + 1],
						 20000 * Wnum, MONEY_NAME);
					showAt(7, 8, buf, YEA);
					return;
				}
				sprintf(buf,
					   "���\033[1;33m%s\033[m%d%s,��ȷ��Ҫ����",
					   FreeWeapon[myInfo->weapon + 1],
					   20000 * Wnum, MONEY_NAME);
				move(7, 8);
				if (askyn(buf, NA, NA) == YEA) {
					myInfo->weapon++;
					myInfo->cash -= 20000 * Wnum;
					mcEnv->Treasury += 20000 * Wnum;
				}
				break;
			case 1:
				if (myInfo->weapon > 14) {
					showAt(7, 8,
						"���õ�������û��˽������",
						YEA);
					return;
				}
				if (myInfo->robExp < 10 * Wnum) {
					sprintf(buf,
						 "���\033[1;33m%s\033[m����̫���ˣ������˷�,�������Ե�\033[1;33m%d\033[m��˵��",
						 RobWeapon[myInfo->weapon + 1],
						 10 * Wnum);
					showAt(7, 8, buf, YEA);
					return;
				}
				if (myInfo->cash < 20000 * Wnum) {
					sprintf(buf,
						 "���\033[1;33m%s\033[m%d%s,�㻹�Ǵ�����Ǯ������",
						 RobWeapon[myInfo->weapon + 1],
						 20000 * Wnum, MONEY_NAME);
					showAt(7, 8, buf, YEA);
					return;
				}
				sprintf(buf,
					   "���\033[1;33m%s\033[m%d%s,��ȷ��Ҫ����",
					   RobWeapon[myInfo->weapon + 1],
					   20000 * Wnum, MONEY_NAME);
				move(7, 8);
				if (askyn(buf, NA, NA) == YEA) {
					myInfo->weapon++;
					myInfo->cash -= 20000 * Wnum;
					mcEnv->Treasury += 20000 * Wnum;
				}
				break;
			case 2:
				if (myInfo->weapon > 14) {
					showAt(7, 8, "����Ԥ����", YEA);
					return;
				}
				if (myInfo->begExp < 10 * Wnum) {
					sprintf(buf,
						 "�Ȿ\033[1;33m%s\033[m̫����ˣ��㿴����,�������Ե�\033[1;33m%d\033[m��˵��",
						 BegWeapon[myInfo->weapon + 1],
						 10 * Wnum);
					showAt(7, 8, buf, YEA);
					return;
				}
				if (myInfo->cash < 20000 * Wnum) {
					sprintf(buf,
						 "�Ȿ\033[1;33m%s\033[m%d%s,�㻹�Ǵ�����Ǯ������",
						 BegWeapon[myInfo->weapon + 1],
						 20000 * Wnum, MONEY_NAME);
					showAt(7, 8, buf, YEA);
					return;
				}
				sprintf(buf,
					   "�Ȿ\033[1;33m%s\033[m%d%s,��ȷ��Ҫ����",
					   BegWeapon[myInfo->weapon + 1],
					   20000 * Wnum, MONEY_NAME);
				move(7, 8);
				if (askyn(buf, NA, NA) == YEA) {
					myInfo->weapon++;
					myInfo->cash -= 20000 * Wnum;
					mcEnv->Treasury += 20000 * Wnum;
				}
				break;
			}
			break;
		case '2':
			switch (type) {
			case 0:
				if (myInfo->armor > 14) {
					showAt(7, 8, "С��û�и��õĻ�����",
						YEA);
					return;
				}
				if ((myInfo->robExp < 10 * Anum)
				     || (myInfo->begExp < 10 * Anum)) {
					sprintf(buf,
						 "���\033[1;33m%s\033[m�㴩û��Ҫ,�������Ե�\033[1;33m%d\033[m��˵��",
						 FreeArmor[myInfo->armor + 1],
						 10 * Anum);
					showAt(7, 8, buf, YEA);
					return;
				}
				if (myInfo->cash < 20000 * Anum) {
					sprintf(buf,
						 "���\033[1;33m%s\033[m%d%s,�㻹�Ǵ�����Ǯ������",
						 FreeArmor[myInfo->armor + 1],
						 20000 * Anum, MONEY_NAME);
					showAt(7, 8, buf, YEA);
					return;
				}
				sprintf(buf,
					   "���\033[1;33m%s\033[m%d%s,��ȷ��Ҫ����",
					   FreeArmor[myInfo->armor + 1],
					   20000 * Anum, MONEY_NAME);
				move(7, 8);
				if (askyn(buf, NA, NA) == YEA) {
					myInfo->armor++;
					myInfo->cash -= 20000 * Anum;
					mcEnv->Treasury += 20000 * Anum;
				}
				break;
			case 1:
				if (myInfo->armor > 14) {
					showAt(7, 8, "Ҫ�ó����Լ���ȥ", YEA);
					return;
				}
				if (myInfo->robExp < 10 * Anum) {
					sprintf(buf,
						 "����\033[1;33m%s\033[m��̫���㣬��ᷭ����,�������Ե�\033[1;33m%d\033[m��˵��",
						 RobArmor[myInfo->armor + 1],
						 10 * Anum);
					showAt(7, 8, buf, YEA);
					return;
				}
				if (myInfo->cash < 20000 * Anum) {
					sprintf(buf,
						 "����\033[1;33m%s\033[m%d%s,�㻹�Ǵ�����Ǯ������",
						 RobArmor[myInfo->armor + 1],
						 20000 * Anum, MONEY_NAME);
					showAt(7, 8, buf, YEA);
					return;
				}
				sprintf(buf,
					   "����\033[1;33m%s\033[m%d%s,��ȷ��Ҫ����",
					   RobArmor[myInfo->armor + 1],
					   20000 * Anum, MONEY_NAME);
				move(7, 8);
				if (askyn(buf, NA, NA) == YEA) {
					myInfo->armor++;
					myInfo->cash -= 20000 * Anum;
					mcEnv->Treasury += 20000 * Anum;
				}
				break;
			case 2:
				if (myInfo->armor > 14) {
					showAt(7, 8,
						"ԭ�����Ÿ�����ͣ������̫���ˣ�û�ÿ���",
						YEA);
					return;
				}
				if (myInfo->begExp < 10 * Anum) {
					sprintf(buf,
						 "\033[1;33m%s\033[m���������ڵ�������������,�������Ե�\033[1;33m%d\033[m��˵��",
						 BegArmor[myInfo->armor + 1],
						 10 * Anum);
					showAt(7, 8, buf, YEA);
					return;
				}
				if (myInfo->cash < 20000 * Anum) {
					sprintf(buf,
						 "�Ȿ\033[1;33m%s\033[m%d%s,�㻹�Ǵ�����Ǯ������",
						 BegArmor[myInfo->armor + 1],
						 20000 * Anum, MONEY_NAME);
					showAt(7, 8, buf, YEA);
					return;
				}
				sprintf(buf,
					   "�Ȿ\033[1;33m%s\033[m%d%s,��ȷ��Ҫ����",
					   BegArmor[myInfo->armor + 1],
					   20000 * Anum, MONEY_NAME);
				move(7, 8);
				if (askyn(buf, NA, NA) == YEA) {
					myInfo->armor++;
					myInfo->cash -= 20000 * Anum;
					mcEnv->Treasury += 20000 * Anum;
				}
				break;
			}
			break;
		case 'q':
		case 'Q':
			quit = 1;
			break;
		}
	}
	return;
}



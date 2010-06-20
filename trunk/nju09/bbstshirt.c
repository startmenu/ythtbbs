#include "bbs.h"
#include "ythtbbs.h"
#include "moneycenter.h"
#include "bbslib.h"
#define REGFORM MY_BBS_HOME "/wwwtmp/tshirt"
#define TSHIRTADMIN MY_BBS_HOME "/wwwtmp/tshirtadmin"
#define TSHIRTBOARD "TshirtInfo"
#define MINPOSTFEE 8
#define BASEPOSTFEE 2
#define PAGESIZE 10
#define SIZEHELP "con?B=330&F=M.1122796285.A&T=0"
#define ORDERHELP "con?B=330&F=M.1122796674.A&T=0"
#define PUBBOARD "Tshirt"
#define MAXTYPE 48		//���48��Ʒ��

struct orderitem {
	int seq_num;		//��ˮ��
	char userid[IDLEN + 1];
	char name[40];
	int type;		//0 �ʼ� 1 ��ȡ
	char address[200];
	char postcode[12];
	char email[80];
	char phone[30];
	int amount[MAXTYPE];	//���48��վ��Ʒ�֣����涩��������
	int bonus[10];		//��Ʒ����
	time_t time;
	int status;		//״̬��0 �ɹ����� 1 �յ����� 2 �ѷ��� -1 ��ɾ��
};

const int itemsize = sizeof (struct orderitem);

const char *bonusname[] = {
	"��Ϳ��+200000",
	"����+2",
	"����ֵ+20",
	0
};

const static struct tshirt {
	int num;
	char sex;
	char size[5];
	char color[4];
	int price;
	int postfee;
} tshirts[] = {
	{
	1, 'B', "S", "��", 30, 1}, {
	2, 'B', "S", "��", 30, 1}, {
	3, 'B', "M", "��", 30, 1}, {
	4, 'B', "M", "��", 30, 1}, {
	5, 'B', "L", "��", 30, 1}, {
	6, 'B', "L", "��", 30, 1}, {
	7, 'B', "XL", "��", 30, 1}, {
	8, 'B', "XL", "��", 30, 1}, {
	9, 'B', "XXL", "��", 30, 1}, {
	10, 'B', "XXL", "��", 30, 1}, {
	11, 'B', "XXXL", "��", 30, 1}, {
	12, 'B', "XXXL", "��", 30, 1}, {
	13, 'B', "����", "��", 50, 2}, {
	14, 'F', "S(F)", "��", 30, 1}, {
	15, 'F', "S(F)", "��", 30, 1}, {
	0, 'F', "F", "F", 0, 0}
};

void printcss(void);
void printform(void);
int getform(struct orderitem *item);
void checkuserinfo(struct orderitem *item);
int saveitem(struct orderitem *item);
void printmyorders(char *userid);
void printorderdetail(struct orderitem *item);
void printstatus(struct orderitem *item);
int isadmin(void);
int ismine(int seq, struct orderitem *item);
void query(int seq);
int update(int seq, int status);	//����״̬
void post_record(char *title, struct orderitem *item);
void admin(void);
void printitemhead(void);
void printitemline(struct orderitem *item);
void printitemend(void);
void printitemoper(struct orderitem *item);
void printitembystatus(int fd, struct orderitem *item, int status, int offset,
		       int admintype);
void dobonus(struct orderitem *item);
void printoffsetnav(int admintype, int offset);
void fixorders(void);

int
bbstshirt_main()
{
	char action;
	static struct orderitem *item = NULL;

	if (!item)
		item = (struct orderitem *) malloc(itemsize);
	if (!item)
		http_fatal("No Memory?");

	action = getparm("action")[0];

	html_header(1);

	if (!loginok || isguest) {
		printf("�Ҵҹ��Ͳ��ܶ���T Shirt�����ȵ�¼!<br>");
		printf
		    ("�����ȷʵ�Ѿ���¼ȴ���������ʾ���볢���˽��¼ʱ�� IP��֤��Χ ѡ��<br>");
		printf("[<a href='javascript:history.go(-1)'>����</a>]");
		http_quit();
	}

	printcss();

	printf("<script src=/tshirt.js></script>");

	switch (action) {
	case 'n':
		if (getform(item) == 0) {
			printf("<div align=center><div class=msgbox>");
			printf("<p>��һ��Ҳû�ж����������� [<a href=#"
			       " onclick=history.go(-1)>����</a>] ������һҳ������д��</p>");
			printf("</div>");
			return -1;
		}
		printf("<div align=center><div class=msgbox>");
		checkuserinfo(item);	//�����Ƿ����Ҫ��

		if (saveitem(item) <= 0)
			http_fatal("���涩����Ϣʧ�ܡ�");

		printf("<p>�����ύ�ɹ�������Ϊ���飬���"
		       " [<a href=tshirt>����</a>] ���ض������档"
		       "</p></div></div><br><br>");
		printorderdetail(item);
		break;

	case 'q':
		query(atoi(getparm("seq")));
		break;

	case 'u':
		update(atoi(getparm("seq")), atoi(getparm("status")));
		break;

	default:
		if (isadmin()) {
			printf("<h1>����ѡ��</h1>");
			admin();
		}
		printf("<h1>����վ��(<a href = " ORDERHELP
		       "><font color=red>����ϵͳ����</font></a>��"
		       "</h1>");
		printform();
		printf("<br><br><h1>�ҵĶ���</h1>");
		printmyorders(currentuser->userid);
	}
	return 0;
}

void
printcss()
{
	printf("<style>.inputbox{\n"
	       "width: 90%%; \n"
	       "border-color: #67a8ea; \n"
	       "border-width: 1px; \n"
	       "border-style: solid; \n"
	       "background-color: #fff;}\n"
	       "tr {text-align: center;}\n"
	       "td {border-width: 0px; "
	       "border-style: solid; "
	       "border-color: #67a8ea; "
	       "background-color: #eee; "
	       "padding: 4px 10px 4px 10px;}\n"
	       ".msgbox {"
	       "width: 85%%;"
	       "background-color: #eee;"
	       "padding: 10px 10px 10px 10px;"
	       "border-color: #67a8ea;"
	       "border-width: 1px;"
	       "border-style: solid;"
	       "float: center;"
	       "}\n"
	       "h1 {"
	       "border-style: dotted dotted dotted solid; "
	       "border-color: rgb(124, 189, 255); "
	       "border-width: 1px 1px 1px 15px; "
	       "margin: 0pt 0pt 10px; font-size: "
	       "14px; line-height: 20px; "
	       "padding: 12 0 12 25px; "
	       "text-align: left; "
	       "background-color: rgb(234, 244, 255);" "</style>");
}

void
printform()
{
	int tshirt_num = 0;
	int bonus_num = 0;
	struct userdata currentdata;

	loaduserdata(currentuser->userid, &currentdata);
	printf("<form action='tshirt?action=new' method=post>"
	       "<table width=100%% align=center>"
	       "<tr><td width=25%%>����ID</td><td>"
	       "<input type=text name=userid value=%s "
	       "class=inputbox disabled></tr>"
	       "<tr><td>�ջ���ʽ</td><td>"
	       "<input type=radio name=usertype value=0 checked "
	       "onclick=show_sh_mail()>�ʼġ���"
	       "<input type=radio name=usertype value=1 "
	       "onclick=show_sh_self()>��ȡ (�ޱ�����)"
	       "<input type=radio name=usertype value=2 "
	       "onclick=show_sh_sh() disabled>�ͻ����� (�ޱ�����)</tr>"
	       "<tr><td>����</td><td>"
	       "<input type=text name=username value=\"%s\""
	       "maxlength=20 class=inputbox></tr>"
	       "<tr><td><span id=sh_mail1>�ʼ�</span>��ַ</td><td><input type=text "
	       "name=useraddress maxlength=100 class=inputbox value=\"%s\"></tr>"
	       "<tr><td><span id=sh_mail2>��������</span><span id=sh_sh1 style=\"DISPLAY: none\">�ͻ�ʱ��</span></td>"
	       "<td><span id=sh_mail3><input type=text "
	       "name=userpostcode maxlength=6 class=inputbox value=%d></span>"
	       "<span id=sh_sh2 style=\"DISPLAY: none\"><input type=text "
	       "name=sendtime maxlength=11 class=inputbox></span></tr>"
	       "<tr><td>��������(��ѡ)</td><td><input type=text "
	       "name=useremail maxlength=40 class=inputbox value=\"%s\"></tr>"
	       "<tr><td>��ϵ�绰</td><td><input type=text "
	       "name=userphone maxlength=15 class=inputbox value=\"%s\"></tr>"
	       "<tr><td valign=middle>������ʽ"
	       "<br><br><a href= " SIZEHELP
	       "><font color=red>�鿴�ߴ�����</font></a>" "</td><td>",
	       currentuser->userid, currentdata.realname, currentdata.address,
	       currentdata.postcode, currentdata.email, currentdata.phone);
	printf("<table width=100%%>");
	printf("<tr><td>��ʽ<td>�Ա�<td>��С<td>��ɫ<td>�۸�<td>����</tr>");
	while (tshirts[tshirt_num].num != 0) {
		printf("<tr><td>%4d<td>%s<td>%s<td>%s<td>%d<td>"
		       "<input type=text name=tshirt%d maxlength=2 class=inputbox></tr>",
		       tshirts[tshirt_num].num,
		       tshirts[tshirt_num].sex == 'M' ? "��" :
		       tshirts[tshirt_num].sex == 'B' ? "��/Ů" : "Ů",
		       tshirts[tshirt_num].size,
		       tshirts[tshirt_num].color,
		       tshirts[tshirt_num].price, tshirt_num);
		tshirt_num++;
	}
	printf("</table>");
	printf
	    ("<tr><td>&nbsp;</td><td><input style=\"width: 33%%\" type=reset value=��д>"
	     "<input style=\"width: 33%%\" type=submit value=�ύ></td></tr>");
	printf("<tr><td>������Ʒ<td align=left>"
	       "<p>����Ϊ�˴�л���Ա���վ���������֧�֣������ر��Ƴ�������Ʒ����ѡ��"
	       "ÿ��վ������ѡ��һ����Ʒ��������ѡ�����Ʒ����������Ҫ����������Ʒ��������"
	       "��������վ�������Ĳ��ֽ����Զ����ԡ�(���к�Ϳ��������Ǳ�վ������Ϸ��أ�"
	       "����ֵ�Ǿ���ֵһ���֡�����ɵ� <a href=bbshome?B=BBSHelp>BBSHelp</a> ���ѯ��)<p>"
	       "<div align=center>");
	while (bonusname[bonus_num] != 0) {
		printf
		    ("%s <input name=bonus%d type=text maxlength=2 class=inputbox style='width: 20px'>����",
		     bonusname[bonus_num], bonus_num);
		bonus_num++;
	}
	printf("<tr><td></td><td align=left><br><p>����������Ϣ����й¶���κε�������"
	     "������վ���⣬Ҳ������վ�ڱ�����������;��"
	     "��������κ����ʺͽ��飬������Ҫ����˵��������뵽SYSOP�������</p>"
	     "<p>�������ѡ����ȡ��ʽ���Բ�����дͨѶ��ַ���������롣</p>"
	     "<p>����������������������д��һ���ύ�������޸ġ�</p>"
	     "</table></form>");
}

int
getform(struct orderitem *item)
{
	int num = 0, amount, total = 0, i = 0, r;
	char buf[10];

	memset(item, 0, sizeof (*item));
	strsncpy(item->userid, currentuser->userid, sizeof (item->userid));
	strsncpy(item->name, getparm("username"), sizeof (item->name));
	item->type = atoi(getparm("usertype"));
	strsncpy(item->address, getparm("useraddress"), sizeof (item->address));
	strsncpy(item->postcode, getparm("userpostcode"),
		 sizeof (item->postcode));
	strsncpy(item->email, getparm("useremail"), sizeof (item->email));
	strsncpy(item->phone, getparm("userphone"), sizeof (item->phone));
	if (item->type == 2)
		strsncpy(item->postcode, getparm("sendtime"),
			 sizeof (item->postcode));

	while (tshirts[num].num != 0) {	//��ȡվ����������
		sprintf(buf, "tshirt%d", num);
		amount = atoi(getparm(buf));
		if (amount > 0) {
			item->amount[num] = amount;
			total += amount;
		} else
			item->amount[num] = 0;
		num++;
	}
	r=total;

	while (bonusname[i] != 0) {	//��ȡ��Ʒ��Ϣ
		sprintf(buf, "bonus%d", i);
		if ((amount = atoi(getparm(buf))) > 0 && total > 0) {
			item->bonus[i] = amount > total ? total : amount;
			total -= amount;
		} else
			item->bonus[i] = 0;
		i++;
	}

	item->status = 0;
	return r;
}

void
checkuserinfo(struct orderitem *item)
{
	int retv = 1;
	struct userdata currentdata;
#if 1
	if (!strcasecmp(item->userid, "guest") || item->userid == NULL) {
		printf
		    ("�Ҵҹ��Ͳ��ܶ���վ����������ѡ���¼�����¶�����<br>");
		retv = 0;
	}
#endif
	loaduserdata(item->userid, &currentdata);
	if (strlen(item->name) < 4) {
		printf("������  ���������Ĵ�����<br>");
		retv = 0;
	} else
		strsncpy(currentdata.realname, item->name,
			 sizeof (currentdata.realname));
	if (strlen(item->address) < 4) {
		printf("������  ���������ĵ�ַ��<br>");
		retv = 0;
	} else
		strsncpy(currentdata.address, item->address,
			 sizeof (currentdata.address));
	if (strlen(item->phone) < 6) {
		printf("������  ���������ĵ绰��<br>");
		retv = 0;
	} else
		strsncpy(currentdata.phone, item->phone,
			 sizeof (currentdata.phone));

	if (item->type == 0) {
		if (strlen(item->postcode) < 6) {
			printf("������  �����������ʱࡣ<br>");
			retv = 0;
		} else
			currentdata.postcode = atoi(item->postcode);
	} else if (item->type == 2) {
		if (strlen(item->postcode) < 6) {
			printf("������  �����º��ʵ��ͻ�ʱ�䡣<br>");
			retv = 0;
		}
	} else if (item->type != 1) {
		printf
		    ("������  ��ѡ���ʹ�����ȡ�����ͻ����� ���ֵ��ͷ�ʽ֮һ��<br>");
		retv = 0;
	}
	if (strlen(item->email) < 4) {
#if 0
		printf("������  ���������ĵ������䡣<br>");
		retv = 0;
#endif
	} else
		strsncpy(currentdata.email, item->email,
			 sizeof (currentdata.email));
	saveuserdata(item->userid, &currentdata);
	if (retv == 0) {
		printf("<p>�����ύʧ�ܣ����� [<a href=#"
		       " onclick=history.go(-1)>����</a>] ������һҳ������д��</p>");
		printf("</div>");
		http_quit();
	}
}

int
saveitem(struct orderitem *item)
{
	int fd;
	char title[STRLEN];

	if ((fd = open(REGFORM, O_RDWR | O_CREAT, 0600)) < 0)
		return 0;

	if (lseek(fd, file_size(REGFORM), SEEK_SET) < 0)
		return 0;

	item->time = time(NULL);
	item->seq_num = file_size(REGFORM) / itemsize;
	flock(fd, LOCK_EX);
	write(fd, item, itemsize);
	flock(fd, LOCK_UN);
	close(fd);

	sprintf(title, "[%04d] %s �ύ�¶���", item->seq_num, item->userid);
	post_record(title, item);

	return 1;
}

void
post_record(char *title, struct orderitem *tmp)
{
	int num = 0, total = 0, price = 0, postfee = 0;
	int num1 = 0, num2 = 0;
	char fname[STRLEN];
	FILE *fp;

	sprintf(fname, "bbstmpfs/tmp/tshirt.%s.%05d", currentuser->userid,
		getpid());
	if ((fp = fopen(fname, "w")) != NULL) {
		fprintf(fp, "%s\n\n", title);
		fprintf(fp, "����Ϊ������ϸ��Ϣ��\n\n");
		fprintf(fp, "�� ˮ ��\t%04d\n", tmp->seq_num);
		fprintf(fp, "����ʱ��\t%s\n", Ctime(tmp->time));
		fprintf(fp, "�� �� ID\t%s\n", tmp->userid);
		if (tmp->type == 0) {
			fprintf(fp, "�ջ���ʽ\t�ʼ�\n");
			fprintf(fp, "�� �� ��\t%s\n", tmp->name);
			fprintf(fp, "�ռ���ַ\t%s\n", tmp->address);
			fprintf(fp, "��������\t%s\n", tmp->postcode);
		} else if (tmp->type == 1) {
			fprintf(fp, "�ջ���ʽ\t��ȡ\n");
		} else if (tmp->type == 2) {
			fprintf(fp, "�ջ���ʽ\t�ͻ�����\n");
			fprintf(fp, "�� �� ��\t%s\n", tmp->name);
			fprintf(fp, "�ͻ���ַ\t%s\n", tmp->address);
			fprintf(fp, "�ͻ�ʱ��\t%s\n", tmp->postcode);
		}
		fprintf(fp, "�����ʼ�\t%s\n", tmp->email);
		fprintf(fp, "��ϵ�绰\t%s\n", tmp->phone);
		fprintf(fp, "��������\t");
		while (tshirts[num].num != 0) {
			if (tmp->amount[num] > 0) {
				total += tmp->amount[num];
				price += tmp->amount[num] * tshirts[num].price;
				if (tshirts[num].postfee == 1)
					num1 += tmp->amount[num];
				else
					num2 += tmp->amount[num];
				fprintf(fp,
					"[��ʽ%d] %s ʽ %s �� %s ɫ %d �� %d Ԫ\n\t\t",
					tshirts[num].num,
					tshirts[num].sex ==
					'M' ? "��" : tshirts[num].sex ==
					'F' ? "Ů" : "��/Ů", tshirts[num].size,
					tshirts[num].color, tmp->amount[num],
					tmp->amount[num] * tshirts[num].price);
			}
			num++;
		}
		if (num1 >= 2)
			postfee = (total - 2) * BASEPOSTFEE + MINPOSTFEE;
		else {
			if (num2 > 0)
				postfee =
				    (total - 1) * BASEPOSTFEE + MINPOSTFEE;
			else
				postfee = MINPOSTFEE;
		}
		fprintf(fp, "\n\t\t���ƣ�%d �� %d Ԫ��", total, price);
		if (tmp->type == 0)
			fprintf(fp, "�������� %d Ԫ���ܽ�� %d Ԫ��", postfee,
				postfee + price);
		if (tmp->type == 2)
			fprintf(fp, "�����ͻ��� 10 Ԫ���ܽ�� %d Ԫ��", 10 + price);
		fprintf(fp, "\n\n��ǰ״̬\t");
		switch (tmp->status) {
		case -1:
			fprintf(fp, "�����ѱ�ɾ����");
			break;
		case 0:
			fprintf(fp, "�����ȴ������С�");
			break;
		case 1:
			fprintf(fp, "�������յ��������ʼġ�");
			break;
		case 2:
			if (tmp->type == 0)
				fprintf(fp, "վ���Ѽĳ�����ע����ա�");
			else
				fprintf(fp, "���׳ɹ���");
			break;
		default:
			break;
		}
		fclose(fp);
		post_article(TSHIRTBOARD, title, fname, currentuser->userid,
			     currentuser->username, fromhost, -1, 0, 0,
			     currentuser->userid, -1);
		post_mail(tmp->userid, title, fname, currentuser->userid,
			  currentuser->username, fromhost, 0, -1);
		unlink(fname);
	}

}

void
printstatus(struct orderitem *item)
{
	int status = item->status;
	int type = item->type;

	if (status == -1)
		printf("����������");
	else if (status == 0)
		printf("�������ύ");
	else if (status == 1)
		printf("���յ������������");
	else if (status == 2 && type == 0)
		printf("�ѷ�������ע�����");
	else if (status == 2 && type == 1)
		printf("�ѽ��׳ɹ�");
	else if (status == 2 && type == 2)
		printf("���׳ɹ�, ��ȴ��ͻ�!\n");
	else
		printf("״̬��������ϵϵͳά��");
}

void
printorderdetail(struct orderitem *item)
{
	int num = 0, amount = 0, price = 0, i = 0, postfee = 0;
	int num1 = 0, num2 = 0, total = 0;

	printf("<table width=100%% align=center>");
	printf("<tr><td width=25%%>��ˮ��<td width=25%%>%04d", item->seq_num);
	printf("<td width=25%%>����ʱ��<td width=25%%>%s</tr>",
	       Ctime(item->time));
	printf("<tr><td>����ID<td>%s", item->userid);
	if (item->type == 0) {
		printf("<td>�ջ���ʽ<td>�ʼ�</tr>");
		printf("<tr><td>�ռ���<td>%s", item->name);
		printf("<td>��������<td>%s</tr>", item->postcode);
		printf("<tr><td>�ռ���ַ<td colspan=3>%s</tr>", item->address);
	} else if (item->type == 1) {
		printf("<td>�ջ���ʽ<td>��ȡ</tr>");
	} else {
		printf("<td>�ջ���ʽ<td>�ͻ�</tr>");
		printf("<tr><td>�ռ���<td>%s", item->name);
		printf("<td>�ͻ�ʱ��<td>%s</tr>", item->postcode);
		printf("<tr><td>�ͻ���ַ<td colspan=3>%s</tr>", item->address);
	}

	printf("<tr><td>�绰<td colspan=3>%s</tr>", item->phone);
	printf("<tr><td>��������<td colspan=3><p>");

	while (tshirts[num].num != 0) {
		if (item->amount[num] > 0) {
			total += item->amount[num];
			printf
			    ("[��ʽ%d] %s ʽ %s �� %s ɫ %d ������ֵ %d Ԫ��<br>",
			     tshirts[num].num,
			     tshirts[num].sex ==
			     'M' ? "��" : tshirts[num].sex ==
			     'B' ? "��/Ů" : "Ů", tshirts[num].size,
			     tshirts[num].color, item->amount[num],
			     tshirts[num].price * item->amount[num]);
			amount += item->amount[num];
			price += item->amount[num] * tshirts[num].price;
			if (tshirts[num].postfee == 1)
				num1 += item->amount[num];
			else
				num2 += item->amount[num];
		}
		num++;
	}

	printf("</p></tr>");
	if (num1 >= 2)
		postfee = (total - 2) * BASEPOSTFEE + MINPOSTFEE;
	else {
		if (num2 > 0)
			postfee = (total - 1) * BASEPOSTFEE + MINPOSTFEE;
		else
			postfee = MINPOSTFEE;
	}
	printf("<tr><td>�ۼ�<td colspan=3>վ�� <font color=red>%d</font> ����"
	       "���� <font color=red>%d</font> Ԫ��", amount, price);
	if (item->type == 0)
		printf
		    ("�������� <font color=red>%d</font> Ԫ���ܽ�� <font color=red>%d</font> Ԫ��</tr>",
		     postfee, postfee + price);
	if (item->type == 2)
		printf
		    ("�����ͻ��� <font color=red>10</font> Ԫ���ܽ�� <font color=red>%d</font> Ԫ��</tr>",
		     10 + price);
	printf("<tr><td>��Ʒ<td colspan=3>");
	while (bonusname[i] != 0) {
		if (item->bonus[i] != 0)
			printf("%s * %d����", bonusname[i], item->bonus[i]);
		i++;
	}
	printf("<tr><td>״̬<td colspan=3>");
	printstatus(item);
	printf("</tr></table>");
}

void
printmyorders(char *userid)
{
	int fd, total, i, retv = 0;
	struct orderitem *tmp;

	printf("<table width=100%% align=center>");
	printf("<tr><td>��ˮ��<td>�û���<td>����ʱ��<td>״̬<td>����</tr>");

	if ((tmp = (struct orderitem *) malloc(itemsize)) == NULL)
		http_fatal("No Memory?");

	if ((total = (file_size(REGFORM) / itemsize)) <= 0) {
		printf("<tr><td colspan=5>û���κζ�����Ϣ��</tr>");
		return;
	}

	if ((fd = open(REGFORM, O_RDWR | O_CREAT, 0600)) < 0)
		return;

	for (i = 0; i < total; i++) {
		if (lseek(fd, i * itemsize, SEEK_SET) < 0)
			http_fatal("Memory Error.");
		if (read(fd, tmp, itemsize) != itemsize)
			http_fatal("Memory Error.");
		if (!strcasecmp(tmp->userid, userid)) {
			printitemline(tmp);
			retv = 1;
		}
	}

	close(fd);

	if (!retv)
		printf("<tr><td colspan=5>û���κζ�����Ϣ��</tr>");
	printf("</table><br><br>");
}

int
isadmin()
{
	if (USERPERM(currentuser, PERM_SYSOP))
		return 1;
	if (file_has_word(TSHIRTADMIN, currentuser->userid))
		return 1;
	return 0;
}

int
ismine(int seq, struct orderitem *item)
{
	int fd;

	if (!item)
		http_fatal("Memory Error");
	if ((fd = open(REGFORM, O_RDWR | O_CREAT, 0600)) < 0)
		http_fatal("File Error");
	if (lseek(fd, itemsize * seq, SEEK_SET) < 0) {
		printf
		    ("<div class=msgbox>���������ڻ�����Ȩ�鿴�˶�����Ϣ��</div>");
		http_quit();
	}
	if (read(fd, item, itemsize) != itemsize) {
		printf
		    ("<div class=msgbox>���������ڻ�����Ȩ�鿴�˶�����Ϣ��</div>");
		http_quit();
	}
	close(fd);

	if (strcmp(item->userid, currentuser->userid))
		return 0;

	return 1;
}

void
query(int seq)
{
	struct orderitem *item;

	item = (struct orderitem *) malloc(itemsize);
	if (!item)
		http_fatal("Memory Error");

	if (!ismine(seq, item) && !isadmin()) {
		printf
		    ("<div class=msgbox>���������ڻ�����Ȩ�鿴�˶�����Ϣ��</div>");
		http_quit();
	}

	printf("<h1>�鿴��������</h1>");
	printorderdetail(item);
}

int
update(int seq, int status)
{
	int fd, oldstatus;
	struct orderitem *item;
	char title[STRLEN];

	item = (struct orderitem *) malloc(itemsize);

	if (!item)
		http_fatal("Memory Error");
	if ((fd = open(REGFORM, O_RDWR | O_CREAT, 0600)) < 0)
		http_fatal("File Error");
	if (lseek(fd, itemsize * seq, SEEK_SET) < 0) {
		close(fd);
		printf
		    ("<div class=msgbox>���������ڻ�����Ȩ����˶�����Ϣ��</div>");
		http_quit();
	}
	if (read(fd, item, itemsize) != itemsize) {
		close(fd);
		printf
		    ("<div class=msgbox>���������ڻ�����Ȩ����˶�����Ϣ��</div>");
		http_quit();
	}

	if (strcmp(item->userid, currentuser->userid) && !isadmin()) {
		close(fd);
		printf
		    ("<div class=msgbox>���������ڻ�����Ȩ����˶�����</div>");
		http_quit();
	}

	oldstatus = item->status;
	item->status = status;

	if ((status > 0 && !isadmin()) || (status == -1 && oldstatus != 0)) {
		printf
		    ("<div class=msgbox>���������ڻ�����Ȩ����˶�����</div>");
		http_quit();
	}

	flock(fd, LOCK_EX);
	if (lseek(fd, itemsize * seq, SEEK_SET) < 0) {
		close(fd);
		printf
		    ("<div class=msgbox>���������ڻ�����Ȩ����˶�����Ϣ��</div>");
		http_quit();
	}
	write(fd, item, itemsize);
	flock(fd, LOCK_UN);
	close(fd);

	printf
	    ("<script>alert(\"�����ѳɹ�\"); location.href='tshirt';</script>");
	if (status == 2)
		dobonus(item);

	sprintf(title, "[%04d] %s ������״̬�� %d ��Ϊ %d", item->seq_num,
		currentuser->userid, oldstatus, status);
	post_record(title, item);

	return 1;
}

void
admin()
{
	int fd, oper, retv = 0, newfd, i;
	int offset = atoi(getparm("offset")), pos = 0;
	int statistic[MAXTYPE + 1][4];
	char buf[STRLEN];
	struct orderitem *item;

	offset = offset >= 0 ? offset : 0;
	bzero(statistic, sizeof (statistic));

	if (!isadmin())
		http_fatal("û�й���Ȩ�޻��¼�ѳ�ʱ��");

	if ((item = (struct orderitem *) malloc(itemsize)) == NULL)
		http_fatal("Memory Error.");

	if ((fd = open(REGFORM, O_RDWR | O_CREAT, 0600)) < 0)
		http_fatal("File Error.");

	printf("<table width=100%%><tr>"
	       "<td width=9%%><a href='tshirt?admin=0'>���ж���</a>"
	       "<td width=10%%><a href='tshirt?admin=1'>��ɾ������</a>"
	       "<td width=10%%><a href='tshirt?admin=2'>δ������</a>"
	       "<td width=10%%><a href='tshirt?admin=3'>�ѵ����</a>"
	       "<td width=10%%><a href='tshirt?admin=4'>�ѽ��׶���</a>"
	       "<td width=6%%><a href='tshirt?admin=8'>ͳ��</a>"
	       "<td width=13%%><a href='tshirt?admin=7'><font color=green>�޸�������Ϣ</font></a></td>"
	       "<form action=tshirt?admin=5 method=post>"
	       "<td width=14%%><input type=text class=inputbox name=quserid"
	       " maxlength=14 style='width: 50%%'>&nbsp;"
	       "<input type=submit value='ID'>"
	       "</td></form>"
	       "<form action=tshirt?admin=6 method=post>"
	       "<td width=18%%><input type=text class=inputbox name=qseq "
	       "maxlength=4 style='width: 50%%'>&nbsp;"
	       "<input type=submit value='��ˮ��'></td></form>"
	       "</tr></table><br><br>");

	oper = atoi(getparm("admin"));
	switch (oper) {
	default:
	case 0:
		printoffsetnav(0, offset);
		printitemhead();
		while (read(fd, item, itemsize) == itemsize) {
			if (pos >= PAGESIZE * offset
			    && pos < PAGESIZE * (offset + 1)) {
				printitemline(item);
				retv = 1;
			}
			pos++;
		}
		if (!retv)
			printf("<tr><td colspan=5>û���ҵ����������Ķ���</tr>");
		printitemend();
		printoffsetnav(0, offset);
		break;

	case 1:
		printitembystatus(fd, item, -1, offset, 1);
		break;

	case 2:
		printitembystatus(fd, item, 0, offset, 2);
		break;

	case 3:
		printitembystatus(fd, item, 1, offset, 3);
		break;

	case 4:
		printitembystatus(fd, item, 2, offset, 4);
		break;

	case 5:
		printmyorders(getparm("quserid"));
		break;

	case 6:
		if (lseek(fd, itemsize * atoi(getparm("qseq")), SEEK_SET) < 0) {
			close(fd);
			http_fatal("File Error");
		}
		if (read(fd, item, itemsize) != itemsize) {
			close(fd);
			http_fatal("File Error");
		}

		printitemhead();
		printitemline(item);
		printitemend();
		break;
	case 7:
		sprintf(buf, "%s.tmp", REGFORM);
		if ((newfd = open(buf, O_RDWR | O_CREAT, 0600)) < 0)
			http_fatal("File Error.");
		flock(fd, LOCK_EX);
		while (read(fd, item, itemsize) == itemsize) {
			i = 0;
			while (tshirts[i].num != 0) {
				if (item->amount[i] < 0 || item->amount[i] > 99)
					item->amount[i] = 0;
				i++;
			}
			flock(newfd, LOCK_EX);
			write(newfd, item, itemsize);
			flock(newfd, LOCK_UN);
		}
		flock(fd, LOCK_UN);
		close(newfd);
		close(fd);
		unlink(REGFORM);
		rename(buf, REGFORM);
		printf("<div class=msgbox>�����޸��ɹ�</div>");
		return;
	case 8:
		while (read(fd, item, itemsize) == itemsize) {
			i = 0;
			while (tshirts[i].num != 0) {
				if (item->amount[i] > 0)
					statistic[i][item->status + 1]++;
				i++;
			}
		}
		printf("<table width=100%%>"
		       "<tr><td width=16%%>��ʽ"
		       "<td width=12%%>��ɾ��[-1]"
		       "<td width=12%%>δ����[0]"
		       "<td width=12%%>�ѵ���[1]"
		       "<td width=12%%>�ѽ���[2]"
		       "<td width=12%%>�ܼ�" "<td width=12%%>��Ч[��-1]");
		i = 0;
		while (tshirts[i].num != 0) {
			printf("<tr><td>��ʽ%d [%s %s]"
			       "<td>%d"
			       "<td>%d"
			       "<td>%d"
			       "<td>%d"
			       "<td>%d"
			       "<td>%d",
			       tshirts[i].num,
			       tshirts[i].size,
			       tshirts[i].color,
			       statistic[i][0],
			       statistic[i][1],
			       statistic[i][2],
			       statistic[i][3],
			       statistic[i][0] + statistic[i][1] +
			       statistic[i][2] + statistic[i][3],
			       statistic[i][1] + statistic[i][2] +
			       statistic[i][3]);
			i++;
		}
		i = 0;
		while (tshirts[i].num != 0) {
			statistic[MAXTYPE][0] += statistic[i][0];
			statistic[MAXTYPE][1] += statistic[i][1];
			statistic[MAXTYPE][2] += statistic[i][2];
			statistic[MAXTYPE][3] += statistic[i][3];
			i++;
		}
		printf("<tr><td>�ܼ�<td>%d<td>%d<td>%d<td>%d<td>%d<td>%d",
		       statistic[MAXTYPE][0],
		       statistic[MAXTYPE][1],
		       statistic[MAXTYPE][2],
		       statistic[MAXTYPE][3],
		       statistic[MAXTYPE][0] + statistic[MAXTYPE][1] +
		       statistic[MAXTYPE][2] + statistic[MAXTYPE][3],
		       statistic[MAXTYPE][1] + statistic[MAXTYPE][2] +
		       statistic[MAXTYPE][3]);
		printf("</table><br><br>");
		break;
	}
	close(fd);
}

void
printitemhead()
{
	printf("<table width=100%%>");
	printf("<tr><td>��ˮ��<td>����ID<td>����ʱ��<td>״̬<td>����</tr>");
}

void
printitemline(struct orderitem *item)
{
	printf("<tr><td>%04d<td>%s<td>%s<td>", item->seq_num, item->userid,
	       Ctime(item->time));
	printstatus(item);
	printf("<td>");
	printitemoper(item);
	printf("</tr>");
}

void
printitemend()
{
	printf("</table><br><br>");
}

void
printitemoper(struct orderitem *item)
{
	int admin = 0;
	int status = item->status;
	int seq = item->seq_num;

	if (isadmin())
		admin = 1;

	printf("<a href=tshirt?action=query&seq=%d>�鿴����</a>��", seq);

	if (status == -1)
		printf("<a href=tshirt?action=update&seq=%d&status=0>�ָ�</a>",
		       seq);
	else if (status == 0) {
		printf
		    ("<a href=tshirt?action=update&seq=%d&status=-1>ɾ��</a>��",
		     seq);
		if (item->type == 0 && admin)
			printf
			    ("<a href=tshirt?action=update&seq=%d&status=1>�յ�����</a>",
			     seq);
		else if (admin)
			printf
			    ("<a href=tshirt?action=update&seq=%d&status=2>���׳ɹ�</a>",
			     seq);
	} else if (status == 1 && admin) {
		printf
		    ("<a href=tshirt?action=update&seq=%d&status=0>ȡ���տ�</a>��",
		     seq);
		printf
		    ("<a href=tshirt?action=update&seq=%d&status=2>�ĳ�վ��</a>",
		     seq);
	} else if (status == 2 && admin) {
		if (item->type == 0)
			printf
			    ("<a href=tshirt?action=update&seq=%d&status=1>ȡ���ĳ�</a>",
			     seq);
		else
			printf
			    ("<a href=tshirt?action=update&seq=%d&status=0>ȡ������</a>",
			     seq);
	}

}

void
printoffsetnav(int admintype, int offset)
{
	printf("<div align=right>");
	if (offset != 0)
		printf("<a href='tshirt?admin=%d&offset=%d'>��һҳ</a>��",
		       admintype, offset - 1);
	else
		printf("��һҳ��");
	printf("<a href='tshirt?admin=%d&offset=%d'>��һҳ</a><br><br>",
	       admintype, offset + 1);
}

void
printitembystatus(int fd, struct orderitem *item, int status, int offset,
		  int admintype)
{
	int retv = 0;
	int pos = 0;
	printoffsetnav(admintype, offset);
	printitemhead();
	while (read(fd, item, itemsize) == itemsize) {
		if (item->status == status) {
			if (pos >= PAGESIZE * offset
			    && pos < PAGESIZE * (offset + 1)) {
				printitemline(item);
				retv = 1;
			}
			pos++;
		}
	}
	if (!retv)
		printf("<tr><td colspan=5>û���ҵ����������Ķ���</tr>");
	printitemend();
	printoffsetnav(status, offset);
}

//��ʼ����������
static int
initmcData(char *filepath)
{
	int fd;
	mcUserInfo ui;
	void *ptr = NULL;
	size_t filesize = 0;

	filesize = sizeof (mcUserInfo);
	bzero(&ui, filesize);
	ui.version = MONEYCENTER_VERSION;
	ptr = &ui;
	ui.con = random() % 10;

	if ((fd = open(filepath, O_CREAT | O_EXCL | O_WRONLY, 0660)) == -1)
		return -1;
	write(fd, ptr, filesize);
	close(fd);
	return 0;
}

void
dobonus(struct orderitem *item)
{
	char buf[512];
	struct userdata data;
	mcUserInfo *mcuInfo;

	sprintf(buf, MY_BBS_HOME "/home/%c/%s/mc.save",
		mytoupper(item->userid[0]), item->userid);
	if (!file_exist(buf))
		initmcData(buf);	//�ļ������ڣ���ʼ��
	if((mcuInfo = loadData(buf, sizeof (mcUserInfo))) == (void*)-1)
		return;

	if (item->bonus[0] != 0 && item->bonus[0] < 100) {
		mcuInfo->credit += 200000 * item->bonus[0];	//�ݶ�����20��/��
	}

	if (item->bonus[1] != 0 && item->bonus[0] < 100) {
		mcuInfo->con += 2 * item->bonus[1];	//�ݶ����Ǽ�2/��
	}

	if (item->bonus[2] != 0 && item->bonus[0] < 100) {
		loaduserdata(item->userid, &data);
		data.extraexp += 20 * item->bonus[2];	//�ݶ�����ֵ��20/��
		saveuserdata(item->userid, &data);
	}

	unloadData(mcuInfo, sizeof (mcUserInfo));
	//...�������ƣ���ൽbonus[9] != 0��Ŀǰֻ��2

	return;
}

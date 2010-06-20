#include "bbslib.h"
#define AUTO_REG 1
#define	MAX_INVINTE 50
#define isletter(a)	(a>= 'A' && a <= 'Z')?1:((a>= 'a' && a <= 'z')?1:0)

#ifdef USESESSIONCOOKIE
extern char sessionCookie[];
#endif

int
verifyInvite(char *email)
{
	char inviter[30], code[30], t[30];
	char invitefn[256];
	char ivemail[80];
	strsncpy(inviter, getparm("inviter"), sizeof (inviter));
	strsncpy(code, getparm("code"), sizeof (code));
	strsncpy(t, getparm("t"), sizeof (t));
	sprintf(invitefn, INVITATIONDIR "/%s/%s.%s", inviter, t, code);
	close(open(invitefn, O_RDWR));	//touch new
	if (!file_exist(invitefn) ||
	    readstrvalue(invitefn, "toemail", ivemail, sizeof (ivemail)) < 0)
		return 0;
	if (strcasecmp(email, ivemail))
		return 0;
	return 1;
}

int
postInvite(char *userid, char *inviter)
{
	struct userec *x, tmpu;
	char buf[256];
	FILE *fp;
	if (getuser(inviter, &x) <= 0)
		return -1;
	loadfriend(userid);
	if (friendnum < 199) {
		strsncpy(fff[friendnum].id, x->userid, sizeof (fff[0].id));
		strsncpy(fff[friendnum].exp, inviter, sizeof (fff[0].exp));
		friendnum++;
		sethomefile(buf, userid, "friends");
		fp = fopen(buf, "w");
		fwrite(fff, sizeof (struct override), friendnum, fp);
		fclose(fp);
	}

	loadfriend(x->userid);
	if (friendnum < 199) {
		strsncpy(fff[friendnum].id, userid, sizeof (fff[0].id));
		strsncpy(fff[friendnum].exp, getparm("name"),
			 sizeof (fff[0].exp));
		friendnum++;
		sethomefile(buf, x->userid, "friends");
		fp = fopen(buf, "w");
		fwrite(fff, sizeof (struct override), friendnum, fp);
		fclose(fp);
	}
	tmpu = *x;
	if (tmpu.extraexp1 >= MAX_INVINTE ) {
		tmpu.extraexp1 = MAX_INVINTE;
		snprintf(buf, sizeof (buf),
			 "������� %s �Ѿ�ע���ˣ��û���Ϊ %s��"
			 "��������ѻ�õĸ��Ӿ���ֵ�ﵽ���ޣ�", getparm("name"), userid);
	} else {
		tmpu.extraexp1 ++;
		snprintf(buf, sizeof (buf),
			 "������� %s �Ѿ�ע���ˣ��û���Ϊ %s��"
			 "���ľ���ֵ���� 20 �㣡", getparm("name"), userid);
	}
	updateuserec(&tmpu, 0);
	system_mail_buf(buf, strlen(buf), x->userid, "��������ע����", userid);

	return 0;
}

int
checkRegPass(char *iregpass, const char *userid)
{
	struct MD5Context mdc;
	unsigned int regmd5[4];
	char regpass[5];
	//��¼�� ID ����Ϣ...
	MD5Init(&mdc);
	MD5Update(&mdc, (void *) (&bbsinfo.ucachehashshm->regkey),
		  sizeof (int) * 4);
	MD5Update(&mdc, userid, strlen(userid));
	MD5Final((char *) regmd5, &mdc);
	sprintf(regpass, "%d%d%d%d", regmd5[0] % 10, regmd5[1] % 10,
		regmd5[2] % 10, regmd5[3] % 10);
	if (!strcmp(regpass, iregpass))
		return 0;
//              errlog("%s regpass:%s iregpass:%s", x.userid, regpass, iregpass);
	MD5Init(&mdc);
	MD5Update(&mdc, (void *) (&bbsinfo.ucachehashshm->oldregkey),
		  sizeof (int) * 4);
	MD5Update(&mdc, userid, strlen(userid));
	MD5Final((char *) regmd5, &mdc);
	sprintf(regpass, "%d%d%d%d", regmd5[0] % 10, regmd5[1] % 10,
		regmd5[2] % 10, regmd5[3] % 10);
	if (strcmp(regpass, iregpass))
//                     errlog("%s regpass:%s iregpass:%s", x.userid, regpass, iregpass);
		//��֤�����
		return -1;
	if (now_t - bbsinfo.ucachehashshm->keytime > 300) {
//              errlog("too long time %d",
//                     (int)(now_t - bbsinfo.ucachehashshm->keytime));
		//http_fatal("��֤����ڣ�����̫���˰�");
		return -2;
	}
	return 0;
}

int
bbsdoreg_main()
{
	FILE *fp;
	struct userec x;
	struct userdata xdata;
	char filename[80], pass1[80], pass2[80], dept[80], phone[80], assoc[80],
	    words[1024], buf[80], buf2[80], *ub = FIRST_PAGE, *ptr;
	char iregpass[5];
	char md5pass[MD5LEN];
	//int lockfd;
	//struct MD5Context mdc;
	//unsigned int regmd5[4];
	//char regpass[5];
	int r = 0, emailcheck = 0, invited = 0;
  char url[10];
	html_header(1);
	printf("<body>");
	bzero(&x, sizeof (x));
	bzero(&xdata, sizeof (xdata));
	strsncpy(x.userid, getparm("userid"), IDLEN + 1);
	strsncpy(pass1, getparm("pass1"), PASSLEN);
	strsncpy(pass2, getparm("pass2"), PASSLEN);
	strsncpy(x.username, getparm("username"), sizeof (x.username));
	strsncpy(xdata.realname, getparm("realname"), sizeof (xdata.realname));
	strsncpy(dept, getparm("dept"), sizeof (dept));
	strsncpy(xdata.address, getparm("address"), sizeof (xdata.address));
	strsncpy(xdata.email, strtrim(getparm("email")), sizeof (xdata.email));
	strsncpy(phone, getparm("phone"), sizeof (phone));
	strsncpy(assoc, getparm("assoc"), sizeof (assoc));
	strsncpy(words, getparm("words"), sizeof (words));
	strsncpy(iregpass, getparm("regpass"), 5);
#ifdef ENABLE_INVITATION
	if (!strcasecmp(getparm("useemail"), "on"))
		emailcheck = 1;
	else if (atoi(getparm("invited")) == 1)
		invited = 1;
#endif

	if (!goodgbid(x.userid))
		http_fatal("�ʺ�ֻ����Ӣ����ĸ�ͱ�׼�������");
	if (strlen(x.userid) < 2 || (strlen(x.userid) == 2 && !(isletter(x.userid[0]) && isletter(x.userid[1])) ) )
		http_fatal("�ʺų���̫��(2-12����Ӣ���ַ�)");
	if (strlen(pass1) < 4)
		http_fatal("����̫��(����4�ַ�)");
	if (strcmp(pass1, pass2))
		http_fatal("������������벻һ��, ��ȷ������");
	if (strlen(x.username) < 2)
		http_fatal("�������ǳ�(�ǳƳ�������2���ַ�)");
	if (strlen(xdata.realname) < 4)
		http_fatal("����������(��������, ����2����)");
	if (strlen(xdata.address) < 4 && !invited)
		http_fatal("�����������ڵĳ���(��������, ����2����)");
	if (strcasecmp(xdata.email, strtrim(getparm("email2")))) {
		printf("������д�� email ��ַ��һ�¡�");
		http_fatal("��������д email ��ַ��");
	}		
	if (!trustEmail(xdata.email)) {
		printf
		    ("���ṩ�� email ����ϵͳ�϶���Χ�����߱����ע�� ID ʹ�ù���");
		http_fatal("������ѡ�� email��\n");
	}
#if 0
	if (!emailcheck && !invited) {
		if (strlen(dept) < 14)
			http_fatal
			    ("ѧУϵ��������λ�����Ƴ�������Ҫ14���ַ�(��7������)");
		if (strlen(xdata.address) < 16)
			http_fatal("ͨѶ��ַ��������Ҫ16���ַ�(��8������)");
	}
#endif
	if (badstr(x.passwd) || badstr(x.username)
	    || badstr(xdata.realname))
		http_fatal("����ע�ᵥ�к��зǷ��ַ� (���롢�ǳơ���ʵ����)");
	if (badstr(xdata.address) || badstr(xdata.email))
		http_fatal("����ע�ᵥ�к��зǷ��ַ� (ͨѶ��ַ��email)");
	if (is_bad_id(x.userid))
		http_fatal("�����ʺŻ��ֹע���id, ������ѡ��");
		
	if ((emailcheck || invited) && !trustEmail(xdata.email)) {
		//... is it in the range?
		printf
		    ("���ṩ�� email ����ϵͳ�϶���Χ�����߱����ע�� ID ʹ�ù���");
		http_fatal("������ѡ�� email������ʹ���˹�ע�ᡣ\n");
	}
	 if (invited && !verifyInvite(xdata.email)) {
	        printf
	         ("û���ҵ���Ӧ�������¼����ʹ��<a href=bbsemailreg>��ͨע�᷽ʽ</a>");
	        return 0;
	 }

	//��¼�� ID ����Ϣ...
	switch (checkRegPass(iregpass, x.userid)) {
	case -1:
		http_fatal("��֤�����");
	case -2:
		http_fatal("��֤����ڣ�����̫���˰�");
	default:
		break;
	}

	x.salt = getsalt_md5();
	genpasswd(md5pass, x.salt, pass1);
	memcpy(x.passwd, md5pass, MD5LEN);
	x.lasthost = from_addr.s_addr;
#ifndef AUTO_REG
	x.userlevel = PERM_BASIC;
#else
	x.userlevel = PERM_DEFAULT;
#endif
	x.firstlogin = now_t;
	x.lastlogin = now_t;
	x.numlogins = 1;
	x.userdefine = 0xffffffff;
	x.flags[0] = CURSOR_FLAG | PAGER_FLAG;
	switch (insertuserec(&x)) {
	case -1:
		http_fatal("�޷�ע�ᣬ����ԭ��ע���û��Ѵ�����");
		break;
	case 0:
		r = user_registered(x.userid);
		if (r > 0)
			http_fatal("���ʺ��Ѿ�����ʹ��,������ѡ��");
		else if (r < 0)
			http_fatal
			    ("���ʺŸո�������������������ע�ᣬ������ѡ��id");
		else {
			errlog("www reg error: %s", x.userid);
			http_fatal("�ڲ�����");
		}
		break;
	}
	sethomefile(filename, x.userid, "");
	mkdir(filename, 0755);
	saveuserdata(x.userid, &xdata);
	if (invited) {
		FILE *fp;
		char *ptr, *iv = getparm("inviter");
		if ((ptr = strchr(iv, '\n')))
			*ptr = 0;
		sethomefile(filename, x.userid, "mailcheck");
		fp = fopen(filename, "w");
		if (!fp) {
			errlog("can't open %s", filename);
			http_fatal("�ڲ�����");
		}
		fprintf(fp, "firstlogin %ld\n", now_t);
		fprintf(fp, "realname %s\n", xdata.realname);
		fprintf(fp, "inviter %s\n", iv);
		fprintf(fp, "lasthost %s\n", realfromhost);
		fprintf(fp, "email %s\n", xdata.email);
		fclose(fp);
		doConfirm(&x, xdata.email, 1);
		postInvite(x.userid, iv);
	}

	if (emailcheck) {
		if (send_emailcheck(&x, &xdata) < 0)
			emailcheck = 0;
	}
	if ( !emailcheck && !invited){
#ifndef AUTO_REG
		//һ���ֹ�����
		lockfd = openlockfile(".lock_new_register", O_RDONLY, LOCK_EX);
		fp = fopen("new_register", "a");
		if (fp) {
			fprintf(fp, "usernum: %d, %s\n", getuser(x.userid, NULL),
			Ctime(now_t));
			fprintf(fp, "userid: %s\n", x.userid);
			fprintf(fp, "realname: %s\n", xdata.realname);
			fprintf(fp, "dept: %s\n", dept);
			fprintf(fp, "addr: %s\n", xdata.address);
			fprintf(fp, "phone: %s\n", phone);
			fprintf(fp, "assoc: %s\n", assoc);
			fprintf(fp, "rereg: 0\n");
			fprintf(fp, "----\n");
			fclose(fp);
		}
		close(lockfd);
#else
		//һ���Զ�����
		sethomefile(buf, x.userid, "register");
		if (file_exist(buf)) {
			sethomefile(buf2, x.userid, "register.old");
			rename(buf, buf2);
		}
		if ((fp = fopen(buf, "w")) != NULL) {
			fprintf(fp, "usernum: %d, %s", getuser(x.userid, NULL),
				Ctime(now_t));
			fprintf(fp, "realname: %s\n", xdata.realname);
			fprintf(fp, "dept: %s\n", dept);
			fprintf(fp, "addr: %s\n", xdata.address);
			fprintf(fp, "phone: %s\n", phone);
			fprintf(fp, "assoc: %s\n", assoc);
			fprintf(fp, "rereg: 0\n");
			fprintf(fp, "----\n");
			fclose(fp);
		}
		mail_file("etc/s_fill", x.userid, "������ͨ�������֤", "SYSOP");
		mail_file("etc/s_fill2", x.userid,
			  "��ӭ����" MY_BBS_NAME "���ͥ", "SYSOP");
#endif
	}
	printf("<center><table><td><td><pre>\n");
	printf("�װ�����ʹ���ߣ����ã�\n\n");
	printf("��ӭ���� ��վ, �������ʺ��Ѿ��ɹ����Ǽ��ˡ�\n");
	printf("��ҳ�������Ļ��ܺ���˽��Ϣ���뾡���������\"���ڽ���" MY_BBS_ID "\"��رմ�ҳ�档\n");
	printf("��Ŀǰӵ�б�վ������Ȩ��, �����Ķ����¡������ķ�������˽��\n");
	printf("�ż����������˵���Ϣ�����������ҵȵȡ�\n");
	printf("���������Է������²������ۡ�\n");
#if 0
	if (invited) {
		printf("���������Է������²������ۡ�\n");
	} else {
		printf
		    ("����ͨ����վ�����ȷ������֮���������ø����Ȩ�ޡ�\n");
	}
	if (!emailcheck && !invited) {
		printf
		    ("Ŀǰ����ע�ᵥ�Ѿ����ύ�ȴ����ġ�һ����� 24 Сʱ���ھͻ�\n");
		printf("�д𸴣������ĵȴ���ͬʱ����������վ�����䡣\n");
	} else if (emailcheck) {
		printf("Email ȷ�����Ѿ����� %s����ע����ղ��ظ���\n"
		       "����ղ���ȷ���ţ�����д��߲˵��е�ע�ᵥ��\n",
		       xdata.email);
	}
#endif
	printf("��������κ����ʣ�����ȥ BBSHelp (BBS����) �淢��������\n\n"
	       "</pre></table>");
	printf("<hr><br>���Ļ�����������:<br>\n");
	printf("<table border=1 width=400>");
	printf("<tr><td>�ʺ�λ��: <td>%d\n", getuser(x.userid, NULL));
	printf("<tr><td>ʹ���ߴ���: <td>%s (%s)\n", x.userid, x.username);
	printf("<tr><td>��  ��: <td>%s<br>\n", xdata.realname);
	printf("<tr><td>��  ��: <td>%s<br>\n", x.username);
	printf("<tr><td>���ڳ���: <td>%s<br>\n", xdata.address);
	printf("<tr><td>��վλ��: <td>%s<br>\n", inet_ntoa(from_addr));
	printf("<tr><td>�����ʼ�: <td>%s<br></table><br>\n", xdata.email);
	newcomer(&x, words);
	tracelog("%s newaccount %d %s www", x.userid, getuser(x.userid, NULL),
		 realfromhost);
	wwwstylenum = 6;
	ub = wwwlogin(&x, 0, 0);
	ptr = getsenv("HTTP_X_FORWARDED_FOR");
	tracelog("%s enter %s www %s", x.userid, realfromhost, ptr);
#ifdef USESESSIONCOOKIE
	printf("<script>document.cookie='SESSION=%s; path=/; </script>", urlencode(sessionCookie));
#endif
	printf
            ("<center><form><input type=button onclick="
             "'top.location.href=\"/%s/bbslogin?id=%s&pw=%s&regjump=1\";' value=\"���ڽ���" MY_BBS_ID
             "\" ></form></center>\n", SMAGIC, x.userid, pass1);
/*
	printf
	    ("<center><form name=jump action=/" SMAGIC "/bbslogin method=post><input type=hidden name=regjump value=1><input type=hidden name=t value=''><input type=hidden name=lastip1 value=''>""<input type=hidden name=lastip2 value=''><input type=hidden name=id value=%s><input type=hidden name=pw value=%s><input type=hidden name=ipmask value=5><input type=submit value=���ڽ���" MY_BBS_ID"></form></center><script language=javascript>jump.submit();</script>\n",x.userid,pass1); */
/*	printf
	    ("<center><form><input type=button onclick="
	    "'top.location.href=\"http://%s/%s?t=%d\";'value=���ڽ���" MY_BBS_ID
	    "><form></center>\n",MY_BBS_DOMAIN,ub,(int)now_t);*/
	return 0;
}

int
badstr(unsigned char *s)
{
	int i;
	for (i = 0; s[i]; i++)
		if (s[i] != 9 && (s[i] < 32 || s[i] == 255))
			return 1;
	return 0;
}

void
newcomer(struct userec *x, char *words)
{
	FILE *fp;
	char filename[80];
	sprintf(filename, "bbstmpfs/tmp/%d.tmp", thispid);
	fp = fopen(filename, "w");
	fprintf(fp, "��Һ�, \n\n");
	fprintf(fp, "���� %s(%s), ���� %s\n", x->userid, x->username, fromhost);
	fprintf(fp, "��������˵ر���, ���Ҷ��ָ��.\n\n");
	fprintf(fp, "���ҽ���:\n\n");
	fprintf(fp, "%s", words);
	fclose(fp);
	post_article("newcomers", "WWW������·", filename, x->userid,
		     x->username, fromhost, -1, 0, 0, x->userid, -1);
	unlink(filename);
}

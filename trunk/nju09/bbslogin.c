#include "bbslib.h"

static char *check_multi(char *id, int uid);

int ajax_login() {
	printf("Content-type: text/html; charset=%s\r\n\r\n", CHARSET);
	printf("�û�����%s<br />���룺%s", 
			getparm("id"), getparm("pwd"));
	return 0;
}

int ajax_check_login() {
	printf("Content-type: text/html; charset=%s\r\n\r\n", CHARSET);
	if (loginok && !isguest)
		printf("%s", currentuser->userid);
	else
		printf("guest");
	return 0;
}

int
extandipmask(int ipmask, char *ip1, char *ip2)
{
	struct in_addr addr1, addr2;
	if (!*ip1 || !*ip2 || !strcmp(ip1, ip2))
		return ipmask;
	if (!inet_aton(ip1, &addr1) || !inet_aton(ip2, &addr2))
		return ipmask;
	while (ipmask < 15
	       && (addr1.s_addr << ipmask) != (addr2.s_addr << ipmask))
		ipmask++;
	return ipmask;
}

int 
bbscms_main()
{
	int n, t, infochanged = 0;
	time_t dtime;
	char filename[128], buf[256], *ptr;
	char id[IDLEN+1], pw[PASSLEN], url[10], *ub = FIRST_PAGE;
	char md5pass[MD5LEN];
	struct userec *x, tmpu;
	int ipmask, usernum, utf8, ajax, regjump;
#ifdef USESESSIONCOOKIE
	extern char sessionCookie[];
#endif

	ajax = atoi(getparm("ajax"));
	html_header(3);

if(!loginok)
{
//	printf("Trace log: login is not ok\n");
	utf8 = atoi(getparm("utf8"));
	regjump=atoi(getparm("regjump"));
	if(utf8) {
		strsncpy(buf, strtrim(getparm("id")), sizeof(buf));
		utf82gb(id, sizeof(id),	buf);
	} else
		strsncpy(id, strtrim(getparm("id")), sizeof(id));

	strsncpy(pw, getparm("pw"), PASSLEN);
	strsncpy(url, getparm("url"), 3);
	ipmask = atoi(getparm("ipmask"));

	if (!id[0]) {
		strcpy(id, "guest");
		ipmask = 8;
	}

	if ((usernum = login_get_user(id, &x)) <= 0) {
		if (ajax)
			ajax_fatal("�����ʹ�����ʺ�(%s)", id);
		else
			http_fatal("�����ʹ�����ʺ�(%s)", id);
	}
	if (strcmp(id, "guest")) {
		ipmask = extandipmask(ipmask, 
				getparm("lastip1"), realfromhost);
		ipmask = extandipmask(ipmask, 
				getparm("lastip2"), realfromhost);
	}
	strcpy(id, x->userid);
	if (strcasecmp(id, "guest")) {
		t = x->lastlogin;
		memcpy(&tmpu, x, sizeof (tmpu));
		if (tmpu.salt == 0) {
			tmpu.salt = getsalt_md5();
			genpasswd(md5pass, tmpu.salt, pw);
			memcpy(tmpu.passwd, md5pass, MD5LEN);
			infochanged = 1;
		}
		if (count_uindex(usernum) == 0) {
			if (now_t - t > 1800)
				tmpu.numlogins++;
			infochanged = 1;
			tmpu.lastlogin = now_t;
			dtime = t - 4 * 3600;
			t = localtime(&dtime)->tm_mday;
			dtime = now_t - 4 * 3600;
			if (t < localtime(&dtime)->tm_mday
			    && x->numdays < 60000) {
				tmpu.numdays++;
			}
		}
		if (x->lasthost != from_addr.s_addr) {
			tmpu.lasthost = from_addr.s_addr;
			infochanged = 1;
		}
		if (infochanged)
			updateuserec(&tmpu, 0);
		currentuser = x;
	}
	ptr = getsenv("HTTP_X_FORWARDED_FOR");
	tracelog("%s enter %s www %d %s", x->userid, realfromhost, infochanged,
		 ptr);
	n = 0;
	if (loginok && isguest) {
		u_info->wwwinfo.iskicked = 1;
	}
	//����web style
	if (strcasecmp(id, "guest")) {
		sethomepath(filename, x->userid);
		mkdir(filename, 0755);

		strsncpy(buf, getparm("style"), 3);
		wwwstylenum = -1;
		if (isdigit(buf[0]))
			wwwstylenum = atoi(buf);
		if ((wwwstylenum > NWWWSTYLE || wwwstylenum < 0))
			if (!readuservalue
			    (x->userid, "wwwstyle", buf, sizeof (buf)))
				wwwstylenum = atoi(buf);
		if (wwwstylenum < 0 || wwwstylenum >= NWWWSTYLE)
			wwwstylenum = DEFAULT_WWWSTYLE;
		currstyle = &wwwstyle[wwwstylenum];
	} else {
		wwwstylenum = DEFAULT_WWWSTYLE;
		currstyle = &wwwstyle[wwwstylenum];

	}
		ub = wwwlogin(x, ipmask, ajax);
	if (!ajax)
		printf("<script>document.cookie='anonypid=%d; "
			"path=/;'</script>", thispid);
#ifdef USESESSIONCOOKIE
	if (!ajax)
		printf("<script>document.cookie='SESSION=%s; "
			"path=/;';</script>", urlencode(sessionCookie));
#endif
	if (!strcmp(url, "1")) {
		printf
		    ("<script>\n"
		     "function URLencode(sStr) {\n"
		     "return escape(sStr).replace(/\\+/g, '%%2C').replace(/\\\"/g,'%%22').replace(/\\'/g, '%%27');\n"
		     "}\n"
		     "a=window.opener.location.href;\n" "l=a.length;\n"
		     "t=a.indexOf('/" SMAGIC "',1);\n" "t=a.indexOf('/',t+1);\n"
		     "nu=\"%s\"+\"?t=%ld&b=\"+URLencode(a.substring(t+1,l));\n"
		     "opener.top.location.href=nu;window.close();</script>",
		     ub, now_t);
	} else {
		char buf[256];
#ifdef USESESSIONCOOKIE
		if (ajax) {
			char *ptr;
			if ((ptr = strchr(ub + 1, '/')))
				*ptr = 0;
			sprintf(ub, "%s%s/", ub, sessionCookie);
		}
#endif
		if (strcmp(x->userid, "guest") && shouldbroadcast(usernum)) {
			sprintf(buf, "%s?t=%d&b=ooo", ub, (int) now_t);
#ifdef ENABLE_BLOG
		} else if(atoi(getparm("blog"))) {
			sprintf(buf, "%sblogpage", ub);
#endif
		} else
			sprintf(buf, "%s?t=%d", ub, (int) now_t);
		if (ajax) {
			printf("%s/%d", buf, thispid);
			http_quit();
		}
	}
}  // !loginok

	printf("</head>\n<body></body></html>");
	http_quit();
	return 0;
}


int
bbslogin_main()
{
	int n, t, infochanged = 0;
	time_t dtime;
	char filename[128], buf[256], *ptr;
	char id[IDLEN+1], pw[PASSLEN], url[10], *ub = FIRST_PAGE;
	char md5pass[MD5LEN];
	struct userec *x, tmpu;
	int ipmask, usernum, utf8, ajax,regjump;
#ifdef USESESSIONCOOKIE
	extern char sessionCookie[];
#endif

	ajax = atoi(getparm("ajax"));
	if (ajax == 2) {	//ajax����¼���ƺ�Ŀǰ�Ƕ���ġ�
		ajax_check_login();
		return 0;
	}
	if (ajax)
		printf("Content-type: text/html; charset=%s\r\n\r\n", CHARSET);
	else
		html_header(3);

	utf8 = atoi(getparm("utf8"));
	regjump=atoi(getparm("regjump"));
	if(utf8) {
		strsncpy(buf, strtrim(getparm("id")), sizeof(buf));
		utf82gb(id, sizeof(id),	buf);
	} else
		strsncpy(id, strtrim(getparm("id")), sizeof(id));

	strsncpy(pw, getparm("pw"), PASSLEN);
	strsncpy(url, getparm("url"), 3);
	ipmask = atoi(getparm("ipmask"));

	if (!id[0]) {
		strcpy(id, "guest");
		ipmask = 8;
	}

	//���д���̫���֡�����
	if (!strcmp(MY_BBS_ID, "YTHT") && !strcmp(id, "guest")) {
		http_fatal("�������û����������Ե�¼��");
	}
	if (strcmp(id, "guest")) {
		ipmask = extandipmask(ipmask, 
				getparm("lastip1"), realfromhost);
		ipmask = extandipmask(ipmask, 
				getparm("lastip2"), realfromhost);
	}
	if ((usernum = login_get_user(id, &x)) <= 0) {
		if (ajax)
			ajax_fatal("�����ʹ�����ʺ�(%s)", id);
		else
			http_fatal("�����ʹ�����ʺ�(%s)", id);
	}
	strcpy(id, x->userid);
	if (strcasecmp(id, "guest")) {
		if (checkbansite(realfromhost)) {
			if (ajax)
				ajax_fatal("��¼ʧ�ܣ��Բ��𣬱�վ"
					"����ӭ���� [%s] ���û���¼��"
					"�������ʣ����� <a href=\"mailto:"
					ADMIN_EMAIL "\">" ADMIN_EMAIL "</a>"
					"��ϵ��", realfromhost);
			else
				http_fatal("�Բ��𣬱�վ����ӭ���� [%s] �ĵ�"
					"¼���������ʣ����� <a href=\"mailto:"
					"<a href=\"mailto:"
					ADMIN_EMAIL "\">" ADMIN_EMAIL "</a>"
					"��ϵ��", realfromhost);
		}
		if (userbansite(x->userid, realfromhost)) {
			if (ajax)
				ajax_fatal("��ID�����ý�ֹ�� [%s] ��¼��", 
						realfromhost);
			else
				http_fatal("��ID�����ý�ֹ�� [%s] ��¼��", 
						realfromhost);
		}
		if (!checkpasswd(x->passwd, x->salt, pw)) {
			logattempt(x->userid, realfromhost, "WWW", now_t);
			if (ajax) 
				ajax_fatal("�������������������ϵվ���飬"
					"�ṩע�������Ա��һ����롣");
			else
				http_fatal("�������������������ϵվ���飬"
					"�ṩע�������Ա��һ����롣");
		}
		if (!user_perm(x, PERM_BASIC)) {
			if (ajax)
				ajax_fatal("���ڱ��ʺ����Ʋ������ʺŹ���"
					"�취���Ѿ�������Ա��ֹ��¼��"
					"��ʹ�������ʺŵ�¼������ "
					DEFAULTBOARD " ��ѯ�ʡ�");
			else
				http_fatal("���ڱ��ʺ����Ʋ������ʺŹ���"
					"�취���Ѿ�������Ա��ֹ��¼��"
					"���������ʺŵ�¼���� "
					DEFAULTBOARD " ��ѯ�ʡ�");
		}
		if (file_has_word(MY_BBS_HOME "/etc/prisonor", x->userid)) {
			if (x->inprison == 0) {
				memcpy(&tmpu, x, sizeof (tmpu));
				tmpu.inprison = 1;
				tmpu.dieday = 2;
				updateuserec(&tmpu, 0);
			}
			if (ajax)
				ajax_fatal("���ĸ��죬��Ҫ���֡�");
			else
				http_fatal("���ĸ��죬��Ҫ����");
		}
		if (x->dieday) {
			if (ajax)
				ajax_fatal("���ˣ�����ʲô��^_^");
			else
				http_fatal("���ˣ�����ʲô��^_^");
		}
		t = x->lastlogin;
		memcpy(&tmpu, x, sizeof (tmpu));
		if (tmpu.salt == 0) {
			tmpu.salt = getsalt_md5();
			genpasswd(md5pass, tmpu.salt, pw);
			memcpy(tmpu.passwd, md5pass, MD5LEN);
			infochanged = 1;
		}
		if (count_uindex(usernum) == 0) {
			if (now_t - t > 1800)
				tmpu.numlogins++;
			infochanged = 1;
			tmpu.lastlogin = now_t;
			dtime = t - 4 * 3600;
			t = localtime(&dtime)->tm_mday;
			dtime = now_t - 4 * 3600;
			if (t < localtime(&dtime)->tm_mday
			    && x->numdays < 60000) {
				tmpu.numdays++;
			}
		}
		if ((abs(t - now_t) < 20) && (regjump!=1)  ) {
		//if (regjump!=1) {
			if (ajax)
				ajax_fatal("���ε�¼������ܣ�");
			else
				http_fatal("���ε�¼������ܣ�");
		}

		if (x->lasthost != from_addr.s_addr) {
			tmpu.lasthost = from_addr.s_addr;
			infochanged = 1;
		}
		if (infochanged)
			updateuserec(&tmpu, 0);
		currentuser = x;
	}
	ptr = getsenv("HTTP_X_FORWARDED_FOR");
	tracelog("%s enter %s www %d %s", x->userid, realfromhost, infochanged,
		 ptr);
	n = 0;
	if (loginok && isguest) {
		u_info->wwwinfo.iskicked = 1;
	}
	//����web style
	if (strcasecmp(id, "guest")) {
		sethomepath(filename, x->userid);
		mkdir(filename, 0755);

		strsncpy(buf, getparm("style"), 3);
		wwwstylenum = -1;
		if (isdigit(buf[0]))
			wwwstylenum = atoi(buf);
		if ((wwwstylenum > NWWWSTYLE || wwwstylenum < 0))
			if (!readuservalue
			    (x->userid, "wwwstyle", buf, sizeof (buf)))
				wwwstylenum = atoi(buf);
		if (wwwstylenum < 0 || wwwstylenum >= NWWWSTYLE)
			wwwstylenum = DEFAULT_WWWSTYLE;
		currstyle = &wwwstyle[wwwstylenum];
	} else {
		wwwstylenum = DEFAULT_WWWSTYLE;
		currstyle = &wwwstyle[wwwstylenum];

	}
	ub = wwwlogin(x, ipmask, ajax);
	if (!ajax)
		printf("<script>document.cookie='anonypid=%d; "
			"path=/;'</script>", thispid);
#ifdef USESESSIONCOOKIE
	if (!ajax)
		printf("<script>document.cookie='SESSION=%s; "
			"path=/;';</script>", urlencode(sessionCookie));
#endif
	if (!strcmp(url, "1")) {
#if 1
		printf
		    ("<script>\n"
		     "function URLencode(sStr) {\n"
		     "return escape(sStr).replace(/\\+/g, '%%2C').replace(/\\\"/g,'%%22').replace(/\\'/g, '%%27');\n"
		     "}\n"
		     "a=window.opener.location.href;\n" "l=a.length;\n"
		     "t=a.indexOf('/" SMAGIC "',1);\n" "t=a.indexOf('/',t+1);\n"
		     "nu=\"%s\"+\"?t=%ld&b=\"+URLencode(a.substring(t+1,l));\n"
		     "opener.top.location.href=nu;window.close();</script>",
		     ub, now_t);
#else
		printf
		    ("<script>opener.top.location.href='%s?t=%d';"
		     	"window.close();</script>",
		     ub, now_t);
#endif
	} else {
		char buf[256];
#ifdef USESESSIONCOOKIE
		if (ajax) {
			char *ptr;
			if ((ptr = strchr(ub + 1, '/')))
				*ptr = 0;
			sprintf(ub, "%s%s/", ub, sessionCookie);
		}
#endif
		if (strcmp(x->userid, "guest") && shouldbroadcast(usernum)) {
			sprintf(buf, "%s?t=%d&b=ooo", ub, (int) now_t);
#ifdef ENABLE_BLOG
		} else if(atoi(getparm("blog"))) {
			sprintf(buf, "%sblogpage", ub);
#endif
		} else
			sprintf(buf, "%s?t=%d", ub, (int) now_t);
		if (ajax) {
			printf("%s/%d", buf, thispid);
			http_quit();
		}
		redirect(buf);
	}
	http_quit();
	return 0;
}

char *
wwwlogin(struct userec *user, int ipmask, int ajax)
{
	FILE *fp1;
	int fd;
	int n, usernum;
	struct user_info u;
	char *urlbase, fname[80];
	char buf[20];

	usernum = getuser(user->userid, NULL);
	fd = open(MY_BBS_HOME "/" ULIST_BASE "." MY_BBS_DOMAIN, O_WRONLY);
	flock(fd, LOCK_EX);

	if ((urlbase = check_multi(user->userid, usernum))) {
		flock(fd, LOCK_UN);
		close(fd);
		return urlbase;
	}

	if (strcasecmp(user->userid, "guest") && count_uindex(usernum) >= 3) {
		flock(fd, LOCK_UN);
		close(fd);
		if (ajax)
			ajax_fatal("���Ѿ���¼�������ʺţ������ٵ�¼�ˡ�");
		else
			http_fatal("���Ѿ���¼�������ʺ�,�����ٵ�¼��");
	}
	bzero(&u, sizeof (struct user_info));
	u.active = 1;
	u.uid = usernum;
	u.pid = 1;
	u.mode = LOGIN;
	u.userlevel = user->userlevel;
	u.lasttime = now_t;
	u.curboard = 0;
	if (user_perm(user, PERM_LOGINCLOAK) && (user->flags[0] & CLOAK_FLAG))
		u.invisible = YEA;
	u.pager = 0;
	if (user->userdefine & DEF_FRIENDCALL)
		u.pager |= FRIEND_PAGER;
	if (user->flags[0] & PAGER_FLAG) {
		u.pager |= ALL_PAGER;
		u.pager |= FRIEND_PAGER;
	}
	if (user->userdefine & DEF_FRIENDMSG)
		u.pager |= FRIENDMSG_PAGER;
	if (user->userdefine & DEF_ALLMSG) {
		u.pager |= ALLMSG_PAGER;
		u.pager |= FRIENDMSG_PAGER;
	}
	strsncpy(u.from, fromhost, 24);
	u.fromIP = from_addr.s_addr;
	strsncpy(u.username, user->username, NAMELEN);
	strsncpy(u.userid, user->userid, IDLEN + 1);
	getrandomstr(u.sessionid);
	n = utmp_login(&u);
	if (n > MAXACTIVERUN || n <= 0) {
		flock(fd, LOCK_UN);
		close(fd);
		if (ajax)
			ajax_fatal("��Ǹ��Ŀǰ�����û��Ѿ��ﵽ����(%d)��"
				"�޷���¼������ʺ��ˡ����Ժ�������", 
				MAXACTIVERUN);
		else
			http_fatal("��Ǹ��Ŀǰ�����û����Ѵ�����(%d)��"
				"�޷���¼������ʺ��ˡ����Ժ�������",
				MAXACTIVERUN);
	}
	flock(fd, LOCK_UN);
	close(fd);
	n--;
	urlbase = makeurlbase(n, usernum);
	u_info = &(shm_utmp->uinfo[n]);
	w_info = &(u_info->wwwinfo);
	w_info->login_start_time = now_t;
	w_info->ipmask = ipmask;
	if (strcasecmp(user->userid, "guest")) {
		u_info->unreadmsg = get_unreadcount(user->userid);
		initfriends(u_info);
		sethomefile(fname, user->userid, "clubrights");
		if ((fp1 = fopen(fname, "r")) == NULL) {
			memset(u_info->clubrights, 0, CLUB_SIZE * sizeof (int));
		} else {
			fread(&(u_info->clubrights), sizeof (int), CLUB_SIZE,
			      fp1);
			fclose(fp1);
		}
		if (readuservalue(user->userid, "signature", buf, sizeof (buf))
		    >= 0)
			u_info->signature = atoi(buf);
		w_info->edit_mode = 0;
		set_my_cookie();
	} else {
/* �����ƺ��Ƕ���δ��¼��id����������� */
		u_info->unreadmsg = 0;
		memset(u_info->friend, 0, sizeof (u.friend));
		memset(u_info->clubrights, 0, CLUB_SIZE * sizeof (int));
		w_info->t_lines = 20;
// def_mode�������Ķ����1 Ϊ����ģʽ�� 0Ϊһ��ģʽ
		w_info->def_mode = 0;
		w_info->att_mode = 0;
		w_info->doc_mode = 1;
	}

	if ((user->userlevel & PERM_BOARDS))
		setbmstatus(user, 1);
	return urlbase;
}

static int
do_check(int uent, int uid)
{
	return (shm_utmp->uinfo[uent].uid == uid
		&& shm_utmp->uinfo[uent].active == 1
		&& shm_utmp->uinfo[uent].pid == 1
		&& now_t - shm_utmp->uinfo[uent].lasttime < 18 * 60
		&& from_addr.s_addr == shm_utmp->uinfo[uent].fromIP);
		//&& !strncmp(fromhost, shm_utmp->uinfo[uent].from, 24));
}

static char *
check_multi(char *id, int uid)
{
	int i, uent;
	int h;
	if (uid <= 0 || uid > MAXUSERS)
		return NULL;
	if (strcasecmp(id, "guest")) {
		//�����㷨, wwwlogin�������Ƶ�¼������Ŀ, ����
		//���������ᱻ���׳屬
		for (i = 0; i < 6; i++) {
			uent = uindexshm->user[uid - 1][i] - 1;
			if (uent < 0)
				continue;
			if (do_check(uent, uid))
				return makeurlbase(uent, uid);
		}
		return NULL;
	} else {
		h = utmp_iphash(realfromhost);
		uent = shm_utmp->guesthash_head[h];
		while (uent != 0) {
			if (do_check(uent - 1, uid))
				return makeurlbase(uent - 1, uid);
			uent = shm_utmp->guesthash_next[uent];
		}
	}
	return NULL;
}

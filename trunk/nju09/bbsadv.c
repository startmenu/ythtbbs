#include "bbslib.h"

#define ADV_MAX_LEN	51200
#define ADV_MAX_TOP	1
#define ADV_MAX_LEFT	1
#define ADV_MAX_TEXT_TOP	0
#define ADV_MAX_TEXT_LEFT	3
#define ADV_PATH	HTMPATH "/adv"
#define ADV_DATA	MY_BBS_HOME "/wwwtmp/adv_data"

char *advcolor[] = {
	"black",
	"red",
	"blue",
	"green"};

struct bbsadv {
	char filename[40];
	char description[40];
	char link[128];
	time_t uploadtime;
	char userid[IDLEN + 1];
	int pos;	//0 left image;1 top image;2 left text; 3 top text
	int color;
	int width;
	int height;
	char unused[19];
};


int bbsadv_newadv(void) {
	char path[STRLEN], filename[100], *ptr;
	struct bbsadv adv, *p = NULL;
	int fd, pos, total, left = 0, top = 0, lefttext = 0, toptext = 0, i;
	int size = sizeof(struct bbsadv), text = 0, color;
	FILE *fp;

	struct parm_file *parmFile;
	
	parmFile = getparmfile("file");
	pos = atoi(getparm("pos"));
	switch (pos) {
		case 0:
		case 1:
			text = 0;
			break;
		case 2:
		case 3:
			text = 1;
			break;
		default:
			http_fatal("δ֪�Ĺ��λ��(%d)", text);
	}
	color = atoi(getparm("color"));
	color = (color < 0 || color > 3) ? 0 : color;
	if (text == 0) {
		if (!parmFile)
			http_fatal("�ƺ���û���ϴ��ļ�������");
		strsncpy(filename, parmFile->filename, sizeof(filename));
	
		if (strlen(filename) >= 35) {
			ptr = strrchr(filename, '.');
			if (ptr) {
				int len = strlen(ptr);
				if (len > 6) {
					ptr[6] = 0;
					len = 6;
				}
				memmove(filename + 35 - len, ptr, len + 1);
			} else 
				filename[35] = 0;
		}

		if (fixfilename(filename))
			http_fatal("%s����Ч���ļ�����", filename);
	
		if (parmFile-> len > ADV_MAX_LEN)
			http_fatal("%s����(����Ϊ %d �ֽ�)��", 
					filename, ADV_MAX_LEN);
	}

	if ((fd = open(ADV_DATA, O_RDWR | O_CREAT, 0600)) < 0)
		http_fatal("�޷��������ļ�03��");
	total = file_size(ADV_DATA) / size;
	p = (struct bbsadv *) malloc(size);
	if (p == NULL)
		http_fatal("�ڴ����ʧ�ܡ�");
	for (i = 0; i < total; i++) {
		if (lseek(fd, i * size, SEEK_SET) < 0)
			http_fatal("�޷��������ļ�05");
		if (read(fd, p, size) != size)
			http_fatal("�޷��������ļ�06");
		switch (p->pos) {
			case 0:
				left++;
				break;
			case 1:
				top++;
				break;
			case 2:
				lefttext++;
				break;
			case 3:
				toptext++;
				break;
		}
	}
	free(p);
	if (lseek(fd, file_size(ADV_DATA), SEEK_SET) < 0)
		http_fatal("�޷��������ļ�04��");
	if (left > ADV_MAX_LEFT || top > ADV_MAX_TOP)
		http_fatal("Ŀǰ���ֻ���ϴ� %d �����ͼƬ���� %d "
				"������ͼƬ��档", ADV_MAX_LEFT, ADV_MAX_TOP);
	if (lefttext > ADV_MAX_TEXT_LEFT || 
			toptext > ADV_MAX_TEXT_TOP)
		http_fatal("Ŀǰ���ֻ���ϴ� %d ��������ֹ��� %d "
				"���������ֹ�档",
				ADV_MAX_TEXT_LEFT, ADV_MAX_TEXT_TOP);
	bzero(&adv, size);
	strncpy(adv.filename, filename, sizeof(adv.filename));
	strncpy(adv.description, getparm("des"), sizeof(adv.description));
	strncpy(adv.link, getparm("link"), sizeof(adv.link));
	strncpy(adv.userid, currentuser->userid, sizeof(adv.userid));
	adv.uploadtime = time(NULL);
	adv.pos = pos;
	adv.color = color;
	adv.width = atoi(getparm("width"));
	adv.height = atoi(getparm("height"));
	flock(fd, LOCK_EX);
	write(fd, &adv, sizeof(adv));
	flock(fd, LOCK_UN);
	close(fd);

	if (text == 0) {
		sprintf(path, ADV_PATH "/%s", filename);
		if (file_exist(path))
			http_fatal("����ɾ��ͬ���ļ�");
		mkdir(ADV_PATH, 0700);
		fp = fopen(path, "w");
		fwrite(parmFile->content, 1, parmFile->len, fp);
		fclose(fp);
		tracelog("%s upload %s as an adv", 
				currentuser->userid, filename);
	} else if (text == 1) {
		tracelog("%s upload an text adv", 
				currentuser->userid);
	}


	strncpy(path, "bbsadv", sizeof(path));
	redirect(path);
	http_quit();
	return 1;
}

int bbsadv_del() {
	int num, fd, fd2, total, i;
	int size = sizeof(struct bbsadv);
	char path[STRLEN], filepath[STRLEN];
	struct bbsadv adv;

	strncpy(path, ADV_DATA ".tmp", sizeof(path));
	num = atoi(getparm("num"));

	if ((fd = open(ADV_DATA, O_RDWR | O_CREAT, 0600)) < 0)
		http_fatal("�޷��������ļ�07��");
	if ((fd2 = open(path, O_RDWR | O_CREAT, 0600)) < 0) {
		close(fd);
		http_fatal("�޷��������ļ�08��");
	}

	total = file_size(ADV_DATA) / size;

	flock(fd2, LOCK_EX);
	for (i = 0; i < total; i++) {
		if (lseek(fd, i * size, SEEK_SET) < 0 || 
				read(fd, &adv, size) != size) {
			close(fd);
			flock(fd2, LOCK_UN);
			close(fd2);
			http_fatal("�޷��������ļ�09��");
		}
		if (i == num) {
			sprintf(filepath, ADV_PATH "/%s", adv.filename);
			unlink(filepath);
			continue;
		}
		write(fd2, &adv, size);
	}
	flock(fd2, LOCK_UN);
	close(fd2);
	close(fd);
	unlink(ADV_DATA);
	rename(path, ADV_DATA);

	tracelog("%s del an adv (%s)", currentuser->userid, filepath);

	sprintf(path, "bbsadv");
	redirect(path);

	http_quit();
	return 1;
}

int bbsadv_main(void) {
	char buf[STRLEN], action[10];
	int fd, total, i;
	int size = sizeof(struct bbsadv);
	struct mmapfile mf = { ptr: NULL };
	struct bbsadv *adv;

	strncpy(buf, "wwwtmp/adv_manager", sizeof(buf));

	strncpy(action, getparm("action"), sizeof(action));


	html_header(1);
	if (!loginok || isguest || !(user_perm(currentuser, PERM_SYSOP) ||
			file_has_word(buf, currentuser->userid))) {
		strncpy(buf, "boa?secstr=&redirect=1", sizeof(buf));
		redirect(buf);
		http_quit();
		return 0;
	}

	printf("<style>\n"
		"td { text-align: center; line-height: 21px;}\n"
		".tr_0 {background-color: #ececec; }\n"
		".tr_1 {background-color: #efefef; }\n"
		"</style>");
	printf("</head>\n<body><script>checkFrame(1);</script>\n");

	if ((fd = open(ADV_DATA, O_RDWR | O_CREAT, 0600)) < 0)
		http_fatal("�޷������ݿ�01");
	close(fd);

	if (!strcmp(action, "new"))
		bbsadv_newadv();
	else if(!strcmp(action, "del"))
		bbsadv_del();

	mmapfile(NULL, &mf);
	MMAP_TRY {
		if (mmapfile(ADV_DATA, &mf) < 0) {
			MMAP_UNTRY;
			http_fatal("�޷������ݿ�02");
		}

		total = mf.size / size;

		printf("<h1>" MY_BBS_NAME "���������</h1><br><br>\n");
		printf("<h2>ͼƬ������</h2>");
		printf("<div align=center><table width=800px>\n");
		printf("<tr class=tr_1>\n<td width=40px>���</td>\n"
			"<td width=180px>ͼƬ������</td>\n"
			"<td width=225px>�ϴ�</td>\n"
			"<td width=225px>˵��������</td>\n"
			"<td width=90px>���/�߶�</td>\n"
			"<td width=40px>����</td></tr>\n");
		for (i = 0; i < total; i++) {
			adv = (struct bbsadv *) (mf.ptr + i * size);
			if (adv->pos != 0 && adv->pos != 1)
				continue;
			printf("<form method=post action=bbsadv?action=del>"
				"<input name=num value=%d type=hidden>"
					"<tr class=tr_%d>\n", i, i % 2);
			printf("<td>%d</td>", i);
			printf("<td><img border=0 src='/adv/%s' onload='"
				"this.width=this.width>200?200:this.width;'>"
				"<br>%s [%s]</td>", 
				adv->filename, adv->filename, 
				adv->pos ? "����" : "���");
			printf("<td>%s<br>", Ctime(adv->uploadtime));
			printf("%s</td>", adv->userid);
			printf("<td>%s<br>%s</td>", 
				adv->description, adv->link);
			printf("<td>%d px<br>%d px</td>", 
					adv->width, adv->height);
			printf("<td><input type=submit value=ɾ�� "
				"onclick = 'return confirm(\"ȷ��ɾ����\")'>"
					"</td></tr></form>\n");
		}
		printf("<form id=adv_newadv method=post "
			"action=bbsadv?action=new "
			"enctype='multipart/form-data'><tr class=tr_%d>\n"
			"<td colspan=3>�ļ���<br />"
			"<input type=file name=file style='width: 90%%;'>\n"
			"</td><td>"
			"˵����<input type=text maxlength=20 name=des "
			"style='width: 120px;'><br />"
			"���ӣ�<input type=text maxlength=128 name=link "
			"style='width: 120px;' value='http://'><br />"
			"<input type=radio name=pos value=0 checked>"
			"��� "
			"<input type=radio name=pos value=1>����</td>\n"
			"<td>��ȣ�<input type=text maxlength=3 name=width "
			"style='width: 30px;'><br />"
			"�߶ȣ�<input type=text maxlength=3 name=height "
			"style='width: 30px;'></td>"
			"<td><input type=submit value=����></td>"
			"</tr></form>\n", i % 2);
		printf("<tr class=tr_%d><td colspan=5>"
			"Ŀǰ֧�� %d �����ͼƬ���� %d ������ͼƬ��棬����"
			"ͼƬ��С������ %d KB��</td></tr>", (i + 1) % 2,
			ADV_MAX_LEFT, ADV_MAX_TOP, ADV_MAX_LEN / 1024);
		printf("</table><br>");
		printf("<h2>���ֹ�����</h2>"
			"<table width=800px>\n<tr class=tr_1>\n"
			"<td width=40px>���</td>"
			"<td width=220px>�ϴ�</td>"
			"<td width=180px>����</td>"
			"<td width=320px>����</td>"
			"<td width=40px>����</td></tr>");
		for (i = 0; i < total; i++) {
			adv = (struct bbsadv *)(mf.ptr + i * size);
			if (adv->pos != 2 && adv->pos != 3)
				continue;
			printf("<form method=post action=bbsadv?action=del>"
				"<input name=num value=%d type=hidden>", i);
			printf("<tr class=tr_%d>", i % 2);
			printf("<td>%d</td>", i);
			printf("<td>%s<br>", Ctime(adv->uploadtime));
			printf("%s</td>", adv->userid);
			printf("<td><font color=%s>%s</font> [%s]</td>",
				advcolor[adv->color], adv->description,
				adv->pos % 2 ? "����" : "���");
			printf("<td><a href='%s'>%s</a></td>", 
				adv->link, adv->link);
			printf("<td><input type=submit value=ɾ�� "
				"onclick = 'return confirm(\"ȷ��ɾ����\")'>"
					"</td></tr></form>\n");
		}
		printf("<form id=adv_newadv method=post "
			"action=bbsadv?action=new>"
			"<tr class=tr_%d>\n"
			"<td colspan=4><p>"
			"˵����<input type=text maxlength=20 name=des "
			"style='width: 240px;'>��"
			"���ӣ�<input type=text maxlength=128 name=link "
			"style='width: 240px;'></p><p>"
			"λ�ã�<input type=radio name=pos value=2 checked>"
			"��� "
			"<input type=radio name=pos value=3 disabled>"
			"���ˡ���"
			"��ɫ��<input type=radio name=color value=0 checked>��"
			"��<input type=radio name=color value=1>��"
			"��<input type=radio name=color value=2>��"
			"��<input type=radio name=color value=3>��"
			"</p></td>\n"
			"<td><input type=submit value=����></td>"
			"</tr></form>\n", i % 2);
		printf("<tr class=tr_%d><td colspan=5>"
			"Ŀǰ֧�� %d ��������ֹ��� %d ���������ֹ�档"
			"</td></tr>", (i + 1) % 2,
			ADV_MAX_TEXT_LEFT, ADV_MAX_TEXT_TOP);
		printf("</table></div><br><br>");
	} MMAP_CATCH {
	} MMAP_END mmapfile(NULL, &mf);

	return 1;
}

int bbsadv_show(int pos) {
	int fd, i, total, retv = 0;
	int size = sizeof(struct bbsadv);
	struct bbsadv adv;

	if ((fd = open(ADV_DATA, O_RDWR | O_CREAT, 0600)) < 0) 
		return 0;
	total = file_size(ADV_DATA) / size;
	if (pos == 2)
		printf("<hr>");
	printf("<div id='boa_adv_%d'>", pos);
	for (i = 0; i < total; i++) {
		if (lseek(fd, i * size, SEEK_SET) < 0 || 
				read(fd, &adv, size) != size)
			continue;
		if (adv.pos != pos)
			continue;
		if (pos == 0 || pos == 1) {
			printf("<a href='%s' name='%s' target=_blank>",
				adv.link, adv.description);
			if(strcasestr(adv.filename, ".swf")) {
				printf("<object classid='clsid:D27CDB6E-AE6D-11cf-96B8-444553540000' width='%d' height='%d'"
				" type='application/x-shockwave-flash'>\n"
				"<param name='movie' value='/adv/%s'>\n"
				"<param name='quality' value='high'>\n"
//				"<embed src='http://proxy.yjrg.net/adv/%s' quality='high' "
				"<embed src='http://yilubbs.com/adv/%s' quality='high' "
				"autostart=true loop=true "
				"width='%d' height='%d'></embed></object>",
				adv.width ? adv.width : (adv.pos ? 600 : 120),
				adv.height ? adv.height : 45,
				adv.filename, adv.filename,
				adv.width ? adv.width : 600,
				adv.height ? adv.height : 45);
			} else {
//				printf("<img src='http://proxy.yjrg.net/adv/%s' name='%s' border=0 "
				printf("<img src='http://yilubbs.com/adv/%s' name='%s' border=0 "
				"width='%d' height='%d'>\n", 
				adv.filename, adv.description, 
				adv.width ? adv.width : (adv.pos ? 600 : 120), 
				adv.height ? adv.height : 40);
			}
			printf("</a><br>");
		} else if (pos == 2 || pos == 3) {
			printf("<a href='%s' name='%s' target=_blank>"
				"<font color='%s'>%s</font></a><br/>\n",
				adv.link, adv.description, 
				advcolor[adv.color], adv.description);
		}
		retv++;
	}
	if (!pos && (user_perm(currentuser, PERM_SYSOP) || 
			file_has_word(MY_BBS_HOME "/wwwtmp/adv_manager", 
				currentuser->userid)))
		printf("<br><a href='bbsadv' target=f3>������</a>");
	printf("</div>");
	close(fd);
	return retv;
}

int bbsadv_cheat(pos) {
	int fd, total, retv;
	int size = sizeof(struct bbsadv);
	struct bbsadv adv;

	if ((fd = open(ADV_DATA, O_RDWR | O_CREAT, 0600)) < 0) 
		return 0;
	total = file_size(ADV_DATA) / size;

	if (pos < 0 || pos >= total)
		retv = random() % (10 * total);
	else
		retv = pos;

	if (retv < 0 || retv >= total)
		return -1;

	printf("<div id=\"adv_cheat\" style=\"display: none;\">");
	if (lseek(fd, retv * size, SEEK_SET) < 0 || 
				read(fd, &adv, size) != size)
		return -1;
	close(fd);
	printf("<iframe src=\"%s\"></iframe></div>", adv.link);
	printf("<script>\nsetTimeout(\"document.getElementById("
		"'adv_cheat').innerHTML=''\", 1500);</script>");
	return retv;
}

#include "bbslib.h"

static int tmpl_add_num_visitors(struct boardmem *brd);

int
testmath(char *ptr)
{
	int i = 0;
	if (!*ptr)
		return 0;
	if (*ptr == '$')
		i = 1;
	ptr++;
	while ((ptr = strchr(ptr, '$')) != NULL) {
		if (*(ptr - 1) != '\\')
			i++;
		ptr++;
	}
	return (i + 1) % 2;
}

int
bbssnd_main()
{
	char filename[80], filename2[80], dir[80];
	char board[80], title[80], buf[256], reffile[80], *content, *ref;
	int r, i, sig, mark = 0, outgoing, anony, guestre = 0, usemath, use_ubb, nore;
	int is1984, to1984 = 0;
	struct boardmem *brd;
	struct fileheader *x = NULL;
	int thread = -1;
	struct mmapfile mf = { ptr:NULL };
	struct userec tmp;
	int tmpl, ajax;
	FILE *fp;
	html_header(1);

	ajax = atoi(getparm("ajax"));
	getparmboard(board, sizeof(board));
	strsncpy(title, getparm("title"), 60);
	outgoing = strlen(getparm("outgoing"));
	anony = strlen(getparm("anony"));
	nore = strlen(getparm("nore"));
	usemath = strlen(getparm("usemath"));
	use_ubb = strlen(getparm("useubb"));
	tmpl = strlen(getparm("tmpl"));
	brd = getboard(board);
	if (brd == 0)
		http_fatal("���������������");
	if (brd->ban == 2)
		http_fatal("��Ϊ�������³���, ������ʱ���.");
	strcpy(board, brd->header.filename);
	sprintf(dir, "boards/%s/.DIR", board);
	ref = getparm("ref");
	r = atoi(getparm("rid"));
	thread = atoi(getparm("th"));
	if (ref[0]) {
		MMAP_TRY {
			if (mmapfile(dir, &mf) == -1) {
				MMAP_UNTRY;
				http_fatal("�����������");
			}
			x = findbarticle(&mf, ref, &r, 1);
		}
		MMAP_CATCH {
		}
		MMAP_END mmapfile(NULL, &mf);
		if (x && (x->accessed & FH_ALLREPLY)) {
			if (strncmp(x->title, "Re: ", 4))
				snprintf(title, 60, "Re: %s", x->title);
			else
				snprintf(title, 60, "%s", x->title);
			guestre = 1;
			mark = mark | FH_ALLREPLY;
		}
		if (x)
			thread = x->thread;
	} else {
		thread = -1;
	}
	if (!loginok || (isguest && !guestre))
		http_fatal("�Ҵҹ��Ͳ��ܷ������£����ȵ�¼");
	changemode(POSTING);

	if (!(brd->header.flag & ANONY_FLAG))
		anony = 0;
	if (brd->header.flag & IS1984_FLAG)
		is1984 = 1;
	else
		is1984 = 0;
	for (i = 0; i < strlen(title); i++)
		if (title[i] <= 27 && title[i] >= -1)
			title[i] = ' ';
	i = strlen(title) - 1;
	while (i >= 0 && isspace(title[i])) {
		title[i] = 0;
		i--;
	}
	if (title[0] == 0)
		http_fatal("���±���Ҫ�б���");
	sig = atoi(getparm("signature"));
	content = getparm("text");
	if (usemath && testmath(content))
		mark |= FH_MATH;
	if (!has_post_perm(currentuser, brd) && !guestre)
		http_fatal("����������Ψ����, ����������Ȩ���ڴ˷�������.");
	if (noadm4political(board))
		http_fatal("�Բ���,��Ϊû�а��������Ա����,������ʱ���.");

	if ((now_t - w_info->lastposttime) < 6) {
		w_info->lastposttime = now_t;
		http_fatal("���η��ļ������, ����Ϣ���������");
	}
	w_info->lastposttime = now_t;
	sprintf(filename, "bbstmpfs/tmp/%d.tmp", thispid);
	sprintf(filename2, "bbstmpfs/tmp/%d.tmp2", thispid);
	if (tmpl)
		snprintf(filename2, sizeof(filename2),
			"bbstmpfs/tmp/tmplpst.%s.www", currentuser->userid);
	else if (use_ubb)
		ubb2ansi(content, filename2);
	else {
		f_write(filename2, content);
		if (ajax) {
			if (NULL != (fp = fopen(filename2, "a"))) {
				sprintf(reffile, "boards/%s/%s", board, ref);
				printquote(fp, reffile, board, "", 0);
				fclose(fp);
			}
		}
	}
	if (!hideboard_x(brd)) {
		int dangerous =
		    dofilter(title, filename2, political_board(board));
		if (dangerous == 1)
			to1984 = 1;
		else if (dangerous == 2) {
		#ifdef POST_WARNING
			char mtitle[256];
			sprintf(mtitle, "[������] %s %.60s", board, title);
			mail_file(filename, "delete", mtitle, currentuser->userid);
			updatelastpost("deleterequest");
		#endif
			mark |= FH_DANGEROUS;
		}
	}

	if (insertattachments_byfile(filename, filename2, currentuser->userid))
		mark = mark | FH_ATTACHED;

	if (nore)
		mark = mark | FH_NOREPLY;

	unlink(filename2);

	if (is1984 || to1984) {
		r = post_article_1984(board, title, filename,
				      currentuser->userid,
				      currentuser->username, fromhost, sig - 1,
				      mark, outgoing, currentuser->userid,
				      thread);
	} else if (anony) {
		sprintf(buf, "����������ʹ - %d", atoi(getparm("anonypid")));
		r = post_article(board, title, filename, "Anonymous",
				 buf, "������ʹ�ļ�", 0, mark,
				 outgoing, currentuser->userid, thread);
	} else {
		r = post_article(board, title, filename, currentuser->userid,
				 currentuser->username, fromhost, sig - 1, mark,
				 outgoing, currentuser->userid, thread);
		if(r > 0 && x) {//re�ļ�������Ȧ			
			sprintf(buf, "1 %s %s", currentuser->userid, x->owner);
			friendslog(buf);
		}
	}
	if (r <= 0)
		http_fatal("�ڲ������޷�����");
	if (!is1984 && !to1984) {
		brc_initial(currentuser->userid, board);
		brc_add_readt(r);
		brc_update(currentuser->userid);
	}
	unlink(filename);
	tracelog("%s post %s %s", currentuser->userid, board, title);
	if (!(brd->header.flag & CLUB_FLAG) && !junkboard(board)) {
		char aibuf[512];
		snprintf(aibuf, sizeof(aibuf), "1\tboards/%s/M.%d.A\t%s\t%s", 
				board, r, currentuser->userid, title);
		ailog(aibuf);
		memcpy(&tmp, currentuser, sizeof (tmp));
		tmp.numposts++;
		updateuserec(&tmp, 0);
	}
	if (sig != u_info->signature) {
		u_info->signature = sig;
		snprintf(buf, 20, "%d", sig);
		saveuservalue(currentuser->userid, "signature", buf);
	}
	if (to1984) {
		printf("%s<br>\n", BAD_WORD_NOTICE);
		printf("[<a href='javascript:history.go(-2)'>����</a>]");
	} else {
//		sprintf(buf, "doc?B=%d&redirect=1", getbnumx(brd));
//����redirect=1���������ҳ��������ʱֻ����ȥ����˵����֪��ԭ��
		sprintf(buf, "doc?B=%d", getbnumx(brd));
		redirect(buf);
	}
	if(tmpl) {
		tmpl_add_num_visitors(brd);
	}
	return 0;
}

static int
tmpl_add_num_visitors(struct boardmem *brd)
{
	int tmpl_filetime, pos;
	struct posttmplheader tmphdr;
	char dir[PATH_MAX];
	if((tmpl_filetime = atoi(getparm("tmplfiletime"))) <= 0) {
		return 0;
	}
	setbfile(dir, brd->header.filename, ".TMPL");
	pos = new_search_record(dir, &tmphdr, sizeof(struct posttmplheader),
			(void *)tmpl_cmp_psttmpls, &tmpl_filetime);
	if(pos <= 0) {
		return 0;
	}
	tmphdr.num_visitors ++;
	put_record(&tmphdr, sizeof(struct posttmplheader), pos - 1, dir);
	return 0;
}

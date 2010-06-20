#include "bbs.h"
#include <sys/types.h>
#include <sys/mman.h>
#include <ght_hash_table.h>
#include "www.h"
#define TOPN 2
#define TOPFN "TOPN"
#define FILTERTITLE "etc/.filtertitle_new"

struct data_s {
	ght_hash_table_t *user_hash;
	struct boardtop bt;
	int water;
};

int now_t;

struct boardtop *topten = NULL;
struct boardtop *ctopten = NULL;
struct boardtop **csectopten = NULL;
int allflag = 0;
struct mmapfile filtermf = { ptr: NULL, size:0 };
struct bbsinfo bbsinfo;

int
getbnum(char *board)
{
	int i;
	struct boardmem *bcache;
	bcache = bbsinfo.bcacheshm->bcache;
	for (i = 0; i < MAXBOARD && i < bbsinfo.bcacheshm->number; i++) {
		if (!strcasecmp(board, bcache[i].header.filename))
			return i;
	}
	return -1;
}

int
cmpbt(struct boardtop *a, struct boardtop *b)
{
	return b->unum - a->unum;
}

int
filtertitle(char *tofilter)
{
	if (filtermf.ptr == NULL)
		return 0;
	return filter_string(tofilter, &filtermf);
}

int
trytoinsert(struct boardheader *bh, struct boardtop *bt)
{
	int index;
	
	memcpy(topten + 10, bt, sizeof (struct boardtop));
	qsort(topten, 11, sizeof (struct boardtop), (void *) cmpbt);
	if (!filtertitle(bt->title) && !filtertitle(bh->filename)) {
		memcpy(ctopten + 10, bt, sizeof (struct boardtop));
		qsort(ctopten, 11, sizeof (struct boardtop), (void *) cmpbt);
	}

	index = bh->secnumber1;
	if(csectopten[index] == NULL) {
		csectopten[index] = calloc(11, sizeof(struct boardtop));
		if(csectopten[index] == NULL)
			return -1;
		bzero(csectopten[index], sizeof(struct boardtop) * 11);
	}
	memcpy(csectopten[index] + 10, bt, sizeof (struct boardtop));
	qsort(csectopten[index], 11, sizeof (struct boardtop), (void *) cmpbt);
		
	return 0;
}

int water_article(char *bname, struct fileheader *fh) {
	int fd, retv = 0;
	struct stat st;
	char path[256];

	sprintf(path, "boards/%s/%s", bname, fh2fname(fh));
	fd = open(path, O_RDONLY);
	if(fd < 0)
		return 1;
	fstat(fd, &st);
	if(st.st_size < (200 + 93)) //��ͷ�ؽŴ�Լ200��
		retv =  1;
	close(fd);
	return retv;
}
	
int
_topn(struct boardheader *bh)
{
	//�� bh �����ҵ�TOPN��thread
	int size = sizeof (struct fileheader), total;
	int i, j, k, *usernum, water;
	struct mmapfile mf;
	struct fileheader *ptr;
	ght_hash_table_t *p_table = NULL;
	ght_iterator_t iterator, iterator1;
	struct data_s *data;
	char owner[14];
	int start, tocount, first;
	struct boardtop *bt, *bt1;
	char topnfilename[80], tmpfn[80];
	FILE *fp;
	char DOTDIR[80];
	char path[80];
	void *key, *key1;
	char *real_title;

	sprintf(DOTDIR, "boards/%s/.DIR", bh->filename);
	sprintf(path, "boards/%s", bh->filename);
	sprintf(topnfilename, "%s/%s", path, TOPFN);
	unlink(topnfilename);
	if ((bh->flag & CLUB_FLAG) || bh->level != 0)	//�������
		return 0;
	if (seek_in_file("etc/junkboards", bh->filename) ||
	    seek_in_file("etc/notop10boards", bh->filename))
		return 0;
	MMAP_TRY {
		if (mmapfile(DOTDIR, &mf) == -1) {
			errlog("open .DIR error, %s\n", DOTDIR);
			MMAP_RETURN(0);
		}
		if (mf.size == 0) {	//�հ���
			mmapfile(NULL, &mf);
			MMAP_RETURN(0);
		}
		total = mf.size / size - 1;	//�� 0 - total ��Ŀ
		start =
		    Search_Bin((struct fileheader *) mf.ptr, now_t - 86400, 0,
			       total);
		if (start < 0) {
			start = -(start + 1);
		}
		if (start > total) {	//û��������
			mmapfile(NULL, &mf);
			MMAP_RETURN(0);
		}
		tocount = total - start + 1;
		bt = malloc(sizeof (struct boardtop) * tocount);
		if (bt == NULL) {
			errlog("malloc failed");
			exit(-1);
		}
		p_table = ght_create(tocount + 3);
		for (i = start; i <= total; i++) {
			ptr =
			    (struct fileheader *) (mf.ptr +
						   i *
						   sizeof (struct fileheader));
			if (strchr(ptr->owner, '.'))
				continue;
			if(ptr->thread < now_t - 3600 * 30) //filter tooooold thread
				continue;
			if (ptr->thread == ptr->filetime) //filter water thread
				water = water_article(bh->filename, ptr);
			else
				water = 0;
			if ((data =
			     ght_get(p_table, sizeof (int),
				     &(ptr->thread))) == NULL) {
				if ((data = malloc(sizeof (struct data_s))) ==
				    NULL) {
					errlog("malloc failed");
					exit(-1);
				}
				data->water = water;
				if ((usernum = malloc(sizeof (int))) == NULL) {
					errlog("malloc failed");
					exit(-1);
				}
				first =
				    Search_Bin((struct fileheader *) mf.ptr,
					       ptr->thread, 0, total);
				if (first < 0) {
					real_title = ptr->title;
					first = -first;
				}
				else
					real_title =
					    ((struct fileheader *) mf.ptr +
					     first)->title;
				if (!strncmp(real_title, "Re: ", 4))
					real_title += 4;
				strncpy(data->bt.title, real_title, 60);
				data->user_hash = NULL;
				data->user_hash = ght_create(tocount);
				data->bt.unum = 1;
				(data->bt.lasttime) = ptr->filetime;
				data->bt.thread = ptr->thread;
				strncpy(data->bt.board, bh->filename, 24);
				strncpy(owner, fh2realauthor(ptr), 14);
				strncpy(data->bt.firstowner,
					fh2owner((struct fileheader *) mf.ptr +
						 first), 14);
				*usernum = 0;
				ght_insert(data->user_hash, usernum,
					   sizeof (char) * strlen(owner),
					   owner);
				ght_insert(p_table, data, sizeof (int),
					   &(ptr->thread));
			} else {
				strncpy(owner, fh2realauthor(ptr), 14);
				if ((usernum =
				     ght_get(data->user_hash,
					     sizeof (char) * strlen(owner),
					     owner)) == NULL) {
					(data->bt.unum)++;
					(data->bt.lasttime) = ptr->filetime;
					if ((usernum = malloc(sizeof (int))) ==
					    NULL) {
						errlog("malloc failed");
						exit(-1);
					}
					ght_insert(data->user_hash, usernum,
						   sizeof (char) *
						   strlen(owner), owner);
				} else {
					(*usernum)++;
				}
			}
		}
	}
	MMAP_CATCH {
		mmapfile(NULL, &mf);
		MMAP_RETURN(0);
	}
	MMAP_END mmapfile(NULL, &mf);
	i = 0;
	bt1 = bt;
	for (data = ght_first(p_table, &iterator, &key); data;
	     data = ght_next(p_table, &iterator, &key)) {
		if(!data->water) {
			memcpy(bt1, &(data->bt), sizeof (struct boardtop));
			bt1++;
			i++;
		}
		for (usernum = ght_first(data->user_hash, &iterator1, &key1);usernum;
		     usernum = ght_next(data->user_hash, &iterator1, &key1)) {
			free(usernum);
		}
		ght_finalize(data->user_hash);
		data->user_hash = NULL;
		free(data);
	}
	ght_finalize(p_table);
	p_table = NULL;
	qsort(bt, i, sizeof (struct boardtop), (void *) cmpbt);
	sprintf(tmpfn, "%s/topntmp", path);
	fp = fopen(tmpfn, "w");
	if (!fp) {
		errlog("touch %s error", tmpfn);
		exit(1);
	}
	for (j = 0, k = 0, bt1 = bt; j < TOPN && j < i; j++, bt1++) {
		if (allflag)
			trytoinsert(bh, bt1);
		if (bt1->unum < 8)
			continue;
		if (k == 0) {
			fprintf(fp, "<table width=80%%>");
		}
		k++;
		fprintf(fp,
			"<tr><td><font color=red>HOT</font></td><td><a href=bbsnt?B=%d&th=%d>%s",
			getbnum(bh->filename), bt1->thread,
			nohtml(void1(bt1->title)));
		fprintf(fp, "</a></td><td>[��������:%d] </td></tr>\n ",
			bt1->unum);
	}
	if (k)
		fprintf(fp, "</table>");
	fclose(fp);
	if (k)
		rename(tmpfn, topnfilename);
	free(bt);
	return 0;
}

int
top_all_dir()
{
	return new_apply_record(".BOARDS", sizeof (struct boardheader),
				(void *) _topn, NULL);
}

int
html_topten(int filter_mode, int sec)
{
	struct boardtop *bt;
	int j, idx;
	FILE *fp;
	char tmppath[256], path[256], *ptr;
	
	if(sec == 0) {	// ȫվtop10
		bt = filter_mode ? ctopten : topten;
		sprintf(tmppath, "wwwtmp/%stopten.tmp", filter_mode ? "c" : "");
	} else {	// ����top10
		bt = csectopten[sec];
		if(bt == NULL)
			return -1;
		sprintf(tmppath, "wwwtmp/%c_topten.tmp", sec);
	}
	fp = fopen(tmppath, "w");
	if (!fp) {
		errlog("topten write error");
		exit(1);
	}
	if(sec == 0)
		fprintf(fp, "<body><center>%s -- ����ʮ�����Ż���\n<hr>\n", MY_BBS_NAME);
	else
		fprintf(fp, "<center class=f2>����%c��ʮ�����Ż���\n\n", sec);
		
	fprintf(fp, "<table border=1>\n");
	fprintf
	    (fp,
	     "<tr><td>����</td><td>������</td><td>����</td><td>����</td><td>����</td></tr>\n");
	for (j = 0; j < 10 && bt->unum != 0; j++, bt++) {
		fprintf(fp,
			"<tr><td>�� %d ��</td><td><a href=bbshome?B=%d>%s</a></td>"
			"<td><a href='bbsnt?B=%d&th=%d'>%s</a></td><td>%d</td><td>"
			"<a href='qry?U=%s'>%s</a></td></tr>\n",
			j + 1, getbnum(bt->board), bt->board,
			getbnum(bt->board), bt->thread,
			nohtml(void1(bt->title)), bt->unum, bt->firstowner,
			bt->firstowner);
	}
	if(sec == 0) {
		fprintf(fp, "</table><br><br><hr>����ʮ�����Ż���<br>\n"
			    "<select name=sectop onchange='gosectop()'>");
		for(j=0; j < sectree.nsubsec; j++) {
			idx = &(sectree.seccodes[j]) - sectree.seccodes;
			fprintf(fp, "<option value=%c' %s>%s��ʮ��", sectree.seccodes[j],
				j ? "" : "SELECTED", sectree.subsec[idx]->title);
		}
		fprintf(fp, "</select><br>\n<script>function gosectop() {\n var sec;\n"
			"sec = document.getElementById('sectop');\n"
			"self.location.replace('bbstop10?sec='+ sec.value);\n}"
			"</script><center></body>");
		fprintf(fp, "</center></body>");
	} else
		fprintf(fp, "</table></center><body>");
			
	fclose(fp);
	strcpy(path, tmppath);
	ptr = strstr(path, ".tmp");
	if(ptr)
		*ptr = 0;
	rename(tmppath, path);
	return 0;
}

int
telnet_topten(int mode, char *file)
{
	struct boardtop *bt;
	int j;
	FILE *fp;
	char buf[40];
	char *p;
	if (mode)
		bt = ctopten;
	else
		bt = topten;
	fp = fopen("etc/topten.tmp", "w");
	if (!fp) {
		errlog("topten write error");
		exit(1);
	}
	fprintf(fp,
		"                \033[1;34m-----\033[37m=====\033[41m ����ʮ�����Ż��� \033[40m=====\033[34m-----\033[0m\n\n");
	for (j = 0; j < 10 && bt->unum != 0; j++, bt++) {
		strcpy(buf, ctime(&(bt->lasttime)));
		buf[20] = '\0';
		p = buf + 4;
		fprintf(fp,
			"\033[1;37m��\033[1;31m%3d\033[37m �� \033[37m���� : \033[33m%-16s\033[37m��\033[32m%s\033[37m��\033[36m%4d\033[37m ��  \033[35m%12s\033[m\n     \033[37m���� : \033[1;44;37m%-60.60s\033[40m\n",
			j + 1, bt->board, p, bt->unum, bt->firstowner,
			bt->title);
	}
	fclose(fp);
	rename("etc/topten.tmp", file);
	return 0;
}

int
main(int argc, char **argv)
{
	int i;
	struct boardheader bh;
	char *name;

	if (initbbsinfo(&bbsinfo) < 0) {
		printf("Failed to attach bbsinfo.\n");
		return 0;
	}

	while (1) {
		int c;
		c = getopt(argc, argv, "ah");
		if (c == -1)
			break;
		switch (c) {
		case 'a':
			allflag = 1;
			break;
		case 'h':
			printf
			    ("%s [-a|boardname]\n  do board top %d article\n",
			     argv[0], TOPN);
			return 0;
		case '?':
			printf
			    ("%s:Unknown argument.\nTry `%s -h' for more information.\n",
			     argv[0], argv[0]);
			return 0;
		}
	}
	chdir(MY_BBS_HOME);
	now_t = time(NULL);
	if (optind < argc) {
		name = argv[optind++];
		if (optind < argc) {
			printf
			    ("%s:Too many arguments.\nTry `%s -h' for more information.\n",
			     argv[0], argv[0]);
			return 0;
		}
		strncpy(bh.filename, name, STRLEN);
		_topn(&bh);
	}
	if (allflag) {
		topten = calloc(11, sizeof (struct boardtop));
		if (NULL == topten) {
			errlog("malloc failed");
			exit(1);
		}
		ctopten = calloc(11, sizeof (struct boardtop));
		if (NULL == ctopten) {
			errlog("malloc failed");
			exit(1);
		}
		
		csectopten = calloc(128, sizeof(struct boardtop *));
		if(NULL == csectopten) {
			errlog("malloc failed");
			exit(1);
		}
		bzero(csectopten, sizeof(struct boardtop *) * 128);
		
		if (file_exist(FILTERTITLE)) {
			if (mmapfile(FILTERTITLE, &filtermf) == -1) {
				errlog("mmap failed");
				exit(1);
			}
		}

		top_all_dir();
		telnet_topten(0, "etc/posts/day");
		telnet_topten(1, "etc/dayf");
		
		html_topten(0, 0);
		html_topten(1, 0);
		for(i=0; i<sectree.nsubsec; i++)
			html_topten(1, sectree.seccodes[i]);

		//Ҫexit�ˣ��ҾͲ�free�ˣ��Ǻ�
	}
	return 0;
}

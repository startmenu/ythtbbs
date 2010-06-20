#include "bbslib.h"

int
bbsbkncon_main()
{
	char board[80], bkn[80], dir[256], file[256], filename[256], *ptr;
	struct fileheader *x, *dirinfo;
	struct mmapfile mf = { ptr:NULL };
	struct boardmem *brd;
	int num, total;
	changemode(BACKNUMBER);
	getparmboard(board, sizeof(board));
	strsncpy(bkn, getparm("bkn"), 32);
	strsncpy(file, getparm("file"), 32);
	num = atoi(getparm("num"));
	ptr = bkn;
	while (*ptr) {
		if (*ptr != 'B' && *ptr != '.' && !isdigit(*ptr))
			http_fatal("����Ĳ���");
		ptr++;
	}
	if (strlen(bkn) < 3)
		http_fatal("����Ĳ���");
	if ((brd=getboard(board)) == NULL)
		http_fatal("�����������");
	if (strncmp(file, "M.", 2) && strncmp(file, "G.", 2))
		http_fatal("����Ĳ���1");
	if (strstr(file, "..") || strstr(file, "/"))
		http_fatal("����Ĳ���2");
	snprintf(dir, 256, "boards/.backnumbers/%s/%s/.DIR", board, bkn);
	total = file_size(dir) / sizeof (*x);
	if (total <= 0)
		http_fatal("�˾���������ڻ���Ϊ��");
	sprintf(filename, "boards/.backnumbers/%s/%s/%s", board, bkn, file);
	if (*getparm("attachname") == '/') {
		showbinaryattach(filename);
		return 0;
	}
	html_header(1);
	//check_msg();
	printf("<body><center>\n");
	printf("%s -- �����Ķ� [������: %s--����]<hr>", BBSNAME, board);
	MMAP_TRY {
		if (mmapfile(dir, &mf) == -1) {
			MMAP_UNTRY;
			http_fatal("�������������ڻ���Ϊ��");
		}
		if (num < 0 || num >= total) {
			mmapfile(NULL, &mf);
			MMAP_UNTRY;
			http_fatal("���Ĳ����ڻ����ѱ�ɾ��");
		}
		dirinfo =
		    (struct fileheader *) (mf.ptr +
					   num * sizeof (struct fileheader));
		if (dirinfo->owner[0] == '-') {
			mmapfile(NULL, &mf);
			MMAP_UNTRY;
			http_fatal("�����ѱ�ɾ��");
		}
		if (dirinfo->accessed & FH_HIDE) {
			mmapfile(NULL, &mf);
			MMAP_UNTRY;
			http_fatal("�����ѱ�ɾ��");
		}
		if (dirinfo->filetime != atoi(file + 2)) {
			mmapfile(NULL, &mf);
			MMAP_UNTRY;
			http_fatal("��Ų�̫�԰�...");
		}
		showcon(filename);
		printf("[<a href=bbsfwd?B=%d&file=%s>�Ƽ�������</a>]", getbnumx(brd),
		       file);
		printf("[<a href=bbsccc?B=%d&file=%s>ת��</a>]", getbnumx(brd),
		       file);
		if (num > 0) {
			x = (struct fileheader *) (mf.ptr +
						   (num -
						    1) *
						   sizeof (struct fileheader));
			printf
			    ("[<a href=bbsbkncon?B=%d&bkn=%s&file=%s&num=%d>��һƪ</a>]",
			     getbnumx(brd), bkn, fh2fname(x), num - 1);
		}
		printf("[<a href=bbsbkndoc?B=%d&bkn=%s>�������</a>]",
		       getbnumx(brd), bkn);
		if (num < total - 1) {
			x = (struct fileheader *) (mf.ptr +
						   (num +
						    1) *
						   sizeof (struct fileheader));
			printf
			    ("[<a href=bbsbkncon?B=%d&bkn=%s&file=%s&num=%d>��һƪ</a>]",
			     getbnumx(brd), bkn, fh2fname(x), num + 1);
		}
	}
	MMAP_CATCH {
		mmapfile(NULL, &mf);
		MMAP_RETURN(-1);
	}
	MMAP_END mmapfile(NULL, &mf);
	printf("</center>");
	showfile(MY_BBS_HOME "/wwwtmp/googleanalytics");
	printf("</body>\n");
	http_quit();
	return 0;
}

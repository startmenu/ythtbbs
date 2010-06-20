#include "bbslib.h"
int quote_quote = 0;

int
bbsincon_main()
{
	char *path_info;
	char *name, filename[128], dir[128], board[40];
#if 0
	struct fileheader *dirinfo;
	struct mmapfile mf = { ptr:NULL };
	int num = -1;
#endif
	extern char *cginame;
	int old_quote_quote;
	path_info = getsenv("SCRIPT_URL");
	path_info = strchr(path_info + 1, '/');
	if (NULL == path_info)
		http_fatal("������ļ���");
	if (!strncmp(path_info, "/boards/", 8))
		path_info += 8;
	else
		http_fatal("������ļ���1 %s", path_info);
	name = strchr(path_info, '/');
	if (NULL == name)
		http_fatal("������ļ���2");
	*(name++) = 0;
	strtok(name, "+");
	strsncpy(board, path_info, sizeof (board));
	//translate board num to actual board name
	if (!getboard(board))
		http_fatal("������ļ���3");
	if (strncmp(name, "M.", 2) && strncmp(name, "G.", 2))
		http_fatal("����Ĳ���1");
	if (strstr(name, "..") || strstr(name, "/"))
		http_fatal("����Ĳ���2");
	snprintf(filename, sizeof (filename), "boards/%s/%s", board, name);
	sprintf(dir, "boards/%s/.DIR", board);
#if 0
	MMAP_TRY {
		if (mmapfile(dir, &mf) == -1) {
			MMAP_UNTRY;
			http_fatal("�������������ڻ���Ϊ��");
		}
		dirinfo = findbarticle(&mf, name, &num, 1);
		if (dirinfo == NULL) {
			mmapfile(NULL, &mf);
			MMAP_UNTRY;
			http_fatal("���Ĳ����ڻ����ѱ�ɾ��");
		}
		if (dirinfo->owner[0] == '-') {
			mmapfile(NULL, &mf);
			MMAP_UNTRY;
			http_fatal("�����ѱ�ɾ��");
		}
		if (cache_header(fh2modifytime(dirinfo), 86400)) {
			mmapfile(NULL, &mf);
			MMAP_RETURN(0);
		}
	}
	MMAP_CATCH {
		mmapfile(NULL, &mf);
		MMAP_RETURN(-1);
	}
	MMAP_END mmapfile(NULL, &mf);
#else
	if (cache_header(file_time(filename), 86400))
		return 0;
#endif
//      dirty code for set a bbscon run environ for fshowcon
	parm_add("F", name);
	parm_add("B", board);
//      dirty code end
	printf("Content-type: application/x-javascript; charset=%s\n\n",
	       CHARSET);
	printf("<!--\n");
	fputs("docStr=\"", stdout);
	old_quote_quote = quote_quote;
	quote_quote = 1;
	cginame = "bbscon";
	binarylinkfile(getparm2("F", "file"));
	fshowcon(stdout, filename, 2);
	cginame = "boards";
	quote_quote = old_quote_quote;
	puts("\";");
	puts("//-->");
	return 0;
}

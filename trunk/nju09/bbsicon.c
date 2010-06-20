#include "bbslib.h"

int
bbsicon_main()
{
	//int num;
	char board[40], buf[256], link[STRLEN];
	struct boardmem *x1;
	struct mmapfile mf = { ptr:NULL };

	getparmboard(board, sizeof (board));
	x1 = getboard2(board);
	if (!x1) {
		html_header(1);
		http_fatal("�����ڵİ���");
		return 0;
	}
	strncpy(link, getparm2("L", "link"), STRLEN - 1);

	if(!link[0]) {	//����logo
		if(!x1->wwwicon) {
			html_header(1);
			http_fatal("ͼ���ļ�������");
			return 0;
		}
		sprintf(buf, "ftphome/root/boards/%s/html/icon.gif", board);
	} else {
		if(!x1->wwwlink) {
			html_header(1);
			http_fatal("ͼ���ļ�������");
			return 0;
		}
		sprintf(buf, "ftphome/root/boards/%s/link/%s", board, link);
	}
	if (cache_header(file_time(buf), 10000)) {
		return 0;
	}
	if (file_size(buf) > 1024 * 20) {
		html_header(1);
		http_fatal("�ļ��������� ftp ����");
	}
	MMAP_TRY {
		if (mmapfile(buf, &mf)) {
			MMAP_UNTRY;
			http_fatal("������ļ���");
		}
		printf("Content-type: %s\r\n\r\n", get_mime_type("icon.gif"));
		//printf("Content-Length: %d\r\n\r\n", mf.size);
		fwrite(mf.ptr, 1, mf.size, stdout);
	}
	MMAP_CATCH {
	}
	MMAP_END {
		mmapfile(NULL, &mf);
	}
	return 0;
}

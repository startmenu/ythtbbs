/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu
    Firebird Bulletin Board System
    Copyright (C) 1996, Hsien-Tsung Chang, Smallpig.bbs@bbs.cs.ccu.edu.tw
                        Peng Piaw Foong, ppfoong@csie.ncu.edu.tw
    
    Copyright (C) 1999, KCN,Zhou Lin, kcn@cic.tsinghua.edu.cn

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/

#include "bbs.h"
#include "ythtbbs.h"
#include "record.h"

#define BUFSIZE (8192)

const struct ANSI2HTML_UBB ubb2[] = {
	{"30", "<font class=c30>", 16, "[color=white]"},
	{"31", "<font class=c31>", 16, "[color=red]"},
	{"32", "<font class=c32>", 16, "[color=green]"},
	{"33", "<font class=c33>", 16, "[color=yellow]"},
	{"34", "<font class=c34>", 16, "[color=blue]"},
	{"35", "<font class=c35>", 16, "[color=purple]"},
	{"36", "<font class=c36>", 16, "[color=cyan]"},
	{"37", "<font class=c37>", 16, "[color=black]"},
	{"40", "<font class=b40>", 16, "[bcolor=white]"},
	{"41", "<font class=b41>", 16, "[bcolor=red]"},
	{"42", "<font class=b42>", 16, "[bcolor=green]"},
	{"43", "<font class=b43>", 16, "[bcolor=yellow]"},
	{"44", "<font class=b44>", 16, "[bcolor=blue]"},
	{"45", "<font class=b45>", 16, "[bcolor=purple]"},
	{"46", "<font class=b46>", 16, "[bcolor=cyan]"},
	{"47", "<font class=b47>", 16, "[bcolor=black]"}
};

long
get_num_records(filename, size)
char *filename;
int size;
{
	struct stat st;

	if (stat(filename, &st) == -1)
		return 0;
	return (st.st_size / size);
}

void
toobigmesg()
{
/*
    prints( "record size too big!!\n" );
    oflush();
*/
}

int
get_record(filename, rptr, size, id)
char *filename;
void *rptr;
int size, id;
{
	int fd;

	if ((fd = open(filename, O_RDONLY, 0)) == -1)
		return -2;
	if (lseek(fd, size * (id - 1), SEEK_SET) == -1) {
		close(fd);
		return -3;
	}
	if (read(fd, rptr, size) != size) {
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

int
get_records(filename, rptr, size, id, number)
char *filename;
void *rptr;
int size, id, number;
{
	int fd;
	int n;

	if ((fd = open(filename, O_RDONLY, 0)) == -1)
		return -1;
	if (lseek(fd, size * (id - 1), SEEK_SET) == -1) {
		close(fd);
		return 0;
	}
	if ((n = read(fd, rptr, size * number)) == -1) {
		close(fd);
		return -1;
	}
	close(fd);
	return (n / size);
}

int
substitute_record(filename, rptr, size, id)
char *filename;
void *rptr;
int size, id;
{
#ifdef LINUX
	struct flock ldata;
	int retval;
#endif
	int fd;
	if ((fd = open(filename, O_WRONLY | O_CREAT, 0660)) == -1)
		return -1;
#ifdef LINUX
	ldata.l_type = F_WRLCK;
	ldata.l_whence = 0;
	ldata.l_len = size;
	ldata.l_start = size * (id - 1);
	if ((retval = fcntl(fd, F_SETLKW, &ldata)) == -1) {
		return -1;
	}
#else
	flock(fd, LOCK_EX);
#endif
	if (lseek(fd, size * (id - 1), SEEK_SET) == -1) 
		return -1;
	if (safewrite(fd, rptr, size) != size)
		return -1;
#ifdef LINUX
	ldata.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &ldata);
#else
	flock(fd, LOCK_UN);
#endif
	close(fd);
	return 0;
}

int
update_file(dirname, size, ent, filecheck, fileupdate)
char *dirname;
int size, ent;
int (*filecheck) ();
void (*fileupdate) ();
{
	char abuf[BUFSIZE];
	int fd;

	if (size > BUFSIZE) {
		toobigmesg();
		return -1;
	}
	if ((fd = open(dirname, O_RDWR)) == -1)
		return -1;
	flock(fd, LOCK_EX);
	if (lseek(fd, size * (ent - 1), SEEK_SET) != -1) {
		if (read(fd, abuf, size) == size)
			if ((*filecheck) (abuf)) {
				lseek(fd, -size, SEEK_CUR);
				(*fileupdate) (abuf);
				if (safewrite(fd, abuf, size) != size) {
					flock(fd, LOCK_UN);
					close(fd);
					return -1;
				}
				flock(fd, LOCK_UN);
				close(fd);
				return 0;
			}
	}
	lseek(fd, 0, SEEK_SET);
	while (read(fd, abuf, size) == size) {
		if ((*filecheck) (abuf)) {
			lseek(fd, -size, SEEK_CUR);
			(*fileupdate) (abuf);
			if (safewrite(fd, abuf, size) != size) {
				flock(fd, LOCK_UN);
				close(fd);
				return -1;
			}
			flock(fd, LOCK_UN);
			close(fd);
			return 0;
		}
	}
	flock(fd, LOCK_UN);
	close(fd);
	return -1;
}

unsigned int separateattachment(char* filepath, unsigned int currentpos, struct attachmentstruct *single_attach )
{
	short int attachnum, i, ispicture;
	// attachnum �����ֵӦ����Ҫ���أ���Ϊ��д��forum_post��
	unsigned int attachlen; // ������С
    struct mmapfile mf = { ptr:NULL };
    char buf[512];
    short int len;
	char *ptr, *str; // ���ڴ�����
	int size;
	char *ext;  // ��������չ��
	char extname[10];
	char attachname[256]; //������ʾ�������������
	FILE *attachfp;  // �����ļ�ָ��
	char attachfilepath[1024]; //����Ŀ¼
	char attachfilename[1024]; //�������ļ�ϵͳ�е���ʵ����
	char attachfilefullpath[1024]; //�������ļ�Ŀ¼
	char yearmonthdate1[7];  // current year, month
	char yearmonthdate2[3];  // current date
	char hourminsec[7];  //current hour, minute and second
	char randomnumalpha16[17]; // save 16-length random number+lower-case alphabet string
	time_t now;
	struct tm time_p = {0};
	unsigned int total_attachlen;  // total size occupied by attachments
	char attachfiledbpath[1024]; //����д�����ݿ��е�Ŀ¼
	char attachfiledbfullpath[1024]; //�������ļ�д�����ݿ��е�λ��

	printf("It has attachment\n");
	attachnum = 0;
	if (mmapfile(filepath, &mf) < 0)
	{
		printf("mmapfile failed in %s\n", filepath);
	}

	str = mf.ptr + currentpos;
	size = mf.size;   // ָ���ļ�ӳ���ڴ��ָ�룬�Լ�ԭʼ�ļ��Ĵ�С�����ڴ�ӡ�����ָ��λ��

	size = size - currentpos;
//	printf("currentpos=%d  size=%d\n", currentpos, size);
	total_attachlen = 0;
	while (size > 18)
	{
		// �Ȳ�������һ�����д�
		ptr = str;
		len = min(size, sizeof(buf)-1);
		str = memchr(ptr, '\n', len);

		if (!str)
			str = ptr + len;
		else {
			str++;
			len = str - ptr;
		}
		size -= len;

		// ����Ǹ���
		if (len > 18 && !strncmp(ptr, "beginbinaryattach ", 18)) {
			strsncpy(buf, ptr, len); // �����д���buf�ַ�����
			ptr = strchr(buf, ' ');  // ���ҵ�һ���ո���ֵ�λ�ã������Ǹ������з�Ϊֹ����attachname
			strcpy(attachname, ptr);
			// ���ҳ��������ͣ��б��Ƿ�ͼƬ���Լ�ͼƬ����
			printf("Attachname: %s\n", attachname);
			ispicture = 0;
			ext = strrchr(attachname, '.');
			if (ext != NULL) {
				if (!strcasecmp(ext, ".bmp") || !strcasecmp(ext, ".jpg")
				    || !strcasecmp(ext, ".gif") || !strcasecmp(ext, ".jpeg")
				    || !strcasecmp(ext, ".png"))
				{
					ispicture = 1;
				}
				else
				{
					ispicture = 0;
				}
			}
			i = 0;
			while((*(ext+i)!='\n') &&(*(ext+i)!='\0')&& (i<9))
			{
				extname[i] = *(ext+i);
//				printf("extname[%d] = %c\n", i, extname[i]);
				i++;
			}
			extname[i] = '\0';
			printf("extname=%s\n", extname);

			// �������ڶ������д�
			ptr = str;
			// strָ��ָ�򸽼��Ŀ�ͷ���ڴ˴�����
			// ����ļ����������ӣ�Ȼ��д�븽����
			attachlen = ntohl(*(unsigned int *) (ptr + 1));
			// ���ַ���д���ļ�����ptr+1һֱдattachlen���ַ�������β
			printf("attachlen=%d\n", attachlen);
			time(&now);
			localtime_r(&now, &time_p);
			strftime(yearmonthdate1, 7, "%Y%m", &time_p);
			strftime(yearmonthdate2, 3, "%d", &time_p);
			strftime(hourminsec, 7, "%H%M%S", &time_p);
//			strftime(yearmonthdate1, 7, "%Y%m", localtime(&now));
//			strftime(yearmonthdate2, 3, "%d", localtime(&now));
//			strftime(hourminsec, 7, "%H%M%S", localtime(&now));
			//testend
			printf("%s\n",yearmonthdate1);
			sprintf(attachfilepath, "/var/www/dx/data/attachment/forum/%s", yearmonthdate1);
			sprintf(attachfiledbpath, "%s", yearmonthdate1);
			printf("attachfilepath here=%s\n", attachfilepath);
			if(access(attachfilepath,0)) // �ж�Ŀ¼�Ƿ���� 0Ҳ����F_OK��ʾcheck existence
			{
				printf("directory %s does not exist!\n", attachfilepath);
				// ������Ӧ��Ŀ¼
				mkdir(attachfilepath, 0700);
			}
			printf("successfully here\n");
			sprintf(attachfilepath, "%s/%s", attachfilepath, yearmonthdate2);
			sprintf(attachfiledbpath, "%s/%s", attachfiledbpath, yearmonthdate2);
			if(access(attachfilepath,0)) // ���ڵ�Ŀ¼�Ƿ����
			{
				printf("directory %s does not exist!\n", attachfilepath);
				// ������Ӧ��Ŀ¼
				mkdir(attachfilepath, 0700);
			}
			//			printf("attachfilepath: %s\n", attachfilepath);
			//			printf("hourminsec: %s\n", hourminsec);
			randomnumalpha(randomnumalpha16, 16); // generate a string contains 16 random number or lower-case alphabets
			sprintf(attachfilename, "%s%s%s", hourminsec, randomnumalpha16, extname);
			sprintf(attachfilefullpath, "%s/%s", attachfilepath, attachfilename);
			sprintf(attachfiledbfullpath, "%s/%s", attachfiledbpath, attachfilename);
			printf("attachfilename: %s\n", attachfilename);
			printf("attachfilefullpath: %s\n", attachfilefullpath);
//			printf("attachlen=%d\n", attachlen);

			// �������洢Ϊ�ļ�
			attachfp = fopen(attachfilefullpath, "wb");  //����һ���������ļ�
			ptr = ptr + 5;
			fwrite(ptr, 1, attachlen, attachfp);
			fclose(attachfp);

			// ����д�븽�����ݱ�Ľṹ��
//			single_attach->ano = attachnum;
			single_attach->dateline = now;
			strcpy(single_attach->filename, attachname);
			strcpy(single_attach->attachment, attachfiledbfullpath);
			single_attach->filesize = attachlen;
			if(ispicture == 1)
			{
				single_attach->isimage = 1;
				single_attach->width = 640;
			}
			else
			{
				single_attach->isimage = 0;
				single_attach->width = 0;
			}

			str = str + attachlen +5;
			size = size - attachlen - 5;
//			total_attachlen = total_attachlen + single_attach->filesize + 5;
			total_attachlen = total_attachlen + attachlen + 5;
			break;
		}//end ����Ǹ���
	}
//	printf("successfully extract file!!!\n");
	return total_attachlen;
//	exit(0);  // for test
}

int ythtfiletodiscuzxubb(char* filepath, char* ubbresult, char* useip, struct attachmentstruct *attinfo)
{
	// Drop Article Head
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	int i, m;
	char ch;
	char *newline = NULL;
	char ansibuf[80], ubbbuf[80];
	char errormsg[256];
	char *tmp;
	char ubbcontrolstack[20];  // ubb���Ʒ��Ҳ�ջ�������\033[m���ƣ������Ҫ����ƥ���ϵ
	// 'u' Ϊunderline 'c' Ϊcolor 'b'Ϊ bgcolor
	short unsigned int ubbcontrolptr;  //ָ���ϸ�stack��ָ��
	int ansinum;
	short int color, underline, bgcolor, bold, italic;
	short int remove_telnet_head, ipplace, islastline;
	short int attnum; // attachment number of this post
	unsigned int row_len;
	unsigned int current_position; // current position of the file
	unsigned int total_attachlen;  // total attachment length in single attachment reading

	newline = (char*) malloc(100000);
	fp = fopen(filepath,"r");
	if(!fp)
		return -1;
	/* getline()��gcc��չ��ssize_t getline(char **lineptr, size_t *n, FILE *stream);
	 ����ssize_t��int��size_t��long unsigned int
	 line����malloc���������Զ�realloc */
	ubbcontrolptr = 0;
	color = 0; underline = 0; bgcolor = 0; bold = 0; italic = 0;
	remove_telnet_head = 0;
	islastline = 0;
	current_position = 0;
	attnum = 0;
	while((read = getline(&line, &len, fp))>0)
	{
		// transfer ansi to UBB or attachment
		// ��ȡ�ַ�������
//		printf("UP current_position=%d\n", current_position);
		row_len = strlen(line);
//		printf("row_len=%d,\n line: %s", row_len, line);  // for test
		if(islastline == 1)  // if wrongly recognize last line
		{
			islastline = 0;
			strcat(ubbresult, newline);
		}
		newline[0] = '\0';
		if(remove_telnet_head == 0)
		{
			// remove telnet file head
			if(strstr(line, "����վ: ")!= NULL)
			{
				remove_telnet_head = 1;
				sprintf(newline, "����ͨ��һ·BBSվtelnet�ͻ��˷���\n");
				strcat(ubbresult, newline);
			}
			continue;
		}

		current_position = current_position + row_len;
		// �жϸ���
		if (row_len > 10 && !strncmp(line, "begin 644 ", 10)) {
			// ������ʽͼƬ������Ӧ���ǲ����ڵģ�������ھͱ���
			sprintf(errormsg, "old attach found! in %s\n", filepath);
			writesyslog(ERRORLOG, errormsg);
			continue;
		}
		if (row_len > 18 && !strncmp(line, "beginbinaryattach ", 18)) {
			// ��ͼƬ�������������͸���
			// beginbinaryattach ����Ӧ�÷����ļ�������һ���ַ�������ȡ��������
			// ��һ�п�ʼ�Ǹ���
			printf("attnum=%d\n", attnum);
			total_attachlen = separateattachment(filepath, current_position, (attinfo+attnum));
			(attinfo+attnum)->ano = attnum;
			attnum++;
			if(attnum>50)
			{
				sprintf(errormsg, "attachnum > 50 %s\n", filepath);
				writesyslog(ERRORLOG, errormsg);
				exit(1);
			}
			current_position = current_position + total_attachlen;
			fseek(fp, total_attachlen, SEEK_CUR);
//			fseek(fp, current_position, SEEK_SET);
		}

		for(i = 0; i< read; i++)
		{
			ch = *(line+i);
			/* ��Ŀǰ�����������discuzx����֧������[nobbc]֮��ı�ǩ��Ҳ��֧�ָ��ӱ�ǩ����������Ϊ�Ű��ϵ�����
			 ��������������÷�������������ֻᱻǿ��ת������ʱû�а취�ų���ֻ�����û�ʹ��[code][/code]���
			 */
			if(ch == '\033')  //telnet��ʹ�õ�ANSI����\ 033��ʼ�����ǰ˽��Ʊ�ʾ
			{
				// ����Ҫע��ģ�һ����telnet���кڰ׷�������⣬��һ���Ǳջ������⣬telnet�±�ɫһ�㲻�ջ�
				if (*(line + i + 1) != '[') {  //����ANSI���Ʒ��Ļ������
					sprintf(newline, "%s%c", newline, ch);
					continue;
				}
				for (m = i + 2; (m < read) && (m < i + 24); m++)
					if (strchr("0123456789;", *(line+m)) == 0)
						break;
				strsncpy(ansibuf, line + i + 2, m - (i + 2) + 1);
				if (*(line + m) != 'm') { //�ٿ�һ�£��ǲ���ANSI���Ʒ�
					sprintf(newline, "%s%c", newline, ch);
					continue;
				}
//				printf("ansibuf = %s\n", ansibuf);
				if(sizeof(ansibuf) == 0)
				{
					ansinum = 0;
				}
				else
				{
					if(strstr(ansibuf, ";"))
					{
						tmp = strtok(ansibuf, ";");
						if(tmp != NULL)
						{
							tmp = strtok(NULL, ";");
							ansinum = atoi(tmp);
						}
						else
							ansinum = 0;
					}
					else
					{
						ansinum = atoi(ansibuf);
//						printf("ansinum = %d\n", ansinum);
					}
				}
//				printf("ansinum=%d\n", ansinum);
				i = m; //����ָ��λ�ã�Խ������ANSI���Ʒ�
				if(ansinum == 101)
				{
					if(bold == 0)
					{
						sprintf(ubbbuf, "%s", "[b]");
						bold = 1;
						ubbcontrolstack[ubbcontrolptr]='o'; //bold
						ubbcontrolptr++;
					}
					else
					{
						sprintf(ubbbuf, "%s", ""); // ignore
					}
				}
				else if(ansinum == 102)
				{
					if(bold == 1)
					{
						sprintf(ubbbuf, "%s", "[/b]");
						if(ubbcontrolstack[ubbcontrolptr - 1] == 'o')
						{
							ubbcontrolptr--;
							bold = 0;
						}
					}
					else
					{
						sprintf(ubbbuf, "%s", ""); // ignore
					}
				}
				else if(ansinum == 111)
				{
					if(italic == 0)
					{
						sprintf(ubbbuf, "%s", "[i]");
						italic = 1;
						ubbcontrolstack[ubbcontrolptr]='i'; //italic
						ubbcontrolptr++;
					}
					else
					{
						sprintf(ubbbuf, "%s", ""); // ignore
					}
				}
				else if(ansinum == 112)
				{
					if(italic == 1)
					{
						sprintf(ubbbuf, "%s", "[/i]");
						if(ubbcontrolstack[ubbcontrolptr - 1] == 'i')
						{
							ubbcontrolptr--;
							italic = 0;
						}
					}
					else
					{
						sprintf(ubbbuf, "%s", ""); // ignore
					}
				}
				else if(ansinum == 4)
				{
					if (underline == 0)
					{
						underline = 1;
						sprintf(ubbbuf, "%s", "[u]");
						ubbcontrolstack[ubbcontrolptr]= 'u'; //underline
						ubbcontrolptr++;
					}
					else
					{
						sprintf(ubbbuf, "%s", ""); // ignore
					}
				}
				else if(ansinum >= 30 && ansinum <= 37)
				{
					if (color == 0)
					{
						color = 1;
						sprintf(ubbbuf, "%s", ubb2[ansinum-30].ubb);
						ubbcontrolstack[ubbcontrolptr]= 'c'; //color
						ubbcontrolptr++;
					}
					else
					{
						sprintf(ubbbuf, "[/color]%s", ubb2[ansinum-30].ubb);
					}
				}
				else if(ansinum >=40 && ansinum <= 47)
				{
					if (bgcolor == 0)
					{
						bgcolor = 1;
						sprintf(ubbbuf, "%s", ""); // discuz��֧�ֱ���ɫ�����Ժ��ԣ�����ΪҪ�ұջ�ƥ�䣬���Եü�
						ubbcontrolstack[ubbcontrolptr]= 'b'; //bgcolor
						ubbcontrolptr++;
					}
					else
					{
						sprintf(ubbbuf, "%s", ""); // ignore
					}
				}
				else if(ansinum == 0)
				{
					if (ubbcontrolptr <= 0)
					{
						// �����������������
						sprintf(ubbbuf, "%s", ""); // ignore
						ubbcontrolptr = 0;
						underline = 0;
						color = 0;
						bgcolor = 0;
					}
					else
					{
						if(ubbcontrolstack[ubbcontrolptr - 1] == 'b')  //bgcolor
						{
							sprintf(ubbbuf, "%s", "");
							ubbcontrolptr--;
							bgcolor = 0;
						}
						else if(ubbcontrolstack[ubbcontrolptr - 1] == 'c')  //color
						{
							sprintf(ubbbuf, "%s", "[/color]");
							ubbcontrolptr--;
							color = 0;
						}
						else if(ubbcontrolstack[ubbcontrolptr - 1] == 'u') //underline
						{
							sprintf(ubbbuf, "%s", "[/u]");
							ubbcontrolptr--;
							underline = 0;
						}
						else if(ubbcontrolstack[ubbcontrolptr - 1] == 'o')  //bold
						{
							sprintf(ubbbuf, "%s", "[/b]");
							ubbcontrolptr--;
							bold = 0;
						}
						else if(ubbcontrolstack[ubbcontrolptr - 1] == 'i')  //italic
						{
							sprintf(ubbbuf, "%s", "[/i]");
							ubbcontrolptr--;
							italic = 0;
						}
						else
						{
							sprintf(ubbbuf, "%s", "");
							ubbcontrolptr--;
						}
					}
				}
				else
				{
					sprintf(ubbbuf, "%s", ""); // ignore
				}
				strcat(newline, ubbbuf);
			}
			else
			{
				sprintf(newline, "%s%c", newline, ch);
			}
		}
//		printf("newline: %s\n", newline);  // for test
//		printf("newline length = %d\n", strlen(newline));  //for test
		// delete the last line
		if(strstr(newline , "�� ��Դ:��"))
			islastline = 1;
		else if(strstr(newline, "beginbinaryattach"))
			; // do nothing
		else
			strcat(ubbresult, newline);
	}
	if(strstr(newline, "�� ��Դ:��")&&(!strstr(newline, "����")))
	{
//		printf("newline: %s\n", newline);  // for test
		tmp = strstr(newline, "[FROM: ");
		ipplace = 0;
		useip[0]='\0';
		while(*tmp != ']' && *tmp!='\0')
		{
			if(ipplace == 1)
			{
				if(*tmp!= ' ')
					sprintf(useip, "%s%c",useip,*tmp);
			}
			if(*tmp == ':')
				ipplace = 1;
			tmp++;
		}
	}
	else
	{
		strcpy(useip, "0.0.0.0");
	}
	(attinfo+attnum)->ano = -1;
//	printf("%s\n", ubbresult);  // for testing
	printf("useip: %s\n", useip);
	fclose(fp);
	if(line)
		free(line);
	if(newline)
		free(newline);
	return 0;
}


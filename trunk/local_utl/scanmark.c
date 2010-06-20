/*
���ø�ʽ�� scanmark ������һ�����cronÿ����� scanmark 1����
���ã����� 24Сʱ֮ǰ argv[1] ������mark������(һ���������ɨ��5000ƪ)
��ȥһЩ���������������ǵ��������߼�Ǯ���Ӿ���ֵ
base on happybird's markday.c 2005/1/5
develeped by koyu       2005/6/29
*/

#include "bbs.h"
#include "sysrecord.c"
#include "ythtbbs.h"

#define MARKED_FILE MY_BBS_HOME"/0Announce/bbslist/marked_article"      //ÿ��mark����ͳ��
#define BASECASH        500     //�������
#define EXTRACASH       100     //ÿEXTRAWORD��׷�Ӹ��
#define MAXCASH         8888	//��ƪ�����
#define STARTWORD       20     	//������������
#define EXTRAWORD       100     //����������λ
#define MAXIDNUM	512	//һ��ɨ�������¼��������

char id[MAXIDNUM][14];
int percash[MAXIDNUM];
short perarticle[MAXIDNUM];
int perexp[MAXIDNUM];
int n=0;
int sum=0;
struct boardmem bp;
//#define DEBUG
#define DETAIL

struct bbsinfo bbsinfo; //ȫ�ֱ���
MC_Env *mcEnv;

//��¼��bbslist�ļ�
int append2file(char *file,char *buf)
{
        FILE *fp;
        fp= fopen(file,"a");
        if(!fp) return 0;
        fprintf(fp,"%s",buf);
        fclose(fp);
        return 1;
}

//���û���Ǯ
//���
int give_money(struct boardheader *bh,struct fileheader *fhdr,int cash)
{
        mcUserInfo *mcuInfo;
        char buf[512];
        int i, baseexp;
        time_t now_t;
        struct userdata data;
#if 1
	if(bp.score>=10000 )
		baseexp = 10;
	else if(bp.score >= 3000)
		baseexp = 5;
	else
		baseexp = 1;
#else
	baseexp = bp.score / 1000  + 1;
	baseexp = baseexp >10?10:baseexp;
#endif
	if(strstr(bh->filename, "Digest") && strstr(bh->title, "����ժ" ))	//��ժ������⽱��
		baseexp = 20;
		
        //�򿪸��˵�moneycenter�ļ�
        sprintf(buf,MY_BBS_HOME"/home/%c/%s/mc.save",mytoupper(fhdr->owner[0]),fhdr->owner);
        now_t = time(NULL);
        if(!file_exist(buf))
            initData(1, buf);    //�ļ������ڣ���ʼ��
	if((mcuInfo = loadData(buf, sizeof (mcUserInfo))) == (void*)-1)
            return -1;
#ifndef DEBUG
	//if(mcEnv->Treasury >= cash){	//���⻹�ܸ�Ǯ
	        mcuInfo->cash += cash;  //��Ǯ��
	//      mcEnv->Treasury -= cash;
		sum += cash;
	//}
	//else
	//	cash = 0;		//����ûǮ�ˡ�����
#endif
        unloadData(mcuInfo, sizeof(mcUserInfo));

        loaduserdata(fhdr->owner, &data);		//������ֵ
#ifndef DEBUG
       	data.extraexp += baseexp;        
#endif
        saveuserdata(fhdr->owner, &data);
        
        for (i=0;i<n;i++) {     //���������Ƿ�����ǰ���ɨ�����н�����¼
            if (!strcmp(id[i],fhdr->owner)) {
                perarticle[i]+=1;
                percash[i]+=cash;
                perexp[i]+= baseexp;
                goto APP;
            }
        }
        strcpy(id[n],fhdr->owner);      //�����µļ�¼
        perarticle[n]=1;
        percash[n]=cash;
        perexp[n]=baseexp;
        n=n+1;
        if(n >= MAXIDNUM)
        	postfile(MARKED_FILE, "deliver", "Code_Discuss", "[�Զ�ɨ��]�����С�����ˣ�");
        goto APP;
        //��¼���ļ�
        //����           ����            ���±���                   ����       ��Ϳ��  ����ֵ
        //========== ========= ================================= =========== ========= ========
APP:    sprintf(buf,"%-10.10s \033[1;32m%-10.10s\033[m %-30.30s \033[1;33m%-10.10s\033[m \033[1;31m%-8d %-7d\033[m\n",bh->filename,bh->bm[0],fhdr->title,fhdr->owner,cash, baseexp);                 
        append2file(MARKED_FILE,buf);
        
        return 1;
}

int
denyUser(char *author)
{
        char buf[256], *ptr;
        FILE *fp;
        int retv = 0;
        if (!strcmp(author, "Anonymous"))       //�����û�����
                return 1;

        fp = fopen("deny_users", "rt");         //������û�����
        if (!fp)
                return 0;
        while (fgets(buf, sizeof (buf), fp)) {
                ptr = strtok(buf, " \t\n\r");
                if (!ptr)
                        continue;
                if (!strcasecmp(ptr, author)) {
                        retv = 1;
                        break;
                }
        }
        fclose(fp);
        return retv;
}

int
searchLastMark(struct boardheader *bh, int days)
{
        char filename[256];
        struct fileheader fhdr;
        int fd, total, n, old = 0;
        time_t now;
        int cash;

        sprintf(filename, MY_BBS_HOME "/boards/%s/.DIR", bh->filename);

        if ((total = file_size(filename) / sizeof (fhdr)) <= 0)
                return 0;

        time(&now);
        if ((fd = open(filename, O_RDONLY, 0)) == -1) {
                return 0;
        }
		
        for (n = total - 1; n >= 0 && total - n < 5000; n--) {  //���ֻɨ��5000ƪ
                if (lseek(fd, n * sizeof (fhdr), SEEK_SET) < 0)
                        break;
                if (read(fd, &fhdr, sizeof (fhdr)) != sizeof (fhdr))
                        break;
                if (now - fhdr.filetime < 3600 * 24 * 1)        //24Сʱ֮����������
                    continue;
                if (now - fhdr.filetime > 3600 * 24 *(days + 1)) { //û��n���ڵ�������
                        old++;
                        if (old > 4)    //���ǵ���ʱ���д�ɾ�����Ȼָ��ľ�����
                                break;
                        continue;
                }
                if (!strcmp(fhdr.owner, "deliver")      //ϵͳ���Ĳ���
                    && !(fhdr.accessed & FH_DIGEST)     
                    && !(fhdr.accessed & FH_MARKED))    //�Ǳ��
                        continue;
                if (fhdr.owner[0] == '-'                //�����ʲô��
                    || !strcmp(fhdr.owner, "post") 
                    || !strcmp(fhdr.owner, "deliver") 
                    || strstr(fhdr.title, "[����]")
                //    || strstr(fhdr.title, "[����]")
                //    || strstr(fhdr.title, "[ά��]")
                //    || strstr(fhdr.title, "[����]")     //��������
                    || strstr(fhdr.title, "[�ϼ�]")     //�����ϼ�
                    || (fhdr.accessed & FH_DANGEROUS))  //Σ�ձ������
                        continue;
                if (bytenum(fhdr.sizebyte) <= STARTWORD)        //��ý������������������
                        continue;
                if (denyUser(fh2owner(&fhdr)))          //�����������߱��������
                        continue;
                if ((fhdr.accessed & FH_DIGEST) || (fhdr.accessed & FH_MARKED)) {       //��M����G������
                        cash = BASECASH + EXTRACASH*(bytenum(fhdr.sizebyte)-STARTWORD)/EXTRAWORD;  //������
                        cash = cash > MAXCASH ? MAXCASH : cash;         //���������
                        give_money(bh,&fhdr,cash);                      //�����߷�Ǯ��������ֵ
                        
#ifdef DEBUG
                        printf("%s %s size:%d cash:%d %d %d %d %d %d\n",fhdr.owner,fhdr.title,bytenum(fhdr.sizebyte),cash,BASECASH,STARTWORD,EXTRAWORD,EXTRACASH,MAXCASH);
#endif
                }
        }
        close(fd);
        return 1;
}

int main( int argc, char *argv[] )
{

        int b_fd, bnum = -1;
        int i,days;
        char title[80];
        char content[256];
        struct boardheader bh;
        char filepath[256];
        char day[40], buf[256];
        time_t now_t;
        int size;
        FILE *fp;
        size = sizeof (bh);
        
        if( argc != 2 ){
            printf("Wrong numbers of paramaters!\n");
            return -1;
        }
	if (initbbsinfo(&bbsinfo) < 0){
		printf("Init bbs info failed.\n");
		return -1;
	}
	sprintf(filepath, "%s", DIR_MC "mc.env");
        if(!file_exist(filepath)){
		printf("no file:%s\n",filepath);
		return -1;
	}
        if((mcEnv = loadData(filepath, sizeof(MC_Env))) == (void*)-1)
                return -1;

        //ɨ�� 24Сʱ֮ǰ days ��֮�ڵ�����
        days = atoi(argv[1])>=1?atoi(argv[1]):1;

	if ((b_fd = open(MY_BBS_HOME"/"BOARDS, O_RDONLY)) == -1){
			perror("open BOARDS");
			return -1;
	}

	//��ʼ��ͳ���б��ļ�
	fp = fopen(MARKED_FILE,"w");
	if(!fp){
		perror("open MARKED_FILE.");
		exit(-1);
	}
	fprintf(fp, "\n%s%d%s%s", "\
          \033[1;37m===========  \033[32m  ���",days,"���ڸ��������¼����߸�� \033[37m   ============ \n\n\
����         ����           ���±���                ����      "MONEY_NAME" ����ֵ","\n\
========== ========= ============================= ========== ====== ======\n\033[0m\
");

	fclose(fp);

	for (bnum = 0; read(b_fd, &bh, size) == size && bnum < MAXBOARD; bnum++) {
		if (!bh.filename[0])
			continue;
		if (!(bh.level & PERM_POSTMASK) && !(bh.level & PERM_NOZAP)	//������д���Ƶİ���
		    && bh.level != 0)
			continue;
		if ( (bh.flag & CLUB_FLAG) && !(strstr(bh.filename, "Digest" ) && strstr(bh.title, "����ժ" )) ) 		//�������ֲ����棬����������ժ����
			continue;
		bp = (bbsinfo.bcacheshm)->bcache[bnum];
		searchLastMark(&bh, days);	//��ʼɨ��
	}
	close(b_fd);
	unloadData(mcEnv, sizeof (MC_Env));
	for (i=0;i<n;i++) {
		sprintf(title,"%sϵͳ�����ĸ��",MY_BBS_NAME);
		sprintf(content,"�����%d���ڹ���%dƪ���±���ǡ�\nϵͳ���������%d %s,�Լ�%d ���⾭��ֵ(����ֵ)������\n\n��������º͸�ѷ�����������bbslists�档 \n���HTB��ʹ�ü�������Ϸ�����millionaire�档\n\n--\n",days,perarticle[i],percash[i],MONEY_NAME,perexp[i]);
		#ifdef DETAIL
		printf("%3d id:%s,article:%d,cash:%d,exp:%d\n",i,id[i],perarticle[i],percash[i],perexp[i]);
		#endif
		#ifndef DEBUG
		if (!inoverride("���֪ͨ", id[i], "rejects"))
			system_mail_buf(content, strlen(content), id[i], 
					title, "���֪ͨ");
		#endif
	}
	now_t=time(NULL);
	sprintf(day,"[%.10s] ϵͳ���Ÿ��һ��",ctime(&now_t));
	sprintf(buf,"\n�����ۼƷ��Ÿ�� \033[1;32m%d\033[m %s��\n\n",sum, MONEY_NAME);
	append2file(MARKED_FILE,buf);
#ifndef DEBUG
	postfile(MARKED_FILE, "deliver", "bbslists", day);
#else
	strcat(day,"(DEBUG)");
	postfile(MARKED_FILE, "deliver", "Code_Discuss", day);
#endif
	return 0;
}

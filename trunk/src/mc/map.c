#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mc.h"
#include "queue.h"

#define NODE_LEFT	0x1
#define NODE_RIGHT	0x2
#define NODE_UP		0x4
#define NODE_DOWN	0x8
#define NODE_QUEUED	0x10

#define InQueue(x)	(x & NODE_QUEUED)
#define NodeExits(x)	(x & 0xF)

typedef struct {
	short i, j;
} QueuePoint, *pQueuePoint;

// ��ppָ���ĵ�, ���ձ߽����Ʋ��������p
static void
gen_exit(McMap * mcMap, int i, int j, double rate, int *prev, int *trans,
	 int *next)
{
	unsigned short *p;

	p = &((mcMap->map[i][j]).exit);
	*p |= random() % 15;	//0-14, ������ֵΪ15��ȫ�����
	//�߽�����
	if (i == 0)
		*p &= ~NODE_UP;
	if (j == 0)
		*p &= ~NODE_LEFT;
	if (i == mcMap->row - 1)
		*p &= ~NODE_DOWN;
	if (j == mcMap->col - 1)
		*p &= ~NODE_RIGHT;

	//��ͼ�����/����/���ڵ�, 
	if (!*prev) {
		if (rate < 0.25) {
			mcMap->prev_x = i;
			mcMap->prev_y = j;
			*prev = 1;
		}
	}
	if (!*trans) {
		if (rate > 0.30 && rate < 0.55) {
			mcMap->trans_x = i;
			mcMap->trans_y = j;
			*trans = 1;
		}
	}
	if (!*next) {
		if (rate > 0.60 && rate < 0.85) {
			mcMap->next_x = i;
			mcMap->next_y = j;
			*next = 1;
		}
	}
	return;
}

// ��ӡ��ͼ
void
print_map(McMap * mcMap, int flag)
{
	int i, j;

	for (i = 0; i < mcMap->row; i++) {
		for (j = 0; j < mcMap->col; j++) {
			if (!NodeExits(mcMap->map[i][j].exit)) {
				prints("  ");
				continue;
			}
			if (flag && myInfo->pos_x == i && myInfo->pos_y == j)	//�ҵ�λ��
				prints("\033[1;33m+\033[0m ");
			else if (flag && i == mcMap->prev_x && j == mcMap->prev_y)	//���
				prints("\033[1;32m#\033[0m ");
			else if (flag && i == mcMap->trans_x && j == mcMap->trans_y)	//���͵�
				prints("\033[1;34m@\033[0m ");
			else if (flag && i == mcMap->next_x && j == mcMap->next_y)	//����
				prints("\033[1;35m*\033[0m ");
			else if (flag && myInfo->task.x == i && myInfo->task.y == j)	//����λ��
				prints("\033[5;31m+\033[0m ");
			else
				prints("+ ");
		}
		prints("\n");
	}
}

//����ѡ����Ⱦ��, Ϊ��֤��ͨ��, �õ��ԭ��ͨ������ѡȡ
static int
random_select(McMap * mcMap, int *x, int *y)
{
	int i, j, k, max, e;

	*x = random() % mcMap->row;
	*y = random() % mcMap->col;
	max = MAX(mcMap->row, mcMap->col);
	for (k = 1; k <= max; k++) {
		for (i = *x - k; i <= (*x + k); i++) {
			if (i < 0 || i >= mcMap->row)
				continue;
			for (j = *y - k; j <= (*y + k); j++) {
				if (j < 0 || j >= mcMap->col)
					continue;
				e = NodeExits(mcMap->map[i][j].exit);
				if (e == NODE_LEFT || e == NODE_RIGHT ||
				    e == NODE_UP || e == NODE_DOWN) {
					*x = i;
					*y = j;
					return 1;
				}
			}
		}
	}
	return 0;
}

//����B�Ƿ�Ӧ�������(��B����pqp�����ĵ�A��dir����)
static int
check_exit_node(pLinkList pList, McMap *mcMap, int i, int j, int dir)
{
	unsigned short *p1, *p2;
	QueuePoint *tmp;
	int exits[4][4] =
	    { {NODE_LEFT, NODE_RIGHT, 0, -1}, {NODE_RIGHT, NODE_LEFT, 0, +1},
	{NODE_UP, NODE_DOWN, -1, 0}, {NODE_DOWN, NODE_UP, +1, 0}
	};

	p1 = &(mcMap->map[i][j].exit);
	if (*p1 & exits[dir][0]) {	//�����A����
		p2 = &((mcMap->map[i + exits[dir][2]][j + exits[dir][3]]).exit);
		*p2 |= exits[dir][1];	//A���ڵ�ض���_A���ڵ�
		if (InQueue(*p2))	//���A���ڵ��Ѿ��ڶ�����
			return 0;
		tmp = calloc(1, sizeof (QueuePoint));
		if (tmp == NULL)
			return -1;
		tmp->i = i + exits[dir][2];
		tmp->j = j + exits[dir][3];
		insert_node(pList, tmp);	//�����ڵ�������
		*p2 |= NODE_QUEUED;	//��Ǵ���
	}
	return 0;
}

//������ͼ��ĳһ����Ķ���
static int
gen_item(McMap * mcMap, int i, int j)
{
	int r_npc, r_box;

	if ((mcMap->trans_x == i && mcMap->trans_y == j) || (mcMap->prev_x == i && mcMap->prev_y == j) || (mcMap->next_x == i && mcMap->next_y == j))	//����㲻������Ʒ
		return 0;

	r_npc = random() % (11 - mcMap->lvl);
	r_box = random() % (31 - mcMap->lvl);

	if (r_npc == 0)
		mcMap->map[i][j].npc_num = random() % mcMap->lvl + 1;
	if (r_box == 0)
		mcMap->map[i][j].box_num = 1;
	return 0;
}

//����һ�������ͼ, rate������ͼ����Ⱦ��. ��ͼ��СӦ��mcMap���趨
int
gen_map(McMap * mcMap, double rate)
{
	int i, j, retv = 0, count = 0;
	int prev = 0, trans = 0, next = 0;
	double sum;
	pLinkList pList;
	pQueuePoint pqp;

	pList = init_queue();
	if (pList == NULL)
		return -1;

	srandom(time(NULL));

	//���ѡ��һ����Ⱦ��
	i = random() % (mcMap->row - 2) + 1;
	j = random() % (mcMap->col - 2) + 1;

	sum = mcMap->row * mcMap->col + 0.1;
	while (count / sum < rate) {	//��֤һ������Ⱦ��
		pqp = calloc(1, sizeof (QueuePoint));
		if (pqp == NULL) {
			retv = -1;
			goto END;
		}
		pqp->i = i;
		pqp->j = j;
		insert_node(pList, pqp);	//������Ⱦ����
		mcMap->map[i][j].exit |= NODE_QUEUED;

		while (!is_empty(pList)) {
			pqp = read_first(pList);	//ȡ�����׵�

			//�����õ���������
			gen_exit(mcMap, pqp->i, pqp->j, count / (rate * sum),
				 &prev, &trans, &next);

			//�����õ�Ķ���, ncp, box, boss...
			gen_item(mcMap, pqp->i, pqp->j);

			//����õ�ĳ��ڵ�
			if (check_exit_node(pList, mcMap, pqp->i, pqp->j, 0) ||
			    check_exit_node(pList, mcMap, pqp->i, pqp->j, 1) ||
			    check_exit_node(pList, mcMap, pqp->i, pqp->j, 2) ||
			    check_exit_node(pList, mcMap, pqp->i, pqp->j, 3)) {
				retv = -1;
				goto END;
			}
			delete_node(pList, 0, 1);	//�Ӷ�����ɾ���õ�
			if ((++count) / sum > rate)	//��Ⱦ�ʴﵽ������
				goto END;
		}		//end of while(...)
		random_select(mcMap, &i, &j);	//���һ���㲻����Ⱦ��, ����ѡ��һ��
	}
      END:
	free_queue(pList, 1);	//�ͷŶ�����Դ
	return retv;
}

static int
open_box()
{
	randomevent();
	return 0;
}

static int
print_map_help()
{
	return 0;
}

/*
static int
kill_boss()
{
	unsigned int i, r, tmp;

	tmp = myInfo->health;
	myInfo->health = 1;

	move(6, 0);
	prints("һ����Ӱ������������, ����һ��: ԭ����lepton!");
	move(7, 0);
	prints("lepton�Դ����ȵ�˵: \"�벻���㻹�ܴ���������! �������ȳ���.\"");
	move(8, 0);
	if (askyn("������Ϊǿ��", YEA, NA) == YEA) {
		move(9, 0);
		prints("���þ�ȫ��,������ٶȳ���lepton!");
		refresh();
		sleep(1);
		move(10, 0);
		prints("һ���׹�����...������...");
		pressanykey();
		Q_Goodbye();
	}
	move(9, 0);
	if (askyn
	    ("lepton�޵�: \"����ò! �㲻���ҵĶ���, �����ȭ���������", NA,
	     NA)) {
		getrandomint(&r);
		if (r % 2) {
			move(10, 0);
			prints("����һ��, ҧ��������, lepton���˲�!");
			move(11, 0);
			prints
			    ("lepton��Ц��: \"����������...\" ˵������Ծ�����.");
			pressanykey();
			myInfo->health = tmp;
			for (i = 0; i < 3; i++)
				randomevent();
			clear();
			showAt(11, 22, "��������, �յóɾ�, Ӣ�ۼ���ǰ��...",
			       YEA);
			return 1;
		} else {
			move(10, 0);
			prints("����һ��, ҧ����ʯͷ, lepton���˲�!");
			move(11, 0);
			prints("�㻹û��Ӧ����, ֻ��һ���׹�����...������...");
			pressanykey();
			Q_Goodbye();
		}
	} else {
		move(10, 0);
		prints
		    ("lepton������Ц��:\"ԭ��Ҳ�ǵ�С��һ��, ������������!\"");
		move(11, 0);
		prints("���������, ֻ��һ���׹�����...������...");
		pressanykey();
		Q_Goodbye();
	}
	return 0;
}
*/

void
print_item(MapPoint * pmp, int *row)
{
	int r;

	move((*row)++, 0);
	prints("��һ������,һ�߽�����ɨ��������.");
	if (pmp->npc_num) {
		move((*row)++, 0);
		prints("%dֻ\033[1;31mħ��[k]\033[0m������צ����������!",
		       pmp->npc_num);
	}
	if (pmp->box_num) {
		move((*row)++, 0);
		prints("%d��\033[1;33m����[o]\033[0m��������ǰ.", pmp->box_num);
	}
	r = random() % 128;
	//if (r == 0 && 0)
	//	kill_boss();
	return;
}

int
map_valid(McMap * mcMap)
{
	if (mcMap->map == NULL)
		return 0;
	if (mcMap->flags & MAP_RESTART)
		return 0;
	if (mcMap->flags & MAP_LOCKED)
		return 0;
	return 1;
}

int
walk_map(McMap * mcMap)
{
	int i, j, ch, r, walk;
	unsigned short exits;

	i = mcMap->prev_x;
	j = mcMap->prev_y;

	for (walk = 0; map_valid(mcMap);) {
		r = 4;
		myInfo->pos_x = i;
		myInfo->pos_y = j;
		money_show_stat("а���ԭ");
		move(r++, 0);
		prints
		    ("�����Ǵ��̳��ⲻԶ��а���ԭ, ��Χ�Ĳ�����, ���ŵ�����ë���Ȼ.");
		if (myInfo->health < 1) {
			showAt(++r, 0, "��û���㹻������������...", YEA);
			continue;
		}
		move(t_lines - 2, 0);
		prints("[h ����]�������Եĳ����ǣ�");
		exits = mcMap->map[i][j].exit;
		if (exits & NODE_LEFT)
			prints("%s��", "��");
		if (exits & NODE_RIGHT)
			prints("%s��", "��");
		if (exits & NODE_UP)
			prints("%s��", "��");
		if (exits & NODE_DOWN)
			prints("%s��", "��");

		print_item(&mcMap->map[i][j], &r);

		ch = igetkey();
		if (ch == KEY_LEFT && (exits & NODE_LEFT)) {
			walk++;
			j--;
		} else if (ch == KEY_RIGHT && (exits & NODE_RIGHT)) {
			walk++;
			j++;
		} else if (ch == KEY_UP && (exits & NODE_UP)) {
			walk++;
			i--;
		} else if (ch == KEY_DOWN && (exits & NODE_DOWN)) {
			walk++;
			i++;
		} else if (ch == 'q' || ch == 'Q') {
			return 0;
		} else if (ch == 'h' || ch == 'H') {
			print_map_help();
		} else if (ch == KEY_TAB) {
			nomoney_show_stat("а���ԭ");
			move(4, 0);
			print_map(mcMap, 1);
			pressanykey();
		} else if (ch == 'k' || ch == 'K') {
			kill_npc();
		} else if (ch == 'o') {
			open_box();
		} else {
			showAt(t_lines - 1, 0, ">��Ҫ��ʲô?", YEA);
		}
		//��������״̬
		update_health();
		if (walk % 4 == 1) {
			walk++;
			myInfo->health--;
			limit_cpu();
		}
	}
	return 0;
}

int load_maps() {
	int shmid;
	key_t shmkey = 12345;
	size_t shmsize = sizeof(McMap) * 3 * 5;

	shmid = shmget(shmkey, shmsize, 0);
	if (shmid < 0) {
		shmid = shmget(shmkey, shmsize, IPC_CREAT | 0644);
		maps = shmat(shmid, NULL, 0);
		if((void*)maps == (void*)-1)
			return -1;
		memset(maps, 0, shmsize);
		maps[0].row = 18;
		maps[0].col = 32;
		maps[0].lvl= 3;
		return gen_map(&maps[0], 0.618);
	} else {
		maps = shmat(shmid, NULL, 0);
	}
	if((void*)maps == (void*)-1)
		return -1;
	return 0;
}


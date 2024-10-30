#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<ctime>
#include<string>
#include<iostream>
#include<stack>
#include<vector>
#include<algorithm>
#include<iomanip>
#include<conio.h>
using namespace std;

const string START = "START", PLACE = "PLACE", TURN = "TURN", END = "END";
const int BOARD_SIZE = 15, EMPTY = 0, BLACK = 1, WHITE = 2;
const int INF = (int)1e9;
const int _5 = (int)1e8, live_4 = (int)1e6, broken_4 = (int)1e5, live_3 = (int)1e3, broken_3 = (int)1e2, live_2 = 50, broken_2 = 10, live_1 = 2, broken_1 = 1, nothing = 0;
int max_depth = 10;
int attack = 1;
int my;
bool isdebug = 0;
int nowdis;
struct Command
{
	int x;
	int y;
	Command(int _x = 0, int _y = 0);
}nowmove;

struct mymove //存每一步的信息
{
	int value;
	Command pos;
	bool operator<(const mymove& a)const
	{
		return value > a.value;
	}
};

Command::Command(int _x, int _y)
{
	x = _x;
	y = _y;
}

int board[BOARD_SIZE][BOARD_SIZE] = { 0 };
int in_2[BOARD_SIZE][BOARD_SIZE] = { 0 };
int in_1[BOARD_SIZE][BOARD_SIZE] = { 0 };
int myFlag;
int enemyFlag;

bool judge(const int& x)//判断是否在范围内
{
	return x >= 0 && x < BOARD_SIZE;
}

void debug(const char* str)
{
	printf("DEBUG %s\n", str);
	fflush(stdout);
}

bool isInBound(int x, int y)
{
	return judge(x) && judge(y);
}

void place(struct Command cmd, const int& piece)
{
	board[cmd.x][cmd.y] = piece;
}

void initAI(int me)
{
	enemyFlag = 3 - me;
}

void start(int flag)
{
	memset(board, 0, sizeof(board));
	int middlePlace = BOARD_SIZE / 2;
	board[middlePlace - 1][middlePlace - 1] = WHITE;
	board[middlePlace][middlePlace] = WHITE;
	board[middlePlace - 1][middlePlace] = BLACK;
	board[middlePlace][middlePlace - 1] = BLACK;
	initAI(flag);
	if (max_depth & 1)attack = enemyFlag;
	else attack = myFlag;
}

bool okplace(const int& x, const int& y)//判断合法落子
{
	return isInBound(x, y) && !board[x][y];
}

bool okplace(const Command& c)//判断合法落子
{
	return isInBound(c.x, c.y) && !board[c.x][c.y];
}

bool compare(const int& x, const int& y, const int& piece)//比较某个位置的点和now，若超出边界则视为不相等,piece为4则判断是否有子
{
	if (!isInBound(x, y))return 0;
	if (piece == 4)return board[x][y];
	return board[x][y] == piece;
}

int judge_shape(const int& x, const int& y, const int& dx, const int& dy)//判断棋形
{
	int now = board[x][y];
	if (now == 0)return nothing;
	int num = 1;//记录与现在位置相同棋子的数量
	for (int i = 1; i <= 4; i++)
		if (compare(x + i * dx, y + i * dy, now))
			num++;
		else if (!okplace(x + i * dx, y + i * dy))
			break;
	//cout << "fuck";
	if (num == 5)
		return _5;

	if (num == 4)
	{
		if (okplace(x + 4 * dx, y + 4 * dy))
		{
			if (okplace(x - dx, y - dy))
				return live_4;//011110
			else
				return broken_4;//211110
		}
		else
		{
			if (compare(x + 4 * dx, y + 4 * dy, now))
				return (int)broken_4;//11011
			else if (okplace(x - dx, y - dy))
				return broken_4;//011112
			else return broken_3;//211112
		}

	}

	if (num == 3)
	{
		if (compare(x + dx, y + dy, now) && compare(x + 2 * dx, y + 2 * dy, now))
		{
			if (okplace(x - dx, y - dy) && okplace(x + 3 * dx, y + 3 * dy))
			{
				if (okplace(x - 2 * dx, y - 2 * dy) || okplace(x + 4 * dx, y + 4 * dy))
					return live_3;//2011100
				else
					return broken_3;//2011102
			}
			if ((okplace(x - dx, y - dy) && okplace(x - 2 * dx, y - 2 * dy)) ||
				(okplace(x + 3 * dx, y + 3 * dy) && okplace(x + 4 * dx, y + 4 * dy)))
				return broken_3;//211100
		}
		if (compare(x + 4 * dx, y + 4 * dy, now))
		{
			return broken_3;//10101
		}
		if (compare(x + 3 * dx, y + 3 * dy, now))
		{
			if (okplace(x + dx, y + dy) || okplace(x + 2 * dx, y + 2 * dy))
			{
				if (okplace(x - dx, y - dy) && okplace(x + 4 * dx, y + 4 * dy))
					return live_3 / 2;//011010
				if (okplace(x - dx, y - dy) || okplace(x + 4 * dx, y + 4 * dy))
					return broken_3;//211010
			}
			else
				return nothing;
		}
	}

	if (num == 2)
	{
		int lx = x, ly = y, rx = x, ry = y;

		while (1)
		{
			rx += dx, ry += dy;
			if (compare(rx, ry, now))break;
		}
		int len = 0;
		if (dx)
			len = (rx - lx) / dx + 1;
		else
			len = (ry - ly) / dy + 1;
		if (len > 3)return nothing;
		while (1)
		{
			if (okplace(lx - dx, ly - dy))
				lx -= dx, ly -= dy, len++;
			else
				break;
		}
		while (len < 6)
		{
			if (okplace(rx + dx, ry + dy))
				rx += dx, ry += dy, len++;
			else
				break;
		}
		if (len >= 6)
		{
			if ((lx == x && ly == y) || (rx == x && ry == y))
				return broken_2;
			else return live_2;
		}
		else if (len == 5)return broken_2;
		else return nothing;
	}

	if (num == 1)
	{
		int lx = x, ly = y, rx = x, ry = y;
		int len = 1;
		while (1)
		{
			if (okplace(lx - dx, ly - dy))
				lx -= dx, ly -= dy, len++;
			else
				break;
		}
		while (len < 6)
		{
			if (okplace(rx + dx, ry + dy))
				rx += dx, ry += dy, len++;
			else
				break;
		}
		if (len >= 6)
		{
			if ((lx == x && ly == y) || (rx == x && ry == y))
				return broken_1;
			else return live_1;
		}
		if (len == 5)return broken_1;
		return nothing;
	}
	return nothing;
}

int cal()
{
	int value = 0;
	int my_b4 = 0, my_l3 = 0;
	int enemy_b4 = 0, enemy_l3 = 0;
	int mag = 1;
	const int dx[4] = { 1,0,1,-1 }, dy[4] = { 0,1,1,1 };
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			int sign = (board[i][j] == myFlag ? 1 : -1);
			for (int k = 0; k < 4; k++)
			{
				int cur = judge_shape(i, j, dx[k], dy[k]);
				if (cur == broken_4)
				{
					if (sign == 1)my_b4++;
					else enemy_b4++;
				}
				else if (cur == live_3 || cur == live_3 / 2)
				{
					if (sign == 1)my_l3++;
					else enemy_l3++;
				}
				if (board[i][j] == attack && cur >= live_3 / 2)cur += cur / 10;
				value += sign * cur;
			}
		}
	if (my_b4 >= 2)value += live_4;
	if (enemy_b4 >= 2)value -= live_4;
	if (my_b4 >= 1 && my_l3 >= 1)value += live_4 / 3 * 2;
	if (enemy_b4 >= 1 && enemy_l3 >= 1)value -= live_4 / 3 * 2;
	if (my_l3 >= 2)value += live_4 / 3;
	if (enemy_l3 >= 2)value -= live_4 / 3;
	return value;
}

Command nextpos(Command& c)
{
	int x = c.x, y = c.y;
	y++;
	if (y == BOARD_SIZE)
	{
		y = 0;
		x++;
	}
	return { x,y };
}

int judge_dis(const int& x, const int& y)
{
	if (compare(x, y, 4))
		return 0;
	if (compare(x + 1, y, 4) ||
		compare(x, y - 1, 4) ||
		compare(x, y + 1, 4) ||
		compare(x - 1, y, 4) ||
		compare(x - 1, y - 1, 4) ||
		compare(x - 1, y + 1, 4) ||
		compare(x + 1, y + 1, 4) ||
		compare(x + 1, y - 1, 4))
		return 1;
	if (compare(x - 2, y, 4) ||
		compare(x, y - 2, 4) ||
		compare(x + 2, y, 4) ||
		compare(x, y + 2, 4) ||
		compare(x - 2, y - 2, 4) ||
		compare(x - 2, y + 2, 4) ||
		compare(x + 2, y + 2, 4) ||
		compare(x + 2, y - 2, 4))
		return 2;
	return 0;
}

void update(const int& x, const int& y)
{
	static const int dx[8] = { -1,-1,-1,0,0,1,1,1 }, dy[8] = { -1,0,1,-1,1,-1,0,1 };
	for (int i = 0; i < 8; i++)
	{
		int cx1 = x + dx[i], cy1 = y + dy[i], cx2 = x + 2 * dx[i], cy2 = y + 2 * dy[i];
		if (isInBound(cx1, cy1))in_1[cx1][cy1]++;
		if (isInBound(cx2, cy2))in_2[cx2][cy2]++;
	}

}

void downdate(const int& x, const int& y)
{
	static const int dx[8] = { -1,-1,-1,0,0,1,1,1 }, dy[8] = { -1,0,1,-1,1,-1,0,1 };
	for (int i = 0; i < 8; i++)
	{
		int cx1 = x + dx[i], cy1 = y + dy[i], cx2 = x + 2 * dx[i], cy2 = y + 2 * dy[i];
		if (isInBound(cx1, cy1))in_1[cx1][cy1]--;
		if (isInBound(cx2, cy2))in_2[cx2][cy2]--;
	}

}
void display()
{
	cout << "  ";
	for (int i = 0; i < BOARD_SIZE; i++)cout << setw(2) << i;
	cout << endl;
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		cout << setw(2) << i;
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if (board[i][j] == 0)cout << "┼ ";
			else if (board[i][j] == 1)cout << "○";
			else cout << "●";
		}
		cout << endl;
	}
	if (isdebug)cout << cal() << endl;
	cout << endl;
}
int alphabeta(int depth, int alpha, int beta, int piece)
{
	//cout << depth << endl;
	if (depth == max_depth)
		return cal();
	if (depth > 0 && depth % 2 == 0)
	{
		int val = cal();
		if (isdebug)
		{
			display();
			cout << val << endl;
			char c = _getch();
			while (c != '\r')c = _getch();
		}
		if (abs(val) < live_3 / 2 || abs(val) > _5 / 2)return val;
		else depth = 2;

	}
	vector<Command>choice;
	int sign = (piece == myFlag ? 1 : -1);
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++)
			if (in_1[i][j] && okplace(i, j))
			{
				choice.push_back({ i,j });
			}
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++)
			if (in_2[i][j] && !in_1[i][j] && okplace(i, j))
			{
				choice.push_back({ i,j });
			}
	bool foundpv = 0;
	for (vector<Command>::iterator it = choice.begin(); it != choice.end(); it++)if (okplace(*it))
	{
		place(*it, piece);//pos放置棋子，颜色由piece指定
		update(it->x, it->y);
		int val = 0;
		if (foundpv)
		{
			val = -alphabeta(depth + 1, -alpha - 1, -alpha, 3 - piece);
			if (val > alpha && val < beta)
				val = -alphabeta(depth + 1, -beta, -alpha, 3 - piece);
		}
		else
		{
			val = -alphabeta(depth + 1, -beta, -alpha, 3 - piece);
		}
		downdate(it->x, it->y);
		place(*it, 0);//撤销棋步
		if (val >= beta)//大于beta则满足剪枝条件
			return beta;
		if (val > alpha)//大于alpha则更新alpha
		{
			alpha = val;
			if (nowdis == 0 || (depth == max_depth && judge_dis(it->x, it->y) < nowdis))nowmove = *it;
			foundpv = 1;
		}
	}
	return alpha;
}

void turn()
{
	int alpha = -INF, beta = INF;
	int cur = alphabeta(0, alpha, beta, myFlag);
	cout << nowmove.x << " " << nowmove.y << endl;
	place(nowmove, myFlag);
}



void end(int x)
{
	exit(0);
}
void wait_for_enter()
{
	char c = _getch();
	while (c != '\r')
		c = _getch();
}
bool judge_over()
{
	const int dx[4] = { -1,0,1,1 }, dy[4] = { 1,1,1,0 };
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++)
			for (int k = 0; k < 4; k++)
			{
				int val = judge_shape(i, j, dx[k], dy[k]);
				if (val == _5)return 1;
			}
	return 0;
}
void loop()
{
	string tag;
	Command command;
	cout << "请输入难度(简单版运行速度快，但其实困难也难不倒哪里去，只是相对来说)：1、简单；2、困难" << endl;
	int x;
	cin >> x;
	if (x == 1)max_depth = 2;
	else max_depth = 4;
	cout << "请选择：1、黑棋（先手）；2、白棋（后手）" << endl;
	cin >> myFlag;
	myFlag = 3 - myFlag;
	start(myFlag);
	if (myFlag == BLACK)
		turn();
	display();
	while (1)
	{
		cout << "请输入你要下的位置(x行y列)：" << endl;
		int x, y;
		cin >> x >> y;
		while (!judge(x) || !judge(y))
		{
			cout << "输入错误请重新输入" << endl;
			cin >> x >> y;
		}
		place({ x,y }, enemyFlag);
		system("cls");
		display();
		if (judge_over())
		{
			cout << "你赢了" << endl;
			break;
		}
		cout << "请输入回车让对手下..." << endl;
		wait_for_enter();
		cout << "请稍等，正在计算（可能需要几十秒）" << endl;
		attack = 0;
		if (cal() >= 0)attack = BLACK;
		else attack = WHITE;
		memset(in_1, 0, sizeof(0));
		memset(in_2, 0, sizeof(0));
		for (int i = 0; i < BOARD_SIZE; i++)
			for (int j = 0; j < BOARD_SIZE; j++)
				if (judge_dis(i, j) == 1)
				{
					in_1[i][j]++; 
				}
				else if (judge_dis(i, j) == 2)
				{
					in_2[i][j]++; 
				}
		turn();
		system("cls");
		display();
		cout << "对方下了(" << nowmove.x << "," << nowmove.y << ")" << endl;
		if (judge_over())
		{
			cout << "你输了" << endl;
			wait_for_enter();
			break;
		}
	}
}
int main(int argc, char* argv[])
{
	loop();
	return 0;
}
/*
START 2
TURN
*/
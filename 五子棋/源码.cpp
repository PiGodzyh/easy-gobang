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
#include<chrono>
#include<conio.h>
using namespace std;

const string START = "START", PLACE = "PLACE", TURN = "TURN", END = "END";
const int BOARD_SIZE = 12, EMPTY = 0, BLACK = 1, WHITE = 2;
const int INF = (int)1e9;
const int _5 = (int)1e8, live_4 = (int)1e6, broken_4 = (int)1e5, live_3 = (int)1e3, broken_3 = (int)1e2, live_2 = 50, broken_2 = 10, live_1 = 2, broken_1 = 1, nothing = 0;
int max_depth = 6;
int my;
bool isdebug = 0;
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

bool isInBound(int x, int y)//判断是否棋盘内
{
	return judge(x) && judge(y);
}

void place(struct Command cmd, const int& piece)//下棋
{
	board[cmd.x][cmd.y] = piece;
}

void initAI(int me)//赋初值
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
}

bool okplace(const int& x, const int& y)//判断合法落子
{
	return isInBound(x, y) && !board[x][y];//在棋盘内且没有子
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
			else return live_2 << 1;//211112
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

int cal()//计算整个棋盘的分值
{
	int value = 0;
	int my_b4 = 0, my_l3 = 0;
	int enemy_b4 = 0, enemy_l3 = 0;
	const int dx[4] = { 1,0,1,-1 }, dy[4] = { 0,1,1,1 };//四个方向
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
				value += sign * cur;
			}
		}
	//双冲四
	if (my_b4 >= 2)value += live_4;
	if (enemy_b4 >= 2)value -= live_4;
	//冲四活三
	if (my_b4 >= 1 && my_l3 >= 1)value += live_4;
	if (enemy_b4 >= 1 && enemy_l3 >= 1)value -= live_4;
	//双三
	if (my_l3 >= 2)value += live_4 / 2;
	if (enemy_l3 >= 2)value -= live_4 / 2;
	return value;
}

int cal(const int& x, const int& y)//计算某个点每个方向上的分值
{
	int val = 0;
	int my_b4 = 0, enemy_b4 = 0;
	int my_l3 = 0, enemy_l3 = 0;
	for (int i = 0, j = y; i < BOARD_SIZE; i++)if (compare(i, j, 4))
	{
		int sign = (compare(i, j, myFlag) ? 1 : -1);
		int cur = judge_shape(i, j, 1, 0);
		val += sign * cur;
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
	}
	for (int i = x, j = 0; j < BOARD_SIZE; j++)if (compare(i, j, 4))
	{
		int sign = (compare(i, j, myFlag) ? 1 : -1);
		int cur = judge_shape(i, j, 0, 1);
		val += sign * cur;
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
	}
	int d = y - x;
	for (int i = max(0, -d), j = d + i; i < min(BOARD_SIZE, BOARD_SIZE - d); i++, j++)if (compare(i, j, 4))
	{
		int sign = (compare(i, j, myFlag) ? 1 : -1);
		int cur = judge_shape(i, j, 1, 1);
		val += sign * cur;
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
	}
	int s = y + x;
	for (int i = min(s, BOARD_SIZE - 1), j = s - i; i >= max(0, s - BOARD_SIZE + 1); i--, j++)if (compare(i, j, 4))
	{
		int sign = (compare(i, j, myFlag) ? 1 : -1);
		int cur = judge_shape(i, j, -1, 1);
		val += sign * cur;
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
	}
	//双冲四
	if (my_b4 >= 2)val += live_4;
	if (enemy_b4 >= 2)val -= live_4;
	//冲四活三
	if (my_b4 >= 1 && my_l3 >= 1)val += live_4;
	if (enemy_b4 >= 1 && enemy_l3 >= 1)val -= live_4;
	//双三
	if (my_l3 >= 2)val += live_4 / 2;
	if (enemy_l3 >= 2)val -= live_4 / 2;
	return val;
}

Command nextpos(Command& c)//寻找下一个位置
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

int judge_dis(const int& x, const int& y)//判断多少距离之内有棋子
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

bool judge_5(const int& x, const int& y)//判断是否5颗子了
{
	int now = board[x][y];
	const int dx[4] = { 1,0,1,-1 }, dy[4] = { 0,1,1,1 };
	bool jud = 0;;
	for (int i = 0; i < 4; i++)
	{
		int len = 1;
		int lx = x, ly = y;
		while (compare(lx + dx[i], ly + dy[i], now))
			lx += dx[i], ly += dy[i], len++;
		int rx = x, ry = y;
		while (compare(rx - dx[i], ry - dy[i], now))
			rx -= dx[i], ry -= dy[i], len++;
		if (len >= 5)
			return 1;
	}
	return 0;
}

void update(const int& x, const int& y)//更新周围两格内的子
{
	static const int dx[8] = { -1,-1,-1,0,0,1,1,1 }, dy[8] = { -1,0,1,-1,1,-1,0,1 };
	for (int i = 0; i < 8; i++)
	{
		int cx1 = x + dx[i], cy1 = y + dy[i], cx2 = x + 2 * dx[i], cy2 = y + 2 * dy[i];
		if (isInBound(cx1, cy1))in_2[cx1][cy1]++;
		if (isInBound(cx2, cy2))in_2[cx2][cy2]++;
	}

}

void downdate(const int& x, const int& y)//撤销更新周围两格内的子
{
	static const int dx[8] = { -1,-1,-1,0,0,1,1,1 }, dy[8] = { -1,0,1,-1,1,-1,0,1 };
	for (int i = 0; i < 8; i++)
	{
		int cx1 = x + dx[i], cy1 = y + dy[i], cx2 = x + 2 * dx[i], cy2 = y + 2 * dy[i];
		if (isInBound(cx1, cy1))in_2[cx1][cy1]--;
		if (isInBound(cx2, cy2))in_2[cx2][cy2]--;
	}

}

void display()//无敌展示棋盘
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

int alphabeta(int depth, int alpha, int beta, int piece, int nowMaxDepth)//搜索
{
	//cout << depth << " ^ ";
	if (depth == max_depth || depth == nowMaxDepth)
		return cal();
	if (0)
	{
		int val = cal();
		if (0)
		{
			display();
			cout << val << endl;
			char c = _getch();
			while (c != '\r')c = _getch();
		}
		if (abs(val) < live_3 * 1.5)
		{
			if (depth > 1 && depth % 2 == 0)return val;
		}
		else nowMaxDepth = ((nowMaxDepth >> 1) << 1) + 4;
	}
	vector<Command>choice1;
	vector<mymove>choice2;
	//先找出来两格内的位置
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++)
			if (in_2[i][j] && okplace(i, j))
			{
				choice1.push_back({ i,j });
			}
	//在两格之内判断每个点分值并排序
	for (int k = 0; k < choice1.size(); k++)
	{
		int x = choice1[k].x, y = choice1[k].y;//cout << "ss";
		int preVal = cal(x, y), myVal, enemyVal;
		place({ x,y }, myFlag);
		myVal = cal(x, y);
		place({ x,y }, enemyFlag);
		enemyVal = cal(x, y);
		choice2.push_back({ max(abs(myVal - preVal), abs(enemyVal - preVal)), choice1[k] });
		place({ x,y }, 0);
	}

	sort(choice2.begin(), choice2.end());
	//debug
	if (0)
	{
		cout << depth << "#" << endl;
		for (int i = 0; i < choice2.size(); i++)
			cout << choice2[i].value << " " << choice2[i].pos.x << " " << choice2[i].pos.y << endl;
	}
	//如果最大的点的分值都很低就剪枝
	if (choice2.front().value < live_3 * 1.5)
	{
		if (depth > 1 && (depth & 1) == 0)
		{
			return cal();
		}
	}
	else
	{
		nowMaxDepth = ((nowMaxDepth >> 1) << 1) + 4;
	}
	if (depth == 0)
		nowmove = choice2.front().pos;
	bool foundpv = 0;
	for (int i = 0; i < choice2.size(); i++)if (okplace(choice2[i].pos))
	{
		Command ps = choice2[i].pos;
		if (i > (choice2.front().value > _5 / 2 ? 0 : 5) && choice2[i].value < choice2.front().value / 5)break;
		place(ps, piece);//pos放置棋子，颜色由piece指定
		if (judge_5(ps.x, ps.y))//若五子则可提前返回
		{
			if (depth == 0)nowmove = ps;//若是第一步则直接获得答案
			int val = (depth & 1 ? -1 : 1) * cal();
			place(ps, 0);//仍要撤销棋步
			return val;
		}
		update(ps.x, ps.y);
		int val = 0;
		if (foundpv)
		{
			//PVS算法优化
			val = -alphabeta(depth + 1, -alpha - 1, -alpha, 3 - piece, nowMaxDepth);
			if (val > alpha && val < beta)
				val = -alphabeta(depth + 1, -beta, -alpha, 3 - piece, nowMaxDepth);
		}
		else
		{
			val = -alphabeta(depth + 1, -beta, -alpha, 3 - piece, nowMaxDepth);
		}
		downdate(ps.x, ps.y);
		place(ps, 0);//撤销棋步
		if (val >= beta)//大于beta则满足剪枝条件
			return beta;
		if (val > alpha)//大于alpha则更新alpha
		{
			alpha = val;
			if (depth == 0)nowmove = ps;
			foundpv = 1;
		}
	}
	return alpha;
}

void turn()
{
	int alpha = -INF, beta = INF;
	int cur = alphabeta(0, alpha, beta, myFlag, 2);
	cout << nowmove.x << " " << nowmove.y << endl;
	place(nowmove, myFlag);
}



void end(int x)
{
	exit(0);
}

void loop()
{
	string tag;
	Command command;
	int status;
	while (1)
	{
		tag = "";
		cin >> tag;
		if (tag == START)
		{
			cin >> myFlag;
			start(myFlag);
			printf("OK\n");
			fflush(stdout);
		}
		else if (tag == PLACE)
		{
			cin >> command.x >> command.y;
			place(command, enemyFlag);
		}
		else if (tag == TURN)
		{
			int val = cal();
			int tot = 0;
			memset(in_2, 0, sizeof(0));
			for (int i = 0; i < BOARD_SIZE; i++)
				for (int j = 0; j < BOARD_SIZE; j++)
					if (judge_dis(i, j) > 0)
					{
						in_2[i][j]++;
						tot++;
						nowmove = { i,j };
					}
			if (tot == 1)
			{
				cout << nowmove.x << " " << nowmove.y << endl;
				break;
			}
			if (0)
			{
				auto beforeTime = chrono::steady_clock::now();
				turn();
				auto afterTime = chrono::steady_clock::now();
				cout << "总耗时:" << endl;
				double duration_second = chrono::duration<double>(afterTime - beforeTime).count();
				cout << duration_second << "秒" << endl;
			}
			else
				turn();

		}
		else if (tag == END)
		{
			cin >> status;
			end(status);
		}
		//display();
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
#include<bits/stdc++.h>
using namespace std;

const int DICES_NUM = 5;
const int DICE_FACE = 6;
const int BONUS_TARGET = 63;
const int BONUS_SCORE = 35;
const int ROUND_NUM = 12;
const int SHUFFLE_CHANCE = 3;
enum score {
	one, two, three, four, five, six,
	all, same, fullhouse, small, large, yahtzee
};
string score_type[ROUND_NUM] = {
	"一点", "二点", "三点", "四点", "五点", "六点",
	"全选", "四条", "葫芦", "小顺", "大顺", "快艇"
};

class Dice {
public:
	Dice() {
		selected = false;
		shuffle();
	}
	int shuffle() {
		return point = rand()%DICE_FACE+1;
	}
	int getPoint() {
		return point;
	}
	bool isSelected() {
		return selected;
	}
	void select() {
		selected = true;
	}
	void deselect() {
		selected = false;
	}
private:
	int point;
	bool selected;
};

class Determiner {
public:
	static void getScores(Dice dices[], int scores[]) {
		int sum = 0; // the total sum of the points
		vector<int> points; // the occurred points
		vector<int> times(DICE_FACE); // the repeating times of the points
		for(int i = 0; i < DICES_NUM; ++i) {
			int point = dices[i].getPoint();
			points.push_back(point);
			++times[point-1];
			// calculate the 1~6 point scores
			scores[point-1] += point;
			sum += point;
		}

		// calculate all select score
		scores[all] = sum;

		// calculate the small and large straight
		// sort the points and erase the duplicated ones
		sort(points.begin(), points.end() );
		points.erase(unique(points.begin(), points.end()), points.end() );

		int max_continuation = 0, cnt = 1;
		for(int i = 1; i < (int)points.size(); ++i) {
			if(points[i] == points[i-1]+1) ++cnt;
			else max_continuation = max(max_continuation, cnt), cnt = 1;
		}
		max_continuation = max(max_continuation, cnt);
		if(max_continuation >= 4) { 
			scores[small] = 15; // the small straight
			if(max_continuation == 5)
				scores[large] = 30; // the large straight
		}

		// calculate the yahtzee and the same of four
		int max_repeated = *max_element(times.begin(), times.end() );
		if(max_repeated >= 4) {
			scores[same] = sum; // the same of four
			if(max_repeated == 5)
				scores[yahtzee] = 50; // the yahtzee
		}

		// calculate the full house
		// the index which a point occurs two or three times
		int ind3 = -1, ind2 = -1;
		for(int i = 0; i < (int)times.size(); ++i) {
			if(times[i] == 3) ind3 = i;
			else if(times[i] == 2) ind2 = i;
		}
		if(ind3 != -1 && ind2 != -1)
			scores[fullhouse] = sum;
	}
};

class Player {
public:
	Player() {
		totalScore = 0;
		bonusScore = 0;
		memset(scores, 0, sizeof(scores) );
		memset(pendings, 0, sizeof(pendings) );
		memset(selected, false, sizeof(selected) );
	}
	void setNickname(string _nickname) {
		nickname = _nickname;
	}
	string getNickname() {
		return nickname;
	}
	int getTotalScore() {
		return totalScore;
	}
	void shuffle() {
		for(int i = 0; i < DICES_NUM; ++i)
			if(!dices[i].isSelected() )
				dices[i].shuffle();
	}
	void select(int num) {
		dices[num].select();
	}
	void selectAll() {
		for(int i = 0; i < DICES_NUM; ++i)
			dices[i].select();
	}
	void deselectAll() {
		for(int i = 0; i < DICES_NUM; ++i)
			dices[i].deselect();
	}
	void printDice() {
		cout << "骰子状态：" << endl;
		cout << "序号  ";
		for(int i = 1; i <= DICES_NUM; ++i)
			cout << i << "  ";
		cout << endl << "点数  ";
		for(int i = 0; i < DICES_NUM; ++i)
			cout << dices[i].getPoint() << "  ";
		cout << endl << endl;
	}
	// in the player's turn, output +xx to show the score going to get
	void printInfo(bool turn) {
		cout << nickname << "的计分板" << endl;

		for(int i = 0; i < DICE_FACE; ++i) {
			cout << i+1 << ". ";
			cout << score_type[i] << ": ";
			if(selected[i]) cout << scores[i];
			else cout << "-";

			if(turn && !selected[i])
				cout << "  +" << pendings[i];
			cout << endl;
		}

		cout << "小计：" << bonusScore << "/" << BONUS_TARGET << endl;
		cout << "奖励分35：";
		if(bonusScore >= BONUS_TARGET)
			cout << "+35" << endl << endl;
		else cout << "0" << endl << endl;

		for(int i = DICE_FACE; i < ROUND_NUM; ++i) {
			cout << i+1 << ".";
			if(i+1 < 10) cout << " ";
			cout << score_type[i] << ": ";
			if(selected[i]) cout << scores[i];
			else cout << "-";

			if(turn && !selected[i])
				cout << "  +" << pendings[i];
			cout << endl;
		}
		cout << endl << "总分：" << totalScore << endl << endl << endl;
	}
	void selectDices(string diceIndexes) {
		// empty input means select all the dices
		if(diceIndexes == "") {
			selectAll();
			return;
		}
		deselectAll();
		stringstream ss(diceIndexes);
		string buf;
		while(ss >> buf) {
			int index = stoi(buf);
			// -1 means deselect all the dices
			if(index == -1) {
				deselectAll();
				break;
			}
			if(index >= 1 && index <= 5)
				select(index-1);
		}
	}
	void clearPendings() {
		memset(pendings, 0, sizeof(pendings) );
	}
	void getScores() {
		clearPendings();
		Determiner::getScores(dices, pendings);
	}
	int fillBlank(int scoreType) {
		// illegal input
		if(scoreType < 0 || scoreType >= ROUND_NUM)
			return -1;
		// can't select the type that has already selected
		if(selected[scoreType])
			return -2;

		int toAdd = pendings[scoreType];
		scores[scoreType] = toAdd;
		totalScore += toAdd;
		selected[scoreType] = true;

		if(scoreType <= six) {
			bonusScore += toAdd;
			// the first time when the bonus score reach the target
			// reward the bonus
			if(bonusScore >= BONUS_TARGET && !rewarded) {
				totalScore += BONUS_SCORE;
				rewarded = true;
			}
		}
		return 0;
	}

private:
	string nickname;
	Dice dices[DICES_NUM];
	int scores[ROUND_NUM];
	int pendings[ROUND_NUM];
	bool selected[ROUND_NUM];
	int totalScore;
	int bonusScore;
	bool rewarded;

};

class Game {
public:
	Game() {
		system("cls");
		setNicknames();
		for(int i = 0; i < ROUND_NUM; ++i) {
			round(A, (i<<1));
			round(B, (i<<1)+1);
		}
		printResult();
	}
	void setNicknames() {
		string nickname;
		cout << "请输入玩家A的昵称：";
		cin >> nickname;
		A.setNickname(nickname);
		cout << "请输入玩家B的昵称：";
		cin >> nickname;
		B.setNickname(nickname);
		// deal the enter input
		getchar();
	}
	void printStatus(bool isBsTurn) {
		system("cls");
		A.printInfo(!isBsTurn);
		B.printInfo(isBsTurn);
	}
	void selectDices(Player &p, string diceIndexes) {
		p.deselectAll();
		stringstream ss(diceIndexes);
		string buf;
		while(ss >> buf) {
			int index = stoi(buf);
			if(index >= 1 && index <= 5)
				p.select(index-1);
		}
	}
	// cnt is even when its A's turn, otherwise its B's
	void round(Player &p, int cnt) {
		string buf;
		p.clearPendings();
		printStatus(cnt%2);
		p.deselectAll();
		for(int i = SHUFFLE_CHANCE; i >= 1; --i) {
			printStatus(cnt%2);
			// show the dices exclude before the first shuffle
			if(i == SHUFFLE_CHANCE)
				cout << p.getNickname() << "的回合" << endl;
			else 
				p.printDice();

			cout << "按下回车掷骰子，剩余" << i << "次机会...";
			getline(cin, buf);
			p.shuffle();
			p.getScores();
			printStatus(cnt%2);
			p.printDice();
			if(i > 1) {
				cout << "还剩" << i-1 << "次" << "重掷机会" << endl;
				cout << "选择要保留骰子的序号（空输入：全部保留并直接进入填表阶段；-1：全部重掷）：";
				getline(cin, buf);
				p.selectDices(buf);
				// empty input means remain the all, and turn to the next stage
				if(buf == "")
					break;
			}
			else {
				cout << "次数已用尽，进入填表阶段";
			}
		}
		int scoreType;
		cout << "请输入将填项目的序号：";
		while(true) {
			cin >> scoreType;
			int status = p.fillBlank(scoreType-1);
			if(status == -1)
				cout << "输入的序号非法，请重试：";
			else if(status == -2)
				cout << "所选项目已被填写过，请重试：";
			else {
				printStatus(cnt%2);
				p.printDice();
				cout << "成功填写，按下回车交换回合...";
				getline(cin, buf);
				getline(cin, buf);
				break;
			}
		}
	}
	void printResult() {
		int aScore = A.getTotalScore();
		int bScore = B.getTotalScore();
		cout << endl;
		cout << A.getNickname() << ": " << aScore << "分" << endl;
		cout << B.getNickname() << ": " << bScore << "分" << endl;
		if(aScore > bScore)
			cout << "恭喜" << A.getNickname() << "赢得比赛！" << endl << endl;
		else if(aScore < bScore)
			cout << "恭喜" << B.getNickname() << "赢得比赛！" << endl << endl;
		else
			cout << "平局！" << endl << endl;
	}

private:
	Player A, B;

};

int main() {
	srand(time(NULL));
	while(true) {
		Game game;
		cout << "是否进行新的一轮游戏？ (y/n)" << endl;
		string buf;
		cin >> buf;
		if(buf != "y")
			break;
	}
	return 0;
}
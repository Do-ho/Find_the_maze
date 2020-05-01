#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <stack>

using namespace std;

typedef struct location{
	int x;
	int y;
};

class find_maze {
	int rows = 0; //행의 길이
	int cols = 0; //열의 길이
	int mouseEnergy = 0; //쥐의 에너지
	double mouseMana = 0.0; //쥐의 마나
	location mousePos = { 0, 1 }; //쥐의 처음 좌표
	location exitPos; //탈출 좌표
	int prevMouseDirection; //쥐의 전 방향
	int loadCount = 0; //현재 시점 갈림길 개수
	int folk_count = 0;
	double move_table[4] = { 0, 0, 0, 0 }; // 상 하 좌 우
	char *filename;
	string filepath; //결과 저장 경로
	char **maze; //미로 맵
	char **check;
	bool start = false; //첫 좌표를 내딛었는가
	stack<location> short_load; //최소 경로를 저장할 스택
	stack<location> teleport_Pos; //텔레포트 가능한 곳을 저장하는 스택

public:
	find_maze(string inputfile) {
		filename = new char[inputfile.size()+1];
		copy(inputfile.begin(), inputfile.end(), filename);
		filename[inputfile.size()] = '\0'; 
		string filename(filename);
		filepath = "result_" + filename; //결과 저장 경로 지정

		set_rowcol();

		mouseEnergy = rows*cols * 2; //쥐의 에너지

		load();

		teleport_Pos.push({ 0,0 });
	}

	void maze_print() {
		system("cls");
		cout << "--------------------------------------------------------" << endl;
		cout << "\t\tWelcome maze world!!!!" << endl;
		cout << "--------------------------------------------------------" << endl;
		cout << "행 길이 : " << rows << " 열 길이 : " << cols <<" 에너지 : "<<mouseEnergy << " 마나 : "<< mouseMana << endl;
		cout << "--------------------------------------------------------" << endl;
		cout << "탈출 좌표 : (" << exitPos.x << "," << exitPos.y << ")" << endl;
		cout << "쥐 좌표 : (" << mousePos.x << "," << mousePos.y << ")" << endl;
		cout << "텔레포트 좌표 : (" << teleport_Pos.top().x << "," << teleport_Pos.top().y << ")" << endl;
		cout << "--------------------------------------------------------" << endl;

		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++) {
				if (maze[i][j] == '0') printf("  ");
				else if (maze[i][j] == 'M') printf("＠");
				else if (maze[i][j] == 'S') printf("※");
				else printf("■");
				//미로 출력
			}
			printf("\n");
		}
	}

	void set_rowcol() {
		ifstream fp_maze;
		fp_maze.open(filename);

		string line;

		while (getline(fp_maze, line)) {
			rows++;
		}

		cols = line.length() / 2 + 1;

		fp_maze.close();

		maze = new char *[rows];
		check = new char *[rows];

		for (int i = 0; i < rows; i++)
		{
			maze[i] = new char[cols];
			check[i] = new char[cols];
		}
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				check[i][j] = 0;
			}
		}
	}

	void load()
	{
		ifstream fp_maze;
		fp_maze.open(filename);

		if (fp_maze.is_open()) {
			cout << "열렸습니다." << endl;
		}
		else {
			cout << "열리지 않았습니다." << endl;
		}

		int count_rows = 0;

		string line;

		char * get_line = new char[cols];
		while (getline(fp_maze, line)) {
			int count_cols = 0;
			strcpy(get_line, line.c_str());

			char * line_tok = strtok(get_line, " ");

			while (line_tok != nullptr) {
				maze[count_rows][count_cols++] = *line_tok; 
				line_tok = strtok(NULL, " ");
			}
			count_rows++;
		}
	}

	void mouse_move() {
		start = true;
		loadCount=0;
		mouseEnergy--;
		mouseMana += 0.1;
		maze[mousePos.x][mousePos.y] = '0';

		switch (move_table_set()) {
		case 0: //상
			if (check[mousePos.x-1][mousePos.y] != 0) {
				short_load.pop();
			}
			else short_load.push(mousePos);
			maze[--mousePos.x][mousePos.y] = 'M';
			prevMouseDirection = 1;
			break;
		case 1: //하
			if (check[mousePos.x + 1][mousePos.y] != 0) {
				short_load.pop();
			}
			else short_load.push(mousePos);
			maze[++mousePos.x][mousePos.y] = 'M';
			prevMouseDirection = 0;
			break;
		case 2: //좌
			if (check[mousePos.x][mousePos.y - 1] != 0) {
				short_load.pop();
			}
			else short_load.push(mousePos);
			maze[mousePos.x][--mousePos.y] = 'M';
			prevMouseDirection = 3;
			break;
		case 3: //우
			if (check[mousePos.x][mousePos.y + 1] != 0) {
				short_load.pop();
			}
			else short_load.push(mousePos);
			maze[mousePos.x][++mousePos.y] = 'M';
			prevMouseDirection = 2;
			break;
		default:
			break;
		}

		loadCount = 0;
		cal_load_count();

		if (loadCount >= 3) { //갈림길이면
			if (check[mousePos.x][mousePos.y] / 10 == loadCount-1) { //갈림길으로 나눠진 길을 다 방문했다면
				teleport_Pos.pop();
				teleport2();
			}
			else if (check[mousePos.x][mousePos.y] / 10 == 1 && loadCount == 3) {
				teleport_Pos.pop();
				if (teleport_Pos.top().x != 0 || teleport_Pos.top().y != 0) teleport_Pos.push(mousePos);
			}
			else if (check[mousePos.x][mousePos.y] == 0) {
				teleport_Pos.push(mousePos);
			}
		}
	}

	int move_table_set() {
		cal_load_count();
		if (loadCount == 1) check[mousePos.x][mousePos.y] += 20;
		else check[mousePos.x][mousePos.y] += 10;
		
		if ((mousePos.x - 1) >= 0 && maze[mousePos.x - 1][mousePos.y] != '1') {
			move_table[0] = Distance(mousePos.x - 1, mousePos.y, exitPos.x, exitPos.y) + check[mousePos.x - 1][mousePos.y];
		}
		else move_table[0] = rows*cols;
		//상
		if ((mousePos.x + 1) <= rows && maze[mousePos.x+1][mousePos.y] != '1') {
			move_table[1] = Distance(mousePos.x + 1, mousePos.y, exitPos.x, exitPos.y) + check[mousePos.x + 1][mousePos.y];
		}
		else move_table[1] = rows*cols;
		//하
		if ((mousePos.y - 1)>=0 && maze[mousePos.x][mousePos.y - 1] != '1') {
			move_table[2] = Distance(mousePos.x , mousePos.y - 1, exitPos.x, exitPos.y) + check[mousePos.x][mousePos.y - 1];
		}
		else move_table[2] = rows*cols;
		//좌
		if ((mousePos.y + 1) <= cols && maze[mousePos.x][mousePos.y+1] != '1') {
			move_table[3] = Distance(mousePos.x, mousePos.y + 1, exitPos.x, exitPos.y) + check[mousePos.x][mousePos.y + 1];
		}
		else move_table[3] = rows*cols;
		//우
		
		int near_direction_index = 0;

		for (int i = 0; i < 4; i++)
		{
			if (is_fork(i)) near_direction_index = i;
		}

		for (int i = 0; i < 4; i++)
		{
			if (move_table[i] != rows*cols){
				if(move_table[near_direction_index] >= move_table[i] && prevMouseDirection !=i) { //벽이 막혀있지 않고 가중치가 저장된 값보다 작을 때
					near_direction_index = i;
				}
			}
		}
		if (mousePos.x != 0 && mousePos.y != 1 && loadCount == 1) near_direction_index = prevMouseDirection; //사방이 벽일 때 전에꺼로 움직여
		
		return near_direction_index;
	}

	double Distance(int x1, int y1, int x2, int y2) {
		double distance;
		//피타고라스 정리
		
		distance = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));

		return distance;
	}

	bool is_fork(int i) {
		if(mousePos.x - 1 > 0 && i==0)
			return cal_folk_count(mousePos.x - 1, mousePos.y) >= 3;
		else if (mousePos.x + 1 < rows && i == 1) {
			return cal_folk_count(mousePos.x + 1, mousePos.y) >= 3;
		}
		else if (mousePos.y - 1 > 0 && i == 2) {
			return cal_folk_count(mousePos.x, mousePos.y - 1) >= 3;
		}
		else if (mousePos.y + 1 < cols && i == 3) {
			return cal_folk_count(mousePos.x, mousePos.y + 1) >= 3;
		}
		return false;
	}


	void cal_load_count() {//뚫려있는 길의 개수
		if ((mousePos.x - 1) > 0 && maze[mousePos.x - 1][mousePos.y] != '1') {
			loadCount++;
		}
		if ((mousePos.x + 1) < rows && maze[mousePos.x + 1][mousePos.y] != '1') {
			loadCount++;
		}
		if ((mousePos.y - 1) > 0 && maze[mousePos.x][mousePos.y - 1] != '1') {
			loadCount++;
		}
		if ((mousePos.y + 1) < cols && maze[mousePos.x][mousePos.y + 1] != '1') {
			loadCount++;
		}
		if (mousePos.x == 1 && mousePos.y == 1) loadCount++;
	}

	int cal_folk_count(int x, int y) {//뚫려있는 길의 개수
		folk_count = 0;
		if ((x - 1) > 0 && maze[x - 1][y] != '1') {
			folk_count++;
		}

		if ((x + 1) < rows && maze[x + 1][y] != '1') {
			folk_count++;
		}

		if ((y - 1) > 0 && maze[x][y - 1] != '1') {
			folk_count++;
		}

		if ((y + 1) < cols && maze[x][y + 1] != '1') {
			folk_count++;
		}
		return folk_count;
	}

	void mouse_teleport() {
		//가는 곳에 대한 탐색이 다 끝났을 때 -> 벽이 3개로 가로막혀있을 때 -> loadCount == 1 일때
		//변화됐던 곳 중 top에 있는 것으로 이동하는데 그것과 거리가 가로나 세로 합이 5이상 차이날 때 작동
		//마나가 5.0 이상일 때 -> isMana()
		//그럼 short_load에 있던 스택 좌표들에게 가중치를 더해줌 -> 변화됐던 곳이 나올 때까지 pop하면서 더해주면 될듯
		//에너지는 줄지 않음.
		if (isEnd()) return;
		if (loadCount == 1 && isMana() && teleport_Pos.top().x!=0 && teleport_Pos.top().y!=0) {
			
			if (Distance(mousePos.x, mousePos.y, teleport_Pos.top().x, teleport_Pos.top().y) >= 3) {
				maze[mousePos.x][mousePos.y] = '0';

				while ((short_load.top().x != teleport_Pos.top().x) || (short_load.top().y != teleport_Pos.top().y)) {
					//maze[short_load.top().x][short_load.top().y] = 'S';
					check[short_load.top().x][short_load.top().y] += 100;
					location x = short_load.top();
					short_load.pop();
				}
				
				mousePos = short_load.top();
				maze[mousePos.x][mousePos.y] = 'M';
				check[mousePos.x][mousePos.y] += 10;
				short_load.pop();
				loadCount = 0;
				cal_load_count();
				mouseMana -= 5.0;
				prevMouseDirection = 5;
				

				if (loadCount >= 3) { //갈림길이면
					if (check[mousePos.x][mousePos.y] / 10 >= loadCount - 1) { //갈림길으로 나눠진 길을 다 방문했다면
						teleport_Pos.pop();
						if (isEnd()) return;
						teleport2();
					}
					else if (check[mousePos.x][mousePos.y] == 0) {
						teleport_Pos.push(mousePos);
					}
				}

				
				
				//maze_print();
			}
		}
	}

	void teleport2() {
		if (isEnd()) return;
		//갈림길을 다 방문하고 봤을 때 teleport_Pos를 pop함. 이 때 mousePos와 teleport좌표의 차이가 5이상 나면 teleport함.
		if (isMana() && teleport_Pos.top().x != 0 && teleport_Pos.top().y != 0) {
			if (Distance(mousePos.x, mousePos.y, teleport_Pos.top().x, teleport_Pos.top().y) >= 4) {
				
				maze[mousePos.x][mousePos.y] = '0';
				
				while ((short_load.top().x != teleport_Pos.top().x) || (short_load.top().y != teleport_Pos.top().y)) {
					//maze[short_load.top().x][short_load.top().y] = 'S';
					check[short_load.top().x][short_load.top().y] += 100;
					location x = short_load.top();
					short_load.pop();
				}

				mousePos = short_load.top();
				maze[mousePos.x][mousePos.y] = 'M';
				check[mousePos.x][mousePos.y] += 10;
				short_load.pop();
				mouseMana -= 5.0;
				prevMouseDirection = 5;

				if (loadCount >= 3) { //갈림길이면
					if (check[mousePos.x][mousePos.y] / 10 >= loadCount - 1) { //갈림길으로 나눠진 길을 다 방문했다면
						teleport_Pos.pop();
						if (isEnd()) return;
						teleport2();
					}
					else if (check[mousePos.x][mousePos.y] == 0) {
						teleport_Pos.push(mousePos);
					}
				}

				
				
			}
		}

	}


	bool isMana() {
		return mouseMana >= 5.0;
	}

	bool decideExit() {
		if (mousePos.x==exitPos.x && mousePos.y==exitPos.y) {
			return true;
		}
		return false;
	}

	bool isEnd() {
		return mouseEnergy == 0 || decideExit() || (start && short_load.empty()) || teleport_Pos.empty();
	}

	void findExit() { //출구 좌표 구하는 함수
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				if ((i != 0 && j != 1)&&(i == 0 || j == 0 || i == rows - 1 || j == cols - 1)&&(maze[i][j] == '0')) {
					exitPos.x = i;
					exitPos.y = j;
					break;
				}
			}
		}
	}

	void play() {
		
		ofstream writeFile(filepath.data());
		maze[0][1] = 'M';
		findExit();
		//maze_print();
		while (!isEnd()) {
			mouse_move();
			//maze_print();
			mouse_teleport();
		}
		if (mousePos.x == exitPos.x && mousePos.y== exitPos.y) {
			while (!short_load.empty()) {
				location stack_location = short_load.top();
				maze[stack_location.x][stack_location.y] = 'S';
				short_load.pop();
			}
			maze_print();

			if (mouseEnergy != 0) {
				cout << endl << "Initial Energy : " << rows*cols * 2 << endl;
				cout << "Wasted Energy : " << (rows*cols * 2) - mouseEnergy << endl;
				cout << "Remain Energy : " << mouseEnergy << endl;
			}
			//파일 write 코드
			
			if (writeFile.is_open()) {
				for (int i = 0; i < rows; i++)
				{
					for (int j = 0; j < cols; j++) {
						if (maze[i][j] == '0') writeFile << "□";
						else if (maze[i][j] == 'M') writeFile <<"＠";
						else if (maze[i][j] == 'S') writeFile <<"＠";
						else writeFile << "■";
						//미로 출력
					}
					writeFile << endl;
				}
				
				writeFile << endl << "Initial Energy : " << rows*cols * 2 << endl;
				writeFile << "Wasted Energy : " << (rows*cols * 2) - mouseEnergy << endl;
				writeFile << "Remain Energy : " << mouseEnergy << endl;
				writeFile.close();
			}

			
		}
		else {
			if (writeFile.is_open()) {
				writeFile << "출구를 찾지 못했습니다.\n";
				writeFile.close();
			}

			cout << "출구를 찾지 못했습니다\n";
		}

		getchar();
	}
};
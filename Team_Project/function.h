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
	int rows = 0; //���� ����
	int cols = 0; //���� ����
	int mouseEnergy = 0; //���� ������
	double mouseMana = 0.0; //���� ����
	location mousePos = { 0, 1 }; //���� ó�� ��ǥ
	location exitPos; //Ż�� ��ǥ
	int prevMouseDirection; //���� �� ����
	int loadCount = 0; //���� ���� ������ ����
	int folk_count = 0;
	double move_table[4] = { 0, 0, 0, 0 }; // �� �� �� ��
	char *filename;
	string filepath; //��� ���� ���
	char **maze; //�̷� ��
	char **check;
	bool start = false; //ù ��ǥ�� ������°�
	stack<location> short_load; //�ּ� ��θ� ������ ����
	stack<location> teleport_Pos; //�ڷ���Ʈ ������ ���� �����ϴ� ����

public:
	find_maze(string inputfile) {
		filename = new char[inputfile.size()+1];
		copy(inputfile.begin(), inputfile.end(), filename);
		filename[inputfile.size()] = '\0'; 
		string filename(filename);
		filepath = "result_" + filename; //��� ���� ��� ����

		set_rowcol();

		mouseEnergy = rows*cols * 2; //���� ������

		load();

		teleport_Pos.push({ 0,0 });
	}

	void maze_print() {
		system("cls");
		cout << "--------------------------------------------------------" << endl;
		cout << "\t\tWelcome maze world!!!!" << endl;
		cout << "--------------------------------------------------------" << endl;
		cout << "�� ���� : " << rows << " �� ���� : " << cols <<" ������ : "<<mouseEnergy << " ���� : "<< mouseMana << endl;
		cout << "--------------------------------------------------------" << endl;
		cout << "Ż�� ��ǥ : (" << exitPos.x << "," << exitPos.y << ")" << endl;
		cout << "�� ��ǥ : (" << mousePos.x << "," << mousePos.y << ")" << endl;
		cout << "�ڷ���Ʈ ��ǥ : (" << teleport_Pos.top().x << "," << teleport_Pos.top().y << ")" << endl;
		cout << "--------------------------------------------------------" << endl;

		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++) {
				if (maze[i][j] == '0') printf("  ");
				else if (maze[i][j] == 'M') printf("��");
				else if (maze[i][j] == 'S') printf("��");
				else printf("��");
				//�̷� ���
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
			cout << "���Ƚ��ϴ�." << endl;
		}
		else {
			cout << "������ �ʾҽ��ϴ�." << endl;
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
		case 0: //��
			if (check[mousePos.x-1][mousePos.y] != 0) {
				short_load.pop();
			}
			else short_load.push(mousePos);
			maze[--mousePos.x][mousePos.y] = 'M';
			prevMouseDirection = 1;
			break;
		case 1: //��
			if (check[mousePos.x + 1][mousePos.y] != 0) {
				short_load.pop();
			}
			else short_load.push(mousePos);
			maze[++mousePos.x][mousePos.y] = 'M';
			prevMouseDirection = 0;
			break;
		case 2: //��
			if (check[mousePos.x][mousePos.y - 1] != 0) {
				short_load.pop();
			}
			else short_load.push(mousePos);
			maze[mousePos.x][--mousePos.y] = 'M';
			prevMouseDirection = 3;
			break;
		case 3: //��
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

		if (loadCount >= 3) { //�������̸�
			if (check[mousePos.x][mousePos.y] / 10 == loadCount-1) { //���������� ������ ���� �� �湮�ߴٸ�
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
		//��
		if ((mousePos.x + 1) <= rows && maze[mousePos.x+1][mousePos.y] != '1') {
			move_table[1] = Distance(mousePos.x + 1, mousePos.y, exitPos.x, exitPos.y) + check[mousePos.x + 1][mousePos.y];
		}
		else move_table[1] = rows*cols;
		//��
		if ((mousePos.y - 1)>=0 && maze[mousePos.x][mousePos.y - 1] != '1') {
			move_table[2] = Distance(mousePos.x , mousePos.y - 1, exitPos.x, exitPos.y) + check[mousePos.x][mousePos.y - 1];
		}
		else move_table[2] = rows*cols;
		//��
		if ((mousePos.y + 1) <= cols && maze[mousePos.x][mousePos.y+1] != '1') {
			move_table[3] = Distance(mousePos.x, mousePos.y + 1, exitPos.x, exitPos.y) + check[mousePos.x][mousePos.y + 1];
		}
		else move_table[3] = rows*cols;
		//��
		
		int near_direction_index = 0;

		for (int i = 0; i < 4; i++)
		{
			if (is_fork(i)) near_direction_index = i;
		}

		for (int i = 0; i < 4; i++)
		{
			if (move_table[i] != rows*cols){
				if(move_table[near_direction_index] >= move_table[i] && prevMouseDirection !=i) { //���� �������� �ʰ� ����ġ�� ����� ������ ���� ��
					near_direction_index = i;
				}
			}
		}
		if (mousePos.x != 0 && mousePos.y != 1 && loadCount == 1) near_direction_index = prevMouseDirection; //����� ���� �� �������� ������
		
		return near_direction_index;
	}

	double Distance(int x1, int y1, int x2, int y2) {
		double distance;
		//��Ÿ��� ����
		
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


	void cal_load_count() {//�շ��ִ� ���� ����
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

	int cal_folk_count(int x, int y) {//�շ��ִ� ���� ����
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
		//���� ���� ���� Ž���� �� ������ �� -> ���� 3���� ���θ������� �� -> loadCount == 1 �϶�
		//��ȭ�ƴ� �� �� top�� �ִ� ������ �̵��ϴµ� �װͰ� �Ÿ��� ���γ� ���� ���� 5�̻� ���̳� �� �۵�
		//������ 5.0 �̻��� �� -> isMana()
		//�׷� short_load�� �ִ� ���� ��ǥ�鿡�� ����ġ�� ������ -> ��ȭ�ƴ� ���� ���� ������ pop�ϸ鼭 �����ָ� �ɵ�
		//�������� ���� ����.
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
				

				if (loadCount >= 3) { //�������̸�
					if (check[mousePos.x][mousePos.y] / 10 >= loadCount - 1) { //���������� ������ ���� �� �湮�ߴٸ�
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
		//�������� �� �湮�ϰ� ���� �� teleport_Pos�� pop��. �� �� mousePos�� teleport��ǥ�� ���̰� 5�̻� ���� teleport��.
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

				if (loadCount >= 3) { //�������̸�
					if (check[mousePos.x][mousePos.y] / 10 >= loadCount - 1) { //���������� ������ ���� �� �湮�ߴٸ�
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

	void findExit() { //�ⱸ ��ǥ ���ϴ� �Լ�
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
			//���� write �ڵ�
			
			if (writeFile.is_open()) {
				for (int i = 0; i < rows; i++)
				{
					for (int j = 0; j < cols; j++) {
						if (maze[i][j] == '0') writeFile << "��";
						else if (maze[i][j] == 'M') writeFile <<"��";
						else if (maze[i][j] == 'S') writeFile <<"��";
						else writeFile << "��";
						//�̷� ���
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
				writeFile << "�ⱸ�� ã�� ���߽��ϴ�.\n";
				writeFile.close();
			}

			cout << "�ⱸ�� ã�� ���߽��ϴ�\n";
		}

		getchar();
	}
};
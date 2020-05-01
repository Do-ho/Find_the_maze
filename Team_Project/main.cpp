#include "function.h"

int main()
{
	string filename;
	cout << "Find Maze Project" <<endl;
	cout << "Insert File Name : ";
	cin >> filename;
	find_maze maze1(filename);
	maze1.play();
}
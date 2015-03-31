#include "Transformer.h"
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;
transformer::transformer(int LNumber, float LWidth, double ang){
	nodes = 0;
	angle = ang;
	//QuartNumber = 1;
	CursorMid = 0;
	CursorQuad = 0;
	LayerNumber = LNumber;
	LayerWidth = LWidth;

	QuartNumber = angle / 90 + 2;
	if (angle < 90)
		QuartNumber = 1;
}

void transformer::CreateSpace(){
	n = (elements * 3 + nodes + 1)*(LayerNumber + 1)*QuartNumber;

	inds = new int*[elements];
	for (int i = 0; i < elements; i++)
		inds[i] = new int[3];


	koor = new double*[n];
	for (int i = 0; i < n; i++)
		koor[i] = new double[3];

	quad = new int*[elements * 3 * LayerNumber*QuartNumber];
	for (int i = 0; i < elements * 3 * LayerNumber*QuartNumber; i++)
		quad[i] = new int[8];

	mid = new int*[elements * 3];
	for (int i = 0; i < elements * 3; i++)
		mid[i] = new int[3];


}

void transformer::SpaceTakesNull(){
	for (int i = 0; i < elements; i++)
		for (int j = 0; j < 3; j++)
			inds[i][j] = 0;

	for (int i = 0; i < n; i++)
		for (int j = 0; j < 3; j++)
			koor[i][j] = 0;

	for (int i = 0; i < elements * 3 * LayerNumber*QuartNumber; i++)
		for (int j = 0; j < 8; j++)
			quad[i][j] = 0;

	for (int i = 0; i < elements * 3; i++)
		for (int j = 0; j < 3; j++)
			mid[i][j] = 0;
}

bool transformer::LoadFile(const char* name){
	ifstream input(name);
	char a = 's';

	while (a != '=') //считываем, пока не доходим до знака =
		input >> a;
	input >> nodes;//после знака - количество узлов

	input >> a;//переводим курсор

	while (a != '=')//опять ищем знак =
		input >> a;
	input >> elements;//после знака - количество элементов

	CreateSpace(); //получив данные, создаем массивы для работы
	SpaceTakesNull();//обнуляем их

	while (a != ']')//далее находим закрывающуюся скобку, после которой - матрица элементов
		input >> a;

	for (int i = 0; i < elements; i++)//считываем матрицу элементов
		for (int j = 0; j < 3; j++)
			input >> inds[i][j];

	input >> a;//снова переводим курсор

	while (a != ']')//ищем следующую закрывающуюся скобку
		input >> a;

	for (int i = 0; i < nodes; i++)//считываем матрицу координат
		for (int j = 0; j < 2; j++)
			input >> koor[i][j];

	CursorNodes = nodes;

	input.close();
	return 0;
}

bool transformer::Partition(){
	int mdot[3], cdot;
	for (int i = 0; i < elements; i++){
		mdot[0] = FindMid(inds[i][0], inds[i][1]);
		mdot[1] = FindMid(inds[i][0], inds[i][2]);
		mdot[2] = FindMid(inds[i][1], inds[i][2]);
		cdot = FindCentral(mdot[0], mdot[1], mdot[2]);//находим середину треугольника

		quad[CursorQuad][0] = inds[i][0];//записываем в массив полученые четырехугольники
		quad[CursorQuad][1] = mdot[0];
		quad[CursorQuad][2] = cdot;
		quad[CursorQuad][3] = mdot[1];
		CursorQuad++;
		quad[CursorQuad][0] = inds[i][1];
		quad[CursorQuad][1] = mdot[2];
		quad[CursorQuad][2] = cdot;
		quad[CursorQuad][3] = mdot[0];
		CursorQuad++;
		quad[CursorQuad][0] = inds[i][2];
		quad[CursorQuad][1] = mdot[1];
		quad[CursorQuad][2] = cdot;
		quad[CursorQuad][3] = mdot[2];
		CursorQuad++;

		//SaveFile();
	}
    
	return 0;
}


int transformer::FindMid(int max, int min){

	if (max < min){ //корректно выбираем элементы для положительного результата вычитания
		max += min;
		min = max - min;
		max = max - min;
	}
	max--;//уменьшаем на один, т.к. используем, как индекс массива, а не номер узла
	min--;

	for (int i = CursorMid; i >= 0; i--)//пытаемся найти данную сторону в массиве уже известных сторон со средними точками
		if (max == mid[i][0] && min == mid[i][1])
			return (mid[i][2] + 1);
	//если не нашли, то вычисляем среднюю точку и сразу заносим ее в матрицу координат
	koor[CursorNodes][0] = (koor[max][0] - koor[min][0]) / 2 + koor[min][0];//середина по Х
	koor[CursorNodes][1] = (koor[max][1] - koor[min][1]) / 2 + koor[min][1];//середина по У
	mid[CursorMid][0] = max;//далее заносим среднюю точку в массив найденых точек
	mid[CursorMid][1] = min;
	mid[CursorMid][2] = CursorNodes;//записываем номер узла
	CursorMid++;//перемещаем курсоры массивов на следующую позицию
	CursorNodes++;

	return CursorNodes;
}

int transformer::FindCentral(int a, int b, int c){
	a--; b--; c--;
	koor[CursorNodes][0] = (koor[a][0] + koor[b][0] + koor[c][0]) / 3.0;//создаем новый узел и передаем ему координаты середины треугольника
	koor[CursorNodes][1] = (koor[a][1] + koor[b][1] + koor[c][1]) / 3.0;
	CursorNodes++;//перемещаем курсор
	return CursorNodes;
}

bool transformer::SaveFile(const char* name){
	ofstream grid("grid.txt");//Создаем потоки вывода
	ofstream bars("bars.txt");
	ofstream dat(name);
	dat << endl << CursorQuad*LayerNumber << "  " << CursorNodes*(LayerNumber + 1) << endl;

	for (int i = 0; i < CursorQuad*LayerNumber; i++){//Выводим массив элементов в документы bars и data
		for (int j = 0; j < 8; j++){
			bars << quad[i][j] << " ";
			dat << quad[i][j] << " ";
		}
		bars << endl;
		dat << endl;
	}

	//dat << endl << "0 0 0" << endl;//Компенсируем особенности FigureBuilder. Переданный массив координат нумеруется там с 0
	//поэтому добавляем еще одну точку, для сдвига всего массива на одну позицию. При нумерации с первого элемента - убрать строку

	for (int i = 0; i < CursorNodes*(LayerNumber + 1); i++){ // Выводим массив координат
		for (int j = 0; j < 3; j++){
			grid << koor[i][j] << " ";
			dat << koor[i][j] << " ";
		}
		grid << endl;
		dat << endl;
	}




	grid.close();//Закрываем потоки вывода
	bars.close();
	dat.close();

	return 0;
}


bool transformer::MakeLayer(){
	/*Стоит обратить внимание, что курсоры Nodes и Quad на данном этапе содержат количество существующих одномерных узлов
	и элементов. То есть узлов и элементов, которые образуют одну плоскость. Дальше мы будем "вытягивать" эту плоскость.
	А если точнее - строить над ней аналогичные. Для того, чтобы получить общее количество узлов или элементов, надо домножить
	курсоры на количество слоев (+1)*/

	cout << "New inds creating \n";
	for (int j = 1; j <= LayerNumber; j++) //добавляем координаты новых слоев
		for (int i = 0; i < CursorNodes; i++)
			for (int k = 0; k < 3; k++){
				if (k != 2)
					koor[i + CursorNodes*j][k] = koor[i][k]; // дублируем координаты Х и У
				else
					koor[i + CursorNodes*j][k] = LayerWidth * j;// координату Z вычисляем путем умножения толщины слоя на его номер
			}


	cout << "New layers creating \n";
	for (int j = 1; j <= LayerNumber; j++){
		if (LayerNumber != 1 && j != LayerNumber)//Если слой не 1, то создаем "нижние" плоскости всех слоев
			for (int i = 0; i < CursorQuad; i++)
				for (int k = 0; k < 4; k++)
					quad[i + CursorQuad*j][k] = quad[i][k] + CursorNodes*j;//смещение задаем количеством существующих узлов

		for (int i = 0; i < CursorQuad; i++)//Задаем "верхние" плоскости слоя или слоев
			for (int k = 0; k < 4; k++)
				quad[i + CursorQuad*(j - 1)][k + 4] = quad[i + CursorQuad*(j - 1)][k] + CursorNodes;
	}

	return 0;
}

void transformer::swap(int a, int b){

	for (int i = 0; i < 3; i++){
		koor[a][i] += koor[b][i];
		koor[b][i] = koor[a][i] - koor[b][i];
		koor[a][i] = koor[a][i] - koor[b][i];
	}
}

void transformer::Sort_Koor(bool k){

	int* temp = new int[CursorNodes];

	for (int i = 0; i < CursorNodes; i++){
		koor[i][2] = i;
	}

	shaker(0, CursorNodes - 1, k);

	for (int i = 0; i < CursorNodes; i++){
		temp[(int)koor[i][2]] = i;
	}

	for (int i = 0; i < CursorQuad; i++)
		for (int j = 0; j < 4; j++)
			quad[i][j] = temp[quad[i][j] - 1] + 1;

	for (int i = 0; i < CursorNodes; i++){
		koor[i][2] = 0;
	}
}



void transformer::shaker(int Left, int Right, bool k){
	bool k1 = 1;
	if (k == 1)
		k1 = 0;

	do
	{
		for (int i = Right; i > Left; i--)
		{
			if /*(koor[i][k] < koor[i-1][k] ||(koor[i][k] == koor[i-1][k] && koor[i][k1] < koor[i-1][k1]))*/
				(koor[i][k] * 1000000 + koor[i][k1] < koor[i - 1][k] * 1000000 + koor[i - 1][k1])
			{
				swap(i, i - 1);
			}
		}
		Left = Left + 1;
		//Сдвигаем к концу массива "тяжелые элементы"
		for (int i = Left; i < Right; i++)
		{
			if /*(koor[i][k] > koor[i+1][k] ||(koor[i][k] == koor[i+1][k] && koor[i][k1] > koor[i+1][k1]))*/
				(koor[i][k] * 1000000 + koor[i][k1] > koor[i + 1][k] * 1000000 + koor[i + 1][k1])
			{
				swap(i, i + 1);
			}
		}
		Right = Right - 1;
	} while (Left <= Right);
}

void transformer::fullReflect(int quart){
	/*NoReflectNodes = CursorNodes; // вынести глобальнее
	NoReflectQuad = CursorQuad;
	QuartNumber = 1;*/

	int tx, ty;
	switch (quart){
	case 2:{
		tx = -1;
		ty = 1;
		break; }
	case 3:{
		tx = -1;
		ty = -1;
		break; }
	case 4:{
		tx = 1;
		ty = -1;
		break; }
	default:{return; }
	}

	for (int i = 0; i < NoReflectNodes; i++){
		//CursorNodes = NoReflectNodes + i;
		koor[CursorNodes][0] = koor[i][0] * tx;
		koor[CursorNodes][1] = koor[i][1] * ty;
		CursorNodes++;
	}

	for (int i = 0; i < NoReflectQuad; i++){
		for (int k = 0; k < 4; k++){
			quad[CursorQuad][k] = quad[i][k] + QuartNumber * NoReflectNodes;
		}
		CursorQuad++;
	}

	QuartNumber++;
}

void transformer::turn(){

	int** tnodes;

	tnodes = new int *[CursorNodes];
	for (int i = 0; i < CursorNodes; i++)
		tnodes[i] = new int[2];

	for (int i = 0; i < CursorNodes; i++){
		tnodes[i][0] = 0;
		tnodes[i][1] = -1;
	}

	NoReflectNodes = CursorNodes;
	NoReflectQuad = CursorQuad;
	QuartNumber = 1;

	if (angle >= 180)
		fullReflect(2);
	if (angle >= 270)
		fullReflect(3);
	if (angle >= 360)
		fullReflect(4);


	/*
	int tx, ty;
	int	tcurs = 0;
	int res[4], sum;
    if(angle > 90 && angle < 180){
	tx = -1;
	ty = 1;
	angle -= 90;
	}
	else if(angle > 180 && angle < 270){
	tx = -1;
	ty = -1;
	angle -= 180;
	}
	else if(angle >270 && angle < 360){
	tx = 1;
	ty = -1;
	angle -= 270;
	}
	else
	return;*/

	/*  bool is = 0;
	int move = 0;
	for(int i = 0; i < NoReflectQuad; i++){
	sum = 0;
	for(int k = 0; k < 4; k++){
	// cout << quad[i][k] << " ";
	res[k] = testKoor(quad[i][k]-1);
	sum+=res[k];
	}
	// cout << " \ " << sum << endl;
	if(sum < 3){
	for(int k = 0; k < 4; k++){

	move = 0;
	is = 0;
	if(res[k] == 1)
	if(k == 3 && res[0] == 0)
	move = quad[i][0];

	else if(k == 0 && res[3] == 0)
	move = quad[i][3];

	else if(res[k+1] == 0)
	move = quad[i][k+1];

	else if(res[k-1] == 0)
	move = quad[i][k-1];


	for(int j = 0; j < tcurs; j++)
	if(tnodes[j][0] == quad[i][k]-1 && (move == 0 || move != 0 && tnodes[j][1] != -1)){
	is = 1;
	if(move)
	if(testOtherNode(tnodes[j][0], tnodes[j][1], move-1))
	tnodes[j][1] = move - 1;

	}
	else if(tnodes[j][0] == quad[i][k]-1 && move == 0 && tnodes[j][1] == -1){
	tnodes[j][1] = move -1;
	is = 1;
	}

	if(is == 0 && quad[i][k] != 1){
	tnodes[tcurs][0] = quad[i][k] - 1;
	if(move != 0)
	tnodes[tcurs][1] = move-1;
	// cout << "    " << tnodes[tcurs][0] << "  >> " << tnodes[tcurs][1] << "|" << tcurs << endl;
	tcurs++;}
	}
	quad[i][5] = 1;
	}
	else if(sum == 3){
	for(int k = 0; k < 4; k++)
	if(res[k] == 0 && quad[i][k] != 1){
	tnodes[tcurs][0] = (quad[i][k] - 1);
	if(k != 0)
	tnodes[tcurs][1] = quad[i][k-1]-1;
	else
	tnodes[tcurs][1] = quad[i][k+1]-1;
	tcurs++;}

	}
	}

	for(int i = 0; i < tcurs; i++){
	is = 0;
	if(tnodes[i][1] == -1){
	koor[CursorNodes][0] = koor[tnodes[i][0]][0] * tx;
	koor[CursorNodes][1] = koor[tnodes[i][0]][1] * ty;}
	else{
	turnNode(tnodes[i][0], tnodes[i][1], CursorNodes);
	koor[CursorNodes][0] *= tx;
	koor[CursorNodes][1] *= ty;
	}

	CursorNodes++;
	}

	for(int i = 0; i < NoReflectQuad; i++){
	if(quad[i][5] == 1){
	for(int k = 0; k < 4; k++){
	if(quad[i][k] == 1)
	quad[CursorQuad][k] = 1;
	for(int j = 0; j < tcurs; j++)
	if(quad[i][k] == tnodes[j][0]+1)
	quad[CursorQuad][k] = CursorNodes - tcurs + j + 1;}
	CursorQuad++;}

	}

	for(int i = 0; i < tcurs; i++)
	cout << tnodes[i][0] << " " << tnodes[i][1] << endl;
	cout << " 0" << tcurs;*/
}

bool transformer::testKoor(int a){
	//angle -= 90;

	//cout << koor[a][0] << " / " << " sqr " <<  pow(koor[a][0], 2) << "+" << pow(koor[a][1],2) << endl;
	if (koor[a][0] == 0 && koor[a][1] == 0)
		return 0;

	double cs = abs(koor[a][0] / sqrt(pow(koor[a][0], 2) + pow(koor[a][1], 2)));

	double mcs = cos((90 - angle) * M_PI / 180);
	/*if(QuartNumber == 5)
	mcs = cos((angle) * M_PI/180);*/
	//cout << koor[a][0] << "  " << koor[a][1] << "    " << cs;

	if (QuartNumber == 2) {
		if (cs > (mcs - 0.0001))
			return 0;
		else
			return 1;
    }


	if (cs < (mcs - 0.0001))
		return 0;
	else
		return 1;
}


void transformer::turnNode(int a, int b, int curs){
	double x1, x2, y1, y2, C, B, k, x, y;

	x1 = koor[a][0];
	y1 = koor[a][1];
	x2 = koor[b][0];
	y2 = koor[b][1];

	B = tan((angle)* M_PI / 180);


	if (x1 != x2 && y1 != y2){
		k = (y1 - y2) / (x1 - x2);
		C = y1 - (y1 - y2) / (x1 - x2)*x1;

		x = (B - C) / ((y1 - y2) / (x1 - x2));
		x = C / (B - k);
		y = x * B;
	}

	else if (x1 == x2){
		B = tan((90 - angle) * M_PI / 180);
		x = x1;
		y = B * x1;
	}
	else{
		B = tan((90 - angle) * M_PI / 180);
		y = y1;
		x = y1 / B;
	}

	koor[curs][0] = x;
	koor[curs][1] = y;
}

bool transformer::testOtherNode(int fo, int one, int sec){

	bool ans;

	turnNode(fo, one, CursorNodes);
	turnNode(fo, sec, CursorNodes);

	if (((abs(koor[CursorNodes][0]) - abs(koor[fo][0])) + (abs(koor[CursorNodes][1]) - abs(koor[fo][1])))  >
		((abs(koor[CursorNodes + 1][0]) - abs(koor[fo][0])) + (abs(koor[CursorNodes + 1][1]) - abs(koor[fo][1]))))
		ans = 0;
	else
		ans = 1;

	koor[CursorNodes][0] = 0;
	koor[CursorNodes + 1][0] = 0;
	koor[CursorNodes][1] = 0;
	koor[CursorNodes + 1][1] = 0;

	return ans;

}

void transformer::makeCentral(int k){
	if (k > 2)
		return;
	int max, min, mid;
	/*int n;

	if(k < 2)
	n = CursorNodes;
	else
	n = CursorNodes*LayerNumber*QuartNumber;*/


	max = min = koor[0][k];

	for (int i = 0; i < n; i++){
		if (max < koor[i][k])
			max = koor[i][k];
		if (min > koor[i][k])
			min = koor[i][k];
	}

	mid = (max - min) / 2 + min;

	for (int i = 0; i < n; i++)
		koor[i][k] -= mid;

}

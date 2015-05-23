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

    // TODO: Не должно ли быть 1, 2, 3, 4 - в зависимости от количества отображаемых четвертей
	QuartNumber = angle / 90 + 2;
	if (angle < 90)
		QuartNumber = 1;
}

void transformer::CreateSpace(){
    // Разбиение создает на каждый элемент по 3 дополнительные точки (середины отрезков и центр)
    // После разбития происходит вытягивание и отражение (относительно [0;0]) в плоскости
    // Слоев на один больше, так как нужен еще один, чтобы фигура состояла из призм (1 слой призм - это два плоских слоя)
    // Память выделяется избыточно, так как дополнительные точки соприкасающихся треугольников могут совпадать
	n = (elements * 3 + nodes) * (LayerNumber+1) * QuartNumber;

	inds = new int*[elements];
	for (int i = 0; i < elements; i++)
		inds[i] = new int[3];

    // Узлы характеризуются пространственными координатами и материалами, из которого они сделаны 
	koor = new double*[n];
	for (int i = 0; i < n; i++)
		koor[i] = new double[3];
    materials = new int[n]; 

    // Из одного треугольника получается три призматических восьмиугольника
	quad = new int*[elements * 3 * LayerNumber*QuartNumber];
	for (int i = 0; i < elements * 3 * LayerNumber*QuartNumber; i++)
		quad[i] = new int[8];

	edge_inds = new int*[elements * 3];
	for (int i = 0; i < elements * 3; i++)
		edge_inds[i] = new int[3];
}

void transformer::SpaceTakesNull(){
	for (int i = 0; i < elements; i++)
		for (int j = 0; j < 3; j++)
			inds[i][j] = 0;

	for (int i = 0; i < n; i++){
		for (int j = 0; j < 3; j++)
			koor[i][j] = 0;
        materials[i] = 1;   // 1 - фигура. По-умолчанию все точки принадлежат фигуре
    }

	for (int i = 0; i < elements * 3 * LayerNumber*QuartNumber; i++)
		for (int j = 0; j < 8; j++)
			quad[i][j] = 0;

	for (int i = 0; i < elements * 3; i++)
		for (int j = 0; j < 3; j++)
			edge_inds[i][j] = 0;
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


	input >> a; // Не до конца понимаю, для чего тут считать нужно дополнительно. Но иначе работает плохо
	while (a != ']') // Игнорируем [contact]
		input >> a;
	input >> a;
	while (a != ']') // Игнорируем [force]
		input >> a;
	input >> a;
	while (a != ']')// Готовимся считать [material]
		input >> a;

    for(int i=0; i < nodes; i++)
        input >> materials[i];


	input.close();
	return 0;
}

// Разбиваем треугольники на 3 четырехугольника (соединяя медианы сторон с центром)
bool transformer::Partition(){
	int mdot[3], cdot;
	for (int i = 0; i < elements; i++){
		mdot[0] = GetEdgeMid_inds(inds[i][0], inds[i][1]);
		mdot[1] = GetEdgeMid_inds(inds[i][0], inds[i][2]);
		mdot[2] = GetEdgeMid_inds(inds[i][1], inds[i][2]);
		cdot = GetCentralDot_inds(mdot[0], mdot[1], mdot[2]);//находим середину треугольника

        // Записываем в массив полученные четырехугольники (quad[4..7] пока игнорируем) 
		quad[CursorQuad][0] = inds[i][0];
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
	}
    
	return 0;
}

int transformer::GetEdgeMid_inds(int start_inds, int fin_inds){
	if (start_inds > fin_inds){  //корректно выбираем элементы для положительного результата вычитания (индексы даны в порядке роста значений x-,y-координат)
        int hold = start_inds;
        start_inds = fin_inds;
        fin_inds = hold;
	}
	start_inds--;//уменьшаем на один, ибо используем, как индекс массива, а не номер узла
	fin_inds--;
    
    // Проверяем, не вычислялась ли середина этого отрезка ранее
    for (int i = CursorMid; i >= 0; i--) 
		if (start_inds == edge_inds[i][0] && fin_inds == edge_inds[i][1])
			return (edge_inds[i][2] + 1);
	// Вычисляем среднюю точку и обновляем данные
	koor[CursorNodes][0] = (koor[fin_inds][0] - koor[start_inds][0]) / 2 + koor[start_inds][0];//середина по Х
	koor[CursorNodes][1] = (koor[fin_inds][1] - koor[start_inds][1]) / 2 + koor[start_inds][1];//середина по У
    if( materials[start_inds] == 0 || materials[fin_inds] == 0 ) // Отрезок с "воздушной" точкой - "воздушный" - как и середина
        materials[CursorNodes] = 0;
	edge_inds[CursorMid][0] = start_inds;//далее заносим среднюю точку в массив просчитанных сторон
	edge_inds[CursorMid][1] = fin_inds;
	edge_inds[CursorMid][2] = CursorNodes;//записываем номер узла
	CursorMid++;//перемещаем курсоры массивов на следующую позицию
	CursorNodes++;

	return CursorNodes;
}

int transformer::GetCentralDot_inds(int a, int b, int c){
	a--; b--; c--;
	koor[CursorNodes][0] = (koor[a][0] + koor[b][0] + koor[c][0]) / 3.0;//создаем новый узел и передаем ему координаты середины треугольника
	koor[CursorNodes][1] = (koor[a][1] + koor[b][1] + koor[c][1]) / 3.0;
     // Если треугольник содержит "воздушную" точку - то средняя точка также будет "воздушной"
    if( materials[a] == 0 || materials[b] == 0 || materials[c] == 0 )
        materials[CursorNodes] = 0;
	CursorNodes++;//перемещаем курсор
	return CursorNodes;
}

// Формат выходных данных: 
// - N,M - количества элементов и узлов
// - N строк - индексы вершин i-го элемента в матрице узлов.
// - M строк - матрица узлов
// - M строк - материал k-го узла
// Сакральный смысл дополнительных файлов grid.txt и bars.txt: Сергею Пащенко (пользователю) на тот момент так было удобнее
bool transformer::SaveFile(const char* name){
	ofstream grid("grid.txt");//Создаем потоки вывода
	ofstream bars("bars.txt");
	ofstream dat(name);
    const int ELEM_COUNT = CursorQuad*LayerNumber;
    const int EDGE_COUNT = CursorNodes*(LayerNumber+1); // на один слой больше, так как призма на призме - это три, а не два слоя
    const int EDGE_IN_ELEM = 8; // Элемент: призматический восьмигранник (созданный параллельным переносом четырехугольника по z)
    const int EUCLID_SPACE = 3; // x, y, z

	dat << endl << ELEM_COUNT << "  " << EDGE_COUNT << endl;

	for (int i = 0; i < ELEM_COUNT; i++){//Выводим массив элементов в документы bars и data
		for (int j = 0; j < EDGE_IN_ELEM; j++){
			bars << quad[i][j] << " ";
			dat << quad[i][j] << " ";
		}
		bars << endl;
		dat << endl;
	}

	for (int i = 0; i < EDGE_COUNT; i++){ // Выводим массив координат
		for (int j = 0; j < EUCLID_SPACE; j++){
			grid << koor[i][j] << " ";
			dat << koor[i][j] << " ";
		}
		grid << endl;
		dat << endl;
	}

    for( int i = 0; i < EDGE_COUNT; i++ ) {
        dat << materials[i] << endl;
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

    if( CursorNodes > elements*3 + nodes ){
        cout << "To few points in layer: " << CursorNodes << ". We planed: " << elements*3 + nodes << endl;
        return false;
    }            
    if( CursorNodes-1 + CursorNodes*(LayerNumber) > n ) {
        cout << "It is more points in figure, than we planed: " << (CursorNodes-1) + CursorNodes*LayerNumber
            << ". We planed: " << n << endl;
        return false;
    }
    
    // Добавляем координаты новых слоев (первый слой - это нижние точки первых призм)
	for (int layer = 1; layer <= LayerNumber; layer++)
		for (int i = 0; i < CursorNodes; i++) {
            koor[i + CursorNodes*layer][0] = koor[i][0];    // Копируем x и y-координаты 
            koor[i + CursorNodes*layer][1] = koor[i][1]; 
            koor[i + CursorNodes*layer][2] = LayerWidth * layer; // z-координата зависит от толщины слоя
            materials[i+CursorNodes*layer] = materials[i];
        }


    // После разбития мы получили плоскость с четырехугольниками на ней. Превратим их параллельным переносом в призмы 
    for (int i = 0; i < CursorQuad; i++)
        for (int vertex = 0; vertex < 4; vertex++){
            if( quad[i][vertex] + CursorNodes > n )
                cout << "Does not good!" << quad[i][vertex] + CursorNodes << "  " << n << endl;
            quad[i][vertex+4] = quad[i][vertex] + CursorNodes;//смещение задаем количеством существующих узлов
        }
    // Создаем новые слои
	for (int layer = 1; layer <= LayerNumber-1; layer++)
        for (int i = 0; i < CursorQuad; i++)
            for (int k = 0; k < 8; k++)
                quad[i + CursorQuad*layer][k] = quad[i][k] + CursorNodes*layer;//смещение задаем количеством существующих узлов
	
	return true;
}

void transformer::swap(int a, int b){
    double hold;
	for (int i = 0; i < 3; i++){
        hold = koor[b][i];
        koor[b][i] = koor[a][i];
        koor[a][i] = hold;
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

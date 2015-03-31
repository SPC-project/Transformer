#ifndef TRANSFORMER_H_INCLUDED
#define TRANSFORMER_H_INCLUDED

class transformer{
public:
	bool LoadFile(const char *name);//получение данных из файла .pmd
	bool Partition();//разбиение треугольника на четырехугольники
	bool MakeLayer();//сделать в фигуре слои
	bool SaveFile(const char* name);//сохраняем результаты в файл
	void Sort_Koor(bool k);
	void turn();
	transformer(int LNumber, float LWidth, double ang);
	void makeCentral(int k);

private:

	void CreateSpace();//создание рабочего пространства
	void SpaceTakesNull();//обнуление рабочего пространства
	int FindMid(int, int);//поиск середины отрезка
	int FindCentral(int, int, int);//поиск средней точки треугольника
	void swap(int a, int b);
	//void Sort_Koor(bool k);
	void quickSort(int first, int last, bool k);
	void shaker(int, int, bool k);
	void fullReflect(int quart);
	bool testKoor(int);
	double turnKoor(int);
	void turnNode(int, int, int);
	bool testOtherNode(int fo, int one, int sec);




	int nodes; //начальное количество узлов
	int elements; //количество треугольников
	int n;//прогнозируемое количество узлов
	int** inds;//матрица элементов
	int** quad;//матрица узлов четырехугольников
	double** koor;//матрица координат
	int** mid; //реестр найденных средних точек сторон
	int CursorNodes;// курсоры текущей строки соответствующих матриц
	int CursorMid;
	int CursorQuad;
	int LayerNumber;//количество слоев фигуры
	float LayerWidth;//толщина слоя
	int QuartNumber;
	int NoReflectNodes;
	int NoReflectQuad;
	double angle;
	//int QuartNumber;

};

#endif // TRANSFORMER_H_INCLUDED

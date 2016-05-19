#ifndef TRANSFORMER_H_INCLUDED
#define TRANSFORMER_H_INCLUDED

class transformer{
public:
	transformer(int LNum, float LWidth, double ang, int CNum);

	int LoadFile(const char *name);//получение данных из файла .pmd
	bool Partition();//разбиение треугольника на четырехугольники
	bool CreateLayers();//сделать в фигуре слои
	bool SaveFile(const char* name);//сохраняем результаты в файл
	void Sort_Koor(bool k);
	void turn();
	void makeCentral(int k);
private:

	void CreateSpace();//создание рабочего пространства
	int GetEdgeMid_inds(int, int);//поиск середины отрезка
	int GetCentralDot_inds(int, int, int);//поиск средней точки треугольника
	inline void swap(int a, int b);
	//void Sort_Koor(bool k);
	void quickSort(int first, int last, bool k);
	void shaker(int, int, bool k);
	void fullReflect(int quart);
	bool testKoor(int);
	double turnKoor(int);
	void turnNode(int, int, int);
	bool testOtherNode(int fo, int one, int sec);
    int  pointsInLayer();
    int  countOfLayers();
    int  countOfElemLayers(); // количество слоев с призмами
	int  countOfElements(); // прогнозируемое количество призматических элементов в слое


	int nodes;      // Начальное количество узлов
	int elements;   // Количество треугольников
	int n;          // Прогнозируемое количество узлов четырехугольников (при отображении по z-координате)
	double** koor;  // Матрица координат узла: (koor[i][0], koor[i][1], koor[i][2]) -> (x,y,z) координаты i-го узла
    int* materials; // Реестр материалов: materials[i] -> материал i-го узла
	int** inds;     // Матрица индексов: inds[k][l] -> индекс l-го узла k-го треугольника 
	int** quad;     // Матрица узлов элементов (призматических четырех угольников - 8 узлов на элемент)
    int* quad_m; 
	int** edge_inds;  // Реестр просчитанных сторон треугольников; mid[i][0], mid[i][1] - индексы концов, mid[i][2] - середина 

	int CursorNodes;  // Курсоры указывают на индекс строки в соответствующих матрицах, в которую будет производится запись
	int CursorMid;
	int CursorQuad;

	int LayerNumber;  // Количество слоев фигуры
    int CapLayerNum;  // Количество 'воздушных' слоев, сгенерируемых 'сверху' и 'снизу' фигуры
	float LayerWidth; //толщина слоя
	int QuartNumber;  //количество четвертей, в которые отражаем. Если 1='.', 2=':', 3=':.', 4='::' 
	int NoReflectNodes;
	int NoReflectQuad;
	double angle;
};

#endif // TRANSFORMER_H_INCLUDED

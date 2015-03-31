#ifndef TRANSFORMER_H_INCLUDED
#define TRANSFORMER_H_INCLUDED

class transformer{
public:
	bool LoadFile(const char *name);//��������� ������ �� ����� .pmd
	bool Partition();//��������� ������������ �� ����������������
	bool MakeLayer();//������� � ������ ����
	bool SaveFile(const char* name);//��������� ���������� � ����
	void Sort_Koor(bool k);
	void turn();
	transformer(int LNumber, float LWidth, double ang);
	void makeCentral(int k);

private:

	void CreateSpace();//�������� �������� ������������
	void SpaceTakesNull();//��������� �������� ������������
	int FindMid(int, int);//����� �������� �������
	int FindCentral(int, int, int);//����� ������� ����� ������������
	void swap(int a, int b);
	//void Sort_Koor(bool k);
	void quickSort(int first, int last, bool k);
	void shaker(int, int, bool k);
	void fullReflect(int quart);
	bool testKoor(int);
	double turnKoor(int);
	void turnNode(int, int, int);
	bool testOtherNode(int fo, int one, int sec);




	int nodes; //��������� ���������� �����
	int elements; //���������� �������������
	int n;//�������������� ���������� �����
	int** inds;//������� ���������
	int** quad;//������� ����� �����������������
	double** koor;//������� ���������
	int** mid; //������ ��������� ������� ����� ������
	int CursorNodes;// ������� ������� ������ ��������������� ������
	int CursorMid;
	int CursorQuad;
	int LayerNumber;//���������� ����� ������
	float LayerWidth;//������� ����
	int QuartNumber;
	int NoReflectNodes;
	int NoReflectQuad;
	double angle;
	//int QuartNumber;

};

#endif // TRANSFORMER_H_INCLUDED

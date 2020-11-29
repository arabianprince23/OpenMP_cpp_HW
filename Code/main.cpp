#include <iostream>
#include <omp.h>
#include <vector>
#include <fstream>
#include <thread>
#include <chrono>
#include <clocale> //����������� ��� ������� setlocale()

using namespace std;

int n, quantityOfThreads;
thread* threads;
vector<bool> usedMatrixLines;
int rankOfMatrix;
int** matrix;
const double EPS = 1E-9;

void inputMatrix() {
    int elem;

    matrix = new int* [n]; // ������������� �������
    for (int i = 0; i < n; i++) {
        matrix[i] = new int[n];
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << "������� ������� (" << i << ";" << j << ") : ";
            cin >> elem;
            matrix[i][j] = elem;
        }
    }
}

void printMatrix() {
    std::cout << endl << "���������� ������� : " << endl;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << endl;
    }
}

void GetMatrixFromFile(string pathToFile) {
    ifstream in(pathToFile);
    in >> n >> quantityOfThreads;

    cout << "���������� �������� n : " << n << endl << "���������� �������� ���-�� ������� : " << quantityOfThreads << endl;
    rankOfMatrix = n;
    usedMatrixLines = vector<bool>(n);

    matrix = new int* [n];

    for (int i = 0; i < n; i++) {
        matrix[i] = new int[n];
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            in >> matrix[i][j];
        }
    }
}

void compute_rank() {
    
    vector<bool> row_selected(n, false);

    // ���� ������, ����������� ����� ������ ������� �� ��� ��� ������.
    // ���� ����� ����������� � quantityOfThreads ������ �������.
    // ��� ���� ��������� ����� ����� ������������� ������������ ����� ����� ��������.
#pragma omp parallel for num_threads(ths)
    for (int i = 0; i < n; ++i) {
        int j;

        for (j = 0; j < n; ++j)
            if (!row_selected[j] && abs(matrix[j][i]) > EPS)
                break;

        if (j == n)
            --rankOfMatrix;

        else {
            row_selected[j] = true;
            for (int p = i + 1; p < n; ++p)
                matrix[j][p] /= matrix[j][i];
            for (int k = 0; k < n; ++k)
                if (k != j && abs(matrix[k][i]) > EPS)
                    for (int p = i + 1; p < n; ++p)
                        matrix[k][p] -= matrix[j][p] * matrix[k][i];
        }
    }
}

int main(int argc, char* argv[]) {

    setlocale(LC_ALL, "Russian"); //������� setlocale() � �����������

    // �������� ������ ������� �� ������ ������ ���������
    auto begin = std::chrono::steady_clock::now();

    // � ������, ���� ������������ �� ������� � ��������� ���� � ����� � ��������, ������ ������ ����
    if (argc == 1) {
        cout << "������� n (����������� �������) : ";
        cin >> n;
        cout << "������� ���������� ������� : ";
        cin >> quantityOfThreads;

        if (quantityOfThreads > n || n < 1 || quantityOfThreads < 1) {
            cout << endl << "������������ ����!" << endl <<
                "1. ���������� ������� ������ ���� <= ����������� �������." << endl <<
                "2. ����������� ������� ������ ���� >= 1." << endl <<
                "3. ���������� ������� ������ ���� >= 1." << endl << endl << endl << endl << endl;
            return 0;
        }

        usedMatrixLines = vector<bool>(n);
        rankOfMatrix = n;

        inputMatrix(); // ���� ������������� �������
        printMatrix(); // ����� ������� �� �����
        compute_rank(); // ������� �������� ����� �������

        cout << endl << "���������� ���� �������: " << rankOfMatrix << endl; // ����� ����������� ������ ������ ��� �����

        auto end = chrono::steady_clock::now();
        auto elapsed_ms = chrono::duration_cast<std::chrono::milliseconds>(end - begin);
        cout << "����� ������ ��������� : " << elapsed_ms.count() << " ms\n";
    }
    else if (argc == 2) { // ���� ������� ���� � �������� - ��������� ���

        GetMatrixFromFile(argv[1]);

        if (quantityOfThreads > n || n < 1 || quantityOfThreads < 1) {
            cout << endl << "������������ ����!" << endl <<
                "1. ���������� ������� ������ ���� <= ����������� �������." << endl <<
                "2. ����������� ������� ������ ���� >= 1." << endl <<
                "3. ���������� ������� ������ ���� >= 1." << endl << endl << endl << endl << endl;
            return 0;
        }

        rankOfMatrix = n;
        printMatrix(); // ����� ������� �� �����
        compute_rank(); // ������� �������� ����� �������

        cout << endl << "���������� ���� ������� : " << rankOfMatrix << endl; // ����� ����������� ������ ������ ��� �����

        auto end = chrono::steady_clock::now();
        auto elapsed_ms = chrono::duration_cast<std::chrono::milliseconds>(end - begin);
        cout << "����� ������ ��������� : " << elapsed_ms.count() << " ms\n";
    }

    return 0;
}
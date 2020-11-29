#include <iostream>
#include <omp.h>
#include <vector>
#include <fstream>
#include <thread>
#include <chrono>
#include <clocale> // Обязательно для функции setlocale()

using namespace std;

int n, quantityOfThreads;
thread* threads;
vector<bool> usedMatrixLines;
int rankOfMatrix;
int** matrix;
const double EPS = 1E-9;

void inputMatrix() {
    int elem;

    matrix = new int* [n]; // Инициализация матрицы
    for (int i = 0; i < n; i++) {
        matrix[i] = new int[n];
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << "Введите элемент (" << i << ";" << j << ") : ";
            cin >> elem;
            matrix[i][j] = elem;
        }
    }
}

void printMatrix() {
    std::cout << endl << "Полученная матрица : " << endl;

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

    cout << "Полученное значение n : " << n << endl << "Полученное значение количества потоков : " << quantityOfThreads << endl;
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

    // Одна прагма, добавленная перед циклом, сделает всю работу за нас.
    // Цикл будет выполняться в quantityOfThreads разных потоках.
    // При этом параметры цикла будут автоматически распределены между всеми потоками.
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

    setlocale(LC_ALL, "Russian"); // Для локализации

    // Начинаем отсчет времени со старта работы программы
    auto begin = std::chrono::steady_clock::now();

    // В случае, если пользователь не передает никаких параметров, делаем ручной ввод
    if (argc == 1) {
        cout << "Введите n (размерность матрицы) : ";
        cin >> n;
        cout << "Введите количество потоков : ";
        cin >> quantityOfThreads;

        if (quantityOfThreads > n || n < 1 || quantityOfThreads < 1) {
            cout << endl << "Некорректный ввод!" << endl <<
                "1. Количество потоков должно быть <= размерности матрицы." << endl <<
                "2. Размерность матрицы должна быть >= 1." << endl <<
                "3. Количество потоков должно быть >= 1." << endl << endl << endl << endl << endl;
            return 0;
        }

        usedMatrixLines = vector<bool>(n);
        rankOfMatrix = n;

        inputMatrix(); // ввод пользователем матрицы
        printMatrix(); // вывод матрицы на экран
        compute_rank(); // функция подсчета ранга матрицы

        cout << endl << "Полученный ранг матрицы: " << rankOfMatrix << endl; // Вывод результатов работы программы

        auto end = chrono::steady_clock::now();
        auto elapsed_ms = chrono::duration_cast<std::chrono::milliseconds>(end - begin);
        cout << "Время работы программы : " << elapsed_ms.count() << " ms\n";
    }
    else if (argc == 2) { // Если передан файл с матрицей - считываем его

        GetMatrixFromFile(argv[1]);

        if (quantityOfThreads > n || n < 1 || quantityOfThreads < 1) {
            cout << endl << "Некорректный ввод!" << endl <<
                "1. Количество потоков должно быть <= размерности матрицы." << endl <<
                "2. Размерность матрицы должна быть >= 1." << endl <<
                "3. Количество потоков должно быть >= 1." << endl << endl << endl << endl << endl;
            return 0;
        }

        rankOfMatrix = n;
        printMatrix(); // вывод полученной матрицы на экран
        compute_rank(); // функция подсчета ранга матрицы

        cout << endl << "Полученный ранг матрицы : " << rankOfMatrix << endl; // Вывод результатов работы программы

        auto end = chrono::steady_clock::now();
        auto elapsed_ms = chrono::duration_cast<std::chrono::milliseconds>(end - begin);
        cout << "Время работы программы : " << elapsed_ms.count() << " ms\n";
    }

    return 0;
}
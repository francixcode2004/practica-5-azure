#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>

#define N 100000000 // Tamaño del arreglo (100 millones de elementos)

using namespace std;

void run_sum(int num_threads) {
    long long sum = 0;
    
    // Inicializar datos (solo secuencial para evitar medir el I/O)
    vector<int> data(N);
    for (int i = 0; i < N; ++i) {
        data[i] = 1;
    }

    // Configurar el número de hilos para esta prueba
    omp_set_num_threads(num_threads);

    // Medición del tiempo
    auto start = chrono::high_resolution_clock::now();

    // Región Paralela: Usa reduction para manejar la comunicación de la suma
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < N; ++i) {
        sum += data[i];
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "Hilos: " << num_threads 
         << ", Tiempo: " << elapsed.count() << " s" 
         << ", Suma: " << sum << endl;
}

int main() {
    cout << "--- Evaluación de Paralelismo (N=" << N << ") ---" << endl;
    // Ejecución Secuencial (1 Hilo)
    run_sum(1);
    // Ejecución Paralela (2 Hilos - máximo de la VM)
    run_sum(2);
    return 0;
}
#include <iostream>
#include <vector>
#include <string>
#include <omp.h>
#include <chrono>
#define N 100000000 // Tamaño de la secuencia
const std::string PATTERN = "ATGC"; // Patrón a buscar
const int P_LEN = PATTERN.length();
using namespace std;
// Función para simular una secuencia grande
void generate_sequence(vector<char>& seq) {
    // Rellenamos con bases aleatorias para simular una carga de trabajo real
    // Insertamos el patrón cerca del final para forzar la búsqueda larga
    for (int i = 0; i < N - P_LEN; ++i) {
        seq[i] = "ATGC"[rand() % 4];
    }
    // Aseguramos que el patrón exista al final para que la búsqueda no falle
    for (int i = 0; i < P_LEN; ++i) {
        seq[N - 10000 + i] = PATTERN[i]; 
    }
}
void run_search(int num_threads, const char* schedule_type, int chunk_size) {
    vector<char> dna_sequence(N);
    generate_sequence(dna_sequence);
    long long first_index = -1; 
    omp_set_num_threads(num_threads);
    auto start = chrono::high_resolution_clock::now();
    //PCAM: Partitioning y Agglomeration
    #pragma omp parallel for schedule(dynamic, chunk_size)
    for (int i = 0; i < N - P_LEN; ++i) {
        // Si ya se encontró el índice, salimos rápidamente (optimización)
        if (first_index != -1) continue; 
        // Buscar el patrón: Tarea computacional (Partitioning)
        bool match = true;
        for (int j = 0; j < P_LEN; ++j) {
            if (dna_sequence[i + j] != PATTERN[j]) {
                match = false;
                break;
            }
        }
        // Si se encuentra, comunicar el resultado
        if (match) {
            // -------------------------------------------------------------
            // PUNTO CLAVE PCAM: Communication (Sección Crítica)
            // -------------------------------------------------------------
            #pragma omp critical
            {
                // Solo actualizamos si es la primera ocurrencia (índice más bajo)
                if (first_index == -1 || i < first_index) {
                    first_index = i;
                }
            }
        }
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "Hilos: " << num_threads 
         << ", Schedule: " << schedule_type 
         << " (" << chunk_size << ")"
         << ", Tiempo: " << elapsed.count() << " s" 
         << ", Posición: " << first_index << endl;
}
int main() {
    cout << "--- Búsqueda de Patron en ADN (" << PATTERN << ") ---" << endl;
    run_search(1, "static", N); 

    run_search(2, "guided", N/2); 

    run_search(2, "dynamic", 1); 

    run_search(2, "auto", 1); 

    return 0;
}
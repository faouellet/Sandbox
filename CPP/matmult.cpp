#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>

const int DIM = 256;

template <class T, int N> void generateZeroMatrix(T (&Mat)[N][N]) {
  for (unsigned i = 0; i < N; ++i)
    for (unsigned j = 0; j < N; ++j)
      Mat[i][j] = T();
}

template <class T, int N> void generateRandomMatrix(T (&Mat)[N][N]) {
  std::default_random_engine Engine;
  std::uniform_real_distribution<T> Dist(0, 1000);

  for (unsigned i = 0; i < N; ++i)
    for (unsigned j = 0; j < N; ++j)
      Mat[i][j] = Dist(Engine);
}

template <class T, int N>
void normalMatMult(const T(&A)[N][N], const T(&B)[N][N], T(&C)[N][N]) {
  for (unsigned i = 0; i < N; ++i)
    for (unsigned  j = 0; j < N; ++j)
      for (unsigned k = 0; k < N; ++k)
        C[i][j] += A[i][k] * B[k][j];
}

template <class T, int N>
void transposeMatMult(const T(&A)[N][N], const T(&B)[N][N], T(&C)[N][N]) {
  for (unsigned i = 0; i < N; ++i)
    for (unsigned j = 0; j < N; ++j)
      for (unsigned k = 0; k < N; ++k)
        C[i][j] += A[i][k] * B[j][k];
}

void coMatMult(const double *A, const double *B, double *C, int m, int n,
                    int p, int fdA, int fdB, int fdC) {
  if (m + n + p <= 64) { /* <= 16x16 matrices "on average" */
    int i, j, k;
    for (i = 0; i < m; ++i)
      for (k = 0; k < p; ++k) {
        double sum = 0;
        for (j = 0; j < n; ++j)
          sum += A[i * fdA + j] * B[j * fdB + k];
        C[i * fdC + k] += sum;
      }
  } else { /* divide and conquer */
    int m2 = m / 2, n2 = n / 2, p2 = p / 2;
    coMatMult(A, B, C, m2, n2, p2, fdA, fdB, fdC);
    coMatMult(A + n2, B + n2 * fdB, C, m2, n - n2, p2, fdA, fdB, fdC);
    coMatMult(A, B + p2, C + p2, m2, n2, p - p2, fdA, fdB, fdC);
    coMatMult(A + n2, B + p2 + n2 * fdB, C, m2, n - n2, p - p2, fdA, fdB,
                   fdC);
    coMatMult(A + m2 * fdA, B, C + m2 * fdC, m - m2, n2, p2, fdA, fdB,
                   fdC);
    coMatMult(A + m2 * fdA + n2, B + n2 * fdB, C + m2 * fdC, m - m2,
                   n - n2, p2, fdA, fdB, fdC);
    coMatMult(A + m2 * fdA, B + p2, C + m2 * fdC + p2, m - m2, n2, p - p2,
                   fdA, fdB, fdC);
    coMatMult(A + m2 * fdA + n2, B + p2 + n2 * fdB, C + m2 * fdC, m - m2,
                   n - n2, p - p2, fdA, fdB, fdC);
  }
}

int main() {
  double A[DIM][DIM];
  double B[DIM][DIM];
  double C[DIM][DIM];

  generateRandomMatrix(A);
  generateRandomMatrix(B);
  generateZeroMatrix(C);
  /////////////////////////////////////////////////////////////////////////////
  auto avant = std::chrono::high_resolution_clock::now();
  { normalMatMult(A, B, C); }
  auto apres = std::chrono::high_resolution_clock::now();
  auto temps = apres - avant;
  std::cout << "Normal. Ecoule: "
            << std::chrono::duration_cast<std::chrono::microseconds>(temps)
                   .count() << " us." << std::endl;
  /////////////////////////////////////////////////////////////////////////////
  generateZeroMatrix(C);
  for (int i = 0; i < DIM; ++i)
    for (int j = 0; j < DIM; ++j)
      B[i][j] = B[j][i];
  avant = std::chrono::high_resolution_clock::now();
  { transposeMatMult(A, B, C); }
  apres = std::chrono::high_resolution_clock::now();
  temps = apres - avant;
  std::cout << "Transpose. Ecoule: "
            << std::chrono::duration_cast<std::chrono::microseconds>(temps)
                   .count() << " us." << std::endl;
  /////////////////////////////////////////////////////////////////////////////
  double *D = new double[DIM*DIM];
  double *E = new double[DIM*DIM];
  double *F = new double[DIM*DIM];

  std::default_random_engine Engine;
  std::uniform_real_distribution<double> Dist(0, 1000);

  for (unsigned i = 0; i < DIM * DIM; ++i) {
    D[i] = Dist(Engine);
    E[i] = Dist(Engine);
    F[i] = 0.0;
  }

  avant = std::chrono::high_resolution_clock::now();
  { coMatMult(D, E, F, DIM, DIM, DIM, DIM, DIM, DIM); }
  apres = std::chrono::high_resolution_clock::now();
  temps = apres - avant;
  std::cout << "CO. Ecoule: "
            << std::chrono::duration_cast<std::chrono::microseconds>(temps)
                   .count() << " us." << std::endl;

  return 0;
}


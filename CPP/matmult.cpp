#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <random>

const int DIM = 256;

void normalMatMult(const double *A, const double *B, double *C, unsigned N) {
  for (unsigned i = 0; i < N; ++i)
    for (unsigned  j = 0; j < N; ++j)
      for (unsigned k = 0; k < N; ++k)
        C[i*N+j] += A[i*N+k] * B[k*N+j];
}

void transposeMatMult(const double *A, const double *B, double *C, unsigned N) {
  for (unsigned i = 0; i < N; ++i)
    for (unsigned j = 0; j < N; ++j)
      for (unsigned k = 0; k < N; ++k)
        C[i*N+j] += A[i*N+k] * B[j*N+k];
}

void coMatMult(const double *A, const double *B, double *C, int m, int n,
                    int p, int fdA, int fdB, int fdC) {
  if (m + n + p <= 48) { 
    int i, j, k;
    for (i = 0; i < m; ++i)
      for (k = 0; k < p; ++k) {
        double sum = 0;
        for (j = 0; j < n; ++j)
          sum += A[i * fdA + j] * B[j * fdB + k];
        C[i * fdC + k] += sum;
      }
  } else { 
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
  double *A = new double[DIM*DIM];
  double *B = new double[DIM*DIM];
  double *C = new double[DIM*DIM];

  std::default_random_engine Engine;
  std::uniform_real_distribution<double> Dist(0, 1000);

  for (unsigned i = 0; i < DIM * DIM; ++i) {
    A[i] = Dist(Engine);
    B[i] = Dist(Engine);
    C[i] = 0.0;
  }

  /////////////////////////////////////////////////////////////////////////////
  auto avant = std::chrono::high_resolution_clock::now();
  { normalMatMult(A, B, C, DIM); }
  auto apres = std::chrono::high_resolution_clock::now();
  auto temps = apres - avant;
  std::cout << "Normal. Ecoule: "
            << std::chrono::duration_cast<std::chrono::microseconds>(temps)
                   .count() << " us." << std::endl;
  /////////////////////////////////////////////////////////////////////////////
  for (int i = 0; i < DIM; ++i) {
    for (int j = 0; j < DIM; ++j) {
      B[i * DIM + j] = B[j * DIM + i];
      C[i * DIM + j] = 0.0;
    }
  }
  avant = std::chrono::high_resolution_clock::now();
  { transposeMatMult(A, B, C, DIM); }
  apres = std::chrono::high_resolution_clock::now();
  temps = apres - avant;
  std::cout << "Transpose. Ecoule: "
            << std::chrono::duration_cast<std::chrono::microseconds>(temps)
                   .count() << " us." << std::endl;
  /////////////////////////////////////////////////////////////////////////////
  for (int i = 0; i < DIM; ++i) 
    for (int j = 0; j < DIM; ++j)
      C[i * DIM + j] = 0.0;

  avant = std::chrono::high_resolution_clock::now();
  { coMatMult(A, B, C, DIM, DIM, DIM, DIM, DIM, DIM); }
  apres = std::chrono::high_resolution_clock::now();
  temps = apres - avant;
  std::cout << "CO. Ecoule: "
            << std::chrono::duration_cast<std::chrono::microseconds>(temps)
                   .count() << " us." << std::endl;

  delete[] A;
  delete[] B;
  delete[] C;

  return 0;
}


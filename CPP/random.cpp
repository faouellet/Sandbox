#include <random>

int main()
{
    double rand_vec[2][10];
    std::mt19937 gen(1337);
    
    for(int i = 0; i < 2; ++i)
        for(int j = 0; j < 10; ++j)
            rand_vec[i][j] = gen();
}


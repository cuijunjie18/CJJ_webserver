#include <exception>

int main()
{
    int x = 1;
    if (x){
        throw std::exception();
    }
    return 0;
}
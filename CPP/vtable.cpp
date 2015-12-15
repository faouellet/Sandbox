#include <iostream>
#include <thread>

struct Base
{
    virtual void Func() { std::cout << "Base::Func" << std::endl; }
};

struct Derived : public Base
{
    void Func() override { std::cout << "Derived::Func" << std::endl; }
};

struct VTable { void(*f)(); };

void HACK() { std::cout << "HACK" << std::endl; }
void FAKE() { std::cout << "FAKE" << std::endl; }

int main()
{
    Derived d;
    Base* bPtr = &d;

    VTable v1;
    v1.f = HACK;
    VTable* v1Ptr = &v1;

    VTable v2;
    v2.f = FAKE;
    VTable* v2Ptr = &v2;
    
    bPtr->Func();

    std::thread([&]() 
    {
        for (size_t i = 0; i < 100000; i++)
            memcpy(bPtr, &v1Ptr, sizeof(void*));
    }).detach();

    std::thread([&]() 
    { 
        for (size_t i = 0; i < 100000; i++)
            memcpy(bPtr, &v2Ptr, sizeof(void*));
    }).detach();
    
    bPtr->Func();
}

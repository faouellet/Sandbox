class Base { virtual void dummy() {} };

class Derived: public Base { int a; };

int main()
{
    Base * pba = new Derived;
    Base * pbb = new Base;
    Derived * pd;

    pd = (Derived*)(pba);
}


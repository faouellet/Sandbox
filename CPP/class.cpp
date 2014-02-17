class Foo
{
private:
    float m_f;

public:
    Foo();
    float Bar(float val);
    int Baz() const;
};

Foo::Foo() : m_f(0.f) { }

float Foo::Bar(float val) { return val + m_f; }

int Foo::Baz() const { return 42; }

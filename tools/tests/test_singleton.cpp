#include "Singleton.h"

#include <iostream>

using namespace std;

class A : public Singleton_v2<A> {
 private:
  friend class Singleton_v2<A>;
  A(int i = 0) { cout << "constructor A(int i), i = " << i << endl; }

 public:
  ~A() { cout << "deconstructor ~A()" << endl; }
};

class B {
 public:
  B() { cout << "constructor B()" << endl; }
  ~B() { cout << "deconstructor ~B()" << endl; }
  void do_sth() { cout << "do something" << endl; }
};

class C : public Singleton<C> {
 private:
  friend class Singleton<C>;
  C() { cout << "constructor C()" << endl; }
  ~C() { cout << "deconstructor ~C()" << endl; }
};

class D {
  TO_SINGLETON(D);
};

int main() {
  cout << "==========Singleton==========" << endl;
  {
    A& inst1 = A::GetInstance(1);
    A& inst2 = A::GetInstance();
    A& inst3 = A::GetInstance(2);
    cout << "inst1: " << &inst1 << endl;
    cout << "inst2: " << &inst2 << endl;
    cout << "inst3: " << &inst3 << endl;

    B& inst4 = Singleton<B>::GetInstance();
    B& inst5 = Singleton<B>::GetInstance();
    cout << "&inst4: " << &inst4 << endl;
    cout << "&inst5: " << &inst5 << endl;

    C& inst6 = C::GetInstance();
    C& inst7 = C::GetInstance();
    cout << "&inst6: " << &inst6 << endl;
    cout << "&inst7: " << &inst7 << endl;

    D& inst8 = D::GetInstance();
    D& inst9 = D::GetInstance();
    cout << "&inst8: " << &inst8 << endl;
    cout << "&inst9: " << &inst9 << endl;
  }

  return 0;
}
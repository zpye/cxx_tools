#include "SharedPtr.h"

#include <iostream>

using namespace std;

class TestB {
 public:
  TestB() {
    cnt = count++;
    cout << "TestB() " << cnt << endl;
  }

  virtual ~TestB() { cout << "~TestB() " << cnt << endl; }

  virtual void Print() { cout << "B::Print()" << endl; }

 protected:
  static size_t count;
  size_t cnt;
};

class TestD : public TestB {
 public:
  TestD() {
    cnt = count++;
    cout << "TestD() " << cnt << endl;
  }

  virtual ~TestD() { cout << "~TestD() " << cnt << endl; }

  virtual void Print() override { cout << "D::Print()" << endl; }
};

size_t TestB::count = 0;

int main() {
  cout << "==========simple SharedPtr==========" << endl;
  {
    SharedPtr<TestB> sp1(new TestB);
    SharedPtr<TestB> sp2(sp1);
    SharedPtr<TestB> sp3(move(sp1));

    SharedPtr<int> sp4(new int(0));
    SharedPtr<int[]> sp5(new int[10]{1});
    SharedPtr<TestD[]> sp6(new TestD[5]);

    SharedPtr<TestB> sp7(new TestD);
  }

  cout << "==========function==========" << endl;
  {
    SharedPtr<TestB> sp1(new TestB);
    TestB* p1 = sp1.Get();
    TestB& b1 = *sp1;
    sp1.Reset();
    sp1.Reset(new TestD);
    sp1.Swap(sp1);
    sp1->Print();

    SharedPtr<void> sp2(sp1);
    sp1.Reset();
  }

  return 0;
}
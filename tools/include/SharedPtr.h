#ifndef CXX_TOOLS_SHARED_PTR_H
#define CXX_TOOLS_SHARED_PTR_H

#include <atomic>
#include <mutex>
#include <type_traits>
#include <utility>

class RefBlockBase {
 public:
  RefBlockBase() : usr_count(1) {}

  void IncCnt() { ++usr_count; }

  void DecCnt() {
    if (--usr_count == 0) {
      DeleteRef();
      DeleteThis();
    }
  }

 protected:
  virtual void DeleteRef() = 0;

  virtual void DeleteThis() = 0;

 private:
  std::atomic_size_t usr_count;
};

template <typename T>
class RefBlock : public RefBlockBase {
 public:
  explicit RefBlock(T* ptr) : ref_pointer(ptr) {}

 protected:
  virtual void DeleteRef() override { delete ref_pointer; }

  virtual void DeleteThis() override { delete this; }

 private:
  T* ref_pointer;
};

template <typename T>
class RefBlock<T[]> : public RefBlockBase {
 public:
  explicit RefBlock(T* ptr) : ref_pointer(ptr) {}

 protected:
  virtual void DeleteRef() override { delete[] ref_pointer; }

  virtual void DeleteThis() override { delete this; }

 private:
  T* ref_pointer;
};

template <typename T>
class SharedPtr {
 public:
  using Type = typename std::remove_extent<T>::type;

  explicit SharedPtr(Type* ptr = nullptr) : pointer(ptr), ref_block(nullptr) {
    if (ptr != nullptr) {
      ref_block = new RefBlock<T>(ptr);
    }
  }

  template <typename U>
  SharedPtr(const SharedPtr<U>& sp) {
    this->CopyConstruct(sp);
  }

  template <typename U>
  SharedPtr& operator=(const SharedPtr<U>& sp) {
    SharedPtr(sp).Swap(*this);
    return (*this);
  }

  template <typename U>
  SharedPtr(SharedPtr<U>&& sp) {
    this->MoveConstruct(std::move(sp));
  }

  template <typename U>
  SharedPtr& operator=(SharedPtr<U>&& sp) {
    SharedPtr(std::move(sp)).Swap(*this);
    return (*this);
  }

  ~SharedPtr() {
    if (ref_block != nullptr) {
      ref_block->DecCnt();
    }
  }

  Type* Get() const noexcept { return pointer; }

  template <typename _T = T, std::enable_if_t<!std::is_void_v<_T>, int> = 0>
  _T& operator*() const noexcept {
    return (*Get());
  }

  template <typename _T = T, std::enable_if_t<!std::is_array_v<_T>, int> = 0>
  _T* operator->() const noexcept {
    return Get();
  }

  // TODO: add operator[]

  void Swap(SharedPtr& sp) noexcept {
    std::swap(pointer, sp.pointer);
    std::swap(ref_block, sp.ref_block);
  }

  void Reset(Type* ptr = nullptr) { SharedPtr(ptr).Swap(*this); }

 private:
  template <typename U>
  void CopyConstruct(const SharedPtr<U>& sp) {
    if (sp.ref_block) {
      sp.ref_block->IncCnt();
    }

    this->pointer = sp.pointer;
    this->ref_block = sp.ref_block;
  }

  template <typename U>
  void MoveConstruct(SharedPtr<U>&& sp) {
    this->pointer = sp.pointer;
    this->ref_block = sp.ref_block;

    sp.pointer = nullptr;
    sp.ref_block = nullptr;
  }

 private:
  Type* pointer;
  RefBlockBase* ref_block;

  template <typename T0>
  friend class SharedPtr;
};

#endif  // CXX_TOOLS_SHARED_PTR_H
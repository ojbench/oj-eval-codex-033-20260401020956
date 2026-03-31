#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include <cstddef>
#include <new>

namespace sjtu {

template <typename T>
class list {
protected:
  struct node {
    node *prev;
    node *next;
    T *data; // nullptr for sentinel
    // construct sentinel
    node() : prev(this), next(this), data(nullptr) {}
    // construct data node by copying value
    explicit node(const T &value) : prev(nullptr), next(nullptr), data(nullptr) {
      data = static_cast<T *>(::operator new(sizeof(T)));
      new (data) T(value);
    }
    ~node() {
      if (data) {
        data->~T();
        ::operator delete(static_cast<void *>(data));
        data = nullptr;
      }
    }
  };

protected:
  node *sent;    // sentinel node
  size_t n;      // size

  node *insert(node *pos, node *cur) {
    // insert cur before pos
    cur->next = pos;
    cur->prev = pos->prev;
    pos->prev->next = cur;
    pos->prev = cur;
    return cur;
  }

  node *erase(node *pos) {
    // unlink pos (do not delete)
    node *nxt = pos->next;
    pos->prev->next = pos->next;
    pos->next->prev = pos->prev;
    pos->prev = pos->next = nullptr;
    return pos;
  }

public:
  class const_iterator;
  class iterator {
    friend class list;
    friend class const_iterator;
  private:
    node *cur;
    const list *owner;
    iterator(node *p, const list *o) : cur(p), owner(o) {}
  public:
    iterator() : cur(nullptr), owner(nullptr) {}

    iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }
    iterator &operator++() { if (cur) cur = cur->next; return *this; }
    iterator operator--(int) { iterator tmp = *this; --(*this); return tmp; }
    iterator &operator--() { if (cur) cur = cur->prev; return *this; }

    T &operator*() const { return *(cur->data); }
    T *operator->() const noexcept { return cur->data; }

    bool operator==(const iterator &rhs) const { return cur == rhs.cur; }
    bool operator==(const const_iterator &rhs) const { return cur == rhs.cur; }
    bool operator!=(const iterator &rhs) const { return cur != rhs.cur; }
    bool operator!=(const const_iterator &rhs) const { return cur != rhs.cur; }
  };

  class const_iterator {
    friend class list;
  private:
    const node *cur;
    const list *owner;
    const_iterator(const node *p, const list *o) : cur(p), owner(o) {}
  public:
    const_iterator() : cur(nullptr), owner(nullptr) {}
    const_iterator(const iterator &it) : cur(it.cur), owner(it.owner) {}

    const_iterator operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; }
    const_iterator &operator++() { if (cur) cur = cur->next; return *this; }
    const_iterator operator--(int) { const_iterator tmp = *this; --(*this); return tmp; }
    const_iterator &operator--() { if (cur) cur = cur->prev; return *this; }

    const T &operator*() const { return *(cur->data); }
    const T *operator->() const noexcept { return cur->data; }

    bool operator==(const const_iterator &rhs) const { return cur == rhs.cur; }
    bool operator==(const iterator &rhs) const { return cur == rhs.cur; }
    bool operator!=(const const_iterator &rhs) const { return cur != rhs.cur; }
    bool operator!=(const iterator &rhs) const { return cur != rhs.cur; }
  };

  // no additional definitions needed

  list() : sent(new node()), n(0) {}

  list(const list &other) : sent(new node()), n(0) {
    for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
      push_back(*it);
    }
  }

  virtual ~list() {
    clear();
    delete sent;
    sent = nullptr;
  }

  list &operator=(const list &other) {
    if (this == &other) return *this;
    clear();
    for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
      push_back(*it);
    }
    return *this;
  }

  const T &front() const { return *(sent->next->data); }
  const T &back() const { return *(sent->prev->data); }
  T &front() { return *(sent->next->data); }
  T &back() { return *(sent->prev->data); }

  class begin_proxy { };

  iterator begin() { return iterator(sent->next, this); }
  const_iterator cbegin() const { return const_iterator(sent->next, this); }
  iterator end() { return iterator(sent, this); }
  const_iterator cend() const { return const_iterator(sent, this); }

  virtual bool empty() const { return n == 0; }
  virtual size_t size() const { return n; }

  virtual void clear() {
    node *p = sent->next;
    while (p != sent) {
      node *nx = p->next;
      erase(p); // unlink
      delete p; // free
      p = nx;
    }
    n = 0;
    sent->next = sent->prev = sent;
  }

  virtual iterator insert(iterator pos, const T &value) {
    node *cur = new node(value);
    insert(pos.cur, cur);
    ++n;
    return iterator(cur, this);
  }

  virtual iterator erase(iterator pos) {
    node *p = pos.cur;
    node *nxt = p->next;
    erase(p);
    delete p;
    --n;
    return iterator(nxt, this);
  }

  void push_back(const T &value) { insert(iterator(sent, this), value); }
  void pop_back() { if (n) erase(iterator(sent->prev, this)); }
  void push_front(const T &value) { insert(iterator(sent->next, this), value); }
  void pop_front() { if (n) erase(iterator(sent->next, this)); }
};

} // namespace sjtu

#endif // SJTU_LIST_HPP

#ifndef SPARSE_MATRIX_H
#define SPARSE_MATRIX_H

#include <vector>

// Matrix entry.
template <typename T>
class Entry {
public:
    Entry(int i, int j): i_(i), j_(j), v_(), is_empty_(true) {}
    Entry(int i, int j, T v): i_(i), j_(j), v_(v), is_empty_(false) {}

    int I() const { return i_; }
    int J() const { return i_; }
    T V() const { return v_; }
    bool IsEmpty() const { return is_empty_; }

    void SetEmpty() { is_empty_ = true; v_ = T(); }
    void SetV(T v) { is_empty_ = false; v_ = v; }

private:
    int i_;         // row index
    int j_;         // column index
    T v_;           // entry value
    bool is_empty_; // true if the entry is empty
};

// Sparse matrix with CSR (Compressed Sparse Row) format.
template <typename T> 
class SparseMatrix {
public:
    SparseMatrix(int m, int n): m_(m), n_(n), a_(), ia_(m + 1), ja_() {}

    int M() const { return m_; }
    int N() const { return n_; }
    std::vector<Entry<T>*> A() const { return a_; }
    std::vector<int> IA() const { return ia_; }
    std::vector<int> JA() const { return ja_; }

    // Get an element at (i, j).
    Entry<T>* Get(int i, int j) const {
        if (i < 0 || i >= m_ || j < 0 || j >= n_) {
            throw std::out_of_range("invalid coordinates (i, j)");
        }
        for (int pos = ia_[i]; pos < ia_[i + 1]; ++pos) {
            if (ja_[pos] == j) {
                return a_[pos];
            }
        }
        // If there's no match, return a new empty entry.
        return new Entry<T>(i, j);
    }

    // Set an element at (i, j) with v.
    void Set(int i, int j, T v) {
        int pos = ia_[i];
        while (pos < ia_[i + 1] && ja_[pos] != j) {
            pos++;
        }
        if (ja_[pos] != j) {
            // If the argument position is empty, insert a new entry.
            // Same as in Get(), if the correct column is found, there is
            // a nonzero entry.
            a_.insert(a_.begin() + pos, new Entry<T>(i, j, v));
            ja_.insert(ja_.begin() + pos, j);
            for (int r = i + 1; r <= m_; ++r) {
                ia_[r] += 1;
            }
        } else {
            // There is already a value at (i, j); set a different value.
            for (int pos = ia_[i]; pos < ia_[i + 1]; ++pos) {
                if (ja_[pos] == j) {
                    a_[pos]->SetV(v);
                    return;
                }
            }
        }
    }

private:
    int m_; // number of rows
    int n_; // number of columns

    std::vector<Entry<T>*> a_;  // nonzero entries
    std::vector<int> ia_;       // indices in JA for each row
    std::vector<int> ja_;       // column indices of elements in A
};

#endif
/*
 * Copyright (C) 2011 Simon A. Berger
 *
 *  This program is free software; you may redistribute it and/or modify its
 *  under the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 */

#ifndef __ivy_mike__tdmatrix_h
#define __ivy_mike__tdmatrix_h
#include <cstddef>

namespace ivy_mike {
template<typename T>
class odmatrix {
public:
    typedef T value_type;
    typedef T* iterator;
private:
    T* m_base;
    size_t m_size;

    bool m_own;
public:
    odmatrix( T* base, size_t size ) : m_base(base), m_size(size), m_own(false) {}
    
    T &operator[](ptrdiff_t a) {
        return m_base[a];
    }
    
    const T &operator[](ptrdiff_t a) const {
        return m_base[a];
    }
    
    iterator begin() {
        return m_base;
    }
    iterator end() {
        return m_base + m_size;
    }
    
    size_t size() const {
        return m_size;
    }
    
};

template<typename T>
class tdmatrix {
public:
    typedef odmatrix<T> row_type;
    typedef T value_type;
    
    typedef T* iterator;
private:
    
    T* m_base;
    const size_t m_asize;
    const size_t m_bsize;
    
    bool m_own;

public:
    class row_iterator {
        T *m_base;
        size_t m_stride;
        
    public:
        row_iterator( T* base, size_t stride ) : m_base(base), m_stride(stride) {}
        row_iterator &operator++() {
            m_base += m_stride;
            return *this;
        }
        
        odmatrix<T> operator*() {
            return odmatrix<T>(m_base, m_stride);
        }
        
        bool operator==( const row_iterator &other ) {
            return m_base == other.m_base && m_stride == other.m_stride;
        }
        
        bool operator!=( const row_iterator &other ) {
            return !((*this) == other);
        }
        
    };

    tdmatrix( size_t asize, size_t bsize ) : m_asize(asize), m_bsize(bsize) {
        m_base = new T[num_elements()];
        m_own = true;
        
        
    }
    
    ~tdmatrix() {
        delete[] m_base;
    }
    
    size_t num_elements() {
        return m_asize * m_bsize;
    }
    
    ptrdiff_t b_offset( ptrdiff_t b ) {
        return m_asize * b;
    }
    
    row_type operator[](ptrdiff_t b) {
        return row_type(m_base + (b * m_asize), m_asize );
    }
    
    const odmatrix<T> operator[](ptrdiff_t b) const {
        return row_type(m_base + (b * m_asize), m_asize );
    }
    
    row_iterator row_begin() {
        return row_iterator(m_base, m_asize);
    }
    
    row_iterator row_end() {
        return row_iterator(m_base + num_elements(), m_asize);
    }
    
    // WARNING: the default iterators work on single elements, not rows! This is inconsistend with operator[]
    iterator begin() {
        return m_base;
    }
    iterator end() {
        return m_base + num_elements();
    }
    
    size_t size() const {
        return m_bsize;
    }
    
};
} // namespace ivy_mike


#endif
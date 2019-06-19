/***************************************************//**
 * @file    Polynomial.h
 * @date    February 2011
 * @author  Ocean Optics, Inc.
 *
 * The template type for this class must be either
 * double or float.  Any other type may cause unexpected
 * results or a failure to compile.
 *
 * The implementation for this class is also contained
 * in the header file so that it is compiled consistently
 * when needed.  Otherwise, there could be linker
 * issues.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2014, Ocean Optics Inc
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************/
#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include <vector>

namespace seabreeze {
    template <class T>
    class Polynomial {
    public:
        Polynomial(std::vector<T> *coefficients);
        Polynomial(T *coefficients, unsigned int length);
        ~Polynomial();
        T evaluate(T x);
    private:
        std::vector<T> *coefficients;
    };

    template <class T>
    Polynomial<T>::Polynomial(std::vector<T> *coeffs) {
        unsigned int i;

        if(NULL == coeffs) {
            this->coefficients = new std::vector<T>(0);
        } else {
            this->coefficients = new std::vector<T>(coeffs->size());
        }
        for(i = 0; i < coeffs->size(); i++) {
            (*(this->coefficients))[i] = (*coeffs)[i];
        }
    }

    template <class T>
    Polynomial<T>::Polynomial(T *coeffs, unsigned int length) {
        unsigned int i;

        if(NULL == coeffs) {
            this->coefficients = new std::vector<T>(0);
        } else {
            this->coefficients = new std::vector<T>(length);
        }
        for(i = 0; i < length; i++) {
            (*(this->coefficients))[i] = coeffs[i];
        }
    }


    template <class T>
    Polynomial<T>::~Polynomial() {
        delete this->coefficients;
    }


    template <class T>
    T Polynomial<T>::evaluate(T x) {
        T acc;
        T retval = 0.0;
        unsigned int order;

        if(NULL == this->coefficients || 0 == this->coefficients->size()) {
            return 0;
        }

        retval = this->coefficients->at(0); /* Initialize to A */
        acc = 1;
        for(order = 1; order < this->coefficients->size(); order++) {
            acc *= x;
            retval += this->coefficients->at(order) * acc; /* Add x^n term (Bx^2, Cx^3, ...) */
        }
        return retval;
    }

}

#endif /* POLYNOMIAL_H */

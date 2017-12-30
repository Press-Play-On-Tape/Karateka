/*
 *  StackArray.h
 *
 *  Library implementing a generic, dynamic stack (array version).
 *
 *  ---
 *
 *  Copyright (C) 2010  Efstathios Chatzikyriakidis (contact@efxa.org)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *  ---
 *
 *  Version 1.0
 *
 *    2010-09-25  Efstathios Chatzikyriakidis  <contact@efxa.org>
 *
 *      - added resize(): for growing, shrinking the array size.
 *
 *    2010-09-23  Efstathios Chatzikyriakidis  <contact@efxa.org>
 *
 *      - added exit(), blink(): error reporting and handling methods.
 *
 *    2010-09-20  Alexander Brevig  <alexanderbrevig@gmail.com>
 *
 *      - added setPrinter(): indirectly reference a Serial object.
 *
 *    2010-09-15  Efstathios Chatzikyriakidis  <contact@efxa.org>
 *
 *      - initial release of the library.
 *
 *  ---
 *
 *  For the latest version see: http://www.arduino.cc/
 */

// header defining the interface of the source.
#ifndef _STACKARRAY_H
#define _STACKARRAY_H

// include Arduino basic header.
#include <Arduino.h>

// the definition of the stack class.
template<typename T>
class StackArray {
  public:
    // init the stack (constructor).
    StackArray ();

    // clear the stack (destructor).
    ~StackArray ();

    // push an item to the stack.
    void push (const T i);
    void push (const T i1, const T i2);
    void push (const T i1, const T i2, const T i3);
    void pushArr (const T i[], const uint8_t iSize);

    // insert an item at bottom of stack.
    void insert (const T i);

    // clear the stack.
    void clear ();

    // pop an item from the stack.
    T pop ();

    // get an item from the stack.
    T peek () const;

    // check if the stack is empty.
    bool isEmpty () const;

    // get the number of items in the stack.
    int count () const;

    // check if the stack is full.
    bool isFull () const;

  private:
    // resize the size of the stack.
    void resize (const int s);

    // exit report method in case of error.
    void exit (const char * m) const;

    // led blinking method in case of error.
    void blink () const;

    // the initial size of the stack.
    static const int initialSize = 20;

    T * contents;    // the array of the stack.
    int size;        // the size of the stack.
    int top;         // the top index of the stack.
};

// init the stack (constructor).
template<typename T>
StackArray<T>::StackArray () {
  size = 0;       // set the size of stack to zero.
  top = 0;        // set the initial top index of the stack.

  // allocate enough memory for the array.
  contents = (T *) malloc (sizeof (T) * initialSize);

  // set the initial size of the stack.
  size = initialSize;
}

// clear the stack (destructor).
template<typename T>
StackArray<T>::~StackArray () {
  free (contents); // deallocate the array of the stack.

  contents = NULL; // set stack's array pointer to nowhere.
  size = 0;        // set the size of stack to zero.
  top = 0;         // set the initial top index of the stack.
  resize(initialSize);
}

// resize the size of the stack.
template<typename T>
void StackArray<T>::resize (const int s) {

  if (s > initialSize) {
  // reallocate enough memory for the array.
  contents = (T *) realloc (contents, sizeof (T) * s);

  // set the new size of the stack.
  size = s;
  }
}

// push an item to the stack.
template<typename T>
void StackArray<T>::push (const T i) {
  // check if the stack is full.
  if (isFull ())
    // double size of array.
    resize (size * 2);

  // store the item to the array.
  contents[top++] = i;
}

// push an item to the stack.
template<typename T>
void StackArray<T>::push (const T i1, const T i2) {
  // check if the stack is full.
  if (isFull ())
    // double size of array.
    resize (size * 2);

  // store the item to the array.
  contents[top++] = i1;
  contents[top++] = i2;
}

// push an item to the stack.
template<typename T>
void StackArray<T>::push (const T i1, const T i2, const T i3) {
  // check if the stack is full.
  if (isFull ())
    // double size of array.
    resize (size * 2);

  // store the item to the array.
  contents[top++] = i1;
  contents[top++] = i2;
  contents[top++] = i3;
}

template<typename T>
void StackArray<T>::pushArr(const T i[], const uint8_t iSize) {
  // check if the stack is full.
  if (isFull ())
    // double size of array.
    resize ((size + iSize) * 2);

  // store the item to the array.
  for (uint8_t x = 0; x < iSize; ++x) {
    contents[top++] = i[x];
 }

}

// insert an item to the stack.
template<typename T>
void StackArray<T>::insert (const T i) {
  // check if the stack is full.
  if (isFull ())
    // double size of array.
    resize (size * 2);

	top++;
	for (uint8_t i = top; i > 0; i--) {
		contents[i] = contents[i - 1];
		} 
  // store the item to the array.
  contents[0] = i;
}

// clear the stack.
template<typename T>
void StackArray<T>::clear () {
	top = 0;
	size = 2;
	resize(2);
}

// pop an item from the stack.
template<typename T>
T StackArray<T>::pop () {

  // fetch the top item from the array.
  T item = contents[--top];

  // shrink size of array if necessary.
  if (!isEmpty () && (top <= size / 4))
    resize (size / 2);

  // return the top item from the array.
  return item;
}

// get an item from the stack.
template<typename T>
T StackArray<T>::peek () const {
  // get the top item from the array.
  return contents[top - 1];
}

// check if the stack is empty.
template<typename T>
bool StackArray<T>::isEmpty () const {
  return top == 0;
}

// check if the stack is full.
template<typename T>
bool StackArray<T>::isFull () const {
  return top == size;
}

// get the number of items in the stack.
template<typename T>
int StackArray<T>::count () const {
  return top;
}

#endif // _STACKARRAY_H

/*
Lists that can be manipulated qucikly and easily. With influence of Java ArrayLists (java.util.ArrayList).
Author: Chaiyawat Nunes (chaiteanunes@gmail.com) (https://github.com/ChaiTeaNunes).
Lisence: GNU GPL v2.0 (http://www.gnu.org/licenses/gpl.txt).
*/

#ifndef __LIST__
#define __LIST__

#include <typeinfo>

/**
 * Lists that can be manipulated qucikly and easily. With influence of Java ArrayLists (java.util.ArrayList).
 *
 * @author	Chaiyawat Nunes	thaiberius.code@gmail.com
 * @version	1.0
 */
template <typename TYPE>
class List {
private:

	/**
	 * The object that will be manipulated for the list.
	 */
	TYPE * objects;

	/**
	 * Size of the list.
	 */
	int size;

public:

	/**
	 * Returns the size of the list, which is crucial for sorting.
	 *
	 * @return	the size of the list
	 */
	int getSize() {
		return size;
	}

	/**
	 * Returns an object that can be manipulated and accessed.
	 *
	 * @param	index	the index to locate the object within the list
	 * @return	the object at the give index of the list
	 */
	TYPE &operator[](int index) {
		return objects[index];
	}

	/**
	 * Returns an object that can be manipulated and accessed.
	 *
	 * @param	index	the index to locate the object within the list
	 * @return	the object at the give index of the list
	 */
	TYPE get(int index) {
		return objects[index];
	}

	/**
	 * Sets the size of the list, trimming objects from the end and seting new objects to 0.
	 *
	 * @param	newSize	the new size of the list
	 */
	void setSize(int newSize) {
		TYPE * newObjects = new TYPE[newSize];
		if (objects != nullptr) {
			for (int i = 0; i < newSize && i < getSize(); i++) {
				newObjects[i] = get(i);
			}
			delete[] objects;
		}
		objects = newObjects;
		size = newSize;
	}

	/**
	 * Sets the object at the given index of the list.
	 *
	 * @param	index	the index to locate the object within the list
	 * @param	value	the new value for the selected object
	 */
	void set(int index, TYPE value) {
		objects[index] = value;
	}

	/**
	 * Adds an object to the end of the list.
	 *
	 * @param	value	the new value for the new object
	 */
	void add(TYPE value) {
		setSize(getSize() + 1);
		set(getSize() - 1, value);
	}

	/**
	 * Adds an object at the given index and moves the objects after the index of the list.
	 *
	 * @param	index	the location of the new object
	 */
	void insert(int index, TYPE value) {
		setSize(getSize() + 1);
		for (int i = getSize() - 1; i > index; i--) {
			set(i, get(i - 1));
		}
		set(index, value);
	}

	/**
	 * Removes and object at the given index and moves the object after the index of the list.
	 *
	 * @param	index	the location of the new object
	 */
	void removeAt(int index) {
		for (int i = index; i < getSize(); i++) {
			set(i, get(i - 1));
		}
		setSize(getSize() - 1);
	}

	/**
	 * Swaps two objects in the list.
	 *
	 * @param	index1	first index to swap
	 * @param	index2	second index to swap
	 */
	void swap(int index1, int index2) {
		TYPE temp = get(index2);
		set(index2, get(index1));
		set(index1, temp);
	}

	/**
	 * Moves an object to a new place in the list.
	 *
	 * @param	start	the beginning location of the object
	 * @param	end		the ending location of the object
	 */
	void move(int start, int end) {
		insert(end, get(start));
		removeAt(start);
	}

	/**
	 * Moves an object to the front of the list.
	 *
	 * @param	index	the location of the object to move to the front
	 */
	void moveToFront(int index) {
		move(index, 0);
	}

	/**
	 * Checks of the list is empty or not.
	 *
	 * @returns	true if the size is 0
	 */
	bool isEmpty() {
		return getSize() == 0;
	}

	/**
	 * Checks if an object in the list is equal to the given value.
	 *
	 * @param	index	the location of the object to compare
	 * @param	value	the value to compare
	 * @returns	true if the two objects have the same value
	 */
	bool equals(int index, TYPE value) {
		return get(index) == value;
	}

	/**
	 * Flips all of the objects in the list.
	 */
	void flip() {
		for (int i = 0; i < getSize(); i++) {
			set(i, getSize() - 1 - i);
		}
	}

	/**
	 * Clears the list.
	 */
	void clear(){
		if (objects != nullptr) {
			delete[] objects;
			objects = nullptr;
			setSize(0);
		}
	}

	/**
	 * O(n) and o(n^2) Bubble Sort.
	 */
	void bubbleSort() {
		if (getSize() > 1){
			for (int i = 0; i < getSize(); i++) {
				for (int j = 0; j < getSize() - j; j++) {
					if (get(j) > get(j + 1)) {
						swap(j, j + 1);
					}
				}
			}
		}
	}

	/**
	 * O(n) and o(n^2) Insertion Sort.
	 */
	void insertionSort() {
		for (int i = 0; i < getSize(); i++) {
			TYPE temp = get(i);
			int j;
			for (j = i - 1; j >= 0 && temp < get(j); j--) {
				set(j + 1, get(j));
			}
			set(j + 1, temp);
		}
	}

	/**
	 * Default Constructor.
	 */
	List() : objects(nullptr), size(0) { }

	/**
	 * Parameterized Contructor.
	 *
	 * @param	size	the size of the list
	 */
	List(int size) : objects(nullptr), size(size) {
		for(int i = 0; i < size; i++) {
			add();
		}
	}

	/**
	 * Deconstructor.
	 */
	~List() {
		clear();
	}
};

#endif

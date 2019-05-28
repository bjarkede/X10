#include "bdeque.hpp"
#include "X10const.hpp"

Custom_deque::Custom_deque()
{
	data = (char unsigned *)malloc(sizeof(unsigned char) * 60);
	_capacity = 60;
	clear();
}

bool Custom_deque::compare_last_four(Custom_deque& other) {
	if (_size < 4) return false;
	
	if (data[_size-4] != other.get_element_at(0)) return false;
	if (data[_size-3] != other.get_element_at(1)) return false;
	if (data[_size-2] != other.get_element_at(2)) return false;
	if (data[_size-1] != other.get_element_at(3)) return false;
	
	return true;
}

Custom_deque::Custom_deque(int size)
{
	data = (char unsigned *)malloc(sizeof(unsigned char) * size);
	_capacity = size;
	clear();
}

bool Custom_deque::is_symmetrical() {
	int halfway = _size/2;
	for(int i = 0; i < halfway; i++) {
		if(get_element_at(i) != get_element_at(i + halfway))
		return false;
	}
	
	return true;
}

bool Custom_deque::is_empty()
{
	return _size == 0;
}

int Custom_deque::size()
{
	return _size;
}

bool Custom_deque::equals(Custom_deque& other)
{
	if(other.size() != _size)
	return false;

	for(int i = 0; i < _size; i++)
	if (other.get_element_at(i) != get_element_at(i))
	return false;

	return true;
}

unsigned char Custom_deque::get_element_at(int index)
{
	int newIndex = _front + index;

	if (index >= _size)
	return -1;

	else if (newIndex >= _capacity)
	newIndex = newIndex - _capacity;

	return data[newIndex];
}


void Custom_deque::clear()
{
	for(int i = 0; i < _capacity; i++)
	{
		data[i] = 0;
	}
	
	_size = 0;
	_index = 0;
	_front = 0;
}

void Custom_deque::push_back(unsigned char item)
{
	data[_index] = item;
	if (_size != 0 && _index == _front)
	_front++;
	
	incrementIndex();
	_size++;
	if (_size >= _capacity) _size = _capacity;
}

unsigned char Custom_deque::pop_front()
{
	if(_index == _front)
	return 255;
	if(_size == 0)
	return 255;
	
	unsigned char value = data[_front];
	incrementIndex();
	_size--;
	return value;
}

unsigned char Custom_deque::pop_back()
{
	unsigned char value = data[_index-1];
	decrementIndex();
	_size--;
	return value;
}
unsigned char Custom_deque::peek_front()
{
	return data[_front];
}

void Custom_deque::incrementIndex()
{
	_index++;
	if (_index >= _capacity) _index = 0;
	if (_front >= _capacity) _front = 0;
}

void Custom_deque::decrementIndex()
{
	_index--;
	if(_index < 0)
	_index = _size;
}

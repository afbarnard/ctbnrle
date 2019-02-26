/* Continuous Time Bayesian Network Reasoning and Learning Engine
 * Copyright (C) 2009 The Regents of the University of California
 *
 * see docs/AUTHORS for contributor list
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cmath>
#include <cstdio>
#include <stdexcept>

#include "samplequeue.h"
#include "defines.h"

using namespace std;

namespace ctbn {

bool operator==(const SampleQueue::Event & e1, const SampleQueue::Event & e2) {
  return e1.var == e2.var && e1.value == e2.value && e1.time == e2.time;
}

SampleQueue::SampleQueue(int maxn) : heap(NULL), places(NULL), capacity(maxn), n(0) {
	heap = new Event[maxn];
	places = new int[maxn];
}

SampleQueue::~SampleQueue() {
	delete[] heap;
	delete[] places;
}

int SampleQueue::Capacity() {
  return capacity;
}

int SampleQueue::Length() {
  return n;
}

SampleQueue::Event & SampleQueue::At(int index) {
  if (index < 0 || index >= n) {
    char buf[128];
    snprintf(buf, 128, "Index out of range: !(0 <= %d < %d)", index, n);
    throw out_of_range(buf);
  }
  return heap[index];
}

bool SampleQueue::Head(Event &e) {
	if (n<=0) return false;
	e = heap[0];
	return true;
}

void SampleQueue::Add(const Event &e) {
	// Check if capacity exists for event and throw exception if not
	if (n >= capacity) {
		char buf[128];
		snprintf(buf, 128, "SampleQueue::Error: Cannot add event: Queue is full (capacity: %d, length: %d)", capacity, n);
		throw SampleQueue::Error(buf);
	}
	// Add event
	heap[n] = e;
	++n;
	double key = e.time;
	int i;
	for(i=n-1;i>0 && key < heap[(i-1)/2].time;i = (i-1)/2) {
		heap[i] = heap[(i-1)/2];
		places[heap[i].var] = i;
	}
	heap[i] = e;
	places[e.var] = i;
}

void SampleQueue::Remove(int var) {
	int i = places[var];
	if (i==0) {
		--n;
		if (n>0) {
			heap[0] = heap[n];
			places[heap[0].var] = 0;
			Heapify(0,heap[0].time);
		}
	} else {
		i = Heapify(i,INFINITY);
		--n;
		if (i==n) return;
		Event temp = heap[n];
		while(i>0 && temp.time < heap[(i-1)/2].time) {
			heap[i] = heap[(i-1)/2];
			places[heap[i].var] = i;
			i = (i-1)/2;
		}
		heap[i] = temp;
		places[temp.var] = i;
	}
}

int SampleQueue::Heapify(int i, double tempk) {
	int l,r,small;
	double smallt;

	while(i<n) {
		l = i*2+1;
		r = i*2+2;
		if (l < n && (heap[l].time < tempk || std::isinf(tempk))) {
			small = l;
			smallt = heap[l].time;
		} else {
			small = i;
			smallt = tempk;
		}
		if (r < n && heap[r].time < smallt) {
			small = r;
			smallt = heap[r].time;
		}
		if (small==i) {
			places[heap[i].var] = i;
			return i;
		}
		Event temp = heap[i];
		heap[i] = heap[small];
		heap[small] = temp;

		places[heap[i].var] = i;
		i = small;
	}
	return i; // shouldn't get here
}

} // end of ctbn namespace
